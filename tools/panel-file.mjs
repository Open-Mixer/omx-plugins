#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * panel-file.mjs — what this repository hands omx-panel for a plugin's MOD GUI: the descriptor (the declaration
 * rendered to one by tools/descriptor.mjs, which the plugin folder's <stem>.descriptor.json keeps equal) and the
 * declared panel as a file, which `omx-panel modgui --panel` reads. omx-panel writes the MOD GUI; this file only
 * names its inputs, its output (the LV2 bundle under generated/) and the source its banner names.
 *
 * Usage:
 *   node tools/panel-file.mjs <plugin dir> <out file>
 *       writes the declared panel of the plugin as JSON, for a Makefile that calls `omx-panel modgui` itself
 *       (plugins/<stem>/Makefile, `make gen` and `make check`);
 *   node tools/panel-file.mjs --modgui [--check | --emit-stdout] <plugin dir>
 *       runs `omx-panel modgui` over the plugin, as the Makefile does (the wizard and the completeness test).
 */
import { mkdirSync, mkdtempSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { descriptorFile, descriptorText } from './descriptor.mjs';
import { loadDecl } from './gen.mjs';
import { omxPanel } from './omx-panel.mjs';
import { isMain } from './omx-contract.mjs';

/** The declared panel of `d`, as the file omx-panel reads. */
export const panelFileText = (d) => `${JSON.stringify(d.panel, null, 2)}\n`;

/** The text the MOD GUI files' banner names as what they were made from. */
export const modguiSource = (d) => `plugins/${d.stem}/${d.stem}.decl.json (its panel block) and tools/look.json`;

/** The LV2 bundle directory a declaration's MOD GUI is written into. */
export const bundleDir = (d) => join(d.dir, 'generated', `${d.stem}.lv2`);

/**
 * `omx-panel modgui` over declaration `d`: `mode` is '' (write), '--check' or '--emit-stdout'.
 * `{ status, stdout, stderr }`.
 */
export function modgui(d, mode = '') {
  if (!d.panel) throw new Error(`plugins/${d.stem}: declares no panel`);
  const tmp = mkdtempSync(join(tmpdir(), 'omx-panel-'));
  try {
    const panel = join(tmp, `${d.stem}.panel.json`);
    writeFileSync(panel, panelFileText(d));
    const descriptor = join(tmp, descriptorFile(d.stem));
    writeFileSync(descriptor, descriptorText(d));
    // the committed bundle carries no film strip: the build writes it (Makefile.tmpl)
    return omxPanel(['modgui', ...(mode ? [mode] : []), '--no-films', '--descriptor', descriptor, '--panel', panel, '--out', bundleDir(d), '--source', modguiSource(d)]);
  } finally {
    rmSync(tmp, { recursive: true, force: true });
  }
}

function main(argv) {
  if (argv[0] === '--modgui') {
    const mode = argv.find((a) => a === '--check' || a === '--emit-stdout');
    const dir = argv.slice(1).find((a) => !a.startsWith('--'));
    if (!dir) {
      console.error('usage: panel-file.mjs --modgui [--check | --emit-stdout] <plugin dir>');
      process.exit(2);
    }
    const r = modgui(loadDecl(dir), mode);
    process.stdout.write(r.stdout);
    process.stderr.write(r.stderr);
    process.exit(r.status);
  }
  const [dir, out] = argv;
  if (!dir || !out) {
    console.error('usage: panel-file.mjs <plugin dir> <out file>');
    process.exit(2);
  }
  const d = loadDecl(dir);
  if (!d.panel) {
    console.error(`panel-file: plugins/${d.stem} declares no panel`);
    process.exit(1);
  }
  mkdirSync(dirname(resolve(out)), { recursive: true });
  writeFileSync(out, panelFileText(d));
}

if (isMain(import.meta.url)) main(process.argv.slice(2));
