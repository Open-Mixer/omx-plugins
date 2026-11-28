#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# The labels' catalogs through omx-contract's release gate (translate --check): every listed language has its
# catalog, no key of the omx-plugin-labels domain is missing from one and no entry is stale. The gate reads the keys
# from a contract directory, so the domain is installed into a scratch one first (make install-labels-domain); the
# catalogs read and written are the tree's, catalogs/.
#
#   tools/catalogs-gate.sh [translate options]   (default --check; e.g. --lang es --model <alias> to make a language)
#
# The keys are named by the domain (--package omx-plugin-labels): translate counts a domain's words under the domain's
# name, its catalogs' owner (omx-plugins, the domain's info.x-owner) finds none.

set -eu
cd "$(dirname "$0")/.."
domain=omx-plugin-labels
dir=$(mktemp -d)
trap 'rm -r "$dir"' EXIT
make -s --no-print-directory install-labels-domain DESTDIR= CONTRACTDIR="$dir" >/dev/null
[ "$#" -gt 0 ] || set -- --check
omx-contract translate "$@" --contract-dir "$dir" --catalogs catalogs --package "$domain"
