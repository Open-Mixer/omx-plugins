// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * static-table-host.c — a CLAP host that links libomxplugins.a through omxplugins.pc and opens
 * every plugin in-process, the way the console's engine does: no .clap path, no dlopen.
 *
 *   static-table-host <clap-id> ... --test <test-clap-id> ...
 *       the ids are the plugins' own (each plugins/<stem>/Makefile's CLAP_ID, from its
 *       declaration). The table must list exactly those, each once, in any order;
 *       omx_clap_static_find() returns each one's row entry and refuses an unknown id and NULL;
 *       each entry's factory holds one plugin whose descriptor id is the row's, created, activated
 *       at 48 kHz, run over a block on every audio port it declares, and destroyed; and the one
 *       entry over all of them (omx_clap_static_entry) lists the same descriptors and creates each
 *       by id. The ids after `--test` are the test plugins' (a declaration's `"role": "test"`): the test
 *       table lists exactly those; neither the shipped table nor the one entry's factory lists them;
 *       find() and the one entry's create_plugin() open and run each by id.
 *       `PASS ...` / `FAIL ...` per check; the exit status is the number of failures (capped).
 */
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <clap/entry.h>
#include <clap/ext/audio-ports.h>
#include <clap/factory/plugin-factory.h>
#include <clap/host.h>
#include <clap/plugin.h>
#include <clap/process.h>

#include <omx-plugins/omx_clap_static.h>

#define RATE 48000.0
#define BLOCK 256u
#define MAX_PORTS 4u
#define MAX_CH 8u

static int failures;

#define CHECK(cond, ...)                                                                         \
  do {                                                                                           \
    bool ok_ = (cond);                                                                           \
    printf("%s ", ok_ ? "PASS" : "FAIL");                                                        \
    printf(__VA_ARGS__);                                                                         \
    printf("\n");                                                                                \
    if (!ok_) failures++;                                                                        \
  } while (0)

static const void *host_get_extension(const clap_host_t *h, const char *id) {
  (void)h, (void)id;
  return NULL;
}
static void host_noop(const clap_host_t *h) { (void)h; }

static const clap_host_t HOST = {
    CLAP_VERSION_INIT, NULL, "omx static-table-host", "openmixer", "", "0.1",
    host_get_extension, host_noop, host_noop, host_noop,
};

static uint32_t ev_size(const clap_input_events_t *l) {
  (void)l;
  return 0;
}
static const clap_event_header_t *ev_get(const clap_input_events_t *l, uint32_t i) {
  (void)l, (void)i;
  return NULL;
}
static bool ev_push(const clap_output_events_t *l, const clap_event_header_t *e) {
  (void)l, (void)e;
  return true;
}
static const clap_input_events_t IN_EVENTS = {NULL, ev_size, ev_get};
static const clap_output_events_t OUT_EVENTS = {NULL, ev_push};

static float bufs[2][MAX_PORTS][MAX_CH][BLOCK];

/* One port side's buffers: every port the plugin declares, its declared channel count. */
static bool ports_of(const clap_plugin_t *p, const clap_plugin_audio_ports_t *ap, bool in, clap_audio_buffer_t *out,
                     float *ptrs[MAX_PORTS][MAX_CH], uint32_t *n) {
  *n = ap ? ap->count(p, in) : 0;
  if (*n > MAX_PORTS) return false;
  for (uint32_t i = 0; i < *n; i++) {
    clap_audio_port_info_t info;
    if (!ap->get(p, i, in, &info) || info.channel_count > MAX_CH) return false;
    for (uint32_t c = 0; c < info.channel_count; c++) ptrs[i][c] = bufs[in][i][c];
    out[i] = (clap_audio_buffer_t){ptrs[i], NULL, info.channel_count, 0, 0};
  }
  return true;
}

