#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
# Publish only by tagging (Open-Mixer/.github#126; omx-plugins#109, where a workflow_dispatch of a tag ref with
# publish=true attached the Zynthian tarball to the release). .github/workflows/release.yml is held to it, line by line:
#   1. no publish input under workflow_dispatch or workflow_call (a dispatch is a dry run, whatever its inputs say);
#   2. nothing reads inputs.publish (an input no trigger declares reads as empty, and says the dispatch still publishes);
#   3. every publish: a shared build call passes is false or exactly
#      ${{ github.event_name == 'push' && startsWith(github.ref, 'refs/tags/') }};
#   4. every step that writes to the GitHub release (gh release upload, create or edit) runs under an if, its own or its
#      job's, holding both github.event_name == 'push' and startsWith(github.ref, 'refs/tags/').
# A release.yml with no publish: and no such step is refused: the law would hold nothing.
# Usage: release-publish-gate-check.sh [release.yml] (default: the tree's), or --self-test, which plants each careless
# edit in a copy of the tree's release.yml and requires it red by name. Exit 0 clean, 1 a hit (each named), 2 nothing to
# check. `make release-publish-gate`, part of `make test`.
set -eu
self=$0
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
TAG_PUSH="github.event_name == 'push' && startsWith(github.ref, 'refs/tags/')"

