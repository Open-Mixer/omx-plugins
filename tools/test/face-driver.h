// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * face-driver.h — a plugin's BUILT faces as a hand-written oracle (the declaration's `oracles`) drives them:
 * one instance at a rate, its parameters set each block, stereo audio and an optional key in, and its meters
 * read after a block by the symbol a host reads them by (docs/meters.md): through clap.meters/1 on the CLAP
 * face (a meter's name), through its output control port on the LV2 face (the port's symbol).
 *
 *   static const struct face_spec SPEC = {OMX_<K>_PARAMS, OMX_<K>_PARAM_COUNT, "key" or NULL, "<oracle>"};
 *   static void arms(const Face *f) { ... f->open, f->run, f->meter, f->close ... }
 *   int main(int argc, char **argv) { return face_main(argc, argv, &SPEC, arms); }
 *
 * The oracle names a face on its command line as the generated Makefile runs it: `clap <x.clap>` or
 * `lv2 <bundle-dir> <uri>`. Parameters are found BY NAME (CLAP) and BY SYMBOL (LV2), the host's bypass by its
 * flag and the `enabled` port, so a renumbered face cannot pass by accident. `face_ok` counts and prints each
 * check; face_main returns 1 when one failed.
 */
#ifndef OMX_FACE_DRIVER_H
#define OMX_FACE_DRIVER_H

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

#include "omx_plugin_param.h"
#include <omx-clap-host/clap_meters.h>
#include <omxdsp/omx_denormal.h>

/** What an oracle drives: the declared parameters (declaration order), the key port's symbol (NULL: none),
 * and its own name, for the host it plays. */
struct face_spec {
  const omx_plugin_param *params;
  uint32_t count;
  const char *key_symbol;
  const char *name;
};

/** The largest block an oracle runs. */
#define FACE_BLOCK_MAX 4096u
#define FACE_PARAMS_MAX 128u

static const struct face_spec *g_spec;
static int g_checks = 0, g_failed = 0;
static const char *g_face = "";
static double g_sr = 0.0;

/** One check: PASS or FAIL, the face and the rate. */
static void face_ok(bool cond, const char *what) {
  g_checks++;
  if (!cond) g_failed++;
  printf("%s %s %s @ %.0f Hz\n", cond ? "PASS" : "FAIL", g_face, what, g_sr);
}

typedef struct Inst Inst;
typedef struct {
  Inst *(*open)(void *ctx, double sr);
  /* one block of `n` frames at the values `v` (declaration order), bypassed or not; `key` NULL routes none */
  bool (*run)(Inst *in, const float *v, bool bypass, const float *key, const float *il, const float *ir, float *ol,
              float *orr, uint32_t n);
  /* the meter channel read by `symbol` after the last block: false when the face has none of that symbol */
  bool (*meter)(Inst *in, const char *symbol, double *value);
  void (*close)(Inst *in);
  void *ctx;
} Face;

struct Inst {
  /* CLAP */
  const clap_plugin_t *p;
  const clap_plugin_meters_t *mx;
  clap_id ids[FACE_PARAMS_MAX + 1];
  uint64_t steady;
  /* LV2 */
  const LilvPlugin *plugin;
  LilvWorld *world;
  LilvInstance *lv;
  int32_t ap[5], cp[FACE_PARAMS_MAX + 2];
  float cv[FACE_PARAMS_MAX + 2];
  float *outs; /* every output control port's value, by port index */
};

/* ---- CLAP ---------------------------------------------------------------------------------------- */

static const void *face_h_ext(const clap_host_t *h, const char *id) {
  (void)h, (void)id;
  return NULL;
}
static void face_h_noop(const clap_host_t *h) { (void)h; }

typedef struct {
  clap_event_param_value_t ev[FACE_PARAMS_MAX + 1];
  uint32_t n;
} FaceEvents;
static uint32_t face_ev_size(const clap_input_events_t *l) { return ((const FaceEvents *)l->ctx)->n; }
static const clap_event_header_t *face_ev_get(const clap_input_events_t *l, uint32_t i) {
  return &((const FaceEvents *)l->ctx)->ev[i].header;
}
static bool face_ev_push(const clap_output_events_t *l, const clap_event_header_t *e) {
  (void)l, (void)e;
  return true;
}

