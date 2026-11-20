#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * panel-words-check.mjs — the enumerations of include/omx_panel_description.h are the panel vocabulary's, spelled
 * once elsewhere: the families and the widgets are omx-contract's PANEL_FAMILIES and PANEL_WIDGETS sheets
 * (data/panel.json), the display kinds, the display sources and the placements are omx-panel's lists
 * (tools/panel-description.mjs, which tools/gen.mjs already writes the rows with), and every one of them is the
 * X-list omx-clap's <omx-clap-host/omx_clap_ext.h> carries for the host, code and enumerator. Each enumerator
 * is the word in upper case after the enum's prefix, its code the word's position, and the count the list's:
 * the header is held to them here, so a word added, moved or renamed anywhere is red until the three agree.
 * `make panel-words`, part of `make test`.
 *
 *   node tools/test/panel-words-check.mjs               the check
 *   node tools/test/panel-words-check.mjs --self-test   the check, then copies of the header with a family
 *                                                       renumbered, one dropped, one appended, two widgets
 *                                                       swapped and a placement renamed, and a copy of the
 *                                                       owner's header with a code moved: each red
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

/** The header's enums held: the C enum, its prefix, and the owner's X-list. */
const ENUMS = {
  family: { c: 'omx_clap_panel_family', prefix: 'OMX_CLAP_FAMILY_', owner: 'OMX_CLAP_PANEL_FAMILIES' },
  widget: { c: 'omx_clap_panel_widget', prefix: 'OMX_CLAP_WIDGET_', owner: 'OMX_CLAP_PANEL_WIDGETS' },
  // omx-clap 0.33.1 no longer lists the display kinds (the table is omx-panel's, the struct field a number): no owner list,
  // the enum is held against omx-panel's DISPLAY_KINDS alone
  display_kind: { c: 'omx_clap_panel_display_kind', prefix: 'OMX_CLAP_DISPLAY_', owner: null },
  display_source: { c: 'omx_clap_panel_display_source', prefix: 'OMX_CLAP_DISPLAY_FROM_', owner: 'OMX_CLAP_PANEL_DISPLAY_SOURCES' },
  placement: { c: 'omx_clap_panel_placement', prefix: 'OMX_CLAP_PLACEMENT_', owner: 'OMX_CLAP_PANEL_PLACEMENTS' },
  table: { c: 'omx_clap_panel_table', prefix: 'OMX_CLAP_PANEL_', owner: 'OMX_CLAP_PANEL_TABLES' },
};

/** The header's enums: `{ family: [[enumerator, code], ...], ... }`, each in source order. */
export function enumsOf(text) {
  const out = {};
  for (const [key, { c, prefix }] of Object.entries(ENUMS)) {
    const block = text.match(new RegExp(`enum ${c} \\{([^}]*)\\}`))?.[1];
    out[key] = block ? [...block.matchAll(/^\s*(\w+)\s*=\s*(\d+)\s*,/gm)].map((m) => [m[1].startsWith(prefix) ? m[1].slice(prefix.length) : m[1], Number(m[2])]) : undefined;
  }
  return out;
}

