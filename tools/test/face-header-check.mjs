#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * tools/test/face-header-check.mjs — include/omx_plugin_face_ids.h is what contract/plugin-face.json renders: the item
 * is this package's since 0.19.3 (omx-contract declared it from 3.9.0 to 3.12.x), so a header left behind after an
 * edit of the file would put the faces at ids the installed item does not name.
 *
 *   node tools/test/face-header-check.mjs              the committed header against the file: each field's
 *                                                      OMX_PLUGIN_FACE_<FIELD> define carries the file's value, and no
 *                                                      other define; with the omx-contract command (OMX_CONTRACT_CLI, its
 *                                                      bin/omx-contract.mjs or an executable, else `omx-contract` on PATH),
 *                                                      also `render --package omx-plugins --check`, byte for byte
 *   node tools/test/face-header-check.mjs --self-test  a changed value, a missing define and an extra define each refuse
 * Exit: 0 pass, 1 refused.
 */
import { execFileSync } from 'node:child_process';
import { cpSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { FACE_IDS_FILE, FACE_IDS_ITEM } from '../param-contract.mjs';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const HEADER = 'omx_plugin_face_ids.h';
const PREFIX = 'OMX_PLUGIN_FACE_';
const snake = (k) => k.replace(/([a-z0-9])([A-Z])/g, '$1_$2').toUpperCase();

/** The refusals of `headerText` against the item of `file`, one line each. */
export function refusals(headerText, file) {
  const value = JSON.parse(readFileSync(file, 'utf8'))[FACE_IDS_ITEM]?.value ?? {};
  const want = new Map(Object.entries(value).map(([k, v]) => [PREFIX + snake(k), v]));
  const got = new Map([...headerText.matchAll(/^#define (OMX_PLUGIN_FACE_\w+) (\d+)u\b/gm)].map((m) => [m[1], Number(m[2])]));
  const out = [];
  for (const [name, v] of want) {
    if (!got.has(name)) out.push(`${HEADER}: ${name} is not defined (contract/${FACE_IDS_FILE} declares ${v})`);
    else if (got.get(name) !== v) out.push(`${HEADER}: ${name} is ${got.get(name)}, contract/${FACE_IDS_FILE} says ${v}`);
  }
  for (const name of got.keys()) if (!want.has(name) && !name.endsWith('_IDS_H')) out.push(`${HEADER}: ${name} is no field of ${FACE_IDS_ITEM}`);
  return out;
}

function cli() {
  const c = process.env.OMX_CONTRACT_CLI;
  if (c) return c.endsWith('.mjs') ? ['node', c] : [c];
  try { execFileSync('sh', ['-c', 'command -v omx-contract'], { stdio: 'ignore' }); return ['omx-contract']; } catch { return null; }
}

const file = join(ROOT, 'contract', FACE_IDS_FILE);
if (process.argv[2] === '--self-test') {
  const good = readFileSync(join(ROOT, 'include', HEADER), 'utf8');
  const arms = {
    changed: good.replace(/(OMX_PLUGIN_FACE_BYPASS_CLAP_ID )\d+u/, '$12147418113u'),
    missing: good.replace(/^#define OMX_PLUGIN_FACE_LATENCY_LV2_INDEX .*\n/m, ''),
    extra: good.replace(/^(#define OMX_PLUGIN_FACE_LATENCY_LV2_INDEX .*\n)/m, '$1#define OMX_PLUGIN_FACE_SIDECHAIN_LV2_INDEX 2u\n'),
  };
  let fail = refusals(good, file).length !== 0;
  if (fail) console.log('self-test: the committed header is not green');
  for (const [arm, text] of Object.entries(arms)) {
    if (text === good) { console.log(`SABOTAGE ${arm}: the sabotage changed nothing`); fail = true; continue; }
    const r = refusals(text, file);
    if (r.length === 0) { console.log(`SABOTAGE ${arm}: STAYED GREEN`); fail = true; } else console.log(`SABOTAGE ${arm}: red (${r[0]})`);
  }
  const c = cli();
  if (c) {
    const scratch = mkdtempSync(join(tmpdir(), 'face-header-'));
    try {
      cpSync(join(ROOT, 'include', HEADER), join(scratch, HEADER));
      writeFileSync(join(scratch, HEADER), arms.changed);
      try {
        execFileSync(c[0], [...c.slice(1), 'render', '--target', 'c', '--package', 'omx-plugins', '--out', scratch, '--data', join(ROOT, 'contract'), '--check'], { stdio: 'pipe' });
        console.log('SABOTAGE render: STAYED GREEN'); fail = true;
      } catch { console.log('SABOTAGE render: red'); }
    } finally { rmSync(scratch, { recursive: true, force: true }); }
  }
  if (fail) process.exit(1);
  console.log('face-header self-test: every sabotage red');
  process.exit(0);
}

const r = refusals(readFileSync(join(ROOT, 'include', HEADER), 'utf8'), file);
const c = cli();
if (c) {
  try {
    execFileSync(c[0], [...c.slice(1), 'render', '--target', 'c', '--package', 'omx-plugins', '--out', join(ROOT, 'include'), '--data', join(ROOT, 'contract'), '--check'], { stdio: 'pipe' });
  } catch (e) { r.push(`render --package omx-plugins --check: ${String(e.stdout || e.stderr || e.message).trim().split('\n')[0]}`); }
} else console.log(`face-header: no omx-contract command: the defines are checked, the render byte for byte is not`);
for (const line of r) console.log(line);
if (r.length) process.exit(1);
console.log(`ok   include/${HEADER} is contract/${FACE_IDS_FILE}'s render`);
