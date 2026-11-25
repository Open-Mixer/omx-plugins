#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/from-channel-check.sh — an ordinary CI run builds against the OpenMixer channel, never staging (D25,
# plugin-hostd-protocol#27: a stale staged contract package outranked the channel). Lists every installed omx-* and
# plugin-* package with the repository dnf installed it from, and refuses one from any repository but the channel's.
#
#   tools/from-channel-check.sh              the check, on this machine's installed packages (dnf repoquery)
#   tools/from-channel-check.sh --self-test  planted package lists, each answered as expected
# Exit 0 clean, 1 a package from outside the channel (each named), 2 no omx package installed (an empty check proves nothing).
# Called after dnf builddep by ci.yml's test, test-heavy and selftest jobs; `make from-channel` runs the self-test.
set -euo pipefail

# stdin: "name version repo" lines; prints each refused line, returns 1 on any, 2 on no line at all
judge() {
  local list bad
  list=$(cat)
  [ -n "$list" ] || { echo "from-channel-check: no omx-* or plugin-* package installed (refusing: an empty check proves nothing)" >&2; return 2; }
  # the channel is the repositories openmixer and openmixer-noarch (openmixer-release's .repo), and on a dedupe/all run
  # wave-deps, the siblings that wave-deps action built from their wave branch; anything else, staging (openmixer-staging*,
  # add-openmixer's add-staging.sh), testing (openmixer-testing*, the release package's) or a local file, is refused
  bad=$(awk '$3 != "openmixer" && $3 != "openmixer-noarch" && $3 != "wave-deps"' <<<"$list")
  if [ -n "$bad" ]; then
    printf '%s\n' "$bad"
    echo "from-channel-check: a package above came from outside the channel (openmixer, openmixer-noarch, wave-deps): an ordinary run builds against the channel" >&2
    return 1
  fi
  echo "from-channel-check: $(wc -l <<<"$list") omx package(s), each from the channel"
}

if [ "${1:-}" = --self-test ]; then
  fails=0
  plant() { # name, expected rc, list
    local rc=0
    printf '%s' "$3" | judge >/dev/null 2>&1 || rc=$?
    if [ "$rc" = "$2" ]; then echo "self-test $1: rc $rc"; else echo "self-test FAIL $1: rc $rc, want $2" >&2; fails=$((fails + 1)); fi
  }
  plant channel 0 $'omx-dsp-devel 0.34.1 openmixer\nomx-contract 3.20.2 openmixer-noarch\n'
  plant wave-branch 0 $'omx-dsp-devel 0.35.0 wave-deps\nomx-contract 3.20.2 openmixer-noarch\n'
  plant add-staging 1 $'omx-dsp-devel 0.34.1 openmixer\nomx-contract 3.21.0 openmixer-staging-noarch\n'
  # the release package's own staging repositories, disabled until enabled by hand (openmixer-release's .repo)
  plant release-testing 1 $'omx-dsp-devel 0.35.0 openmixer-testing\nomx-contract 3.20.2 openmixer-noarch\n'
  plant release-testing-noarch 1 $'omx-contract 3.21.0 openmixer-testing-noarch\n'
  plant local-rpm 1 $'omx-dsp-devel 0.35.0 @commandline\n'
  plant nothing-installed 2 ''
  [ "$fails" = 0 ] || exit 1
  echo "from-channel-check self-test: 7 planted lists, each answered as expected"
  exit 0
fi

dnf repoquery --installed --qf '%{name} %{version} %{from_repo}\n' 'omx-*' 'plugin-*' | tee /dev/stderr | judge
