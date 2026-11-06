#!/usr/bin/env python3
"""Extract the exact hook entry and capture-off native-return path for its fixture.

The diagnostic continuation is outside this fixture. Both production paths call
the same captured grant lambda; this extractor also verifies that connection.
No admission decision in the extracted source is rewritten.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
from pathlib import Path
import hashlib
import json

root = Path(__file__).resolve().parents[1]
source_path = root / "src/frik_pose_probe.cpp"
raw = read_source_bytes(source_path)
source = raw.decode().replace("\r\n", "\n")
head = "static std::uint64_t __fastcall InteractiveActionDetour6689Body(void* actor,const char* name,std::uint32_t targetId,bool force,float speed) noexcept {\n"
tail = "    char action[192]{};bool nameOk=false;"
assert source.count(head) == 1
start = source.index(head) + len(head)
end = source.index(tail, start)
body = source[start:end]
assert "const auto fast=DoorNativeCall66(g_interaction_original6689,actor,name,targetId,force,speed,requestDoor38,requestHand38.side,captureNow175);" in body
assert "grantHandUse178r13(fast);return fast;}" in body
continuation_end = source.index("static std::uint64_t", end) if "static std::uint64_t" in source[end:] else len(source)
assert "grantHandUse178r13(result);" in source[end:continuation_end]

types = []
provenance = []
for path, marker in [
    (source_path, "struct InteractionHandContext158 {"),
    (root / "src/vrik/native_door19c4.inl", "struct DoorRequestWitness38 {"),
]:
    text = read_source(path)
    assert text.count(marker) == 1
    a = text.index(marker)
    b = text.index("{", a) + 1
    depth = 1
    while depth:
        depth += (text[b] == "{") - (text[b] == "}")
        b += 1
    assert text[b] == ";"
    b += 1
    value = text[a:b]
    types.append(value)
    provenance.append({"source": str(path.relative_to(root)), "sha256": hashlib.sha256(value.encode()).hexdigest()})

(root / "tests/generated_door_request38_types.inc").write_text("// Generated; production POD types only.\n" + "\n".join(types) + "\n")
(root / "tests/generated_door_request38.inc").write_text("// Generated exact hook entry and capture-off path; do not edit.\n" + body)
(root / "tests/door_request38_extraction.json").write_text(json.dumps({
    "source": "src/frik_pose_probe.cpp",
    "source_sha256": hashlib.sha256(raw).hexdigest(),
    "excerpt_sha256": hashlib.sha256(body.encode()).hexdigest(),
    "source_lines": [source[:start].count("\n") + 1, source[:end].count("\n")],
    "production_decisions_rewritten": False,
    "covered": "request entry, shared grant lambda, capture-off native call and return",
    "not_executed": "diagnostic continuation, native ArmDoor leaves, DoorNativeCall66 motion scope (separate native_door_motion66 fixture), real game thread",
    "types": provenance,
}, indent=2) + "\n")
print("Extracted exact C38 native request-entry snapshot and capture-off return path")

# C46 uses the same immutable hook entry for its input intent. Extract the
# complete capture helper as well, rather than replacing it with a fixture spy.
door_source = read_source(root / "src/vrik/native_door19c4.inl")
capture_start = door_source.index("static void CaptureDoorRequest46(")
capture_end = door_source.index("static bool ReadDoorInput46(", capture_start)
capture = door_source[capture_start:capture_end]
(root / "tests/generated_door_capture46.inc").write_text("// Generated complete production capture helper.\n" + capture)
(root / "tests/door_capture46_extraction.json").write_text(json.dumps({
    "source": "src/vrik/native_door19c4.inl",
    "source_sha256": hashlib.sha256(read_source_bytes(root / "src/vrik/native_door19c4.inl")).hexdigest(),
    "excerpt_sha256": hashlib.sha256(capture.encode()).hexdigest(),
    "production_decisions_rewritten": False,
    "covered": "complete native-entry semi-door input capture helper",
    "not_executed": "retail native calls; source-current leaf substituted in request fixture",
}, indent=2) + "\n")

# The same door provenance change extends this existing committed-palm accessor.
palm_marker = "bool ReadInteractionPalm59c9("
assert source.count(palm_marker) == 1
palm_start = source.index(palm_marker)
palm_end = source.index("\n#include \"vrik/phys_bodies59c10r8.inl\"", palm_start)
palm = source[palm_start:palm_end] + "\n"
assert "sourceTick=p.stamp.tick" in palm and "p.doorPlayer38" in palm
(root / "tests/generated_door_palm38.inc").write_text("// Generated exact committed-palm accessor; do not edit.\n" + palm)
(root / "tests/door_palm38_extraction.json").write_text(json.dumps({
    "source": "src/frik_pose_probe.cpp", "source_sha256": hashlib.sha256(raw).hexdigest(),
    "excerpt_sha256": hashlib.sha256(palm.encode()).hexdigest(),
    "source_lines": [source[:palm_start].count("\n") + 1, source[:palm_end].count("\n")],
    "production_decisions_rewritten": False,
    "covered": "complete ReadInteractionPalm59c9; optional arguments are declared by actual public header",
    "not_executed": "native packet and tracking providers; math uses portable production pose kernel",
}, indent=2) + "\n")
print("Extracted exact committed-palm optional provenance accessor")
