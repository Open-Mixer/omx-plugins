#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas
#
# tools/test/no-dated-bridge-check.sh — no CI bridge that waits for an older package to leave the channel: no script that
# refuses to run after an EXPIRES date (it reds CI on that day, wherever it is called), no call to the old
# tools/ci-stale-units.sh, and no package install that allows a downgrade. Development carries no backward-compatibility
# machinery (owner rule, 2026-11-16). A planted dated bridge must go red (the sabotage arm). `make no-dated-bridge`,
# part of `make test`.
set -euo pipefail
HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)

scan() { # root: prints one line per offence
  ( cd "$1" && git grep -nE '^[[:space:]]*EXPIRES=|ci-stale-units|--allow-downgrades' -- . \
      ':!tools/test/no-dated-bridge-check.sh' || true )
}

bad=0
hits=$(scan "$HERE")
if [ -n "$hits" ]; then
  echo "FAIL no-dated-bridge: a dated CI bridge or a downgrade allowance is still in the tree:"
  sed 's/^/  /' <<<"$hits"
  bad=1
else
  echo "PASS no-dated-bridge: no dated bridge, no stale-units call, no downgrade allowance"
fi

# sabotage: a planted script that expires must be found
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
git -C "$tmp" init -q
printf '#!/bin/sh\nEXPIRES=2026-11-20\n' > "$tmp/bridge.sh"
git -C "$tmp" add bridge.sh
if [ -n "$(scan "$tmp")" ]; then
  echo "PASS no-dated-bridge: sabotage, a planted EXPIRES bridge is found"
else
  echo "FAIL no-dated-bridge: sabotage, a planted EXPIRES bridge stayed hidden"
  bad=1
fi
exit "$bad"
