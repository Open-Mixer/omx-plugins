// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas
/*
 * lv2-refuse-rate-test.c — the shared LV2 face (lib/omx_lv2_face.c) refuses to instantiate at a rate its
 * core cannot hold, so a host is never handed a silent wire. Two refusals, each caught by its own arm:
 *
 *   - a rate outside the 32-bit rate every core holds (or not positive, or NaN): refused before the core
 *     is touched. The stub core takes every rate it is shown but 44100, so only the face's own bound can
 *     refuse 4294967296 Hz (its cast would be 0, which the stub takes), and the core is never initialised;
 *   - a rate the core's own answer refuses (the stub refuses 44100): instantiate returns NULL.
 *
 * Built from lib/omx_lv2_face.c with a stub face; part of `make test` (make lv2-refuse-rate).
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "omx_lv2_face.h"

static int inits;
static int stub_init(void *core, uint32_t rate) {
  (void)core;
  inits++;
  return rate != 44100u;
}
static void stub_resolve(void *core, const float *values, int bypass) { (void)core, (void)values, (void)bypass; }
static uint32_t stub_latency(const void *core) { return (void)core, 0u; }
static void stub_run(void *core, const float *key, const float *in_l, const float *in_r, float *out_l, float *out_r,
                     uint32_t frames) {
  (void)core, (void)key, (void)in_l, (void)in_r, (void)out_l, (void)out_r, (void)frames;
}
static const struct omx_face_core CORE = {16, stub_init, stub_resolve, stub_latency, stub_run, NULL};
static const struct omx_face FACE = {.core = &CORE};
static const struct omx_lv2_face LV2 = {.face = &FACE};

static int failures;
/* instantiate at `rate`: 1 when the host gets a handle; `core_calls` is how often the core was initialised */
static int instantiate(double rate, int *core_calls) {
  inits = 0;
  LV2_Handle h = omx_lv2_face_instantiate(&LV2.lv2, rate, NULL, NULL);
  if (core_calls) *core_calls = inits;
  if (h) omx_lv2_face_cleanup(h);
  return h != NULL;
}
static void check(int ok, const char *what) {
  printf("%s %s\n", ok ? "PASS" : "FAIL", what);
  if (!ok) failures++;
}

int main(void) {
  int calls;
  check(instantiate(48000.0, &calls) && calls == 1, "a rate the core takes instantiates, the core initialised once");
  check(!instantiate(44100.0, &calls) && calls == 1, "a rate the core's own answer refuses (44100) is refused");
  check(!instantiate(4294967296.0, &calls) && calls == 0, "4294967296 Hz (past the 32-bit rate) is refused before the core is touched");
  check(!instantiate(1e10, &calls) && calls == 0, "1e10 Hz is refused before the core is touched");
  check(!instantiate(0.0, &calls) && calls == 0, "0 Hz is refused before the core is touched");
  check(!instantiate(-48000.0, &calls) && calls == 0, "a negative rate is refused before the core is touched");
  check(!instantiate(NAN, &calls) && calls == 0, "NaN is refused before the core is touched");
  check(!instantiate(INFINITY, &calls) && calls == 0, "infinity is refused before the core is touched");
  printf(failures ? "lv2-refuse-rate-test: RED\n" : "lv2-refuse-rate-test: green\n");
  return failures != 0;
}
