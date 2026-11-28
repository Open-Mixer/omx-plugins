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
 *   - the top Makefile: the fast checks (the prerequisites of `test-checks`), the $(MAKE) -s steps of `test` and of
 *     `test-heavy`, and the prerequisites of `test-heavy`;
 *   - `make test` runs every part whatever an earlier part answered: one shell command that runs `$(MAKE) -k test-checks`,
 *     then every plugin's test, then each step, each call `|| { rc=1; ... }`, and ends `exit $$rc`. A red fast check
 *     hid every plugin's test until 0.20.0 (omx-strip's remote-controls red, #212, behind role-meanings and debt-base);
 *   - every Makefile target that runs a tools/test/ script is a fast check, a step, or listed under `elsewhere` with
 *     where it runs (strip-pages-follow was written, said "part of make test", and was run by nothing);
 *   - the plugin Makefile template (recipes/templates/plugin/Makefile.tmpl): each `|| fail=1` step of a
 *     plugin's `test` and `test-heavy` recipes is one listed step, in the listed target, and every listed
 *     step is there;
 *   - every plugin Makefile has both targets (the hand-written test plugins too), so the loops reach them;
 *   - .github/workflows/ci.yml runs make test-heavy on a pull request, dispatch or call, in a matrix whose shards are 1/n..n/n.
 * A step dropped from both targets, moved without the list, or a shard left out is red.
 *
 * --self-test: each sabotage arm must be red, the unsabotaged tree green.
 * `make test-split`, part of `make test` (a fast check).
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

/** The targets whose own recipe runs a tools/test/ script. */
function testTargets(makefile) {
  const out = [];
  for (const m of makefile.matchAll(/^([A-Za-z0-9_-]+):(?!=)/gm)) {
    const r = rule(makefile, m[1]);
    if (r && r.recipe.some((l) => l.includes('tools/test/'))) out.push(m[1]);
  }
  return [...new Set(out)];
}

/** `make test` runs every part whatever an earlier one answered: one shell command, `$(MAKE) -k test-checks`, then the
 * plugin loop and the steps, each `|| { rc=1`, ending `exit $$rc`. */
