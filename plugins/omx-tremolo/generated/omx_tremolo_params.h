// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_TREMOLO_PARAMS_H
#define OMX_TREMOLO_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-tremolo/omx-tremolo.decl.json.
 * Regenerate: `make -C plugins/omx-tremolo gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_tremolo_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_TREMOLO_PARAM_RATE_HZ = 0,
  OMX_TREMOLO_PARAM_DEPTH = 1,
  OMX_TREMOLO_PARAM_MIX = 2,
  OMX_TREMOLO_PARAM_MODE = 3,
  OMX_TREMOLO_PARAM_COUNT = 4
};

static const omx_plugin_param OMX_TREMOLO_PARAMS[OMX_TREMOLO_PARAM_COUNT] = {
  { "rateHz", "Rate", "Hz", 0.1f, 20.0f, 4.0f, 0u },
  { "depth", "Depth", "%", 0.0f, 100.0f, 50.0f, 0u },
  { "mix", "Mix", "%", 0.0f, 100.0f, 100.0f, 0u },
  { "mode", "Mode", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_TREMOLO_PARAM_MODULES[OMX_TREMOLO_PARAM_COUNT] = {
  "Tremolo",
  "Tremolo",
  "Tremolo",
  "Tremolo",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_TREMOLO_PARAM_RATE_HZ_MIN 0.1f
#define OMX_TREMOLO_PARAM_RATE_HZ_MAX 20.0f
#define OMX_TREMOLO_PARAM_RATE_HZ_DEFAULT 4.0f
#define OMX_TREMOLO_PARAM_DEPTH_MIN 0.0f
#define OMX_TREMOLO_PARAM_DEPTH_MAX 100.0f
#define OMX_TREMOLO_PARAM_DEPTH_DEFAULT 50.0f
#define OMX_TREMOLO_PARAM_MIX_MIN 0.0f
#define OMX_TREMOLO_PARAM_MIX_MAX 100.0f
#define OMX_TREMOLO_PARAM_MIX_DEFAULT 100.0f
#define OMX_TREMOLO_PARAM_MODE_MIN 0.0f
#define OMX_TREMOLO_PARAM_MODE_MAX 1.0f
#define OMX_TREMOLO_PARAM_MODE_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_TREMOLO_NAME "omx tremolo"
#define OMX_TREMOLO_VENDOR "openmixer"
#define OMX_TREMOLO_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_TREMOLO_VERSION "0.19.2"
#define OMX_TREMOLO_DESCRIPTION "The console's tremolo and auto-pan: one oscillator turned into a level change on both legs, or into a left-right pan."
#define OMX_TREMOLO_CLAP_ID "org.openmixer.tremolo"
#define OMX_TREMOLO_CLAP_FEATURES "audio-effect", "tremolo", "stereo"
#define OMX_TREMOLO_LV2_URI "urn:openmixer:tremolo"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_TREMOLO_DECL_SOURCE "omx tremolo: Rate 0.1 to 20 Hz, Depth 0 to 100 %, Mix 0 to 100 %, Mode 0 to 1"
#define OMX_TREMOLO_DECL_DIGEST "b57991962924d939b9966fa18b44ce86bb73f3c940611aaf165dd227ec9b6d1a"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), is the face's. */
#define OMX_TREMOLO_PARAM_CONTRACT_VERSION 1u
#define OMX_TREMOLO_PARAM_CONTRACT_FINGERPRINT "7eda7f36d4553d98706b108002422d309b187291a28efb210a2d3c6369dee62f"
static const struct omx_param_contract_row OMX_TREMOLO_PARAM_CONTRACT[OMX_TREMOLO_PARAM_COUNT] = {
  { 16u, OMX_CLAP_SCALE_LOG, 0.0 }, /* rateHz hz */
  { 14u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* depth pc */
  { 14u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* mix pc */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* mode none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_TREMOLO_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family modulation. */
#define OMX_TREMOLO_PANEL_FINGERPRINT "ea8780516cae00fd4bd65468243ec11aeb98b3b9afda8d95b3addb72f3da5a2a"
static const struct omx_panel_section_row OMX_TREMOLO_PANEL_SECTIONS[] = {
  { "tremolo", "fx.tremolo.section.tremolo", "Tremolo" },
};
static const struct omx_panel_control_row OMX_TREMOLO_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.tremolo.rate_hz", "Rate", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.tremolo.depth", "Depth", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.tremolo.mix", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.tremolo.mode", "Mode", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_TREMOLO_PANEL_ROLES[] = {
  { "rate", 0u, 0u },
  { "depth", 0u, 1u },
  { "mix", 0u, 2u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_TREMOLO_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_TREMOLO_PANEL_PAGE_COUNT 1u
static const struct omx_panel_rows OMX_TREMOLO_PANEL = {
  OMX_TREMOLO_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_MODULATION,
  OMX_TREMOLO_PANEL_SECTIONS, 1u,
  OMX_TREMOLO_PANEL_CONTROLS, 4u,
  OMX_TREMOLO_PANEL_ROLES, 3u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_TREMOLO_LV2_PORT_ENABLED 0u
#define OMX_TREMOLO_LV2_PORT_LATENCY 1u
#define OMX_TREMOLO_LV2_PORT_IN_L 2u
#define OMX_TREMOLO_LV2_PORT_IN_R 3u
#define OMX_TREMOLO_LV2_PORT_OUT_L 4u
#define OMX_TREMOLO_LV2_PORT_OUT_R 5u
#define OMX_TREMOLO_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_TREMOLO_LV2_PORT_FIRST_METER 10u
#define OMX_TREMOLO_LV2_PORT_COUNT 10u

#endif /* OMX_TREMOLO_PARAMS_H */
