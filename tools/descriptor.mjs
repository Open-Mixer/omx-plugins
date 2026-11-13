// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * descriptor.mjs — a plugin of ours in omx-contract's descriptor words (omx-contract docs/descriptors.md;
 * docs/declaration-as-descriptor.md):
 *
 *   - descriptorOf(d): the `authored` descriptor of a declaration as tools/gen.mjs loads it: the plugin's
 *     identity, one control per parameter, its ports, its latency, its meters. A plugin folder holding
 *     <stem>.descriptor.json keeps it equal to this render (gen.mjs writes it, --check refuses it stale),
 *     so the travels it states are always what its links resolve to.
 *   - viewOf(descriptor, extras): the declaration tools/gen.mjs resolves, from the descriptor and the
 *     extras its <stem>.decl.json keeps: what the descriptor has no word for.
 *
 * omx-panel lays a panel out from a descriptor of any origin (tools/omx-panel.mjs); for ours it reads this one.
 */
import { existsSync, readFileSync } from 'node:fs';
import { join } from 'node:path';
import { choicesOf, metersOf, scaleOf } from './gen.mjs';
import { clapFeatureField, clapFeatureValue } from './format-tables.mjs';
import { installedContractDir, installedDataDir, locateContract, ownersOf, packagingRange } from './omx-contract.mjs';
import { meaningOfRole, panelLayout } from './omx-panel.mjs';

/** The descriptor file of a plugin folder. */
export const descriptorFile = (stem) => `${stem}.descriptor.json`;

/** The schema the descriptor words are in, and the release that added them. */
export const DESCRIPTOR_SCHEMA = 'https://github.com/FreeMixer/omx-contract/raw/v3/schema/omx-contract.schema.json';
export const DESCRIPTOR_SCHEMA_FLOOR = '3.7.0';
/** The release that added `meters` (omx-contract docs/descriptors.md): the floor of a descriptor that states them. */
export const DESCRIPTOR_METERS_FLOOR = '3.9.0';
/** The descriptor's word for each meter kind a declaration names. */
const METER_KIND_WORDS = { gain_reduction: 'gain-reduction', level: 'level' };

/** The schema's file in omx-contract's data directory, where the descriptor words are read from. */
export const DESCRIPTOR_SCHEMA_FILE = 'omx-contract.schema.json';

/** The installed omx-contract's schema: the file $OMX_CONTRACT_SCHEMA names (a moved copy standing in for the install,
 * as the perturbation proofs set it beside OMX_CONTRACT_DIR), else the one in $OMX_CONTRACT_DIR when that copy carries
 * it, else the data directory's (`pkg-config --variable=datadir omx-contract`). */
export function descriptorSchemaFile() {
  const file = [process.env.OMX_CONTRACT_SCHEMA, ...[process.env.OMX_CONTRACT_DIR, installedDataDir()].filter(Boolean).map((d) => join(d, DESCRIPTOR_SCHEMA_FILE))].filter(Boolean).find((f) => existsSync(f));
  if (!file) throw new Error(`${DESCRIPTOR_SCHEMA_FILE}: not in the installed omx-contract's data directory; install omx-contract-devel >= ${DESCRIPTOR_SCHEMA_FLOOR} (BUILDING.md)`);
  return file;
}

/** The plugin features a descriptor names, the enum of the schema's `$defs.plugin` features (omx-contract
 * docs/descriptors.md); a declaration's other CLAP features have no word there. */
export function descriptorFeatures(schema = JSON.parse(readFileSync(descriptorSchemaFile(), 'utf8'))) {
  const list = schema?.$defs?.plugin?.properties?.features?.items?.enum;
  if (!Array.isArray(list) || !list.length || list.some((w) => typeof w !== 'string')) throw new Error(`${DESCRIPTOR_SCHEMA_FILE}: $defs.plugin.properties.features.items.enum is not the descriptor's feature words`);
  return list;
}

/** The descriptor's feature words, the installed schema's: never spelled here (tools/test/descriptor-features-check.mjs
 * refuses a list of them in any tool). */
export const FEATURES = descriptorFeatures();

/** The extension bag's source fields our faces write (omx-contract docs/descriptors.md, The extension bag): CLAP's
 * open features, in the face's order, and the LV2 face's types as its .ttl states them. */
export const CLAP_FEATURES_FIELD = 'clap_plugin_descriptor.features';
export const LV2_TYPE_FIELD = 'rdf:type';
const LV2_PLUGIN = 'lv2:Plugin';

