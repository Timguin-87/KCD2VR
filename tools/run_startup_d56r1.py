"""Extract and run exact D56R1 production startup boundaries on Windows.

Native game/D3D objects are not loaded. RTTI uses real Windows memory faults;
the command-list fixture supplies fake COM leaves and real native threads.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
RTTI_CASES = (
    'rtti_valid', 'rtti_truncate', 'rtti_null', 'rtti_object_noaccess',
    'rtti_object_split_read', 'rtti_null_vtable', 'rtti_vtable_noaccess',
    'rtti_null_col', 'rtti_col_noaccess', 'rtti_col_split_read', 'rtti_zero_type',
    'rtti_no_module', 'rtti_name_noaccess', 'rtti_name_crosses_guard',
    'rtti_bounded_name_at_guard', 'rtti_null_output', 'rtti_tiny_output',
    'vtable_valid', 'vtable_null', 'vtable_noaccess', 'vtable_split_read',
    'vtable_outside_image', 'vtable_before_image', 'vtable_no_module',
)
CHAIN_CASES = ('concurrent', 'primary_failure_retry', 'invalid_paths', 'pso_state')


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def function(text, signature):
    """Brace-aware extraction; source comments and string literals are ignored."""
    start = text.index(signature)
    brace = text.index('{', start)
    level, mode, escape, i = 0, '', False, brace
    while i < len(text):
        c, pair = text[i], text[i:i+2]
        if mode == '//':
            if c == '\n': mode = ''
        elif mode == '/*':
            if pair == '*/': mode = ''; i += 1
        elif mode in ('"', "'"):
            if escape: escape = False
            elif c == '\\': escape = True
            elif c == mode: mode = ''
        elif pair in ('//', '/*'): mode = pair; i += 1
        elif c in ('"', "'"): mode = c
        elif c == '{': level += 1
        elif c == '}':
            level -= 1
            if level == 0: return text[start:i+1], text.count('\n', 0, start) + 1
        i += 1
    raise ValueError('Unterminated function '+signature)


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out', required=True, type=Path)
    p.add_argument('--rtti-only', action='store_true')
    p.add_argument('--chain-only', action='store_true')
    p.add_argument('--compare-chain-ref', help='One immutable Git revision for the unmodified chain counterfactual')
    p.add_argument('--clang', type=Path, help='Optional existing clang-cl; never installed by this test')
    a = p.parse_args()
    if a.rtti_only and a.chain_only: p.error('Choose at most one filter')
    out = a.out.resolve(); out.mkdir(parents=True, exist_ok=True)
    provenance = {}
    suites = []
    if not a.chain_only:
        src = ROOT/'src/stereo_archaeology.h'
        source = src.read_text()
        extracted = []
        for signature in ('inline std::uintptr_t WhBase() noexcept {',
                          'inline std::uintptr_t ToRva(',
                          'inline std::uintptr_t ObjectVtableRva(void* p) noexcept {',
                          'inline const char* ObjectRttiName('):
            body, line = function(source, signature)
            extracted.append(body)
            provenance[signature] = dict(source=str(src), source_sha256=sha(src), line=line,
                extracted_sha256=hashlib.sha256(body.encode()).hexdigest())
        (out/'d56r1_rtti_actual.inc').write_text('\n\n'.join(extracted)+'\n')
        suites.append(('rtti', ROOT/'tests/d56r1_rtti_seh_test.cpp', RTTI_CASES))
    if not a.rtti_only:
        src = ROOT/'src/device_hook.cpp'
        body, line = function(src.read_text(), 'HRESULT __stdcall DeviceHook::DetourCreateCommandList(')
        (out/'d56r1_command_list_actual.inc').write_text(body+'\n')
        provenance['DetourCreateCommandList'] = dict(source=str(src), source_sha256=sha(src), line=line,
            extracted_sha256=hashlib.sha256(body.encode()).hexdigest())
        suites.append(('chain', ROOT/'tests/d56r1_command_list_chain_test.cpp', CHAIN_CASES))
        if a.compare_chain_ref:
            revision = subprocess.check_output(['git', 'rev-parse', a.compare_chain_ref], cwd=ROOT, text=True).strip()
            old_source = subprocess.check_output(['git', 'show', revision+':src/device_hook.cpp'], cwd=ROOT, text=True)
            old_body, old_line = function(old_source, 'HRESULT __stdcall DeviceHook::DetourCreateCommandList(')
            (out/'d56r1_command_list_baseline.inc').write_text(old_body+'\n')
            fixture = ROOT/'tests/d56r1_command_list_chain_test.cpp'
            old_fixture = out/'d56r1_command_list_baseline_test.cpp'
            old_fixture.write_text(fixture.read_text().replace('d56r1_command_list_actual.inc', 'd56r1_command_list_baseline.inc'))
            provenance['DetourCreateCommandList_baseline'] = dict(git_revision=revision,
                source='src/device_hook.cpp', source_sha256=hashlib.sha256(old_source.encode()).hexdigest(),
                line=old_line, extracted_sha256=hashlib.sha256(old_body.encode()).hexdigest(),
                fixture_sha256=sha(fixture), fixture_change='include basename only')
            suites.append(('chain_baseline', old_fixture, CHAIN_CASES))
    vc = Path(os.environ.get('ProgramFiles', 'C:/Program Files'))/'Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
    compilers = [('msvc_ehsc', 'cl', '/EHsc')]
    if a.clang:
        if not a.clang.is_file(): raise FileNotFoundError(a.clang)
        compilers += [('clang_ehsc', '"'+str(a.clang)+'"', '/EHsc'),
                      ('clang_eha', '"'+str(a.clang)+'"', '/EHa')]
    results = []
    for compiler, command, eh in compilers:
        for suite, source, cases in suites:
            stem = compiler+'_'+suite
            exe = out/(stem+'.exe')
            # /O2 and /DNDEBUG match release optimization; checks use explicit returns.
            cmd = out/(stem+'.cmd')
            cmd.write_text('@echo off\ncall "'+str(vc)+'" >nul\nif errorlevel 1 exit /b 1\n'+
                command+' /nologo /std:c++20 /O2 /DNDEBUG '+eh+' /I"'+str(out)+'" "'+str(source)+'" '+
                '/Fe:"'+str(exe)+'" /Fo:"'+str(out/(stem+'.obj'))+'" /Fd:"'+str(out/(stem+'.pdb'))+'"\nexit /b %errorlevel%\n')
            build = subprocess.run(['cmd', '/d', '/c', str(cmd)], cwd=out, capture_output=True, text=True)
            (out/(stem+'.compile.log')).write_text(build.stdout+build.stderr)
            item = dict(compiler=compiler, suite=suite, flags='/std:c++20 /O2 /DNDEBUG '+eh,
                compile_exit=build.returncode, cases=[], source_sha256=sha(source))
            if build.returncode == 0:
                item['exe_sha256'] = sha(exe)
                for case in cases:
                    try:
                        run = subprocess.run([str(exe), case], cwd=out, capture_output=True, text=True, timeout=30)
                        record = dict(name=case, exit=run.returncode, output=run.stdout+run.stderr,
                                      passed=run.returncode == 0 and 'PASS' in run.stdout and 'FAIL' not in run.stdout)
                    except subprocess.TimeoutExpired as exc:
                        record = dict(name=case, exit='TIMEOUT', output=str(exc), passed=False)
                    item['cases'].append(record)
                (out/(stem+'.run.log')).write_text('\n'.join(str(r['exit'])+' '+r['output'] for r in item['cases']))
            item['expected_failure_cases'] = ['concurrent', 'primary_failure_retry'] if suite == 'chain_baseline' else []
            # These two failures establish the old function violates the new
            # invariant. Unrelated invalid-path/PSO cases must still pass.
            item['passed'] = build.returncode == 0 and all(
                (r['exit'] == 1 and 'FAIL' in r['output']) if r['name'] in item['expected_failure_cases'] else r['passed']
                for r in item['cases'])
            results.append(item)
    report = dict(provenance=provenance, results=results,
        passed=all(r['passed'] for r in results if r['compiler'] != 'clang_ehsc'),
        donor_clang_ehsc_is_comparison_only=True, game_loaded=False, dll_injected=False,
        headset_test=False, native_d3d_hooks='FAKE_LEAVES', rtti_memory_faults='REAL_WINDOWS_SEH',
        runner_sha256=sha(Path(__file__)))
    (out/'D56R1_STARTUP_RESULTS.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(dict(passed=report['passed'], results=[dict(compiler=r['compiler'], suite=r['suite'],
        compile_exit=r['compile_exit'], passed=r['passed'], cases=len(r['cases']),
        failures=[x for x in r['cases'] if not x['passed']]) for r in results]), indent=2))
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
