#!/usr/bin/env python3
"""Run actual bow preparation -> foundation -> eye isolation with explicit host leaves."""
from pathlib import Path
import argparse,hashlib,json,os,re,subprocess,sys

ROOT=Path(__file__).resolve().parents[1]

def sha(data):return hashlib.sha256(data).hexdigest()
def extract(source,out):
    prepare_path=source/'src/subsystems/physics/native_handling_profile.inl'
    foundation_path=source/'src/subsystems/vrik/pose_frame_foundation.inl'
    prepare=prepare_path.read_text();prepare=prepare[prepare.index('static bool PhysPrepare6698('):]
    foundation=foundation_path.read_text()
    token='    const auto cameraBowD37B='
    if token not in foundation:token='    fi51.bow=prepared169r2&&bowProfile169r2.bow();'
    a=foundation.index(token);b=foundation.index('    if(kcd2vr::tracked_frame51::advance',a)
    foundation=foundation[a:b]
    (out/'actual_prepare.inc').write_text(prepare)
    (out/'actual_foundation.inc').write_text(foundation)
    return {'source':str(source),'prepare_source_sha256':sha(prepare_path.read_bytes()),
      'foundation_source_sha256':sha(foundation_path.read_bytes()),'prepare_extraction_sha256':sha(prepare.encode()),
      'foundation_extraction_sha256':sha(foundation.encode()),
      'native_leaves':'No retail execution. Mailbox read/failure and irrelevant profile geometry fill are explicit synthetic leaves. Full PhysPrepare and exact foundation assignment are current source; physical Profile and tracked_frame51::eye are direct includes.'}

def run_case(source,out,compiler,sanitize):
    out.mkdir(parents=True,exist_ok=True);proof=extract(source,out)
    exe=out/'bow_camera_identity_d37b_test'
    command=[compiler,'-std=c++23','-O0','-g','-I',str(out),'-I',str(ROOT/'src'),str(ROOT/'tests/bow_camera_identity_d37b_test.cpp'),'-o',str(exe)]
    if sanitize:command[1:1]=['-fsanitize=address,undefined','-fno-omit-frame-pointer']
    build=subprocess.run(command,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    (out/'build.log').write_text(build.stdout);proof.update(command=command,build_exit=build.returncode)
    if build.returncode==0:
        exe.chmod(exe.stat().st_mode|0o111)
        env=os.environ.copy()
        if sanitize:env['ASAN_OPTIONS']='detect_leaks=0' # LeakSanitizer is unavailable under the executor ptrace wrapper.
        run=subprocess.run([str(exe)],text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,env=env)
        (out/'run.log').write_text(run.stdout);proof['run_exit']=run.returncode
        m=re.search(r'checks=(\d+) failures=(\d+)',run.stdout)
        if m:proof.update(checks=int(m[1]),failures=int(m[2]))
    (out/'results.json').write_text(json.dumps(proof,indent=2)+'\n');return proof

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,required=True);ap.add_argument('--compiler',default=os.environ.get('CXX','g++'))
    ap.add_argument('--before-source',type=Path);ap.add_argument('--sanitize',action='store_true');args=ap.parse_args();args.out=args.out.resolve()
    result={'current':run_case(ROOT,args.out/'current',args.compiler,args.sanitize)}
    ok=result['current'].get('run_exit')==0
    if args.before_source:
        result['before']=run_case(args.before_source.resolve(),args.out/'before',args.compiler,args.sanitize)
        result['negative_control_detects_bug']=result['before'].get('build_exit')==0 and result['before'].get('failures',0)>0
        ok=ok and result['negative_control_detects_bug']
    result['passed']=ok;(args.out/'results.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2));return 0 if ok else 1
if __name__=='__main__':sys.exit(main())
