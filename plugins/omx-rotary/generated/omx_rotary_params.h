// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_ROTARY_PARAMS_H
#define OMX_ROTARY_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-rotary/omx-rotary.decl.json.
 * Regenerate: `make -C plugins/omx-rotary gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_rotary_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_ROTARY_PARAM_HORN_SLOW_HZ = 0,
  OMX_ROTARY_PARAM_HORN_FAST_HZ = 1,
  OMX_ROTARY_PARAM_DRUM_SLOW_HZ = 2,
  OMX_ROTARY_PARAM_DRUM_FAST_HZ = 3,
  OMX_ROTARY_PARAM_ACCEL = 4,
  OMX_ROTARY_PARAM_BALANCE = 5,
  OMX_ROTARY_PARAM_MIX = 6,
  OMX_ROTARY_PARAM_SPEED = 7,
  OMX_ROTARY_PARAM_COUNT = 8
};

static const omx_plugin_param OMX_ROTARY_PARAMS[OMX_ROTARY_PARAM_COUNT] = {
  { "hornSlowHz", "Horn Slow", "Hz", 0.1f, 2.0f, 0.8f, 0u },
  { "hornFastHz", "Horn Fast", "Hz", 3.0f, 10.0f, 6.8f, 0u },
  { "drumSlowHz", "Drum Slow", "Hz", 0.1f, 2.0f, 0.7f, 0u },
  { "drumFastHz", "Drum Fast", "Hz", 3.0f, 10.0f, 5.9f, 0u },
  { "accel", "Accel", "x", 0.25f, 4.0f, 1.0f, 0u },
  { "balance", "Balance", "%", -100.0f, 100.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "mix", "Mix", "%", 0.0f, 100.0f, 100.0f, 0u },
  { "speed", "Speed", "", 0.0f, 2.0f, 1.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_ROTARY_PARAM_MODULES[OMX_ROTARY_PARAM_COUNT] = {
  "Rotary",
  "Rotary",
  "Rotary",
  "Rotary",
  "Rotary",
  "Rotary",
  "Rotary",
  "Rotary",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_ROTARY_PARAM_HORN_SLOW_HZ_MIN 0.1f
#define OMX_ROTARY_PARAM_HORN_SLOW_HZ_MAX 2.0f
#define OMX_ROTARY_PARAM_HORN_SLOW_HZ_DEFAULT 0.8f
#define OMX_ROTARY_PARAM_HORN_FAST_HZ_MIN 3.0f
#define OMX_ROTARY_PARAM_HORN_FAST_HZ_MAX 10.0f
#define OMX_ROTARY_PARAM_HORN_FAST_HZ_DEFAULT 6.8f
#define OMX_ROTARY_PARAM_DRUM_SLOW_HZ_MIN 0.1f
#define OMX_ROTARY_PARAM_DRUM_SLOW_HZ_MAX 2.0f
#define OMX_ROTARY_PARAM_DRUM_SLOW_HZ_DEFAULT 0.7f
#define OMX_ROTARY_PARAM_DRUM_FAST_HZ_MIN 3.0f
#define OMX_ROTARY_PARAM_DRUM_FAST_HZ_MAX 10.0f
#define OMX_ROTARY_PARAM_DRUM_FAST_HZ_DEFAULT 5.9f
#define OMX_ROTARY_PARAM_ACCEL_MIN 0.25f
#define OMX_ROTARY_PARAM_ACCEL_MAX 4.0f
#define OMX_ROTARY_PARAM_ACCEL_DEFAULT 1.0f
#define OMX_ROTARY_PARAM_BALANCE_MIN -100.0f
#define OMX_ROTARY_PARAM_BALANCE_MAX 100.0f
#define OMX_ROTARY_PARAM_BALANCE_DEFAULT 0.0f
#define OMX_ROTARY_PARAM_MIX_MIN 0.0f
#define OMX_ROTARY_PARAM_MIX_MAX 100.0f
#define OMX_ROTARY_PARAM_MIX_DEFAULT 100.0f
#define OMX_ROTARY_PARAM_SPEED_MIN 0.0f
#define OMX_ROTARY_PARAM_SPEED_MAX 2.0f
#define OMX_ROTARY_PARAM_SPEED_DEFAULT 1.0f

/* The identity every face publishes. */
#define OMX_ROTARY_NAME "omx rotary"
#define OMX_ROTARY_VENDOR "openmixer"
#define OMX_ROTARY_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_ROTARY_VERSION "0.19.3"
#define OMX_ROTARY_DESCRIPTION "The console's rotary speaker: a drum rotor on the low band and a horn rotor on the high band, with stop, slow and fast speeds, mixed with the dry signal."
#define OMX_ROTARY_CLAP_ID "org.openmixer.rotary"
#define OMX_ROTARY_CLAP_FEATURES "audio-effect", "stereo"
#define OMX_ROTARY_LV2_URI "urn:openmixer:rotary"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_ROTARY_DECL_SOURCE "omx rotary: Horn Slow 0.1 to 2 Hz, Horn Fast 3 to 10 Hz, Drum Slow 0.1 to 2 Hz, Drum Fast 3 to 10 Hz, Accel 0.25 to 4 x, Balance -100 to 100 %, Mix 0 to 100 %, Speed 0 to 2"
#define OMX_ROTARY_DECL_DIGEST "0f1ffd01a7af4af40fdfb19f71a56ccec05cd0ed038c21237374b29ea4d347e3"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_ROTARY_PARAM_CONTRACT_VERSION 2u
#define OMX_ROTARY_PARAM_CONTRACT_FINGERPRINT "c8df04231f5071d1830fa6e67ac6e067f46b7bb659db9eafb2a76f8d779846e8"
static const struct omx_param_contract_row OMX_ROTARY_PARAM_CONTRACT[OMX_ROTARY_PARAM_COUNT] = {
  { 16u, OMX_CLAP_SCALE_LOG, 0.0 }, /* hornSlowHz hz */
  { 16u, OMX_CLAP_SCALE_LOG, 0.0 }, /* hornFastHz hz */
  { 16u, OMX_CLAP_SCALE_LOG, 0.0 }, /* drumSlowHz hz */
  { 16u, OMX_CLAP_SCALE_LOG, 0.0 }, /* drumFastHz hz */
  { 15u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* accel coef */
  { 14u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* balance pc */
  { 14u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* mix pc */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* speed none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_ROTARY_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family modulation. */
#define OMX_ROTARY_PANEL_FINGERPRINT "b64b377d922b9cc677876ee7959372502ed855881f59c953cdce3a9d4fa8a9ae"
static const struct omx_panel_section_row OMX_ROTARY_PANEL_SECTIONS[] = {
  { "rotary", "fx.rotary.section.rotary", "Rotary" },
};
static const struct omx_panel_control_row OMX_ROTARY_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.rotary.horn_slow_hz", "Horn Slow", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.rotary.horn_fast_hz", "Horn Fast", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.rotary.drum_slow_hz", "Drum Slow", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.rotary.drum_fast_hz", "Drum Fast", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.rotary.accel", "Accel", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.rotary.balance", "Balance", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.rotary.mix", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.rotary.speed", "Speed", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_ROTARY_PANEL_ROLES[] = {
  { "mix", 0u, 6u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_ROTARY_PANEL_PAGES[] = {
  { "Easy", "Easy", { 6u, 0u, 1u, 2u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Rotary", "Rotary", { 3u, 4u, 5u, 7u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_ROTARY_PANEL_PAGE_COUNT 2u
static const struct omx_panel_rows OMX_ROTARY_PANEL = {
  OMX_ROTARY_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_MODULATION,
  OMX_ROTARY_PANEL_SECTIONS, 1u,
  OMX_ROTARY_PANEL_CONTROLS, 8u,
  OMX_ROTARY_PANEL_ROLES, 1u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_ROTARY_LV2_PORT_ENABLED 0u
#define OMX_ROTARY_LV2_PORT_LATENCY 1u
#define OMX_ROTARY_LV2_PORT_IN_L 2u
#define OMX_ROTARY_LV2_PORT_IN_R 3u
#define OMX_ROTARY_LV2_PORT_OUT_L 4u
#define OMX_ROTARY_LV2_PORT_OUT_R 5u
#define OMX_ROTARY_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_ROTARY_LV2_PORT_FIRST_METER 14u
#define OMX_ROTARY_LV2_PORT_COUNT 14u

#endif /* OMX_ROTARY_PARAMS_H */
