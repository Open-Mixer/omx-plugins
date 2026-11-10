<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com> -->
# Building omx-plugins

## Dependencies

| What | Fedora | Debian | Used for |
|---|---|---|---|
| omx-dsp ≥ 0.31.0 (`pkg-config omxdsp`) | `omx-dsp-devel` (channel) | `libomxdsp-dev` (channel) | the DSP: every kernel lives there, none here (since 0.26.0: the dynamic EQ, the multiband's controls and readouts, the compressor's key) |
| omx-contract ≥ 3.9.0 (`pkg-config omx-contract`) | `omx-contract-devel` (channel) | `libomx-contract-dev` (channel) | the declarations: every parameter's travel, read from its installed `omx-contract.json`, and the EQ default rule (its C render; the generators build a small probe, so they need the C compiler too) |
| openmixer CLAP extensions | `omx-clap-core-devel` (channel) | `libomx-clap-core-dev` (channel) | `<omx-clap-host/omx_clap_ext.h>` |
| the omx-contract command ≥ 3.9.0 | `omx-contract` (channel) | `omx-contract` (channel) | the generators: the union of the contract directory, read through `omx-contract render --installed` |
| CLAP 1.2 headers | `clap-devel` | `clap-headers` (built from the pinned CLAP, see FreeMixer/.github) | the CLAP face |
| LV2 headers | `lv2-devel` | `lv2-dev` | the LV2 face |
| C compiler, make, pkg-config | `gcc make pkgconf-pkg-config` | `build-essential pkgconf` | |
| lilv | `lilv lilv-devel` | `lilv-utils liblilv-dev` | tests: the oracles' LV2 host, `lv2ls`, `lv2info` |
| Node.js ≥ 18 | `nodejs` | `nodejs` | tests and `make gen`: the generators |
| omx-panel ≥ 0.11.0 (the `omx-panel` command, `/usr/share/omx-panel`; `$OMX_PANEL_DIR` names another tree, `$OMX_PANEL` another command) | `omx-panel` (channel) | `omx-panel` (channel) | the panels: the layout every face serves and the MOD GUI of each plugin, from its descriptor and its declared panel |
| sord | `sord` | `sordi` | tests: `sord_validate` on the LV2 bundle |
| omx-clap-host | `omx-clap-host` (channel) | `omx-clap-host` (channel) | tests: `omx-clap-scan` (the check says SKIP when it is absent; CI and the package builds install it) |

From omx-contract 2.15.0 on, the kernels' definitions ship with omx-dsp into omx-contract's contract
directory (`pkg-config --variable=contractdir omx-contract`), and the installed `omx-contract.json` holds
only the shared facts. The generators then read the union of that directory through the omx-contract CLI,
`omx-contract render --target json --installed`, rendered into `build/contract-union-<digest>/` (a name only that content has, so parallel make targets never share a half-written render): the CLI is
`$OMX_CONTRACT_CLI` (its `bin/omx-contract.mjs`) or `omx-contract` on `PATH`, which the omx-contract
RPM installs. A descriptor's links name the
package that owns each definition, from the directory's manifests.

A Debian build host gets the omx-contract command from the `omx-contract` package (a Build-Depends), so
`debian/rules` runs `make test` whole, as the RPM build does; `tools/deps-check.sh` holds both.

The channel is the OpenMixer package repository, added by installing the openmixer-release package
(`https://www.openmixer.org/rpm/openmixer-release.noarch.rpm` for dnf,
`https://www.openmixer.org/deb/openmixer-release.deb` for apt). With it installed, `dnf builddep packaging/omx-plugins.spec` or `apt-get build-dep ./` installs
everything above.

The ranges are written once, in the packaging: `packaging/omx-plugins.spec` (`BuildRequires`, a floor and a
ceiling: `(omx-dsp-devel >= 0.30.0 with omx-dsp-devel < 0.31)`) and `debian/control` (the same: `(>= 0.30.0~)` and
`(<< 0.31~)`); `tools/deps-check.sh` holds the two equal. Nothing else names a version: the Makefile reads the
floors and ceilings from the spec, and building the archive stops, naming the dependency, the range
and what `pkg-config` finds installed, when an installed release is outside it.

## Build, test, install

