#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * filter-response-check.mjs — an EQ descriptor states the response of every filter whose shape is fixed (omx-contract
 * docs/descriptors.md "Filter responses"; omx-plugins#218): the HPF and LPF's on switch, frequency and slope, and the
 * graphic EQ's bands. A band whose shape a type choice picks states none, on any of its controls: its response is the
 * choice's value. The words are the installed schema's `$defs.filterResponse`, read by tools/descriptor.mjs.
 * `make filter-response`, part of `make test`.
 *
 *   node tools/test/filter-response-check.mjs               the shipped descriptors
 *   node tools/test/filter-response-check.mjs --self-test   the arms, each red, then the shipped descriptors
 * Exit: 0 pass, 1 refused.
 */
import { readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { FILTER_RESPONSES, filterResponses, responseOfControl, typedBand } from '../descriptor.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const load = (stem) => JSON.parse(readFileSync(join(root, 'plugins', stem, `${stem}.descriptor.json`), 'utf8'));
const edges = (on, freq, slope) => Object.fromEntries(['hpf', 'lpf'].flatMap((f) => [on, freq, slope].map((s) => [f + s, f === 'hpf' ? 'highpass' : 'lowpass'])));
const EQ = edges('_on', '_freq', '_slope');
/** Each EQ's controls that state a response, and no others. */
export const WANT = {
  'omx-eq8': EQ, 'omx-eq16': EQ, 'omx-eq32': EQ,
  'omx-strip': edges('On', 'Freq', 'Slope'),
  'omx-geq': Object.fromEntries(Array.from({ length: 31 }, (_, i) => [`band${String(i + 1).padStart(2, '0')}`, 'peaking'])),
  'omx-dyneq': {},
};

/** The refusals of descriptor `d`, which is to state `want` and nothing else. */
export function problems(stem, d, want, words = FILTER_RESPONSES) {
  const out = [];
  for (const c of d.controls) {
    if (c.response !== want[c.symbol]) out.push(`${stem}: '${c.symbol}' states ${c.response ?? 'no response'}, not ${want[c.symbol] ?? 'none'}`);
    if (c.response !== undefined && !words.includes(c.response)) out.push(`${stem}: '${c.symbol}' states '${c.response}', no word of the schema's $defs.filterResponse`);
  }
  for (const sym of Object.keys(want)) if (!d.controls.some((c) => c.symbol === sym)) out.push(`${stem}: no control '${sym}'`);
  return out;
}

const bad = [];
for (const [stem, want] of Object.entries(WANT)) bad.push(...problems(stem, load(stem), want));

if (process.argv.includes('--self-test')) {
  const red = (name, errors) => { if (!errors.length) bad.push(`arm '${name}' passed`); };
  const throws = (name, f) => { try { f(); bad.push(`arm '${name}' passed`); } catch { /* refused, as it should be */ } };
  const change = (sym, response) => { const d = load('omx-eq8'); d.controls.find((c) => c.symbol === sym).response = response; return problems('omx-eq8', d, EQ); };
  red('an edge with no response', change('hpf_freq', undefined));
  red('an edge with the other edge\'s word', change('hpf_freq', 'lowpass'));
  red('a response on a typed band', change('b1_freq', 'peaking'));
  red('a word the schema does not list', problems('omx-eq8', load('omx-eq8'), EQ, FILTER_RESPONSES.filter((w) => w !== 'highpass')));
  throws('a schema with no filterResponse', () => filterResponses({ $defs: {} }));
  throws('the generator takes a word the schema does not list', () => responseOfControl({ symbol: 'x', response: 'lowCut' }, 'arm'));
  throws('the generator takes a response on a per-band control beside a type choice', () => responseOfControl({ symbol: 'b1_freq', response: FILTER_RESPONSES[0] }, 'arm', true));
  for (const [type, on, off] of [['b1_type', 'b1_freq', 'hpf_freq'], ['eq1Type', 'eq1Freq', 'lpfOn'], ['bandType1', 'freq1', 'freq2']]) {
    const params = [{ symbol: type, ref: 'EQ_BAND_TYPES' }, { symbol: on, ref: 'EQ_FREQ_RANGE' }, { symbol: off, ref: 'EQ_FREQ_RANGE' }];
    if (!typedBand(params[1], params)) bad.push(`arm: ${on} is not on ${type}'s band`);
    if (typedBand(params[2], params)) bad.push(`arm: ${off} is on ${type}'s band`);
  }
  if (responseOfControl({ symbol: 'x' }, 'arm') !== undefined) bad.push('arm: a control with no response was given one');
  if (responseOfControl({ symbol: 'x', response: FILTER_RESPONSES[0] }, 'arm') !== FILTER_RESPONSES[0]) bad.push('arm: a stated word was not kept');
}
for (const e of bad) console.error(`FAIL filter-response ${e}`);
if (bad.length) process.exit(1);
console.log(`PASS filter-response: ${Object.values(WANT).reduce((n, w) => n + Object.keys(w).length, 0)} controls of ${Object.keys(WANT).length} EQs state theirs, and no other does`);
