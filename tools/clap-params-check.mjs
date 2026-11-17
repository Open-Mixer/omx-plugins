#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * clap-params-check.mjs — the BUILT CLAP plugin's parameters, as plugin-probe read them through
 * clap_plugin_params.get_info and value_to_text, against the DECLARATION (never against the generated header):
 *
 *   1. AS DECLARED. Every declared parameter in order with its name and travel (as a float), stepped exactly when it
 *      is an integer or a toggle, an enumeration (CLAP_PARAM_IS_ENUM, its value texts the declared labels) exactly
 *      when it is a choice whose points are every whole step of its travel, then the host's bypass last, at
 *      omx-contract's permanent id (OMX_PLUGIN_FACE_IDS.bypassClapId), never the id after the last parameter.
 *   2. AS A HOST READS IT. A describing host reads a scanned CLAP parameter's kind by omx-contract's one rule
 *      (docs/descriptors.md, "A scanned CLAP parameter's kind"), held by the vectors the installed omx-contract
 *      ships, share/omx-contract/clap-kind-vectors.json. Each built parameter whose words (flags, range, default,
 *      value texts) are a vector's input must be read as the kind the declaration gives it: a two-option choice
 *      built as a plain stepped 0..1 is the toggle vectors' input and reads as a toggle (Open-Mixer/omx-plugins#97),
 *      and a toggle built as an enumeration reads as a choice. The rule is never restated here: a parameter no
 *      vector's input covers (a stepped travel over a wider range, a three-way choice) is noted, not judged, and
 *      every declared toggle and every declared choice over 0..1 must be one some vector covers.
 *      The vectors with a contract entry are read as the flag's: our faces' contract entries restate the flags
 *      (scale stepped, step 1, exactly when CLAP_PARAM_IS_STEPPED; tools/clap-ext-check.mjs holds that).
 *
 * Usage: node tools/clap-params-check.mjs plugins/<stem> <clap-params.txt>
 *   clap-params.txt: `id name min max default flags texts`, the clap-probe `params` dump; texts are the value texts of
 *   a stepped parameter's whole steps, `|`-joined (at most 16 steps), and a text that is not the value's number, with
 *   or without the unit, is a point (a label), what omx-clap-scan carries as scale points.
 * The vectors file is $OMX_CONTRACT_KIND_VECTORS (a checkout's copy standing in for the install, as OMX_CONTRACT_SCHEMA
 * does for the schema), else clap-kind-vectors.json in $OMX_CONTRACT_DIR, else in the installed omx-contract's data
 * directory (it ships from omx-contract 3.19.0, Open-Mixer/omx-contract#67). With none, part 2 is a NOTE, never a
 * silent pass: tools/test/probe-checks-selftest.mjs, the gate of this rule, is red by name without the vectors.
 */
import { existsSync, readFileSync } from 'node:fs';
import { join } from 'node:path';
import { enumLabelsOf, loadDecl } from './gen.mjs';
import { installedDataDir } from './omx-contract.mjs';
import { faceIds } from './param-contract.mjs';

const [dir, dump] = process.argv.slice(2);
const d = loadDecl(dir);
const rows = readFileSync(dump, 'utf8').trim().split('\n').map((l) => l.split('\t'));
const STEPPED = 1, ENUM = 1 << 16, BYPASS = 16, AUTOMATABLE = 32; // clap/ext/params.h: CLAP_PARAM_IS_ENUM is bit 16
const FLAG_NAMES = { [STEPPED]: 'CLAP_PARAM_IS_STEPPED', [ENUM]: 'CLAP_PARAM_IS_ENUM', [BYPASS]: 'CLAP_PARAM_IS_BYPASS', [AUTOMATABLE]: 'CLAP_PARAM_IS_AUTOMATABLE' };
const VECTORS_FILE = 'clap-kind-vectors.json';
let bad = 0;
const fail = (m) => (bad++, console.log(`FAIL clap-params ${m}`));
const textsOf = (g) => (g[6] ? g[6].split('|') : []);

// ---- 1. as declared ----
const want = [
  ...d.params.map((p, i) => [i, p.name, p.min, p.max, p.def, AUTOMATABLE | (p.kind ? STEPPED : 0) | (enumLabelsOf(p) ? ENUM : 0), enumLabelsOf(p) ?? []]),
  [faceIds().bypassClapId, 'Bypass', 0, 1, 0, AUTOMATABLE | STEPPED | BYPASS, []],
];
if (rows.length !== want.length) bad++, console.error(`FAIL clap-params: ${rows.length} parameters, the declaration has ${want.length}`);
want.forEach((w, i) => {
  const g = rows[i] ?? [];
  const ok = Number(g[0]) === w[0] && g[1] === w[1] && [2, 3, 4].every((k) => Number(g[k]) === Math.fround(w[k])) && Number(g[5]) === w[5];
  const labelsOk = !w[6].length || w[6].join('|') === textsOf(g).join('|');
  if (!ok || !labelsOk) bad++;
  const why = ok ? (labelsOk ? 'as declared' : `value texts [${textsOf(g).join(', ')}], the declared labels are [${w[6].join(', ')}]`) : `got [${g.slice(0, 6).join(', ')}], want [${w.slice(0, 6).join(', ')}]`;
  console.log(`${ok && labelsOk ? 'PASS' : 'FAIL'} clap-params ${w[1]}: ${why}`);
});

// ---- 2. as a host reads it: omx-contract's vectors ----
const vectorsDir = process.env.OMX_CONTRACT_DIR || installedDataDir();
const vectorsFile = process.env.OMX_CONTRACT_KIND_VECTORS || (vectorsDir && join(vectorsDir, VECTORS_FILE));
if (!vectorsFile || !existsSync(vectorsFile)) {
  console.log(`NOTE clap-params ${d.stem}: no ${vectorsFile ?? VECTORS_FILE} (a describing host's kind rule is omx-contract >= 3.19.0's, Open-Mixer/omx-contract#67): the parameters are held as declared only, not as a host reads them; tools/test/probe-checks-selftest.mjs is red without it`);
} else {
  const { vectors } = JSON.parse(readFileSync(vectorsFile, 'utf8'));
  const isPoint = (t, v, unit) => t !== `${v}` && t !== `${v} ${unit}`;
  const flagSet = (bits) => Object.keys(FLAG_NAMES).filter((b) => Number(bits) & Number(b)).map((b) => FLAG_NAMES[b]).sort().join(' ');
  const sameFlags = (v, g) => [...v.input.flags].sort().join(' ') === flagSet(g[5]);
  const kindOf = (p) => (p.kind === 'toggle' ? 'toggle' : enumLabelsOf(p) ? 'choice' : 'travel');
  let held = 0;
  d.params.forEach((p, i) => {
    const g = rows[i];
    if (!g) return;
    const [min, max, def] = [2, 3, 4].map((k) => Number(g[k]));
    const texts = textsOf(g);
    const points = texts.some((t, k) => isPoint(t, min + k, p.unit));
    // the vector this parameter is the input of: the flags, the range and the points as built; the default when the
    // vector reads one (a toggle's side); the contract entry the flag's (above)
    const covering = vectors.filter((v) => sameFlags(v, g) && v.input.min === min && v.input.max === max && v.input.points === points && v.input.contract === null
      && (v.expect.default === undefined || v.input.default === def));
    const declared = kindOf(p);
    const decisive = declared === 'toggle' || (declared === 'choice' && min === 0 && max === 1);
    const built = `flags [${flagSet(g[5])}], ${min}..${max} default ${def}, ${points ? 'points' : 'no points'}`;
    if (!covering.length) {
      if (decisive) fail(`${p.symbol}: a ${declared} no vector covers as built (${built})`);
      else console.log(`NOTE clap-params ${p.symbol}: no vector covers it (${built}); its kind is held as declared only`);
      return;
    }
    for (const v of covering) {
      held++;
      const read = v.expect.kind;
      if (read !== declared) fail(`${p.symbol}: declared a ${declared}, built as the vector '${v.name}' (${built}), which a host reads as a ${read}`);
      else if (v.expect.default !== undefined && v.expect.default !== p.def >= 0.5) fail(`${p.symbol}: a toggle whose default ${p.def} a host reads as ${v.expect.default} ('${v.name}')`);
      else console.log(`PASS clap-params ${p.symbol}: a host reads a ${read} ('${v.name}')`);
    }
  });
  console.log(`PASS clap-params ${d.stem}: ${held} readings held to ${vectorsFile}`);
}
process.exit(bad ? 1 : 0);
