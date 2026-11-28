// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * transport-probe-oracle.c — omx test-transport-probe through the BUILT .clap, at the nine rates an
 * audio interface clocks at (OMX_ORACLE_FULL_RATES, 32 to 192 kHz).
 *
 *   transport-probe-oracle <omx-test-transport-probe.clap>
 *
 * Arms, at every rate:
 *   transport  a block handed 120 BPM, beat 0, playing reads back whole (memcmp of the
 *              clap_event_transport_t) and unchanged before the next block; a second block handed
 *              another transport (every field set: tempo, beats and seconds, bar, 7/8, loop) reads
 *              that one back whole; the block count and frames are the blocks'
 *   absent     a block handed no transport reads back has_transport 0 and an all-zero transport
 *   audio      both legs pass unchanged, bit for bit
 *   none       before the first block, last() answers false
 *
 * `PASS ...` / `FAIL ...` per check; the exit status is the number of failures (capped).
 */
#include <dlfcn.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clap/clap.h>
#include <omxdsp/omxdsp_definitions.h> /* the oracle rates, omx-dsp's ORACLE_FULL_RATES */

#include "omx_test_transport_probe.h"

#define BLOCK 256u

static const float RATES[OMX_ORACLE_FULL_RATES_COUNT] = OMX_ORACLE_FULL_RATES_INIT;
static int failures;

#define CHECK(cond, ...)                                                                                     \
  do {                                                                                                       \
    bool ok_ = (cond);                                                                                       \
    printf("%s ", ok_ ? "PASS" : "FAIL");                                                                    \
    printf(__VA_ARGS__);                                                                                     \
    printf("\n");                                                                                            \
    if (!ok_) failures++;                                                                                    \
  } while (0)

static uint32_t ev_size(const clap_input_events_t *l) {
  (void)l;
  return 0;
}
static const clap_event_header_t *ev_get(const clap_input_events_t *l, uint32_t i) {
  (void)l, (void)i;
  return NULL;
}
static bool ev_push(const clap_output_events_t *l, const clap_event_header_t *e) {
  (void)l, (void)e;
  return true;
}
static const clap_input_events_t IN = {NULL, ev_size, ev_get};
static const clap_output_events_t OUT = {NULL, ev_push};
static const void *host_ext(const clap_host_t *h, const char *id) {
  (void)h, (void)id;
  return NULL;
}
static void host_noop(const clap_host_t *h) { (void)h; }
static const clap_host_t HOST = {CLAP_VERSION_INIT, NULL, "transport-probe-oracle", "openmixer", "", "0.1", host_ext, host_noop, host_noop, host_noop};

static float in_l[BLOCK], in_r[BLOCK], out_l[BLOCK], out_r[BLOCK];

static void run(const clap_plugin_t *p, const clap_event_transport_t *t, int64_t steady) {
  float *ins[2] = {in_l, in_r}, *outs[2] = {out_l, out_r};
  clap_audio_buffer_t ib = {ins, NULL, 2, 0, 0}, ob = {outs, NULL, 2, 0, 0};
  const clap_process_t proc = {steady, BLOCK, t, &ib, &ob, 1, 1, &IN, &OUT};
  p->process(p, &proc);
}

static clap_event_transport_t transport_at(double bpm, double beats, double secs, uint32_t flags) {
  clap_event_transport_t t;
  memset(&t, 0, sizeof t);
  t.header = (clap_event_header_t){sizeof t, 0, CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_TRANSPORT, 0};
  t.flags = flags;
  t.song_pos_beats = (clap_beattime)llround(beats * CLAP_BEATTIME_FACTOR);
  t.song_pos_seconds = (clap_sectime)llround(secs * CLAP_SECTIME_FACTOR);
  t.tempo = bpm;
  t.tsig_num = 4, t.tsig_denom = 4;
  return t;
}

