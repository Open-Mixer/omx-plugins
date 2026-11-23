// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
//
// Every generated file that states a version states the release's: the Makefile's VERSION. Reads the committed
// plugins/*/generated headers (<NAME>_VERSION), each plugin's descriptor and each LV2 plugin .ttl, and fails
// naming the file when one differs. `--self-test` sabotages a copy of each kind of file and expects red.
import { cpSync, existsSync, mkdtempSync, readFileSync, readdirSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { releaseVersion } from '../release-version.mjs';
import { isMain } from '../omx-contract.mjs';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');

function walk(dir, out = []) {
  for (const e of readdirSync(dir, { withFileTypes: true })) {
    const p = join(dir, e.name);
    if (e.isDirectory()) { if (e.name !== 'build') walk(p, out); } else out.push(p);
  }
  return out;
}

/** The disagreements under `root`: [{ file, said, want }]. */
export function check(root) {
  const want = releaseVersion(root);
  const bad = [], seen = { header: 0, descriptor: 0, ttl: 0 };
  for (const f of walk(join(root, 'plugins'))) {
    const text = f.endsWith('.h') || f.endsWith('.json') || f.endsWith('.ttl') ? readFileSync(f, 'utf8') : '';
    const say = (kind, said) => { seen[kind]++; if (said !== want) bad.push({ file: f.slice(root.length + 1), said, want }); };
    if (/\/generated\/[^/]*\.h$/.test(f)) { const m = text.match(/^#define OMX_\w+_VERSION "([^"]*)"/m); if (m) say('header', m[1]); }
    else if (/\.descriptor\.json$/.test(f)) say('descriptor', JSON.parse(text).plugin.version);
    else if (/\/generated\/[^/]*\.lv2\/(?!manifest)[^/]*\.ttl$/.test(f)) {
      const mi = text.match(/lv2:minorVersion (\d+)/), mu = text.match(/lv2:microVersion (\d+)/);
      if (mi && mu) say('ttl', `${want.split('.')[0]}.${mi[1]}.${mu[1]}`);
    }
  }
  return { bad, seen };
}

if (isMain(import.meta.url)) {
  const { bad, seen } = check(ROOT);
  if (!seen.header || !seen.descriptor || !seen.ttl) { console.error(`FAIL release-version: read ${JSON.stringify(seen)}; a kind of file was not found`); process.exit(1); }
  for (const b of bad) console.error(`FAIL ${b.file} says ${b.said}, the release is ${b.want}`);
  if (bad.length) process.exit(1);
  console.log(`ok   ${seen.header} headers, ${seen.descriptor} descriptors, ${seen.ttl} LV2 plugin files carry ${releaseVersion(ROOT)}`);
  if (process.argv.includes('--self-test')) {
    const tmp = mkdtempSync(join(tmpdir(), 'relver-'));
    try {
      cpSync(join(ROOT, 'plugins'), join(tmp, 'plugins'), { recursive: true });
      cpSync(join(ROOT, 'Makefile'), join(tmp, 'Makefile'));
      const mk = join(tmp, 'Makefile'), mkText = readFileSync(mk, 'utf8');
      const arms = {
        'a header': (f) => /\/generated\/[^/]*\.h$/.test(f) && /^#define OMX_\w+_VERSION "/m.test(readFileSync(f, 'utf8')),
        'a descriptor': (f) => /\.descriptor\.json$/.test(f),
        'a plugin .ttl': (f) => /\.lv2\/(?!manifest)[^/]*\.ttl$/.test(f) && /lv2:microVersion/.test(readFileSync(f, 'utf8')),
      };
      for (const [name, pick] of Object.entries(arms)) {
        const f = walk(join(tmp, 'plugins')).find(pick);
        const t = readFileSync(f, 'utf8');
        writeFileSync(f, t.replace(/("version": ")[\d.]+|(_VERSION ")[\d.]+|(lv2:microVersion )\d+/, (m, a, b, c) => `${a ?? b ?? c}${c ? '999' : '0.0.1'}`));
        if (!check(tmp).bad.length) { console.error(`FAIL self-test: ${name} at the wrong version went unnoticed`); process.exit(1); }
        writeFileSync(f, t);
      }
      writeFileSync(mk, mkText.replace(/^VERSION := .*/m, 'VERSION := 9.9.9'));
      if (!check(tmp).bad.length) { console.error('FAIL self-test: a moved release went unnoticed'); process.exit(1); }
      console.log('ok   self-test: a header, a descriptor, a .ttl and a moved release each go red');
    } finally { rmSync(tmp, { recursive: true, force: true }); }
  }
}
