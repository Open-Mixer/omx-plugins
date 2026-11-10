// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_GEQ_PARAMS_H
#define OMX_GEQ_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-geq/omx-geq.decl.json.
 * Regenerate: `make -C plugins/omx-geq gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_geq_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_GEQ_PARAM_BAND01 = 0,
  OMX_GEQ_PARAM_BAND02 = 1,
  OMX_GEQ_PARAM_BAND03 = 2,
  OMX_GEQ_PARAM_BAND04 = 3,
  OMX_GEQ_PARAM_BAND05 = 4,
  OMX_GEQ_PARAM_BAND06 = 5,
  OMX_GEQ_PARAM_BAND07 = 6,
  OMX_GEQ_PARAM_BAND08 = 7,
  OMX_GEQ_PARAM_BAND09 = 8,
  OMX_GEQ_PARAM_BAND10 = 9,
  OMX_GEQ_PARAM_BAND11 = 10,
  OMX_GEQ_PARAM_BAND12 = 11,
  OMX_GEQ_PARAM_BAND13 = 12,
  OMX_GEQ_PARAM_BAND14 = 13,
  OMX_GEQ_PARAM_BAND15 = 14,
  OMX_GEQ_PARAM_BAND16 = 15,
  OMX_GEQ_PARAM_BAND17 = 16,
  OMX_GEQ_PARAM_BAND18 = 17,
  OMX_GEQ_PARAM_BAND19 = 18,
  OMX_GEQ_PARAM_BAND20 = 19,
  OMX_GEQ_PARAM_BAND21 = 20,
  OMX_GEQ_PARAM_BAND22 = 21,
  OMX_GEQ_PARAM_BAND23 = 22,
  OMX_GEQ_PARAM_BAND24 = 23,
  OMX_GEQ_PARAM_BAND25 = 24,
  OMX_GEQ_PARAM_BAND26 = 25,
  OMX_GEQ_PARAM_BAND27 = 26,
  OMX_GEQ_PARAM_BAND28 = 27,
  OMX_GEQ_PARAM_BAND29 = 28,
  OMX_GEQ_PARAM_BAND30 = 29,
  OMX_GEQ_PARAM_BAND31 = 30,
  OMX_GEQ_PARAM_COUNT = 31
};

static const omx_plugin_param OMX_GEQ_PARAMS[OMX_GEQ_PARAM_COUNT] = {
  { "band01", "20 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band02", "25 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band03", "31.5 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band04", "40 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band05", "50 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band06", "63 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band07", "80 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band08", "100 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band09", "125 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band10", "160 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band11", "200 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band12", "250 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band13", "315 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band14", "400 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band15", "500 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band16", "630 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band17", "800 Hz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band18", "1 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band19", "1.25 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band20", "1.6 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band21", "2 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band22", "2.5 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band23", "3.15 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band24", "4 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band25", "5 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band26", "6.3 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band27", "8 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band28", "10 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band29", "12.5 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band30", "16 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "band31", "20 kHz", "dB", -15.0f, 15.0f, 0.0f, 0u },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_GEQ_PARAM_MODULES[OMX_GEQ_PARAM_COUNT] = {
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
  "Geq",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_GEQ_PARAM_BAND01_MIN -15.0f
#define OMX_GEQ_PARAM_BAND01_MAX 15.0f
#define OMX_GEQ_PARAM_BAND01_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND02_MIN -15.0f
#define OMX_GEQ_PARAM_BAND02_MAX 15.0f
#define OMX_GEQ_PARAM_BAND02_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND03_MIN -15.0f
#define OMX_GEQ_PARAM_BAND03_MAX 15.0f
#define OMX_GEQ_PARAM_BAND03_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND04_MIN -15.0f
#define OMX_GEQ_PARAM_BAND04_MAX 15.0f
#define OMX_GEQ_PARAM_BAND04_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND05_MIN -15.0f
#define OMX_GEQ_PARAM_BAND05_MAX 15.0f
#define OMX_GEQ_PARAM_BAND05_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND06_MIN -15.0f
#define OMX_GEQ_PARAM_BAND06_MAX 15.0f
#define OMX_GEQ_PARAM_BAND06_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND07_MIN -15.0f
#define OMX_GEQ_PARAM_BAND07_MAX 15.0f
#define OMX_GEQ_PARAM_BAND07_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND08_MIN -15.0f
#define OMX_GEQ_PARAM_BAND08_MAX 15.0f
#define OMX_GEQ_PARAM_BAND08_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND09_MIN -15.0f
#define OMX_GEQ_PARAM_BAND09_MAX 15.0f
#define OMX_GEQ_PARAM_BAND09_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND10_MIN -15.0f
#define OMX_GEQ_PARAM_BAND10_MAX 15.0f
#define OMX_GEQ_PARAM_BAND10_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND11_MIN -15.0f
#define OMX_GEQ_PARAM_BAND11_MAX 15.0f
#define OMX_GEQ_PARAM_BAND11_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND12_MIN -15.0f
#define OMX_GEQ_PARAM_BAND12_MAX 15.0f
#define OMX_GEQ_PARAM_BAND12_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND13_MIN -15.0f
#define OMX_GEQ_PARAM_BAND13_MAX 15.0f
#define OMX_GEQ_PARAM_BAND13_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND14_MIN -15.0f
#define OMX_GEQ_PARAM_BAND14_MAX 15.0f
#define OMX_GEQ_PARAM_BAND14_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND15_MIN -15.0f
#define OMX_GEQ_PARAM_BAND15_MAX 15.0f
#define OMX_GEQ_PARAM_BAND15_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND16_MIN -15.0f
#define OMX_GEQ_PARAM_BAND16_MAX 15.0f
#define OMX_GEQ_PARAM_BAND16_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND17_MIN -15.0f
#define OMX_GEQ_PARAM_BAND17_MAX 15.0f
#define OMX_GEQ_PARAM_BAND17_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND18_MIN -15.0f
#define OMX_GEQ_PARAM_BAND18_MAX 15.0f
#define OMX_GEQ_PARAM_BAND18_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND19_MIN -15.0f
#define OMX_GEQ_PARAM_BAND19_MAX 15.0f
#define OMX_GEQ_PARAM_BAND19_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND20_MIN -15.0f
#define OMX_GEQ_PARAM_BAND20_MAX 15.0f
#define OMX_GEQ_PARAM_BAND20_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND21_MIN -15.0f
#define OMX_GEQ_PARAM_BAND21_MAX 15.0f
#define OMX_GEQ_PARAM_BAND21_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND22_MIN -15.0f
#define OMX_GEQ_PARAM_BAND22_MAX 15.0f
#define OMX_GEQ_PARAM_BAND22_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND23_MIN -15.0f
#define OMX_GEQ_PARAM_BAND23_MAX 15.0f
#define OMX_GEQ_PARAM_BAND23_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND24_MIN -15.0f
#define OMX_GEQ_PARAM_BAND24_MAX 15.0f
#define OMX_GEQ_PARAM_BAND24_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND25_MIN -15.0f
#define OMX_GEQ_PARAM_BAND25_MAX 15.0f
#define OMX_GEQ_PARAM_BAND25_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND26_MIN -15.0f
#define OMX_GEQ_PARAM_BAND26_MAX 15.0f
#define OMX_GEQ_PARAM_BAND26_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND27_MIN -15.0f
#define OMX_GEQ_PARAM_BAND27_MAX 15.0f
#define OMX_GEQ_PARAM_BAND27_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND28_MIN -15.0f
#define OMX_GEQ_PARAM_BAND28_MAX 15.0f
#define OMX_GEQ_PARAM_BAND28_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND29_MIN -15.0f
#define OMX_GEQ_PARAM_BAND29_MAX 15.0f
#define OMX_GEQ_PARAM_BAND29_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND30_MIN -15.0f
#define OMX_GEQ_PARAM_BAND30_MAX 15.0f
#define OMX_GEQ_PARAM_BAND30_DEFAULT 0.0f
#define OMX_GEQ_PARAM_BAND31_MIN -15.0f
#define OMX_GEQ_PARAM_BAND31_MAX 15.0f
#define OMX_GEQ_PARAM_BAND31_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_GEQ_NAME "omx geq"
#define OMX_GEQ_VENDOR "openmixer"
#define OMX_GEQ_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_GEQ_VERSION "0.19.2"
#define OMX_GEQ_DESCRIPTION "The console's 31-band graphic EQ: one fader per ISO third-octave band from 20 Hz to 20 kHz, each cutting or boosting by up to 15 dB."
#define OMX_GEQ_CLAP_ID "org.openmixer.geq"
#define OMX_GEQ_CLAP_FEATURES "audio-effect", "equalizer", "stereo"
#define OMX_GEQ_LV2_URI "urn:openmixer:geq"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_GEQ_DECL_SOURCE "omx geq: 20 Hz -15 to 15 dB, 25 Hz -15 to 15 dB, 31.5 Hz -15 to 15 dB, 40 Hz -15 to 15 dB, 50 Hz -15 to 15 dB, 63 Hz -15 to 15 dB, 80 Hz -15 to 15 dB, 100 Hz -15 to 15 dB, 125 Hz -15 to 15 dB, 160 Hz -15 to 15 dB, 200 Hz -15 to 15 dB, 250 Hz -15 to 15 dB, 315 Hz -15 to 15 dB, 400 Hz -15 to 15 dB, 500 Hz -15 to 15 dB, 630 Hz -15 to 15 dB, 800 Hz -15 to 15 dB, 1 kHz -15 to 15 dB, 1.25 kHz -15 to 15 dB, 1.6 kHz -15 to 15 dB, 2 kHz -15 to 15 dB, 2.5 kHz -15 to 15 dB, 3.15 kHz -15 to 15 dB, 4 kHz -15 to 15 dB, 5 kHz -15 to 15 dB, 6.3 kHz -15 to 15 dB, 8 kHz -15 to 15 dB, 10 kHz -15 to 15 dB, 12.5 kHz -15 to 15 dB, 16 kHz -15 to 15 dB, 20 kHz -15 to 15 dB"
#define OMX_GEQ_DECL_DIGEST "d0a6a22360a6ce7914f59d4b3eb245c16250f1a772ad308a64bbe05698fe2f48"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_GEQ_PARAM_CONTRACT_VERSION 2u
#define OMX_GEQ_PARAM_CONTRACT_FINGERPRINT "ceff1b2ff48a000abfc1b36aa9bae9d3ebf21f7b192230848fc84f398b9b1317"
static const struct omx_param_contract_row OMX_GEQ_PARAM_CONTRACT[OMX_GEQ_PARAM_COUNT] = {
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band01 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band02 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band03 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band04 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band05 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band06 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band07 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band08 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band09 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band10 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band11 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band12 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band13 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band14 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band15 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band16 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band17 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band18 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band19 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band20 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band21 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band22 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band23 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band24 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band25 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band26 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band27 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band28 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band29 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band30 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* band31 db */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_GEQ_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family eq. */
#define OMX_GEQ_PANEL_FINGERPRINT "bd8501fcc10dc6a789f5c5e71680518fa481a00214f38740cde478fc5514aa88"
static const struct omx_panel_section_row OMX_GEQ_PANEL_SECTIONS[] = {
  { "geq", "fx.geq.section.geq", "Geq" },
};
static const struct omx_panel_control_row OMX_GEQ_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band01", "20 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band02", "25 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band03", "31.5 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band04", "40 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band05", "50 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band06", "63 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band07", "80 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band08", "100 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 8u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band09", "125 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 9u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band10", "160 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 10u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band11", "200 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 11u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band12", "250 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 12u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band13", "315 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 13u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band14", "400 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 14u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band15", "500 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 15u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band16", "630 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 16u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band17", "800 Hz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 17u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band18", "1 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 18u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band19", "1.25 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 19u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band20", "1.6 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 20u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band21", "2 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 21u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band22", "2.5 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 22u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band23", "3.15 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 23u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band24", "4 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 24u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band25", "5 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 25u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band26", "6.3 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 26u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band27", "8 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 27u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band28", "10 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 28u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band29", "12.5 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 29u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band30", "16 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 30u, 0u, OMX_CLAP_WIDGET_FADER, "fx.geq.band31", "20 kHz", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_GEQ_PANEL_ROLES[] = {
  { "band_gain", 0u, 0u },
  { "band_gain", 1u, 1u },
  { "band_gain", 2u, 2u },
  { "band_gain", 3u, 3u },
  { "band_gain", 4u, 4u },
  { "band_gain", 5u, 5u },
  { "band_gain", 6u, 6u },
  { "band_gain", 7u, 7u },
  { "band_gain", 8u, 8u },
  { "band_gain", 9u, 9u },
  { "band_gain", 10u, 10u },
  { "band_gain", 11u, 11u },
  { "band_gain", 12u, 12u },
  { "band_gain", 13u, 13u },
  { "band_gain", 14u, 14u },
  { "band_gain", 15u, 15u },
  { "band_gain", 16u, 16u },
  { "band_gain", 17u, 17u },
  { "band_gain", 18u, 18u },
  { "band_gain", 19u, 19u },
  { "band_gain", 20u, 20u },
  { "band_gain", 21u, 21u },
  { "band_gain", 22u, 22u },
  { "band_gain", 23u, 23u },
  { "band_gain", 24u, 24u },
  { "band_gain", 25u, 25u },
  { "band_gain", 26u, 26u },
  { "band_gain", 27u, 27u },
  { "band_gain", 28u, 28u },
  { "band_gain", 29u, 29u },
  { "band_gain", 30u, 30u },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_GEQ_PANEL_PAGES[] = {
  { "Easy", "Easy", { 0u, 6u, 12u, 18u, 24u, 30u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Geq", "Geq: 25 Hz - 160 Hz", { 1u, 2u, 3u, 4u, 5u, 7u, 8u, 9u } },
  { "Geq", "Geq: 200 Hz - 1.6 kHz", { 10u, 11u, 13u, 14u, 15u, 16u, 17u, 19u } },
  { "Geq", "Geq: 2 kHz - 12.5 kHz", { 20u, 21u, 22u, 23u, 25u, 26u, 27u, 28u } },
  { "Geq", "Geq: 16 kHz", { 29u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_GEQ_PANEL_PAGE_COUNT 5u
static const struct omx_panel_rows OMX_GEQ_PANEL = {
  OMX_GEQ_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_EQ,
  OMX_GEQ_PANEL_SECTIONS, 1u,
  OMX_GEQ_PANEL_CONTROLS, 31u,
  OMX_GEQ_PANEL_ROLES, 31u,
  NULL, 0u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_GEQ_LV2_PORT_ENABLED 0u
#define OMX_GEQ_LV2_PORT_LATENCY 1u
#define OMX_GEQ_LV2_PORT_IN_L 2u
#define OMX_GEQ_LV2_PORT_IN_R 3u
#define OMX_GEQ_LV2_PORT_OUT_L 4u
#define OMX_GEQ_LV2_PORT_OUT_R 5u
#define OMX_GEQ_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_GEQ_LV2_PORT_FIRST_METER 37u
#define OMX_GEQ_LV2_PORT_COUNT 37u

#endif /* OMX_GEQ_PARAMS_H */
