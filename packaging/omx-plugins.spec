# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
Name: omx-plugins
Version: 0.20.0
Release: 1%{?dist}
License: GPL-3.0-or-later
Summary: The OpenMixer console's delay, drive, EQ, channel strip, keyed gate and effects, as plugins for your DAW
URL: https://github.com/FreeMixer/omx-plugins

Source0: %{url}/archive/v%{version}/%{name}-%{version}.tar.gz

BuildRequires: gcc
BuildRequires: make
BuildRequires: pkgconf-pkg-config
# The releases this tree builds against: a floor each and no ceiling (floors only, as 0.8.0 ruled), once here and
# in debian/control the same (tools/deps-check.sh). The Makefile and tools/gen.mjs read them from here.
BuildRequires: omx-dsp-devel >= 0.34.0
BuildRequires: omx-contract-devel >= 3.21.0
BuildRequires: omx-clap-devel >= 0.34.1
# plugin-hostd's format tables, which the generators read backwards (tools/format-tables.mjs)
BuildRequires: plugin-hostd-devel >= 0.21.0
# the omx-contract command, first packaged in 2.15.0: the generators read the contract directory through it
# (3.3.0: the directory and its schema are omx-contract-data's, the definitions each owner's -contract package's)
# (3.20.2: the tools decide "run, not imported" with its isMain, <clidir>/lib/entry.mjs, read through pkg-config)
# (3.21.0: the descriptor word latency.sweep, which omx-rotary states)
BuildRequires: omx-contract >= 3.21.0
# the panel layout is omx-panel's, from a descriptor and the declared panel (tools/omx-panel.mjs)
BuildRequires: omx-panel >= 0.16.0
# omx-clap's domain the generators read (tools/omx-contract.mjs domainDocument): clap, the CLAP feature terms
# (tools/format-tables.mjs)
BuildRequires: omx-clap-contract >= 0.34.1
# omx-panel recommends omx-plugins-devel: an older one installed beside this build puts its release's contract
# package where make test reads this one's (tools/test/contract-dir-check.sh), so a build installs none
BuildConflicts: omx-plugins-devel < %{version}
# the panel words: omx-panel's panel domain and its render <omxdomain/panel_codes.h>, which include/omx_panel_description.h
# expands (tools/test/panel-words-check.mjs)
BuildRequires: omx-panel-contract >= 0.16.0
BuildRequires: clap-devel >= 1.2.7
# include/omx_panel_description.h includes omx-panel's <omxdomain/panel_codes.h>
Requires: omx-panel-contract >= 0.16.0
BuildRequires: lv2-devel
# %%check: the generators and checks are Node scripts; the hosts that load the built plugins
# (20.19: tools/omx-contract.mjs require()s omx-contract's entry.mjs, an ES module, unflagged from 20.19 / 22.12)
BuildRequires: nodejs >= 20.19
BuildRequires: python3
# make selftest copies the tree with tar
BuildRequires: tar
# %%check: make debt-base runs git (tools/test/debt-base-test.mjs builds its fixture repositories with it)
BuildRequires: git-core
BuildRequires: diffutils
# the curve kinds the schema's graph enum renders: omx-dsp's CURVE_KINDS (tools/graph-enum.mjs)
BuildRequires: omx-dsp-contract >= 0.34.0
BuildRequires: lilv
BuildRequires: lilv-devel
# the standard units a face publishes, by symbol (omx-contract's param domain, PARAM_UNITS x-symbol), and the OpenMixer
# unit spellings an LV2 face writes (plugin-db's lv2 domain, LV2_OPENMIXER_UNIT_IDS x-uri): tools/format-tables.mjs
BuildRequires: omx-contract-data >= 3.21.0
BuildRequires: plugin-db-contract >= 0.5.0
BuildRequires: sord
BuildRequires: omx-clap-host >= 0.34.1
# %%check: make round-trip scans the built faces back with plugin-hostd-probe describe
BuildRequires: plugin-hostd >= 0.21.0
# plugin-hostd's words the generators read: its domains plugin-hostd/clap and plugin-hostd/lv2, the format tables
# (tools/format-tables.mjs), and the resource hints, RESOURCE_* (tools/resource-hints.mjs)
BuildRequires: plugin-hostd-contract >= 0.21.0

# The name a user installs: both formats.
Requires: %{name}-clap%{?_isa} = %{version}-%{release}
Requires: %{name}-lv2%{?_isa} = %{version}-%{release}
# each plugin's descriptor, which a host and the console read at run time
Requires: %{name}-contract = %{version}-%{release}
Recommends: plugin-hostd >= 0.21.0

