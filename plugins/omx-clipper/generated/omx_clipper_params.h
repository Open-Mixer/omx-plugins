// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_CLIPPER_PARAMS_H
#define OMX_CLIPPER_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-clipper/omx-clipper.decl.json.
 * Regenerate: `make -C plugins/omx-clipper gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_clipper_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_CLIPPER_PARAM_DRIVE_DB = 0,
  OMX_CLIPPER_PARAM_CEILING_DB = 1,
  OMX_CLIPPER_PARAM_SOFTNESS_PCT = 2,
  OMX_CLIPPER_PARAM_COUNT = 3
};

static const omx_plugin_param OMX_CLIPPER_PARAMS[OMX_CLIPPER_PARAM_COUNT] = {
  { "driveDb", "Drive", "dB", 0.0f, 24.0f, 0.0f, 0u },
  { "ceilingDb", "Ceiling", "dB", -24.0f, 0.0f, 0.0f, 0u },
  { "softnessPct", "Softness", "%", 0.0f, 100.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_CLIPPER_PARAM_MODULES[OMX_CLIPPER_PARAM_COUNT] = {
  "Clipper",
  "Clipper",
  "Clipper",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_CLIPPER_PARAM_DRIVE_DB_MIN 0.0f
#define OMX_CLIPPER_PARAM_DRIVE_DB_MAX 24.0f
#define OMX_CLIPPER_PARAM_DRIVE_DB_DEFAULT 0.0f
#define OMX_CLIPPER_PARAM_CEILING_DB_MIN -24.0f
#define OMX_CLIPPER_PARAM_CEILING_DB_MAX 0.0f
#define OMX_CLIPPER_PARAM_CEILING_DB_DEFAULT 0.0f
#define OMX_CLIPPER_PARAM_SOFTNESS_PCT_MIN 0.0f
#define OMX_CLIPPER_PARAM_SOFTNESS_PCT_MAX 100.0f
#define OMX_CLIPPER_PARAM_SOFTNESS_PCT_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_CLIPPER_NAME "omx clipper"
#define OMX_CLIPPER_VENDOR "openmixer"
#define OMX_CLIPPER_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_CLIPPER_VERSION "0.19.2"
#define OMX_CLIPPER_DESCRIPTION "Clips the peaks at a ceiling, with drive in front and an adjustable softness of the bend."
#define OMX_CLIPPER_CLAP_ID "org.openmixer.clipper"
#define OMX_CLIPPER_CLAP_FEATURES "audio-effect", "distortion", "stereo"
#define OMX_CLIPPER_LV2_URI "urn:openmixer:clipper"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_CLIPPER_DECL_SOURCE "omx clipper: Drive 0 to 24 dB, Ceiling -24 to 0 dB, Softness 0 to 100 %"
#define OMX_CLIPPER_DECL_DIGEST "e77c8f23e4bc2293f2cfad1a340186f5a3021a47cb98d7500d66f000b7f955dd"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_CLIPPER_PARAM_CONTRACT_VERSION 2u
#define OMX_CLIPPER_PARAM_CONTRACT_FINGERPRINT "2ca379ddafa020c0e207a19f14c068e2fc66d63ca27faad5da176cbd84ccf43a"
static const struct omx_param_contract_row OMX_CLIPPER_PARAM_CONTRACT[OMX_CLIPPER_PARAM_COUNT] = {
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* driveDb db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ceilingDb db */
  { 14u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* softnessPct pc */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_CLIPPER_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family none. */
#define OMX_CLIPPER_PANEL_FINGERPRINT "8de33b0a494463ab73407d81f51ecf7f597a2d40e2b044d037d4ed3a96b2cb69"
static const struct omx_panel_section_row OMX_CLIPPER_PANEL_SECTIONS[] = {
  { "clipper", "fx.clipper.section.clipper", "Clipper" },
};
static const struct omx_panel_control_row OMX_CLIPPER_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.clipper.drive_db", "Drive", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.clipper.ceiling_db", "Ceiling", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.clipper.softness_pct", "Softness", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_CLIPPER_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_CLIPPER_PANEL_PAGE_COUNT 1u
static const struct omx_panel_rows OMX_CLIPPER_PANEL = {
  OMX_CLIPPER_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_NONE,
  OMX_CLIPPER_PANEL_SECTIONS, 1u,
  OMX_CLIPPER_PANEL_CONTROLS, 3u,
  NULL, 0u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_CLIPPER_LV2_PORT_ENABLED 0u
#define OMX_CLIPPER_LV2_PORT_LATENCY 1u
#define OMX_CLIPPER_LV2_PORT_IN_L 2u
#define OMX_CLIPPER_LV2_PORT_IN_R 3u
#define OMX_CLIPPER_LV2_PORT_OUT_L 4u
#define OMX_CLIPPER_LV2_PORT_OUT_R 5u
#define OMX_CLIPPER_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_CLIPPER_LV2_PORT_FIRST_METER 9u
#define OMX_CLIPPER_LV2_PORT_COUNT 9u

#endif /* OMX_CLIPPER_PARAMS_H */
