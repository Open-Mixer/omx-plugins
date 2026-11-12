#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * round-trip.mjs — the authored descriptor against what plugin-hostd scans back from our built faces
 * (docs/declaration-as-descriptor.md, "The round trip"; FreeMixer/omx-plugins#210, decision B). tools/round-trip.sh
 * generates and builds each described plugin and writes `plugin-hostd-probe describe` of both faces; this compares
 * each scanned descriptor with the authored one, word by word, one line each:
 *
 *   equal <format> <where> <word>             the face states what the descriptor says
 *   lost <format> <where> <word> <authored>   the face does not carry the word: no row of plugin-hostd's <format>
 *                                             table reads it back, or the scan states none (`unknown`, or absent)
 *   differs <format> <where> <word> ...       the face states another value
 *
 * A word the format's table has no row for is lost by name, never filled in; so is a value the scan leaves unknown.
 * What the format itself says otherwise is named here, each once: an LV2 plugin's id is its URI and an LV2 control's
 * id its symbol (omx-contract's descriptor schema; plugin-hostd docs/formats.md), an LV2 face's audio ports are one
 * per channel with the generator's own symbols and names (the descriptor's port id and name are CLAP's), a CLAP face
 * reads back the float it stores. The face's own bypass (CLAP_PARAM_IS_BYPASS, lv2:enabled) stands beside the controls.
 * A meter is an output control port on the LV2 face, which a scan reads as a read-only control by the meter's symbol,
 * one per channel (`<symbol>_<channel>` past one); the CLAP face publishes it through clap.meters/1, which plugin-hostd's
 * CLAP table has no row for.
 * The bags (`extensions.<format>`) are equal: every source field the authored bag holds, the scanned one holds with the
 * same value (an LV2 field's values a set, as RDF states them).
 *
 * Pass: no `differs`, every bag equal, and the lost words the ones LOSSES names, by count (a new loss is red until it
 * is named there). LOSSES is counted against one plugin-hostd release (LOSSES_PROBE): a run against another release
 * whose losses moved says so once, `ledger for plugin-hostd <ledger>, probe <installed>`, and the ledger is counted
 * again against that release.
 *
 * Usage: node tools/round-trip.mjs <scans dir> [--authored <stem>=<file>]... ; the scans dir holds <stem>.clap.json
 * and <stem>.lv2.json for every described plugin. --authored reads another descriptor in the plugin's place (the
 * sabotage arm's). Exit 1 when it does not pass.
 */
import { execFileSync } from 'node:child_process';
import { existsSync, readFileSync, readdirSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { descriptorFile } from './descriptor.mjs';
import { lookupValues, tableRows, tableWords } from './format-tables.mjs';
import { compareVersions } from './omx-contract.mjs';

/** The installed plugin-hostd's version, as pkg-config reads it, or undefined. */
function hostdVersion() {
  try {
    return execFileSync(process.env.PKG_CONFIG || 'pkg-config', ['--modversion', 'plugin-hostd'], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'] }).trim() || undefined;
  } catch {
    return undefined;
  }
}

export const ROOT = resolve(new URL('..', import.meta.url).pathname);
export const FORMATS = ['clap', 'lv2'];

/** The losses of this tree, `<format> <word>` -> how many words (OR2: the meaning has a CLAP word for the bypass only
 * and an LV2 term for some meanings; LV2 has no unit for ""). Each other entry is a word the format's table has no
 * row for, or one its face states otherwise, named in the header above. */
/** The plugin-hostd release LOSSES is counted against (its format tables decide what each face loses). */
export const LOSSES_PROBE = '0.20.3';

export const LOSSES = {
  // OR2: a meaning has a CLAP word only for some (plugin-hostd 0.20.3's CLAP table reads the meanings it has rows for;
  // ten are left), an LV2 term only for some (lv2:designation); LV2 has no unit for "" (a plain number)
  'clap controls[].meaning': 10,
  'lv2 controls[].meaning': 116,
  'lv2 controls[].travel.unit ""': 123,
  // nor for urn:openmixer:unit#dBoct, the unit of the compressor's key tilt (dB/oct, omx-contract 3.1.0)
  'lv2 controls[].travel.unit "dB/oct"': 2,
  // CLAP: plugin-hostd 0.20.3 reads a control's scale and, through the parameter contract, most units and steps; no
  // row reads the rest of the units and steps, a choice (our faces set no CLAP_PARAM_IS_ENUM), the session symbol, the
  // LV2 URI, a side-chain's role or what an LV2 host may lend
  'clap controls[].travel.unit': 125,
  'clap controls[].travel.step': 375,
  'clap controls[].kind': 266,
  'clap controls[].ids': 137,
  'clap controls[].values': 137,
  'clap controls[].labels': 137,
  'clap controls[].default': 266,
  'clap controls[].symbol': 825,
  'clap plugin.uri': 32,
  'clap ports[].role': 3,
  'clap needs.extensions.optional': 32,
  // LV2: no row reads a step, automation, the plugin's version or vendor (our .ttl writes no doap:maintainer); a
  // control's id is its symbol and a point's id its label, so the CLAP id and our point ids are no LV2 word; the
  // audio ports are one per channel, with the generator's symbols and names
  'lv2 controls[].travel.step': 559,
  'lv2 controls[].automatable': 825,
  'lv2 controls[].id': 825,
  'lv2 controls[].ids': 137,
  'lv2 plugin.version': 32,
  'lv2 plugin.vendor': 32,
  'lv2 ports[].id': 67,
  'lv2 ports[].name': 67,
  // LV2 port groups are flat: a control in a section's subgroup (an EQ's band, #257) carries the group, not the
  // section above it (omx-eq8, omx-eq16 and omx-eq32: five controls a band, 8 + 16 + 32 bands)
  'lv2 controls[].group (panel section)': 280,
  // the toggles UPSTREAM names, scanned as travels, carry no toggle's default
  'lv2 controls[].default': 129,
  // neither face carries a control's prose (omx-delay's sync), nor states that it reads the tempo: CLAP has no static
  // word for it, and the LV2 face's control port supports time:Position, which a scan reads as no tempo word
  'clap controls[].doc': 1,
  'lv2 controls[].doc': 1,
  'clap needs.tempo': 1,
  'lv2 needs.tempo': 1,
  // a meter: plugin-hostd's CLAP table has no row for clap.meters/1 (one word a meter: 20 meters); an LV2 output
  // control port carries its symbol, range, unit and scale, and no word for its CLAP id, kind, channel count or
  // readout doc; a two-channel level is two ports, each named for its channel (4 meters, 8 ports)
  'clap meters[]': 20,
  'lv2 meters[].id': 20,
  'lv2 meters[].kind': 20,
  'lv2 meters[].channels': 20,
  'lv2 meters[].doc': 16,
  'lv2 meters[].name': 8,
};

/**
 * The ledger's lines and failures for the losses a run counted, against the installed plugin-hostd `probe`. Each lost
 * word LOSSES names by its count is named; one it does not is an unnamed loss, a failure each, when the probe is the
 * release LOSSES was counted against. Against another release (a probe bump moves the format tables, and with them the
 * counts) the moved counts are ONE failure, said as that: the ledger is counted again, not read as unnamed losses.
 */
export function ledgerLines(losses, probe) {
  const lines = [];
  const keys = [...new Set([...Object.keys(LOSSES), ...Object.keys(losses)])].sort();
  const moved = keys.filter((k) => LOSSES[k] !== losses[k]);
  const otherRelease = probe !== undefined && probe !== LOSSES_PROBE;
  if (otherRelease && moved.length > 0) lines.push(`ledger for plugin-hostd ${LOSSES_PROBE}, probe ${probe}: ${moved.length} count(s) moved with the tables; count LOSSES again against ${probe} and set LOSSES_PROBE`);
  for (const k of keys) {
    const ok = !moved.includes(k);
    const label = ok ? 'named' : otherRelease ? `loss against the ${LOSSES_PROBE} ledger,` : 'unnamed';
    lines.push(`${label} loss ${k}: ${losses[k] ?? 0} word(s)${ok ? '' : `, LOSSES names ${LOSSES[k] ?? 0}`}`);
  }
  return { lines, fails: otherRelease ? (moved.length > 0 ? 1 : 0) : moved.length };
}

/** A break in plugin-hostd's wave tip, reported, each `differs` it causes printed with it
 * and not failing; the run fails when one no longer shows, so the entry leaves with the fix (an entry that names its
 * `at` ports: when a port outside them differs; an entry with a `through` also, when the installed plugin-hostd is past
 * it; an entry with a `through` and no `at` names a class and excuses any number of its differs). */
export const UPSTREAM = {
};

/** An UPSTREAM entry excuses a differ when it names no ports (`at`), or names this plugin's port. */
const excused = (entry, stem, where) => entry !== undefined && (entry.at === undefined || entry.at.includes(`${stem} ${where}`));

/** The described plugins (a descriptor beside the declaration), and the ones outside the round trip. */
export function described(root = ROOT) {
  const stems = readdirSync(join(root, 'plugins'), { withFileTypes: true }).filter((e) => e.isDirectory()).map((e) => e.name).sort();
  return { inside: stems.filter((s) => existsSync(join(root, 'plugins', s, descriptorFile(s)))), outside: stems.filter((s) => !existsSync(join(root, 'plugins', s, descriptorFile(s)))) };
}

const f32 = (x) => (typeof x === 'number' ? Math.fround(x) : x);
const same = (a, b) => JSON.stringify(a) === JSON.stringify(b);
const sorted = (v) => (Array.isArray(v) ? [...v].sort() : [v]);
const FACE_OWN = { clap: 'bypass', lv2: 'enabled' };

/** The leaf words of `o`, `[path, value]`, arrays of scalars as one value. */
function leaves(o, path = '', out = []) {
  if (o && typeof o === 'object' && !Array.isArray(o)) for (const [k, v] of Object.entries(o)) leaves(v, path ? `${path}.${k}` : k, out);
  else out.push([path, o]);
  return out;
}

/** Every line of one face's comparison: `[{ verdict, format, where, word, authored, scanned }]`. */
export function compareFace(format, authored, scanned) {
  const out = [];
  const words = tableWords(format);
  // a value no row of the table gives the word is describe's own when no row set it (a CLAP control a `travel`, a CLAP
  // port that is not main `aux`): the face stated nothing
  const lookupOnly = (word) => Object.values(tableRows(format)).every((r) => r.word?.word !== word || r.word.transform === 'lookup');
  const defaulted = (word, s) => lookupOnly(word) && words.has(word) && !Object.values(lookupValues(format, word)).some((v) => same(v, s));
  const emit = (where, word, a, s, carried = true) => {
    // CLAP states an integer travel's step (CLAP_PARAM_IS_STEPPED, scanned stepped) and has no word for its law: the log
    // of an integer frequency is lost on the CLAP face, as every log and linear scale is there
    const stepOnly = format === 'clap' && word === 'controls[].travel.scale' && a === 'log' && s === 'stepped';
    const verdict = !carried || stepOnly || s === undefined || s === 'unknown' || (!same(a, s) && defaulted(word, s)) ? 'lost' : same(a, s) ? 'equal' : 'differs';
    out.push({ verdict, format, where, word, authored: a, scanned: s });
  };
  const read = (word, base = word) => words.has(base) || words.has(base.replace(/^(controls|ports)\[\]\./, '$1[].'));

  // the plugin: what a face states of it
  const ap = authored.plugin, sp = scanned.plugin;
  for (const [k, a] of Object.entries(ap)) {
    if (k === 'format') continue;
    if (format === 'lv2' && k === 'id') emit('plugin', 'plugin.id', ap.uri, sp.id);
    else if (format === 'lv2' && k === 'uri') emit('plugin', 'plugin.uri', a, sp.uri);
    else emit('plugin', `plugin.${k}`, a, sp[k], read(`plugin.${k}`));
  }

  // the controls, in order, the face's own bypass beside them and, on LV2, the meters' read-only ports after them
  const outs = (authored.meters ?? []).flatMap((m) => (m.channels > 1 ? Array.from({ length: m.channels }, (_, k) => `${m.symbol}_${k}`) : [m.symbol]));
  const isMeterPort = (c) => format === 'lv2' && c.readonly === true && outs.includes(c.id);
  const sc = scanned.controls.filter((c) => c.meaning !== FACE_OWN[format] && !isMeterPort(c));
  if (sc.length !== authored.controls.length) out.push({ verdict: 'differs', format, where: 'controls', word: 'controls', authored: authored.controls.length, scanned: sc.length });
  authored.controls.forEach((c, i) => {
    const s = sc[i] ?? {};
    const where = c.symbol ?? String(c.id);
    for (const [path, a] of leaves(c)) {
      if (path.startsWith('source')) continue;
      const word = `controls[].${path}`;
      if (format === 'lv2' && path === 'id') emit(where, word, a, undefined, false);
      else if (format === 'lv2' && path === 'symbol') emit(where, word, a, s.id);
      // a choice's points carry its values and labels; describe names each point by its label, so the default is
      // the value it designates and our ids are no word a face carries
      else if (c.kind === 'choice' && s.kind === 'choice' && path === 'values') emit(where, word, a, s.values);
      else if (c.kind === 'choice' && s.kind === 'choice' && path === 'default') emit(where, word, c.values[c.ids.indexOf(a)], s.values?.[s.ids?.indexOf(s.default)]);
      else if (path === 'ids') emit(where, word, a, undefined, false);
      // an LV2 port group is one level (pg:group): a path [section, group] is carried as its last element, the LV2
      // group; the panel section above it is no LV2 word (FreeMixer/omx-plugins#257)
      else if (format === 'lv2' && path === 'group' && Array.isArray(a) && a.length > 1) {
        emit(where, word, a.slice(-1), s.group);
        emit(where, `${word} (panel section)`, a.slice(0, -1), undefined, false);
      }
      else {
        const v = path.split('.').reduce((o, k) => o?.[k], s);
        const fl = format === 'clap' && path.startsWith('travel.') && typeof a === 'number';
        emit(where, word, fl ? f32(a) : a, fl ? f32(v) : v, word === 'controls[].symbol' ? false : read(word) || ['controls[].id', 'controls[].name'].includes(word));
      }
    }
  });

  // the meters: on LV2 each channel's read-only control, by its symbol; the CLAP scan reads no clap.meters/1
  const sm = scanned.controls.filter(isMeterPort);
  for (const m of authored.meters ?? []) {
    const where = `meter ${m.symbol}`;
    if (format === 'clap') {
      emit(where, 'meters[]', m.symbol, undefined, false);
      continue;
    }
    const chans = m.channels > 1 ? Array.from({ length: m.channels }, (_, k) => `${m.symbol}_${k}`) : [m.symbol];
    for (const sym of chans) {
      const s = sm.find((c) => c.id === sym);
      if (!s) {
        out.push({ verdict: 'differs', format, where, word: 'meters[].symbol', authored: sym, scanned: undefined });
        continue;
      }
      emit(where, 'meters[].symbol', sym, s.id);
      for (const k of ['min', 'max', 'unit', 'scale']) emit(where, `meters[].${k}`, m[k], s.travel?.[k]);
      // a meter of more than one channel is one port per channel, each named for its channel
      emit(where, 'meters[].name', m.name, m.channels > 1 ? undefined : s.name, m.channels === 1);
    }
    for (const k of ['id', 'kind', 'channels', 'doc']) if (m[k] !== undefined) emit(where, `meters[].${k}`, m[k], undefined, false);
  }

  // the ports: by direction, in order
  const sports = [...scanned.ports];
  for (const p of authored.ports) {
    const k = sports.findIndex((q) => q.direction === p.direction);
    const s = k < 0 ? {} : sports.splice(k, 1)[0];
    const where = `port ${p.direction} ${p.name}`;
    for (const [path, a] of leaves(p)) {
      const word = `ports[].${path}`;
      const perChannel = format === 'lv2' && p.kind === 'audio' && (path === 'id' || path === 'name');
      emit(where, word, a, s[path], !perChannel);
    }
  }
  for (const s of sports) out.push({ verdict: 'differs', format, where: `port ${s.direction} ${s.name}`, word: 'ports[]', authored: undefined, scanned: s.id });

  // the rest of the descriptor's words: latency, needs, instrument
  for (const k of ['latency', 'needs', 'instrument']) {
    for (const [path, a] of leaves(authored[k] ?? {}, k)) emit('plugin', path, a, path.split('.').reduce((o, x) => o?.[x], scanned), read(path));
    if (scanned[k] && !authored[k]) out.push({ verdict: 'differs', format, where: 'plugin', word: k, authored: undefined, scanned: scanned[k] });
  }

  // the bag
  const ab = authored.extensions?.[format] ?? {}, sb = scanned.extensions?.[format] ?? {};
  for (const [field, a] of Object.entries(ab)) {
    const s = sb[field];
    // the bag holds a field's one value as it is, two or more as a list (plugin-hostd docs/formats.md)
    const eq = format === 'lv2' ? same(sorted(a), sorted(s)) : same(a, s);
    out.push({ verdict: eq ? 'equal' : 'differs', format, where: 'bag', word: `extensions.${format}.${field}`, authored: a, scanned: s });
  }
  return out;
}

/** The lines of a whole run: `{ lines, losses, fails }`. */
export function roundTrip(scansDir, { root = ROOT, authoredFiles = {} } = {}) {
  const { inside, outside } = described(root);
  const lines = [], losses = {}, upstream = {};
  let fails = 0;
  for (const stem of inside) {
    const authored = JSON.parse(readFileSync(authoredFiles[stem] ?? join(root, 'plugins', stem, descriptorFile(stem)), 'utf8'));
    for (const format of FORMATS) {
      const file = join(scansDir, `${stem}.${format}.json`);
      if (!existsSync(file)) {
        lines.push(`differs ${format} ${stem}: no scan (${file})`);
        fails++;
        continue;
      }
      for (const r of compareFace(format, authored, JSON.parse(readFileSync(file, 'utf8')))) {
        const show = (v) => (v === undefined ? '-' : JSON.stringify(v));
        if (r.verdict === 'equal') lines.push(`equal ${format} ${stem} ${r.where} ${r.word}`);
        else if (r.verdict === 'lost') {
          lines.push(`lost ${format} ${stem} ${r.where} ${r.word} ${show(r.authored)}`);
          const k = r.word === 'controls[].travel.unit' && format === 'lv2' ? `${format} ${r.word} ${show(r.authored)}` : `${format} ${r.word}`;
          losses[k] = (losses[k] ?? 0) + 1;
        } else if (excused(UPSTREAM[`${format} ${r.word} ${show(r.authored)} ${show(r.scanned)}`], stem, r.where)) {
          const k = `${format} ${r.word} ${show(r.authored)} ${show(r.scanned)}`;
          lines.push(`differs ${format} ${stem} ${r.where} ${r.word}: authored ${show(r.authored)}, scanned ${show(r.scanned)} (upstream: ${UPSTREAM[k].why})`);
          upstream[k] = (upstream[k] ?? 0) + 1;
        } else {
          lines.push(`differs ${format} ${stem} ${r.where} ${r.word}: authored ${show(r.authored)}, scanned ${show(r.scanned)}`);
          fails++;
        }
      }
    }
  }
  for (const stem of outside) lines.push(`outside ${stem}: no descriptor, outside the round trip`);
  const ledger = ledgerLines(losses, hostdVersion());
  lines.push(...ledger.lines);
  fails += ledger.fails;
  for (const [k, { count: named, at, through }] of Object.entries(UPSTREAM)) {
    const count = at?.length ?? named;
    if (through !== undefined) {
      const v = hostdVersion();
      if (v === undefined || compareVersions(v, through) > 0) {
        lines.push(`unnamed upstream ${k}: plugin-hostd ${v ?? '(no version: pkg-config --modversion plugin-hostd)'} is past ${through}, the release the entry holds through: run without it, retire it if no named port differs, else move it on`);
        fails++;
        continue;
      }
    }
    const ok = through === undefined ? upstream[k] === count : upstream[k] > 0 && (count === undefined || upstream[k] <= count);
    lines.push(`${ok ? 'upstream' : 'unnamed upstream'} ${k}: ${upstream[k] ?? 0} word(s)${ok ? '' : `, UPSTREAM names ${count} (fixed upstream? remove the entry)`}`);
    if (!ok) fails++;
  }
  return { lines, losses, fails, inside, outside };
}

if (process.argv[1] && resolve(process.argv[1]) === resolve(new URL(import.meta.url).pathname)) {
  const args = process.argv.slice(2);
  const authoredFiles = {};
  for (let i = 1; i < args.length; i += 2) if (args[i] === '--authored') { const [s, f] = args[i + 1].split('='); authoredFiles[s] = f; }
  const { lines, fails, inside, outside } = roundTrip(args[0], { authoredFiles });
  for (const l of lines) console.log(l);
  const count = (v) => lines.filter((l) => l.startsWith(`${v} `)).length;
  const up = lines.filter((l) => l.startsWith('differs ') && l.includes(' (upstream: ')).length;
  console.log(`round-trip: ${inside.length} plugins, both faces: ${count('equal')} equal, ${count('lost')} lost, ${count('differs') - up} differs, ${up} upstream; ${outside.length} outside (${outside.join(', ')})`);
  if (fails) console.log(`FAIL round-trip: ${fails} difference(s) or unnamed loss(es)`);
  process.exitCode = fails ? 1 : 0;
}
