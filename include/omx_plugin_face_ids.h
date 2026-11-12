/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * This file is part of omx-plugins.
 *
 * omx-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * omx-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with omx-plugins.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
 */

#ifndef OMX_PLUGIN_FACE_IDS_H
#define OMX_PLUGIN_FACE_IDS_H
/*
 * GENERATED: DO NOT EDIT BY HAND.
 * Rendered from OMX_PLUGIN_FACE_IDS of contract/plugin-face.json: the ids a plugin face gives its own controls.
 * omx-plugins' CLAP and LV2 faces include this header; the item is installed into omx-contract's contract directory.
 */

/** @file
 * @brief The face's own controls' permanent ids, one OMX_PLUGIN_FACE_ define per field. */

/** the CLAP id of the host's bypass and the LV2 indices of enabled and latency, the same in every plugin and every release */
#define OMX_PLUGIN_FACE_BYPASS_CLAP_ID 2147418112u  ///< 0x7FFF0000: the host's bypass, clap_param_info.id; below INT32_MAX so a host storing ids signed reads it whole
#define OMX_PLUGIN_FACE_ENABLED_LV2_INDEX 0u  ///< the input port lv2:enabled, 1 processing and 0 bypassed
#define OMX_PLUGIN_FACE_LATENCY_LV2_INDEX 1u  ///< the output port reporting the latency in frames

#endif /* OMX_PLUGIN_FACE_IDS_H */
