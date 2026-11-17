#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas
#
# published-comp-scan.py: omx-clap-scan states what a published plugin exports through clap.meters/1 and
# org.openmixer.param-contract/1, held by value on omx-comp.clap as this tree builds it. The plugin is this repository's; the scanner is
# omx-clap-host's, the lower layer's.
#
#   published-comp-scan.py <omx-clap-scan> <omx-comp.clap>
#   published-comp-scan.py --self-test <omx-clap-scan> <omx-comp.clap>   each expected value changed once must go red
#
# The field dump (--fields, docs/formats.md of plugin-hostd reads it) carries the same values under the structures'
# own member names: meter:<n> clap_meter_info.{id,name,kind,flags,channel_count,min_value,max_value}, the kind by its enum
# name; plugin omx_clap_plugin_param_contract_ext.{version,fingerprint}; param:<n>
# omx_clap_param_contract_entry.{symbol,unit,scale,step}, unit and scale by their contract names.
#
# omx-comp declares four meters, in this order (its declaration's `meters`, the CLAP meter id the index): compGr, a
# linked gain reduction; compIn and compOut, stereo levels; compKey, the key's level. Its contract states each
# parameter's symbol, unit, scale and step. The scan must print both, as text and as JSON, with these values; a scan
# that reads neither (every omx-clap-scan up to 0.31.1) fails here, on the first missing word.
# Exit 0 held; 1 a value missing or different (self test: a changed value was not found); 2 usage or the scan failed.
import json
import re
import subprocess
import sys

PLUGIN_ID = 'org.openmixer.comp'
# id, name, kind, channels: the meters as omx-comp declares them
METERS = [(0, 'compGr', 'gain-reduction', 1), (1, 'compIn', 'level', 2), (2, 'compOut', 'level', 2),
          (3, 'compKey', 'level', 1)]
# parameter name, contract symbol, unit, scale: the parameters the panel's dynamics roles read (Ratio's unit is ratio,
# 258, from omx-plugins 0.19.3: before it the unit was none and the dynamics role withheld)
CONTRACT = [('Threshold', 'thresholdDb', 'db', 'stepped'), ('Ratio', 'ratio', 'none', 'linear'),
            ('Attack', 'attackMs', 'ms', 'linear'), ('Release', 'releaseMs', 'ms', 'stepped')]

failures = []


def expect(ok, what):
    if not ok:
        failures.append(what)


def scan(prog, path, *args):
    out = subprocess.run([prog, *args, path], capture_output=True, text=True)
    if out.returncode != 0:
        sys.exit(f'published-comp-scan: {prog} {" ".join(args)} {path} exited {out.returncode}: {out.stderr.strip()[:300]}')
    return out.stdout


