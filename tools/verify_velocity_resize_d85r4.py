#!/usr/bin/env python3
"""Verify the exact native backed-resize/lifetime contract; never executes game code."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess

EXPECTED_SHA256 = "bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7"
PROOF_RANGES = (
    (0x73CEF4, 0x17B, "provision_helper"),
    (0x56CD8C, 0xD9, "invalidate_dimensions_and_dispatch_release"),
    (0x1DF3160, 0x1EE, "render_thread_release_dispatch"),
    (0x7AE938, 0x194, "release_texture_device_backing"),
    (0x7AC434, 0x28, "device_texture_delete"),
    (0x1DFDB50, 0x34, "device_texture_deleting_destructor"),
    (0x1E01FC0, 0x8B, "device_texture_destructor"),
    (0x7AD42C, 0x4F, "device_resource_destructor"),
    (0x7AE0FC, 0x60, "device_resource_release"),
    (0x507B10, 0x30, "native_resource_reference_release"),
    (0x7B814C, 0x14B, "dx12_resource_destructor"),
    (0x7B40AC, 0x39B, "native_deferred_resource_retirement"),
    (0x7B3EA4, 0x206, "three_queue_retirement_collector"),
    (0x7AE5AC, 0x51, "creation_temporary_native_reference"),
    (0x1DF75C0, 0xFC, "creation_balances_native_reference"),
    (0x7ACEBC, 0xE5, "creation_builds_device_texture"),
    (0x7B07F0, 0xA9, "creation_copies_texture_metadata"),
    (0x7AD47C, 0x569, "device_texture_creation_initializes_views"),
    (0x7B38D4, 0x435, "eager_default_resource_view_entries"),
    (0x7B6CE8, 0xA5, "resource_view_create_and_append"),
    (0x7B6D90, 0x22C, "resource_view_type_dispatch"),
    (0x7B089C, 0x7F, "creation_publishes_device_backing"),
)


def fnv(data):
    value = 14695981039346656037
    for byte in data:
        value = ((value ^ byte) * 1099511628211) & ((1 << 64) - 1)
    return value


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("binary", type=Path)
    parser.add_argument("--out", type=Path)
    parser.add_argument("--skip-disassembly", action="store_true", help="Verify exact bytes/vtables and save hashes without optional objdump output")
    args = parser.parse_args()
    data = args.binary.read_bytes()
    actual = hashlib.sha256(data).hexdigest()
    if actual != EXPECTED_SHA256:
        raise SystemExit(f"REFUSED: exact retail SHA-256 differs: {actual}")
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    count = struct.unpack_from("<H", data, pe + 6)[0]
    optional = pe + 24
    sections_at = optional + struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<Q", data, optional + 24)[0]
    sections = [struct.unpack_from("<8sIIII", data, sections_at + 40 * i)[1:]
                for i in range(count)]

    def read(rva, size):
        for virtual_size, address, raw_size, offset in sections:
            if address <= rva and rva + size <= address + raw_size:
                start = offset + rva - address
                return data[start:start + size]
        raise ValueError(f"unmapped RVA {rva:#x}+{size:#x}")

    source = Path(__file__).resolve().parents[1] / "src/native_stereo_resource_signatures_v6603.inc"
    declared = re.findall(r"\{(0x[0-9A-Fa-f]+),(\d+),(0x[0-9A-Fa-f]+)ull\}", source.read_text())
    for rva, size, expected in declared:
        got = fnv(read(int(rva, 16), int(size)))
        if got != int(expected, 16):
            raise SystemExit(f"FAIL: runtime guard {rva}/{size}: {got:#x} != {expected}")
    records = []
    for rva, size, label in PROOF_RANGES:
        records.append({"rva": hex(rva), "size": size, "label": label,
                        "fnv1a64": f"0x{fnv(read(rva, size)):016X}"})
    assert struct.unpack("<Q", read(0x3A54170, 8))[0] == base + 0x1DFDB50
    assert struct.unpack("<Q", read(0x3A54400, 8))[0] == base + 0x7B8298
    report = {"binary_sha256": actual, "runtime_range_guards_verified": len(declared),
              "binary_timestamp": hex(struct.unpack_from("<I", data, pe+8)[0]),
              "disassembly": "NOT_RUN_REQUESTED" if args.skip_disassembly else "objdump",
              "proof_ranges": records, "execution": "NONE_STATIC_READ_ONLY"}
    if args.out:
        args.out.mkdir(parents=True, exist_ok=True)
        (args.out / "native_resize_contract.json").write_text(json.dumps(report, indent=2) + "\n")
        disassembly = []
        for rva, size, label in (() if args.skip_disassembly else PROOF_RANGES):
            result = subprocess.run(
                ["objdump", "-d", "-Mintel", f"--start-address={base+rva:#x}",
                 f"--stop-address={base+rva+size:#x}", str(args.binary)],
                capture_output=True, text=True, check=True)
            disassembly.append(f"\n### {label}\n{result.stdout}")
        if not args.skip_disassembly:
            (args.out / "native_resize_disassembly.txt").write_text("".join(disassembly))
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