/** What every LV2 face states it can use (tools/gen.mjs writes lv2:optionalFeature lv2:hardRTCapable), the
 * descriptor's needs.extensions.optional. */
export const LV2_HARD_RT = 'lv2:hardRTCapable';

/** The bag of a declaration: `{ clap?, lv2 }`. */
export function extensionsOf(d) {
  const open = d.clap.features.filter((f) => !FEATURES.includes(f));
  return {
    ...(open.length ? { clap: { [CLAP_FEATURES_FIELD]: open.length === 1 ? clapFeatureField(open[0]) : open.map(clapFeatureField) } } : {}),
    lv2: { [LV2_TYPE_FIELD]: d.lv2.class === LV2_PLUGIN ? LV2_PLUGIN : [LV2_PLUGIN, d.lv2.class] },
  };
}

/** The declaration's CLAP features and LV2 class, read back from a descriptor's words and bag. */
function bagOf(descriptor, file) {
  const ext = descriptor.extensions ?? {};
  const open = [ext.clap?.[CLAP_FEATURES_FIELD] ?? []].flat();
  const types = [ext.lv2?.[LV2_TYPE_FIELD] ?? []].flat();
  if (!Array.isArray(open) || open.some((f) => FEATURES.includes(clapFeatureValue(f)))) throw new Error(`${file}: extensions.clap["${CLAP_FEATURES_FIELD}"] holds CLAP's open features, a list without the ones plugin.features names`);
  if (!types.includes(LV2_PLUGIN) || types.length > 2) throw new Error(`${file}: extensions.lv2["${LV2_TYPE_FIELD}"] is the LV2 face's types, "${LV2_PLUGIN}" and at most its class`);
  return { features: [...descriptor.plugin.features, ...open.map(clapFeatureValue)], class: types.find((t) => t !== LV2_PLUGIN) ?? LV2_PLUGIN };
}

/** A descriptor's unit for a travel whose link resolves to `unit`: the item list's "" is a ratio or a fraction,
 * and a ratio (n:1) states `ratio`, the one word the descriptor says beyond its link (gap 4). */
const RATIO = 'ratio';

/** An integer travel's scale (PARAM_SCALES): stepped, what CLAP_PARAM_IS_STEPPED, lv2:integer and the parameter contract
 * say; log when its law is logarithmic (an integer frequency), which its LV2 face states with pprops:logarithmic beside
 * lv2:integer and plugin-hostd's describe reads as log (plugin-hostd#142). */
const STEPPED = 'stepped';
const integerScale = (p) => (scaleOf(p) === 'log' ? 'log' : STEPPED);
export const travelUnit = (unit, authored) => (unit === '' && authored === RATIO ? RATIO : unit);

/** A reference link, `<file>#<NAME>[.<field>]`, as its parts. */
const LINK = /^(data\/[A-Za-z0-9_./-]+\.json)#([A-Za-z0-9_]+)(?:\.([A-Za-z0-9_]+))?$/;
export function parseLink(link) {
  const m = LINK.exec(link ?? '');
  if (!m) return undefined;
  const kernel = /^data\/kernels\/([a-z0-9_]+)\.json$/.exec(m[1])?.[1];
  return { file: m[1], ref: m[2], field: m[3], kernel };
}

/** The per-control extras a declaration keeps: how a value is read, not the value. */
export const CONTROL_EXTRAS = ['forKind', 'defaultRef', 'defaultId', 'defaultBand', 'perBand', 'unit', 'values', 'own', 'control'];

/**
 * The LV2 port groups of declaration `d` (as tools/gen.mjs loads it), in panel section order: one per section, or,
 * for a section the declaration splits (`subgroups`, tools/variants.mjs), one per subgroup and one for the section's
 * controls no subgroup holds, if any. Each is `{ key, label, path, controls, priority }`: `key` its lv2:symbol, `label`
 * its lv2:name and rdfs:label, `path` its controls' descriptor `group` (the panel section's label first, the group's
 * last; a section's own group has a one-element path), `priority` its lv2:displayPriority, the groups counted down
 * in the order their first port comes (a host that shows the higher first, as Zynthian does, shows them in port
 * order). A section with no controls left makes no group.
 */
