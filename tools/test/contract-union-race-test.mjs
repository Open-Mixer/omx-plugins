#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * contract-union-race-test.mjs — the union render (tools/omx-contract.mjs unionRender) is read whole
 * while other processes render. `make -j` runs several node tools at once, and each renders for itself;
 * a render written in place under a reader gave panel-layout-test a half-written omx-contract.json
 * (release PR #186). Here a fake omx-contract CLI opens its output file, writes the front of it, waits, then
 * writes the rest (the window a real render has between open and close), and several readers start a little apart: every
 * one must read a complete render, and the one they read must stay whole while the later ones render.
 *
 * Nothing here needs the real omx-contract: a fake pkg-config names a fake data directory and contract
 * directory, and the fake CLI stands for `omx-contract render`. `make contract-union`, part of
 * `make test` and `make test-faces`.
 */
import { spawn } from 'node:child_process';
import { chmodSync, existsSync, mkdirSync, mkdtempSync, readFileSync, readdirSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';

const HERE = fileURLToPath(import.meta.url);
const LIB = join(HERE, '..', '..', 'omx-contract.mjs');
const READERS = 3;
const RENDER_MS = 400;
const STAGGER_MS = 500;

/** A throwaway root: the packaging floor, a fake pkg-config, a fake render CLI. */
function scratch() {
  const root = mkdtempSync(join(tmpdir(), 'union-race-'));
  mkdirSync(join(root, 'packaging'));
  writeFileSync(join(root, 'packaging', 'omx-plugins.spec'), 'BuildRequires: omx-contract-devel >= 2.15.0\n');
  const data = join(root, 'data');
  const contractdir = join(root, 'contractdir');
  mkdirSync(data);
  mkdirSync(contractdir);
  // the installed render holds only the shared facts: no kernels, which is what asks for the union
  writeFileSync(join(data, 'omx-contract.json'), JSON.stringify({ version: '2.16.0', kernels: {}, items: {} }));
  const pkg = join(root, 'pkg-config');
  writeFileSync(pkg, `#!/bin/sh\ncase "$*" in\n  *datadir*) echo ${data};;\n  *contractdir*) echo ${contractdir};;\nesac\n`);
  chmodSync(pkg, 0o755);
  const cli = join(root, 'fake-omx-contract.mjs');
  writeFileSync(cli, `#!/usr/bin/env node
import { closeSync, openSync, writeSync } from 'node:fs';
import { join } from 'node:path';
const out = process.argv[process.argv.indexOf('--out') + 1];
const file = join(out, 'omx-contract.json');
const doc = JSON.stringify({ version: '2.16.0', kernels: { k: {} }, items: { X: { rel: 'data/kernels/k.json', kind: 'scalar', value: 1 } } });
const fd = openSync(file, 'w');
writeSync(fd, doc.slice(0, 40));
await new Promise((r) => setTimeout(r, ${RENDER_MS}));
writeSync(fd, doc.slice(40));
closeSync(fd);
`);
  chmodSync(cli, 0o755);
  return { root, pkg, cli };
}

/** One reader process: locate the contract, then read the resolved items the way every tool does. */
function reader(s) {
  const code = `import { locateContract, items } from ${JSON.stringify(`file://${LIB}`)};
const c = locateContract(${JSON.stringify(s.root)});
if (!c.dir) { console.log(JSON.stringify({ why: c.why })); process.exit(0); }
await new Promise((r) => setTimeout(r, ${RENDER_MS}));
console.log(JSON.stringify({ dir: c.dir, X: items(c.dir).X?.value }));`;
  return new Promise((resolve) => {
    const p = spawn(process.execPath, ['--input-type=module', '-e', code], {
      env: { ...process.env, PKG_CONFIG: s.pkg, OMX_CONTRACT_CLI: s.cli, OMX_CONTRACT_DIR: '' },
      stdio: ['ignore', 'pipe', 'pipe'],
    });
    let out = '', err = '';
    p.stdout.on('data', (d) => (out += d));
    p.stderr.on('data', (d) => (err += d));
    p.on('close', (rc) => resolve({ rc, out: out.trim(), err: err.trim().split('\n').find((l) => /Error/.test(l)) ?? err.trim().split('\n')[0] }));
  });
}

const s = scratch();
const runs = [];
for (let i = 0; i < READERS; i++) {
  runs.push(reader(s));
  await new Promise((r) => setTimeout(r, STAGGER_MS));
}
const got = await Promise.all(runs);


let bad = 0;
const check = (what, ok, detail = '') => {
  if (!ok) bad++;
  console.log(`${ok ? 'PASS' : 'FAIL'} contract-union ${what}${ok ? '' : `: ${detail}`}`);
};
got.forEach((g, i) => {
  let v;
  try {
    v = JSON.parse(g.out);
  } catch {
    v = undefined;
  }
  check(`reader ${i + 1} of ${READERS} reads a whole render while the others render`, v?.X === 1, `rc ${g.rc}, out ${JSON.stringify(g.out)}, ${g.err}`);
});
const dirs = new Set(got.map((g) => { try { return JSON.parse(g.out).dir; } catch { return undefined; } }));
check('the readers of one content share one directory', dirs.size === 1 && !dirs.has(undefined), JSON.stringify([...dirs]));
const left = readdirSync(join(s.root, 'build')).filter((n) => n.includes('.tmp-'));
check('no temporary render is left behind', left.length === 0, left.join(', '));
const only = [...dirs][0];
check('the directory read holds its render and its owners', !!only && existsSync(join(only, 'omx-contract.json')) && existsSync(join(only, 'owners.json')), String(only));
if (only) check('the render read is the whole file', JSON.parse(readFileSync(join(only, 'omx-contract.json'), 'utf8')).items.X.value === 1);

if (!process.env.UNION_KEEP) rmSync(s.root, { recursive: true, force: true }); else console.log(s.root);
console.log(bad ? `${bad} FAILED` : 'contract-union: all pass');
process.exit(bad ? 1 : 0);
