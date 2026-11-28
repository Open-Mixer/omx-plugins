// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * eq-identity.c — the EQ faces' kernel-identity test: both BUILT faces of omx-eq8, omx-eq16 or
 * omx-eq32, sample for sample, against the console's EQ, at the nine oracle rates (32, 44.1, 48, 64,
 * 88.2, 96, 128, 176.4 and 192 kHz), with no tolerance. One source for the three plugins: the
 * band count is a compile-time number, -DOMX_EQ_LV2_BANDS=8|16|32, as it is for the kernel.
 *
 *   eq-identity clap <omx-eqN.clap>
 *   eq-identity lv2 <omx-eqN.lv2> <uri>
 *
 * The face is reached the way a host reaches it (dlopen and clap_entry, or lilv), never by
 * including it, and its controls are found by NAME (CLAP) or by SYMBOL (LV2). It runs in 256-frame
 * quanta through a programme of seven segments with every band moving between them: every band
 * type, both slopes of both pass filters, a notch past the bell's Q ceiling and a bell asking for
 * one, bands switched off so the bank closes up, bands at 0 dB that the console parks, the host's
 * bypass and the EQ's own switch. Two references run each segment as ONE block:
 *
 *   - the instance core, <omxdsp/fx/omx_eq_instance.h>, one per leg, called directly through its
 *     setters: the face's binding must hand every control to it unchanged;
 *   - the console's EQ as the desk runs it: the strip's coefficient bank built from the band
 *     settings (omx_eq_design_f per band in EqBand order, a band that is off dropped and the slots
 *     closed up, a gain shape at 0 dB parked, then the HPF and the LPF as one or two Butterworth
 *     sections), the console's clamps (clampEqFreq, clampEqGain, clampEqQFor, clampFilterFreq)
 *     written out here, and the bank run through omx_biquad_cascade_stereo, the desk's stereo
 *     applier, in chunks of at most OMX_EQ_MAX_BANDS sections.
 *
 * Equal output proves the face calls the console's EQ, its controls land on the same sections, and
 * the state carries across a host's block boundary as it does inside one block. Then two laws on
 * the face's own audio: bypass and the EQ switched off are the input, bit for bit; a busy bank
 * moves it.
 *
 * The LV2 face is also held to the console bundle's port hints: every frequency logarithmic in
 * hertz, every gain in decibels, every band type an enumeration of the console's six labels and
 * every slope an enumeration of 12 and 24 dB/oct.
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
#include <lv2/core/lv2.h>

#ifndef OMX_EQ_LV2_BANDS
#error "build with -DOMX_EQ_LV2_BANDS=8, 16 or 32"
#endif
#include <omxdsp/fx/omx_eq_instance.h>
#include <omxdsp/omx_biquad.h>
#include <omxdsp/omx_denormal.h>
#include <omxdsp/omxdsp_definitions.h>       /* the EQ's travels and band count and the oracle rates, omx-dsp's */
#include <omxdsp/omx_eq_design.h>

#define BANDS ((uint32_t)OMX_EQ_LV2_BANDS)
static const float RATES[] = OMX_ORACLE_FULL_RATES_INIT; /* the nine rates an audio interface clocks at */
#define QUANTUM 256u
#define SEGMENTS 7u
#define SEG_FRAMES (QUANTUM * 100u) /* 25 600 frames: 0.13 s at 192 kHz, 0.8 s at 32 kHz */
#define FRAMES (SEG_FRAMES * SEGMENTS)

static int failures, checks;

static void check(bool ok, const char *face, const char *what) {
  checks++;
  if (!ok) failures++;
  printf("%s %s %s\n", ok ? "PASS" : "FAIL", face, what);
}

/* ---- the programme ------------------------------------------------------------------------ */

/* One segment's controls, as a host writes them: every value inside its declared travel. */
typedef struct {
  float type, freq, gain, q, on;
} Band;
typedef struct {
  float bypass, on;
  float hpf_on, hpf_freq, hpf_slope, lpf_on, lpf_freq, lpf_slope;
  Band band[OMX_EQ_LV2_BANDS];
  const char *name;
} Ctl;
static Ctl PROGRAMME[SEGMENTS];

