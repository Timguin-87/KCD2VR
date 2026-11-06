#!/usr/bin/env python3
"""Focused current-production passenger binding and right quiver region checks."""
from pathlib import Path
import argparse,hashlib,json,os,subprocess
from check_crossbow_d82 import function
from host_compiler import command,executable_path
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True);ap.add_argument('--compiler',default='cl');a=ap.parse_args()
 out=a.out.resolve();assert ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
 rp=ROOT/'src/subsystems/ranged_reload';adapterpath=rp/'crossbow_native_xr34.inl';passpath=rp/'reload_passenger_xr34.inl';rangepath=rp/'crossbow_rifle_request_h21.inl'
 adapter=adapterpath.read_text();start=adapter.index('    __try{started=g_crossbow_xr34.startNewAfterRestore(');endtxt='__finally{(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}';end=adapter.index(endtxt,start)+len(endtxt)
 body=adapter[start:end].replace('    __try{','    {struct HostFinishD82 {ma::Token& token;bool& started;~HostFinishD82(){(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}} finish{token,started};',1).replace('}\n    '+endtxt,'}')
 adapter=adapter[:start]+body+adapter[end:]
 passenger=''.join(function(passpath.read_text(),s) for s in ['static bool ReloadNativeClockCurrentD86(','static bool ReloadPassengerCurrentXR34(','static bool ReadReloadPassengerXR34(','static bool ReloadPoseWriterCurrentXR34('])
 completed=function(rangepath.read_text(),'static bool RangedRawIntentD78(')+function(passpath.read_text(),'static xcXR34::Source CompletedReloadPoseSourceXR34(')
 geometry=function(passpath.read_text(),'template<class Snapshot> static void ChestFlagsD66C(')
 names=['crossbow_native_xr34_test.cpp','crossbow_scope_d82_test.cpp','crossbow_pending_passenger_r95_test.cpp']
 fixtures={n:(ROOT/'tests'/n).read_text().replace('"../src/','"'+(ROOT/'src').as_posix()+'/') for n in names}
 fixtures[names[0]]=fixtures[names[0]].replace('"'+adapterpath.as_posix()+'"','"native_adapter_host.inc"')
 runs=[]
 def run(name,pa=passenger,geo=geometry,optimized=False,negative=False):
  d=out/name;d.mkdir(exist_ok=True)
  for n,s in fixtures.items():(d/n).write_text(s)
  for n,s in [('native_adapter_host.inc',adapter),('passenger_consumer_d82_actual.inc',pa),('completed_source_d82_actual.inc',completed),('quiver_geometry_r95_actual.inc',geo)]:(d/n).write_text(s)
  exe=executable_path(d/name);cmd=command(d/names[-1],exe,[ROOT/'src',ROOT/'tests/syntax_shims',d],a.compiler)
  if not optimized:cmd=[('/Od' if x=='/O2' else '-O0' if x=='-O2' else x) for x in cmd]
  built=subprocess.run(cmd,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(d/'build.log').write_text(built.stdout)
  if built.returncode:raise RuntimeError(name+' compile failed\n'+built.stdout[-7000:])
  result=subprocess.run([str(exe)],text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(d/'run.log').write_text(result.stdout)
  passed=(result.returncode==1 and 'FAIL_PENDING_R95' in result.stdout) if negative else result.returncode==0
  runs.append({'name':name,'passed':passed,'negative':negative,'exit_code':result.returncode,'output':result.stdout,'command':cmd});print(name+': '+result.stdout,end='',flush=True)
  if not passed:raise RuntimeError(name+' unexpected result')
 run('debug');run('optimized',optimized=True)
 run('old_pending_rejected',pa=passenger.replace('if(b.handBoltPendingR95){bool pending=false;','if(b.handBoltPendingR95){return false;bool pending=false;'),negative=True)
 old='    out.handInQuiverD72[0]=relative.x>.06f&&relative.x<.55f&&relative.y>-.35f&&relative.y<.25f&&\n        relative.z>.06f&&relative.z<.55f;'
 assert old in geometry
 run('old_quiver_sphere',geo=geometry.replace(old,''),negative=True)
 paths=[adapterpath,passpath,rangepath,rp/'crossbow_native_xr34.h',Path(__file__),*[ROOT/'tests'/n for n in names]]
 (out/'RESULT.json').write_text(json.dumps({'passed':all(r['passed'] for r in runs),'runs':runs,'sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},'limits':['Production passenger consumer and quiver geometry execute. Native attachment completion/timeouts are explicit leaves, covered separately by bolt adapter suite.','No game, DLL, headset, inventory or actual rendered pose is executed.']},indent=2)+'\n')
if __name__=='__main__':main()
