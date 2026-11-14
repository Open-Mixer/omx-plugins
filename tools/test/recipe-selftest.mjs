#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * recipe-selftest.mjs — the plugin recipe's machinery, sabotaged arm by arm (`make selftest`).
 *
 *   1. The wizard, `--from-contract wobble` against a stand-in omx-contract release carrying the
 *      wobble kernel (at the pin) and a stand-in omx-dsp instance face: the draft takes one
 *      parameter per resolve() argument, by reference, and marks every design choice REVIEW; a
 *      settled draft generates the whole folder, the binding passing each parameter to the
 *      argument it names; only the package description (prose) is left.
 *   1b. The generators: `gen.mjs --check` is green on that tree and red, naming the file or the
 *      argument, for a hand edit to the generated binding, a declaration that moved, a face whose
 *      argument no control names, and a hand edit to a shared file; each restored, green.
 *   2. Its commit plan, applied literally in a throwaway repository, keeps the commit protocol
 *      (tools/commit-plan-check.mjs); two commits swapped, or two concerns in one commit, break it.
 *   3. Every checker family of the completeness test: break the artifact on a copy of the tree
 *      (omx-drive), and the test names that entry and its wizard step; restore, and it is green.
 *      The declaration's schema check among them: a declaration with no panel, with no console
 *      block, with a second panel in its console block, whose console chip or panel names no
 *      parameter, or whose panel draws a parameter twice, is red.
 *   4. The recipe: a new required entry makes every plugin red; an entry naming a checker that does
 *      not exist breaks the recipe.
 *   5. The wizard refuses, before writing: a kernel omx-contract lacks, a kernel with no instance
 *      face or one of another shape, a REVIEW mark left, a folder of another kernel, a typed travel,
 *      an unresolved reference, a reference into another kernel, a derived field written wrong, a
 *      parameter the face takes no argument for, a panel or console naming no parameter, a panel
 *      it cannot draw; and a strict build refuses an argument that binds only by its words.
 *   6. The port hints (tools/port-hints.mjs): a TTL that drops a hint, a declaration that drops a
 *      band type's labels or declares a frequency linear, a plugin with no pin, and a pin moved in a
 *      declaration that the TTL does not carry are each named; the view follows the declarations
 *      (`gen --check` stale on a moved pin or a hand edit, and on a per-plugin pin file); the whole
 *      tree is green.
 *   7. The identity oracle: a wobble control the stand-in render lists with `rearms` (its
 *      `kernels`) is held at its default in every block of the plan, and named in the oracle's
 *      comment, while the rest move; the same render without the flag moves it again; a render
 *      that does not list the kernel is refused, never read some other way. The plan ends with a
 *      full-scale burst and a quiet tail, then each value of every choice over a burst and tail of
 *      its own with the other parameters stepped once for its second tail.
 *   8. A chain, on the real faces eq, gate and comp: `--from-contract eq,gate,comp`
 *      drafts one element per kernel and refuses a kernel with no face; the settled chain is
 *      written whole, its parameter list flattened (each element's switch, then its parameters,
 *      then `order`), and its `make test` is green: both faces bit-identical to the chain of faces
 *      called directly at every declared rate, each parameter one step off seen. The generated
 *      core with two orders swapped makes the oracle red; restored, green. gen refuses a `key` on
 *      an element whose face takes none, and a parameter of another element's kernel; held to a
 *      released tag (`shipped`), a list that differs from it is refused, each index named, unless
 *      `shippedDiff` names exactly those, and a tree with no such tag says it was not checked.
 *
 * Works in build/selftest/ (removed first). Needs git, and omx-dsp's headers as the build does.
 * Exit 1 when any check failed, including an arm that stopped early.
 */
