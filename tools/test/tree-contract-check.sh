#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tree-contract-check.sh — the build reads THIS tree's omx-plugins contract, never the installed omx-plugins-contract.
# A package build root pulls in an older released omx-plugins-contract (a BuildRequires chain), and an omx-contract CLI
# reads the whole installed contract directory and refuses that old file ($schema in the old address form). The generators
# reach the CLI through tools/omx-contract.mjs (locateContract: the scale words, the descriptor and panel views, ...): this runs that
# path with the installed contract directory a FAKE one holding a stale omx-plugins package, against a fake omx-contract
# CLI that refuses, as the real one does, any plugin-face.json in the directory it is handed whose $schema is the old
# form. Reached through `--installed` the fake dir is refused; handed the composed directory (this tree's package) it reads.
# Needs omx-contract's install-contract-dir.sh (omx-contract-devel).
# Exit 0 pass, 1 refused, 2 no omx-contract installed.
set -eu
here=$(cd "$(dirname "$0")/../.." && pwd)
real=${PKG_CONFIG:-pkg-config}
realdata=$($real --variable=datadir omx-contract 2>/dev/null || true)
[ -n "$realdata" ] && [ -f "$realdata/install-contract-dir.sh" ] || { echo "tree-contract: no omx-contract-devel installed" >&2; exit 2; }
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
mkdir -p "$tmp/data" "$tmp/installed/3/omx-plugins" "$tmp/installed/3/other-owner"
ln -s "$realdata/install-contract-dir.sh" "$tmp/data/install-contract-dir.sh"
# the installed render holds only the shared facts: no kernels, which is what asks for the union (a version inside the
# packaging's range: the fake stands for the install, whichever release the host has)
printf '{"version":"%s","kernels":{},"items":{}}\n' 3.99.0 > "$tmp/data/omx-contract.json"
# the stale package: the old address form, as an older release installed it
printf '{\n  "%s": "%s"\n}\n' "\$schema" 'https://github.com/FreeMixer/omx-contract/raw/v3/schema/omx-contract.schema.json' > "$tmp/installed/3/omx-plugins/plugin-face.json"
printf '{}\n' > "$tmp/installed/3/other-owner/x.json"
cat > "$tmp/pkg-config" <<EOF
#!/bin/sh
case "\$*" in
  *--variable=contractdir*) echo "$tmp/installed";;
  *--variable=datadir*) echo "$tmp/data";;
  *) exec $real "\$@";;
esac
EOF
chmod +x "$tmp/pkg-config"
cat > "$tmp/cli.mjs" <<'EOF'
import { existsSync, readFileSync, readdirSync, writeFileSync } from 'node:fs';
import { execFileSync } from 'node:child_process';
import { join } from 'node:path';
const a = process.argv.slice(2);
const at = (f) => a[a.indexOf(f) + 1];
const dir = a.includes('--contract-dir') ? at('--contract-dir') : execFileSync(process.env.PKG_CONFIG, ['--variable=contractdir', 'omx-contract'], { encoding: 'utf8' }).trim();
for (const major of readdirSync(dir)) for (const pkg of readdirSync(join(dir, major))) {
  const f = join(dir, major, pkg, 'plugin-face.json');
  if (!existsSync(f)) continue;
  const s = JSON.parse(readFileSync(f, 'utf8')).$schema;
  if (/FreeMixer/.test(s)) { console.error(`contract.schema.major ${pkg}/plugin-face.json: $schema "${s}" is not the installed form`); process.exit(1); }
}
const v = JSON.parse(readFileSync(join(process.env.FAKE_DATA, 'omx-contract.json'), 'utf8')).version;
writeFileSync(join(at('--out'), 'omx-contract.json'), JSON.stringify({ version: v, kernels: { k: {} }, items: { PARAM_SCALES: { kind: 'words', value: ['linear', 'log', 'stepped'] } } }));
EOF
got=$(cd "$here" && PKG_CONFIG="$tmp/pkg-config" OMX_CONTRACT_CLI="$tmp/cli.mjs" FAKE_DATA="$tmp/data" OMX_INSTALLED_CONTRACTDIR="$tmp/installed" OMX_CONTRACT_DIR='' \
  node --input-type=module -e "import { items, locateContract } from './tools/omx-contract.mjs'; const c = locateContract(); if (!c.dir) throw new Error(c.why); console.log(JSON.stringify(items(c.dir).PARAM_SCALES.value));" 2>&1) || true
case $got in
  '["linear","log","stepped"]') echo "tree-contract: the build's CLI path read this tree's omx-plugins, not the installed stale one" ;;
  *) echo "tree-contract: the build's CLI path did not read this tree's contract: $(printf '%s\n' "$got" | grep -m1 '^Error' || printf '%s\n' "$got" | head -1)" >&2; exit 1 ;;
esac
