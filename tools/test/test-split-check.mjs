#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * test-split-check.mjs — make test and make test-heavy together run every check, and none falls out of both.
 *
 * make test is the fast set: the checks that read declarations, generated files and TTL, and the in-process
 * probes. The package %check (RPM) and dh_auto_test (DEB) run it. make test-heavy is the set that runs the
 * built plugins through a host: each plugin's kernel-identity oracles (both faces), its MOD GUI test, the
 * external scanners of host-check.sh, and the round trip through plugin-hostd. It runs on every pull request
 * in its own CI jobs, sharded (OMX_HEAVY_SHARD=i/n), in parallel.
 *
 * tools/test/test-split.json lists every step once, under the target that runs it. This holds to that list:
 *   - the top Makefile: the prerequisites and the $(MAKE) -s steps of `test` and of `test-heavy`;
 *   - the plugin Makefile template (recipes/templates/plugin/Makefile.tmpl): each `|| fail=1` step of a
 *     plugin's `test` and `test-heavy` recipes is one listed step, in the listed target, and every listed
 *     step is there;
 *   - every plugin Makefile has both targets (the hand-written test plugins too), so the loops reach them;
 *   - .github/workflows/ci.yml runs make test-heavy on pull requests, in a matrix whose shards are 1/n..n/n.
 * A step dropped from both targets, moved without the list, or a shard left out is red.
 *
 * --self-test: each sabotage arm must be red, the unsabotaged tree green.
 * `make test-split`, part of `make test`.
 */
import { readFileSync, readdirSync, existsSync } from 'node:fs';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = join(fileURLToPath(new URL('.', import.meta.url)), '..', '..');
const read = (p) => readFileSync(join(ROOT, p), 'utf8');

