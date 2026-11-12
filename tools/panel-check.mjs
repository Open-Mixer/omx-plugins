#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * panel-check.mjs — what a BUILT face serves as org.openmixer.panel-description/1, as tools/
 * panel-probe.c read it through CLAP or LV2, against the plugin's DECLARATION:
 *
 *   - the sections are the declaration's panel sections, in order, with their labels;
 *   - the controls are every parameter the sections list, in order, each in its section, labelled
 *     with the parameter's name, its id the parameter's (CLAP: its declared index; LV2: the port
 *     whose symbol is the parameter's, read from the bundle);
 *   - the family, widgets, label keys, roles, displays and placement defaults are the description
 *     omx-panel's layout derives from the declaration;
 *   - the fingerprint is the SHA-256 of the canonical serialisation of what was served, and that of
 *     the declaration's.
 *
 * Usage: node tools/panel-check.mjs plugins/<stem> clap|lv2 <probe output>
 */
import { readFileSync } from 'node:fs';
import { panelDescription } from './descriptor.mjs';
import { loadDecl } from './gen.mjs';
import { DISPLAY_KINDS, DISPLAY_SOURCES, FAMILIES, PLACEMENTS, WIDGETS, panelCanonical, panelFingerprint } from './omx-panel.mjs';

const [dir, face, dump] = process.argv.slice(2);
const d = loadDecl(dir);
const want = panelDescription(d);
const rows = readFileSync(dump, 'utf8').split('\n').filter(Boolean).map((l) => l.split('\t'));
const of = (k) => rows.filter((r) => r[0] === k).map((r) => r.slice(1));
const NONE = 4294967295;

// a served parameter id -> its symbol: the declared index on CLAP, the port's symbol on LV2
const ports = new Map(of('port').map(([i, sym]) => [Number(i), sym]));
const symbolOf = (id) => {
  const n = Number(id);
  if (n === NONE) return null;
  return face === 'lv2' ? ports.get(n) ?? `?port${n}` : d.params[n]?.symbol ?? `?id${n}`;
};

let bad = 0;
const check = (ok, what, detail = '') => {
  if (!ok) bad++;
  console.log(`${ok ? 'PASS' : 'FAIL'} panel ${face} ${what}${ok || !detail ? '' : `: ${detail}`}`);
};
const same = (a, b) => JSON.stringify(a) === JSON.stringify(b);

// what was served, in the description's shape
const sections = of('section').map(([key, labelKey, label]) => ({ key, labelKey, label }));
const keyOf = (i) => sections[Number(i)]?.key ?? `?section${i}`;
// a governing control as the description names it: the parameter's declared id, the same on both faces
const declaredId = new Map(want.controls.map((c) => [c.symbol, c.param]));
const governing = (id) => {
  const sym = symbolOf(id);
  return sym === null ? null : declaredId.get(sym) ?? `?${sym}`;
};
// a display's floor as the description says it: a parameter's symbol, a level in dB, or none. A face serves none as no
// parameter and 0 dB, whatever the display's kind: omx-panel lays the gain-reduction bar out from the meters with no floor
const floorOf = (id, db) => (Number(id) !== NONE ? symbolOf(id) : Number(db) !== 0 ? Number(db) : null);
const served = {
  family: FAMILIES[Number(of('family')[0]?.[0])] ?? '?',
  sections,
  controls: of('control').map(([id, section, widget, labelKey, visible, mask, label]) => ({
    symbol: symbolOf(id), sectionKey: keyOf(section), widget: WIDGETS[Number(widget)] ?? '?', labelKey, visible: governing(visible), mask: BigInt(`0x${mask}`), label,
  })),
  roles: of('role').map(([role, index, id]) => ({ role, index: Number(index), symbol: symbolOf(id) })),
  displays: of('display').map(([kind, section, source, m0, m1, curve, floorId, floorDb, labelKey, label]) => ({
    kind: DISPLAY_KINDS[Number(kind)] ?? '?', sectionKey: keyOf(section), source: DISPLAY_SOURCES[Number(source)] ?? '?',
    meters: [m0, m1].map((m) => (Number(m) === NONE ? null : Number(m))), curve: source === '3' ? Number(curve) : null,
    floor: floorOf(floorId, floorDb), labelKey, label,
  })),
  defaults: of('default').map(([placement, id, value]) => ({ placement: PLACEMENTS[Number(placement)] ?? '?', symbol: symbolOf(id), value: Number(value) })),
};

// the declaration, read directly: its sections, and each listed parameter with its name
const declSections = d.panel.sections.map((s) => s.label);
const declControls = d.panel.sections.flatMap((s, i) => s.controls.map((sym) => [sym, i, d.params.find((p) => p.symbol === sym)?.name]));
check(same(served.sections.map((s) => s.label), declSections), `sections are the declaration's (${declSections.length})`, JSON.stringify(served.sections.map((s) => s.label)));
check(
  same(served.controls.map((c) => [c.symbol, served.sections.findIndex((s) => s.key === c.sectionKey), c.label]), declControls),
  `controls are the declaration's parameters, in its sections and order (${declControls.length})`,
  JSON.stringify(served.controls.map((c) => c.symbol)),
);
check(served.family === want.family, `family ${want.family}`, served.family);
check(same(served.sections, want.sections.map(({ key, labelKey, label }) => ({ key, labelKey, label }))), 'section keys and label keys');
const ctl = (c) => [c.symbol, c.sectionKey, c.widget, c.labelKey, c.visible, c.mask.toString(16)];
check(same(served.controls.map(ctl), want.controls.map(ctl)), 'control widgets, label keys and visibility', JSON.stringify(served.controls.map(ctl)));
const role = (r) => [r.role, r.index, r.symbol];
check(same(served.roles.map(role), want.roles.map(role)), `roles (${want.roles.length})`, JSON.stringify(served.roles.map(role)));
const disp = (v) => [v.kind, v.sectionKey, v.source, v.meters, v.curve, v.floor === 0 ? null : v.floor, v.labelKey, v.label];
check(same(served.displays.map(disp), want.displays.map(disp)), `displays (${want.displays.length})`, JSON.stringify(served.displays.map(disp)));
const def = (p) => [p.placement, p.symbol, p.value];
check(same(served.defaults.map(def), want.defaults.map(def)), `placement defaults (${want.defaults.length})`, JSON.stringify(served.defaults.map(def)));
const fingerprint = of('fingerprint')[0]?.[0];
check(fingerprint === panelFingerprint(served), 'the fingerprint is that of what it serves', `${fingerprint} served, ${panelFingerprint(served)} computed over:\n${panelCanonical(served)}`);
check(fingerprint === panelFingerprint(want), "the fingerprint is the declaration's", `${fingerprint}, the declaration's ${panelFingerprint(want)}`);
process.exit(bad ? 1 : 0);
