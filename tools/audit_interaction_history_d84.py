"""Read the canonical latest-50 manifest and preserve interaction evidence.

Counts are recording-line counts, not play sessions or accepted native actions.
Each bucket retains exact original line numbers and source identity.
"""
import argparse
from collections import Counter, deque
import json
from pathlib import Path
import re
import subprocess

PATTERN = (r"BUILD KCD2VR|F5_PROP stage=|INSTANT_PICKUP phase=|F5_PICKUP_USE|F5_PICKUP_REFUSED|"
           r"NATIVE_GRASP (?:phase=|ATTACH_NOT_ARMED|DROP_RESULT|FINISH_CLEANUP_BOUND|QUARANTINE)|"
           r"ALCHEMY_RETURN|ALCHEMY_RESOURCE_EVENT|ALCHEMY_INSTANT_OUTCOME_D79|F5_ALCHEMY_STAGE|"
           r"ALCHEMY_SOUND|MORTAR_(?:RETURN|STOP|POUR|WITHDRAW|HANDOFF)|"
           r"F5_CRAFT_NATIVE.*kind=ALCHEMY|F5_CRAFT_OUTPUT.*gestureAlchemy=1|DICE|dice_")


def field(text, name):
    found = re.search(r"(?:^| )" + name + r"=([^ ]+)", text)
    return found[1] if found else "UNOBSERVED"


def bucket(text):
    if "BUILD KCD2VR" in text:
        return "BUILD:" + text.split("BUILD ", 1)[1].split()[0]
    if "F5_PROP stage=" in text:
        return "PROP:" + ":".join(field(text, n) for n in ("stage", "reason", "side", "nativeResult"))
    if "INSTANT_PICKUP phase=" in text:
        return "INSTANT_PICKUP:" + ":".join(field(text, n) for n in ("phase", "side", "result"))
    if "F5_PICKUP_USE" in text:
        return "PICKUP_USE:" + ":".join(field(text, n) for n in ("hand", "queued", "nativeUse"))
    if "F5_PICKUP_REFUSED" in text:
        return "PICKUP_REFUSED:" + ":".join(field(text, n) for n in ("reason", "side", "bodyVftRva"))
    if "NATIVE_GRASP DROP_RESULT " in text:
        return "NATIVE_DROP:" + ":".join(field(text, n) for n in ("dispatch", "released", "physicsCompletion"))
    if "NATIVE_GRASP " in text:
        return "NATIVE_GRASP:" + ":".join(field(text, n) for n in ("phase", "reason", "side", "acknowledged"))
    if "ALCHEMY_INSTANT_OUTCOME_D79" in text:
        return "ALCHEMY_INSTANT:" + ":".join(field(text, n) for n in ("action", "semanticReadback", "parentRetired", "side"))
    if "ALCHEMY_RETURN" in text:
        return "ALCHEMY_RETURN:" + ":".join(field(text, n) for n in ("type", "status", "nativeDetach", "nativeHome"))
    if "ALCHEMY_RESOURCE_EVENT" in text:
        return "RESOURCE_EVENT:" + ":".join(field(text, n) for n in ("event", "nativeHandler", "semanticEffect"))
    if "F5_ALCHEMY_STAGE" in text:
        return "ALCHEMY_STAGE:" + ":".join(field(text, n) for n in ("class", "childStatus", "event"))
    if "ALCHEMY_SOUND" in text:
        return "ALCHEMY_SOUND:" + ":".join(field(text, n) for n in ("cue", "result", "status"))
    if "F5_CRAFT_OUTPUT" in text:
        return "ALCHEMY_OUTPUT:" + ":".join(field(text, n) for n in ("qualified", "a", "b", "x"))
    if "F5_CRAFT_NATIVE" in text:
        return "ALCHEMY_NATIVE:" + ":".join(field(text, n) for n in ("actionClass", "childStatus", "hold"))
    mortar = re.search(r"MORTAR_(?:RETURN|STOP|POUR|WITHDRAW|HANDOFF)", text)
    if mortar:
        return mortar[0] + ":" + ":".join(field(text, n) for n in ("status", "reason", "result"))
    return "DICE:" + ":".join(field(text, n) for n in ("event", "kind", "result"))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--index", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    index = json.loads(args.index.read_text())
    assert index["selected_count"] == 50 and len(index["selected"]) == 50
    args.out.mkdir(parents=True, exist_ok=True)
    records = []
    all_counts = Counter()
    for ordinal, source in enumerate(index["selected"], 1):
        counts, examples = Counter(), {}
        process = subprocess.Popen(["rg", "-n", "--text", "--no-heading", "--color", "never", "-e", PATTERN, source["path"]], stdout=subprocess.PIPE, text=True, encoding="utf-8", errors="replace")
        for raw in process.stdout:
            number, text = raw.rstrip("\n").split(":", 1)
            key = bucket(text)
            counts[key] += 1
            entry = examples.setdefault(key, {"first": [], "last": deque(maxlen=2)})
            sample = {"line": int(number), "text": text}
            if len(entry["first"]) < 2:
                entry["first"].append(sample)
            entry["last"].append(sample)
        code = process.wait()
        if code not in (0, 1):
            raise SystemExit(f"Cannot scan {source['path']}: rg exit {code}")
        all_counts.update(counts)
        record = {"ordinal": ordinal, "source": source["library_name"], "sha256": source["sha256"],
                  "member": source["member"], "original_path": source["path"],
                  "last_timestamp_edge": source["last_timestamp_edge"], "matched_lines": sum(counts.values()),
                  "counts": dict(counts), "examples": {k: {"first": v["first"], "last": list(v["last"])} for k, v in examples.items()}}
        (args.out / f"{ordinal:02d}_{source['sha256'][:12]}.json").write_text(json.dumps(record, indent=2) + "\n")
        records.append({k: v for k, v in record.items() if k != "examples"})
        print(f"{ordinal:02d}/50 {source['library_name']}: {sum(counts.values())} relevant lines", flush=True)
    report = {"selection_index": str(args.index.resolve()), "records": records, "record_count": len(records),
              "aggregate_line_counts": dict(all_counts), "evidence_limit": "Counts are log rows; native handler-return is not recipe/physical outcome, absence is not failure, content-distinct recordings are not necessarily distinct sessions."}
    (args.out / "SUMMARY.json").write_text(json.dumps(report, indent=2) + "\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
