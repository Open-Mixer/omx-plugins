// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * Oracle (spec 7.7 Proof): omx-fbs's report is equal to its live set, and a host with no worker
 * gets the audio untouched. A crafted set is adopted through the real LV2 work_response() callback
 * with a distinct value in every word of every slot, so a transposed field or a slot off by one
 * cannot pass by accident; the four read-only ports per slot and `analysing` must then read exactly
 * what omx_fbs_lv2_report() reads off the live instance. Then a second pass installs one enabled
 * notch and the audio must change (the cascade reaches the output) while the report stays the set's.
 */
#include <assert.h>
#include <lv2/core/lv2.h>
#include <lv2/worker/worker.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <omxdsp/analysis/omx_fbs_lv2.h>

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
  FBS_PORT_REPORT_BASE,
};

extern const LV2_Descriptor *lv2_descriptor(uint32_t index);

static int g_failed = 0;
#define CHECK_EQ_F(got, want, what)                                                                        \
  do {                                                                                                     \
    if ((got) != (want)) {                                                                                 \
      fprintf(stderr, "FAIL %s: got %f want %f\n", (what), (double)(got), (double)(want));                 \
      g_failed = 1;                                                                                        \
    }                                                                                                       \
  } while (0)

#define N 512

int main(void) {
  const LV2_Descriptor *d = lv2_descriptor(0);
  assert(d && lv2_descriptor(1) == NULL);
  assert(strcmp(d->URI, "urn:openmixer:fbs") == 0);

  LV2_Handle inst = d->instantiate(d, 48000.0, ".", NULL);
  assert(inst != NULL);

  float in[N], out[N];
  for (uint32_t i = 0; i < N; i++) in[i] = 0.5f * sinf(6.2831853f * 1000.0f * (float)i / 48000.0f);
  memset(out, 0, sizeof(out));
  float mode = (float)OMX_FBS_MODES_CUT, pool = (float)OMX_FBS_POOLS_LIVE, hold = (float)OMX_FBS_DEFAULT_HOLD_SECONDS;
  float bands = (float)OMX_FBS_DEFAULT_MAX_AUTO_BANDS, merge = (float)OMX_FBS_DEFAULT_MERGE_CENTS;
  float cooldown = (float)OMX_FBS_DEFAULT_COOLDOWN_SECONDS, analysing = -1.0f;
  float report_ports[OMX_FBS_LV2_SLOTS][4];
  memset(report_ports, 0, sizeof(report_ports));

  d->connect_port(inst, FBS_PORT_IN, in);
  d->connect_port(inst, FBS_PORT_OUT, out);
  d->connect_port(inst, FBS_PORT_MODE, &mode);
  d->connect_port(inst, FBS_PORT_POOL, &pool);
  d->connect_port(inst, FBS_PORT_HOLD, &hold);
  d->connect_port(inst, FBS_PORT_BANDS, &bands);
  d->connect_port(inst, FBS_PORT_MERGE, &merge);
  d->connect_port(inst, FBS_PORT_COOLDOWN, &cooldown);
  d->connect_port(inst, FBS_PORT_ANALYSING, &analysing);
  for (uint32_t k = 0; k < OMX_FBS_LV2_SLOTS; k++)
    for (uint32_t w = 0; w < 4u; w++) d->connect_port(inst, FBS_PORT_REPORT_BASE + 4u * k + w, &report_ports[k][w]);

  d->activate(inst);
  d->run(inst, N);

  /* no worker answered: the audio is bit-exact, analysing is 0, every slot free */
  CHECK_EQ_F(analysing, 0.0f, "analysing before any pass");
  CHECK_EQ_F(memcmp(in, out, sizeof(in)), 0, "no worker: audio untouched (memcmp)");
  for (uint32_t k = 0; k < OMX_FBS_LV2_SLOTS; k++) CHECK_EQ_F(report_ports[k][3], 0.0f, "on before any pass");

  const LV2_Worker_Interface *worker = (const LV2_Worker_Interface *)d->extension_data(LV2_WORKER__interface);
  assert(worker != NULL);

  static OmxFbsLv2Set set;
  memset(&set, 0, sizeof(set));
  for (uint32_t k = 0; k < OMX_FBS_LV2_SLOTS; k++) {
    OmxBandReport r;
    r.freq_hz = 100.0f + 10.0f * (float)k;
    r.depth_db = -(1.0f + (float)k);
    r.q = 5.0f + (float)k;
    r.on = 1.0f;
    r.origin = (float)OMX_CURVE_ORIGINS_FBS_LIVE;
    r.shape = (float)OMX_CURVE_SHAPES_BELL;
    set.report[k] = r;
  }
  assert(worker->work_response(inst, (uint32_t)sizeof(set), &set) == LV2_WORKER_SUCCESS);
  /* a malformed response is refused, not adopted */
  assert(worker->work_response(inst, 3u, &set) != LV2_WORKER_SUCCESS);

  d->run(inst, N);

  CHECK_EQ_F(analysing, 1.0f, "analysing after one adopted pass");
  for (uint32_t k = 0; k < OMX_FBS_LV2_SLOTS; k++) {
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

  /* an enabled notch at the tone, adopted: cut mode applies it to the audio ... */
  static OmxFbsLv2Set cutset;
  memset(&cutset, 0, sizeof(cutset));
  omx_live_report_clear(cutset.report, OMX_FBS_LV2_SLOTS);
  omx_eq_design_f(OMX_EQ_NOTCH, 1000.0, 30.0, 0.0, 48000.0, cutset.coeffs[0]);
  cutset.enabled[0] = 1u;
  cutset.report[0].freq_hz = 1000.0f;
  cutset.report[0].q = 30.0f;
  cutset.report[0].on = 1.0f;
  assert(worker->work_response(inst, (uint32_t)sizeof(cutset), &cutset) == LV2_WORKER_SUCCESS);
  /* a continuous tone across the blocks (a restarted phase would excite the notch's transient), 40 blocks to settle */
  for (uint32_t pass = 0; pass < 40u; pass++) {
    for (uint32_t i = 0; i < N; i++) in[i] = 0.5f * sinf(6.2831853f * 1000.0f * (float)(pass * N + i) / 48000.0f);
    d->run(inst, N);
  }
  double e_in = 0.0, e_out = 0.0;
  for (uint32_t i = N / 2; i < N; i++) {
    e_in += (double)in[i] * (double)in[i];
    e_out += (double)out[i] * (double)out[i];
  }
  if (!(e_out < 0.01 * e_in)) {
    fprintf(stderr, "FAIL cut mode: energy out %g not under 1%% of in %g\n", e_out, e_in);
    g_failed = 1;
  }
  CHECK_EQ_F(report_ports[0][0], 1000.0f, "cut set: slot 0 freq");

  /* ... and report mode keeps the audio bit-exact while the report stays the set's */
  mode = (float)OMX_FBS_MODES_REPORT;
  d->run(inst, N);
  CHECK_EQ_F(memcmp(in, out, sizeof(in)), 0, "report mode: audio untouched (memcmp)");
  CHECK_EQ_F(report_ports[0][0], 1000.0f, "report mode: slot 0 freq kept");

  d->deactivate(inst);
  d->cleanup(inst);

  if (g_failed) {
    fprintf(stderr, "RED fbs_report_matches_live_set\n");
    return 1;
  }
  printf("GREEN fbs_report_matches_live_set\n");
  return 0;
}
