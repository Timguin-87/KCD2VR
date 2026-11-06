#!/usr/bin/env python3
"""Execute actual reload ownership, late placement and native recovery paths."""
from pathlib import Path
import argparse, hashlib, json, os, re, subprocess
from check_crossbow_d82 import function
from host_compiler import command, executable_path

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', required=True, type=Path)
    parser.add_argument('--compiler', default='g++')
    parser.add_argument('--baseline-repro', action='store_true')
    args = parser.parse_args()
    out = args.out.resolve()
    assert out != ROOT and ROOT not in out.parents
    out.mkdir(parents=True, exist_ok=True)
    ranged = ROOT / 'src/subsystems/ranged_reload'
    header_path = ranged / 'crossbow_native_xr34.h'
    claim_path = ranged / 'reload_throw_claim_t25.h'
    adapter_path = ranged / 'crossbow_native_xr34.inl'
    passenger_path = ranged / 'reload_passenger_xr34.inl'
    route_path = ranged / 'crossbow_rifle_request_h21.inl'
    header, adapter, passenger, route = (p.read_text() for p in (header_path, adapter_path, passenger_path, route_path))
    start = adapter.index('    __try{started=g_crossbow_xr34.startNewAfterRestore(')
    ending = '__finally{(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}'
    end = adapter.index(ending, start) + len(ending)
    scope = adapter[start:end].replace('    __try{', '    {struct HostFinishR95R2 {ma::Token& token;bool& started;~HostFinishR95R2(){(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}} finish{token,started};', 1)
    scope = scope.replace('}\n    ' + ending, '}')
    adapter = adapter[:start] + scope + adapter[end:]
    consumer = ''.join(function(passenger, sig) for sig in [
        'static bool ReloadNativeClockCurrentD86(', 'static bool ReloadPassengerCurrentXR34(',
        'static bool ReadReloadPassengerXR34(', 'static bool ReloadPoseWriterCurrentXR34('])
    source = function(route, 'static bool RangedSourceXR34(')
    source = source.replace('out=xcXR34::Source{};', 'const xcXR34::Source emptySourceD87{};out=emptySourceD87;', 1).replace('out=xcXR34::Source{};', 'out=emptySourceD87;')
    retained = function(passenger, 'static bool RangedReloadSourceXR34(').replace('out=xcXR34::Source{};', 'const xcXR34::Source emptySourceD87{};out=emptySourceD87;', 1)
    includes = {
        'completed_source_d82_actual.inc': function(route, 'static bool RangedRawIntentD78(') + function(passenger, 'static xcXR34::Source CompletedReloadPoseSourceXR34('),
        'retained_source_d87_actual.inc': retained,
        'ranged_source_d87_actual.inc': source,
        'publish_source_r95_actual.inc': ''.join(function(route, sig) for sig in ['static bool RangedQuiverInputR95(', 'static bool RangedGapReleaseR95(', 'static void RangedPublishXR34(']),
        'prepare_reload_body_r93r3_actual.inc': ''.join(function(passenger, sig) for sig in ['static bool PrepareReloadPoseExactR93R3(', 'static bool SameReloadPoseRetryR93R3(', 'static bool PrepareReloadPoseXR34(']),
        'reload_context_r95r2_actual.inc': function(passenger, 'static bool ReloadContextXR34('),
    }
    names = ['crossbow_native_xr34_test.cpp', 'crossbow_scope_d82_test.cpp', 'crossbow_body_source_r93r3_test.cpp',
             'crossbow_source_route_d87_fixture.h', 'crossbow_liveness_r95r2_test.cpp']
    fixtures = {name: (ROOT / 'tests' / name).read_text().replace('"../src/', '"' + (ROOT / 'src').as_posix() + '/') for name in names}
    msvc = Path(args.compiler).name.lower() in ('cl', 'cl.exe')
    runs = []

    def build_run(name, client=header, pa=consumer, context=None, flags=('-O0', '-g'), negative=False, cases=('all',)):
        directory = out / name
        directory.mkdir(exist_ok=True)
        local_header = re.sub(r'#include "([^"]+)"', lambda match: '#include "' + (header_path.parent / match[1]).resolve().as_posix() + '"', client)
        (directory / 'client.h').write_text(local_header)
        claim = re.sub(r'#include "([^"]+)"', lambda match: '#include "' + (claim_path.parent / match[1]).resolve().as_posix() + '"', claim_path.read_text())
        claim = claim.replace('"' + header_path.as_posix() + '"', '"client.h"')
        (directory / 'claim.h').write_text(claim)
        for filename, content in fixtures.items():
            content = content.replace('"' + header_path.as_posix() + '"', '"client.h"')
            content = content.replace('"' + claim_path.as_posix() + '"', '"claim.h"')
            content = content.replace('"' + adapter_path.as_posix() + '"', '"native_adapter_host.inc"')
            (directory / filename).write_text(content)
        local_includes={**includes, 'native_adapter_host.inc': adapter, 'passenger_consumer_d82_actual.inc': pa}
        if context is not None:local_includes['reload_context_r95r2_actual.inc']=context
        for filename, content in local_includes.items():
            (directory / filename).write_text(content)
        executable = executable_path(directory / name)
        cmd = command(directory / names[-1], executable, [directory, ROOT / 'src', ROOT / 'tests/syntax_shims'], args.compiler)
        if msvc:
            cmd = [('/Od' if '-O0' in flags else '/O2') if value == '/O2' else value for value in cmd]
        else:
            cmd = [value for value in cmd if value != '-O2'] + list(flags) + ['-fmax-errors=6']
        compiled = subprocess.run(cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (directory / 'build.log').write_text(compiled.stdout)
        if compiled.returncode:
            raise RuntimeError(name + ' build failed\n' + compiled.stdout[-7000:])
        env = os.environ.copy()
        env['ASAN_OPTIONS'] = 'detect_leaks=0'
        for case in cases:
            result = subprocess.run([str(executable), case], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
            (directory / (case + '.log')).write_text(result.stdout)
            passed = (result.returncode == 1 and 'FAIL_LIVENESS_R95R2' in result.stdout) if negative else result.returncode == 0
            row = dict(name=name, case=case, command=cmd, expected_failure=negative, exit_code=result.returncode, passed=passed, output=result.stdout)
            runs.append(row)
            print(name + '/' + case + ': ' + result.stdout, end='', flush=True)
            if not passed:
                raise RuntimeError(name + '/' + case + ' unexpected result')
        executable.unlink()

    if args.baseline_repro:
        build_run('baseline', negative=True, cases=('transition', 'tool_wait', 'bolt_wait', 'pending_release', 'context_clock'))
    else:
        build_run('debug')
        build_run('optimized', flags=('-O2',))
        if not msvc:
            build_run('asan_ubsan', flags=('-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-no-pie'))
        old='        !p.clip.cocking&&!p.clip.loading;'
        assert consumer.count(old)==1
        build_run('old_bound_transition_cycle',pa=consumer.replace(old,old[:-1]+'&&!p.binding.toolBound&&!p.binding.handBoltBound;'),negative=True,cases=('transition',))
        old='if(!setupJoinCurrentR95R2(waitingPlacementR95R2||waitingRevealR95R2,now))return;'
        assert header.count(old)==1
        build_run('old_unbounded_tool_wait',client=header.replace(old,'(void)waitingPlacementR95R2;(void)waitingRevealR95R2;'),negative=True,cases=('tool_wait',))
        old='if(!setupJoinCurrentR95R2(!joined&&clock.normalized>=afterSetup-1e-5f,now))return;'
        assert header.count(old)==1
        build_run('old_unbounded_bolt_wait',client=header.replace(old,''),negative=True,cases=('bolt_wait',))
        old='if(s.grip[hand]<.25||releasedAheadR95R2){grasp_=false;neutral_=true;}'
        assert header.count(old)==1
        build_run('old_pending_release_replayed',client=header.replace(old,''),negative=True,cases=('pending_release',))
        context=includes['reload_context_r95r2_actual.inc'];old='    now=std::uint64_t(GetTickCount64());\n'
        assert context.count(old)==1
        build_run('old_context_clock_order',context=context.replace(old,''),negative=True,cases=('context_clock',))
    paths = [header_path, adapter_path, passenger_path, route_path, claim_path, Path(__file__), *[ROOT / 'tests' / name for name in names]]
    report = dict(passed=all(row['passed'] for row in runs), baseline_reproduction=args.baseline_repro, runs=runs,
                  source_sha256={str(path.relative_to(ROOT)): hashlib.sha256(path.read_bytes()).hexdigest() for path in paths},
                  limits=['Actual native adapter/client, passenger consumer, source preparation and publication execute.',
                          'Native memory/API, attachment readiness, animation/skin evaluation and event effects are explicit fixture leaves.',
                          'Host SEH-finally substitution only. No Windows DLL/link, game/headset, finite-stock inventory or real scheduler execution.'])
    (out / 'RESULT.json').write_text(json.dumps(report, indent=2) + '\n')


if __name__ == '__main__':
    main()
