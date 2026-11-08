// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_eq32_core.h — omx-eq32's binding to omx-dsp's eq32 kernel: the ONE place both faces
 * (omx_eq32_clap.c, omx_eq32_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_eq_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-eq32/omx-eq32.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_EQ32_CORE_H
#define OMX_EQ32_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_eq32_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_eq_instance.h>

_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one band_type parameter per band of the face");

_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one freq parameter per band of the face");

_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one gain parameter per band of the face");

_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one q parameter per band of the face");

_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one band_on parameter per band of the face");

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxEqInstance inst;
} OmxEq32Core;

/** A fresh instance at `rate`: every state word zero. A rate the kernel does not declare leaves it a wire. */
static inline void omx_eq32_core_init(OmxEq32Core *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_eq_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_eq32_core_resolve(OmxEq32Core *c, const float *values, int bypass) {
  omx_eq_instance_resolve(&c->inst, bypass,
                                  /* hpf_on */ (int)lrintf(values[OMX_EQ32_PARAM_HPF_ON]),
                                  /* hpf_freq */ values[OMX_EQ32_PARAM_HPF_FREQ],
                                  /* hpf_slope */ (int)lrintf(values[OMX_EQ32_PARAM_HPF_SLOPE]),
                                  /* lpf_on */ (int)lrintf(values[OMX_EQ32_PARAM_LPF_ON]),
                                  /* lpf_freq */ values[OMX_EQ32_PARAM_LPF_FREQ],
                                  /* lpf_slope */ (int)lrintf(values[OMX_EQ32_PARAM_LPF_SLOPE]),
                                  /* band_type */ (const int[OMX_EQ_INSTANCE_BANDS]){(int)lrintf(values[OMX_EQ32_PARAM_B1_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B2_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B3_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B4_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B5_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B6_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B7_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B8_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B9_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B10_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B11_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B12_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B13_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B14_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B15_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B16_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B17_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B18_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B19_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B20_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B21_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B22_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B23_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B24_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B25_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B26_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B27_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B28_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B29_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B30_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B31_TYPE]), (int)lrintf(values[OMX_EQ32_PARAM_B32_TYPE])},
                                  /* freq */ (const float[OMX_EQ_INSTANCE_BANDS]){values[OMX_EQ32_PARAM_B1_FREQ], values[OMX_EQ32_PARAM_B2_FREQ], values[OMX_EQ32_PARAM_B3_FREQ], values[OMX_EQ32_PARAM_B4_FREQ], values[OMX_EQ32_PARAM_B5_FREQ], values[OMX_EQ32_PARAM_B6_FREQ], values[OMX_EQ32_PARAM_B7_FREQ], values[OMX_EQ32_PARAM_B8_FREQ], values[OMX_EQ32_PARAM_B9_FREQ], values[OMX_EQ32_PARAM_B10_FREQ], values[OMX_EQ32_PARAM_B11_FREQ], values[OMX_EQ32_PARAM_B12_FREQ], values[OMX_EQ32_PARAM_B13_FREQ], values[OMX_EQ32_PARAM_B14_FREQ], values[OMX_EQ32_PARAM_B15_FREQ], values[OMX_EQ32_PARAM_B16_FREQ], values[OMX_EQ32_PARAM_B17_FREQ], values[OMX_EQ32_PARAM_B18_FREQ], values[OMX_EQ32_PARAM_B19_FREQ], values[OMX_EQ32_PARAM_B20_FREQ], values[OMX_EQ32_PARAM_B21_FREQ], values[OMX_EQ32_PARAM_B22_FREQ], values[OMX_EQ32_PARAM_B23_FREQ], values[OMX_EQ32_PARAM_B24_FREQ], values[OMX_EQ32_PARAM_B25_FREQ], values[OMX_EQ32_PARAM_B26_FREQ], values[OMX_EQ32_PARAM_B27_FREQ], values[OMX_EQ32_PARAM_B28_FREQ], values[OMX_EQ32_PARAM_B29_FREQ], values[OMX_EQ32_PARAM_B30_FREQ], values[OMX_EQ32_PARAM_B31_FREQ], values[OMX_EQ32_PARAM_B32_FREQ]},
                                  /* gain */ (const float[OMX_EQ_INSTANCE_BANDS]){values[OMX_EQ32_PARAM_B1_GAIN], values[OMX_EQ32_PARAM_B2_GAIN], values[OMX_EQ32_PARAM_B3_GAIN], values[OMX_EQ32_PARAM_B4_GAIN], values[OMX_EQ32_PARAM_B5_GAIN], values[OMX_EQ32_PARAM_B6_GAIN], values[OMX_EQ32_PARAM_B7_GAIN], values[OMX_EQ32_PARAM_B8_GAIN], values[OMX_EQ32_PARAM_B9_GAIN], values[OMX_EQ32_PARAM_B10_GAIN], values[OMX_EQ32_PARAM_B11_GAIN], values[OMX_EQ32_PARAM_B12_GAIN], values[OMX_EQ32_PARAM_B13_GAIN], values[OMX_EQ32_PARAM_B14_GAIN], values[OMX_EQ32_PARAM_B15_GAIN], values[OMX_EQ32_PARAM_B16_GAIN], values[OMX_EQ32_PARAM_B17_GAIN], values[OMX_EQ32_PARAM_B18_GAIN], values[OMX_EQ32_PARAM_B19_GAIN], values[OMX_EQ32_PARAM_B20_GAIN], values[OMX_EQ32_PARAM_B21_GAIN], values[OMX_EQ32_PARAM_B22_GAIN], values[OMX_EQ32_PARAM_B23_GAIN], values[OMX_EQ32_PARAM_B24_GAIN], values[OMX_EQ32_PARAM_B25_GAIN], values[OMX_EQ32_PARAM_B26_GAIN], values[OMX_EQ32_PARAM_B27_GAIN], values[OMX_EQ32_PARAM_B28_GAIN], values[OMX_EQ32_PARAM_B29_GAIN], values[OMX_EQ32_PARAM_B30_GAIN], values[OMX_EQ32_PARAM_B31_GAIN], values[OMX_EQ32_PARAM_B32_GAIN]},
                                  /* q */ (const float[OMX_EQ_INSTANCE_BANDS]){values[OMX_EQ32_PARAM_B1_Q], values[OMX_EQ32_PARAM_B2_Q], values[OMX_EQ32_PARAM_B3_Q], values[OMX_EQ32_PARAM_B4_Q], values[OMX_EQ32_PARAM_B5_Q], values[OMX_EQ32_PARAM_B6_Q], values[OMX_EQ32_PARAM_B7_Q], values[OMX_EQ32_PARAM_B8_Q], values[OMX_EQ32_PARAM_B9_Q], values[OMX_EQ32_PARAM_B10_Q], values[OMX_EQ32_PARAM_B11_Q], values[OMX_EQ32_PARAM_B12_Q], values[OMX_EQ32_PARAM_B13_Q], values[OMX_EQ32_PARAM_B14_Q], values[OMX_EQ32_PARAM_B15_Q], values[OMX_EQ32_PARAM_B16_Q], values[OMX_EQ32_PARAM_B17_Q], values[OMX_EQ32_PARAM_B18_Q], values[OMX_EQ32_PARAM_B19_Q], values[OMX_EQ32_PARAM_B20_Q], values[OMX_EQ32_PARAM_B21_Q], values[OMX_EQ32_PARAM_B22_Q], values[OMX_EQ32_PARAM_B23_Q], values[OMX_EQ32_PARAM_B24_Q], values[OMX_EQ32_PARAM_B25_Q], values[OMX_EQ32_PARAM_B26_Q], values[OMX_EQ32_PARAM_B27_Q], values[OMX_EQ32_PARAM_B28_Q], values[OMX_EQ32_PARAM_B29_Q], values[OMX_EQ32_PARAM_B30_Q], values[OMX_EQ32_PARAM_B31_Q], values[OMX_EQ32_PARAM_B32_Q]},
                                  /* band_on */ (const int[OMX_EQ_INSTANCE_BANDS]){(int)lrintf(values[OMX_EQ32_PARAM_B1_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B2_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B3_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B4_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B5_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B6_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B7_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B8_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B9_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B10_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B11_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B12_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B13_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B14_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B15_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B16_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B17_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B18_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B19_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B20_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B21_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B22_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B23_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B24_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B25_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B26_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B27_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B28_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B29_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B30_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B31_ON]), (int)lrintf(values[OMX_EQ32_PARAM_B32_ON])});
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_eq32_core_latency(const OmxEq32Core *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_eq_instance_latency(&c->inst)));
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_eq32_core_run(OmxEq32Core *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_eq_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_eq32_core_init_v(void *c, uint32_t rate) { omx_eq32_core_init((OmxEq32Core *)c, rate); }
static inline void omx_eq32_core_resolve_v(void *c, const float *values, int bypass) {
  omx_eq32_core_resolve((OmxEq32Core *)c, values, bypass);
}
static inline uint32_t omx_eq32_core_latency_v(const void *c) { return omx_eq32_core_latency((const OmxEq32Core *)c); }
static inline void omx_eq32_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_eq32_core_run((OmxEq32Core *)c, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_EQ32_CORE = {sizeof(OmxEq32Core), omx_eq32_core_init_v, omx_eq32_core_resolve_v,
                                                   omx_eq32_core_latency_v, omx_eq32_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_EQ32_CORE_H */
