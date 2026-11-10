// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_DYNEQ_PARAMS_H
#define OMX_DYNEQ_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-dyneq/omx-dyneq.decl.json.
 * Regenerate: `make -C plugins/omx-dyneq gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_dyneq_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_DYNEQ_PARAM_BAND_ON1 = 0,
  OMX_DYNEQ_PARAM_BAND_ON2 = 1,
  OMX_DYNEQ_PARAM_BAND_ON3 = 2,
  OMX_DYNEQ_PARAM_BAND_ON4 = 3,
  OMX_DYNEQ_PARAM_BAND_ON5 = 4,
  OMX_DYNEQ_PARAM_BAND_ON6 = 5,
  OMX_DYNEQ_PARAM_BAND_TYPE1 = 6,
  OMX_DYNEQ_PARAM_BAND_TYPE2 = 7,
  OMX_DYNEQ_PARAM_BAND_TYPE3 = 8,
  OMX_DYNEQ_PARAM_BAND_TYPE4 = 9,
  OMX_DYNEQ_PARAM_BAND_TYPE5 = 10,
  OMX_DYNEQ_PARAM_BAND_TYPE6 = 11,
  OMX_DYNEQ_PARAM_FREQ1 = 12,
  OMX_DYNEQ_PARAM_FREQ2 = 13,
  OMX_DYNEQ_PARAM_FREQ3 = 14,
  OMX_DYNEQ_PARAM_FREQ4 = 15,
  OMX_DYNEQ_PARAM_FREQ5 = 16,
  OMX_DYNEQ_PARAM_FREQ6 = 17,
  OMX_DYNEQ_PARAM_GAIN1 = 18,
  OMX_DYNEQ_PARAM_GAIN2 = 19,
  OMX_DYNEQ_PARAM_GAIN3 = 20,
  OMX_DYNEQ_PARAM_GAIN4 = 21,
  OMX_DYNEQ_PARAM_GAIN5 = 22,
  OMX_DYNEQ_PARAM_GAIN6 = 23,
  OMX_DYNEQ_PARAM_Q1 = 24,
  OMX_DYNEQ_PARAM_Q2 = 25,
  OMX_DYNEQ_PARAM_Q3 = 26,
  OMX_DYNEQ_PARAM_Q4 = 27,
  OMX_DYNEQ_PARAM_Q5 = 28,
  OMX_DYNEQ_PARAM_Q6 = 29,
  OMX_DYNEQ_PARAM_DYN_MODE1 = 30,
  OMX_DYNEQ_PARAM_DYN_MODE2 = 31,
  OMX_DYNEQ_PARAM_DYN_MODE3 = 32,
  OMX_DYNEQ_PARAM_DYN_MODE4 = 33,
  OMX_DYNEQ_PARAM_DYN_MODE5 = 34,
  OMX_DYNEQ_PARAM_DYN_MODE6 = 35,
  OMX_DYNEQ_PARAM_THRESHOLD1 = 36,
  OMX_DYNEQ_PARAM_THRESHOLD2 = 37,
  OMX_DYNEQ_PARAM_THRESHOLD3 = 38,
  OMX_DYNEQ_PARAM_THRESHOLD4 = 39,
  OMX_DYNEQ_PARAM_THRESHOLD5 = 40,
  OMX_DYNEQ_PARAM_THRESHOLD6 = 41,
  OMX_DYNEQ_PARAM_RATIO1 = 42,
  OMX_DYNEQ_PARAM_RATIO2 = 43,
  OMX_DYNEQ_PARAM_RATIO3 = 44,
  OMX_DYNEQ_PARAM_RATIO4 = 45,
  OMX_DYNEQ_PARAM_RATIO5 = 46,
  OMX_DYNEQ_PARAM_RATIO6 = 47,
  OMX_DYNEQ_PARAM_RANGE1 = 48,
  OMX_DYNEQ_PARAM_RANGE2 = 49,
  OMX_DYNEQ_PARAM_RANGE3 = 50,
  OMX_DYNEQ_PARAM_RANGE4 = 51,
  OMX_DYNEQ_PARAM_RANGE5 = 52,
  OMX_DYNEQ_PARAM_RANGE6 = 53,
  OMX_DYNEQ_PARAM_ATTACK1 = 54,
  OMX_DYNEQ_PARAM_ATTACK2 = 55,
  OMX_DYNEQ_PARAM_ATTACK3 = 56,
  OMX_DYNEQ_PARAM_ATTACK4 = 57,
  OMX_DYNEQ_PARAM_ATTACK5 = 58,
  OMX_DYNEQ_PARAM_ATTACK6 = 59,
  OMX_DYNEQ_PARAM_RELEASE1 = 60,
  OMX_DYNEQ_PARAM_RELEASE2 = 61,
  OMX_DYNEQ_PARAM_RELEASE3 = 62,
  OMX_DYNEQ_PARAM_RELEASE4 = 63,
  OMX_DYNEQ_PARAM_RELEASE5 = 64,
  OMX_DYNEQ_PARAM_RELEASE6 = 65,
  OMX_DYNEQ_PARAM_LISTEN1 = 66,
  OMX_DYNEQ_PARAM_LISTEN2 = 67,
  OMX_DYNEQ_PARAM_LISTEN3 = 68,
  OMX_DYNEQ_PARAM_LISTEN4 = 69,
  OMX_DYNEQ_PARAM_LISTEN5 = 70,
  OMX_DYNEQ_PARAM_LISTEN6 = 71,
  OMX_DYNEQ_PARAM_MIX_PCT = 72,
  OMX_DYNEQ_PARAM_COUNT = 73
};

static const omx_plugin_param OMX_DYNEQ_PARAMS[OMX_DYNEQ_PARAM_COUNT] = {
  { "bandOn1", "Band 1 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "bandOn2", "Band 2 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "bandOn3", "Band 3 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "bandOn4", "Band 4 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "bandOn5", "Band 5 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "bandOn6", "Band 6 On", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "bandType1", "Band 1 Type", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "bandType2", "Band 2 Type", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "bandType3", "Band 3 Type", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "bandType4", "Band 4 Type", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "bandType5", "Band 5 Type", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "bandType6", "Band 6 Type", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "freq1", "Band 1 Freq", "Hz", 20.0f, 20000.0f, 40.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "freq2", "Band 2 Freq", "Hz", 20.0f, 20000.0f, 125.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "freq3", "Band 3 Freq", "Hz", 20.0f, 20000.0f, 400.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "freq4", "Band 4 Freq", "Hz", 20.0f, 20000.0f, 1250.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "freq5", "Band 5 Freq", "Hz", 20.0f, 20000.0f, 4000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "freq6", "Band 6 Freq", "Hz", 20.0f, 20000.0f, 12500.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "gain1", "Band 1 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "gain2", "Band 2 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "gain3", "Band 3 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "gain4", "Band 4 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "gain5", "Band 5 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "gain6", "Band 6 Gain", "dB", -15.0f, 15.0f, 0.0f, 0u },
  { "q1", "Band 1 Q", "", 0.3f, 8.0f, 1.0f, 0u },
  { "q2", "Band 2 Q", "", 0.3f, 8.0f, 1.0f, 0u },
  { "q3", "Band 3 Q", "", 0.3f, 8.0f, 1.0f, 0u },
  { "q4", "Band 4 Q", "", 0.3f, 8.0f, 1.0f, 0u },
  { "q5", "Band 5 Q", "", 0.3f, 8.0f, 1.0f, 0u },
  { "q6", "Band 6 Q", "", 0.3f, 8.0f, 1.0f, 0u },
  { "dynMode1", "Band 1 Dynamics", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "dynMode2", "Band 2 Dynamics", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "dynMode3", "Band 3 Dynamics", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "dynMode4", "Band 4 Dynamics", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "dynMode5", "Band 5 Dynamics", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "dynMode6", "Band 6 Dynamics", "", 0.0f, 2.0f, 0.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "threshold1", "Band 1 Threshold", "dB", -60.0f, 0.0f, -30.0f, 0u },
  { "threshold2", "Band 2 Threshold", "dB", -60.0f, 0.0f, -30.0f, 0u },
  { "threshold3", "Band 3 Threshold", "dB", -60.0f, 0.0f, -30.0f, 0u },
  { "threshold4", "Band 4 Threshold", "dB", -60.0f, 0.0f, -30.0f, 0u },
  { "threshold5", "Band 5 Threshold", "dB", -60.0f, 0.0f, -30.0f, 0u },
  { "threshold6", "Band 6 Threshold", "dB", -60.0f, 0.0f, -30.0f, 0u },
  { "ratio1", "Band 1 Ratio", "", 1.0f, 20.0f, 2.0f, 0u },
  { "ratio2", "Band 2 Ratio", "", 1.0f, 20.0f, 2.0f, 0u },
  { "ratio3", "Band 3 Ratio", "", 1.0f, 20.0f, 2.0f, 0u },
  { "ratio4", "Band 4 Ratio", "", 1.0f, 20.0f, 2.0f, 0u },
  { "ratio5", "Band 5 Ratio", "", 1.0f, 20.0f, 2.0f, 0u },
  { "ratio6", "Band 6 Ratio", "", 1.0f, 20.0f, 2.0f, 0u },
  { "range1", "Band 1 Range", "dB", -24.0f, 12.0f, -6.0f, 0u },
  { "range2", "Band 2 Range", "dB", -24.0f, 12.0f, -6.0f, 0u },
  { "range3", "Band 3 Range", "dB", -24.0f, 12.0f, -6.0f, 0u },
  { "range4", "Band 4 Range", "dB", -24.0f, 12.0f, -6.0f, 0u },
  { "range5", "Band 5 Range", "dB", -24.0f, 12.0f, -6.0f, 0u },
  { "range6", "Band 6 Range", "dB", -24.0f, 12.0f, -6.0f, 0u },
  { "attack1", "Band 1 Attack", "ms", 0.5f, 100.0f, 5.0f, 0u },
  { "attack2", "Band 2 Attack", "ms", 0.5f, 100.0f, 5.0f, 0u },
  { "attack3", "Band 3 Attack", "ms", 0.5f, 100.0f, 5.0f, 0u },
  { "attack4", "Band 4 Attack", "ms", 0.5f, 100.0f, 5.0f, 0u },
  { "attack5", "Band 5 Attack", "ms", 0.5f, 100.0f, 5.0f, 0u },
  { "attack6", "Band 6 Attack", "ms", 0.5f, 100.0f, 5.0f, 0u },
  { "release1", "Band 1 Release", "ms", 5.0f, 3000.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "release2", "Band 2 Release", "ms", 5.0f, 3000.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "release3", "Band 3 Release", "ms", 5.0f, 3000.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "release4", "Band 4 Release", "ms", 5.0f, 3000.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "release5", "Band 5 Release", "ms", 5.0f, 3000.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "release6", "Band 6 Release", "ms", 5.0f, 3000.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "listen1", "Band 1 Listen", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "listen2", "Band 2 Listen", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "listen3", "Band 3 Listen", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "listen4", "Band 4 Listen", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "listen5", "Band 5 Listen", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "listen6", "Band 6 Listen", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "mixPct", "Mix", "%", 0.0f, 100.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_DYNEQ_PARAM_MODULES[OMX_DYNEQ_PARAM_COUNT] = {
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 6",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 6",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 6",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 6",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 6",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 6",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 6",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 6",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 6",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 6",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 6",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 6",
  "Output",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_DYNEQ_PARAM_BAND_ON1_MIN 0.0f
#define OMX_DYNEQ_PARAM_BAND_ON1_MAX 1.0f
#define OMX_DYNEQ_PARAM_BAND_ON1_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_BAND_ON2_MIN 0.0f
#define OMX_DYNEQ_PARAM_BAND_ON2_MAX 1.0f
#define OMX_DYNEQ_PARAM_BAND_ON2_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_BAND_ON3_MIN 0.0f
#define OMX_DYNEQ_PARAM_BAND_ON3_MAX 1.0f
#define OMX_DYNEQ_PARAM_BAND_ON3_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_BAND_ON4_MIN 0.0f
#define OMX_DYNEQ_PARAM_BAND_ON4_MAX 1.0f
#define OMX_DYNEQ_PARAM_BAND_ON4_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_BAND_ON5_MIN 0.0f
#define OMX_DYNEQ_PARAM_BAND_ON5_MAX 1.0f
#define OMX_DYNEQ_PARAM_BAND_ON5_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_BAND_ON6_MIN 0.0f
#define OMX_DYNEQ_PARAM_BAND_ON6_MAX 1.0f
#define OMX_DYNEQ_PARAM_BAND_ON6_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE1_MIN 0.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE1_MAX 2.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE1_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE2_MIN 0.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE2_MAX 2.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE2_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE3_MIN 0.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE3_MAX 2.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE3_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE4_MIN 0.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE4_MAX 2.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE4_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE5_MIN 0.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE5_MAX 2.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE5_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE6_MIN 0.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE6_MAX 2.0f
#define OMX_DYNEQ_PARAM_BAND_TYPE6_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_FREQ1_MIN 20.0f
#define OMX_DYNEQ_PARAM_FREQ1_MAX 20000.0f
#define OMX_DYNEQ_PARAM_FREQ1_DEFAULT 40.0f
#define OMX_DYNEQ_PARAM_FREQ2_MIN 20.0f
#define OMX_DYNEQ_PARAM_FREQ2_MAX 20000.0f
#define OMX_DYNEQ_PARAM_FREQ2_DEFAULT 125.0f
#define OMX_DYNEQ_PARAM_FREQ3_MIN 20.0f
#define OMX_DYNEQ_PARAM_FREQ3_MAX 20000.0f
#define OMX_DYNEQ_PARAM_FREQ3_DEFAULT 400.0f
#define OMX_DYNEQ_PARAM_FREQ4_MIN 20.0f
#define OMX_DYNEQ_PARAM_FREQ4_MAX 20000.0f
#define OMX_DYNEQ_PARAM_FREQ4_DEFAULT 1250.0f
#define OMX_DYNEQ_PARAM_FREQ5_MIN 20.0f
#define OMX_DYNEQ_PARAM_FREQ5_MAX 20000.0f
#define OMX_DYNEQ_PARAM_FREQ5_DEFAULT 4000.0f
#define OMX_DYNEQ_PARAM_FREQ6_MIN 20.0f
#define OMX_DYNEQ_PARAM_FREQ6_MAX 20000.0f
#define OMX_DYNEQ_PARAM_FREQ6_DEFAULT 12500.0f
#define OMX_DYNEQ_PARAM_GAIN1_MIN -15.0f
#define OMX_DYNEQ_PARAM_GAIN1_MAX 15.0f
#define OMX_DYNEQ_PARAM_GAIN1_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_GAIN2_MIN -15.0f
#define OMX_DYNEQ_PARAM_GAIN2_MAX 15.0f
#define OMX_DYNEQ_PARAM_GAIN2_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_GAIN3_MIN -15.0f
#define OMX_DYNEQ_PARAM_GAIN3_MAX 15.0f
#define OMX_DYNEQ_PARAM_GAIN3_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_GAIN4_MIN -15.0f
#define OMX_DYNEQ_PARAM_GAIN4_MAX 15.0f
#define OMX_DYNEQ_PARAM_GAIN4_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_GAIN5_MIN -15.0f
#define OMX_DYNEQ_PARAM_GAIN5_MAX 15.0f
#define OMX_DYNEQ_PARAM_GAIN5_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_GAIN6_MIN -15.0f
#define OMX_DYNEQ_PARAM_GAIN6_MAX 15.0f
#define OMX_DYNEQ_PARAM_GAIN6_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_Q1_MIN 0.3f
#define OMX_DYNEQ_PARAM_Q1_MAX 8.0f
#define OMX_DYNEQ_PARAM_Q1_DEFAULT 1.0f
#define OMX_DYNEQ_PARAM_Q2_MIN 0.3f
#define OMX_DYNEQ_PARAM_Q2_MAX 8.0f
#define OMX_DYNEQ_PARAM_Q2_DEFAULT 1.0f
#define OMX_DYNEQ_PARAM_Q3_MIN 0.3f
#define OMX_DYNEQ_PARAM_Q3_MAX 8.0f
#define OMX_DYNEQ_PARAM_Q3_DEFAULT 1.0f
#define OMX_DYNEQ_PARAM_Q4_MIN 0.3f
#define OMX_DYNEQ_PARAM_Q4_MAX 8.0f
#define OMX_DYNEQ_PARAM_Q4_DEFAULT 1.0f
#define OMX_DYNEQ_PARAM_Q5_MIN 0.3f
#define OMX_DYNEQ_PARAM_Q5_MAX 8.0f
#define OMX_DYNEQ_PARAM_Q5_DEFAULT 1.0f
#define OMX_DYNEQ_PARAM_Q6_MIN 0.3f
#define OMX_DYNEQ_PARAM_Q6_MAX 8.0f
#define OMX_DYNEQ_PARAM_Q6_DEFAULT 1.0f
#define OMX_DYNEQ_PARAM_DYN_MODE1_MIN 0.0f
#define OMX_DYNEQ_PARAM_DYN_MODE1_MAX 2.0f
#define OMX_DYNEQ_PARAM_DYN_MODE1_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_DYN_MODE2_MIN 0.0f
#define OMX_DYNEQ_PARAM_DYN_MODE2_MAX 2.0f
#define OMX_DYNEQ_PARAM_DYN_MODE2_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_DYN_MODE3_MIN 0.0f
#define OMX_DYNEQ_PARAM_DYN_MODE3_MAX 2.0f
#define OMX_DYNEQ_PARAM_DYN_MODE3_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_DYN_MODE4_MIN 0.0f
#define OMX_DYNEQ_PARAM_DYN_MODE4_MAX 2.0f
#define OMX_DYNEQ_PARAM_DYN_MODE4_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_DYN_MODE5_MIN 0.0f
#define OMX_DYNEQ_PARAM_DYN_MODE5_MAX 2.0f
#define OMX_DYNEQ_PARAM_DYN_MODE5_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_DYN_MODE6_MIN 0.0f
#define OMX_DYNEQ_PARAM_DYN_MODE6_MAX 2.0f
#define OMX_DYNEQ_PARAM_DYN_MODE6_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_THRESHOLD1_MIN -60.0f
#define OMX_DYNEQ_PARAM_THRESHOLD1_MAX 0.0f
#define OMX_DYNEQ_PARAM_THRESHOLD1_DEFAULT -30.0f
#define OMX_DYNEQ_PARAM_THRESHOLD2_MIN -60.0f
#define OMX_DYNEQ_PARAM_THRESHOLD2_MAX 0.0f
#define OMX_DYNEQ_PARAM_THRESHOLD2_DEFAULT -30.0f
#define OMX_DYNEQ_PARAM_THRESHOLD3_MIN -60.0f
#define OMX_DYNEQ_PARAM_THRESHOLD3_MAX 0.0f
#define OMX_DYNEQ_PARAM_THRESHOLD3_DEFAULT -30.0f
#define OMX_DYNEQ_PARAM_THRESHOLD4_MIN -60.0f
#define OMX_DYNEQ_PARAM_THRESHOLD4_MAX 0.0f
#define OMX_DYNEQ_PARAM_THRESHOLD4_DEFAULT -30.0f
#define OMX_DYNEQ_PARAM_THRESHOLD5_MIN -60.0f
#define OMX_DYNEQ_PARAM_THRESHOLD5_MAX 0.0f
#define OMX_DYNEQ_PARAM_THRESHOLD5_DEFAULT -30.0f
#define OMX_DYNEQ_PARAM_THRESHOLD6_MIN -60.0f
#define OMX_DYNEQ_PARAM_THRESHOLD6_MAX 0.0f
#define OMX_DYNEQ_PARAM_THRESHOLD6_DEFAULT -30.0f
#define OMX_DYNEQ_PARAM_RATIO1_MIN 1.0f
#define OMX_DYNEQ_PARAM_RATIO1_MAX 20.0f
#define OMX_DYNEQ_PARAM_RATIO1_DEFAULT 2.0f
#define OMX_DYNEQ_PARAM_RATIO2_MIN 1.0f
#define OMX_DYNEQ_PARAM_RATIO2_MAX 20.0f
#define OMX_DYNEQ_PARAM_RATIO2_DEFAULT 2.0f
#define OMX_DYNEQ_PARAM_RATIO3_MIN 1.0f
#define OMX_DYNEQ_PARAM_RATIO3_MAX 20.0f
#define OMX_DYNEQ_PARAM_RATIO3_DEFAULT 2.0f
#define OMX_DYNEQ_PARAM_RATIO4_MIN 1.0f
#define OMX_DYNEQ_PARAM_RATIO4_MAX 20.0f
#define OMX_DYNEQ_PARAM_RATIO4_DEFAULT 2.0f
#define OMX_DYNEQ_PARAM_RATIO5_MIN 1.0f
#define OMX_DYNEQ_PARAM_RATIO5_MAX 20.0f
#define OMX_DYNEQ_PARAM_RATIO5_DEFAULT 2.0f
#define OMX_DYNEQ_PARAM_RATIO6_MIN 1.0f
#define OMX_DYNEQ_PARAM_RATIO6_MAX 20.0f
#define OMX_DYNEQ_PARAM_RATIO6_DEFAULT 2.0f
#define OMX_DYNEQ_PARAM_RANGE1_MIN -24.0f
#define OMX_DYNEQ_PARAM_RANGE1_MAX 12.0f
#define OMX_DYNEQ_PARAM_RANGE1_DEFAULT -6.0f
#define OMX_DYNEQ_PARAM_RANGE2_MIN -24.0f
#define OMX_DYNEQ_PARAM_RANGE2_MAX 12.0f
#define OMX_DYNEQ_PARAM_RANGE2_DEFAULT -6.0f
#define OMX_DYNEQ_PARAM_RANGE3_MIN -24.0f
#define OMX_DYNEQ_PARAM_RANGE3_MAX 12.0f
#define OMX_DYNEQ_PARAM_RANGE3_DEFAULT -6.0f
#define OMX_DYNEQ_PARAM_RANGE4_MIN -24.0f
#define OMX_DYNEQ_PARAM_RANGE4_MAX 12.0f
#define OMX_DYNEQ_PARAM_RANGE4_DEFAULT -6.0f
#define OMX_DYNEQ_PARAM_RANGE5_MIN -24.0f
#define OMX_DYNEQ_PARAM_RANGE5_MAX 12.0f
#define OMX_DYNEQ_PARAM_RANGE5_DEFAULT -6.0f
#define OMX_DYNEQ_PARAM_RANGE6_MIN -24.0f
#define OMX_DYNEQ_PARAM_RANGE6_MAX 12.0f
#define OMX_DYNEQ_PARAM_RANGE6_DEFAULT -6.0f
#define OMX_DYNEQ_PARAM_ATTACK1_MIN 0.5f
#define OMX_DYNEQ_PARAM_ATTACK1_MAX 100.0f
#define OMX_DYNEQ_PARAM_ATTACK1_DEFAULT 5.0f
#define OMX_DYNEQ_PARAM_ATTACK2_MIN 0.5f
#define OMX_DYNEQ_PARAM_ATTACK2_MAX 100.0f
#define OMX_DYNEQ_PARAM_ATTACK2_DEFAULT 5.0f
#define OMX_DYNEQ_PARAM_ATTACK3_MIN 0.5f
#define OMX_DYNEQ_PARAM_ATTACK3_MAX 100.0f
#define OMX_DYNEQ_PARAM_ATTACK3_DEFAULT 5.0f
#define OMX_DYNEQ_PARAM_ATTACK4_MIN 0.5f
#define OMX_DYNEQ_PARAM_ATTACK4_MAX 100.0f
#define OMX_DYNEQ_PARAM_ATTACK4_DEFAULT 5.0f
#define OMX_DYNEQ_PARAM_ATTACK5_MIN 0.5f
#define OMX_DYNEQ_PARAM_ATTACK5_MAX 100.0f
#define OMX_DYNEQ_PARAM_ATTACK5_DEFAULT 5.0f
#define OMX_DYNEQ_PARAM_ATTACK6_MIN 0.5f
#define OMX_DYNEQ_PARAM_ATTACK6_MAX 100.0f
#define OMX_DYNEQ_PARAM_ATTACK6_DEFAULT 5.0f
#define OMX_DYNEQ_PARAM_RELEASE1_MIN 5.0f
#define OMX_DYNEQ_PARAM_RELEASE1_MAX 3000.0f
#define OMX_DYNEQ_PARAM_RELEASE1_DEFAULT 100.0f
#define OMX_DYNEQ_PARAM_RELEASE2_MIN 5.0f
#define OMX_DYNEQ_PARAM_RELEASE2_MAX 3000.0f
#define OMX_DYNEQ_PARAM_RELEASE2_DEFAULT 100.0f
#define OMX_DYNEQ_PARAM_RELEASE3_MIN 5.0f
#define OMX_DYNEQ_PARAM_RELEASE3_MAX 3000.0f
#define OMX_DYNEQ_PARAM_RELEASE3_DEFAULT 100.0f
#define OMX_DYNEQ_PARAM_RELEASE4_MIN 5.0f
#define OMX_DYNEQ_PARAM_RELEASE4_MAX 3000.0f
#define OMX_DYNEQ_PARAM_RELEASE4_DEFAULT 100.0f
#define OMX_DYNEQ_PARAM_RELEASE5_MIN 5.0f
#define OMX_DYNEQ_PARAM_RELEASE5_MAX 3000.0f
#define OMX_DYNEQ_PARAM_RELEASE5_DEFAULT 100.0f
#define OMX_DYNEQ_PARAM_RELEASE6_MIN 5.0f
#define OMX_DYNEQ_PARAM_RELEASE6_MAX 3000.0f
#define OMX_DYNEQ_PARAM_RELEASE6_DEFAULT 100.0f
#define OMX_DYNEQ_PARAM_LISTEN1_MIN 0.0f
#define OMX_DYNEQ_PARAM_LISTEN1_MAX 1.0f
#define OMX_DYNEQ_PARAM_LISTEN1_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_LISTEN2_MIN 0.0f
#define OMX_DYNEQ_PARAM_LISTEN2_MAX 1.0f
#define OMX_DYNEQ_PARAM_LISTEN2_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_LISTEN3_MIN 0.0f
#define OMX_DYNEQ_PARAM_LISTEN3_MAX 1.0f
#define OMX_DYNEQ_PARAM_LISTEN3_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_LISTEN4_MIN 0.0f
#define OMX_DYNEQ_PARAM_LISTEN4_MAX 1.0f
#define OMX_DYNEQ_PARAM_LISTEN4_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_LISTEN5_MIN 0.0f
#define OMX_DYNEQ_PARAM_LISTEN5_MAX 1.0f
#define OMX_DYNEQ_PARAM_LISTEN5_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_LISTEN6_MIN 0.0f
#define OMX_DYNEQ_PARAM_LISTEN6_MAX 1.0f
#define OMX_DYNEQ_PARAM_LISTEN6_DEFAULT 0.0f
#define OMX_DYNEQ_PARAM_MIX_PCT_MIN 0.0f
#define OMX_DYNEQ_PARAM_MIX_PCT_MAX 100.0f
#define OMX_DYNEQ_PARAM_MIX_PCT_DEFAULT 100.0f

/* The identity every face publishes. */
#define OMX_DYNEQ_NAME "omx dyneq"
#define OMX_DYNEQ_VENDOR "openmixer"
#define OMX_DYNEQ_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_DYNEQ_VERSION "0.19.2"
#define OMX_DYNEQ_DESCRIPTION "A six-band dynamic EQ: each band is an EQ bell or shelf whose gain also moves with the level of its own band, cutting above a threshold (down) or lifting below it (up), held to a range, so a harsh or boomy band is tamed only while it is too loud."
#define OMX_DYNEQ_CLAP_ID "org.openmixer.dyneq"
#define OMX_DYNEQ_CLAP_FEATURES "audio-effect", "equalizer", "stereo"
#define OMX_DYNEQ_LV2_URI "urn:openmixer:dyneq"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_DYNEQ_DECL_SOURCE "omx dyneq: Band 1 On 0 to 1, Band 2 On 0 to 1, Band 3 On 0 to 1, Band 4 On 0 to 1, Band 5 On 0 to 1, Band 6 On 0 to 1, Band 1 Type 0 to 2, Band 2 Type 0 to 2, Band 3 Type 0 to 2, Band 4 Type 0 to 2, Band 5 Type 0 to 2, Band 6 Type 0 to 2, Band 1 Freq 20 to 20000 Hz, Band 2 Freq 20 to 20000 Hz, Band 3 Freq 20 to 20000 Hz, Band 4 Freq 20 to 20000 Hz, Band 5 Freq 20 to 20000 Hz, Band 6 Freq 20 to 20000 Hz, Band 1 Gain -15 to 15 dB, Band 2 Gain -15 to 15 dB, Band 3 Gain -15 to 15 dB, Band 4 Gain -15 to 15 dB, Band 5 Gain -15 to 15 dB, Band 6 Gain -15 to 15 dB, Band 1 Q 0.3 to 8, Band 2 Q 0.3 to 8, Band 3 Q 0.3 to 8, Band 4 Q 0.3 to 8, Band 5 Q 0.3 to 8, Band 6 Q 0.3 to 8, Band 1 Dynamics 0 to 2, Band 2 Dynamics 0 to 2, Band 3 Dynamics 0 to 2, Band 4 Dynamics 0 to 2, Band 5 Dynamics 0 to 2, Band 6 Dynamics 0 to 2, Band 1 Threshold -60 to 0 dB, Band 2 Threshold -60 to 0 dB, Band 3 Threshold -60 to 0 dB, Band 4 Threshold -60 to 0 dB, Band 5 Threshold -60 to 0 dB, Band 6 Threshold -60 to 0 dB, Band 1 Ratio 1 to 20, Band 2 Ratio 1 to 20, Band 3 Ratio 1 to 20, Band 4 Ratio 1 to 20, Band 5 Ratio 1 to 20, Band 6 Ratio 1 to 20, Band 1 Range -24 to 12 dB, Band 2 Range -24 to 12 dB, Band 3 Range -24 to 12 dB, Band 4 Range -24 to 12 dB, Band 5 Range -24 to 12 dB, Band 6 Range -24 to 12 dB, Band 1 Attack 0.5 to 100 ms, Band 2 Attack 0.5 to 100 ms, Band 3 Attack 0.5 to 100 ms, Band 4 Attack 0.5 to 100 ms, Band 5 Attack 0.5 to 100 ms, Band 6 Attack 0.5 to 100 ms, Band 1 Release 5 to 3000 ms, Band 2 Release 5 to 3000 ms, Band 3 Release 5 to 3000 ms, Band 4 Release 5 to 3000 ms, Band 5 Release 5 to 3000 ms, Band 6 Release 5 to 3000 ms, Band 1 Listen 0 to 1, Band 2 Listen 0 to 1, Band 3 Listen 0 to 1, Band 4 Listen 0 to 1, Band 5 Listen 0 to 1, Band 6 Listen 0 to 1, Mix 0 to 100 %"
#define OMX_DYNEQ_DECL_DIGEST "42f922522e41342fe1d9de839ecccca9f1537956c5418067f09bbe0ec8b15024"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (omx-contract), is the face's. */
#define OMX_DYNEQ_PARAM_CONTRACT_VERSION 2u
#define OMX_DYNEQ_PARAM_CONTRACT_FINGERPRINT "fa72b31fef51f2de8c2690bda7d8f5b09f76032186bda513f187a0b70b56aa64"
static const struct omx_param_contract_row OMX_DYNEQ_PARAM_CONTRACT[OMX_DYNEQ_PARAM_COUNT] = {
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandOn1 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandOn2 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandOn3 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandOn4 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandOn5 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandOn6 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandType1 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandType2 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandType3 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandType4 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandType5 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandType6 none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* freq1 hz */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* freq2 hz */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* freq3 hz */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* freq4 hz */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* freq5 hz */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* freq6 hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* gain1 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* gain2 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* gain3 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* gain4 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* gain5 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* gain6 db */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* q1 none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* q2 none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* q3 none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* q4 none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* q5 none */
  { 0u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* q6 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* dynMode1 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* dynMode2 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* dynMode3 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* dynMode4 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* dynMode5 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* dynMode6 none */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* threshold1 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* threshold2 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* threshold3 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* threshold4 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* threshold5 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* threshold6 db */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio1 ratio */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio2 ratio */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio3 ratio */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio4 ratio */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio5 ratio */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio6 ratio */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* range1 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* range2 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* range3 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* range4 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* range5 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* range6 db */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attack1 ms */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attack2 ms */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attack3 ms */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attack4 ms */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attack5 ms */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attack6 ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* release1 ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* release2 ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* release3 ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* release4 ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* release5 ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* release6 ms */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* listen1 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* listen2 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* listen3 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* listen4 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* listen5 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* listen6 none */
  { 14u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* mixPct pc */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_DYNEQ_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family eq. */
#define OMX_DYNEQ_PANEL_FINGERPRINT "71d3f75cf894b2975fddbad6ee8725ae50979ff4bc7cab31a827f2b55c7c851c"
static const struct omx_panel_section_row OMX_DYNEQ_PANEL_SECTIONS[] = {
  { "band1", "fx.dyneq.section.band1", "Band 1" },
  { "band2", "fx.dyneq.section.band2", "Band 2" },
  { "band3", "fx.dyneq.section.band3", "Band 3" },
  { "band4", "fx.dyneq.section.band4", "Band 4" },
  { "band5", "fx.dyneq.section.band5", "Band 5" },
  { "band6", "fx.dyneq.section.band6", "Band 6" },
  { "output", "fx.dyneq.section.output", "Output" },
};
static const struct omx_panel_control_row OMX_DYNEQ_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.dyneq.band_on1", "Band 1 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.dyneq.band_type1", "Band 1 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 12u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.freq1", "Band 1 Freq", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 18u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.gain1", "Band 1 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 24u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.q1", "Band 1 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 30u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.dyneq.dyn_mode1", "Band 1 Dynamics", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 36u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.threshold1", "Band 1 Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 42u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.ratio1", "Band 1 Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 48u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.range1", "Band 1 Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 54u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.attack1", "Band 1 Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 60u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.release1", "Band 1 Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 66u, 0u, OMX_CLAP_WIDGET_SWITCH, "fx.dyneq.listen1", "Band 1 Listen", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 1u, OMX_CLAP_WIDGET_SWITCH, "fx.dyneq.band_on2", "Band 2 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 1u, OMX_CLAP_WIDGET_AUTO, "fx.dyneq.band_type2", "Band 2 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 13u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.freq2", "Band 2 Freq", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 19u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.gain2", "Band 2 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 25u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.q2", "Band 2 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 31u, 1u, OMX_CLAP_WIDGET_AUTO, "fx.dyneq.dyn_mode2", "Band 2 Dynamics", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 37u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.threshold2", "Band 2 Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 43u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.ratio2", "Band 2 Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 49u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.range2", "Band 2 Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 55u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.attack2", "Band 2 Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 61u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.release2", "Band 2 Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 67u, 1u, OMX_CLAP_WIDGET_SWITCH, "fx.dyneq.listen2", "Band 2 Listen", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 2u, OMX_CLAP_WIDGET_SWITCH, "fx.dyneq.band_on3", "Band 3 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 8u, 2u, OMX_CLAP_WIDGET_AUTO, "fx.dyneq.band_type3", "Band 3 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 14u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.freq3", "Band 3 Freq", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 20u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.gain3", "Band 3 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 26u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.q3", "Band 3 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 32u, 2u, OMX_CLAP_WIDGET_AUTO, "fx.dyneq.dyn_mode3", "Band 3 Dynamics", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 38u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.threshold3", "Band 3 Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 44u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.ratio3", "Band 3 Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 50u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.range3", "Band 3 Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 56u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.attack3", "Band 3 Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 62u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.release3", "Band 3 Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 68u, 2u, OMX_CLAP_WIDGET_SWITCH, "fx.dyneq.listen3", "Band 3 Listen", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 3u, OMX_CLAP_WIDGET_SWITCH, "fx.dyneq.band_on4", "Band 4 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 9u, 3u, OMX_CLAP_WIDGET_AUTO, "fx.dyneq.band_type4", "Band 4 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 15u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.freq4", "Band 4 Freq", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 21u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.gain4", "Band 4 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 27u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.q4", "Band 4 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 33u, 3u, OMX_CLAP_WIDGET_AUTO, "fx.dyneq.dyn_mode4", "Band 4 Dynamics", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 39u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.threshold4", "Band 4 Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 45u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.ratio4", "Band 4 Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 51u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.range4", "Band 4 Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 57u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.attack4", "Band 4 Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 63u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.release4", "Band 4 Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 69u, 3u, OMX_CLAP_WIDGET_SWITCH, "fx.dyneq.listen4", "Band 4 Listen", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 4u, OMX_CLAP_WIDGET_SWITCH, "fx.dyneq.band_on5", "Band 5 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 10u, 4u, OMX_CLAP_WIDGET_AUTO, "fx.dyneq.band_type5", "Band 5 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 16u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.freq5", "Band 5 Freq", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 22u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.gain5", "Band 5 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 28u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.q5", "Band 5 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 34u, 4u, OMX_CLAP_WIDGET_AUTO, "fx.dyneq.dyn_mode5", "Band 5 Dynamics", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 40u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.threshold5", "Band 5 Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 46u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.ratio5", "Band 5 Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 52u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.range5", "Band 5 Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 58u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.attack5", "Band 5 Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 64u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.release5", "Band 5 Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 70u, 4u, OMX_CLAP_WIDGET_SWITCH, "fx.dyneq.listen5", "Band 5 Listen", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 5u, OMX_CLAP_WIDGET_SWITCH, "fx.dyneq.band_on6", "Band 6 On", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 11u, 5u, OMX_CLAP_WIDGET_AUTO, "fx.dyneq.band_type6", "Band 6 Type", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 17u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.freq6", "Band 6 Freq", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 23u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.gain6", "Band 6 Gain", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 29u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.q6", "Band 6 Q", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 35u, 5u, OMX_CLAP_WIDGET_AUTO, "fx.dyneq.dyn_mode6", "Band 6 Dynamics", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 41u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.threshold6", "Band 6 Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 47u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.ratio6", "Band 6 Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 53u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.range6", "Band 6 Range", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 59u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.attack6", "Band 6 Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 65u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.release6", "Band 6 Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 71u, 5u, OMX_CLAP_WIDGET_SWITCH, "fx.dyneq.listen6", "Band 6 Listen", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 72u, 6u, OMX_CLAP_WIDGET_KNOB, "fx.dyneq.mix_pct", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_DYNEQ_PANEL_ROLES[] = {
  { "band_on", 0u, 0u },
  { "band_on", 1u, 1u },
  { "band_on", 2u, 2u },
  { "band_on", 3u, 3u },
  { "band_on", 4u, 4u },
  { "band_on", 5u, 5u },
  { "band_type", 0u, 6u },
  { "band_type", 1u, 7u },
  { "band_type", 2u, 8u },
  { "band_type", 3u, 9u },
  { "band_type", 4u, 10u },
  { "band_type", 5u, 11u },
  { "band_freq", 0u, 12u },
  { "band_freq", 1u, 13u },
  { "band_freq", 2u, 14u },
  { "band_freq", 3u, 15u },
  { "band_freq", 4u, 16u },
  { "band_freq", 5u, 17u },
  { "band_gain", 0u, 18u },
  { "band_gain", 1u, 19u },
  { "band_gain", 2u, 20u },
  { "band_gain", 3u, 21u },
  { "band_gain", 4u, 22u },
  { "band_gain", 5u, 23u },
  { "band_q", 0u, 24u },
  { "band_q", 1u, 25u },
  { "band_q", 2u, 26u },
  { "band_q", 3u, 27u },
  { "band_q", 4u, 28u },
  { "band_q", 5u, 29u },
};
static const struct omx_panel_display_row OMX_DYNEQ_PANEL_DISPLAYS[] = {
  { OMX_CLAP_DISPLAY_EQ_RESPONSE, 0u, OMX_CLAP_DISPLAY_FROM_FAMILY, { OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.dyneq.display.eq_response", "Band 1" },
  { OMX_CLAP_DISPLAY_EQ_RESPONSE, 1u, OMX_CLAP_DISPLAY_FROM_FAMILY, { OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.dyneq.display.eq_response", "Band 2" },
  { OMX_CLAP_DISPLAY_EQ_RESPONSE, 2u, OMX_CLAP_DISPLAY_FROM_FAMILY, { OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.dyneq.display.eq_response", "Band 3" },
  { OMX_CLAP_DISPLAY_EQ_RESPONSE, 3u, OMX_CLAP_DISPLAY_FROM_FAMILY, { OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.dyneq.display.eq_response", "Band 4" },
  { OMX_CLAP_DISPLAY_EQ_RESPONSE, 4u, OMX_CLAP_DISPLAY_FROM_FAMILY, { OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.dyneq.display.eq_response", "Band 5" },
  { OMX_CLAP_DISPLAY_EQ_RESPONSE, 5u, OMX_CLAP_DISPLAY_FROM_FAMILY, { OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.dyneq.display.eq_response", "Band 6" },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_DYNEQ_PANEL_PAGES[] = {
  { "Easy", "Easy", { 18u, 19u, 20u, 21u, 22u, 23u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Band 1", "Band 1: Band 1 On - Band 1 Range", { 0u, 6u, 12u, 24u, 30u, 36u, 42u, 48u } },
  { "Band 1", "Band 1: Band 1 Attack - Band 1 Listen", { 54u, 60u, 66u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Band 2", "Band 2: Band 2 On - Band 2 Range", { 1u, 7u, 13u, 25u, 31u, 37u, 43u, 49u } },
  { "Band 2", "Band 2: Band 2 Attack - Band 2 Listen", { 55u, 61u, 67u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Band 3", "Band 3: Band 3 On - Band 3 Range", { 2u, 8u, 14u, 26u, 32u, 38u, 44u, 50u } },
  { "Band 3", "Band 3: Band 3 Attack - Band 3 Listen", { 56u, 62u, 68u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Band 4", "Band 4: Band 4 On - Band 4 Range", { 3u, 9u, 15u, 27u, 33u, 39u, 45u, 51u } },
  { "Band 4", "Band 4: Band 4 Attack - Band 4 Listen", { 57u, 63u, 69u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Band 5", "Band 5: Band 5 On - Band 5 Range", { 4u, 10u, 16u, 28u, 34u, 40u, 46u, 52u } },
  { "Band 5", "Band 5: Band 5 Attack - Band 5 Listen", { 58u, 64u, 70u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Band 6", "Band 6: Band 6 On - Band 6 Range", { 5u, 11u, 17u, 29u, 35u, 41u, 47u, 53u } },
  { "Band 6", "Band 6: Band 6 Attack - Band 6 Listen", { 59u, 65u, 71u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Output", "Output", { 72u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_DYNEQ_PANEL_PAGE_COUNT 14u
static const struct omx_panel_rows OMX_DYNEQ_PANEL = {
  OMX_DYNEQ_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_EQ,
  OMX_DYNEQ_PANEL_SECTIONS, 7u,
  OMX_DYNEQ_PANEL_CONTROLS, 73u,
  OMX_DYNEQ_PANEL_ROLES, 30u,
  OMX_DYNEQ_PANEL_DISPLAYS, 6u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_DYNEQ_LV2_PORT_ENABLED 0u
#define OMX_DYNEQ_LV2_PORT_LATENCY 1u
#define OMX_DYNEQ_LV2_PORT_IN_L 8u
#define OMX_DYNEQ_LV2_PORT_IN_R 9u
#define OMX_DYNEQ_LV2_PORT_OUT_L 10u
#define OMX_DYNEQ_LV2_PORT_OUT_R 11u
#define OMX_DYNEQ_LV2_PORT_FIRST_PARAM 12u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_DYNEQ_LV2_PORT_FIRST_METER 2u
#define OMX_DYNEQ_LV2_PORT_COUNT 85u

#endif /* OMX_DYNEQ_PARAMS_H */
