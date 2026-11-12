#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/test/contract-dir-check.sh — `make install-contract` puts each shipped plugin's descriptor into omx-contract's
# contract directory (omx-contract docs/contract-dir.md): <contractdir>/3/omx-plugins/plugins/<stem>.json, byte for
# byte the plugin folder's <stem>.descriptor.json, one per shipped plugin that has one, and each definition file of
# contract/ as <contractdir>/3/omx-plugins/<file>.json byte for byte, and nothing else, each listed in
# the package's MANIFEST.json with its sha256 and this release's version; and omx-contract's own reader reads the
# installed tree beside the installed contract directory (`omx-contract validate --installed`, with
# OMX_CONTRACT_PATH naming both), needing omx-plugins at this version.
#
#   tools/test/contract-dir-check.sh               `make install-contract` into a scratch directory, then the check
#   tools/test/contract-dir-check.sh --self-test   the check over sabotaged installs: a descriptor left out of the
#                                                  install list, one missing, one changed, one extra file, a manifest
#                                                  hash that is not the file's and another version each refuse; the
#                                                  right install passes
# The reader step needs the omx-contract command (OMX_CONTRACT_CLI, its bin/omx-contract.mjs or an executable, else
# `omx-contract` on PATH); without one it says so in one line and the file checks still run.
# Exit: 0 pass, 1 refused, 2 the check could not run.
set -euo pipefail
HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
VERSION=$(make -s --no-print-directory -C "$HERE" version)

# the plugins whose descriptor a host reads from the contract directory (FreeMixer/omx-plugins#219): each must ship
# one, and the install must list it
REQUIRED="omx-eq8 omx-eq16 omx-eq32 omx-strip"

# the definition files this package owns (omx-contract's ownership audit, 2026-11-10): each must be in contract/ and
# installed. A file that moved here from omx-contract and went missing would leave the item declared by no one, and
# the check over contract/*.json alone would stay green (it reads what is there). CONTRACT_SRC: the self-test's copy.
REQUIRED_DEFS="role-meanings.json plugin-face.json"
CONTRACT_SRC=${CONTRACT_SRC:-$HERE/contract}

