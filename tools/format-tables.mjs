// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * format-tables.mjs — plugin-hostd's format tables, read backwards (FreeMixer/omx-plugins#210, decision B: one
 * vocabulary, two directions). plugin-hostd installs one mapping table per format as a domain of the contract
 * directory, plugin-hostd/clap and plugin-hostd/lv2 (named by their items' prefix, PLUGIN_HOSTD_CLAP_MAP and
 * PLUGIN_HOSTD_LV2_MAP; sorting row V19), <contractdir>/plugin-hostd/<format>/<format>.json, each item a sheet whose
 * value is the map ({ format, rows }; plugin-hostd docs/formats.md): how each field a scan reads becomes a descriptor
 * word. A generator
 * writes a face from a descriptor by reading the same rows the other way: the row whose `lookup` maps a source value
 * to the word's value names what the face writes. Only `identity` and `lookup` are read here; no transform is code
 * of ours (what is structure, a module path or a scale point, stays in the generator that writes it).
 *
 * $OMX_INSTALLED_CONTRACTDIR, a contract directory holding the two domains, stands in for the installed one.
 */
import { existsSync, readFileSync } from 'node:fs';
import { join } from 'node:path';
import { domainDocument, installedContractDir } from './omx-contract.mjs';

/** The item each format's domain holds, and the domain (named by the item's prefix). */
const ITEM = { clap: 'PLUGIN_HOSTD_CLAP_MAP', lv2: 'PLUGIN_HOSTD_LV2_MAP' };
export const tableDomain = (format) => `plugin-hostd/${format}`;

/** The contract directory the tables' domains are read from, or undefined. */
export const tablesDir = () => installedContractDir();

