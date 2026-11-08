// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_plosive_core.h — omx-plosive's binding to omx-dsp's plosive kernel: the ONE place both faces
 * (omx_plosive_clap.c, omx_plosive_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_plosive_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-plosive/omx-plosive.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_PLOSIVE_CORE_H
#define OMX_PLOSIVE_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_plosive_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_plosive_instance.h>

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxPlosiveInstance inst;
} OmxPlosiveCore;

/** A fresh instance at `rate`: every state word zero. A rate the kernel does not declare leaves it a wire. */
static inline void omx_plosive_core_init(OmxPlosiveCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_plosive_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_plosive_core_resolve(OmxPlosiveCore *c, const float *values, int bypass) {
  omx_plosive_instance_resolve(&c->inst, bypass,
                                  /* consonant_db */ values[OMX_PLOSIVE_PARAM_CONSONANT_DB],
                                  /* sensitivity_db */ values[OMX_PLOSIVE_PARAM_SENSITIVITY_DB],
                                  /* recovery_ms */ values[OMX_PLOSIVE_PARAM_RECOVERY_MS],
                                  /* band_hz */ values[OMX_PLOSIVE_PARAM_BAND_HZ],
                                  /* band_width_oct */ values[OMX_PLOSIVE_PARAM_BAND_WIDTH_OCT],
                                  /* pop_db */ values[OMX_PLOSIVE_PARAM_POP_DB],
                                  /* pop_sensitivity_db */ values[OMX_PLOSIVE_PARAM_POP_SENSITIVITY_DB],
                                  /* pop_recovery_ms */ values[OMX_PLOSIVE_PARAM_POP_RECOVERY_MS],
                                  /* pop_hz */ values[OMX_PLOSIVE_PARAM_POP_HZ]);
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_plosive_core_latency(const OmxPlosiveCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_plosive_instance_latency(&c->inst)));
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_plosive_core_run(OmxPlosiveCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_plosive_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_plosive_core_init_v(void *c, uint32_t rate) { omx_plosive_core_init((OmxPlosiveCore *)c, rate); }
static inline void omx_plosive_core_resolve_v(void *c, const float *values, int bypass) {
  omx_plosive_core_resolve((OmxPlosiveCore *)c, values, bypass);
}
static inline uint32_t omx_plosive_core_latency_v(const void *c) { return omx_plosive_core_latency((const OmxPlosiveCore *)c); }
static inline void omx_plosive_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_plosive_core_run((OmxPlosiveCore *)c, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_PLOSIVE_CORE = {sizeof(OmxPlosiveCore), omx_plosive_core_init_v, omx_plosive_core_resolve_v,
                                                   omx_plosive_core_latency_v, omx_plosive_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_PLOSIVE_CORE_H */
