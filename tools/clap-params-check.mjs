#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * clap-params-check.mjs — the BUILT CLAP plugin's parameters, as plugin-probe read them through
 * clap_plugin_params.get_info, against the DECLARATION (never against the generated header): every
 * declared parameter in order with its name and travel (as a float), stepped exactly when it is an
 * integer or a toggle, then the host's bypass last, at omx-contract's permanent id (OMX_PLUGIN_FACE_IDS.bypassClapId),
 * never the id after the last parameter. value_to_text shows the default with its unit as a person reads it, the
 * declared PARAM_UNITS id's x-symbol (50 %, 12 dB/oct, a bare number for none), never the id (C2).
 *
 * Usage: node tools/clap-params-check.mjs plugins/<stem> <clap-params.txt>
 */
import { readFileSync } from 'node:fs';
import { loadDecl } from './gen.mjs';
import { unitSymbol } from './format-tables.mjs';
import { faceIds } from './param-contract.mjs';

const [dir, dump] = process.argv.slice(2);
const d = loadDecl(dir);
const rows = readFileSync(dump, 'utf8').trim().split('\n').map((l) => l.split('\t'));
const STEPPED = 1, BYPASS = 16, AUTOMATABLE = 32;
const want = [
  ...d.params.map((p, i) => [i, p.name, p.min, p.max, p.def, AUTOMATABLE | (p.kind ? STEPPED : 0), unitSymbol(p.unit, d.units)]),
  [faceIds().bypassClapId, 'Bypass', 0, 1, 0, AUTOMATABLE | STEPPED | BYPASS, ''],
];
let bad = 0;
if (rows.length !== want.length) bad++, console.error(`FAIL clap-params: ${rows.length} parameters, the declaration has ${want.length}`);
want.forEach((w, i) => {
  const g = rows[i] ?? [];
  // value_to_text's "<number> <unit>", or the bare number: the unit part is the text after the number
  const text = g[6] ?? '', shown = text.includes(' ') ? text.slice(text.indexOf(' ') + 1) : '';
  const ok = Number(g[0]) === w[0] && g[1] === w[1] && [2, 3, 4].every((k) => Number(g[k]) === Math.fround(w[k])) && Number(g[5]) === w[5] && shown === w[6];
  if (!ok) bad++;
  console.log(`${ok ? 'PASS' : 'FAIL'} clap-params ${w[1]}: ${ok ? 'as declared' : `got [${g.join(', ')}], want [${w.join(', ')}]`}`);
});
process.exit(bad ? 1 : 0);
