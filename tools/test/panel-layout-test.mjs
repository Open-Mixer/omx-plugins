#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * panel-layout-test.mjs — our plugins through omx-panel's layout (omx-panel docs/panel-layout.md). The layout's
 * own rules, each with an arm that breaks it, are omx-panel's suite; what is held here is what this repository
 * hands it, by value:
 *
 *   1. our authored descriptors state no unknown word, and every travel unit of ours is a descriptor unit;
 *   2. the layout with no declared panel finds the declared family and every role a meaning plays, for omx-delay
 *      and omx-comp, from the meanings their roles render to, so those are the ones the layout reads (the other
 *      controls' meanings are declared on the control, and a duck's threshold, range and release would read as a
 *      gate; a family is declared, never inferred, omx-contract#61 R1);
 *   3. every unit the param-contract table serves has a token a role reads.
 *
 * `make panel-layout`, part of `make test`. It loads the declarations, so it needs omx-contract and
 * omx-dsp as `make test` does.
 */
import { descriptorOf } from '../descriptor.mjs';
import { loadDecl, pluginDirs } from '../gen.mjs';
import { meaningOfRole, panelLayout, unitToken } from '../omx-panel.mjs';
import { contractUnits } from '../param-contract.mjs';

let bad = 0, n = 0;
const same = (a, b) => JSON.stringify(a) === JSON.stringify(b);
function check(what, got, want) {
  n++;
  const ok = same(got, want);
  if (!ok) bad++;
  console.log(`${ok ? 'PASS' : 'FAIL'} panel-layout ${what}${ok ? '' : `: got ${JSON.stringify(got)}, want ${JSON.stringify(want)}`}`);
}

const rolesOf = (x) => x.roles.map((r) => [r.role, r.index, r.symbol, r.unit]);

// 1 and 2. our plugins, through omx-panel's layout
const ITEM_UNITS = ['dB', 'dBFS', 'ms', 's', 'min', 'Hz', 'oct', 'cents', 'st', 'percent', 'coef', 'none', 'dBPerOct']; // the PARAM_UNITS ids (C2)
const DESCRIPTOR_UNITS = [...ITEM_UNITS, 'frames', 'bpm', 'midiNote', 'LUFS', 'deg', 'ratio']; // ratio: omx-contract 3.0.0
const decls = pluginDirs().map((p) => loadDecl(p)).filter((d) => d.panel);
const stated = decls.flatMap((d) => descriptorOf(d).descriptor.controls.filter((c) => c.meaning === 'unknown' || c.travel?.unit === 'unknown' || c.travel?.scale === 'unknown').map((c) => `${d.stem}.${c.name}`));
check(`our ${decls.length} authored descriptors state no unknown`, stated, []);
const words = decls.flatMap((d) => descriptorOf(d).descriptor.controls.filter((c) => c.travel && !DESCRIPTOR_UNITS.includes(c.travel.unit)).map((c) => `${d.stem}.${c.name} ${c.travel.unit}`));
check('every travel unit of ours is a descriptor unit', words, []);
// 3. the units the param-contract table serves are the ones omx-panel's roles read
check('every unit the param-contract table serves has a token a role reads', Object.entries(contractUnits()).filter(([, [, v]]) => v !== 0).map(([u]) => u).filter((u) => typeof unitToken(u) !== 'string'), []);
for (const stem of ['omx-delay', 'omx-comp']) {
  const d = decls.find((x) => x.stem === stem);
  const { descriptor: desc } = descriptorOf(d);
  const declared = panelLayout(desc, { panel: d.panel, ns: 'fx.x' });
  const bound = new Set(Object.values(d.panel.roles ?? {}).flat());
  const generic = panelLayout({ ...desc, controls: desc.controls.map((c) => (bound.has(c.symbol) ? c : { ...c, meaning: 'unknown' })) });
  // the roles a meaning plays: a role with no meaning word (a delay's pingpong) is bound only by a panel
  const meant = (x) => rolesOf(x).filter(([role]) => meaningOfRole(role)).sort();
  check(`${stem} with no declared panel: the declared family and every published role a meaning plays`, [generic.family, meant(generic)], [declared.family, meant(declared)]);
}

console.log(`panel-layout: ${n - bad}/${n} pass`);
process.exit(bad ? 1 : 0);
