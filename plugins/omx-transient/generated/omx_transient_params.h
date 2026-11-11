// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_TRANSIENT_PARAMS_H
#define OMX_TRANSIENT_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-transient/omx-transient.decl.json.
 * Regenerate: `make -C plugins/omx-transient gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_transient_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_TRANSIENT_PARAM_ATTACK_DB = 0,
  OMX_TRANSIENT_PARAM_SUSTAIN_DB = 1,
  OMX_TRANSIENT_PARAM_ATTACK_TIME_MS = 2,
  OMX_TRANSIENT_PARAM_SUSTAIN_TIME_MS = 3,
  OMX_TRANSIENT_PARAM_OUTPUT_DB = 4,
  OMX_TRANSIENT_PARAM_COUNT = 5
};

static const omx_plugin_param OMX_TRANSIENT_PARAMS[OMX_TRANSIENT_PARAM_COUNT] = {
  { "attackDb", "Attack", "dB", -24.0f, 24.0f, 0.0f, 0u },
  { "sustainDb", "Sustain", "dB", -24.0f, 24.0f, 0.0f, 0u },
  { "attackTimeMs", "Attack Time", "ms", 2.0f, 50.0f, 10.0f, 0u },
  { "sustainTimeMs", "Sustain Time", "ms", 50.0f, 2000.0f, 250.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "outputDb", "Output", "dB", -24.0f, 12.0f, 0.0f, 0u },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_TRANSIENT_PARAM_MODULES[OMX_TRANSIENT_PARAM_COUNT] = {
  "Transient",
  "Transient",
  "Transient",
  "Transient",
  "Transient",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_TRANSIENT_PARAM_ATTACK_DB_MIN -24.0f
#define OMX_TRANSIENT_PARAM_ATTACK_DB_MAX 24.0f
#define OMX_TRANSIENT_PARAM_ATTACK_DB_DEFAULT 0.0f
#define OMX_TRANSIENT_PARAM_SUSTAIN_DB_MIN -24.0f
#define OMX_TRANSIENT_PARAM_SUSTAIN_DB_MAX 24.0f
#define OMX_TRANSIENT_PARAM_SUSTAIN_DB_DEFAULT 0.0f
#define OMX_TRANSIENT_PARAM_ATTACK_TIME_MS_MIN 2.0f
#define OMX_TRANSIENT_PARAM_ATTACK_TIME_MS_MAX 50.0f
#define OMX_TRANSIENT_PARAM_ATTACK_TIME_MS_DEFAULT 10.0f
#define OMX_TRANSIENT_PARAM_SUSTAIN_TIME_MS_MIN 50.0f
#define OMX_TRANSIENT_PARAM_SUSTAIN_TIME_MS_MAX 2000.0f
#define OMX_TRANSIENT_PARAM_SUSTAIN_TIME_MS_DEFAULT 250.0f
#define OMX_TRANSIENT_PARAM_OUTPUT_DB_MIN -24.0f
#define OMX_TRANSIENT_PARAM_OUTPUT_DB_MAX 12.0f
#define OMX_TRANSIENT_PARAM_OUTPUT_DB_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_TRANSIENT_NAME "omx transient"
#define OMX_TRANSIENT_VENDOR "openmixer"
#define OMX_TRANSIENT_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_TRANSIENT_VERSION "0.19.3"
#define OMX_TRANSIENT_DESCRIPTION "The console's transient designer: more or less attack and sustain, from two envelope contrasts, with no threshold to set."
#define OMX_TRANSIENT_CLAP_ID "org.openmixer.transient"
#define OMX_TRANSIENT_CLAP_FEATURES "audio-effect", "transient-shaper", "stereo"
#define OMX_TRANSIENT_LV2_URI "urn:openmixer:transient"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_TRANSIENT_DECL_SOURCE "omx transient: Attack -24 to 24 dB, Sustain -24 to 24 dB, Attack Time 2 to 50 ms, Sustain Time 50 to 2000 ms, Output -24 to 12 dB"
#define OMX_TRANSIENT_DECL_DIGEST "b735a9f45b45b01b0ef5660e8ac5e43729aaab9fd5b9c7cce4976934c82759a5"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_TRANSIENT_PARAM_CONTRACT_VERSION 2u
#define OMX_TRANSIENT_PARAM_CONTRACT_FINGERPRINT "110cf0365d7f60b97a18b53213f8bfd9042bbe9110b4f60c490cca6ad8cb16b0"
static const struct omx_param_contract_row OMX_TRANSIENT_PARAM_CONTRACT[OMX_TRANSIENT_PARAM_COUNT] = {
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attackDb db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* sustainDb db */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attackTimeMs ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* sustainTimeMs ms */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* outputDb db */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_TRANSIENT_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family none. */
#define OMX_TRANSIENT_PANEL_FINGERPRINT "628f9caf28b29c12b3c738305685d5d19caa64b1582d097fb87efc2a3ef1cecf"
static const struct omx_panel_section_row OMX_TRANSIENT_PANEL_SECTIONS[] = {
  { "transient", "fx.transient.section.transient", "Transient" },
};
static const struct omx_panel_control_row OMX_TRANSIENT_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.transient.attack_db", "Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.transient.sustain_db", "Sustain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.transient.attack_time_ms", "Attack Time", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.transient.sustain_time_ms", "Sustain Time", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.transient.output_db", "Output", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_TRANSIENT_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Transient", "Transient", { 4u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_TRANSIENT_PANEL_PAGE_COUNT 2u
static const struct omx_panel_rows OMX_TRANSIENT_PANEL = {
  OMX_TRANSIENT_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_NONE,
  OMX_TRANSIENT_PANEL_SECTIONS, 1u,
  OMX_TRANSIENT_PANEL_CONTROLS, 5u,
  NULL, 0u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_TRANSIENT_LV2_PORT_ENABLED 0u
#define OMX_TRANSIENT_LV2_PORT_LATENCY 1u
#define OMX_TRANSIENT_LV2_PORT_IN_L 2u
#define OMX_TRANSIENT_LV2_PORT_IN_R 3u
#define OMX_TRANSIENT_LV2_PORT_OUT_L 4u
#define OMX_TRANSIENT_LV2_PORT_OUT_R 5u
#define OMX_TRANSIENT_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_TRANSIENT_LV2_PORT_FIRST_METER 11u
#define OMX_TRANSIENT_LV2_PORT_COUNT 11u

#endif /* OMX_TRANSIENT_PARAMS_H */
