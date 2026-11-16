// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_TRIM_PARAMS_H
#define OMX_TRIM_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-trim/omx-trim.decl.json.
 * Regenerate: `make -C plugins/omx-trim gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_trim_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_TRIM_PARAM_TRIM_DB = 0,
  OMX_TRIM_PARAM_COUNT = 1
};

static const omx_plugin_param OMX_TRIM_PARAMS[OMX_TRIM_PARAM_COUNT] = {
  { "trimDb", "Trim", "dB", -24.0f, 24.0f, 0.0f, 0u },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_TRIM_PARAM_MODULES[OMX_TRIM_PARAM_COUNT] = {
  "Trim",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_TRIM_PARAM_TRIM_DB_MIN -24.0f
#define OMX_TRIM_PARAM_TRIM_DB_MAX 24.0f
#define OMX_TRIM_PARAM_TRIM_DB_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_TRIM_NAME "omx trim"
#define OMX_TRIM_VENDOR "openmixer"
#define OMX_TRIM_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_TRIM_VERSION "0.19.3"
#define OMX_TRIM_DESCRIPTION "The OpenMixer console's input trim: one gain control, -24 to +24 dB in 0.1 dB steps, applied first in the channel as a click-free ramp, stereo, zero latency; at 0 dB a racked instance is a wire."
#define OMX_TRIM_CLAP_ID "org.openmixer.trim"
#define OMX_TRIM_CLAP_FEATURES "audio-effect", "mixing", "stereo"
#define OMX_TRIM_LV2_URI "urn:openmixer:trim"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_TRIM_DECL_SOURCE "omx trim: Trim -24 to 24 dB"
#define OMX_TRIM_DECL_DIGEST "565c8315d0e415fe61008704dc8a03f6b0ccff1697d611e3fbd20a46e19456bf"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_TRIM_PARAM_CONTRACT_VERSION 1u
#define OMX_TRIM_PARAM_CONTRACT_FINGERPRINT "d907fd51485f3e84928aa1707841fd45301d7c8db8b5ba499b28d9037a74f6e5"
static const struct omx_param_contract_row OMX_TRIM_PARAM_CONTRACT[OMX_TRIM_PARAM_COUNT] = {
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* trimDb db */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_TRIM_RESOURCE_HINTS = {
  .declared = 0u,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family none. */
#define OMX_TRIM_PANEL_FINGERPRINT "5b03040936b5e2e9dd33e601404791b672610caf533e90c571f9db94d2787446"
static const struct omx_panel_section_row OMX_TRIM_PANEL_SECTIONS[] = {
  { "trim", "fx.trim.section.trim", "Trim" },
};
static const struct omx_panel_control_row OMX_TRIM_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.trim.trim_db", "Trim", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_TRIM_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_TRIM_PANEL_PAGE_COUNT 1u
static const struct omx_panel_rows OMX_TRIM_PANEL = {
  OMX_TRIM_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_NONE,
  OMX_TRIM_PANEL_SECTIONS, 1u,
  OMX_TRIM_PANEL_CONTROLS, 1u,
  NULL, 0u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_TRIM_LV2_PORT_ENABLED 0u
#define OMX_TRIM_LV2_PORT_LATENCY 1u
#define OMX_TRIM_LV2_PORT_IN_L 2u
#define OMX_TRIM_LV2_PORT_IN_R 3u
#define OMX_TRIM_LV2_PORT_OUT_L 4u
#define OMX_TRIM_LV2_PORT_OUT_R 5u
#define OMX_TRIM_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_TRIM_LV2_PORT_FIRST_METER 7u
#define OMX_TRIM_LV2_PORT_COUNT 7u

#endif /* OMX_TRIM_PARAMS_H */
