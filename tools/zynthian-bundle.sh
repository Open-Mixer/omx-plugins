#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# Pack the plugins for a Zynthian (ZynthianOS Vangelis, Debian trixie, arm64) as one tarball, from the
# trixie DEBs the release workflow has just built. Nothing is rebuilt: the tarball carries exactly the
# files of omx-plugins-lv2 and omx-plugins-clap, laid out for Zynthian's plugin directories.
#
#   tools/zynthian-bundle.sh DEB_DIR ARCH OUT_DIR
#
# DEB_DIR holds omx-plugins-lv2_*_ARCH.deb and omx-plugins-clap_*_ARCH.deb (searched recursively).
# ARCH is the Debian architecture the DEBs were built for: arm64 at release, amd64 in the pull-request
# dry run, which packs and checks the same way. Writes OUT_DIR/omx-plugins-zynthian-VERSION-ARCH.tar.gz
# and its .sha256.
#
# Refused, before anything is packed: a missing DEB, an ELF of another architecture, a glibc symbol
# version above trixie's 2.41, a NEEDED library a bare trixie does not have, a CLAP without
# clap_entry, and an LV2 plugin lilv cannot find, describe, instantiate and run (lv2bench). The probes
# need make, file, binutils and lilv-utils.
set -euo pipefail

die() { echo "zynthian-bundle: REFUSED: $*" >&2; exit 1; }

[ $# -eq 3 ] || { echo "usage: $0 DEB_DIR ARCH OUT_DIR" >&2; exit 2; }
debs=$1 arch=$2 out=$3
case "$arch" in
  arm64) elf='ARM aarch64' ;;
  amd64) elf='x86-64' ;;
  *) die "unknown architecture '$arch' (arm64 or amd64)" ;;
esac

# trixie's libc6 is 2.41: Zynthian Vangelis's floor
CEIL_GLIBC=2.41
# libmvec is glibc's vector maths, in libc6 beside libm: an auto-vectorised sinf/expf loop links it
NEEDED_OK=" libc.so.6 libm.so.6 libmvec.so.1 libdl.so.2 libpthread.so.0 libgcc_s.so.1 libstdc++.so.6 ld-linux-aarch64.so.1 ld-linux-x86-64.so.2 "

root=$(cd "$(dirname "$0")/.." && pwd)
version=$(make -s --no-print-directory -C "$root" version)
name="omx-plugins-zynthian-$version-$arch"
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

find_deb() {
  local found
  found=$(find "$debs" -name "$1_*_$arch.deb" | sort)
  [ -n "$found" ] || die "no $1 DEB for $arch under $debs"
  [ "$(wc -l <<<"$found")" -eq 1 ] || die "more than one $1 DEB for $arch under $debs: $found"
  echo "$found"
}
lv2deb=$(find_deb omx-plugins-lv2)
clapdeb=$(find_deb omx-plugins-clap)
dpkg-deb -x "$lv2deb" "$work/lv2"
dpkg-deb -x "$clapdeb" "$work/clap"

