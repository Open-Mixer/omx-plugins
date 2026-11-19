// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * no-modgui-check.mjs — omx-plugins ships no generated MOD GUI (owner decision 2026-11-19, omx-panel#137 section B: generated
 * toolkit files are never shipped; a host generates them from the descriptions with omx-modgui-sync). Three rules, each read
 * by value:
 *   (a) no tracked file is a MOD GUI file: modgui.ttl, or anything under a modgui/ directory;
 *   (b) no generated manifest.ttl points at <modgui.ttl> (a dangling reference lilv would trip on);
 *   (c) nothing that builds or tests a bundle calls `omx-panel modgui` or modgui-test.sh: the plugin Makefile template, the
 *       root Makefile and the CI workflow, so no MOD GUI is written into build/*.lv2 and none is held fresh.
 * `--self-test` plants each violation and reads the check go red.
 * usage: no-modgui-check.mjs [--self-test]
 */
import { execFileSync } from 'node:child_process';
import { readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const CALLS = [/\$\(OMX_PANEL\)\s+modgui\b/, /\bomx-panel\s+modgui\b/, /modgui-test\.sh/, /^\s*make\s+modgui\b/m];

/** The refusals, one line each, for `files` (tracked paths), `manifests` ({path, text}) and `texts` ({path, text} of the build files). */
export function refusals({ files, manifests, texts }) {
  const out = [];
  for (const f of files) if (/(^|\/)modgui\.ttl$/.test(f) || /(^|\/)modgui\//.test(f)) out.push(`${f}: a tracked MOD GUI file`);
  for (const m of manifests) if (/<modgui\.ttl>/.test(m.text)) out.push(`${m.path}: points at <modgui.ttl>`);
  for (const t of texts) for (const re of CALLS) if (re.test(t.text)) out.push(`${t.path}: builds or checks a MOD GUI (${re})`);
  return out;
}

const tracked = () => execFileSync('git', ['ls-files', '-z'], { cwd: ROOT, encoding: 'utf8', maxBuffer: 1 << 26 }).split('\0').filter(Boolean);
const read = (p) => readFileSync(join(ROOT, p), 'utf8');
const BUILD_FILES = ['recipes/templates/plugin/Makefile.tmpl', 'Makefile', '.github/workflows/ci.yml'];

function tree() {
  const files = tracked();
  return {
    files,
    manifests: files.filter((f) => /(^|\/)generated\/[^/]+\.lv2\/manifest\.ttl$/.test(f)).map((path) => ({ path, text: read(path) })),
    texts: BUILD_FILES.map((path) => ({ path, text: read(path) })),
  };
}

const found = refusals(tree());
for (const r of found) console.error(`no-modgui: ${r}`);
let bad = found.length > 0;

if (process.argv.includes('--self-test')) {
  const clean = { files: ['plugins/omx-drive/generated/omx_drive_params.h'], manifests: [{ path: 'm.ttl', text: 'rdfs:seeAlso <omx-drive.ttl> .' }],
    texts: [{ path: 'Makefile', text: 'all:\n\t$(CC) x.c\n' }] };
  const arms = {
    'a tracked modgui.ttl': { ...clean, files: [...clean.files, 'plugins/omx-drive/generated/omx-drive.lv2/modgui.ttl'] },
    'a tracked file under modgui/': { ...clean, files: [...clean.files, 'plugins/omx-drive/generated/omx-drive.lv2/modgui/icon-omx-drive.html'] },
    'a manifest naming <modgui.ttl>': { ...clean, manifests: [{ path: 'm.ttl', text: 'rdfs:seeAlso <modgui.ttl> , <omx-drive.ttl> .' }] },
    'a template writing films into the bundle': { ...clean, texts: [{ path: 'T', text: '\t$(OMX_PANEL) modgui --films-only --descriptor d --out $(BUNDLE)\n' }] },
    'a make modgui step': { ...clean, texts: [{ path: 'ci.yml', text: '          make modgui\n' }] },
    'a modgui-test.sh call': { ...clean, texts: [{ path: 'T', text: '\tsh $(OMX_PANEL_DIR)/tools/modgui-test.sh $(BUNDLE)\n' }] },
  };
  if (refusals(clean).length > 0) { console.error('no-modgui: self-test: the clean fixture is refused'); bad = true; }
  for (const [name, fixture] of Object.entries(arms)) {
    if (refusals(fixture).length === 0) { console.error(`no-modgui: SABOTAGE ${name}: STAYED GREEN`); bad = true; } else console.log(`SABOTAGE ${name}: red`);
  }
}
if (bad) process.exit(1);
console.log('no-modgui: no tracked MOD GUI file, no manifest pointing at one, no build step that writes or checks one');
