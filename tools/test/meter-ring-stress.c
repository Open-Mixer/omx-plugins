// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * meter-ring-stress.c — include/omx_meter_ring.h under contention: one audio thread publishing, many
 * readers copying, and every value of one publication equal to that publication's number, so a read
 * that mixes two blocks is seen at once (its values differ), and so is a read older than one the same
 * reader already had (its number went down).
 *
 *   meter-ring-stress [seconds per phase] [readers]
 *
 * Two phases: PACED, a publication every 3 µs (an audio thread at a small block), then HAMMER, one
 * publication after another with no pause. The readers outnumber the cores two to one, so the
 * scheduler preempts them in the middle of a copy and the audio thread laps them: the case the
 * ring's re-check exists for. Exit 1 on any torn or backwards read, or when no read was accepted.
 *
 * Then the held accumulator (the plugin holds until read, spec 7.9): DETERMINISTIC, one thread, a burst
 * is held until the next take and the take after it recovers (largest for a level, smallest for a gain
 * reduction, the longest run for the clip word, the read counter counts); and BURST, one writer
 * publishing quiet blocks with a short burst now and then, MANY plain readers (more than the cores) and
 * ONE taker: a burst that began after one take returned and completed before the next began is seen by
 * that next take, and no take mixes two blocks.
 * Build with OMX_METER_RING_H_PATH to test another copy of the header (tools/meter-ring-check.sh's
 * sabotage arms); the first torn read stops the run.
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define OMX_METER_RING_MAX 64u
#ifdef OMX_METER_RING_H_PATH
#include OMX_METER_RING_H_PATH
#else
#include "omx_meter_ring.h"
#endif

#define N OMX_METER_RING_MAX

static struct omx_meter_ring ring;
static atomic_int stop, torn;
static int paced;

typedef struct {
  unsigned long long reads, accepted, refused, backwards, distinct;
  double first_bad[2];
} Tally;

static double now_us(void) {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return (double)t.tv_sec * 1e6 + (double)t.tv_nsec / 1e3;
}

static void *writer(void *arg) {
  unsigned long long *published = arg;
  double v[N], next = now_us();
  for (unsigned long long k = 1; !atomic_load_explicit(&stop, memory_order_relaxed); k++) {
    for (unsigned i = 0; i < N; i++) v[i] = (double)k;
    if (paced) {
      while (now_us() < next) {
      }
      next += 3.0;
    }
    omx_meter_ring_publish(&ring, v, N);
    *published = k;
  }
  return NULL;
}

static void *reader(void *arg) {
  Tally *t = arg;
  double v[N], last = 0.0;
  while (!atomic_load_explicit(&stop, memory_order_relaxed)) {
    t->reads++;
    if (omx_meter_ring_read(&ring, 0, v, N) != N) {
      t->refused++;
      continue;
    }
    t->accepted++;
    for (unsigned i = 1; i < N; i++)
      if (v[i] != v[0]) {
        if (atomic_fetch_add(&torn, 1) == 0) t->first_bad[0] = v[0], t->first_bad[1] = v[i];
        atomic_store(&stop, 1);
        return NULL;
      }
    if (v[0] < last) t->backwards++;
    if (v[0] != last) t->distinct++;
    last = v[0];
  }
  return NULL;
}

static int phase(const char *name, int pace, double seconds, unsigned nreaders) {
  omx_meter_ring_init(&ring);
  atomic_store(&stop, 0);
  atomic_store(&torn, 0);
  paced = pace;
  Tally *t = calloc(nreaders, sizeof *t);
  pthread_t *th = calloc(nreaders, sizeof *th), w;
  unsigned long long published = 0;
  if (!t || !th) abort();
  pthread_create(&w, NULL, writer, &published);
  for (unsigned r = 0; r < nreaders; r++) pthread_create(&th[r], NULL, reader, &t[r]);
  const double end = now_us() + seconds * 1e6;
  while (now_us() < end && !atomic_load(&torn)) usleep(1000);
  atomic_store(&stop, 1);
  pthread_join(w, NULL);
  Tally sum = {0};
  double bad[2] = {0, 0};
  for (unsigned r = 0; r < nreaders; r++) {
    pthread_join(th[r], NULL);
    sum.reads += t[r].reads, sum.accepted += t[r].accepted, sum.refused += t[r].refused;
    sum.backwards += t[r].backwards, sum.distinct += t[r].distinct;
    if (t[r].first_bad[0] != t[r].first_bad[1]) bad[0] = t[r].first_bad[0], bad[1] = t[r].first_bad[1];
  }
  const int n_torn = atomic_load(&torn);
  printf("%s %s: %llu publications, %u readers, %llu reads: %llu whole, %llu refused after %u tries, %d torn, %llu "
         "backwards, %llu changes seen\n",
         n_torn || sum.backwards || !sum.accepted ? "FAIL" : "PASS", name, published, nreaders, sum.reads,
         sum.accepted, sum.refused, OMX_METER_RING_TRIES, n_torn, sum.backwards, sum.distinct);
  if (n_torn) printf("  a read mixed publication %.0f with publication %.0f\n", bad[0], bad[1]);
  free(t);
  free(th);
  return n_torn || sum.backwards || !sum.accepted;
}

