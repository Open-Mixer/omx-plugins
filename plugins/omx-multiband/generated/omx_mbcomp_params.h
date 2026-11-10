// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#ifndef OMX_MBCOMP_PARAMS_H
#define OMX_MBCOMP_PARAMS_H
/*
 * GENERATED — DO NOT EDIT BY HAND.
 * Produced by tools/gen.mjs from plugins/omx-multiband/omx-multiband.decl.json.
 * Regenerate: `make -C plugins/omx-multiband gen`, then commit the result. Order is append-only.
 * The include guard is omx-dsp's own <omxdsp/params/omx_mbcomp_params.h>: included first, this
 * table is the one the kernel's instance header reads.
 */
#include <stddef.h>

#include <omxcontract/omx_resource_hints.h>

#include "omx_panel_description.h"
#include "omx_param_contract.h"
#include "omx_plugin_param.h"

enum {
  OMX_MBCOMP_PARAM_BANDS = 0,
  OMX_MBCOMP_PARAM_XOVER_HZ1 = 1,
  OMX_MBCOMP_PARAM_XOVER_HZ2 = 2,
  OMX_MBCOMP_PARAM_XOVER_HZ3 = 3,
  OMX_MBCOMP_PARAM_XOVER_HZ4 = 4,
  OMX_MBCOMP_PARAM_THRESHOLD_DB1 = 5,
  OMX_MBCOMP_PARAM_THRESHOLD_DB2 = 6,
  OMX_MBCOMP_PARAM_THRESHOLD_DB3 = 7,
  OMX_MBCOMP_PARAM_THRESHOLD_DB4 = 8,
  OMX_MBCOMP_PARAM_THRESHOLD_DB5 = 9,
  OMX_MBCOMP_PARAM_RATIO1 = 10,
  OMX_MBCOMP_PARAM_RATIO2 = 11,
  OMX_MBCOMP_PARAM_RATIO3 = 12,
  OMX_MBCOMP_PARAM_RATIO4 = 13,
  OMX_MBCOMP_PARAM_RATIO5 = 14,
  OMX_MBCOMP_PARAM_ATTACK_MS1 = 15,
  OMX_MBCOMP_PARAM_ATTACK_MS2 = 16,
  OMX_MBCOMP_PARAM_ATTACK_MS3 = 17,
  OMX_MBCOMP_PARAM_ATTACK_MS4 = 18,
  OMX_MBCOMP_PARAM_ATTACK_MS5 = 19,
  OMX_MBCOMP_PARAM_RELEASE_MS1 = 20,
  OMX_MBCOMP_PARAM_RELEASE_MS2 = 21,
  OMX_MBCOMP_PARAM_RELEASE_MS3 = 22,
  OMX_MBCOMP_PARAM_RELEASE_MS4 = 23,
  OMX_MBCOMP_PARAM_RELEASE_MS5 = 24,
  OMX_MBCOMP_PARAM_MAKEUP_DB1 = 25,
  OMX_MBCOMP_PARAM_MAKEUP_DB2 = 26,
  OMX_MBCOMP_PARAM_MAKEUP_DB3 = 27,
  OMX_MBCOMP_PARAM_MAKEUP_DB4 = 28,
  OMX_MBCOMP_PARAM_MAKEUP_DB5 = 29,
  OMX_MBCOMP_PARAM_SOLO1 = 30,
  OMX_MBCOMP_PARAM_SOLO2 = 31,
  OMX_MBCOMP_PARAM_SOLO3 = 32,
  OMX_MBCOMP_PARAM_SOLO4 = 33,
  OMX_MBCOMP_PARAM_SOLO5 = 34,
  OMX_MBCOMP_PARAM_BAND_BYPASS1 = 35,
  OMX_MBCOMP_PARAM_BAND_BYPASS2 = 36,
  OMX_MBCOMP_PARAM_BAND_BYPASS3 = 37,
  OMX_MBCOMP_PARAM_BAND_BYPASS4 = 38,
  OMX_MBCOMP_PARAM_BAND_BYPASS5 = 39,
  OMX_MBCOMP_PARAM_MIX_PCT = 40,
  OMX_MBCOMP_PARAM_OUTPUT_DB = 41,
  OMX_MBCOMP_PARAM_COUNT = 42
};

