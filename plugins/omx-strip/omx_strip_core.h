// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * omx_strip_core.h — omx-strip's binding to its chain of omx-dsp instance faces, Input > Filters > Gate > EQ > Comp: the ONE
 * place both faces (omx_strip_clap.c, omx_strip_lv2.c) reach the DSP. It holds no DSP of its
 * own: it copies in to out once, then runs each element's face in place on out, in the chosen order.
 * Each element's parameters go, by name, to its own face's resolve(); its switch off, or the host's
 * bypass, is that face's bypass. The latency is the sum of the elements'. Plain inline state: nothing
 * is allocated, locked or called into the system, in process or at activate.
 *
 * GENERATED — DO NOT EDIT BY HAND. Produced by tools/gen.mjs from plugins/omx-strip/omx-strip.decl.json and
 * each face's resolve() prototype.
 * filter: `bandType` is not a parameter; it is passed at "default".
 * filter: `freq` is not a parameter; it is passed at "default".
 * filter: `gain` is not a parameter; it is passed at "default".
 * filter: `q` is not a parameter; it is passed at "default".
 * filter: `bandOn` is not a parameter; it is passed at "off".
 * gate: `keySource` is not a parameter; it is passed at "self".
 * gate: `hpHz` is not a parameter; it is passed at 0.
 * gate: `lpHz` is not a parameter; it is passed at 0.
 * gate: `key_listen` is no parameter: the declaration holds it at its kernel's declared default, OMX_GATE_KEY_LISTENS_DEFAULT.
 * gate: `key_tilt_db` is no parameter: the declaration holds it at its kernel's declared default, OMX_GATE_KEY_TILT_DB_RANGE_DEFAULT.
 * eq: `hpfOn` is not a parameter; it is passed at "off".
 * eq: `hpfFreq` is not a parameter; it is passed at {"defaultRef":"EQ_PASS_FILTER_DEFAULTS.hpfFreqHz"}.
 * eq: `hpfSlope` is not a parameter; it is passed at "default".
 * eq: `lpfOn` is not a parameter; it is passed at "off".
 * eq: `lpfFreq` is not a parameter; it is passed at {"defaultRef":"EQ_PASS_FILTER_DEFAULTS.lpfFreqHz"}.
 * eq: `lpfSlope` is not a parameter; it is passed at "default".
 * eq: `bandType` is not a parameter; it is passed at "default".
 * eq: `freq` is not a parameter; it is passed at "default".
 * eq: `gain` is not a parameter; it is passed at "default".
 * eq: `q` is not a parameter; it is passed at "default".
 * eq: `bandOn` is not a parameter; it is passed at "off".
 * comp: `detectorTopology` is not a parameter; it is passed at "feedforward".
 * comp: `key_source` is no parameter: the declaration holds it at its kernel's declared default, OMX_COMP_KEY_SOURCES_DEFAULT.
 * comp: `key_hp_hz` is no parameter: the declaration holds it at its kernel's declared default, OMX_COMP_KEY_HP_HZ_RANGE_DEFAULT.
 * comp: `key_lp_hz` is no parameter: the declaration holds it at its kernel's declared default, OMX_COMP_KEY_LP_HZ_RANGE_DEFAULT.
 * comp: `key_tilt_db` is no parameter: the declaration holds it at its kernel's declared default, OMX_COMP_KEY_TILT_DB_RANGE_DEFAULT.
 * comp: `key_listen` is no parameter: the declaration holds it at its kernel's declared default, OMX_COMP_KEY_LISTENS_DEFAULT.
 * comp: `hold_ms` is no parameter: the declaration holds it at its kernel's declared default, OMX_COMP_HOLD_MS_RANGE_DEFAULT.
 * comp: `detect_link` is no parameter: the declaration holds it at its kernel's declared default, OMX_COMP_DETECT_LINKS_DEFAULT.
 * comp: `duck` is no parameter: the declaration holds it at its kernel's declared default, OMX_COMP_DUCKS_DEFAULT.
 * comp: `range_db` is no parameter: the declaration holds it at its kernel's declared default, OMX_COMP_RANGE_DB_RANGE_DEFAULT.
 * comp: `release_mode` is no parameter: the declaration holds it at its kernel's declared default, OMX_COMP_RELEASE_MODES_DEFAULT.
 */
