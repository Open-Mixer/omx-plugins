#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * panel-words-check.mjs — the enumerations of include/omx_panel_description.h are the panel vocabulary's, spelled
 * once elsewhere: the families, the widgets, the display kinds, the display sources and the placements are
 * omx-contract's PANEL_FAMILIES, PANEL_WIDGETS, PANEL_DISPLAY_KINDS, PANEL_DISPLAY_SOURCES and PANEL_PLACEMENTS
 * (data/panel.json), whose X-macros in <omxcontract/omx_vocabulary.h> the header builds those five enums from
 * (omx-clap-core#104); omx-panel's lists (tools/panel-description.mjs, which tools/gen.mjs writes the rows with) are
 * held to the contract's. The table codes are omx-clap-core's alone: the header spells them as C, held to the X-list
 * <omx-clap-host/omx_clap_ext.h> carries for the host, code and enumerator. Each enumerator is the word in upper case
 * after the enum's prefix, its code the word's position, and the count the list's: the header is held to them here,
 * so a word added, moved or renamed anywhere is red until they agree.
 *
 * An enum the contract owns is built from its X-macro, never spelled by hand: a hand enum is red even when it agrees
 * today. It is held to the contract only, not to omx-clap-core's list: both are the contract's by construction, each
 * held to it in its own repository, so neither release waits on the other's. `make panel-words`, part of
 * `make test`.
 *
 *   node tools/test/panel-words-check.mjs               the check
 *   node tools/test/panel-words-check.mjs --self-test   the check, then sabotaged copies, each red: a hand family enum
 *                                                       (the contract's list, and one family short), a hand
 *                                                       placement enum, a contract with a family moved and with a
 *                                                       placement renamed, and the owner's tables with a code moved
 *
 * OMX_PANEL_WORDS_CONTRACT_INCLUDE names a directory holding omxcontract/omx_vocabulary.h ahead of pkg-config's.
 */
import { execFileSync } from 'node:child_process';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { items, locateContract } from '../omx-contract.mjs';
import { DISPLAY_KINDS, DISPLAY_SOURCES, PLACEMENTS } from '../omx-panel.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const HEADER = 'include/omx_panel_description.h';
const OWNER = 'omx-clap-host/omx_clap_ext.h';
const CONTRACT_HEADER = 'omxcontract/omx_vocabulary.h';

/** The header's enums held: the C enum, its prefix, the owner's X-list, and the contract's X-macro it is built from. */
const ENUMS = {
  family: { c: 'omx_clap_panel_family', prefix: 'OMX_CLAP_FAMILY_', owner: 'OMX_CLAP_PANEL_FAMILIES', contract: 'OMX_PANEL_FAMILIES' },
  widget: { c: 'omx_clap_panel_widget', prefix: 'OMX_CLAP_WIDGET_', owner: 'OMX_CLAP_PANEL_WIDGETS', contract: 'OMX_PANEL_WIDGETS' },
  display_kind: { c: 'omx_clap_panel_display_kind', prefix: 'OMX_CLAP_DISPLAY_', owner: 'OMX_CLAP_PANEL_DISPLAY_KINDS', contract: 'OMX_PANEL_DISPLAY_KINDS' },
  display_source: { c: 'omx_clap_panel_display_source', prefix: 'OMX_CLAP_DISPLAY_FROM_', owner: 'OMX_CLAP_PANEL_DISPLAY_SOURCES', contract: 'OMX_PANEL_DISPLAY_SOURCES' },
  placement: { c: 'omx_clap_panel_placement', prefix: 'OMX_CLAP_PLACEMENT_', owner: 'OMX_CLAP_PANEL_PLACEMENTS', contract: 'OMX_PANEL_PLACEMENTS' },
  table: { c: 'omx_clap_panel_table', prefix: 'OMX_CLAP_PANEL_', owner: 'OMX_CLAP_PANEL_TABLES' },
};

const ROW = /X\((\d+)[uU]?,\s*"([^"]*)",\s*(\w+)\)/g;

/** Every `#define <NAME>(X)` list of a header: `{ NAME: [[code, word, enumerator], ...] }`. */
export function xListsOf(text) {
  const out = {};
  for (const m of text.matchAll(/^#define (\w+)\(X\)((?:[^\n]*\\\n)*[^\n]*)/gm)) {
    out[m[1]] = [...m[2].matchAll(ROW)].map((x) => [Number(x[1]), x[2], x[3]]);
  }
  return out;
}

/** The header's enums: `{ family: [[enumerator, code], ...], ... }`, each in source order, an enum built from a
 * contract X-macro read through `contract` (xListsOf of the contract's header); and `derived`, the enums so built. */
export function enumsOf(text, contract = {}) {
  const out = { derived: {} };
  for (const [key, { c, prefix }] of Object.entries(ENUMS)) {
    const block = text.match(new RegExp(`enum ${c} \\{([^}]*)\\}`))?.[1];
    const via = block?.match(/^\s*(OMX_PANEL_\w+)\(\w+\)\s*$/)?.[1];
    if (via) {
      out.derived[key] = via;
      out[key] = contract[via]?.map(([code, , id]) => [id, code]);
      continue;
    }
    out[key] = block ? [...block.matchAll(/^\s*(\w+)\s*=\s*(\d+)\s*,/gm)].map((m) => [m[1].startsWith(prefix) ? m[1].slice(prefix.length) : m[1], Number(m[2])]) : undefined;
  }
  return out;
}

/** The owner's X-lists: `{ table: [[code, word, enumerator], ...] }`, of the lists not the contract's. */
export function ownerListsOf(text) {
  const out = {};
  const lists = xListsOf(text);
  for (const [key, { owner, contract }] of Object.entries(ENUMS)) if (!contract) out[key] = lists[owner];
  return out;
}

const listed = (rows) => rows.map(([n, v]) => `${n}=${v}`).join(', ');

/** The refusals: each enum of `enums` (enumsOf) against the words `declared` (`{ family: [...], ... }`, a list per
 * enum that has one) and the owner's lists (ownerListsOf). */
export function drift(enums, declared, owner) {
  const errors = [];
  for (const [key, { c, contract }] of Object.entries(ENUMS)) {
    const got = enums[key];
    if (contract && !enums.derived?.[key]) errors.push(`${HEADER}: enum ${c} is spelled by hand; build it from omx-contract's ${contract}(X) (${CONTRACT_HEADER})`);
    if (!got) {
      errors.push(enums.derived?.[key] ? `${CONTRACT_HEADER}: no ${enums.derived[key]}(X) list (omx-contract >= 3.19.0)` : `${HEADER}: no enum ${c}`);
      continue;
    }
    const words = declared[key];
    if (words) {
      const want = words.map((w, i) => [w.toUpperCase(), i]);
      if (JSON.stringify(got) !== JSON.stringify(want)) errors.push(`${HEADER}: enum ${c} is ${listed(got)}; the declaration says ${listed(want)}`);
    }
    // a contract's list is held to the contract alone: omx-clap-core's is too, in its own repository
    if (contract) continue;
    const x = owner[key];
    if (!x) {
      errors.push(`${OWNER}: no ${ENUMS[key].owner}(X) list`);
      continue;
    }
    const theirs = x.map(([code, , enumerator]) => [enumerator, code]);
    if (JSON.stringify(got) !== JSON.stringify(theirs)) errors.push(`${HEADER}: enum ${c} is ${listed(got)}; ${OWNER} says ${listed(theirs)}`);
    const ownerWords = x.map(([, w]) => w);
    if (words && JSON.stringify(ownerWords) !== JSON.stringify(words)) errors.push(`${OWNER}: ${ENUMS[key].owner} words ${ownerWords.join(', ')}; the declaration says ${words.join(', ')}`);
  }
  return errors;
}

/** The words of an omx-contract sheet of kind `words`, in its order, or undefined when the contract holds none. */
function sheetWords(dir, name) {
  const item = items(dir)[name];
  if (item?.kind !== 'words' || typeof item.value !== 'object') return undefined;
  return Object.values(item.value).map((w) => w.word);
}

/** A header where pkg-config says `pkg`'s headers are, else the system's; `first` is looked in before them. */
function headerOf(pkg, rel, first) {
  let dirs = [];
  try {
    dirs = execFileSync(process.env.PKG_CONFIG || 'pkg-config', ['--cflags-only-I', pkg], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'] }).split(/\s+/).filter((f) => f.startsWith('-I')).map((f) => f.slice(2));
  } catch { /* not installed through pkg-config: the system include directory below */ }
  return [...(first ? [first] : []), ...dirs, '/usr/include', '/usr/local/include'].map((d) => join(d, rel)).find((f) => existsSync(f));
}

const contractDir = locateContract(root);
if (!contractDir.dir) {
  console.error(`panel-words: ${contractDir.why}`);
  process.exit(1);
}
const declared = {
  family: sheetWords(contractDir.dir, 'PANEL_FAMILIES'),
  widget: sheetWords(contractDir.dir, 'PANEL_WIDGETS'),
  display_kind: sheetWords(contractDir.dir, 'PANEL_DISPLAY_KINDS'),
  display_source: sheetWords(contractDir.dir, 'PANEL_DISPLAY_SOURCES'),
  placement: sheetWords(contractDir.dir, 'PANEL_PLACEMENTS'),
};
const NAMES = [['family', 'PANEL_FAMILIES'], ['widget', 'PANEL_WIDGETS'], ['display_kind', 'PANEL_DISPLAY_KINDS'], ['display_source', 'PANEL_DISPLAY_SOURCES'], ['placement', 'PANEL_PLACEMENTS']];
for (const [key, name] of NAMES) {
  if (!declared[key]) {
    console.error(`panel-words: ${contractDir.dir}: the resolved contract holds no ${name} words (omx-contract >= 3.19.0)`);
    process.exit(1);
  }
}
for (const [panel, key, name] of [[DISPLAY_KINDS, 'display_kind', 'DISPLAY_KINDS'], [DISPLAY_SOURCES, 'display_source', 'DISPLAY_SOURCES'], [PLACEMENTS, 'placement', 'PLACEMENTS']]) {
  if (JSON.stringify(panel) !== JSON.stringify(declared[key])) {
    console.error(`panel-words: omx-panel's ${name} ${panel.join(', ')}; omx-contract's PANEL_${name} ${declared[key].join(', ')}`);
    process.exit(1);
  }
}
const contractFile = headerOf('omx-contract', CONTRACT_HEADER, process.env.OMX_PANEL_WORDS_CONTRACT_INCLUDE);
if (!contractFile) {
  console.error(`panel-words: ${CONTRACT_HEADER} not found: install omx-contract-devel (BUILDING.md)`);
  process.exit(1);
}
const contract = xListsOf(readFileSync(contractFile, 'utf8'));
const headerText = readFileSync(join(root, HEADER), 'utf8');
const ownerFile = headerOf('omx-clap-core', OWNER);
if (!ownerFile) {
  console.error(`panel-words: ${OWNER} not found: install omx-clap-core-devel (BUILDING.md)`);
  process.exit(1);
}
const ownerText = readFileSync(ownerFile, 'utf8');
const errors = drift(enumsOf(headerText, contract), declared, ownerListsOf(ownerText));
if (errors.length) {
  for (const e of errors) console.error(`panel-words: ${e}`);
  process.exit(1);
}
console.log(`panel-words: ${Object.keys(ENUMS).length} enums of ${HEADER} are the declared words (${Object.values(declared).flat().length}): the contract's from ${CONTRACT_HEADER}, the tables ${OWNER}'s list`);

if (process.argv.includes('--self-test')) {
  const families = contract.OMX_PANEL_FAMILIES;
  const handEnum = (c, prefix, rows) => `enum ${c} {\n${rows.map(([code, , id]) => `  ${prefix}${id} = ${code},\n`).join('')}}`;
  const enumOf = (c) => new RegExp(`enum ${c} \\{[^}]*\\}`);
  const moved = families.map(([code, w, id]) => [id === 'GATE' ? 2 : code, w, id]);
  const renamed = contract.OMX_PANEL_PLACEMENTS.map(([code, w, id]) => [code, w, id === 'BUS' ? 'BUSS' : id]);
  const arms = {
    'family-hand-copy': [headerText.replace(enumOf('omx_clap_panel_family'), handEnum('omx_clap_panel_family', 'OMX_CLAP_FAMILY_', families)), contract, ownerText],
    'family-hand-copy-short': [headerText.replace(enumOf('omx_clap_panel_family'), handEnum('omx_clap_panel_family', 'OMX_CLAP_FAMILY_', families.slice(0, -1))), contract, ownerText],
    'placement-hand-copy': [headerText.replace(enumOf('omx_clap_panel_placement'), handEnum('omx_clap_panel_placement', 'OMX_CLAP_PLACEMENT_', contract.OMX_PANEL_PLACEMENTS)), contract, ownerText],
    'contract-family-moved': [headerText, { ...contract, OMX_PANEL_FAMILIES: moved }, ownerText],
    'contract-placement-renamed': [headerText, { ...contract, OMX_PANEL_PLACEMENTS: renamed }, ownerText],
    'owner-table-code-moved': [headerText, contract, ownerText.replace(/X\(2, "roles", ROLES\)/, 'X(5, "roles", ROLES)')],
  };
  let fail = 0;
  for (const [arm, [h, k, o]] of Object.entries(arms)) {
    if (h === headerText && k === contract && o === ownerText) {
      console.error(`SABOTAGE ${arm}: the sabotage did not land`);
      fail++;
      continue;
    }
    const out = drift(enumsOf(h, k), declared, ownerListsOf(o));
    if (out.length) console.log(`SABOTAGE ${arm}: red (${out[0]})`);
    else {
      console.error(`SABOTAGE ${arm}: STAYED GREEN`);
      fail++;
    }
  }
  if (fail) process.exit(1);
  console.log('panel-words self-test: every sabotage red');
}
