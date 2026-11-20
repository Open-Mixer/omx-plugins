// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * sine-voice-oracle.c — omx test-sine-voice against its closed form, through the BUILT .clap, at the
 * nine rates an audio interface clocks at (OMX_ORACLE_FULL_RATES, 32 to 192 kHz).
 *
 *   sine-voice-oracle <omx-test-sine-voice.clap>
 *
 * The closed form is the face's header comment, written here apart from the face (long double,
 * sin of the whole argument, no running phase): y(k) = GAIN * v * env(k) * sin(2 pi f k / rate).
 * Arms, at every rate, in blocks of BLOCK frames, events at their frame:
 *
 *   note     note 69 at velocity 100: every sample of both legs is the closed form (440 Hz, level
 *            GAIN * 100/127 after the attack); the sustain's peak is that level and its upward zero
 *            crossings count 440 per second
 *   partials note 69 (CLAP) at frame 0 and note 76 (MIDI) at frame 37: the sum of two closed forms
 *   release  a note off mid-block: the linear release, silence (exact zeros) from R frames after it
 *            on, and CLAP_EVENT_NOTE_END at that frame
 *   velocity0  a MIDI note on of velocity 0 is that note off
 *   voices   voice-info reports VOICES; VOICES + 1 notes end the oldest voice at the last note on
 *
 * `PASS ...` / `FAIL ...` per check; the exit status is the number of failures (capped).
 */
#include <dlfcn.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clap/clap.h>
#include <omxdsp/omxdsp_definitions.h> /* the oracle rates, omx-dsp's ORACLE_FULL_RATES */

#include "omx_test_sine_voice_decl.h"

#define BLOCK 256u
#define MAXEV 64u
#define VOICES ((uint32_t)OMX_TEST_SINE_VOICE_VOICES)

static const float RATES[OMX_ORACLE_FULL_RATES_COUNT] = OMX_ORACLE_FULL_RATES_INIT;
static int failures;

#define CHECK(cond, ...)                                                                                     \
  do {                                                                                                       \
    bool ok_ = (cond);                                                                                       \
    printf("%s ", ok_ ? "PASS" : "FAIL");                                                                    \
    printf(__VA_ARGS__);                                                                                     \
    printf("\n");                                                                                            \
    if (!ok_) failures++;                                                                                    \
  } while (0)

/* ---- the closed form ---------------------------------------------------------------------- */

typedef struct {
  int key;
  double vel;     /* 0..1 */
  int64_t t0;     /* note-on frame */
  int64_t t1;     /* note-off frame, or -1 */
} Note;

static long double frames_of(double ms, double rate) {
  long double f = roundl((long double)ms * rate / 1000.0L);
  return f < 1 ? 1 : f;
}

static long double closed(const Note *n, int64_t frame, double rate) {
  const int64_t k = frame - n->t0;
  if (k < 0) return 0.0L;
  const long double A = frames_of(OMX_TEST_SINE_VOICE_ATTACK_MS, rate), R = frames_of(OMX_TEST_SINE_VOICE_RELEASE_MS, rate);
  long double env;
  if (n->t1 < 0 || frame < n->t1) env = k >= A ? 1.0L : k / A;
  else {
    const int64_t k1 = n->t1 - n->t0, j = frame - n->t1;
    const long double e0 = k1 >= A ? 1.0L : k1 / A;
    env = j >= R ? 0.0L : e0 * (1.0L - j / R);
  }
  const long double f = OMX_TEST_SINE_VOICE_A4_HZ * powl(2.0L, (n->key - 69) / 12.0L);
  return OMX_TEST_SINE_VOICE_GAIN * n->vel * env * sinl(2.0L * 3.141592653589793238462643383279L * f * k / rate);
}

/* ---- a host with an event list ------------------------------------------------------------ */

typedef union {
  clap_event_header_t h;
  clap_event_note_t note;
  clap_event_midi_t midi;
} Ev;

static Ev in_ev[MAXEV];
static uint32_t in_n;
static int64_t ends[MAXEV];    /* absolute frame of each NOTE_END */
static int ends_key[MAXEV];
static uint32_t ends_n;
static int64_t block_at;

static uint32_t ev_size(const clap_input_events_t *l) {
  (void)l;
  return in_n;
}
static const clap_event_header_t *ev_get(const clap_input_events_t *l, uint32_t i) {
  (void)l;
  return &in_ev[i].h;
}
static bool ev_push(const clap_output_events_t *l, const clap_event_header_t *e) {
  (void)l;
  if (e->space_id == CLAP_CORE_EVENT_SPACE_ID && e->type == CLAP_EVENT_NOTE_END && ends_n < MAXEV) {
    ends[ends_n] = block_at + e->time;
    ends_key[ends_n++] = ((const clap_event_note_t *)e)->key;
  }
  return true;
}
static const clap_input_events_t IN = {NULL, ev_size, ev_get};
static const clap_output_events_t OUT = {NULL, ev_push};

