"""Run the actual D72 native pickup adapter with substituted native leaves.

Checks exact-retail code/data pins separately. Does not execute WHGame or claim
headset, renderer, physics integration or Windows SEH validation.
"""
import argparse
import hashlib
import json
import re
import struct
import subprocess
from pathlib import Path
from native_research112 import PE

ROOT = Path(__file__).resolve().parents[1]
DLL_SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--dll', type=Path, required=True)
    parser.add_argument('--build-dir', type=Path, required=True)
    parser.add_argument('--compiler', default='g++')
    args = parser.parse_args()
    build = args.build_dir.resolve()
    assert build != ROOT and ROOT not in build.parents
    build.mkdir(parents=True, exist_ok=True)
    dll = PE(args.dll)
    assert hashlib.sha256(dll.data).hexdigest() == DLL_SHA
    assert dll.stamp == 0x6A350E20
    adapter = ROOT / 'src/vrik/instant_pickup_native_d72.inl'
    text = adapter.read_text()
    pins = []
    for match in re.finditer(r'\{(0x[0-9A-F]+),\{((?:0x[0-9A-F]+,?)+)\}\}', text):
        rva = int(match[1], 0)
        value = bytes(int(part, 0) for part in match[2].split(',') if part)
        assert dll.read(rva, len(value)) == value, hex(rva)
        pins.append({'rva': hex(rva), 'bytes': value.hex()})
    assert len(pins) == 15
    tables = {
        0x4001F20: 0x33CA59C,  # real Henry service method +10
        0x3B5BE20: 0x6C7724,  # native parent destruction invalidation
        0x3B5BF58: 0x8C8910,  # actual immediate execute method
        0x3B5BF88: 0x8C9E24,  # request adapter
        0x3B5C040: 0x8C9F08,  # typed PickUpRequest factory
        0x3B55EB8: 0x8CA6BC,  # item handling immediate transfer
        0x3FCF7E8: 0x3DBE54,  # control destroy -> action vft+0
    }
    for slot, target in tables.items():
        assert struct.unpack('<Q', dll.read(slot, 8))[0] - dll.base == target, hex(slot)
    # Exact native semantic routes, independent of the host substitutes.
    assert 0x8CA7D3 in dll.calls(0x8CD484)
    assert 0x8CA7E9 in dll.calls(0x8CF6FC)
    assert dll.read(0x3992B7, 7) == bytes.fromhex('48 ff a0 38 01 00 00')
    assert dll.read(0x3DBE54, 12) == bytes.fromhex('48 83 c1 10 33 d2 48 8b 01 48 ff 20')
    test = ROOT / 'tests/instant_pickup_d72_test.cpp'
    exe = build / test.stem
    command = [args.compiler, '-std=c++20', '-O0', '-g0', '-pthread', '-I'+str(ROOT/'src'), str(test), '-o', str(exe)]
    compile_result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    run_result = subprocess.run([str(exe)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True) if compile_result.returncode == 0 else None
    log = compile_result.stdout + (run_result.stdout if run_result else '')
    (build / 'adapter.log').write_text(log)
    sources = [adapter, test, Path(__file__), ROOT/'src/core/instant_pickup_d72.h', ROOT/'src/core/mutual_admission_t25.h',
        ROOT/'src/subsystems/throw_drop/hand_claim_t24.h', ROOT/'src/vrik/staged_pickup596c2.inl',
        ROOT/'src/diagnostics/interaction_lifecycle165.inl', ROOT/'src/native_grasp59c13.inl',
        ROOT/'src/action_time_owner59c11.cpp', ROOT/'src/action_time_owner59c11.h', ROOT/'src/frik_pose_probe.cpp']
    report = {
        'all_passed': compile_result.returncode == 0 and run_result is not None and run_result.returncode == 0,
        'compile_exit': compile_result.returncode, 'run_exit': run_result.returncode if run_result else None,
        'native_execution': False, 'Windows_SEH_tested': False, 'exact_dll_sha256': DLL_SHA,
        'native_function_pins': pins, 'native_vft_targets': {hex(k): hex(v) for k,v in tables.items()},
        'adapter_log': str(build/'adapter.log'),
        'source_sha256': {str(p.resolve().relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in sources},
        'limits': ['Native scheduling, approval, transfer, notice and physics leaves are explicit host substitutes.',
                   'The native no-child route is independently qualified by exact retail disassembly and pins.',
                   'Tests cover allocation reuse, native refusal, coherent stale/released input, native one-shot stow, partial fault, captured policy, exclusive hand ownership and deferred teardown.',
                   '1000 coordinated concurrent metadata admissions test pickup versus throwing; no native game calls run in these races.',
                   'Windows DLL compilation is a separate validation gate. In-game pickup timing and physical object response require runtime validation.'],
    }
    (build/'report.json').write_text(json.dumps(report, indent=2)+'\n')
    print(log.strip())
    print('D72 exact-retail pickup pins:', len(pins), 'function sites,', len(tables), 'native vft targets')
    return 0 if report['all_passed'] else 1

if __name__ == '__main__':
    raise SystemExit(main())