static const omx_plugin_param OMX_MBCOMP_PARAMS[OMX_MBCOMP_PARAM_COUNT] = {
  { "bands", "Bands", "", 2.0f, 5.0f, 3.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "xoverHz1", "Crossover 1", "Hz", 40.0f, 16000.0f, 200.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "xoverHz2", "Crossover 2", "Hz", 40.0f, 16000.0f, 2000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "xoverHz3", "Crossover 3", "Hz", 40.0f, 16000.0f, 5000.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "xoverHz4", "Crossover 4", "Hz", 40.0f, 16000.0f, 6400.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "thresholdDb1", "Band 1 Threshold", "dB", -60.0f, 0.0f, -20.0f, 0u },
  { "thresholdDb2", "Band 2 Threshold", "dB", -60.0f, 0.0f, -20.0f, 0u },
  { "thresholdDb3", "Band 3 Threshold", "dB", -60.0f, 0.0f, -20.0f, 0u },
  { "thresholdDb4", "Band 4 Threshold", "dB", -60.0f, 0.0f, -20.0f, 0u },
  { "thresholdDb5", "Band 5 Threshold", "dB", -60.0f, 0.0f, -20.0f, 0u },
  { "ratio1", "Band 1 Ratio", "", 1.0f, 20.0f, 2.0f, 0u },
  { "ratio2", "Band 2 Ratio", "", 1.0f, 20.0f, 2.0f, 0u },
  { "ratio3", "Band 3 Ratio", "", 1.0f, 20.0f, 2.0f, 0u },
  { "ratio4", "Band 4 Ratio", "", 1.0f, 20.0f, 2.0f, 0u },
  { "ratio5", "Band 5 Ratio", "", 1.0f, 20.0f, 2.0f, 0u },
  { "attackMs1", "Band 1 Attack", "ms", 0.1f, 200.0f, 10.0f, 0u },
  { "attackMs2", "Band 2 Attack", "ms", 0.1f, 200.0f, 10.0f, 0u },
  { "attackMs3", "Band 3 Attack", "ms", 0.1f, 200.0f, 10.0f, 0u },
  { "attackMs4", "Band 4 Attack", "ms", 0.1f, 200.0f, 10.0f, 0u },
  { "attackMs5", "Band 5 Attack", "ms", 0.1f, 200.0f, 10.0f, 0u },
  { "releaseMs1", "Band 1 Release", "ms", 10.0f, 2000.0f, 150.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "releaseMs2", "Band 2 Release", "ms", 10.0f, 2000.0f, 150.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "releaseMs3", "Band 3 Release", "ms", 10.0f, 2000.0f, 150.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "releaseMs4", "Band 4 Release", "ms", 10.0f, 2000.0f, 150.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "releaseMs5", "Band 5 Release", "ms", 10.0f, 2000.0f, 150.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "makeupDb1", "Band 1 Makeup", "dB", 0.0f, 24.0f, 0.0f, 0u },
  { "makeupDb2", "Band 2 Makeup", "dB", 0.0f, 24.0f, 0.0f, 0u },
  { "makeupDb3", "Band 3 Makeup", "dB", 0.0f, 24.0f, 0.0f, 0u },
  { "makeupDb4", "Band 4 Makeup", "dB", 0.0f, 24.0f, 0.0f, 0u },
  { "makeupDb5", "Band 5 Makeup", "dB", 0.0f, 24.0f, 0.0f, 0u },
  { "solo1", "Band 1 Solo", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "solo2", "Band 2 Solo", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "solo3", "Band 3 Solo", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "solo4", "Band 4 Solo", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "solo5", "Band 5 Solo", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "bandBypass1", "Band 1 Bypass", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "bandBypass2", "Band 2 Bypass", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "bandBypass3", "Band 3 Bypass", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "bandBypass4", "Band 4 Bypass", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "bandBypass5", "Band 5 Bypass", "", 0.0f, 1.0f, 0.0f, OMX_PLUGIN_PARAM_TOGGLE },
  { "mixPct", "Mix", "%", 0.0f, 100.0f, 100.0f, OMX_PLUGIN_PARAM_INTEGER },
  { "outputDb", "Output", "dB", -24.0f, 24.0f, 0.0f, 0u },
};

/* Each parameter's CLAP module (clap_param_info.module): its descriptor control's group as a module path, "" for a
 * plugin with no descriptor. */
