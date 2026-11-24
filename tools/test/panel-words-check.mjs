#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * panel-words-check.mjs — the panel words of include/omx_panel_description.h are omx-panel's (sorting row V1, row 3):
 * the families, widgets, display kinds, display sources and placements are the five words items of omx-panel's panel
 * domain (<contractdir>/panel/panel.json: PANEL_FAMILIES, PANEL_WIDGETS, PANEL_DISPLAY_KINDS, PANEL_DISPLAY_SOURCES,
 * PANEL_PLACEMENTS), which omx-panel renders as <omxdomain/panel_codes.h>, one X(code, WORD) list each. The header
 * spells none of them: each of its five enums is expanded from omx-panel's list, so its enumerators are
 * OMX_CLAP_<PREFIX><WORD> at omx-panel's codes. Held here: the domain's words = golden (as the plugins shipped them),
 * the render's lists = the domain's words in order, and the header's five enums expanded from those lists with no code
 * written. The table enum is omx-clap's own (OMX_CLAP_PANEL_TABLES of <omx-clap-host/omx_clap_ext.h>), held code for
 * code. `make panel-words`, part of `make test`.
 *
 *   node tools/test/panel-words-check.mjs               the check
 *   node tools/test/panel-words-check.mjs --self-test   the check, then: a header with a code spelled, a render with a
 *                                                       code moved, an owner table renumbered, and the whole check over
 *                                                       a copy of the contract directory whose panel domain lost knob:
 *                                                       each red
 */