import { execFileSync, spawnSync } from 'node:child_process';
import { cpSync, existsSync, mkdirSync, readFileSync, rmSync, unlinkSync, writeFileSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { checkCommits, rangeCommits } from '../commit-plan-check.mjs';
import { chainSources, commitPlan, draftChain, draftDeclaration, kernelSources, planChain, planPlugin, reviewMarks, writePlugin } from '../omx-new-plugin.mjs';
import { bindFace, parseFace } from '../instance-face.mjs';
import { eqDefaultBands, kernelControls, locateContract, resolveParam } from '../omx-contract.mjs';
import { omxdspInclude } from '../template.mjs';
import { defaultPanel, emitPluginTtl, generateInstance, loadDecl, meterRows } from '../gen.mjs';
import { hintErrors } from '../port-hints.mjs';
import { expandVariant, variantBands } from '../variants.mjs';
import { registerFirstContract } from '../param-contract.mjs';
import { checkPlugin, debtGrowth, debtVerdict, gapLines, loadDebt, loadRecipe, pluginStems, recipeErrors } from '../plugin-recipe.mjs';

const ROOT = resolve(fileURLToPath(import.meta.url), '..', '..', '..');
const WORK = join(ROOT, 'build', 'selftest');
let fails = 0;
const pass = (what) => console.log(`PASS ${what}`);
const fail = (what) => {
  console.log(`FAIL ${what}`);
  fails++;
};
const expect = (cond, what) => (cond ? pass(what) : fail(what));

/** A copy of this tree's files (tracked and new, never ignored) at `dst`. */
function copyTree(dst) {
  mkdirSync(dst, { recursive: true });
  // listed first, so a git that cannot read the tree fails here rather than copying nothing
  const files = execFileSync('git', ['ls-files', '-co', '--exclude-standard', '-z'], { cwd: ROOT });
  if (!files.length) throw new Error('git lists no file in this tree');
  execFileSync('sh', ['-c', 'xargs -0 tar -cf - | tar -xf - -C "$1"', 'sh', dst], { cwd: ROOT, input: files });
}

/** A stand-in omx-contract release: the installed release's resolved render, with the items of each
 * kernel given in place of that kernel's own (the rest of the tree still resolves against it), and
 * `listed` added to the render's `kernels` (each kernel's ordered controls). */
function fakeContract(dst, kernels, listed) {
  const real = locateContract(ROOT);
  if (!real.dir) throw new Error(`the stand-in starts from the installed omx-contract: ${real.why}`);
  mkdirSync(dst, { recursive: true });
  const render = JSON.parse(readFileSync(join(real.dir, 'omx-contract.json'), 'utf8'));
  for (const [k, v] of Object.entries(kernels)) {
    for (const [name, e] of Object.entries(render.items)) if (e.rel === `data/kernels/${k}.json`) delete render.items[name];
    for (const [name, e] of Object.entries(v)) render.items[name] = { rel: `data/kernels/${k}.json`, ...e };
  }
  if (listed) render.kernels = { ...(render.kernels ?? {}), ...listed };
  writeFileSync(join(dst, 'omx-contract.json'), JSON.stringify(render, null, 2));
}

const travel = (min, max, step, unit, def) => ({ kind: 'travels', shape: 'travel', value: { min, max, step, unit, default: def, defaultFrom: 'desk' } });
const WOBBLE = {
  WOBBLE_RATE_RANGE: travel(0.1, 20, 0.01, 'Hz', 4),
  WOBBLE_DEPTH_RANGE: travel(0, 100, 0.1, '%', 50),
  WOBBLE_MIX_RANGE: travel(0, 100, 0.1, '%', 100),
  WOBBLE_MODES: { kind: 'set', value: ['wobble', 'pan'], default: 'wobble' },
  // the aggregate: its field names are the controls' names (rateHz, not rate)
  WOBBLE_TRAVELS: { kind: 'travels', shape: 'table', value: { rateHz: travel(0.1, 20, 0.01, 'Hz', 4).value, depth: travel(0, 100, 0.1, '%', 50).value, mix: travel(0, 100, 0.1, '%', 100).value } },
};

/** The render's `kernels` entry for wobble: its controls in order, `depth` re-arming when asked. */
const wobbleControls = (rearms) => ({
  wobble: {
    controls: [
      { name: 'rateHz', kind: 'travel', global: 'WOBBLE_RATE_RANGE' },
      { name: 'depth', kind: 'travel', global: 'WOBBLE_DEPTH_RANGE', ...(rearms ? { rearms: true } : {}) },
      { name: 'mix', kind: 'travel', global: 'WOBBLE_MIX_RANGE' },
      { name: 'mode', kind: 'choice', global: 'WOBBLE_MODES' },
    ],
  },
});

/** A stand-in omx-dsp: the real headers, plus a wobble instance face of the generated shape. */
function fakeOmxdsp(dst, args = 'float rate_hz, float depth, float mix, int mode') {
  const real = omxdspInclude();
  if (!real) throw new Error('no omx-dsp headers (pkg-config omxdsp, or OMXDSP_INCLUDE)');
  if (!existsSync(dst)) cpSync(real, dst, { recursive: true });
  const fx = join(dst, existsSync(join(dst, 'omxdsp')) ? 'omxdsp' : '', 'fx', 'omx_wobble_instance.h');
  writeFileSync(fx, `#ifndef OMX_WOBBLE_INSTANCE_H
#define OMX_WOBBLE_INSTANCE_H
#include <stdint.h>
typedef struct { float sr; } OmxWobbleInstance;
#define OMX_WOBBLE_INSTANCE_LATENCY_FRAMES 0.0f
static inline int omx_wobble_instance_init(OmxWobbleInstance *s, float sr) { s->sr = sr; return 1; }
static inline void omx_wobble_instance_resolve(OmxWobbleInstance *s, int bypass, ${args}) { (void)s; (void)bypass; }
static inline void omx_wobble_instance_run(OmxWobbleInstance *s, const float *in_l, const float *in_r, float *out_l,
                                            float *out_r, uint32_t n) { (void)s; (void)in_l; (void)in_r; (void)out_l; (void)out_r; (void)n; }
#endif
`);
  return fx;
}

const git = (repo, ...args) => execFileSync('git', ['-C', repo, '-c', 'user.name=selftest', '-c', 'user.email=selftest@invalid', '-c', 'commit.gpgsign=false', ...args], { encoding: 'utf8' });

/** A throwaway repository holding `base`, then each commit of `commits` copied from `from`. */
function applyPlan(repo, base, from, commits) {
  copyTree(repo);
  git(repo, 'init', '-q');
  git(repo, 'add', '-A');
  git(repo, 'commit', '-q', '-m', 'base');
  for (const c of commits) {
    for (const f of c.files) {
      mkdirSync(join(repo, f, '..'), { recursive: true });
      cpSync(join(from, f), join(repo, f));
    }
    git(repo, 'add', ...c.files);
    git(repo, 'commit', '-q', '-m', c.message);
  }
  return checkCommits(loadRecipe(ROOT), rangeCommits(repo, 'HEAD~' + commits.length + '..HEAD'));
}

async function main() {
  rmSync(WORK, { recursive: true, force: true });
  mkdirSync(WORK, { recursive: true });
  const recipe = loadRecipe(ROOT);
  const realInc = omxdspInclude();
  const contract = join(WORK, 'omx-contract');
  fakeContract(contract, { wobble: WOBBLE }, wobbleControls(false));
  const inc = join(WORK, 'omxdsp-include');
  const face = fakeOmxdsp(inc);
  process.env.OMX_CONTRACT_DIR = contract;
  process.env.OMXDSP_INCLUDE = inc;

  // ---- 1. the wizard -------------------------------------------------------------------------
  const tree = join(WORK, 'wizard');
  copyTree(tree);
  const src = kernelSources('wobble', { root: tree });
  expect(!src.refusals.length, `the wizard finds the wobble kernel and its face${src.refusals.length ? `: ${src.refusals.map((r) => r.reason).join('; ')}` : ''}`);
  if (src.refusals.length) return;
  const draft = draftDeclaration('wobble', src, { root: tree, recipe });
  expect(draft.decl?.params.map((p) => `${p.symbol}:${p.name}`).join(' ') === 'rateHz:Rate depth:Depth mix:Mix mode:Mode',
    `the draft has one parameter per resolve() argument, named by the contract (${draft.decl?.params.map((p) => `${p.symbol}:${p.name}`).join(' ')})`);
  expect(draft.decl?.params.every((p) => p.ref && !('min' in p) && !('def' in p)), 'the draft is by reference only');
  const marks = reviewMarks(draft.decl);
  expect(marks.join(' ') === '/description /clap/features/1 /lv2/class /params/3/values/0 /params/3/values/1 /console/placement/group',
    `the draft marks every design choice REVIEW (${marks.join(' ')})`);
  const unsettled = await planPlugin(draft.decl, src, { root: tree, recipe });
  expect(!unsettled.ok && unsettled.refusals.length === marks.length, `a draft with REVIEW marks is refused, each named (${unsettled.refusals.map((r) => r.field).join(' ')})`);
  const settled = structuredClone(draft.decl);
  settled.description = 'The console\'s wobble: the level, or the balance, moved by one LFO.';
  settled.clap.features[1] = 'wobble';
  settled.lv2.class = 'lv2:ModulatorPlugin';
  settled.params[3].values = ['Wobble', 'Pan'];
  settled.console.placement = { group: 'plugins' };
  const answers = settled; // what section 5 mutates
  const plan = await planPlugin(settled, src, { root: tree, recipe });
  expect(plan.ok, `the settled draft is accepted${plan.ok ? '' : `: ${plan.refusals.map((r) => `${r.field} ${r.reason}`).join('; ')}`}`);
  if (!plan.ok) return;
  const written = writePlugin(plan, { root: tree });
  const P_ = 'plugins/omx-wobble';
  for (const f of ['omx-wobble.decl.json', 'Makefile', 'omx_wobble_clap.c', 'omx_wobble_lv2.c', 'omx_wobble_core.h', 'test/wobble-oracle.c', 'generated/omx_wobble_params.h'])
    expect(written.includes(`${P_}/${f}`), `the wizard wrote ${P_}/${f}`);
  for (const f of ['docs/catalogue.md', 'packaging/omx-plugins.spec', '.github/workflows/ci.yml']) expect(written.includes(f), `the shared file ${f} lists the new plugin`);
  // a new plugin's contract version is 1: the wizard wrote its fingerprint where tools/gen.mjs reads the version
  const VERSIONS = 'tools/param-contract-versions.json';
  const versionsOf = () => JSON.parse(readFileSync(join(tree, VERSIONS), 'utf8')).plugins;
  const first = versionsOf()['omx-wobble'];
  expect(written.includes(VERSIONS) && first?.length === 1 && /^[0-9a-f]{64}$/.test(first[0]), `the wizard wrote the new plugin's first contract fingerprint in ${VERSIONS} (${JSON.stringify(first)})`);
  expect(JSON.stringify(Object.keys(versionsOf())) === JSON.stringify(Object.keys(versionsOf()).sort()), `${VERSIONS} keeps its plugins in name order`);
  {
    const was = readFileSync(join(tree, VERSIONS), 'utf8');
    const wrote = ['omx-wobble', 'omx-drive'].map((stem) => registerFirstContract(tree, stem, 'f'.repeat(64)));
    expect(!wrote.some(Boolean) && readFileSync(join(tree, VERSIONS), 'utf8') === was, 'sabotage: a plugin the file already lists is never registered again, the file left as it was');
  }
  const header = readFileSync(join(tree, P_, 'generated/omx_wobble_params.h'), 'utf8');
  expect(header.includes('#define OMX_WOBBLE_PARAM_CONTRACT_VERSION 1u'), 'the generated header serves contract version 1');
  expect(header.includes('{ "rateHz", "Rate", "Hz", 0.1f, 20.0f, 4.0f, 0u }'), 'the generated table carries the kernel file\'s rate travel');
  const core = readFileSync(join(tree, P_, 'omx_wobble_core.h'), 'utf8');
  expect(core.includes('/* rate_hz */ values[OMX_WOBBLE_PARAM_RATE_HZ]') && core.includes('/* mode */ (int)lrintf(values[OMX_WOBBLE_PARAM_MODE])') && !/OMX_WIZARD_STUB/.test(core),
    'the generated binding passes each parameter to the resolve() argument it names, a choice as an int');
  // The scratch tree's package prose may already name wobble (the real catalogue does); take it out so the arm
  // proves a new plugin's missing description is caught.
  for (const f of ['debian/control', 'packaging/omx-plugins.spec']) {
    const p = join(tree, f);
    writeFileSync(p, readFileSync(p, 'utf8').replace(/(?<![-_])\bwobble\b,?[ \t]?/gi, ''));
  }
  const owed = gapLines(await checkPlugin(tree, recipe, 'omx-wobble'));
  expect(owed.length === 1 && owed[0].startsWith('omx-wobble: package-description missing'), `a generated plugin owes only the package description, prose a person writes (${owed.map((g) => g.split(' (')[0]).join('; ')})`);

  // ---- 1b. the generators, sabotaged input by input --------------------------------------------
  const check = () => spawnSync('node', [join(tree, 'tools', 'gen.mjs'), '--check'], { cwd: tree, encoding: 'utf8' });
  const green = check();
  expect(green.status === 0, `gen.mjs --check is green on the wizard's tree${green.status ? `: ${green.stderr.trim().split('\n')[0]}` : ''}`);
  const sabotageGen = (what, file, mutate, needle) => {
    const at = join(file.startsWith('/') ? '' : tree, file);
    const was = readFileSync(at, 'utf8');
    writeFileSync(at, mutate(was));
    const red = check();
    writeFileSync(at, was);
    const back = check();
    const out = `${red.stdout}${red.stderr}`;
    expect(red.status !== 0 && out.includes(needle) && back.status === 0, `sabotage gen: ${what} goes red naming ${needle}, green restored${red.status ? '' : ' (STAYED GREEN)'}`);
  };
  sabotageGen('a hand edit to the generated binding', `${P_}/omx_wobble_core.h`, (t) => t.replace('/* depth */', '/* depth (tuned) */'), `STALE ${P_}/omx_wobble_core.h`);
  sabotageGen('a hand edit to the generated identity test', `${P_}/test/wobble-oracle.c`, (t) => t.replace('no tolerance', 'a little tolerance'), `STALE ${P_}/test/wobble-oracle.c`);
  // the wizard writes the plugin described: a control's name is its descriptor's
  sabotageGen('a descriptor that moved a control\'s name', `${P_}/omx-wobble.descriptor.json`, (t) => t.replace('"name": "Depth"', '"name": "Depth Amount"'), `STALE ${P_}/generated/omx_wobble_params.h`);
  sabotageGen('a face whose argument no contract control names', face, (t) => t.replace('float depth,', 'float intensity,'), "resolve argument 'intensity'");
  // a listed plugin whose rows are not its listed fingerprint is refused, never registered again
  sabotageGen('a contract fingerprint the file does not list', VERSIONS, (t) => {
    const j = JSON.parse(t);
    j.plugins['omx-wobble'] = ['f'.repeat(64)];
    return `${JSON.stringify(j, null, 2)}\n`;
  }, `plugins/omx-wobble: param-contract fingerprint ${first[0]} is not in ${VERSIONS}: append it, the next version (2)`);
  sabotageGen('a hand edit to a shared file', 'docs/catalogue.md', (t) => t.replace('| **omx wobble** |', '| **omx wobble (beta)** |'), 'STALE docs/catalogue.md');
  {
    const was = readFileSync(join(tree, P_, 'omx_wobble_core.h'), 'utf8');
    writeFileSync(join(tree, P_, 'omx_wobble_core.h'), `${was}\n`);
    const r = gapLines(await checkPlugin(tree, recipe, 'omx-wobble'));
    writeFileSync(join(tree, P_, 'omx_wobble_core.h'), was);
    expect(r.some((l) => l.startsWith('omx-wobble: instance-files missing')), `sabotage completeness: a stale generated binding is the gap instance-files (${r.length} gaps)`);
  }

  // ---- 2. the commit plan, applied literally ------------------------------------------------
  const commits = commitPlan(recipe, plan, written);
  const order = recipe.layers.map((l) => l.id);
  expect(commits.every((c, i) => i === 0 || order.indexOf(c.layer) > order.indexOf(commits[i - 1].layer)), `the plan is in layer order (${commits.map((c) => c.layer).join(' → ')})`);
  const good = applyPlan(join(WORK, 'plan-ok'), ROOT, tree, commits);
  expect(good.ok, `the plan applied literally keeps the commit protocol${good.ok ? '' : `: ${good.violations.join('; ')}`}`);
  const iContract = commits.findIndex((c) => c.layer === 'contract');
  const iCode = commits.findIndex((c) => c.layer === 'code');
  const swapped = [...commits];
  [swapped[iContract], swapped[iCode]] = [swapped[iCode], swapped[iContract]];
  const bad = applyPlan(join(WORK, 'plan-swapped'), ROOT, tree, swapped);
  expect(!bad.ok && bad.violations.some((v) => /layer contract after code/.test(v)), `sabotage: faces before the declaration is refused (${bad.violations[0] ?? 'no violation'})`);
  const merged = [{ ...commits[iContract], files: [...commits[iContract].files, ...commits[iCode].files] }, ...commits.filter((_c, i) => i !== iContract && i !== iCode)];
  const two = applyPlan(join(WORK, 'plan-merged'), ROOT, tree, merged);
  expect(!two.ok && two.violations.some((v) => /2 concerns in one commit/.test(v)), 'sabotage: declaration and faces in one commit is refused');

  const none = checkCommits(recipe, rangeCommits(join(WORK, 'plan-ok'), 'HEAD..HEAD'));
  expect(!none.ok && none.violations.some((v) => /holds no commit/.test(v)), 'sabotage: an empty commit range fails the protocol check');

  // ---- 3. every checker family, broken on a copy of omx-drive --------------------------------
  delete process.env.OMX_CONTRACT_DIR;
  delete process.env.OMXDSP_INCLUDE;
  const t = join(WORK, 'sabotage');
  copyTree(t);
  const P = 'plugins/omx-drive';
  const base = await checkPlugin(t, recipe, 'omx-drive');
  expect(gapLines(base).length === 0, `baseline: omx-drive is complete${gapLines(base).length ? `: ${gapLines(base).join('; ')}` : ''}`);
  const edit = (f, fn) => {
    const at = join(t, f);
    const was = readFileSync(at, 'utf8');
    writeFileSync(at, fn(was));
    return () => writeFileSync(at, was);
  };
  const add = (f, text) => {
    writeFileSync(join(t, f), text);
    return () => unlinkSync(join(t, f));
  };
  const remove = (f) => {
    const was = readFileSync(join(t, f));
    unlinkSync(join(t, f));
    return () => writeFileSync(join(t, f), was);
  };
  const dspName = (() => {
    const inc = realInc;
    const h = readFileSync(join(inc, 'omxdsp', 'omx_lfo.h'), 'utf8');
    return h.match(/static inline \w+ (omx_lfo_[a-z_]+)\s*\(/)[1];
  })();
  const SABOTAGE = [
    ['declValid', 'declaration', () => edit(`${P}/omx-drive.descriptor.json`, (s) => s.replace('"org.openmixer.drive"', '"org.openmixer.delay"'))],
    ['generatedFresh', 'params-header', () => edit(`${P}/generated/omx_drive_params.h`, (s) => `${s}\n`)],
    ['generatedFresh', 'lv2-description', () => edit(`${P}/generated/omx-drive.lv2/omx-drive.ttl`, (s) => s.replace('lv2:default 0 ;', 'lv2:default 1 ;'))],
    ['modguiFresh', 'modgui', () => add(`${P}/generated/omx-drive.lv2/modgui/stray.css`, '')],
    ['makefile', 'makefile', () => edit(`${P}/Makefile`, (s) => s.replace('urn:openmixer:drive', 'urn:openmixer:drives'))],
    // the generated faces reach the table directly and through the binding: both includes go
    ['face', 'clap-face', () => edit(`${P}/omx_drive_clap.c`, (s) => s.replace('#include "omx_drive_params.h"', '').replace('#include "omx_drive_core.h"', ''))],
    ['face', 'lv2-face', () => edit(`${P}/omx_drive_lv2.c`, (s) => s.replace('lv2_descriptor(uint32_t', 'lv2_descriptor_gone(uint32_t'))],
    ['kernelBinding', 'kernel-binding', () => edit(`${P}/omx_drive_core.h`, (s) => `#define OMX_WIZARD_STUB 1\n${s}`)],
    ['kernelBinding', 'kernel-binding', () => {
      // only omx_denormal.h left: an omx-dsp header, but not the plugin's own kernel
      const undo = ['omx_drive_core.h'].map((f) => edit(`${P}/${f}`, (s) => s.replace(/#include <omxdsp\/fx\/omx_drive_instance\.h>\n/, '')));
      return () => undo.forEach((u) => u());
    }],
    ['kernelBinding', 'kernel-binding', () => edit(`${P}/omx-drive.decl.json`, (s) => s.replace(/"kernels": \[[^\]]*\]/, '"kernels": ["drive", "delay"]'))],
    ['makeTestRuns', 'parameter-test', () => edit(`${P}/Makefile`, (s) => s.replace(/^.*clap-params-check\.mjs.*\n/m, ''))],
    ['oracle', 'kernel-identity-test', () => remove(`${P}/test/drive-oracle.c`)],
    ['sharedListed', 'spec-files', () => edit('packaging/omx-plugins.spec', (s) => s.replace('%{_libdir}/lv2/omx-drive.lv2/\n', ''))],
    ['installCovers', 'deb-install', () => edit('debian/omx-plugins-clap.install', () => 'usr/lib/clap/omx-delay.clap\n')],
    ['namedIn', 'package-description', () => edit('debian/control', (s) => s.replace(/delay, drive,/g, 'delay,'))],
    ['sharedListed', 'ci-installed-files', () => edit('.github/workflows/ci.yml', (s) => s.replaceAll(' drive ', ' '))],
    ['sharedListed', 'static-table', () => edit('lib/omx_clap_static.c', (s) => s.replace('{"org.openmixer.drive", &omx_clap_entry_drive},\n', ''))],
    ['sharedListed', 'catalogue-row', () => edit('docs/catalogue.md', (s) => s.replace('`org.openmixer.drive`', '`org.openmixer.x`'))],
    ['sharedListed', 'manual-section', () => edit('docs/catalogue.md', (s) => s.replace('### omx drive\n', '### drive\n'))],
    ['noFiles', 'no-cpp', () => add(`${P}/shell.cpp`, '// SPDX-License-Identifier: GPL-3.0-or-later\n')],
    ['noText', 'no-dpf', () => add(`${P}/dpf_shell.h`, '#include "DistrhoPlugin.hpp"\n')],
    ['noCopiedDsp', 'no-copied-dsp', () => add(`${P}/copied.h`, `static inline float ${dspName}(float x) {\n  return x;\n}\n`)],
    ['noCopiedDsp', 'no-copied-dsp', () => add(`${P}/engine.h`, '#include "mix_drive.h"\n')],
    // every plugin declares its one panel and its console block, each naming declared parameters only
    ['declValid', 'declaration', () => edit(`${P}/omx-drive.decl.json`, (s) => JSON.stringify((({ panel: _p, ...d }) => d)(JSON.parse(s)), null, 2))],
    ['declValid', 'declaration', () => edit(`${P}/omx-drive.decl.json`, (s) => JSON.stringify((({ console: _c, ...d }) => d)(JSON.parse(s)), null, 2))],
    ['declValid', 'declaration', () => edit(`${P}/omx-drive.decl.json`, (s) => s.replace('"chip": "{amount}"', '"chip": "{drive}"'))],
    ['declValid', 'declaration', () => edit(`${P}/omx-drive.decl.json`, (s) => s.replace('"bandFreq",\n          "mix"', '"bandFreq",\n          "wet"'))],
    ['declValid', 'declaration', () => edit(`${P}/omx-drive.decl.json`, (s) => s.replace('"bandFreq",\n          "mix"', '"bandFreq",\n          "amount"'))],
    ['declValid', 'declaration', () => edit(`${P}/omx-drive.decl.json`, (s) => JSON.stringify((({ console: c, panel: p, ...d }) => ({ ...d, panel: p, console: { ...c, panel: { sections: p.sections } } }))(JSON.parse(s)), null, 2))],
    ['declValid', 'declaration', () => edit(`${P}/omx-drive.decl.json`, (s) => s.replace('"family": "drive"', '"family": "saturation"'))],
    ['declValid', 'declaration', () => edit(`${P}/omx-drive.decl.json`, (s) => s.replace('"tone": "character"', '"character": "character"'))],
    // the schema's roles are omx-panel's table (ROLES): a role added to one family in the schema, or taken from one, is red
    ['declValid', 'declaration', () => edit('schema/plugin.decl.schema.json', (s) => s.replace('"range",\n            "attack"', '"range",\n            "wobble",\n            "attack"'))],
    ['declValid', 'declaration', () => edit('schema/plugin.decl.schema.json', (s) => s.replace('"lookahead",\n            "mix",\n            "output"\n          ]\n        },\n        "limiter"', '"mix",\n            "output"\n          ]\n        },\n        "limiter"'))],
    // a control both read by reference (its descriptor link) and the face's own (its extras' reason)
    ['paramsByReference', 'params-by-reference', () => edit(`${P}/omx-drive.decl.json`, (s) => s.replace('"controls": {\n', '"controls": {\n    "amount": { "own": "switch" },\n'))],
  ];
  // Each arm checks the entry it breaks, not the whole plugin (omx-plugins#39: two whole checks an arm, ~36 s each, an
  // entry alone at most ~2.4 s): green before the break, so an arm never passes on a red a former arm left; red,
  // naming its step, while broken; green once restored. After every arm, one whole check: the tree is whole again.
  const only = (id) => ({ ...recipe, artifacts: recipe.artifacts.filter((e) => e.id === id), laws: recipe.laws.filter((e) => e.id === id) });
  for (const [family, id, breakIt] of SABOTAGE) {
    const before = gapLines(await checkPlugin(t, only(id), 'omx-drive'));
    const restore = breakIt();
    const r = await checkPlugin(t, only(id), 'omx-drive');
    const entry = r.results.find((x) => x.id === id);
    const line = gapLines(r).find((l) => l.startsWith(`omx-drive: ${entry?.kind === 'law' ? 'law ' : ''}${id} missing (wizard step '${entry?.step}'`));
    restore();
    const back = gapLines(await checkPlugin(t, only(id), 'omx-drive'));
    expect(entry && before.length === 0 && line && back.length === 0,
      `sabotage ${family}: ${id} green, then red naming step '${entry?.step}', green restored${line ? ` — ${line.slice(0, 140)}` : ''}`);
  }
  const whole3 = gapLines(await checkPlugin(t, recipe, 'omx-drive'));
  expect(whole3.length === 0, `every sabotage restored: omx-drive is complete again${whole3.length ? `: ${whole3.join('; ')}` : ''}`);

  // ---- 3b. the debt ratchet, both directions ---------------------------------------------------
  // checkPlugin reads the tree and never the debt (only debtVerdict does), so the whole tree is checked ONCE: the arms
  // that change only the debt list judge those same reports, and the arm that edits docs/catalogue.md re-checks, for
  // every plugin, the entries that read that file (a sharedListed entry compares its own checker.file and nothing
  // else), merged over the whole reports by id: the verdict of the edited tree, entry for entry, for the cost of those
  // entries (omx-plugins#39: each arm used to re-check every plugin, ~10 minutes each).
  const wholeReports = [];
  for (const st of pluginStems(t)) wholeReports.push(await checkPlugin(t, recipe, st));
  const ratchet = (reports, debt, known) => debtVerdict(reports, debt, known);
  const whole = ratchet(wholeReports, loadDebt(t));
  expect(!whole.fresh.length && !whole.stale.length && whole.held.length === loadDebt(t).length,
    `the debt holds today's gaps exactly (${whole.held.length} held, ${whole.fresh.length} new, ${whole.stale.length} stale)`);
  const reading = (file) => ({ ...recipe, artifacts: recipe.artifacts.filter((e) => e.checker?.file === file || (e.paths ?? []).includes(file)),
    laws: recipe.laws.filter((e) => e.checker?.file === file) });
  const recheck = async (file) => {
    const only = reading(file);
    const out = [];
    for (const r of wholeReports) {
      const moved = new Map((await checkPlugin(t, only, r.stem)).results.map((x) => [`${x.kind} ${x.id}`, x]));
      out.push({ ...r, results: r.results.map((x) => moved.get(`${x.kind} ${x.id}`) ?? x) });
    }
    return out;
  };
  expect(reading('docs/catalogue.md').artifacts.length > 0, `the entries that read docs/catalogue.md are found (${reading('docs/catalogue.md').artifacts.map((e) => e.id).join(', ')})`);
  let undo = edit('docs/catalogue.md', (s) => s.replace('### omx drive\n', '### drive\n'));
  const unheldReports = await recheck('docs/catalogue.md');
  undo();
  const unheld = ratchet(unheldReports, loadDebt(t));
  expect(unheld.fresh.includes("omx-drive manual-section"), `sabotage: a gap the debt does not hold fails the ratchet (${unheld.fresh.join(", ")})`);
  // An entry owed by a plugin that has the artifact: written here, so the arm does not depend on
  // which plugin still owes something.
  const owedEntry = '{ "plugin": "omx-drive", "entry": "makefile", "owedBy": "selftest" }';
  undo = edit('recipes/completeness-debt.json', (s) => (/"debt": \[\s*\]/.test(s) ? s.replace(/"debt": \[\s*\]/, `"debt": [\n    ${owedEntry}\n  ]`) : s.replace('"debt": [', `"debt": [\n    ${owedEntry},`)));
  const paid = ratchet(wholeReports, loadDebt(t));
  undo();
  expect(paid.stale.some((x) => x.startsWith('omx-drive makefile')), `sabotage: a debt entry now satisfied fails the ratchet as stale (${paid.stale.join(', ')})`);

  const ghost = ratchet(wholeReports, [...loadDebt(t), { plugin: 'omx-ghost', entry: 'makefile', owedBy: 'nobody' }], pluginStems(t));
  expect((ghost.unknown ?? []).length === 1 && ghost.unknown[0].startsWith('omx-ghost makefile'), `sabotage: a debt entry for a plugin that does not exist fails the ratchet (${(ghost.unknown ?? []).join(', ')})`);
  {
    // an empty plugin set: the run itself fails, rather than passing over nothing
    const empty = join(WORK, 'empty');
    mkdirSync(join(empty, 'plugins'), { recursive: true });
    cpSync(join(ROOT, 'recipes'), join(empty, 'recipes'), { recursive: true });
    cpSync(join(ROOT, 'schema'), join(empty, 'schema'), { recursive: true });
    mkdirSync(join(empty, 'packaging'), { recursive: true });
    cpSync(join(ROOT, 'packaging', 'omx-plugins.spec'), join(empty, 'packaging', 'omx-plugins.spec'));
    const run = spawnSync('node', [join(ROOT, 'tools', 'plugin-recipe.mjs')], { env: { ...process.env, OMX_PLUGINS_ROOT: empty }, encoding: 'utf8' });
    expect(run.status === 1 && /no plugin to check/.test(run.stdout + run.stderr), `sabotage: a run over no plugin fails (exit ${run.status})`);
  }

  {
    // the debt against its merge-base: a throwaway repository whose origin/main holds today's debt
    const repo = join(WORK, 'growth');
    copyTree(repo);
    git(repo, 'init', '-q');
    git(repo, 'add', '-A');
    git(repo, 'commit', '-q', '-m', 'main');
    git(repo, 'update-ref', 'refs/remotes/origin/main', 'HEAD');
    git(repo, 'checkout', '-q', '-b', 'topic');
    const level = debtGrowth(repo, loadDebt(repo));
    expect(level.base === loadDebt(repo).length && !level.grown, `the debt as main holds it has not grown (${level.base} entries at the merge-base)`);
    const more = [...loadDebt(repo), { plugin: 'omx-drive', entry: 'manual-section', owedBy: 'sabotage' }];
    const grew = debtGrowth(repo, more);
    expect(grew.grown && grew.added.join() === 'omx-drive manual-section', `sabotage: a debt entry added since main fails the growth check (${grew.added.join()})`);
    const fewer = debtGrowth(repo, loadDebt(repo).slice(1));
    expect(!fewer.grown, 'a debt that shrank passes the growth check');
    git(repo, 'update-ref', '-d', 'refs/remotes/origin/main');
    const none = debtGrowth(repo, loadDebt(repo));
    expect(none.base === undefined && /no merge-base/.test(none.why), `no main to compare to is reported, never a silent pass (${none.why?.slice(0, 60)})`);
  }

  // ---- 4. the recipe itself ----------------------------------------------------------------
  const grown = JSON.parse(JSON.stringify(recipe));
  grown.artifacts.push({ id: 'sabotage-notes', what: 'a section every plugin now owes', step: 'docs', layer: 'docs', made: 'HAND-WRITTEN', target: 'docs/catalogue.md', paths: [], questions: [], template: null, when: 'always', checker: { fn: 'sharedListed', file: 'docs/catalogue.md', region: 'sections', needle: '### {name} notes' } });
  let allRed = true;
  for (const s of ['omx-delay', 'omx-drive', 'omx-eq8', 'omx-strip']) {
    if (!gapLines(await checkPlugin(t, grown, s)).some((l) => l.includes('sabotage-notes missing'))) allRed = false;
  }
  expect(allRed, 'sabotage: one new required entry in the recipe makes every plugin red');
  const broken = JSON.parse(JSON.stringify(recipe));
  broken.artifacts[0].checker.fn = 'noSuchChecker';
  expect(recipeErrors(broken).some((e) => e.includes('noSuchChecker')), 'sabotage: an entry naming a missing checker breaks the recipe');
  const unruled = JSON.parse(JSON.stringify(recipe));
  unruled.laws[0].when = 'someday';
  expect(recipeErrors(unruled).some((e) => e.includes("'someday'")), 'sabotage: an entry naming an undeclared rule breaks the recipe');

  // ---- 5. the wizard refuses before writing ------------------------------------------------------
  process.env.OMX_CONTRACT_DIR = contract;
  process.env.OMXDSP_INCLUDE = inc;
  const fresh = join(WORK, 'refusals');
  copyTree(fresh);
  const srcFresh = kernelSources('wobble', { root: fresh });
  const refused = async (what, mutate, field) => {
    const a = structuredClone(answers);
    mutate(a);
    const p = await planPlugin(a, srcFresh, { root: fresh, recipe });
    const r = p.refusals.find((x) => x.field.startsWith(field));
    expect(!p.ok && r, `wizard refuses ${what}: ${r ? `${r.field} ${r.reason}` : 'NOT REFUSED'}`);
  };
  const sourceRefused = (what, kernel, field, needle) => {
    const r = kernelSources(kernel, { root: fresh }).refusals.find((x) => x.field === field);
    expect(r && r.reason.includes(needle), `wizard refuses ${what}: ${r ? r.reason.slice(0, 120) : 'NOT REFUSED'}`);
  };
  sourceRefused('a kernel omx-contract lacks', 'nosuch', 'omx-contract', 'has no data/kernels/nosuch.json');
  // balance: a contract kernel omx-dsp gives no instance face (delay, chorus and the rest have one now)
  sourceRefused('a kernel with no instance face', 'balance', 'omx-dsp', 'has no <omxdsp/fx/omx_balance_instance.h>');
  {
    // A face whose init is handed the rings, the shape the faces had before the generator's: written
    // here, so the arm does not depend on which of omx-dsp's faces still has it.
    const f = parseFace(`static inline int omx_wobble_instance_init(OmxWobbleInstance *s, float sr, float *ring_l, float *ring_r, uint32_t cap) { return 1; }
static inline void omx_wobble_instance_resolve(OmxWobbleInstance *s, int bypass, float rate, float depth) { }
static inline void omx_wobble_instance_run(OmxWobbleInstance *s, const float *in_l, const float *in_r, float *out_l, float *out_r, uint32_t n) { }
#define OMX_WOBBLE_INSTANCE_LATENCY_FRAMES 0.0f`, 'wobble');
    expect(/hands in/.test(f.error ?? ''), `a face of another shape is named, not guessed (rings handed to init: ${f.error ?? 'ACCEPTED'})`);
  }
  await refused('a REVIEW mark left', (a) => { a.lv2.class = 'REVIEW: the LV2 class'; }, '/lv2/class');
  await refused('a plugin not generated from the face', (a) => { delete a.binding; }, '/binding');
  await refused('a folder that is another kernel\'s', (a) => { a.stem = 'omx-drive'; delete a.clap.id; delete a.lv2.uri; delete a.name; }, '/stem');
  await refused('another plugin\'s LV2 URI', (a) => { a.stem = 'omx-trem'; a.lv2.uri = 'urn:openmixer:drive'; delete a.clap.id; delete a.name; }, '/lv2/uri');
  await refused('a typed travel', (a) => { a.params[0].min = 0.1; }, '/params/0');
  await refused('an unresolved reference', (a) => { a.params[1].ref = 'WOBBLE_SPEED_RANGE'; }, '/params/1/ref');
  await refused('a reference into another kernel', (a) => { a.params[0].ref = 'FX_DELAY_TIME_RANGE'; }, '/params/0/ref');
  await refused('a derived field written wrong', (a) => { a.clap.id = 'org.openmixer.trem'; }, '/clap/id');
  await refused('a face argument no parameter binds', (a) => { a.params.splice(2, 1); }, '/params');
  await refused('a panel naming no parameter', (a) => { a.panel = { family: 'modulation', roles: { rate: 'rateHz' }, sections: [{ key: 'wobble', label: 'Wobble', controls: ['rateHz', 'speed'] }] }; }, '/panel/sections');
  await refused('a console chip naming no parameter', (a) => { a.console = { placement: { strips: ['input'], group: 'insert' }, chip: '{rateHz} · {speed}' }; }, '/console/chip');
  await refused('a widget outside the shared list', (a) => { a.panel = { ...a.panel, widgets: { rateHz: 'choice' } }; }, '/panel/widgets/rateHz');
  await refused('a widget that cannot draw its parameter', (a) => { a.panel = { ...a.panel, widgets: { mode: 'knob' } }; }, '/panel');
  await refused('a switch for a travel', (a) => { a.panel = { ...a.panel, widgets: { depth: 'switch' } }; }, '/panel');
  {
    // the declared widgets reach omx-panel in the panel file, which draws a control with the widget it names, else its kind's
    const { modgui } = await import(join(ROOT, 'tools', 'panel-file.mjs'));
    const d = loadDecl(join(ROOT, 'plugins', 'omx-drive'));
    const files = (widgets) => modgui({ ...d, panel: { ...d.panel, widgets } }, '--emit-stdout');
    const auto = files(undefined), named = files({ amount: 'fader', curve: 'selector' });
    expect(auto.status === 0 && named.status === 0, `modgui: omx-panel draws the declared panel (${auto.stderr || named.stderr})`);
    expect(/omx-knob-image" mod-role="input-control-port" mod-port-symbol="amount"/.test(auto.stdout) && !auto.stdout.includes('omx-fader.png'),
      'modgui: a control the panel names no widget for takes its kind\'s (amount a knob, no fader film)');
    expect(/omx-fader-image" mod-role="input-control-port" mod-port-symbol="amount"/.test(named.stdout) && named.stdout.includes('omx-fader.png') &&
      named.stdout.includes('.omx-fader-image') && named.stdout.includes('mod-port-symbol="curve" mod-widget="custom-select"'),
      'modgui: the panel\'s widgets are drawn (amount a fader with its film, curve a selector)');
  }
  await refused('a panel that leaves a parameter off the MOD GUI', (a) => {
    a.panel = { family: 'modulation', roles: { rate: 'rateHz' }, sections: [{ key: 'wobble', label: 'Wobble', controls: ['rateHz', 'depth', 'mix'] }] };
  }, '/panel');
  const panelOf = (family, roles) => ({ family, roles, sections: [{ key: 'wobble', label: 'Wobble', controls: ['rateHz', 'depth', 'mix', 'mode'] }] });
  await refused('a panel family the console does not know', (a) => { a.panel = panelOf('wobble', {}); }, '/panel/family');
  await refused('a role outside its family', (a) => { a.panel = panelOf('modulation', { rate: 'rateHz', time: 'depth' }); }, '/panel/roles/time');
  await refused('a family with none of its roles', (a) => { a.panel = panelOf('modulation', {}); }, '/panel/roles');
  await refused('a list of symbols for a role that does not repeat by band', (a) => { a.panel = panelOf('modulation', { rate: ['rateHz', 'depth'] }); }, '/panel/roles/rate');
  await refused('a role naming no parameter', (a) => { a.panel = panelOf('modulation', { rate: 'speed' }); }, '/panel/roles/rate');
  {
    const f = parseFace(`static inline int omx_wobble_instance_init(OmxWobbleInstance *s, float sr) { return 1; }
static inline void omx_wobble_instance_resolve(OmxWobbleInstance *s, int bypass, float rate, float depth) { }
static inline void omx_wobble_instance_run(OmxWobbleInstance *s, const float *in_l, const float *in_r, float *out_l, float *out_r, uint32_t n) { }
#define OMX_WOBBLE_INSTANCE_LATENCY_FRAMES 0.0f`, 'wobble');
    const ps = [{ symbol: 'rateHz' }, { symbol: 'depth' }];
    const loose = bindFace(f, ps, [{ name: 'rateHz' }, { name: 'depth' }], { strict: false });
    const strict = bindFace(f, ps, [{ name: 'rateHz' }, { name: 'depth' }], { strict: true });
    expect(!loose.errors.length && loose.renames[0]?.rename === 'rate_hz' && strict.errors.some((e) => e.includes("'rate'")),
      `an argument that binds only by its words is a rename owed, refused when strict (${loose.renames.map((r) => `${r.arg}->${r.rename}`).join(', ')}; ${strict.errors[0] ?? 'NOT REFUSED'})`);
  }
  {
    const face = (arg) => parseFace(`static inline int omx_wobble_instance_init(OmxWobbleInstance *s, float sr) { return 1; }
static inline void omx_wobble_instance_resolve(OmxWobbleInstance *s, int bypass, ${arg}, float mix) { }
static inline void omx_wobble_instance_run(OmxWobbleInstance *s, const float *in_l, const float *in_r, float *out_l, float *out_r, uint32_t n) { }
#define OMX_WOBBLE_INSTANCE_LATENCY_FRAMES 0.0f`, 'wobble');
    const ps = [{ symbol: 'band1' }, { symbol: 'mix' }, { symbol: 'band2' }, { symbol: 'band3' }];
    const cs = [{ name: 'band' }, { name: 'mix' }, { name: 'band' }, { name: 'band' }];
    const b = bindFace(face('const float band[OMX_WOBBLE_BANDS]'), ps, cs);
    const arr = b.binding.find((x) => x.arg === 'band');
    expect(!b.errors.length && arr?.extent === 'OMX_WOBBLE_BANDS' && arr.params.map((p) => p.symbol).join() === 'band1,band2,band3',
      `a per-band array argument binds every parameter of its control, in declaration order (${arr?.params?.map((p) => p.symbol).join() ?? b.errors[0]})`);
    const literal = face('const float band[3]');
    expect(/not one scalar/.test(literal.error ?? ''), `a per-band array of literal extent is named, not bound (${literal.error ?? 'ACCEPTED'})`);
  }
  {
    // a keyed face takes the key block first, and only a block named `key` is read as one
    const face = (key) => `static inline int omx_wobble_instance_init(OmxWobbleInstance *s, float sr) { return 1; }
static inline void omx_wobble_instance_resolve(OmxWobbleInstance *s, int bypass, float depth) { }
static inline void omx_wobble_instance_run(OmxWobbleInstance *s, const float *${key}, const float *in_l, const float *in_r, float *out_l, float *out_r, uint32_t n) { }
#define OMX_WOBBLE_INSTANCE_LATENCY_FRAMES 0.0f`;
    const keyed = parseFace(face('key'), 'wobble'), odd = parseFace(face('side'), 'wobble');
    expect(keyed.keyed === true && /nor a key first/.test(odd.error ?? ''), `a key first is a keyed face, another block first is refused (${keyed.error ?? keyed.keyed}; ${odd.error ?? 'ACCEPTED'})`);
  }
  {
    // a keyable face: an unkeyed run and a run_keyed beside it, the two key legs first, by name
    const face = (legs) => `static inline int omx_wobble_instance_init(OmxWobbleInstance *s, float sr) { return 1; }
static inline void omx_wobble_instance_resolve(OmxWobbleInstance *s, int bypass, float depth) { }
static inline void omx_wobble_instance_run(OmxWobbleInstance *s, const float *in_l, const float *in_r, float *out_l, float *out_r, uint32_t n) { }
static inline void omx_wobble_instance_run_keyed(OmxWobbleInstance *s, ${legs}, const float *in_l, const float *in_r, float *out_l, float *out_r, uint32_t n) { }
#define OMX_WOBBLE_INSTANCE_LATENCY_FRAMES 0.0f`;
    const keyable = parseFace(face('const float *key_l, const float *key_r'), 'wobble');
    const one = parseFace(face('const float *key_l'), 'wobble'), named = parseFace(face('const float *side_l, const float *side_r'), 'wobble');
    expect(keyable.keyable === true && keyable.keyed === false, `a run_keyed beside an unkeyed run is a keyable face (${keyable.error ?? `keyable ${keyable.keyable}, keyed ${keyable.keyed}`})`);
    expect(/run_keyed takes/.test(one.error ?? '') && /run_keyed takes/.test(named.error ?? ''),
      `a run_keyed of one key leg, or of legs not named key_l and key_r, is refused (${one.error ?? 'ACCEPTED'}; ${named.error ?? 'ACCEPTED'})`);
  }
  {
    // a meter read through the face: its readings are the face's float functions of the instance, a band index or none
    const face = parseFace(`static inline int omx_wobble_instance_init(OmxWobbleInstance *s, float sr) { return 1; }
static inline void omx_wobble_instance_resolve(OmxWobbleInstance *s, int bypass, float depth) { }
static inline void omx_wobble_instance_run(OmxWobbleInstance *s, const float *in_l, const float *in_r, float *out_l, float *out_r, uint32_t n) { }
static inline float omx_wobble_instance_band_gr_db(const OmxWobbleInstance *s, uint32_t k) { return 0.0f; }
static inline float omx_wobble_instance_level_db(const OmxWobbleInstance *s) { return 0.0f; }
#define OMX_WOBBLE_INSTANCE_LATENCY_FRAMES 0.0f`, 'wobble');
    const rows = (meters) => {
      try {
        return meterRows({ stem: 'omx-wobble', kernel: 'wobble', params: [], meters }, face);
      } catch (e) {
        return e.message;
      }
    };
    const ok = rows([{ id: 'gr2', kind: 'gain_reduction', read: 'band_gr_db(1)', readout: 'BAND2_GR_DB' }, { id: 'lvl', kind: 'level', read: 'level_db', readout: 'LEVEL_DB' }]);
    expect(Array.isArray(ok) && ok[0].meterReadCore === 'omx_wobble_instance_band_gr_db(&c->inst, 1u)' && ok[0].meterReadRef === 'omx_wobble_instance_band_gr_db(&inst, 1u)' &&
      ok[1].meterChannels === 1 && ok[1].isRead && !ok[1].isGr && ok[1].meterBypassRef === 'METERS[1].min' && ok[1].meterMin === '(double)OMX_WOBBLE_READOUT_LEVEL_DB_MIN',
      `meters: a band's reading and a level the kernel reads are called on the instance, the level one channel over its readout (${Array.isArray(ok) ? ok.map((r) => r.meterReadCore).join(', ') : ok})`);
    const missing = rows([{ id: 'gr', kind: 'gain_reduction', read: 'gr_db', readout: 'GR_DB' }]);
    const unbanded = rows([{ id: 'gr', kind: 'gain_reduction', read: 'band_gr_db', readout: 'GR_DB' }]);
    const both = rows([{ id: 'gr', kind: 'gain_reduction', read: 'band_gr_db(0)', field: 'st.gr_min_db', readout: 'GR_DB' }]);
    expect(/has no float omx_wobble_instance_gr_db/.test(missing) && /takes band index/.test(unbanded) && /takes no 'field'/.test(both),
      `meters: a reading the face lacks, a band index left out, and a field beside a read are refused (${[missing, unbanded, both].map((x) => (typeof x === 'string' ? x.split(': ').pop() : 'ACCEPTED')).join('; ')})`);
  }

  // ---- 6. the port hints ---------------------------------------------------------------------
  delete process.env.OMX_CONTRACT_DIR; // the real tree reads the installed release
  delete process.env.OMXDSP_INCLUDE;
  {
    // a band's centre at a contract count: the dynamic EQ's bands come up by the EQ rule for DYNEQ_BANDS
    const real = locateContract(ROOT).dir;
    const centre = (k, count) => {
      try {
        return resolveParam(real, 'dyneq', { symbol: `freq${k + 1}`, ref: 'DYNEQ_FREQ_RANGE', defaultBand: { count, index: k, of: 'centre' } }).def;
      } catch (e) {
        return e.message;
      }
    };
    const rule = eqDefaultBands(real, 6).map((b) => b.freqHz);
    const got = [0, 1, 2, 3, 4, 5].map((k) => centre(k, 'DYNEQ_BANDS'));
    expect(JSON.stringify(got) === JSON.stringify(rule) && rule[3] !== rule[0], `defaultBand: the dynamic EQ's six centres are the EQ rule's for DYNEQ_BANDS (${got.join(' ')})`);
    expect(/no count omx-contract declares/.test(String(centre(0, 'DYNEQ_FREQ_RANGE'))), `defaultBand: a count that is no count is refused (${centre(0, 'DYNEQ_FREQ_RANGE')})`);
  }
  // ---- 5b. variants: one declaration, several band counts (tools/variants.mjs) ------------------
  {
    const real = locateContract(ROOT).dir;
    const baseFile = join(ROOT, 'plugins', 'omx-eq', 'omx-eq.decl.json');
    if (real && existsSync(baseFile)) {
      const base = JSON.parse(readFileSync(baseFile, 'utf8'));
      const v16 = base.variants.of.find((v) => v.count === 'eq16');
      const d = expandVariant(real, base, v16);
      const own = base.params.filter((p) => !p.perBand).length, per = base.params.filter((p) => p.perBand).length;
      expect(variantBands(real, base, v16) === 16 && d.params.length === own + 16 * per && d.params.at(-1).symbol === `b16_${base.params.at(-1).symbol}` &&
        d.kernel === 'eq16' && d.face === 'eq' && d.clap.id === 'org.openmixer.eq16' && !/\{bands\}/.test(d.description),
        `a variant expands from its base: ${d.params.length} parameters, the last ${d.params.at(-1).symbol}, files ${d.kernel} over face ${d.face}`);
      const typed = d.params.find((p) => p.symbol === 'b3_type');
      expect(typed?.defaultBand?.strip === 'eq16' && typed.defaultBand.index === 2, `a per-band default takes the variant's strip and the band's index (${JSON.stringify(typed?.defaultBand)})`);
      const loaded = loadDecl(join(ROOT, 'plugins', 'omx-eq16'));
      expect(loaded.params.length === d.params.length && loaded.dir.endsWith('omx-eq16'), 'a variant folder with no declaration loads its base, expanded');
      const sabotaged = { ...base, variants: { ...base.variants, of: [{ stem: 'omx-eq9', count: 'eq9' }] } };
      let threw = '';
      try {
        expandVariant(real, sabotaged, sabotaged.variants.of[0]);
      } catch (e) {
        threw = e.message;
      }
      expect(/has no 'eq9'/.test(threw), `a variant the count sheet does not hold is refused (${threw || 'NOT REFUSED'})`);
      const src = kernelSources('eq', { root: ROOT });
      const draft = src.refusals.length ? { refusals: src.refusals } : draftDeclaration('eq', src, { root: ROOT, recipe });
      expect(!draft.refusals?.length && draft.decl.variants?.count === 'EQ_BAND_COUNTS' && draft.decl.params.some((p) => p.perBand) &&
        reviewMarks(draft.decl).some((m) => m.startsWith('/variants/of/')),
        `the wizard drafts the EQ as one declaration of variants, each variant a REVIEW mark (${draft.refusals?.[0]?.reason ?? reviewMarks(draft.decl ?? {}).filter((m) => m.startsWith('/variants')).length})`);
    } else expect(false, 'variants: the real contract and plugins/omx-eq are there to expand');
  }
  {
    const hints = join(WORK, 'hints');
    copyTree(hints);
    const named = (what, needle) => {
      const e = hintErrors(hints);
      expect(e.some((x) => x.includes(needle)), `port hints: ${what} is named${e.length ? ` (${e[0]})` : ' (NOTHING NAMED)'}`);
    };
    expect(hintErrors(hints).length === 0, 'port hints: the whole tree keeps every pinned hint');
    const eq16 = join(hints, 'plugins/omx-eq16/generated/omx-eq16.lv2/omx-eq16.ttl');
    const ttl16 = readFileSync(eq16, 'utf8');
    writeFileSync(eq16, ttl16.replace('pprops:logarithmic ;', '').replace('units:unit units:db', 'units:unit units:pc'));
    named('a TTL that drops a frequency\'s logarithmic travel', 'omx-eq16: port "hpf_freq" lost pprops:logarithmic');
    named('a TTL that drops a gain\'s unit', 'omx-eq16: port "b1_gain" lost units:unit units:db');
    writeFileSync(eq16, ttl16);
    // omx-eq8 is a variant of plugins/omx-eq (tools/variants.mjs): its parameters are declared there
    const dir8 = join(hints, 'plugins/omx-eq8'), decl8 = join(hints, 'plugins/omx-eq/omx-eq.decl.json'), ttl8 = join(dir8, 'generated/omx-eq8.lv2/omx-eq8.ttl');
    const d8 = readFileSync(decl8, 'utf8');
    const regen = (mutate) => {
      const d = JSON.parse(d8);
      mutate(d.params);
      writeFileSync(decl8, JSON.stringify(d, null, 2));
      writeFileSync(ttl8, emitPluginTtl(loadDecl(dir8)));
    };
    regen((ps) => ps.splice(ps.findIndex((p) => p.symbol === 'type'), 1, { symbol: 'type', name: 'Type', perBand: true, own: 'switch', unit: '', min: 0, max: 5, def: 0, kind: 'integer' }));
    named('a declaration that drops a band type\'s labels', 'omx-eq8: port "b1_type" lost the scale point 0 Bell');
    regen((ps) => (ps.find((p) => p.symbol === 'hpf_freq').scale = 'linear'));
    named('a frequency declared linear', 'omx-eq8: port "hpf_freq" lost pprops:logarithmic');
    regen(() => {});
    expect(hintErrors(hints).length === 0, 'port hints: restored, green again');
    cpSync(join(hints, 'plugins/omx-delay'), join(hints, 'plugins/omx-delay9'), { recursive: true });
    for (const [from, to] of [['omx-delay.decl.json', 'omx-delay9.decl.json'], ['omx-delay.descriptor.json', 'omx-delay9.descriptor.json'], ['generated/omx-delay.lv2', 'generated/omx-delay9.lv2'], ['generated/omx-delay9.lv2/omx-delay.ttl', 'generated/omx-delay9.lv2/omx-delay9.ttl']])
      execFileSync('mv', [join(hints, 'plugins/omx-delay9', from), join(hints, 'plugins/omx-delay9', to)]);
    const d9 = JSON.parse(readFileSync(join(hints, 'plugins/omx-delay9/omx-delay9.decl.json'), 'utf8'));
    d9.stem = 'omx-delay9';
    delete d9.portHints; // the copy carries delay's pin in its declaration
    writeFileSync(join(hints, 'plugins/omx-delay9/omx-delay9.decl.json'), JSON.stringify(d9, null, 2));
    named('a plugin with no pin', 'omx-delay9: no pinned hints');
    rmSync(join(hints, 'plugins/omx-delay9'), { recursive: true });
    // the declaration is the pin: a hint moved there moves the assembled view and is held against the
    // TTL; a hand edit of the view goes stale; a per-plugin pin file beside the declaration is refused
    const fEq = join(hints, 'plugins/omx-eq/omx-eq.decl.json'), pEq = readFileSync(fEq, 'utf8');
    const view = () => readFileSync(join(hints, 'tools/test/port-hints.json'), 'utf8');
    const gen = (...a) => spawnSync('node', ['tools/gen.mjs', ...a], { cwd: hints, encoding: 'utf8' });
    gen();
    const v0 = view();
    const moved = JSON.parse(pEq);
    moved.portHints.gain.unit = 'units:pc';
    writeFileSync(fEq, `${JSON.stringify(moved, null, 2)}\n`);
    expect(gen('--check').status === 1, 'port hints: a hint moved in a declaration leaves the view stale');
    gen();
    expect(view() !== v0 && ['omx-eq8', 'omx-eq16', 'omx-eq32'].every((s) => JSON.parse(view())[s].b1_gain.unit === 'units:pc' && JSON.parse(view())[s][`b${s === 'omx-eq8' ? 8 : 16}_gain`].unit === 'units:pc') && gen('--check').status === 0,
      'port hints: and regenerating moves the view, the per-band pin on every band of every variant');
    named('a pin the TTL does not carry', 'omx-eq16: port "b16_gain" lost units:unit units:pc');
    writeFileSync(fEq, pEq);
    gen();
    expect(view() === v0 && hintErrors(hints).length === 0, 'port hints: the declaration restored, the view and the check whole again');
    writeFileSync(join(hints, 'tools/test/port-hints.json'), v0.replace('units:db', 'units:pc'));
    expect(gen('--check').status === 1, 'port hints: a hand edit of the generated view goes stale');
    gen();
    writeFileSync(join(hints, 'plugins/omx-tremolo/port-hints.json'), '{}\n');
    const stray = gen('--check');
    expect(stray.status === 1 && /plugins\/omx-tremolo\/port-hints\.json/.test(stray.stderr), `port hints: a per-plugin pin file is refused (${stray.stderr.trim().split('\n')[0] || 'NOT REFUSED'})`);
    rmSync(join(hints, 'plugins/omx-tremolo/port-hints.json'));
    expect(view() === v0 && gen('--check').status === 0, 'port hints: restored, the view is whole again');
  }

  // ---- 7. the identity oracle holds a control that re-arms ---------------------------------------
  {
    // the oracle's plan, one column per parameter in declaration order
    const planOf = (dir) => {
      process.env.OMX_CONTRACT_DIR = dir;
      process.env.OMXDSP_INCLUDE = inc; // the stand-in wobble face
      try {
        const oracle = generateInstance(loadDecl(join(tree, P_)))['test/wobble-oracle.c'];
        const found = [...oracle.matchAll(/^ *\{ (\d+), ([-\d.e]+)f, ([012]), \{([^}]*)\}, [01] \},$/gm)];
        const rows = found.map((m) => m[4].split(',').map((x) => x.trim()));
        const timed = found.map((m) => ({ ms: Number(m[2]), signal: Number(m[3]), v: m[4].split(',').map((x) => x.trim()) }));
        return { oracle, timed, cols: rows.length ? rows[0].map((_, i) => rows.map((r) => r[i])) : [] };
      } finally {
        delete process.env.OMX_CONTRACT_DIR; // as arm 6 left them: the real tree reads what is installed
        delete process.env.OMXDSP_INCLUDE;
      }
    };
    const varies = (col) => new Set(col).size > 1;
    const held = join(WORK, 'omx-contract-rearms');
    fakeContract(held, { wobble: WOBBLE }, wobbleControls(true));
    const h = planOf(held);
    expect(h.cols.length === 4, `oracle: the plan has one column per parameter (${h.cols.length})`);
    expect(h.cols[1]?.every((v) => v === '50.0f'), `oracle: depth, which re-arms, is held at its default 50 in every block (${[...new Set(h.cols[1] ?? [])].join(' ')})`);
    expect(varies(h.cols[0] ?? []) && varies(h.cols[2] ?? []) && varies(h.cols[3] ?? []), 'oracle: rateHz, mix and mode still move across their travels');
    expect(/Held at its default in every block[^]*`rearms`\): depth\./.test(h.oracle), "oracle: the comment names the held parameter");
    // the release section: a full-scale burst at the defaults, then a quiet tail of at least the floor
    const rel = h.timed.findIndex((t) => t.signal === 1);
    expect(rel > 0 && h.timed[rel + 1]?.signal === 2 && h.timed[rel + 1].ms >= 100, `oracle: a full-scale burst, then a quiet tail of ${h.timed[rel + 1]?.ms} ms`);
    // the choice sections: mode (2 values) held at each value over its own burst and two tail halves, the rest stepped once
    const sections = h.timed.slice(rel + 2);
    const modes = [...new Set(sections.map((t) => t.v[3]))];
    expect(sections.length === 6 && modes.length === 2 && sections.every((t, k) => t.signal === (k % 3 === 0 ? 1 : 2)),
      `oracle: each value of the mode choice gets a burst and a tail (${sections.length} blocks, values ${modes.join(' ')})`);
    expect(sections[1]?.v[0] !== sections[2]?.v[0] && sections[1]?.v[2] !== sections[2]?.v[2] && sections[1]?.v[1] === sections[2]?.v[1],
      'oracle: under a choice value, every other parameter is stepped once for the second tail, the one that re-arms excepted');
    const moving = join(WORK, 'omx-contract-no-rearms');
    fakeContract(moving, { wobble: WOBBLE }, wobbleControls(false));
    const m = planOf(moving);
    expect(varies(m.cols[1] ?? []) && !/Held at its default/.test(m.oracle), 'sabotage oracle: without rearms, depth moves again and nothing is named held');
    // a plugin's own `fixed`: the mode is no parameter, the binding and the oracle's reference pass its value
    const fixedCore = (fixed) => {
      process.env.OMX_CONTRACT_DIR = moving;
      process.env.OMXDSP_INCLUDE = inc;
      try {
        const d = loadDecl(join(tree, P_));
        const k = d.params.findIndex((p) => p.symbol === 'mode');
        const files = generateInstance({ ...d, params: d.params.filter((_, i) => i !== k), controls: d.controls.filter((_, i) => i !== k), fixed });
        return { core: files['omx_wobble_core.h'], oracle: files['test/wobble-oracle.c'] };
      } catch (e) {
        return { error: e.message };
      } finally {
        delete process.env.OMX_CONTRACT_DIR;
        delete process.env.OMXDSP_INCLUDE;
      }
    };
    const pan = fixedCore({ mode: 'pan' });
    expect(/\/\* mode \*\/ 1\)/.test(pan.core ?? '') && /, 1\);/.test(pan.oracle ?? '') && !/PARAM_MODE/.test(pan.core ?? '') && pan.core?.includes('`mode` is not a parameter of this plugin: it is passed at "pan".'),
      `fixed: a plugin's fixed mode is passed at pan's index to the face and to the reference, and named in the binding's comment (${pan.error ?? 'see the generated core'})`);
    // a monitor: the mode at its default in the moving blocks and the stepped tails, both values in its own section
    const monitored = (monitors) => {
      process.env.OMX_CONTRACT_DIR = moving;
      process.env.OMXDSP_INCLUDE = inc;
      try {
        const oracle = generateInstance({ ...loadDecl(join(tree, P_)), monitors })['test/wobble-oracle.c'];
        return [...oracle.matchAll(/^ *\{ (\d+), ([-\d.e]+)f, ([012]), \{([^}]*)\}, [01] \},$/gm)].map((x) => x[4].split(',').map((y) => y.trim()));
      } catch (e) {
        return e.message;
      } finally {
        delete process.env.OMX_CONTRACT_DIR;
        delete process.env.OMXDSP_INCLUDE;
      }
    };
    const mon = monitored(['mode']);
    const monModes = Array.isArray(mon) ? mon.map((r) => r[3]) : [];
    const free = monitored(undefined);
    expect(monModes.slice(0, 12).every((x) => x === '0.0f') && new Set(monModes).size === 2 && Array.isArray(free) && new Set(free.slice(0, 12).map((r) => r[3])).size === 2,
      `monitors: the mode is held at its default over the moving blocks and still takes both values in its own section; undeclared, it moves there (${monModes.join(' ') || mon})`);
    expect(/monitor 'depth' is a travel/.test(String(monitored(['depth']))), `monitors: a travel named a monitor is refused (${String(monitored(['depth'])).split(': ').pop()})`);
    // engaged: depth held at 80 over the release and choice sections, and a value outside its travel refused
    const engagedRows = (engaged) => {
      process.env.OMX_CONTRACT_DIR = moving;
      process.env.OMXDSP_INCLUDE = inc;
      try {
        const oracle = generateInstance({ ...loadDecl(join(tree, P_)), engaged })['test/wobble-oracle.c'];
        return [...oracle.matchAll(/^ *\{ (\d+), ([-\d.e]+)f, ([012]), \{([^}]*)\}, [01] \},$/gm)].map((x) => x[4].split(',').map((y) => y.trim()));
      } catch (e) {
        return e.message;
      } finally {
        delete process.env.OMX_CONTRACT_DIR;
        delete process.env.OMXDSP_INCLUDE;
      }
    };
    const eng = engagedRows({ depth: 80 });
    expect(Array.isArray(eng) && eng.slice(12).every((r) => r[1] === '80.0f') && new Set(eng.slice(0, 12).map((r) => r[1])).size > 1,
      `engaged: depth is held at 80 over every release and choice row, and still moves in the moving blocks (${Array.isArray(eng) ? [...new Set(eng.slice(12).map((r) => r[1]))].join(' ') : eng})`);
    expect(/engaged 'depth' at 120, outside/.test(String(engagedRows({ depth: 120 }))), `engaged: a value outside the travel is refused (${String(engagedRows({ depth: 120 })).split(': ').pop()})`);
    const stray = fixedCore({ speed: 'default' });
    expect(/fixes 'speed', no control of the wobble kernel/.test(stray.error ?? ''), `fixed: a control the kernel does not have is refused (${stray.error ?? 'ACCEPTED'})`);
    const unlisted = join(WORK, 'omx-contract-unlisted');
    fakeContract(unlisted, { wobble: WOBBLE });
    let why = '';
    try { kernelControls(unlisted, 'wobble'); } catch (e) { why = e.message; }
    expect(why.includes("lists no kernel 'wobble'"), `oracle: a render that does not list the kernel is refused (${why || 'READ ANYWAY'})`);
  }

  // ---- 8. a chain of real faces: eq, gate, comp ------------------------------------------------
  {
    process.env.OMX_CONTRACT_DIR = contract; // the pinned render (plus wobble), for the tree and its make
    delete process.env.OMXDSP_INCLUDE; // the real faces, as the build finds them
    const ct = join(WORK, 'chain');
    copyTree(ct);
    const kernels = ['eq', 'gate', 'comp'];
    const csrc = chainSources(kernels, { root: ct });
    expect(!csrc.refusals.length, `chain: the wizard finds eq, gate and comp and their faces${csrc.refusals.length ? `: ${csrc.refusals.map((r) => r.reason).join('; ')}` : ''}`);
    const noFace = chainSources(['eq', 'balance'], { root: ct });
    expect(noFace.refusals.some((r) => r.field === 'balance: omx-dsp' && /omx_balance_instance\.h/.test(r.reason)), `chain: a kernel with no instance face is refused, naming the omx-dsp work (${noFace.refusals.map((r) => r.field).join(', ')})`);
    if (csrc.refusals.length) return;
    const stem = 'omx-chainproof', CP = `plugins/${stem}`;
    const draft = draftChain(kernels, csrc, { root: ct, recipe, stem });
    expect(draft.decl?.binding === 'chain' && draft.decl.chain.map((el) => `${el.id}:${el.kernel}`).join(' ') === 'eq:eq gate:gate comp:comp',
      `chain: the draft has one element per kernel, in the order given (${draft.decl?.chain.map((el) => el.id).join(' ')})`);
    const cmarks = reviewMarks(draft.decl);
    expect(['/chain/0/on', '/chain/0/bands', '/chain/1/on', '/chain/2/on', '/description', '/console/placement/group'].every((m) => cmarks.includes(m)), `chain: each switch's default, the band count and the console placement are marked REVIEW (${cmarks.join(' ')})`);
    expect(draft.decl?.panel?.sections.map((x) => x.key).join(' ') === 'eq gate comp', `chain: the drafted panel is one section per element (${draft.decl?.panel?.sections.map((x) => x.key).join(' ')})`);
    expect(draft.notes.some((n) => n.startsWith("element 'gate'")), 'chain: the draft notes that the gate\'s face takes a key');
    const settled = structuredClone(draft.decl);
    settled.description = 'A test chain: the console EQ, then its gate, then its compressor, in any order.';
    settled.clap.features[1] = 'mixing';
    settled.lv2.class = 'lv2:MixerPlugin';
    settled.console.placement = { group: 'plugins' };
    const [eq, gate, comp] = settled.chain;
    eq.on = true;
    eq.name = 'EQ';
    eq.bands = 'eq8';
    eq.params = eq.params.filter((p) => !/^eq\d+/.test(p.symbol) || Number(p.symbol.match(/^eq(\d+)/)[1]) <= 8);
    gate.on = false;
    gate.fixed = { keySource: 'self' };
    gate.params = gate.params.filter((p) => p.symbol !== 'gateKeySource');
    comp.on = true;
    // omx-dsp 0.26.0's key, hold, link and duck controls are L3 (#179): the chain holds them at their defaults, as
    // omx-strip's gate and comp do; exposed, each key listen sends the key to the output and the chain has no key
    const hold = (el, controls) => {
      el.held = controls;
      el.params = el.params.filter((p) => !controls.some((c) => p.symbol === `${el.id}${c[0].toUpperCase()}${c.slice(1)}`));
    };
    hold(gate, ['keyListen', 'keyTiltDb']);
    hold(comp, ['keySource', 'keyHpHz', 'keyLpHz', 'keyTiltDb', 'keyListen', 'holdMs', 'detectLink', 'duck', 'rangeDb']);
    // the detector topology's set carries no labels in the contract: the person names them, as omx-comp does
    for (const p of comp.params.filter((x) => x.ref === 'DETECTOR_TOPOLOGY')) p.values = ['Feedforward', 'Feedback'];
    settled.order = 'permutable';
    settled.panel = defaultPanel(settled); // the settled elements' parameters, one section each
    const plan = await planChain(settled, csrc, { root: ct, recipe });
    expect(plan.ok, `chain: the settled chain is accepted${plan.ok ? '' : `: ${plan.refusals.map((r) => `${r.field} ${r.reason}`).join('; ')}`}`);
    if (!plan.ok) return;
    const symbols = plan.resolved.map((p) => p.symbol);
    expect(symbols[0] === 'eqOn' && symbols[symbols.indexOf('gateOn') - 1] === 'eq8BandOn' && symbols[symbols.indexOf('gateOn') + 1] === 'gateThresholdDb' &&
      symbols[symbols.indexOf('compOn') + 1] === 'compThresholdDb' && symbols.at(-1) === 'order' && plan.resolved.at(-1).max === 5,
      `chain: the flattened list is each element's switch then its parameters, then order over 3! orders (${symbols.length} parameters)`);
    const written = writePlugin(plan, { root: ct });
    for (const f of ['Makefile', 'omx_chainproof_core.h', 'omx_chainproof_clap.c', 'omx_chainproof_lv2.c', 'test/chainproof-oracle.c', 'generated/omx_chainproof_params.h', `generated/${stem}.lv2/modgui.ttl`])
      expect(written.includes(`${CP}/${f}`), `chain: the wizard wrote ${CP}/${f}`);
    const header = readFileSync(join(ct, CP, 'generated/omx_chainproof_params.h'), 'utf8');
    expect(header.includes('#define OMX_EQ_INSTANCE_BANDS OMX_EQ_BAND_COUNTS_EQ8_MAX'), 'chain: the eq element\'s band count is the contract\'s, compiled into its face');
    const coreFile = join(ct, CP, 'omx_chainproof_core.h'), core = readFileSync(coreFile, 'utf8');
    expect(/omx_gate_instance_resolve\(&c->gate, bypass \|\| values\[OMX_CHAINPROOF_PARAM_GATE_ON\] < 0\.5f,\s*\/\* key_source \*\/ 0,/.test(core) && core.includes('omx_gate_instance_run(&c->gate, NULL,'),
      'chain: the gate, no key of the chain\'s, runs self-keyed, its key source fixed at self');
    // a shipped plugin carries factory presets (tools/gen-presets.mjs refuses one without, and make test lists and
    // loads them through both faces): one declared as a person declares it, then the preset file, presets.ttl and
    // the bundle's manifest generated
    const presetsJs = join(ct, 'tools/gen-presets.mjs');
    writeFileSync(presetsJs, readFileSync(presetsJs, 'utf8').replace('const DEFINITIONS = {\n',
      `const DEFINITIONS = {\n  'org.openmixer.chainproof': [['gate-first', 'Gate First', { gateOn: 1 }]],\n`));
    const presetGen = spawnSync('node', ['tools/gen-presets.mjs'], { cwd: ct, encoding: 'utf8' });
    const manifestGen = spawnSync('node', ['tools/gen.mjs', CP], { cwd: ct, encoding: 'utf8' });
    expect(presetGen.status === 0 && manifestGen.status === 0 && existsSync(join(ct, 'presets/org.openmixer.chainproof/gate-first.json')) &&
      readFileSync(join(ct, CP, `generated/${stem}.lv2/manifest.ttl`), 'utf8').includes('#preset-gate-first>'),
      `chain: its factory preset declared, the preset file and the bundle's pset:Preset are generated${presetGen.status === 0 && manifestGen.status === 0 ? '' : `: ${`${presetGen.stderr}${manifestGen.stderr}`.trim().split('\n')[0]}`}`);
    const make = (...a) => spawnSync('make', ['-C', join(ct, CP), ...a], { encoding: 'utf8' });
    // make test is the fast set; the oracles, both faces through a host, are make test-heavy's (tools/test/test-split.json)
    const failLines = (r) => `${r.stdout}${r.stderr}`.split('\n').filter((l) => /^FAIL|\bError\b|\berror\b|^presets-host-check/.test(l)).slice(0, 3).join(' | ');
    const green = make('test');
    expect(green.status === 0, `chain: make test is green${green.status === 0 ? '' : `: ${failLines(green)}`}`);
    const heavy = make('test-heavy');
    const faces = ['clap', 'lv2'].filter((f) => !new RegExp(`ORACLE-GREEN ${f}`).test(heavy.stdout));
    expect(heavy.status === 0 && faces.length === 0,
      `chain: make test-heavy is green, both faces the chain of faces bit for bit${heavy.status !== 0 ? `: ${failLines(heavy)}` : faces.length ? `: no ORACLE-GREEN on ${faces.join(', ')}` : ''}`);
    const oracle = () => {
      const b = make('build/omx-chainproof.clap', 'build/chainproof-oracle');
      if (b.status !== 0) return `build failed: ${b.stderr.trim().split('\n').at(-1)}`;
      const r = spawnSync(join(ct, CP, 'build/chainproof-oracle'), ['clap', join(ct, CP, 'build/omx-chainproof.clap')], { encoding: 'utf8' });
      return r.stdout.split('\n').find((l) => /^ORACLE-/.test(l)) ?? `no verdict (${r.status})`;
    };
    writeFileSync(coreFile, core.replace('    { 0, 1, 2 },\n    { 0, 2, 1 },', '    { 0, 2, 1 },\n    { 0, 1, 2 },'));
    const red = oracle();
    expect(/^ORACLE-RED/.test(red), `sabotage chain: the core with two orders swapped makes the oracle red (${red})`);
    writeFileSync(coreFile, core);
    const back = oracle();
    expect(/^ORACLE-GREEN/.test(back), `chain: restored, the oracle is green (${back})`);
    // gen's own refusals
    const refusedBy = (what, mutate, needle) => {
      const d = structuredClone(plan.decl);
      mutate(d);
      writeFileSync(join(ct, CP, `${stem}.decl.json`), `${JSON.stringify(d, null, 2)}\n`);
      const r = spawnSync('node', ['tools/gen.mjs', CP], { cwd: ct, encoding: 'utf8' });
      expect(r.status !== 0 && r.stderr.includes(needle), `chain: gen refuses ${what} (${r.stderr.split('\n').find((l) => l.includes('Error')) ?? r.status})`);
    };
    refusedBy('a key on an element whose face takes none', (d) => {
      d.key = 'comp';
      d.sidechain = { symbol: 'key', name: 'Key' };
    }, "omx_comp_instance_run takes no key");
    refusedBy('a parameter of another kernel', (d) => {
      d.chain[2].params[0].kernel = 'gate';
    }, "names kernel 'gate', not the element's 'comp'");
    writeFileSync(join(ct, CP, `${stem}.decl.json`), `${JSON.stringify(plan.decl, null, 2)}\n`);
    // a chain converted from a release: gen holds it to that release's parameter list
    // gen reads the declaration, then generates (the list a change moves is no longer the committed one)
    const shippedAs = (mutate, ...mode) => {
      const d = structuredClone(plan.decl);
      d.shipped = 'v9.9.9';
      mutate(d);
      writeFileSync(join(ct, CP, `${stem}.decl.json`), `${JSON.stringify(d, null, 2)}\n`);
      return spawnSync('node', ['tools/gen.mjs', ...mode, CP], { cwd: ct, encoding: 'utf8' });
    };
    const untagged = shippedAs(() => {}, '--check');
    expect(untagged.status === 0 && untagged.stderr.includes('v9.9.9 is not readable here'), `chain: with no such tag, gen says the release list is not checked (${untagged.stderr.trim().split('\n')[0]})`);
    git(ct, 'init', '-q');
    git(ct, 'add', '-A');
    git(ct, 'commit', '-q', '-m', 'released');
    git(ct, 'tag', 'v9.9.9');
    expect(shippedAs(() => {}, '--check').status === 0, 'chain: the released list unchanged, gen is green');
    const swap = (d) => {
      const ps = d.chain[2].params;
      [ps[0], ps[1]] = [ps[1], ps[0]];
    };
    const moved = shippedAs(swap);
    expect(moved.status !== 0 && moved.stderr.includes('compThresholdDb -> compRatio') && moved.stderr.includes('compRatio -> compThresholdDb'),
      `sabotage chain: two parameters swapped against the release are refused, each index named (${moved.stderr.split('\n').find((l) => l.includes('->'))?.trim()})`);
    const idxs = [...moved.stderr.matchAll(/^ {2}(\d+ \S+ -> \S+)$/gm)].map((m) => m[1]);
    const named = (d) => {
      swap(d);
      d.shippedDiff = idxs;
    };
    // a parameter that moved is served under another id: the swap is a new parameter contract, refused until a person appends it
    const unlisted = shippedAs(named);
    const next = unlisted.stderr.match(new RegExp(`plugins/${stem}: param-contract fingerprint ([0-9a-f]{64}) is not in tools/param-contract-versions\\.json: append it, the next version \\(2\\)`))?.[1];
    expect(idxs.length === 2 && unlisted.status !== 0 && next !== undefined,
      `sabotage chain: the swap named in shippedDiff, its contract fingerprint not appended, is refused naming version 2 (${unlisted.stderr.split('\n').find((l) => l.includes('param-contract fingerprint'))?.trim().slice(0, 60)})`);
    const versionsFile = join(ct, 'tools', 'param-contract-versions.json'), versionsWas = readFileSync(versionsFile, 'utf8');
    const versions = JSON.parse(versionsWas);
    versions.plugins[stem].push(next);
    writeFileSync(versionsFile, `${JSON.stringify(versions, null, 2)}\n`);
    expect(next !== undefined && shippedAs(named).status === 0, `chain: the same swap named in shippedDiff, its fingerprint appended as version 2, is accepted (${idxs.join('; ')})`);
    writeFileSync(versionsFile, versionsWas);
    writeFileSync(join(ct, CP, `${stem}.decl.json`), `${JSON.stringify(plan.decl, null, 2)}\n`);
    spawnSync('node', ['tools/gen.mjs', CP], { cwd: ct, encoding: 'utf8' }); // the plugin as it was released
    delete process.env.OMX_CONTRACT_DIR;
  }

}

// an arm that stops early has failed already: the verdict and the exit come after main, always
await main();
console.log(fails ? `recipe-selftest: ${fails} check(s) failed` : 'recipe-selftest: every arm red when broken, green when whole');
process.exit(fails ? 1 : 0);
