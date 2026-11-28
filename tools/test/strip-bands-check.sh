#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# strip-bands-check.sh [<ref>] — omx-strip's band count (`eqBands`, FreeMixer/omx-plugins#308) against the strip
# before it had one, bit for bit, at every rate the contract declares (tools/test/clap-crosscheck.c):
#
#   golden    Bands at its default, every band past it switched on at a boost: today's strip, bit for bit
#             (the bands past the count are not processed, and the first ones are today's).
#   flat      Bands at the maximum, every band past today's on and flat (0 dB): today's strip, bit for bit.
#   boundary  Bands at 8, bands 5 to 8 on and flat, every band past 8 on at a boost: today's strip.
#   reach     Bands at the maximum, every band past today's on at a boost: NOT today's strip (the count
#             reaches them; without this arm the three above could pass with bands that never reach the face).
#   reach5    Bands at 5, band 5 alone on at a boost: NOT today's strip (the count moves one band at a time).
#   sabotage  the golden arm against a strip built to process every band whatever the count: NOT today's
#             strip (the guard of the golden arm, broken on purpose; it must go red).
#   sessions  today's factory presets, as presets and as the sessions today's strip saves from them, loaded
#             into this strip: the same values, the band count at its default, and the same output
#             (tools/test/clap-session-crosscheck.c).
#   sessions-sabotage  the same against a strip built to lose a session's third band: NOT the same (red).
#
# Today's strip is built from <ref>, by default the newest first-parent commit whose declaration has no band count, against
# the omx-dsp installed (the one this tree builds with). Exit 0 when every arm is as stated, 1 when one is not, 2
# when a plugin cannot be built.
set -eu
ROOT=$(cd "$(dirname "$0")/../.." && pwd -P)
REF=${1:-}
DECL=plugins/omx-strip/omx-strip.decl.json
# a release tarball (a package's %check) has no history to build today's strip from: said, never silent; CI runs it
if [ -z "$REF" ] && [ "$(git -C "$ROOT" rev-parse --show-toplevel 2>/dev/null)" != "$ROOT" ]; then
  echo "strip-bands-check: NOTE no git history here (a release tarball): the strip before its band count cannot be built, nothing compared; CI's make test runs it" >&2
  exit 0
fi
if [ -z "$REF" ]; then
  # the whole tree as it was (its faces, its presets), not only the declaration: first-parent history, newest first
  for c in $(git -C "$ROOT" rev-list --first-parent HEAD); do
    git -C "$ROOT" show "$c:$DECL" | grep -q '"bandCount"' || { REF=$c; break; }
  done
fi
[ -n "$REF" ] || { echo "strip-bands-check: no commit holds omx-strip without its band count" >&2; exit 2; }
W=$(mktemp -d)
trap 'rm -rf "$W"' EXIT INT TERM
PKG_CONFIG=${PKG_CONFIG:-pkg-config}
CC=${CC:-cc}

# today's strip, from <ref>
mkdir -p "$W/old"
git -C "$ROOT" archive "$REF" include lib plugins/omx-strip presets/org.openmixer.strip | tar -x -C "$W/old"
# <ref>'s tree names the installed headers where their owners kept them then; a header an owner has moved since is
# included from its home today, the same declarations under another path (one row per move: old path, new path)
while read -r was now; do
  grep -rlF "<$was>" "$W/old" | while read -r f; do sed -i "s|<$was>|<$now>|" "$f"; done
done <<'MOVED'
omxcontract/omx_param_units.h omx-clap-host/omx_param_units.h
MOVED
make -s -C "$W/old/plugins/omx-strip" build/omx-strip.clap >/dev/null || { echo "strip-bands-check: $REF's omx-strip does not build" >&2; exit 2; }
OLD=$W/old/plugins/omx-strip/build/omx-strip.clap
# this tree's
make -s -C "$ROOT/plugins/omx-strip" build/omx-strip.clap >/dev/null || exit 2
NEW=$ROOT/plugins/omx-strip/build/omx-strip.clap
# sabotaged <dir> <sed expression> <what>: this tree's strip with its binding broken by <sed expression>, built in <dir>
sabotaged() {
  mkdir -p "$W/$1/plugins"
  cp -r "$ROOT/include" "$ROOT/lib" "$W/$1/"
  cp -r "$ROOT/plugins/omx-strip" "$W/$1/plugins/"
  rm -rf "$W/$1/plugins/omx-strip/build"
  cp "$W/$1/plugins/omx-strip/omx_strip_core.h" "$W/$1/core.h"
  sed -i "$2" "$W/$1/plugins/omx-strip/omx_strip_core.h"
  ! cmp -s "$W/$1/core.h" "$W/$1/plugins/omx-strip/omx_strip_core.h" ||
    { echo "strip-bands-check: the sabotage ($3) found nothing to break in omx_strip_core.h" >&2; exit 2; }
  make -s -C "$W/$1/plugins/omx-strip" build/omx-strip.clap >/dev/null || { echo "strip-bands-check: the strip sabotaged ($3) does not build" >&2; exit 2; }
}
# every band processed whatever the count: the binding's band gate replaced by the maximum
sabotaged sab 's/u < omx_strip_eq_band_count(values) ?/u < (uint32_t)OMX_STRIP_PARAM_EQ_BANDS_MAX ?/g' 'every band processed'
SAB=$W/sab/plugins/omx-strip/build/omx-strip.clap
# a session's third band lost: the gate of band 3 shut whatever the count
sabotaged sab3 's/(2u < omx_strip_eq_band_count(values) ?/(2u < 2u ?/' 'band 3 lost'
SAB3=$W/sab3/plugins/omx-strip/build/omx-strip.clap

