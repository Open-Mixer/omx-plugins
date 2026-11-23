// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_comp_core.h — omx-comp's binding to omx-dsp's comp kernel: the ONE place both faces
 * (omx_comp_clap.c, omx_comp_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_comp_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-comp/omx-comp.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_COMP_CORE_H
#define OMX_COMP_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_comp_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_comp_instance.h>

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxCompInstance inst;
} OmxCompCore;

/** A fresh instance at `rate`: every state word zero. 1 when the kernel takes the rate, 0 when it refuses
 * it (the instance is then the identity: the caller must not run it). */
static inline int omx_comp_core_init(OmxCompCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  return omx_comp_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_comp_core_resolve(OmxCompCore *c, const float *values, int bypass) {
  omx_comp_instance_resolve(&c->inst, bypass,
                                  /* threshold_db */ values[OMX_COMP_PARAM_THRESHOLD_DB],
                                  /* ratio */ values[OMX_COMP_PARAM_RATIO],
                                  /* knee_db */ values[OMX_COMP_PARAM_KNEE_DB],
                                  /* attack_ms */ values[OMX_COMP_PARAM_ATTACK_MS],
                                  /* release_ms */ values[OMX_COMP_PARAM_RELEASE_MS],
                                  /* makeup_db */ values[OMX_COMP_PARAM_MAKEUP_DB],
                                  /* mix_pct */ values[OMX_COMP_PARAM_MIX_PCT],
                                  /* kind */ (int)lrintf(values[OMX_COMP_PARAM_KIND]),
                                  /* detector_oversampling */ (int)lrintf(values[OMX_COMP_PARAM_DETECTOR_OVERSAMPLING]),
                                  /* detector_topology */ (int)lrintf(values[OMX_COMP_PARAM_DETECTOR_TOPOLOGY]),
                                  /* key_source */ (int)lrintf(values[OMX_COMP_PARAM_KEY_SOURCE]),
                                  /* key_hp_hz */ values[OMX_COMP_PARAM_KEY_HP_HZ],
                                  /* key_lp_hz */ values[OMX_COMP_PARAM_KEY_LP_HZ],
                                  /* key_tilt_db */ values[OMX_COMP_PARAM_KEY_TILT_DB],
                                  /* key_listen */ (int)lrintf(values[OMX_COMP_PARAM_KEY_LISTEN]),
                                  /* hold_ms */ values[OMX_COMP_PARAM_HOLD_MS],
                                  /* detect_link */ (int)lrintf(values[OMX_COMP_PARAM_DETECT_LINK]),
                                  /* duck */ (int)lrintf(values[OMX_COMP_PARAM_DUCK]),
                                  /* range_db */ values[OMX_COMP_PARAM_RANGE_DB],
                                  /* release_mode */ (int)lrintf(values[OMX_COMP_PARAM_RELEASE_MODE]));
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_comp_core_latency(const OmxCompCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_comp_instance_latency(&c->inst)));
}

/** Kernel-read meter (a gain reduction, or a level the kernel reads) `i`'s reading (the declaration's `meters`, in order): the kernel's own, after the
 * last block. */
static inline float omx_comp_core_meter(const OmxCompCore *c, uint32_t i) {
  switch (i) {
  case 0:
    return c->inst.st.gr_min_db;
  case 3:
    return omx_comp_instance_key_level_db(&c->inst);
  default:
    return 0.0f;
  }
}

/** One block, stereo, keyed by `key` (NULL: nothing routed to the key, the kernel's own detector);
 * the outputs may alias the inputs or the key. */
static inline void omx_comp_core_run(OmxCompCore *c, const float *key, const float *in_l, const float *in_r,
                                         float *out_l, float *out_r, uint32_t frames) {
  omx_comp_instance_run_keyed(&c->inst, key, NULL, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline int omx_comp_core_init_v(void *c, uint32_t rate) { return omx_comp_core_init((OmxCompCore *)c, rate); }
static inline void omx_comp_core_resolve_v(void *c, const float *values, int bypass) {
  omx_comp_core_resolve((OmxCompCore *)c, values, bypass);
}
static inline uint32_t omx_comp_core_latency_v(const void *c) { return omx_comp_core_latency((const OmxCompCore *)c); }
static inline void omx_comp_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  omx_comp_core_run((OmxCompCore *)c, key, in_l, in_r, out_l, out_r, frames);
}
static inline float omx_comp_core_meter_v(const void *c, uint32_t i) { return omx_comp_core_meter((const OmxCompCore *)c, i); }

static const struct omx_face_core OMX_COMP_CORE = {sizeof(OmxCompCore), omx_comp_core_init_v, omx_comp_core_resolve_v,
                                                   omx_comp_core_latency_v, omx_comp_core_run_v,
                                                   omx_comp_core_meter_v};

#endif /* OMX_COMP_CORE_H */
