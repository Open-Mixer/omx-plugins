// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_GLUE_PARAMS_H
#define OMX_GLUE_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-glue/omx-glue.decl.json.
 * Regenerate: `make -C plugins/omx-glue gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_glue_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_GLUE_PARAM_THRESHOLD_DB = 0,
  OMX_GLUE_PARAM_RATIO = 1,
  OMX_GLUE_PARAM_ATTACK = 2,
  OMX_GLUE_PARAM_RELEASE = 3,
  OMX_GLUE_PARAM_MAKEUP_DB = 4,
  OMX_GLUE_PARAM_KEY_HP_HZ = 5,
  OMX_GLUE_PARAM_MIX_PCT = 6,
  OMX_GLUE_PARAM_DETECTOR_TOPOLOGY = 7,
  OMX_GLUE_PARAM_COUNT = 8
};

static const omx_plugin_param OMX_GLUE_PARAMS[OMX_GLUE_PARAM_COUNT] = {
  { "thresholdDb", "Threshold", "dB", -40.0f, 0.0f, -10.0f, 0u },
  { "ratio", "Ratio", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "attack", "Attack", "", 0.0f, 5.0f, 4.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "release", "Release", "", 0.0f, 4.0f, 4.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "makeupDb", "Makeup", "dB", 0.0f, 24.0f, 0.0f, 0u },
  { "keyHpHz", "Key High-pass", "Hz", 0.0f, 500.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "mixPct", "Mix", "%", 0.0f, 100.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "detectorTopology", "Detector Topology", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_GLUE_PARAM_MODULES[OMX_GLUE_PARAM_COUNT] = {
  "Glue",
  "Glue",
  "Glue",
  "Glue",
  "Glue",
  "Glue",
  "Glue",
  "Glue",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_GLUE_PARAM_THRESHOLD_DB_MIN -40.0f
#define OMX_GLUE_PARAM_THRESHOLD_DB_MAX 0.0f
#define OMX_GLUE_PARAM_THRESHOLD_DB_DEFAULT -10.0f
#define OMX_GLUE_PARAM_RATIO_MIN 0.0f
#define OMX_GLUE_PARAM_RATIO_MAX 2.0f
#define OMX_GLUE_PARAM_RATIO_DEFAULT 0.0f
#define OMX_GLUE_PARAM_ATTACK_MIN 0.0f
#define OMX_GLUE_PARAM_ATTACK_MAX 5.0f
#define OMX_GLUE_PARAM_ATTACK_DEFAULT 4.0f
#define OMX_GLUE_PARAM_RELEASE_MIN 0.0f
#define OMX_GLUE_PARAM_RELEASE_MAX 4.0f
#define OMX_GLUE_PARAM_RELEASE_DEFAULT 4.0f
#define OMX_GLUE_PARAM_MAKEUP_DB_MIN 0.0f
#define OMX_GLUE_PARAM_MAKEUP_DB_MAX 24.0f
#define OMX_GLUE_PARAM_MAKEUP_DB_DEFAULT 0.0f
#define OMX_GLUE_PARAM_KEY_HP_HZ_MIN 0.0f
#define OMX_GLUE_PARAM_KEY_HP_HZ_MAX 500.0f
#define OMX_GLUE_PARAM_KEY_HP_HZ_DEFAULT 0.0f
#define OMX_GLUE_PARAM_MIX_PCT_MIN 0.0f
#define OMX_GLUE_PARAM_MIX_PCT_MAX 100.0f
#define OMX_GLUE_PARAM_MIX_PCT_DEFAULT 100.0f
#define OMX_GLUE_PARAM_DETECTOR_TOPOLOGY_MIN 0.0f
#define OMX_GLUE_PARAM_DETECTOR_TOPOLOGY_MAX 1.0f
#define OMX_GLUE_PARAM_DETECTOR_TOPOLOGY_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_GLUE_NAME "omx glue"
#define OMX_GLUE_VENDOR "openmixer"
#define OMX_GLUE_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_GLUE_VERSION "0.19.3"
#define OMX_GLUE_DESCRIPTION "A bus compressor with the detents of a classic console: stepped ratio, attack and release, a key filter and a dry mix."
#define OMX_GLUE_CLAP_ID "org.openmixer.glue"
#define OMX_GLUE_CLAP_FEATURES "audio-effect", "compressor", "stereo"
#define OMX_GLUE_LV2_URI "urn:openmixer:glue"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_GLUE_DECL_SOURCE "omx glue: Threshold -40 to 0 dB, Ratio 0 to 2, Attack 0 to 5, Release 0 to 4, Makeup 0 to 24 dB, Key High-pass 0 to 500 Hz, Mix 0 to 100 %, Detector Topology 0 to 1"
#define OMX_GLUE_DECL_DIGEST "7bcb023ca90e579d8a3ed67bd6066c428c3c8eb20cb121d2d092ce0876f6de0f"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_GLUE_PARAM_CONTRACT_VERSION 2u
#define OMX_GLUE_PARAM_CONTRACT_FINGERPRINT "0d7112e4b0cdbdd2d87e1cc276fa30ee1c8b7e9c527ff618b9df7f2cf67ecc8b"
static const struct omx_param_contract_row OMX_GLUE_PARAM_CONTRACT[OMX_GLUE_PARAM_COUNT] = {
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* thresholdDb db */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* ratio none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* attack none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* release none */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* makeupDb db */
  { 16u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* keyHpHz hz */
  { 14u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* mixPct pc */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* detectorTopology none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_GLUE_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family none. */
#define OMX_GLUE_PANEL_FINGERPRINT "12377e18c7b109d7e936905498378ce92d5ced468826c46133310cfef298f63e"
static const struct omx_panel_section_row OMX_GLUE_PANEL_SECTIONS[] = {
  { "glue", "fx.glue.section.glue", "Glue" },
};
static const struct omx_panel_control_row OMX_GLUE_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.glue.threshold_db", "Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.glue.ratio", "Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.glue.attack", "Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.glue.release", "Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.glue.makeup_db", "Makeup", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.glue.key_hp_hz", "Key High-pass", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.glue.mix_pct", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.glue.detector_topology", "Detector Topology", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_GLUE_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Glue", "Glue", { 4u, 5u, 6u, 7u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_GLUE_PANEL_PAGE_COUNT 2u
static const struct omx_panel_rows OMX_GLUE_PANEL = {
  OMX_GLUE_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_NONE,
  OMX_GLUE_PANEL_SECTIONS, 1u,
  OMX_GLUE_PANEL_CONTROLS, 8u,
  NULL, 0u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_GLUE_LV2_PORT_ENABLED 0u
#define OMX_GLUE_LV2_PORT_LATENCY 1u
#define OMX_GLUE_LV2_PORT_IN_L 2u
#define OMX_GLUE_LV2_PORT_IN_R 3u
#define OMX_GLUE_LV2_PORT_OUT_L 4u
#define OMX_GLUE_LV2_PORT_OUT_R 5u
#define OMX_GLUE_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_GLUE_LV2_PORT_FIRST_METER 14u
#define OMX_GLUE_LV2_PORT_COUNT 14u

#endif /* OMX_GLUE_PARAMS_H */
