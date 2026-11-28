// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * clap-session-crosscheck.c — what a plugin saved before it gained parameters, loaded into the plugin
 * that has them: the same values and the same sound. What proves "old presets and sessions load
 * unchanged" when parameters are appended (omx-strip's band slots and its band count).
 *
 *   clap-session-crosscheck <old.clap> <new.clap> <preset.json>...
 *
 * For each preset file (one the OLD plugin shipped), at every rate the contract declares:
 *   - the old plugin loads it through clap.preset-load and saves its clap.state: that text is a
 *     session the old plugin wrote;
 *   - a fresh new plugin loads the same file through clap.preset-load, and another fresh one loads
 *     the old session through clap.state;
 *   - each of the two holds every parameter the old one has (by name) at the old one's value, and
 *     every parameter the old one has not at its declared default: nothing the old plugin never
 *     saved is decided by the load;
 *   - the three run the same signal (no parameter event, so the loaded values alone decide), and
 *     the two new outputs are the old one's, bit for bit.
 * Prints PASS/FAIL per check and exits 1 on any FAIL, 2 when a plugin or a file cannot be read.
 */
#include <dlfcn.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clap/clap.h>
#include <omxcontract/omx_contract.h>

#define MAXP 512
#define BLOCK 256u
#define BLOCKS 160u

static void h_loaded(const clap_host_t *h, uint32_t kind, const char *location, const char *key) {
  (void)h, (void)kind, (void)location, (void)key;
}
static void h_error(const clap_host_t *h, uint32_t kind, const char *location, const char *key, int32_t status, const char *msg) {
  (void)h, (void)kind, (void)key, (void)status;
  fprintf(stderr, "preset-load error %s: %s\n", location, msg ? msg : "");
}
static const clap_host_preset_load_t HOST_PRESET_LOAD = {h_error, h_loaded};
static void h_rescan(const clap_host_t *h, clap_param_rescan_flags flags) { (void)h, (void)flags; }
static void h_clear(const clap_host_t *h, clap_id id, clap_param_clear_flags flags) { (void)h, (void)id, (void)flags; }
static void h_flush(const clap_host_t *h) { (void)h; }
static const clap_host_params_t HOST_PARAMS = {h_rescan, h_clear, h_flush};
static const void *h_ext(const clap_host_t *h, const char *id) {
  (void)h;
  if (strcmp(id, CLAP_EXT_PARAMS) == 0) return &HOST_PARAMS;
  return strcmp(id, CLAP_EXT_PRESET_LOAD) == 0 ? &HOST_PRESET_LOAD : NULL;
}
static void h_noop(const clap_host_t *h) { (void)h; }
static const clap_host_t HOST = {CLAP_VERSION_INIT, NULL, "clap-session-crosscheck", "openmixer", "", "1", h_ext, h_noop, h_noop, h_noop};

typedef struct {
  void *lib;
  const clap_plugin_factory_t *factory;
} Lib;

static bool open_lib(Lib *l, const char *path) {
  l->lib = dlopen(path, RTLD_NOW | RTLD_LOCAL);
  if (!l->lib) return fprintf(stderr, "dlopen %s: %s\n", path, dlerror()), false;
  const clap_plugin_entry_t *e = (const clap_plugin_entry_t *)dlsym(l->lib, "clap_entry");
  if (!e || !e->init(path)) return fprintf(stderr, "clap_entry %s\n", path), false;
  l->factory = (const clap_plugin_factory_t *)e->get_factory(CLAP_PLUGIN_FACTORY_ID);
  return l->factory && l->factory->get_plugin_count(l->factory) == 1;
}

/* One instance, its parameters by name with their values, after a load. */
typedef struct {
  const clap_plugin_t *p;
  uint32_t n;
  char name[MAXP][CLAP_NAME_SIZE];
  double value[MAXP], def[MAXP];
} Inst;

static bool create(Inst *x, const Lib *l) {
  memset(x, 0, sizeof *x);
  x->p = l->factory->create_plugin(l->factory, &HOST, l->factory->get_plugin_descriptor(l->factory, 0)->id);
  return x->p && x->p->init(x->p);
}

static bool read_values(Inst *x) {
  const clap_plugin_params_t *pp = (const clap_plugin_params_t *)x->p->get_extension(x->p, CLAP_EXT_PARAMS);
  if (!pp || pp->count(x->p) > MAXP) return false;
  x->n = pp->count(x->p);
  for (uint32_t i = 0; i < x->n; i++) {
    clap_param_info_t info;
    if (!pp->get_info(x->p, i, &info) || !pp->get_value(x->p, info.id, &x->value[i])) return false;
    snprintf(x->name[i], sizeof x->name[i], "%s", (info.flags & CLAP_PARAM_IS_BYPASS) ? "(bypass)" : info.name);
    x->def[i] = info.default_value;
  }
  return true;
}

