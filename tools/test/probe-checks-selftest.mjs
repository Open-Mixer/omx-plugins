#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * probe-checks-selftest.mjs — the four checkers that read a BUILT face back against its declaration
 * (clap-params-check, panel-check, clap-ext-check, port-groups-check) each go red when the face is
 * wrong. A checker with no failing arm is decoration: it would stay green over a face that lost a
 * parameter, a section, a unit or a group, and nobody would see it.
 *
 * For each checker the real probe's output is read from a built plugin (omx-reverb, four sections and a
 * placement default; omx-keyed-gate for the one display), the checker is run on it unchanged and must
 * pass, and then on copies of it with ONE thing changed at a time (an arm): each must exit 1 and name,
 * in a FAIL line, the check that caught it. Scratch is under the temporary directory.
 *
 * `make probe-checks`, part of `make test`; it needs the plugins' probes built (lilv, as `make test`).
 */
import { spawnSync } from 'node:child_process';
import { mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
const { loadDecl, lv2Ports, scaleOf } = await import('../gen.mjs');
const { contractFingerprint, contractOf, scalesOf } = await import('../param-contract.mjs');
const { locateContract } = await import('../omx-contract.mjs');
const contract = locateContract(ROOT);
if (!contract.dir) throw new Error(`probe-checks: ${contract.why}`);
const SCALES = scalesOf(contract.dir);
const TMP = mkdtempSync(join(tmpdir(), 'probe-checks-'));
process.on('exit', () => rmSync(TMP, { recursive: true, force: true }));

const run = (cmd, args, opts = {}) => spawnSync(cmd, args, { encoding: 'utf8', cwd: ROOT, ...opts });

/** Build the plugin and its four probes, and read what each prints: `{ dir, uri, dumps }`. */
function dumpsOf(stem) {
  const dir = join(ROOT, 'plugins', stem);
  const b = run('make', ['-s', '-C', dir, 'all', 'build/clap-probe', 'build/panel-probe', 'build/clap-ext-probe', 'build/port-groups-probe']);
  if (b.status !== 0) throw new Error(`${stem} does not build:\n${b.stderr}`);
  const uri = /^URI\s*:=\s*(\S+)/m.exec(readFileSync(join(dir, 'Makefile'), 'utf8'))[1];
  const bundle = join(dir, 'build', `${stem}.lv2`), clap = join(dir, 'build', `${stem}.clap`);
  const probe = (name, args) => {
    const r = run(join(dir, 'build', name), args);
    if (r.status !== 0) throw new Error(`${stem} ${name}: exit ${r.status}\n${r.stderr}`);
    return r.stdout;
  };
  return {
    dir, uri,
    dumps: {
      params: probe('clap-probe', ['params', clap]),
      panelClap: probe('panel-probe', ['clap', clap]),
      panelLv2: probe('panel-probe', ['lv2', bundle, uri]),
      ext: probe('clap-ext-probe', [clap]),
      groups: probe('port-groups-probe', [bundle, uri]),
    },
  };
}

// ---- the changes: each takes the rows of a dump (arrays of columns) and returns the changed rows ----
const nth = (rows, kind, n) => rows.map((r, i) => [r, i]).filter(([r]) => r[0] === kind)[n]?.[1];
const need = (i, what) => {
  if (i === undefined) throw new Error(`the probe output has no ${what}: the arm cannot be made`);
  return i;
};
const set = (kind, n, col, f) => (rows) => {
  const i = need(nth(rows, kind, n), `${kind} row ${n}`);
  const out = rows.map((r) => [...r]);
  out[i][col] = typeof f === 'function' ? f(out[i][col]) : f;
  return out;
};
const drop = (kind, n) => (rows) => rows.filter((_, i) => i !== need(nth(rows, kind, n), `${kind} row ${n}`));
const swap = (kind, a, b) => (rows) => {
  const [i, j] = [nth(rows, kind, a), nth(rows, kind, b)].map((x, k) => need(x, `${kind} row ${k ? b : a}`));
  const out = [...rows];
  [out[i], out[j]] = [out[j], out[i]];
  return out;
};
const plus = (k) => (v) => `${Number(v) + k}`;
/** The `hint <member> <value>` row of `member`, its value changed by `f`. */
const hint = (member, f) => (rows) => {
  const at = rows.findIndex((r) => r[0] === 'hint' && r[1] === member);
  const i = need(at < 0 ? undefined : at, `hint ${member} row`);
  return rows.map((r, k) => (k === i ? [r[0], r[1], f(r[2])] : r));
};
const bit = (m) => (v) => `${Number(v) ^ m}`;
const flip = (v) => `${v.slice(0, -1)}${v.endsWith('0') ? '1' : '0'}`;

// ---- the arms: [what changed, change, the FAIL line that must name it] ----
// the clap-probe `params` dump has no kind column: rows are `id name min max default flags`
const paramsRows = (text) => text.trim().split('\n').map((l) => l.split('\t'));
const paramArms = [
  ['a parameter renamed', (r) => r.map((x, i) => (i === 0 ? [x[0], 'Renamed', ...x.slice(2)] : x)), /FAIL clap-params /],
  ['a maximum moved', (r) => r.map((x, i) => (i === 1 ? [...x.slice(0, 3), plus(1)(x[3]), ...x.slice(4)] : x)), /FAIL clap-params /],
  ['a default moved', (r) => r.map((x, i) => (i === 2 ? [...x.slice(0, 4), plus(1)(x[4]), x[5]] : x)), /FAIL clap-params /],
  ['a stepped flag lost', (r) => r.map((x, i) => (i === 0 ? [...x.slice(0, 5), bit(1)(x[5])] : x)), /FAIL clap-params /],
  ['the bypass flag lost', (r) => r.map((x, i, all) => (i === all.length - 1 ? [...x.slice(0, 5), bit(16)(x[5])] : x)), /FAIL clap-params /],
  ['a parameter lost', (r) => r.filter((_, i) => i !== 1), /FAIL clap-params: \d+ parameters/],
  ['two parameters swapped', (r) => { const o = [...r]; [o[0], o[1]] = [o[1], o[0]]; return o; }, /FAIL clap-params /],
  ['the bypass at the id after the last parameter (the rule before omx-contract 3.9.0)', (r) => r.map((x, i, all) => (i === all.length - 1 ? [`${all.length - 1}`, ...x.slice(1)] : x)), /FAIL clap-params Bypass/],
];

const panelArms = (face) => [
  ['a section label changed', set('section', 1, 3, 'Other'), new RegExp(`FAIL panel ${face} sections are the declaration's`)],
  ['a section key changed', set('section', 0, 1, 'other'), new RegExp(`FAIL panel ${face} section keys and label keys`)],
  ['a control lost', drop('control', 2), new RegExp(`FAIL panel ${face} controls are the declaration's`)],
  ['a control moved to another section', set('control', 0, 2, '1'), new RegExp(`FAIL panel ${face} controls are the declaration's`)],
  ['a control relabelled', set('control', 0, 7, 'Other'), new RegExp(`FAIL panel ${face} controls are the declaration's`)],
  ['a widget changed', set('control', 0, 3, (v) => (v === '0' ? '1' : '0')), new RegExp(`FAIL panel ${face} control widgets`)],
  ['a role lost', drop('role', 0), new RegExp(`FAIL panel ${face} roles`)],
  ['a role pointing at another parameter', set('role', 0, 3, plus(1)), new RegExp(`FAIL panel ${face} roles`)],
  ['the family changed', set('family', 0, 1, (v) => (v === '0' ? '1' : '0')), new RegExp(`FAIL panel ${face} family`)],
  ['a placement default changed', set('default', 0, 3, plus(1)), new RegExp(`FAIL panel ${face} placement defaults`)],
  ['the fingerprint changed', set('fingerprint', 0, 1, (v) => `${v.slice(0, -1)}${v.endsWith('0') ? '1' : '0'}`), new RegExp(`FAIL panel ${face} the fingerprint`)],
];
const displayArms = (face) => [
  ['a display lost', drop('display', 0), new RegExp(`FAIL panel ${face} displays`)],
  ['a display moved to another section', set('display', 0, 2, '1'), new RegExp(`FAIL panel ${face} displays`)],
  ['a display source changed', set('display', 0, 3, (v) => (v === '0' ? '1' : '0')), new RegExp(`FAIL panel ${face} displays`)],
  ['a floor in dB given to the transfer curve', set('display', 0, 8, '-60'), new RegExp(`FAIL panel ${face} displays`)],
  ['a floor in dB given to the gain-reduction bar', set('display', 1, 8, '-60'), new RegExp(`FAIL panel ${face} displays`)],
  ['a floor parameter given to the gain-reduction bar', set('display', 1, 7, '0'), new RegExp(`FAIL panel ${face} displays`)],
];

const extArms = [
  ['a parameter served under another id', set('param', 0, 1, plus(1000)), /FAIL clap-ext param-contract: id, symbol, unit, scale and step/],
  ['a unit changed', set('param', 0, 3, plus(1)), /FAIL clap-ext param-contract: id, symbol, unit, scale and step/],
  ['a scale changed', set('param', 0, 4, (v) => (v === '0' ? '1' : '0')), /FAIL clap-ext param-contract: id, symbol, unit, scale and step/],
  ['a step changed', set('param', 0, 5, (v) => (v === '0' ? '1' : '0')), /FAIL clap-ext param-contract: id, symbol, unit, scale and step/],
  ['a parameter lost', drop('param', 1), /FAIL clap-ext param-contract: id, symbol, unit, scale and step/],
  ['the contract version changed', set('contract', 0, 1, plus(1)), /FAIL clap-ext param-contract: version/],
  ['the fingerprint changed', set('contract', 0, 2, flip), /FAIL clap-ext param-contract: the fingerprint/],
  ['a remote-control page lost', drop('page', 0), /FAIL clap-ext remote-controls/],
  ['a remote-control page renamed', set('page', 0, 4, 'Other'), /FAIL clap-ext remote-controls/],
  ['a remote-control slot changed', set('page', 0, 5, '4294967295,'), /FAIL clap-ext remote-controls/],
  ['a default read after placement changed', set('placed', 0, 3, plus(1)), /FAIL clap-ext track-info placed/],
  ['a placement default ignored when the host moved', set('moved', 5, 3, plus(1)), /FAIL clap-ext track-info moved/],
  ['a value the host set not kept', set('kept', 0, 3, plus(1)), /FAIL clap-ext track-info kept/],
  ['a stated hint served with another value', hint('threads_spawned', plus(1)), /FAIL clap-ext resource-hints: each stated hint/],
  ['a stated hint\'s bit cleared', set('hints', 0, 2, (v) => (parseInt(v, 16) & ~0x8).toString(16) /* bit 3, threads-spawned */), /FAIL clap-ext resource-hints: declared/],
  ['an unstated hint\'s bit set', set('hints', 0, 2, (v) => (parseInt(v, 16) | 0x1).toString(16) /* bit 0, footprint-at-rest, which no plugin states */), /FAIL clap-ext resource-hints: declared/],
  ['get answers false', set('hints', 0, 1, '0'), /FAIL clap-ext resource-hints: get answers/],
  ['the resource-hints extension absent', (rows) => rows.filter((r) => r[0] !== 'hint').map((r) => (r[0] === 'hints' ? ['hints', 'absent'] : r)), /FAIL clap-ext resource-hints: the face serves/],
];

/** A face whose travel moved, and whose fingerprint was recomputed over what it serves: self-consistent, so only
 * the declaration's own fingerprint can tell it is wrong. */
const movedAndSealed = (dir) => (rows) => {
  const moved = set('param', 0, 7, plus(1))(rows);
  const want = contractOf(loadDecl(dir), scaleOf);
  const unitOf = Object.fromEntries(want.map((r) => [r.unitEnum, r.unit]));
  const served = moved.filter((r) => r[0] === 'param').map(([, id, symbol, unitEnum, scale, step, min, max, def, flags]) => ({
    id: Number(id), symbol, unitEnum: Number(unitEnum), unit: unitOf[Number(unitEnum)], scale: SCALES[Number(scale)], step: Number(step), min: Number(min), max: Number(max), def: Number(def), flags: parseInt(flags, 16) & 0x1001b,
  }));
  return set('contract', 0, 2, contractFingerprint(served))(moved);
};

/* The port rows by LV2 index: omx-reverb's first control and its in_l, wherever the port rule puts them. */
const reverbPorts = lv2Ports(loadDecl(join(ROOT, 'plugins', 'omx-reverb')));
const FIRST_CONTROL = reverbPorts.find((p) => p.param).index, IN_L = reverbPorts.find((p) => p.symbol === 'in_l').index;
const groupArms = [
  ['a parameter out of its group', set('port', FIRST_CONTROL, 2, '-'), /FAIL port-groups /],
  ['a group relabelled', set('port', FIRST_CONTROL, 3, 'Other'), /FAIL port-groups /],
  ['a parameter in another section\'s group', set('port', FIRST_CONTROL, 2, (v) => (v === 'reverb' ? 'plate' : 'reverb')), /FAIL port-groups /],
  ['a group renamed (lv2:name)', set('port', FIRST_CONTROL, 4, 'Other'), /FAIL port-groups /],
  ['a group\'s display priority moved', set('port', FIRST_CONTROL, 5, plus(1)), /FAIL port-groups /],
  ['a port lost', drop('port', FIRST_CONTROL + 1), /FAIL port-groups /],
  ['an audio port put in a group', set('port', IN_L, 2, 'reverb'), /FAIL port-groups in_l/],
];

// ---- the runner ----
let bad = 0, n = 0;
const report = (ok, what, detail = '') => {
  n++;
  if (!ok) bad++;
  console.log(`${ok ? 'PASS' : 'FAIL'} probe-checks ${what}${ok ? '' : `: ${detail}`}`);
};
const toRows = (text) => text.split('\n').filter(Boolean).map((l) => l.split('\t'));
const toText = (rows) => `${rows.map((r) => r.join('\t')).join('\n')}\n`;

/** The checker on `text`: `{ status, out }`. */
function check(script, args, face, text, file) {
  const path = join(TMP, file);
  writeFileSync(path, text);
  const r = run('node', [join(ROOT, 'tools', script), ...args, path]);
  return { status: r.status, out: `${r.stdout}${r.stderr}` };
}

function arms(name, script, argsOf, text, list, mutate = (change, t) => toText(change(toRows(t)))) {
  const base = check(script, argsOf, name, text, `${name}.txt`);
  report(base.status === 0, `${name}: the checker passes on what the built face serves`, base.out.split('\n').filter((l) => l.startsWith('FAIL')).join(' | '));
  list.forEach(([what, change, want], k) => {
    let broken;
    try {
      broken = mutate(change, text);
    } catch (e) {
      report(false, `${name}: ${what} is red`, e.message);
      return;
    }
    const r = check(script, argsOf, name, broken, `${name}-${k}.txt`);
    const fails = r.out.split('\n').filter((l) => l.startsWith('FAIL'));
    report(r.status === 1 && fails.some((l) => want.test(l)), `${name}: ${what} is red`, `exit ${r.status}, ${fails.length ? fails.join(' | ').slice(0, 300) : 'no FAIL line'}; wanted ${want}`);
  });
}

const reverb = dumpsOf('omx-reverb');
const gate = dumpsOf('omx-keyed-gate');
const reverbDir = reverb.dir, gateDir = gate.dir;

arms('clap-params', 'clap-params-check.mjs', [reverbDir], reverb.dumps.params, paramArms, (change, t) => toText(change(paramsRows(t))));
for (const face of ['clap', 'lv2']) {
  const text = reverb.dumps[face === 'clap' ? 'panelClap' : 'panelLv2'];
  arms(`panel-${face}`, 'panel-check.mjs', [reverbDir, face], text, panelArms(face));
  arms(`panel-${face}-display`, 'panel-check.mjs', [gateDir, face], gate.dumps[face === 'clap' ? 'panelClap' : 'panelLv2'], displayArms(face));
}
arms('clap-ext', 'clap-ext-check.mjs', [reverbDir], reverb.dumps.ext, [...extArms, ['a travel moved with its fingerprint recomputed', movedAndSealed(reverbDir), /FAIL clap-ext param-contract: the fingerprint is the declaration's/]]);
arms('port-groups', 'port-groups-check.mjs', [reverbDir], reverb.dumps.groups, groupArms);

console.log(bad ? `probe-checks: ${bad} of ${n} FAILED` : `probe-checks: all ${n} pass`);
process.exit(bad ? 1 : 0);