/* The bands spread log across 30 Hz .. 12 kHz (inside Nyquist at 32 kHz), every type in turn. */
static void make_programme(void) {
  static const char *const NAMES[SEGMENTS] = {
      "every band on, every type, HPF 12 dB/oct",
      "every band moved, HPF and LPF at 24 dB/oct",
      "odd bands off (the bank closes up), even bells at 0 dB (parked)",
      "bypassed",
      "the EQ switched off",
      "back on: Q at its floor and its ceiling, a bell asking for a notch's Q",
      "every band off, the HPF alone",
  };
  for (uint32_t s = 0; s < SEGMENTS; s++) {
    Ctl *c = &PROGRAMME[s];
    memset(c, 0, sizeof *c);
    c->name = NAMES[s];
    c->on = 1.0f;
    c->hpf_freq = 80.0f, c->hpf_slope = 12.0f, c->lpf_freq = 12000.0f, c->lpf_slope = 12.0f;
    for (uint32_t b = 0; b < BANDS; b++) {
      Band *x = &c->band[b];
      const float at = BANDS > 1u ? (float)b / (float)(BANDS - 1u) : 0.0f;
      x->type = (float)(b % OMX_EQ_LV2_TYPE_COUNT);
      x->freq = 30.0f * powf(400.0f, at);
      x->gain = (b % 2u ? -1.0f : 1.0f) * (3.0f + (float)(b % 5u) * 2.5f);
      x->q = x->type == OMX_EQ_LV2_NOTCH ? 30.0f : 0.5f + (float)(b % 4u) * 0.75f;
      x->on = 1.0f;
      switch (s) {
      case 1:
        x->freq *= 1.3f;
        x->gain = -x->gain * 0.8f;
        x->type = (float)((b + 1u) % OMX_EQ_LV2_TYPE_COUNT);
        break;
      case 2:
        x->on = b % 2u ? 0.0f : 1.0f;
        if (b % 2u == 0u) x->type = OMX_EQ_LV2_BELL, x->gain = 0.0f;
        if (b % 4u == 0u) x->gain = 6.0f; /* every fourth band still cuts, so the bank is not empty */
        break;
      case 5:
        x->q = b % 3u == 0u ? 0.3f : b % 3u == 1u ? 116.0f : 8.0f;
        break;
      case 6:
        x->on = 0.0f;
        break;
      default:
        break;
      }
    }
    if (s == 0u) c->hpf_on = 1.0f;
    if (s == 1u) c->hpf_on = 1.0f, c->hpf_slope = 24.0f, c->hpf_freq = 150.0f, c->lpf_on = 1.0f, c->lpf_slope = 24.0f;
    if (s == 2u) c->lpf_on = 1.0f, c->lpf_freq = 6000.0f;
    if (s == 3u) c->bypass = 1.0f, c->hpf_on = 1.0f, c->lpf_on = 1.0f;
    if (s == 4u) c->on = 0.0f, c->hpf_on = 1.0f;
    if (s == 5u) c->lpf_on = 1.0f, c->lpf_freq = 9000.0f;
    if (s == 6u) c->hpf_on = 1.0f, c->hpf_freq = 400.0f, c->hpf_slope = 24.0f;
  }
}

static float in_l[FRAMES], in_r[FRAMES];
static float out_l[FRAMES], out_r[FRAMES];
static float ref_l[FRAMES], ref_r[FRAMES], con_l[FRAMES], con_r[FRAMES];

static uint32_t lcg;
static float noise(void) {
  lcg = lcg * 1664525u + 1013904223u;
  return (float)(int32_t)lcg * (1.0f / 2147483648.0f);
}

/* Noise with a low and a high tone on top, different on each leg. */
static void make_input(float rate) {
  lcg = 7u;
  const float tau = 6.283185307f;
  for (uint32_t i = 0; i < FRAMES; i++) {
    const float t = (float)i / rate;
    in_l[i] = 0.2f * noise() + 0.2f * sinf(tau * 110.0f * t) + 0.1f * sinf(tau * 5000.0f * t);
    in_r[i] = 0.2f * noise() + 0.2f * sinf(tau * 70.0f * t) + 0.1f * sinf(tau * 9000.0f * t);
  }
}

/* ---- the references --------------------------------------------------------------------- */

