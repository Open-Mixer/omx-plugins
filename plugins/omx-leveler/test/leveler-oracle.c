// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * leveler-oracle.c — omx-leveler's kernel-identity test: both built faces, sample for sample, against
 * omx-dsp's leveler instance face called DIRECTLY on the same blocks, at every declared rate,
 * engaged and bypassed — no tolerance.
 *
 *   leveler-oracle clap <omx-leveler.clap>
 *   leveler-oracle lv2 <bundle-dir> <uri>
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-leveler/omx-leveler.decl.json:
 * the plan moves every declared parameter across its travel between uneven blocks (1 to 3000 frames)
 * and toggles the bypass; then, timed in milliseconds so every rate gets the same time, a full-scale
 * burst and a quiet tail at the defaults (a dynamics kernel engages and releases), and each value of
 * every choice held over its own burst and tail with the other parameters stepped once (a control
 * read only under one value is reached). All of it runs through ONE instance of the face. Parameters are found BY NAME (CLAP) or BY
 * SYMBOL (LV2, through lilv), so a renumbered face cannot pass by accident. Two guards keep the test
 * honest: the reference must move the signal, and a reference with any ONE parameter one step off must
 * differ from the face (the sabotage arm), so a wrong or dead coefficient is seen. A CLAP face's
 * reported latency holds while it is active and moves only at the activation after the restart it
 * asks for, as the CLAP latency extension says. The plan runs again IN PLACE (each output the buffer of
 * its input), bit for bit the first run; and, through CLAP, with each block split in two, its second half
 * carrying no event, bit for bit the kernel resolving the same values again before that half.
 */
#include <dlfcn.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clap/clap.h>
#include <lilv/lilv.h>

#include "omx_leveler_params.h"

#include <omxdsp/fx/omx_leveler_instance.h>
#include <omxdsp/omx_denormal.h>

#define NP OMX_LEVELER_PARAM_COUNT
/* 1 when the face takes a sidechain key. */
#define KEYED 0
/* Each block of the plan is processed this many times in a row, the same values each time. */
#define STRETCH 1u
#define SPAN 1u

/* One block: its length (frames, or ms when ms > 0), the stimulus it carries (0 the moving bursts,
 * 1 a full-scale burst, 2 the quiet tail), the declared parameters (declaration order) and the
 * host's bypass. */
typedef struct {
  uint32_t frames;
  float ms;
  int signal;
  float v[NP];
  int bypass;
} Block;

static const Block PLAN[] = {
    { 64, 0.0f, 0, {0.0f, 2.0f, 2.0f}, 0 },
    { 1, 0.0f, 0, {25.0f, 0.0f, 1.0f}, 0 },
    { 333, 0.0f, 0, {100.0f, 2.0f, 1.0f}, 0 },
    { 512, 0.0f, 0, {0.0f, 2.0f, 2.0f}, 0 },
    { 17, 0.0f, 0, {75.0f, 1.0f, 0.0f}, 1 },
    { 480, 0.0f, 0, {0.0f, 2.0f, 1.0f}, 1 },
    { 129, 0.0f, 0, {50.0f, 0.0f, 1.0f}, 0 },
    { 1024, 0.0f, 0, {90.0f, 1.0f, 1.0f}, 0 },
    { 7, 0.0f, 0, {10.0f, 1.0f, 1.0f}, 0 },
    { 2500, 0.0f, 0, {60.0f, 2.0f, 1.0f}, 0 },
    { 600, 0.0f, 0, {35.0f, 2.0f, 2.0f}, 0 },
    { 3000, 0.0f, 0, {0.0f, 1.0f, 0.0f}, 0 },
    { 0, 10.0f, 1, {0.0f, 2.0f, 1.0f}, 0 },
    { 0, 100.0f, 2, {0.0f, 2.0f, 1.0f}, 0 },
    { 0, 10.0f, 1, {0.0f, 0.0f, 1.0f}, 0 },
    { 0, 100.0f, 2, {0.0f, 0.0f, 1.0f}, 0 },
    { 0, 100.0f, 2, {75.0f, 0.0f, 2.0f}, 0 },
    { 0, 10.0f, 1, {0.0f, 1.0f, 1.0f}, 0 },
    { 0, 100.0f, 2, {0.0f, 1.0f, 1.0f}, 0 },
    { 0, 100.0f, 2, {75.0f, 1.0f, 2.0f}, 0 },
    { 0, 10.0f, 1, {0.0f, 2.0f, 1.0f}, 0 },
    { 0, 100.0f, 2, {0.0f, 2.0f, 1.0f}, 0 },
    { 0, 100.0f, 2, {75.0f, 2.0f, 2.0f}, 0 },
    { 0, 10.0f, 1, {0.0f, 2.0f, 0.0f}, 0 },
    { 0, 100.0f, 2, {0.0f, 2.0f, 0.0f}, 0 },
    { 0, 100.0f, 2, {75.0f, 0.0f, 0.0f}, 0 },
    { 0, 10.0f, 1, {0.0f, 2.0f, 1.0f}, 0 },
    { 0, 100.0f, 2, {0.0f, 2.0f, 1.0f}, 0 },
    { 0, 100.0f, 2, {75.0f, 0.0f, 1.0f}, 0 },
    { 0, 10.0f, 1, {0.0f, 2.0f, 2.0f}, 0 },
    { 0, 100.0f, 2, {0.0f, 2.0f, 2.0f}, 0 },
    { 0, 100.0f, 2, {75.0f, 0.0f, 2.0f}, 0 },
};
#define NBLOCKS (sizeof PLAN / sizeof PLAN[0])

