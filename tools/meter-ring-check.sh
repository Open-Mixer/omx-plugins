#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# meter-ring-check.sh — include/omx_meter_ring.h, the copies behind every coherent meter, under
# tools/test/meter-ring-stress.c: an audio thread publishing against readers copying, no read may
# mix two blocks.
#
#   tools/meter-ring-check.sh              the stress test and the held-accumulator proof, SECONDS (default 1) per phase
#   tools/meter-ring-check.sh --self-test  each sabotage of the header must make it go red
#
# The sabotage arms, each a copy of the header with one thing broken:
#   two-copies   two copies instead of three: the copy a reader holds is refilled one block on
#   no-recheck   the word is not read again after the copy: a lapped reader keeps a torn copy
#   word-first   the word is stored before the copy is filled: a reader copies a half-filled array
#   no-hold      the held word is the block's own value: a 10 ms burst between two reads is gone
#   no-restart   the accumulator never starts again: a read after a quiet stretch still shows the burst
#   no-deep      a gain reduction holds its largest, not its deepest
#   no-clip-hold the clip word is the block's own run, not the longest since the read
#   no-count     a take does not count
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
CC="${CC:-cc}"
SECONDS_PER_PHASE="${SECONDS_PER_PHASE:-1}"
scratch="$(mktemp -d)"; trap 'rm -rf "$scratch"' EXIT

build() { # <header> <out>
  $CC -std=gnu11 -O2 -Wall -Wextra -Werror -pthread -DOMX_METER_RING_H_PATH="\"$1\"" \
    -o "$2" "$HERE/tools/test/meter-ring-stress.c"
}

if [ "${1:-}" = --self-test ]; then
  h="$HERE/include/omx_meter_ring.h"
  sed 's/^#define OMX_METER_RING_COPIES 3u$/#define OMX_METER_RING_COPIES 2u/' "$h" > "$scratch/two-copies.h"
  sed 's/^    if (again != 0u && .* return n;$/    (void)again; return n;/' "$h" > "$scratch/no-recheck.h"
  # the word's store moved ahead of the fill loop
  awk '/^  atomic_thread_fence\(memory_order_release\);$/ { print "  atomic_store_explicit(&r->word, (((r->count + 1u) & OMX_METER_RING_COUNT_MASK) << 2) | c, memory_order_release);"; next }
       { print }' "$h" > "$scratch/word-first.h"
  sed 's/^    if (restart || !((r->hold_mask >> i) & 1u)) {$/    if (1 || restart) {/' "$h" > "$scratch/no-hold.h"
  sed 's/^  const int restart = .*$/  const int restart = 0;/' "$h" > "$scratch/no-restart.h"
  sed 's/((r->deep_mask >> i) \& 1u) ?/0 ?/' "$h" > "$scratch/no-deep.h"
  sed 's/^      if (k > r->clip_acc\[i\]) r->clip_acc\[i\] = k;$/      r->clip_acc[i] = k;/' "$h" > "$scratch/no-clip-hold.h"
  sed '/atomic_fetch_add_explicit(&r->reads/d' "$h" > "$scratch/no-count.h"
  fail=0
  for s in two-copies no-recheck word-first no-hold no-restart no-deep no-clip-hold no-count; do
    if cmp -s "$h" "$scratch/$s.h"; then echo "SABOTAGE $s: did not change the header"; fail=1; continue; fi
    build "$scratch/$s.h" "$scratch/$s"
    # up to five tries of SECONDS_PER_PHASE * 2 each: a torn read is a race, it is not on the clock
    red=0
    for _ in 1 2 3 4 5; do
      if ! "$scratch/$s" "$SECONDS_PER_PHASE" > "$scratch/$s.log"; then red=1; break; fi
    done
    if [ "$red" = 1 ]; then echo "SABOTAGE $s: red ($(grep -m1 '^FAIL' "$scratch/$s.log"))"
    else echo "SABOTAGE $s: STAYED GREEN"; cat "$scratch/$s.log"; fail=1; fi
  done
  [ "$fail" = 0 ] || exit 1
  echo "meter-ring-check self-test: every sabotage red"
  exit 0
fi

build "$HERE/include/omx_meter_ring.h" "$scratch/stress"
"$scratch/stress" "$SECONDS_PER_PHASE"
