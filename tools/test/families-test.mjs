#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * families-test.mjs — every effect serves the panel family it really is, and the ones no family fits say so.
 * Compared by value against the panel description omx-panel derives from each declaration:
 *
 *   1. omx-pitch serves family pitch with its shift, fine and mix roles, none of them unpublished, and its authored
 *      units (st, cents) are served as the contract's names (semitone12TET, cent);
 *   2. each effect below serves its family (never none), and every role its declaration binds is published;
 *   3. the effects no family fits (balancer, transient, suboct) stay family none and each declaration names the
 *      TODO: the utility family, or the control meaning its family would stand on.
 *
 * `make families`, part of `make test`.
 */
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { panelDescription } from '../descriptor.mjs';
import { loadDecl } from '../gen.mjs';
import { unitToken } from '../omx-panel.mjs';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
const at = (stem) => join(ROOT, 'plugins', stem);
let bad = 0, n = 0;
const check = (ok, what, detail = '') => {
  n++;
  if (!ok) bad++;
  console.log(`${ok ? 'PASS' : 'FAIL'} families ${what}${ok ? '' : `: ${detail}`}`);
};
const served = (stem) => panelDescription(loadDecl(at(stem)));

// 1. pitch
const pitch = served('omx-pitch');
const roles = (p) => p.roles.map((r) => r.role);
check(pitch.family === 'pitch', 'omx-pitch serves family pitch', pitch.family);
check(['shift', 'fine', 'mix'].every((r) => roles(pitch).includes(r)), 'omx-pitch serves its shift, fine and mix roles', JSON.stringify(roles(pitch)));
check(pitch.unpublished.length === 0, 'omx-pitch publishes every role it binds', JSON.stringify(pitch.unpublished));
// the descriptor's authored words st and cents are served as the contract's own unit names
const tokens = Object.fromEntries(pitch.roles.map((r) => [r.role, unitToken(r.unit)]));
check(tokens.shift === 'semitone12TET' && tokens.fine === 'cent', "omx-pitch's units are served as the contract's names", JSON.stringify(tokens));

// 2. the effects that served family none now serve their own
const FAMILY = {
  'omx-eq8': 'eq', 'omx-eq16': 'eq', 'omx-eq32': 'eq',
  'omx-glue': 'dynamics', 'omx-upward': 'dynamics', 'omx-loudlevel': 'dynamics',
  'omx-plosive': 'deesser', 'omx-clipper': 'drive', 'omx-spring': 'reverb',
};
for (const [stem, family] of Object.entries(FAMILY)) {
  const p = served(stem);
  check(p.family === family, `${stem} serves family ${family}`, p.family);
  if (!stem.startsWith('omx-eq')) check(p.unpublished.length === 0, `${stem} publishes every role it binds`, JSON.stringify(p.unpublished));
}

// 3. no family fits: still none, and the declaration says what it waits for
for (const stem of ['omx-balancer', 'omx-transient', 'omx-suboct']) {
  const d = loadDecl(at(stem));
  check(d.panel.family === 'none' && /TODO/.test(d.panel.$comment ?? '') && /utility|meaning/.test(d.panel.$comment ?? ''), `${stem} stays none and names its TODO`, JSON.stringify(d.panel.$comment));
}

console.log(`families: ${n - bad}/${n} pass`);
process.exit(bad ? 1 : 0);