/* One step of each parameter: what the sabotage arm moves it by. */
static const float STEP[NP] = { 1.0f, 1.0f, 1.0f };

static int failures;

static void check(bool ok, const char *face, double sr, const char *what) {
  if (!ok) failures++;
  printf("%s %s %s @ %.0f Hz\n", ok ? "PASS" : "FAIL", face, what, sr);
}

static uint32_t frames_of(const Block *k, double sr) {
  return k->ms > 0.0f ? (uint32_t)ceil((double)k->ms * sr / 1000.0) : k->frames;
}

static uint32_t total_frames(double sr) {
  uint32_t n = 0;
  for (size_t i = 0; i < NBLOCKS; i++) n += frames_of(&PLAN[i], sr);
  return n * STRETCH;
}

static uint32_t longest_block(double sr) {
  uint32_t n = 1;
  for (size_t i = 0; i < NBLOCKS; i++) n = frames_of(&PLAN[i], sr) > n ? frames_of(&PLAN[i], sr) : n;
  return n;
}

/* The moving blocks: bursts of decaying noise on a quiet bed, the legs unequal (onsets, decays and a
 * stereo image). A burst block: full-scale noise. A tail block: noise falling from full scale by
 * 8.7 dB every 10 ms onto a bed at about -60 dBFS, so a level crosses every threshold at its own
 * time and a gate closes under its default threshold. */
static void signal_make(float *l, float *r, uint32_t n, double sr) {
  uint32_t seed = 0x6f6d78u;
  const uint32_t period = (uint32_t)(0.09 * sr);
  uint32_t start = 0, end = 0;
  size_t b = 0;
  for (uint32_t i = 0; i < n; i++) {
    while (i >= end && b < NBLOCKS) start = end, end += frames_of(&PLAN[b++], sr) * SPAN;
    const int sig = PLAN[b - 1].signal;
    seed = seed * 1664525u + 1013904223u;
    const float a = (float)(seed >> 8) / 16777216.0f - 0.5f;
    seed = seed * 1664525u + 1013904223u;
    const float c = (float)(seed >> 8) / 16777216.0f - 0.5f;
    if (sig == 1) {
      l[i] = 2.0f * a;
      r[i] = 1.8f * c;
    } else if (sig == 2) {
      const float fall = 2.0f * expf(-(float)(i - start) / (0.01f * (float)sr)) + 0.002f;
      l[i] = fall * c;
      r[i] = 0.9f * fall * a;
    } else {
      const float env = expf(-(float)(i % period) / (0.012f * (float)sr));
      l[i] = 0.9f * env * a + 0.02f * c;
      r[i] = 0.7f * env * c + 0.02f * a;
    }
  }
}