ABI=$($PKG_CONFIG --cflags clap omx-contract)
$CC -O2 -std=gnu11 -Wall -Wextra -Werror $ABI -o "$W/clap-crosscheck" "$ROOT/tools/test/clap-crosscheck.c" -ldl -lm || exit 2
$CC -O2 -std=gnu11 -Wall -Wextra -Werror $ABI -o "$W/clap-session-crosscheck" "$ROOT/tools/test/clap-session-crosscheck.c" -ldl -lm || exit 2

# The parameters, by CLAP name: today's (the first OLDN of this tree's, unchanged), and this tree's appended ones.
OLDN=$(sed -n 's/^  OMX_STRIP_PARAM_COUNT = \([0-9]*\)$/\1/p' "$W/old/plugins/omx-strip/generated/omx_strip_params.h")
[ -n "$OLDN" ] || { echo "strip-bands-check: $REF's parameter count is unreadable" >&2; exit 2; }
node --input-type=module -e "
import { loadDecl } from '$ROOT/tools/gen.mjs';
const d = loadDecl('$ROOT/plugins/omx-strip');
for (const p of d.params) console.log([p.symbol, p.name, p.min, p.max, p.def].join('|'));
" > "$W/params.txt"
BANDS_MAX=$(awk -F'|' '$1 == "eqBands" { print $4 }' "$W/params.txt")
BANDS_DEF=$(awk -F'|' '$1 == "eqBands" { print $5 }' "$W/params.txt")
[ -n "$BANDS_MAX" ] || { echo "strip-bands-check: this tree's omx-strip has no eqBands" >&2; exit 2; }

# map <bands> <first flat band> <first hot band> [<only hot band>]: today's parameters as they are; Bands held; band n
# (past today's) on and flat from <first flat>, on and boosted from <first hot> (or only band <only hot>), else off
map() {
  awk -F'|' -v oldn="$OLDN" -v bands="$1" -v flat="$2" -v hot="$3" -v only="${4:-0}" '
    NR <= oldn { print $2 "|" $2; next }
    $1 == "eqBands" { print "|" $2 "|=" bands; next }
    {
      n = $1; sub(/^eq/, "", n); kind = n; sub(/^[0-9]+/, "", kind); sub(/[A-Za-z]+$/, "", n); n += 0
      h = only ? (n == only) : (n >= hot); f = !h && !only && n >= flat
      v = $5
      if (kind == "On") v = (h || f) ? 1 : 0
      else if (kind == "Type") v = 0
      else if (kind == "Gain") v = h ? 9 : 0
      else if (kind == "Q") v = 2
      else if (kind == "Freq") v = 60 * 2 ^ ((n - 4) / 3.5)
      print "|" $2 "|=" v
    }' "$W/params.txt"
}
NONE=$((BANDS_MAX + 1))
map "$BANDS_DEF" "$NONE" $((BANDS_DEF + 1)) > "$W/golden.map"
map "$BANDS_MAX" 5 "$NONE" > "$W/flat.map"
map 8 5 9 > "$W/boundary.map"
map "$BANDS_MAX" "$NONE" 5 > "$W/reach.map"
map 5 "$NONE" "$NONE" 5 > "$W/reach5.map"

fail=0
# arm <name> <green|red> <b.clap> <map>
arm() {
  if "$W/clap-crosscheck" "$OLD" "$3" "$4" > "$W/$1.log" 2>&1; then got=green; else got=red; fi
  grep -q 'CROSSCHECK-' "$W/$1.log" || got=broken
  if [ "$got" = "$2" ]; then
    echo "ok   strip-bands $1: $(tail -1 "$W/$1.log") ($2, as it must be)"
  else
    echo "FAIL strip-bands $1: $got, and it must be $2" >&2
    sed 's/^/  /' "$W/$1.log" | grep -v '^  PASS' | head -20 >&2
    fail=1
  fi
}
echo "strip-bands-check: today's omx-strip at $(git -C "$ROOT" rev-parse --short "$REF") ($OLDN parameters) against this tree's (Bands 1 to $BANDS_MAX, default $BANDS_DEF)"
arm golden green "$NEW" "$W/golden.map"
arm flat green "$NEW" "$W/flat.map"
arm boundary green "$NEW" "$W/boundary.map"
arm reach red "$NEW" "$W/reach.map"
arm reach5 red "$NEW" "$W/reach5.map"
arm sabotage red "$SAB" "$W/golden.map"
# sessions <name> <green|red> <b.clap>
sessions() {
  if "$W/clap-session-crosscheck" "$OLD" "$3" "$W"/old/presets/org.openmixer.strip/*.json > "$W/$1.log" 2>&1; then got=green; else got=red; fi
  grep -q 'SESSION-CROSSCHECK-' "$W/$1.log" || got=broken
  if [ "$got" = "$2" ]; then
    echo "ok   strip-bands $1: $(tail -1 "$W/$1.log") ($2, as it must be)"
  else
    echo "FAIL strip-bands $1: $got, and it must be $2" >&2
    sed 's/^/  /' "$W/$1.log" | grep -v '^  PASS' | head -20 >&2
    fail=1
  fi
}
sessions sessions green "$NEW"
sessions sessions-sabotage red "$SAB3"
exit $fail
