#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * resource-hints-check.mjs — every shipped plugin declares its resource hints (omx-contract RESOURCE_HINTS,
 * tools/resource-hints.mjs), and both carriers say what the declaration says:
 *
 *   1. the vocabulary: the installed contract names the LV2 prefix omxres and the namespace
 *      https://www.openmixer.org/ns/resource-hints#, and its own carriers (omx_resource_hints.h, omx-resource-hints.ttl)
 *      give each hint the bit, the C type and the XSD range tools/resource-hints.mjs renders;
 *   2. every shipped plugin's declaration states every hint of REQUIRED_HINTS;
 *   3. the LV2 render: the generated <stem>.ttl, read by serdi as a host's parser reads it, carries one triple per
 *      stated hint on the plugin, the predicate in the omxres namespace exactly, the value and datatype the
 *      declaration's, and no other triple in any namespace that names resource hints;
 *   4. the CLAP render: the generated parameter header's OMX_<K>_RESOURCE_HINTS sets the bits of the stated hints and
 *      no other, and each stated member to the declaration's value. (What the BUILT face's get returns is
 *      tools/clap-ext-check.mjs's, in each plugin's `make test`.)
 *
 * `--self-test` adds the sabotage arms: each breaks one thing and must go red.
 *
 * `--binaries <plugin dir> <file>...` is the other half, run by each plugin's `make test` on its built faces: a
 * claim a binary's imports can refute is held to them. threads-spawned 0 imports no thread creation, opens-sockets
 * false no socket, streams-from-disk false no file open or read (omx-dsp is linked in, so its calls count too).
 *
 * `make resource-hints`, part of `make test`. It reads omx-contract as `make test` does.
 */
import { execFileSync } from 'node:child_process';
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { loadDecl, pluginDirs, ROOT } from '../gen.mjs';
import { installedDataDir, locateContract } from '../omx-contract.mjs';
import { presetsOf } from '../preset-lib.mjs';
import { REQUIRED_HINTS, hintVocabulary, maskOf } from '../resource-hints.mjs';

/** The pin: the LV2 prefix and namespace as #216 and omx-contract#140 settled them, which this check holds the installed
 * contract to. No generator reads them; tools/gen.mjs takes both from the contract (tools/resource-hints.mjs). */
export const LV2_PREFIX = 'omxres';
export const LV2_NAMESPACE = 'https://www.openmixer.org/ns/resource-hints#';
const XSD = 'http://www.w3.org/2001/XMLSchema#';

/** The imports that refute a claim, by the hint and the value whose claim they refute. */
export const REFUTED_BY = {
  'threads-spawned': { when: (v) => v === 0, symbols: ['pthread_create', 'thrd_create', 'clone', 'clone3'] },
  'opens-sockets': { when: (v) => v === false, symbols: ['socket', 'socketpair'] },
  'streams-from-disk': { when: (v) => v === false, symbols: ['open', 'open64', 'openat', 'openat64', 'fopen', 'fopen64', 'read', 'pread', 'pread64', 'readv', 'fread', 'mmap', 'mmap64'] },
};

const cpp = (s) => s.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');

/** Law 1: the vocabulary and the installed carriers. */
export function vocabularyErrors(v, header, ttl) {
  const errors = [];
  if (v.prefix !== LV2_PREFIX) errors.push(`the contract's LV2 prefix is '${v.prefix}', not '${LV2_PREFIX}'`);
  if (v.namespace !== LV2_NAMESPACE) errors.push(`the contract's LV2 namespace is <${v.namespace}>, not <${LV2_NAMESPACE}>`);
  if (!new RegExp(`^@prefix ${cpp(v.prefix)}: <${cpp(v.namespace)}> \\.$`, 'm').test(ttl)) errors.push(`the installed omx-resource-hints.ttl does not declare @prefix ${v.prefix}: <${v.namespace}>`);
  for (const h of v.hints) {
    if (!new RegExp(`^#define ${h.macro} \\(1u << ${h.bit}\\)`, 'm').test(header)) errors.push(`${h.word}: the installed header's ${h.macro} is not bit ${h.bit}`);
    if (!new RegExp(`^\\s*${cpp(h.c)} ${h.member};`, 'm').test(header)) errors.push(`${h.word}: the installed header's member is not ${h.c} ${h.member}`);
    const block = new RegExp(`^${cpp(v.prefix)}:${h.key}\\n([\\s\\S]*?) \\.$`, 'm').exec(ttl)?.[1];
    if (!block) errors.push(`${h.word}: the installed omx-resource-hints.ttl has no ${v.prefix}:${h.key}`);
    else if (!block.includes(`rdfs:range xsd:${h.xsd} ;`)) errors.push(`${h.word}: the installed omx-resource-hints.ttl's ${v.prefix}:${h.key} is not ranged xsd:${h.xsd}`);
  }
  return errors;
}

