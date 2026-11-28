#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * control-layout-check.mjs: every built CLAP face serves org.openmixer.control-layout.draft/1, the cut-down shape
 * OpenMixer would propose to CLAP, and a host draws all of it. Each face is read by omx-clap-scan --json, whose reader
 * (omx-clap-core's src/scan_layout.c) applies the host's rules against what the plugin offers (clap.params, clap.meters/1,
 * the curve and gain-adjustment extensions) and names every entry it would leave out. Held, for every plugin:
 *   - the layout is served, and the scan left nothing out;
 *   - it is the panel description cut down, compared here with the scan's reading of org.openmixer.panel-description/1:
 *     section i has id i and the section's label as its name; the controls are the panel's in order, without label
 *     keys; the displays are the panel's that read clap.meters/1, in order, with the same meter ids.
 * And across the faces at least one meter display is served, so the display rule is exercised.
 *
 *   control-layout-check.mjs <omx-clap-scan> <face.clap>...
 *
 * Exit 0 held; 1 a face that breaks it, each failure printed; 2 usage.
 */
import { execFileSync } from 'node:child_process';
import { isDeepStrictEqual } from 'node:util';

const [scan, ...faces] = process.argv.slice(2);
if (!scan || faces.length === 0) {
  process.stderr.write('usage: control-layout-check.mjs <omx-clap-scan> <face.clap>...\n');
  process.exit(2);
}

let bad = 0;
let meterDisplays = 0;
const fail = (what) => { console.log(`FAIL control-layout ${what}`); bad = 1; };

for (const face of faces) {
  let doc;
  try {
    doc = JSON.parse(execFileSync(scan, ['--json', face], { encoding: 'utf8', maxBuffer: 64 << 20 }));
  } catch (e) {
    fail(`${face}: omx-clap-scan --json did not read it: ${e.message.split('\n')[0]}`);
    continue;
  }
  for (const p of doc.files.flatMap((f) => f.plugins ?? [])) {
    const name = `${face}#${p.id}`;
    const lay = p.controlLayout, pan = p.panel;
    if (!lay || !lay.leftOut) { fail(`${name}: no control layout served`); continue; }
    if (!pan || !Array.isArray(pan.sections)) { fail(`${name}: no panel description to compare with`); continue; }
    if (lay.leftOut.length) fail(`${name}: the host leaves out ${JSON.stringify(lay.leftOut)}`);
    const sections = pan.sections.map((s, i) => ({ id: i, name: s.label }));
    const controls = pan.controls.map((c) => ({ paramId: c.paramId, sectionId: c.section, widget: c.widget, label: c.label,
      visibleIfParamId: c.visibleParamId, visibleMask: c.visibleMask }));
    const displays = pan.displays.filter((d) => d.source === 'meters')
      .map((d) => ({ sectionId: d.section, source: 'meter', ids: d.meterIds, label: d.label }));
    if (!isDeepStrictEqual(lay.sections, sections)) fail(`${name}: sections ${JSON.stringify(lay.sections)}, the panel's ${JSON.stringify(sections)}`);
    if (!isDeepStrictEqual(lay.controls, controls)) fail(`${name}: controls differ from the panel's`);
    if (!isDeepStrictEqual(lay.displays, displays)) fail(`${name}: displays ${JSON.stringify(lay.displays)}, the panel's meter displays ${JSON.stringify(displays)}`);
    meterDisplays += Array.isArray(lay.displays) ? lay.displays.length : 0;
    if (!bad) console.log(`ok   control-layout ${name}: ${sections.length} sections, ${controls.length} controls, ${displays.length} meter displays, none left out`);
  }
}
if (meterDisplays === 0) fail('no face serves a meter display: the display rule is not exercised');
process.exit(bad);
