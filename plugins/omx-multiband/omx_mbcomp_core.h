// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_mbcomp_core.h — omx-multiband's binding to omx-dsp's mbcomp kernel: the ONE place both faces
 * (omx_mbcomp_clap.c, omx_mbcomp_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_mbcomp_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-multiband/omx-multiband.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_MBCOMP_CORE_H
#define OMX_MBCOMP_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_mbcomp_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_mbcomp_instance.h>

_Static_assert(4 == OMX_MBC_MAX_XOVERS, "the declaration names one xover_hz parameter per band of the face");

_Static_assert(5 == OMX_MBC_MAX_BANDS, "the declaration names one threshold_db parameter per band of the face");

_Static_assert(5 == OMX_MBC_MAX_BANDS, "the declaration names one ratio parameter per band of the face");

_Static_assert(5 == OMX_MBC_MAX_BANDS, "the declaration names one attack_ms parameter per band of the face");

_Static_assert(5 == OMX_MBC_MAX_BANDS, "the declaration names one release_ms parameter per band of the face");

_Static_assert(5 == OMX_MBC_MAX_BANDS, "the declaration names one makeup_db parameter per band of the face");

_Static_assert(5 == OMX_MBC_MAX_BANDS, "the declaration names one solo parameter per band of the face");

_Static_assert(5 == OMX_MBC_MAX_BANDS, "the declaration names one band_bypass parameter per band of the face");

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxMbcompInstance inst;
} OmxMbcompCore;

/** A fresh instance at `rate`: every state word zero. 1 when the kernel takes the rate, 0 when it refuses
 * it (the instance is then the identity: the caller must not run it). */
static inline int omx_mbcomp_core_init(OmxMbcompCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  return omx_mbcomp_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_mbcomp_core_resolve(OmxMbcompCore *c, const float *values, int bypass) {
  omx_mbcomp_instance_resolve(&c->inst, bypass,
                                  /* bands */ values[OMX_MBCOMP_PARAM_BANDS],
                                  /* xover_hz */ (const float[OMX_MBC_MAX_XOVERS]){values[OMX_MBCOMP_PARAM_XOVER_HZ1], values[OMX_MBCOMP_PARAM_XOVER_HZ2], values[OMX_MBCOMP_PARAM_XOVER_HZ3], values[OMX_MBCOMP_PARAM_XOVER_HZ4]},
                                  /* threshold_db */ (const float[OMX_MBC_MAX_BANDS]){values[OMX_MBCOMP_PARAM_THRESHOLD_DB1], values[OMX_MBCOMP_PARAM_THRESHOLD_DB2], values[OMX_MBCOMP_PARAM_THRESHOLD_DB3], values[OMX_MBCOMP_PARAM_THRESHOLD_DB4], values[OMX_MBCOMP_PARAM_THRESHOLD_DB5]},
                                  /* ratio */ (const float[OMX_MBC_MAX_BANDS]){values[OMX_MBCOMP_PARAM_RATIO1], values[OMX_MBCOMP_PARAM_RATIO2], values[OMX_MBCOMP_PARAM_RATIO3], values[OMX_MBCOMP_PARAM_RATIO4], values[OMX_MBCOMP_PARAM_RATIO5]},
                                  /* attack_ms */ (const float[OMX_MBC_MAX_BANDS]){values[OMX_MBCOMP_PARAM_ATTACK_MS1], values[OMX_MBCOMP_PARAM_ATTACK_MS2], values[OMX_MBCOMP_PARAM_ATTACK_MS3], values[OMX_MBCOMP_PARAM_ATTACK_MS4], values[OMX_MBCOMP_PARAM_ATTACK_MS5]},
                                  /* release_ms */ (const float[OMX_MBC_MAX_BANDS]){values[OMX_MBCOMP_PARAM_RELEASE_MS1], values[OMX_MBCOMP_PARAM_RELEASE_MS2], values[OMX_MBCOMP_PARAM_RELEASE_MS3], values[OMX_MBCOMP_PARAM_RELEASE_MS4], values[OMX_MBCOMP_PARAM_RELEASE_MS5]},
                                  /* makeup_db */ (const float[OMX_MBC_MAX_BANDS]){values[OMX_MBCOMP_PARAM_MAKEUP_DB1], values[OMX_MBCOMP_PARAM_MAKEUP_DB2], values[OMX_MBCOMP_PARAM_MAKEUP_DB3], values[OMX_MBCOMP_PARAM_MAKEUP_DB4], values[OMX_MBCOMP_PARAM_MAKEUP_DB5]},
                                  /* solo */ (const int[OMX_MBC_MAX_BANDS]){(int)lrintf(values[OMX_MBCOMP_PARAM_SOLO1]), (int)lrintf(values[OMX_MBCOMP_PARAM_SOLO2]), (int)lrintf(values[OMX_MBCOMP_PARAM_SOLO3]), (int)lrintf(values[OMX_MBCOMP_PARAM_SOLO4]), (int)lrintf(values[OMX_MBCOMP_PARAM_SOLO5])},
                                  /* band_bypass */ (const int[OMX_MBC_MAX_BANDS]){(int)lrintf(values[OMX_MBCOMP_PARAM_BAND_BYPASS1]), (int)lrintf(values[OMX_MBCOMP_PARAM_BAND_BYPASS2]), (int)lrintf(values[OMX_MBCOMP_PARAM_BAND_BYPASS3]), (int)lrintf(values[OMX_MBCOMP_PARAM_BAND_BYPASS4]), (int)lrintf(values[OMX_MBCOMP_PARAM_BAND_BYPASS5])},
                                  /* mix_pct */ values[OMX_MBCOMP_PARAM_MIX_PCT],
                                  /* output_db */ values[OMX_MBCOMP_PARAM_OUTPUT_DB]);
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_mbcomp_core_latency(const OmxMbcompCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_mbcomp_instance_latency(&c->inst)));
}

