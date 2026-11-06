#!/usr/bin/env python3
"""Read-only retail/source pin audit; never loads or executes WHGame.dll."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib
import json
import re
import struct
import argparse

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1])
parser.add_argument('--native',type=Path,default=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll'))
parser.add_argument('--out',type=Path,required=True)
args=parser.parse_args()
SOURCE=args.source.resolve()
NATIVE=args.native.resolve()
data = NATIVE.read_bytes()
source = read_source(SOURCE / "src/interaction_alignment.cpp")
retail = read_source(SOURCE / "src/retail_build112.cpp")
expected_sha = "bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7"
pe = struct.unpack_from("<I", data, 0x3C)[0]
assert data[pe:pe+4] == b"PE\0\0"
count = struct.unpack_from("<H", data, pe+6)[0]
timestamp = struct.unpack_from("<I", data, pe+8)[0]
optional = pe+24
optional_size = struct.unpack_from("<H", data, pe+20)[0]
image_base = struct.unpack_from("<Q", data, optional+24)[0]
sections = []
for i in range(count):
    section = optional + optional_size + i*40
    _, va, size, raw = struct.unpack_from("<IIII", data, section+8)
    sections.append((va, va+size, raw))

def offset(rva, length=1):
    for start, end, raw in sections:
        if start <= rva and rva+length <= end:
            return raw+rva-start
    raise ValueError(f"Unmapped file RVA {rva:x}+{length}")

def bytes_at(rva, length):
    start = offset(rva, length)
    return data[start:start+length]

def source_bytes(name, text=source):
    match = re.search(r"\b" + re.escape(name) + r"(?:\[\])?\s*\{([^}]+)\}", text)
    if not match:
        raise ValueError(f"Missing source array {name}")
    return bytes(int(value, 16) for value in re.findall(r"0x([0-9A-Fa-f]{2})\b", match[1]))

checks = []
def check(name, ok, **detail):
    checks.append({"name": name, "passed": bool(ok), **detail})

actual_sha = hashlib.sha256(data).hexdigest()
check("supplied_dll_sha256", actual_sha == expected_sha, sha256=actual_sha)
check("source_retail_sha256_guard", source_bytes("expected", retail).hex() == expected_sha)
check("timestamp", timestamp == 0x6A350E20, actual=hex(timestamp))
check("image_base", image_base == 0x180000000, actual=hex(image_base))
for name, rva in (
    ("directEmit54r1", 0x6404CA),
    ("directPolicy54r1", 0x640124),
    ("directOwner54r1", 0x63FA3A),
    ("directDescriptor54r1", 0x63FED7),
    ("directCopy54r1", 0x640235),
):
    expected = source_bytes(name)
    actual = bytes_at(rva, len(expected))
    check(name, actual == expected, rva=hex(rva), bytes=len(expected), hex=actual.hex())
for name, table, slot, target in (
    ("native_object_policy_accept", 0x3A69D90, 0x60, 0x8AC3C8),
    ("native_object_policy_radius", 0x3A69D90, 0x38, 0x82CDBC),
    ("existing_entity_lookup", 0x4077048, 0x70, 0x4B4F98),
    ("existing_entity_class_getter", 0x4072C78, 0x18, 0x19A2A90),
    ("existing_class_name_getter", 0x3B282A0, 0x10, 0x638480),
):
    actual = struct.unpack("<Q", bytes_at(table+slot, 8))[0]
    check(name, actual == image_base+target, table=hex(table), slot=hex(slot), target_rva=hex(actual-image_base))
pdata, length = struct.unpack_from("<II", data, optional+112+3*8)
owner = None
for start in range(offset(pdata), offset(pdata)+length, 12):
    begin, end, unwind = struct.unpack_from("<III", data, start)
    if begin <= 0x6404E1 < end:
        owner = {"begin": hex(begin), "end": hex(end), "unwind": hex(unwind)}
        break
check("native_selector_unwind_owner", owner is not None and owner["begin"] == "0x63f9ac" and owner["end"] == "0x6405ee", owner=owner)
result = {
    "kind": "READ_ONLY_DISK_AND_SOURCE_PIN_AUDIT",
    "native_execution": False,
    "native_file": str(NATIVE),
    "source_file": str(SOURCE / "src/interaction_alignment.cpp"),
    "checks": checks,
    "passed": sum(row["passed"] for row in checks),
    "total": len(checks),
    "limits": "Byte and file identity only. Payload/register semantics are independently reviewed disassembly in sibling ASM files; no retail hook or headset execution occurs here.",
}
args.out.write_text(json.dumps(result, indent=2)+"\n")
print(f"direct54r1 retail/source audit: {result['passed']}/{result['total']} passed; native_execution=false")
raise SystemExit(0 if result["passed"] == result["total"] else 1)
