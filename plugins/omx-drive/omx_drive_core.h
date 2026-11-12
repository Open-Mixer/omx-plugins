// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_drive_core.h — omx-drive's binding to omx-dsp's drive kernel: the ONE place both faces
 * (omx_drive_clap.c, omx_drive_lv2.c) reach the DSP. It holds no DSP of its own: it hands the
 * declared parameter values, by name, to omx-dsp's instance face (<omxdsp/fx/omx_drive_instance.h>)
 * and runs it. The instance is plain inline state: nothing is allocated, locked or called into the
 * system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-drive/omx-drive.decl.json and
 * the face's resolve() prototype: each argument takes the parameter whose contract control it names.
 */
#ifndef OMX_DRIVE_CORE_H
#define OMX_DRIVE_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST: its guard is omx-dsp's own, so the instance header reads this table. */
#include "omx_drive_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_drive_instance.h>

/** The kernel's state and what a block runs with. */
typedef struct {
  uint32_t rate;
  OmxDriveInstance inst;
} OmxDriveCore;

/** A fresh instance at `rate`: every state word zero. A rate the kernel does not declare leaves it a wire. */
static inline void omx_drive_core_init(OmxDriveCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_drive_instance_init(&c->inst, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each to its resolve() argument. */
static inline void omx_drive_core_resolve(OmxDriveCore *c, const float *values, int bypass) {
  omx_drive_instance_resolve(&c->inst, bypass,
                                  /* amount */ values[OMX_DRIVE_PARAM_AMOUNT],
                                  /* character */ values[OMX_DRIVE_PARAM_CHARACTER],
                                  /* band_freq */ values[OMX_DRIVE_PARAM_BAND_FREQ],
                                  /* mix */ values[OMX_DRIVE_PARAM_MIX],
                                  /* trim */ values[OMX_DRIVE_PARAM_TRIM],
                                  /* curve */ (int)lrintf(values[OMX_DRIVE_PARAM_CURVE]),
                                  /* band */ (int)lrintf(values[OMX_DRIVE_PARAM_BAND]),
                                  /* auto_gain */ (int)lrintf(values[OMX_DRIVE_PARAM_AUTO_GAIN]),
                                  /* stereo_link */ (int)lrintf(values[OMX_DRIVE_PARAM_STEREO_LINK]),
                                  /* hf_rolloff */ (int)lrintf(values[OMX_DRIVE_PARAM_HF_ROLLOFF]),
                                  /* oversample */ (int)lrintf(values[OMX_DRIVE_PARAM_OVERSAMPLE]));
}

/** The latency the kernel adds, in samples. */
static inline uint32_t omx_drive_core_latency(const OmxDriveCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_drive_instance_latency(&c->inst)));
}

/** One block, stereo; the outputs may alias the inputs. */
static inline void omx_drive_core_run(OmxDriveCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  omx_drive_instance_run(&c->inst, in_l, in_r, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_drive_core_init_v(void *c, uint32_t rate) { omx_drive_core_init((OmxDriveCore *)c, rate); }
static inline void omx_drive_core_resolve_v(void *c, const float *values, int bypass) {
  omx_drive_core_resolve((OmxDriveCore *)c, values, bypass);
}
static inline uint32_t omx_drive_core_latency_v(const void *c) { return omx_drive_core_latency((const OmxDriveCore *)c); }
static inline void omx_drive_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_drive_core_run((OmxDriveCore *)c, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_DRIVE_CORE = {sizeof(OmxDriveCore), omx_drive_core_init_v, omx_drive_core_resolve_v,
                                                   omx_drive_core_latency_v, omx_drive_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_DRIVE_CORE_H */
