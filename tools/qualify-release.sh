#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/qualify-release.sh <out-dir>
#
# The release gate: every CLAP and every LV2 binary the tree builds gets plugin-qualify's real-time-safety verdict,
# and the release goes ahead only when each one is `suitable`. It builds the tree, installs it into a
# staging directory, runs plugin-qualify over every installed `.clap` and every installed LV2 bundle (its `.so`) and checks the qualification
# records it wrote (`<id>@<version>-<sha256 first 12>.<offer>.qualification.json`, each beside the byte
# copy of the probe output it judged), so the verdicts that gate the release are the ones that ship
# beside it. A record belongs to a plugin by the full SHA-256 of the binary it names.
#
#   - a plugin whose record holds a verdict other than `suitable` (or one not qualified, or not
#     measured) refuses the release, named with its verdict;
#   - an installed `.clap` or LV2 `.so` with no record refuses it too (a prober that wrote nothing is not a pass);
#   - a record of a schema this gate does not know, or whose probe output copy is missing or is not
#     the bytes the record names, refuses it;
#   - the LV2 binaries are judged the same way, one record per `<bundle>.lv2/<binary>.so` by its SHA-256 (spec 7.13);
#   - plugin-qualify's own exit code (1 some unqualified, 2 not measurable) refuses it.
#
# Needs the channel packages: plugin-qualify >= 0.11.0 (the first to write qualification records),
# omx-clap-host (`omx-clap-scan`) and the build dependencies of packaging/omx-plugins.spec. `make` is
# skipped when QUALIFY_STAGE names a prefix that already holds a build.
# Exit: 0 every plugin suitable, 1 refused, 2 the gate could not run.
#
#   tools/qualify-release.sh --self-test   a stand-in prober over a two-plugin stage: a pass passes, and
#                                          an unsuitable verdict, a missing record, a `suitable` rating
#                                          that is not qualified, an unknown schema, a missing or altered
#                                          probe output copy and a non-zero prober exit each refuse
set -euo pipefail
HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)

if [ "${1:-}" = --self-test ]; then
  scratch=$(mktemp -d)
  trap 'rm -rf "$scratch"' EXIT
  mkdir -p "$scratch/bin" "$scratch/stage/usr/lib/clap" "$scratch/stage/usr/lib/lv2/a.lv2" "$scratch/stage/usr/lib/lv2/b.lv2"
  printf 'lv2 a\n' > "$scratch/stage/usr/lib/lv2/a.lv2/a.so"
  printf 'lv2 b\n' > "$scratch/stage/usr/lib/lv2/b.lv2/b.so"
  printf 'plugin a\n' > "$scratch/stage/usr/lib/clap/a.clap"
  printf 'plugin b\n' > "$scratch/stage/usr/lib/clap/b.clap"
  printf '#!/bin/sh\nexit 0\n' > "$scratch/bin/omx-clap-scan"
  # the stand-in prober: STUB_MODE says what it writes for the SECOND plugin and how it exits
  cat > "$scratch/bin/plugin-qualify" <<'STUB'