/** The instance core called directly, one per leg, one block per segment. */
static void reference_instance(float rate) {
  static struct omx_eq_lv2 leg[2];
  omx_eq_lv2_init(&leg[0], rate);
  omx_eq_lv2_init(&leg[1], rate);
  for (uint32_t s = 0; s < SEGMENTS; s++) {
    const Ctl *c = &PROGRAMME[s];
    const uint32_t off = s * SEG_FRAMES;
    for (int k = 0; k < 2; k++) {
      struct omx_eq_lv2 *e = &leg[k];
      omx_eq_lv2_set_on(e, c->bypass < 0.5f && c->on > 0.5f);
      omx_eq_lv2_set_hpf(e, c->hpf_on > 0.5f, c->hpf_freq, c->hpf_slope >= 24.0f);
      omx_eq_lv2_set_lpf(e, c->lpf_on > 0.5f, c->lpf_freq, c->lpf_slope >= 24.0f);
      for (uint32_t b = 0; b < BANDS; b++) {
        const Band *x = &c->band[b];
        omx_eq_lv2_set_band(e, b, (int)x->type, x->freq, x->gain, x->q, x->on > 0.5f);
      }
      omx_eq_lv2_run(e, k ? in_r + off : in_l + off, k ? ref_r + off : ref_l + off, SEG_FRAMES);
    }
  }
}

static float clampf(float v, float lo, float hi) { return v < lo ? lo : v > hi ? hi : v; }

/** The console's EQ: the strip's bank, built here from the settings, through the desk's applier. */
static void reference_console(float rate) {
  enum { CAP = OMX_EQ_LV2_BANDS + 4 };
  static float st_l[CAP][4], st_r[CAP][4];
  memset(st_l, 0, sizeof st_l), memset(st_r, 0, sizeof st_r);
  memcpy(con_l, in_l, sizeof con_l);
  memcpy(con_r, in_r, sizeof con_r);
  static const enum omx_eq_kind KIND[OMX_EQ_LV2_TYPE_COUNT] = {OMX_EQ_PEAKING, OMX_EQ_LOWSHELF, OMX_EQ_HIGHSHELF,
                                                               OMX_EQ_NOTCH,   OMX_EQ_ALLPASS1, OMX_EQ_ALLPASS2};
  for (uint32_t s = 0; s < SEGMENTS; s++) {
    const Ctl *c = &PROGRAMME[s];
    float coeffs[CAP][5];
    uint8_t enabled[CAP];
    uint32_t n = 0;
    if (c->bypass < 0.5f && c->on > 0.5f) {
      for (uint32_t b = 0; b < BANDS; b++) {
        const Band *x = &c->band[b];
        if (x->on <= 0.5f) continue; /* off: not in the bank, the slots close up */
        const int type = (int)x->type;
        const float freq = clampf(x->freq, OMX_EQ_FREQ_RANGE_MIN, OMX_EQ_FREQ_RANGE_MAX);
        const float gain = clampf(x->gain, OMX_EQ_GAIN_RANGE_MIN, OMX_EQ_GAIN_RANGE_MAX);
        const float q = type == OMX_EQ_LV2_NOTCH ? clampf(x->q, OMX_EQ_NOTCH_Q_RANGE_MIN, OMX_EQ_NOTCH_Q_RANGE_MAX)
                                                 : clampf(x->q, OMX_EQ_Q_RANGE_MIN, OMX_EQ_Q_RANGE_MAX);
        omx_eq_design_f(KIND[type], freq, q, gain, rate, coeffs[n]);
        const int has_gain = type == OMX_EQ_LV2_BELL || type == OMX_EQ_LV2_LOWSHELF || type == OMX_EQ_LV2_HIGHSHELF;
        enabled[n++] = has_gain && gain == 0.0f ? 0u : 1u; /* eqBandIsIdentity: in the bank, parked */
      }
      const struct {
        float on, freq, slope, lo, hi;
        enum omx_eq_kind kind;
      } pass[2] = {{c->hpf_on, c->hpf_freq, c->hpf_slope, OMX_HPF_FREQ_RANGE_MIN, OMX_HPF_FREQ_RANGE_MAX, OMX_EQ_HIGHPASS},
                   {c->lpf_on, c->lpf_freq, c->lpf_slope, OMX_LPF_FREQ_RANGE_MIN, OMX_LPF_FREQ_RANGE_MAX, OMX_EQ_LOWPASS}};
      for (int p = 0; p < 2; p++) {
        if (pass[p].on <= 0.5f) continue;
        double qs[2];
        const uint32_t sections = omx_eq_butterworth_qs(pass[p].slope >= 24.0f, qs);
        for (uint32_t k = 0; k < sections; k++) {
          omx_eq_design_f(pass[p].kind, clampf(pass[p].freq, pass[p].lo, pass[p].hi), qs[k], 0.0, rate, coeffs[n]);
          enabled[n++] = 1u;
        }
      }
    }
    const uint32_t off = s * SEG_FRAMES;
    for (uint32_t b0 = 0; b0 < n; b0 += OMX_EQ_MAX_BANDS) {
      const uint32_t nb = n - b0 > OMX_EQ_MAX_BANDS ? OMX_EQ_MAX_BANDS : n - b0;
      omx_biquad_cascade_stereo(con_l + off, con_r + off, SEG_FRAMES, nb, (const float(*)[5])coeffs[b0], &enabled[b0],
                                &st_l[b0], &st_r[b0]);
    }
  }
}

