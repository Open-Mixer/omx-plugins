#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/test/ci-scope-check.sh — tools/ci-scope.sh scopes the selftest to what a change can move: a change confined to the
# folder of a plugin the selftest never reads is false, any other is true, and every plugin recipe-selftest.mjs names is
# in the script's list (a planted name that is missing would be false for it). `make ci-scope`, part of `make test`.
set -euo pipefail
HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
ROOT=$(cd "$HERE/.." && pwd)
scope=$HERE/ci-scope.sh
bad=0
expect() { # want label paths...
  local want=$1 label=$2 got; shift 2
  got=$(printf '%s\n' "$@" | "$scope")
  if [ "$got" = "machinery=$want" ]; then echo "PASS ci-scope: $label"; else echo "FAIL ci-scope: $label (got $got, wanted $want)"; bad=$((bad + 1)); fi
}
expect false "a plugin the selftest never reads" plugins/omx-reverb/omx-reverb.decl.json plugins/omx-reverb/Makefile
expect true "a plugin the selftest drives" plugins/omx-drive/omx-drive.decl.json
expect true "one such plugin among others" plugins/omx-reverb/Makefile plugins/omx-eq8/Makefile
expect true "the tools" tools/gen.mjs
expect true "a plugin folder and the tools" plugins/omx-reverb/Makefile tools/gen.mjs
expect true "the Makefile" Makefile
expect true "the workflow" .github/workflows/ci.yml
expect true "no path at all" ""
# every plugin of the tree that recipe-selftest.mjs names is in the list
for dir in "$ROOT"/plugins/*/; do
  name=$(basename "$dir")
  if grep -q "\b$name\b" "$HERE/test/recipe-selftest.mjs"; then
    got=$(printf 'plugins/%s/Makefile\n' "$name" | "$scope")
    if [ "$got" = "machinery=true" ]; then :; else echo "FAIL ci-scope: $name is named by recipe-selftest.mjs and not in SELFTEST_PLUGINS"; bad=$((bad + 1)); fi
  fi
done
[ "$bad" = 0 ] && echo "PASS ci-scope: all" || { echo "ci-scope: $bad failure(s)"; exit 1; }
