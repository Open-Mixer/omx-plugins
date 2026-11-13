// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_PLOSIVE_PARAMS_H
#define OMX_PLOSIVE_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-plosive/omx-plosive.decl.json.
 * Regenerate: `make -C plugins/omx-plosive gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_plosive_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_PLOSIVE_PARAM_CONSONANT_DB = 0,
  OMX_PLOSIVE_PARAM_SENSITIVITY_DB = 1,
  OMX_PLOSIVE_PARAM_RECOVERY_MS = 2,
  OMX_PLOSIVE_PARAM_BAND_HZ = 3,
  OMX_PLOSIVE_PARAM_BAND_WIDTH_OCT = 4,
  OMX_PLOSIVE_PARAM_POP_DB = 5,
  OMX_PLOSIVE_PARAM_POP_SENSITIVITY_DB = 6,
  OMX_PLOSIVE_PARAM_POP_RECOVERY_MS = 7,
  OMX_PLOSIVE_PARAM_POP_HZ = 8,
  OMX_PLOSIVE_PARAM_COUNT = 9
};

static const omx_plugin_param OMX_PLOSIVE_PARAMS[OMX_PLOSIVE_PARAM_COUNT] = {
  { "consonantDb", "Consonant Cut", "dB", 0.0f, 24.0f, 6.0f, 0u },
  { "sensitivityDb", "Consonant Sensitivity", "dB", 0.0f, 30.0f, 16.0f, 0u },
  { "recoveryMs", "Consonant Recovery", "ms", 2.0f, 50.0f, 8.0f, 0u },
  { "bandHz", "Consonant Band", "Hz", 1000.0f, 12000.0f, 4000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "bandWidthOct", "Consonant Band Width", "oct", 0.5f, 4.0f, 2.0f, 0u },
  { "popDb", "Pop Cut", "dB", 0.0f, 24.0f, 12.0f, 0u },
  { "popSensitivityDb", "Pop Sensitivity", "dB", 0.0f, 30.0f, 16.0f, 0u },
  { "popRecoveryMs", "Pop Recovery", "ms", 10.0f, 200.0f, 40.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "popHz", "Pop Corner", "Hz", 60.0f, 300.0f, 150.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_PLOSIVE_PARAM_MODULES[OMX_PLOSIVE_PARAM_COUNT] = {
  "Plosive",
  "Plosive",
  "Plosive",
  "Plosive",
  "Plosive",
  "Plosive",
  "Plosive",
  "Plosive",
  "Plosive",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_PLOSIVE_PARAM_CONSONANT_DB_MIN 0.0f
#define OMX_PLOSIVE_PARAM_CONSONANT_DB_MAX 24.0f
#define OMX_PLOSIVE_PARAM_CONSONANT_DB_DEFAULT 6.0f
#define OMX_PLOSIVE_PARAM_SENSITIVITY_DB_MIN 0.0f
#define OMX_PLOSIVE_PARAM_SENSITIVITY_DB_MAX 30.0f
#define OMX_PLOSIVE_PARAM_SENSITIVITY_DB_DEFAULT 16.0f
#define OMX_PLOSIVE_PARAM_RECOVERY_MS_MIN 2.0f
#define OMX_PLOSIVE_PARAM_RECOVERY_MS_MAX 50.0f
#define OMX_PLOSIVE_PARAM_RECOVERY_MS_DEFAULT 8.0f
#define OMX_PLOSIVE_PARAM_BAND_HZ_MIN 1000.0f
#define OMX_PLOSIVE_PARAM_BAND_HZ_MAX 12000.0f
#define OMX_PLOSIVE_PARAM_BAND_HZ_DEFAULT 4000.0f
#define OMX_PLOSIVE_PARAM_BAND_WIDTH_OCT_MIN 0.5f
#define OMX_PLOSIVE_PARAM_BAND_WIDTH_OCT_MAX 4.0f
#define OMX_PLOSIVE_PARAM_BAND_WIDTH_OCT_DEFAULT 2.0f
#define OMX_PLOSIVE_PARAM_POP_DB_MIN 0.0f
#define OMX_PLOSIVE_PARAM_POP_DB_MAX 24.0f
#define OMX_PLOSIVE_PARAM_POP_DB_DEFAULT 12.0f
#define OMX_PLOSIVE_PARAM_POP_SENSITIVITY_DB_MIN 0.0f
#define OMX_PLOSIVE_PARAM_POP_SENSITIVITY_DB_MAX 30.0f
#define OMX_PLOSIVE_PARAM_POP_SENSITIVITY_DB_DEFAULT 16.0f
#define OMX_PLOSIVE_PARAM_POP_RECOVERY_MS_MIN 10.0f
#define OMX_PLOSIVE_PARAM_POP_RECOVERY_MS_MAX 200.0f
#define OMX_PLOSIVE_PARAM_POP_RECOVERY_MS_DEFAULT 40.0f
#define OMX_PLOSIVE_PARAM_POP_HZ_MIN 60.0f
#define OMX_PLOSIVE_PARAM_POP_HZ_MAX 300.0f
#define OMX_PLOSIVE_PARAM_POP_HZ_DEFAULT 150.0f

/* The identity every face publishes. */
#define OMX_PLOSIVE_NAME "omx plosive"
#define OMX_PLOSIVE_VENDOR "openmixer"
#define OMX_PLOSIVE_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_PLOSIVE_VERSION "0.19.4"
#define OMX_PLOSIVE_DESCRIPTION "Softens the p pops and hard t and k consonants of a voice, cutting each only while it bursts."
#define OMX_PLOSIVE_CLAP_ID "org.openmixer.plosive"
#define OMX_PLOSIVE_CLAP_FEATURES "audio-effect", "de-esser", "stereo"
#define OMX_PLOSIVE_LV2_URI "urn:openmixer:plosive"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_PLOSIVE_DECL_SOURCE "omx plosive: Consonant Cut 0 to 24 dB, Consonant Sensitivity 0 to 30 dB, Consonant Recovery 2 to 50 ms, Consonant Band 1000 to 12000 Hz, Consonant Band Width 0.5 to 4 oct, Pop Cut 0 to 24 dB, Pop Sensitivity 0 to 30 dB, Pop Recovery 10 to 200 ms, Pop Corner 60 to 300 Hz"
#define OMX_PLOSIVE_DECL_DIGEST "1f7aefaa6ad81d242266a7500ef6518bd97654dee392eaa333bd5b82f1144d59"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_PLOSIVE_PARAM_CONTRACT_VERSION 2u
#define OMX_PLOSIVE_PARAM_CONTRACT_FINGERPRINT "2a41eecfe15389d0caf8ba2240c4b18cef9ddbb34a843f86783be0ea67fe9ea3"
static const struct omx_param_contract_row OMX_PLOSIVE_PARAM_CONTRACT[OMX_PLOSIVE_PARAM_COUNT] = {
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* consonantDb db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* sensitivityDb db */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* recoveryMs ms */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* bandHz hz */
  { 20u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* bandWidthOct oct */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* popDb db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* popSensitivityDb db */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* popRecoveryMs ms */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* popHz hz */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_PLOSIVE_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family none. */
#define OMX_PLOSIVE_PANEL_FINGERPRINT "eb6543f8129e147b31e879d680071590531831c3407b4b34c2b27a80bca54436"
static const struct omx_panel_section_row OMX_PLOSIVE_PANEL_SECTIONS[] = {
  { "plosive", "fx.plosive.section.plosive", "Plosive" },
};
static const struct omx_panel_control_row OMX_PLOSIVE_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.plosive.consonant_db", "Consonant Cut", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.plosive.sensitivity_db", "Consonant Sensitivity", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.plosive.recovery_ms", "Consonant Recovery", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.plosive.band_hz", "Consonant Band", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.plosive.band_width_oct", "Consonant Band Width", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.plosive.pop_db", "Pop Cut", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.plosive.pop_sensitivity_db", "Pop Sensitivity", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.plosive.pop_recovery_ms", "Pop Recovery", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 8u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.plosive.pop_hz", "Pop Corner", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_PLOSIVE_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 2u, 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Plosive", "Plosive", { 4u, 5u, 6u, 7u, 8u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_PLOSIVE_PANEL_PAGE_COUNT 2u
static const struct omx_panel_rows OMX_PLOSIVE_PANEL = {
  OMX_PLOSIVE_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_NONE,
  OMX_PLOSIVE_PANEL_SECTIONS, 1u,
  OMX_PLOSIVE_PANEL_CONTROLS, 9u,
  NULL, 0u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_PLOSIVE_LV2_PORT_ENABLED 0u
#define OMX_PLOSIVE_LV2_PORT_LATENCY 1u
#define OMX_PLOSIVE_LV2_PORT_IN_L 2u
#define OMX_PLOSIVE_LV2_PORT_IN_R 3u
#define OMX_PLOSIVE_LV2_PORT_OUT_L 4u
#define OMX_PLOSIVE_LV2_PORT_OUT_R 5u
#define OMX_PLOSIVE_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_PLOSIVE_LV2_PORT_FIRST_METER 15u
#define OMX_PLOSIVE_LV2_PORT_COUNT 15u

#endif /* OMX_PLOSIVE_PARAMS_H */
