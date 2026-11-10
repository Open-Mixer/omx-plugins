// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_SUBOCT_PARAMS_H
#define OMX_SUBOCT_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-suboct/omx-suboct.decl.json.
 * Regenerate: `make -C plugins/omx-suboct gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_suboct_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_SUBOCT_PARAM_RANGE = 0,
  OMX_SUBOCT_PARAM_OCT1 = 1,
  OMX_SUBOCT_PARAM_OCT2 = 2,
  OMX_SUBOCT_PARAM_DRY = 3,
  OMX_SUBOCT_PARAM_TRACK_GATE_DB = 4,
  OMX_SUBOCT_PARAM_COUNT = 5
};

static const omx_plugin_param OMX_SUBOCT_PARAMS[OMX_SUBOCT_PARAM_COUNT] = {
  { "range", "Range", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "oct1", "Oct1", "%", 0.0f, 100.0f, 100.0f, 0u },
  { "oct2", "Oct2", "%", 0.0f, 100.0f, 0.0f, 0u },
  { "dry", "Dry", "%", 0.0f, 100.0f, 100.0f, 0u },
  { "trackGateDb", "Track Gate", "dBFS", -80.0f, -20.0f, -50.0f, 0u },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_SUBOCT_PARAM_MODULES[OMX_SUBOCT_PARAM_COUNT] = {
  "Suboct",
  "Suboct",
  "Suboct",
  "Suboct",
  "Suboct",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_SUBOCT_PARAM_RANGE_MIN 0.0f
#define OMX_SUBOCT_PARAM_RANGE_MAX 2.0f
#define OMX_SUBOCT_PARAM_RANGE_DEFAULT 0.0f
#define OMX_SUBOCT_PARAM_OCT1_MIN 0.0f
#define OMX_SUBOCT_PARAM_OCT1_MAX 100.0f
#define OMX_SUBOCT_PARAM_OCT1_DEFAULT 100.0f
#define OMX_SUBOCT_PARAM_OCT2_MIN 0.0f
#define OMX_SUBOCT_PARAM_OCT2_MAX 100.0f
#define OMX_SUBOCT_PARAM_OCT2_DEFAULT 0.0f
#define OMX_SUBOCT_PARAM_DRY_MIN 0.0f
#define OMX_SUBOCT_PARAM_DRY_MAX 100.0f
#define OMX_SUBOCT_PARAM_DRY_DEFAULT 100.0f
#define OMX_SUBOCT_PARAM_TRACK_GATE_DB_MIN -80.0f
#define OMX_SUBOCT_PARAM_TRACK_GATE_DB_MAX -20.0f
#define OMX_SUBOCT_PARAM_TRACK_GATE_DB_DEFAULT -50.0f

/* The identity every face publishes. */
#define OMX_SUBOCT_NAME "omx suboct"
#define OMX_SUBOCT_VENDOR "openmixer"
#define OMX_SUBOCT_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_SUBOCT_VERSION "0.19.2"
#define OMX_SUBOCT_DESCRIPTION "Adds a note one or two octaves below a single played line, by dividing its pitch, with a tracking range and a gate for quiet input."
#define OMX_SUBOCT_CLAP_ID "org.openmixer.suboct"
#define OMX_SUBOCT_CLAP_FEATURES "audio-effect", "pitch-shifter", "stereo"
#define OMX_SUBOCT_LV2_URI "urn:openmixer:suboct"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_SUBOCT_DECL_SOURCE "omx suboct: Range 0 to 2, Oct1 0 to 100 %, Oct2 0 to 100 %, Dry 0 to 100 %, Track Gate -80 to -20 dBFS"
#define OMX_SUBOCT_DECL_DIGEST "6e650f4f11758449a64b5a67752d62f89f3c852e48f19bff02327520610abbb3"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_SUBOCT_PARAM_CONTRACT_VERSION 2u
#define OMX_SUBOCT_PARAM_CONTRACT_FINGERPRINT "e3f1c410cb527f1a95e0e7e5e53aac3e6447468e70dcdba4148b074ffdd165e6"
static const struct omx_param_contract_row OMX_SUBOCT_PARAM_CONTRACT[OMX_SUBOCT_PARAM_COUNT] = {
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* range none */
  { 14u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* oct1 pc */
  { 14u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* oct2 pc */
  { 14u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* dry pc */
  { 256u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* trackGateDb dbfs */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_SUBOCT_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family none. */
#define OMX_SUBOCT_PANEL_FINGERPRINT "2d67df6940c34f31b282d9a6f9c3a3d8e81db8b594798fc77de9bc6ec40e6a14"
static const struct omx_panel_section_row OMX_SUBOCT_PANEL_SECTIONS[] = {
  { "suboct", "fx.suboct.section.suboct", "Suboct" },
};
static const struct omx_panel_control_row OMX_SUBOCT_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.suboct.range", "Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.suboct.oct1", "Oct1", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.suboct.oct2", "Oct2", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.suboct.dry", "Dry", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.suboct.track_gate_db", "Track Gate", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_SUBOCT_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Suboct", "Suboct", { 4u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_SUBOCT_PANEL_PAGE_COUNT 2u
static const struct omx_panel_rows OMX_SUBOCT_PANEL = {
  OMX_SUBOCT_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_NONE,
  OMX_SUBOCT_PANEL_SECTIONS, 1u,
  OMX_SUBOCT_PANEL_CONTROLS, 5u,
  NULL, 0u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_SUBOCT_LV2_PORT_ENABLED 0u
#define OMX_SUBOCT_LV2_PORT_LATENCY 1u
#define OMX_SUBOCT_LV2_PORT_IN_L 2u
#define OMX_SUBOCT_LV2_PORT_IN_R 3u
#define OMX_SUBOCT_LV2_PORT_OUT_L 4u
#define OMX_SUBOCT_LV2_PORT_OUT_R 5u
#define OMX_SUBOCT_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_SUBOCT_LV2_PORT_FIRST_METER 11u
#define OMX_SUBOCT_LV2_PORT_COUNT 11u

#endif /* OMX_SUBOCT_PARAMS_H */