/** Law 2: the declaration states every required hint. */
export function requiredErrors(plugins) {
  return plugins.flatMap(({ d }) => REQUIRED_HINTS.filter((w) => !d.hints.some((h) => h.word === w)).map((w) => `${d.stem}: its declaration's "resourceHints" does not state ${w}`));
}

/** The N-Triples of a Turtle text, read by serdi: `[{ s, p, o }]`, a literal's object as `{ lex, dt }`. */
export function triplesOf(ttlText, base) {
  const nt = execFileSync('serdi', ['-i', 'turtle', '-o', 'ntriples', '-', base], { input: ttlText, encoding: 'utf8', stdio: ['pipe', 'pipe', 'pipe'] });
  return nt.split('\n').filter(Boolean).map((l) => {
    const m = /^(<[^>]*>|_:\S+) <([^>]*)> (.*) \.$/.exec(l);
    if (!m) throw new Error(`serdi wrote a line this check cannot read: ${l}`);
    const lit = /^"(.*)"(?:\^\^<([^>]*)>)?$/.exec(m[3]);
    return { s: m[1], p: m[2], o: lit ? { lex: lit[1], dt: lit[2] } : m[3] };
  });
}

/** Law 3: the LV2 render of one plugin. */
export function ttlErrors(v, d, ttlText) {
  const errors = [];
  const subject = `<${d.lv2.uri}>`;
  const triples = triplesOf(ttlText, `file:///${d.stem}.lv2/${d.stem}.ttl`).filter((t) => t.p.startsWith(v.namespace) || /resource-hints/.test(t.p));
  for (const t of triples) if (!t.p.startsWith(v.namespace)) errors.push(`${d.stem}: <${t.p}> is a resource hint outside the namespace <${v.namespace}>`);
  const served = triples.filter((t) => t.p.startsWith(v.namespace));
  for (const t of served) if (t.s !== subject) errors.push(`${d.stem}: <${t.p}> is stated of ${t.s}, not the plugin ${subject}`);
  const want = d.hints.map((h) => [`${v.namespace}${h.key}`, `${h.value}`, `${XSD}${h.xsd}`]).sort();
  const got = served.map((t) => [t.p, t.o.lex, t.o.dt]).sort();
  if (JSON.stringify(got) !== JSON.stringify(want)) errors.push(`${d.stem}: the TTL states ${JSON.stringify(got)}; the declaration ${JSON.stringify(want)}`);
  return errors;
}

/** Law 4: the CLAP render of one plugin, in its generated parameter header. */
export function headerErrors(v, d, header) {
  const K = d.kernel.toUpperCase();
  const body = new RegExp(`static const omx_resource_hints_t OMX_${K}_RESOURCE_HINTS = \\{\\n([\\s\\S]*?)\\n\\};`).exec(header)?.[1];
  if (!body) return [`${d.stem}: its parameter header has no OMX_${K}_RESOURCE_HINTS`];
  const errors = [];
  const declared = /^\s*\.declared = (.*),$/m.exec(body)?.[1] ?? '';
  const bits = declared === '0u' ? [] : declared.split(' | ');
  const mask = bits.reduce((m, b) => m | (1 << (v.hints.find((h) => h.macro === b)?.bit ?? 31)), 0) >>> 0;
  if (mask !== maskOf(d.hints) || bits.some((b) => !v.hints.some((h) => h.macro === b))) errors.push(`${d.stem}: OMX_${K}_RESOURCE_HINTS.declared is ${declared}; the declaration's ${d.hints.map((h) => h.macro).join(' | ') || '0u'}`);
  const members = Object.fromEntries([...body.matchAll(/^\s*\.([a-z_]+) = ([^,]+),$/gm)].filter(([, m]) => m !== 'declared').map(([, m, x]) => [m, x.replace(/u(ll)?$/, '')]));
  const want = Object.fromEntries(d.hints.map((h) => [h.member, `${h.value}`]));
  if (JSON.stringify(members) !== JSON.stringify(want)) errors.push(`${d.stem}: OMX_${K}_RESOURCE_HINTS sets ${JSON.stringify(members)}; the declaration ${JSON.stringify(want)}`);
  return errors;
}

