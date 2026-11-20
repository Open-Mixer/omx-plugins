// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
//
// Reads one clap-validator JSON report and prints a line per test that failed or crashed:
//   node tools/clap-validate-summary.mjs <plugin> <report.json>
// Exit 1 when any test failed or crashed, or the report cannot be read; a report with no tests at
// all is unreadable, not clean. Exit 3 when the only failures are the one accepted below (the caller
// counts that as a pass even though the validator exited non-zero).
//
// The ONE accepted failure (Open-Mixer/omx-plugins#91, coordinator ruling 2026-11-16): the kernels run
// only at omx-contract's nine ORACLE_FULL_RATES by design, so activate refuses any other rate, and
// clap-validator's process-varying-sample-rates, which activates at 8 kHz among others, reports
// "'clap_plugin::activate()' returned false". That failure is accepted when the rate it names is NOT
// one of the nine, read from the installed contract (never a typed copy). The same refusal at one of
// the nine, in any other test, or any other failure stays red.
import { readFileSync } from 'node:fs';
import { items, locateContract } from './omx-contract.mjs';

const ACCEPTED_TEST = 'plugin-instance/process-varying-sample-rates';
const REFUSAL = /^Error while processing with ([0-9]+(?:\.[0-9]+)?) ?hz sample rate 'clap_plugin::activate\(\)' returned false\.?$/i;
let oracleRates;
/** omx-contract's ORACLE_FULL_RATES, from the installed contract this tree builds against. */
function oracleFullRates() {
  if (!oracleRates) {
    const c = locateContract();
    if (!c.dir) throw new Error(`no omx-contract to read ORACLE_FULL_RATES from: ${c.why}`);
    const v = items(c.dir).ORACLE_FULL_RATES?.value;
    if (!Array.isArray(v) || !v.length) throw new Error(`omx-contract at ${c.dir} has no ORACLE_FULL_RATES list`);
    oracleRates = v.map(Number);
  }
  return oracleRates;
}
/** Is this failure the accepted one: activate refused at a rate outside the nine? */
function accepted(test, details) {
  if (test !== ACCEPTED_TEST) return false;
  const m = REFUSAL.exec(String(details ?? '').replace(/\s+/g, ' ').trim());
  return !!m && !oracleFullRates().includes(Number(m[1]));
}

const [name, path] = process.argv.slice(2);
let report;
try {
  report = JSON.parse(readFileSync(path, 'utf8'));
} catch (e) {
  console.error(`FAIL ${name}: no readable report (${e.message})`);
  process.exit(1);
}
const counts = {};
const failures = [];
const acceptedFailures = [];
// report.results[]: {test: {<kind>: {test, ...}}, status: {code: "success"|"failed"|"crashed"|"skipped"|"warning", details}}
for (const r of Array.isArray(report.results) ? report.results : []) {
  const code = r?.status?.code;
  const [kind, t] = Object.entries(r?.test ?? {})[0] ?? ['?', {}];
  if (typeof code !== 'string') {
    console.error(`FAIL ${name}: a result has no status code`);
    process.exit(1);
  }
  counts[code] = (counts[code] ?? 0) + 1;
  if (code === 'failed' && accepted(`${kind}/${t.test ?? '?'}`, r.status.details)) acceptedFailures.push(`${kind}/${t.test}: ${String(r.status.details).replace(/\s+/g, ' ')}`);
  else if (code === 'failed' || code === 'crashed') failures.push(`${kind}/${t.test ?? '?'}: ${code}${r.status.details ? ': ' + String(r.status.details).replace(/\s+/g, ' ').slice(0, 160) : ''}`);
}
const total = Object.values(counts).reduce((a, b) => a + b, 0);
if (total === 0) {
  console.error(`FAIL ${name}: the report holds no test`);
  process.exit(1);
}
console.log(`${name}: ${Object.entries(counts).map(([k, v]) => `${v} ${k}`).join(', ')}`);
for (const f of acceptedFailures) console.log(`  accepted ${name} / ${f} (outside the nine oracle rates, Open-Mixer/omx-plugins#91)`);
for (const f of failures) console.log(`  FAIL ${name} / ${f}`);
process.exit(failures.length ? 1 : acceptedFailures.length ? 3 : 0);
