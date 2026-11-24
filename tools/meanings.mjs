#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * meanings.mjs — every control of every descriptor states its meaning, its ROLE (omx-contract docs/descriptors.md
 * "Meanings", ruling R1 on omx-contract#61: declare, don't infer; omx-plugins#208).
 *
 * A control's meaning comes from one of two places (docs/declaration-as-descriptor.md):
 *   - the panel role it plays, through contract/role-meanings.json, for a control a declared panel binds;
 *   - its own declaration, for every other control: `meaning` on the control of a described plugin's
 *     <stem>.descriptor.json, or on the parameter of a whole declaration (omx-strip's chain elements). A control the
 *     generator makes itself (a chain element's switch, the stage order, a band count) states its meaning where
 *     tools/gen.mjs makes it.
 *
 *   node tools/meanings.mjs --list     every shipped descriptor's controls with no meaning or `unknown`, and the count
 *                                      per plugin (missing/total); exit 1 when any is left
 *   node tools/meanings.mjs --apply    writes TABLE's word into each named control's declaration (the migration of
 *                                      omx-plugins#208), refusing a name that is no control, a control that plays a
 *                                      panel role, and a word the installed schema does not list; then run
 *                                      `node tools/gen.mjs` to render the descriptors and the faces
 *
 * TABLE is the record of that migration: each group is one word of omx-contract's PARAM_MEANINGS with the reason it
 * fits, read from the word's definition. After it the declarations are the store: a new control states its meaning
 * in its own declaration, and tools/descriptor.mjs refuses a control that states none.
 */
import { existsSync, readdirSync, readFileSync, writeFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { isMain } from './omx-contract.mjs';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..');

/** `omx-x:sym{1..3}Gain` as its names: a brace range expands to each integer, in order. */
export function expand(name) {
  const m = /\{(\d+)\.\.(\d+)\}/.exec(name);
  if (!m) return [name];
  const out = [];
  for (let i = Number(m[1]); i <= Number(m[2]); i++) out.push(...expand(name.slice(0, m.index) + i + name.slice(m.index + m[0].length)));
  return out;
}

/**
 * The word each control without a panel role plays, by group: `{ meaning, why, controls }`, `why` read from the
 * word's definition in PARAM_MEANINGS (contract.d/param/param.json), `controls` as `<stem>:<symbol>`.
 */
export const TABLE = [
  // levels
  { meaning: 'gain', why: '"A gain, a level change in dB or as a factor": a tap\'s, a layer\'s, a sub-octave\'s or the dry level, an EQ band\'s boost or cut, a trim, the transient designer\'s attack and sustain emphasis in dB',
    controls: ['omx-delay:tap{0..3}Gain', 'omx-reverb:earlyDb', 'omx-reverb:tailDb', 'omx-strip:trimDb', 'omx-strip:eq{1..32}Gain', 'omx-suboct:oct1', 'omx-suboct:oct2', 'omx-suboct:dry', 'omx-transient:attackDb', 'omx-transient:sustainDb', 'omx-trim:trimDb'] },
  { meaning: 'output', why: '"The output level of a process": the loudness the leveller holds its output at, the transient designer\'s output',
    controls: ['omx-loudlevel:targetLufs', 'omx-transient:outputDb'] },
  { meaning: 'makeup', why: '"The gain applied after a dynamics process"',
    controls: ['omx-glue:makeupDb', 'omx-multiband:makeupDb{2..5}', 'omx-strip:compMakeup'] },
  { meaning: 'ceiling', why: '"The level a limiter\'s output never exceeds": a clipper is the hard limit at its ceiling',
    controls: ['omx-clipper:ceilingDb'] },
  // dynamics
  { meaning: 'threshold', why: '"The level a dynamics process acts above or below": a band\'s, a duck\'s, a gate\'s, a de-esser\'s; the plosive detector\'s sensitivity (how far a burst rises before it is cut), the leveller\'s gate, the sub-octave\'s tracking gate',
    controls: ['omx-delay:duckThresholdDb', 'omx-dyneq:threshold{1..6}', 'omx-glue:thresholdDb', 'omx-loudlevel:gateLufs', 'omx-multiband:thresholdDb{2..5}', 'omx-plosive:sensitivityDb', 'omx-plosive:popSensitivityDb', 'omx-reverb:gateThreshold', 'omx-reverb:duckThresholdDb', 'omx-reverb:deessThresholdDb', 'omx-strip:gateThreshold', 'omx-strip:compThreshold', 'omx-suboct:trackGateDb', 'omx-upward:thresholdDb'] },
  { meaning: 'ratio', why: '"A dynamics ratio, n:1" (a choice of ratios included: omx-glue\'s 2:1, 4:1, 10:1)',
    controls: ['omx-dyneq:ratio{1..6}', 'omx-glue:ratio', 'omx-multiband:ratio{2..5}', 'omx-strip:gateRatio', 'omx-strip:compRatio', 'omx-upward:ratio'] },
  { meaning: 'range', why: '"How far a process may act": a gate\'s, an expander\'s or a duck\'s depth, a band\'s range, the most the balancer, the leveller or the plosive cut may move',
    controls: ['omx-balancer:rangeDb', 'omx-comp:rangeDb', 'omx-delay:duckDb', 'omx-ducker:rangeDb', 'omx-dyneq:range{1..6}', 'omx-loudlevel:rangeDb', 'omx-plosive:consonantDb', 'omx-plosive:popDb', 'omx-reverb:duckDb', 'omx-strip:gateRange', 'omx-upward:rangeDb'] },
  { meaning: 'knee', why: '"How gradually a dynamics process engages around its threshold": a compressor\'s knee, a gate\'s knee start and end, a clipper\'s softness around its ceiling',
    controls: ['omx-clipper:softnessPct', 'omx-strip:gateKneeStart', 'omx-strip:gateKneeEnd', 'omx-strip:compKnee'] },
  { meaning: 'hysteresis', why: '"The gap between a gate\'s opening and closing levels"', controls: ['omx-strip:gateHysteresis'] },
  // times
  { meaning: 'attack', why: '"How fast a process engages"',
    controls: ['omx-dyneq:attack{1..6}', 'omx-glue:attack', 'omx-multiband:attackMs{2..5}', 'omx-strip:gateAttack', 'omx-strip:compAttack', 'omx-transient:attackTimeMs', 'omx-upward:attackMs'] },
  { meaning: 'release', why: '"How fast a process lets go": a release, a duck\'s release, the plosive cut\'s recovery, the transient designer\'s sustain window',
    controls: ['omx-delay:duckReleaseMs', 'omx-dyneq:release{1..6}', 'omx-glue:release', 'omx-multiband:releaseMs{2..5}', 'omx-plosive:recoveryMs', 'omx-plosive:popRecoveryMs', 'omx-reverb:release', 'omx-reverb:duckReleaseMs', 'omx-strip:gateRelease', 'omx-strip:compRelease', 'omx-transient:sustainTimeMs', 'omx-upward:releaseMs'] },
  { meaning: 'hold', why: '"How long a process holds before it releases"',
    controls: ['omx-comp:holdMs', 'omx-ducker:holdMs', 'omx-reverb:hold', 'omx-strip:gateHold'] },
  { meaning: 'decay', why: '"How fast a sound or an envelope dies away": a spring\'s decay time', controls: ['omx-spring:decayS'] },
  { meaning: 'delay', why: '"A delay time": a tap\'s time (a factor of the delay time), a doubler voice\'s delay, the pseudo-stereo delay',
    controls: ['omx-delay:tap{1..3}Factor', 'omx-doubler:delayMs', 'omx-width:pseudoMs'] },
  { meaning: 'window', why: '"How long a stretch of signal a process reads or turns at once" (omx-contract#221): the leveller\'s loudness window, the reverse reverb\'s swell (the window of omx-dsp\'s reverse render)',
    controls: ['omx-loudlevel:window', 'omx-reverb:reverse'] },
  // frequencies
  { meaning: 'frequency', why: '"A frequency: an EQ band\'s centre": a band\'s centre, the band a drive or a de-esser acts on, a phaser\'s base, a tilt\'s pivot, the bass-mono corner (as the bass_mono_freq role plays)',
    controls: ['omx-drive:bandFreq', 'omx-phaser:baseHz', 'omx-plosive:bandHz', 'omx-reverb:deessFreqHz', 'omx-strip:eq{1..32}Freq', 'omx-tone:pivotHz', 'omx-width:bassMonoHz'] },
  { meaning: 'cutoffFrequency', why: '"A filter\'s cutoff frequency": a crossover\'s corner, the corner the pop band lies under',
    controls: ['omx-multiband:xoverHz{1..4}', 'omx-plosive:popHz'] },
  { meaning: 'lowCut', why: '"A low cut, a high-pass corner": a key\'s high-pass, the strip\'s HPF (as the hpf_freq role plays)',
    controls: ['omx-comp:keyHpHz', 'omx-ducker:keyHpHz', 'omx-glue:keyHpHz', 'omx-keyed-gate:key_hp', 'omx-strip:hpfFreq'] },
  { meaning: 'highCut', why: '"A high cut, a low-pass corner": a key\'s low-pass, the strip\'s LPF (as the lpf_freq role plays), the drive\'s HF roll-off corner',
    controls: ['omx-comp:keyLpHz', 'omx-ducker:keyLpHz', 'omx-keyed-gate:key_lp', 'omx-strip:lpfFreq', 'omx-drive:hfRolloff'] },
  { meaning: 'q', why: '"An EQ band\'s or a filter\'s quality factor"', controls: ['omx-strip:eq{1..32}Q'] },
  { meaning: 'bandwidth', why: '"An EQ band\'s or a filter\'s width, in octaves": the consonant band\'s width in oct', controls: ['omx-plosive:bandWidthOct'] },
  { meaning: 'slope', why: '"How steeply a filter or a shelf rolls off": the strip\'s filter slopes (as hpf_slope and lpf_slope play), the slope in dB/oct of the balancer\'s target tone',
    controls: ['omx-balancer:slopeDbOct', 'omx-strip:hpfSlope', 'omx-strip:lpfSlope'] },
  { meaning: 'tone', why: '"A tone control: a brightness or a darkness": a tilt in dB, of the signal or of the key',
    controls: ['omx-comp:keyTiltDb', 'omx-tone:tiltDb'] },
  // modulation, pitch, space
  { meaning: 'rate', why: '"The rate of a modulation, such as an LFO\'s speed" (`speed` is `rate`, docs/descriptors.md): a rotor\'s slow and fast speeds and its speed switch, the dB/s a balancer or a leveller moves at',
    controls: ['omx-balancer:speedDbPerS', 'omx-loudlevel:speedDbPerS', 'omx-rotary:hornSlowHz', 'omx-rotary:hornFastHz', 'omx-rotary:drumSlowHz', 'omx-rotary:drumFastHz', 'omx-rotary:speed'] },
  { meaning: 'acceleration', why: '"How fast a moving part changes speed" (omx-contract#221): the scale on the rotors\' speed-change times', controls: ['omx-rotary:accel'] },
  { meaning: 'depth', why: '"The depth of a modulation": the plate\'s modulation depth', controls: ['omx-reverb:plateModDepth'] },
  { meaning: 'shift', why: '"A pitch or a frequency shift": a doubler voice\'s detune in cents', controls: ['omx-doubler:detuneCents'] },
  { meaning: 'size', why: '"A reverb\'s room size": the spring\'s length (its loop\'s delay), the early reflections\' size',
    controls: ['omx-spring:lengthMs', 'omx-reverb:earlySizePct'] },
  { meaning: 'damping', why: '"How fast a reverb\'s high frequencies die away": the spring\'s darkening corner', controls: ['omx-spring:dampHz'] },
  { meaning: 'dispersion', why: '"How far a process spreads a sound\'s frequencies apart in time" (omx-contract#221): the spring\'s tension sets its all-pass corner, the chirp', controls: ['omx-spring:tensionPct'] },
  { meaning: 'width', why: '"A stereo width"', controls: ['omx-width:widthPct'] },
  { meaning: 'balance', why: '"Where a signal sits between two sides" (omx-contract#221): a tap\'s pan (the balance law\'s travel), the horn against the drum',
    controls: ['omx-delay:tap{0..3}Pan', 'omx-rotary:balance'] },
  { meaning: 'wetDryRatio', why: '"The mix of a wet and a dry signal"', controls: ['omx-dyneq:mixPct', 'omx-glue:mixPct', 'omx-spring:mixPct', 'omx-strip:compMix'] },
  { meaning: 'drive', why: '"How hard a saturation or distortion is driven"', controls: ['omx-clipper:driveDb'] },
  { meaning: 'amount', why: '"How strongly a process acts, from none to all of it" (omx-contract#221): the balancer\'s pull toward its target', controls: ['omx-balancer:amountPct'] },
  { meaning: 'number', why: '"How many of a repeated part a process runs" (omx-contract#221): taps, bands, stages',
    controls: ['omx-delay:fxDelayTaps', 'omx-multiband:bands', 'omx-phaser:stages'] },
  // switches
  { meaning: 'enabled', why: '"On when the process is on": a part of the plugin switched on, a filter, an EQ band, the duck, the auto gain, the wet de-esser, the bass mono (as band_on, hpf_on and lpf_on play)',
    controls: ['omx-comp:duck', 'omx-drive:autoGain', 'omx-reverb:wetDeess', 'omx-strip:hpfOn', 'omx-strip:lpfOn', 'omx-strip:eq{1..32}On', 'omx-width:bassMono'] },
  { meaning: 'bypass', why: '"On when the process is bypassed": a band\'s bypass', controls: ['omx-multiband:bandBypass{1..5}'] },
  { meaning: 'listen', why: '"On when the process\'s side signal is heard alone": a key listen, a band\'s detector listen', controls: ['omx-comp:keyListen', 'omx-ducker:keyListen', 'omx-dyneq:listen{1..6}'] },
  { meaning: 'solo', why: '"On when one part of a process is heard alone" (omx-contract#221): a band\'s solo, its output and not its side signal', controls: ['omx-multiband:solo{1..5}'] },
  { meaning: 'link', why: '"How a process ties its channels together" (omx-contract#221): the stereo link switch, the detector\'s link choice (linked, mid, side, max)',
    controls: ['omx-comp:detectLink', 'omx-drive:stereoLink'] },
  { meaning: 'sync', why: '"On when a time follows the host\'s tempo": Free or Tempo', controls: ['omx-delay:sync'] },
  { meaning: 'division', why: '"A note division a tempo-synced time takes"', controls: ['omx-delay:division'] },
  // choices
  { meaning: 'type', why: '"A choice of type, such as a filter\'s": an EQ band\'s type, a compressor\'s kind (compressor or limiter), a detector\'s topology (feedforward or feedback)',
    controls: ['omx-comp:kind', 'omx-comp:detectorTopology', 'omx-glue:detectorTopology', 'omx-strip:eq{1..32}Type', 'omx-strip:compKind'] },
  { meaning: 'model', why: '"A choice of the modelled unit": the delay\'s digital, tape or analog line', controls: ['omx-delay:mode'] },
  { meaning: 'algorithm', why: '"A choice of algorithm": the pseudo-stereo\'s comb or Haas (off the third)', controls: ['omx-width:pseudo'] },
  { meaning: 'mode', why: '"A choice of mode": which key the detector hears, the release mode, the oversampling, the band a drive works on, a band\'s dynamics (off, down, up), the tracking range, tremolo or pan',
    controls: ['omx-comp:keySource', 'omx-comp:releaseMode', 'omx-comp:detectorOversampling', 'omx-drive:band', 'omx-drive:oversample', 'omx-dyneq:dynMode{1..6}', 'omx-keyed-gate:key_external', 'omx-suboct:range', 'omx-tremolo:mode', 'omx-strip:compDetectorOversampling'] },
];

/** The shipped plugin folders: each with a <stem>.descriptor.json, whose declaration is no test plugin's. */
export function shipped(root = ROOT) {
  return readdirSync(join(root, 'plugins')).filter((stem) => {
    const decl = join(root, 'plugins', stem, `${stem}.decl.json`);
    if (existsSync(decl) && JSON.parse(readFileSync(decl, 'utf8')).role === 'test') return false;
    return existsSync(join(root, 'plugins', stem, `${stem}.descriptor.json`));
  }).sort();
}

/** `{ stem: { total, missing: [symbol...] } }`: the controls of each shipped descriptor with no meaning or `unknown`. */
export function missingMeanings(root = ROOT) {
  const out = {};
  for (const stem of shipped(root)) {
    const d = JSON.parse(readFileSync(join(root, 'plugins', stem, `${stem}.descriptor.json`), 'utf8'));
    out[stem] = { total: d.controls.length, missing: d.controls.filter((c) => c.meaning === undefined || c.meaning === 'unknown').map((c) => c.symbol) };
  }
  return out;
}

/** The meanings omx-contract's installed schema lists ($defs.meaning). */
function schemaMeanings() {
  // the same file tools/descriptor.mjs reads the descriptor words from
  return import('./descriptor.mjs').then(({ descriptorSchemaFile }) => JSON.parse(readFileSync(descriptorSchemaFile(), 'utf8')).$defs.meaning.enum);
}

const write = (file, doc) => writeFileSync(file, `${JSON.stringify(doc, null, 2)}\n`);

/** The file control `sym` of plugin `stem` is declared in, and the plugin's panel roles: a chain's or a base's
 * declaration for a whole one, else the plugin's descriptor. */
function declarationOf(root, stem) {
  const own = join(root, 'plugins', stem, `${stem}.decl.json`);
  const decl = JSON.parse(readFileSync(own, 'utf8'));
  const whole = decl.binding === 'chain' || decl.variants !== undefined || decl.params !== undefined;
  return { file: whole ? own : join(root, 'plugins', stem, `${stem}.descriptor.json`), roles: decl.panel?.roles ?? {} };
}

/** The object of `sym` in a loaded declaration or descriptor `doc`: a chain element's parameter, a control, a parameter. */
function nodeOf(doc, sym) {
  if (doc.binding === 'chain') return doc.chain.flatMap((el) => [...el.params, ...(el.appended ?? [])]).find((p) => p.symbol === sym);
  if (doc.controls) return doc.controls.find((c) => c.symbol === sym);
  return (doc.params ?? []).find((p) => p.symbol === sym);
}

/** Writes TABLE into the declarations; returns the number of controls written. Throws on the first refusal. */
export async function apply(root = ROOT, table = TABLE) {
  const words = new Set(await schemaMeanings());
  const docs = new Map();
  const seen = new Set();
  let n = 0;
  for (const g of table) {
    if (!words.has(g.meaning) || g.meaning === 'unknown') throw new Error(`TABLE: '${g.meaning}' is no word an authored control may state (the installed schema's $defs.meaning, never unknown)`);
    for (const name of g.controls.flatMap(expand)) {
      if (seen.has(name)) throw new Error(`TABLE: ${name} is named twice`);
      seen.add(name);
      const [stem, sym] = name.split(':');
      const { file, roles } = declarationOf(root, stem);
      if (Object.values(roles).flat().includes(sym)) throw new Error(`TABLE: ${name} plays a panel role; its meaning is its role's (contract/role-meanings.json)`);
      if (!docs.has(file)) docs.set(file, JSON.parse(readFileSync(file, 'utf8')));
      const node = nodeOf(docs.get(file), sym);
      if (!node) throw new Error(`TABLE: ${name} is no control declared in ${file.slice(root.length + 1)} (a chain's switch, its order and a band count state theirs in tools/gen.mjs)`);
      node.meaning = g.meaning;
      n++;
    }
  }
  for (const [file, doc] of docs) write(file, doc);
  return n;
}

function list(root = ROOT) {
  const m = missingMeanings(root);
  let missing = 0, total = 0;
  for (const [stem, { total: t, missing: x }] of Object.entries(m)) {
    total += t; missing += x.length;
    console.log(`${stem} ${x.length}/${t}${x.length ? `: ${x.join(' ')}` : ''}`);
  }
  console.log(`meanings: ${missing} of ${total} controls state no meaning`);
  return missing ? 1 : 0;
}

if (isMain(import.meta.url)) {
  const arg = process.argv[2];
  if (arg === '--list') process.exit(list());
  else if (arg === '--apply') apply().then((n) => console.log(`meanings: ${n} controls written; now node tools/gen.mjs`), (e) => { console.error(`meanings: ${e.message}`); process.exit(1); });
  else { console.error('usage: node tools/meanings.mjs --list | --apply'); process.exit(2); }
}
