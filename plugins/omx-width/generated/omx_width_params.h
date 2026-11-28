// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_WIDTH_PARAMS_H
#define OMX_WIDTH_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-width/omx-width.decl.json.
 * Regenerate: `make -C plugins/omx-width gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_width_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_WIDTH_PARAM_WIDTH_PCT = 0,
  OMX_WIDTH_PARAM_PSEUDO = 1,
  OMX_WIDTH_PARAM_PSEUDO_MS = 2,
  OMX_WIDTH_PARAM_BASS_MONO = 3,
  OMX_WIDTH_PARAM_BASS_MONO_HZ = 4,
  OMX_WIDTH_PARAM_COUNT = 5
};

static const omx_plugin_param OMX_WIDTH_PARAMS[OMX_WIDTH_PARAM_COUNT] = {
  { "widthPct", "Width", "%", 0.0f, 200.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "pseudo", "Pseudo", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "pseudoMs", "Pseudo Delay", "ms", 0.0f, 30.0f, 12.0f, 0u },
  { "bassMono", "Bass Mono", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "bassMonoHz", "Bass Mono Frequency", "Hz", 20.0f, 300.0f, 120.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_WIDTH_PARAM_MODULES[OMX_WIDTH_PARAM_COUNT] = {
  "Width",
  "Width",
  "Width",
  "Width",
  "Width",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_WIDTH_PARAM_WIDTH_PCT_MIN 0.0f
#define OMX_WIDTH_PARAM_WIDTH_PCT_MAX 200.0f
#define OMX_WIDTH_PARAM_WIDTH_PCT_DEFAULT 100.0f
#define OMX_WIDTH_PARAM_PSEUDO_MIN 0.0f
#define OMX_WIDTH_PARAM_PSEUDO_MAX 2.0f
#define OMX_WIDTH_PARAM_PSEUDO_DEFAULT 0.0f
#define OMX_WIDTH_PARAM_PSEUDO_MS_MIN 0.0f
#define OMX_WIDTH_PARAM_PSEUDO_MS_MAX 30.0f
#define OMX_WIDTH_PARAM_PSEUDO_MS_DEFAULT 12.0f
#define OMX_WIDTH_PARAM_BASS_MONO_MIN 0.0f
#define OMX_WIDTH_PARAM_BASS_MONO_MAX 1.0f
#define OMX_WIDTH_PARAM_BASS_MONO_DEFAULT 0.0f
#define OMX_WIDTH_PARAM_BASS_MONO_HZ_MIN 20.0f
#define OMX_WIDTH_PARAM_BASS_MONO_HZ_MAX 300.0f
#define OMX_WIDTH_PARAM_BASS_MONO_HZ_DEFAULT 120.0f

/* The identity every face publishes. */
#define OMX_WIDTH_NAME "omx width"
#define OMX_WIDTH_VENDOR "openmixer"
#define OMX_WIDTH_URL "https://github.com/Open-Mixer/omx-plugins"
#define OMX_WIDTH_VERSION "0.22.0"
#define OMX_WIDTH_DESCRIPTION "Widens or narrows the stereo image, with an optional pseudo-stereo spread for a mono source and a bass-mono cutoff that keeps the lows centred."
#define OMX_WIDTH_CLAP_ID "org.openmixer.width"
#define OMX_WIDTH_CLAP_FEATURES "audio-effect", "utility", "stereo"
#define OMX_WIDTH_LV2_URI "urn:openmixer:width"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_WIDTH_DECL_SOURCE "omx width: Width 0 to 200 %, Pseudo 0 to 2, Pseudo Delay 0 to 30 ms, Bass Mono 0 to 1, Bass Mono Frequency 20 to 300 Hz"
#define OMX_WIDTH_DECL_DIGEST "c4f8888c027f4ce250f12b68314bda665bb1e8d96594d17f2f67ff410ecf9eba"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_WIDTH_PARAM_CONTRACT_VERSION 1u
#define OMX_WIDTH_PARAM_CONTRACT_FINGERPRINT "6a77b6877ad3ddd60e4b227e2d98aa601be552ed3e9c696acb1cbc9413b5653d"
static const struct omx_param_contract_row OMX_WIDTH_PARAM_CONTRACT[OMX_WIDTH_PARAM_COUNT] = {
  { 14u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* widthPct pc */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* pseudo none */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* pseudoMs ms */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bassMono none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* bassMonoHz hz */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_WIDTH_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family none. */
#define OMX_WIDTH_PANEL_FINGERPRINT "b22b35350fb27a00e57b89d33c5d4b9f0c318652392dd9ea17c7ce912c41baf4"
static const struct omx_panel_section_row OMX_WIDTH_PANEL_SECTIONS[] = {
  { "width", "fx.width.section.width", "Width" },
};
static const struct omx_panel_control_row OMX_WIDTH_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.width.width_pct", "Width", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.width.pseudo", "Pseudo", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.width.pseudo_ms", "Pseudo Delay", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.width.bass_mono", "Bass Mono", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.width.bass_mono_hz", "Bass Mono Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_WIDTH_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, 4u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Width", "Width", { 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_WIDTH_PANEL_PAGE_COUNT 2u
static const struct omx_panel_rows OMX_WIDTH_PANEL = {
  OMX_WIDTH_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_NONE,
  OMX_WIDTH_PANEL_SECTIONS, 1u,
  OMX_WIDTH_PANEL_CONTROLS, 5u,
  NULL, 0u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_WIDTH_LV2_PORT_ENABLED 0u
#define OMX_WIDTH_LV2_PORT_LATENCY 1u
#define OMX_WIDTH_LV2_PORT_IN_L 2u
#define OMX_WIDTH_LV2_PORT_IN_R 3u
#define OMX_WIDTH_LV2_PORT_OUT_L 4u
#define OMX_WIDTH_LV2_PORT_OUT_R 5u
#define OMX_WIDTH_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_WIDTH_LV2_PORT_FIRST_METER 11u
#define OMX_WIDTH_LV2_PORT_COUNT 11u

#endif /* OMX_WIDTH_PARAMS_H */