/* Create `id` through `fac`, run one block over every declared port, destroy it. */
static bool run_one(const clap_plugin_factory_t *fac, const char *id) {
  const clap_plugin_t *p = fac->create_plugin(fac, &HOST, id);
  if (!p) return false;
  bool ok = p->init(p) && strcmp(p->desc->id, id) == 0 && p->activate(p, RATE, 1, BLOCK) && p->start_processing(p);
  if (ok) {
    const clap_plugin_audio_ports_t *ap = (const clap_plugin_audio_ports_t *)p->get_extension(p, CLAP_EXT_AUDIO_PORTS);
    clap_audio_buffer_t ins[MAX_PORTS], outs[MAX_PORTS];
    float *in_ptrs[MAX_PORTS][MAX_CH], *out_ptrs[MAX_PORTS][MAX_CH];
    uint32_t nin = 0, nout = 0;
    ok = ap && ports_of(p, ap, true, ins, in_ptrs, &nin) && ports_of(p, ap, false, outs, out_ptrs, &nout) && nout > 0;
    for (uint32_t i = 0; ok && i < nin; i++)
      for (uint32_t c = 0; c < ins[i].channel_count; c++)
        for (uint32_t f = 0; f < BLOCK; f++) bufs[1][i][c][f] = 0.25f * sinf((float)f * 0.05f * (float)(c + 1));
    for (uint32_t i = 0; ok && i < nout; i++)
      for (uint32_t c = 0; c < outs[i].channel_count; c++)
        for (uint32_t f = 0; f < BLOCK; f++) bufs[0][i][c][f] = NAN;
    if (ok) {
      const clap_process_t proc = {0, BLOCK, NULL, ins, outs, nin, nout, &IN_EVENTS, &OUT_EVENTS};
      ok = p->process(p, &proc) != CLAP_PROCESS_ERROR;
    }
    /* every output sample written, and finite */
    for (uint32_t i = 0; ok && i < nout; i++)
      for (uint32_t c = 0; c < outs[i].channel_count; c++)
        for (uint32_t f = 0; f < BLOCK; f++) ok = ok && isfinite(bufs[0][i][c][f]);
    p->stop_processing(p);
    p->deactivate(p);
  }
  p->destroy(p);
  return ok;
}

/* A rate the kernel refuses: activate must say no, so the host never runs the plugin as a silent wire.
 * 0.5 Hz is positive and finite, so only the kernel's own answer (the rate as the kernel takes it is 0) refuses it;
 * 1e10 does not fit the 32-bit rate every core holds. */
static bool refuses_rate(const clap_plugin_factory_t *fac, const char *id, double rate) {
  const clap_plugin_t *p = fac->create_plugin(fac, &HOST, id);
  if (!p) return false;
  bool refused = p->init(p) && !p->activate(p, rate, 1, BLOCK);
  p->destroy(p);
  return refused;
}

