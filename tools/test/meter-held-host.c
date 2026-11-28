// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * meter-held-host.c — a 10 ms burst between two host reads reaches the host: omx-clap's library (libomx-clap) opens the
 * built face, runs it in a chain on this thread as its audio thread, and reads its meters with
 * omx_clap_host_meter_values, the call a host's meter tick makes. At every one of omx-dsp's ORACLE_FULL_RATES and every
 * quantum from 32 to 1024 frames: a read, 10 ms of 1.2 on both legs from a block boundary, 20 ms of silence (one 50 Hz
 * tick), a read. That read must show the burst: the level's peak (+1.58 dBFS), the clip word after its channels and a
 * gain reduction; the read after 20 ms more of silence the floor and no clip. A stopped instance, activated again and
 * not yet run, reads nothing. Exit 1 on the first value that is not what the stimulus says; failing lines start FAIL.
 *
 *   meter-held-host <omx-comp.clap>
 *
 * Used by tools/meter-held-check.sh.
 */
#include <math.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

#include <omx-clap-host/clap_chain.h>
#include <omx-clap-host/clap_host.h>
#include <omxdsp/omxdsp_definitions.h> /* OMX_ORACLE_FULL_RATES_INIT */

#define QMAX 1024u
#define BURST 1.2f

static int bad;

static void expect(int ok, const char *where, const char *what, double got) {
  if (!ok) printf("FAIL %s: %s (%.17g)\n", where, what, got), bad = 1;
}

static void run(struct omx_clap_chain *ch, uint32_t q, uint32_t blocks, uint32_t *hot) {
  static float l[QMAX], r[QMAX];
  float *io[2] = {l, r};
  for (uint32_t b = 0; b < blocks; b++) {
    const uint32_t h = *hot < q ? *hot : q;
    *hot -= h;
    for (uint32_t i = 0; i < q; i++) l[i] = r[i] = i < h ? BURST : 0.0f;
    omx_clap_chain_process(ch, (const float *const *)io, io, q);
  }
}

/* Unpublish as a host does: the control thread asks and waits while the audio thread keeps cycling until the stage
 * says STOPPED. Here the control side is a second thread, this one the audio thread. */
struct unpub {
  struct omx_clap_instance *in;
  _Atomic int done, rc;
};

static void *unpublish_thread(void *arg) {
  struct unpub *u = arg;
  atomic_store(&u->rc, omx_clap_host_unpublish(u->in, 100, 2000000));
  atomic_store(&u->done, 1);
  return NULL;
}

static int stop_rt(struct omx_clap_instance *in, struct omx_clap_chain *ch, uint32_t q) {
  struct unpub u = {.in = in};
  pthread_t t;
  uint32_t none = 0;
  if (pthread_create(&t, NULL, unpublish_thread, &u) != 0) return -1;
  while (!atomic_load(&u.done)) run(ch, q, 1, &none);
  pthread_join(t, NULL);
  return atomic_load(&u.rc);
}

/* omx_clap_host_meter_values into `v`, every slot NaN first, so a value the read did not write never passes for one */
static uint32_t host_read(struct omx_clap_instance *in, clap_id id, double *v, uint32_t capacity) {
  for (uint32_t i = 0; i < capacity; i++) v[i] = NAN;
  return omx_clap_host_meter_values(in, id, v, capacity);
}

static uint32_t blocks_of(double rate, uint32_t q, double ms) { return (uint32_t)ceil(rate * ms / 1000.0 / q); }

