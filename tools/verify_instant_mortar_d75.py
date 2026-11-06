"""Read-only exact-retail pins/vtable verification for the D75 mortar adapter.

Does not load or execute game code. Uses the existing exact-fingerprint PE reader.
"""
from pathlib import Path
import argparse
import hashlib
import json
import re
import struct
from smith_native_probe46 import PE, EXPECTED


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("dll", type=Path)
    ap.add_argument("--out", type=Path, required=True)
    args = ap.parse_args()
    root = Path(__file__).resolve().parents[1]
    pe = PE(args.dll)
    header = root / "src/subsystems/alchemy/instant_native_pins_d72.h"
    pins = []
    for m in re.finditer(r"\{(0x[0-9A-Fa-f]+),\{([^}]+)\}\}", header.read_text()):
        rva = int(m[1], 16)
        expected = bytes(int(x.strip(), 16) for x in m[2].split(","))
        actual = pe.read(rva, len(expected))
        assert actual == expected, f"native pin mismatch at {rva:X}"
        pins.append({"rva": hex(rva), "bytes": actual.hex(), "verified": True})
    adapter = root / "src/subsystems/alchemy/instant_native_d72.inl"
    slots = []
    for m in re.finditer(r"\{(0x[0-9A-Fa-f]+),(0x[0-9A-Fa-f]+),(0x[0-9A-Fa-f]+),ai72::Action::(\w+)\}", adapter.read_text()):
        vft, start, end = (int(m[i], 16) for i in range(1, 4))
        assert struct.unpack("<Q", pe.read(vft + 0xF8, 8))[0] == pe.base + start
        assert struct.unpack("<Q", pe.read(vft + 0x100, 8))[0] == pe.base + end
        slots.append({"action": m[4], "vft": hex(vft), "start": hex(start), "end": hex(end), "verified": True})
    assert len(pins) == 30 and len(slots) == 7
    result = {"retail_sha256": EXPECTED, "timestamp": hex(pe.stamp),
              "method": "read-only PE bytes; no native execution",
              "source_sha256": {str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest() for p in (header, adapter)},
              "pins": pins, "vtable_slots": slots}
    args.out.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(f"D75 exact retail: {len(pins)} pins and {len(slots)} Start/End vtable pairs PASS")


if __name__ == "__main__":
    main()
