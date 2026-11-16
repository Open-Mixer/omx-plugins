#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# deps-check.sh — the releases this repository builds against are declared only in the packaging, as a
# FLOOR ONLY, NO CEILING (owner rule 2026-11-16): packaging/omx-plugins.spec's BuildRequires (one `pkg >= F` line per
# dependency), the -devel package's Requires on omx-dsp and omx-contract, the Recommends on plugin-hostd, and
# debian/control's Build-Depends, -dev Depends and Recommends, written the same (`pkg (>= F~)`), and
# lib/omxplugins.pc.in's Requires. A ceiling on one of our libraries, or a floor that differs between the formats, is a failure.
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
  for row in "omx-dsp-devel libomxdsp-dev 2 yes" "omx-contract-devel libomx-contract-dev 2 yes" "omx-clap-core-devel libomx-clap-core-dev 1 no" "plugin-hostd-devel plugin-hostd-dev 1 no" "omx-contract omx-contract 1 no" "omx-panel omx-panel 1 no" "plugin-qualify plugin-qualify 1 no"; do
    read -r rpm deb times devel <<<"$row"
    local range floor
    range="$(sed -n "s/^BuildRequires: $rpm >= \([0-9.]*\)\$/\1/p" "$spec")"
    if [ "$(wc -l <<<"$range")" != 1 ] || [ -z "$range" ]; then
      echo "deps-check: the spec needs exactly one 'BuildRequires: $rpm >= <floor>' line" >&2; fail=1; continue
    fi
    floor="$range"
    if [ "$devel" = yes ]; then
      grep -qx "Requires: $rpm >= $floor" "$spec" ||
        { echo "deps-check: the -devel package does not require $rpm >= $floor" >&2; fail=1; }
    fi
    [ "$(count "$deb (>= $floor~)" "$control")" = "$times" ] ||
      { echo "deps-check: debian/control does not carry $deb (>= $floor~) $times time(s)" >&2; fail=1; }
    [ "$(count "$deb (" "$control")" = "$times" ] || { echo "deps-check: debian/control names $deb with another range too" >&2; fail=1; }
    [ "$(count "$deb (<<" "$control")" = 0 ] || { echo "deps-check: debian/control puts a ceiling on $deb" >&2; fail=1; }
    [ "$fail" = 1 ] || echo "deps-check: $rpm / $deb >= $floor, floor only, in the spec and debian/control"
  done
  # plugin-hostd at run time (Recommends) and at build time: a floor, no ceiling, in both formats
  grep -qE "^BuildRequires: plugin-hostd >= [0-9.]+\$" "$spec" ||
    { echo "deps-check: the spec needs one 'BuildRequires: plugin-hostd >= <floor>' line" >&2; fail=1; }
  grep -qE "^Recommends: plugin-hostd >= [0-9.]+\$" "$spec" ||
    { echo "deps-check: the spec's Recommends on plugin-hostd is not a floor only" >&2; fail=1; }
  grep -qE "^Recommends: plugin-hostd \(>= [0-9.]+~\)\$" "$control" ||
    { echo "deps-check: debian/control's Recommends on plugin-hostd is not a floor only" >&2; fail=1; }
  grep -qE "^ *plugin-hostd \(>= [0-9.]+~\) <!nocheck>,?\$" "$control" ||
    { echo "deps-check: debian/control's Build-Depends on plugin-hostd is not a floor" >&2; fail=1; }
  # no ceiling on any library of ours, anywhere in the packaging (clap, a third party's, keeps its own)
  if grep -E ' with (omx|plugin)[a-z-]* <|(omx|plugin|libomx)[a-z-]* \(<<' "$spec" "$control" | grep -vE '^[^:]*:?(Breaks|Replaces):' >/dev/null; then
    echo "deps-check: a ceiling on a library of ours in the spec or debian/control" >&2; fail=1; fi
  grep -qE '^Requires:.*omxdsp *>=' "$root/lib/omxplugins.pc.in" && ! grep -qE '^Requires:.*omxdsp *<' "$root/lib/omxplugins.pc.in" || { echo "deps-check: omxplugins.pc.in has no floor-only Requires on omxdsp" >&2; fail=1; }
  grep -qE '^Requires:.*omx-contract *>=' "$root/lib/omxplugins.pc.in" && ! grep -qE '^Requires:.*omx-contract *<' "$root/lib/omxplugins.pc.in" || { echo "deps-check: omxplugins.pc.in has no floor-only Requires on omx-contract" >&2; fail=1; }
  # the DEB build runs the generator checks as the RPM build does: the omx-contract command is a build dependency
  # (above), so debian/rules has no branch that skips them
  if grep -q 'NO-CONTRACT-COMMAND\|make test-faces' "$root/debian/rules"; then echo "deps-check: debian/rules skips the generator checks where the omx-contract command is absent: it is a Build-Depends, run make test" >&2; fail=1; fi
  grep -q 'make test\b\|\$(MAKE) test OMX_LV2_SPEC_DIR' "$root/debian/rules" || { echo "deps-check: debian/rules does not run make test" >&2; fail=1; }
  return "$fail"
}

if [ "${1:-}" = --self-test ]; then
  scratch="$(mktemp -d)"; trap 'rm -rf "$scratch"' EXIT
  fail=0
  for s in dsp-floor spec-ceiling devel-ceiling control-dsp control-ceiling pc-ceiling contract-missing control-no-command rules-skip qualify-ceiling hostd-recommends-ceiling; do
    rm -rf "$scratch/t"; mkdir -p "$scratch/t/packaging" "$scratch/t/debian"
    mkdir -p "$scratch/t/lib"; cp "$HERE/lib/omxplugins.pc.in" "$scratch/t/lib/"; cp "$HERE/debian/rules" "$scratch/t/debian/"
    cp "$HERE/packaging/omx-plugins.spec" "$scratch/t/packaging/"; cp "$HERE/debian/control" "$scratch/t/debian/"
    case "$s" in
      dsp-floor) sed -i 's/^BuildRequires: omx-dsp-devel >= [0-9.]*/BuildRequires: omx-dsp-devel >= 0.0.1/' "$scratch/t/packaging/omx-plugins.spec" ;;
      spec-ceiling) sed -i 's/^\(BuildRequires: omx-dsp-devel >= [0-9.]*\)$/BuildRequires: (omx-dsp-devel >= 0.1 with omx-dsp-devel < 0.99)/' "$scratch/t/packaging/omx-plugins.spec" ;;
      devel-ceiling) sed -i 's/^\(Requires: omx-dsp-devel >= [0-9.]*\)$/Requires: (omx-dsp-devel >= 0.1 with omx-dsp-devel < 0.99)/' "$scratch/t/packaging/omx-plugins.spec" ;;
      qualify-ceiling) sed -i 's/^BuildRequires: plugin-qualify >= \([0-9.]*\)$/BuildRequires: (plugin-qualify >= \1 with plugin-qualify < 99)/' "$scratch/t/packaging/omx-plugins.spec" ;;
      hostd-recommends-ceiling) sed -i 's/^Recommends: plugin-hostd >= \([0-9.]*\)$/Recommends: (plugin-hostd >= \1 with plugin-hostd < 99)/' "$scratch/t/packaging/omx-plugins.spec" ;;
      control-dsp) sed -i '0,/libomxdsp-dev (>= [0-9.]*~)/s//libomxdsp-dev (>= 0.0.1~)/' "$scratch/t/debian/control" ;;
      control-ceiling) sed -i '0,/omx-panel (>= \([0-9.]*\)~)/s//omx-panel (>= \1~), omx-panel (<< 99~)/' "$scratch/t/debian/control" ;;
      pc-ceiling) sed -i 's/omxdsp >= @OMXDSP_MIN@/&, omxdsp < 99/' "$scratch/t/lib/omxplugins.pc.in" ;;
      control-no-command) sed -i '/^ *omx-contract (>=/d' "$scratch/t/debian/control" ;;
      rules-skip) sed -i 's/^override_dh_auto_test:/&\n\t@echo NO-CONTRACT-COMMAND; $(MAKE) test-faces; exit 0/' "$scratch/t/debian/rules" ;;
      contract-missing) sed -i '/^BuildRequires: omx-contract-devel /d' "$scratch/t/packaging/omx-plugins.spec" ;;
    esac
    if check "$scratch/t" >/dev/null 2>&1; then echo "SABOTAGE $s: STAYED GREEN"; fail=1; else echo "SABOTAGE $s: red"; fi
  done
  check "$HERE" >/dev/null || { echo "self-test: the unsabotaged check is not green" >&2; exit 1; }
  [ "$fail" = 0 ] || exit 1
  echo "deps-check self-test: every sabotage red"
  exit 0
fi

check "$HERE"
