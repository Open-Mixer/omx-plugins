#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/test/lv2bench-exit-check.sh — lv2bench's own exit status is not a per-plugin signal: it mixes every
# plugin on LV2_PATH into one number. Reproduced directly on this host (Fedora lilv 0.26.4): a working
# omx-eq8.lv2 staged beside a bundle whose .so is missing makes lv2bench exit 3, yet eq8's own line is still
# printed, correctly. tools/zynthian-bundle.sh must not let that exit status gate the per-URI check
# tools/lv2bench-ran.sh already makes (`bench=$(... lv2bench) || true`): a nonzero exit alongside a URI that
# did run is a pass. No Debian-lilv binary is on this host to read its exit code the same way, so that shape
# is exercised with a representative nonzero code; the fix tolerates any nonzero code identically in both
# shapes, so this still proves the pattern holds for the Debian output too. `make lv2bench-exit`, part of
# `make test`.
set -euo pipefail
HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
ran=$HERE/lv2bench-ran.sh
bad=0

# what tools/zynthian-bundle.sh's `bench=$(LV2_PATH=... lv2bench) || true` captures: the output stands
# whether or not the process itself exited clean.
fake_lv2bench() { printf '%s' "$2"; return "$1"; }

check() { # shape exit-code output uri
  local out rc=0
  out=$(fake_lv2bench "$2" "$3") || rc=$?
  [ "$rc" = "$2" ] || { echo "FAIL lv2bench-exit: $1: fake_lv2bench itself did not exit $2 (got $rc)"; bad=$((bad + 1)); return; }
  if printf '%s' "$out" | "$ran" "$4"; then
    echo "PASS lv2bench-exit: $1: a nonzero lv2bench exit does not hide a URI that ran"
  else
    echo "FAIL lv2bench-exit: $1: $4 read as not run, though its line is in the output"
    bad=$((bad + 1))
  fi
}

# Debian shape, a representative nonzero exit (a sibling plugin failed while chorus's own line ran)
check debian 1 $'0.014597676 urn:openmixer:chorus\n' urn:openmixer:chorus
# Fedora lilv 0.26.4 shape, exit 3 as reproduced directly against a broken sibling bundle (see comment above)
check fedora 3 $'Block\tFrames\tRate\tMin\tMean\tMax\tTotal\tPlugin\n512\t524288\t48000.000000\t6.1e-08\t7.43e-08\t1.3e-05\t0.00011\turn:openmixer:eq8\n' urn:openmixer:eq8

[ "$bad" = 0 ] && echo "PASS lv2bench-exit: both output shapes" || { echo "lv2bench-exit: $bad failure(s)"; exit 1; }
