"""Run actual-source dialogue/riding fixes, including compiled old-source failures."""
import argparse
import concurrent.futures
import hashlib
import json
from pathlib import Path
import re
import subprocess


def extract(source, output):
    output.mkdir(parents=True, exist_ok=True)
    mod = (source / 'src/mod.cpp').read_text(encoding='utf-8-sig')
    pieces = []
    for pattern in [r'const float gameplay_trigger_l=.*?;', r'const float gameplay_trigger_r=.*?;',
                    r'frik_pose_probe::PublishTrackingSnapshot\(hp,.*?;',
                    r'routeL.trigger=gameplay_trigger_l;routeR.trigger=gameplay_trigger_r;']:
        matches = re.findall(pattern, mod, re.S)
        if len(matches) != 1:
            raise RuntimeError(f'Expected one source statement: {pattern}, got {len(matches)}')
        pieces.append(matches[0])
    host = '\n'.join(pieces) + '\n'
    coherent = (source / 'src/vrik/coherent_pose.inl').read_text(encoding='utf-8-sig')
    start = coherent.index('        if(!nativeFingerCapture&&!objectFingers){\n            combatFistH34=')
    end = coherent.index('\n        }', start) + len('\n        }')
    writer = coherent[start:end].replace('return RejectCoherent6693(tx,__LINE__);', 'return false;') + '\n'
    visual_line = re.search(r'        const float freeFingerTriggerD78=.*?;', coherent)
    if visual_line:
        writer = visual_line.group(0) + '\n' + writer
    (output / 'generated_dialogue_host_d78.inc').write_text(host)
    (output / 'generated_dialogue_writer_d78.inc').write_text(writer)
    frik = (source / 'src/frik_pose_probe.cpp').read_text(encoding='utf-8-sig')
    producer = (source / 'src/subsystems/vrik/tracking_producer.inl').read_text(encoding='utf-8-sig')
    fingers = (source / 'src/subsystems/vrik/pose_fingers.inl').read_text(encoding='utf-8-sig')

    def cut(text, start, end):
        at = text.index(start)
        return text[at:text.index(end, at)]

    def function(text, signature):
        at = text.index(signature)
        brace = text.index('{', at)
        depth, pos = 1, brace + 1
        while depth:
            depth += (text[pos] == '{') - (text[pos] == '}')
            pos += 1
        return text[at:pos]

    snapshot = cut(frik, 'struct V3 {', 'static V3 Add(')
    signature = producer[:producer.index('{')]
    packet = (signature + '{\n' + cut(producer, '    const auto poseNow=', '    // C55:') +
              '    const bool acquisitionPose55=false; // native acquisition is outside this visual-input fixture\n' +
              re.search(r'    const bool poseAllowed=.*?;', producer).group(0) + '\n' +
              '    const bool headUsable=hv; // live-valid fixture input, no HMD-dropout claim\n' +
              re.search(r'    kcd2vr::haptics101::allowed.store\(.*?;', producer).group(0) + '\n' +
              cut(producer, '    g_left_grip.store(', '    // The permission/button packet') + '\n}\n')
    functions = '\n'.join(function(fingers, name) for name in [
        *(['static float FreeFingerTriggerD78('] if 'static float FreeFingerTriggerD78(' in fingers else []),
        'static double FreeFingerInputH7(', 'static bool QualifiedCombatFistH34(', 'static bool ApplyFingerLocals6689(']) + '\n'
    for name, contents in [('snapshot', snapshot), ('packet', packet), ('functions', functions)]:
        (output / f'generated_dialogue_{name}_d78.inc').write_text(contents)
    (output / 'extraction.json').write_text(json.dumps({
        'source': str(source), 'source_hashes': {str(p): hashlib.sha256((source / p).read_bytes()).hexdigest()
            for p in ['src/mod.cpp', 'src/frik_pose_probe.cpp', 'src/subsystems/vrik/tracking_producer.inl',
                      'src/subsystems/vrik/pose_fingers.inl', 'src/vrik/coherent_pose.inl']},
        'host_sha256': hashlib.sha256(host.encode()).hexdigest(),
        'writer_sha256': hashlib.sha256(writer.encode()).hexdigest(),
        'packet_sha256': hashlib.sha256(packet.encode()).hexdigest(),
        'functions_sha256': hashlib.sha256(functions.encode()).hexdigest(),
        'boundary_substitution': 'Only RejectCoherent6693 becomes false in isolated dispatch. Portable variant executes literal publisher validity/permission/haptic/trigger/packet/grace spans and complete free-trigger/fist/writer functions; native acquisition, other ingress work, authored endpoint acquisition and final renderer are controlled/omitted. Full-TU variant is retained for Windows.'
    }, indent=2) + '\n')


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--output', type=Path, required=True)
    ap.add_argument('--baseline', type=Path, required=True)
    ap.add_argument('--compiler', default='g++')
    args = ap.parse_args()
    root = Path(__file__).resolve().parents[1]
    output = args.output.resolve()
    baseline = args.baseline.resolve()
    for src in (root, baseline):
        if src == output or src in output.parents:
            ap.error('Evidence output must be outside both source trees')
    sources = [('current', root), ('baseline', baseline)]
    for label, source in sources:
        extract(source, output / label)

    def run(job):
        label, source, name = job
        out = output / label / name
        command = [args.compiler, '-std=c++23', '-O0', '-UNDEBUG', '-pthread', '-fms-extensions',
                   '-ffunction-sections', '-fdata-sections', '-D_ReturnAddress=HostReturnAddress', '-D__fastcall=',
                   '-I', str(source / 'src'), '-I', str(source / 'tests/syntax_shims'),
                   '-I', str(source / 'tests/openxr_sdk'), '-I', str(output / label),
                   '-I', str(root / 'tests')]
        macro, production = ('PRODUCTION_FRIK_SOURCE_D78', 'src/frik_pose_probe.cpp') if name == 'dialogue_fingers_d78' else ('PRODUCTION_ANIMATION_SOURCE_D50', 'src/animation_filter.cpp')
        command += [f'-D{macro}="{source / production}"', str(root / 'tests' / (name + '_test.cpp'))]
        if name == 'dialogue_fingers_d78':
            command += ['-DKCD2VR_DIALOGUE_PORTABLE_D78']
        command += ['-Wl,--gc-sections', '-o', str(out)]
        compiled = subprocess.run(command, capture_output=True, text=True)
        out.with_suffix('.build.log').write_text(compiled.stdout + compiled.stderr)
        row = {'name': name, 'label': label, 'command': command, 'compile_rc': compiled.returncode, 'passed': False}
        if compiled.returncode == 0:
            tested = subprocess.run([str(out)], capture_output=True, text=True)
            out.with_suffix('.run.log').write_text(tested.stdout + tested.stderr)
            row.update(run_rc=tested.returncode, stdout=tested.stdout, stderr=tested.stderr)
            row['passed'] = tested.returncode == 0 if label == 'current' else tested.returncode == 1 and 'FAIL ' in tested.stdout and not tested.stderr
        print(f'{label} {name}: {"PASS" if row["passed"] else "FAIL"}', flush=True)
        return row

    jobs = [(label, source, name) for label, source in sources for name in ['dialogue_fingers_d78', 'riding_ownership_d76']]
    with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:
        results = list(pool.map(run, jobs))
    (output / 'results.json').write_text(json.dumps({'scope': 'Actual source with native/Windows leaves mocked; no game or headset execution.',
        'passed': all(r['passed'] for r in results), 'results': results}, indent=2) + '\n')
    return 0 if all(r['passed'] for r in results) else 1


if __name__ == '__main__':
    raise SystemExit(main())
