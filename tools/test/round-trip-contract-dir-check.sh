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
mkdir -p "$fake/3/omx-plugins" "$fake/3/other-owner"
printf '{"OMX_PLUGINS_ROLE_MEANINGS":{"kind":"roleMeanings","roles":{"threshold":{"role":"threshold","meaning":"unknown"}}}}\n' > "$fake/3/omx-plugins/role-meanings.json"
printf '{}\n' > "$fake/3/other-owner/x.json"
out="$tmp/composed"
OMX_INSTALLED_CONTRACTDIR="$fake" sh "$here/tools/round-trip-contract-dir.sh" "$out" >/dev/null
bad=0
cmp -s "$out/3/omx-plugins/role-meanings.json" "$here/contract/role-meanings.json" || { echo "round-trip-contract-dir: omx-plugins/role-meanings.json is not this tree's" >&2; bad=1; }
[ -e "$out/3/other-owner/x.json" ] || { echo "round-trip-contract-dir: the other installed package is missing" >&2; bad=1; }
[ "$bad" -eq 0 ] && echo "round-trip-contract-dir: this tree's omx-plugins beside the installed others"
exit "$bad"
