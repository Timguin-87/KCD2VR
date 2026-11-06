"""Extract actual T18 contact/flight and T22 scope/drain bodies; explicit native leaves; Debug+ASAN only."""
from pathlib import Path
import argparse,hashlib,json,re,subprocess
ROOT=Path(__file__).resolve().parents[1]
def fn(text,name):
 m=re.search(r'static [^\n]+\b'+name+r'\(',text);assert m,name
 start=m.start();i=text.index('{',m.end())+1;depth=1
 while depth:
  if text[i]=='{':depth+=1
  elif text[i]=='}':depth-=1
  i+=1
 return text[start:i]+'\n'
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--build-dir',type=Path,required=True);ap.add_argument('--out',type=Path,required=True);a=ap.parse_args();out=a.build_dir.resolve();assert ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
 carrier=ROOT/'src/subsystems/throw_drop/runtime/native_ammo_carrier_t18.inl';service=ROOT/'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl';fixture=ROOT/'tests/subsystems/throw_drop/cause_receipt_t25_test.cpp'
 c=carrier.read_text();s=service.read_text();parts={
 'types.inc':c[c.index('struct AmmoFlightT18 {'):c.index('static std::optional<',c.index('struct AmmoFlightT18 {'))],
 'capture.inc':fn(c,'AmmoCaptureCauseContactT25'),'observer.inc':fn(c,'ArrowContactT18'),'track.inc':fn(c,'AmmoTrackFlightT18'),
 'tick.inc':fn(c,'AmmoTickFlightsT18'),'scope.inc':s[s.index('struct AmmoFlightScopeT22 {'):s.index('static std::atomic<bool> g_ammo_hooks_ready22')],
 'drain.inc':fn(s,'AmmoDrainCauseContactT25'),'service_tick.inc':fn(s,'AmmoServiceTickFlightsT22')}
 for n,t in parts.items():(out/n).write_text(t)
 vc='C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat';runs=[]
 for mode in ['Debug','ASAN']:
  cmd=out/(mode+'.cmd');cmd.write_text('@echo off\ncall "'+vc+'" >nul\ncd /d "'+str(out)+'"\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG '+('/fsanitize=address ' if mode=='ASAN' else '')+'/I "'+str(ROOT/'src')+'" /I "'+str(out)+'" "'+str(fixture)+'" /Fe:'+mode+'.exe\nif errorlevel 1 exit /b %errorlevel%\n'+mode+'.exe\nexit /b %errorlevel%\n')
  r=subprocess.run(['cmd.exe','/d','/c',str(cmd)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(out/(mode+'.log')).write_text(r.stdout);print(r.stdout)
  lines=[x for x in r.stdout.splitlines() if x.startswith('{"checks":')];runs.append(dict(mode=mode,exit_code=r.returncode,fixture=json.loads(lines[-1]) if lines else None))
 # Causal RED: wrong-payload evidence must not be sampled as this flight.
 mutant=parts['capture.inc'];assert mutant.count('||key!=id.key')==1
 (out/'capture_mutant.inc').write_text(mutant.replace('||key!=id.key',''))
 probe=out/'cause_receipt_mutant.cpp';probe.write_text(fixture.read_text().replace('#include "capture.inc"','#include "capture_mutant.inc"'))
 cmd=out/'CAUSAL.cmd';cmd.write_text('@echo off\ncall "'+vc+'" >nul\ncd /d "'+str(out)+'"\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /I "'+str(ROOT/'src')+'" /I "'+str(out)+'" "'+str(probe)+'" /Fe:causal.exe\nif errorlevel 1 exit /b %errorlevel%\ncausal.exe\nexit /b %errorlevel%\n')
 nr=subprocess.run(['cmd.exe','/d','/c',str(cmd)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(out/'CAUSAL.log').write_text(nr.stdout)
 causal=nr.returncode!=0 and 'causeReads==0&&notes.empty()' in nr.stdout
 paths=[carrier,service,fixture,Path(__file__),ROOT/'src/core/snapshot_mailbox.h',ROOT/'src/core/native_grasp13.h',ROOT/'src/vrik/native_supply_t2.h',ROOT/'src/subsystems/throw_drop/projectile_flight_t17.h']
 report=dict(native_execution=False,production_build=False,all_passed=all(r['exit_code']==0 and r['fixture'] for r in runs) and causal,runs=runs,causal_wrong_payload_guard={'passed':causal,'exit_code':nr.returncode,'log':str(out/'CAUSAL.log'),'mutation':'remove ONLY ||key!=id.key'},
 input_sha256={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},
 extracted_sha256={n:hashlib.sha256((out/n).read_bytes()).hexdigest() for n in parts},
 actual_bodies=list(parts),substituted_leaves=['bounded native-memory reads','current native entity/item identity','existing slot/status/velocity leaves','current script capability and pure manager/world getters','mailbox busy/unknown injection around actual SnapshotMailbox','owner log sink'],
 limits=['No native RPG result or damage acceptance proof','Early collision before post-Shoot registration can be absent','A native original may retire entity/payload before owner drain; strict current join then loses the diagnostic','No callback IO, new native query, hit gate, lifetime renewal, retry or cleanup policy change'])
 a.out.write_text(json.dumps(report,indent=2)+'\n');return 0 if report['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