/** The stdio calls of omx_preset_file.h, which a face with factory presets imports to read a preset file when the
 * host asks for it (never while it processes, which is what streams-from-disk states). */
export const PRESET_READER = ['fopen', 'fopen64', 'fread'];

/** The built binaries' imports against the declaration's claims. `presetReader`: the plugin ships factory presets,
 * so PRESET_READER is not a refutation of streams-from-disk false. */
export function importErrors(d, files, importsOf, presetReader = false) {
  const errors = [];
  for (const file of files) {
    const imported = new Set(importsOf(file));
    for (const h of d.hints) {
      const r = REFUTED_BY[h.word];
      if (!r || !r.when(h.value)) continue;
      const hit = r.symbols.filter((s) => imported.has(s) && !(presetReader && h.word === 'streams-from-disk' && PRESET_READER.includes(s)));
      if (hit.length) errors.push(`${d.stem}: declares ${h.word} ${h.value}, and ${file} imports ${hit.join(', ')}`);
    }
  }
  return errors;
}

/** A binary's undefined dynamic symbols, without their version. */
const nmImports = (file) => execFileSync('nm', ['-D', '--undefined-only', file], { encoding: 'utf8' }).split('\n').filter(Boolean).map((l) => l.trim().split(/\s+/).pop().replace(/@.*$/, ''));

function contractCarriers() {
  const where = locateContract(ROOT);
  if (!where.dir) throw new Error(`resource-hints: ${where.why}`);
  const includedir = execFileSync(process.env.PKG_CONFIG || 'pkg-config', ['--variable=includedir', 'omx-contract'], { encoding: 'utf8' }).trim();
  return {
    v: hintVocabulary(where.dir),
    header: readFileSync(join(includedir, 'omxcontract', 'omx_resource_hints.h'), 'utf8'),
    ttl: readFileSync(join(installedDataDir(), 'omx-resource-hints.ttl'), 'utf8'),
  };
}

export function load(root = ROOT) {
  return pluginDirs(root).map((dir) => {
    const d = loadDecl(dir);
    return {
      d,
      ttl: readFileSync(join(dir, 'generated', `${d.stem}.lv2`, `${d.stem}.ttl`), 'utf8'),
      header: readFileSync(join(dir, 'generated', `omx_${d.kernel}_params.h`), 'utf8'),
    };
  });
}

const treeErrors = (c, plugins) => [
  ...vocabularyErrors(c.v, c.header, c.ttl),
  ...requiredErrors(plugins),
  ...plugins.flatMap((p) => [...ttlErrors(c.v, p.d, p.ttl), ...headerErrors(c.v, p.d, p.header)]),
];

