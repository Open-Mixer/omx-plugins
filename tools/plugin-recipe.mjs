#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * plugin-recipe.mjs — THE PLUGIN RECIPE'S CHECKERS and the completeness test that runs them.
 *
 * recipes/plugin.recipe.json lists every artifact a complete plugin needs and every law it keeps;
 * each entry names one checker below and that checker's arguments. This module holds no list of
 * plugins and no list of artifacts: it runs the recipe's entries over every plugin directory under
 * plugins/ that carries a declaration, the old ones too.
 *
 *   node tools/plugin-recipe.mjs [--all] [--json] [--no-debt] [<stem> ...]     (`make completeness`)
 *
 * One line per owed artifact a plugin lacks, naming the recipe's wizard step and commit layer.
 * `--all` also prints what is not owed yet (with the rule) and every artifact present.
 *
 * The gaps owed today are listed in recipes/completeness-debt.json, each with who owes it. It is a
 * ratchet: a gap the debt does not hold fails, and so does a debt entry that is now satisfied (delete
 * it). `--no-debt` fails on every gap. Exit 0 the ratchet holds; 1 it does not; 2 the recipe itself
 * is broken (a rule or checker it names is missing).
 *
 * Also the template renderer and the path-to-layer map the wizard and the commit-plan check share.
 */
