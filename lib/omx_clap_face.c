// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_clap_face.c — the CLAP face of every plugin, in plain C over our CLAP ABI (the released
 * omx-clap-host's <omx-clap-host/omx_clap_ext.h> for the openmixer extensions), over one plugin's
 * struct omx_face (include/omx_plugin_face.h): its numbers are its generated/omx_<kernel>_params.h's,
 * its DSP omx-dsp's kernel reached through its omx_<kernel>_core.h's vtable. This file adds no
 * arithmetic on the audio path: drain the events, resolve, run.
 *
 * Extensions: params, latency, audio-ports, state, preset-load; org.openmixer.declaration/1, panel-description/1
 * and param-contract/1; clap.remote-controls (the panel's sections as pages of eight) and
 * clap.track-info (a placement's defaults, the contract's per-kind ones, on every parameter the host
 * or a state has not set); org.openmixer.resource-hints.draft/0 (the declaration's resource hints).
 * clap.meters/1, for a plugin that declares meters: every one coherent: after each block the face publishes
 * all their values at once through include/omx_meter_ring.h, and a read copies one block's (docs/meters.md).
 * A gain reduction is the kernel's own reading after the block, 0 while bypassed. A level the kernel reads
 * (its detector's) is its own reading after the block, the display minimum while bypassed. An input level is
 * the block's peak at the inputs, in dBFS, measured here before the kernel runs (the kernel keeps no level of
 * its own); an output level the block's peak at the outputs, measured here after the kernel ran.
 *
 * Tempo: a plugin whose descriptor says needs.tempo is handed each block's tempo from clap_process.transport
 * (its `tempo` when CLAP_TRANSPORT_HAS_TEMPO is set), and none (0) when the host hands no transport or no tempo.
 *
 * Audio ports: the main stereo pair in and out; a keyed plugin's second, mono, non-main input is the
 * sidechain key. A host that routes nothing to the key (no second port in the process call, no channel,
 * or no buffer) gets the kernel's own detector, as the face's NULL key is.
 */
#include <stdalign.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clap/entry.h"
#include "clap/events.h"
#include "clap/ext/audio-ports.h"
#include "clap/ext/latency.h"
#include "clap/ext/params.h"
#include "clap/ext/preset-load.h"
#include "clap/ext/remote-controls.h"
#include "clap/ext/state.h"
#include "clap/ext/track-info.h"
#include "clap/factory/plugin-factory.h"
#include "clap/factory/preset-discovery.h"
#include "clap/plugin.h"
#include "clap/process.h"
#include "clap/stream.h"

#include "omx_clap_face.h"
#include <omxcontract/omx_plugin_face_ids.h> /* the host bypass's permanent CLAP id */
#include <omx-clap-host/omx_clap_ext.h>
#include <omx-clap-host/clap_meters.h>
#include "omx_meter_ring.h"
#include "omx_preset_file.h"
#include <omxdsp/omx_denormal.h>    /* omx_denormals_off, per callback */
#include "omx_face_meters.h"         /* one block's meter values, the LV2 face's too */

_Static_assert((int)OMX_FACE_METER_KIND_GAIN_REDUCTION == (int)CLAP_METER_KIND_GAIN_REDUCTION &&
                   (int)OMX_FACE_METER_KIND_LEVEL == (int)CLAP_METER_KIND_LEVEL,
               "a face meter's kind is clap.meters/1's");

/* The host's bypass: the slot after the declared ones, and CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), the
 * same in every plugin and never moved by an appended parameter. A declared parameter's id is its slot. */
#define G_DECLARED(g) ((g)->face->param_count)
#define G_BYPASS(g) G_DECLARED(g)
#define G_PARAMS(g) (G_DECLARED(g) + 1u)
#define G_NO_SLOT UINT32_MAX

#define G_GOV_MAX 8u

/* One allocation: this, then the values, the applied values, the touched flags and the core, each
 * G_PARAMS long (the core the vtable's size). */
typedef struct {
  clap_plugin_t plugin;
  const clap_host_t *host;
  const struct omx_clap_face *clap;
  const struct omx_face *face;
  void *core;
  uint32_t rate;                   /* the rate of the last activation; 0 before one */
  float *values;                   /* audio-thread owned */
  _Atomic float *applied;          /* the APPLIED values, published after each drain */
  _Atomic uint32_t latency;        /* the frames the host was told of at the last activation */
  _Atomic int restart_asked;       /* a restart asked for since, for a latency the values moved */
  const clap_host_latency_t *host_latency; /* the host's clap.latency, when it offers one */
  const clap_host_params_t *host_params;   /* the host's clap.params, when it offers one */
  int active, processing;
  const clap_host_track_info_t *track_info; /* the host's clap.track-info, when it offers one */
  unsigned char *touched;                   /* set by the host or a state: a placement default leaves it */
  _Atomic uint32_t placed;                  /* a placement for the audio thread to apply, plus one; 0 none */
  struct omx_meter_ring meters;             /* every meter's values after the last block; none since activation */
  const clap_host_remote_controls_t *host_pages; /* the host's clap.remote-controls, when it offers one */
  uint32_t gov_count;                       /* the parameters that decide which controls a page holds (visibleWhen) */
  uint32_t gov[G_GOV_MAX];
  _Atomic float paged[G_GOV_MAX];           /* each one's value when the pages were last announced */
  _Atomic int pages_dirty;                  /* a governing value moved: the host is told on its main thread */
  _Atomic int dirty;                        /* the values moved since the core last resolved them: resolve before the next block */
  double tempo;                             /* the tempo the core last resolved with, BPM; 0 none (audio thread) */
} G;

#define G_OF(p) ((G *)(p)->plugin_data)

/** CLAP id -> slot: a declared id is its own, the bypass id the bypass's slot; G_NO_SLOT for any other. */
static uint32_t g_slot(const G *g, clap_id id) {
  if (id < G_DECLARED(g)) return id;
  return id == OMX_PLUGIN_FACE_BYPASS_CLAP_ID ? G_BYPASS(g) : G_NO_SLOT;
}

/** Slot -> CLAP id. */
static clap_id g_id(const G *g, uint32_t slot) { return slot == G_BYPASS(g) ? OMX_PLUGIN_FACE_BYPASS_CLAP_ID : slot; }

static float g_min(const G *g, uint32_t i) { return i < G_DECLARED(g) ? g->face->params[i].min : 0.0f; }
static float g_max(const G *g, uint32_t i) { return i < G_DECLARED(g) ? g->face->params[i].max : 1.0f; }
static float g_def(const G *g, uint32_t i) { return i < G_DECLARED(g) ? g->face->params[i].def : 0.0f; }

/** Into the declared travel; NaN floors. */
static float g_clamp(const G *g, uint32_t i, double v) {
  if (!(v > (double)g_min(g, i))) return g_min(g, i);
  if (v > (double)g_max(g, i)) return g_max(g, i);
  return (float)v;
}

static void g_publish(G *g) {
  for (uint32_t i = 0; i < G_PARAMS(g); ++i) atomic_store_explicit(&g->applied[i], g->values[i], memory_order_relaxed);
  /* A value that decides which controls the pages hold moved: the pages are a function of it, so the host is asked,
   * on its main thread, to read them again (clap.remote-controls changed()). */
  int moved = 0;
  for (uint32_t k = 0; k < g->gov_count; ++k)
    if (g->values[g->gov[k]] != atomic_load_explicit(&g->paged[k], memory_order_relaxed)) {
      atomic_store_explicit(&g->paged[k], g->values[g->gov[k]], memory_order_relaxed);
      moved = 1;
    }
  if (moved && !atomic_exchange(&g->pages_dirty, 1) && g->host && g->host->request_callback) g->host->request_callback(g->host);
}

/** The placement's defaults (the panel's placement defaults) on every parameter that has one and
 * that neither the host nor a state has set: the placement's value, or the parameter's own default
 * where the placement states none. */
static void g_place(G *g, uint32_t placement) {
  const struct omx_panel_rows *r = g->face->panel;
  for (uint32_t k = 0; k < r->default_count; ++k) {
    const uint32_t i = r->defaults[k].param;
    if (g->touched[i]) continue;
    g->values[i] = g_def(g, i);
    for (uint32_t j = 0; j < r->default_count; ++j)
      if (r->defaults[j].param == i && r->defaults[j].placement == placement) g->values[i] = g_clamp(g, i, r->defaults[j].value);
  }
}

/** Frame 0: a placement the host changed while active, then every value event in the block, are
 * applied before the kernel runs. A value that moved marks the values dirty; a block with no event leaves
 * them, and the core, as they are. */
static void g_drain(G *g, const clap_input_events_t *in) {
  const uint32_t placed = atomic_exchange_explicit(&g->placed, 0, memory_order_acquire);
  if (placed) g_place(g, placed - 1), atomic_store_explicit(&g->dirty, 1, memory_order_relaxed);
  const uint32_t n = in ? in->size(in) : 0u;
  for (uint32_t k = 0; k < n; ++k) {
    const clap_event_header_t *h = in->get(in, k);
    if (!h || h->space_id != CLAP_CORE_EVENT_SPACE_ID || h->type != CLAP_EVENT_PARAM_VALUE) continue;
    const clap_event_param_value_t *e = (const clap_event_param_value_t *)h;
    const uint32_t i = g_slot(g, e->param_id);
    if (i != G_NO_SLOT) {
      const float v = g_clamp(g, i, e->value);
      if (v != g->values[i]) g->values[i] = v, atomic_store_explicit(&g->dirty, 1, memory_order_relaxed);
      g->touched[i] = 1;
    }
  }
}

/** Where the host placed the instance, from its clap.track-info: enum omx_clap_panel_placement. */
static uint32_t g_placement(const G *g) {
  clap_track_info_t info;
  memset(&info, 0, sizeof info);
  if (!g->track_info || !g->track_info->get(g->host, &info)) return OMX_CLAP_PLACEMENT_INSERT;
  if (info.flags & CLAP_TRACK_INFO_IS_FOR_RETURN_TRACK) return OMX_CLAP_PLACEMENT_RETURN;
  if (info.flags & CLAP_TRACK_INFO_IS_FOR_BUS) return OMX_CLAP_PLACEMENT_BUS;
  if (info.flags & CLAP_TRACK_INFO_IS_FOR_MASTER) return OMX_CLAP_PLACEMENT_MASTER;
  return OMX_CLAP_PLACEMENT_INSERT;
}

/* ---- clap_plugin ------------------------------------------------------------------------ */

static bool g_init(const clap_plugin_t *p) {
  G *g = G_OF(p);
  if (g->host && g->host->get_extension)
    g->track_info = (const clap_host_track_info_t *)g->host->get_extension(g->host, CLAP_EXT_TRACK_INFO);
  if (g->host && g->host->get_extension)
    g->host_latency = (const clap_host_latency_t *)g->host->get_extension(g->host, CLAP_EXT_LATENCY);
  if (g->host && g->host->get_extension)
    g->host_params = (const clap_host_params_t *)g->host->get_extension(g->host, CLAP_EXT_PARAMS);
  if (g->host && g->host->get_extension)
    g->host_pages = (const clap_host_remote_controls_t *)g->host->get_extension(g->host, CLAP_EXT_REMOTE_CONTROLS);
  g_place(g, g_placement(g));
  g_publish(g);
  if (g->face->meter_count) omx_meter_ring_init(&g->meters);
  return true;
}

static void g_destroy(const clap_plugin_t *p) { free(G_OF(p)); }

static bool g_activate(const clap_plugin_t *p, double sample_rate, uint32_t min_frames, uint32_t max_frames) {
  (void)min_frames;
  (void)max_frames;
  G *g = G_OF(p);
  const struct omx_face_core *c = g->face->core;
  if (g->active || !(sample_rate > 0.0)) return false;
  g->rate = (uint32_t)sample_rate;
  c->init(g->core, g->rate);
  /* The latency the host reads after activation is the one the current values give: resolved on a
   * probe instance, so the running core's state is not touched before its first block. Activation is
   * the one place the CLAP latency extension lets it change, and where the host is told it did. */
  void *probe = malloc(c->size);
  if (!probe) return false;
  c->init(probe, g->rate);
  c->resolve(probe, g->values, g->values[G_BYPASS(g)] > 0.5f);
  const uint32_t lat = c->latency(probe);
  free(probe);
  if (atomic_exchange(&g->latency, lat) != lat && g->host_latency && g->host_latency->changed)
    g->host_latency->changed(g->host);
  atomic_store(&g->restart_asked, 0);
  atomic_store(&g->dirty, 1); /* a fresh core resolves before its first block */
  if (g->face->meter_count) omx_meter_ring_clear(&g->meters);
  g->active = 1;
  return true;
}

static void g_deactivate(const clap_plugin_t *p) {
  G *g = G_OF(p);
  if (g->face->meter_count) omx_meter_ring_clear(&g->meters);
  g->active = 0;
}

static bool g_start_processing(const clap_plugin_t *p) {
  G_OF(p)->processing = 1;
  return true;
}

static void g_stop_processing(const clap_plugin_t *p) { G_OF(p)->processing = 0; }

static void g_reset(const clap_plugin_t *p) {
  G *g = G_OF(p);
  g->face->core->init(g->core, g->rate);
  atomic_store(&g->dirty, 1);
}

static clap_process_status g_process(const clap_plugin_t *p, const clap_process_t *pr) {
  G *g = G_OF(p);
  const struct omx_face *f = g->face;
  omx_denormals_off();
  g_drain(g, pr->in_events);
  if (pr->audio_inputs_count < 1 || pr->audio_outputs_count < 1) return CLAP_PROCESS_ERROR;
  const clap_audio_buffer_t *ai = &pr->audio_inputs[0];
  const clap_audio_buffer_t *ao = &pr->audio_outputs[0];
  if (ai->channel_count < 2 || ao->channel_count < 2 || !ai->data32 || !ao->data32) return CLAP_PROCESS_ERROR;
  const int bypassed = g->values[G_BYPASS(g)] > 0.5f;
  /* A quiet block (no value moved, the same tempo) skips the resolve: the core keeps its smoothing as it is, which is
   * what resolving the same values again gives, bit for bit (each plugin's oracle, its quiet arm). */
  int dirty = atomic_exchange_explicit(&g->dirty, 0, memory_order_acquire);
  /* the block's tempo, for a plugin that reads one: the transport's when it carries a tempo, else none (0); the core
   * takes it at its next resolve, so a tempo that moved resolves */
  if (f->tempo) {
    const clap_event_transport_t *t = pr->transport;
    const double bpm = t && (t->flags & CLAP_TRANSPORT_HAS_TEMPO) ? t->tempo : 0.0;
    if (bpm != g->tempo) g->tempo = bpm, dirty = 1;
    f->tempo(g->core, bpm);
  }
  if (dirty) f->core->resolve(g->core, g->values, bypassed);
  double mv[OMX_METER_RING_MAX];
  /* the inputs' peak, before the kernel runs (the outputs may be the inputs) */
  if (f->meter_count) omx_face_meter_levels(f, OMX_FACE_METER_IN, ai->data32[0], ai->data32[1], pr->frames_count, mv);
  const float *key = NULL;
  if (f->key_symbol) {
    const clap_audio_buffer_t *ak = pr->audio_inputs_count > 1 ? &pr->audio_inputs[1] : NULL;
    key = ak && ak->channel_count > 0 && ak->data32 ? ak->data32[0] : NULL;
  }
  f->core->run(g->core, key, ai->data32[0], ai->data32[1], ao->data32[0], ao->data32[1], pr->frames_count);
  g_publish(g);
  if (f->meter_count) {
    omx_face_meter_readings(f, g->core, bypassed, mv);
    omx_face_meter_levels(f, OMX_FACE_METER_OUT, ao->data32[0], ao->data32[1], pr->frames_count, mv);
    omx_meter_ring_publish(&g->meters, mv, f->meter_values);
  }
  /* A parameter that moves the kernel's latency (as the pitch shifter's mix crossing 100 % does):
   * the reported latency holds until the next activation, and the host is asked, once, for the
   * restart that brings it. */
  if (f->core->latency(g->core) != atomic_load_explicit(&g->latency, memory_order_relaxed) &&
      !atomic_exchange_explicit(&g->restart_asked, 1, memory_order_relaxed) && g->host && g->host->request_restart)
    g->host->request_restart(g->host);
  return CLAP_PROCESS_CONTINUE;
}

/* ---- params --------------------------------------------------------------------- */

static uint32_t g_params_count(const clap_plugin_t *p) { return G_PARAMS(G_OF(p)); }

static bool g_params_info(const clap_plugin_t *p, uint32_t i, clap_param_info_t *info) {
  const G *g = G_OF(p);
  if (i >= G_PARAMS(g)) return false;
  memset(info, 0, sizeof *info);
  info->id = g_id(g, i);
  info->flags = CLAP_PARAM_IS_AUTOMATABLE;
  if (i == G_BYPASS(g)) {
    info->flags |= CLAP_PARAM_IS_STEPPED | CLAP_PARAM_IS_BYPASS;
    snprintf(info->name, sizeof info->name, "%s", "Bypass");
  } else {
    if (g->face->params[i].flags & (OMX_PLUGIN_PARAM_INTEGER | OMX_PLUGIN_PARAM_TOGGLE)) info->flags |= CLAP_PARAM_IS_STEPPED;
    snprintf(info->name, sizeof info->name, "%s", g->face->params[i].name);
    snprintf(info->module, sizeof info->module, "%s", g->face->modules[i]);
  }
  info->min_value = g_min(g, i);
  info->max_value = g_max(g, i);
  info->default_value = g_def(g, i);
  return true;
}

/** The APPLIED value, safe on the host's main (control) thread while audio runs. */
static bool g_params_value(const clap_plugin_t *p, clap_id id, double *out) {
  G *g = G_OF(p);
  const uint32_t i = g_slot(g, id);
  if (i == G_NO_SLOT) return false;
  *out = atomic_load_explicit(&g->applied[i], memory_order_relaxed);
  return true;
}

static bool g_params_to_text(const clap_plugin_t *p, clap_id id, double v, char *out, uint32_t size) {
  const G *g = G_OF(p);
  const uint32_t i = g_slot(g, id);
  if (i == G_NO_SLOT) return false;
  const char *unit = i < G_DECLARED(g) ? g->face->params[i].unit : "";
  snprintf(out, size, unit[0] ? "%g %s" : "%g", v, unit);
  return true;
}

static bool g_params_from_text(const clap_plugin_t *p, clap_id id, const char *text, double *out) {
  char *end = NULL;
  if (g_slot(G_OF(p), id) == G_NO_SLOT || !text) return false;
  *out = strtod(text, &end);
  return end != text;
}

static void g_params_flush(const clap_plugin_t *p, const clap_input_events_t *in, const clap_output_events_t *out) {
  (void)out;
  G *g = G_OF(p);
  g_drain(g, in);
  g_publish(g);
}

static const clap_plugin_params_t G_PARAMS_EXT = {g_params_count, g_params_info, g_params_value,
                                                  g_params_to_text, g_params_from_text, g_params_flush};

/* ---- latency, audio-ports ----------------------------------------------------- */

static uint32_t g_latency(const clap_plugin_t *p) { return atomic_load(&G_OF(p)->latency); }

static const clap_plugin_latency_t G_LATENCY_EXT = {g_latency};

/* Inputs: the main pair, then a keyed plugin's key. Outputs: the main pair. */
static uint32_t g_ports_count(const clap_plugin_t *p, bool is_input) { return is_input && G_OF(p)->face->key_symbol ? 2 : 1; }

static bool g_ports_get(const clap_plugin_t *p, uint32_t index, bool is_input, clap_audio_port_info_t *info) {
  if (index >= g_ports_count(p, is_input)) return false;
  memset(info, 0, sizeof *info);
  info->id = index;
  if (index == 1) {
    snprintf(info->name, sizeof info->name, "%s", G_OF(p)->face->key_symbol);
    info->flags = 0;
    info->channel_count = 1;
    info->port_type = CLAP_PORT_MONO;
    info->in_place_pair = CLAP_INVALID_ID;
    return true;
  }
  snprintf(info->name, sizeof info->name, "%s", is_input ? "in" : "out");
  info->flags = CLAP_AUDIO_PORT_IS_MAIN;
  info->channel_count = 2;
  info->port_type = CLAP_PORT_STEREO;
  info->in_place_pair = 0;
  return true;
}

static const clap_plugin_audio_ports_t G_PORTS_EXT = {g_ports_count, g_ports_get};

/* ---- state: symbol=value lines, declaration order, then the host's bypass ------------------- */

/** Parameter `i`'s name in the state: the declaration's symbol, `bypass` for the host's bypass. */
static const char *g_state_symbol(const G *g, uint32_t i) { return i < G_DECLARED(g) ? g->face->params[i].symbol : "bypass"; }

static bool g_state_save(const clap_plugin_t *p, const clap_ostream_t *s) {
  G *g = G_OF(p);
  char line[160];
  for (uint32_t i = 0; i < G_PARAMS(g); ++i) {
    int n = snprintf(line, sizeof line, "%s=%.9g\n", g_state_symbol(g, i), (double)atomic_load(&g->applied[i]));
    for (int off = 0; off < n;) {
      int64_t w = s->write(s, line + off, (uint64_t)(n - off));
      if (w <= 0) return false;
      off += (int)w;
    }
  }
  return true;
}

/* A state the face writes holds at most G_PARAMS lines of a symbol and a %.9g value; this bound leaves
 * room for lines a later version adds, and refuses anything far larger before it is parsed. */
#define G_STATE_MAX(g) (G_PARAMS(g) * 256u)

static bool g_state_load(const clap_plugin_t *p, const clap_istream_t *s) {
  G *g = G_OF(p);
  if (g->processing) return false; /* never race the audio thread's values */
  const size_t max = G_STATE_MAX(g);
  char *buf = malloc(max + 1u);
  if (!buf) return false;
  size_t len = 0;
  for (;;) {
    if (len == max) return free(buf), false;
    int64_t r = s->read(s, buf + len, max - len);
    if (r < 0) return free(buf), false;
    if (r == 0) break;
    len += (size_t)r;
  }
  buf[len] = 0;
  /* parsed first, applied only when at least one line names a parameter: an empty or foreign state
   * is refused and leaves the instance as it was */
  const uint32_t n = G_PARAMS(g);
  float v[n];
  unsigned char set[n];
  memset(set, 0, sizeof set);
  bool any = false;
  for (char *line = strtok(buf, "\n"); line; line = strtok(NULL, "\n")) {
    char *eq = strchr(line, '=');
    if (!eq) continue;
    *eq = 0;
    for (uint32_t i = 0; i < n; ++i) {
      if (strcmp(line, g_state_symbol(g, i)) == 0) {
        v[i] = g_clamp(g, i, strtod(eq + 1, NULL));
        set[i] = 1;
        any = true;
      }
    }
  }
  free(buf);
  if (!any) return false;
  for (uint32_t i = 0; i < n; ++i)
    if (set[i]) g->values[i] = v[i], g->touched[i] = 1;
  atomic_store(&g->dirty, 1);
  g_publish(g);
  /* The values a state set are new to the host: it reads them again (clap.params rescan). */
  if (g->host_params && g->host_params->rescan) g->host_params->rescan(g->host, CLAP_PARAM_RESCAN_VALUES);
  return true;
}

static const clap_plugin_state_t G_STATE_EXT = {g_state_save, g_state_load};

/* ---- clap.preset-load: a factory preset file, its params as the state load applies them -------- */

static void g_preset_param(const char *symbol, double value, void *ctx) {
  G *g = (G *)ctx;
  for (uint32_t i = 0; i < G_DECLARED(g); ++i) {
    if (strcmp(symbol, g->face->params[i].symbol) == 0) {
      g->values[i] = g_clamp(g, i, value);
      g->touched[i] = 1;
      atomic_store(&g->dirty, 1);
    }
  }
}

static bool g_preset_from_location(const clap_plugin_t *p, uint32_t kind, const char *location, const char *load_key) {
  G *g = G_OF(p);
  const clap_host_preset_load_t *h = g->host && g->host->get_extension
      ? (const clap_host_preset_load_t *)g->host->get_extension(g->host, CLAP_EXT_PRESET_LOAD) : NULL;
  if (!h && g->host && g->host->get_extension)
    h = (const clap_host_preset_load_t *)g->host->get_extension(g->host, CLAP_EXT_PRESET_LOAD_COMPAT);
  char *text = kind == CLAP_PRESET_DISCOVERY_LOCATION_FILE && !g->processing ? omx_preset_slurp(location) : NULL;
  struct omx_preset_head head;
  bool ok = text && omx_preset_head(text, g->face->clap_id, &head) && omx_preset_params(&head, g_preset_param, g);
  free(text);
  if (!ok) {
    if (h && h->on_error) h->on_error(g->host, kind, location, load_key, 0, "not an org.openmixer.preset/1 file of this plugin");
    return false;
  }
  g_publish(g);
  /* as after a state load: the values are new to the host, it reads them again */
  if (g->host_params && g->host_params->rescan) g->host_params->rescan(g->host, CLAP_PARAM_RESCAN_VALUES);
  if (h && h->loaded) h->loaded(g->host, kind, location, load_key);
  return true;
}

static const clap_plugin_preset_load_t G_PRESET_LOAD_EXT = {g_preset_from_location};

/* ---- omx.declaration -------------------------------------------------------------- */

static const char *g_decl_source(const clap_plugin_t *p) { return G_OF(p)->face->decl_source; }

static const char *g_decl_digest(const clap_plugin_t *p) { return G_OF(p)->face->decl_digest; }

static const omx_clap_plugin_declaration_t G_DECL_EXT = {g_decl_source, g_decl_digest};

/* ---- org.openmixer.panel-description/1: the declaration's panel ------------------------------ */

static const char *g_panel_fingerprint(const clap_plugin_t *p) { return G_OF(p)->face->panel->fingerprint; }

static uint32_t g_panel_family(const clap_plugin_t *p) { return G_OF(p)->face->panel->family; }

static uint32_t g_panel_count(const clap_plugin_t *p, uint32_t table) { return omx_panel_count(G_OF(p)->face->panel, table); }

static bool g_panel_get(const clap_plugin_t *p, uint32_t table, uint32_t index, void *out) {
  return omx_panel_get(G_OF(p)->face->panel, table, index, out, 0);
}

static const omx_clap_plugin_panel_description_t G_PANEL_EXT = {g_panel_fingerprint, g_panel_family, g_panel_count,
                                                                g_panel_get};

/* ---- org.openmixer.param-contract/1: the declaration's contract, the bypass last --------------- */

/* The scale codes the generator writes into the rows (include/omx_param_contract.h) are the owner's OMX_CLAP_PARAM_SCALES,
 * one each: tools/test/param-units-check.mjs holds their numbers to omx-contract's PARAM_SCALES words. */
#define G_ONE_SCALE(code, name) +1
_Static_assert(0 OMX_CLAP_PARAM_SCALES(G_ONE_SCALE) == OMX_CLAP_SCALE_COUNT, "omx_param_contract.h's scales are not omx-clap-core's");
#undef G_ONE_SCALE

static uint32_t g_contract_version(const clap_plugin_t *p) { return G_OF(p)->face->contract_version; }

static const char *g_contract_fingerprint(const clap_plugin_t *p) { return G_OF(p)->face->contract_fingerprint; }

/* Fills the host's `entry` for CLAP parameter `id` from the face's rows: a declared one (0 to count - 1, its symbol the
 * parameter table's) or the bypass at OMX_PLUGIN_FACE_BYPASS_CLAP_ID; false for any other id. */
static bool g_contract_get(const clap_plugin_t *p, clap_id id, omx_clap_param_contract_entry_t *entry) {
  const G *g = G_OF(p);
  const uint32_t count = G_DECLARED(g);
  const bool bypass_id = id == OMX_PLUGIN_FACE_BYPASS_CLAP_ID;
  if (!entry || (id >= count && !bypass_id)) return false;
  const struct omx_param_contract_row bypass = {OMX_CLAP_UNIT_NONE, OMX_CLAP_SCALE_STEPPED, 1.0};
  const struct omx_param_contract_row *r = bypass_id ? &bypass : &g->face->contract[id];
  const char *s = bypass_id ? "bypass" : g->face->params[id].symbol;
  uint32_t i = 0;
  for (; s && s[i] && i + 1 < OMX_CLAP_PARAM_CONTRACT_SYMBOL_SIZE; ++i) entry->symbol[i] = s[i];
  entry->symbol[i] = 0;
  entry->param_id = id;
  entry->unit = r->unit;
  entry->scale = r->scale;
  entry->step = r->step;
  return true;
}

static const omx_clap_plugin_param_contract_ext_t G_CONTRACT_EXT = {g_contract_version, g_contract_fingerprint, g_contract_get};

/* ---- org.openmixer.resource-hints.draft/0: the declaration's hints -------------------------- */

static bool g_resource_hints_get(const clap_plugin_t *p, omx_resource_hints_t *hints) {
  *hints = *G_OF(p)->face->hints;
  return hints->declared != 0u;
}

static const omx_clap_plugin_resource_hints_t G_RESOURCE_HINTS_EXT = {g_resource_hints_get};

/* ---- clap.remote-controls: the panel's pages ------------------------------------------------ */

/* A control the current values show: no rule, or the governing parameter's value is a bit of the rule's mask. */
static bool g_shown(const G *g, const struct omx_panel_control_row *c) {
  if (c->visible_param == OMX_CLAP_PANEL_INVALID_ID) return true;
  if (c->visible_param >= G_DECLARED(g)) return false;
  const float v = atomic_load_explicit(&g->applied[c->visible_param], memory_order_relaxed);
  if (!(v >= 0.0f && v <= 63.0f) || v != (float)(uint32_t)v) return false;
  return ((c->visible_mask >> (uint32_t)v) & 1u) != 0u;
}

/** Whether the control of parameter `param` is shown at the governing values now: a slot of a hidden control is blank. The
 * page rule is omx-panel's pagesOf: the pages and their boundaries are fixed (the generated table holds every control's
 * id), and a control the governing values hide keeps its slot, blank, so the slots after it do not move. */
static bool g_slot_shown(const G *g, clap_id param) {
  const struct omx_panel_rows *r = g->face->panel;
  for (uint32_t c = 0; c < r->control_count; ++c)
    if (r->controls[c].param == param) return g_shown(g, &r->controls[c]);
  return true;
}

static uint32_t g_pages_count(const clap_plugin_t *p) {
  const G *g = G_OF(p);
  return g->face->page_count;
}

static bool g_pages_get(const clap_plugin_t *p, uint32_t index, clap_remote_controls_page_t *page) {
  const G *g = G_OF(p);
  const struct omx_face *f = g->face;
  if (index >= f->page_count) return false;
  const struct omx_panel_page_row *r = &f->pages[index];
  memset(page, 0, sizeof *page);
  snprintf(page->section_name, sizeof page->section_name, "%s", r->section_name);
  page->page_id = index;
  snprintf(page->page_name, sizeof page->page_name, "%s", r->page_name);
  for (uint32_t k = 0; k < CLAP_REMOTE_CONTROLS_COUNT; ++k)
    page->param_ids[k] = r->ids[k] != OMX_CLAP_PANEL_INVALID_ID && g->gov_count && !g_slot_shown(g, r->ids[k]) ? OMX_CLAP_PANEL_INVALID_ID : r->ids[k];
  page->is_for_preset = false;
  return true;
}

static const clap_plugin_remote_controls_t G_PAGES_EXT = {g_pages_count, g_pages_get};

/* ---- clap.track-info: the placement's defaults, applied now or by the audio thread -------- */

static void g_track_info_changed(const clap_plugin_t *p) {
  G *g = G_OF(p);
  const uint32_t placement = g_placement(g);
  if (g->active) {
    atomic_store_explicit(&g->placed, placement + 1, memory_order_release);
    return;
  }
  g_place(g, placement);
  g_publish(g);
}

static const clap_plugin_track_info_t G_TRACK_INFO_EXT = {g_track_info_changed};

/* ---- clap.meters/1: the declaration's meters (docs/meters.md) ----------------------------- */

static uint32_t g_meters_count(const clap_plugin_t *p) { return G_OF(p)->face->meter_count; }

static bool g_meters_info(const clap_plugin_t *p, uint32_t index, clap_meter_info_t *info) {
  const struct omx_face *f = G_OF(p)->face;
  if (index >= f->meter_count) return false;
  memset(info, 0, sizeof *info);
  info->id = index;
  snprintf(info->name, sizeof info->name, "%s", f->meters[index].name);
  info->kind = f->meters[index].kind;
  info->flags = CLAP_METER_IS_COHERENT;
  info->channel_count = f->meters[index].channels;
  info->min_value = f->meters[index].min;
  info->max_value = f->meters[index].max;
  return true;
}

/** The last block's values of meter `id`, all of that one block, from any thread; nothing before a block
 * since activation. */
static uint32_t g_meters_values(const clap_plugin_t *p, clap_id id, double *values, uint32_t capacity) {
  G *g = G_OF(p);
  const struct omx_face *f = g->face;
  if (id >= f->meter_count || capacity < 1 || !values) return 0;
  const uint32_t n = capacity < f->meters[id].channels ? capacity : f->meters[id].channels;
  return omx_meter_ring_read(&g->meters, f->meters[id].at, values, n);
}

static const clap_plugin_meters_t G_METERS_EXT = {g_meters_count, g_meters_info, g_meters_values};

static const void *g_get_extension(const clap_plugin_t *p, const char *id) {
  if (strcmp(id, CLAP_EXT_PARAMS) == 0) return &G_PARAMS_EXT;
  if (strcmp(id, CLAP_EXT_LATENCY) == 0) return &G_LATENCY_EXT;
  if (strcmp(id, CLAP_EXT_AUDIO_PORTS) == 0) return &G_PORTS_EXT;
  if (strcmp(id, CLAP_EXT_STATE) == 0) return &G_STATE_EXT;
  if (strcmp(id, CLAP_EXT_PRESET_LOAD) == 0 || strcmp(id, CLAP_EXT_PRESET_LOAD_COMPAT) == 0) return &G_PRESET_LOAD_EXT;
  if (strcmp(id, OMX_CLAP_EXT_DECLARATION) == 0) return &G_DECL_EXT;
  if (strcmp(id, OMX_PANEL_DESCRIPTION_ID) == 0) return &G_PANEL_EXT;
  if (strcmp(id, OMX_CLAP_EXT_PARAM_CONTRACT) == 0) return &G_CONTRACT_EXT;
  if (strcmp(id, CLAP_EXT_REMOTE_CONTROLS) == 0) return &G_PAGES_EXT;
  if (strcmp(id, CLAP_EXT_TRACK_INFO) == 0) return &G_TRACK_INFO_EXT;
  if (strcmp(id, OMX_CLAP_EXT_RESOURCE_HINTS_DRAFT) == 0) return &G_RESOURCE_HINTS_EXT;
  if (G_OF(p)->face->meter_count && strcmp(id, CLAP_EXT_METERS) == 0) return &G_METERS_EXT;
  return NULL;
}

static void g_on_main_thread(const clap_plugin_t *p) {
  G *g = G_OF(p);
  if (atomic_exchange(&g->pages_dirty, 0) && g->host_pages && g->host_pages->changed) g->host_pages->changed(g->host);
}

/* ---- factory ---------------------------------------------------------------------------- */

#define ALIGN_UP(n, a) (((n) + (a)-1u) / (a) * (a))

static const clap_plugin_t *g_create(const struct omx_clap_face *cf, const clap_host_t *host) {
  const struct omx_face *f = cf->face;
  const size_t n = f->param_count + 1u;
  const size_t at_applied = ALIGN_UP(sizeof(G) + n * sizeof(float), alignof(_Atomic float));
  const size_t at_touched = at_applied + n * sizeof(_Atomic float);
  const size_t at_core = ALIGN_UP(at_touched + n, alignof(max_align_t));
  /* the plugin, the core and the values, one allocation */
  char *mem = (char *)calloc(1, at_core + f->core->size);
  if (!mem) return NULL;
  G *g = (G *)mem;
  g->values = (float *)(mem + sizeof(G));
  g->applied = (_Atomic float *)(mem + at_applied);
  g->touched = (unsigned char *)(mem + at_touched);
  g->core = mem + at_core;
  g->host = host;
  g->clap = cf;
  g->face = f;
  g->plugin.desc = &cf->desc;
  g->plugin.plugin_data = g;
  g->plugin.init = g_init;
  g->plugin.destroy = g_destroy;
  g->plugin.activate = g_activate;
  g->plugin.deactivate = g_deactivate;
  g->plugin.start_processing = g_start_processing;
  g->plugin.stop_processing = g_stop_processing;
  g->plugin.reset = g_reset;
  g->plugin.process = g_process;
  g->plugin.get_extension = g_get_extension;
  g->plugin.on_main_thread = g_on_main_thread;
  for (uint32_t i = 0; i < G_PARAMS(g); ++i) g->values[i] = g_def(g, i);
  /* the parameters the panel's visibility rules read: the pages follow them */
  const struct omx_panel_rows *pr = f->panel;
  for (uint32_t c = 0; pr && c < pr->control_count; ++c) {
    const uint32_t gp = pr->controls[c].visible_param;
    if (gp == OMX_CLAP_PANEL_INVALID_ID || gp >= G_DECLARED(g)) continue;
    uint32_t k = 0;
    while (k < g->gov_count && g->gov[k] != gp) ++k;
    if (k == g->gov_count && g->gov_count < G_GOV_MAX) {
      g->gov[g->gov_count++] = gp;
      atomic_store_explicit(&g->paged[k], g->values[gp], memory_order_relaxed);
    }
  }
  g_publish(g);
  return &g->plugin;
}

/* The factories are the first and second members of the plugin's struct omx_clap_face. */
static const struct omx_clap_face *of_factory(const clap_plugin_factory_t *f) { return (const struct omx_clap_face *)f; }
static const struct omx_clap_face *of_presets(const clap_preset_discovery_factory_t *f) {
  return (const struct omx_clap_face *)((const char *)f - offsetof(struct omx_clap_face, presets));
}

uint32_t omx_clap_face_count(const clap_plugin_factory_t *f) {
  (void)f;
  return 1;
}

const clap_plugin_descriptor_t *omx_clap_face_desc(const clap_plugin_factory_t *f, uint32_t i) {
  return i == 0 ? &of_factory(f)->desc : NULL;
}

const clap_plugin_t *omx_clap_face_create(const clap_plugin_factory_t *f, const clap_host_t *host, const char *id) {
  const struct omx_clap_face *cf = of_factory(f);
  return id && strcmp(id, cf->desc.id) == 0 ? g_create(cf, host) : NULL;
}

/* ---- clap.preset-discovery: the factory presets' location, one preset per file --------------- */

typedef struct {
  clap_preset_discovery_provider_t provider;
  const clap_preset_discovery_indexer_t *indexer;
  const struct omx_clap_face *clap;
} G_PROVIDER;

static bool p_init(const clap_preset_discovery_provider_t *pv) {
  const G_PROVIDER *gp = (const G_PROVIDER *)pv->provider_data;
  const clap_preset_discovery_indexer_t *ix = gp->indexer;
  const struct omx_clap_face *cf = gp->clap;
  const clap_preset_discovery_filetype_t type = {"OpenMixer preset", OMX_PRESET_FORMAT, "json"};
  const clap_preset_discovery_soundpack_t pack = {CLAP_PRESET_DISCOVERY_IS_FACTORY_CONTENT, cf->desc.id, cf->provider.name,
                                                  NULL, cf->desc.url, cf->desc.vendor, NULL, CLAP_TIMESTAMP_UNKNOWN};
  const clap_preset_discovery_location_t where = {CLAP_PRESET_DISCOVERY_IS_FACTORY_CONTENT, cf->provider.name,
                                                  CLAP_PRESET_DISCOVERY_LOCATION_FILE, cf->preset_location};
  return ix->declare_filetype(ix, &type) && ix->declare_soundpack(ix, &pack) && ix->declare_location(ix, &where);
}

static void p_destroy(const clap_preset_discovery_provider_t *pv) { free(pv->provider_data); }

static bool p_get_metadata(const clap_preset_discovery_provider_t *pv, uint32_t kind, const char *location,
                           const clap_preset_discovery_metadata_receiver_t *r) {
  const char *clap_id = ((const G_PROVIDER *)pv->provider_data)->clap->desc.id;
  if (kind != CLAP_PRESET_DISCOVERY_LOCATION_FILE) return false;
  char *text = omx_preset_slurp(location);
  struct omx_preset_head head;
  bool ok = text && omx_preset_head(text, clap_id, &head);
  free(text);
  if (!ok) return true; /* not one of ours: no preset in it */
  if (!r->begin_preset(r, head.name, NULL)) return true;
  const clap_universal_plugin_id_t id = {"clap", clap_id};
  r->add_plugin_id(r, &id);
  r->set_soundpack_id(r, clap_id);
  r->set_flags(r, CLAP_PRESET_DISCOVERY_IS_FACTORY_CONTENT);
  return true;
}

static const void *p_get_extension(const clap_preset_discovery_provider_t *pv, const char *id) {
  (void)pv;
  (void)id;
  return NULL;
}

uint32_t omx_clap_face_presets_count(const clap_preset_discovery_factory_t *f) {
  (void)f;
  return 1;
}

const clap_preset_discovery_provider_descriptor_t *omx_clap_face_presets_desc(const clap_preset_discovery_factory_t *f,
                                                                             uint32_t i) {
  return i == 0 ? &of_presets(f)->provider : NULL;
}

const clap_preset_discovery_provider_t *omx_clap_face_presets_create(const clap_preset_discovery_factory_t *f,
                                                                     const clap_preset_discovery_indexer_t *ix,
                                                                     const char *id) {
  const struct omx_clap_face *cf = of_presets(f);
  if (!ix || !id || strcmp(id, cf->provider.id) != 0) return NULL;
  G_PROVIDER *pv = (G_PROVIDER *)calloc(1, sizeof *pv);
  if (!pv) return NULL;
  pv->indexer = ix;
  pv->clap = cf;
  pv->provider = (clap_preset_discovery_provider_t){&cf->provider, pv, p_init, p_destroy, p_get_metadata, p_get_extension};
  return &pv->provider;
}

/* ---- entry ------------------------------------------------------------------------------ */

bool omx_clap_face_init(const char *path) {
  (void)path;
  return true;
}

void omx_clap_face_deinit(void) {}

const void *omx_clap_face_factory(const struct omx_clap_face *f, const char *id) {
  if (strcmp(id, CLAP_PLUGIN_FACTORY_ID) == 0) return &f->factory;
  if (strcmp(id, CLAP_PRESET_DISCOVERY_FACTORY_ID) == 0 || strcmp(id, CLAP_PRESET_DISCOVERY_FACTORY_ID_COMPAT) == 0) return &f->presets;
  return NULL;
}
