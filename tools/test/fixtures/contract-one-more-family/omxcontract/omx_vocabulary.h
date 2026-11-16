// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas
/*
 * A FIXTURE, not the contract: omx-contract's <omxcontract/omx_vocabulary.h> reduced to the three panel lists
 * omx-clap-core and omx-plugins build their tables and enums from, with one family, one widget and one display kind more than any release
 * declares. tools/test/panel-enums-follow-contract.c is built with this directory first on the include path: the
 * enums of include/omx_panel_description.h must follow it with no edit here (Open-Mixer/omx-clap-core#104).
 */
#ifndef OMX_VOCABULARY_H
#define OMX_VOCABULARY_H

#define OMX_PANEL_FAMILIES(X) \
    X(0u, "none", NONE) X(1u, "gate", GATE) X(2u, "dynamics", DYNAMICS) X(3u, "limiter", LIMITER) \
    X(4u, "deesser", DEESSER) X(5u, "eq", EQ) X(6u, "filter", FILTER) X(7u, "drive", DRIVE) \
    X(8u, "modulation", MODULATION) X(9u, "delay", DELAY) X(10u, "reverb", REVERB) X(11u, "pitch", PITCH) \
    X(12u, "utility", UTILITY) X(13u, "fixture_family", FIXTURE_FAMILY)

#define OMX_PANEL_WIDGETS(X) \
    X(0u, "auto", AUTO) X(1u, "knob", KNOB) X(2u, "fader", FADER) X(3u, "switch", SWITCH) \
    X(4u, "selector", SELECTOR) X(5u, "fixture_widget", FIXTURE_WIDGET)

#define OMX_PANEL_DISPLAY_KINDS(X) \
    X(0u, "gain_reduction_bar", GAIN_REDUCTION_BAR) X(1u, "transfer_curve", TRANSFER_CURVE) X(2u, "eq_response", EQ_RESPONSE) X(3u, "meter", METER) \
    X(4u, "fixture_display", FIXTURE_DISPLAY)

#endif /* OMX_VOCABULARY_H */
