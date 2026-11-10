// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_EQ16_PARAMS_H
#define OMX_EQ16_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-eq16/omx-eq16.decl.json.
 * Regenerate: `make -C plugins/omx-eq16 gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_eq16_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

/* The band count of this variant of plugins/omx-eq: the instance face's compile-time count. */
#define OMX_EQ_INSTANCE_BANDS OMX_EQ_BAND_COUNTS_EQ16_MAX

enum {
  OMX_EQ16_PARAM_HPF_ON = 0,
  OMX_EQ16_PARAM_HPF_FREQ = 1,
  OMX_EQ16_PARAM_HPF_SLOPE = 2,
  OMX_EQ16_PARAM_LPF_ON = 3,
  OMX_EQ16_PARAM_LPF_FREQ = 4,
  OMX_EQ16_PARAM_LPF_SLOPE = 5,
  OMX_EQ16_PARAM_B1_TYPE = 6,
  OMX_EQ16_PARAM_B1_FREQ = 7,
  OMX_EQ16_PARAM_B1_GAIN = 8,
  OMX_EQ16_PARAM_B1_Q = 9,
  OMX_EQ16_PARAM_B1_ON = 10,
  OMX_EQ16_PARAM_B2_TYPE = 11,
  OMX_EQ16_PARAM_B2_FREQ = 12,
  OMX_EQ16_PARAM_B2_GAIN = 13,
  OMX_EQ16_PARAM_B2_Q = 14,
  OMX_EQ16_PARAM_B2_ON = 15,
  OMX_EQ16_PARAM_B3_TYPE = 16,
  OMX_EQ16_PARAM_B3_FREQ = 17,
  OMX_EQ16_PARAM_B3_GAIN = 18,
  OMX_EQ16_PARAM_B3_Q = 19,
  OMX_EQ16_PARAM_B3_ON = 20,
  OMX_EQ16_PARAM_B4_TYPE = 21,
  OMX_EQ16_PARAM_B4_FREQ = 22,
  OMX_EQ16_PARAM_B4_GAIN = 23,
  OMX_EQ16_PARAM_B4_Q = 24,
  OMX_EQ16_PARAM_B4_ON = 25,
  OMX_EQ16_PARAM_B5_TYPE = 26,
  OMX_EQ16_PARAM_B5_FREQ = 27,
  OMX_EQ16_PARAM_B5_GAIN = 28,
  OMX_EQ16_PARAM_B5_Q = 29,
  OMX_EQ16_PARAM_B5_ON = 30,
  OMX_EQ16_PARAM_B6_TYPE = 31,
  OMX_EQ16_PARAM_B6_FREQ = 32,
  OMX_EQ16_PARAM_B6_GAIN = 33,
  OMX_EQ16_PARAM_B6_Q = 34,
  OMX_EQ16_PARAM_B6_ON = 35,
  OMX_EQ16_PARAM_B7_TYPE = 36,
  OMX_EQ16_PARAM_B7_FREQ = 37,
  OMX_EQ16_PARAM_B7_GAIN = 38,
  OMX_EQ16_PARAM_B7_Q = 39,
  OMX_EQ16_PARAM_B7_ON = 40,
  OMX_EQ16_PARAM_B8_TYPE = 41,
  OMX_EQ16_PARAM_B8_FREQ = 42,
  OMX_EQ16_PARAM_B8_GAIN = 43,
  OMX_EQ16_PARAM_B8_Q = 44,
  OMX_EQ16_PARAM_B8_ON = 45,
  OMX_EQ16_PARAM_B9_TYPE = 46,
  OMX_EQ16_PARAM_B9_FREQ = 47,
  OMX_EQ16_PARAM_B9_GAIN = 48,
  OMX_EQ16_PARAM_B9_Q = 49,
  OMX_EQ16_PARAM_B9_ON = 50,
  OMX_EQ16_PARAM_B10_TYPE = 51,
  OMX_EQ16_PARAM_B10_FREQ = 52,
  OMX_EQ16_PARAM_B10_GAIN = 53,
  OMX_EQ16_PARAM_B10_Q = 54,
  OMX_EQ16_PARAM_B10_ON = 55,
  OMX_EQ16_PARAM_B11_TYPE = 56,
  OMX_EQ16_PARAM_B11_FREQ = 57,
  OMX_EQ16_PARAM_B11_GAIN = 58,
  OMX_EQ16_PARAM_B11_Q = 59,
  OMX_EQ16_PARAM_B11_ON = 60,
  OMX_EQ16_PARAM_B12_TYPE = 61,
  OMX_EQ16_PARAM_B12_FREQ = 62,
  OMX_EQ16_PARAM_B12_GAIN = 63,
  OMX_EQ16_PARAM_B12_Q = 64,
  OMX_EQ16_PARAM_B12_ON = 65,
  OMX_EQ16_PARAM_B13_TYPE = 66,
  OMX_EQ16_PARAM_B13_FREQ = 67,
  OMX_EQ16_PARAM_B13_GAIN = 68,
  OMX_EQ16_PARAM_B13_Q = 69,
  OMX_EQ16_PARAM_B13_ON = 70,
  OMX_EQ16_PARAM_B14_TYPE = 71,
  OMX_EQ16_PARAM_B14_FREQ = 72,
  OMX_EQ16_PARAM_B14_GAIN = 73,
  OMX_EQ16_PARAM_B14_Q = 74,
  OMX_EQ16_PARAM_B14_ON = 75,
  OMX_EQ16_PARAM_B15_TYPE = 76,
  OMX_EQ16_PARAM_B15_FREQ = 77,
  OMX_EQ16_PARAM_B15_GAIN = 78,
  OMX_EQ16_PARAM_B15_Q = 79,
  OMX_EQ16_PARAM_B15_ON = 80,
  OMX_EQ16_PARAM_B16_TYPE = 81,
  OMX_EQ16_PARAM_B16_FREQ = 82,
  OMX_EQ16_PARAM_B16_GAIN = 83,
  OMX_EQ16_PARAM_B16_Q = 84,
  OMX_EQ16_PARAM_B16_ON = 85,
  OMX_EQ16_PARAM_COUNT = 86
};

static const omx_plugin_param OMX_EQ16_PARAMS[OMX_EQ16_PARAM_COUNT] = {
  { "hpf_on", "HPF On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "hpf_freq", "HPF Frequency", "Hz", 20.0f, 1000.0f, 80.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "hpf_slope", "HPF Slope", "dB/oct", 12.0f, 24.0f, 12.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "lpf_on", "LPF On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "lpf_freq", "LPF Frequency", "Hz", 1000.0f, 20000.0f, 18000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "lpf_slope", "LPF Slope", "dB/oct", 12.0f, 24.0f, 12.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b1_type", "Band 1 Type", "", 0.0f, 5.0f, 1.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b1_freq", "Band 1 Frequency", "Hz", 20.0f, 20000.0f, 25.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b1_gain", "Band 1 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b1_q", "Band 1 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b1_on", "Band 1 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b2_type", "Band 2 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b2_freq", "Band 2 Frequency", "Hz", 20.0f, 20000.0f, 40.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b2_gain", "Band 2 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b2_q", "Band 2 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b2_on", "Band 2 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b3_type", "Band 3 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b3_freq", "Band 3 Frequency", "Hz", 20.0f, 20000.0f, 63.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b3_gain", "Band 3 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b3_q", "Band 3 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b3_on", "Band 3 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b4_type", "Band 4 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b4_freq", "Band 4 Frequency", "Hz", 20.0f, 20000.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b4_gain", "Band 4 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b4_q", "Band 4 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b4_on", "Band 4 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b5_type", "Band 5 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b5_freq", "Band 5 Frequency", "Hz", 20.0f, 20000.0f, 125.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b5_gain", "Band 5 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b5_q", "Band 5 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b5_on", "Band 5 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b6_type", "Band 6 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b6_freq", "Band 6 Frequency", "Hz", 20.0f, 20000.0f, 200.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b6_gain", "Band 6 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b6_q", "Band 6 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b6_on", "Band 6 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b7_type", "Band 7 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b7_freq", "Band 7 Frequency", "Hz", 20.0f, 20000.0f, 315.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b7_gain", "Band 7 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b7_q", "Band 7 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b7_on", "Band 7 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b8_type", "Band 8 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b8_freq", "Band 8 Frequency", "Hz", 20.0f, 20000.0f, 500.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b8_gain", "Band 8 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b8_q", "Band 8 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b8_on", "Band 8 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b9_type", "Band 9 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b9_freq", "Band 9 Frequency", "Hz", 20.0f, 20000.0f, 800.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b9_gain", "Band 9 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b9_q", "Band 9 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b9_on", "Band 9 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b10_type", "Band 10 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b10_freq", "Band 10 Frequency", "Hz", 20.0f, 20000.0f, 1250.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b10_gain", "Band 10 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b10_q", "Band 10 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b10_on", "Band 10 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b11_type", "Band 11 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b11_freq", "Band 11 Frequency", "Hz", 20.0f, 20000.0f, 2000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b11_gain", "Band 11 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b11_q", "Band 11 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b11_on", "Band 11 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b12_type", "Band 12 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b12_freq", "Band 12 Frequency", "Hz", 20.0f, 20000.0f, 3150.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b12_gain", "Band 12 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b12_q", "Band 12 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b12_on", "Band 12 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b13_type", "Band 13 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b13_freq", "Band 13 Frequency", "Hz", 20.0f, 20000.0f, 4000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b13_gain", "Band 13 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b13_q", "Band 13 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b13_on", "Band 13 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b14_type", "Band 14 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b14_freq", "Band 14 Frequency", "Hz", 20.0f, 20000.0f, 6300.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b14_gain", "Band 14 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b14_q", "Band 14 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b14_on", "Band 14 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b15_type", "Band 15 Type", "", 0.0f, 5.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b15_freq", "Band 15 Frequency", "Hz", 20.0f, 20000.0f, 10000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b15_gain", "Band 15 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b15_q", "Band 15 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b15_on", "Band 15 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "b16_type", "Band 16 Type", "", 0.0f, 5.0f, 2.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b16_freq", "Band 16 Frequency", "Hz", 20.0f, 20000.0f, 16000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "b16_gain", "Band 16 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "b16_q", "Band 16 Q", "", 0.3f, 116.0f, 1.0f, 0u },
  { "b16_on", "Band 16 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_EQ16_PARAM_MODULES[OMX_EQ16_PARAM_COUNT] = {
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
  "Bands/Band 9",
  "Bands/Band 9",
  "Bands/Band 9",
  "Bands/Band 9",
  "Bands/Band 9",
  "Bands/Band 10",
  "Bands/Band 10",
  "Bands/Band 10",
  "Bands/Band 10",
  "Bands/Band 10",
  "Bands/Band 11",
  "Bands/Band 11",
  "Bands/Band 11",
  "Bands/Band 11",
  "Bands/Band 11",
  "Bands/Band 12",
  "Bands/Band 12",
  "Bands/Band 12",
  "Bands/Band 12",
  "Bands/Band 12",
  "Bands/Band 13",
  "Bands/Band 13",
  "Bands/Band 13",
  "Bands/Band 13",
  "Bands/Band 13",
  "Bands/Band 14",
  "Bands/Band 14",
  "Bands/Band 14",
  "Bands/Band 14",
  "Bands/Band 14",
  "Bands/Band 15",
  "Bands/Band 15",
  "Bands/Band 15",
  "Bands/Band 15",
  "Bands/Band 15",
  "Bands/Band 16",
  "Bands/Band 16",
  "Bands/Band 16",
  "Bands/Band 16",
  "Bands/Band 16",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_EQ16_PARAM_HPF_ON_MIN 0.0f
#define OMX_EQ16_PARAM_HPF_ON_MAX 1.0f
#define OMX_EQ16_PARAM_HPF_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_HPF_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_HPF_FREQ_MAX 1000.0f
#define OMX_EQ16_PARAM_HPF_FREQ_DEFAULT 80.0f
#define OMX_EQ16_PARAM_HPF_SLOPE_MIN 12.0f
#define OMX_EQ16_PARAM_HPF_SLOPE_MAX 24.0f
#define OMX_EQ16_PARAM_HPF_SLOPE_DEFAULT 12.0f
#define OMX_EQ16_PARAM_LPF_ON_MIN 0.0f
#define OMX_EQ16_PARAM_LPF_ON_MAX 1.0f
#define OMX_EQ16_PARAM_LPF_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_LPF_FREQ_MIN 1000.0f
#define OMX_EQ16_PARAM_LPF_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_LPF_FREQ_DEFAULT 18000.0f
#define OMX_EQ16_PARAM_LPF_SLOPE_MIN 12.0f
#define OMX_EQ16_PARAM_LPF_SLOPE_MAX 24.0f
#define OMX_EQ16_PARAM_LPF_SLOPE_DEFAULT 12.0f
#define OMX_EQ16_PARAM_B1_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B1_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B1_TYPE_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B1_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B1_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B1_FREQ_DEFAULT 25.0f
#define OMX_EQ16_PARAM_B1_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B1_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B1_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B1_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B1_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B1_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B1_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B1_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B1_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B2_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B2_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B2_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B2_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B2_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B2_FREQ_DEFAULT 40.0f
#define OMX_EQ16_PARAM_B2_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B2_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B2_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B2_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B2_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B2_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B2_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B2_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B2_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B3_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B3_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B3_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B3_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B3_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B3_FREQ_DEFAULT 63.0f
#define OMX_EQ16_PARAM_B3_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B3_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B3_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B3_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B3_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B3_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B3_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B3_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B3_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B4_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B4_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B4_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B4_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B4_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B4_FREQ_DEFAULT 100.0f
#define OMX_EQ16_PARAM_B4_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B4_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B4_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B4_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B4_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B4_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B4_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B4_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B4_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B5_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B5_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B5_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B5_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B5_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B5_FREQ_DEFAULT 125.0f
#define OMX_EQ16_PARAM_B5_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B5_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B5_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B5_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B5_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B5_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B5_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B5_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B5_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B6_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B6_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B6_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B6_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B6_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B6_FREQ_DEFAULT 200.0f
#define OMX_EQ16_PARAM_B6_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B6_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B6_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B6_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B6_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B6_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B6_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B6_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B6_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B7_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B7_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B7_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B7_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B7_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B7_FREQ_DEFAULT 315.0f
#define OMX_EQ16_PARAM_B7_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B7_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B7_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B7_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B7_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B7_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B7_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B7_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B7_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B8_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B8_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B8_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B8_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B8_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B8_FREQ_DEFAULT 500.0f
#define OMX_EQ16_PARAM_B8_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B8_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B8_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B8_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B8_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B8_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B8_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B8_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B8_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B9_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B9_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B9_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B9_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B9_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B9_FREQ_DEFAULT 800.0f
#define OMX_EQ16_PARAM_B9_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B9_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B9_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B9_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B9_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B9_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B9_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B9_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B9_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B10_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B10_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B10_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B10_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B10_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B10_FREQ_DEFAULT 1250.0f
#define OMX_EQ16_PARAM_B10_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B10_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B10_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B10_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B10_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B10_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B10_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B10_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B10_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B11_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B11_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B11_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B11_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B11_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B11_FREQ_DEFAULT 2000.0f
#define OMX_EQ16_PARAM_B11_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B11_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B11_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B11_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B11_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B11_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B11_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B11_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B11_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B12_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B12_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B12_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B12_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B12_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B12_FREQ_DEFAULT 3150.0f
#define OMX_EQ16_PARAM_B12_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B12_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B12_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B12_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B12_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B12_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B12_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B12_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B12_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B13_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B13_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B13_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B13_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B13_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B13_FREQ_DEFAULT 4000.0f
#define OMX_EQ16_PARAM_B13_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B13_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B13_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B13_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B13_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B13_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B13_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B13_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B13_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B14_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B14_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B14_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B14_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B14_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B14_FREQ_DEFAULT 6300.0f
#define OMX_EQ16_PARAM_B14_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B14_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B14_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B14_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B14_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B14_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B14_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B14_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B14_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B15_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B15_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B15_TYPE_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B15_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B15_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B15_FREQ_DEFAULT 10000.0f
#define OMX_EQ16_PARAM_B15_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B15_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B15_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B15_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B15_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B15_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B15_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B15_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B15_ON_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B16_TYPE_MIN 0.0f
#define OMX_EQ16_PARAM_B16_TYPE_MAX 5.0f
#define OMX_EQ16_PARAM_B16_TYPE_DEFAULT 2.0f
#define OMX_EQ16_PARAM_B16_FREQ_MIN 20.0f
#define OMX_EQ16_PARAM_B16_FREQ_MAX 20000.0f
#define OMX_EQ16_PARAM_B16_FREQ_DEFAULT 16000.0f
#define OMX_EQ16_PARAM_B16_GAIN_MIN -15.0f
#define OMX_EQ16_PARAM_B16_GAIN_MAX 15.0f
#define OMX_EQ16_PARAM_B16_GAIN_DEFAULT 0.0f
#define OMX_EQ16_PARAM_B16_Q_MIN 0.3f
#define OMX_EQ16_PARAM_B16_Q_MAX 116.0f
#define OMX_EQ16_PARAM_B16_Q_DEFAULT 1.0f
#define OMX_EQ16_PARAM_B16_ON_MIN 0.0f
#define OMX_EQ16_PARAM_B16_ON_MAX 1.0f
#define OMX_EQ16_PARAM_B16_ON_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_EQ16_NAME "omx eq16"
#define OMX_EQ16_VENDOR "openmixer"
#define OMX_EQ16_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_EQ16_VERSION "0.19.2"
#define OMX_EQ16_DESCRIPTION "The OpenMixer console's channel EQ, 16-band form: 16 parametric bands (bell, shelves, notch, all-pass) and a high-pass and a low-pass filter at 12 or 24 dB/oct, stereo, zero latency; every band ships off, so a racked instance is a wire."
#define OMX_EQ16_CLAP_ID "org.openmixer.eq16"
#define OMX_EQ16_CLAP_FEATURES "audio-effect", "equalizer", "stereo"
#define OMX_EQ16_LV2_URI "urn:openmixer:eq16"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_EQ16_DECL_SOURCE "omx eq16: HPF On 0 to 1, HPF Frequency 20 to 1000 Hz, HPF Slope 12 to 24 dB/oct, LPF On 0 to 1, LPF Frequency 1000 to 20000 Hz, LPF Slope 12 to 24 dB/oct, Band 1 Type 0 to 5, Band 1 Frequency 20 to 20000 Hz, Band 1 Gain -15 to 15 dB, Band 1 Q 0.3 to 116, Band 1 On 0 to 1, Band 2 Type 0 to 5, Band 2 Frequency 20 to 20000 Hz, Band 2 Gain -15 to 15 dB, Band 2 Q 0.3 to 116, Band 2 On 0 to 1, Band 3 Type 0 to 5, Band 3 Frequency 20 to 20000 Hz, Band 3 Gain -15 to 15 dB, Band 3 Q 0.3 to 116, Band 3 On 0 to 1, Band 4 Type 0 to 5, Band 4 Frequency 20 to 20000 Hz, Band 4 Gain -15 to 15 dB, Band 4 Q 0.3 to 116, Band 4 On 0 to 1, Band 5 Type 0 to 5, Band 5 Frequency 20 to 20000 Hz, Band 5 Gain -15 to 15 dB, Band 5 Q 0.3 to 116, Band 5 On 0 to 1, Band 6 Type 0 to 5, Band 6 Frequency 20 to 20000 Hz, Band 6 Gain -15 to 15 dB, Band 6 Q 0.3 to 116, Band 6 On 0 to 1, Band 7 Type 0 to 5, Band 7 Frequency 20 to 20000 Hz, Band 7 Gain -15 to 15 dB, Band 7 Q 0.3 to 116, Band 7 On 0 to 1, Band 8 Type 0 to 5, Band 8 Frequency 20 to 20000 Hz, Band 8 Gain -15 to 15 dB, Band 8 Q 0.3 to 116, Band 8 On 0 to 1, Band 9 Type 0 to 5, Band 9 Frequency 20 to 20000 Hz, Band 9 Gain -15 to 15 dB, Band 9 Q 0.3 to 116, Band 9 On 0 to 1, Band 10 Type 0 to 5, Band 10 Frequency 20 to 20000 Hz, Band 10 Gain -15 to 15 dB, Band 10 Q 0.3 to 116, Band 10 On 0 to 1, Band 11 Type 0 to 5, Band 11 Frequency 20 to 20000 Hz, Band 11 Gain -15 to 15 dB, Band 11 Q 0.3 to 116, Band 11 On 0 to 1, Band 12 Type 0 to 5, Band 12 Frequency 20 to 20000 Hz, Band 12 Gain -15 to 15 dB, Band 12 Q 0.3 to 116, Band 12 On 0 to 1, Band 13 Type 0 to 5, Band 13 Frequency 20 to 20000 Hz, Band 13 Gain -15 to 15 dB, Band 13 Q 0.3 to 116, Band 13 On 0 to 1, Band 14 Type 0 to 5, Band 14 Frequency 20 to 20000 Hz, Band 14 Gain -15 to 15 dB, Band 14 Q 0.3 to 116, Band 14 On 0 to 1, Band 15 Type 0 to 5, Band 15 Frequency 20 to 20000 Hz, Band 15 Gain -15 to 15 dB, Band 15 Q 0.3 to 116, Band 15 On 0 to 1, Band 16 Type 0 to 5, Band 16 Frequency 20 to 20000 Hz, Band 16 Gain -15 to 15 dB, Band 16 Q 0.3 to 116, Band 16 On 0 to 1"
#define OMX_EQ16_DECL_DIGEST "7b5f7b1f46ebc116414f68203cf8240c72c0154227d0169c0da832ee9aad2c56"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), is the face's. */
#define OMX_EQ16_PARAM_CONTRACT_VERSION 1u
#define OMX_EQ16_PARAM_CONTRACT_FINGERPRINT "f5e8fc02ca0b569ec0dde54fb0ab1fe10ec05c33ef19e1f2571524951f14e0c6"
static const struct omx_param_contract_row OMX_EQ16_PARAM_CONTRACT[OMX_EQ16_PARAM_COUNT] = {
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
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b9_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b9_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b9_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b9_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b9_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b10_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b10_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b10_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b10_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b10_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b11_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b11_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b11_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b11_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b11_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b12_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b12_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b12_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b12_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b12_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b13_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b13_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b13_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b13_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b13_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b14_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b14_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b14_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b14_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b14_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b15_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b15_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b15_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b15_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b15_on none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b16_type none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* b16_freq hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b16_gain db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* b16_q none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* b16_on none */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_EQ16_RESOURCE_HINTS = {
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
#define OMX_EQ16_PANEL_FINGERPRINT "6fda5dc0611874a6af3ee3c2dacaa72efea530b3cd5f19fd613f3a6d1ff76609"
static const struct omx_panel_section_row OMX_EQ16_PANEL_SECTIONS[] = {
  { "bands", "fx.eq.section.bands", "Bands" },
  { "hpf", "fx.eq.section.hpf", "HPF" },
  { "lpf", "fx.eq.section.lpf", "LPF" },
};
static const struct omx_panel_control_row OMX_EQ16_PANEL_CONTROLS[] = {
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
  { 50u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b9_on", "Band 9 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 46u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b9_type", "Band 9 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 47u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b9_freq", "Band 9 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 48u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b9_gain", "Band 9 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 49u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b9_q", "Band 9 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 55u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b10_on", "Band 10 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 51u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b10_type", "Band 10 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 52u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b10_freq", "Band 10 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 53u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b10_gain", "Band 10 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 54u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b10_q", "Band 10 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 60u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b11_on", "Band 11 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 56u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b11_type", "Band 11 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 57u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b11_freq", "Band 11 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 58u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b11_gain", "Band 11 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 59u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b11_q", "Band 11 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 65u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b12_on", "Band 12 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 61u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b12_type", "Band 12 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 62u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b12_freq", "Band 12 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 63u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b12_gain", "Band 12 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 64u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b12_q", "Band 12 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 70u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b13_on", "Band 13 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 66u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b13_type", "Band 13 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 67u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b13_freq", "Band 13 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 68u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b13_gain", "Band 13 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 69u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b13_q", "Band 13 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 75u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b14_on", "Band 14 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 71u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b14_type", "Band 14 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 72u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b14_freq", "Band 14 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 73u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b14_gain", "Band 14 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 74u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b14_q", "Band 14 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 80u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b15_on", "Band 15 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 76u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b15_type", "Band 15 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 77u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b15_freq", "Band 15 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 78u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b15_gain", "Band 15 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 79u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b15_q", "Band 15 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 85u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.b16_on", "Band 16 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 81u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.eq.b16_type", "Band 16 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 82u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b16_freq", "Band 16 Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 83u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b16_gain", "Band 16 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 84u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.eq.b16_q", "Band 16 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 0u, 1u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.hpf_on", "HPF On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 1u, OMX_CLAP_WIDGET_AUTO, "fx.eq.hpf_slope", "HPF Slope", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.eq.hpf_freq", "HPF Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 2u, OMX_CLAP_WIDGET_SWITCH, "fx.eq.lpf_on", "LPF On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 2u, OMX_CLAP_WIDGET_AUTO, "fx.eq.lpf_slope", "LPF Slope", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.eq.lpf_freq", "LPF Frequency", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_EQ16_PANEL_ROLES[] = {
  { "band_type", 0u, 6u },
  { "band_type", 1u, 11u },
  { "band_type", 2u, 16u },
  { "band_type", 3u, 21u },
  { "band_type", 4u, 26u },
  { "band_type", 5u, 31u },
  { "band_type", 6u, 36u },
  { "band_type", 7u, 41u },
  { "band_type", 8u, 46u },
  { "band_type", 9u, 51u },
  { "band_type", 10u, 56u },
  { "band_type", 11u, 61u },
  { "band_type", 12u, 66u },
  { "band_type", 13u, 71u },
  { "band_type", 14u, 76u },
  { "band_type", 15u, 81u },
  { "band_freq", 0u, 7u },
  { "band_freq", 1u, 12u },
  { "band_freq", 2u, 17u },
  { "band_freq", 3u, 22u },
  { "band_freq", 4u, 27u },
  { "band_freq", 5u, 32u },
  { "band_freq", 6u, 37u },
  { "band_freq", 7u, 42u },
  { "band_freq", 8u, 47u },
  { "band_freq", 9u, 52u },
  { "band_freq", 10u, 57u },
  { "band_freq", 11u, 62u },
  { "band_freq", 12u, 67u },
  { "band_freq", 13u, 72u },
  { "band_freq", 14u, 77u },
  { "band_freq", 15u, 82u },
  { "band_gain", 0u, 8u },
  { "band_gain", 1u, 13u },
  { "band_gain", 2u, 18u },
  { "band_gain", 3u, 23u },
  { "band_gain", 4u, 28u },
  { "band_gain", 5u, 33u },
  { "band_gain", 6u, 38u },
  { "band_gain", 7u, 43u },
  { "band_gain", 8u, 48u },
  { "band_gain", 9u, 53u },
  { "band_gain", 10u, 58u },
  { "band_gain", 11u, 63u },
  { "band_gain", 12u, 68u },
  { "band_gain", 13u, 73u },
  { "band_gain", 14u, 78u },
  { "band_gain", 15u, 83u },
  { "band_q", 0u, 9u },
  { "band_q", 1u, 14u },
  { "band_q", 2u, 19u },
  { "band_q", 3u, 24u },
  { "band_q", 4u, 29u },
  { "band_q", 5u, 34u },
  { "band_q", 6u, 39u },
  { "band_q", 7u, 44u },
  { "band_q", 8u, 49u },
  { "band_q", 9u, 54u },
  { "band_q", 10u, 59u },
  { "band_q", 11u, 64u },
  { "band_q", 12u, 69u },
  { "band_q", 13u, 74u },
  { "band_q", 14u, 79u },
  { "band_q", 15u, 84u },
  { "band_on", 0u, 10u },
  { "band_on", 1u, 15u },
  { "band_on", 2u, 20u },
  { "band_on", 3u, 25u },
  { "band_on", 4u, 30u },
  { "band_on", 5u, 35u },
  { "band_on", 6u, 40u },
  { "band_on", 7u, 45u },
  { "band_on", 8u, 50u },
  { "band_on", 9u, 55u },
  { "band_on", 10u, 60u },
  { "band_on", 11u, 65u },
  { "band_on", 12u, 70u },
  { "band_on", 13u, 75u },
  { "band_on", 14u, 80u },
  { "band_on", 15u, 85u },
  { "hpf_freq", 0u, 1u },
  { "hpf_on", 0u, 0u },
  { "lpf_freq", 0u, 4u },
  { "lpf_on", 0u, 3u },
};
static const struct omx_panel_display_row OMX_EQ16_PANEL_DISPLAYS[] = {
  { OMX_CLAP_DISPLAY_EQ_RESPONSE, 0u, OMX_CLAP_DISPLAY_FROM_FAMILY, { OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.eq.display.eq_response", "Bands" },
};
/* clap.remote-controls/2: the pages of the page rule over the sections. */
static const struct omx_panel_page_row OMX_EQ16_PANEL_PAGES[] = {
  { "Easy", "Easy", { 8u, 23u, 38u, 53u, 68u, 83u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 1 On - Band 1 Q", { 10u, 6u, 7u, 9u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 2 On - Band 2 Q", { 15u, 11u, 12u, 13u, 14u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 3 On - Band 3 Q", { 20u, 16u, 17u, 18u, 19u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 4 On - Band 4 Q", { 25u, 21u, 22u, 24u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 5 On - Band 5 Q", { 30u, 26u, 27u, 28u, 29u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 6 On - Band 6 Q", { 35u, 31u, 32u, 33u, 34u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 7 On - Band 7 Q", { 40u, 36u, 37u, 39u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 8 On - Band 8 Q", { 45u, 41u, 42u, 43u, 44u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 9 On - Band 9 Q", { 50u, 46u, 47u, 48u, 49u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 10 On - Band 10 Q", { 55u, 51u, 52u, 54u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 11 On - Band 11 Q", { 60u, 56u, 57u, 58u, 59u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 12 On - Band 12 Q", { 65u, 61u, 62u, 63u, 64u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 13 On - Band 13 Q", { 70u, 66u, 67u, 69u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 14 On - Band 14 Q", { 75u, 71u, 72u, 73u, 74u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 15 On - Band 15 Q", { 80u, 76u, 77u, 78u, 79u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Bands", "Bands: Band 16 On - Band 16 Q", { 85u, 81u, 82u, 84u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "HPF", "HPF", { 0u, 2u, 1u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "LPF", "LPF", { 3u, 5u, 4u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_EQ16_PANEL_PAGE_COUNT 19u
static const struct omx_panel_rows OMX_EQ16_PANEL = {
  OMX_EQ16_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_EQ,
  OMX_EQ16_PANEL_SECTIONS, 3u,
  OMX_EQ16_PANEL_CONTROLS, 86u,
  OMX_EQ16_PANEL_ROLES, 84u,
  OMX_EQ16_PANEL_DISPLAYS, 1u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_EQ16_LV2_PORT_ENABLED 0u
#define OMX_EQ16_LV2_PORT_LATENCY 1u
#define OMX_EQ16_LV2_PORT_IN_L 2u
#define OMX_EQ16_LV2_PORT_IN_R 3u
#define OMX_EQ16_LV2_PORT_OUT_L 4u
#define OMX_EQ16_LV2_PORT_OUT_R 5u
#define OMX_EQ16_LV2_PORT_FIRST_PARAM 6u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_EQ16_LV2_PORT_FIRST_METER 92u
#define OMX_EQ16_LV2_PORT_COUNT 92u

#endif /* OMX_EQ16_PARAMS_H */
