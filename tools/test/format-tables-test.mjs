#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * format-tables-test.mjs — the generators read plugin-hostd's installed format tables backwards
 * (tools/format-tables.mjs; FreeMixer/omx-plugins#210 P3), held by value:
 *
 *   1. the LV2 units a face writes are the ones tools/gen.mjs held before it read the table (LV2_UNITS), each
 *      unit the table maps read from it, the declarations' own (dBu, dB/oct, ratio) beside it;
 *   2. every described control whose meaning the LV2 table maps (param:gain, ...) carries it as its port's
 *      lv2:designation in the generated TTL, and no other control does: the 169 of this release, by term;
 *   3. the three ratio controls state urn:openmixer:unit#ratio, and the TTL defines it;
 *   4. each generated header's OMX_<K>_PARAM_MODULES is its descriptor's groups as module paths, "" without one.
 *   5. the installed contract's directories are asked of pkg-config once per pkg-config environment, not once per
 *      table row (omx-plugins#39: tableRows asked it on every call, 47 of gen.mjs --check's 60 s), and a changed
 *      PKG_CONFIG_PATH asks again and gets the new answer.
 *   7. the declarations' own LV2 units are F1's (plugin-db's lv2 domain LV2_OPENMIXER_UNIT_IDS, exact x-uri; M3) and
 *      every unit a face publishes is a standard unit by its symbol (PARAM_UNITS x-symbol; F4), golden, with an arm;
 *   6. the CLAP features a descriptor writes are omx-clap's terms (CLAP_PLUGIN_FEATURES_TERMS of its domain clap,
 *      <contractdir>/clap/clap.json; sorting row V24/27): every open feature the shipped descriptors name maps to its
 *      term id and back = golden (omx-contract 3.20.2's terms), and over a copy of the contract directory whose clap
 *      domain lost `delay`, delay is no term (the read is the domain's).
 *
 *   8. the tables are x-kind map items whose unit and meaning rows carry no values: their pairs are the owners'
 *      (sorting rows V19, M5; omx-plugins#182): the CLAP unit row's are omx-clap's OMX_PARAM_CONTRACT_UNIT_IDS, whole,
 *      and a declared unit reaches the same LV2 unit by its symbol or its PARAM_UNITS id (% and percent: units:pc).
 *
 * Its arms, on a copy of the contract directory whose plugin-hostd/lv2 domain (PLUGIN_HOSTD_LV2_MAP) or plugin-db's lv2
 * domain is edited: a meaning's row removed loses its designations, a unit's row removed loses its unit, two pairs
 * mapping one value are refused; the table as the old sheet, an owner's mapping item absent, a domain that is not the
 * owner's and a row its owner holds no pair for are each refused by name; a code renamed in omx-clap's sheet is the
 * code the CLAP row then reads. `make format-tables`, part of `make test`.
 */
import { spawnSync } from 'node:child_process';
import { cpSync, existsSync, mkdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';
import { loadDecl, pluginDirs, ROOT } from '../gen.mjs';
import { descriptorFile } from '../descriptor.mjs';
import { lv2Designation, lv2UnitNames, tablesDir } from '../format-tables.mjs';
import { installedContractDir } from '../omx-contract.mjs';

let n = 0, bad = 0;
function check(what, got, want) {
  n++;
  const ok = JSON.stringify(got) === JSON.stringify(want);
  if (!ok) bad++;
  console.log(`${ok ? 'PASS' : 'FAIL'} format-tables ${what}${ok ? '' : `: got ${JSON.stringify(got)}, want ${JSON.stringify(want)}`}`);
}

// 1. the units, against the map tools/gen.mjs held before (LV2_UNITS, the release before #210)
const SHIPPED_LV2_UNITS = {
  Hz: 'hz', dB: 'db', ms: 'ms', s: 's', min: 'min', '%': 'pc', coef: 'coef', oct: 'oct', cents: 'cent', st: 'semitone12TET',
  dBFS: 'dBFS', dBu: 'dBu', LUFS: 'LUFS', 'dB/oct': 'dBoct',
};
const names = lv2UnitNames();
check('the LV2 units a face writes are the ones it wrote before the table was read', Object.fromEntries(Object.keys(SHIPPED_LV2_UNITS).map((u) => [u, names[u]])), SHIPPED_LV2_UNITS);

// the ports of a generated TTL: { <symbol>: { designation, unit } }
const portsOf = (ttl) => Object.fromEntries(ttl.split(/\n    \[\n/).slice(1).map((block) => [
  /lv2:symbol "([^"]+)"/.exec(block)?.[1], { designation: /lv2:designation (\S+) ;?/.exec(block)?.[1], unit: /units:unit (\S+) ;?/.exec(block)?.[1] },
]));

const described = pluginDirs().filter((dir) => existsSync(join(dir, descriptorFile(dir.split('/').pop()))));
const ttlOf = (dir, stem) => readFileSync(join(dir, 'generated', `${stem}.lv2`, `${stem}.ttl`), 'utf8');

/** Every control's designation as the TTLs state it and as the descriptors' meanings want it. */
function designations(designationOf) {
  const got = [], want = [];
  for (const dir of pluginDirs()) {
    const stem = dir.split('/').pop();
    const desc = existsSync(join(dir, descriptorFile(stem))) ? JSON.parse(readFileSync(join(dir, descriptorFile(stem)), 'utf8')) : undefined;
    const ports = portsOf(ttlOf(dir, stem));
    for (const q of loadDecl(dir).params) {
      const c = desc?.controls.find((x) => x.symbol === q.symbol);
      if (ports[q.symbol]?.designation) got.push(`${stem}.${q.symbol} ${ports[q.symbol].designation}`);
      const d = designationOf(c?.meaning);
      if (d) want.push(`${stem}.${q.symbol} ${d}`);
    }
  }
  return { got, want };
}

// 2. the designations
const { got, want } = designations(lv2Designation);
check('every described control whose meaning the LV2 table maps is designated by it, and no other', got, want);
const byTerm = {};
for (const g of got) byTerm[g.split(' ')[1]] = (byTerm[g.split(' ')[1]] ?? 0) + 1;
check('the designations of this release, by term', Object.fromEntries(Object.entries(byTerm).sort((x, y) => y[1] - x[1] || x[0].localeCompare(y[0]))), { 'param:gain': 93, 'param:frequency': 63, 'param:wetDryRatio': 12, 'param:release': 6, 'param:attack': 5, 'param:ratio': 5, 'param:threshold': 5, 'param:cutoffFrequency': 1, 'param:delay': 1, 'param:hold': 1, 'param:resonance': 1 });

// 3. the ratio unit
const ratios = [];
for (const dir of described) {
  const stem = dir.split('/').pop(), ttl = ttlOf(dir, stem), ports = portsOf(ttl);
  for (const [sym, p] of Object.entries(ports)) if (p.unit === '<urn:openmixer:unit#ratio>') ratios.push(`${stem}.${sym}${ttl.includes('<urn:openmixer:unit#ratio>\n    a units:Unit') ? '' : ' (undefined)'}`);
}
check('the ratio controls state urn:openmixer:unit#ratio, defined in their TTL', ratios, [
  'omx-comp.ratio', 'omx-deesser.deessRatio', 'omx-ducker.ratio', ...[1, 2, 3, 4, 5, 6].map((n) => `omx-dyneq.ratio${n}`), 'omx-keyed-gate.ratio', ...[1, 2, 3, 4, 5].map((n) => `omx-multiband.ratio${n}`), 'omx-upward.ratio',
]);

// 4. the CLAP modules
for (const dir of pluginDirs()) {
  const d = loadDecl(dir), header = join(dir, 'generated', `omx_${d.kernel}_params.h`);
  if (!existsSync(header)) continue;
  const body = readFileSync(header, 'utf8').match(new RegExp(`OMX_${d.kernel.toUpperCase()}_PARAM_MODULES\\[[^\\]]*\\] = \\{\\n([\\s\\S]*?)\\n\\};`))?.[1];
  const modules = body === undefined ? undefined : [...body.matchAll(/"((?:[^"\\]|\\.)*)"/g)].map((m) => m[1]);
  check(`${d.stem}: the CLAP modules are its descriptor's groups`, modules, d.params.map((_, i) => (d.descriptor?.controls[i]?.group ?? []).join('/')));
}

// the arms, on a copy of the contract directory with one domain edited (the tables are plugin-hostd's domains
// plugin-hostd/clap and plugin-hostd/lv2, <contractdir>/plugin-hostd/<format>/<format>.json, an x-kind map item each;
// the unit and meaning pairs are plugin-db's domain lv2 and omx-clap's omx-param-contract)
const scratch = join(ROOT, 'build', 'tmp', 'format-tables');
rmSync(scratch, { recursive: true, force: true });
mkdirSync(scratch, { recursive: true });
/** A copy of the contract directory whose domain `domain` document is passed to `edit` (its parsed JSON). */
const broken = (name, domain, edit) => {
  const dir = join(scratch, name);
  cpSync(tablesDir(), dir, { recursive: true });
  const file = join(dir, domain, `${domain.split('/').pop()}.json`), t = JSON.parse(readFileSync(file, 'utf8'));
  edit(t, t.components.schemas);
  writeFileSync(file, JSON.stringify(t));
  return dir;
};
const LV2_MAP = 'plugin-hostd/lv2';
const dropRow = (key) => (t, s) => { s.PLUGIN_HOSTD_LV2_MAP.oneOf = s.PLUGIN_HOSTD_LV2_MAP.oneOf.filter((r) => r.const !== key); };
// each arm runs in a process of its own: the tables are read once per process
const arm = (dir, code) => spawnSync(process.execPath, ['--input-type=module', '-e', code], { cwd: ROOT, env: { ...process.env, OMX_INSTALLED_CONTRACTDIR: dir }, encoding: 'utf8' });
const FT = JSON.stringify(join(ROOT, 'tools', 'format-tables.mjs'));
const refused = (r, re) => r.status !== 0 && re.test(r.stderr);
let r = arm(broken('no-gain', LV2_MAP, dropRow('param:gain')), `import { lv2Designation } from ${FT}; console.log(String(lv2Designation('gain')));`);
check('arm: the param:gain row removed, gain is designated by nothing', r.stdout.trim(), 'undefined');
r = arm(broken('no-db', LV2_MAP, dropRow('units:db')), `import { lv2UnitRef } from ${FT}; console.log(String(lv2UnitRef('dB')));`);
check('arm: the units:db row removed, dB has no LV2 unit', r.stdout.trim(), 'undefined');
r = arm(broken('twice', 'lv2', (t, s) => { s.LV2_OPENMIXER_UNIT_IDS.oneOf.push({ const: 'dB', title: 'db', 'x-uri': 'urn:openmixer:unit#dB' }); }), `import { lv2UnitRef } from ${FT}; lv2UnitRef('dB');`);
check('arm: two pairs mapping dB are refused', refused(r, /maps controls\[\]\.travel\.unit "dB" from units:db and urn:openmixer:unit#dB/), true);
r = arm(broken('sheet', LV2_MAP, (t, s) => { s.PLUGIN_HOSTD_LV2_MAP['x-kind'] = 'sheet'; }), `import { lv2UnitRef } from ${FT}; lv2UnitRef('dB');`);
check('arm: the table as a sheet, not x-kind map, is refused by name', refused(r, /holds no PLUGIN_HOSTD_LV2_MAP map of format lv2/), true);
r = arm(broken('no-unit-ids', 'lv2', (t, s) => { delete s.LV2_UNIT_IDS; }), `import { lv2UnitRef } from ${FT}; lv2UnitRef('dB');`);
check('arm: plugin-db\'s LV2_UNIT_IDS absent, refused by name', refused(r, /holds no LV2_UNIT_IDS \(plugin-db-contract >= 0\.5\.1\)/), true);
r = arm(broken('not-owner', 'lv2', (t) => { t.info['x-owner'] = 'plugin-hostd'; }), `import { lv2Designation } from ${FT}; lv2Designation('gain');`);
check('arm: a lv2 domain that is not plugin-db\'s is refused by name', refused(r, /the domain lv2 is "plugin-hostd"'s, not plugin-db's/), true);
r = arm(broken('no-gain-pair', 'lv2', (t, s) => { s.LV2_DESIGNATION_MEANINGS.oneOf = s.LV2_DESIGNATION_MEANINGS.oneOf.filter((w) => w.const !== 'gain'); }), `import { lv2Designation } from ${FT}; lv2Designation('gain');`);
check('arm: a meaning row whose owner holds no pair (gain) is refused by name', refused(r, /lv2 row param:gain \(designation-meaning, LV2_PARAMETERS_TERMS#gain\) has no pair in its owner's mapping/), true);
r = arm(broken('clap-code', 'omx-param-contract', (t, s) => { const v = s.OMX_PARAM_CONTRACT_UNIT_IDS['x-value']; v.percent_code = v.pc; delete v.pc; }), `import { lookupValues } from ${FT}; console.log(JSON.stringify(lookupValues('clap', 'controls[].travel.unit')));`);
check('arm: a code renamed in omx-clap\'s sheet (pc -> percent_code) is the code the CLAP unit row reads', r.status === 0 ? [JSON.parse(r.stdout).pc, JSON.parse(r.stdout).percent_code] : r.stderr.trim().split('\n').pop(), [undefined, 'percent']);
rmSync(scratch, { recursive: true, force: true });

// 8. the owners' pairs, as read: the CLAP unit row's are omx-clap's sheet whole; % and percent write the same LV2 unit
{
  const r8 = spawnSync(process.execPath, ['--input-type=module', '-e', `
    import { lookupValues, lv2UnitRef } from ${FT};
    import { domainDocument } from ${JSON.stringify(join(ROOT, 'tools', 'omx-contract.mjs'))};
    console.log(JSON.stringify({ clap: lookupValues('clap', 'controls[].travel.unit'), sheet: domainDocument('omx-param-contract', '').schemas.OMX_PARAM_CONTRACT_UNIT_IDS['x-value'],
      refs: ['%', 'percent', 'dB/oct', 'dB-oct', 'dB', 'coef'].map((u) => lv2UnitRef(u)) }));`], { cwd: ROOT, encoding: 'utf8' });
  const g = r8.status === 0 ? JSON.parse(r8.stdout) : { error: r8.stderr.trim().split('\n').find((l) => /Error/.test(l)) };
  check('M5: the CLAP unit row\'s pairs are omx-clap\'s OMX_PARAM_CONTRACT_UNIT_IDS, whole', g.clap ?? g.error, g.sheet ?? g.error);
  check('M5: a declared unit writes one LV2 unit by its symbol or its PARAM_UNITS id', g.refs ?? g.error, ['units:pc', 'units:pc', '<urn:openmixer:unit#dBoct>', '<urn:openmixer:unit#dBoct>', 'units:db', 'units:coef']);
}

// 5. pkg-config once per environment: a fake pkg-config that logs each call and answers from PKG_CONFIG_PATH
{
  const dir = join(ROOT, 'build', 'format-tables-pkgconfig');
  rmSync(dir, { recursive: true, force: true });
  mkdirSync(dir, { recursive: true });
  const log = join(dir, 'calls');
  writeFileSync(join(dir, 'pkg-config'), `#!/bin/sh\necho "$*" >> '${log}'\necho "/fake/$PKG_CONFIG_PATH/\${1#--variable=}"\n`, { mode: 0o755 });
  const OC = JSON.stringify(join(ROOT, 'tools', 'omx-contract.mjs')), FTS = JSON.stringify(join(ROOT, 'tools', 'format-tables.mjs'));
  const r = spawnSync(process.execPath, ['--input-type=module', '-e', `
    import { installedContractDir, installedDataDir } from ${OC};
    import { tablesDir } from ${FTS};
    const seen = [];
    for (let i = 0; i < 50; i++) seen.push(tablesDir(), installedDataDir(), installedContractDir());
    process.env.PKG_CONFIG_PATH = 'b';
    for (let i = 0; i < 50; i++) seen.push(tablesDir(), installedDataDir());
    console.log(JSON.stringify([...new Set(seen)]));`], {
    encoding: 'utf8', env: { ...process.env, PKG_CONFIG: join(dir, 'pkg-config'), PKG_CONFIG_PATH: 'a', OMX_INSTALLED_CONTRACTDIR: '' },
  });
  const calls = existsSync(log) ? readFileSync(log, 'utf8').trim().split('\n') : [];
  check('pkg-config: each directory asked once per environment (2 environments, 2 variables: 4 calls)', calls.length, 4);
  check('pkg-config: a changed PKG_CONFIG_PATH gets the new answers', r.status === 0 ? JSON.parse(r.stdout) : r.stderr.trim(),
    ['/fake/a/contractdir', '/fake/a/datadir', '/fake/b/contractdir', '/fake/b/datadir']);
  rmSync(dir, { recursive: true, force: true });
}

// 6. the CLAP features: omx-clap's clap domain, golden from omx-contract 3.20.2's CLAP_PLUGIN_FEATURES_TERMS
{
  const GOLDEN_FEATURES = {
    chorus: 'CLAP_PLUGIN_FEATURE_CHORUS', compressor: 'CLAP_PLUGIN_FEATURE_COMPRESSOR', 'de-esser': 'CLAP_PLUGIN_FEATURE_DEESSER',
    delay: 'CLAP_PLUGIN_FEATURE_DELAY', distortion: 'CLAP_PLUGIN_FEATURE_DISTORTION', equalizer: 'CLAP_PLUGIN_FEATURE_EQUALIZER',
    filter: 'CLAP_PLUGIN_FEATURE_FILTER', flanger: 'CLAP_PLUGIN_FEATURE_FLANGER', gate: 'CLAP_PLUGIN_FEATURE_GATE',
    limiter: 'CLAP_PLUGIN_FEATURE_LIMITER', mixing: 'CLAP_PLUGIN_FEATURE_MIXING', phaser: 'CLAP_PLUGIN_FEATURE_PHASER',
    'pitch-shifter': 'CLAP_PLUGIN_FEATURE_PITCH_SHIFTER', reverb: 'CLAP_PLUGIN_FEATURE_REVERB', stereo: 'CLAP_PLUGIN_FEATURE_STEREO',
    'transient-shaper': 'CLAP_PLUGIN_FEATURE_TRANSIENT_SHAPER', tremolo: 'CLAP_PLUGIN_FEATURE_TREMOLO', utility: 'CLAP_PLUGIN_FEATURE_UTILITY',
  };
  const FTS = JSON.stringify(join(ROOT, 'tools', 'format-tables.mjs'));
  const call = (env) => spawnSync(process.execPath, ['--input-type=module', '-e', `
    import { clapFeatureField, clapFeatureValue } from ${FTS};
    const v = ${JSON.stringify(Object.keys(GOLDEN_FEATURES))};
    console.log(JSON.stringify(Object.fromEntries(v.map((f) => [f, clapFeatureField(f)]).map(([f, id]) => [f, [id, clapFeatureValue(id)]]))));`], { cwd: ROOT, encoding: 'utf8', env: { ...process.env, ...env } });
  const golden = Object.fromEntries(Object.entries(GOLDEN_FEATURES).map(([f, id]) => [f, [id, f]]));
  let r6 = call({});
  check('the CLAP features the descriptors name map to their term ids and back (omx-clap clap domain = golden)', r6.status === 0 ? JSON.parse(r6.stdout) : r6.stderr.trim().split('\n').pop(), golden);
  // (c) a copy of the contract directory whose clap domain has no delay: delay is no term there
  const cdir = installedContractDir();
  const tmp = join(ROOT, 'build', 'tmp', 'format-tables-clap');
  rmSync(tmp, { recursive: true, force: true });
  let planted = false;
  try {
    cpSync(cdir, tmp, { recursive: true });
    const file = join(tmp, 'clap', 'clap.json'), doc = JSON.parse(readFileSync(file, 'utf8'));
    const item = doc.components.schemas.CLAP_PLUGIN_FEATURES_TERMS;
    item.oneOf = item.oneOf.filter((w) => w.const !== 'delay');
    writeFileSync(file, JSON.stringify(doc));
    planted = true;
  } catch (e) {
    console.log(`format-tables arm: the clap domain could not be copied (${e.message})`);
  }
  r6 = planted ? call({ OMX_INSTALLED_CONTRACTDIR: tmp }) : { status: 1, stdout: '', stderr: '' };
  check('arm: the clap domain without delay, delay is no term (the read is omx-clap\'s)', r6.status === 0 ? JSON.parse(r6.stdout).delay : 'not planted', ['delay', 'delay']);
  rmSync(tmp, { recursive: true, force: true });
}

// 7. M3 + F4: the declarations' own LV2 units are the standard units F1 spells (plugin-db's lv2 domain,
// LV2_OPENMIXER_UNIT_IDS: the PARAM_UNITS word and the exact x-uri), and every unit a face publishes is a standard unit
// by its symbol (omx-contract's param domain, PARAM_UNITS x-symbol): golden; the lv2 domain without dBu loses its URI
{
  const FTS = JSON.stringify(join(ROOT, 'tools', 'format-tables.mjs'));
  const PUBLISHED = { '': 'none', Hz: 'Hz', dB: 'dB', ms: 'ms', s: 's', min: 'min', '%': 'percent', coef: 'coef', oct: 'oct', cents: 'cents', st: 'st', dBFS: 'dBFS', dBu: 'dBu', LUFS: 'LUFS', 'dB/oct': 'dB-oct', ratio: 'ratio' };
  const OWN = { dBFS: 'urn:openmixer:unit#dBFS', LUFS: 'urn:openmixer:unit#LUFS', ratio: 'urn:openmixer:unit#ratio', dBu: 'urn:openmixer:unit#dBu', 'dB/oct': 'urn:openmixer:unit#dBoct' };
  const call = (env) => spawnSync(process.execPath, ['--input-type=module', '-e', `
    import { ownLv2Units, standardUnitOf, lv2UnitRef } from ${FTS};
    console.log(JSON.stringify({ own: ownLv2Units(), std: Object.fromEntries(${JSON.stringify(Object.keys(PUBLISHED))}.map((u) => [u, standardUnitOf(u)])), dbu: String(lv2UnitRef('dBu')) }));`], { cwd: ROOT, encoding: 'utf8', env: { ...process.env, ...env } });
  let r7 = call({});
  const got = r7.status === 0 ? JSON.parse(r7.stdout) : { error: r7.stderr.trim().split('\n').find((l) => /Error/.test(l)) };
  check('M3: the own LV2 units are F1\'s words and exact URIs (plugin-db lv2 domain) = golden', got.own ?? got.error, OWN);
  check('F4: every published unit is a standard unit by its symbol (PARAM_UNITS x-symbol) = golden', got.std ?? got.error, PUBLISHED);
  const tmp = join(ROOT, 'build', 'tmp', 'format-tables-lv2');
  rmSync(tmp, { recursive: true, force: true });
  let planted = false;
  try {
    cpSync(installedContractDir(), tmp, { recursive: true });
    const file = join(tmp, 'lv2', 'lv2.json'), doc = JSON.parse(readFileSync(file, 'utf8'));
    const item = doc.components.schemas.LV2_OPENMIXER_UNIT_IDS;
    item.oneOf = item.oneOf.filter((w) => w.const !== 'dBu');
    writeFileSync(file, JSON.stringify(doc));
    planted = true;
  } catch (e) {
    console.log(`format-tables arm: the lv2 domain could not be copied (${e.message})`);
  }
  r7 = planted ? call({ OMX_INSTALLED_CONTRACTDIR: tmp }) : { status: 1, stdout: '' };
  check('arm: the lv2 domain without dBu, dBu has no LV2 unit (the read is plugin-db\'s)', r7.status === 0 ? JSON.parse(r7.stdout).dbu : 'not planted', 'undefined');
  rmSync(tmp, { recursive: true, force: true });
}

console.log(bad ? `format-tables: ${bad} of ${n} failed` : `format-tables: ${n}/${n} pass`);
process.exit(bad ? 1 : 0);