static const void *host_ext(const clap_host_t *h, const char *id) {
  (void)h, (void)id;
  return NULL;
}
static void host_noop(const clap_host_t *h) { (void)h; }
static const clap_host_t HOST = {CLAP_VERSION_INIT, NULL, "sine-voice-oracle", "openmixer", "", "0.1", host_ext, host_noop, host_noop, host_noop};

/* One event to play at an absolute frame: a CLAP note (on/off) or a MIDI message. */
typedef struct {
  int64_t at;
  bool midi;
  uint16_t type; /* CLAP_EVENT_NOTE_ON / OFF when !midi */
  int key;
  double vel;    /* CLAP velocity, or the MIDI byte when midi */
  uint8_t status;
} Play;

/* Render `frames` frames of `plug` into l, r with the events of `play`, in blocks of BLOCK. */
static void render(const clap_plugin_t *plug, const Play *play, uint32_t np, float *l, float *r, int64_t frames) {
  ends_n = 0;
  for (block_at = 0; block_at < frames; block_at += BLOCK) {
    const uint32_t n = (uint32_t)(frames - block_at < BLOCK ? frames - block_at : BLOCK);
    in_n = 0;
    for (uint32_t i = 0; i < np; i++) {
      if (play[i].at < block_at || play[i].at >= block_at + n) continue;
      Ev *e = &in_ev[in_n++];
      memset(e, 0, sizeof *e);
      if (play[i].midi) {
        e->midi.header = (clap_event_header_t){sizeof e->midi, (uint32_t)(play[i].at - block_at), CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_MIDI, 0};
        e->midi.port_index = 0;
        e->midi.data[0] = play[i].status, e->midi.data[1] = (uint8_t)play[i].key, e->midi.data[2] = (uint8_t)play[i].vel;
      } else {
        e->note.header = (clap_event_header_t){sizeof e->note, (uint32_t)(play[i].at - block_at), CLAP_CORE_EVENT_SPACE_ID, play[i].type, 0};
        e->note.note_id = -1, e->note.port_index = 0, e->note.channel = 0, e->note.key = (int16_t)play[i].key;
        e->note.velocity = play[i].vel;
      }
    }
    float *outs[2] = {l + block_at, r + block_at};
    clap_audio_buffer_t ob = {outs, NULL, 2, 0, 0};
    const clap_process_t proc = {block_at, n, NULL, NULL, &ob, 0, 1, &IN, &OUT};
    plug->process(plug, &proc);
  }
}

/* The largest distance between both legs and the sum of the notes' closed forms over [from, to). */
static double distance(const float *l, const float *r, const Note *notes, uint32_t nn, int64_t from, int64_t to, double rate) {
  double worst = 0.0;
  for (int64_t f = from; f < to; f++) {
    long double want = 0.0L;
    for (uint32_t i = 0; i < nn; i++) want += closed(&notes[i], f, rate);
    const double dl = fabs((double)(l[f] - want)), dr = fabs((double)(r[f] - want));
    if (dl > worst) worst = dl;
    if (dr > worst) worst = dr;
  }
  return worst;
}

#define TOL 2e-6 /* a float's rounding of a value below 1, with room */

