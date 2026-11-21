// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * plugin-gui-gate.mjs — the CI gate of omx-plugins#120 (docs/plugin-guis.md, "The CI gate"): every shipped plugin of the tree
 * has a GUI, so a plugin added without one is red the moment it exists. Layer 1, read from the generated files by value:
 *   (a) the bundle's manifest.ttl declares a ui:X11UI on the plugin's URI with a ui:binary (the LV2 UI);
 *   (b) the plugin's own TTL names that UI with ui:ui;
 *   (c) generated/<kernel>_params.h carries OMX_<K>_PANEL_LAYOUT_JSON, the layout the window draws, as
 *       a JSON object (the plugin's own description, which the window draws).
 * The built face's `clap.gui` and the window opening are layers 2 and 3 (tools/gui-probe.c).
 * `--self-test` runs the rules over a clean plugin and plants each violation: each must go red and name the plugin.
 * usage: plugin-gui-gate.mjs [--self-test | --arms-only]  (--arms-only: the arms alone, no tree, no toolchain)
 */
import { existsSync, readFileSync } from 'node:fs';
import { basename, dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const UI_TYPE = /\ba\s+ui:X11UI\b/;

/** The refusals for one plugin: `p` is {stem, uri, kernel, manifest, ttl, header} (text or undefined when the file is absent). */
export function refusals(p) {
  const out = [];
  const no = (why) => out.push(`${p.stem}: ${why}`);
  if (p.manifest === undefined) no('no generated manifest.ttl');
  else {
    const uiBlock = p.manifest.split(/\n\s*\n/).find((b) => UI_TYPE.test(b));
    if (!uiBlock) no('the manifest declares no ui:X11UI');
    else if (!/\bui:binary\s+<[^>]+>/.test(uiBlock)) no('its ui:X11UI has no ui:binary');
    else if (!/@prefix\s+ui:/.test(p.manifest)) no('the manifest has no ui: prefix');
  }
  if (p.ttl === undefined) no('no generated plugin TTL');
  else if (!/\bui:ui\s+</.test(p.ttl)) no('the plugin TTL names no ui:ui');
  const key = `OMX_${p.kernel.toUpperCase()}_PANEL_LAYOUT_JSON`;
  if (p.header === undefined) no('no generated parameter header');
  else {
    const m = new RegExp(`#define\\s+${key}\\s+(".*")\\s*$`, 'm').exec(p.header);
    if (!m) no(`the parameter header defines no ${key}`);
    else {
      try { if (typeof JSON.parse(JSON.parse(m[1])) !== 'object') throw new Error('not an object'); } catch { no(`${key} is not a string of a JSON object`); }
    }
  }
  return out;
}

async function tree() {
  const { loadDecl, pluginDirs } = await import('../gen.mjs');
  return pluginDirs(ROOT).map((dir) => {
    const d = loadDecl(dir);
    const gen = join(dir, 'generated');
    const stem = basename(dir);
    const rd = (f) => (existsSync(f) ? readFileSync(f, 'utf8') : undefined);
    return { stem, kernel: d.kernel, uri: d.lv2?.uri, manifest: rd(join(gen, `${stem}.lv2`, 'manifest.ttl')), ttl: rd(join(gen, `${stem}.lv2`, `${stem}.ttl`)),
      header: rd(join(gen, `omx_${d.kernel}_params.h`)) };
  });
}

let bad = 0;
if (!process.argv.includes('--arms-only')) {
  const all = await tree();
  if (all.length === 0) { console.error('plugin-gui-gate: the tree has no plugin to hold'); bad = 1; }
  for (const p of all) for (const r of refusals(p)) { console.error(`plugin-gui-gate: ${r}`); bad = 1; }
  if (!bad) console.log(`plugin-gui-gate: ${all.length} plugins, each declares its UI and carries its layout`);
}

if (process.argv.includes('--self-test') || process.argv.includes('--arms-only')) {
  const layout = JSON.stringify(JSON.stringify({ family: 'dynamics', sections: [] }));
  const clean = { stem: 'omx-x', kernel: 'x', uri: 'urn:openmixer:x',
    manifest: '@prefix ui: <http://lv2plug.in/ns/extensions/ui#> .\n\n<urn:openmixer:x>\n a lv2:Plugin .\n\n<urn:openmixer:x#ui>\n a ui:X11UI ;\n ui:binary <omx-x-ui.so> .\n',
    ttl: '<urn:openmixer:x> ui:ui <urn:openmixer:x#ui> .\n', header: `#define OMX_X_PANEL_LAYOUT_JSON ${layout}\n` };
  const arms = {
    'no UI in the manifest': { ...clean, manifest: clean.manifest.replace('ui:X11UI', 'ui:GtkUI') },
    'a UI with no binary': { ...clean, manifest: clean.manifest.replace(/ ;\n ui:binary <[^>]+>/, '') },
    'the plugin TTL does not name it': { ...clean, ttl: '<urn:openmixer:x> a lv2:Plugin .\n' },
    'no layout in the header': { ...clean, header: '/* nothing */\n' },
    'a layout that is no JSON': { ...clean, header: '#define OMX_X_PANEL_LAYOUT_JSON "not json"\n' },
    'a missing bundle': { ...clean, manifest: undefined, ttl: undefined },
  };
  const ok = refusals(clean).length === 0;
  console.log(`${ok ? 'PASS' : 'FAIL'} plugin-gui-gate self-test: the clean plugin passes`);
  if (!ok) { console.error(refusals(clean).join('\n')); bad = 1; }
  for (const [what, p] of Object.entries(arms)) {
    const r = refusals(p);
    const red = r.length > 0 && r.every((l) => l.startsWith('omx-x:'));
    console.log(`${red ? 'PASS' : 'FAIL'} plugin-gui-gate self-test: ${what} goes red and names the plugin`);
    if (!red) bad = 1;
  }
}
process.exit(bad ? 1 : 0);
