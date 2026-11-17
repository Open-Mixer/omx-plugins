// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_deesser_core.h — omx-deesser's binding to omx-dsp's deesser kernel: the ONE place both faces
 * (omx_deesser_clap.c, omx_deesser_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_deesser_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-deesser/omx-deesser.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_DEESSER_CORE_H
#define OMX_DEESSER_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_deesser_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_deesser_instance.h>

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxDeesserInstance inst;
} OmxDeesserCore;

/** A fresh instance at `rate`: every state word zero. 1 when the kernel takes the rate, 0 when it refuses
 * it (the instance is then the identity: the caller must not run it). */
static inline int omx_deesser_core_init(OmxDeesserCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  return omx_deesser_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_deesser_core_resolve(OmxDeesserCore *c, const float *values, int bypass) {
  omx_deesser_instance_resolve(&c->inst, bypass,
                                  /* deess_freq */ values[OMX_DEESSER_PARAM_DEESS_FREQ],
                                  /* deess_width */ values[OMX_DEESSER_PARAM_DEESS_WIDTH],
                                  /* deess_threshold */ values[OMX_DEESSER_PARAM_DEESS_THRESHOLD],
                                  /* deess_ratio */ values[OMX_DEESSER_PARAM_DEESS_RATIO],
                                  /* deess_range */ values[OMX_DEESSER_PARAM_DEESS_RANGE],
                                  /* deess_attack */ values[OMX_DEESSER_PARAM_DEESS_ATTACK],
                                  /* deess_release */ values[OMX_DEESSER_PARAM_DEESS_RELEASE],
                                  /* deess_mode */ (int)lrintf(values[OMX_DEESSER_PARAM_DEESS_MODE]));
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_deesser_core_latency(const OmxDeesserCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_deesser_instance_latency(&c->inst)));
}

/** Gain-reduction meter `i`'s reading (the declaration's `meters`, in order): the kernel's own, after the
 * last block. */
static inline float omx_deesser_core_meter(const OmxDeesserCore *c, uint32_t i) {
  switch (i) {
  case 0:
    return c->inst.state.band.gr_min_db;
  default:
    return 0.0f;
  }
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_deesser_core_run(OmxDeesserCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_deesser_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline int omx_deesser_core_init_v(void *c, uint32_t rate) { return omx_deesser_core_init((OmxDeesserCore *)c, rate); }
static inline void omx_deesser_core_resolve_v(void *c, const float *values, int bypass) {
  omx_deesser_core_resolve((OmxDeesserCore *)c, values, bypass);
}
static inline uint32_t omx_deesser_core_latency_v(const void *c) { return omx_deesser_core_latency((const OmxDeesserCore *)c); }
static inline void omx_deesser_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_deesser_core_run((OmxDeesserCore *)c, in_l, in_r, out_l, out_r, frames);
}
static inline float omx_deesser_core_meter_v(const void *c, uint32_t i) { return omx_deesser_core_meter((const OmxDeesserCore *)c, i); }

static const struct omx_face_core OMX_DEESSER_CORE = {sizeof(OmxDeesserCore), omx_deesser_core_init_v, omx_deesser_core_resolve_v,
                                                   omx_deesser_core_latency_v, omx_deesser_core_run_v,
                                                   omx_deesser_core_meter_v};

#endif /* OMX_DEESSER_CORE_H */