import { execFileSync, spawnSync } from 'node:child_process';
import { cpSync, existsSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { installedContractDir } from '../omx-contract.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const HEADER = 'include/omx_panel_description.h';
const OWNER = 'omx-clap-host/omx_clap_ext.h';
const RENDER = 'omxdomain/panel_codes.h';
const DOMAIN = 'panel';

/** The header's panel enums: the C enum, its enumerator prefix and omx-panel's item. */
export const PANEL_ENUMS = {
  family: { c: 'omx_clap_panel_family', prefix: 'OMX_CLAP_FAMILY_', item: 'PANEL_FAMILIES' },
  widget: { c: 'omx_clap_panel_widget', prefix: 'OMX_CLAP_WIDGET_', item: 'PANEL_WIDGETS' },
  display_kind: { c: 'omx_clap_panel_display_kind', prefix: 'OMX_CLAP_DISPLAY_', item: 'PANEL_DISPLAY_KINDS' },
  display_source: { c: 'omx_clap_panel_display_source', prefix: 'OMX_CLAP_DISPLAY_FROM_', item: 'PANEL_DISPLAY_SOURCES' },
  placement: { c: 'omx_clap_panel_placement', prefix: 'OMX_CLAP_PLACEMENT_', item: 'PANEL_PLACEMENTS' },
};

/** The words as the plugins shipped them (the 0.20.0 header's enums), frozen: omx-panel's domain keeps each at its place (append only). */
export const GOLDEN = {
  family: ['none', 'gate', 'dynamics', 'limiter', 'deesser', 'eq', 'filter', 'drive', 'modulation', 'delay', 'reverb', 'pitch', 'utility'],
  widget: ['auto', 'knob', 'fader', 'switch', 'selector'],
  display_kind: ['gain_reduction_bar', 'transfer_curve', 'eq_response', 'meter', 'time_response'],
  display_source: ['family', 'meters', 'gain_adjustment', 'curve'],
  placement: ['insert', 'return', 'bus', 'master'],
};

const upper = (w) => w.toUpperCase().replace(/-/g, '_');

/** The domain's words, `{ family: [...], ... }`, read from <contractdir>/panel/panel.json. */
export function domainWords(contractdir = installedContractDir()) {
  const file = contractdir && join(contractdir, DOMAIN, `${DOMAIN}.json`);
  if (!file || !existsSync(file)) throw new Error(`domain ${DOMAIN}: ${file ?? 'no contract directory'} is not installed; this tree needs omx-panel-contract >= 0.16.0`);
  const schemas = JSON.parse(readFileSync(file, 'utf8')).components?.schemas ?? {};
  const out = {};
  for (const [key, { item }] of Object.entries(PANEL_ENUMS)) {
    const words = schemas[item]?.oneOf;
    if (!Array.isArray(words)) throw new Error(`${file}: holds no ${item} words`);
    out[key] = words.map((w) => w.const);
  }
  return out;
}

/** omx-panel's render: `{ family: [[WORD, code], ...], ... }`, each list's X(OMX_PANEL_<ITEM>_<WORD>, <WORD>) rows with
 * the code its define gives. */
export function renderLists(text) {
  const out = {};
  for (const [key, { item }] of Object.entries(PANEL_ENUMS)) {
    const m = text.match(new RegExp(`#define OMX_${item}\\(X\\)((?:[^\\n]*\\\\\\n)*[^\\n]*)`));
    out[key] = m ? [...m[1].matchAll(/X\((\w+),\s*(\w+)\)/g)].map(([, macro, word]) => {
      const code = text.match(new RegExp(`^#define ${macro} (\\d+)u\\b`, 'm'))?.[1];
      return [word, code === undefined ? undefined : Number(code)];
    }) : undefined;
  }
  return out;
}

/** The header's refusals: each panel enum must be omx-panel's list expanded, no code written in it. */
export function headerErrors(text) {
  const errors = [];
  if (!text.includes(`#include <${RENDER}>`)) errors.push(`${HEADER}: does not include <${RENDER}>`);
  for (const { c, prefix, item } of Object.values(PANEL_ENUMS)) {
    const block = text.match(new RegExp(`enum ${c} \\{([^}]*)\\}`))?.[1];
    if (block === undefined) { errors.push(`${HEADER}: no enum ${c}`); continue; }
    if (/=\s*\d/.test(block) || new RegExp(`\\b${prefix}\\w+\\s*[,=]`).test(block)) errors.push(`${HEADER}: enum ${c} spells its words or codes; it is omx-panel's OMX_${item}(X) expanded`);
    if (!new RegExp(`^\\s*OMX_${item}\\(\\w+\\)\\s*$`).test(block)) errors.push(`${HEADER}: enum ${c} is not OMX_${item}(<macro>) expanded`);
  }
  return errors;
}

/** The refusals of the render against the domain's words and of the domain's words against the golden. */
export function wordErrors(words, lists) {
  const errors = [];
  for (const key of Object.keys(PANEL_ENUMS)) {
    const w = words[key];
    if (JSON.stringify(w.slice(0, GOLDEN[key].length)) !== JSON.stringify(GOLDEN[key])) errors.push(`${PANEL_ENUMS[key].item}: the domain's words are ${w.join(', ')}; golden ${GOLDEN[key].join(', ')}`);
    const want = w.map((x, i) => [upper(x), i]);
    if (JSON.stringify(lists[key]) !== JSON.stringify(want)) errors.push(`${RENDER}: OMX_${PANEL_ENUMS[key].item}(X) is ${JSON.stringify(lists[key])}; the domain says ${JSON.stringify(want)}`);
  }
  return errors;
}

/** The table enum (omx-clap's own list) against OMX_CLAP_PANEL_TABLES. */
export function tableErrors(headerText, ownerText) {
  const got = [...(headerText.match(/enum omx_clap_panel_table \{([^}]*)\}/)?.[1] ?? '').matchAll(/^\s*OMX_CLAP_PANEL_(\w+)\s*=\s*(\d+)\s*,/gm)].map((m) => [m[1], Number(m[2])]);
  const m = ownerText.match(/#define OMX_CLAP_PANEL_TABLES\(X\)((?:[^\n]*\\\n)*[^\n]*)/);
  const want = m ? [...m[1].matchAll(/X\((\d+),\s*"[^"]*",\s*(\w+)\)/g)].map((x) => [x[2], Number(x[1])]) : [];
  return JSON.stringify(got) === JSON.stringify(want) && got.length ? [] : [`${HEADER}: enum omx_clap_panel_table is ${JSON.stringify(got)}; ${OWNER} says ${JSON.stringify(want)}`];
}

/** An installed header, where pkg-config says `pkg`'s headers are, else the system's. */
function installedHeader(pkg, name) {
  let dirs = [];
  try {
    dirs = execFileSync(process.env.PKG_CONFIG || 'pkg-config', ['--cflags-only-I', pkg], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'] }).split(/\s+/).filter((f) => f.startsWith('-I')).map((f) => f.slice(2));
  } catch { /* not installed through pkg-config: the system include directory below */ }
  return [...dirs, '/usr/include', '/usr/local/include'].map((d) => join(d, name)).find((f) => existsSync(f));
}

const headerText = readFileSync(join(root, HEADER), 'utf8');
const errors = [];
let words;
try {
  words = domainWords();
} catch (e) {
  errors.push(e.message);
}
const renderFile = installedHeader('omx-panel', RENDER);
const ownerFile = installedHeader('omx-clap', OWNER);
if (!renderFile) errors.push(`${RENDER} not found: install omx-panel-contract >= 0.16.0`);
if (!ownerFile) errors.push(`${OWNER} not found: install omx-clap-devel >= 0.34.1`);
const renderText = renderFile ? readFileSync(renderFile, 'utf8') : '';
const ownerText = ownerFile ? readFileSync(ownerFile, 'utf8') : '';
if (words && renderFile) errors.push(...wordErrors(words, renderLists(renderText)));
errors.push(...headerErrors(headerText));
if (ownerFile) errors.push(...tableErrors(headerText, ownerText));
if (errors.length) {
  for (const e of errors) console.error(`panel-words: ${e}`);
  process.exit(1);
}
console.log(`panel-words: the five panel enums of ${HEADER} are omx-panel's lists (${Object.values(words).flat().length} words of its panel domain, ${RENDER}), none spelled here; the table enum is ${OWNER}'s`);

if (process.argv.includes('--self-test')) {
  let fail = 0;
  const arm = (name, landed, out) => {
    if (!landed) { console.error(`SABOTAGE ${name}: the sabotage did not land`); fail++; } else if (out.length) console.log(`SABOTAGE ${name}: red (${out[0]})`);
    else { console.error(`SABOTAGE ${name}: STAYED GREEN`); fail++; }
  };
  const spelled = headerText.replace(/enum omx_clap_panel_widget \{[^}]*\}/, 'enum omx_clap_panel_widget {\n  OMX_CLAP_WIDGET_AUTO = 0,\n}');
  arm('header-spells-a-code', spelled !== headerText, headerErrors(spelled));
  const moved = renderText.replace('#define OMX_PANEL_FAMILIES_GATE 1u', '#define OMX_PANEL_FAMILIES_GATE 2u');
  arm('render-code-moved', moved !== renderText, wordErrors(words, renderLists(moved)));
  const table = headerText.replace('OMX_CLAP_PANEL_ROLES = 2,', 'OMX_CLAP_PANEL_ROLES = 3,');
  arm('table-renumbered', table !== headerText, tableErrors(table, ownerText));
  // (c) the read is omx-panel's domain: the whole check over a copy of the contract directory whose PANEL_WIDGETS lost knob
  const tmp = mkdtempSync(join(tmpdir(), 'panel-words-'));
  try {
    const copy = join(tmp, 'contract.d');
    cpSync(installedContractDir(), copy, { recursive: true });
    const file = join(copy, DOMAIN, `${DOMAIN}.json`);
    const doc = JSON.parse(readFileSync(file, 'utf8'));
    const w = doc.components.schemas.PANEL_WIDGETS;
    w.oneOf = w.oneOf.filter((x) => x.const !== 'knob');
    writeFileSync(file, JSON.stringify(doc));
    const r = spawnSync(process.execPath, [fileURLToPath(import.meta.url)], { encoding: 'utf8', env: { ...process.env, OMX_INSTALLED_CONTRACTDIR: copy } });
    arm('domain-without-knob', true, r.status !== 0 ? [r.stderr.trim().split('\n')[0]] : []);
  } catch (e) {
    console.error(`SABOTAGE domain-without-knob: could not be planted: ${e.message}`);
    fail++;
  } finally {
    rmSync(tmp, { recursive: true, force: true });
  }
  if (fail) process.exit(1);
  console.log('panel-words self-test: every sabotage red');
}
