// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx-contract.mjs — where a plugin's parameter references are read from: the RESOLVED data of the
 * INSTALLED omx-contract (omx-contract.json in its data directory, `pkg-config --variable=datadir
 * omx-contract`: every reference and derivation already worked out), and nowhere else. Which releases
 * are acceptable is said once, in the packaging: packaging/omx-plugins.spec's BuildRequires on
 * omx-contract-devel, a floor (>=) with no ceiling, the same floor debian/control writes.
 *
 * A parameter in plugins/<stem>/<stem>.decl.json names its travel BY REFERENCE (`"ref"`, with an
 * optional `"field"` and `"forKind"`), never by retyped numbers. A name resolves in this order, and
 * it must resolve in exactly one place:
 *
 *   1. the plugin's kernel file, data/kernels/<kernel>.json;
 *   2. omx-contract's shared files, data/primitives.json and data/rates.json.
 *
 * A name found in ANOTHER kernel's file is refused (a plugin reads only its own kernel), as is a
 * name found in more than one place and a name found nowhere.
 *
 * $OMX_CONTRACT_DIR, a directory holding an omx-contract.json, stands in for the installed data
 * directory: the perturbation proofs point it at a moved copy. Either way the release's version (the
 * render's own `version`) must lie in the packaging's range, or nothing is read.
 */
import { execFileSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import { existsSync, mkdirSync, mkdtempSync, readdirSync, readFileSync, renameSync, rmSync, writeFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

export const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..');
/** Where the packaging declares the releases this repository builds against. */
export const SPEC_FILE = join('packaging', 'omx-plugins.spec');
/** The resolved render, in omx-contract's data directory. */
export const RESOLVED = 'omx-contract.json';
const PKG_CONFIG = () => process.env.PKG_CONFIG || 'pkg-config';

/** -1, 0 or 1, comparing two dotted versions (a missing part is 0: 2.11 is 2.11.0). */
export function compareVersions(a, b) {
  const pa = String(a).split('.').map(Number), pb = String(b).split('.').map(Number);
  for (let i = 0; i < Math.max(pa.length, pb.length); i++) if ((pa[i] ?? 0) !== (pb[i] ?? 0)) return (pa[i] ?? 0) < (pb[i] ?? 0) ? -1 : 1;
  return 0;
}

/**
 * The range packaging/omx-plugins.spec requires of the build dependency `pkg` (`omx-dsp-devel`,
 * `omx-contract-devel`): `{ floor, ceiling }`, from its BuildRequires line `(pkg >= floor with pkg < ceiling)`.
 * Throws when it is missing, written twice, or has no ceiling.
 */
export function packagingRange(pkg, root = ROOT) {
  const file = join(root, SPEC_FILE);
  if (!existsSync(file)) throw new Error(`${SPEC_FILE} is missing: the packaging is where the releases this tree builds against are declared`);
  const spec = readFileSync(file, 'utf8');
  const name = pkg.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
  const found = [...spec.matchAll(new RegExp(`^BuildRequires: \\(${name} >= ([0-9][0-9.]*) with ${name} < ([0-9][0-9.]*)\\)$`, 'gm'))];
  if (found.length !== 1) throw new Error(`${SPEC_FILE}: ${found.length ? 'more than one' : 'no'} "BuildRequires: (${pkg} >= <floor> with ${pkg} < <ceiling>)" line`);
  return { floor: found[0][1], ceiling: found[0][2] };
}

const inRange = (v, r) => compareVersions(v, r.floor) >= 0 && compareVersions(v, r.ceiling) < 0;

const versionOf = (dir) => {
  try {
    return JSON.parse(readFileSync(join(dir, RESOLVED), 'utf8')).version;
  } catch {
    return undefined;
  }
};

/** One omx-contract variable from pkg-config, asked once per pkg-config environment (the program, its search path,
 * libdir and sysroot): the generators ask it per table row, and a spawn each time was most of gen.mjs's run
 * (omx-plugins#39). A changed environment asks again; an absent package is remembered as absent too. */
const pkgVariables = new Map();
function pkgVariable(name) {
  const e = process.env;
  const key = [PKG_CONFIG(), e.PKG_CONFIG_PATH, e.PKG_CONFIG_LIBDIR, e.PKG_CONFIG_SYSROOT_DIR, name].join('\0');
  if (!pkgVariables.has(key)) {
    let v;
    try {
      v = execFileSync(PKG_CONFIG(), [`--variable=${name}`, 'omx-contract'], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'] }).trim() || undefined;
    } catch {
      v = undefined;
    }
    pkgVariables.set(key, v);
  }
  return pkgVariables.get(key);
}

/** The installed omx-contract's data directory (pkg-config), or undefined. */
export const installedDataDir = () => pkgVariable('datadir');

/** The installed omx-contract's contract directory (pkg-config's contractdir, from 2.15.0 on), or undefined. */
export const installedContractDir = () => pkgVariable('contractdir');

/** The omx-contract CLI: $OMX_CONTRACT_CLI (its bin/omx-contract.mjs, or an executable), else `omx-contract`
 * on PATH; `[command, ...leading args]`, or undefined when there is none. */
function contractCli() {
  const env = process.env.OMX_CONTRACT_CLI;
  if (env) return env.endsWith('.mjs') ? [process.execPath, env] : [env];
  try {
    execFileSync('omx-contract', ['--help'], { stdio: 'ignore' });
    return ['omx-contract'];
  } catch (e) {
    return e.code === 'ENOENT' ? undefined : ['omx-contract'];
  }
}

const unionCache = new Map();
/**
 * From omx-contract 2.15.0 on, the kernels ship with their owners (omx-dsp) into omx-contract's contract
 * directory, and the installed omx-contract.json holds only the shared facts. The union is read through the
 * omx-contract CLI, `render --target json --installed`. Every node process renders for itself, and the
 * make targets run side by side (`make -j`), so no render is ever written where another process reads:
 * it goes into a private temporary directory, which is renamed to build/contract-union-<digest of its
 * files>, a name only that content has. A directory under that name is whole by construction and is never
 * rewritten; a second process with the same content finds it in place and drops its own.
 * `{ dir }`, or `{ why }` when the CLI is missing or refuses the directory.
 */
function unionRender(root, contractdir) {
  if (unionCache.has(contractdir)) return unionCache.get(contractdir);
  const cli = contractCli();
  let out;
  if (!cli) out = { why: `omx-contract keeps its kernels in the contract directory ${contractdir}, read through the omx-contract CLI (\`omx-contract render --target json --installed\`), and none is installed: set OMX_CONTRACT_CLI to its bin/omx-contract.mjs or put omx-contract on PATH (BUILDING.md)` };
  else {
    const build = join(root, 'build');
    let tmp;
    try {
      mkdirSync(build, { recursive: true });
      tmp = mkdtempSync(join(build, 'contract-union.tmp-'));
      execFileSync(cli[0], [...cli.slice(1), 'render', '--target', 'json', '--out', tmp, '--installed'], { stdio: ['ignore', 'ignore', 'pipe'] });
      // the render cites each definition by its path only: who owns it goes beside it, so a reader
      // pointed at this directory (OMX_CONTRACT_DIR) still knows
      writeFileSync(join(tmp, OWNERS), `${JSON.stringify(Object.fromEntries(ownersOf(contractdir)), null, 2)}\n`);
      const digest = createHash('sha256');
      for (const f of readdirSync(tmp).sort()) digest.update(f).update('\0').update(readFileSync(join(tmp, f))).update('\0');
      const dir = join(build, `contract-union-${digest.digest('hex').slice(0, 16)}`);
      try {
        renameSync(tmp, dir);
        tmp = undefined;
      } catch (e) {
        // the same content already stands there (another process landed it first): it is whole, use it
        if (!existsSync(join(dir, RESOLVED))) throw e;
      }
      out = { dir };
    } catch (e) {
      out = { why: `the omx-contract CLI refused the contract directory ${contractdir}: ${String(e.stderr ?? e.message).trim().split('\n')[0]}` };
    } finally {
      if (tmp) rmSync(tmp, { recursive: true, force: true });
    }
  }
  unionCache.set(contractdir, out);
  return out;
}

/** Beside a union render: each definition file's owner, as ownersOf read it from the manifests. */
export const OWNERS = 'owners.json';

/** Each installed definition file's package and version, by its path in its owner's tree
 * (`<tree>/<file>`, the `rel` a render cites): read from the manifests of a contract directory, or from
 * the owners.json a union render carries beside it (the directory OMX_CONTRACT_DIR stands in for). */
export function ownersOf(contractdir = installedContractDir()) {
  const out = new Map();
  if (contractdir && existsSync(join(contractdir, OWNERS))) {
    for (const [rel, o] of Object.entries(JSON.parse(readFileSync(join(contractdir, OWNERS), 'utf8')))) out.set(rel, o);
    return out;
  }
  const base = contractdir && join(contractdir, '3');
  if (!base || !existsSync(base)) return out;
  for (const pkg of readdirSync(base)) {
    const file = join(base, pkg, 'MANIFEST.json');
    if (!existsSync(file)) continue;
    const m = JSON.parse(readFileSync(file, 'utf8'));
    for (const f of Object.keys(m.files ?? {})) out.set(m.tree ? `${m.tree}/${f}` : f, { package: m.package, version: m.version });
  }
  return out;
}

/**
 * The directory omx-contract's resolved render is read from: `{ dir, version, range, at, contractdir? }`, or
 * `{ dir: undefined, range, why }` when no omx-contract in the packaging's range is installed (nor given by
 * OMX_CONTRACT_DIR). From 2.15.0 on, the render is the union of the contract directory (unionRender).
 */
export function locateContract(root = ROOT) {
  let range;
  try {
    range = packagingRange('omx-contract-devel', root);
  } catch (e) {
    return { dir: undefined, why: e.message };
  }
  const want = `omx-contract >= ${range.floor} and < ${range.ceiling} (${SPEC_FILE})`;
  const env = process.env.OMX_CONTRACT_DIR;
  const dir = env || installedDataDir();
  const at = env ? `OMX_CONTRACT_DIR=${env}` : dir;
  if (!dir) return { dir: undefined, range, why: `no omx-contract is installed (\`${PKG_CONFIG()} --variable=datadir omx-contract\` finds none); this tree needs ${want}: install omx-contract-devel or libomx-contract-dev (BUILDING.md)` };
  const version = versionOf(dir);
  if (!version) return { dir: undefined, range, why: `${at} holds no readable ${RESOLVED}` };
  if (!inRange(version, range)) return { dir: undefined, range, why: `${at} is omx-contract ${version}; this tree needs ${want}` };
  const contractdir = env ? undefined : installedContractDir();
  if (contractdir && !Object.keys(JSON.parse(readFileSync(join(dir, RESOLVED), 'utf8')).kernels ?? {}).length) {
    const union = unionRender(root, contractdir);
    if (!union.dir) return { dir: undefined, range, why: union.why };
    return { dir: union.dir, version, range, at: `${contractdir} (omx-contract ${version} and its owners)`, contractdir };
  }
  return { dir, version, range, at: `${at} (omx-contract ${version})` };
}

const resolvedCache = new Map();
/** The resolved items of the release at `dir`: name -> { rel, kind, shape, value, ... }. */
export function items(dir) {
  if (!resolvedCache.has(dir)) resolvedCache.set(dir, JSON.parse(readFileSync(join(dir, RESOLVED), 'utf8')).items);
  return resolvedCache.get(dir);
}

const kernelFile = (k) => `data/kernels/${k}.json`;
const SHARED_FILES = ['data/primitives.json', 'data/rates.json'];
/** omx-contract's shared files: the two of a release before 2.15, and one file per item since (an owner's
 * data/primitives/<item>.json, omx-contract's data/rates/<item>.json). A shared name belongs to no kernel. */
const SHARED = { includes: (rel) => SHARED_FILES.includes(rel) || /^data\/(primitives|rates)\/[a-z0-9-]+\.json$/.test(rel ?? '') };

/** The kernels index of the release at `dir` (the json render's `kernels`): each kernel's controls and their items. */
function kernelsIndex(dir) {
  const k = `kernels:${dir}`;
  if (!resolvedCache.has(k)) resolvedCache.set(k, JSON.parse(readFileSync(join(dir, RESOLVED), 'utf8')).kernels ?? {});
  return resolvedCache.get(k);
}

/** Does `kernel` take `name` by a USE: one of its controls whose item another kernel declares (omx-contract's
 * cross-kernel use: glue's detectorTopology takes comp's DETECTOR_TOPOLOGY, one declaration, no copy)? */
const usesName = (index, kernel, name) => (index?.[kernel]?.controls ?? []).some((c) => c.global === name);

/** Does the kernel have its file in omx-contract at `dir`? */
export const kernelExists = (dir, kernel) => Object.values(items(dir)).some((e) => e.rel === kernelFile(kernel));

/** Where one name resolves: `{ file, entry }` or a refusal `{ error }`. */
export function findName(dir, kernel, name) {
  const e = items(dir)[name];
  if (!e) return { error: `'${name}' is in neither ${kernelFile(kernel)} nor the shared files` };
  if (e.rel === kernelFile(kernel) || SHARED.includes(e.rel) || usesName(kernelsIndex(dir), kernel, name)) return { file: e.rel, entry: e };
  return { error: `'${name}' is in ${e.rel}, another kernel's file: a plugin reads only its own kernel (${kernel}) and the items its controls use` };
}

const num = (v, what) => {
  if (typeof v === 'number') return v;
  throw new Error(`${what} is not a plain number (${JSON.stringify(v)}); a parameter's travel must be one`);
};

/** A dotted path into a sheet's value: `NAME.a.b` -> the number there. */
function sheetValue(dir, kernel, path, symbol) {
  const [name, ...keys] = path.split('.');
  const d = findName(dir, kernel, name);
  if (d.error) throw new Error(`param '${symbol}' defaultRef: ${d.error}`);
  let v = d.entry.value;
  for (const k of keys) v = v?.[k];
  if (typeof v !== 'number' && typeof v !== 'boolean') throw new Error(`param '${symbol}' defaultRef: ${path} is not a number in ${d.file}`);
  return typeof v === 'boolean' ? (v ? 1 : 0) : v;
}

const eqCache = new Map();
/**
 * The default bands of an EQ with `count` bands, by omx-contract's own rule: its C render,
 * omx_eq_default_centres() and omx_eq_default_type() of omx-dsp's <omxdsp/omx_eq_defaults.h> (from omx-dsp 0.25.0;
 * <omxdsp/omxdsp_definitions.h> before it, and omx-contract's own until 2.15.0)
 * (the package installs no JavaScript), run through a small probe built once into build/; gain and Q
 * are EQ_BAND_DEFAULTS of the render at `dir`. `[{ type, freqHz, gainDb, q }]`.
 */
export function eqDefaultBands(dir, count, root = ROOT) {
  const key = `${dir}#${count}`;
  if (!eqCache.has(key)) {
    const rows = execFileSync(eqProbe(root), [String(Number(count))], { encoding: 'utf8' }).trim().split('\n');
    const types = items(dir).EQ_BAND_TYPES?.value;
    const band = items(dir).EQ_BAND_DEFAULTS?.value;
    if (!Array.isArray(types) || !band) throw new Error(`omx-contract at ${dir}: no EQ_BAND_TYPES or EQ_BAND_DEFAULTS`);
    eqCache.set(key, rows.map((r) => {
      const [t, hz] = r.split(' ');
      return { type: types[Number(t)], freqHz: Number(hz), gainDb: band.gainDb, q: band.q };
    }));
  }
  return eqCache.get(key);
}

let probe;
/**
 * build/eq-default-probe, compiled against the installed omx-contract (cc, pkg-config, -lm). Every process
 * compiles it for itself (`make -j` runs several at once), so it is compiled in a private directory and renamed
 * into place whole: a process never runs a probe another one's compiler still writes (#195).
 */
function eqProbe(root) {
  if (probe) return probe;
  const out = join(root, 'build', 'eq-default-probe');
  mkdirSync(dirname(out), { recursive: true });
  const tmp = mkdtempSync(join(dirname(out), '.eq-default-probe.tmp-'));
  try {
    buildEqProbe(join(tmp, 'eq-default-probe'));
    renameSync(join(tmp, 'eq-default-probe'), out);
  } finally {
    rmSync(tmp, { recursive: true, force: true });
  }
  probe = out;
  return out;
}

function buildEqProbe(out) {
  const src = `${out}.c`;
  writeFileSync(src, `#include <stdio.h>
#include <stdlib.h>
#if __has_include(<omxdsp/omx_eq_defaults.h>)
#include <omxdsp/omx_eq_defaults.h>
#else
#include <omxdsp/omxdsp_definitions.h>
#endif
int main(int argc, char **argv) {
  static float hz[1024];
  unsigned n = argc > 1 ? (unsigned)strtoul(argv[1], NULL, 10) : 0;
  if (n == 0 || n > 1024 || omx_eq_default_centres(n, hz) != n) { fprintf(stderr, "eq-default-probe: no default bands for %u\\n", n); return 1; }
  for (unsigned i = 0; i < n; i++) printf("%d %.6g\\n", omx_eq_default_type(i, n), (double)hz[i]);
  return 0;
}
`);
  let cflags;
  try {
    // the rule is omx-dsp's definitions header, which reads omx-contract's own
    cflags = execFileSync(PKG_CONFIG(), ['--cflags', 'omxdsp', 'omx-contract'], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] }).trim().split(/\s+/).filter(Boolean);
  } catch (e) {
    throw new Error(`the EQ default rule is omx-dsp's C render, and \`${PKG_CONFIG()} --cflags omxdsp omx-contract\` failed: ${String(e.stderr ?? e.message).trim()}`);
  }
  execFileSync(process.env.CC || 'cc', ['-O1', '-o', out, src, ...cflags, '-lm'], { stdio: ['ignore', 'ignore', 'inherit'] });
}

/** The band count of a strip type (`eq16`, ...) in EQ_BAND_COUNTS: its default. */
const bandCount = (dir, strip, symbol) => {
  const c = items(dir).EQ_BAND_COUNTS?.value?.[strip];
  if (!c) throw new Error(`param '${symbol}': EQ_BAND_COUNTS has no strip type '${strip}'`);
  return c.default;
};

/**
 * One parameter's travel, read from its reference: `{ min, max, def, unit, kind?, values?, points? }`.
 * A `travels` entry gives min/max/default/unit (from `field` when the entry is a table; the default
 * for `forKind` from its byKind; `defaultRef` NAME.path names a sheet number to come up at instead,
 * and `defaultBand` {strip, index, of: 'centre'|'type'} takes the default of one band of
 * omx-contract's default EQ rule); a boolean `scalar` is a toggle, and so is a set whose ids are
 * exactly off, on; a `list` is an integer index into
 * its values, its default named by `defaultRef` (a scalar holding one of the values) or the first;
 * a `set` is an integer index into its ids, labelled by its labels (or the declaration's `values`
 * when the set has none), its default the set's own, `defaultRef` or `defaultBand`.
 */
export function resolveParam(dir, kernel, p) {
  const where = findName(dir, kernel, p.ref);
  if (where.error) throw new Error(`param '${p.symbol}': ${where.error}`);
  const e = where.entry;
  const at = `${where.file}#${p.ref}${p.field ? `.${p.field}` : ''}`;
  // the bank's count: a strip type's default, or a count the contract declares (DYNEQ_BANDS)
  const count = (b) => {
    if (b.count === undefined) return bandCount(dir, b.strip, p.symbol);
    const n = items(dir)[b.count]?.value;
    if (!Number.isInteger(n) || n < 1) throw new Error(`param '${p.symbol}': defaultBand's count ${b.count} is no count omx-contract declares`);
    return n;
  };
  const band = p.defaultBand ? eqDefaultBands(dir, count(p.defaultBand))[p.defaultBand.index] : undefined;
  if (p.defaultBand && !band) throw new Error(`param '${p.symbol}': ${p.defaultBand.strip} has no band ${p.defaultBand.index}`);
  if (e.kind === 'travels') {
    let t = e.value;
    if (e.shape === 'table') {
      if (!p.field) throw new Error(`param '${p.symbol}': ${where.file}#${p.ref} has fields (${Object.keys(e.value).join(', ')}); name one with "field"`);
      t = e.value[p.field];
      if (!t) throw new Error(`param '${p.symbol}': ${where.file}#${p.ref} has no field '${p.field}'`);
    } else if (p.field) throw new Error(`param '${p.symbol}': ${where.file}#${p.ref} has no fields; drop "field"`);
    let def = t.default ?? t.min;
    // a kind the travel does not override comes up at the travel's own default
    if (p.forKind && t.byKind && p.forKind in t.byKind) def = t.byKind[p.forKind];
    if (p.defaultRef) def = sheetValue(dir, kernel, p.defaultRef, p.symbol);
    if (band) {
      if (p.defaultBand.of !== 'centre') throw new Error(`param '${p.symbol}': a travel's band default is the 'centre'`);
      def = band.freqHz;
    }
    const out = { min: num(t.min, at), max: num(t.max, at), def: num(def, at), unit: t.unit ?? '', from: at };
    // the default a channel kind comes up at instead (a return's wet mix), where the travel names one
    if (t.byKind && !p.forKind) out.byKind = Object.fromEntries(Object.entries(t.byKind).map(([k, v]) => [k, num(v, at)]));
    if (typeof t.step === 'number' && t.step >= 1 && Number.isInteger(out.min) && Number.isInteger(out.max)) out.kind = 'integer';
    return out;
  }
  if (e.kind === 'scalar' && typeof e.value === 'boolean') return { min: 0, max: 1, def: e.value ? 1 : 0, unit: '', kind: 'toggle', from: at };
  if (e.kind === 'list' && Array.isArray(e.value)) {
    let def = 0;
    if (p.defaultRef) {
      const d = findName(dir, kernel, p.defaultRef);
      if (d.error) throw new Error(`param '${p.symbol}' defaultRef: ${d.error}`);
      def = e.value.indexOf(d.entry.value);
      if (def < 0) throw new Error(`param '${p.symbol}': ${p.defaultRef}'s value ${JSON.stringify(d.entry.value)} is not one of ${p.ref}'s values`);
    }
    return { min: 0, max: e.value.length - 1, def, unit: '', kind: 'integer', values: e.value, from: at };
  }
  if (e.kind === 'set' && Array.isArray(e.value)) {
    const ids = e.value;
    const numeric = ids.every((x) => typeof x === 'number');
    if (p.defaultId !== undefined && e.default !== undefined) throw new Error(`param '${p.symbol}': ${p.ref} names its own default (${JSON.stringify(e.default)}); "defaultId" is for a set that names none`);
    let defId = e.default ?? p.defaultId ?? ids[0];
    if (band) {
      if (p.defaultBand.of !== 'type') throw new Error(`param '${p.symbol}': a set's band default is the 'type'`);
      defId = band.type;
    }
    const def = ids.indexOf(defId);
    if (def < 0) throw new Error(`param '${p.symbol}': ${JSON.stringify(defId)} is not one of ${p.ref}'s ids ${JSON.stringify(ids)}`);
    if (numeric) {
      // a set of numbers (the pass filters' slopes) is the travel from its first to its last, labelled at each id
      const unit = p.unit ?? '';
      return { min: ids[0], max: ids[ids.length - 1], def: ids[def], unit, kind: 'integer', points: ids.map((v) => ({ value: v, label: unit ? `${v} ${unit}` : `${v}` })), from: at };
    }
    // a switch (ids exactly off, on: DELAY_PINGPONGS, DRIVE_AUTO_GAINS, EQ_BAND_ONS) is a toggle,
    // its value the id's index, so a host draws it as one and keeps the toggled hint
    if (ids.length === 2 && ids[0] === 'off' && ids[1] === 'on' && p.values === undefined)
      return { min: 0, max: 1, def, unit: '', kind: 'toggle', from: at };
    const labels = e.labels ?? p.values;
    if (!Array.isArray(labels) || labels.length !== ids.length) throw new Error(`param '${p.symbol}': ${p.ref} has no labels; the declaration's "values" must give one per id (${ids.length})`);
    return { min: 0, max: ids.length - 1, def, unit: '', kind: 'integer', values: labels, from: at };
  }
  throw new Error(`param '${p.symbol}': ${at} is a ${e.kind}${e.kind === 'scalar' ? ` of ${JSON.stringify(e.value)}` : ''}, not a travel, a toggle, a list or a set`);
}

/**
 * The perturbation proof's move: copy omx-contract's render at `src` to `dst` and move the default
 * of the travel parameter `p` (of `kernel`) references to its other end; with `move: 'max'`, move
 * the travel's upper limit out by one step (its own, else 1) instead. The copy is what a consumer
 * reads with OMX_CONTRACT_DIR=dst; every generated file must then follow, or be refused as stale.
 */
export function perturbCopy(src, dst, kernel, p, { move = 'default' } = {}) {
  mkdirSync(dst, { recursive: true });
  execFileSync('cp', [join(src, RESOLVED), dst]);
  if (existsSync(join(src, OWNERS))) execFileSync('cp', [join(src, OWNERS), dst]);
  const file = join(dst, RESOLVED);
  const data = JSON.parse(readFileSync(file, 'utf8'));
  const where = findNameIn(data.items, kernel, p.ref, data.kernels);
  if (where.error) throw new Error(where.error);
  const e = data.items[p.ref];
  if (move === 'max') {
    if (e.kind !== 'travels') throw new Error(`${p.ref}: a ${e.kind} has no limit to move`);
    const t = e.shape === 'table' ? e.value[p.field] : e.value;
    t.max += typeof t.step === 'number' && t.step > 0 ? t.step : 1;
  } else if (e.kind === 'travels') {
    const t = e.shape === 'table' ? e.value[p.field] : e.value;
    const def = t.default ?? t.min;
    t.default = def === t.max ? t.min : t.max;
    if (p.forKind && t.byKind && p.forKind in t.byKind) t.byKind[p.forKind] = t.default;
  } else if (e.kind === 'scalar' && typeof e.value === 'boolean') e.value = !e.value;
  else if (e.kind === 'set' && Array.isArray(e.value) && e.value.length > 1) {
    // a set's default is one of its ids: it moves to the last, or from the last to the first
    const ids = e.value;
    const now = ids.includes(e.default) ? e.default : ids[0];
    e.default = now === ids[ids.length - 1] ? ids[0] : ids[ids.length - 1];
  } else throw new Error(`${p.ref}: a ${e.kind} has no default to move`);
  writeFileSync(file, `${JSON.stringify(data, null, 2)}\n`);
  return where.file;
}

function findNameIn(its, kernel, name, index) {
  const e = its[name];
  if (!e) return { error: `'${name}' is in neither ${kernelFile(kernel)} nor the shared files` };
  if (e.rel === kernelFile(kernel) || SHARED.includes(e.rel) || usesName(index, kernel, name)) return { file: e.rel };
  return { error: `'${name}' is in ${e.rel}, another kernel's file` };
}

/** The omx-contract kernels a declaration's parameters reference: `kernels`, a chain's elements'
 * (each once, in chain order), or `[kernel]`. */
export const declKernels = (d) =>
  Array.isArray(d.kernels) && d.kernels.length ? d.kernels : Array.isArray(d.chain) ? [...new Set(d.chain.map((el) => el.kernel))] : [d.kernel];

/** A declaration's hand-written parameters: its `params`, or a chain's elements' (each naming its
 * element's kernel); a chain's switches and `order` are generated, never declared. */
export const declParams = (d) => d.params ?? (d.chain ?? []).flatMap((el) => [...(el.params ?? []), ...(el.appended ?? [])].map((p) => ({ ...p, kernel: p.kernel ?? el.kernel })));

/**
 * The kernel one parameter's reference is read from: its own `kernel` (which must be one of the
 * plugin's kernels), or the plugin's only kernel. A composite's parameter must name its kernel.
 */
export function paramKernel(d, p, dir) {
  const ks = declKernels(d);
  if (p.kernel !== undefined) {
    if (!ks.includes(p.kernel)) throw new Error(`param '${p.symbol}': kernel '${p.kernel}' is not one of the plugin's kernels (${ks.join(', ')})`);
    return p.kernel;
  }
  // a name in the shared files (primitives, rates) belongs to no kernel: any of the plugin's reads it
  if (ks.length > 1 && dir && SHARED.includes(items(dir)[p.ref]?.rel)) return ks[0];
  if (ks.length > 1) throw new Error(`param '${p.symbol}': a composite (${ks.join(', ')}) names each parameter's kernel`);
  return ks[0];
}

/** The render's `kernels`: kernel -> { controls: [{ name, kind, global, table?, rearms? }] }. */
const kernelsCache = new Map();
export function renderKernels(dir) {
  if (!kernelsCache.has(dir)) kernelsCache.set(dir, JSON.parse(readFileSync(join(dir, RESOLVED), 'utf8')).kernels ?? {});
  return kernelsCache.get(dir);
}

/**
 * The controls of one kernel, in the kernel file's order, as omx-contract's render lists them under
 * `kernels`: `[{ name, ref, field?, kind: 'travel'|'choice', rearms? }]`. A control's `global` is its
 * `ref`; a table field's name is its `field`; `rearms` is true on a control whose change re-arms the
 * kernel's state.
 * @throws when the render lists no such kernel.
 */
export function kernelControls(dir, kernel) {
  const listed = renderKernels(dir)[kernel];
  if (!listed) throw new Error(`omx-contract's render (${join(dir, RESOLVED)}) lists no kernel '${kernel}' under "kernels"`);
  return listed.controls.map((c) => ({
    name: c.name,
    ref: c.global,
    ...(c.table ? { field: c.name } : {}),
    kind: c.kind,
    ...(c.rearms === true ? { rearms: true } : {}),
    ...(c.count ? { count: c.count } : {}),
    ...(c.when ? { when: c.when } : {}),
  }));
}

/** The contract control a by-reference parameter reads: its `field`, or the control its `ref` is. A
 * field of a shared table (the primitives' GATE_CONTINUOUS_TRAVELS: the same control's travel
 * without its step) names the kernel's control of that name. */
export function controlOf(dir, kernel, p) {
  if (p.ref === undefined) return undefined;
  const cs = kernelControls(dir, kernel);
  // two controls may use one set (eq's hpfSlope and lpfSlope, FILTER_SLOPES): the parameter whose
  // symbol is the control's name in snake case (or its base symbol, a per-band b<n>_ prefix off) takes it
  const snake = (x) => x.replace(/([a-z0-9])([A-Z])/g, '$1_$2').toLowerCase();
  const byRef = cs.filter((x) => x.ref === p.ref);
  const sym = snake(p.symbol.replace(/^b\d+_/, ''));
  const shared = SHARED.includes(items(dir)[p.ref]?.rel);
  const c = p.field
    ? cs.find((x) => x.name === p.field && (x.ref === p.ref || !x.field || shared))
    : byRef.length > 1 ? byRef.find((x) => snake(x.name) === sym) ?? byRef[0] : byRef[0];
  return c ? { ...c, ...(p.field ? { name: p.field } : {}) } : undefined;
}
