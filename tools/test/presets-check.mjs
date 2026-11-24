#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * presets-check.mjs — every factory preset file parses, holds to plugin-hostd's preset schema, belongs to a shipped plugin, names only that
 * plugin's parameters, and keeps each value inside the travel its generated header declares (whole
 * steps where the parameter is stepped, a choice's label where it has one). Every shipped plugin has
 * at least one preset. The two presets measured on the console are pinned to their values here, and to their
 * measured figures through the DSP by tools/test/preset-figures.mjs (`make presets-figures`).
 * `make presets`, part of `make test`; `--self-test` breaks a copy of a preset each way and wants each red, and the schema reader refuses a word it does not know.
 */
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { ROOT, plugins, presetFiles, presetSchema, problems, schemaProblems } from '../preset-lib.mjs';

const shipped = plugins();
const byId = new Map(shipped.map((p) => [p.id, p]));
const files = presetFiles();
const errors = [];
for (const { file, dir, preset } of files) errors.push(...problems(file, dir, preset, byId.get(dir)));
for (const p of shipped) if (!files.some((f) => f.dir === p.id)) errors.push(`${p.id}: no factory presets`);

// The figures measured on the console (the console's catalogue): the guitar amp's two stages and the dimension.
const value = (dir, id, symbol) => files.find((f) => f.dir === dir && f.preset.id === id)?.preset.params.find((p) => p.symbol === symbol)?.value;
const pinned = [
  ['org.openmixer.eq8', 'amp-input', 'b1_freq', 2500], ['org.openmixer.eq8', 'amp-input', 'b1_gain', 4], ['org.openmixer.eq8', 'amp-input', 'b1_q', 1], ['org.openmixer.eq8', 'amp-input', 'b1_on', 1], ['org.openmixer.eq8', 'amp-input', 'b1_type', 0],
  ['org.openmixer.drive', 'amp-tube', 'amount', 18], ['org.openmixer.drive', 'amp-tube', 'character', 0.6], ['org.openmixer.drive', 'amp-tube', 'curve', 2], ['org.openmixer.drive', 'amp-tube', 'hfRolloff', 1],
  ['org.openmixer.chorus', 'dimension', 'voices', 2], ['org.openmixer.chorus', 'dimension', 'spread', 0.5], ['org.openmixer.chorus', 'dimension', 'mix', 100],
];
for (const [dir, id, symbol, want] of pinned) if (value(dir, id, symbol) !== want) errors.push(`${dir}/${id}: ${symbol} is ${value(dir, id, symbol)}, measured at ${want}`);

if (process.argv.includes('--self-test')) {
  const drive = byId.get('org.openmixer.drive');
  const good = JSON.parse(readFileSync(join(ROOT, 'presets/org.openmixer.drive/amp-tube.json'), 'utf8'));
  const run = (mutate) => { const c = structuredClone(good); mutate(c); return problems('presets/org.openmixer.drive/amp-tube.json', 'org.openmixer.drive', c, drive).length; };
  const arms = {
    'a value above its travel': (c) => { c.params.find((p) => p.symbol === 'amount').value = 37; },
    'a value below its travel': (c) => { c.params.find((p) => p.symbol === 'trim').value = -25; },
    'a fractional step': (c) => { c.params.find((p) => p.symbol === 'bandFreq').value = 2000.5; },
    'a parameter the plugin lacks': (c) => { c.params.push({ symbol: 'wobble', unit: 'none', value: 0 }); },
    'a label the plugin does not give': (c) => { c.params.find((p) => p.symbol === 'curve').label = 'Fuzz'; },
    'a plugin that is a string': (c) => { c.plugin = c.plugin.id; },
    'a plugin with an empty id': (c) => { c.plugin.id = ''; },
    'a date with no UTC mark': (c) => { c.createdAt = '2026-10-17T00:00:00'; },
    'a state in a params-only format': (c) => { c.state = { encoding: 'base64', context: 'preset', bytes: '' }; },
  };
  if (problems('presets/org.openmixer.drive/amp-tube.json', 'org.openmixer.drive', good, drive).length) errors.push('self-test: the untouched copy is red');
  for (const [name, mutate] of Object.entries(arms)) if (run(mutate) === 0) errors.push(`self-test: ${name} stays green`);
  let refused = false;
  try { schemaProblems({}, { ...presetSchema(), oneOf: [] }); } catch { refused = true; }
  if (!refused) errors.push('self-test: the schema reader takes a word it does not know');
}

if (errors.length) {
  for (const e of errors) console.error(`presets: ${e}`);
  process.exit(1);
}
console.log(`presets: ${files.length} presets of ${shipped.length} plugins, every value inside its declared travel`);
