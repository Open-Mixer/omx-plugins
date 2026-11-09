// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_DELAY_PARAMS_H
#define OMX_DELAY_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-delay/omx-delay.decl.json.
 * Regenerate: `make -C plugins/omx-delay gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_delay_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_DELAY_PARAM_FX_DELAY_TIME = 0,
  OMX_DELAY_PARAM_FX_DELAY_FEEDBACK = 1,
  OMX_DELAY_PARAM_TONE = 2,
  OMX_DELAY_PARAM_MIX = 3,
  OMX_DELAY_PARAM_PINGPONG = 4,
  OMX_DELAY_PARAM_FX_DELAY_TAPS = 5,
  OMX_DELAY_PARAM_TAP0_GAIN = 6,
  OMX_DELAY_PARAM_TAP0_PAN = 7,
  OMX_DELAY_PARAM_TAP1_FACTOR = 8,
  OMX_DELAY_PARAM_TAP1_GAIN = 9,
  OMX_DELAY_PARAM_TAP1_PAN = 10,
  OMX_DELAY_PARAM_TAP2_FACTOR = 11,
  OMX_DELAY_PARAM_TAP2_GAIN = 12,
  OMX_DELAY_PARAM_TAP2_PAN = 13,
  OMX_DELAY_PARAM_TAP3_FACTOR = 14,
  OMX_DELAY_PARAM_TAP3_GAIN = 15,
  OMX_DELAY_PARAM_TAP3_PAN = 16,
  OMX_DELAY_PARAM_SYNC = 17,
  OMX_DELAY_PARAM_DIVISION = 18,
  OMX_DELAY_PARAM_MODE = 19,
  OMX_DELAY_PARAM_DUCK_DB = 20,
  OMX_DELAY_PARAM_DUCK_THRESHOLD_DB = 21,
  OMX_DELAY_PARAM_DUCK_RELEASE_MS = 22,
  OMX_DELAY_PARAM_COUNT = 23
};

static const omx_plugin_param OMX_DELAY_PARAMS[OMX_DELAY_PARAM_COUNT] = {
  { "fxDelayTime", "Time", "ms", 0.0f, 2000.0f, 300.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "fxDelayFeedback", "Feedback", "", 0.0f, 0.99f, 0.3f, 0u },
  { "tone", "Tone", "", 0.0f, 1.0f, 0.3f, 0u },
  { "mix", "Mix", "", 0.0f, 1.0f, 0.3f, 0u },
  { "pingpong", "Pingpong", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "fxDelayTaps", "Taps", "", 1.0f, 4.0f, 1.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "tap0Gain", "Tap 1 Gain", "", 0.0f, 1.0f, 1.0f, 0u },
  { "tap0Pan", "Tap 1 Pan", "", -1.0f, 1.0f, 0.0f, 0u },
  { "tap1Factor", "Tap 2 Time", "", 0.0f, 10.0f, 3.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "tap1Gain", "Tap 2 Gain", "", 0.0f, 1.0f, 0.7f, 0u },
  { "tap1Pan", "Tap 2 Pan", "", -1.0f, 1.0f, -0.5f, 0u },
  { "tap2Factor", "Tap 3 Time", "", 0.0f, 10.0f, 5.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "tap2Gain", "Tap 3 Gain", "", 0.0f, 1.0f, 0.5f, 0u },
  { "tap2Pan", "Tap 3 Pan", "", -1.0f, 1.0f, 0.5f, 0u },
  { "tap3Factor", "Tap 4 Time", "", 0.0f, 10.0f, 8.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "tap3Gain", "Tap 4 Gain", "", 0.0f, 1.0f, 0.35f, 0u },
  { "tap3Pan", "Tap 4 Pan", "", -1.0f, 1.0f, 0.0f, 0u },
  { "sync", "Sync", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "division", "Division", "", 0.0f, 17.0f, 6.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "mode", "Mode", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "duckDb", "Duck", "dB", -24.0f, 0.0f, 0.0f, 0u },
  { "duckThresholdDb", "Duck Threshold", "dB", -60.0f, 0.0f, -30.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "duckReleaseMs", "Duck Release", "ms", 20.0f, 2000.0f, 250.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_DELAY_PARAM_MODULES[OMX_DELAY_PARAM_COUNT] = {
  "Delay",
  "Delay",
  "Delay",
  "Delay",
  "Delay",
  "Taps",
  "Taps",
  "Taps",
  "Taps",
  "Taps",
  "Taps",
  "Taps",
  "Taps",
  "Taps",
  "Taps",
  "Taps",
  "Taps",
  "Tempo",
  "Tempo",
  "Delay",
  "Duck",
  "Duck",
  "Duck",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_DELAY_PARAM_FX_DELAY_TIME_MIN 0.0f
#define OMX_DELAY_PARAM_FX_DELAY_TIME_MAX 2000.0f
#define OMX_DELAY_PARAM_FX_DELAY_TIME_DEFAULT 300.0f
#define OMX_DELAY_PARAM_FX_DELAY_FEEDBACK_MIN 0.0f
#define OMX_DELAY_PARAM_FX_DELAY_FEEDBACK_MAX 0.99f
#define OMX_DELAY_PARAM_FX_DELAY_FEEDBACK_DEFAULT 0.3f
#define OMX_DELAY_PARAM_TONE_MIN 0.0f
#define OMX_DELAY_PARAM_TONE_MAX 1.0f
#define OMX_DELAY_PARAM_TONE_DEFAULT 0.3f
#define OMX_DELAY_PARAM_MIX_MIN 0.0f
#define OMX_DELAY_PARAM_MIX_MAX 1.0f
#define OMX_DELAY_PARAM_MIX_DEFAULT 0.3f
#define OMX_DELAY_PARAM_PINGPONG_MIN 0.0f
#define OMX_DELAY_PARAM_PINGPONG_MAX 1.0f
#define OMX_DELAY_PARAM_PINGPONG_DEFAULT 0.0f
#define OMX_DELAY_PARAM_FX_DELAY_TAPS_MIN 1.0f
#define OMX_DELAY_PARAM_FX_DELAY_TAPS_MAX 4.0f
#define OMX_DELAY_PARAM_FX_DELAY_TAPS_DEFAULT 1.0f
#define OMX_DELAY_PARAM_TAP0_GAIN_MIN 0.0f
#define OMX_DELAY_PARAM_TAP0_GAIN_MAX 1.0f
#define OMX_DELAY_PARAM_TAP0_GAIN_DEFAULT 1.0f
#define OMX_DELAY_PARAM_TAP0_PAN_MIN -1.0f
#define OMX_DELAY_PARAM_TAP0_PAN_MAX 1.0f
#define OMX_DELAY_PARAM_TAP0_PAN_DEFAULT 0.0f
#define OMX_DELAY_PARAM_TAP1_FACTOR_MIN 0.0f
#define OMX_DELAY_PARAM_TAP1_FACTOR_MAX 10.0f
#define OMX_DELAY_PARAM_TAP1_FACTOR_DEFAULT 3.0f
#define OMX_DELAY_PARAM_TAP1_GAIN_MIN 0.0f
#define OMX_DELAY_PARAM_TAP1_GAIN_MAX 1.0f
#define OMX_DELAY_PARAM_TAP1_GAIN_DEFAULT 0.7f
#define OMX_DELAY_PARAM_TAP1_PAN_MIN -1.0f
#define OMX_DELAY_PARAM_TAP1_PAN_MAX 1.0f
#define OMX_DELAY_PARAM_TAP1_PAN_DEFAULT -0.5f
#define OMX_DELAY_PARAM_TAP2_FACTOR_MIN 0.0f
#define OMX_DELAY_PARAM_TAP2_FACTOR_MAX 10.0f
#define OMX_DELAY_PARAM_TAP2_FACTOR_DEFAULT 5.0f
#define OMX_DELAY_PARAM_TAP2_GAIN_MIN 0.0f
#define OMX_DELAY_PARAM_TAP2_GAIN_MAX 1.0f
#define OMX_DELAY_PARAM_TAP2_GAIN_DEFAULT 0.5f
#define OMX_DELAY_PARAM_TAP2_PAN_MIN -1.0f
#define OMX_DELAY_PARAM_TAP2_PAN_MAX 1.0f
#define OMX_DELAY_PARAM_TAP2_PAN_DEFAULT 0.5f
#define OMX_DELAY_PARAM_TAP3_FACTOR_MIN 0.0f
#define OMX_DELAY_PARAM_TAP3_FACTOR_MAX 10.0f
#define OMX_DELAY_PARAM_TAP3_FACTOR_DEFAULT 8.0f
#define OMX_DELAY_PARAM_TAP3_GAIN_MIN 0.0f
#define OMX_DELAY_PARAM_TAP3_GAIN_MAX 1.0f
#define OMX_DELAY_PARAM_TAP3_GAIN_DEFAULT 0.35f
#define OMX_DELAY_PARAM_TAP3_PAN_MIN -1.0f
#define OMX_DELAY_PARAM_TAP3_PAN_MAX 1.0f
#define OMX_DELAY_PARAM_TAP3_PAN_DEFAULT 0.0f
#define OMX_DELAY_PARAM_SYNC_MIN 0.0f
#define OMX_DELAY_PARAM_SYNC_MAX 1.0f
#define OMX_DELAY_PARAM_SYNC_DEFAULT 0.0f
#define OMX_DELAY_PARAM_DIVISION_MIN 0.0f
#define OMX_DELAY_PARAM_DIVISION_MAX 17.0f
#define OMX_DELAY_PARAM_DIVISION_DEFAULT 6.0f
#define OMX_DELAY_PARAM_MODE_MIN 0.0f
#define OMX_DELAY_PARAM_MODE_MAX 2.0f
#define OMX_DELAY_PARAM_MODE_DEFAULT 0.0f
#define OMX_DELAY_PARAM_DUCK_DB_MIN -24.0f
#define OMX_DELAY_PARAM_DUCK_DB_MAX 0.0f
#define OMX_DELAY_PARAM_DUCK_DB_DEFAULT 0.0f
#define OMX_DELAY_PARAM_DUCK_THRESHOLD_DB_MIN -60.0f
#define OMX_DELAY_PARAM_DUCK_THRESHOLD_DB_MAX 0.0f
#define OMX_DELAY_PARAM_DUCK_THRESHOLD_DB_DEFAULT -30.0f
#define OMX_DELAY_PARAM_DUCK_RELEASE_MS_MIN 20.0f
#define OMX_DELAY_PARAM_DUCK_RELEASE_MS_MAX 2000.0f
#define OMX_DELAY_PARAM_DUCK_RELEASE_MS_DEFAULT 250.0f

/* The identity every face publishes. */
#define OMX_DELAY_NAME "omx delay"
#define OMX_DELAY_VENDOR "openmixer"
#define OMX_DELAY_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_DELAY_VERSION "0.19.2"
#define OMX_DELAY_DESCRIPTION "The OpenMixer console's stereo delay: one time, free or synced to the host's tempo, feedback through a tone filter, ping-pong, read taps, a tape or analog colour, a wet/dry mix and a duck under the input."
#define OMX_DELAY_CLAP_ID "org.openmixer.delay"
#define OMX_DELAY_CLAP_FEATURES "audio-effect", "delay", "stereo"
#define OMX_DELAY_LV2_URI "urn:openmixer:delay"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_DELAY_DECL_SOURCE "omx delay: Time 0 to 2000 ms, Feedback 0 to 0.99, Tone 0 to 1, Mix 0 to 1, Pingpong 0 to 1, Taps 1 to 4, Tap 1 Gain 0 to 1, Tap 1 Pan -1 to 1, Tap 2 Time 0 to 10, Tap 2 Gain 0 to 1, Tap 2 Pan -1 to 1, Tap 3 Time 0 to 10, Tap 3 Gain 0 to 1, Tap 3 Pan -1 to 1, Tap 4 Time 0 to 10, Tap 4 Gain 0 to 1, Tap 4 Pan -1 to 1, Sync 0 to 1, Division 0 to 17, Mode 0 to 2, Duck -24 to 0 dB, Duck Threshold -60 to 0 dB, Duck Release 20 to 2000 ms"
#define OMX_DELAY_DECL_DIGEST "3185a8fe2345bf9f9b48598b83c02556320301a873ac36ea4c18236ee59829e3"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), is the face's. */
#define OMX_DELAY_PARAM_CONTRACT_VERSION 1u
#define OMX_DELAY_PARAM_CONTRACT_FINGERPRINT "0fb8078f57eefce8cebfd48112c29e53c7e1648dcd485bad6ee881e645744c74"
static const struct omx_param_contract_row OMX_DELAY_PARAM_CONTRACT[OMX_DELAY_PARAM_COUNT] = {
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* fxDelayTime ms */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* fxDelayFeedback none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* tone none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* mix none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* pingpong none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* fxDelayTaps none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* tap0Gain none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* tap0Pan none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* tap1Factor none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* tap1Gain none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* tap1Pan none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* tap2Factor none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* tap2Gain none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* tap2Pan none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* tap3Factor none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* tap3Gain none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* tap3Pan none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* sync none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* division none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* mode none */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* duckDb db */
  { 13u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* duckThresholdDb db */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* duckReleaseMs ms */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_DELAY_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family delay. */
#define OMX_DELAY_PANEL_FINGERPRINT "a801adf0482b982181e8648ec892281e38f8ed2f7f8a17a071f216e6346d2cae"
static const struct omx_panel_section_row OMX_DELAY_PANEL_SECTIONS[] = {
  { "delay", "fx.delay.section.delay", "Delay" },
  { "tempo", "fx.delay.section.tempo", "Tempo" },
  { "taps", "fx.delay.section.taps", "Taps" },
  { "duck", "fx.delay.section.duck", "Duck" },
};
static const struct omx_panel_control_row OMX_DELAY_PANEL_CONTROLS[] = {
  { 4u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.delay.pingpong", "Pingpong", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.delay.fx_delay_time", "Time", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.delay.fx_delay_feedback", "Feedback", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.delay.mix", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.delay.tone", "Tone", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 19u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.delay.mode", "Mode", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 17u, 1u, OMX_CLAP_WIDGET_AUTO, "fx.delay.sync", "Sync", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 18u, 1u, OMX_CLAP_WIDGET_AUTO, "fx.delay.division", "Division", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.delay.fx_delay_taps", "Taps", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.delay.tap0_gain", "Tap 1 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.delay.tap0_pan", "Tap 1 Pan", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 8u, 2u, OMX_CLAP_WIDGET_AUTO, "fx.delay.tap1_factor", "Tap 2 Time", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 9u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.delay.tap1_gain", "Tap 2 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 10u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.delay.tap1_pan", "Tap 2 Pan", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 11u, 2u, OMX_CLAP_WIDGET_AUTO, "fx.delay.tap2_factor", "Tap 3 Time", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 12u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.delay.tap2_gain", "Tap 3 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 13u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.delay.tap2_pan", "Tap 3 Pan", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 14u, 2u, OMX_CLAP_WIDGET_AUTO, "fx.delay.tap3_factor", "Tap 4 Time", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 15u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.delay.tap3_gain", "Tap 4 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 16u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.delay.tap3_pan", "Tap 4 Pan", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 20u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.delay.duck_db", "Duck", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 21u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.delay.duck_threshold_db", "Duck Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 22u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.delay.duck_release_ms", "Duck Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_DELAY_PANEL_ROLES[] = {
  { "time", 0u, 0u },
  { "feedback", 0u, 1u },
  { "tone", 0u, 2u },
  { "pingpong", 0u, 4u },
  { "mix", 0u, 3u },
};
static const struct omx_panel_default_row OMX_DELAY_PANEL_DEFAULTS[] = {
  { OMX_CLAP_PLACEMENT_RETURN, 3u, 1 },
};
/* clap.remote-controls/2: the pages of the page rule over the sections. */
static const struct omx_panel_page_row OMX_DELAY_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Delay", "Delay", { 4u, 19u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Tempo", "Tempo", { 17u, 18u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Taps", "Taps: Taps - Tap 3 Gain", { 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u } },
  { "Taps", "Taps: Tap 3 Pan - Tap 4 Pan", { 13u, 14u, 15u, 16u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Duck", "Duck", { 20u, 21u, 22u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_DELAY_PANEL_PAGE_COUNT 6u
static const struct omx_panel_rows OMX_DELAY_PANEL = {
  OMX_DELAY_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_DELAY,
  OMX_DELAY_PANEL_SECTIONS, 4u,
  OMX_DELAY_PANEL_CONTROLS, 23u,
  OMX_DELAY_PANEL_ROLES, 5u,
  NULL, 0u,
  OMX_DELAY_PANEL_DEFAULTS, 1u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_DELAY_LV2_PORT_ENABLED 0u
#define OMX_DELAY_LV2_PORT_LATENCY 1u
#define OMX_DELAY_LV2_PORT_CONTROL 2u
#define OMX_DELAY_LV2_PORT_IN_L 3u
#define OMX_DELAY_LV2_PORT_IN_R 4u
#define OMX_DELAY_LV2_PORT_OUT_L 5u
#define OMX_DELAY_LV2_PORT_OUT_R 6u
#define OMX_DELAY_LV2_PORT_FIRST_PARAM 7u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_DELAY_LV2_PORT_FIRST_METER 30u
#define OMX_DELAY_LV2_PORT_COUNT 30u

#endif /* OMX_DELAY_PARAMS_H */
