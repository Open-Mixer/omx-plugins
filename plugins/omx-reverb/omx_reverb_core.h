// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_reverb_core.h — omx-reverb's binding to omx-dsp's reverb kernel: the ONE place both faces
 * (omx_reverb_clap.c, omx_reverb_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_reverb_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-reverb/omx-reverb.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 * `early_db` is no parameter: the declaration holds it at its kernel's declared default, OMX_REVERB_EARLY_DB_RANGE_DEFAULT.
 * `tail_db` is no parameter: the declaration holds it at its kernel's declared default, OMX_REVERB_TAIL_DB_RANGE_DEFAULT.
 * `early_size_pct` is no parameter: the declaration holds it at its kernel's declared default, OMX_REVERB_EARLY_SIZE_PCT_RANGE_DEFAULT.
 * `duck_db` is no parameter: the declaration holds it at its kernel's declared default, OMX_REVERB_DUCK_DB_RANGE_DEFAULT.
 * `duck_threshold_db` is no parameter: the declaration holds it at its kernel's declared default, OMX_REVERB_DUCK_THRESHOLD_DB_RANGE_DEFAULT.
 * `duck_release_ms` is no parameter: the declaration holds it at its kernel's declared default, OMX_REVERB_DUCK_RELEASE_MS_RANGE_DEFAULT.
 * `wet_deess` is no parameter: the declaration holds it at its kernel's declared default, OMX_REVERB_WET_DEESSES_DEFAULT.
 * `deess_freq_hz` is no parameter: the declaration holds it at its kernel's declared default, OMX_REVERB_DEESS_FREQ_HZ_RANGE_DEFAULT.
 * `deess_threshold_db` is no parameter: the declaration holds it at its kernel's declared default, OMX_REVERB_DEESS_THRESHOLD_DB_RANGE_DEFAULT.
 */
#ifndef OMX_REVERB_CORE_H
#define OMX_REVERB_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_reverb_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_reverb_instance.h>

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxReverbInstance inst;
} OmxReverbCore;

/** A fresh instance at `rate`: every state word zero. A rate the kernel does not declare leaves it a wire. */
static inline void omx_reverb_core_init(OmxReverbCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_reverb_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_reverb_core_resolve(OmxReverbCore *c, const float *values, int bypass) {
  omx_reverb_instance_resolve(&c->inst, bypass,
                                  /* plate_mod_depth */ values[OMX_REVERB_PARAM_PLATE_MOD_DEPTH],
                                  /* mix */ values[OMX_REVERB_PARAM_MIX],
                                  /* size */ values[OMX_REVERB_PARAM_SIZE],
                                  /* damping */ values[OMX_REVERB_PARAM_DAMPING],
                                  /* width */ values[OMX_REVERB_PARAM_WIDTH],
                                  /* predelay */ values[OMX_REVERB_PARAM_PREDELAY],
                                  /* lowcut */ values[OMX_REVERB_PARAM_LOWCUT],
                                  /* highcut */ values[OMX_REVERB_PARAM_HIGHCUT],
                                  /* reverse */ values[OMX_REVERB_PARAM_REVERSE],
                                  /* hold */ values[OMX_REVERB_PARAM_HOLD],
                                  /* release */ values[OMX_REVERB_PARAM_RELEASE],
                                  /* gate_threshold */ values[OMX_REVERB_PARAM_GATE_THRESHOLD],
                                  /* algorithm */ (int)lrintf(values[OMX_REVERB_PARAM_ALGORITHM]),
                                  /* early_db */ (float)OMX_REVERB_EARLY_DB_RANGE_DEFAULT,
                                  /* tail_db */ (float)OMX_REVERB_TAIL_DB_RANGE_DEFAULT,
                                  /* early_size_pct */ (float)OMX_REVERB_EARLY_SIZE_PCT_RANGE_DEFAULT,
                                  /* duck_db */ (float)OMX_REVERB_DUCK_DB_RANGE_DEFAULT,
                                  /* duck_threshold_db */ (float)OMX_REVERB_DUCK_THRESHOLD_DB_RANGE_DEFAULT,
                                  /* duck_release_ms */ (float)OMX_REVERB_DUCK_RELEASE_MS_RANGE_DEFAULT,
                                  /* wet_deess */ (int)OMX_REVERB_WET_DEESSES_DEFAULT,
                                  /* deess_freq_hz */ (float)OMX_REVERB_DEESS_FREQ_HZ_RANGE_DEFAULT,
                                  /* deess_threshold_db */ (float)OMX_REVERB_DEESS_THRESHOLD_DB_RANGE_DEFAULT);
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_reverb_core_latency(const OmxReverbCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(OMX_REVERB_INSTANCE_LATENCY_FRAMES));
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_reverb_core_run(OmxReverbCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_reverb_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_reverb_core_init_v(void *c, uint32_t rate) { omx_reverb_core_init((OmxReverbCore *)c, rate); }
static inline void omx_reverb_core_resolve_v(void *c, const float *values, int bypass) {
  omx_reverb_core_resolve((OmxReverbCore *)c, values, bypass);
}
static inline uint32_t omx_reverb_core_latency_v(const void *c) { return omx_reverb_core_latency((const OmxReverbCore *)c); }
static inline void omx_reverb_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_reverb_core_run((OmxReverbCore *)c, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_REVERB_CORE = {sizeof(OmxReverbCore), omx_reverb_core_init_v, omx_reverb_core_resolve_v,
                                                   omx_reverb_core_latency_v, omx_reverb_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_REVERB_CORE_H */
