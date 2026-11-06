#!/usr/bin/env python3
"""Copy the actual completed-body reader into a focused host fixture."""
import argparse
import hashlib
import json
from pathlib import Path

p = argparse.ArgumentParser()
p.add_argument("--source", type=Path, required=True)
p.add_argument("--out", type=Path, required=True)
a = p.parse_args()
a.out.mkdir(parents=True, exist_ok=True)
path = a.source / "src/frik_pose_probe.cpp"
text = path.read_text()
start = "static bool ReadCommittedPacketFor6695(CommittedFrame6695& p,bool workstationBody,"
assert text.count(start) == 1
begin = text.index(start)
end = text.index("{", begin) + 1
depth = 1
while depth:
    depth += (text[end] == "{") - (text[end] == "}")
    end += 1
body = text[begin:end] + "\n"
(a.out / "pose_reader_d50.inc").write_text("// Actual production reader; regenerate with extract_pose_reader_d50.py.\n" + body)
(a.out / "pose_reader_provenance_d50.json").write_text(json.dumps({
    "path": "src/frik_pose_probe.cpp",
    "source_sha256": hashlib.sha256(path.read_bytes()).hexdigest(),
    "extracted_sha256": hashlib.sha256(body.encode()).hexdigest(),
    "limits": "Actual reader, Stamp, acquisition contract and SnapshotMailbox; minimal payload fields, context/tracking/native leaves and deterministic read interleavings are fixture-controlled."
}, indent=2) + "\n")
print("D50 extracted the actual completed-body reader.")
