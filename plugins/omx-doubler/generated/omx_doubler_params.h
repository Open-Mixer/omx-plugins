// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_DOUBLER_PARAMS_H
#define OMX_DOUBLER_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-doubler/omx-doubler.decl.json.
 * Regenerate: `make -C plugins/omx-doubler gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_doubler_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_DOUBLER_PARAM_VOICES = 0,
  OMX_DOUBLER_PARAM_DELAY_MS = 1,
  OMX_DOUBLER_PARAM_DETUNE_CENTS = 2,
  OMX_DOUBLER_PARAM_MOD_RATE_HZ = 3,
  OMX_DOUBLER_PARAM_MOD_DEPTH_MS = 4,
  OMX_DOUBLER_PARAM_SPREAD_PCT = 5,
  OMX_DOUBLER_PARAM_MIX_PCT = 6,
  OMX_DOUBLER_PARAM_COUNT = 7
};

static const omx_plugin_param OMX_DOUBLER_PARAMS[OMX_DOUBLER_PARAM_COUNT] = {
  { "voices", "Voices", "", 1.0f, 4.0f, 2.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "delayMs", "Delay", "ms", 5.0f, 40.0f, 12.0f, 0u },
  { "detuneCents", "Detune", "cents", 0.0f, 25.0f, 8.0f, 0u },
  { "modRateHz", "Drift Rate", "Hz", 0.1f, 2.0f, 0.6f, 0u },
  { "modDepthMs", "Drift Depth", "ms", 0.0f, 3.0f, 0.5f, 0u },
  { "spreadPct", "Spread", "%", 0.0f, 100.0f, 60.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "mixPct", "Mix", "%", 0.0f, 100.0f, 50.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_DOUBLER_PARAM_MODULES[OMX_DOUBLER_PARAM_COUNT] = {
  "Voices",
  "Voices",
  "Voices",
  "Drift",
  "Drift",
  "Output",
  "Output",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_DOUBLER_PARAM_VOICES_MIN 1.0f
#define OMX_DOUBLER_PARAM_VOICES_MAX 4.0f
#define OMX_DOUBLER_PARAM_VOICES_DEFAULT 2.0f
#define OMX_DOUBLER_PARAM_DELAY_MS_MIN 5.0f
#define OMX_DOUBLER_PARAM_DELAY_MS_MAX 40.0f
#define OMX_DOUBLER_PARAM_DELAY_MS_DEFAULT 12.0f
#define OMX_DOUBLER_PARAM_DETUNE_CENTS_MIN 0.0f
#define OMX_DOUBLER_PARAM_DETUNE_CENTS_MAX 25.0f
#define OMX_DOUBLER_PARAM_DETUNE_CENTS_DEFAULT 8.0f
#define OMX_DOUBLER_PARAM_MOD_RATE_HZ_MIN 0.1f
#define OMX_DOUBLER_PARAM_MOD_RATE_HZ_MAX 2.0f
#define OMX_DOUBLER_PARAM_MOD_RATE_HZ_DEFAULT 0.6f
#define OMX_DOUBLER_PARAM_MOD_DEPTH_MS_MIN 0.0f
#define OMX_DOUBLER_PARAM_MOD_DEPTH_MS_MAX 3.0f
#define OMX_DOUBLER_PARAM_MOD_DEPTH_MS_DEFAULT 0.5f
#define OMX_DOUBLER_PARAM_SPREAD_PCT_MIN 0.0f
#define OMX_DOUBLER_PARAM_SPREAD_PCT_MAX 100.0f
#define OMX_DOUBLER_PARAM_SPREAD_PCT_DEFAULT 60.0f
#define OMX_DOUBLER_PARAM_MIX_PCT_MIN 0.0f
#define OMX_DOUBLER_PARAM_MIX_PCT_MAX 100.0f
#define OMX_DOUBLER_PARAM_MIX_PCT_DEFAULT 50.0f

/* The identity every face publishes. */
#define OMX_DOUBLER_NAME "omx doubler"
#define OMX_DOUBLER_VENDOR "openmixer"
#define OMX_DOUBLER_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_DOUBLER_VERSION "0.19.2"
#define OMX_DOUBLER_DESCRIPTION "Doubles a voice or an instrument with up to four slightly delayed, detuned and drifting copies spread across the stereo field, the double-tracked sound without a second take."
#define OMX_DOUBLER_CLAP_ID "org.openmixer.doubler"
#define OMX_DOUBLER_CLAP_FEATURES "audio-effect", "doubler", "stereo"
#define OMX_DOUBLER_LV2_URI "urn:openmixer:doubler"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_DOUBLER_DECL_SOURCE "omx doubler: Voices 1 to 4, Delay 5 to 40 ms, Detune 0 to 25 cents, Drift Rate 0.1 to 2 Hz, Drift Depth 0 to 3 ms, Spread 0 to 100 %, Mix 0 to 100 %"
#define OMX_DOUBLER_DECL_DIGEST "592de6b0ab8b42b9169e9007f3fa2bd33b350d61b48292ee6fba41e8fa2f3870"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_DOUBLER_PARAM_CONTRACT_VERSION 1u
#define OMX_DOUBLER_PARAM_CONTRACT_FINGERPRINT "2bc2860d318e0c31b59d7c0d7e822fbc0c28a471d0d57e34d357dd8dddd66cfe"
static const struct omx_param_contract_row OMX_DOUBLER_PARAM_CONTRACT[OMX_DOUBLER_PARAM_COUNT] = {
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* voices none */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* delayMs ms */
  { 21u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* detuneCents cent */
  { 16u, OMX_CLAP_SCALE_LOG, 0.0 }, /* modRateHz hz */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* modDepthMs ms */
  { 14u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* spreadPct pc */
  { 14u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* mixPct pc */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_DOUBLER_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family modulation.
 * Declared roles not published:
 *   depth (modDepthMs): the depth role takes none, pc, coef, oct, not ms */
#define OMX_DOUBLER_PANEL_FINGERPRINT "473323858d975bd4a9dc4c33793d227e4685b027715cbe5bd76a258626f10d96"
static const struct omx_panel_section_row OMX_DOUBLER_PANEL_SECTIONS[] = {
  { "voices", "fx.doubler.section.voices", "Voices" },
  { "drift", "fx.doubler.section.drift", "Drift" },
  { "output", "fx.doubler.section.output", "Output" },
};
static const struct omx_panel_control_row OMX_DOUBLER_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.doubler.voices", "Voices", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.doubler.delay_ms", "Delay", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.doubler.detune_cents", "Detune", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.doubler.mod_rate_hz", "Drift Rate", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.doubler.mod_depth_ms", "Drift Depth", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.doubler.spread_pct", "Spread", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.doubler.mix_pct", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_DOUBLER_PANEL_ROLES[] = {
  { "rate", 0u, 3u },
  { "voices", 0u, 0u },
  { "spread", 0u, 5u },
  { "mix", 0u, 6u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_DOUBLER_PANEL_PAGES[] = {
  { "Easy", "Easy", { 3u, 6u, 0u, 1u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Voices", "Voices", { 2u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Drift", "Drift", { 4u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Output", "Output", { 5u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_DOUBLER_PANEL_PAGE_COUNT 4u
static const struct omx_panel_rows OMX_DOUBLER_PANEL = {
  OMX_DOUBLER_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_MODULATION,
  OMX_DOUBLER_PANEL_SECTIONS, 3u,
  OMX_DOUBLER_PANEL_CONTROLS, 7u,
  OMX_DOUBLER_PANEL_ROLES, 4u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_DOUBLER_LV2_PORT_ENABLED 0u
#define OMX_DOUBLER_LV2_PORT_LATENCY 1u
#define OMX_DOUBLER_LV2_PORT_IN_L 2u
#define OMX_DOUBLER_LV2_PORT_IN_R 3u
#define OMX_DOUBLER_LV2_PORT_OUT_L 4u
#define OMX_DOUBLER_LV2_PORT_OUT_R 5u
#define OMX_DOUBLER_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_DOUBLER_LV2_PORT_FIRST_METER 13u
#define OMX_DOUBLER_LV2_PORT_COUNT 13u

#endif /* OMX_DOUBLER_PARAMS_H */
