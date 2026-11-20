#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * schema-urls-check.mjs — this package's own JSON schemas (schema/*.json) name the project's own domain, never a code host's
 * organisation, which moved once already (FreeMixer to Open-Mixer). The rule, as omx-contract holds it for its schemas:
 *   $schema  only https://json-schema.org/... or https://openmixer.org/schema/...
 *   $id      only https://openmixer.org/contract/<path>, the path made of names, no .json
 *   $ref     relative (`#/...` or a relative path), or https://openmixer.org/...
 * `make schema-urls`, part of `make test`. The descriptors' and the definition files' `$schema` (the relative form the install
 * rewrites) are the contract-directory check's (tools/test/contract-dir-check.sh), not this one's.
 *
 *   node tools/test/schema-urls-check.mjs               the check over schema/*.json
 *   node tools/test/schema-urls-check.mjs --self-test   the check, then copies with a code-host $id, $ref and $schema, a $id with .json
 *                                                       and a $id off the contract path: each red
 */
import { readFileSync, readdirSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { isMain } from '../omx-contract.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const ABSOLUTE = /^[a-z][a-z0-9+.-]*:/i;

/** The refusals of one schema document: `[path, why]`, each a key whose value breaks the rule. */
export function refusals(doc) {
  const out = [];
  const walk = (node, at) => {
    if (Array.isArray(node)) return node.forEach((v, i) => walk(v, `${at}/${i}`));
    if (!node || typeof node !== 'object') return;
    for (const [k, v] of Object.entries(node)) {
      const here = `${at}/${k}`;
      if (typeof v === 'string' && k === '$schema' && !/^https:\/\/(json-schema\.org\/|openmixer\.org\/schema\/)/.test(v)) out.push([here, `$schema ${v} is neither json-schema.org nor openmixer.org/schema`]);
      if (typeof v === 'string' && k === '$id' && !/^https:\/\/openmixer\.org\/contract\/[A-Za-z0-9_-]+(\/[A-Za-z0-9_-]+)*$/.test(v)) out.push([here, `$id ${v} is not https://openmixer.org/contract/<path> with no .json`]);
      if (typeof v === 'string' && k === '$ref' && ABSOLUTE.test(v) && !/^https:\/\/openmixer\.org\//.test(v)) out.push([here, `$ref ${v} is neither relative nor openmixer.org`]);
      walk(v, here);
    }
  };
  walk(doc, '');
  return out;
}

/** Every schema file of the tree, with its refusals. */
export function check(dir = join(root, 'schema')) {
  const bad = [];
  const files = readdirSync(dir).filter((f) => f.endsWith('.json'));
  if (!files.length) bad.push([dir, 'no schema file: the check would pass over nothing']);
  for (const f of files) for (const [at, why] of refusals(JSON.parse(readFileSync(join(dir, f), 'utf8')))) bad.push([`${f}${at}`, why]);
  return bad;
}

if (isMain(import.meta.url)) {
  const bad = check();
  for (const [at, why] of bad) console.error(`schema-urls: schema/${at}: ${why}`);
  let status = bad.length ? 1 : 0;
  if (!bad.length) console.log('schema-urls: every $schema, $id and $ref of schema/*.json is relative or the project\'s own domain');
  if (process.argv.includes('--self-test')) {
    const good = { $schema: 'https://json-schema.org/draft/2020-12/schema', $id: 'https://openmixer.org/contract/omx-plugins/plugin-decl', a: { $ref: '#/$defs/x' }, b: { $ref: 'https://openmixer.org/schema/omx-panel/panel.schema.json#/$defs/widget' }, c: { $ref: 'other.json' } };
    const arms = {
      'a green document': [good, 0],
      'a $id on a code host': [{ ...good, $id: 'https://github.com/Open-Mixer/omx-plugins/schema/plugin.decl.schema.json' }, 1],
      'a $id with .json': [{ ...good, $id: 'https://openmixer.org/contract/omx-plugins/plugin-decl.json' }, 1],
      'a $id off the contract path': [{ ...good, $id: 'https://openmixer.org/schema/omx-plugins/plugin-decl' }, 1],
      'a $ref on a code host': [{ ...good, a: { $ref: 'https://github.com/Open-Mixer/omx-panel/schema/panel.schema.json#/$defs/widget' } }, 1],
      'a $schema on a code host': [{ ...good, $schema: 'https://github.com/FreeMixer/omx-contract/raw/v3/schema/omx-contract.schema.json' }, 1],
      'a nested $ref on a code host': [{ ...good, d: { items: [{ $ref: 'http://example.org/x' }] } }, 1],
    };
    for (const [name, [doc, want]] of Object.entries(arms)) {
      const got = refusals(doc).length ? 1 : 0;
      if (got !== want) { console.error(`schema-urls self-test: ${name}: ${got ? 'red' : 'green'}, want ${want ? 'red' : 'green'}`); status = 1; }
    }
    if (!status) console.log('schema-urls self-test: every sabotage red, the green document green');
  }
  process.exit(status);
}