export function portGroupsOf(d) {
  const groups = [];
  for (const s of d.panel?.sections ?? []) {
    const subs = d.subgroups?.[s.key] ?? [];
    const held = new Set(subs.flatMap((g) => g.controls));
    const own = s.controls.filter((sym) => !held.has(sym));
    if (own.length) groups.push({ key: s.key, label: s.label, path: [s.label], controls: own });
    for (const g of subs) groups.push({ key: g.key, label: g.label, path: [s.label, g.label], controls: g.controls });
  }
  const indexOf = new Map(d.params.map((p, i) => [p.symbol, i]));
  const first = (g) => Math.min(...g.controls.map((sym) => indexOf.get(sym) ?? Infinity));
  const order = [...groups].sort((a, b) => first(a) - first(b));
  for (const g of groups) g.priority = groups.length - order.indexOf(g);
  return groups;
}

/**
 * `{ descriptor, extras }` of declaration `d`, every parameter resolved. Each control's id is the
 * parameter's index; its symbol and name the parameter's; a toggle is a toggle, an integer with labelled
 * values a choice (ids the values, as text), any other parameter a travel with the declared unit and
 * scale; its group the label of the panel section listing it (and of its port group within it, where the
 * declaration splits the section: portGroupsOf), its meaning the one its declared role
 * plays; its source the link its travel is read from, or the face for a parameter of the face's own.
 * `extras` carries what the descriptor has no word for: a choice's unit and the per-kind come-up values.
 */
export function descriptorOf(d) {
  const groupOf = new Map();
  for (const g of portGroupsOf(d)) for (const sym of g.controls) groupOf.set(sym, g.path);
  const meaningOf = new Map();
  for (const [role, syms] of Object.entries(d.panel?.roles ?? {})) {
    const m = meaningOfRole(role);
    if (m) for (const sym of [syms].flat()) if (!meaningOf.has(sym)) meaningOf.set(sym, m);
  }
  // the release a link is read from: its owner (the contract directory's manifests, from omx-contract 2.15.0
  // on; omx-contract itself before) at the floor the packaging builds against, else the installed version
  let owners;
  const atOf = (rel) => {
    if (owners === undefined) {
      // the owners beside the render read (owners.json: a union render and every copy of it carry
      // one, OMX_CONTRACT_DIR's included), else the installed contract directory's manifests
      const where = locateContract(d.tree);
      owners = ownersOf(where.contractdir ?? where.dir);
      if (!owners.size) owners = ownersOf(installedContractDir());
    }
    const owner = owners.get(rel.split('#')[0]) ?? { package: 'omx-contract' };
    try {
      return `${owner.package}@${packagingRange(`${owner.package}-devel`, d.tree).floor}`;
    } catch {
      return owner.version ? `${owner.package}@${owner.version}` : null;
    }
  };
  const face = `plugins/${d.stem}/omx_${d.kernel}_clap.c#clap_entry`;
  const choiceUnits = {}, byKind = {};
  const controls = d.params.map((p, i) => {
    const c = { id: i, symbol: p.symbol, name: p.name };
    // a described control's prose is the descriptor's own word, kept as written
    const doc = d.descriptor?.controls[i]?.doc;
    if (doc !== undefined) c.doc = doc;
    const choices = choicesOf(p);
    if (p.kind === 'toggle') Object.assign(c, { kind: 'toggle', default: p.def !== 0 });
    else if (choices) {
      Object.assign(c, { kind: 'choice', ids: choices.map((x) => String(x.value)), labels: choices.map((x) => x.label), values: choices.map((x) => x.value), default: String(p.def) });
      if (p.unit) choiceUnits[p.symbol] = p.unit;
    } else {
      c.kind = 'travel';
      c.travel = { min: p.min, max: p.max, step: p.kind === 'integer' ? 1 : 0, unit: travelUnit(p.unit, d.descriptor?.controls[i]?.travel?.unit), default: p.def, scale: p.kind === 'integer' ? integerScale(p) : scaleOf(p) };
    }
    if (groupOf.has(p.symbol)) c.group = groupOf.get(p.symbol);
    if (meaningOf.has(p.symbol)) c.meaning = meaningOf.get(p.symbol);
    c.automatable = true;
    c.source = p.from ? { from: atOf(p.from) ? { link: p.from, at: atOf(p.from) } : p.from } : { impl: face };
    if (p.byKind) byKind[p.symbol] = p.byKind;
    return c;
  });
  const ports = [
    { id: 0, name: 'in', kind: 'audio', direction: 'in', role: 'main', channels: 2, layout: 'stereo' },
    ...(d.sidechain ? [{ id: 1, name: d.sidechain.symbol, kind: 'audio', direction: 'in', role: 'sidechain', channels: 1, layout: 'mono' }] : []),
    { id: 0, name: 'out', kind: 'audio', direction: 'out', role: 'main', channels: 2, layout: 'stereo' },
  ];
  // each meter as the faces publish it: its CLAP id (its index), the symbol a host reads it by, its range, and
  // the kernel readout it publishes, linked where its travel would be; a level the face measures is the face's own
  const meters = metersOf(d).map((m, k) => {
    const link = m.readout ? `data/kernels/${d.kernel}.json#readouts[${m.readout.name}]` : undefined;
    return {
      id: k, symbol: m.id, name: m.label, ...(m.readout ? { doc: m.readout.doc } : {}), kind: METER_KIND_WORDS[m.kind], channels: m.channels,
      unit: m.unit, min: m.min, max: m.max, scale: m.scale,
      source: link ? { from: atOf(link) ? { link, at: atOf(link) } : link } : { impl: face },
    };
  });
  const descriptor = {
    $schema: DESCRIPTOR_SCHEMA,
    requires: { 'omx-contract-schema': `>= ${meters.length ? DESCRIPTOR_METERS_FLOOR : DESCRIPTOR_SCHEMA_FLOOR}` },
    origin: 'authored',
    plugin: { id: d.clap.id, format: 'clap', uri: d.lv2.uri, name: d.name, vendor: d.vendor, version: d.version, doc: d.description, features: d.clap.features.filter((f) => FEATURES.includes(f)) },
    controls,
    ports,
    // what the host must hand: the tempo (a plugin that syncs to it), as its descriptor says; the RT capability always
    // a plugin that reads the tempo maps the time:Position's URIs when its LV2 host lends urid:map
    needs: d.descriptor?.needs?.tempo ? { tempo: true, extensions: { optional: ['urid:map', LV2_HARD_RT] } } : { extensions: { optional: [LV2_HARD_RT] } },
    latency: { reports: true },
    ...(meters.length ? { meters } : {}),
    extensions: extensionsOf(d),
    source: { impl: face },
  };
  return { descriptor, extras: { choiceUnits, byKind } };
}

