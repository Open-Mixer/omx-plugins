#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas
/*
 * core-init-answers-check.mjs — a core's init answers 1 only when EVERY kernel instance it holds took the
 * rate. The elements of a chain (omx-strip: trim, two EQs, gate, comp) mostly accept the same rates, so no
 * rate told one element's answer from the others': a dropped element answer (`(void)omx_comp_instance_init`)
 * changes no run. This reads the source instead: in every omx_<kernel>_core_init (each plugins/*\/omx_*_core.h
 * and the templates they are generated from) every call of an `omx_*_instance_init` is returned, or folded
 * into the answer with `ok &= ... != 0`; none is discarded, and a chain's init returns `ok`. Each form is matched
 * as the whole line (an answer or-ed with a constant, or a statement after the fold, is refused). Every line that
 * names `ok` must be one of three held forms: `int ok = 1;`, a fold, or the final `return ok;`; any other line naming
 * it is refused, so other correct forms (a fold wrapped over lines, `ok = ok && ...`, an early return) are refused
 * too and the template is the one way to write a chain init.
 *
 *   node tools/test/core-init-answers-check.mjs [--self-test]
 * --self-test plants a discarded answer in each element of omx-strip's init, a discarded single-kernel answer,
 * a chain init that returns 1, `ok = 1;` before the return and after a fold, `ok++;` and `*(&ok) = 1;` before
 * the return, and a single answer or-ed with a constant: each must go red. `make core-init-answers`, part of `make test`.
 */
import { readFileSync, readdirSync, existsSync } from 'node:fs';
import { join } from 'node:path';
import { ROOT } from '../gen.mjs';

/** Every defect of one core header's (or template's) init function, one line each. */
export function initErrors(src, file) {
  const m = /static inline int omx_[\w{}]+_core_init\([^)]*\)\s*\{([\s\S]*?)\n\}/.exec(src);
  if (!m) return [`${file}: no \`static inline int ..._core_init\` (an init that answers nothing)`];
  const body = m[1];
  const errors = [];
  const calls = [...body.matchAll(/^[^\n]*omx_[\w{}]+_instance_init\([^\n]*$/gm)].map((x) => x[0].trim());
  if (!calls.length) errors.push(`${file}: its init calls no kernel instance init`);
  const chain = calls.length > 1 || /\{\{#elements\}\}/.test(body);
  const init = String.raw`omx_[\w{}]+_instance_init\(&c->[\w{}]+, \((?:float|double|\{\{srType\}\})\)rate\)`;
  const single = new RegExp(`^return ${init};$`);
  const folded = new RegExp(`^(?:\\{\\{#elements\\}\\})?\\s*ok &= ${init} != 0;$`);
  for (const line of calls)
    if (!(chain ? folded.test(line) : single.test(line) || folded.test(line)))
      errors.push(`${file}: an instance init answer is discarded, altered or not folded: ${line}`);
  const lines = body.trim().split('\n').map((l) => l.trim());
  const last = lines[lines.length - 1];
  const finalReturn = /^(?:\{\{\/elements\}\})?\s*return ok;$/;
  lines.forEach((t, i) => {
    if (!/\bok\b/.test(t)) return;
    const held = t === 'int ok = 1;' || folded.test(t) || (i === lines.length - 1 && finalReturn.test(t));
    if (!held) errors.push(`${file}: a line names the answer \`ok\` outside its three held forms: ${t}`);
  });
  if (chain && !finalReturn.test(last)) errors.push(`${file}: a chain's init does not end by returning the folded answer`);
  return errors;
}

/** The files held to it: every shipped plugin's core header and the generating templates. */
function sources(root) {
  const out = [];
  for (const d of readdirSync(join(root, 'plugins'))) {
    const dir = join(root, 'plugins', d);
    if (existsSync(dir) && readdirSync(dir).length)
      for (const f of readdirSync(dir)) if (/^omx_\w+_core\.h$/.test(f)) out.push(join('plugins', d, f));
  }
  for (const t of ['core.h.tmpl', 'chain-core.h.tmpl']) out.push(join('recipes/templates/plugin', t));
  return out;
}

function check(root) {
  const files = sources(root);
  const errors = files.flatMap((f) => initErrors(readFileSync(join(root, f), 'utf8'), f));
  return { files, errors };
}

function selfTest() {
  const strip = readFileSync(join(ROOT, 'plugins/omx-strip/omx_strip_core.h'), 'utf8');
  const arms = [];
  for (const el of ['trim', 'filter', 'gate', 'eq', 'comp'])
    arms.push([`omx-strip: the ${el} element's answer discarded`, strip.replace(new RegExp(`ok &= (omx_\\w+_instance_init\\(&c->${el},[^;]*?\\)) != 0;`), '(void)$1;')]);
  const trim = readFileSync(join(ROOT, 'plugins/omx-trim/omx_trim_core.h'), 'utf8');
  arms.push(['omx-trim: its answer discarded', trim.replace(/return (omx_trim_instance_init\([^;]*\));/, '(void)$1; return 1;')]);
  arms.push(['omx-strip: the chain returns 1', strip.replace('return ok;', 'return 1;')]);
  arms.push(['omx-strip: ok overwritten just before `return ok;`', strip.replace('  return ok;', '  ok = 1;\n  return ok;')]);
  arms.push(['omx-strip: ok overwritten after a fold on the same line', strip.replace(/(ok &= omx_comp_instance_init\([^;]*\) != 0;)/, '$1 ok = 1;')]);
  arms.push(['omx-strip: ok incremented before the return', strip.replace('  return ok;', '  ok++;\n  return ok;')]);
  arms.push(['omx-strip: ok written through a pointer before the return', strip.replace('  return ok;', '  *(&ok) = 1;\n  return ok;')]);
  arms.push(['omx-trim: its answer or-ed with a constant', trim.replace(/return (omx_trim_instance_init\([^;]*\));/, 'return $1 || sizeof(*c);')]);
  let bad = 0;
  if (initErrors(strip, 'omx-strip').length || initErrors(trim, 'omx-trim').length) { console.log('FAIL the unplanted sources are not clean'); bad = 1; }
  for (const [what, src] of arms) {
    const red = src !== strip && src !== trim && initErrors(src, 'planted').length > 0;
    console.log(`${red ? 'ok  ' : 'FAIL'} sabotage goes red: ${what}`);
    if (!red) bad = 1;
  }
  return bad;
}

if (process.argv.includes('--self-test')) {
  const bad = selfTest();
  if (bad) process.exit(1);
}
const { files, errors } = check(ROOT);
for (const e of errors) console.log(`FAIL ${e}`);
if (errors.length) process.exit(1);
console.log(`ok   ${files.length} core inits each return or fold every kernel instance answer`);
