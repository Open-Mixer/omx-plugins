#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * meanings-check.mjs — every authored control states its meaning (omx-contract docs/descriptors.md "Meanings", ruling R1
 * on omx-contract#61: declare, don't infer; omx-plugins#208): each control of every shipped descriptor carries a word
 * of the installed schema's $defs.meaning, never `unknown`; and the renderer refuses a control that states none
 * (tools/descriptor.mjs meaningOfControl, which descriptorOf calls for every control). `make meanings`, part of
 * `make test`.
 *
 *   node tools/test/meanings-check.mjs               the shipped descriptors
 *   node tools/test/meanings-check.mjs --self-test   the renderer's refusals, one arm each: a control with no meaning,
 *                                                    `unknown`, a word the schema does not list, and a word beside a
 *                                                    role that plays another; a role's word, a stated word and a
 *                                                    described control's word each pass; then the shipped descriptors
 * Exit: 0 pass, 1 refused.
 */
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { MEANINGS, UNKNOWN_MEANING, meaningOfControl } from '../descriptor.mjs';
import { isMain } from '../omx-contract.mjs';
import { missingMeanings, shipped } from '../meanings.mjs';

/** The refusals of the shipped descriptors: a control with no meaning, `unknown`, or a word the schema does not list. */
export function problems(root) {
  const out = [];
  for (const [stem, { missing }] of Object.entries(missingMeanings(root))) for (const sym of missing) out.push(`plugins/${stem}: control '${sym}' states no meaning (or ${UNKNOWN_MEANING})`);
  for (const stem of shipped(root)) {
    const d = JSON.parse(readFileSync(join(root, 'plugins', stem, `${stem}.descriptor.json`), 'utf8'));
    for (const c of d.controls) if (c.meaning !== undefined && !MEANINGS.includes(c.meaning)) out.push(`plugins/${stem}: control '${c.symbol}' states '${c.meaning}', no word of the installed schema's $defs.meaning`);
  }
  return out;
}

/** One arm: `run` refuses (throws) when `refuses`, else returns `want`. */
function arm(name, run, refuses, want) {
  try {
    const got = run();
    if (refuses) return `${name}: passed, with '${got}'`;
    return got === want ? undefined : `${name}: '${got}', not '${want}'`;
  } catch (e) {
    return refuses ? undefined : `${name}: refused: ${e.message}`;
  }
}

function selfTest() {
  const d = { stem: 'omx-x' };
  const described = { stem: 'omx-x', descriptor: { controls: [{ meaning: 'gain' }] } };
  const p = (meaning) => ({ symbol: 'x', ...(meaning === undefined ? {} : { meaning }) });
  const arms = [
    ['a control with no meaning', () => meaningOfControl(d, p(), 0, undefined), true],
    [`a control stating ${UNKNOWN_MEANING}`, () => meaningOfControl(d, p(UNKNOWN_MEANING), 0, undefined), true],
    ['a word the schema does not list', () => meaningOfControl(d, p('octaves'), 0, undefined), true],
    ['a stated word beside a role that plays another', () => meaningOfControl(d, p('gain'), 0, 'threshold'), true],
    ['a role\'s word', () => meaningOfControl(d, p(), 0, 'threshold'), false, 'threshold'],
    ['a stated word', () => meaningOfControl(d, p('gain'), 0, undefined), false, 'gain'],
    ['a described control\'s word', () => meaningOfControl(described, p(), 0, undefined), false, 'gain'],
    ['a described control\'s role over its stale word', () => meaningOfControl(described, p(), 0, 'threshold'), false, 'threshold'],
  ];
  const bad = arms.map(([name, run, refuses, want]) => arm(name, run, refuses, want)).filter(Boolean);
  for (const l of bad) console.error(`meanings self-test: ${l}`);
  if (!bad.length) console.log(`meanings self-test: ${arms.filter((a) => a[2]).length} arms refuse, ${arms.filter((a) => !a[2]).length} pass`);
  return bad.length;
}

function main(argv) {
  const root = join(import.meta.dirname, '..', '..');
  let bad = argv.includes('--self-test') ? selfTest() : 0;
  const p = problems(root);
  for (const l of p) console.error(`meanings: ${l}`);
  const m = missingMeanings(root);
  if (!p.length) console.log(`meanings: ${Object.values(m).reduce((n, x) => n + x.total, 0)} controls of ${Object.keys(m).length} descriptors, each a word of the installed schema's ${MEANINGS.length}, none ${UNKNOWN_MEANING}`);
  bad += p.length;
  return bad ? 1 : 0;
}

if (isMain(import.meta.url)) process.exit(main(process.argv.slice(2)));
