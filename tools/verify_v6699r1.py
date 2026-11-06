#!/usr/bin/env python3
"""Read-only retail frame-contract and optional runtime hook-correlation check.

Does not load a DLL, install a hook, or execute game code. Reuses the retained
V66.98 physical-method verification and adds the exact physical frame gate.
"""
from __future__ import annotations
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path
from verify_v6698 import run as verify_physical_methods


def verify(dll: Path, root: Path, log: Path | None = None) -> dict:
    physical = verify_physical_methods(dll, root)
    data = dll.read_bytes()
    pe = struct.unpack_from('<I', data, 0x3C)[0]
    section_count = struct.unpack_from('<H', data, pe + 6)[0]
    optional_size = struct.unpack_from('<H', data, pe + 20)[0]
    image_base = struct.unpack_from('<Q', data, pe + 48)[0]
    sections = [struct.unpack_from('<IIII', data, pe + 24 + optional_size + 40*i + 8)
                for i in range(section_count)]

    def read(rva: int, size: int) -> bytes:
        for _virtual_size, virtual_address, raw_size, raw_pointer in sections:
            if virtual_address <= rva and rva + size <= virtual_address + raw_size:
                start = raw_pointer + rva - virtual_address
                return data[start:start+size]
        raise ValueError(f'RVA {rva:#x} has no complete raw representation')

    source = read_source(root / 'src/native_stereo_outer_lab.cpp', encoding='utf-8')

    def address(name: str) -> int:
        match = re.search(r'constexpr\s+\S+\s+' + re.escape(name) +
                          r'\s*=\s*(0x[0-9a-fA-F]+|\d+)(?:ull|u|U|ULL|ll|LL)?\b([^;]*);', source)
        if not match:
            raise ValueError(f'Missing source constant {name}')
        value = int(match[1], 0)
        tail = match[2].strip()
        if tail == '/ sizeof(void*)':
            return value // 8
        if tail:
            raise ValueError(f'Unsupported constant expression {name}: {tail}')
        return value

    checks = []
    for name, array in [
        ('kFrameNexusRva', 'kFrameNexusPrologue'),
        ('kPhase7e8CallsiteRva', 'kPhase7e8Callsite'),
        ('kCameraHandoffCallsiteRva', 'kCameraHandoffCallsite'),
        ('kRendererPhaseCallsiteRva', 'kRendererPhaseCallsite'),
        ('kPhase7f8CallsiteRva', 'kPhase7f8Callsite')]:
        match = re.search(re.escape(array) + r'\[\]\s*=\s*\{([^}]+)', source)
        if not match:
            raise ValueError(f'Missing source byte array {array}')
        expected = bytes(int(v, 16) for v in re.findall(r'0x[0-9a-fA-F]+', match[1]))
        actual = read(address(name), len(expected))
        checks.append(dict(name=name, rva=hex(address(name)), passed=actual == expected,
                           expected=expected.hex(), actual=actual.hex()))
    slot_rva = address('kCCryActionVtableRva') + address('kCCryActionFrameNexusSlot') * 8
    target = struct.unpack('<Q', read(slot_rva, 8))[0] - image_base
    checks.append(dict(name='CCryAction frame vtable slot', rva=hex(slot_rva),
                       target=hex(target), passed=target == address('kFrameNexusRva')))
    correlation = None
    if log:
        text = log.read_text(encoding='utf-8', errors='replace')
        def hook(name: str) -> int:
            match = re.search(r"created inline hook '" + re.escape(name) + r"' at (0x[0-9a-fA-F]+)", text)
            if not match:
                raise ValueError(f'Runtime log has no {name} creation record')
            return int(match[1], 16)
        # The retained exact attachment entry is independently pinned in source.
        runtime_base = hook('kcd2vr_v6688_entity_attachment_update') - 0x4238B8
        camera_address = hook('v6604_camera_consumer')
        correlation = dict(runtime_log_sha256=hashlib.sha256(log.read_bytes()).hexdigest(),
                           runtime_module_base=hex(runtime_base),
                           runtime_camera_hook_address=hex(camera_address),
                           runtime_camera_hook_rva=hex(camera_address-runtime_base),
                           old_broad_gate_camera_rva=hex(address('kCRenderViewCameraBuildRva')),
                           same_entry=(camera_address-runtime_base == address('kCRenderViewCameraBuildRva')),
                           limitation='Log records hook creation; actual in-process patched bytes were not captured.')
    result = dict(dll_sha256=hashlib.sha256(data).hexdigest(), physical_methods=physical,
                  frame_checks=checks, runtime_camera_correlation=correlation,
                  native_execution='NOT_PERFORMED')
    result['passed'] = (all(x['passed'] for x in physical['checks']) and
                        all(x['passed'] for x in checks) and
                        (correlation is None or correlation['same_entry']))
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--dll', required=True, type=Path)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--runtime-log', type=Path)
    parser.add_argument('--report', type=Path)
    args = parser.parse_args()
    try:
        report = verify(args.dll, args.root, args.runtime_log)
    except (OSError, ValueError, KeyError, struct.error) as error:
        parser.exit(2, f'Verification could not complete: {error}\n')
    rendered = json.dumps(report, indent=2) + '\n'
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(rendered, encoding='utf-8')
    print(rendered, end='')
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
