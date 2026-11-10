# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# Top-level build: every plugin under plugins/<name>/ has its own Makefile with `all`, `test`,
# `install` and `clean`.
#
#   make            build every plugin
#   make test       every plugin's tests (what the package builds run), then `make hints` and `make manual`
#   make units      the declared units' contract codes (tools/param-contract.mjs) are omx-contract's header's
#                   (OMX_PARAM_CONTRACT_UNITS, omx_param_units.h), its vendor block is dbfs, dbu, ratio, lufs, a unit
#                   changed on either side red (tools/test/param-units-check.mjs)
#   make lv2-units  every unit a declaration uses has an LV2 unit, and every port states the unit it declares
#                   (tools/test/lv2-units-check.mjs, with its sabotage arms; part of `make test`)
#   make lv2-maintainer  every generated TTL gives its doap:maintainer a foaf:homepage (tools/test/lv2-maintainer-check.mjs; part of `make test`)
#   make lv2-manifest  every manifest lists <modgui.ttl> where omx-panel's reader finds it (tools/test/lv2-manifest-check.mjs; part of `make test`)
#   make format-tables  the generators read plugin-hostd's installed format tables backwards (tools/format-tables.mjs):
#                   the units, the 169 lv2:designation lines and the ratio unit, the CLAP modules from the descriptors'
#                   groups (tools/test/format-tables-test.mjs, with its sabotage arms; part of `make test`)
#   make round-trip  every described plugin generated, both faces built and scanned back by plugin-hostd-probe describe,
#                   compared with its authored descriptor word by word: equal, lost <format> <word> or differs
#                   (tools/round-trip.{sh,mjs}, with its two sabotage arms; part of `make test`)
#   make contract-union  parallel make targets render omx-contract's union into private directories and read it whole
#                   (tools/test/contract-union-race-test.mjs; part of `make test` and `make test-faces`)
#   make face-ids   the host's bypass and LV2 enabled and latency sit at omx-contract's permanent ids (OMX_PLUGIN_FACE_IDS), none
#                   moved by an appended parameter (tools/test/face-ids-test.mjs, with its self-test; part of `make test`)
#   make probe-checks  clap-params-check, panel-check, clap-ext-check and port-groups-check each fail on a built face that is wrong,
#                   one change at a time (tools/test/probe-checks-selftest.mjs; part of `make test`)
#   make eq-probe-race  parallel make targets each build build/eq-default-probe privately and run it whole
#                   (tools/test/eq-probe-race-test.mjs; part of `make test` and `make test-faces`)
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
#   make geq-widgets  omx-geq declares the fader for its 31 bands, and its panel description and generated header serve them
#                   (tools/test/geq-widgets-test.mjs; part of `make test`)
#   make hints      no LV2 face lost a port hint: tools/port-hints.mjs against each declaration's `portHints` pin
#   make modgui     every plugin's MOD GUI is what omx-panel writes from its descriptor and its declared panel: each plugin's
#                   `make check` (the generated-fresh step of CI runs it beside `node tools/gen.mjs --check`)
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
#   make meters-abi  the clap.meters/1 header omx-clap-core installs has the id, enums, sizeof and offsetof the
#                    plugins are built for (tools/test/meters-abi-test.c, compile and run; part of `make test`)
#   make param-contract-abi  include/omx_param_contract.h's extension table and entry are the layout omx-clap-core's
#                    omx_clap_ext.h reads (tools/test/param-contract-abi-test.c, compile and run; part of `make test`)
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
#                   factory presets in $(DATADIR)/omx/presets/<plugin id>/<preset id>.json, then install-pins
#   make install-pins  PINS_DIR, PINS_FILE: plugin-qualify --pins over the just-installed .clap files, written to
#                   $(DESTDIR)$(PINS_DIR)/$(PINS_FILE) with the buildroot rewritten to the final $(CLAPDIR)
#                   (tools/gen-pins.sh, with its --self-test; needs plugin-qualify >= 0.9.0); tools/pins-check.sh
#                   checks the shipped pins against the shipped binaries' hashes (with its --self-test, part of
#                   `make selftest`), run by a package's own build (%check / dh_auto_install), not by this Makefile
#   make install-contract  DESTDIR, CONTRACTDIR: each shipped plugin's descriptor into omx-contract's contract directory,
#                   $(CONTRACTDIR)/3/omx-plugins/plugins/<stem>.json and its MANIFEST.json (run by install-devel)
#   make contract-dir  the installed tree carries one descriptor per shipped plugin that has one, byte for byte, and
#                   omx-contract's reader reads it (tools/test/contract-dir-check.sh, with its sabotage arms; part of `make test`)
#   make lib        build/lib/libomxplugins.a: every shipped plugin's CLAP face and the table of their
#                   entries (include/omx_clap_static.h), for a host that runs them in-process
#   make lib-test   a C host linked against the archive opens every face through the table, then
#                   again through the installed omxplugins.pc (part of `make test`)
#   make install-devel   the public headers other projects include: $(INCLUDEDIR)/omx-plugins/*.h,
#                   and $(LIBDIR)/libomxplugins.a with $(LIBDIR)/pkgconfig/omxplugins.pc
#   make clap-validate  clap-validator on every built .clap (tools/clap-validate.sh): one JSON report each in build/clap-validator;
#                   the factory presets must be installed (make install-presets); in CI a failed test fails the job
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
VERSION := 0.19.1
PLUGINS := $(sort $(dir $(wildcard plugins/*/Makefile)))
# CHANGED_PLUGINS (CI's `test` job, from tools/plugin-scope.sh over the pull request's diff): the short names
# (omx-eq8, never a path) of the plugins a change touches, space-separated, or ALL. Empty (every local `make
# test`, a push to main or a tag) and ALL both mean every plugin, exactly as before; a narrower list scopes the
# per-plugin test loop and the round-trip rescan below to it, so a one-plugin pull request no longer rebuilds,
# rescans and sabotages the other thirty (#2).
CHANGED_PLUGINS ?=
ifeq ($(strip $(CHANGED_PLUGINS))$(filter ALL,$(CHANGED_PLUGINS)),)
SCOPE_PLUGINS := $(PLUGINS)
else ifeq ($(filter ALL,$(CHANGED_PLUGINS)),ALL)
SCOPE_PLUGINS := $(PLUGINS)
else
SCOPE_PLUGINS := $(filter $(addsuffix /,$(addprefix plugins/,$(CHANGED_PLUGINS))),$(PLUGINS))
endif
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
# plugin-hostd's read-only layer of package pins (plugin-hostd issue #42): our own plugins' pins are
# known at build time, so `make install` writes them here instead of leaving them to be recorded on
# first use (tools/gen-pins.sh, which needs plugin-qualify >= 0.9.0).
PINS_DIR   ?= $(DATADIR)/plugin-hostd/pins.d
PINS_FILE  := omx-plugins.pins
# Every header a consumer includes: the shared parameter type, the panel description and parameter contract rows, then each shipped plugin's generated
# parameter header and its instance header (the DSP shell around the omx-dsp kernel). A plugin's
# headers are the .h files beside its Makefile and in its generated/ directory.
# A test plugin's own hand-written header (an extension a host's test reads, omx_test_transport_probe.h)
# is installed with them, and so is its generated generated/omx_<kernel>_decl.h: the constants a
# proof predicts from (and the CLAP id) have one home, the declaration, not a hardcoded second copy
# in the host that reads it. Neither declares anything a picker lists: macros only.
HEADERS := include/omx_plugin_param.h include/omx_panel_description.h include/omx_param_contract.h include/omx_preset_file.h include/omx_clap_static.h include/omx_plugin_face.h $(foreach p,$(SHIP_PLUGINS),$(wildcard $(p)*.h) $(wildcard $(p)generated/*.h)) $(foreach p,$(TEST_PLUGINS),$(wildcard $(p)*.h) $(wildcard $(p)generated/*.h))

# libomxplugins.a: each shipped plugin's CLAP face built with -DOMX_CLAP_STATIC (no `clap_entry`;
# its `omx_clap_entry_<kernel>` stays) and lib/omx_clap_static.c, the table of those entries by CLAP
# id that tools/gen.mjs fills. Position-independent, so a shared object (the console's engine) can
# link it. omxplugins.pc requires the omx-dsp floor the packaging declares, whose archive the faces call.
B          := build
LIB_B      := $(B)/lib
LIB        := $(LIB_B)/libomxplugins.a
LIB_PC     := $(LIB_B)/omxplugins.pc
CLAP_FACES := $(foreach p,$(PLUGINS),$(wildcard $(p)omx_*_clap.c))
LIB_OBJS   := $(LIB_B)/omx_clap_static.o $(LIB_B)/omx_clap_face.o $(patsubst %.c,$(LIB_B)/%.o,$(notdir $(CLAP_FACES)))
# The releases this tree builds against are declared once, in the packaging: packaging/omx-plugins.spec's
# BuildRequires, a floor and a ceiling per dependency (`(pkg >= F with pkg < C)`); debian/control says the same
# (tools/deps-check.sh). Building the archive stops first, naming the dependency, the range and what is
# installed, when pkg-config finds a release outside it.
SPEC       := packaging/omx-plugins.spec
spec_floor   = $(shell sed -n 's/^BuildRequires: ($(1) >= \([0-9.]*\) with $(1) < [0-9.]*)$$/\1/p' $(SPEC))
spec_ceiling = $(shell sed -n 's/^BuildRequires: ($(1) >= [0-9.]* with $(1) < \([0-9.]*\))$$/\1/p' $(SPEC))
OMXDSP_MIN        := $(call spec_floor,omx-dsp-devel)
OMXDSP_MAX        := $(call spec_ceiling,omx-dsp-devel)
CONTRACT_MIN      := $(call spec_floor,omx-contract-devel)
CONTRACT_MAX      := $(call spec_ceiling,omx-contract-devel)
# $(call dep_check,<pkg-config name>,<floor>,<ceiling>): nothing, or a stop naming what is installed
dep_check = $(if $(shell $(PKG_CONFIG) --atleast-version=$(2) $(1) && ! $(PKG_CONFIG) --atleast-version=$(3) $(1) && echo y),,$(error $(1) >= $(2) and < $(3) is required ($(SPEC)); installed: $(or $(shell $(PKG_CONFIG) --modversion $(1) 2>/dev/null),none). Install omx-dsp-devel and omx-contract-devel (or libomxdsp-dev and libomx-contract-dev) from the channel: BUILDING.md))
PKG_CONFIG ?= pkg-config
CC         ?= cc
AR         ?= ar
CFLAGS     ?= -O2 -g
LIB_WARN   := -std=gnu11 -Wall -Wextra -Werror
LIB_FLAGS  := -fPIC -fvisibility=hidden -DOMX_CLAP_STATIC -Iinclude
# read only by the targets that build the archive, so `make version` and the rest need no omx-dsp
lib_cflags = $(call dep_check,omxdsp,$(OMXDSP_MIN),$(OMXDSP_MAX))$(call dep_check,omx-contract,$(CONTRACT_MIN),$(CONTRACT_MAX))$(shell $(PKG_CONFIG) --cflags omxdsp omx-contract omx-clap-core clap)

.PHONY: summary-numbers param-tie presets presets-figures test-role all lib lib-test test test-faces units panel-words panel-layout descriptor-view descriptor-features placement-strips lv2-units lv2-deprecated lv2-maintainer lv2-manifest release-version resource-hints format-tables contract-union probe-checks face-ids eq-probe-race band-groups strip-visible-when strip-pages-follow phaser-depth geq-widgets round-trip hints modgui manual install install-pins install-test package-lists install-devel install-contract contract-dir lv2bench-ran lv2bench-exit plugin-scope clean version recipe-test completeness selftest params-doc check-params-doc strip-crosscheck strip-bands strip-bands-bench meters-abi meter-level dsp-copies panel-copies meter-ring clap-validate face-dirty-order param-contract-abi
all: lib
	@for p in $(PLUGINS); do $(MAKE) -C $$p all || exit 1; done
lib: $(LIB) $(LIB_PC)
test: lib units panel-words panel-layout descriptor-view descriptor-features placement-strips lv2-units lv2-deprecated lv2-maintainer lv2-manifest release-version resource-hints format-tables contract-union contract-dir lv2bench-ran lv2bench-exit plugin-scope probe-checks eq-probe-race band-groups phaser-depth geq-widgets summary-numbers param-tie face-ids strip-visible-when face-dirty-order
	@if [ -z "$(PLUGINS)" ]; then echo "no plugins yet"; fi
	@if [ "$(words $(SCOPE_PLUGINS))" != "$(words $(PLUGINS))" ]; then \
	  echo "scope: CHANGED_PLUGINS=$(CHANGED_PLUGINS): testing only $(SCOPE_PLUGINS), not $(filter-out $(SCOPE_PLUGINS),$(PLUGINS))"; \
	fi
	@for p in $(SCOPE_PLUGINS); do $(MAKE) -C $$p test || exit 1; done
	@$(MAKE) -s round-trip
	@$(MAKE) -s presets-figures
	@$(MAKE) -s lib-test
	@$(MAKE) -s hints
	@$(MAKE) -s manual
	@$(MAKE) -s presets
	@$(MAKE) -s presets-figures
	@$(MAKE) -s meters-abi
	@$(MAKE) -s param-contract-abi
	@$(MAKE) -s meter-level
	@$(MAKE) -s strip-bands
	@$(MAKE) -s test-role
	@$(MAKE) -s package-lists
# what runs with no omx-contract command (debian/rules: no .deb ships the command yet): the built faces, the static
# table and the ABI checks; nothing here reads the contract directory (the round trip generates, so it runs in make test)
test-faces: all lib contract-union eq-probe-race
	@$(MAKE) -s lib-test
	@$(MAKE) -s meters-abi
	@$(MAKE) -s param-contract-abi
	@$(MAKE) -s meter-level
	@$(MAKE) -s test-role
	@$(MAKE) -s package-lists
units:
	node tools/test/param-units-check.mjs --self-test
panel-words:
	node tools/test/panel-words-check.mjs --self-test
panel-layout:
	node tools/test/panel-layout-test.mjs
descriptor-view:
	node tools/test/descriptor-view-test.mjs
descriptor-features:
	node tools/test/descriptor-features-check.mjs --self-test
placement-strips:
	node tools/test/placement-strips-check.mjs --self-test
# every declared unit is stated by its port in the generated TTL, with its sabotage arms
lv2-units:
	node tools/test/lv2-units-check.mjs --self-test
# no generated TTL states a term LV2 has deprecated (lv2:reportsLatency), with its sabotage arm
lv2-deprecated:
	node tools/test/lv2-deprecated-check.mjs --self-test
# every generated maintainer states foaf:homepage (mod-ui segfaults without it, #244), with its sabotage arm
lv2-maintainer:
	node tools/test/lv2-maintainer-check.mjs --self-test
# every manifest points at its modgui.ttl in the spelling omx-panel's --check reads (CI was red on omx-panel 0.5.0), with its sabotage arm
lv2-manifest:
	node tools/test/lv2-manifest-check.mjs --self-test
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
# the host's bypass (CLAP) and enabled and latency (LV2) at omx-contract's permanent ids in every plugin, none moved by an
# appended parameter or an added sidechain; the self-test holds the rule it replaced red
face-ids:
	node tools/test/face-ids-test.mjs
	node tools/test/face-ids-test.mjs --self-test
# the EQ default probe is run whole while other processes build it (make -j): a fake compiler, readers a little apart
eq-probe-race:
	node tools/test/eq-probe-race-test.mjs
# omx-eq8's generated descriptor and TTL hold a port group per band, HPF and LPF as before (#257)
param-tie:
	node tools/test/param-tie-test.mjs
# the CLAP face reads its dirty flag acquire and publishes it release, so the audio thread never sees the flag
# before the values (weak-memory targets), with its sabotage arms
face-dirty-order:
	node tools/test/face-dirty-order-check.mjs --self-test
summary-numbers:
	node tools/test/summary-numbers-test.mjs --self-test
phaser-depth:
	node tools/test/phaser-depth-role-test.mjs
	node tools/test/phaser-depth-role-test.mjs --self-test

band-groups:
	node tools/test/eq8-band-groups-test.mjs
strip-visible-when:
	node tools/test/strip-visible-when-test.mjs
# the strip's CLAP remote-control pages follow Bands: the built face and a host that offers clap.remote-controls,
# read back through the extension at 4, 32, 9, 9 and 4 bands (tools/test/clap-pages-follow-probe.c)
strip-pages-follow:
	@mkdir -p build
	@$(MAKE) -s -C plugins/omx-strip all
	$(CC) -std=gnu11 -Wall -Wextra -Werror $(shell $(PKG_CONFIG) --cflags omx-clap-core clap) -o build/clap-pages-follow-probe tools/test/clap-pages-follow-probe.c -ldl
	@id=$$(node -e "import('./tools/gen.mjs').then(m=>{const d=m.loadDecl('plugins/omx-strip');console.log(d.params.findIndex(p=>p.symbol==='eqBands'))})"); \
	  build/clap-pages-follow-probe plugins/omx-strip/build/omx-strip.clap $$id 4 32 9 9 4 > build/strip-pages-follow.txt && \
	  node tools/test/strip-pages-follow-test.mjs build/strip-pages-follow.txt
geq-widgets:
	node tools/test/geq-widgets-test.mjs
# the parameter reference of every plugin, written from the declarations (docs/plugins/)
params-doc:
	node tools/params-doc.mjs
check-params-doc:
	node tools/params-doc.mjs --check
	node tools/test/params-doc-test.mjs
# author, generate both faces, scan them back through plugin-hostd and compare, with its two sabotage arms
round-trip:
	tools/round-trip.sh
	tools/round-trip.sh --self-test
hints:
	node tools/gen.mjs --check
	node tools/gen.mjs
	node tools/port-hints.mjs
# the plugins that have a panel say so in their Makefile (PANEL_FILE); `make check` holds gen.mjs's files and the MOD GUI fresh
modgui:
	@for p in $(PLUGINS); do if grep -q '^PANEL_FILE' $$p/Makefile; then $(MAKE) -s -C $$p check || exit 1; fi; done
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
selftest:
	node tools/test/recipe-selftest.mjs
	tools/qualify-release.sh --self-test
	tools/gen-pins.sh --self-test
	tools/pins-check.sh --self-test
OMX_PANEL ?= omx-panel
panel-copies:
	$(OMX_PANEL) copies --self-test
	$(OMX_PANEL) copies .
meters-abi:
	@mkdir -p build
	$(CC) -std=gnu11 -Wall -Wextra -Werror $(shell $(PKG_CONFIG) --cflags omx-clap-core clap) -o build/meters-abi-test tools/test/meters-abi-test.c
	build/meters-abi-test
	@echo "ok   clap.meters/1 as omx-clap-core installs it: id, enums, sizeof and offsetof"
param-contract-abi:
	@mkdir -p build
	$(CC) -std=gnu11 -Wall -Wextra -Werror -Iinclude $(shell $(PKG_CONFIG) --cflags omx-contract omx-clap-core clap) -o build/param-contract-abi-test tools/test/param-contract-abi-test.c
	build/param-contract-abi-test
	@echo "ok   org.openmixer.param-contract/1 as omx-clap-core reads it: id, sizeof and offsetof"
# every level meter of the metered faces reads what tools/test/<face>.meter-level.txt holds within the bound
# omx-dsp's fast-maths bound gives (meter-level-digest --tolerance), and a level moved past that bound is red; every
# other meter (a gain reduction, float through libm) reads within a tolerance what tools/test/<face>.meter-others.txt holds
METER_LEVEL_FACES := omx-comp omx-deesser
# faces whose meters are all gain reductions: no level to digest, only the values within the tolerance
METER_GR_FACES := omx-keyed-gate omx-limiter
meter-level:
	@mkdir -p build
	@for f in $(METER_LEVEL_FACES) $(METER_GR_FACES); do $(MAKE) -s -C plugins/$$f all || exit 1; done
	$(CC) -std=gnu11 -Wall -Wextra -Werror $(shell $(PKG_CONFIG) --cflags omxdsp omx-clap-core clap) -o build/meter-level-digest tools/test/meter-level-digest.c -ldl -lm
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
# no source, hand-written or generated, carries a copy of a function omx-dsp exports (omx-dsp-copy-check --enforce)
dsp-copies:
	tools/dsp-copies-check.sh
	tools/dsp-copies-check.sh --self-test
meter-ring:
	tools/meter-ring-check.sh
	tools/meter-ring-check.sh --self-test
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
	$(MAKE) -s install-pins
# the factory presets, where every plugin's preset-discovery factory declares them
install-presets:
	find presets -type f -name '*.json' | while read -r f; do install -D -m644 "$$f" "$(DESTDIR)$(DATADIR)/omx/$$f"; done
# every shipped plugin's pin, known at build time: scans the .clap files the step above just installed,
# so the pins ship beside the binaries they were measured from (tools/gen-pins.sh, which needs
# plugin-qualify >= 0.9.0; run on its own once the plugins are already installed, e.g. in a package's
# %check against %{buildroot})
install-pins:
	tools/gen-pins.sh "$(DESTDIR)" "$(CLAPDIR)" "$(PINS_DIR)" "$(PINS_FILE)"
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
# contract/ (a definition file this tree owns; none today) as
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
# lv2bench's two output shapes (Debian, Fedora lilv 0.26.4) are both read by the Zynthian bundle probe, with its refusals
lv2bench-ran:
	tools/test/lv2bench-ran-check.sh
# lv2bench's own exit status mixes every plugin on LV2_PATH into one number, so the Zynthian bundle probe must not
# let it gate the per-URI check above, in either output shape
lv2bench-exit:
	tools/test/lv2bench-exit-check.sh
# CHANGED_PLUGINS (CI's `test` job) scopes the per-plugin test loop and the round-trip rescan to a pull
# request's own plugins; tools/plugin-scope.sh (what computes it from a diff) and its ALL fallback, checked
plugin-scope:
	tools/test/plugin-scope-check.sh
contract-dir:
	tools/test/contract-dir-check.sh
	tools/test/contract-dir-check.sh --self-test
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

# The prefix, libdir and includedir are filled at install; the version and the omx-dsp and omx-contract floors here.
$(LIB_PC): lib/omxplugins.pc.in Makefile $(SPEC) | $(LIB_B)
	sed -e 's,@VERSION@,$(VERSION),' -e 's,@OMXDSP_MIN@,$(OMXDSP_MIN),' -e 's,@OMXDSP_MAX@,$(OMXDSP_MAX),' -e 's,@CONTRACT_MIN@,$(CONTRACT_MIN),' -e 's,@CONTRACT_MAX@,$(CONTRACT_MAX),' $< > $@

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