/** The owner's X-lists: `{ family: [[code, word, enumerator], ...], ... }`. */
export function ownerListsOf(text) {
  const out = {};
  for (const [key, { owner }] of Object.entries(ENUMS)) {
    if (!owner) continue;
    const m = text.match(new RegExp(`#define ${owner}\\(X\\)((?:[^\\n]*\\\\\\n)*[^\\n]*)`));
    out[key] = m ? [...m[1].matchAll(/X\((\d+),\s*"([^"]*)",\s*(\w+)\)/g)].map((x) => [Number(x[1]), x[2], x[3]]) : undefined;
  }
  return out;
}

/** The refusals: each enum of `enums` (enumsOf) against the words `declared` (`{ family: [...], ... }`, a list
 * per enum that has one) and the owner's lists (ownerListsOf). */
export function drift(enums, declared, owner) {
  const errors = [];
  for (const [key, { c }] of Object.entries(ENUMS)) {
    const got = enums[key];
    if (!got) {
      errors.push(`${HEADER}: no enum ${c}`);
      continue;
    }
    const words = declared[key];
    if (words) {
      const want = words.map((w, i) => [w.toUpperCase(), i]);
      if (JSON.stringify(got) !== JSON.stringify(want)) errors.push(`${HEADER}: enum ${c} is ${got.map(([n, v]) => `${n}=${v}`).join(', ')}; the declaration says ${want.map(([n, v]) => `${n}=${v}`).join(', ')}`);
    }
    if (!ENUMS[key].owner) continue; // omx-panel's words are the only list (above)
    const x = owner[key];
    if (!x) {
      errors.push(`${OWNER}: no ${ENUMS[key].owner}(X) list`);
      continue;
    }
    const want = x.map(([code, , enumerator]) => [enumerator, code]);
    if (JSON.stringify(got) !== JSON.stringify(want)) errors.push(`${HEADER}: enum ${c} is ${got.map(([n, v]) => `${n}=${v}`).join(', ')}; ${OWNER} says ${want.map(([n, v]) => `${n}=${v}`).join(', ')}`);
    if (words && JSON.stringify(x.map(([, w]) => w)) !== JSON.stringify(words)) errors.push(`${OWNER}: ${ENUMS[key].owner} words ${x.map(([, w]) => w).join(', ')}; the declaration says ${words.join(', ')}`);
  }
  return errors;
}

/** The words of an omx-contract sheet of kind `words`, in its order. */
function sheetWords(dir, name) {
  const item = items(dir)[name];
  if (item?.kind !== 'words' || typeof item.value !== 'object') throw new Error(`${dir}: the resolved contract holds no ${name} words (omx-contract >= 3.7.0)`);
  return Object.values(item.value).map((w) => w.word);
}

/** The owner's header, where pkg-config says omx-clap's headers are, else the system's. */
function ownerHeader() {
  let dirs = [];
  try {
    dirs = execFileSync(process.env.PKG_CONFIG || 'pkg-config', ['--cflags-only-I', 'omx-clap'], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'] }).split(/\s+/).filter((f) => f.startsWith('-I')).map((f) => f.slice(2));
  } catch { /* not installed through pkg-config: the system include directory below */ }
  return [...dirs, '/usr/include', '/usr/local/include'].map((d) => join(d, OWNER)).find((f) => existsSync(f));
}

const contract = locateContract(root);
if (!contract.dir) {
  console.error(`panel-words: ${contract.why}`);
  process.exit(1);
}
const declared = {
  family: sheetWords(contract.dir, 'PANEL_FAMILIES'),
  widget: sheetWords(contract.dir, 'PANEL_WIDGETS'),
  display_kind: DISPLAY_KINDS,
  display_source: DISPLAY_SOURCES,
  placement: PLACEMENTS,
};
const headerText = readFileSync(join(root, HEADER), 'utf8');
const ownerFile = ownerHeader();
if (!ownerFile) {
  console.error(`panel-words: ${OWNER} not found: install omx-clap-devel (BUILDING.md)`);
  process.exit(1);
}
const ownerText = readFileSync(ownerFile, 'utf8');
const errors = drift(enumsOf(headerText), declared, ownerListsOf(ownerText));
if (errors.length) {
  for (const e of errors) console.error(`panel-words: ${e}`);
  process.exit(1);
}
console.log(`panel-words: ${Object.keys(ENUMS).length} enums of ${HEADER} are the declared words (${Object.values(declared).flat().length}) and ${OWNER}'s lists`);

if (process.argv.includes('--self-test')) {
  const arms = {
    'family-renumbered': headerText.replace('OMX_CLAP_FAMILY_GATE = 1,', 'OMX_CLAP_FAMILY_GATE = 2,'),
    'family-dropped': headerText.replace(/^\s*OMX_CLAP_FAMILY_PITCH = 11,\n/m, ''),
    'family-appended': headerText.replace('OMX_CLAP_FAMILY_PITCH = 11,', 'OMX_CLAP_FAMILY_PITCH = 11,\n  OMX_CLAP_FAMILY_OTHER = 12,'),
    'widgets-swapped': headerText.replace('OMX_CLAP_WIDGET_KNOB = 1,', 'OMX_CLAP_WIDGET_KNOB = 2,').replace('OMX_CLAP_WIDGET_FADER = 2,', 'OMX_CLAP_WIDGET_FADER = 1,'),
    'display-kind-renumbered': headerText.replace(/(OMX_CLAP_DISPLAY_\w+ = )1,/, '$12,'),
    'placement-renamed': headerText.replace('OMX_CLAP_PLACEMENT_BUS = 2,', 'OMX_CLAP_PLACEMENT_BUSS = 2,'),
  };
  let fail = 0;
  for (const [arm, text] of Object.entries(arms)) {
    if (text === headerText) {
      console.error(`SABOTAGE ${arm}: the sabotage did not land`);
      fail++;
      continue;
    }
    const out = drift(enumsOf(text), declared, ownerListsOf(ownerText));
    if (out.length) console.log(`SABOTAGE ${arm}: red (${out[0]})`);
    else {
      console.error(`SABOTAGE ${arm}: STAYED GREEN`);
      fail++;
    }
  }
  const movedOwner = ownerText.replace('X(1, "gate", GATE)', 'X(2, "gate", GATE)');
  const out = movedOwner === ownerText ? [] : drift(enumsOf(headerText), declared, ownerListsOf(movedOwner));
  if (out.length) console.log(`SABOTAGE owner-code-moved: red (${out[0]})`);
  else {
    console.error(`SABOTAGE owner-code-moved: ${movedOwner === ownerText ? 'the sabotage did not land' : 'STAYED GREEN'}`);
    fail++;
  }
  if (fail) process.exit(1);
  console.log('panel-words self-test: every sabotage red');
}
