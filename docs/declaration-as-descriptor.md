<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com> -->
# The declaration as a view of the descriptor

A plugin of ours is described in omx-contract's words, like anyone's (omx-contract `docs/descriptors.md`):
`plugins/<stem>/<stem>.descriptor.json` is its `authored` descriptor. `plugins/<stem>/<stem>.decl.json` keeps only
what the descriptor has no word for and only this repository needs: how the faces are built, the presets'
source, the panel's hints, the docs and the console's placement. `schema/plugin.decl.schema.json` is the shape
of the declaration a generator reads, the descriptor's view with those extras folded in; for a described plugin
the extras file carries only the extras, and `viewOf` (`tools/descriptor.mjs`) refuses a field the descriptor
says (the identity, the parameters, the CLAP features, the LV2 class, the description).

`tools/gen.mjs`'s `loadDecl` reads the two and gives every generator the declaration it gave before, so the
headers, both faces, the panel description, the MOD GUI and its rasters do not change by a byte when a
plugin moves to a descriptor (`node tools/gen.mjs --check`, `omx-panel modgui --check`, `make panel-layout`).
The panel is omx-panel's: it lays a panel out from a descriptor (its `docs/panel-layout.md`) and writes the MOD GUI
(its `docs/modgui.md`), and reads a descriptor, never a declaration. This repository renders a declaration to its
descriptor (`panelDescription(d)` lays that descriptor out under the declared panel) and hands omx-panel the
descriptor and the declared panel as a file.

Two plugins keep a whole declaration: omx-eq, a base of three variants, and omx-strip, a chain. Their declaration is
no extras file, so their descriptors are rendered from it (`node tools/gen.mjs` writes `omx-eq8`, `omx-eq16`,
`omx-eq32` and `omx-strip` `.descriptor.json`, `--check` holds them fresh) and are not read back. Their faces state
what the descriptor states, as every described plugin's do: the CLAP parameter modules from the groups, the LV2
designations from the meanings.

A link carries the release it is read from, `at`: `omx-contract@<floor>`, the packaging's floor.

## Where a host reads it

`make install-devel` (through `make install-contract`) installs each shipped plugin's descriptor, byte for byte,
into omx-contract's contract directory as `<contractdir>/3/omx-plugins/plugins/<stem>.json`, with a `MANIFEST.json`
naming the release (omx-contract `docs/contract-dir.md`); the -devel and -dev packages carry it. A host reads it
there through omx-contract's reader, never from this tree. `make contract-dir` holds the installed tree to the
plugin folders' descriptors, one per shipped plugin that has one, and reads it back through the reader.

## No number is typed

A control's travel is still read by reference. Its `source.from` is the link into the kernel's definition,
`data/kernels/<kernel>.json#<NAME>` (a field: `#<NAME>.<field>`), or into omx-contract's shared files
(`data/primitives.json#<NAME>`). The descriptor states the travel the link resolves to, because the
descriptor words always state it; `node tools/gen.mjs` writes those numbers from the link, and
`node tools/gen.mjs --check` refuses a descriptor whose stated travel, default, unit or choice is not what its
link resolves to. A number edited by hand is stale, never a declaration.

## Where each declaration field goes

