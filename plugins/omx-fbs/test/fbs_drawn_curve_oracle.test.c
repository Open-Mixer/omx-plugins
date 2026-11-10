// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * Oracle: omx-fbs's drawn curve is within 0.5 dB of the measured response, at 48 and 96 kHz
 * (omx-fbs/README.md, "Not yet done"). Three live bells are adopted through the real LV2
 * work_response() callback, cut mode runs them on the audio, and at each band's three anchors
 * (omx_eq_band_anchors: the lower edge, the centre, the upper edge) two numbers are compared:
 *
 *   drawn    omx_eq_magnitude_sum_db() over bands built from the report's own words (freq_hz,
 *            depth_db, q, shape) -- exactly what a host draws for the eq_response display
 *            (omx_curve.h), read off the ports a host actually has.
 *   measured a single-bin DFT of the real run() output over a settled tone at that frequency,
 *            against the same tone's input bin, in dB -- the cascade's actual gain, in audio.
 *
 * `--self-test`: the drawn side is evaluated at a frequency shifted off every band (sabotage),
 * which must then disagree with the measured side by more than the tolerance -- proof the
 * comparison can fail, not a tautology.
 */
#include <assert.h>
#include <lv2/core/lv2.h>
#include <lv2/worker/worker.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <omxdsp/analysis/omx_fbs_lv2.h>
#include <omxdsp/omx_curve.h>
#include <omxdsp/omx_magnitude.h>

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

#define PROBE_BANDS 3u
#define TOLERANCE_DB 0.5
#define CONTROL_HZ 15000.0

struct probe_band {
  double freq_hz, gain_db, q;
};
static const struct probe_band BANDS[PROBE_BANDS] = {
    {300.0, -6.0, 2.0},
    {1500.0, 4.0, 3.0},
    {6000.0, -3.0, 1.5},
};

static int g_failed = 0;
static int g_sabotage = 0;

static double dft_mag(const float *x, uint32_t n, double hz, double rate) {
  double re = 0.0, im = 0.0;
  for (uint32_t i = 0; i < n; i++) {
    const double ph = 2.0 * M_PI * hz * (double)i / rate;
    re += (double)x[i] * cos(ph);
    im += (double)x[i] * sin(ph);
  }
  return 2.0 * hypot(re, im) / (double)n;
}

static double db_of(double amp) { return 20.0 * log10(amp > 1e-9 ? amp : 1e-9); }

/* Feeds a settled tone at hz through the instance (already adopted), in blocks, and returns the
 * measured gain in dB: the output's DFT bin over the input's, at hz, over one window after a
 * settle -- a whole number of cycles in both (window == rate: a 1 Hz bin spacing). */
static double measure_gain_db(const LV2_Descriptor *d, LV2_Handle inst, float *in_port, float *out_port,
                              double rate, double hz, uint32_t settle, uint32_t window, uint32_t block) {
  const uint32_t total = settle + window;
  float *in = (float *)malloc(total * sizeof(float));
  float *out = (float *)malloc(total * sizeof(float));
  assert(in && out);
  for (uint32_t i = 0; i < total; i++) in[i] = (float)(0.25 * sin(2.0 * M_PI * hz * (double)i / rate));
  for (uint32_t at = 0; at < total; at += block) {
    const uint32_t f = total - at < block ? total - at : block;
    memcpy(in_port, in + at, f * sizeof(float));
    d->run(inst, f);
    memcpy(out + at, out_port, f * sizeof(float));
  }
  const double in_mag = dft_mag(in + settle, window, hz, rate);
  const double out_mag = dft_mag(out + settle, window, hz, rate);
  free(in);
  free(out);
  return db_of(out_mag) - db_of(in_mag);
}

/* The analytic drawn curve at hz: the sum of every live band's omx_eq_band_magnitude_db(), built
 * from the report's own words -- the production path (omx_curve.h) a host draws eq_response with.
 * `freq_scale` is 1.0 except in the sabotage arm, which reads every band off its centre. */
static double drawn_curve_db(const OmxBandReport report[], uint32_t n, double hz, double rate, double freq_scale) {
  struct omx_eq_band bands[PROBE_BANDS];
  uint32_t used = 0;
  for (uint32_t k = 0; k < n && used < PROBE_BANDS; k++) {
    if (!(report[k].on > 0.5f)) continue;
    const int kind = omx_curve_shape_kind((int)report[k].shape);
    if (kind < 0) continue;
    bands[used].kind = (enum omx_eq_kind)kind;
    bands[used].freq_hz = (double)report[k].freq_hz * freq_scale;
    bands[used].gain_db = (double)report[k].depth_db;
    bands[used].q = (double)report[k].q;
    bands[used].on = 1;
    used++;
  }
  return omx_eq_magnitude_sum_db(bands, used, hz, rate, OMX_CURVE_EXTENTS_EQ_FLOOR_DB);
}

