#!/usr/bin/env python3
"""Stream KCD2VR logs into an evidence report. No game process access or writes.

Accepts .log/.txt and ZIPs containing logs, deduplicates by full-content SHA256,
uses internal timestamps/build markers, preserves explicit attribution limits.
A native call, group match, or material effect is not inferred damage/success.
"""
from __future__ import annotations
import argparse
from collections import Counter
from datetime import datetime
import hashlib
import json
from pathlib import Path
import re
import sys
from typing import BinaryIO, Iterable
import zipfile

STAMP = re.compile(r"\[(\d{4}-\d\d-\d\d \d\d:\d\d:\d\d\.\d{3})\]")
BUILD = re.compile(r"KCD2VR BUILD\s+(\S+)")
PAIR = re.compile(r"LIVE-PUBLISH pair=(\d+)")
CAPTURE = re.compile(r"CAPTURE domain=(\w+) session=(\d+) state=(BEGIN|END|EXPIRED)")
MAX_EXAMPLES = 24
MAX_LINE_BYTES = 2 * 1024 * 1024

def integer(line: str, field: str, default: int = 0) -> int:
    m = re.search(r"(?:^|[\s{])" + re.escape(field) + r"=(-?\d+)", line)
    return int(m.group(1)) if m else default

class Collector:
    def __init__(self, label: str) -> None:
        self.label = label
        self.counts: Counter[str] = Counter()
        self.examples: dict[str, list[dict]] = {}
        self.builds: list[str] = []
        self.first: str | None = None
        self.last: str | None = None
        self.capture: list[dict] = []
        self.open_capture: dict[str, dict] = {}
        self.pair_previous: tuple[datetime, int, int] | None = None
        self.pair_intervals: list[dict] = []
        self.line_count = 0
        self.complete = True
        self.warnings: list[str] = []

    def example(self, category: str, line: str, n: int, stamp: str | None) -> None:
        rows = self.examples.setdefault(category, [])
        if len(rows) < MAX_EXAMPLES:
            rows.append({"line": n, "timestamp": stamp, "text": line.rstrip()[:1800]})

    def feed(self, line: str, n: int) -> None:
        self.line_count = n
        match = STAMP.search(line)
        stamp = match.group(1) if match else None
        if stamp:
            if self.last and stamp < self.last:
                self.counts["out_of_order_timestamps"] += 1
            if self.first is None:
                self.first = stamp
            self.last = stamp
        match = BUILD.search(line)
        if match:
            build = match.group(1)
            if build not in self.builds:
                self.builds.append(build)
            self.pair_previous = None
        match = CAPTURE.search(line)
        if match:
            domain, session, state = match.groups()
            item = {"domain": domain, "session": int(session), "state": state, "timestamp": stamp, "line": n}
            self.capture.append(item)
            if state == "BEGIN":
                self.open_capture[domain] = item
            else:
                old = self.open_capture.get(domain)
                if old and old["session"] == int(session):
                    del self.open_capture[domain]
        match = PAIR.search(line)
        if match and stamp:
            now, pair = datetime.fromisoformat(stamp), int(match.group(1))
            previous = self.pair_previous
            if previous:
                dt, dp = (now - previous[0]).total_seconds(), pair - previous[1]
                if dt > 0 and dp > 0:
                    self.pair_intervals.append({"from_line": previous[2], "to_line": n, "seconds": dt,
                                               "pairs": dp, "pairs_per_second": round(dp / dt, 3)})
                elif dp <= 0:
                    self.counts["pair_counter_restarts_or_duplicates"] += 1
            self.pair_previous = now, pair, n
        for token, category in (
            ("R3_NATIVE", "r3_native"), ("CLOSE_QUERY", "close_query"),
            ("MASTER_FACTORY", "master_factory"), ("GUARD_MOTION", "guard_motion"),
            ("POSE_VALIDITY", "pose_validity"), ("INPUT_OWNER", "input_owner"),
            ("CAMERA_LOCK_CVAR", "camera_lock_cvar"), ("HAND_CONTACT_OWNER", "hand_owner"),
            ("F8_LIFECYCLE", "interaction_lifecycle"), ("LIFECYCLE168", "interaction_lifecycle168"),
            ("HUD_COST", "hud_cost"), ("V66.168R1 COVERAGE", "coverage168r1"),
            ("HUD_COVERAGE", "hud_coverage168r1"), ("HAND_CONTACT side=", "hand_contact"),
            ("COMBO_EVENT", "combo_event"), ("COMBO_CANDIDATE", "combo_candidate")):
            if token in line:
                self.counts[category] += 1
                self.example(category, line, n, stamp)
        if "MASTER_FACTORY" in line and "nativeGroupMatch=" in line:
            self.counts["master_factory_native_group_match"] += integer(line, "nativeGroupMatch") == 1
            self.counts["master_factory_vr_permit"] += integer(line, "vrPermit") == 1
            if integer(line, "nativeGroupMatch") == 1 and integer(line, "vrPermit") == 0:
                self.counts["master_factory_additional_vr_rejection"] += 1
        if "SUBMIT_HIT" in line:
            # Decode HIT INFO separately: request target and hitInfo target are
            # not silently substituted for one another. Never count PROCESS_HIT
            # again as another damage event.
            self.counts["submit_hit_records"] += 1
            info = re.search(r"hitInfo\{([^}]*)\}", line)
            if not info:
                self.counts["submit_hit_without_decodable_hitinfo"] += 1
            else:
                target = integer(info.group(1), "target", -1)
                dm = re.search(r"\bdmg=(-?(?:\d+\.?\d*|\.\d+))", info.group(1))
                damage = float(dm.group(1)) if dm else None
                if target == 0:
                    self.counts["environment_submit_records"] += 1
                elif target > 0:
                    self.counts["nonzero_target_submit_records"] += 1
                    if damage is not None and damage > 0:
                        self.counts["nonzero_target_positive_damage_records"] += 1
                        causal = integer(line, "prediction") > 0 and integer(line, "sweep") > 0 and integer(line, "redirected") == 1
                        self.counts["positive_damage_with_tracked_prediction_ids" if causal else "positive_damage_without_tracked_prediction_ids"] += 1
                if integer(info.group(1), "death") == 1:
                    self.counts["native_death_flag_records"] += 1
            self.counts["decoded_masterstrike_flag_records"] += integer(line, "masterStrike") == 1
            self.example("submit_hit", line, n, stamp)
        if "droppedTotal=" in line or "DROPPED" in line or "FAULT" in line:
            self.counts["loss_or_fault_records"] += 1
            self.example("loss_or_fault", line, n, stamp)

    def result(self) -> dict:
        warnings = list(self.warnings)
        if self.open_capture:
            warnings.append("File ends with capture BEGIN not followed by matching END/EXPIRED; do not infer native cleanup or complete capture.")
        if not self.capture:
            warnings.append("No V168 capture markers: legacy or uninstrumented log, not proof diagnostics were OFF.")
        if not self.builds:
            warnings.append("No internal build marker; filename is not accepted as build identity.")
        if self.counts["loss_or_fault_records"]:
            warnings.append("Loss/fault records are present; quoted cumulative drop values must not be summed as unique dropped events.")
        # Keep the report bounded even for a long running log; lowest/highest
        # intervals identify relevant ranges while aggregate uses all samples.
        samples = self.pair_intervals
        total_seconds = sum(x["seconds"] for x in samples)
        total_pairs = sum(x["pairs"] for x in samples)
        cadence = {"interval_count": len(samples), "sampled_seconds": round(total_seconds, 3), "sampled_pair_delta": total_pairs,
                   "weighted_pairs_per_second": round(total_pairs / total_seconds, 3) if total_seconds else None,
                   "slowest_intervals": sorted(samples, key=lambda x: x["pairs_per_second"])[:12],
                   "fastest_intervals": sorted(samples, key=lambda x: -x["pairs_per_second"])[:6],
                   "interpretation": "Producer throughput between sparse counters, NOT delivered headset FPS or GPU time."}
        return {"source": self.label, "build_markers": self.builds, "internal_first_timestamp": self.first,
                "internal_last_timestamp": self.last, "lines": self.line_count, "complete_read": self.complete,
                "counts": dict(sorted(self.counts.items())), "capture_transitions": self.capture,
                "captures_open_at_file_end": list(self.open_capture.values()), "cadence": cadence,
                "examples_first_n": self.examples, "warnings": warnings}

