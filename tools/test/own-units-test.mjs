#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * own-units-test.mjs — a plugin declares its own units in its declaration's `units` block (Open-Mixer/omx-plugins#172,
 * sorting row N1): each entry a word shaped as omx-contract's PARAM_UNITS words are (its domain schema's $defs.word: the
 * id in `const`, the display spelling in `x-symbol`), and a parameter names a unit of that block or a PARAM_UNITS id.
 * Units are owned per producer (omx-contract#144): one name, one home, so an own id or symbol that is PARAM_UNITS' is
 * refused, as is a parameter naming a unit neither declares.
 *
 * Over the fixture tools/test/own-units/omx-ownunit.decl.json, copied to a scratch plugins/ folder:
 *   1. it is valid against schema/plugin.decl.schema.json, and it loads;
 *   2. its own unit flows where a standard unit flows: the C parameter table (the CLAP face's value_to_text text) and
 *      the plain description show its x-symbol, the manual row too; the LV2 port carries `units:unit` of the unit's URI
 *      under the plugin's own, and the TTL defines that unit (units:Unit, render, symbol); the descriptor and the panel
 *      description state its id, and a preset reads the header's text back to it; the parameter contract states none,
 *      the text left to value_to_text, as for a standard unit omx-clap has no code for;
 *   3. the refusals, each by name: an own id that is a PARAM_UNITS id, an own x-symbol that is a PARAM_UNITS x-symbol,
 *      a parameter naming a unit neither declares, an id declared twice, an x-code off its place, an x-dimension that is
 *      no PARAM_UNIT_DIMENSIONS word, an x-group that is no PARAM_UNIT_SCOPES word, an own unit no parameter names, and
 *      (the schema) an entry with no x-symbol.
 *
 * `make own-units`, part of `make test`. Reads the installed omx-contract, as the generator does.
 */
import { mkdirSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import * as gen from '../gen.mjs';
import { descriptorOf, panelDescription } from '../descriptor.mjs';
import { contractOf } from '../param-contract.mjs';
import { loadRecipe, loadSchema, validate } from '../plugin-recipe.mjs';
import * as presets from '../preset-lib.mjs';

const { emitPluginTtl, loadDecl, paramRow, scaleOf, sourceOf } = gen;
/** A parameter's row of the C table (gen.mjs paramTableRow), or '' from a generator that has none. */
const paramTableRow = (d, p) => gen.paramTableRow?.(d, p) ?? '';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
const STEM = 'omx-ownunit';
const FIXTURE = JSON.parse(readFileSync(join(ROOT, 'tools', 'test', 'own-units', `${STEM}.decl.json`), 'utf8'));
let failed = 0;
const check = (ok, what, detail = '') => {
  console.log(`${ok ? 'PASS' : 'FAIL'} ${what}${ok ? '' : ` ${detail}`}`);
  if (!ok) failed++;
};

const schema = loadSchema(ROOT, loadRecipe(ROOT));
const scratch = mkdtempSync(join(tmpdir(), 'own-units-'));
try {
  let n = 0;
  /** The fixture, edited by `edit`, written to its own scratch plugins/<stem>/ and loaded: `{ d }` or `{ e }`. */
  const load = (edit) => {
    const decl = structuredClone(FIXTURE);
    edit?.(decl);
    const dir = join(scratch, String(n++), 'plugins', STEM);
    mkdirSync(dir, { recursive: true });
    writeFileSync(join(dir, `${STEM}.decl.json`), `${JSON.stringify(decl, null, 2)}\n`);
    try {
      return { d: loadDecl(dir), decl };
    } catch (e) {
      return { e: String(e.message), decl };
    }
  };

  // 1. the declaration
  const errs = validate(schema, schema, FIXTURE);
  check(errs.length === 0, 'the fixture with its own unit is valid against the declaration schema', errs.join('; '));
  const { d, e } = load();
  check(d !== undefined, 'the fixture loads', e);

  // 2. where the unit flows
  if (d) {
    const nudge = d.params.find((p) => p.symbol === 'nudge');
    const trim = d.params.find((p) => p.symbol === 'trimDb');
    check(/"nudge", "Nudge", "nd",/.test(paramTableRow(d, nudge)), 'the C parameter table (CLAP value_to_text) shows the own unit\'s x-symbol', paramTableRow(d, nudge));
    check(/"trimDb", "Trim", "dB",/.test(paramTableRow(d, trim)), 'a standard unit beside it shows its own x-symbol', paramTableRow(d, trim));
    check(sourceOf(d).endsWith('Nudge -4 to 4 nd'), 'the plain description shows the x-symbol', sourceOf(d));
    check(paramRow(nudge, d.units).range === '-4 to 4 nd, whole steps', 'the manual row shows the x-symbol', JSON.stringify(paramRow(nudge, d.units)));
    const ttl = emitPluginTtl(d);
    const port = ttl.split('    [\n').find((b) => b.includes('lv2:symbol "nudge"')) ?? '';
    check(port.includes('units:unit <urn:openmixer:ownunit#unit.nudge>'), 'the LV2 port carries the own unit\'s URI, under the plugin\'s', port);
    check(ttl.includes('<urn:openmixer:ownunit#unit.nudge>\n    a units:Unit ;\n    rdfs:label "Nudges, the fixture\'s own step of half a decibel." ;\n    units:render "%f nd" ;\n    units:symbol "nd" .'),
      'the TTL defines the own unit: label, render and symbol from its word', ttl.slice(ttl.lastIndexOf('<urn:openmixer:ownunit#')));
    const travel = descriptorOf(d).descriptor.controls.find((c) => c.symbol === 'nudge')?.travel;
    check(travel?.unit === 'nudge', 'the descriptor states the own unit\'s id', JSON.stringify(travel));
    const panel = panelDescription(d).controls.find((c) => c.symbol === 'nudge');
    check(panel?.unit === 'nudge', 'the panel description states the own unit\'s id', JSON.stringify(panel, (k, v) => (typeof v === 'bigint' ? String(v) : v)));
    const back = presets.unitIdOfText?.('nd', d.units);
    check(back === 'nudge' && presets.unitIdOfText('%', d.units) === 'percent', 'a preset reads the header\'s x-symbol back to the own unit\'s id', String(back));
    const row = contractOf(d, scaleOf).find((r) => r.symbol === 'nudge');
    check(row?.unit === 'none' && row.unitEnum === 0, 'the parameter contract states none, its text value_to_text\'s', JSON.stringify(row));
  }

  // 3. the refusals
  const refused = (what, edit, want) => {
    const r = load(edit);
    check(r.e !== undefined && want.test(r.e), what, r.e ?? 'it loaded');
  };
  refused('an own unit whose id is a PARAM_UNITS id is refused (one name, one home)', (x) => {
    x.units[0].const = 'dB';
    x.params[1].unit = 'dB';
  }, /units\/0: 'dB' is a PARAM_UNITS id/);
  refused('an own unit whose x-symbol is a PARAM_UNITS x-symbol is refused', (x) => {
    x.units[0]['x-symbol'] = 'dB/oct';
  }, /units\/0: x-symbol 'dB\/oct' is PARAM_UNITS' dBPerOct's/);
  refused('a parameter naming a unit neither the block nor PARAM_UNITS declares is refused', (x) => {
    x.params[1].unit = 'furlong';
  }, /'nudge' names unit 'furlong', neither a PARAM_UNITS id nor one of its own "units"/);
  refused('an own id declared twice is refused', (x) => {
    x.units.push({ ...x.units[0], 'x-code': 1, 'x-symbol': 'nd2' });
  }, /units\/1: 'nudge' is declared twice/);
  refused('an own unit whose x-code is not its place is refused', (x) => {
    x.units[0]['x-code'] = 3;
  }, /units\/0: x-code 3 is not its place, 0/);
  refused('an own x-dimension that is no PARAM_UNIT_DIMENSIONS word is refused', (x) => {
    x.units[0]['x-dimension'] = 'mood';
  }, /units\/0: x-dimension 'mood' is no word of PARAM_UNIT_DIMENSIONS/);
  refused('an own x-group that is no PARAM_UNIT_SCOPES word is refused', (x) => {
    x.units[0]['x-group'] = 'console';
  }, /units\/0: x-group 'console' is no word of PARAM_UNIT_SCOPES/);
  refused('an own unit no parameter names is refused', (x) => {
    x.params[1].unit = 'dB';
  }, /units\/0: 'nudge' is named by no parameter/);
  const bare = structuredClone(FIXTURE);
  delete bare.units[0]['x-symbol'];
  check(validate(schema, schema, bare).some((m) => /units\/0.*'x-symbol' is required/.test(m)), 'an own unit with no x-symbol is refused by the schema', validate(schema, schema, bare).join('; '));
} finally {
  rmSync(scratch, { recursive: true, force: true });
}
console.log(failed ? `own-units: ${failed} FAILED` : 'own-units: a plugin\'s own units are declared once, flow to every face, and collide with no standard unit');
process.exit(failed ? 1 : 0);