int main(int argc, char **argv) {
  if (argc != 2) return fprintf(stderr, "usage: sine-voice-oracle <omx-test-sine-voice.clap>\n"), 2;
  void *lib = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
  if (!lib) return fprintf(stderr, "FAIL dlopen %s: %s\n", argv[1], dlerror()), 1;
  const clap_plugin_entry_t *entry = (const clap_plugin_entry_t *)dlsym(lib, "clap_entry");
  if (!entry || !entry->init(argv[1])) return fprintf(stderr, "FAIL no clap_entry in %s\n", argv[1]), 1;
  const clap_plugin_factory_t *fac = (const clap_plugin_factory_t *)entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  const double vel = 100.0 / 127.0;
  for (uint32_t ri = 0; ri < OMX_ORACLE_FULL_RATES_COUNT; ri++) {
    const double rate = RATES[ri];
    const clap_plugin_t *p = fac->create_plugin(fac, &HOST, OMX_TEST_SINE_VOICE_CLAP_ID);
    if (!p || !p->init(p) || !p->activate(p, rate, 1, BLOCK) || !p->start_processing(p)) {
      CHECK(false, "%.0f Hz: the plugin is created and activated", rate);
      continue;
    }
    const int64_t len = (int64_t)(rate * 0.3);
    const int64_t A = (int64_t)frames_of(OMX_TEST_SINE_VOICE_ATTACK_MS, rate), R = (int64_t)frames_of(OMX_TEST_SINE_VOICE_RELEASE_MS, rate);
    float *l = calloc((size_t)len, sizeof *l), *r = calloc((size_t)len, sizeof *r);

    /* note: 69 at velocity 100 */
    {
      const Play play[] = {{0, false, CLAP_EVENT_NOTE_ON, 69, vel, 0}};
      const Note notes[] = {{69, vel, 0, -1}};
      render(p, play, 1, l, r, len);
      const double d = distance(l, r, notes, 1, 0, len, rate);
      CHECK(d <= TOL, "note %.0f Hz: note 69 at velocity 100 is the closed form on both legs (worst %.3g)", rate, d);
      double peak = 0.0;
      uint32_t ups = 0;
      for (int64_t f = A; f < len; f++) {
        if (fabs(l[f]) > peak) peak = fabs(l[f]);
        if (f > A && l[f - 1] < 0.0f && l[f] >= 0.0f) ups++;
      }
      const double level = OMX_TEST_SINE_VOICE_GAIN * vel, secs = (double)(len - A) / rate;
      CHECK(fabs(peak - level) < 1e-3 * level, "note %.0f Hz: the sustain's peak is GAIN * 100/127 = %.6f (%.6f)", rate, level, peak);
      CHECK(fabs(ups - 440.0 * secs) <= 1.0, "note %.0f Hz: %u upward zero crossings in %.4f s: 440 Hz", rate, ups, secs);
      p->reset(p);
    }
    /* partials: 69 (CLAP) and 76 (MIDI) */
    {
      const Play play[] = {{0, false, CLAP_EVENT_NOTE_ON, 69, vel, 0}, {37, true, 0, 76, 64, 0x90}};
      const Note notes[] = {{69, vel, 0, -1}, {76, 64.0 / 127.0, 37, -1}};
      render(p, play, 2, l, r, len);
      const double d = distance(l, r, notes, 2, 0, len, rate);
      CHECK(d <= TOL, "partials %.0f Hz: note 69 and MIDI note 76 at frame 37 are the sum of two closed forms (worst %.3g)", rate, d);
      p->reset(p);
    }
    /* release: a CLAP note off mid-block */
    {
      const int64_t off = (int64_t)(rate * 0.1) + 100;
      const Play play[] = {{0, false, CLAP_EVENT_NOTE_ON, 69, vel, 0}, {off, false, CLAP_EVENT_NOTE_OFF, 69, 0.0, 0}};
      const Note notes[] = {{69, vel, 0, off}};
      render(p, play, 2, l, r, len);
      const double d = distance(l, r, notes, 1, 0, len, rate);
      bool silent = true;
      for (int64_t f = off + R; f < len; f++) silent = silent && l[f] == 0.0f && r[f] == 0.0f;
      bool sounding = false;
      for (int64_t f = off + R - 8; f < off + R; f++) sounding = sounding || l[f] != 0.0f;
      CHECK(d <= TOL, "release %.0f Hz: the release is the closed form (worst %.3g)", rate, d);
      CHECK(silent && sounding, "release %.0f Hz: silence from %lld frames after the note off on, not before", rate, (long long)R);
      CHECK(ends_n == 1 && ends[0] == off + R && ends_key[0] == 69, "release %.0f Hz: NOTE_END for 69 at frame %lld (%u ends, first at %lld)",
            rate, (long long)(off + R), ends_n, ends_n ? (long long)ends[0] : -1LL);
      p->reset(p);
    }
    /* velocity0: a MIDI note on of velocity 0 is a note off */
    {
      const int64_t off = (int64_t)(rate * 0.05) + 3;
      const Play play[] = {{0, true, 0, 69, 100, 0x90}, {off, true, 0, 69, 0, 0x90}};
      const Note notes[] = {{69, vel, 0, off}};
      render(p, play, 2, l, r, len);
      const double d = distance(l, r, notes, 1, 0, len, rate);
      CHECK(d <= TOL && ends_n == 1 && ends[0] == off + R, "velocity0 %.0f Hz: MIDI 90 45 00 releases the note (worst %.3g, %u ends)", rate,
            d, ends_n);
      p->reset(p);
    }
    /* voices: the declared capacity, and the oldest voice taken past it */
    {
      const clap_plugin_voice_info_t *vi = (const clap_plugin_voice_info_t *)p->get_extension(p, CLAP_EXT_VOICE_INFO);
      clap_voice_info_t info = {0};
      CHECK(vi && vi->get(p, &info) && info.voice_capacity == VOICES && info.voice_count == VOICES,
            "voices %.0f Hz: voice-info reports %u voices (%u)", rate, VOICES, info.voice_capacity);
      Play play[VOICES + 1];
      for (uint32_t i = 0; i <= VOICES; i++) play[i] = (Play){(int64_t)i * 10, false, CLAP_EVENT_NOTE_ON, 40 + (int)i, vel, 0};
      render(p, play, VOICES + 1, l, r, (int64_t)VOICES * 10 + BLOCK);
      CHECK(ends_n == 1 && ends_key[0] == 40 && ends[0] == (int64_t)VOICES * 10,
            "voices %.0f Hz: note %u ends the oldest voice (40) at its frame (%u ends, key %d)", rate, VOICES + 1, ends_n,
            ends_n ? ends_key[0] : -1);
      p->reset(p);
    }
    (void)A;
    free(l), free(r);
    p->stop_processing(p);
    p->deactivate(p);
    p->destroy(p);
  }
  entry->deinit();
  printf("sine-voice-oracle: %u rates, %d failures\n", (unsigned)OMX_ORACLE_FULL_RATES_COUNT, failures);
  return failures > 100 ? 100 : failures;
}
