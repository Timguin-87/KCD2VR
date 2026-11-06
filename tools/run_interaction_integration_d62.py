#!/usr/bin/env python3
"""Execute current collision/camera production adapters and D61 negative controls."""
from pathlib import Path
import argparse, hashlib, json, os, re, shlex, shutil, subprocess, sys

ROOT=Path(__file__).resolve().parents[1]

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out',type=Path,required=True)
    p.add_argument('--baseline',type=Path)
    p.add_argument('--compare-legacy',action='store_true',help='Also run the broad historical world fixture on both sources; report assertion parity separately, never as a passing suite.')
    a=p.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
    if a.compare_legacy and not a.baseline:p.error('--compare-legacy requires --baseline')
    baseline=None
    if a.baseline:
        baseline=out/'baseline_source'
        if baseline.exists():raise SystemExit('Use a fresh output directory for immutable baseline evidence.')
        for name in ('src','tests','tools'):
            shutil.copytree(a.baseline.resolve()/name,baseline/name)
        # Same test bodies on both implementations; current fixture imports the
        # production D60 cost scope that was missing from the old fixture seam.
        for name in ('world_adapter_test.cpp','contact_release_public_d62_test.cpp','tracked_camera_pickup_d55_test.cpp'):
            shutil.copyfile(ROOT/'tests'/name,baseline/'tests'/name)
    records=[]
    cases=['contact_release_public_d62','contact_release_provenance_d62','tracked_camera_pickup_d55']
    if a.compare_legacy:cases.append('world_adapter')
    for label,source in [('current',ROOT)]+([('baseline',baseline)] if baseline else []):
        dest=out/label;dest.mkdir(exist_ok=True);gen=dest/'generated';gen.mkdir(exist_ok=True)
        for script,args in [('extract_adapter_types.py',[]),('extract_world_adapter_test.py',[]),
                            ('extract_tracked_camera_pickup_d55.py',['--out',str(gen)])]:
            run=subprocess.run([sys.executable,str(source/'tools'/script),*args],cwd=source,capture_output=True,text=True)
            (dest/(script+'.log')).write_text(run.stdout+run.stderr)
            if run.returncode:raise SystemExit(run.stderr)
        selected=cases if label=='current' else [cases[0],cases[2]]+(['world_adapter'] if a.compare_legacy else [])
        for name in selected:
            exe=dest/name;dep=dest/(name+'.d')
            cmd=[os.environ.get('CXX','g++'),'-std=c++23','-O1','-g0','-pthread','-fms-extensions',
                 '-ffunction-sections','-fdata-sections','-Wno-subobject-linkage','-D_ReturnAddress=HostReturnAddress',
                 '-D__fastcall=','-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie',
                 '-MMD','-MF',str(dep),*['-I'+str(x) for x in (source/'src',source/'tests',gen,source/'tests/syntax_shims',source/'tests/openxr_sdk')],
                 str(source/'tests'/(name+'_test.cpp')),'-Wl,--gc-sections','-o',str(exe)]
            build=subprocess.run(cmd,cwd=source,capture_output=True,text=True)
            (dest/(name+'.build.log')).write_text(build.stdout+build.stderr)
            rec={'lane':label,'test':name,'compile_exit':build.returncode,'command':cmd,'native':'MOCKED','headset':'NOT_RUN','role':'legacy_diagnostic' if name=='world_adapter' else 'regression_gate' if label=='current' else 'negative_control'}
            if not build.returncode:
                run=subprocess.run([str(exe)],cwd=source,capture_output=True,text=True,timeout=90,
                    env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='halt_on_error=1'))
                log=run.stdout+run.stderr;(dest/(name+'.run.log')).write_text(log)
                rec.update(run_exit=run.returncode,failures=re.findall(r'^FAIL (.+)$',log,re.M),
                    sanitizer_error=bool(re.search(r'AddressSanitizer|runtime error:|UndefinedBehaviorSanitizer',log)))
                rec['passed']=(run.returncode==0 and not rec['sanitizer_error']) if label=='current' or name=='world_adapter' else (
                    run.returncode==1 and bool(rec['failures']) and not rec['sanitizer_error'])
                print(label,name,'PASS' if rec['passed'] else 'FAIL',log[-250:],flush=True)
            else:
                rec['passed']=False;print(label,name,'COMPILE_FAIL',build.stderr[-2000:],flush=True)
            records.append(rec)
    # Hash the compiler's actual production/fixture dependency closure. A clean
    # source ZIP or committed checkout must not produce an empty input receipt.
    inputs=set()
    for dep in (out/'current').glob('*.d'):
        dependencies=dep.read_text().replace('\\\n',' ').partition(':')[2]
        for value in shlex.split(dependencies):
            path=Path(value).resolve()
            if path.is_file() and ROOT in path.parents:inputs.add(path)
    for name in ('extract_adapter_types.py','extract_world_adapter_test.py','extract_tracked_camera_pickup_d55.py','run_interaction_integration_d62.py'):
        inputs.add(ROOT/'tools'/name)
    hashes={p.relative_to(ROOT).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(inputs)}
    generated={p.relative_to(out/'current').as_posix():hashlib.sha256(p.read_bytes()).hexdigest()
        for p in sorted((out/'current/generated').glob('*')) if p.is_file()}
    primary=all(x['passed'] for x in records if x['role']!='legacy_diagnostic')
    legacy={'executed':a.compare_legacy}
    if a.compare_legacy:
        old=next(x for x in records if x['lane']=='baseline' and x['test']=='world_adapter')
        current=next(x for x in records if x['lane']=='current' and x['test']=='world_adapter')
        legacy.update(both_suites_passed=old['passed'] and current['passed'],
            assertion_parity_passed=old['compile_exit']==current['compile_exit']==0 and
                old.get('run_exit')==current.get('run_exit') and old.get('run_exit') in (0,1) and
                not old.get('sanitizer_error') and not current.get('sanitizer_error') and
                old.get('failures')==current.get('failures'),
            note='Identical legacy failures are disclosed existing issues, never counted as passing assertions or successful negative controls.')
    report={'passed':primary and legacy.get('assertion_parity_passed',True),
        'primary_gates_passed':primary,'legacy_world':legacy,'results':records,'source_sha256':hashes,'generated_sha256':generated,
        'limits':'Actual extracted production paths, synthetic native memory/rays/IO. No native game or headset run. A negative control must compile and fail assertions without sanitizer faults. Legacy parity does not establish that the broad historical suite passes.'}
    (out/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n')
    return 0 if report['passed'] else 1

if __name__=='__main__':raise SystemExit(main())