check() { # <release.yml>
	local f=$1 out
	[ -f "$f" ] || { echo "release-publish-gate-check: no $f to check (refusing: an empty check proves nothing)" >&2; return 2; }
	out=$(awk -v tagpush="$TAG_PUSH" '
		function held(c) { return index(c, "github.event_name == '"'"'push'"'"'") && index(c, "startsWith(github.ref, '"'"'refs/tags/'"'"')") }
		function flushstep() {
			if (instep && verb != "") {
				points++
				if (!(held(stepif) || held(jobif)))
					print "BAD " FILENAME ":" stepat ": job " job " step '"'"'" (sname != "" ? sname : "gh release " verb) "'"'"' writes to the release (gh release " verb ") under if `" (stepif != "" ? stepif : jobif) "`: not held to a tag push (" tagpush ")"
			}
			instep = 0; verb = ""; stepif = ""; sname = ""; inrun = 0
		}
		/^[[:space:]]*#/ { next }
		/^[A-Za-z]/ { flushstep(); sect = ($0 ~ /^on:/) ? "on" : ($0 ~ /^jobs:/) ? "jobs" : ""; trig = ""; job = ""; jobif = ""; next }
		/inputs\.publish/ { print "BAD " FILENAME ":" FNR ": inputs.publish is read, an input no trigger declares (a dispatch publish is gone)" }
		sect == "on" && /^  (workflow_dispatch|workflow_call):/ { trig = $1; sub(":", "", trig); next }
		sect == "on" && /^  [a-z_]+:/ { trig = ""; next }
		sect == "on" && trig != "" && /^      publish:/ { print "BAD " FILENAME ":" FNR ": a publish input under " trig " (a dispatch is a dry run: only a tag push publishes)" }
		sect == "jobs" && /^  [A-Za-z0-9_-]+:[[:space:]]*$/ { flushstep(); job = $1; sub(":", "", job); jobif = ""; next }
		sect == "jobs" && /^    if:/ { jobif = $0; sub(/^    if:[[:space:]]*/, "", jobif); next }
		sect == "jobs" && !instep && /^      publish:/ {
			points++
			v = $0; sub(/^      publish:[[:space:]]*/, "", v); sub(/[[:space:]]+$/, "", v)
			if (v != "false" && v != "${{ " tagpush " }}") print "BAD " FILENAME ":" FNR ": job " job " publishes under `" v "`: a build publishes on a tag push alone (${{ " tagpush " }}) or not at all (false)"
			next
		}
		sect == "jobs" && /^      - / { flushstep(); instep = 1; stepat = FNR; if ($0 ~ /^      - name:/) { sname = $0; sub(/^      - name:[[:space:]]*/, "", sname) } }
		instep && /^        name:/ { sname = $0; sub(/^        name:[[:space:]]*/, "", sname) }
		instep && /^        if:/ { stepif = $0; sub(/^        if:[[:space:]]*/, "", stepif) }
		instep && /^        [A-Za-z_-]+:/ { inrun = ($0 ~ /^        run:/) }
		instep && inrun && match($0, /gh release (upload|create|edit)/) { verb = substr($0, RSTART + 11, RLENGTH - 11) }
		END { flushstep(); print "POINTS " points + 0 }
	' "$f")
	local points
	points=$(sed -n 's/^POINTS //p' <<<"$out")
	if [ "${points:-0}" = 0 ]; then
		echo "release-publish-gate-check: no publish: and no step writing to the release in $f (refusing: an empty check proves nothing)" >&2
		return 2
	fi
	if grep -q '^BAD ' <<<"$out"; then
		sed -n 's/^BAD //p' <<<"$out"
		echo "release-publish-gate-check: FAIL, $f publishes off the tag push (publish only by tagging: $TAG_PUSH)" >&2
		return 1
	fi
	echo "release-publish-gate-check: every publishing point of $f ($points) is held to a tag push, and no input or read says otherwise"
}

if [ "${1:-}" = "--self-test" ]; then
	t=$(mktemp -d)
	trap 'rm -rf "$t"' EXIT
	fails=0
	REL=$ROOT/.github/workflows/release.yml
	plant() { # <name> <old text> <new text> <pattern the red must carry>
		cp "$REL" "$t/release.yml"
		python3 -I -c 'import sys; p,o,n=sys.argv[1:4]; s=open(p).read(); assert o in s, "the plant matched nothing: "+o; open(p,"w").write(s.replace(o,n,1))' "$t/release.yml" "$2" "$3" || { echo "self-test FAIL $1: nothing to plant" >&2; fails=$((fails + 1)); return; }
		local out rc=0
		out=$("$self" "$t/release.yml" 2>/dev/null) || rc=$?
		if [ "$rc" != 1 ]; then echo "self-test FAIL $1: rc $rc, want 1 ($out)" >&2; fails=$((fails + 1)); return; fi
		grep -qE "$4" <<<"$out" || { echo "self-test FAIL $1: red, but not by name: $out" >&2; fails=$((fails + 1)); return; }
		echo "ok   self-test $1: $(grep -m1 -E "$4" <<<"$out")"
	}
	"$self" "$REL" >/dev/null || { echo "self-test FAIL: the tree's release.yml is not green (each plant below changes one thing)" >&2; fails=$((fails + 1)); }
	plant "the tarball attached on a dispatch of a tag ref (omx-plugins#109)" \
		"github.ref_type == 'tag' && (github.event_name == 'push' && startsWith(github.ref, 'refs/tags/')) }}" \
		"github.ref_type == 'tag' && (github.event_name == 'push' || inputs.publish == true) }}" \
		"step 'Attach, never creating a release' writes to the release \(gh release upload\) under if .*inputs.publish == true.*: not held to a tag push"
	plant "a publish dispatch input" \
		"      staged:
        description: 'Build fc45 x86_64" \
		"      publish:
        description: 'Sign and publish'
        type: boolean
        default: false
      staged:
        description: 'Build fc45 x86_64" \
		'a publish input under workflow_dispatch'
	plant "a read of inputs.publish in a retention expression" \
		"retention-days: \${{ (startsWith(github.ref, 'refs/tags/') || inputs.staged) && 30 || 2 }}" \
		"retention-days: \${{ (startsWith(github.ref, 'refs/tags/') || inputs.publish || inputs.staged) && 30 || 2 }}" \
		'inputs.publish is read'
	plant "the rpm build publishes on every push" \
		"      publish: \${{ github.event_name == 'push' && startsWith(github.ref, 'refs/tags/') }}
      needs-mod-host" \
		"      publish: \${{ github.event_name == 'push' }}
      needs-mod-host" \
		"job rpm publishes under .*github.event_name == 'push' }}.: a build publishes on a tag push alone"
	plant "the attach job's if dropped (its step has none of its own)" \
		"    if: \${{ !cancelled() && needs.zynthian.result == 'success' && github.ref_type == 'tag' && (github.event_name == 'push' && startsWith(github.ref, 'refs/tags/')) }}
" "" \
		"step 'Attach, never creating a release' writes to the release .* under if \`\`: not held to a tag push"
	# a workflow with nothing to hold is refused, never green
	printf 'on:\n  push:\njobs:\n  a:\n    steps:\n      - run: true\n' > "$t/release.yml"
	rc=0; "$self" "$t/release.yml" >/dev/null 2>&1 || rc=$?
	[ "$rc" = 2 ] || { echo "self-test FAIL nothing-to-hold: rc $rc, want 2" >&2; fails=$((fails + 1)); }
	[ "$fails" = 0 ] || exit 1
	echo "release-publish-gate-check self-test: five planted edits named red, an empty workflow refused"
	exit 0
fi

check "${1:-$ROOT/.github/workflows/release.yml}"
