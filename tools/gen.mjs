#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * gen.mjs — every file a plugin's faces read, generated from its ONE declaration
 * (plugins/<stem>/<stem>.decl.json):
 *
 *   plugins/<stem>/generated/omx_<kernel>_params.h   the parameter table the CLAP and LV2 faces read
 *   plugins/<stem>/generated/<stem>.lv2/manifest.ttl  the LV2 bundle's manifest
 *   plugins/<stem>/generated/<stem>.lv2/<stem>.ttl    the LV2 plugin description
 *
 * and, for a plugin whose declaration says `"binding": "instance"` (or `"chain"`: a composite whose
 * `chain` runs several kernels' faces in order), the rest of its folder (the declaration
 * is then the folder's one hand-written file), from recipes/templates/plugin/:
 *
 *   plugins/<stem>/omx_<kernel>_core.h               the binding: each parameter to the resolve()
 *                                                    argument of omx-dsp's instance face it names
 *   plugins/<stem>/test/<kernel>-oracle.c            the kernel-identity test against that face
 *   plugins/<stem>/omx_<kernel>_clap.c, _lv2.c       the CLAP and LV2 faces
 *   plugins/<stem>/Makefile                          the build and `make test`
 *
 * and, from every plugin folder at once, the regions of the shared files between a `BEGIN
 * GENERATED <name>` line and its `END GENERATED <name>` line (SHARED below): the catalogue's
 * and sections, the RPM %files lists, CI's installed-file lists and libomxplugins' table of CLAP
 * entries (include/omx_clap_static.h, lib/omx_clap_static.c). Adding a plugin touches only its
 * folder; two plugins' changes merge in either order.
 *
 * and the hint view tools/test/port-hints.json (git-ignored), assembled from every declaration's
 * `portHints` (the pin of its LV2 port hints) for tools/port-hints.mjs.
 *
 * The MOD GUI of the same bundle is omx-panel's (tools/panel-file.mjs hands it the declared panel).
 *
 * Usage: `node tools/gen.mjs [--check] [plugins/<stem> ...]` — `--check` writes nothing and fails on a
 * stale, missing or changed file. Named plugins are generated alone; with none, every plugin and
 * the shared files. No clock and no absolute path reach the output.
 */
import { execFileSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import { existsSync, mkdirSync, readFileSync, readdirSync, writeFileSync } from 'node:fs';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { releaseVersion } from './release-version.mjs';
import { faceBinding } from './instance-face.mjs';
import { descriptorFile, descriptorOf, descriptorText, isWhole, panelDescription, portGroupsOf, travelUnit, viewOf } from './descriptor.mjs';
import { lv2Designation, lv2UnitNames, lv2UnitRef } from './format-tables.mjs';
import { DISPLAY_KINDS, DISPLAY_SOURCES, FAMILIES, PLACEMENTS, WIDGETS, pagesOf, panelFingerprint } from './omx-panel.mjs';
import { CONTRACT_VERSION, contractFingerprint, contractOf, faceIds } from './param-contract.mjs';
import { SPEC_FILE, controlOf, eqDefaultBands, findName, kernelControls, locateContract, packagingRange, paramKernel, renderKernels, resolveParam } from './omx-contract.mjs';
import { bandsMacro, baseOf, expandVariant, variantStems } from './variants.mjs';
import { omxdspInclude, render } from './template.mjs';
import { generateTest, isTestDir, loadTestDecl, testPluginDirs } from './test-plugins.mjs';
import { emitHintsC, hintPrefixes, hintTriples, statedHints } from './resource-hints.mjs';
import { presetsManifest, presetsOf } from './preset-lib.mjs';

export const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..');

const BANNER = '// SPDX-License-Identifier: GPL-3.0-or-later\n// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>\n';
const TTL_BANNER = '# SPDX-License-Identifier: GPL-3.0-or-later\n# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>\n';

/** Read and validate a plugin's declaration. Everything downstream trusts what this returns. */
export function loadDecl(dir) {
  const stem = basename(resolve(dir));
  let file = join(dir, `${stem}.decl.json`);
  let d, whole;
  if (existsSync(file) || !baseOf(dir)) {
    d = JSON.parse(readFileSync(file, 'utf8'));
    whole = isWhole(d);
    // a plugin described in omx-contract's words: its declaration is the descriptor's view, the
    // extras of <stem>.decl.json folded in (tools/descriptor.mjs)
    const described = join(dir, descriptorFile(stem));
    if (existsSync(described) && !whole) {
      const descriptor = JSON.parse(readFileSync(described, 'utf8'));
      d = { ...viewOf(descriptor, d, file), described: true, descriptor };
    }
  } else {
    // a variant's folder holds generated files only: its declaration is its base's, expanded
    const base = baseOf(dir);
    file = base.file;
    whole = isWhole(base.decl);
    const where = locateContract(treeOf(file));
    if (!where.dir) throw new Error(`${file}: its variant ${stem} is expanded from omx-contract and ${where.why}`);
    d = expandVariant(where.dir, base.decl, base.decl.variants.of.find((v) => v.stem === stem));
  }
  d.version = releaseVersion(treeOf(file));
  if (d.descriptor) d.descriptor.plugin.version = d.version;
  const prepared = prepareDecl(file, d, stem, dir);
  // a whole declaration (a base of variants, a chain) is rendered as the descriptor its faces are held to, as a
  // described plugin's is read from it: the faces state the designations, units and modules it states
  return prepared.described || !whole ? prepared : { ...prepared, described: true, descriptor: descriptorOf(prepared).descriptor };
}

/** A declaration read from `file` (or about to be written there: the wizard's plan), validated and
 * resolved for the plugin folder `dir`, whose name is `stem`. */
export function prepareDecl(file, d, stem, dir = dirname(file)) {
  if (d.variants) throw new Error(`${file}: the base of ${variantStems(d).join(', ')} is no plugin of its own; load one of its variants`);
  // a variant's sections had their `subgroups` expanded (tools/variants.mjs); a band is a variant's word only
  for (const s of d.panel?.sections ?? []) if (s.subgroups !== undefined) throw new Error(`${file}: section '${s.key}' says "subgroups", a word of a declaration with variants (its bands)`);
  if (d.subgroups !== undefined && !d.variantOf) throw new Error(`${file}: "subgroups" is what tools/variants.mjs writes for a variant (its bands); a declaration without variants carries none`);
  if (d.stem !== stem) throw new Error(`${file}: stem '${d.stem}' is not the directory's '${stem}'`);
  if (!/^[a-z][a-z0-9_]*$/.test(d.kernel)) throw new Error(`${file}: kernel '${d.kernel}' is not a C identifier`);
  if (d.binding === 'chain') {
    d.params = flattenChain(file, d);
    d.kernels = chainKernels(file, d);
  } else if (d.chain !== undefined || d.order !== undefined || d.key !== undefined) throw new Error(`${file}: "chain", "order" and "key" belong to a "binding": "chain" declaration`);
  const { params, controls } = resolveParams(file, d);
  d.params = params;
  // the resource hints, by word, from the contract's RESOURCE_HINTS (tools/resource-hints.mjs)
  const hints = statedHints(file, d, () => {
    const where = locateContract(treeOf(file));
    if (!where.dir) throw new Error(`${file}: its "resourceHints" are words of omx-contract's RESOURCE_HINTS and ${where.why}`);
    return where.dir;
  });
  d.hints = hints.stated;
  d.hintVocabulary = hints.vocabulary;
  if (generatedBinding(d)) d.controls = controls();
  const seen = new Set();
  for (const p of d.params) {
    if (!/^[A-Za-z][A-Za-z0-9_]*$/.test(p.symbol)) throw new Error(`${file}: symbol '${p.symbol}' is not an LV2 symbol`);
    if (seen.has(p.symbol)) throw new Error(`${file}: symbol '${p.symbol}' declared twice`);
    seen.add(p.symbol);
    if (!(p.min <= p.def && p.def <= p.max && p.min < p.max)) throw new Error(`${file}: '${p.symbol}' travel ${p.min}..${p.max} default ${p.def}`);
    if (p.kind && p.kind !== 'integer' && p.kind !== 'toggle') throw new Error(`${file}: '${p.symbol}' kind '${p.kind}'`);
    if (p.kind === 'toggle' && (p.min !== 0 || p.max !== 1)) throw new Error(`${file}: toggle '${p.symbol}' is not 0..1`);
    if (p.values && (p.kind !== 'integer' || p.values.length !== p.max - p.min + 1)) throw new Error(`${file}: '${p.symbol}' values name every step of an integer travel`);
    if (p.points) {
      const v = p.points.map((x) => x.value);
      if (p.kind !== 'integer' || p.values) throw new Error(`${file}: '${p.symbol}' points label an integer travel that has no values`);
      if (v[0] !== p.min || v[v.length - 1] !== p.max || v.some((x, k) => !Number.isInteger(x) || (k && x <= v[k - 1])) || !v.includes(p.def))
        throw new Error(`${file}: '${p.symbol}' points rise from min to max in whole steps and include the default`);
    }
  }
  if (d.sidechain && (seen.has(d.sidechain.symbol) || FIXED_PORTS.includes(d.sidechain.symbol))) throw new Error(`${file}: sidechain symbol '${d.sidechain.symbol}' is taken`);
  if (d.binding === 'chain') checkShipped(file, d);
  if (d.meters && d.binding !== 'instance') throw new Error(`${file}: "meters" are read from an instance face; this declaration's binding is ${d.binding ?? 'its own'}`);
  d.meterReadouts = meterReadoutsOf(file, d);
  for (const o of d.oracles ?? []) {
    if (o === `${d.kernel}-oracle`) throw new Error(`${file}: oracle '${o}' is the generated identity test's name`);
    if (!existsSync(join(dir, 'test', `${o}.c`))) throw new Error(`${file}: oracle '${o}' names test/${o}.c, which does not exist`);
  }
  return { ...d, dir: resolve(dir), tree: treeOf(file) };
}

/** The panel family (schema `$defs.family`, the console's closed set) of a kernel whose own name is
 * none of them; a kernel named after a family is that family, any other kernel `none`. */
export const FAMILY_OF_KERNEL = Object.freeze({
  comp: 'dynamics', transient: 'dynamics', chorus: 'modulation', flanger: 'modulation', phaser: 'modulation',
  tremolo: 'modulation', rotary: 'modulation', geq: 'eq', dyneq: 'eq', mbcomp: 'dynamics',
});

/** The panel family a kernel's plugin draws as: FAMILY_OF_KERNEL's, else the family of its own name. */
export function familyOfKernel(kernel) {
  const families = JSON.parse(readFileSync(join(ROOT, 'schema', 'plugin.decl.schema.json'), 'utf8')).$defs.family.enum;
  const family = FAMILY_OF_KERNEL[kernel] ?? kernel;
  return families.includes(family) ? family : 'none';
}

/** The panel the wizard drafts for a plugin generated from its instance face: one section, every
 * parameter in declaration order, so the MOD GUI draws every parameter until a person groups them;
 * its family the kernel's, its roles left to a person. */
export function defaultPanel(d) {
  if (d.binding === 'chain') {
    // one section per element, by its id (the elements are drawn, never placed, apart)
    const sections = d.chain.map((el) => ({ key: el.id, label: elementName(el), controls: [`${el.id}On`, ...el.params.map((p) => p.symbol)] }));
    if (d.order === 'permutable') sections.push({ key: 'order', label: 'Order', controls: ['order'] });
    return { family: familyOfKernel(d.kernel), roles: {}, sections };
  }
  const label = d.name.replace(/^omx /, '');
  return { family: familyOfKernel(d.kernel), roles: {}, sections: [{ key: d.kernel, label: label[0].toUpperCase() + label.slice(1), controls: d.params.map((p) => p.symbol) }] };
}

/** A plugin whose folder tools/gen.mjs writes whole from the declaration: over one instance face,
 * or a chain of them. */
export const generatedBinding = (d) => d.binding === 'instance' || d.binding === 'chain';

// ---- a composite: a chain of kernel faces -----------

/** An element's name, the label of its switch and of its panel section: its `name`, else its id. */
const elementName = (el) => el.name ?? `${el.id[0].toUpperCase()}${el.id.slice(1)}`;

/** n! */
const factorial = (n) => (n <= 1 ? 1 : n * factorial(n - 1));

/** Every order of `n` elements, lexicographic: row k is the order the `order` parameter's value k runs
 * (0 the declared order). */
export function permutations(n) {
  const out = [];
  const walk = (row, left) => {
    if (!left.length) return out.push(row);
    for (const x of left) walk([...row, x], left.filter((y) => y !== x));
  };
  walk([], [...Array(n).keys()]);
  return out;
}

/** A parameter as its element names it: the symbol without the element's id in front (`eqHpfSlope`
 * -> `hpfSlope`), what omx-contract matches a control shared by two names against. */
const localParam = (p) => {
  if (!p.element || !p.symbol.startsWith(p.element) || !/^[A-Z]/.test(p.symbol.slice(p.element.length))) return p;
  const rest = p.symbol.slice(p.element.length);
  return { ...p, symbol: `${rest[0].toLowerCase()}${rest.slice(1)}` };
};

/**
 * The flattened parameter list of a chain: element by element in chain order, each element's
 * generated switch `<id>On` (its default the element's `on`) and then its parameters, each read from
 * the element's kernel; then, for `"order": "permutable"`, the plugin's own `order` over the
 * permutations of the elements.
 */
function flattenChain(file, d) {
  if (d.params !== undefined) throw new Error(`${file}: a chain's parameters are generated from "chain"; declare no "params"`);
  if (!Array.isArray(d.chain) || d.chain.length < 2) throw new Error(`${file}: "binding": "chain" runs two or more elements, listed in "chain"`);
  const ids = new Set();
  const out = [];
  for (const el of d.chain) {
    if (!/^[a-z][a-z0-9]*$/.test(el.id ?? '')) throw new Error(`${file}: chain element id '${el.id}' is not a lower-case word`);
    if (ids.has(el.id)) throw new Error(`${file}: chain element '${el.id}' declared twice`);
    ids.add(el.id);
    if (!/^[a-z][a-z0-9_]*$/.test(el.kernel ?? '')) throw new Error(`${file}: chain element '${el.id}' names no kernel`);
    if (typeof el.on !== 'boolean') throw new Error(`${file}: chain element '${el.id}': "on" (its switch's default) is true or false`);
    if (!Array.isArray(el.params) || !el.params.length) throw new Error(`${file}: chain element '${el.id}' has no "params"`);
    out.push({ symbol: `${el.id}On`, name: elementName(el), unit: '', min: 0, max: 1, def: el.on ? 1 : 0, kind: 'toggle', own: 'switch', element: el.id });
    for (const p of el.params) {
      if (p.ref === undefined) throw new Error(`${file}: chain element '${el.id}': '${p.symbol}' has no ref; an element's parameters are its kernel's contract controls`);
      if (p.kernel !== undefined && p.kernel !== el.kernel) throw new Error(`${file}: chain element '${el.id}': '${p.symbol}' names kernel '${p.kernel}', not the element's '${el.kernel}'`);
      out.push({ ...p, kernel: el.kernel, element: el.id });
    }
  }
  if (d.order === 'permutable') out.push({ symbol: 'order', name: 'Order', unit: '', min: 0, max: factorial(d.chain.length) - 1, def: 0, kind: 'integer', own: 'stage-order' });
  else if (d.order !== undefined) throw new Error(`${file}: "order" is "permutable" or left out (the declared order)`);
  // what an element gained after the plugin shipped comes AFTER every parameter it had, so no id and no LV2 parameter
  // port moves: element by element in chain order, its band count (`bandCount`), then its `appended` parameters
  for (const el of d.chain) {
    if (el.bandCount !== undefined) out.push(bandCountParam(file, d, el));
    for (const p of el.appended ?? []) {
      if (p.ref === undefined) throw new Error(`${file}: chain element '${el.id}': appended '${p.symbol}' has no ref; an element's parameters are its kernel's contract controls`);
      if (p.kernel !== undefined && p.kernel !== el.kernel) throw new Error(`${file}: chain element '${el.id}': '${p.symbol}' names kernel '${p.kernel}', not the element's '${el.kernel}'`);
      out.push({ ...p, kernel: el.kernel, element: el.id });
    }
  }
  if (d.key !== undefined) {
    if (!ids.has(d.key)) throw new Error(`${file}: "key" names '${d.key}', no element of the chain`);
    if (!d.sidechain) throw new Error(`${file}: "key" hands the plugin's sidechain to '${d.key}'; declare it in "sidechain"`);
  } else if (d.sidechain) throw new Error(`${file}: a chain's "sidechain" goes to one element, named by "key"`);
  return out;
}

/**
 * An element's band count (`bandCount`): the plugin's own integer parameter, from one band to the `max` of the
 * element's `bands` in its kernel's count sheet (eq's EQ_BAND_COUNTS), coming up at the `default` of the sheet key
 * `bandCount.default` names. The bands at and past it are passed to the face switched off (its per-band control
 * `bandCount.gates`), so the face leaves them out of its bank: they cost no processing and sound as if absent.
 */
function bandCountParam(file, d, el) {
  const bc = el.bandCount;
  if (!el.bands) throw new Error(`${file}: chain element '${el.id}' declares "bandCount" and no "bands": a count of what?`);
  const where = locateContract(treeOf(file));
  if (!where.dir) throw new Error(`${file}: chain element '${el.id}' counts its bands in omx-contract and ${where.why}`);
  const controls = kernelControls(where.dir, el.kernel);
  const sheet = controls.find((c) => c.count)?.count;
  if (!sheet) throw new Error(`${file}: chain element '${el.id}' declares "bandCount", and the ${el.kernel} kernel has no per-band control`);
  const counts = findName(where.dir, el.kernel, sheet).entry?.value ?? {};
  const max = counts[el.bands]?.max, def = counts[bc.default]?.default;
  if (!Number.isInteger(max)) throw new Error(`${file}: chain element '${el.id}': ${sheet} has no '${el.bands}' with a max`);
  if (!Number.isInteger(def) || def < 1 || def > max) throw new Error(`${file}: chain element '${el.id}': "bandCount" comes up at ${sheet}.${bc.default}.default (${def}), not a count from 1 to ${max}`);
  const gated = controls.find((c) => c.name === bc.gates);
  if (!gated?.count) throw new Error(`${file}: chain element '${el.id}': "bandCount" gates '${bc.gates}', no per-band control of the ${el.kernel} kernel`);
  return { symbol: bc.symbol, name: bc.name, unit: '', min: 1, max, def, kind: 'integer', own: 'band-count', element: el.id, gates: bc.gates };
}

/** The band-count parameter of element `el`, or undefined. */
const bandCountOf = (d, el) => d.params.find((p) => p.own === 'band-count' && p.element === el.id);

/** The per-band parameters of each element with a band count, by control: `[[i, ...], ...]`, each list one control's
 * parameters (every band's type, every band's switch, ...) of one element, indices into d.params. */
function bandGroups(d) {
  const out = [];
  for (const el of d.chain ?? []) {
    if (!bandCountOf(d, el)) continue;
    const byControl = new Map();
    d.params.forEach((p, i) => {
      const c = d.controls?.[i];
      if (p.element !== el.id || !c?.count) return;
      if (!byControl.has(c.name)) byControl.set(c.name, []);
      byControl.get(c.name).push(i);
    });
    out.push(...byControl.values());
  }
  return out;
}

/** The C name of the function that reads element `el`'s band count from the values (emitted in the parameter header). */
const bandCountFn = (d, el) => `omx_${d.kernel}_${el.id}_band_count`;

/** The macro of an element's band count, `bands` the key of its kernel's count sheet (eq's
 * EQ_BAND_COUNTS: strip, eq8, ...): OMX_EQ_BAND_COUNTS_STRIP_MAX. */
/** The count sheet key an element's face is compiled at: `faceBands` (a count the face takes where
 * the element exposes fewer bands), else `bands`. */
export const faceBandsOf = (el) => el.faceBands ?? el.bands;

export function chainBandsMacro(d, el) {
  const where = locateContract(d.tree ?? ROOT);
  if (!where.dir) throw new Error(`plugins/${d.stem}: element '${el.id}' counts its bands in omx-contract and ${where.why}`);
  const sheet = kernelControls(where.dir, el.kernel).find((c) => c.count)?.count;
  if (!sheet) throw new Error(`plugins/${d.stem}: element '${el.id}' declares "bands", and the ${el.kernel} kernel has no per-band control`);
  const e = findName(where.dir, el.kernel, sheet).entry;
  const key = faceBandsOf(el);
  if (!Number.isInteger(e?.value?.[key]?.max)) throw new Error(`plugins/${d.stem}: element '${el.id}': ${sheet} has no '${key}' with a max`);
  return `OMX_${sheet}_${key.toUpperCase()}_MAX`;
}

/** The kernels a chain reads, in chain order, each once; a declared `kernels` must be the same. */
function chainKernels(file, d) {
  const ks = [...new Set(d.chain.map((el) => el.kernel))];
  if (d.kernels !== undefined && JSON.stringify(d.kernels) !== JSON.stringify(ks)) throw new Error(`${file}: "kernels" is ${JSON.stringify(d.kernels)}, the chain's are ${JSON.stringify(ks)}; leave it out`);
  return ks;
}

/** The parameter symbols a released plugin shipped: its generated header at the git tag `tag`, or
 * undefined where the tree has no git or no such tag (a tarball build). */
function shippedSymbols(d, tag) {
  try {
    const text = execFileSync('git', ['-C', d.tree, 'show', `${tag}:plugins/${d.stem}/generated/omx_${d.kernel}_params.h`], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'] });
    // the rows of the parameter table alone: the header holds other tables of strings after it
    const table = text.slice(text.indexOf(`OMX_${d.kernel.toUpperCase()}_PARAMS[`)).split('\n};')[0];
    return [...table.matchAll(/^\s*\{ "([A-Za-z0-9_]+)", "/gm)].map((m) => m[1]);
  } catch {
    return undefined;
  }
}

/** The differences between a released list of symbols and the flattened one, index by index. */
export function symbolDiff(was, now) {
  const out = [];
  for (let i = 0; i < Math.max(was.length, now.length); i++) {
    if (was[i] === now[i]) continue;
    out.push(i >= was.length ? `+${i} ${now[i]}` : i >= now.length ? `-${i} ${was[i]}` : `${i} ${was[i]} -> ${now[i]}`);
  }
  return out;
}

/** A chain converted from a released plugin (`shipped`: its release tag) keeps that plugin's ids
 * and symbols, or names every difference in `shippedDiff`: CLAP ids are indices and LV2
 * symbols are saved state, so a difference is a decision, never a side effect. */
function checkShipped(file, d) {
  if (d.shipped === undefined) return;
  const was = shippedSymbols({ ...d, tree: treeOf(file) }, d.shipped);
  if (!was) {
    // a tarball or shallow build has no tag to read: said, never silent
    console.error(`gen: NOTE ${d.stem}: ${d.shipped} is not readable here (no git or no such tag); its parameter list is not checked against that release`);
    return;
  }
  const diff = symbolDiff(was, d.params.map((p) => p.symbol));
  const named = d.shippedDiff ?? [];
  if (JSON.stringify(diff) !== JSON.stringify(named))
    throw new Error(`${file}: the flattened parameters differ from ${d.stem} ${d.shipped}'s, and "shippedDiff" does not name exactly the differences; they are:\n  ${diff.join('\n  ') || '(none: drop "shippedDiff")'}`);
}

/** The tree a declaration sits in; a declaration copied out of its tree (a test's scratch copy)
 * reads this tree's packaging. */
const treeOf = (file) => {
  const tree = resolve(dirname(file), '..', '..');
  return existsSync(join(tree, SPEC_FILE)) ? tree : ROOT;
};

/** The travel fields a parameter BY REFERENCE reads from omx-contract and must never retype. */
export const TRAVEL_FIELDS = ['min', 'max', 'def', 'unit', 'kind'];

/** Each parameter with its travel: read from the installed omx-contract through its `ref`
 * (tools/omx-contract.mjs), or typed and marked `own` with the reason omx-contract declares no such
 * control (a switch of the plugin's face, the strip's stage order). A parameter is one or the other. */
function resolveParams(file, d) {
  let where;
  const contract = () => {
    if (!where) {
      where = locateContract(treeOf(file));
      if (!where.dir) throw new Error(`${file}: its parameters are by reference and ${where.why}`);
    }
    return where.dir;
  };
  // each parameter's contract control ({ name }), or undefined for an own one: what the generated
  // binding matches resolve() arguments against
  const controls = () => d.params.map((p) => (p.ref === undefined ? undefined : controlOf(contract(), paramKernel(d, p, contract()), localParam(p))));
  const params = d.params.map((p) => {
    if (p.ref === undefined) {
      if (typeof p.own !== 'string' || !p.own) throw new Error(`${file}: '${p.symbol}' has no ref and no "own" reason; a parameter is by reference to omx-contract or says why it is the plugin's own`);
      return p;
    }
    if (p.own !== undefined) throw new Error(`${file}: '${p.symbol}' is by reference and "own"; it is one or the other`);
    const dir = contract();
    const kernel = paramKernel(d, p, dir);
    // a set's unit is face text the set does not declare (a slope's dB/oct); a travel's is the contract's
    const isSet = findName(dir, kernel, p.ref).entry?.kind === 'set';
    const typed = TRAVEL_FIELDS.filter((k) => k in p && !(isSet && k === 'unit'));
    if (typed.length) throw new Error(`${file}: '${p.symbol}' is by reference and retypes ${typed.join(', ')}; omx-contract holds them`);
    const t = resolveParam(dir, kernel, p);
    const out = { ...p, min: t.min, max: t.max, def: t.def, unit: t.unit, from: t.from };
    // a control whose travel another choice widens (eq's q, notchQ while the band is a notch): the
    // port spans every travel it may take, and the face clamps by the choice
    const c = generatedBinding(d) ? controlOf(dir, kernel, localParam(p)) : undefined;
    const widest = [];
    for (const w of c?.when ?? []) {
      const u = resolveParam(dir, kernel, { symbol: p.symbol, ref: w.global });
      out.min = Math.min(out.min, u.min);
      out.max = Math.max(out.max, u.max);
      widest.push(u);
    }
    // the travel the port states is the one its link resolves to (tools/descriptor.mjs): the link is the
    // widening travel itself, which must be the whole extent the port spans
    if (widest.length) {
      const whole = widest.find((u) => u.min === out.min && u.max === out.max);
      if (!whole) throw new Error(`${file}: '${p.symbol}' spans ${out.min}..${out.max}, which no single travel it may take states; its link could not resolve to it`);
      out.from = whole.from;
    }
    if (t.kind) out.kind = t.kind;
    if (t.byKind) out.byKind = t.byKind;
    if (t.points) out.points = t.points;
    if (t.values) out.values = t.values;
    return out;
  });
  // a chain's element parameters are named from their control; every other by-reference parameter's symbol must be its control's
  if (!d.chain) {
    const dir = contract();
    const errs = d.params.flatMap((p, i) => {
      const c = controls()[i];
      return c ? symbolTie(localParam(p), c, kernelControls(dir, paramKernel(d, p, dir))) : [];
    });
    if (errs.length) throw new Error(`${file}: a parameter's symbol must be its control's words, or say the control (\`control\`):\n  ${errs.join('\n  ')}`);
  }
  for (const q of params) delete q.control;
  return { params, controls };
}

/** A name's words with its digits dropped: `key_hp` and `keyHp` -> key, hp; `b3_gain` -> b, gain is read by the caller. */
const wordsOf = (x) => x.replace(/([a-z0-9])([A-Z])/g, '$1_$2').toLowerCase().split('_').map((w) => w.replace(/[0-9]+/g, '')).filter(Boolean);
const inOrder = (small, big) => {
  let i = 0;
  for (const w of big) if (w === small[i]) i++;
  return i === small.length;
};

/**
 * What ties a parameter's symbol to the control its `ref` names (FreeMixer/omx-plugins#58): without it two refs can
 * be swapped and every generated check follows the swap. The symbol, a band's `b<n>_` prefix and an element's prefix
 * off, digits dropped, is the control's words (`rateHz` for rate_hz) or an in-order part of them (`threshold` of
 * thresholdDb) that is not exactly the words of another control; a symbol frozen from before the wizard drafted
 * it from the control (`key_external`) says `control` and the control must be the one the ref names. Returns the
 * refusals.
 */
export function symbolTie(p, control, controls) {
  if (p.control !== undefined) {
    return p.control === control.name ? [] : [`'${p.symbol}' says its control is '${p.control}', and its ref (${p.ref}) names '${control.name}'`];
  }
  const sym = wordsOf(p.symbol.replace(/^b[0-9]+_/, ''));
  const own = wordsOf(control.name);
  if (sym.join('_') === own.join('_')) return [];
  // a part of the words may fit several controls (`on` of bandOn and hpfOn); the other side of any swap is caught, but a
  // symbol that is exactly another control's words is that control's
  const exact = controls.filter((c) => c.name !== control.name && wordsOf(c.name).join('_') === sym.join('_'));
  if (exact.length) return [`'${p.symbol}' is the words of '${exact[0].name}', but its ref (${p.ref}) names '${control.name}'`];
  if (!inOrder(sym, own)) return [`'${p.symbol}' is not the words of '${control.name}', the control its ref (${p.ref}) names`];
  return [];
}

/** SHA-256 of the resolved parameter list: the org.openmixer.declaration/1 digest, the formula the
 * OpenMixer engine computes for its stale-parameter check, so it reads ours unchanged. */
/** The declaration's plain description for org.openmixer.declaration/1: each parameter's name and
 * travel, in declaration order. */
export function sourceOf(d) {
  return `${d.name}: ${d.params.map((p) => `${p.name} ${p.min} to ${p.max}${p.unit ? ` ${p.unit}` : ''}`).join(', ')}`;
}

export function digestOf(params) {
  const rows = params.map((p) => [p.symbol, p.unit, p.min, p.max, p.def, p.kind ? 1 : 0, p.kind === 'toggle']);
  return createHash('sha256').update(JSON.stringify(rows)).digest('hex');
}

/** The plugin-defined units, by name (urn:openmixer:unit#<name>): defined (units:Unit) in the plugin's own TTL, so a
 * host that reads units:unit finds the unit and how to write it. */
export const LV2_CUSTOM_UNITS = {
  dBFS: { label: 'decibels (full scale)', render: '%f dBFS', symbol: 'dBFS' },
  dBu: { label: 'decibels (unloaded)', render: '%f dBu', symbol: 'dBu' },
  LUFS: { label: 'loudness units (full scale)', render: '%f LUFS', symbol: 'LUFS' },
  dBoct: { label: 'decibels per octave', render: '%f dB/oct', symbol: 'dB/oct' },
  ratio: { label: 'ratio', render: '%f:1', symbol: ':1' },
};

export { lv2Designation, lv2UnitNames, lv2UnitRef };

/** How a host draws a travel: as declared, else a frequency logarithmic (the console's every
 * frequency control is) when its travel is positive (a key filter whose 0 is off is not), else linear. */
export const scaleOf = (p) => p.scale ?? (p.unit === 'Hz' && !(p.min <= 0) ? 'log' : 'linear');

/** The labelled values of an integer travel, `{value, label}` from min up: one per step (`values`)
 * or only the ones it takes (`points`). Either makes the LV2 port an enumeration. */
export const choicesOf = (p) => p.points ?? p.values?.map((label, k) => ({ value: p.min + k, label }));

/** The port symbols every plugin has, which no parameter or key may take. */
const FIXED_PORTS = ['in_l', 'in_r', 'out_l', 'out_r', 'enabled', 'latency'];

/** The kernel readout each meter of declaration `d` names (`readout`, the readout's name in macro case), as
 * omx-contract's render resolves it for the plugin's kernel: `{ name, doc, unit, min, max, scale }`, or null for a
 * meter the face measures (a level at the input or the output), in the order of `meters`. */
function meterReadoutsOf(file, d) {
  if (!d.meters?.some((m) => m.readout)) return (d.meters ?? []).map(() => null);
  const where = locateContract(treeOf(file));
  if (!where.dir) throw new Error(`${file}: its "meters" read omx-contract's ${d.kernel} readouts and ${where.why}`);
  const listed = renderKernels(where.dir)[d.kernel]?.readouts ?? [];
  return d.meters.map((m, k) => {
    if (!m.readout) return null;
    const r = listed.find((x) => macro(x.name) === m.readout);
    if (!r) throw new Error(`${file}: meters[${k}] names readout ${m.readout}, which omx-contract's ${d.kernel} kernel does not declare (${listed.map((x) => macro(x.name)).join(', ') || 'none'})`);
    return r;
  });
}

/** What a meter's port and descriptor entry are called, from its id: a band's prefix (`band2`) as "Band 2",
 * the kind its suffix names (Gr, Gain, In, Out, Key) in words, any other prefix (the plugin's own name, as
 * `comp` in compGr) left out. */
const METER_WORDS = { Gr: 'gain reduction', Gain: 'gain', In: 'input level', Out: 'output level', Key: 'key level' };
export function meterLabel(id) {
  const m = id.match(/^(?:band(\d+))?[a-z]*?(Gr|Gain|In|Out|Key)$/);
  if (!m) throw new Error(`meter '${id}': no label for it (its id ends in none of ${Object.keys(METER_WORDS).join(', ')})`);
  const words = METER_WORDS[m[2]];
  return m[1] ? `Band ${m[1]} ${words}` : `${words[0].toUpperCase()}${words.slice(1)}`;
}

/** Each meter of declaration `d` as the faces publish it, in order: its id, kind, channels (one for a reading
 * of the kernel's, two for a level the face measures, left and right), label, display range and unit (a
 * readout's, else the declared range in dBFS), and the output symbol of each channel: the id for one channel,
 * `<id>_<channel>` from 0 for more, the names omx-clap-core derives from clap.meters/1 (doc/reference.md). */
export function metersOf(d) {
  const readouts = d.meterReadouts ?? (d.meters ?? []).map(() => null);
  return (d.meters ?? []).map((m, k) => {
    const r = readouts[k];
    const channels = m.kind === 'gain_reduction' || 'read' in m ? 1 : 2;
    const range = r ? { min: r.min, max: r.max, unit: r.unit, scale: r.scale } : { min: m.min, max: m.max, unit: 'dBFS', scale: 'linear' };
    const symbols = channels === 1 ? [m.id] : Array.from({ length: channels }, (_, c) => `${m.id}_${c}`);
    return { id: m.id, kind: m.kind, channels, label: meterLabel(m.id), ...range, symbols, readout: r };
  });
}

/** Whether the plugin reads the host's tempo: its descriptor's needs.tempo. Its LV2 face then has a `control` atom
 * input, the port right after the face's own (enabled, latency), so an appended parameter moves it no more than them. */
export const tempoOf = (d) => Boolean(d.descriptor?.needs?.tempo);

/** The LV2 port list, in index order: the face's own ports at omx-contract's permanent indices (OMX_PLUGIN_FACE_IDS:
 * enabled 0, latency 1), then (a plugin that reads the host's tempo) the `control` atom input, then audio in (and the
 * sidechain key, when declared), audio out, then the declared parameters. Appending a parameter moves no port; adding
 * a port moves neither enabled, latency nor control. `face` is the ids (default: this tree's contract).
 * One rule, read by the TTL, the MOD GUI and (as macros) the LV2 C face. */
export function lv2Ports(d, face = faceIds()) {
  const IN = 'lv2:AudioPort , lv2:InputPort', OUT = 'lv2:AudioPort , lv2:OutputPort';
  const audio = [
    { symbol: 'in_l', name: 'In L', a: IN }, { symbol: 'in_r', name: 'In R', a: IN },
    ...(d.sidechain ? [{ symbol: d.sidechain.symbol, name: d.sidechain.name, a: IN, sidechain: true }] : []),
    { symbol: 'out_l', name: 'Out L', a: OUT }, { symbol: 'out_r', name: 'Out R', a: OUT },
  ];
  const params = d.params.map((p) => ({ symbol: p.symbol, name: p.name, a: 'lv2:ControlPort , lv2:InputPort', param: p }));
  const fixed = new Map([
    [face.enabledLv2Index, { symbol: 'enabled', name: 'Enabled', a: 'lv2:ControlPort , lv2:InputPort', enabled: true }],
    [face.latencyLv2Index, { symbol: 'latency', name: 'Latency', a: 'lv2:ControlPort , lv2:OutputPort', latency: true }],
  ]);
  const meters = metersOf(d).flatMap((m) => m.symbols.map((symbol, c) => ({
    symbol, name: m.channels === 1 ? m.label : `${m.label} ${['L', 'R'][c] ?? c}`, a: 'lv2:ControlPort , lv2:OutputPort', meter: m,
  })));
  const rest = [...(tempoOf(d) ? [{ symbol: 'control', name: 'Control', a: 'atom:AtomPort , lv2:InputPort', time: true }] : []),
    ...meters, ...audio, ...params];
  const n = rest.length + fixed.size;
  if (fixed.size !== 2 || [...fixed.keys()].some((i) => !(i >= 0 && i < n))) throw new Error(`${d.stem}: the face's LV2 indices ${[...fixed.keys()]} do not fit ${n} ports`);
  const out = [];
  for (let index = 0, k = 0; index < n; index++) out.push({ ...(fixed.get(index) ?? rest[k++]), index });
  return out;
}

const cf = (n) => `${Number.isInteger(n) ? `${n}.0` : `${n}`}f`;
const macro = (s) => s.replace(/([a-z0-9])([A-Z])/g, '$1_$2').toUpperCase();
const cstr = (s) => `"${s.replace(/\\/g, '\\\\').replace(/"/g, '\\"')}"`;

/** The rows of org.openmixer.panel-description/1 in the parameter header: each table of
 * omx-panel's layout as C, and OMX_<K>_PANEL, the description a face serves. */
function emitPanelRows(d) {
  const K = d.kernel.toUpperCase();
  const x = panelDescription(d);
  const NONE = 'OMX_CLAP_PANEL_INVALID_ID';
  const id = (n) => (n === null || n === undefined ? NONE : `${n}u`);
  const enumOf = (prefix, list, v) => `OMX_CLAP_${prefix}_${list[list.indexOf(v)].toUpperCase()}`;
  const tables = [
    ['SECTIONS', 'section', x.sections.map((s) => `{ ${cstr(s.key)}, ${cstr(s.labelKey)}, ${cstr(s.label)} }`)],
    ['CONTROLS', 'control', x.controls.map((c) => `{ ${c.param}u, ${c.section}u, ${enumOf('WIDGET', WIDGETS, c.widget)}, ${cstr(c.labelKey)}, ${cstr(c.label)}, ${id(c.visible)}, 0x${c.mask.toString(16)}u }`)],
    ['ROLES', 'role', x.roles.map((r) => `{ ${cstr(r.role)}, ${r.index}u, ${r.param}u }`)],
    ['DISPLAYS', 'display', x.displays.map((v) => `{ ${enumOf('DISPLAY', DISPLAY_KINDS, v.kind)}, ${v.section}u, OMX_CLAP_DISPLAY_FROM_${DISPLAY_SOURCES[DISPLAY_SOURCES.indexOf(v.source)].toUpperCase()}, { ${id(v.meters[0])}, ${id(v.meters[1])} }, ${v.curve ?? 0}u, ${NONE}, 0.0, ${cstr(v.labelKey)}, ${cstr(v.label)} }`)],
    ['DEFAULTS', 'default', x.defaults.map((p) => `{ ${enumOf('PLACEMENT', PLACEMENTS, p.placement)}, ${p.param}u, ${p.value} }`)],
  ];
  const array = ([T, row, rows]) => (rows.length ? `static const struct omx_panel_${row}_row OMX_${K}_PANEL_${T}[] = {\n${rows.map((r) => `  ${r},`).join('\n')}\n};\n` : '');
  const ref = ([T, , rows]) => (rows.length ? `OMX_${K}_PANEL_${T}, ${rows.length}u` : 'NULL, 0u');
  const unpublished = x.unpublished.map((u) => ` *   ${u.role} (${u.symbol}): ${u.why}`).join('\n');
  return `/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family ${x.family}.${unpublished ? `
 * Declared roles not published:
${unpublished}` : ''} */
#define OMX_${K}_PANEL_FINGERPRINT "${panelFingerprint(x)}"
${tables.map(array).join('')}/* clap.remote-controls/2: the pages of the page rule over the sections. */
static const struct omx_panel_page_row OMX_${K}_PANEL_PAGES[] = {
${pagesOf(x, d).map((g) => `  { ${cstr(g.sectionName)}, ${cstr(g.pageName)}, { ${g.ids.map(id).join(', ')} } },`).join('\n')}
};
#define OMX_${K}_PANEL_PAGE_COUNT ${pagesOf(x, d).length}u
static const struct omx_panel_rows OMX_${K}_PANEL = {
  OMX_${K}_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_${FAMILIES[FAMILIES.indexOf(x.family)].toUpperCase()},
${tables.map((t) => `  ${ref(t)},`).join('\n')}
};
`;
}

export function emitParamsHeader(d) {
  const K = d.kernel.toUpperCase();
  const P = `OMX_${K}_PARAM`;
  const flags = (p) => (p.kind === 'integer' ? 'OMX_PLUGIN_PARAM_INTEGER' : p.kind === 'toggle' ? 'OMX_PLUGIN_PARAM_TOGGLE' : '0u');
  const ports = lv2Ports(d);
  const firstParam = ports.find((p) => p.param).index;
  return `${BANNER}#ifndef OMX_${K}_PARAMS_H
#define OMX_${K}_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/${d.stem}/${d.stem}.decl.json.
 * Regenerate: \`make -C plugins/${d.stem} gen\`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_${d.kernel}_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>
${(d.chain ?? []).some((el) => bandCountOf(d, el)) ? '#include <math.h>\n#include <stdint.h>\n' : ''}
#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"
${d.variantOf ? `
/* The band count of this variant of plugins/${d.variantOf.stem}: the instance face's compile-time count. */
#define OMX_${(d.face ?? d.kernel).toUpperCase()}_INSTANCE_BANDS ${bandsMacro(d)}
` : ''}${(d.chain ?? []).filter((el, i, all) => faceBandsOf(el) && all.findIndex((o) => o.kernel === el.kernel) === i).map((el) => `
/* The band count of the chain's ${el.id}: its instance face's compile-time count, omx-contract's ${chainBandsMacro(d, el)}. */
#define OMX_${el.kernel.toUpperCase()}_INSTANCE_BANDS ${chainBandsMacro(d, el)}
`).join('')}
enum {
${d.params.map((p, i) => `  ${P}_${macro(p.symbol)} = ${i},`).join('\n')}
  ${P}_COUNT = ${d.params.length}
};

static const omx_plugin_param OMX_${K}_PARAMS[${P}_COUNT] = {
${d.params.map((p) => `  { "${p.symbol}", "${p.name}", "${p.unit}", ${cf(p.min)}, ${cf(p.max)}, ${cf(p.def)}, ${flags(p)} },`).join('\n')}
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_${K}_PARAM_MODULES[${P}_COUNT] = {
${d.params.map((p, i) => `  ${cstr(moduleOf(d, i))},`).join('\n')}
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
${d.params.map((p) => [['MIN', p.min], ['MAX', p.max], ['DEFAULT', p.def]].map(([k, v]) => `#define ${P}_${macro(p.symbol)}_${k} ${cf(v)}`).join('\n')).join('\n')}
${(d.chain ?? []).filter((el) => bandCountOf(d, el)).map((el) => {
    const N = `${P}_${macro(bandCountOf(d, el).symbol)}`;
    return `
/* The ${el.id} element's band count, read from the values the faces resolve: the word rounded into its travel, a
 * non-finite word its default. Its bands from this one on are passed switched off: out of the face's bank. */
static inline uint32_t ${bandCountFn(d, el)}(const float *values) {
  const float w = values[${N}];
  /* clamped, then rounded to the nearest (ties to even): the same count as rounding first, and lrintf is a call
   * omx-rt-gate classifies where rintf is not */
  const int n = w - w == 0.0f ? (int)lrintf(fminf(fmaxf(w, (float)${N}_MIN), (float)${N}_MAX)) : (int)${N}_DEFAULT;
  return (uint32_t)(n < (int)${N}_MIN ? (int)${N}_MIN : n > (int)${N}_MAX ? (int)${N}_MAX : n);
}
`;
  }).join('')}
/* The identity every face publishes. */
#define OMX_${K}_NAME "${d.name}"
#define OMX_${K}_VENDOR "${d.vendor}"
#define OMX_${K}_URL "${d.url}"
#define OMX_${K}_VERSION "${d.version}"
#define OMX_${K}_DESCRIPTION "${d.description.replace(/"/g, '\\"')}"
#define OMX_${K}_CLAP_ID "${d.clap.id}"
#define OMX_${K}_CLAP_FEATURES ${d.clap.features.map((f) => `"${f}"`).join(', ')}
#define OMX_${K}_LV2_URI "${d.lv2.uri}"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_${K}_DECL_SOURCE "${sourceOf(d).replace(/"/g, '\\"')}"
#define OMX_${K}_DECL_DIGEST "${digestOf(d.params)}"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), is the face's. */
#define OMX_${K}_PARAM_CONTRACT_VERSION ${CONTRACT_VERSION}u
#define OMX_${K}_PARAM_CONTRACT_FINGERPRINT "${contractFingerprint(contractOf(d, scaleOf))}"
static const struct omx_param_contract_row OMX_${K}_PARAM_CONTRACT[${P}_COUNT] = {
${contractOf(d, scaleOf).slice(0, -1).map((r) => `  { ${r.unitEnum}u, OMX_CLAP_SCALE_${r.scale.toUpperCase()}, ${r.step}.0 }, /* ${r.symbol} ${r.unit} */`).join('\n')}
};

${emitHintsC(K, d.hints ?? [])}
${emitPanelRows(d)}
/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
${ports.filter((p) => !p.param && !p.meter).map((p) => `#define OMX_${K}_LV2_PORT_${macro(p.symbol)} ${p.index}u`).join('\n')}
#define OMX_${K}_LV2_PORT_FIRST_PARAM ${firstParam}u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_${K}_LV2_PORT_FIRST_METER ${ports.find((p) => p.meter)?.index ?? ports.length}u
#define OMX_${K}_LV2_PORT_COUNT ${ports.length}u

#endif /* OMX_${K}_PARAMS_H */
`;
}

const ttlNum = (n) => (Number.isInteger(n) ? `${n}` : `${n}`);
const ttlStr = (s) => `"${s.replace(/\\/g, '\\\\').replace(/"/g, '\\"')}"`;

/** The manifest: the plugin, and each factory preset of presets/<clap id>/ (its pset:Preset, in the
 * presets.ttl tools/gen-presets.mjs writes from the same files; docs/presets.md). */
export function emitManifest(d, { modgui = false } = {}) {
  const presets = d.clap?.id ? presetsOf(d.clap.id) : [];
  return `${TTL_BANNER}# GENERATED by tools/gen.mjs from plugins/${d.stem}/${d.stem}.decl.json — DO NOT EDIT BY HAND.
@prefix lv2:  <http://lv2plug.in/ns/lv2core#> .
${presets.length ? '@prefix pset: <http://lv2plug.in/ns/ext/presets#> .\n' : ''}@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> .

<${d.lv2.uri}>
    a lv2:Plugin ;
    lv2:binary <${d.stem}.so> ;
    rdfs:seeAlso ${modgui ? '<modgui.ttl> , ' : ''}<${d.stem}.ttl> .
${presetsManifest(d.lv2.uri, presets)}`;
}

/** What a face states of parameter `i` beyond the declaration: its descriptor control's unit (a ratio's `ratio`, which
 * its link resolves to "") and meaning, for a plugin described in omx-contract's words; the declared unit otherwise. */
const faceUnit = (d, i) => travelUnit(d.params[i].unit, d.descriptor?.controls[i]?.travel?.unit);
const faceDesignation = (d, i) => lv2Designation(d.descriptor?.controls[i]?.meaning);

export function emitPluginTtl(d) {
  const port = (p) => {
    const lines = [`        a ${p.a} ;`, `        lv2:index ${p.index} ;`, `        lv2:symbol "${p.symbol}" ;`, `        lv2:name "${p.name}" ;`];
    if (p.param) {
      const q = p.param;
      lines.push(`        lv2:default ${ttlNum(q.def)} ;`, `        lv2:minimum ${ttlNum(q.min)} ;`, `        lv2:maximum ${ttlNum(q.max)} ;`);
      const choices = choicesOf(q);
      if (q.kind === 'integer') lines.push(`        lv2:portProperty lv2:integer${choices ? ' , lv2:enumeration' : ''} ;`);
      if (scaleOf(q) === 'log') lines.push('        lv2:portProperty pprops:logarithmic ;');
      if (q.kind === 'toggle') lines.push('        lv2:portProperty lv2:toggled ;');
      const i = d.params.indexOf(q);
      if (lv2UnitRef(faceUnit(d, i))) lines.push(`        units:unit ${lv2UnitRef(faceUnit(d, i))} ;`);
      if (faceDesignation(d, i)) lines.push(`        lv2:designation ${faceDesignation(d, i)} ;`);
      if (choices) {
        const points = choices.map((c) => `            [ rdfs:label ${ttlStr(c.label)} ; rdf:value ${c.value} ]`);
        lines.push(`        lv2:scalePoint\n${points.join(' ,\n')} ;`);
      }
      const group = groupOf(d, q.symbol);
      if (group) lines.push(`        pg:group <${groupUri(d, group)}> ;`);
    } else if (p.sidechain) {
      // optional: a host that routes nothing to the key gets the effect's own detector
      lines.push('        lv2:portProperty lv2:isSideChain , lv2:connectionOptional ;');
    } else if (p.enabled) {
      lines.push('        lv2:default 1 ;', '        lv2:minimum 0 ;', '        lv2:maximum 1 ;', '        lv2:designation lv2:enabled ;', '        lv2:portProperty lv2:toggled ;');
    } else if (p.latency) {
      lines.push('        lv2:designation lv2:latency ;', '        lv2:portProperty lv2:integer ;', '        units:unit units:frame ;');
    } else if (p.meter) {
      lines.push(`        lv2:minimum ${ttlNum(p.meter.min)} ;`, `        lv2:maximum ${ttlNum(p.meter.max)} ;`);
      if (lv2UnitRef(p.meter.unit)) lines.push(`        units:unit ${lv2UnitRef(p.meter.unit)} ;`);
    } else if (p.time) {
      // the host's transport: time:Position objects, read for their time:beatsPerMinute; optional, no tempo without it
      lines.push('        atom:bufferType atom:Sequence ;', '        atom:supports time:Position ;', '        lv2:designation lv2:control ;', '        lv2:portProperty lv2:connectionOptional ;');
    }
    return `    [\n${lines.join('\n').replace(/ ;$/, '')}\n    ]`;
  };
  return `${TTL_BANNER}# GENERATED by tools/gen.mjs from plugins/${d.stem}/${d.stem}.decl.json — DO NOT EDIT BY HAND.
${tempoOf(d) ? '@prefix atom:  <http://lv2plug.in/ns/ext/atom#> .\n' : ''}@prefix doap:  <http://usefulinc.com/ns/doap#> .
@prefix foaf:  <http://xmlns.com/foaf/0.1/> .
@prefix lv2:   <http://lv2plug.in/ns/lv2core#> .
${hintPrefix(d, (p) => p !== 'xsd')}${d.params.some((q, i) => faceDesignation(d, i)?.startsWith('param:')) ? '@prefix param: <http://lv2plug.in/ns/ext/parameters#> .\n' : ''}${d.panel ? '@prefix pg:    <http://lv2plug.in/ns/ext/port-groups#> .\n' : ''}${d.params.some((q) => scaleOf(q) === 'log') ? '@prefix pprops: <http://lv2plug.in/ns/ext/port-props#> .\n' : ''}${d.params.some((q) => choicesOf(q)) ? '@prefix rdf:   <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n' : ''}@prefix rdfs:  <http://www.w3.org/2000/01/rdf-schema#> .
${tempoOf(d) ? '@prefix time:  <http://lv2plug.in/ns/ext/time#> .\n' : ''}@prefix units: <http://lv2plug.in/ns/extensions/units#> .
${tempoOf(d) ? '@prefix urid:  <http://lv2plug.in/ns/ext/urid#> .\n' : ''}${hintPrefix(d, (p) => p === 'xsd')}
<${d.url}>
    a doap:Project ;
    doap:name "omx-plugins" ;
    doap:maintainer [ a foaf:Person ; foaf:name "Pau Aliagas" ; foaf:homepage <https://github.com/linuxnow> ] .

<${d.lv2.uri}>
    a lv2:Plugin , ${d.lv2.class} ;
    doap:name ${ttlStr(d.name)} ;
    doap:license <https://www.gnu.org/licenses/gpl-3.0.html> ;
    lv2:project <${d.url}> ;
    lv2:minorVersion ${d.version.split('.')[1]} ;
    lv2:microVersion ${d.version.split('.')[2]} ;
    rdfs:comment ${ttlStr(d.description)} ;
    lv2:optionalFeature lv2:hardRTCapable ;
${tempoOf(d) ? '    lv2:optionalFeature urid:map ;\n' : ''}${hintTriples(d.hintVocabulary, d.hints ?? []).map((l) => `${l}\n`).join('')}    lv2:port
${lv2Ports(d).map(port).join(' ,\n')} .
${portGroupsOf(d).length ? `
# lv2:displayPriority on a port group is the term Zynthian reads to order a plugin's groups (zynthian_lv2.py), and the
# one FreeMixer/omx-plugins#257 rules; LV2 defines none in lv2core (port-props' pprops:displayPriority is a port's), so
# it is stated here, for a validator, as what it is.
lv2:displayPriority
    a <http://www.w3.org/1999/02/22-rdf-syntax-ns#Property> ;
    rdfs:label "display priority" ;
    rdfs:comment "A port group's rank among its plugin's groups, higher first, as Zynthian reads it." .
` : ''}${portGroupsOf(d).map((g) => `
<${groupUri(d, g)}>
    a pg:Group ;
    lv2:symbol "${g.key}" ;
    lv2:name ${ttlStr(g.label)} ;
    lv2:displayPriority ${g.priority} ;
    rdfs:label ${ttlStr(g.label)} .
`).join('')}${customUnitsOf(d).map((name) => `
<urn:openmixer:unit#${name}>
    a units:Unit ;
    rdfs:label ${ttlStr(LV2_CUSTOM_UNITS[name].label)} ;
    units:render ${ttlStr(LV2_CUSTOM_UNITS[name].render)} ;
    units:symbol ${ttlStr(LV2_CUSTOM_UNITS[name].symbol)} .
`).join('')}`;
}

/** The @prefix lines of the stated resource hints (tools/resource-hints.mjs) that `pick` keeps, in the TTL's column. */
const hintPrefix = (d, pick) => hintPrefixes(d.hintVocabulary, d.hints ?? []).filter(([p]) => pick(p)).map(([p, ns]) => `@prefix ${`${p}:`.padEnd(6)} <${ns}> .\n`).join('');

/** The plugin-defined units this plugin's ports use, in a fixed order. */
const customUnitsOf = (d) => Object.keys(LV2_CUSTOM_UNITS).filter((name) =>
  [...d.params.map((q, i) => faceUnit(d, i)), ...metersOf(d).map((m) => m.unit)].some((u) => lv2UnitRef(u) === `<urn:openmixer:unit#${name}>`));

/** A control's group as a CLAP module path (plugin-hostd's group-path, written: the labels joined by "/"). */
const moduleOf = (d, i) => (d.descriptor?.controls[i]?.group ?? []).join('/');

/** A parameter's LV2 port group (pg:Group): the panel section it is drawn in, or the section's subgroup holding it
 * (tools/descriptor.mjs portGroupsOf). */
const groupOf = (d, symbol) => portGroupsOf(d).find((g) => g.controls.includes(symbol));
const groupUri = (d, section) => `${d.lv2.uri}#${section.key}`;

// ---- a plugin generated from its instance face (`binding: instance`) ---------------------------

const TEMPLATES = join(ROOT, 'recipes', 'templates', 'plugin');
const template = (name) => readFileSync(join(TEMPLATES, name), 'utf8');
/** A template rendered INTO another file: its own SPDX header dropped, the host file already carries one. */
const partial = (name) => template(name).replace(BANNER, '');

/** The C float literal of a number: always with a point, never an exponent a float cannot hold. */
const cfloat = (n) => {
  const t = `${Number(n.toPrecision(9))}`;
  return `${/[.e]/.test(t) ? t : `${t}.0`}f`;
};

/** How a catalogue row and section read a resolved parameter: its range and its default, in words. */
export function paramRow(p) {
  const unit = p.unit ? ` ${p.unit}` : '';
  if (p.kind === 'toggle') return { name: p.name, range: 'off / on', default: p.def ? 'on' : 'off' };
  const choices = choicesOf(p);
  if (choices) return { name: p.name, range: choices.map((c) => c.label).join(' / '), default: choices.find((c) => c.value === p.def)?.label ?? `${p.def}` };
  return { name: p.name, range: `${p.min} to ${p.max}${unit}${p.kind === 'integer' ? ', whole steps' : ''}`, default: `${p.def}${unit}` };
}

/** The manual's own words around the numbers: each `{range:<symbol>}`, `{default:<symbol>}`,
 * `{min:<symbol>}` and `{max:<symbol>}` in a line of `manual` reads that resolved parameter (its
 * range and default as the parameter table words them, or its bare limit), so the catalogue never
 * retypes a number omx-contract or the declaration holds. A symbol no parameter has, or any other
 * `{word:...}`, is refused. A `summary` reads the same words (`summaryOf`). */
export function fillManual(d, line, what = 'manual') {
  const out = line.replace(/\{(range|default|min|max):([A-Za-z][A-Za-z0-9_]*)\}/g, (_m, what, symbol) => {
    const p = d.params.find((q) => q.symbol === symbol);
    if (!p) throw new Error(`plugins/${d.stem}: its ${what} reads '${symbol}', which is none of its parameters`);
    return what === 'min' || what === 'max' ? `${p[what]}` : paramRow(p)[what];
  });
  const left = out.match(/\{[a-z]+:[^}]*\}/);
  if (left) throw new Error(`plugins/${d.stem}: its ${what}'s ${left[0]} is none of {range|default|min|max:<symbol>}`);
  return out;
}

/** The catalogue's words for a plugin: its `summary` with the numbers it reads from the declaration filled in
 * (`up to {max:fxDelayTime} ms`), else its description as the descriptor states it. */
export function summaryOf(d) {
  return d.summary === undefined ? d.description : fillManual(d, d.summary, 'summary');
}

/** The view every plugin template renders from. */
export function templateView(d) {
  return {
    stem: d.stem, kernel: d.kernel, face: d.face ?? d.kernel, K: d.kernel.toUpperCase(), Kernel: d.kernel.replace(/(^|_)([a-z])/g, (_m, _u, c) => c.toUpperCase()),
    name: d.name, uri: d.lv2.uri, clapId: d.clap.id, description: d.description,
    omxdspMin: packagingRange('omx-dsp-devel', d.tree ?? ROOT).floor, panel: Boolean(d.panel), params: d.params.map(paramRow),
    oracles: (d.oracles ?? []).map((oracle) => ({ oracle })),
  };
}

/** A templated file with the GENERATED line under its SPDX header (`//` for C, `#` for make). */
function withBanner(text, d, comment) {
  const lines = text.split('\n');
  const at = lines.findIndex((l) => l.startsWith(`${comment} Copyright`)) + 1;
  lines.splice(at, 0, `${comment} GENERATED by tools/gen.mjs from plugins/${d.stem}/${d.stem}.decl.json — DO NOT EDIT BY HAND.`);
  return lines.join('\n');
}

/**
 * The block plan of the identity test, three parts:
 *
 *   1. MOVES: twelve uneven blocks, fixed in frames, every parameter moved across its travel between
 *      them, the bypass toggled.
 *   2. RELEASE: at the defaults, a full-scale burst and then a tail that falls from full scale onto
 *      a quiet bed (about -60 dBFS, under any gate's default threshold, each threshold crossed at its
 *      own time), both in milliseconds so every rate gets the same time. The tail
 *      lasts the sum of every time travel's default (ms or s): the longest release or decay, with
 *      the predelay, hold or window that comes before it (a gated reverb holds 120 ms, then releases),
 *      and at least TAIL_FLOOR_MS. A dynamics kernel engages on the burst and releases in the tail, so
 *      its release is seen.
 *   3. CHOICES: for every choice (a set's values, or a toggle), each of its values in turn, held over
 *      its own burst and two tails: every other parameter at its default for the first, stepped once
 *      for the second. A control that is read only under one value of a choice (a reverb's plate
 *      depth under the plate algorithm) is then in the plan for long enough to be seen.
 *
 * A parameter whose contract control `rearms` (`controls[i].rearms`: changing it re-arms the kernel's
 * state) is held at its default in every block: moving it would test the re-arm, not the identity
 * of the faces and the kernel. The sabotage arm still moves it, by one step.
 *
 * A row is `frames, ms, signal, {values}, bypass`. `ms` > 0 means the block lasts that long at every
 * rate and `frames` is unused. `signal` is 0 for the moving stimulus, 1 for the full-scale burst and
 * 2 for the quiet tail.
 */
const PLAN_FRAMES = [64, 1, 333, 512, 17, 480, 129, 1024, 7, 2500, 600, 3000];
const PLAN_BYPASS = [0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0];
const PLAN_AT = ['def', 0.25, 1, 0, 0.75, 'def', 0.5, 0.9, 0.1, 0.6, 0.35, 'def'];
export const BURST_MS = 10;
export const TAIL_FLOOR_MS = 100;
const SIGNAL = { moves: 0, burst: 1, tail: 2 };

/** The quiet tail of a release section, ms: the sum of every time travel's default, at least TAIL_FLOOR_MS. */
export function tailMs(params) {
  const sum = params
    .filter((p) => p.unit === 'ms' || p.unit === 's')
    .reduce((t, p) => t + (p.unit === 's' ? p.def * 1000 : p.def), 0);
  return Math.max(TAIL_FLOOR_MS, Math.ceil(sum));
}

/** A choice's values (a set's ids by index, a toggle's 0 and 1), or undefined for a travel. */
const valuesOf = (p) => (p.kind === 'toggle' ? [0, 1] : choicesOf(p)?.map((c) => c.value));

/** A parameter's value at `at` (a fraction of its travel, or 'def'). */
function valueAt(p, at) {
  if (at === 'def') return p.def;
  const choices = choicesOf(p);
  if (choices) return choices[Math.round(at * (choices.length - 1))].value;
  const x = p.min + at * (p.max - p.min);
  return p.kind === 'integer' ? Math.round(x) : x;
}

/** One step of every parameter but those in `keep`: a travel to three quarters of its travel (a
 * quarter when that is its default), a choice to its next value, a toggle flipped. */
function stepped(params, v, keep) {
  return params.map((p, i) => {
    if (keep.has(i)) return v[i];
    const vals = valuesOf(p);
    if (vals) return vals[(vals.indexOf(v[i]) + 1) % vals.length];
    const x = valueAt(p, 0.75);
    return x === v[i] ? valueAt(p, 0.25) : x;
  });
}

export function oraclePlan(params, controls = [], { engaged = [], monitors = [], together = [] } = {}) {
  const held = new Set(params.map((_, i) => i).filter((i) => controls[i]?.rearms));
  // a chain's element switches: on in the release and choice sections, so each element is heard
  // there (an EQ switched off by the stepped row would hide every band's Q), and never stepped; a
  // declaration's engaged values ([index, value]) likewise, a band's threshold where every band is driven
  const at = new Map(engaged.map((e) => (Array.isArray(e) ? e : [e, 1])));
  const on = new Set(at.keys());
  // a monitor replaces the output while on (a band's listen): at its default where the others move or
  // are stepped, so what it would replace stays heard; its own section still holds each of its values
  const quiet = new Set(monitors);
  const row = (frames, ms, signal, v, bypass) => ({ row: `${frames}, ${cfloat(ms)}, ${signal}, {${v.map(cfloat).join(', ')}}, ${bypass}` });
  const moves = PLAN_FRAMES.map((frames, b) => {
    const v = params.map((p, i) => {
      if (held.has(i) || quiet.has(i)) return p.def;
      if (p.kind === 'toggle') return (b + i) % 2;
      return valueAt(p, PLAN_AT[(b + 2 * i) % PLAN_AT.length]);
    });
    return row(frames, 0, SIGNAL.moves, v, PLAN_BYPASS[b]);
  });
  const tail = tailMs(params);
  const defaults = params.map((p, i) => (on.has(i) ? at.get(i) : p.def));
  const release = [row(0, BURST_MS, SIGNAL.burst, defaults, 0), row(0, tail, SIGNAL.tail, defaults, 0)];
  // each choice heard at each of its values; a group run together (`together`: one per-band control of an element
  // with a band count, every band of it) takes each value on every member at once, so each band is heard at each
  // value in one set of rows, not one set per band
  const choices = [];
  const groupOf = new Map(together.flatMap((g) => g.map((i) => [i, g])));
  params.forEach((p, c) => {
    const vals = valuesOf(p);
    if (!vals || held.has(c)) return;
    const members = groupOf.get(c) ?? [c];
    if (members[0] !== c) return;
    for (const value of vals) {
      const v = defaults.map((d, i) => (members.includes(i) ? value : d));
      const moved = stepped(params, v, new Set([...held, ...on, ...quiet, ...members]));
      choices.push(row(0, BURST_MS, SIGNAL.burst, v, 0), row(0, tail, SIGNAL.tail, v, 0), row(0, tail, SIGNAL.tail, moved, 0));
    }
  });
  return [...moves, ...release, ...choices];
}

/** The declaration's latency checks in the oracle's shape (`latencyChecks` in the schema): each a row of
 * every parameter's value, the named ones set and the rest at their defaults, then what the faces
 * report (-1: where an impulse lands) and the set values in words. */
export function latencyRows(d) {
  return (d.latencyChecks ?? []).map((c, k) => {
    for (const [sym, x] of Object.entries(c.values)) {
      const p = d.params.find((q) => q.symbol === sym);
      if (!p) throw new Error(`plugins/${d.stem}: latencyChecks[${k}] sets '${sym}', which is no parameter`);
      if (!(p.min <= x && x <= p.max)) throw new Error(`plugins/${d.stem}: latencyChecks[${k}] sets '${sym}' to ${x}, outside ${p.min}..${p.max}`);
    }
    const v = d.params.map((p) => (p.symbol in c.values ? c.values[p.symbol] : p.def));
    const label = Object.entries(c.values).map(([sym, x]) => `${sym} ${x}`).join(', ') || 'the defaults';
    return { row: `{${v.map(cfloat).join(', ')}}, ${c.reports === 'impulse' ? -1 : c.reports}, ${JSON.stringify(label)}` };
  });
}

/** clap.meters/1's kind of each meter kind a declaration may name. */
const METER_KINDS = { gain_reduction: 'CLAP_METER_KIND_GAIN_REDUCTION', level: 'CLAP_METER_KIND_LEVEL' };
/** The shared faces' kind of each (include/omx_plugin_face.h): clap.meters/1's numbers, without CLAP's headers. */
const FACE_METER_KINDS = { gain_reduction: 'OMX_FACE_METER_KIND_GAIN_REDUCTION', level: 'OMX_FACE_METER_KIND_LEVEL' };
/** The most values a plugin's meters carry together: include/omx_meter_ring.h's OMX_METER_RING_MAX. */
const METER_VALUES_MAX = 16;
/** A C double literal: always with a point. */
const cdouble = (n) => (typeof n === 'string' || /[.e]/.test(`${n}`) ? `${n}` : `${n}.0`);

/** The declaration's meters in the faces' shape (`meters` in the schema, docs/meters.md): each its CLAP
 * id (its index), its name, its kind, its channels and where they start among the plugin's meter
 * values (the order the face publishes them in, one block at a time), and its display range. A
 * gain reduction reads the instance field it names, one channel, over the range omx-contract declares for the kernel's `readout`
 * of that name (OMX_<KERNEL>_READOUT_<readout>_MIN/_MAX, read by the compiler); a level is the stereo pair's peak in dBFS at the plugin's `input` or
 * `output`, its range as declared. */
export function meterRows(d, face) {
  const taken = new Set([...d.params.map((p) => p.symbol), ...FIXED_PORTS]);
  const K = (d.face ?? d.kernel);
  let offset = 0;
  const rows = (d.meters ?? []).map((m, k) => {
    const where = `plugins/${d.stem}: meters[${k}]`;
    if (!(m.kind in METER_KINDS)) throw new Error(`${where}: kind '${m.kind}' is none of ${Object.keys(METER_KINDS).join(', ')}`);
    if (!/^[A-Za-z][A-Za-z0-9_]*$/.test(m.id ?? '')) throw new Error(`${where}: id '${m.id}' is not a symbol`);
    if (taken.has(m.id)) throw new Error(`${where}: id '${m.id}' is taken`);
    taken.add(m.id);
    // a reading of the kernel's: a gain reduction, or a level the kernel reads itself (its detector's)
    const reads = m.kind === 'gain_reduction' || 'read' in m;
    const at = 'read' in m ? 'read' : 'field';
    const own = m.kind === 'gain_reduction' ? [at, 'readout'] : reads ? ['read', 'readout'] : ['source', 'min', 'max'];
    if ('linear' in m && !(m.kind === 'gain_reduction' && at === 'field' && m.linear === true)) throw new Error(`${where}: 'linear' is true on a gain reduction read from a 'field'`);
    for (const key of ['field', 'read', 'readout', 'source', 'min', 'max']) {
      if (own.includes(key) && !(key in m)) throw new Error(`${where}: a ${m.kind} meter names its '${key}'`);
      if (!own.includes(key) && key in m) throw new Error(`${where}: a ${m.kind} meter ${reads && m.kind === 'level' ? 'the kernel reads ' : ''}takes no '${key}'`);
    }
    let row;
    if (reads) {
      // where the reading is: a member of the instance, or the face's reading function, banded or not
      let core, ref;
      if (at === 'field') {
        if (!/^[a-z_][a-z0-9_]*(\.[a-z_][a-z0-9_]*)*$/.test(m.field)) throw new Error(`${where}: field '${m.field}' is not a member path`);
        // a kernel that keeps its block's deepest gain as a linear factor (the limiter's blk_gain) is read through omx-dsp's
        // own omx_gr_block_db(), the conversion every dynamics state's gr_min_db is made with
        core = m.linear ? `omx_gr_block_db(c->inst.${m.field})` : `c->inst.${m.field}`;
        ref = m.linear ? `omx_gr_block_db(inst.${m.field})` : `inst.${m.field}`;
      } else {
        const r = m.read.match(/^([a-z][a-z0-9_]*)(?:\(([0-9]+)\))?$/);
        if (!r) throw new Error(`${where}: read '${m.read}' is not <reading> or <reading>(<band>)`);
        const reading = face?.readings?.[r[1]];
        if (face && !reading) throw new Error(`${where}: omx-dsp's <omxdsp/fx/omx_${K}_instance.h> has no float omx_${K}_instance_${r[1]}(const ${face.type} *${r[2] === undefined ? '' : ', uint32_t'})`);
        if (reading && reading.banded !== (r[2] !== undefined)) throw new Error(`${where}: omx_${K}_instance_${r[1]} ${reading.banded ? 'takes' : 'takes no'} band index`);
        const call = (self) => `omx_${K}_instance_${r[1]}(${self}${r[2] === undefined ? '' : `, ${r[2]}u`})`;
        core = call('&c->inst');
        ref = call('&inst');
      }
      if (!/^[A-Z][A-Z0-9_]*$/.test(m.readout)) throw new Error(`${where}: readout '${m.readout}' is not a readout name`);
      const r = `OMX_${d.kernel.toUpperCase()}_READOUT_${m.readout}`;
      row = { channels: 1, min: `(double)${r}_MIN`, max: `(double)${r}_MAX`, core, ref };
    } else {
      if (m.source !== 'input' && m.source !== 'output') throw new Error(`${where}: source '${m.source}' is not input or output`);
      if (!(Number.isFinite(m.min) && Number.isFinite(m.max) && m.min < m.max)) throw new Error(`${where}: the range ${m.min}..${m.max} is not a range`);
      row = { channels: 2, min: m.min, max: m.max, core: '', ref: '' };
    }
    const out = {
      meterIndex: k, meterName: m.id, meterKind: METER_KINDS[m.kind], meterFaceKind: FACE_METER_KINDS[m.kind], meterReadCore: row.core, meterReadRef: row.ref,
      meterChannels: row.channels, meterOffset: offset, meterRight: offset + 1, meterMin: cdouble(row.min), meterMax: cdouble(row.max),
      isGr: m.kind === 'gain_reduction', isLinear: m.linear === true, isRead: reads, isIn: m.source === 'input', isOut: m.source === 'output',
      // where the shared face reads it (include/omx_plugin_face.h); what a bypassed kernel's stale reading
      // publishes: no reduction, or a level's display minimum
      meterSource: reads ? 'OMX_FACE_METER_READ' : m.source === 'input' ? 'OMX_FACE_METER_IN' : 'OMX_FACE_METER_OUT',
      meterBypassRef: m.kind === 'gain_reduction' ? '0.0' : `METERS[${k}].min`,
    };
    offset += row.channels;
    return out;
  });
  if (offset > METER_VALUES_MAX) throw new Error(`plugins/${d.stem}: its meters carry ${offset} values, more than the ${METER_VALUES_MAX} one read holds`);
  return rows;
}

/** The template's view of the meters: the rows, how many, how many values, which levels are read. */
function meterView(d, face) {
  const meters = meterRows(d, face);
  return {
    meters, metered: meters.length > 0, meterCount: meters.length,
    meterValues: meters.reduce((n, m) => n + m.meterChannels, 0),
    meterGr: meters.some((m) => m.isGr), meterIn: meters.some((m) => m.isIn), meterOut: meters.some((m) => m.isOut),
    meterLevel: meters.some((m) => m.isIn || m.isOut),
    meterLinear: meters.some((m) => m.isLinear), meterRead: meters.some((m) => m.isRead), meterKernelLevel: meters.some((m) => m.isRead && !m.isGr),
  };
}

/** The indices of the declaration's `monitors`, each a toggle or a choice among its parameters. */
export function monitorsOf(d) {
  return (d.monitors ?? []).map((sym) => {
    const i = d.params.findIndex((p) => p.symbol === sym);
    if (i < 0) throw new Error(`plugins/${d.stem}: monitors names '${sym}', no parameter`);
    if (!valuesOf(d.params[i])) throw new Error(`plugins/${d.stem}: monitor '${sym}' is a travel; a monitor is switched, a toggle or a choice`);
    return i;
  });
}

/** The declaration's `engaged` values, `[[index, value]]`, each inside its parameter's travel. */
export function engagedOf(d) {
  return Object.entries(d.engaged ?? {}).map(([sym, v]) => {
    const i = d.params.findIndex((p) => p.symbol === sym);
    if (i < 0) throw new Error(`plugins/${d.stem}: engaged names '${sym}', no parameter`);
    const p = d.params[i];
    if (!(typeof v === 'number' && p.min <= v && v <= p.max)) throw new Error(`plugins/${d.stem}: engaged '${sym}' at ${JSON.stringify(v)}, outside ${p.min}..${p.max}`);
    return [i, v];
  });
}

/** The parameters the plan holds at their default, for the oracle's comment: `[{ symbol }]`. */
const heldOf = (d) => d.params.filter((p, i) => d.controls?.[i]?.rearms).map((p) => ({ symbol: p.symbol }));

/** How many times the identity test runs each block of the plan in a row: 1, or for a keyed face 64,
 * so a detector has the time its declared release travel spans (up to seconds) to fall through
 * every threshold of the plan: a wrong ratio or release is then seen, not hidden by a gate held open. */
export const oracleStretch = (face) => (face.keyed ? 64 : 1);

/** One step of each parameter, the sabotage arm's move: 1 % of a travel, the next value of a choice. */
// an integer travel by its share too, in whole steps (one at least): +1 ms on a 2 s release moves no float
// pole; a time travel (ms, s) by 5 %: a slow detector's pole at 176.4 kHz is 1 - 1e-5, and 1 % of a release
// travel moves it by about two float steps, which can round to none
const stepShare = (p) => (p.unit === 'ms' || p.unit === 's' ? 0.05 : 0.01);
const stepOf = (p) => (p.kind === 'toggle' || choicesOf(p) ? 1 : p.kind === 'integer' ? Math.max(1, Math.round((p.max - p.min) * stepShare(p))) : (p.max - p.min) * stepShare(p));

/**
 * The controls of `kernel` a declaration (or a chain element) `holds`: no parameter, no port, each
 * passed at its kernel's declared default by the default's named macro, OMX_<ref>_DEFAULT (a face
 * argument the plugin does not expose yet). `[{ p: { symbol, held }, c: { name } }]`, each symbol
 * `<prefix>.<control>`, the errors pushed on `errors`; `who` names the holder in them.
 */
function heldControls(d, kernel, names, prefix, who, errors) {
  if (names === undefined) return [];
  if (!Array.isArray(names) || names.some((n) => typeof n !== 'string')) {
    errors.push(`${who}'s "held" lists control names`);
    return [];
  }
  const where = locateContract(d.tree ?? ROOT);
  if (!where.dir) {
    errors.push(`${who} holds ${names.join(', ')} and ${where.why}`);
    return [];
  }
  return names.flatMap((name) => {
    const c = kernelControls(where.dir, kernel).find((x) => x.name === name);
    if (!c) errors.push(`${who} holds '${name}', no control of the ${kernel} kernel`);
    else if (c.field || c.count) errors.push(`${who} holds '${name}', a field of ${c.ref} with no default macro of its own: pass it as "fixed"`);
    else return [{ p: { symbol: `${prefix}.${name}`, held: `OMX_${c.ref}_DEFAULT` }, c: { name } }];
    return [];
  });
}

/** The binding of `d` to its kernel's instance face, or a thrown error naming what is missing. */
/** The face arguments a single-face plugin does not expose: each `fixed` control at its value, then each `held`
 * one at its kernel's default, as `{ p, c }` pairs a binding appends to the parameters and their controls. */
export function instanceExtras(d, errors) {
  const held = heldControls(d, d.face ?? d.kernel, d.held, d.stem, d.stem, errors);
  const fixed = fixedArgs(d, { id: d.kernel, kernel: d.face ?? d.kernel, fixed: d.fixed }, d.controls ?? [], errors);
  return [...fixed, ...held];
}

export function bindingOf(d) {
  const errors = [];
  // the controls of the face this plugin does not expose: each passed at its `fixed` value, or held at its kernel's default
  const extra = instanceExtras(d, errors);
  const b = faceBinding(omxdspInclude(), d.face ?? d.kernel, [...d.params, ...extra.map((f) => f.p)], [...(d.controls ?? []), ...extra.map((f) => f.c)]);
  errors.push(...b.errors);
  if (errors.length) throw new Error(`plugins/${d.stem}: binding: instance, but ${errors.join('; ')}`);
  return b;
}

/** A held argument's value at the call site: its default's macro, cast to the argument's type. */
const heldValue = (type, p) => `(${type})${p.held}`;

/** The files of a `binding: instance` plugin beyond generated/: path relative to the plugin dir -> text. */
export function generateInstance(d) {
  const { face, binding, renames } = bindingOf(d);
  if (face.keyed && !d.sidechain) throw new Error(`plugins/${d.stem}: omx_${d.kernel}_instance_run takes a key; the declaration names its port in "sidechain"`);
  if (!face.keyed && !face.keyable && d.sidechain) throw new Error(`plugins/${d.stem}: declares a "sidechain", and omx_${d.kernel}_instance_run takes no key (nor has it a run_keyed)`);
  // keyed: the plugin's key port reaches the face, through run (the key first) or run_keyed (the
  // mono key on key_l, key_r NULL)
  const keyed = face.keyed || (face.keyable && Boolean(d.sidechain));
  if (tempoOf(d) && !face.tempo) throw new Error(`plugins/${d.stem}: its descriptor needs.tempo, and omx-dsp's face defines no omx_${d.face ?? d.kernel}_instance_set_tempo(${face.type} *, float bpm)`);
  const K = d.kernel.toUpperCase();
  const macro_ = (sym) => `OMX_${K}_PARAM_${macro(sym)}`;
  // a fixed control is its value, a parameter its port's
  const one = (type, p) => (p.fixed !== undefined ? (type === 'int' ? `${p.fixed}` : cfloat(p.fixed)) : type === 'int' ? `(int)lrintf(values[${macro_(p.symbol)}])` : `values[${macro_(p.symbol)}]`);
  // a per-band array: a compound literal of its parameters, in declaration order
  const value = (b) => (b.extent ? `(const ${b.elem}[${b.extent}]){${b.params.map((p) => one(b.elem, p)).join(', ')}}` : b.param.held ? heldValue(b.type, b.param) : one(b.type, b.param));
  const lat = (self) => (face.latency.fn ? `${face.latency.fn}(${self})` : face.latency.macro);
  const view = {
    ...templateView(d),
    faceType: face.type, srType: face.srType,
    binding: binding.map((b) => ({ arg: b.arg, value: value(b) })),
    arrays: binding.filter((b) => b.extent).map((b) => ({ arg: b.arg, count: b.params.length, extent: b.extent })),
    renames: renames.map((b) => ({ arg: b.arg, rename: b.rename })),
    heldArgs: binding.filter((b) => b.param.held).map((b) => ({ arg: b.arg, at: b.param.held })),
    fixed: Object.entries(d.fixed ?? {}).map(([name, v]) => ({ control: name, at: JSON.stringify(v) })),
    latency: lat('&c->inst'), refLatency: lat('&inst'),
    keyed, keyPort: keyed ? macro(d.sidechain.symbol) : '', keySymbol: keyed ? d.sidechain.symbol : '',
    runFn: keyed && !face.keyed ? 'run_keyed' : 'run', keyTail: keyed && !face.keyed ? ', NULL' : '',
    plan: oraclePlan(d.params, d.controls ?? [], { monitors: monitorsOf(d), engaged: engagedOf(d) }), held: heldOf(d),
    engaged: engagedOf(d).map(([i, v]) => ({ symbol: d.params[i].symbol, value: v })), monitors: monitorsOf(d).map((i) => ({ symbol: d.params[i].symbol })), stretch: d.stretch ?? oracleStretch(face), liveStretch: d.liveStretch ?? false, spanStretch: d.stretch !== undefined, noSpan: d.stretch === undefined, steps: d.params.map((p) => cfloat(d.sabotageStep?.[p.symbol] ?? stepOf(p))).join(', '),
    chain: false, tempo: tempoOf(d),
    latencyChecks: latencyRows(d), measuresLatency: Boolean(d.latencyChecks),
    ...meterView(d, face),
  };
  return {
    Makefile: withBanner(render(template('Makefile.tmpl'), view), d, '#'),
    [`omx_${d.kernel}_clap.c`]: withBanner(render(template('clap.c.tmpl'), view), d, '//'),
    [`omx_${d.kernel}_lv2.c`]: withBanner(render(template('lv2.c.tmpl'), view), d, '//'),
    [`omx_${d.kernel}_core.h`]: render(template('core.h.tmpl'), { ...view, vtable: render(partial('core-vtable.h.tmpl'), view) }),
    [`test/${d.kernel}-oracle.c`]: render(template('oracle.c.tmpl'), view),
  };
}

/** The value a chain element's `fixed` control is passed at: a set's id as its index (a numeric
 * set's id as itself), a travel's number as given, `"default"` the control's declared default (a
 * travel that declares none is refused), `{ "defaultRef": "<SHEET>.<field>" }` the default a sheet
 * of omx-contract holds for it (EQ_PASS_FILTER_DEFAULTS.hpfFreqHz). */
function fixedValue(d, el, control, v) {
  const where = locateContract(d.tree ?? ROOT);
  if (!where.dir) throw new Error(`plugins/${d.stem}: element '${el.id}' fixes '${control.name}' and ${where.why}`);
  const as = { symbol: `${el.id}.${control.name}`, ref: control.ref, ...(control.field ? { field: control.field } : {}) };
  const e = findName(where.dir, el.kernel, control.ref).entry;
  if (v && typeof v === 'object') return resolveParam(where.dir, el.kernel, { ...as, defaultRef: v.defaultRef }).def;
  if (v === 'default') {
    const t = e?.kind === 'travels' ? (control.field ? e.value?.[control.field] : e.value) : undefined;
    if (t && t.default === undefined) throw new Error(`plugins/${d.stem}: element '${el.id}' fixes '${control.name}' at "default", and ${control.ref} declares no default; name the sheet that holds it, { "defaultRef": "<SHEET>.<field>" }`);
    return resolveParam(where.dir, el.kernel, as).def;
  }
  if (e?.kind === 'set') {
    if (e.value.every((x) => typeof x === 'number')) {
      if (!e.value.includes(v)) throw new Error(`plugins/${d.stem}: element '${el.id}' fixes '${control.name}' at ${JSON.stringify(v)}, not one of ${control.ref}'s ids ${JSON.stringify(e.value)}`);
      return v;
    }
    const k = e.value.indexOf(v);
    if (k < 0) throw new Error(`plugins/${d.stem}: element '${el.id}' fixes '${control.name}' at ${JSON.stringify(v)}, not one of ${control.ref}'s ids ${JSON.stringify(e.value)}`);
    return k;
  }
  if (typeof v !== 'number') throw new Error(`plugins/${d.stem}: element '${el.id}' fixes the travel '${control.name}' at ${JSON.stringify(v)}, not a number, "default" or { "defaultRef" }`);
  return v;
}

/** The values of a per-band control an element fixes, for the bands of its face it does not expose:
 * from band `exposed` (the element's own parameters of that control come first) to the face's
 * compiled count. `"default"` gives each band its own default by omx-contract's one default rule at
 * that count (its type, centre, gain and Q; a control the rule does not name, its declared default);
 * any other value is the one value for every band. */
function fixedBands(d, el, control, v, exposed) {
  const where = locateContract(d.tree ?? ROOT);
  if (!faceBandsOf(el)) throw new Error(`plugins/${d.stem}: element '${el.id}' fixes '${control.name}', a control taken once per band; it declares its "bands" (or "faceBands")`);
  const n = findName(where.dir, el.kernel, control.count).entry?.value?.[faceBandsOf(el)]?.max;
  if (!Number.isInteger(n)) throw new Error(`plugins/${d.stem}: element '${el.id}': ${control.count} has no '${faceBandsOf(el)}' with a max`);
  const rest = [...Array(n).keys()].slice(exposed);
  if (v !== 'default') return rest.map(() => fixedValue(d, el, control, v));
  const rule = eqDefaultBands(where.dir, n);
  const name = control.name.toLowerCase();
  return rest.map((i) => {
    const band = rule[i];
    const key = Object.keys(band).find((k) => k.toLowerCase() === name || k.toLowerCase().startsWith(name) || name.endsWith(k.toLowerCase()));
    return key === undefined ? fixedValue(d, el, control, 'default') : fixedValue(d, el, control, band[key]);
  });
}

/**
 * The controls a face is handed at a fixed value, never a port: a chain element's `fixed`, or an
 * instance plugin's own (`el` is then the plugin, its id its kernel): `[{ p, c }]`, each bound like a
 * parameter (`p.fixed` the value) to its control `c`. `controls` are the exposed parameters' controls;
 * a refusal is pushed to `errors`.
 */
function fixedArgs(d, el, controls, errors) {
  const where = locateContract(d.tree ?? ROOT);
  return Object.entries(el.fixed ?? {}).flatMap(([name, v]) => {
    const c = where.dir ? kernelControls(where.dir, el.kernel).find((x) => x.name === name) : undefined;
    if (!c) {
      errors.push(`element '${el.id}' fixes '${name}', no control of the ${el.kernel} kernel`);
      return [];
    }
    try {
      // a per-band control: one fixed value per band, bound as the face's array argument
      if (c.count) {
        const exposed = controls.filter((x) => x?.name === name).length;
        return fixedBands(d, el, c, v, exposed).map((x, k) => ({ p: { symbol: `${el.id}.${name}[${exposed + k}]`, fixed: x }, c: { name } }));
      }
      return [{ p: { symbol: `${el.id}.${name}`, fixed: fixedValue(d, el, c, v) }, c: { name } }];
    } catch (e) {
      errors.push(e.message);
      return [];
    }
  });
}

/**
 * Each element of a chain bound to its own kernel's instance face, exactly as a single plugin's
 * parameters are (bindFace, by name): `[{ el, face, binding, renames, fixed }]`, or a thrown error
 * naming every element that does not bind. An element's `fixed` controls (`{ control: id | number }`)
 * are the ones it does not expose (a keyed face's key source, in an element that is not the chain's
 * `key`): passed at that value, they bind like parameters and are never a port.
 */
export function chainBindings(d) {
  const inc = omxdspInclude();
  const where = locateContract(d.tree ?? ROOT);
  const errors = [];
  for (const el of d.chain) {
    const other = d.chain.find((o) => o !== el && o.kernel === el.kernel && faceBandsOf(o) !== faceBandsOf(el));
    if (other) errors.push(`elements '${el.id}' and '${other.id}' run one ${el.kernel} face, compiled once: they declare the same face band count ("faceBands", else "bands")`);
  }
  const out = d.chain.map((el) => {
    const idx = d.params.map((p, i) => i).filter((i) => d.params[i].element === el.id && d.params[i].own === undefined);
    const params = idx.map((i) => d.params[i]), controls = idx.map((i) => d.controls[i]);
    if (el.bands && where.dir) {
      const want = findName(where.dir, el.kernel, kernelControls(where.dir, el.kernel).find((x) => x.count)?.count ?? '').entry?.value?.[el.bands]?.max;
      for (const c of kernelControls(where.dir, el.kernel).filter((x) => x.count)) {
        const have = controls.filter((x) => x?.name === c.name).length;
        if (have && have !== want) errors.push(`element '${el.id}' exposes ${have} '${c.name}' parameters, and its "bands" '${el.bands}' counts ${want}`);
      }
    }
    const fixed = fixedArgs(d, el, controls, errors);
    const held = heldControls(d, el.kernel, el.held, el.id, `element '${el.id}'`, errors);
    const extra = [...fixed, ...held];
    const b = faceBinding(inc, el.kernel, [...params, ...extra.map((f) => f.p)], [...controls, ...extra.map((f) => f.c)]);
    for (const e of b.errors) errors.push(`element '${el.id}' (${el.kernel}): ${e}`);
    if (b.face && !b.face.keyed && d.key === el.id) errors.push(`"key" names '${el.id}', and omx_${el.kernel}_instance_run takes no key`);
    return { el, ...b };
  });
  if (errors.length) throw new Error(`plugins/${d.stem}: binding: chain, but ${errors.join('; ')}`);
  return out;
}

/** The files of a `binding: chain` plugin beyond generated/: path relative to the plugin dir -> text.
 * The faces and the Makefile are a single plugin's; the binding is the chain core (copy in to out
 * once, each element's face in place in the chosen order, no arithmetic of its own) and the oracle's
 * reference is the same chain of faces called directly. */
export function generateChain(d) {
  const bound = chainBindings(d);
  const K = d.kernel.toUpperCase();
  const macro_ = (sym) => `OMX_${K}_PARAM_${macro(sym)}`;
  const one = (type, p) => (p.held ? heldValue(type, p) : p.fixed !== undefined ? (type === 'int' ? `${p.fixed}` : cfloat(p.fixed)) : type === 'int' ? `(int)lrintf(values[${macro_(p.symbol)}])` : `values[${macro_(p.symbol)}]`);
  // a per-band control an element's band count gates: band k passes its own value while k is under the count, else 0
  // (off), so the face leaves it out of its bank
  const gate = (el, b, p, k, text) => {
    const n = bandCountOf(d, el);
    return n && b.control === n.gates && p.fixed === undefined ? `(${k}u < ${bandCountFn(d, el)}(values) ? ${text} : 0)` : text;
  };
  const valueOf = (el) => (b) => (b.extent ? `(const ${b.elem}[${b.extent}]){${b.params.map((p, k) => gate(el, b, p, k, one(b.elem, p))).join(', ')}}` : one(b.type, b.param));
  const lat = (b, self) => (b.face.latency.fn ? `${b.face.latency.fn}(${self})` : b.face.latency.macro);
  const keyed = d.key !== undefined;
  const elements = bound.map((b, index) => ({
    id: b.el.id, index, name: elementName(b.el), face: b.el.kernel, faceType: b.face.type, srType: b.face.srType,
    on: macro_(`${b.el.id}On`),
    binding: b.binding.map((x) => ({ arg: x.arg, value: valueOf(b.el)(x) })),
    arrays: b.binding.filter((x) => x.extent).map((x) => ({ arg: x.arg, count: x.params.length, extent: x.extent })),
    renames: b.renames.map((x) => ({ arg: x.arg, rename: x.rename })),
    fixed: (b.el.fixed ? Object.entries(b.el.fixed) : []).map(([name, v]) => ({ control: name, at: JSON.stringify(v) })),
    heldArgs: b.binding.filter((x) => x.param.held).map((x) => ({ arg: x.arg, at: x.param.held })),
    // a keyable face (the compressor's) runs keyed with no key, so its key filter, tilt and link act on its
    // own legs; idle, run_keyed is run bit for bit
    runFn: b.face.keyable ? 'run_keyed' : 'run',
    keyedFace: b.face.keyed || b.face.keyable, keyArg: b.face.keyable ? 'NULL, NULL' : b.face.keyed ? (d.key === b.el.id ? 'key' : 'NULL') : '',
    keyRef: b.face.keyable ? 'NULL, NULL' : b.face.keyed && d.key === b.el.id ? 'key ? key + at : NULL' : 'NULL',
    latency: lat(b, `&c->${b.el.id}`), refLatency: lat(b, `&${b.el.id}`),
  }));
  const rows = d.order === 'permutable' ? permutations(d.chain.length) : [[...d.chain.keys()]];
  const view = {
    ...templateView(d),
    chain: true, elements, faces: [...new Set(elements.map((e) => e.face))].map((face) => ({ face })),
    length: elements.length, orders: rows.length, orderRows: rows.map((r) => ({ row: r.join(', ') })),
    permutable: d.order === 'permutable', orderParam: d.order === 'permutable' ? macro_('order') : '',
    sequence: elements.map((e) => e.name).join(' > '),
    latency: elements.map((e) => `(float)(${e.latency})`).join(' + '), refLatency: elements.map((e) => `(float)(${e.refLatency})`).join(' + '),
    keyed, keyPort: keyed ? macro(d.sidechain.symbol) : '', keySymbol: keyed ? d.sidechain.symbol : '',
    // engaged where the others are held: each element's switch on, and each band count at its maximum, so every band
    // the release and choice sections switch on is heard there
    plan: oraclePlan(d.params, d.controls ?? [], {
      engaged: d.params.flatMap((p, i) => (p.own === 'switch' && p.element ? [i] : p.own === 'band-count' ? [[i, p.max]] : [])),
      together: bandGroups(d),
    }),
    held: heldOf(d), stretch: keyed ? 64 : 1, noSpan: true, steps: d.params.map((p) => cfloat(d.sabotageStep?.[p.symbol] ?? stepOf(p))).join(', '),
    latencyChecks: latencyRows(d), measuresLatency: Boolean(d.latencyChecks),
    // the single-face fields the shared templates read, empty for a chain
    faceType: '', srType: '', binding: [], arrays: [], renames: [], heldArgs: [],
  };
  return {
    Makefile: withBanner(render(template('Makefile.tmpl'), view), d, '#'),
    [`omx_${d.kernel}_clap.c`]: withBanner(render(template('clap.c.tmpl'), view), d, '//'),
    [`omx_${d.kernel}_lv2.c`]: withBanner(render(template('lv2.c.tmpl'), view), d, '//'),
    [`omx_${d.kernel}_core.h`]: render(template('chain-core.h.tmpl'), { ...view, vtable: render(partial('core-vtable.h.tmpl'), view) }),
    [`test/${d.kernel}-oracle.c`]: render(template('oracle.c.tmpl'), view),
  };
}

/** Every generated file of one plugin: path relative to the plugin dir -> text. */
export function generate(d) {
  const lv2 = `generated/${d.stem}.lv2`;
  return {
    [`generated/omx_${d.kernel}_params.h`]: emitParamsHeader(d),
    [`${lv2}/manifest.ttl`]: emitManifest(d, { modgui: Boolean(d.panel) }),
    [`${lv2}/${d.stem}.ttl`]: emitPluginTtl(d),
    ...(d.binding === 'instance' ? generateInstance(d) : {}),
    ...(d.binding === 'chain' ? generateChain(d) : {}),
    ...(d.descriptor ? { [descriptorFile(d.stem)]: descriptorText(d) } : {}),
  };
}

/** Every plugin's folder: each plugins/<stem>/ holding <stem>.decl.json, a base of variants standing
 * for its variants' folders (tools/variants.mjs). A test plugin's folder (`"role": "test"`,
 * tools/test-plugins.mjs) is none of them: testPluginDirs() lists those. */
export const pluginDirs = (root = ROOT) =>
  readdirSync(join(root, 'plugins'))
    .map((n) => join(root, 'plugins', n))
    .filter((p) => existsSync(join(p, `${basename(p)}.decl.json`)) && !isTestDir(p))
    .flatMap((p) => {
      const raw = JSON.parse(readFileSync(join(p, `${basename(p)}.decl.json`), 'utf8'));
      return raw.variants ? variantStems(raw).map((s) => join(root, 'plugins', s)) : [p];
    })
    .sort();

// ---- the shared files, generated from every plugin folder ---------------------------------------

/** A plugin ships (is built, packaged, listed) when its folder has a Makefile or generates one. */
export const ships = (d) => generatedBinding(d) || existsSync(join(d.dir, 'Makefile'));

/** Plugin order everywhere a list is generated: by stem, numbers by value (eq8 before eq16). */
const byStem = (a, b) => a.stem.localeCompare(b.stem, 'en', { numeric: true });

/** Each region's lines, from the shipped plugins' declarations. */
export const SHARED = {
  'docs/catalogue.md': {
    catalogue: (ds) => [
      '',
      '| Plugin | CLAP id | LV2 URI | What it does |',
      '|---|---|---|---|',
      ...ds.map((d) => `| **${d.name}** | \`${d.clap.id}\` | \`${d.lv2.uri}\` | ${summaryOf(d)} |`),
      '',
    ],
    sections: (ds) => ds.flatMap((d) => [
      '',
      `### ${d.name}`,
      '',
      ...(d.manual?.map((l) => fillManual(d, l)) ?? ['| Parameter | Range | Default |', '|---|---|---|', ...d.params.map(paramRow).map((r) => `| ${r.name} | ${r.range} | ${r.default} |`), '', "Plus the host's bypass."]),
    ]).concat(['']),
  },
  'packaging/omx-plugins.spec': {
    'clap-files': (ds) => ds.map((d) => `/usr/lib/clap/${d.stem}.clap`),
    'lv2-files': (ds) => ds.map((d) => `%{_libdir}/lv2/${d.stem}.lv2/`),
    // the test plugins (`"role": "test"`): omx-plugins-test, outside every CLAP search path (`make install-test`)
    'clap-test-files': (ds, ts = []) => ts.map((d) => `%{_libexecdir}/omx-plugins/test/${d.stem}.clap`),
  },
  '.github/workflows/ci.yml': {
    // the test plugins' ids too: static-table-host expects them after `--test`, and a hand-written
    // list in CI hides a new test plugin
    plugins: (ds, ts = []) => [
      `shorts="${ds.map((d) => d.stem.replace(/^omx-/, '')).join(' ')}"`,
      `kernels="${ds.map((d) => d.kernel).join(' ')}"`,
      `tests="${ts.map((d) => d.clap.id).join(' ')}"`,
      `teststems="${ts.map((d) => d.stem).join(' ')}"`,
    ],
  },
  // libomxplugins: each face's entry, defined by omx_<kernel>_clap.c, listed by its CLAP id
  'include/omx_clap_static.h': {
    'clap-entries': (ds) => ds.map((d) => `extern const clap_plugin_entry_t omx_clap_entry_${d.kernel};`),
    'clap-test-entries': (ds, ts = []) => ts.map((d) => `extern const clap_plugin_entry_t omx_clap_entry_${d.kernel};`),
  },
  'lib/omx_clap_static.c': {
    'clap-table': (ds) => ds.map((d) => `{"${d.clap.id}", &omx_clap_entry_${d.kernel}},`),
    // the test plugins (`"role": "test"`): linked and found by id, never listed with the shipped ones
    'clap-test-table': (ds, ts = []) => ts.map((d) => `{"${d.clap.id}", &omx_clap_entry_${d.kernel}},`),
  },
};

/** `text` with each `BEGIN GENERATED <name>` .. `END GENERATED <name>` region refilled; the inner
 * lines take the BEGIN line's indentation. A region missing, unclosed or unknown throws. */
export function fillRegions(file, text, producers, ds, ts = []) {
  const lines = text.split('\n');
  const out = [];
  const seen = new Set();
  for (let i = 0; i < lines.length; i++) {
    const m = lines[i].match(/^(\s*).*\bBEGIN GENERATED ([a-z][a-z0-9-]*)\b/);
    out.push(lines[i]);
    if (!m) continue;
    const [, indent, name] = m;
    if (!producers[name]) throw new Error(`${file}: line ${i + 1} opens a region '${name}' tools/gen.mjs does not fill`);
    const end = lines.findIndex((l, j) => j > i && new RegExp(`\\bEND GENERATED ${name}\\b`).test(l));
    if (end < 0) throw new Error(`${file}: region '${name}' (line ${i + 1}) is never closed`);
    out.push(...producers[name](ds, ts).map((l) => (l ? `${indent}${l}` : l)));
    out.push(lines[end]);
    seen.add(name);
    i = end;
  }
  const absent = Object.keys(producers).filter((n) => !seen.has(n));
  if (absent.length) throw new Error(`${file}: no region ${absent.map((n) => `'${n}'`).join(', ')}`);
  return out.join('\n');
}

/** Every shared file: path relative to the tree -> text. */
export function generateShared(root = ROOT, decls = pluginDirs(root).map((p) => loadDecl(p)), tests = testPluginDirs(root).map((p) => loadTestDecl(root, p))) {
  const ds = decls.filter(ships).sort(byStem);
  const ts = [...tests].sort(byStem);
  return Object.fromEntries(Object.entries(SHARED).map(([file, producers]) => [file, fillRegions(file, readFileSync(join(root, file), 'utf8'), producers, ds, ts)]));
}

/** A declaration's pinned port hints (its `portHints`, tools/port-hints.mjs) in the view's shape,
 * `{props, unit, points}` by port symbol in LV2 port order, then any symbol no port has (which the
 * hint check names as gone); undefined when it pins none. */
export function hintsOf(d) {
  if (!d.portHints) return undefined;
  const out = {};
  for (const sym of new Set([...lv2Ports(d).map((p) => p.symbol), ...Object.keys(d.portHints)])) {
    const h = d.portHints[sym];
    if (h) out[sym] = { props: h.props ?? [], unit: h.unit ?? null, points: h.points ?? [] };
  }
  return out;
}

/** The shared view the hint check reads: every plugin's pinned hints, from its declaration, by stem.
 * A build product (tools/test/port-hints.json, git-ignored), never hand-edited. */
export const HINTS_VIEW = 'tools/test/port-hints.json';
export function hintsView(root = ROOT, decls = pluginDirs(root).map((p) => loadDecl(p))) {
  const view = {};
  for (const d of decls) {
    const h = hintsOf(d);
    if (h) view[d.stem] = h;
  }
  return view;
}
export const hintsViewText = (root = ROOT, decls) => `${JSON.stringify(hintsView(root, decls), null, 2)}\n`;

function main(argv) {
  const check = argv.includes('--check');
  const dirs = argv.filter((a) => !a.startsWith('--'));
  let stale = 0;
  const put = (path, shown, text, how) => {
    // Every plugin runs in place: its oracle's in-place arm proves the output bit for bit the separate
    // buffers' one, so no bundle may tell a host otherwise.
    if (path.endsWith('.ttl') && /inPlaceBroken/.test(text))
      throw new Error(`gen: ${shown} declares lv2:inPlaceBroken; every plugin's oracle proves it runs in place (its in-place arm)`);
    const now = existsSync(path) ? readFileSync(path, 'utf8') : null;
    if (now === text) return;
    if (check) {
      console.error(`gen: STALE ${shown} (run \`${how}\`)`);
      stale++;
    } else {
      mkdirSync(dirname(path), { recursive: true });
      writeFileSync(path, text);
      console.log(`gen: wrote ${shown}`);
    }
  };
  const decls = (dirs.length ? dirs.filter((dir) => !isTestDir(dir)) : pluginDirs()).map((dir) => loadDecl(dir));
  const tests = (dirs.length ? dirs.filter((dir) => isTestDir(dir)) : testPluginDirs(ROOT)).map((dir) => loadTestDecl(ROOT, dir));
  for (const d of decls) {
    for (const [rel, text] of Object.entries(generate(d))) put(join(d.dir, rel), join('plugins', d.stem, rel), text, `make -C plugins/${d.stem} gen`);
  }
  for (const d of tests) {
    for (const [rel, text] of Object.entries(generateTest(d))) put(join(d.dir, rel), join('plugins', d.stem, rel), text, 'node tools/gen.mjs');
  }
  if (!dirs.length) for (const [rel, text] of Object.entries(generateShared(ROOT, decls, tests))) put(join(ROOT, rel), rel, text, 'node tools/gen.mjs');
  if (!dirs.length) {
    // the pin is the declaration's `portHints`: a pin file beside it would be a second home
    for (const n of readdirSync(join(ROOT, 'plugins')).sort()) {
      if (!existsSync(join(ROOT, 'plugins', n, 'port-hints.json'))) continue;
      console.error(`gen: EXTRA plugins/${n}/port-hints.json: the pin of the port hints is the declaration's "portHints"; fold it in and delete the file`);
      stale++;
    }
    const viewPath = join(ROOT, HINTS_VIEW);
    // absent is fine (it is built by `make hints`); present must be fresh, so a hand edit goes stale
    const text = hintsViewText(ROOT, decls), now = existsSync(viewPath) ? readFileSync(viewPath, 'utf8') : null;
    if (check && now !== null && now !== text) {
      console.error(`gen: STALE ${HINTS_VIEW} (run \`node tools/gen.mjs\`)`);
      stale++;
    } else if (!check && now !== text) writeFileSync(viewPath, text); // a build product, not a file the wizard commits
  }
  if (stale) process.exit(1);
  if (check) console.log(`gen: every generated file is fresh${dirs.length ? '' : ', the shared files too'}`);
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) main(process.argv.slice(2));
