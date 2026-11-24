// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_face_meters.h — one block's meter values, the same for every face (lib/omx_clap_face.c publishes
 * them through clap.meters/1, lib/omx_lv2_face.c writes them to its output control ports): a level is
 * the block's peak of each leg at the inputs (before the kernel runs) or the outputs (after), a reading
 * the kernel's own after the block (docs/meters.md). Arithmetic on the caller's array: no allocation,
 * no lock, no call into the system.
 */
#ifndef OMX_FACE_METERS_H
#define OMX_FACE_METERS_H

#include <math.h>
#include <stdint.h>

#include "omx_plugin_face.h"
#include <omxdsp/mix/omx_meter.h> /* omx_meter_level_db: a level meter's reading */

/** The meters of `source` (OMX_FACE_METER_IN or _OUT): each leg's level of `l` and `r` into `mv`. */
static inline void omx_face_meter_levels(const struct omx_face *f, uint32_t source, const float *l, const float *r,
                                         uint32_t frames, double *mv) {
  for (uint32_t m = 0; m < f->meter_count; ++m) {
    const struct omx_face_meter *row = &f->meters[m];
    if (row->source != source) continue;
    mv[row->at] = omx_meter_level_db(l, frames, row->min);
    mv[row->at + 1] = omx_meter_level_db(r, frames, row->min);
  }
}

/** The longest run of consecutive samples over full scale in `x` (a NaN is none). */
static inline double omx_face_clip_run(const float *x, uint32_t frames) {
  uint32_t run = 0, longest = 0;
  for (uint32_t i = 0; i < frames; ++i) {
    run = fabsf(x[i]) > 1.0f ? run + 1u : 0u;
    if (run > longest) longest = run;
  }
  return (double)longest;
}

/** The clip words of `source`'s levels that declare one: each leg's longest over-full-scale run of `l` and `r`
 * into `cv` (laid out as the values; the caller zeroes it first). */
static inline void omx_face_meter_clips(const struct omx_face *f, uint32_t source, const float *l, const float *r,
                                        uint32_t frames, double *cv) {
  for (uint32_t m = 0; m < f->meter_count; ++m) {
    const struct omx_face_meter *row = &f->meters[m];
    if (row->source != source || !row->clip) continue;
    cv[row->at] = omx_face_clip_run(l, frames);
    cv[row->at + 1] = omx_face_clip_run(r, frames);
  }
}

/** The ring's policy for the declaration's meters: a gain reduction holds its deepest, a meter declared
 * `held: false` holds nothing. */
static inline void omx_face_meter_masks(const struct omx_face *f, unsigned int *deep, unsigned int *hold) {
  *deep = 0u;
  *hold = 0u;
  for (uint32_t m = 0; m < f->meter_count; ++m) {
    const struct omx_face_meter *row = &f->meters[m];
    for (uint32_t c = 0; c < row->channels; ++c) {
      if (row->kind == OMX_FACE_METER_KIND_GAIN_REDUCTION) *deep |= 1u << (row->at + c);
      if (row->held) *hold |= 1u << (row->at + c);
    }
  }
}

/** The kernel's readings of the block just run into `mv`; a bypassed kernel's state is stale, and reads 0
 * (a level the kernel reads, its display minimum). */
static inline void omx_face_meter_readings(const struct omx_face *f, const void *core, int bypassed, double *mv) {
  for (uint32_t m = 0; m < f->meter_count; ++m) {
    const struct omx_face_meter *row = &f->meters[m];
    if (row->source == OMX_FACE_METER_READ)
      mv[row->at] = bypassed ? (row->kind == OMX_FACE_METER_KIND_GAIN_REDUCTION ? 0.0 : row->min)
                             : (double)f->core->meter(core, m);
  }
}

#endif /* OMX_FACE_METERS_H */
