#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas
#
# panel-enums-hand-check.sh — include/omx_panel_description.h spells no enumerator of the panel vocabulary with a
# code of its own (`= <number>`): the families, widgets, display kinds, display sources and placements are
# expanded from omx-contract's X-macros in <omxcontract/omx_vocabulary.h>. Part of `make panel-enums`.
set -eu
cd "$(dirname "$0")/../.."
hand=$(grep -nE '^[[:space:]]*OMX_CLAP_(FAMILY|WIDGET|DISPLAY|DISPLAY_FROM|PLACEMENT)_[A-Z0-9_]+[[:space:]]*=[[:space:]]*[0-9]+' include/omx_panel_description.h || true)
if [ -n "$hand" ]; then
  echo "FAIL panel-enums-hand: include/omx_panel_description.h spells codes by hand; they are omx-contract's:" >&2
  echo "$hand" >&2
  exit 1
fi
echo "ok   panel-enums-hand: no hand code in include/omx_panel_description.h"
