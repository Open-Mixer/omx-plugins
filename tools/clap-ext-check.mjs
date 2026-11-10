#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * clap-ext-check.mjs — what a BUILT CLAP face serves beyond its parameters, as tools/clap-ext-probe.c
 * read it, against the plugin's DECLARATION:
 *
 *   - org.openmixer.param-contract/1: each parameter's symbol, unit, scale and step are the
 *     declaration's (tools/param-contract.mjs), the bypass last; version 1; the
 *     fingerprint is the SHA-256 of what get_info and the contract serve, and the declaration's;
 *   - clap.remote-controls: the pages of the page rule over the declaration's panel sections;
 *   - clap.track-info: under each placement, before activation and after it, every parameter reads
 *     the placement's default the contract states for it, else its own default; a parameter the host
 *     has set keeps its value;
 *   - org.openmixer.resource-hints.draft/0: `declared` holds the bits of the hints the declaration's
 *     "resourceHints" states (tools/resource-hints.mjs) and no other, get answers whether there is one,
 *     and each stated member is the declaration's value.
 *
 * Usage: node tools/clap-ext-check.mjs plugins/<stem> <probe output>
 */
import { readFileSync } from 'node:fs';
import { panelDescription } from './descriptor.mjs';
import { loadDecl, scaleOf } from './gen.mjs';
import { locateContract } from './omx-contract.mjs';
import { contractCanonical, contractFingerprint, contractOf, contractVersionOf, scalesOf } from './param-contract.mjs';
import { PLACEMENTS, pagesOf } from './omx-panel.mjs';
import { maskOf } from './resource-hints.mjs';

const [dir, dump] = process.argv.slice(2);
const contract = locateContract();
if (!contract.dir) throw new Error(`clap-ext-check: the scale words are omx-contract's PARAM_SCALES, and ${contract.why}`);
const SCALES = scalesOf(contract.dir);
const d = loadDecl(dir);
const rows = readFileSync(dump, 'utf8').split('\n').filter(Boolean).map((l) => l.split('\t'));
const of = (k) => rows.filter((r) => r[0] === k).map((r) => r.slice(1));
const NONE = 4294967295;

let bad = 0;
const check = (ok, what, detail = '') => {
  if (!ok) bad++;
  console.log(`${ok ? 'PASS' : 'FAIL'} clap-ext ${what}${ok || !detail ? '' : `: ${detail}`}`);
};
const same = (a, b) => JSON.stringify(a) === JSON.stringify(b);

// the parameter contract
const want = contractOf(d, scaleOf);
const served = of('param').map(([id, symbol, unitEnum, scale, step, min, max, def, flags]) => ({
  id: Number(id), symbol, unitEnum: Number(unitEnum), scale: SCALES[Number(scale)] ?? '?', step: Number(step), min: Number(min), max: Number(max), def: Number(def), flags: parseInt(flags, 16) & 0x1001b,
}));
const unitOf = Object.fromEntries(want.map((r) => [r.unitEnum, r.unit]));
const row = (r) => [r.id, r.symbol, r.unitEnum, r.scale, r.step];
check(same(served.map(row), want.map(row)), `param-contract: id, symbol, unit, scale and step of the ${want.length} parameters are the declaration's`, JSON.stringify(served.map(row)));
const [version, fingerprint] = of('contract')[0] ?? [];
const wantVersion = contractVersionOf(d.stem, contractFingerprint(want));
check(Number(version) === wantVersion, `param-contract: version ${wantVersion}`, `version ${version}`);
const servedRows = served.map((r) => ({ ...r, unit: unitOf[r.unitEnum] ?? `?${r.unitEnum}` }));
check(fingerprint === contractFingerprint(servedRows), 'param-contract: the fingerprint is that of what get_info and the contract serve', `${fingerprint}, computed over:\n${contractCanonical(servedRows)}`);
check(fingerprint === contractFingerprint(want), "param-contract: the fingerprint is the declaration's", `${fingerprint}, the declaration's ${contractFingerprint(want)}`);

// the remote controls
const desc = panelDescription(d);
const pages = pagesOf(desc, d).map((g, k) => [k, k, g.sectionName, g.pageName, g.ids.map((i) => (i === null ? NONE : i)).join(','), 0]);
const servedPages = of('page').map(([k, id, section, name, ids, preset]) => [Number(k), Number(id), section, name, ids, Number(preset)]);
check(same(servedPages, pages), `remote-controls: the ${pages.length} page(s) of the declaration's sections`, JSON.stringify(servedPages));

// the track info
const n = d.params.length + 1;
const expect = (placement, id) => {
  if (id >= d.params.length) return 0;
  const r = desc.defaults.find((x) => x.param === id && x.placement === PLACEMENTS[placement]);
  return Math.fround(r ? r.value : d.params[id].def);
};
const maxOf = (id) => (id < d.params.length ? Math.fround(d.params[id].max) : 1);
for (const how of ['placed', 'moved', 'kept']) {
  for (let placement = 0; placement < 4; placement++) {
    const got = of(how).filter((r) => Number(r[0]) === placement).map((r) => Number(r[2]));
    const exp = [...Array(n).keys()].map((id) => (how === 'kept' ? maxOf(id) : expect(placement, id)));
    const named = desc.defaults.filter((x) => x.placement === PLACEMENTS[placement]).map((x) => `${x.symbol} ${x.value}`).join(', ');
    check(same(got, exp), `track-info ${how} ${PLACEMENTS[placement]}: ${how === 'kept' ? 'the values the host set stay' : named || 'every default its own'}`, `got ${JSON.stringify(got)}, want ${JSON.stringify(exp)}`);
  }
}

// the resource hints
const hintsRow = of('hints')[0];
const stated = d.hints;
const mask = maskOf(stated);
const words = stated.map((h) => h.word).join(', ') || 'none';
if (!hintsRow || hintsRow[0] === 'absent') check(false, 'resource-hints: the face serves org.openmixer.resource-hints.draft/0', hintsRow ? 'absent' : 'no hints row');
else {
  const [answer, declared] = hintsRow;
  check(parseInt(declared, 16) === mask, `resource-hints: declared holds the bits of the declaration's hints (${words})`, `0x${declared}, the declaration's 0x${mask.toString(16)}`);
  check(Number(answer) === (mask ? 1 : 0), `resource-hints: get answers ${mask ? 'true' : 'false'}`, `it answers ${answer}`);
  const member = Object.fromEntries(of('hint'));
  const value = (h) => (h.figureUnit === 'flag' ? (h.value ? '1' : '0') : `${h.value}`);
  const got = stated.map((h) => [h.word, member[h.member]]), want = stated.map((h) => [h.word, value(h)]);
  check(same(got, want), "resource-hints: each stated hint is the declaration's value", `${JSON.stringify(got)}, the declaration's ${JSON.stringify(want)}`);
}
process.exit(bad ? 1 : 0);