if (process.argv[1] && import.meta.url === new URL(`file://${process.argv[1]}`).href) {
  const args = process.argv.slice(2);
  if (args[0] === '--binaries') {
    const [, dir, ...files] = args;
    const d = loadDecl(dir);
    const errors = importErrors(d, files, nmImports, presetsOf(`org.openmixer.${d.stem.replace(/^omx-/, '')}`, ROOT).length > 0);
    for (const e of errors) console.log(`FAIL resource-hints ${e}`);
    if (!errors.length) console.log(`PASS resource-hints ${d.stem}: no import of ${files.length} built face(s) refutes ${d.hints.map((h) => `${h.word} ${h.value}`).join(', ') || 'a stated hint'}`);
    process.exit(errors.length ? 1 : 0);
  }
  const c = contractCarriers();
  const plugins = load();
  const errors = treeErrors(c, plugins);
  for (const e of errors) console.log(`FAIL resource-hints ${e}`);
  let bad = errors.length;
  if (args.includes('--self-test')) {
    const one = plugins.find((p) => p.d.stem === 'omx-delay') ?? plugins[0];
    const others = plugins.filter((p) => p !== one);
    const withOne = (q) => [...others, q];
    const dropped = { ...one, d: { ...one.d, hints: one.d.hints.filter((h) => h.word !== 'opens-sockets') } };
    const delay = one.d;
    const arms = {
      [`${delay.stem}'s declaration without opens-sockets`]: () => requiredErrors(withOne(dropped)),
      [`${delay.stem}'s TTL in another namespace (http, not https)`]: () => ttlErrors(c.v, delay, one.ttl.replace(`<${LV2_NAMESPACE}>`, `<${LV2_NAMESPACE.replace('https:', 'http:')}>`)),
      [`${delay.stem}'s TTL in the retired namespace`]: () => ttlErrors(c.v, delay, one.ttl.replace(`<${LV2_NAMESPACE}>`, '<https://open-mixer.github.io/ns/resource-hints#>')),
      [`${delay.stem}'s TTL with a hint's value flipped`]: () => ttlErrors(c.v, delay, one.ttl.replace(`${LV2_PREFIX}:opensSockets false`, `${LV2_PREFIX}:opensSockets true`)),
      [`${delay.stem}'s TTL with a figure left a plain string`]: () => ttlErrors(c.v, delay, one.ttl.replace(`${LV2_PREFIX}:threadsSpawned "0"^^xsd:integer`, `${LV2_PREFIX}:threadsSpawned "0"`)),
      [`${delay.stem}'s TTL without a stated hint`]: () => ttlErrors(c.v, delay, one.ttl.replace(/^ {4}omxres:streamsFromDisk .*\n/m, '')),
      [`${delay.stem}'s TTL with a hint the declaration leaves unknown`]: () => ttlErrors(c.v, delay, one.ttl.replace(`${LV2_PREFIX}:opensSockets false ;`, `${LV2_PREFIX}:opensSockets false ;\n    ${LV2_PREFIX}:activateTime "0"^^xsd:integer ;`)),
      [`${delay.stem}'s header with a bit dropped`]: () => headerErrors(c.v, delay, one.header.replace(' | OMX_RESOURCE_HINT_OPENS_SOCKETS', '')),
      [`${delay.stem}'s header with a member changed`]: () => headerErrors(c.v, delay, one.header.replace('.threads_spawned = 0u', '.threads_spawned = 2u')),
      'a contract naming another LV2 namespace': () => vocabularyErrors({ ...c.v, namespace: 'https://open-mixer.github.io/ns/resource-hints#' }, c.header, c.ttl),
      'an installed header whose bit order moved': () => vocabularyErrors(c.v, c.header.replace('OMX_RESOURCE_HINT_THREADS_SPAWNED (1u << 3)', 'OMX_RESOURCE_HINT_THREADS_SPAWNED (1u << 4)'), c.ttl),
      [`${delay.stem}'s face importing pthread_create`]: () => importErrors(delay, ['face.clap'], () => ['calloc', 'pthread_create']),
      [`${delay.stem}'s face importing fopen`]: () => importErrors(delay, ['face.so'], () => ['fopen']),
      [`${delay.stem}'s face importing open, with presets shipped`]: () => importErrors(delay, ['face.so'], () => ['open', 'fopen'], true),
      [`${delay.stem}'s face importing fread, no presets shipped`]: () => importErrors(delay, ['face.so'], () => ['fread'], false),
      [`${delay.stem}'s face importing socket`]: () => importErrors(delay, ['face.so'], () => ['socket']),
    };
    for (const [what, run] of Object.entries(arms)) {
      const red = run();
      console.log(`${red.length ? 'PASS' : 'FAIL'} resource-hints sabotage: ${what} is red${red.length ? ` (${red[0]})` : ''}`);
      if (!red.length) bad++;
    }
  }
  console.log(bad ? `resource-hints: ${bad} failure(s)` : `PASS resource-hints: ${plugins.length} plugins state ${REQUIRED_HINTS.join(', ')}; their TTL in ${LV2_PREFIX}: <${LV2_NAMESPACE}> and their CLAP header say what each declaration says`);
  process.exit(bad ? 1 : 0);
}
