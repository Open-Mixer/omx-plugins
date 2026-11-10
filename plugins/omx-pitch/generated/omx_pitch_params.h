// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_PITCH_PARAMS_H
#define OMX_PITCH_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-pitch/omx-pitch.decl.json.
 * Regenerate: `make -C plugins/omx-pitch gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_pitch_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_PITCH_PARAM_SEMITONES = 0,
  OMX_PITCH_PARAM_CENTS = 1,
  OMX_PITCH_PARAM_MIX = 2,
  OMX_PITCH_PARAM_COUNT = 3
};

static const omx_plugin_param OMX_PITCH_PARAMS[OMX_PITCH_PARAM_COUNT] = {
  { "semitones", "Semitones", "st", -12.0f, 12.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "cents", "Cents", "cents", -50.0f, 50.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "mix", "Mix", "%", 0.0f, 100.0f, 100.0f, 0u },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_PITCH_PARAM_MODULES[OMX_PITCH_PARAM_COUNT] = {
  "Pitch",
  "Pitch",
  "Pitch",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_PITCH_PARAM_SEMITONES_MIN -12.0f
#define OMX_PITCH_PARAM_SEMITONES_MAX 12.0f
#define OMX_PITCH_PARAM_SEMITONES_DEFAULT 0.0f
#define OMX_PITCH_PARAM_CENTS_MIN -50.0f
#define OMX_PITCH_PARAM_CENTS_MAX 50.0f
#define OMX_PITCH_PARAM_CENTS_DEFAULT 0.0f
#define OMX_PITCH_PARAM_MIX_MIN 0.0f
#define OMX_PITCH_PARAM_MIX_MAX 100.0f
#define OMX_PITCH_PARAM_MIX_DEFAULT 100.0f

/* The identity every face publishes. */
#define OMX_PITCH_NAME "omx pitch"
#define OMX_PITCH_VENDOR "openmixer"
#define OMX_PITCH_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_PITCH_VERSION "0.19.2"
#define OMX_PITCH_DESCRIPTION "The console's pitch shifter: shifts both channels up or down by semitones and cents, without changing their length, mixed with the dry signal."
#define OMX_PITCH_CLAP_ID "org.openmixer.pitch"
#define OMX_PITCH_CLAP_FEATURES "audio-effect", "pitch-shifter", "stereo"
#define OMX_PITCH_LV2_URI "urn:openmixer:pitch"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_PITCH_DECL_SOURCE "omx pitch: Semitones -12 to 12 st, Cents -50 to 50 cents, Mix 0 to 100 %"
#define OMX_PITCH_DECL_DIGEST "1147fee9b0992b434223331f065cdfacf96577f00fb64751d0c42aca706448b6"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_PITCH_PARAM_CONTRACT_VERSION 2u
#define OMX_PITCH_PARAM_CONTRACT_FINGERPRINT "38bfaa5502a46fd716a79b1d4ab2946be65ec549fcbc54d2b4e575e4d6a3ffaf"
static const struct omx_param_contract_row OMX_PITCH_PARAM_CONTRACT[OMX_PITCH_PARAM_COUNT] = {
  { 22u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* semitones semitone12TET */
  { 21u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* cents cent */
  { 14u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* mix pc */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_PITCH_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family pitch. */
#define OMX_PITCH_PANEL_FINGERPRINT "346afbde3998f87326a46a344219d7df8ad9f356771ec81454a5f88ac31093ce"
static const struct omx_panel_section_row OMX_PITCH_PANEL_SECTIONS[] = {
  { "pitch", "fx.pitch.section.pitch", "Pitch" },
};
static const struct omx_panel_control_row OMX_PITCH_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.pitch.semitones", "Semitones", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.pitch.cents", "Cents", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.pitch.mix", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_PITCH_PANEL_ROLES[] = {
  { "shift", 0u, 0u },
  { "fine", 0u, 1u },
  { "mix", 0u, 2u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_PITCH_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_PITCH_PANEL_PAGE_COUNT 1u
static const struct omx_panel_rows OMX_PITCH_PANEL = {
  OMX_PITCH_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_PITCH,
  OMX_PITCH_PANEL_SECTIONS, 1u,
  OMX_PITCH_PANEL_CONTROLS, 3u,
  OMX_PITCH_PANEL_ROLES, 3u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_PITCH_LV2_PORT_ENABLED 0u
#define OMX_PITCH_LV2_PORT_LATENCY 1u
#define OMX_PITCH_LV2_PORT_IN_L 2u
#define OMX_PITCH_LV2_PORT_IN_R 3u
#define OMX_PITCH_LV2_PORT_OUT_L 4u
#define OMX_PITCH_LV2_PORT_OUT_R 5u
#define OMX_PITCH_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_PITCH_LV2_PORT_FIRST_METER 9u
#define OMX_PITCH_LV2_PORT_COUNT 9u

#endif /* OMX_PITCH_PARAMS_H */