static int one(const char *path, double rate, uint32_t q) {
  char where[64], why[OMX_CLAP_WHY_MAX] = "";
  snprintf(where, sizeof where, "%.0f Hz, quantum %u", rate, q);
  struct omx_clap_instance *in = NULL;
  if (omx_clap_host_open(path, NULL, &in, why) != 0) return printf("FAIL %s: open: %s\n", where, why), 1;
  clap_id gr = UINT32_MAX, lv = UINT32_MAX;
  for (uint32_t k = 0; k < omx_clap_host_meter_count(in); k++) {
    struct omx_clap_meter m;
    if (omx_clap_host_meter_info(in, k, &m) != 0) continue;
    if (strcmp(m.name, "compGr") == 0) gr = m.id;
    if (strcmp(m.name, "compIn") == 0) lv = m.id;
  }
  if (gr == UINT32_MAX || lv == UINT32_MAX) return printf("FAIL %s: the host lists no compGr or compIn\n", where), 1;
  struct omx_clap_chain_config cfg;
  omx_clap_chain_config_init(&cfg);
  cfg.channels = 2;
  struct omx_clap_chain *ch = NULL;
  if (omx_clap_chain_create(&cfg, &ch) != OMX_CLAP_CHAIN_OK) return printf("FAIL %s: chain\n", where), 1;
  double v[3];
  uint32_t hot = 0;
  for (int pass = 0; pass < 2; pass++) {
    if (omx_clap_host_activate(in, rate, q, why) != 0) return printf("FAIL %s: activate: %s\n", where, why), 1;
    omx_clap_host_publish(in, pthread_self());
    struct omx_clap_chain_member mem;
    omx_clap_chain_member_init(&mem);
    mem.instance = in;
    uint32_t id = 0;
    if (omx_clap_chain_add(ch, &mem, OMX_CLAP_CHAIN_END, &id) != OMX_CLAP_CHAIN_OK) return printf("FAIL %s: chain add\n", where), 1;
    expect(host_read(in, lv, v, 3) == 0, where, pass ? "a stopped instance, activated again, reads nothing before it runs" :
           "nothing is read before the first block", v[0]);
    if (pass == 0) {
      hot = (uint32_t)lround(rate * 0.010);
      run(ch, q, blocks_of(rate, q, 10.0), &hot); /* a burst the stop must take with it */
      if (stop_rt(in, ch, q) != 0) return printf("FAIL %s: unpublish\n", where), 1;
      omx_clap_chain_remove(ch, id);
      omx_clap_host_stop(in);
      omx_clap_host_deactivate(in);
    }
  }
  run(ch, q, blocks_of(rate, q, 20.0), &hot);
  expect(host_read(in, lv, v, 3) == 3 && v[2] == 0.0, where, "the read before the burst: no clip, nothing held over the restart", v[2]);
  host_read(in, gr, v, 1);
  hot = (uint32_t)lround(rate * 0.010);
  run(ch, q, blocks_of(rate, q, 10.0) + blocks_of(rate, q, 20.0), &hot);
  uint32_t n = host_read(in, lv, v, 3);
  const double peak = 20.0 * log10((double)BURST);
  expect(n == 3 && fabs(v[0] - peak) < 1e-4 && fabs(v[1] - peak) < 1e-4, where, "the level read after the burst shows its peak", v[0]);
  expect(n == 3 && v[2] == 1.0, where, "the clip word after the channels shows the burst", v[2]);
  n = host_read(in, gr, v, 3);
  const double step = v[0];
  expect(n == 1 && step < -0.1, where, "the gain reduction read after the burst shows a reduction", step);
  run(ch, q, blocks_of(rate, q, 20.0), &hot);
  n = host_read(in, lv, v, 3);
  expect(n == 3 && v[0] < -50.0 && v[2] == 0.0, where, "the next read has recovered: the floor, no clip", v[0]);
  n = host_read(in, gr, v, 1);
  expect(n == 1 && v[0] > step, where, "the next gain reduction read is shallower", v[0]);
  if (stop_rt(in, ch, q) != 0) return printf("FAIL %s: unpublish\n", where), 1;
  omx_clap_chain_destroy(ch);
  omx_clap_host_stop(in);
  omx_clap_host_close(in);
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 2) return fprintf(stderr, "usage: meter-held-host <omx-comp.clap>\n"), 2;
  static const float rates[OMX_ORACLE_FULL_RATES_COUNT] = OMX_ORACLE_FULL_RATES_INIT;
  static const uint32_t quanta[] = {32u, 64u, 128u, 256u, 512u, 1024u};
  for (uint32_t i = 0; i < OMX_ORACLE_FULL_RATES_COUNT; i++)
    for (uint32_t k = 0; k < sizeof quanta / sizeof quanta[0]; k++) bad |= one(argv[1], (double)rates[i], quanta[k]);
  printf("%s meter-held-host: a 10 ms burst between two reads of omx_clap_host_meter_values reaches the later one, at %u rates x "
         "%zu quanta\n", bad ? "FAIL" : "ok  ", (unsigned)OMX_ORACLE_FULL_RATES_COUNT, sizeof quanta / sizeof quanta[0]);
  return bad;
}
