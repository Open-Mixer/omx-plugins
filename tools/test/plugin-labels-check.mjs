#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
//
// tools/test/plugin-labels-check.mjs — every text a surface translates has a key and an English source sentence
// (owner decision 2026-11-11, omx-plugins#18): each shipped plugin's panel section titles, the labels of its
// enumerated parameters' values and its factory presets' names are each a word of the committed domain
// contract.d/omx-plugin-labels/omx-plugin-labels.json (tools/plugin-labels.mjs), its x-text the text as declared, and
// the domain holds no other word; a parameter's name stays English, so no word is a parameter. Each listed language
// (omx-contract's languages.json) has its catalog beside the domain, owned by omx-plugins, its every key a word of the
// domain; that each entry's placeholders and the catalog's shape pass is omx-contract validate's (plugin-labels-validate.sh).
//
//   node tools/test/plugin-labels-check.mjs              the check
//   node tools/test/plugin-labels-check.mjs --self-test  a declaration that gains a section with no key, a choice
//                                                        relabelled, a preset with no key, a parameter name given a
//                                                        key (in the domain, or by the generator), a stray word, a
//                                                        stray item, the domain absent, a missing catalog and a
//                                                        catalog key no word has each refuse; the shipped tree passes
// Exit: 0 pass, 1 refused.
import { existsSync, readdirSync, readFileSync } from 'node:fs';
import { join } from 'node:path';
import { ROOT, choicesOf, loadDecl, pluginDirs, ships } from '../gen.mjs';
import { presetsOf } from '../preset-lib.mjs';
import { isMain } from '../omx-contract.mjs';
import { LABELS_CATALOGS, LABELS_FILE, LABELS_OWNER, itemOf, labelsOf } from '../plugin-labels.mjs';

/** omx-contract's listed languages: its data's languages.json (OMX_CONTRACT_DATADIR, else the installed one). */
function listedLanguages() {
  const file = join(process.env.OMX_CONTRACT_DATADIR ?? '/usr/share/omx-contract', 'languages.json');
  if (!existsSync(file)) throw new Error(`no language list at ${file}: install omx-contract-data, or name its directory in OMX_CONTRACT_DATADIR`);
  return JSON.parse(readFileSync(file, 'utf8')).languages;
}

/** The catalogs beside the domain, by language (undefined when absent). */
function readCatalogs(langs) {
  const dir = join(ROOT, LABELS_CATALOGS);
  const have = existsSync(dir) ? readdirSync(dir).filter((f) => f.endsWith('.json')) : [];
  const out = new Map(have.map((f) => [f.slice(0, -5), JSON.parse(readFileSync(join(dir, f), 'utf8'))]));
  for (const l of langs) if (!out.has(l)) out.set(l, undefined);
  return out;
}

/**
 * The problems, one line each: `ds` the shipped declarations, `presets(d)` a plugin's preset files, `doc` the
 * committed domain (undefined when absent), `catalogs` by language, `langs` the listed languages, `labels` the words
 * a declaration gives (tools/plugin-labels.mjs labelsOf; a self-test arm hands a broken one).
 */
