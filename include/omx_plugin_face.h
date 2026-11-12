// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_plugin_face.h — what a plugin hands the faces every plugin shares (lib/omx_clap_face.c,
 * lib/omx_lv2_face.c): its generated tables (generated/omx_<kernel>_params.h), its meters, its key
 * port and its core's vtable (omx_<kernel>_core.h). The faces add no arithmetic on the audio path:
 * they drain the host's values, resolve and run through the vtable.
 *
 * tools/gen.mjs writes one `struct omx_face` per plugin, in omx_<kernel>_clap.c and omx_<kernel>_lv2.c.
 */
#ifndef OMX_PLUGIN_FACE_H
#define OMX_PLUGIN_FACE_H

#include <stddef.h>
#include <stdint.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"
#include <omxcontract/omx_resource_hints.h>

/** A plugin's core (omx_<kernel>_core.h) behind function pointers: the kernel's state is `size` bytes the
 * face allocates, and every call takes it untyped. A face without a key gets `run` with a NULL key. */
struct omx_face_core {
  size_t size;
  void (*init)(void *core, uint32_t rate);
  void (*resolve)(void *core, const float *values, int bypass);
  uint32_t (*latency)(const void *core);
  void (*run)(void *core, const float *key, const float *in_l, const float *in_r, float *out_l, float *out_r,
              uint32_t frames);
  /** The kernel's reading of meter `i` after a block (the declaration's `meters`, in order); omx_face_no_meter
   * when the kernel reads none (a function, never NULL: omx-rt-gate classifies a call through a field by the
   * functions an initializer stores into it). */
  float (*meter)(const void *core, uint32_t i);
};

/** Where a meter's values come from: the kernel's reading after the block, or the block's peak at the
 * inputs (before the kernel runs) or at the outputs (after), in dBFS. */
enum omx_face_meter_source { OMX_FACE_METER_READ, OMX_FACE_METER_IN, OMX_FACE_METER_OUT };

/** A meter's kind, by clap.meters/1's numbers (lib/omx_clap_face.c asserts the two agree), so the LV2 face
 * reads it without CLAP's headers. */
enum omx_face_meter_kind { OMX_FACE_METER_KIND_GAIN_REDUCTION = 1, OMX_FACE_METER_KIND_LEVEL = 2 };

/** One of the declaration's meters: the index is the CLAP meter id; its channels sit at `at` among the
 * values the face publishes after a block, and at LV2 port `first_meter + at` onwards. `kind` is an
 * enum omx_face_meter_kind; a gain reduction reads 0 while bypassed, any other kernel reading its `min`. */
struct omx_face_meter {
  const char *name;
  uint32_t kind, channels, at;
  double min, max;
  uint32_t source; /* enum omx_face_meter_source */
};

/** The LV2 face's port indices (generated/omx_<kernel>_params.h's OMX_<K>_LV2_PORT_*): `first_meter` is
 * the first of the meters' output control ports, one per channel in the order the values are published. */
struct omx_face_lv2_ports {
  uint32_t in_l, in_r, key, out_l, out_r, enabled, latency, first_param, first_meter;
};

/** What a core without a meter reading, or a face without a tempo, stores in the field: a call through the field then
 * has a target omx-rt-gate can walk. A face asks omx_face_reads_tempo(), not the field's truth. */
static inline float omx_face_no_meter(const void *core, uint32_t i) { (void)core; (void)i; return 0.0f; }
static inline void omx_face_no_tempo(void *core, double bpm) { (void)core; (void)bpm; }

struct omx_face {
  /* identity */
  const char *clap_id, *name, *vendor, *url, *version, *description;
  const char *const *features; /* NULL-terminated */
  const char *lv2_uri;
  /* org.openmixer.declaration/1 */
  const char *decl_source, *decl_digest;
  /* the declared parameters; the host's bypass is appended after them */
  uint32_t param_count;
  const omx_plugin_param *params;
  const char *const *modules;
  /* org.openmixer.panel-description/1 and clap.remote-controls */
  const struct omx_panel_rows *panel;
  const struct omx_panel_page_row *pages;
  uint32_t page_count;
  /* org.openmixer.param-contract/1 */
  uint32_t contract_version;
  const char *contract_fingerprint;
  const struct omx_param_contract_row *contract;
  /* org.openmixer.resource-hints.draft/0 */
  const omx_resource_hints_t *hints;
  /* the sidechain key's port symbol; NULL: no key */
  const char *key_symbol;
  /* clap.meters/1: `meter_values` values after each block, every meter's channels */
  const struct omx_face_meter *meters;
  uint32_t meter_count, meter_values;
  struct omx_face_lv2_ports lv2;
  const struct omx_face_core *core;
  /* The host's tempo, for a plugin whose descriptor says needs.tempo (its core's omx_<kernel>_core_tempo): called
   * before every resolve with the block's tempo in BPM, 0 when the host handed none (no CLAP transport, or one
   * without CLAP_TRANSPORT_HAS_TEMPO; no LV2 time:Position with time:beatsPerMinute yet). omx_face_no_tempo: reads no
   * tempo (a function, never NULL, for the same reason as `meter`'s). */
  void (*tempo)(void *core, double bpm);
  /* With `tempo`, the LV2 face's atom input port (generated OMX_<K>_LV2_PORT_CONTROL) where time:Position arrives. */
  uint32_t lv2_control;
};

/* The fields generated/omx_<kernel>_params.h names for every plugin, by its OMX_<K>_ prefix: a plugin's
 * descriptor is OMX_FACE_TABLES(<K>) and what is its own (features, key, meters, core). */
#define OMX_FACE_TABLES(K)                                                                                          \
  .clap_id = OMX_##K##_CLAP_ID, .name = OMX_##K##_NAME, .vendor = OMX_##K##_VENDOR, .url = OMX_##K##_URL,          \
  .version = OMX_##K##_VERSION, .description = OMX_##K##_DESCRIPTION, .lv2_uri = OMX_##K##_LV2_URI,                \
  .decl_source = OMX_##K##_DECL_SOURCE, .decl_digest = OMX_##K##_DECL_DIGEST, .param_count = OMX_##K##_PARAM_COUNT, \
  .params = OMX_##K##_PARAMS, .modules = OMX_##K##_PARAM_MODULES, .panel = &OMX_##K##_PANEL,                        \
  .pages = OMX_##K##_PANEL_PAGES, .page_count = OMX_##K##_PANEL_PAGE_COUNT,                                         \
  .contract_version = OMX_##K##_PARAM_CONTRACT_VERSION, .contract_fingerprint = OMX_##K##_PARAM_CONTRACT_FINGERPRINT, \
  .contract = OMX_##K##_PARAM_CONTRACT, .hints = &OMX_##K##_RESOURCE_HINTS

/** Whether the face's plugin reads the host's tempo. */
static inline int omx_face_reads_tempo(const struct omx_face *f) { return f->tempo != omx_face_no_tempo; }

#endif /* OMX_PLUGIN_FACE_H */
