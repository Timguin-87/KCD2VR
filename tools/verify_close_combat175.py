#!/usr/bin/env python3
"""Verify the actual V175 auto-clinch rejection boundary against supplied retail.

Static verification is not hook-installation or headset/runtime acceptance.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse
import hashlib
import json
import pathlib
import re
import struct

EXPECTED = "bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("dll", type=pathlib.Path)
    parser.add_argument("--out", type=pathlib.Path)
    args = parser.parse_args()
    data = args.dll.read_bytes()
    assert hashlib.sha256(data).hexdigest() == EXPECTED, "exact retail SHA mismatch"
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    assert struct.unpack_from("<I", data, pe + 8)[0] == 0x6A350E20
    section_count = struct.unpack_from("<H", data, pe + 6)[0]
    header = pe + 24 + struct.unpack_from("<H", data, pe + 20)[0]
    sections = []
    for index in range(section_count):
        at = header + index * 40
        size, rva, raw_size, raw = struct.unpack_from("<IIII", data, at + 8)
        sections.append((data[at:at + 8].rstrip(b"\0"), rva, size, raw, raw_size))

    def read(rva, length):
        for _, start, _, raw, raw_size in sections:
            if start <= rva and rva + length <= start + raw_size:
                return data[raw + rva - start:raw + rva - start + length]
        raise AssertionError(f"unbacked RVA {rva:X}")

    source = read_source(pathlib.Path(__file__).resolve().parents[1] / "src/vrik/native_bridges168.inl")
    arm = source.split("static bool ArmCloseCombat175()", 1)[1].split("using namespace DetourModKit::hook;Options", 1)[0]
    pins = []
    for rva, values in re.findall(r"\{(0x[0-9A-Fa-f]+),\{([^{}]+)\}\}", arm):
        expected = bytes(int(value, 16) for value in re.findall(r"0x[0-9A-Fa-f]+", values))
        assert len(expected) == 16
        assert read(int(rva, 16), 16) == expected, f"pin mismatch at {rva}"
        pins.append(rva)
    assert len(pins) == 15
    assert struct.unpack('<Q', read(0x4072C78 + 8, 8))[0] == 0x180000000 + 0x1A76130
    assert struct.unpack('<Q', read(0x4072C78 + 0x100, 8))[0] == 0x180000000 + 0x1A74140

    def call(rva, target):
        instruction = read(rva, 5)
        assert instruction[0] == 0xE8
        assert rva + 5 + struct.unpack_from("<i", instruction, 1)[0] == target

    call(0x72C979, 0x937AF4)
    assert read(0x72C97E, 4) == bytes.fromhex("84 c0 74 cb")
    assert 0x72C982 + struct.unpack("<b", bytes([0xCB]))[0] == 0x72C94D
    assert read(0x72CB26, 4) == bytes.fromhex("c6 43 1a 01")
    call(0x72CAAD, 0x9D5994)
    call(0x72CAC3, 0x9D5994)
    call(0x72CAD5, 0x9D5994)
    call(0x72CAFF, 0x686DB0)
    call(0x937B43, 0x937B84)
    call(0x937B6F, 0x937B84)
    action_load = read(0x937B2B, 6)
    assert action_load[:2] == bytes.fromhex("8b 05")
    assert 0x937B31 + struct.unpack_from("<i", action_load, 2)[0] == 0x53204E4

    pdata = next(section for section in sections if section[0] == b".pdata")
    records = struct.iter_unpack("<III", data[pdata[3]:pdata[3] + pdata[4] - pdata[4] % 12])
    boundaries = {start: end for start, end, _ in records}
    assert boundaries.get(0x72C918) == 0x72CB3E
    assert boundaries.get(0x937AF4) == 0x937B84
    result = {
        "retail_sha256": EXPECTED,
        "timestamp": "0x6A350E20",
        "pins_checked": pins,
        "native_auto_update": "72C918..72CB3E",
        "native_pair_query": "937AF4..937B84",
        "hook": "72C97E: native bool true -> false, caller's JE ->72C94D",
        "native_flag_not_written": "self+1A at72CB26",
        "settings_consumers": {"+344": "minimum velocity", "+348": "maximum velocity", "+34C": "movement weight"},
        "scope": "F4 current fresh melee/fists Henry+reciprocal opponent initiation-only radius; existing F10 closed-empty-hand veto preserved",
        "c32_cached_position_proof": {"entity_vft": "4072C78", "id_leaf": "1A76130 reads+C", "world_leaf": "1A74140 returns+58"},
        "c32_radius_default_m": 0.55,
        "c32_native_settings_or_active_paired_actions_modified": False,
        "stand_off_modified": False,
        "runtime_or_headset_verified": False,
    }
    if args.out:
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(json.dumps(result, indent=2) + "\n")
    print("PASS exact-retail C32 close combat: 15 pins, 2 entity vft leafs, 7 calls, 2 pdata extents and native early-return branch; runtime=UNTESTED")


if __name__ == "__main__":
    main()
