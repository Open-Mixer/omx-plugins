// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_REVERB_PARAMS_H
#define OMX_REVERB_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-reverb/omx-reverb.decl.json.
 * Regenerate: `make -C plugins/omx-reverb gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_reverb_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_REVERB_PARAM_PLATE_MOD_DEPTH = 0,
  OMX_REVERB_PARAM_MIX = 1,
  OMX_REVERB_PARAM_SIZE = 2,
  OMX_REVERB_PARAM_DAMPING = 3,
  OMX_REVERB_PARAM_WIDTH = 4,
  OMX_REVERB_PARAM_PREDELAY = 5,
  OMX_REVERB_PARAM_LOWCUT = 6,
  OMX_REVERB_PARAM_HIGHCUT = 7,
  OMX_REVERB_PARAM_REVERSE = 8,
  OMX_REVERB_PARAM_HOLD = 9,
  OMX_REVERB_PARAM_RELEASE = 10,
  OMX_REVERB_PARAM_GATE_THRESHOLD = 11,
  OMX_REVERB_PARAM_ALGORITHM = 12,
  OMX_REVERB_PARAM_COUNT = 13
};

static const omx_plugin_param OMX_REVERB_PARAMS[OMX_REVERB_PARAM_COUNT] = {
  { "plateModDepth", "Plate Mod Depth", "%", 0.0f, 400.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "mix", "Mix", "", 0.0f, 1.0f, 0.3f, 0u },
  { "size", "Size", "", 0.0f, 1.0f, 0.7f, 0u },
  { "damping", "Damping", "", 0.0f, 1.0f, 0.5f, 0u },
  { "width", "Width", "", 0.0f, 1.0f, 1.0f, 0u },
  { "predelay", "Predelay", "ms", 0.0f, 100.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "lowcut", "Lowcut", "Hz", 0.0f, 20000.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "highcut", "Highcut", "Hz", 0.0f, 20000.0f, 20000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "reverse", "Reverse", "ms", 50.0f, 500.0f, 300.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "hold", "Hold", "ms", 10.0f, 2000.0f, 120.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "release", "Release", "ms", 1.0f, 500.0f, 20.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "gateThreshold", "Gate Threshold", "dBFS", -80.0f, 0.0f, -40.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "algorithm", "Algorithm", "", 0.0f, 4.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_REVERB_PARAM_MODULES[OMX_REVERB_PARAM_COUNT] = {
  "Plate",
  "Reverb",
  "Reverb",
  "Reverb",
  "Reverb",
  "Reverb",
  "Reverb",
  "Reverb",
  "Reverse",
  "Gated",
  "Gated",
  "Gated",
  "Reverb",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_REVERB_PARAM_PLATE_MOD_DEPTH_MIN 0.0f
#define OMX_REVERB_PARAM_PLATE_MOD_DEPTH_MAX 400.0f
#define OMX_REVERB_PARAM_PLATE_MOD_DEPTH_DEFAULT 100.0f
#define OMX_REVERB_PARAM_MIX_MIN 0.0f
#define OMX_REVERB_PARAM_MIX_MAX 1.0f
#define OMX_REVERB_PARAM_MIX_DEFAULT 0.3f
#define OMX_REVERB_PARAM_SIZE_MIN 0.0f
#define OMX_REVERB_PARAM_SIZE_MAX 1.0f
#define OMX_REVERB_PARAM_SIZE_DEFAULT 0.7f
#define OMX_REVERB_PARAM_DAMPING_MIN 0.0f
#define OMX_REVERB_PARAM_DAMPING_MAX 1.0f
#define OMX_REVERB_PARAM_DAMPING_DEFAULT 0.5f
#define OMX_REVERB_PARAM_WIDTH_MIN 0.0f
#define OMX_REVERB_PARAM_WIDTH_MAX 1.0f
#define OMX_REVERB_PARAM_WIDTH_DEFAULT 1.0f
#define OMX_REVERB_PARAM_PREDELAY_MIN 0.0f
#define OMX_REVERB_PARAM_PREDELAY_MAX 100.0f
#define OMX_REVERB_PARAM_PREDELAY_DEFAULT 0.0f
#define OMX_REVERB_PARAM_LOWCUT_MIN 0.0f
#define OMX_REVERB_PARAM_LOWCUT_MAX 20000.0f
#define OMX_REVERB_PARAM_LOWCUT_DEFAULT 0.0f
#define OMX_REVERB_PARAM_HIGHCUT_MIN 0.0f
#define OMX_REVERB_PARAM_HIGHCUT_MAX 20000.0f
#define OMX_REVERB_PARAM_HIGHCUT_DEFAULT 20000.0f
#define OMX_REVERB_PARAM_REVERSE_MIN 50.0f
#define OMX_REVERB_PARAM_REVERSE_MAX 500.0f
#define OMX_REVERB_PARAM_REVERSE_DEFAULT 300.0f
#define OMX_REVERB_PARAM_HOLD_MIN 10.0f
#define OMX_REVERB_PARAM_HOLD_MAX 2000.0f
#define OMX_REVERB_PARAM_HOLD_DEFAULT 120.0f
#define OMX_REVERB_PARAM_RELEASE_MIN 1.0f
#define OMX_REVERB_PARAM_RELEASE_MAX 500.0f
#define OMX_REVERB_PARAM_RELEASE_DEFAULT 20.0f
#define OMX_REVERB_PARAM_GATE_THRESHOLD_MIN -80.0f
#define OMX_REVERB_PARAM_GATE_THRESHOLD_MAX 0.0f
#define OMX_REVERB_PARAM_GATE_THRESHOLD_DEFAULT -40.0f
#define OMX_REVERB_PARAM_ALGORITHM_MIN 0.0f
#define OMX_REVERB_PARAM_ALGORITHM_MAX 4.0f
#define OMX_REVERB_PARAM_ALGORITHM_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_REVERB_NAME "omx reverb"
#define OMX_REVERB_VENDOR "openmixer"
#define OMX_REVERB_URL "https://github.com/Open-Mixer/omx-plugins"
#define OMX_REVERB_VERSION "0.19.3"
#define OMX_REVERB_DESCRIPTION "The console's reverb: room, plate, hall, reverse and gated algorithms with pre-delay, size, damping, width and low and high cuts, mixed with the dry signal."
#define OMX_REVERB_CLAP_ID "org.openmixer.reverb"
#define OMX_REVERB_CLAP_FEATURES "audio-effect", "reverb", "stereo"
#define OMX_REVERB_LV2_URI "urn:openmixer:reverb"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_REVERB_DECL_SOURCE "omx reverb: Plate Mod Depth 0 to 400 %, Mix 0 to 1, Size 0 to 1, Damping 0 to 1, Width 0 to 1, Predelay 0 to 100 ms, Lowcut 0 to 20000 Hz, Highcut 0 to 20000 Hz, Reverse 50 to 500 ms, Hold 10 to 2000 ms, Release 1 to 500 ms, Gate Threshold -80 to 0 dBFS, Algorithm 0 to 4"
#define OMX_REVERB_DECL_DIGEST "a42c6e20426afd868f08938af4dfb54b487bdf4d8a5d9b13f75a967319df7009"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_REVERB_PARAM_CONTRACT_VERSION 2u
#define OMX_REVERB_PARAM_CONTRACT_FINGERPRINT "4bfc8b448bcedb15595b78e1694ba771f4cf8f7cd7fb2ce077d9414b9af31afc"
static const struct omx_param_contract_row OMX_REVERB_PARAM_CONTRACT[OMX_REVERB_PARAM_COUNT] = {
  { 14u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* plateModDepth pc */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* mix none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* size none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* damping none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* width none */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* predelay ms */
  { 16u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* lowcut hz */
  { 16u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* highcut hz */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* reverse ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* hold ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* release ms */
  { 256u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* gateThreshold dbfs */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* algorithm none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_REVERB_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family reverb. */
#define OMX_REVERB_PANEL_FINGERPRINT "7626d07e785ceff25b3603d57d7ad73c2bf132c17ab248bf6d227e1d2e93a722"
static const struct omx_panel_section_row OMX_REVERB_PANEL_SECTIONS[] = {
  { "reverb", "fx.reverb.section.reverb", "Reverb" },
  { "reverse", "fx.reverb.section.reverse", "Reverse" },
  { "gated", "fx.reverb.section.gated", "Gated" },
  { "plate", "fx.reverb.section.plate", "Plate" },
};
static const struct omx_panel_control_row OMX_REVERB_PANEL_CONTROLS[] = {
  { 12u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.reverb.algorithm", "Algorithm", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.reverb.size", "Size", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.reverb.damping", "Damping", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.reverb.predelay", "Predelay", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.reverb.width", "Width", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.reverb.mix", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.reverb.lowcut", "Lowcut", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.reverb.highcut", "Highcut", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 8u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.reverb.reverse", "Reverse", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 9u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.reverb.hold", "Hold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 10u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.reverb.release", "Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 11u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.reverb.gate_threshold", "Gate Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 0u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.reverb.plate_mod_depth", "Plate Mod Depth", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_REVERB_PANEL_ROLES[] = {
  { "size", 0u, 2u },
  { "algorithm", 0u, 12u },
  { "predelay", 0u, 5u },
  { "damping", 0u, 3u },
  { "width", 0u, 4u },
  { "low_cut", 0u, 6u },
  { "high_cut", 0u, 7u },
  { "mix", 0u, 1u },
};
static const struct omx_panel_default_row OMX_REVERB_PANEL_DEFAULTS[] = {
  { OMX_CLAP_PLACEMENT_RETURN, 1u, 1 },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_REVERB_PANEL_PAGES[] = {
  { "Easy", "Easy", { 2u, 3u, 1u, 12u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Reverb", "Reverb", { 5u, 4u, 6u, 7u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Reverse", "Reverse", { 8u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Gated", "Gated", { 9u, 10u, 11u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Plate", "Plate", { 0u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_REVERB_PANEL_PAGE_COUNT 5u
static const struct omx_panel_rows OMX_REVERB_PANEL = {
  OMX_REVERB_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_REVERB,
  OMX_REVERB_PANEL_SECTIONS, 4u,
  OMX_REVERB_PANEL_CONTROLS, 13u,
  OMX_REVERB_PANEL_ROLES, 8u,
  NULL, 0u,
  OMX_REVERB_PANEL_DEFAULTS, 1u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_REVERB_LV2_PORT_ENABLED 0u
#define OMX_REVERB_LV2_PORT_LATENCY 1u
#define OMX_REVERB_LV2_PORT_IN_L 2u
#define OMX_REVERB_LV2_PORT_IN_R 3u
#define OMX_REVERB_LV2_PORT_OUT_L 4u
#define OMX_REVERB_LV2_PORT_OUT_R 5u
#define OMX_REVERB_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_REVERB_LV2_PORT_FIRST_METER 19u
#define OMX_REVERB_LV2_PORT_COUNT 19u

#endif /* OMX_REVERB_PARAMS_H */
