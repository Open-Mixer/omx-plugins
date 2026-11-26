// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * plugin-labels.mjs — the words a surface shows in the user's language, generated from the shipped plugins'
 * declarations into one omx-contract domain, omx-plugin-labels (owner decision 2026-11-11, omx-plugins#18): each
 * panel section's title, each choice of an enumerated parameter and each factory preset's name is a word of a words
 * item, keyed `<ITEM>.<word>` in the catalogs, its `x-text` the English source sentence the catalogs translate
 * (omx-contract docs/contract-dir.md, "Domains"). A parameter's name stays English: it is no word here.
 *
 *   contract.d/omx-plugin-labels/omx-plugin-labels.json       the domain, written by tools/gen.mjs, never by hand
 *   catalogs/<lang>.json                                      its translations, one per listed language, installed with
 *                                                              the domain (its catalogs/) and to /usr/share/omx-plugins/catalogs/
 *
 * One item per shipped plugin, OMX_PLUGIN_LABELS_<STEM> (omx-keyed-gate: OMX_PLUGIN_LABELS_KEYED_GATE), so a plugin
 * added, or a preset added to one, moves no other plugin's codes (a word's x-code is its place in its item). A word is
 * `section.<section key>`, `choice.<parameter symbol>.<value>` or `preset.<preset id>`: stable while the text it carries
 * is reworded. The domain's version is its vocabulary's own, never the package's: the committed document's, 0.1.0 at first; an
 * item's `x-since` is the domain version that first carried it, read from the committed document.
 */
import { existsSync, readFileSync } from 'node:fs';
import { join } from 'node:path';

export const LABELS_DOMAIN = 'omx-plugin-labels';
export const LABELS_FILE = `contract.d/${LABELS_DOMAIN}/${LABELS_DOMAIN}.json`;
export const LABELS_CATALOGS = 'catalogs';
export const LABELS_OWNER = 'omx-plugins';
const PREFIX = 'OMX_PLUGIN_LABELS_';

const WHY = 'Section titles, enum values and preset names are translated, parameter names stay English (owner decision 2026-11-11); each translated text gets a key and one English source sentence (omx-contract#2), carried by an existing words item (omx-plugins#18).';

/** A plugin's item: OMX_PLUGIN_LABELS_ and its stem less `omx-` in upper snake case. */
export const itemOf = (stem) => `${PREFIX}${stem.replace(/^omx-/, '').replace(/-/g, '_').toUpperCase()}`;

/** The words a plugin `d` (a loaded declaration) gives, in declaration order, as `[item, word, text]`: its sections,
 * its parameters' choices, its presets. `choicesOf(p)` is a parameter's labelled values (tools/gen.mjs), `presets` the
 * plugin's preset files. */
export function labelsOf(d, choicesOf, presets) {
  const item = itemOf(d.stem);
  const out = [];
  for (const s of d.panel?.sections ?? []) {
    if (typeof s.label !== 'string' || s.label === '') throw new Error(`${d.stem}: the section '${s.key}' has no label: a section title is a translated word (omx-plugins#18)`);
    out.push([item, `section.${s.key}`, s.label]);
  }
  for (const p of d.params) {
    for (const c of choicesOf(p) ?? []) out.push([item, `choice.${p.symbol}.${c.value}`, c.label]);
  }
  for (const p of presets) out.push([item, `preset.${p.id}`, p.name]);
  return out;
}

/** A word's title, its macro name: the word's parts in lowerCamel (`choice.releaseMode.0` -> choiceReleaseMode0), a
 * negative value's minus spelled. */
export function titleOf(word) {
  const parts = word.replace(/(^|\.)-(\d)/g, '$1minus$2').split(/[^A-Za-z0-9]+/).filter(Boolean);
  return parts.map((w, i) => (i === 0 ? w : `${w[0].toUpperCase()}${w.slice(1)}`)).join('');
}

/** The committed document, or undefined: what keeps each item's x-since. */
export function committedLabels(root) {
  const f = join(root, LABELS_FILE);
  return existsSync(f) ? JSON.parse(readFileSync(f, 'utf8')) : undefined;
}

/** The domain document for the shipped declarations `ds` (in list order): `presetsOf(d)` their preset files,
 * `choicesOf(p)` a parameter's labelled values, `version` the release's, `committed` the committed document. A plugin
 * with no section, choice or preset has no item. */
export function labelsDomain(ds, { presetsOf, choicesOf, version, committed, format }) {
  const schemas = {};
  for (const d of ds) {
    const words = labelsOf(d, choicesOf, presetsOf(d));
    if (!words.length) continue;
    const item = itemOf(d.stem);
    if (schemas[item]) throw new Error(`${d.stem}: its item ${item} is another plugin's too`);
    const seen = new Map();
    const oneOf = words.map(([, word, text], code) => {
      const title = titleOf(word);
      if (seen.has(title)) throw new Error(`${item}: the words ${seen.get(title)} and ${word} have one title ${title}`);
      seen.set(title, word);
      if (/[{}]/.test(text)) throw new Error(`${item}: ${word}'s text '${text}' holds a brace: a label is no sentence with placeholders`);
      return { const: word, title, 'x-code': code, 'x-text': text };
    });
    schemas[item] = {
      description: `The labels ${d.name} shows that a surface translates: its panel's section titles (section.<section key>), its enumerated parameters' choice labels (choice.<parameter symbol>.<value>) and its factory presets' names (preset.<preset id>), each x-text the English text as declared. Its parameters' names stay English and are no word. Generated by tools/gen.mjs from the declaration and the preset files; the translations are in the catalogs.`,
      'x-since': committed?.components?.schemas?.[item]?.['x-since'] ?? version,
      'x-why': WHY,
      oneOf,
    };
  }
  return {
    $schema: 'https://json-schema.org/draft/2020-12/schema',
    $id: `https://openmixer.org/contract/${LABELS_DOMAIN}`,
    // the omx-contract format the document is written in, MAJOR.MINOR (omx-contract docs/contract-dir.md, "x-omx-contract")
    'x-omx-contract': format,
    info: {
      title: 'The OpenMixer plugins\' labels a surface translates: section titles, choice labels and preset names',
      version,
      'x-owner': LABELS_OWNER,
      'x-stability': 'experimental',
    },
    components: { schemas },
  };
}

export const labelsText = (doc) => `${JSON.stringify(doc, null, 2)}\n`;