#ifndef OMX_STRIP_CORE_H
#define OMX_STRIP_CORE_H

#include <math.h>
#include <stdint.h>
#include <string.h>

/* The generated table FIRST, then the faces. */
#include "omx_strip_params.h"
#include "omx_plugin_face.h"

#include <omxdsp/fx/omx_trim_instance.h>
#include <omxdsp/fx/omx_eq_instance.h>
#include <omxdsp/fx/omx_gate_instance.h>
#include <omxdsp/fx/omx_comp_instance.h>
_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one filter band_type parameter per band of the face");
_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one filter freq parameter per band of the face");
_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one filter gain parameter per band of the face");
_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one filter q parameter per band of the face");
_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one filter band_on parameter per band of the face");
_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one eq band_type parameter per band of the face");
_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one eq freq parameter per band of the face");
_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one eq gain parameter per band of the face");
_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one eq q parameter per band of the face");
_Static_assert(32 == OMX_EQ_INSTANCE_BANDS, "the declaration names one eq band_on parameter per band of the face");

/* The elements, in declared order, and every order they may run in (the `order` parameter's value is
 * the row: lexicographic, 0 the declared order). */
enum { OMX_STRIP_CHAIN_0_trim = 0, OMX_STRIP_CHAIN_1_filter = 1, OMX_STRIP_CHAIN_2_gate = 2, OMX_STRIP_CHAIN_3_eq = 3, OMX_STRIP_CHAIN_4_comp = 4, OMX_STRIP_CHAIN_LENGTH = 5 };
#define OMX_STRIP_CHAIN_ORDERS 120u
static const uint8_t OMX_STRIP_CHAIN_ORDER[OMX_STRIP_CHAIN_ORDERS][OMX_STRIP_CHAIN_LENGTH] = {
    { 0, 1, 2, 3, 4 },
    { 0, 1, 2, 4, 3 },
    { 0, 1, 3, 2, 4 },
    { 0, 1, 3, 4, 2 },
    { 0, 1, 4, 2, 3 },
    { 0, 1, 4, 3, 2 },
    { 0, 2, 1, 3, 4 },
    { 0, 2, 1, 4, 3 },
    { 0, 2, 3, 1, 4 },
    { 0, 2, 3, 4, 1 },
    { 0, 2, 4, 1, 3 },
    { 0, 2, 4, 3, 1 },
    { 0, 3, 1, 2, 4 },
    { 0, 3, 1, 4, 2 },
    { 0, 3, 2, 1, 4 },
    { 0, 3, 2, 4, 1 },
    { 0, 3, 4, 1, 2 },
    { 0, 3, 4, 2, 1 },
    { 0, 4, 1, 2, 3 },
    { 0, 4, 1, 3, 2 },
    { 0, 4, 2, 1, 3 },
    { 0, 4, 2, 3, 1 },
    { 0, 4, 3, 1, 2 },
    { 0, 4, 3, 2, 1 },
    { 1, 0, 2, 3, 4 },
    { 1, 0, 2, 4, 3 },
    { 1, 0, 3, 2, 4 },
    { 1, 0, 3, 4, 2 },
    { 1, 0, 4, 2, 3 },
    { 1, 0, 4, 3, 2 },
    { 1, 2, 0, 3, 4 },
    { 1, 2, 0, 4, 3 },
    { 1, 2, 3, 0, 4 },
    { 1, 2, 3, 4, 0 },
    { 1, 2, 4, 0, 3 },
    { 1, 2, 4, 3, 0 },
    { 1, 3, 0, 2, 4 },
    { 1, 3, 0, 4, 2 },
    { 1, 3, 2, 0, 4 },
    { 1, 3, 2, 4, 0 },
    { 1, 3, 4, 0, 2 },
    { 1, 3, 4, 2, 0 },
    { 1, 4, 0, 2, 3 },
    { 1, 4, 0, 3, 2 },
    { 1, 4, 2, 0, 3 },
    { 1, 4, 2, 3, 0 },
    { 1, 4, 3, 0, 2 },
    { 1, 4, 3, 2, 0 },
    { 2, 0, 1, 3, 4 },
    { 2, 0, 1, 4, 3 },
    { 2, 0, 3, 1, 4 },
    { 2, 0, 3, 4, 1 },
    { 2, 0, 4, 1, 3 },
    { 2, 0, 4, 3, 1 },
    { 2, 1, 0, 3, 4 },
    { 2, 1, 0, 4, 3 },
    { 2, 1, 3, 0, 4 },
    { 2, 1, 3, 4, 0 },
    { 2, 1, 4, 0, 3 },
    { 2, 1, 4, 3, 0 },
    { 2, 3, 0, 1, 4 },
    { 2, 3, 0, 4, 1 },
    { 2, 3, 1, 0, 4 },
    { 2, 3, 1, 4, 0 },
    { 2, 3, 4, 0, 1 },
    { 2, 3, 4, 1, 0 },
    { 2, 4, 0, 1, 3 },
    { 2, 4, 0, 3, 1 },
    { 2, 4, 1, 0, 3 },
    { 2, 4, 1, 3, 0 },
    { 2, 4, 3, 0, 1 },
    { 2, 4, 3, 1, 0 },
    { 3, 0, 1, 2, 4 },
    { 3, 0, 1, 4, 2 },
    { 3, 0, 2, 1, 4 },
    { 3, 0, 2, 4, 1 },
    { 3, 0, 4, 1, 2 },
    { 3, 0, 4, 2, 1 },
    { 3, 1, 0, 2, 4 },
    { 3, 1, 0, 4, 2 },
    { 3, 1, 2, 0, 4 },
    { 3, 1, 2, 4, 0 },
    { 3, 1, 4, 0, 2 },
    { 3, 1, 4, 2, 0 },
    { 3, 2, 0, 1, 4 },
    { 3, 2, 0, 4, 1 },
    { 3, 2, 1, 0, 4 },
    { 3, 2, 1, 4, 0 },
    { 3, 2, 4, 0, 1 },
    { 3, 2, 4, 1, 0 },
    { 3, 4, 0, 1, 2 },
    { 3, 4, 0, 2, 1 },
    { 3, 4, 1, 0, 2 },
    { 3, 4, 1, 2, 0 },
    { 3, 4, 2, 0, 1 },
    { 3, 4, 2, 1, 0 },
    { 4, 0, 1, 2, 3 },
    { 4, 0, 1, 3, 2 },
    { 4, 0, 2, 1, 3 },
    { 4, 0, 2, 3, 1 },
    { 4, 0, 3, 1, 2 },
    { 4, 0, 3, 2, 1 },
    { 4, 1, 0, 2, 3 },
    { 4, 1, 0, 3, 2 },
    { 4, 1, 2, 0, 3 },
    { 4, 1, 2, 3, 0 },
    { 4, 1, 3, 0, 2 },
    { 4, 1, 3, 2, 0 },
    { 4, 2, 0, 1, 3 },
    { 4, 2, 0, 3, 1 },
    { 4, 2, 1, 0, 3 },
    { 4, 2, 1, 3, 0 },
    { 4, 2, 3, 0, 1 },
    { 4, 2, 3, 1, 0 },
    { 4, 3, 0, 1, 2 },
    { 4, 3, 0, 2, 1 },
    { 4, 3, 1, 0, 2 },
    { 4, 3, 1, 2, 0 },
    { 4, 3, 2, 0, 1 },
    { 4, 3, 2, 1, 0 },
};

