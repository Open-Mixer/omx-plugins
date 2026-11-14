// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * format-tables.mjs — plugin-hostd's format tables, read backwards (FreeMixer/omx-plugins#210, decision B: one
 * vocabulary, two directions). plugin-hostd installs one mapping table per format into omx-contract's contract
 * directory, <contractdir>/3/plugin-hostd/formats/{clap,lv2}.json (PLUGIN_HOSTD_CLAP_MAP, PLUGIN_HOSTD_LV2_MAP, item
 * kind formatMap; plugin-hostd docs/formats.md): how each field a scan reads becomes a descriptor word. A generator
 * writes a face from a descriptor by reading the same rows the other way: the row whose `lookup` maps a source value
 * to the word's value names what the face writes. Only `identity` and `lookup` are read here; no transform is code
 * of ours (what is structure, a module path or a scale point, stays in the generator that writes it).
 *
 * $PLUGIN_HOSTD_TABLES, a directory holding clap.json and lv2.json (a plugin-hostd checkout's data/formats), stands in
 * for the installed tables.
 */
import { existsSync, readFileSync } from 'node:fs';
import { join } from 'node:path';
import { installedContractDir, locateContract } from './omx-contract.mjs';

/** The schema major the tables are installed under, and the item each format's file holds. */
export const TABLES_MAJOR = '3';
const ITEM = { clap: 'PLUGIN_HOSTD_CLAP_MAP', lv2: 'PLUGIN_HOSTD_LV2_MAP' };

/** The directory the tables are read from, or undefined. */
export function tablesDir() {
  if (process.env.PLUGIN_HOSTD_TABLES) return process.env.PLUGIN_HOSTD_TABLES;
  const cdir = installedContractDir();
  return cdir && join(cdir, TABLES_MAJOR, 'plugin-hostd', 'formats');
}

const cache = new Map();
/** The rows of `format`'s table (`clap`, `lv2`): `{ <source field>: { word? | extension? | dropped?, term? | from? } }`. */
export function tableRows(format) {
  if (!ITEM[format]) throw new Error(`format-tables: no table for format '${format}'`);
  const dir = tablesDir();
  const file = dir && join(dir, `${format}.json`);
  if (!cache.has(file)) {
    if (!file || !existsSync(file)) {
      throw new Error(`plugin-hostd's ${format} table is not installed (${file ?? 'no contract directory: pkg-config --variable=contractdir omx-contract finds none'}): this tree needs plugin-hostd-devel >= 0.7.0 (packaging/omx-plugins.spec), or PLUGIN_HOSTD_TABLES naming a plugin-hostd checkout's data/formats`);
    }
    const item = JSON.parse(readFileSync(file, 'utf8'))[ITEM[format]];
    if (item?.kind !== 'formatMap' || item.format !== format || typeof item.rows !== 'object') throw new Error(`${file}: holds no ${ITEM[format]} formatMap of format ${format}`);
    cache.set(file, item.rows);
  }
  return cache.get(file);
}

/**
 * What a face of `format` writes for descriptor word `word` (`controls[].travel.unit`, `controls[].meaning`) taking
 * `value`: the source value of the one `lookup` row that maps to it (`units:db` for dB, `param:gain` for gain,
 * `urn:openmixer:unit#dBFS` for dBFS), or undefined when no row does. Throws when two rows would.
 */
export function outboundValue(format, word, value) {
  const keys = [];
  for (const row of Object.values(tableRows(format))) {
    const w = row.word;
    if (!w || w.word !== word || w.transform !== 'lookup') continue;
    for (const [k, v] of Object.entries(w.values ?? {})) if (v === value) keys.push(k);
  }
  if (new Set(keys).size > 1) throw new Error(`plugin-hostd's ${format} table maps ${word} ${JSON.stringify(value)} from ${[...new Set(keys)].join(' and ')}: a face writes one`);
  return keys[0];
}

/** Every value `format`'s table maps to word `word` through a `lookup` row, `{ <source value>: <value> }`. */
export function lookupValues(format, word) {
  const out = {};
  for (const row of Object.values(tableRows(format))) if (row.word?.word === word && row.word.transform === 'lookup') Object.assign(out, row.word.values);
  return out;
}

/** The descriptor words `format`'s table produces at all (a row of class word naming it). */
export function tableWords(format) {
  return new Set(Object.values(tableRows(format)).filter((r) => r.word).map((r) => r.word.word));
}

// ---- the CLAP features a face writes, as a scan reads them -----------------------------------------------------------

/** omx-contract's CLAP feature terms (CLAP_PLUGIN_FEATURES_TERMS, data/formats/clap/plugin-features.json), read from
 * the resolved render (locateContract, OMX_CONTRACT_DIR included): `{ <value>: <term id> }`, delay ->
 * CLAP_PLUGIN_FEATURE_DELAY. A field dump writes a feature the headers define by its macro name (plugin-hostd
 * docs/formats.md, The field dump), so the bag carries that name and a face writes its value. */
let featureTerms;
export function clapFeatureTerms() {
  if (featureTerms) return featureTerms;
  const where = locateContract();
  if (!where.dir) throw new Error(`omx-contract's CLAP feature terms: ${where.why}`);
  const item = JSON.parse(readFileSync(join(where.dir, 'omx-contract.json'), 'utf8')).items?.CLAP_PLUGIN_FEATURES_TERMS;
  if (item?.kind !== 'terms') throw new Error(`${where.at}: holds no CLAP_PLUGIN_FEATURES_TERMS (omx-contract >= 3.0.0 installs it)`);
  featureTerms = Object.fromEntries(Object.values(item.value ?? item.terms).map((t) => [t.value, t.id]));
  return featureTerms;
}

/** A CLAP feature as the bag carries it (its term id, or the text of an open feature the headers do not define), and
 * back. */
export const clapFeatureField = (value) => clapFeatureTerms()[value] ?? value;
export const clapFeatureValue = (field) => Object.entries(clapFeatureTerms()).find(([, id]) => id === field)?.[0] ?? field;

// ---- what the LV2 generator writes (tools/gen.mjs), read backwards -------------------------------------------------

const UNIT_WORD = 'controls[].travel.unit', MEANING_WORD = 'controls[].meaning';

/** The units a declaration writes that the LV2 table (what a scan reads back) has no row for, each a plugin-defined
 * unit (tools/gen.mjs LV2_CUSTOM_UNITS defines them in the TTL): a slope's dB/oct and dBu, which no descriptor word
 * reads, and the ratio (n:1), a descriptor unit from omx-contract 3.0.0 that plugin-hostd 0.7.0's table does not map
 * (a scan carries it in the bag; the round trip names it lost). */
export const OWN_LV2_UNITS = Object.freeze({ dBu: 'urn:openmixer:unit#dBu', 'dB/oct': 'urn:openmixer:unit#dBoct', ratio: 'urn:openmixer:unit#ratio' });

/** The `units:unit` value of a declared unit, as a TTL writes it: the LV2 table read backwards (units:db for dB,
 * <urn:openmixer:unit#dBFS> for dBFS), else the declaration's own unit (OWN_LV2_UNITS); none for "". */
export function lv2UnitRef(unit) {
  const key = unit ? outboundValue('lv2', UNIT_WORD, unit) ?? OWN_LV2_UNITS[unit] : undefined;
  if (!key) return undefined;
  return key.startsWith('units:') ? key : `<${key}>`;
}

/** The LV2 name of each unit a face can write, `{ <unit>: <name> }` (`units:db` -> db, `urn:openmixer:unit#dBFS` -> dBFS). */
export function lv2UnitNames() {
  const pairs = [...Object.entries(lookupValues('lv2', UNIT_WORD)).map(([k, v]) => [v, k]), ...Object.entries(OWN_LV2_UNITS)];
  return Object.fromEntries(pairs.map(([unit, key]) => [unit, key.split(/[:#]/).pop()]));
}

/** The LV2 designation of a meaning, the LV2 table read backwards (gain -> param:gain); a meaning the table maps no
 * param: term for (knee, makeup, ...) falls back to its own urn (<urn:openmixer:meaning#knee>, FreeMixer/omx-plugins#12),
 * generated into every face from the declarations. None for "" or unknown (a plugin stating no meaning). */
export const lv2Designation = (meaning) => (meaning && meaning !== 'unknown' ? outboundValue('lv2', MEANING_WORD, meaning) ?? `<urn:openmixer:meaning#${meaning}>` : undefined);
