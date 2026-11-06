#!/usr/bin/env python3
"""D85 dice "point to choose": pins + host test + negative controls.

1. Every literal pin and vft slot of src/subsystems/dice/hand_select_d85.h is
   compared with the supplied WHGame.dll (exact SHA/timestamp).
2. The production mod.cpp ray block is extracted verbatim and compiled with the
   production header into tests/dice_hands_d85_test.cpp (explicit native leaves).
3. Negative controls: the unpatched baseline (git HEAD~ or --baseline) has no
   dice hand path and fails; three source mutants of the production header
   (scored die not skipped, no edge-only write, pins not checked) must fail.
Native game execution and the headset are NOT run here.
"""
import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from native_research112 import PE  # noqa: E402

DLL_SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
HEADER = 'src/subsystems/dice/hand_select_d85.h'
BLOCK_START = '        // D85 dice: right hand world ray'
BLOCK_END = '        bool handUseAllowed59c10=false;\n'


def pins(dll_path):
    dll = PE(dll_path)
    assert hashlib.sha256(dll.data).hexdigest() == DLL_SHA and dll.stamp == 0x6A350E20
    text = (ROOT / HEADER).read_text()
    arrays = {m[1]: bytes(int(b, 16) for b in m[2].split(',')) for m in re.finditer(
        r'inline constexpr unsigned char (\w+)\[\]\{((?:0x[0-9A-F]+,?)+)\};', text)}
    table = re.search(r'kPins\[\]\{(.*?)\};', text, re.S)[1]
    checked = []
    for rva, name in re.findall(r'\{(0x[0-9A-F]+),(\w+),sizeof\(\2\)\}', table):
        actual = dll.read(int(rva, 16), len(arrays[name]))
        assert actual == arrays[name], (rva, name, actual.hex())
        checked.append({'rva': rva, 'name': name, 'bytes': actual.hex()})
    assert len(checked) == 14, len(checked)
    slots = re.search(r'kSlots\[\]\{(.*?)\};', text, re.S)[1]
    found = []
    for at, fn in re.findall(r'\{(0x[0-9A-F]+(?:\+0x[0-9A-F]+)?),(0x[0-9A-F]+)\}', slots):
        address = sum(int(x, 16) for x in at.split('+'))
        value = int.from_bytes(dll.read(address, 8), 'little') - dll.base
        assert value == int(fn, 16), (at, hex(value), fn)
        found.append({'slot': at, 'fn': fn})
    assert len(found) == 7, len(found)
    # Constants used by readTurn: RTTI-confirmed class vftables.
    for name, rva, cls in (('kGameModelVft', 0x473F178, b'C_DiceGameModel'), ('kDieModelVft', 0x473F160, b'C_DieModel'),
                           ('kPlayerTurnVft', 0x3F711E0, b'C_PlayerTurnDiceGameState'), ('kHoldingVft', 0x473F1C8, b'C_HoldingDiceGameState'),
                           ('kEntityVft', 0x4072C78, b'CEntity')):
        assert re.search(name + r'=0x%X\b' % rva, text), name
        col = int.from_bytes(dll.read(rva - 8, 8), 'little') - dll.base
        td = int.from_bytes(dll.read(col + 12, 4), 'little')
        assert cls in dll.read(td + 16, 96), (name, dll.read(td + 16, 96))
    # Native leaves in the .inl: setter call sites and entity registry slot.
    inl = (ROOT / 'src/subsystems/dice/hand_select_native_d85.inl').read_text()
    assert 'b+0x2E951BC' in inl and '0x492D8A0' in inl and '0x4077048' in inl and 'vf+0x70' in inl and '0x4B4F98' in inl
    assert int.from_bytes(dll.read(0x4077048 + 0x70, 8), 'little') - dll.base == 0x4B4F98
    for site in (0x202AF04, 0x2E9007C):
        rel = int.from_bytes(dll.read(site + 1, 4), 'little', signed=True)
        assert dll.read(site, 1) == b'\xe8' and site + 5 + rel == 0x2E951BC
    return checked, found