%description
Take the sound of the OpenMixer console into your DAW or onto your own rig:
delay, drive, the channel EQ as an 8-band EQ, a 16-band EQ or a 32-band EQ, a
31-band graphic EQ, the full channel strip and the keyed gate with its sidechain
input, and the console's effects: chorus, flanger, phaser, tremolo, rotary
speaker, reverb, pitch shifter, transient shaper, de-esser, sub-octaver, plosive tamer, clipper, upward compressor, bus compressor, loudness leveler, spectral balancer, spring reverb, doubler, tone control, stereo width, ducker, input trim, limiter, compressor
with its sidechain key, multiband compressor and dynamic EQ. They come
as CLAP plugins for Bitwig, REAPER and Carla, and as LV2 plugins for Ardour,
Carla, MOD and Zynthian. Each one runs the same DSP as the console, so a track sounds the same
in a session as it does on the desk, sample for sample. Every plugin comes in
both formats with the same controls.
Its documentation is on openmixer.org.

This package installs both formats; omx-plugins-clap and omx-plugins-lv2
install one each. It also carries the factory presets of every plugin, as
files in %{_datadir}/omx/presets.

%package clap
Summary: OpenMixer delay, drive, EQ, strip, gate and effects as CLAP plugins

%description clap
The OpenMixer console's delay, drive, 8-, 16- and 32-band EQ, 31-band graphic
EQ, channel strip, keyed gate and effects (chorus, flanger, phaser, tremolo,
rotary speaker, reverb, pitch shifter, transient shaper, de-esser, sub-octaver, plosive tamer, clipper, upward compressor, bus compressor, loudness leveler, spectral balancer, spring reverb, input trim, limiter, compressor) as
CLAP plugins, installed in /usr/lib/clap where Bitwig, REAPER,
Carla and other CLAP hosts find them. They run the console's own DSP, so they
sound like the desk.
Its documentation is on openmixer.org.

%package lv2
Summary: OpenMixer delay, drive, EQ, strip, gate and effects as LV2 plugins
Requires: lv2
Recommends: omx-panel >= 0.16.0

%description lv2
The OpenMixer console's delay, drive, 8-, 16- and 32-band EQ, 31-band graphic
EQ, channel strip, keyed gate and effects (chorus, flanger, phaser, tremolo,
rotary speaker, reverb, pitch shifter, transient shaper, de-esser, sub-octaver, plosive tamer, clipper, upward compressor, bus compressor, loudness leveler, spectral balancer, spring reverb, input trim, limiter, compressor) as
LV2 plugins, installed in %{_libdir}/lv2 where Ardour, Carla, MOD
and Zynthian find them. They run the
console's own DSP, so they sound like the desk.
Its documentation is on openmixer.org.

%package test
Summary: The OpenMixer test instruments, for host tests and plugin measurement

%description test
The test instruments the OpenMixer console's own proofs play: a sine voice, a
pool voice, a note echo and a transport probe, as CLAP plugins. They are installed in
%{_libexecdir}/omx-plugins/test, outside every CLAP search path, so no host
lists them; a test or the plugin qualifier opens them there by path. Install
this package on a console to measure the linked test instruments' cost on its
own machine.
Its documentation is on openmixer.org.

%package devel
Summary: Headers and static library of the OpenMixer plugins, for projects that build on them
Requires: omx-dsp-devel >= 0.34.0
# the generated parameter headers include omx-contract's C renders, and omxplugins.pc requires omx-contract
Requires: omx-contract-devel >= 3.21.0
# include/omx_param_contract.h includes omx-clap's scale codes, <omx-clap-host/omx_param_scales.h>
Requires: omx-clap-devel >= 0.34.1
Requires: %{name}-contract = %{version}-%{release}
Requires: clap-devel >= 1.2.7

%description devel
The headers other projects include to use the OpenMixer plugins' parameters and
instance code, such as omx_delay_core.h and each plugin's generated
parameter header, installed in %{_includedir}/omx-plugins, and
libomxplugins.a (pkg-config omxplugins): every plugin's CLAP face in one
static library, with the table of their entries in omx_clap_static.h, for a
host that runs the plugins in its own process without loading .clap files.
Its documentation is on openmixer.org.

%package contract
Summary: The OpenMixer plugins' descriptors in omx-contract's contract directory
BuildArch: noarch
# the contract directory, its schema and its reader's own package
# 3.19.1: its install-contract-dir.sh rewrites the relative $schema of contract/*.json and the descriptors to the
# major's URL, and its reader requires that URL (3.13.0 brought the roleMeanings kind of contract/role-meanings.json)
Requires: omx-contract-data >= 3.21.0

%description contract
Each OpenMixer plugin's descriptor, its parameters in omx-contract's words,
and the manifest naming this release, as files in omx-contract's contract
directory, for any program that reads them at run time: a host, the console's
panels and the omx-contract command.
Its documentation is on openmixer.org.

%prep
%autosetup

%build
%set_build_flags
%make_build

%install
%make_install PREFIX=%{_prefix} LIBDIR=%{_libdir} CLAPDIR=/usr/lib/clap
%{__make} install-devel DESTDIR=%{buildroot} PREFIX=%{_prefix} LIBDIR=%{_libdir}
%{__make} install-test DESTDIR=%{buildroot} PREFIX=%{_prefix} TESTCLAPDIR=%{_libexecdir}/omx-plugins/test

