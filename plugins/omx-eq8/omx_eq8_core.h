// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_eq8_core.h — omx-eq8's binding to omx-dsp's eq8 kernel: the ONE place both faces
 * (omx_eq8_clap.c, omx_eq8_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_eq_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-eq8/omx-eq8.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_EQ8_CORE_H
#define OMX_EQ8_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_eq8_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_eq_instance.h>

_Static_assert(8 == OMX_EQ_INSTANCE_BANDS, "the declaration names one band_type parameter per band of the face");

_Static_assert(8 == OMX_EQ_INSTANCE_BANDS, "the declaration names one freq parameter per band of the face");

_Static_assert(8 == OMX_EQ_INSTANCE_BANDS, "the declaration names one gain parameter per band of the face");

_Static_assert(8 == OMX_EQ_INSTANCE_BANDS, "the declaration names one q parameter per band of the face");

_Static_assert(8 == OMX_EQ_INSTANCE_BANDS, "the declaration names one band_on parameter per band of the face");

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxEqInstance inst;
} OmxEq8Core;

/** A fresh instance at `rate`: every state word zero. A rate the kernel does not declare leaves it a wire. */
static inline void omx_eq8_core_init(OmxEq8Core *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_eq_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_eq8_core_resolve(OmxEq8Core *c, const float *values, int bypass) {
  omx_eq_instance_resolve(&c->inst, bypass,
                                  /* hpf_on */ (int)lrintf(values[OMX_EQ8_PARAM_HPF_ON]),
                                  /* hpf_freq */ values[OMX_EQ8_PARAM_HPF_FREQ],
                                  /* hpf_slope */ (int)lrintf(values[OMX_EQ8_PARAM_HPF_SLOPE]),
                                  /* lpf_on */ (int)lrintf(values[OMX_EQ8_PARAM_LPF_ON]),
                                  /* lpf_freq */ values[OMX_EQ8_PARAM_LPF_FREQ],
                                  /* lpf_slope */ (int)lrintf(values[OMX_EQ8_PARAM_LPF_SLOPE]),
                                  /* band_type */ (const int[OMX_EQ_INSTANCE_BANDS]){(int)lrintf(values[OMX_EQ8_PARAM_B1_TYPE]), (int)lrintf(values[OMX_EQ8_PARAM_B2_TYPE]), (int)lrintf(values[OMX_EQ8_PARAM_B3_TYPE]), (int)lrintf(values[OMX_EQ8_PARAM_B4_TYPE]), (int)lrintf(values[OMX_EQ8_PARAM_B5_TYPE]), (int)lrintf(values[OMX_EQ8_PARAM_B6_TYPE]), (int)lrintf(values[OMX_EQ8_PARAM_B7_TYPE]), (int)lrintf(values[OMX_EQ8_PARAM_B8_TYPE])},
                                  /* freq */ (const float[OMX_EQ_INSTANCE_BANDS]){values[OMX_EQ8_PARAM_B1_FREQ], values[OMX_EQ8_PARAM_B2_FREQ], values[OMX_EQ8_PARAM_B3_FREQ], values[OMX_EQ8_PARAM_B4_FREQ], values[OMX_EQ8_PARAM_B5_FREQ], values[OMX_EQ8_PARAM_B6_FREQ], values[OMX_EQ8_PARAM_B7_FREQ], values[OMX_EQ8_PARAM_B8_FREQ]},
                                  /* gain */ (const float[OMX_EQ_INSTANCE_BANDS]){values[OMX_EQ8_PARAM_B1_GAIN], values[OMX_EQ8_PARAM_B2_GAIN], values[OMX_EQ8_PARAM_B3_GAIN], values[OMX_EQ8_PARAM_B4_GAIN], values[OMX_EQ8_PARAM_B5_GAIN], values[OMX_EQ8_PARAM_B6_GAIN], values[OMX_EQ8_PARAM_B7_GAIN], values[OMX_EQ8_PARAM_B8_GAIN]},
                                  /* q */ (const float[OMX_EQ_INSTANCE_BANDS]){values[OMX_EQ8_PARAM_B1_Q], values[OMX_EQ8_PARAM_B2_Q], values[OMX_EQ8_PARAM_B3_Q], values[OMX_EQ8_PARAM_B4_Q], values[OMX_EQ8_PARAM_B5_Q], values[OMX_EQ8_PARAM_B6_Q], values[OMX_EQ8_PARAM_B7_Q], values[OMX_EQ8_PARAM_B8_Q]},
                                  /* band_on */ (const int[OMX_EQ_INSTANCE_BANDS]){(int)lrintf(values[OMX_EQ8_PARAM_B1_ON]), (int)lrintf(values[OMX_EQ8_PARAM_B2_ON]), (int)lrintf(values[OMX_EQ8_PARAM_B3_ON]), (int)lrintf(values[OMX_EQ8_PARAM_B4_ON]), (int)lrintf(values[OMX_EQ8_PARAM_B5_ON]), (int)lrintf(values[OMX_EQ8_PARAM_B6_ON]), (int)lrintf(values[OMX_EQ8_PARAM_B7_ON]), (int)lrintf(values[OMX_EQ8_PARAM_B8_ON])});
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_eq8_core_latency(const OmxEq8Core *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_eq_instance_latency(&c->inst)));
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_eq8_core_run(OmxEq8Core *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_eq_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_eq8_core_init_v(void *c, uint32_t rate) { omx_eq8_core_init((OmxEq8Core *)c, rate); }
static inline void omx_eq8_core_resolve_v(void *c, const float *values, int bypass) {
  omx_eq8_core_resolve((OmxEq8Core *)c, values, bypass);
}
static inline uint32_t omx_eq8_core_latency_v(const void *c) { return omx_eq8_core_latency((const OmxEq8Core *)c); }
static inline void omx_eq8_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_eq8_core_run((OmxEq8Core *)c, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_EQ8_CORE = {sizeof(OmxEq8Core), omx_eq8_core_init_v, omx_eq8_core_resolve_v,
                                                   omx_eq8_core_latency_v, omx_eq8_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_EQ8_CORE_H */
