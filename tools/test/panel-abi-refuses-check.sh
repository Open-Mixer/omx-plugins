#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/test/panel-abi-refuses-check.sh — `make panel-abi` holds include/omx_panel_description.h to the layout
# omx-clap-core's installed omx_clap_ext.h declares. Where that header is absent (no omx-clap-core-devel), the target
# REFUSES BY NAME: one FAIL line naming the header and the package, and a non-zero exit, never a compiler error that
# reads like a broken test. Proven against an include directory that has no omx-clap-host/omx_clap_ext.h.
set -u
here=$(cd "$(dirname "$0")/../.." && pwd)
empty=$(mktemp -d)
trap 'rm -rf "$empty"' EXIT
out=$(make -s -C "$here" panel-abi CLAP_CORE_INCLUDEDIR="$empty" 2>&1)
rc=$?
if [ "$rc" -eq 0 ]; then
  echo "FAIL panel-abi passed with no omx_clap_ext.h under $empty: it did not read the header it names"
  exit 1
fi
if ! grep -q "^FAIL panel-abi: no omx-clap-host/omx_clap_ext.h under $empty" <<<"$out"; then
  echo "FAIL panel-abi refused without naming the missing header (rc $rc):"
  printf '%s\n' "$out" | head -5
  exit 1
fi
if grep -qE "error:|fatal error" <<<"$out"; then
  echo "FAIL panel-abi reached the compiler with the header absent:"
  printf '%s\n' "$out" | head -5
  exit 1
fi
echo "ok   panel-abi refuses by name when omx-clap-core's omx_clap_ext.h is absent"
