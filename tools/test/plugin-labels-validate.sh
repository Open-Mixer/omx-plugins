#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/test/plugin-labels-validate.sh — the labels domain, contract.d/omx-plugin-labels (tools/plugin-labels.mjs),
# with the tree's catalogs/, installed as `make install-contract` installs it (make install-labels-domain: omx-contract's
# install-contract-dir.sh --domain) into a scratch contract directory, is read by omx-contract's own reader with its catalogs: `omx-contract validate` over the domain
# alone, then beside the installed contract directory (OMX_CONTRACT_PATH, so the prefix rule meets every other domain).
# The reader holds each catalog to its schema, each key to a word of the domain and each placeholder to its args.
#
#   tools/test/plugin-labels-validate.sh               the check
#   tools/test/plugin-labels-validate.sh --self-test   a catalog key no word has, an entry naming a placeholder the
#                                                      word has none of, and an item outside the domain's prefix
#                                                      each refuse; the tree's domain passes
# Needs the omx-contract command (OMX_CONTRACT_CLI, its bin/omx-contract.mjs or an executable, else `omx-contract` on
# PATH) and install-contract-dir.sh (omx-contract-devel's, or OMX_CONTRACT_DIR's tools/).
# Exit: 0 pass, 1 refused, 2 the check could not run.
set -euo pipefail
HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
DOMAIN=omx-plugin-labels
SRC=$HERE/contract.d/$DOMAIN/$DOMAIN.json

cli() {
  if [ -n "${OMX_CONTRACT_CLI:-}" ]; then
    case $OMX_CONTRACT_CLI in *.mjs) echo "node $OMX_CONTRACT_CLI" ;; *) echo "$OMX_CONTRACT_CLI" ;; esac
  elif command -v omx-contract >/dev/null 2>&1; then echo omx-contract
  fi
}
CLI=$(cli)
[ -n "$CLI" ] || { echo "plugin-labels-validate: no omx-contract command (OMX_CONTRACT_CLI, or omx-contract on PATH)" >&2; exit 2; }
INSTALLER=$(make -s --no-print-directory -C "$HERE" print-install-contract-dir)
[ -n "$INSTALLER" ] || { echo "plugin-labels-validate: no install-contract-dir.sh (omx-contract-devel, or OMX_CONTRACT_DIR)" >&2; exit 2; }
INSTALLED=$(pkg-config --variable=contractdir omx-contract 2>/dev/null || true)

SCRATCH=$(mktemp -d)
trap 'rm -r "$SCRATCH"' EXIT

# read <contract dir>: the domain read there, alone and beside the installed contract directory
read_dir() {
  local dir=$1
  $CLI validate --contract-dir "$dir" || return 1
  if [ -n "$INSTALLED" ] && [ -d "$INSTALLED" ]; then
    OMX_CONTRACT_PATH="$dir:$INSTALLED" $CLI validate --installed || return 1
  fi
}

# validate <domain dir> <tag>: install a staged domain into a fresh contract directory and read it
validate() {
  local from=$1 dir=$SCRATCH/$2/contract.d
  mkdir -p "$dir"
  sh "$INSTALLER" --domain "$from" "$dir" >/dev/null || return 1
  read_dir "$dir"
}

# the domain staged as make install-labels-domain stages it (its document and the tree's catalogs/), under a
# contract.d/ (the installer names the domain from the path), for an arm to edit
copy() {
  local to=$SCRATCH/src-$1/contract.d/$DOMAIN
  mkdir -p "$to/catalogs"
  cp "$SRC" "$to/"
  cp "$HERE"/catalogs/*.json "$to/catalogs/"
  echo "$to"
}

if [ "${1:-}" = --self-test ]; then
  bad=0
  arm() {
    local name=$1 edit=$2 to
    to=$(copy "$3")
    node -e "$edit" "$to" "$4"
    if validate "$to" "arm-$3" >/dev/null 2>"$SCRATCH/err-$3"; then
      echo "self-test: $name passed" >&2; bad=$((bad + 1))
    elif ! grep -q -- "$5" "$SCRATCH/err-$3"; then
      echo "self-test: $name refused without '$5': $(grep -m1 -v '^$' "$SCRATCH/err-$3")" >&2; bad=$((bad + 1))
    else
      echo "ok   $name refuses: $(grep -m1 -- "$5" "$SCRATCH/err-$3")"
    fi
  }
  # arm <name> <edit> <tag> <rel> <refusal>: the edit, a node expression over the parsed value `d` of the copy's JSON
  # file <rel>, must be refused by a line carrying <refusal>
  EDIT='const fs = require("fs"); const f = process.argv[1] + "/" + process.argv[2]; const d = JSON.parse(fs.readFileSync(f, "utf8"));'
  SAVE='fs.writeFileSync(f, JSON.stringify(d, null, 2));'
  arm 'a catalog key no word has' "$EDIT d.messages['OMX_PLUGIN_LABELS_COMP.section.sabotage'] = { text: 'Sabotatge', by: 'human' }; $SAVE" key catalogs/ca.json 'catalog-key: OMX_PLUGIN_LABELS_COMP.section.sabotage'
  arm 'an entry naming a placeholder its word has none of' "$EDIT d.messages['OMX_PLUGIN_LABELS_COMP.section.comp'] = { text: 'Comp {plugin}', by: 'human' }; $SAVE" placeholder catalogs/ca.json 'catalog-placeholders: OMX_PLUGIN_LABELS_COMP.section.comp'
  arm 'an item outside the domain prefix' "$EDIT const s = d.components.schemas; s.PLUGIN_LABELS_SABOTAGE = s.OMX_PLUGIN_LABELS_COMP; $SAVE" prefix omx-plugin-labels.json 'contract.item.prefix'
  if ! validate "$(copy tree)" tree >/dev/null; then echo "self-test: the tree's domain does not pass" >&2; bad=$((bad + 1)); fi
  [ "$bad" -eq 0 ] || { echo "plugin-labels-validate self-test: $bad arm(s) wrong" >&2; exit 1; }
  echo "plugin-labels-validate self-test: 3 arms refuse, the tree's domain passes"
  exit 0
fi

[ -f "$SRC" ] || { echo "plugin-labels-validate: no $SRC (run \`node tools/gen.mjs\`)" >&2; exit 1; }
make -s --no-print-directory -C "$HERE" install-labels-domain DESTDIR= CONTRACTDIR="$SCRATCH/tree/contract.d" >/dev/null
read_dir "$SCRATCH/tree/contract.d"
echo "plugin-labels-validate: the $DOMAIN domain and its catalogs read through omx-contract validate"
