<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com> -->
# A plugin's meters

A dynamics plugin that replaces a console stage owes the console the stage's gain-reduction meter, and a console
draws a plugin's input and output levels beside it. This is how a plugin here reports them.

## The interface

`clap.meters/1`, the CLAP meters extension (free-audio/clap#521), and nothing else. It is the one
omx-clap reads for meters since it removed `org.openmixer.meters/1` (omx-clap
`docs/meters.md`): every channel of every meter becomes an output symbol, read at
the host's idle rate through `get_values()`.

The LV2 face writes the same values to output control ports, one per channel, after the `latency` port: each
port's symbol is the one omx-clap derives from the CLAP meter's name (the meter's `id` for one channel,
`<id>_<channel>` from 0 for more), its range the meter's display range. So a host reads `band2Gr` by the same
symbol from either face. The plugin's descriptor lists the meters (omx-contract's `meters` word, schema 3.9.0):
the id, the symbol, the kind, the channels, the range, and the kernel readout each publishes, linked as
`data/kernels/<kernel>.json#readouts[<name>]`. A console learns from the descriptor which readouts a linked
plugin carries. A meter is never also a read-only parameter: CLAP has a word for meters, and omx-clap
refuses a meter symbol a parameter answers to.

No released CLAP SDK ships `clap/ext/draft/meters.h` yet. omx-clap installs its declaration as
`<omx-clap-host/clap_meters.h>` (it includes the SDK's header instead once the SDK has it), and the
plugins include that one. `tools/test/meters-abi-test.c`, run by `make test`, holds its id, enums,
sizes and offsets.

## The declaration

A plugin declares its meters in its declaration, `meters`, one entry each:

- `id`: the meter's symbol, also its stable `clap_meter_info.name` (the host derives the output
  symbol from the name, so the two agree). Its CLAP id is its index in the list: append-only, as
  the parameters are.
- `kind`: `gain_reduction` or `level`.

A `gain_reduction` (one channel: one detector drives both legs) names:

- `field`: where the kernel's own reading is, in its instance face's state
  (`state.band.gr_min_db` for the de-esser). The face reads it and nothing else: no meter is
  computed twice. Or, where the reading is a function of the face (a band's gain computer re-run on
  its detector's level, omx-dsp's readout),
- `read`: the face's reading, `omx_<kernel>_instance_<read>`, with its band index when it takes one
  (`band_gr_db(0)` for the multiband's first band). The generator refuses a reading the face's header
  does not declare as `float omx_<kernel>_instance_<read>(const <Instance> *[, uint32_t])`.
- `linear`: `true` when the `field` holds the block's deepest gain as a linear factor, not in dB (the
  limiter's `state.blk_gain`): the face reads it through omx-dsp's `omx_gr_block_db()`, the conversion
  every dynamics state's own `gr_min_db` is made with.
- `readout`: the kernel's readout in omx-contract (`GAIN_REDUCTION_DB`), whose declared range is
  the display range (a gain reduction always names one): the face reads `OMX_<KERNEL>_READOUT_<readout>_MIN` and `_MAX`, so the
  number is never typed here.

A `level` (two channels, left and right, in dBFS) names:

- `source`: `input` or `output`, where the face measures it.
- `min`, `max`: the display range the plugin suggests. A block quieter than `min`, silence
  included, reads `min`.

The kernels keep no level of their own, so the face measures one: the largest magnitude of each
leg's samples over the block, at the inputs before the kernel runs (the outputs may be the same
buffers) and at the outputs after it ran, as `20 log10(peak)`. That is one pass over each buffer
per block, and only a plugin that declares a level pays it.

A level a kernel does read, its detector's (the compressor's `keyLevelDb`, what the detector hears),
is one channel and names `read` and `readout` instead: the face reads it as it reads a gain
reduction, over the readout's declared range, and publishes the range's minimum while bypassed.