static bool load_preset(Inst *x, const char *file) {
  const clap_plugin_preset_load_t *pl = (const clap_plugin_preset_load_t *)x->p->get_extension(x->p, CLAP_EXT_PRESET_LOAD);
  return pl && pl->from_location(x->p, CLAP_PRESET_DISCOVERY_LOCATION_FILE, file, NULL) && read_values(x);
}

/* clap.state through a memory buffer. */
typedef struct {
  char buf[1 << 16];
  size_t len, at;
} Mem;
static int64_t m_write(const clap_ostream_t *s, const void *b, uint64_t n) {
  Mem *m = (Mem *)s->ctx;
  if (m->len + n > sizeof m->buf) return -1;
  memcpy(m->buf + m->len, b, n);
  m->len += n;
  return (int64_t)n;
}
static int64_t m_read(const clap_istream_t *s, void *b, uint64_t n) {
  Mem *m = (Mem *)s->ctx;
  const size_t k = m->len - m->at < n ? m->len - m->at : (size_t)n;
  memcpy(b, m->buf + m->at, k);
  m->at += k;
  return (int64_t)k;
}

static bool save_state(Inst *x, Mem *m) {
  const clap_plugin_state_t *st = (const clap_plugin_state_t *)x->p->get_extension(x->p, CLAP_EXT_STATE);
  m->len = m->at = 0;
  clap_ostream_t os = {m, m_write};
  return st && st->save(x->p, &os);
}

static bool load_state(Inst *x, Mem *m) {
  const clap_plugin_state_t *st = (const clap_plugin_state_t *)x->p->get_extension(x->p, CLAP_EXT_STATE);
  m->at = 0;
  clap_istream_t is = {m, m_read};
  return st && st->load(x->p, &is) && read_values(x);
}

/* Every parameter of `old` at old's value in `x`, every other parameter of `x` at its default. */
static int same_values(const Inst *old, const Inst *x, const char *what) {
  int bad = 0;
  for (uint32_t i = 0; i < x->n; i++) {
    int k = -1;
    for (uint32_t j = 0; j < old->n; j++)
      if (strcmp(old->name[j], x->name[i]) == 0) k = (int)j;
    const double want = k >= 0 ? old->value[k] : x->def[i];
    if (x->value[i] != want) {
      if (bad++ < 5)
        fprintf(stderr, "  %s: '%s' is %.9g, %s %.9g\n", what, x->name[i], x->value[i], k >= 0 ? "the old plugin's" : "its default", want);
    }
  }
  for (uint32_t j = 0; j < old->n; j++) {
    bool found = false;
    for (uint32_t i = 0; i < x->n; i++) found = found || strcmp(old->name[j], x->name[i]) == 0;
    if (!found && bad++ < 5) fprintf(stderr, "  %s: the old parameter '%s' is gone\n", what, old->name[j]);
  }
  return bad;
}

static void signal_make(float *l, float *r, uint32_t n, double sr) {
  uint32_t seed = 0x736573u;
  const uint32_t period = (uint32_t)(0.11 * sr);
  for (uint32_t i = 0; i < n; i++) {
    seed = seed * 1664525u + 1013904223u;
    const float a = (float)(seed >> 8) / 16777216.0f - 0.5f;
    seed = seed * 1664525u + 1013904223u;
    const float c = (float)(seed >> 8) / 16777216.0f - 0.5f;
    const float env = expf(-(float)(i % period) / (0.015f * (float)sr));
    l[i] = 1.6f * env * a + 0.01f * c, r[i] = 1.2f * env * c + 0.01f * a;
  }
}

static uint32_t ev_size(const clap_input_events_t *l) { (void)l; return 0; }
static const clap_event_header_t *ev_get(const clap_input_events_t *l, uint32_t i) { (void)l, (void)i; return NULL; }
static bool ev_push(const clap_output_events_t *l, const clap_event_header_t *e) { (void)l, (void)e; return true; }

