#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * comp-release-mode-test.mjs — the compressor's release mode is a parameter of omx-comp, read from the kernel's
 * declaration and never restated: the descriptor carries a `releaseMode` choice linked to omx-dsp's
 * COMP_RELEASE_MODES (fixed, opto, auto, in the kernel's order, `fixed` the come-up value), and the declaration no longer
 * holds it at the kernel's default.
 *
 *   1. PORT. The descriptor has the `releaseMode` choice, kind choice, three values 0, 1, 2, default 0, whose source
 *      link names COMP_RELEASE_MODES.
 *   2. NOT HELD. omx-comp.decl.json's `held` no longer lists it, and omx_comp_core.h passes the parameter, not the
 *      kernel default, to the instance face's release_mode argument.
 *
 * `make comp-release-mode`, part of `make test`. Exit 1 on any failure.
 */
import { readFileSync } from 'node:fs';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const dir = join(ROOT, 'plugins', 'omx-comp');
const desc = JSON.parse(readFileSync(join(dir, 'omx-comp.descriptor.json'), 'utf8'));
const decl = JSON.parse(readFileSync(join(dir, 'omx-comp.decl.json'), 'utf8'));
const core = readFileSync(join(dir, 'omx_comp_core.h'), 'utf8');
let failed = 0;
const ok = (c, what) => {
  if (!c) { failed++; console.log(`FAIL ${what}`); }
};

const p = desc.controls.find((c) => c.symbol === 'releaseMode');
ok(!!p, 'the descriptor has a releaseMode control');
if (p) {
  ok(p.kind === 'choice', 'releaseMode is a choice');
  ok(JSON.stringify(p.values) === '[0,1,2]', 'releaseMode has the three kernel values 0, 1, 2');
  ok(String(p.default) === '0', 'releaseMode comes up fixed (0)');
  ok(p.source?.from?.link === 'data/kernels/comp.json#COMP_RELEASE_MODES', 'releaseMode is read from the kernel declaration COMP_RELEASE_MODES');
}
ok(!(decl.held ?? []).includes('releaseMode'), 'the declaration does not hold releaseMode');
ok(/release_mode \*\/ \(int\)lrintf\(values\[OMX_COMP_PARAM_RELEASE_MODE\]\)/.test(core), 'the core passes the releaseMode parameter to the instance face');

console.log(`comp-release-mode: ${failed === 0 ? 'ok' : failed + ' failed'}`);
process.exit(failed === 0 ? 0 : 1);