/** The descriptor file's text: what `node tools/gen.mjs` writes and `--check` holds equal. */
export const descriptorText = (d) => `${JSON.stringify(descriptorOf(d).descriptor, null, 2)}\n`;

/**
 * The description of declaration `d` (as tools/gen.mjs loads it): its descriptor laid out by omx-panel under its
 * declared panel. Every table in published order, each parameter by its symbol and its declared id.
 * Throws on what a host would refuse whole.
 */
export function panelDescription(d) {
  // the units the faces serve beside it (the parameter contract, the declaration's): a ratio travel with no unit of
  // its own is served as ratio (omx-contract's param-contract unit 258, travelUnit), by the LV2 face and the contract
  // alike, so its role is judged on that unit
  const { descriptor, extras } = descriptorOf({ ...d, descriptor: undefined });
  descriptor.controls.forEach((c, i) => {
    if (c.travel) c.travel.unit = travelUnit(c.travel.unit, d.descriptor?.controls[i]?.travel?.unit);
  });
  return panelLayout(descriptor, { panel: d.panel, ns: `fx.${d.face ?? d.kernel}`, where: `plugins/${d.stem}`, ...extras });
}

/**
 * Whether declaration `decl` is whole: a base of variants (omx-eq) or a chain (omx-strip) keeps its own
 * parameters, identity and prose, and its descriptor is a render of it (gen.mjs writes it, --check holds it),
 * never an input. A single-kernel plugin's <stem>.decl.json is the extras of its descriptor, and carries none
 * of these.
 */
export const isWhole = (decl) => decl.variants !== undefined || decl.binding === 'chain';

/** The declaration fields the descriptor says: an extras file beside a descriptor carries none of them. */
const SAID = ['name', 'vendor', 'version', 'params', 'description'];

/**
 * A whole single-kernel declaration as its two files, the inverse of viewOf: `{ descriptor, extras }`,
 * `descriptor` rendered from `prepared` (the declaration tools/gen.mjs resolved from `raw`), `extras`
 * `raw` without what the descriptor says, each parameter's own words (CONTROL_EXTRAS) under `controls`
 * by symbol. A travel the contract's control `ratio` reads with no unit states `ratio` (n:1), as every
 * described ratio of ours does.
 */