# ship stems with a descriptor: the plugin folders the top Makefile builds whose declaration does not say
# `"role": "test"`, holding <stem>.descriptor.json
described() {
  local m dir stem
  for m in "$HERE"/plugins/*/Makefile; do
    dir=$(dirname "$m"); stem=$(basename "$dir")
    if [ -f "$dir/$stem.decl.json" ] && grep -q '"role": *"test"' "$dir/$stem.decl.json"; then continue; fi
    [ -f "$dir/$stem.descriptor.json" ] && echo "$stem"
  done
  return 0
}

cli() {
  if [ -n "${OMX_CONTRACT_CLI:-}" ]; then
    case $OMX_CONTRACT_CLI in *.mjs) echo "node $OMX_CONTRACT_CLI" ;; *) echo "$OMX_CONTRACT_CLI" ;; esac
  elif command -v omx-contract >/dev/null 2>&1; then echo omx-contract
  fi
}

# check <contractdir> [reader]: one line per refusal; the count is the exit status
check() {
  local dir=$1 reader=${2:-reader} pkg=$1/3/omx-plugins bad=0 stem f rel want got listed n=0 c sys stems
  stems=$(described)
  if [ ! -f "$pkg/MANIFEST.json" ]; then echo "contract-dir: $pkg holds no MANIFEST.json"; return 1; fi
  listed=$(node -e '
    const m = JSON.parse(require("fs").readFileSync(process.argv[1], "utf8"));
    if (m.package !== "omx-plugins") console.log(`package ${m.package}`);
    if (m.version !== process.argv[2]) console.log(`version ${m.version}, this release is ${process.argv[2]}`);
    for (const [rel, h] of Object.entries(m.files ?? {})) console.log(`file ${rel} ${h}`);
  ' "$pkg/MANIFEST.json" "$VERSION")
  while read -r c rest; do
    case $c in package|version) echo "contract-dir: MANIFEST.json names the $c $rest"; bad=$((bad + 1)) ;; esac
  done <<<"$listed"
  for stem in $stems; do
    n=$((n + 1)); rel=plugins/$stem.json
    if [ ! -f "$pkg/$rel" ]; then echo "contract-dir: $stem: $rel is not installed"; bad=$((bad + 1)); continue; fi
    cmp -s "$HERE/plugins/$stem/$stem.descriptor.json" "$pkg/$rel" \
      || { echo "contract-dir: $stem: $rel is not plugins/$stem/$stem.descriptor.json byte for byte"; bad=$((bad + 1)); }
    want=$(sha256sum "$pkg/$rel" | cut -d' ' -f1)
    got=$(awk -v r="$rel" '$1 == "file" && $2 == r {print $3}' <<<"$listed")
    [ "$got" = "$want" ] || { echo "contract-dir: $stem: MANIFEST.json lists $rel as ${got:-nothing}, the file is $want"; bad=$((bad + 1)); }
  done
  for stem in $REQUIRED; do
    grep -qx "$stem" <<<"$stems" || { echo "contract-dir: $stem ships no descriptor (plugins/$stem/$stem.descriptor.json)"; bad=$((bad + 1)); continue; }
    awk -v r="plugins/$stem.json" '$1 == "file" && $2 == r {f = 1} END {exit !f}' <<<"$listed" \
      || { echo "contract-dir: $stem: MANIFEST.json does not list plugins/$stem.json"; bad=$((bad + 1)); }
  done
  [ "$n" -gt 0 ] || { echo "contract-dir: no shipped plugin has a descriptor"; bad=$((bad + 1)); }
  for rel in $REQUIRED_DEFS; do
    [ -f "$CONTRACT_SRC/$rel" ] || { echo "contract-dir: contract/$rel is missing: this package owns it"; bad=$((bad + 1)); continue; }
    [ -f "$pkg/$rel" ] || { echo "contract-dir: $rel is owned here and not installed"; bad=$((bad + 1)); }
  done
  for f in "$CONTRACT_SRC"/*.json; do
    [ -e "$f" ] || continue   # no definition file: the unit codes are omx-contract's since 3.6.0
    rel=$(basename "$f")
    if [ ! -f "$pkg/$rel" ]; then echo "contract-dir: contract/$rel is not installed as $rel"; bad=$((bad + 1)); continue; fi
    cmp -s "$f" "$pkg/$rel" || { echo "contract-dir: $rel is not contract/$rel byte for byte"; bad=$((bad + 1)); }
    want=$(sha256sum "$pkg/$rel" | cut -d' ' -f1)
    got=$(awk -v r="$rel" '$1 == "file" && $2 == r {print $3}' <<<"$listed")
    [ "$got" = "$want" ] || { echo "contract-dir: MANIFEST.json lists $rel as ${got:-nothing}, the file is $want"; bad=$((bad + 1)); }
  done
  while IFS= read -r f; do
    rel=${f#"$pkg"/}
    [ "$rel" = MANIFEST.json ] && continue
    case $rel in plugins/*.json) stem=$(basename "$rel" .json); grep -qx "$stem" <<<"$stems" && continue ;; esac
    [ -f "$CONTRACT_SRC/$rel" ] && continue
    echo "contract-dir: $rel is installed and is no shipped plugin's descriptor nor a file of contract/"; bad=$((bad + 1))
  done < <(find "$pkg" -type f | LC_ALL=C sort)
  awk '$1 == "file" {print $2}' <<<"$listed" | while read -r rel; do [ -f "$pkg/$rel" ] || echo "contract-dir: MANIFEST.json lists $rel, which is not installed"; done | grep . && bad=$((bad + 1))
  if [ "$reader" = reader ]; then
    c=$(cli)
    if [ -z "$c" ]; then
      echo "NO-CONTRACT-COMMAND contract-dir: no omx-contract command: the reader step is skipped" >&2
    else
      # The reader sees the installed owners' definitions with this install in place of the omx-plugins package of
      # the system: omx-panel requires that package, so a build host holds this release's own, and read beside the
      # install under test it would answer for the file the install changed.
      sys=$(pkg-config --variable=contractdir omx-contract 2>/dev/null || true)
      ov=$(mktemp -d)
      [ -n "$sys" ] && [ -d "$sys" ] && cp -r "$sys/." "$ov"
      rm -rf "$ov/${pkg#"$dir"/}"
      cp -r "$dir/." "$ov"
      OMX_CONTRACT_PATH="$ov" $c validate --installed --needs "omx-plugins@>= $VERSION" >/dev/null 2>"$dir.reader" \
        || { echo "contract-dir: omx-contract's reader refuses the install: $(head -1 "$dir.reader")"; bad=$((bad + 1)); }
      rm -rf "$ov" "$dir.reader"
    fi
  fi
  return "$bad"
}

install_into() { # <contractdir> [make args...]
  local to=$1; shift
  make -s -C "$HERE" install-contract DESTDIR= CONTRACTDIR="$to" "$@" >/dev/null
}

scratch=$(mktemp -d); trap 'rm -rf "$scratch"' EXIT

if [ "${1:-}" = --self-test ]; then
  fail=0
  install_into "$scratch/good"
  all=$(described); first=${all%%$'\n'*}
  rest=$(for s in $all; do [ "$s" = "$first" ] || printf '%s ' "plugins/$s/$s.descriptor.json"; done)
  rehash() { # <contractdir> <rel>: the manifest lists the file's new sha256, so only the content check can see it
    local m=$1/3/omx-plugins/MANIFEST.json h
    h=$(sha256sum "$1/3/omx-plugins/$2" | cut -d' ' -f1)
    node -e 'const fs = require("fs"); const [p, r, h] = process.argv.slice(1); const m = JSON.parse(fs.readFileSync(p, "utf8"));
      m.files[r] = h; fs.writeFileSync(p, JSON.stringify(m, null, 2) + "\n");' "$m" "$2" "$h"
  }
  for s in left-out missing changed extra hash version $(for r in $REQUIRED_DEFS; do printf 'owned-%s ' "$r"; done) $(for r in $REQUIRED; do printf 'unlisted-%s ' "$r"; done); do
    t=$scratch/$s; rm -rf "$t"
    case $s in
      left-out) install_into "$t" DESCRIPTORS="$rest" ;;
      *) cp -r "$scratch/good" "$t" ;;
    esac
    p=$t/3/omx-plugins
    case $s in
      missing) rm "$p/plugins/$first.json" ;;
      changed) sed -i '0,/"name": "/s//"name": "x/' "$p/plugins/$first.json"; rehash "$t" "plugins/$first.json" ;;
      extra) cp "$p/plugins/$first.json" "$p/plugins/omx-test-sine-voice.json"; rehash "$t" plugins/omx-test-sine-voice.json ;;
      hash) sed -i "s|\"plugins/$first.json\": \"[0-9a-f]*\"|\"plugins/$first.json\": \"$(printf '0%.0s' $(seq 64))\"|" "$p/MANIFEST.json" ;;
      version) sed -i 's/"version": "[0-9.]*"/"version": "0.0.1"/' "$p/MANIFEST.json" ;;
      unlisted-*) node -e 'const fs = require("fs"); const [f, r] = process.argv.slice(1); const m = JSON.parse(fs.readFileSync(f, "utf8"));
          delete m.files[r]; fs.writeFileSync(f, JSON.stringify(m, null, 2) + "\n");' "$p/MANIFEST.json" "plugins/${s#unlisted-}.json" ;;
    esac
    src=$HERE/contract
    case $s in
      owned-*) # the owned file gone from the tree AND from the install and manifest: only the ownership list sees it
        r=${s#owned-}; src=$scratch/src-$r; rm -rf "$src"; cp -r "$HERE/contract" "$src"; rm "$src/$r" "$p/$r"
        node -e 'const fs = require("fs"); const [f, r] = process.argv.slice(1); const m = JSON.parse(fs.readFileSync(f, "utf8"));
          delete m.files[r]; fs.writeFileSync(f, JSON.stringify(m, null, 2) + "\n");' "$p/MANIFEST.json" "$r" ;;
    esac
    if out=$(CONTRACT_SRC=$src check "$t" none 2>&1); then echo "SABOTAGE $s: STAYED GREEN"; fail=1
    elif [ -z "$out" ]; then echo "SABOTAGE $s: the check stopped naming nothing"; fail=1
    else echo "SABOTAGE $s: red (${out%%$'\n'*})"; fi
  done
  if [ -n "$(cli)" ]; then
    t=$scratch/reader; cp -r "$scratch/good" "$t"
    printf ' ' >> "$t/3/omx-plugins/plugins/$first.json"
    out=$(check "$t" reader 2>&1) || true
    if grep -q "reader refuses" <<<"$out"; then echo "SABOTAGE reader: red"; else echo "SABOTAGE reader: STAYED GREEN"; fail=1; fi
  fi
  check "$scratch/good" >/dev/null || { echo "self-test: the unsabotaged install is not green" >&2; exit 1; }
  [ "$fail" = 0 ] || exit 1
  echo "contract-dir self-test: every sabotage red"
  exit 0
fi

install_into "$scratch/dir" || { echo "contract-dir: make install-contract failed" >&2; exit 2; }
if out=$(check "$scratch/dir"); then
  echo "contract-dir: $(described | wc -l) descriptors installed under 3/omx-plugins/plugins/, byte for byte, listed in MANIFEST.json, read by omx-contract's reader"
else
  printf '%s\n' "$out"; exit 1
fi
