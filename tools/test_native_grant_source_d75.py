"""D75 actual service/grant + production carrier source predicate; native leaves substituted."""
import argparse,hashlib,json,re,subprocess
from pathlib import Path

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True);args=ap.parse_args()
 root=Path(__file__).resolve().parents[1];out=args.out.resolve();assert root!=out and root not in out.parents;out.mkdir(parents=True,exist_ok=True)
 runtime=root/'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl';carrier=root/'src/subsystems/throw_drop/runtime/native_ammo_carrier_t18.inl'
 base=root/'tests/subsystems/throw_drop/native_ammo_service_t22_test.cpp';test=root/'tests/subsystems/throw_drop/native_grant_source_d75_test.cpp'
 source=runtime.read_text();bf=base.read_text();car=carrier.read_text()
 def extract(text,name,next_name):return text[text.index(name):text.index(next_name,text.index(name))].strip()
 # Prove fixture's carrier dependency matches current production exactly.
 name='static bool AmmoCharacterCurrentK33(';end='static bool AmmoInputT18('
 actual=extract(car,name,end)
 fixture_begin=bf.index(name);fixture_end=bf.index('static bool AmmoOwnerT18R11(',fixture_begin)
 assert bf[fixture_begin:fixture_end].strip()==actual
 old='static bool AmmoInputT18(const st2::Input& in,int side,std::uint64_t t){assert(!finalThrowLease(side));return inputReady&&side>=0&&side<2&&in.allowed&&in.pose18[side]&&st2::fresh(t,in.tick,80);}'
 assert bf.count(old)==1
 bf=bf.replace(old,'static bool AmmoCharacterCurrentK33(const st2::Input&,std::uint64_t) noexcept;\n'+old.replace('&&st2::fresh(t,in.tick,80);','&&st2::fresh(t,in.tick,80)&&AmmoCharacterCurrentK33(in,t);'))
 pins=[]
 for name,rv in [('slot',0x7F1E3C),('filename',0x1A81E20),('helper',0x51EF4C),('selected',0x2AAD1AD),('worldDisableT26',0xABDFF8),('proxyEnableT26',0xABE028),('getProxyT26',0x5D70C4),('hideD72',0x87963C)]:
  m=re.search(r'char '+name+r'\[\]\{([^}]+)\}',source);pins.append((rv,bytes(int(v.strip(),0) for v in m[1].split(','))))
 (out/'pins_t22.inc').write_text('\n'.join('{0x%X,{%s}},'%(rv,','.join('0x%02x'%x for x in v)) for rv,v in pins))
 def run(name,body,extra):
  local=out/name;local.mkdir(exist_ok=True)
  (local/'carrier_source_fixture_d75.inc').write_text(body)
  cmd=local/'run.cmd';cmd.write_text('@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncd /d "'+str(local)+'"\ncl /nologo /std:c++20 /EHsc '+extra+' /Zi /UNDEBUG /I "'+str(root/'src')+'" /I "'+str(local)+'" /I "'+str(out)+'" "'+str(test)+'" /Fe:probe.exe\nif errorlevel 1 exit /b %errorlevel%\nprobe.exe\nexit /b %errorlevel%\n')
  r=subprocess.run(['cmd.exe','/d','/c',str(cmd)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(local/'RUN.log').write_text(r.stdout);return r
 pos=run('debug',bf,'/Od');san=run('asan',bf,'/O2 /fsanitize=address')
 mutant=source
 for old,new in [('AmmoServiceGrantInputD75(h,frame,grant,now)','AmmoInputT18(frame,h.side,now)'),('AmmoServiceGrantInputD75(h,grant->frame,grant,GetTickCount64())','AmmoInputT18(grant->frame,side,GetTickCount64())')]:
  assert mutant.count(old)==1;mutant=mutant.replace(old,new)
 mp=out/'original_carrier_source.inl';mp.write_text(mutant)
 include='#include "subsystems/throw_drop/runtime/native_ammo_service_t22.inl"';assert bf.count(include)==1
 neg=run('baseline_negative',bf.replace(include,'#include "'+mp.as_posix()+'"'),'/Od')
 rows=[next((x for x in r.stdout.splitlines() if x.startswith('{"checks":')),None) for r in (pos,san)]
 negative=neg.returncode!=0 and 'check=3 ' in neg.stdout and 'FAILED line=' in neg.stdout
 report={'passed':pos.returncode==san.returncode==0 and all(rows) and negative,'debug':json.loads(rows[0]) if rows[0] else None,'asan':json.loads(rows[1]) if rows[1] else None,'negative_control':{'causal':negative,'exit':neg.returncode,'change':'restore the two original carrier calls; keep all fixtures and checks'},'native_execution':False,'hashes':{str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in (runtime,carrier,base,test,Path(__file__),root/'src/subsystems/throw_drop/pending_reach_t23.h')}}
 (out/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n');print(pos.stdout);print(san.stdout);print('negative',negative);return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
