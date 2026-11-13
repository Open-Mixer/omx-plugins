<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com> -->
# omx-plugins: the plugins

Every plugin, what it does, and its controls; the parameter pages, the factory presets, the packages and the static library.

## The plugins

<!-- BEGIN GENERATED catalogue: tools/gen.mjs, from each shipped plugin's declaration -->

| Plugin | CLAP id | LV2 URI | What it does |
|---|---|---|---|
| **omx balancer** | `org.openmixer.balancer` | `urn:openmixer:balancer` | Walks four bands slowly toward a target tone, so a mix settles toward a pink-noise balance without chasing notes. |
| **omx chorus** | `org.openmixer.chorus` | `urn:openmixer:chorus` | The console's chorus: up to four voices read one modulated delay line, the right leg's sweep offset by the spread, mixed with the dry signal. |
| **omx clipper** | `org.openmixer.clipper` | `urn:openmixer:clipper` | Clips the peaks at a ceiling, with drive in front and an adjustable softness of the bend. |
| **omx comp** | `org.openmixer.comp` | `urn:openmixer:comp` | The console's compressor: threshold, ratio and knee set how hard it holds the level down, attack and release how fast, with makeup gain and a dry/wet mix. Kind picks the RMS detector (Compressor) or the peak detector (Limiter), and detector oversampling can run the control path at 4x; detector topology has it hear the input (feedforward) or its own output (feedback). Its detector can listen to a key routed to its sidechain input instead, through a high-pass, a low-pass and a tilt, on the louder leg, the mid or the side, with a hold; key listen puts what the detector hears on the output, and duck holds the reduction to a set depth. The DSP is omx-dsp's <omxdsp/fx/omx_comp_instance.h>, the console's own compressor. |
| **omx deesser** | `org.openmixer.deesser` | `urn:openmixer:deesser` | The console's de-esser: a detector on a band around the sibilance drives the compressor's gain computer, and the reduction lands on that band alone or on the whole signal. |
| **omx delay** | `org.openmixer.delay` | `urn:openmixer:delay` | Stereo delay up to 2000 ms, free or synced to the host's tempo: feedback through a tone filter that darkens each repeat, ping-pong, up to four read taps, a tape or analog colour, wet/dry mix and a duck under the input. Zero latency, real-time safe. |
| **omx doubler** | `org.openmixer.doubler` | `urn:openmixer:doubler` | Doubles a voice or an instrument with up to four slightly delayed, detuned and drifting copies spread across the stereo field, the double-tracked sound without a second take. |
| **omx drive** | `org.openmixer.drive` | `urn:openmixer:drive` | The console's drive: one waveshaper (soft, tape, tube or exciter) run inside the console's 4x oversampler, on the full band, the lows, the highs or a tilt, with drive, character, wet/dry mix and trim. Stereo, real-time safe. |
| **omx ducker** | `org.openmixer.ducker` | `urn:openmixer:ducker` | Pulls the programme down while a key is heard, music under a voice, by a set depth with attack, hold and release, the console ducker. |
| **omx dyneq** | `org.openmixer.dyneq` | `urn:openmixer:dyneq` | A six-band dynamic EQ: each band is an EQ bell or shelf whose gain also moves with the level of its own band, cutting above a threshold (down) or lifting below it (up), held to a range, so a harsh or boomy band is tamed only while it is too loud. |
| **omx eq8** | `org.openmixer.eq8` | `urn:openmixer:eq8` | The console's channel EQ in its 8-band form: 8 parametric bands (bell, shelves, notch, all-pass) plus high- and low-pass filters at 12 or 24 dB/oct. Stereo, zero latency. |
| **omx eq16** | `org.openmixer.eq16` | `urn:openmixer:eq16` | The console's channel EQ in its 16-band form: 16 parametric bands (bell, shelves, notch, all-pass) plus high- and low-pass filters at 12 or 24 dB/oct. Stereo, zero latency. |
| **omx eq32** | `org.openmixer.eq32` | `urn:openmixer:eq32` | The console's channel EQ in its 32-band form: 32 parametric bands (bell, shelves, notch, all-pass) plus high- and low-pass filters at 12 or 24 dB/oct. Stereo, zero latency. |
| **omx flanger** | `org.openmixer.flanger` | `urn:openmixer:flanger` | The console's flanger: one modulated short delay with signed feedback, swept by one oscillator and mixed with the dry signal. |
| **omx geq** | `org.openmixer.geq` | `urn:openmixer:geq` | The console's 31-band graphic EQ: one fader per ISO third-octave band, 20 Hz to 20 kHz, ±15 dB each, designed in double precision so the lowest bands hold their shape at 192 kHz. Stereo, zero latency. |
| **omx glue** | `org.openmixer.glue` | `urn:openmixer:glue` | A bus compressor with the detents of a classic console: stepped ratio, attack and release, a key filter and a dry mix. |
| **omx keyed-gate** | `org.openmixer.keyed-gate` | `urn:openmixer:keyed-gate` | The console's channel gate with a sidechain key: the detector listens to the key your host routes to it, or to the signal itself. One gain for both legs, so the stereo image never shifts. Real-time safe. |
| **omx limiter** | `org.openmixer.limiter` | `urn:openmixer:limiter` | The precision limiter: a look-ahead, true-peak, stereo-linked brickwall. |
| **omx loudlevel** | `org.openmixer.loudlevel` | `urn:openmixer:loudlevel` | Holds a signal at a loudness target by moving its gain slowly, with a gate so pauses are left alone. |
| **omx multiband** | `org.openmixer.multiband` | `urn:openmixer:multiband` | A multiband compressor: two to five bands split by Linkwitz-Riley crossovers, each band its own compressor with threshold, ratio, attack, release, make-up, solo and bypass, then a mix and an output gain, for in-ear mixes, vocals and playback buses. |
| **omx phaser** | `org.openmixer.phaser` | `urn:openmixer:phaser` | The console's phaser: identical all-pass sections swept in octaves by one oscillator, with feedback around the chain and a wet/dry mix. |
| **omx pitch** | `org.openmixer.pitch` | `urn:openmixer:pitch` | The console's pitch shifter: shifts both channels up or down by semitones and cents, without changing their length, mixed with the dry signal. |
| **omx plosive** | `org.openmixer.plosive` | `urn:openmixer:plosive` | Softens the p pops and hard t and k consonants of a voice, cutting each only while it bursts. |
| **omx reverb** | `org.openmixer.reverb` | `urn:openmixer:reverb` | The console's reverb: room, plate, hall, reverse and gated algorithms with pre-delay, size, damping, width and low and high cuts, mixed with the dry signal. |
| **omx rotary** | `org.openmixer.rotary` | `urn:openmixer:rotary` | The console's rotary speaker: a drum rotor on the low band and a horn rotor on the high band, with stop, slow and fast speeds, mixed with the dry signal. |
| **omx spring** | `org.openmixer.spring` | `urn:openmixer:spring` | A spring reverb: a delay, a chain of all-passes and a darkening loop, with decay, length, tension and damping. |
| **omx strip** | `org.openmixer.strip` | `urn:openmixer:strip` | The console's channel strip in one plugin: input trim, HPF/LPF, gate, EQ of 1 to 32 bands (4 at first) and compressor, in the desk's order or any other. Stereo, real-time safe. |
| **omx suboct** | `org.openmixer.suboct` | `urn:openmixer:suboct` | Adds a note one or two octaves below a single played line, by dividing its pitch, with a tracking range and a gate for quiet input. |
| **omx tone** | `org.openmixer.tone` | `urn:openmixer:tone` | Tilts the whole tone darker or brighter about a pivot, and sweeps a resonant DJ filter from a low-pass through open to a high-pass with one knob. |
| **omx transient** | `org.openmixer.transient` | `urn:openmixer:transient` | The console's transient designer: more or less attack and sustain, from two envelope contrasts, with no threshold to set. |
| **omx tremolo** | `org.openmixer.tremolo` | `urn:openmixer:tremolo` | The console's tremolo and auto-pan: one oscillator turned into a level change on both legs, or into a left-right pan. |
| **omx upward** | `org.openmixer.upward` | `urn:openmixer:upward` | Lifts what is quiet toward a threshold, by a ratio and up to a limit, and leaves the loud alone. |

