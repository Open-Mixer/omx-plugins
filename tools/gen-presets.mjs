#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * gen-presets.mjs — the factory presets: presets/<plugin id>/<preset id>.json, one file per preset,
 * in the console's preset format (org.openmixer.preset/1). Every preset names the values it changes;
 * the rest come up at the plugin's declared defaults, so each file carries the whole plugin. A
 * choice is written by its label ('Tube') and the file records both the value and the label.
 * Parameters a preset must not decide are left out: the gate's and the compressor's key source, and the mix of the
 * reverb and the delay, which belong to the place the plugin sits in.
 *
 * Usage: `node tools/gen-presets.mjs [--check]` — `--check` writes nothing and fails on a stale,
 * missing or extra file. The files are installed read only to $(PREFIX)/share/omx/presets.
 *
 * From the same presets, each LV2 bundle's plugins/<stem>/generated/<stem>.lv2/presets.ttl: one
 * pset:Preset per file (docs/presets.md); tools/gen.mjs lists them in the bundle's manifest.
 */
import { existsSync, mkdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';
import { FORMAT, PRESET_DIR, ROOT, plugins, presetFiles, presetsTtl } from './preset-lib.mjs';

/** The date every factory file carries: fixed, so regenerating changes nothing. */
const WHEN = '2026-10-17T00:00:00Z';
/** Parameters a preset leaves to the host: they depend on where the plugin sits. */
const LEFT_OUT = { 'org.openmixer.keyed-gate': ['key_external'], 'org.openmixer.comp': ['keySource'], 'org.openmixer.reverb': ['mix'], 'org.openmixer.delay': ['mix'] };

/** The slots of an EQ: type, freq, gain, q and on of band n, named as the plugin names them. */
const eqBands = (list, style = 'eq8') => Object.fromEntries(list.flatMap(([type, freq, gain, q], i) => {
  const n = i + 1;
  const k = style === 'strip' ? [`eq${n}Type`, `eq${n}Freq`, `eq${n}Gain`, `eq${n}Q`, `eq${n}On`] : [`b${n}_type`, `b${n}_freq`, `b${n}_gain`, `b${n}_q`, `b${n}_on`];
  return [[k[0], type], [k[1], freq], [k[2], gain], [k[3], q], [k[4], 1]];
}));
const bell = (f, g, q = 1) => ['Bell', f, g, q];
const low = (f, g, q = 0.7) => ['Low Shelf', f, g, q];
const high = (f, g, q = 0.7) => ['High Shelf', f, g, q];

const EQ_PRESETS = [
  ['pultec-low', 'Low Shelf Boost and Dip', eqBands([low(60, 5), bell(180, -3, 1.2)])],
  ['pultec-mid', 'Mid Presence', eqBands([bell(500, 3), bell(1500, -2, 1.4), bell(3000, 2)])],
  ['portico-5033', 'Five Band Polish', eqBands([low(100, 2), bell(300, -2, 1.4), bell(1500, 1.5), bell(4000, 2), high(12000, 2)])],
  ['eq-601', 'Gentle Tilt', eqBands([low(80, 1.5), bell(400, -1.5), bell(2500, 2, 0.8), high(10000, 2.5)])],
  ['mustard-eq', 'Warm Presence', eqBands([low(120, 2), bell(500, -2, 1.2), bell(3000, 2), high(10000, 1.5)])],
  ['maxer-low', 'Low Shelf', eqBands([low(90, 3)])],
  ['amp-input', 'Amp Input', eqBands([bell(2500, 4)])],
  ['vocal-cleanup', 'Vocal Cleanup', { hpf_on: 1, hpf_freq: 90, ...eqBands([bell(300, -2.5, 1.2), bell(3500, 2), high(10000, 2)]) }],
];

/** A graphic EQ curve: dB at a frequency, the other faders flat. */
const geq = (points) => ({ points });
const GEQ_PRESETS = [
  ['smile', 'Smile', geq({ 20: 3, 25: 3.5, 31.5: 4, 40: 4, 50: 4, 63: 3.5, 80: 3, 100: 2, 125: 1, 160: 0, 250: -1.5, 315: -2, 400: -2.5, 500: -3, 630: -3, 800: -2.5, 1000: -2, 1250: -1, 1600: 0, 2000: 1, 2500: 2, 3150: 3, 4000: 3.5, 5000: 4, 6300: 4, 8000: 4, 10000: 3.5, 12500: 3, 16000: 2.5, 20000: 2 })],
  ['vocal-presence', 'Vocal Presence', geq({ 160: -2, 200: -2, 250: -2, 315: -1.5, 400: -1, 2000: 1, 2500: 2.5, 3150: 3.5, 4000: 3.5, 5000: 2.5, 6300: 1.5 })],
  ['bass-lift', 'Bass Lift', geq({ 31.5: 3, 40: 4, 50: 5, 63: 5, 80: 4, 100: 3, 125: 2, 160: 1 })],
  ['air', 'Air', geq({ 6300: 1, 8000: 2, 10000: 3, 12500: 4, 16000: 4.5, 20000: 4 })],
  ['telephone', 'Telephone', geq({ 20: -15, 25: -15, 31.5: -15, 40: -15, 50: -15, 63: -15, 80: -15, 100: -15, 125: -12, 160: -9, 200: -6, 250: -3, 1000: 3, 1250: 4, 1600: 4, 2000: 3, 4000: -6, 5000: -9, 6300: -12, 8000: -15, 10000: -15, 12500: -15, 16000: -15, 20000: -15 })],
];

const COMP = (threshold, ratio, attack, release, makeup, kind = 'Compressor', knee) => ({ thresholdDb: threshold, ratio, attackMs: attack, releaseMs: release, makeupDb: makeup, kind, ...(knee === undefined ? {} : { kneeDb: knee }) });
const GATE = (threshold, range, kneeStart, kneeEnd, attack, hold, release, hysteresis, ratio) => ({ threshold, range, knee_start: kneeStart, knee_end: kneeEnd, attack, hold, release, hysteresis, ratio });
const REVERB = (algorithm, size, damping, predelay, width, lowcut, highcut) => ({ algorithm, size, damping, predelay, width, lowcut, highcut });
/** One dynamic band n of omx-dyneq, on, cutting while it is loud: its shape, centre, Q and dynamics. */
const DYNEQ = (n, type, freq, q, threshold, ratio, range, attack, release) => ({
  [`bandOn${n}`]: 1, [`bandType${n}`]: type, [`freq${n}`]: freq, [`q${n}`]: q, [`dynMode${n}`]: 'Down',
  [`threshold${n}`]: threshold, [`ratio${n}`]: ratio, [`range${n}`]: range, [`attack${n}`]: attack, [`release${n}`]: release,
});
/** omx-multiband over `bands` bands, each band's threshold, one ratio for all. */
const MBAND = (bands, thresholds, ratio) => ({
  bands, ...Object.fromEntries(thresholds.flatMap((t, k) => [[`thresholdDb${k + 1}`, t], [`ratio${k + 1}`, ratio]])),
});

/** plugin id -> [[preset id, name, values]]. A value of a choice is its label. */
const DEFINITIONS = {
  'org.openmixer.chorus': [
    ['stereo-chorus', 'Stereo Chorus', { voices: 2, rate: 0.8, depth: 3 }],
    ['lush-chorus', 'Lush Chorus', { voices: 4 }],
    ['dimension', 'Dimension', { voices: 2, spread: 0.5, mix: 100 }],
    ['slow-wide', 'Slow and Wide', { voices: 3, rate: 0.2, depth: 8, spread: 0.25, mix: 50 }],
  ],
  'org.openmixer.flanger': [
    ['stereo-flanger', 'Stereo Flanger', {}],
    ['jet-flanger', 'Jet Flanger', { feedback: 0.8 }],
    ['negative-sweep', 'Negative Sweep', { rate: 0.1, depth: 4, feedback: -0.6 }],
  ],
  'org.openmixer.drive': [
    ['tube-stage', 'Tube Stage', { amount: 12, curve: 'Tube', character: 0.4 }],
    ['digi-tube', 'Digital Tube', { amount: 9, curve: 'Tube', character: 0.25 }],
    ['tape-deck', 'Tape Deck', { amount: 8, curve: 'Tape', character: 0.2 }],
    ['saturator', 'Saturator', { amount: 6, curve: 'Soft' }],
    ['pre-a', 'Preamp Light', { amount: 3, curve: 'Soft', character: 0.1 }],
    ['pre-b', 'Preamp Dark', { amount: 4.5, curve: 'Soft', character: -0.2 }],
    ['exciter-air', 'Exciter Air', { amount: 12, curve: 'Exciter', band: 'High', bandFreq: 5000, mix: 20, hfRolloff: '16 kHz' }],
    ['exciter-presence', 'Exciter Presence', { amount: 9, curve: 'Exciter', band: 'High', bandFreq: 2500, mix: 15, hfRolloff: '12 kHz' }],
    ['bass-enhancer', 'Bass Enhancer', { amount: 12, curve: 'Exciter', character: 0.3, band: 'Low', bandFreq: 120, mix: 25 }],
    ['bus-colour', 'Bus Colour', { amount: 3, curve: 'Tape' }],
    ['amp-tube', 'Amp Tube', { amount: 18, curve: 'Tube', character: 0.6, band: 'Full', hfRolloff: '12 kHz' }],
  ],
  'org.openmixer.delay': [
    ['stereo-delay', 'Stereo Delay', { fxDelayTime: 375, fxDelayFeedback: 0.35, tone: 0.5 }],
    ['ping-pong', 'Ping-pong', { fxDelayTime: 375, fxDelayFeedback: 0.35, tone: 0.5, pingpong: 1 }],
    ['slapback', 'Slapback', { fxDelayTime: 90, fxDelayFeedback: 0.1, tone: 0.6 }],
    ['dub-echo', 'Dub Echo', { fxDelayTime: 500, fxDelayFeedback: 0.7, tone: 0.15, pingpong: 1 }],
  ],
  'org.openmixer.comp': [
    ['vocal', 'Vocal', COMP(-18, 3, 5, 120, 4)],
    ['kick', 'Kick', COMP(-14, 4, 8, 90, 3)],
    ['snare', 'Snare', COMP(-12, 4, 3, 80, 3)],
    ['bass', 'Bass', COMP(-16, 3.5, 15, 150, 3)],
    ['bus-glue', 'Bus Glue', COMP(-10, 2, 20, 250, 1.5)],
    ['limiter-ish', 'Limiter', COMP(-3, 20, 0.1, 60, 0, 'Limiter')],
    ['fet-4', 'FET 4:1', COMP(-20, 4, 1, 50, 4, 'Limiter')],
    ['fet-20', 'FET 20:1', COMP(-12, 20, 1, 50, 2, 'Limiter')],
    ['comp-276', 'Peak 6:1', COMP(-16, 6, 10, 150, 3, 'Limiter')],
    ['rms-160', 'RMS 4:1', COMP(-20, 4, 15, 100, 3)],
    ['rms-160-crush', 'RMS Crush', COMP(-24, 8, 10, 80, 5)],
    ['mustard-a', 'Fast Peak', COMP(-18, 4, 3, 100, 3, 'Limiter')],
    ['mustard-b', 'Hard Peak', COMP(-10, 10, 1, 200, 2, 'Limiter')],
    ['mustard-c', 'Slow RMS', COMP(-22, 2.5, 20, 300, 3)],
    ['mustard-d', 'Firm RMS', COMP(-14, 6, 8, 150, 4)],
    // the bus-glue shape at 4:1 with the detector on the output: the feedback loop of a VCA bus compressor
    ['ssl-bus-glue', 'Bus Glue 4:1', { ...COMP(-10, 4, 30, 300, 1.5), detectorTopology: 'Feedback' }],
    // the ducker (openmixer ducker spec §3 item 4): ratio 20 held at a -12 dB range, a fast duck and a slow return;
    // the key is the host's (keySource is left out), and the threshold and the rest come up at their defaults
    ['ducker', 'Ducker', { ratio: 20, attackMs: 1, releaseMs: 300, duck: 1, rangeDb: -12 }],
  ],
  // the card's uses (the remaining-FX wave, FX-1): a vocal harsh only when pushed, proximity boom, a
  // monitor ring that appears at level; one band each, cutting while the band is loud
  'org.openmixer.dyneq': [
    ['harsh-vocal', 'Harsh Vocal', DYNEQ(4, 'Bell', 3500, 1.4, -30, 3, -6, 5, 120)],
    ['proximity', 'Proximity', DYNEQ(1, 'Low Shelf', 150, 0.7, -24, 2.5, -6, 10, 200)],
    ['monitor-ring', 'Monitor Ring', DYNEQ(3, 'Bell', 2500, 4, -24, 4, -9, 2, 80)],
  ],
  // the card's uses (FX-3): in-ear mixes, a vocal whose low mid swells, a playback bus
  'org.openmixer.multiband': [
    ['iem-control', 'IEM Control', MBAND(3, [-24, -24, -24], 2.5)],
    ['vocal-low-mid', 'Vocal Low Mid', { ...MBAND(3, [-30, -18, -18], 3), xoverHz1: 250, xoverHz2: 2500 }],
    ['playback-bus', 'Playback Bus', MBAND(4, [-20, -20, -20, -20], 2)],
  ],
  'org.openmixer.deesser': [
    ['vocal-s', 'Vocal S', { deessFreq: 6000, deessMode: 'Split' }],
    ['broad-s', 'Broadband S', { deessMode: 'Wideband' }],
  ],
  'org.openmixer.keyed-gate': [
    ['kick', 'Kick', GATE(-35, -60, -38, -30, 0.5, 30, 80, 4, 20)],
    ['snare', 'Snare', GATE(-38, -50, -41, -33, 0.5, 50, 120, 5, 16)],
    ['toms', 'Toms', GATE(-32, -70, -35, -25, 0.5, 150, 250, 6, 20)],
    ['hi-hat-spill', 'Hi-hat Spill', GATE(-45, -20, -48, -40, 2, 5, 150, 3, 3)],
    ['vocal-noise', 'Vocal Noise', GATE(-50, -18, -55, -45, 5, 200, 300, 2, 4)],
    ['drum-room', 'Drum Room', GATE(-42, -12, -46, -36, 10, 300, 600, 4, 2)],
    ['expander', 'Expander', { ratio: 2, range: -20 }],
    ['gentle-expander', 'Gentle Expander', GATE(-50, -10, -56, -44, 5, 20, 250, 3, 2)],
  ],
  'org.openmixer.reverb': [
    ['small-room', 'Small Room', REVERB('Room', 0.35, 0.45, 8, 0.9, 120, 8000)],
    ['vocal-plate', 'Vocal Plate', REVERB('Plate', 0.6, 0.35, 20, 1, 200, 12000)],
    ['concert-hall', 'Concert Hall', REVERB('Hall', 0.85, 0.5, 30, 1, 80, 9000)],
    ['chamber', 'Chamber', REVERB('Hall', 0.5, 0.65, 12, 0.8, 150, 7000)],
    ['reverse-swell', 'Reverse Swell', REVERB('Reverse', 0.7, 0.3, 0, 1, 100, 12000)],
    ['gated-snare', 'Gated Snare', REVERB('Gated', 0.6, 0.35, 0, 1, 150, 10000)],
    ['ambience', 'Ambience', REVERB('Room', 0.2, 0.3, 0, 1, 150, 14000)],
    ['bright-room', 'Bright Room', REVERB('Room', 0.4, 0.35, 6, 1, 90, 11000)],
    ['studio-room', 'Studio Room', REVERB('Room', 0.45, 0.4, 10, 0.9, 100, 10000)],
    ['vintage-room', 'Vintage Room', REVERB('Room', 0.5, 0.2, 10, 1, 100, 12000)],
    ['short-plate', 'Short Plate', REVERB('Plate', 0.1, 0.7, 5, 0.8, 150, 9000)],
    ['vintage-plate', 'Vintage Plate', REVERB('Plate', 0.33, 0.2, 0, 1, 80, 10000)],
    ['bright-plate', 'Bright Plate', REVERB('Plate', 0.5, 0.3, 15, 1, 150, 11000)],
    ['rich-plate', 'Rich Plate', REVERB('Plate', 0.8, 0.3, 25, 1, 120, 12000)],
    ['bright-hall', 'Bright Hall', REVERB('Hall', 0.6, 0.4, 20, 1, 80, 11000)],
    ['studio-hall', 'Studio Hall', REVERB('Hall', 0.7, 0.45, 25, 1, 100, 10000)],
  ],
  'org.openmixer.limiter': [
    ['master', 'Master', { ceilingDb: -1 }],
    ['streaming', 'Streaming', { ceilingDb: -1, lookaheadMs: 2, releaseMs: 100 }],
    ['loud', 'Loud', { ceilingDb: -0.3, lookaheadMs: 1, releaseMs: 30 }],
    ['gentle', 'Gentle', { ceilingDb: -2, lookaheadMs: 3, releaseMs: 200 }],
  ],
  'org.openmixer.phaser': [
    ['slow-sweep', 'Slow Sweep', { rateHz: 0.15, depthOct: 4, stages: 6, feedback: 0.4 }],
    ['four-stage', 'Four Stage', { rateHz: 0.5, stages: 4, feedback: 0.2 }],
    ['deep-jet', 'Deep Jet', { rateHz: 0.3, depthOct: 5, stages: 10, feedback: 0.8 }],
    ['gentle-shimmer', 'Gentle Shimmer', { rateHz: 1.2, baseHz: 400, depthOct: 2, stages: 4, feedback: 0, mix: 35 }],
  ],
  'org.openmixer.pitch': [
    ['octave-up', 'Octave Up', { semitones: 12, mix: 50 }],
    ['octave-down', 'Octave Down', { semitones: -12, mix: 50 }],
    ['fifth-up', 'Fifth Up', { semitones: 7, mix: 40 }],
    ['detune-thicken', 'Detune Thicken', { cents: 12, mix: 50 }],
  ],
  'org.openmixer.rotary': [
    ['slow', 'Slow', { speed: 'Slow' }],
    ['fast', 'Fast', { speed: 'Fast' }],
    ['stopped', 'Stopped', { speed: 'Stop' }],
    ['quick-accel', 'Quick Acceleration', { speed: 'Fast', accel: 3 }],
  ],
  'org.openmixer.transient': [
    ['punch', 'Punch', { attackDb: 8, sustainDb: -4 }],
    ['tighten', 'Tighten', { sustainDb: -9 }],
    ['sustain-lift', 'Sustain Lift', { sustainDb: 8, sustainTimeMs: 400 }],
    ['soften', 'Soften', { attackDb: -8, attackTimeMs: 20 }],
  ],
  'org.openmixer.suboct': [
    ['bass-octave', 'Bass Octave', { range: 'Low', oct1: 100, oct2: 0, dry: 100 }],
    ['deep-sub', 'Deep Sub', { range: 'Low', oct1: 60, oct2: 70, dry: 80 }],
    ['guitar-octave', 'Guitar Octave', { range: 'Mid', oct1: 70, oct2: 0, dry: 100 }],
    ['sub-only', 'Sub Only', { range: 'Low', oct1: 100, oct2: 0, dry: 0 }],
  ],
  'org.openmixer.plosive': [
    ['spoken-voice', 'Spoken Voice', { consonantDb: 6, popDb: 12 }],
    ['close-mic', 'Close Mic', { popDb: 18, popHz: 180, consonantDb: 8 }],
    ['gentle', 'Gentle', { popDb: 6, consonantDb: 3, popSensitivityDb: 14 }],
  ],
  'org.openmixer.clipper': [
    ['soft-clip', 'Soft Clip', { driveDb: 6, ceilingDb: -1, softnessPct: 60 }],
    ['hard-ceiling', 'Hard Ceiling', { driveDb: 3, ceilingDb: -0.3, softnessPct: 0 }],
    ['loud-bus', 'Loud Bus', { driveDb: 9, ceilingDb: -2, softnessPct: 30 }],
  ],
  'org.openmixer.doubler': [
    ['vocal-double', 'Vocal Double', { voices: 2, delayMs: 12, detuneCents: 8, spreadPct: 60, mixPct: 50 }],
    ['tight-adt', 'Tight Double', { voices: 1, delayMs: 9, detuneCents: 4, modDepthMs: 0.3, spreadPct: 30, mixPct: 45 }],
    ['wide-backing', 'Wide Backing Vocals', { voices: 4, delayMs: 18, detuneCents: 12, modDepthMs: 1, spreadPct: 100, mixPct: 60 }],
    ['guitar-double', 'Guitar Double', { voices: 2, delayMs: 25, detuneCents: 6, modRateHz: 0.3, spreadPct: 100, mixPct: 50 }],
  ],
  'org.openmixer.tone': [
    ['warmer', 'Warmer', { tiltDb: -3 }],
    ['brighter', 'Brighter', { tiltDb: 3 }],
    ['low-pass-sweep', 'Low-Pass Sweep', { filter: -50, resonance: 1.5 }],
    ['high-pass-sweep', 'High-Pass Sweep', { filter: 50, resonance: 1.5 }],
  ],
  'org.openmixer.ducker': [
    ['voice-over-music', 'Voice Over Music', { thresholdDb: -30, rangeDb: -12, ratio: 20, attackMs: 5, holdMs: 200, releaseMs: 500 }],
    ['talkback-duck', 'Talkback Duck', { thresholdDb: -35, rangeDb: -20, ratio: 20, attackMs: 1, holdMs: 100, releaseMs: 300 }],
    ['gentle-bed', 'Gentle Bed Duck', { thresholdDb: -30, rangeDb: -6, ratio: 4, attackMs: 20, holdMs: 300, releaseMs: 1000 }],
    ['bass-under-kick', 'Bass Under Kick', { thresholdDb: -20, rangeDb: -6, ratio: 20, attackMs: 1, releaseMs: 80, keyLpHz: 150 }],
  ],
  'org.openmixer.upward': [
    ['room-lift', 'Room Lift', { thresholdDb: -40, ratio: 2, rangeDb: 6 }],
    ['ambience', 'Ambience Up', { thresholdDb: -50, ratio: 3, rangeDb: 12, releaseMs: 400 }],
    ['detail', 'Detail', { thresholdDb: -30, ratio: 1.6, rangeDb: 4, attackMs: 10 }],
  ],
  'org.openmixer.glue': [
    ['mix-bus', 'Mix Bus', { thresholdDb: -12, ratio: '2:1', attack: '30 ms', release: 'Auto', makeupDb: 2 }],
    ['drum-bus', 'Drum Bus', { thresholdDb: -14, ratio: '4:1', attack: '10 ms', release: '0.3 s', makeupDb: 3, keyHpHz: 80 }],
    ['parallel', 'Parallel Squash', { thresholdDb: -24, ratio: '10:1', attack: '3 ms', release: '0.1 s', makeupDb: 6, mixPct: 40 }],
    // the VCA bus compressor's front panel at its public default (4:1, fastest attack, auto release), the detector
    // on the output; threshold and make-up are desk judgement
    ['vca-bus', 'VCA Bus 4:1', { thresholdDb: -10, ratio: '4:1', attack: '0.1 ms', release: 'Auto', makeupDb: 2, detectorTopology: 'Feedback' }],
  ],
  'org.openmixer.loudlevel': [
    ['speech', 'Speech', { targetLufs: -20, rangeDb: 9, speedDbPerS: 1.5, gateLufs: -50, window: 'Short-term' }],
    ['programme', 'Programme', { targetLufs: -23, rangeDb: 6, speedDbPerS: 0.8, gateLufs: -55, window: 'Short-term' }],
    ['fast-ride', 'Fast Ride', { targetLufs: -18, rangeDb: 12, speedDbPerS: 3, window: 'Momentary' }],
  ],
  'org.openmixer.leveler': [
    ['vocal-ride', 'Vocal Ride', { amount: 60, direction: 'Both', speed: 'Medium' }],
    ['dialogue-down', 'Dialogue Down', { amount: 75, direction: 'Down', speed: 'Slow' }],
    ['bring-up-quiet', 'Bring Up Quiet', { amount: 50, direction: 'Up', speed: 'Fast' }],
  ],
  'org.openmixer.balancer': [
    ['gentle', 'Gentle', { amountPct: 40, rangeDb: 3, speedDbPerS: 0.5, slopeDbOct: -3 }],
    ['full-mix', 'Full Mix', { amountPct: 70, rangeDb: 6, speedDbPerS: 1, slopeDbOct: -3 }],
    ['brighter', 'Brighter Target', { amountPct: 50, rangeDb: 4, slopeDbOct: -2 }],
  ],
  'org.openmixer.spring': [
    ['guitar-amp', 'Guitar Amp', { decayS: 2, lengthMs: 35, tensionPct: 50, dampHz: 4000, mixPct: 25 }],
    ['long-drip', 'Long Drip', { decayS: 4, lengthMs: 50, tensionPct: 70, dampHz: 3000, mixPct: 35 }],
    ['tight', 'Tight Spring', { decayS: 1, lengthMs: 25, tensionPct: 30, dampHz: 6000, mixPct: 20 }],
  ],
  'org.openmixer.tremolo': [
    ['classic', 'Classic', { rateHz: 5, depth: 60 }],
    ['slow-swell', 'Slow Swell', { rateHz: 0.8, depth: 80 }],
    ['auto-pan', 'Auto-pan', { rateHz: 1, depth: 80, mode: 'Pan' }],
    ['helicopter', 'Helicopter', { rateHz: 14, depth: 100 }],
  ],
  'org.openmixer.eq8': EQ_PRESETS,
  'org.openmixer.eq16': EQ_PRESETS,
  'org.openmixer.eq32': EQ_PRESETS,
  'org.openmixer.geq': GEQ_PRESETS,
  'org.openmixer.strip': [
    ['vocal', 'Vocal', { hpfOn: 1, hpfFreq: 90, eqOn: 1, ...eqBands([bell(300, -2.5, 1.2), bell(3500, 2), high(10000, 2)], 'strip'), compOn: 1, ...Object.fromEntries(Object.entries(COMP(-18, 3, 5, 120, 4)).map(([k, v]) => [`comp${k[0].toUpperCase()}${k.slice(1).replace(/Db$|Ms$/, '')}`, v])) }],
    ['kick', 'Kick', { hpfOn: 1, hpfFreq: 30, gateOn: 1, ...Object.fromEntries(Object.entries(GATE(-35, -60, -38, -30, 0.5, 30, 80, 4, 20)).map(([k, v]) => [`gate${k[0].toUpperCase()}${k.slice(1).replace(/_(.)/g, (_, c) => c.toUpperCase())}`, v])), eqOn: 1, ...eqBands([low(60, 3), bell(350, -4, 1.4), bell(4000, 3)], 'strip'), compOn: 1, compThreshold: -14, compRatio: 4, compAttack: 8, compRelease: 90, compMakeup: 3 }],
    ['bass', 'Bass', { hpfOn: 1, hpfFreq: 35, eqOn: 1, ...eqBands([low(80, 2), bell(800, -2, 1.2), bell(2500, 2)], 'strip'), compOn: 1, compThreshold: -16, compRatio: 3.5, compAttack: 15, compRelease: 150, compMakeup: 3 }],
  ],
};

/** The values of one preset as the file lists them: every parameter, the plugin's order. */
function paramsOf(plugin, values) {
  const out = [];
  const named = new Set(Object.keys(values));
  for (const s of named) if (!plugin.params.some((p) => p.symbol === s)) throw new Error(`${plugin.id}: '${s}' is not a parameter`);
  const skip = LEFT_OUT[plugin.id] ?? [];
  for (const d of plugin.params) {
    if (skip.includes(d.symbol) && !named.has(d.symbol)) continue;
    let v = named.has(d.symbol) ? values[d.symbol] : d.def;
    const labels = plugin.labels.get(d.symbol);
    if (typeof v === 'string') {
      const hit = labels && [...labels].find(([, l]) => l === v);
      if (!hit) throw new Error(`${plugin.id}: '${d.symbol}' has no choice '${v}'`);
      v = hit[0];
    }
    const row = { symbol: d.symbol, unit: d.unit, value: v };
    if (labels?.has(v)) row.label = labels.get(v);
    out.push(row);
  }
  return out;
}

/** The values of a graphic EQ preset: the named bands at their dB, the rest flat. */
function geqValues(plugin, curve) {
  const hz = (name) => parseFloat(name) * (/kHz/.test(name) ? 1000 : 1);
  const values = {};
  const used = new Set();
  for (const d of plugin.params) {
    const f = hz(d.name);
    const key = Object.keys(curve.points).find((k) => Math.abs(Number(k) - f) < 1e-6);
    if (key !== undefined) { values[d.symbol] = curve.points[key]; used.add(key); }
  }
  for (const k of Object.keys(curve.points)) if (!used.has(k)) throw new Error(`${plugin.id}: no fader at ${k} Hz`);
  return values;
}

/** path -> content, for every preset. */
export function build() {
  const files = new Map();
  const shipped = plugins();
  for (const [id, list] of Object.entries(DEFINITIONS)) {
    const plugin = shipped.find((p) => p.id === id);
    if (!plugin) throw new Error(`no shipped plugin ${id}`);
    const presets = [];
    for (const [pid, name, def] of list) {
      const values = def.points ? geqValues(plugin, def) : def;
      const preset = { format: FORMAT, plugin: { id }, id: pid, name, carries: 'params', params: paramsOf(plugin, values), createdAt: WHEN, modifiedAt: WHEN, copyOf: null };
      files.set(`${PRESET_DIR}/${id}/${pid}.json`, `${JSON.stringify(preset, null, 2)}\n`);
      presets.push(preset);
    }
    presets.sort((a, b) => (`${a.id}.json` < `${b.id}.json` ? -1 : 1));
    files.set(`plugins/${plugin.stem}/generated/${plugin.stem}.lv2/presets.ttl`, presetsTtl(plugin, presets));
  }
  const missing = shipped.filter((p) => !(p.id in DEFINITIONS)).map((p) => p.id);
  if (missing.length) throw new Error(`no factory presets for ${missing.join(', ')}`);
  return files;
}

if (import.meta.url === `file://${process.argv[1]}`) {
  const check = process.argv.includes('--check');
  const want = build();
  const have = new Set(presetFiles().map((f) => f.file));
  const stale = [];
  for (const [path, text] of want) {
    const full = join(ROOT, path);
    const same = existsSync(full) && readFileSync(full, 'utf8') === text;
    if (same) { have.delete(path); continue; }
    have.delete(path);
    stale.push(path);
    if (!check) { mkdirSync(join(full, '..'), { recursive: true }); writeFileSync(full, text); }
  }
  const extra = [...have];
  if (!check) for (const e of extra) rmSync(join(ROOT, e));
  if (check && (stale.length || extra.length)) {
    for (const s of stale) console.error(`gen-presets: ${s} is stale or missing`);
    for (const e of extra) console.error(`gen-presets: ${e} is not a declared preset`);
    process.exit(1);
  }
  const json = [...want.keys()].filter((p) => p.startsWith(`${PRESET_DIR}/`));
  console.log(`gen-presets: ${json.length} presets for ${new Set(json.map((p) => p.split('/')[1])).size} plugins${check ? ' are fresh' : `, ${stale.length} written, ${extra.length} removed`}`);
}