#if KEYED
/* The key: a noise burst at 0 dBFS every 3 s, silence between: a detector keyed by it opens, then
 * releases down through every threshold of the plan at the rate of each block's release. */
static void key_make(float *k, uint32_t n, double sr) {
  uint32_t seed = 0x6b6579u;
  const uint32_t period = (uint32_t)(3.0 * sr), burst = (uint32_t)(0.02 * sr);
  for (uint32_t i = 0; i < n; i++) {
    seed = seed * 1664525u + 1013904223u;
    k[i] = i % period < burst ? 2.0f * ((float)(seed >> 8) / 16777216.0f - 0.5f) : 0.0f;
  }
}
#endif

/* Set for the quiet arm: every block of the plan runs as two, its values resolved before each half (the
 * reference) or sent before the first half only (the face, whose second half is a block with no event). */
static bool quiet;
static uint32_t quiet_blocks;

/* The frames of the next part of a block of `whole` frames with `left` to run: all of it, or in the quiet
 * arm its first half, then the rest. */
static uint32_t part_of(uint32_t whole, uint32_t left) { return quiet && left == whole && whole > 1u ? whole / 2u : left; }

/* The block's values, with parameter `off` (or none, -1) moved one step inside its travel. */
static void values_of(const Block *k, int off, float *v) {
  memcpy(v, k->v, sizeof k->v);
  if (off < 0) return;
  const omx_plugin_param *p = &OMX_LEVELER_PARAMS[off];
  v[off] = v[off] + STEP[off] <= p->max ? v[off] + STEP[off] : v[off] - STEP[off];
}

/* The reference: omx-dsp's instance face, called directly, on the same blocks. */
static void reference(double sr, int off, const float *key, const float *il, const float *ir, float *ol, float *orr,
                      uint32_t *latency) {
  static OmxLevelerInstance inst;
  (void)omx_leveler_instance_init(&inst, (float)sr);
  uint32_t at = 0;
  for (size_t b = 0; b < NBLOCKS; b++) {
    const Block *k = &PLAN[b];
    float values[NP];
    values_of(k, off, values);
    const uint32_t whole = frames_of(k, sr);
    for (uint32_t r = 0; r < STRETCH; r++)
      for (uint32_t left = whole, frames; left; left -= frames) {
      frames = part_of(whole, left);
      omx_leveler_instance_resolve(&inst, k->bypass, values[OMX_LEVELER_PARAM_AMOUNT], (int)lrintf(values[OMX_LEVELER_PARAM_DIRECTION]), (int)lrintf(values[OMX_LEVELER_PARAM_SPEED]));
#if KEYED
      omx_leveler_instance_run(&inst, key ? key + at : NULL, il + at, ir + at, ol + at, orr + at, frames);
#else
      (void)key;
      omx_leveler_instance_run(&inst, il + at, ir + at, ol + at, orr + at, frames);
#endif
      at += frames;
    }
  }
  if (latency) *latency = (uint32_t)lrintf((float)(omx_leveler_instance_latency(&inst)));
}

typedef struct Driver Driver;
struct Driver {
  /* the whole plan through ONE fresh instance; `*latency` is read back */
  bool (*run)(void *face, const Block *plan, size_t nblocks, double sr, const float *key, const float *il,
              const float *ir, float *ol, float *orr, uint32_t *latency);
  void *face;
};

static bool same(const float *a, const float *b, uint32_t n) { return memcmp(a, b, (size_t)n * sizeof(float)) == 0; }

/* Set by a CLAP run whose face reported a latency other than the activation's while active, or
 * reported a new one after the run without asking the host for the restart that reads it. */
static bool latency_broke;

/* The plan through the face and the reference; `keyed` hands both the key, else no key is routed.
 * `seen[p]` is set when a reference with parameter p one step off differs from the face's output. */
