// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * format-tables.mjs — plugin-hostd's format tables, read backwards (FreeMixer/omx-plugins#210, decision B: one
 * vocabulary, two directions). plugin-hostd installs one mapping table per format as a domain of the contract
 * directory, plugin-hostd/clap and plugin-hostd/lv2 (named by their items' prefix, PLUGIN_HOSTD_CLAP_MAP and
 * PLUGIN_HOSTD_LV2_MAP), <contractdir>/plugin-hostd/<format>/<format>.json, each item omx-contract's mapping-table kind
 * (`x-kind: map`, sorting row V19): `x-format` and a `oneOf` of one row per source field, `const` the field, `x-term` or
 * `x-from` what covers it, and `x-word`, `x-extension` or `x-dropped` (plugin-hostd docs/formats.md): how each field a
 * scan reads becomes a descriptor word. A generator writes a face from a descriptor by reading the same rows the other
 * way: the row whose pair maps a source value to the word's value names what the face writes. Only the rows whose
 * transform carries pairs are read here: `lookup`, whose `values` the row holds, and `unit-id` and `designation-meaning`,
 * whose pairs the format level's owner publishes (row M5, docs/formats.md "Lookup values the owners publish"; PAIRS
 * below). No transform is code of ours (what is structure, a module path or a scale point, stays in the generator that
 * writes it).
 *
 * $OMX_INSTALLED_CONTRACTDIR, a contract directory holding the domains, stands in for the installed one.
 */
import { existsSync, readFileSync } from 'node:fs';
import { join } from 'node:path';
import { domainDocument, installedContractDir } from './omx-contract.mjs';

/** The item each format's domain holds, and the domain (named by the item's prefix). */
const ITEM = { clap: 'PLUGIN_HOSTD_CLAP_MAP', lv2: 'PLUGIN_HOSTD_LV2_MAP' };
export const tableDomain = (format) => `plugin-hostd/${format}`;
/** The floor of the release whose tables are x-kind map items with the M5 rows (plugin-hostd#233, on its 0.22.0). */
const TABLES_WANT = 'plugin-hostd-contract >= 0.22.0';

/** The contract directory the tables' domains are read from, or undefined. */
export const tablesDir = () => installedContractDir();

const cache = new Map();
/** The rows of `format`'s table (`clap`, `lv2`): `{ <source field>: { word? | extension? | dropped?, term? | from? } }`,
 * each row's `x-` members without their prefix. */
export function tableRows(format) {
  if (!ITEM[format]) throw new Error(`format-tables: no table for format '${format}'`);
  const dir = tablesDir();
  const file = dir && join(dir, tableDomain(format), `${format}.json`);
  if (!cache.has(file)) {
    if (!file || !existsSync(file)) {
      throw new Error(`plugin-hostd's ${format} table, the domain ${tableDomain(format)}, is not installed (${file ?? 'no contract directory: pkg-config --variable=contractdir omx-contract finds none'}): this tree needs ${TABLES_WANT} (packaging/omx-plugins.spec)`);
    }
    const item = JSON.parse(readFileSync(file, 'utf8')).components?.schemas?.[ITEM[format]];
    if (item?.['x-kind'] !== 'map' || item['x-format'] !== format || !Array.isArray(item.oneOf)) throw new Error(`${file}: holds no ${ITEM[format]} map of format ${format} (x-kind map, ${TABLES_WANT})`);
    const rows = {};
    for (const r of item.oneOf) {
      if (typeof r?.const !== 'string' || rows[r.const]) throw new Error(`${file}: ${ITEM[format]} has a row with no field, or two rows for ${JSON.stringify(r?.const)}`);
      rows[r.const] = Object.fromEntries(['word', 'extension', 'dropped', 'term', 'from'].filter((k) => r[`x-${k}`] !== undefined).map((k) => [k, r[`x-${k}`]]));
    }
    cache.set(file, rows);
  }
  return cache.get(file);
}

// ---- the pairs a row's transform carries ------------------------------------------------------------------------------

/** One owner's mapping item, `{ file, item }`: the domain document checked to be the owner's (info.x-owner), the item
 * present. */
function ownerItem(domain, owner, name, want) {
  const { file, doc, schemas } = domainDocument(domain, want);
  if (doc.info?.['x-owner'] !== owner) throw new Error(`${file}: the domain ${domain} is ${JSON.stringify(doc.info?.['x-owner'])}'s, not ${owner}'s (${want})`);
  if (!schemas[name]) throw new Error(`${file}: holds no ${name} (${want})`);
  return { file, item: schemas[name] };
}