/* ---- the controls, by name and by symbol ------------------------------------------------- */

/* Control k of a segment: 0 the bypass, 1..7 the EQ switch and the pass filters, then five per band. */
#define CONTROLS (8u + 5u * BANDS)
static float ctl_value(const Ctl *c, uint32_t k) {
  const float head[8] = {c->bypass, c->on, c->hpf_on, c->hpf_freq, c->hpf_slope, c->lpf_on, c->lpf_freq, c->lpf_slope};
  if (k < 8u) return head[k];
  const Band *x = &c->band[(k - 8u) / 5u];
  const float v[5] = {x->type, x->freq, x->gain, x->q, x->on};
  return v[(k - 8u) % 5u];
}
static const char *const HEAD_NAMES[8] = {NULL, "EQ On", "HPF On", "HPF Frequency", "HPF Slope", "LPF On", "LPF Frequency", "LPF Slope"};
static const char *const HEAD_SYMBOLS[8] = {"enabled", "on", "hpf_on", "hpf_freq", "hpf_slope", "lpf_on", "lpf_freq", "lpf_slope"};
static const char *const BAND_NAMES[5] = {"Type", "Frequency", "Gain", "Q", "On"};
static const char *const BAND_SYMBOLS[5] = {"type", "freq", "gain", "q", "on"};

static void ctl_name(uint32_t k, char *buf, size_t n) {
  if (k < 8u) snprintf(buf, n, "%s", HEAD_NAMES[k] ? HEAD_NAMES[k] : "");
  else snprintf(buf, n, "Band %u %s", (k - 8u) / 5u + 1u, BAND_NAMES[(k - 8u) % 5u]);
}
static void ctl_symbol(uint32_t k, char *buf, size_t n) {
  if (k < 8u) snprintf(buf, n, "%s", HEAD_SYMBOLS[k]);
  else snprintf(buf, n, "b%u_%s", (k - 8u) / 5u + 1u, BAND_SYMBOLS[(k - 8u) % 5u]);
}

/* ---- the faces ---------------------------------------------------------------------------- */

typedef bool (*RunFn)(void *face, float rate);

/* CLAP */

static const void *h_ext(const clap_host_t *h, const char *id) {
  (void)h, (void)id;
  return NULL;
}
static void h_noop(const clap_host_t *h) { (void)h; }
static const clap_host_t HOST = {CLAP_VERSION_INIT, NULL, "eq-identity", "openmixer", "", "0.1", h_ext, h_noop, h_noop, h_noop};

