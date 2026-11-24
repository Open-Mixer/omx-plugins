// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * param-units-check.mjs — the units tools/param-contract.mjs writes are the ones omx-contract declares
 * (OMX_PARAM_CONTRACT_UNITS, rendered to omxcontract/omx_param_units.h): each unit's token is the LV2 unit its face writes
 * (plugin-hostd's LV2 table read backwards) and names an OMX_CLAP_UNIT_<TOKEN>; the units the declarations shipped with
 * before the table was read keep their token and value; and the vendor block is the one openmixer's CONTRACT_UNITS ends
 * with (dbfs, dbu, ratio, lufs, dbPerOct, dbPerS, dbTp and lu from 0x100, append-only: omx-contract 3.21.0, clap#163). `make units`, part of `make test`.
 *
 * The declaration and the header are omx-contract's and cannot drift there; this holds what this tree reads of them:
 * the declared codes equal the header's defines, every unit a face writes has its code, and the scale words are PARAM_SCALES.
 *
 *   node tools/test/param-units-check.mjs               the check
 *   node tools/test/param-units-check.mjs --self-test   the check, then the drift check over copies with one unit
 *                                                       changed in the declaration, one in the header, one added to
 *                                                       the declaration and one dropped from the header: each red; and
 *                                                       the same over a parameter-contract header with its own code;
 *                                                       and a copy of omx-clap's scales header with a code moved, and an
 *                                                       own scale enumerator planted: each red
 *
 * The scale codes are omx-clap's (sorting row V21b): its sheet OMX_CLAP_SCALE_CODES (domain omx-clap-scale), rendered to
 * <omx-clap-host/omx_param_scales.h>. The unit codes stay omx-contract's (%4 ruling 2026-11-20: V23d is not in this
 * chain): no file here reads omx-clap's omx-param-contract.
 */
import { execFileSync } from 'node:child_process';
import { existsSync, readdirSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { contractUnits, declaredUnits, headerUnits, PARAM_CONTRACT_H, scalesOf } from '../param-contract.mjs';
import { installedDataDir, locateContract } from '../omx-contract.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const HEADER = 'include/omx_param_contract.h';
const headerFile = process.env.OMX_PARAM_CONTRACT_H ?? PARAM_CONTRACT_H;
const VENDOR = ['dbfs', 'dbu', 'ratio', 'lufs', 'dbPerOct', 'dbPerS', 'dbtp', 'lu'];
/** The table tools/param-contract.mjs held before it read plugin-hostd's (FreeMixer/omx-plugins#210): unchanged. */
const SHIPPED = {
  '': ['none', 0], s: ['s', 1], ms: ['ms', 2], min: ['min', 3], dB: ['db', 13], '%': ['pc', 14], x: ['coef', 15],
  Hz: ['hz', 16], oct: ['oct', 20], cents: ['cent', 21], st: ['semitone12TET', 22], dBFS: ['dbfs', 0x100], dBu: ['dbu', 0x101],
  LUFS: ['lufs', 0x103],
};

/** The scale codes are omx-clap's too (sorting row V21b): its sheet OMX_CLAP_SCALE_CODES (domain omx-clap-scale),
 * rendered to <omx-clap-host/omx_param_scales.h> as OMX_CLAP_SCALE_<WORD> defines; golden as the plugins shipped them. */
const SCALES_HEADER = 'omx-clap-host/omx_param_scales.h';
const GOLDEN_SCALES = [['linear', 0], ['log', 1], ['stepped', 2]];
/** The scale codes a rendered scales header defines, `[[word, code]]` in code order. */
export const scaleCodesOf = (text) => [...text.matchAll(/^#define OMX_CLAP_SCALE_([A-Z0-9_]+) (\d+)u\b/gm)].map((m) => [m[1].toLowerCase(), Number(m[2])]).sort((a, b) => a[1] - b[1]);
/** The scale codes a header of this tree spells itself: none, they are omx-clap's rendered header's alone. */
export const ownScaleCodes = (text) => [...text.matchAll(/^\s*#\s*define\s+(OMX_CLAP_SCALE_\w+)|^\s*(OMX_CLAP_SCALE_\w+)\s*=|^\s*(OMX_CLAP_SCALE_\w+)\s*,/gm)].map((m) => m[1] ?? m[2] ?? m[3]);

/** An installed header of omx-clap's, where pkg-config says its headers are, else the system's. */
function clapHeader(name) {
  let dirs = [];
  try {
    dirs = execFileSync(process.env.PKG_CONFIG || 'pkg-config', ['--cflags-only-I', 'omx-clap'], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'] }).split(/\s+/).filter((f) => f.startsWith('-I')).map((f) => f.slice(2));
  } catch { /* not installed through pkg-config: the system include directory below */ }
  return [...dirs, '/usr/include', '/usr/local/include'].map((d) => join(d, name)).find((f) => existsSync(f));
}

/** The refusals of a declaration `units` ({ token: code }) and the text of a rendered header that drift apart. */
export function drift(units, headerText) {
  const errors = [];
  const declared = new Map(Object.entries(units).map(([t, v]) => [t.toUpperCase(), v]));
  const header = headerUnits(headerText);
  for (const [name, v] of declared) if (header.get(name) !== v) errors.push(`unit ${name}: the declaration has ${v}, the header ${header.get(name) ?? 'none'}`);
  for (const [name, v] of header) if (!declared.has(name)) errors.push(`unit ${name}: the header has ${v}, the declaration none`);
  return errors;
}

/** The unit codes a header of this tree defines itself: none, they are omx-contract's rendered header's alone. */
export function ownCodes(text) {
  return [...text.matchAll(/^\s*#\s*define\s+(OMX_CLAP_UNIT_\w+)|^\s*(OMX_CLAP_UNIT_\w+)\s*=/gm)].map((m) => m[1] ?? m[2]);
}

const contract = locateContract(root);
if (!contract.dir) {
  console.error(`param-units: ${contract.why}`);
  process.exit(1);
}
/** The header omx-contract renders beside its data: <prefix>/share/omx-contract/.. and <prefix>/include/omxcontract/. */
const renderedHeader = [contract.dir, installedDataDir()]
  .filter(Boolean)
  .flatMap((d) => [join(d, '..', '..', 'include', 'omxcontract', 'omx_param_units.h'), join(d, '..', '..', '..', 'include', 'omxcontract', 'omx_param_units.h')])
  .find((p) => existsSync(p));
const errors = [];
const units = declaredUnits(contract.dir);
if (!renderedHeader) errors.push(`omxcontract/omx_param_units.h: not found beside ${contract.dir}; install omx-contract-devel >= 3.6.0`);
const headerText = renderedHeader ? readFileSync(renderedHeader, 'utf8') : '';
errors.push(...(renderedHeader ? drift(units, headerText) : []));
const text = readFileSync(headerFile, 'utf8');
if (!/#include <omxcontract\/omx_param_units\.h>/.test(text)) errors.push(`${HEADER}: does not include <omxcontract/omx_param_units.h>`);
for (const name of ownCodes(text)) errors.push(`${HEADER}: defines ${name} itself; the codes are omx-contract's rendered header's`);
const enumOf = headerUnits(headerText);
const UNITS = contractUnits();
if (enumOf.size === 0) errors.push('omx_param_units.h: no OMX_CLAP_UNIT_ define');
// the scale words: the declaration schema offers a travel the contract's PARAM_SCALES but the one a `kind` already says
let scales = [];
{
  scales = scalesOf(contract.dir);
  // a scale's code is omx-clap's (V21b): its rendered header's OMX_CLAP_SCALE_<WORD> = golden, in PARAM_SCALES' order, and
  // this tree's header spells none and includes the owner's
  const scalesFile = clapHeader(SCALES_HEADER);
  const codes = scalesFile ? scaleCodesOf(readFileSync(scalesFile, 'utf8')) : [];
  if (!scalesFile) errors.push(`${SCALES_HEADER}: not found; install omx-clap-devel >= 0.34.1`);
  else if (JSON.stringify(codes) !== JSON.stringify(GOLDEN_SCALES)) errors.push(`${SCALES_HEADER} numbers the scales ${JSON.stringify(codes)}; golden ${JSON.stringify(GOLDEN_SCALES)}`);
  if (JSON.stringify(codes.map(([w]) => w)) !== JSON.stringify(scales)) errors.push(`${SCALES_HEADER} words the scales ${JSON.stringify(codes.map(([w]) => w))}; PARAM_SCALES is ${JSON.stringify(scales)}`);
  if (!text.includes(`#include <${SCALES_HEADER}>`)) errors.push(`${HEADER}: does not include <${SCALES_HEADER}>`);
  for (const name of ownScaleCodes(text)) errors.push(`${HEADER}: spells ${name} itself; the scale codes are omx-clap's rendered header's`);
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
// the token a unit is served by is the sheet's own, case and all: a host names the unit by the sheet's token when it computes
// the contract's fingerprint (omx-clap's pc_unit_name), so a token of our own case is an R4 refusal (dbperoct, #213)
{
  const sheet = declaredUnits();
  for (const [declared, [token, code]] of Object.entries(UNITS)) {
    if (token !== 'none' && sheet[token] !== code) errors.push(`unit '${declared}' is served as ${token}, which the sheet does not spell (it has ${Object.keys(sheet).find((t) => t.toUpperCase() === token.toUpperCase()) ?? 'no such token'})`);
  }
}
const vendor = [...enumOf].filter(([, v]) => v >= 0x100).sort((a, b) => a[1] - b[1]);
const want = VENDOR.map((t, i) => [t.toUpperCase(), 0x100 + i]);
if (JSON.stringify(vendor) !== JSON.stringify(want)) {
  errors.push(`vendor block is ${vendor.map(([n, v]) => `${n}=0x${v.toString(16)}`).join(', ')}; expected ${want.map(([n, v]) => `${n}=0x${v.toString(16)}`).join(', ')}`);
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
// the unit code sheet is omx-contract's alone (%4 ruling 2026-11-20: V23d is out of this chain): no file of this tree reads
// omx-clap's copy, the domain omx-param-contract
{
  const self = fileURLToPath(import.meta.url);
  const walk = (d) => readdirSync(d, { withFileTypes: true }).flatMap((e) => (e.isDirectory() ? walk(join(d, e.name)) : [join(d, e.name)]));
  const named = [...['tools', 'include', 'lib'].flatMap((d) => walk(join(root, d))), join(root, 'Makefile')]
    .filter((f) => f !== self && /\.(mjs|js|h|c|sh|json)$|Makefile$/.test(f))
    .filter((f) => /omx-param-contract|omx-clap-host\/omx_param_units\.h/.test(readFileSync(f, 'utf8')))
    .map((f) => f.slice(root.length + 1));
  if (named.length) errors.push(`${named.join(', ')}: read omx-clap's omx-param-contract; the unit codes are omx-contract's sheet`);
}
if (errors.length) {
  for (const e of errors) console.error(`param-units: ${e}`);
  process.exit(1);
}
console.log(`param-units: ${Object.keys(UNITS).length} declared units and the vendor block (${VENDOR.join(', ')}) match omx-contract's header (${Object.keys(units).length} codes); the scale words are ${scales.join(', ')}`);

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
  {
    const scalesFile = clapHeader(SCALES_HEADER);
    const head = scalesFile ? readFileSync(scalesFile, 'utf8') : '';
    const moved = head.replace('#define OMX_CLAP_SCALE_LOG 1u', '#define OMX_CLAP_SCALE_LOG 3u');
    const got = scaleCodesOf(moved);
    if (moved !== head && JSON.stringify(got) !== JSON.stringify(GOLDEN_SCALES)) console.log(`SABOTAGE scale-code-moved: red (${JSON.stringify(got)})`);
    else { console.error(`SABOTAGE scale-code-moved: ${moved === head ? 'the sabotage did not land' : 'STAYED GREEN'}`); fail++; }
    const spelled = ownScaleCodes(text + 'enum omx_clap_param_scale {\n  OMX_CLAP_SCALE_LINEAR = 0,\n};\n');
    if (spelled.length === 1) console.log(`SABOTAGE own-scale-enumerator: red (${spelled[0]})`);
    else { console.error(`SABOTAGE own-scale-enumerator: read ${JSON.stringify(spelled)}`); fail++; }
  }
  if (ownCodes(text).length) {
    console.error('self-test: the unsabotaged header defines a code');
    fail++;
  }
  if (drift(units, head).length) {
    console.error('self-test: the unsabotaged copies are not green');
    fail++;
  }
  if (fail) process.exit(1);
  console.log('param-units self-test: every sabotage red');
}
