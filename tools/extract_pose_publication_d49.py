#!/usr/bin/env python3
"""Extract the live paired-mailbox publication boundary for a focused host check.

The native packet builder and service consumers are explicit test leaves. The
publication scope, retirement, rejection, paired writes, owner guards and success
store are taken verbatim from the selected source, not reimplemented policies.
"""
import argparse
import hashlib
import json
from pathlib import Path

p = argparse.ArgumentParser()
p.add_argument("--source", type=Path, required=True)
p.add_argument("--out", type=Path, required=True)
a = p.parse_args()
a.out.mkdir(parents=True, exist_ok=True)
path = a.source / "src/vrik/coherent_pose.inl"
text = path.read_text()


def braced(start):
    assert text.count(start) == 1, (start, text.count(start))
    lo = text.index(start)
    i = text.index("{", lo) + 1
    depth = 1
    while depth:
        depth += (text[i] == "{") - (text[i] == "}")
        i += 1
    return text[lo:i]


def region(start, end):
    assert text.count(start) == 1, (start, text.count(start))
    lo = text.index(start)
    return text[lo:text.index(end, lo)]


publication = text.index("static void PublishCommitted6689(")
history = text.index("    if(pairedFramePublishedR38&&tx.motionD47.ready)")
late = text.rfind("    if(!CoherentOwnerCurrent57(tx)){rejectPublish169r2(__LINE__);return;}", publication, history)
assert late > publication
parts = {
    "pose_scope_d49.inc": braced("struct FramePublicationScope51") + ";\n",
    "pose_retire_d49.inc": braced("static void RetireTrackedFrame51() noexcept") + "\n",
    "pose_reject_d49.inc": braced("    const auto rejectPublish169r2=[&](unsigned line)") + ";\n",
    "pose_admit_d49.inc": region("    if(!tx.committed||!tx.restored||g_pose_restore_fault6689.load()", "    auto& s=g_pose6689;CommittedFrame6695 packet{};"),
    "pose_pair_d49.inc": region("    FramePublicationScope51 publication51;", "    if(diagnostics::ownership48::active(GetTickCount64()))for(int side48"),
    "pose_late_d49.inc": text[late:text.index("\n", late) + 1],
    "pose_success_d49.inc": region("    g_committed_pose_valid6689.store(true,std::memory_order_release);\n    if(tx.traceH29)tx.tracePublishedH29=true;", "    if(tx.fingerDiagnostic28)tx.fingerPublished28=true;"),
}
# Check every live side-effecting consumer still occurs after the paired guard
# and even publication token. The host leaf can therefore read a completed pair.
pair = text.index("    const bool pairedFramePublishedR38=g_tracked_frame51.publish(tx.frame51);", publication)
finish = text.index("    publication51.finish();", pair)
assert pair < text.index("    if(!pairedFramePublishedR38){rejectPublish169r2(__LINE__);return;}", pair) < finish
assert pair < text.index("    if(!CoherentOwnerCurrent57(tx)){rejectPublish169r2(__LINE__);return;}", pair) < finish
consumers = ["handPresenceH24::SubmitPresenceH24", "PublishGuidedTools55(packet", "g_graspHands13(h)",
             "kcd2vr::physics::committed_push_h21::PublishCommitted", "PhysPublish6698(tx.physicalInput",
             "PublishOwnBladeContact54(tx", "FistPublish101(tx.tracking", "PublishDoorSurface65(packet"]
for consumer in consumers:
    assert text.index(consumer, publication) > finish, consumer
assert finish < text.index("g_graspHands13(h)", publication) < late < history
for name, body in parts.items():
    (a.out / name).write_text("// Verbatim production boundary; regenerate with extract_pose_publication_d49.py.\n" + body)
(a.out / "pose_publication_d49_provenance.json").write_text(json.dumps({
    "source_sha256": hashlib.sha256(path.read_bytes()).hexdigest(),
    "parts_sha256": {name: hashlib.sha256(body.encode()).hexdigest() for name, body in parts.items()},
    "order": "restore/owner admission -> odd token -> body + paired frame -> owner -> even token -> consumers -> late owner -> history/success",
    "limits": "Real SnapshotMailbox/Frame and extracted publication control. Native packet builder and consumers are fixture leaves; failed mailbox return is injected deterministically. No engine, GPU or headset execution."
}, indent=2) + "\n")
print(f"D49 extracted {len(parts)} live publication blocks and checked consumer ordering.")
