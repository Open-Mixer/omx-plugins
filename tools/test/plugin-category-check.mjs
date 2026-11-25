#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * plugin-category-check.mjs — every shipped plugin declares what a console needs to offer it and draw it: its one
 * `category` (the row a console's folded strip line groups it under, schema/plugin.decl.schema.json `$defs.category`)
 * and a panel `family` other than `none` (the house view a console draws instead of its generic editor). A test
 * plugin (omx-test-*) declares neither: it is never offered. The categories were ruled by the owner on 2026-11-25.
 * `make plugin-category`, part of `make test`.
 *
 * NO_FAMILY names the plugins no design gives a family and whose parameters play no family's roles: each
 * carries why. The list only shrinks: a plugin on it that declares a family is red until its entry goes.
 *
 *   node tools/test/plugin-category-check.mjs               the check, over every declaration
 *   node tools/test/plugin-category-check.mjs --self-test   the check, then: no category, a category outside the
 *                                                           schema's, a test plugin with one, family none off the
 *                                                           list, a listed plugin with a family: each red; a
 *                                                           declared plugin and a bare test plugin: each green
 */
import { readdirSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const SCHEMA = 'schema/plugin.decl.schema.json';

/** The plugins that keep panel family `none`, each with why (shrink-only). */
export const NO_FAMILY = {
  'omx-balancer': 'a spectral balancer: no design names its family and no family has an amount or slope role',
  'omx-loudlevel': 'a loudness leveler: no design names its family and no family has a target-loudness role',
  'omx-strip': 'a chain of five stages (trim, eq, gate, eq, comp): its design declares it family none, as no one family draws five',
  'omx-transient': 'its design names dynamics, and omx-panel refuses it (FamilyNotBacked: the dynamics family needs a threshold or ratio meaning, and a transient designer is level-independent)',
  'omx-trim': 'one gain: no design names its family and no family is a lone gain',
  'omx-width': 'a stereo imager: its design names no family and no family has a width role',
  'omx-suboct': 'a sub-octaver: its design names no family and the pitch roles (shift, fine) are not its octave levels',
};

/** Whether `stem` is a test plugin, which is never offered and declares no category. */
export const isTestPlugin = (stem) => stem.startsWith('omx-test-');

/** The categories the schema allows. */
export function categories(schema) {
  const list = schema?.$defs?.category?.enum;
  if (!Array.isArray(list) || list.length < 2) throw new Error(`${SCHEMA}: $defs.category has no enum of the categories`);
  return list;
}

/** The refusals of one declaration `decl` of plugin `stem`. */
export function drift(stem, decl, allowed, noFamily = NO_FAMILY) {
  const errors = [];
  if (isTestPlugin(stem)) {
    if (decl.category !== undefined) errors.push('a test plugin declares a category; it is never offered');
    return errors;
  }
  if (decl.category === undefined) errors.push(`declares no category (one of ${allowed.join(', ')})`);
  else if (!allowed.includes(decl.category)) errors.push(`declares category ${JSON.stringify(decl.category)}, not one of ${allowed.join(', ')}`);
  const family = decl.panel?.family;
  if (family === 'none' && !(stem in noFamily)) errors.push('declares panel family none: give it the family its design names and its roles');
  if (family !== 'none' && stem in noFamily) errors.push(`declares panel family ${JSON.stringify(family)} and is still on NO_FAMILY: take it off the list`);
  return errors;
}

const allowed = categories(JSON.parse(readFileSync(join(root, SCHEMA), 'utf8')));
const errors = [];
const counts = {};
let read = 0;
for (const e of readdirSync(join(root, 'plugins'), { withFileTypes: true })) {
  if (!e.isDirectory()) continue;
  const rel = join('plugins', e.name, `${e.name}.decl.json`);
  let decl;
  try { decl = JSON.parse(readFileSync(join(root, rel), 'utf8')); } catch { continue; }
  read++;
  for (const m of drift(e.name, decl, allowed)) errors.push(`${rel}: ${m}`);
  if (decl.category !== undefined) counts[decl.category] = (counts[decl.category] ?? 0) + 1;
}
for (const stem of Object.keys(NO_FAMILY)) if (!readdirSync(join(root, 'plugins')).includes(stem)) errors.push(`NO_FAMILY names ${stem}, which is no plugin: take it off the list`);
if (read === 0) errors.push('no declaration was read');
if (errors.length) {
  for (const e of errors) console.error(`plugin-category: ${e}`);
  process.exit(1);
}
console.log(`plugin-category: ${read} declarations; ${Object.entries(counts).map(([c, n]) => `${c} ${n}`).join(', ')}; ${Object.keys(NO_FAMILY).length} keep family none by name`);

if (process.argv.includes('--self-test')) {
  let fail = 0;
  const want = (arm, stem, decl, red) => {
    const out = drift(stem, decl, allowed);
    if (Boolean(out.length) === red) console.log(`SABOTAGE ${arm}: ${red ? `red (${out[0]})` : 'green'}`);
    else {
      console.error(`SABOTAGE ${arm}: ${red ? 'STAYED GREEN' : `went red (${out[0]})`}`);
      fail++;
    }
  };
  const fam = { family: 'dynamics' };
  want('no category', 'omx-comp', { panel: fam }, true);
  want('a category outside the schema', 'omx-comp', { category: 'space', panel: fam }, true);
  want('a test plugin with a category', 'omx-test-sine-voice', { category: allowed[0] }, true);
  want('family none off the list', 'omx-plosive', { category: allowed[0], panel: { family: 'none' } }, true);
  want('a listed plugin with a family', 'omx-strip', { category: allowed[0], panel: fam }, true);
  want('a declared plugin', 'omx-comp', { category: allowed[0], panel: fam }, false);
  want('a bare test plugin', 'omx-test-sine-voice', {}, false);
  if (fail) process.exit(1);
  console.log('plugin-category self-test: every sabotage red, every declared plugin green');
}
