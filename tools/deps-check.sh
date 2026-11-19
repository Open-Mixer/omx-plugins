#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# deps-check.sh — the releases this repository builds against are declared only in the packaging, as a FLOOR and
# NO CEILING (floors only, as 0.8.0 ruled): packaging/omx-plugins.spec's BuildRequires (one `BuildRequires: pkg >= F`
# line per dependency), the -devel package's Requires on omx-dsp and omx-contract, the Recommends on plugin-hostd,
# debian/control's Build-Depends, -dev Depends and Recommends, written the same (`pkg (>= F~)`), and
# lib/omxplugins.pc.in's Requires. A ceiling anywhere in those, or a floor that differs between the formats, fails.
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
    local floor
    floor="$(sed -n "s/^BuildRequires: $rpm >= \([0-9.]*\)\$/\1/p" "$spec")"
    if [ "$(wc -l <<<"$floor")" != 1 ] || [ -z "$floor" ]; then
      echo "deps-check: the spec needs exactly one 'BuildRequires: $rpm >= <floor>' line" >&2; fail=1; continue
    fi
    if [ "$devel" = yes ]; then
      grep -qx "Requires: $rpm >= $floor" "$spec" ||
        { echo "deps-check: the -devel package does not require $rpm >= $floor" >&2; fail=1; }
    fi
    [ "$(count "$deb (>= $floor~)" "$control")" = "$times" ] ||
      { echo "deps-check: debian/control does not carry $deb (>= $floor~) $times time(s)" >&2; fail=1; }
    # a floor only: the pair is the whole of what debian/control says about $deb
    [ "$(count "$deb (" "$control")" = "$times" ] || { echo "deps-check: debian/control names $deb with another range too" >&2; fail=1; }
    [ "$fail" = 1 ] || echo "deps-check: $rpm / $deb >= $floor, in the spec and debian/control"
  done
  # no ceiling on any dependency: the spec before its %changelog (the history is not read), and debian/control
  # (there are no Breaks, Replaces or Conflicts lines, below). A ceiling is any
  # '<' on a dependency line in the spec, bare or inside 'with' (rpm), and any '(<' clause in debian/control (dpkg: << or <)
  if sed '/^%changelog/q' "$spec" | grep -qE '^(BuildRequires|Requires|Recommends|Suggests)[(:].*<'; then echo "deps-check: the spec states a ceiling (<) on a dependency" >&2; fail=1; fi
  if grep -vE '^(Breaks|Replaces|Conflicts):' "$control" | grep -q '(<'; then echo "deps-check: debian/control states a ceiling (<< or <) on a dependency" >&2; fail=1; fi
  # no Breaks, Replaces or Conflicts between any of our packages (owner rule 2026-11-16), a package of this source included:
  # any such field in debian/control, and any Conflicts or Obsoletes in the spec before its %changelog, is refused by name
  if grep -qE '^(Breaks|Replaces|Conflicts):' "$control"; then echo "deps-check: debian/control has a Breaks, Replaces or Conflicts field" >&2; fail=1; fi
  if sed '/^%changelog/q' "$spec" | grep -qE '^(Conflicts|Obsoletes):'; then echo "deps-check: the spec has a Conflicts or Obsoletes line" >&2; fail=1; fi
  # plugin-hostd at run time (Recommends) and at build time: a floor, no ceiling
  grep -qE '^BuildRequires: plugin-hostd >= [0-9.]+$' "$spec" || { echo "deps-check: the spec needs one 'BuildRequires: plugin-hostd >= <floor>' line" >&2; fail=1; }
  grep -qE '^Recommends: plugin-hostd >= [0-9.]+$' "$spec" || { echo "deps-check: the spec's Recommends on plugin-hostd is not a floor" >&2; fail=1; }
  grep -qE '^Recommends: plugin-hostd \(>= [0-9.]+~\)$' "$control" || { echo "deps-check: debian/control's Recommends on plugin-hostd is not a floor" >&2; fail=1; }
  grep -qE '^Requires:.*omxdsp >= ' "$root/lib/omxplugins.pc.in" || { echo "deps-check: omxplugins.pc.in has no floor on omxdsp" >&2; fail=1; }
  grep -qE '^Requires:.*omx-contract >= ' "$root/lib/omxplugins.pc.in" || { echo "deps-check: omxplugins.pc.in has no floor on omx-contract" >&2; fail=1; }
  if grep -qE '@(OMXDSP|CONTRACT)_MAX@|^Requires:.*<' "$root/lib/omxplugins.pc.in"; then echo "deps-check: omxplugins.pc.in states a ceiling" >&2; fail=1; fi
  # the DEB build runs the generator checks as the RPM build does: the omx-contract command is a build dependency
  # (above), so debian/rules has no branch that skips them
  if grep -q 'NO-CONTRACT-COMMAND\|make test-faces' "$root/debian/rules"; then echo "deps-check: debian/rules skips the generator checks where the omx-contract command is absent: it is a Build-Depends, run make test" >&2; fail=1; fi
  grep -q 'make test\b\|\$(MAKE) test OMX_LV2_SPEC_DIR' "$root/debian/rules" || { echo "deps-check: debian/rules does not run make test" >&2; fail=1; }
  return "$fail"
}

