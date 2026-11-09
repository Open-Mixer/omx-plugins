#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * live-faces.mjs -- the words of the two worker-threaded faces, omx-fbs and omx-hrp (spec 7.7): each one's authored
 * descriptor, its declared panel and its LV2 bundle text, written from ONE table below and the kernel's contract.
 *
 * The two faces are hand-written C over omx-dsp's live shells (plugins/<stem>/omx_<k>_lv2.c): the generated faces
 * (lib/omx_lv2_face.c) have no worker. What the generator owns is everything a host reads: the port table the C
 * enum has to equal (tools/test/live-faces-test.mjs holds them equal), the descriptor, the panel.
 *
 *   node tools/live-faces.mjs            write plugins/omx-fbs and plugins/omx-hrp's descriptor, panel file and TTLs
 *   node tools/live-faces.mjs --check    every one fresh, or exit 1 naming the file
 *
 * Reads the kernel travels from the installed omx-contract (the union with omx-dsp's contract directory); nothing
 * kernel-defined is typed here. The report controls are the face's own: four read-only words per slot, which the
 * panel binds to the eq family's band roles with the planter that owns the bands (spec 7.7, "The description").
 */
import { existsSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { controlOf, kernelControls, locateContract, resolveParam, ROOT } from './omx-contract.mjs';

const HERE = dirname(fileURLToPath(import.meta.url));

/** The faces. `slots` is the shell's report size (OMX_HRP_LV2_MAX_BANDS, OMX_FBS_LV2_SLOTS: the kernel's `bands` ceiling);
 * `own` the face's controls that are no kernel control, in the C enum's order after the audio ports; `labels` the
 * kernel choices' words, which the contract does not give. */
export const FACES = {
  'omx-hrp': {
    kernel: 'hrp',
    uri: 'urn:openmixer:hrp',
    title: 'omx hrp',
    doc: 'The harmonic resonance processor: finds the resonances of a mono source from its harmonics and cuts them, reporting each cut as a band.',
    origin: 'hrp',
    slots: { control: 'bands' },
    order: ['bypass', 'family', 'amount', 'bands', 'mode'],
    own: [{ symbol: 'bypass', name: 'Bypass', kind: 'toggle', default: false, doc: 'Pass the audio untouched and analyse nothing.' }],
    labels: { mode: ['Cut', 'Report'] },
    hasClap: false,
  },
  'omx-fbs': {
    kernel: 'fbs',
    uri: 'urn:openmixer:fbs',
    title: 'omx fbs',
    doc: 'The feedback corrector: finds the frequencies that ring in a mono signal and plants a notch on each, reporting each as a band.',
    origin: 'fbs-live',
    slots: { control: 'bands' },
    order: ['mode', 'pool', 'hold', 'bands', 'merge', 'cooldown'],
    own: [],
    labels: { mode: ['Cut', 'Report'], pool: ['Fixed', 'Live'] },
    hasClap: false,
  },
};

const UNIT_TTL = { Hz: 'units:hz', dB: 'units:db', s: 'units:s', cents: 'units:cent' };
const WORKER = 'http://lv2plug.in/ns/ext/worker#';
const kebab = (s) => s.replace(/([a-z0-9])([A-Z])/g, '$1-$2').toLowerCase();
const title = (s) => s[0].toUpperCase() + s.slice(1);
const version = () => /^VERSION := (\S+)$/m.exec(readFileSync(join(ROOT, 'Makefile'), 'utf8'))[1];
const dspVersion = () => /^BuildRequires: \(omx-dsp-devel >= ([0-9.]+) with/m.exec(readFileSync(join(ROOT, 'packaging/omx-plugins.spec'), 'utf8'))[1];

/** The kernel's controls of `stem`, resolved: `{ symbol, name, kind, travel|ids,... , link }` in the face's order. */
function kernelParams(stem, dir) {
  const f = FACES[stem];
  const byName = Object.fromEntries(kernelControls(dir, f.kernel).map((c) => [c.name, c]));
  return f.order.map((name) => {
    const own = f.own.find((o) => o.symbol === name);
    if (own) return { ...own };
    const c = byName[name];
    if (!c) throw new Error(`${stem}: the contract's ${f.kernel} kernel has no control '${name}' (it lists ${Object.keys(byName).join(', ')})`);
    const link = `data/kernels/${f.kernel}.json#${c.ref}`;
    if (c.kind === 'choice') {
      const { ids, default: dflt } = resolveChoice(dir, f.kernel, c);
      const labels = f.labels[name];
      if (!labels || labels.length !== ids.length) throw new Error(`${stem}: choice '${name}' has ${ids.length} ids and ${labels?.length ?? 0} labels`);
      return { symbol: name, name: title(name), kind: 'choice', ids, labels, default: dflt, link };
    }
    const r = resolveParam(dir, f.kernel, { ref: c.ref, symbol: name, ...(c.field ? { field: c.field } : {}) });
    return { symbol: name, name: title(name), kind: 'travel', min: r.min, max: r.max, def: r.def, unit: r.unit, integer: r.kind === 'integer', link };
  });
}

/** The ids of a kernel choice and the id it comes up at, from the render (the contract gives both; the declaration the words). */
function resolveChoice(dir, kernel, c) {
  const item = JSON.parse(readFileSync(join(dir, 'omx-contract.json'), 'utf8')).items?.[c.ref];
  if (!Array.isArray(item?.value)) throw new Error(`${kernel}: cannot read the ids of ${c.ref} from omx-contract.json`);
  const ids = item.value.map(String);
  return { ids, default: String(item.default ?? ids[0]) };
}

/** The slot count: the `bands` control's ceiling. */
const slotsOf = (stem, params) => params.find((p) => p.symbol === FACES[stem].slots.control).max;

/** The report controls of one slot, in the C enum's order: frequency, depth, Q, on. */
const REPORT = [
  { suffix: 'freq', name: 'Frequency', kind: 'travel', travel: { min: 0, max: 20000, step: 0, unit: 'Hz', default: 0, scale: 'log' }, meaning: 'frequency', doc: 'The centre of the band, Hz; 0 while the slot is free.' },
  { suffix: 'depth', name: 'Depth', kind: 'travel', travel: { min: -60, max: 0, step: 0, unit: 'dB', default: 0, scale: 'linear' }, meaning: 'gain', doc: 'The band\'s gain, dB; a cut is negative.' },
  { suffix: 'q', name: 'Q', kind: 'travel', travel: { min: 0, max: 400, step: 0, unit: '', default: 0, scale: 'linear' }, meaning: 'q', doc: 'The band\'s Q.' },
  { suffix: 'on', name: 'On', kind: 'toggle', default: false, doc: '1 while the slot holds a band.' },
];

/** The face's control table, in port order after the audio ports: `{ symbol, ... }` with `report` on a report word. */
export function controlTable(stem, dir) {
  const params = kernelParams(stem, dir);
  const slots = slotsOf(stem, params);
  const table = params.map((p) => ({ ...p, port: 'in' }));
  table.push({ symbol: 'analysing', name: 'Analysing', kind: 'toggle', default: false, port: 'out', doc: 'Set once a worker has answered a pass: a host with no worker gets audio passed untouched, nothing corrected, and this stays 0.' });
  for (let k = 0; k < slots; k++) for (const w of REPORT) table.push({ ...w, symbol: `band${k + 1}_${w.suffix}`, name: `Band ${k + 1} ${w.name}`, port: 'out', report: { slot: k, word: w.suffix } });
  return { table, slots };
}

const nstr = (n) => (Number.isInteger(n) ? String(n) : String(+n.toFixed(6)));

export function descriptorOf(stem, dir) {
  const f = FACES[stem];
  const { table, slots } = controlTable(stem, dir);
  const at = `omx-dsp@${dspVersion()}`;
  const controls = table.map((c, id) => {
    const out = { id, symbol: c.symbol, name: c.name, kind: c.kind };
    if (c.kind === 'travel') {
      out.travel = c.travel ?? { min: c.min, max: c.max, step: c.integer ? 1 : 0, unit: c.unit, default: c.def, scale: c.unit === 'Hz' ? 'log' : 'linear' };
    } else if (c.kind === 'choice') {
      Object.assign(out, { ids: c.ids, labels: c.labels, default: c.default });
    } else out.default = c.default;
    out.group = [c.report ? 'Report' : c.symbol === 'analysing' ? 'Report' : 'Controls'];
    if (c.meaning) out.meaning = c.meaning;
    if (c.doc) out.doc = c.doc;
    if (c.port === 'out') out.readonly = true;
    else out.automatable = true;
    out.source = c.link ? { from: { link: c.link, at } } : { impl: `plugins/${stem}/omx_${f.kernel}_lv2.c#${f.kernel}_connect_port` };
    return out;
  });
  return {
    $schema: 'https://github.com/FreeMixer/omx-contract/raw/v3/schema/omx-contract.schema.json',
    requires: { 'omx-contract-schema': '>= 3.7.0' },
    origin: 'authored',
    plugin: { id: f.uri, format: 'lv2', uri: f.uri, name: f.title, vendor: 'openmixer', version: version(), doc: f.doc, features: ['audio-effect'] },
    controls,
    ports: [
      { id: 0, name: 'in', kind: 'audio', direction: 'in', role: 'main', channels: 1, layout: 'mono' },
      { id: 0, name: 'out', kind: 'audio', direction: 'out', role: 'main', channels: 1, layout: 'mono' },
    ],
    needs: { extensions: { optional: ['lv2:hardRTCapable', `<${WORKER}schedule>`] } },
    latency: { reports: false },
    extensions: { lv2: { 'rdf:type': ['lv2:Plugin'] } },
    source: { impl: `plugins/${stem}/omx_${f.kernel}_lv2.c#lv2_descriptor` },
  };
}

export function panelOf(stem, dir) {
  const f = FACES[stem];
  const { table, slots } = controlTable(stem, dir);
  const word = (w) => table.filter((c) => c.report?.word === w).map((c) => c.symbol);
  const freq = word('freq');
  const own = table.filter((c) => !c.report && c.symbol !== 'analysing').map((c) => c.symbol);
  return {
    family: 'eq',
    roles: { band_freq: freq, band_gain: word('depth'), band_q: word('q'), band_on: word('on') },
    origins: Object.fromEntries(freq.map((s) => [s, f.origin])),
    sections: [
      { key: 'report', label: 'Report', controls: [...table.filter((c) => c.report).map((c) => c.symbol), 'analysing'], graph: 'eq-curve', basis: 'exact' },
      { key: 'controls', label: 'Controls', controls: own },
    ],
  };
}

const ttlStr = (s) => JSON.stringify(s);

export function ttlOf(stem, dir) {
  const f = FACES[stem];
  const { table } = controlTable(stem, dir);
  const v = version().split('.');
  const lines = [];
  const port = (idx, ...rest) => lines.push(`    [\n        ${rest.join(' ;\n        ')}\n    ]`);
  port(0, 'a lv2:AudioPort , lv2:InputPort', `lv2:index 0`, 'lv2:symbol "in"', 'lv2:name "In"');
  port(1, 'a lv2:AudioPort , lv2:OutputPort', `lv2:index 1`, 'lv2:symbol "out"', 'lv2:name "Out"');
  table.forEach((c, i) => {
    const idx = i + 2;
    const parts = [`a lv2:ControlPort , lv2:${c.port === 'in' ? 'InputPort' : 'OutputPort'}`, `lv2:index ${idx}`, `lv2:symbol ${ttlStr(c.symbol)}`, `lv2:name ${ttlStr(c.name)}`];
    if (c.kind === 'toggle') {
      parts.push(`lv2:default ${c.default ? 1 : 0}`, 'lv2:minimum 0', 'lv2:maximum 1', 'lv2:portProperty lv2:toggled');
    } else if (c.kind === 'choice') {
      parts.push(`lv2:default ${c.ids.indexOf(c.default)}`, 'lv2:minimum 0', `lv2:maximum ${c.ids.length - 1}`, 'lv2:portProperty lv2:integer , lv2:enumeration');
      parts.push(`lv2:scalePoint ${c.labels.map((l, k) => `[ rdfs:label ${ttlStr(l)} ; rdf:value ${k} ]`).join(' , ')}`);
    } else {
      const t = c.travel ?? { min: c.min, max: c.max, default: c.def, unit: c.unit };
      parts.push(`lv2:default ${nstr(t.default)}`, `lv2:minimum ${nstr(t.min)}`, `lv2:maximum ${nstr(t.max)}`);
      if (c.integer) parts.push('lv2:portProperty lv2:integer');
      if (UNIT_TTL[t.unit]) parts.push(`units:unit ${UNIT_TTL[t.unit]}`);
      else if (t.unit) throw new Error(`${stem}: unit '${t.unit}' of ${c.symbol} has no LV2 unit in live-faces.mjs`);
      if (t.scale === 'log' || t.unit === 'Hz') parts.push('lv2:portProperty pprops:logarithmic');
    }
    if (c.doc) parts.push(`rdfs:comment ${ttlStr(c.doc)}`);
    lines.push(`    [\n        ${parts.join(' ;\n        ')}\n    ]`);
  });
  const head = `# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
# GENERATED by tools/live-faces.mjs -- DO NOT EDIT BY HAND.
@prefix doap:  <http://usefulinc.com/ns/doap#> .
@prefix foaf:  <http://xmlns.com/foaf/0.1/> .
@prefix lv2:   <http://lv2plug.in/ns/lv2core#> .
@prefix omxres: <https://www.openmixer.org/ns/resource-hints#> .
@prefix pprops: <http://lv2plug.in/ns/ext/port-props#> .
@prefix rdf:   <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .
@prefix rdfs:  <http://www.w3.org/2000/01/rdf-schema#> .
@prefix units: <http://lv2plug.in/ns/extensions/units#> .
@prefix work:  <${WORKER}> .
@prefix xsd:   <http://www.w3.org/2001/XMLSchema#> .

<https://github.com/FreeMixer/omx-plugins>
    a doap:Project ;
    doap:name "omx-plugins" ;
    doap:maintainer [ a foaf:Person ; foaf:name "Pau Aliagas" ; foaf:homepage <https://github.com/linuxnow> ] .

<${f.uri}>
    a lv2:Plugin ;
    doap:name ${ttlStr(f.title)} ;
    doap:license <https://www.gnu.org/licenses/gpl-3.0.html> ;
    lv2:project <https://github.com/FreeMixer/omx-plugins> ;
    lv2:minorVersion ${v[1]} ;
    lv2:microVersion ${v[2]} ;
    rdfs:comment ${ttlStr(f.doc)} ;
    lv2:optionalFeature lv2:hardRTCapable , work:schedule ;
    lv2:extensionData work:interface ;
    omxres:threadsSpawned "0"^^xsd:integer ;
    omxres:streamsFromDisk false ;
    omxres:opensSockets false ;
    lv2:port
`;
  const manifest = `# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
# GENERATED by tools/live-faces.mjs -- DO NOT EDIT BY HAND.
@prefix lv2:  <http://lv2plug.in/ns/lv2core#> .
@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> .

<${f.uri}>
    a lv2:Plugin ;
    lv2:binary <${stem}.so> ;
    rdfs:seeAlso <${stem}.ttl> .
`;
  return { [`${stem}.ttl`]: `${head}${lines.join(' ,\n')} .\n`, 'manifest.ttl': manifest };
}

/** Every file of one face: path (relative to the repository) -> text. */
export function filesOf(stem, dir) {
  const json = (o) => `${JSON.stringify(o, null, 2)}\n`;
  const base = `plugins/${stem}`;
  const out = {
    [`${base}/${stem}.descriptor.json`]: json(descriptorOf(stem, dir)),
    [`${base}/${stem}.panel.json`]: json(panelOf(stem, dir)),
  };
  for (const [name, text] of Object.entries(ttlOf(stem, dir))) out[`${base}/generated/${stem}.lv2/${name}`] = text;
  return out;
}

function main(argv) {
  const check = argv.includes('--check');
  const c = locateContract();
  if (!c.dir) { console.error(`live-faces: ${c.why}`); process.exit(1); }
  let stale = 0;
  for (const stem of Object.keys(FACES)) {
    for (const [rel, text] of Object.entries(filesOf(stem, c.dir))) {
      const file = join(ROOT, rel);
      const have = existsSync(file) ? readFileSync(file, 'utf8') : null;
      if (have === text) continue;
      if (check) { console.error(`live-faces: ${rel} is ${have === null ? 'missing' : 'stale'}: run node tools/live-faces.mjs`); stale = 1; continue; }
      mkdirSync(dirname(file), { recursive: true });
      writeFileSync(file, text);
      console.log(`live-faces: wrote ${rel}`);
    }
  }
  if (check && !stale) console.log('live-faces: every file is fresh');
  process.exit(stale);
}

if (process.argv[1] === fileURLToPath(import.meta.url)) main(process.argv.slice(2));
