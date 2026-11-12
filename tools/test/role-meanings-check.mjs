#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
//
// tools/test/role-meanings-check.mjs — contract/role-meanings.json, the meaning each role of our declared panels
// plays (installed as <contractdir>/3/omx-plugins/role-meanings.json; omx-contract docs/contract-dir.md "A package's
// role meanings"), is whole and only ours: every role of every family schema/plugin.decl.schema.json's familyRoles
// declares has exactly one row, no row names a role no family has, and a row's key is its role in lowerCamel case.
// That each meaning is a word of the contract's closed list, and that a role has one owner, is omx-contract's reader's
// (contract-dir-check.sh runs it over the installed tree).
//
//   node tools/test/role-meanings-check.mjs              the check
//   node tools/test/role-meanings-check.mjs --self-test  a family role left out, a stray role, a role twice and a key
//                                                        that is not its role each refuse; the shipped file passes
// Exit: 0 pass, 1 refused.
import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const read = (f) => JSON.parse(readFileSync(join(ROOT, f), 'utf8'));
const camel = (r) => r.replace(/_([a-z0-9])/g, (_, c) => c.toUpperCase());

/** Every role a family declares, once. */
export function familyRoles(schema) {
  const out = new Set();
  for (const v of Object.values(schema.$defs.familyRoles.properties)) for (const r of v.enum ?? []) out.add(r);
  return out;
}

/** The problems of `doc` (the role-meanings file) against `roles` (the family roles): one line each. */
export function problems(doc, roles) {
  const out = [];
  const item = doc.OMX_PLUGINS_ROLE_MEANINGS;
  if (item?.kind !== 'roleMeanings') return ['contract/role-meanings.json: no OMX_PLUGINS_ROLE_MEANINGS item of kind roleMeanings'];
  const seen = new Map();
  for (const [key, row] of Object.entries(item.roles ?? {})) {
    if (key !== camel(row.role)) out.push(`${key}: the key is not its role ${row.role} in lowerCamel case (${camel(row.role)})`);
    if (seen.has(row.role)) out.push(`${row.role}: a row of its own twice (${seen.get(row.role)}, ${key})`);
    seen.set(row.role, key);
    if (!roles.has(row.role)) out.push(`${row.role}: no family of schema/plugin.decl.schema.json has this role`);
  }
  for (const r of roles) if (!seen.has(r)) out.push(`${r}: a family role with no row (state its meaning, or unknown)`);
  return out;
}

function main(argv) {
  const schema = read('schema/plugin.decl.schema.json');
  const doc = read('contract/role-meanings.json');
  const roles = familyRoles(schema);
  if (argv.includes('--self-test')) {
    const copy = () => JSON.parse(JSON.stringify(doc));
    const rows = (d) => d.OMX_PLUGINS_ROLE_MEANINGS.roles;
    const arms = {
      'a family role left out': (d) => { delete rows(d).threshold; },
      'a stray role': (d) => { rows(d).wobble = { role: 'wobble', meaning: 'unknown' }; },
      'a role twice': (d) => { rows(d).again = rows(d).mix; },
      'a key that is not its role': (d) => { rows(d).thresh = rows(d).threshold; delete rows(d).threshold; },
    };
    let bad = 0;
    if (problems(doc, roles).length) { console.error('self-test: the shipped file does not pass'); bad++; }
    for (const [name, edit] of Object.entries(arms)) {
      const d = copy();
      edit(d);
      if (!problems(d, roles).length) { console.error(`self-test: ${name} passed`); bad++; }
    }
    console.log(bad ? `role-meanings self-test: ${bad} arm(s) wrong` : `role-meanings self-test: ${Object.keys(arms).length} arms refuse, the shipped file passes`);
    return bad ? 1 : 0;
  }
  const p = problems(doc, roles);
  for (const l of p) console.error(`role-meanings: ${l}`);
  if (!p.length) console.log(`role-meanings: ${roles.size} family roles, each a row of its own`);
  return p.length ? 1 : 0;
}

if (process.argv[1] === fileURLToPath(import.meta.url)) process.exit(main(process.argv.slice(2)));
