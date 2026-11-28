// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * pool-voice-oracle.c — omx test-pool-voice against its closed form and against itself, through the
 * BUILT .clap, at the nine rates an audio interface clocks at (OMX_ORACLE_FULL_RATES, 32 to 192 kHz).
 *
 *   pool-voice-oracle <omx-test-pool-voice.clap>
 *
 * Three hosts play the same notes: one that offers no clap.thread-pool, one whose request_exec
 * refuses, and one whose request_exec runs the tasks on four pthreads. Arms, at every rate, in blocks
 * of BLOCK frames, events at a block's start:
 *
 *   note      three notes (69, 76, 60): every sample of both legs is the closed form
 *             y(k) = GAIN * v * env(k) * sin(2 pi f k / rate), on each of the three hosts
 *   release   a note off at a block start: the linear release, silence from R frames after it on, and
 *             CLAP_EVENT_NOTE_END at that frame
 *   identity  VOICES notes, some released mid-run: the three hosts' outputs and note ends are the same
 *             bits (memcmp), not "close"
 *   dispatch  on the pool host every block with two or more sounding voices makes one request_exec for
 *             exactly that many tasks, each task index is run exactly once, and the tasks ran on more
 *             than one thread; the refusing host is asked and the plugin renders the voices itself
 *   load      one block of VOICES voices costs at least LOAD_FLOOR_NS of CPU per voice on the calling
 *             thread when no pool is offered
 *
 * `PASS ...` / `FAIL ...` per check; the exit status is the number of failures (capped).
 */
#include <dlfcn.h>
#include <math.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <clap/clap.h>
#include <omxdsp/omxdsp_definitions.h> /* the oracle rates, omx-dsp's ORACLE_FULL_RATES */

#include "omx_test_pool_voice_decl.h"

#define BLOCK 256u
#define MAXEV 64u
#define VOICES ((uint32_t)OMX_TEST_POOL_VOICE_VOICES)
#define THREADS 4
#define LOAD_FLOOR_NS 10000.0 /* per voice per block: 400 dependent steps of 256 frames cannot run faster */

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

/* ---- the closed form ---------------------------------------------------------------------- */

typedef struct {
  int key;
  double vel;
  int64_t t0, t1; /* note on and note off frames (block starts), t1 -1 when held */
} Note;

static long double frames_of(double ms, double rate) {
  long double f = roundl((long double)ms * rate / 1000.0L);
  return f < 1 ? 1 : f;
}

static long double closed(const Note *n, int64_t frame, double rate) {
  const int64_t k = frame - n->t0;
  if (k < 0) return 0.0L;
  const long double A = frames_of(OMX_TEST_POOL_VOICE_ATTACK_MS, rate), R = frames_of(OMX_TEST_POOL_VOICE_RELEASE_MS, rate);
  long double env;
  if (n->t1 < 0 || frame < n->t1) env = k >= A ? 1.0L : k / A;
  else {
    const int64_t k1 = n->t1 - n->t0, j = frame - n->t1;
    const long double e0 = k1 >= A ? 1.0L : k1 / A;
    env = j >= R ? 0.0L : e0 * (1.0L - j / R);
  }
  const long double f = OMX_TEST_POOL_VOICE_A4_HZ * powl(2.0L, (n->key - 69) / 12.0L);
  return OMX_TEST_POOL_VOICE_GAIN * n->vel * env * sinl(2.0L * 3.141592653589793238462643383279L * f * k / rate);
}

/* ---- three hosts -------------------------------------------------------------------------- */

typedef union {
  clap_event_header_t h;
  clap_event_note_t note;
} Ev;

static Ev in_ev[MAXEV];
static uint32_t in_n;
static int64_t ends[MAXEV];
static int ends_key[MAXEV];
static uint32_t ends_n;
static int64_t block_at;

static uint32_t ev_size(const clap_input_events_t *l) { return (void)l, in_n; }
static const clap_event_header_t *ev_get(const clap_input_events_t *l, uint32_t i) { return (void)l, &in_ev[i].h; }
static bool ev_push(const clap_output_events_t *l, const clap_event_header_t *e) {
  (void)l;
  if (e->space_id == CLAP_CORE_EVENT_SPACE_ID && e->type == CLAP_EVENT_NOTE_END && ends_n < MAXEV) {
    ends[ends_n] = block_at + e->time;
    ends_key[ends_n++] = ((const clap_event_note_t *)e)->key;
  }
  return true;
}
static const clap_input_events_t IN = {NULL, ev_size, ev_get};
static const clap_output_events_t OUT = {NULL, ev_push};