static bool face_clap_param(const clap_plugin_params_t *pp, const clap_plugin_t *p, const char *name, clap_id *id) {
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

static void face_clap_close(Inst *in) {
  in->p->stop_processing(in->p);
  in->p->deactivate(in->p);
  in->p->destroy(in->p);
  free(in);
}

static Inst *face_clap_open(void *ctx, double sr) {
  static clap_host_t host = {CLAP_VERSION_INIT, NULL, "", "openmixer", "", "0.1", face_h_ext, face_h_noop, face_h_noop, face_h_noop};
  host.name = g_spec->name;
  const clap_plugin_factory_t *factory = ctx;
  Inst *in = calloc(1, sizeof *in);
  if (!in) abort();
  in->p = factory->create_plugin(factory, &host, factory->get_plugin_descriptor(factory, 0)->id);
  if (!in->p || !in->p->init(in->p)) return free(in), NULL;
  const clap_plugin_params_t *pp = (const clap_plugin_params_t *)in->p->get_extension(in->p, CLAP_EXT_PARAMS);
  in->mx = (const clap_plugin_meters_t *)in->p->get_extension(in->p, CLAP_EXT_METERS);
  bool good = pp != NULL;
  for (uint32_t i = 0; good && i < g_spec->count; i++) good = face_clap_param(pp, in->p, g_spec->params[i].name, &in->ids[i]);
  good = good && face_clap_param(pp, in->p, NULL, &in->ids[g_spec->count]);
  if (!good || !in->p->activate(in->p, sr, 1, FACE_BLOCK_MAX) || !in->p->start_processing(in->p)) {
    in->p->destroy(in->p);
    return free(in), NULL;
  }
  return in;
}

static bool face_clap_run(Inst *in, const float *v, bool bypass, const float *key, const float *il, const float *ir,
                          float *ol, float *orr, uint32_t n) {
  const uint32_t np = g_spec->count;
  FaceEvents evs = {.n = 0};
  for (uint32_t i = 0; i <= np; i++) {
    clap_event_param_value_t *e = &evs.ev[evs.n++];
    memset(e, 0, sizeof *e);
    e->header = (clap_event_header_t){sizeof *e, 0, CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_PARAM_VALUE, 0};
    e->param_id = in->ids[i], e->note_id = -1, e->port_index = -1, e->channel = -1, e->key = -1;
    e->value = i < np ? (double)v[i] : (bypass ? 1.0 : 0.0);
  }
  clap_input_events_t ie = {&evs, face_ev_size, face_ev_get};
  clap_output_events_t oe = {NULL, face_ev_push};
  float *ib[2] = {(float *)il, (float *)ir}, *ob[2] = {ol, orr}, *kb[1] = {(float *)key};
  clap_audio_buffer_t ai[2] = {{ib, NULL, 2, 0, 0}, {kb, NULL, 1, 0, 0}}, ao = {ob, NULL, 2, 0, 0};
  clap_process_t pr = {.steady_time = (int64_t)in->steady, .frames_count = n, .audio_inputs = ai, .audio_outputs = &ao,
                       .audio_inputs_count = key ? 2u : 1u, .audio_outputs_count = 1, .in_events = &ie, .out_events = &oe};
  in->steady += n;
  return in->p->process(in->p, &pr) != CLAP_PROCESS_ERROR;
}

/* A meter by the symbol a host reads it by: its name, one channel (omx-clap doc/reference.md). */
static bool face_clap_meter(Inst *in, const char *symbol, double *value) {
  if (!in->mx) return false;
  for (uint32_t m = 0; m < in->mx->count(in->p); m++) {
    clap_meter_info_t info;
    if (in->mx->get_info(in->p, m, &info) && strcmp(info.name, symbol) == 0 && info.channel_count == 1u)
      return in->mx->get_values(in->p, info.id, value, 1) == 1;
  }
  return false;
}

static int face_clap_main(const char *path, void (*arms)(const Face *)) {
  void *lib = dlopen(path, RTLD_NOW | RTLD_LOCAL);
  if (!lib) return fprintf(stderr, "FAIL dlopen %s: %s\n", path, dlerror()), 1;
  const clap_plugin_entry_t *entry = (const clap_plugin_entry_t *)dlsym(lib, "clap_entry");
  if (!entry || !entry->init(path)) return fprintf(stderr, "FAIL clap_entry %s\n", path), 1;
  const clap_plugin_factory_t *factory = (const clap_plugin_factory_t *)entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  if (!factory || factory->get_plugin_count(factory) != 1) return fprintf(stderr, "FAIL factory\n"), 1;
  Face f = {face_clap_open, face_clap_run, face_clap_meter, face_clap_close, (void *)factory};
  g_face = "clap";
  arms(&f);
  entry->deinit();
  dlclose(lib);
  return g_failed ? 1 : 0;
}

/* ---- LV2 ----------------------------------------------------------------------------------------- */

typedef struct {
  LilvWorld *world;
  const LilvPlugin *plugin;
} FaceLv2;

static int32_t face_port_of(const FaceLv2 *l, const char *symbol, bool quiet) {
  LilvNode *sym = lilv_new_string(l->world, symbol);
  const LilvPort *port = lilv_plugin_get_port_by_symbol(l->plugin, sym);
  lilv_node_free(sym);
  if (!port && !quiet) fprintf(stderr, "  no LV2 port '%s'\n", symbol);
  return port ? (int32_t)lilv_port_get_index(l->plugin, port) : -1;
}

static void face_lv2_close(Inst *in) {
  lilv_instance_deactivate(in->lv);
  lilv_instance_free(in->lv);
  free(in->outs);
  free(in);
}

/* Every output control port (the latency and each meter channel) is connected to a slot of its own. */
static Inst *face_lv2_open(void *ctx, double sr) {
  const FaceLv2 *l = ctx;
  const uint32_t np = g_spec->count;
  Inst *in = calloc(1, sizeof *in);
  if (!in) abort();
  in->plugin = l->plugin, in->world = l->world;
  const char *audio[] = {"in_l", "in_r", "out_l", "out_r", g_spec->key_symbol};
  bool good = true;
  for (int i = 0; i < (g_spec->key_symbol ? 5 : 4); i++) good = (in->ap[i] = face_port_of(l, audio[i], false)) >= 0 && good;
  for (uint32_t i = 0; i < np; i++) good = (in->cp[i] = face_port_of(l, g_spec->params[i].symbol, false)) >= 0 && good;
  good = (in->cp[np] = face_port_of(l, "enabled", false)) >= 0 && good;
  if (!good || !(in->lv = lilv_plugin_instantiate(l->plugin, sr, NULL))) return free(in), NULL;
  for (uint32_t i = 0; i < np + 1; i++) lilv_instance_connect_port(in->lv, (uint32_t)in->cp[i], &in->cv[i]);
  const uint32_t ports = lilv_plugin_get_num_ports(l->plugin);
  in->outs = calloc(ports, sizeof(float));
  if (!in->outs) abort();
  LilvNode *control = lilv_new_uri(l->world, LV2_CORE__ControlPort), *output = lilv_new_uri(l->world, LV2_CORE__OutputPort);
  for (uint32_t i = 0; i < ports; i++) {
    const LilvPort *port = lilv_plugin_get_port_by_index(l->plugin, i);
    in->outs[i] = NAN;
    if (lilv_port_is_a(l->plugin, port, control) && lilv_port_is_a(l->plugin, port, output))
      lilv_instance_connect_port(in->lv, i, &in->outs[i]);
  }
  lilv_node_free(control), lilv_node_free(output);
  lilv_instance_activate(in->lv);
  return in;
}

static bool face_lv2_run(Inst *in, const float *v, bool bypass, const float *key, const float *il, const float *ir,
                         float *ol, float *orr, uint32_t n) {
  const uint32_t np = g_spec->count;
  memcpy(in->cv, v, np * sizeof(float));
  in->cv[np] = bypass ? 0.0f : 1.0f; /* enabled */
  lilv_instance_connect_port(in->lv, (uint32_t)in->ap[0], (void *)il);
  lilv_instance_connect_port(in->lv, (uint32_t)in->ap[1], (void *)ir);
  lilv_instance_connect_port(in->lv, (uint32_t)in->ap[2], ol);
  lilv_instance_connect_port(in->lv, (uint32_t)in->ap[3], orr);
  if (g_spec->key_symbol) lilv_instance_connect_port(in->lv, (uint32_t)in->ap[4], (void *)key);
  lilv_instance_run(in->lv, n);
  return true;
}

/* A meter channel by its output control port's symbol: the value the face wrote after the last block. */
static bool face_lv2_meter(Inst *in, const char *symbol, double *value) {
  const FaceLv2 l = {in->world, in->plugin};
  const int32_t i = face_port_of(&l, symbol, true);
  if (i < 0 || isnan(in->outs[i])) return false;
  *value = (double)in->outs[i];
  return true;
}

static int face_lv2_main(const char *bundle, const char *uri, void (*arms)(const Face *)) {
  FaceLv2 l = {lilv_world_new(), NULL};
  char dir[4096];
  snprintf(dir, sizeof dir, "%s%s", bundle, bundle[strlen(bundle) - 1] == '/' ? "" : "/");
  LilvNode *b = lilv_new_file_uri(l.world, NULL, dir);
  lilv_world_load_bundle(l.world, b);
  LilvNode *u = lilv_new_uri(l.world, uri);
  l.plugin = lilv_plugins_get_by_uri(lilv_world_get_all_plugins(l.world), u);
  if (!l.plugin) return fprintf(stderr, "FAIL no LV2 plugin %s in %s\n", uri, dir), 1;
  Face f = {face_lv2_open, face_lv2_run, face_lv2_meter, face_lv2_close, &l};
  g_face = "lv2";
  arms(&f);
  lilv_node_free(u), lilv_node_free(b);
  lilv_world_free(l.world);
  return g_failed ? 1 : 0;
}

/** The oracle's main: the face its command line names, every arm through it, then the tally. */
static int face_main(int argc, char **argv, const struct face_spec *spec, void (*arms)(const Face *)) {
  if (spec->count > FACE_PARAMS_MAX) return fprintf(stderr, "FAIL %u parameters, more than the driver's %u\n", spec->count, FACE_PARAMS_MAX), 2;
  g_spec = spec;
  omx_denormals_off(); /* the faces run with flush-to-zero; so does any reference */
  int rc;
  if (argc == 3 && strcmp(argv[1], "clap") == 0) rc = face_clap_main(argv[2], arms);
  else if (argc == 4 && strcmp(argv[1], "lv2") == 0) rc = face_lv2_main(argv[2], argv[3], arms);
  else return fprintf(stderr, "usage: %s clap <x.clap> | lv2 <bundle-dir> <uri>\n", spec->name), 2;
  printf("%s %s: %d checks, %d failed\n", rc ? "ORACLE-RED" : "ORACLE-GREEN", argv[1], g_checks, g_failed);
  return rc;
}

#endif /* OMX_FACE_DRIVER_H */
