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
 * as the whole line. EVERY line of the init body is held to the template forms, in order: `memset(c, 0, sizeof *c);`,
 * `c->rate = rate;`, then either `return <instance init>;` or `int ok = 1;`, one fold per element, `return ok;`
 * (blank lines aside). Any other line is refused: control flow (if, goto, an early return), a block, a
 * preprocessor line, a comment, a second `ok`. Other correct forms are refused too: the templates are the one way to
 * write an init. In a shipped header the answers must be exactly the core's kernel instances, in struct order (a
 * deleted fold is refused).
 *
 *   node tools/test/core-init-answers-check.mjs [--self-test]
 * --self-test plants a discarded answer in each element of omx-strip's init, a discarded single-kernel answer,
 * a chain init that returns 1, `ok = 1;` before the return and after a fold, `ok++;` and `*(&ok) = 1;` before
 * the return, a single answer or-ed with a constant, an early `if (rate) return 1;` (chain and single), a goto
 * past the folds, a fold inside `if (0)`, `#if 0` or a block comment, an inner block re-declaring ok, and a deleted
 * fold: each must go red. `make core-init-answers`, part of `make test`.
 */
import { readFileSync, readdirSync, existsSync } from 'node:fs';
import { join } from 'node:path';
import { ROOT } from '../gen.mjs';

/** Every defect of one core header's (or template's) init function, one line each. */
export function initErrors(src, file) {
  const m = /static inline int omx_[\w{}]+_core_init\([^)]*\)\s*\{([\s\S]*?)\n\}/.exec(src);
  if (!m) return [`${file}: no \`static inline int ..._core_init\` (an init that answers nothing)`];
  const errors = [];
  const init = String.raw`omx_[\w{}]+_instance_init\(&c->([\w{}]+), \((?:float|double|\{\{srType\}\})\)rate\)`;
  const single = new RegExp(`^return ${init};$`);
  const folded = new RegExp(`^(?:\\{\\{#elements\\}\\})?\\s*ok &= ${init} != 0;$`);
  const finalReturn = /^(?:\{\{\/elements\}\})?\s*return ok;$/;
  const lines = m[1].split('\n').map((l) => l.trim()).filter((l) => l !== '');
  const head = ['memset(c, 0, sizeof *c);', 'c->rate = rate;'];
  const bad = (t) => errors.push(`${file}: a line outside the template forms: ${t}`);
  head.forEach((h, i) => { if (lines[i] !== h) bad(lines[i] ?? '(missing)'); });
  const rest = lines.slice(head.length);
  let answered = [];
  if (rest.length === 1 && single.test(rest[0])) answered = [single.exec(rest[0])[1]];
  else if (rest[0] === 'int ok = 1;' && rest.length >= 3 && finalReturn.test(rest[rest.length - 1])) {
    for (const t of rest.slice(1, -1)) {
      const f = folded.exec(t);
      if (f) answered.push(f[1]);
      else bad(t);
    }
  } else rest.forEach((t) => { if (!single.test(t) && !folded.test(t) && !finalReturn.test(t) && t !== 'int ok = 1;') bad(t); });
  if (!answered.length) errors.push(`${file}: its init is not \`return <instance init>;\` nor \`int ok = 1;\`, folds, \`return ok;\``);
  // A shipped header (not a template): its answers are exactly the core's kernel instances, in order.
  const struct = /typedef struct \{([^}]*)\} Omx\w+Core;/.exec(src);
  if (struct && !/\{\{/.test(m[1])) {
    const members = [...struct[1].matchAll(/^\s*Omx\w+Instance (\w+);/gm)].map((x) => x[1]);
    if (members.join(',') !== answered.join(','))
      errors.push(`${file}: the answers (${answered.join(', ') || 'none'}) are not the core's kernel instances (${members.join(', ')})`);
  }
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
  arms.push(['omx-strip: an early `if (rate) return 1;` before the folds', strip.replace('  int ok = 1;', '  if (rate) return 1;\n  int ok = 1;')]);
  arms.push(['omx-trim: an early `if (rate) return 1;` before its answer', trim.replace('  c->rate = rate;', '  c->rate = rate;\n  if (rate) return 1;')]);
  arms.push(['omx-strip: a goto past the folds', strip.replace('  int ok = 1;', '  int ok = 1;\n  goto done;').replace('  return ok;', 'done:\n  return ok;')]);
  arms.push(['omx-strip: the comp fold inside `if (0)`', strip.replace(/(  ok &= omx_comp_instance_init\([^;]*\) != 0;)/, '  if (0)\n$1')]);
  arms.push(['omx-strip: the comp fold inside `#if 0`', strip.replace(/(  ok &= omx_comp_instance_init\([^;]*\) != 0;)/, '#if 0\n$1\n#endif')]);
  arms.push(['omx-strip: the comp fold inside a block comment', strip.replace(/(  ok &= omx_comp_instance_init\([^;]*\) != 0;)/, '/*\n$1\n*/')]);
  arms.push(['omx-strip: an inner block re-declaring ok', strip.replace(/(  ok &= omx_comp_instance_init\([^;]*\) != 0;)/, '  {\n  int ok = 1;\n$1\n  }')]);
  arms.push(['omx-strip: the comp fold deleted', strip.replace(/  ok &= omx_comp_instance_init\([^;]*\) != 0;\n/, '')]);
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
