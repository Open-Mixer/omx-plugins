// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_UPWARD_PARAMS_H
#define OMX_UPWARD_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-upward/omx-upward.decl.json.
 * Regenerate: `make -C plugins/omx-upward gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_upward_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_UPWARD_PARAM_THRESHOLD_DB = 0,
  OMX_UPWARD_PARAM_RATIO = 1,
  OMX_UPWARD_PARAM_RANGE_DB = 2,
  OMX_UPWARD_PARAM_ATTACK_MS = 3,
  OMX_UPWARD_PARAM_RELEASE_MS = 4,
  OMX_UPWARD_PARAM_COUNT = 5
};

static const omx_plugin_param OMX_UPWARD_PARAMS[OMX_UPWARD_PARAM_COUNT] = {
  { "thresholdDb", "Threshold", "dB", -70.0f, 0.0f, -40.0f, 0u },
  { "ratio", "Ratio", "", 1.0f, 10.0f, 1.0f, 0u },
  { "rangeDb", "Range", "dB", 0.0f, 24.0f, 6.0f, 0u },
  { "attackMs", "Attack", "ms", 0.1f, 100.0f, 5.0f, 0u },
  { "releaseMs", "Release", "ms", 5.0f, 3000.0f, 200.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_UPWARD_PARAM_MODULES[OMX_UPWARD_PARAM_COUNT] = {
  "Upward",
  "Upward",
  "Upward",
  "Upward",
  "Upward",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_UPWARD_PARAM_THRESHOLD_DB_MIN -70.0f
#define OMX_UPWARD_PARAM_THRESHOLD_DB_MAX 0.0f
#define OMX_UPWARD_PARAM_THRESHOLD_DB_DEFAULT -40.0f
#define OMX_UPWARD_PARAM_RATIO_MIN 1.0f
#define OMX_UPWARD_PARAM_RATIO_MAX 10.0f
#define OMX_UPWARD_PARAM_RATIO_DEFAULT 1.0f
#define OMX_UPWARD_PARAM_RANGE_DB_MIN 0.0f
#define OMX_UPWARD_PARAM_RANGE_DB_MAX 24.0f
#define OMX_UPWARD_PARAM_RANGE_DB_DEFAULT 6.0f
#define OMX_UPWARD_PARAM_ATTACK_MS_MIN 0.1f
#define OMX_UPWARD_PARAM_ATTACK_MS_MAX 100.0f
#define OMX_UPWARD_PARAM_ATTACK_MS_DEFAULT 5.0f
#define OMX_UPWARD_PARAM_RELEASE_MS_MIN 5.0f
#define OMX_UPWARD_PARAM_RELEASE_MS_MAX 3000.0f
#define OMX_UPWARD_PARAM_RELEASE_MS_DEFAULT 200.0f

/* The identity every face publishes. */
#define OMX_UPWARD_NAME "omx upward"
#define OMX_UPWARD_VENDOR "openmixer"
#define OMX_UPWARD_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_UPWARD_VERSION "0.19.2"
#define OMX_UPWARD_DESCRIPTION "Lifts what is quiet toward a threshold, by a ratio and up to a limit, and leaves the loud alone."
#define OMX_UPWARD_CLAP_ID "org.openmixer.upward"
#define OMX_UPWARD_CLAP_FEATURES "audio-effect", "compressor", "stereo"
#define OMX_UPWARD_LV2_URI "urn:openmixer:upward"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_UPWARD_DECL_SOURCE "omx upward: Threshold -70 to 0 dB, Ratio 1 to 10, Range 0 to 24 dB, Attack 0.1 to 100 ms, Release 5 to 3000 ms"
#define OMX_UPWARD_DECL_DIGEST "c3af33da420f6e4fa8933bbd1d4cde26bff717ba1eeb11161ca378aafaffe5df"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), is the face's. */
#define OMX_UPWARD_PARAM_CONTRACT_VERSION 2u
#define OMX_UPWARD_PARAM_CONTRACT_FINGERPRINT "2fc982aabf55f20d07941be4636a22c33034de41d19a62ea47d472ae04b9c214"
static const struct omx_param_contract_row OMX_UPWARD_PARAM_CONTRACT[OMX_UPWARD_PARAM_COUNT] = {
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* thresholdDb db */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio ratio */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* rangeDb db */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attackMs ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* releaseMs ms */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_UPWARD_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family none. */
#define OMX_UPWARD_PANEL_FINGERPRINT "67d036aa4eed8fee4fea6b48f88df4433a8c603496bb0f326c55840618497ee7"
static const struct omx_panel_section_row OMX_UPWARD_PANEL_SECTIONS[] = {
  { "upward", "fx.upward.section.upward", "Upward" },
};
static const struct omx_panel_control_row OMX_UPWARD_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.upward.threshold_db", "Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.upward.ratio", "Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.upward.range_db", "Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.upward.attack_ms", "Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.upward.release_ms", "Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_UPWARD_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Upward", "Upward", { 4u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_UPWARD_PANEL_PAGE_COUNT 2u
static const struct omx_panel_rows OMX_UPWARD_PANEL = {
  OMX_UPWARD_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_NONE,
  OMX_UPWARD_PANEL_SECTIONS, 1u,
  OMX_UPWARD_PANEL_CONTROLS, 5u,
  NULL, 0u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_UPWARD_LV2_PORT_ENABLED 0u
#define OMX_UPWARD_LV2_PORT_LATENCY 1u
#define OMX_UPWARD_LV2_PORT_IN_L 2u
#define OMX_UPWARD_LV2_PORT_IN_R 3u
#define OMX_UPWARD_LV2_PORT_OUT_L 4u
#define OMX_UPWARD_LV2_PORT_OUT_R 5u
#define OMX_UPWARD_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_UPWARD_LV2_PORT_FIRST_METER 11u
#define OMX_UPWARD_LV2_PORT_COUNT 11u

#endif /* OMX_UPWARD_PARAMS_H */
