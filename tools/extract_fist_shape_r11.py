#!/usr/bin/env python3
"""Build-time extraction of current native fist contact provenance and receiver."""
from pathlib import Path
import argparse
import subprocess
import sys

def span(text, start, end):
    if text.count(start) != 1 or text.count(end) != 1:
        raise SystemExit("native fist extraction boundary changed: " + start)
    return text[text.index(start):text.index(end)]

def main():
    p = argparse.ArgumentParser()
    p.add_argument("output", type=Path)
    p.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
    args = p.parse_args()
    subprocess.run([sys.executable, str(Path(__file__).with_name("extract_fist_clock_r11.py")), str(args.output), "--source", str(args.source)], check=True)
    source = (args.source / "src/frik_pose_probe.cpp").read_text(encoding="utf-8-sig")
    receiver = span(source, "static int FistContactFrameAdmissionH31(", "static unsigned PhysicalHandReservationsBaseH40()")
    prediction = (args.source / "src/vrik/native_prediction120.inl").read_text(encoding="utf-8-sig")
    authority = span(prediction, "static kcd2vr::contact176::Payload PredictionAuthority176(", "static void PredictionMid120(")
    for name, text in {"receiver": receiver, "authority": authority}.items():
        (args.output / ("fist_shape_r11_" + name + ".inc")).write_text(text, encoding="utf-8", newline="\n")

if __name__ == "__main__":
    main()
