/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
 *
 * omx_plugin_param.h — the SHAPE of one generated plugin parameter, and nothing else: no number
 * lives here. The rows are emitted by tools/gen.mjs into generated/omx_<kernel>_params.h from the
 * plugin's declaration (plugins/<name>/<name>.decl.json); the CLAP and LV2 faces read them and type
 * no range of their own.
 */
#ifndef OMX_PLUGIN_PARAM_H
#define OMX_PLUGIN_PARAM_H

/** A whole-number travel (declared `step` >= 1 over integer bounds). */
#define OMX_PLUGIN_PARAM_INTEGER 1u
/** A declared boolean: domain {0, 1}, the come-up value declared. */
#define OMX_PLUGIN_PARAM_TOGGLE 2u
/** A choice whose points are every whole step of its travel (with OMX_PLUGIN_PARAM_INTEGER): the CLAP face's
 * enumeration (CLAP_PARAM_IS_ENUM, a value text per step from the face's labels table), the LV2 face's scale points.
 * A choice with gaps (a slope's 12 and 24) carries no flag and stays a stepped travel on CLAP. */
#define OMX_PLUGIN_PARAM_CHOICE 4u

typedef struct {
  const char *symbol; /* the declared field name — the LV2 symbol, the CLAP param's identity */
  const char *name;   /* derived from the symbol, never a label table */
  const char *unit;
  float min, max, def;
  unsigned flags;
} omx_plugin_param;

#endif /* OMX_PLUGIN_PARAM_H */
