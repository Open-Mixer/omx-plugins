#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * lv2-maintainer-check.mjs — the generated LV2 description gives its doap:maintainer a foaf:homepage.
 * mod-ui dereferences the maintainer's homepage node and segfaults when the bundle never stated one
 * (omx-plugins#244), so every generated TTL must carry it. --self-test strips the triple and must go red.
 *
 * `make lv2-maintainer`, part of `make test`.
 */
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { loadDecl, pluginDirs, ROOT } from '../gen.mjs';
import { isMain } from '../omx-contract.mjs';

const MAINTAINER = /doap:maintainer\s*\[[^\]]*foaf:homepage\s+<[^>\s]+>[^\]]*\]/;

/** Every defect, one line each: `plugins` is `[{ stem, ttl }]`. */
export function maintainerErrors(plugins) {
  return plugins.filter(({ ttl }) => !MAINTAINER.test(ttl)).map(({ stem }) => `${stem}: doap:maintainer has no foaf:homepage`);
}

export function load(root = ROOT) {
  return pluginDirs(root).map((dir) => {
    const d = loadDecl(dir);
    return { stem: d.stem, ttl: readFileSync(join(dir, 'generated', `${d.stem}.lv2`, `${d.stem}.ttl`), 'utf8') };
  });
}

if (isMain(import.meta.url)) {
  const plugins = load();
  const errors = maintainerErrors(plugins);
  for (const e of errors) console.log(`FAIL lv2-maintainer ${e}`);
  let bad = errors.length;
  if (!plugins.length) { console.log('FAIL lv2-maintainer: no plugins found'); bad++; }
  if (process.argv.includes('--self-test')) {
    const red = maintainerErrors(plugins.map((p, i) => (i ? p : { ...p, ttl: p.ttl.replace(/ ; foaf:homepage <[^>]*>/, '') }))).length > 0;
    console.log(`${red ? 'PASS' : 'FAIL'} lv2-maintainer sabotage: a maintainer without foaf:homepage is red`);
    if (!red) bad++;
  }
  console.log(bad ? `lv2-maintainer: ${bad} failure(s)` : `PASS lv2-maintainer: every maintainer states foaf:homepage (${plugins.length} plugins)`);
  process.exit(bad ? 1 : 0);
}
