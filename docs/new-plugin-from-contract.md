<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com> -->
# A new plugin from its contract

How a plugin over an omx-dsp effect kernel is born, and what a person writes.

## The inputs

| Input | Where | Who writes it |
|---|---|---|
| The kernel's controls: names, travels, choices | the installed omx-contract, read only through its JSON render (`omx-contract.json` in `pkg-config --variable=datadir omx-contract`) | omx-contract |
| The kernel's instance face: `init`, `resolve`, `run`, latency | omx-dsp, `<omxdsp/fx/omx_<kernel>_instance.h>` | omx-dsp |
| The plugin: identity, design choices, `panel`, `console` | `plugins/<stem>/<stem>.decl.json`, the folder's ONE hand-written file | a person, from the wizard's draft |

There is no answers file. The declaration is the wizard's input and its output.

## Four commands

```sh
node tools/omx-new-plugin.mjs --from-contract transient   # 1. drafts plugins/omx-transient/omx-transient.decl.json
$EDITOR plugins/omx-transient/omx-transient.decl.json     # 2. a person settles every REVIEW mark
node tools/omx-new-plugin.mjs --from-contract transient   # 3. validates, generates everything, runs make test
git commit ...                                            # 4. the commit plan it printed, one concern per commit
```

Step 1 checks before it writes:

- the kernel is in the installed omx-contract (else: the omx-contract work, `tools/new-kernel.mjs`, is
  named and nothing is written);