static bool run(Inst *x, double sr, const float *il, const float *ir, float *ol, float *orr, uint32_t n) {
  if (!x->p->activate(x->p, sr, 1, BLOCK) || !x->p->start_processing(x->p)) return false;
  clap_input_events_t in = {NULL, ev_size, ev_get};
  clap_output_events_t out = {NULL, ev_push};
  bool ok = true;
  for (uint32_t at = 0; ok && at < n; at += BLOCK) {
    float *ib[2] = {(float *)il + at, (float *)ir + at}, *ob[2] = {ol + at, orr + at};
    clap_audio_buffer_t ai = {ib, NULL, 2, 0, 0}, ao = {ob, NULL, 2, 0, 0};
    clap_process_t pr = {.steady_time = at, .frames_count = BLOCK, .audio_inputs = &ai, .audio_outputs = &ao,
                         .audio_inputs_count = 1, .audio_outputs_count = 1, .in_events = &in, .out_events = &out};
    ok = x->p->process(x->p, &pr) != CLAP_PROCESS_ERROR;
  }
  x->p->stop_processing(x->p);
  x->p->deactivate(x->p);
  return ok;
}

int main(int argc, char **argv) {
  if (argc < 4) return fprintf(stderr, "usage: clap-session-crosscheck <old.clap> <new.clap> <preset.json>...\n"), 2;
  Lib A, B;
  if (!open_lib(&A, argv[1]) || !open_lib(&B, argv[2])) return 2;
  static Inst a, bp, bs;
  static Mem session;
  const uint32_t n = BLOCK * BLOCKS;
  float *buf = calloc((size_t)n * 8u, sizeof(float));
  if (!buf) abort();
  int fails = 0;
  for (int f = 3; f < argc; f++) {
    const char *file = argv[f];
    for (uint32_t r = 0; r < OMX_DECLARED_RATE_COUNT; r++) {
      const double sr = OMX_DECLARED_RATES[r];
      if (!create(&a, &A) || !load_preset(&a, file) || !save_state(&a, &session))
        return fprintf(stderr, "%s: the old plugin does not load it\n", file), 2;
      if (!create(&bp, &B) || !load_preset(&bp, file)) return fprintf(stderr, "%s: the new plugin does not load it\n", file), 2;
      if (!create(&bs, &B) || !load_state(&bs, &session)) return fprintf(stderr, "%s: the new plugin does not load the old session\n", file), 2;
      const int vp = same_values(&a, &bp, "preset"), vs = same_values(&a, &bs, "session");
      float *il = buf, *ir = buf + n, *al = buf + 2u * n, *ar = buf + 3u * n, *pl = buf + 4u * n, *prr = buf + 5u * n,
            *sl = buf + 6u * n, *srr = buf + 7u * n;
      signal_make(il, ir, n, sr);
      const bool ran = run(&a, sr, il, ir, al, ar, n) && run(&bp, sr, il, ir, pl, prr, n) && run(&bs, sr, il, ir, sl, srr, n);
      const size_t bytes = (size_t)n * sizeof(float);
      const bool sp = ran && !memcmp(al, pl, bytes) && !memcmp(ar, prr, bytes);
      const bool ss = ran && !memcmp(al, sl, bytes) && !memcmp(ar, srr, bytes);
      const bool moved = ran && memcmp(al, il, bytes) != 0;
      if (r == 1 || vp || vs || !sp || !ss || !moved) {
        printf("%s %s: the preset's values in the new plugin, the rest at their defaults @ %.0f Hz\n", vp ? "FAIL" : "PASS", file, sr);
        printf("%s %s: the old session's values in the new plugin, the rest at their defaults @ %.0f Hz\n", vs ? "FAIL" : "PASS", file, sr);
        printf("%s %s: loaded as a preset, the old plugin's output bit for bit @ %.0f Hz\n", sp ? "PASS" : "FAIL", file, sr);
        printf("%s %s: loaded as a session, the old plugin's output bit for bit @ %.0f Hz\n", ss ? "PASS" : "FAIL", file, sr);
        printf("%s %s: the preset moves the signal @ %.0f Hz\n", moved ? "PASS" : "FAIL", file, sr);
      }
      fails += (vp != 0) + (vs != 0) + !sp + !ss + !moved;
      a.p->destroy(a.p), bp.p->destroy(bp.p), bs.p->destroy(bs.p);
    }
  }
  free(buf);
  printf("%s: %d failure(s) over %d file(s) at %u rates\n", fails ? "SESSION-CROSSCHECK-RED" : "SESSION-CROSSCHECK-GREEN", fails, argc - 3,
         (unsigned)OMX_DECLARED_RATE_COUNT);
  return fails ? 1 : 0;
}
