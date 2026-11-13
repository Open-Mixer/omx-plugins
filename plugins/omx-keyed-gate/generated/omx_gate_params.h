// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_GATE_PARAMS_H
#define OMX_GATE_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-keyed-gate/omx-keyed-gate.decl.json.
 * Regenerate: `make -C plugins/omx-keyed-gate gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_gate_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_GATE_PARAM_KEY_EXTERNAL = 0,
  OMX_GATE_PARAM_THRESHOLD = 1,
  OMX_GATE_PARAM_RATIO = 2,
  OMX_GATE_PARAM_RANGE = 3,
  OMX_GATE_PARAM_ATTACK = 4,
  OMX_GATE_PARAM_RELEASE = 5,
  OMX_GATE_PARAM_KNEE_START = 6,
  OMX_GATE_PARAM_KNEE_END = 7,
  OMX_GATE_PARAM_HOLD = 8,
  OMX_GATE_PARAM_HYSTERESIS = 9,
  OMX_GATE_PARAM_KEY_HP = 10,
  OMX_GATE_PARAM_KEY_LP = 11,
  OMX_GATE_PARAM_COUNT = 12
};

static const omx_plugin_param OMX_GATE_PARAMS[OMX_GATE_PARAM_COUNT] = {
  { "key_external", "Key", "", 0.0f, 1.0f, 1.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "threshold", "Threshold", "dB", -80.0f, 0.0f, -40.0f, 0u },
  { "ratio", "Ratio", "", 1.0f, 100.0f, 16.0f, 0u },
  { "range", "Range", "dB", -90.0f, 0.0f, -90.0f, 0u },
  { "attack", "Attack", "ms", 0.0f, 500.0f, 1.0f, 0u },
  { "release", "Release", "ms", 0.0f, 5000.0f, 100.0f, 0u },
  { "knee_start", "Knee Start", "dB", -80.0f, 0.0f, -43.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "knee_end", "Knee End", "dB", -80.0f, 0.0f, -37.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "hold", "Hold", "ms", 0.0f, 2000.0f, 10.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "hysteresis", "Hysteresis", "dB", 0.0f, 24.0f, 3.0f, 0u },
  { "key_hp", "Key High-pass", "Hz", 0.0f, 20000.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "key_lp", "Key Low-pass", "Hz", 0.0f, 20000.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_GATE_PARAM_MODULES[OMX_GATE_PARAM_COUNT] = {
  "Key",
  "Gate",
  "Gate",
  "Gate",
  "Gate",
  "Gate",
  "Gate",
  "Gate",
  "Gate",
  "Gate",
  "Key",
  "Key",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_GATE_PARAM_KEY_EXTERNAL_MIN 0.0f
#define OMX_GATE_PARAM_KEY_EXTERNAL_MAX 1.0f
#define OMX_GATE_PARAM_KEY_EXTERNAL_DEFAULT 1.0f
#define OMX_GATE_PARAM_THRESHOLD_MIN -80.0f
#define OMX_GATE_PARAM_THRESHOLD_MAX 0.0f
#define OMX_GATE_PARAM_THRESHOLD_DEFAULT -40.0f
#define OMX_GATE_PARAM_RATIO_MIN 1.0f
#define OMX_GATE_PARAM_RATIO_MAX 100.0f
#define OMX_GATE_PARAM_RATIO_DEFAULT 16.0f
#define OMX_GATE_PARAM_RANGE_MIN -90.0f
#define OMX_GATE_PARAM_RANGE_MAX 0.0f
#define OMX_GATE_PARAM_RANGE_DEFAULT -90.0f
#define OMX_GATE_PARAM_ATTACK_MIN 0.0f
#define OMX_GATE_PARAM_ATTACK_MAX 500.0f
#define OMX_GATE_PARAM_ATTACK_DEFAULT 1.0f
#define OMX_GATE_PARAM_RELEASE_MIN 0.0f
#define OMX_GATE_PARAM_RELEASE_MAX 5000.0f
#define OMX_GATE_PARAM_RELEASE_DEFAULT 100.0f
#define OMX_GATE_PARAM_KNEE_START_MIN -80.0f
#define OMX_GATE_PARAM_KNEE_START_MAX 0.0f
#define OMX_GATE_PARAM_KNEE_START_DEFAULT -43.0f
#define OMX_GATE_PARAM_KNEE_END_MIN -80.0f
#define OMX_GATE_PARAM_KNEE_END_MAX 0.0f
#define OMX_GATE_PARAM_KNEE_END_DEFAULT -37.0f
#define OMX_GATE_PARAM_HOLD_MIN 0.0f
#define OMX_GATE_PARAM_HOLD_MAX 2000.0f
#define OMX_GATE_PARAM_HOLD_DEFAULT 10.0f
#define OMX_GATE_PARAM_HYSTERESIS_MIN 0.0f
#define OMX_GATE_PARAM_HYSTERESIS_MAX 24.0f
#define OMX_GATE_PARAM_HYSTERESIS_DEFAULT 3.0f
#define OMX_GATE_PARAM_KEY_HP_MIN 0.0f
#define OMX_GATE_PARAM_KEY_HP_MAX 20000.0f
#define OMX_GATE_PARAM_KEY_HP_DEFAULT 0.0f
#define OMX_GATE_PARAM_KEY_LP_MIN 0.0f
#define OMX_GATE_PARAM_KEY_LP_MAX 20000.0f
#define OMX_GATE_PARAM_KEY_LP_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_GATE_NAME "omx keyed-gate"
#define OMX_GATE_VENDOR "openmixer"
#define OMX_GATE_URL "https://github.com/Open-Mixer/omx-plugins"
#define OMX_GATE_VERSION "0.19.3"
#define OMX_GATE_DESCRIPTION "The OpenMixer console's channel gate with an external key: the stereo signal is gated while the detector listens to the sidechain input the host routes to it, or to the signal itself with the key unconnected or set to Self. One gain drives both legs. An attack under 0.5 ms engages 4x detector oversampling, which adds 72 frames of latency, reported to the host. The DSP is omx-dsp's <omxdsp/fx/omx_gate_instance.h>, the console's own gate."
#define OMX_GATE_CLAP_ID "org.openmixer.keyed-gate"
#define OMX_GATE_CLAP_FEATURES "audio-effect", "gate", "stereo"
#define OMX_GATE_LV2_URI "urn:openmixer:keyed-gate"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_GATE_DECL_SOURCE "omx keyed-gate: Key 0 to 1, Threshold -80 to 0 dB, Ratio 1 to 100, Range -90 to 0 dB, Attack 0 to 500 ms, Release 0 to 5000 ms, Knee Start -80 to 0 dB, Knee End -80 to 0 dB, Hold 0 to 2000 ms, Hysteresis 0 to 24 dB, Key High-pass 0 to 20000 Hz, Key Low-pass 0 to 20000 Hz"
#define OMX_GATE_DECL_DIGEST "89c6643026ff9a19b63266e7c5661b32863e3689620767db36a0707c46367fe8"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_GATE_PARAM_CONTRACT_VERSION 3u
#define OMX_GATE_PARAM_CONTRACT_FINGERPRINT "5b1c1de696eb5a9be6973c2ca62cc1de55f797d360193e760b5f6923b4f64db4"
static const struct omx_param_contract_row OMX_GATE_PARAM_CONTRACT[OMX_GATE_PARAM_COUNT] = {
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* key_external none */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* threshold db */
  { 258u, OMX_CLAP_SCALE_LOG, 0.0 }, /* ratio ratio */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* range db */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attack ms */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* release ms */
  { 13u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* knee_start db */
  { 13u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* knee_end db */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* hold ms */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* hysteresis db */
  { 16u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* key_hp hz */
  { 16u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* key_lp hz */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_GATE_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family gate. */
#define OMX_GATE_PANEL_FINGERPRINT "242ab64ba6d431ad101586d5ddcec59fecdee16d1cc142dc6f0145b4a44c30f3"
static const struct omx_panel_section_row OMX_GATE_PANEL_SECTIONS[] = {
  { "gate", "fx.gate.section.gate", "Gate" },
  { "key", "fx.gate.section.key", "Key" },
};
static const struct omx_panel_control_row OMX_GATE_PANEL_CONTROLS[] = {
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.gate.threshold", "Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.gate.range", "Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.gate.ratio", "Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.gate.attack", "Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 8u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.gate.hold", "Hold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.gate.release", "Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 9u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.gate.hysteresis", "Hysteresis", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.gate.knee_start", "Knee Start", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.gate.knee_end", "Knee End", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 0u, 1u, OMX_CLAP_WIDGET_AUTO, "fx.gate.key_external", "Key", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 10u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.gate.key_hp", "Key High-pass", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 11u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.gate.key_lp", "Key Low-pass", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_GATE_PANEL_ROLES[] = {
  { "threshold", 0u, 1u },
  { "range", 0u, 3u },
  { "attack", 0u, 4u },
  { "release", 0u, 5u },
  { "hold", 0u, 8u },
  { "knee_start", 0u, 6u },
  { "knee_end", 0u, 7u },
  { "hysteresis", 0u, 9u },
  { "ratio", 0u, 2u },
};
static const struct omx_panel_display_row OMX_GATE_PANEL_DISPLAYS[] = {
  { OMX_CLAP_DISPLAY_TRANSFER_CURVE, 0u, OMX_CLAP_DISPLAY_FROM_FAMILY, { OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.gate.display.transfer_curve", "Gate" },
  { OMX_CLAP_DISPLAY_GAIN_REDUCTION_BAR, 0u, OMX_CLAP_DISPLAY_FROM_METERS, { 0u, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.gate.display.gain_reduction_bar", "Gain reduction" },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_GATE_PANEL_PAGES[] = {
  { "Easy", "Easy", { 1u, 3u, 4u, 5u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Gate", "Gate", { 2u, 8u, 9u, 6u, 7u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Key", "Key", { 0u, 10u, 11u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_GATE_PANEL_PAGE_COUNT 3u
static const struct omx_panel_rows OMX_GATE_PANEL = {
  OMX_GATE_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_GATE,
  OMX_GATE_PANEL_SECTIONS, 2u,
  OMX_GATE_PANEL_CONTROLS, 12u,
  OMX_GATE_PANEL_ROLES, 9u,
  OMX_GATE_PANEL_DISPLAYS, 2u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_GATE_LV2_PORT_ENABLED 0u
#define OMX_GATE_LV2_PORT_LATENCY 1u
#define OMX_GATE_LV2_PORT_IN_L 3u
#define OMX_GATE_LV2_PORT_IN_R 4u
#define OMX_GATE_LV2_PORT_KEY 5u
#define OMX_GATE_LV2_PORT_OUT_L 6u
#define OMX_GATE_LV2_PORT_OUT_R 7u
#define OMX_GATE_LV2_PORT_FIRST_PARAM 8u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_GATE_LV2_PORT_FIRST_METER 2u
#define OMX_GATE_LV2_PORT_COUNT 20u

#endif /* OMX_GATE_PARAMS_H */
