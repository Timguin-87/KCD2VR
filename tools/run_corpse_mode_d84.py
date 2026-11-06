"""D84 corpse feature/recorder independence using literal production adapters."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys


def extract_function(text, marker):
    at = text.index(marker)
    p = text.index('{', at) + 1
    level = 1
    while level:
        level += (text[p] == '{') - (text[p] == '}')
        p += 1
    return text[at:p]


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--output', type=Path, required=True)
    ap.add_argument('--baseline', type=Path)
    args = ap.parse_args()
    root = Path(__file__).resolve().parents[1]
    out = args.output.resolve()
    rows = []
    for label, source in [('current', root)] + ([('baseline', args.baseline.resolve())] if args.baseline else []):
        if out == source or source in out.parents:
            ap.error('Evidence output must be outside the source and baseline')
        dest = out / label
        dest.mkdir(parents=True, exist_ok=True)
        native = source / 'src/vrik/lab58.inl'
        text = native.read_text(encoding='utf-8-sig')
        literal = '\n\n'.join(extract_function(text, f'static void {name}(') for name in ('PhysicsDiagnosticsW5', 'Tick')) + '\n'
        (dest / 'corpse_mode_source.inc').write_text(literal)
        (dest / 'source.json').write_text(json.dumps({'path': str(native), 'sha256': hashlib.sha256(native.read_bytes()).hexdigest(), 'extraction_sha256': hashlib.sha256(literal.encode()).hexdigest(), 'scope': 'Literal native Tick and PhysicsDiagnosticsW5; real finite recorder; named native read/service leaves.'}, indent=2) + '\n')
        # Preserve the actual early return and tail order. The intervening
        # combat-only work is outside this fixture; do not fabricate its result.
        frik = source / 'src/frik_pose_probe.cpp'
        frik_text = frik.read_text(encoding='utf-8-sig')
        outer = extract_function(frik_text, 'static void NativeRhythmTick178(')
        begin = outer.index('    ReadScope101R2 reads;')
        end = outer.index('    // One read-only articulated-part query', begin)
        excluded = outer[begin:end]
        if 'return;' in excluded:
            raise RuntimeError('Unexpected outer return in combat region; inspect before extracting')
        outer = outer[:begin] + '    OuterCombatWorkD84(); // unrelated native/combat work controlled by fixture\n' + outer[end:]
        frame = extract_function(frik_text, 'void PhysicalNativeFrameTick6698(')
        stamp = next(line for line in frame.splitlines() if 'g_stage_thread596=GetCurrentThreadId();' in line)
        dispatch = next(line for line in frame.splitlines() if 'NativeRhythmTick178();' in line)
        if frame.index(stamp) >= frame.index(dispatch):
            raise RuntimeError('Stage owner no longer precedes service')
        nexus = source / 'src/native_stereo_outer_lab.cpp'
        nexus_text = nexus.read_text(encoding='utf-8-sig')
        nexus_line = next(line.strip() for line in nexus_text.splitlines() if 'if(old_depth==0' in line and 'frik_pose_probe::PhysicalNativeFrameTick6698();' in line)
        outer += '\nvoid PhysicalNativeFrameTick6698(){\n' + stamp + '\n' + dispatch + '\n}\n'
        outer += 'namespace frik_pose_probe {static void PhysicalNativeFrameTick6698(){::PhysicalNativeFrameTick6698();}}\n'
        outer += 'static void NexusFrameD84(unsigned old_depth,unsigned flags){\n' + nexus_line + '\n}\n'
        (dest / 'corpse_outer_source.inc').write_text(outer)
        (dest / 'outer_source.json').write_text(json.dumps({
            'sources': [{'path': str(p), 'sha256': hashlib.sha256(p.read_bytes()).hexdigest()} for p in (frik, nexus)],
            'extraction_sha256': hashlib.sha256(outer.encode()).hexdigest(),
            'scope': 'Literal NativeRhythmTick early branch, prelude and tail; unrelated combat interval replaced by named leaf. Literal outer frame owner+dispatch and nexus guard; actual lab58 Tick in local fixture.'}, indent=2) + '\n')
        subprocess.run([sys.executable, str(root / 'tools/extract_corpse_drag_d40.py'), str(dest), '--source-root', str(source)], check=True, stdout=subprocess.DEVNULL)
        names = ('corpse_mode_d84', 'corpse_outer_d84', 'saved_corpse_native_d72', 'corpse_drag_native_d40', 'lab58', 'physics_lab_w5') if label == 'current' else ('corpse_mode_d84', 'corpse_outer_d84', 'saved_corpse_native_d72')
        flags = ['-DKCD2VR_TEST_PHYSICS_D84'] if label == 'current' else []
        for name in names:
            configs = [('debug', ['-O0'])]
            if label == 'current' and name in ('corpse_mode_d84', 'corpse_outer_d84', 'saved_corpse_native_d72'):
                configs += [('release', ['-O2']), ('sanitized', ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-no-pie'])]
            for mode, optimize in configs:
                binary = dest / (name + '_' + mode)
                cmd = ['g++', '-std=c++20', '-UNDEBUG', '-pthread', *flags, *optimize, '-I', str(source / 'src'), '-I', str(dest), '-I', str(root / 'tests'), str(root / 'tests' / (name + '_test.cpp')), '-o', str(binary)]
                build = subprocess.run(cmd, capture_output=True, text=True)
                binary.with_suffix('.build.log').write_text(build.stdout + build.stderr)
                row = {'label': label, 'name': name, 'mode': mode, 'command': cmd, 'build_rc': build.returncode, 'passed': False}
                if not build.returncode:
                    env = dict(os.environ)
                    if mode == 'sanitized':
                        env['ASAN_OPTIONS'] = 'detect_leaks=0'
                    run = subprocess.run([str(binary)], capture_output=True, text=True, env=env)
                    binary.with_suffix('.run.log').write_text(run.stdout + run.stderr)
                    row.update(run_rc=run.returncode, stdout=run.stdout, stderr=run.stderr)
                    row['passed'] = run.returncode == 0 if label == 'current' else run.returncode == 1 and 'FAIL D84' in run.stdout and not run.stderr
                rows.append(row)
                print(f'{label}/{name}/{mode}: {"PASS" if row["passed"] else "FAIL"}', flush=True)
    (out / 'results.json').write_text(json.dumps(rows, indent=2) + '\n')
    return 0 if all(r['passed'] for r in rows) else 1


if __name__ == '__main__':
    raise SystemExit(main())