<!-- END GENERATED catalogue -->

<!-- BEGIN GENERATED sections: tools/gen.mjs, from each shipped plugin's declaration (its manual, else its parameters) -->

### omx balancer

| Parameter | Range | Default |
|---|---|---|
| Amount | 0 to 100 %, whole steps | 0 % |
| Range | 1 to 12 dB | 3 dB |
| Speed | 0.1 to 6 dB | 0.5 dB |
| Slope | -6 to 0 dB/oct | -3 dB/oct |

Plus the host's bypass.

### omx chorus

| Parameter | Range | Default |
|---|---|---|
| Spread | 0 to 0.5 | 0 |
| Rate | 0.05 to 8 Hz | 0.6 Hz |
| Depth | 0 to 12 ms | 4 ms |
| Voices | 1 to 4, whole steps | 3 |
| Mix | 0 to 100 % | 35 % |

Plus the host's bypass.

### omx clipper

| Parameter | Range | Default |
|---|---|---|
| Drive | 0 to 24 dB | 0 dB |
| Ceiling | -24 to 0 dB | 0 dB |
| Softness | 0 to 100 %, whole steps | 0 % |

Plus the host's bypass.

### omx comp

| Parameter | Range | Default |
|---|---|---|
| Threshold | -60 to 0 dB, whole steps | -18 dB |
| Ratio | 1 to 20 | 4 |
| Knee | 0 to 24 dB | 6 dB |
| Attack | 0.1 to 100 ms | 5 ms |
| Release | 5 to 3000 ms, whole steps | 200 ms |
| Makeup | 0 to 24 dB | 0 dB |
| Mix | 0 to 100 %, whole steps | 100 % |
| Kind | Compressor / Limiter | Compressor |
| Detector Oversampling | Auto / Off / 4x | Auto |
| Detector Topology | Feedforward / Feedback | Feedforward |
| Key Source | Self / Sidechain | Self |
| Key HP | 0 to 2000 Hz, whole steps | 0 Hz |
| Key LP | 0 to 20000 Hz, whole steps | 0 Hz |
| Key Tilt | -6 to 6 dB/oct | 0 dB/oct |
| Key Listen | off / on | off |
| Hold | 0 to 2000 ms, whole steps | 0 ms |
| Detect Link | Linked / Mid / Side / Max | Linked |
| Duck | off / on | off |
| Duck Range | -40 to 0 dB, whole steps | -12 dB |