typedef struct {
  clap_event_param_value_t ev[CONTROLS];
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
static void ev_add(Events *e, clap_id id, double v) {
  clap_event_param_value_t *x = &e->ev[e->n++];
  memset(x, 0, sizeof *x);
  x->header = (clap_event_header_t){sizeof *x, 0, CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_PARAM_VALUE, 0};
  x->param_id = id, x->note_id = -1, x->port_index = -1, x->channel = -1, x->key = -1, x->value = v;
}

typedef struct {
  void *lib;
  const clap_plugin_entry_t *entry;
  const clap_plugin_factory_t *factory;
} ClapFace;

/** The param whose name is `name`, or the bypass param when `name` is empty. */
static bool clap_param(const clap_plugin_params_t *pp, const clap_plugin_t *p, const char *name, clap_id *id) {
  for (uint32_t i = 0; i < pp->count(p); i++) {
    clap_param_info_t info;
    if (!pp->get_info(p, i, &info)) continue;
    if (*name ? strcmp(info.name, name) == 0 : (info.flags & CLAP_PARAM_IS_BYPASS) != 0) {
      *id = info.id;
      return true;
    }
  }
  fprintf(stderr, "  no CLAP parameter named %s\n", *name ? name : "(bypass)");
  return false;
}

static bool clap_run(void *face, float rate) {
  ClapFace *c = (ClapFace *)face;
  const clap_plugin_t *p = c->factory->create_plugin(c->factory, &HOST, c->factory->get_plugin_descriptor(c->factory, 0)->id);
  if (!p || !p->init(p)) return false;
  const clap_plugin_params_t *pp = (const clap_plugin_params_t *)p->get_extension(p, CLAP_EXT_PARAMS);
  bool ok = pp != NULL;
  static clap_id ids[CONTROLS];
  for (uint32_t k = 0; ok && k < CONTROLS; k++) {
    char name[64];
    ctl_name(k, name, sizeof name);
    ok = clap_param(pp, p, name, &ids[k]);
  }
  ok = ok && p->activate(p, rate, 1, QUANTUM) && p->start_processing(p);
  for (uint32_t s = 0; ok && s < SEGMENTS; s++) {
    for (uint32_t f = s * SEG_FRAMES; ok && f < (s + 1u) * SEG_FRAMES; f += QUANTUM) {
      static Events evs;
      evs.n = 0;
      if (f == s * SEG_FRAMES)
        for (uint32_t k = 0; k < CONTROLS; k++) ev_add(&evs, ids[k], ctl_value(&PROGRAMME[s], k));
      clap_input_events_t in = {&evs, ev_size, ev_get};
      clap_output_events_t out = {NULL, ev_push};
      float *ib[2] = {in_l + f, in_r + f}, *ob[2] = {out_l + f, out_r + f};
      clap_audio_buffer_t ai = {ib, NULL, 2, 0, 0}, ao = {ob, NULL, 2, 0, 0};
      clap_process_t pr = {.steady_time = f, .frames_count = QUANTUM, .audio_inputs = &ai, .audio_outputs = &ao,
                           .audio_inputs_count = 1, .audio_outputs_count = 1, .in_events = &in, .out_events = &out};
      ok = p->process(p, &pr) != CLAP_PROCESS_ERROR;
    }
  }
  if (pp) {
    p->stop_processing(p);
    p->deactivate(p);
  }
  p->destroy(p);
  return ok;
}

/* LV2 */

typedef struct {
  LilvWorld *world;
  const LilvPlugin *plugin;
} Lv2Face;

static const LilvPort *port_by(const Lv2Face *l, const char *symbol) {
  LilvNode *sym = lilv_new_string(l->world, symbol);
  const LilvPort *port = lilv_plugin_get_port_by_symbol(l->plugin, sym);
  lilv_node_free(sym);
  if (!port) fprintf(stderr, "  no LV2 port '%s'\n", symbol);
  return port;
}
static int32_t port_of(const Lv2Face *l, const char *symbol) {
  const LilvPort *port = port_by(l, symbol);
  return port ? (int32_t)lilv_port_get_index(l->plugin, port) : -1;
}

static bool lv2_run(void *face, float rate) {
  Lv2Face *l = (Lv2Face *)face;
  LilvInstance *inst = lilv_plugin_instantiate(l->plugin, rate, NULL);
  if (!inst) return false;
  static float cv[CONTROLS];
  float latency = 0.0f;
  const int32_t pil = port_of(l, "in_l"), pir = port_of(l, "in_r"), pol = port_of(l, "out_l"), por = port_of(l, "out_r"),
                plat = port_of(l, "latency");
  bool ok = pil >= 0 && pir >= 0 && pol >= 0 && por >= 0 && plat >= 0;
  for (uint32_t k = 0; k < CONTROLS; k++) {
    char sym[32];
    ctl_symbol(k, sym, sizeof sym);
    const int32_t idx = port_of(l, sym);
    ok = idx >= 0 && ok;
    if (idx >= 0) lilv_instance_connect_port(inst, (uint32_t)idx, &cv[k]);
  }
  if (ok) {
    lilv_instance_connect_port(inst, (uint32_t)plat, &latency);
    lilv_instance_activate(inst);
  }
  for (uint32_t s = 0; ok && s < SEGMENTS; s++) {
    for (uint32_t k = 0; k < CONTROLS; k++) cv[k] = ctl_value(&PROGRAMME[s], k);
    cv[0] = PROGRAMME[s].bypass > 0.5f ? 0.0f : 1.0f; /* lv2:enabled is the bypass inverted */
    for (uint32_t f = s * SEG_FRAMES; f < (s + 1u) * SEG_FRAMES; f += QUANTUM) {
      lilv_instance_connect_port(inst, (uint32_t)pil, in_l + f);
      lilv_instance_connect_port(inst, (uint32_t)pir, in_r + f);
      lilv_instance_connect_port(inst, (uint32_t)pol, out_l + f);
      lilv_instance_connect_port(inst, (uint32_t)por, out_r + f);
      lilv_instance_run(inst, QUANTUM);
    }
  }
  if (ok) lilv_instance_deactivate(inst);
  lilv_instance_free(inst);
  return ok;
}

/* The console bundle's hints: what a host draws from them, and what a session saved against the
 * console's omx-eqN.lv2 found there. */
static void lv2_hints(Lv2Face *l) {
  static const char *const TYPES[OMX_EQ_LV2_TYPE_COUNT] = {"Bell", "Low Shelf", "High Shelf", "Notch", "All-pass 1st", "All-pass 2nd"};
  LilvNode *log = lilv_new_uri(l->world, "http://lv2plug.in/ns/ext/port-props#logarithmic");
  LilvNode *enumeration = lilv_new_uri(l->world, LV2_CORE__enumeration);
  LilvNode *unit = lilv_new_uri(l->world, "http://lv2plug.in/ns/extensions/units#unit");
  LilvNode *hz = lilv_new_uri(l->world, "http://lv2plug.in/ns/extensions/units#hz");
  LilvNode *db = lilv_new_uri(l->world, "http://lv2plug.in/ns/extensions/units#db");
  char what[200];
  uint32_t bad_freq = 0, bad_gain = 0, bad_type = 0, bad_slope = 0;
  for (uint32_t k = 1; k < CONTROLS; k++) {
    char sym[32];
    ctl_symbol(k, sym, sizeof sym);
    const LilvPort *port = port_by(l, sym);
    if (!port) continue;
    const size_t len = strlen(sym);
    const bool freq = len > 5 && strcmp(sym + len - 5, "_freq") == 0, gain = len > 5 && strcmp(sym + len - 5, "_gain") == 0;
    const bool type = len > 5 && strcmp(sym + len - 5, "_type") == 0, slope = len > 6 && strcmp(sym + len - 6, "_slope") == 0;
    LilvNodes *units = lilv_port_get_value(l->plugin, port, unit);
    const LilvNode *u = units ? lilv_nodes_get_first(units) : NULL;
    if (freq) bad_freq += !lilv_port_has_property(l->plugin, port, log) || !u || !lilv_node_equals(u, hz);
    if (gain) bad_gain += !u || !lilv_node_equals(u, db);
    lilv_nodes_free(units);
    if (type || slope) {
      LilvScalePoints *points = lilv_port_get_scale_points(l->plugin, port);
      bool ok = lilv_port_has_property(l->plugin, port, enumeration) && points &&
                lilv_scale_points_size(points) == (type ? (unsigned)OMX_EQ_LV2_TYPE_COUNT : 2u);
      LILV_FOREACH (scale_points, i, points) {
        const LilvScalePoint *sp = lilv_scale_points_get(points, i);
        const int v = (int)lilv_node_as_float(lilv_scale_point_get_value(sp));
        const char *label = lilv_node_as_string(lilv_scale_point_get_label(sp));
        char want[16];
        snprintf(want, sizeof want, "%d dB/oct", v);
        ok = ok && (type ? v >= 0 && v < OMX_EQ_LV2_TYPE_COUNT && strcmp(label, TYPES[v]) == 0
                         : (v == 12 || v == 24) && strcmp(label, want) == 0);
      }
      if (points) lilv_scale_points_free(points);
      if (type) bad_type += !ok;
      else bad_slope += !ok;
    }
  }
  snprintf(what, sizeof what, "every frequency is logarithmic, in hertz (%u of %u not)", bad_freq, BANDS + 2u);
  check(bad_freq == 0, "lv2", what);
  snprintf(what, sizeof what, "every gain is in decibels (%u of %u not)", bad_gain, BANDS);
  check(bad_gain == 0, "lv2", what);
  snprintf(what, sizeof what, "every band type lists the console's six types by name (%u of %u not)", bad_type, BANDS);
  check(bad_type == 0, "lv2", what);
  snprintf(what, sizeof what, "both slopes list 12 and 24 dB/oct (%u of 2 not)", bad_slope);
  check(bad_slope == 0, "lv2", what);
  lilv_node_free(log), lilv_node_free(enumeration), lilv_node_free(unit), lilv_node_free(hz), lilv_node_free(db);
}

/* ---- the oracle --------------------------------------------------------------------------- */

static void oracle(const char *face, void *h, RunFn run) {
  char what[240];
  make_programme();
  for (size_t ri = 0; ri < sizeof RATES / sizeof RATES[0]; ri++) {
    const float rate = RATES[ri];
    make_input(rate);
    reference_instance(rate);
    reference_console(rate);
    memset(out_l, 0, sizeof out_l), memset(out_r, 0, sizeof out_r);
    snprintf(what, sizeof what, "@ %.0f Hz: the programme runs", (double)rate);
    if (!run(h, rate)) {
      check(false, face, what);
      continue;
    }
    /* Bit for bit, per segment, so a red names where. */
    for (uint32_t s = 0; s < SEGMENTS; s++) {
      const uint32_t off = s * SEG_FRAMES;
      uint32_t di = 0, dc = 0, moved = 0;
      for (uint32_t i = off; i < off + SEG_FRAMES; i++) {
        di += out_l[i] != ref_l[i] || out_r[i] != ref_r[i];
        dc += out_l[i] != con_l[i] || out_r[i] != con_r[i];
        moved += out_l[i] != in_l[i] || out_r[i] != in_r[i];
      }
      snprintf(what, sizeof what, "@ %.0f Hz segment %u (%s): face == omx_eq_instance bit for bit (%u frames differ)",
               (double)rate, s, PROGRAMME[s].name, di);
      check(di == 0, face, what);
      snprintf(what, sizeof what, "@ %.0f Hz segment %u: face == the console's bank through omx_biquad_cascade_stereo bit for bit (%u frames differ)",
               (double)rate, s, dc);
      check(dc == 0, face, what);
      const bool wire = PROGRAMME[s].bypass > 0.5f || PROGRAMME[s].on < 0.5f;
      snprintf(what, sizeof what, "@ %.0f Hz segment %u: %s (%u frames moved)", (double)rate, s,
               wire ? "the output is the input" : "the bank moves the audio", moved);
      check(wire ? moved == 0u : moved > SEG_FRAMES / 2u, face, what);
    }
  }
}

static int clap_main(const char *path) {
  ClapFace c = {dlopen(path, RTLD_NOW | RTLD_LOCAL), NULL, NULL};
  if (!c.lib) return fprintf(stderr, "FAIL dlopen %s: %s\n", path, dlerror()), 1;
  c.entry = (const clap_plugin_entry_t *)dlsym(c.lib, "clap_entry");
  if (!c.entry || !c.entry->init(path)) return fprintf(stderr, "FAIL clap_entry %s\n", path), 1;
  c.factory = (const clap_plugin_factory_t *)c.entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  if (!c.factory || c.factory->get_plugin_count(c.factory) != 1) return fprintf(stderr, "FAIL factory\n"), 1;
  oracle("clap", &c, clap_run);
  c.entry->deinit();
  dlclose(c.lib);
  return failures ? 1 : 0;
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
  lv2_hints(&l);
  oracle("lv2", &l, lv2_run);
  lilv_node_free(u), lilv_node_free(b);
  lilv_world_free(l.world);
  return failures ? 1 : 0;
}

int main(int argc, char **argv) {
  /* The faces flush denormals on every callback; the references run under the same mode. */
  omx_denormals_off();
  int rc;
  if (argc == 3 && strcmp(argv[1], "clap") == 0) rc = clap_main(argv[2]);
  else if (argc == 4 && strcmp(argv[1], "lv2") == 0) rc = lv2_main(argv[2], argv[3]);
  else return fprintf(stderr, "usage: eq-identity clap <x.clap> | lv2 <bundle-dir> <uri>\n"), 2;
  printf("%s %s: %d checks, %d failure(s)\n", rc ? "ORACLE-RED" : "ORACLE-GREEN", argv[1], checks, failures);
  return rc;
}
