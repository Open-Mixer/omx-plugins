#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * choice-units-check.mjs — the unit of a choice's values is the descriptor's (omx-contract docs/descriptors.md
 * "choiceUnits"): an object from the choice's name to its PARAM_UNITS id, stated by every descriptor whose declaration
 * gives a choice a unit, and handed to omx-panel as no option beside it (omx-panel#68). `make choice-units`, part of
 * `make test`.
 *   node tools/test/choice-units-check.mjs [--self-test]
 */
import { readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const slopes = { 'HPF Slope': 'dBPerOct', 'LPF Slope': 'dBPerOct' };
const WANT = { 'omx-eq8': slopes, 'omx-eq16': slopes, 'omx-eq32': slopes, 'omx-strip': slopes, 'omx-geq': undefined };
/** The refusals of descriptor `d`, which is to state `want`: each key names a choice of it. */
export function problems(stem, d, want) {
  const out = JSON.stringify(d.choiceUnits) === JSON.stringify(want) ? [] : [`${stem}: choiceUnits is ${JSON.stringify(d.choiceUnits)}, not ${JSON.stringify(want)}`];
  for (const name of Object.keys(d.choiceUnits ?? {})) if (!d.controls.some((c) => c.name === name && c.kind === 'choice')) out.push(`${stem}: choiceUnits names '${name}', no choice of it`);
  return out;
}
const load = (stem) => JSON.parse(readFileSync(join(root, 'plugins', stem, `${stem}.descriptor.json`), 'utf8'));
const bad = Object.entries(WANT).flatMap(([stem, want]) => problems(stem, load(stem), want));
const text = readFileSync(join(root, 'tools', 'descriptor.mjs'), 'utf8');
const optioned = (t) => /extras: \{[^}]*choiceUnits/.test(t);
if (optioned(text)) bad.push('tools/descriptor.mjs hands choiceUnits beside the descriptor');
if (process.argv.includes('--self-test')) {
  const d = { controls: [{ name: 'HPF Slope', kind: 'choice' }], choiceUnits: { 'HPF Slope': 'dBPerOct' } };
  if (problems('arm', d, { 'HPF Slope': 'dBPerOct' }).length) bad.push('arm: a stated unit was refused');
  for (const [name, x, want] of [['no choiceUnits', { ...d, choiceUnits: undefined }, d.choiceUnits], ['another unit', { ...d, choiceUnits: { 'HPF Slope': 'dB' } }, d.choiceUnits], ['a name of no choice', { ...d, choiceUnits: { x: 'dB' } }, { x: 'dB' }]])
    if (!problems('arm', x, want).length) bad.push(`arm '${name}' passed`);
  if (!optioned('return { descriptor, extras: { choiceUnits, byKind } };')) bad.push('arm: the option form passed');
}
for (const e of bad) console.error(`FAIL choice-units ${e}`);
if (bad.length) process.exit(1);
console.log(`PASS choice-units: ${Object.values(WANT).filter(Boolean).length} descriptors state their choices' units`);