Plus the host's bypass.

### omx deesser

| Parameter | Range | Default |
|---|---|---|
| Freq | 2000 to 16000 Hz, whole steps | 7000 Hz |
| Width | 0.25 to 4 oct | 1 oct |
| Threshold | -60 to 0 dB | -30 dB |
| Ratio | 1 to 20 | 4 |
| Range | -24 to 0 dB | -12 dB |
| Attack | 0.1 to 50 ms | 1 ms |
| Release | 5 to 500 ms, whole steps | 60 ms |
| Mode | Split / Wideband | Split |

Plus the host's bypass.

### omx delay

| Parameter | Range | Default |
|---|---|---|
| Time | 0 to 2000 ms, whole steps | 300 ms |
| Feedback | 0 to 0.99 | 0.3 |
| Mix (wet) | 0 to 1 | 0.3 |
| Tone (0 bright, 1 dark repeats) | 0 to 1 | 0.3 |
| Ping-pong | off / on | off |
| Sync (free: the Time control; tempo: the Division at the host's tempo) | Free, Tempo | Free |
| Division (while synced) | 1/1 to 1/32, each straight, dotted and triplet | 1/4 |
| Mode (the repeats' colour) | Digital, Tape, Analog | Digital |
| Taps (engaged read taps of the one delay line) | 1 to 4, whole steps | 1 |
| Tap 1 gain | 0 to 1 | 1 |
| Tap 1 pan (left to right) | -1 to 1 | 0 |
| Tap 2 time (a share of the delay time) | 1/4 to 3 | 1/2 |
| Tap 2 gain | 0 to 1 | 0.7 |
| Tap 2 pan (left to right) | -1 to 1 | -0.5 |
| Tap 3 time (a share of the delay time) | 1/4 to 3 | 3/4 |
| Tap 3 gain | 0 to 1 | 0.5 |
| Tap 3 pan (left to right) | -1 to 1 | 0.5 |
| Tap 4 time (a share of the delay time) | 1/4 to 3 | 3/2 |
| Tap 4 gain | 0 to 1 | 0.35 |
| Tap 4 pan (left to right) | -1 to 1 | 0 |
| Duck (how far the repeats drop while the input is loud) | -24 to 0 dB | 0 dB |
| Duck threshold | -60 to 0 dB, whole steps | -30 dB |
| Duck release | 20 to 2000 ms, whole steps | 250 ms |

With one tap the delay is the single-tap echo. Raising Taps adds the second to fourth reads of the same line, each with its own time, gain and pan; the feedback comes from the longest engaged tap, so a pattern repeats as a whole.

With Sync at Tempo the time is the Division at the host's tempo: 60000 / BPM x the division's beats (a quarter note is one beat), held to the Time control's top. The tempo is the CLAP host's transport tempo, or the LV2 host's time:Position time:beatsPerMinute on the plugin's Control port. While the host hands no tempo the delay keeps the Time control, exactly as at Free.

Mode colours the repeats: Digital is the delay as it shipped; Tape puts a low pass and a slow wow in the loop; Analog a lower low pass and a soft saturation. Duck lowers the repeats while the input is above the threshold, by up to the Duck depth, and lets them back up over the release.

Plus the host's bypass. The tone filter holds the same corner at every sample rate, so a session
moved from 48 to 96 kHz sounds the same.

### omx doubler

| Parameter | Range | Default |
|---|---|---|
| Voices | 1 to 4, whole steps | 2 |
| Delay | 5 to 40 ms | 12 ms |
| Detune | 0 to 25 cents | 8 cents |
| Drift Rate | 0.1 to 2 Hz | 0.6 Hz |
| Drift Depth | 0 to 3 ms | 0.5 ms |
| Spread | 0 to 100 %, whole steps | 60 % |
| Mix | 0 to 100 %, whole steps | 50 % |

Plus the host's bypass.

### omx drive

| Parameter | Range | Default |
|---|---|---|
| Drive | 0 to 36 dB | 0 dB |
| Character | -1 to 1 | 0 |
| Band Frequency | 20 to 20000 Hz, whole steps | 2000 Hz |
| Mix | 0 to 100 % | 100 % |
| Trim | -24 to 12 dB | 0 dB |
| Curve | Soft / Tape / Tube / Exciter | Soft |
| Band | Full / Low / High / Tilt | Full |
| Auto Gain | off / on | on |
| Stereo Link | off / on | on |
| HF Roll-off | Off / 12 kHz / 16 kHz | Off |
| Oversampling | 1 / 2 / 4 | 4 |

Plus the host's bypass.

### omx ducker

| Parameter | Range | Default |
|---|---|---|
| Threshold | -60 to 0 dB, whole steps | -18 dB |
| Depth | -40 to 0 dB, whole steps | -12 dB |
| Attack | 0.1 to 100 ms | 5 ms |
| Hold | 0 to 2000 ms, whole steps | 0 ms |
| Release | 5 to 3000 ms, whole steps | 200 ms |
| Ratio | 1 to 20 | 4 |
| Knee | 0 to 24 dB | 6 dB |
| Key High-Pass | 0 to 2000 Hz, whole steps | 0 Hz |
| Key Low-Pass | 0 to 20000 Hz, whole steps | 0 Hz |
| Key Listen | off / on | off |

Plus the host's bypass.

### omx dyneq

| Parameter | Range | Default |
|---|---|---|
| Band 1 On | off / on | off |
| Band 2 On | off / on | off |
| Band 3 On | off / on | off |
| Band 4 On | off / on | off |
| Band 5 On | off / on | off |
| Band 6 On | off / on | off |
| Band 1 Type | Bell / Low Shelf / High Shelf | Bell |
| Band 2 Type | Bell / Low Shelf / High Shelf | Bell |
| Band 3 Type | Bell / Low Shelf / High Shelf | Bell |
| Band 4 Type | Bell / Low Shelf / High Shelf | Bell |
| Band 5 Type | Bell / Low Shelf / High Shelf | Bell |
| Band 6 Type | Bell / Low Shelf / High Shelf | Bell |
| Band 1 Freq | 20 to 20000 Hz, whole steps | 40 Hz |
| Band 2 Freq | 20 to 20000 Hz, whole steps | 125 Hz |
| Band 3 Freq | 20 to 20000 Hz, whole steps | 400 Hz |
| Band 4 Freq | 20 to 20000 Hz, whole steps | 1250 Hz |
| Band 5 Freq | 20 to 20000 Hz, whole steps | 4000 Hz |
| Band 6 Freq | 20 to 20000 Hz, whole steps | 12500 Hz |
| Band 1 Gain | -15 to 15 dB | 0 dB |
| Band 2 Gain | -15 to 15 dB | 0 dB |
| Band 3 Gain | -15 to 15 dB | 0 dB |
| Band 4 Gain | -15 to 15 dB | 0 dB |
| Band 5 Gain | -15 to 15 dB | 0 dB |
| Band 6 Gain | -15 to 15 dB | 0 dB |
| Band 1 Q | 0.3 to 8 | 1 |
| Band 2 Q | 0.3 to 8 | 1 |
| Band 3 Q | 0.3 to 8 | 1 |
| Band 4 Q | 0.3 to 8 | 1 |
| Band 5 Q | 0.3 to 8 | 1 |
| Band 6 Q | 0.3 to 8 | 1 |
| Band 1 Dynamics | Off / Down / Up | Off |
| Band 2 Dynamics | Off / Down / Up | Off |
| Band 3 Dynamics | Off / Down / Up | Off |
| Band 4 Dynamics | Off / Down / Up | Off |
| Band 5 Dynamics | Off / Down / Up | Off |
| Band 6 Dynamics | Off / Down / Up | Off |
| Band 1 Threshold | -60 to 0 dB | -30 dB |
| Band 2 Threshold | -60 to 0 dB | -30 dB |
| Band 3 Threshold | -60 to 0 dB | -30 dB |
| Band 4 Threshold | -60 to 0 dB | -30 dB |
| Band 5 Threshold | -60 to 0 dB | -30 dB |
| Band 6 Threshold | -60 to 0 dB | -30 dB |
| Band 1 Ratio | 1 to 20 | 2 |
| Band 2 Ratio | 1 to 20 | 2 |
| Band 3 Ratio | 1 to 20 | 2 |
| Band 4 Ratio | 1 to 20 | 2 |
| Band 5 Ratio | 1 to 20 | 2 |
| Band 6 Ratio | 1 to 20 | 2 |
| Band 1 Range | -24 to 12 dB | -6 dB |
| Band 2 Range | -24 to 12 dB | -6 dB |
| Band 3 Range | -24 to 12 dB | -6 dB |
| Band 4 Range | -24 to 12 dB | -6 dB |
| Band 5 Range | -24 to 12 dB | -6 dB |
| Band 6 Range | -24 to 12 dB | -6 dB |
| Band 1 Attack | 0.5 to 100 ms | 5 ms |
| Band 2 Attack | 0.5 to 100 ms | 5 ms |
| Band 3 Attack | 0.5 to 100 ms | 5 ms |
| Band 4 Attack | 0.5 to 100 ms | 5 ms |
| Band 5 Attack | 0.5 to 100 ms | 5 ms |
| Band 6 Attack | 0.5 to 100 ms | 5 ms |
| Band 1 Release | 5 to 3000 ms, whole steps | 100 ms |
| Band 2 Release | 5 to 3000 ms, whole steps | 100 ms |
| Band 3 Release | 5 to 3000 ms, whole steps | 100 ms |
| Band 4 Release | 5 to 3000 ms, whole steps | 100 ms |
| Band 5 Release | 5 to 3000 ms, whole steps | 100 ms |
| Band 6 Release | 5 to 3000 ms, whole steps | 100 ms |
| Band 1 Listen | off / on | off |
| Band 2 Listen | off / on | off |
| Band 3 Listen | off / on | off |
| Band 4 Listen | off / on | off |
| Band 5 Listen | off / on | off |
| Band 6 Listen | off / on | off |
| Mix | 0 to 100 %, whole steps | 100 % |

Plus the host's bypass.

### omx eq8

| Section | Parameters |
|---|---|
| High-pass | on/off, 20 to 1000 Hz, whole steps; slope 12 dB/oct / 24 dB/oct |
| Low-pass | on/off, 1000 to 20000 Hz, whole steps; slope 12 dB/oct / 24 dB/oct |
| Bands 1 to 8 | each on/off, type, frequency (20 to 20000 Hz, whole steps), gain (-15 to 15 dB), Q (0.3 to 116; its top for a notch only) |

Plus the host's bypass, which is the whole EQ's switch. Every band and filter starts off, so a freshly loaded instance passes the
signal untouched.

### omx eq16

| Section | Parameters |
|---|---|
| High-pass | on/off, 20 to 1000 Hz, whole steps; slope 12 dB/oct / 24 dB/oct |
| Low-pass | on/off, 1000 to 20000 Hz, whole steps; slope 12 dB/oct / 24 dB/oct |
| Bands 1 to 16 | each on/off, type, frequency (20 to 20000 Hz, whole steps), gain (-15 to 15 dB), Q (0.3 to 116; its top for a notch only) |

Plus the host's bypass, which is the whole EQ's switch. Every band and filter starts off, so a freshly loaded instance passes the
signal untouched.

### omx eq32

| Section | Parameters |
|---|---|
| High-pass | on/off, 20 to 1000 Hz, whole steps; slope 12 dB/oct / 24 dB/oct |
| Low-pass | on/off, 1000 to 20000 Hz, whole steps; slope 12 dB/oct / 24 dB/oct |
| Bands 1 to 32 | each on/off, type, frequency (20 to 20000 Hz, whole steps), gain (-15 to 15 dB), Q (0.3 to 116; its top for a notch only) |

Plus the host's bypass, which is the whole EQ's switch. Every band and filter starts off, so a freshly loaded instance passes the
signal untouched.

### omx flanger

| Parameter | Range | Default |
|---|---|---|
| Rate | 0.05 to 5 Hz | 0.25 Hz |
| Depth | 0 to 5 ms | 2 ms |
| Feedback | -0.95 to 0.95 | 0.6 |
| Mix | 0 to 100 % | 50 % |

Plus the host's bypass.

### omx geq

| Parameter | Range | Default |
|---|---|---|
| 20 Hz | -15 to 15 dB | 0 dB |
| 25 Hz | -15 to 15 dB | 0 dB |
| 31.5 Hz | -15 to 15 dB | 0 dB |
| 40 Hz | -15 to 15 dB | 0 dB |
| 50 Hz | -15 to 15 dB | 0 dB |
| 63 Hz | -15 to 15 dB | 0 dB |
| 80 Hz | -15 to 15 dB | 0 dB |
| 100 Hz | -15 to 15 dB | 0 dB |
| 125 Hz | -15 to 15 dB | 0 dB |
| 160 Hz | -15 to 15 dB | 0 dB |
| 200 Hz | -15 to 15 dB | 0 dB |
| 250 Hz | -15 to 15 dB | 0 dB |
| 315 Hz | -15 to 15 dB | 0 dB |
| 400 Hz | -15 to 15 dB | 0 dB |
| 500 Hz | -15 to 15 dB | 0 dB |
| 630 Hz | -15 to 15 dB | 0 dB |
| 800 Hz | -15 to 15 dB | 0 dB |
| 1 kHz | -15 to 15 dB | 0 dB |
| 1.25 kHz | -15 to 15 dB | 0 dB |
| 1.6 kHz | -15 to 15 dB | 0 dB |
| 2 kHz | -15 to 15 dB | 0 dB |
| 2.5 kHz | -15 to 15 dB | 0 dB |
| 3.15 kHz | -15 to 15 dB | 0 dB |
| 4 kHz | -15 to 15 dB | 0 dB |
| 5 kHz | -15 to 15 dB | 0 dB |
| 6.3 kHz | -15 to 15 dB | 0 dB |
| 8 kHz | -15 to 15 dB | 0 dB |
| 10 kHz | -15 to 15 dB | 0 dB |
| 12.5 kHz | -15 to 15 dB | 0 dB |
| 16 kHz | -15 to 15 dB | 0 dB |
| 20 kHz | -15 to 15 dB | 0 dB |

Plus the host's bypass.

### omx glue

| Parameter | Range | Default |
|---|---|---|
| Threshold | -40 to 0 dB | -10 dB |
| Ratio | 2:1 / 4:1 / 10:1 | 2:1 |
| Attack | 0.1 ms / 0.3 ms / 1 ms / 3 ms / 10 ms / 30 ms | 10 ms |
| Release | 0.1 s / 0.3 s / 0.6 s / 1.2 s / Auto | Auto |
| Makeup | 0 to 24 dB | 0 dB |
| Key High-pass | 0 to 500 Hz, whole steps | 0 Hz |
| Mix | 0 to 100 %, whole steps | 100 % |
| Detector Topology | Feedforward / Feedback | Feedforward |

Plus the host's bypass.

### omx keyed-gate

The console's gate, keyed: the stereo signal is gated while the detector listens to a second, mono
input, the **key** (an LV2 sidechain port; in CLAP a second input port). With nothing routed to the
key, or **Key** set to Self, it is the ordinary self-keyed gate.

| Parameter | Range | Default |
|---|---|---|
| Key | Self / Sidechain | Sidechain |
| Threshold | -80 to 0 dB | -40 dB |
| Ratio | 1 to 100 | 16 |
| Range | -90 to 0 dB | -90 dB |
| Attack | 0 to 500 ms | 1 ms |
| Release | 0 to 5000 ms | 100 ms |
| Knee start | -80 to 0 dB, whole steps | -43 dB |
| Knee end | -80 to 0 dB, whole steps | -37 dB |
| Hold | 0 to 2000 ms, whole steps | 10 ms |
| Hysteresis | 0 to 24 dB | 3 dB |
| Key high-pass | 0 to 20000 Hz, whole steps | 0 Hz |
| Key low-pass | 0 to 20000 Hz, whole steps | 0 Hz |

Plus the host's bypass. An attack under 0.5 ms engages the 4x detector path, which adds 72 frames
of latency, reported to the host; otherwise the latency is zero. Its LV2 URI, ports and defaults
are those of the keyed gate bundle the OpenMixer console used to ship, so a session saved with that
bundle loads this one.

### omx limiter

| Parameter | Range | Default |
|---|---|---|
| Ceiling | -12 to 0 dBFS | -1 dBFS |
| Lookahead | 0.5 to 5 ms | 1.5 ms |
| Release | 1 to 1000 ms, whole steps | 50 ms |

Plus the host's bypass.

### omx loudlevel

| Parameter | Range | Default |
|---|---|---|
| Target | -40 to -6 dB | -23 dB |
| Range | 0 to 24 dB | 0 dB |
| Speed | 0.1 to 6 dB | 1 dB |
| Gate | -70 to -30 dB, whole steps | -50 dB |
| Window | Momentary / Short-term | Short-term |

Plus the host's bypass.

### omx multiband

| Parameter | Range | Default |
|---|---|---|
| Bands | 2 / 3 / 4 / 5 | 3 |
| Crossover 1 | 40 to 16000 Hz, whole steps | 200 Hz |
| Crossover 2 | 40 to 16000 Hz, whole steps | 2000 Hz |
| Crossover 3 | 40 to 16000 Hz, whole steps | 5000 Hz |
| Crossover 4 | 40 to 16000 Hz, whole steps | 6400 Hz |
| Band 1 Threshold | -60 to 0 dB | -20 dB |
| Band 2 Threshold | -60 to 0 dB | -20 dB |
| Band 3 Threshold | -60 to 0 dB | -20 dB |
| Band 4 Threshold | -60 to 0 dB | -20 dB |
| Band 5 Threshold | -60 to 0 dB | -20 dB |
| Band 1 Ratio | 1 to 20 | 2 |
| Band 2 Ratio | 1 to 20 | 2 |
| Band 3 Ratio | 1 to 20 | 2 |
| Band 4 Ratio | 1 to 20 | 2 |
| Band 5 Ratio | 1 to 20 | 2 |
| Band 1 Attack | 0.1 to 200 ms | 10 ms |
| Band 2 Attack | 0.1 to 200 ms | 10 ms |
| Band 3 Attack | 0.1 to 200 ms | 10 ms |
| Band 4 Attack | 0.1 to 200 ms | 10 ms |
| Band 5 Attack | 0.1 to 200 ms | 10 ms |
| Band 1 Release | 10 to 2000 ms, whole steps | 150 ms |
| Band 2 Release | 10 to 2000 ms, whole steps | 150 ms |
| Band 3 Release | 10 to 2000 ms, whole steps | 150 ms |
| Band 4 Release | 10 to 2000 ms, whole steps | 150 ms |
| Band 5 Release | 10 to 2000 ms, whole steps | 150 ms |
| Band 1 Makeup | 0 to 24 dB | 0 dB |
| Band 2 Makeup | 0 to 24 dB | 0 dB |
| Band 3 Makeup | 0 to 24 dB | 0 dB |
| Band 4 Makeup | 0 to 24 dB | 0 dB |
| Band 5 Makeup | 0 to 24 dB | 0 dB |
| Band 1 Solo | off / on | off |
| Band 2 Solo | off / on | off |
| Band 3 Solo | off / on | off |
| Band 4 Solo | off / on | off |
| Band 5 Solo | off / on | off |
| Band 1 Bypass | off / on | off |
| Band 2 Bypass | off / on | off |
| Band 3 Bypass | off / on | off |
| Band 4 Bypass | off / on | off |
| Band 5 Bypass | off / on | off |
| Mix | 0 to 100 %, whole steps | 100 % |
| Output | -24 to 24 dB | 0 dB |

Plus the host's bypass.

### omx phaser

| Parameter | Range | Default |
|---|---|---|
| Rate | 0.05 to 5 Hz | 0.5 Hz |
| Base | 50 to 2000 Hz, whole steps | 200 Hz |
| Depth | 0 to 6 oct | 4 oct |
| Stages | 2 to 12, whole steps | 6 |
| Feedback | -0.9 to 0.9 | 0.4 |
| Mix | 0 to 100 % | 50 % |

Plus the host's bypass.

### omx pitch

| Parameter | Range | Default |
|---|---|---|
| Semitones | -12 to 12 st, whole steps | 0 st |
| Cents | -50 to 50 cents, whole steps | 0 cents |
| Mix | 0 to 100 % | 100 % |

Plus the host's bypass.

### omx plosive

| Parameter | Range | Default |
|---|---|---|
| Consonant Cut | 0 to 24 dB | 6 dB |
| Consonant Sensitivity | 0 to 30 dB | 16 dB |
| Consonant Recovery | 2 to 50 ms | 8 ms |
| Consonant Band | 1000 to 12000 Hz, whole steps | 4000 Hz |
| Consonant Band Width | 0.5 to 4 oct | 2 oct |
| Pop Cut | 0 to 24 dB | 12 dB |
| Pop Sensitivity | 0 to 30 dB | 16 dB |
| Pop Recovery | 10 to 200 ms, whole steps | 40 ms |
| Pop Corner | 60 to 300 Hz, whole steps | 150 Hz |

Plus the host's bypass.

### omx reverb

| Parameter | Range | Default |
|---|---|---|
| Plate Mod Depth | 0 to 400 %, whole steps | 100 % |
| Mix | 0 to 1 | 0.3 |
| Size | 0 to 1 | 0.7 |
| Damping | 0 to 1 | 0.5 |
| Width | 0 to 1 | 1 |
| Predelay | 0 to 100 ms, whole steps | 0 ms |
| Lowcut | 0 to 20000 Hz, whole steps | 0 Hz |
| Highcut | 0 to 20000 Hz, whole steps | 20000 Hz |
| Reverse | 50 to 500 ms, whole steps | 300 ms |
| Hold | 10 to 2000 ms, whole steps | 120 ms |
| Release | 1 to 500 ms, whole steps | 20 ms |
| Gate Threshold | -80 to 0 dBFS, whole steps | -40 dBFS |
| Algorithm | Room / Plate / Hall / Reverse / Gated | Room |

Plus the host's bypass.

### omx rotary

| Parameter | Range | Default |
|---|---|---|
| Horn Slow | 0.1 to 2 Hz | 0.8 Hz |
| Horn Fast | 3 to 10 Hz | 6.8 Hz |
| Drum Slow | 0.1 to 2 Hz | 0.7 Hz |
| Drum Fast | 3 to 10 Hz | 5.9 Hz |
| Accel | 0.25 to 4 x | 1 x |
| Balance | -100 to 100 %, whole steps | 0 % |
| Mix | 0 to 100 % | 100 % |
| Speed | Stop / Slow / Fast | Slow |

Plus the host's bypass.

### omx spring

| Parameter | Range | Default |
|---|---|---|
| Decay | 0.5 to 8 s | 2 s |
| Length | 20 to 60 ms, whole steps | 35 ms |
| Tension | 0 to 100 %, whole steps | 50 % |
| Damping | 1000 to 10000 Hz, whole steps | 4000 Hz |
| Mix | 0 to 100 %, whole steps | 0 % |

Plus the host's bypass.

### omx strip

Five stages, each omx-dsp's instance face of the console's own kernel, run one after the other; each has its own switch:

| Stage | Parameters |
|---|---|
| Input | on/off; Trim, a click-free ramp to each new gain |
| Filters | on/off; HPF and LPF, each on/off, its frequency, 12 or 24 dB/oct |
| Gate | on/off, threshold, ratio, range, attack, release, knee start and end, hold, hysteresis |
| EQ | on/off; Bands, how many bands run (1 to 32, whole steps); each band on/off, type (bell, low or high shelf, notch, all-pass 1st or 2nd order), frequency, gain, Q |
| Comp | on/off, threshold, ratio, knee, attack, release, make-up, kind (compressor: RMS detector; limiter: peak), mix, detector oversampling (auto, off, 4x) |

**Order** (0 to 119) picks the order of the five stages: 0 is the desk's input > filters > gate > EQ >
comp, and the rest follow in lexicographic order (1 is input > filters > gate > comp > EQ, ... 119 is
comp > EQ > gate > filters > input). Plus the host's bypass. The latency reported is the sum of the
stages': the gate's and the compressor's 4x detector paths while they are engaged, else zero.

0.3.0 changed the parameter list (the Input and Filters switches, the gate's knee, hold and hysteresis, the compressor's kind, mix and detector oversampling, five-stage orders): a session saved with 0.2.0 does not load as it was, though at 0.2.0's settings the strip sounds the same, sample for sample.

**Bands** sets how many of the EQ's band slots run, from the first: a band past it is not processed at all (it costs nothing, and the strip sounds as if it were not there), and keeps its settings for when Bands reaches it again. The bands past the fourth and Bands itself came after the strip's other parameters, so their ids and ports follow all of those: a session or preset saved before them loads with its four bands as they were and Bands at its default.

### omx suboct

| Parameter | Range | Default |
|---|---|---|
| Range | Low / Mid / High | Low |
| Oct1 | 0 to 100 % | 100 % |
| Oct2 | 0 to 100 % | 0 % |
| Dry | 0 to 100 % | 100 % |
| Track Gate | -80 to -20 dBFS | -50 dBFS |

Plus the host's bypass.

### omx tone

| Parameter | Range | Default |
|---|---|---|
| Tilt | -6 to 6 dB | 0 dB |
| Pivot | 200 to 5000 Hz, whole steps | 1000 Hz |
| DJ Filter | -100 to 100, whole steps | 0 |
| Resonance | 0.5 to 4 | 0.71 |

Plus the host's bypass.

### omx transient

| Parameter | Range | Default |
|---|---|---|
| Attack | -24 to 24 dB | 0 dB |
| Sustain | -24 to 24 dB | 0 dB |
| Attack Time | 2 to 50 ms | 10 ms |
| Sustain Time | 50 to 2000 ms, whole steps | 250 ms |
| Output | -24 to 12 dB | 0 dB |

Plus the host's bypass.

### omx tremolo

| Parameter | Range | Default |
|---|---|---|
| Rate | 0.1 to 20 Hz | 4 Hz |
| Depth | 0 to 100 % | 50 % |
| Mix | 0 to 100 % | 100 % |
| Mode | Tremolo / Pan | Tremolo |

Plus the host's bypass.

### omx upward

| Parameter | Range | Default |
|---|---|---|
| Threshold | -70 to 0 dB | -40 dB |
| Ratio | 1 to 10 | 1 |
| Range | 0 to 24 dB | 6 dB |
| Attack | 0.1 to 100 ms | 5 ms |
| Release | 5 to 3000 ms, whole steps | 200 ms |

Plus the host's bypass.

<!-- END GENERATED sections -->

## Parameters

[plugins/README.md](plugins/README.md) lists every plugin with a page of its own: its ids, its audio ports, the
pages a host shows (LV2 port groups, CLAP modules) and a table of every parameter with its unit, range, default,
steps or values and flags. The pages are generated from the declarations by `tools/params-doc.mjs` (`make params-doc`),
and `make check-params-doc` fails when a committed page differs from what they say.

## Factory presets

Every plugin ships a set of factory presets, one JSON file each, in `presets/<plugin id>/<preset id>.json`
(for example `presets/org.openmixer.drive/amp-tube.json`). The `omx-plugins` package installs them
read only under `/usr/share/omx/presets/`, where the console's preset library lists them as the
factory group. A file names the plugin, the preset's id and name, and every parameter by symbol,
unit and plain value (a choice also by its label); no value leaves its declared range.
`tools/gen-presets.mjs` writes the files and `make presets` checks them.

## Packages

`omx-plugins` installs both formats. To take only the one your host loads, install
`omx-plugins-clap` (CLAP hosts) or `omx-plugins-lv2` (LV2 hosts) instead. The plugins land where
hosts look without configuration (`/usr/lib/clap` on every distribution, the path the CLAP specification gives; `/usr/lib64/lv2` on Fedora,
`/usr/lib/lv2` on Debian). Rescan plugins in your host and look for the **omx** plugins under openmixer.

Every release also carries the packages on its
[GitHub release page](https://github.com/Open-Mixer/omx-plugins/releases), and for a Zynthian without
the apt channel, `omx-plugins-zynthian-<version>-arm64.tar.gz`: copy its `lv2/` bundles into
`/zynthian/zynthian-plugins/lv2` and rescan the LV2 plugins from Zynthian's admin menu.

Projects that build on the plugins' parameters and instance code install the headers instead of
copying them: `omx-plugins-devel` on Fedora, `omx-plugins-dev` on Debian (they land in
`/usr/include/omx-plugins`). Each plugin's descriptor, its parameters in omx-contract's words, is in
`omx-plugins-contract`, which `omx-plugins` and the devel package require: files in omx-contract's
contract directory (`/usr/share/omx-contract/contract.d/3/omx-plugins/`) that a host reads at run time.
The code of each unit the param-contract extension serves is omx-contract's (`3/omx-contract/param-contract.json`).
The devel package carries `libomxplugins.a` (`pkg-config omxplugins`):
every plugin's CLAP face in one static library, for a host that runs them in its own process
without loading `.clap` files. `<omx-plugins/omx_clap_static.h>` lists their entries by CLAP id:

```c
#include <omx-plugins/omx_clap_static.h>

const clap_plugin_entry_t *e = omx_clap_static_find("org.openmixer.delay");
e->init("");
const clap_plugin_factory_t *f = e->get_factory(CLAP_PLUGIN_FACTORY_ID);
const clap_plugin_t *delay = f->create_plugin(f, host, "org.openmixer.delay");
```

`OMX_CLAP_STATIC_PLUGINS` (`OMX_CLAP_STATIC_COUNT` rows of `{id, entry}`) walks them all, and
`omx_clap_static_entry` is all of them behind one CLAP entry and one plugin factory.
