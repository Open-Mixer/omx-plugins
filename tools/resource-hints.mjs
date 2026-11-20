// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * resource-hints.mjs — what a plugin declares about its own footprint, in plugin-hostd's RESOURCE_HINTS (its domain
 * `resource`, plugin-hostd-contract >= 0.21.0; sorting row V28: the words are a host's, KEPT at plugin-hostd), read from
 * the render the build reads, where that package owns the item; from the ONE place a plugin states it: its declaration's
 * `resourceHints`, keyed by the hint's word. tools/gen.mjs renders both carriers from it:
 *
 *   - the CLAP face: OMX_<K>_RESOURCE_HINTS in generated/omx_<kernel>_params.h, the omx_resource_hints_t the
 *     face's org.openmixer.resource-hints.draft/0 `get` fills;
 *   - the LV2 face: one triple per stated hint on the plugin in generated/<stem>.lv2/<stem>.ttl, in the
 *     namespace the contract names (prefix omxres).
 *
 * A hint the declaration leaves out is unknown: its bit clear, its LV2 property absent, never zero. The words,
 * their order (the bit order), their units and the LV2 namespace are read from the installed contract, never
 * typed here; only the C type and XSD datatype per unit are spelled, as omx-contract's lib/hints.mjs spells them,
 * and tools/test/resource-hints-check.mjs holds them to the installed header and Turtle.
 */
import { items, OWNERS, ownersOf } from './omx-contract.mjs';

/** The contract item that declares the hints, and the package that owns it (sorting row V28). */
export const HINTS_ITEM = 'RESOURCE_HINTS';
export const HINTS_OWNER = 'plugin-hostd';

/** The hints every shipped plugin states: what each knows about itself without a measurement (#216's first cut).
 * The figures (footprint at rest, per voice, shared load, activate time) come from measurement
 * (plugin-qualify#116) and stay undeclared until a plugin has one. */
export const REQUIRED_HINTS = Object.freeze(['threads-spawned', 'streams-from-disk', 'opens-sockets']);

/** The C type, XSD datatype and the largest figure of a hint, by the word of RESOURCE_UNITS it is given in. */
export const HINT_UNITS = Object.freeze({
  bytes: { c: 'uint64_t', xsd: 'integer', max: Number.MAX_SAFE_INTEGER },
  count: { c: 'uint32_t', xsd: 'integer', max: 0xffffffff },
  ms: { c: 'uint32_t', xsd: 'integer', max: 0xffffffff },
  flag: { c: 'bool', xsd: 'boolean' },
});

const constOf = (key) => key.replace(/([a-z0-9])([A-Z])/g, '$1_$2').toUpperCase();

/** The hint vocabulary of the release at `dir`: the CLAP extension id, the LV2 prefix and namespace, and each
 * hint in the order of the declaration (bit k is hint k). */
export function hintVocabulary(dir) {
  const item = items(dir)[HINTS_ITEM];
  if (!item || item.kind !== 'words' || !item.extension) throw new Error(`${dir}: holds no ${HINTS_ITEM} words item with an extension (plugin-hostd-contract >= 0.21.0 declares it)`);
  const owners = ownersOf(dir);
  if (!owners.size) throw new Error(`${dir}: the render read carries no ${OWNERS}, so the owner of its ${HINTS_ITEM} is unknown (the words are ${HINTS_OWNER}'s)`);
  const owner = owners.get(item.rel)?.package;
  if (owner !== HINTS_OWNER) throw new Error(`${dir}: its ${HINTS_ITEM} is ${owner ?? 'no package'}'s (${item.rel}); the words are ${HINTS_OWNER}'s, this tree needs plugin-hostd-contract >= 0.21.0`);
  const hints = Object.entries(item.value).map(([key, w], bit) => {
    const unit = HINT_UNITS[w.figureUnit];
    if (!unit) throw new Error(`${dir}: ${HINTS_ITEM}.${key} is given in ${w.figureUnit}, a unit no carrier here has a type for (${Object.keys(HINT_UNITS).join(', ')})`);
    return { key, word: w.word, figureUnit: w.figureUnit, basis: w.basis, bit, member: w.word.replace(/-/g, '_'), macro: `OMX_RESOURCE_HINT_${constOf(key)}`, ...unit };
  });
  return { owner, clap: item.extension.clap, prefix: item.extension.lv2.prefix, namespace: item.extension.lv2.namespace, hints };
}

/** The hints a declaration states, in the vocabulary's order, each with its value: `[]` for a declaration
 * with no `resourceHints`. `contractDir()` is called only when there is something to resolve. */
export function statedHints(file, d, contractDir) {
  if (d.resourceHints === undefined) return { vocabulary: undefined, stated: [] };
  const rh = d.resourceHints;
  if (rh === null || typeof rh !== 'object' || Array.isArray(rh)) throw new Error(`${file}: "resourceHints" maps a hint's word to its value`);
  const vocabulary = hintVocabulary(contractDir());
  const byWord = new Map(vocabulary.hints.map((h) => [h.word, h]));
  for (const word of Object.keys(rh)) if (!byWord.has(word)) throw new Error(`${file}: "resourceHints" names '${word}', no word of ${HINTS_ITEM} (${[...byWord.keys()].join(', ')})`);
  const stated = vocabulary.hints.filter((h) => h.word in rh).map((h) => {
    const v = rh[h.word];
    const ok = h.figureUnit === 'flag' ? typeof v === 'boolean' : Number.isInteger(v) && v >= 0 && v <= h.max;
    if (!ok) throw new Error(`${file}: "resourceHints" '${h.word}' is ${JSON.stringify(v)}; a ${h.figureUnit} hint is ${h.figureUnit === 'flag' ? 'true or false' : `a whole number from 0 to ${h.max}`}`);
    return { ...h, value: v };
  });
  return { vocabulary, stated };
}

/** The `declared` mask of a stated list. */
export const maskOf = (stated) => stated.reduce((m, h) => m | (1 << h.bit), 0) >>> 0;

const cValue = (h) => (h.figureUnit === 'flag' ? `${h.value}` : `${h.value}u${h.c === 'uint64_t' ? 'll' : ''}`);

/** The C initializer of OMX_<K>_RESOURCE_HINTS, for the generated parameter header. */
export function emitHintsC(K, stated) {
  const declared = stated.length ? stated.map((h) => h.macro).join(' | ') : '0u';
  return `/* org.openmixer.resource-hints.draft/0 (plugin-hostd ${HINTS_ITEM}, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of \`declared\` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_${K}_RESOURCE_HINTS = {
  .declared = ${declared},
${stated.map((h) => `  .${h.member} = ${cValue(h)},`).join('\n')}${stated.length ? '\n' : ''}};
`;
}

const ttlValue = (h) => (h.figureUnit === 'flag' ? `${h.value}` : `"${h.value}"^^xsd:${h.xsd}`);

/** The Turtle prefixes the stated hints need, as `[prefix, namespace]` pairs. */
export function hintPrefixes(vocabulary, stated) {
  if (!stated.length) return [];
  return [[vocabulary.prefix, vocabulary.namespace], ...(stated.some((h) => h.figureUnit !== 'flag') ? [['xsd', 'http://www.w3.org/2001/XMLSchema#']] : [])];
}

/** The plugin's triples of the stated hints, one `    <prefix>:<key> <value> ;` line each. */
export const hintTriples = (vocabulary, stated) => stated.map((h) => `    ${vocabulary.prefix}:${h.key} ${ttlValue(h)} ;`);
