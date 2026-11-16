// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * reverb-return.c: the reverb's return on the plugin face (FX-7 of the remaining-FX spec): the early reflections
 * and the tail's level, the wet duck and the wet de-ess, through each BUILT face at the nine rates an audio
 * interface clocks at (OMX_ORACLE_FULL_RATES, 32 to 192 kHz), read from the audio a host sees. The kernel's own
 * oracle (omx-dsp test/fx/reverb_return.test.c) holds the return's arithmetic; this one holds that the FACE
 * carries the nine controls to it, by name (CLAP) and by symbol (LV2), so a face that held one at its default
 * would be red.
 *
 *   reverb-return clap <omx-reverb.clap>
 *   reverb-return lv2 <bundle-dir> <uri>
 *
 *   A  today      at the declared defaults (the early reflections off, the tail at 0 dB, no duck, no de-ess) the
 *                 face is omx-dsp's reverb instance resolved with the same values, memcmp-identical over a burst
 *                 and a tail: a session saved before the nine existed sounds the same (the instance at those
 *                 values is today's omx_reverb_process bit for bit, the kernel's arm A)
 *   B  early      the tail off, the early reflections at 0 dB, mix 1, both cuts off (0 is none: the wet path's
 *                 high-pass would take a tap's DC sum away, its 20 kHz low-pass delay and smear it at the high
 *                 rates) and the width full (it would put a tap on both legs): an
 *                 impulse's wet is the tap table, each tap
 *                 within ONE SAMPLE of predelay + REVERB_EARLY_TAP_MS x scale x rate / 1000 (its energy's centroid)
 *                 and its sum REVERB_EARLY_TAP_GAINS within 1e-3, even taps left and odd right, at earlySizePct
 *                 0, 50 and 100 (the scale from REVERB_EARLY_SIZE_MIN to REVERB_EARLY_SIZE_MAX), nothing else
 *                 on the wire; and earlyDb one step up from the floor puts them there, so the floor IS off
 *   C  tail       tailDb -6 dB with the early reflections off: the wet is the 0 dB wet x 10^(-6/20), within 1e-5
 *                 of its peak, sample for sample
 *   D  duck       a steady 1 kHz sine at 0.5 (-6.02 dBFS) with duck -12 dB over a -30 dBFS threshold, mix 1: the
 *                 wet, once the follower has settled, is the unducked wet x 10^(-12/20) within 0.01 dB (the static
 *                 law max(depth, min(0, T - L)) at a key 24 dB over the threshold is the depth)
 *   E  de-ess     a steady full-scale sine at deessFreqHz (30 dB over the de-ess threshold at the input; the
 *                 detector reads the WET's band, whose level the network sets per rate), mix 1: with wetDeess on
 *                 the wet's RMS is at least 6 dB under the wet with it off; 20 dB under the threshold the two wets
 *                 are bit-identical
 *
 * Every arm runs under --self-test first with its expectation sabotaged (the tap times a tenth late, the tail
 * at -5 dB, the duck at -11 dB, the de-ess threshold moved past the sine) and must go red, so a check that cannot
 * see its control is not mistaken for one that passes.
 */
#include <omxdsp/fx/omx_reverb_instance.h>
#include "omx_reverb_params.h"
#include "face-driver.h"

#define NP OMX_REVERB_PARAM_COUNT
#define BLOCK 256u
#define MAXLEN 300000u /* 1.5 s at 192 kHz */

static const float RATES[OMX_ORACLE_FULL_RATES_COUNT] = OMX_ORACLE_FULL_RATES_INIT;
/* The declarations, stated here so a moved one turns arm B red. */
static const double TAP_MS[OMX_REVERB_EARLY_TAPS] = {3.1, 5.3, 7.9, 10.7, 13.9, 17.3, 21.1, 25.3};
static const double TAP_G[OMX_REVERB_EARLY_TAPS] = {0.84, 0.75, 0.67, 0.6, 0.53, 0.47, 0.42, 0.37};
#define EARLY_SIZE_MIN 0.5
#define EARLY_SIZE_MAX 1.5

static const struct face_spec SPEC = {OMX_REVERB_PARAMS, NP, NULL, "reverb-return"};
static float g_il[MAXLEN], g_ir[MAXLEN], g_ol[MAXLEN], g_or[MAXLEN], g_wl[MAXLEN], g_wr[MAXLEN];
static bool g_sabotage = false;

static void defaults(float *v) {
  for (uint32_t i = 0; i < NP; i++) v[i] = OMX_REVERB_PARAMS[i].def;
}

/* The whole run through one fresh face in BLOCK-frame pieces, into g_ol/g_or. */
static bool run_face(const Face *f, const float *v, uint32_t n) {
  Inst *in = f->open(f->ctx, g_sr);
  if (!in) return false;
  bool good = true;
  for (uint32_t at = 0; good && at < n; at += BLOCK) {
    const uint32_t k = n - at < BLOCK ? n - at : BLOCK;
    good = f->run(in, v, false, NULL, g_il + at, g_ir + at, g_ol + at, g_or + at, k);
  }
  f->close(in);
  return good;
}

/* omx-dsp's reverb instance at the same values, into g_wl/g_wr. */
static void reference(const float *v, uint32_t n) {
  static OmxReverbInstance s;
  omx_reverb_instance_init(&s, (float)g_sr);
  for (uint32_t at = 0; at < n; at += BLOCK) {
    const uint32_t k = n - at < BLOCK ? n - at : BLOCK;
    omx_reverb_instance_resolve(
        &s, 0, v[OMX_REVERB_PARAM_PLATE_MOD_DEPTH], v[OMX_REVERB_PARAM_MIX], v[OMX_REVERB_PARAM_SIZE],
        v[OMX_REVERB_PARAM_DAMPING], v[OMX_REVERB_PARAM_WIDTH], v[OMX_REVERB_PARAM_PREDELAY], v[OMX_REVERB_PARAM_LOWCUT],
        v[OMX_REVERB_PARAM_HIGHCUT], v[OMX_REVERB_PARAM_REVERSE], v[OMX_REVERB_PARAM_HOLD], v[OMX_REVERB_PARAM_RELEASE],
        v[OMX_REVERB_PARAM_GATE_THRESHOLD], (int)lrintf(v[OMX_REVERB_PARAM_ALGORITHM]), v[OMX_REVERB_PARAM_EARLY_DB],
        v[OMX_REVERB_PARAM_TAIL_DB], v[OMX_REVERB_PARAM_EARLY_SIZE_PCT], v[OMX_REVERB_PARAM_DUCK_DB],
        v[OMX_REVERB_PARAM_DUCK_THRESHOLD_DB], v[OMX_REVERB_PARAM_DUCK_RELEASE_MS], (int)lrintf(v[OMX_REVERB_PARAM_WET_DEESS]),
        v[OMX_REVERB_PARAM_DEESS_FREQ_HZ], v[OMX_REVERB_PARAM_DEESS_THRESHOLD_DB]);
    memcpy(g_wl + at, g_il + at, k * sizeof(float));
    memcpy(g_wr + at, g_ir + at, k * sizeof(float));
    omx_reverb_instance_run(&s, g_wl + at, g_wr + at, g_wl + at, g_wr + at, k);
  }
}

static uint32_t at_ms(double ms) { return (uint32_t)(ms * g_sr / 1000.0); }

static void impulse(uint32_t n) {
  memset(g_il, 0, n * sizeof(float));
  memset(g_ir, 0, n * sizeof(float));
  g_il[0] = g_ir[0] = 1.0f;
}

static void sine(uint32_t n, double hz, double peak) {
  for (uint32_t i = 0; i < n; i++) g_il[i] = g_ir[i] = (float)(peak * sin(2.0 * M_PI * hz * (double)i / g_sr));
}

/* A burst then a tail: 300 ms of a sine, then silence. */
static void burst(uint32_t n) {
  sine(n, 440.0, 0.5);
  for (uint32_t i = at_ms(300.0); i < n; i++) g_il[i] = g_ir[i] = 0.0f;
}

static double rms(const float *x, uint32_t from, uint32_t to) {
  double e = 0.0;
  for (uint32_t i = from; i < to; i++) e += (double)x[i] * x[i];
  return sqrt(e / (double)(to - from));
}

static double db(double lin) { return 20.0 * log10(lin); }

/* The energy centroid of |x| over [from, to), and the signed sum there. */
static double centroid(const float *x, uint32_t from, uint32_t to, double *sum) {
  double w = 0.0, m = 0.0, s = 0.0;
  for (uint32_t i = from; i < to; i++) w += fabs(x[i]), m += fabs(x[i]) * (double)i, s += x[i];
  *sum = s;
  return w > 0.0 ? m / w : -1.0;
}

static void arm_today(const Face *f) {
  float v[NP];
  defaults(v);
  const uint32_t n = at_ms(900.0);
  burst(n);
  face_ok(run_face(f, v, n), "A today: the face runs at the defaults");
  reference(v, n);
  face_ok(memcmp(g_ol, g_wl, n * sizeof(float)) == 0 && memcmp(g_or, g_wr, n * sizeof(float)) == 0,
          "A today: at the defaults the face is the instance resolved alike, memcmp-identical");
}

static void arm_early(const Face *f) {
  float v[NP];
  const double pcts[3] = {0.0, 50.0, 100.0};
  for (int s = 0; s < 3; s++) {
    defaults(v);
    v[OMX_REVERB_PARAM_MIX] = 1.0f;
    v[OMX_REVERB_PARAM_PREDELAY] = 0.0f;
    v[OMX_REVERB_PARAM_LOWCUT] = 0.0f;  /* the wet's high-pass off: a tap's DC sum is its gain */
    v[OMX_REVERB_PARAM_HIGHCUT] = 0.0f; /* and its low-pass off (0 is none): the taps are measured, not the filter's tail */
    v[OMX_REVERB_PARAM_WIDTH] = 1.0f;  /* each tap on its own leg */
    v[OMX_REVERB_PARAM_TAIL_DB] = OMX_REVERB_PARAM_TAIL_DB_MIN; /* the tail off */
    v[OMX_REVERB_PARAM_EARLY_DB] = 0.0f;
    v[OMX_REVERB_PARAM_EARLY_SIZE_PCT] = (float)pcts[s];
    const double scale = EARLY_SIZE_MIN + (EARLY_SIZE_MAX - EARLY_SIZE_MIN) * pcts[s] / 100.0;
    const uint32_t n = at_ms(60.0);
    impulse(n);
    char what[128];
    snprintf(what, sizeof what, "B early: the face runs at earlySizePct %.0f", pcts[s]);
    face_ok(run_face(f, v, n), what);
    double stray = 0.0;
    for (uint32_t t = 0; t < OMX_REVERB_EARLY_TAPS; t++) {
      const double want = TAP_MS[t] * scale * g_sr / 1000.0 * (g_sabotage ? 1.1 : 1.0);
      const float *leg = (t & 1u) ? g_or : g_ol, *other = (t & 1u) ? g_ol : g_or;
      const uint32_t lo = (uint32_t)(want - 6.0 < 0.0 ? 0.0 : want - 6.0), hi = (uint32_t)(want + 7.0);
      double sum = 0.0;
      const double c = centroid(leg, lo, hi, &sum);
      snprintf(what, sizeof what, "B early: tap %u at size %.0f within one sample of its time", t, pcts[s]);
      face_ok(c >= 0.0 && fabs(c - want) <= 1.0, what);
      snprintf(what, sizeof what, "B early: tap %u at size %.0f sums to its table gain", t, pcts[s]);
      face_ok(fabs(sum - TAP_G[t]) <= 1e-3, what);
      double os = 0.0;
      centroid(other, lo, hi, &os);
      stray += fabs(os);
    }
    face_ok(stray <= 1e-6, "B early: each tap on its own leg, nothing on the other");
    /* and the floor is off: one step up from it the taps appear, at the floor the wet is silent */
    v[OMX_REVERB_PARAM_EARLY_DB] = OMX_REVERB_PARAM_EARLY_DB_MIN;
    face_ok(run_face(f, v, n), "B early: the face runs at the early floor");
    face_ok(rms(g_ol, 0, n) == 0.0 && rms(g_or, 0, n) == 0.0, "B early: at the floor, with the tail off, the wet is silent");
    v[OMX_REVERB_PARAM_EARLY_DB] = OMX_REVERB_PARAM_EARLY_DB_MIN + OMX_REVERB_EARLY_DB_RANGE_STEP;
    face_ok(run_face(f, v, n), "B early: the face runs one step over the early floor");
    face_ok(rms(g_ol, 0, n) > 0.0, "B early: one step over the floor the reflections are there");
  }
}

static void arm_tail(const Face *f) {
  float v[NP];
  defaults(v);
  v[OMX_REVERB_PARAM_MIX] = 1.0f;
  const uint32_t n = at_ms(600.0);
  burst(n);
  face_ok(run_face(f, v, n), "C tail: the face runs at 0 dB");
  memcpy(g_wl, g_ol, n * sizeof(float));
  memcpy(g_wr, g_or, n * sizeof(float));
  float peak = 0.0f;
  for (uint32_t i = 0; i < n; i++) peak = fmaxf(peak, fmaxf(fabsf(g_wl[i]), fabsf(g_wr[i])));
  v[OMX_REVERB_PARAM_TAIL_DB] = -6.0f;
  face_ok(run_face(f, v, n), "C tail: the face runs at -6 dB");
  const double g = pow(10.0, (g_sabotage ? -5.0 : -6.0) / 20.0);
  double worst = 0.0;
  for (uint32_t i = 0; i < n; i++) worst = fmax(worst, fmax(fabs(g_ol[i] - g * g_wl[i]), fabs(g_or[i] - g * g_wr[i])));
  face_ok(peak > 0.0f && worst <= 1e-5 * peak, "C tail: at -6 dB the wet is the 0 dB wet x 10^(-6/20), within 1e-5 of its peak");
}

static void arm_duck(const Face *f) {
  float v[NP];
  defaults(v);
  v[OMX_REVERB_PARAM_MIX] = 1.0f;
  const uint32_t n = at_ms(1500.0);
  sine(n, 1000.0, 0.5);
  face_ok(run_face(f, v, n), "D duck: the face runs unducked");
  memcpy(g_wl, g_ol, n * sizeof(float));
  memcpy(g_wr, g_or, n * sizeof(float));
  v[OMX_REVERB_PARAM_DUCK_DB] = -12.0f;
  v[OMX_REVERB_PARAM_DUCK_THRESHOLD_DB] = -30.0f;
  face_ok(run_face(f, v, n), "D duck: the face runs ducked");
  /* the last 300 ms: the key is 24 dB over the threshold, so the gain is the depth */
  const uint32_t from = at_ms(1200.0);
  const double ratio = db(rms(g_ol, from, n) / rms(g_wl, from, n)), want = g_sabotage ? -11.0 : -12.0;
  face_ok(fabs(ratio - want) <= 0.01, "D duck: the ducked wet settles 12 dB under the unducked, within 0.01 dB");
}

static void arm_deess(const Face *f) {
  float v[NP];
  defaults(v);
  v[OMX_REVERB_PARAM_MIX] = 1.0f;
  const double hz = (double)OMX_REVERB_PARAM_DEESS_FREQ_HZ_DEFAULT, thr = (double)OMX_REVERB_PARAM_DEESS_THRESHOLD_DB_DEFAULT;
  const uint32_t n = at_ms(1200.0), from = at_ms(900.0);
  /* full scale: 30 dB over the threshold at the input, the wet's band well over it at every rate */
  sine(n, hz, 1.0);
  face_ok(run_face(f, v, n), "E de-ess: the face runs with the de-ess off");
  const double off = rms(g_ol, from, n);
  v[OMX_REVERB_PARAM_WET_DEESS] = 1.0f;
  v[OMX_REVERB_PARAM_DEESS_THRESHOLD_DB] = g_sabotage ? (float)(thr + 40.0 > 0.0 ? 0.0 : thr + 40.0) : (float)thr;
  face_ok(run_face(f, v, n), "E de-ess: the face runs with the de-ess on");
  const double on = rms(g_ol, from, n);
  char what[160];
  snprintf(what, sizeof what, "E de-ess: a full-scale sine in the band, the de-ess cuts the wet by 6 dB or more (measured %.2f dB)", off > 0.0 ? db(on / off) : 0.0);
  face_ok(off > 0.0 && db(on / off) <= -6.0, what);
  /* 20 dB under the threshold: untouched */
  sine(n, hz, pow(10.0, (thr - 20.0) / 20.0));
  v[OMX_REVERB_PARAM_DEESS_THRESHOLD_DB] = (float)thr;
  face_ok(run_face(f, v, n), "E de-ess: the face runs quiet with the de-ess on");
  memcpy(g_wl, g_ol, n * sizeof(float));
  v[OMX_REVERB_PARAM_WET_DEESS] = 0.0f;
  face_ok(run_face(f, v, n), "E de-ess: the face runs quiet with the de-ess off");
  face_ok(memcmp(g_ol, g_wl, n * sizeof(float)) == 0, "E de-ess: 20 dB under the threshold the wet is itself, bit for bit");
}

static void arms(const Face *f) {
  for (uint32_t r = 0; r < OMX_ORACLE_FULL_RATES_COUNT; r++) {
    g_sr = RATES[r];
    arm_today(f);
    arm_early(f);
    arm_tail(f);
    arm_duck(f);
    arm_deess(f);
  }
}

/* --self-test: every arm with its expectation sabotaged must go red, at one rate. */
static void self_test(const Face *f) {
  const int before = g_failed;
  printf("self-test: the FAIL lines up to its verdict are the sabotaged expectations, and must appear\n");
  g_sabotage = true;
  g_sr = RATES[0];
  arm_early(f);
  arm_tail(f);
  arm_duck(f);
  arm_deess(f);
  g_sabotage = false;
  const int red = g_failed - before;
  g_failed = before;
  printf("self-test: %d sabotaged checks went red, as they must\n", red);
  face_ok(red >= 4, "self-test: the sabotaged expectations (taps late, tail -5, duck -11, de-ess past the sine) go red");
}

static void arms_with_self_test(const Face *f) {
  self_test(f);
  arms(f);
}

int main(int argc, char **argv) { return face_main(argc, argv, &SPEC, arms_with_self_test); }