/** Gain-reduction meter `i`'s reading (the declaration's `meters`, in order): the kernel's own, after the
 * last block. */
static inline float omx_mbcomp_core_meter(const OmxMbcompCore *c, uint32_t i) {
  switch (i) {
  case 0:
    return omx_mbcomp_instance_band_gr_db(&c->inst, 0u);
  case 1:
    return omx_mbcomp_instance_band_gr_db(&c->inst, 1u);
  case 2:
    return omx_mbcomp_instance_band_gr_db(&c->inst, 2u);
  case 3:
    return omx_mbcomp_instance_band_gr_db(&c->inst, 3u);
  case 4:
    return omx_mbcomp_instance_band_gr_db(&c->inst, 4u);
  default:
    return 0.0f;
  }
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_mbcomp_core_run(OmxMbcompCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_mbcomp_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline int omx_mbcomp_core_init_v(void *c, uint32_t rate) { return omx_mbcomp_core_init((OmxMbcompCore *)c, rate); }
static inline void omx_mbcomp_core_resolve_v(void *c, const float *values, int bypass) {
  omx_mbcomp_core_resolve((OmxMbcompCore *)c, values, bypass);
}
static inline uint32_t omx_mbcomp_core_latency_v(const void *c) { return omx_mbcomp_core_latency((const OmxMbcompCore *)c); }
static inline void omx_mbcomp_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_mbcomp_core_run((OmxMbcompCore *)c, in_l, in_r, out_l, out_r, frames);
}
static inline float omx_mbcomp_core_meter_v(const void *c, uint32_t i) { return omx_mbcomp_core_meter((const OmxMbcompCore *)c, i); }

static const struct omx_face_core OMX_MBCOMP_CORE = {sizeof(OmxMbcompCore), omx_mbcomp_core_init_v, omx_mbcomp_core_resolve_v,
                                                   omx_mbcomp_core_latency_v, omx_mbcomp_core_run_v,
                                                   omx_mbcomp_core_meter_v};

#endif /* OMX_MBCOMP_CORE_H */