def inspect_stream(stream: BinaryIO, label: str, limit: int) -> dict:
    collector = Collector(label)
    h = hashlib.sha256()
    size = 0
    line_number = 0
    while size < limit:
        raw = stream.readline(min(MAX_LINE_BYTES + 1, limit - size))
        if not raw:
            break
        h.update(raw); size += len(raw); line_number += 1
        if len(raw) > MAX_LINE_BYTES and not raw.endswith(b"\n"):
            collector.counts["oversized_lines_skipped"] += 1
            while size < limit and not raw.endswith(b"\n"):
                raw = stream.readline(min(MAX_LINE_BYTES, limit - size))
                if not raw:
                    break
                h.update(raw); size += len(raw)
            collector.line_count = line_number
            continue
        collector.feed(raw.decode("utf-8-sig" if line_number == 1 else "utf-8", errors="replace"), line_number)
    if size >= limit and stream.read(1):
        collector.complete = False
        collector.warnings.append("Size limit reached: only a prefix was examined and hashed.")
    result = collector.result()
    result.update({"bytes_examined": size, "sha256": h.hexdigest(), "hash_scope": "FULL_FILE" if collector.complete else "PREFIX_ONLY"})
    return result

def inspect_paths(paths: Iterable[Path], limit: int = 512 * 1024 * 1024) -> dict:
    reports, errors, duplicates = [], [], []
    hashes: dict[str, str] = {}
    def add(report: dict) -> None:
        if report["complete_read"] and report["sha256"] in hashes:
            duplicates.append({"source": report["source"], "identical_to": hashes[report["sha256"]], "sha256": report["sha256"]})
        else:
            reports.append(report)
            if report["complete_read"]:
                hashes[report["sha256"]] = report["source"]
    for path in paths:
        try:
            if zipfile.is_zipfile(path):
                with zipfile.ZipFile(path) as zf:
                    for entry in zf.infolist():
                        if entry.is_dir() or not entry.filename.lower().endswith((".log", ".txt")):
                            continue
                        label = str(path) + "::" + entry.filename
                        try:
                            with zf.open(entry) as stream:
                                add(inspect_stream(stream, label, limit))
                        except (OSError, RuntimeError, zipfile.BadZipFile) as exc:
                            errors.append({"source": label, "error": str(exc)})
            else:
                with path.open("rb") as stream:
                    add(inspect_stream(stream, str(path), limit))
        except (OSError, RuntimeError, zipfile.BadZipFile) as exc:
            errors.append({"source": str(path), "error": str(exc)})
    return {"schema": "KCD2VR_SESSION_REVIEW_168_v1", "files": reports, "duplicates_not_recounted": duplicates, "errors": errors,
            "interpretation_limits": [
                "Counts describe recorded events, not unique gameplay outcomes unless exact native IDs establish identity.",
                "R3 press/release returned does not prove a kick; attack_special is context-dependent.",
                "Master factory group match/VR permission is NOT final native eligibility, accepted masterstrike, hit, or damage.",
                "Nonzero hit target alone is not proof of NPC class or Henry being the attacker; inspect exact identity in the record.",
                "Tracked prediction IDs strengthen provenance but are not proof of physical contact correctness or full-path coverage.",
                "Lifecycle observation/capture END is not proof of a physical grab or successful native cleanup.",
                "Missing records, quiet/expired captures and losses cannot prove an action never happened."]}

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("logs", nargs="+", type=Path)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--max-mib", type=int, default=512, help="Maximum uncompressed bytes examined per log (default 512 MiB)")
    args = parser.parse_args()
    if args.max_mib < 1:
        parser.error("--max-mib must be positive")
    report = inspect_paths(args.logs, args.max_mib * 1024 * 1024)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"{len(report['files'])} unique logs, {len(report['duplicates_not_recounted'])} duplicate copies, {len(report['errors'])} errors -> {args.out}")
    return 1 if report["errors"] else 0
if __name__ == "__main__":
    sys.exit(main())
