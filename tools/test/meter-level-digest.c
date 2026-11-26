// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * meter-level-digest.c — the clap.meters/1 values of a built face: a minimal CLAP host runs the face over
 * a fixed stimulus (silence, dc and noise at levels from 0 dBFS to far below any meter's minimum, sines,
 * impulses, a NaN and an infinite sample, blocks of 1 to 300 frames) and, after every block, writes every
 * value of every LEVEL meter the face measures (two channels) to stdout as text, one line per value,
 * `<block> <meter> <value>`, %.17g. tools/test/meter-values-check.mjs compares it against
 * tools/test/<face>.meter-level.txt within the bound `--tolerance` prints. A level meter is a peak and a
 * log10 in double, but what it reads went through the kernel's gain curve, whose log10f and powf omx-dsp may run
 * through libmvec: the readings are not bit-identical between a build with libmvec and one without.
 *
 * `--tolerance` prints, in dB, the largest move of a reading that omx-dsp's fast-maths bound allows
 * (omx_fastmath.h): a level taken to dB by log10f, off by OMX_FM_LOG10F_MAX_ULP float ULP at the largest
 * magnitude the library takes it over (the log10 of OMX_LIN_TO_DB_FLOOR_LIN), and a gain taken back to linear
 * by powf, off by OMX_FM_POWF_MAX_ULP ULP.
 *
 * `--input-gain-db <x>` scales the stimulus by x dB: a real level error, for the sabotage arm of `make meter-level`.
 *
 * `--others <file>` writes every other meter's values (a gain reduction or a level the kernel reads: its own reading, in
 * float through libm's float functions, whose last bit differs between C libraries) to <file> as text,
 * one line per value, `<block> <meter> <value>`, %.9g. tools/test/meter-values-check.mjs compares that
 * file against tools/test/<face>.meter-others.txt within a tolerance. A face with no level meter (a limiter's, a
 * gate's: a gain reduction only) is read with --others alone. Used by `make meter-level`.
 */
#include <dlfcn.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <omx-clap-host/clap_meters.h>
#include <omxdsp/omx_fastmath.h>
#include <omxdsp/omxdsp_definitions.h>

#include "clap/entry.h"
#include "clap/factory/plugin-factory.h"
#include "clap/host.h"
#include "clap/process.h"

#define RATE 48000.0
#define MAXBLOCK 300u

static const void *host_get_extension(const clap_host_t *h, const char *id) { (void)h, (void)id; return NULL; }
static void host_noop(const clap_host_t *h) { (void)h; }
static const clap_host_t HOST = {CLAP_VERSION_INIT, NULL, "omx meter-level-digest", "openmixer", "", "0.1",
                                 host_get_extension, host_noop, host_noop, host_noop};

static uint32_t ev_size(const clap_input_events_t *l) { (void)l; return 0; }
static const clap_event_header_t *ev_get(const clap_input_events_t *l, uint32_t i) { (void)l, (void)i; return NULL; }
static bool ev_push(const clap_output_events_t *l, const clap_event_header_t *e) { (void)l, (void)e; return true; }

/** dB: the bound of a reading's move through a gain curve, from omx-dsp's fast-maths bound. */
static double tolerance_db(void) {
  const double log10_ulp = ldexp(1.0, ilogb(-log10(OMX_LIN_TO_DB_FLOOR_LIN_DOUBLE)) - (FLT_MANT_DIG - 1));
  return 20.0 * OMX_FM_LOG10F_MAX_ULP * log10_ulp + 20.0 * log10(1.0 + OMX_FM_POWF_MAX_ULP * FLT_EPSILON);
}

static double g_input_gain = 1.0;
static uint32_t g_seed = 0x2545F491u;
static float noise(void) {
  g_seed = g_seed * 1664525u + 1013904223u;
  return (float)(g_seed >> 8) / 16777216.0f - 0.5f;
}

/** The stimulus of block `b`: a kind, a level and a length, all functions of `b`. */
static void fill(uint32_t b, uint32_t n, float *l, float *r) {
  static const double db[] = {0.0, -3.0, -12.0, -30.0, -45.0, -59.0, -61.0, -80.0, -120.0, -200.0, 6.0};
  const double amp = g_input_gain * pow(10.0, db[(b / 7u) % 11u] / 20.0);
  const uint32_t kind = b % 7u;
  for (uint32_t i = 0; i < n; i++) {
    float a = 0.0f, c = 0.0f;
    switch (kind) {
      case 0: break;                                                  /* silence */
      case 1: a = c = (float)amp; break;                              /* dc */
      case 2: a = (float)(amp * 2.0 * noise()); c = (float)(amp * 1.5 * noise()); break;
      case 3: a = (float)(amp * sin(2.0 * M_PI * 997.0 * (double)i / RATE)); c = -a * 0.5f; break;
      case 4: a = i == n / 2u ? (float)amp : 0.0f; c = i == 0 ? -(float)amp : 0.0f; break;
      case 5: a = (float)(amp * noise()); c = 0.0f; break;            /* one leg silent */
      default: a = (float)(amp * 2.0 * noise()); c = (float)(amp * 2.0 * noise()); break;
    }
    l[i] = a;
    r[i] = c;
  }
  if (b % 53u == 52u && n > 2u) l[1] = NAN;
  if (b % 61u == 60u && n > 3u) r[2] = INFINITY;
  if (b % 59u == 58u && n > 3u) l[3] = -INFINITY;
}

int main(int argc, char **argv) {
  if (argc == 2 && strcmp(argv[1], "--tolerance") == 0) return printf("%.3g\n", tolerance_db()), 0;
  FILE *others = NULL;
  const char *others_path = NULL;
  for (int a = 2; a < argc; a++) {
    if (strcmp(argv[a], "--others") == 0 && a + 1 < argc) {
      others_path = argv[++a];
    } else if (strcmp(argv[a], "--input-gain-db") == 0 && a + 1 < argc) {
      g_input_gain = pow(10.0, strtod(argv[++a], NULL) / 20.0);
    } else {
      argc = 0;
    }
  }
  if (argc < 2)
    return fprintf(stderr, "usage: meter-level-digest <x.clap> [--others <file>] [--input-gain-db <x>] | --tolerance\n"), 2;
  if (others_path && !(others = fopen(others_path, "w"))) return fprintf(stderr, "open %s\n", others_path), 2;
  void *so = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
  if (!so) return fprintf(stderr, "dlopen %s: %s\n", argv[1], dlerror()), 1;
  const clap_plugin_entry_t *entry = (const clap_plugin_entry_t *)dlsym(so, "clap_entry");
  if (!entry || !entry->init(argv[1])) return fprintf(stderr, "no clap_entry\n"), 1;
  const clap_plugin_factory_t *fac = (const clap_plugin_factory_t *)entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  if (!fac || fac->get_plugin_count(fac) != 1) return fprintf(stderr, "factory\n"), 1;
  const clap_plugin_t *p = fac->create_plugin(fac, &HOST, fac->get_plugin_descriptor(fac, 0)->id);
  if (!p || !p->init(p)) return fprintf(stderr, "create/init\n"), 1;
  const clap_plugin_meters_t *m = (const clap_plugin_meters_t *)p->get_extension(p, CLAP_EXT_METERS);
  if (!m) return fprintf(stderr, "no clap.meters/1\n"), 1;
  if (!p->activate(p, RATE, 1, MAXBLOCK) || !p->start_processing(p)) return fprintf(stderr, "activate\n"), 1;

  const uint32_t nm = m->count(p);
  uint32_t levels = 0;
  static bool is_level[64];
  static uint32_t chans[64];
  if (nm > 64) return fprintf(stderr, "%u meters, more than this host reads\n", nm), 1;
  for (uint32_t k = 0; k < nm; k++) {
    clap_meter_info_t info;
    if (!m->get_info(p, k, &info)) return fprintf(stderr, "get_info %u\n", k), 1;
    /* a level the face measures, its two legs' peaks; a level the kernel reads (one channel, a detector's) is
     * the kernel's float through libm, an other */
    chans[k] = info.channel_count;
    is_level[k] = info.kind == CLAP_METER_KIND_LEVEL && info.channel_count == 2u;
    if (is_level[k]) levels++;
  }
  if (levels == 0 && !others) return fprintf(stderr, "no level meter: nothing to digest\n"), 1;

  static float il[MAXBLOCK], ir[MAXBLOCK], ol[MAXBLOCK], or_[MAXBLOCK];
  float *in[2] = {il, ir}, *out[2] = {ol, or_};
  clap_audio_buffer_t ai = {in, NULL, 2, 0, 0}, ao = {out, NULL, 2, 0, 0};
  const clap_input_events_t ev_in = {NULL, ev_size, ev_get};
  const clap_output_events_t ev_out = {NULL, ev_push};
  for (uint32_t b = 0; b < 700u; b++) {
    const uint32_t n = 1u + (b * 37u + (b >> 3) * 11u) % MAXBLOCK;
    fill(b, n, il, ir);
    memset(ol, 0, sizeof ol), memset(or_, 0, sizeof or_);
    clap_process_t pr = {0};
    pr.frames_count = n;
    pr.audio_inputs = &ai;
    pr.audio_outputs = &ao;
    pr.audio_inputs_count = pr.audio_outputs_count = 1;
    pr.in_events = &ev_in;
    pr.out_events = NULL;
    pr.out_events = &ev_out;
    if (p->process(p, &pr) == CLAP_PROCESS_ERROR) return fprintf(stderr, "process\n"), 1;
    for (uint32_t k = 0; k < nm; k++) {
      double v[8] = {0};
      uint32_t got = m->get_values(p, k, v, chans[k]); /* the channels: a clip word after them is tools/meter-held-check.sh's */
      if (is_level[k]) {
        for (uint32_t j = 0; j < got; j++) printf("%u %u %.17g\n", b, k, v[j]);
        if (got == 0) printf("%u %u none\n", b, k);
      } else if (others) {
        for (uint32_t j = 0; j < got; j++) fprintf(others, "%u %u %.9g\n", b, k, v[j]);
        if (got == 0) fprintf(others, "%u %u none\n", b, k);
      }
    }
  }
  p->stop_processing(p);
  p->deactivate(p);
  p->destroy(p);
  entry->deinit();
  dlclose(so);
  if (others && fclose(others) != 0) return fprintf(stderr, "write %s\n", others_path), 1;
  return 0;
}
