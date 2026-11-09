# omx-hrp

The harmonic resonance processor as an LV2 effect, for any host (spec 7.7, 7.13, 7.16;
FreeMixer/omx-plugins#330). `omx_hrp_lv2.c` is a hand-written LV2 face over omx-dsp's live-shell
header, `omx_hrp_lv2.h` -- the first worker-threaded face in this tree, so it does not go through
the generated `lib/omx_lv2_face.c` path the other plugins share (that path has no worker support
yet).

Mono, one in and one out: the per-channel refusal spec 7.7/7.17 ask for. Report ports are
read-only: frequency, depth, Q and on, four per slot, `OMX_HRP_LV2_MAX_BANDS` slots, plus one
`analysing` output. `run()` is audio-thread only (feed the ring, apply the cascade); `work()` and
`work_response()` carry one analysis pass off it, per spec 7.13's audio/worker split.

## Building

The live-shell API (`omx_band_report.h`, `omx_live_shell.h`, `omx_hrp_lv2.h`) is in omx-dsp 0.32, the floor
`packaging/omx-plugins.spec` declares. `make test` builds against the installed omx-dsp (`pkg-config omxdsp`).

Everything a host reads is written by `tools/live-faces.mjs` and held by `tools/test/live-faces-test.mjs`: the
descriptor, the declared panel (the eq family, the report words as band roles, `hrp` as the origin, one exact
`eq_response`) and the TTLs. The LV2 URI is `urn:openmixer:hrp`.

## Not yet done

- No CLAP face: spec 7.13 says omx-hrp's CLAP face starts no thread until omx-clap-core's worker hook exists, so it ships
  LV2-only for now.
- Not gated by `omx-rt-gate` (omx-plugins#334), not qualified per class, rate and quantum, and no TSAN arm for the
  run()/work()/work_response() handoff.
- No oracle for the drawn curve within 0.5 dB of the measured response at 48 and 96 kHz.
- The marks of a plugin's own notches reach a surface through the host's `curve_points` reply, which carries no marks
  today: a plugin-hostd-protocol minor and a plugin-hostd change.
