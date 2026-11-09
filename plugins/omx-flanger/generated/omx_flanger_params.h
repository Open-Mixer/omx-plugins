// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_FLANGER_PARAMS_H
#define OMX_FLANGER_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-flanger/omx-flanger.decl.json.
 * Regenerate: `make -C plugins/omx-flanger gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_flanger_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_FLANGER_PARAM_RATE = 0,
  OMX_FLANGER_PARAM_DEPTH = 1,
  OMX_FLANGER_PARAM_FEEDBACK = 2,
  OMX_FLANGER_PARAM_MIX = 3,
  OMX_FLANGER_PARAM_COUNT = 4
};

static const omx_plugin_param OMX_FLANGER_PARAMS[OMX_FLANGER_PARAM_COUNT] = {
  { "rate", "Rate", "Hz", 0.05f, 5.0f, 0.25f, 0u },
  { "depth", "Depth", "ms", 0.0f, 5.0f, 2.0f, 0u },
  { "feedback", "Feedback", "", -0.95f, 0.95f, 0.6f, 0u },
  { "mix", "Mix", "%", 0.0f, 100.0f, 50.0f, 0u },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_FLANGER_PARAM_MODULES[OMX_FLANGER_PARAM_COUNT] = {
  "Flanger",
  "Flanger",
  "Flanger",
  "Flanger",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_FLANGER_PARAM_RATE_MIN 0.05f
#define OMX_FLANGER_PARAM_RATE_MAX 5.0f
#define OMX_FLANGER_PARAM_RATE_DEFAULT 0.25f
#define OMX_FLANGER_PARAM_DEPTH_MIN 0.0f
#define OMX_FLANGER_PARAM_DEPTH_MAX 5.0f
#define OMX_FLANGER_PARAM_DEPTH_DEFAULT 2.0f
#define OMX_FLANGER_PARAM_FEEDBACK_MIN -0.95f
#define OMX_FLANGER_PARAM_FEEDBACK_MAX 0.95f
#define OMX_FLANGER_PARAM_FEEDBACK_DEFAULT 0.6f
#define OMX_FLANGER_PARAM_MIX_MIN 0.0f
#define OMX_FLANGER_PARAM_MIX_MAX 100.0f
#define OMX_FLANGER_PARAM_MIX_DEFAULT 50.0f

/* The identity every face publishes. */
#define OMX_FLANGER_NAME "omx flanger"
#define OMX_FLANGER_VENDOR "openmixer"
#define OMX_FLANGER_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_FLANGER_VERSION "0.19.2"
#define OMX_FLANGER_DESCRIPTION "The console's flanger: one modulated short delay with signed feedback, swept by one oscillator and mixed with the dry signal."
#define OMX_FLANGER_CLAP_ID "org.openmixer.flanger"
#define OMX_FLANGER_CLAP_FEATURES "audio-effect", "flanger", "stereo"
#define OMX_FLANGER_LV2_URI "urn:openmixer:flanger"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_FLANGER_DECL_SOURCE "omx flanger: Rate 0.05 to 5 Hz, Depth 0 to 5 ms, Feedback -0.95 to 0.95, Mix 0 to 100 %"
#define OMX_FLANGER_DECL_DIGEST "323560b13042be1596a25511cdd1cabcddfc4f976cb7c1ab68f318f2ad8fc73e"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), is the face's. */
#define OMX_FLANGER_PARAM_CONTRACT_VERSION 1u
#define OMX_FLANGER_PARAM_CONTRACT_FINGERPRINT "5fe17545e5852d6db37874ddaaadb5ba6a87a1b375a97ac936251fd39b6db747"
static const struct omx_param_contract_row OMX_FLANGER_PARAM_CONTRACT[OMX_FLANGER_PARAM_COUNT] = {
  { 16u, OMX_CLAP_SCALE_LOG, 0.0 }, /* rate hz */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* depth ms */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* feedback none */
  { 14u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* mix pc */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_FLANGER_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family modulation.
 * Declared roles not published:
 *   depth (depth): the depth role takes none, pc, coef, oct, not ms */
#define OMX_FLANGER_PANEL_FINGERPRINT "efbf83c2361c47c85db68bcf5e339be2cc1679cbb35e2ce2c50eb8e39a6907a6"
static const struct omx_panel_section_row OMX_FLANGER_PANEL_SECTIONS[] = {
  { "flanger", "fx.flanger.section.flanger", "Flanger" },
};
static const struct omx_panel_control_row OMX_FLANGER_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.flanger.rate", "Rate", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.flanger.depth", "Depth", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.flanger.feedback", "Feedback", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.flanger.mix", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_FLANGER_PANEL_ROLES[] = {
  { "rate", 0u, 0u },
  { "feedback", 0u, 2u },
  { "mix", 0u, 3u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections. */
static const struct omx_panel_page_row OMX_FLANGER_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 2u, 3u, 1u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_FLANGER_PANEL_PAGE_COUNT 1u
static const struct omx_panel_rows OMX_FLANGER_PANEL = {
  OMX_FLANGER_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_MODULATION,
  OMX_FLANGER_PANEL_SECTIONS, 1u,
  OMX_FLANGER_PANEL_CONTROLS, 4u,
  OMX_FLANGER_PANEL_ROLES, 3u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_FLANGER_LV2_PORT_ENABLED 0u
#define OMX_FLANGER_LV2_PORT_LATENCY 1u
#define OMX_FLANGER_LV2_PORT_IN_L 2u
#define OMX_FLANGER_LV2_PORT_IN_R 3u
#define OMX_FLANGER_LV2_PORT_OUT_L 4u
#define OMX_FLANGER_LV2_PORT_OUT_R 5u
#define OMX_FLANGER_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_FLANGER_LV2_PORT_FIRST_METER 10u
#define OMX_FLANGER_LV2_PORT_COUNT 10u

#endif /* OMX_FLANGER_PARAMS_H */
