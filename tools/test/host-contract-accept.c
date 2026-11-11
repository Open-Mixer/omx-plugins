// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * host-contract-accept.c — every built face's org.openmixer.param-contract/1 is trusted by the host that reads it:
 * libomx-clap-core opens the .clap and omx_clap_host_param_contract() answers 1. The host computes the fingerprint
 * itself, over the ids get_info answers, so this holds what tools/param-contract.mjs writes to an implementation that
 * shares none of its code; clap-ext-check recomputes with contractCanonical and cannot tell a canonical that is wrong
 * the same way on both sides.
 *
 *   host-contract-accept <face.clap>...
 *
 * Exit 0 every face trusted; 1 a face not opened or its contract not trusted, the host's reason printed; 2 usage.
 */
#include <omx-clap-host/clap_host.h>

#include <stdio.h>

int main(int argc, char **argv)
{
    int bad = 0;

    if (argc < 2)
    {
        fputs("usage: host-contract-accept <face.clap>...\n", stderr);
        return 2;
    }
    for (int i = 1; i < argc; i++)
    {
        struct omx_clap_instance *in = NULL;
        char why[OMX_CLAP_WHY_MAX] = "";
        const char *reason = NULL;
        uint32_t version = 0;

        if (omx_clap_host_open(argv[i], NULL, &in, why) != 0)
        {
            printf("FAIL host-contract %s: the host did not open it: %s\n", argv[i], why);
            bad = 1;
            continue;
        }
        if (omx_clap_host_param_contract(in, &version, &reason) == 1)
            printf("ok   host-contract %s: version %u trusted\n", argv[i], version);
        else
        {
            printf("FAIL host-contract %s: not trusted: %s\n", argv[i], reason ? reason : "(no reason)");
            bad = 1;
        }
        omx_clap_host_close(in);
    }
    return bad;
}