/** Each element's state, and the order a block runs in. */
typedef struct {
  uint32_t rate;
  uint32_t order;
  OmxTrimInstance trim;
  OmxEqInstance filter;
  OmxGateInstance gate;
  OmxEqInstance eq;
  OmxCompInstance comp;
} OmxStripCore;

/** A fresh chain at `rate`: every state word zero, each face bound to the rate. A rate a face does not
 * declare leaves that element a wire. */
static inline void omx_strip_core_init(OmxStripCore *c, uint32_t rate) {
  memset(c, 0, sizeof *c);
  c->rate = rate;
  (void)omx_trim_instance_init(&c->trim, (float)rate);
  (void)omx_eq_instance_init(&c->filter, (float)rate);
  (void)omx_gate_instance_init(&c->gate, (float)rate);
  (void)omx_eq_instance_init(&c->eq, (float)rate);
  (void)omx_comp_instance_init(&c->comp, (float)rate);
}

/** The block's parameters (declaration order) and the host's bypass, each element's to its own face. */
static inline void omx_strip_core_resolve(OmxStripCore *c, const float *values, int bypass) {
  omx_trim_instance_resolve(&c->trim, bypass || values[OMX_STRIP_PARAM_TRIM_ON] < 0.5f,
                                  /* trim_db */ values[OMX_STRIP_PARAM_TRIM_DB]);
  omx_eq_instance_resolve(&c->filter, bypass || values[OMX_STRIP_PARAM_FILTER_ON] < 0.5f,
                                  /* hpf_on */ (int)lrintf(values[OMX_STRIP_PARAM_HPF_ON]),
                                  /* hpf_freq */ values[OMX_STRIP_PARAM_HPF_FREQ],
                                  /* hpf_slope */ (int)lrintf(values[OMX_STRIP_PARAM_HPF_SLOPE]),
                                  /* lpf_on */ (int)lrintf(values[OMX_STRIP_PARAM_LPF_ON]),
                                  /* lpf_freq */ values[OMX_STRIP_PARAM_LPF_FREQ],
                                  /* lpf_slope */ (int)lrintf(values[OMX_STRIP_PARAM_LPF_SLOPE]),
                                  /* band_type */ (const int[OMX_EQ_INSTANCE_BANDS]){1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
                                  /* freq */ (const float[OMX_EQ_INSTANCE_BANDS]){22.4f, 28.0f, 35.5f, 45.0f, 50.0f, 63.0f, 80.0f, 100.0f, 125.0f, 160.0f, 200.0f, 250.0f, 315.0f, 355.0f, 450.0f, 560.0f, 710.0f, 900.0f, 1120.0f, 1400.0f, 1600.0f, 2000.0f, 2500.0f, 3150.0f, 4000.0f, 5000.0f, 6300.0f, 8000.0f, 9000.0f, 11200.0f, 14000.0f, 18000.0f},
                                  /* gain */ (const float[OMX_EQ_INSTANCE_BANDS]){0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                                  /* q */ (const float[OMX_EQ_INSTANCE_BANDS]){1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
                                  /* band_on */ (const int[OMX_EQ_INSTANCE_BANDS]){0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
  omx_gate_instance_resolve(&c->gate, bypass || values[OMX_STRIP_PARAM_GATE_ON] < 0.5f,
                                  /* key_source */ 0,
                                  /* threshold_db */ values[OMX_STRIP_PARAM_GATE_THRESHOLD],
                                  /* range_db */ values[OMX_STRIP_PARAM_GATE_RANGE],
                                  /* knee_start_db */ values[OMX_STRIP_PARAM_GATE_KNEE_START],
                                  /* knee_end_db */ values[OMX_STRIP_PARAM_GATE_KNEE_END],
                                  /* attack_ms */ values[OMX_STRIP_PARAM_GATE_ATTACK],
                                  /* hold_ms */ values[OMX_STRIP_PARAM_GATE_HOLD],
                                  /* release_ms */ values[OMX_STRIP_PARAM_GATE_RELEASE],
                                  /* hysteresis_db */ values[OMX_STRIP_PARAM_GATE_HYSTERESIS],
                                  /* ratio */ values[OMX_STRIP_PARAM_GATE_RATIO],
                                  /* hp_hz */ 0.0f,
                                  /* lp_hz */ 0.0f,
                                  /* key_listen */ (int)OMX_GATE_KEY_LISTENS_DEFAULT,
                                  /* key_tilt_db */ (float)OMX_GATE_KEY_TILT_DB_RANGE_DEFAULT);
  omx_eq_instance_resolve(&c->eq, bypass || values[OMX_STRIP_PARAM_EQ_ON] < 0.5f,
                                  /* hpf_on */ 0,
                                  /* hpf_freq */ 80.0f,
                                  /* hpf_slope */ 12,
                                  /* lpf_on */ 0,
                                  /* lpf_freq */ 18000.0f,
                                  /* lpf_slope */ 12,
                                  /* band_type */ (const int[OMX_EQ_INSTANCE_BANDS]){(int)lrintf(values[OMX_STRIP_PARAM_EQ1_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ2_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ3_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ4_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ5_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ6_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ7_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ8_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ9_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ10_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ11_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ12_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ13_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ14_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ15_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ16_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ17_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ18_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ19_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ20_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ21_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ22_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ23_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ24_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ25_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ26_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ27_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ28_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ29_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ30_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ31_TYPE]), (int)lrintf(values[OMX_STRIP_PARAM_EQ32_TYPE])},
                                  /* freq */ (const float[OMX_EQ_INSTANCE_BANDS]){values[OMX_STRIP_PARAM_EQ1_FREQ], values[OMX_STRIP_PARAM_EQ2_FREQ], values[OMX_STRIP_PARAM_EQ3_FREQ], values[OMX_STRIP_PARAM_EQ4_FREQ], values[OMX_STRIP_PARAM_EQ5_FREQ], values[OMX_STRIP_PARAM_EQ6_FREQ], values[OMX_STRIP_PARAM_EQ7_FREQ], values[OMX_STRIP_PARAM_EQ8_FREQ], values[OMX_STRIP_PARAM_EQ9_FREQ], values[OMX_STRIP_PARAM_EQ10_FREQ], values[OMX_STRIP_PARAM_EQ11_FREQ], values[OMX_STRIP_PARAM_EQ12_FREQ], values[OMX_STRIP_PARAM_EQ13_FREQ], values[OMX_STRIP_PARAM_EQ14_FREQ], values[OMX_STRIP_PARAM_EQ15_FREQ], values[OMX_STRIP_PARAM_EQ16_FREQ], values[OMX_STRIP_PARAM_EQ17_FREQ], values[OMX_STRIP_PARAM_EQ18_FREQ], values[OMX_STRIP_PARAM_EQ19_FREQ], values[OMX_STRIP_PARAM_EQ20_FREQ], values[OMX_STRIP_PARAM_EQ21_FREQ], values[OMX_STRIP_PARAM_EQ22_FREQ], values[OMX_STRIP_PARAM_EQ23_FREQ], values[OMX_STRIP_PARAM_EQ24_FREQ], values[OMX_STRIP_PARAM_EQ25_FREQ], values[OMX_STRIP_PARAM_EQ26_FREQ], values[OMX_STRIP_PARAM_EQ27_FREQ], values[OMX_STRIP_PARAM_EQ28_FREQ], values[OMX_STRIP_PARAM_EQ29_FREQ], values[OMX_STRIP_PARAM_EQ30_FREQ], values[OMX_STRIP_PARAM_EQ31_FREQ], values[OMX_STRIP_PARAM_EQ32_FREQ]},
                                  /* gain */ (const float[OMX_EQ_INSTANCE_BANDS]){values[OMX_STRIP_PARAM_EQ1_GAIN], values[OMX_STRIP_PARAM_EQ2_GAIN], values[OMX_STRIP_PARAM_EQ3_GAIN], values[OMX_STRIP_PARAM_EQ4_GAIN], values[OMX_STRIP_PARAM_EQ5_GAIN], values[OMX_STRIP_PARAM_EQ6_GAIN], values[OMX_STRIP_PARAM_EQ7_GAIN], values[OMX_STRIP_PARAM_EQ8_GAIN], values[OMX_STRIP_PARAM_EQ9_GAIN], values[OMX_STRIP_PARAM_EQ10_GAIN], values[OMX_STRIP_PARAM_EQ11_GAIN], values[OMX_STRIP_PARAM_EQ12_GAIN], values[OMX_STRIP_PARAM_EQ13_GAIN], values[OMX_STRIP_PARAM_EQ14_GAIN], values[OMX_STRIP_PARAM_EQ15_GAIN], values[OMX_STRIP_PARAM_EQ16_GAIN], values[OMX_STRIP_PARAM_EQ17_GAIN], values[OMX_STRIP_PARAM_EQ18_GAIN], values[OMX_STRIP_PARAM_EQ19_GAIN], values[OMX_STRIP_PARAM_EQ20_GAIN], values[OMX_STRIP_PARAM_EQ21_GAIN], values[OMX_STRIP_PARAM_EQ22_GAIN], values[OMX_STRIP_PARAM_EQ23_GAIN], values[OMX_STRIP_PARAM_EQ24_GAIN], values[OMX_STRIP_PARAM_EQ25_GAIN], values[OMX_STRIP_PARAM_EQ26_GAIN], values[OMX_STRIP_PARAM_EQ27_GAIN], values[OMX_STRIP_PARAM_EQ28_GAIN], values[OMX_STRIP_PARAM_EQ29_GAIN], values[OMX_STRIP_PARAM_EQ30_GAIN], values[OMX_STRIP_PARAM_EQ31_GAIN], values[OMX_STRIP_PARAM_EQ32_GAIN]},
                                  /* q */ (const float[OMX_EQ_INSTANCE_BANDS]){values[OMX_STRIP_PARAM_EQ1_Q], values[OMX_STRIP_PARAM_EQ2_Q], values[OMX_STRIP_PARAM_EQ3_Q], values[OMX_STRIP_PARAM_EQ4_Q], values[OMX_STRIP_PARAM_EQ5_Q], values[OMX_STRIP_PARAM_EQ6_Q], values[OMX_STRIP_PARAM_EQ7_Q], values[OMX_STRIP_PARAM_EQ8_Q], values[OMX_STRIP_PARAM_EQ9_Q], values[OMX_STRIP_PARAM_EQ10_Q], values[OMX_STRIP_PARAM_EQ11_Q], values[OMX_STRIP_PARAM_EQ12_Q], values[OMX_STRIP_PARAM_EQ13_Q], values[OMX_STRIP_PARAM_EQ14_Q], values[OMX_STRIP_PARAM_EQ15_Q], values[OMX_STRIP_PARAM_EQ16_Q], values[OMX_STRIP_PARAM_EQ17_Q], values[OMX_STRIP_PARAM_EQ18_Q], values[OMX_STRIP_PARAM_EQ19_Q], values[OMX_STRIP_PARAM_EQ20_Q], values[OMX_STRIP_PARAM_EQ21_Q], values[OMX_STRIP_PARAM_EQ22_Q], values[OMX_STRIP_PARAM_EQ23_Q], values[OMX_STRIP_PARAM_EQ24_Q], values[OMX_STRIP_PARAM_EQ25_Q], values[OMX_STRIP_PARAM_EQ26_Q], values[OMX_STRIP_PARAM_EQ27_Q], values[OMX_STRIP_PARAM_EQ28_Q], values[OMX_STRIP_PARAM_EQ29_Q], values[OMX_STRIP_PARAM_EQ30_Q], values[OMX_STRIP_PARAM_EQ31_Q], values[OMX_STRIP_PARAM_EQ32_Q]},
                                  /* band_on */ (const int[OMX_EQ_INSTANCE_BANDS]){(0u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ1_ON]) : 0), (1u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ2_ON]) : 0), (2u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ3_ON]) : 0), (3u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ4_ON]) : 0), (4u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ5_ON]) : 0), (5u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ6_ON]) : 0), (6u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ7_ON]) : 0), (7u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ8_ON]) : 0), (8u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ9_ON]) : 0), (9u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ10_ON]) : 0), (10u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ11_ON]) : 0), (11u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ12_ON]) : 0), (12u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ13_ON]) : 0), (13u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ14_ON]) : 0), (14u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ15_ON]) : 0), (15u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ16_ON]) : 0), (16u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ17_ON]) : 0), (17u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ18_ON]) : 0), (18u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ19_ON]) : 0), (19u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ20_ON]) : 0), (20u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ21_ON]) : 0), (21u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ22_ON]) : 0), (22u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ23_ON]) : 0), (23u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ24_ON]) : 0), (24u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ25_ON]) : 0), (25u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ26_ON]) : 0), (26u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ27_ON]) : 0), (27u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ28_ON]) : 0), (28u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ29_ON]) : 0), (29u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ30_ON]) : 0), (30u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ31_ON]) : 0), (31u < omx_strip_eq_band_count(values) ? (int)lrintf(values[OMX_STRIP_PARAM_EQ32_ON]) : 0)});
  omx_comp_instance_resolve(&c->comp, bypass || values[OMX_STRIP_PARAM_COMP_ON] < 0.5f,
                                  /* threshold_db */ values[OMX_STRIP_PARAM_COMP_THRESHOLD],
                                  /* ratio */ values[OMX_STRIP_PARAM_COMP_RATIO],
                                  /* knee_db */ values[OMX_STRIP_PARAM_COMP_KNEE],
                                  /* attack_ms */ values[OMX_STRIP_PARAM_COMP_ATTACK],
                                  /* release_ms */ values[OMX_STRIP_PARAM_COMP_RELEASE],
                                  /* makeup_db */ values[OMX_STRIP_PARAM_COMP_MAKEUP],
                                  /* mix_pct */ values[OMX_STRIP_PARAM_COMP_MIX],
                                  /* kind */ (int)lrintf(values[OMX_STRIP_PARAM_COMP_KIND]),
                                  /* detector_oversampling */ (int)lrintf(values[OMX_STRIP_PARAM_COMP_DETECTOR_OVERSAMPLING]),
                                  /* detector_topology */ 0,
                                  /* key_source */ (int)OMX_COMP_KEY_SOURCES_DEFAULT,
                                  /* key_hp_hz */ (float)OMX_COMP_KEY_HP_HZ_RANGE_DEFAULT,
                                  /* key_lp_hz */ (float)OMX_COMP_KEY_LP_HZ_RANGE_DEFAULT,
                                  /* key_tilt_db */ (float)OMX_COMP_KEY_TILT_DB_RANGE_DEFAULT,
                                  /* key_listen */ (int)OMX_COMP_KEY_LISTENS_DEFAULT,
                                  /* hold_ms */ (float)OMX_COMP_HOLD_MS_RANGE_DEFAULT,
                                  /* detect_link */ (int)OMX_COMP_DETECT_LINKS_DEFAULT,
                                  /* duck */ (int)OMX_COMP_DUCKS_DEFAULT,
                                  /* range_db */ (float)OMX_COMP_RANGE_DB_RANGE_DEFAULT,
                                  /* release_mode */ (int)OMX_COMP_RELEASE_MODES_DEFAULT);
  const float o = values[OMX_STRIP_PARAM_ORDER];
  const long row = o - o == 0.0f ? lrintf(o) : 0;
  c->order = row >= 0 && row < (long)OMX_STRIP_CHAIN_ORDERS ? (uint32_t)row : 0u;
}

