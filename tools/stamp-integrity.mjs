#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * tools/stamp-integrity.mjs <descriptor.json>... --commit <40-hex> — stamp each descriptor, in place, with the source
 * it was built from (#32): integrity { version, the plugin's own (plugin.version), commit, the commit the build was
 * made from }. A host that links the plugin table matches the descriptor to the build it links by the commit, as it
 * matches the plugins' RT verdicts. `make install-contract` runs it on the copies it installs; the committed
 * descriptors carry no stamp (a commit cannot name itself). It names no binary: the descriptor package serves every
 * architecture.
 *
 * Refused (exit 2, nothing written): no commit, a commit that is not 40 lowercase hex digits, a descriptor whose plugin
 * states no version. Placed where omx-contract's schema orders it, after meters and before extensions. It writes no
 * floor: a descriptor states none (the reader that installs it is the one its package was built against).
 */
import { readFileSync, writeFileSync } from 'node:fs';

const fail = (m) => { console.error(`stamp-integrity: ${m}`); process.exit(2); };
const args = process.argv.slice(2);
const at = args.indexOf('--commit');
const commit = at < 0 ? undefined : args[at + 1];
const files = args.filter((_, i) => i !== at && i !== at + 1);
if (!commit) fail('no --commit: the source commit the build was made from (SOURCE_COMMIT, else the tree\'s HEAD)');
if (!/^[0-9a-f]{40}$/.test(commit)) fail(`--commit ${JSON.stringify(commit)} is not a full commit (40 lowercase hex digits)`);
if (!files.length) fail('no descriptor named');

const ORDER = ['$schema', 'requires', 'origin', 'plugin', 'controls', 'ports', 'instrument', 'needs', 'latency', 'easy', 'meters', 'integrity', 'extensions', 'source'];
const stamped = files.map((f) => {
  const d = JSON.parse(readFileSync(f, 'utf8'));
  const version = d.plugin?.version;
  if (typeof version !== 'string') fail(`${f}: the plugin states no version to stamp`);
  const withStamp = { ...d, integrity: { version, commit } };
  const keys = [...ORDER.filter((k) => k in withStamp), ...Object.keys(withStamp).filter((k) => !ORDER.includes(k))];
  return [f, Object.fromEntries(keys.map((k) => [k, withStamp[k]]))];
});
for (const [f, d] of stamped) writeFileSync(f, `${JSON.stringify(d, null, 2)}\n`);