#!/usr/bin/env bash
out=
while [ $# -gt 0 ]; do case "$1" in --out) out=$2; shift 2 ;; *) break ;; esac; done
mkdir -p "$out"
n=0
bins=()
for arg in "$@"; do if [ -d "$arg" ]; then bins+=("$arg"/*.so); else bins+=("$arg"); fi; done
for clap in "${bins[@]}"; do
  n=$((n + 1))
  sha=$(sha256sum "$clap" | cut -c1-64)
  base="org.example.p$n@unversioned-${sha:0:12}.default"
  rating=suitable; qualified=true; schema=1
  case "$STUB_MODE:$n" in
    v9:2) schema=9 ;;
    unsuitable:2) rating=unsuitable; qualified=false ;;
    unqualified:2) qualified=false ;;
    missing:2) continue ;;
    lv2-unsuitable:4) rating=unsuitable; qualified=false ;;
    lv2-missing:4) continue ;;
  esac
  printf '{"format":"plugin-hostd.probe-output","n":%s}\n' "$n" > "$out/$base.probe.json"
  psha=$(sha256sum "$out/$base.probe.json" | cut -c1-64)
  printf '{"schema":"plugin-qualify.qualification-record/%s","binary":{"sha256":"%s"},"verdicts":[{"host":"openmixer-console","measured":true,"rating":"%s","qualified":%s}],"probe":{"file":"%s","sha256":"%s"}}\n' \
    "$schema" "$sha" "$rating" "$qualified" "$base.probe.json" "$psha" > "$out/$base.qualification.json"
  case "$STUB_MODE:$n" in
    no-probe:2) rm "$out/$base.probe.json" ;;
    probe-altered:2) printf 'other bytes\n' > "$out/$base.probe.json" ;;
  esac
done
[ "$STUB_MODE" != exit2 ] || exit 2
exit 0
STUB
  chmod +x "$scratch/bin/omx-clap-scan" "$scratch/bin/plugin-qualify"
  fail=0
  run() { # <mode> <wanted exit> <what> [<text the output must carry>]
    local rc=0 log
    log=$(STUB_MODE=$1 PATH="$scratch/bin:$PATH" QUALIFY_STAGE="$scratch/stage" "${BASH_SOURCE[0]}" "$scratch/out-$1" 2>&1) || rc=$?
    if [ "$rc" = "$2" ]; then echo "ok   $3 (exit $rc)"; else echo "FAIL $3: exit $rc, wanted $2"; printf '%s\n' "$log" >&2; fail=1; fi
    if [ -n "${4:-}" ] && ! grep -qF -- "$4" <<<"$log"; then echo "FAIL $3: the output does not say '$4'"; printf '%s\n' "$log" >&2; fail=1; fi
  }
  run ok 0 "every record suitable: the release goes ahead" "every plugin is suitable (4 binaries: 2 CLAP, 2 LV2)"
  run unsuitable 1 "one unsuitable verdict refuses, naming the plugin" 'REFUSED b.clap'
  run missing 1 "one .clap without a record refuses" 'REFUSED b.clap: no qualification record'
  run unqualified 1 "a suitable rating that is not qualified refuses" 'REFUSED b.clap'
  run v9 1 "a record of a schema this gate does not know refuses" 'REFUSED b.clap'
  run no-probe 1 "a record without its probe output copy refuses" 'REFUSED b.clap'
  run probe-altered 1 "a probe output copy that is not the bytes the record names refuses" 'REFUSED b.clap'
  run lv2-unsuitable 1 "an unsuitable LV2 binary refuses, naming it" 'REFUSED b.so'
  run lv2-missing 1 "an LV2 binary without a record refuses" 'REFUSED b.so: no qualification record'
  run exit2 1 "a non-zero prober exit refuses even with every record suitable" 'plugin-qualify exit 2'
  [ "$fail" = 0 ] && echo "qualify-release self-test ok"
  exit "$fail"
fi

OUT=${1:?usage: qualify-release.sh <out-dir> | --self-test}

for tool in plugin-qualify omx-clap-scan node; do
  command -v "$tool" >/dev/null || { echo "qualify-release: $tool is not installed; install plugin-qualify and omx-clap-host from the channel" >&2; exit 2; }
done

STAGE=${QUALIFY_STAGE:-}
if [ -z "$STAGE" ]; then
  STAGE=$(mktemp -d)
  trap 'rm -rf "$STAGE"' EXIT
  make -C "$HERE" -j"$(nproc)"
  make -C "$HERE" install DESTDIR="$STAGE" LIBDIR=/usr/lib64
fi

CLAPDIR=$(find "$STAGE" -type d -path '*/lib/clap' | head -1)
[ -n "$CLAPDIR" ] || { echo "qualify-release: no lib/clap under $STAGE; nothing was installed" >&2; exit 2; }
mapfile -t CLAPS < <(find "$CLAPDIR" -maxdepth 1 -name '*.clap' | sort)
[ "${#CLAPS[@]}" -gt 0 ] || { echo "qualify-release: $CLAPDIR holds no .clap; the tree built no plugin" >&2; exit 2; }

# the LV2 bundles beside them: plugin-qualify takes a bundle and judges each plugin in it; the binary of each is its .so
mapfile -t LV2DIRS < <(find "$STAGE" -type d -path '*/lib*/lv2/*.lv2' | sort)
LV2SOS=()
for d in "${LV2DIRS[@]}"; do
  mapfile -t so < <(find "$d" -maxdepth 1 -name '*.so' | sort)
  [ "${#so[@]}" -gt 0 ] || { echo "qualify-release: $d holds no .so; the bundle has no binary to qualify" >&2; exit 2; }
  LV2SOS+=("${so[@]}")
done
[ "${#LV2SOS[@]}" -gt 0 ] || { echo "qualify-release: no LV2 bundle under $STAGE; the tree installed none" >&2; exit 2; }
BINS=("${CLAPS[@]}" "${LV2SOS[@]}")

mkdir -p "$OUT"
rc=0
plugin-qualify --out "$OUT" "${CLAPS[@]}" "${LV2DIRS[@]}" || rc=$?

# The records are the evidence: each read here rather than trusted from the exit code.
node - "$OUT" "${BINS[@]}" <<'JS' || exit_js=$?
const { existsSync, readdirSync, readFileSync } = require('node:fs');
const { createHash } = require('node:crypto');
const { join, basename } = require('node:path');
const [out, ...bins] = process.argv.slice(2);
const sha256 = (bytes) => createHash('sha256').update(bytes).digest('hex');
const records = readdirSync(out).filter((f) => f.endsWith('.qualification.json')).map((f) => ({ f, r: JSON.parse(readFileSync(join(out, f), 'utf8')) }));
// plugin-qualify.qualification-record/1: binary.sha256 is the binary judged, verdicts[] one per host profile
// (rating suitable | unsuitable | unknown, qualified, measured), probe the copy of the probe output judged
const SCHEMA = 'plugin-qualify.qualification-record/1';
const problemOf = (r) => {
  if (r.schema !== SCHEMA) return `unknown schema ${JSON.stringify(r.schema ?? null)}`;
  const copy = typeof r.probe?.file === 'string' ? join(out, basename(r.probe.file)) : null;
  if (copy === null || !existsSync(copy)) return 'no probe output beside it';
  if (sha256(readFileSync(copy)) !== r.probe.sha256) return 'its probe output copy is not the bytes it names';
  if (!Array.isArray(r.verdicts) || r.verdicts.length === 0) return 'no verdict';
  for (const v of r.verdicts) {
    if (v.rating !== 'suitable') return `${v.host}: ${v.rating ?? 'absent'}`;
    if (v.qualified !== true || v.measured !== true) return `${v.host}: suitable, but not qualified and measured`;
  }
  return null;
};
let refused = 0;
for (const clap of bins) {
  const sha = sha256(readFileSync(clap));
  const mine = records.filter((x) => x.r.binary?.sha256 === sha);
  if (mine.length === 0) { console.log(`REFUSED ${basename(clap)}: no qualification record for sha256 ${sha}`); refused++; continue; }
  for (const { f, r } of mine) {
    const p = problemOf(r);
    if (p !== null) { console.log(`REFUSED ${basename(clap)}: ${f} says ${JSON.stringify(p)}`); refused++; }
    else console.log(`suitable ${basename(clap)}: ${f}`);
  }
}
process.exit(refused === 0 ? 0 : 1);
JS
exit_js=${exit_js:-0}

if [ "$rc" -ne 0 ] || [ "$exit_js" -ne 0 ]; then
  echo "qualify-release: REFUSED (plugin-qualify exit $rc, document check exit $exit_js): no release without a suitable verdict for every plugin" >&2
  exit 1
fi
echo "qualify-release: every plugin is suitable (${#BINS[@]} binaries: ${#CLAPS[@]} CLAP, ${#LV2SOS[@]} LV2)"
