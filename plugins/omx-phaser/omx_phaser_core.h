// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_phaser_core.h — omx-phaser's binding to omx-dsp's phaser kernel: the ONE place both faces
 * (omx_phaser_clap.c, omx_phaser_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_phaser_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-phaser/omx-phaser.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_PHASER_CORE_H
#define OMX_PHASER_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_phaser_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_phaser_instance.h>

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxPhaserInstance inst;
} OmxPhaserCore;

/** A fresh instance at `rate`: every state word zero. A rate the kernel does not declare leaves it a wire. */
static inline void omx_phaser_core_init(OmxPhaserCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_phaser_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_phaser_core_resolve(OmxPhaserCore *c, const float *values, int bypass) {
  omx_phaser_instance_resolve(&c->inst, bypass,
                                  /* rate_hz */ values[OMX_PHASER_PARAM_RATE_HZ],
                                  /* base_hz */ values[OMX_PHASER_PARAM_BASE_HZ],
                                  /* depth_oct */ values[OMX_PHASER_PARAM_DEPTH_OCT],
                                  /* stages */ values[OMX_PHASER_PARAM_STAGES],
                                  /* feedback */ values[OMX_PHASER_PARAM_FEEDBACK],
                                  /* mix */ values[OMX_PHASER_PARAM_MIX]);
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_phaser_core_latency(const OmxPhaserCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_phaser_instance_latency(&c->inst)));
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_phaser_core_run(OmxPhaserCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_phaser_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_phaser_core_init_v(void *c, uint32_t rate) { omx_phaser_core_init((OmxPhaserCore *)c, rate); }
static inline void omx_phaser_core_resolve_v(void *c, const float *values, int bypass) {
  omx_phaser_core_resolve((OmxPhaserCore *)c, values, bypass);
}
static inline uint32_t omx_phaser_core_latency_v(const void *c) { return omx_phaser_core_latency((const OmxPhaserCore *)c); }
static inline void omx_phaser_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_phaser_core_run((OmxPhaserCore *)c, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_PHASER_CORE = {sizeof(OmxPhaserCore), omx_phaser_core_init_v, omx_phaser_core_resolve_v,
                                                   omx_phaser_core_latency_v, omx_phaser_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_PHASER_CORE_H */
