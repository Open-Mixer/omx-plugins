// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * note-echo-oracle.c — omx test-note-echo through the BUILT .clap, at the nine rates an audio interface
 * clocks at (OMX_ORACLE_FULL_RATES, 32 to 192 kHz), each in its own block size.
 *
 *   note-echo-oracle <omx-test-note-echo.clap>
 *
 * Arms, at every rate:
 *   echo   a stream of CLAP note ons, offs and a choke and MIDI note ons and offs, at frames spread over
 *          the blocks (block edges included): exactly one note out per note in, the same event byte for
 *          byte (key, velocity, channel, note id) at the same absolute frame, in order
 *   other  a non-note event (a MIDI control change, a CLAP parameter value) is not echoed
 *   audio  the audio output is silence (every word written, all zero)
 *   ports  one note input and one note output, each in the CLAP and MIDI dialects
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

#include "omx_test_note_echo_decl.h"

#define MAXEV 256u
#define MAXBLOCK 2048u

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

typedef union {
  clap_event_header_t h;
  clap_event_note_t note;
  clap_event_midi_t midi;
  clap_event_param_value_t param;
} Ev;

/* The stream, absolute frames; and what came out, absolute frames. */
static Ev play[MAXEV];
static int64_t play_at[MAXEV];
static bool play_note[MAXEV];
static uint32_t np;
static Ev got[MAXEV];
static int64_t got_at[MAXEV];
static uint32_t ng;

static Ev blk[MAXEV];
static uint32_t nb;
static int64_t block_at;

static uint32_t ev_size(const clap_input_events_t *l) {
  (void)l;
  return nb;
}
static const clap_event_header_t *ev_get(const clap_input_events_t *l, uint32_t i) {
  (void)l;
  return &blk[i].h;
}
static bool ev_push(const clap_output_events_t *l, const clap_event_header_t *e) {
  (void)l;
  if (ng >= MAXEV || e->size > sizeof(Ev)) return false;
  memcpy(&got[ng], e, e->size);
  got_at[ng++] = block_at + e->time;
  return true;
}
static const clap_input_events_t IN = {NULL, ev_size, ev_get};
static const clap_output_events_t OUT = {NULL, ev_push};
static const void *host_ext(const clap_host_t *h, const char *id) {
  (void)h, (void)id;
  return NULL;
}
static void host_noop(const clap_host_t *h) { (void)h; }
static const clap_host_t HOST = {CLAP_VERSION_INIT, NULL, "note-echo-oracle", "openmixer", "", "0.1", host_ext, host_noop, host_noop, host_noop};

static void add_note(int64_t at, uint16_t type, int32_t id, int16_t ch, int16_t key, double vel) {
  Ev *e = &play[np];
  memset(e, 0, sizeof *e);
  e->note.header = (clap_event_header_t){sizeof e->note, 0, CLAP_CORE_EVENT_SPACE_ID, type, 0};
  e->note.note_id = id, e->note.port_index = 0, e->note.channel = ch, e->note.key = key, e->note.velocity = vel;
  play_at[np] = at, play_note[np++] = true;
}
static void add_midi(int64_t at, uint8_t s, uint8_t d1, uint8_t d2, bool note) {
  Ev *e = &play[np];
  memset(e, 0, sizeof *e);
  e->midi.header = (clap_event_header_t){sizeof e->midi, 0, CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_MIDI, 0};
  e->midi.port_index = 0, e->midi.data[0] = s, e->midi.data[1] = d1, e->midi.data[2] = d2;
  play_at[np] = at, play_note[np++] = note;
}
static void add_param(int64_t at) {
  Ev *e = &play[np];
  memset(e, 0, sizeof *e);
  e->param.header = (clap_event_header_t){sizeof e->param, 0, CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_PARAM_VALUE, 0};
  e->param.param_id = 0, e->param.note_id = -1, e->param.port_index = -1, e->param.channel = -1, e->param.key = -1, e->param.value = 0.5;
  play_at[np] = at, play_note[np++] = false;
}

