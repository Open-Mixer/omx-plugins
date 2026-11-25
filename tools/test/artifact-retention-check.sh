#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
# Refuses an artifact a pull-request or lane run keeps for more than 2 days (omx-plugins#53): every actions/upload-artifact
# step in .github/workflows states its retention-days, and states it as an expression that keeps the release value on a
# tag (or a publishing or staged dispatch) and ends `|| 2 }}` on any other run. An upload with no retention-days keeps the
# repository default (90 days) and is refused as well. Usage: artifact-retention-check.sh [repo-root] (default: .), or
# --self-test, which runs the check on planted trees of each shape. Exit 0 clean, 1 a hit (each step named), 2 no
# .github/workflows to check, or no upload in them. `make artifact-retention`, part of `make test`.
set -eu
self=$0

check() {
	local wf=$1/.github/workflows
	if [ ! -d "$wf" ]; then
		echo "artifact-retention-check: no $wf to check (refusing: an empty check proves nothing)" >&2
		return 2
	fi
	# one UP line per upload step, then BAD lines; a step ends at the next `- ` item at its indent or less
	local out
	out=$(find "$wf" -type f \( -name '*.yml' -o -name '*.yaml' \) -exec awk '
		function flush() {
			if (up) {
				print "UP " FILENAME ":" at
				if (ret == "") print "BAD " FILENAME ":" at ": an upload-artifact step with no retention-days (the 90-day default)"
				else if (ret !~ /\|\|[[:space:]]*2[[:space:]]*\}\}$/) print "BAD " FILENAME ":" retat ": retention-days: " ret " (not 2 days off a release run)"
			}
			up = 0; ret = ""; ind = -1
		}
		FNR == 1 { flush() }
		/^[[:space:]]*#/ { next }
		/^[[:space:]]*-[[:space:]]/ {
			i = match($0, /-/) - 1
			if (up && i <= ind) flush()
			if ($0 ~ /uses:[[:space:]]*actions\/upload-artifact@/) { up = 1; at = FNR; ind = i }
			next
		}
		up && /^[^[:space:]]/ { flush(); next }
		up && /^[[:space:]]+retention-days:/ { r = $0; sub(/^[[:space:]]+retention-days:[[:space:]]*/, "", r); sub(/[[:space:]]+$/, "", r); ret = r; retat = FNR }
		END { flush() }
	' {} +)
	if ! grep -q '^UP ' <<<"$out"; then
		echo "artifact-retention-check: no upload-artifact step in $wf (refusing: an empty check proves nothing)" >&2
		return 2
	fi
	if grep -q '^BAD ' <<<"$out"; then
		sed -n 's/^BAD //p' <<<"$out"
		echo "artifact-retention-check: an artifact kept more than 2 days off a release run (retention-days: \${{ (startsWith(github.ref, 'refs/tags/')) && N || 2 }})" >&2
		return 1
	fi
	echo "artifact-retention-check: every upload keeps 2 days off a release run ($(grep -c '^UP ' <<<"$out") upload(s))"
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
	step() { # retention line (empty: none)
		printf 'jobs:\n  rpm:\n    steps:\n      - uses: actions/upload-artifact@v4\n        with:\n          name: rpms\n          path: rpms/*.rpm\n%s      - name: after\n        run: true\n' "${1:+          retention-days: $1
}"
	}
	plant good 0 "$(step "\${{ (startsWith(github.ref, 'refs/tags/')) && 30 || 2 }}")"
	plant release-inputs 0 "$(step "\${{ (startsWith(github.ref, 'refs/tags/') || inputs.publish || inputs.staged) && 90 || 2 }}")"
	plant bare-number 1 "$(step 7)"
	plant seven-off-release 1 "$(step "\${{ (startsWith(github.ref, 'refs/tags/')) && 30 || 7 }}")"
	plant no-retention 1 "$(step '')"
	# the retention of the NEXT step does not count for an upload that has none
	plant next-step-retention 1 "$(printf 'jobs:\n  a:\n    steps:\n      - uses: actions/upload-artifact@v4\n        with:\n          name: x\n      - uses: actions/upload-artifact@v4\n        with:\n          name: y\n          retention-days: ${{ (startsWith(github.ref, %s)) && 30 || 2 }}\n' "'refs/tags/'")"
	plant no-upload 2 "$(printf 'jobs:\n  a:\n    steps:\n      - run: true\n')"
	mkdir -p "$t/empty"
	rc=0; "$self" "$t/empty" >/dev/null 2>&1 || rc=$?
	[ "$rc" = 2 ] || { echo "self-test FAIL no-workflows: rc $rc, want 2" >&2; fails=$((fails + 1)); }
	[ "$fails" = 0 ] || exit 1
	echo "artifact-retention-check self-test: 8 planted trees, each answered as expected"
	exit 0
fi

check "${1:-.}"
