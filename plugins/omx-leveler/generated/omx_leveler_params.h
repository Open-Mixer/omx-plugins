// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_LEVELER_PARAMS_H
#define OMX_LEVELER_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-leveler/omx-leveler.decl.json.
 * Regenerate: `make -C plugins/omx-leveler gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_leveler_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_LEVELER_PARAM_AMOUNT = 0,
  OMX_LEVELER_PARAM_DIRECTION = 1,
  OMX_LEVELER_PARAM_SPEED = 2,
  OMX_LEVELER_PARAM_COUNT = 3
};

static const omx_plugin_param OMX_LEVELER_PARAMS[OMX_LEVELER_PARAM_COUNT] = {
  { "amount", "Amount", "%", 0.0f, 100.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "direction", "Direction", "", 0.0f, 2.0f, 2.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "speed", "Speed", "", 0.0f, 2.0f, 1.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_LEVELER_PARAM_MODULES[OMX_LEVELER_PARAM_COUNT] = {
  "Leveler",
  "Leveler",
  "Leveler",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_LEVELER_PARAM_AMOUNT_MIN 0.0f
#define OMX_LEVELER_PARAM_AMOUNT_MAX 100.0f
#define OMX_LEVELER_PARAM_AMOUNT_DEFAULT 0.0f
#define OMX_LEVELER_PARAM_DIRECTION_MIN 0.0f
#define OMX_LEVELER_PARAM_DIRECTION_MAX 2.0f
#define OMX_LEVELER_PARAM_DIRECTION_DEFAULT 2.0f
#define OMX_LEVELER_PARAM_SPEED_MIN 0.0f
#define OMX_LEVELER_PARAM_SPEED_MAX 2.0f
#define OMX_LEVELER_PARAM_SPEED_DEFAULT 1.0f

/* The identity every face publishes. */
#define OMX_LEVELER_NAME "omx leveler"
#define OMX_LEVELER_VENDOR "openmixer"
#define OMX_LEVELER_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_LEVELER_VERSION "0.19.3"
#define OMX_LEVELER_DESCRIPTION "Rides a signal's level up, down or both ways, by how much and how fast you set."
#define OMX_LEVELER_CLAP_ID "org.openmixer.leveler"
#define OMX_LEVELER_CLAP_FEATURES "audio-effect"
#define OMX_LEVELER_LV2_URI "urn:openmixer:leveler"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_LEVELER_DECL_SOURCE "omx leveler: Amount 0 to 100 %, Direction 0 to 2, Speed 0 to 2"
#define OMX_LEVELER_DECL_DIGEST "12e606844afd207d09b2519e73940f36db98fdb9712ab6049968e295213c9259"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_LEVELER_PARAM_CONTRACT_VERSION 1u
#define OMX_LEVELER_PARAM_CONTRACT_FINGERPRINT "af9652a09e29334a12a6099e6105ea5a0ab56496439b00645956d899f5a637a7"
static const struct omx_param_contract_row OMX_LEVELER_PARAM_CONTRACT[OMX_LEVELER_PARAM_COUNT] = {
  { 14u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* amount pc */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* direction none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* speed none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_LEVELER_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family none. */
#define OMX_LEVELER_PANEL_FINGERPRINT "a0296c0eb8ebe3af7269076c0a02c4745b48efe2eb154642664de24bbfc51176"
static const struct omx_panel_section_row OMX_LEVELER_PANEL_SECTIONS[] = {
  { "leveler", "fx.leveler.section.leveler", "Leveler" },
};
static const struct omx_panel_control_row OMX_LEVELER_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.leveler.amount", "Amount", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.leveler.direction", "Direction", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.leveler.speed", "Speed", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_LEVELER_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_LEVELER_PANEL_PAGE_COUNT 1u
static const struct omx_panel_rows OMX_LEVELER_PANEL = {
  OMX_LEVELER_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_NONE,
  OMX_LEVELER_PANEL_SECTIONS, 1u,
  OMX_LEVELER_PANEL_CONTROLS, 3u,
  NULL, 0u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_LEVELER_LV2_PORT_ENABLED 0u
#define OMX_LEVELER_LV2_PORT_LATENCY 1u
#define OMX_LEVELER_LV2_PORT_IN_L 2u
#define OMX_LEVELER_LV2_PORT_IN_R 3u
#define OMX_LEVELER_LV2_PORT_OUT_L 4u
#define OMX_LEVELER_LV2_PORT_OUT_R 5u
#define OMX_LEVELER_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_LEVELER_LV2_PORT_FIRST_METER 9u
#define OMX_LEVELER_LV2_PORT_COUNT 9u

#endif /* OMX_LEVELER_PARAMS_H */
