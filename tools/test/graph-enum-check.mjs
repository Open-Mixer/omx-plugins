#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * graph-enum-check.mjs — the graph kinds a declaration may name (schema/plugin.decl.schema.json $defs.graph) are
 * omx-dsp's CURVE_KINDS, all of them, rendered into the schema by tools/graph-enum.mjs (sorting row V26: one holder,
 * the plugins render all its kinds; the old `lfo` is omx-dsp's modulation-shape, the picture omx-dsp says it stood for).
 * Held here: the schema's enum = golden (omx-dsp's eight kinds) = the CURVE_KINDS omx-dsp's package holds in the render
 * read; and the render of a copy of that render whose CURVE_KINDS lost interval-map, or whose CURVE_KINDS is another
 * package's, is refused. `make graph-enum`, part of `make test`.
 */
import { spawnSync } from 'node:child_process';
import { cpSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { items, locateContract, OWNERS } from '../omx-contract.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const TOOL = join(root, 'tools', 'graph-enum.mjs');
/** omx-dsp's eight curve kinds (omx-dsp#123, lane/sort-omx-dsp-pass), frozen; append only. */
const GOLDEN = ['eq-curve', 'dynamics-curve', 'transfer-curve', 'delay-taps', 'decay-envelope', 'comb-response', 'modulation-shape', 'interval-map'];

let bad = 0;
const check = (what, ok, detail) => { console.log(`${ok ? 'PASS' : 'FAIL'} graph-enum ${what}${ok ? '' : `: ${detail}`}`); if (!ok) bad++; };
const schemaKinds = () => JSON.parse(readFileSync(join(root, 'schema', 'plugin.decl.schema.json'), 'utf8')).$defs.graph.enum;
const where = locateContract(root);
if (!where.dir) { console.error(`graph-enum: ${where.why}`); process.exit(1); }
check('the schema offers omx-dsp\'s eight curve kinds (golden)', JSON.stringify(schemaKinds()) === JSON.stringify(GOLDEN), JSON.stringify(schemaKinds()));
const run = (dir) => spawnSync(process.execPath, [TOOL, '--check'], { cwd: root, encoding: 'utf8', env: { ...process.env, OMX_CONTRACT_DIR: dir } });
let r = run(where.dir);
check('tools/graph-enum.mjs --check: the schema is CURVE_KINDS rendered', r.status === 0, (r.stderr || r.stdout).trim().split('\n').pop());
// the arms, on copies of the render read
const arm = (name, edit, want) => {
  const tmp = mkdtempSync(join(tmpdir(), 'graph-enum-'));
  try {
    for (const f of ['omx-contract.json', OWNERS]) cpSync(join(where.dir, f), join(tmp, f));
    const j = JSON.parse(readFileSync(join(tmp, 'omx-contract.json'), 'utf8'));
    const o = JSON.parse(readFileSync(join(tmp, OWNERS), 'utf8'));
    edit(j, o);
    writeFileSync(join(tmp, 'omx-contract.json'), JSON.stringify(j));
    writeFileSync(join(tmp, OWNERS), JSON.stringify(o));
    const x = run(tmp);
    check(`arm: ${name} is refused`, x.status !== 0 && want.test(x.stderr), `exit ${x.status} ${x.stderr.trim().split('\n').pop()}`);
  } finally {
    rmSync(tmp, { recursive: true, force: true });
  }
};
const rel = items(where.dir).CURVE_KINDS?.rel;
arm('a CURVE_KINDS without interval-map', (j) => { delete j.items.CURVE_KINDS.value.intervalMap; }, /STALE/);
arm('a CURVE_KINDS that is omx-contract\'s', (j, o) => { o[rel] = { package: 'omx-contract', version: '3.20.2' }; }, /omx-dsp/);
console.log(bad ? `graph-enum: ${bad} failed` : 'graph-enum: all pass');
process.exit(bad ? 1 : 0);
