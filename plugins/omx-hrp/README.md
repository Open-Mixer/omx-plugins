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

## Not yet done

- No CLAP face: spec 7.13 says omx-hrp's CLAP face starts no thread until omx-clap-core's worker
  hook exists, so it ships LV2-only for now.
- No descriptor.json / decl.json wired into tools/gen.mjs (the generator has no notion yet of a
  read-only report control bound to an eq band role, origin or `eq_response` display -- spec
  7.7's panel/description side).
- Not gated by `omx-rt-gate` yet (built from the same branch, not run against this face yet).
- No TSAN arm for the run()/work()/work_response() handoff yet.
- No oracle for the drawn curve within 0.5 dB of the measured response at 48/96 kHz yet.
- The LV2 URI (`urn:openmixer:hrp`) follows this repo's `urn:openmixer:<name>` convention; spec
  7.7 says omx-hrp "keeps its URI" from wherever it ran before the move, which this has not been
  checked against.
