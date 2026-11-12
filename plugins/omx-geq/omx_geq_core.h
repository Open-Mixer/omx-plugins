// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_geq_core.h — omx-geq's binding to omx-dsp's geq kernel: the ONE place both faces
 * (omx_geq_clap.c, omx_geq_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_geq_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-geq/omx-geq.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_GEQ_CORE_H
#define OMX_GEQ_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_geq_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_geq_instance.h>

_Static_assert(31 == OMX_GEQ_BANDS, "the declaration names one band parameter per band of the face");

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxGeqInstance inst;
} OmxGeqCore;

/** A fresh instance at `rate`: every state word zero. A rate the kernel does not declare leaves it a wire. */
static inline void omx_geq_core_init(OmxGeqCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_geq_instance_init(&c->inst, (double)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_geq_core_resolve(OmxGeqCore *c, const float *values, int bypass) {
  omx_geq_instance_resolve(&c->inst, bypass,
                                  /* band */ (const float[OMX_GEQ_BANDS]){values[OMX_GEQ_PARAM_BAND01], values[OMX_GEQ_PARAM_BAND02], values[OMX_GEQ_PARAM_BAND03], values[OMX_GEQ_PARAM_BAND04], values[OMX_GEQ_PARAM_BAND05], values[OMX_GEQ_PARAM_BAND06], values[OMX_GEQ_PARAM_BAND07], values[OMX_GEQ_PARAM_BAND08], values[OMX_GEQ_PARAM_BAND09], values[OMX_GEQ_PARAM_BAND10], values[OMX_GEQ_PARAM_BAND11], values[OMX_GEQ_PARAM_BAND12], values[OMX_GEQ_PARAM_BAND13], values[OMX_GEQ_PARAM_BAND14], values[OMX_GEQ_PARAM_BAND15], values[OMX_GEQ_PARAM_BAND16], values[OMX_GEQ_PARAM_BAND17], values[OMX_GEQ_PARAM_BAND18], values[OMX_GEQ_PARAM_BAND19], values[OMX_GEQ_PARAM_BAND20], values[OMX_GEQ_PARAM_BAND21], values[OMX_GEQ_PARAM_BAND22], values[OMX_GEQ_PARAM_BAND23], values[OMX_GEQ_PARAM_BAND24], values[OMX_GEQ_PARAM_BAND25], values[OMX_GEQ_PARAM_BAND26], values[OMX_GEQ_PARAM_BAND27], values[OMX_GEQ_PARAM_BAND28], values[OMX_GEQ_PARAM_BAND29], values[OMX_GEQ_PARAM_BAND30], values[OMX_GEQ_PARAM_BAND31]});
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_geq_core_latency(const OmxGeqCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(OMX_GEQ_INSTANCE_LATENCY_FRAMES));
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_geq_core_run(OmxGeqCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_geq_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_geq_core_init_v(void *c, uint32_t rate) { omx_geq_core_init((OmxGeqCore *)c, rate); }
static inline void omx_geq_core_resolve_v(void *c, const float *values, int bypass) {
  omx_geq_core_resolve((OmxGeqCore *)c, values, bypass);
}
static inline uint32_t omx_geq_core_latency_v(const void *c) { return omx_geq_core_latency((const OmxGeqCore *)c); }
static inline void omx_geq_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_geq_core_run((OmxGeqCore *)c, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_GEQ_CORE = {sizeof(OmxGeqCore), omx_geq_core_init_v, omx_geq_core_resolve_v,
                                                   omx_geq_core_latency_v, omx_geq_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_GEQ_CORE_H */