static void run_checks(const char *face, Driver *d, double sr, bool keyed, bool *seen) {
  const uint32_t n = total_frames(sr);
  float *buf = calloc((size_t)n * 9u, sizeof(float));
  if (!buf) abort();
  float *il = buf, *ir = buf + n, *ol = buf + 2u * n, *orr = buf + 3u * n, *wl = buf + 4u * n, *wr = buf + 5u * n;
  float *xl = buf + 6u * n, *xr = buf + 7u * n, *key = NULL;
  signal_make(il, ir, n, sr);
#if KEYED
  if (keyed) key_make(key = buf + 8u * n, n, sr);
#endif
  const char *pass = KEYED ? (keyed ? " (keyed)" : " (no key)") : "";
  char what[200];
  omx_denormals_off(); /* the faces run with flush-to-zero; so does the reference */
  uint32_t want = 0;
  reference(sr, -1, key, il, ir, wl, wr, &want);
  uint32_t lat = 99;
  latency_broke = false;
  bool ran = d->run(d->face, PLAN, NBLOCKS, sr, key, il, ir, ol, orr, &lat);
  bool ident = ran && same(ol, wl, n) && same(orr, wr, n);
  for (uint32_t i = 0; ran && !ident && i < n; i++)
    if (memcmp(ol + i, wl + i, sizeof(float)) != 0 || memcmp(orr + i, wr + i, sizeof(float)) != 0) {
      fprintf(stderr, "  first difference at frame %u: got %.9g/%.9g, kernel %.9g/%.9g\n", i, (double)ol[i],
              (double)orr[i], (double)wl[i], (double)wr[i]);
      break;
    }
  snprintf(what, sizeof what, "output is the instance face's, bit for bit%s", pass);
  check(ident, face, sr, what);
  /* the reference must have moved the signal, or identity would be vacuous */
  snprintf(what, sizeof what, "the kernel shapes the signal%s", pass);
  check(!same(wl, il, n) || !same(wr, ir, n), face, sr, what);
  snprintf(what, sizeof what, "published latency is the kernel's%s", pass);
  check(ran && lat == want, face, sr, what);
  snprintf(what, sizeof what, "the reported latency moves only at activation, after a restart asked for%s", pass);
  check(ran && !latency_broke, face, sr, what);
  /* in place: the same plan with each output the buffer of its input (CLAP's in_place_pair, an LV2 host that
   * connects one buffer to both ports) gives what separate buffers gave, bit for bit */
  memcpy(xl, il, (size_t)n * sizeof(float));
  memcpy(xr, ir, (size_t)n * sizeof(float));
  bool in_place = d->run(d->face, PLAN, NBLOCKS, sr, key, xl, xr, xl, xr, NULL);
  snprintf(what, sizeof what, "in place, the output is the separate buffers' output, bit for bit%s", pass);
  check(ran && in_place && same(xl, ol, n) && same(xr, orr, n), face, sr, what);
  if (strcmp(face, "clap") == 0) {
    /* quiet blocks: each block split in two, its values sent before the first half only; the second half, a
     * block with no event, runs as the reference's, which resolves the same values again before it */
    quiet = true, quiet_blocks = 0;
    bool q = d->run(d->face, PLAN, NBLOCKS, sr, key, il, ir, xl, xr, NULL);
    reference(sr, -1, key, il, ir, wl, wr, NULL);
    quiet = false;
    snprintf(what, sizeof what, "%u blocks with no event, each the output of resolving the same values again, bit for bit%s",
             quiet_blocks, pass);
    check(ran && q && quiet_blocks > 0 && same(xl, wl, n) && same(xr, wr, n), face, sr, what);
    reference(sr, -1, key, il, ir, wl, wr, NULL); /* the arms below read the whole-block reference */
  }
  if (KEYED && keyed) {
    /* the key must reach the detector: the same plan with no key routed differs */
    reference(sr, -1, NULL, il, ir, xl, xr, NULL);
    check(!same(xl, wl, n) || !same(xr, wr, n), face, sr, "the key changes the output");
  }
  /* the sabotage arm: each parameter one step off */
  for (int p = 0; p < (int)NP; p++) {
    reference(sr, p, key, il, ir, xl, xr, NULL);
    if (ran && (!same(xl, ol, n) || !same(xr, orr, n))) seen[p] = true;
  }
  free(buf);
}