import { execFileSync } from 'node:child_process';
import { existsSync, readFileSync, readdirSync, statSync } from 'node:fs';
import { dirname, join, relative, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { descriptorFile, isWhole, viewOf } from './descriptor.mjs';
import { compareVersions, declKernels, declParams, findName, kernelExists, locateContract, packagingRange, paramKernel } from './omx-contract.mjs';
import { INDEXED_ROLES, ROLES } from './omx-panel.mjs';
import { validate } from './schema-validate.mjs';
import { omxdspInclude, render } from './template.mjs';
import { baseOf, expandVariant, variantStems } from './variants.mjs';

export { omxdspInclude, render };

export const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..');
export const RECIPE_PATH = 'recipes/plugin.recipe.json';

export const loadRecipe = (root = ROOT) => JSON.parse(readFileSync(join(root, RECIPE_PATH), 'utf8'));
export const loadSchema = (root, recipe) => JSON.parse(readFileSync(join(root, recipe.schema), 'utf8'));

// ---- the JSON Schema validator: tools/schema-validate.mjs, re-exported for the wizard ----------

export { validate };

// ---- facts of one plugin ------------------------------------------------------------------------

/** The plugin directories: every plugins/<stem>/ holding <stem>.decl.json, a test plugin's excepted. */
export function pluginStems(root = ROOT) {
  const dir = join(root, 'plugins');
  // a base of variants is no plugin: its variants are (tools/variants.mjs)
  return readdirSync(dir)
    .filter((n) => existsSync(join(dir, n, `${n}.decl.json`)))
    .flatMap((n) => {
      let raw;
      try {
        raw = JSON.parse(readFileSync(join(dir, n, `${n}.decl.json`), 'utf8'));
      } catch {
        return [n];
      }
      // a test plugin ("role": "test", tools/test-plugins.mjs) ships nothing the recipe asks for
      if (raw.role === 'test') return [];
      return raw.variants ? variantStems(raw) : [n];
    })
    .sort();
}

/** What the checkers fill placeholders with, read from the raw declaration (never resolved: a
 * declaration whose references cannot resolve still has its identity checked). */
export function pluginFacts(root, stem) {
  let file = join(root, 'plugins', stem, `${stem}.decl.json`);
  let decl;
  try {
    const base = existsSync(file) ? undefined : baseOf(join(root, 'plugins', stem));
    if (base) {
      // a variant's declaration is its base's, expanded (its band count read from omx-contract)
      file = base.file;
      const where = locateContract(root);
      if (!where.dir) throw new Error(`its variant ${stem} is expanded from omx-contract and ${where.why}`);
      decl = expandVariant(where.dir, base.decl, base.decl.variants.of.find((v) => v.stem === stem));
    } else {
      decl = JSON.parse(readFileSync(file, 'utf8'));
      // a plugin described in omx-contract's words: the declaration is its descriptor's view
      const described = join(root, 'plugins', stem, descriptorFile(stem));
      if (existsSync(described) && !isWhole(decl)) decl = viewOf(JSON.parse(readFileSync(described, 'utf8')), decl, file);
    }
  } catch (e) {
    return { stem, short: stem.replace(/^omx-/, ''), decl: undefined, error: `${relative(root, file)}: ${e.message}` };
  }
  return {
    stem,
    short: stem.replace(/^omx-/, ''),
    kernel: decl.kernel,
    name: decl.name,
    noun: decl.noun ?? stem.replace(/^omx-/, ''),
    uri: decl.lv2?.uri,
    clapId: decl.clap?.id,
    decl,
  };
}

/** `{key}` placeholders filled from facts; an unknown key stays as it is. */
export const fill = (template, facts) =>
  template.replace(/\{(\w+)\}/g, (m, k) => (facts[k] === undefined || facts[k] === null ? m : String(facts[k])));

const escapeRe = (s) => s.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');

// ---- the rules that say when an artifact is owed -------------------------------------------------

export const RULES = {
  always: () => true,
  // a chain is generated whole, as an instance plugin is
  instance: (_root, _recipe, facts) => facts.decl?.binding === 'instance' || facts.decl?.binding === 'chain',
  panel: (_root, _recipe, facts) => Boolean(facts.decl?.panel) || facts.decl?.binding === 'instance' || facts.decl?.binding === 'chain',
  byReference: (root, recipe) => compareVersions(packagingRange('omx-contract-devel', root).floor, recipe.contract.byReferenceSince) >= 0,
};

// ---- the checkers: one per artifact kind ---------------------------------------------------------

const ok = (detail) => ({ ok: true, detail });
const missing = (detail) => ({ ok: false, detail });

const text = (root, file) => readFileSync(join(root, file), 'utf8');
const has = (root, file) => existsSync(join(root, file));

/** Every file under `dir` (relative to root), recursively, sorted. */
function walk(root, dir) {
  const abs = join(root, dir);
  if (!existsSync(abs)) return [];
  return readdirSync(abs).sort().flatMap((n) => {
    const rel = join(dir, n);
    return statSync(join(root, rel)).isDirectory() ? walk(root, rel) : [rel];
  });
}

/** C sources of a plugin that are its own (not generated, not tests). */
const ownSources = (root, dir) => walk(root, dir).filter((f) => /\.(c|h)$/.test(f) && !f.includes('/generated/') && !f.includes('/test/'));

/** The quoted includes a source reaches, following the plugin's own headers (same directory). */
function localIncludes(root, file, seen = new Set()) {
  for (const m of stripC(text(root, file)).matchAll(/#include\s*"([^"]+)"/g)) {
    if (seen.has(m[1])) continue;
    seen.add(m[1]);
    const local = join(dirname(file), m[1]);
    if (has(root, local)) localIncludes(root, local, seen);
  }
  return seen;
}

const stripC = (t) => t.replace(/\/\*[\s\S]*?\*\//g, ' ').replace(/\/\/[^\n]*/g, '');

/** The recipe-lines of a make target (the lines after `name:` that start with a tab). */
function makeRecipe(makefile, target) {
  const lines = makefile.split('\n');
  const at = lines.findIndex((l) => new RegExp(`^${escapeRe(target)}\\s*:`).test(l));
  if (at < 0) return undefined;
  const out = [];
  for (let i = at + 1; i < lines.length && lines[i].startsWith('\t'); i++) out.push(lines[i]);
  return out.join('\n');
}

/**
 * What a plugin's tests run: its `test` target's recipe and its `test-heavy` one's together. make test is the fast set
 * a package build runs and make test-heavy the set that runs the built faces through a host (tools/test/test-split.json
 * lists every step under one of them); a step is run when either target runs it. Undefined when neither target exists.
 */
function testRecipes(makefile) {
  const parts = ['test', 'test-heavy'].map((t) => makeRecipe(makefile, t)).filter((r) => r !== undefined);
  return parts.length ? parts.join('\n') : undefined;
}

/** The lines of `file` between the first line matching `from` and the next matching `to`. */
function region(t, from, to) {
  if (!from) return t;
  const lines = t.split('\n');
  const a = lines.findIndex((l) => new RegExp(from).test(l));
  if (a < 0) return undefined;
  let b = lines.findIndex((l, i) => i > a && new RegExp(to).test(l));
  if (b < 0) b = lines.length;
  return lines.slice(a, b).join('\n');
}

/** A glob (`*`, `?`, `**`) as a RegExp over a relative path. */
export function globRe(glob) {
  let re = '';
  for (let i = 0; i < glob.length; i++) {
    const c = glob[i];
    if (c === '*' && glob[i + 1] === '*') {
      re += '.*';
      i++;
    } else if (c === '*') re += '[^/]*';
    else if (c === '?') re += '[^/]';
    else re += escapeRe(c);
  }
  return new RegExp(`^${re}$`);
}

/** Where `$defs.familyRoles` leaves omx-panel's table of roles (its ROLES, the one table): a family it has or lacks,
 * and a family's roles that are not the table's. The schema lists the names so a declaration is checked with no
 * omx-panel to read; the units each role accepts stay omx-panel's. */
export function familyRolesErrors(schema, table = ROLES) {
  const errs = [];
  const tables = schema.$defs.familyRoles.properties;
  const same = (a, b) => [...a].sort().join() === [...b].sort().join();
  if (!same(Object.keys(tables), Object.keys(table))) errs.push(`schema: $defs.familyRoles families (${Object.keys(tables).join(', ')}) are not omx-panel's (${Object.keys(table).join(', ')})`);
  for (const [family, roles] of Object.entries(table)) {
    const own = tables[family]?.enum ?? [];
    if (tables[family] && !same(own, Object.keys(roles))) errs.push(`schema: $defs.familyRoles.${family} (${own.join(', ')}) is not omx-panel's roles of it (${Object.keys(roles).join(', ')})`);
  }
  return errs;
}

/** Every way the `panel` block leaves the console's vocabulary or names no declared parameter: its
 * family outside `$defs.family`, a role outside its family's (`$defs.familyRoles`), a list of symbols
 * for a role that is not indexed, a family other than none with no role, a section, role or widget
 * naming no parameter, and a parameter drawn in two sections. The families of `$defs.family` and of
 * `$defs.familyRoles` are one list. */
export function panelErrors(schema, decl, symbols) {
  const errs = [];
  const p = decl.panel;
  if (!p) return errs;
  const families = schema.$defs.family.enum;
  const tables = schema.$defs.familyRoles.properties;
  const indexed = new Set(INDEXED_ROLES);
  if (families.join() !== Object.keys(tables).join()) errs.push(`schema: $defs.family (${families.join(', ')}) and $defs.familyRoles (${Object.keys(tables).join(', ')}) are not one list`);
  errs.push(...familyRolesErrors(schema));
  const drawn = new Set();
  for (const s of p.sections ?? []) {
    for (const c of s.controls) {
      if (!symbols.has(c)) errs.push(`/panel/sections: section '${s.key}' names '${c}', no parameter`);
      else if (drawn.has(c)) errs.push(`/panel/sections: '${c}' is in two sections`);
      drawn.add(c);
    }
  }
  if (families.includes(p.family)) {
    const own = tables[p.family]?.enum ?? [];
    const roles = Object.entries(p.roles ?? {});
    if (p.family !== 'none' && roles.length === 0) errs.push(`/panel/roles: family '${p.family}' names none of its roles (${own.join(', ')})`);
    for (const [role, sym] of roles) {
      if (!own.includes(role)) errs.push(`/panel/roles/${role}: not a role of family '${p.family}'${own.length ? ` (${own.join(', ')})` : ''}`);
      if (Array.isArray(sym) && !indexed.has(role)) errs.push(`/panel/roles/${role}: names a list, and only an indexed role (${[...indexed].join(', ')}) repeats by band`);
      for (const x of [sym].flat()) if (!symbols.has(x)) errs.push(`/panel/roles/${role}: '${x}' is no parameter`);
    }
  }
  for (const sym of Object.keys(p.widgets ?? {})) if (!symbols.has(sym)) errs.push(`/panel/widgets/${sym}: is no parameter`);
  return errs;
}

/** Every name the `console` block gives that is no declared parameter: the card's
 * controls and the chip's `{holes}`. */
export function consoleErrors(decl, symbols) {
  const errs = [];
  const c = decl.console ?? {};
  const holes = (t) => [...(t ?? '').matchAll(/\{([A-Za-z][A-Za-z0-9_]*)\}/g)].map((m) => m[1]);
  for (const sym of c.card?.show ?? []) if (!sym.startsWith('readout:') && !symbols.has(sym)) errs.push(`/console/card/show: '${sym}' is no parameter`);
  for (const [at, t] of [['/console/chip', c.chip], ['/console/card/summary', c.card?.summary]]) for (const h of holes(t)) if (!symbols.has(h)) errs.push(`${at}: {${h}} is no parameter`);
  return errs;
}

export const CHECKERS = {
  /** The declaration is valid against the schema, matches its directory, and its stem, CLAP id and
   * LV2 URI are unique among the plugins. */
  declValid(root, recipe, facts) {
    if (!facts.decl) return missing(facts.error);
    const schema = loadSchema(root, recipe);
    const errs = validate(schema, schema, facts.decl);
    if (facts.decl.stem !== facts.stem) errs.push(`/stem: '${facts.decl.stem}' is not its directory's '${facts.stem}'`);
    if (facts.decl.name && facts.decl.name !== `omx ${facts.short}`) errs.push(`/name: '${facts.decl.name}' is not 'omx ${facts.short}'`);
    if (facts.clapId && facts.clapId !== `org.openmixer.${facts.short}`) errs.push(`/clap/id: '${facts.clapId}' is not 'org.openmixer.${facts.short}'`);
    if (facts.uri && facts.uri !== `urn:openmixer:${facts.short}`) errs.push(`/lv2/uri: '${facts.uri}' is not 'urn:openmixer:${facts.short}'`);
    for (const other of pluginStems(root)) {
      if (other === facts.stem) continue;
      const o = pluginFacts(root, other);
      if (o.clapId && o.clapId === facts.clapId) errs.push(`/clap/id: '${facts.clapId}' is also ${other}'s`);
      if (o.uri && o.uri === facts.uri) errs.push(`/lv2/uri: '${facts.uri}' is also ${other}'s`);
    }
    // a chain's generated switches, `order` and band counts are parameters too
    const generated = [...(facts.decl.chain ?? []).flatMap((el) => [`${el.id}On`, ...(el.bandCount ? [el.bandCount.symbol] : [])]), ...(facts.decl.order === 'permutable' ? ['order'] : [])];
    const symbols = new Set([...declParams(facts.decl).map((p) => p.symbol), ...generated]);
    errs.push(...panelErrors(schema, facts.decl, symbols));
    errs.push(...consoleErrors(facts.decl, symbols));
    return errs.length ? missing(`plugins/${facts.stem}/${facts.stem}.decl.json: ${errs.join('; ')}`) : ok(`plugins/${facts.stem}/${facts.stem}.decl.json`);
  },

  /** Each kernel's file is in the installed omx-contract. */
  kernelFile(root, _recipe, facts) {
    if (!facts.decl) return missing(facts.error);
    const where = locateContract(root);
    if (!where.dir) return missing(`cannot check: ${where.why}`);
    const ks = declKernels(facts.decl);
    const absent = ks.filter((k) => !kernelExists(where.dir, k));
    return absent.length
      ? missing(`omx-contract ${where.version} has no ${absent.map((k) => `data/kernels/${k}.json`).join(', ')} (a NEW kernel: the kernel recipe in omx-contract and omx-dsp, then raise the packaging's omx-contract floor; a composite declares its kernels)`)
      : ok(`omx-contract ${where.version}: ${ks.map((k) => `data/kernels/${k}.json`).join(', ')}`);
  },

  /** The named generated files are what tools/gen.mjs writes now. */
  async generatedFresh(root, _recipe, facts, { files }) {
    const gen = await import(join(root, 'tools', 'gen.mjs'));
    let d;
    try {
      d = gen.loadDecl(join(root, 'plugins', facts.stem));
    } catch (e) {
      return missing(`cannot generate: ${e.message}`);
    }
    const out = gen.generate(d);
    const stale = [];
    for (const f of files.map((x) => fill(x, facts))) {
      const at = join('plugins', facts.stem, f);
      if (!(f in out)) stale.push(`${at} (not a generated file)`);
      else if (!has(root, at)) stale.push(`${at} missing`);
      else if (text(root, at) !== out[f]) stale.push(`${at} stale`);
    }
    return stale.length ? missing(`${stale.join(', ')} (make -C plugins/${facts.stem} gen)`) : ok(files.map((f) => fill(f, facts)).join(', '));
  },

  /** The MOD GUI is what omx-panel writes now from the descriptor and the declared panel, and nothing more. */
  async modguiFresh(root, _recipe, facts) {
    const gen = await import(join(root, 'tools', 'gen.mjs'));
    const pf = await import(join(root, 'tools', 'panel-file.mjs'));
    let d;
    try {
      d = gen.loadDecl(join(root, 'plugins', facts.stem));
    } catch (e) {
      return missing(`cannot generate: ${e.message}`);
    }
    const dir = join('plugins', facts.stem, 'generated', `${facts.stem}.lv2`);
    const r = pf.modgui(d, '--check');
    if (r.status !== 0) return missing(`${r.stderr.trim().split('\n').map((l) => l.replace(/^modgui: /, '')).join(', ')} (make -C plugins/${facts.stem} gen)`);
    return ok(`${dir}/modgui.ttl and modgui/`);
  },

  /** The Makefile exists, its identity variables are the declaration's, and it has every target. */
  makefile(root, _recipe, facts, { path, vars, targets }) {
    const p = fill(path, facts);
    if (!has(root, p)) return missing(`${p}: not in the tree`);
    const t = text(root, p);
    const bad = [];
    for (const [v, want] of Object.entries(vars)) {
      const m = t.match(new RegExp(`^${v}\\s*:?=\\s*(\\S+)\\s*$`, 'm'));
      if (!m) bad.push(`no ${v}`);
      else if (m[1] !== fill(want, facts)) bad.push(`${v} is ${m[1]}, the declaration says ${fill(want, facts)}`);
    }
    for (const tg of targets) if (!new RegExp(`^${escapeRe(tg)}\\s*:`, 'm').test(t)) bad.push(`no '${tg}' target`);
    return bad.length ? missing(`${p}: ${bad.join('; ')}`) : ok(p);
  },

  /** A face: exists, includes the generated table, exports its entry symbol. */
  face(root, _recipe, facts, { path, includes, exports }) {
    const p = fill(path, facts);
    if (!has(root, p)) return missing(`${p}: not in the tree`);
    const t = stripC(text(root, p));
    const reached = localIncludes(root, p);
    const bad = includes.map((i) => fill(i, facts)).filter((i) => !reached.has(i)).map((i) => `does not include "${i}" (directly or through its own headers)`);
    if (!new RegExp(`\\b${exports}\\b`).test(t)) bad.push(`exports no ${exports}`);
    return bad.length ? missing(`${p}: ${bad.join('; ')}`) : ok(p);
  },

  /**
   * The plugin's own sources include the omx-dsp header of EACH kernel it declares (`kernel`, or
   * every one of `kernels`): <omxdsp/fx/omx_<kernel>.h> or its instance core. Another omx-dsp
   * header (omx_denormal.h) binds nothing. `headerStems` names the headers of a kernel whose omx-dsp
   * modules are not named like it (eq8, eq16 and eq32 are omx-dsp's eq; ). None
   * of the sources may still be the wizard's stub.
   */
  kernelBinding(root, _recipe, facts, { dir, headerStems = {} }) {
    const d = fill(dir, facts);
    const srcs = ownSources(root, d);
    if (!srcs.length) return missing(`${d}: no C source`);
    const stubs = srcs.filter((f) => /^#define OMX_WIZARD_STUB\b/m.test(text(root, f)));
    if (stubs.length) return missing(`${stubs.join(', ')}: still the wizard's stub (OMX_WIZARD_STUB): bind the faces to omx-dsp's ${facts.kernel} kernel`);
    const code = srcs.map((f) => stripC(text(root, f))).join('\n');
    const kernels = facts.decl ? declKernels(facts.decl) : [facts.kernel];
    const unbound = [];
    for (const k of kernels) {
      for (const st of [headerStems[k] ?? k].flat()) {
        if (!new RegExp(`#include\\s*<omxdsp/fx/omx_${escapeRe(st)}(_instance)?\\.h>`).test(code)) unbound.push(`<omxdsp/fx/omx_${st}.h> (kernel ${k})`);
      }
    }
    return unbound.length
      ? missing(`${d}: no source includes ${unbound.join(', ')}: the binding reaches the plugin's own omx-dsp kernel, not just any omxdsp header`)
      : ok(`${d} includes the omx-dsp header of ${kernels.join(', ')}`);
  },

  /** The Makefile's `test` and `test-heavy` targets together run every token. */
  makeTestRuns(root, _recipe, facts, { path, tokens }) {
    const p = fill(path, facts);
    if (!has(root, p)) return missing(`${p}: not in the tree`);
    const r = testRecipes(text(root, p));
    if (r === undefined) return missing(`${p}: no 'test' target`);
    const absent = tokens.filter((t) => !r.includes(t));
    return absent.length ? missing(`${p}: neither 'make test' nor 'make test-heavy' runs ${absent.join(', ')}`) : ok(`${p}: make test and make test-heavy run ${tokens.join(', ')}`);
  },

  /** An oracle test/<x>-oracle.c, not the wizard's stub, run by make test or make test-heavy. */
  oracle(root, _recipe, facts, { dir, makefile }) {
    const d = fill(dir, facts);
    const oracles = walk(root, d).filter((f) => /-oracle\.c$/.test(f));
    if (!oracles.length) return missing(`${d}: no *-oracle.c`);
    const stubs = oracles.filter((f) => /^#define OMX_WIZARD_STUB\b/m.test(text(root, f)));
    if (stubs.length) return missing(`${stubs.join(', ')}: still the wizard's stub (OMX_WIZARD_STUB): write the comparison against the kernel`);
    const mk = fill(makefile, facts);
    const r = has(root, mk) ? testRecipes(text(root, mk)) : undefined;
    const run = oracles.filter((f) => r && r.includes(f.split('/').pop().replace(/\.c$/, '')));
    return run.length ? ok(`${run.join(', ')}, run by make test or make test-heavy`) : missing(`${oracles.join(', ')}: run by neither ${mk}'s test nor its test-heavy target`);
  },

  /** Each install list has a line (glob) covering the plugin's installed path. */
  installCovers(root, _recipe, facts, { files }) {
    const bad = [];
    for (const [file, path] of Object.entries(files)) {
      const want = fill(path, facts);
      const lines = text(root, file).split('\n').map((l) => l.trim()).filter(Boolean);
      if (!lines.some((g) => globRe(g).test(want))) bad.push(`${file} covers no ${want}`);
    }
    return bad.length ? missing(bad.join('; ')) : ok(Object.keys(files).join(', '));
  },

  /** The plugin's noun (its prose name: `noun`, else the short name) appears in every place (a
   * region of a file, whitespace-insensitive). */
  namedIn(root, _recipe, facts, { places }) {
    const bad = [];
    for (const { file, from, to } of places) {
      const r = region(text(root, file), from, to);
      if (r === undefined) bad.push(`${file}: no ${from}`);
      else if (!r.replace(/\s+/g, ' ').includes(facts.noun)) bad.push(`${file}${from ? ` (${from})` : ''} does not name the ${facts.noun}`);
    }
    return bad.length ? missing(bad.join('; ')) : ok(places.map((p) => p.file).join(', '));
  },

  /**
   * A shared file lists the plugin inside its generated region `region` (tools/gen.mjs SHARED),
   * `needle` standing as a whole there, and the file is what tools/gen.mjs generates now from every
   * plugin folder. A plugin is listed once it ships: its folder has a Makefile, or its declaration
   * says `binding: instance`.
   */
  async sharedListed(root, _recipe, facts, { file, region: name, needle }) {
    const gen = await import(join(root, 'tools', 'gen.mjs'));
    let d, shared;
    try {
      d = gen.loadDecl(join(root, 'plugins', facts.stem));
      shared = gen.generateShared(root);
    } catch (e) {
      return missing(`cannot generate: ${e.message}`);
    }
    if (!gen.ships(d)) return missing(`${facts.stem} does not ship yet: its folder has no Makefile and its declaration no "binding" (instance or chain)`);
    const want = fill(needle, facts);
    const lines = shared[file].split('\n');
    const from = lines.findIndex((l) => new RegExp(`\\bBEGIN GENERATED ${name}\\b`).test(l));
    const to = lines.findIndex((l, i) => i > from && new RegExp(`\\bEND GENERATED ${name}\\b`).test(l));
    const inside = lines.slice(from + 1, to).join('\n');
    if (from < 0 || !new RegExp(`(?<![\\w-])${escapeRe(want)}(?![\\w-])`).test(inside)) return missing(`${file}: the region '${name}' does not list ${want}`);
    if (text(root, file) !== shared[file]) return missing(`${file} is stale: its generated regions are not what the declarations generate (node tools/gen.mjs)`);
    return ok(`${file}: '${name}' lists ${want}`);
  },

  // ---- the laws ----

  /** No file with one of the extensions. */
  noFiles(root, _recipe, facts, { dir, extensions }) {
    const bad = walk(root, fill(dir, facts)).filter((f) => extensions.some((e) => f.toLowerCase().endsWith(e)));
    return bad.length ? missing(`${bad.join(', ')}: not plain C`) : ok('plain C');
  },

  /** No file carries a pattern, and no file has a forbidden name. */
  noText(root, _recipe, facts, { dir, patterns, files }) {
    const bad = [];
    for (const f of walk(root, fill(dir, facts))) {
      if ((files ?? []).includes(f.split('/').pop())) bad.push(`${f} exists`);
      if (!/\.(c|h|mk|txt|json|ttl|cpp|hpp|cc)$|Makefile$/.test(f)) continue;
      const t = text(root, f);
      for (const p of patterns) if (new RegExp(p).test(t)) bad.push(`${f} matches /${p}/`);
    }
    return bad.length ? missing(bad.join('; ')) : ok('none');
  },

  /** No engine source included; no function omx-dsp defines is defined again here. */
  noCopiedDsp(root, _recipe, facts, { dir, engineIncludes }) {
    const srcs = ownSources(root, fill(dir, facts)).concat(walk(root, join(fill(dir, facts), 'test')).filter((f) => /\.(c|h)$/.test(f)));
    const bad = [];
    for (const f of srcs) for (const p of engineIncludes) if (new RegExp(p).test(text(root, f))) bad.push(`${f} includes engine source (/${p}/)`);
    const inc = omxdspInclude();
    if (!inc) return missing(`cannot check: omx-dsp's headers not found (pkg-config omxdsp, or OMXDSP_INCLUDE)`);
    const dsp = dspFunctions(inc);
    const defRe = /^[A-Za-z_][\w \t*]*?\b(omx_[a-z0-9_]+)\s*\([^;{]*\)\s*\{/gm;
    for (const f of srcs) {
      for (const m of stripC(text(root, f)).matchAll(defRe)) if (dsp.has(m[1])) bad.push(`${f} defines ${m[1]}, which omx-dsp defines (${dsp.get(m[1])})`);
    }
    return bad.length ? missing(bad.join('; ')) : ok(`no engine include, no omx-dsp function redefined (${dsp.size} known)`);
  },

  /** Every parameter by reference with no travel retyped, or `own` with its reason and its travel typed; every reference resolving. */
  paramsByReference(root, _recipe, facts) {
    if (!facts.decl) return missing(facts.error);
    const typed = ['min', 'max', 'def', 'unit', 'kind'];
    const bad = [];
    for (const p of declParams(facts.decl)) {
      if (p.ref === undefined) {
        if (typeof p.own !== 'string' || !p.own) bad.push(`'${p.symbol}' has no ref and no "own" reason`);
        continue;
      }
      if ('own' in p) bad.push(`'${p.symbol}' is by reference and "own"`);
      const t = typed.filter((k) => k in p && k !== 'unit'); // a set's unit is face text; a travel's is checked when it resolves
      if (t.length) bad.push(`'${p.symbol}' retypes ${t.join(', ')}`);
    }
    if (!bad.length) {
      const where = locateContract(root);
      if (!where.dir) return missing(`cannot check: ${where.why}`);
      for (const p of declParams(facts.decl)) {
        if (p.ref === undefined) continue;
        let k;
        try {
          k = paramKernel(facts.decl, p, where.dir);
        } catch (e) {
          bad.push(e.message);
          continue;
        }
        const r = findName(where.dir, k, p.ref);
        if (r.error) bad.push(`'${p.symbol}': ${r.error}`);
        else if ('unit' in p && r.entry.kind !== 'set') bad.push(`'${p.symbol}' retypes unit`);
      }
    }
    return bad.length ? missing(`plugins/${facts.stem}/${facts.stem}.decl.json: ${bad.join('; ')}`) : ok('every parameter by reference or own');
  },
};

let dspCache;
/** Every function omx-dsp's headers define: name → header. */
function dspFunctions(inc) {
  if (dspCache) return dspCache;
  dspCache = new Map();
  const base = existsSync(join(inc, 'omxdsp')) ? join(inc, 'omxdsp') : inc;
  const files = [];
  const rec = (d) => {
    for (const n of readdirSync(d)) {
      const p = join(d, n);
      if (statSync(p).isDirectory()) rec(p);
      else if (n.endsWith('.h')) files.push(p);
    }
  };
  rec(base);
  const defRe = /^[A-Za-z_][\w \t*]*?\b(omx_[a-z0-9_]+)\s*\([^;{]*\)\s*\{/gm;
  for (const f of files) for (const m of stripC(readFileSync(f, 'utf8')).matchAll(defRe)) dspCache.set(m[1], relative(base, f));
  return dspCache;
}

// ---- running the recipe ----------------------------------------------------------------------

/** One entry's verdict for one plugin. */
export async function checkEntry(root, recipe, facts, entry) {
  const rule = RULES[entry.when];
  if (rule === undefined) throw new Error(`${RECIPE_PATH}: ${entry.id} names the rule '${entry.when}', which has no predicate`);
  const fn = CHECKERS[entry.checker.fn];
  if (fn === undefined) throw new Error(`${RECIPE_PATH}: ${entry.id} names the checker '${entry.checker.fn}', which does not exist`);
  const required = rule(root, recipe, facts);
  let verdict;
  try {
    verdict = await fn(root, recipe, facts, entry.checker);
  } catch (e) {
    verdict = missing(`cannot check: ${e.message}`);
  }
  return { id: entry.id, step: entry.step, layer: entry.layer ?? null, made: entry.made ?? 'LAW', when: entry.when, required, ...verdict };
}

/** Every artifact's and law's verdict for one plugin. */
export async function checkPlugin(root, recipe, stem) {
  const facts = pluginFacts(root, stem);
  const results = [];
  for (const e of recipe.artifacts) results.push({ kind: 'artifact', ...(await checkEntry(root, recipe, facts, e)) });
  for (const e of recipe.laws) results.push({ kind: 'law', ...(await checkEntry(root, recipe, facts, e)) });
  return { stem, facts, results };
}

/** The owed entries a report found missing, each one line naming the plugin, the entry and its step. */
export function gapLines(report) {
  return report.results
    .filter((r) => r.required && !r.ok)
    .map((r) => `${report.stem}: ${r.kind === 'law' ? 'law ' : ''}${r.id} missing (wizard step '${r.step}'${r.layer ? `, layer ${r.layer}` : ''}): ${r.detail}`);
}

/** Check the recipe itself: every rule, checker, layer and step it names exists. */
export function recipeErrors(recipe) {
  const errs = [];
  const layers = new Set(recipe.layers.map((l) => l.id));
  for (const e of [...recipe.artifacts, ...recipe.laws]) {
    if (!RULES[e.when]) errs.push(`${e.id}: rule '${e.when}' has no predicate`);
    if (!recipe.rules[e.when]) errs.push(`${e.id}: rule '${e.when}' is not declared in the recipe's rules`);
    if (!CHECKERS[e.checker?.fn]) errs.push(`${e.id}: checker '${e.checker?.fn}' does not exist`);
    if (!recipe.steps[e.step]) errs.push(`${e.id}: step '${e.step}' is not declared`);
  }
  for (const a of recipe.artifacts) if (!layers.has(a.layer)) errs.push(`${a.id}: layer '${a.layer}' is not declared in layers`);
  return errs;
}

// ---- paths and layers, for the commit plan ---------------------------------------------------

/** The commit layer of one changed path, from the artifacts' paths (any stem, any kernel). */
export function layerOfPath(recipe, path) {
  for (const a of recipe.artifacts) {
    for (const p of a.paths ?? []) {
      const glob = p.replace(/\{stem\}/g, 'omx-*').replace(/\{kernel\}/g, '*');
      if (globRe(glob).test(path)) return a.layer;
    }
  }
  return undefined;
}

// ---- templates ---------------------------------------------------------------------------------

// ---- the debt: the gaps owed today, held as a ratchet ---------------------------------------

export const DEBT_PATH = 'recipes/completeness-debt.json';
export const loadDebt = (root = ROOT) => (existsSync(join(root, DEBT_PATH)) ? JSON.parse(readFileSync(join(root, DEBT_PATH), 'utf8')).debt : []);

/**
 * The ratchet over the reports: every gap must be a debt entry (`new`: a gap the debt does not
 * hold), and every debt entry must still be a gap (`stale`: a paid debt, to delete). Both fail,
 * and so does an entry naming a plugin that is not in the tree (`unknown`, `known` being every
 * plugin there is: a debt for nothing is never paid and never noticed).
 */
export function debtVerdict(reports, debt, known = reports.map((r) => r.stem)) {
  const gaps = new Set();
  for (const r of reports) for (const x of r.results) if (x.required && !x.ok) gaps.add(`${r.stem} ${x.id}`);
  const owed = new Set(debt.map((d) => `${d.plugin} ${d.entry}`));
  const checked = new Set(reports.map((r) => r.stem));
  return {
    fresh: [...gaps].filter((g) => !owed.has(g)),
    stale: debt.filter((d) => checked.has(d.plugin) && !gaps.has(`${d.plugin} ${d.entry}`)).map((d) => `${d.plugin} ${d.entry} (${d.owedBy})`),
    held: [...gaps].filter((g) => owed.has(g)),
    unknown: debt.filter((d) => !known.includes(d.plugin)).map((d) => `${d.plugin} ${d.entry} (${d.owedBy})`),
  };
}

/**
 * The debt only shrinks: the entries it holds now against the ones at the merge-base with
 * `origin/main` (OMX_DEBT_BASE_REF overrides the ref). `added` lists the entries the base did not
 * hold; `grown` is set when the debt has MORE entries than the base, which fails (a gap paid and
 * another added in the same change keeps the count and is caught as a new gap by debtVerdict).
 * `base` is undefined, with `why`, when there is no base to compare to: a failure in CI, where
 * the checkout must hold main, a note elsewhere.
 */
export function debtGrowth(root, debt) {
  const ref = process.env.OMX_DEBT_BASE_REF || 'origin/main';
  const git = (...a) => execFileSync('git', ['-C', root, ...a], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] }).trim();
  let base;
  try {
    const point = git('merge-base', 'HEAD', ref);
    base = JSON.parse(git('show', `${point}:${DEBT_PATH}`)).debt;
  } catch (e) {
    return { base: undefined, why: `no merge-base with ${ref} to read ${DEBT_PATH} at (${String(e.stderr || e.message).trim().split('\n')[0]})`, added: [], grown: false };
  }
  const held = new Set(base.map((d) => `${d.plugin} ${d.entry}`));
  const added = debt.filter((d) => !held.has(`${d.plugin} ${d.entry}`)).map((d) => `${d.plugin} ${d.entry}`);
  return { base: base.length, added, grown: debt.length > base.length };
}

// ---- the completeness test ------------------------------------------------------------------

async function main(argv) {
  const all = argv.includes('--all');
  const json = argv.includes('--json');
  const root = process.env.OMX_PLUGINS_ROOT ? resolve(process.env.OMX_PLUGINS_ROOT) : ROOT;
  const recipe = loadRecipe(root);
  const broken = recipeErrors(recipe);
  if (broken.length) {
    for (const b of broken) console.error(`completeness: the recipe is broken: ${b}`);
    process.exit(2);
  }
  const asked = argv.filter((a) => !a.startsWith('--'));
  const stems = asked.length ? asked : pluginStems(root);
  if (!stems.length) {
    console.error(`FAIL completeness: no plugin to check (no plugins/<stem>/<stem>.decl.json in ${root}): a run over nothing proves nothing`);
    process.exit(1);
  }
  const reports = [];
  for (const s of stems) reports.push(await checkPlugin(root, recipe, s));
  if (json) {
    console.log(JSON.stringify(reports.map(({ stem, results }) => ({ stem, results })), null, 2));
  } else {
    for (const r of reports) {
      const gaps = gapLines(r);
      const owed = r.results.filter((x) => x.required);
      console.log(`${gaps.length ? 'FAIL' : 'PASS'} ${r.stem}: ${owed.length - gaps.length}/${owed.length} owed entries present`);
      for (const g of gaps) console.log(`  ${g}`);
      if (all) {
        for (const x of r.results) {
          if (!x.required) console.log(`  not owed yet (rule '${x.when}'): ${x.id} ${x.ok ? 'present' : `absent: ${x.detail}`}`);
          else if (x.ok) console.log(`  ok ${x.id}: ${x.detail}`);
        }
      }
    }
  }
  const total = reports.reduce((n, r) => n + gapLines(r).length, 0);
  console.log(`completeness: ${reports.length} plugins, ${total} gaps (recipe ${RECIPE_PATH}: ${recipe.artifacts.length} artifacts, ${recipe.laws.length} laws)`);
  if (argv.includes('--no-debt')) process.exit(total ? 1 : 0);
  const v = debtVerdict(reports, loadDebt(root), pluginStems(root));
  for (const g of v.fresh) console.log(`FAIL new gap, not in ${DEBT_PATH}: ${g}`);
  for (const g of v.stale) console.log(`FAIL stale debt, now satisfied: ${g} (delete it from ${DEBT_PATH})`);
  for (const g of v.unknown) console.log(`FAIL debt for a plugin that does not exist: ${g} (delete it from ${DEBT_PATH})`);
  const growth = debtGrowth(root, loadDebt(root));
  if (growth.base === undefined) {
    console.log(`${process.env.CI ? 'FAIL' : 'NOTE'} debt growth cannot be checked: ${growth.why}${process.env.CI ? ' (CI must fetch main: checkout with fetch-depth 0)' : ''}`);
  } else if (growth.grown) {
    console.log(`FAIL the debt grew: ${loadDebt(root).length} entries, ${growth.base} at the merge-base with main; added: ${growth.added.join(', ') || 'none by name'}. Nothing is added to ${DEBT_PATH} to make a gap pass`);
  }
  const bad = v.fresh.length + v.stale.length + v.unknown.length + (growth.grown ? 1 : 0) + (growth.base === undefined && process.env.CI ? 1 : 0);
  console.log(bad
    ? `completeness: ${v.fresh.length} new gap(s), ${v.stale.length} stale debt entr${v.stale.length === 1 ? 'y' : 'ies'}, ${v.unknown.length} debt entr${v.unknown.length === 1 ? 'y' : 'ies'} for no plugin`
    : `PASS completeness: every gap is held by ${DEBT_PATH} (${v.held.length}), no debt is stale`);
  process.exit(bad ? 1 : 0);
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) await main(process.argv.slice(2));