- omx-dsp has the kernel's instance face, in the shape the generated binding calls (`init(s, sr)`,
  `resolve(s, bypass, <one scalar per control>)`, `run(s, in_l, in_r, out_l, out_r, n)`); a kernel
  without one, or with a face of another shape (ring buffers handed in, a ports struct), is refused
  with the omx-dsp work named. A control the kernel takes once per band is one array argument,
  `const float band[OMX_GEQ_BANDS]`, sized by a count the contract renders: the draft gives it one
  parameter per band (`band01` … `band31`), the binding passes them in declaration order, and the
  generated code asserts their count is the extent. Where the extent is the face's own macro
  (`OMX_DYNEQ_INSTANCE_BANDS`), the count is the one the control declares (`DYNEQ_BANDS`), and the same
  assertion holds the two equal. A band's centre whose travel names no default (the dynamic EQ's: its
  bands come up spread, as an EQ's do) is settled by a person with `defaultBand` `{ "count": "DYNEQ_BANDS",
  "index": <band>, "of": "centre" }`: the centre omx-contract's EQ default rule gives that band in a bank
  of that count, the one the face comes up at. A keyed face takes the sidechain block first,
  `run(s, key, in_l, in_r, out_l, out_r, n)` (`NULL`: nothing routed, the kernel's own detector); its
  declaration names the key port in `sidechain`, and the faces and the identity test carry the key.
  A keyable face keeps an unkeyed `run` and adds `run_keyed(s, key_l, key_r, in_l, in_r, out_l, out_r, n)`
  (the compressor's): a declaration that names a `sidechain` runs it with the mono key on `key_l` and
  `key_r` `NULL` (one leg read on both), one that names none runs `run`;
- every `resolve` argument binds to exactly one contract control BY NAME: the argument is the
  control's name in snake case (`attackDb` → `attack_db`). An argument that still carries an older
  short name (`attack_ms` for `attackTimeMs`) binds when its words are, in order, a unique subset
  of the control's; the generated binding then lists it as a rename owed by omx-dsp's name
  conformance, and `OMX_BINDING_STRICT=1` refuses it.

## What is drafted, what a person writes

| Field | Drafted as | A person |
|---|---|---|
| `stem`, `kernel`, `kernels` | `omx-<kernel>` (underscores become hyphens), the kernel | may rename the stem before the first release |
| `name`, `vendor`, `url`, `version`, `clap.id`, `lv2.uri`, `$comment` | derived (recipe `derived`), refused if changed | — |
| `binding` | `instance`: the binding, the oracle, both faces and the Makefile are generated | — |
| `params` | one per `resolve` argument, in its order, each BY REFERENCE (`ref`, `field`), `symbol` = the control's name | — |
| each parameter's `name` | the control's words, its unit suffix dropped (`attackTimeMs` → "Attack Time") | reviews |
| a choice's `values` | the set's labels when the contract has them, else `REVIEW` | writes the labels |
| `description` | `REVIEW` | one plain sentence a host shows |
| `lv2.class`, `clap.features` | `REVIEW`, and `audio-effect`, `stereo` | the LV2 class and the CLAP kind |
| `summary`, `manual` | left out: the README row reads `description`, the section is the parameter table | optional prose |
| `panel` | one section of every parameter | groups the controls into sections: the one panel the MOD GUI and the console draw |
| `console` | its `placement`'s group as `REVIEW` | the placement (its `strips` only when the plugin confines itself to some strip kinds), and the card and chip the console draws |
| `portHints` | written after the first generation: the pin of the hints the generated TTL carries (`make hints` holds every later TTL to it) | — |

Step 3 refuses while any `REVIEW` mark is left, naming each by its JSON pointer. When the draft is
settled it fills the derived fields and runs `tools/gen.mjs`, which writes:

- per plugin: `generated/` (parameter header, LV2 bundle, MOD GUI from the `panel`), the
  `Makefile`, `omx_<kernel>_clap.c`, `omx_<kernel>_lv2.c`, the binding `omx_<kernel>_core.h` and
  the kernel-identity test `test/<kernel>-oracle.c`, each with a GENERATED banner;
- across plugins, from the folders: the README's catalogue and sections, the RPM `%files` lists and
  CI's installed-file lists, each between `BEGIN GENERATED` and `END GENERATED` lines.

The settled declaration is then written as the plugin's two hand-written files, as every
single-kernel plugin of ours is described (docs/declaration-as-descriptor.md): `<stem>.descriptor.json`,
its `authored` descriptor, rendered from the declaration, and `<stem>.decl.json`, the declaration's
extras (each parameter's own words, such as a set's `values` or a band's `defaultBand`, kept under
`controls` by symbol). The faces and every generated file are the same bytes from either form; the
plugin's Makefile hands omx-panel the descriptor. From then on the two files are the plugin: the wizard
drafts a new plugin only.

`node tools/gen.mjs --check` holds every one of them; CI runs it. Then the wizard runs the plugin's
`make test` and `make completeness` for it, and prints the commit plan in the recipe's layer order.

The wizard writes no factory presets. A shipped plugin carries them (`docs/presets.md`): until a
person declares them in `tools/gen-presets.mjs` and generates them, the plugin's `make test` is red
on its presets check.

## The identity test

One template, filled from the declaration. The plan has three parts. First, a deterministic stimulus
in uneven blocks, with every parameter moving between blocks across its declared travel and the
bypass toggled. Second, at the defaults, a full-scale burst followed by a tail that falls onto a quiet
bed. The tail lasts the sum of every time travel's default (at least 100 ms), so a dynamics stage
engages and releases, and a gate holds and closes. Third, for every choice or toggle, each of its
values in turn over its own burst and two tails: the other parameters stay at their defaults for the
first tail and are stepped once for the second. A control that only one mode reads is therefore
reached. The second and third parts are timed in milliseconds, so every rate gets the same time. The CLAP and LV2
builds must equal omx-dsp's instance face called directly, bit for bit, at every rate in
`OMX_DECLARED_RATES`, and publish its latency. A keyed face runs the plan twice, with a key that
differs from the main signal and with none, each block 64 times in a row so a detector has time to
release through every threshold; the key must change the output. Two guards keep it honest: the reference must move
the signal, and a reference rendered with one parameter one step off must differ from the faces
(the sabotage arm), so a test that could not see a wrong coefficient fails. A step is the next value of a
choice or a toggle, 1 % of a travel, 5 % of a time travel (ms, s: a slow detector's pole at 176.4 kHz lies
within 1e-5 of 1, where 1 % of a release moves it by about two float steps), an integer travel's share in
whole steps, one at least.

A parameter whose contract control declares `rearms` is held at its default in every block. Such a
control re-arms the kernel's state when it changes, so it is not a smooth parameter: the limiter's
look-ahead, for example, rebuilds the rings and restarts the gain at unity. Moving it between blocks
would test the re-arm, not the identity. The flag is read from omx-contract's JSON render, where
`kernels.<kernel>.controls` lists each kernel's controls in declared order. The oracle's header comment names each held parameter, and the sabotage arm still moves it
by one step. The other parameters move as before.

A parameter the declaration names in `monitors` replaces the output while it is on (a detector's
listen: the dynamic EQ's band listens, the compressor's key listen). The plan holds it at its default in
the moving blocks and when it steps the other parameters, so what it would replace stays heard; it still
gets its own section, each of its values over a burst and two tails, and the sabotage arm still moves it.

A declaration's `engaged` names values the plan holds in its release and choice sections instead of the
defaults, never stepped there, as a chain's element switches are held on: the multiband's band thresholds
at -40 dB, so every band is driven past its threshold, and its release heard, at every rate (white noise
at 176.4 kHz puts a thousandth of its power under 200 Hz, below a -20 dB default threshold).

A declaration's `stretch` runs each block of the plan that many times in a row, for a kernel that walks its gain or
its bands at dB per second (the loudness leveler, the spectral balancer): the test then has seconds of signal in which a
control is heard. Without it a block runs once, and a keyed face 64 times.
A declaration's `liveStretch` likewise lengthens the liveness test (1.5 s, times that number).
A declaration's `sabotageStep` gives the sabotage arm a larger step for a named parameter (the loudness leveler's gate, which its
meter decides on in blocks, is moved by 15 dB rather than its 1 % of travel).

## A control the plugin does not expose

A kernel release can add controls to a face a plugin already binds (omx-dsp 0.26.0's delay, gate and
reverb). Until the plugin takes them as parameters, its declaration names each in `fixed`, as a chain
element does: a set's id, a travel's number, `"default"` or `{"defaultRef": "<SHEET>.<field>"}`. The
binding passes the value, the identity test's reference passes the same, and no port, state word or
descriptor control is added; the binding's header comment names each one. At `"default"` the plugin is
the one it was: omx-dsp holds every added control's default bit-identical to the kernel without it.

## Oracles beside the identity test

The identity test proves the faces run omx-dsp's kernel; it does not say what the kernel does. When
an effect moves out of the console engine, the engine's own oracle comes with it: the declaration
names it in `oracles`, a list of names, and each `test/<name>.c` is written by hand (it is not
generated, and `tools/gen.mjs` refuses a name whose file is missing). The generated Makefile builds
each one as it builds the identity test and runs it in `make test-heavy` on both faces, as
`<name> clap <stem>.clap` and `<name> lv2 <bundle> <uri>`. Such an oracle opens the built faces as a
host does and reads only their audio, so an arm that read the kernel's state in the engine says what
of it a host can see. It runs at the nine rates an audio interface clocks at (`OMX_ORACLE_FULL_RATES`, 32 to
192 kHz), the rates omx-dsp's own kernel oracles run at.

## A plugin made of several kernels: a chain

`node tools/omx-new-plugin.mjs --from-contract <k1>,<k2>,... --stem omx-<x>` drafts a composite
(`"binding": "chain"`): one element per kernel, in the
order given, each element's parameters drafted as a single plugin's are and refused, naming the
omx-dsp work, for a kernel with no instance face of the generated shape. The declaration has no
`params`: the list is generated from `chain`, element by element, each element's switch `<id>On`
(its default the element's `on`) and then its parameters, then `order` when the chain says
`"order": "permutable"` (the permutations of the elements, lexicographic, 0 the declared order).

An element may declare `bands`, the key of its kernel's count sheet whose `max` is how many bands
it exposes (`strip`), `faceBands`, the key its face is compiled at where omx-dsp's face takes no
smaller count (`eq8`), and `fixed`, the controls of its kernel it does not expose, each at an id, a
number, `"default"` (refused for a travel that declares none) or `{"defaultRef": "<SHEET>.<field>"}`;
a per-band control fixed so covers every band the element does not expose, `"default"` giving each
its own default by the contract's one rule. An element with `bands` may declare `bandCount`
(`symbol`, `name`, `default`, `gates`): the plugin's own integer parameter, one band to the `max`
of its `bands`, coming up at the `default` of the sheet key `default` names; each band from the
count on is passed with its per-band control `gates` off, out of the face's bank, so it costs no
processing and the output is the output without it. Parameters an element gained after its plugin
shipped go in its `appended`: they, and its `bandCount` before them, come after every element's
`params` and the `order`, so no id and no LV2 parameter port the plugin had moves. omx-strip's EQ
exposes 32 band slots on the eq32 face, the first four in `params` and the rest appended, with
`eqBands` (EQ_BAND_COUNTS: up to `eq32`'s max, at first `strip`'s default) running the first of
them; its filter element is the eq face with every band fixed off; its gate, which takes no key in
a strip, has its key source fixed at `self`.

The generated binding copies in to out once and runs each element's face in place in the chosen
order; an element's bypass is its switch off or the host's bypass; the latency is the sum. The
oracle's reference is the same chain of faces called directly, its order computed in the test.
A chain converted from a released plugin names that release in `shipped`; `tools/gen.mjs` then
refuses a parameter list that differs from the release's, unless `shippedDiff` names every
difference; where the tag cannot be read (a tarball build) gen says the list went unchecked.
`make strip-crosscheck` (`tools/test/strip-crosscheck.sh`) holds omx-strip, so converted, to the hand-ported
plugin it replaced, bit for bit at that plugin's settings.

## What it does not do

Placement, the console's card and chip, and the one panel with its widgets are declared here (every
plugin declares `panel` and `console`) and checked against the vocabulary in
`schema/plugin.decl.schema.json`; the console's placement rules are
openmixer's and are applied there. The package descriptions are prose a person writes; the
completeness test checks that they name every shipped plugin.

## A test plugin

An instrument or probe that only a host's own tests create (the console's job proofs play a sine
voice, echo notes and read the transport a block was handed) is declared with `"role": "test"`
(schema `$defs.testPlugin`): its identity and the `constants` its face runs with and its oracle
predicts from. There is no wizard and no kernel: the CLAP face, the Makefile and the oracle with its
sabotage arms are written by hand, and `tools/gen.mjs` writes `generated/omx_<kernel>_decl.h` from
the declaration (`tools/test-plugins.mjs`).

A test plugin is linked into libomxplugins.a in a table of its own, `OMX_CLAP_STATIC_TEST_PLUGINS`:
`omx_clap_static_find()` and the one entry's `create_plugin()` open it by id, while
`OMX_CLAP_STATIC_PLUGINS`, the one entry's factory listing, the README, the packages' plugin lists,
the completeness recipe and `make install` never carry it, so no picker lists it and the release
gate, which probes every installed `.clap`, never sees it. `make test-role` holds that over a staged
install (`tools/test-role-check.sh`, with its self-test). `make install-test` installs it in
`/usr/libexec/omx-plugins/test`, outside every CLAP search path, for the `omx-plugins-test` package:
a host's tests and the plugin qualifier open it there by path. `make package-lists`
(`tools/package-lists-check.sh`) checks that every plugin the build makes is installed and listed by
its own package in the RPM spec and the Debian `.install` files, so a new test plugin cannot be built
and shipped by nothing.

Each test plugin's face says, in its header comment, what a test reads from it.
