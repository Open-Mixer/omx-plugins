#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas
/*
 * band-port-groups-check.mjs — every plugin whose model repeats a band holds ONE LV2 port group per band in its
 * GENERATED TTL, read as committed. A host that shows a screen per pg:group (Zynthian: zynthian_lv2.py reads each
 * port's pg:group, zynthian_engine.py makes its screens by it) then shows a band per screen.
 *
 * The bands come from the model, never from the TTL: a control omx-contract counts (its `count`, the kernel's band
 * sheet) is declared once per band, so the n-th parameter of each counted control of one sheet (within one chain
 * element, for a chain) is band n. A sheet with two or more counted controls is a band of several controls (omx-eq8's
 * type, frequency, gain, Q and switch), and these laws hold for it:
 *
 *   1. every port of a band names exactly one pg:group;
 *   2. the ports of a band name the same group;
 *   3. no two bands name the same group;
 *   4. a band's group holds that band's ports and no other port, so the band's own switch is in it and nothing else is.
 *
 * A sheet with ONE counted control (omx-geq's 31 faders, omx-multiband's crossovers) repeats a control, not a group:
 * a group each would be a screen of one control, so those ports stay in their section's group and only law 1 holds.
 *
 * And of every plugin, banded or not:
 *
 *   5. the TTL is Turtle a host's parser reads (serdi, strict: the parser lilv reads a bundle with);
 *   6. every node a port names by pg:group is a pg:Group stating one lv2:symbol (an LV2 symbol, unique among the
 *      plugin's groups), one lv2:name and one rdfs:label.
 *
 * `--self-test` adds the sabotage arms, each on a scratch copy of omx-eq8's TTL with one thing changed, each of which
 * must go red on its own law: two bands sharing a group, a band port with no group, a band port moved out of its
 * band's group, a group with no lv2:symbol, two groups with one symbol, and a TTL that is not Turtle.
 *
 * `make band-groups`, part of `make test`. It reads files only: a stale or hand-edited file is gen.mjs --check's.
 */
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { loadDecl, pluginDirs } from '../gen.mjs';
import { triplesOf } from './resource-hints-check.mjs';

const LV2 = 'http://lv2plug.in/ns/lv2core#';
const PG = 'http://lv2plug.in/ns/ext/port-groups#';
const RDF_TYPE = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#type';
const RDFS_LABEL = 'http://www.w3.org/2000/01/rdf-schema#label';

/**
 * The bands of declaration `d`, from its model: `[{ sheet, element, controls, bands }]`, one entry per count sheet
 * (per chain element), `controls` the counted controls' names and `bands[n - 1]` the parameter symbols of band n.
 */
export function bandsOf(d) {
  const sheets = new Map();
  d.params.forEach((p, i) => {
    const c = d.controls?.[i];
    if (!c?.count) return;
    const key = JSON.stringify([p.element ?? null, c.count]);
    if (!sheets.has(key)) sheets.set(key, { sheet: c.count, element: p.element ?? null, byControl: new Map() });
    const by = sheets.get(key).byControl;
    if (!by.has(c.name)) by.set(c.name, []);
    by.get(c.name).push(p.symbol);
  });
  return [...sheets.values()].map(({ sheet, element, byControl }) => {
    const lists = [...byControl.values()];
    const count = Math.max(...lists.map((l) => l.length));
    return { sheet, element, controls: [...byControl.keys()], bands: Array.from({ length: count }, (_x, b) => lists.map((l) => l[b]).filter((s) => s !== undefined)) };
  });
}