stage="$work/stage/$name"
mkdir -p "$stage/lv2" "$stage/clap"
compgen -G "$work/lv2/usr/lib/lv2/*.lv2" >/dev/null || die "$lv2deb has no /usr/lib/lv2/*.lv2"
compgen -G "$work/clap/usr/lib/clap/*.clap" >/dev/null || die "$clapdeb has no /usr/lib/clap/*.clap"
cp -a "$work"/lv2/usr/lib/lv2/*.lv2 "$stage/lv2/"
cp -a "$work"/clap/usr/lib/clap/*.clap "$stage/clap/"
cp "$root/LICENSE" "$stage/"

# ver_gt A B: A sorts strictly after B
ver_gt() { [ "$1" != "$2" ] && [ "$(printf '%s\n%s\n' "$1" "$2" | sort -V | tail -1)" = "$1" ]; }

bad=0 max=0
mapfile -t elfs < <(find "$stage" -type f \( -name '*.so' -o -name '*.clap' \) | sort)
[ "${#elfs[@]}" -gt 0 ] || die "no ELF in the bundle"
for f in "${elfs[@]}"; do
  rel=${f#"$stage"/}
  file -bL "$f" | grep -q "^ELF 64-bit LSB shared object, $elf" ||
    { echo "REFUSED $rel: $(file -bL "$f")" >&2; bad=1; continue; }
  # a file with no GLIBC_ version at all means objdump saw nothing: refused, not passed
  vers=$(objdump -T "$f" | grep -oE 'GLIBC_[0-9][0-9.]*' | sort -uV || true)
  [ -n "$vers" ] || { echo "REFUSED $rel: objdump -T shows no GLIBC_ version" >&2; bad=1; continue; }
  for v in $vers; do
    num=${v#GLIBC_}
    ver_gt "$num" "$CEIL_GLIBC" && { echo "REFUSED $rel: needs $v, above trixie's GLIBC_$CEIL_GLIBC" >&2; bad=1; }
    ver_gt "$num" "$max" && max=$num
  done
  for lib in $(objdump -p "$f" | awk '$1 == "NEEDED" { print $2 }'); do
    case "$NEEDED_OK" in *" $lib "*) ;; *) echo "REFUSED $rel: NEEDED $lib is not on a bare trixie" >&2; bad=1 ;; esac
  done
  case "$f" in
    *.clap) objdump -T "$f" | grep -qw clap_entry || { echo "REFUSED $rel: no clap_entry symbol" >&2; bad=1; } ;;
  esac
done
[ "$bad" = 0 ] || die "ELF check failed (above)"

# every bundle found and described by lilv from the tarball's own directory, nothing else on the path
lv2s=0
for b in "$stage"/lv2/*.lv2; do
  uri=$(sed -n 's/^<\(urn:openmixer:[^>]*\)>.*/\1/p' "$b/manifest.ttl" | head -1)
  [ -n "$uri" ] || die "${b#"$stage"/} names no urn:openmixer: plugin in its manifest.ttl"
  listed=$(LV2_PATH="$stage/lv2" lv2ls)
  grep -qx "$uri" <<<"$listed" || die "lv2ls does not list $uri from ${b#"$stage"/}"
  info=$(LV2_PATH="$stage/lv2" lv2info "$uri")
  grep -q 'Binary:' <<<"$info" || die "lv2info shows no binary for $uri"
  lv2s=$((lv2s + 1))
done
# and every plugin instantiated and run on this architecture: lv2bench loads each binary and processes
# audio through it (either shape of its output: tools/lv2bench-ran.sh), so a plugin that cannot load on the target is
# refused here, not on the Zynthian. lv2bench's own exit status is not a per-plugin signal: Fedora's lilv 0.26.4
# exits nonzero the moment ANY plugin on LV2_PATH fails to instantiate, even one of ours that ran fine, so it must
# not gate this script (`|| true`); the per-URI check below, on the captured text, is what decides.
bench=$(LV2_PATH="$stage/lv2" lv2bench) || true
for b in "$stage"/lv2/*.lv2; do
  uri=$(sed -n 's/^<\(urn:openmixer:[^>]*\)>.*/\1/p' "$b/manifest.ttl" | head -1)
  "$root/tools/lv2bench-ran.sh" "$uri" <<<"$bench" || die "lv2bench did not run $uri"
done
claps=$(find "$stage/clap" -name '*.clap' | wc -l)

cat >"$stage/README" <<EOF
omx-plugins $version for Zynthian ($arch, Debian trixie / ZynthianOS Vangelis, glibc >= 2.41)

The files of the omx-plugins-lv2 and omx-plugins-clap $version packages for trixie, as a tarball.
Highest glibc symbol: GLIBC_$max. $lv2s LV2 bundles, $claps CLAP plugins.

lv2/   copy each .lv2 directory into /zynthian/zynthian-plugins/lv2 (Zynthian's own LV2 directory;
       system-wide: /usr/lib/lv2), then rescan the LV2 plugins from Zynthian's admin menu.
clap/  copy each .clap into /usr/lib/clap (or ~/.clap) for a CLAP host.

On a Debian or Raspberry Pi OS system with apt, the OpenMixer channel's packages are the better way:
https://www.openmixer.org/deb
EOF

mkdir -p "$out"
tarball="$name.tar.gz"
tar -C "$work/stage" --owner=0 --group=0 --numeric-owner --sort=name -czf "$out/$tarball" "$name"
(cd "$out" && sha256sum "$tarball" >"$tarball.sha256")
echo "zynthian-bundle: $out/$tarball: $lv2s LV2 bundles, $claps CLAP, ${#elfs[@]} ELF, max GLIBC_$max"
