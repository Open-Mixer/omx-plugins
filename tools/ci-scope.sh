#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/ci-scope.sh < changed-paths — whether a pull request changes what `make selftest` (the recipe machinery,
# sabotaged arm by arm, over copies of the tree) exercises. Prints machinery=true or machinery=false.
#
# The selftest takes about 80 minutes because it sabotages the tool chain and the tree-wide checks. A change that is
# confined to the folders of plugins it never reads cannot move it, so it need not run again: false only when EVERY changed
# path is under plugins/<name>/ and <name> is not one of SELFTEST_PLUGINS, the plugins tools/test/recipe-selftest.mjs copies,
# drives or names (tools/test/ci-scope-check.sh keeps this list whole). Anything else, and an empty list, is true:
# main and tags never ask this at all.
set -euo pipefail
SELFTEST_PLUGINS=" omx-chainproof omx-comp omx-delay omx-delay9 omx-drive omx-eq omx-eq16 omx-eq32 omx-eq8 omx-eq9 omx-fader omx-gate omx-ghost omx-knob omx-strip omx-trem omx-tremolo omx-wobble "
n=0
while IFS= read -r path; do
  [ -n "$path" ] || continue
  n=$((n + 1))
  case "$path" in
    plugins/*/*)
      name=${path#plugins/}
      name=${name%%/*}
      case "$SELFTEST_PLUGINS" in *" $name "*) echo "machinery=true"; exit 0 ;; esac ;;
    *) echo "machinery=true"; exit 0 ;;
  esac
done
[ "$n" -gt 0 ] && echo "machinery=false" || echo "machinery=true"
