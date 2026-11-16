#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas
/*
 * backcompat-removed-check.mjs — the compatibility machinery the owner ordered out stays out. Each arm reads the tree for
 * one thing that protected an older release and is red while it is there:
 *
 *   1. no Breaks/Conflicts on an older omx-clap-host in the CLAP package (debian/control, the rpm spec).
 *   2. no Breaks/Replaces/Conflicts/BuildConflicts naming omx-plugins-dev or -devel, and no CI check requiring them.
 *   3. no `shipped`/`shippedDiff` (the schema, any declaration, tools/gen.mjs).
 *   4. the parameter-contract ledger holds one fingerprint per plugin: the current one, version 1.
 *   5. no append-only upgrade proofs: face-ids parts 2-3 (and their --self-test arms), comp-release-mode part 2, the
 *      "append-only" comment of tools/gen.mjs, and descriptor-view's "predates the descriptor words" skip.
 *
 * `--self-test` plants each old thing in a scratch copy of the files it reads and each must go red.
 * `make backcompat-removed`, part of `make test`. Exit 1 on any failure.
 */
import { readFileSync, readdirSync } from 'node:fs';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const selfTest = process.argv.includes('--self-test');

const FILES = ['debian/control', 'packaging/omx-plugins.spec', '.github/workflows/ci.yml', 'schema/plugin.decl.schema.json', 'tools/gen.mjs',
  'tools/param-contract-versions.json', 'tools/test/face-ids-test.mjs', 'tools/test/comp-release-mode-test.mjs', 'tools/test/descriptor-view-test.mjs'];
const read = () => {
  const t = Object.fromEntries(FILES.map((f) => [f, readFileSync(join(ROOT, f), 'utf8')]));
  for (const d of readdirSync(join(ROOT, 'plugins'))) {
    try { t[`plugins/${d}/${d}.decl.json`] = readFileSync(join(ROOT, 'plugins', d, `${d}.decl.json`), 'utf8'); } catch { /* no declaration */ }
  }
  return t;
};
const noComment = (s) => s.split('\n').filter((l) => !/^\s*#/.test(l)).join('\n');

/** The messages of every old thing still in tree `t`. */
function found(t) {
  const out = [];
  const cm = (f, re, what) => { if (re.test(noComment(t[f]))) out.push(`${f}: ${what}`); };
  cm('debian/control', /^Breaks: omx-clap-host/m, 'Breaks omx-clap-host');
  cm('packaging/omx-plugins.spec', /^Conflicts: omx-clap-host/m, 'Conflicts omx-clap-host');
  cm('debian/control', /^(Breaks|Replaces): omx-plugins-dev/m, 'Breaks/Replaces omx-plugins-dev');
  cm('packaging/omx-plugins.spec', /^(Build)?Conflicts: (%\{name\}|omx-plugins)-devel/m, 'Conflicts omx-plugins-devel');
  cm('.github/workflows/ci.yml', /(Breaks|Replaces): omx-plugins-dev/, 'a CI check requires Breaks/Replaces omx-plugins-dev');
  cm('schema/plugin.decl.schema.json', /"shipped(Diff)?"/, 'shipped/shippedDiff in the schema');
  cm('tools/gen.mjs', /shippedSymbols|checkShipped|\.shippedDiff/, 'the shipped check in gen.mjs');
  for (const f of Object.keys(t)) if (/\.decl\.json$/.test(f) && /"shipped(Diff)?"/.test(t[f])) out.push(`${f}: shipped/shippedDiff`);
  const ledger = JSON.parse(t['tools/param-contract-versions.json']).plugins;
  for (const [stem, fps] of Object.entries(ledger)) if (fps.length !== 1) out.push(`tools/param-contract-versions.json: ${stem} lists ${fps.length} fingerprints`);
  cm('tools/gen.mjs', /append-only/i, 'the append-only comment');
  cm('tools/test/face-ids-test.mjs', /oldLv2Ports|function moved|--self-test/, 'face-ids parts 2-3 or their self-test');
  cm('tools/test/comp-release-mode-test.mjs', /appended after the last control|APPENDED/, 'comp-release-mode part 2');
  cm('tools/test/descriptor-view-test.mjs', /predates the descriptor words|carriesWords/, 'the descriptor-view floor skip');
  return out;
}

let failed = 0;
const say = (ok, m) => {
  if (!ok) failed++;
  console.log(`${ok ? 'PASS' : 'FAIL'} backcompat-removed ${m}`);
};

if (selfTest) {
  const plant = {
    'Breaks omx-clap-host': (t) => { t['debian/control'] += '\nBreaks: omx-clap-host (<< 0.23.0~)\n'; },
    'Conflicts omx-plugins-devel': (t) => { t['packaging/omx-plugins.spec'] += '\nConflicts: %{name}-devel < 1\n'; },
    'Replaces omx-plugins-dev': (t) => { t['debian/control'] += '\nReplaces: omx-plugins-dev (<< 1)\n'; },
    'CI check for Replaces': (t) => { t['.github/workflows/ci.yml'] += "\ngrep -qE '^ Replaces: omx-plugins-dev' x\n"; },
    'schema shipped': (t) => { t['schema/plugin.decl.schema.json'] += '"shipped": {}'; },
    'gen shipped check': (t) => { t['tools/gen.mjs'] += '\nfunction checkShipped() {}\n'; },
    'declaration shipped': (t) => { t[Object.keys(t).find((f) => /\.decl\.json$/.test(f))] += '"shipped": "v0.2.0"'; },
    'two fingerprints': (t) => {
      const j = JSON.parse(t['tools/param-contract-versions.json']);
      const k = Object.keys(j.plugins)[0];
      j.plugins[k].push('0'.repeat(64));
      t['tools/param-contract-versions.json'] = JSON.stringify(j);
    },
    'append-only comment': (t) => { t['tools/gen.mjs'] += '\n// Order is append-only.\n'; },
    'face-ids part 2': (t) => { t['tools/test/face-ids-test.mjs'] += '\nfunction moved() {}\n'; },
    'comp-release-mode part 2': (t) => { t['tools/test/comp-release-mode-test.mjs'] += '\n// 2. APPENDED\n'; },
    'descriptor-view skip': (t) => { t['tools/test/descriptor-view-test.mjs'] += '\n// predates the descriptor words\n'; },
  };
  say(found(read()).length === 0, 'the tree itself is clean before the arms plant anything');
  for (const [what, mutate] of Object.entries(plant)) {
    const t = read();
    mutate(t);
    say(found(t).length > 0, `self-test: planting "${what}" is red`);
  }
} else {
  const f = found(read());
  say(f.length === 0, `no compatibility machinery of the removed rows${f.length ? `:\n  ${f.join('\n  ')}` : ''}`);
}
console.log(failed ? `backcompat-removed: ${failed} failed` : 'backcompat-removed: ok');
process.exit(failed ? 1 : 0);
