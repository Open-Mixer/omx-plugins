// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
//
// A tool started through a symlink (a linked checkout, a link on PATH) answers as it does by its own path: a refusal
// exits non-zero, never 0 with nothing done. Every entry point decides that it was run, not imported, with
// omx-contract's isMain (real paths on both sides, read through tools/omx-contract.mjs), never by comparing
// process.argv[1] with import.meta.url by hand. `--self-test` plants a hand comparison in a copy and expects red.
import { spawnSync } from 'node:child_process';
import { mkdirSync, mkdtempSync, readdirSync, readFileSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, relative, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
// the tools that refuse a run with no argument after loading: a load failure would refuse either way
const REFUSING = ['tools/commit-plan-check.mjs', 'tools/omx-new-plugin.mjs', 'tools/panel-file.mjs'];

/** The files under `root`'s tools/ that compare argv[1] with their own module URL by hand. */
export function handCompares(root) {
  const files = [];
  const walk = (dir) => {
    for (const e of readdirSync(dir, { withFileTypes: true })) {
      if (e.name === 'node_modules' || e.name.startsWith('.')) continue;
      const p = join(dir, e.name);
      if (e.isDirectory()) walk(p);
      else if (e.name.endsWith('.mjs')) files.push(p);
    }
  };
  walk(join(root, 'tools'));
  const code = (f) => readFileSync(f, 'utf8').split('\n').filter((l) => !/^\s*(\/\/|\*|\/\*)/.test(l));
  const hand = /process\.argv\[1\].*import\.meta\.url|import\.meta\.url.*process\.argv\[1\]/;
  return files.filter((f) => code(f).some((l) => hand.test(l))).map((f) => relative(root, f));
}

/** The tools that answer differently through a symlink: [{ tool, direct, linked }]. */
function linkedAnswers(scratch) {
  const run = (path) => spawnSync(process.execPath, [path], { cwd: ROOT, encoding: 'utf8', timeout: 60_000 });
  const out = [];
  for (const tool of REFUSING) {
    const link = join(scratch, tool.replaceAll('/', '-'));
    symlinkSync(join(ROOT, tool), link);
    const direct = run(join(ROOT, tool));
    if (direct.status === 0) out.push({ tool, direct: 'exit 0 by its own path: no longer a refusing tool', linked: '' });
    const linked = run(link);
    if (linked.status !== direct.status || linked.stderr !== direct.stderr)
      out.push({ tool, direct: `exit ${direct.status}`, linked: `exit ${linked.status}` });
  }
  return out;
}

const scratch = mkdtempSync(join(tmpdir(), 'omx-plugins-entry-'));
try {
  let bad = 0;
  for (const f of handCompares(ROOT)) { console.error(`FAIL ${f}: compares argv[1] with its own module URL by hand`); bad++; }
  for (const d of linkedAnswers(scratch)) { console.error(`FAIL ${d.tool}: ${d.direct} by its own path, ${d.linked} through a symlink`); bad++; }
  if (bad) process.exit(1);
  console.log(`ok   every entry point runs through a symlink (${REFUSING.length} refusing tools) and none compares argv[1] by hand`);
  if (process.argv.includes('--self-test')) {
    const copy = join(scratch, 'copy');
    mkdirSync(join(copy, 'tools'), { recursive: true });
    const argv1 = ['process', 'argv[1]'].join('.');
    writeFileSync(join(copy, 'tools', 'planted.mjs'), `if (${argv1} === new URL(import.meta.url).pathname) main();\n`);
    if (handCompares(copy).length !== 1) { console.error('FAIL the self-test\'s planted hand comparison was not found'); process.exit(1); }
    console.log('ok   a planted hand comparison is red');
  }
} finally {
  rmSync(scratch, { recursive: true, force: true });
}
