#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/test/plugin-scope-check.sh — tools/plugin-scope.sh names exactly the plugins a change touches, and
# falls back to ALL (every plugin runs, as before #2) the moment a path is not under one plugins/<name>/
# directory, or the input is empty. `make plugin-scope`, part of `make test`.
set -euo pipefail
HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
scope=$HERE/plugin-scope.sh
bad=0

expect() { # want-output label < input
  local got
  got=$(cat | "$scope")
  if [ "$got" = "$1" ]; then echo "PASS plugin-scope: $2"; else echo "FAIL plugin-scope: $2 (got '$got', wanted '$1')"; bad=$((bad + 1)); fi
}

expect "omx-eq8" "one plugin's own files scope to it" <<'EOF'
plugins/omx-eq8/omx_eq8_core.h
plugins/omx-eq8/generated/omx-eq8.lv2/manifest.ttl
EOF

expect "omx-delay omx-eq8" "two changed plugins scope to both, sorted" <<'EOF'
plugins/omx-eq8/omx_eq8_core.h
plugins/omx-delay/omx_delay_clap.c
EOF

expect "ALL" "a shared file (the top-level Makefile) alongside a plugin falls back to ALL" <<'EOF'
plugins/omx-eq8/omx_eq8_core.h
Makefile
EOF

expect "ALL" "a tools/ change falls back to ALL" <<'EOF'
tools/gen.mjs
EOF

expect "ALL" "empty input falls back to ALL" <<'EOF'
EOF

expect "ALL" "a bare plugins/<name> path with no file under it falls back to ALL" <<'EOF'
plugins/omx-eq8
EOF

[ "$bad" = 0 ] && echo "PASS plugin-scope: every case" || { echo "plugin-scope: $bad failure(s)"; exit 1; }
