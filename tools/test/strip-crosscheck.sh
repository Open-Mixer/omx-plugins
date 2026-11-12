#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# strip-crosscheck.sh [<ref>] — omx-strip generated as a chain against the hand-ported omx-strip it replaced, bit for bit at the old plugin's
# settings, at every rate the contract declares (tools/test/clap-crosscheck.c, tools/test/strip-0.2.0.map).
#
# The old plugin is built from <ref>, by default the newest commit whose declaration is not yet a
# chain (its hand-written omx_strip.h and its own generated header together), against the omx-contract
# and omx-dsp releases that commit pinned (its .github/pins.txt): the DSP's instance faces change shape
# between releases (omx_comp_instance_resolve gained an argument in 0.7.0), so the old header builds
# only against its own. Each pinned release is built from the git tag in a sibling checkout
# ($OMX_DEPS_DIR/<name>, default the directory beside this tree) or from its GitHub release source, and
# installed into a private prefix the old plugin alone sees. The new one is built from this tree against
# what is installed. Exit 0 when the two are the same plugin at every setting the
# old one had, 1 on any difference, 2 when either cannot be built.
set -eu
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
REF=${1:-}
if [ -z "$REF" ]; then
  for c in $(git -C "$ROOT" rev-list HEAD -- plugins/omx-strip/omx-strip.decl.json); do
    git -C "$ROOT" show "$c:plugins/omx-strip/omx-strip.decl.json" | grep -q '"binding": "chain"' || { REF=$c; break; }
  done
fi
[ -n "$REF" ] || { echo "strip-crosscheck: no commit holds the hand-ported omx-strip" >&2; exit 2; }
W=$(mktemp -d)
trap 'rm -rf "$W"' EXIT INT TERM
git -C "$ROOT" archive "$REF" include plugins/omx-strip .github/pins.txt | tar -x -C "$W"
[ -f "$W/plugins/omx-strip/omx_strip.h" ] || { echo "strip-crosscheck: $REF holds no hand-written omx_strip.h" >&2; exit 2; }
OLD=$W/old
DEPS=${OMX_DEPS_DIR:-$ROOT/..}
# install_pinned <name>: the release .github/pins.txt of $REF names, into $OLD
install_pinned() {
  ver=$(awk -v n="$1" '$1 == n { print $3 }' "$W/.github/pins.txt")
  [ -n "$ver" ] || { echo "strip-crosscheck: $REF pins no $1" >&2; exit 2; }
  src=$W/src/$1
  mkdir -p "$src"
  if git -C "$DEPS/$1" rev-parse -q --verify "v$ver^{commit}" >/dev/null 2>&1; then
    git -C "$DEPS/$1" archive "v$ver" | tar -x -C "$src"
  else
    curl -fsSL "https://github.com/Open-Mixer/$1/archive/refs/tags/v$ver.tar.gz" | tar -xz --strip-components=1 -C "$src" ||
      { echo "strip-crosscheck: $1 $ver is neither tag v$ver in $DEPS/$1 nor fetchable from github.com/Open-Mixer/$1" >&2; exit 2; }
  fi
  PKG_CONFIG_PATH=$OLD/share/pkgconfig:$OLD/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH} \
    make -s -C "$src" install PREFIX="$OLD" LIBDIR="$OLD/lib" >/dev/null || { echo "strip-crosscheck: $1 $ver does not build" >&2; exit 2; }
}
install_pinned omx-contract
install_pinned omx-dsp
# The CLAP ABI headers come from the installed omx-clap-core, read before the old prefix shadows omx-dsp:
# its .pc requires an omx-dsp newer than the pinned one, which would leave the old build with no flags.
PKG_CONFIG=${PKG_CONFIG:-pkg-config}
ABI_CFLAGS=$($PKG_CONFIG --cflags omx-clap-core clap)
PKG_CONFIG_PATH=$OLD/share/pkgconfig:$OLD/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH} \
  make -s -C "$W/plugins/omx-strip" build/omx-strip.clap ABI_CFLAGS="$ABI_CFLAGS" >/dev/null || exit 2
make -s -C "$ROOT/plugins/omx-strip" build/omx-strip.clap >/dev/null || exit 2
${CC:-cc} -O2 -std=gnu11 -Wall -Wextra -Werror $($PKG_CONFIG --cflags clap omx-contract) -o "$W/clap-crosscheck" \
  "$ROOT/tools/test/clap-crosscheck.c" -ldl -lm || exit 2
echo "strip-crosscheck: the hand-ported omx-strip at $(git -C "$ROOT" rev-parse --short "$REF") against this tree's"
"$W/clap-crosscheck" "$W/plugins/omx-strip/build/omx-strip.clap" "$ROOT/plugins/omx-strip/build/omx-strip.clap" \
  "$ROOT/tools/test/strip-0.2.0.map"
