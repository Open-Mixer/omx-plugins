#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
# A CI bridge, to be deleted with its calls once omx-plugins 0.17.0 and omx-panel 0.8.1 are in the OpenMixer channel.
#
# The sheet of the parameter units moved to omx-contract 3.6.0 (OMX_PARAM_CONTRACT_UNITS). The channel's
# omx-plugins-contract before 0.17.0 still installs it as .../contract.d/3/omx-plugins/param-units.json, so beside
# omx-contract 3.6.0 the contract directory declares the item twice and every omx-contract read refuses it
# (contract.item.duplicate). `dnf builddep` pulls that package in (omx-panel requires it). This script, run right after
# builddep:
#   1. drops the file AND its line in the installed MANIFEST.json (the file alone makes the reader refuse the directory
#      with contract.manifest.mismatch, because the manifest still lists it);
#   2. replaces an omx-panel older than 0.8.1 with one built from source. omx-panel 0.8.0 reads its unit codes from that
#      very file and fails without it; 0.8.1 reads omx-contract's sheet. $OMX_PANEL_REF is the omx-panel ref to build
#      (default main, which holds the reader once omx-panel#108 has merged).
# The package this tree builds has neither the file nor the problem, so with 0.17.0 in the channel there is nothing to
# drop and this exits at once. It refuses to run after EXPIRES: by then the channel has both packages, or this is a bug.
set -eu
EXPIRES=2026-11-20
dir=/usr/share/omx-contract/contract.d/3/omx-plugins
f=$dir/param-units.json
[ -e "$f" ] || { echo "ci-stale-units: no stale sheet"; exit 0; }
if [ "$(date -u +%Y-%m-%d)" \> "$EXPIRES" ]; then
  echo "ci-stale-units: expired $EXPIRES: remove this script and its calls from the workflows" >&2
  exit 1
fi

echo "ci-stale-units: dropping $f and its manifest entry, installed by the channel's omx-plugins-contract before 0.17.0"
node -e '
  const fs = require("fs");
  const m = process.argv[1];
  const j = JSON.parse(fs.readFileSync(m, "utf8"));
  if (!j.files || !("param-units.json" in j.files)) { console.error("ci-stale-units: " + m + " does not list param-units.json"); process.exit(1); }
  delete j.files["param-units.json"];
  fs.writeFileSync(m, JSON.stringify(j, null, 2) + "\n");
' "$dir/MANIFEST.json"
rm -f "$f"

have=$(rpm -q --qf '%{VERSION}' omx-panel 2>/dev/null || true)
case "$have" in
  "" ) echo "ci-stale-units: omx-panel is not installed" ;;
  0.8.0|0.[0-7].*)
    ref=${OMX_PANEL_REF:-main}
    echo "ci-stale-units: omx-panel $have reads the dropped sheet; building omx-panel $ref from source"
    dnf install -y --setopt=install_weak_deps=False git npm >/dev/null
    src=$(mktemp -d)
    git clone -q --depth 1 --branch "$ref" https://github.com/Open-Mixer/omx-panel.git "$src"
    (cd "$src" && npm ci --silent && make install PREFIX=/usr BINDIR=/usr/bin DATADIR=/usr/share >/dev/null)
    rm -rf "$src" ;;
  * ) echo "ci-stale-units: omx-panel $have reads omx-contract's sheet" ;;
esac
omx-contract validate --installed >/dev/null && echo "ci-stale-units: the contract directory is accepted"
