#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/test-role-check.sh — a test plugin (its declaration says `"role": "test"`,
# tools/test-plugins.mjs) is linked into libomxplugins.a and `make install` never installs it, so
# no CLAP or LV2 package carries it and the release gate (tools/qualify-release.sh, which probes every
# INSTALLED .clap) never sees it; every shipped plugin is installed. `make install-test` puts it in
# omx-plugins-test's directory outside the CLAP path (tools/package-lists-check.sh checks that one).
#
#   tools/test-role-check.sh               `make install` into a scratch stage, then the check over it
#                                          and build/lib/libomxplugins.a
#   tools/test-role-check.sh --self-test   the check over hand-made stages: a test plugin's .clap or
#                                          .lv2 in the stage, a shipped plugin missing from it and a
#                                          test face missing from the archive each refuse; a right
#                                          stage passes
# Exit: 0 pass, 1 refused, 2 the check could not run.
set -euo pipefail
HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)

# stems <role>: the plugin folders the top Makefile builds (each with a Makefile) whose declaration
# says `"role": "test"` (test), or does not (ship). A variant's folder has no declaration of its own
# and ships.
stems() {
  local m dir stem test
  for m in "$HERE"/plugins/*/Makefile; do
    dir=$(dirname "$m"); stem=$(basename "$dir"); test=no
    if [ -f "$dir/$stem.decl.json" ] && grep -q '"role": *"test"' "$dir/$stem.decl.json"; then test=yes; fi
    if [ "$1" = test ] && [ "$test" = yes ]; then echo "$stem"; fi
    if [ "$1" = ship ] && [ "$test" = no ]; then echo "$stem"; fi
  done
  return 0
}

# check <stage> <archive> <test stems> <ship stems>: one line per refusal; the count is the exit status
check() {
  local stage=$1 archive=$2 tests=$3 ships=$4 bad=0 s k hit defined
  # the archive's defined symbols, read once (a `nm | grep -q` under pipefail fails on nm's SIGPIPE)
  defined=$(nm "$archive" 2>/dev/null | awk '$2 ~ /^[A-Z]$/ && $2 != "U" {print $3}')
  for s in $tests; do
    hit=$(find "$stage" -name "$s.clap" -o -name "$s.lv2" -o -name "$s.so" | head -1)
    if [ -n "$hit" ]; then echo "FAIL test-role: test plugin $s is installed (${hit#"$stage"})"; bad=$((bad + 1)); fi
    k=$(sed -n 's/^ *"kernel": *"\([a-z0-9_]*\)".*/\1/p' "$HERE/plugins/$s/$s.decl.json")
    if ! grep -qx "omx_clap_entry_$k" <<<"$defined"; then
      echo "FAIL test-role: test plugin $s's face (omx_clap_entry_$k) is not linked into $(basename "$archive")"
      bad=$((bad + 1))
    fi
  done
  for s in $ships; do
    # a face with no CLAP source (omx-fbs, omx-hrp: LV2 only until omx-clap-core has a worker hook) is installed as its bundle
    if compgen -G "$HERE/plugins/$s/omx_*_clap.c" >/dev/null; then want="$s.clap"; kind=clap; else want="$s.lv2"; kind=lv2; fi
    if [ -z "$(find "$stage" -name "$want" | head -1)" ]; then echo "FAIL test-role: shipped plugin $s is not installed (its $kind face)"; bad=$((bad + 1)); fi
  done
  return "$bad"
}

