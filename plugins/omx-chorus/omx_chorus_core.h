// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_chorus_core.h — omx-chorus's binding to omx-dsp's chorus kernel: the ONE place both faces
 * (omx_chorus_clap.c, omx_chorus_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_chorus_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-chorus/omx-chorus.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_CHORUS_CORE_H
#define OMX_CHORUS_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_chorus_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_chorus_instance.h>

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxChorusInstance inst;
} OmxChorusCore;

/** A fresh instance at `rate`: every state word zero. A rate the kernel does not declare leaves it a wire. */
static inline void omx_chorus_core_init(OmxChorusCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_chorus_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_chorus_core_resolve(OmxChorusCore *c, const float *values, int bypass) {
  omx_chorus_instance_resolve(&c->inst, bypass,
                                  /* spread */ values[OMX_CHORUS_PARAM_SPREAD],
                                  /* rate */ values[OMX_CHORUS_PARAM_RATE],
                                  /* depth */ values[OMX_CHORUS_PARAM_DEPTH],
                                  /* voices */ values[OMX_CHORUS_PARAM_VOICES],
                                  /* mix */ values[OMX_CHORUS_PARAM_MIX]);
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_chorus_core_latency(const OmxChorusCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(OMX_CHORUS_INSTANCE_LATENCY_FRAMES));
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_chorus_core_run(OmxChorusCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_chorus_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_chorus_core_init_v(void *c, uint32_t rate) { omx_chorus_core_init((OmxChorusCore *)c, rate); }
static inline void omx_chorus_core_resolve_v(void *c, const float *values, int bypass) {
  omx_chorus_core_resolve((OmxChorusCore *)c, values, bypass);
}
static inline uint32_t omx_chorus_core_latency_v(const void *c) { return omx_chorus_core_latency((const OmxChorusCore *)c); }
static inline void omx_chorus_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_chorus_core_run((OmxChorusCore *)c, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_CHORUS_CORE = {sizeof(OmxChorusCore), omx_chorus_core_init_v, omx_chorus_core_resolve_v,
                                                   omx_chorus_core_latency_v, omx_chorus_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_CHORUS_CORE_H */
