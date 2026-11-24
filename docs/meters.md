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
symbol from either face. The plugin's descriptor lists the meters (omx-contract's `meters` word, schema 3.9.0, with its `held`, `clip` and `measurement` words, 3.11.0):
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
  the console's own stage publishes). The plugin holds it until the host reads it: the value a read
  gives is the deepest since the last read. The reader recovers it for display, as the console does
  for its own stages.
- **A plugin holds until read.** Every meter keeps the block value until the host reads it: a level's
  largest, a gain reduction's deepest since the last read. The audio thread does this with one
  compare per value per block, beside the value, in the same copy of the ring, so a 10 ms burst
  between two reads is in the next read whatever the blocks after it were. A read takes the meter it
  reads, and one reader per instance does it; the host reads at its own tick (50 Hz, spec 7.9) and
  any thread but the audio role may. A block that lands between a reader's copy and its note of what
  it took is held once more, never dropped. A meter whose declaration says `held: false` gives the
  block's own value.
- **The clip word.** A level the face measures (at the inputs or the outputs) carries, on the same
  publication, the longest run of consecutive samples over full scale since the last read, per
  channel, unless its declaration says `clip: false`. It says digital over-full-scale, nothing of
  the analog stage. The meter flags `OMX_CLAP_METER_HAS_CLIP` (bit 9, omx-clap's
  `<omx-clap-host/clap_host.h>`) and, written as the host reads them, `get_values()` gives one more value after
  the channels when the host leaves room for it: 1 if a run of any channel ran since the last read,
  0 otherwise. A held meter also carries `OMX_CLAP_METER_IS_HELD` (bit 8). The LV2 face writes the
  block's values to its ports on each run and has no reader to hold for.
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
  `get_values()` writes nothing and returns 0. Deactivating ends every hold: nothing measured
  before it is read after the next activation.
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
filled. The held accumulator has its own proof there: a burst held until the take and gone at the
one after, a gain reduction's deepest, a clip word's longest run, a take that is not yet noted,
one meter taken at a time, and a clear that ends every hold; then one taker beside plain readers
that outnumber the cores, every burst between two takes in the later one. It is built under
UndefinedBehaviorSanitizer. Its arms (no hold, no restart, no deepest, no clip hold, no count, no
clear, a 32-bit hold mask) each go red.

`make meter-held` (`tools/meter-held-check.sh`) holds the same through a built face, omx-comp, at
each of omx-dsp's nine ORACLE_FULL_RATES and each quantum from 32 to 1024 frames: 20 ms of silence,
a 10 ms burst of 1.2 on both legs, 100 ms of silence, one read, 100 ms more, one read. A reference
instance read after every block gives each block's own values, and the held reads must be their
largest level and deepest gain reduction since the read before, bit for bit: +1.58 dBFS and a clip
word of 1 after the burst, the floor and 0 at the read after; a gain reduction that steps down with
the burst and falls back after it without deepening again. A face read before its first block, or
after a deactivation and a new activation, gives nothing. The same burst between two reads of
omx-clap's `omx_clap_host_meter_values`, the host's own call, reaches the later read
(`tools/test/meter-held-host.c`). Its self-test breaks the ring once per kind (no hold, no restart,
no deepest for the gain reduction, no clip hold for the clip word), each red.

The readouts' values are held by hand-written oracles on both faces, at the nine oracle rates, against the audio
they describe (`tools/test/face-driver.h` drives a built face and reads a meter by its symbol):

- `omx-dyneq` `dyneq-readout`: a bell at the tone with dynamics Down at threshold -60 and ratio 20 reads its range
  (-6, then -9) as band2Gain and cuts the tone by the same, each within 0.05 dB; under the threshold it reads 0;
  a static -6 dB bell with dynamics off reads exactly 0 (the readout is the dynamics' gain); bypassed, 0.
- `omx-multiband` `mbcomp-readout`: a -40 dBFS RMS tone at band 2's centre, threshold -60, ratio 20 and a symmetric
  detector reads -(L - T)(1 - 1/R) at the band's soloed level L within 0.05 dB (-18.84 dB) as band2Gr, and the
  soloed band is held down by what it reads; the other bands read exactly 0; bypassed, 0.
- `omx-comp` `comp-key`: compKey and compGr after a keyed run, now on the LV2 face too.
