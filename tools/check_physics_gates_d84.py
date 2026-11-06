#!/usr/bin/env python3
"""Execute literal D84 physics diagnostic producers with controlled platform leaves.

No Windows/native/renderer/headset execution. Geometry and bow math are compared
against the supplied immutable baseline, not replaced by a second test policy.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess


def region(text, anchor):
    start = text.index(anchor)
    end = text.index('{', start) + 1
    depth = 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    return text[start:end]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--baseline', type=Path)
    args = parser.parse_args()
    source, out = args.source.resolve(), args.out.resolve()
    if out == source or source in out.parents:
        parser.error('Compiler output must be outside the source tree')
    out.mkdir(parents=True, exist_ok=True)
    hashes, excerpts, results = {}, {}, []

    def read(name):
        data = (source / name).read_bytes()
        hashes[name] = hashlib.sha256(data).hexdigest()
        return data.decode('utf-8').replace('\r\n', '\n')

    def save(name, text, origin, substitutions=False):
        raw = text
        if substitutions:
            text = text.replace('std::snprintf', 'fixture::format')
        (out / name).write_text(text, encoding='utf-8')
        excerpts[name] = {'source': origin, 'normalized_literal_sha256': hashlib.sha256(raw.encode()).hexdigest(),
                          'compiled_sha256': hashlib.sha256(text.encode()).hexdigest(),
                          'substitution': 'std::snprintf -> fixture::format (counter/restart hook, then real snprintf)' if substitutions else 'none'}

    profile = read('src/subsystems/physics/native_handling_profile.inl')
    host = read('src/subsystems/physics/host.inl')
    mod = read('src/mod.cpp')
    trace = read('src/diagnostic_trace.cpp')
    weapon = read('src/vrik/physical_weapon.h')
    first_anchor = '{   // Capture-only, copied data:'
    second_anchor = '{   // C37: one line when THIS item'
    first, second = region(profile, first_anchor), region(profile, second_anchor)
    handling = region(profile, 'static auto HandlingCaptureD84()') + '\n'
    handling += 'static void HandlingDiagnostics(FixtureProfile& p,const FixtureGrip& grip){\n'
    handling += 'const double slotErr37=-1,slotDeg37=-1,slotSpan37=-1,slotVsNative37=-1;\n'
    handling += first + '\n' + second + '\n}\n'
    save('physics_handling_gate_d84.inc', handling, 'src/subsystems/physics/native_handling_profile.inl: helper and two diagnostic blocks; wrapper supplies existing copied values', True)
    save('physics_gate_host_d84.inc', host, 'src/subsystems/physics/host.inl (actual slots 5007,5009,5011)', True)
    save('physics_receipt_sink_d84.inc', region(trace, 'void WriteReceiptFor(') + '\n' + region(trace, 'void Write(') + '\n', 'src/diagnostic_trace.cpp: actual receipt and ordinary sinks')
    baseline_line = next(line for line in mod.splitlines() if 'static bool baseline59c10r7=false;' in line)
    assert 'baseline59c10r7=LogPhysicsBaseline59c10r7()' in baseline_line
    save('physics_baseline_worker_d84.inc', baseline_line + '\n', 'src/mod.cpp: actual once-only baseline latch')
    save('physics_exhaustion_math_d84.inc', region(weapon, 'struct Attributes {') + ';\n' + region(weapon, 'inline double exhaustionScale59c7b(') + '\n', 'src/vrik/physical_weapon.h: actual Attributes and exhaustion scale')
    save('physics_exhaustion_gate_d84.inc', region(profile, '{   // R59.16C7B:') + '\n', 'src/subsystems/physics/native_handling_profile.inl: complete actual exhaustion/gameplay/diagnostic block', True)
    preservation = {'checked': False}
    if args.baseline:
        path = args.baseline.resolve() / 'src/subsystems/physics/native_handling_profile.inl'
        before = path.read_text()
        def without_diagnostics(text):
            body = region(text, 'static void PhysFillProfile6698(')
            for anchor in [first_anchor, second_anchor]:
                body = body.replace(region(body, anchor), '')
            return body
        previous, current = without_diagnostics(before), without_diagnostics(profile)
        preservation = {'checked': True, 'baseline_file': str(path),
                        'baseline_file_sha256': hashlib.sha256(path.read_bytes()).hexdigest(),
                        'remaining_fill_profile_bytes_identical_after_newline_normalization': previous == current,
                        'remaining_fill_profile_sha256': hashlib.sha256(current.encode()).hexdigest(),
                        'scope': 'Complete PhysFillProfile6698 function excluding exactly its two diagnostic blocks. Includes bounds, grip/support and bow-axis math.'}
        if previous != current:
            raise AssertionError('Geometry/bow profile changed outside the selected diagnostic blocks')
    tests = ['capture_ticket_d84', 'physics_gate_producers_d84', 'f9_observability_d56']
    for name in tests:
        read('tests/' + name + '_test.cpp')
    for name in ['src/diagnostics/capture_ticket_d84.h', 'src/diagnostics/capture168.h',
                 'src/vrik/handling_profile59c10r2.h', 'src/ragdoll_lease59.h',
                 'src/subsystems/physics/f9_status_fd.h', 'src/vrik/visible_failure.h']:
        read(name)
    for variant, flags in [('debug', ['-O0', '-g']), ('release', ['-O2', '-g0']),
                           ('sanitized', ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-pie', '-no-pie'])]:
        folder = out / variant
        folder.mkdir(exist_ok=True)
        for name in tests:
            exe = folder / name
            cmd = [os.environ.get('CXX', 'g++'), '-std=c++23', '-UNDEBUG', *flags,
                   '-I' + str(source / 'src'), '-I' + str(out),
                   str(source / 'tests' / (name + '_test.cpp')), '-pthread', '-o', str(exe)]
            built = subprocess.run(cmd, capture_output=True, text=True)
            (folder / (name + '.build.log')).write_text(built.stdout + built.stderr)
            row = {'name': name, 'variant': variant, 'command': cmd, 'build_exit': built.returncode}
            if built.returncode == 0:
                run = subprocess.run([str(exe)], capture_output=True, text=True, timeout=60,
                                     env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0', UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1'))
                row.update({'run_exit': run.returncode, 'output': run.stdout + run.stderr})
                (folder / (name + '.run.log')).write_text(row['output'])
            row['passed'] = built.returncode == 0 and row.get('run_exit') == 0
            results.append(row)
            print(name, variant, 'PASS' if row['passed'] else 'FAIL', flush=True)
    receipt = {'passed': all(row['passed'] for row in results), 'results': results,
               'source_sha256': hashes, 'excerpts': excerpts, 'geometry_bow_preservation': preservation,
               'limits': ['Portable actual-header/literal-producer tests, with native/OS/format/output leaves controlled.',
                          'Profile diagnostics execute from copied-data values; complete geometry/pose runtime is not executed here.',
                          'Actual host lifecycle, native read admission, failure-status mailbox and receipt sink execute.',
                          'Existing D80 session-zero failure status enters logMailboxD80; no on-screen popup or actual log durability is asserted while captures are OFF.',
                          'Recorder restart is injected at the formatting leaf; no exhaustive thread-race proof is claimed.',
                          'Capture OFF still permits necessary gameplay clocks and native retirement/visible-status work.',
                          'ASAN+UBSAN used where labelled; leak detection disabled in this executor.',
                          'No Windows DLL, native game hooks, GPU or headset execution.']}
    (out / 'RESULTS.json').write_text(json.dumps(receipt, indent=2) + '\n')
    return 0 if receipt['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
