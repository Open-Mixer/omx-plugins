// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_gate_core.h — omx-keyed-gate's binding to omx-dsp's gate kernel: the ONE place both faces
 * (omx_gate_clap.c, omx_gate_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_gate_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-keyed-gate/omx-keyed-gate.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 * `key_listen` is no parameter: the declaration holds it at its kernel's declared default, OMX_GATE_KEY_LISTENS_DEFAULT.
 * `key_tilt_db` is no parameter: the declaration holds it at its kernel's declared default, OMX_GATE_KEY_TILT_DB_RANGE_DEFAULT.
 */
#ifndef OMX_GATE_CORE_H
#define OMX_GATE_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_gate_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_gate_instance.h>

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxGateInstance inst;
} OmxGateCore;

/** A fresh instance at `rate`: every state word zero. 1 when the kernel takes the rate, 0 when it refuses
 * it (the instance is then the identity: the caller must not run it). */
static inline int omx_gate_core_init(OmxGateCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  return omx_gate_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_gate_core_resolve(OmxGateCore *c, const float *values, int bypass) {
  omx_gate_instance_resolve(&c->inst, bypass,
                                  /* key_source */ (int)lrintf(values[OMX_GATE_PARAM_KEY_EXTERNAL]),
                                  /* threshold_db */ values[OMX_GATE_PARAM_THRESHOLD],
                                  /* range_db */ values[OMX_GATE_PARAM_RANGE],
                                  /* knee_start_db */ values[OMX_GATE_PARAM_KNEE_START],
                                  /* knee_end_db */ values[OMX_GATE_PARAM_KNEE_END],
                                  /* attack_ms */ values[OMX_GATE_PARAM_ATTACK],
                                  /* hold_ms */ values[OMX_GATE_PARAM_HOLD],
                                  /* release_ms */ values[OMX_GATE_PARAM_RELEASE],
                                  /* hysteresis_db */ values[OMX_GATE_PARAM_HYSTERESIS],
                                  /* ratio */ values[OMX_GATE_PARAM_RATIO],
                                  /* hp_hz */ values[OMX_GATE_PARAM_KEY_HP],
                                  /* lp_hz */ values[OMX_GATE_PARAM_KEY_LP],
                                  /* key_listen */ (int)OMX_GATE_KEY_LISTENS_DEFAULT,
                                  /* key_tilt_db */ (float)OMX_GATE_KEY_TILT_DB_RANGE_DEFAULT);
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_gate_core_latency(const OmxGateCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_gate_instance_latency(&c->inst)));
}

/** Gain-reduction meter `i`'s reading (the declaration's `meters`, in order): the kernel's own, after the
 * last block. */
static inline float omx_gate_core_meter(const OmxGateCore *c, uint32_t i) {
  switch (i) {
  case 0:
    return c->inst.gate.st.gr_min_db;
  default:
    return 0.0f;
  }
}

/** One block, stereo, keyed by `key` (NULL: nothing routed to the key, the kernel's own detector);
 * the outputs may alias the inputs or the key. */
static inline void omx_gate_core_run(OmxGateCore *c, const float *key, const float *in_l, const float *in_r,
                                         float *out_l, float *out_r, uint32_t frames) {
  omx_gate_instance_run(&c->inst, key, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline int omx_gate_core_init_v(void *c, uint32_t rate) { return omx_gate_core_init((OmxGateCore *)c, rate); }
static inline void omx_gate_core_resolve_v(void *c, const float *values, int bypass) {
  omx_gate_core_resolve((OmxGateCore *)c, values, bypass);
}
static inline uint32_t omx_gate_core_latency_v(const void *c) { return omx_gate_core_latency((const OmxGateCore *)c); }
static inline void omx_gate_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  omx_gate_core_run((OmxGateCore *)c, key, in_l, in_r, out_l, out_r, frames);
}
static inline float omx_gate_core_meter_v(const void *c, uint32_t i) { return omx_gate_core_meter((const OmxGateCore *)c, i); }

static const struct omx_face_core OMX_GATE_CORE = {sizeof(OmxGateCore), omx_gate_core_init_v, omx_gate_core_resolve_v,
                                                   omx_gate_core_latency_v, omx_gate_core_run_v,
                                                   omx_gate_core_meter_v};

#endif /* OMX_GATE_CORE_H */
