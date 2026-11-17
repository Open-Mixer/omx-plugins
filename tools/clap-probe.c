// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * clap-probe.c — a minimal CLAP host over ANY built omx plugin: it knows no plugin, only CLAP.
 *
 *   clap-probe params <x.clap>
 *       one line per parameter, as clap_plugin_params.get_info reports it:
 *       `id<TAB>name<TAB>min<TAB>max<TAB>default<TAB>flags` — tools/clap-params-check.mjs compares
 *       it with the declaration.
 *   clap-probe live <x.clap>
 *       every parameter but the host's bypass MOVES the output: a deterministic stereo signal (noise
 *       bursts with quiet gaps) is run
 *       with the parameter at its default and at the far end of its travel, in four settings of
 *       the others until one shows a difference: (a) all at their defaults; (b) every other toggle
 *       on (a filter's frequency moves nothing while the filter is off); (c) every other parameter
 *       at the far end of its travel (a bell's frequency moves nothing at 0 dB); (d) every other
 *       toggle on and every other travel at its far end; (e) every other choice (stepped from 0
 *       to at most 15, not a toggle) at its far value, the rest at their defaults: a choice that selects which of
 *       several controls is heard (a speed switch) hides them until it is moved, and a far
 *       value of another travel (a mix at 0) can hide them again. A parameter whose output is
 *       byte-identical in all five, and under every value of every other choice or toggle in turn (the
 *       rest at their defaults: a control read only under one mode), and with every other toggle on and
 *       each other travel in turn at either end of it (the rest at their defaults: a strip's gate hold is
 *       heard only once its trim lowers the quiet bed under the gate's threshold), and with each other
 *       travel in turn at either end, the rest at their defaults (a ducker heard only against a depth
 *       its default does not reach, which a key listen switched on would hide), and with each other
 *       toggle that comes up off on alone, the choices at their far values and the travels at their
 *       defaults (a dynamic band's own switch on and its dynamics in a mode, its listens off), and,
 *       last, with each pair of other travels at their ends, the rest at their defaults (a multiband's
 *       fourth band, heard once the band count includes it and a crossover gives it width), is one
 *       the face does not deliver. Every block hands a transport with a tempo (PROBE_BPM, CLAP_TRANSPORT_HAS_TEMPO),
 *       as a host does, so a time synced to the tempo is heard; a face that reads no tempo ignores it.
 *       `PASS live <name>` / `FAIL live <name>` per parameter. A face still on the wizard's stub
 *       binding is red here, by design. Every input port past the main one (a sidechain key) is
 *       connected too, fed a signal of its own: square-wave bursts the main signal does not have, so a
 *       parameter that picks what the detector listens to can move the output.
 *   clap-probe state <x.clap>
 *       the state round trip, as a host restores a session: every parameter, the host's bypass
 *       included, is set off its default (a travel to a value with nine significant digits) on one
 *       instance through clap_plugin_params.flush, and its state saved; a fresh instance loads that
 *       state through a stream that hands over 17 bytes at a time. The loading instance must ask the
 *       host to rescan the values, report every value the first one did (exactly, as doubles), save a
 *       state byte-identical to the one it loaded, and, both activated on the same signal, put out
 *       the same samples. Run once with the bypass off and once with it on. An empty state must be
 *       refused, the instance's values untouched. `PASS state ...` / `FAIL state ...` lines.
 */
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clap/entry.h"
#include "clap/ext/audio-ports.h"
#include "clap/ext/params.h"
#include "clap/ext/state.h"
#include "clap/factory/plugin-factory.h"
#include "clap/host.h"

#define RATE 48000.0
#define BLOCK 256u
/* The tempo the live test's transport carries, BPM: a quarter is 454.5 ms, no time travel's default. */
#define PROBE_BPM 132.0
/* 1.5 s at 48 kHz in whole blocks: a delay's second repeat. OMX_PROBE_STRETCH=n runs the live test n times that
 * long, for a kernel that walks its gain or its bands at dB per second (a declaration's "liveStretch"). */
static uint32_t probe_frames(void) {
  const char *e = getenv("OMX_PROBE_STRETCH");
  const long n = e ? strtol(e, NULL, 10) : 1;
  return 282u * BLOCK * (uint32_t)(n >= 1 && n <= 64 ? n : 1);
}
#define FRAMES (probe_frames())
#define STATE_FRAMES (188u * BLOCK) /* the state test's run: one second at 48 kHz in whole blocks */
#define MAX_PARAMS 256u
#define MAX_SIDE 8u /* channels over every input port past the main one */

/* The host's clap.params: what `state` reads is whether a loaded state asked for a rescan. */
static clap_param_rescan_flags rescanned;
static void host_params_rescan(const clap_host_t *h, clap_param_rescan_flags f) { (void)h, rescanned |= f; }
static void host_params_clear(const clap_host_t *h, clap_id id, clap_param_clear_flags f) { (void)h, (void)id, (void)f; }
static void host_params_flush(const clap_host_t *h) { (void)h; }
static const clap_host_params_t HOST_PARAMS = {host_params_rescan, host_params_clear, host_params_flush};

static const void *host_get_extension(const clap_host_t *h, const char *id) {
  (void)h;
  return strcmp(id, CLAP_EXT_PARAMS) == 0 ? &HOST_PARAMS : NULL;
}
static void host_noop(const clap_host_t *h) { (void)h; }

static const clap_host_t HOST = {
    CLAP_VERSION_INIT, NULL, "omx clap-probe", "openmixer", "", "0.1",
    host_get_extension, host_noop, host_noop, host_noop,
};

typedef struct {
  clap_event_param_value_t ev[MAX_PARAMS];
  uint32_t n;
} EvList;

static uint32_t ev_size(const clap_input_events_t *l) { return ((const EvList *)l->ctx)->n; }
static const clap_event_header_t *ev_get(const clap_input_events_t *l, uint32_t i) {
  return &((const EvList *)l->ctx)->ev[i].header;
}
static bool ev_push(const clap_output_events_t *l, const clap_event_header_t *e) {
  (void)l, (void)e;
  return true;
}

typedef struct {
  void *so;
  const clap_plugin_entry_t *entry;
  const clap_plugin_t *plugin;
  const clap_plugin_params_t *params;
} Clap;

static int clap_open(Clap *c, const char *path) {
  memset(c, 0, sizeof *c);
  c->so = dlopen(path, RTLD_NOW | RTLD_LOCAL);
  if (!c->so) return fprintf(stderr, "dlopen %s: %s\n", path, dlerror()), 0;
  c->entry = (const clap_plugin_entry_t *)dlsym(c->so, "clap_entry");
  if (!c->entry || !c->entry->init(path)) return fprintf(stderr, "no clap_entry\n"), 0;
  const clap_plugin_factory_t *fac = (const clap_plugin_factory_t *)c->entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  if (!fac || fac->get_plugin_count(fac) != 1) return fprintf(stderr, "factory\n"), 0;
  const clap_plugin_descriptor_t *d = fac->get_plugin_descriptor(fac, 0);
  c->plugin = fac->create_plugin(fac, &HOST, d->id);
  if (!c->plugin || !c->plugin->init(c->plugin)) return fprintf(stderr, "create/init\n"), 0;
  c->params = (const clap_plugin_params_t *)c->plugin->get_extension(c->plugin, CLAP_EXT_PARAMS);
  if (!c->params) return fprintf(stderr, "no params extension\n"), 0;
  return 1;
}

static void clap_close(Clap *c) {
  if (c->plugin) c->plugin->destroy(c->plugin);
  if (c->entry) c->entry->deinit();
  if (c->so) dlclose(c->so);
}

static int params_dump(const char *path) {
  Clap c;
  if (!clap_open(&c, path)) return 1;
  uint32_t n = c.params->count(c.plugin);
  for (uint32_t i = 0; i < n; i++) {
    clap_param_info_t info;
    if (!c.params->get_info(c.plugin, i, &info)) return fprintf(stderr, "get_info %u\n", i), 1;
    /* the value texts of a stepped parameter's whole steps (at most 16), `|`-joined: a choice's labels, as a host reads
     * them through value_to_text (omx-clap-scan's scale points); a travel's texts are its numbers */
    char texts[4096] = "";
    const double span = info.max_value - info.min_value;
    if ((info.flags & (CLAP_PARAM_IS_STEPPED | CLAP_PARAM_IS_ENUM)) && span == (double)(long)span && span <= 15.0) {
      size_t at = 0;
      for (double v = info.min_value; v <= info.max_value && at + 2 < sizeof texts; v += 1.0) {
        char t[128];
        if (!c.params->value_to_text(c.plugin, info.id, v, t, sizeof t)) t[0] = 0;
        const int w = snprintf(texts + at, sizeof texts - at, "%s%s", v == info.min_value ? "" : "|", t);
        if (w < 0 || (size_t)w >= sizeof texts - at) break;
        at += (size_t)w;
      }
    }
    printf("%u\t%s\t%.17g\t%.17g\t%.17g\t%u\t%s\n", info.id, info.name, info.min_value, info.max_value,
           info.default_value, (unsigned)info.flags, texts);
  }
  clap_close(&c);
  return 0;
}

/** Deterministic stereo noise with sparse impulses, the legs different, in 0.25 s bursts each followed
 * by 0.5 s of a quiet bed: a gate closes, a dynamics stage releases, a reverb's tail is heard. */
static void make_signal(float *l, float *r) {
  uint32_t seed = 0x2545F491u;
  for (uint32_t i = 0; i < FRAMES; i++) {
    const float g = (i % 36000u) < 12000u ? 1.0f : 0.025f;
    seed = seed * 1664525u + 1013904223u;
    l[i] = ((float)(seed >> 8) / 16777216.0f - 0.5f) * 0.8f * g;
    seed = seed * 1664525u + 1013904223u;
    r[i] = ((float)(seed >> 8) / 16777216.0f - 0.5f) * 0.6f * g;
    if (i % 4800u == 0 && g == 1.0f) l[i] = r[i] = 0.9f;
  }
}

/** The side inputs' signal: a 1 kHz square wave at half scale, on for the first 0.1 s of every
 * second and silent for the rest, long enough for a detector's slowest release to tell. */
static void make_side(float *k) {
  for (uint32_t i = 0; i < FRAMES; i++) k[i] = (i % 48000u) < 4800u ? 0.5f * (float)((i / 24u) % 2u ? 1 : -1) : 0.0f;
}

static float *side;

/** One run from a fresh instance, the `n` (id, value) pairs set before the first block. */
static int render(const char *path, const clap_id *ids, const double *vals, uint32_t n, const float *in_l,
                  const float *in_r, float *out_l, float *out_r) {
  Clap c;
  if (!clap_open(&c, path)) return 0;
  if (!c.plugin->activate(c.plugin, RATE, 1, BLOCK) || !c.plugin->start_processing(c.plugin)) {
    clap_close(&c);
    return fprintf(stderr, "activate\n"), 0;
  }
  /* The input ports the plugin declares past the main one, each channel fed the side signal. */
  const clap_plugin_audio_ports_t *ap = (const clap_plugin_audio_ports_t *)c.plugin->get_extension(c.plugin, CLAP_EXT_AUDIO_PORTS);
  uint32_t nin = ap ? ap->count(c.plugin, true) : 1u, nside = 0;
  clap_audio_buffer_t ain[1 + MAX_SIDE];
  float *side_ch[MAX_SIDE];
  uint32_t side_first[MAX_SIDE];
  if (nin < 1u) nin = 1u;
  for (uint32_t k = 1; k < nin && k <= MAX_SIDE; k++) {
    clap_audio_port_info_t pi;
    uint32_t ch = ap->get(c.plugin, k, true, &pi) ? pi.channel_count : 0u;
    side_first[k - 1] = nside;
    for (; ch > 0 && nside < MAX_SIDE; ch--) nside++;
    ain[k] = (clap_audio_buffer_t){side_ch + side_first[k - 1], NULL, nside - side_first[k - 1], 0, 0};
  }
  if (nin > 1u + MAX_SIDE) nin = 1u + MAX_SIDE;
  static EvList evs;
  for (uint32_t f = 0; f < FRAMES; f += BLOCK) {
    for (uint32_t k = 0; k < nside; k++) side_ch[k] = side + f;
    evs.n = 0;
    for (uint32_t k = 0; f == 0 && k < n; k++) {
      clap_event_param_value_t *e = &evs.ev[evs.n++];
      memset(e, 0, sizeof *e);
      e->header.size = sizeof *e;
      e->header.space_id = CLAP_CORE_EVENT_SPACE_ID;
      e->header.type = CLAP_EVENT_PARAM_VALUE;
      e->param_id = ids[k];
      e->note_id = -1;
      e->port_index = -1;
      e->channel = -1;
      e->key = -1;
      e->value = vals[k];
    }
    clap_input_events_t in_ev = {&evs, ev_size, ev_get};
    clap_output_events_t out_ev = {NULL, ev_push};
    float *ins[2] = {(float *)in_l + f, (float *)in_r + f};
    float *outs[2] = {out_l + f, out_r + f};
    ain[0] = (clap_audio_buffer_t){ins, NULL, 2, 0, 0};
    clap_audio_buffer_t aout = {outs, NULL, 2, 0, 0};
    clap_event_transport_t tr;
    memset(&tr, 0, sizeof tr);
    tr.header = (clap_event_header_t){sizeof tr, 0, CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_TRANSPORT, 0};
    tr.flags = CLAP_TRANSPORT_HAS_TEMPO | CLAP_TRANSPORT_IS_PLAYING;
    tr.tempo = PROBE_BPM;
    clap_process_t pr;
    memset(&pr, 0, sizeof pr);
    pr.steady_time = f;
    pr.frames_count = BLOCK;
    pr.transport = &tr;
    pr.audio_inputs = ain;
    pr.audio_outputs = &aout;
    pr.audio_inputs_count = nin;
    pr.audio_outputs_count = 1;
    pr.in_events = &in_ev;
    pr.out_events = &out_ev;
    if (c.plugin->process(c.plugin, &pr) == CLAP_PROCESS_ERROR) {
      clap_close(&c);
      return fprintf(stderr, "process error\n"), 0;
    }
  }
  c.plugin->stop_processing(c.plugin);
  c.plugin->deactivate(c.plugin);
  clap_close(&c);
  return 1;
}

static double far_of(const clap_param_info_t *p) { return p->default_value == p->max_value ? p->min_value : p->max_value; }
/** A choice: stepped, counting from 0 to at most 15 (a mode, a speed), not a toggle. A stepped travel in
 * whole units (a balance in percent) is not one. */
static int is_choice(const clap_param_info_t *p) {
  return (p->flags & CLAP_PARAM_IS_STEPPED) && p->min_value == 0.0 && p->max_value >= 2.0 && p->max_value <= 15.0;
}
static int is_toggle(const clap_param_info_t *p) {
  return (p->flags & CLAP_PARAM_IS_STEPPED) && p->min_value == 0.0 && p->max_value == 1.0;
}

static float *in_l, *in_r, *a_l, *a_r, *b_l, *b_r;
/* setting 9's second travel and its value */
static uint32_t pair_at;
static double pair_v;

/** Does moving parameter `i` from its default to its far value change the output, the others set by
 * `setting` (0 defaults, 1 toggles on, 2 at their far values, 3 toggles on and the rest far, 4 the
 * other stepped choices far and the rest at their defaults, 5 the other choice or toggle `k` at
 * `kv` and the rest at their defaults, 6 every other toggle on and the travel `k` at `kv`, the rest at
 * their defaults, 7 the travel `k` at `kv` and the rest at their defaults, 8 the toggle `k` on, the
 * choices at their far values and the rest at their defaults, 9 the travels `k` at `kv` and pair_at at
 * pair_v, the rest at their defaults)? -1 on a host error. */
static int moves(const char *path, const clap_param_info_t *info, uint32_t n, uint32_t i, int setting, uint32_t k_at,
                 double kv) {
  static clap_id ids[MAX_PARAMS];
  static double vals[MAX_PARAMS];
  uint32_t m = 0;
  for (uint32_t k = 0; k < n; k++) {
    if (k == i || (info[k].flags & CLAP_PARAM_IS_BYPASS)) continue;
    if (setting == 1 && is_toggle(&info[k])) ids[m] = info[k].id, vals[m++] = 1.0;
    if (setting == 2) ids[m] = info[k].id, vals[m++] = far_of(&info[k]);
    if (setting == 3) ids[m] = info[k].id, vals[m++] = is_toggle(&info[k]) ? 1.0 : far_of(&info[k]);
    if (setting == 4 && is_choice(&info[k])) ids[m] = info[k].id, vals[m++] = far_of(&info[k]);
    if (setting == 5 && k == k_at) ids[m] = info[k].id, vals[m++] = kv;
    if (setting == 6 && (k == k_at || is_toggle(&info[k]))) ids[m] = info[k].id, vals[m++] = k == k_at ? kv : 1.0;
    if (setting == 7 && k == k_at) ids[m] = info[k].id, vals[m++] = kv;
    if (setting == 8 && (k == k_at || is_choice(&info[k]))) ids[m] = info[k].id, vals[m++] = k == k_at ? 1.0 : far_of(&info[k]);
    if (setting == 9 && (k == k_at || k == pair_at)) ids[m] = info[k].id, vals[m++] = k == k_at ? kv : pair_v;
  }
  ids[m] = info[i].id;
  vals[m] = info[i].default_value;
  if (!render(path, ids, vals, m + 1, in_l, in_r, a_l, a_r)) return -1;
  vals[m] = far_of(&info[i]);
  if (!render(path, ids, vals, m + 1, in_l, in_r, b_l, b_r)) return -1;
  return memcmp(a_l, b_l, FRAMES * sizeof(float)) != 0 || memcmp(a_r, b_r, FRAMES * sizeof(float)) != 0;
}

static int live(const char *path) {
  Clap c;
  if (!clap_open(&c, path)) return 1;
  const uint32_t n = c.params->count(c.plugin);
  if (n > MAX_PARAMS) return fprintf(stderr, "%u parameters, more than %u\n", n, MAX_PARAMS), 1;
  clap_param_info_t *info = calloc(n, sizeof *info);
  for (uint32_t i = 0; i < n; i++) c.params->get_info(c.plugin, i, &info[i]);
  clap_close(&c);

  float **bufs[] = {&in_l, &in_r, &a_l, &a_r, &b_l, &b_r, &side};
  for (size_t k = 0; k < 7; k++) *bufs[k] = malloc(FRAMES * sizeof(float));
  make_signal(in_l, in_r);
  make_side(side);
  static const char *const SETTING[] = {"the others at their defaults", "every other toggle on", "the others at their far ends",
                                        "every other toggle on, the rest at their far ends",
                                        "the other choices at their far values"};
  int fails = 0, checked = 0;
  for (uint32_t i = 0; i < n; i++) {
    if (info[i].flags & CLAP_PARAM_IS_BYPASS) continue;
    checked++;
    int setting = 0, r = 0;
    for (; setting < 5; setting++) {
      r = moves(path, info, n, i, setting, 0, 0.0);
      if (r != 0) break;
    }
    /* then each value of every other choice or toggle in turn: a control read only under one value
     * (a reverb's plate depth under the plate algorithm) is moved where it is read */
    uint32_t k_at = 0;
    double kv = 0.0;
    for (uint32_t k = 0; r == 0 && k < n; k++) {
      if (k == i || !(is_choice(&info[k]) || is_toggle(&info[k]))) continue;
      for (double v = info[k].min_value; r == 0 && v <= info[k].max_value; v += 1.0) {
        if (v == info[k].default_value) continue;
        r = moves(path, info, n, i, 5, k, v);
        k_at = k, kv = v;
      }
    }
    /* then, every other toggle on, each other travel in turn at either end of it */
    int ends = 0;
    for (uint32_t k = 0; r == 0 && k < n; k++) {
      if (k == i || is_choice(&info[k]) || is_toggle(&info[k]) || (info[k].flags & CLAP_PARAM_IS_BYPASS)) continue;
      const double end[2] = {info[k].min_value, info[k].max_value};
      for (int e = 0; r == 0 && e < 2; e++) {
        if (end[e] == info[k].default_value) continue;
        r = moves(path, info, n, i, 6, k, end[e]);
        k_at = k, kv = end[e], ends = 1;
      }
    }
    /* then each other travel in turn at either end of it, the toggles at their defaults too: a switch
     * heard only while a travel it reads leaves its default (a ducker against its depth), which a
     * toggle that replaces the output (a key listen) would hide */
    for (uint32_t k = 0; r == 0 && k < n; k++) {
      if (k == i || is_choice(&info[k]) || is_toggle(&info[k]) || (info[k].flags & CLAP_PARAM_IS_BYPASS)) continue;
      const double end[2] = {info[k].min_value, info[k].max_value};
      for (int e = 0; r == 0 && e < 2; e++) {
        if (end[e] == info[k].default_value) continue;
        r = moves(path, info, n, i, 7, k, end[e]);
        k_at = k, kv = end[e], ends = 2;
      }
    }
    /* then each other toggle on alone, the choices at their far values, the travels at their defaults: a
     * band's own switch on and its dynamics in a mode, while the toggles that replace the output (its
     * listens) stay off and no travel (a mix at 0) hides it */
    for (uint32_t k = 0; r == 0 && k < n; k++) {
      if (k == i || !is_toggle(&info[k]) || info[k].default_value != 0.0) continue;
      r = moves(path, info, n, i, 8, k, 1.0);
      k_at = k, kv = 1.0, ends = 3;
    }
    /* last, each pair of other travels at their ends, the rest at their defaults: a band that is heard only
     * once the band count includes it and a crossover gives it the width the signal fills */
    for (uint32_t k = 0; r == 0 && k < n; k++) {
      if (k == i || is_choice(&info[k]) || is_toggle(&info[k]) || (info[k].flags & CLAP_PARAM_IS_BYPASS)) continue;
      for (uint32_t j = k + 1; r == 0 && j < n; j++) {
        if (j == i || is_choice(&info[j]) || is_toggle(&info[j]) || (info[j].flags & CLAP_PARAM_IS_BYPASS)) continue;
        const double ek[2] = {info[k].min_value, info[k].max_value}, ej[2] = {info[j].min_value, info[j].max_value};
        for (int a = 0; r == 0 && a < 2; a++)
          for (int b = 0; r == 0 && b < 2; b++) {
            if (ek[a] == info[k].default_value || ej[b] == info[j].default_value) continue;
            pair_at = j, pair_v = ej[b];
            r = moves(path, info, n, i, 9, k, ek[a]);
            k_at = k, kv = ek[a], ends = 4;
          }
      }
    }
    if (r < 0) return 1;
    if (r && ends == 4) {
      printf("PASS live %s moves the output (%s at %g and %s at %g, the rest at their defaults)\n", info[i].name, info[k_at].name, kv,
             info[pair_at].name, pair_v);
    } else if (r && ends == 3) {
      printf("PASS live %s moves the output (%s on alone, the choices at their far values, the rest at their defaults)\n", info[i].name, info[k_at].name);
    } else if (r && ends == 2) {
      printf("PASS live %s moves the output (%s at %g, the rest at their defaults)\n", info[i].name, info[k_at].name, kv);
    } else if (r && ends) {
      printf("PASS live %s moves the output (every other toggle on, %s at %g, the rest at their defaults)\n", info[i].name, info[k_at].name, kv);
    } else if (r && setting < 5) {
      printf("PASS live %s moves the output (%s)\n", info[i].name, SETTING[setting]);
    } else if (r) {
      printf("PASS live %s moves the output (%s at %g, the rest at their defaults)\n", info[i].name, info[k_at].name, kv);
    } else {
      printf("FAIL live %s: %g leaves the output byte-identical in every setting; the face does not deliver it to the kernel\n",
             info[i].name, far_of(&info[i]));
      fails++;
    }
  }
  free(info);
  for (size_t k = 0; k < 7; k++) free(*bufs[k]);
  if (checked == 0) return printf("FAIL live: no parameter to move\n"), 1;
  return fails ? 1 : 0;
}

/* ---- state: the round trip a host makes when it restores a session --------------------------- */

typedef struct {
  unsigned char *data;
  size_t len, cap, pos;
} Blob;

static int64_t blob_write(const clap_ostream_t *s, const void *buf, uint64_t size) {
  Blob *b = (Blob *)s->ctx;
  if (b->len + size > b->cap) {
    size_t cap = (b->len + size) * 2u;
    unsigned char *d = realloc(b->data, cap);
    if (!d) return -1;
    b->data = d, b->cap = cap;
  }
  memcpy(b->data + b->len, buf, size);
  b->len += size;
  return (int64_t)size;
}

/** 17 bytes at a time, as clap-validator's buffered test reads: a loader that assumes one read is caught. */
static int64_t blob_read(const clap_istream_t *s, void *buf, uint64_t size) {
  Blob *b = (Blob *)s->ctx;
  uint64_t n = b->len - b->pos;
  if (n > size) n = size;
  if (n > 17u) n = 17u;
  memcpy(buf, b->data + b->pos, n);
  b->pos += n;
  return (int64_t)n;
}

static int state_save(const Clap *c, const clap_plugin_state_t *st, Blob *b) {
  memset(b, 0, sizeof *b);
  clap_ostream_t os = {b, blob_write};
  return st->save(c->plugin, &os);
}

static int state_load(const Clap *c, const clap_plugin_state_t *st, Blob *b) {
  b->pos = 0;
  clap_istream_t is = {b, blob_read};
  return st->load(c->plugin, &is);
}

/** Parameter `i`'s value off its default: a stepped one a whole step away, a travel at a fraction of
 * its range that prints with nine significant digits, distinct per parameter. */
static double off_default(const clap_param_info_t *p, uint32_t i, int bypass) {
  if (p->flags & CLAP_PARAM_IS_BYPASS) return bypass ? 1.0 : 0.0;
  if (p->flags & CLAP_PARAM_IS_STEPPED) {
    double v = p->default_value + 1.0 + (double)(i % 3u);
    if (v > p->max_value) v = p->default_value - 1.0;
    return v < p->min_value ? p->min_value : v;
  }
  double f = 0.1 + 0.8 * ((double)(i + 1u) * 0.6180339887498949 - (double)(uint32_t)((double)(i + 1u) * 0.6180339887498949));
  double v = p->min_value + (p->max_value - p->min_value) * f;
  if (v == p->default_value) v = p->min_value + (p->max_value - p->min_value) * 0.0317;
  return v;
}

/** One second of the probe's signal through an instance already set, from its activation. */
static int run_open(const Clap *c, const float *il, const float *ir, float *ol, float *or_) {
  if (!c->plugin->activate(c->plugin, RATE, 1, BLOCK) || !c->plugin->start_processing(c->plugin)) return 0;
  static EvList none;
  none.n = 0;
  for (uint32_t f = 0; f < STATE_FRAMES; f += BLOCK) {
    clap_input_events_t in_ev = {&none, ev_size, ev_get};
    clap_output_events_t out_ev = {NULL, ev_push};
    float *ins[2] = {(float *)il + f, (float *)ir + f};
    float *outs[2] = {ol + f, or_ + f};
    clap_audio_buffer_t ain = {ins, NULL, 2, 0, 0};
    clap_audio_buffer_t aout = {outs, NULL, 2, 0, 0};
    clap_process_t pr;
    memset(&pr, 0, sizeof pr);
    pr.steady_time = f;
    pr.frames_count = BLOCK;
    pr.audio_inputs = &ain;
    pr.audio_outputs = &aout;
    pr.audio_inputs_count = 1;
    pr.audio_outputs_count = 1;
    pr.in_events = &in_ev;
    pr.out_events = &out_ev;
    if (c->plugin->process(c->plugin, &pr) == CLAP_PROCESS_ERROR) return 0;
  }
  c->plugin->stop_processing(c->plugin);
  c->plugin->deactivate(c->plugin);
  return 1;
}

/** One round trip, the bypass `bypass`: the number of failed checks, -1 on a host error. */
static int state_round(const char *path, int bypass, const float *il, const float *ir, float *al, float *ar, float *bl,
                       float *br) {
  const char *mode = bypass ? "bypass on" : "bypass off";
  Clap a, b;
  if (!clap_open(&a, path)) return -1;
  const clap_plugin_state_t *sta = (const clap_plugin_state_t *)a.plugin->get_extension(a.plugin, CLAP_EXT_STATE);
  if (!sta) return printf("FAIL state: no clap.state extension\n"), clap_close(&a), 1;
  const uint32_t n = a.params->count(a.plugin);
  if (n > MAX_PARAMS) return fprintf(stderr, "%u parameters, more than %u\n", n, MAX_PARAMS), clap_close(&a), -1;
  static clap_param_info_t info[MAX_PARAMS];
  static double va[MAX_PARAMS], vb[MAX_PARAMS];
  static EvList evs;
  evs.n = 0;
  for (uint32_t i = 0; i < n; i++) {
    a.params->get_info(a.plugin, i, &info[i]);
    clap_event_param_value_t *e = &evs.ev[evs.n++];
    memset(e, 0, sizeof *e);
    e->header.size = sizeof *e;
    e->header.space_id = CLAP_CORE_EVENT_SPACE_ID;
    e->header.type = CLAP_EVENT_PARAM_VALUE;
    e->param_id = info[i].id;
    e->note_id = -1, e->port_index = -1, e->channel = -1, e->key = -1;
    e->value = off_default(&info[i], i, bypass);
  }
  clap_input_events_t in_ev = {&evs, ev_size, ev_get};
  clap_output_events_t out_ev = {NULL, ev_push};
  a.params->flush(a.plugin, &in_ev, &out_ev);
  for (uint32_t i = 0; i < n; i++) a.params->get_value(a.plugin, info[i].id, &va[i]);
  Blob sa, sb;
  if (!state_save(&a, sta, &sa)) return printf("FAIL state (%s): save refused\n", mode), clap_close(&a), 1;

  if (!clap_open(&b, path)) return clap_close(&a), -1;
  const clap_plugin_state_t *stb = (const clap_plugin_state_t *)b.plugin->get_extension(b.plugin, CLAP_EXT_STATE);
  int fails = 0;
  rescanned = 0;
  if (!state_load(&b, stb, &sa)) {
    printf("FAIL state (%s): a fresh instance refused the %zu-byte state the first one saved\n", mode, sa.len);
    fails++;
  }
  if (!(rescanned & (CLAP_PARAM_RESCAN_VALUES | CLAP_PARAM_RESCAN_ALL))) {
    printf("FAIL state (%s): loading the state changed the values and asked the host for no rescan\n", mode);
    fails++;
  }
  for (uint32_t i = 0; i < n; i++) {
    b.params->get_value(b.plugin, info[i].id, &vb[i]);
    if (vb[i] != va[i]) {
      printf("FAIL state (%s): %s is %.17g after the load, %.17g when saved\n", mode, info[i].name, vb[i], va[i]);
      fails++;
    }
  }
  if (!state_save(&b, stb, &sb)) {
    printf("FAIL state (%s): the loaded instance refused to save\n", mode);
    fails++;
  } else if (sb.len != sa.len || memcmp(sa.data, sb.data, sa.len) != 0) {
    printf("FAIL state (%s): the state saved after the load differs from the one loaded (%zu vs %zu bytes)\n", mode,
           sb.len, sa.len);
    fails++;
  }
  if (!run_open(&a, il, ir, al, ar) || !run_open(&b, il, ir, bl, br)) {
    free(sa.data), free(sb.data), clap_close(&a), clap_close(&b);
    return fprintf(stderr, "activate/process\n"), -1;
  }
  if (memcmp(al, bl, STATE_FRAMES * sizeof(float)) != 0 || memcmp(ar, br, STATE_FRAMES * sizeof(float)) != 0) {
    printf("FAIL state (%s): the instance that loaded the state does not put out the samples the one that saved it does\n", mode);
    fails++;
  }
  if (!fails)
    printf("PASS state (%s): %u values, a %zu-byte state, the output sample for sample\n", mode, n, sa.len);
  free(sa.data), free(sb.data);
  clap_close(&a);
  clap_close(&b);
  return fails;
}

/** An empty state is refused, and leaves the instance as it was. */
static int state_empty(const char *path) {
  Clap c;
  if (!clap_open(&c, path)) return -1;
  const clap_plugin_state_t *st = (const clap_plugin_state_t *)c.plugin->get_extension(c.plugin, CLAP_EXT_STATE);
  const uint32_t n = c.params->count(c.plugin);
  static double before[MAX_PARAMS];
  static clap_id ids[MAX_PARAMS];
  int fails = 0;
  /* by each parameter's id, never its index: the bypass's id is not its index */
  for (uint32_t i = 0; i < n && i < MAX_PARAMS; i++) {
    clap_param_info_t info;
    if (!c.params->get_info(c.plugin, i, &info) || !c.params->get_value(c.plugin, ids[i] = info.id, &before[i]))
      printf("FAIL state: parameter %u has no value\n", i), fails++;
  }
  Blob empty = {NULL, 0, 0, 0};
  if (state_load(&c, st, &empty)) printf("FAIL state: an empty state was accepted\n"), fails++;
  for (uint32_t i = 0; i < n && i < MAX_PARAMS; i++) {
    double v = 0.0;
    if (!c.params->get_value(c.plugin, ids[i], &v) || v != before[i]) printf("FAIL state: a refused empty state moved parameter %u\n", ids[i]), fails++;
  }
  if (!fails) printf("PASS state: an empty state is refused, the values untouched\n");
  clap_close(&c);
  return fails;
}

static int state(const char *path) {
  float *buf[6];
  for (int k = 0; k < 6; k++) buf[k] = calloc(FRAMES, sizeof(float));
  make_signal(buf[0], buf[1]);
  int fails = 0, r;
  for (int bypass = 0; bypass < 2; bypass++) {
    if ((r = state_round(path, bypass, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5])) < 0) return 1;
    fails += r;
  }
  if ((r = state_empty(path)) < 0) return 1;
  fails += r;
  for (int k = 0; k < 6; k++) free(buf[k]);
  return fails ? 1 : 0;
}

int main(int argc, char **argv) {
  if (argc == 3 && strcmp(argv[1], "params") == 0) return params_dump(argv[2]);
  if (argc == 3 && strcmp(argv[1], "live") == 0) return live(argv[2]);
  if (argc == 3 && strcmp(argv[1], "state") == 0) return state(argv[2]);
  fprintf(stderr, "usage: clap-probe params|live|state <x.clap>\n");
  return 2;
}
