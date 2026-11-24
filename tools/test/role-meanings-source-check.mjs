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

/** The `$schema` an install writes: the major's URL, read off omx-contract's OWN package (the one that installs it), and only
 * when every file of it names the same one: another package's files may still carry an older URL, so none is read. */
export function installedSchemaOf(dirs) {
  const urls = new Set();
  for (const src of dirs) {
    const pkg = join(src, 'omx-contract');
    if (!existsSync(pkg)) continue;
    for (const f of readdirSync(pkg).filter((n) => n.endsWith('.json') && n !== 'MANIFEST.json')) {
      const u = JSON.parse(readFileSync(join(pkg, f), 'utf8')).$schema;
      if (typeof u === 'string' && u.startsWith('https://')) urls.add(u);
    }
  }
  if (urls.size !== 1) throw new Error(`role-meanings-source: omx-contract's own files name ${urls.size} different $schema URLs (${[...urls].join(', ') || 'none'}), exactly one is needed`);
  return [...urls][0];
}

/** Mirror installed contract roots into `at`: every package of the major (`3/<pkg>`) but omx-plugins, and every domain
 * directory a root keeps outside the major (omx-contract-data >= 3.21.0 installs its param domain as `param/param.json`
 * beside `3/`), the first root that holds a name winning, as the reader's search order does. */
export function mirrorContract(contractRoots, at) {
  const major = join(at, '3');
  mkdirSync(major, { recursive: true });
  for (const root of contractRoots) for (const e of readdirSync(root, { withFileTypes: true })) {
    if (!e.isDirectory()) continue;
    if (e.name === '3') {
      for (const pkg of readdirSync(join(root, '3'), { withFileTypes: true })) {
        if (!pkg.isDirectory() || pkg.name === 'omx-plugins' || existsSync(join(major, pkg.name))) continue;
        symlinkSync(join(root, '3', pkg.name), join(major, pkg.name));
      }
    } else if (!/^[0-9]+$/.test(e.name) && !existsSync(join(at, e.name))) symlinkSync(join(root, e.name), join(at, e.name));
  }
}

if (process.argv.includes('--self-test')) {
  const t = mkdtempSync(join(tmpdir(), 'rms-self-'));
  const put = (name, urls) => {
    const d = join(t, name, 'omx-contract');
    mkdirSync(d, { recursive: true });
    urls.forEach((u, i) => writeFileSync(join(d, `f${i}.json`), JSON.stringify({ $schema: u })));
    return join(t, name);
  };
  const bad = [];
  try {
    if (installedSchemaOf([put('one', ['https://x/v3.json', 'https://x/v3.json'])]) !== 'https://x/v3.json') bad.push('one URL is not returned');
    for (const [arm, urls] of [['two URLs', ['https://x/v3.json', 'https://old/raw/v3.json']], ['no URL', []]]) {
      try { installedSchemaOf([put(arm.replace(' ', '-'), urls)]); bad.push(`${arm}: not refused`); } catch (e) { if (!/exactly one is needed/.test(e.message)) bad.push(`${arm}: ${e.message}`); }
    }
    // the mirror carries a domain kept outside the major (param/param.json beside 3/), and leaves omx-plugins out
    const root = join(t, 'root');
    for (const [f, text] of [['3/omx-contract/a.json', '{}'], ['3/omx-plugins/b.json', '{}'], ['param/param.json', '{}']]) {
      mkdirSync(dirname(join(root, f)), { recursive: true });
      writeFileSync(join(root, f), text);
    }
    mirrorContract([root], join(t, 'mirror'));
    if (!existsSync(join(t, 'mirror', 'param', 'param.json'))) bad.push('mirror: the param domain outside the major is not carried');
    if (!existsSync(join(t, 'mirror', '3', 'omx-contract', 'a.json'))) bad.push('mirror: a package of the major is not carried');
    if (existsSync(join(t, 'mirror', '3', 'omx-plugins'))) bad.push('mirror: omx-plugins is carried');
  } finally { rmSync(t, { recursive: true, force: true }); }
  for (const b of bad) console.error(`FAIL self-test: ${b}`);
  if (bad.length) process.exit(1);
  console.log('role-meanings-source self-test: one URL read, two URLs and none refused by name; the mirror carries the param domain beside the major');
  process.exit(0);
}

const { contractDirs } = await import('../omx-panel.mjs').then(() => import(join(process.env.OMX_PANEL_DIR || '/usr/share/omx-panel', 'tools', 'vocabulary.mjs')));
const roots = contractDirs().filter((d) => existsSync(join(d, '3')));
const sources = roots.map((d) => join(d, '3'));
if (!sources.length) { console.error('role-meanings-source: no installed contract directory to build from'); process.exit(2); }
let installedSchema;
try { installedSchema = installedSchemaOf(sources); } catch (e) { console.error(e.message); process.exit(2); }

const tmp = mkdtempSync(join(tmpdir(), 'role-meanings-source-'));
/** A contract directory holding every installed package but omx-plugins and every installed domain (param's PARAM_UNITS,
 * which omx-panel's layout reads, among them), plus `omxPlugins` files when given. */
function contractDir(name, omxPlugins) {
  const major = join(tmp, name, '3');
  mirrorContract(roots, join(tmp, name));
  if (omxPlugins) {
    const at = join(major, 'omx-plugins');
    mkdirSync(at);
    const files = {};
    for (const [f, doc] of Object.entries(omxPlugins)) {
      // as an install writes it: the major's URL, which omx-contract's reader requires of every file of the directory
      const text = `${JSON.stringify({ ...doc, $schema: installedSchema }, null, 2)}\n`;
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
