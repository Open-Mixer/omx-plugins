// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_COMP_PARAMS_H
#define OMX_COMP_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-comp/omx-comp.decl.json.
 * Regenerate: `make -C plugins/omx-comp gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_comp_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_COMP_PARAM_THRESHOLD_DB = 0,
  OMX_COMP_PARAM_RATIO = 1,
  OMX_COMP_PARAM_KNEE_DB = 2,
  OMX_COMP_PARAM_ATTACK_MS = 3,
  OMX_COMP_PARAM_RELEASE_MS = 4,
  OMX_COMP_PARAM_MAKEUP_DB = 5,
  OMX_COMP_PARAM_MIX_PCT = 6,
  OMX_COMP_PARAM_KIND = 7,
  OMX_COMP_PARAM_DETECTOR_OVERSAMPLING = 8,
  OMX_COMP_PARAM_DETECTOR_TOPOLOGY = 9,
  OMX_COMP_PARAM_KEY_SOURCE = 10,
  OMX_COMP_PARAM_KEY_HP_HZ = 11,
  OMX_COMP_PARAM_KEY_LP_HZ = 12,
  OMX_COMP_PARAM_KEY_TILT_DB = 13,
  OMX_COMP_PARAM_KEY_LISTEN = 14,
  OMX_COMP_PARAM_HOLD_MS = 15,
  OMX_COMP_PARAM_DETECT_LINK = 16,
  OMX_COMP_PARAM_DUCK = 17,
  OMX_COMP_PARAM_RANGE_DB = 18,
  OMX_COMP_PARAM_COUNT = 19
};

static const omx_plugin_param OMX_COMP_PARAMS[OMX_COMP_PARAM_COUNT] = {
  { "thresholdDb", "Threshold", "dB", -60.0f, 0.0f, -18.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "ratio", "Ratio", "", 1.0f, 20.0f, 4.0f, 0u },
  { "kneeDb", "Knee", "dB", 0.0f, 24.0f, 6.0f, 0u },
  { "attackMs", "Attack", "ms", 0.1f, 100.0f, 5.0f, 0u },
  { "releaseMs", "Release", "ms", 5.0f, 3000.0f, 200.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "makeupDb", "Makeup", "dB", 0.0f, 24.0f, 0.0f, 0u },
  { "mixPct", "Mix", "%", 0.0f, 100.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "kind", "Kind", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "detectorOversampling", "Detector Oversampling", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "detectorTopology", "Detector Topology", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "keySource", "Key Source", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "keyHpHz", "Key HP", "Hz", 0.0f, 2000.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "keyLpHz", "Key LP", "Hz", 0.0f, 20000.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "keyTiltDb", "Key Tilt", "dB/oct", -6.0f, 6.0f, 0.0f, 0u },
  { "keyListen", "Key Listen", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "holdMs", "Hold", "ms", 0.0f, 2000.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "detectLink", "Detect Link", "", 0.0f, 3.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "duck", "Duck", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "rangeDb", "Duck Range", "dB", -40.0f, 0.0f, -12.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_COMP_PARAM_MODULES[OMX_COMP_PARAM_COUNT] = {
  "Comp",
  "Comp",
  "Comp",
  "Comp",
  "Comp",
  "Comp",
  "Comp",
  "Comp",
  "Comp",
  "Comp",
  "Key",
  "Key",
  "Key",
  "Key",
  "Key",
  "Comp",
  "Key",
  "Duck",
  "Duck",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_COMP_PARAM_THRESHOLD_DB_MIN -60.0f
#define OMX_COMP_PARAM_THRESHOLD_DB_MAX 0.0f
#define OMX_COMP_PARAM_THRESHOLD_DB_DEFAULT -18.0f
#define OMX_COMP_PARAM_RATIO_MIN 1.0f
#define OMX_COMP_PARAM_RATIO_MAX 20.0f
#define OMX_COMP_PARAM_RATIO_DEFAULT 4.0f
#define OMX_COMP_PARAM_KNEE_DB_MIN 0.0f
#define OMX_COMP_PARAM_KNEE_DB_MAX 24.0f
#define OMX_COMP_PARAM_KNEE_DB_DEFAULT 6.0f
#define OMX_COMP_PARAM_ATTACK_MS_MIN 0.1f
#define OMX_COMP_PARAM_ATTACK_MS_MAX 100.0f
#define OMX_COMP_PARAM_ATTACK_MS_DEFAULT 5.0f
#define OMX_COMP_PARAM_RELEASE_MS_MIN 5.0f
#define OMX_COMP_PARAM_RELEASE_MS_MAX 3000.0f
#define OMX_COMP_PARAM_RELEASE_MS_DEFAULT 200.0f
#define OMX_COMP_PARAM_MAKEUP_DB_MIN 0.0f
#define OMX_COMP_PARAM_MAKEUP_DB_MAX 24.0f
#define OMX_COMP_PARAM_MAKEUP_DB_DEFAULT 0.0f
#define OMX_COMP_PARAM_MIX_PCT_MIN 0.0f
#define OMX_COMP_PARAM_MIX_PCT_MAX 100.0f
#define OMX_COMP_PARAM_MIX_PCT_DEFAULT 100.0f
#define OMX_COMP_PARAM_KIND_MIN 0.0f
#define OMX_COMP_PARAM_KIND_MAX 1.0f
#define OMX_COMP_PARAM_KIND_DEFAULT 0.0f
#define OMX_COMP_PARAM_DETECTOR_OVERSAMPLING_MIN 0.0f
#define OMX_COMP_PARAM_DETECTOR_OVERSAMPLING_MAX 2.0f
#define OMX_COMP_PARAM_DETECTOR_OVERSAMPLING_DEFAULT 0.0f
#define OMX_COMP_PARAM_DETECTOR_TOPOLOGY_MIN 0.0f
#define OMX_COMP_PARAM_DETECTOR_TOPOLOGY_MAX 1.0f
#define OMX_COMP_PARAM_DETECTOR_TOPOLOGY_DEFAULT 0.0f
#define OMX_COMP_PARAM_KEY_SOURCE_MIN 0.0f
#define OMX_COMP_PARAM_KEY_SOURCE_MAX 1.0f
#define OMX_COMP_PARAM_KEY_SOURCE_DEFAULT 0.0f
#define OMX_COMP_PARAM_KEY_HP_HZ_MIN 0.0f
#define OMX_COMP_PARAM_KEY_HP_HZ_MAX 2000.0f
#define OMX_COMP_PARAM_KEY_HP_HZ_DEFAULT 0.0f
#define OMX_COMP_PARAM_KEY_LP_HZ_MIN 0.0f
#define OMX_COMP_PARAM_KEY_LP_HZ_MAX 20000.0f
#define OMX_COMP_PARAM_KEY_LP_HZ_DEFAULT 0.0f
#define OMX_COMP_PARAM_KEY_TILT_DB_MIN -6.0f
#define OMX_COMP_PARAM_KEY_TILT_DB_MAX 6.0f
#define OMX_COMP_PARAM_KEY_TILT_DB_DEFAULT 0.0f
#define OMX_COMP_PARAM_KEY_LISTEN_MIN 0.0f
#define OMX_COMP_PARAM_KEY_LISTEN_MAX 1.0f
#define OMX_COMP_PARAM_KEY_LISTEN_DEFAULT 0.0f
#define OMX_COMP_PARAM_HOLD_MS_MIN 0.0f
#define OMX_COMP_PARAM_HOLD_MS_MAX 2000.0f
#define OMX_COMP_PARAM_HOLD_MS_DEFAULT 0.0f
#define OMX_COMP_PARAM_DETECT_LINK_MIN 0.0f
#define OMX_COMP_PARAM_DETECT_LINK_MAX 3.0f
#define OMX_COMP_PARAM_DETECT_LINK_DEFAULT 0.0f
#define OMX_COMP_PARAM_DUCK_MIN 0.0f
#define OMX_COMP_PARAM_DUCK_MAX 1.0f
#define OMX_COMP_PARAM_DUCK_DEFAULT 0.0f
#define OMX_COMP_PARAM_RANGE_DB_MIN -40.0f
#define OMX_COMP_PARAM_RANGE_DB_MAX 0.0f
#define OMX_COMP_PARAM_RANGE_DB_DEFAULT -12.0f

/* The identity every face publishes. */
#define OMX_COMP_NAME "omx comp"
#define OMX_COMP_VENDOR "openmixer"
#define OMX_COMP_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_COMP_VERSION "0.19.2"
#define OMX_COMP_DESCRIPTION "The console's compressor: threshold, ratio and knee set how hard it holds the level down, attack and release how fast, with makeup gain and a dry/wet mix. Kind picks the RMS detector (Compressor) or the peak detector (Limiter), and detector oversampling can run the control path at 4x; detector topology has it hear the input (feedforward) or its own output (feedback). Its detector can listen to a key routed to its sidechain input instead, through a high-pass, a low-pass and a tilt, on the louder leg, the mid or the side, with a hold; key listen puts what the detector hears on the output, and duck holds the reduction to a set depth. The DSP is omx-dsp's <omxdsp/fx/omx_comp_instance.h>, the console's own compressor."
#define OMX_COMP_CLAP_ID "org.openmixer.comp"
#define OMX_COMP_CLAP_FEATURES "audio-effect", "compressor", "stereo"
#define OMX_COMP_LV2_URI "urn:openmixer:comp"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_COMP_DECL_SOURCE "omx comp: Threshold -60 to 0 dB, Ratio 1 to 20, Knee 0 to 24 dB, Attack 0.1 to 100 ms, Release 5 to 3000 ms, Makeup 0 to 24 dB, Mix 0 to 100 %, Kind 0 to 1, Detector Oversampling 0 to 2, Detector Topology 0 to 1, Key Source 0 to 1, Key HP 0 to 2000 Hz, Key LP 0 to 20000 Hz, Key Tilt -6 to 6 dB/oct, Key Listen 0 to 1, Hold 0 to 2000 ms, Detect Link 0 to 3, Duck 0 to 1, Duck Range -40 to 0 dB"
#define OMX_COMP_DECL_DIGEST "433ca2f5811fecbbb6cc46a13dad9327d5b372ddfc808c51ecdefcc09495e0f7"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), is the face's. */
#define OMX_COMP_PARAM_CONTRACT_VERSION 1u
#define OMX_COMP_PARAM_CONTRACT_FINGERPRINT "9e6eb2dc019e7d5c8c4d9bfd3c4125056dc2eb2db90a34b5719e46bf0a8477cb"
static const struct omx_param_contract_row OMX_COMP_PARAM_CONTRACT[OMX_COMP_PARAM_COUNT] = {
  { 13u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* thresholdDb db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio none */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* kneeDb db */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attackMs ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* releaseMs ms */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* makeupDb db */
  { 14u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* mixPct pc */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* kind none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* detectorOversampling none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* detectorTopology none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* keySource none */
  { 16u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* keyHpHz hz */
  { 16u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* keyLpHz hz */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* keyTiltDb none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* keyListen none */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* holdMs ms */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* detectLink none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* duck none */
  { 13u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* rangeDb db */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_COMP_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family dynamics.
 * Declared roles not published:
 *   ratio (ratio): the ratio role takes ratio, coef, not no unit */
#define OMX_COMP_PANEL_FINGERPRINT "6efeb5c69eeb29e615a8fed653f4f1237aedcfe2374593812b674b29a05c76a1"
static const struct omx_panel_section_row OMX_COMP_PANEL_SECTIONS[] = {
  { "comp", "fx.comp.section.comp", "Comp" },
  { "key", "fx.comp.section.key", "Key" },
  { "duck", "fx.comp.section.duck", "Duck" },
};
static const struct omx_panel_control_row OMX_COMP_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.comp.threshold_db", "Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.comp.ratio", "Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.comp.knee_db", "Knee", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.comp.attack_ms", "Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.comp.release_ms", "Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 15u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.comp.hold_ms", "Hold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.comp.makeup_db", "Makeup", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.comp.mix_pct", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.comp.kind", "Kind", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 8u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.comp.detector_oversampling", "Detector Oversampling", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 9u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.comp.detector_topology", "Detector Topology", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 10u, 1u, OMX_CLAP_WIDGET_AUTO, "fx.comp.key_source", "Key Source", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 11u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.comp.key_hp_hz", "Key HP", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 12u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.comp.key_lp_hz", "Key LP", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 13u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.comp.key_tilt_db", "Key Tilt", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 16u, 1u, OMX_CLAP_WIDGET_AUTO, "fx.comp.detect_link", "Detect Link", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 14u, 1u, OMX_CLAP_WIDGET_SWITCH, "fx.comp.key_listen", "Key Listen", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 17u, 2u, OMX_CLAP_WIDGET_SWITCH, "fx.comp.duck", "Duck", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 18u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.comp.range_db", "Duck Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_COMP_PANEL_ROLES[] = {
  { "threshold", 0u, 0u },
  { "attack", 0u, 3u },
  { "release", 0u, 4u },
  { "knee", 0u, 2u },
  { "makeup", 0u, 5u },
  { "mix", 0u, 6u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections. */
static const struct omx_panel_page_row OMX_COMP_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 5u, 6u, 3u, 4u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Comp", "Comp", { 1u, 2u, 15u, 7u, 8u, 9u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Key", "Key", { 10u, 11u, 12u, 13u, 16u, 14u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Duck", "Duck", { 17u, 18u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_COMP_PANEL_PAGE_COUNT 4u
static const struct omx_panel_rows OMX_COMP_PANEL = {
  OMX_COMP_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_DYNAMICS,
  OMX_COMP_PANEL_SECTIONS, 3u,
  OMX_COMP_PANEL_CONTROLS, 19u,
  OMX_COMP_PANEL_ROLES, 6u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_COMP_LV2_PORT_ENABLED 0u
#define OMX_COMP_LV2_PORT_LATENCY 1u
#define OMX_COMP_LV2_PORT_IN_L 8u
#define OMX_COMP_LV2_PORT_IN_R 9u
#define OMX_COMP_LV2_PORT_KEY 10u
#define OMX_COMP_LV2_PORT_OUT_L 11u
#define OMX_COMP_LV2_PORT_OUT_R 12u
#define OMX_COMP_LV2_PORT_FIRST_PARAM 13u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_COMP_LV2_PORT_FIRST_METER 2u
#define OMX_COMP_LV2_PORT_COUNT 32u

#endif /* OMX_COMP_PARAMS_H */
