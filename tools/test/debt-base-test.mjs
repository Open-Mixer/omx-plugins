#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas
/*
 * debt-base-test.mjs — the completeness debt's growth check (tools/plugin-recipe.mjs debtGrowth) compares
 * against a base it has fetched and resolved, and REFUSES, by name, a base it cannot resolve: in CI and
 * locally alike, never a note that passes and never HEAD against HEAD.
 *
 * Each arm builds a throwaway repository holding only recipes/completeness-debt.json:
 *  - main's debt lives on a bare origin whose main is NOT in the topic checkout's refs: the check fetches it;
 *  - a STALE origin/main (already holding the grown row) and a fetch that fails: refused, never compared;
 *  - origin/main absent and no remote to fetch from, CI unset: refused, naming origin/main;
 *  - OMX_DEBT_BASE_REF naming a ref that does not exist: refused, naming that ref;
 *  - a single-branch, depth-1 clone (the shape of a CI checkout) with its remote gone: refused;
 *  - the fetch leaves the checkout's refs as they were: no remote-tracking ref written, no tag brought (#102);
 *  - OMX_DEBT_BASE_REF=HEAD, a base that is HEAD's own commit: refused by name, never the debt against itself (#102);
 *  - a checkout of main itself, its origin/main HEAD's commit: compared, not refused.
 *
 *   node tools/test/debt-base-test.mjs      (make debt-base, part of make test)
 */
import { execFileSync } from 'node:child_process';
import { mkdtempSync, mkdirSync, writeFileSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import * as recipe from '../plugin-recipe.mjs';

const WORK = mkdtempSync(join(tmpdir(), 'debt-base-'));
process.on('exit', () => rmSync(WORK, { recursive: true, force: true }));
const git = (dir, ...a) => execFileSync('git', ['-C', dir, ...a], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] }).trim();
const ID = ['-c', 'user.name=t', '-c', 'user.email=t@example.invalid', '-c', 'commit.gpgsign=false'];
let bad = 0;
const expect = (ok, what) => { console.log(`${ok ? 'PASS' : 'FAIL'} ${what}`); if (!ok) bad = 1; };

const DEBT = [{ plugin: 'omx-drive', entry: 'manual-section', owedBy: 'test' }];
function repoWithDebt(dir) {
  mkdirSync(join(dir, 'recipes'), { recursive: true });
  writeFileSync(join(dir, recipe.DEBT_PATH), JSON.stringify({ debt: DEBT }));
  git(dir, 'init', '-q', '-b', 'main');
  git(dir, 'add', '-A');
  git(dir, ...ID, 'commit', '-q', '-m', 'main');
}
/** The verdict the completeness run prints for this growth, with CI unset. */
function verdictWithoutCI(growth) {
  const saved = process.env.CI;
  delete process.env.CI;
  try {
    return recipe.growthVerdict ? recipe.growthVerdict(growth) : null;
  } finally {
    if (saved !== undefined) process.env.CI = saved;
  }
}
function withRef(ref, fn) {
  const saved = process.env.OMX_DEBT_BASE_REF;
  if (ref === undefined) delete process.env.OMX_DEBT_BASE_REF; else process.env.OMX_DEBT_BASE_REF = ref;
  try { return fn(); } finally { if (saved === undefined) delete process.env.OMX_DEBT_BASE_REF; else process.env.OMX_DEBT_BASE_REF = saved; }
}

// 1. the base is fetched before it is resolved
{
  const src = join(WORK, 'src');
  repoWithDebt(src);
  const origin = join(WORK, 'origin.git');
  execFileSync('git', ['clone', '-q', '--bare', src, origin]);
  const topic = join(WORK, 'topic');
  execFileSync('git', ['clone', '-q', origin, topic]);
  git(topic, 'checkout', '-q', '-b', 'topic');
  git(topic, 'update-ref', '-d', 'refs/remotes/origin/main');
  // a tag on the origin the checkout does not hold: a fetch with tags would bring it
  git(origin, 'tag', 'v9.9.9', 'main');
  const refsBefore = git(topic, 'for-each-ref', '--format=%(refname) %(objectname)');
  const g = withRef(undefined, () => recipe.debtGrowth(topic, DEBT));
  expect(g.base === DEBT.length, `the base is fetched before it is resolved: origin/main missing from the checkout, read from the remote (${g.base ?? g.why})`);
  const refsAfter = git(topic, 'for-each-ref', '--format=%(refname) %(objectname)').split('\n').filter((l) => !l.startsWith('refs/omx-debt-base '));
  expect(refsAfter.join('\n') === refsBefore, `the fetch leaves the checkout's refs as they were: no refs/remotes/origin/main written, no tag brought (${refsAfter.filter((l) => !refsBefore.split('\n').includes(l)).join(', ') || 'none changed'})`);
}

