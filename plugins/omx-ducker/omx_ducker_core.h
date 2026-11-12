// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_ducker_core.h — omx-ducker's binding to omx-dsp's ducker kernel: the ONE place both faces
 * (omx_ducker_clap.c, omx_ducker_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_comp_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-ducker/omx-ducker.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 * `duck` is not a parameter of this plugin: it is passed at "on".
 * `keySource` is not a parameter of this plugin: it is passed at "sidechain".
 * `makeupDb` is not a parameter of this plugin: it is passed at "default".
 * `mixPct` is not a parameter of this plugin: it is passed at "default".
 * `kind` is not a parameter of this plugin: it is passed at "default".
 * `detectorOversampling` is not a parameter of this plugin: it is passed at "default".
 * `detectorTopology` is not a parameter of this plugin: it is passed at "feedforward".
 * `keyTiltDb` is not a parameter of this plugin: it is passed at "default".
 * `detectLink` is not a parameter of this plugin: it is passed at "default".
 * `releaseMode` is not a parameter of this plugin: it is passed at "fixed".
 */
#ifndef OMX_DUCKER_CORE_H
#define OMX_DUCKER_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_ducker_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_comp_instance.h>

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxCompInstance inst;
} OmxDuckerCore;

/** A fresh instance at `rate`: every state word zero. A rate the kernel does not declare leaves it a wire. */
static inline void omx_ducker_core_init(OmxDuckerCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_comp_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_ducker_core_resolve(OmxDuckerCore *c, const float *values, int bypass) {
  omx_comp_instance_resolve(&c->inst, bypass,
                                  /* threshold_db */ values[OMX_DUCKER_PARAM_THRESHOLD_DB],
                                  /* ratio */ values[OMX_DUCKER_PARAM_RATIO],
                                  /* knee_db */ values[OMX_DUCKER_PARAM_KNEE_DB],
                                  /* attack_ms */ values[OMX_DUCKER_PARAM_ATTACK_MS],
                                  /* release_ms */ values[OMX_DUCKER_PARAM_RELEASE_MS],
                                  /* makeup_db */ 0.0f,
                                  /* mix_pct */ 100.0f,
                                  /* kind */ 0,
                                  /* detector_oversampling */ 0,
                                  /* detector_topology */ 0,
                                  /* key_source */ 1,
                                  /* key_hp_hz */ values[OMX_DUCKER_PARAM_KEY_HP_HZ],
                                  /* key_lp_hz */ values[OMX_DUCKER_PARAM_KEY_LP_HZ],
                                  /* key_tilt_db */ 0.0f,
                                  /* key_listen */ (int)lrintf(values[OMX_DUCKER_PARAM_KEY_LISTEN]),
                                  /* hold_ms */ values[OMX_DUCKER_PARAM_HOLD_MS],
                                  /* detect_link */ 0,
                                  /* duck */ 1,
                                  /* range_db */ values[OMX_DUCKER_PARAM_RANGE_DB],
                                  /* release_mode */ 0);
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_ducker_core_latency(const OmxDuckerCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_comp_instance_latency(&c->inst)));
}

/** One block, stereo, keyed by `key` (NULL: nothing routed to the key, the kernel's own detector);
 * the outputs may alias the inputs or the key. */
static inline void omx_ducker_core_run(OmxDuckerCore *c, const float *key, const float *in_l, const float *in_r,
                                         float *out_l, float *out_r, uint32_t frames) {
  omx_comp_instance_run_keyed(&c->inst, key, NULL, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_ducker_core_init_v(void *c, uint32_t rate) { omx_ducker_core_init((OmxDuckerCore *)c, rate); }
static inline void omx_ducker_core_resolve_v(void *c, const float *values, int bypass) {
  omx_ducker_core_resolve((OmxDuckerCore *)c, values, bypass);
}
static inline uint32_t omx_ducker_core_latency_v(const void *c) { return omx_ducker_core_latency((const OmxDuckerCore *)c); }
static inline void omx_ducker_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  omx_ducker_core_run((OmxDuckerCore *)c, key, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_DUCKER_CORE = {sizeof(OmxDuckerCore), omx_ducker_core_init_v, omx_ducker_core_resolve_v,
                                                   omx_ducker_core_latency_v, omx_ducker_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_DUCKER_CORE_H */
