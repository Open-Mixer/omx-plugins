#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
//
// tools/test/role-meanings-source-check.mjs — the meaning a declared role plays is read from THIS tree's
// contract/role-meanings.json, never from an installed omx-plugins-contract of another release (or none). A family
// stands only when its required roles carry meanings (dynamics: threshold or ratio), so a build box with no
// omx-plugins installed read every meaning as none and refused omx-comp (panel.describe.family-not-backed), while a
// box with an older release installed read that release's meanings.
//
// `gen.mjs --check` (every generated file of every plugin fresh, every family backed) runs under two contract
// directories, each holding every installed package but omx-plugins:
//   none   no omx-plugins package at all (a clean build box);
//   stale  an omx-plugins package (its MANIFEST.json naming one file) whose role-meanings.json maps every role to
//          unknown (another release's file).
// Both must pass: the tree's file decides.
//
//   node tools/test/role-meanings-source-check.mjs              the check
// Exit: 0 pass, 1 refused, 2 the check could not run.
import { spawnSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import { existsSync, mkdirSync, mkdtempSync, readFileSync, readdirSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..');

const { contractDirs } = await import('../omx-panel.mjs').then(() => import(join(process.env.OMX_PANEL_DIR || '/usr/share/omx-panel', 'tools', 'vocabulary.mjs')));
const sources = contractDirs().map((d) => join(d, '3')).filter((d) => existsSync(d));
if (!sources.length) { console.error('role-meanings-source: no installed contract directory to build from'); process.exit(2); }

const tmp = mkdtempSync(join(tmpdir(), 'role-meanings-source-'));
/** A contract directory holding every installed package but omx-plugins, plus `omxPlugins` files when given. */
function contractDir(name, omxPlugins) {
  const major = join(tmp, name, '3');
  mkdirSync(major, { recursive: true });
  for (const src of sources) for (const pkg of readdirSync(src, { withFileTypes: true })) {
    if (!pkg.isDirectory() || pkg.name === 'omx-plugins' || existsSync(join(major, pkg.name))) continue;
    symlinkSync(join(src, pkg.name), join(major, pkg.name));
  }
  if (omxPlugins) {
    const at = join(major, 'omx-plugins');
    mkdirSync(at);
    const files = {};
    for (const [f, doc] of Object.entries(omxPlugins)) {
      const text = `${JSON.stringify(doc, null, 2)}\n`;
      writeFileSync(join(at, f), text);
      files[f] = createHash('sha256').update(text).digest('hex');
    }
    writeFileSync(join(at, 'MANIFEST.json'), `${JSON.stringify({ package: 'omx-plugins', version: '0.0.1', schemaMajor: 3, files }, null, 2)}\n`);
  }
  return join(tmp, name);
}

const own = JSON.parse(readFileSync(join(ROOT, 'contract', 'role-meanings.json'), 'utf8'));
const stale = structuredClone(own);
for (const row of Object.values(stale.OMX_PLUGINS_ROLE_MEANINGS.roles)) row.meaning = 'unknown';

const fails = [];
try {
  for (const [name, dir] of [['none', contractDir('none')], ['stale', contractDir('stale', { 'role-meanings.json': stale })]]) {
    const env = { ...process.env, OMX_CONTRACT_PATH: dir };
    delete env.OMX_ROLE_MEANINGS;
    const r = spawnSync(process.execPath, [join(ROOT, 'tools', 'gen.mjs'), '--check'], { cwd: ROOT, env, encoding: 'utf8' });
    if (r.status === 0) console.log(`ok   ${name}: gen --check passes over every plugin`);
    else fails.push(`${name}: gen --check exit ${r.status}: ${(r.stderr + r.stdout).trim().split('\n').filter((l) => /FamilyNotBacked|STALE|Error/.test(l)).slice(-2).join(' | ')}`);
  }
} finally {
  rmSync(tmp, { recursive: true, force: true });
}
for (const f of fails) console.error(`FAIL ${f}`);
process.exit(fails.length ? 1 : 0);
