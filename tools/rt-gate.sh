#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/rt-gate.sh [--faces clap|lv2|all] [--out DIR] [plugin ...]
#
# omx-rt-gate (omx-dsp-devel; spec 7.13) over every face the tree builds: each plugin's CLAP unit and LV2 unit, each
# with the shared face (lib/omx_clap_face.c, lib/omx_lv2_face.c), compiled with the flags the plugin's own Makefile uses.
# One report per face in DIR (default build/rt-gate): <plugin>.<face>.txt, the gate's lines whole.
#
#   exit 0  every face walked and nothing red
#   exit 1  some face is red (its lines are printed)
#   exit 2  something was not judged: the gate is missing, a face has no unit, a unit that derives no root, or the
#           units found are not the units the tree's plugins list. Never a pass in place of a judgement.
#
# The gate and its tables are the installed omx-dsp-devel's; $OMX_RT_GATE names another command and
# $OMX_RT_GATE_DATADIR another tables tree (a branch build of omx-dsp).
set -u
HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd) || exit 2
GATE=${OMX_RT_GATE:-omx-rt-gate}
PKG_CONFIG=${PKG_CONFIG:-pkg-config}
CC=${CC:-cc}
OUT=$HERE/build/rt-gate
FACES="clap lv2"
PLUGINS=()
while [ $# -gt 0 ]; do
  case $1 in
    --out) OUT=${2:?--out needs a directory}; shift 2 ;;
    --faces) case ${2:-} in clap) FACES=clap ;; lv2) FACES=lv2 ;; all) FACES="clap lv2" ;; *) echo "rt-gate: --faces clap|lv2|all" >&2; exit 2 ;; esac; shift 2 ;;
    -*) echo "rt-gate: unknown option $1" >&2; exit 2 ;;
    *) PLUGINS+=("$1"); shift ;;
  esac
done
command -v "$GATE" >/dev/null 2>&1 || { echo "rt-gate: $GATE is not installed: install omx-dsp-devel or libomxdsp-dev (>= the release that ships omx-rt-gate)" >&2; exit 2; }
if [ ${#PLUGINS[@]} -eq 0 ]; then
  for d in "$HERE"/plugins/*/; do PLUGINS+=("$(basename "$d")"); done
fi
mkdir -p "$OUT" || exit 2
rm -f "$OUT"/*.txt
CFLAGS_GATE=$($PKG_CONFIG --cflags omxdsp omx-contract clap lv2 2>/dev/null) || { echo "rt-gate: pkg-config finds no omxdsp, omx-contract, clap or lv2" >&2; exit 2; }
# The units of a face are the .c files on its rule in the plugin's own Makefile: `$(B)/$(NAME).clap:` (CLAP) and
# `$(BUNDLE)/$(NAME).so:` (LV2). A face the Makefile builds and this list misses cannot be skipped: it is read from there.
units_of() { # <plugin dir> <clap|lv2>
  local pat
  case $2 in clap) pat='^\$\(B\)/\$\(NAME\)\.clap:' ;; lv2) pat='^\$\(BUNDLE\)/\$\(NAME\)\.so:' ;; esac
  grep -E "$pat" "$1/Makefile" 2>/dev/null | head -1 | sed 's/^[^:]*://; s/|.*//' | tr ' ' '\n' \
    | sed -n 's|^\$(ROOT)/||p; t; s|^\([A-Za-z0-9_./-]*\.c\)$|plugins/'"$(basename "$1")"'/\1|p' | grep '\.c$' | sed -n 's|^|'"$HERE"'/|p'
}
red=0; bad=0; faces=0
for name in "${PLUGINS[@]}"; do
  dir=$HERE/plugins/$name
  [ -f "$dir/Makefile" ] || continue
  for face in $FACES; do
    mapfile -t units < <(units_of "$dir" "$face")
    [ ${#units[@]} -gt 0 ] || continue
    for u in "${units[@]}"; do [ -f "$u" ] || { echo "rt-gate: $name $face: $u is on the Makefile's rule and does not exist" >&2; exit 2; }; done
    rep=$OUT/$name.$face.txt
    # shellcheck disable=SC2086
    "$GATE" --abi "$face" -- "$CC" $CFLAGS_GATE -std=gnu11 -fPIC -I"$dir/generated" -I"$dir" -I"$HERE/include" "${units[@]}" >"$rep" 2>&1
    rc=$?
    faces=$((faces + 1))
    case $rc in
      0) ;;
      1) red=$((red + 1)); echo "rt-gate: $name $face: RED"; grep -E '^RT-' "$rep" | sed "s|^|  |" ;;
      *) bad=$((bad + 1)); echo "rt-gate: $name $face: NOT JUDGED (exit $rc)"; sed "s|^|  |" "$rep" ;;
    esac
  done
done
[ "$faces" -gt 0 ] || { echo "rt-gate: no face found" >&2; exit 2; }
echo "rt-gate: $faces faces, $red red, $bad not judged"
[ "$bad" -eq 0 ] || exit 2
[ "$red" -eq 0 ] || exit 1
exit 0
