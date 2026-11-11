// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_DEESSER_PARAMS_H
#define OMX_DEESSER_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-deesser/omx-deesser.decl.json.
 * Regenerate: `make -C plugins/omx-deesser gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_deesser_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_DEESSER_PARAM_DEESS_FREQ = 0,
  OMX_DEESSER_PARAM_DEESS_WIDTH = 1,
  OMX_DEESSER_PARAM_DEESS_THRESHOLD = 2,
  OMX_DEESSER_PARAM_DEESS_RATIO = 3,
  OMX_DEESSER_PARAM_DEESS_RANGE = 4,
  OMX_DEESSER_PARAM_DEESS_ATTACK = 5,
  OMX_DEESSER_PARAM_DEESS_RELEASE = 6,
  OMX_DEESSER_PARAM_DEESS_MODE = 7,
  OMX_DEESSER_PARAM_COUNT = 8
};

static const omx_plugin_param OMX_DEESSER_PARAMS[OMX_DEESSER_PARAM_COUNT] = {
  { "deessFreq", "Freq", "Hz", 2000.0f, 16000.0f, 7000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "deessWidth", "Width", "oct", 0.25f, 4.0f, 1.0f, 0u },
  { "deessThreshold", "Threshold", "dB", -60.0f, 0.0f, -30.0f, 0u },
  { "deessRatio", "Ratio", "", 1.0f, 20.0f, 4.0f, 0u },
  { "deessRange", "Range", "dB", -24.0f, 0.0f, -12.0f, 0u },
  { "deessAttack", "Attack", "ms", 0.1f, 50.0f, 1.0f, 0u },
  { "deessRelease", "Release", "ms", 5.0f, 500.0f, 60.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "deessMode", "Mode", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_DEESSER_PARAM_MODULES[OMX_DEESSER_PARAM_COUNT] = {
  "De-esser",
  "De-esser",
  "De-esser",
  "De-esser",
  "De-esser",
  "De-esser",
  "De-esser",
  "De-esser",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_DEESSER_PARAM_DEESS_FREQ_MIN 2000.0f
#define OMX_DEESSER_PARAM_DEESS_FREQ_MAX 16000.0f
#define OMX_DEESSER_PARAM_DEESS_FREQ_DEFAULT 7000.0f
#define OMX_DEESSER_PARAM_DEESS_WIDTH_MIN 0.25f
#define OMX_DEESSER_PARAM_DEESS_WIDTH_MAX 4.0f
#define OMX_DEESSER_PARAM_DEESS_WIDTH_DEFAULT 1.0f
#define OMX_DEESSER_PARAM_DEESS_THRESHOLD_MIN -60.0f
#define OMX_DEESSER_PARAM_DEESS_THRESHOLD_MAX 0.0f
#define OMX_DEESSER_PARAM_DEESS_THRESHOLD_DEFAULT -30.0f
#define OMX_DEESSER_PARAM_DEESS_RATIO_MIN 1.0f
#define OMX_DEESSER_PARAM_DEESS_RATIO_MAX 20.0f
#define OMX_DEESSER_PARAM_DEESS_RATIO_DEFAULT 4.0f
#define OMX_DEESSER_PARAM_DEESS_RANGE_MIN -24.0f
#define OMX_DEESSER_PARAM_DEESS_RANGE_MAX 0.0f
#define OMX_DEESSER_PARAM_DEESS_RANGE_DEFAULT -12.0f
#define OMX_DEESSER_PARAM_DEESS_ATTACK_MIN 0.1f
#define OMX_DEESSER_PARAM_DEESS_ATTACK_MAX 50.0f
#define OMX_DEESSER_PARAM_DEESS_ATTACK_DEFAULT 1.0f
#define OMX_DEESSER_PARAM_DEESS_RELEASE_MIN 5.0f
#define OMX_DEESSER_PARAM_DEESS_RELEASE_MAX 500.0f
#define OMX_DEESSER_PARAM_DEESS_RELEASE_DEFAULT 60.0f
#define OMX_DEESSER_PARAM_DEESS_MODE_MIN 0.0f
#define OMX_DEESSER_PARAM_DEESS_MODE_MAX 1.0f
#define OMX_DEESSER_PARAM_DEESS_MODE_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_DEESSER_NAME "omx deesser"
#define OMX_DEESSER_VENDOR "openmixer"
#define OMX_DEESSER_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_DEESSER_VERSION "0.19.3"
#define OMX_DEESSER_DESCRIPTION "The console's de-esser: a detector on a band around the sibilance drives the compressor's gain computer, and the reduction lands on that band alone or on the whole signal."
#define OMX_DEESSER_CLAP_ID "org.openmixer.deesser"
#define OMX_DEESSER_CLAP_FEATURES "audio-effect", "de-esser", "stereo"
#define OMX_DEESSER_LV2_URI "urn:openmixer:deesser"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_DEESSER_DECL_SOURCE "omx deesser: Freq 2000 to 16000 Hz, Width 0.25 to 4 oct, Threshold -60 to 0 dB, Ratio 1 to 20, Range -24 to 0 dB, Attack 0.1 to 50 ms, Release 5 to 500 ms, Mode 0 to 1"
#define OMX_DEESSER_DECL_DIGEST "5229f9c0817d98fea312f088c1f8519628c36ef5f1d2b453b22f3ba0571f097f"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_DEESSER_PARAM_CONTRACT_VERSION 3u
#define OMX_DEESSER_PARAM_CONTRACT_FINGERPRINT "6e8f9ba1730c2a1430be5992c8744d81ee7b89a76d34a78fc63c27422df47e5c"
static const struct omx_param_contract_row OMX_DEESSER_PARAM_CONTRACT[OMX_DEESSER_PARAM_COUNT] = {
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* deessFreq hz */
  { 20u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* deessWidth oct */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* deessThreshold db */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* deessRatio ratio */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* deessRange db */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* deessAttack ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* deessRelease ms */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* deessMode none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_DEESSER_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family deesser. */
#define OMX_DEESSER_PANEL_FINGERPRINT "71c3d18dee433be807a6c6c82a361606764e5516ec90199620c959d1a09f9764"
static const struct omx_panel_section_row OMX_DEESSER_PANEL_SECTIONS[] = {
  { "de_esser", "fx.deesser.section.de_esser", "De-esser" },
};
static const struct omx_panel_control_row OMX_DEESSER_PANEL_CONTROLS[] = {
  { 7u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.deesser.deess_mode", "Mode", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.deesser.deess_freq", "Freq", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.deesser.deess_width", "Width", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.deesser.deess_threshold", "Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.deesser.deess_ratio", "Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.deesser.deess_range", "Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.deesser.deess_attack", "Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.deesser.deess_release", "Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_DEESSER_PANEL_ROLES[] = {
  { "frequency", 0u, 0u },
  { "threshold", 0u, 2u },
  { "range", 0u, 4u },
  { "width", 0u, 1u },
  { "ratio", 0u, 3u },
  { "attack", 0u, 5u },
  { "release", 0u, 6u },
  { "mode", 0u, 7u },
};
static const struct omx_panel_display_row OMX_DEESSER_PANEL_DISPLAYS[] = {
  { OMX_CLAP_DISPLAY_GAIN_REDUCTION_BAR, 0u, OMX_CLAP_DISPLAY_FROM_METERS, { 0u, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.deesser.display.gain_reduction_bar", "Gain reduction" },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_DEESSER_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 2u, 4u, 7u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "De-esser", "De-esser", { 1u, 3u, 5u, 6u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_DEESSER_PANEL_PAGE_COUNT 2u
static const struct omx_panel_rows OMX_DEESSER_PANEL = {
  OMX_DEESSER_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_DEESSER,
  OMX_DEESSER_PANEL_SECTIONS, 1u,
  OMX_DEESSER_PANEL_CONTROLS, 8u,
  OMX_DEESSER_PANEL_ROLES, 8u,
  OMX_DEESSER_PANEL_DISPLAYS, 1u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_DEESSER_LV2_PORT_ENABLED 0u
#define OMX_DEESSER_LV2_PORT_LATENCY 1u
#define OMX_DEESSER_LV2_PORT_IN_L 7u
#define OMX_DEESSER_LV2_PORT_IN_R 8u
#define OMX_DEESSER_LV2_PORT_OUT_L 9u
#define OMX_DEESSER_LV2_PORT_OUT_R 10u
#define OMX_DEESSER_LV2_PORT_FIRST_PARAM 11u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_DEESSER_LV2_PORT_FIRST_METER 2u
#define OMX_DEESSER_LV2_PORT_COUNT 19u

#endif /* OMX_DEESSER_PARAMS_H */
