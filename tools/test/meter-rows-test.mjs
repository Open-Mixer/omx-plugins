#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * meter-rows-test.mjs — the meter words `held` and `clip` of a declaration reach the face rows tools/gen.mjs
 * emits (include/omx_plugin_face.h's struct omx_face_meter): held unless the meter says `held: false`, a clip word
 * on a level the face measures unless it says `clip: false`, none on a reading of the kernel's, and a clip word on
 * a gain reduction refused. The generated plugins are held to the same: the comp's gain reduction holds and
 * carries no clip, its input and output levels hold and carry one.
 */
import { readFileSync } from 'node:fs';
import { meterRows } from '../gen.mjs';
import { ROOT } from '../omx-contract.mjs';
import { join } from 'node:path';

let bad = 0;
const is = (what, got, want) => {
  const ok = JSON.stringify(got) === JSON.stringify(want);
  if (!ok) bad++;
  console.log(`${ok ? 'ok  ' : 'FAIL'} meter-rows ${what}${ok ? '' : `: got ${JSON.stringify(got)}, want ${JSON.stringify(want)}`}`);
};
const rows = (meters) => meterRows({ stem: 't', kernel: 'comp', params: [], meters }).map((r) => [r.meterHeld, r.meterClip]);
const gr = { id: 'tGr', kind: 'gain_reduction', field: 'st.gr_min_db', readout: 'GAIN_REDUCTION_DB' };
const lv = { id: 'tIn', kind: 'level', source: 'input', min: -60, max: 6 };

is('defaults: a level holds and clips, a reduction holds', rows([gr, lv]), [['1u', '0u'], ['1u', '1u']]);
is('clip: false on a level', rows([{ ...lv, clip: false }]), [['1u', '0u']]);
is('held: false: no hold, and so no clip word', rows([{ ...lv, held: false }, { ...gr, held: false }]), [['0u', '0u'], ['0u', '0u']]);
for (const [what, m] of [['a clip word on a gain reduction', { ...gr, clip: true }], ['a held that is not a boolean', { ...lv, held: 'yes' }],
  ['a clip word on a meter held: false', { ...lv, held: false, clip: true }]]) {
  let refused = false;
  try { meterRows({ stem: 't', kernel: 'comp', params: [], meters: [m] }); } catch { refused = true; }
  is(`${what} is refused`, refused, true);
}
const comp = JSON.parse(readFileSync(join(ROOT, 'plugins/omx-comp/omx-comp.decl.json'), 'utf8'));
const cs = readFileSync(join(ROOT, 'plugins/omx-comp/omx_comp_clap.c'), 'utf8');
is('the generated comp face states the words', [...cs.matchAll(/OMX_FACE_METER_(?:READ|IN|OUT), ([01]u), ([01]u)\}/g)].map((x) => [x[1], x[2]]),
   comp.meters.map((m) => ('read' in m || m.kind === 'gain_reduction' ? ['1u', '0u'] : ['1u', '1u'])));
process.exit(bad ? 1 : 0);
