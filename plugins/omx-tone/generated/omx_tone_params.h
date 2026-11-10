// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_TONE_PARAMS_H
#define OMX_TONE_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-tone/omx-tone.decl.json.
 * Regenerate: `make -C plugins/omx-tone gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_tone_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_TONE_PARAM_TILT_DB = 0,
  OMX_TONE_PARAM_PIVOT_HZ = 1,
  OMX_TONE_PARAM_FILTER = 2,
  OMX_TONE_PARAM_RESONANCE = 3,
  OMX_TONE_PARAM_COUNT = 4
};

static const omx_plugin_param OMX_TONE_PARAMS[OMX_TONE_PARAM_COUNT] = {
  { "tiltDb", "Tilt", "dB", -6.0f, 6.0f, 0.0f, 0u },
  { "pivotHz", "Pivot", "Hz", 200.0f, 5000.0f, 1000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "filter", "DJ Filter", "", -100.0f, 100.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "resonance", "Resonance", "", 0.5f, 4.0f, 0.71f, 0u },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_TONE_PARAM_MODULES[OMX_TONE_PARAM_COUNT] = {
  "Tilt",
  "Tilt",
  "DJ Filter",
  "DJ Filter",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_TONE_PARAM_TILT_DB_MIN -6.0f
#define OMX_TONE_PARAM_TILT_DB_MAX 6.0f
#define OMX_TONE_PARAM_TILT_DB_DEFAULT 0.0f
#define OMX_TONE_PARAM_PIVOT_HZ_MIN 200.0f
#define OMX_TONE_PARAM_PIVOT_HZ_MAX 5000.0f
#define OMX_TONE_PARAM_PIVOT_HZ_DEFAULT 1000.0f
#define OMX_TONE_PARAM_FILTER_MIN -100.0f
#define OMX_TONE_PARAM_FILTER_MAX 100.0f
#define OMX_TONE_PARAM_FILTER_DEFAULT 0.0f
#define OMX_TONE_PARAM_RESONANCE_MIN 0.5f
#define OMX_TONE_PARAM_RESONANCE_MAX 4.0f
#define OMX_TONE_PARAM_RESONANCE_DEFAULT 0.71f

/* The identity every face publishes. */
#define OMX_TONE_NAME "omx tone"
#define OMX_TONE_VENDOR "openmixer"
#define OMX_TONE_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_TONE_VERSION "0.19.2"
#define OMX_TONE_DESCRIPTION "Tilts the whole tone darker or brighter about a pivot, and sweeps a resonant DJ filter from a low-pass through open to a high-pass with one knob."
#define OMX_TONE_CLAP_ID "org.openmixer.tone"
#define OMX_TONE_CLAP_FEATURES "audio-effect", "filter", "stereo"
#define OMX_TONE_LV2_URI "urn:openmixer:tone"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_TONE_DECL_SOURCE "omx tone: Tilt -6 to 6 dB, Pivot 200 to 5000 Hz, DJ Filter -100 to 100, Resonance 0.5 to 4"
#define OMX_TONE_DECL_DIGEST "98bc8d37e1cb6e843ad5cae6db1e41273e996044dc425c5aa435f945ed96656e"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), is the face's. */
#define OMX_TONE_PARAM_CONTRACT_VERSION 1u
#define OMX_TONE_PARAM_CONTRACT_FINGERPRINT "33df5e703efc53d22079c985ffd9a121f845f316e6e5ca997e5d2fceb5a16816"
static const struct omx_param_contract_row OMX_TONE_PARAM_CONTRACT[OMX_TONE_PARAM_COUNT] = {
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* tiltDb db */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* pivotHz hz */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* filter none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* resonance none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_TONE_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family filter.
 * Declared roles not published:
 *   cutoff (filter): the cutoff role takes hz, khz, not no unit */
#define OMX_TONE_PANEL_FINGERPRINT "cbd169d6af2f1e750a148ed7b1cca60644e10dfd30d7c13853e008092d7e4daa"
static const struct omx_panel_section_row OMX_TONE_PANEL_SECTIONS[] = {
  { "tilt", "fx.tone.section.tilt", "Tilt" },
  { "djfilter", "fx.tone.section.djfilter", "DJ Filter" },
};
static const struct omx_panel_control_row OMX_TONE_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.tone.tilt_db", "Tilt", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.tone.pivot_hz", "Pivot", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.tone.filter", "DJ Filter", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.tone.resonance", "Resonance", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_TONE_PANEL_ROLES[] = {
  { "resonance", 0u, 3u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_TONE_PANEL_PAGES[] = {
  { "Easy", "Easy", { 3u, 0u, 1u, 2u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_TONE_PANEL_PAGE_COUNT 1u
static const struct omx_panel_rows OMX_TONE_PANEL = {
  OMX_TONE_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_FILTER,
  OMX_TONE_PANEL_SECTIONS, 2u,
  OMX_TONE_PANEL_CONTROLS, 4u,
  OMX_TONE_PANEL_ROLES, 1u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_TONE_LV2_PORT_ENABLED 0u
#define OMX_TONE_LV2_PORT_LATENCY 1u
#define OMX_TONE_LV2_PORT_IN_L 2u
#define OMX_TONE_LV2_PORT_IN_R 3u
#define OMX_TONE_LV2_PORT_OUT_L 4u
#define OMX_TONE_LV2_PORT_OUT_R 5u
#define OMX_TONE_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_TONE_LV2_PORT_FIRST_METER 10u
#define OMX_TONE_LV2_PORT_COUNT 10u

#endif /* OMX_TONE_PARAMS_H */