/* Every check at one rate: the plan (keyed, then with no key, for a keyed face), and the sabotage arm
 * over both, so each parameter one step off must be seen by one of them. */
static void rate_checks(const char *face, Driver *d, double sr) {
  bool seen[NP] = {false};
  run_checks(face, d, sr, KEYED, seen);
  if (KEYED) run_checks(face, d, sr, false, seen);
  for (int p = 0; p < (int)NP; p++) {
    char what[160];
    snprintf(what, sizeof what, "a reference with %s one step off differs", OMX_LEVELER_PARAMS[p].symbol);
    check(seen[p], face, sr, what);
  }
}

/* ---- CLAP ---------------------------------------------------------------------------------- */

static const void *h_ext(const clap_host_t *h, const char *id) {
  (void)h, (void)id;
  return NULL;
}
static void h_noop(const clap_host_t *h) { (void)h; }
/* A restart the face asked for: the run deactivates and activates it again, as a host does. */
static bool restart_asked;
static void h_restart(const clap_host_t *h) {
  (void)h;
  restart_asked = true;
}
static const clap_host_t HOST = {CLAP_VERSION_INIT, NULL, "leveler-oracle", "openmixer", "", "0.1", h_ext, h_restart,
                                 h_noop, h_noop};

typedef struct {
  clap_event_param_value_t ev[NP + 1];
  uint32_t n;
} Events;
static uint32_t ev_size(const clap_input_events_t *l) { return ((const Events *)l->ctx)->n; }
static const clap_event_header_t *ev_get(const clap_input_events_t *l, uint32_t i) {
  return &((const Events *)l->ctx)->ev[i].header;
}
static bool ev_push(const clap_output_events_t *l, const clap_event_header_t *e) {
  (void)l, (void)e;
  return true;
}

typedef struct {
  const clap_plugin_entry_t *entry;
  const clap_plugin_factory_t *factory;
} ClapFace;

static bool clap_param(const clap_plugin_params_t *pp, const clap_plugin_t *p, const char *name, clap_id *id) {
  for (uint32_t i = 0; i < pp->count(p); i++) {
    clap_param_info_t info;
    if (!pp->get_info(p, i, &info)) continue;
    if (name ? strcmp(info.name, name) == 0 : (info.flags & CLAP_PARAM_IS_BYPASS) != 0) {
      *id = info.id;
      return true;
    }
  }
  fprintf(stderr, "  no CLAP parameter named %s\n", name ? name : "(bypass)");
  return false;
}