| Declaration field | Descriptor word | Or the extra, in `<stem>.decl.json` |
|---|---|---|
| `name`, `vendor`, `version` | `plugin.name`, `.vendor`, `.version` | |
| `clap.id` | `plugin.id` (`format: clap`) | |
| `lv2.uri` | `plugin.uri` | |
| `clap.features` | `plugin.features`: the four the descriptor names (`audio-effect`, `instrument`, `note-effect`, `analyzer`); CLAP's other features (`delay`, `stereo`, ...) in the extension bag, `extensions.clap["clap_plugin_descriptor.features"]`, in the face's order, each as a scan's field dump writes it: a feature the CLAP headers define by its macro name (`CLAP_PLUGIN_FEATURE_DELAY`, omx-contract's `CLAP_PLUGIN_FEATURES_TERMS`), one feature as it is, two or more a list | |
| (every LV2 face) `lv2:optionalFeature lv2:hardRTCapable` | `needs.extensions.optional: ["lv2:hardRTCapable"]` | |
| `lv2.class` | the extension bag, `extensions.lv2["rdf:type"]`: the face's types as its `.ttl` states them, `["lv2:Plugin", <class>]` (`"lv2:Plugin"` alone when that is the class) | |
| `description` | `plugin.doc`: the one-line description, CLAP `description` and LV2 `rdfs:comment` | |
| `summary`, `noun`, `manual`, `url` | | kept: what a host and the README show |
| `stem`, `kernel`, `kernels`, `binding`, `face`, `variants`, `variantOf`, `chain`, `order`, `key`, `shipped`, `shippedDiff`, `role` | | kept: the kind of face and how it is built |
| `sidechain` (`symbol`, `name`) | a `ports[]` entry: `kind: audio`, `direction: in`, `role: sidechain`, `channels: 1`, `layout: mono`, named as the CLAP face names it (the symbol) | `sidechain`, whole: the LV2 port's symbol and name |
| the audio pair | `ports[]`: `in` and `out` (the CLAP face's names), `role: main`, `channels: 2`, `layout: stereo` | |
| `latencyChecks` | `latency.reports: true`; `latency.frames` when every check reports one constant | kept: the checks the oracles run |
| `meters` | `meters[]`: `id` its index, `symbol` its id, `name` from it, `kind` (`gain-reduction`, `level`), `channels`, the range, unit and scale (the kernel readout's, linked in `source.from` as `data/kernels/<kernel>.json#readouts[<name>]`; a level's as declared, `dBFS`), the readout's `doc` | kept: where the face reads it (`field`, `read`, `linear`, `source`) |
| `oracles`, `portHints` | | kept: tests, the LV2 hint pin |
| `panel.sections[].label` | each listed control's `group` | `panel` (family, roles, section keys, order, widgets, graphs): the panel's hints |
| `panel.sections[].subgroups` (`band`, a declaration with variants) | each band's controls' `group`, the path `[<section label>, "Band <n>"]`: the panel section first, the LV2 port group last (#257) | the panel is handed to omx-panel without it: the section is drawn as declared |
| `panel.roles` | each bound control's `meaning`, where the role has a meaning word (omx-panel docs/panel-layout.md) | kept: the roles themselves |
| `console` | | kept: the console's placement, card and chip |
| `params[i]` | `controls[i]`, `id: i` | |
| `params[].symbol` | `controls[].symbol` | |
| `params[].name` | `controls[].name` | |
| `params[].ref`, `.field`, `.kernel` | `controls[].source.from`: `data/kernels/<kernel>.json#<ref>[.<field>]` | |
| resolved `min`, `max`, `def`, `unit`, `kind` | `kind` (`travel`, `toggle`, `choice`), `travel.min`, `.max`, `.default`, `.unit`, `.step` (1 for an integer), or a toggle's `default`; `.unit` is `ratio` where the link resolves to `""` and the control is a ratio (n:1), the one word the descriptor states beyond its link | |
| `params[].scale` | `travel.scale` (left out of the declaration: `log` for Hz, else `linear`) | |
| resolved `values`, `points` | a `choice`: `ids` (each value as text), `labels`, `values` | |
| `params[].forKind`, `.defaultRef`, `.defaultId`, `.defaultBand`, `.perBand`, `.values` | the come-up value and the labels they give, stated | kept, by symbol (`controls`): how the value is read, not the value |
| resolved `byKind` | | kept as the read (gap 2) |
| a set's `unit` (`dB/oct`) | | kept, by symbol (gap 1) |
| `params[].own` (a face's switch, a stage order) | the control, its travel typed, `source.impl` naming the face | `own`, the reason, by symbol |

## The gaps

What the descriptor has no word for, found laying out our panels, and where each goes. A gap marked
"contract" is proposed to omx-contract as a schema minor; until it lands the extra stays here.

| # | Gap | Goes to | Until then |
|---|---|---|---|
| 1 | A choice's unit: our EQ slopes, 12 and 24 `dB/oct` | contract: an optional `unit` on a choice, and `dB/oct` among the descriptor units | the set's unit, an extra by symbol |
| 2 | The come-up value per kind of channel (a delay's mix fully wet on a return): the kernel files say it as `byKind` | contract: `travel.byKind`, the kernel word, on a descriptor travel | read through the link at load |
| 3 | Meanings for a rotor's speeds and speed switch, an LFO's shape, a drive's model, a reverb's algorithm, a de-esser's mode, a filter or band type, a band's on switch, a ping-pong switch | contract: `speed`, `shape`, `model`, `algorithm`, `mode`, `type`, `on`, `pingpong` | the declared roles bind them |
| 4 | A ratio's unit: LV2 has none, the item list has only `x` | contract: `ratio` (n:1) among the descriptor units (omx-contract 3.0.0); the LV2 face writes it as `urn:openmixer:unit#ratio` | done: omx-comp `ratio`, omx-deesser `deessRatio`, omx-keyed-gate `ratio` state `ratio` |
| 5 | What a meter measures | contract: `meters[].kind` (`gain-reduction`, `level`, omx-contract 3.9.0); `peak`, `rms`, `vu`, `ppm`, `lufs` have no word yet | a meter is a `meters[]` entry, never a read-only control |
| 6 | `dBu`, which param-contract serves | contract: `dBu` among the descriptor units | the unit stays on our declarations only |
| 7 | An xy pad (two controls drawn as one) | omx-plugins: a panel hint, `panel.widgets` naming the pair | none of ours draws one |
| 8 | A momentary control | none: a `trigger` is momentary | |
| 9 | CLAP's open features (`delay`, `stereo`) and the LV2 class | contract: the extension bag, `extensions` (omx-contract 3.0.0) | done: each descriptor's bag carries them |
| 10 | The plugin's one-line description (CLAP `description`, LV2 `rdfs:comment`) | contract: `plugin.doc` (omx-contract 3.0.0) | done |

## The faces, from plugin-hostd's tables read backwards

One vocabulary, two directions (FreeMixer/omx-plugins#210, ruling B). plugin-hostd installs one mapping table per
format into omx-contract's contract directory, `<contractdir>/3/plugin-hostd/formats/{clap,lv2}.json`: how each field a
scan reads becomes a descriptor word. Our generators write a face from the authored descriptor by reading the same
rows the other way (`tools/format-tables.mjs`), and only the rows whose transform is `identity` or `lookup`: the row
whose `lookup` maps a source value to the word's value names what the face writes. No transform is code of ours.
`$PLUGIN_HOSTD_TABLES`, a directory holding `clap.json` and `lv2.json` (a plugin-hostd checkout's `data/formats`),
stands in for the installed tables.

- An LV2 port's `units:unit` is the LV2 row mapping its unit (`units:db` for `dB`, `<urn:openmixer:unit#dBFS>` for
  `dBFS`); a unit no row maps is the declaration's own, defined in the plugin's TTL: `dBu`, `dB/oct` and `ratio`
  (`urn:openmixer:unit#ratio`, gap 4).
- An LV2 port's `lv2:designation` is the LV2 row mapping its control's `meaning`: 186 ports carry a `param:` term
  (gain 93, frequency 63, wetDryRatio 11, release 5, threshold 4, ratio 4, attack 4, hold 1, delay 1). A meaning with
  no LV2 term is written nowhere.
- A CLAP parameter's `module` is its control's `group`, the labels joined by `/` (the inverse of plugin-hostd's
  `group-path`); a plugin with no descriptor writes `""`.
- param-contract's unit tokens (`tools/param-contract.mjs`) are the LV2 units a face writes, their values those
  omx-contract declares (the sheet `OMX_PARAM_CONTRACT_UNITS`, `{ <token>: <code> }`, in its `param-contract.json`) and
  renders to `omxcontract/omx_param_units.h`, which `include/omx_param_contract.h` includes; `make units` refuses a
  drift between them and a unit code defined in this tree.

`make format-tables` (part of `make test`) holds the faces to the tables, with its sabotage arms.

## The round trip

The authored descriptor is also what our built faces must serve, read back as plugin-hostd reads anyone's plugin
(ruling B: the proof of one vocabulary is the round trip). `make round-trip` (`tools/round-trip.{sh,mjs}`, part of
`make test-heavy`) takes every plugin with a descriptor, generates it (`node tools/gen.mjs`), builds both faces, runs
`plugin-hostd-probe describe` on each built face and compares the scanned descriptor with the authored one word by
word, one line each: `equal`, `lost <format> <word>` (the face does not carry it: no row of the format's table reads it
back, or the scan leaves it `unknown`), or `differs`. A lost word is printed by name, never filled in.

What a format states otherwise is named once in the tool: an LV2 plugin's id is its URI and an LV2 control's id its
symbol; an LV2 face's audio ports are one per channel, with the generator's own symbols and names; a choice's points
name each value by its label, so its default is compared as the value it designates; a CLAP face reads back the float
it stores; the face's own bypass (`CLAP_PARAM_IS_BYPASS`, `lv2:enabled`) stands beside the controls; a value no row
of the table gives a word (a CLAP control a `travel`, a CLAP port that is not main `aux`) is `describe`'s own, so
lost. The bags are equal: every field an authored bag holds, the scan holds with the same value, as the field dump
writes it (a CLAP feature by its macro name, `CLAP_PLUGIN_FEATURE_DELAY`; one value as it is, two or more a list).

It passes with no `differs`, every bag equal, and each lost word one the tool's `LOSSES` names, by count:

- OR2's: `meaning` on CLAP (294) and on LV2 (108, those with no LV2 term), the `""` unit on LV2 (82);
- the `ratio` unit on LV2 (14) and the `dB/oct` unit (1): plugin-hostd 0.7.0's LV2 table has no row for
  `urn:openmixer:unit#ratio` nor `urn:openmixer:unit#dBoct`;
- on CLAP, words no row reads: the unit (404), a linear or log scale (276), the step (404), a choice's kind, ids,
  values, labels and default (197, 97, 97, 97, 197), the symbol (601), the LV2 URI (21), a side-chain's role (2), what
  an LV2 host may lend (21);
- on LV2: the step (404), automation (601), the plugin's version and vendor (21 each), the CLAP id (601), our point
  ids (97), the audio ports' ids and names (44 each).

A break in plugin-hostd's wave tip is named in `UPSTREAM` and printed with its `differs`, not failing; the run fails
when one no longer shows, so the entry leaves with the fix. One stands: `describe` keeps a
word's last value and the LV2 dump lists `lv2:toggled` before `rdf:type lv2:ControlPort`, so the 100 LV2 toggles
(omx-delay `pingpong`, omx-drive `autoGain` and `stereoLink`, the band, filter and stage switches of omx-eq8, omx-eq16,
omx-eq32 and omx-strip, omx-comp's `keyListen` and `duck`, omx-dyneq's band switches and listens, omx-multiband's solos
and bypasses) scan as travels and lose their default.

The four folders with no descriptor (omx-eq, the base of the three EQ variants, and the three omx-test-*) are
named as outside the round trip. Its self-test breaks the first described plugin twice, one range in a copy of its
authored descriptor and one `lv2:maximum` in a copy of its built `.ttl`, and wants each red.