/** The latency the chain adds, in samples: the sum of its elements'. */
static inline uint32_t omx_strip_core_latency(const OmxStripCore *c) {
  (void)c;
  return (uint32_t)lrintf((float)(omx_trim_instance_latency(&c->trim)) + (float)(omx_eq_instance_latency(&c->filter)) + (float)(omx_gate_instance_latency(&c->gate)) + (float)(omx_eq_instance_latency(&c->eq)) + (float)(omx_comp_instance_latency(&c->comp)));
}

/** One element over the block, in place on out. */
static inline void omx_strip_core_element(OmxStripCore *c, uint32_t e, const float *key, float *out_l, float *out_r,
                                            uint32_t frames) {
  (void)key;
  switch (e) {
  case OMX_STRIP_CHAIN_0_trim:
    omx_trim_instance_run(&c->trim, out_l, out_r, out_l, out_r, frames);
    return;
  case OMX_STRIP_CHAIN_1_filter:
    omx_eq_instance_run(&c->filter, out_l, out_r, out_l, out_r, frames);
    return;
  case OMX_STRIP_CHAIN_2_gate:
    omx_gate_instance_run(&c->gate, NULL, out_l, out_r, out_l, out_r, frames);
    return;
  case OMX_STRIP_CHAIN_3_eq:
    omx_eq_instance_run(&c->eq, out_l, out_r, out_l, out_r, frames);
    return;
  case OMX_STRIP_CHAIN_4_comp:
    omx_comp_instance_run_keyed(&c->comp, NULL, NULL, out_l, out_r, out_l, out_r, frames);
    return;
  }
}

