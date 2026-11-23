#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * lv2-manifest-check.mjs — every generated bundle manifest that ships a MOD GUI points a lilv host at it in the
 * spelling omx-panel's `modgui --check` reads: `rdfs:seeAlso`, then the statement's objects up to its first `.` or `;`,
 * with <modgui.ttl> among them. omx-panel 0.5.0 reads it with /(?:rdfs:seeAlso|rdf-schema#seeAlso>)[^.;]*<modgui\.ttl>/,
 * so an object listed AFTER <plugin.ttl> (whose name holds a dot) is invisible to it and CI went red on every plugin.
 * Which omx-panel is installed must not decide this, so the check carries that reader's pattern. --self-test puts
 * <modgui.ttl> after <stem.ttl> and must go red.
 *
 * `make lv2-manifest`, part of `make test`.
 */
import { existsSync, readFileSync } from 'node:fs';
import { join } from 'node:path';
import { loadDecl, pluginDirs, ROOT } from '../gen.mjs';
import { isMain } from '../omx-contract.mjs';

const SEES_MODGUI = /(?:rdfs:seeAlso|rdf-schema#seeAlso>)[^.;]*<modgui\.ttl>/;

/** Every defect, one line each: `plugins` is `[{ stem, manifest, hasModgui }]`. */
export function manifestErrors(plugins) {
  return plugins.filter((p) => p.hasModgui && !SEES_MODGUI.test(p.manifest)).map((p) => `${p.stem}: manifest.ttl has no rdfs:seeAlso <modgui.ttl> that omx-panel's reader finds`);
}

export function load(root = ROOT) {
  return pluginDirs(root).map((dir) => {
    const d = loadDecl(dir);
    const bundle = join(dir, 'generated', `${d.stem}.lv2`);
    return { stem: d.stem, hasModgui: existsSync(join(bundle, 'modgui.ttl')), manifest: readFileSync(join(bundle, 'manifest.ttl'), 'utf8') };
  });
}

if (isMain(import.meta.url)) {
  const plugins = load();
  const errors = manifestErrors(plugins);
  for (const e of errors) console.log(`FAIL lv2-manifest ${e}`);
  let bad = errors.length;
  if (!plugins.some((p) => p.hasModgui)) { console.log('FAIL lv2-manifest: no bundle ships a modgui.ttl'); bad++; }
  if (process.argv.includes('--self-test')) {
    const swap = (m) => m.replace(/<modgui\.ttl> , (<[^>]+\.ttl>)/, '$1 , <modgui.ttl>');
    const red = manifestErrors(plugins.map((p) => ({ ...p, manifest: swap(p.manifest) }))).length > 0;
    console.log(`${red ? 'PASS' : 'FAIL'} lv2-manifest sabotage: <modgui.ttl> listed after <stem.ttl> is red`);
    if (!red) bad++;
  }
  console.log(bad ? `lv2-manifest: ${bad} failure(s)` : `PASS lv2-manifest: every MOD GUI is found by the manifest's seeAlso (${plugins.filter((p) => p.hasModgui).length} bundles)`);
  process.exit(bad ? 1 : 0);
}
