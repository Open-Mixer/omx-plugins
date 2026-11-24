#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * lv2-deprecated-check.mjs — no generated TTL states a term LV2 has deprecated. lv2:reportsLatency is one: the
 * port's lv2:designation lv2:latency says it, so tools/gen.mjs does not write the property (and the declarations'
 * portHints do not pin it). --self-test plants the term in a TTL and must go red.
 *
 * `make lv2-deprecated`, part of `make test`.
 */
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { loadDecl, pluginDirs, ROOT } from '../gen.mjs';
import { isMain } from '../omx-contract.mjs';

const DEPRECATED = ['lv2:reportsLatency'];

/** Every defect, one line each: `plugins` is `[{ stem, ttl }]`. */
export function deprecatedErrors(plugins) {
  return plugins.flatMap((p) => DEPRECATED.filter((t) => p.ttl.includes(t)).map((t) => `${p.stem}: the generated TTL states ${t}, which LV2 deprecates`));
}

export function load(root = ROOT) {
  return pluginDirs(root).map((dir) => {
    const d = loadDecl(dir);
    return { stem: d.stem, ttl: readFileSync(join(dir, 'generated', `${d.stem}.lv2`, `${d.stem}.ttl`), 'utf8') };
  });
}

if (isMain(import.meta.url)) {
  const plugins = load();
  const errors = deprecatedErrors(plugins);
  for (const e of errors) console.log(`FAIL lv2-deprecated ${e}`);
  let bad = errors.length;
  if (process.argv.includes('--self-test')) {
    const red = deprecatedErrors([{ stem: 'x', ttl: 'lv2:portProperty lv2:reportsLatency , lv2:integer ;' }]).length === 1;
    console.log(`${red ? 'PASS' : 'FAIL'} lv2-deprecated sabotage: a planted lv2:reportsLatency is red`);
    if (!red) bad++;
  }
  console.log(bad ? `lv2-deprecated: ${bad} failure(s)` : `PASS lv2-deprecated: no deprecated term in ${plugins.length} generated TTLs`);
  process.exit(bad ? 1 : 0);
}
