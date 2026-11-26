#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# meter-held-check.sh — a BUILT face holds its meters until the host reads them, through the whole path (omx-comp's
# CLAP face, lib/omx_clap_face.c, include/omx_meter_ring.h), at every oracle rate and every quantum from 32 to 1024:
# tools/test/meter-held-face.c holds the level, the clip word and the gain reduction's step and fall to a reference
# instance read after every block, and tools/test/meter-held-host.c reads the face through omx-clap's library
# (omx_clap_host_meter_values): a 10 ms burst between two reads is in the later one.
#
#   tools/meter-held-check.sh              build omx-comp and run both
#   tools/meter-held-check.sh --self-test  the same over a copy of the tree whose ring is broken, one kind at a time:
#                                          each arm must go red
#
# The sabotage arms, each a copy of the ring with one thing broken:
#   no-hold       nothing is held: a value is the block's own (every kind)
#   no-restart    a read never starts the hold again (every kind: the read after never recovers)
#   no-deep       a gain reduction holds its largest, not its deepest (the gain reduction)
#   no-clip-hold  the clip word is the last block's run, not the longest since the read (the clip word)
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
CC="${CC:-cc}"
PKG_CONFIG="${PKG_CONFIG:-pkg-config}"
scratch="$(mktemp -d)"; trap 'rm -rf "$scratch"' EXIT

# shellcheck disable=SC2046
$CC -std=gnu11 -O2 -Wall -Wextra -Werror $($PKG_CONFIG --cflags omx-clap omxdsp clap) \
  -o "$scratch/held-face" "$HERE/tools/test/meter-held-face.c" -ldl -lm
# shellcheck disable=SC2046
$CC -std=gnu11 -O2 -Wall -Wextra -Werror $($PKG_CONFIG --cflags omx-clap omxdsp clap) \
  -o "$scratch/held-host" "$HERE/tools/test/meter-held-host.c" $($PKG_CONFIG --libs omx-clap) -lpthread -lm

run() { # <tree>
  make -s -C "$1/plugins/omx-comp" all >/dev/null
  "$scratch/held-face" "$1/plugins/omx-comp/build/omx-comp.clap" && "$scratch/held-host" "$1/plugins/omx-comp/build/omx-comp.clap"
}

if [ "${1:-}" = --self-test ]; then
  tree="$scratch/tree"; mkdir -p "$tree"
  # the tracked files in a checkout; a release tarball (the package builds' make test) has no git, and is its tracked files
  if [ "$(git -C "$HERE" rev-parse --show-toplevel 2>/dev/null)" = "$HERE" ]; then
    (cd "$HERE" && git ls-files -z | tar --null -T - -cf -) | tar -C "$tree" -xf -
  else
    tar -C "$HERE" --exclude=./build --exclude='./plugins/*/build' -cf - . | tar -C "$tree" -xf -
  fi
  h="$tree/include/omx_meter_ring.h"
  fail=0
  for arm in no-hold no-restart no-deep no-clip-hold; do
    cp "$HERE/include/omx_meter_ring.h" "$h"
    case $arm in
      no-hold) sed -i 's/^    if (restart || !((r->hold_mask >> i) \& 1u)) {$/    if (1 || restart) {/' "$h" ;;
      no-restart) sed -i 's/^    const int restart = .*$/    const int restart = 0;/' "$h" ;;
      no-deep) sed -i 's/((r->deep_mask >> i) \& 1u) ?/0 ?/' "$h" ;;
      no-clip-hold) sed -i 's/^      if (k > r->clip_acc\[i\]) r->clip_acc\[i\] = k;$/      r->clip_acc[i] = k;/' "$h" ;;
    esac
    cmp -s "$h" "$HERE/include/omx_meter_ring.h" && { echo "SABOTAGE $arm: did not change the header"; fail=1; continue; }
    rm -rf "$tree/plugins/omx-comp/build" "$tree/build"
    if run "$tree" > "$scratch/$arm.log" 2>&1; then echo "SABOTAGE $arm: STAYED GREEN"; cat "$scratch/$arm.log"; fail=1
    else echo "SABOTAGE $arm: red ($(grep -m1 '^FAIL' "$scratch/$arm.log" || tail -n 1 "$scratch/$arm.log"))"; fi
  done
  [ "$fail" = 0 ] || exit 1
  echo "meter-held-check self-test: every sabotage red"
  exit 0
fi
run "$HERE"
