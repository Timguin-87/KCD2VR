"""Extract the current production camera-reader/settings boundary for host tests."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib
import json

root = Path(__file__).resolve().parents[1]
source = read_source(root / "src/frik_pose_probe.cpp")
context_source = read_source(root / "src/vrik/tool_attachment59c10.inl")
parts = [context_source[context_source.index("static const char* WorkstationContextRefusal19c4r2("):
                        context_source.index("static const char* WorkstationToolReason19c4r2(")]]
for start, end in (("static bool ReadCommittedPacketFor6695(", "static bool ReadF12HandDeltaV6557("),
                   ("void SetSmoothBody51(", "void SetHeadRaiseMax59c9("),
                   ("static kcd2vr::core::SnapshotMailbox<kcd2vr::camera_hold_e1::Sample> g_camera_hold_e1;", "bool MotionOwnedCamera47(")):
    a = source.index(start)
    b = source.index(end, a)
    parts.append(source[a:b])
body = "\n".join(parts)
(root / "tests/generated_tracked_frame51.inc").write_text(
    "// Exact current production functions; regenerate, do not hand-edit.\n" + body)
(root / "tests/tracked_frame51_provenance.json").write_text(json.dumps({
    "source": "src/frik_pose_probe.cpp",
    "sha256": hashlib.sha256(body.encode()).hexdigest(),
    "native_leaves": "None executed; current packet, tracking/context, and validated ReadModelWorld6692 are explicit synthetic leaves",
    "context_source": "src/vrik/tool_attachment59c10.inl",
    "functions": ["WorkstationContextRefusal19c4r2", "ReadCommittedPacketFor6695", "ReadCommittedPacket6695", "SetSmoothBody51", "SmoothBody51", "TrackedFrameCamera55", "TrackedFrameCamera51"],
}, indent=2) + "\n")
