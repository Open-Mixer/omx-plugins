#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
# Refuses a Fedora 44 reference in the workflows: the fc44 packages are dropped for this pass (protocol 0.6.0 and later
# are published for fc45 only), so a fedora:44 image or an fc44 cell name is a job that can no longer resolve its build
# dependencies. Usage: workflow-fedora-check.sh [repo-root]   (default: the current directory). Exit 1 names each line.
set -eu
root=${1:-.}
hits=$(grep -rnE 'fedora:44|fc44' "$root/.github/workflows" || true)
if [ -n "$hits" ]; then
	printf '%s\n' "$hits"
	echo "workflow-fedora-check: Fedora 44 reference in .github/workflows (move it to fedora:45 / fc45)" >&2
	exit 1
fi
echo "workflow-fedora-check: no Fedora 44 reference in .github/workflows"
