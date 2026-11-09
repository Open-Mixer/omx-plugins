// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_CHORUS_PARAMS_H
#define OMX_CHORUS_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-chorus/omx-chorus.decl.json.
 * Regenerate: `make -C plugins/omx-chorus gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_chorus_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_CHORUS_PARAM_SPREAD = 0,
  OMX_CHORUS_PARAM_RATE = 1,
  OMX_CHORUS_PARAM_DEPTH = 2,
  OMX_CHORUS_PARAM_VOICES = 3,
  OMX_CHORUS_PARAM_MIX = 4,
  OMX_CHORUS_PARAM_COUNT = 5
};

static const omx_plugin_param OMX_CHORUS_PARAMS[OMX_CHORUS_PARAM_COUNT] = {
  { "spread", "Spread", "", 0.0f, 0.5f, 0.0f, 0u },
  { "rate", "Rate", "Hz", 0.05f, 8.0f, 0.6f, 0u },
  { "depth", "Depth", "ms", 0.0f, 12.0f, 4.0f, 0u },
  { "voices", "Voices", "", 1.0f, 4.0f, 3.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "mix", "Mix", "%", 0.0f, 100.0f, 35.0f, 0u },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_CHORUS_PARAM_MODULES[OMX_CHORUS_PARAM_COUNT] = {
  "Chorus",
  "Chorus",
  "Chorus",
  "Chorus",
  "Chorus",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_CHORUS_PARAM_SPREAD_MIN 0.0f
#define OMX_CHORUS_PARAM_SPREAD_MAX 0.5f
#define OMX_CHORUS_PARAM_SPREAD_DEFAULT 0.0f
#define OMX_CHORUS_PARAM_RATE_MIN 0.05f
#define OMX_CHORUS_PARAM_RATE_MAX 8.0f
#define OMX_CHORUS_PARAM_RATE_DEFAULT 0.6f
#define OMX_CHORUS_PARAM_DEPTH_MIN 0.0f
#define OMX_CHORUS_PARAM_DEPTH_MAX 12.0f
#define OMX_CHORUS_PARAM_DEPTH_DEFAULT 4.0f
#define OMX_CHORUS_PARAM_VOICES_MIN 1.0f
#define OMX_CHORUS_PARAM_VOICES_MAX 4.0f
#define OMX_CHORUS_PARAM_VOICES_DEFAULT 3.0f
#define OMX_CHORUS_PARAM_MIX_MIN 0.0f
#define OMX_CHORUS_PARAM_MIX_MAX 100.0f
#define OMX_CHORUS_PARAM_MIX_DEFAULT 35.0f

/* The identity every face publishes. */
#define OMX_CHORUS_NAME "omx chorus"
#define OMX_CHORUS_VENDOR "openmixer"
#define OMX_CHORUS_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_CHORUS_VERSION "0.19.2"
#define OMX_CHORUS_DESCRIPTION "The console's chorus: up to four voices read one modulated delay line, the right leg's sweep offset by the spread, mixed with the dry signal."
#define OMX_CHORUS_CLAP_ID "org.openmixer.chorus"
#define OMX_CHORUS_CLAP_FEATURES "audio-effect", "chorus", "stereo"
#define OMX_CHORUS_LV2_URI "urn:openmixer:chorus"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_CHORUS_DECL_SOURCE "omx chorus: Spread 0 to 0.5, Rate 0.05 to 8 Hz, Depth 0 to 12 ms, Voices 1 to 4, Mix 0 to 100 %"
#define OMX_CHORUS_DECL_DIGEST "7e9d7de96f6b48a792b97d200fa3c98c64f705c253f31637301752d03a5a2e51"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), is the face's. */
#define OMX_CHORUS_PARAM_CONTRACT_VERSION 1u
#define OMX_CHORUS_PARAM_CONTRACT_FINGERPRINT "0ed178cc68e5aec571483b30d59a2e0bbce5baebe61b7b209cb069601584a0ad"
static const struct omx_param_contract_row OMX_CHORUS_PARAM_CONTRACT[OMX_CHORUS_PARAM_COUNT] = {
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* spread none */
  { 16u, OMX_CLAP_SCALE_LOG, 0.0 }, /* rate hz */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* depth ms */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* voices none */
  { 14u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* mix pc */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_CHORUS_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family modulation.
 * Declared roles not published:
 *   depth (depth): the depth role takes none, pc, coef, oct, not ms */
#define OMX_CHORUS_PANEL_FINGERPRINT "f74674fc0273db9c13b04bcefbdde42d9faa30dd64fec72cd7b881bfb1d98b2f"
static const struct omx_panel_section_row OMX_CHORUS_PANEL_SECTIONS[] = {
  { "chorus", "fx.chorus.section.chorus", "Chorus" },
};
static const struct omx_panel_control_row OMX_CHORUS_PANEL_CONTROLS[] = {
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.chorus.rate", "Rate", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.chorus.depth", "Depth", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.chorus.voices", "Voices", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.chorus.mix", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.chorus.spread", "Spread", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_CHORUS_PANEL_ROLES[] = {
  { "rate", 0u, 1u },
  { "voices", 0u, 3u },
  { "spread", 0u, 0u },
  { "mix", 0u, 4u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections. */
static const struct omx_panel_page_row OMX_CHORUS_PANEL_PAGES[] = {
  { "Easy", "Easy", { 1u, 4u, 2u, 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Chorus", "Chorus", { 0u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_CHORUS_PANEL_PAGE_COUNT 2u
static const struct omx_panel_rows OMX_CHORUS_PANEL = {
  OMX_CHORUS_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_MODULATION,
  OMX_CHORUS_PANEL_SECTIONS, 1u,
  OMX_CHORUS_PANEL_CONTROLS, 5u,
  OMX_CHORUS_PANEL_ROLES, 4u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_CHORUS_LV2_PORT_ENABLED 0u
#define OMX_CHORUS_LV2_PORT_LATENCY 1u
#define OMX_CHORUS_LV2_PORT_IN_L 2u
#define OMX_CHORUS_LV2_PORT_IN_R 3u
#define OMX_CHORUS_LV2_PORT_OUT_L 4u
#define OMX_CHORUS_LV2_PORT_OUT_R 5u
#define OMX_CHORUS_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_CHORUS_LV2_PORT_FIRST_METER 11u
#define OMX_CHORUS_LV2_PORT_COUNT 11u

#endif /* OMX_CHORUS_PARAMS_H */
