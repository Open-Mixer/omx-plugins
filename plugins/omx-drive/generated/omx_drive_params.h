// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_DRIVE_PARAMS_H
#define OMX_DRIVE_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-drive/omx-drive.decl.json.
 * Regenerate: `make -C plugins/omx-drive gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_drive_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_DRIVE_PARAM_AMOUNT = 0,
  OMX_DRIVE_PARAM_CHARACTER = 1,
  OMX_DRIVE_PARAM_BAND_FREQ = 2,
  OMX_DRIVE_PARAM_MIX = 3,
  OMX_DRIVE_PARAM_TRIM = 4,
  OMX_DRIVE_PARAM_CURVE = 5,
  OMX_DRIVE_PARAM_BAND = 6,
  OMX_DRIVE_PARAM_AUTO_GAIN = 7,
  OMX_DRIVE_PARAM_STEREO_LINK = 8,
  OMX_DRIVE_PARAM_HF_ROLLOFF = 9,
  OMX_DRIVE_PARAM_OVERSAMPLE = 10,
  OMX_DRIVE_PARAM_COUNT = 11
};

static const omx_plugin_param OMX_DRIVE_PARAMS[OMX_DRIVE_PARAM_COUNT] = {
  { "amount", "Drive", "dB", 0.0f, 36.0f, 0.0f, 0u },
  { "character", "Character", "", -1.0f, 1.0f, 0.0f, 0u },
  { "bandFreq", "Band Frequency", "Hz", 20.0f, 20000.0f, 2000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "mix", "Mix", "%", 0.0f, 100.0f, 100.0f, 0u },
  { "trim", "Trim", "dB", -24.0f, 12.0f, 0.0f, 0u },
  { "curve", "Curve", "", 0.0f, 3.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "band", "Band", "", 0.0f, 3.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "autoGain", "Auto Gain", "", 0.0f, 1.0f, 1.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "stereoLink", "Stereo Link", "", 0.0f, 1.0f, 1.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "hfRolloff", "HF Roll-off", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "oversample", "Oversampling", "", 1.0f, 4.0f, 4.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_DRIVE_PARAM_MODULES[OMX_DRIVE_PARAM_COUNT] = {
  "Drive",
  "Drive",
  "Drive",
  "Drive",
  "Drive",
  "Drive",
  "Drive",
  "Drive",
  "Drive",
  "Drive",
  "Drive",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_DRIVE_PARAM_AMOUNT_MIN 0.0f
#define OMX_DRIVE_PARAM_AMOUNT_MAX 36.0f
#define OMX_DRIVE_PARAM_AMOUNT_DEFAULT 0.0f
#define OMX_DRIVE_PARAM_CHARACTER_MIN -1.0f
#define OMX_DRIVE_PARAM_CHARACTER_MAX 1.0f
#define OMX_DRIVE_PARAM_CHARACTER_DEFAULT 0.0f
#define OMX_DRIVE_PARAM_BAND_FREQ_MIN 20.0f
#define OMX_DRIVE_PARAM_BAND_FREQ_MAX 20000.0f
#define OMX_DRIVE_PARAM_BAND_FREQ_DEFAULT 2000.0f
#define OMX_DRIVE_PARAM_MIX_MIN 0.0f
#define OMX_DRIVE_PARAM_MIX_MAX 100.0f
#define OMX_DRIVE_PARAM_MIX_DEFAULT 100.0f
#define OMX_DRIVE_PARAM_TRIM_MIN -24.0f
#define OMX_DRIVE_PARAM_TRIM_MAX 12.0f
#define OMX_DRIVE_PARAM_TRIM_DEFAULT 0.0f
#define OMX_DRIVE_PARAM_CURVE_MIN 0.0f
#define OMX_DRIVE_PARAM_CURVE_MAX 3.0f
#define OMX_DRIVE_PARAM_CURVE_DEFAULT 0.0f
#define OMX_DRIVE_PARAM_BAND_MIN 0.0f
#define OMX_DRIVE_PARAM_BAND_MAX 3.0f
#define OMX_DRIVE_PARAM_BAND_DEFAULT 0.0f
#define OMX_DRIVE_PARAM_AUTO_GAIN_MIN 0.0f
#define OMX_DRIVE_PARAM_AUTO_GAIN_MAX 1.0f
#define OMX_DRIVE_PARAM_AUTO_GAIN_DEFAULT 1.0f
#define OMX_DRIVE_PARAM_STEREO_LINK_MIN 0.0f
#define OMX_DRIVE_PARAM_STEREO_LINK_MAX 1.0f
#define OMX_DRIVE_PARAM_STEREO_LINK_DEFAULT 1.0f
#define OMX_DRIVE_PARAM_HF_ROLLOFF_MIN 0.0f
#define OMX_DRIVE_PARAM_HF_ROLLOFF_MAX 2.0f
#define OMX_DRIVE_PARAM_HF_ROLLOFF_DEFAULT 0.0f
#define OMX_DRIVE_PARAM_OVERSAMPLE_MIN 1.0f
#define OMX_DRIVE_PARAM_OVERSAMPLE_MAX 4.0f
#define OMX_DRIVE_PARAM_OVERSAMPLE_DEFAULT 4.0f

/* The identity every face publishes. */
#define OMX_DRIVE_NAME "omx drive"
#define OMX_DRIVE_VENDOR "openmixer"
#define OMX_DRIVE_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_DRIVE_VERSION "0.19.2"
#define OMX_DRIVE_DESCRIPTION "The console's drive: one waveshaper (soft, tape, tube or exciter) run inside the console's oversampler, on the full band, the lows, the highs or a tilt, with a wet/dry mix and a trim."
#define OMX_DRIVE_CLAP_ID "org.openmixer.drive"
#define OMX_DRIVE_CLAP_FEATURES "audio-effect", "distortion", "stereo"
#define OMX_DRIVE_LV2_URI "urn:openmixer:drive"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_DRIVE_DECL_SOURCE "omx drive: Drive 0 to 36 dB, Character -1 to 1, Band Frequency 20 to 20000 Hz, Mix 0 to 100 %, Trim -24 to 12 dB, Curve 0 to 3, Band 0 to 3, Auto Gain 0 to 1, Stereo Link 0 to 1, HF Roll-off 0 to 2, Oversampling 1 to 4"
#define OMX_DRIVE_DECL_DIGEST "0ace5ddc34e47ca5e4258c820757c6b1d5dc0c2172e8e71c39b036347f8e6e0c"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_DRIVE_PARAM_CONTRACT_VERSION 2u
#define OMX_DRIVE_PARAM_CONTRACT_FINGERPRINT "1a35b3fb687b7421dd51cdeb9fa4947172c628c8164a4f76a3f88bfaf9b17a8e"
static const struct omx_param_contract_row OMX_DRIVE_PARAM_CONTRACT[OMX_DRIVE_PARAM_COUNT] = {
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* amount db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* character none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* bandFreq hz */
  { 14u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* mix pc */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* trim db */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* curve none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* band none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* autoGain none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* stereoLink none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* hfRolloff none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* oversample none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_DRIVE_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family drive. */
#define OMX_DRIVE_PANEL_FINGERPRINT "be6473460e82fb3bb4299f5763201893eaf9394e6c4594c68c305d41625a48bb"
static const struct omx_panel_section_row OMX_DRIVE_PANEL_SECTIONS[] = {
  { "drive", "fx.drive.section.drive", "Drive" },
};
static const struct omx_panel_control_row OMX_DRIVE_PANEL_CONTROLS[] = {
  { 5u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.drive.curve", "Curve", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.drive.band", "Band", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 9u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.drive.hf_rolloff", "HF Roll-off", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 10u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.drive.oversample", "Oversampling", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 0u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.drive.amount", "Drive", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.drive.character", "Character", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.drive.band_freq", "Band Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.drive.mix", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.drive.trim", "Trim", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.drive.auto_gain", "Auto Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 8u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.drive.stereo_link", "Stereo Link", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_DRIVE_PANEL_ROLES[] = {
  { "drive", 0u, 0u },
  { "tone", 0u, 1u },
  { "model", 0u, 5u },
  { "mix", 0u, 3u },
  { "output", 0u, 4u },
};
static const struct omx_panel_display_row OMX_DRIVE_PANEL_DISPLAYS[] = {
  { OMX_CLAP_DISPLAY_TRANSFER_CURVE, 0u, OMX_CLAP_DISPLAY_FROM_FAMILY, { OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.drive.display.transfer_curve", "Drive" },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_DRIVE_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 1u, 5u, 3u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Drive", "Drive", { 6u, 9u, 10u, 2u, 4u, 7u, 8u, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_DRIVE_PANEL_PAGE_COUNT 2u
static const struct omx_panel_rows OMX_DRIVE_PANEL = {
  OMX_DRIVE_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_DRIVE,
  OMX_DRIVE_PANEL_SECTIONS, 1u,
  OMX_DRIVE_PANEL_CONTROLS, 11u,
  OMX_DRIVE_PANEL_ROLES, 5u,
  OMX_DRIVE_PANEL_DISPLAYS, 1u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_DRIVE_LV2_PORT_ENABLED 0u
#define OMX_DRIVE_LV2_PORT_LATENCY 1u
#define OMX_DRIVE_LV2_PORT_IN_L 2u
#define OMX_DRIVE_LV2_PORT_IN_R 3u
#define OMX_DRIVE_LV2_PORT_OUT_L 4u
#define OMX_DRIVE_LV2_PORT_OUT_R 5u
#define OMX_DRIVE_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_DRIVE_LV2_PORT_FIRST_METER 17u
#define OMX_DRIVE_LV2_PORT_COUNT 17u

#endif /* OMX_DRIVE_PARAMS_H */
