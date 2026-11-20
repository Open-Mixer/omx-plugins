#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * oracle-rates-check.mjs — the nine oracle rates (ORACLE_FULL_RATES) are omx-dsp's (sorting row V25, RULED: RT_HARD_TARGET
 * and ORACLE_* go to omx-dsp; omx-contract keeps STANDARD_SAMPLE_RATES and STANDARD_QUANTA): no file of this tree reads
 * them from omx-contract. Held here: a C file that uses OMX_ORACLE_FULL_RATES* includes omx-dsp's
 * <omxdsp/omxdsp_definitions.h> and not <omxcontract/omx_contract_limits.h> for them; a tool reads the list as omx-dsp's
 * item (tools/clap-validate-summary.mjs), refused when another package holds it; no comment names omx-contract as
 * their owner. `make oracle-rates`, part of `make test`.
 */
import { readdirSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const self = fileURLToPath(import.meta.url);
const walk = (d) => readdirSync(d, { withFileTypes: true }).flatMap((e) => (e.isDirectory() ? (e.name === 'generated' || e.name === 'build' ? [] : walk(join(d, e.name))) : [join(d, e.name)]));
const files = ['tools', 'plugins', 'lib', 'include'].flatMap((d) => walk(join(root, d))).filter((f) => f !== self && /\.(c|h|mjs)$/.test(f));
const errors = [];
for (const f of files) {
  const t = readFileSync(f, 'utf8');
  const rel = f.slice(root.length + 1);
  if (/\.(c|h)$/.test(f) && /OMX_ORACLE_FULL_RATES/.test(t)) {
    if (/#include <omxcontract\/omx_contract_limits\.h>/.test(t)) errors.push(`${rel}: uses OMX_ORACLE_FULL_RATES and includes <omxcontract/omx_contract_limits.h>; the rates are omx-dsp's <omxdsp/omxdsp_definitions.h>`);
  }
  if (/oracle rates, omx-contract|omx-contract's ORACLE_FULL_RATES|omx-contract at .* has no ORACLE_FULL_RATES|no omx-contract to read ORACLE_FULL_RATES/.test(t)) errors.push(`${rel}: names omx-contract as the owner of ORACLE_FULL_RATES`);
  if (/\.mjs$/.test(f) && /ORACLE_FULL_RATES/.test(t) && !/ORACLE_OWNER|'omx-dsp'/.test(t)) errors.push(`${rel}: reads ORACLE_FULL_RATES without holding it to omx-dsp's package`);
}
if (errors.length) {
  for (const e of errors) console.error(`oracle-rates: ${e}`);
  process.exit(1);
}
console.log(`oracle-rates: ${files.length} files read; ORACLE_FULL_RATES is omx-dsp's wherever it is read`);
