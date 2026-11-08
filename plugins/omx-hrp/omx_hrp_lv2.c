// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_hrp_lv2.c -- the LV2 face of omx-hrp (spec 7.7, 7.13, 7.16).
 *
 * The whole DSP is omx-dsp's omx_hrp_lv2.h (the live shell's third shell, hrp_lv2.c is its
 * model); this file is the LV2 boilerplate over it: a descriptor, ports, and the worker pair
 * that carries one analysis pass off the audio thread.
 *
 * run()           (audio)  omx_hrp_lv2_run      the ring fed, the cascade applied or bypassed
 * work()          (worker) omx_hrp_lv2_answer   install the ask, analyse one hop, write the set
 * work_response() (audio)  omx_hrp_lv2_adopt    the set installed, the report ports updated
 *
 * Report ports are read-only (lv2:portProperty lv2:reportsLatency is not this; the TTL marks
 * them output control ports): frequency, depth, Q and on, four per slot, OMX_HRP_LV2_MAX_BANDS
 * slots, plus one `analysing` output. Mono: one audio in, one audio out (the per-channel
 * refusal of spec 7.7/7.17 -- a bus has no mono insert point to rack it at).
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/core/lv2.h>
#include <lv2/worker/worker.h>

#include <omxdsp/analysis/omx_hrp_lv2.h>

#define OMX_HRP_LV2_URI "urn:openmixer:hrp"
#define OMX_HRP_LV2_SLOTS OMX_HRP_LV2_MAX_BANDS

enum {
  HRP_PORT_IN = 0,
  HRP_PORT_OUT,
  HRP_PORT_BYPASS,
  HRP_PORT_FAMILY,
  HRP_PORT_AMOUNT,
  HRP_PORT_BANDS,
  HRP_PORT_MODE,
  HRP_PORT_ANALYSING,
  HRP_PORT_REPORT_BASE, /* 4 words * OMX_HRP_LV2_SLOTS, freq/depth/q/on interleaved per slot */
};

#define HRP_PORT_COUNT (HRP_PORT_REPORT_BASE + 4u * OMX_HRP_LV2_SLOTS)

typedef struct {
  OmxHrpLv2 core;
  void *block;

  const float *in;
  float *out;
  const float *ctl_bypass;
  const float *ctl_family;
  const float *ctl_amount;
  const float *ctl_bands;
  const float *ctl_mode;
  float *out_analysing;
  float *out_report[OMX_HRP_LV2_SLOTS][4];

  LV2_Worker_Schedule *schedule;
} OmxHrpLv2Face;

static LV2_Handle hrp_instantiate(const LV2_Descriptor *descriptor, double rate, const char *bundle_path,
                                  const LV2_Feature *const *features) {
  (void)descriptor;
  (void)bundle_path;
  OmxHrpLv2Face *f = (OmxHrpLv2Face *)calloc(1, sizeof(OmxHrpLv2Face));
  if (!f) return NULL;
  const uint32_t r = (uint32_t)rate;
  const uint32_t floats = omx_hrp_lv2_block_floats(r);
  f->block = calloc(floats, sizeof(float));
  if (!f->block || !omx_hrp_lv2_init_block(&f->core, r, (float *)f->block)) {
    free(f->block);
    free(f);
    return NULL;
  }
  for (uint32_t i = 0; features && features[i]; i++) {
    if (strcmp(features[i]->URI, LV2_WORKER__schedule) == 0) f->schedule = (LV2_Worker_Schedule *)features[i]->data;
  }
  return (LV2_Handle)f;
}

static void hrp_connect_port(LV2_Handle instance, uint32_t port, void *data) {
  OmxHrpLv2Face *f = (OmxHrpLv2Face *)instance;
  switch (port) {
    case HRP_PORT_IN: f->in = (const float *)data; return;
    case HRP_PORT_OUT: f->out = (float *)data; return;
    case HRP_PORT_BYPASS: f->ctl_bypass = (const float *)data; return;
    case HRP_PORT_FAMILY: f->ctl_family = (const float *)data; return;
    case HRP_PORT_AMOUNT: f->ctl_amount = (const float *)data; return;
    case HRP_PORT_BANDS: f->ctl_bands = (const float *)data; return;
    case HRP_PORT_MODE: f->ctl_mode = (const float *)data; return;
    case HRP_PORT_ANALYSING: f->out_analysing = (float *)data; return;
    default: break;
  }
  if (port >= HRP_PORT_REPORT_BASE && port < HRP_PORT_COUNT) {
    const uint32_t off = port - HRP_PORT_REPORT_BASE;
    f->out_report[off / 4u][off % 4u] = (float *)data;
  }
}

