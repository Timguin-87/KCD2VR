"""Read-only retail-byte audit of the automatic reload caller expansion.

Does not load or execute WHGame. No third-party Python packages required.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct


def audit(dll: Path) -> dict:
    data = dll.read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    if digest != "bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7":
        raise ValueError("Unsupported WHGame SHA-256")
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    if struct.unpack_from("<I", data, pe + 8)[0] != 0x6A350E20:
        raise ValueError("Unsupported WHGame timestamp")
    count = struct.unpack_from("<H", data, pe + 6)[0]
    opt_size = struct.unpack_from("<H", data, pe + 20)[0]
    table = pe + 24 + opt_size
    sections = [struct.unpack_from("<IIII", data, table + 40 * i + 8) for i in range(count)]

    def raw(rva):
        for _, va, raw_size, offset in sections:
            if va <= rva < va + raw_size:
                return offset + rva - va
        raise ValueError(f"Unmapped RVA {rva:X}")

    root = Path(__file__).resolve().parents[1]
    source = read_source(root / "src/diagnostics/reload_lifecycle178r36c2.inl")
    pins = source.split("static constexpr ReloadCallerPin37 kReloadCallerPins37[]{", 1)[1].split("struct ReloadSlotPin37", 1)[0]
    results = []
    for addr, expected in re.findall(r'\{(0x[0-9A-Fa-f]+),"([0-9a-f]+)"\}', pins):
        rva, wanted = int(addr, 16), bytes.fromhex(expected)
        if data[raw(rva):raw(rva) + len(wanted)] != wanted:
            raise ValueError(f"Caller pin mismatch {addr}")
        results.append({"rva": addr, "bytes": expected, "match": True})
    if not results:
        raise ValueError("No caller pins extracted")
    base = struct.unpack_from("<Q", data, pe + 24 + 24)[0]
    slots = source.split("static constexpr ReloadSlotPin37 kReloadSlotPins37[]", 1)[1].split(";", 1)[0]
    slot_count = 0
    for addr, target in re.findall(r"\{(0x[0-9A-Fa-f]+),(0x[0-9A-Fa-f]+)\}", slots):
        if struct.unpack_from("<Q", data, raw(int(addr, 16)))[0] != base + int(target, 16):
            raise ValueError(f"Caller slot mismatch {addr}")
        slot_count += 1
    if not slot_count:
        raise ValueError("No caller slots extracted")
    return {"retail_sha256": digest, "timestamp": "0x6A350E20", "caller_pins": results,
            "slot_pins": slot_count, "new_caller": "222CB40",
            "native_trace": "14:08:52.823 manualMode=1 CALLER_NOT_MATCHED; native request 222CB3B->18468BC, return unused; equipment+5C already cleared by caller; no controller+20 access/write introduced"}


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("dll", type=Path)
    parser.add_argument("--out", type=Path)
    args = parser.parse_args()
    result = audit(args.dll)
    if args.out:
        args.out.write_text(json.dumps(result, indent=2) + "\n")
    print(f"PASS {len(result['caller_pins'])} caller pins + {result['slot_pins']} slot pins; exact retail fingerprint")