```sh
make                      # every plugin: plugins/<name>/build/
make test                 # every plugin's tests, then the static table's (make lib-test)
make test CHANGED_PLUGINS="omx-eq8 omx-delay"   # only those plugins' tests and the round-trip rescan;
                                                         # CI sets this from a pull request's own diff (tools/plugin-scope.sh);
                                                         # empty or ALL (the default) means every plugin, as above
make install DESTDIR=/tmp/stage LIBDIR=/usr/lib64   # <CLAPDIR>/*.clap (default /usr/lib/clap, any LIBDIR), <LIBDIR>/lv2/*.lv2/
make install-devel DESTDIR=/tmp/stage LIBDIR=/usr/lib64   # headers, <LIBDIR>/libomxplugins.a, <LIBDIR>/pkgconfig/omxplugins.pc, and each plugin's descriptor
                                                         # in <DATADIR>/omx-contract/contract.d/3/omx-plugins/ (make install-contract, CONTRACTDIR=)
make version              # the release version
```

`make` also builds `build/lib/libomxplugins.a`: every shipped plugin's CLAP face, compiled with
`-DOMX_CLAP_STATIC` (the face then defines only its `omx_clap_entry_<kernel>`, not CLAP's
`clap_entry`, so all of them link into one program), and `lib/omx_clap_static.c`, the table of those
entries by CLAP id that `include/omx_clap_static.h` declares. A host links it through
`pkg-config omxplugins` and opens any plugin in-process, with no `.clap` file and no `dlopen`.
`make lib-test` installs the devel files into `build/lib/stage`, builds
`tools/test/static-table-host.c` from the staged `omxplugins.pc` alone and runs it: every plugin's
CLAP id is in the table once, and each one is created through its factory, activated and run.

