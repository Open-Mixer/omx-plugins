#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * live-faces-test.mjs -- omx-fbs and omx-hrp as effects, their notches on the curve (spec 7.7). For each face:
 *
 *   1. its committed words are what tools/live-faces.mjs writes (descriptor, panel, TTLs);
 *   2. its port table is the C enum of its face, in order (a port the host wires by index cannot drift);
 *   3. omx-panel lays the descriptor out under the panel as the eq family with ONE eq_response display whose basis is
 *      exact, every band_freq role carrying the planter that owns the bands, and every report control read-only,
 *      so a surface draws a mark per band and drags none;
 *   4. its TTL declares the worker as optional, so a host with none still loads it, and the report ports as outputs.
 *
 * `--self-test`: an origin dropped from the panel, a report control made writable, the display's basis made
 * `modelled` and two ports of the C enum swapped must each be red.
 *
 * `make live-faces`, part of `make test`. Needs omx-panel >= 0.11.1 (the panel's `origins`) and the installed omx-contract.
 */
import { readFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { FACES, controlTable, descriptorOf, filesOf, panelOf } from '../live-faces.mjs';
import { locateContract } from '../omx-contract.mjs';
import { panelLayout } from '../omx-panel.mjs';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
const c = locateContract();
if (!c.dir) { console.error(`live-faces-test: ${c.why}`); process.exit(1); }

/** the C face's port enum, as symbols in index order: HRP_PORT_REPORT_BASE and the count are not ports */
function enumSymbols(stem, text) {
  const kernel = FACES[stem].kernel.toUpperCase();
  const body = new RegExp(`enum \\{([^}]*)\\};`).exec(text)?.[1] ?? '';
  return [...body.matchAll(new RegExp(`${kernel}_PORT_([A-Z_]+)`, 'g'))].map((m) => m[1].toLowerCase()).filter((s) => s !== 'report_base');
}

/** the failures of one face over (committed texts, descriptor, panel, C source, TTL) */
function failures(stem, { committed, descriptor, panel, source, ttl }) {
  const out = [];
  const f = FACES[stem];
  const want = filesOf(stem, c.dir);
  for (const [rel, text] of Object.entries(want)) if (committed[rel] !== text) out.push(`${rel} is not what tools/live-faces.mjs writes`);

  const { table, slots } = controlTable(stem, c.dir);
  const wired = ['in', 'out', ...table.filter((x) => !x.report).map((x) => x.symbol)];
  const have = enumSymbols(stem, source);
  if (JSON.stringify(wired) !== JSON.stringify(have)) out.push(`the C enum's ports (${have.join(' ')}) are not the table's (${wired.join(' ')})`);
  if (!new RegExp(`${f.kernel.toUpperCase()}_PORT_REPORT_BASE`).test(source)) out.push('the C face has no HRP/FBS_PORT_REPORT_BASE');
  const reportPorts = table.filter((x) => x.report).length;
  if (reportPorts !== 4 * slots) out.push(`${reportPorts} report ports for ${slots} slots`);

  let layout;
  try {
    layout = panelLayout(descriptor, { panel, ns: `fx.${f.kernel}`, where: `plugins/${stem}` });
  } catch (e) {
    return [...out, `omx-panel refuses the panel: ${e.message}`];
  }
  if (layout.family !== 'eq') out.push(`the family is ${layout.family}, not eq`);
  const displays = layout.displays.filter((d) => d.kind === 'eq_response');
  if (displays.length !== 1) out.push(`${displays.length} eq_response displays, want 1`);
  else if (displays[0].basis !== 'exact') out.push(`the display's basis is ${displays[0].basis}, not exact`);
  const freqs = layout.roles.filter((r) => r.role === 'band_freq');
  if (freqs.length !== slots) out.push(`${freqs.length} band_freq roles for ${slots} slots`);
  for (const r of freqs) if (r.origin !== f.origin) out.push(`${r.symbol} has the origin ${r.origin}, not ${f.origin}`);
  for (const role of ['band_gain', 'band_q', 'band_on']) if (layout.roles.filter((r) => r.role === role).length !== slots) out.push(`${role} is not bound for every slot`);
  for (const x of table.filter((t) => t.report)) {
    const d = descriptor.controls.find((k) => k.symbol === x.symbol);
    if (d?.readonly !== true) out.push(`${x.symbol} is writable: a band on a report control is drawn and never dragged`);
  }

  if (!/lv2:optionalFeature[^;]*work:schedule/.test(ttl)) out.push('the TTL does not declare work:schedule as optional');
  if (/lv2:requiredFeature[^;]*work:schedule/.test(ttl)) out.push('the TTL requires work:schedule: a host with no worker could not load it');
  const outs = (ttl.match(/a lv2:ControlPort , lv2:OutputPort/g) ?? []).length;
  if (outs !== reportPorts + 1) out.push(`${outs} output control ports, want ${reportPorts + 1} (the report and analysing)`);
  return out;
}

function load(stem) {
  const base = `plugins/${stem}`;
  const committed = {};
  for (const rel of Object.keys(filesOf(stem, c.dir))) committed[rel] = readFileSync(join(ROOT, rel), 'utf8');
  return {
    committed,
    descriptor: JSON.parse(committed[`${base}/${stem}.descriptor.json`]),
    panel: JSON.parse(committed[`${base}/${stem}.panel.json`]),
    source: readFileSync(join(ROOT, base, `omx_${FACES[stem].kernel}_lv2.c`), 'utf8'),
    ttl: committed[`${base}/generated/${stem}.lv2/${stem}.ttl`],
  };
}

const clone = (o) => JSON.parse(JSON.stringify(o));
let bad = 0;
for (const stem of Object.keys(FACES)) {
  const inputs = load(stem);
  const real = failures(stem, inputs);
  for (const m of real) console.log(`FAIL ${stem}: ${m}`);
  if (!real.length) console.log(`PASS ${stem}: report, panel, marks and ports`);
  bad += real.length;

  if (process.argv.includes('--self-test')) {
    const sab = {
      'an origin dropped': () => { const i = clone(inputs); delete i.panel.origins[Object.keys(i.panel.origins)[0]]; return i; },
      'a report control made writable': () => { const i = clone(inputs); delete i.descriptor.controls.find((k) => k.symbol === 'band1_freq').readonly; return i; },
      'the basis made modelled': () => { const i = clone(inputs); delete i.panel.sections[0].basis; return i; },
      'two ports of the C enum swapped': () => { const i = clone(inputs); i.source = i.source.replace(/(\w+_PORT_)IN = 0,\n\s+\w+_PORT_OUT,/, '$1OUT = 0,\n  $1IN,'); return i; },
      'the worker made required': () => { const i = clone(inputs); i.ttl = i.ttl.replace('lv2:optionalFeature lv2:hardRTCapable , work:schedule', 'lv2:requiredFeature work:schedule'); return i; },
    };
    for (const [name, make] of Object.entries(sab)) {
      const made = make();
      const changed = JSON.stringify(made) !== JSON.stringify(inputs);
      const red = changed && failures(stem, made).length > 0;
      console.log(`${red ? 'PASS' : 'FAIL'} ${stem} self-test: ${name} is ${red ? 'red' : changed ? 'NOT red' : 'not applied'}`);
      bad += red ? 0 : 1;
    }
  }
}
process.exit(bad ? 1 : 0);
