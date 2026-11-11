// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_DUCKER_PARAMS_H
#define OMX_DUCKER_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-ducker/omx-ducker.decl.json.
 * Regenerate: `make -C plugins/omx-ducker gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_ducker_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_DUCKER_PARAM_THRESHOLD_DB = 0,
  OMX_DUCKER_PARAM_RANGE_DB = 1,
  OMX_DUCKER_PARAM_ATTACK_MS = 2,
  OMX_DUCKER_PARAM_HOLD_MS = 3,
  OMX_DUCKER_PARAM_RELEASE_MS = 4,
  OMX_DUCKER_PARAM_RATIO = 5,
  OMX_DUCKER_PARAM_KNEE_DB = 6,
  OMX_DUCKER_PARAM_KEY_HP_HZ = 7,
  OMX_DUCKER_PARAM_KEY_LP_HZ = 8,
  OMX_DUCKER_PARAM_KEY_LISTEN = 9,
  OMX_DUCKER_PARAM_COUNT = 10
};

static const omx_plugin_param OMX_DUCKER_PARAMS[OMX_DUCKER_PARAM_COUNT] = {
  { "thresholdDb", "Threshold", "dB", -60.0f, 0.0f, -18.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "rangeDb", "Depth", "dB", -40.0f, 0.0f, -12.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "attackMs", "Attack", "ms", 0.1f, 100.0f, 5.0f, 0u },
  { "holdMs", "Hold", "ms", 0.0f, 2000.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "releaseMs", "Release", "ms", 5.0f, 3000.0f, 200.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "ratio", "Ratio", "", 1.0f, 20.0f, 4.0f, 0u },
  { "kneeDb", "Knee", "dB", 0.0f, 24.0f, 6.0f, 0u },
  { "keyHpHz", "Key High-Pass", "Hz", 0.0f, 2000.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "keyLpHz", "Key Low-Pass", "Hz", 0.0f, 20000.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "keyListen", "Key Listen", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_DUCKER_PARAM_MODULES[OMX_DUCKER_PARAM_COUNT] = {
  "Duck",
  "Duck",
  "Timing",
  "Timing",
  "Timing",
  "Duck",
  "Duck",
  "Key",
  "Key",
  "Key",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_DUCKER_PARAM_THRESHOLD_DB_MIN -60.0f
#define OMX_DUCKER_PARAM_THRESHOLD_DB_MAX 0.0f
#define OMX_DUCKER_PARAM_THRESHOLD_DB_DEFAULT -18.0f
#define OMX_DUCKER_PARAM_RANGE_DB_MIN -40.0f
#define OMX_DUCKER_PARAM_RANGE_DB_MAX 0.0f
#define OMX_DUCKER_PARAM_RANGE_DB_DEFAULT -12.0f
#define OMX_DUCKER_PARAM_ATTACK_MS_MIN 0.1f
#define OMX_DUCKER_PARAM_ATTACK_MS_MAX 100.0f
#define OMX_DUCKER_PARAM_ATTACK_MS_DEFAULT 5.0f
#define OMX_DUCKER_PARAM_HOLD_MS_MIN 0.0f
#define OMX_DUCKER_PARAM_HOLD_MS_MAX 2000.0f
#define OMX_DUCKER_PARAM_HOLD_MS_DEFAULT 0.0f
#define OMX_DUCKER_PARAM_RELEASE_MS_MIN 5.0f
#define OMX_DUCKER_PARAM_RELEASE_MS_MAX 3000.0f
#define OMX_DUCKER_PARAM_RELEASE_MS_DEFAULT 200.0f
#define OMX_DUCKER_PARAM_RATIO_MIN 1.0f
#define OMX_DUCKER_PARAM_RATIO_MAX 20.0f
#define OMX_DUCKER_PARAM_RATIO_DEFAULT 4.0f
#define OMX_DUCKER_PARAM_KNEE_DB_MIN 0.0f
#define OMX_DUCKER_PARAM_KNEE_DB_MAX 24.0f
#define OMX_DUCKER_PARAM_KNEE_DB_DEFAULT 6.0f
#define OMX_DUCKER_PARAM_KEY_HP_HZ_MIN 0.0f
#define OMX_DUCKER_PARAM_KEY_HP_HZ_MAX 2000.0f
#define OMX_DUCKER_PARAM_KEY_HP_HZ_DEFAULT 0.0f
#define OMX_DUCKER_PARAM_KEY_LP_HZ_MIN 0.0f
#define OMX_DUCKER_PARAM_KEY_LP_HZ_MAX 20000.0f
#define OMX_DUCKER_PARAM_KEY_LP_HZ_DEFAULT 0.0f
#define OMX_DUCKER_PARAM_KEY_LISTEN_MIN 0.0f
#define OMX_DUCKER_PARAM_KEY_LISTEN_MAX 1.0f
#define OMX_DUCKER_PARAM_KEY_LISTEN_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_DUCKER_NAME "omx ducker"
#define OMX_DUCKER_VENDOR "openmixer"
#define OMX_DUCKER_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_DUCKER_VERSION "0.19.3"
#define OMX_DUCKER_DESCRIPTION "Pulls the programme down while a key is heard, music under a voice, by a set depth with attack, hold and release, the console ducker."
#define OMX_DUCKER_CLAP_ID "org.openmixer.ducker"
#define OMX_DUCKER_CLAP_FEATURES "audio-effect", "compressor", "stereo"
#define OMX_DUCKER_LV2_URI "urn:openmixer:ducker"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_DUCKER_DECL_SOURCE "omx ducker: Threshold -60 to 0 dB, Depth -40 to 0 dB, Attack 0.1 to 100 ms, Hold 0 to 2000 ms, Release 5 to 3000 ms, Ratio 1 to 20, Knee 0 to 24 dB, Key High-Pass 0 to 2000 Hz, Key Low-Pass 0 to 20000 Hz, Key Listen 0 to 1"
#define OMX_DUCKER_DECL_DIGEST "cd2a96de102d23b5c4293c47764f817f2b2e659e7a8a88a01c538731a6e439ed"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_DUCKER_PARAM_CONTRACT_VERSION 3u
#define OMX_DUCKER_PARAM_CONTRACT_FINGERPRINT "26cb02750c18a914890aa7b6b007905fa2cc563d1ca4a8ad1e6006f05f873864"
static const struct omx_param_contract_row OMX_DUCKER_PARAM_CONTRACT[OMX_DUCKER_PARAM_COUNT] = {
  { 13u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* thresholdDb db */
  { 13u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* rangeDb db */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attackMs ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* holdMs ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* releaseMs ms */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio ratio */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* kneeDb db */
  { 16u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* keyHpHz hz */
  { 16u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* keyLpHz hz */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* keyListen none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_DUCKER_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family dynamics. */
#define OMX_DUCKER_PANEL_FINGERPRINT "ff49ae5eef7e5e9dc9c18a231ad4e911ccede5f6088efbda41a7afe2905a3a12"
static const struct omx_panel_section_row OMX_DUCKER_PANEL_SECTIONS[] = {
  { "duck", "fx.comp.section.duck", "Duck" },
  { "timing", "fx.comp.section.timing", "Timing" },
  { "key", "fx.comp.section.key", "Key" },
};
static const struct omx_panel_control_row OMX_DUCKER_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.comp.threshold_db", "Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.comp.range_db", "Depth", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.comp.ratio", "Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.comp.knee_db", "Knee", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.comp.attack_ms", "Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.comp.hold_ms", "Hold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.comp.release_ms", "Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.comp.key_hp_hz", "Key High-Pass", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 8u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.comp.key_lp_hz", "Key Low-Pass", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 9u, 2u, OMX_CLAP_WIDGET_SWITCH, "fx.comp.key_listen", "Key Listen", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_DUCKER_PANEL_ROLES[] = {
  { "threshold", 0u, 0u },
  { "attack", 0u, 2u },
  { "release", 0u, 4u },
  { "ratio", 0u, 5u },
  { "knee", 0u, 6u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_DUCKER_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 5u, 2u, 4u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Duck", "Duck", { 1u, 6u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Timing", "Timing", { 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Key", "Key", { 7u, 8u, 9u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_DUCKER_PANEL_PAGE_COUNT 4u
static const struct omx_panel_rows OMX_DUCKER_PANEL = {
  OMX_DUCKER_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_DYNAMICS,
  OMX_DUCKER_PANEL_SECTIONS, 3u,
  OMX_DUCKER_PANEL_CONTROLS, 10u,
  OMX_DUCKER_PANEL_ROLES, 5u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_DUCKER_LV2_PORT_ENABLED 0u
#define OMX_DUCKER_LV2_PORT_LATENCY 1u
#define OMX_DUCKER_LV2_PORT_IN_L 2u
#define OMX_DUCKER_LV2_PORT_IN_R 3u
#define OMX_DUCKER_LV2_PORT_KEY 4u
#define OMX_DUCKER_LV2_PORT_OUT_L 5u
#define OMX_DUCKER_LV2_PORT_OUT_R 6u
#define OMX_DUCKER_LV2_PORT_FIRST_PARAM 7u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_DUCKER_LV2_PORT_FIRST_METER 17u
#define OMX_DUCKER_LV2_PORT_COUNT 17u

#endif /* OMX_DUCKER_PARAMS_H */
