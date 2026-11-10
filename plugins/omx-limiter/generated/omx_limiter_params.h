// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_LIMITER_PARAMS_H
#define OMX_LIMITER_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-limiter/omx-limiter.decl.json.
 * Regenerate: `make -C plugins/omx-limiter gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_limiter_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_LIMITER_PARAM_CEILING_DB = 0,
  OMX_LIMITER_PARAM_LOOKAHEAD_MS = 1,
  OMX_LIMITER_PARAM_RELEASE_MS = 2,
  OMX_LIMITER_PARAM_COUNT = 3
};

static const omx_plugin_param OMX_LIMITER_PARAMS[OMX_LIMITER_PARAM_COUNT] = {
  { "ceilingDb", "Ceiling", "dBFS", -12.0f, 0.0f, -1.0f, 0u },
  { "lookaheadMs", "Lookahead", "ms", 0.5f, 5.0f, 1.5f, 0u },
  { "releaseMs", "Release", "ms", 1.0f, 1000.0f, 50.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_LIMITER_PARAM_MODULES[OMX_LIMITER_PARAM_COUNT] = {
  "Limiter",
  "Limiter",
  "Limiter",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_LIMITER_PARAM_CEILING_DB_MIN -12.0f
#define OMX_LIMITER_PARAM_CEILING_DB_MAX 0.0f
#define OMX_LIMITER_PARAM_CEILING_DB_DEFAULT -1.0f
#define OMX_LIMITER_PARAM_LOOKAHEAD_MS_MIN 0.5f
#define OMX_LIMITER_PARAM_LOOKAHEAD_MS_MAX 5.0f
#define OMX_LIMITER_PARAM_LOOKAHEAD_MS_DEFAULT 1.5f
#define OMX_LIMITER_PARAM_RELEASE_MS_MIN 1.0f
#define OMX_LIMITER_PARAM_RELEASE_MS_MAX 1000.0f
#define OMX_LIMITER_PARAM_RELEASE_MS_DEFAULT 50.0f

/* The identity every face publishes. */
#define OMX_LIMITER_NAME "omx limiter"
#define OMX_LIMITER_VENDOR "openmixer"
#define OMX_LIMITER_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_LIMITER_VERSION "0.19.2"
#define OMX_LIMITER_DESCRIPTION "The precision limiter: a look-ahead, true-peak, stereo-linked brickwall."
#define OMX_LIMITER_CLAP_ID "org.openmixer.limiter"
#define OMX_LIMITER_CLAP_FEATURES "audio-effect", "limiter", "stereo"
#define OMX_LIMITER_LV2_URI "urn:openmixer:limiter"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_LIMITER_DECL_SOURCE "omx limiter: Ceiling -12 to 0 dBFS, Lookahead 0.5 to 5 ms, Release 1 to 1000 ms"
#define OMX_LIMITER_DECL_DIGEST "aaae50a0d0219bc9406793d74ddc52d57717d8a4433e4d39f174cd5ef6e4c52c"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_LIMITER_PARAM_CONTRACT_VERSION 1u
#define OMX_LIMITER_PARAM_CONTRACT_FINGERPRINT "5eac6b3d4921ead5ed21ef89862a7f21ca7a6aca16cd6c5d49a320f0e52fb65e"
static const struct omx_param_contract_row OMX_LIMITER_PARAM_CONTRACT[OMX_LIMITER_PARAM_COUNT] = {
  { 256u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ceilingDb dbfs */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* lookaheadMs ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* releaseMs ms */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_LIMITER_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family limiter. */
#define OMX_LIMITER_PANEL_FINGERPRINT "5b5052495463e37e4a8540c1c9343419e7f328ade7d9186920937d12c3c0ffdd"
static const struct omx_panel_section_row OMX_LIMITER_PANEL_SECTIONS[] = {
  { "limiter", "fx.limiter.section.limiter", "Limiter" },
};
static const struct omx_panel_control_row OMX_LIMITER_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.limiter.ceiling_db", "Ceiling", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.limiter.lookahead_ms", "Lookahead", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.limiter.release_ms", "Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_LIMITER_PANEL_ROLES[] = {
  { "ceiling", 0u, 0u },
  { "release", 0u, 2u },
  { "lookahead", 0u, 1u },
};
static const struct omx_panel_display_row OMX_LIMITER_PANEL_DISPLAYS[] = {
  { OMX_CLAP_DISPLAY_GAIN_REDUCTION_BAR, 0u, OMX_CLAP_DISPLAY_FROM_METERS, { 0u, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.limiter.display.gain_reduction_bar", "Gain reduction" },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_LIMITER_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 2u, 1u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_LIMITER_PANEL_PAGE_COUNT 1u
static const struct omx_panel_rows OMX_LIMITER_PANEL = {
  OMX_LIMITER_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_LIMITER,
  OMX_LIMITER_PANEL_SECTIONS, 1u,
  OMX_LIMITER_PANEL_CONTROLS, 3u,
  OMX_LIMITER_PANEL_ROLES, 3u,
  OMX_LIMITER_PANEL_DISPLAYS, 1u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_LIMITER_LV2_PORT_ENABLED 0u
#define OMX_LIMITER_LV2_PORT_LATENCY 1u
#define OMX_LIMITER_LV2_PORT_IN_L 3u
#define OMX_LIMITER_LV2_PORT_IN_R 4u
#define OMX_LIMITER_LV2_PORT_OUT_L 5u
#define OMX_LIMITER_LV2_PORT_OUT_R 6u
#define OMX_LIMITER_LV2_PORT_FIRST_PARAM 7u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_LIMITER_LV2_PORT_FIRST_METER 2u
#define OMX_LIMITER_LV2_PORT_COUNT 10u

#endif /* OMX_LIMITER_PARAMS_H */
