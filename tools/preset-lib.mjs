// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * preset-lib.mjs — what a preset file is checked against: each shipped plugin's parameters as its
 * generated header declares them (symbol, unit, travel, integer steps) and the labels of its choices,
 * the unit as the PARAM_UNITS id a preset states (the header's text is the id's x-symbol, value_to_text's; C2),
 * read from the generated LV2 description, and plugin-hostd's preset schema. Shared by tools/gen-presets.mjs (which writes the files)
 * and tools/test/presets-check.mjs (which holds them to the travels).
 */
import { existsSync, readdirSync, readFileSync } from 'node:fs';
import { execFileSync } from 'node:child_process';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { standardUnitSymbols } from './format-tables.mjs';
import { rawDeclOf } from './variants.mjs';

export const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..');
export const FORMAT = 'org.openmixer.preset/1';
export const PRESET_DIR = 'presets';

/** The shipped plugins that have a parameter header: `{ stem, id, uri, params, labels }`. */
export function plugins(root = ROOT) {
  const out = [];
  for (const stem of readdirSync(join(root, 'plugins')).sort()) {
    const gen = join(root, 'plugins', stem, 'generated');
    if (!existsSync(gen)) continue;
    const header = readdirSync(gen).find((f) => f.endsWith('_params.h'));
    if (!header) continue;
    const text = readFileSync(join(gen, header), 'utf8');
    out.push({ stem, id: `org.openmixer.${stem.replace(/^omx-/, '')}`, uri: text.match(/_LV2_URI "([^"]+)"/)?.[1], params: paramsOf(text, header, rawDeclOf(join(root, 'plugins', stem)).units), labels: labelsOf(join(gen, `${stem}.lv2`, `${stem}.ttl`)) });
  }
  return out;
}

/** The unit id whose x-symbol a header's unit text is: a PARAM_UNITS id (`%` -> percent, `` -> none), or one of the
 * plugin's own units (`units`, its declaration's block), else the text as written. The symbols are distinct (an own
 * x-symbol that is a PARAM_UNITS one is refused, tools/format-tables.mjs ownUnitRefusals), so the text names one id. */
let idBySymbol;
export const unitIdOfText = (text, units = []) =>
  (idBySymbol ??= new Map(Object.entries(standardUnitSymbols()).map(([id, symbol]) => [symbol, id]))).get(text) ?? units.find((u) => u['x-symbol'] === text)?.const ?? text;

/** The rows of the OMX_<X>_PARAMS table: `{ symbol, name, unit, min, max, def, integer, toggle }`, the unit its id. */
function paramsOf(text, file, units) {
  const table = text.match(/OMX_\w+_PARAMS\[[^\]]*\]\s*=\s*\{([\s\S]*?)\n\};/);
  if (!table) throw new Error(`${file}: no parameter table`);
  const rows = [...table[1].matchAll(/\{\s*"([^"]+)",\s*"([^"]*)",\s*"([^"]*)",\s*(-?[\d.]+)f,\s*(-?[\d.]+)f,\s*(-?[\d.]+)f,\s*([^}]+?)\s*\}/g)];
  return rows.map((m) => ({ symbol: m[1], name: m[2], unit: unitIdOfText(m[3], units), min: Number(m[4]), max: Number(m[5]), def: Number(m[6]), integer: /INTEGER|TOGGLE/.test(m[7]), toggle: /TOGGLE/.test(m[7]) }));
}

/** symbol -> labels by value, from the LV2 description's scale points. */
function labelsOf(ttl) {
  const text = existsSync(ttl) ? readFileSync(ttl, 'utf8') : '';
  const out = new Map();
  for (const part of text.split(/lv2:symbol "/).slice(1)) {
    const symbol = part.slice(0, part.indexOf('"'));
    const points = [...part.matchAll(/\[\s*rdfs:label "([^"]*)"\s*;\s*rdf:value (-?\d+)\s*\]/g)];
    if (points.length) out.set(symbol, new Map(points.map((m) => [Number(m[2]), m[1]])));
  }
  return out;
}

/** Every preset file: `{ file, dir, preset }`. */
export function presetFiles(root = ROOT) {
  const base = join(root, PRESET_DIR);
  if (!existsSync(base)) return [];
  return readdirSync(base).sort().flatMap((dir) => readdirSync(join(base, dir)).filter((f) => f.endsWith('.json')).sort().map((f) => ({ file: `${PRESET_DIR}/${dir}/${f}`, dir, preset: JSON.parse(readFileSync(join(base, dir, f), 'utf8')) })));
}

/** The path of plugin-hostd's preset schema: PLUGIN_HOSTD_PRESET_SCHEMA, else under the installed package's prefix. */
function schemaPath() {
  if (process.env.PLUGIN_HOSTD_PRESET_SCHEMA) return process.env.PLUGIN_HOSTD_PRESET_SCHEMA;
  const prefix = execFileSync('pkg-config', ['--variable=prefix', 'plugin-hostd'], { encoding: 'utf8' }).trim();
  return join(prefix, 'share/plugin-hostd/preset.schema.json');
}

let schemaRead;
/** plugin-hostd's preset schema, the one its daemon checks a preset file against. */
export function presetSchema() {
  return (schemaRead ??= JSON.parse(readFileSync(schemaPath(), 'utf8')));
}

const SCHEMA_WORDS = new Set(['$schema', '$id', '$defs', 'title', 'description', 'type', 'enum', 'const', 'required', 'properties', 'items', 'minLength', 'maxLength', 'pattern', 'allOf', 'if', 'then', 'not', '$ref']);
const isObject = (v) => typeof v === 'object' && v !== null && !Array.isArray(v);
const TYPES = { object: isObject, array: Array.isArray, string: (v) => typeof v === 'string', number: (v) => typeof v === 'number' };

/**
 * Where `value` breaks `schema`, as JSON Schema reads the words the preset schema uses; an empty list is valid.
 * Throws on a word it does not know, so a schema that grows one is never half read.
 */
export function schemaProblems(value, schema, root = schema, at = '') {
  const out = [];
  const name = at || '(the file)';
  for (const word of Object.keys(schema)) if (!SCHEMA_WORDS.has(word)) throw new Error(`preset schema: '${word}' is not read here`);
  if (schema.$ref) out.push(...schemaProblems(value, root.$defs[schema.$ref.replace('#/$defs/', '')], root, at));
  if (schema.type && !TYPES[schema.type](value)) out.push(`${name} is not of type ${schema.type}`);
  if (schema.enum && !schema.enum.includes(value)) out.push(`${name} is none of ${schema.enum.join(', ')}`);
  if ('const' in schema && value !== schema.const) out.push(`${name} is not ${schema.const}`);
  if (typeof value === 'string') {
    if (schema.minLength !== undefined && value.length < schema.minLength) out.push(`${name} is shorter than ${schema.minLength}`);
    if (schema.maxLength !== undefined && value.length > schema.maxLength) out.push(`${name} is longer than ${schema.maxLength}`);
    if (schema.pattern && !new RegExp(schema.pattern).test(value)) out.push(`${name} does not match ${schema.pattern}`);
  }
  if (isObject(value)) {
    for (const key of schema.required ?? []) if (!(key in value)) out.push(`${name} has no ${key}`);
    for (const [key, sub] of Object.entries(schema.properties ?? {})) if (key in value) out.push(...schemaProblems(value[key], sub, root, at ? `${at}.${key}` : key));
  }
  if (Array.isArray(value) && schema.items) value.forEach((v, k) => out.push(...schemaProblems(v, schema.items, root, `${at}[${k}]`)));
  for (const sub of schema.allOf ?? []) out.push(...schemaProblems(value, sub, root, at));
  if (schema.if && schema.then && schemaProblems(value, schema.if, root, at).length === 0) out.push(...schemaProblems(value, schema.then, root, at));
  if (schema.not && schemaProblems(value, schema.not, root, at).length === 0) out.push(`${name} is what the schema refuses`);
  return out;
}

/** The problems with one preset against its plugin; an empty list is a clean file. */
export function problems(file, dir, preset, plugin) {
  const bad = [];
  const at = (m) => bad.push(`${file}: ${m}`);
  for (const m of schemaProblems(preset, presetSchema())) at(m);
  if (preset.format !== FORMAT) at(`format is ${JSON.stringify(preset.format)}, expected ${FORMAT}`);
  if (!plugin) return [...bad, `${file}: no shipped plugin has id ${dir}`];
  if (preset.plugin?.id !== plugin.id) at(`plugin.id is ${preset.plugin?.id}, the directory is ${plugin.id}`);
  if (!/^[a-z0-9]+(-[a-z0-9]+)*$/.test(preset.id ?? '')) at(`id ${JSON.stringify(preset.id)} is not a lower-case dash id`);
  if (file.split('/').pop() !== `${preset.id}.json`) at(`the file name is not ${preset.id}.json`);
  if (typeof preset.name !== 'string' || !preset.name) at('no name');
  if (preset.carries !== 'params') at(`carries is ${JSON.stringify(preset.carries)}, a factory preset carries params`);
  if (!('copyOf' in preset)) at('no copyOf');
  for (const k of ['createdAt', 'modifiedAt']) if (Number.isNaN(Date.parse(preset[k]))) at(`${k} is not a date`);
  if (!Array.isArray(preset.params) || preset.params.length === 0) return [...bad, `${file}: no params`];
  const seen = new Set();
  for (const p of preset.params) {
    const decl = plugin.params.find((d) => d.symbol === p.symbol);
    if (!decl) { at(`'${p.symbol}' is not a parameter of ${plugin.id}`); continue; }
    if (seen.has(p.symbol)) at(`'${p.symbol}' is set twice`);
    seen.add(p.symbol);
    if (p.unit !== decl.unit) at(`'${p.symbol}' unit is ${JSON.stringify(p.unit)}, declared ${JSON.stringify(decl.unit)}`);
    if (typeof p.value !== 'number' || !Number.isFinite(p.value)) { at(`'${p.symbol}' value is not a number`); continue; }
    if (p.value < decl.min || p.value > decl.max) at(`'${p.symbol}' = ${p.value} is outside ${decl.min}..${decl.max}`);
    if (decl.integer && !Number.isInteger(p.value) && p.value !== decl.def) at(`'${p.symbol}' = ${p.value} is not a whole step`);
    const label = plugin.labels.get(p.symbol)?.get(p.value);
    if ((p.label ?? undefined) !== label) at(`'${p.symbol}' label is ${JSON.stringify(p.label)}, the plugin names ${p.value} ${JSON.stringify(label)}`);
  }
  return bad;
}

// ---- the LV2 face of the presets (docs/presets.md) ------------------------------------------------

const PSET_PREFIXES = '@prefix lv2:  <http://lv2plug.in/ns/lv2core#> .\n@prefix pset: <http://lv2plug.in/ns/ext/presets#> .\n@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> .\n';
const ttlString = (s) => `"${s.replace(/\\/g, '\\\\').replace(/"/g, '\\"')}"`;
/** A plain value as a Turtle decimal: always with a point, so every host reads a float. */
const ttlDecimal = (n) => (Number.isInteger(n) ? `${n}.0` : `${n}`);
/** The preset's resource in the bundle: the plugin's URI, the fragment `preset-<preset id>`. */
export const presetUri = (uri, preset) => `${uri}#preset-${preset.id}`;

/** The bundle's presets.ttl: one pset:Preset per preset file of the plugin, in file order. */
export function presetsTtl(plugin, presets) {
  const body = presets.map((p) => [
    `<${presetUri(plugin.uri, p)}>`,
    '    a pset:Preset ;',
    `    lv2:appliesTo <${plugin.uri}> ;`,
    `    rdfs:label ${ttlString(p.name)} ;`,
    '    lv2:port',
    p.params.map((q) => `        [ lv2:symbol ${ttlString(q.symbol)} ; pset:value ${ttlDecimal(q.value)} ]`).join(' ,\n') + ' .',
  ].join('\n'));
  return [`# SPDX-License-Identifier: GPL-3.0-or-later\n# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>\n# GENERATED by tools/gen-presets.mjs from presets/${plugin.id}/ — DO NOT EDIT BY HAND.`, PSET_PREFIXES, ...body.map((b) => `${b}\n`)].join('\n');
}

/** The manifest's lines for the same presets: each typed, bound to the plugin, described in presets.ttl. */
export function presetsManifest(uri, presets) {
  return presets.map((p) => `\n<${presetUri(uri, p)}>\n    a pset:Preset ;\n    lv2:appliesTo <${uri}> ;\n    rdfs:seeAlso <presets.ttl> .\n`).join('');
}

/** The preset files of one plugin id, parsed, in file order. */
export const presetsOf = (id, root = ROOT) => presetFiles(root).filter((f) => f.dir === id).map((f) => f.preset);
