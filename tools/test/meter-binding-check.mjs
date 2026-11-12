#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
//
// tools/test/meter-binding-check.mjs — every meter a shipped plugin exports (clap.meters/1, its declaration's
// `meters`, tools/gen.mjs metersOf) is drawn: a display of its panel description binds it (a gain-reduction bar, a
// transfer curve's operating point, a meter), or tools/test/meter-binding-unbound.json names it with the reason it is
// not drawn. An entry there for a meter a display binds, or for a meter the plugin does not export, is refused too, so
// the list only ever holds what is true.
//
//   node tools/test/meter-binding-check.mjs              the check over every shipped plugin
//   node tools/test/meter-binding-check.mjs --self-test  a meter neither bound nor listed, a listed meter that is bound,
//                                                        and a listed meter the plugin does not export each refuse;
//                                                        the shipped tree passes
// Exit: 0 pass, 1 refused.
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { loadDecl, metersOf, pluginDirs, ROOT } from '../gen.mjs';
import { panelDescription } from '../descriptor.mjs';

const UNBOUND_FILE = join(ROOT, 'tools', 'test', 'meter-binding-unbound.json');

/** `{ stem: { exported: [symbol], bound: Set<symbol> } }` of every shipped plugin with meters. */
export function meterBindings() {
  const out = {};
  for (const dir of pluginDirs()) {
    const d = loadDecl(dir);
    if (d.role === 'test') continue;
    const exported = metersOf(d).map((m) => m.id);
    if (!exported.length) continue;
    const x = panelDescription(d);
    const bound = new Set();
    for (const v of x.displays ?? []) for (const i of v.meters ?? []) if (i !== null && i !== undefined && x.meters?.[i]) bound.add(x.meters[i].symbol);
    out[d.stem] = { exported, bound };
  }
  return out;
}

/** The problems of `bindings` against `unbound` (`{ stem: { symbol: reason } }`): one line each. */
export function problems(bindings, unbound) {
  const out = [];
  for (const [stem, { exported, bound }] of Object.entries(bindings)) {
    const listed = unbound[stem] ?? {};
    for (const m of exported) {
      if (bound.has(m) && m in listed) out.push(`${stem} ${m}: listed as unbound, but a display binds it`);
      if (!bound.has(m) && !(m in listed)) out.push(`${stem} ${m}: exported, bound by no display and not listed in meter-binding-unbound.json with a reason`);
      if (m in listed && !String(listed[m] ?? '').trim()) out.push(`${stem} ${m}: listed with no reason`);
    }
    for (const m of Object.keys(listed)) if (!exported.includes(m)) out.push(`${stem} ${m}: listed, but the plugin exports no such meter`);
  }
  for (const stem of Object.keys(unbound)) if (stem !== '$comment' && !bindings[stem]) out.push(`${stem}: listed, but no shipped plugin of that name exports meters`);
  return out;
}

function main(argv) {
  const bindings = meterBindings();
  const { $comment, ...unbound } = JSON.parse(readFileSync(UNBOUND_FILE, 'utf8'));
  if (argv.includes('--self-test')) {
    const [stem] = Object.keys(bindings).filter((s) => bindings[s].bound.size);
    const boundOne = [...bindings[stem].bound][0];
    const copy = () => JSON.parse(JSON.stringify(unbound));
    const arms = {
      'a meter neither bound nor listed': () => { const u = copy(); const [s, ms] = Object.entries(u).find(([, v]) => Object.keys(v).length); delete ms[Object.keys(ms)[0]]; u[s] = ms; return u; },
      'a listed meter that a display binds': () => { const u = copy(); u[stem] = { ...(u[stem] ?? {}), [boundOne]: 'stale' }; return u; },
      'a listed meter the plugin does not export': () => { const u = copy(); u[stem] = { ...(u[stem] ?? {}), noSuchMeter: 'gone' }; return u; },
    };
    let bad = problems(bindings, unbound).length ? 1 : 0;
    if (bad) console.error('self-test: the shipped tree does not pass');
    for (const [name, make] of Object.entries(arms)) if (!problems(bindings, make()).length) { console.error(`self-test: ${name} passed`); bad++; }
    console.log(bad ? `meter-binding self-test: ${bad} wrong` : `meter-binding self-test: ${Object.keys(arms).length} arms refuse, the shipped tree passes`);
    return bad ? 1 : 0;
  }
  const p = problems(bindings, unbound);
  for (const l of p) console.error(`meter-binding: ${l}`);
  if (!p.length) {
    const n = Object.values(bindings).reduce((a, b) => a + b.exported.length, 0);
    const listed = Object.values(unbound).reduce((a, b) => a + Object.keys(b).length, 0);
    console.log(`meter-binding: ${n} meters of ${Object.keys(bindings).length} plugins, ${n - listed} bound by a display, ${listed} listed unbound with a reason`);
  }
  return p.length ? 1 : 0;
}

if (process.argv[1] === fileURLToPath(import.meta.url)) process.exit(main(process.argv.slice(2)));
