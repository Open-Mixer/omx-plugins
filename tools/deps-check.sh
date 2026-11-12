#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# deps-check.sh — the releases this repository builds against are declared only in the packaging, as a
# FLOOR AND A CEILING (below 1.0 the ceiling is the next minor of the release the tree is proven on; from 1.0 the
# next major): packaging/omx-plugins.spec's BuildRequires (one `(pkg >= F with pkg < C)` line per dependency), the
# -devel package's Requires on omx-dsp and omx-contract, the Recommends on plugin-hostd, and debian/control's
# Build-Depends, -dev Depends and Recommends, written the same (`pkg (>= F~), pkg (<< C~)`), and
# lib/omxplugins.pc.in's Requires. A floor with no ceiling, or a pair that differs between the formats, is a failure.
#
#   tools/deps-check.sh              the check
#   tools/deps-check.sh --self-test  each sabotage must go red
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"

# how many times the fixed string $1 occurs in file $2 (several on one line count each)
count() { { grep -oF -- "$1" "$2" || true; } | wc -l; }
check() {
  local root="$1" fail=0 spec="$1/packaging/omx-plugins.spec" control="$1/debian/control"
  # rpm name, deb name, how many times debian/control must carry the deb pair, does -devel require it
  for row in "omx-dsp-devel libomxdsp-dev 2 yes" "omx-contract-devel libomx-contract-dev 2 yes" "omx-clap-core-devel libomx-clap-core-dev 1 no" "plugin-hostd-devel plugin-hostd-dev 1 no" "omx-contract omx-contract 1 no" "omx-panel omx-panel 1 no" "plugin-qualify plugin-qualify 1 no" "clap-devel clap-headers 2 yes"; do
    read -r rpm deb times devel <<<"$row"
    local range floor ceil
    range="$(sed -n "s/^BuildRequires: ($rpm >= \([0-9.]*\) with $rpm < \([0-9.]*\))\$/\1 \2/p" "$spec")"
    if [ "$(wc -l <<<"$range")" != 1 ] || [ -z "$range" ]; then
      echo "deps-check: the spec needs exactly one 'BuildRequires: ($rpm >= <floor> with $rpm < <ceiling>)' line" >&2; fail=1; continue
    fi
    read -r floor ceil <<<"$range"
    if [ "$devel" = yes ]; then
      grep -qx "Requires: ($rpm >= $floor with $rpm < $ceil)" "$spec" ||
        { echo "deps-check: the -devel package does not require ($rpm >= $floor with $rpm < $ceil)" >&2; fail=1; }
    fi
    [ "$(count "$deb (>= $floor~)" "$control")" = "$times" ] ||
      { echo "deps-check: debian/control does not carry $deb (>= $floor~) $times time(s)" >&2; fail=1; }
    [ "$(count "$deb (<< $ceil~)" "$control")" = "$times" ] ||
      { echo "deps-check: debian/control does not carry $deb (<< $ceil~) $times time(s)" >&2; fail=1; }
    [ "$(count "$deb (" "$control")" = "$((times * 2))" ] || { echo "deps-check: debian/control names $deb with another range too" >&2; fail=1; }
    [ "$fail" = 1 ] || echo "deps-check: $rpm / $deb >= $floor and < $ceil, in the spec and debian/control"
  done
  # plugin-hostd at run time (Recommends) and at build time: the same ceiling in both formats
  local hostd
  hostd="$(sed -n 's/^BuildRequires: (plugin-hostd >= [0-9.]* with plugin-hostd < \([0-9.]*\))$/\1/p' "$spec")"
  [ -n "$hostd" ] || { echo "deps-check: the spec needs one 'BuildRequires: (plugin-hostd >= <floor> with plugin-hostd < <ceiling>)' line" >&2; fail=1; }
  grep -qE "^Recommends: \(plugin-hostd >= [0-9.]+ with plugin-hostd < ${hostd:-x}\)\$" "$spec" ||
    { echo "deps-check: the spec's Recommends on plugin-hostd has no ceiling, or not $hostd" >&2; fail=1; }
  grep -qE "^Recommends: plugin-hostd \(>= [0-9.]+~\), plugin-hostd \(<< ${hostd:-x}~\)\$" "$control" ||
    { echo "deps-check: debian/control's Recommends on plugin-hostd has no ceiling, or not $hostd" >&2; fail=1; }
  grep -qE "^ *plugin-hostd \(<< ${hostd:-x}~\) <!nocheck>,?\$" "$control" ||
    { echo "deps-check: debian/control's Build-Depends on plugin-hostd has no ceiling, or not $hostd" >&2; fail=1; }
  grep -qE '^Requires:.*omxdsp *>=.*omxdsp *<' "$root/lib/omxplugins.pc.in" || { echo "deps-check: omxplugins.pc.in has no ceiling on omxdsp" >&2; fail=1; }
  grep -qE '^Requires:.*omx-contract *>=.*omx-contract *<' "$root/lib/omxplugins.pc.in" || { echo "deps-check: omxplugins.pc.in has no ceiling on omx-contract" >&2; fail=1; }
  # the DEB build runs the generator checks as the RPM build does: the omx-contract command is a build dependency
  # (above), so debian/rules has no branch that skips them
  if grep -q 'NO-CONTRACT-COMMAND\|make test-faces' "$root/debian/rules"; then echo "deps-check: debian/rules skips the generator checks where the omx-contract command is absent: it is a Build-Depends, run make test" >&2; fail=1; fi
  grep -q 'make test\b\|\$(MAKE) test OMX_LV2_SPEC_DIR' "$root/debian/rules" || { echo "deps-check: debian/rules does not run make test" >&2; fail=1; }
  return "$fail"
}

if [ "${1:-}" = --self-test ]; then
  scratch="$(mktemp -d)"; trap 'rm -rf "$scratch"' EXIT
  fail=0
  for s in dsp-floor dsp-no-ceiling-spec devel-no-ceiling clap-floor control-dsp control-no-ceiling control-ceiling-differs pc-no-ceiling contract-missing control-no-command rules-skip qualify-no-ceiling hostd-recommends-no-ceiling; do
    rm -rf "$scratch/t"; mkdir -p "$scratch/t/packaging" "$scratch/t/debian"
    mkdir -p "$scratch/t/lib"; cp "$HERE/lib/omxplugins.pc.in" "$scratch/t/lib/"; cp "$HERE/debian/rules" "$scratch/t/debian/"
    cp "$HERE/packaging/omx-plugins.spec" "$scratch/t/packaging/"; cp "$HERE/debian/control" "$scratch/t/debian/"
    case "$s" in
      dsp-floor) sed -i 's/^BuildRequires: (omx-dsp-devel >= [0-9.]* /BuildRequires: (omx-dsp-devel >= 0.0.1 /' "$scratch/t/packaging/omx-plugins.spec" ;;
      dsp-no-ceiling-spec) sed -i 's/^BuildRequires: (omx-dsp-devel >= \([0-9.]*\) with .*/BuildRequires: omx-dsp-devel >= \1/' "$scratch/t/packaging/omx-plugins.spec" ;;
      devel-no-ceiling) sed -i 's/^Requires: (omx-dsp-devel >= \([0-9.]*\) with .*/Requires: omx-dsp-devel >= \1/' "$scratch/t/packaging/omx-plugins.spec" ;;
      clap-floor) sed -i '0,/^BuildRequires: (clap-devel >= [0-9.]* /s//BuildRequires: (clap-devel >= 1.2.7 /' "$scratch/t/packaging/omx-plugins.spec" ;;
      qualify-no-ceiling) sed -i 's/^BuildRequires: (plugin-qualify >= \([0-9.]*\) with .*/BuildRequires: plugin-qualify >= \1/' "$scratch/t/packaging/omx-plugins.spec" ;;
      hostd-recommends-no-ceiling) sed -i 's/^Recommends: (plugin-hostd >= \([0-9.]*\) with .*/Recommends: plugin-hostd >= \1/' "$scratch/t/packaging/omx-plugins.spec" ;;
      control-dsp) sed -i '0,/libomxdsp-dev (>= [0-9.]*~)/s//libomxdsp-dev (>= 0.0.1~)/' "$scratch/t/debian/control" ;;
      control-no-ceiling) sed -i 's/, libomx-contract-dev (<< [0-9.]*~)//' "$scratch/t/debian/control" ;;
      control-ceiling-differs) sed -i '0,/omx-panel (<< [0-9.]*~)/s//omx-panel (<< 9~)/' "$scratch/t/debian/control" ;;
      pc-no-ceiling) sed -i 's/, omxdsp < @OMXDSP_MAX@//' "$scratch/t/lib/omxplugins.pc.in" ;;
      control-no-command) sed -i '/^ *omx-contract (>=/d' "$scratch/t/debian/control" ;;
      rules-skip) sed -i 's/^override_dh_auto_test:/&\n\t@echo NO-CONTRACT-COMMAND; $(MAKE) test-faces; exit 0/' "$scratch/t/debian/rules" ;;
      contract-missing) sed -i '/^BuildRequires: (omx-contract-devel /d' "$scratch/t/packaging/omx-plugins.spec" ;;
    esac
    if check "$scratch/t" >/dev/null 2>&1; then echo "SABOTAGE $s: STAYED GREEN"; fail=1; else echo "SABOTAGE $s: red"; fi
  done
  check "$HERE" >/dev/null || { echo "self-test: the unsabotaged check is not green" >&2; exit 1; }
  [ "$fail" = 0 ] || exit 1
  echo "deps-check self-test: every sabotage red"
  exit 0
fi

check "$HERE"
