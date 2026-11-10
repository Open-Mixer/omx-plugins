// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_fbs_lv2.c -- the LV2 face of omx-fbs (spec 7.7, 7.13, 7.16).
 *
 * The whole DSP is omx-dsp's omx_fbs_lv2.h (the live shell, no LV2 in it); this file is the LV2
 * boilerplate over it: a descriptor, ports, and the worker pair that carries one analysis pass off
 * the audio thread. omx-hrp's face is its twin.
 *
 * run()           (audio)  omx_fbs_lv2_run      the input into the ring; the planted bands on the audio
 * work()          (worker) omx_fbs_lv2_answer   one detector frame, the placement, the set
 * work_response() (audio)  omx_fbs_lv2_adopt    a fixed-size copy of the set, the report updated
 *
 * Report ports are read-only output control ports: frequency, depth, Q and on, four per slot,
 * OMX_FBS_LV2_SLOTS slots, plus one `analysing` output. Mono: one audio in, one audio out (the
 * per-channel rule of spec 7.7/7.17). A host with no worker gets audio passed bit-exact, every slot
 * free and `analysing` 0.
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/core/lv2.h>
#include <lv2/worker/worker.h>

#include <omxdsp/analysis/omx_fbs_lv2.h>

#define OMX_FBS_LV2_URI "urn:openmixer:fbs"

enum {
  FBS_PORT_IN = 0,
  FBS_PORT_OUT,
  FBS_PORT_MODE,
  FBS_PORT_POOL,
  FBS_PORT_HOLD,
  FBS_PORT_BANDS,
  FBS_PORT_MERGE,
  FBS_PORT_COOLDOWN,
  FBS_PORT_ANALYSING,
  FBS_PORT_REPORT_BASE, /* 4 words * OMX_FBS_LV2_SLOTS, freq/depth/q/on interleaved per slot */
};

#define FBS_PORT_COUNT (FBS_PORT_REPORT_BASE + 4u * OMX_FBS_LV2_SLOTS)

typedef struct {
  OmxFbsLv2 core;
  void *block;

  const float *in;
  float *out;
  const float *ctl_mode;
  const float *ctl_pool;
  const float *ctl_hold;
  const float *ctl_bands;
  const float *ctl_merge;
  const float *ctl_cooldown;
  float *out_analysing;
  float *out_report[OMX_FBS_LV2_SLOTS][4];

  LV2_Worker_Schedule *schedule;
} OmxFbsLv2Face;

static LV2_Handle fbs_instantiate(const LV2_Descriptor *descriptor, double rate, const char *bundle_path,
                                  const LV2_Feature *const *features) {
  (void)descriptor;
  (void)bundle_path;
  OmxFbsLv2Face *f = (OmxFbsLv2Face *)calloc(1, sizeof(OmxFbsLv2Face));
  if (!f) return NULL;
  const uint32_t r = (uint32_t)rate;
  /* the block is the shell's 8-aligned partition; calloc's alignment is enough */
  f->block = calloc(1, omx_fbs_lv2_block_bytes(r));
  if (!f->block || !omx_fbs_lv2_init_block(&f->core, r, f->block)) {
    free(f->block);
    free(f);
    return NULL;
  }
  for (uint32_t i = 0; features && features[i]; i++) {
    if (strcmp(features[i]->URI, LV2_WORKER__schedule) == 0) f->schedule = (LV2_Worker_Schedule *)features[i]->data;
  }
  return (LV2_Handle)f;
}

static void fbs_connect_port(LV2_Handle instance, uint32_t port, void *data) {
  OmxFbsLv2Face *f = (OmxFbsLv2Face *)instance;
  switch (port) {
    case FBS_PORT_IN: f->in = (const float *)data; return;
    case FBS_PORT_OUT: f->out = (float *)data; return;
    case FBS_PORT_MODE: f->ctl_mode = (const float *)data; return;
    case FBS_PORT_POOL: f->ctl_pool = (const float *)data; return;
    case FBS_PORT_HOLD: f->ctl_hold = (const float *)data; return;
    case FBS_PORT_BANDS: f->ctl_bands = (const float *)data; return;
    case FBS_PORT_MERGE: f->ctl_merge = (const float *)data; return;
    case FBS_PORT_COOLDOWN: f->ctl_cooldown = (const float *)data; return;
    case FBS_PORT_ANALYSING: f->out_analysing = (float *)data; return;
    default: break;
  }
  if (port >= FBS_PORT_REPORT_BASE && port < FBS_PORT_COUNT) {
    const uint32_t off = port - FBS_PORT_REPORT_BASE;
    f->out_report[off / 4u][off % 4u] = (float *)data;
  }
}

static void fbs_activate(LV2_Handle instance) { omx_fbs_lv2_restart(&((OmxFbsLv2Face *)instance)->core); }

