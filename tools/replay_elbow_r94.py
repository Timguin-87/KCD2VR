#!/usr/bin/env python3
"""Replay actual R94 F7 R89 inputs; do not substitute inferred hand axes.

Usage: python tools/replay_elbow_r94.py KCD2VR.log OUTPUT_DIRECTORY
Each case restores its recorded prior state. No uninterrupted capture assumed.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import host_compiler


def parse(text):
    return dict(re.findall(r'(\w+)=(\([^)]*\)|[^\s]+)', text))


def values(row, key):
    return row[key].strip('()').split(',')


def main():
    p=argparse.ArgumentParser();p.add_argument('log',type=Path);p.add_argument('output',type=Path)
    p.add_argument('--compiler',default=None);a=p.parse_args();root=Path(__file__).resolve().parents[1]
    a.output.mkdir(parents=True,exist_ok=True);groups={}
    with a.log.open(errors='replace') as stream:
        for line in stream:
            for stage in ('R89_INPUT','R89_OUTPUT','ELBOW_INPUT'):
                if 'VRIK_F7 '+stage+' ' not in line: continue
                r=parse(line);key=tuple(r.get(k) for k in ('session','kind','observerOrdinal','sample','side'))
                if None not in key: groups.setdefault(key,{})[stage]=r
                break
    rows=[];identities=[];partial=0
    for key,g in groups.items():
        if 'R89_INPUT' not in g: continue
        if not all(k in g for k in ('R89_INPUT','R89_OUTPUT','ELBOW_INPUT')): partial+=1;continue
        i,o,c=g['R89_INPUT'],g['R89_OUTPUT'],g['ELBOW_INPUT']
        row=[c[k] for k in ('epoch','sample','sourceTick','timeNs','priorReady','priorEpoch','priorSample','priorTick','priorTimeNs')]
        for k in ('priorAxis','priorBend'): row+=values(c,k)
        for k in ('shoulder','wrist','lengths','body','forward','thumb','torsoA','torsoB','radius','seedPresent','seed'): row+=values(i,k)
        for k in ('weights','tau','rate','confidenceScale','torsoMargin','torsoBand'): row+=values(i,k)
        row+=[o['ok']]+values(o,'output');rows.append(' '.join(row));identities.append(key)
    report={'complete_rows':len(rows),'partial_rows':partial,'game':'NOT_RUN','headset':'NOT_RUN',
            'input_sha256':hashlib.sha256(a.log.read_bytes()).hexdigest(),
            'solver_sha256':hashlib.sha256((root/'src/vrik/elbow_r89.h').read_bytes()).hexdigest()}
    if not rows:
        report['status']='NO_COMPLETE_ACTUAL_R89_RECORDS';(a.output/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status']);return 2
    source=root/'tools/elbow_exact_r94.cpp';exe=host_compiler.executable_path(a.output/'elbow_exact_r94')
    command=host_compiler.command(source,exe,[root/'src'],a.compiler)
    built=subprocess.run(command,capture_output=True,text=True)
    (a.output/'build.log').write_text(built.stdout+built.stderr);report['build_exit']=built.returncode
    data='\n'.join(rows)+'\n';(a.output/'observed_rows.txt').write_text(data)
    (a.output/'identities.json').write_text(json.dumps(identities,indent=2)+'\n')
    if built.returncode==0:
        result=subprocess.run([str(exe)],input=data,text=True,capture_output=True,timeout=30)
        report.update(run_exit=result.returncode,output=result.stdout+result.stderr,status='PASS' if result.returncode==0 else 'MISMATCH')
    else:report['status']='COMPILE_FAILED'
    (a.output/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
    return 0 if report.get('status')=='PASS' else 1


if __name__=='__main__':raise SystemExit(main())
