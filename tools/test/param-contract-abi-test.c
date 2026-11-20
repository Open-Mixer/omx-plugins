// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * param-contract-abi-test.c — org.openmixer.param-contract/1 is omx-clap's <omx-clap-host/omx_clap_ext.h>;
 * what this tree keeps is struct omx_param_contract_row, the unit, scale and step a face fills a host's entry
 * from. The row's members are the entry's, member for member, and the scale codes are the owner's: a member
 * changed on either side, or a scale added on one, stops this compiling.
 * Compile-only: `make param-contract-abi`, part of `make test`.
 */
#include "omx_param_contract.h"

#include <omx-clap-host/omx_clap_ext.h>

#include <stddef.h>
#include <string.h>

#define MEMBER(t, m) (((t *)0)->m)
#define SAME_MEMBER(row_m, entry_m) \
    _Static_assert(sizeof(MEMBER(struct omx_param_contract_row, row_m)) == sizeof(MEMBER(omx_clap_param_contract_entry_t, entry_m)), \
                   "the row's " #row_m " is the entry's " #entry_m)

/* the row's three members are the entry's unit, scale and step */
SAME_MEMBER(unit, unit);
SAME_MEMBER(scale, scale);
SAME_MEMBER(step, step);
_Static_assert(offsetof(struct omx_param_contract_row, unit) == 0, "the row opens with the unit");

/* the owner's table a face serves: version, fingerprint and get, nothing after them */
#define PTR sizeof(void (*)(void))
_Static_assert(sizeof(omx_clap_plugin_param_contract_ext_t) == 3 * PTR, "the param-contract table is version, fingerprint and get");
_Static_assert(offsetof(omx_clap_plugin_param_contract_ext_t, version) == 0, "offsetof version");
_Static_assert(offsetof(omx_clap_plugin_param_contract_ext_t, fingerprint) == PTR, "offsetof fingerprint");
_Static_assert(offsetof(omx_clap_plugin_param_contract_ext_t, get) == 2 * PTR, "offsetof get");

/* the scale codes this tree writes into a row are the owner's three */
#define COUNT_SCALE(code, name) +1
_Static_assert(OMX_CLAP_SCALE_COUNT == (0 OMX_CLAP_PARAM_SCALES(COUNT_SCALE)), "the scale codes");
_Static_assert(OMX_CLAP_SCALE_STEPPED == 2, "stepped is the owner's code 2");

int main(void)
{
    return strcmp(OMX_CLAP_EXT_PARAM_CONTRACT, "org.openmixer.param-contract/1") != 0;
}
