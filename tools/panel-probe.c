// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * panel-probe.c — a host that reads org.openmixer.panel-description/1 from ANY built omx plugin, as
 * a host would: it knows no plugin, only CLAP, LV2 and the extension's structures
 * (include/omx_panel_description.h).
 *
 *   panel-probe clap <x.clap>
 *       the plugin created through clap_entry, the extension through get_extension;
 *   panel-probe lv2 <bundle> <uri>
 *       the bundle loaded and the plugin instantiated through lilv, the extension through
 *       extension_data, and one `port` line per port (index, symbol) for the reader.
 *
 * It prints what the extension serves, one TAB-separated line per entry, in the order it was served:
 *
 *   fingerprint <hex>          family <n>
 *   section <key> <label_key> <label>
 *   control <param_id> <section> <widget> <label_key> <visible_param_id> <mask hex> <label>
 *   role <role> <index> <param_id>
 *   display <kind> <section> <source> <meter0> <meter1> <curve> <floor_param_id> <floor_db> <label_key> <label>
 *   default <placement> <param_id> <value>
 *
 * and, from the CLAP face, the meters the displays index, as clap.meters/1 serves them (the ids a
 * display's <meter0> <meter1> are, omx-clap-core's omx_clap_ext.h):
 *
 *   meter <id> <name> <kind> <channels> <min> <max>
 *
 * The LV2 face serves each meter as an output control port, one of the `port` lines, by the symbol
 * the declaration gives it (docs/meters.md). tools/panel-check.mjs reads it all back against the
 * declaration. Exit 1 when the extension is absent, a table answers past its end, an unknown table is
 * answered, or a meter clap.meters/1 counts is not served.
 */
#include <dlfcn.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clap/entry.h"
#include "clap/factory/plugin-factory.h"
#include "clap/host.h"
#include <lilv/lilv.h>

#include <omx-clap-host/clap_meters.h>

#include "omx_panel_description.h"

/** The extension, whichever face serves it: a closure over the CLAP plugin or the LV2 handle. */
typedef struct {
  const omx_clap_plugin_panel_description_t *clap;
  const clap_plugin_t *plugin;
  const omx_lv2_panel_description_t *lv2;
  void *handle;
} Panel;

static const char *p_fingerprint(const Panel *p) { return p->clap ? p->clap->fingerprint(p->plugin) : p->lv2->fingerprint(p->handle); }
static uint32_t p_family(const Panel *p) { return p->clap ? p->clap->family(p->plugin) : p->lv2->family(p->handle); }
static uint32_t p_count(const Panel *p, uint32_t t) { return p->clap ? p->clap->count(p->plugin, t) : p->lv2->count(p->handle, t); }
static bool p_get(const Panel *p, uint32_t t, uint32_t i, void *out) {
  return p->clap ? p->clap->get(p->plugin, t, i, out) : p->lv2->get(p->handle, t, i, out);
}

/** Every table, in published order; 0 when each answers exactly its count and nothing past it. */
static int dump(const Panel *p) {
  int bad = 0;
  const char *fp = p_fingerprint(p);
  printf("fingerprint\t%s\nfamily\t%" PRIu32 "\n", fp ? fp : "-", p_family(p));
  union {
    omx_clap_panel_section_t s;
    omx_clap_panel_control_t c;
    omx_clap_panel_role_t r;
    omx_clap_panel_display_t d;
    omx_clap_panel_placement_default_t v;
  } u;
  for (uint32_t t = OMX_CLAP_PANEL_SECTIONS; t <= OMX_CLAP_PANEL_PLACEMENT_DEFAULTS; ++t) {
    const uint32_t n = p_count(p, t);
    for (uint32_t i = 0; i < n; ++i) {
      memset(&u, 0xa5, sizeof u);
      if (!p_get(p, t, i, &u)) {
        fprintf(stderr, "panel-probe: table %u entry %u of %u is not served\n", t, i, n);
        bad = 1;
        continue;
      }
      switch (t) {
      case OMX_CLAP_PANEL_SECTIONS: printf("section\t%s\t%s\t%s\n", u.s.key, u.s.label_key, u.s.label); break;
      case OMX_CLAP_PANEL_CONTROLS:
        printf("control\t%" PRIu32 "\t%" PRIu32 "\t%" PRIu32 "\t%s\t%" PRIu32 "\t%" PRIx64 "\t%s\n", u.c.param_id, u.c.section,
               u.c.widget, u.c.label_key, u.c.visible_param_id, u.c.visible_mask, u.c.label);
        break;
      case OMX_CLAP_PANEL_ROLES: printf("role\t%s\t%" PRIu32 "\t%" PRIu32 "\n", u.r.role, u.r.index, u.r.param_id); break;
      case OMX_CLAP_PANEL_DISPLAYS:
        printf("display\t%" PRIu32 "\t%" PRIu32 "\t%" PRIu32 "\t%" PRIu32 "\t%" PRIu32 "\t%" PRIu32 "\t%" PRIu32 "\t%.17g\t%s\t%s\n", u.d.kind,
               u.d.section, u.d.source, u.d.meter_ids[0], u.d.meter_ids[1], u.d.curve_index, u.d.floor_param_id, u.d.floor_db,
               u.d.label_key, u.d.label);
        break;
      default: printf("default\t%" PRIu32 "\t%" PRIu32 "\t%.17g\n", u.v.placement, u.v.param_id, u.v.value); break;
      }
    }
    if (p_get(p, t, n, &u)) {
      fprintf(stderr, "panel-probe: table %u answers entry %u, past its count\n", t, n);
      bad = 1;
    }
  }
  if (p_count(p, OMX_CLAP_PANEL_PLACEMENT_DEFAULTS + 1) != 0 || p_get(p, OMX_CLAP_PANEL_PLACEMENT_DEFAULTS + 1, 0, &u)) {
    fprintf(stderr, "panel-probe: an unknown table is answered\n");
    bad = 1;
  }
  return bad;
}

