# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# Top-level build: every plugin under plugins/<name>/ has its own Makefile with `all`, `test`,
# `install` and `clean`.
#
#   make            build every plugin
#   make test       the fast set (what the package builds run): declaration checks, in-process probes, hints, manual;
#                   every part runs whatever an earlier part answered (the fast checks with -k, every plugin, each step)
#   make test-heavy the built plugins through a host: oracles, host-check.sh, the round trip, the curve proof; CI runs
#                   it on every pull request in slices (OMX_HEAVY_SHARD=i/n); tools/test/test-split.json lists both
#   make units      the declared units' contract codes (tools/param-contract.mjs) are omx-clap's header's
#                   (OMX_PARAM_CONTRACT_UNITS, omx-clap-host/omx_param_units.h), its vendor block is dbfs, dbu, ratio, lufs, a unit
#                   changed on either side red (tools/test/param-units-check.mjs)
#   make lv2-units  every unit a declaration uses has an LV2 unit, and every port states the unit it declares
#                   (tools/test/lv2-units-check.mjs, with its sabotage arms; part of `make test`)
#   make lv2-maintainer  every generated TTL gives its doap:maintainer a foaf:homepage (tools/test/lv2-maintainer-check.mjs; part of `make test`)
#   make no-modgui  omx-plugins ships no generated MOD GUI: no tracked MOD GUI file, no manifest pointing at one, no build step that
#                   writes or checks one (tools/test/no-modgui-check.mjs; part of `make test`); a host generates it (omx-modgui-sync)
#   make format-tables  the generators read plugin-hostd's installed format tables backwards (tools/format-tables.mjs):
#                   the units, the 169 lv2:designation lines and the ratio unit, the CLAP modules from the descriptors'
#                   groups (tools/test/format-tables-test.mjs, with its sabotage arms; part of `make test`)
#   make round-trip  every described plugin generated, both faces built and scanned back by plugin-hostd-probe describe,
#                   compared with its authored descriptor word by word: equal, lost <format> <word> or differs
#                   (tools/round-trip.{sh,mjs}, with its two sabotage arms; part of `make test`)
#   make curve-proof  every display a declaration marks `basis: exact` is measured: omx-comp's transfer curve and
#                   omx-eq8/16/32's EQ response at three settings each, at 48 and 96 kHz, the curve omx-dsp's
#                   omx_curve_points draws over the description's roles against the plugin's static response through
#                   libomx-clap (a level sweep, a sine sweep), within 0.5 dB at every point; a display marked exact and
#                   not measured, or measured and not marked, refused (tools/test/curve-proof.{mjs,c}, with its
#                   sabotage arms; part of `make test-heavy`, in its first slice)
#   make contract-union  parallel make targets render omx-contract's union into private directories and read it whole
#                   (tools/test/contract-union-race-test.mjs; part of `make test` and `make test-faces`)
#   make face-header  include/omx_plugin_face_ids.h is contract/plugin-face.json's render, byte for byte with the omx-contract
#                   command (tools/test/face-header-check.mjs; part of `make test`)
#   make face-ids   the host's bypass and LV2 enabled and latency sit at the permanent ids of contract/plugin-face.json (OMX_PLUGIN_FACE_IDS), none
#                   moved by an appended parameter (tools/test/face-ids-test.mjs, with its self-test; part of `make test`)
#   make probe-checks  clap-params-check, panel-check, clap-ext-check and port-groups-check each fail on a built face that is wrong,
#                   one change at a time (tools/test/probe-checks-selftest.mjs; part of `make test`)
#   make eq-probe-race  parallel make targets each build build/eq-default-probe privately and run it whole
#                   (tools/test/eq-probe-race-test.mjs; part of `make test` and `make test-faces`)
#   make own-units   a plugin's own units (its declaration's `units` block) flow to every face as a standard unit does, and
#                   an own name that is PARAM_UNITS', or a parameter naming an undeclared unit, is refused
#                   (Open-Mixer/omx-plugins#172; tools/test/own-units-test.mjs; part of `make test`)
#   make param-tie   every by-reference parameter's symbol is its control's words or says its `control`: swapped refs are refused
#                   (FreeMixer/omx-plugins#58; tools/test/param-tie-test.mjs; part of `make test`)
#   make summary-numbers  a plugin summary reads its numbers from the declaration: none types a limit or default of its parameters,
#                   and a moved contract moves the catalogue row (FreeMixer/omx-plugins#174; tools/test/summary-numbers-test.mjs; part of `make test`)
#   make phaser-depth  omx-phaser's generated panel publishes its depth role on depthOct, in octaves (FreeMixer/omx-plugins#232;
#                   tools/test/phaser-depth-role-test.mjs and its --self-test; part of `make test`)
#   make strip-visible-when  omx-strip's panel follows its band count: at eqBands 4 bands 1..4 are shown and 5..32
#                   hidden, and no page holds a hidden band (tools/test/strip-visible-when-test.mjs; part of `make test`)
#   make strip-pages-follow  omx-strip's CLAP remote-control pages follow Bands: 4, 32, 9 and 4 bands read back through the
#                   extension from the built face, the host told by changed() (tools/test/strip-pages-follow-test.mjs; part of `make test`)
#   make band-groups  omx-eq8's generated descriptor and TTL hold one LV2 port group per band, Band 1 ... Band 8 in band
#                   order, each its five controls (tools/test/eq8-band-groups-test.mjs; part of `make test`)
#                   and every plugin whose model repeats a band of several controls holds one port group per band in
#                   its generated TTL, read by serdi, each group its lv2:symbol, lv2:name and rdfs:label: omx-eq8, 16
#                   and 32, omx-dyneq, omx-multiband, and omx-strip's EQ 1 ... EQ 32
#                   (tools/test/band-port-groups-check.mjs, with its sabotage arms)
#   make geq-widgets  omx-geq declares the fader for its 31 bands, and its panel description and generated header serve them
#                   (tools/test/geq-widgets-test.mjs; part of `make test`)
#   make decl-schema-panel-ref  the declaration schema reaches omx-panel's definitions by panel.schema.json's name, under an
#                   omx-panel whose schema prints another $id (tools/test/decl-schema-panel-ref-test.mjs; part of `make test`)
#   make hints      no LV2 face lost a port hint: tools/port-hints.mjs against each declaration's `portHints` pin
#   make manual     each declaration's manual follows omx-contract: a travel moved in a copy of the release moves
#                   the catalogue section's range and default (tools/test/manual-perturb.mjs)
#   make params-doc  write docs/plugins/: one page of every parameter for each plugin and the index (tools/params-doc.mjs, from the declarations)
#   make check-params-doc  the committed pages are what the declarations generate, and say what the generated TTLs say (tools/test/params-doc-test.mjs); run by CI beside `node tools/gen.mjs --check`
#   make panel-copies  no tracked file is a film strip or a fixture omx-panel renders or ships: `omx-panel copies`
#                      over the tree, with its --self-test; run by CI beside `make test`
#   make recipe-test   the repository's own checks, run by CI beside `make test`:
#     make completeness  every plugin against recipes/plugin.recipe.json, each gap naming its wizard step;
#                        a gap not in recipes/completeness-debt.json fails, and so does a paid debt entry
#     make selftest      the recipe's checkers, wizard and commit protocol, each sabotaged (needs git)
#   make meter-held  a built face (omx-comp) keeps a 10 ms burst of level, gain reduction and clip until the host reads it, at
#                   every oracle rate and quantum, and each sabotage of the ring goes red (tools/meter-held-check.sh; part of
#                   `make test`); `make meter-rows` holds the generator's held and clip words, and `make meter-words`
#                   (part of `make test`) the descriptors' held, clip and measurement (omx-contract's descriptorMeter)
#   make meters-abi  the clap.meters/1 header omx-clap installs has the id, enums, sizeof and offsetof the
#                    plugins are built for (tools/test/meters-abi-test.c, compile and run; part of `make test`)
#   make param-contract-abi  include/omx_param_contract.h's extension table and entry are the layout omx-clap's
#                    omx_clap_ext.h reads (tools/test/param-contract-abi-test.c, compile and run; part of `make test`)
#   make panel-abi   include/omx_panel_description.h's structures, codes and omx_panel_get read-back are the layout
#                    omx-clap's installed omx_clap_ext.h declares (tools/test/panel-abi-test.c, compile and run;
#                    part of `make test`)
#   make comp-published-scan  omx-comp.clap as built, read by omx-clap-scan: its four meters and four contract rows by
#                    value, each expected value changed once and found (tools/test/published-comp-scan.py; part of
#                    `make test` and `make test-faces`; OMX_CLAP_SCAN names the scanner)
#   make host-contract  every shipped face opened by libomx-clap, whose own fingerprint of what get_info and the
#                    contract serve must match the face's: omx_clap_host_param_contract() trusts it
#                    (tools/test/host-contract-accept.c; part of `make test` and `make test-faces`)
#   make strip-crosscheck   omx-strip, a chain, against the hand-ported strip it replaced, bit for bit at
#                    the old plugin's settings (tools/test/strip-crosscheck.sh; needs the git history, and builds the
#                    omx-contract and omx-dsp releases the old commit pinned, from sibling checkouts' tags or GitHub).
#                    Known gap: the low-pass above 0.6 of Nyquist at 44.1 and 48 kHz is not compared (omx-dsp's hand-over,
#                    omx-dsp#105/#116/#119, removed a passband lift the hand-ported strip had); the map's @cap line says so
#   make strip-bands  omx-strip's band count (#308) against the strip before it had one, bit for bit at every declared
#                   rate: at its default count with every band past it boosted, and with the extra bands flat, it is
#                   that strip; with the count reaching a boosted band it is not; built to process every band
#                   whatever the count it is not (the sabotage); and that strip's factory presets, as presets and as the
#                   sessions it saves, load into this one with the same values and output (tools/test/strip-bands-check.sh,
#                   tools/test/clap-session-crosscheck.c; needs the git history; part of `make test`)
#   make strip-bands-bench  omx-strip's CPU cost by band count (4, 8, 16, 32, and its EQ off) through the built CLAP
#                   face, at BENCH_RATES (default 48000 96000): microseconds per 128-frame block and the share of a core
#                   (tools/test/strip-bands-bench.c; a figure for one machine, not part of `make test`)
#   make presets    the factory presets: fresh against tools/gen-presets.mjs, each value inside its declared travel (part of `make test`)
#   make presets-figures  the measured figures of the console's two catalogue presets, through omx-dsp's instance faces: the
#                   amp-input bell (+4.00 dB at 2.5 kHz), the amp-tube harmonics (H2/H3 on the closed form), the dimension's
#                   L - R null; each sabotaged by --self-test (tools/test/preset-figures.mjs; part of `make test` and `make test-faces`)
#   make install    DESTDIR, PREFIX, LIBDIR, CLAPDIR, DATADIR: $(CLAPDIR)/<name>.clap, $(LIBDIR)/lv2/<name>.lv2/ and the
#                   factory presets in $(DATADIR)/omx/presets/<plugin id>/<preset id>.json
#   make install-contract  DESTDIR, CONTRACTDIR: each shipped plugin's descriptor into omx-contract's contract directory,
#                   $(CONTRACTDIR)/3/omx-plugins/plugins/<stem>.json and its MANIFEST.json, and the labels domain,
#                   $(CONTRACTDIR)/omx-plugin-labels/ with the catalogs (run by install-devel; alone: install-labels-domain)
#   make install-catalogs  DESTDIR, DATADIR: the labels' catalogs, $(DATADIR)/omx-plugins/catalogs/<lang>.json, one per
#                   listed language (run by install; each is its language pack's)
#   make plugin-labels  every shipped plugin's section titles, choice labels and preset names are each a key of the
#                   labels domain (contract.d/omx-plugin-labels, written by tools/gen.mjs) with its English source
#                   sentence, no parameter name is a key, every listed language has its catalog, and omx-contract
#                   validate reads the domain with its catalogs (tools/test/plugin-labels-check.mjs and
#                   plugin-labels-validate.sh, with their sabotage arms; part of `make test`)
#   make catalogs-gate  the catalogs through omx-contract's release gate, translate --check: a listed language with no
#                   catalog, a key a catalog lacks or a stale entry is red (tools/catalogs-gate.sh; CI's language packs step)
#   make contract-dir  the installed tree carries one descriptor per shipped plugin that has one, byte for byte, and
#                   omx-contract's reader reads it (tools/test/contract-dir-check.sh, with its sabotage arms; part of `make test`)
#   make lib        build/lib/libomxplugins.a: every shipped plugin's CLAP face and the table of their
#                   entries (include/omx_clap_static.h), for a host that runs them in-process
#   make lib-test   a C host linked against the archive opens every face through the table, then
#                   again through the installed omxplugins.pc (part of `make test`)
#   make install-devel   the public headers other projects include: $(INCLUDEDIR)/omx-plugins/*.h,
#                   and $(LIBDIR)/libomxplugins.a with $(LIBDIR)/pkgconfig/omxplugins.pc
#   make clap-validate  clap-validator on every built .clap (tools/clap-validate.sh): one JSON report each in build/clap-validator;
#                   the factory presets must be installed (make install-presets); in CI a failed test fails the job, except the one
#                   accepted failure (activate refusing a rate outside the nine oracle rates; tools/clap-validate-summary.mjs), which is printed as `accepted`
#   make release-version  every generated header, descriptor and LV2 plugin file carries VERSION, with its sabotage arms
#                   (tools/test/release-version-check.mjs; part of `make test`)
#   make version    the release version, the one packaging/omx-plugins.spec and debian/changelog carry
#   make install-test  DESTDIR, PREFIX, TESTCLAPDIR: each test plugin's .clap in $(TESTCLAPDIR), outside every
#                   CLAP search path (default $(PREFIX)/libexec/omx-plugins/test), for the omx-plugins-test package:
#                   a host's tests and the qualifier open them there by path; no host or picker finds them
#   make test-role  a test plugin (`"role": "test"`) is linked into the archive and `make install` never
#                   installs it: tools/test-role-check.sh over a staged install, with its --self-test
#                   (part of `make test`)
#   make package-lists  every plugin the build makes is installed and listed by exactly the package it belongs
#                   to, in the RPM spec and the Debian .install files: tools/package-lists-check.sh over a staged
#                   install, with its --self-test (part of `make test`)
VERSION := 0.21.0
PLUGINS := $(sort $(dir $(wildcard plugins/*/Makefile)))
# A test plugin is built, tested and linked into libomxplugins.a like the others; `make install` never installs
# it, and `make install-test` puts it outside the CLAP path: its declaration says `"role": "test"`
# (tools/test-plugins.mjs).
TEST_PLUGINS := $(sort $(dir $(shell grep -l '"role": *"test"' plugins/*/*.decl.json 2>/dev/null)))
SHIP_PLUGINS := $(filter-out $(TEST_PLUGINS),$(PLUGINS))
PREFIX     ?= /usr
INCLUDEDIR ?= $(PREFIX)/include
LIBDIR     ?= $(PREFIX)/lib
TESTCLAPDIR ?= $(PREFIX)/libexec/omx-plugins/test
DATADIR    ?= $(PREFIX)/share
# the path every plugin's own Makefile installs its .clap to (the CLAP specification's, whatever LIBDIR is)
CLAPDIR    ?= $(PREFIX)/lib/clap
# Every header a consumer includes: the shared parameter type, the panel description and parameter contract rows, then each shipped plugin's generated
# parameter header and its instance header (the DSP shell around the omx-dsp kernel). A plugin's
# headers are the .h files beside its Makefile and in its generated/ directory.
# A test plugin's own hand-written header (an extension a host's test reads, omx_test_transport_probe.h)
# is installed with them, and so is its generated generated/omx_<kernel>_decl.h: the constants a
# proof predicts from (and the CLAP id) have one home, the declaration, not a hardcoded second copy
# in the host that reads it. Neither declares anything a picker lists: macros only.
HEADERS := include/omx_plugin_param.h include/omx_panel_description.h include/omx_param_contract.h include/omx_preset_file.h include/omx_clap_static.h include/omx_plugin_face.h include/omx_plugin_face_ids.h $(foreach p,$(SHIP_PLUGINS),$(wildcard $(p)*.h) $(wildcard $(p)generated/*.h)) $(foreach p,$(TEST_PLUGINS),$(wildcard $(p)*.h) $(wildcard $(p)generated/*.h))

# libomxplugins.a: each shipped plugin's CLAP face built with -DOMX_CLAP_STATIC (no `clap_entry`;
# its `omx_clap_entry_<kernel>` stays) and lib/omx_clap_static.c, the table of those entries by CLAP
# id that tools/gen.mjs fills. Position-independent, so a shared object (the console's engine) can
# link it. omxplugins.pc requires omx-dsp, whose archive the faces call.
B          := build
LIB_B      := $(B)/lib
LIB        := $(LIB_B)/libomxplugins.a
LIB_PC     := $(LIB_B)/omxplugins.pc
CLAP_FACES := $(foreach p,$(PLUGINS),$(wildcard $(p)omx_*_clap.c))
LIB_OBJS   := $(LIB_B)/omx_clap_static.o $(LIB_B)/omx_clap_face.o $(patsubst %.c,$(LIB_B)/%.o,$(notdir $(CLAP_FACES)))
# What this tree builds against is what is installed: the packaging names each dependency with no version
# (tools/deps-check.sh). Building the archive stops first, naming what pkg-config does not find.
SPEC       := packaging/omx-plugins.spec
# $(call dep_check,<pkg-config name>): nothing, or a stop naming the package
dep_check = $(if $(shell $(PKG_CONFIG) --exists $(1) && echo y),,$(error $(1) is not installed. Install omx-dsp-devel and omx-contract-devel (or libomxdsp-dev and libomx-contract-dev) from the channel (BUILDING.md)))
PKG_CONFIG ?= pkg-config
CC         ?= cc
AR         ?= ar
CFLAGS     ?= -O2 -g
LIB_WARN   := -std=gnu11 -Wall -Wextra -Werror
LIB_FLAGS  := -fPIC -fvisibility=hidden -DOMX_CLAP_STATIC -Iinclude
# read only by the targets that build the archive, so `make version` and the rest need no omx-dsp
lib_cflags = $(call dep_check,omxdsp)$(call dep_check,omx-contract)$(shell $(PKG_CONFIG) --cflags omxdsp omx-contract omx-clap clap)

.PHONY: test-checks graph-enum oracle-rates comp-published-scan schema-urls face-header role-meanings plugin-labels catalogs-gate install-catalogs install-labels-domain print-install-contract-dir meter-binding meanings meter-binding summary-numbers param-tie own-units presets presets-figures test-role all lib lib-test test test-faces units panel-words panel-layout descriptor-view descriptor-features choice-units placement-strips lv2-units round-trip-loss-units lv2-deprecated lv2-maintainer no-modgui release-version resource-hints format-tables contract-union probe-checks face-ids comp-release-mode eq-probe-race band-groups strip-visible-when strip-pages-follow phaser-depth geq-widgets round-trip hints manual install install-test package-lists install-devel install-contract contract-dir lv2bench-ran ci-scope no-ci-bridge deb-cells clean version recipe-test completeness selftest params-doc check-params-doc docs-site strip-crosscheck strip-bands strip-bands-bench meters-abi panel-abi meter-level dsp-copies panel-copies meter-ring meter-held meter-rows meter-words clap-validate face-dirty-order core-init-answers lv2-refuse-rate param-contract-abi host-contract curve-proof test-split test-heavy debt-base selftest-shards decl-schema-panel-ref filter-response artifact-retention release-same-pass from-channel
all: lib
	@for p in $(PLUGINS); do $(MAKE) -C $$p all || exit 1; done
lib: $(LIB) $(LIB_PC)
# every fast check, each its own target: `make test` runs them with -k, so one red does not hide the others
test-checks: entry-symlink units panel-words schema-urls graph-enum oracle-rates face-header role-meanings plugin-labels meanings meter-binding panel-layout descriptor-view descriptor-features filter-response choice-units placement-strips lv2-units round-trip-loss-units lv2-deprecated lv2-maintainer no-modgui release-version resource-hints format-tables contract-union contract-dir lv2bench-ran ci-scope no-ci-bridge deb-cells probe-checks eq-probe-race band-groups phaser-depth geq-widgets decl-schema-panel-ref summary-numbers param-tie own-units face-ids comp-release-mode strip-visible-when strip-pages-follow face-dirty-order core-init-answers selftest-shards meter-words artifact-retention test-no-host-nproc release-same-pass from-channel debt-base test-split
# make test runs every part whatever an earlier part answered, and is red when any part was: the fast checks (-k),
# then every plugin's test, then the steps that read the built plugins. A red check never hides a plugin's test
# (tools/test/test-split-check.mjs holds the recipe to that).
test: lib
	@if [ -z "$(PLUGINS)" ]; then echo "no plugins yet"; fi
	@rc=0; red=; \
	$(MAKE) -k test-checks || { rc=1; red="$$red test-checks"; }; \
	for p in $(PLUGINS); do $(MAKE) -C $$p test || { rc=1; red="$$red $$p"; }; done; \
	$(MAKE) -s presets-figures || { rc=1; red="$$red presets-figures"; }; \
	$(MAKE) -s lib-test || { rc=1; red="$$red lib-test"; }; \
	$(MAKE) -s hints || { rc=1; red="$$red hints"; }; \
	$(MAKE) -s manual || { rc=1; red="$$red manual"; }; \
	$(MAKE) -s presets || { rc=1; red="$$red presets"; }; \
	$(MAKE) -s meters-abi || { rc=1; red="$$red meters-abi"; }; \
	$(MAKE) -s lv2-refuse-rate || { rc=1; red="$$red lv2-refuse-rate"; }; \
	$(MAKE) -s param-contract-abi || { rc=1; red="$$red param-contract-abi"; }; \
	$(MAKE) -s panel-abi || { rc=1; red="$$red panel-abi"; }; \
	$(MAKE) -s host-contract || { rc=1; red="$$red host-contract"; }; \
	$(MAKE) -s meter-level || { rc=1; red="$$red meter-level"; }; \
	$(MAKE) -s comp-published-scan || { rc=1; red="$$red comp-published-scan"; }; \
	$(MAKE) -s meter-held || { rc=1; red="$$red meter-held"; }; \
	$(MAKE) -s meter-rows || { rc=1; red="$$red meter-rows"; }; \
	$(MAKE) -s strip-bands || { rc=1; red="$$red strip-bands"; }; \
	$(MAKE) -s test-role || { rc=1; red="$$red test-role"; }; \
	$(MAKE) -s package-lists || { rc=1; red="$$red package-lists"; }; \
	if [ $$rc -ne 0 ]; then echo "make test: red:$$red" >&2; fi; exit $$rc
# the set that runs the built plugins through a host: each plugin's kernel-identity and hand-written oracles on both
# faces, host-check.sh's external scanners, then the round trip through plugin-hostd. It runs on
# every pull request in its own CI jobs, not in the package build. OMX_HEAVY_SHARD=i/n runs the i-th of n slices of
# the plugins (the round trip in slice 1); unset, it runs all of them (tools/test/test-split.json lists both sets)
OMX_HEAVY_SHARD ?=
HEAVY_I = $(word 1,$(subst /, ,$(OMX_HEAVY_SHARD)))
HEAVY_N = $(word 2,$(subst /, ,$(OMX_HEAVY_SHARD)))
HEAVY_PLUGINS = $(if $(OMX_HEAVY_SHARD),$(shell printf '%s\n' $(PLUGINS) | awk -v i=$(HEAVY_I) -v n=$(HEAVY_N) '(NR - 1) % n == i - 1'),$(PLUGINS))
HEAVY_ROUND_TRIP = $(if $(OMX_HEAVY_SHARD),$(filter 1,$(HEAVY_I)),all)
test-heavy: lib
	@case '$(OMX_HEAVY_SHARD)' in ''|[1-9]/[1-9]|[1-9]/[1-9][0-9]|[1-9][0-9]/[1-9][0-9]) ;; \
	  *) echo "test-heavy: OMX_HEAVY_SHARD='$(OMX_HEAVY_SHARD)' is not i/n" >&2; exit 2;; esac
	@echo "test-heavy: $(words $(HEAVY_PLUGINS)) plugins$(if $(HEAVY_ROUND_TRIP), plus the round trip and the curve proof)$(if $(OMX_HEAVY_SHARD), (slice $(OMX_HEAVY_SHARD)))"
	@for p in $(HEAVY_PLUGINS); do $(MAKE) -C $$p test-heavy || exit 1; done
	@if [ -n '$(HEAVY_ROUND_TRIP)' ]; then $(MAKE) -s round-trip; fi
	@if [ -n '$(HEAVY_ROUND_TRIP)' ]; then $(MAKE) -s curve-proof; fi
# what runs with no omx-contract command (debian/rules: no .deb ships the command yet): the built faces, the static
# table and the ABI checks; nothing here reads the contract directory (the round trip generates, so it runs in make test)
test-faces: all lib contract-union eq-probe-race
	@$(MAKE) -s lib-test
	@$(MAKE) -s meters-abi
	@$(MAKE) -s lv2-refuse-rate
	@$(MAKE) -s param-contract-abi
	@$(MAKE) -s panel-abi
	@$(MAKE) -s host-contract
	@$(MAKE) -s meter-level
	@$(MAKE) -s comp-published-scan
	@$(MAKE) -s meter-held
	@$(MAKE) -s test-role
	@$(MAKE) -s package-lists
units:
	node tools/test/param-units-check.mjs --self-test
panel-words:
	node tools/test/panel-words-check.mjs --self-test
# schema/*.json names the project's own domain, never a code host (tools/test/schema-urls-check.mjs, with its sabotage arms)
schema-urls:
	node tools/test/schema-urls-check.mjs --self-test
# include/omx_plugin_face_ids.h is contract/plugin-face.json's render (tools/test/face-header-check.mjs, with its sabotage arms)
face-header:
	node tools/test/face-header-check.mjs
	node tools/test/face-header-check.mjs --self-test
# contract/role-meanings.json holds every role of every family once, and none other (tools/test/role-meanings-check.mjs),
# and it is the file the tools read, never an installed omx-plugins-contract (tools/test/role-meanings-source-check.mjs)
# every control of every shipped descriptor states its meaning, a word of the installed schema, never unknown, and the
# renderer refuses one that states none (tools/test/meanings-check.mjs; omx-contract docs/descriptors.md "Meanings")
meanings:
	node tools/test/meanings-check.mjs --self-test
# every section title, choice label and preset name is a key with its English sentence, no parameter name is one, and
# omx-contract validate reads the domain with its catalogs (tools/test/plugin-labels-check.mjs, plugin-labels-validate.sh)
plugin-labels:
	node tools/test/plugin-labels-check.mjs
	node tools/test/plugin-labels-check.mjs --self-test
	tools/test/plugin-labels-validate.sh
	tools/test/plugin-labels-validate.sh --self-test
# the catalogs whole and none stale: omx-contract's release gate (not part of `make test`: the entries are made at
# release preparation, or by CI's language packs step)
catalogs-gate:
	tools/catalogs-gate.sh
role-meanings:
	node tools/test/role-meanings-check.mjs
	node tools/test/role-meanings-check.mjs --self-test
	node tools/test/role-meanings-source-check.mjs --self-test
	node tools/test/role-meanings-source-check.mjs
# every meter a shipped plugin exports is bound by a display of its panel, or listed unbound with a reason (tools/test/meter-binding-check.mjs)
meter-binding:
	node tools/test/meter-binding-check.mjs
	node tools/test/meter-binding-check.mjs --self-test
panel-layout:
	node tools/test/panel-layout-test.mjs
descriptor-view:
	node tools/test/descriptor-view-test.mjs
# the unit of a choice is the descriptor's, never an option beside it (tools/test/choice-units-check.mjs)
choice-units:
	node tools/test/choice-units-check.mjs --self-test
# an EQ states the response of its fixed filters, and none on a band a type choice shapes (tools/test/filter-response-check.mjs)
filter-response:
	node tools/test/filter-response-check.mjs --self-test
descriptor-features:
	node tools/test/descriptor-features-check.mjs --self-test
placement-strips:
	node tools/test/placement-strips-check.mjs --self-test
# a tool run through a symlink answers as by its own path; no entry point compares argv[1] by hand (self-tested)
entry-symlink:
	node tools/test/entry-symlink-check.mjs --self-test
# every declared unit is stated by its port in the generated TTL, with its sabotage arms
lv2-units:
	node tools/test/lv2-units-check.mjs --self-test
# every unit tools/round-trip.mjs's ledger keys a loss by is a PARAM_UNITS id a descriptor states, with its sabotage arms
round-trip-loss-units:
	node tools/test/round-trip-loss-units-check.mjs --self-test
# no generated TTL states a term LV2 has deprecated (lv2:reportsLatency), with its sabotage arm
lv2-deprecated:
	node tools/test/lv2-deprecated-check.mjs --self-test
# every generated maintainer states foaf:homepage (mod-ui segfaults without it, #244), with its sabotage arm
lv2-maintainer:
	node tools/test/lv2-maintainer-check.mjs --self-test
# omx-plugins ships no generated MOD GUI (owner decision 2026-11-19), with its sabotage arms
no-modgui:
	node tools/test/no-modgui-check.mjs --self-test
# every plugin states its resource hints, and its TTL (omxres:) and CLAP header say what it states, with its sabotage arms
resource-hints:
	node tools/test/resource-hints-check.mjs --self-test
# the generators read plugin-hostd's format tables backwards, held by value, with its sabotage arms
format-tables:
	node tools/test/format-tables-test.mjs
# the union render is read whole while other processes render it (make -j): a fake CLI, readers a little apart
contract-union:
	node tools/test/contract-union-race-test.mjs
# the four probe checkers (params, panel, CLAP extensions, port groups) each go red on a face that is wrong, one change at a time
probe-checks:
	node tools/test/probe-checks-selftest.mjs
# the host's bypass (CLAP) and enabled and latency (LV2) at contract/plugin-face.json's permanent ids in every plugin, none moved by an
# appended parameter or an added sidechain; the self-test holds the rule it replaced red
# schema/plugin.decl.schema.json's graph kinds are omx-dsp's CURVE_KINDS rendered (tools/graph-enum.mjs; tools/test/graph-enum-check.mjs)
graph-enum:
	node tools/test/graph-enum-check.mjs
# the oracle rates are omx-dsp's ORACLE_FULL_RATES, read from nowhere else (tools/test/oracle-rates-check.mjs)
oracle-rates:
	node tools/test/oracle-rates-check.mjs
face-ids:
	node tools/test/face-ids-test.mjs
	node tools/test/face-ids-test.mjs --self-test

comp-release-mode:
	node tools/test/comp-release-mode-test.mjs
# the EQ default probe is run whole while other processes build it (make -j): a fake compiler, readers a little apart
eq-probe-race:
	node tools/test/eq-probe-race-test.mjs
param-tie:
	node tools/test/param-tie-test.mjs
# a plugin's own units, over a fixture plugin: they flow to every face, and the refusals (one name, one home)
own-units:
	node tools/test/own-units-test.mjs
# the CLAP face reads its dirty flag acquire and publishes it release, so the audio thread never sees the flag
# before the values (weak-memory targets), with its sabotage arms
face-dirty-order:
	node tools/test/face-dirty-order-check.mjs --self-test
# every core's init answers 1 only when every kernel instance it holds took the rate: a chain's elements accept almost
# the same rates, so no rate shows a dropped element answer and the source is read instead, with its sabotage arms
core-init-answers:
	node tools/test/core-init-answers-check.mjs --self-test
# the shared LV2 face refuses a rate outside the 32-bit rate every core holds, and a rate the core's own answer refuses
lv2-refuse-rate:
	@mkdir -p build
	$(CC) -std=gnu11 -Wall -Wextra -Werror -Iinclude $(shell $(PKG_CONFIG) --cflags lv2 omx-clap) -o build/lv2-refuse-rate-test tools/test/lv2-refuse-rate-test.c lib/omx_lv2_face.c -lm
	build/lv2-refuse-rate-test
# make test and make test-heavy together run every check tools/test/test-split.json lists, each in its own target,
# and CI runs the heavy set on every pull request, every shard (tools/test/test-split-check.mjs, its sabotage arms)
debt-base:
	node tools/test/debt-base-test.mjs
test-split:
	node tools/test/test-split-check.mjs --self-test
# make selftest runs in CI as shards packed by measured time (tools/test/selftest-shards.json), each part in exactly
# one, each shard under its budget, the fewest that fit, the CI matrix the table's (tools/test/selftest-shards-check.mjs,
# its sabotage arms)
selftest-shards:
	node tools/test/selftest-shards-check.mjs --self-test
summary-numbers:
	node tools/test/summary-numbers-test.mjs --self-test
phaser-depth:
	node tools/test/phaser-depth-role-test.mjs
	node tools/test/phaser-depth-role-test.mjs --self-test

# omx-eq8's generated descriptor and TTL hold a port group per band, HPF and LPF as before (#257); and every plugin
# with a band of several controls holds a port group per band in its generated TTL
band-groups:
	node tools/test/eq8-band-groups-test.mjs
	node tools/test/band-port-groups-check.mjs --self-test
strip-visible-when:
	node tools/test/strip-visible-when-test.mjs
# the strip's CLAP remote-control pages follow Bands: the built face and a host that offers clap.remote-controls,
# read back through the extension at 4, 32, 9, 9 and 4 bands (tools/test/clap-pages-follow-probe.c)
strip-pages-follow:
	@mkdir -p build
	@$(MAKE) -s -C plugins/omx-strip all
	$(CC) -std=gnu11 -Wall -Wextra -Werror $(shell $(PKG_CONFIG) --cflags omx-clap clap) -o build/clap-pages-follow-probe tools/test/clap-pages-follow-probe.c -ldl
	@id=$$(node -e "import('./tools/gen.mjs').then(m=>{const d=m.loadDecl('plugins/omx-strip');console.log(d.params.findIndex(p=>p.symbol==='eqBands'))})"); \
	  build/clap-pages-follow-probe plugins/omx-strip/build/omx-strip.clap $$id 4 32 9 9 4 > build/strip-pages-follow.txt && \
	  node tools/test/strip-pages-follow-test.mjs build/strip-pages-follow.txt
geq-widgets:
	node tools/test/geq-widgets-test.mjs
# the declaration schema reaches omx-panel's definitions by the document's name, whatever address its $id prints
# (tools/test/decl-schema-panel-ref-test.mjs; part of `make test`)
decl-schema-panel-ref:
	node tools/test/decl-schema-panel-ref-test.mjs
# the parameter reference of every plugin, written from the declarations (docs/plugins/)
params-doc:
	node tools/params-doc.mjs
check-params-doc:
	node tools/params-doc.mjs --check
	node tools/test/params-doc-test.mjs
# The published reference: Open-Mixer/.github's docs-publish.yml runs this with DOCS_OUT, an empty absolute directory,
# then renders the Markdown and, at a release tag, publishes the tree under
# https://www.openmixer.org/reference/omx-plugins/<version>/. check-params-doc runs first: every page of docs/plugins/ is
# what tools/params-doc.mjs writes from the plugin's descriptor and declaration, and a page that is not is red. Then only
# what the tree generates or keeps: those pages, the other pages of docs/ with their images, README, BUILDING, LICENSE.
docs-site: check-params-doc
	@case "$(DOCS_OUT)" in /*) ;; *) echo "docs-site: FAIL - DOCS_OUT '$(DOCS_OUT)' is not an absolute directory: name the directory the reference is written to" >&2; exit 1;; esac
	mkdir -p $(DOCS_OUT)/docs
	cp README.md BUILDING.md LICENSE $(DOCS_OUT)/
	cp -a docs/. $(DOCS_OUT)/docs/
# author, generate both faces, scan them back through plugin-hostd and compare, with its two sabotage arms; the probe
# reads this tree's role meanings (tools/round-trip-contract-dir.sh), never an installed omx-plugins-contract's
round-trip:
	tools/test/round-trip-contract-dir-check.sh
	tools/round-trip.sh
	tools/round-trip.sh --self-test
hints:
	node tools/gen.mjs --check
	node tools/gen.mjs
	node tools/port-hints.mjs
manual:
	node tools/test/manual-perturb.mjs
# the factory presets: fresh against tools/gen-presets.mjs, every value inside its declared travel
presets:
	node tools/gen-presets.mjs --check
	node tools/test/presets-check.mjs --self-test
# the presets' measured figures: the values through the DSP (omx-dsp, the generated headers)
presets-figures:
	node tools/test/preset-figures.mjs --self-test
# A plugin is complete when it has every artifact the recipe lists and keeps every law; a new plugin
# starts with `node tools/omx-new-plugin.mjs --from-contract <kernel>` (docs/new-plugin-from-contract.md).
recipe-test: selftest completeness
completeness:
	node tools/plugin-recipe.mjs
# OMX_SELFTEST_SHARD=i/n runs the i-th of the n shards of the selftest's parts (tools/test/selftest-shards.json, packed
# by measured time; CI runs each in its own job), the release and pin gates' self-tests (seconds) in the first; unset, all
OMX_SELFTEST_SHARD ?=
selftest:
	@case '$(OMX_SELFTEST_SHARD)' in ''|[1-9]/[1-9]|[1-9]/[1-9][0-9]|[1-9][0-9]/[1-9][0-9]) ;; \
	  *) echo "selftest: OMX_SELFTEST_SHARD='$(OMX_SELFTEST_SHARD)' is not i/n" >&2; exit 2;; esac
	OMX_SELFTEST_SHARD='$(OMX_SELFTEST_SHARD)' node tools/test/recipe-selftest.mjs
	@case '$(OMX_SELFTEST_SHARD)' in ''|1/*) set -e; tools/qualify-release.sh --self-test;; esac
OMX_PANEL ?= omx-panel
panel-copies:
	$(OMX_PANEL) copies --self-test
	$(OMX_PANEL) copies .
meters-abi:
	@mkdir -p build
	$(CC) -std=gnu11 -Wall -Wextra -Werror $(shell $(PKG_CONFIG) --cflags omx-clap clap) -o build/meters-abi-test tools/test/meters-abi-test.c
	build/meters-abi-test
	@echo "ok   clap.meters/1 as omx-clap installs it: id, enums, sizeof and offsetof"
param-contract-abi:
	@mkdir -p build
	$(CC) -std=gnu11 -Wall -Wextra -Werror -Iinclude $(shell $(PKG_CONFIG) --cflags omx-contract omx-clap clap) -o build/param-contract-abi-test tools/test/param-contract-abi-test.c
	build/param-contract-abi-test
	@echo "ok   org.openmixer.param-contract/1 as omx-clap reads it: id, sizeof and offsetof"
panel-abi:
	@mkdir -p build
	$(CC) -std=gnu11 -Wall -Wextra -Werror -Iinclude $(shell $(PKG_CONFIG) --cflags omx-clap clap) -o build/panel-abi-test tools/test/panel-abi-test.c
	build/panel-abi-test
	@echo "ok   org.openmixer.panel-description/1 as omx-clap declares it: id, codes, sizeof, offsetof and the omx_panel_get read-back"
# the displays marked `basis: exact` against the measured plugins: the sabotage arms, then the guard and every case
curve-proof:
	@for p in $$(node tools/test/curve-proof.mjs --stems); do $(MAKE) -s -C plugins/$$p build/$$p.clap || exit 1; done
	@mkdir -p build
	$(CC) -std=gnu11 -O2 -Wall -Wextra -Werror $(shell $(PKG_CONFIG) --cflags omx-clap clap omxdsp omx-contract) -o build/curve-proof tools/test/curve-proof.c $(shell $(PKG_CONFIG) --libs omx-clap omxdsp) -lm
	node tools/test/curve-proof.mjs build/curve-proof --self-test
	node tools/test/curve-proof.mjs build/curve-proof
host-contract: all
	@mkdir -p build
	$(CC) -std=gnu11 -Wall -Wextra -Werror $(shell $(PKG_CONFIG) --cflags omx-clap clap) -o build/host-contract-accept tools/test/host-contract-accept.c $(shell $(PKG_CONFIG) --libs omx-clap)
	build/host-contract-accept $(foreach p,$(SHIP_PLUGINS),$(p)build/$(notdir $(patsubst %/,%,$(p))).clap)
# every level meter of the metered faces reads what tools/test/<face>.meter-level.txt holds within the bound
# omx-dsp's fast-maths bound gives (meter-level-digest --tolerance), and a level moved past that bound is red; every
# other meter (a gain reduction, float through libm) reads within a tolerance what tools/test/<face>.meter-others.txt holds
METER_LEVEL_FACES := omx-comp omx-deesser
# faces whose meters are all gain reductions: no level to digest, only the values within the tolerance
METER_GR_FACES := omx-keyed-gate omx-limiter
meter-level:
	@mkdir -p build
	@for f in $(METER_LEVEL_FACES) $(METER_GR_FACES); do $(MAKE) -s -C plugins/$$f all || exit 1; done
	$(CC) -std=gnu11 -Wall -Wextra -Werror $(shell $(PKG_CONFIG) --cflags omxdsp omx-clap clap) -o build/meter-level-digest tools/test/meter-level-digest.c -ldl -lm
	node tools/test/meter-values-check.mjs --self-test
	@tol=$$(build/meter-level-digest --tolerance) || exit 1; \
	for f in $(METER_LEVEL_FACES); do \
	  build/meter-level-digest plugins/$$f/build/$$f.clap --others build/$$f.meter-others.txt > build/$$f.meter-level.txt || exit 1; \
	  node tools/test/meter-values-check.mjs --tolerance-db $$tol build/$$f.meter-level.txt tools/test/$$f.meter-level.txt || exit 1; \
	  node tools/test/meter-values-check.mjs build/$$f.meter-others.txt tools/test/$$f.meter-others.txt || exit 1; \
	  build/meter-level-digest plugins/$$f/build/$$f.clap --input-gain-db $$(awk "BEGIN { print 4 * $$tol }") > build/$$f.meter-level-moved.txt || exit 1; \
	  if node tools/test/meter-values-check.mjs --tolerance-db $$tol build/$$f.meter-level-moved.txt tools/test/$$f.meter-level.txt >/dev/null 2>&1; then \
	    echo "FAIL meter-level $$f: a level moved by four times the bound ($$tol dB) passed" >&2; exit 1; fi; \
	  echo "ok   meter-level $$f: a level moved by four times the bound is red"; \
	done
	@for f in $(METER_GR_FACES); do \
	  build/meter-level-digest plugins/$$f/build/$$f.clap --others build/$$f.meter-others.txt >/dev/null || exit 1; \
	  node tools/test/meter-values-check.mjs build/$$f.meter-others.txt tools/test/$$f.meter-others.txt || exit 1; \
	done
# omx-comp as this tree builds it, read by omx-clap-scan (omx-clap-host's): the scan states its clap.meters/1 meters and its
# org.openmixer.param-contract/1 entries by value, as text, JSON and field dump (tools/test/published-comp-scan.py), and
# each expected value changed once is found. The scanner is a build dependency of the package, not built here
OMX_CLAP_SCAN ?= omx-clap-scan
comp-published-scan:
	@$(MAKE) -s -C plugins/omx-comp all
	python3 -I tools/test/published-comp-scan.py --self-test $(OMX_CLAP_SCAN) plugins/omx-comp/build/omx-comp.clap
	python3 -I tools/test/published-comp-scan.py $(OMX_CLAP_SCAN) plugins/omx-comp/build/omx-comp.clap
# no source, hand-written or generated, carries a copy of a function omx-dsp exports (omx-dsp-copy-check --enforce)
dsp-copies:
	tools/dsp-copies-check.sh
	tools/dsp-copies-check.sh --self-test
meter-ring:
	tools/meter-ring-check.sh
	tools/meter-ring-check.sh --self-test
# a built face holds its meters until the host reads them, through the whole path (omx-comp), and each sabotage goes red
meter-held:
	tools/meter-held-check.sh
	tools/meter-held-check.sh --self-test
# the meter words held and clip reach the face rows the generator emits
meter-rows:
	node tools/test/meter-rows-test.mjs
# every descriptor meter states omx-contract's meter words as the declaration spells them
meter-words:
	node tools/test/meter-words-check.mjs
strip-crosscheck:
	sh tools/test/strip-crosscheck.sh
strip-bands:
	sh tools/test/strip-bands-check.sh
strip-bands-bench:
	@$(MAKE) -s -C plugins/omx-strip build/omx-strip.clap
	@mkdir -p build
	$(CC) -O2 -std=gnu11 -Wall -Wextra -Werror $(shell $(PKG_CONFIG) --cflags clap) -o build/strip-bands-bench tools/test/strip-bands-bench.c -ldl -lm
	build/strip-bands-bench plugins/omx-strip/build/omx-strip.clap $(BENCH_RATES)
install:
	@for p in $(SHIP_PLUGINS); do $(MAKE) -C $$p install || exit 1; done
	$(MAKE) -s install-presets
	$(MAKE) -s install-catalogs
# the factory presets, where every plugin's preset-discovery factory declares them
install-presets:
	find presets -type f -name '*.json' | while read -r f; do install -D -m644 "$$f" "$(DESTDIR)$(DATADIR)/omx/$$f"; done
# the labels' catalogs, where a surface reads them (omx-contract's catalog.mjs catalogPathOf): each is its language pack's
LABELS_CATALOGS := catalogs
install-catalogs:
	install -d $(DESTDIR)$(DATADIR)/omx-plugins/catalogs
	install -m644 $(wildcard $(LABELS_CATALOGS)/*.json) $(DESTDIR)$(DATADIR)/omx-plugins/catalogs/
install-test:
	@for p in $(TEST_PLUGINS); do $(MAKE) -C $$p all || exit 1; \
	  n=$$(basename $$p); install -Dm755 $$p/build/$$n.clap $(DESTDIR)$(TESTCLAPDIR)/$$n.clap || exit 1; done
test-role:
	tools/test-role-check.sh
	tools/test-role-check.sh --self-test
package-lists:
	tools/package-lists-check.sh
	tools/package-lists-check.sh --self-test
# Each shipped plugin's descriptor, plugins/<stem>/<stem>.descriptor.json, installed byte for byte into omx-contract's
# contract directory as $(CONTRACTDIR)/<schema major>/omx-plugins/plugins/<stem>.json, and each definition file of
# contract/ (a definition file this tree owns: role-meanings.json, the meaning each panel role plays) as
# $(CONTRACTDIR)/<schema major>/omx-plugins/<file>.json, with a MANIFEST.json naming this
# version (omx-contract docs/contract-dir.md); the packages ship it in omx-plugins-contract. The directory is
# omx-contract-data's, under the same data directory
# (`pkg-config --variable=contractdir omx-contract`); the install script too, or OMX_CONTRACT_DIR's, an omx-contract
# checkout, when no package is installed.
CONTRACTDIR ?= $(DATADIR)/omx-contract/contract.d
CONTRACT_SCHEMA_MAJOR := 3
INSTALL_CONTRACT_DIR = $(firstword $(wildcard $(shell $(PKG_CONFIG) --variable=datadir omx-contract 2>/dev/null)/install-contract-dir.sh) $(wildcard $(OMX_CONTRACT_DIR)/tools/install-contract-dir.sh))
DESCRIPTORS = $(foreach p,$(SHIP_PLUGINS),$(wildcard $(p)$(notdir $(p:/=)).descriptor.json))
CONTRACT_FILES = $(wildcard contract/*.json)
install-contract:
	@test -n "$(INSTALL_CONTRACT_DIR)" || { echo "install-contract: no install-contract-dir.sh: install omx-contract-devel, or set OMX_CONTRACT_DIR to a checkout" >&2; exit 1; }
	@stage=$$(mktemp -d) && trap 'rm -rf "$$stage"' EXIT && mkdir -p "$$stage/plugins" && \
	  for f in $(DESCRIPTORS); do cp "$$f" "$$stage/plugins/$$(basename "$$f" .descriptor.json).json" || exit 1; done && \
	  for f in $(CONTRACT_FILES); do cp "$$f" "$$stage/" || exit 1; done && \
	  sh $(INSTALL_CONTRACT_DIR) omx-plugins $(VERSION) $(CONTRACT_SCHEMA_MAJOR) "$$stage" $(DESTDIR)$(CONTRACTDIR)
	$(MAKE) -s install-labels-domain
# the labels domain: its document (contract.d/omx-plugin-labels, tools/gen.mjs) and the tree's catalogs/ beside it, staged
# as omx-contract's domain layout (<domain>/<domain>.json, <domain>/catalogs/<lang>.json) and installed by its script
install-labels-domain:
	@test -n "$(INSTALL_CONTRACT_DIR)" || { echo "install-labels-domain: no install-contract-dir.sh: install omx-contract-devel, or set OMX_CONTRACT_DIR to a checkout" >&2; exit 1; }
	@stage=$$(mktemp -d) && trap 'rm -rf "$$stage"' EXIT && mkdir -p "$$stage/contract.d/omx-plugin-labels/catalogs" && \
	  cp contract.d/omx-plugin-labels/omx-plugin-labels.json "$$stage/contract.d/omx-plugin-labels/" && \
	  cp $(wildcard $(LABELS_CATALOGS)/*.json) "$$stage/contract.d/omx-plugin-labels/catalogs/" && \
	  sh $(INSTALL_CONTRACT_DIR) --domain "$$stage/contract.d/omx-plugin-labels" $(DESTDIR)$(CONTRACTDIR) omx-plugins
# the install script the targets above run, for the checks that stage a contract directory the same way
print-install-contract-dir:
	@echo $(INSTALL_CONTRACT_DIR)
# lv2bench's two output shapes (Debian, Fedora lilv 0.26.4) are both read by the Zynthian bundle probe, with its refusals
lv2bench-ran:
	tools/test/lv2bench-ran-check.sh
# the selftest runs for a change that can move it (tools/ci-scope.sh)
ci-scope:
	tools/test/ci-scope-check.sh
no-ci-bridge:
	tools/test/no-ci-bridge-check.sh
release-same-pass:
	tools/test/release-same-pass-check.sh
# a DEB job runs on its check-matrix cell's runner, never a hand-written runner or arch list
deb-cells:
	tools/test/workflow-deb-cells-check.sh --self-test
	tools/test/workflow-deb-cells-check.sh .
artifact-retention:
	tools/test/artifact-retention-check.sh --self-test
	tools/test/artifact-retention-check.sh .
# an ordinary CI run takes every omx package from the channel, never staging: the check ci.yml runs after builddep, sabotaged
from-channel:
	tools/from-channel-check.sh --self-test
contract-dir:
	tools/test/contract-dir-check.sh
	tools/test/contract-dir-check.sh --self-test
	tools/test/tree-contract-check.sh
install-devel: lib install-contract
	install -d $(DESTDIR)$(INCLUDEDIR)/omx-plugins $(DESTDIR)$(LIBDIR)/pkgconfig
	install -m644 $(HEADERS) $(DESTDIR)$(INCLUDEDIR)/omx-plugins/
	install -m644 $(LIB) $(DESTDIR)$(LIBDIR)/
	sed -e 's,@PREFIX@,$(PREFIX),' -e 's,@LIBDIR@,$(LIBDIR),' -e 's,@INCLUDEDIR@,$(INCLUDEDIR),' $(LIB_PC) \
	  > $(DESTDIR)$(LIBDIR)/pkgconfig/omxplugins.pc
	chmod 644 $(DESTDIR)$(LIBDIR)/pkgconfig/omxplugins.pc
clean:
	@for p in $(PLUGINS); do $(MAKE) -C $$p clean; done
	rm -rf $(LIB_B)

$(LIB): $(LIB_OBJS)
	rm -f $@
	$(AR) rcs $@ $^

$(LIB_B)/omx_clap_static.o: lib/omx_clap_static.c include/omx_clap_static.h | $(LIB_B)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LIB_WARN) $(LIB_FLAGS) $(lib_cflags) -c -o $@ $<

# The CLAP face every plugin's descriptor file names, once in the archive.
$(LIB_B)/omx_clap_face.o: lib/omx_clap_face.c include/omx_clap_face.h include/omx_plugin_face.h include/omx_meter_ring.h include/omx_preset_file.h | $(LIB_B)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LIB_WARN) $(LIB_FLAGS) $(lib_cflags) -c -o $@ $<

# One rule per face: its generated headers sit in its own folder.
define face_rule
$(LIB_B)/$(notdir $(1:.c=.o)): $(1) $(wildcard $(dir $(1))*.h $(dir $(1))generated/*.h) include/omx_panel_description.h include/omx_param_contract.h include/omx_preset_file.h include/omx_plugin_face.h include/omx_clap_face.h | $(LIB_B)
	$$(CC) $$(CPPFLAGS) $$(CFLAGS) $$(LIB_WARN) $$(LIB_FLAGS) -I$(dir $(1))generated $$(lib_cflags) -c -o $$@ $$<
endef
$(foreach f,$(CLAP_FACES),$(eval $(call face_rule,$(f))))

# The prefix, libdir and includedir are filled at install; the version here.
$(LIB_PC): lib/omxplugins.pc.in Makefile $(SPEC) | $(LIB_B)
	sed -e 's,@VERSION@,$(VERSION),' $< > $@

$(LIB_B):
	@mkdir -p $@

# The consumer's proof: install-devel into a stage, then a C host built only from the staged
# omxplugins.pc (no -ldl, and dlopen must not be among its symbols) opens every face through the
# table. The ids it expects are each plugin folder's CLAP_ID, in folder order: the plugins' own; the
# test plugins' after `--test`, in their own table.
LIB_STAGE := $(abspath $(LIB_B)/stage)
CLAP_IDS   = $(foreach p,$(SHIP_PLUGINS),$(shell sed -n 's/^CLAP_ID *:= *//p' $(p)Makefile))
TEST_IDS   = $(foreach p,$(TEST_PLUGINS),$(shell sed -n 's/^CLAP_ID *:= *//p' $(p)Makefile))
lib-test: lib
	rm -rf $(LIB_STAGE)
	$(MAKE) -s install-devel DESTDIR= PREFIX=$(LIB_STAGE) LIBDIR=$(LIB_STAGE)/lib
	PKG_CONFIG_PATH=$(LIB_STAGE)/lib/pkgconfig$${PKG_CONFIG_PATH:+:$$PKG_CONFIG_PATH} && export PKG_CONFIG_PATH && \
	  $(CC) $(CPPFLAGS) $(CFLAGS) $(LIB_WARN) $$($(PKG_CONFIG) --cflags omxplugins) -o $(LIB_B)/static-table-host \
	    tools/test/static-table-host.c $(LDFLAGS) $$($(PKG_CONFIG) --libs omxplugins)
	@if nm -u $(LIB_B)/static-table-host | grep -qw dlopen; then echo "FAIL static-table-host calls dlopen"; exit 1; fi
	@echo "PASS static-table-host links libomxplugins through omxplugins.pc, without dlopen"
	$(LIB_B)/static-table-host $(CLAP_IDS) --test $(TEST_IDS)
version:
	@echo $(VERSION)
# every generated header, descriptor and LV2 plugin file states the release's version, the VERSION above
release-version:
	node tools/test/release-version-check.mjs --self-test

clap-validate: all
	tools/clap-validate.sh build/clap-validator

# no workflow sizes its parallel jobs from the host's nproc (tools/no-host-nproc.sh: its self-test, then the scan)
.PHONY: test-no-host-nproc
test-no-host-nproc:
	sh tools/no-host-nproc.sh --self-test
	sh tools/no-host-nproc.sh