int main(int argc, char **argv) {
  if (argc != 2) return fprintf(stderr, "usage: transport-probe-oracle <omx-test-transport-probe.clap>\n"), 2;
  void *lib = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
  if (!lib) return fprintf(stderr, "FAIL dlopen %s: %s\n", argv[1], dlerror()), 1;
  const clap_plugin_entry_t *entry = (const clap_plugin_entry_t *)dlsym(lib, "clap_entry");
  if (!entry || !entry->init(argv[1])) return fprintf(stderr, "FAIL no clap_entry in %s\n", argv[1]), 1;
  const clap_plugin_factory_t *fac = (const clap_plugin_factory_t *)entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  for (uint32_t ri = 0; ri < OMX_ORACLE_FULL_RATES_COUNT; ri++) {
    const double rate = RATES[ri];
    const clap_plugin_t *p = fac->create_plugin(fac, &HOST, "org.openmixer.test-transport-probe");
    if (!p || !p->init(p) || !p->activate(p, rate, 1, BLOCK) || !p->start_processing(p)) {
      CHECK(false, "%.0f Hz: the plugin is created and activated", rate);
      continue;
    }
    const omx_test_transport_probe_t *x = (const omx_test_transport_probe_t *)p->get_extension(p, OMX_TEST_TRANSPORT_PROBE_ID);
    struct omx_test_transport_record rec;
    CHECK(x && !x->last(p, &rec), "none %.0f Hz: the extension is there and has no record before the first block", rate);
    if (!x) continue;
    for (uint32_t f = 0; f < BLOCK; f++) {
      in_l[f] = (float)sin(2.0 * M_PI * 997.0 * f / rate) * 0.5f;
      in_r[f] = (float)cos(2.0 * M_PI * 331.0 * f / rate) * 0.25f + 1e-39f; /* a subnormal-sized offset too */
    }

    /* transport: 120 BPM, beat 0, playing */
    const clap_event_transport_t t1 = transport_at(120.0, 0.0, 0.0, CLAP_TRANSPORT_HAS_TEMPO | CLAP_TRANSPORT_HAS_BEATS_TIMELINE |
                                                                      CLAP_TRANSPORT_HAS_SECONDS_TIMELINE | CLAP_TRANSPORT_HAS_TIME_SIGNATURE |
                                                                      CLAP_TRANSPORT_IS_PLAYING);
    run(p, &t1, 0);
    bool got = x->last(p, &rec);
    CHECK(got && rec.has_transport == 1 && memcmp(&rec.transport, &t1, sizeof t1) == 0,
          "transport %.0f Hz: 120 BPM, beat 0, playing reads back whole and unchanged (tempo %.3f)", rate, got ? rec.transport.tempo : -1.0);
    CHECK(got && rec.blocks == 1 && rec.frames == BLOCK && rec.steady_time == 0, "transport %.0f Hz: block 1 of %u frames at steady time 0",
          rate, BLOCK);
    bool same = true;
    for (uint32_t f = 0; f < BLOCK; f++) same = same && memcmp(&out_l[f], &in_l[f], sizeof(float)) == 0 && memcmp(&out_r[f], &in_r[f], sizeof(float)) == 0;
    CHECK(same, "audio %.0f Hz: both legs pass unchanged, bit for bit", rate);

    /* every field set, the next block */
    clap_event_transport_t t2 = transport_at(97.25, 3.5, 3.5 * 60.0 / 97.25,
                                             CLAP_TRANSPORT_HAS_TEMPO | CLAP_TRANSPORT_HAS_BEATS_TIMELINE | CLAP_TRANSPORT_HAS_SECONDS_TIMELINE |
                                                 CLAP_TRANSPORT_HAS_TIME_SIGNATURE | CLAP_TRANSPORT_IS_PLAYING | CLAP_TRANSPORT_IS_LOOP_ACTIVE);
    t2.tempo_inc = 0.001;
    t2.loop_start_beats = 2 * CLAP_BEATTIME_FACTOR, t2.loop_end_beats = 6 * CLAP_BEATTIME_FACTOR;
    t2.loop_start_seconds = 1 * CLAP_SECTIME_FACTOR, t2.loop_end_seconds = 4 * CLAP_SECTIME_FACTOR;
    t2.bar_start = 7 * CLAP_BEATTIME_FACTOR / 2, t2.bar_number = 1;
    t2.tsig_num = 7, t2.tsig_denom = 8;
    run(p, &t2, BLOCK);
    got = x->last(p, &rec);
    CHECK(got && rec.has_transport == 1 && memcmp(&rec.transport, &t2, sizeof t2) == 0,
          "transport %.0f Hz: the next block's transport (97.25 BPM, beat 3.5, 7/8, loop 2..6, bar 1) reads back whole", rate);
    CHECK(got && rec.blocks == 2 && rec.steady_time == BLOCK, "transport %.0f Hz: block 2 at steady time %u", rate, BLOCK);

    /* absent */
    run(p, NULL, 2 * BLOCK);
    got = x->last(p, &rec);
    clap_event_transport_t zero;
    memset(&zero, 0, sizeof zero);
    CHECK(got && rec.has_transport == 0 && memcmp(&rec.transport, &zero, sizeof zero) == 0 && rec.blocks == 3,
          "absent %.0f Hz: a block handed no transport reads back none", rate);

    p->stop_processing(p);
    p->deactivate(p);
    p->destroy(p);
  }
  entry->deinit();
  printf("transport-probe-oracle: %u rates, %d failures\n", (unsigned)OMX_ORACLE_FULL_RATES_COUNT, failures);
  return failures > 100 ? 100 : failures;
}
