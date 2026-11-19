#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * lv2-units-check.mjs — the generated LV2 description states the unit a parameter declares.
 * tools/gen.mjs maps a declared unit to an LV2 unit (plugin-hostd's LV2 table read backwards, and the declarations' own
 * units: tools/format-tables.mjs lv2UnitNames); a unit it did not map was dropped
 * without a word, so the installed omx-limiter's ceilingDb port carried no unit at all. Two laws,
 * each with sabotage arms that must go red (--self-test runs them):
 *
 *   1. every unit omx-contract's parameter contract names (tools/param-contract.mjs UNITS), and every
 *      other unit a declaration uses, has an LV2 unit;
 *   2. every control of every plugin that declares a unit has, on its port in the generated TTL, the
 *      LV2 unit that maps to it, and a unit that is not LV2's own is defined in the same file.
 *
 * `make lv2-units`, part of `make test`. It reads omx-contract as `make test` does.
 */
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { lv2UnitNames, loadDecl, pluginDirs, ROOT } from '../gen.mjs';
import { contractUnits } from '../param-contract.mjs';
import { ttlHints } from '../port-hints.mjs';
import { isMain } from '../omx-contract.mjs';

/** The part of an LV2 unit reference that names it: `units:db` -> db, `<urn:...#dBFS>` -> dBFS. */
const nameOf = (ref) => ref?.replace(/^<|>$/g, '').split(/[:#]/).pop() ?? null;

/** Every defect, one line each: `plugins` is `[{ stem, params, ttl }]`, `map` the unit -> LV2 name table. */
export function unitErrors(plugins, map = lv2UnitNames(), served = Object.entries(contractUnits()).filter(([, [t]]) => t !== 'none').map(([u]) => u)) {
  const errors = [];
  const used = new Set([...served, ...plugins.flatMap((p) => p.params.map((q) => q.unit))].filter(Boolean));
  for (const u of [...used].sort()) if (!map[u]) errors.push(`unit '${u}' has no LV2 unit`);
  for (const { stem, params, ttl } of plugins) {
    const ports = ttlHints(ttl);
    for (const q of params) {
      if (!q.unit || !map[q.unit]) continue;
      const got = ports[q.symbol]?.unit;
      if (!got) errors.push(`${stem}: port "${q.symbol}" declares ${q.unit} and carries no units:unit`);
      else if (nameOf(got) !== map[q.unit]) errors.push(`${stem}: port "${q.symbol}" declares ${q.unit}, the TTL says ${got}, not ${map[q.unit]}`);
      else if (got.startsWith('<') && !ttl.includes(`${got}\n    a units:Unit`)) errors.push(`${stem}: port "${q.symbol}" uses ${got}, which the TTL does not define as a units:Unit`);
    }
  }
  return errors;
}

export function load(root = ROOT) {
  return pluginDirs(root).map((dir) => {
    const d = loadDecl(dir);
    return { stem: d.stem, params: d.params, ttl: readFileSync(join(dir, 'generated', `${d.stem}.lv2`, `${d.stem}.ttl`), 'utf8') };
  });
}

if (isMain(import.meta.url)) {
  const plugins = load();
  const errors = unitErrors(plugins);
  for (const e of errors) console.log(`FAIL lv2-units ${e}`);
  let bad = errors.length;
  if (process.argv.includes('--self-test')) {
    const stripUnits = (ttl) => ttl.replace(/^\s*units:unit [^;]*;\n/gm, '');
    const arms = {
      'a port that lost its units:unit': () => unitErrors(plugins.map((p) => ({ ...p, ttl: stripUnits(p.ttl) }))),
      'a unit with no LV2 unit': () => unitErrors(plugins, Object.fromEntries(Object.entries(lv2UnitNames()).filter(([k]) => k !== 'dBFS'))),
      'a port with another plugin\'s unit': () => unitErrors(plugins.map((p) => ({ ...p, ttl: p.ttl.replace(/units:unit units:hz ;/g, 'units:unit units:ms ;') }))),
      'a custom unit the TTL never defines': () => unitErrors(plugins.map((p) => ({ ...p, ttl: p.ttl.replace(/\n(<[^>\n]+>)\n    a units:Unit/g, '\n$1\n    a rdfs:Resource') }))),
    };
    for (const [what, run] of Object.entries(arms)) {
      const red = run().length > 0;
      console.log(`${red ? 'PASS' : 'FAIL'} lv2-units sabotage: ${what} is red`);
      if (!red) bad++;
    }
  }
  console.log(bad ? `lv2-units: ${bad} failure(s)` : `PASS lv2-units: every declared unit is stated by its port (${plugins.length} plugins)`);
  process.exit(bad ? 1 : 0);
}