static const clap_plugin_t *cur; /* the plugin whose tasks a request_exec runs */
static uint32_t asked, last_tasks, ran[VOICES];
static uint32_t calls_ok;        /* calls whose every task index ran exactly once */
static uint32_t calls_bad;
static pthread_t seen[64];
static uint32_t seen_n;
static pthread_mutex_t seen_mu = PTHREAD_MUTEX_INITIALIZER;

static void note_thread(void) {
  const pthread_t me = pthread_self();
  pthread_mutex_lock(&seen_mu);
  bool have = false;
  for (uint32_t i = 0; i < seen_n; i++) have = have || pthread_equal(seen[i], me);
  if (!have && seen_n < 64) seen[seen_n++] = me;
  pthread_mutex_unlock(&seen_mu);
}

static bool refuse_exec(const clap_host_t *h, uint32_t n) {
  (void)h;
  asked++, last_tasks = n;
  return false;
}

static atomic_uint next_task;
static uint32_t n_tasks;
static const clap_plugin_thread_pool_t *plug_pool;

static void *worker(void *arg) {
  (void)arg;
  for (;;) {
    const uint32_t i = atomic_fetch_add(&next_task, 1u);
    if (i >= n_tasks) return NULL;
    note_thread();
    __atomic_fetch_add(&ran[i], 1u, __ATOMIC_RELAXED);
    plug_pool->exec(cur, i);
  }
}

static bool threads_exec(const clap_host_t *h, uint32_t n) {
  (void)h;
  asked++, last_tasks = n;
  memset(ran, 0, sizeof ran);
  atomic_store(&next_task, 0u);
  n_tasks = n;
  pthread_t t[THREADS];
  for (int i = 0; i < THREADS; i++) pthread_create(&t[i], NULL, worker, NULL);
  for (int i = 0; i < THREADS; i++) pthread_join(t[i], NULL);
  bool each = true;
  for (uint32_t i = 0; i < VOICES; i++) each = each && ran[i] == (i < n ? 1u : 0u);
  each ? calls_ok++ : calls_bad++;
  return true;
}

static const clap_host_thread_pool_t POOL_REFUSE = {refuse_exec};
static const clap_host_thread_pool_t POOL_THREADS = {threads_exec};

static const void *ext_none(const clap_host_t *h, const char *id) { return (void)h, (void)id, NULL; }
static const void *ext_refuse(const clap_host_t *h, const char *id) {
  return (void)h, strcmp(id, CLAP_EXT_THREAD_POOL) == 0 ? &POOL_REFUSE : NULL;
}
static const void *ext_threads(const clap_host_t *h, const char *id) {
  return (void)h, strcmp(id, CLAP_EXT_THREAD_POOL) == 0 ? &POOL_THREADS : NULL;
}
static void host_noop(const clap_host_t *h) { (void)h; }

#define HOST_OF(ext) {CLAP_VERSION_INIT, NULL, "pool-voice-oracle", "openmixer", "", "0.1", ext, host_noop, host_noop, host_noop}
static const clap_host_t HOSTS[3] = {HOST_OF(ext_none), HOST_OF(ext_refuse), HOST_OF(ext_threads)};
static const char *const HOST_NAME[3] = {"no pool", "refusing pool", "thread pool"};

typedef struct {
  int64_t at; /* a block start */
  uint16_t type;
  int key;
  double vel;
} Play;

/* Render `frames` frames of `plug` into l, r with the events of `play`, in blocks of BLOCK. */
static void render(const clap_plugin_t *plug, const Play *play, uint32_t np, float *l, float *r, int64_t frames) {
  ends_n = 0;
  for (block_at = 0; block_at < frames; block_at += BLOCK) {
    const uint32_t n = (uint32_t)(frames - block_at < BLOCK ? frames - block_at : BLOCK);
    in_n = 0;
    for (uint32_t i = 0; i < np; i++) {
      if (play[i].at < block_at || play[i].at >= block_at + n) continue;
      Ev *e = &in_ev[in_n++];
      memset(e, 0, sizeof *e);
      e->note.header = (clap_event_header_t){sizeof e->note, 0, CLAP_CORE_EVENT_SPACE_ID, play[i].type, 0};
      e->note.note_id = -1, e->note.port_index = 0, e->note.channel = 0, e->note.key = (int16_t)play[i].key;
      e->note.velocity = play[i].vel;
    }
    float *outs[2] = {l + block_at, r + block_at};
    clap_audio_buffer_t ob = {outs, NULL, 2, 0, 0};
    const clap_process_t proc = {block_at, n, NULL, NULL, &ob, 0, 1, &IN, &OUT};
    plug->process(plug, &proc);
  }
}

