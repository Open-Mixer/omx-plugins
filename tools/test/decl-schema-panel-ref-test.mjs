#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * decl-schema-panel-ref-test.mjs — the declaration schema reaches omx-panel's definitions (the widgets, the indexed
 * roles, visibleWhen) by the document's name, `panel.schema.json`, never by the address its publisher prints in its
 * `$id`: that address moved (github.com/FreeMixer; github.com/Open-Mixer in omx-panel 0.13.8; openmixer.org by ruling
 * D21), and with 0.13.8 installed every `$ref` into it was unresolved, so omx-geq's and omx-strip's declarations, the
 * two that use those definitions, read as missing to the plugin recipe (omx-plugins#98: the selftest debt line's
 * `omx-geq declaration`, `omx-strip declaration`).
 *
 *   1. ADDRESS MOVED. Under a scratch omx-panel tree that is the installed one with its schema's `$id` rewritten to
 *      another address, omx-geq's and omx-strip's declarations pass the recipe's declValid, no reference unresolved.
 *   2. NO HOST. schema/plugin.decl.schema.json names no host in a `$ref`: omx-panel's definitions are
 *      `panel.schema.json#/$defs/<name>`.
 *   3. NOT EVERY NAME. A `$ref` into another document (`other.schema.json#/$defs/widget`) stays unresolved: the
 *      resolver binds the one installed schema by its name, not any name.
 *
 * `make decl-schema-panel-ref`, part of `make test`. Needs the installed omx-panel (or OMX_PANEL_DIR).
 */
import { spawnSync } from 'node:child_process';
import { cpSync, mkdtempSync, readdirSync, readFileSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
const { PANEL_DIR } = await import('../omx-panel.mjs');
const TMP = mkdtempSync(join(tmpdir(), 'decl-schema-panel-ref-'));
process.on('exit', () => rmSync(TMP, { recursive: true, force: true }));
let failed = 0;
const say = (ok, what, detail = '') => {
  if (!ok) failed++;
  console.log(`${ok ? 'PASS' : 'FAIL'} decl-schema-panel-ref ${what}${ok || !detail ? '' : `: ${detail}`}`);
};

// 1. the installed omx-panel, its schema's $id moved to another address
const moved = join(TMP, 'omx-panel');
cpSync(PANEL_DIR, moved, { recursive: true, dereference: true });
const schemaFile = join(moved, 'schema', 'panel.schema.json');
const panelSchema = JSON.parse(readFileSync(schemaFile, 'utf8'));
const MOVED_ID = 'https://example.org/moved/omx-panel/schema/panel.schema.json';
say(panelSchema.$id !== MOVED_ID, 'the scratch address is not the installed one', panelSchema.$id);
writeFileSync(schemaFile, JSON.stringify({ ...panelSchema, $id: MOVED_ID }, null, 2));
const declValid = (stem, env = {}) => spawnSync('node', ['--input-type=module', '-e', `
  import { CHECKERS, loadRecipe, pluginFacts, ROOT } from '${join(ROOT, 'tools', 'plugin-recipe.mjs')}';
  const r = CHECKERS.declValid(ROOT, loadRecipe(), pluginFacts(ROOT, '${stem}'));
  console.log(JSON.stringify(r));
`], { encoding: 'utf8', cwd: ROOT, env: { ...process.env, OMX_PANEL_DIR: moved, ...env } });
for (const stem of ['omx-geq', 'omx-strip']) {
  const r = declValid(stem);
  let out;
  try {
    out = JSON.parse(r.stdout.trim().split('\n').pop());
  } catch {
    out = { ok: false, detail: `${r.stdout}${r.stderr}`.trim() };
  }
  say(r.status === 0 && out.ok && !/unresolved/.test(out.detail), `${stem}'s declaration validates under an omx-panel whose schema declares another $id`, (out.detail ?? '').slice(0, 300));
}

// 2. the declaration schema names no host
const declSchema = readFileSync(join(ROOT, 'schema', 'plugin.decl.schema.json'), 'utf8');
const refs = [...declSchema.matchAll(/"\$ref":\s*"([^"]+)"/g)].map((m) => m[1]);
const hosted = refs.filter((r) => /^[a-z]+:\/\//.test(r));
say(hosted.length === 0, `no $ref of schema/plugin.decl.schema.json names a host (${refs.length} refs)`, hosted.join(', '));
const panelRefs = refs.filter((r) => r.startsWith('panel.schema.json#'));
say(panelRefs.length >= 3, "omx-panel's definitions are reached as panel.schema.json#/$defs/<name>", refs.filter((r) => !r.startsWith('#')).join(', '));

// 3. another document's name stays unresolved
const { validate } = await import('../schema-validate.mjs');
const errs = validate({}, { $ref: 'other.schema.json#/$defs/widget' }, 'fader');
say(errs.length === 1 && /unresolved other\.schema\.json/.test(errs[0]), 'a $ref into other.schema.json stays unresolved', errs.join(' | '));

console.log(failed ? `decl-schema-panel-ref: ${failed} FAILED` : 'decl-schema-panel-ref: all pass');
process.exit(failed ? 1 : 0);
