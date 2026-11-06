"""Actual worker-slot/map detector composition and bounded knife readiness; no DLL/PAK."""
from pathlib import Path
import argparse,subprocess,zipfile,json,hashlib,re
from native_research112 import PE
ROOT=Path(__file__).resolve().parents[1]
VC='C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--out',required=True,type=Path);ap.add_argument('--baseline-zip',required=True,type=Path);ap.add_argument('--dll',required=True,type=Path);a=ap.parse_args()
 out=a.out.resolve();assert ROOT!=out and ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True);rows={}
 mod=(ROOT/'src/mod.cpp').read_text()
 start=mod.index('            auto padLeft5916c3=routeL,padRight5916c3=routeR;')
 end=mod.index('\n',mod.index('if(held_map_m1::Selected()',start))
 carrier=mod[start:end]+'\n';(out/'m35_native_carrier.inc').write_text(carrier)
 assert mod.index('KCD2VR_M2_HOST_SLOT 6001')<mod.index('touch_events=touch_bindings_.Update(routeL,routeR')
 assert mod.index('touch_events=touch_bindings_.Update(routeL,routeR')>end
 def run(name,file,asan=True):
  cmd=out/(name+'.cmd');cmd.write_text('@echo off\ncall "'+VC+'" >nul\ncd /d "'+str(out)+'"\ncl /nologo /std:c++latest /EHsc /Od /Zi /UNDEBUG '+('/fsanitize=address ' if asan else '')+'/I "'+str(ROOT/'src')+'" /I "'+str(out)+'" /I "D:/KCD2VR-review-builds/T15_GHOST_THROW_HOLSTERS_20261021/windows/_deps/openxr_sdk_source-build/include" /I "E:/backup/DetourModKit/include" /I "E:/backup/DetourModKit/include/DirectXMath" "'+str(file)+'" /Fe:'+name+'.exe /link user32.lib\nif errorlevel 1 exit /b %errorlevel%\n'+name+'.exe\nexit /b %errorlevel%\n')
  r=subprocess.run(['cmd','/d','/c',str(cmd)],capture_output=True,text=True);log=r.stdout+r.stderr;(out/(name+'.log')).write_text(log);rows[name]={'exit':r.returncode,'log':str(out/(name+'.log'))};print(name,r.returncode,log[-1000:]);return r.returncode,log
 for asan in (False,True):
  assert run('map_worker_'+('asan' if asan else 'debug'),ROOT/'tests/held_map_worker_m35_test.cpp',asan)[0]==0
 with zipfile.ZipFile(a.baseline_zip) as z:old=z.read('src/subsystems/vrmap/host.inl')
 oldhost=out/'old_host.inl';oldhost.write_bytes(old)
 fixture=(ROOT/'tests/held_map_worker_m35_test.cpp').read_text().replace('../src/subsystems/hud_ui_controls/runtime/touch_bindings.cpp',(ROOT/'src/subsystems/hud_ui_controls/runtime/touch_bindings.cpp').as_posix()).replace('../src/subsystems/vrmap/host.inl',oldhost.as_posix())
 red=out/'before_worker.cpp';red.write_text(fixture);code,log=run('before_worker',red)
 assert code!=0 and 'Requested()' in log,'baseline must compile then fail actual 1000ms map request'
 assert run('retained_detector',ROOT/'tests/held_map_touch_h24_test.cpp')[0]==0
 fixture=(ROOT/'tests/subsystems/throw_drop/native_ammo_service_t22_test.cpp').read_text()
 old='static void AmmoNoteT18(const char*,int,std::uint64_t,std::uint64_t,const char*){}'
 new='static unsigned preflightNotes28=0;static void AmmoNoteT18(const char*,int,std::uint64_t,std::uint64_t,const char* detail){if(detail&&std::strstr(detail,"T27 LEFT_CHEST preflight="))++preflightNotes28;}'
 assert fixture.count(old)==1;fixture=fixture.replace(old,new)
 first=fixture.index('    for(int side=0;side<2;++side)for(int family=0;family<2;++family)for(int count:{1,3,5}){',fixture.index('int main()'))
 last=fixture.index('    // T26: actual service, separate world-held LEFT chest knife; RIGHT unchanged.',first)
 fixture=fixture[:first]+fixture[last:]
 marker='    std::printf("{\\"checks\\":'
 index=fixture.index(marker)
 fixture=fixture[:index]+'''    Reset(5,0);in.chestKnivesT26=true;
    for(int cycle=0;cycle<100;++cycle){now+=1100;st2::enabled=(cycle%2)==0;Tick(0,0);}
    Check(preflightNotes28==16);
'''+fixture[index:]
 (out/'world.cpp').write_text(fixture)
 native=PE(a.dll);assert hashlib.sha256(native.data).hexdigest()=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
 runtime=(ROOT/'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl').read_text();pins=[]
 for name,rva in [('slot',0x7F1E3C),('filename',0x1A81E20),('helper',0x51EF4C),('selected',0x2AAD1AD),('worldDisableT26',0xABDFF8),('proxyEnableT26',0xABE028),('getProxyT26',0x5D70C4)]:
  match=re.search(r'char '+name+r'\[\]\{([^}]+)\}',runtime);value=bytes(int(x.strip(),0) for x in match[1].split(','));assert native.read(rva,len(value))==value;pins.append((rva,value))
 (out/'pins_t22.inc').write_text('\n'.join('{0x%X,{%s}},'%(r,','.join('0x%02x'%x for x in v)) for r,v in pins))
 assert run('enabled_world',out/'world.cpp')[0]==0
 quiet=out/'quiet.cpp';quiet.write_text('''#include "diagnostics/capture168.h"
#include <cassert>
int main(){using namespace kcd2vr::diagnostics::capture168;
gate.mask=0;
assert(essential("T18 AMMO stage=LEFT_CHEST_PREFLIGHT T27 LEFT_CHEST preflight=127"));
assert(!essential("T18 AMMO stage=HELD_NATIVE_PREVIEW"));
assert(essential("H25 MAP_GESTURE route=DIRECT_HELD_PARCHMENT_M31"));
}
''')
 assert run('quiet_classifier',quiet)[0]==0
 inputs=['src/mod.cpp','src/subsystems/vrmap/host.inl','src/subsystems/hud_ui_controls/runtime/touch_bindings.cpp','src/diagnostics/capture168.h','src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl','tests/held_map_worker_m35_test.cpp','tests/held_map_touch_h24_test.cpp','tests/subsystems/throw_drop/native_ammo_service_t22_test.cpp']
 report={'passed':True,'runs':rows,'native_game':False,'headset':False,'actual_detector_and_worker_clauses':True,'native_carrier_sha256':hashlib.sha256(carrier.encode()).hexdigest(),'input_sha256':{n:hashlib.sha256((ROOT/n).read_bytes()).hexdigest() for n in inputs},'limits':['Readiness is not native render/unfolding or damage acceptance','Baseline causal failure occurs at expected actual map-request assertion','No OS input injection: test spy intercepts SendInput','Native leaves in world service substituted']}
 (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':main()
