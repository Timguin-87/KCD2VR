#!/usr/bin/env python3
"""Summarize V169R2 bow/coverage observations, not visual or native-shot success.

Reads log files and ZIP entries without extracting them. Deduplicates by bytes.
All error/clearance values describe the sampled model/proxy geometry only.
"""
from __future__ import annotations
import argparse
from collections import Counter
import hashlib
import json
import math
from pathlib import Path
import re
import zipfile

PREFIX = "V66.169R2 "
MAX_BYTES = 256 * 1024 * 1024
EXPECTED = "KCD2VR-V66.169R2-BOW-POSE-HILT"


def fields(line: str) -> dict[str, str]:
    return dict(re.findall(r"(\w+)=([^\s]+)", line))


def number(value: str | None) -> float | None:
    try:
        result = float(value)  # type: ignore[arg-type]
        return result if math.isfinite(result) else None
    except (ValueError, TypeError):
        return None


def summarize(inputs: list[tuple[str, bytes]]) -> dict:
    seen: dict[str, str] = {}
    sources, duplicates, observations = [], [], []
    markers: set[str] = set()
    details: Counter = Counter()
    aggregate: Counter = Counter()
    maxima: dict[str, float] = {}
    bow_count = rejected = negative_proxy = unpublished = unowned = 0
    for name, raw in inputs:
        digest = hashlib.sha256(raw).hexdigest()
        if digest in seen:
            duplicates.append({"source": name, "same_bytes_as": seen[digest], "sha256": digest})
            continue
        seen[digest] = name
        lines = raw.decode("utf-8-sig", errors="replace").splitlines()
        local_markers = sorted(set(re.findall(r"KCD2VR-V[\w.\-]+", "\n".join(lines))))
        markers.update(local_markers)
        sources.append({"source": name, "sha256": digest, "lines": len(lines), "markers": local_markers})
        for line_no, line in enumerate(lines, 1):
            if PREFIX not in line:
                continue
            data = fields(line)
            kind = line.split(PREFIX, 1)[1].split(None, 1)[0]
            if kind == "BOW_POSE":
                bow_count += 1
                rejected += data.get("stage") in ("1", "2")
                unpublished += data.get("posePacketPublished") != "1"
                unowned += data.get("rightOwned") != "1"
                if data.get("geometryReady") == "1":
                    for key in ("wristErrorM", "reachExcessM"):
                        value = number(data.get(key))
                        if value is not None:
                            maxima[key] = max(maxima.get(key, 0), value)
                    gap = number(data.get("armTorsoProxyGapM"))
                    negative_proxy += gap is not None and gap < 0
            elif kind == "SWEEP_COVERAGE":
                details[data.get("reason", "UNLABELLED")] += 1
            elif kind == "SWEEP_SUMMARY":
                # Only delta query-call counts; never combine with sparse details.
                for key, value in data.items():
                    if key.isupper() and value.isdigit():
                        aggregate[key] += int(value)
            if kind in ("BOW_POSE", "SWEEP_COVERAGE", "SWEEP_SUMMARY") and len(observations) < 60:
                observations.append({"source": name, "line": line_no, "text": line})
    return {
        "schema": "KCD2VR_BOW169R2_REVIEW_1", "expected_marker": EXPECTED,
        "contains_expected_marker": EXPECTED in markers, "internal_markers": sorted(markers),
        "sources": sources, "duplicate_sources": duplicates,
        "bow": {"sparse_records": bow_count, "rejected_records": rejected,
                "unpublished_pose_packets": unpublished, "right_unowned_records": unowned,
                "negative_final_torso_proxy_gap_records": negative_proxy, "sampled_maxima": maxima},
        "sweeps": {"sparse_detail_counts": dict(details), "aggregate_query_call_deltas": dict(aggregate)},
        "evidence": observations,
        "limits": ["Counts are query calls or sparse observations, not a percentage of all frames.",
                   "A committed pose packet does not prove physical-frame publication, native shot acceptance, or headset appearance.",
                   "Negative final arm-proxy clearance is unresolved; nonnegative sparse proxy clearance does not certify a mesh.",
                   "No R2 records is missing evidence, not a passing bow test. Duplicate renamed logs are not additional sessions.",
                   "TIME_BUDGET and ANGULAR_PREFIX require different changes; no larger budget is automatically recommended."]}


def load(paths: list[Path]) -> list[tuple[str, bytes]]:
    result = []
    for path in paths:
        if zipfile.is_zipfile(path):
            with zipfile.ZipFile(path) as archive:
                for item in archive.infolist():
                    if not item.is_dir() and Path(item.filename).suffix.lower() in (".log", ".txt"):
                        if item.file_size > MAX_BYTES:
                            raise ValueError(f"Log entry exceeds {MAX_BYTES} bytes: {path}!{item.filename}")
                        result.append((f"{path.name}!{item.filename}", archive.read(item)))
        else:
            if path.stat().st_size > MAX_BYTES:
                raise ValueError(f"Log exceeds {MAX_BYTES} bytes: {path}")
            result.append((str(path), path.read_bytes()))
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("logs", nargs="+", type=Path)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--markdown", type=Path)
    args = parser.parse_args()
    try:
        report = summarize(load(args.logs))
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        if args.markdown:
            args.markdown.parent.mkdir(parents=True, exist_ok=True)
            args.markdown.write_text("# V169R2 bow / contact observations\n\n```json\n" +
                json.dumps({k: v for k, v in report.items() if k != "evidence"}, indent=2) + "\n```\n", encoding="utf-8")
    except (OSError, ValueError, zipfile.BadZipFile) as exc:
        parser.exit(2, f"Cannot summarize logs: {exc}\n")


if __name__ == "__main__":
    main()
