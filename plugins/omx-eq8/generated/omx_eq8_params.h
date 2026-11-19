// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_EQ8_PARAMS_H
#define OMX_EQ8_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-eq8/omx-eq8.decl.json.
 * Regenerate: `make -C plugins/omx-eq8 gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_eq8_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

/* The band count of this variant of plugins/omx-eq: the instance face's compile-time count. */
#define OMX_EQ_INSTANCE_BANDS OMX_EQ_BAND_COUNTS_EQ8_MAX

enum {
  OMX_EQ8_PARAM_HPF_ON = 0,
  OMX_EQ8_PARAM_HPF_FREQ = 1,
  OMX_EQ8_PARAM_HPF_SLOPE = 2,
  OMX_EQ8_PARAM_LPF_ON = 3,
  OMX_EQ8_PARAM_LPF_FREQ = 4,
  OMX_EQ8_PARAM_LPF_SLOPE = 5,
  OMX_EQ8_PARAM_B1_TYPE = 6,
  OMX_EQ8_PARAM_B1_FREQ = 7,
  OMX_EQ8_PARAM_B1_GAIN = 8,
  OMX_EQ8_PARAM_B1_Q = 9,
  OMX_EQ8_PARAM_B1_ON = 10,
  OMX_EQ8_PARAM_B2_TYPE = 11,
  OMX_EQ8_PARAM_B2_FREQ = 12,
  OMX_EQ8_PARAM_B2_GAIN = 13,
  OMX_EQ8_PARAM_B2_Q = 14,
  OMX_EQ8_PARAM_B2_ON = 15,
  OMX_EQ8_PARAM_B3_TYPE = 16,
  OMX_EQ8_PARAM_B3_FREQ = 17,
  OMX_EQ8_PARAM_B3_GAIN = 18,
  OMX_EQ8_PARAM_B3_Q = 19,
  OMX_EQ8_PARAM_B3_ON = 20,
  OMX_EQ8_PARAM_B4_TYPE = 21,
  OMX_EQ8_PARAM_B4_FREQ = 22,
  OMX_EQ8_PARAM_B4_GAIN = 23,
  OMX_EQ8_PARAM_B4_Q = 24,
  OMX_EQ8_PARAM_B4_ON = 25,
  OMX_EQ8_PARAM_B5_TYPE = 26,
  OMX_EQ8_PARAM_B5_FREQ = 27,
  OMX_EQ8_PARAM_B5_GAIN = 28,
  OMX_EQ8_PARAM_B5_Q = 29,
  OMX_EQ8_PARAM_B5_ON = 30,
  OMX_EQ8_PARAM_B6_TYPE = 31,
  OMX_EQ8_PARAM_B6_FREQ = 32,
  OMX_EQ8_PARAM_B6_GAIN = 33,
  OMX_EQ8_PARAM_B6_Q = 34,
  OMX_EQ8_PARAM_B6_ON = 35,
  OMX_EQ8_PARAM_B7_TYPE = 36,
  OMX_EQ8_PARAM_B7_FREQ = 37,
  OMX_EQ8_PARAM_B7_GAIN = 38,
  OMX_EQ8_PARAM_B7_Q = 39,
  OMX_EQ8_PARAM_B7_ON = 40,
  OMX_EQ8_PARAM_B8_TYPE = 41,
  OMX_EQ8_PARAM_B8_FREQ = 42,
  OMX_EQ8_PARAM_B8_GAIN = 43,
  OMX_EQ8_PARAM_B8_Q = 44,
  OMX_EQ8_PARAM_B8_ON = 45,
  OMX_EQ8_PARAM_COUNT = 46
};

static const omx_plugin_param OMX_EQ8_PARAMS[OMX_EQ8_PARAM_COUNT] = {
  { "hpf_on", "HPF On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "hpf_freq", "HPF Frequency", "Hz", 20.0f, 1000.0f, 80.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "hpf_slope", "HPF Slope", "dB/oct", 12.0f, 24.0f, 12.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "lpf_on", "LPF On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "lpf_freq", "LPF Frequency", "Hz", 1000.0f, 20000.0f, 18000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "lpf_slope", "LPF Slope", "dB/oct", 12.0f, 24.0f, 12.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b1_type", "Band 1 Type", "", 0.0f, 5.0f, 1.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b1_freq", "Band 1 Frequency", "Hz", 20.0f, 20000.0f, 31.5f, OMX_PLUGIN_PARAM_INTEGER },
  { "b1_gain", "Band 1 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b1_q", "Band 1 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b1_on", "Band 1 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b2_type", "Band 2 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b2_freq", "Band 2 Frequency", "Hz", 20.0f, 20000.0f, 80.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b2_gain", "Band 2 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b2_q", "Band 2 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b2_on", "Band 2 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b3_type", "Band 3 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b3_freq", "Band 3 Frequency", "Hz", 20.0f, 20000.0f, 160.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b3_gain", "Band 3 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b3_q", "Band 3 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b3_on", "Band 3 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b4_type", "Band 4 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b4_freq", "Band 4 Frequency", "Hz", 20.0f, 20000.0f, 400.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b4_gain", "Band 4 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b4_q", "Band 4 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b4_on", "Band 4 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b5_type", "Band 5 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b5_freq", "Band 5 Frequency", "Hz", 20.0f, 20000.0f, 1000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b5_gain", "Band 5 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b5_q", "Band 5 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b5_on", "Band 5 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b6_type", "Band 6 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b6_freq", "Band 6 Frequency", "Hz", 20.0f, 20000.0f, 2500.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b6_gain", "Band 6 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b6_q", "Band 6 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b6_on", "Band 6 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b7_type", "Band 7 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b7_freq", "Band 7 Frequency", "Hz", 20.0f, 20000.0f, 5000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b7_gain", "Band 7 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b7_q", "Band 7 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b7_on", "Band 7 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b8_type", "Band 8 Type", "", 0.0f, 5.0f, 2.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b8_freq", "Band 8 Frequency", "Hz", 20.0f, 20000.0f, 12500.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b8_gain", "Band 8 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b8_q", "Band 8 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b8_on", "Band 8 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_EQ8_PARAM_MODULES[OMX_EQ8_PARAM_COUNT] = {
  "HPF",
  "HPF",
  "HPF",
  "LPF",
  "LPF",
  "LPF",
  "Bands/Band 1",
  "Bands/Band 1",
  "Bands/Band 1",
  "Bands/Band 1",
  "Bands/Band 1",
  "Bands/Band 2",
  "Bands/Band 2",
  "Bands/Band 2",
  "Bands/Band 2",
  "Bands/Band 2",
  "Bands/Band 3",
  "Bands/Band 3",
  "Bands/Band 3",
  "Bands/Band 3",
  "Bands/Band 3",
  "Bands/Band 4",
  "Bands/Band 4",
  "Bands/Band 4",
  "Bands/Band 4",
  "Bands/Band 4",
  "Bands/Band 5",
  "Bands/Band 5",
  "Bands/Band 5",
  "Bands/Band 5",
  "Bands/Band 5",
  "Bands/Band 6",
  "Bands/Band 6",
  "Bands/Band 6",
  "Bands/Band 6",
  "Bands/Band 6",
  "Bands/Band 7",
  "Bands/Band 7",
  "Bands/Band 7",
  "Bands/Band 7",
  "Bands/Band 7",
  "Bands/Band 8",
  "Bands/Band 8",
  "Bands/Band 8",
  "Bands/Band 8",
  "Bands/Band 8",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_EQ8_PARAM_HPF_ON_MIN 0.0f
#define OMX_EQ8_PARAM_HPF_ON_MAX 1.0f
#define OMX_EQ8_PARAM_HPF_ON_DEFAULT 0.0f
#define OMX_EQ8_PARAM_HPF_FREQ_MIN 20.0f
#define OMX_EQ8_PARAM_HPF_FREQ_MAX 1000.0f
#define OMX_EQ8_PARAM_HPF_FREQ_DEFAULT 80.0f
#define OMX_EQ8_PARAM_HPF_SLOPE_MIN 12.0f
#define OMX_EQ8_PARAM_HPF_SLOPE_MAX 24.0f
#define OMX_EQ8_PARAM_HPF_SLOPE_DEFAULT 12.0f
#define OMX_EQ8_PARAM_LPF_ON_MIN 0.0f
#define OMX_EQ8_PARAM_LPF_ON_MAX 1.0f
#define OMX_EQ8_PARAM_LPF_ON_DEFAULT 0.0f
#define OMX_EQ8_PARAM_LPF_FREQ_MIN 1000.0f
#define OMX_EQ8_PARAM_LPF_FREQ_MAX 20000.0f
#define OMX_EQ8_PARAM_LPF_FREQ_DEFAULT 18000.0f
#define OMX_EQ8_PARAM_LPF_SLOPE_MIN 12.0f
#define OMX_EQ8_PARAM_LPF_SLOPE_MAX 24.0f
#define OMX_EQ8_PARAM_LPF_SLOPE_DEFAULT 12.0f
#define OMX_EQ8_PARAM_B1_TYPE_MIN 0.0f
#define OMX_EQ8_PARAM_B1_TYPE_MAX 5.0f
#define OMX_EQ8_PARAM_B1_TYPE_DEFAULT 1.0f
#define OMX_EQ8_PARAM_B1_FREQ_MIN 20.0f
#define OMX_EQ8_PARAM_B1_FREQ_MAX 20000.0f
#define OMX_EQ8_PARAM_B1_FREQ_DEFAULT 31.5f
#define OMX_EQ8_PARAM_B1_GAIN_MIN -15.0f
#define OMX_EQ8_PARAM_B1_GAIN_MAX 15.0f
#define OMX_EQ8_PARAM_B1_GAIN_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B1_Q_MIN 0.3f
#define OMX_EQ8_PARAM_B1_Q_MAX 116.0f
#define OMX_EQ8_PARAM_B1_Q_DEFAULT 1.0f
#define OMX_EQ8_PARAM_B1_ON_MIN 0.0f
#define OMX_EQ8_PARAM_B1_ON_MAX 1.0f
#define OMX_EQ8_PARAM_B1_ON_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B2_TYPE_MIN 0.0f
#define OMX_EQ8_PARAM_B2_TYPE_MAX 5.0f
#define OMX_EQ8_PARAM_B2_TYPE_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B2_FREQ_MIN 20.0f
#define OMX_EQ8_PARAM_B2_FREQ_MAX 20000.0f
#define OMX_EQ8_PARAM_B2_FREQ_DEFAULT 80.0f
#define OMX_EQ8_PARAM_B2_GAIN_MIN -15.0f
#define OMX_EQ8_PARAM_B2_GAIN_MAX 15.0f
#define OMX_EQ8_PARAM_B2_GAIN_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B2_Q_MIN 0.3f
#define OMX_EQ8_PARAM_B2_Q_MAX 116.0f
#define OMX_EQ8_PARAM_B2_Q_DEFAULT 1.0f
#define OMX_EQ8_PARAM_B2_ON_MIN 0.0f
#define OMX_EQ8_PARAM_B2_ON_MAX 1.0f
#define OMX_EQ8_PARAM_B2_ON_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B3_TYPE_MIN 0.0f
#define OMX_EQ8_PARAM_B3_TYPE_MAX 5.0f
#define OMX_EQ8_PARAM_B3_TYPE_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B3_FREQ_MIN 20.0f
#define OMX_EQ8_PARAM_B3_FREQ_MAX 20000.0f
#define OMX_EQ8_PARAM_B3_FREQ_DEFAULT 160.0f
#define OMX_EQ8_PARAM_B3_GAIN_MIN -15.0f
#define OMX_EQ8_PARAM_B3_GAIN_MAX 15.0f
#define OMX_EQ8_PARAM_B3_GAIN_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B3_Q_MIN 0.3f
#define OMX_EQ8_PARAM_B3_Q_MAX 116.0f
#define OMX_EQ8_PARAM_B3_Q_DEFAULT 1.0f
#define OMX_EQ8_PARAM_B3_ON_MIN 0.0f
#define OMX_EQ8_PARAM_B3_ON_MAX 1.0f
#define OMX_EQ8_PARAM_B3_ON_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B4_TYPE_MIN 0.0f
#define OMX_EQ8_PARAM_B4_TYPE_MAX 5.0f
#define OMX_EQ8_PARAM_B4_TYPE_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B4_FREQ_MIN 20.0f
#define OMX_EQ8_PARAM_B4_FREQ_MAX 20000.0f
#define OMX_EQ8_PARAM_B4_FREQ_DEFAULT 400.0f
#define OMX_EQ8_PARAM_B4_GAIN_MIN -15.0f
#define OMX_EQ8_PARAM_B4_GAIN_MAX 15.0f
#define OMX_EQ8_PARAM_B4_GAIN_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B4_Q_MIN 0.3f
#define OMX_EQ8_PARAM_B4_Q_MAX 116.0f
#define OMX_EQ8_PARAM_B4_Q_DEFAULT 1.0f
#define OMX_EQ8_PARAM_B4_ON_MIN 0.0f
#define OMX_EQ8_PARAM_B4_ON_MAX 1.0f
#define OMX_EQ8_PARAM_B4_ON_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B5_TYPE_MIN 0.0f
#define OMX_EQ8_PARAM_B5_TYPE_MAX 5.0f
#define OMX_EQ8_PARAM_B5_TYPE_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B5_FREQ_MIN 20.0f
#define OMX_EQ8_PARAM_B5_FREQ_MAX 20000.0f
#define OMX_EQ8_PARAM_B5_FREQ_DEFAULT 1000.0f
#define OMX_EQ8_PARAM_B5_GAIN_MIN -15.0f
#define OMX_EQ8_PARAM_B5_GAIN_MAX 15.0f
#define OMX_EQ8_PARAM_B5_GAIN_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B5_Q_MIN 0.3f
#define OMX_EQ8_PARAM_B5_Q_MAX 116.0f
#define OMX_EQ8_PARAM_B5_Q_DEFAULT 1.0f
#define OMX_EQ8_PARAM_B5_ON_MIN 0.0f
#define OMX_EQ8_PARAM_B5_ON_MAX 1.0f
#define OMX_EQ8_PARAM_B5_ON_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B6_TYPE_MIN 0.0f
#define OMX_EQ8_PARAM_B6_TYPE_MAX 5.0f
#define OMX_EQ8_PARAM_B6_TYPE_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B6_FREQ_MIN 20.0f
#define OMX_EQ8_PARAM_B6_FREQ_MAX 20000.0f
#define OMX_EQ8_PARAM_B6_FREQ_DEFAULT 2500.0f
#define OMX_EQ8_PARAM_B6_GAIN_MIN -15.0f
#define OMX_EQ8_PARAM_B6_GAIN_MAX 15.0f
#define OMX_EQ8_PARAM_B6_GAIN_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B6_Q_MIN 0.3f
#define OMX_EQ8_PARAM_B6_Q_MAX 116.0f
#define OMX_EQ8_PARAM_B6_Q_DEFAULT 1.0f
#define OMX_EQ8_PARAM_B6_ON_MIN 0.0f
#define OMX_EQ8_PARAM_B6_ON_MAX 1.0f
#define OMX_EQ8_PARAM_B6_ON_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B7_TYPE_MIN 0.0f
#define OMX_EQ8_PARAM_B7_TYPE_MAX 5.0f
#define OMX_EQ8_PARAM_B7_TYPE_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B7_FREQ_MIN 20.0f
#define OMX_EQ8_PARAM_B7_FREQ_MAX 20000.0f
#define OMX_EQ8_PARAM_B7_FREQ_DEFAULT 5000.0f
#define OMX_EQ8_PARAM_B7_GAIN_MIN -15.0f
#define OMX_EQ8_PARAM_B7_GAIN_MAX 15.0f
#define OMX_EQ8_PARAM_B7_GAIN_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B7_Q_MIN 0.3f
#define OMX_EQ8_PARAM_B7_Q_MAX 116.0f
#define OMX_EQ8_PARAM_B7_Q_DEFAULT 1.0f
#define OMX_EQ8_PARAM_B7_ON_MIN 0.0f
#define OMX_EQ8_PARAM_B7_ON_MAX 1.0f
#define OMX_EQ8_PARAM_B7_ON_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B8_TYPE_MIN 0.0f
#define OMX_EQ8_PARAM_B8_TYPE_MAX 5.0f
#define OMX_EQ8_PARAM_B8_TYPE_DEFAULT 2.0f
#define OMX_EQ8_PARAM_B8_FREQ_MIN 20.0f
#define OMX_EQ8_PARAM_B8_FREQ_MAX 20000.0f
#define OMX_EQ8_PARAM_B8_FREQ_DEFAULT 12500.0f
#define OMX_EQ8_PARAM_B8_GAIN_MIN -15.0f
#define OMX_EQ8_PARAM_B8_GAIN_MAX 15.0f
#define OMX_EQ8_PARAM_B8_GAIN_DEFAULT 0.0f
#define OMX_EQ8_PARAM_B8_Q_MIN 0.3f
#define OMX_EQ8_PARAM_B8_Q_MAX 116.0f
#define OMX_EQ8_PARAM_B8_Q_DEFAULT 1.0f
#define OMX_EQ8_PARAM_B8_ON_MIN 0.0f
#define OMX_EQ8_PARAM_B8_ON_MAX 1.0f
#define OMX_EQ8_PARAM_B8_ON_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_EQ8_NAME "omx eq8"
#define OMX_EQ8_VENDOR "openmixer"
#define OMX_EQ8_URL "https://github.com/Open-Mixer/omx-plugins"
#define OMX_EQ8_VERSION "0.19.3"
#define OMX_EQ8_DESCRIPTION "The OpenMixer console's channel EQ, 8-band form: 8 parametric bands (bell, shelves, notch, all-pass) and a high-pass and a low-pass filter at 12 or 24 dB/oct, stereo, zero latency; every band ships off, so a racked instance is a wire."
#define OMX_EQ8_CLAP_ID "org.openmixer.eq8"
#define OMX_EQ8_CLAP_FEATURES "audio-effect", "equalizer", "stereo"
#define OMX_EQ8_LV2_URI "urn:openmixer:eq8"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_EQ8_DECL_SOURCE "omx eq8: HPF On 0 to 1, HPF Frequency 20 to 1000 Hz, HPF Slope 12 to 24 dB/oct, LPF On 0 to 1, LPF Frequency 1000 to 20000 Hz, LPF Slope 12 to 24 dB/oct, Band 1 Type 0 to 5, Band 1 Frequency 20 to 20000 Hz, Band 1 Gain -15 to 15 dB, Band 1 Q 0.3 to 116, Band 1 On 0 to 1, Band 2 Type 0 to 5, Band 2 Frequency 20 to 20000 Hz, Band 2 Gain -15 to 15 dB, Band 2 Q 0.3 to 116, Band 2 On 0 to 1, Band 3 Type 0 to 5, Band 3 Frequency 20 to 20000 Hz, Band 3 Gain -15 to 15 dB, Band 3 Q 0.3 to 116, Band 3 On 0 to 1, Band 4 Type 0 to 5, Band 4 Frequency 20 to 20000 Hz, Band 4 Gain -15 to 15 dB, Band 4 Q 0.3 to 116, Band 4 On 0 to 1, Band 5 Type 0 to 5, Band 5 Frequency 20 to 20000 Hz, Band 5 Gain -15 to 15 dB, Band 5 Q 0.3 to 116, Band 5 On 0 to 1, Band 6 Type 0 to 5, Band 6 Frequency 20 to 20000 Hz, Band 6 Gain -15 to 15 dB, Band 6 Q 0.3 to 116, Band 6 On 0 to 1, Band 7 Type 0 to 5, Band 7 Frequency 20 to 20000 Hz, Band 7 Gain -15 to 15 dB, Band 7 Q 0.3 to 116, Band 7 On 0 to 1, Band 8 Type 0 to 5, Band 8 Frequency 20 to 20000 Hz, Band 8 Gain -15 to 15 dB, Band 8 Q 0.3 to 116, Band 8 On 0 to 1"
#define OMX_EQ8_DECL_DIGEST "c3aed8574463db0f192ce1c2001a47944578978db7ebdfd2cadcd5602dc271a0"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_EQ8_PARAM_CONTRACT_VERSION 2u
#define OMX_EQ8_PARAM_CONTRACT_FINGERPRINT "fa5d824adc0ccb18b03484bf85b0fa0a82c55a676bbc41fcd34ce207dbae30eb"
static const struct omx_param_contract_row OMX_EQ8_PARAM_CONTRACT[OMX_EQ8_PARAM_COUNT] = {
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* hpf_on none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* hpf_freq hz */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* hpf_slope none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* lpf_on none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* lpf_freq hz */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* lpf_slope none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b1_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b1_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b1_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b1_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b1_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b2_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b2_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b2_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b2_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b2_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b3_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b3_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b3_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b3_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b3_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b4_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b4_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b4_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b4_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b4_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b5_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b5_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b5_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b5_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b5_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b6_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b6_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b6_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b6_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b6_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b7_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b7_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b7_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b7_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b7_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b8_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b8_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b8_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b8_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b8_on none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_EQ8_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family eq.
 * Declared roles not published:
 *   hpf_slope (hpf_slope): the hpf_slope role takes none, db, not dB/oct
 *   lpf_slope (lpf_slope): the lpf_slope role takes none, db, not dB/oct */
#define OMX_EQ8_PANEL_FINGERPRINT "b2160f0a294ed44a7dbdf639aeb5739d508cc718af12ecd6706f275d55318f25"
static const struct omx_panel_section_row OMX_EQ8_PANEL_SECTIONS[] = {
  { "bands", "fx.eq.section.bands", "Bands" },
  { "hpf", "fx.eq.section.hpf", "HPF" },
  { "lpf", "fx.eq.section.lpf", "LPF" },
};
static const struct omx_panel_control_row OMX_EQ8_PANEL_CONTROLS[] = {
  { 10u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b1_on", "Band 1 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b1_type", "Band 1 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b1_freq", "Band 1 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 8u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b1_gain", "Band 1 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 9u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b1_q", "Band 1 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 15u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b2_on", "Band 2 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 11u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b2_type", "Band 2 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 12u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b2_freq", "Band 2 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 13u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b2_gain", "Band 2 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 14u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b2_q", "Band 2 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 20u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b3_on", "Band 3 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 16u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b3_type", "Band 3 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 17u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b3_freq", "Band 3 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 18u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b3_gain", "Band 3 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 19u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b3_q", "Band 3 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 25u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b4_on", "Band 4 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 21u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b4_type", "Band 4 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 22u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b4_freq", "Band 4 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 23u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b4_gain", "Band 4 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 24u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b4_q", "Band 4 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 30u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b5_on", "Band 5 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 26u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b5_type", "Band 5 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 27u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b5_freq", "Band 5 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 28u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b5_gain", "Band 5 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 29u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b5_q", "Band 5 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 35u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b6_on", "Band 6 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 31u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b6_type", "Band 6 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 32u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b6_freq", "Band 6 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 33u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b6_gain", "Band 6 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 34u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b6_q", "Band 6 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 40u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b7_on", "Band 7 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 36u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b7_type", "Band 7 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 37u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b7_freq", "Band 7 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 38u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b7_gain", "Band 7 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 39u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b7_q", "Band 7 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 45u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b8_on", "Band 8 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 41u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b8_type", "Band 8 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 42u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b8_freq", "Band 8 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 43u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b8_gain", "Band 8 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 44u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b8_q", "Band 8 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 0u, 1u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.hpf_on", "HPF On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 1u, OMX_CLAP_WIDGET_AUTO, "fx.eq.hpf_slope", "HPF Slope", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.eq.hpf_freq", "HPF Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 2u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.lpf_on", "LPF On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 2u, OMX_CLAP_WIDGET_AUTO, "fx.eq.lpf_slope", "LPF Slope", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.eq.lpf_freq", "LPF Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_EQ8_PANEL_ROLES[] = {
  { "band_type", 0u, 6u },
  { "band_type", 1u, 11u },
  { "band_type", 2u, 16u },
  { "band_type", 3u, 21u },
  { "band_type", 4u, 26u },
  { "band_type", 5u, 31u },
  { "band_type", 6u, 36u },
  { "band_type", 7u, 41u },
  { "band_freq", 0u, 7u },
  { "band_freq", 1u, 12u },
  { "band_freq", 2u, 17u },
  { "band_freq", 3u, 22u },
  { "band_freq", 4u, 27u },
  { "band_freq", 5u, 32u },
  { "band_freq", 6u, 37u },
  { "band_freq", 7u, 42u },
  { "band_gain", 0u, 8u },
  { "band_gain", 1u, 13u },
  { "band_gain", 2u, 18u },
  { "band_gain", 3u, 23u },
  { "band_gain", 4u, 28u },
  { "band_gain", 5u, 33u },
  { "band_gain", 6u, 38u },
  { "band_gain", 7u, 43u },
  { "band_q", 0u, 9u },
  { "band_q", 1u, 14u },
  { "band_q", 2u, 19u },
  { "band_q", 3u, 24u },
  { "band_q", 4u, 29u },
  { "band_q", 5u, 34u },
  { "band_q", 6u, 39u },
  { "band_q", 7u, 44u },
  { "band_on", 0u, 10u },
  { "band_on", 1u, 15u },
  { "band_on", 2u, 20u },
  { "band_on", 3u, 25u },
  { "band_on", 4u, 30u },
  { "band_on", 5u, 35u },
  { "band_on", 6u, 40u },
  { "band_on", 7u, 45u },
  { "hpf_freq", 0u, 1u },
  { "hpf_on", 0u, 0u },
  { "lpf_freq", 0u, 4u },
  { "lpf_on", 0u, 3u },
};
static const struct omx_panel_display_row OMX_EQ8_PANEL_DISPLAYS[] = {
  { OMX_CLAP_DISPLAY_EQ_RESPONSE, 0u, OMX_CLAP_DISPLAY_FROM_FAMILY, { OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.eq.display.eq_response", "Bands" },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_EQ8_PANEL_PAGES[] = {
  { "Easy", "Easy", { 8u, 13u, 23u, 28u, 38u, 43u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 1 On - Band 2 Q", { 10u, 6u, 7u, 9u, 15u, 11u, 12u, 14u } },
  { "Bands", "Bands: Band 3 On - Band 3 Q", { 20u, 16u, 17u, 18u, 19u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 4 On - Band 5 Q", { 25u, 21u, 22u, 24u, 30u, 26u, 27u, 29u } },
  { "Bands", "Bands: Band 6 On - Band 6 Q", { 35u, 31u, 32u, 33u, 34u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 7 On - Band 8 Q", { 40u, 36u, 37u, 39u, 45u, 41u, 42u, 44u } },
  { "HPF", "HPF", { 0u, 2u, 1u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "LPF", "LPF", { 3u, 5u, 4u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_EQ8_PANEL_PAGE_COUNT 8u
static const struct omx_panel_rows OMX_EQ8_PANEL = {
  OMX_EQ8_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_EQ,
  OMX_EQ8_PANEL_SECTIONS, 3u,
  OMX_EQ8_PANEL_CONTROLS, 46u,
  OMX_EQ8_PANEL_ROLES, 44u,
  OMX_EQ8_PANEL_DISPLAYS, 1u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_EQ8_LV2_PORT_ENABLED 0u
#define OMX_EQ8_LV2_PORT_LATENCY 1u
#define OMX_EQ8_LV2_PORT_IN_L 2u
#define OMX_EQ8_LV2_PORT_IN_R 3u
#define OMX_EQ8_LV2_PORT_OUT_L 4u
#define OMX_EQ8_LV2_PORT_OUT_R 5u
#define OMX_EQ8_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_EQ8_LV2_PORT_FIRST_METER 52u
#define OMX_EQ8_LV2_PORT_COUNT 52u

#endif /* OMX_EQ8_PARAMS_H */