/* The four read-only report words of every slot, read by value from the instance's live report: the
 * ports hold no second copy a bug could let drift from the live set (spec 7.7 proof). */
static void fbs_publish_report(OmxFbsLv2Face *f) {
  for (uint32_t k = 0; k < OMX_FBS_LV2_SLOTS; k++) {
    const OmxBandReport r = omx_fbs_lv2_report(&f->core, k);
    if (f->out_report[k][0]) *f->out_report[k][0] = r.freq_hz;
    if (f->out_report[k][1]) *f->out_report[k][1] = r.depth_db;
    if (f->out_report[k][2]) *f->out_report[k][2] = r.q;
    if (f->out_report[k][3]) *f->out_report[k][3] = r.on;
  }
  if (f->out_analysing) *f->out_analysing = (float)omx_fbs_lv2_analysing(&f->core);
}

static void fbs_run(LV2_Handle instance, uint32_t n_samples) {
  OmxFbsLv2Face *f = (OmxFbsLv2Face *)instance;
  if (!f->in || !f->out) return;
  omx_fbs_lv2_set_mode(&f->core, f->ctl_mode ? *f->ctl_mode : (float)OMX_FBS_MODES_DEFAULT);
  omx_fbs_lv2_run(&f->core, f->in, f->out, n_samples);
  fbs_publish_report(f);

  if (f->schedule && omx_fbs_lv2_due(&f->core) && !omx_fbs_lv2_in_flight(&f->core)) {
    const OmxFbsLv2Ask ask = omx_fbs_lv2_ask_of(
        f->ctl_pool ? *f->ctl_pool : (float)OMX_FBS_POOLS_DEFAULT, f->ctl_hold ? *f->ctl_hold : (float)OMX_FBS_DEFAULT_HOLD_SECONDS,
        f->ctl_bands ? *f->ctl_bands : (float)OMX_FBS_DEFAULT_MAX_AUTO_BANDS, (float)OMX_FBS_DEFAULT_NOTCH_Q,
        f->ctl_merge ? *f->ctl_merge : (float)OMX_FBS_DEFAULT_MERGE_CENTS,
        f->ctl_cooldown ? *f->ctl_cooldown : (float)OMX_FBS_DEFAULT_COOLDOWN_SECONDS);
    if (f->schedule->schedule_work(f->schedule->handle, (uint32_t)sizeof(ask), &ask) == LV2_WORKER_SUCCESS)
      omx_fbs_lv2_mark_asked(&f->core);
  }
}

static void fbs_deactivate(LV2_Handle instance) { (void)instance; }

static void fbs_cleanup(LV2_Handle instance) {
  OmxFbsLv2Face *f = (OmxFbsLv2Face *)instance;
  free(f->block);
  free(f);
}

static LV2_Worker_Status fbs_work(LV2_Handle instance, LV2_Worker_Respond_Function respond,
                                  LV2_Worker_Respond_Handle handle, uint32_t size, const void *data) {
  OmxFbsLv2Face *f = (OmxFbsLv2Face *)instance;
  if (size != sizeof(OmxFbsLv2Ask) || !data) return LV2_WORKER_ERR_UNKNOWN;
  /* the set is a few kB: static storage would be shared by two instances, so it is on the worker's stack */
  OmxFbsLv2Set set;
  omx_fbs_lv2_answer(&f->core, (const OmxFbsLv2Ask *)data, &set);
  respond(handle, (uint32_t)sizeof(set), &set);
  return LV2_WORKER_SUCCESS;
}

static LV2_Worker_Status fbs_work_response(LV2_Handle instance, uint32_t size, const void *data) {
  OmxFbsLv2Face *f = (OmxFbsLv2Face *)instance;
  if (size != sizeof(OmxFbsLv2Set) || !data) return LV2_WORKER_ERR_UNKNOWN;
  omx_fbs_lv2_adopt(&f->core, (const OmxFbsLv2Set *)data);
  omx_fbs_lv2_mark_answered(&f->core);
  return LV2_WORKER_SUCCESS;
}

static LV2_Worker_Status fbs_end_run(LV2_Handle instance) {
  (void)instance;
  return LV2_WORKER_SUCCESS;
}

static const void *fbs_extension_data(const char *uri) {
  static const LV2_Worker_Interface worker = {fbs_work, fbs_work_response, fbs_end_run};
  if (strcmp(uri, LV2_WORKER__interface) == 0) return &worker;
  return NULL;
}

static const LV2_Descriptor G_FBS_LV2 = {
    OMX_FBS_LV2_URI, fbs_instantiate, fbs_connect_port, fbs_activate,
    fbs_run,         fbs_deactivate,  fbs_cleanup,      fbs_extension_data,
};

LV2_SYMBOL_EXPORT const LV2_Descriptor *lv2_descriptor(uint32_t index) { return index == 0 ? &G_FBS_LV2 : NULL; }