function keepGoingErrors(r) {
  if (!r) return [];
  const errors = [];
  const runs = r.recipe.filter((l) => l.includes('$(MAKE)'));
  if (!runs.some((l) => /\$\(MAKE\) -k test-checks\b/.test(l))) errors.push('Makefile `test`: does not run `$(MAKE) -k test-checks` (a red fast check would stop the others)');
  for (const l of runs) if (!/\$\(MAKE\)[^|]*\|\| \{ rc=1;/.test(l)) errors.push(`Makefile \`test\`: a part stops the rest when it is red: ${l.trim()}`);
  const first = r.recipe.findIndex((l) => l.includes('$(MAKE)'));
  const last = r.recipe.length - 1;
  if (first >= 0 && (r.recipe.slice(first, last).some((l) => !l.endsWith('\\')) || !/exit \$\$rc\s*$/.test(r.recipe[last]))) errors.push('Makefile `test`: its parts are not one shell command ending `exit $$rc` (a separate recipe line stops make at a red)');
  return errors;
}

const sameSet = (a, b) => a.length === b.length && [...a].sort().join('\n') === [...b].sort().join('\n');

/** Every defect, one line each, of the tree given as file texts. */
export function splitErrors(t) {
  const errors = [];
  const list = JSON.parse(t.list);
  // the top Makefile: the fast checks are test-checks' prerequisites, test-heavy's its own
  for (const target of ['test', 'test-heavy']) {
    const r = rule(t.makefile, target);
    if (!r) { errors.push(`Makefile: no \`${target}\` target`); continue; }
    const want = list[target];
    const pre = target === 'test' ? rule(t.makefile, 'test-checks') : r;
    if (!pre) { errors.push('Makefile: no `test-checks` target'); continue; }
    if (!sameSet(pre.prerequisites, want.prerequisites)) {
      const gone = want.prerequisites.filter((p) => !pre.prerequisites.includes(p));
      const extra = pre.prerequisites.filter((p) => !want.prerequisites.includes(p));
      errors.push(`Makefile \`${target === 'test' ? 'test-checks' : target}\`: prerequisites differ from the list (missing: ${gone.join(' ') || '-'}; not listed: ${extra.join(' ') || '-'})`);
    }
    const steps = [...new Set(topSteps(r))];
    const gone = want.steps.filter((s) => !steps.includes(s));
    const extra = steps.filter((s) => !want.steps.includes(s));
    if (gone.length || extra.length) errors.push(`Makefile \`${target}\`: steps differ from the list (missing: ${gone.join(' ') || '-'}; not listed: ${extra.join(' ') || '-'})`);
  }
  errors.push(...keepGoingErrors(rule(t.makefile, 'test')));
  // every target that runs a tools/test/ script is run by one of the sets, or says where it runs
  const listed = new Set([...list.test.prerequisites, ...list.test.steps, ...list['test-heavy'].steps, ...Object.keys(list.elsewhere ?? {})]);
  for (const target of testTargets(t.makefile)) if (!listed.has(target)) errors.push(`Makefile \`${target}\` runs a tools/test/ script and neither make test nor make test-heavy runs it, and it is not under elsewhere`);
  for (const target of Object.keys(list.elsewhere ?? {})) if (!rule(t.makefile, target)) errors.push(`test-split.json elsewhere: \`${target}\` is no Makefile target`);
  // nothing is listed twice
  const all = [...list.test.prerequisites, ...list.test.steps, ...list['test-heavy'].steps, ...Object.keys(list.elsewhere ?? {}),
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
  // CI runs the heavy set (every shard) on a pull request, or on a dispatch or a call when the pull request is checked elsewhere
  if (!/^\s*(pull_request|workflow_dispatch|workflow_call):/m.test(t.ci)) errors.push('ci.yml: runs on none of pull_request, workflow_dispatch, workflow_call');
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
  console.log(`PASS test-split: make test runs ${list.test.prerequisites.length + list.test.steps.length} listed targets and steps, make test-heavy ${list['test-heavy'].steps.length}; a plugin's test ${list.plugin.test.length} steps, its test-heavy ${list.plugin['test-heavy'].length}; CI runs the heavy set on pull requests, dispatch or call`);
}

if (process.argv.includes('--self-test')) {
  const swap = (s, a, b) => { if (!s.includes(a)) throw new Error(`self-test: "${a}" is not in the text it sabotages`); return s.replace(a, b); };
  const arms = {
    'round-trip dropped from make test-heavy': { ...t, makefile: swap(t.makefile, '$(MAKE) -s round-trip;', 'true;') },
    'hints dropped from make test': { ...t, makefile: swap(t.makefile, '\t$(MAKE) -s hints || { rc=1; red="$$red hints"; }; \\\n', '') },
    'a fast prerequisite dropped from make test': { ...t, makefile: swap(t.makefile, ' debt-base test-split\n', ' test-split\n') },
    'the fast checks run without -k': { ...t, makefile: swap(t.makefile, '$(MAKE) -k test-checks', '$(MAKE) test-checks') },
    'the plugin loop stops at the first red plugin': { ...t, makefile: swap(t.makefile, '$(MAKE) -C $$p test || { rc=1; red="$$red $$p"; }', '$(MAKE) -C $$p test || exit 1') },
    'a step on a recipe line of its own': { ...t, makefile: swap(t.makefile, '\t$(MAKE) -s lib-test || { rc=1; red="$$red lib-test"; }; \\\n', '\t:\n\t@$(MAKE) -s lib-test || { rc=1; red="$$red lib-test"; }\n') },
    'a check target run by nothing (strip-pages-follow out of test-checks)': { ...t, makefile: swap(t.makefile, ' strip-pages-follow face-dirty-order', ' face-dirty-order'), list: JSON.stringify({ ...JSON.parse(t.list), test: { ...JSON.parse(t.list).test, prerequisites: JSON.parse(t.list).test.prerequisites.filter((x) => x !== 'strip-pages-follow') } }) },
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
  // the accepted shapes: a ci.yml whose trigger set is workflow_dispatch (or workflow_call) runs the split and is green
  const onBlock = (ci, keys) => ci.replace(/\non:\n[\s\S]*?\n(?=\S)/, `\non:\n${keys.map((k) => `  ${k}:\n`).join('')}\n`);
  for (const [name, ci] of Object.entries({
    'a ci.yml triggered by workflow_dispatch only': onBlock(t.ci, ['workflow_dispatch']),
    'a ci.yml triggered by workflow_call only': onBlock(t.ci, ['workflow_call']),
    'a ci.yml triggered by pull_request': onBlock(t.ci, ['pull_request']),
  })) {
    const errs = splitErrors({ ...t, ci });
    if (errs.length === 0) console.log(`PASS test-split accepted: ${name} is green`);
    else { console.log(`FAIL test-split accepted: ${name} is refused: ${errs.join('; ')}`); failed++; }
  }
  arms['a push-only ci.yml'] = { ...t, ci: onBlock(t.ci, ['push']) };
  arms['a ci.yml with no pull_request, workflow_dispatch or workflow_call trigger'] = { ...t, ci: onBlock(t.ci, ['schedule']) };
  for (const [name, sabotaged] of Object.entries(arms)) {
    let red;
    try { red = splitErrors(sabotaged).length > 0; } catch (e) { red = false; console.log(`FAIL test-split sabotage: ${name}: ${e.message}`); }
    if (red) console.log(`PASS test-split sabotage: ${name} is red`);
    else { console.log(`FAIL test-split sabotage: ${name} stays green`); failed++; }
  }
}
process.exit(failed ? 1 : 0);