export function problems({ ds, presets, doc, catalogs, langs, labels = labelsOf }) {
  const out = [];
  if (doc === undefined) out.push(`${LABELS_FILE}: no domain: the section titles, choices and preset names have no key (run \`node tools/gen.mjs\`)`);
  const schemas = doc?.components?.schemas ?? {};
  const words = new Map();
  for (const [item, schema] of Object.entries(schemas)) for (const w of schema.oneOf ?? []) words.set(`${item}.${w.const}`, w['x-text']);
  const want = new Set();
  // a word that names a parameter of its item's plugin, other than as one of its choices: `<ITEM>[.<any>].<symbol>`
  const named = [];
  for (const d of ds) {
    const given = labels(d, choicesOf, presets(d));
    for (const [item, word, text] of given) {
      const key = `${item}.${word}`;
      want.add(key);
      if (!words.has(key)) out.push(`${key}: '${text}' has no key (run \`node tools/gen.mjs\`)`);
      else if (words.get(key) !== text) out.push(`${key}: the English source sentence is '${words.get(key)}', the declaration says '${text}' (run \`node tools/gen.mjs\`)`);
    }
    // a parameter's name stays English (owner decision 2026-11-11): renaming every parameter moves no word or text
    const renamed = { ...d, params: d.params.map((p) => ({ ...p, name: `${p.name} renamed` })) };
    const again = labels(renamed, choicesOf, presets(d));
    if (JSON.stringify(again) !== JSON.stringify(given)) out.push(`${d.stem}: a parameter's name reaches a key: renaming the parameters changes the words`);
    for (const p of d.params) named.push([`${itemOf(d.stem)}.`, p.symbol, p.name]);
  }
  for (const key of words.keys()) {
    if (want.has(key)) continue;
    const param = named.find(([item, symbol]) => key.startsWith(item) && !key.startsWith(`${item}choice.`) && key.split('.').includes(symbol));
    if (param) out.push(`${key}: a key for the parameter name '${param[2]}', which stays English`);
    else out.push(`${key}: no section title, choice or preset name of a shipped plugin gives this word`);
  }
  const items = new Set([...want].map((k) => k.slice(0, k.indexOf('.'))));
  for (const item of Object.keys(schemas)) if (!items.has(item)) out.push(`${LABELS_FILE}: ${item} is no shipped plugin's item`);
  for (const lang of langs) {
    const c = catalogs.get(lang);
    const rel = `${LABELS_CATALOGS}/${lang}.json`;
    if (c === undefined) { out.push(`${rel}: no catalog for the listed language ${lang}`); continue; }
    if (c.kind !== 'catalog' || c.lang !== lang || c.owner !== LABELS_OWNER) out.push(`${rel}: not the ${lang} catalog of ${LABELS_OWNER} (kind ${c.kind}, lang ${c.lang}, owner ${c.owner})`);
    for (const key of Object.keys(c.messages ?? {})) if (!words.has(key)) out.push(`${rel}: ${key} is no word of the domain`);
  }
  for (const lang of catalogs.keys()) if (!langs.includes(lang)) out.push(`${LABELS_CATALOGS}/${lang}.json: ${lang} is no listed language`);
  return out;
}

/** Every key of a domain document, `<ITEM>.<word>`. */
const words = (doc) => Object.entries(doc.components.schemas).flatMap(([item, s]) => s.oneOf.map((w) => `${item}.${w.const}`));

