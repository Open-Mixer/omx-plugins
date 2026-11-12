#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# tools/test/rt-gate-test.sh — tools/rt-gate.sh (omx-rt-gate over every face, spec 7.13) held by planted violations.
# Each arm runs on a scratch copy of the shared faces and two plugins (omx-comp, which reads no tempo, and omx-delay,
# which does): red first with the violation planted, then the copy restored and read green. A guard is broken in the
# copy and a test must go red (the arm says which line), so a guard that holds nothing cannot pass.
#
#   tools/test/rt-gate-test.sh        needs omx-rt-gate (omx-dsp-devel) and the build deps of the faces
set -u
HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd) || exit 2
command -v "${OMX_RT_GATE:-omx-rt-gate}" >/dev/null 2>&1 || { echo "rt-gate-test: omx-rt-gate is not installed: install omx-dsp-devel" >&2; exit 2; }
W=$(mktemp -d) || exit 2
trap 'rm -rf "$W"' EXIT
fail=0
fresh() { # a clean copy in $W/t
  rm -rf "$W/t" && mkdir -p "$W/t/plugins" "$W/t/tools" || exit 2
  cp -r "$HERE/lib" "$HERE/include" "$W/t/" && cp "$HERE/tools/rt-gate.sh" "$W/t/tools/" || exit 2
  for p in omx-comp omx-delay; do mkdir -p "$W/t/plugins/$p" && cp -r "$HERE/plugins/$p/Makefile" "$HERE/plugins/$p/generated" "$HERE/plugins/$p"/*.c "$HERE/plugins/$p"/*.h "$W/t/plugins/$p/"; done
}
gate() { (cd "$W/t" && tools/rt-gate.sh --faces "${1:-lv2}" --out "$W/out" "${@:2}" 2>&1); }
# expect <name> <exit> <pattern-or-empty> -- <command...>
arm() {
  local name=$1 want=$2 pat=$3 out rc; shift 3
  out=$("$@"); rc=$?
  if [ "$rc" -ne "$want" ]; then echo "FAIL $name: exit $rc, wanted $want"; printf '%s\n' "$out" | tail -5 | sed 's/^/    /'; fail=1; return; fi
  if [ -n "$pat" ] && ! grep -qE -- "$pat" <<<"$out"; then echo "FAIL $name: no line matches /$pat/"; printf '%s\n' "$out" | tail -5 | sed 's/^/    /'; fail=1; return; fi
  echo "PASS $name"
}

# 0. the tree's faces, whole, are green: the positive control the arms below depart from
fresh; arm "control: both plugins' LV2 faces are green" 0 'rt-gate: 2 faces, 0 red' gate lv2

# 1. a malloc planted in the shared run is red, and green again restored
fresh; sed -i 's|^  omx_denormals_off();$|  omx_denormals_off(); free(malloc(8));|' "$W/t/lib/omx_lv2_face.c"
arm "planted malloc in the LV2 run is RT-ALLOC" 1 'RT-ALLOC malloc at .*omx_lv2_face.c:[0-9]+:[0-9]+ via omx_lv2_face_run\[audio\]' gate lv2
fresh; arm "malloc removed: green" 0 '' gate lv2

# 2. a sleep planted in a plugin's core vtable (one call below run) is red
fresh; sed -i 's|^\(  omx_comp_core_run((OmxCompCore \*)c.*\)$|  usleep(1);\n\1|' "$W/t/plugins/omx-comp/omx_comp_core.h"
sed -i '1i #include <unistd.h>' "$W/t/plugins/omx-comp/omx_comp_core.h"
arm "planted usleep in a plugin core is RT-BLOCK" 1 'RT-BLOCK usleep .*omx_comp_core_run_v' gate lv2
fresh; arm "usleep removed: green" 0 '' gate lv2

# 3. the same malloc in the CLAP process is red too (the CLAP face is judged by the same gate)
fresh; sed -i 's|^  omx_denormals_off();$|  omx_denormals_off(); free(malloc(8));|' "$W/t/lib/omx_clap_face.c"
arm "planted malloc in the CLAP process is RT-ALLOC" 1 'RT-ALLOC malloc at .*omx_clap_face.c:[0-9]+:[0-9]+ via g_process\[audio\]' gate clap omx-comp

# 4. guard: the descriptor's fields are named, so the gate derives the roots. A positional initializer derives none: not judged
fresh; python3 - "$W/t/include/omx_lv2_face.h" <<'PY'
import sys,re
p=sys.argv[1]; s=open(p).read()
a=s.index("#define OMX_LV2_FACE("); b=s.index("/* The port indices")
s=s[:a]+"#define OMX_LV2_FACE(face_, uri_) {{uri_, omx_lv2_face_instantiate, omx_lv2_face_connect_port, omx_lv2_face_activate, omx_lv2_face_run, NULL, omx_lv2_face_cleanup, omx_lv2_face_extension_data}, face_}\n\n"+s[b:]
open(p,"w").write(s)
PY
arm "sabotage: positional descriptor derives no root (exit 2, not a pass)" 2 'NOT-JUDGED no root' gate lv2

# 5. guard: a face without a tempo stores omx_face_no_tempo; NULL leaves an indirect call with no target
fresh; sed -i 's|^    .tempo = omx_face_no_tempo,$||' "$W/t/plugins/omx-comp/omx_comp_lv2.c"
arm "sabotage: no tempo target is RT-INDIRECT" 1 'RT-INDIRECT s->face->tempo' gate lv2 omx-comp
fresh; arm "tempo target restored: green" 0 '' gate lv2 omx-comp

# 6. a worker face: work() holds the audio role's forbidden set on its thread, and its result crosses by value
fresh; cat >> "$W/t/lib/omx_lv2_face.c" <<'WORKER'
#include <lv2/worker/worker.h>
#include <stdlib.h>
static LV2_Worker_Status w_work(LV2_Handle h, LV2_Worker_Respond_Function respond, LV2_Worker_Respond_Handle rh, uint32_t size, const void *data) {
  (void)h; (void)size; (void)data; float *tmp = malloc(64); free(tmp); uint32_t r = 1u; return respond(rh, sizeof r, &r);
}
static LV2_Worker_Status w_resp(LV2_Handle h, uint32_t size, const void *body) { (void)size; ((Face *)h)->in_l = (const float *)body; return LV2_WORKER_SUCCESS; }
static LV2_Worker_Status w_end(LV2_Handle h) { (void)h; return LV2_WORKER_SUCCESS; }
static const LV2_Worker_Interface W = {.work = w_work, .work_response = w_resp, .end_run = w_end};
const void *omx_lv2_face_worker(void) { return &W; }
WORKER
arm "planted worker: work() allocates is RT-ALLOC [worker]" 1 'RT-ALLOC malloc at .*omx_lv2_face.c.* via w_work\[worker\]' gate lv2 omx-comp
arm "planted worker: the body pointer kept is RT-RESULT" 1 'RT-RESULT .*w_resp' gate lv2 omx-comp

# 7. the driver's own guards: nothing judged is never a pass
fresh; arm "the gate not installed is exit 2" 2 'is not installed' bash -c 'OMX_RT_GATE=/nonexistent/omx-rt-gate "$0" --out "$1" 2>&1' "$W/t/tools/rt-gate.sh" "$W/out"
fresh; rm "$W/t/plugins/omx-comp/omx_comp_lv2.c"
arm "a unit on the Makefile's rule that does not exist is exit 2" 2 'does not exist' gate lv2 omx-comp
fresh; arm "a plugin that is not in the tree is exit 2 (no face found)" 2 'no face found' gate lv2 omx-nothing

[ "$fail" -eq 0 ] && echo "rt-gate-test: ok" || { echo "rt-gate-test: FAILED"; exit 1; }
