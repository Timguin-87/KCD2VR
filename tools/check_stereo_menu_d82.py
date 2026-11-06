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
        callback(mod, "stereo_ready") + "\n" + callback(mod, "apply_upscaler") + "\n" +
        callback(mod, "save_settings") + "\n}\n" +
        'void Mod::FlushSettings(){if(!io.pendingSave)return;io.pendingSave=false;bool ok=true;\n' +
        'auto wi=[](const char* key,int value){if(std::strcmp(key,"VrStereoExperiment"))return false;io.persistedMode=value;return true;};\n' +
        line_matching(mod, r'^\s*ok = wi\("VrStereoExperiment"[^\n]+') + '\n}\n' +
        'bool WorkerStereoReadyForTest(){return ' +
        line_matching(mod, r'^\s*(native_upscaler_stereo_d82::CanEnterStereo\(SafeCVarGetInt\(nativeUpscalerCvarD82\.load\(std::memory_order_acquire\),-1\)\))\) \{') + ';}\n')

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
    view_text = line_matching(menu[draw_stop:], r"^\s*(Text\(dc,font_small_,L\"(?:DLSS, FSR and XeSS stereo|DLSS and FSR stereo support|Stereo requires|MONO and STEREO)[^\n]+)")
    (output / "actual_draw.inc").write_text(
        "void VrMenu::DrawUpscaler(const VrMenuState& s){const int dc=0;\n" + page_title + "\n" +
        title + "\n" + seed + "\n}\n" +
        "void VrMenu::DrawViewText(){const int dc=0;\n" + view_text + "\n}\n")
    paths = ["src/mod.cpp", "src/vr_menu.cpp", "src/vr_menu.h", "src/graphics_menu_options.h",
             "src/graphics_owner31.h"]
    if (root / "src/stereo_upscaler_d81r2.h").exists():
        paths.append("src/stereo_upscaler_d81r2.h")
    return {p: hashlib.sha256((root / p).read_bytes()).hexdigest() for p in paths}


def run_one(root: Path, fixture_root: Path, out: Path, cxx: str, negative: bool, mutation=None) -> dict:
    out.mkdir(parents=True, exist_ok=True)
    hashes = generate(root, out)
    if mutation:
        name, filename, before, after, count = mutation
        target = out / filename
        text = target.read_text()
        if text.count(before) != count:
            raise ValueError(f"Mutation {name}: expected {count} source match(es)")
        target.write_text(text.replace(before, after))
    command = [cxx, "-std=c++23", "-O2", "-Wall", "-Wextra", "-Wno-unused-variable",
               "-Wno-misleading-indentation", "-I" + str(out), "-I" + str(root / "src"),
               "-I" + str(fixture_root / "src"),
               str(fixture_root / "tests/stereo_menu_d82_test.cpp"), "-o", str(out / "fixture")]
    compiled = subprocess.run(command, capture_output=True, text=True)
    (out / "compile.log").write_text(compiled.stdout + compiled.stderr)
    result = {"source": str(root), "negative_control": mutation[0] if mutation else False, "source_sha256": hashes,
              "command": command, "compile_returncode": compiled.returncode}
    if compiled.returncode:
        result["passed"] = False
        return result
    ran = subprocess.run([str(out / "fixture")], capture_output=True, text=True)
    (out / "run.log").write_text(ran.stdout + ran.stderr)
    result.update(run_returncode=ran.returncode, summary=ran.stdout.splitlines()[-1:],
                  passed=(ran.returncode == 1 and "FAIL " in ran.stdout) if negative else (ran.returncode == 0))
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--cxx", default="g++")
    args = parser.parse_args()
    mutations = [
        ("discard_stereo_request_until_ready", "actual_mod.inc", "if(!stereo_upscaler_d81r2::canKeep(3,actual>=0,actual))return false;", "if(!stereo_upscaler_d81r2::canKeep(3,actual>=0,actual)||!native_upscaler_stereo_d82::CanEnterStereo(actual))return false;", 1),
        ("missing_readiness_explanation", "actual_mod.inc", "&&native_upscaler_stereo_d82::CanEnterStereo(actual)", "&&true", 1),
        # D85: XeSS is a supported Stereo choice; the old skip must not return.
        ("stereo_cycle_skips_xess", "actual_menu.inc", "if(stereoD82&&!stereo_upscaler_d81r2::canSelect(s.stereo_mode,nt))", "if(stereoD82&&(nt==4||!stereo_upscaler_d81r2::canSelect(s.stereo_mode,nt)))", 1),
        ("save_without_live_switch", "actual_mod.inc", "if(ok)upscaler_live_switch_d85::Instance().Request({tech,quality,preset});", "", 1),
        ("first_draw_erases_restart", "actual_draw.inc", "if(up_status_.load()!=5)", "", 2),
    ]
    results = [run_one(args.source.resolve(), args.source.resolve(), args.out / "current", args.cxx, False)]
    for mutation in mutations:
        results.append(run_one(args.source.resolve(), args.source.resolve(), args.out / mutation[0], args.cxx, True, mutation))
    report = {"jobs": results, "passed": all(result["passed"] for result in results), "limits": [
        "Actual extracted source blocks; native/config/GDI IO and aggregate readiness leaves substituted.",
        "ApplyMenuState coverage is its entry guard and actual stereo commit, not unrelated settings.",
        "Drawing verifies exact text and bounded rectangles, not Windows font rasterization.",
        "Callback-boundary races are deterministic interleavings, not concurrent runtime stress.",
        "No Windows DLL, vendor GPU, game or headset execution.",
    ]}
    (args.out / "RESULT.json").write_text(json.dumps(report, indent=2)+"\n")
    print(json.dumps({"passed": report["passed"], "jobs": [{"name": r["negative_control"] or "current", "passed": r["passed"], "summary": r.get("summary", "compile failed")} for r in results]}, indent=2))
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    sys.exit(main())
