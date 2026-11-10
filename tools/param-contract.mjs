// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * param-contract.mjs — org.openmixer.param-contract/1 of a plugin, from its declaration: each
 * parameter's symbol, unit, scale and step, the host's bypass last, and the canonical serialisation
 * whose SHA-256 is the fingerprint. The extension is omx-clap-core's (<omx-clap-host/omx_clap_ext.h>); tools/gen.mjs
 * writes the rows a CLAP face serves it from (include/omx_param_contract.h); the unit codes are omx-contract's
 * (OMX_PARAM_CONTRACT_UNITS). tools/clap-ext-check.mjs reads what a built face serves back.
 *
 * A plugin's contract version is the 1-based position of its fingerprint in tools/param-contract-versions.json: 1 its
 * first publication, and a later change of a symbol, unit, scale, travel or stepping changes the fingerprint and appends
 * it there, the next version. A fingerprint the file does not list is refused, so no change ships under an old version.
 */
import { createHash } from 'node:crypto';
import { readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { lv2UnitNames, lv2UnitRef } from './format-tables.mjs';
import { items, locateContract } from './omx-contract.mjs';
import { travelUnit } from './descriptor.mjs';

export const CONTRACT_ID = 'org.openmixer.param-contract/1';
/** The contract version of plugin `stem` whose contract fingerprint is `fingerprint` (tools/param-contract-versions.json). */
export function contractVersionOf(stem, fingerprint) {
  const known = JSON.parse(readFileSync(join(ROOT, 'tools', 'param-contract-versions.json'), 'utf8')).plugins[stem] ?? [];
  const i = known.indexOf(fingerprint);
  if (i < 0) throw new Error(`plugins/${stem}: param-contract fingerprint ${fingerprint} is not in tools/param-contract-versions.json: append it, the next version (${known.length + 1})`);
  return i + 1;
}

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..');

/** The declaration of the contract's units is omx-contract's: the sheet OMX_PARAM_CONTRACT_UNITS (data/param-contract.json,
 * omx-contract >= 3.6.0), each token's code, rendered to omxcontract/omx_param_units.h as OMX_CLAP_UNIT_* defines. */
export const PARAM_UNITS_ITEM = 'OMX_PARAM_CONTRACT_UNITS';
/** The header the extension's C structures are in; it includes omx-contract's unit codes. */
export const PARAM_CONTRACT_H = join(ROOT, 'include', 'omx_param_contract.h');

/** The declared units, `{ <token>: <code> }` in code order, of the resolved contract at `dir` (default: the one this tree is
 * built against, tools/omx-contract.mjs locateContract). */
export function declaredUnits(dir) {
  if (dir === undefined) {
    const found = locateContract(ROOT);
    if (!found.dir) throw new Error(`${PARAM_UNITS_ITEM}: ${found.why}`);
    dir = found.dir;
  }
  const item = items(dir)[PARAM_UNITS_ITEM];
  if (item?.kind !== 'sheet' || typeof item.value !== 'object') throw new Error(`${dir}: the resolved contract holds no ${PARAM_UNITS_ITEM} sheet (omx-contract >= 3.6.0)`);
  return Object.fromEntries(Object.entries(item.value).sort((a, b) => a[1] - b[1]));
}

/** The ids a face gives its own controls are this tree's: the sheet OMX_PLUGIN_FACE_IDS of contract/plugin-face.json
 * (omx-contract's from 3.9.0 to 3.12.x, ours since 0.19.3), rendered to include/omx_plugin_face_ids.h and installed into
 * the contract directory. The CLAP bypass id and the LV2 enabled and latency
 * indices are the same in every plugin and never move when a control is appended. */
export const FACE_IDS_ITEM = 'OMX_PLUGIN_FACE_IDS';
export const FACE_IDS_FILE = 'plugin-face.json';

/** `{ bypassClapId, enabledLv2Index, latencyLv2Index }` of this tree's contract/plugin-face.json, or of the resolved
 * contract at `dir` (a union holding the installed copy). */
let faceIdsCache;
export function faceIds(dir) {
  if (dir === undefined && faceIdsCache) return faceIdsCache;
  const at = dir ?? join(ROOT, 'contract', FACE_IDS_FILE);
  const v = dir === undefined ? JSON.parse(readFileSync(at, 'utf8'))[FACE_IDS_ITEM]?.value : items(at)[FACE_IDS_ITEM]?.value;
  for (const k of ['bypassClapId', 'enabledLv2Index', 'latencyLv2Index'])
    if (!Number.isInteger(v?.[k])) throw new Error(`${at}: holds no ${FACE_IDS_ITEM}.${k}`);
  const ids = { bypassClapId: v.bypassClapId, enabledLv2Index: v.enabledLv2Index, latencyLv2Index: v.latencyLv2Index };
  if (dir === undefined) faceIdsCache = ids;
  return ids;
}

/** The units by name without underscores (SEMITONE12TET), as `Map<name, code>`: the declaration's, or with `text` the
 * OMX_CLAP_UNIT_* defines of a rendered omx_param_units.h. */
let declared;
export function headerUnits(text) {
  if (text === undefined) return (declared ??= new Map(Object.entries(declaredUnits()).map(([t, v]) => [t.toUpperCase(), v])));
  return new Map([...text.matchAll(/^#define OMX_CLAP_UNIT_(\w+) (\d+)u\b/gm)].map((m) => [m[1].replaceAll('_', ''), Number(m[2])]));
}

/** A declared unit -> the contract's unit, `[token, enum value]`: the token is the LV2 unit the face writes for it
 * (plugin-hostd's LV2 table read backwards, tools/format-tables.mjs: units:db -> db; a plugin-defined unit's name in
 * the vendor block's lower case: urn:openmixer:unit#dBFS -> dbfs), the value the declaration's. A unit it has no
 * value for (dB/oct), and "", is NONE, its text left to value_to_text. */
export function contractUnit(unit) {
  const ref = lv2UnitRef(unit);
  const token = ref && (ref.startsWith('units:') ? ref.slice('units:'.length) : ref.replace(/^<|>$/g, '').split('#').pop().toLowerCase());
  const value = token && headerUnits().get(token.toUpperCase());
  return value === undefined ? ['none', 0] : [token, value];
}

/** The token of a unit the contract serves (one with a header value, or "" for none), else undefined. */
export const servedToken = (unit) => (unit === '' ? 'none' : contractUnit(unit)[1] ? contractUnit(unit)[0] : undefined);

/** Every unit a face can write, with its contract unit: `{ <unit>: [token, value] }`, "" among them. */
export const contractUnits = () => Object.fromEntries(['', ...Object.keys(lv2UnitNames())].map((u) => [u, contractUnit(u)]));

/** The scale words in the order of their codes (OMX_CLAP_SCALE_*), PARAM_SCALES of the resolved contract at `dir`
 * (tools/omx-contract.mjs locateContract): the list is omx-contract's, never retyped here. */
export function scalesOf(dir) {
  const scales = items(dir).PARAM_SCALES?.value;
  if (!Array.isArray(scales) || scales.length === 0) throw new Error(`${dir}: the resolved contract has no PARAM_SCALES list`);
  return scales;
}
const STEPPED = 0x1, BYPASS = 0x10;

/** Each parameter's contract, in id order, the host's bypass last: `{ id, symbol, unit, unitEnum, scale,
 * min, max, def, step, flags }`, `id` the CLAP id the face serves it under (a declared parameter its index, the bypass
 * OMX_PLUGIN_FACE_IDS.bypassClapId), min, max and def as the face's float travel reports them. */
export function contractOf(d, scaleOf) {
  const where = `plugins/${d.stem}`;
  const rows = d.params.map((p, i) => {
    // the unit the LV2 face writes for it (gen.mjs faceUnit): a ratio's travel with no unit of its own is a ratio
    const [unit, unitEnum] = contractUnit(travelUnit(p.unit, d.descriptor?.controls[i]?.travel?.unit));
    // an integer travel is stepped, or log when its law is (an integer frequency, plugin-hostd#142); its step is 1 either way
    const choice = p.values !== undefined || p.points !== undefined;
    const scale = !choice && scaleOf(p) === 'log' ? 'log' : p.kind ? 'stepped' : 'linear';
    if (scale === 'log' && !(p.min > 0 && p.max > 0)) throw new Error(`${where}: '${p.symbol}' is logarithmic over ${p.min}..${p.max}; a log scale needs a positive travel`);
    return { id: i, symbol: p.symbol, unit, unitEnum, scale, min: Math.fround(p.min), max: Math.fround(p.max), def: Math.fround(p.def), step: p.kind ? 1 : 0, flags: p.kind ? STEPPED : 0 };
  });
  if (rows.some((r) => r.symbol === 'bypass')) throw new Error(`${where}: 'bypass' is the host's bypass parameter's symbol; a declared parameter cannot take it`);
  rows.push({ id: faceIds().bypassClapId, symbol: 'bypass', unit: 'none', unitEnum: 0, scale: 'stepped', min: 0, max: 1, def: 0, step: 1, flags: STEPPED | BYPASS });
  return rows;
}

/** The shortest decimal that round-trips to the same binary64, -0 written 0. */
const shortest = (n) => (Object.is(n, -0) ? '0' : String(n));

/** The canonical serialisation: the id line, then `id symbol unit scale min max default step flags`
 * per parameter in CLAP id order, TAB-separated, LF after every line. `id` is the row's CLAP id, the one get_info
 * answers and a host hashes, never its position. */
export const contractCanonical = (rows) => {
  const bad = rows.find((r) => !Number.isInteger(r.id) || r.id < 0 || r.id > 0xffffffff);
  if (bad) throw new Error(`param-contract: the row '${bad.symbol}' has no CLAP id`);
  const sorted = [...rows].sort((a, b) => a.id - b.id);
  return `${[CONTRACT_ID, ...sorted.map((r) => [r.id, r.symbol, r.unit, r.scale, shortest(r.min), shortest(r.max), shortest(r.def), shortest(r.step), r.flags.toString(16)].join('\t'))].join('\n')}\n`;
};

export const contractFingerprint = (rows) => createHash('sha256').update(contractCanonical(rows)).digest('hex');
