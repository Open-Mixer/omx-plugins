// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas
/*
 * THE PANEL ENUMS FOLLOW THE CONTRACT (Open-Mixer/omx-clap-core#104). include/omx_panel_description.h's families,
 * widgets and display kinds are omx-contract's PANEL_FAMILIES, PANEL_WIDGETS and PANEL_DISPLAY_KINDS, never a copy
 * spelled here. Built with tools/test/fixtures/contract-one-more-family first on the include path, a contract that
 * declares one more of each: every enumerator the fixture lists is declared with the fixture's code, with no edit in
 * this tree. A hand enum stays at the words it was written with, and the build stops at the first enumerator it lacks.
 * `make panel-enums`, part of `make test`.
 */
#include <stdio.h>

#include <omxcontract/omx_vocabulary.h>

#include "omx_panel_description.h"

static int g_failures;

#define CHECK(prefix, code, id)                                                                                        \
    do {                                                                                                               \
        if ((long)(prefix##id) == (long)(code)) printf("ok   " #prefix #id " = %ld\n", (long)(code));                 \
        else { printf("FAIL " #prefix #id " is %ld, the contract says %ld\n", (long)(prefix##id), (long)(code)); g_failures++; } \
    } while (0);
#define FAMILY(code, name, id) CHECK(OMX_CLAP_FAMILY_, code, id)
#define WIDGET(code, name, id) CHECK(OMX_CLAP_WIDGET_, code, id)
#define DISPLAY_KIND(code, name, id) CHECK(OMX_CLAP_DISPLAY_, code, id)

int main(void)
{
    OMX_PANEL_FAMILIES(FAMILY)
    OMX_PANEL_WIDGETS(WIDGET)
    OMX_PANEL_DISPLAY_KINDS(DISPLAY_KIND)
    return g_failures ? 1 : 0;
}
