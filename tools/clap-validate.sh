#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# Runs clap-validator (the CLAP project's own conformance tester) on every .clap the build produced,
# the test plugins included, and keeps one JSON report per plugin.
#
#   tools/clap-validate.sh [OUTDIR]     reports to OUTDIR (default build/clap-validator)
#
# Exit status: 0 when every plugin passed (a plugin whose only failure is the one accepted in
# clap-validate-summary.mjs, activate refusing a rate outside the nine oracle rates, passes too, its
# accepted line printed), 1 when any other test failed or a plugin could not be run,
# 2 when the validator or the built plugins are missing (nothing was judged). The plugins' factory
# presets must be installed (make install-presets) or the preset-discovery tests find no location to crawl.
set -euo pipefail
cd "$(dirname "$0")/.."
out=${1:-build/clap-validator}
command -v clap-validator >/dev/null || { echo "clap-validate: clap-validator is not installed" >&2; exit 2; }
mapfile -t claps < <(find plugins -path '*/build/*.clap' -type f | sort)
[ "${#claps[@]}" -gt 0 ] || { echo "clap-validate: no built .clap under plugins/ (run make first)" >&2; exit 2; }
mkdir -p "$out"
clap-validator --version
# the verdict's one accepted failure is held to its exact shape before any report is read
tools/test/clap-validate-summary-test.sh || exit 1
bad=0
for f in "${claps[@]}"; do
  name=$(basename "$f" .clap)
  # the validator exits non-zero on a failed test; its report is read either way
  rc=0
  clap-validator validate --json "$f" >"$out/$name.json" 2>"$out/$name.err" || rc=$?
  # 3: the only failures are the one accepted (Open-Mixer/omx-plugins#91), so the validator's own non-zero exit is expected
  sum=0; node tools/clap-validate-summary.mjs "$name" "$out/$name.json" || sum=$?
  if [ "$sum" = 3 ]; then
    :
  elif [ "$sum" != 0 ]; then
    bad=1
  elif [ "$rc" != 0 ]; then
    echo "FAIL $name: clap-validator exited $rc with no failed test in its report" >&2
    bad=1
  fi
done
echo "clap-validate: ${#claps[@]} plugins, reports in $out"
exit "$bad"
