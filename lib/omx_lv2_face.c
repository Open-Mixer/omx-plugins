// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_lv2_face.c — the LV2 face of every plugin, in plain C: one plugin's struct omx_face
 * (include/omx_plugin_face.h), its generated declaration over omx-dsp's kernel, reached through its
 * omx_<kernel>_core.h's vtable. No DSP of its own: flush denormals, read the control ports (an
 * unconnected one is its declared default), resolve, run; the `enabled` port is the host's bypass and
 * `latency` reports the kernel's. A plugin that declares meters writes each channel's value after every
 * block to its output control port (docs/meters.md), the values clap.meters/1 publishes on the CLAP face.
 *
 * A keyed plugin's sidechain key (lv2:isSideChain, connectionOptional) follows in_l and in_r; an
 * unconnected key is the kernel's own detector.
 *
 * A plugin that reads the host's tempo (its descriptor's needs.tempo) has one more input, after
 * `latency`: `control`, an atom:Sequence that supports time:Position (connectionOptional). Each
 * time:Position object's time:beatsPerMinute becomes the tempo, held until the next one (a host sends a
 * position when it changes). With no urid:map, no port connected or no tempo received yet the tempo is
 * none (0), and the kernel keeps its time control.
 */
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "omx_face_meters.h"
#include "omx_lv2_face.h"
#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>
#include <lv2/time/time.h>
#include <lv2/urid/urid.h>
#include "omx_meter_ring.h"
#include <omxdsp/omx_denormal.h>

/* The URIDs a tempo reader needs, mapped once at instantiate. */
typedef struct {
  LV2_URID object, blank, position, bpm, a_float, a_double, a_int, a_long;
} TimeUrids;

/* One allocation: this, the control ports and the run's values (each param_count long), the meter ports
 * (meter_values long), then the core. */
typedef struct {
  const struct omx_face *face;
  void *core;
  uint32_t rate;
  const float *in_l, *in_r;
  const float *key;
  float *out_l, *out_r;
  const float *enabled;
  float *latency;
  const float **param;
  float *values;
  float **meter;
  const LV2_Atom_Sequence *control; /* time:Position in; NULL: not connected */
  int mapped;                       /* the host gave urid:map */
  TimeUrids u;
  double bpm; /* the last time:beatsPerMinute received; 0: none yet */
  int bypass; /* the bypass the core last resolved */
  int dirty;  /* resolve before the next block: a value or the bypass moved, or the core is fresh */
  double tempo; /* the tempo the core last resolved with; 0: none */
} Face;

LV2_Handle omx_lv2_face_instantiate(const LV2_Descriptor *d, double rate, const char *bundle,
                                    const LV2_Feature *const *features) {
  (void)bundle;
  if (!(rate > 0.0)) return NULL;
  const struct omx_face *f = ((const struct omx_lv2_face *)d)->face;
  const size_t n = f->param_count;
  const size_t at_values = sizeof(Face) + n * sizeof(const float *);
  const size_t at_meters = (at_values + n * sizeof(float) + alignof(float *) - 1u) / alignof(float *) * alignof(float *);
  const size_t at_core = (at_meters + f->meter_values * sizeof(float *) + alignof(max_align_t) - 1u) / alignof(max_align_t) * alignof(max_align_t);
  char *mem = (char *)calloc(1, at_core + f->core->size);
  if (!mem) return NULL;
  Face *s = (Face *)mem;
  s->face = f;
  s->param = (const float **)(mem + sizeof(Face));
  s->values = (float *)(mem + at_values);
  s->meter = (float **)(mem + at_meters);
  s->core = mem + at_core;
  s->rate = (uint32_t)rate;
  if (omx_face_reads_tempo(f))
    for (const LV2_Feature *const *x = features; x && *x; ++x)
      if (strcmp((*x)->URI, LV2_URID__map) == 0 && (*x)->data) {
        const LV2_URID_Map *m = (const LV2_URID_Map *)(*x)->data;
        s->u = (TimeUrids){m->map(m->handle, LV2_ATOM__Object), m->map(m->handle, LV2_ATOM__Blank),
                           m->map(m->handle, LV2_TIME__Position), m->map(m->handle, LV2_TIME__beatsPerMinute),
                           m->map(m->handle, LV2_ATOM__Float),  m->map(m->handle, LV2_ATOM__Double),
                           m->map(m->handle, LV2_ATOM__Int),    m->map(m->handle, LV2_ATOM__Long)};
        s->mapped = 1;
      }
  f->core->init(s->core, s->rate);
  s->dirty = 1;
  return s;
}

void omx_lv2_face_connect_port(LV2_Handle h, uint32_t port, void *data) {
  Face *s = (Face *)h;
  const struct omx_face_lv2_ports *p = &s->face->lv2;
  if (port == p->in_l) s->in_l = (const float *)data;
  else if (port == p->in_r) s->in_r = (const float *)data;
  else if (s->face->key_symbol && port == p->key) s->key = (const float *)data;
  else if (port == p->out_l) s->out_l = (float *)data;
  else if (port == p->out_r) s->out_r = (float *)data;
  else if (port == p->enabled) s->enabled = (const float *)data;
  else if (port == p->latency) s->latency = (float *)data;
  else if (omx_face_reads_tempo(s->face) && port == s->face->lv2_control) s->control = (const LV2_Atom_Sequence *)data;
  else if (port >= p->first_param && port < p->first_param + s->face->param_count)
    s->param[port - p->first_param] = (const float *)data;
  else if (port >= p->first_meter && port < p->first_meter + s->face->meter_values)
    s->meter[port - p->first_meter] = (float *)data;
}

