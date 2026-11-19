#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * commit-plan-check.mjs <base>..<head> [--repo <dir>] — a plugin's range of commits keeps the
 * commit protocol the plugin recipe declares (recipes/plugin.recipe.json, `layers`):
 *
 *   1. one concern per commit: the files a commit touches map to ONE layer;
 *   2. the layers come in the declared order (spec, contract, kernel, generated, code, consumer,
 *      tests, docs): never a face before its declaration, never a consumer before what it consumes.
 *
 * A file maps to a layer through the recipe's artifact `paths` (any stem, any kernel); a file no
 * artifact names (the tools, the recipe itself) is outside the protocol and ignored. It reads the
 * recipe of the tree it runs from. Exit 0 the range keeps the protocol; 1 it does not (each
 * violation named with its commit; an empty range is one); 2 usage.
 */
import { execFileSync } from 'node:child_process';
import { resolve } from 'node:path';
import { ROOT, layerOfPath, loadRecipe } from './plugin-recipe.mjs';
import { isMain } from './omx-contract.mjs';

/** The commits of a range, oldest first: `{ sha, subject, files }`. */
export function rangeCommits(repo, range) {
  const out = execFileSync('git', ['-C', repo, 'log', '--reverse', '--no-merges', '--name-only', '--format=%x00%H%x09%s', range], { encoding: 'utf8' });
  return out.split('\0').filter(Boolean).map((block) => {
    const [head, ...files] = block.split('\n');
    const [sha, subject] = head.split('\t');
    return { sha, subject, files: files.filter(Boolean) };
  });
}

/** The protocol's verdict over a list of commits. */
export function checkCommits(recipe, commits) {
  const order = recipe.layers.map((l) => l.id);
  const violations = [];
  const lines = [];
  let last = -1;
  let lastCommit;
  if (commits.length === 0) violations.push('the range holds no commit: a check over nothing proves nothing (is <base> the branch point and <head> the tip?)');
  for (const c of commits) {
    const layers = [...new Set(c.files.map((f) => layerOfPath(recipe, f)).filter(Boolean))];
    const short = `${c.sha.slice(0, 9)} ${c.subject}`;
    if (layers.length === 0) {
      lines.push(`  -- ${short}: outside the protocol (no plugin artifact)`);
      continue;
    }
    if (layers.length > 1) {
      const by = layers.map((l) => `${l}: ${c.files.filter((f) => layerOfPath(recipe, f) === l).join(', ')}`);
      violations.push(`${short}: ${layers.length} concerns in one commit (${by.join('; ')})`);
    }
    const idx = Math.max(...layers.map((l) => order.indexOf(l)));
    const first = Math.min(...layers.map((l) => order.indexOf(l)));
    if (first < last) {
      violations.push(`${short}: layer ${order[first]} after ${order[last]} (${lastCommit}); the order is ${order.join(' → ')}`);
    }
    lines.push(`  ${layers.join('+')} ${short}`);
    if (idx >= last) {
      last = idx;
      lastCommit = short;
    }
  }
  return { ok: violations.length === 0, violations, lines };
}

function main(argv) {
  const i = argv.indexOf('--repo');
  const repo = resolve(i >= 0 ? argv[i + 1] : ROOT);
  const range = argv.find((a, k) => !a.startsWith('--') && argv[k - 1] !== '--repo');
  if (!range || !range.includes('..')) {
    process.stderr.write('usage: commit-plan-check.mjs <base>..<head> [--repo <dir>]\n');
    process.exit(2);
  }
  const recipe = loadRecipe(ROOT);
  const v = checkCommits(recipe, rangeCommits(repo, range));
  for (const l of v.lines) console.log(l);
  for (const x of v.violations) console.log(`FAIL commit protocol: ${x}`);
  console.log(v.ok ? `PASS commit protocol: ${range} keeps one concern per commit, in layer order` : `commit protocol: ${v.violations.length} violation(s) in ${range}`);
  process.exit(v.ok ? 0 : 1);
}

if (isMain(import.meta.url)) main(process.argv.slice(2));
