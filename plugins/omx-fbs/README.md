# omx-fbs

The feedback corrector as an LV2 effect, for any host (spec 7.7, 7.13, 7.16). `omx_fbs_lv2.c` is a hand-written LV2 face
over omx-dsp's live-shell header, `omx_fbs_lv2.h`, as omx-hrp's is: the generated faces (`lib/omx_lv2_face.c`) have no
worker. It carries both pools, fixed (a true notch that never lifts) and live (a bell the staged release lifts).

Mono, one in and one out. Report ports are read-only outputs: frequency, depth, Q and on, four per slot, 24 slots, plus
one `analysing` output. `run()` is audio-thread only (the input into the ring, the planted bands on the audio in `cut`);
`work()` and `work_response()` carry one detector pass off it. A host with no worker gets the audio untouched, every slot
free and `analysing` 0.

Everything a host reads is written by `tools/live-faces.mjs` and held by `tools/test/live-faces-test.mjs`: the
descriptor, the declared panel (the eq family, the report words as band roles, `fbs-live` as the origin, one exact
`eq_response`) and the TTLs.

## Not yet done

- No CLAP face: the CLAP worker hook is omx-clap-core's, and the face ships LV2-only until it exists.
- The report carries four words per slot, so a surface draws every mark as `fbs-live`; telling a fixed-pool notch from a
  live bell needs the shell's `origin` and `shape` words on the wire (the host's `curve_points` reply, a
  plugin-hostd-protocol minor).
- Not gated by `omx-rt-gate` (omx-plugins#334), and not qualified per class, rate and quantum.
