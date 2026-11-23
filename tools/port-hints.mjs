#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * port-hints.mjs — no LV2 face loses a port hint (`make hints`, run by `make test`).
 *
 *   node tools/port-hints.mjs            check every plugin's generated TTL
 *
 * A hint is what a host draws a control from: a port property (logarithmic, integer, enumeration,
 * toggled, ...), a unit, a labelled scale point. Two checks, each naming the plugin, the port and
 * the hint:
 *
 *   - the PIN, the declaration's `portHints` (gen.mjs assembles every plugin's into the view
 *     tools/test/port-hints.json): every hint pinned for a port is still on it. A face may gain
 *     hints; losing one is a capability regression, so it is red until the pin is changed on
 *     purpose, in the declaration. A plugin with no pin is red too: a new face pins its hints
 *     (tools/omx-new-plugin.mjs writes the pin from its first TTL, pinOfTtl below).
 *   - the LAW, from the declaration: a frequency is logarithmic in hertz unless declared linear,
 *     every unit the LV2 units extension names is on its port, and a parameter with labelled
 *     values or points is an enumeration that lists them all.
 */
import { readFileSync } from 'node:fs';
import { basename, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { choicesOf, hintsView, loadDecl, lv2UnitRef, pluginDirs, scaleOf } from './gen.mjs';
import { isMain } from './omx-contract.mjs';

const ROOT = resolve(fileURLToPath(import.meta.url), '..', '..');

/** The hints of each port of a generated TTL, by symbol: `props`, `unit`, `points` ["v label"]. */
export function ttlHints(text) {
  const ports = {};
  // a port opens with its class (`[ a lv2:...`), a scale point with its label
  for (const block of text.split(/\]\s*,\s*\[(?=\s*a\s)/)) {
    const sym = /lv2:symbol\s+"([^"]+)"/.exec(block);
    if (!sym) continue;
    const props = [];
    for (const m of block.matchAll(/lv2:portProperty\s+([^;\]]+)/g)) props.push(...m[1].split(',').map((s) => s.trim()).filter(Boolean));
    const unit = /units:unit\s+(units:\w+|<[^>\s]+>)/.exec(block)?.[1] ?? null;
    const points = [...block.matchAll(/\[\s*rdfs:label\s+"((?:[^"\\]|\\.)*)"\s*;\s*rdf:value\s+(-?[\d.]+)\s*\]/g)].map((m) => `${Number(m[2])} ${m[1]}`);
    ports[sym[1]] = { props: [...new Set(props)].sort(), unit, points };
  }
  return ports;
}

/** Every plugin's stem, a base of variants standing for its variants (tools/variants.mjs). */
export function pluginStems(root = ROOT) {
  return pluginDirs(root).map((p) => basename(p)).sort();
}

const ttlOf = (root, stem) => readFileSync(join(root, 'plugins', stem, 'generated', `${stem}.lv2`, `${stem}.ttl`), 'utf8');

/** The pin of a generated TTL, as a declaration's `portHints` carries it: each port that has a hint,
 * with only the hints it has (no empty list, no null unit). `perBand`, the per-band symbols of a
 * declaration with variants: their `b<n>_` ports collapse onto the one symbol, which must hint
 * every band alike. */
export function pinOfTtl(text, perBand = []) {
  const pin = {};
  for (const [sym, h] of Object.entries(ttlHints(text))) {
    if (!(h.props.length || h.unit || h.points.length)) continue;
    const band = /^b\d+_(.*)$/.exec(sym);
    const key = band && perBand.includes(band[1]) ? band[1] : sym;
    const v = { ...(h.props.length ? { props: h.props } : {}), ...(h.unit ? { unit: h.unit } : {}), ...(h.points.length ? { points: h.points } : {}) };
    if (pin[key] && JSON.stringify(pin[key]) !== JSON.stringify(v)) throw new Error(`port-hints: ${sym} hints otherwise than another band of '${key}'`);
    pin[key] = v;
  }
  return pin;
}

/** Every lost or missing hint, one line each; empty when every face keeps them all. */
export function hintErrors(root = ROOT) {
  const errors = [];
  const pin = hintsView(root);
  for (const stem of pluginStems(root)) {
    const have = ttlHints(ttlOf(root, stem));
    if (!pin[stem]) errors.push(`${stem}: no pinned hints (its declaration's "portHints": tools/omx-new-plugin.mjs writes it from the generated TTL)`);
    for (const [sym, want] of Object.entries(pin[stem] ?? {})) {
      const got = have[sym];
      if (!got) {
        errors.push(`${stem}: port "${sym}" is gone`);
        continue;
      }
      for (const p of want.props) if (!got.props.includes(p)) errors.push(`${stem}: port "${sym}" lost ${p}`);
      if (want.unit && got.unit !== want.unit) errors.push(`${stem}: port "${sym}" lost units:unit ${want.unit}`);
      for (const p of want.points) if (!got.points.includes(p)) errors.push(`${stem}: port "${sym}" lost the scale point ${p}`);
    }
    const d = loadDecl(join(root, 'plugins', stem));
    for (const q of d.params) {
      const got = have[q.symbol];
      if (!got) {
        errors.push(`${stem}: declared parameter "${q.symbol}" has no port`);
        continue;
      }
      if (scaleOf(q) === 'log' && !got.props.includes('pprops:logarithmic')) errors.push(`${stem}: port "${q.symbol}" is not logarithmic`);
      if (lv2UnitRef(q.unit) && got.unit !== lv2UnitRef(q.unit)) errors.push(`${stem}: port "${q.symbol}" has no units:unit ${lv2UnitRef(q.unit)}`);
      const choices = choicesOf(q);
      if (choices) {
        if (!got.props.includes('lv2:enumeration')) errors.push(`${stem}: port "${q.symbol}" is not an enumeration`);
        for (const c of choices) if (!got.points.includes(`${c.value} ${c.label}`)) errors.push(`${stem}: port "${q.symbol}" lists no "${c.label}" (${c.value})`);
      }
    }
  }
  return errors;
}

if (isMain(import.meta.url)) {
  const errors = hintErrors();
  for (const e of errors) console.log(`FAIL ${e}`);
  console.log(errors.length ? `port-hints: ${errors.length} hint(s) lost` : `PASS port-hints: every face keeps its hints (${pluginStems().length} plugins)`);
  process.exit(errors.length ? 1 : 0);
}
