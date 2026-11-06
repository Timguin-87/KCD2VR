#!/usr/bin/env python3
"""Compile literal menu/callback slices; fake only their host/native IO leaves.

The enormous ApplyMenuState body also configures unrelated input/render options.
This fixture extracts its *complete entry section through the compatibility
guard* and its literal production-stereo commit. Those unrelated settings are
not executed or claimed covered. No production source is rewritten.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys


def block(text: str, start: int) -> tuple[str, int]:
    """Balanced C++ braces, excluding comments and string/character literals."""
    brace = text.index("{", start)
    depth, pos, mode = 0, brace, "code"
    while pos < len(text):
        c, pair = text[pos], text[pos:pos + 2]
        if mode == "line":
            if c == "\n":
                mode = "code"
        elif mode == "comment":
            if pair == "*/":
                mode = "code"
                pos += 1
        elif mode in ('"', "'"):
            if c == "\\":
                pos += 1
            elif c == mode:
                mode = "code"
        elif pair == "//":
            mode = "line"
            pos += 1
        elif pair == "/*":
            mode = "comment"
            pos += 1
        elif c in ('"', "'"):
            mode = c
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if not depth:
                return text[start:pos + 1], pos + 1
        pos += 1
    raise ValueError("Unclosed C++ block")


def function(text: str, signature: str) -> str:
    return block(text, text.index(signature))[0]


def callback(text: str, name: str) -> str:
    return block(text, text.index("menu_callbacks." + name + " ="))[0] + ";"


def line_matching(text: str, pattern: str) -> str:
    matches = re.findall(pattern, text, re.M)
    if len(matches) != 1:
        raise ValueError(f"Expected exactly one source match: {pattern!r}, got {len(matches)}")
    return matches[0]


def generate(root: Path, output: Path) -> dict:
    mod = (root / "src/mod.cpp").read_text(encoding="utf-8-sig")
    menu = (root / "src/vr_menu.cpp").read_text(encoding="utf-8-sig")
    header = (root / "src/vr_menu.h").read_text(encoding="utf-8-sig")
    apply_start = mod.index("bool Mod::ApplyMenuState(")
    apply_stop = mod.index("    const int old_cyclopean", apply_start)
    commit = line_matching(mod, r"^\s*vr_production_stereo_mode_\.store\(production_stereo[^\n]+")
    (output / "actual_mod.inc").write_text(
        mod[apply_start:apply_stop] + commit + "\n    return true;\n}\n" +
        "void Mod::InstallCallbacks(){\n" + callback(mod, "read_upscaler") + "\n" +
        callback(mod, "apply_upscaler") + "\n}\n")

    click = function(menu, "void VrMenu::HandleClick(")
    entry = click[:click.index("    if(!developer_mode::Enabled()&&IsDeveloperPage(page))")]
    home = block(click, click.index("if(page==MenuPage::Home){"))[0]
    view = block(click, click.index("if(page==MenuPage::ViewModes){"))[0]
    graphics = block(click, click.index("if(page==MenuPage::Graphics){"))[0]
    click_parts = [entry]
    for name, src, mono, stereo in (
        ("Home", home, "kHomeModeMono", "kHomeModeCorrected"),
        ("ViewModes", view, "kV4ModeMono", "kV4ModeCorrected"),
    ):
        click_parts += [f"if(page==MenuPage::{name}){{\n",
                        line_matching(src, rf"^\s*if\(PtIn\({mono},x,y\)\)[^\n]+"), "\n",
                        line_matching(src, rf"^\s*if\(PtIn\({stereo},x,y\)\)[^\n]+"),
                        "\nreturn;\n}\n"]
    click_parts += ["if(page==MenuPage::Graphics){\n",
                    graphics[graphics.index("        const int t=up_tech_.load();"):], "\n}\n"]
    helpers = [function(menu, name) for name in (
        "void VrMenu::ReadState(", "bool VrMenu::ApplyState(",
        "void VrMenu::MarkDirty(", "void VrMenu::GoToPage(")]
    (output / "actual_menu.inc").write_text("\n".join(helpers) + "\n" + "".join(click_parts))

    names = ["kHomeModeMono", "kHomeModeCorrected", "kV4ModeMono", "kV4ModeCorrected", "kPageBack",
             "kUpTechPrev", "kUpTechNext", "kUpQualPrev", "kUpQualNext", "kUpPresetPrev",
             "kUpPresetNext", "kUpApply"]
    declarations = [line_matching(menu, rf"^(constexpr UiRect {name}\{{[^\n]+)") for name in names]
    declarations += [function(menu, "bool PtIn("), function(menu, "static int UpQualityCount("),
                     function(menu, "static const wchar_t* UpTechName("),
                     function(menu, "static const wchar_t* UpQualityName("),
                     function(menu, "static const wchar_t* UpPresetName(")]
    (output / "actual_rects.inc").write_text("\n".join(declarations) + "\n")
    enum = line_matching(header, r"^\s*(enum class MenuPage : int \{[^\n]+)")
    (output / "actual_menu_enum.inc").write_text(enum + "\n")

    draw_start = menu.index("} else if(page==MenuPage::Graphics){")
    draw_stop = menu.index("} else if(page==MenuPage::ViewModes){", draw_start)
    draw = menu[draw_start:draw_stop]
    title = line_matching(draw, r"^\s*(pageTitle\(L\"GRAPHICS\"[^\n]+)")
    seed = draw[draw.index("        if(!up_seeded_.exchange(true)"):]
    page_title = line_matching(menu, r"^\s*(auto pageTitle=\[&\][^\n]+)")
    view_text = line_matching(menu[draw_stop:], r"^\s*(Text\(dc,font_small_,L\"(?:Stereo requires|MONO and STEREO)[^\n]+)")
    (output / "actual_draw.inc").write_text(
        "void VrMenu::DrawUpscaler(const VrMenuState& s){const int dc=0;\n" + page_title + "\n" +
        title + "\n" + seed + "\n}\n" +
        "void VrMenu::DrawViewText(){const int dc=0;\n" + view_text + "\n}\n")
    paths = ["src/mod.cpp", "src/vr_menu.cpp", "src/vr_menu.h", "src/graphics_menu_options.h",
             "src/graphics_owner31.h"]
    if (root / "src/stereo_upscaler_d81r2.h").exists():
        paths.append("src/stereo_upscaler_d81r2.h")
    return {p: hashlib.sha256((root / p).read_bytes()).hexdigest() for p in paths}


def run_one(root: Path, fixture_root: Path, out: Path, cxx: str, negative: bool) -> dict:
    out.mkdir(parents=True, exist_ok=True)
    hashes = generate(root, out)
    # Baseline controls use baseline source callbacks/menu and the same test
    # harness; the new policy header is needed only by the repaired source.
    command = [cxx, "-std=c++23", "-O2", "-Wall", "-Wextra", "-Wno-unused-variable",
               "-Wno-misleading-indentation", "-I" + str(out), "-I" + str(root / "src"),
               "-I" + str(fixture_root / "src"),
               str(fixture_root / "tests/stereo_menu_d81r2_test.cpp"), "-o", str(out / "fixture")]
    compiled = subprocess.run(command, capture_output=True, text=True)
    (out / "compile.log").write_text(compiled.stdout + compiled.stderr)
    result = {"source": str(root), "negative_control": negative, "source_sha256": hashes,
              "command": command, "compile_returncode": compiled.returncode}
    if compiled.returncode:
        result["passed"] = False
        return result
    ran = subprocess.run([str(out / "fixture")], capture_output=True, text=True)
    (out / "run.log").write_text(ran.stdout + ran.stderr)
    result.update(run_returncode=ran.returncode, summary=ran.stdout.splitlines()[-1:],
                  passed=(ran.returncode != 0) if negative else (ran.returncode == 0))
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--baseline", type=Path)
    parser.add_argument("--cxx", default="g++")
    args = parser.parse_args()
    results = [run_one(args.source.resolve(), args.source.resolve(), args.out / "current", args.cxx, False)]
    if args.baseline:
        results.append(run_one(args.baseline.resolve(), args.source.resolve(), args.out / "baseline", args.cxx, True))
    report = {"jobs": results, "limits": [
        "Actual extracted source blocks, portable native/config/UI IO leaves; no game, Windows DLL or headset execution.",
        "ApplyMenuState coverage is its entry guard and actual stereo commit; unrelated input/render option updates omitted.",
        "UI drawing records verify rectangles/text selection; native Segoe UI rasterization is not executed.",
        "State races are deterministic interleavings at callback boundaries, not a concurrent runtime stress test.",
    ]}
    (args.out / "results.json").write_text(json.dumps(report, indent=2) + "\n")
    for result in results:
        print(("BASELINE" if result["negative_control"] else "CURRENT"), result.get("summary", result),
              "PASS" if result["passed"] else "FAIL")
    return 0 if all(r["passed"] for r in results) else 1


if __name__ == "__main__":
    sys.exit(main())
