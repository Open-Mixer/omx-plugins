// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * param-units-check.mjs — the units tools/param-contract.mjs writes are the ones omx-clap declares (the code sheet
 * OMX_PARAM_CONTRACT_UNITS of its domain omx-param-contract, <contractdir>/omx-param-contract/omx-param-contract.json,
 * rendered to <omx-clap-host/omx_param_units.h>; sorting row V23d): each unit's token is the LV2 unit its face writes
 * (plugin-hostd's LV2 table read backwards) and names an OMX_CLAP_UNIT_<TOKEN>; the units the declarations shipped with
 * before the table was read keep their token and value; and the vendor block is the one openmixer's CONTRACT_UNITS ends
 * with (dbfs, dbu, ratio, lufs from 0x100, append-only). `make units`, part of `make test`.
 *
 * The declaration and the header are omx-clap's and cannot drift there; this holds what this tree reads of them:
 * the declared codes equal the header's defines and the golden codes of omx-contract 3.20.2's sheet (append-only),
 * every unit a face writes has its code, and the scale words are PARAM_SCALES.
 *
 *   node tools/test/param-units-check.mjs               the check
 *   node tools/test/param-units-check.mjs --self-test   the check, then the drift check over copies with one unit
 *                                                       changed in the declaration, one in the header, one added to
 *                                                       the declaration and one dropped from the header: each red; and
 *                                                       the same over a parameter-contract header with its own code;
 *                                                       and the whole check over a copy of the contract directory whose
 *                                                       omx-param-contract sheet has no dbu: red (the read is the domain's)
 */
import { execFileSync, spawnSync } from 'node:child_process';
import { cpSync, existsSync, mkdtempSync, readdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { contractUnits, declaredUnits, headerUnits, PARAM_CONTRACT_H, scalesOf } from '../param-contract.mjs';
import { installedContractDir, locateContract } from '../omx-contract.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const HEADER = 'include/omx_param_contract.h';
const headerFile = process.env.OMX_PARAM_CONTRACT_H ?? PARAM_CONTRACT_H;
const VENDOR = ['dbfs', 'dbu', 'ratio', 'lufs'];
/** The table tools/param-contract.mjs held before it read plugin-hostd's (FreeMixer/omx-plugins#210): unchanged. */
const SHIPPED = {
  '': ['none', 0], s: ['s', 1], ms: ['ms', 2], min: ['min', 3], dB: ['db', 13], '%': ['pc', 14], x: ['coef', 15],
  Hz: ['hz', 16], oct: ['oct', 20], cents: ['cent', 21], st: ['semitone12TET', 22], dBFS: ['dbfs', 0x100], dBu: ['dbu', 0x101],
  LUFS: ['lufs', 0x103],
};
/** The code sheet as omx-contract 3.20.2 released it (OMX_PARAM_CONTRACT_UNITS, 29 tokens), frozen: the owner's sheet keeps
 * every one at its code (append-only; omx-clap may append, never move or drop). */
const GOLDEN_CODES = {
  none: 0, s: 1, ms: 2, min: 3, bar: 4, beat: 5, frame: 6, m: 7, cm: 8, mm: 9, km: 10, inch: 11, mile: 12, db: 13, pc: 14,
  coef: 15, hz: 16, khz: 17, mhz: 18, bpm: 19, oct: 20, cent: 21, semitone12TET: 22, degree: 23, midiNote: 24, dbfs: 256,
  dbu: 257, ratio: 258, lufs: 259,
};
/** The domain and the header the codes are read from, omx-clap's (sorting row V23d). */
const DOMAIN = 'omx-param-contract';
const OWNER_HEADER = 'omx-clap-host/omx_param_units.h';

/** The refusals of a declaration `units` ({ token: code }) and the text of a rendered header that drift apart. */
export function drift(units, headerText) {
  const errors = [];
  const declared = new Map(Object.entries(units).map(([t, v]) => [t.toUpperCase(), v]));
  const header = headerUnits(headerText);
  for (const [name, v] of declared) if (header.get(name) !== v) errors.push(`unit ${name}: the declaration has ${v}, the header ${header.get(name) ?? 'none'}`);
  for (const [name, v] of header) if (!declared.has(name)) errors.push(`unit ${name}: the header has ${v}, the declaration none`);
  return errors;
}

/** The unit codes a header of this tree defines itself: none, they are omx-clap's rendered header's alone. */
export function ownCodes(text) {
  return [...text.matchAll(/^\s*#\s*define\s+(OMX_CLAP_UNIT_\w+)|^\s*(OMX_CLAP_UNIT_\w+)\s*=/gm)].map((m) => m[1] ?? m[2]);
}

const contract = locateContract(root);
if (!contract.dir) {
  console.error(`param-units: ${contract.why}`);
  process.exit(1);
}
/** The header omx-clap renders, where pkg-config says its headers are, else the system's. */
function ownerHeader() {
  let dirs = [];
  try {
    dirs = execFileSync(process.env.PKG_CONFIG || 'pkg-config', ['--cflags-only-I', 'omx-clap'], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'] }).split(/\s+/).filter((f) => f.startsWith('-I')).map((f) => f.slice(2));
  } catch { /* not installed through pkg-config: the system include directory below */ }
  return [...dirs, '/usr/include', '/usr/local/include'].map((d) => join(d, OWNER_HEADER)).find((f) => existsSync(f));
}
const renderedHeader = ownerHeader();
const errors = [];
const units = declaredUnits();
if (!renderedHeader) errors.push(`${OWNER_HEADER}: not found; install omx-clap-devel >= 0.34.0`);
const headerText = renderedHeader ? readFileSync(renderedHeader, 'utf8') : '';
errors.push(...(renderedHeader ? drift(units, headerText) : []));
// the call-level golden: every code omx-contract 3.20.2 released is the declaration's, at the same code
for (const [token, code] of Object.entries(GOLDEN_CODES)) if (units[token] !== code) errors.push(`unit ${token}: the declaration has ${units[token] ?? 'none'}, golden ${code}`);
const text = readFileSync(headerFile, 'utf8');
if (!text.includes(`#include <${OWNER_HEADER}>`)) errors.push(`${HEADER}: does not include <${OWNER_HEADER}>`);
for (const name of ownCodes(text)) errors.push(`${HEADER}: defines ${name} itself; the codes are omx-clap's rendered header's`);
const enumOf = headerUnits(headerText);
const UNITS = contractUnits();
if (enumOf.size === 0) errors.push('omx_param_units.h: no OMX_CLAP_UNIT_ define');
// the scale words: the declaration schema offers a travel the contract's PARAM_SCALES but the one a `kind` already says
let scales = [];
{
  scales = scalesOf(contract.dir);
  // a scale's code is its position in PARAM_SCALES: the header's OMX_CLAP_SCALE_<WORD> values say the same
  const codes = [...text.matchAll(/OMX_CLAP_SCALE_(\w+)\s*=\s*(\d+)/g)].map((m) => [m[1].toLowerCase(), Number(m[2])]);
  if (JSON.stringify(codes) !== JSON.stringify(scales.map((w, i) => [w, i]))) errors.push(`${HEADER} numbers the scales ${JSON.stringify(codes)}; PARAM_SCALES is ${JSON.stringify(scales)}`);
  const schema = JSON.parse(readFileSync(join(root, 'schema/plugin.decl.schema.json'), 'utf8'));
  const offered = schema.$defs.param.properties.scale.enum;
  const want = scales.filter((w) => w !== 'stepped');
  if (JSON.stringify(offered) !== JSON.stringify(want)) errors.push(`schema/plugin.decl.schema.json offers scale ${JSON.stringify(offered)}; PARAM_SCALES minus stepped (a \`kind\` says it) is ${JSON.stringify(want)}`);
}
for (const [declared, want] of Object.entries(SHIPPED)) if (JSON.stringify(UNITS[declared]) !== JSON.stringify(want)) errors.push(`unit '${declared}' writes ${JSON.stringify(UNITS[declared])}, it shipped as ${JSON.stringify(want)}`);
for (const [declared, [token, code]] of Object.entries(UNITS)) {
  if (token === 'none') continue;
  const got = enumOf.get(token.toUpperCase());
  if (got !== code) errors.push(`unit '${declared}' writes ${token} = ${code}, the header has ${got === undefined ? 'no OMX_CLAP_UNIT_' + token.toUpperCase() : got}`);
}
const vendor = [...enumOf].filter(([, v]) => v >= 0x100).sort((a, b) => a[1] - b[1]);
const want = VENDOR.map((t, i) => [t.toUpperCase(), 0x100 + i]);
// append-only: the block starts with the four this tree shipped with; the owner appends (omx-clap 0.34.0: dB/oct, dB/s,
// dBTP, LU at 0x104..0x107, V23d's Q15 codes)
if (JSON.stringify(vendor.slice(0, want.length)) !== JSON.stringify(want)) {
  errors.push(`vendor block is ${vendor.map(([n, v]) => `${n}=0x${v.toString(16)}`).join(', ')}; expected it to start ${want.map(([n, v]) => `${n}=0x${v.toString(16)}`).join(', ')}`);
}
// a summary names no sample-rate span: STANDARD_SAMPLE_RATES is omx-contract's, and prose retyped from it goes stale
const SPAN = /\d+(\.\d+)?\s*(to|-|\u2013)\s*\d+(\.\d+)?\s*kHz/;
const decls = readdirSync(join(root, 'plugins'), { withFileTypes: true }).filter((e) => e.isDirectory()).map((e) => join(root, 'plugins', e.name, `${e.name}.decl.json`));
let summaries = 0;
for (const f of decls) {
  let decl;
  try { decl = JSON.parse(readFileSync(f, 'utf8')); } catch { continue; }
  if (typeof decl.summary !== 'string') continue;
  summaries++;
  if (SPAN.test(decl.summary)) errors.push(`${f.slice(root.length + 1)}: the summary states a sample-rate span ("${decl.summary.match(SPAN)[0]}"); the rates are omx-contract's STANDARD_SAMPLE_RATES`);
}
if (summaries === 0) errors.push('no declaration summary was read: the span check looked at nothing');
if (errors.length) {
  for (const e of errors) console.error(`param-units: ${e}`);
  process.exit(1);
}
console.log(`param-units: ${Object.keys(UNITS).length} declared units and the vendor block (${VENDOR.join(', ')}) match omx-clap's header (${Object.keys(units).length} codes); the scale words are ${scales.join(', ')}`);

if (process.argv.includes('--self-test')) {
  const head = headerText;
  const arms = {
    'declaration-changed': [{ ...units, db: 12 }, head],
    'header-changed': [units, head.replace('#define OMX_CLAP_UNIT_DB 13u', '#define OMX_CLAP_UNIT_DB 12u')],
    'declaration-added': [{ ...units, phon: 260 }, head],
    'header-dropped': [units, head.replace(/^#define OMX_CLAP_UNIT_LUFS .*\n/m, '')],
  };
  let fail = 0;
  for (const [arm, [d, h]] of Object.entries(arms)) {
    if (d === units && h === head) {
      console.error(`SABOTAGE ${arm}: the sabotage did not land`);
      fail++;
      continue;
    }
    const out = drift(d, h);
    if (out.length) console.log(`SABOTAGE ${arm}: red (${out[0]})`);
    else {
      console.error(`SABOTAGE ${arm}: STAYED GREEN`);
      fail++;
    }
  }
  for (const [arm, own] of Object.entries({ 'own-define': '#define OMX_CLAP_UNIT_DB 13u\n', 'own-enumerator': 'enum omx_clap_unit {\n  OMX_CLAP_UNIT_DB = 13,\n};\n' })) {
    const out = ownCodes(text + own);
    if (out.length === 1 && out[0] === 'OMX_CLAP_UNIT_DB') console.log(`SABOTAGE ${arm}: red (${out[0]})`);
    else {
      console.error(`SABOTAGE ${arm}: read ${JSON.stringify(out)}`);
      fail++;
    }
  }
  if (ownCodes(text).length) {
    console.error('self-test: the unsabotaged header defines a code');
    fail++;
  }
  if (drift(units, head).length) {
    console.error('self-test: the unsabotaged copies are not green');
    fail++;
  }
  // (c) the read is omx-clap's domain: the whole check over a copy of the contract directory whose sheet lost dbu is red
  {
    const cdir = installedContractDir();
    const tmp = mkdtempSync(join(tmpdir(), 'param-units-'));
    try {
      const copy = join(tmp, 'contract.d');
      cpSync(cdir, copy, { recursive: true });
      const file = join(copy, DOMAIN, `${DOMAIN}.json`);
      const doc = JSON.parse(readFileSync(file, 'utf8'));
      delete doc.components.schemas.OMX_PARAM_CONTRACT_UNITS['x-value'].dbu;
      writeFileSync(file, `${JSON.stringify(doc, null, 2)}\n`);
      const r = spawnSync(process.execPath, [fileURLToPath(import.meta.url)], { encoding: 'utf8', env: { ...process.env, OMX_INSTALLED_CONTRACTDIR: copy } });
      if (r.status !== 0 && /dbu|DBU/.test(r.stderr)) console.log(`SABOTAGE domain-sheet-without-dbu: red (${r.stderr.split('\n').find((l) => /dbu|DBU/.test(l))})`);
      else {
        console.error(`SABOTAGE domain-sheet-without-dbu: STAYED GREEN (exit ${r.status}; the check does not read ${DOMAIN})`);
        fail++;
      }
    } catch (e) {
      console.error(`SABOTAGE domain-sheet-without-dbu: could not be planted: ${e.message}`);
      fail++;
    } finally {
      rmSync(tmp, { recursive: true, force: true });
    }
  }
  if (fail) process.exit(1);
  console.log('param-units self-test: every sabotage red');
}