static void check_point(double rate_hz, double hz, double drawn, double measured, const char *what) {
  const double diff = fabs(drawn - measured);
  const int red = diff > TOLERANCE_DB;
  if (g_sabotage) {
    if (!red) {
      fprintf(stderr, "FAIL self-test %s @ %.0f Hz (%.0f): sabotage did not disagree (drawn %.2f measured %.2f)\n",
              what, rate_hz, hz, drawn, measured);
      g_failed = 1;
    }
    return;
  }
  if (red) {
    fprintf(stderr, "FAIL %s @ %.0f Hz (%.0f): drawn %.2f dB measured %.2f dB diff %.2f > %.1f\n", what, rate_hz, hz,
            drawn, measured, diff, TOLERANCE_DB);
    g_failed = 1;
  }
}

static void run_rate(double rate) {
  const LV2_Descriptor *d = lv2_descriptor(0);
  LV2_Handle inst = d->instantiate(d, rate, ".", NULL);
  assert(inst != NULL);

  static float in_buf[512], out_buf[512];
  float mode = (float)OMX_FBS_MODES_CUT, pool = (float)OMX_FBS_POOLS_LIVE;
  float hold = (float)OMX_FBS_DEFAULT_HOLD_SECONDS, bands = (float)OMX_FBS_DEFAULT_MAX_AUTO_BANDS;
  float merge = (float)OMX_FBS_DEFAULT_MERGE_CENTS, cooldown = (float)OMX_FBS_DEFAULT_COOLDOWN_SECONDS;
  float analysing = -1.0f;
  static float report_ports[OMX_FBS_LV2_SLOTS][4];
  memset(report_ports, 0, sizeof(report_ports));
  memset(in_buf, 0, sizeof(in_buf));
  memset(out_buf, 0, sizeof(out_buf));

  d->connect_port(inst, FBS_PORT_IN, in_buf);
  d->connect_port(inst, FBS_PORT_OUT, out_buf);
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

  const LV2_Worker_Interface *worker = (const LV2_Worker_Interface *)d->extension_data(LV2_WORKER__interface);
  assert(worker != NULL);

  static OmxFbsLv2Set set;
  memset(&set, 0, sizeof(set));
  omx_live_report_clear(set.report, OMX_FBS_LV2_SLOTS);
  OmxBandReport report[PROBE_BANDS];
  for (uint32_t k = 0; k < PROBE_BANDS; k++) {
    omx_eq_design_f(OMX_EQ_PEAKING, BANDS[k].freq_hz, BANDS[k].q, BANDS[k].gain_db, rate, set.coeffs[k]);
    set.enabled[k] = 1u;
    OmxBandReport r;
    r.freq_hz = (float)BANDS[k].freq_hz;
    r.depth_db = (float)BANDS[k].gain_db;
    r.q = (float)BANDS[k].q;
    r.on = 1.0f;
    r.origin = (float)OMX_CURVE_ORIGINS_FBS_LIVE;
    r.shape = (float)OMX_CURVE_SHAPES_BELL;
    set.report[k] = r;
    report[k] = r;
  }
  assert(worker->work_response(inst, (uint32_t)sizeof(set), &set) == LV2_WORKER_SUCCESS);

  const uint32_t settle = (uint32_t)rate, window = (uint32_t)rate, block = 128u;
  const double freq_scale = g_sabotage ? 1.37 : 1.0;

  /* The sabotage (a shifted analytic centre) only has to disagree where the curve actually moves:
   * at each band's own centre, not at its flatter anchors or at the control point, both close to
   * 0 dB on every band in play here, where a shifted band can land back near its true value by
   * coincidence. The real oracle (not self-test) still draws and measures the whole curve. */
  for (uint32_t k = 0; k < PROBE_BANDS; k++) {
    double anchors[3];
    omx_eq_band_anchors(BANDS[k].freq_hz, BANDS[k].q, anchors);
    for (uint32_t a = 0; a < 3; a++) {
      if (g_sabotage && a != 1u) continue;
      const double hz = anchors[a];
      const double measured = measure_gain_db(d, inst, in_buf, out_buf, rate, hz, settle, window, block);
      const double drawn = drawn_curve_db(report, PROBE_BANDS, hz, rate, freq_scale);
      char what[64];
      snprintf(what, sizeof(what), "band %u anchor %u", k, a);
      check_point(rate, hz, drawn, measured, what);
    }
  }
  if (!g_sabotage) {
    const double measured = measure_gain_db(d, inst, in_buf, out_buf, rate, CONTROL_HZ, settle, window, block);
    const double drawn = drawn_curve_db(report, PROBE_BANDS, CONTROL_HZ, rate, freq_scale);
    check_point(rate, CONTROL_HZ, drawn, measured, "control point");
  }

  d->deactivate(inst);
  d->cleanup(inst);
}

int main(int argc, char **argv) {
  g_sabotage = argc > 1 && strcmp(argv[1], "--self-test") == 0;

  run_rate(48000.0);
  run_rate(96000.0);

  if (g_failed) {
    fprintf(stderr, "RED fbs_drawn_curve_oracle%s\n", g_sabotage ? " self-test" : "");
    return 1;
  }
  printf("GREEN fbs_drawn_curve_oracle%s\n", g_sabotage ? " self-test" : "");
  return 0;
}