static const char *const OMX_MBCOMP_PARAM_MODULES[OMX_MBCOMP_PARAM_COUNT] = {
  "Split",
  "Split",
  "Split",
  "Split",
  "Split",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Band 1",
  "Band 2",
  "Band 3",
  "Band 4",
  "Band 5",
  "Output",
  "Output",
};

/* One macro per declared bound: what a C face reads where a constant is needed. */
#define OMX_MBCOMP_PARAM_BANDS_MIN 2.0f
#define OMX_MBCOMP_PARAM_BANDS_MAX 5.0f
#define OMX_MBCOMP_PARAM_BANDS_DEFAULT 3.0f
#define OMX_MBCOMP_PARAM_XOVER_HZ1_MIN 40.0f
#define OMX_MBCOMP_PARAM_XOVER_HZ1_MAX 16000.0f
#define OMX_MBCOMP_PARAM_XOVER_HZ1_DEFAULT 200.0f
#define OMX_MBCOMP_PARAM_XOVER_HZ2_MIN 40.0f
#define OMX_MBCOMP_PARAM_XOVER_HZ2_MAX 16000.0f
#define OMX_MBCOMP_PARAM_XOVER_HZ2_DEFAULT 2000.0f
#define OMX_MBCOMP_PARAM_XOVER_HZ3_MIN 40.0f
#define OMX_MBCOMP_PARAM_XOVER_HZ3_MAX 16000.0f
#define OMX_MBCOMP_PARAM_XOVER_HZ3_DEFAULT 5000.0f
#define OMX_MBCOMP_PARAM_XOVER_HZ4_MIN 40.0f
#define OMX_MBCOMP_PARAM_XOVER_HZ4_MAX 16000.0f
#define OMX_MBCOMP_PARAM_XOVER_HZ4_DEFAULT 6400.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB1_MIN -60.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB1_MAX 0.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB1_DEFAULT -20.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB2_MIN -60.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB2_MAX 0.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB2_DEFAULT -20.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB3_MIN -60.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB3_MAX 0.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB3_DEFAULT -20.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB4_MIN -60.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB4_MAX 0.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB4_DEFAULT -20.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB5_MIN -60.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB5_MAX 0.0f
#define OMX_MBCOMP_PARAM_THRESHOLD_DB5_DEFAULT -20.0f
#define OMX_MBCOMP_PARAM_RATIO1_MIN 1.0f
#define OMX_MBCOMP_PARAM_RATIO1_MAX 20.0f
#define OMX_MBCOMP_PARAM_RATIO1_DEFAULT 2.0f
#define OMX_MBCOMP_PARAM_RATIO2_MIN 1.0f
#define OMX_MBCOMP_PARAM_RATIO2_MAX 20.0f
#define OMX_MBCOMP_PARAM_RATIO2_DEFAULT 2.0f
#define OMX_MBCOMP_PARAM_RATIO3_MIN 1.0f
#define OMX_MBCOMP_PARAM_RATIO3_MAX 20.0f
#define OMX_MBCOMP_PARAM_RATIO3_DEFAULT 2.0f
#define OMX_MBCOMP_PARAM_RATIO4_MIN 1.0f
#define OMX_MBCOMP_PARAM_RATIO4_MAX 20.0f
#define OMX_MBCOMP_PARAM_RATIO4_DEFAULT 2.0f
#define OMX_MBCOMP_PARAM_RATIO5_MIN 1.0f
#define OMX_MBCOMP_PARAM_RATIO5_MAX 20.0f
#define OMX_MBCOMP_PARAM_RATIO5_DEFAULT 2.0f
#define OMX_MBCOMP_PARAM_ATTACK_MS1_MIN 0.1f
#define OMX_MBCOMP_PARAM_ATTACK_MS1_MAX 200.0f
#define OMX_MBCOMP_PARAM_ATTACK_MS1_DEFAULT 10.0f
#define OMX_MBCOMP_PARAM_ATTACK_MS2_MIN 0.1f
#define OMX_MBCOMP_PARAM_ATTACK_MS2_MAX 200.0f
#define OMX_MBCOMP_PARAM_ATTACK_MS2_DEFAULT 10.0f
#define OMX_MBCOMP_PARAM_ATTACK_MS3_MIN 0.1f
#define OMX_MBCOMP_PARAM_ATTACK_MS3_MAX 200.0f
#define OMX_MBCOMP_PARAM_ATTACK_MS3_DEFAULT 10.0f
#define OMX_MBCOMP_PARAM_ATTACK_MS4_MIN 0.1f
#define OMX_MBCOMP_PARAM_ATTACK_MS4_MAX 200.0f
#define OMX_MBCOMP_PARAM_ATTACK_MS4_DEFAULT 10.0f
#define OMX_MBCOMP_PARAM_ATTACK_MS5_MIN 0.1f
#define OMX_MBCOMP_PARAM_ATTACK_MS5_MAX 200.0f
#define OMX_MBCOMP_PARAM_ATTACK_MS5_DEFAULT 10.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS1_MIN 10.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS1_MAX 2000.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS1_DEFAULT 150.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS2_MIN 10.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS2_MAX 2000.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS2_DEFAULT 150.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS3_MIN 10.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS3_MAX 2000.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS3_DEFAULT 150.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS4_MIN 10.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS4_MAX 2000.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS4_DEFAULT 150.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS5_MIN 10.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS5_MAX 2000.0f
#define OMX_MBCOMP_PARAM_RELEASE_MS5_DEFAULT 150.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB1_MIN 0.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB1_MAX 24.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB1_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB2_MIN 0.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB2_MAX 24.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB2_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB3_MIN 0.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB3_MAX 24.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB3_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB4_MIN 0.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB4_MAX 24.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB4_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB5_MIN 0.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB5_MAX 24.0f
#define OMX_MBCOMP_PARAM_MAKEUP_DB5_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_SOLO1_MIN 0.0f
#define OMX_MBCOMP_PARAM_SOLO1_MAX 1.0f
#define OMX_MBCOMP_PARAM_SOLO1_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_SOLO2_MIN 0.0f
#define OMX_MBCOMP_PARAM_SOLO2_MAX 1.0f
#define OMX_MBCOMP_PARAM_SOLO2_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_SOLO3_MIN 0.0f
#define OMX_MBCOMP_PARAM_SOLO3_MAX 1.0f
#define OMX_MBCOMP_PARAM_SOLO3_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_SOLO4_MIN 0.0f
#define OMX_MBCOMP_PARAM_SOLO4_MAX 1.0f
#define OMX_MBCOMP_PARAM_SOLO4_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_SOLO5_MIN 0.0f
#define OMX_MBCOMP_PARAM_SOLO5_MAX 1.0f
#define OMX_MBCOMP_PARAM_SOLO5_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS1_MIN 0.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS1_MAX 1.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS1_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS2_MIN 0.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS2_MAX 1.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS2_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS3_MIN 0.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS3_MAX 1.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS3_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS4_MIN 0.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS4_MAX 1.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS4_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS5_MIN 0.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS5_MAX 1.0f
#define OMX_MBCOMP_PARAM_BAND_BYPASS5_DEFAULT 0.0f
#define OMX_MBCOMP_PARAM_MIX_PCT_MIN 0.0f
#define OMX_MBCOMP_PARAM_MIX_PCT_MAX 100.0f
#define OMX_MBCOMP_PARAM_MIX_PCT_DEFAULT 100.0f
#define OMX_MBCOMP_PARAM_OUTPUT_DB_MIN -24.0f
#define OMX_MBCOMP_PARAM_OUTPUT_DB_MAX 24.0f
#define OMX_MBCOMP_PARAM_OUTPUT_DB_DEFAULT 0.0f

