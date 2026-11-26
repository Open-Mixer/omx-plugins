<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com> -->
# Recipes

`plugin.recipe.json` is the procedure for adding a plugin, as data: every artifact a complete
plugin needs (with its wizard step, its commit layer, its template and the checker that reads it)
and the laws every plugin keeps. `tools/omx-new-plugin.mjs` writes a new plugin from it,
`make completeness` holds every plugin to it, and `tools/commit-plan-check.mjs` holds a plugin's
commits to its layer order. `completeness-debt.json` lists the gaps owed today; it only shrinks: `make completeness` fails when
it holds more entries than at the merge-base with `origin/main`.

- A plugin declares `kernels: [..]`, the omx-contract kernels its parameters reference: one entry
  is the normal case, several make a composite (omx strip), and then each parameter's `ref` names
  its kernel with `"kernel"` (ruling 10-07, by recommendation).
- Every plugin declares each parameter by `ref`, with no typed `min`/`max`/`def`/`unit`/`kind`, or
  `params-by-reference` goes red for it.