/** The words of an owner's item whose `x-from` names the TERM `term` (`LV2_UNITS_TERMS#db`: a `$ref` to that item and
 * its `x-term`). */
const wordsFrom = (item, term) => {
  const [of, t] = term.split('#');
  return (item.oneOf ?? []).filter((w) => (w['x-from'] ?? []).some((f) => typeof f?.$ref === 'string' && f.$ref.endsWith(`/${of}`) && f['x-term'] === t));
};

const PLUGIN_DB_LV2 = ['lv2', 'plugin-db', 'plugin-db-contract >= 0.5.1'];
/** The transforms whose data is pairs, and where each format's owner publishes them: (row key, row) -> `{ <source
 * value>: <value> }`. */
const PAIRS = {
  lookup: { clap: (k, row) => row.word.values ?? {}, lv2: (k, row) => row.word.values ?? {} },
  'unit-id': {
    clap: () => {
      const { file, item } = ownerItem('omx-param-contract', 'omx-clap', 'OMX_PARAM_CONTRACT_UNIT_IDS', 'omx-clap-contract >= 0.35.0');
      if (item['x-kind'] !== 'sheet' || typeof item['x-value'] !== 'object') throw new Error(`${file}: OMX_PARAM_CONTRACT_UNIT_IDS is no sheet`);
      return item['x-value'];
    },
    lv2: (k, row) => {
      if (k === 'units:unit') {
        const { item } = ownerItem(...PLUGIN_DB_LV2.slice(0, 2), 'LV2_OPENMIXER_UNIT_IDS', PLUGIN_DB_LV2[2]);
        return Object.fromEntries((item.oneOf ?? []).map((w) => [w['x-uri'], w.const]));
      }
      const { item } = ownerItem(...PLUGIN_DB_LV2.slice(0, 2), 'LV2_UNIT_IDS', PLUGIN_DB_LV2[2]);
      return Object.fromEntries(wordsFrom(item, row.term ?? '').map((w) => [k, w.const]));
    },
  },
  'designation-meaning': {
    lv2: (k, row) => {
      const { item } = ownerItem(...PLUGIN_DB_LV2.slice(0, 2), 'LV2_DESIGNATION_MEANINGS', PLUGIN_DB_LV2[2]);
      return Object.fromEntries(wordsFrom(item, row.term ?? '').map((w) => [k, w.const]));
    },
  },
};

const pairsCache = new Map();
/** The pairs of `format`'s row `key`, `{ <source value>: <value> }`, or undefined for a row whose transform carries
 * none. A row whose owner's mapping holds no pair for it is refused by name, as plugin-hostd's describe refuses it. */
function rowPairs(format, key, row) {
  const read = PAIRS[row.word?.transform]?.[format];
  if (!read) return undefined;
  const id = `${tablesDir()}\0${format}\0${key}`;
  if (!pairsCache.has(id)) {
    const pairs = read(key, row);
    if (!Object.keys(pairs).length) throw new Error(`plugin-hostd's ${format} row ${key} (${row.word.transform}${row.term ? `, ${row.term}` : ''}) has no pair in its owner's mapping: no face can write it`);
    pairsCache.set(id, pairs);
  }
  return pairsCache.get(id);
}

/** Whether the rows of `format` giving word `word` all carry pairs (a value none of them gives is no face's). */
export const pairedOnly = (format, word) => Object.entries(tableRows(format)).every(([k, r]) => r.word?.word !== word || rowPairs(format, k, r) !== undefined);

/**
 * What a face of `format` writes for descriptor word `word` (`controls[].travel.unit`, `controls[].meaning`) taking
 * `value`: the source value of the one row whose pair maps to it (`units:db` for dB, `param:gain` for gain,
 * `urn:openmixer:unit#dBFS` for dBFS), or undefined when no row does. Throws when two rows would.
 */
export function outboundValue(format, word, value) {
  const keys = Object.entries(lookupValues(format, word)).filter(([, v]) => v === value).map(([k]) => k);
  if (keys.length > 1) throw new Error(`plugin-hostd's ${format} table maps ${word} ${JSON.stringify(value)} from ${keys.join(' and ')}: a face writes one`);
  return keys[0];
}

/** Every value `format`'s table maps to word `word` through a row that carries pairs, `{ <source value>: <value> }`. */
export function lookupValues(format, word) {
  const out = {};
  for (const [k, row] of Object.entries(tableRows(format))) if (row.word?.word === word) Object.assign(out, rowPairs(format, k, row) ?? {});
  return out;
}

