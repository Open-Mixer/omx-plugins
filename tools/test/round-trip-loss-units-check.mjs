#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * round-trip-loss-units-check.mjs: a unit that tools/round-trip.mjs's ledger names is a unit the descriptors state.
 * round-trip.mjs keys an LV2 unit loss by the authored `controls[].travel.unit` (`lv2 controls[].travel.unit "<unit>"`).
 * Since the descriptors state the PARAM_UNITS ids (C2), that word is an id: `none`, not `""`, and `dBPerOct`, not
 * `dB/oct`. A key spelled the old way names a unit no descriptor states any more, so its count can never be matched.
 * Two laws, each with a sabotage arm that must go red (--self-test runs them):
 *
 *   1. the unit in every such key of LOSSES and UPSTREAM is spelled as a standard id: a letter first, then letters
 *      and digits, never "-", "/" or an empty string (docs/units.md of omx-contract, owner order 2026-11-24);
 *   2. it is the `travel.unit` of at least one control of a described plugin.
 *
 * `make round-trip-loss-units`, part of `make test`.
 */
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { descriptorFile } from '../descriptor.mjs';
import { LOSSES, ROOT, UPSTREAM, described } from '../round-trip.mjs';
import { isMain } from '../omx-contract.mjs';

const ID = /^[A-Za-z][A-Za-z0-9]*$/;
const KEY = /^(clap|lv2) controls\[\]\.travel\.unit (".*?")/;

/** Every travel.unit the described plugins state. */
export function authoredUnits(root = ROOT) {
  const units = new Set();
  for (const stem of described(root).inside) {
    const d = JSON.parse(readFileSync(join(root, 'plugins', stem, descriptorFile(stem)), 'utf8'));
    for (const c of d.controls ?? []) if (c.travel?.unit !== undefined) units.add(c.travel.unit);
  }
  return units;
}

/** Every defect, one line each: `keys` are ledger keys, `units` the authored units. */
export function lossUnitErrors(keys, units) {
  const errors = [];
  for (const k of keys) {
    const m = KEY.exec(k);
    if (!m) continue;
    const unit = JSON.parse(m[2]);
    if (!ID.test(unit)) errors.push(`'${k}': "${unit}" is no unit id (a letter first, then letters and digits; never "-", "/" or "")`);
    else if (!units.has(unit)) errors.push(`'${k}': no described plugin states the unit "${unit}"`);
  }
  return errors;
}

if (isMain(import.meta.url)) {
  const keys = [...Object.keys(LOSSES), ...Object.keys(UPSTREAM)];
  const units = authoredUnits();
  const errors = lossUnitErrors(keys, units);
  for (const e of errors) console.log(`FAIL round-trip-loss-units ${e}`);
  let bad = errors.length;
  const named = keys.filter((k) => KEY.test(k)).length;
  if (named === 0) {
    console.log('FAIL round-trip-loss-units the ledger names no travel.unit key: nothing was checked');
    bad++;
  }
  if (process.argv.includes('--self-test')) {
    const arms = [
      ['a symbol spelling ("dB/oct")', ['lv2 controls[].travel.unit "dB/oct"'], /is no unit id/],
      ['a hyphenated spelling ("dB-oct")', ['lv2 controls[].travel.unit "dB-oct"'], /is no unit id/],
      ['the empty spelling of none ("")', ['lv2 controls[].travel.unit ""'], /is no unit id/],
      ['an id no descriptor states ("dBPerHz")', ['lv2 controls[].travel.unit "dBPerHz"'], /no described plugin states/],
    ];
    for (const [what, k, want] of arms) {
      const got = lossUnitErrors(k, units);
      if (got.length === 1 && want.test(got[0])) console.log(`ok round-trip-loss-units self-test: ${what} is refused`);
      else {
        console.log(`FAIL round-trip-loss-units self-test: ${what} is not refused (${JSON.stringify(got)})`);
        bad++;
      }
    }
  }
  if (bad === 0) console.log(`ok round-trip-loss-units: ${named} ledger key(s) name a unit, each a stated PARAM_UNITS id`);
  process.exit(bad === 0 ? 0 : 1);
}
