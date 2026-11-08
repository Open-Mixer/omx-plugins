// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_glue_core.h — omx-glue's binding to omx-dsp's glue kernel: the ONE place both faces
 * (omx_glue_clap.c, omx_glue_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_glue_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-glue/omx-glue.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_GLUE_CORE_H
#define OMX_GLUE_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_glue_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_glue_instance.h>

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxGlueInstance inst;
} OmxGlueCore;

/** A fresh instance at `rate`: every state word zero. A rate the kernel does not declare leaves it a wire. */
static inline void omx_glue_core_init(OmxGlueCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_glue_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_glue_core_resolve(OmxGlueCore *c, const float *values, int bypass) {
  omx_glue_instance_resolve(&c->inst, bypass,
                                  /* threshold_db */ values[OMX_GLUE_PARAM_THRESHOLD_DB],
                                  /* ratio */ (int)lrintf(values[OMX_GLUE_PARAM_RATIO]),
                                  /* attack */ (int)lrintf(values[OMX_GLUE_PARAM_ATTACK]),
                                  /* release */ (int)lrintf(values[OMX_GLUE_PARAM_RELEASE]),
                                  /* makeup_db */ values[OMX_GLUE_PARAM_MAKEUP_DB],
                                  /* key_hp_hz */ values[OMX_GLUE_PARAM_KEY_HP_HZ],
                                  /* mix_pct */ values[OMX_GLUE_PARAM_MIX_PCT],
                                  /* detector_topology */ (int)lrintf(values[OMX_GLUE_PARAM_DETECTOR_TOPOLOGY]));
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_glue_core_latency(const OmxGlueCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_glue_instance_latency(&c->inst)));
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_glue_core_run(OmxGlueCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_glue_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_glue_core_init_v(void *c, uint32_t rate) { omx_glue_core_init((OmxGlueCore *)c, rate); }
static inline void omx_glue_core_resolve_v(void *c, const float *values, int bypass) {
  omx_glue_core_resolve((OmxGlueCore *)c, values, bypass);
}
static inline uint32_t omx_glue_core_latency_v(const void *c) { return omx_glue_core_latency((const OmxGlueCore *)c); }
static inline void omx_glue_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_glue_core_run((OmxGlueCore *)c, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_GLUE_CORE = {sizeof(OmxGlueCore), omx_glue_core_init_v, omx_glue_core_resolve_v,
                                                   omx_glue_core_latency_v, omx_glue_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_GLUE_CORE_H */