/** clap.meters/1, the meters a display's ids name: one `meter` line each, in the order served; 0 when every
 * counted meter is served. A face with no meters may serve no extension at all: nothing to print. */
static int dump_meters(const clap_plugin_t *plugin) {
  const clap_plugin_meters_t *m = (const clap_plugin_meters_t *)plugin->get_extension(plugin, CLAP_EXT_METERS);
  if (!m) return 0;
  int bad = 0;
  const uint32_t n = m->count(plugin);
  for (uint32_t i = 0; i < n; ++i) {
    clap_meter_info_t info;
    memset(&info, 0xa5, sizeof info);
    if (!m->get_info(plugin, i, &info)) {
      fprintf(stderr, "panel-probe: clap.meters/1 meter %" PRIu32 " of %" PRIu32 " is not served\n", i, n);
      bad = 1;
      continue;
    }
    printf("meter\t%" PRIu32 "\t%s\t%" PRIu32 "\t%" PRIu32 "\t%.17g\t%.17g\n", info.id, info.name, info.kind, info.channel_count,
           info.min_value, info.max_value);
  }
  return bad;
}

static const void *host_get_extension(const clap_host_t *h, const char *id) {
  (void)h, (void)id;
  return NULL;
}
static void host_noop(const clap_host_t *h) { (void)h; }
static const clap_host_t HOST = {
    CLAP_VERSION_INIT, NULL, "omx panel-probe", "openmixer", "", "0.1", host_get_extension, host_noop, host_noop, host_noop,
};

static int probe_clap(const char *path) {
  void *so = dlopen(path, RTLD_NOW | RTLD_LOCAL);
  if (!so) return fprintf(stderr, "dlopen %s: %s\n", path, dlerror()), 1;
  const clap_plugin_entry_t *entry = (const clap_plugin_entry_t *)dlsym(so, "clap_entry");
  if (!entry || !entry->init(path)) return fprintf(stderr, "%s: no clap_entry\n", path), 1;
  const clap_plugin_factory_t *fac = (const clap_plugin_factory_t *)entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  if (!fac || fac->get_plugin_count(fac) != 1) return fprintf(stderr, "%s: factory\n", path), 1;
  const clap_plugin_t *plugin = fac->create_plugin(fac, &HOST, fac->get_plugin_descriptor(fac, 0)->id);
  if (!plugin || !plugin->init(plugin)) return fprintf(stderr, "%s: create/init\n", path), 1;
  Panel p = {(const omx_clap_plugin_panel_description_t *)plugin->get_extension(plugin, OMX_PANEL_DESCRIPTION_ID), plugin, NULL, NULL};
  int rc = p.clap ? dump(&p) | dump_meters(plugin) : (fprintf(stderr, "%s: no %s extension\n", path, OMX_PANEL_DESCRIPTION_ID), 1);
  plugin->destroy(plugin);
  entry->deinit();
  dlclose(so);
  return rc;
}

static int probe_lv2(const char *bundle, const char *uri) {
  char dir[4096];
  snprintf(dir, sizeof dir, "%s%s", bundle, bundle[0] && bundle[strlen(bundle) - 1] == '/' ? "" : "/");
  LilvWorld *world = lilv_world_new();
  LilvNode *b = lilv_new_file_uri(world, NULL, dir), *u = lilv_new_uri(world, uri);
  lilv_world_load_bundle(world, b);
  const LilvPlugin *plugin = lilv_plugins_get_by_uri(lilv_world_get_all_plugins(world), u);
  int rc = 1;
  LilvInstance *inst = plugin ? lilv_plugin_instantiate(plugin, 48000.0, NULL) : NULL;
  if (!inst) fprintf(stderr, "%s: %s does not instantiate\n", dir, uri);
  else {
    for (uint32_t i = 0; i < lilv_plugin_get_num_ports(plugin); ++i)
      printf("port\t%" PRIu32 "\t%s\n", i, lilv_node_as_string(lilv_port_get_symbol(plugin, lilv_plugin_get_port_by_index(plugin, i))));
    Panel p = {NULL, NULL, (const omx_lv2_panel_description_t *)lilv_instance_get_extension_data(inst, OMX_PANEL_DESCRIPTION_ID),
               lilv_instance_get_handle(inst)};
    rc = p.lv2 ? dump(&p) : (fprintf(stderr, "%s: no %s extension data\n", uri, OMX_PANEL_DESCRIPTION_ID), 1);
    lilv_instance_free(inst);
  }
  lilv_node_free(u), lilv_node_free(b);
  lilv_world_free(world);
  return rc;
}

int main(int argc, char **argv) {
  if (argc == 3 && strcmp(argv[1], "clap") == 0) return probe_clap(argv[2]);
  if (argc == 4 && strcmp(argv[1], "lv2") == 0) return probe_lv2(argv[2], argv[3]);
  fprintf(stderr, "usage: panel-probe clap <x.clap> | panel-probe lv2 <bundle> <uri>\n");
  return 2;
}
