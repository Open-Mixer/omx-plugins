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
# OMX_WAVE (a wave's devel channel, devel/<wave>, set on a wave's run) also admits that wave's devel channel and staging,
# and OMX_DEVEL_BASE (the wave's base waves, devel/<wave>...) the repositories add-openmixer adds for each of them.
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
  # a wave's run (OMX_WAVE, the wave's devel channel, Open-Mixer/.github#329) builds against that wave's devel channel
  # and staging beneath it, as its merge to devel will: openmixer-devel* and openmixer-staging* are expected there alone
  # and a wave one step behind another (OMX_DEVEL_BASE, add-openmixer's devel-base, devel/<wave> words split by spaces or
  # commas) also reads each base wave named there, as openmixer-base-<wave> and openmixer-base-<wave>-noarch, and no other
  local wave=${OMX_WAVE:-} bases='' b
  if [ -n "$wave" ]; then
    for b in ${OMX_DEVEL_BASE//,/ }; do bases+=" openmixer-base-${b#devel/} openmixer-base-${b#devel/}-noarch"; done
  fi
  bad=$(awk -v wave="$wave" -v bases="$bases " '$3 != "openmixer" && $3 != "openmixer-noarch" && $3 != "wave-deps" &&
    !(wave != "" && ($3 == "openmixer-devel" || $3 == "openmixer-devel-noarch" || $3 == "openmixer-staging" || $3 == "openmixer-staging-noarch" ||
      index(bases, " " $3 " ")))' <<<"$list")
  if [ -n "$bad" ]; then
    printf '%s\n' "$bad"
    echo "from-channel-check: a package above came from outside the channel (openmixer, openmixer-noarch, wave-deps${wave:+, and for the wave $wave its devel channel, staging${bases:+ and its base waves (${bases# })}}): an ordinary run builds against the channel" >&2
    return 1
  fi
  echo "from-channel-check: $(wc -l <<<"$list") omx package(s), each from the channel${wave:+, the devel channel of $wave or staging}"
}

if [ "${1:-}" = --self-test ]; then
  fails=0
  plant() { # name, expected rc, list[, wave[, bases]]
    local rc=0
    printf '%s' "$3" | OMX_WAVE=${4:-} OMX_DEVEL_BASE=${5:-} judge >/dev/null 2>&1 || rc=$?
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
  # a wave's run: its devel channel and staging are expected; testing and a local file are still refused, and without
  # the wave its devel channel is refused like staging
  plant wave-devel-and-staging 0 $'omx-clap-libs 0.36.0 openmixer-devel\nomx-contract 0.24.0 openmixer-devel-noarch\nomx-dsp-devel 0.34.3 openmixer-staging\n' devel/wave-a
  plant wave-testing 1 $'omx-clap-libs 0.36.0 openmixer-devel\nomx-contract 3.21.0 openmixer-testing-noarch\n' devel/wave-a
  plant wave-local-rpm 1 $'omx-dsp-devel 0.35.0 @commandline\n' devel/wave-a
  plant devel-without-wave 1 $'omx-clap-libs 0.36.0 openmixer-devel\n'
  # a wave one step behind another (OMX_DEVEL_BASE, add-openmixer's devel-base): the base wave's repositories
  # (openmixer-base-<wave>, -noarch) are expected for the bases named there alone, and never without the wave
  # (devel wave 2026-11-27-n3: omx-clap 0.36.0 came from its base bc2 and every test job refused it)
  plant wave-base 0 $'omx-clap-libs 0.36.0 openmixer-base-devel-wave-b\nomx-clap-contract 0.36.0 openmixer-base-devel-wave-b-noarch\nomx-dsp-devel 0.34.4 openmixer-devel\n' devel/wave-a devel/devel-wave-b
  plant wave-other-base 1 $'omx-clap-libs 0.36.0 openmixer-base-devel-wave-c\n' devel/wave-a devel/devel-wave-b
  plant base-without-wave 1 $'omx-clap-libs 0.36.0 openmixer-base-devel-wave-b\n' '' devel/devel-wave-b
  plant wave-base-unnamed 1 $'omx-clap-libs 0.36.0 openmixer-base-devel-wave-b\n' devel/wave-a
  [ "$fails" = 0 ] || exit 1
  echo "from-channel-check self-test: 15 planted lists, each answered as expected"
  exit 0
fi

dnf repoquery --installed --qf '%{name} %{version} %{from_repo}\n' 'omx-*' 'plugin-*' | tee /dev/stderr | judge