The de-esser declares its gain reduction (`deessGr`, id 0), its input level (`deessIn`, id 1) and
its output level (`deessOut`, id 2); the compressor its gain reduction (`compGr`, over the comp
kernel's `GAIN_REDUCTION_DB` readout), `compIn` and `compOut`. Every level runs from -60 to +6
dBFS. The keyed gate declares its gain reduction (`gateGr`) and the limiter its own (`limiterGr`, read
from the block's smallest linear gain); neither declares a level. The graphic EQ declares no meter:
its kernel's one readout, `DESIGN_CLAMPED`, is 1 only in the `true` design mode, which the plugin
does not expose yet (FX-14, #148), so today it would read 0 for ever.

## What the face reports

- **A gain reduction is the kernel's.** A gain reduction is dB with CLAP's sign: 0 none, negative a
  reduction, the deepest gain the kernel applied inside the block (omx-dsp's `gr_min_db`, what
  the console's own stage publishes). The console holds and releases it for display, as it does
  for its own stages; the plugin does not.
- **Per block, and coherent.** After each `process()` the audio thread publishes the values of
  every meter at once, through `include/omx_meter_ring.h`, and every meter carries
  `CLAP_METER_IS_COHERENT`: one `get_values()` never mixes two blocks, so a host may derive a
  balance from a level's two legs. (Two calls, for two meters, may still read two blocks.)
- **The ring.** The recipe the draft's header documents: three copies of the values and one
  atomic word naming the current one. The audio thread fills a copy no reader holds and then
  stores the word; `get_values()` loads the word, copies, and loads the word again, keeping the
  copy only when the audio thread published at most once in between. The header proves that this
  is enough: the copy a reader holds is refilled only after two more publications. A reader that
  was preempted across two blocks copies again, at most four times, and then returns 0 rather
  than a torn copy. No allocation and no lock on either side, and both finish in a bounded number
  of steps.
- **A bypassed gain reduction is 0.** A bypassed kernel leaves its state alone, so its last reading
  is stale; the face reports 0, as the console's disabled stage does. A level is measured on the
  buffers whatever the bypass, so a bypassed plugin's output level is its input's.
- **Nothing before the first block.** After activation, until a block is processed,
  `get_values()` writes nothing and returns 0.
- `channel_count` is 1 for a gain reduction and a level the kernel reads, 2 for a level the face
  measures, and every meter is coherent.

## The tests

Both faces compute one block's values in one place, `include/omx_face_meters.h`: the CLAP face publishes them,
the LV2 face writes them to its ports as floats.

The identity test (`test/<kernel>-oracle.c`) reads every meter after every block of its plan
through the CLAP face's clap.meters/1 and the LV2 face's output control ports (the reference as a float there),
at every declared rate, and holds every channel to a reference it computes
from the same block: a gain reduction to the same instance face, run directly on the same blocks,
read at the declared `field` (0 when bypassed); a level to its own peak of the block's input, or of
the reference's output, in dBFS and floored at `min`. Bit for bit. The meter list must be the
declaration's, every meter coherent. Guards: the plan must make the kernel reduce gain somewhere,
the two legs' levels must differ somewhere, so must the input and output levels, and the face must
write nothing before its first block. A wrong sign, swapped legs, an output level read at the
input, or a value one block stale is red.

`make meter-ring` (`tools/meter-ring-check.sh`) holds the ring itself: an audio thread publishing
every 3 µs, then with no pause, against readers that outnumber the cores two to one, so they are
preempted mid-copy and lapped. Every value of a publication is its own number, so a read that
mixes two blocks shows two numbers, and a read older than the same reader's last shows a smaller
one; either fails the run. Its self-test breaks the header three ways, each of which must go red:
two copies instead of three, no second load of the word, and the word stored before the copy is
filled.

The readouts' values are held by hand-written oracles on both faces, at the nine oracle rates, against the audio
they describe (`tools/test/face-driver.h` drives a built face and reads a meter by its symbol):

- `omx-dyneq` `dyneq-readout`: a bell at the tone with dynamics Down at threshold -60 and ratio 20 reads its range
  (-6, then -9) as band2Gain and cuts the tone by the same, each within 0.05 dB; under the threshold it reads 0;
  a static -6 dB bell with dynamics off reads exactly 0 (the readout is the dynamics' gain); bypassed, 0.
- `omx-multiband` `mbcomp-readout`: a -40 dBFS RMS tone at band 2's centre, threshold -60, ratio 20 and a symmetric
  detector reads -(L - T)(1 - 1/R) at the band's soloed level L within 0.05 dB (-18.84 dB) as band2Gr, and the
  soloed band is held down by what it reads; the other bands read exactly 0; bypassed, 0.
- `omx-comp` `comp-key`: compKey and compGr after a keyed run, now on the LV2 face too.
