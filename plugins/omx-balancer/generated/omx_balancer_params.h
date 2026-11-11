// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_BALANCER_PARAMS_H
#define OMX_BALANCER_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-balancer/omx-balancer.decl.json.
 * Regenerate: `make -C plugins/omx-balancer gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_balancer_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_BALANCER_PARAM_AMOUNT_PCT = 0,
  OMX_BALANCER_PARAM_RANGE_DB = 1,
  OMX_BALANCER_PARAM_SPEED_DB_PER_S = 2,
  OMX_BALANCER_PARAM_SLOPE_DB_OCT = 3,
  OMX_BALANCER_PARAM_COUNT = 4
};

static const omx_plugin_param OMX_BALANCER_PARAMS[OMX_BALANCER_PARAM_COUNT] = {
  { "amountPct", "Amount", "%", 0.0f, 100.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "rangeDb", "Range", "dB", 1.0f, 12.0f, 3.0f, 0u },
  { "speedDbPerS", "Speed", "dB", 0.1f, 6.0f, 0.5f, 0u },
  { "slopeDbOct", "Slope", "dB/oct", -6.0f, 0.0f, -3.0f, 0u },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_BALANCER_PARAM_MODULES[OMX_BALANCER_PARAM_COUNT] = {
  "Balancer",
  "Balancer",
  "Balancer",
  "Balancer",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_BALANCER_PARAM_AMOUNT_PCT_MIN 0.0f
#define OMX_BALANCER_PARAM_AMOUNT_PCT_MAX 100.0f
#define OMX_BALANCER_PARAM_AMOUNT_PCT_DEFAULT 0.0f
#define OMX_BALANCER_PARAM_RANGE_DB_MIN 1.0f
#define OMX_BALANCER_PARAM_RANGE_DB_MAX 12.0f
#define OMX_BALANCER_PARAM_RANGE_DB_DEFAULT 3.0f
#define OMX_BALANCER_PARAM_SPEED_DB_PER_S_MIN 0.1f
#define OMX_BALANCER_PARAM_SPEED_DB_PER_S_MAX 6.0f
#define OMX_BALANCER_PARAM_SPEED_DB_PER_S_DEFAULT 0.5f
#define OMX_BALANCER_PARAM_SLOPE_DB_OCT_MIN -6.0f
#define OMX_BALANCER_PARAM_SLOPE_DB_OCT_MAX 0.0f
#define OMX_BALANCER_PARAM_SLOPE_DB_OCT_DEFAULT -3.0f

/* The identity every face publishes. */
#define OMX_BALANCER_NAME "omx balancer"
#define OMX_BALANCER_VENDOR "openmixer"
#define OMX_BALANCER_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_BALANCER_VERSION "0.19.3"
#define OMX_BALANCER_DESCRIPTION "Walks four bands slowly toward a target tone, so a mix settles toward a pink-noise balance without chasing notes."
#define OMX_BALANCER_CLAP_ID "org.openmixer.balancer"
#define OMX_BALANCER_CLAP_FEATURES "audio-effect", "equalizer", "stereo"
#define OMX_BALANCER_LV2_URI "urn:openmixer:balancer"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_BALANCER_DECL_SOURCE "omx balancer: Amount 0 to 100 %, Range 1 to 12 dB, Speed 0.1 to 6 dB, Slope -6 to 0 dB/oct"
#define OMX_BALANCER_DECL_DIGEST "427d243fbeeca3d3a7f7a86bb6f9c0a2a75ff42890268e7e92547d573480a461"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_BALANCER_PARAM_CONTRACT_VERSION 2u
#define OMX_BALANCER_PARAM_CONTRACT_FINGERPRINT "5e755c2ceeb1ca618602fc5b9d39aef3f7edcbe81ed218d629a954588962a79d"
static const struct omx_param_contract_row OMX_BALANCER_PARAM_CONTRACT[OMX_BALANCER_PARAM_COUNT] = {
  { 14u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* amountPct pc */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* rangeDb db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* speedDbPerS db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* slopeDbOct none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_BALANCER_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family none. */
#define OMX_BALANCER_PANEL_FINGERPRINT "2950b38caca95ec0ca123d2ae840a261e90f6462ff63bc7bff42a704c6ec9884"
static const struct omx_panel_section_row OMX_BALANCER_PANEL_SECTIONS[] = {
  { "balancer", "fx.balancer.section.balancer", "Balancer" },
};
static const struct omx_panel_control_row OMX_BALANCER_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.balancer.amount_pct", "Amount", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.balancer.range_db", "Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.balancer.speed_db_per_s", "Speed", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.balancer.slope_db_oct", "Slope", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_BALANCER_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_BALANCER_PANEL_PAGE_COUNT 1u
static const struct omx_panel_rows OMX_BALANCER_PANEL = {
  OMX_BALANCER_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_NONE,
  OMX_BALANCER_PANEL_SECTIONS, 1u,
  OMX_BALANCER_PANEL_CONTROLS, 4u,
  NULL, 0u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_BALANCER_LV2_PORT_ENABLED 0u
#define OMX_BALANCER_LV2_PORT_LATENCY 1u
#define OMX_BALANCER_LV2_PORT_IN_L 2u
#define OMX_BALANCER_LV2_PORT_IN_R 3u
#define OMX_BALANCER_LV2_PORT_OUT_L 4u
#define OMX_BALANCER_LV2_PORT_OUT_R 5u
#define OMX_BALANCER_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_BALANCER_LV2_PORT_FIRST_METER 10u
#define OMX_BALANCER_LV2_PORT_COUNT 10u

#endif /* OMX_BALANCER_PARAMS_H */
