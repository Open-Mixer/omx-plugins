#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * graph-enum.mjs — the graph kinds a panel section may draw (schema/plugin.decl.schema.json $defs.graph) are omx-dsp's
 * CURVE_KINDS (omx-dsp data/primitives/curve.json, sorting row V26: one holder), all of them in its order, rendered here
 * from the render the build reads, where omx-dsp's package owns the item. Nothing is typed in the schema by hand.
 *
 *   node tools/graph-enum.mjs           write the schema's enum
 *   node tools/graph-enum.mjs --check   exit 1 when the schema's enum is not CURVE_KINDS rendered (STALE)
 */
import { readFileSync, writeFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { isMain, items, locateContract, OWNERS, ownersOf } from './omx-contract.mjs';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..');
export const SCHEMA = join(ROOT, 'schema', 'plugin.decl.schema.json');
export const KINDS_ITEM = 'CURVE_KINDS';
export const KINDS_OWNER = 'omx-dsp';

/** omx-dsp's curve kinds, its words in order, from the render at `dir`; refused when another package holds the item. */
export function curveKinds(dir) {
  const item = items(dir)[KINDS_ITEM];
  if (item?.kind !== 'words' || typeof item.value !== 'object') throw new Error(`${dir}: holds no ${KINDS_ITEM} words (omx-dsp-contract >= 0.34.0 declares them)`);
  const owners = ownersOf(dir);
  if (!owners.size) throw new Error(`${dir}: the render read carries no ${OWNERS}, so the owner of its ${KINDS_ITEM} is unknown (the kinds are ${KINDS_OWNER}'s)`);
  const owner = owners.get(item.rel)?.package;
  if (owner !== KINDS_OWNER) throw new Error(`${dir}: its ${KINDS_ITEM} is ${owner ?? 'no package'}'s (${item.rel}); the kinds are ${KINDS_OWNER}'s, this tree needs omx-dsp-contract >= 0.34.0`);
  return Object.values(item.value).map((w) => w.word);
}

const GRAPH = /("graph": \{\n(\s*)"enum": \[)[^\]]*(\])/;
/** The schema text with its $defs.graph enum set to `kinds`, in the file's own layout. */
export function rendered(text, kinds) {
  const m = GRAPH.exec(text);
  if (!m) throw new Error(`${SCHEMA}: no "graph": { "enum": [...] } definition`);
  const indent = `${m[2]}  `;
  return text.replace(GRAPH, `$1\n${kinds.map((k) => `${indent}${JSON.stringify(k)}`).join(',\n')}\n${m[2]}$3`);
}

if (isMain(import.meta.url)) {
  const where = locateContract(ROOT);
  if (!where.dir) { console.error(`graph-enum: ${where.why}`); process.exit(1); }
  let kinds;
  try { kinds = curveKinds(where.dir); } catch (e) { console.error(`graph-enum: ${e.message}`); process.exit(1); }
  const text = readFileSync(SCHEMA, 'utf8');
  const want = rendered(text, kinds);
  if (process.argv.includes('--check')) {
    if (want !== text) { console.error(`graph-enum: STALE schema/plugin.decl.schema.json $defs.graph: it is not ${KINDS_OWNER}'s ${KINDS_ITEM} (${kinds.join(', ')}); run node tools/graph-enum.mjs`); process.exit(1); }
    console.log(`graph-enum: $defs.graph is ${KINDS_OWNER}'s ${KINDS_ITEM}, ${kinds.length} kinds`);
  } else {
    writeFileSync(SCHEMA, want);
    console.log(`graph-enum: wrote ${kinds.length} kinds of ${KINDS_OWNER}'s ${KINDS_ITEM} into $defs.graph`);
  }
}