/** The descriptor words `format`'s table produces at all (a row of class word naming it). */
export function tableWords(format) {
  return new Set(Object.values(tableRows(format)).filter((r) => r.word).map((r) => r.word.word));
}

// ---- the CLAP features a face writes, as a scan reads them -----------------------------------------------------------

/** omx-clap's CLAP feature terms (CLAP_PLUGIN_FEATURES_TERMS of its domain clap, <contractdir>/clap/clap.json,
 * omx-clap-contract >= 0.34.0; sorting row V24/27): `{ <value>: <term id> }`, delay -> CLAP_PLUGIN_FEATURE_DELAY,
 * each term's `x-term` value and id (omx-contract 3.21.0's terms kind: `const` and `title` are the term's key). A field dump writes a feature the headers define by its macro name (plugin-hostd
 * docs/formats.md, The field dump), so the bag carries that name and a face writes its value. */
let featureTerms;
export function clapFeatureTerms() {
  if (featureTerms) return featureTerms;
  const { file, schemas } = domainDocument('clap', 'omx-clap-contract >= 0.34.0');
  const words = schemas.CLAP_PLUGIN_FEATURES_TERMS?.oneOf;
  if (!Array.isArray(words) || !words.length) throw new Error(`${file}: holds no CLAP_PLUGIN_FEATURES_TERMS (omx-clap-contract >= 0.34.0 installs it)`);
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

/** A declared unit as the PARAM_UNITS id the tables' pairs carry (M5, decision 2 of plugin-hostd#215): the id its
 * published symbol stands for (% -> percent, dB/oct -> dB-oct), or the unit itself when it is already an id. */
export const unitId = (unit) => standardUnitOf(unit) ?? unit;
/** The symbol a face publishes for PARAM_UNITS id `id` (its x-symbol, else the id), or undefined for no PARAM_UNITS word. */
const symbolOfId = (id) => Object.entries(standardUnitsBySymbol()).find(([, w]) => w === id)?.[0];

/** The OpenMixer units, the pairs of the LV2 table's `units:unit` row (plugin-db's lv2 domain, LV2_OPENMIXER_UNIT_IDS:
 * the exact URI an LV2 file writes, x-uri, and its PARAM_UNITS id; sorting rows M3, M5): `{ <published symbol>: <uri> }`,
 * dB/oct -> urn:openmixer:unit#dBoct. The plugin's TTL defines each it uses (tools/gen.mjs LV2_CUSTOM_UNITS). */
export function ownLv2Units() {
  const row = tableRows('lv2')['units:unit'];
  const pairs = row?.word?.word === UNIT_WORD ? rowPairs('lv2', 'units:unit', row) : undefined;
  if (!pairs) throw new Error(`plugin-hostd's lv2 table has no units:unit row whose pairs give ${UNIT_WORD} (${TABLES_WANT})`);
  return Object.fromEntries(Object.entries(pairs).map(([uri, id]) => {
    const symbol = symbolOfId(id);
    if (symbol === undefined) throw new Error(`plugin-db's LV2_OPENMIXER_UNIT_IDS maps ${uri} to ${id}, no word of PARAM_UNITS`);
    return [symbol, uri];
  }));
}

/** The `units:unit` value of a declared unit (a published symbol or its PARAM_UNITS id), as a TTL writes it: the LV2
 * table read backwards (units:db for dB, units:pc for % and percent, <urn:openmixer:unit#dBFS> for dBFS); none for "". */
export function lv2UnitRef(unit) {
  const key = unit ? outboundValue('lv2', UNIT_WORD, unitId(unit)) : undefined;
  if (!key) return undefined;
  return key.startsWith('units:') ? key : `<${key}>`;
}

/** The LV2 name of each unit a face can write, by its published symbol, `{ <unit>: <name> }` (`units:db` -> db,
 * `urn:openmixer:unit#dBFS` -> dBFS). */
export function lv2UnitNames() {
  return Object.fromEntries(Object.entries(lookupValues('lv2', UNIT_WORD)).map(([key, id]) => [symbolOfId(id) ?? id, key.split(/[:#]/).pop()]));
}

/** The LV2 designation of a meaning, the LV2 table read backwards (gain -> param:gain), or undefined. */
export const lv2Designation = (meaning) => (meaning ? outboundValue('lv2', MEANING_WORD, meaning) : undefined);
