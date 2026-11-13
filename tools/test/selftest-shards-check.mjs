#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * selftest-shards-check.mjs — make selftest runs in CI as shards, each under its time budget, none dropping a part.
 *
 * make selftest (tools/test/recipe-selftest.mjs) took 82 to 115 minutes in one CI job and hit its 120-minute timeout.
 * Its parts are part('<id>') blocks; tools/test/selftest-shards.json holds each part's measured minutes and the
 * shards CI runs (OMX_SELFTEST_SHARD=i/n). This holds the three to each other:
 *   - every part the selftest runs is in the table, every part of the table is one the selftest runs, and every part
 *     is in exactly one shard;
 *   - every shard's estimate (the setup wait, its parts, and each part they need that the shard does not hold, run
 *     there as a prerequisite) is under the budget by the margin;
 *   - .github/workflows/ci.yml runs make selftest in a matrix whose shards are 1/n..n/n for the table's n, the shard
 *     in OMX_SELFTEST_SHARD, the job's name carrying the shard, and its timeout-minutes still 120;
 *   - the Makefile passes OMX_SELFTEST_SHARD to the selftest.
 *
 * --self-test: each sabotage arm must be red for its own reason, the unsabotaged tree green.
 * `make selftest-shards`, part of `make test`.
 */
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = join(fileURLToPath(new URL('.', import.meta.url)), '..', '..');
const read = (p) => readFileSync(join(ROOT, p), 'utf8');
const TIMEOUT = 120;