/* ---- the held accumulator ---- */

/* value 0 and 2: a level's two legs (burst 1.0), value 1: a gain reduction (burst -1.0, deepest wins), clip
 * word of value 0 and 2: a run of 7 samples. Quiet: 0, 0, 0 and no clip. */
static atomic_ullong bursts_started, bursts_done;
static int burst_every = 40;

static void *burst_writer(void *arg) {
  unsigned long long *published = arg;
  double v[3], c[3], next = now_us();
  for (unsigned long long k = 1; !atomic_load_explicit(&stop, memory_order_relaxed); k++) {
    const int burst = (int)(k % (unsigned long long)burst_every) == 0;
    v[0] = v[2] = burst ? 1.0 : 0.0, v[1] = burst ? -1.0 : 0.0;
    c[0] = c[2] = burst ? 7.0 : 0.0, c[1] = 0.0;
    while (now_us() < next) {
    }
    next += 3.0;
    if (burst) atomic_fetch_add(&bursts_started, 1);
    omx_meter_ring_publish_clip(&ring, v, c, 3);
    if (burst) atomic_fetch_add(&bursts_done, 1);
    *published = k;
  }
  return NULL;
}

typedef struct {
  unsigned long long takes, refused, checked, missed, torn;
} TakeTally;

static void *plain_reader(void *arg) {
  double v[3];
  (void)arg;
  while (!atomic_load_explicit(&stop, memory_order_relaxed)) omx_meter_ring_read(&ring, 0, v, 3);
  return NULL;
}

static void *taker(void *arg) {
  TakeTally *t = arg;
  double v[3], h[3], c[3];
  unsigned long long after_prev = atomic_load(&bursts_started);
  while (!atomic_load_explicit(&stop, memory_order_relaxed)) {
    const unsigned long long done_before = atomic_load(&bursts_done);
    if (omx_meter_ring_take(&ring, 0, v, h, c, 3) != 3) {
      t->refused++;
      continue;
    }
    t->takes++;
    if (h[0] != h[2] || c[0] != c[2] || v[0] != v[2]) t->torn++;
    /* a burst that began after the previous take returned and was done before this one began */
    if (done_before > after_prev) {
      t->checked++;
      if (h[0] < 1.0 || h[2] < 1.0 || h[1] > -1.0 || c[0] < 7.0) t->missed++;
    }
    after_prev = atomic_load(&bursts_started);
    usleep(200 + (unsigned)(t->takes % 7) * 90); /* a host reading at its own tick, not the writer's */
  }
  return NULL;
}

static int burst_phase(double seconds, unsigned nreaders) {
  omx_meter_ring_init(&ring);
  omx_meter_ring_policy(&ring, 1u << 1, ~0u);
  atomic_store(&stop, 0);
  atomic_store(&bursts_started, 0);
  atomic_store(&bursts_done, 0);
  TakeTally t = {0};
  pthread_t w, tk, *th = calloc(nreaders, sizeof *th);
  unsigned long long published = 0;
  if (!th) abort();
  pthread_create(&w, NULL, burst_writer, &published);
  for (unsigned r = 0; r < nreaders; r++) pthread_create(&th[r], NULL, plain_reader, NULL);
  pthread_create(&tk, NULL, taker, &t);
  const double end = now_us() + seconds * 1e6;
  while (now_us() < end) usleep(1000);
  atomic_store(&stop, 1);
  pthread_join(w, NULL);
  pthread_join(tk, NULL);
  for (unsigned r = 0; r < nreaders; r++) pthread_join(th[r], NULL);
  free(th);
  const int bad = t.missed || t.torn || !t.checked || omx_meter_ring_reads(&ring) != t.takes;
  printf("%s burst (%u plain readers beside one taker): %llu publications, %llu takes, %llu with a whole burst "
         "between them, %llu missed it, %llu torn, read counter %u\n",
         bad ? "FAIL" : "PASS", nreaders, published, t.takes, t.checked, t.missed, t.torn,
         omx_meter_ring_reads(&ring));
  return bad;
}

