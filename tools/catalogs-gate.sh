#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# The labels' catalogs through omx-contract's release gate (translate --check): every listed language has its
# catalog, no key of the omx-plugin-labels domain is missing from one and no entry is stale. The gate reads the keys
# from a contract directory, so the domain is installed into a scratch one first (install-contract-dir.sh --domain);
# the catalogs read and written are the tree's, contract.d/omx-plugin-labels/catalogs.
#
#   tools/catalogs-gate.sh [translate options]   (default --check; e.g. --lang es --model <alias> to make a language)
#
# The keys are named by the domain (--package omx-plugin-labels): translate counts a domain's words under the domain's
# name, its catalogs' owner (omx-plugins, the domain's info.x-owner) finds none.

set -eu
cd "$(dirname "$0")/.."
domain=omx-plugin-labels
installer=$(make -s --no-print-directory print-install-contract-dir)
[ -n "$installer" ] || { echo "catalogs-gate: no install-contract-dir.sh (omx-contract-devel, or OMX_CONTRACT_DIR)" >&2; exit 2; }
dir=$(mktemp -d)
trap 'rm -r "$dir"' EXIT
sh "$installer" --domain "contract.d/$domain" "$dir" >/dev/null
[ "$#" -gt 0 ] || set -- --check
exec omx-contract translate "$@" --contract-dir "$dir" --catalogs "contract.d/$domain/catalogs" --package "$domain"
