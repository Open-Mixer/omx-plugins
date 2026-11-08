// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_loudlevel_core.h — omx-loudlevel's binding to omx-dsp's loudlevel kernel: the ONE place both faces
 * (omx_loudlevel_clap.c, omx_loudlevel_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_loudlevel_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-loudlevel/omx-loudlevel.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_LOUDLEVEL_CORE_H
#define OMX_LOUDLEVEL_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_loudlevel_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_loudlevel_instance.h>

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxLoudlevelInstance inst;
} OmxLoudlevelCore;

/** A fresh instance at `rate`: every state word zero. A rate the kernel does not declare leaves it a wire. */
static inline void omx_loudlevel_core_init(OmxLoudlevelCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_loudlevel_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_loudlevel_core_resolve(OmxLoudlevelCore *c, const float *values, int bypass) {
  omx_loudlevel_instance_resolve(&c->inst, bypass,
                                  /* target_lufs */ values[OMX_LOUDLEVEL_PARAM_TARGET_LUFS],
                                  /* range_db */ values[OMX_LOUDLEVEL_PARAM_RANGE_DB],
                                  /* speed_db_per_s */ values[OMX_LOUDLEVEL_PARAM_SPEED_DB_PER_S],
                                  /* gate_lufs */ values[OMX_LOUDLEVEL_PARAM_GATE_LUFS],
                                  /* window */ (int)lrintf(values[OMX_LOUDLEVEL_PARAM_WINDOW]));
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_loudlevel_core_latency(const OmxLoudlevelCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_loudlevel_instance_latency(&c->inst)));
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_loudlevel_core_run(OmxLoudlevelCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_loudlevel_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_loudlevel_core_init_v(void *c, uint32_t rate) { omx_loudlevel_core_init((OmxLoudlevelCore *)c, rate); }
static inline void omx_loudlevel_core_resolve_v(void *c, const float *values, int bypass) {
  omx_loudlevel_core_resolve((OmxLoudlevelCore *)c, values, bypass);
}
static inline uint32_t omx_loudlevel_core_latency_v(const void *c) { return omx_loudlevel_core_latency((const OmxLoudlevelCore *)c); }
static inline void omx_loudlevel_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_loudlevel_core_run((OmxLoudlevelCore *)c, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_LOUDLEVEL_CORE = {sizeof(OmxLoudlevelCore), omx_loudlevel_core_init_v, omx_loudlevel_core_resolve_v,
                                                   omx_loudlevel_core_latency_v, omx_loudlevel_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_LOUDLEVEL_CORE_H */
