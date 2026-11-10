#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/plugin-scope.sh < changed-files — one repo-relative path per line (as `git diff --name-only` writes
# them) — prints either `ALL` or the sorted, space-separated, de-duplicated list of plugin directory names
# (plugins/<name>/...) the paths touch. `ALL` is the safe default: empty input (nothing read, or the diff
# could not be read), or any path that is not under exactly one plugins/<name>/ directory (shared code,
# tools/, docs, the top-level Makefile, a renamed or added plugins/ directory itself with no name captured).
# A pull request that only touches plugins/<name>/... files can scope its CI to those plugins (CHANGED_PLUGINS,
# Makefile, CI_SCOPE.md); anything else runs the whole tree, same as today. (#2)
set -euo pipefail
all=0
declare -A seen=()
names=()
while IFS= read -r path; do
  [ -n "$path" ] || continue
  if [[ "$path" =~ ^plugins/([^/]+)/.+ ]]; then
    name=${BASH_REMATCH[1]}
    if [ -z "${seen[$name]+x}" ]; then
      seen[$name]=1
      names+=("$name")
    fi
  else
    all=1
  fi
done

if [ "$all" = 1 ] || [ "${#names[@]}" -eq 0 ]; then
  echo ALL
else
  printf '%s\n' "${names[@]}" | sort -u | tr '\n' ' ' | sed 's/ $/\n/'
fi