/** The parts the selftest runs, in source order. */
export const selftestParts = (source) => [...source.matchAll(/\bpart\('([a-z0-9-]+)'/g)].map((m) => m[1]);

/** Each shard's estimate in minutes: the setup wait, its parts, and the parts they need from outside it. */
export function estimates(table) {
  return table.shards.map((shard) => {
    const run = new Set(shard);
    for (const id of shard) for (const n of table.parts[id]?.needs ?? []) run.add(n);
    return table.setupMinutes + [...run].reduce((s, id) => s + (table.parts[id]?.minutes ?? 0), 0);
  });
}

/** Longest first into the lightest shard: the packing a table of n shards is held against in --self-test. */
export function pack(table, n) {
  const shards = Array.from({ length: n }, () => []);
  const load = (shard) => estimates({ ...table, shards: [shard] })[0];
  const ids = Object.keys(table.parts).sort((a, b) => table.parts[b].minutes - table.parts[a].minutes || a.localeCompare(b));
  for (const id of ids) {
    const at = shards.map((s, i) => [load([...s, id]), i]).sort((a, b) => a[0] - b[0] || a[1] - b[1])[0][1];
    shards[at].push(id);
  }
  return shards;
}

/** Every defect, one line each, of the tree given as file texts. */
export function shardErrors(t) {
  const errors = [];
  let table;
  try { table = JSON.parse(t.table); } catch (e) { return [`tools/test/selftest-shards.json: ${e.message}`]; }
  const parts = selftestParts(t.selftest);
  if (!parts.length) errors.push('recipe-selftest.mjs: no part(\'<id>\') block, so no shard can select its share');
  for (const id of new Set(parts.filter((p, i) => parts.indexOf(p) !== i))) errors.push(`recipe-selftest.mjs: part '${id}' is declared twice`);
  const listed = Object.keys(table.parts ?? {});
  for (const id of parts) if (!listed.includes(id)) errors.push(`recipe-selftest.mjs: part '${id}' has no measured minutes in the table`);
  for (const id of listed) if (!parts.includes(id)) errors.push(`table: part '${id}' is no part the selftest runs`);
  for (const id of listed) for (const n of table.parts[id].needs ?? []) if (!listed.includes(n)) errors.push(`table: part '${id}' needs '${n}', no part`);
  const shards = table.shards ?? [];
  if (!shards.length) errors.push('table: no shards');
  for (const id of listed) {
    const in_ = shards.map((s, i) => (s.includes(id) ? i + 1 : 0)).filter(Boolean);
    if (in_.length !== 1) errors.push(`table: part '${id}' is in ${in_.length} shards${in_.length ? ` (${in_.join(', ')})` : ''}, not exactly one`);
  }
  for (const [i, s] of shards.entries()) {
    if (!s.length) errors.push(`table: shard ${i + 1} runs no part`);
    for (const id of s) if (!listed.includes(id)) errors.push(`table: shard ${i + 1} names '${id}', no part`);
  }
  if (!(table.budgetMinutes > 0) || !(table.setupMinutes >= 0) || !(table.marginMinutes >= 0)) errors.push('table: no budgetMinutes, setupMinutes or marginMinutes');
  for (const [i, m] of estimates(table).entries()) {
    if (!(m + table.marginMinutes <= table.budgetMinutes)) errors.push(`table: shard ${i + 1}/${shards.length} is estimated at ${m.toFixed(1)} minutes, over the ${table.budgetMinutes}-minute budget less its ${table.marginMinutes}-minute margin`);
  }
  // the CI job: a matrix of every shard, the shard in the environment, the guard kept
  const n = shards.length;
  const job = t.ci.match(/\n {2}selftest:\n([\s\S]*?)(?=\n {2}[a-z][a-z0-9-]*:\n|$)/);
  if (!job) errors.push('ci.yml: no `selftest` job');
  else {
    const body = job[1];
    if (!/\n\s+run: make selftest\s*\n|\n\s+make selftest\s*\n/.test(`${body}\n`)) errors.push('ci.yml `selftest`: no step runs make selftest');
    const got = [...body.matchAll(/^\s+- (\d+)\/(\d+)\s*$/gm)].map((m) => [Number(m[1]), Number(m[2])]);
    const want = Array.from({ length: n }, (_x, i) => `${i + 1}/${n}`).join(' ');
    if (got.map((s) => s.join('/')).join(' ') !== want) errors.push(`ci.yml \`selftest\`: the matrix shards are '${got.map((s) => s.join('/')).join(' ')}', not the table's ${want}`);
    if (!/OMX_SELFTEST_SHARD:\s*\$\{\{\s*matrix\.shard\s*\}\}/.test(body)) errors.push('ci.yml `selftest`: OMX_SELFTEST_SHARD is not the matrix shard');
    if (!/name: make selftest \$\{\{\s*matrix\.shard\s*\}\}/.test(body)) errors.push('ci.yml `selftest`: the job name does not carry the shard (make selftest ${{ matrix.shard }} ...)');
    if (!/fail-fast:\s*false/.test(body)) errors.push('ci.yml `selftest`: a red shard cancels the others (fail-fast is not false)');
    const timeout = body.match(/^ {4}timeout-minutes:\s*(\d+)/m);
    if (!timeout || Number(timeout[1]) !== TIMEOUT) errors.push(`ci.yml \`selftest\`: timeout-minutes is ${timeout?.[1] ?? 'unset'}, not the ${TIMEOUT}-minute guard`);
  }
  const make = t.makefile.match(/\nselftest:[^\n]*\n((?:\t[^\n]*\n)+)/);
  if (!make || !/\tOMX_SELFTEST_SHARD='\$\(OMX_SELFTEST_SHARD\)' node tools\/test\/recipe-selftest\.mjs\n/.test(make[1])) errors.push('Makefile: `selftest` does not pass OMX_SELFTEST_SHARD to the selftest');
  return errors;
}

export const tree = () => ({
  table: read('tools/test/selftest-shards.json'),
  selftest: read('tools/test/recipe-selftest.mjs'),
  ci: read('.github/workflows/ci.yml'),
  makefile: read('Makefile'),
});

const t = tree();
const errors = shardErrors(t);
for (const e of errors) console.log(`FAIL selftest-shards ${e}`);
let failed = errors.length;
if (!failed) {
  const table = JSON.parse(t.table);
  const est = estimates(table);
  console.log(`PASS selftest-shards: ${Object.keys(table.parts).length} parts, each in one of ${table.shards.length} shards; estimates ${est.map((m, i) => `${i + 1}/${est.length} ${m.toFixed(1)}`).join(', ')} min, slowest ${Math.max(...est).toFixed(1)}, under ${table.budgetMinutes} by ${(table.budgetMinutes - Math.max(...est)).toFixed(1)} (margin ${table.marginMinutes}); CI runs every shard under the ${TIMEOUT}-minute guard`);
}

// the arms prove each defect red on a tree that is whole; on a red tree they prove nothing
if (process.argv.includes('--self-test') && failed) console.log('FAIL selftest-shards sabotage: not run, the tree itself is red');
else if (process.argv.includes('--self-test')) {
  const table = JSON.parse(t.table);
  const n = table.shards.length;
  const swap = (s, a, b) => { if (!s.includes(a)) throw new Error(`self-test: "${a}" is not in the text it sabotages`); return s.replace(a, b); };
  const withTable = (fn) => { const x = structuredClone(table); fn(x); return JSON.stringify(x); };
  const matrixOf = (k) => Array.from({ length: k }, (_x, i) => `          - ${i + 1}/${k}\n`).join('');
  const ciMatrix = matrixOf(n);
  const arms = [
    ['a part dropped from the shard map', { ...t, table: withTable((x) => { x.shards[0] = x.shards[0].slice(1); }) }, 'in 0 shards'],
    ['a part in two shards', { ...t, table: withTable((x) => { x.shards[1].push(x.shards[0][0]); }) }, 'in 2 shards'],
    ['a part the selftest runs with no measured minutes', { ...t, selftest: `${t.selftest}\nawait part('sabotage-extra', async () => {});\n` }, "part 'sabotage-extra' has no measured minutes"],
    ['a measured part the selftest no longer runs', { ...t, selftest: t.selftest.replace(/\bpart\('chain'/, "part_('chain'") }, "'chain' is no part"],
    [`N too small: the parts packed longest-first into ${n - 1} shards, CI matching`, { ...t, table: withTable((x) => { x.shards = pack(table, n - 1); }), ci: swap(t.ci, ciMatrix, matrixOf(n - 1)) }, 'over the'],
    ['a part measured slower than its shard can hold', { ...t, table: withTable((x) => { x.parts.chain.minutes = 30; }) }, 'over the'],
    ['a shard left out of the CI matrix', { ...t, ci: swap(t.ci, `          - ${n}/${n}\n`, '') }, 'the matrix shards are'],
    ['a table of another N than the CI matrix', { ...t, table: withTable((x) => { x.shards.push([x.shards[0].pop()]); }) }, 'the matrix shards are'],
    ['the 120-minute guard lowered', { ...t, ci: t.ci.replace(/(\n {2}selftest:\n[\s\S]*?\n {4}timeout-minutes: )120/, '$190') }, 'not the 120-minute guard'],
    ['OMX_SELFTEST_SHARD not the matrix shard', { ...t, ci: swap(t.ci, 'OMX_SELFTEST_SHARD: ${{ matrix.shard }}', 'OMX_SELFTEST_SHARD: 1/1') }, 'not the matrix shard'],
    ['the Makefile no longer passing the shard', { ...t, makefile: swap(t.makefile, "\tOMX_SELFTEST_SHARD='$(OMX_SELFTEST_SHARD)' node", '\tnode') }, 'does not pass OMX_SELFTEST_SHARD'],
  ];
  for (const [name, sabotaged, needle] of arms) {
    let errs;
    try { errs = shardErrors(sabotaged); } catch (e) { errs = []; console.log(`FAIL selftest-shards sabotage: ${name}: ${e.message}`); }
    const hit = errs.find((e) => e.includes(needle));
    if (hit) console.log(`PASS selftest-shards sabotage: ${name} is red — ${hit}`);
    else { console.log(`FAIL selftest-shards sabotage: ${name} ${errs.length ? `is red for another reason: ${errs[0]}` : 'stays green'}`); failed++; }
  }
}
process.exit(failed ? 1 : 0);
