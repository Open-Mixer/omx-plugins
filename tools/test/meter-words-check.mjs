#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * meter-words-check.mjs — every meter of every plugin descriptor states omx-contract's meter words (descriptorMeter,
 * schema 3.11.0) as the declaration spells them (tools/gen.mjs meterWords, the rule the face rows read too): `held`
 * on every meter, true unless the declaration says not; `clip` on every level, true on a held one the face measures,
 * and none on a gain reduction (the contract refuses it there); `measurement` where the plugin knows it, a gain
 * reduction's own and a measured level's peak, and none on a level the kernel reads. A descriptor whose meter lacks a
 * word, carries one the declaration does not, or misses a declared meter, is red. The check's own arms: a descriptor
 * with a held word stripped, a clip word put on a gain reduction, and a measurement dropped, are each refused. Part
 * of `make test`.
 *
 *   node tools/test/meter-words-check.mjs
 */
import { readFileSync, readdirSync } from 'node:fs';
import { join } from 'node:path';
import { ROOT } from '../omx-contract.mjs';
import { loadDecl, metersOf } from '../gen.mjs';

/** The faults of descriptor `d` against the words of declaration `decl`: one line each, none when it holds. */
export function meterWordFaults(stem, d, decl) {
  const want = metersOf(decl);
  const out = [];
  for (const m of d.meters ?? []) {
    const w = want.find((x) => x.id === m.symbol);
    if (!w) {
      out.push(`${stem}: meter ${m.symbol} is none of the declaration's`);
      continue;
    }
    if (typeof m.held !== 'boolean') out.push(`${stem}: meter ${m.symbol} states no held word`);
    else if (m.held !== w.held) out.push(`${stem}: meter ${m.symbol} says held ${m.held}, the declaration ${w.held}`);
    if (m.kind !== 'level') {
      if ('clip' in m) out.push(`${stem}: meter ${m.symbol} is a ${m.kind} with a clip word (omx-contract refuses it)`);
    } else if (typeof m.clip !== 'boolean') out.push(`${stem}: meter ${m.symbol} states no clip word`);
    else if (m.clip !== w.clip) out.push(`${stem}: meter ${m.symbol} says clip ${m.clip}, the declaration ${w.clip}`);
    if (m.measurement !== w.measurement) out.push(`${stem}: meter ${m.symbol} measures ${m.measurement ?? 'nothing stated'}, the declaration ${w.measurement ?? 'nothing stated'}`);
  }
  for (const w of want) if (!(d.meters ?? []).some((m) => m.symbol === w.id)) out.push(`${stem}: the declaration's meter ${w.id} is not in the descriptor`);
  return out;
}

const faults = [];
let meters = 0;
let held = 0;
const arms = {};
const arm = (name, stem, d, decl, edit, says) => {
  if (arms[name]) return;
  const copy = JSON.parse(JSON.stringify(d));
  if (!edit(copy)) return;
  arms[name] = meterWordFaults(stem, copy, decl).some((f) => f.includes(says)) ? 'red' : `not red (${stem})`;
};
for (const stem of readdirSync(join(ROOT, 'plugins')).sort()) {
  let d;
  try {
    d = JSON.parse(readFileSync(join(ROOT, 'plugins', stem, `${stem}.descriptor.json`), 'utf8'));
  } catch {
    continue;
  }
  const decl = loadDecl(join(ROOT, 'plugins', stem));
  faults.push(...meterWordFaults(stem, d, decl));
  meters += (d.meters ?? []).length;
  held += (d.meters ?? []).filter((m) => m.held === true).length;
  const first = (copy, pick) => (copy.meters ?? []).find(pick);
  arm('a held word stripped', stem, d, decl, (c) => { const m = first(c, (x) => x.held === true); return m && delete m.held; }, 'states no held word');
  arm('a clip word on a gain reduction', stem, d, decl, (c) => { const m = first(c, (x) => x.kind === 'gain-reduction'); return m && (m.clip = false, true); }, 'with a clip word');
  arm('a measurement dropped', stem, d, decl, (c) => { const m = first(c, (x) => x.measurement); return m && delete m.measurement; }, 'measures nothing stated');
}
for (const f of faults) console.error(`FAIL meter-words: ${f}`);
if (!meters) faults.push('no descriptor states a meter');
for (const name of ['a held word stripped', 'a clip word on a gain reduction', 'a measurement dropped'])
  if (arms[name] !== 'red') faults.push(`arm: ${name} is ${arms[name] ?? 'not tried: no descriptor to try it on'}`);
for (const f of faults.filter((x) => x.startsWith('arm') || x.startsWith('no '))) console.error(`FAIL meter-words: ${f}`);
console.log(faults.length ? `FAIL meter-words: ${faults.length} fault(s)` : `ok   meter-words: ${meters} descriptor meters state held, clip and measurement as declared (${held} held); each arm is red`);
process.exit(faults.length ? 1 : 0);