static double distance(const float *l, const float *r, const Note *notes, uint32_t nn, int64_t from, int64_t to, double rate) {
  double worst = 0.0;
  for (int64_t f = from; f < to; f++) {
    long double want = 0.0L;
    for (uint32_t i = 0; i < nn; i++) want += closed(&notes[i], f, rate);
    const double dl = fabs((double)(l[f] - want)), dr = fabs((double)(r[f] - want));
    if (dl > worst) worst = dl;
    if (dr > worst) worst = dr;
  }
  return worst;
}

#define TOL 1e-5 /* the sum of a few floats, below 1, with room */

static double thread_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
  return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

static const clap_plugin_t *make(const clap_plugin_factory_t *fac, int h, double rate) {
  const clap_plugin_t *p = fac->create_plugin(fac, &HOSTS[h], OMX_TEST_POOL_VOICE_CLAP_ID);
  if (!p || !p->init(p) || !p->activate(p, rate, 1, BLOCK) || !p->start_processing(p)) return NULL;
  return p;
}

int main(int argc, char **argv) {
  if (argc != 2) return fprintf(stderr, "usage: pool-voice-oracle <omx-test-pool-voice.clap>\n"), 2;
  void *lib = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
  if (!lib) return fprintf(stderr, "FAIL dlopen %s: %s\n", argv[1], dlerror()), 1;
  const clap_plugin_entry_t *entry = (const clap_plugin_entry_t *)dlsym(lib, "clap_entry");
  if (!entry || !entry->init(argv[1])) return fprintf(stderr, "FAIL no clap_entry in %s\n", argv[1]), 1;
  const clap_plugin_factory_t *fac = (const clap_plugin_factory_t *)entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  const double vel = 100.0 / 127.0;
  for (uint32_t ri = 0; ri < OMX_ORACLE_FULL_RATES_COUNT; ri++) {
    const double rate = RATES[ri];
    const int64_t len = (int64_t)BLOCK * (int64_t)(rate * 0.2 / BLOCK + 1);
    const int64_t R = (int64_t)frames_of(OMX_TEST_POOL_VOICE_RELEASE_MS, rate);
    float *out[3][2];
    for (int h = 0; h < 3; h++) out[h][0] = calloc((size_t)len, sizeof(float)), out[h][1] = calloc((size_t)len, sizeof(float));
    const clap_plugin_t *plug[3];
    bool made = true;
    for (int h = 0; h < 3; h++) made = made && (plug[h] = make(fac, h, rate)) != NULL;
    if (!made) {
      CHECK(false, "%.0f Hz: the plugin is created and activated on every host", rate);
      continue;
    }
    plug_pool = (const clap_plugin_thread_pool_t *)plug[2]->get_extension(plug[2], CLAP_EXT_THREAD_POOL);
    CHECK(plug_pool && plug_pool->exec, "%.0f Hz: the plugin offers clap.thread-pool", rate);
    if (!plug_pool) continue;
    cur = plug[2];

    /* note: three held notes on each host */
    {
      const Play play[] = {{0, CLAP_EVENT_NOTE_ON, 69, vel}, {0, CLAP_EVENT_NOTE_ON, 76, 0.5}, {0, CLAP_EVENT_NOTE_ON, 60, 1.0}};
      const Note notes[] = {{69, vel, 0, -1}, {76, 0.5, 0, -1}, {60, 1.0, 0, -1}};
      for (int h = 0; h < 3; h++) {
        cur = plug[h];
        render(plug[h], play, 3, out[h][0], out[h][1], len);
        const double d = distance(out[h][0], out[h][1], notes, 3, 0, len, rate);
        CHECK(d <= TOL, "note %.0f Hz, %s: three notes are the sum of three closed forms (worst %.3g)", rate, HOST_NAME[h], d);
        plug[h]->reset(plug[h]);
      }
    }
    /* release: a note off at a block start */
    {
      const int64_t off = (int64_t)BLOCK * (int64_t)(rate * 0.05 / BLOCK + 1);
      const Play play[] = {{0, CLAP_EVENT_NOTE_ON, 69, vel}, {0, CLAP_EVENT_NOTE_ON, 72, vel}, {off, CLAP_EVENT_NOTE_OFF, 69, 0.0}};
      const Note notes[] = {{69, vel, 0, off}, {72, vel, 0, -1}};
      for (int h = 0; h < 3; h++) {
        cur = plug[h];
        render(plug[h], play, 3, out[h][0], out[h][1], len);
        const double d = distance(out[h][0], out[h][1], notes, 2, 0, len, rate);
        CHECK(d <= TOL, "release %.0f Hz, %s: the release is the closed form (worst %.3g)", rate, HOST_NAME[h], d);
        CHECK(ends_n == 1 && ends[0] == off + R && ends_key[0] == 69, "release %.0f Hz, %s: NOTE_END for 69 at frame %lld (%u ends, first at %lld)",
              rate, HOST_NAME[h], (long long)(off + R), ends_n, ends_n ? (long long)ends[0] : -1LL);
        plug[h]->reset(plug[h]);
      }
    }
    /* identity and dispatch: VOICES notes, two released on the way */
    {
      Play play[VOICES + 2];
      for (uint32_t i = 0; i < VOICES; i++) play[i] = (Play){0, CLAP_EVENT_NOTE_ON, 45 + 3 * (int)i, 0.3 + 0.08 * i};
      const int64_t off = (int64_t)BLOCK * (int64_t)(rate * 0.06 / BLOCK + 1);
      play[VOICES] = (Play){off, CLAP_EVENT_NOTE_OFF, 45, 0.0};
      play[VOICES + 1] = (Play){off, CLAP_EVENT_NOTE_OFF, 54, 0.0};
      int64_t e0[MAXEV];
      uint32_t en0 = 0;
      bool same = true, ends_same = true;
      for (int h = 0; h < 3; h++) {
        cur = plug[h];
        asked = 0, calls_ok = calls_bad = seen_n = 0;
        render(plug[h], play, VOICES + 2, out[h][0], out[h][1], len);
        if (h == 0) {
          en0 = ends_n;
          memcpy(e0, ends, sizeof ends);
        } else {
          same = same && memcmp(out[0][0], out[h][0], (size_t)len * sizeof(float)) == 0 && memcmp(out[0][1], out[h][1], (size_t)len * sizeof(float)) == 0;
          ends_same = ends_same && ends_n == en0 && memcmp(e0, ends, en0 * sizeof *ends) == 0;
        }
        const int64_t blocks = len / BLOCK, tail = off + R; /* every block up to the releases has VOICES sounding */
        if (h == 0) CHECK(asked == 0, "dispatch %.0f Hz, %s: no request is made (%u)", rate, HOST_NAME[h], asked);
        if (h == 1) CHECK(asked >= (uint32_t)(tail / BLOCK) && asked <= (uint32_t)blocks && last_tasks >= 2,
                          "dispatch %.0f Hz, %s: the plugin asks (%u requests over %lld blocks)", rate, HOST_NAME[h], asked, (long long)blocks);
        if (h == 2) {
          CHECK(asked >= (uint32_t)(tail / BLOCK) && asked <= (uint32_t)blocks && calls_ok == asked && calls_bad == 0,
                "dispatch %.0f Hz, %s: one request per block with 2 or more voices (%u over %lld blocks), every task index run exactly once (%u bad)",
                rate, HOST_NAME[h], asked, (long long)blocks, calls_bad);
          CHECK(seen_n >= 2, "dispatch %.0f Hz, %s: the tasks ran on %u threads", rate, HOST_NAME[h], seen_n);
        }
        plug[h]->reset(plug[h]);
      }
      CHECK(same, "identity %.0f Hz: %u voices, two released: both legs are the same bits with no pool, a refusing pool and a thread pool", rate, VOICES);
      CHECK(ends_same, "identity %.0f Hz: the note ends are the same on the three hosts", rate);
      CHECK(en0 == 2, "identity %.0f Hz: the two released voices end (%u ends)", rate, en0);
    }
    /* load: the CPU of the calling thread for one block of VOICES voices, no pool */
    {
      Play play[VOICES];
      for (uint32_t i = 0; i < VOICES; i++) play[i] = (Play){0, CLAP_EVENT_NOTE_ON, 45 + 3 * (int)i, 0.5};
      cur = plug[0];
      const double t0 = thread_ns();
      render(plug[0], play, VOICES, out[0][0], out[0][1], BLOCK);
      const double per = (thread_ns() - t0) / VOICES;
      CHECK(per >= LOAD_FLOOR_NS, "load %.0f Hz: a block costs %.0f ns of CPU per voice (at least %.0f)", rate, per, LOAD_FLOOR_NS);
      plug[0]->reset(plug[0]);
    }
    for (int h = 0; h < 3; h++) {
      plug[h]->stop_processing(plug[h]);
      plug[h]->deactivate(plug[h]);
      plug[h]->destroy(plug[h]);
      free(out[h][0]), free(out[h][1]);
    }
  }
  printf("pool-voice-oracle: %d failure(s)\n", failures);
  return failures > 100 ? 100 : failures;
}