%check
%make_build test

%files
%license LICENSE
%doc README.md
# the factory presets of every plugin, read only data the console's preset library lists (presets/)
%{_datadir}/omx/presets/

%files clap
%license LICENSE
%doc README.md
%dir /usr/lib/clap
# BEGIN GENERATED clap-files: tools/gen.mjs, one line per shipped plugin
/usr/lib/clap/omx-balancer.clap
/usr/lib/clap/omx-chorus.clap
/usr/lib/clap/omx-clipper.clap
/usr/lib/clap/omx-comp.clap
/usr/lib/clap/omx-deesser.clap
/usr/lib/clap/omx-delay.clap
/usr/lib/clap/omx-doubler.clap
/usr/lib/clap/omx-drive.clap
/usr/lib/clap/omx-ducker.clap
/usr/lib/clap/omx-dyneq.clap
/usr/lib/clap/omx-eq8.clap
/usr/lib/clap/omx-eq16.clap
/usr/lib/clap/omx-eq32.clap
/usr/lib/clap/omx-flanger.clap
/usr/lib/clap/omx-geq.clap
/usr/lib/clap/omx-glue.clap
/usr/lib/clap/omx-keyed-gate.clap
/usr/lib/clap/omx-limiter.clap
/usr/lib/clap/omx-loudlevel.clap
/usr/lib/clap/omx-multiband.clap
/usr/lib/clap/omx-phaser.clap
/usr/lib/clap/omx-pitch.clap
/usr/lib/clap/omx-plosive.clap
/usr/lib/clap/omx-reverb.clap
/usr/lib/clap/omx-rotary.clap
/usr/lib/clap/omx-spring.clap
/usr/lib/clap/omx-strip.clap
/usr/lib/clap/omx-suboct.clap
/usr/lib/clap/omx-tone.clap
/usr/lib/clap/omx-transient.clap
/usr/lib/clap/omx-tremolo.clap
/usr/lib/clap/omx-trim.clap
/usr/lib/clap/omx-upward.clap
/usr/lib/clap/omx-width.clap
# END GENERATED clap-files

%files test
%license LICENSE
%doc README.md
%dir %{_libexecdir}/omx-plugins
%dir %{_libexecdir}/omx-plugins/test
# BEGIN GENERATED clap-test-files: tools/gen.mjs, one line per test plugin
%{_libexecdir}/omx-plugins/test/omx-test-note-echo.clap
%{_libexecdir}/omx-plugins/test/omx-test-pool-voice.clap
%{_libexecdir}/omx-plugins/test/omx-test-sine-voice.clap
%{_libexecdir}/omx-plugins/test/omx-test-transport-probe.clap
# END GENERATED clap-test-files

%files devel
%license LICENSE
%doc README.md
%{_includedir}/omx-plugins/
%{_libdir}/libomxplugins.a
%{_libdir}/pkgconfig/omxplugins.pc

%files contract
%license LICENSE
%doc README.md
# each plugin's descriptor and the manifest, in omx-contract's contract directory (the directory itself is omx-contract-data's)
%{_datadir}/omx-contract/contract.d/3/omx-plugins/

%files lv2
%license LICENSE
%doc README.md
# BEGIN GENERATED lv2-files: tools/gen.mjs, one line per shipped plugin
%{_libdir}/lv2/omx-balancer.lv2/
%{_libdir}/lv2/omx-chorus.lv2/
%{_libdir}/lv2/omx-clipper.lv2/
%{_libdir}/lv2/omx-comp.lv2/
%{_libdir}/lv2/omx-deesser.lv2/
%{_libdir}/lv2/omx-delay.lv2/
%{_libdir}/lv2/omx-doubler.lv2/
%{_libdir}/lv2/omx-drive.lv2/
%{_libdir}/lv2/omx-ducker.lv2/
%{_libdir}/lv2/omx-dyneq.lv2/
%{_libdir}/lv2/omx-eq8.lv2/
%{_libdir}/lv2/omx-eq16.lv2/
%{_libdir}/lv2/omx-eq32.lv2/
%{_libdir}/lv2/omx-flanger.lv2/
%{_libdir}/lv2/omx-geq.lv2/
%{_libdir}/lv2/omx-glue.lv2/
%{_libdir}/lv2/omx-keyed-gate.lv2/
%{_libdir}/lv2/omx-limiter.lv2/
%{_libdir}/lv2/omx-loudlevel.lv2/
%{_libdir}/lv2/omx-multiband.lv2/
%{_libdir}/lv2/omx-phaser.lv2/
%{_libdir}/lv2/omx-pitch.lv2/
%{_libdir}/lv2/omx-plosive.lv2/
%{_libdir}/lv2/omx-reverb.lv2/
%{_libdir}/lv2/omx-rotary.lv2/
%{_libdir}/lv2/omx-spring.lv2/
%{_libdir}/lv2/omx-strip.lv2/
%{_libdir}/lv2/omx-suboct.lv2/
%{_libdir}/lv2/omx-tone.lv2/
%{_libdir}/lv2/omx-transient.lv2/
%{_libdir}/lv2/omx-tremolo.lv2/
%{_libdir}/lv2/omx-trim.lv2/
%{_libdir}/lv2/omx-upward.lv2/
%{_libdir}/lv2/omx-width.lv2/
# END GENERATED lv2-files