static bool clap_run(void *face, const Block *plan, size_t nblocks, double sr, const float *key, const float *il,
                     const float *ir, float *ol, float *orr, uint32_t *latency) {
  ClapFace *c = (ClapFace *)face;
  const clap_plugin_descriptor_t *desc = c->factory->get_plugin_descriptor(c->factory, 0);
  const clap_plugin_t *p = c->factory->create_plugin(c->factory, &HOST, desc->id);
  if (!p || !p->init(p)) return false;
  const clap_plugin_params_t *pp = (const clap_plugin_params_t *)p->get_extension(p, CLAP_EXT_PARAMS);
  const clap_plugin_latency_t *lat = (const clap_plugin_latency_t *)p->get_extension(p, CLAP_EXT_LATENCY);
  restart_asked = false;
  bool ok = pp && lat && p->activate(p, sr, 1, longest_block(sr)) && p->start_processing(p);
  const uint32_t activated = ok ? lat->get(p) : 0;
  clap_id ids[NP + 1];
  for (uint32_t i = 0; ok && i < NP; i++) ok = clap_param(pp, p, OMX_LEVELER_PARAMS[i].name, &ids[i]);
  ok = ok && clap_param(pp, p, NULL, &ids[NP]);
  clap_output_events_t out = {NULL, ev_push};
  uint32_t at = 0;
  for (size_t b = 0; ok && b < nblocks * STRETCH; b++) {
    const Block *k = &plan[b / STRETCH];
    if (lat->get(p) != activated) latency_broke = true;
    const uint32_t whole = frames_of(k, sr);
    for (uint32_t left = whole, frames; ok && left; left -= frames) {
    frames = part_of(whole, left);
    Events evs = {.n = 0};
    for (uint32_t i = 0; i <= NP && left == whole; i++) {
      clap_event_param_value_t *e = &evs.ev[evs.n++];
      memset(e, 0, sizeof *e);
      e->header = (clap_event_header_t){sizeof *e, 0, CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_PARAM_VALUE, 0};
      e->param_id = ids[i], e->note_id = -1, e->port_index = -1, e->channel = -1, e->key = -1;
      e->value = i < NP ? (double)k->v[i] : (k->bypass ? 1.0 : 0.0);
    }
    if (evs.n == 0) quiet_blocks++;
    clap_input_events_t in = {&evs, ev_size, ev_get};
    float *ib[2] = {(float *)il + at, (float *)ir + at}, *ob[2] = {ol + at, orr + at};
    float *kb[1] = {key ? (float *)key + at : NULL};
    clap_audio_buffer_t ai[2] = {{ib, NULL, 2, 0, 0}, {kb, NULL, 1, 0, 0}}, ao = {ob, NULL, 2, 0, 0};
    clap_process_t pr = {.steady_time = at, .frames_count = frames, .audio_inputs = ai, .audio_outputs = &ao,
                         .audio_inputs_count = key ? 2u : 1u, .audio_outputs_count = 1, .in_events = &in,
                         .out_events = &out};
    ok = p->process(p, &pr) != CLAP_PROCESS_ERROR;
    at += frames;
    }
  }
  if (ok && lat->get(p) != activated) latency_broke = true;
  if (ok && restart_asked) {
    p->stop_processing(p);
    p->deactivate(p);
    ok = p->activate(p, sr, 1, longest_block(sr)) && p->start_processing(p);
  }
  if (ok && latency) *latency = lat->get(p);
  p->stop_processing(p);
  p->deactivate(p);
  p->destroy(p);
  return ok;
}

static int clap_main(const char *path) {
  void *lib = dlopen(path, RTLD_NOW | RTLD_LOCAL);
  if (!lib) return fprintf(stderr, "FAIL dlopen %s: %s\n", path, dlerror()), 1;
  ClapFace c = {(const clap_plugin_entry_t *)dlsym(lib, "clap_entry"), NULL};
  if (!c.entry || !c.entry->init(path)) return fprintf(stderr, "FAIL clap_entry %s\n", path), 1;
  c.factory = (const clap_plugin_factory_t *)c.entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  if (!c.factory || c.factory->get_plugin_count(c.factory) != 1) return fprintf(stderr, "FAIL factory\n"), 1;
  Driver d = {clap_run, &c};
  for (uint32_t i = 0; i < OMX_DECLARED_RATE_COUNT; i++) rate_checks("clap", &d, (double)OMX_DECLARED_RATES[i]);
  c.entry->deinit();
  dlclose(lib);
  return failures ? 1 : 0;
}

/* ---- LV2 ------------------------------------------------------------------------------------ */

typedef struct {
  LilvWorld *world;
  const LilvPlugin *plugin;
} Lv2Face;

static int32_t port_of(const Lv2Face *l, const char *symbol) {
  LilvNode *sym = lilv_new_string(l->world, symbol);
  const LilvPort *port = lilv_plugin_get_port_by_symbol(l->plugin, sym);
  lilv_node_free(sym);
  if (!port) fprintf(stderr, "  no LV2 port '%s'\n", symbol);
  return port ? (int32_t)lilv_port_get_index(l->plugin, port) : -1;
}