int main(int argc, char **argv) {
  /* argv: the shipped plugins' ids, then `--test` and the test plugins' ids */
  int sep = argc;
  for (int a = 1; a < argc; a++)
    if (strcmp(argv[a], "--test") == 0) sep = a;
  const uint32_t want = (uint32_t)(sep - 1);
  char **tests = argv + (sep < argc ? sep + 1 : argc);
  const uint32_t ntest = (uint32_t)(sep < argc ? argc - sep - 1 : 0);
  CHECK(want > 0, "the expected ids are given (%u)", want);
  CHECK(OMX_CLAP_STATIC_COUNT == want, "the table lists %u plugins, the plugin folders declare %u", OMX_CLAP_STATIC_COUNT,
        want);

  for (uint32_t i = 0; i < want; i++) {
    const char *id = argv[i + 1];
    const struct omx_clap_static_plugin *row = NULL;
    uint32_t rows = 0;
    for (uint32_t r = 0; r < OMX_CLAP_STATIC_COUNT; r++)
      if (strcmp(OMX_CLAP_STATIC_PLUGINS[r].id, id) == 0) row = &OMX_CLAP_STATIC_PLUGINS[r], rows++;
    CHECK(rows == 1, "the table lists %s once (%u rows)", id, rows);
    if (!row) continue;
    CHECK(omx_clap_static_find(id) == row->entry, "find(%s) returns the row's entry", id);
    const clap_plugin_entry_t *e = row->entry;
    CHECK(clap_version_is_compatible(e->clap_version) && e->init(""), "%s: the entry is CLAP and inits", id);
    const clap_plugin_factory_t *fac = (const clap_plugin_factory_t *)e->get_factory(CLAP_PLUGIN_FACTORY_ID);
    const clap_plugin_descriptor_t *d = fac && fac->get_plugin_count(fac) == 1 ? fac->get_plugin_descriptor(fac, 0) : NULL;
    CHECK(d && strcmp(d->id, id) == 0, "%s: its factory holds one plugin, descriptor id %s", id, d ? d->id : "(none)");
    CHECK(fac && fac->create_plugin(fac, &HOST, "org.example.not-ours") == NULL, "%s: its factory refuses another id", id);
    CHECK(fac && run_one(fac, id), "%s: created, activated at 48 kHz, one block run on every port, destroyed", id);
    CHECK(fac && refuses_rate(fac, id, 0.5), "%s: activate at 0.5 Hz (a rate the kernel refuses) returns false", id);
    CHECK(fac && refuses_rate(fac, id, 1e10), "%s: activate at 1e10 Hz (past the 32-bit rate) returns false", id);
    e->deinit();
  }
  CHECK(omx_clap_static_find("org.example.not-ours") == NULL, "find refuses an id no plugin has");
  CHECK(omx_clap_static_find(NULL) == NULL, "find refuses NULL");

  /* the one entry over all of them */
  const clap_plugin_entry_t *all = &omx_clap_static_entry;
  CHECK(clap_version_is_compatible(all->clap_version) && all->init(""), "omx_clap_static_entry is CLAP and inits every face");
  const clap_plugin_factory_t *fac = (const clap_plugin_factory_t *)all->get_factory(CLAP_PLUGIN_FACTORY_ID);
  CHECK(fac && fac->get_plugin_count(fac) == OMX_CLAP_STATIC_COUNT, "its factory lists %u plugins",
        fac ? fac->get_plugin_count(fac) : 0);
  CHECK(all->get_factory("org.example.no-factory") == NULL, "it has no other factory");
  for (uint32_t i = 0; fac && i < fac->get_plugin_count(fac); i++) {
    const clap_plugin_descriptor_t *d = fac->get_plugin_descriptor(fac, i);
    const char *id = OMX_CLAP_STATIC_PLUGINS[i].id;
    CHECK(d && strcmp(d->id, id) == 0, "its descriptor %u is %s", i, id);
    CHECK(run_one(fac, id), "its factory creates and runs %s", id);
  }
  CHECK(fac && fac->get_plugin_descriptor(fac, OMX_CLAP_STATIC_COUNT) == NULL, "no descriptor past the last");
  CHECK(fac && fac->create_plugin(fac, &HOST, "org.example.not-ours") == NULL, "its factory refuses an id no plugin has");

  /* the test plugins: linked, found by id, created by the one entry, listed by neither table nor factory */
  CHECK(OMX_CLAP_STATIC_TEST_COUNT == ntest, "the test table lists %u plugins, the test folders declare %u",
        OMX_CLAP_STATIC_TEST_COUNT, ntest);
  for (uint32_t i = 0; i < ntest; i++) {
    const char *id = tests[i];
    uint32_t rows = 0, shipped = 0, listed = 0;
    const struct omx_clap_static_plugin *row = NULL;
    for (uint32_t r = 0; r < OMX_CLAP_STATIC_TEST_COUNT; r++)
      if (strcmp(OMX_CLAP_STATIC_TEST_PLUGINS[r].id, id) == 0) row = &OMX_CLAP_STATIC_TEST_PLUGINS[r], rows++;
    for (uint32_t r = 0; r < OMX_CLAP_STATIC_COUNT; r++) shipped += strcmp(OMX_CLAP_STATIC_PLUGINS[r].id, id) == 0;
    for (uint32_t r = 0; fac && r < fac->get_plugin_count(fac); r++) {
      const clap_plugin_descriptor_t *d = fac->get_plugin_descriptor(fac, r);
      listed += d && strcmp(d->id, id) == 0;
    }
    CHECK(rows == 1 && shipped == 0, "test plugin %s: once in the test table, never in the shipped one", id);
    CHECK(listed == 0, "test plugin %s: the one entry's factory does not list it", id);
    CHECK(row && omx_clap_static_find(id) == row->entry, "test plugin %s: find() returns its entry", id);
    CHECK(fac && run_one(fac, id), "test plugin %s: the one entry's factory creates and runs it by id", id);
  }
  CHECK(OMX_CLAP_STATIC_TEST_PLUGINS[OMX_CLAP_STATIC_TEST_COUNT].id == NULL, "the test table ends in its sentinel");
  all->deinit();

  printf("static-table-host: %u plugins, %u test plugins, %d failures\n", OMX_CLAP_STATIC_COUNT, OMX_CLAP_STATIC_TEST_COUNT,
         failures);
  return failures > 100 ? 100 : failures;
}
