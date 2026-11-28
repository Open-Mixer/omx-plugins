#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# round-trip-contract-dir-check.sh — the contract directory the round trip hands plugin-hostd-probe
# (tools/round-trip-contract-dir.sh) holds THIS tree's omx-plugins package, never an installed one, beside every other
# installed package. The probe writes a control's meaning from the role meanings it reads there, so an installed
# omx-plugins-contract of another release (a package build root pulls one in through a BuildRequires chain) would make
# the counted losses depend on what happens to be installed (#35).
#
# Against a fake installed directory holding a STALE omx-plugins (every role meaning unknown) and one other package:
# the composed directory's omx-plugins role meanings are this tree's file, and the other package is there.
# Exit 0 pass, 1 refused.
set -eu
here=$(cd "$(dirname "$0")/../.." && pwd)
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
fake="$tmp/installed"
mkdir -p "$fake/omx-plugins" "$fake/other-owner"
printf '{"OMX_PLUGINS_ROLE_MEANINGS":{"kind":"roleMeanings","roles":{"threshold":{"role":"threshold","meaning":"unknown"}}}}\n' > "$fake/omx-plugins/role-meanings.json"
printf '{}\n' > "$fake/other-owner/x.json"
# a DOMAIN directory sits beside the packages (plugin-hostd's machine domain, contract.d/machine/machine.json and its
# own subdirectories): its subdirectories are not packages, and it is carried whole
mkdir -p "$fake/machine/formats"
printf '{}\n' > "$fake/machine/machine.json"
printf '{}\n' > "$fake/machine/formats/x.json"
out="$tmp/composed"
OMX_INSTALLED_CONTRACTDIR="$fake" sh "$here/tools/round-trip-contract-dir.sh" "$out" >/dev/null
bad=0
# The install rewrites line 2, the relative $schema, to the major's URL; every other line is this tree's, byte for byte.
got="$out/omx-plugins/role-meanings.json"
if [ -f "$got" ]; then
  sed 2d "$got" > "$tmp/installed-body"; sed 2d "$here/contract/role-meanings.json" > "$tmp/tree-body"
  cmp -s "$tmp/installed-body" "$tmp/tree-body" || { echo "round-trip-contract-dir: omx-plugins/role-meanings.json is not this tree's" >&2; bad=1; }
  sed -n 2p "$got" | grep -Eq '^  "[$]schema": "https://openmixer\.org/schema/omx-contract/v[0-9]+/omx-contract\.schema\.json",$' || { echo "round-trip-contract-dir: omx-plugins/role-meanings.json line 2 is not the major's \$schema URL: $(sed -n 2p "$got")" >&2; bad=1; }
else
  echo "round-trip-contract-dir: omx-plugins/role-meanings.json is absent from the composed directory" >&2; bad=1
fi
[ -e "$out/machine/machine.json" ] && [ -e "$out/machine/formats/x.json" ] || { echo "round-trip-contract-dir: the installed machine domain is not carried whole (omx-contract refuses a domain directory without machine.json)" >&2; bad=1; }
[ ! -e "$out/machine/formats/formats" ] && [ ! -d "$out/formats" ] || { echo "round-trip-contract-dir: a domain's subdirectory was treated as a package" >&2; bad=1; }
[ -e "$out/other-owner/x.json" ] || { echo "round-trip-contract-dir: the other installed package is missing" >&2; bad=1; }
# A reader that lists the packages as directory entries (omx-panel's contractFiles: readdir withFileTypes, then
# isDirectory()) skips a symbolic link, so an installed package linked in is not there for it.
[ -d "$out/other-owner" ] && [ ! -L "$out/other-owner" ] || { echo "round-trip-contract-dir: the other installed package is a link, which a reader listing directory entries skips" >&2; bad=1; }
[ "$bad" -eq 0 ] && echo "round-trip-contract-dir: this tree's omx-plugins beside the installed others"
exit "$bad"
