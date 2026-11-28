// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * clap-crosscheck.c — two built CLAP plugins, the same settings and the same signal, compared bit
 * for bit: what proves a regenerated plugin is the one it replaces (the strip as a chain against
 * the hand-ported strip it was).
 *
 *   clap-crosscheck <a.clap> <b.clap> <map>
 *
 * A line `@cap|<A name>|<fraction>|<why>` keeps A's value of that parameter at or below that fraction
 * of the Nyquist rate at every rate, on both plugins: the stretch above it is a known gap, declared
 * in the map where it says why, not a difference to wave through. The check prints each cap it applies.
 *
 * The map says how a setting of A is the same setting of B, one line each ('#' starts a comment;
 * fields are separated by '|', parameter names are the CLAP names, as a host shows them):
 *
 *   <A name>|<B name>[,<B name>...]          each B parameter takes A's value
 *   <A name>|<B name>|<a>:<b> <a>:<b> ...    A's value a is B's value b (a choice spelled differently)
 *   |<B name>|=<x>                           B's parameter held at x (a control A did not have)
 *
 * Every parameter of A and of B (the host's bypass aside, which both get) is in the map, or the
 * check refuses to run: nothing is compared by accident. The plan, at every rate the contract
 * declares: one block at the defaults, then uneven blocks (1 to 3000 frames) with every parameter
 * of A moved across its travel (a stepped one through its steps) and the bypass toggled, then a
 * full-scale burst and a quiet tail at the defaults and at the last moved settings, so dynamics
 * engage and release. Prints `PASS`/`FAIL` per rate, the first differing frame, and exits 1 on
 * any difference.
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

#define MAXP 256
#define MAXB 8

typedef struct {
  char name[CLAP_NAME_SIZE];
  clap_id id;
  double min, max, def;
  bool stepped;
} Param;

typedef struct {
  void *lib;
  const clap_plugin_entry_t *entry;
  const clap_plugin_factory_t *factory;
  Param p[MAXP];
  uint32_t n;
  clap_id bypass;
} Plugin;

/* One line of the map: the A parameter (or -1: B held), the B parameters, and a value map. */
typedef struct {
  int a;
  int b[MAXB];
  int nb;
  double from[32], to[32];
  int nmap;
  double held;
} Rule;

static Rule rules[MAXP];
static int nrules;

/* `@cap` lines: A's parameter `a` is held at or below `frac` of Nyquist. */
typedef struct {
  int a;
  double frac;
  char why[200];
} Cap;
static Cap caps[MAXP];
static int ncaps;

static const void *h_ext(const clap_host_t *h, const char *id) {
  (void)h, (void)id;
  return NULL;
}
static void h_noop(const clap_host_t *h) { (void)h; }
static const clap_host_t HOST = {CLAP_VERSION_INIT, NULL, "clap-crosscheck", "openmixer", "", "0.1", h_ext, h_noop,
                                 h_noop, h_noop};

static bool load(Plugin *pl, const char *path) {
  memset(pl, 0, sizeof *pl);
  pl->lib = dlopen(path, RTLD_NOW | RTLD_LOCAL);
  if (!pl->lib) return fprintf(stderr, "dlopen %s: %s\n", path, dlerror()), false;
  pl->entry = (const clap_plugin_entry_t *)dlsym(pl->lib, "clap_entry");
  if (!pl->entry || !pl->entry->init(path)) return fprintf(stderr, "clap_entry %s\n", path), false;
  pl->factory = (const clap_plugin_factory_t *)pl->entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  if (!pl->factory || pl->factory->get_plugin_count(pl->factory) != 1) return fprintf(stderr, "factory %s\n", path), false;
  const clap_plugin_t *p = pl->factory->create_plugin(pl->factory, &HOST, pl->factory->get_plugin_descriptor(pl->factory, 0)->id);
  if (!p || !p->init(p)) return false;
  const clap_plugin_params_t *pp = (const clap_plugin_params_t *)p->get_extension(p, CLAP_EXT_PARAMS);
  pl->bypass = CLAP_INVALID_ID;
  for (uint32_t i = 0; pp && i < pp->count(p); i++) {
    clap_param_info_t info;
    if (!pp->get_info(p, i, &info)) continue;
    if (info.flags & CLAP_PARAM_IS_BYPASS) {
      pl->bypass = info.id;
      continue;
    }
    if (pl->n == MAXP) return fprintf(stderr, "too many parameters\n"), false;
    Param *q = &pl->p[pl->n++];
    snprintf(q->name, sizeof q->name, "%s", info.name);
    q->id = info.id, q->min = info.min_value, q->max = info.max_value, q->def = info.default_value;
    q->stepped = (info.flags & CLAP_PARAM_IS_STEPPED) != 0;
  }
  p->destroy(p);
  return pl->bypass != CLAP_INVALID_ID;
}

static int find(const Plugin *pl, const char *name) {
  for (uint32_t i = 0; i < pl->n; i++)
    if (strcmp(pl->p[i].name, name) == 0) return (int)i;
  return -1;
}

static char *trim(char *s) {
  while (*s == ' ' || *s == '\t') s++;
  char *e = s + strlen(s);
  while (e > s && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\n' || e[-1] == '\r')) *--e = 0;
  return s;
}

static bool read_map(const char *path, const Plugin *A, const Plugin *B) {
  FILE *f = fopen(path, "r");
  if (!f) return fprintf(stderr, "cannot read %s\n", path), false;
  char line[1024];
  bool ok = true, seenA[MAXP] = {false}, seenB[MAXP] = {false};
  while (fgets(line, sizeof line, f)) {
    char *s = trim(line);
    if (!*s || *s == '#') continue;
    if (!strncmp(s, "@cap|", 5)) {
      char *name = s + 5, *fr = strchr(name, '|'), *why = fr ? strchr(fr + 1, '|') : NULL;
      if (!why) return fprintf(stderr, "map: @cap wants @cap|<A name>|<fraction>|<why>: %s\n", s), fclose(f), false;
      *fr++ = 0, *why++ = 0;
      if (ncaps == MAXP) return fprintf(stderr, "map: more than %d @cap lines\n", MAXP), fclose(f), false;
      Cap *c = &caps[ncaps++];
      c->a = find(A, trim(name)), c->frac = strtod(fr, NULL);
      snprintf(c->why, sizeof c->why, "%s", trim(why));
      if (c->a < 0 || c->frac <= 0 || c->frac >= 1 || !*c->why) fprintf(stderr, "map: bad @cap line: %s\n", s), ok = false;
      continue;
    }
    char *f1 = s, *f2 = strchr(f1, '|');
    if (!f2) return fprintf(stderr, "map: no '|' in: %s\n", s), fclose(f), false;
    *f2++ = 0;
    char *f3 = strchr(f2, '|');
    if (f3) *f3++ = 0;
    if (nrules == MAXP) return fprintf(stderr, "map: more than %d lines\n", MAXP), fclose(f), false;
    Rule *r = &rules[nrules++];
    memset(r, 0, sizeof *r);
    f1 = trim(f1);
    r->a = *f1 ? find(A, f1) : -1;
    if (*f1 && r->a < 0) fprintf(stderr, "map: A has no parameter '%s'\n", f1), ok = false;
    if (r->a >= 0) seenA[r->a] = true;
    for (char *t = strtok(f2, ","); t; t = strtok(NULL, ",")) {
      const int b = find(B, trim(t));
      if (b < 0) fprintf(stderr, "map: B has no parameter '%s'\n", trim(t)), ok = false;
      else if (r->nb < MAXB) seenB[b] = true, r->b[r->nb++] = b;
    }
    if (f3 && *(f3 = trim(f3)) == '=') {
      r->held = strtod(f3 + 1, NULL);
      if (r->a >= 0) fprintf(stderr, "map: '=%s' holds a B parameter; leave the A field empty\n", f3 + 1), ok = false;
    } else if (f3) {
      for (char *t = strtok(f3, " "); t && r->nmap < 32; t = strtok(NULL, " ")) {
        char *c = strchr(t, ':');
        if (!c) fprintf(stderr, "map: '%s' is not a:b\n", t), ok = false;
        else r->from[r->nmap] = strtod(t, NULL), r->to[r->nmap++] = strtod(c + 1, NULL);
      }
    } else if (r->a < 0) fprintf(stderr, "map: a line with no A parameter holds its B parameters at '=x'\n"), ok = false;
  }
  fclose(f);
  for (uint32_t i = 0; i < A->n; i++)
    if (!seenA[i]) fprintf(stderr, "map: A's '%s' is not mapped\n", A->p[i].name), ok = false;
  for (uint32_t i = 0; i < B->n; i++)
    if (!seenB[i]) fprintf(stderr, "map: B's '%s' is not mapped\n", B->p[i].name), ok = false;
  return ok;
}

/* ---- the plan ------------------------------------------------------------------------------ */

static const uint32_t FRAMES[] = {64, 1, 333, 512, 17, 480, 129, 1024, 7, 2500, 600, 3000};
static const double AT[] = {0.5, 0.25, 1.0, 0.0, 0.75, 0.1, 0.9, 0.6, 0.35};
#define MOVES 36
#define BLOCKS (1 + MOVES + 4)

typedef struct {
  uint32_t frames; /* 0: `ms` long */
  double ms;
  int signal;      /* 0 moving bursts, 1 full-scale burst, 2 quiet tail */
  int bypass;
  double v[MAXP];  /* A's values */
} Block;

static Block plan[BLOCKS];

/* A's value of parameter `i` in block `k` at `sr`, held under its cap. */
static double capped(const Block *k, int i, double sr) {
  double v = k->v[i];
  for (int c = 0; c < ncaps; c++)
    if (caps[c].a == i && v > caps[c].frac * sr / 2.0) v = caps[c].frac * sr / 2.0;
  return v;
}

static void make_plan(const Plugin *A) {
  for (int b = 0; b < BLOCKS; b++) {
    Block *k = &plan[b];
    k->signal = 0, k->bypass = 0, k->ms = 0, k->frames = FRAMES[b % 12];
    for (uint32_t i = 0; i < A->n; i++) {
      const Param *p = &A->p[i];
      if (b == 0 || b == 1 + MOVES || b == 2 + MOVES) k->v[i] = p->def; /* the defaults, then their burst and tail */
      else if (b > 2 + MOVES) k->v[i] = plan[MOVES].v[i];             /* the last moves, burst and tail */
      else if (p->stepped) k->v[i] = p->min + (double)((uint32_t)(b * 7 + i * 3) % (uint32_t)(p->max - p->min + 1));
      else k->v[i] = p->min + AT[(b + 2 * i) % 9] * (p->max - p->min);
    }
    if (b > 0 && b <= MOVES) k->bypass = (b % 9) == 4 || (b % 9) == 5;
    if (b > MOVES) k->frames = 0, k->ms = (b - MOVES) % 2 ? 10.0 : 400.0, k->signal = (b - MOVES) % 2 ? 1 : 2;
  }
}

static uint32_t frames_of(const Block *k, double sr) { return k->ms > 0 ? (uint32_t)ceil(k->ms * sr / 1000.0) : k->frames; }

static void signal_make(float *l, float *r, uint32_t n, double sr) {
  uint32_t seed = 0x6f6d78u, start = 0, end = 0;
  const uint32_t period = (uint32_t)(0.09 * sr);
  int b = 0;
  for (uint32_t i = 0; i < n; i++) {
    while (i >= end && b < BLOCKS) start = end, end += frames_of(&plan[b++], sr);
    const int sig = plan[b - 1].signal;
    seed = seed * 1664525u + 1013904223u;
    const float a = (float)(seed >> 8) / 16777216.0f - 0.5f;
    seed = seed * 1664525u + 1013904223u;
    const float c = (float)(seed >> 8) / 16777216.0f - 0.5f;
    if (sig == 1) l[i] = 2.0f * a, r[i] = 1.8f * c;
    else if (sig == 2) {
      const float fall = 2.0f * expf(-(float)(i - start) / (0.01f * (float)sr)) + 0.002f;
      l[i] = fall * c, r[i] = 0.9f * fall * a;
    } else {
      const float env = expf(-(float)(i % period) / (0.012f * (float)sr));
      l[i] = 0.9f * env * a + 0.02f * c, r[i] = 0.7f * env * c + 0.02f * a;
    }
  }
}

/* B's value of rule r for A's value v. */
static double mapped(const Rule *r, double v, bool *ok) {
  if (r->a < 0) return r->held;
  if (!r->nmap) return v;
  for (int k = 0; k < r->nmap; k++)
    if (r->from[k] == v) return r->to[k];
  *ok = false;
  return v;
}

typedef struct {
  clap_event_param_value_t ev[MAXP + 1];
  uint32_t n;
} Events;
static uint32_t ev_size(const clap_input_events_t *l) { return ((const Events *)l->ctx)->n; }
static const clap_event_header_t *ev_get(const clap_input_events_t *l, uint32_t i) { return &((const Events *)l->ctx)->ev[i].header; }
static bool ev_push(const clap_output_events_t *l, const clap_event_header_t *e) {
  (void)l, (void)e;
  return true;
}
static void push(Events *e, clap_id id, double v) {
  clap_event_param_value_t *x = &e->ev[e->n++];
  memset(x, 0, sizeof *x);
  x->header = (clap_event_header_t){sizeof *x, 0, CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_PARAM_VALUE, 0};
  x->param_id = id, x->note_id = -1, x->port_index = -1, x->channel = -1, x->key = -1, x->value = v;
}

/* The whole plan through one fresh instance of `pl`; `side` 0 is A (the plan's values), 1 is B (mapped). */
static bool run(const Plugin *pl, int side, double sr, const float *il, const float *ir, float *ol, float *orr, uint32_t *lat) {
  const clap_plugin_t *p = pl->factory->create_plugin(pl->factory, &HOST, pl->factory->get_plugin_descriptor(pl->factory, 0)->id);
  if (!p || !p->init(p)) return false;
  const clap_plugin_latency_t *latency = (const clap_plugin_latency_t *)p->get_extension(p, CLAP_EXT_LATENCY);
  uint32_t longest = 1;
  for (int b = 0; b < BLOCKS; b++) longest = frames_of(&plan[b], sr) > longest ? frames_of(&plan[b], sr) : longest;
  bool ok = p->activate(p, sr, 1, longest) && p->start_processing(p);
  clap_output_events_t out = {NULL, ev_push};
  uint32_t at = 0;
  for (int b = 0; ok && b < BLOCKS; b++) {
    const Block *k = &plan[b];
    Events evs = {.n = 0};
    if (side == 0)
      for (uint32_t i = 0; i < pl->n; i++) push(&evs, pl->p[i].id, capped(k, (int)i, sr));
    else
      for (int j = 0; j < nrules; j++)
        for (int q = 0; q < rules[j].nb; q++) {
          bool m = true;
          const double v = mapped(&rules[j], rules[j].a >= 0 ? capped(k, rules[j].a, sr) : 0.0, &m);
          if (!m) return fprintf(stderr, "map: rule %d maps no B value for A's value %g\n", j + 1, k->v[rules[j].a]), false;
          push(&evs, pl->p[rules[j].b[q]].id, v);
        }
    push(&evs, pl->bypass, k->bypass ? 1.0 : 0.0);
    clap_input_events_t in = {&evs, ev_size, ev_get};
    float *ib[2] = {(float *)il + at, (float *)ir + at}, *ob[2] = {ol + at, orr + at};
    clap_audio_buffer_t ai = {ib, NULL, 2, 0, 0}, ao = {ob, NULL, 2, 0, 0};
    const uint32_t frames = frames_of(k, sr);
    clap_process_t pr = {.steady_time = at, .frames_count = frames, .audio_inputs = &ai, .audio_outputs = &ao,
                         .audio_inputs_count = 1, .audio_outputs_count = 1, .in_events = &in, .out_events = &out};
    ok = p->process(p, &pr) != CLAP_PROCESS_ERROR;
    at += frames;
  }
  if (ok && lat) *lat = latency ? latency->get(p) : 0;
  p->stop_processing(p);
  p->deactivate(p);
  p->destroy(p);
  return ok;
}

int main(int argc, char **argv) {
  if (argc != 4) return fprintf(stderr, "usage: clap-crosscheck <a.clap> <b.clap> <map>\n"), 2;
  Plugin A, B;
  if (!load(&A, argv[1]) || !load(&B, argv[2])) return 2;
  if (!read_map(argv[3], &A, &B)) return 2;
  make_plan(&A);
  for (int c = 0; c < ncaps; c++)
    printf("CAP %s <= %.2f of Nyquist: %s\n", A.p[caps[c].a].name, caps[c].frac, caps[c].why);
  int fails = 0;
  for (uint32_t r = 0; r < OMX_DECLARED_RATE_COUNT; r++) {
    const double sr = OMX_DECLARED_RATES[r];
    uint32_t n = 0;
    for (int b = 0; b < BLOCKS; b++) n += frames_of(&plan[b], sr);
    float *buf = calloc((size_t)n * 6u, sizeof(float));
    if (!buf) abort();
    float *il = buf, *ir = buf + n, *al = buf + 2u * n, *ar = buf + 3u * n, *bl = buf + 4u * n, *br = buf + 5u * n;
    signal_make(il, ir, n, sr);
    uint32_t la = 0, lb = 1;
    const bool ran = run(&A, 0, sr, il, ir, al, ar, &la) && run(&B, 1, sr, il, ir, bl, br, &lb);
    bool same = ran && memcmp(al, bl, (size_t)n * sizeof(float)) == 0 && memcmp(ar, br, (size_t)n * sizeof(float)) == 0;
    for (uint32_t i = 0; ran && !same && i < n; i++)
      if (memcmp(al + i, bl + i, sizeof(float)) || memcmp(ar + i, br + i, sizeof(float))) {
        uint32_t at = 0;
        int b = 0;
        while (b < BLOCKS && at + frames_of(&plan[b], sr) <= i) at += frames_of(&plan[b++], sr);
        fprintf(stderr, "  first difference at frame %u (block %d, frame %u of it): A %.9g/%.9g, B %.9g/%.9g\n", i, b, i - at,
                (double)al[i], (double)ar[i], (double)bl[i], (double)br[i]);
        break;
      }
    const bool moved = memcmp(al, il, (size_t)n * sizeof(float)) != 0;
    printf("%s output bit for bit @ %.0f Hz\n", same ? "PASS" : "FAIL", sr);
    printf("%s the plan moves the signal @ %.0f Hz\n", moved ? "PASS" : "FAIL", sr);
    printf("%s latency %u = %u @ %.0f Hz\n", ran && la == lb ? "PASS" : "FAIL", la, lb, sr);
    fails += !same + !moved + !(ran && la == lb);
    free(buf);
  }
  printf("%s: %d failure(s)\n", fails ? "CROSSCHECK-RED" : "CROSSCHECK-GREEN", fails);
  return fails ? 1 : 0;
}