void omx_lv2_face_activate(LV2_Handle h) {
  Face *s = (Face *)h;
  s->face->core->init(s->core, s->rate);
  s->dirty = 1;
}

static float value(const Face *s, uint32_t i) { return s->param[i] ? *s->param[i] : s->face->params[i].def; }

/** A number atom's value, or -1 when `a` is none of float, double, int, long. */
static double number(const Face *s, const LV2_Atom *a) {
  if (!a) return -1.0;
  if (a->type == s->u.a_float) return ((const LV2_Atom_Float *)a)->body;
  if (a->type == s->u.a_double) return ((const LV2_Atom_Double *)a)->body;
  if (a->type == s->u.a_int) return ((const LV2_Atom_Int *)a)->body;
  if (a->type == s->u.a_long) return (double)((const LV2_Atom_Long *)a)->body;
  return -1.0;
}

/** The block's tempo: the last time:Position's time:beatsPerMinute in the control sequence, kept across
 * blocks; none (0) with no urid:map or nothing connected. */
static double block_tempo(Face *s) {
  if (!s->mapped || !s->control) return 0.0;
  LV2_ATOM_SEQUENCE_FOREACH(s->control, ev) {
    const LV2_Atom *a = &ev->body;
    if (a->type != s->u.object && a->type != s->u.blank) continue;
    const LV2_Atom_Object *o = (const LV2_Atom_Object *)a;
    if (o->body.otype != s->u.position) continue;
    const LV2_Atom *bpm = NULL;
    lv2_atom_object_get(o, s->u.bpm, &bpm, 0);
    const double v = number(s, bpm);
    if (v > 0.0) s->bpm = v;
  }
  return s->bpm;
}

void omx_lv2_face_run(LV2_Handle h, uint32_t frames) {
  Face *s = (Face *)h;
  const struct omx_face_core *c = s->face->core;
  omx_denormals_off();
  const int bypass = s->enabled && *s->enabled < 0.5f;
  /* A block whose control ports and tempo read what the core last resolved skips the resolve: the core keeps its
   * smoothing as it is, which resolving the same values again gives, bit for bit. */
  int dirty = s->dirty || bypass != s->bypass;
  for (uint32_t i = 0; i < s->face->param_count; ++i) {
    const float v = value(s, i);
    if (v != s->values[i]) s->values[i] = v, dirty = 1;
  }
  if (omx_face_reads_tempo(s->face)) {
    const double bpm = block_tempo(s);
    if (bpm != s->tempo) s->tempo = bpm, dirty = 1;
    s->face->tempo(s->core, bpm);
  }
  if (dirty) c->resolve(s->core, s->values, bypass), s->bypass = bypass, s->dirty = 0;
  if (s->latency) *s->latency = (float)c->latency(s->core);
  const struct omx_face *f = s->face;
  double mv[OMX_METER_RING_MAX];
  /* the inputs' peak, before the kernel runs (the outputs may be the inputs) */
  if (f->meter_count) omx_face_meter_levels(f, OMX_FACE_METER_IN, s->in_l, s->in_r, frames, mv);
  c->run(s->core, s->key, s->in_l, s->in_r, s->out_l, s->out_r, frames);
  if (f->meter_count) {
    omx_face_meter_readings(f, s->core, bypass, mv);
    omx_face_meter_levels(f, OMX_FACE_METER_OUT, s->out_l, s->out_r, frames, mv);
    for (uint32_t i = 0; i < f->meter_values; ++i)
      if (s->meter[i]) *s->meter[i] = (float)mv[i];
  }
}

void omx_lv2_face_cleanup(LV2_Handle h) { free(h); }

/* org.openmixer.panel-description/1: the declaration's panel, its parameter ids the LV2 port indices
 * (the first parameter's port added). */
static const char *panel_fingerprint(void *h) { return ((const Face *)h)->face->panel->fingerprint; }

static uint32_t panel_family(void *h) { return ((const Face *)h)->face->panel->family; }

static uint32_t panel_count(void *h, uint32_t table) { return omx_panel_count(((const Face *)h)->face->panel, table); }

static bool panel_get(void *h, uint32_t table, uint32_t index, void *out) {
  const struct omx_face *f = ((const Face *)h)->face;
  return omx_panel_get(f->panel, table, index, out, f->lv2.first_param);
}

static const omx_lv2_panel_description_t PANEL = {panel_fingerprint, panel_family, panel_count, panel_get};

const void *omx_lv2_face_extension_data(const char *uri) {
  if (strcmp(uri, OMX_PANEL_DESCRIPTION_ID) == 0) return &PANEL;
  return NULL;
}
