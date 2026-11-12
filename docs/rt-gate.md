<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com> -->
# The real-time gate

`omx-rt-gate` (omx-dsp-devel, spec 7.13) reads the real-time translation units of a face the way the compiler
sees them and walks every call from the roots it derives from the descriptor's tables. `tools/rt-gate.sh` runs it
over every face the tree builds: the units on the `.clap` and `.so` rules of each plugin's Makefile, with the shared
face (`lib/omx_clap_face.c`, `lib/omx_lv2_face.c`).

```sh
make rt-gate                  # every LV2 face
make rt-gate RT_GATE_FACES=all  # and the CLAP faces
make rt-gate-test             # planted violations and the guards broken one at a time
```

Exit 0: every root walked, nothing red. 1: red. 2: nothing judged (the gate missing, a unit that does not exist,
a face that derives no root). Never a pass in place of a judgement.

## What the faces do for the gate

- `OMX_LV2_FACE` names the fields of `LV2_Descriptor`: the gate derives its roots from the functions an
  initializer stores into them, and reads a positional initializer nested in the plugin's struct as no store.
- A core without a meter reading stores `omx_face_no_meter`, and a face without a tempo stores
  `omx_face_no_tempo`, so a call through the field has a target the gate can walk. `omx_face_reads_tempo()` asks
  whether the plugin reads one.
- The strip's band count rounds with `lrintf`, a call the class table names, where `rintf` was not.

## What waits

The CLAP faces are red and the red is not this tree's to waive:

- `request_callback` and `request_restart` (called from `process`) have no role in omx-dsp's `clap.abi`.
- `reset` reaches the core's `init`, which omx-dsp tags `@thread control`; and the gate joins a call through the
  core's `init` field to every function stored in any field named `init`, so `reset` appears to reach `get_extension`
  (`strcmp`) and the preset provider's `init` (`declare_*`).
- `fmod` is not in the class table (the two test voices).
- The LV2 faces start no worker. A face with one is held by the TSAN arm (run and work on two threads), which
  this tree has nothing to run until one exists.
