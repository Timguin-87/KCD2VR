#!/usr/bin/env python3
"""Summarize actual F10=1 observation records, never infer native kick eligibility.
Animation actor pointers are ISkeletonAnim, input actors C_Player: they are NOT
compared as equal pointers. Both producers independently validate Henry. Matching
below is temporal within one capture session, not proof of causation or damage.
"""
from __future__ import annotations
import argparse, json, re
from pathlib import Path
from typing import Any, Iterable
PREFIX = "V66.101R1-COMBAT_CAPTURE "
KICK = "combat_rg_sz5_az5_natk_kick_nwpn"
STATIC_KICK = {
    "fragment_guid": "9c22a61c-fb64-3ac1-b0b8-1d325a2cced7",
    "input_class_id": 1, "input_class_name": "attack_heavy",
    "attack_type_id": 4, "zone_id": 5, "right_weapon_class_id": 12,
    "right_weapon_class_name": "unarmed",
    "source": "supplied combat_action_attack.xml; one selected authored variant",
    "raw_player_flag": False, "raw_player_flag_semantics": "NOT_DECODED",
    "armed_kick_permission": "NOT_ESTABLISHED",
}

def parse(lines: Iterable[str]) -> list[dict[str, Any]]:
    rows = []
    for number, line in enumerate(lines, 1):
        if PREFIX not in line or ' name="' not in line:
            continue
        content = line.split(PREFIX, 1)[1]
        match = re.search(r' name="([^"]*)"', content)
        if match is None:
            continue
        row: dict[str, Any] = dict(re.findall(r"(\w+)=([^ ]+)", content[:match.start()]))
        try:
            for key in ("session", "kind", "tick", "sample", "mode", "id", "layer", "flags"):
                row[key] = int(row[key])
            row["phase"] = float(row.get("phase", 0))
            row["value"] = float(row.get("value", 0))
        except (ValueError, KeyError):
            continue
        row["name"], row["line"] = match.group(1), number
        rows.append(row)
    return sorted(rows, key=lambda r: (r["session"], r["tick"], r["line"]))

def analyze(lines: Iterable[str]) -> dict[str, Any]:
    lines = list(lines)
    rows = parse(lines)
    kicks = []
    seen: dict[tuple[Any, ...], int] = {}
    for r in rows:
        if r["kind"] != 1 or KICK not in r["name"].lower():
            continue
        key = (r["session"], r.get("actor"), r["id"], r["layer"])
        if key in seen and r["tick"] - seen[key] < 2000:
            seen[key] = r["tick"]
            continue
        seen[key] = r["tick"]
        nearby = [e for e in rows if e["kind"] == 0 and e["session"] == r["session"]
                  and 0 <= r["tick"] - e["tick"] <= 2000]
        progress = [e for e in rows if e["kind"] == 1 and (e["flags"] & 4)
                    and e["session"] == r["session"] and e.get("actor") == r.get("actor")
                    and e["id"] == r["id"] and r["tick"] <= e["tick"] <= r["tick"] + 2000]
        kicks.append({"observation": r, "prior_native_input_records": nearby[-32:],
                      "fifo_progress_records": progress[:24],
                      "input_to_animation_causation": "UNPROVEN_TEMPORAL_ASSOCIATION_ONLY",
                      "accepted_hit": "UNKNOWN", "armed_kick_permission": "NOT_ESTABLISHED"})
    drop = [int(m.group(1)) for line in lines for m in [re.search(r"COMBAT_CAPTURE dropped=(\d+)", line)] if m]
    return {"format": "KCD2VR-kick-capture-review-1", "records": len(rows),
            "native_inputs": sum(r["kind"] == 0 for r in rows), "kick_observations": kicks,
            "punch_requests": [r for r in rows if r["kind"] == 4],
            "last_reported_drops": drop[-1] if drop else None, "static_reference": STATIC_KICK,
            "limits": ["No matching clip does not prove there was no kick.",
                       "Native enqueue/FIFO phase is not hit acceptance or collision proof.",
                       "Input C_Player and animation ISkeletonAnim pointers identify different native objects.",
                       "This tool does not modify configuration, animation, damage, or native permissions."]}

def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    result = analyze(args.log.read_text(encoding="utf-8", errors="replace").splitlines())
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(f"records={result['records']} kick_observations={len(result['kick_observations'])}; accepted_hit=UNKNOWN")
if __name__ == "__main__":
    main()
