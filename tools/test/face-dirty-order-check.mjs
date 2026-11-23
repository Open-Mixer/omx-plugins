#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * face-dirty-order-check.mjs — the CLAP face's `dirty` flag carries the values the main thread wrote to the
 * audio thread, so its memory order is part of the contract:
 *
 *   - every read of `dirty` (exchange, load, compare-exchange) is acquire or stronger: the audio thread then
 *     sees the `values` stored before the flag, not stale ones (a relaxed read is free to on aarch64);
 *   - every store of `dirty` outside the audio thread (g_drain, g_process) is release or stronger: the values
 *     are published before the flag.
 *
 * A memory-order bug does not show in a single-threaded run, so this reads the source. --self-test plants a
 * relaxed read, a relaxed main-thread store and a source with no read at all, each must go red.
 *
 * `make face-dirty-order`, part of `make test`.
 */
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { ROOT } from '../gen.mjs';
import { isMain } from '../omx-contract.mjs';

const FACE = 'lib/omx_clap_face.c';
/** The functions that run on the audio thread: a relaxed store there is ordered by program order. */
const AUDIO_THREAD = new Set(['g_drain', 'g_process']);
const ACQUIRE = new Set(['memory_order_acquire', 'memory_order_acq_rel', 'memory_order_seq_cst']);
const RELEASE = new Set(['memory_order_release', 'memory_order_acq_rel', 'memory_order_seq_cst']);

/** The function a position sits in: the last `static ... name(` at the start of a line before it. */
function enclosing(src, at) {
  let name = null;
  for (const m of src.slice(0, at).matchAll(/^static\b[^\n(]*?\b(\w+)\s*\(/gm)) name = m[1];
  return name;
}

/** Every defect, one line each. */
export function dirtyOrderErrors(src, file = FACE) {
  const errors = [];
  let reads = 0;
  for (const m of src.matchAll(/\batomic_(exchange|load|store|compare_exchange_strong|compare_exchange_weak)(_explicit)?\s*\(\s*&\s*g->dirty\b([^;]*)/g)) {
    const line = src.slice(0, m.index).split('\n').length;
    const op = m[1];
    const orders = m[2] ? [...m[3].matchAll(/memory_order_\w+/g)].map((o) => o[0]) : ['memory_order_seq_cst'];
    const where = `${file}:${line} atomic_${op}${m[2] ?? ''}(&g->dirty, ...)`;
    if (op === 'store') {
      const fn = enclosing(src, m.index);
      if (!AUDIO_THREAD.has(fn) && !RELEASE.has(orders[0])) errors.push(`${where} in ${fn}: a main-thread store of dirty is ${orders[0]}, it must be release or stronger`);
    } else {
      reads++;
      // a compare-exchange's failure order is a load too, but only its success order pairs with the writers
      if (!ACQUIRE.has(orders[0])) errors.push(`${where}: a read of dirty is ${orders[0]}, it must be acquire or stronger (the values stored before the flag may read stale)`);
    }
  }
  if (reads === 0) errors.push(`${file}: no read of g->dirty found; the check has nothing to hold`);
  return errors;
}

if (isMain(import.meta.url)) {
  const src = readFileSync(join(ROOT, FACE), 'utf8');
  const errors = dirtyOrderErrors(src);
  for (const e of errors) console.log(`FAIL face-dirty-order ${e}`);
  let bad = errors.length;
  if (process.argv.includes('--self-test')) {
    const arms = [
      ['a relaxed read of dirty', 'static int g_process(void) {\n  if (atomic_exchange_explicit(&g->dirty, 0, memory_order_relaxed)) run();\n}\n'],
      ['a relaxed store of dirty on the main thread', 'static int g_reset(void) {\n  atomic_store_explicit(&g->dirty, 1, memory_order_relaxed);\n}\nstatic int g_process(void) {\n  if (atomic_exchange_explicit(&g->dirty, 0, memory_order_acquire)) run();\n}\n'],
      ['a source with no read of dirty', 'static int g_reset(void) {\n  atomic_store(&g->dirty, 1);\n}\n'],
    ];
    for (const [what, planted] of arms) {
      const red = dirtyOrderErrors(planted, 'planted').length > 0;
      console.log(`${red ? 'PASS' : 'FAIL'} face-dirty-order sabotage: ${what} is red`);
      if (!red) bad++;
    }
    const green = dirtyOrderErrors('static int g_drain(void) {\n  atomic_store_explicit(&g->dirty, 1, memory_order_relaxed);\n}\nstatic int g_process(void) {\n  if (atomic_exchange_explicit(&g->dirty, 0, memory_order_acquire)) run();\n}\n', 'planted').length === 0;
    console.log(`${green ? 'PASS' : 'FAIL'} face-dirty-order control: an audio-thread relaxed store and an acquire read are green`);
    if (!green) bad++;
  }
  console.log(bad ? `face-dirty-order: ${bad} failure(s)` : `PASS face-dirty-order: every read of dirty in ${FACE} is acquire or stronger, every main-thread store release or stronger`);
  process.exit(bad ? 1 : 0);
}
