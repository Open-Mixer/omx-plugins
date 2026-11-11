#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx-new-plugin.mjs — `omx new plugin`: a plugin over an omx-dsp effect kernel, from the kernel's
 * contract and instance face (docs/new-plugin-from-contract.md).
 *
 *   node tools/omx-new-plugin.mjs --from-contract <kernel> [--stem omx-<x>] [--root <tree>]
 *                                 [--plan-json <file>] [--no-run]
 *   node tools/omx-new-plugin.mjs --from-contract <k1>,<k2>,... --stem omx-<x> [...]
 *
 * Several kernels draft a composite (`binding: chain`):
 * one element per kernel in the order given, each drafted as a single plugin's parameters are, and
 * each refused, naming the omx-dsp work, when its kernel has no instance face of the generated shape.
 *
 * The declaration plugins/<stem>/<stem>.decl.json is the input and the output; there is no
 * answers file.
 *
 * 1. Checks, writing nothing on a refusal: the kernel is in the installed omx-contract (read through its
 *    JSON render, tools/omx-contract.mjs); omx-dsp has its instance face in the shape the generated
 *    binding calls, and every resolve() argument names one contract control (tools/instance-face.mjs).
 * 2. No declaration yet: DRAFTS one, a parameter per resolve() argument in its order, each by
 *    reference, every design choice marked REVIEW (the description, the LV2 class, the CLAP kind, a
 *    set's labels the contract does not give), and stops, naming each mark.
 * 3. A declaration with no REVIEW mark: validates it (the schema, the derived fields, identity
 *    unique among the plugins, every reference resolving, every resolve() argument bound, the panel
 *    and console naming declared parameters), fills the derived fields, and runs tools/gen.mjs:
 *    every other file of the folder and the shared files are generated.
 * 4. Prints the COMMIT PLAN in the recipe's layer order (tools/commit-plan-check.mjs holds a range
 *    to it), then runs the plugin's `make test` and the completeness test for it.
 *
 * Exit 0 drafted, or written; 1 refused (each reason on its own line), or REVIEW marks left; 2 usage.
 */
