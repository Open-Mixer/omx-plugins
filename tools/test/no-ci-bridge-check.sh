#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/test/no-ci-bridge-check.sh — the tree carries no CI bridge script and no call to one: tools/ci-stale-units.sh is
# absent, and no tracked file names it (a bridge expires, and a call left behind turns CI red the day it does).
# `make no-ci-bridge`, part of `make test`.
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
cd "$ROOT"
name=ci-stale
name=$name-units
bad=0
if [ -e "tools/$name.sh" ]; then echo "FAIL no-ci-bridge: tools/$name.sh is still there"; bad=$((bad + 1)); fi
# the line count of the names that remain, outside this check
while IFS= read -r hit; do
  echo "FAIL no-ci-bridge: $hit"; bad=$((bad + 1))
done < <(git grep -n -I -e "$name" -- . ':!tools/test/no-ci-bridge-check.sh' || true)
if [ "$bad" -ne 0 ]; then echo "no-ci-bridge: $bad reference(s) to the $name bridge" >&2; exit 1; fi
echo "PASS no-ci-bridge: no bridge script, no call"
