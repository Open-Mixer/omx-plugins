#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * panel-images.mjs — the picture of every shipped plugin's panel, drawn from its declaration alone, for the parameter
 * reference (docs/plugins/<stem>.md) and every page that shows a plugin.
 *
 *   node tools/panel-images.mjs              write docs/images/panels/<stem>.png for every plugin (make panel-images)
 *   node tools/panel-images.mjs --check      write nothing; exit 1 naming each plugin whose picture is missing or not what
 *                                            its declaration draws, and each picture no declaration draws
 *                                            (make check-panel-images)
 *   node tools/panel-images.mjs --self-test  the check's sabotage: a declaration with a section relabelled draws another
 *                                            picture, and a tree missing a picture is red
 *
 * Each picture is `omx-panel picture` (omx-panel's tools/panel-picture.mjs) over the plugin's descriptor (tools/descriptor.mjs,
 * what plugins/<stem>/<stem>.descriptor.json keeps) and its declared panel (tools/panel-file.mjs): the layout every host
 * draws, each control at its declared default, drawn by omx-panel-render. No host runs and nothing is drawn by hand.
 * The renderer writes its image data uncompressed; the picture kept here is the same pixels deflated by node's zlib, and
 * the check compares pixels, never bytes, so a zlib of another build does not stale it.
 */
import { mkdirSync, mkdtempSync, readdirSync, readFileSync, rmSync, writeFileSync, existsSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { deflateSync, inflateSync } from 'node:zlib';
import { descriptorFile, descriptorText } from './descriptor.mjs';
import { loadDecl } from './gen.mjs';
import { isMain } from './omx-contract.mjs';
import { omxPanel } from './omx-panel.mjs';
import { panelFileText } from './panel-file.mjs';
import { ROOT, describedStems } from './params-doc.mjs';

/** Where the pictures are kept, relative to the repository, and the one picture of a plugin. */
export const IMAGES = 'docs/images/panels';
export const imageOf = (stem) => `${IMAGES}/${stem}.png`;

/** The plugins that are pictured: every described plugin whose declaration draws a panel, by stem. */
export function picturedStems(root = ROOT) {
  return describedStems(root).filter((stem) => loadDecl(join(root, 'plugins', stem)).panel);
}

/** A PNG's size and RGBA pixels: 8-bit RGBA, not interlaced, every scanline filter None (what the renderer and encode() write). */
export function decode(bytes) {
  const b = Buffer.from(bytes);
  let at = 8, w = 0, h = 0;
  const idat = [];
  while (at < b.length) {
    const n = b.readUInt32BE(at), type = b.toString('latin1', at + 4, at + 8), body = b.subarray(at + 8, at + 8 + n);
    if (type === 'IHDR') {
      w = body.readUInt32BE(0); h = body.readUInt32BE(4);
      if (body[8] !== 8 || body[9] !== 6 || body[12] !== 0) throw new Error('panel-images: not an 8-bit RGBA PNG');
    } else if (type === 'IDAT') idat.push(body);
    at += 12 + n;
  }
  const raw = inflateSync(Buffer.concat(idat));
  const data = Buffer.alloc(w * h * 4);
  for (let y = 0; y < h; y++) {
    if (raw[y * (w * 4 + 1)] !== 0) throw new Error('panel-images: a scanline filter other than None');
    raw.copy(data, y * w * 4, y * (w * 4 + 1) + 1, (y + 1) * (w * 4 + 1));
  }
  return { w, h, data };
}

const CRC = Array.from({ length: 256 }, (_, n) => {
  let c = n;
  for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
  return c >>> 0;
});
const crc = (buf) => {
  let c = 0xffffffff;
  for (const x of buf) c = CRC[(c ^ x) & 255] ^ (c >>> 8);
  return (c ^ 0xffffffff) >>> 0;
};
const chunk = (type, body) => {
  const head = Buffer.alloc(8);
  head.writeUInt32BE(body.length, 0);
  head.write(type, 4, 'latin1');
  const tail = Buffer.alloc(4);
  tail.writeUInt32BE(crc(Buffer.concat([Buffer.from(type, 'latin1'), body])), 0);
  return Buffer.concat([head, body, tail]);
};

/** The pixels as a deflated PNG: the same 8-bit RGBA, every scanline filter None. */
export function encode({ w, h, data }) {
  const ihdr = Buffer.alloc(13);
  ihdr.writeUInt32BE(w, 0);
  ihdr.writeUInt32BE(h, 4);
  ihdr[8] = 8; ihdr[9] = 6;
  const raw = Buffer.alloc((w * 4 + 1) * h);
  for (let y = 0; y < h; y++) data.copy(raw, y * (w * 4 + 1) + 1, y * w * 4, (y + 1) * w * 4);
  return Buffer.concat([Buffer.from([0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a]), chunk('IHDR', ihdr),
    chunk('IDAT', deflateSync(raw, { level: 9 })), chunk('IEND', Buffer.alloc(0))]);
}

/** Two pictures draw the same pixels. */
export const samePixels = (a, b) => a.w === b.w && a.h === b.h && a.data.equals(b.data);

/** The picture declaration `d` draws (`d` as tools/gen.mjs loads it; its panel may be replaced by `panel`), as pixels. */
export function draw(d, { panel = d.panel } = {}) {
  const tmp = mkdtempSync(join(tmpdir(), 'omx-panel-images-'));
  try {
    const descriptor = join(tmp, descriptorFile(d.stem));
    writeFileSync(descriptor, descriptorText(d));
    const panelFile = join(tmp, `${d.stem}.panel.json`);
    writeFileSync(panelFile, panelFileText({ ...d, panel }));
    const png = join(tmp, `${d.stem}.png`);
    const r = omxPanel(['picture', '--descriptor', descriptor, '--panel', panelFile, '--stem', d.stem, '--png', png]);
    if (r.status !== 0) throw new Error(`plugins/${d.stem}: omx-panel picture refused it: ${(r.stderr || r.stdout).trim()}`);
    return decode(readFileSync(png));
  } finally {
    rmSync(tmp, { recursive: true, force: true });
  }
}

/** What the pictures kept under `at` (the repository, by default) are against the declarations of `root`:
 * `{ stale, missing, extra, held }`, each a list of paths. `only` limits the plugins drawn (the self-test). */
export function check(root = ROOT, { at = root, only } = {}) {
  const out = { stale: [], missing: [], extra: [], held: [] };
  const stems = picturedStems(root);
  for (const stem of only ?? stems) {
    const file = join(at, imageOf(stem));
    if (!existsSync(file)) { out.missing.push(imageOf(stem)); continue; }
    (samePixels(draw(loadDecl(join(root, 'plugins', stem))), decode(readFileSync(file))) ? out.held : out.stale).push(imageOf(stem));
  }
  const dir = join(at, IMAGES);
  for (const f of existsSync(dir) ? readdirSync(dir) : []) if (!stems.some((s) => `${s}.png` === f)) out.extra.push(`${IMAGES}/${f}`);
  return out;
}

/** Write every picture whose pixels moved (or that is missing); remove those no declaration draws. */
function write(root = ROOT) {
  mkdirSync(join(root, IMAGES), { recursive: true });
  const stems = picturedStems(root);
  for (const stem of stems) {
    const file = join(root, imageOf(stem));
    const drawn = draw(loadDecl(join(root, 'plugins', stem)));
    if (existsSync(file) && samePixels(drawn, decode(readFileSync(file)))) continue;
    writeFileSync(file, encode(drawn));
    console.log(`panel-images: wrote ${imageOf(stem)} (${drawn.w}x${drawn.h})`);
  }
  for (const f of readdirSync(join(root, IMAGES))) {
    if (!stems.some((s) => `${s}.png` === f)) { rmSync(join(root, IMAGES, f)); console.log(`panel-images: removed ${IMAGES}/${f}`); }
  }
  console.log(`panel-images: ${stems.length} plugins, ${stems.length} pictures`);
}

/** The check's sabotage, each arm red where it must be: a declaration with its first section relabelled draws another
 * picture; under a tree with no pictures every plugin is missing; a picture swapped for another plugin's is stale; a
 * picture of no plugin is extra. */
function selfTest(root = ROOT) {
  const stems = picturedStems(root);
  const [one, two] = stems;
  const fail = (why) => { console.error(`panel-images --self-test: FAIL ${why}`); process.exit(1); };
  const d = loadDecl(join(root, 'plugins', one));
  const relabelled = structuredClone(d.panel);
  relabelled.sections[0].label = `${relabelled.sections[0].label} X`;
  const drawn = draw(d);
  if (samePixels(drawn, draw(d, { panel: relabelled }))) fail(`plugins/${one} with its first section relabelled drew the same picture`);
  const tree = mkdtempSync(join(tmpdir(), 'omx-panel-images-tree-'));
  try {
    const none = check(root, { at: tree, only: [one, two] });
    if (none.missing.length !== 2) fail(`a tree with no pictures is missing ${none.missing.length} of 2`);
    mkdirSync(join(tree, IMAGES), { recursive: true });
    writeFileSync(join(tree, imageOf(one)), encode(drawn));
    writeFileSync(join(tree, imageOf(two)), encode(drawn));
    writeFileSync(join(tree, IMAGES, 'omx-nothing.png'), encode(drawn));
    const swapped = check(root, { at: tree, only: [one, two] });
    if (swapped.held.join() !== imageOf(one)) fail(`${imageOf(one)} drawn afresh is not held (${JSON.stringify(swapped)})`);
    if (swapped.stale.join() !== imageOf(two)) fail(`${imageOf(two)} holding ${one}'s picture is not stale (${JSON.stringify(swapped)})`);
    if (swapped.extra.join() !== `${IMAGES}/omx-nothing.png`) fail(`a picture of no plugin is not extra (${JSON.stringify(swapped)})`);
  } finally {
    rmSync(tree, { recursive: true, force: true });
  }
  console.log(`panel-images --self-test: ok, ${one} relabelled draws another picture; missing, swapped (stale) and extra pictures are red`);
}

function main(argv) {
  if (argv.includes('--self-test')) return selfTest();
  if (!argv.includes('--check')) return write();
  const r = check();
  for (const f of r.missing) console.error(`panel-images: MISSING ${f} (run \`make panel-images\`)`);
  for (const f of r.stale) console.error(`panel-images: STALE ${f}: not what its declaration draws (run \`make panel-images\`)`);
  for (const f of r.extra) console.error(`panel-images: EXTRA ${f}: no declaration draws it`);
  if (r.missing.length + r.stale.length + r.extra.length) process.exit(1);
  console.log(`panel-images: ${r.held.length} pictures are what the declarations draw`);
}

if (isMain(import.meta.url)) main(process.argv.slice(2));
