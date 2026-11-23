// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_dyneq_core.h — omx-dyneq's binding to omx-dsp's dyneq kernel: the ONE place both faces
 * (omx_dyneq_clap.c, omx_dyneq_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_dyneq_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-dyneq/omx-dyneq.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_DYNEQ_CORE_H
#define OMX_DYNEQ_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_dyneq_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_dyneq_instance.h>

_Static_assert(6 == OMX_DYNEQ_INSTANCE_BANDS, "the declaration names one band_on parameter per band of the face");

_Static_assert(6 == OMX_DYNEQ_INSTANCE_BANDS, "the declaration names one band_type parameter per band of the face");

_Static_assert(6 == OMX_DYNEQ_INSTANCE_BANDS, "the declaration names one freq parameter per band of the face");

_Static_assert(6 == OMX_DYNEQ_INSTANCE_BANDS, "the declaration names one gain parameter per band of the face");

_Static_assert(6 == OMX_DYNEQ_INSTANCE_BANDS, "the declaration names one q parameter per band of the face");

_Static_assert(6 == OMX_DYNEQ_INSTANCE_BANDS, "the declaration names one dyn_mode parameter per band of the face");

_Static_assert(6 == OMX_DYNEQ_INSTANCE_BANDS, "the declaration names one threshold parameter per band of the face");

_Static_assert(6 == OMX_DYNEQ_INSTANCE_BANDS, "the declaration names one ratio parameter per band of the face");

_Static_assert(6 == OMX_DYNEQ_INSTANCE_BANDS, "the declaration names one range parameter per band of the face");

_Static_assert(6 == OMX_DYNEQ_INSTANCE_BANDS, "the declaration names one attack parameter per band of the face");

_Static_assert(6 == OMX_DYNEQ_INSTANCE_BANDS, "the declaration names one release parameter per band of the face");

_Static_assert(6 == OMX_DYNEQ_INSTANCE_BANDS, "the declaration names one listen parameter per band of the face");

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxDyneqInstance inst;
} OmxDyneqCore;

/** A fresh instance at `rate`: every state word zero. 1 when the kernel takes the rate, 0 when it refuses
 * it (the instance is then the identity: the caller must not run it). */
