#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
# no-host-nproc.sh [DIR] — no workflow sizes its parallel jobs from nproc. Inside a job's container nproc counts the
# HOST's cores, and one make -j24 on a node of fourteen three-CPU runners swamps it: a heavy job takes its count from
# Open-Mixer/.github's jobs action (the cgroup's CPU quota, bounded by the runner's declared CPUs) and builds with
# make -j"$OMX_JOBS". --self-test proves the scan sees a planted nproc, passes a clean file and refuses (exit 2) a
# directory with no workflow.
set -eu
scan() { grep -nE 'nproc|getconf +_NPROCESSORS' "$1"/*.yml 2>/dev/null | grep -vE '^[^:]+:[0-9]+: *#' || true; }
if [ "${1:-}" = --self-test ]; then
  t=$(mktemp -d); trap 'rm -rf "$t"' EXIT
  printf 'jobs:\n  a:\n    steps:\n      - run: make -j"$(nproc)" test\n' > "$t/bad.yml"
  [ -n "$(scan "$t")" ] || { echo "no-host-nproc: FAIL the scan does not see a planted make -j\"\$(nproc)\"" >&2; exit 1; }
  printf 'jobs:\n  a:\n    steps:\n      - run: make -j"$OMX_JOBS" test\n' > "$t/bad.yml"
  [ -z "$(scan "$t")" ] || { echo "no-host-nproc: FAIL the scan refuses a clean make -j\"\$OMX_JOBS\"" >&2; exit 1; }
  rm -f "$t/bad.yml"; rc=0; sh "$0" "$t" >/dev/null 2>&1 || rc=$?
  [ "$rc" = 2 ] || { echo "no-host-nproc: FAIL a directory with no workflow answers $rc, not 2" >&2; exit 1; }
  echo "no-host-nproc: self-test ok"; exit 0
fi
dir=${1:-.github/workflows}
# an empty scan proves nothing: a directory with no workflow is refused, never passed
ls "$dir"/*.yml >/dev/null 2>&1 || { echo "no-host-nproc: no workflow (*.yml) in $dir to scan" >&2; exit 2; }
found=$(scan "$dir")
[ -z "$found" ] || { echo "no-host-nproc: these steps size their jobs from the host's nproc; use Open-Mixer/.github's jobs action and make -j\"\$OMX_JOBS\":" >&2; echo "$found" | sed 's/^/  /' >&2; exit 1; }
echo "no-host-nproc: no workflow sizes its jobs from nproc"