Before the plugins, `make test` runs `make units` (the param-contract units are the header's), `make panel-words`
(`tools/test/panel-words-check.mjs`: the enums of `include/omx_panel_description.h` are omx-contract's PANEL_FAMILIES and
PANEL_WIDGETS, omx-panel's display kinds, sources and placements, and omx-clap-core's lists, code for code) and
`make panel-layout` (`tools/test/panel-layout-test.mjs`: our plugins' descriptors state no unknown word, and
omx-panel's layout of each finds the declared family and roles; the layout's own rules are omx-panel's
[docs/panel-layout.md](https://github.com/FreeMixer/omx-panel/blob/main/docs/panel-layout.md)) and `make descriptor-view`
(`tools/test/descriptor-view-test.mjs`: each described plugin's descriptor fresh and in omx-contract's
words, and the declaration its view) and `make descriptor-features` (`tools/test/descriptor-features-check.mjs`: the
descriptor's feature words are read from the installed schema, and no tool lists them) and `make placement-strips`
(`tools/test/placement-strips-check.mjs`: a declaration's `console.placement.strips` is a restriction to fewer than the
rack kinds the schema enumerates, never the whole set, never a kind without a rack).

After the plugins, `make round-trip` runs the round trip: for every plugin with a descriptor it generates
(`node tools/gen.mjs`), builds both faces, scans each built face back with `plugin-hostd-probe describe` (plugin-hostd
>= 0.7.0 on `PATH`, its format tables installed) and compares the scanned descriptor with the authored one word by
word (`tools/round-trip.{sh,mjs}`): `equal`, `lost <format> <word>` or `differs`. It fails on a `differs`, a bag
that is not equal or a loss it does not name, and its self-test breaks a copy of a descriptor and of a built `.ttl`
and wants both red ([docs/declaration-as-descriptor.md](docs/declaration-as-descriptor.md)).

`make test` for a plugin runs, in order:

1. **freshness**: every generated file matches what its declaration generates today;
2. **parameters**: the built CLAP plugin's parameters, read through `clap_plugin_params`, are the
   declaration's, in order, with the host's bypass last;
3. **plugin probe** (`test/plugin-probe.c`): the CLAP and LV2 builds, run over a deterministic
   signal in uneven blocks with parameter changes, are bit-identical to the omx-dsp kernel called
   directly, at every declared rate, engaged, bypassed and re-activated;
4. **delay oracle** (`test/delay-oracle.c`): at 44.1, 48, 96 and 192 kHz, on both builds, the
   echoes, ping-pong, mix and bypass computed from the definition of a delay, not from omx-dsp;
5. **MOD GUI** (`omx-panel modgui --check`, `omx-panel`'s `modgui-test.sh`): fresh, carrying every declared control
   with its travel and default, valid under `sord_validate`, and stale when the declaration moves;
6. **host check**: `omx-clap-scan` loads the CLAP file, `lv2ls`/`lv2info` read the LV2 bundle;
7. **port groups** (`tools/port-groups-probe.c`, `tools/port-groups-check.mjs`): the LV2 bundle,
   read through lilv, holds one `pg:Group` per panel section (its `lv2:symbol` the section's key, its
   `lv2:name` and `rdfs:label` the section's label), and each parameter a section lists carries
   `pg:group` naming it; no other port is grouped. A section a declaration with variants marks
   `"subgroups": "band"` is a group per band instead (`band<n>`, `Band <n>`: omx-eq8's Band 1 ... Band 8),
   the panel section drawn as declared. Every group states an `lv2:displayPriority`, counted down in the
   order its first port comes, so a host showing the higher first (Zynthian) shows them in port order;
8. **panel description** (`tools/panel-probe.c`, `tools/panel-check.mjs`): each built face is opened
   as a host opens it (the `.clap` through `clap_entry`, the LV2 bundle through lilv) and its
   `org.openmixer.panel-description/1` read back: the family, the sections, every listed parameter as
   a control in its section and order, the roles and displays equal the declaration's panel, and the
   fingerprint is the SHA-256 of what it serves.
8. **CLAP extensions** (`tools/clap-ext-probe.c`, `tools/clap-ext-check.mjs`): the CLAP face's
   `org.openmixer.param-contract/1` (each parameter's symbol, unit, scale and step, version 1, the
   fingerprint of what it serves), its `clap.remote-controls` pages (each section's controls, eight
   a page) and its `clap.track-info`: under each placement, before and after activation, every
   parameter comes up at the contract's default for that kind of channel (a delay's or reverb's mix
   fully wet on a return), else its own, and a value the host set stays.

A plugin whose declaration lists `oracles` also runs each `test/<name>.c` on both builds: an oracle
moved from the console engine with its effect, at the nine oracle rates, reading only the faces' audio
(omx-transient's `test/transient-arms.c`, the engine's transient arms).

For omx-strip, steps 3 and 4 are one **strip oracle** (`test/strip-oracle.c`): at 44.1, 48, 96
and 192 kHz, on both builds, in each of the 24 stage orders and at the defaults, the output equals
omx-dsp's strip modules called in sequence by the oracle itself; bypass is the identity. omx-strip
has no MOD GUI yet (its declaration carries no panel).

## Layout

```
plugins/<name>/
  <name>.decl.json        THE declaration: parameters, identity, panel
  omx_<kernel>_core.h     the binding: each parameter to the kernel's resolve(), and the core's vtable
                          (struct omx_face_core) the shared faces call
  omx_<kernel>_clap.c     the CLAP face's descriptor: the plugin's struct omx_face (its tables, its
                          meters, its key, its core vtable) and its entry, omx_clap_entry_<kernel>
                          (clap_entry in a standalone .clap); the face itself is lib/omx_clap_face.c
  omx_<kernel>_lv2.c      the LV2 face's descriptor and lv2_descriptor(); the face is lib/omx_lv2_face.c
  generated/              written by `make gen`, committed: the parameter header and the LV2 bundle
  test/                   the probe and the oracle
include/                  shared C shapes (no DSP): omx_panel_description.h and omx_param_contract.h,
                          the openmixer extensions and their rows; omx_clap_static.h, libomxplugins' table;
                          omx_plugin_face.h, what a plugin hands the shared faces (struct omx_face), and
                          omx_clap_face.h and omx_lv2_face.h, the faces' entry points
contract/                 the definitions installed beside the descriptors in omx-contract's contract
                          directory
lib/                      omx_clap_static.c (the table's rows are generated), omxplugins.pc.in, and the
                          two faces every plugin shares: omx_clap_face.c (one object in libomxplugins.a)
                          and omx_lv2_face.c, each built beside the plugin's descriptor file
tools/                    gen.mjs (header + TTL), descriptor.mjs (a declaration as its plugin
                          descriptor), omx-panel.mjs (where the installed omx-panel is found: its layout of a
                          descriptor, its lists) and param-contract.mjs (the extensions' rows),
                          panel-file.mjs (a declaration's panel as the file `omx-panel modgui` reads),
                          the test helpers,
                          omx-new-plugin.mjs (the wizard), plugin-recipe.mjs (completeness)
recipes/                  plugin.recipe.json, the templates tools/gen.mjs writes from
schema/                   plugin.decl.schema.json, the shape of a declaration
```

## Changing a plugin

Edit the declaration, run `make -C plugins/<name> gen`, commit the declaration and `generated/`
together. Parameter order is append-only: hosts save sessions by parameter id. DSP changes go to
omx-dsp, never here.

## Adding a plugin

A plugin over an omx-dsp effect kernel starts with the wizard, never by hand
([docs/new-plugin-from-contract.md](docs/new-plugin-from-contract.md)):

```sh
node tools/omx-new-plugin.mjs --from-contract <kernel>   # drafts plugins/omx-<kernel>/omx-<kernel>.decl.json
$EDITOR plugins/omx-<kernel>/omx-<kernel>.decl.json      # settle every REVIEW mark
node tools/omx-new-plugin.mjs --from-contract <kernel>   # validates, generates everything, runs make test
```

The declaration is the folder's one hand-written file. Every parameter names its travel BY
REFERENCE into the kernel's file in omx-contract (`"ref"`, `"field"`); no number is typed in a
declaration. The release read is the installed one, through its JSON render (`omx-contract.json` in
`pkg-config --variable=datadir omx-contract`), and its version must lie in the spec's range;
`$OMX_CONTRACT_DIR`, a directory holding an `omx-contract.json`, stands in for it (the perturbation
proofs point it at a moved copy). The kernel must
have its instance face in omx-dsp (`<omxdsp/fx/omx_<kernel>_instance.h>`): each parameter binds to
the `resolve()` argument named after its contract control, and `"binding": "instance"` makes
`tools/gen.mjs` write the binding, the kernel-identity test, both faces and the Makefile. A kernel
missing from omx-contract, or without a face of that shape, is refused with the work named.

`node tools/gen.mjs` also refills the shared files' generated regions from every plugin folder: the
README's catalogue and sections (a declaration's `summary` and `manual`, else its description and
parameters), the RPM `%files` lists, CI's installed-file lists and libomxplugins' table of CLAP
entries. The package descriptions are
prose; the completeness test checks they name every shipped plugin. The older plugins (delay,
drive, the EQs, strip, keyed gate) keep their hand-written faces and bindings.

`make completeness` holds every plugin to the same recipe and names the wizard step for each gap;
`node tools/commit-plan-check.mjs <base>..<head>` holds a plugin's commits to the plan's order.

A directory with a declaration and no Makefile (omx-chorus and omx-deesser today) is not built or
packaged yet: `make completeness` lists what it still needs. `node tools/gen.mjs --check` still
keeps its generated files fresh.

## Packages

`packaging/omx-plugins.spec` builds `omx-plugins-clap`, `omx-plugins-lv2`, the `omx-plugins`
metapackage that requires both, `omx-plugins-devel` (the headers, `libomxplugins.a` and
`omxplugins.pc`), `omx-plugins-contract` (noarch; each plugin's descriptor, the param-contract units
and the manifest in omx-contract's contract directory, which a host and the console read at run time; `omx-plugins` and
`omx-plugins-devel` require it at their own version) and `omx-plugins-test` (the test instruments, in `/usr/libexec/omx-plugins/test`,
outside every CLAP search path, for host tests and for measuring them on a console), and runs `make test` in `%check`; `debian/` builds the same packages for bookworm
and trixie (`omx-plugins-dev`, with the archive in the multiarch directory, and `omx-plugins-contract`,
`Architecture: all`) and runs `make test`
unless `nocheck` is set. The version lives in three places that CI keeps equal: the top-level
`Makefile` (`make version`), the spec's `Version:` and the top of `debian/changelog`.

A `v<version>` tag runs `.github/workflows/release.yml`: RPMs for Fedora 44 (x86_64, aarch64)
and DEBs for bookworm and trixie (amd64, arm64), signed and published into the FreeMixer package
channel and attached to the GitHub release. Beside them goes
`omx-plugins-zynthian-<version>-arm64.tar.gz`: the files of the trixie arm64 `omx-plugins-lv2` and
`omx-plugins-clap` packages, repacked by `tools/zynthian-bundle.sh` as `lv2/` and `clap/` directories for
a Zynthian's plugin directories, after checking every ELF (architecture, glibc no newer than trixie's
2.41, no library a bare trixie lacks, `clap_entry` in every CLAP) and running every LV2 plugin
through `lv2bench` on arm64. Pull requests run the same workflow as a dry run that signs and publishes
nothing; there the tarball is packed and checked from the amd64 build.
