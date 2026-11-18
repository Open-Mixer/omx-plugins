#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas
#
# tools/test/clap-validate-summary-test.sh — the clap-validator verdict accepts exactly one failure:
# process-varying-sample-rates' "activate() returned false" at a rate OUTSIDE omx-contract's
# ORACLE_FULL_RATES (the kernels run only at those nine rates by design, Open-Mixer/omx-plugins#91).
# Any other failure, and that refusal AT one of the nine, stays red.
#   tools/test/clap-validate-summary-test.sh
set -uo pipefail
HERE=$(cd "$(dirname "$0")/../.." && pwd)
d=$(mktemp -d); trap 'rm -rf "$d"' EXIT
report() { # <file> <test> <details>
  printf '{"results":[{"test":{"plugin-instance":{"test":"ok-test"}},"status":{"code":"success"}},{"test":{"plugin-instance":{"test":"%s"}},"status":{"code":"failed","details":"%s"}}]}\n' "$2" "$3" > "$1"
}
act() { # <rate>: clap-validator's line for an activate() refused at that rate
  printf "Error while processing with %s sample rate 'clap_plugin::activate()' returned false." "$1"
}
report "$d/off-grid.json" process-varying-sample-rates "$(act 8000.00hz)"
report "$d/on-grid.json" process-varying-sample-rates "$(act 48000.00hz)"
report "$d/other-test.json" process-audio-out-of-place-basic "$(act 8000.00hz)"
report "$d/other-failure.json" process-varying-sample-rates "Error while processing with 8000.00hz sample rate: the output holds NaN"
bad=0
want() { # <expected rc class: pass|red> <description> <report>
  node "$HERE/tools/clap-validate-summary.mjs" t "$3" >"$d/out" 2>&1; rc=$?
  case "$1:$rc" in pass:3|red:1) echo "ok   $2" ;; *) echo "FAIL $2: rc $rc"; sed 's/^/     /' "$d/out"; bad=1 ;; esac
}
want pass "a refusal at 8 kHz (outside the nine) in process-varying-sample-rates is accepted" "$d/off-grid.json"
want red  "a refusal at 48 kHz (one of the nine) stays red" "$d/on-grid.json"
want red  "the same refusal in another test stays red" "$d/other-test.json"
want red  "another failure at 8 kHz in process-varying-sample-rates stays red" "$d/other-failure.json"
[ "$bad" = 0 ] && echo "clap-validate-summary-test: green" || echo "clap-validate-summary-test: RED"
exit "$bad"