/** One block, stereo: in to out once (they may alias), then
 * each element in the chosen order. */
static inline void omx_strip_core_run(OmxStripCore *c, const float *in_l, const float *in_r, float *out_l,
                                         float *out_r, uint32_t frames) {
  if (!in_l || !in_r || !out_l || !out_r || frames == 0u) return;
  if (out_l != in_l) memmove(out_l, in_l, (size_t)frames * sizeof(float));
  if (out_r != in_r) memmove(out_r, in_r, (size_t)frames * sizeof(float));
  const float *key = NULL;
  for (uint32_t k = 0; k < OMX_STRIP_CHAIN_LENGTH; k++) omx_strip_core_element(c, OMX_STRIP_CHAIN_ORDER[c->order][k], key, out_l, out_r, frames);
}

/* The core behind the vtable the shared faces call (include/omx_plugin_face.h): the same functions, untyped. */
static inline void omx_strip_core_init_v(void *c, uint32_t rate) { omx_strip_core_init((OmxStripCore *)c, rate); }
static inline void omx_strip_core_resolve_v(void *c, const float *values, int bypass) {
  omx_strip_core_resolve((OmxStripCore *)c, values, bypass);
}
static inline uint32_t omx_strip_core_latency_v(const void *c) { return omx_strip_core_latency((const OmxStripCore *)c); }
static inline void omx_strip_core_run_v(void *c, const float *key, const float *in_l, const float *in_r, float *out_l,
                                           float *out_r, uint32_t frames) {
  (void)key;
  omx_strip_core_run((OmxStripCore *)c, in_l, in_r, out_l, out_r, frames);
}

static const struct omx_face_core OMX_STRIP_CORE = {sizeof(OmxStripCore), omx_strip_core_init_v, omx_strip_core_resolve_v,
                                                   omx_strip_core_latency_v, omx_strip_core_run_v,
                                                   omx_face_no_meter};

#endif /* OMX_STRIP_CORE_H */
