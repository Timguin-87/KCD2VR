#!/usr/bin/env python3
"""Run D63 combat regressions against actual adapters and optional original Pro D63 control."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--baseline', type=Path, help='Original Pro D63 source for paired/draw negative controls; not the earlier D62C tree')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    fixtures = Path(__file__).resolve().parents[1]
    source, out = args.source.resolve(), args.out.resolve()
    if source == out or source in out.parents:
        parser.error('output must be outside the source tree')
    out.mkdir(parents=True, exist_ok=True)
    if args.baseline and not (args.baseline / 'src/subsystems/masterstrike_combos/draw_motion_d63.h').is_file():
        parser.error('--baseline requires original Pro D63 draw source, not an earlier architecture')
    results = []
    for production, control in [(source, False)] + ([(args.baseline.resolve(), True)] if args.baseline else []):
        folder = out / ('baseline' if control else 'current')
        folder.mkdir(parents=True, exist_ok=True)
        profile = (production / 'src/subsystems/physics/native_handling_profile.inl').read_text()
        begin = profile.index('static bool PhysPrepare6698(')
        ingress = profile[begin:profile.index('    BowGeometry166 bowGeometry{};', begin)]
        (folder / 'draw_ingress_d63.inc').write_text(ingress + '    return true;\n}\n')
        equipment = (production / 'src/subsystems/throw_drop/native_equipment_commands.inl').read_text()
        dispatch = equipment[equipment.index('        if(c.kind==ph::CommandKind::Draw||c.kind==ph::CommandKind::Holster)'): ]
        (folder / 'draw_dispatch_d63.inc').write_text(dispatch)
        holster = (production / 'src/subsystems/throw_drop/holster_gestures.inl').read_text()
        helper = 'static kcd2vr::draw_motion_d63::Source PhysLegacyDrawSourceD63('
        if helper in holster:
            begin = holster.index(helper)
            legacy = holster[begin:holster.index('\n}\n', begin) + 3]
        else:
            # Exact old production transport; negative control only, not a
            # stand-in for the repaired occurrence/latch implementation.
            assert 'drawSourceD63=ph::draw_source_d63(in,side,true);' in holster
            legacy = '''static kcd2vr::draw_motion_d63::Source PhysLegacyDrawSourceD63(const ph::Input& in,int side,
    ph::CommandKind kind,const ph::HolsterState&,ph::HolsterState&) noexcept {
    return kind==ph::CommandKind::Draw?ph::draw_source_d63(in,side,true):kcd2vr::draw_motion_d63::Source{};
}
'''
        (folder / 'legacy_draw_transport_d63.inc').write_text(legacy)

        for name in ('paired_presentation_d63', 'paired_runtime_d63', 'draw_motion_d63', 'draw_bridge_d63') + (() if control else ('ms_paired_timing_d62c', 'draw_extraction_source_d63')):
            exe, dep = folder / name, folder / (name + '.d')
            cmd = [os.environ.get('CXX', 'g++'), '-std=c++23', '-O0', '-g0', '-pthread', '-fms-extensions',
                   '-ffunction-sections', '-fdata-sections', '-Wno-subobject-linkage', '-D_ReturnAddress=HostReturnAddress',
                   '-D__fastcall=', '-MMD', '-MF', str(dep), '-I' + str(production / 'src'),
                   '-I' + str(fixtures / 'src'), '-I' + str(folder), '-I' + str(fixtures / 'tests/syntax_shims'),
                   '-I' + str(fixtures / 'tests/openxr_sdk'),
                   '-fsanitize=address,undefined', '-fno-sanitize-recover=all', '-fno-omit-frame-pointer', '-fno-pie', '-no-pie']
            if name == 'paired_presentation_d63':
                cmd += ['-DPRODUCTION_ANIMATION_SOURCE_D63="' + str(production / 'src/animation_filter.cpp') + '"']
            elif name == 'paired_runtime_d63':
                cmd += ['-DPRODUCTION_PAIRED_ADAPTER_D63="' + str(production / 'src/subsystems/masterstrike_combos/ms_reaction_owner_d62c.inl') + '"']
            cmd += [str(fixtures / 'tests' / (name + '_test.cpp')), '-Wl,--gc-sections', '-o', str(exe)]
            build = subprocess.run(cmd, capture_output=True, text=True)
            (folder / (name + '.build.log')).write_text(build.stdout + build.stderr)
            row = {'test': name, 'source': str(production), 'baseline_expected_failure': control and name in ('paired_presentation_d63', 'draw_bridge_d63'),
                   'compile_command': cmd, 'compile_exit': build.returncode}
            if build.returncode == 0:
                env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0', UBSAN_OPTIONS='print_stacktrace=1:halt_on_error=1')
                run = subprocess.run([str(exe)], capture_output=True, text=True, env=env)
                row.update(run_exit=run.returncode, output=run.stdout + run.stderr)
                (folder / (name + '.run.log')).write_text(row['output'])
            dependencies = {}
            if dep.is_file():
                for token in dep.read_text().replace('\\\n', ' ').split()[1:]:
                    path = Path(token)
                    if path.is_file():
                        dependencies[str(path.resolve())] = hashlib.sha256(path.read_bytes()).hexdigest()
            row['dependencies'] = dependencies
            if name == 'draw_bridge_d63':
                row['extractions'] = {
                    'draw_ingress_d63.inc': {'source': 'src/subsystems/physics/native_handling_profile.inl', 'sha256': hashlib.sha256(ingress.encode()).hexdigest(), 'scope': 'Actual admission/profile ingress prefix, explicit return replacing unrelated bow/attribute tail.'},
                    'draw_dispatch_d63.inc': {'source': 'src/subsystems/throw_drop/native_equipment_commands.inl', 'sha256': hashlib.sha256(dispatch.encode()).hexdigest(), 'scope': 'Verbatim complete Draw/Holster dispatch block; native leaves modeled.'},
                    'legacy_draw_transport_d63.inc': {'source': 'src/subsystems/throw_drop/holster_gestures.inl', 'sha256': hashlib.sha256(legacy.encode()).hexdigest(), 'scope': 'Actual raw latch/source helper, or the original Pro D63 expression for negative control.'}}
            text = row.get('output', '')
            row['passed'] = row.get('run_exit') == (1 if row['baseline_expected_failure'] else 0)
            if row['baseline_expected_failure']:
                row['passed'] &= 'FAIL ' in text and 'AddressSanitizer' not in text and 'runtime error:' not in text
            results.append(row)
            print(name, 'Pro D63 control' if control else 'D63 corrected', 'PASS' if row['passed'] else 'FAIL',
                  (text or build.stderr)[-1600:], flush=True)
    receipt = {'tests': results, 'passed': all(r['passed'] for r in results), 'sanitizers': 'ASAN+UBSAN',
               'native_game': 'NOT_RUN', 'native_SEH': 'NOT_RUN', 'headset': 'NOT_RUN'}
    (out / 'RESULTS.json').write_text(json.dumps(receipt, indent=2) + '\n')
    return 0 if receipt['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