/** A make rule: its prerequisites and recipe lines (tab-led), or null when the target is not defined. */
export function rule(makefile, target) {
  const lines = makefile.split('\n');
  const at = lines.findIndex((l) => l.startsWith(`${target}:`) && !l.startsWith(`${target}::`));
  if (at < 0) return null;
  const prerequisites = lines[at].slice(target.length + 1).trim().split(/\s+/).filter(Boolean);
  const recipe = [];
  // a template line may open with mustache section tags ({{#panel}}, {{/oracles}}) before its tab
  const bare = (l) => l.replace(/^(\{\{[#/^][a-zA-Z]+\}\})+/, '');
  for (let i = at + 1; i < lines.length && bare(lines[i]).startsWith('\t'); i++) recipe.push(bare(lines[i]));
  return { prerequisites, recipe };
}

/** The steps of a top-level rule: `$(MAKE) -s <x>` lines and the per-plugin loop (`$(MAKE) -C $$p <t>`). */
function topSteps(r) {
  const steps = [];
  for (const l of r.recipe) {
    const loop = l.match(/\$\(MAKE\) -C \$\$p ([A-Za-z0-9_-]+)/);
    if (loop) steps.push(`per-plugin ${loop[1]}`);
    for (const m of l.matchAll(/\$\(MAKE\) -s ([A-Za-z0-9_-]+)/g)) steps.push(m[1]);
  }
  return steps;
}

/** The `|| fail=1` steps of a plugin recipe, each named by the one listed marker it carries. */
function pluginSteps(r, markers, where, errors) {
  const text = r.recipe.join('\n').replace(/\{\{[#/][a-zA-Z]+\}\}/g, '');
  const segments = text.split('|| fail=1').slice(0, -1);
  const found = [];
  for (const s of segments) {
    const hits = markers.filter((m) => s.includes(m));
    if (hits.length !== 1) errors.push(`${where}: a step is named by ${hits.length} listed markers (${hits.join(', ') || 'none'}): ${s.trim().split('\n').pop().trim()}`);
    else found.push(hits[0]);
  }
  return found;
}

const sameSet = (a, b) => a.length === b.length && [...a].sort().join('\n') === [...b].sort().join('\n');

/** Every defect, one line each, of the tree given as file texts. */
export function splitErrors(t) {
  const errors = [];
  const list = JSON.parse(t.list);
  // the top Makefile
  for (const target of ['test', 'test-heavy']) {
    const r = rule(t.makefile, target);
    if (!r) { errors.push(`Makefile: no \`${target}\` target`); continue; }
    const want = list[target];
    if (!sameSet(r.prerequisites, want.prerequisites)) {
      const gone = want.prerequisites.filter((p) => !r.prerequisites.includes(p));
      const extra = r.prerequisites.filter((p) => !want.prerequisites.includes(p));
      errors.push(`Makefile \`${target}\`: prerequisites differ from the list (missing: ${gone.join(' ') || '-'}; not listed: ${extra.join(' ') || '-'})`);
    }
    const steps = [...new Set(topSteps(r))];
    const gone = want.steps.filter((s) => !steps.includes(s));
    const extra = steps.filter((s) => !want.steps.includes(s));
    if (gone.length || extra.length) errors.push(`Makefile \`${target}\`: steps differ from the list (missing: ${gone.join(' ') || '-'}; not listed: ${extra.join(' ') || '-'})`);
  }
  // nothing is listed twice
  const all = [...list.test.prerequisites, ...list.test.steps, ...list['test-heavy'].steps,
    ...list.plugin.test, ...list.plugin['test-heavy']];
  for (const s of new Set(all)) if (all.filter((x) => x === s).length > 1 && s !== 'lib') errors.push(`test-split.json: "${s}" is listed twice`);
  // the plugin template
  const markers = [...list.plugin.test, ...list.plugin['test-heavy']];
  for (const target of ['test', 'test-heavy']) {
    const r = rule(t.template, target);
    if (!r) { errors.push(`Makefile.tmpl: no \`${target}\` target`); continue; }
    const found = pluginSteps(r, markers, `Makefile.tmpl \`${target}\``, errors);
    for (const s of found) if (!list.plugin[target].includes(s)) errors.push(`Makefile.tmpl \`${target}\` runs "${s}", which the list puts in the other target`);
    for (const s of list.plugin[target]) if (!found.includes(s)) errors.push(`Makefile.tmpl \`${target}\` does not run "${s}"`);
  }
  // every plugin Makefile reaches both loops
  for (const [name, text] of Object.entries(t.plugins)) {
    for (const target of ['test', 'test-heavy']) if (!rule(text, target)) errors.push(`plugins/${name}/Makefile: no \`${target}\` target`);
  }
  // CI runs the heavy set on every pull request, every shard
  if (!/^\s*pull_request:/m.test(t.ci)) errors.push('ci.yml: does not run on pull requests');
  const job = t.ci.match(/\n {2}test-heavy:\n([\s\S]*?)(?=\n {2}[a-z][a-z0-9-]*:\n|$)/);
  if (!job) errors.push('ci.yml: no `test-heavy` job');
  else {
    if (!/run: [^\n]*make test-heavy|\n\s+make test-heavy/.test(job[1])) errors.push('ci.yml `test-heavy`: no step runs make test-heavy');
    const shards = [...job[1].matchAll(/^\s+- (\d+)\/(\d+)\s*$/gm)].map((m) => [Number(m[1]), Number(m[2])]);
    const n = shards[0]?.[1];
    if (!n) errors.push('ci.yml `test-heavy`: no shard matrix (- i/n)');
    else {
      const got = shards.filter(([, m]) => m === n).map(([i]) => i).sort((a, b) => a - b).join(',');
      const want = Array.from({ length: n }, (_, i) => i + 1).join(',');
      if (got !== want || shards.some(([, m]) => m !== n)) errors.push(`ci.yml \`test-heavy\`: shards ${shards.map((s) => s.join('/')).join(' ')} are not 1/${n}..${n}/${n}`);
    }
    if (!/OMX_HEAVY_SHARD:\s*\$\{\{\s*matrix\.shard\s*\}\}/.test(job[1])) errors.push('ci.yml `test-heavy`: OMX_HEAVY_SHARD is not the matrix shard');
  }
  return errors;
}

export function tree() {
  const plugins = {};
  for (const d of readdirSync(join(ROOT, 'plugins'))) if (existsSync(join(ROOT, 'plugins', d, 'Makefile'))) plugins[d] = read(`plugins/${d}/Makefile`);
  return {
    list: read('tools/test/test-split.json'),
    makefile: read('Makefile'),
    template: read('recipes/templates/plugin/Makefile.tmpl'),
    ci: read('.github/workflows/ci.yml'),
    plugins,
  };
}

function report(errors) {
  for (const e of errors) console.log(`FAIL test-split ${e}`);
  return errors.length;
}

const t = tree();
const errors = splitErrors(t);
let failed = report(errors);
if (!failed) {
  const list = JSON.parse(t.list);
  console.log(`PASS test-split: make test runs ${list.test.prerequisites.length + list.test.steps.length} listed targets and steps, make test-heavy ${list['test-heavy'].steps.length}; a plugin's test ${list.plugin.test.length} steps, its test-heavy ${list.plugin['test-heavy'].length}; CI runs the heavy set on pull requests`);
}

if (process.argv.includes('--self-test')) {
  const swap = (s, a, b) => { if (!s.includes(a)) throw new Error(`self-test: "${a}" is not in the text it sabotages`); return s.replace(a, b); };
  const arms = {
    'round-trip dropped from make test-heavy': { ...t, makefile: swap(t.makefile, '$(MAKE) -s round-trip;', 'true;') },
    'hints dropped from make test': { ...t, makefile: swap(t.makefile, '\t@$(MAKE) -s hints\n', '') },
    'a fast prerequisite dropped from make test': { ...t, makefile: swap(t.makefile, ' face-dirty-order test-split\n', ' test-split\n') },
    'host-check.sh dropped from a plugin\'s test-heavy': { ...t, template: t.template.split('\n').filter((l) => !l.includes('host-check.sh')).join('\n') },
    'a plugin Makefile with no test-heavy': { ...t, plugins: { ...t.plugins, 'omx-x': 'all:\n\ttrue\ntest: all\n\ttrue\n' } },
    'a shard left out of the CI matrix': { ...t, ci: swap(t.ci, '          - 4/4\n', '') },
    'the CI test-heavy job gone': { ...t, ci: t.ci.replace(/\n {2}test-heavy:\n/, '\n  test-heavy-gone:\n') },
  };
  // the oracle arm: put the first oracle line of test-heavy into test's recipe instead (a step in the wrong target)
  {
    const lines = t.template.split('\n');
    const oracle = lines.findIndex((l) => l.includes('{{kernel}}-oracle clap'));
    const testAt = lines.findIndex((l) => l.startsWith('test:'));
    if (oracle < 0 || testAt < 0) throw new Error('self-test: the template has no kernel oracle line or no test target');
    const moved = [...lines];
    const [line] = moved.splice(oracle, 1);
    moved.splice(testAt + 2, 0, line);
    arms['the kernel oracle moved back into a plugin\'s test'] = { ...t, template: moved.join('\n') };
  }
  for (const [name, sabotaged] of Object.entries(arms)) {
    let red;
    try { red = splitErrors(sabotaged).length > 0; } catch (e) { red = false; console.log(`FAIL test-split sabotage: ${name}: ${e.message}`); }
    if (red) console.log(`PASS test-split sabotage: ${name} is red`);
    else { console.log(`FAIL test-split sabotage: ${name} stays green`); failed++; }
  }
}
process.exit(failed ? 1 : 0);