def held(prog, path, METERS, CONTRACT):
    """The failures of the scan of path against the expected meters and contract rows (empty: held)."""
    del failures[:]
    doc = json.loads(scan(prog, path, '--json'))
    plugins = [p for f in doc.get('files', []) for p in f.get('plugins', []) if p.get('id') == PLUGIN_ID]
    if len(plugins) != 1:
        sys.exit(f'published-comp-scan: {path} holds no single {PLUGIN_ID}')
    p = plugins[0]

    # JSON: the meters, by value
    meters = p.get('meters')
    expect(isinstance(meters, list), 'json: the plugin has no "meters" list')
    meters = meters if isinstance(meters, list) else []
    got = [(m.get('id'), m.get('name'), m.get('kind'), m.get('channels')) for m in meters]
    expect(got == METERS, f'json: meters {got}, expected {METERS}')
    for m in meters:
        lo, hi = m.get('min'), m.get('max')
        expect(isinstance(lo, (int, float)) and isinstance(hi, (int, float)) and lo < hi,
               f'json: meter {m.get("name")} has no display range min < max ({lo}, {hi})')

    # JSON: the contract, whole and per parameter
    pc = p.get('param_contract')
    expect(isinstance(pc, dict), 'json: the plugin has no "param_contract"')
    pc = pc if isinstance(pc, dict) else {}
    expect(isinstance(pc.get('version'), int) and pc.get('version') >= 1, f'json: contract version {pc.get("version")}')
    expect(isinstance(pc.get('fingerprint'), str) and re.fullmatch(r'[0-9a-f]{64}', pc.get('fingerprint') or ''),
           f'json: contract fingerprint {pc.get("fingerprint")!r} is not 64 lowercase hex')
    by_name = {q.get('name'): q for q in p.get('params', [])}
    for name, symbol, unit, scale in CONTRACT:
        c = (by_name.get(name) or {}).get('contract')
        expect(isinstance(c, dict), f'json: parameter {name} has no "contract"')
        c = c if isinstance(c, dict) else {}
        expect((c.get('symbol'), c.get('unit'), c.get('scale')) == (symbol, unit, scale),
               f'json: parameter {name} contract {(c.get("symbol"), c.get("unit"), c.get("scale"))}, '
               f'expected {(symbol, unit, scale)}')
    expect(all(isinstance(q.get('contract'), dict) for q in p.get('params', [])),
           'json: a parameter clap.params lists has no contract entry (the contract is whole or absent)')

    # the field dump: the same values under the structures' member names
    fields = {}
    for line in scan(prog, f'clap:{path}#{PLUGIN_ID}', '--fields').splitlines():
        parts = line.split('\t')
        if len(parts) == 3 and not line.startswith('#'):
            fields.setdefault((parts[0], parts[1]), parts[2])
    kinds = {'gain-reduction': 'CLAP_METER_KIND_GAIN_REDUCTION', 'level': 'CLAP_METER_KIND_LEVEL'}
    for mid, name, kind, ch in METERS:
        rec = f'meter:{mid}'
        got = tuple(fields.get((rec, f'clap_meter_info.{k}')) for k in ('id', 'name', 'kind', 'channel_count'))
        expect(got == (str(mid), name, kinds[kind], str(ch)),
               f'fields: {rec} clap_meter_info id/name/kind/channel_count {got}, expected {(str(mid), name, kinds[kind], str(ch))}')
    expect(re.fullmatch(r'[0-9a-f]{64}', fields.get(('plugin', 'omx_clap_plugin_param_contract_ext.fingerprint')) or ''),
           'fields: no plugin omx_clap_plugin_param_contract_ext.fingerprint')
    ids = {fields.get((r, 'clap_param_info.name')): r for (r, k) in fields if k == 'clap_param_info.name'}
    for name, symbol, unit, scale in CONTRACT:
        rec = ids.get(name)
        got = tuple(fields.get((rec, f'omx_clap_param_contract_entry.{k}')) for k in ('symbol', 'unit', 'scale'))
        expect(rec is not None and got == (symbol, unit, scale),
               f'fields: {rec or name} omx_clap_param_contract_entry symbol/unit/scale {got}, expected {(symbol, unit, scale)}')

    # text: the same words, one line per meter and per parameter's contract
    text = scan(prog, path)
    for mid, name, kind, ch in METERS:
        expect(re.search(rf'^\s+meter {mid} "{name}" {kind} {ch} channels? \[', text, re.M),
               f'text: no line "meter {mid} "{name}" {kind} {ch} channel(s) [min .. max]"')
    expect(re.search(r'^\s+contract version \d+ fingerprint [0-9a-f]{64}$', text, re.M),
           'text: no line "contract version <n> fingerprint <sha256>"')
    for name, symbol, unit, scale in CONTRACT:
        expect(re.search(rf'^\s+param \d+ {name} .*\n\s+contract {symbol} {unit} {scale} step ', text, re.M),
               f'text: parameter {name} is not followed by "contract {symbol} {unit} {scale} step <s>"')

    return list(failures)


def main(argv):
    selftest = argv[1:2] == ['--self-test']
    args = argv[2:] if selftest else argv[1:]
    if len(args) != 2:
        print('usage: published-comp-scan.py [--self-test] <omx-clap-scan> <omx-comp.clap>', file=sys.stderr)
        return 2
    prog, path = args
    if selftest:
        # the control: the declared values hold, then each one changed on its own is found
        if held(prog, path, METERS, CONTRACT):
            print('FAIL the control: the declared values do not hold')
            return 1
        arms = [('a meter id', [(9,) + METERS[0][1:]] + METERS[1:], CONTRACT),
                ('a meter name', [METERS[0][:1] + ('compX',) + METERS[0][2:]] + METERS[1:], CONTRACT),
                ('a meter kind', [METERS[0][:2] + ('level',) + METERS[0][3:]] + METERS[1:], CONTRACT),
                ('a meter channel count', METERS[:3] + [METERS[3][:3] + (2,)], CONTRACT),
                ('a contract symbol', METERS, [CONTRACT[0][:1] + ('threshold',) + CONTRACT[0][2:]] + CONTRACT[1:]),
                ('a contract unit', METERS, CONTRACT[:1] + [CONTRACT[1][:2] + ('none',) + CONTRACT[1][3:]] + CONTRACT[2:]),
                ('a contract scale', METERS, CONTRACT[:3] + [CONTRACT[3][:3] + ('linear',)])]
        bad = 0
        for what, meters, contract in arms:
            if held(prog, path, meters, contract):
                print(f'ok   {what} changed is found')
            else:
                print(f'FAIL {what} changed is not found')
                bad = 1
        return bad
    found = held(prog, path, METERS, CONTRACT)
    for f in found:
        print(f'FAIL {f}')
    print(f'published-comp-scan: {len(found)} failure(s) on {path}' if found
          else f'published-comp-scan: held on {path} ({len(METERS)} meters, {len(CONTRACT)} contract rows by value)')
    return 1 if found else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
