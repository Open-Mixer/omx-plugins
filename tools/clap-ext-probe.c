// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * clap-ext-probe.c — a CLAP host that reads the extensions a built omx plugin serves beyond its
 * parameters, as a host would: it knows no plugin, only CLAP, omx-clap's <omx-clap-host/omx_clap_ext.h>
 * and omx-contract's <omxcontract/omx_resource_hints.h>.
 *
 *   clap-ext-probe <x.clap>
 *
 * prints, TAB-separated:
 *
 *   param <id> <symbol> <unit> <scale> <step> <min> <max> <default> <flags>
 *       get_info and org.openmixer.param-contract/1's get, for every parameter params lists;
 *   contract <version> <fingerprint>
 *   page <index> <page_id> <section name> <page name> <param ids, comma-separated> <is_for_preset>
 *       clap.remote-controls, every page;
 *   placed|moved|kept <placement> <id> <value>
 *       clap.track-info, for each placement (0 insert, 1 return, 2 bus, 3 master) and parameter:
 *       placed: the host says the placement before init, the plugin is activated, get_value;
 *       moved: the host says it after activation, calls changed(), flushes, get_value;
 *       kept: as moved, after the host set every parameter to its maximum by a flush.
 *   hints <get's answer, 0 or 1> <declared, hex> | hints absent
 *   hint <member> <value>
 *       org.openmixer.resource-hints.draft/0's get, every member of the structure whatever its bit says.
 *
 * tools/clap-ext-check.mjs reads it back against the declaration. Exit 1 when an extension is
 * absent or a call fails.
 */
#include <dlfcn.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clap/entry.h"
#include "clap/events.h"
#include "clap/ext/params.h"
#include "clap/ext/remote-controls.h"
#include "clap/ext/track-info.h"
#include "clap/factory/plugin-factory.h"
#include "clap/host.h"

#include <omx-clap-host/omx_clap_ext.h>
#include <omxcontract/omx_resource_hints.h>

#define MAX_PARAMS 512u

static uint64_t g_flags; /* what the host's track-info answers */

static bool track_info_get(const clap_host_t *h, clap_track_info_t *info) {
  (void)h;
  memset(info, 0, sizeof *info);
  info->flags = g_flags;
  snprintf(info->name, sizeof info->name, "%s", "probe");
  return true;
}
static const clap_host_track_info_t HOST_TRACK_INFO = {track_info_get};

static const void *host_get_extension(const clap_host_t *h, const char *id) {
  (void)h;
  return strcmp(id, CLAP_EXT_TRACK_INFO) == 0 ? &HOST_TRACK_INFO : NULL;
}
static void host_noop(const clap_host_t *h) { (void)h; }
static const clap_host_t HOST = {
    CLAP_VERSION_INIT, NULL, "omx clap-ext-probe", "openmixer", "", "0.1", host_get_extension, host_noop, host_noop, host_noop,
};

/** The flags of a placement: 0 insert, 1 return, 2 bus, 3 master. */
static uint64_t flags_of(uint32_t placement) {
  static const uint64_t F[] = {0, CLAP_TRACK_INFO_IS_FOR_RETURN_TRACK, CLAP_TRACK_INFO_IS_FOR_BUS, CLAP_TRACK_INFO_IS_FOR_MASTER};
  return F[placement];
}

typedef struct {
  clap_event_param_value_t ev[MAX_PARAMS];
  uint32_t n;
} EvList;
static uint32_t ev_size(const clap_input_events_t *l) { return ((const EvList *)l->ctx)->n; }
static const clap_event_header_t *ev_get(const clap_input_events_t *l, uint32_t i) { return &((const EvList *)l->ctx)->ev[i].header; }
static bool ev_push(const clap_output_events_t *l, const clap_event_header_t *e) {
  (void)l, (void)e;
  return true;
}

static const clap_plugin_factory_t *g_factory;
static const char *g_id;

static const clap_plugin_t *create(void) {
  const clap_plugin_t *p = g_factory->create_plugin(g_factory, &HOST, g_id);
  if (!p || !p->init(p)) return fprintf(stderr, "create/init\n"), NULL;
  return p;
}

static void flush(const clap_plugin_t *p, const clap_plugin_params_t *params, EvList *ev) {
  const clap_input_events_t in = {ev, ev_size, ev_get};
  const clap_output_events_t out = {NULL, ev_push};
  params->flush(p, &in, &out);
}