static bool lv2_run(void *face, const Block *plan, size_t nblocks, double sr, const float *key, const float *il,
                    const float *ir, float *ol, float *orr, uint32_t *latency) {
  Lv2Face *l = (Lv2Face *)face;
  LilvInstance *inst = lilv_plugin_instantiate(l->plugin, sr, NULL);
  if (!inst) return false;
  const char *audio[] = {"in_l", "in_r", "out_l", "out_r"};
  int32_t ap[4], cp[NP + 2];
  float cv[NP + 2] = {0};
  bool ok = true;
  for (int i = 0; i < 4; i++) ok = (ap[i] = port_of(l, audio[i])) >= 0 && ok;
#if KEYED
  const int32_t kp = port_of(l, "");
  ok = kp >= 0 && ok;
#else
  (void)key;
#endif
  for (uint32_t i = 0; i < NP; i++) ok = (cp[i] = port_of(l, OMX_LEVELER_PARAMS[i].symbol)) >= 0 && ok;
  ok = (cp[NP] = port_of(l, "enabled")) >= 0 && ok;
  ok = (cp[NP + 1] = port_of(l, "latency")) >= 0 && ok;
  for (uint32_t i = 0; ok && i < NP + 2; i++) lilv_instance_connect_port(inst, (uint32_t)cp[i], &cv[i]);
  if (ok) lilv_instance_activate(inst);
  uint32_t at = 0;
  for (size_t b = 0; ok && b < nblocks * STRETCH; b++) {
    const Block *k = &plan[b / STRETCH];
    for (uint32_t i = 0; i < NP; i++) cv[i] = k->v[i];
    cv[NP] = k->bypass ? 0.0f : 1.0f;
    lilv_instance_connect_port(inst, (uint32_t)ap[0], (void *)(il + at));
    lilv_instance_connect_port(inst, (uint32_t)ap[1], (void *)(ir + at));
    lilv_instance_connect_port(inst, (uint32_t)ap[2], ol + at);
    lilv_instance_connect_port(inst, (uint32_t)ap[3], orr + at);
#if KEYED
    lilv_instance_connect_port(inst, (uint32_t)kp, key ? (void *)(key + at) : NULL);
#endif
    const uint32_t frames = frames_of(k, sr);
    lilv_instance_run(inst, frames);
    at += frames;
  }
  if (ok) lilv_instance_deactivate(inst);
  lilv_instance_free(inst);
  if (ok && latency) *latency = (uint32_t)(cv[NP + 1] + 0.5f);
  return ok;
}

static int lv2_main(const char *bundle, const char *uri) {
  Lv2Face l = {lilv_world_new(), NULL};
  char dir[4096];
  snprintf(dir, sizeof dir, "%s%s", bundle, bundle[strlen(bundle) - 1] == '/' ? "" : "/");
  LilvNode *b = lilv_new_file_uri(l.world, NULL, dir);
  lilv_world_load_bundle(l.world, b);
  LilvNode *u = lilv_new_uri(l.world, uri);
  l.plugin = lilv_plugins_get_by_uri(lilv_world_get_all_plugins(l.world), u);
  if (!l.plugin) return fprintf(stderr, "FAIL no LV2 plugin %s in %s\n", uri, dir), 1;
  Driver d = {lv2_run, &l};
  for (uint32_t i = 0; i < OMX_DECLARED_RATE_COUNT; i++) rate_checks("lv2", &d, (double)OMX_DECLARED_RATES[i]);
  lilv_node_free(u), lilv_node_free(b);
  lilv_world_free(l.world);
  return failures ? 1 : 0;
}

int main(int argc, char **argv) {
  int rc;
  if (argc == 3 && strcmp(argv[1], "clap") == 0) rc = clap_main(argv[2]);
  else if (argc == 4 && strcmp(argv[1], "lv2") == 0) rc = lv2_main(argv[2], argv[3]);
  else return fprintf(stderr, "usage: leveler-oracle clap <x.clap> | lv2 <bundle-dir> <uri>\n"), 2;
  printf("%s %s: %d failure(s)\n", rc ? "ORACLE-RED" : "ORACLE-GREEN", argv[1], failures);
  return rc;
}
