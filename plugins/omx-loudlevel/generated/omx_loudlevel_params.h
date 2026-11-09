// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_LOUDLEVEL_PARAMS_H
#define OMX_LOUDLEVEL_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-loudlevel/omx-loudlevel.decl.json.
 * Regenerate: `make -C plugins/omx-loudlevel gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_loudlevel_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_LOUDLEVEL_PARAM_TARGET_LUFS = 0,
  OMX_LOUDLEVEL_PARAM_RANGE_DB = 1,
  OMX_LOUDLEVEL_PARAM_SPEED_DB_PER_S = 2,
  OMX_LOUDLEVEL_PARAM_GATE_LUFS = 3,
  OMX_LOUDLEVEL_PARAM_WINDOW = 4,
  OMX_LOUDLEVEL_PARAM_COUNT = 5
};

static const omx_plugin_param OMX_LOUDLEVEL_PARAMS[OMX_LOUDLEVEL_PARAM_COUNT] = {
  { "targetLufs", "Target", "dB", -40.0f, -6.0f, -23.0f, 0u },
  { "rangeDb", "Range", "dB", 0.0f, 24.0f, 0.0f, 0u },
  { "speedDbPerS", "Speed", "dB", 0.1f, 6.0f, 1.0f, 0u },
  { "gateLufs", "Gate", "dB", -70.0f, -30.0f, -50.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "window", "Window", "", 0.0f, 1.0f, 1.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_LOUDLEVEL_PARAM_MODULES[OMX_LOUDLEVEL_PARAM_COUNT] = {
  "Loudlevel",
  "Loudlevel",
  "Loudlevel",
  "Loudlevel",
  "Loudlevel",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_LOUDLEVEL_PARAM_TARGET_LUFS_MIN -40.0f
#define OMX_LOUDLEVEL_PARAM_TARGET_LUFS_MAX -6.0f
#define OMX_LOUDLEVEL_PARAM_TARGET_LUFS_DEFAULT -23.0f
#define OMX_LOUDLEVEL_PARAM_RANGE_DB_MIN 0.0f
#define OMX_LOUDLEVEL_PARAM_RANGE_DB_MAX 24.0f
#define OMX_LOUDLEVEL_PARAM_RANGE_DB_DEFAULT 0.0f
#define OMX_LOUDLEVEL_PARAM_SPEED_DB_PER_S_MIN 0.1f
#define OMX_LOUDLEVEL_PARAM_SPEED_DB_PER_S_MAX 6.0f
#define OMX_LOUDLEVEL_PARAM_SPEED_DB_PER_S_DEFAULT 1.0f
#define OMX_LOUDLEVEL_PARAM_GATE_LUFS_MIN -70.0f
#define OMX_LOUDLEVEL_PARAM_GATE_LUFS_MAX -30.0f
#define OMX_LOUDLEVEL_PARAM_GATE_LUFS_DEFAULT -50.0f
#define OMX_LOUDLEVEL_PARAM_WINDOW_MIN 0.0f
#define OMX_LOUDLEVEL_PARAM_WINDOW_MAX 1.0f
#define OMX_LOUDLEVEL_PARAM_WINDOW_DEFAULT 1.0f

/* The identity every face publishes. */
#define OMX_LOUDLEVEL_NAME "omx loudlevel"
#define OMX_LOUDLEVEL_VENDOR "openmixer"
#define OMX_LOUDLEVEL_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_LOUDLEVEL_VERSION "0.19.2"
#define OMX_LOUDLEVEL_DESCRIPTION "Holds a signal at a loudness target by moving its gain slowly, with a gate so pauses are left alone."
#define OMX_LOUDLEVEL_CLAP_ID "org.openmixer.loudlevel"
#define OMX_LOUDLEVEL_CLAP_FEATURES "audio-effect", "utility", "stereo"
#define OMX_LOUDLEVEL_LV2_URI "urn:openmixer:loudlevel"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_LOUDLEVEL_DECL_SOURCE "omx loudlevel: Target -40 to -6 dB, Range 0 to 24 dB, Speed 0.1 to 6 dB, Gate -70 to -30 dB, Window 0 to 1"
#define OMX_LOUDLEVEL_DECL_DIGEST "5ca40a12d69abada26cd79a670a6090d0dabf4de21b92531fafaccc34968ba61"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), is the face's. */
#define OMX_LOUDLEVEL_PARAM_CONTRACT_VERSION 1u
#define OMX_LOUDLEVEL_PARAM_CONTRACT_FINGERPRINT "33b959ab49e65da93fa433b0263dae01c9d5d54c62a1705229854dc45467f2dc"
static const struct omx_param_contract_row OMX_LOUDLEVEL_PARAM_CONTRACT[OMX_LOUDLEVEL_PARAM_COUNT] = {
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* targetLufs db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* rangeDb db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* speedDbPerS db */
  { 13u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* gateLufs db */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* window none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_LOUDLEVEL_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family none. */
#define OMX_LOUDLEVEL_PANEL_FINGERPRINT "ad026efb5b81e04155533091f7118f6c5aa650ab148e228989562ca21c607ef0"
static const struct omx_panel_section_row OMX_LOUDLEVEL_PANEL_SECTIONS[] = {
  { "loudlevel", "fx.loudlevel.section.loudlevel", "Loudlevel" },
};
static const struct omx_panel_control_row OMX_LOUDLEVEL_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.loudlevel.target_lufs", "Target", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.loudlevel.range_db", "Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.loudlevel.speed_db_per_s", "Speed", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.loudlevel.gate_lufs", "Gate", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.loudlevel.window", "Window", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_LOUDLEVEL_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Loudlevel", "Loudlevel", { 4u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_LOUDLEVEL_PANEL_PAGE_COUNT 2u
static const struct omx_panel_rows OMX_LOUDLEVEL_PANEL = {
  OMX_LOUDLEVEL_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_NONE,
  OMX_LOUDLEVEL_PANEL_SECTIONS, 1u,
  OMX_LOUDLEVEL_PANEL_CONTROLS, 5u,
  NULL, 0u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_LOUDLEVEL_LV2_PORT_ENABLED 0u
#define OMX_LOUDLEVEL_LV2_PORT_LATENCY 1u
#define OMX_LOUDLEVEL_LV2_PORT_IN_L 2u
#define OMX_LOUDLEVEL_LV2_PORT_IN_R 3u
#define OMX_LOUDLEVEL_LV2_PORT_OUT_L 4u
#define OMX_LOUDLEVEL_LV2_PORT_OUT_R 5u
#define OMX_LOUDLEVEL_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_LOUDLEVEL_LV2_PORT_FIRST_METER 11u
#define OMX_LOUDLEVEL_LV2_PORT_COUNT 11u

#endif /* OMX_LOUDLEVEL_PARAMS_H */