/** What the TTL states: `ports`, each control port's symbol with the groups it names, and `groups`, each named node. */
export function readGroups(d, ttlText) {
  const triples = triplesOf(ttlText, `file:///${d.stem}.lv2/${d.stem}.ttl`);
  const of = (s, p) => triples.filter((t) => t.s === s && t.p === p).map((t) => t.o);
  const ports = new Map();
  for (const node of of(`<${d.lv2.uri}>`, `${LV2}port`)) {
    const symbol = of(node, `${LV2}symbol`)[0]?.lex;
    if (symbol !== undefined) ports.set(symbol, of(node, `${PG}group`));
  }
  const groups = new Map();
  for (const named of ports.values()) {
    for (const g of named) {
      if (groups.has(g)) continue;
      groups.set(g, {
        isGroup: of(g, RDF_TYPE).includes(`<${PG}Group>`),
        symbols: of(g, `${LV2}symbol`).map((o) => o.lex),
        names: of(g, `${LV2}name`).map((o) => o.lex),
        labels: of(g, RDFS_LABEL).map((o) => o.lex),
      });
    }
  }
  return { ports, groups };
}

/** The laws of the header on one plugin's TTL text: `[error, ...]`, each naming its law. */
export function bandGroupErrors(d, ttlText) {
  let read;
  try {
    read = readGroups(d, ttlText);
  } catch (e) {
    return [`${d.stem}: law 5: its TTL is not Turtle serdi reads: ${String(e.stderr || e.message).trim().split('\n')[0]}`];
  }
  const { ports, groups } = read;
  const errors = [];
  const members = (g) => [...ports].filter(([, named]) => named.includes(g)).map(([s]) => s);
  for (const { sheet, bands, controls } of bandsOf(d)) {
    const several = controls.length > 1;
    const groupOfBand = new Map();
    bands.forEach((band, b) => {
      const what = `${d.stem}: ${sheet} band ${b + 1}`;
      const named = new Set();
      for (const symbol of band) {
        const own = ports.get(symbol);
        if (own === undefined) errors.push(`${what}: law 1: '${symbol}' is no control port of the TTL`);
        else if (own.length !== 1) errors.push(`${what}: law 1: port '${symbol}' names ${own.length} pg:group`);
        else named.add(own[0]);
      }
      if (!several || !named.size) return;
      if (named.size > 1) {
        errors.push(`${what}: law 2: its ports are spread over ${[...named].join(', ')}`);
        return;
      }
      const [g] = named;
      if (groupOfBand.has(g)) errors.push(`${what}: law 3: it shares ${g} with band ${groupOfBand.get(g) + 1}`);
      else {
        groupOfBand.set(g, b);
        // another band's port in it is law 3's (or law 2's) to name, once
        const others = members(g).filter((s) => !bands.some((x) => x.includes(s)));
        if (others.length) errors.push(`${what}: law 4: ${g} also holds ${others.join(' ')}`);
      }
    });
  }
  const bySymbol = new Map();
  for (const [g, node] of groups) {
    const what = `${d.stem}: group ${g}`;
    if (!node.isGroup) errors.push(`${what}: law 6: it is no pg:Group`);
    if (node.symbols.length !== 1) errors.push(`${what}: law 6: it states ${node.symbols.length} lv2:symbol`);
    else if (!/^[A-Za-z_][A-Za-z0-9_]*$/.test(node.symbols[0])) errors.push(`${what}: law 6: '${node.symbols[0]}' is no LV2 symbol`);
    else if (bySymbol.has(node.symbols[0])) errors.push(`${what}: law 6: its lv2:symbol '${node.symbols[0]}' is also ${bySymbol.get(node.symbols[0])}'s`);
    else bySymbol.set(node.symbols[0], g);
    if (node.names.length !== 1 || !node.names[0]) errors.push(`${what}: law 6: it states ${node.names.length} lv2:name`);
    if (node.labels.length !== 1 || !node.labels[0]) errors.push(`${what}: law 6: it states ${node.labels.length} rdfs:label`);
  }
  return errors;
}