/* The identity every face publishes. */
#define OMX_MBCOMP_NAME "omx multiband"
#define OMX_MBCOMP_VENDOR "openmixer"
#define OMX_MBCOMP_URL "https://github.com/FreeMixer/omx-plugins"
#define OMX_MBCOMP_VERSION "0.19.2"
#define OMX_MBCOMP_DESCRIPTION "A multiband compressor: two to five bands split by Linkwitz-Riley crossovers, each band its own compressor with threshold, ratio, attack, release, make-up, solo and bypass, then a mix and an output gain, for in-ear mixes, vocals and playback buses."
#define OMX_MBCOMP_CLAP_ID "org.openmixer.multiband"
#define OMX_MBCOMP_CLAP_FEATURES "audio-effect", "compressor", "stereo"
#define OMX_MBCOMP_LV2_URI "urn:openmixer:multiband"
/* org.openmixer.declaration/1: the declaration's plain description and the digest of its parameters. */
#define OMX_MBCOMP_DECL_SOURCE "omx multiband: Bands 2 to 5, Crossover 1 40 to 16000 Hz, Crossover 2 40 to 16000 Hz, Crossover 3 40 to 16000 Hz, Crossover 4 40 to 16000 Hz, Band 1 Threshold -60 to 0 dB, Band 2 Threshold -60 to 0 dB, Band 3 Threshold -60 to 0 dB, Band 4 Threshold -60 to 0 dB, Band 5 Threshold -60 to 0 dB, Band 1 Ratio 1 to 20, Band 2 Ratio 1 to 20, Band 3 Ratio 1 to 20, Band 4 Ratio 1 to 20, Band 5 Ratio 1 to 20, Band 1 Attack 0.1 to 200 ms, Band 2 Attack 0.1 to 200 ms, Band 3 Attack 0.1 to 200 ms, Band 4 Attack 0.1 to 200 ms, Band 5 Attack 0.1 to 200 ms, Band 1 Release 10 to 2000 ms, Band 2 Release 10 to 2000 ms, Band 3 Release 10 to 2000 ms, Band 4 Release 10 to 2000 ms, Band 5 Release 10 to 2000 ms, Band 1 Makeup 0 to 24 dB, Band 2 Makeup 0 to 24 dB, Band 3 Makeup 0 to 24 dB, Band 4 Makeup 0 to 24 dB, Band 5 Makeup 0 to 24 dB, Band 1 Solo 0 to 1, Band 2 Solo 0 to 1, Band 3 Solo 0 to 1, Band 4 Solo 0 to 1, Band 5 Solo 0 to 1, Band 1 Bypass 0 to 1, Band 2 Bypass 0 to 1, Band 3 Bypass 0 to 1, Band 4 Bypass 0 to 1, Band 5 Bypass 0 to 1, Mix 0 to 100 %, Output -24 to 24 dB"
#define OMX_MBCOMP_DECL_DIGEST "e94249a95a2284f21f484388a13d5ab5e249fd138fc123a063260c19573edd10"

