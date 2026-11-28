#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/test/release-same-pass-check.sh — the release workflow reads the same-pass list exactly as plugin-hostd does:
# a dispatch input same_pass, else .github/release-same-pass at the built commit (a samepass job), and that list goes to
# EVERY shared build call (build-rpm.yml, build-deb.yml). A call left without it reports a same-pass breakage as red.
# `make release-same-pass`, part of `make test`.
set -euo pipefail
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
cd "$ROOT"
f=.github/workflows/release.yml
bad=0
fail() { echo "FAIL release-same-pass: $1"; bad=$((bad + 1)); }
grep -q '^  samepass:' "$f" || fail "no samepass job in $f"
grep -q 'f=.github/release-same-pass' "$f" || fail "$f does not read .github/release-same-pass"
grep -qE '^      same_pass:' "$f" || fail "no same_pass dispatch input in $f"
calls=$(grep -c 'uses: Open-Mixer/\.github/\.github/workflows/' "$f" || true)
passes=$(grep -cF 'same-pass: ${{ inputs.same_pass || needs.samepass.outputs.list }}' "$f" || true)
if [ "$calls" -eq 0 ]; then fail "no shared build call in $f"; fi
if [ "$passes" -ne "$calls" ]; then fail "$passes of $calls shared build calls pass the same-pass list"; fi
if [ "$bad" -ne 0 ]; then echo "release-same-pass: $bad problem(s) in $f" >&2; exit 1; fi
echo "PASS release-same-pass: samepass job, dispatch input, $calls of $calls shared calls pass the list"
