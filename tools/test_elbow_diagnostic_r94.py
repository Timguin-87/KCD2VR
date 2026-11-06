#!/usr/bin/env python3
"""Exercise the production R89 solver, F7 serializer and exact replay consumer.

The input motion is synthetic. No game, rig, native memory or headset is used.
Positive rows must replay exactly; altered output and solver weights must fail.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys
import host_compiler


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('output',type=Path);p.add_argument('--compiler',default=None)
    a=p.parse_args();root=Path(__file__).resolve().parents[1];out=a.output.resolve()
    if out==root or root in out.parents:p.error('Keep evidence outside the source tree.')
    out.mkdir(parents=True,exist_ok=True)
    source=root/'tests/elbow_diagnostic_r94_test.cpp';exe=host_compiler.executable_path(out/'elbow_diagnostic_r94')
    command=host_compiler.command(source,exe,[root/'src'],a.compiler)
    built=subprocess.run(command,capture_output=True,text=True)
    (out/'build.log').write_text(built.stdout+built.stderr)
    report={'scope':'Actual solver and production F7 serializer; synthetic motion; exact recorded prior states.',
            'game':'NOT_RUN','headset':'NOT_RUN','build_exit':built.returncode,'cases':[],
            'source_hashes':{str(x.relative_to(root)):hashlib.sha256(x.read_bytes()).hexdigest() for x in
                (source,root/'src/vrik/elbow_r89.h',root/'src/diagnostics/vrik_trace_h29.h',root/'tools/elbow_exact_r94.cpp')}}
    if built.returncode==0:
        run=subprocess.run([str(exe)],capture_output=True,text=True,timeout=30)
        report['fixture_exit']=run.returncode
        (out/'fixture.log').write_text(run.stdout+run.stderr)
        if run.returncode==0:
            changed=re.sub(r'(VRIK_F7 R89_OUTPUT[^\n]* output=)\([^)]*\)',r'\g<1>(9,9,9)',run.stdout,count=1)
            policy=run.stdout.replace('weights=(0.75,1,0.5,2)','weights=(0.76,1,0.5,2)',1)
            for name,text,expected in (('actual',run.stdout,0),('altered_output',changed,1),('altered_policy',policy,1)):
                log=out/(name+'.log');log.write_text(text)
                cmd=[sys.executable,str(root/'tools/replay_elbow_r94.py'),str(log),str(out/name)]
                if a.compiler:cmd+=['--compiler',a.compiler]
                replay=subprocess.run(cmd,capture_output=True,text=True,timeout=90)
                (out/(name+'_replay.log')).write_text(replay.stdout+replay.stderr)
                result=json.loads((out/name/'RESULT.json').read_text()) if (out/name/'RESULT.json').exists() else {}
                passed=replay.returncode==expected and result.get('status')==('PASS' if expected==0 else 'MISMATCH')
                report['cases'].append({'name':name,'exit':replay.returncode,'expected_exit':expected,'passed':passed,'replay':result})
    report['status']='PASS' if len(report['cases'])==3 and all(x['passed'] for x in report['cases']) else 'FAIL'
    (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({'status':report['status'],'cases':[(x['name'],x['passed']) for x in report['cases']]},indent=2))
    return 0 if report['status']=='PASS' else 1


if __name__=='__main__':raise SystemExit(main())