#define EXPECT(cond, what)                                      \
  do {                                                          \
    if (!(cond)) {                                              \
      printf("FAIL held: %s\n", what);                          \
      return 1;                                                 \
    }                                                           \
  } while (0)

static int held_deterministic(void) {
  struct omx_meter_ring r;
  double q[3] = {0, 0, 0}, b[3] = {1, -1, 1}, bc[3] = {7, 0, 7}, v[3], h[3], c[3];
  omx_meter_ring_init(&r);
  omx_meter_ring_policy(&r, 1u << 1, ~0u);
  EXPECT(omx_meter_ring_take(&r, 0, v, h, c, 3) == 0, "a take before any block reads nothing");
  omx_meter_ring_publish(&r, q, 3);
  omx_meter_ring_publish_clip(&r, b, bc, 3); /* the burst */
  omx_meter_ring_publish(&r, q, 3);          /* the block the reader sees is quiet again */
  omx_meter_ring_publish(&r, q, 3);
  EXPECT(omx_meter_ring_take(&r, 0, v, h, c, 3) == 3, "take");
  EXPECT(v[0] == 0 && v[1] == 0, "the value is the last block's");
  EXPECT(h[0] == 1.0 && h[2] == 1.0, "a level holds the burst until it is read");
  EXPECT(h[1] == -1.0, "a gain reduction holds its deepest until it is read");
  EXPECT(c[0] == 7.0 && c[1] == 0.0, "a clip word holds its longest run until it is read");
  omx_meter_ring_publish(&r, q, 3);
  omx_meter_ring_publish(&r, q, 3);
  EXPECT(omx_meter_ring_take(&r, 0, v, h, c, 3) == 3, "second take");
  EXPECT(h[0] == 0 && h[1] == 0 && h[2] == 0 && c[0] == 0, "the take after a quiet stretch recovers");
  /* the lag: a block published between the reader's copy and its store of what it saw is held once more */
  omx_meter_ring_publish_clip(&r, b, bc, 3);
  for (int i = 0; i < 3; i++) atomic_store(&r.taken[i], 0u); /* a reader that has not stored its take yet */
  omx_meter_ring_publish(&r, q, 3);
  EXPECT(omx_meter_ring_take(&r, 0, v, h, c, 3) == 3 && h[0] == 1.0, "an unconfirmed take keeps the burst");
  EXPECT(omx_meter_ring_reads(&r) == 3u, "three takes counted");
  /* a value outside the hold gives the block's own */
  omx_meter_ring_policy(&r, 1u << 1, ~1u);
  omx_meter_ring_publish(&r, b, 3);
  omx_meter_ring_publish(&r, q, 3);
  EXPECT(omx_meter_ring_take(&r, 0, v, h, NULL, 3) == 3 && h[0] == 0.0 && h[2] == 1.0, "an unheld value is the block's own");
  /* a meter taken restarts its own values only: the next meter's burst waits for its own take */
  omx_meter_ring_init(&r);
  omx_meter_ring_policy(&r, 1u << 1, ~0u);
  omx_meter_ring_publish(&r, b, 3);
  EXPECT(omx_meter_ring_take(&r, 0, v, h, NULL, 1) == 1 && h[0] == 1.0, "the first meter is taken");
  omx_meter_ring_publish(&r, q, 3);
  EXPECT(omx_meter_ring_take(&r, 1, v, h, NULL, 1) == 1 && h[0] == -1.0, "the second meter still holds its burst");
  EXPECT(omx_meter_ring_take(&r, 0, v, h, NULL, 1) == 1 && h[0] == 0.0, "the first meter recovered");
  printf("PASS held: burst, deepest, clip run, recovery, unconfirmed take, unheld value, one meter at a time\n");
  return 0;
}

int main(int argc, char **argv) {
  const double seconds = argc > 1 ? atof(argv[1]) : 1.0;
  long cpus = sysconf(_SC_NPROCESSORS_ONLN);
  const unsigned readers = argc > 2 ? (unsigned)atoi(argv[2]) : (unsigned)(cpus > 1 ? 2 * cpus : 2);
  int bad = phase("paced (3 us)", 1, seconds, readers);
  bad |= phase("hammer", 0, seconds, readers);
  bad |= held_deterministic();
  bad |= burst_phase(seconds, readers);
  return bad ? 1 : 0;
}
