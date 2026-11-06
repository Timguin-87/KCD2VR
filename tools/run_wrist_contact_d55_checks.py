"""MSVC production-adapter contact checks; native memory/rays remain test leaves."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
TESTS = ('contact_occupancy_d52', 'contact_query_d53', 'contact_reseed_d53', 'world_adapter')


def digest(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out', required=True, type=Path)
    ap.add_argument('--asan', action='store_true')
    ap.add_argument('--focused', action='store_true', help='Run only changed contact paths; inherited legacy world fixture is reported separately')
    args = ap.parse_args()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    for script in ('extract_adapter_types.py', 'extract_world_adapter_test.py'):
        subprocess.run([sys.executable, str(ROOT/'tools'/script)], cwd=ROOT, check=True)
    # Counterfactual substitutes only the restart-origin policy in the extracted
    # adapter. Production body/queries and all fixtures otherwise stay identical.
    negative = out/'old_origin'
    negative.mkdir(exist_ok=True)
    adapter = (ROOT/'tests/generated_world_adapter.inc').read_text()
    before = 'c.worldAllowed=displayedD53?c.presentationWorldD53:ModelToWorld6692(x,nativeHand);'
    after = 'c.worldAllowed=ModelToWorld6692(x,nativeHand);'
    assert adapter.count(before) == 1
    (negative/'generated_world_adapter.inc').write_text(adapter.replace(before, after))
    world = (ROOT/'tests/world_adapter_test.cpp').read_text()
    world = world.replace('"../src/', '"'+(ROOT/'src').as_posix()+'/')
    world = world.replace('"generated_adapter_types.h"', '"'+(ROOT/'tests/generated_adapter_types.h').as_posix()+'"')
    (negative/'world_adapter_test.cpp').write_text(world)
    (negative/'contact_reseed_d53_test.cpp').write_bytes((ROOT/'tests/contact_reseed_d53_test.cpp').read_bytes())
    (negative/'POLICY_COUNTERFACTUAL.txt').write_text(
        'Only generated sweep-origin selection changed:\n'+before+'\n=>\n'+after+
        '\nThis is not an unmodified D54 binary/native execution.\n')
    vc = Path(os.environ.get('ProgramFiles', 'C:/Program Files'))/'Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
    cases = [(name, ROOT/'tests'/(name+'_test.cpp'), False) for name in TESTS
             if not args.focused or name != 'world_adapter']
    cases.append(('contact_reseed_old_origin', negative/'contact_reseed_d53_test.cpp', True))
    results = []
    for name, source, expected_failure in cases:
        dependency = out/(name+'.dependencies.json')
        command = ['@echo off', 'call "'+str(vc)+'" >nul', 'if errorlevel 1 exit /b 1',
                   'cl /nologo /std:c++20 /EHsc /O1 /UNDEBUG '+
                   ('/fsanitize=address /Zi ' if args.asan else '')+
                   '/I"'+str(ROOT/'src')+'" /I"'+str(ROOT/'tests')+'" /I"'+str(ROOT/'tests/syntax_shims')+'" '+
                   '/sourceDependencies "'+str(dependency)+'" "'+str(source)+'" '+
                   '/Fe:"'+str(out/(name+'.exe'))+'" /Fo:"'+str(out/(name+'.obj'))+'" /Fd:"'+str(out/(name+'.pdb'))+'"',
                   'exit /b %errorlevel%']
        cmd = out/(name+'.cmd')
        cmd.write_text('\n'.join(command)+'\n')
        compile = subprocess.run(['cmd', '/d', '/c', str(cmd)], cwd=out, capture_output=True, text=True)
        (out/(name+'.compile.log')).write_text(compile.stdout+compile.stderr)
        run = None
        if compile.returncode == 0:
            # vcvars also supplies the ASan runtime on PATH.
            cmd.write_text('@echo off\ncall "'+str(vc)+'" >nul\n"'+str(out/(name+'.exe'))+'"\nexit /b %errorlevel%\n')
            run = subprocess.run(['cmd', '/d', '/c', str(cmd)], cwd=out, capture_output=True, text=True)
            (out/(name+'.run.log')).write_text(run.stdout+run.stderr)
        failed_lines = [] if run is None else [s for s in run.stdout.splitlines() if s.startswith('FAIL ')]
        expected = ['FAIL 1.25mm real motion never sweeps unrelated authored wrist across old33cm obstacle gap',
                    'FAIL actual native ray starts at prior accepted displayed wrist',
                    'FAIL new obstacle on real controller trajectory still produces measured contact']
        passed = compile.returncode == 0 and run is not None and (
            run.returncode == 0 if not expected_failure else run.returncode == 1 and failed_lines == expected)
        hashes = {str(source):digest(source)}
        if dependency.exists():
            data = json.loads(dependency.read_text(encoding='utf-8-sig'))['Data']
            for n in data.get('Includes', []):
                dep = Path(n)
                if dep.is_relative_to(ROOT) or dep.is_relative_to(out):
                    hashes[str(dep)] = digest(dep)
        results.append(dict(test=name, compile_exit=compile.returncode,
                            run_exit=run.returncode if run else None, passed=passed,
                            expected_failure=expected_failure, failed_lines=failed_lines,
                            run_output=(run.stdout+run.stderr) if run else 'NOT_RUN', source_sha256=hashes))
    report = dict(compiler='MSVC', address_sanitizer=args.asan, focused=args.focused,
                  native_calls='MOCKED', headset='NOT_RUN', windows_dll_link='NOT_THIS_CHECK',
                  passed=all(r['passed'] for r in results), results=results,
                  extractor_sha256={n:digest(ROOT/'tools'/n) for n in ('extract_adapter_types.py','extract_world_adapter_test.py')})
    (out/'RESULTS.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps({'passed':report['passed'], 'asan':args.asan,
                      'results':[{k:r[k] for k in ('test','compile_exit','run_exit','passed','failed_lines')} for r in results]},indent=2))
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