int main(int argc, char **argv) {
  if (argc != 2) return fprintf(stderr, "usage: note-echo-oracle <omx-test-note-echo.clap>\n"), 2;
  void *lib = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
  if (!lib) return fprintf(stderr, "FAIL dlopen %s: %s\n", argv[1], dlerror()), 1;
  const clap_plugin_entry_t *entry = (const clap_plugin_entry_t *)dlsym(lib, "clap_entry");
  if (!entry || !entry->init(argv[1])) return fprintf(stderr, "FAIL no clap_entry in %s\n", argv[1]), 1;
  const clap_plugin_factory_t *fac = (const clap_plugin_factory_t *)entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
  static float out[OMX_TEST_NOTE_ECHO_OUTPUT_CHANNELS][MAXBLOCK];
  for (uint32_t ri = 0; ri < OMX_ORACLE_FULL_RATES_COUNT; ri++) {
    const double rate = RATES[ri];
    const uint32_t block = 64u << (ri % 5); /* 64 .. 1024 */
    const clap_plugin_t *p = fac->create_plugin(fac, &HOST, OMX_TEST_NOTE_ECHO_CLAP_ID);
    if (!p || !p->init(p) || !p->activate(p, rate, 1, block) || !p->start_processing(p)) {
      CHECK(false, "%.0f Hz: the plugin is created and activated", rate);
      continue;
    }
    const clap_plugin_note_ports_t *nports = (const clap_plugin_note_ports_t *)p->get_extension(p, CLAP_EXT_NOTE_PORTS);
    clap_note_port_info_t ni = {0}, no = {0};
    const uint32_t both = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI;
    CHECK(nports && nports->count(p, true) == 1 && nports->count(p, false) == 1 && nports->get(p, 0, true, &ni) &&
              nports->get(p, 0, false, &no) && (ni.supported_dialects & both) == both && (no.supported_dialects & both) == both,
          "ports %.0f Hz: one note input and one note output, CLAP and MIDI dialects", rate);

    /* the stream: notes at a rate-scaled spread, edges of blocks included */
    np = 0;
    const int64_t ms = (int64_t)(rate / 1000.0);
    add_note(0, CLAP_EVENT_NOTE_ON, 7, 0, 60, 0.8);
    add_midi(1, 0x91, 64, 100, true);
    add_param(2);
    add_note(block - 1, CLAP_EVENT_NOTE_ON, 8, 3, 67, 0.25);
    add_note(block, CLAP_EVENT_NOTE_OFF, 7, 0, 60, 0.5);
    add_midi(block, 0xB0, 64, 127, false);
    add_midi(3 * ms + 5, 0x81, 64, 33, true);
    add_note(5 * ms, CLAP_EVENT_NOTE_CHOKE, 8, 3, 67, 0.0);
    add_midi(7 * ms + 1, 0x95, 72, 0, true);
    const int64_t len = 8 * ms + block;
    /* in frame order, stable: a block's events reach the plugin sorted, as CLAP requires */
    for (uint32_t i = 1; i < np; i++)
      for (uint32_t j = i; j > 0 && play_at[j - 1] > play_at[j]; j--) {
        const Ev e = play[j];
        const int64_t at = play_at[j];
        const bool nt = play_note[j];
        play[j] = play[j - 1], play_at[j] = play_at[j - 1], play_note[j] = play_note[j - 1];
        play[j - 1] = e, play_at[j - 1] = at, play_note[j - 1] = nt;
      }

    ng = 0;
    bool silent = true;
    for (block_at = 0; block_at < len; block_at += block) {
      nb = 0;
      for (uint32_t i = 0; i < np; i++)
        if (play_at[i] >= block_at && play_at[i] < block_at + block) {
          blk[nb] = play[i];
          blk[nb++].h.time = (uint32_t)(play_at[i] - block_at);
        }
      float *outs[OMX_TEST_NOTE_ECHO_OUTPUT_CHANNELS];
      for (uint32_t c = 0; c < OMX_TEST_NOTE_ECHO_OUTPUT_CHANNELS; c++) {
        outs[c] = out[c];
        for (uint32_t f = 0; f < block; f++) out[c][f] = NAN;
      }
      clap_audio_buffer_t ob = {outs, NULL, OMX_TEST_NOTE_ECHO_OUTPUT_CHANNELS, 0, 0};
      const clap_process_t proc = {block_at, block, NULL, NULL, &ob, 0, 1, &IN, &OUT};
      p->process(p, &proc);
      for (uint32_t c = 0; c < OMX_TEST_NOTE_ECHO_OUTPUT_CHANNELS; c++)
        for (uint32_t f = 0; f < block; f++) silent = silent && out[c][f] == 0.0f && !signbit(out[c][f]);
    }

    uint32_t want = 0, match = 0;
    for (uint32_t i = 0; i < np; i++) {
      if (!play_note[i]) continue;
      if (want < ng && got_at[want] == play_at[i] && got[want].h.size == play[i].h.size &&
          memcmp((const char *)&got[want] + sizeof(clap_event_header_t), (const char *)&play[i] + sizeof(clap_event_header_t),
                 play[i].h.size - sizeof(clap_event_header_t)) == 0 &&
          got[want].h.type == play[i].h.type && got[want].h.space_id == play[i].h.space_id)
        match++;
      want++;
    }
    CHECK(ng >= want && match == want, "echo %.0f Hz (blocks of %u): %u of %u notes out unchanged at their frame", rate, block, match, want);
    bool other = ng == want;
    for (uint32_t k = 0; k < ng; k++) other = other && got[k].h.type != CLAP_EVENT_PARAM_VALUE && !(got[k].h.type == CLAP_EVENT_MIDI && (got[k].midi.data[0] & 0xF0) == 0xB0);
    CHECK(other, "other %.0f Hz: no event but the notes is echoed (%u out, %u notes in)", rate, ng, want);
    CHECK(silent, "audio %.0f Hz: the audio output is silence", rate);

    p->stop_processing(p);
    p->deactivate(p);
    p->destroy(p);
  }
  entry->deinit();
  printf("note-echo-oracle: %u rates, %d failures\n", (unsigned)OMX_ORACLE_FULL_RATES_COUNT, failures);
  return failures > 100 ? 100 : failures;
}