def build(source_root, out, tag, header_text=None, sanitize=True):
    work = out / tag
    work.mkdir(parents=True, exist_ok=True)
    mod = (source_root / 'src/mod.cpp').read_text()
    if BLOCK_START not in mod:
        return {'tag': tag, 'built': False, 'reason': 'mod.cpp has no D85 dice ray block'}
    i = mod.index(BLOCK_START)
    j = mod.index(BLOCK_END, i)
    (work / 'dice_ray_block_d85.inc').write_text(mod[i:j])
    include = source_root / 'src'
    if header_text is not None:
        shadow = work / 'src/subsystems/dice'
        shadow.mkdir(parents=True, exist_ok=True)
        (shadow / 'hand_select_d85.h').write_text(header_text)
        core = work / 'src/core'
        core.mkdir(parents=True, exist_ok=True)
        (core / 'snapshot_mailbox.h').write_text((source_root / 'src/core/snapshot_mailbox.h').read_text())
        include = work / 'src'
    if not (include / 'subsystems/dice/hand_select_d85.h').exists():
        return {'tag': tag, 'built': False, 'reason': 'no hand_select_d85.h'}
    exe = work / 'dice_hands_d85'
    cmd = [os.environ.get('CXX', 'g++'), '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-I', str(include), '-I', str(work),
           str(ROOT / 'tests/dice_hands_d85_test.cpp'), '-o', str(exe)]
    if sanitize:
        cmd[2:2] = ['-fsanitize=address,undefined', '-fno-sanitize-recover=all']
    b = subprocess.run(cmd, capture_output=True, text=True)
    if b.returncode:
        return {'tag': tag, 'built': False, 'reason': b.stderr[-1500:]}
    r = subprocess.run([str(exe)], capture_output=True, text=True)
    return {'tag': tag, 'built': True, 'exit': r.returncode, 'stdout': r.stdout.strip(), 'stderr': r.stderr.strip()[-600:]}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--dll', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--baseline', type=Path, help='unpatched source tree (default: git HEAD~ of this branch exported)')
    a = p.parse_args()
    out = a.out.resolve()
    if out == ROOT or ROOT in out.parents:
        p.error('--out must be outside source')
    out.mkdir(parents=True, exist_ok=True)
    checked, slots = pins(a.dll)
    result = {'pins': checked, 'slots': slots, 'dll_sha256': DLL_SHA}
    result['patched'] = build(ROOT, out, 'patched')
    assert result['patched'].get('exit') == 0, result['patched']
    baseline = a.baseline
    if baseline is None:
        baseline = out / 'baseline_src'
        if not baseline.exists():
            baseline.mkdir()
            rev = subprocess.run(['git', '-C', str(ROOT), 'log', '--format=%H', '--grep', 'D84R1 baseline', '-n', '1'],
                                 capture_output=True, text=True).stdout.strip() or 'HEAD'
            archive = subprocess.run(['git', '-C', str(ROOT), 'archive', rev, 'src'], capture_output=True, check=True)
            subprocess.run(['tar', '-x', '-C', str(baseline)], input=archive.stdout, check=True)
    result['negative_baseline'] = build(baseline, out, 'baseline')
    assert not result['negative_baseline'].get('exit') == 0, 'unpatched baseline must fail'
    header = (ROOT / HEADER).read_text()
    mutants = {
        'scored_die_targeted': ('if(!d.pos||d.index!=int(i)||d.state==2)continue;', 'if(!d.pos||d.index!=int(i))continue;'),
        'cursor_fights_stick': ('    if(target==o.target)return d;\n', ''),
        'pins_ignored': ('    for(const auto& p:kPins)if(!m.same(b+p.rva,p.bytes,p.size))return false;\n', ''),
    }
    result['mutants'] = {}
    for name, (old, new) in mutants.items():
        assert header.count(old) == 1, name
        r = build(ROOT, out, 'mutant_' + name, header.replace(old, new), sanitize=False)
        result['mutants'][name] = r
        assert r.get('built') and r.get('exit') != 0, ('mutant survived', name, r)
    result['native_execution'] = False
    result['headset'] = 'NOT_RUN'
    (out / 'RESULT.json').write_text(json.dumps(result, indent=2) + '\n')
    print(result['patched']['stdout'])
    print('pins %d + slots %d match WHGame; baseline fails (%s); mutants fail: %s' % (
        len(checked), len(slots), result['negative_baseline'].get('reason', 'exit %s' % result['negative_baseline'].get('exit')),
        ', '.join(result['mutants'])))


if __name__ == '__main__':
    main()
