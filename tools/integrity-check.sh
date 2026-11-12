#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/integrity-check.sh <contractdir> <version> <commit> — a package build's hold on the descriptors it ships (#32):
# every <contractdir>/3/omx-plugins/plugins/*.json carries integrity { version, commit }, its version this release's
# and its plugin's own, its commit the one the build's source was made from (the RPM's tarball, the deb's checkout,
# which the build scripts hold to the release tag). Run by the spec's %check and by debian/rules after the install.
# Exit: 0 pass, 1 refused (one line per descriptor), 2 the check could not run.
set -euo pipefail
[ $# = 3 ] || { echo "usage: integrity-check.sh <contractdir> <version> <commit>" >&2; exit 2; }
dir=$1/3/omx-plugins/plugins version=$2 commit=$3
[[ $commit =~ ^[0-9a-f]{40}$ ]] || { echo "integrity-check: $commit is no full commit" >&2; exit 2; }
[ -d "$dir" ] || { echo "integrity-check: $dir does not exist" >&2; exit 2; }
node -e '
  const fs = require("fs"); const path = require("path");
  const [dir, version, commit] = process.argv.slice(1);
  const files = fs.readdirSync(dir).filter((f) => f.endsWith(".json")).sort();
  let bad = 0;
  if (!files.length) { console.log(`integrity-check: ${dir} holds no descriptor`); process.exit(1); }
  for (const f of files) {
    const d = JSON.parse(fs.readFileSync(path.join(dir, f), "utf8"));
    const i = d.integrity;
    const why = !i ? "carries no integrity"
      : i.commit !== commit ? `names commit ${i.commit}, the source is ${commit}`
      : i.version !== version ? `names version ${i.version}, this release is ${version}`
      : i.version !== d.plugin?.version ? `names version ${i.version}, its plugin states ${d.plugin?.version}` : "";
    if (why) { console.log(`integrity-check: ${f} ${why}`); bad++; }
  }
  if (bad) process.exit(1);
  console.log(`integrity-check: ${files.length} descriptors carry ${version} and ${commit.slice(0, 12)}`);
' "$dir" "$version" "$commit"
