#!/usr/bin/env python3
"""Compile actual D83 finisher code against controlled native leaves; optionally
run the same probes against the immutable Claude tree as negative controls.
This is a host contract test, not a Windows ABI or headset rendering test.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
from pose_restore_extract_r96 import shared as extract_restore_shared

def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()

def block(text, prefix):
    at = text.index(prefix)
    start = text.index('{', at)
    depth = 1
    end = start + 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    if text[end:end+1] == ';':
        end += 1
    return text[at:end]

def extract(root, dest):
    cpp = (root/'src/frik_pose_probe.cpp').read_text()
    coh = (root/'src/vrik/coherent_pose.inl').read_text()
    prefixes = ['struct QuatT28 ', 'struct RigMap ', 'struct V3 ', 'struct Q4 ', 'struct TrackingSnapshot ']
    parts = [block(cpp, p) for p in prefixes]
    parts += [re.search(r'using '+n+r' = .*?;', cpp).group() for n in ('PoseGetFn', 'PoseSetFn')]
    names = ['Add','Sub','Mul','Dot','Cross','Len','Normalize','FromT','ToT','QConj','QMul','QNormalize','QFromQuatT','QToQuatT','XrToCryQ','QRotateV','RelativeFromAbsolute','AbsoluteFromRelative']
    for n in names:
        m = re.search(r'static \w+ '+n+r'\(', cpp)
        parts.append(block(cpp, m.group()))
    parts.append(block(coh, 'static bool ValidPose6689('))
    dest.write_text('\n\n'.join(parts)+'\n')
    extract_restore_shared(root,dest.parent/'pose_restore_shared_r96.inc')
    return {'frik_pose_probe.cpp':sha(root/'src/frik_pose_probe.cpp'), 'coherent_pose.inl':sha(root/'src/vrik/coherent_pose.inl'), 'extracted_helpers':sha(dest), 'literal_functions':names+['ValidPose6689']}

def run_one(root, fixture, out, compiler, flags):
    out.mkdir(parents=True, exist_ok=True)
    provenance = extract(root,out/'finisher_production_helpers.inc')
    command=[compiler,'-std=c++20',*flags,'-I',str(out),'-I',str(root/'src'),str(fixture),'-o',str(out/'finisher_test')]
    built=subprocess.run(command,text=True,capture_output=True)
    (out/'compile.txt').write_text(built.stdout+built.stderr)
    record={'root':str(root),'command':command,'compile_exit':built.returncode,'provenance':provenance,
            'header_sha256':sha(root/'src/vrik/finisher_upper_d83.h'),'implementation_sha256':sha(root/'src/vrik/finisher_upper_d83.inl')}
    if built.returncode:
        raise RuntimeError(f'Compilation failed; see {out/"compile.txt"}')
    env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1',UBSAN_OPTIONS='halt_on_error=1')
    tested=subprocess.run([str(out/'finisher_test')],text=True,capture_output=True,env=env)
    (out/'run.txt').write_text(tested.stdout+tested.stderr)
    record.update(exit=tested.returncode,passed=re.findall(r'^PASS (\S+)',tested.stdout,re.M),failed=re.findall(r'^FAIL (\S+)',tested.stdout,re.M))
    return record

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1])
    parser.add_argument('--original',type=Path)
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args()
    root=args.source.resolve();out=args.out.resolve();out.mkdir(parents=True,exist_ok=True)
    fixture=root/'tests/finisher_upper_d83_test.cpp'
    compiler=shutil.which('clang++') or shutil.which('g++')
    if not compiler:raise RuntimeError('C++20 host compiler required')
    records={}
    for name,flags in [('debug',['-O0','-g']),('optimized',['-O2']),('sanitized',['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer'])]:
        records[name]=run_one(root,fixture,out/name,compiler,flags)
    if args.original:
        records['claude_negative_control']=run_one(args.original.resolve(),fixture,out/'claude_negative_control',compiler,['-O0','-g'])
    result={'scope':'Actual finisher header/inl and literal production pose math; native access, clock, diagnostics and context publications are controlled leaves. No Windows ABI, full DLL or headset claim.',
            'fixture_sha256':sha(fixture),'runs':records}
    ok=all(records[n]['exit']==0 and len(records[n]['passed'])>=40 for n in ('debug','optimized','sanitized'))
    if args.original:
        original=records['claude_negative_control'];must_fail={'source_reset_retains_pose_epoch_but_rejects_latch','shared_restore_fault_blocks_overlay','policy_change_during_write_rolls_back','failed_restore_invalidates_shared_publication','scope_unwind_restores','torso_attachment_native'}
        ok=ok and original['exit']==1 and must_fail.issubset(set(original['failed']))
    result['passed']=ok
    (out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'passed':ok,'runs':{k:{'passed':len(v['passed']),'failed':len(v['failed']),'exit':v['exit']} for k,v in records.items()},'result':str(out/'RESULT.json')},indent=2))
    return 0 if ok else 1

if __name__=='__main__':raise SystemExit(main())