/** The sabotage arms: `[what, the law that must refuse it, the change to omx-eq8's TTL text]`. */
const ARMS = [
  ['two bands share a group (band 2\'s ports name band 1\'s)', /law 3: it shares <urn:openmixer:eq8#band1> with band 1/,
    (t) => t.replaceAll('pg:group <urn:openmixer:eq8#band2>', 'pg:group <urn:openmixer:eq8#band1>')],
  ['a band port names no group', /law 1: port 'b3_gain' names 0 pg:group/,
    (t) => t.replace(/(lv2:symbol "b3_gain" ;[^\]]*?) ;\n {8}pg:group <urn:openmixer:eq8#band3>/, '$1')],
  ['a band port is in another group than its band\'s', /band 3: law 2: its ports are spread over /,
    (t) => t.replace(/(lv2:symbol "b3_on" ;[^\]]*?pg:group <urn:openmixer:eq8#)band3>/, '$1hpf>')],
  ['a port that is no band\'s sits in a band\'s group', /band 1: law 4: <urn:openmixer:eq8#band1> also holds hpf_on/,
    (t) => t.replace(/(lv2:symbol "hpf_on" ;[^\]]*?pg:group <urn:openmixer:eq8#)hpf>/, '$1band1>')],
  ['a group states no lv2:symbol', /group <urn:openmixer:eq8#band4>: law 6: it states 0 lv2:symbol/,
    (t) => t.replace('    lv2:symbol "band4" ;\n', '')],
  ['two groups state one lv2:symbol', /law 6: its lv2:symbol 'band5' is also <urn:openmixer:eq8#band5>'s/,
    (t) => t.replace('    lv2:symbol "band6" ;\n', '    lv2:symbol "band5" ;\n')],
  ['the TTL is not Turtle (a statement left open)', /law 5: its TTL is not Turtle serdi reads/,
    (t) => t.replace('    a pg:Group ;\n', '    a pg:Group ;;;\n    ] \n')],
];

if (process.argv[1] && import.meta.url === new URL(`file://${process.argv[1]}`).href) {
  let bad = 0, n = 0;
  const check = (ok, what, detail = []) => {
    n++;
    if (!ok) bad++;
    console.log(`${ok ? 'PASS' : 'FAIL'} band-port-groups ${what}`);
    for (const l of ok ? [] : detail) console.log(`  FAIL ${l}`);
  };
  const ttlOf = (d) => readFileSync(join(d.dir, 'generated', `${d.stem}.lv2`, `${d.stem}.ttl`), 'utf8');
  const plugins = pluginDirs().map((dir) => loadDecl(dir));
  let banded = 0;
  for (const d of plugins) {
    const several = bandsOf(d).filter((s) => s.controls.length > 1), single = bandsOf(d).filter((s) => s.controls.length === 1);
    if (several.length) banded++;
    const said = [...several.map((s) => `${s.bands.length} bands of ${s.controls.length} controls, a group each`),
      ...single.map((s) => `${s.bands.length} of one repeated control (${s.controls[0]}), grouped`)].join('; ') || 'no bands; its groups are whole';
    const errors = bandGroupErrors(d, ttlOf(d));
    check(!errors.length, `${d.stem}: ${said}`, errors.slice(0, 6).concat(errors.length > 6 ? [`... and ${errors.length - 6} more`] : []));
  }
  check(banded >= 5, `the model names the banded plugins (${banded} with a band of several controls; omx-eq8, 16, 32, omx-dyneq, omx-multiband at least)`);
  if (process.argv.includes('--self-test')) {
    const d = plugins.find((p) => p.stem === 'omx-eq8');
    const text = ttlOf(d);
    check(!bandGroupErrors(d, text).length, 'sabotage: the scratch omx-eq8 TTL passes unchanged (each arm below changes one thing)', bandGroupErrors(d, text));
    for (const [what, want, change] of ARMS) {
      const changed = change(text);
      const errors = changed === text ? ['the arm changed nothing'] : bandGroupErrors(d, changed);
      check(errors.some((e) => want.test(e)), `sabotage goes red: ${what}`, errors.length ? errors.slice(0, 3) : ['the check passed it']);
    }
  }
  console.log(bad ? `band-port-groups: ${bad} of ${n} FAILED` : `band-port-groups: all ${n} pass`);
  process.exit(bad ? 1 : 0);
}
