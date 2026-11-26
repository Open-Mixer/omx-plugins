#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * curve-proof.mjs — a display marked `basis: exact` is the plugin's own maths, and the measurement says so (the
 * one-panel spec §5.3 and lane 5). Two holds:
 *
 *   1. the guard: every display omx-panel's layout marks `exact` (from a declared section's `basis`) is one PROOF
 *      proves, and every display PROOF proves is marked `exact`; neither set has a member the other lacks;
 *   2. the measurement: for each proved plugin at each of its settings, at 48 and 96 kHz, tools/test/curve-proof.c
 *      draws the curve (omx-dsp's omx_curve_points over the terms the layout binds, bound as a surface binds them)
 *      and measures the plugin's static response through libomx-clap: within 0.5 dB at every point.
 *
 * The terms are the layout's roles at the setting's values, each in omx-dsp's curve unit word for the unit the
 * description gives the role, a band type's shape the one its value selects from the role's `shapes`: the binding
 * omx-panel's MOD GUI generator makes for its static curve. The host writes the same values to the plugin.
 *
 *   node tools/test/curve-proof.mjs <curve-proof binary>              the guard, then every case
 *   node tools/test/curve-proof.mjs <curve-proof binary> --self-test  each sabotage must go red
 *   node tools/test/curve-proof.mjs --stems                           the proved plugins, one a line (what to build)
 *
 * `make curve-proof`, part of `make test-heavy`.
 */
import { spawnSync } from 'node:child_process';
import { resolve } from 'node:path';
import { descriptorOf, panelDescription } from '../descriptor.mjs';
import { loadDecl, pluginDirs } from '../gen.mjs';
import { isMain } from '../omx-contract.mjs';

/** The rates every case runs at (the spec's lane 5). */
const RATES = [48000, 96000];

/** An EQ setting's bands, by band number: [type value, Hz, dB, Q]; a band not listed is switched off. */
const eqSetting = (bands, rest = {}) => {
  const v = { hpf_on: 0, lpf_on: 0, ...rest };
  for (const [n, [type, freq, gain, q]] of Object.entries(bands)) Object.assign(v, { [`b${n}_on`]: 1, [`b${n}_type`]: type, [`b${n}_freq`]: freq, [`b${n}_gain`]: gain, [`b${n}_q`]: q });
  return v;
};
const BELL = 0, LOW_SHELF = 1, HIGH_SHELF = 2, NOTCH = 3; // EQ_BAND_TYPES values
const EQ_SETTINGS = {
  'one bell': eqSetting({ 1: [BELL, 1000, 6, 1] }),
  'shelves, bell, notch, high-pass': eqSetting(
    { 1: [LOW_SHELF, 100, -4.5, 0.7], 2: [BELL, 400, -8, 4], 3: [NOTCH, 3000, 0, 8], 4: [HIGH_SHELF, 8000, 3, 0.7] },
    { hpf_on: 1, hpf_freq: 40, hpf_slope: 24 }),
  'eight bells, both passes': eqSetting(
    { 1: [BELL, 60, 4, 0.7], 2: [BELL, 150, -3, 1.5], 3: [BELL, 350, 2, 2], 4: [BELL, 800, -5, 3], 5: [BELL, 1800, 6, 1],
      6: [BELL, 4000, -2, 0.5], 7: [BELL, 9000, 3, 2], 8: [BELL, 15000, -4, 1] },
    { hpf_on: 1, hpf_freq: 30, hpf_slope: 12, lpf_on: 1, lpf_freq: 12000, lpf_slope: 12 }),
};

/** The plugins whose curve display is proved, each with its three settings (values by parameter symbol; a parameter
 * not named stays at its default). omx-eq's declaration is its three variants'. */
export const PROOF = {
  'omx-comp': {
    default: { thresholdDb: -18, ratio: 4, kneeDb: 6, makeupDb: 0, attackMs: 5, releaseMs: 100 },
    'hard knee, low threshold, make-up': { thresholdDb: -40, ratio: 2, kneeDb: 0, makeupDb: 6, attackMs: 5, releaseMs: 100 },
    'steep, wide knee': { thresholdDb: -10, ratio: 20, kneeDb: 12, makeupDb: 3, attackMs: 5, releaseMs: 100 },
  },
  'omx-eq8': EQ_SETTINGS,
  'omx-eq16': EQ_SETTINGS,
  'omx-eq32': EQ_SETTINGS,
};

/** omx-dsp's curve unit word for a description's unit id: its id in lower case, but for the three omx-dsp spells
 * otherwise (omx-panel's MOD GUI generator reads them the same). */
const CURVE_UNIT = { percent: 'pc', x: 'coef', dBPerOct: 'db' };
const curveUnit = (id) => CURVE_UNIT[id] ?? String(id).toLowerCase();

/** Every curve display of our declarations, in the description our faces serve (omx-panel's layout of the descriptor
 * under the declared panel, tools/descriptor.mjs panelDescription): `{ stem, descriptor, layout, display }`. */
export function curveDisplays(decls = pluginDirs().map((p) => loadDecl(p)).filter((d) => d.panel)) {
  return decls.flatMap((d) => {
    const { descriptor } = descriptorOf(d);
    const layout = panelDescription(d);
    return layout.displays.filter((x) => x.source === 'family').map((display) => ({ stem: d.stem, descriptor, layout, display }));
  });
}

/** The guard: the displays marked exact against the plugins PROOF proves. A list of refusals, empty when they agree. */
export function guard(displays, proof = PROOF) {
  const errs = [];
  const exact = displays.filter((x) => x.display.basis === 'exact');
  for (const x of exact) if (!proof[x.stem]) errs.push(`${x.stem}'s ${x.display.kind} is marked exact, and no measurement proves it (add it to PROOF, or leave it modelled)`);
  for (const stem of Object.keys(proof)) {
    const mine = displays.filter((x) => x.stem === stem);
    if (!mine.length) errs.push(`${stem} is proved, and lays out no curve display`);
    for (const x of mine) if (x.display.basis !== 'exact') errs.push(`${stem}'s ${x.display.kind} is proved, and its declaration leaves it ${x.display.basis}: mark its section's graph \`basis: exact\``);
  }
  return errs;
}

/** The plan tools/test/curve-proof.c reads, for `cases` ([stem, setting name, rate]); `perturb` moves one term of the
 * curve (`{ term: [role, index, delta] }`) or one value the host writes (`{ param: [symbol, delta] }`). */
export function plan(displays, cases, perturb = {}) {
  const lines = [];
  for (const [stem, name, rate] of cases) {
    const x = displays.find((y) => y.stem === stem);
    const setting = PROOF[stem][name];
    const control = (symbol) => x.descriptor.controls.find((c) => c.symbol === symbol);
    const unknown = Object.keys(setting).filter((s) => !control(s));
    if (unknown.length) throw new Error(`curve-proof: ${stem} '${name}' sets ${unknown.join(', ')}, no parameter of it`);
    const valueOf = (c) => (c.symbol in setting ? setting[c.symbol]
      : c.kind === 'choice' ? (c.values ?? c.ids.map((_, i) => i))[Math.max(0, c.ids.indexOf(c.default))]
        : c.kind === 'toggle' ? (c.default === true ? 1 : 0) : (c.travel.default ?? c.travel.min));
    const path = resolve('plugins', stem, 'build', `${stem}.clap`);
    lines.push(['case', stem, name, rate, path, x.display.kind, x.layout.family].join('\t'));
    const bound = [...new Set([...x.layout.roles.map((r) => r.symbol), ...Object.keys(setting)])];
    for (const symbol of bound) {
      const c = control(symbol);
      const moved = perturb.param?.[0] === symbol ? perturb.param[1] : 0;
      lines.push(['param', c.name, valueOf(c) + moved].join('\t'));
    }
    for (const r of x.layout.roles) {
      const value = valueOf(control(r.symbol));
      const moved = perturb.term?.[0] === r.role && perturb.term[1] === r.index ? perturb.term[2] : 0;
      const shape = r.role === 'band_type' && r.shapes ? r.shapes[value] : '*';
      lines.push(['term', r.role, r.index, value + moved, curveUnit(r.unit), shape].join('\t'));
    }
    lines.push('end');
  }
  return `${lines.join('\n')}\n`;
}

/** Runs the binary over a plan: `{ status, out }`. */
function measure(bin, text) {
  const r = spawnSync(bin, [], { input: text, encoding: 'utf8' });
  return { status: r.status ?? 1, out: `${r.stdout ?? ''}${r.stderr ?? ''}` };
}

const allCases = () => Object.entries(PROOF).flatMap(([stem, settings]) => Object.keys(settings).flatMap((name) => RATES.map((rate) => [stem, name, rate])));

function main(argv) {
  if (argv[0] === '--stems') {
    console.log(Object.keys(PROOF).join('\n'));
    return 0;
  }
  const [bin, flag] = argv;
  if (!bin) {
    console.error('usage: curve-proof.mjs <curve-proof binary> [--self-test] | --stems');
    return 2;
  }
  const displays = curveDisplays();
  if (flag === '--self-test') return selfTest(resolve(bin), displays);
  const errs = guard(displays);
  for (const e of errs) console.log(`FAIL curve-proof guard: ${e}`);
  if (errs.length) return 1;
  console.log(`PASS curve-proof guard: the ${Object.keys(PROOF).length} proved plugins are the displays marked exact`);
  const cases = allCases();
  const r = measure(resolve(bin), plan(displays, cases));
  process.stdout.write(r.out);
  console.log(`curve-proof: ${cases.length} cases (${Object.keys(PROOF).length} plugins, ${Object.values(PROOF).map((s) => Object.keys(s).length).join('/')} settings, at ${RATES.join(' and ')} Hz): ${r.status === 0 ? 'all hold' : 'FAILED'}`);
  return r.status === 0 ? 0 : 1;
}

/** Each sabotage must go red, naming what it broke. */
function selfTest(bin, displays) {
  let bad = 0;
  const arm = (what, red, detail) => {
    if (!red) bad = 1;
    console.log(`${red ? 'SABOTAGE' : 'FAIL'} curve-proof ${what}: ${red ? `red: ${detail}` : 'STAYED GREEN'}`);
  };
  // the guard: a display marked exact that nothing proves, and a proved display left modelled
  const unproved = displays.find((x) => !PROOF[x.stem]).stem;
  const e1 = guard(displays.map((x) => (x.stem === unproved ? { ...x, display: { ...x.display, basis: 'exact' } } : x)));
  arm(`guard: ${unproved} marked exact with no proof`, e1.some((e) => e.startsWith(unproved)), e1[0]);
  const modelled = displays.map((x) => (x.stem === 'omx-comp' ? { ...x, display: { ...x.display, basis: 'modelled' } } : x));
  const e2 = guard(modelled);
  arm('guard: omx-comp proved and left modelled', e2.some((e) => e.startsWith('omx-comp')), e2[0]);
  // the measurement: the curve moved, and the plugin moved, each on one case that holds unmoved
  const comp = [['omx-comp', 'default', 48000]], eq = [['omx-eq8', 'one bell', 96000]];
  const base = measure(bin, plan(displays, [...comp, ...eq]));
  console.log(`${base.status === 0 ? 'PASS' : 'FAIL'} curve-proof self-test control, the two cases unmoved: ${base.out.trim().split('\n').join(' | ')}`);
  if (base.status !== 0) bad = 1;
  const curve = measure(bin, plan(displays, comp, { term: ['threshold', 0, 1] }));
  arm('the drawn curve\'s threshold moved 1 dB', curve.status === 1 && /FAIL curve-proof omx-comp 'default' at 48000 Hz: .* off by/.test(curve.out), curve.out.trim());
  const plugin = measure(bin, plan(displays, eq, { param: ['b1_gain', 1] }));
  arm('the plugin\'s band 1 gain moved 1 dB under the drawn curve', plugin.status === 1 && /FAIL curve-proof omx-eq8 'one bell' at 96000 Hz: .* off by/.test(plugin.out), plugin.out.trim());
  // the depth rule takes no moved notch: the plugin's notch off the drawn one is not deep where the curve is
  const notch = measure(bin, plan(displays, [['omx-eq8', 'shelves, bell, notch, high-pass', 48000]], { param: ['b3_freq', 150] }));
  arm('the plugin\'s notch moved 150 Hz off the drawn one', notch.status === 1 && /FAIL curve-proof omx-eq8 'shelves, bell, notch, high-pass' at 48000 Hz: .* off by/.test(notch.out), notch.out.trim());
  console.log(`curve-proof self-test: ${bad ? 'FAILED' : 'every sabotage red'}`);
  return bad;
}

if (isMain(import.meta.url)) process.exit(main(process.argv.slice(2)));