function main(argv) {
  const ds = pluginDirs().map((p) => loadDecl(p)).filter(ships);
  const presets = (d) => presetsOf(d.clap.id);
  const file = join(ROOT, LABELS_FILE);
  const doc = existsSync(file) ? JSON.parse(readFileSync(file, 'utf8')) : undefined;
  const langs = listedLanguages();
  const catalogs = readCatalogs(langs);
  const input = { ds, presets, doc, catalogs, langs };
  if (argv.includes('--self-test')) {
    const clone = (x) => JSON.parse(JSON.stringify(x));
    const comp = (all) => all.find((d) => d.stem === 'omx-comp');
    // each arm, and the line its refusal must carry
    const EXPECT = {
      'a declaration gains a section with no key': "OMX_PLUGIN_LABELS_COMP.section.sabotage: 'Sabotage' has no key",
      'a choice relabelled in its declaration': 'OMX_PLUGIN_LABELS_COMP.choice.releaseMode.0: the English source sentence',
      'a preset with no key': "OMX_PLUGIN_LABELS_COMP.preset.sabotage: 'Sabotage' has no key",
      'a parameter name given a key': 'which stays English',
      'the generator keys a parameter name': "a parameter's name reaches a key",
      'a stray word': 'OMX_PLUGIN_LABELS_COMP.preset.stray: no section title',
      'a stray item': 'OMX_PLUGIN_LABELS_WOBBLE is no shipped plugin',
      'the domain absent': 'no domain',
      'a listed language with no catalog': 'no catalog for the listed language ca',
      'a catalog key no word has': 'is no word of the domain',
    };
    const arms = {
      'a declaration gains a section with no key': () => {
        const all = ds.map((d) => (d.stem === 'omx-comp' ? { ...d, panel: { ...d.panel, sections: [...d.panel.sections, { key: 'sabotage', label: 'Sabotage', controls: [] }] } } : d));
        return { ...input, ds: all };
      },
      'a choice relabelled in its declaration': () => {
        const all = ds.map((d) => (d.stem === 'omx-comp' ? { ...d, params: d.params.map((p) => (p.symbol === 'releaseMode' ? { ...p, points: choicesOf(p).map((c, i) => (i ? c : { ...c, label: 'Sabotage' })) } : p)) } : d));
        return { ...input, ds: all };
      },
      'a preset with no key': () => ({ ...input, presets: (d) => (d.stem === 'omx-comp' ? [...presets(d), { id: 'sabotage', name: 'Sabotage' }] : presets(d)) }),
      'a parameter name given a key': () => {
        const d = clone(doc);
        const p = comp(ds).params[0];
        d.components.schemas.OMX_PLUGIN_LABELS_COMP.oneOf.push({ const: `param.${p.symbol}`, title: 'sabotage', 'x-code': 9999, 'x-text': p.name });
        return { ...input, doc: d };
      },
      'the generator keys a parameter name': () => ({ ...input, labels: (d, c, p) => [...labelsOf(d, c, p), ...d.params.map((q) => [itemOf(d.stem), `param.${q.symbol}`, q.name])] }),
      'a stray word': () => {
        const d = clone(doc);
        d.components.schemas.OMX_PLUGIN_LABELS_COMP.oneOf.push({ const: 'preset.stray', title: 'presetStray', 'x-code': 9999, 'x-text': 'Stray' });
        return { ...input, doc: d };
      },
      'a stray item': () => {
        const d = clone(doc);
        d.components.schemas.OMX_PLUGIN_LABELS_WOBBLE = d.components.schemas.OMX_PLUGIN_LABELS_COMP;
        return { ...input, doc: d };
      },
      'the domain absent': () => ({ ...input, doc: undefined }),
      'a listed language with no catalog': () => ({ ...input, catalogs: new Map([...catalogs].map(([l, c]) => [l, l === 'ca' ? undefined : c])) }),
      'a catalog key no word has': () => ({ ...input, catalogs: new Map([...catalogs].map(([l, c]) => [l, l === 'ca' ? { ...c, messages: { ...c.messages, ['OMX_PLUGIN_LABELS_COMP.section.sabotage']: { text: 'Sabotatge', by: 'human' } } } : c])) }),
    };
    let bad = 0;
    const base = problems(input);
    if (base.length) { console.error(`self-test: the shipped tree does not pass (${base[0]})`); bad++; }
    for (const [name, arm] of Object.entries(arms)) {
      const p = problems(arm());
      const hit = p.find((l) => l.includes(EXPECT[name]));
      if (!hit) { console.error(`self-test: ${name} ${p.length ? `refused without '${EXPECT[name]}' (${p[0]})` : 'passed'}`); bad++; } else console.log(`ok   ${name} refuses: ${hit}`);
    }
    console.log(bad ? `plugin-labels self-test: ${bad} arm(s) wrong` : `plugin-labels self-test: ${Object.keys(arms).length} arms refuse, the shipped tree passes`);
    return bad ? 1 : 0;
  }
  const p = problems(input);
  for (const l of p) console.error(`plugin-labels: ${l}`);
  if (!p.length) {
    const count = (kind) => [...words(doc)].filter((k) => k.split('.')[1] === kind).length;
    console.log(`plugin-labels: ${ds.length} shipped plugins in ${Object.keys(doc.components.schemas).length} items, every section title (${count('section')}), choice (${count('choice')}) and preset name (${count('preset')}) a key with its English sentence, no parameter name a key, ${langs.length} catalogs (${langs.join(', ')})`);
  }
  return p.length ? 1 : 0;
}

if (isMain(import.meta.url)) process.exit(main(process.argv.slice(2)));
