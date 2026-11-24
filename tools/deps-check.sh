#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# deps-check.sh — no version relation on a package of ours is typed by hand (openmixer-ops#661): not in
# packaging/omx-plugins.spec, debian/control, lib/omxplugins.pc.in, the Makefiles or their template. A build uses what is
# installed; a relation that must exist is generated (soname dependencies, `= %{version}-%{release}`). A third party's
# floor (CLAP, Node.js) stays, with no ceiling. Also held here: no Breaks, Replaces, Conflicts or Obsoletes between our
# packages, the recommendations of omx-panel and plugin-hostd, and debian/rules running make test.
#
#   tools/deps-check.sh              the check
#   tools/deps-check.sh --self-test  each sabotage must go red
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
OURS='((lib)?(omx|plugin)-[a-z0-9-]+|libomx[a-z0-9-]+)'

say() { echo "deps-check: $1" >&2; fail=1; }
check() {
  local root="$1" fail=0 spec="$1/packaging/omx-plugins.spec" control="$1/debian/control" hits
  hits=$(sed '/^%changelog/q' "$spec" | grep -nE "^(BuildRequires|Requires|Recommends|Suggests)[(:].*$OURS[^ ]* *(>=|<=?|=) *[0-9]" || true)
  [ -z "$hits" ] || say "the spec types a version of a package of ours: $hits"
  hits=$(tr ',' '\n' < "$control" | grep -nE "$OURS \((>=|<<|<=|=) *[0-9]" || true)
  [ -z "$hits" ] || say "debian/control types a version of a package of ours: $hits"
  hits=$(grep -nE '^Requires:.*(>=|<|=)' "$root/lib/omxplugins.pc.in" || true)
  [ -z "$hits" ] || say "omxplugins.pc.in types a version: $hits"
  hits=$(cd "$root" && grep -nE 'atleast-version|OMXDSP_MIN' Makefile recipes/templates/plugin/Makefile.tmpl plugins/*/Makefile || true)
  [ -z "$hits" ] || say "a Makefile checks a version: $hits"
  hits=$(cd "$root" && grep -lE '"requires": \{|"omx-contract-schema"' contract/*.json plugins/*/*.descriptor.json || true)
  [ -z "$hits" ] || say "a requires is a map of floors: $hits"
  # no ceiling on any dependency, a third party's included
  if sed '/^%changelog/q' "$spec" | grep -E '^(BuildRequires|Requires|Recommends|Suggests)[(:].*<' > /dev/null; then say "the spec states a ceiling (<) on a dependency"; fi
  if grep -vE '^(Breaks|Replaces|Conflicts):' "$control" | grep '(<' > /dev/null; then say "debian/control states a ceiling (<< or <) on a dependency"; fi
  # no Breaks, Replaces or Conflicts between any of our packages, a package of this source included
  if grep -qE '^(Breaks|Replaces|Conflicts):' "$control"; then say "debian/control has a Breaks, Replaces or Conflicts field"; fi
  if sed '/^%changelog/q' "$spec" | grep -E '^(Conflicts|Obsoletes):' > /dev/null; then say "the spec has a Conflicts or Obsoletes line"; fi
  # the LV2 package recommends omx-panel, so omx-modgui-sync (the host's MOD GUI generator) is there; plugin-hostd at run time
  sed -n '/^%package lv2$/,/^%description lv2$/p' "$spec" | grep -x 'Recommends: omx-panel' > /dev/null || say "the spec's lv2 package does not recommend omx-panel (omx-modgui-sync)"
  sed -n '/^Package: omx-plugins-lv2$/,/^Description:/p' "$control" | grep -x 'Recommends: omx-panel' > /dev/null || say "debian/control's omx-plugins-lv2 does not recommend omx-panel (omx-modgui-sync)"
  grep -qx 'BuildRequires: plugin-hostd' "$spec" || say "the spec does not build against plugin-hostd"
  grep -qx 'Recommends: plugin-hostd' "$spec" || say "the spec does not recommend plugin-hostd"
  grep -qx 'Recommends: plugin-hostd' "$control" || say "debian/control does not recommend plugin-hostd"
  # the DEB build runs the generator checks as the RPM build does: the omx-contract command is a build dependency
  if grep -q 'NO-CONTRACT-COMMAND\|make test-faces' "$root/debian/rules"; then say "debian/rules skips the generator checks where the omx-contract command is absent: it is a Build-Depends, run make test"; fi
  grep -q 'make test\b\|\$(MAKE) test OMX_LV2_SPEC_DIR' "$root/debian/rules" || say "debian/rules does not run make test"
  return "$fail"
}

if [ "${1:-}" = --self-test ]; then
  scratch="$(mktemp -d)"; trap 'rm -rf "${scratch:?}"' EXIT
  fail=0
  for s in spec-floor spec-ceiling control-floor pc-floor makefile-check map-requires breaks obsoletes panel-recommends hostd-recommends rules-skip; do
    t="$scratch/$s"; mkdir -p "$t"
    (cd "$HERE" && tar -cf - packaging debian lib/omxplugins.pc.in Makefile recipes/templates contract plugins/*/Makefile plugins/*/*.descriptor.json) | tar -xf - -C "$t"
    case "$s" in
      spec-floor) sed -i 's/^BuildRequires: omx-dsp-devel$/BuildRequires: omx-dsp-devel >= 0.34.0/' "$t/packaging/omx-plugins.spec" ;;
      spec-ceiling) sed -i 's/^BuildRequires: make$/BuildRequires: (make with make < 9)/' "$t/packaging/omx-plugins.spec" ;;
      control-floor) sed -i 's/libomxdsp-dev,/libomxdsp-dev (>= 0.34.0~),/' "$t/debian/control" ;;
      pc-floor) sed -i 's/^Requires: omxdsp,/Requires: omxdsp >= 0.34.0,/' "$t/lib/omxplugins.pc.in" ;;
      makefile-check) sed -i 's/--exists omxdsp/--atleast-version=0.34.0 omxdsp/' "$t/plugins/omx-eq8/Makefile" ;;
      map-requires) sed -i '1a "requires": {' "$t/contract/plugin-face.json" ;;
      breaks) sed -i 's/^Package: omx-plugins-lv2$/&\nBreaks: omx-panel/' "$t/debian/control" ;;
      obsoletes) sed -i 's/^Name:.*/&\nObsoletes: omx-x/' "$t/packaging/omx-plugins.spec" ;;
      panel-recommends) sed -i '/^Recommends: omx-panel$/d' "$t/packaging/omx-plugins.spec" ;;
      hostd-recommends) sed -i '/^Recommends: plugin-hostd$/d' "$t/debian/control" ;;
      rules-skip) sed -i '$a # make test-faces' "$t/debian/rules" ;;
    esac
    if (cd "$t" && find . -type f -exec cmp -s {} "$HERE/{}" \; -o -type f -print) > "$scratch/planted"; [ -s "$scratch/planted" ]; then :; else echo "SABOTAGE $s: PLANTED NOTHING"; fail=1; fi
    if (check "$t") >/dev/null 2>&1; then echo "SABOTAGE $s: STAYED GREEN"; fail=1; else echo "SABOTAGE $s: red"; fi
  done
  (check "$HERE") >/dev/null || { echo "self-test: the unsabotaged check is not green" >&2; exit 1; }
  [ "$fail" = 0 ] || exit 1
  echo "deps-check self-test: every sabotage red"
  exit 0
fi

check "$HERE"
echo "deps-check: no version of a package of ours is typed"
