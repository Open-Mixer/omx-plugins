#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * round-trip-ledger-check.mjs — the round-trip ledger's counts are the run's, never raised by hand (omx-plugins#77).
 * tools/round-trip-losses.generated.json holds the losses per plugin; `node tools/round-trip.mjs --check` (run by
 * `make completeness`) fails, naming the plugin, when the tree and the ledger disagree on which plugins are described.
 *
 *   1. this tree: the check passes;
 *   2. a fixture plugin added to the tree, the ledger unchanged: the check fails and names it;
 *   3. a plugin gone from the tree, the ledger unchanged: the check fails and names it;
 *   4. the guard is wired: `make completeness` runs the check, and a Makefile without that line is caught.
 * Exit 0 pass, 1 refused.
 */
import { execFileSync, spawnSync } from 'node:child_process';
import { mkdirSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { described, readLedger, ROOT } from '../round-trip.mjs';
import { descriptorFile } from '../descriptor.mjs';

let fails = 0;
const ok = (cond, what) => { console.log(`${cond ? 'ok  ' : 'FAIL'} ${what}`); if (!cond) fails++; };
const check = (root) => spawnSync('node', [join(ROOT, 'tools/round-trip.mjs'), '--check', ...(root ? ['--root', root] : [])], { encoding: 'utf8' });
const tmp = mkdtempSync(join(process.env.TMPDIR ?? tmpdir(), 'ledger-check-'));
const fixtureTree = (stems) => {
  const root = mkdtempSync(join(tmp, 'tree-'));
  for (const s of stems) { mkdirSync(join(root, 'plugins', s), { recursive: true }); writeFileSync(join(root, 'plugins', s, descriptorFile(s)), '{}\n'); }
  return root;
};
try {
  const inside = described().inside;
  ok(inside.length > 0 && check().status === 0, 'this tree: the ledger names every described plugin');

  const added = check(fixtureTree([...inside, 'omx-fixture-new']));
  ok(added.status === 1 && /ledger lacks omx-fixture-new/.test(added.stdout), 'a fixture plugin added, the ledger unchanged: red, naming it');

  const gone = check(fixtureTree(inside.slice(1)));
  ok(gone.status === 1 && gone.stdout.includes(`ledger holds ${inside[0]}`), 'a plugin gone from the tree, the ledger unchanged: red, naming it');

  ok(Object.keys(readLedger()).length === inside.length, 'the ledger holds one entry per described plugin');

  const make = readFileSync(join(ROOT, 'Makefile'), 'utf8');
  const wired = (text) => /^completeness:\n(\t.*\n)*?\tnode tools\/round-trip\.mjs --check\n/m.test(text);
  ok(wired(make), 'make completeness runs the ledger check');
  ok(!wired(make.replace('\tnode tools/round-trip.mjs --check\n', '')), 'sabotage: a completeness without the check goes unwired (the guard cannot go blind unseen)');
} finally {
  rmSync(tmp, { recursive: true, force: true });
}
process.exit(fails ? 1 : 0);