/** One placement path; `max` sets every parameter to its maximum first. */
static int placement_run(const char *how, uint32_t placement, int after, int max) {
  g_flags = after ? 0 : flags_of(placement);
  const clap_plugin_t *p = create();
  if (!p) return 1;
  const clap_plugin_params_t *params = (const clap_plugin_params_t *)p->get_extension(p, CLAP_EXT_PARAMS);
  const clap_plugin_track_info_t *ti = (const clap_plugin_track_info_t *)p->get_extension(p, CLAP_EXT_TRACK_INFO);
  if (!params || !ti) return fprintf(stderr, "no params or no clap.track-info extension\n"), p->destroy(p), 1;
  const uint32_t n = params->count(p);
  if (!p->activate(p, 48000.0, 1, 4096)) return fprintf(stderr, "activate\n"), p->destroy(p), 1;
  static EvList ev;
  memset(&ev, 0, sizeof ev);
  if (max) {
    for (uint32_t i = 0; i < n && i < MAX_PARAMS; ++i) {
      clap_param_info_t info;
      if (!params->get_info(p, i, &info)) continue;
      clap_event_param_value_t *e = &ev.ev[ev.n++];
      e->header = (clap_event_header_t){sizeof *e, 0, CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_PARAM_VALUE, 0};
      e->param_id = info.id;
      e->note_id = -1, e->port_index = -1, e->channel = -1, e->key = -1;
      e->value = info.max_value;
    }
    flush(p, params, &ev);
    ev.n = 0;
  }
  if (after) {
    g_flags = flags_of(placement);
    ti->changed(p);
    flush(p, params, &ev);
  }
  for (uint32_t i = 0; i < n; ++i) {
    clap_param_info_t info;
    double v;
    if (!params->get_info(p, i, &info) || !params->get_value(p, info.id, &v)) return fprintf(stderr, "get_value %u\n", i), 1;
    printf("%s\t%" PRIu32 "\t%" PRIu32 "\t%.17g\n", how, placement, info.id, v);
  }
  p->deactivate(p);
  p->destroy(p);
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 2) return fprintf(stderr, "usage: clap-ext-probe <x.clap>\n"), 2;
  void *so = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
  if (!so) return fprintf(stderr, "dlopen %s: %s\n", argv[1], dlerror()), 1;
  const clap_plugin_entry_t *entry = (const clap_plugin_entry_t *)dlsym(so, "clap_entry");
  if (!entry || !entry->init(argv[1])) return fprintf(stderr, "no clap_entry\n"), 1;
  g_factory = (const clap_plugin_factory_t *)entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  if (!g_factory || g_factory->get_plugin_count(g_factory) != 1) return fprintf(stderr, "factory\n"), 1;
  g_id = g_factory->get_plugin_descriptor(g_factory, 0)->id;
  int rc = 0;

  const clap_plugin_t *p = create();
  if (!p) return 1;
  const clap_plugin_params_t *params = (const clap_plugin_params_t *)p->get_extension(p, CLAP_EXT_PARAMS);
  const omx_clap_plugin_param_contract_ext_t *pc = (const omx_clap_plugin_param_contract_ext_t *)p->get_extension(p, OMX_CLAP_EXT_PARAM_CONTRACT);
  const clap_plugin_remote_controls_t *rc_ext = (const clap_plugin_remote_controls_t *)p->get_extension(p, CLAP_EXT_REMOTE_CONTROLS);
  if (!params || !pc || !rc_ext) return fprintf(stderr, "missing: %s%s%s\n", params ? "" : "params ", pc ? "" : "org.openmixer.param-contract/1 ", rc_ext ? "" : CLAP_EXT_REMOTE_CONTROLS), 1;
  for (uint32_t i = 0; i < params->count(p); ++i) {
    clap_param_info_t info;
    omx_clap_param_contract_entry_t c;
    if (!params->get_info(p, i, &info) || !pc->get(p, info.id, &c)) {
      fprintf(stderr, "param %u has no contract entry\n", i);
      rc = 1;
      continue;
    }
    printf("param\t%" PRIu32 "\t%s\t%" PRIu32 "\t%" PRIu32 "\t%.17g\t%.17g\t%.17g\t%.17g\t%" PRIx32 "\n", info.id, c.symbol, c.unit, c.scale, c.step,
           info.min_value, info.max_value, info.default_value, (uint32_t)info.flags);
  }
  omx_clap_param_contract_entry_t c;
  if (pc->get(p, params->count(p), &c)) fprintf(stderr, "the contract answers an id params does not list\n"), rc = 1;
  const char *fp = pc->fingerprint(p);
  printf("contract\t%" PRIu32 "\t%s\n", pc->version(p), fp ? fp : "-");
  for (uint32_t k = 0; k < rc_ext->count(p); ++k) {
    clap_remote_controls_page_t page;
    if (!rc_ext->get(p, k, &page)) {
      fprintf(stderr, "page %u is not served\n", k);
      rc = 1;
      continue;
    }
    printf("page\t%" PRIu32 "\t%" PRIu64 "\t%s\t%s\t", k, (uint64_t)page.page_id, page.section_name, page.page_name);
    for (uint32_t s = 0; s < CLAP_REMOTE_CONTROLS_COUNT; ++s) printf("%s%" PRIu32, s ? "," : "", page.param_ids[s]);
    printf("\t%d\n", page.is_for_preset ? 1 : 0);
  }
  clap_remote_controls_page_t page;
  if (rc_ext->get(p, rc_ext->count(p), &page)) fprintf(stderr, "a page past the count is served\n"), rc = 1;
  const omx_clap_plugin_resource_hints_t *rh =
      (const omx_clap_plugin_resource_hints_t *)p->get_extension(p, OMX_CLAP_EXT_RESOURCE_HINTS_DRAFT);
  if (!rh) printf("hints\tabsent\n");
  else {
    omx_resource_hints_t h;
    memset(&h, 0, sizeof h);
    const bool got = rh->get(p, &h);
    printf("hints\t%d\t%" PRIx32 "\n", got ? 1 : 0, h.declared);
    printf("hint\tfootprint_at_rest\t%" PRIu64 "\n", h.footprint_at_rest);
    printf("hint\tfootprint_per_voice\t%" PRIu64 "\n", h.footprint_per_voice);
    printf("hint\tshared_load\t%" PRIu64 "\n", h.shared_load);
    printf("hint\tthreads_spawned\t%" PRIu32 "\n", h.threads_spawned);
    printf("hint\tstreams_from_disk\t%d\n", h.streams_from_disk ? 1 : 0);
    printf("hint\topens_sockets\t%d\n", h.opens_sockets ? 1 : 0);
    printf("hint\tactivate_time\t%" PRIu32 "\n", h.activate_time);
  }
  p->destroy(p);

  for (uint32_t placement = 0; placement < 4; ++placement) {
    rc |= placement_run("placed", placement, 0, 0);
    rc |= placement_run("moved", placement, 1, 0);
    rc |= placement_run("kept", placement, 1, 1);
  }
  entry->deinit();
  dlclose(so);
  return rc;
}
