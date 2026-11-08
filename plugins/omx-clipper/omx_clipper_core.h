// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_clipper_core.h — omx-clipper's binding to omx-dsp's clipper kernel: the ONE place both faces
 * (omx_clipper_clap.c, omx_clipper_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_clipper_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-clipper/omx-clipper.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_CLIPPER_CORE_H
#define OMX_CLIPPER_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_clipper_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_clipper_instance.h>

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxClipperInstance inst;
} OmxClipperCore;

/** A fresh instance at `rate`: every state word zero. A rate the kernel does not declare leaves it a wire. */
static inline void omx_clipper_core_init(OmxClipperCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_clipper_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_clipper_core_resolve(OmxClipperCore *c, const float *values, int bypass) {
  omx_clipper_instance_resolve(&c->inst, bypass,
                                  /* drive_db */ values[OMX_CLIPPER_PARAM_DRIVE_DB],
                                  /* ceiling_db */ values[OMX_CLIPPER_PARAM_CEILING_DB],
                                  /* softness_pct */ values[OMX_CLIPPER_PARAM_SOFTNESS_PCT]);
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_clipper_core_latency(const OmxClipperCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_clipper_instance_latency(&c->inst)));
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_clipper_core_run(OmxClipperCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_clipper_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_clipper_core_init_v(void *c, uint32_t rate) { omx_clipper_core_init((OmxClipperCore *)c, rate); }
static inline void omx_clipper_core_resolve_v(void *c, const float *values, int bypass) {
  omx_clipper_core_resolve((OmxClipperCore *)c, values, bypass);
}
static inline uint32_t omx_clipper_core_latency_v(const void *c) { return omx_clipper_core_latency((const OmxClipperCore *)c); }
static inline void omx_clipper_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_clipper_core_run((OmxClipperCore *)c, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_CLIPPER_CORE = {sizeof(OmxClipperCore), omx_clipper_core_init_v, omx_clipper_core_resolve_v,
                                                   omx_clipper_core_latency_v, omx_clipper_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_CLIPPER_CORE_H */
