#!/usr/bin/env python3
"""Compile the real menu reducer and extracted menu/INI/runtime statements.

The Windows drawing and application callbacks are host doubles. Selection,
hitboxes, highlighting, reducer cycling, serialization, and validation are
compiled from production source; this does not exercise a headset or Windows UI.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import host_compiler


def block(text, marker):
    start = text.index(marker)
    opening = text.index('{', start)
    depth, state, pos = 0, 'code', opening
    while pos < len(text):
        char, following = text[pos], text[pos + 1:pos + 2]
        if state == 'line':
            if char == '\n':
                state = 'code'
        elif state == 'comment':
            if char == '*' and following == '/':
                state = 'code'
                pos += 1
        elif state in ('"', "'"):
            if char == '\\':
                pos += 1
            elif char == state:
                state = 'code'
        elif char == '/' and following == '/':
            state = 'line'
            pos += 1
        elif char == '/' and following == '*':
            state = 'comment'
            pos += 1
        elif char in ('"', "'"):
            state = char
        elif char == '{':
            depth += 1
        elif char == '}':
            depth -= 1
            if depth == 0:
                return text[start:pos + 1]
        pos += 1
    raise ValueError('Unterminated source block: ' + marker)


def statement(text, marker):
    start = text.index(marker)
    return text[start:text.index(';', start) + 1]


def extract(root, output):
    paths = ['src/vr_menu.cpp', 'src/vr_menu.h', 'src/mod.cpp',
             'src/subsystems/experimental/live_runtime_d40.inl',
             'src/subsystems/experimental/live_settings_d39.h',
             'src/vrik/motion_trials_d67.h',
             'src/diagnostics/block_indicator_h24.h']
    source = {p: (root / p).read_text(encoding='utf-8-sig') for p in paths}
    menu, header, mod, runtime = (source[p] for p in paths[:4])
    state = block(header, 'struct VrMenuState') + ';\n'
    state = ('namespace kcd2vr::block_indicator_h24 {\n' +
             statement(source[paths[-1]], 'inline constexpr bool defaultEnabled=') + '\n}\n' +
             'namespace kcd2vr {\n' + state + '\n}\n')
    output.mkdir(parents=True, exist_ok=True)
    (output / 'fresh_uncapped_menu_state.inc').write_text(state)

    constants = []
    for decl in re.findall(r'constexpr UiRect\s+[^;]+;', menu):
        if re.search(r'\b(?:kPageBack|kVrikLeadD61|kVrikBaseD61|kVrikTimingD61|'
                     r'kVrikUncappedD67|kVrikElbowD61|kPickupCameraD65|kRepairTrialsD65)\b', decl):
            constants.append(decl)
    geometry = '\n'.join(constants) + '\n' + block(menu, 'bool PtIn(') + '\n'
    (output / 'fresh_uncapped_menu_geometry.inc').write_text(geometry)

    getter = statement(mod, 'out.vrik_comparison_d61=')
    reader = statement(runtime, 'd40.motionTrialD67=')
    setter = statement(runtime, 'motion_trials_d67::SetMenuMode(unsigned(d40.motionTrialD67))')
    load = statement(mod, 'const int storedMotionR92=') + '\n' + statement(
        mod, 'motion_trials_d67::SetMenuMode(motion_trials_d67::MenuModeFromIni(storedMotionR92))')
    save = next(line.strip() for line in mod.splitlines()
                if 'ok=wi("VrMotionTrialD67"' in line)
    # The first occurrence is GoToPage's redirect to the Hands group, not
    # the retained legacy handler. Test both explicit production branches.
    redirect = block(menu, 'if(page==MenuPage::VrikExperimentsD61)')
    click_start = menu.index('if(page==MenuPage::VrikExperimentsD61){\n')
    click = block(menu[click_start:], 'if(page==MenuPage::VrikExperimentsD61)')
    draw = block(menu, 'else if(page==MenuPage::VrikExperimentsD61)')
    draw = draw[draw.index('{') + 1:-1]
    connected = ('static VrMenuState Fill() { VrMenuState out{};\n' + getter +
                 '\nauto& d40=out.live_d39;\n' + reader + '\nreturn out; }\n' +
                 'static void ApplyMotion(const VrMenuState& s) { const auto& d40=s.live_d39;\n' +
                 setter + '\n}\n' +
                 'static void LoadMotion() { const char* explicit_ini="fixture.ini";\n' + load + '\n}\n' +
                 'static bool SaveMotion() { bool ok=true;\n' + save + '\nreturn ok; }\n')
    # Definitions need the production persistence/owner functions before callbacks.
    (output / 'fresh_uncapped_menu_owner.inc').write_text(connected)
    actions = ('static void RedirectLegacy(MenuPage& page) {\n' + redirect + '\n}\n' +
               'static void ClickClassic(VrMenuState& s,int x,int y) {\n' + click + '\n}\n' +
               'static void DrawClassic(const VrMenuState& s) {\n' + draw + '\n}\n')
    (output / 'fresh_uncapped_menu_actions.inc').write_text(actions)
    return {p: hashlib.sha256((root / p).read_bytes()).hexdigest() for p in paths}


def run(root, output, label, change=None):
    dest = output / label
    hashes = extract(root, dest)
    if change:
        file_name, old, new = change
        path = dest / file_name
        text = path.read_text()
        if text.count(old) != 1:
            raise ValueError('Negative-control source no longer matches: ' + label)
        path.write_text(text.replace(old, new))
    executable = host_compiler.executable_path(dest / 'fresh_pose_uncapped_menu')
    cmd = host_compiler.command(root / 'tests/fresh_pose_uncapped_menu_test.cpp', executable, [root / 'src', dest])
    build = subprocess.run(cmd, capture_output=True, text=True)
    (dest / 'build.log').write_text(build.stdout + build.stderr)
    result = dict(label=label, inputs=hashes, command=cmd, build_rc=build.returncode,
                  expected='test failure' if change else 'pass', passed=False)
    if build.returncode == 0:
        executed = subprocess.run([str(executable)], capture_output=True, text=True)
        (dest / 'run.log').write_text(executed.stdout + executed.stderr)
        result.update(run_rc=executed.returncode, stdout=executed.stdout, stderr=executed.stderr)
        result['passed'] = (executed.returncode == 1 and 'FAIL ' in executed.stdout) if change else executed.returncode == 0
    print(label + ': ' + ('PASS' if result['passed'] else 'FAIL'), flush=True)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    root, output = args.source.resolve(), args.output.resolve()
    if output == root or root in output.parents:
        parser.error('Evidence output must be outside the source tree.')
    results = [run(root, output, 'production')]
    if results[0]['passed']:
        results += [run(root, output, 'legacy_load_clamp', (
            'fresh_uncapped_menu_owner.inc',
            'motion_trials_d67::SetMenuMode(motion_trials_d67::MenuModeFromIni(storedMotionR92));',
            'motion_trials_d67::SetMenuMode(unsigned(std::clamp(storedMotionR92,0,3)));'))]
        results += [run(root, output, 'legacy_three_buttons', (
            'fresh_uncapped_menu_actions.inc',
            'const UiRect choices[]{kVrikBaseD61,kVrikTimingD61,kVrikUncappedD67,kVrikElbowD61};',
            'const UiRect choices[]{kVrikBaseD61,kVrikTimingD61,kVrikElbowD61};'))]
    receipt = dict(scope='Shipped Hands-group reducer/dispatch/labels, navigation redirect, source-extracted state and retained legacy branches, Mod getter/INI load/save, live owner read/apply. Legacy mode 3 migrates to capped timing plus independent elbow preference. Native UI, filesystem INI API, and runtime callbacks are explicit host doubles.',
                   headset_tested=False, windows_ui_tested=False, results=results)
    (output / 'RESULT.json').write_text(json.dumps(receipt, indent=2) + '\n')
    return 0 if all(row['passed'] for row in results) else 1


if __name__ == '__main__':
    raise SystemExit(main())
