#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * strip-order-enumeration-test.mjs — omx-strip's Order names the order of its five stages: its descriptor control is a CHOICE whose
 * labels are the 120 orders (input > filters > gate > EQ > comp first, the lexicographic permutations after), and its LV2 port
 * carries the same 120 scale points, so mod-ui and every host draw a selector that names the order, not a knob reading a number.
 * Reviewed on the real mod-ui shot: Order was a bare knob reading 0.
 *
 * `make strip-order-enumeration`, part of `make test`. It loads the declaration, so it needs omx-contract and omx-dsp.
 */
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { descriptorOf } from '../descriptor.mjs';
import { loadDecl, permutations } from '../gen.mjs';

const ROOT = join(fileURLToPath(new URL('.', import.meta.url)), '..', '..');
let bad = 0, n = 0;
const check = (what, ok, detail = '') => { n++; if (!ok) bad++; console.log(`${ok ? 'PASS' : 'FAIL'} strip-order-enumeration ${what}${ok ? '' : `: ${detail}`}`); };

const d = loadDecl(join(ROOT, 'plugins', 'omx-strip'));
const order = descriptorOf(d).descriptor.controls.find((c) => c.symbol === 'order');
const NAMES = { trim: 'Input', filter: 'Filters', gate: 'Gate', eq: 'EQ', comp: 'Comp' };
const rows = permutations(d.chain.length).map((row) => row.map((i) => NAMES[d.chain[i].id]).join(' > '));
check('the Order control is a choice', order?.kind === 'choice', `kind ${order?.kind}`);
check('it has the 120 orders as labels, the desk\'s first', order?.labels?.length === 120 && order.labels[0] === 'Input > Filters > Gate > EQ > Comp' && order.labels[119] === 'Comp > EQ > Gate > Filters > Input', JSON.stringify(order?.labels?.slice(0, 2)));
check('every label is the order its value runs', JSON.stringify(order?.labels) === JSON.stringify(rows), 'labels differ from the permutations');
check('the values are 0..119', JSON.stringify(order?.values) === JSON.stringify([...Array(120).keys()]));
const ttl = readFileSync(join(ROOT, 'plugins', 'omx-strip', 'generated', 'omx-strip.lv2', 'omx-strip.ttl'), 'utf8');
const port = ttl.split('lv2:symbol "order" ;')[1]?.split('lv2:symbol')[0] ?? '';
check('its LV2 port is an enumeration', /lv2:enumeration/.test(port), 'no lv2:enumeration');
check('its LV2 port has 120 scale points', (port.match(/lv2:scalePoint/g) ?? []).length === 120 || (port.match(/rdfs:label "/g) ?? []).length === 120, `${(port.match(/rdfs:label "/g) ?? []).length} labels`);
console.log(`strip-order-enumeration: ${n - bad}/${n} passed`);
process.exit(bad ? 1 : 0);
