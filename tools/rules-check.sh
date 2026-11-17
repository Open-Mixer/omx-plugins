#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas
#
# rules-check.sh: the DEB build runs the generator checks as the RPM build does. The omx-contract command is a
# Build-Depends (packaging/deps.txt), so debian/rules has no branch that skips them and runs make test.
#
#   tools/rules-check.sh              the check
#   tools/rules-check.sh --self-test  each sabotage must go red
# shellcheck disable=SC2016  # the sed scripts and the pattern are literal, not shell
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"

check() {
  local rules="$1/debian/rules" fail=0
  if grep -q 'NO-CONTRACT-COMMAND\|make test-faces' "$rules"; then
    echo "rules-check: debian/rules skips the generator checks where the omx-contract command is absent: it is a Build-Depends, run make test" >&2; fail=1
  fi
  grep -q 'make test\b\|\$(MAKE) test OMX_LV2_SPEC_DIR' "$rules" || { echo "rules-check: debian/rules does not run make test" >&2; fail=1; }
  return "$fail"
}

if [ "${1:-}" = --self-test ]; then
  scratch="$(mktemp -d)"; trap 'rm -rf "$scratch"' EXIT
  fail=0
  for s in rules-skip rules-no-test; do
    rm -rf "$scratch/t"; mkdir -p "$scratch/t/debian"; cp "$HERE/debian/rules" "$scratch/t/debian/"
    case "$s" in
      rules-skip) sed -i 's/^override_dh_auto_test:/&\n\t@echo NO-CONTRACT-COMMAND; $(MAKE) test-faces; exit 0/' "$scratch/t/debian/rules" ;;
      rules-no-test) sed -i 's/make test\b/make all/; s/\$(MAKE) test OMX_LV2_SPEC_DIR/$(MAKE) all/' "$scratch/t/debian/rules" ;;
    esac
    if check "$scratch/t" >/dev/null 2>&1; then echo "SABOTAGE $s: STAYED GREEN"; fail=1; else echo "SABOTAGE $s: red"; fi
  done
  check "$HERE" || { echo "self-test: the unsabotaged check is not green" >&2; exit 1; }
  [ "$fail" = 0 ] || exit 1
  echo "rules-check self-test: every sabotage red"
  exit 0
fi

check "$HERE" && echo "rules-check: debian/rules runs make test and skips nothing"
