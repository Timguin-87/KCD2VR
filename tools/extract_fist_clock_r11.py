#!/usr/bin/env python3
"""Extract current production fist callback bodies for the portable host fixture.

No frozen copy of production logic ships in the test. All native engine accesses
are explicit leaves supplied by fist_clock_r11_test.cpp.
"""
from pathlib import Path
import argparse

def between(text, start, end):
    if text.count(start) != 1 or text.count(end) != 1:
        raise SystemExit("production extraction boundary changed: " + start)
    return text[text.index(start):text.index(end)]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=Path)
    parser.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
    args = parser.parse_args()
    fist = (args.source / "src/vrik/fist_native.inl").read_text(encoding="utf-8-sig")
    clock = (args.source / "src/vrik/native_attack_clock50c2.inl").read_text(encoding="utf-8-sig")
    coherent = (args.source / "src/vrik/coherent_pose.inl").read_text(encoding="utf-8-sig")
    frame = between(fist, "struct FistContactFrameH11 {", "static std::atomic<std::uint64_t> g_fist_seen101")
    # Only the frame, mailboxes and mailbox accessor belong to this fixture.
    last = frame.index("static kcd2vr::core::SnapshotMailbox<FistPeakR10>& FistPeakMailboxR10")
    frame = frame[:frame.index("\n", last) + 1]
    publisher = fist[fist.index("// R11: first real committed contact"):]
    callback = between(clock, "// R11 (fists", "static void MotionClockMid50(")
    commit = between(coherent, "    if(pairedFramePublishedR38&&!tx.reloadWorkingHandXR34){",
                     "    if(pairedFramePublishedR38)CommitSmithContactHistoryR38")
    args.output.mkdir(parents=True, exist_ok=True)
    for name, text in {"frame": frame, "publisher": publisher, "callback": callback, "commit": commit}.items():
        (args.output / ("fist_clock_r11_" + name + ".inc")).write_text(text, encoding="utf-8", newline="\n")

if __name__ == "__main__":
    main()
