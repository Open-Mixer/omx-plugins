#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
# Refuses a hand-written DEB job whose cells are not check-matrix's: a job that builds with the build-deb action runs on
# its cell's own runner (runs-on: ${{ matrix.cell.runner }}) and takes its cells from Open-Mixer/.github's check-matrix,
# never from an arch list of its own. A container of one architecture cannot install the packages of another, and the
# channel's development debs are trixie arm64 only: an amd64 cell on an amd64 runner finds libomxdsp-dev and
# libomx-clap-core-dev not installable (omx-plugins ci 44594367073), and an arm64 debian container on an amd64 runner
# has apt pick libomxdsp-dev arm64 (omx-panel ci 44563540081). check-matrix's quick set holds no amd64 deb cell.
# Usage: workflow-deb-cells-check.sh [repo-root] (default: .), or --self-test, which runs this check on planted trees with
# each shape, a missing directory and a tree with no DEB job included. Exit 0 clean, 1 a hit (each job named), 2 no
# .github/workflows to check, or no DEB job in them. `make deb-cells`, part of `make test`.
set -eu
self=$0

check() {
	local wf=$1/.github/workflows
	if [ ! -d "$wf" ]; then
		echo "workflow-deb-cells-check: no $wf to check (refusing: an empty check proves nothing)" >&2
		return 2
	fi
	# one line per DEB job: file:line job, then BAD:<why> lines; a DEB job is one with a build-deb action step
	local out
	out=$(find "$wf" -type f \( -name '*.yml' -o -name '*.yaml' \) -exec awk '
		function flush() {
			if (job != "" && deb) {
				print "DEB " FILENAME ":" at " " job
				if (runs != "${{ matrix.cell.runner }}") print "BAD " FILENAME ":" at " " job ": runs-on is \"" runs "\", not ${{ matrix.cell.runner }}"
				if (arch) print "BAD " FILENAME ":" arch " " job ": an arch list of its own in the matrix, not check-matrix cells"
				if (!cell) print "BAD " FILENAME ":" at " " job ": its matrix has no check-matrix cell (cell: ${{ fromJSON(...) }})"
			}
			job = ""; deb = 0; runs = ""; arch = 0; cell = 0
		}
		FNR == 1 { flush(); injobs = 0 }
		/^jobs:[[:space:]]*$/ { injobs = 1; next }
		/^[^[:space:]#]/ { flush(); injobs = 0; next }
		injobs && /^  [A-Za-z0-9_-]+:[[:space:]]*$/ { flush(); job = $1; sub(/:$/, "", job); at = FNR; next }
		job == "" { next }
		/^[[:space:]]*#/ { next }
		/\/actions\/build-deb@/ { deb = 1 }
		/^    runs-on:/ { r = $0; sub(/^    runs-on:[[:space:]]*/, "", r); sub(/[[:space:]]+$/, "", r); runs = r }
		/^[[:space:]]+arch:/ { arch = FNR }
		/^[[:space:]]+cell:[[:space:]]*\$\{\{[[:space:]]*fromJSON\(/ { cell = 1 }
		END { flush() }
	' {} +)
	if ! grep -q '^DEB ' <<<"$out"; then
		echo "workflow-deb-cells-check: no DEB job (a build-deb action step) in $wf (refusing: an empty check proves nothing)" >&2
		return 2
	fi
	if grep -q '^BAD ' <<<"$out"; then
		sed -n 's/^BAD //p' <<<"$out"
		echo "workflow-deb-cells-check: a DEB job not on check-matrix cells (runs-on: \${{ matrix.cell.runner }}, cell: \${{ fromJSON(needs.<job>.outputs.deb) }})" >&2
		return 1
	fi
	echo "workflow-deb-cells-check: every DEB job runs on its check-matrix cell's runner ($(grep -c '^DEB ' <<<"$out") job(s))"
}

if [ "${1:-}" = "--self-test" ]; then
	t=$(mktemp -d)
	trap 'rm -rf "$t"' EXIT
	fails=0
	plant() { # name, expected rc, yaml body
		local d=$t/$1
		mkdir -p "$d/.github/workflows"
		printf '%s\n' "$3" > "$d/.github/workflows/ci.yml"
		local rc=0
		"$self" "$d" >/dev/null 2>&1 || rc=$?
		if [ "$rc" != "$2" ]; then echo "self-test FAIL $1: rc $rc, want $2" >&2; fails=$((fails + 1)); fi
	}
	good='jobs:
  deb-cells:
    runs-on: ubuntu-latest
    steps:
      - uses: Open-Mixer/.github/.github/actions/check-matrix@openmixer-v1
  deb:
    needs: deb-cells
    runs-on: ${{ matrix.cell.runner }}
    strategy:
      matrix:
        cell: ${{ fromJSON(needs.deb-cells.outputs.deb) }}
    container:
      image: ${{ matrix.cell.image }}
    steps:
      - uses: Open-Mixer/.github/.github/actions/build-deb@openmixer-v1'
	plant clean 0 "$good"
	cellrun='runs-on: ${{ matrix.cell.runner }}'
	fixed='runs-on: ${{ vars.OMX_RUNNER || '"'ubuntu-latest'"' }}'
	cells='cell: ${{ fromJSON(needs.deb-cells.outputs.deb) }}'
	plant fixedrunner 1 "${good/"$cellrun"/"$fixed"}"
	plant archlist 1 "${good/"$cells"/"$cells
        arch: [amd64]"}"
	plant handmatrix 1 "${good/"$cells"/"suite: [trixie]"}"
	plant nodeb 2 'jobs:
  rpm:
    runs-on: ubuntu-latest
    steps:
      - run: true'
	plant commented 2 'jobs:
  deb:
    runs-on: ubuntu-latest
    steps:
      # - uses: Open-Mixer/.github/.github/actions/build-deb@openmixer-v1
      - run: true'
	mkdir -p "$t/nodir"
	rc=0; "$self" "$t/nodir" >/dev/null 2>&1 || rc=$?
	if [ "$rc" != 2 ]; then echo "self-test FAIL missing dir: rc $rc, want 2" >&2; fails=$((fails + 1)); fi
	if [ "$fails" != 0 ]; then echo "workflow-deb-cells-check: self-test $fails red" >&2; exit 1; fi
	echo "workflow-deb-cells-check: self-test green (7 planted trees)"
	exit 0
fi

rc=0
check "${1:-.}" || rc=$?
exit "$rc"
