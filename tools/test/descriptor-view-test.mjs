#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * descriptor-view-test.mjs — a plugin described in omx-contract's words (docs/declaration-as-descriptor.md):
 * its <stem>.descriptor.json and the extras of its <stem>.decl.json make the declaration tools/gen.mjs
 * resolves. Held by value over every described plugin, each rule with an arm that breaks it on a copy:
 *
 *   1. the descriptor is fresh: it is what its declaration renders, and it holds to the descriptor words of
 *      the installed omx-contract's schema (`$defs.descriptor`; a release before them is named, and from
 *      them on a schema not found is red); the links' owners travel with the render (owners.json), so
 *      OMX_CONTRACT_DIR alone gives the same descriptors, and a render without them does not;
 *   2. its links are the references: a travel is read from the link, never from the number it states (a
 *      number edited by hand is stale), and a link moved reads the other travel;
 *   3. the descriptor and the extras never both say one thing: an extras file naming the plugin is
 *      refused, and so is a control with no link and no reason of its own, and an id out of order.
 *
 * The generated files the view gives are the ones committed: `node tools/gen.mjs --check` holds that.
 * `make descriptor-view`, part of `make test`; it reads omx-contract as `make test` does.
 */
import { spawnSync } from 'node:child_process';
import { cpSync, existsSync, mkdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';
import { descriptorFile, descriptorOf, descriptorText, parseLink } from '../descriptor.mjs';
import { clapFeatureValue } from '../format-tables.mjs';
import { ROOT, loadDecl, pluginDirs } from '../gen.mjs';
import { OWNERS, findName, installedDataDir, items, locateContract } from '../omx-contract.mjs';
import { validate } from '../schema-validate.mjs';

let bad = 0, n = 0;
function check(what, got, want) {
  n++;
  const ok = JSON.stringify(got) === JSON.stringify(want);
  if (!ok) bad++;
  console.log(`${ok ? 'PASS' : 'FAIL'} descriptor-view ${what}${ok ? '' : `: got ${JSON.stringify(got)}, want ${JSON.stringify(want)}`}`);
}
const refusal = (f) => {
  try {
    f();
    return 'accepted';
  } catch (e) {
    return e.message.replace(/^.*?\.decl\.json: /, '');
  }
};

const described = pluginDirs().filter((dir) => existsSync(join(dir, descriptorFile(dir.split('/').pop()))));
check('at least one plugin is described', described.length > 0, true);
for (const dir of described) {
  const stem = dir.split('/').pop();
  check(`${stem}: the descriptor is what its declaration renders`, descriptorText(loadDecl(dir)) === readFileSync(join(dir, descriptorFile(stem)), 'utf8'), true);
}

// a band-Q control states the travel its source link resolves to (tools/descriptor.mjs promises it): eq's q is
// widened by the notch while the band is one, so the port spans 0.3..116 and its link is EQ_NOTCH_Q_RANGE.
// Held over the committed descriptor and over what the generator renders now (a generator that names the primary
// link again is red here even when the committed files were fixed by hand).
{
  const contract = locateContract().dir;
  const travelOf = (c) => {
    const link = parseLink(c.source?.from?.link);
    const e = link && contract ? findName(contract, link.kernel, link.ref).entry : undefined;
    const t = e?.kind === 'travels' ? (link.field ? e.value[link.field] : e.value) : undefined;
    return t ? { min: t.min, max: t.max } : undefined;
  };
  const bandQ = (d) => d.controls.filter((c) => c.kind === 'travel' && /^(b\d+_q|eq\d+Q)$/.test(c.symbol));
  let total = 0;
  for (const dir of described) {
    const stem = dir.split('/').pop();
    const committed = JSON.parse(readFileSync(join(dir, descriptorFile(stem)), 'utf8'));
    const rendered = descriptorOf(loadDecl(dir)).descriptor;
    for (const [label, d] of [['committed', committed], ['rendered', rendered]]) {
      const qs = bandQ(d);
      const bad = qs.filter((c) => JSON.stringify(travelOf(c)) !== JSON.stringify({ min: c.travel.min, max: c.travel.max })).map((c) => `${c.symbol}: states ${c.travel.min}..${c.travel.max}, ${c.source.from.link} resolves to ${JSON.stringify(travelOf(c))}`);
      if (label === 'committed') total += qs.length;
      if (qs.length) check(`${stem}: ${qs.length} band-Q controls (${label}) state the travel their link resolves to`, bad, []);
    }
  }
  if (items(contract).EQ_NOTCH_Q_RANGE) check('the band-Q controls checked are the 88 of eq8, eq16, eq32 and the strip', total, 88);
}

// the descriptor words: the installed omx-contract's schema
const where = locateContract();
// beside the render read, else in the installed data directory (a union render carries no schema)
const schemaFile = [where.dir, installedDataDir()].filter(Boolean).map((d) => join(d, 'omx-contract.schema.json')).find((f) => existsSync(f));
const schema = schemaFile && existsSync(schemaFile) ? JSON.parse(readFileSync(schemaFile, 'utf8')) : undefined;
// the descriptor words are always checked: a schema not found is red
if (schema?.$defs?.descriptor) {
  for (const dir of described) {
    const stem = dir.split('/').pop();
    check(`${stem}: the descriptor holds to omx-contract's descriptor words`, validate(schema, schema.$defs.descriptor, JSON.parse(readFileSync(join(dir, descriptorFile(stem)), 'utf8'))), []);
  }
  const broken = JSON.parse(readFileSync(join(described[0], descriptorFile(described[0].split('/').pop())), 'utf8'));
  broken.controls[0].ids = ['a'];
  check('arm: a travel carrying choice ids breaks the descriptor words', validate(schema, schema.$defs.descriptor, broken).length > 0, true);
} else check(`omx-contract ${where.version ?? '(none)'} carries the descriptor words: its schema is found and holds $defs.descriptor`, schemaFile ? 'no $defs.descriptor' : 'no omx-contract.schema.json', 'found');

// the owners a descriptor's links name come with the render: read through OMX_CONTRACT_DIR alone, with no
// installed omx-contract, a copy of the render with its owners.json gives the same descriptors, and one
// without gives others
{
  const first = described[0], firstStem = first.split('/').pop();
  const lone = (dir) => {
    const r = spawnSync(process.execPath, ['--input-type=module', '-e', `
      const { loadDecl } = await import(${JSON.stringify(join(ROOT, 'tools', 'gen.mjs'))});
      const { descriptorText } = await import(${JSON.stringify(join(ROOT, 'tools', 'descriptor.mjs'))});
      process.stdout.write(descriptorText(loadDecl(${JSON.stringify(first)})));`],
    { encoding: 'utf8', env: { ...process.env, OMX_CONTRACT_DIR: dir, OMX_CONTRACT_SCHEMA: schemaFile, PKG_CONFIG_PATH: '/nonexistent', PKG_CONFIG_LIBDIR: '/nonexistent' } });
    return r.status === 0 ? r.stdout : `refused: ${r.stderr.trim().split('\n').find((l) => l.startsWith('Error')) ?? r.stderr.trim()}`;
  };
  const render = join(ROOT, 'build', 'tmp', 'descriptor-view-render');
  rmSync(render, { recursive: true, force: true });
  mkdirSync(render, { recursive: true });
  cpSync(join(where.dir, 'omx-contract.json'), join(render, 'omx-contract.json'));
  check(`the render read carries ${OWNERS}`, existsSync(join(where.dir, OWNERS)), true);
  if (existsSync(join(where.dir, OWNERS))) cpSync(join(where.dir, OWNERS), join(render, OWNERS));
  const want = readFileSync(join(first, descriptorFile(firstStem)), 'utf8');
  check(`${firstStem}: OMX_CONTRACT_DIR alone gives the committed descriptor, its links' owners included`, lone(render) === want, true);
  rmSync(join(render, OWNERS), { force: true });
  check(`arm: the same render without ${OWNERS} names other owners`, lone(render) === want, false);
  rmSync(render, { recursive: true, force: true });
}

// the arms, on a copy of the first described plugin
const dir = described[0], stem = dir.split('/').pop();
const scratch = join(ROOT, 'build', 'tmp', 'descriptor-view');
const copy = join(scratch, stem);
const fresh = () => {
  rmSync(scratch, { recursive: true, force: true });
  mkdirSync(scratch, { recursive: true });
  cpSync(dir, copy, { recursive: true, filter: (src) => !src.includes('/build') });
};
const editJson = (name, f) => {
  const file = join(copy, name);
  const x = JSON.parse(readFileSync(file, 'utf8'));
  f(x);
  writeFileSync(file, `${JSON.stringify(x, null, 2)}\n`);
};
const DESC = descriptorFile(stem), DECL = `${stem}.decl.json`;
const travelled = JSON.parse(readFileSync(join(dir, DESC), 'utf8')).controls.find((c) => c.kind === 'travel' && c.source?.from);
const want = loadDecl(dir).params[travelled.id];

fresh();
editJson(DESC, (x) => { x.controls[travelled.id].travel.max += 1; });
const edited = loadDecl(copy);
check(`arm: ${travelled.symbol}'s max edited by hand is not read`, edited.params[travelled.id].max, want.max);
check(`arm: ${travelled.symbol}'s max edited by hand is stale`, descriptorText(edited) === readFileSync(join(copy, DESC), 'utf8'), false);

const other = JSON.parse(readFileSync(join(dir, DESC), 'utf8')).controls.find((c) => c.kind === 'travel' && c.source?.from && c.id !== travelled.id);
fresh();
editJson(DESC, (x) => { x.controls[travelled.id].source = other.source; });
check(`arm: ${travelled.symbol}'s link moved to ${other.symbol}'s is refused while the symbol says otherwise`, /must be its control's words/.test(refusal(() => loadDecl(copy))), true);
editJson(DECL, (x) => { x.controls = { ...x.controls, [travelled.symbol]: { ...x.controls?.[travelled.symbol], control: other.symbol } }; });
const moved = loadDecl(copy).params[travelled.id];
check(`arm: ${travelled.symbol}'s link moved to ${other.symbol}'s reads that travel`, [moved.min, moved.max, moved.def], [loadDecl(dir).params[other.id].min, loadDecl(dir).params[other.id].max, loadDecl(dir).params[other.id].def]);

fresh();
editJson(DECL, (x) => { x.name = 'omx elsewhere'; });
check('arm: an extras file naming the plugin is refused', refusal(() => loadDecl(copy)), `"name" is the descriptor's to say (${DESC})`);

fresh();
editJson(DESC, (x) => { delete x.controls[travelled.id].source; });
check('arm: a control with no link and no reason of its own is refused', refusal(() => loadDecl(copy)), `'${travelled.symbol}' has no source link and no "own" reason`);

fresh();
editJson(DESC, (x) => { [x.controls[0], x.controls[1]] = [x.controls[1], x.controls[0]]; });
check('arm: a control out of the parameter order is refused', refusal(() => loadDecl(copy)), `the descriptor's control 0 has id 1; our ids are the parameter order`);

// the descriptor's own words for the faces' description, CLAP features and LV2 class (omx-contract 3.0.0: plugin.doc
// and the extension bag, a CLAP feature by its macro name), read back into the declaration every generator reads, over every described plugin
for (const d of described) {
  const s = d.split('/').pop(), x = JSON.parse(readFileSync(join(d, descriptorFile(s)), 'utf8')), v = loadDecl(d);
  const types = [x.extensions.lv2['rdf:type']].flat();
  check(`${s}: the description, CLAP features and LV2 class are plugin.doc and the bag's`, [v.description, v.clap.features, ['lv2:Plugin', v.lv2.class].filter((t, i) => i === 0 || t !== 'lv2:Plugin')],
    [x.plugin.doc, [...x.plugin.features, ...[x.extensions.clap?.['clap_plugin_descriptor.features'] ?? []].flat().map(clapFeatureValue)], types]);
}

fresh();
editJson(DECL, (x) => { x.description = 'A description the descriptor already says.'; });
check('arm: an extras file carrying the description is refused', refusal(() => loadDecl(copy)), `"description" is the descriptor's to say (${DESC})`);

fresh();
editJson(DECL, (x) => { x.clap = { features: ['audio-effect', 'stereo'] }; });
check('arm: an extras file carrying the CLAP features is refused', refusal(() => loadDecl(copy)), `the plugin's CLAP id and features, its LV2 URI and class are the descriptor's to say (${DESC}: plugin and extensions)`);

fresh();
editJson(DESC, (x) => { x.extensions.lv2['rdf:type'] = 'lv2:DelayPlugin'; });
check('arm: an LV2 bag without lv2:Plugin is refused', refusal(() => loadDecl(copy)), `extensions.lv2["rdf:type"] is the LV2 face's types, "lv2:Plugin" and at most its class`);

const united = JSON.parse(readFileSync(join(dir, DESC), 'utf8')).controls.find((c) => c.kind === 'travel' && c.source?.from && c.travel.unit !== '');
fresh();
editJson(DESC, (x) => { x.controls[united.id].travel.unit = 'ratio'; });
check(`arm: ${united.symbol}, a ${united.travel.unit} travel stating ratio, is stale`, descriptorText(loadDecl(copy)) === readFileSync(join(copy, DESC), 'utf8'), false);

const ratios = described.flatMap((d) => JSON.parse(readFileSync(join(d, descriptorFile(d.split('/').pop())), 'utf8')).controls.filter((c) => c.travel?.unit === 'ratio').map((c) => `${d.split('/').pop()}.${c.symbol}`));
check('the ratio unit (gap 4) is stated by exactly the ratio controls: comp, de-esser, ducker, gate, the dynamic EQ\'s six bands and the multiband\'s five', ratios, [
  'omx-comp.ratio', 'omx-deesser.deessRatio', 'omx-ducker.ratio', ...[1, 2, 3, 4, 5, 6].map((n) => `omx-dyneq.ratio${n}`), 'omx-keyed-gate.ratio', ...[1, 2, 3, 4, 5].map((n) => `omx-multiband.ratio${n}`), 'omx-upward.ratio',
]);

rmSync(scratch, { recursive: true, force: true });
console.log(`descriptor-view: ${n - bad}/${n} pass`);
process.exit(bad ? 1 : 0);
