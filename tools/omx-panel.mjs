// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx-panel.mjs — where the INSTALLED omx-panel is found, and what this repository reads from it. The panel
 * is omx-panel's: it lays a panel out from a descriptor and writes a plugin's MOD GUI from a descriptor and a
 * declared panel, and it reads a descriptor, never a declaration. This repository renders a declaration to its
 * descriptor (tools/descriptor.mjs) and hands omx-panel that descriptor and the declared panel as a file
 * (tools/panel-file.mjs). Which releases are acceptable is said once, in the packaging: packaging/omx-plugins.spec's
 * BuildRequires on omx-panel, the same floor debian/control writes.
 *
 * The tree is $OMX_PANEL_DIR, else /usr/share/omx-panel, where the omx-panel package installs it (`tools/`,
 * `schema/`, `bin/omx-panel.mjs`).
 *
 * omx-panel's vocabulary reads the unit values of the param-contract header, which this repository owns: the
 * header it reads is this tree's include/omx_param_contract.h, never an installed omx-plugins-devel of another
 * release, unless $OMX_PARAM_CONTRACT_H names one.
 *
 * The same holds for the meaning each declared role plays, which decides whether a family stands (dynamics needs a
 * control meaning threshold or ratio): omx-panel reads the role meanings of every installed package, and ours are this
 * tree's contract/role-meanings.json, never an installed omx-plugins-contract of another release (or none, as on a
 * build box, where every meaning read none and omx-comp's family fell to none). $OMX_ROLE_MEANINGS is set to our file
 * and every OTHER installed package's files, unless it names its own list (tools/test/role-meanings-source-check.mjs).
 */
import { spawnSync } from 'node:child_process';
import { existsSync, readFileSync, readdirSync } from 'node:fs';
import { join } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

export const PANEL_DIR = process.env.OMX_PANEL_DIR || '/usr/share/omx-panel';
const at = (...p) => join(PANEL_DIR, ...p);
if (!existsSync(at('bin', 'omx-panel.mjs'))) {
  throw new Error(`omx-panel is not installed at ${PANEL_DIR}: install omx-panel from the channel, or set OMX_PANEL_DIR to its tree (BUILDING.md)`);
}
process.env.OMX_PARAM_CONTRACT_H ||= fileURLToPath(new URL('../include/omx_param_contract.h', import.meta.url));
// the roles' meanings are this tree's own (omx-panel >= 0.13.5 reads them from an installed omx-plugins-contract, which a
// build of omx-plugins has not installed yet)
process.env.OMX_ROLE_MEANINGS ||= fileURLToPath(new URL('../contract/role-meanings.json', import.meta.url));

/** omx-panel's layout module: a descriptor, of any origin, laid out under a declared panel (its docs/panel-layout.md). */
const layout = await import(pathToFileURL(at('tools', 'panel-description.mjs')).href);
export const { DISPLAY_KINDS, DISPLAY_SOURCES, FAMILIES, INDEXED_ROLES, PLACEMENTS, ROLES, WIDGETS, isVisible, pagesOf, panelCanonical, panelFingerprint, panelLayout, servedWidget, unitToken, widgetOf } = layout;

/** omx-panel's reader of the installed vocabulary: the meanings, and the meaning each panel role plays. */
const vocabulary = await import(pathToFileURL(at('tools', 'vocabulary.mjs')).href);
export const { meaningOfRole } = vocabulary;

/** The role-meaning sources: this tree's contract/role-meanings.json, then the JSON files of every installed package
 * but omx-plugins (the first contract directory holding a package wins, as omx-panel reads it). */
export function roleMeaningFiles() {
  const files = [fileURLToPath(new URL('../contract/role-meanings.json', import.meta.url))];
  const seen = new Set(['omx-plugins']);
  for (const dir of vocabulary.contractDirs()) {
    const major = join(dir, '3');
    if (!existsSync(major)) continue;
    for (const pkg of readdirSync(major, { withFileTypes: true })) {
      if (!pkg.isDirectory() || seen.has(pkg.name)) continue;
      seen.add(pkg.name);
      for (const f of readdirSync(join(major, pkg.name))) if (f.endsWith('.json')) files.push(join(major, pkg.name, f));
    }
  }
  return files;
}
process.env.OMX_ROLE_MEANINGS ||= roleMeaningFiles().join(':');

/** omx-panel's declared lists (schema/panel.schema.json): the widgets a control is drawn with and the roles that repeat by
 * index. A plugin's declaration schema references its `$defs` by this schema's `$id`. */
export const PANEL_SCHEMA = JSON.parse(readFileSync(at('schema', 'panel.schema.json'), 'utf8'));

/** Runs `omx-panel <args>`: `{ status, stdout, stderr }`. */
export function omxPanel(args, options = {}) {
  const r = spawnSync(process.execPath, [at('bin', 'omx-panel.mjs'), ...args], { encoding: 'utf8', ...options });
  return { status: r.status ?? 1, stdout: r.stdout ?? '', stderr: r.stderr ?? '' };
}
