// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_PHASER_PARAMS_H
#define OMX_PHASER_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-phaser/omx-phaser.decl.json.
 * Regenerate: `make -C plugins/omx-phaser gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_phaser_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_PHASER_PARAM_RATE_HZ = 0,
  OMX_PHASER_PARAM_BASE_HZ = 1,
  OMX_PHASER_PARAM_DEPTH_OCT = 2,
  OMX_PHASER_PARAM_STAGES = 3,
  OMX_PHASER_PARAM_FEEDBACK = 4,
  OMX_PHASER_PARAM_MIX = 5,
  OMX_PHASER_PARAM_COUNT = 6
};

static const omx_plugin_param OMX_PHASER_PARAMS[OMX_PHASER_PARAM_COUNT] = {
  { "rateHz", "Rate", "Hz", 0.05f, 5.0f, 0.5f, 0u },
  { "baseHz", "Base", "Hz", 50.0f, 2000.0f, 200.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "depthOct", "Depth", "oct", 0.0f, 6.0f, 4.0f, 0u },
  { "stages", "Stages", "", 2.0f, 12.0f, 6.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "feedback", "Feedback", "", -0.9f, 0.9f, 0.4f, 0u },
  { "mix", "Mix", "%", 0.0f, 100.0f, 50.0f, 0u },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_PHASER_PARAM_MODULES[OMX_PHASER_PARAM_COUNT] = {
  "Phaser",
  "Phaser",
  "Phaser",
  "Phaser",
  "Phaser",
  "Phaser",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_PHASER_PARAM_RATE_HZ_MIN 0.05f
#define OMX_PHASER_PARAM_RATE_HZ_MAX 5.0f
#define OMX_PHASER_PARAM_RATE_HZ_DEFAULT 0.5f
#define OMX_PHASER_PARAM_BASE_HZ_MIN 50.0f
#define OMX_PHASER_PARAM_BASE_HZ_MAX 2000.0f
#define OMX_PHASER_PARAM_BASE_HZ_DEFAULT 200.0f
#define OMX_PHASER_PARAM_DEPTH_OCT_MIN 0.0f
#define OMX_PHASER_PARAM_DEPTH_OCT_MAX 6.0f
#define OMX_PHASER_PARAM_DEPTH_OCT_DEFAULT 4.0f
#define OMX_PHASER_PARAM_STAGES_MIN 2.0f
#define OMX_PHASER_PARAM_STAGES_MAX 12.0f
#define OMX_PHASER_PARAM_STAGES_DEFAULT 6.0f
#define OMX_PHASER_PARAM_FEEDBACK_MIN -0.9f
#define OMX_PHASER_PARAM_FEEDBACK_MAX 0.9f
#define OMX_PHASER_PARAM_FEEDBACK_DEFAULT 0.4f
#define OMX_PHASER_PARAM_MIX_MIN 0.0f
#define OMX_PHASER_PARAM_MIX_MAX 100.0f
#define OMX_PHASER_PARAM_MIX_DEFAULT 50.0f

/* The identity every face publishes. */
#define OMX_PHASER_NAME "omx phaser"
#define OMX_PHASER_VENDOR "openmixer"
#define OMX_PHASER_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_PHASER_VERSION "0.19.2"
#define OMX_PHASER_DESCRIPTION "The console's phaser: identical all-pass sections swept in octaves by one oscillator, with feedback around the chain and a wet/dry mix."
#define OMX_PHASER_CLAP_ID "org.openmixer.phaser"
#define OMX_PHASER_CLAP_FEATURES "audio-effect", "phaser", "stereo"
#define OMX_PHASER_LV2_URI "urn:openmixer:phaser"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_PHASER_DECL_SOURCE "omx phaser: Rate 0.05 to 5 Hz, Base 50 to 2000 Hz, Depth 0 to 6 oct, Stages 2 to 12, Feedback -0.9 to 0.9, Mix 0 to 100 %"
#define OMX_PHASER_DECL_DIGEST "d39a2a6935c7d0e32234d86d28aa84bcb731db7c680d1cd28dd517edff5fe512"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), is the face's. */
#define OMX_PHASER_PARAM_CONTRACT_VERSION 1u
#define OMX_PHASER_PARAM_CONTRACT_FINGERPRINT "ce52de8667a3734d436618dd2a7c57f9a6428f2ae0d80e3ae77159efef05c13e"
static const struct omx_param_contract_row OMX_PHASER_PARAM_CONTRACT[OMX_PHASER_PARAM_COUNT] = {
  { 16u, OMX_CLAP_SCALE_LOG, 0.0 }, /* rateHz hz */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* baseHz hz */
  { 20u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* depthOct oct */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* stages none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* feedback none */
  { 14u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* mix pc */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_PHASER_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family modulation. */
#define OMX_PHASER_PANEL_FINGERPRINT "3a910c99c400fce35b5148f714e5925e0b4a1badf0e24f9fb52534a321309999"
static const struct omx_panel_section_row OMX_PHASER_PANEL_SECTIONS[] = {
  { "phaser", "fx.phaser.section.phaser", "Phaser" },
};
static const struct omx_panel_control_row OMX_PHASER_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.phaser.rate_hz", "Rate", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.phaser.base_hz", "Base", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.phaser.depth_oct", "Depth", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.phaser.stages", "Stages", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.phaser.feedback", "Feedback", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.phaser.mix", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_PHASER_PANEL_ROLES[] = {
  { "rate", 0u, 0u },
  { "depth", 0u, 2u },
  { "feedback", 0u, 4u },
  { "mix", 0u, 5u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_PHASER_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 2u, 4u, 5u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Phaser", "Phaser", { 1u, 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_PHASER_PANEL_PAGE_COUNT 2u
static const struct omx_panel_rows OMX_PHASER_PANEL = {
  OMX_PHASER_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_MODULATION,
  OMX_PHASER_PANEL_SECTIONS, 1u,
  OMX_PHASER_PANEL_CONTROLS, 6u,
  OMX_PHASER_PANEL_ROLES, 4u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_PHASER_LV2_PORT_ENABLED 0u
#define OMX_PHASER_LV2_PORT_LATENCY 1u
#define OMX_PHASER_LV2_PORT_IN_L 2u
#define OMX_PHASER_LV2_PORT_IN_R 3u
#define OMX_PHASER_LV2_PORT_OUT_L 4u
#define OMX_PHASER_LV2_PORT_OUT_R 5u
#define OMX_PHASER_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_PHASER_LV2_PORT_FIRST_METER 12u
#define OMX_PHASER_LV2_PORT_COUNT 12u

#endif /* OMX_PHASER_PARAMS_H */
