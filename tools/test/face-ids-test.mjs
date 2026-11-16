#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * face-ids-test.mjs — the controls that are the face's own (the host's bypass, LV2 enabled and latency) sit at
 * omx-contract's permanent ids (OMX_PLUGIN_FACE_IDS), in every plugin.
 *
 *   1. EVERY PLUGIN. For each shipped declaration, the LV2 port list (tools/gen.mjs lv2Ports), the generated header's
 *      OMX_<K>_LV2_PORT_ENABLED / _LATENCY and the generated TTL's ports put enabled and latency at the contract's
 *      indices; the built CLAP face's bypass id is held by tools/clap-params-check.mjs against the same constant.
 *   2. METERS. A plugin's meter output ports are STRUCTURAL: after the face's own ports (and the tempo's control), before
 *      the audio ports and the parameters, in the order the declaration lists its meters, one port a channel. The
 *      generated header's FIRST_METER and the TTL agree with the list.
 * `make face-ids`, part of `make test`. Needs the installed omx-contract (>= 3.9.0). Exit 1 on any failure.
 */
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { ROOT, loadDecl, lv2Ports, metersOf, pluginDirs, tempoOf } from '../gen.mjs';
import { faceIds } from '../param-contract.mjs';

const ids = faceIds();
let failed = 0;
const say = (ok, m) => {
  if (!ok) failed++;
  console.log(`${ok ? 'PASS' : 'FAIL'} face-ids ${m}`);
};

const indexOf = (ports, symbol) => ports.find((p) => p.symbol === symbol)?.index;

const decls = pluginDirs(ROOT).map((dir) => loadDecl(dir));

for (const d of decls) {
  const K = d.kernel.toUpperCase();
  const ports = lv2Ports(d);
  say(indexOf(ports, 'enabled') === ids.enabledLv2Index && indexOf(ports, 'latency') === ids.latencyLv2Index,
    `${d.stem}: enabled at ${indexOf(ports, 'enabled')}, latency at ${indexOf(ports, 'latency')} (contract ${ids.enabledLv2Index}, ${ids.latencyLv2Index})`);
  const dir = join(ROOT, 'plugins', d.stem);
  const header = readFileSync(join(dir, 'generated', `omx_${d.kernel}_params.h`), 'utf8');
  const def = (n) => Number(header.match(new RegExp(`^#define OMX_${K}_LV2_PORT_${n} (\\d+)u$`, 'm'))?.[1]);
  say(def('ENABLED') === ids.enabledLv2Index && def('LATENCY') === ids.latencyLv2Index, `${d.stem}: the generated header's LV2 enabled and latency are the contract's`);
  const ttl = readFileSync(join(dir, 'generated', `${d.stem}.lv2`, `${d.stem}.ttl`), 'utf8');
  const at = (symbol) => Number(ttl.match(new RegExp(`lv2:index (\\d+) ;\\s*lv2:symbol "${symbol}"`))?.[1]);
  say(at('enabled') === ids.enabledLv2Index && at('latency') === ids.latencyLv2Index, `${d.stem}: the generated TTL's enabled and latency are the contract's`);
  const meterSyms = metersOf(d).flatMap((mt) => mt.symbols);
  if (meterSyms.length) {
    const at0 = meterSyms.map((sy) => indexOf(ports, sy));
    const firstAudio = Math.min(...['in_l', 'in_r', 'out_l', 'out_r'].map((sy) => indexOf(ports, sy))), firstParam = Math.min(...d.params.map((p) => indexOf(ports, p.symbol)));
    const structuralFloor = 2 + (tempoOf(d) ? 1 : 0);
    say(at0.every((x, k) => x === structuralFloor + k) && at0.at(-1) < firstAudio && firstAudio < firstParam,
      `${d.stem}: the ${meterSyms.length} meter ports are structural, in declared order at ${at0[0]}..${at0.at(-1)}, before the audio (${firstAudio}) and the parameters (${firstParam})`);
    const header2 = header.match(new RegExp(`^#define OMX_${K}_LV2_PORT_FIRST_METER (\\d+)u$`, 'm'))?.[1];
    say(Number(header2) === at0[0], `${d.stem}: the generated header's FIRST_METER is ${at0[0]}`);
    say(meterSyms.every((sy, k) => at(sy) === at0[k]), `${d.stem}: the generated TTL's meter ports are where the list puts them`);
  }
}
console.log(failed ? `face-ids: ${failed} failed` : `face-ids: ${decls.length} plugins, all held`);
process.exit(failed ? 1 : 0);