static inline int omx_dyneq_core_init(OmxDyneqCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  return omx_dyneq_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_dyneq_core_resolve(OmxDyneqCore *c, const float *values, int bypass) {
  omx_dyneq_instance_resolve(&c->inst, bypass,
                                  /* band_on */ (const int[OMX_DYNEQ_INSTANCE_BANDS]){(int)lrintf(values[OMX_DYNEQ_PARAM_BAND_ON1]), (int)lrintf(values[OMX_DYNEQ_PARAM_BAND_ON2]), (int)lrintf(values[OMX_DYNEQ_PARAM_BAND_ON3]), (int)lrintf(values[OMX_DYNEQ_PARAM_BAND_ON4]), (int)lrintf(values[OMX_DYNEQ_PARAM_BAND_ON5]), (int)lrintf(values[OMX_DYNEQ_PARAM_BAND_ON6])},
                                  /* band_type */ (const int[OMX_DYNEQ_INSTANCE_BANDS]){(int)lrintf(values[OMX_DYNEQ_PARAM_BAND_TYPE1]), (int)lrintf(values[OMX_DYNEQ_PARAM_BAND_TYPE2]), (int)lrintf(values[OMX_DYNEQ_PARAM_BAND_TYPE3]), (int)lrintf(values[OMX_DYNEQ_PARAM_BAND_TYPE4]), (int)lrintf(values[OMX_DYNEQ_PARAM_BAND_TYPE5]), (int)lrintf(values[OMX_DYNEQ_PARAM_BAND_TYPE6])},
                                  /* freq */ (const float[OMX_DYNEQ_INSTANCE_BANDS]){values[OMX_DYNEQ_PARAM_FREQ1], values[OMX_DYNEQ_PARAM_FREQ2], values[OMX_DYNEQ_PARAM_FREQ3], values[OMX_DYNEQ_PARAM_FREQ4], values[OMX_DYNEQ_PARAM_FREQ5], values[OMX_DYNEQ_PARAM_FREQ6]},
                                  /* gain */ (const float[OMX_DYNEQ_INSTANCE_BANDS]){values[OMX_DYNEQ_PARAM_GAIN1], values[OMX_DYNEQ_PARAM_GAIN2], values[OMX_DYNEQ_PARAM_GAIN3], values[OMX_DYNEQ_PARAM_GAIN4], values[OMX_DYNEQ_PARAM_GAIN5], values[OMX_DYNEQ_PARAM_GAIN6]},
                                  /* q */ (const float[OMX_DYNEQ_INSTANCE_BANDS]){values[OMX_DYNEQ_PARAM_Q1], values[OMX_DYNEQ_PARAM_Q2], values[OMX_DYNEQ_PARAM_Q3], values[OMX_DYNEQ_PARAM_Q4], values[OMX_DYNEQ_PARAM_Q5], values[OMX_DYNEQ_PARAM_Q6]},
                                  /* dyn_mode */ (const int[OMX_DYNEQ_INSTANCE_BANDS]){(int)lrintf(values[OMX_DYNEQ_PARAM_DYN_MODE1]), (int)lrintf(values[OMX_DYNEQ_PARAM_DYN_MODE2]), (int)lrintf(values[OMX_DYNEQ_PARAM_DYN_MODE3]), (int)lrintf(values[OMX_DYNEQ_PARAM_DYN_MODE4]), (int)lrintf(values[OMX_DYNEQ_PARAM_DYN_MODE5]), (int)lrintf(values[OMX_DYNEQ_PARAM_DYN_MODE6])},
                                  /* threshold */ (const float[OMX_DYNEQ_INSTANCE_BANDS]){values[OMX_DYNEQ_PARAM_THRESHOLD1], values[OMX_DYNEQ_PARAM_THRESHOLD2], values[OMX_DYNEQ_PARAM_THRESHOLD3], values[OMX_DYNEQ_PARAM_THRESHOLD4], values[OMX_DYNEQ_PARAM_THRESHOLD5], values[OMX_DYNEQ_PARAM_THRESHOLD6]},
                                  /* ratio */ (const float[OMX_DYNEQ_INSTANCE_BANDS]){values[OMX_DYNEQ_PARAM_RATIO1], values[OMX_DYNEQ_PARAM_RATIO2], values[OMX_DYNEQ_PARAM_RATIO3], values[OMX_DYNEQ_PARAM_RATIO4], values[OMX_DYNEQ_PARAM_RATIO5], values[OMX_DYNEQ_PARAM_RATIO6]},
                                  /* range */ (const float[OMX_DYNEQ_INSTANCE_BANDS]){values[OMX_DYNEQ_PARAM_RANGE1], values[OMX_DYNEQ_PARAM_RANGE2], values[OMX_DYNEQ_PARAM_RANGE3], values[OMX_DYNEQ_PARAM_RANGE4], values[OMX_DYNEQ_PARAM_RANGE5], values[OMX_DYNEQ_PARAM_RANGE6]},
                                  /* attack */ (const float[OMX_DYNEQ_INSTANCE_BANDS]){values[OMX_DYNEQ_PARAM_ATTACK1], values[OMX_DYNEQ_PARAM_ATTACK2], values[OMX_DYNEQ_PARAM_ATTACK3], values[OMX_DYNEQ_PARAM_ATTACK4], values[OMX_DYNEQ_PARAM_ATTACK5], values[OMX_DYNEQ_PARAM_ATTACK6]},
                                  /* release */ (const float[OMX_DYNEQ_INSTANCE_BANDS]){values[OMX_DYNEQ_PARAM_RELEASE1], values[OMX_DYNEQ_PARAM_RELEASE2], values[OMX_DYNEQ_PARAM_RELEASE3], values[OMX_DYNEQ_PARAM_RELEASE4], values[OMX_DYNEQ_PARAM_RELEASE5], values[OMX_DYNEQ_PARAM_RELEASE6]},
                                  /* listen */ (const int[OMX_DYNEQ_INSTANCE_BANDS]){(int)lrintf(values[OMX_DYNEQ_PARAM_LISTEN1]), (int)lrintf(values[OMX_DYNEQ_PARAM_LISTEN2]), (int)lrintf(values[OMX_DYNEQ_PARAM_LISTEN3]), (int)lrintf(values[OMX_DYNEQ_PARAM_LISTEN4]), (int)lrintf(values[OMX_DYNEQ_PARAM_LISTEN5]), (int)lrintf(values[OMX_DYNEQ_PARAM_LISTEN6])},
                                  /* mix_pct */ values[OMX_DYNEQ_PARAM_MIX_PCT]);
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_dyneq_core_latency(const OmxDyneqCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_dyneq_instance_latency(&c->inst)));
}

/** Gain-reduction meter `i`'s reading (the declaration's `meters`, in order): the kernel's own, after the
 * last block. */
static inline float omx_dyneq_core_meter(const OmxDyneqCore *c, uint32_t i) {
  switch (i) {
  case 0:
    return omx_dyneq_instance_band_gain_db(&c->inst, 0u);
  case 1:
    return omx_dyneq_instance_band_gain_db(&c->inst, 1u);
  case 2:
    return omx_dyneq_instance_band_gain_db(&c->inst, 2u);
  case 3:
    return omx_dyneq_instance_band_gain_db(&c->inst, 3u);
  case 4:
    return omx_dyneq_instance_band_gain_db(&c->inst, 4u);
  case 5:
    return omx_dyneq_instance_band_gain_db(&c->inst, 5u);
  default:
    return 0.0f;
  }
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_dyneq_core_run(OmxDyneqCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_dyneq_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline int omx_dyneq_core_init_v(void *c, uint32_t rate) { return omx_dyneq_core_init((OmxDyneqCore *)c, rate); }
static inline void omx_dyneq_core_resolve_v(void *c, const float *values, int bypass) {
  omx_dyneq_core_resolve((OmxDyneqCore *)c, values, bypass);
}
static inline uint32_t omx_dyneq_core_latency_v(const void *c) { return omx_dyneq_core_latency((const OmxDyneqCore *)c); }
static inline void omx_dyneq_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_dyneq_core_run((OmxDyneqCore *)c, in_l, in_r, out_l, out_r, frames);
}
static inline float omx_dyneq_core_meter_v(const void *c, uint32_t i) { return omx_dyneq_core_meter((const OmxDyneqCore *)c, i); }

static const struct omx_face_core OMX_DYNEQ_CORE = {sizeof(OmxDyneqCore), omx_dyneq_core_init_v, omx_dyneq_core_resolve_v,
                                                   omx_dyneq_core_latency_v, omx_dyneq_core_run_v,
                                                   omx_dyneq_core_meter_v};

#endif /* OMX_DYNEQ_CORE_H */