static void hrp_activate(LV2_Handle instance) { omx_hrp_lv2_restart(&((OmxHrpLv2Face *)instance)->core); }

/* Write the four read-only report words for every slot from the instance's live report (spec
 * 7.7: "report equal to the live set" is this call -- the ports are read by value here and
 * nowhere else holds a second copy a bug could let them drift from). */
static void hrp_publish_report(OmxHrpLv2Face *f) {
  for (uint32_t k = 0; k < OMX_HRP_LV2_SLOTS; k++) {
    const OmxBandReport r = omx_hrp_lv2_report(&f->core, k);
    if (f->out_report[k][0]) *f->out_report[k][0] = r.freq_hz;
    if (f->out_report[k][1]) *f->out_report[k][1] = r.depth_db;
    if (f->out_report[k][2]) *f->out_report[k][2] = r.q;
    if (f->out_report[k][3]) *f->out_report[k][3] = r.on;
  }
  if (f->out_analysing) *f->out_analysing = (float)omx_hrp_lv2_analysing(&f->core);
}

static void hrp_run(LV2_Handle instance, uint32_t n_samples) {
  OmxHrpLv2Face *f = (OmxHrpLv2Face *)instance;
  if (!f->in || !f->out) return;
  omx_hrp_lv2_set_mode(&f->core, f->ctl_mode ? *f->ctl_mode : 0.0f);
  omx_hrp_lv2_run(&f->core, f->in, f->out, n_samples);
  hrp_publish_report(f);

  const OmxHrpLv2Ask ask = omx_hrp_lv2_ask_of(f->ctl_bypass ? *f->ctl_bypass : 0.0f,
                                              f->ctl_family ? *f->ctl_family : -1.0f,
                                              f->ctl_amount ? *f->ctl_amount : 0.5f,
                                              f->ctl_bands ? *f->ctl_bands : (float)OMX_HRP_LV2_SLOTS);
  if (f->schedule && omx_hrp_lv2_wants(&f->core, &ask) && !omx_hrp_lv2_in_flight(&f->core)) {
    if (f->schedule->schedule_work(f->schedule->handle, (uint32_t)sizeof(ask), &ask) == LV2_WORKER_SUCCESS)
      omx_hrp_lv2_mark_asked(&f->core);
  }
}

static void hrp_deactivate(LV2_Handle instance) { (void)instance; }

static void hrp_cleanup(LV2_Handle instance) {
  OmxHrpLv2Face *f = (OmxHrpLv2Face *)instance;
  free(f->block);
  free(f);
}

static LV2_Worker_Status hrp_work(LV2_Handle instance, LV2_Worker_Respond_Function respond,
                                  LV2_Worker_Respond_Handle handle, uint32_t size, const void *data) {
  OmxHrpLv2Face *f = (OmxHrpLv2Face *)instance;
  if (size != sizeof(OmxHrpLv2Ask) || !data) return LV2_WORKER_ERR_UNKNOWN;
  OmxHrpLv2Set set;
  omx_hrp_lv2_answer(&f->core, (const OmxHrpLv2Ask *)data, &set);
  respond(handle, (uint32_t)sizeof(set), &set);
  return LV2_WORKER_SUCCESS;
}

static LV2_Worker_Status hrp_work_response(LV2_Handle instance, uint32_t size, const void *data) {
  OmxHrpLv2Face *f = (OmxHrpLv2Face *)instance;
  if (size != sizeof(OmxHrpLv2Set) || !data) return LV2_WORKER_ERR_UNKNOWN;
  omx_hrp_lv2_adopt(&f->core, (const OmxHrpLv2Set *)data);
  omx_hrp_lv2_mark_answered(&f->core);
  return LV2_WORKER_SUCCESS;
}

static LV2_Worker_Status hrp_end_run(LV2_Handle instance) {
  (void)instance;
  return LV2_WORKER_SUCCESS;
}

static const void *hrp_extension_data(const char *uri) {
  static const LV2_Worker_Interface worker = {hrp_work, hrp_work_response, hrp_end_run};
  if (strcmp(uri, LV2_WORKER__interface) == 0) return &worker;
  return NULL;
}

static const LV2_Descriptor G_HRP_LV2 = {
    OMX_HRP_LV2_URI, hrp_instantiate, hrp_connect_port, hrp_activate,
    hrp_run,         hrp_deactivate,  hrp_cleanup,      hrp_extension_data,
};

LV2_SYMBOL_EXPORT const LV2_Descriptor *lv2_descriptor(uint32_t index) { return index == 0 ? &G_HRP_LV2 : NULL; }