if [ "${1:-}" = --self-test ]; then
  scratch="$(mktemp -d)"; trap 'rm -rf "$scratch"' EXIT
  fail=0
  for s in dsp-floor dsp-ceiling-returns-spec devel-ceiling-returns qualify-ceiling-returns hostd-recommends-ceiling-returns control-dsp control-ceiling-returns control-hostd-ceiling-returns pc-ceiling-returns contract-devel-ceiling-returns devel-contract-ceiling-returns panel-ceiling-returns control-contract-ceiling-returns control-panel-ceiling-returns spec-bare-ceiling-added control-bare-ceiling-added control-breaks-sibling-added spec-conflicts-sibling-added control-breaks-own-added control-replaces-added control-breaks-continued spec-conflicts-own-added contract-missing control-no-command rules-skip; do
    rm -rf "$scratch/t"; mkdir -p "$scratch/t/packaging" "$scratch/t/debian"
    mkdir -p "$scratch/t/lib"; cp "$HERE/lib/omxplugins.pc.in" "$scratch/t/lib/"; cp "$HERE/debian/rules" "$scratch/t/debian/"
    cp "$HERE/packaging/omx-plugins.spec" "$scratch/t/packaging/"; cp "$HERE/debian/control" "$scratch/t/debian/"
    case "$s" in
      dsp-floor) sed -i 's/^BuildRequires: omx-dsp-devel >= [0-9.]*$/BuildRequires: omx-dsp-devel >= 0.0.1/' "$scratch/t/packaging/omx-plugins.spec" ;;
      dsp-ceiling-returns-spec) sed -i 's/^BuildRequires: omx-dsp-devel >= \([0-9.]*\)$/BuildRequires: (omx-dsp-devel >= \1 with omx-dsp-devel < 0.33)/' "$scratch/t/packaging/omx-plugins.spec" ;;
      devel-ceiling-returns) sed -i 's/^Requires: omx-dsp-devel >= \([0-9.]*\)$/Requires: (omx-dsp-devel >= \1 with omx-dsp-devel < 0.33)/' "$scratch/t/packaging/omx-plugins.spec" ;;
      qualify-ceiling-returns) sed -i 's/^BuildRequires: plugin-qualify >= \([0-9.]*\)$/BuildRequires: (plugin-qualify >= \1 with plugin-qualify < 0.21)/' "$scratch/t/packaging/omx-plugins.spec" ;;
      hostd-recommends-ceiling-returns) sed -i 's/^Recommends: plugin-hostd >= \([0-9.]*\)$/Recommends: (plugin-hostd >= \1 with plugin-hostd < 0.21)/' "$scratch/t/packaging/omx-plugins.spec" ;;
      control-dsp) sed -i '0,/libomxdsp-dev (>= [0-9.]*~)/s//libomxdsp-dev (>= 0.0.1~)/' "$scratch/t/debian/control" ;;
      control-ceiling-returns) sed -i '0,/libomxdsp-dev (>= 0.32.0~),/s//libomxdsp-dev (>= 0.32.0~), libomxdsp-dev (<< 0.33~),/' "$scratch/t/debian/control" ;;
      control-hostd-ceiling-returns) sed -i 's/^Recommends: plugin-hostd (>= \([0-9.]*\)~)$/Recommends: plugin-hostd (>= \1~), plugin-hostd (<< 0.21~)/' "$scratch/t/debian/control" ;;
      pc-ceiling-returns) sed -i 's/omxdsp >= @OMXDSP_MIN@/omxdsp >= @OMXDSP_MIN@, omxdsp < @OMXDSP_MAX@/' "$scratch/t/lib/omxplugins.pc.in" ;;
      contract-devel-ceiling-returns) sed -i 's/^BuildRequires: omx-contract-devel >= \([0-9.]*\)$/BuildRequires: (omx-contract-devel >= \1 with omx-contract-devel < 4)/' "$scratch/t/packaging/omx-plugins.spec" ;;
      devel-contract-ceiling-returns) sed -i 's/^Requires: omx-contract-devel >= \([0-9.]*\)$/Requires: (omx-contract-devel >= \1 with omx-contract-devel < 4)/' "$scratch/t/packaging/omx-plugins.spec" ;;
      panel-ceiling-returns) sed -i 's/^BuildRequires: omx-panel >= \([0-9.]*\)$/BuildRequires: (omx-panel >= \1 with omx-panel < 0.14)/' "$scratch/t/packaging/omx-plugins.spec" ;;
      control-contract-ceiling-returns) sed -i '0,/omx-contract (>= \([0-9.]*\)~)/s//omx-contract (>= \1~), omx-contract (<< 4~)/' "$scratch/t/debian/control" ;;
      control-panel-ceiling-returns) sed -i '0,/omx-panel (>= \([0-9.]*\)~)/s//omx-panel (>= \1~), omx-panel (<< 0.14~)/' "$scratch/t/debian/control" ;;
      spec-bare-ceiling-added) sed -i '/^BuildRequires: omx-contract >= /a BuildRequires: omx-contract < 4' "$scratch/t/packaging/omx-plugins.spec" ;;
      control-bare-ceiling-added) sed -i '/^Recommends: plugin-hostd (>= /a Suggests: omx-dsp-contract (<< 0.33~)' "$scratch/t/debian/control" ;;
      control-breaks-sibling-added) sed -i '/^Breaks: omx-clap-host /d; /^Package: omx-plugins-clap$/,/^Description:/s/^Description:/Breaks: omx-clap-host (<< 0.23.0~)\nDescription:/' "$scratch/t/debian/control" ;;
      spec-conflicts-sibling-added) sed -i '/^Conflicts: omx-clap-host /d; /^%package clap$/,/^%description clap$/s/^%description clap$/Conflicts: omx-clap-host < 0.23.0\n&/' "$scratch/t/packaging/omx-plugins.spec" ;;
      control-breaks-own-added) sed -i '/^Package: omx-plugins-clap$/,/^Description:/s/^Description:/Breaks: omx-plugins-dev (<< ${source:Version})\nDescription:/' "$scratch/t/debian/control" ;;
      control-replaces-added) sed -i '/^Package: omx-plugins-clap$/,/^Description:/s/^Description:/Replaces: omx-plugins-dev (<< ${source:Version})\nDescription:/' "$scratch/t/debian/control" ;;
      control-breaks-continued) sed -i '/^Package: omx-plugins-clap$/,/^Description:/s/^Description:/Breaks: omx-plugins-dev (<< ${source:Version}),\n omx-clap-host (<< 0.23.0~)\nDescription:/' "$scratch/t/debian/control" ;;
      spec-conflicts-own-added) sed -i '/^%package clap$/,/^%description clap$/s/^%description clap$/Conflicts: %{name}-devel < %{version}-%{release}\n&/' "$scratch/t/packaging/omx-plugins.spec" ;;
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
