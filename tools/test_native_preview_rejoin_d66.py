"""Actual T22 service + grant code, bounded D66 same-stack refresh checks.
Uses existing native-leaf fixture substitutes. No DLL load/install/full build.
"""
import argparse,hashlib,json,re,subprocess
from pathlib import Path

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True);a=ap.parse_args()
 root=Path(__file__).resolve().parents[1];out=a.out.resolve();assert root!=out and root not in out.parents;out.mkdir(parents=True,exist_ok=True)
 runtime=root/'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl'
 basefixture=root/'tests/subsystems/throw_drop/native_ammo_service_t22_test.cpp'
 fixture=root/'tests/subsystems/throw_drop/native_preview_rejoin_d66_test.cpp'
 source=runtime.read_text();pins=[]
 for name,rv in [('slot',0x7F1E3C),('filename',0x1A81E20),('helper',0x51EF4C),('selected',0x2AAD1AD),('worldDisableT26',0xABDFF8),('proxyEnableT26',0xABE028),('getProxyT26',0x5D70C4)]:
  m=re.search(r'char '+name+r'\[\]\{([^}]+)\}',source);pins.append((rv,bytes(int(v.strip(),0) for v in m[1].split(','))))
 (out/'pins_t22.inc').write_text('\n'.join('{0x%X,{%s}},'%(rv,','.join('0x%02x'%x for x in v)) for rv,v in pins))
 def run(name,cpp):
  cmd=out/(name+'.cmd');cmd.write_text('@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncd /d "'+str(out)+'"\ncl /nologo /std:c++20 /EHsc /O2 /Zi /UNDEBUG /fsanitize=address /I "'+str(root/'src')+'" /I "'+str(out)+'" "'+str(cpp)+'" /Fe:'+name+'.exe\nif errorlevel 1 exit /b %errorlevel%\n'+name+'.exe\nexit /b %errorlevel%\n')
  r=subprocess.run(['cmd.exe','/d','/c',str(cmd)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(out/(name+'.log')).write_text(r.stdout);return r
 pos=run('rejoin',fixture)
 marker='||AmmoServiceRefreshPreviewGrantD66(ctx,q,h)';assert source.count(marker)==1
 mutant=out/'disabled_rejoin.inl';mutant.write_text(source.replace(marker,'||false'))
 bf=basefixture.read_text();inc='#include "subsystems/throw_drop/runtime/native_ammo_service_t22.inl"';assert bf.count(inc)==1
 mirror=out/'disabled_fixture.cpp';mirror.write_text(bf.replace(inc,'#include "'+mutant.as_posix()+'"'))
 test=out/'disabled_rejoin_test.cpp';test.write_text(fixture.read_text().replace('#include "native_ammo_service_t22_test.cpp"','#include "'+mirror.as_posix()+'"'))
 neg=run('disabled',test)
 first=next(i for i,s in enumerate(test.read_text().splitlines(),1) if 'Check(AmmoServiceTransientOwnerT22(context,q));Check(h.preview' in s)
 red=neg.returncode!=0 and ('FAILED line='+str(first)+' check=1 ') in neg.stdout
 dependencies=[runtime,basefixture,fixture,Path(__file__),root/'src/subsystems/throw_drop/pending_reach_t23.h',root/'src/subsystems/throw_drop/runtime/native_transient_ammo_t20.inl',root/'src/subsystems/throw_drop/native_transient_ammo_t20.h']
 line=next((x for x in pos.stdout.splitlines() if x.startswith('{"checks":')),None)
 report={'passed':pos.returncode==0 and bool(line) and red,'positive':json.loads(line) if line else None,'positive_exit':pos.returncode,'negative_control':{'passed':red,'exit_code':neg.returncode,'expected_first_check_line':first,'removed_only':marker},'native_execution':False,'optimized_msvc_asan':True,'source_sha256':{str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in dependencies}}
 (out/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n');print(pos.stdout);print('Causal removal RED:',red);return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
