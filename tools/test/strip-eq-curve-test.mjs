#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * strip-eq-curve-test.mjs — omx-strip's EQ screen draws the EQ curve with the HPF's and LPF's roll-off in it. The
 * declaration gives the EQ section its `eq-curve` graph and binds the band roles and the `hpf_*` / `lpf_*` roles;
 * omx-panel's MOD GUI generator draws the response of those controls, through the omx-dsp twin it loads.
 *
 *   1. the omx-dsp twin omx-panel draws the curve with is installed (this test FAILS, never skips, without it);
 *   2. the generated MOD GUI holds the EQ curve, a polyline, in the EQ section;
 *   3. with the high-pass on at 400 Hz the curve's low end lies below its middle (the HPF's roll-off is in it), and
 *      with the low-pass on at 2 kHz its high end does too;
 *   4. with both filters off the curve is flat.
 *
 * `make strip-eq-curve`, part of `make test`. It loads the declaration, so it needs omx-contract, omx-dsp and an omx-panel
 * with edge bands and the curve's filter roll-off (OMX_PANEL_DIR, the omx-panel branch of #58 or later).
 */
import { existsSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { descriptorText } from '../descriptor.mjs';
import { loadDecl } from '../gen.mjs';
import { omxPanel, PANEL_DIR } from '../omx-panel.mjs';
import { panelFileText } from '../panel-file.mjs';

const ROOT = join(fileURLToPath(new URL('.', import.meta.url)), '..', '..');
let bad = 0, n = 0;
function check(what, ok, detail = '') {
  n++;
  if (!ok) bad++;
  console.log(`${ok ? 'PASS' : 'FAIL'} strip-eq-curve ${what}${ok ? '' : `: ${detail}`}`);
}

// 1. the twin, in the omx-panel tree's own resolution: a missing twin is a red, not a skip
const twinFile = join(PANEL_DIR, 'node_modules', '@openmixer', 'omx-dsp', 'package.json');
const twin = existsSync(twinFile);
check('the omx-dsp twin omx-panel draws the curve with is installed', twin, `${twinFile} is absent: install or build the twin in the omx-panel tree`);

const d = loadDecl(join(ROOT, 'plugins', 'omx-strip'));
const descriptor = JSON.parse(descriptorText(d));
const tmp = mkdtempSync(join(tmpdir(), 'strip-eq-curve-'));
/** The EQ curve's points, `[[x, y], ...]`, of the MOD GUI generated with the filters at `on`, or null when none is drawn. */
function curve(on) {
  const dd = structuredClone(descriptor);
  const set = (symbol, v) => { const c = dd.controls.find((x) => x.symbol === symbol); if (c.kind === 'travel') c.travel.default = v; else c.default = v; };
  set('hpfOn', on.hpf); set('hpfFreq', 400); set('lpfOn', on.lpf); set('lpfFreq', 2000);
  const out = join(tmp, `b${Number(on.hpf)}${Number(on.lpf)}`, 'omx-strip.lv2');
  writeFileSync(join(tmp, 'd.json'), `${JSON.stringify(dd)}\n`);
  writeFileSync(join(tmp, 'p.json'), panelFileText(d));
  const r = omxPanel(['modgui', '--no-films', '--descriptor', join(tmp, 'd.json'), '--panel', join(tmp, 'p.json'), '--out', out, '--source', 'strip-eq-curve-test']);
  if (r.status !== 0) throw new Error(`omx-panel modgui: ${r.stderr}`);
  const html = readFileSync(join(out, 'modgui', 'icon-omx-strip.html'), 'utf8');
  const m = html.match(/<polyline class="omx-curve-line" fill="none" points="([^"]+)"/);
  return m ? m[1].split(' ').map((p) => p.split(',').map(Number)) : null;
}
try {
  const both = twin ? curve({ hpf: true, lpf: true }) : null;
  const none = twin ? curve({ hpf: false, lpf: false }) : null;
  check('the generated MOD GUI holds the EQ curve', both !== null, 'no omx-curve-line polyline in the strip\'s MOD GUI');
  if (both) {
    const y = (i) => both[i][1];
    const q = Math.floor(both.length / 8), mid = Math.floor(both.length / 2);
    check('the high-pass roll-off is in the curve: its low end lies below the middle (larger y)', y(q) > y(mid) + 1, `y at the first eighth ${y(q)}, at the middle ${y(mid)}`);
    check('the low-pass roll-off is in the curve: its high end lies below the middle (larger y)', y(both.length - 1 - q) > y(mid) + 1, `y at the last eighth ${y(both.length - 1 - q)}, at the middle ${y(mid)}`);
  } else {
    check('the high-pass roll-off is in the curve', false, 'no curve');
    check('the low-pass roll-off is in the curve', false, 'no curve');
  }
  check('with both filters off the curve is flat', none !== null && none.every((p) => p[1] === none[0][1]), 'no curve, or a curve that is not flat');
} finally {
  rmSync(tmp, { recursive: true, force: true });
}
console.log(`strip-eq-curve: ${n - bad}/${n} passed`);
process.exit(bad ? 1 : 0);
