#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# round-trip-contract-dir.sh <dir> — compose the contract directory the round trip hands plugin-hostd-probe
# (tools/round-trip.sh): every installed package of omx-contract's contract directory BUT omx-plugins, linked, and
# this tree's omx-plugins package (its descriptors and its definition files, contract/role-meanings.json among them),
# installed by this tree's own `make install-contract`. The probe writes a control's meaning from the role meanings it
# reads there, so what the round trip counts never depends on which omx-plugins-contract is installed (none on a clean
# runner, another release in a package build root; #35). The installed directory is $OMX_INSTALLED_CONTRACTDIR, else
# omx-contract's pkg-config contractdir, else /usr/share/omx-contract/contract.d. Prints <dir>.
set -eu
[ $# -eq 1 ] || { echo "usage: round-trip-contract-dir.sh <dir>" >&2; exit 2; }
out=$1
here=$(cd "$(dirname "$0")/.." && pwd)
installed=${OMX_INSTALLED_CONTRACTDIR:-$(pkg-config --variable=contractdir omx-contract 2>/dev/null || true)}
[ -n "$installed" ] || installed=/usr/share/omx-contract/contract.d
rm -rf "$out"
mkdir -p "$out"
for major in "$installed"/*/; do
  [ -d "$major" ] || continue
  m=$(basename "$major")
  mkdir -p "$out/$m"
  for pkg in "$major"*/; do
    [ -d "$pkg" ] || continue
    name=$(basename "$pkg")
    [ "$name" = omx-plugins ] && continue
    ln -s "${pkg%/}" "$out/$m/$name"
  done
done
make -s -C "$here" install-contract DESTDIR= CONTRACTDIR="$out" >/dev/null
echo "$out"
