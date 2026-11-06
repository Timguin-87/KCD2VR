"""Qualify the read-only dice observer's literal pins against supplied WHGame.

This reads the PE and source only. It does not execute native dice code, prove
runtime object lifetime, or claim a hand-controlled cup implementation.
"""
import argparse
import hashlib
import json
import re
from pathlib import Path
from native_research112 import PE

ROOT = Path(__file__).resolve().parents[1]
DLL_SHA = "bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7"


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--dll", type=Path, required=True)
    p.add_argument("--out", type=Path, required=True)
    args = p.parse_args()
    dll = PE(args.dll)
    assert hashlib.sha256(dll.data).hexdigest() == DLL_SHA
    assert dll.stamp == 0x6A350E20
    source = ROOT / "src/subsystems/dice/diagnostic_d84.inl"
    text = source.read_text()
    arrays = {m[1]: bytes(int(b, 16) for b in m[2].split(",")) for m in re.finditer(
        r"constexpr unsigned char (\w+)\[\]\{((?:0x[0-9A-F]+,?)+)\};", text)}
    pins = []
    for m in re.finditer(r"b\+(0x[0-9A-F]+)\),([a-zA-Z0-9_]+),sizeof\(\2\)", text):
        rva = int(m[1], 16)
        expected = arrays[m[2]]
        actual = dll.read(rva, len(expected))
        assert actual == expected, m[1]
        pins.append({"name": m[2], "rva": m[1], "bytes": actual.hex(),
                     "native_function_range": dll.function(rva)})
    assert len(pins) == 4
    ranges = {}
    for rva in (0x2E7EC5C, 0x2E8FA84, 0x2E8FBFC, 0x2E951BC, 0x2E97F64):
        function = dll.function(rva)
        if function is None:
            ranges[hex(rva)] = {"range": None, "reason": "leaf has no PE unwind range; reviewed bounded disassembly separately"}
            continue
        first, last = function
        ranges[hex(rva)] = {"first": hex(first), "last": hex(last),
                            "sha256": hashlib.sha256(dll.read(first, last-first)).hexdigest()}
    report = {"all_passed": True, "exact_dll_sha256": DLL_SHA,
              "timestamp": hex(dll.stamp), "literal_production_pins": pins,
              "research_function_hashes": ranges,
              "source_sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
              "native_execution": False, "semantic_disassembly_review": "separate evidence/native/*.asm.txt",
              "headset": "NOT_RUN", "cup_control_implemented": False}
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + "\n")
    print("D84 dice snapshot: four literal production pins match exact WHGame; native execution NOT_RUN")


if __name__ == "__main__":
    main()
