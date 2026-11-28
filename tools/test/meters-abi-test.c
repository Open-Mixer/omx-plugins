// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * meters-abi-test.c — the clap.meters/1 declarations omx-clap installs as
 * <omx-clap-host/clap_meters.h> are the layout the plugins here are built for: the extension id, the
 * enum values, and sizeof/offsetof of every structure they use. A change in omx-clap, or in the
 * SDK header it prefers once the SDK has one, stops this compiling (FreeMixer/omx-plugins#119).
 * Compile-only: `make meters-abi`, part of `make test`.
 */
#include <omx-clap-host/clap_meters.h>

#include <stddef.h>
#include <string.h>

#define SIZE(t, n) _Static_assert(sizeof(t) == (n), "sizeof " #t)
#define OFF(t, f, n) _Static_assert(offsetof(t, f) == (n), "offsetof " #t "." #f)

_Static_assert(CLAP_METER_KIND_OTHER == 0, "CLAP_METER_KIND_OTHER");
_Static_assert(CLAP_METER_KIND_GAIN_REDUCTION == 1, "CLAP_METER_KIND_GAIN_REDUCTION");
_Static_assert(CLAP_METER_KIND_LEVEL == 2, "CLAP_METER_KIND_LEVEL");
_Static_assert(CLAP_METER_IS_COHERENT == 1, "CLAP_METER_IS_COHERENT");

SIZE(clap_meter_info_t, 288);
OFF(clap_meter_info_t, id, 0);
OFF(clap_meter_info_t, name, 4);
OFF(clap_meter_info_t, kind, 260);
OFF(clap_meter_info_t, flags, 264);
OFF(clap_meter_info_t, channel_count, 268);
OFF(clap_meter_info_t, min_value, 272);
OFF(clap_meter_info_t, max_value, 280);
SIZE(clap_plugin_meters_t, 24);
OFF(clap_plugin_meters_t, count, 0);
OFF(clap_plugin_meters_t, get_info, 8);
OFF(clap_plugin_meters_t, get_values, 16);
SIZE(clap_host_meters_t, 8);
OFF(clap_host_meters_t, changed, 0);

int main(void)
{
    return strcmp(CLAP_EXT_METERS, "clap.meters/1") != 0;
}
