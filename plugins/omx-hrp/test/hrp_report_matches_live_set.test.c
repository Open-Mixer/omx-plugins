// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * Oracle (spec 7.7 Proof): omx-hrp's report is equal to its live set. A crafted correction set
 * is adopted through the real LV2 work_response() callback, with a distinct, non-uniform value
 * in every slot's four report words so a transposed field or a wrong slot index cannot pass by
 * accident; the four read-only ports per slot and `analysing` must then read exactly what
 * omx_hrp_lv2_report() reads off the live instance, slot by slot.
 */
#include <assert.h>
#include <lv2/core/lv2.h>
#include <lv2/worker/worker.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <omxdsp/analysis/omx_hrp_lv2.h>

enum {
  HRP_PORT_IN = 0,
  HRP_PORT_OUT,
  HRP_PORT_BYPASS,
  HRP_PORT_FAMILY,
  HRP_PORT_AMOUNT,
  HRP_PORT_BANDS,
  HRP_PORT_MODE,
  HRP_PORT_ANALYSING,
  HRP_PORT_REPORT_BASE,
};
#define HRP_PORT_COUNT (HRP_PORT_REPORT_BASE + 4u * OMX_HRP_LV2_MAX_BANDS)

extern const LV2_Descriptor *lv2_descriptor(uint32_t index);

static int g_failed = 0;
#define CHECK_EQ_F(got, want, what)                                                                        \
  do {                                                                                                     \
    if ((got) != (want)) {                                                                                 \
      fprintf(stderr, "FAIL %s: got %f want %f\n", (what), (double)(got), (double)(want));                 \
      g_failed = 1;                                                                                        \
    }                                                                                                       \
  } while (0)

int main(void) {
  const LV2_Descriptor *d = lv2_descriptor(0);
  assert(d && lv2_descriptor(1) == NULL);

  const double rate = 48000.0;
  LV2_Handle inst = d->instantiate(d, rate, ".", NULL);
  assert(inst != NULL);

  float in[64] = {0}, out[64] = {0};
  float bypass = 0.0f, family = -1.0f, amount = 0.5f, bands = (float)OMX_HRP_LV2_MAX_BANDS, mode = 0.0f;
  float analysing = -1.0f;
  float report_ports[OMX_HRP_LV2_MAX_BANDS][4];
  memset(report_ports, 0, sizeof(report_ports));

  d->connect_port(inst, HRP_PORT_IN, in);
  d->connect_port(inst, HRP_PORT_OUT, out);
  d->connect_port(inst, HRP_PORT_BYPASS, &bypass);
  d->connect_port(inst, HRP_PORT_FAMILY, &family);
  d->connect_port(inst, HRP_PORT_AMOUNT, &amount);
  d->connect_port(inst, HRP_PORT_BANDS, &bands);
  d->connect_port(inst, HRP_PORT_MODE, &mode);
  d->connect_port(inst, HRP_PORT_ANALYSING, &analysing);
  for (uint32_t k = 0; k < OMX_HRP_LV2_MAX_BANDS; k++)
    for (uint32_t w = 0; w < 4u; w++)
      d->connect_port(inst, HRP_PORT_REPORT_BASE + 4u * k + w, &report_ports[k][w]);

  d->activate(inst);
  d->run(inst, 64);

  /* before a worker ever answers: analysing is 0 and every slot free (spec: "a host with no
   * worker gets a plugin that passes audio and corrects nothing, and the panel says so"). */
  CHECK_EQ_F(analysing, 0.0f, "analysing before any pass");
  for (uint32_t k = 0; k < OMX_HRP_LV2_MAX_BANDS; k++) CHECK_EQ_F(report_ports[k][3], 0.0f, "on before any pass");

  const LV2_Worker_Interface *worker = (const LV2_Worker_Interface *)d->extension_data(LV2_WORKER__interface);
  assert(worker != NULL);

  /* A crafted set, one distinct value per word per slot: a swapped field or a slot off by one
   * cannot pass this by coincidence. */
  OmxHrpLv2Set set;
  memset(&set, 0, sizeof(set));
  set.count = 0;
  for (uint32_t k = 0; k < OMX_HRP_LV2_MAX_BANDS; k++) {
    OmxBandReport r;
    r.freq_hz = 100.0f + 10.0f * (float)k;
    r.depth_db = -(1.0f + (float)k);
    r.q = 5.0f + (float)k;
    r.on = 1.0f;
    r.origin = (float)OMX_CURVE_ORIGINS_HRP;
    r.shape = (float)OMX_CURVE_SHAPES_BELL;
    set.report[k] = r;
  }
  LV2_Worker_Status st = worker->work_response(inst, (uint32_t)sizeof(set), &set);
  assert(st == LV2_WORKER_SUCCESS);

  d->run(inst, 64);

  CHECK_EQ_F(analysing, 1.0f, "analysing after one adopted pass");
  for (uint32_t k = 0; k < OMX_HRP_LV2_MAX_BANDS; k++) {
    char what[64];
    snprintf(what, sizeof(what), "slot %u freq_hz", k);
    CHECK_EQ_F(report_ports[k][0], set.report[k].freq_hz, what);
    snprintf(what, sizeof(what), "slot %u depth_db", k);
    CHECK_EQ_F(report_ports[k][1], set.report[k].depth_db, what);
    snprintf(what, sizeof(what), "slot %u q", k);
    CHECK_EQ_F(report_ports[k][2], set.report[k].q, what);
    snprintf(what, sizeof(what), "slot %u on", k);
    CHECK_EQ_F(report_ports[k][3], set.report[k].on, what);
  }

  d->deactivate(inst);
  d->cleanup(inst);

  if (g_failed) {
    fprintf(stderr, "RED hrp_report_matches_live_set\n");
    return 1;
  }
  printf("GREEN hrp_report_matches_live_set\n");
  return 0;
}