// 1b. a STALE origin/main and a fetch that fails: refused, never compared with the stale base
{
  const src = join(WORK, 'src1b');
  repoWithDebt(src);
  const origin = join(WORK, 'origin1b.git');
  execFileSync('git', ['clone', '-q', '--bare', src, origin]);
  const topic = join(WORK, 'topic1b');
  execFileSync('git', ['clone', '-q', origin, topic]);
  git(topic, 'checkout', '-q', '-b', 'topic');
  // the checkout's origin/main now points at a commit whose debt already holds a grown row ...
  const grown = [...DEBT, { plugin: 'omx-comp', entry: 'manual-section', owedBy: 'stale' }];
  writeFileSync(join(topic, recipe.DEBT_PATH), JSON.stringify({ debt: grown }));
  git(topic, ...ID, 'commit', '-q', '-am', 'grown');
  git(topic, 'update-ref', 'refs/remotes/origin/main', 'HEAD');
  // ... and the remote cannot be reached
  rmSync(origin, { recursive: true, force: true });
  const g = withRef(undefined, () => recipe.debtGrowth(topic, grown));
  const v = verdictWithoutCI(g);
  expect(v !== null && v.bad === 1 && /origin\/main/.test(v.line) && /fetch/.test(v.line), `a stale origin/main with a failed fetch is refused by name, not compared (${v ? (v.line || 'passed: base ' + g.base).slice(0, 90) : 'no growthVerdict'})`);
}

// 2. no base and nothing to fetch it from, CI unset: refused by name
{
  const lone = join(WORK, 'lone');
  repoWithDebt(lone);
  git(lone, 'checkout', '-q', '-b', 'topic');
  const g = withRef(undefined, () => recipe.debtGrowth(lone, DEBT));
  const v = verdictWithoutCI(g);
  expect(g.base === undefined && /origin\/main/.test(g.why || ''), `no origin/main: the growth check names the ref it could not resolve (${(g.why || 'no reason').slice(0, 90)})`);
  expect(v !== null && v.bad === 1 && /^FAIL/.test(v.line) && /origin\/main/.test(v.line), `no origin/main and CI unset: the completeness run REFUSES, naming the ref (${v ? v.line.slice(0, 90) : 'no growthVerdict: the miss is decided in main(), a FAIL only under CI'})`);
}

// 3. a misnamed base ref: refused by that name
{
  const named = join(WORK, 'named');
  repoWithDebt(named);
  git(named, 'checkout', '-q', '-b', 'topic');
  const g = withRef('origin/no-such-branch', () => recipe.debtGrowth(named, DEBT));
  const v = verdictWithoutCI(g);
  expect(g.base === undefined && v !== null && v.bad === 1 && /origin\/no-such-branch/.test(v.line), `OMX_DEBT_BASE_REF naming no ref is refused by that name (${v ? v.line.slice(0, 90) : 'no growthVerdict'})`);
}

// 4. a CI-shaped checkout: single branch, depth 1, its remote gone
{
  const src = join(WORK, 'src4');
  repoWithDebt(src);
  git(src, 'checkout', '-q', '-b', 'topic');
  git(src, ...ID, 'commit', '-q', '--allow-empty', '-m', 'topic');
  const shallow = join(WORK, 'shallow');
  execFileSync('git', ['clone', '-q', '--single-branch', '--branch', 'topic', '--depth', '1', `file://${src}`, shallow]);
  git(shallow, 'remote', 'remove', 'origin');
  const g = withRef(undefined, () => recipe.debtGrowth(shallow, DEBT));
  const v = verdictWithoutCI(g);
  expect(g.base === undefined && v !== null && v.bad === 1 && /origin\/main/.test(v.line), `a single-branch depth-1 checkout with no main is refused by name, not passed (${v ? v.line.slice(0, 90) : 'no growthVerdict'})`);
}

// 5. OMX_DEBT_BASE_REF=HEAD: the merge-base is HEAD, the debt compared with itself could never grow: refused by name
{
  const self = join(WORK, 'self');
  repoWithDebt(self);
  git(self, 'checkout', '-q', '-b', 'topic');
  const grown = [...DEBT, { plugin: 'omx-comp', entry: 'manual-section', owedBy: 'self' }];
  writeFileSync(join(self, recipe.DEBT_PATH), JSON.stringify({ debt: grown }));
  git(self, ...ID, 'commit', '-q', '-am', 'grown');
  const g = withRef('HEAD', () => recipe.debtGrowth(self, grown));
  const v = verdictWithoutCI(g);
  expect(g.base === undefined && v !== null && v.bad === 1 && /the base HEAD resolves to HEAD's own commit/.test(v.line), `OMX_DEBT_BASE_REF=HEAD is refused by name, never the debt against itself (${v ? (v.line || 'passed: base ' + g.base).slice(0, 110) : 'no growthVerdict'})`);
}

// 6. main itself: a checkout whose origin/main is HEAD's commit compares with it (a run on main), never refused
{
  const src = join(WORK, 'src6');
  repoWithDebt(src);
  const origin = join(WORK, 'origin6.git');
  execFileSync('git', ['clone', '-q', '--bare', src, origin]);
  const onMain = join(WORK, 'onmain');
  execFileSync('git', ['clone', '-q', origin, onMain]);
  const g = withRef(undefined, () => recipe.debtGrowth(onMain, DEBT));
  expect(g.base === DEBT.length && verdictWithoutCI(g).bad === 0, `a checkout of main itself compares with origin/main, HEAD's own commit (${g.base ?? g.why})`);
}

console.log(bad ? 'debt-base: FAILED' : 'debt-base: green');
process.exit(bad);
