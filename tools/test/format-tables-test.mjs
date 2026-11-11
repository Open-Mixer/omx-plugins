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
 *
 * Its arms, on a copy of the LV2 table (PLUGIN_HOSTD_TABLES): a meaning's row removed loses its designations, a unit's
 * row removed loses its unit, two rows mapping one value are refused. `make format-tables`, part of `make test`.
 */
import { spawnSync } from 'node:child_process';
import { cpSync, existsSync, mkdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';
import { loadDecl, pluginDirs, ROOT } from '../gen.mjs';
import { descriptorFile } from '../descriptor.mjs';
import { lv2Designation, lv2UnitNames, tablesDir } from '../format-tables.mjs';

let n = 0, bad = 0;
function check(what, got, want) {
  n++;
  const ok = JSON.stringify(got) === JSON.stringify(want);
  if (!ok) bad++;
  console.log(`${ok ? 'PASS' : 'FAIL'} format-tables ${what}${ok ? '' : `: got ${JSON.stringify(got)}, want ${JSON.stringify(want)}`}`);
}

// 1. the units, against the map tools/gen.mjs held before (LV2_UNITS, the release before #210)
const SHIPPED_LV2_UNITS = {
  Hz: 'hz', dB: 'db', ms: 'ms', s: 's', min: 'min', '%': 'pc', x: 'coef', oct: 'oct', cents: 'cent', st: 'semitone12TET',
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
// held by value: a meaning the LV2 table maps no param: term for (knee, makeup) still carries one, urn:openmixer:meaning#<meaning>
const compPorts = portsOf(ttlOf(join(ROOT, 'plugins', 'omx-comp'), 'omx-comp'));
check("omx-comp.kneeDb's designation (knee, no LV2 param: term)", compPorts.kneeDb?.designation, '<urn:openmixer:meaning#knee>');
check("omx-comp.makeupDb's designation (makeup, no LV2 param: term)", compPorts.makeupDb?.designation, '<urn:openmixer:meaning#makeup>');
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

// the arms, on a copy of the LV2 table
const scratch = join(ROOT, 'build', 'tmp', 'format-tables');
rmSync(scratch, { recursive: true, force: true });
mkdirSync(scratch, { recursive: true });
cpSync(tablesDir(), scratch, { recursive: true });
const broken = (name, edit) => {
  const dir = join(scratch, name);
  cpSync(tablesDir(), dir, { recursive: true });
  const file = join(dir, 'lv2.json'), t = JSON.parse(readFileSync(file, 'utf8'));
  edit(t.PLUGIN_HOSTD_LV2_MAP.rows);
  writeFileSync(file, JSON.stringify(t));
  return dir;
};
// each arm runs in a process of its own: the tables are read once per process
const arm = (dir, code) => spawnSync(process.execPath, ['--input-type=module', '-e', code], { cwd: ROOT, env: { ...process.env, PLUGIN_HOSTD_TABLES: dir }, encoding: 'utf8' });
const FT = JSON.stringify(join(ROOT, 'tools', 'format-tables.mjs'));
let r = arm(broken('no-gain', (rows) => { delete rows['param:gain']; }), `import { lv2Designation } from ${FT}; console.log(String(lv2Designation('gain')));`);
check('arm: the param:gain row removed, gain is designated by nothing', r.stdout.trim(), 'undefined');
r = arm(broken('no-db', (rows) => { delete rows['units:db']; }), `import { lv2UnitRef } from ${FT}; console.log(String(lv2UnitRef('dB')));`);
check('arm: the units:db row removed, dB has no LV2 unit', r.stdout.trim(), 'undefined');
r = arm(broken('twice', (rows) => { rows['units:unit'].word.values['urn:openmixer:unit#dB'] = 'dB'; }), `import { lv2UnitRef } from ${FT}; lv2UnitRef('dB');`);
check('arm: two rows mapping dB are refused', r.status !== 0 && /maps controls\[\]\.travel\.unit "dB" from units:db and urn:openmixer:unit#dB/.test(r.stderr), true);
rmSync(scratch, { recursive: true, force: true });

console.log(bad ? `format-tables: ${bad} of ${n} failed` : `format-tables: ${n}/${n} pass`);
process.exit(bad ? 1 : 0);
