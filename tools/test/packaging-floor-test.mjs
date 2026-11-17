#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas
/*
 * packaging-floor-test.mjs — the releases this tree builds against are FLOORS only: tools/omx-contract.mjs reads
 * `BuildRequires: <pkg> >= <floor>` from packaging/omx-plugins.spec (generated from packaging/deps.txt) and refuses a
 * release below it, and no ceiling is enforced. Arms: the floors-only line is read; a release above the old ceiling
 * is accepted; one below the floor is refused naming the floor; a line missing or written twice is refused.
 * `make packaging-floor`, part of `make test`.
 */
import { mkdirSync, mkdtempSync, writeFileSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { locateContract, packagingFloor } from '../omx-contract.mjs';

let failed = 0;
const expect = (ok, what) => { console.log(`${ok ? 'ok  ' : 'FAIL'} ${what}`); if (!ok) failed = 1; };

const root = mkdtempSync(join(tmpdir(), 'packaging-floor-'));
const data = join(root, 'data');
mkdirSync(join(root, 'packaging'));
mkdirSync(data);
const spec = (text) => writeFileSync(join(root, 'packaging', 'omx-plugins.spec'), text);
const installed = (version) => writeFileSync(join(data, 'omx-contract.json'), JSON.stringify({ version, kernels: { k: {} }, items: {} }));
const throws = (fn) => { try { fn(); } catch (e) { return e.message; } return undefined; };

spec('BuildRequires: omx-contract-devel >= 3.13.0\nBuildRequires: omx-dsp-devel >= 0.32.0\n');
expect(packagingFloor('omx-contract-devel', root) === '3.13.0', 'a floors-only BuildRequires line is read');
expect(packagingFloor('omx-dsp-devel', root) === '0.32.0', 'the next dependency is read from its own line');

process.env.OMX_CONTRACT_DIR = data;
installed('3.13.0');
expect(locateContract(root).dir === data, 'the floor itself is accepted');
installed('9.4.0');
expect(locateContract(root).dir === data, 'a release far above any former ceiling is accepted: no ceiling is enforced');
installed('3.12.9');
const below = locateContract(root);
expect(below.dir === undefined && /3\.12\.9/.test(below.why) && /3\.13\.0/.test(below.why) && !/ and < /.test(below.why), 'a release below the floor is refused, naming the floor and no ceiling');

spec('BuildRequires: omx-dsp-devel >= 0.32.0\n');
expect(/no "BuildRequires: omx-contract-devel >= <floor>" line/.test(throws(() => packagingFloor('omx-contract-devel', root)) ?? ''), 'a missing line is refused');
spec('BuildRequires: omx-contract-devel >= 3.13.0\nBuildRequires: omx-contract-devel >= 3.14.0\n');
expect(/more than one/.test(throws(() => packagingFloor('omx-contract-devel', root)) ?? ''), 'a line written twice is refused');
spec('BuildRequires: (omx-contract-devel >= 3.13.0 with omx-contract-devel < 4)\n');
expect(throws(() => packagingFloor('omx-contract-devel', root)) !== undefined, 'the old ceiling form is not a floors-only line');

rmSync(root, { recursive: true, force: true });
process.exit(failed);