const cache = new Map();
/** The rows of `format`'s table (`clap`, `lv2`): `{ <source field>: { word? | extension? | dropped?, term? | from? } }`. */
export function tableRows(format) {
  if (!ITEM[format]) throw new Error(`format-tables: no table for format '${format}'`);
  const dir = tablesDir();
  const file = dir && join(dir, tableDomain(format), `${format}.json`);
  if (!cache.has(file)) {
    if (!file || !existsSync(file)) {
      throw new Error(`plugin-hostd's ${format} table, the domain ${tableDomain(format)}, is not installed (${file ?? 'no contract directory: pkg-config --variable=contractdir omx-contract finds none'}): this tree needs plugin-hostd-contract >= 0.21.0 (packaging/omx-plugins.spec)`);
    }
    const item = JSON.parse(readFileSync(file, 'utf8')).components?.schemas?.[ITEM[format]];
    const map = item?.['x-value'];
    if (item?.['x-kind'] !== 'sheet' || map?.format !== format || typeof map.rows !== 'object') throw new Error(`${file}: holds no ${ITEM[format]} sheet of format ${format}`);
    cache.set(file, map.rows);
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

/** omx-clap's CLAP feature terms (CLAP_PLUGIN_FEATURES_TERMS of its domain clap, <contractdir>/clap/clap.json,
 * omx-clap-contract >= 0.34.1; sorting row V24/27): `{ <value>: <term id> }`, delay -> CLAP_PLUGIN_FEATURE_DELAY,
 * each term's `x-term` value and id (omx-contract 3.21.0's terms kind: `const` and `title` are the term's key). A field dump writes a feature the headers define by its macro name (plugin-hostd
 * docs/formats.md, The field dump), so the bag carries that name and a face writes its value. */
let featureTerms;
export function clapFeatureTerms() {
  if (featureTerms) return featureTerms;
  const { file, schemas } = domainDocument('clap', 'omx-clap-contract >= 0.34.1');
  const words = schemas.CLAP_PLUGIN_FEATURES_TERMS?.oneOf;
  if (!Array.isArray(words) || !words.length) throw new Error(`${file}: holds no CLAP_PLUGIN_FEATURES_TERMS (omx-clap-contract >= 0.34.1 installs it)`);
  featureTerms = Object.fromEntries(words.map((w) => [w['x-term']?.value ?? w.const, w['x-term']?.id]).filter(([, id]) => id));
  return featureTerms;
}

/** A CLAP feature as the bag carries it (its term id, or the text of an open feature the headers do not define), and
 * back. */
export const clapFeatureField = (value) => clapFeatureTerms()[value] ?? value;
export const clapFeatureValue = (field) => Object.entries(clapFeatureTerms()).find(([, id]) => id === field)?.[0] ?? field;

// ---- what the LV2 generator writes (tools/gen.mjs), read backwards -------------------------------------------------

const UNIT_WORD = 'controls[].travel.unit', MEANING_WORD = 'controls[].meaning';

/** The standard units by the symbol a face publishes (omx-contract's param domain, PARAM_UNITS: each word's x-symbol,
 * % for percent, dB/oct for dB-oct, '' for none; F4): `{ <symbol>: <PARAM_UNITS word> }`. No published unit is renamed. */
let standardUnits;
export function standardUnitsBySymbol() {
  if (standardUnits) return standardUnits;
  const { file, schemas } = domainDocument('param', 'omx-contract-data >= 3.21.0');
  const words = schemas.PARAM_UNITS?.oneOf;
  if (!Array.isArray(words)) throw new Error(`${file}: holds no PARAM_UNITS words`);
  standardUnits = Object.fromEntries(words.map((w) => [w['x-symbol'] ?? w.const, w.const]));
  return standardUnits;
}
/** The PARAM_UNITS word a published unit symbol is, or undefined (a plugin's own unit: N1's, not in this pass). */
export const standardUnitOf = (symbol) => standardUnitsBySymbol()[symbol];

/** The units a declaration writes that the LV2 table (what a scan reads back) has no row for, each one of the OpenMixer
 * spellings F1 maps (plugin-db's lv2 domain, LV2_OPENMIXER_UNIT_IDS: the PARAM_UNITS word and the exact URI an LV2
 * file writes, x-uri; sorting row M3): `{ <published symbol>: <uri> }`, dB/oct -> urn:openmixer:unit#dBoct. The plugin's
 * TTL defines each it uses (tools/gen.mjs LV2_CUSTOM_UNITS). */
let ownUnits;
export function ownLv2Units() {
  if (ownUnits) return ownUnits;
  const { file, schemas } = domainDocument('lv2', 'plugin-db-contract >= 0.5.0');
  const words = schemas.LV2_OPENMIXER_UNIT_IDS?.oneOf;
  if (!Array.isArray(words)) throw new Error(`${file}: holds no LV2_OPENMIXER_UNIT_IDS words (plugin-db-contract >= 0.5.0)`);
  const symbolOf = Object.fromEntries(Object.entries(standardUnitsBySymbol()).map(([sym, word]) => [word, sym]));
  ownUnits = {};
  for (const w of words) {
    if (typeof w['x-uri'] !== 'string') throw new Error(`${file}: LV2_OPENMIXER_UNIT_IDS.${w.title} has no x-uri`);
    if (symbolOf[w.const] === undefined) throw new Error(`${file}: LV2_OPENMIXER_UNIT_IDS.${w.title} is ${w.const}, no word of PARAM_UNITS`);
    ownUnits[symbolOf[w.const]] = w['x-uri'];
  }
  return ownUnits;
}

/** The `units:unit` value of a declared unit, as a TTL writes it: the LV2 table read backwards (units:db for dB,
 * <urn:openmixer:unit#dBFS> for dBFS), else the declaration's own unit (ownLv2Units, F1); none for "". */
export function lv2UnitRef(unit) {
  const key = unit ? outboundValue('lv2', UNIT_WORD, unit) ?? ownLv2Units()[unit] : undefined;
  if (!key) return undefined;
  return key.startsWith('units:') ? key : `<${key}>`;
}

/** The LV2 name of each unit a face can write, `{ <unit>: <name> }` (`units:db` -> db, `urn:openmixer:unit#dBFS` -> dBFS). */
export function lv2UnitNames() {
  const pairs = [...Object.entries(lookupValues('lv2', UNIT_WORD)).map(([k, v]) => [v, k]), ...Object.entries(ownLv2Units())];
  return Object.fromEntries(pairs.map(([unit, key]) => [unit, key.split(/[:#]/).pop()]));
}

/** The LV2 designation of a meaning, the LV2 table read backwards (gain -> param:gain), or undefined. */
export const lv2Designation = (meaning) => (meaning ? outboundValue('lv2', MEANING_WORD, meaning) : undefined);