/* org.openmixer.param-contract/1 (tools/param-contract.mjs): each parameter's unit, scale and step, in
 * id order; the host's bypass, CLAP id OMX_PLUGIN_FACE_BYPASS_CLAP_ID (contract/plugin-face.json), is the face's. */
#define OMX_MBCOMP_PARAM_CONTRACT_VERSION 2u
#define OMX_MBCOMP_PARAM_CONTRACT_FINGERPRINT "2093e6e94b941f469715ca034b123be763ab9b2e3f1e156d69307b0297e000cd"
static const struct omx_param_contract_row OMX_MBCOMP_PARAM_CONTRACT[OMX_MBCOMP_PARAM_COUNT] = {
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bands none */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* xoverHz1 hz */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* xoverHz2 hz */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* xoverHz3 hz */
  { 16u, OMX_CLAP_SCALE_LOG, 1.0 }, /* xoverHz4 hz */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* thresholdDb1 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* thresholdDb2 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* thresholdDb3 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* thresholdDb4 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* thresholdDb5 db */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio1 ratio */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio2 ratio */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio3 ratio */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio4 ratio */
  { 258u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* ratio5 ratio */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attackMs1 ms */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attackMs2 ms */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attackMs3 ms */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attackMs4 ms */
  { 2u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* attackMs5 ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* releaseMs1 ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* releaseMs2 ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* releaseMs3 ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* releaseMs4 ms */
  { 2u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* releaseMs5 ms */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* makeupDb1 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* makeupDb2 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* makeupDb3 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* makeupDb4 db */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* makeupDb5 db */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* solo1 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* solo2 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* solo3 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* solo4 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* solo5 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandBypass1 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandBypass2 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandBypass3 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandBypass4 none */
  { 0u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* bandBypass5 none */
  { 14u, OMX_CLAP_SCALE_STEPPED, 1.0 }, /* mixPct pc */
  { 13u, OMX_CLAP_SCALE_LINEAR, 0.0 }, /* outputDb db */
};

/* org.openmixer.resource-hints.draft/0 (omx-contract RESOURCE_HINTS, tools/resource-hints.mjs): the hints the
 * declaration's "resourceHints" states. A hint it leaves out has its bit of `declared` clear: unknown, never zero. */
static const omx_resource_hints_t OMX_MBCOMP_RESOURCE_HINTS = {
  .declared = OMX_RESOURCE_HINT_THREADS_SPAWNED | OMX_RESOURCE_HINT_STREAMS_FROM_DISK | OMX_RESOURCE_HINT_OPENS_SOCKETS,
  .threads_spawned = 0u,
  .streams_from_disk = false,
  .opens_sockets = false,
};

/* org.openmixer.panel-description/1 (tools/panel-description.mjs): the declaration's panel as the rows
 * include/omx_panel_description.h serves, parameter ids the declaration's. Family dynamics. */
#define OMX_MBCOMP_PANEL_FINGERPRINT "f23f047e9bfe7c8130f4b03d0dea116732d23bdde9f706af22882d2229d070fe"
static const struct omx_panel_section_row OMX_MBCOMP_PANEL_SECTIONS[] = {
  { "split", "fx.mbcomp.section.split", "Split" },
  { "band1", "fx.mbcomp.section.band1", "Band 1" },
  { "band2", "fx.mbcomp.section.band2", "Band 2" },
  { "band3", "fx.mbcomp.section.band3", "Band 3" },
  { "band4", "fx.mbcomp.section.band4", "Band 4" },
  { "band5", "fx.mbcomp.section.band5", "Band 5" },
  { "output", "fx.mbcomp.section.output", "Output" },
};
static const struct omx_panel_control_row OMX_MBCOMP_PANEL_CONTROLS[] = {
  { 0u, 0u, OMX_CLAP_WIDGET_AUTO, "fx.mbcomp.bands", "Bands", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 1u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.xover_hz1", "Crossover 1", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 2u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.xover_hz2", "Crossover 2", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 3u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.xover_hz3", "Crossover 3", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 4u, 0u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.xover_hz4", "Crossover 4", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 5u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.threshold_db1", "Band 1 Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 10u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.ratio1", "Band 1 Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 15u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.attack_ms1", "Band 1 Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 20u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.release_ms1", "Band 1 Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 25u, 1u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.makeup_db1", "Band 1 Makeup", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 30u, 1u, OMX_CLAP_WIDGET_SWITCH, "fx.mbcomp.solo1", "Band 1 Solo", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 35u, 1u, OMX_CLAP_WIDGET_SWITCH, "fx.mbcomp.band_bypass1", "Band 1 Bypass", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 6u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.threshold_db2", "Band 2 Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 11u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.ratio2", "Band 2 Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 16u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.attack_ms2", "Band 2 Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 21u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.release_ms2", "Band 2 Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 26u, 2u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.makeup_db2", "Band 2 Makeup", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 31u, 2u, OMX_CLAP_WIDGET_SWITCH, "fx.mbcomp.solo2", "Band 2 Solo", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 36u, 2u, OMX_CLAP_WIDGET_SWITCH, "fx.mbcomp.band_bypass2", "Band 2 Bypass", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 7u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.threshold_db3", "Band 3 Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 12u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.ratio3", "Band 3 Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 17u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.attack_ms3", "Band 3 Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 22u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.release_ms3", "Band 3 Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 27u, 3u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.makeup_db3", "Band 3 Makeup", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 32u, 3u, OMX_CLAP_WIDGET_SWITCH, "fx.mbcomp.solo3", "Band 3 Solo", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 37u, 3u, OMX_CLAP_WIDGET_SWITCH, "fx.mbcomp.band_bypass3", "Band 3 Bypass", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 8u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.threshold_db4", "Band 4 Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 13u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.ratio4", "Band 4 Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 18u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.attack_ms4", "Band 4 Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 23u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.release_ms4", "Band 4 Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 28u, 4u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.makeup_db4", "Band 4 Makeup", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 33u, 4u, OMX_CLAP_WIDGET_SWITCH, "fx.mbcomp.solo4", "Band 4 Solo", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 38u, 4u, OMX_CLAP_WIDGET_SWITCH, "fx.mbcomp.band_bypass4", "Band 4 Bypass", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 9u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.threshold_db5", "Band 5 Threshold", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 14u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.ratio5", "Band 5 Ratio", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 19u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.attack_ms5", "Band 5 Attack", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 24u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.release_ms5", "Band 5 Release", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 29u, 5u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.makeup_db5", "Band 5 Makeup", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 34u, 5u, OMX_CLAP_WIDGET_SWITCH, "fx.mbcomp.solo5", "Band 5 Solo", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 39u, 5u, OMX_CLAP_WIDGET_SWITCH, "fx.mbcomp.band_bypass5", "Band 5 Bypass", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 40u, 6u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.mix_pct", "Mix", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
  { 41u, 6u, OMX_CLAP_WIDGET_KNOB, "fx.mbcomp.output_db", "Output", OMX_CLAP_PANEL_INVALID_ID, 0x0u },
};
static const struct omx_panel_role_row OMX_MBCOMP_PANEL_ROLES[] = {
  { "threshold", 0u, 5u },
  { "ratio", 0u, 10u },
  { "attack", 0u, 15u },
  { "release", 0u, 20u },
  { "makeup", 0u, 25u },
  { "mix", 0u, 40u },
  { "output", 0u, 41u },
};
static const struct omx_panel_display_row OMX_MBCOMP_PANEL_DISPLAYS[] = {
  { OMX_CLAP_DISPLAY_GAIN_REDUCTION_BAR, 1u, OMX_CLAP_DISPLAY_FROM_METERS, { 0u, OMX_CLAP_PANEL_INVALID_ID }, 0u, OMX_CLAP_PANEL_INVALID_ID, 0.0, "fx.mbcomp.display.gain_reduction_bar", "Gain reduction" },
};
/* clap.remote-controls/2: the pages of the page rule over the sections, every control's id in its slot; a face blanks the
 * slot of a control its governing values hide (lib/omx_clap_face.c). */
static const struct omx_panel_page_row OMX_MBCOMP_PANEL_PAGES[] = {
  { "Easy", "Easy", { 5u, 10u, 25u, 40u, 15u, 20u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Split", "Split", { 0u, 1u, 2u, 3u, 4u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Band 1", "Band 1", { 30u, 35u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
  { "Band 2", "Band 2", { 6u, 11u, 16u, 21u, 26u, 31u, 36u, OMX_CLAP_PANEL_INVALID_ID } },
  { "Band 3", "Band 3", { 7u, 12u, 17u, 22u, 27u, 32u, 37u, OMX_CLAP_PANEL_INVALID_ID } },
  { "Band 4", "Band 4", { 8u, 13u, 18u, 23u, 28u, 33u, 38u, OMX_CLAP_PANEL_INVALID_ID } },
  { "Band 5", "Band 5", { 9u, 14u, 19u, 24u, 29u, 34u, 39u, OMX_CLAP_PANEL_INVALID_ID } },
  { "Output", "Output", { 41u, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID, OMX_CLAP_PANEL_INVALID_ID } },
};
#define OMX_MBCOMP_PANEL_PAGE_COUNT 8u
static const struct omx_panel_rows OMX_MBCOMP_PANEL = {
  OMX_MBCOMP_PANEL_FINGERPRINT, OMX_CLAP_FAMILY_DYNAMICS,
  OMX_MBCOMP_PANEL_SECTIONS, 7u,
  OMX_MBCOMP_PANEL_CONTROLS, 42u,
  OMX_MBCOMP_PANEL_ROLES, 7u,
  OMX_MBCOMP_PANEL_DISPLAYS, 1u,
  NULL, 0u,
};

/* LV2 port indices (tools/gen.mjs lv2Ports): enabled and latency at omx-contract's permanent indices, the tempo's control,
 * the meters' outputs, the audio ports, then parameter i at FIRST_PARAM + i. */
#define OMX_MBCOMP_LV2_PORT_ENABLED 0u
#define OMX_MBCOMP_LV2_PORT_LATENCY 1u
#define OMX_MBCOMP_LV2_PORT_IN_L 7u
#define OMX_MBCOMP_LV2_PORT_IN_R 8u
#define OMX_MBCOMP_LV2_PORT_OUT_L 9u
#define OMX_MBCOMP_LV2_PORT_OUT_R 10u
#define OMX_MBCOMP_LV2_PORT_FIRST_PARAM 11u
/* The meters' output control ports, one per channel in the order their values are published, from here on. */
#define OMX_MBCOMP_LV2_PORT_FIRST_METER 2u
#define OMX_MBCOMP_LV2_PORT_COUNT 53u

#endif /* OMX_MBCOMP_PARAMS_H */
