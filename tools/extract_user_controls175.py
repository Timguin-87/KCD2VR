#!/usr/bin/env python3
"""Compile current mod.cpp key dispatch, without rewriting its decisions.

Only the surrounding worker/device environment is supplied by the host fixture.
All selected C++ clauses, edge publication and mode lambdas are copied verbatim.
Missing or ambiguous anchors fail extraction instead of testing stale copies.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source_bytes
from pathlib import Path
import hashlib
import json
import re

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/mod.cpp"
raw = read_source_bytes(SOURCE)
source = raw.decode("utf-8").replace("\r\n", "\n")
parts = []


def unique(anchor):
    hits = [m.start() for m in re.finditer(re.escape(anchor), source)]
    if len(hits) != 1:
        raise RuntimeError(f"Expected one production anchor ({len(hits)}): {anchor}")
    return hits[0]


def closing_brace(start):
    """Find a C++ block's end while ignoring comments and quoted braces."""
    begin = source.index("{", start)
    depth = 0
    quote = None
    comment = None
    i = begin
    while i < len(source):
        c = source[i]
        pair = source[i:i+2]
        if comment == "line":
            if c == "\n":
                comment = None
        elif comment == "block":
            if pair == "*/":
                comment = None
                i += 1
        elif quote:
            if c == "\\":
                i += 1
            elif c == quote:
                quote = None
        elif pair == "//":
            comment = "line"
            i += 1
        elif pair == "/*":
            comment = "block"
            i += 1
        elif c in ('"', "'"):
            quote = c
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return i + 1
        i += 1
    raise RuntimeError("Unterminated production block")


def keep(name, start, end):
    parts.append((start, end, name))


def block(name, anchor, semicolon=False):
    start = unique(anchor)
    end = closing_brace(start)
    if semicolon:
        if source[end] != ";":
            raise RuntimeError(f"Missing lambda terminator: {name}")
        end += 1
    keep(name, start, end)


start = unique("static diagnostics::keys168r1::Edges captureKeys;")
end = source.index("diagnostics::capture168::poll(GetTickCount64());", start)
end += len("diagnostics::capture168::poll(GetTickCount64());")
keep("real_key_edges_and_expiry", start, end)
block("F2_hud_only", "if(capturePress.hud){")
block("read_current_mode", "auto read_shortcut167r1=[]() noexcept -> kcd2vr::user175::Configuration", True)
block("apply_explicit_mode", "auto apply_user_mode175=[&](kcd2vr::user175::Mode requested)", True)
block("F3_recorder", "if(capturePress.combat){")
block("common_calibration_start", "auto start_guided_vrik_calibration=[&]() noexcept", True)
block("Shift_F8_start_or_cancel", "if(!matrix_active&&f8_down&&!f8_cal_key_prev&&shift_down){")
start = unique("f8_cal_key_prev=f8_down;")
end = start + len("f8_cal_key_prev=f8_down;")
keep("real_calibration_key_history", start, end)
start = unique("const bool recalibrate_req=vrik_recalibrate_requested_.exchange(false,std::memory_order_acq_rel);")
end = source.index(";", start) + 1
keep("menu_calibration_request", start, end)
block("menu_uses_common_calibration_start", "if(recalibrate_req){")
start = unique("const int requestedF10=vrik_menu_mode_requested_.exchange(-1,std::memory_order_acq_rel);")
end = source.index(";", start) + 1
keep("menu_gameplay_request", start, end)
start = unique("if(requestedF10==0){")
end = closing_brace(unique("} else if(requestedF10>0){"))
keep("menu_gameplay_selection", start, end)
block("calibration_progress_and_result", "if(f8_cal_stage>0){")
block("calibration_result_UI_expiry", "if(vrik_cal_complete_until_ms && ladder_now_ms>=vrik_cal_complete_until_ms){")
block("F4_combat", "if(!matrix_active&&f4_down&&!f4_down_prev){")
start = unique("frik_pose_probe::ServiceInteractionExperiment36();")
keep("F5_expiry_and_prerequisites", start, start + len("frik_pose_probe::ServiceInteractionExperiment36();"))
block("F5_recorder", "if(capturePress.interaction){")
block("F10_hands", "if(!matrix_active && f10_down && !f10_down_prev){")
start = unique("touch_bindings_.SetVrikInputLabStage(0);")
end = source.index("touch_bindings_.SetRightGripFreeLookEnabled(", start)
end = source.index(";", end) + 1
keep("current_hand_input_ownership", start, end)
block("F11_recorder", "if(!matrix_active && f11_down && !f11_down_prev){")
block("F12_router_and_shift_weapons", "if(!matrix_active && f12_down && !f12_down_prev){")
start = source.index("if(combat_diag_probe_installed){", parts[-1][1])
keep("observer_publication_follows_live_lease", start, closing_brace(start))
start = unique("f4_down_prev=f4_down; f6_down_prev=f6_down; f7_down_prev=f7_down;")
end = source.index("f12_down_prev=f12_down;", start) + len("f12_down_prev=f12_down;")
keep("real_end_of_poll_key_history", start, end)
parts.sort()
for previous, current in zip(parts, parts[1:]):
    if previous[1] > current[0]:
        raise RuntimeError("Overlapping production excerpts")

output = ROOT / "tests/generated_user_controls175.inc"
manifest = ROOT / "review/V66.175/user_controls_extract.json"
content = ["// Generated by tools/extract_user_controls175.py; do not edit.\n"]
records = []
for start, end, name in parts:
    snippet = source[start:end]
    line = source.count("\n", 0, start) + 1
    content += [f'\n#line {line} "src/mod.cpp"\n', snippet, "\n"]
    records.append({"clause": name, "source_line": line,
                    "sha256": hashlib.sha256(snippet.encode()).hexdigest()})
content.append('\n#line 1 "tests/generated_user_controls175.inc"\n')
output.write_text("".join(content))
manifest.parent.mkdir(parents=True, exist_ok=True)
manifest.write_text(json.dumps({
    "source": "src/mod.cpp", "source_sha256": hashlib.sha256(raw).hexdigest(),
    "generated": str(output.relative_to(ROOT)), "clauses": records,
    "adaptation": "No C++ statement replacements. Fixture supplies only the worker environment and leaf native/UI APIs. Mode graph, lease state machine, rising edges and selected dispatch clauses are current production. This test does not execute retail APIs or a headset."
}, indent=2) + "\n")
print(f"Extracted {len(parts)} production clauses -> {output.relative_to(ROOT)}")
f12_start = unique("if(!matrix_active && f12_down && !f12_down_prev){")
(ROOT / "tests/generated_shift_f12_h29.inc").write_text(
    "// Generated verbatim from current mod.cpp by extract_user_controls175.py\n" +
    source[f12_start:closing_brace(f12_start)] + "\n")
