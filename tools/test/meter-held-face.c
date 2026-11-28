// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * meter-held-face.c — a built face holds its meters until the host reads them (docs/meters.md), at every one of
 * omx-dsp's ORACLE_FULL_RATES and every quantum from 32 to 1024 frames. A minimal CLAP host runs two instances of the
 * face over the same signal: 20 ms of silence, a 10 ms burst of 1.2 on both legs (+1.58 dBFS, every sample over full
 * scale) starting on a block boundary, then 100 ms of silence, then 100 ms more. The REFERENCE instance is read after
 * every block, so each of its reads is that one block's value; the HELD instance is read only at the ends of the two
 * silent stretches, as a host reading at its own tick reads it. The golden vectors, at each rate and quantum:
 *
 *   - level: the held read after the burst is the largest of the reference's block values since the read before
 *     (bit for bit) and 20 log10(1.2) (the burst's peak); the read after is the reference's largest again: the floor.
 *   - clip: the value after the level's channels is 1 after the burst and 0 at the read after it.
 *   - gain reduction, the step: the held read is the deepest of the reference's block values since the read before
 *     (bit for bit), and is a reduction (below -0.1 dB).
 *   - gain reduction, the fall: the reference's block values never deepen again once past their deepest, and the
 *     next held read is the deepest of the second stretch (bit for bit), shallower than the step.
 *
 * Besides: the flags (held, and a clip word on a level only), a host that leaves room for the channels only gets the
 * channels, and nothing is read before the first block after an activation, nor after a deactivation and a new
 * activation (a stopped or restarted face has no last read to hold). Exit 1 on the first value that is not what the
 * stimulus says; every failing line starts FAIL.
 *
 *   meter-held-face <x.clap>
 *
 * The face is a compressor (omx-comp): meters compGr, compIn, compOut, compKey. Used by tools/meter-held-check.sh.
 */
#include <dlfcn.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <omx-clap-host/clap_host.h> /* OMX_CLAP_METER_IS_HELD, OMX_CLAP_METER_HAS_CLIP */
#include <omx-clap-host/clap_meters.h>
#include <omxdsp/omxdsp_definitions.h> /* OMX_ORACLE_FULL_RATES_INIT */

#include "clap/entry.h"
#include "clap/factory/plugin-factory.h"
#include "clap/host.h"
#include "clap/process.h"

#define QMAX 1024u
#define BURST 1.2f

static const void *host_get_extension(const clap_host_t *h, const char *id) { (void)h, (void)id; return NULL; }
static void host_noop(const clap_host_t *h) { (void)h; }
static const clap_host_t HOST = {CLAP_VERSION_INIT, NULL, "omx meter-held-face", "openmixer", "", "0.1",
                                 host_get_extension, host_noop, host_noop, host_noop};
static uint32_t ev_size(const clap_input_events_t *l) { (void)l; return 0; }
static const clap_event_header_t *ev_get(const clap_input_events_t *l, uint32_t i) { (void)l, (void)i; return NULL; }
static bool ev_push(const clap_output_events_t *l, const clap_event_header_t *e) { (void)l, (void)e; return true; }

static int bad;
static char where[64];

static void expect(int ok, const char *what, double got) {
  if (!ok || where[0] == '\0') printf("%s %s%s%s (%.17g)\n", ok ? "ok  " : "FAIL", where, where[0] ? ": " : "", what, got);
  if (!ok) bad++;
}

/* one instance and its meter ids */
typedef struct {
  const clap_plugin_t *p;
  const clap_plugin_meters_t *m;
  clap_id gr, in;
} Inst;

static int open_inst(const clap_plugin_factory_t *fac, Inst *x) {
  x->p = fac->create_plugin(fac, &HOST, fac->get_plugin_descriptor(fac, 0)->id);
  if (!x->p || !x->p->init(x->p)) return fprintf(stderr, "create/init\n"), -1;
  x->m = (const clap_plugin_meters_t *)x->p->get_extension(x->p, CLAP_EXT_METERS);
  if (!x->m) return fprintf(stderr, "no clap.meters/1\n"), -1;
  int gr = -1, in = -1;
  for (uint32_t k = 0; k < x->m->count(x->p); k++) {
    clap_meter_info_t info;
    if (x->m->get_info(x->p, k, &info)) {
      if (strcmp(info.name, "compGr") == 0) gr = (int)k;
      if (strcmp(info.name, "compIn") == 0) in = (int)k;
    }
  }
  if (gr < 0 || in < 0) return fprintf(stderr, "meters not found\n"), -1;
  x->gr = (clap_id)gr, x->in = (clap_id)in;
  return 0;
}

/* one block of `frames`: its first `hot` samples are the burst, the rest silence */
static void block(const clap_plugin_t *p, uint32_t frames, uint32_t hot) {
  static float il[QMAX], ir[QMAX], ol[QMAX], or_[QMAX];
  float *in[2] = {il, ir}, *out[2] = {ol, or_};
  clap_audio_buffer_t ai = {in, NULL, 2, 0, 0}, ao = {out, NULL, 2, 0, 0};
  const clap_input_events_t ev_in = {NULL, ev_size, ev_get};
  const clap_output_events_t ev_out = {NULL, ev_push};
  for (uint32_t i = 0; i < frames; i++) il[i] = ir[i] = i < hot ? BURST : 0.0f;
  clap_process_t pr = {0};
  pr.frames_count = frames;
  pr.audio_inputs = &ai, pr.audio_inputs_count = 1;
  pr.audio_outputs = &ao, pr.audio_outputs_count = 1;
  pr.in_events = &ev_in, pr.out_events = &ev_out;
  if (p->process(p, &pr) == CLAP_PROCESS_ERROR) fprintf(stderr, "process failed\n"), exit(1);
}

/* What the reference saw since the held instance's last read: the level's largest, any clip, the reduction's deepest,
 * and whether the reduction deepened again after it had begun to fall. */
typedef struct {
  double level, gr, last_gr;
  int clip, rose, deepened;
} Ref;

static void ref_reset(Ref *r) { r->level = -INFINITY, r->gr = INFINITY, r->clip = 0, r->rose = 0, r->deepened = 0, r->last_gr = NAN; }

/* both instances run `blocks` blocks of `q` frames, the burst's `*hot` samples first; the reference is read after each */
static void run(Inst *held, Inst *ref, uint32_t q, uint32_t blocks, uint32_t *hot, Ref *r) {
  for (uint32_t b = 0; b < blocks; b++) {
    const uint32_t h = *hot < q ? *hot : q;
    *hot -= h;
    block(held->p, q, h);
    block(ref->p, q, h);
    double v[3];
    if (ref->m->get_values(ref->p, ref->in, v, 3) != 3) expect(0, "the reference reads its level after a block", 0);
    r->level = v[0] > r->level ? v[0] : r->level;
    r->level = v[1] > r->level ? v[1] : r->level;
    r->clip |= v[2] != 0.0;
    if (ref->m->get_values(ref->p, ref->gr, v, 1) != 1) expect(0, "the reference reads its gain reduction after a block", 0);
    if (!isnan(r->last_gr) && v[0] > r->last_gr) r->rose = 1;
    if (r->rose && v[0] < r->last_gr) r->deepened = 1;
    r->last_gr = v[0];
    r->gr = v[0] < r->gr ? v[0] : r->gr;
  }
}

static uint32_t blocks_of(double rate, uint32_t q, double ms) { return (uint32_t)ceil(rate * ms / 1000.0 / q); }

static void one(const clap_plugin_factory_t *fac, double rate, uint32_t q) {
  Inst held, ref;
  if (open_inst(fac, &held) || open_inst(fac, &ref)) exit(1);
  snprintf(where, sizeof where, "%.0f Hz, quantum %u", rate, q);
  const int bad_before = bad;
  double v[3];
  for (int restart = 0; restart < 2; restart++) {
    if (!held.p->activate(held.p, rate, 1, q) || !held.p->start_processing(held.p)) fprintf(stderr, "activate\n"), exit(1);
    expect(held.m->get_values(held.p, held.in, v, 3) == 0, restart ? "a restarted face reads nothing before its first block" :
           "nothing is read before the first block", 0);
    if (!restart) {
      block(held.p, q, q); /* a burst, then the face stops: its hold goes with the activation */
      held.p->stop_processing(held.p);
      held.p->deactivate(held.p);
    }
  }
  if (!ref.p->activate(ref.p, rate, 1, q) || !ref.p->start_processing(ref.p)) fprintf(stderr, "activate\n"), exit(1);
  Ref r;
  uint32_t hot = 0;
  ref_reset(&r);
  run(&held, &ref, q, blocks_of(rate, q, 20.0), &hot, &r);
  expect(held.m->get_values(held.p, held.in, v, 3) == 3 && v[2] == 0.0, "the read before the burst: no clip", v[2]);
  expect(held.m->get_values(held.p, held.gr, v, 1) == 1, "the read before the burst: the reduction", v[0]);

  /* the burst, 10 ms from a block boundary, and 100 ms of silence: one read after it all */
  hot = (uint32_t)lround(rate * 0.010);
  ref_reset(&r);
  run(&held, &ref, q, blocks_of(rate, q, 10.0) + blocks_of(rate, q, 100.0), &hot, &r);
  uint32_t n = held.m->get_values(held.p, held.in, v, 3);
  const double level = v[0], clip = n == 3 ? v[2] : NAN;
  expect(n == 3 && v[0] == r.level && v[1] == r.level, "level: the held read is the largest block since the last read", v[0]);
  expect(n == 3 && fabs(v[0] - 20.0 * log10((double)BURST)) < 1e-4, "level: the burst's peak, +1.58 dBFS, after 100 ms of silence", v[0]);
  expect(n == 3 && v[2] == 1.0 && r.clip, "clip: the word after the channels holds the burst", v[2]);
  n = held.m->get_values(held.p, held.gr, v, 3);
  const double step = v[0];
  expect(n == 1 && step == r.gr, "gain reduction: the held read is the deepest block since the last read, and no clip word", step);
  expect(step < -0.1, "gain reduction: the step, a reduction", step);
  expect(!r.deepened, "gain reduction: once falling it never deepens again", r.last_gr);

  /* 100 ms more of silence: the next read has recovered, and the reduction falls */
  ref_reset(&r);
  run(&held, &ref, q, blocks_of(rate, q, 100.0), &hot, &r);
  n = held.m->get_values(held.p, held.in, v, 3);
  expect(n == 3 && v[0] == r.level && v[1] == r.level && v[0] < -50.0, "level: the next read has recovered, the floor", v[0]);
  expect(n == 3 && v[2] == 0.0 && !r.clip, "clip: the next read has none", v[2]);
  n = held.m->get_values(held.p, held.gr, v, 1);
  const double fall = v[0];
  expect(n == 1 && fall == r.gr && fall > step, "gain reduction: the fall, the next read the deepest of its own stretch, shallower", fall);
  expect(held.m->get_values(held.p, held.in, v, 2) == 2, "a host that leaves room for the channels only gets the channels", 2);
  expect(!r.deepened, "gain reduction: the fall never deepens again in silence", r.last_gr);

  clap_meter_info_t info;
  held.m->get_info(held.p, held.in, &info);
  expect((info.flags & OMX_CLAP_METER_IS_HELD) && (info.flags & OMX_CLAP_METER_HAS_CLIP), "the level is flagged held, with a clip word", info.flags);
  held.m->get_info(held.p, held.gr, &info);
  expect((info.flags & OMX_CLAP_METER_IS_HELD) && !(info.flags & OMX_CLAP_METER_HAS_CLIP), "the gain reduction is flagged held, with no clip word", info.flags);
  const int ok = bad == bad_before;
  printf("%s %s: held level %.6f dBFS, clip %g, gain reduction step %.6f dB, fall %.6f dB\n", ok ? "ok  " : "FAIL", where,
         level, clip, step, fall);
  for (Inst *x = &held; x; x = x == &held ? &ref : NULL) {
    x->p->stop_processing(x->p);
    x->p->deactivate(x->p);
    x->p->destroy(x->p);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) return fprintf(stderr, "usage: meter-held-face <x.clap>\n"), 2;
  void *so = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
  if (!so) return fprintf(stderr, "dlopen %s: %s\n", argv[1], dlerror()), 1;
  const clap_plugin_entry_t *entry = (const clap_plugin_entry_t *)dlsym(so, "clap_entry");
  if (!entry || !entry->init(argv[1])) return fprintf(stderr, "no clap_entry\n"), 1;
  const clap_plugin_factory_t *fac = (const clap_plugin_factory_t *)entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  if (!fac || fac->get_plugin_count(fac) != 1) return fprintf(stderr, "factory\n"), 1;
  static const float rates[OMX_ORACLE_FULL_RATES_COUNT] = OMX_ORACLE_FULL_RATES_INIT;
  static const uint32_t quanta[] = {32u, 64u, 128u, 256u, 512u, 1024u};
  for (uint32_t i = 0; i < OMX_ORACLE_FULL_RATES_COUNT; i++)
    for (uint32_t k = 0; k < sizeof quanta / sizeof quanta[0]; k++) one(fac, (double)rates[i], quanta[k]);
  entry->deinit();
  printf("%s meter-held-face: %u rates x %zu quanta, level, clip and gain reduction held until read\n", bad ? "FAIL" : "ok  ",
         (unsigned)OMX_ORACLE_FULL_RATES_COUNT, sizeof quanta / sizeof quanta[0]);
  return bad ? 1 : 0;
}