import { spawnSync } from 'node:child_process';
import { existsSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { dirname, join, relative, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { bindFace, instanceHeader, parseFace } from './instance-face.mjs';
import { controlOf, items, kernelControls, kernelExists, locateContract, resolveParam } from './omx-contract.mjs';
import { ROOT, checkPlugin, consoleErrors, gapLines, layerOfPath, loadRecipe, loadSchema, panelErrors, pluginFacts, pluginStems, validate } from './plugin-recipe.mjs';
import { chainBindings, defaultPanel, instanceExtras, loadDecl, prepareDecl, scaleOf } from './gen.mjs';
import { contractFingerprint, contractOf, registerFirstContract } from './param-contract.mjs';
import { describedOf, descriptorFile } from './descriptor.mjs';
import { modgui } from './panel-file.mjs';
import { pinOfTtl } from './port-hints.mjs';
import { omxdspInclude } from './template.mjs';
import { expandVariant } from './variants.mjs';

const REVIEW = 'REVIEW';

// ---- the derived fields -----------------------------------------------------------------------

/** How each pointer the recipe declares as derived is computed. A pointer the recipe derives and
 * this table lacks throws: the recipe and the wizard cannot drift. */
const DERIVERS = {
  '/name': (d) => `omx ${d.stem.replace(/^omx-/, '')}`,
  '/vendor': (_d, ctx) => ctx.schema.$defs.plugin.properties.vendor.const,
  '/url': (_d, ctx) => ctx.schema.$defs.plugin.properties.url.const,
  '/version': (_d, ctx) => ctx.version,
  '/clap/id': (d) => `org.openmixer.${d.stem.replace(/^omx-/, '')}`,
  '/lv2/uri': (d) => `urn:openmixer:${d.stem.replace(/^omx-/, '')}`,
  '/$comment': (d) => (d.binding === 'chain'
    ? `THE declaration of ${d.stem}, the folder's one hand-written file. Every other file of the folder is generated from it by tools/gen.mjs. A chain of omx-dsp instance faces (${d.chain.map((el) => el.kernel).join(', ')}): each element's parameters are by reference to omx-contract's kernel of that element and bind by name to its face. The parameter list is generated from the chain and is append-only.`
    : `THE declaration of ${d.stem}, the folder's one hand-written file. Every other file of the folder is generated from it by tools/gen.mjs. Its parameters are by reference to omx-contract's ${d.kernel} kernel and bind by name to omx-dsp's instance face. Parameter order is append-only.`),
};

const getPtr = (obj, ptr) => ptr.split('/').slice(1).reduce((o, k) => o?.[k], obj);
function setPtr(obj, ptr, v) {
  const keys = ptr.split('/').slice(1);
  let o = obj;
  for (const k of keys.slice(0, -1)) o = o[k] ??= {};
  o[keys.at(-1)] = v;
}

/** The tree's release version, from the file and variable the recipe names. */
function treeVersion(root, recipe) {
  const { file, var: v } = recipe.tree.versionFrom;
  const m = readFileSync(join(root, file), 'utf8').match(new RegExp(`^${v}\\s*:?=\\s*(\\S+)`, 'm'));
  if (!m) throw new Error(`${file}: no ${v}`);
  return m[1];
}

/** The declaration in the schema's key order, at every depth (the form every declaration is written in). */
export function canonical(schema, decl) {
  const deref = (node) => (node?.$ref ? deref(node.$ref.replace(/^#\//, '').split('/').reduce((n, k) => n?.[k], schema)) : node);
  const order = (node, value) => {
    const s = deref(node);
    if (Array.isArray(value)) return value.map((v) => order(s?.items, v));
    if (value === null || typeof value !== 'object' || !s?.properties) return value;
    const keys = [...Object.keys(s.properties).filter((k) => k in value), ...Object.keys(value).filter((k) => !(k in s.properties))];
    return Object.fromEntries(keys.map((k) => [k, order(s.properties[k], value[k])]));
  };
  return order(schema.$defs.plugin, decl);
}

/** Every REVIEW mark left in a declaration, as JSON pointers. */
export function reviewMarks(value, ptr = '') {
  if (typeof value === 'string') return value.startsWith(REVIEW) ? [ptr || '/'] : [];
  if (Array.isArray(value)) return value.flatMap((v, i) => reviewMarks(v, `${ptr}/${i}`));
  if (value && typeof value === 'object') return Object.entries(value).flatMap(([k, v]) => reviewMarks(v, `${ptr}/${k}`));
  return [];
}

// ---- step 1: the kernel's contract and face ----------------------------------------------------

/** The kernel's contract and instance face, or the refusals naming the work owed elsewhere. */
export function kernelSources(kernel, { root = ROOT } = {}) {
  const refusals = [];
  if (!/^[a-z][a-z0-9_]*$/.test(kernel ?? '')) return { refusals: [{ field: '--from-contract', reason: `'${kernel}' is not a kernel name` }] };
  const where = locateContract(root);
  if (!where.dir) refusals.push({ field: 'omx-contract', reason: where.why });
  else if (!kernelExists(where.dir, kernel))
    refusals.push({ field: 'omx-contract', reason: `omx-contract ${where.version} has no data/kernels/${kernel}.json: draft it there first (tools/new-kernel.mjs ${kernel}), release it, then raise the omx-contract floor in packaging/omx-plugins.spec and debian/control` });
  const inc = omxdspInclude();
  const header = instanceHeader(inc, kernel);
  let face;
  if (!header) refusals.push({ field: 'omx-dsp', reason: `omx-dsp has no <omxdsp/fx/omx_${kernel}_instance.h>${inc ? '' : ' (no omx-dsp found: pkg-config omxdsp, or OMXDSP_INCLUDE)'}: the instance face is omx-dsp's work first` });
  else {
    face = parseFace(readFileSync(header, 'utf8'), kernel);
    if (face.error) refusals.push({ field: 'omx-dsp', reason: `<omxdsp/fx/omx_${kernel}_instance.h>: ${face.error}; a plugin over it keeps a hand-written binding, or omx-dsp gives it a face of the generated shape` });
  }
  return { refusals, contract: where.dir, release: where.version, face: face?.error ? undefined : face, header };
}

// ---- step 2: the draft -------------------------------------------------------------------------

/** A control's words for a host, its unit suffix dropped: attackTimeMs -> "Attack Time". */
export function labelOf(name) {
  const words = name.replace(/([a-z0-9])([A-Z])/g, '$1 $2').split(' ');
  if (words.length > 1 && /^(Db|Ms|Hz|Pct|Oct|S)$/.test(words.at(-1))) words.pop();
  return words.map((w) => w[0].toUpperCase() + w.slice(1)).join(' ');
}

/** The draft declaration of the plugin over `kernel`: everything the contract and the face say,
 * every design choice marked REVIEW. */
export function draftDeclaration(kernel, src, { root = ROOT, recipe = loadRecipe(root), stem = `omx-${kernel.replace(/_/g, '-')}` } = {}) {
  const controls = kernelControls(src.contract, kernel);
  const asParams = controls.map((c) => ({ symbol: c.name, ref: c.ref, ...(c.field ? { field: c.field } : {}) }));
  const bound = bindFace(src.face, asParams, controls, { strict: false });
  if (bound.errors.length) return { refusals: bound.errors.map((reason) => ({ field: 'omx-dsp', reason })) };
  let variantsOf;
  const params = bound.binding.flatMap((b) => {
    const p = { symbol: b.param.symbol, name: labelOf(b.param.symbol), ref: b.param.ref, ...(b.param.field ? { field: b.param.field } : {}) };
    const e = items(src.contract)[p.ref];
    if (e.kind === 'set' && !e.value.every((x) => typeof x === 'number') && !e.labels) p.values = e.value.map((id) => `${REVIEW}: the label of ${id}`);
    if (!b.extent) return [p];
    // a per-band array whose count is a sheet of variants (EQ_BAND_COUNTS): one declaration for the
    // variants (tools/variants.mjs), the parameter written once and repeated per band
    const count = controls.find((c) => c.name === b.control)?.count;
    if (count && items(src.contract)[count]?.kind === 'sheet') {
      variantsOf = count;
      return [{ ...p, perBand: true }];
    }
    // a per-band array: one parameter per band, the count the contract renders as the extent, or,
    // where the extent is the face's own macro (OMX_DYNEQ_INSTANCE_BANDS), the count the control
    // declares; the generated binding asserts the two are equal
    const n = items(src.contract)[b.extent.replace(/^OMX_/, '')]?.value ?? items(src.contract)[count]?.value;
    if (!Number.isInteger(n) || n < 1) throw new Error(`resolve argument '${b.arg}[${b.extent}]': omx-contract renders no count ${b.extent.replace(/^OMX_/, '')}${count ? ` and its control's count ${count} is no number` : ''}`);
    const w = String(n).length;
    return Array.from({ length: n }, (_x, k) => ({ ...p, symbol: `${p.symbol}${String(k + 1).padStart(w, '0')}`, name: `${p.name} ${k + 1}` }));
  });
  const schema = loadSchema(root, recipe);
  const decl = {
    stem, kernel, kernels: [kernel],
    description: `${REVIEW}: one plain sentence a host shows, what the effect does for you`,
    clap: { features: ['audio-effect', `${REVIEW}: the CLAP kind (delay, distortion, modulation, ...)`, 'stereo'] },
    lv2: { class: `${REVIEW}: the LV2 class (lv2:DelayPlugin, lv2:ModulatorPlugin, ...)` },
    binding: 'instance',
    params,
  };
  if (variantsOf) {
    // every entry of the count sheet drafted; a person keeps the ones that ship as plugins
    decl.variants = {
      count: variantsOf,
      of: Object.keys(items(src.contract)[variantsOf].value).map((k) => ({ stem: `${REVIEW}: the stem of the ${k} variant, or drop it`, count: k })),
    };
  }
  fillDerived(decl, { schema, version: treeVersion(root, recipe) }, recipe);
  // every plugin declares its panel (the MOD GUI: one section of every parameter until a person
  // groups them) and its console block (the group the console puts it in: a person's choice; `strips` only
  // when the plugin confines itself to some strip kinds)
  decl.panel = draftPanel(schema, decl);
  decl.console = { placement: { group: `${REVIEW}: walk, tail or plugins (the console's channel layout)` } };
  return { refusals: [], decl: canonical(schema, decl), unbound: controls.filter((c) => !bound.binding.some((b) => b.param.symbol === c.name)).map((c) => c.name) };
}

/** gen.mjs's default panel, and for a family other than none a REVIEW mark where its roles go: which
 * parameter plays each of the family's roles is a person's reading. */
function draftPanel(schema, decl) {
  const panel = defaultPanel(decl);
  const roles = schema.$defs.familyRoles.properties[panel.family]?.enum ?? [];
  if (roles.length) panel.roles = { [roles[0]]: `${REVIEW}: the parameter playing each role of the ${panel.family} family the panel draws (${roles.join(', ')}), one per role` };
  return panel;
}

function fillDerived(decl, ctx, recipe, refuse) {
  for (const ptr of Object.keys(recipe.derived)) {
    const derive = DERIVERS[ptr];
    if (!derive) throw new Error(`${recipe.item} recipe derives ${ptr}, which the wizard cannot compute`);
    const want = derive(decl, ctx);
    const given = getPtr(decl, ptr);
    if (given === undefined || ptr === '/$comment') setPtr(decl, ptr, want);
    else if (given !== want && refuse) refuse(ptr, `'${given}' is derived: it is '${want}'`);
  }
}

// ---- step 3: the plan: validate everything, write nothing ----------------------------------------

export async function planPlugin(declIn, src, { root = ROOT, recipe = loadRecipe(root) } = {}) {
  const schema = loadSchema(root, recipe);
  const refusals = [];
  const refuse = (field, reason) => refusals.push({ field, reason });
  refuse.count = () => refusals.length;
  const decl = structuredClone(declIn);
  const marks = reviewMarks(decl);
  for (const m of marks) refuse(m, 'still marked REVIEW: a person settles it');
  if (marks.length) return { ok: false, refusals };
  if (decl.binding !== 'instance') refuse('/binding', 'is not "instance": this wizard writes plugins generated from omx-dsp\'s instance face');
  fillDerived(decl, { schema, version: treeVersion(root, recipe) }, recipe, refuse);
  decl.kernels ??= [decl.face ?? decl.kernel];
  // a variant (tools/variants.mjs) names its files `kernel` and binds its base's face
  const face = decl.face ?? decl.kernel;
  for (const e of validate(schema, schema, decl)) refuse(e.split(':')[0], `schema: ${e.slice(e.indexOf(':') + 2)}`);
  if (refusals.length) return { ok: false, refusals };

  // unique among the other plugins of the tree; a folder is one kernel's
  const here = pluginStems(root).includes(decl.stem) ? pluginFacts(root, decl.stem) : undefined;
  if (here && here.kernel !== decl.kernel) refuse('/stem', `plugins/${decl.stem} is the ${here.kernel} plugin's folder`);
  for (const s of pluginStems(root).filter((x) => x !== decl.stem)) {
    const o = pluginFacts(root, s);
    if (o.clapId === decl.clap.id) refuse('/clap/id', `'${decl.clap.id}' is ${s}'s`);
    if (o.uri === decl.lv2.uri) refuse('/lv2/uri', `'${decl.lv2.uri}' is ${s}'s`);
  }
  if (decl.kernels.length !== 1 || decl.kernels[0] !== face) refuse('/kernels', `a plugin generated from one instance face reads one kernel, '${face}'`);

  // every parameter by reference, resolving in the kernel's file, and bound to the face by name
  const symbols = new Set();
  const resolved = [];
  const controls = [];
  decl.params.forEach((p, i) => {
    if (symbols.has(p.symbol)) refuse(`/params/${i}/symbol`, `'${p.symbol}' declared twice`);
    symbols.add(p.symbol);
    if (p.ref === undefined) return refuse(`/params/${i}`, `'${p.symbol}' has no ref: every parameter of a generated plugin is a contract control`);
    const isSet = items(src.contract)[p.ref]?.kind === 'set';
    const typed = ['min', 'max', 'def', 'unit', 'kind'].filter((k) => k in p && !(isSet && k === 'unit'));
    if (typed.length) refuse(`/params/${i}`, `'${p.symbol}' retypes ${typed.join(', ')}: omx-contract holds them`);
    try {
      resolved.push({ ...p, ...resolveParam(src.contract, face, p) });
      controls.push(controlOf(src.contract, face, p));
    } catch (e) {
      refuse(`/params/${i}/ref`, e.message);
    }
  });
  if (refusals.length) return { ok: false, refusals };
  // a control the plugin does not expose is passed at its "fixed" value or held at its default, as tools/gen.mjs binds it
  const extraErrors = [];
  const extra = instanceExtras({ ...decl, tree: root, controls }, extraErrors);
  for (const e of extraErrors) refuse('/fixed', e);
  const bound = bindFace(src.face, [...decl.params, ...extra.map((f) => f.p)], [...controls, ...extra.map((f) => f.c)]);
  for (const e of bound.errors) refuse('/params', e);

  await checkPanelConsole(schema, decl, symbols, resolved, refuse, root);

  return { ok: refusals.length === 0, refusals, decl: canonical(schema, decl), resolved, binding: bound.binding, renames: bound.renames, release: src.release };
}

/** The panel speaks the console's family and role vocabulary, the panel and the console name declared
 * parameters, and the panel draws every one of them as the MOD GUI generator draws it (selectors included). */
async function checkPanelConsole(schema, decl, symbols, resolved, refuse, root) {
  // the panel's family and roles are the console's; the panel and the console name declared parameters
  for (const e of [...panelErrors(schema, decl, symbols), ...consoleErrors(decl, symbols)]) refuse(e.split(':')[0], e.slice(e.indexOf(':') + 2));

  // the panel, drawn as omx-panel draws the MOD GUI (every parameter on it, selectors included)
  if (decl.panel && !refuse.count()) {
    try {
      const r = modgui({ ...decl, params: resolved, dir: join(root, 'plugins', decl.stem), tree: root }, '--emit-stdout');
      if (r.status !== 0) throw new Error(r.stderr.trim().split('\n')[0]);
      const drawn = new Set(decl.panel.sections.flatMap((x) => x.controls));
      const undrawn = resolved.filter((p) => !drawn.has(p.symbol)).map((p) => p.symbol);
      if (undrawn.length) throw new Error(`the MOD GUI draws every parameter (omx-panel's modgui-test.sh), and the panel leaves out ${undrawn.join(', ')}`);
    } catch (e) {
      refuse('/panel', `${e.message}; put every parameter on the panel, or leave the panel out and the MOD GUI draws every parameter in one section`);
    }
  }
}

// ---- a composite: --from-contract <k1>,<k2>,... -------

/** Each element's kernel sources (kernelSources), and every refusal among them, each naming its kernel. */
export function chainSources(kernels, { root = ROOT } = {}) {
  const srcs = kernels.map((k) => kernelSources(k, { root }));
  const refusals = srcs.flatMap((src, i) => src.refusals.map((r) => ({ ...r, field: `${kernels[i]}: ${r.field}` })));
  if (kernels.length < 2) refusals.push({ field: '--from-contract', reason: 'a chain runs two or more kernels' });
  return { refusals, srcs, kernels, contract: srcs[0]?.contract, release: srcs[0]?.release };
}

const cap = (w) => `${w[0].toUpperCase()}${w.slice(1)}`;

/**
 * The draft declaration of a chain over `kernels`, in the order given: one element per kernel, its
 * parameters drafted as a single plugin's are (one per resolve() argument, by reference, a set's
 * labels the contract does not give marked REVIEW), each symbol carrying the element's id in front;
 * a per-band control repeated for each band of the count sheet entry `bands` (the stem's short name
 * when the sheet has it, else REVIEW). Each element's switch default and every design choice of the
 * plugin are marked REVIEW.
 */
export function draftChain(kernels, chainSrc, { root = ROOT, recipe = loadRecipe(root), stem } = {}) {
  const short = stem.replace(/^omx-/, '');
  const refusals = [];
  const notes = [];
  const seen = {};
  const chain = kernels.map((kernel, i) => {
    const src = chainSrc.srcs[i];
    const base = kernel.replace(/_/g, '');
    seen[base] = (seen[base] ?? 0) + 1;
    const id = kernels.filter((k) => k.replace(/_/g, '') === base).length > 1 ? `${base}${seen[base]}` : base;
    const name = labelOf(id);
    const controls = kernelControls(src.contract, kernel);
    const asParams = controls.map((c) => ({ symbol: c.name, ref: c.ref, ...(c.field ? { field: c.field } : {}) }));
    const bound = bindFace(src.face, asParams, controls, { strict: false });
    for (const reason of bound.errors) refusals.push({ field: `${kernel}: omx-dsp`, reason });
    if (bound.errors.length) return undefined;
    const el = { id, kernel, name, on: `${REVIEW}: the switch's default, true or false`, params: [] };
    const one = (b) => {
      const p = { symbol: `${id}${cap(b.param.symbol)}`, name: `${name} ${labelOf(b.param.symbol)}`, ref: b.param.ref, ...(b.param.field ? { field: b.param.field } : {}) };
      const e = items(src.contract)[p.ref];
      if (e.kind === 'set' && !e.value.every((x) => typeof x === 'number') && !e.labels) p.values = e.value.map((v) => `${REVIEW}: the label of ${v}`);
      return p;
    };
    const bands = bound.binding.filter((b) => b.extent);
    el.params.push(...bound.binding.filter((b) => !b.extent).map(one));
    if (bands.length) {
      const sheet = controls.find((c) => c.count)?.count;
      const counts = sheet ? items(src.contract)[sheet]?.value ?? {} : {};
      const key = short in counts ? short : Object.keys(counts)[0];
      const n = counts[key]?.max;
      if (!Number.isInteger(n)) {
        refusals.push({ field: `${kernel}: omx-contract`, reason: `the per-band controls of ${kernel} name no count sheet with a max per entry` });
        return undefined;
      }
      el.bands = short in counts ? key : `${REVIEW}: the key of ${sheet} whose max is the band count (${Object.keys(counts).join(', ')}); drafted with ${key}'s ${n}`;
      for (let k = 1; k <= n; k++)
        for (const b of bands) {
          const p = one(b);
          el.params.push({ ...p, symbol: `${id}${k}${cap(b.param.symbol)}`, name: `${name} ${k} ${labelOf(b.param.symbol)}` });
        }
    }
    if (src.face.keyed) notes.push(`element '${id}': its face takes a key; name it in "key" (with a "sidechain"), or pass its key-source control in "fixed" and leave it out of "params"`);
    return el;
  });
  if (refusals.length) return { refusals };
  const schema = loadSchema(root, recipe);
  const decl = {
    stem, kernel: short.replace(/-/g, '_'),
    description: `${REVIEW}: one plain sentence a host shows, what the effect does for you`,
    clap: { features: ['audio-effect', `${REVIEW}: the CLAP kind (mixing, dynamics, ...)`, 'stereo'] },
    lv2: { class: `${REVIEW}: the LV2 class (lv2:MixerPlugin, lv2:DynamicsPlugin, ...)` },
    binding: 'chain',
    chain,
  };
  fillDerived(decl, { schema, version: treeVersion(root, recipe) }, recipe);
  // as a single plugin's: the panel one section per element, the console placement a person's choice
  decl.panel = draftPanel(schema, decl);
  decl.console = { placement: { group: `${REVIEW}: walk, tail or plugins (the console's channel layout)` } };
  return { refusals: [], decl: canonical(schema, decl), notes };
}

/** A chain's plan: everything planPlugin checks, each element bound to its own face by tools/gen.mjs's
 * own reading of the declaration (prepareDecl, chainBindings), writing nothing. */
export async function planChain(declIn, chainSrc, { root = ROOT, recipe = loadRecipe(root) } = {}) {
  const schema = loadSchema(root, recipe);
  const refusals = [];
  const refuse = (field, reason) => refusals.push({ field, reason });
  refuse.count = () => refusals.length;
  const decl = structuredClone(declIn);
  const marks = reviewMarks(decl);
  for (const m of marks) refuse(m, 'still marked REVIEW: a person settles it');
  if (marks.length) return { ok: false, refusals };
  if (decl.binding !== 'chain') refuse('/binding', 'is not "chain": several kernels make a chain');
  fillDerived(decl, { schema, version: treeVersion(root, recipe) }, recipe, refuse);
  for (const e of validate(schema, schema, decl)) refuse(e.split(':')[0], `schema: ${e.slice(e.indexOf(':') + 2)}`);
  if (refusals.length) return { ok: false, refusals };
  const kernels = (decl.chain ?? []).map((el) => el.kernel);
  if (JSON.stringify(kernels) !== JSON.stringify(chainSrc.kernels)) refuse('/chain', `runs ${kernels.join(', ')}, and --from-contract names ${chainSrc.kernels.join(', ')}`);
  const here = pluginStems(root).includes(decl.stem) ? pluginFacts(root, decl.stem) : undefined;
  if (here && here.kernel !== decl.kernel) refuse('/stem', `plugins/${decl.stem} is the ${here.kernel} plugin's folder`);
  for (const st of pluginStems(root).filter((x) => x !== decl.stem)) {
    const o = pluginFacts(root, st);
    if (o.clapId === decl.clap.id) refuse('/clap/id', `'${decl.clap.id}' is ${st}'s`);
    if (o.uri === decl.lv2.uri) refuse('/lv2/uri', `'${decl.lv2.uri}' is ${st}'s`);
  }
  if (refusals.length) return { ok: false, refusals };
  let d;
  let bound = [];
  try {
    d = prepareDecl(join(root, 'plugins', decl.stem, `${decl.stem}.decl.json`), structuredClone(decl), decl.stem);
    bound = chainBindings(d);
  } catch (e) {
    refuse('/chain', e.message);
    return { ok: false, refusals };
  }
  await checkPanelConsole(schema, decl, new Set(d.params.map((p) => p.symbol)), d.params, refuse, root);
  const renames = bound.flatMap((b) => b.renames);
  return { ok: refusals.length === 0, refusals, decl: canonical(schema, decl), resolved: d.params, binding: bound.flatMap((b) => b.binding), renames, release: chainSrc.release };
}

// ---- writing: the declaration, then every generated file ---------------------------------------

/** `decl` with its `portHints` pin, placed where the schema puts it: after the parameters (and the
 * sidechain), every other key where the person wrote it. */
export function withPortHints(decl, pin) {
  const keys = Object.keys(decl).filter((k) => k !== 'portHints');
  // after the parameters, or a chain's elements, and whatever of `order`, `key`, `sidechain` follows them
  const after = keys.filter((k) => ['params', 'chain', 'order', 'key', 'sidechain'].includes(k)).at(-1) ?? keys.at(-1);
  return Object.fromEntries(keys.flatMap((k) => (k === after ? [[k, decl[k]], ['portHints', pin]] : [[k, decl[k]]])));
}

/** A declaration with no `portHints` yet gets the pin of the TTL just generated (tools/port-hints.mjs):
 * a new face pins the hints it carries, and `make hints` holds every later TTL to them. */
function pinHints(root, declPath, decl, stems, perBand = []) {
  if (decl.portHints) return;
  const pins = stems.map((s) => pinOfTtl(readFileSync(join(root, 'plugins', s, 'generated', `${s}.lv2`, `${s}.ttl`), 'utf8'), perBand));
  for (const [i, p] of pins.entries()) if (JSON.stringify(p) !== JSON.stringify(pins[0])) throw new Error(`port-hints: ${stems[i]} pins other hints than ${stems[0]}`);
  writeFileSync(declPath, `${JSON.stringify(withPortHints(decl, pins[0]), null, 2)}\n`);
  // the pin moved the hint view gen.mjs assembles: refresh it (no other file reads the pin)
  const r = run('node', [join(root, 'tools', 'gen.mjs')], { cwd: root });
  if (r.code !== 0) throw new Error(`tools/gen.mjs: ${r.out.trim()}`);
}

function run(cmd, args, opts) {
  const r = spawnSync(cmd, args, { encoding: 'utf8', ...opts });
  return { code: r.status, out: `${r.stdout ?? ''}${r.stderr ?? ''}` };
}

const CONTRACT_VERSIONS = 'tools/param-contract-versions.json';

/** A plugin the tree has never published takes contract version 1: its fingerprint, from the declaration just written,
 * goes into tools/param-contract-versions.json before tools/gen.mjs reads its version there. Returns the file when it
 * was written; a plugin already listed is not touched, and a change to its contract is appended by hand. */
function registerContract(root, stem) {
  const fingerprint = contractFingerprint(contractOf(loadDecl(join(root, 'plugins', stem)), scaleOf));
  return registerFirstContract(root, stem, fingerprint) ? [CONTRACT_VERSIONS] : [];
}

/** Write the declaration and run the generators; returns every file written, relative to the tree. */
export function writePlugin(plan, { root = ROOT } = {}) {
  const d = plan.decl;
  const rel = `plugins/${d.stem}/${d.stem}.decl.json`;
  const text = `${JSON.stringify(d, null, 2)}\n`;
  // the declaration is always in the plan: it is the one file a person wrote
  const written = [rel];
  if (!existsSync(join(root, rel)) || readFileSync(join(root, rel), 'utf8') !== text) {
    mkdirSync(dirname(join(root, rel)), { recursive: true });
    writeFileSync(join(root, rel), text);
  }
  written.push(...registerContract(root, d.stem));
  // a generated plugin always has its MOD GUI: its declared panel, or gen.mjs's one section of every parameter
  const tools = [[join(root, 'tools', 'gen.mjs')], ...(d.panel || d.binding === 'instance' || d.binding === 'chain' ? [[join(root, 'tools', 'panel-file.mjs'), '--modgui', join(root, 'plugins', d.stem)]] : [])];
  const generate = () => {
    for (const args of tools) {
      const r = run('node', args, { cwd: root });
      if (r.code !== 0) throw new Error(`${relative(root, args[0])}: ${r.out.trim()}`);
      for (const m of r.out.matchAll(/wrote (\S+)/g)) written.push(relative(root, resolve(root, m[1])));
    }
  };
  generate();
  let decl = d;
  if (d.binding === 'instance' && !d.variants) {
    // described, as every single-kernel plugin of ours is: the descriptor rendered from the settled
    // declaration, and the declaration's extras beside it; then generated again from the two
    const dir = join(root, 'plugins', d.stem);
    const { descriptor, extras } = describedOf(d, loadDecl(dir));
    extras.$comment = `The extras of ${d.stem}'s declaration: what its descriptor, ${descriptorFile(d.stem)}, has no word for (docs/declaration-as-descriptor.md). The two are the folder's hand-written files; tools/gen.mjs generates the rest and writes the travels the descriptor's links resolve to. Its parameters are by reference to omx-contract's ${d.kernel} kernel and bind by name to omx-dsp's instance face. Control order, the descriptor's, is append-only.`;
    writeFileSync(join(dir, descriptorFile(d.stem)), `${JSON.stringify(descriptor, null, 2)}\n`);
    decl = { $comment: extras.$comment, ...extras };
    writeFileSync(join(root, rel), `${JSON.stringify(decl, null, 2)}\n`);
    written.push(`plugins/${d.stem}/${descriptorFile(d.stem)}`);
    generate();
  }
  pinHints(root, join(root, rel), decl, [d.stem]);
  return [...new Set(written)];
}

// ---- the commit plan -------------------------------------------------------------------------

/** One commit per layer that has files, in the recipe's layer order. */
export function commitPlan(recipe, plan, files) {
  const facts = { name: plan.decl.name, kernel: plan.decl.kernel, contractRelease: plan.release };
  const byLayer = new Map(recipe.layers.map((l) => [l.id, []]));
  const unmapped = [];
  for (const f of files) {
    const layer = layerOfPath(recipe, f);
    if (layer) byLayer.get(layer).push(f);
    else unmapped.push(f);
  }
  if (unmapped.length) throw new Error(`the wizard wrote files no recipe artifact maps to a layer: ${unmapped.join(', ')}`);
  const fill = (t) => t.replace(/\{(\w+)\}/g, (m, k) => facts[k] ?? m);
  return recipe.layers.filter((l) => byLayer.get(l.id).length).map((l) => ({ layer: l.id, files: [...new Set(byLayer.get(l.id))].sort(), message: fill(l.message) }));
}

// ---- the command ---------------------------------------------------------------------------

function usage(msg) {
  if (msg) process.stderr.write(`omx-new-plugin: ${msg}\n`);
  process.stderr.write('usage: omx-new-plugin.mjs --from-contract <kernel>[,<kernel>...] [--stem omx-<x>] [--root <tree>] [--plan-json <file>] [--no-run]\n');
  process.exit(2);
}

function refused(refusals) {
  for (const r of refusals) console.error(`REFUSED ${r.field}: ${r.reason}`);
  process.exit(1);
}

async function main(argv) {
  const opt = (name) => {
    const i = argv.indexOf(name);
    return i >= 0 ? argv[i + 1] : undefined;
  };
  const kernel = opt('--from-contract');
  if (!kernel) usage('--from-contract <kernel> is required');
  const root = resolve(opt('--root') ?? ROOT);
  const recipe = loadRecipe(root);
  const stem = opt('--stem') ?? `omx-${kernel.replace(/_/g, '-').replace(/,/g, '-')}`;
  const declPath = join(root, 'plugins', stem, `${stem}.decl.json`);
  if (kernel.includes(',')) return chainMain(kernel.split(','), { root, recipe, stem, declPath, argv, opt });

  const src = kernelSources(kernel, { root });
  if (src.refusals.length) refused(src.refusals);
  console.log(`omx-contract ${locateContract(root).version}: data/kernels/${kernel}.json; omx-dsp: <omxdsp/fx/omx_${kernel}_instance.h> (${src.header})`);

  if (!existsSync(declPath)) {
    const draft = draftDeclaration(kernel, src, { root, recipe, stem });
    if (draft.refusals.length) refused(draft.refusals);
    mkdirSync(dirname(declPath), { recursive: true });
    writeFileSync(declPath, `${JSON.stringify(draft.decl, null, 2)}\n`);
    console.log(`drafted ${relative(root, declPath)}: ${draft.decl.params.length} parameters, one per resolve() argument`);
    if (draft.unbound.length) console.log(`note: contract controls the face takes no argument for, left out: ${draft.unbound.join(', ')}`);
    console.log('settle each mark, then run the same command again:');
    for (const m of reviewMarks(draft.decl)) console.log(`  [ ] ${m}`);
    console.log('  [ ] review each parameter\'s name; group the "panel" into sections (the one panel the MOD GUI and the console draw), add the "console" card and chip, "summary" or "manual" where wanted');
    return;
  }

  const raw = JSON.parse(readFileSync(declPath, 'utf8'));
  if (raw.variants) return variantsMain(raw, src, { root, recipe, declPath, argv, opt });
  const plan = await planPlugin(raw, src, { root, recipe });
  if (!plan.ok) refused(plan.refusals);
  for (const b of plan.renames) console.log(`note: resolve argument '${b.arg}' binds '${b.param.symbol}' by its words; omx-dsp's name conformance owes the rename to '${b.rename}'`);
  return finish(plan, { root, recipe, stem, argv, opt });
}

/** Write a settled plan, print its commit plan, then run the plugin's make test and completeness. */
async function finish(plan, { root, recipe, stem, argv, opt }) {
  const files = writePlugin(plan, { root });
  console.log(`\nwritten (${files.length}):`);
  for (const f of files) console.log(`  ${f}`);
  const commits = commitPlan(recipe, plan, files);
  console.log('\nCOMMIT PLAN (one concern per commit, in layer order; tools/commit-plan-check.mjs holds the range to it):');
  commits.forEach((c, i) => {
    console.log(`  ${i + 1}. [${c.layer}] ${c.message}`);
    for (const f of c.files) console.log(`       ${f}`);
  });
  if (opt('--plan-json')) writeFileSync(opt('--plan-json'), `${JSON.stringify(commits, null, 2)}\n`);
  if (argv.includes('--no-run')) return;

  const make = run('make', ['-C', join(root, 'plugins', stem), 'test']);
  const red = make.out.split('\n').filter((l) => /^FAIL /.test(l));
  console.log(`\nmake -C plugins/${stem} test: ${make.code === 0 ? 'green' : `RED\n  ${(red.length ? red : make.out.trim().split('\n').slice(-3)).join('\n  ')}`}`);
  const gaps = gapLines(await checkPlugin(root, recipe, stem));
  console.log(`completeness for ${stem}: ${gaps.length ? `${gaps.length} owed entries left` : 'complete'}`);
  for (const g of gaps) console.log(`  [ ] ${g}`);
  if (make.code !== 0 || gaps.length) process.exit(1);
}

/** `--from-contract <k1>,<k2>,...`: draft the chain, or plan and write a settled one. */
async function chainMain(kernels, { root, recipe, stem, declPath, argv, opt }) {
  const src = chainSources(kernels, { root });
  if (src.refusals.length) refused(src.refusals);
  console.log(`omx-contract ${locateContract(root).version}: ${kernels.map((k) => `data/kernels/${k}.json`).join(', ')}; omx-dsp: ${src.srcs.map((x) => x.header).join(', ')}`);
  if (!existsSync(declPath)) {
    const draft = draftChain(kernels, src, { root, recipe, stem });
    if (draft.refusals.length) refused(draft.refusals);
    mkdirSync(dirname(declPath), { recursive: true });
    writeFileSync(declPath, `${JSON.stringify(draft.decl, null, 2)}\n`);
    console.log(`drafted ${relative(root, declPath)}: a chain of ${kernels.length} elements (${draft.decl.chain.map((el) => `${el.id} ${el.params.length}`).join(', ')} parameters), one per resolve() argument`);
    for (const n of draft.notes) console.log(`note: ${n}`);
    console.log('settle each mark, then run the same command again:');
    for (const m of reviewMarks(draft.decl)) console.log(`  [ ] ${m}`);
    console.log('  [ ] review each parameter\'s name; add "order": "permutable", "summary", "manual", "panel" or "console" where wanted');
    return;
  }
  const plan = await planChain(JSON.parse(readFileSync(declPath, 'utf8')), src, { root, recipe });
  if (!plan.ok) refused(plan.refusals);
  for (const b of plan.renames) console.log(`note: resolve argument '${b.arg}' binds '${b.param.symbol}' by its words; omx-dsp's name conformance owes the rename to '${b.rename}'`);
  return finish(plan, { root, recipe, stem, argv, opt });
}

/**
 * A declaration with `variants` (tools/variants.mjs): ONE hand-written file for several plugins.
 * Each variant is planned as a plugin of its own from the expanded declaration, and refused as one;
 * the base is written as the person wrote it, and every variant's folder is generated.
 */
async function variantsMain(raw, src, { root, recipe, declPath, argv, opt }) {
  const marks = reviewMarks(raw);
  if (marks.length) refused(marks.map((m) => ({ field: m, reason: 'still marked REVIEW: a person settles it' })));
  const schema = loadSchema(root, recipe);
  const errs = validate(schema, schema, raw);
  if (errs.length) refused(errs.map((e) => ({ field: e.split(':')[0], reason: `schema: ${e.slice(e.indexOf(':') + 2)}` })));
  const plans = [];
  for (const v of raw.variants.of) {
    const plan = await planPlugin(expandVariant(src.contract, raw, v), src, { root, recipe });
    if (!plan.ok) refused(plan.refusals.map((r) => ({ ...r, field: `${v.stem} ${r.field}` })));
    plans.push(plan);
  }
  const written = [relative(root, declPath)];
  for (const p of plans) written.push(...registerContract(root, p.decl.stem));
  const tools = [[join(root, 'tools', 'gen.mjs')], ...plans.map((p) => [join(root, 'tools', 'panel-file.mjs'), '--modgui', join(root, 'plugins', p.decl.stem)])];
  for (const args of tools) {
    const r = run('node', args, { cwd: root });
    if (r.code !== 0) throw new Error(`${relative(root, args[0])}: ${r.out.trim()}`);
    for (const m of r.out.matchAll(/wrote (\S+)/g)) written.push(relative(root, resolve(root, m[1])));
  }
  pinHints(root, declPath, raw, plans.map((p) => p.decl.stem), raw.params.filter((p) => p.perBand).map((p) => p.symbol));
  const files = [...new Set(written)];
  console.log(`\nwritten (${files.length}), the variants ${plans.map((p) => p.decl.stem).join(', ')} of ${raw.stem}:`);
  for (const f of files) console.log(`  ${f}`);
  const commits = commitPlan(recipe, { ...plans[0], decl: { ...plans[0].decl, name: raw.name, kernel: raw.kernel } }, files);
  console.log('\nCOMMIT PLAN (one concern per commit, in layer order; tools/commit-plan-check.mjs holds the range to it):');
  commits.forEach((c, i) => {
    console.log(`  ${i + 1}. [${c.layer}] ${c.message}`);
    for (const f of c.files) console.log(`       ${f}`);
  });
  if (opt('--plan-json')) writeFileSync(opt('--plan-json'), `${JSON.stringify(commits, null, 2)}\n`);
  if (argv.includes('--no-run')) return;
  let red = 0;
  for (const p of plans) {
    const stem = p.decl.stem;
    const make = run('make', ['-C', join(root, 'plugins', stem), 'test']);
    const fails = make.out.split('\n').filter((l) => /^FAIL /.test(l));
    console.log(`\nmake -C plugins/${stem} test: ${make.code === 0 ? 'green' : `RED\n  ${(fails.length ? fails : make.out.trim().split('\n').slice(-3)).join('\n  ')}`}`);
    const gaps = gapLines(await checkPlugin(root, recipe, stem));
    console.log(`completeness for ${stem}: ${gaps.length ? `${gaps.length} owed entries left` : 'complete'}`);
    for (const g of gaps) console.log(`  [ ] ${g}`);
    if (make.code !== 0 || gaps.length) red++;
  }
  if (red) process.exit(1);
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) await main(process.argv.slice(2));