export function describedOf(raw, prepared) {
  const descriptor = descriptorOf(prepared).descriptor;
  descriptor.controls.forEach((c, i) => {
    if (c.travel && c.travel.unit === '' && prepared.controls?.[i]?.name === 'ratio') c.travel.unit = RATIO;
  });
  const extras = {};
  for (const [k, v] of Object.entries(raw)) {
    if (SAID.includes(k) || k === 'clap') continue;
    if (k === 'lv2') {
      const { uri, class: cls, ...rest } = v;
      if (Object.keys(rest).length) extras.lv2 = rest;
      continue;
    }
    extras[k] = v;
  }
  const controls = Object.fromEntries(raw.params.flatMap((p) => {
    const x = Object.fromEntries(CONTROL_EXTRAS.filter((k) => k in p).map((k) => [k, p[k]]));
    return Object.keys(x).length ? [[p.symbol, x]] : [];
  }));
  if (Object.keys(controls).length) extras.controls = controls;
  return { descriptor, extras };
}

/**
 * The declaration `extras` (a <stem>.decl.json beside a descriptor) and `descriptor` make together, as
 * tools/gen.mjs resolves it: identity from the descriptor's plugin block, one parameter per control (its
 * reference read from the control's source link, or its travel typed for a control of the face's own),
 * and each control's extras (`controls`, by symbol) folded in. Throws, naming `file`, on what the two
 * would both say or what neither does.
 */
export function viewOf(descriptor, extras, file) {
  for (const k of SAID) if (k in extras) throw new Error(`${file}: "${k}" is the descriptor's to say (${descriptorFile(extras.stem)})`);
  if (extras.clap !== undefined || extras.lv2?.uri !== undefined || extras.lv2?.class !== undefined) throw new Error(`${file}: the plugin's CLAP id and features, its LV2 URI and class are the descriptor's to say (${descriptorFile(extras.stem)}: plugin and extensions)`);
  if (descriptor.origin !== 'authored') throw new Error(`${file}: ${descriptorFile(extras.stem)} is ${descriptor.origin}; ours is authored`);
  const known = new Set(descriptor.controls.map((c) => c.symbol));
  for (const sym of Object.keys(extras.controls ?? {})) if (!known.has(sym)) throw new Error(`${file}: "controls" names '${sym}', no control of the descriptor`);
  const { controls: perControl = {}, ...rest } = extras;
  const pl = descriptor.plugin;
  const bag = bagOf(descriptor, file);
  const params = descriptor.controls.map((c, i) => {
    if (c.id !== i) throw new Error(`${file}: the descriptor's control ${i} has id ${JSON.stringify(c.id)}; our ids are the parameter order`);
    const x = perControl[c.symbol] ?? {};
    for (const k of Object.keys(x)) if (!CONTROL_EXTRAS.includes(k)) throw new Error(`${file}: '${c.symbol}' keeps "${k}", which is not an extra (${CONTROL_EXTRAS.join(', ')})`);
    const p = { symbol: c.symbol, name: c.name };
    const from = c.source?.from;
    if (from !== undefined) {
      const link = parseLink(typeof from === 'string' ? from : from.link);
      if (!link) throw new Error(`${file}: '${c.symbol}' reads its travel from ${JSON.stringify(from)}, not a link <file>#<NAME>[.<field>]`);
      p.ref = link.ref;
      if (link.field) p.field = link.field;
      if (link.kernel && (rest.kernels ?? [rest.kernel]).length > 1) p.kernel = link.kernel;
      // an integer travel is stepped, as both faces and the parameter contract state it; its faces' log hint is its unit's
      if (c.travel && c.travel.scale !== (c.travel.step === 1 ? integerScale({ unit: c.travel.unit, min: c.travel.min }) : scaleOf({ unit: c.travel.unit }))) p.scale = c.travel.scale;
    } else {
      // a control of the face's own: its travel is typed, the reason kept with it
      if (!x.own) throw new Error(`${file}: '${c.symbol}' has no source link and no "own" reason`);
      if (c.kind === 'toggle') Object.assign(p, { unit: '', min: 0, max: 1, def: c.default ? 1 : 0, kind: 'toggle' });
      else if (c.kind === 'travel') Object.assign(p, { unit: c.travel.unit, min: c.travel.min, max: c.travel.max, def: c.travel.default, ...(c.travel.step === 1 ? { kind: 'integer' } : {}) });
      else throw new Error(`${file}: '${c.symbol}', a ${c.kind} of the face's own, is not a toggle or a travel`);
    }
    return { ...p, ...x };
  });
  return { ...rest, name: pl.name, vendor: pl.vendor, version: pl.version, description: pl.doc, clap: { id: pl.id, features: bag.features }, lv2: { uri: pl.uri, class: bag.class, ...rest.lv2 }, params };
}