%changelog
* Sun Nov 22 2026 Pau Aliagas <linuxnow@gmail.com> - 0.20.0-1
- the version moves to 0.20.0
- floors: omx-clap-devel, omx-clap-contract and omx-clap-host >= 0.34.1 (0.34.1 carries
  the rename's Obsoletes an upgrade needs), omx-contract >= 3.21.0,
  omx-dsp >= 0.34.0
- the sorting pass: the moves into the owner domains
- units: the vendor block is omx-contract 3.21.0's eight (dbfs, dbu, ratio, lufs, dbPerOct, dbPerS, dbtp, lu), and a
  slope is served: dB/oct writes the sheet's own token dbPerOct (260) in the param contract, the token a host
  fingerprints, so the balancer, comp, eq8, eq16, eq32 and strip contracts each move on (clap#163, omx-plugins#213);
  each of the six gained two entries in tools/param-contract-versions.json, the first (lowercased dbperoct) never
  served, kept because the list is append-only
- no shipped pins: the pins.d route (plugin-hostd#42) is retired, and tools/gen-pins.sh, the packaged
  /usr/share/plugin-hostd/pins.d/omx-plugins.pins, tools/pins-check.sh and the build dependency on
  plugin-qualify go; our plugins are qualified like any plugin, plugin-qualify measuring them into the
  qualification records once the release is served (omx-plugins#211)
- CLAP remote controls: a page whose every slot the governing values hide is not served (omx-panel's page
  rule, no screen has zero visible items), so omx-strip at 4 bands serves 11 pages, not 39 (omx-plugins#212)
- make test runs every part whatever an earlier part answered, and runs strip-pages-follow; %%check needs git-core

* Wed Nov 11 2026 Pau Aliagas <linuxnow@gmail.com> - 0.19.3-1
- omx-plugins-contract installs role-meanings.json (the meaning each panel
  role plays) and plugin-face.json (the face ids, declared here since
  omx-contract 3.13.0 no longer does); omx-contract-data >= 3.19.1 and < 4
- a ratio travel's unit is ratio in the parameter contract, as in the LV2 face
- omx-comp draws its transfer curve and gain-reduction bar; the panels are
  laid out by omx-panel >= 0.13.2 and < 0.14
- the contract fingerprint names each parameter by its CLAP id, so a host
  that computes it from the plugin trusts the contract

* Mon Nov 09 2026 Pau Aliagas <linuxnow@gmail.com> - 0.19.2-1
- built and tested on the one-panel wave's releases, each with a floor and a
  ceiling: omx-dsp >= 0.32.0 and < 0.33, omx-contract >= 3.12.0 and < 4,
  omx-clap-core and omx-clap-host >= 0.32.2 and < 0.33, plugin-hostd >= 0.20.0
  and < 0.21, omx-panel >= 0.12.0 and < 0.13, plugin-qualify >= 0.20.0 and
  < 0.21, and the CLAP headers >= 1.2.7 (1.2.10 on Debian) and < 2
- the control-surface pages and faces come from omx-panel's one cut and are
  generated again on omx-panel 0.12

* Fri Nov 06 2026 Pau Aliagas <linuxnow@gmail.com> - 0.19.1-1
- omx-dsp is taken at 0.31: >= 0.31.0 and < 0.32 (the build, the -devel package
  and omxplugins.pc), and omx-panel at 0.11: >= 0.11.0 and < 0.12, so the
  channel's omx-dsp 0.31.0 and omx-panel 0.11.0 resolve beside this release;
  the MOD GUIs are generated again by omx-panel 0.11.0

* Wed Nov 04 2026 Pau Aliagas <linuxnow@gmail.com> - 0.19.0-1
- omx-delay syncs to the host's tempo: sync, division, mode and duck, appended,
  defaults unchanged in sound; with no tempo a synced delay keeps its Time (#311)
- omx-strip's EQ has up to 32 bands, chosen by Bands; existing ids and ports do
  not move; band controls and CLAP pages follow Bands (#312, #317, #318)
- New plugins: omx-doubler, omx-tone and omx-ducker, each with CLAP and LV2 faces and
  factory presets, over omx-dsp's doubler, tone and comp faces
- omx-glue gains Detector Topology and the VCA Bus 4:1 preset, ssl-bus-glue runs
  the feedback detector (#310); omx-comp gains the ducker preset (#309)
- Readouts on both faces: LV2 output ports beside clap.meters/1, and meters in
  every descriptor; the limiter's look-ahead held by an impulse (#313)
- The host's bypass keeps CLAP id 0x7FFF0000 in every plugin, LV2 enabled and
  latency are ports 0 and 1; automation stored on the bypass by id must be bound
  again (#315)
- A quiet block skips the resolve unless the tempo moved
- The MOD GUI film strips are rendered by omx-panel at build time
- The packages install their licence and a short README and no other markdown
  (#314)
- Build floors: omx-dsp 0.30.0, omx-contract 3.9.0, omx-panel 0.10.0
- Every dependency on a library of ours states a floor and a ceiling, in the spec, debian/control and
  omxplugins.pc: omx-dsp >= 0.30.0 and < 0.31, omx-contract >= 3.9.0 and < 4, omx-clap-core >= 0.24.0 and
  < 0.31, plugin-hostd >= 0.15.2 and < 0.20, omx-panel >= 0.10.0 and < 0.11, plugin-qualify >= 0.9.0 and
  < 0.20 (tools/deps-check.sh holds them)

* Tue Nov 03 2026 Pau Aliagas <linuxnow@gmail.com> - 0.18.0-1
- Each plugin runs in place where it is proven to: a host that hands an output
  the buffer of its input (CLAP's in_place_pair, an LV2 host that connects both
  ports to one buffer) gets the same result as with two buffers, held by a test
  per plugin.
- A quiet block skips the resolve: when no parameter moved since the last block,
  the plugin runs on the coefficients it holds.
- The CLAP face reads its dirty flag with acquire, against the main thread's
  release store.
- Built on omx-dsp 0.29.0's fast maths (libmvec where the toolchain has it); the
  build floor is omx-dsp 0.29.0.
- The CLAP plugins no longer carry the parameter-contract migration members,
  which no plugin ever filled (zero steps everywhere); a host older than omx-
  clap-host 0.23.0 read past the shorter table, so the package declares the
  conflict (#184).

* Sat Oct 31 2026 Pau Aliagas <linuxnow@gmail.com> - 0.17.0-1
- omx-dyneq, the six-band dynamic EQ, and omx-multiband, the multiband
  compressor of two to five bands, from omx-dsp 0.26.0's kernels (#179).
- omx-comp takes a key: a sidechain port, the key and duck sections and
  the compKey meter (#179).
- The EQs' and the multiband's band centres, the deesser, drive and
  phaser frequencies and every integer frequency are authored
  logarithmic, as their LV2 faces state; a Hz travel from 0 is not (#276).
- omx-geq declares the fader for its 31 bands (#231).
- The panels are regenerated against omx-panel 0.7.1: the MOD GUI text in
  Inter and the phaser's depth published (#232).
- DEB builds install python3 in CI and read the version without make's
  directory lines (#278).
- omx-test-pool-voice, a test instrument that renders its voices as
  clap.thread-pool tasks (installed by omx-plugins-test only).
- omx-suboct (sub-octaver), omx-plosive (plosive tamer), omx-clipper,
  omx-upward (upward compressor), omx-glue (bus compressor), omx-loudlevel
  (loudness leveler), omx-balancer (spectral balancer) and omx-spring
  (spring reverb), each with its factory presets.
- omx-delay takes up to four read taps of its one line: the number of taps
  and, for each, its time, gain and pan.
- The unit codes come from omx-contract 3.6.0 (omx_param_units.h); the copy in
  omx-plugins-contract is gone, and the build floors are omx-dsp 0.28.0,
  omx-contract 3.6.0 and plugin-hostd 0.15.2.

* Thu Oct 29 2026 Pau Aliagas <linuxnow@gmail.com> - 0.16.0-1
- The EQs carry one parameter group per band on eq8, eq16 and eq32, so a
  touchscreen shows one band per screen (#257).
- The generated headers, LV2 files and descriptors carry the release
  version, taken from the Makefile VERSION (#260).
- The parameter reference of every plugin is generated from the
  declarations and checked.
- The generated doap:maintainer states foaf:homepage, and the manifests
  list modgui.ttl first, so omx-panel 0.5.0's check finds it (#244).
- The panel layout and the MOD GUI are omx-panel's: this repository hands
  it a descriptor and a declared panel, and a build needs omx-panel >=
  0.2.0 (#233).
- The commit-message check runs under the ai-trace-check action and in
  its own named job.

* Wed Oct 28 2026 Pau Aliagas <linuxnow@gmail.com> - 0.15.1-1
- The sixty band-Q controls of eq8, eq16, eq32 and strip link
  EQ_NOTCH_Q_RANGE, the range their travel states (0.3..116), so the travel
  each descriptor states is what its link resolves to. The generator is fixed
  and a check holds every one of the sixty to it (#252).

* Tue Oct 27 2026 Pau Aliagas <linuxnow@gmail.com> - 0.15.0-1
- The plugins' descriptors ship in a noarch package, omx-plugins-contract, in omx-contract's contract
  directory beside a MANIFEST.json naming the release; omx-plugins and the devel package require it.
  The omx-contract floor is 3.4.0.
- The parameter units are a sheet, contract/param-units.json, in that directory, and the unit enum in
  omx_param_contract.h is generated from it.
- The factory kernels' calls follow omx-dsp 0.26.0 (floor 0.26.0): comp, delay, gate and reverb pass its
  new resolve arguments, and a declaration holds a face argument at its kernel's named default.
- The omx-clap-core floor is 0.14.0, the first tag that carries what 0.13.0 was meant to.
- preset-probe refuses a staged location longer than its buffer instead of truncating it; the
  round-trip scan entry for stepped-to-log parameters holds through plugin-hostd 0.12.0.

* Thu Oct 22 2026 Pau Aliagas <linuxnow@gmail.com> - 0.14.0-1
- Our own plugins' pins are known at build time, so they ship with the package instead of being
  recorded by plugin-hostd on first use (plugin-hostd issue #42, pins.d): plugin-qualify --pins runs
  over the plugins the build just installed and the result is packaged as
  /usr/share/plugin-hostd/pins.d/omx-plugins.pins (RPM and DEB), replaced with the binaries in the
  same transaction and removed with the package (tools/gen-pins.sh).
- The package recommends plugin-hostd >= 0.5.0, the first release that reads pins.d: a floor only, no
  ceiling; it co-owns the pins.d directory.
- %%check (RPM) and dh_auto_install (DEB) hold the shipped pins to the shipped binaries' own hashes:
  a plugin's binary changed after its pin was written goes red (tools/pins-check.sh).
- The shipped pin is measured after the package is stripped, so it names the bytes the package ships:
  find-debuginfo (RPM) and dh_strip (DEB) run before make install-pins.
- The new subpackage omx-plugins-test (RPM and DEB) carries the test instruments, a sine voice, a note
  echo and a transport probe, in %%{_libexecdir}/omx-plugins/test, outside every CLAP search path; make
  install-test installs them, make install never does. tools/package-lists-check.sh (make
  package-lists, in make test) refuses a built plugin that no package lists.
- CI runs clap-validator on every built CLAP plugin (tools/clap-validate.sh) and keeps the JSON
  reports; its state-* tests are required (tools/clap-validate-required.mjs), the others are reported.
- Every plugin's CLAP state round trips exactly: a loaded state asks the host to rescan the values, the
  state carries the host's bypass (older states still load), a state that names no parameter is refused
  and a plugin's worst-case state fits the loader. clap-probe state, in every plugin's make test,
  checks it.
- The factory presets, the JSON files in presets/, are offered in every host: CLAP preset-discovery
  (clap.preset-discovery-factory, files in /usr/share/omx/presets/<clap id>/) and clap.preset-load,
  which applies a preset's parameters and asks the host to rescan the values, and a presets.ttl in each
  LV2 bundle with one pset:Preset per file. make test lists and loads them through CLAP discovery,
  preset-load and lilv (FreeMixer/omx-plugins issue #214).
- omx-eq8, omx-eq16, omx-eq32 and omx-strip install their descriptors too: tools/gen.mjs renders the
  descriptor of a base of variants and of a chain, so the contract directory holds all nineteen and
  MANIFEST.json lists them; their CLAP faces state the parameter modules and their LV2 faces the
  designations, as every described plugin's do (tools/test/contract-dir-check.sh requires the four).
- The round-trip ledger records the seven EQ frequency ports that are authored stepped and scan
  back as log, against plugin-hostd issue #147.
- Every plugin declares its resource hints (omx-contract 3.1.0): threads-spawned 0, streams-from-disk
  false and opens-sockets false, served by the CLAP face as org.openmixer.resource-hints.draft/0 and stated
  by the LV2 face as omxres: properties, both from the one declaration. The footprints and the activate
  time stay undeclared until a measurement backs them. Builds against omx-contract >= 3.1.0.
- The Zynthian tarball is repacked from the trixie arm64 DEBs, for ZynthianOS Vangelis (Debian
  trixie); Oram (bookworm) gets no upstream updates. The bundle check refuses a glibc symbol above
  trixie's 2.41 and a library a bare trixie lacks (tools/zynthian-bundle.sh, FreeMixer/omx-plugins
  issue #225).

* Wed Oct 21 2026 Pau Aliagas <linuxnow@gmail.com> - 0.13.0-1
- The descriptors install under contract.d/3, in omx-contract 3.0.0's words:
  CLAP's open features and the LV2 class in the extension bag, the one-line
  description in plugin.doc, a ratio's unit ratio.
- The generators read plugin-hostd's format tables backwards: the LV2 faces
  state 57 lv2:designation param: terms and the ratio unit
  urn:openmixer:unit#ratio, the CLAP faces each parameter's module.
- make round-trip, in make test, replaces make face-descriptor: every described
  plugin's faces are built, scanned back with plugin-hostd-probe describe and
  compared with the authored descriptor word by word.
- Builds against omx-contract >= 3.0.0, plugin-hostd-devel >= 0.7.0 and
  omx-clap-core-devel >= 0.13.0.

* Tue Oct 20 2026 Pau Aliagas <linuxnow@gmail.com> - 0.12.0-1
- Each plugin's descriptor is installed byte for byte in omx-contract's contract
  directory, as contract.d/2/omx-plugins/plugins/<name>.json, with a
  MANIFEST.json that lists every file with its sha256 and this version.
  omx-plugins-devel carries them and needs omx-contract-devel 2.16.0 or newer,
  which owns the directory.
- The spec builds with diffutils, which the new check uses to compare the
  installed descriptors with the sources.

* Sun Oct 18 2026 Pau Aliagas <linuxnow@gmail.com> - 0.11.0-1
- Factory presets: 120 presets for the 19 plugins, installed with the
  omx-plugins package as files in /usr/share/omx/presets, one directory per
  plugin. The catalogue presets are checked against their measured figures
  through the DSP.
- omx-plugins-devel also installs the test plugins' generated declaration
  headers, which hold the constants and CLAP id a test predicts from.
- The compressor and de-esser level meters read their level through omx-dsp's
  omx_meter_level_db instead of a copy of their own. The readings are bit for
  bit the same.
- Builds against omx-dsp >= 0.24.0 and omx-contract >= 2.16.0.
- The oracles take their sample rates from omx-contract's
  OMX_ORACLE_FULL_RATES_* names.
- The strip's cross-check declares the low-pass above 0.6 of Nyquist as a
  known gap and checks the rest.
- The parameter, panel, CLAP extension and port group checks each have
  sabotage tests that must fail on a broken face.
- A parallel make no longer races on the EQ default probe: each build makes
  its own copy and moves it into place.

* Sat Oct 17 2026 Pau Aliagas <linuxnow@gmail.com> - 0.10.0-1
- Every plugin over one kernel is described in omx-contract's own words: a
  descriptor file per plugin carries its identity, its controls with their
  symbols, names, groups, meanings and the definitions their travels are
  read from, its ports and its latency, and the declaration keeps only what
  the descriptor has no word for.
- The built CLAP and LV2 faces are scanned as a host scans any plugin, and
  must state exactly what the descriptor says.
- The panel description is laid out from a descriptor, so any plugin, ours
  or a scanned one, gets a panel; a LUFS or dBu level takes the threshold
  and ceiling roles.
- omx transient is no longer drawn under the compressor's family: a declared
  family needs the meanings it stands on.
- Builds against omx-contract >= 2.15.0 and omx-dsp >= 0.22.0, which ships
  the kernels' definitions; the build reads them from the contract directory
  through the omx-contract command.

* Fri Oct 16 2026 Pau Aliagas <linuxnow@gmail.com> - 0.9.0-1
- Every meter is coherent: the plugins publish their meters through a ring
  of three copies, so a host never reads values from two different blocks.
- omx comp reports its gain reduction and its input and output levels, and
  the de-esser its input and output levels.
- omx pitch declares its units: semitones and cents are whole steps that a
  host shows as semitones and cents, and the mix as a percent.
- The plugins use the meters header omx-clap-core installs instead of a
  copy of their own.
- The first release that waits for plugin-qualify: every plugin is measured
  for real-time safety and must be rated suitable before any package is
  built, and the verdicts are kept with the release run.

* Thu Oct 15 2026 Pau Aliagas <linuxnow@gmail.com> - 0.8.0-1
- Library dependencies are a floor only, no ceiling: omx-dsp >= 0.18.0
  (the first release the plugins build and pass make test against; 0.17.0
  does not link) and omx-contract >= 2.11.0. tools/deps-check.sh fails on
  any upper bound.
- The parameter-contract header names the LUFS unit (OMX_CLAP_UNIT_LUFS,
  0x103), which openmixer's MAIN loudness target travels in.

* Wed Oct 14 2026 Pau Aliagas <linuxnow@gmail.com> - 0.7.0-1
- Built against omx-dsp 0.18.0 (at least 0.18.0, below 0.19) and omx-
  contract 2.11.0 (at least 2.11.0, below 2.12). No pins file: the
  packaging declares both ranges and the build reads them from there.
- The manuals read every range, default and limit from the plugins'
  parameters, which come from omx-contract, so a travel moved in the
  contract moves the manual with it.

* Tue Oct 13 2026 Pau Aliagas <linuxnow@gmail.com> - 0.6.0-1
- Built against omx-dsp 0.13.0 and omx-contract 2.6.0, the pair the channel
  installs.
- omx comp takes the compressor's detector topology: Feedforward hears the
  input, Feedback the compressor's own output. omx strip keeps its
  parameters and runs its compressor feed-forward.
* Tue Oct 13 2026 Pau Aliagas <linuxnow@gmail.com> - 0.5.1-1
- omx deesser reports its gain reduction: the CLAP face serves the kernel's
  gain reduction per block through clap.meters/1, and its oracle holds that
  meter to the kernel at every rate.
- A plugin declares its meters in its declaration; the generator serves them
  through clap.meters/1 and checks them in the oracle.
* Mon Oct 12 2026 Pau Aliagas <linuxnow@gmail.com> - 0.5.0-1
- The faces follow omx-dsp 0.6.0, the oldest release they build against now.
- omx pitch reports its latency at full mix: its shortest delay plus half its
  window, 883 frames at 44.1 kHz, and 0 below 100 %, where the dry signal is
  on time. The CLAP face takes a new latency at its next activation and asks
  the host for that restart when the mix crosses 100 %; the LV2 latency port
  follows the mix.
- Every face serves its panel description, read from its declaration.
- The CLAP faces carry the extensions their declaration names: the parameter
  contract, remote-control pages and each placement's defaults.
- Each plugin declares its panel once; the console's sections and graphs are
  part of it.
- The panels use one widget vocabulary, the console's names.
- The LV2 faces group their ports, one group per panel section.
- The MOD GUI look is generated from its design tokens.
- The declarations' comments are in plain words.
* Sun Oct 11 2026 Pau Aliagas <linuxnow@gmail.com> - 0.4.0-1
- **libomxplugins**: one static library with a table of every plugin's CLAP
  entry, so a host links the plugins in-process without dlopen. The devel
  package ships `libomxplugins.a`, `omx-plugins/omx_clap_static.h` and
  `omxplugins.pc`; `omx_clap_static_find(id)` returns a plugin's entry and
  `omx_clap_static_entry` is one entry whose factory lists them all.
* Sat Oct 10 2026 Pau Aliagas <linuxnow@gmail.com> - 0.3.0-1
- New plugin, **omx comp** (CLAP and LV2): the console's compressor.
  Threshold, ratio and knee set how hard it holds the level down, attack and
  release how fast, with make-up gain and a dry/wet mix; Kind picks the RMS
  detector (compressor) or the peak detector (limiter), and detector
  oversampling can run the control path at 4x.
- **omx strip** is now built from its declaration, as a chain of the console's
  own stages: input trim, high- and low-pass filters, gate, four-band EQ and
  compressor, each with its own on/off switch, in the console's order or any
  other.
- **omx strip: sessions saved with 0.2.0 do not load as they were.** Its
  parameter list changed: the new Input and Filters switches come first, so
  every later parameter moved; Order now picks among the 120 orders of five
  stages (0 is still the console's order); the gate gains knee start and end,
  hold and hysteresis; the RMS switch is replaced by Kind (compressor, RMS
  detector, or limiter, peak), and a fresh strip now starts as a compressor
  where 0.2.0 started with the peak detector; the compressor gains mix and
  detector oversampling. At the settings 0.2.0 had, the strip sounds the same,
  sample for sample.
- Builds against omx-dsp 0.5.2 and omx-contract 2.3.0, the releases with the
  input trim.

* Thu Oct 08 2026 Pau Aliagas <linuxnow@gmail.com> - 0.2.0-1
- New plugin, **omx keyed-gate** (CLAP and LV2): the console's keyed gate, its
  detector fed by a sidechain key the host routes to it, or by the signal
  itself. It takes over the keyed gate bundle the OpenMixer console shipped,
  with the same LV2 URI, ports and defaults.
- New plugins, **omx eq16** and **omx eq32** (CLAP and LV2): the console's
  channel EQ as a 16-band EQ and a 32-band EQ, the same bands, pass filters
  and DSP as omx eq8. Stereo, zero latency; every band starts off, so a
  freshly loaded instance passes the signal untouched.
- Every LV2 plugin shows its controls as the console's bundles did:
  frequencies on a logarithmic travel in hertz, gains in dB, times,
  percentages and octaves in their units, the latency in frames, and the EQ
  band types and filter slopes as named choices instead of bare numbers.
- New package **omx-plugins-devel** (RPM) and **omx-plugins-dev** (DEB): the
  headers other projects include, such as omx_delay_instance.h and each
  plugin's generated parameter header, so they stop copying them.
- Every parameter's travel, default and choices now come from the OpenMixer
  contract (omx-contract 1.3.0), not from numbers typed in each plugin, so a
  plugin and the console cannot disagree about a range. A fresh EQ (eq8, eq16,
  eq32) now starts with its bands where the console's one rule puts them: four
  bands at 100, 400, 2000 and 8000 Hz, any other count spread evenly and
  snapped to preferred frequencies, a low shelf first, a high shelf last and
  bells between. Every band still starts off, so a freshly loaded instance
  passes the signal untouched.
- The CLAP plugins now install to /usr/lib/clap on every distribution, the
  path the CLAP standard gives for Linux. The Fedora packages used
  /usr/lib64/clap before.
- Builds against omx-dsp 0.2.0 and omx-contract 1.3.0; CI runs on GitHub's
  ubuntu-latest runners.

* Wed Oct 07 2026 Pau Aliagas <linuxnow@gmail.com> - 0.1.0-1
- First package: the OpenMixer console's delay, drive, 8-band EQ and channel
  strip, each as a CLAP and an LV2 plugin, running the same DSP as the console
  (omx-dsp 0.1.4).
