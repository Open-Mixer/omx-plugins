#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
# Refuses a release.yml whose push builds part of the matrix: every package in every cell (owner ruling 2026-11-26),
# so the arch-matrix a push takes names both architectures of its format, amd64 and arm64 for a deb, x86_64 and
# aarch64 for an rpm. The value a push takes is the one after the last `||` of an expression, or the whole literal.
# A deb job built for arm64 alone left a trixie amd64 host nothing to install but omx-plugins-clap:arm64, and apt
# failed on libc6:arm64 (plugin-hostd's own-host page, trixie amd64, in the devel wave).
# Usage: workflow-full-matrix-check.sh [repo-root] (default: .), or --self-test, which runs this check on planted trees
# with each shape, a missing file and a release.yml with no arch-matrix included. Exit 0 clean, 1 a hit (each line
# named), 2 no release.yml or no arch-matrix in it. `make full-matrix`, part of `make test`.
set -eu
self=$0

check() {
	local f=$1/.github/workflows/release.yml
	if [ ! -f "$f" ]; then
		echo "workflow-full-matrix-check: no $f to check (refusing: an empty check proves nothing)" >&2
		return 2
	fi
	local out
	out=$(awk '
		/^[[:space:]]*arch-matrix:/ {
			n++
			v = $0; sub(/^[[:space:]]*arch-matrix:[[:space:]]*/, "", v)
			push = v; i = 0
			while ((j = index(substr(push, i + 1), "||")) > 0) i += j + 1
			push = substr(push, i + 1)
			split("amd64 arm64|x86_64 aarch64", formats, "|")
			for (k = 1; k <= 2; k++) {
				split(formats[k], a, " ")
				named = (v ~ ("\"" a[1] "\"") || v ~ ("\"" a[2] "\""))
				miss = ""
				if (push !~ ("\"" a[1] "\"")) miss = miss " " a[1]
				if (push !~ ("\"" a[2] "\"")) miss = miss " " a[2]
				if (named && miss != "") print "BAD " FILENAME ":" FNR ": a push builds without" miss
			}
		}
		END { print "COUNT " n + 0 }
	' "$f")
	if grep -q '^COUNT 0$' <<<"$out"; then
		echo "workflow-full-matrix-check: no arch-matrix in $f (refusing: an empty check proves nothing)" >&2
		return 2
	fi
	if grep -q '^BAD ' <<<"$out"; then
		sed -n 's/^BAD //p' <<<"$out"
		echo "workflow-full-matrix-check: a push releases the full matrix, every package in every cell (owner 2026-11-26)" >&2
		return 1
	fi
	echo "workflow-full-matrix-check: every arch-matrix a push takes is the full matrix ($(sed -n 's/^COUNT //p' <<<"$out") of them)"
}

if [ "${1:-}" = "--self-test" ]; then
	t=$(mktemp -d)
	trap 'rm -rf "$t"' EXIT
	fails=0
	plant() { # name, expected rc, arch-matrix line
		local d=$t/$1
		mkdir -p "$d/.github/workflows"
		printf 'jobs:\n  deb:\n    with:\n%s\n' "$3" > "$d/.github/workflows/release.yml"
		local rc=0
		"$self" "$d" >/dev/null 2>&1 || rc=$?
		if [ "$rc" != "$2" ]; then echo "self-test FAIL $1: rc $rc, want $2" >&2; fails=$((fails + 1)); fi
	}
	plant both 0 "      arch-matrix: '[\"amd64\", \"arm64\"]'"
	plant rpmboth 0 "      arch-matrix: '[\"x86_64\", \"aarch64\"]'"
	plant pushboth 0 "      arch-matrix: \${{ (github.event_name != 'push' && inputs.arm == false) && '[\"amd64\"]' || '[\"amd64\", \"arm64\"]' }}"
	plant arm64only 1 "      arch-matrix: '[\"arm64\"]'"
	plant x86only 1 "      arch-matrix: '[\"x86_64\"]'"
	plant pusharm64 1 "      arch-matrix: \${{ (github.event_name != 'push' && inputs.arm == false) && '[\"amd64\"]' || '[\"arm64\"]' }}"
	plant commented 2 "      # arch-matrix: '[\"arm64\"]'"
	mkdir -p "$t/nofile"
	rc=0; "$self" "$t/nofile" >/dev/null 2>&1 || rc=$?
	if [ "$rc" != 2 ]; then echo "self-test FAIL missing file: rc $rc, want 2" >&2; fails=$((fails + 1)); fi
	if [ "$fails" != 0 ]; then echo "workflow-full-matrix-check: self-test $fails red" >&2; exit 1; fi
	echo "workflow-full-matrix-check: self-test green (8 planted trees)"
	exit 0
fi

rc=0
check "${1:-.}" || rc=$?
exit "$rc"