if [ "${1:-}" = --self-test ]; then
  scratch=$(mktemp -d "${TMPDIR:-/tmp}/test-role.XXXXXX")
  trap 'rm -rf "$scratch"' EXIT
  tests=$(stems test); ships=$(stems ship)
  if [ -z "$tests" ]; then echo "test-role self-test: no test plugin is declared; nothing to sabotage"; exit 0; fi
  t1=$(echo "$tests" | head -1); s1=$(echo "$ships" | head -1)
  k1=$(sed -n 's/^ *"kernel": *"\([a-z0-9_]*\)".*/\1/p' "$HERE/plugins/$t1/$t1.decl.json")
  printf 'const int omx_clap_entry_%s = 1;\n' $(for t in $tests; do sed -n 's/^ *"kernel": *"\([a-z0-9_]*\)".*/\1/p' "$HERE/plugins/$t/$t.decl.json"; done) > "$scratch/faces.c"
  cc -c -o "$scratch/faces.o" "$scratch/faces.c" && ar rcs "$scratch/full.a" "$scratch/faces.o"
  grep -v "omx_clap_entry_$k1 " "$scratch/faces.c" > "$scratch/short.c" || true
  cc -c -o "$scratch/short.o" "$scratch/short.c" && ar rcs "$scratch/short.a" "$scratch/short.o"
  # a right stage: each shipped plugin's .clap, and for a face with no CLAP source (LV2 only) its bundle
  good() {
    rm -rf "$scratch/stage" && mkdir -p "$scratch/stage/usr/lib/clap" "$scratch/stage/usr/lib64/lv2" || return 1
    for s in $ships; do
      if compgen -G "$HERE/plugins/$s/omx_*_clap.c" >/dev/null; then : > "$scratch/stage/usr/lib/clap/$s.clap"; else mkdir -p "$scratch/stage/usr/lib64/lv2/$s.lv2"; fi
    done
  }
  fail=0
  arm() { # <name> <want 0|1> <archive>
    local rc=0 out
    out=$(check "$scratch/stage" "$3" "$tests" "$ships") || rc=$?
    if { [ "$2" = 0 ] && [ "$rc" -ne 0 ]; } || { [ "$2" = 1 ] && [ "$rc" -eq 0 ]; }; then
      echo "FAIL test-role self-test: arm '$1' exit $rc, wanted $([ "$2" = 0 ] && echo pass || echo a refusal)"
      [ -z "$out" ] || echo "$out"
      fail=1
    else
      echo "ok   test-role self-test: $1 ($([ "$rc" -eq 0 ] && echo passes || echo "refused: ${out%%$'\n'*}"))"
    fi
  }
  good; arm "a right stage" 0 "$scratch/full.a"
  good; : > "$scratch/stage/usr/lib/clap/$t1.clap"; arm "a test plugin's .clap installed" 1 "$scratch/full.a"
  good; mkdir -p "$scratch/stage/usr/lib64/lv2/$t1.lv2"; arm "a test plugin's .lv2 installed" 1 "$scratch/full.a"
  good; rm -f "$scratch/stage/usr/lib/clap/$s1.clap"; arm "a shipped plugin not installed" 1 "$scratch/full.a"
  for s in $ships; do compgen -G "$HERE/plugins/$s/omx_*_clap.c" >/dev/null || { l1=$s; break; }; done
  if [ -n "${l1:-}" ]; then good; rmdir "$scratch/stage/usr/lib64/lv2/$l1.lv2"; arm "an LV2-only plugin's bundle not installed" 1 "$scratch/full.a"; fi
  good; arm "a test face missing from the archive" 1 "$scratch/short.a"
  exit "$fail"
fi

archive="$HERE/build/lib/libomxplugins.a"
[ -f "$archive" ] || { echo "test-role: $archive is not built (make lib)"; exit 2; }
stage=$(mktemp -d "${TMPDIR:-/tmp}/test-role-stage.XXXXXX")
trap 'rm -rf "$stage"' EXIT
make -s -C "$HERE" install DESTDIR="$stage" PREFIX=/usr LIBDIR=/usr/lib64 CLAPDIR=/usr/lib/clap >/dev/null || { echo "test-role: make install failed"; exit 2; }
tests=$(stems test); ships=$(stems ship)
rc=0
check "$stage" "$archive" "$tests" "$ships" || rc=$?
if [ "$rc" -ne 0 ]; then echo "FAIL test-role: $rc refusal(s)"; exit 1; fi
echo "PASS test-role: $(echo $tests | wc -w) test plugin(s) linked and installed by no target ($(echo $tests)); $(echo $ships | wc -w) shipped plugins installed"
