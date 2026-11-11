# `make test` profile (omx-plugins#19)

Measured on a 4 vCPU worker (same vCPU count the issue's CI numbers used), fresh checkout, no
build cache. Absolute seconds here are not CI's wall clock (the sandbox is faster than the GitHub
Actions runner, and the real job is also slowed by fedora:44's container overhead), but the
*relative* split is the answer the ruling asks `make test` profiling to give.

## The ~30 top-level named targets (prerequisites of `test:` plus its explicit `$(MAKE) -s` steps)

All but one are pure Node/script checks against the declarations, not the built plugin, and all but
one run well under ten seconds:

```
lib 10.0s  units 1.0s  panel-words 0.8s  face-header 0.4s  role-meanings 0.1s  meter-binding 5.2s
panel-layout 4.0s  descriptor-view 7.4s  descriptor-features 0.1s  placement-strips 0.1s
lv2-units 3.3s  lv2-deprecated 2.4s  lv2-maintainer 2.4s  lv2-manifest 2.4s  release-version 0.1s
resource-hints 2.6s  format-tables 6.7s  contract-union 2.0s  contract-dir 6.6s  lv2bench-ran 0.0s
probe-checks 74.6s  eq-probe-race 1.0s  band-groups 1.0s  phaser-depth 0.1s  geq-widgets 0.9s
summary-numbers 1.2s  param-tie 0.9s  face-ids 4.8s  strip-visible-when 1.0s  face-dirty-order 0.1s
presets-figures 1.1s  lib-test 0.5s  hints 114.0s  manual 3.3s  presets 0.2s  host-contract 2.0s
meter-level 0.9s  strip-bands 5.4s  test-role 23.9s  package-lists 48.5s
round-trip ~52.5s to the first plugin (see below)
```

`lv2bench-ran` and `eq-probe-race` (named in the issue as suspects) turned out to be fixture tests
against a parser and a fake compiler, not real lv2bench/gcc runs: fast, and nothing to cut.
`probe-checks`, `hints`, `package-lists` and `test-role` are slower than the rest but stay pure
script/Node work against declarations and staged installs — never load the compiled plugin into any
host, so they stay in the ruling's "(a) pure Node or script checks" bucket.

`round-trip` is the one top-level target that spawns a real separate host process
(`plugin-hostd-probe describe`), twice per plugin (CLAP and LV2). It failed on the very first plugin
in this sandbox on an unrelated contract-field mismatch between the installed plugin-hostd 0.20.2 and
this tree's declarations (pre-existing, not touched by this change — the CI matrix's exact package
combination evidently differs). Even failing after one plugin it took 52.5s; at ~30 plugins the real
total is the largest single top-level cost. → `test-heavy`.

## The per-plugin loop (`for p in $(PLUGINS); do $(MAKE) -C $p test; done`)

37 plugins, each running identical steps. Sampled: 10.7s–50.0s per plugin (omx-balancer 16.8s,
omx-chorus 11.6s, omx-clipper 10.7s, omx-comp 24.6s, omx-deesser 12.8s, omx-delay 50.0s,
omx-doubler 12.0s, omx-drive 14.9s, omx-ducker 14.1s), and omx-dyneq alone ran past 150s before this
profiling run was stopped in favour of a step-by-step breakdown (below) once the shape was clear.

Breaking down one plugin (omx-balancer, build already warm) by step:

```
clap-probe params     0.00s      panel-probe clap     0.00s      port-groups-probe   0.00s
clap-probe live       0.06s      panel-probe lv2       0.00s      preset-probe        0.00s
clap-probe state      0.01s      clap-ext-probe        0.00s
balancer-oracle clap   3.34s      host-check.sh (clap-scan/lv2ls/lv2info)   0.012s
balancer-oracle lv2    2.48s      modgui-test.sh (omx-panel)                0.84s
```

The kernel-identity oracle (`{{kernel}}-oracle`, generated from `oracle.c.tmpl`: both faces against
omx-dsp's kernel, every parameter one step off, at every declared rate) is 5.8 of the ~14.5s total —
and it is the one step whose cost scales with a plugin's parameter/band count: a plugin with few
parameters (omx-balancer) costs ~6s here, a plugin with many (omx-dyneq: dynamic EQ, many
bands × dynamics controls) ran past 150s **in this one step alone**. `host-check.sh` (real external
scanners: omx-clap-scan, lv2ls, lv2info) and `modgui-test.sh` (omx-panel's MOD GUI render/validate,
also a real external host) are genuinely "host-loading" by the issue's own words, but together they
cost under a second per plugin — not the bottleneck the issue's title suggested, just the same
category of cost (running the built plugin through something other than this repo's own
declaration-reading scripts) as the oracle sweep, at a smaller scale.

## The cut

Everything that only reads a declaration/JSON/TTL and never loads the built plugin into anything
that runs it (the ~29 fast named targets, `probe-checks`, `hints`, `package-lists`, `test-role`, and
the per-plugin `clap-probe`/`panel-probe`/`clap-ext-probe`/`port-groups-probe`/`preset-probe` —
custom, in-process, sub-10ms probes this tree wrote for itself) stays in `make test`.

Everything that runs the built plugin through a real host, in- or out-of-process — the per-plugin
kernel-identity oracle (both faces, every declared extra oracle), `host-check.sh`,
`modgui-test.sh`, and the top-level `round-trip` — moves to `make test-heavy`, run on every PR in
its own CI job(s), in parallel, never inside the package `%check`.

Nothing is weakened: every one of these still runs, just not inside `make test`; the new
`test-heavy-conformance` check (`tools/test/test-heavy-conformance.mjs`) is red if a target is ever
dropped from both.

No target in `make test` sleeps or polls on a timeout (the only `sleep`/`usleep` in `tools/` is
`meter-ring-stress.c`, already outside `make test`), so the ruling's item 2(c) has nothing to do here.
