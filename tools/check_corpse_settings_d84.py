#!/usr/bin/env python3
"""Run literal corpse menu/read/apply/save/load owners with host INI IO leaves."""
import argparse, hashlib, json, os, subprocess
from pathlib import Path
from check_root_d84 import function

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True);p.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1]);p.add_argument('--expect-regression',action='store_true')
 a=p.parse_args();root=a.source.resolve();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
 def read(n):return (root/n).read_text()
 lab=read('src/vrik/lab58.inl');runtime=read('src/subsystems/experimental/live_runtime_d40.inl');mod=read('src/mod.cpp')
 menu=function(read('tests/live_settings_d39_test.cpp'),'struct MenuState ')+';'
 take='\n'.join(x for x in runtime.splitlines() if 'd40.labCorpseDragD40=lab58::' in x)
 assert take
 start=runtime.index('        if(lab58::livingHitImpact.load()!=d40.');end=runtime.index(';',runtime.index('frik_pose_probe::SetLab58(',start))+1;apply=runtime[start:end]
 save='\n'.join(x for x in mod.splitlines() if any('wb("'+key+'"' in x for key in ('VrLabCorpseDrag58','VrLabLivingHitImpact58','VrLabPropWake58')))
 assert len(save.splitlines())==3
 load='\n'.join(x for x in read('src/subsystems/masterstrike_combos/host.inl').splitlines() if 'frik_pose_probe::SetLab58(' in x or 'frik_pose_probe::SetLab58LivingTouch(' in x)
 body='''#include "vrik/lab58.h"
#include "subsystems/experimental/live_settings_d39.h"
#include <map>
#include <string>
#include <cstdio>
#include <cwchar>
static std::uint64_t GetTickCount64(){return 10000;}
namespace kcd2vr::diagnostic_trace {static unsigned lines=0;void Write(const char*){++lines;}void WriteFor(kcd2vr::diagnostics::capture168::Domain,const char*){++lines;}}
namespace kcd2vr::frik_pose_probe {
'''+function(lab,'void SetLab58LivingTouch(')+'\n'+function(lab,'void SetLab58(')+'\n}\n'+menu+'''
using namespace kcd2vr;
static std::map<std::string,bool> ini;
bool ReadIniBoolExplicit(const char*,const char*,const char*k,bool fallback){auto it=ini.find(k);return it==ini.end()?fallback:it->second;}
float ReadIniFloatExplicit(const char*,const char*,const char*,float fallback){return fallback;}
void Read(live_settings_d39::Settings& d40){
'''+take+'''
}
void Apply(const live_settings_d39::Settings& d40){
'''+apply+'''
}
bool Save(bool good=true){bool ok=true;auto wb=[&](const char*k,bool v){if(good)ini[k]=v;return good;};
'''+save+'''
return ok;}
void Load(){const char*explicit_ini="memory.ini";
'''+load+'''
}
int main(){unsigned checks=0,fail=0;auto check=[&](bool ok,const char*why){++checks;if(!ok){++fail;std::printf("FAIL %s\\n",why);}};
 Load();MenuState menu;Read(menu.live_d39);check(!menu.live_d39.labCorpseDragD40,"missing preference OFF");
 for(bool enabled:{true,false,true}){
  menu.live_d39.labCorpseDragD40=!enabled;check(live_settings_d39::advance(menu,live_settings_d39::Id::CorpseDragD40),"real menu reducer");
  Apply(menu.live_d39);Read(menu.live_d39);check(menu.live_d39.labCorpseDragD40==enabled,"actual read/apply");
  for(int mode=0;mode<4;++mode){menu.physical_f9_rung61=mode;wchar_t label[160]{};
   live_settings_d39::label(menu,live_settings_d39::kRows[unsigned(live_settings_d39::Group::Npc)][5],label,160);
   check(std::wcsstr(label,enabled?L"EXPERIMENTAL":L"OFF"),"label independent of F9 gameplay rung");}
  check(Save()&&ini["VrLabCorpseDrag58"]==enabled,"save exact preference");
  frik_pose_probe::SetLab58(true,!enabled,true,4);frik_pose_probe::SetLab58LivingTouch(true);Load();Read(menu.live_d39);
  check(menu.live_d39.labCorpseDragD40==enabled,"load exact preference");
  check(!lab58::livingHitImpact.load()&&!lab58::livingTouch.load()&&!lab58::propWake.load(),"corpse load cannot enable living impact/touch or prop trial");
 }
 const auto before=ini;check(!Save(false)&&ini==before,"failed save leaves previous storage unchanged");
 check(diagnostic_trace::lines==0,"setting selection has no OFF diagnostics");
 std::printf("%u corpse settings checks; %u failures; actual owners with INI IO substituted\\n",checks,fail);return fail?1:0;
}
'''
 # Older source's setters refer to the namespace without pulling this header.
 body='#include "diagnostics/capture168.h"\n'+body
 cpp=out/'corpse_settings.cpp';cpp.write_text(body);results=[]
 for variant in ('debug','release','sanitized'):
  exe=out/variant;cmd=[os.environ.get('CXX','g++'),'-std=c++20','-O2' if variant=='release' else '-O0','-I'+str(root/'src')]
  if variant=='sanitized':cmd+=['-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie','-no-pie']
  cmd += [str(cpp),'-o',str(exe)];b=subprocess.run(cmd,capture_output=True,text=True);(out/(variant+'.build.log')).write_text(b.stdout+b.stderr)
  row={'variant':variant,'command':cmd,'build_exit':b.returncode}
  if b.returncode==0:
   r=subprocess.run([str(exe)],capture_output=True,text=True,env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='halt_on_error=1'));row['exit']=r.returncode;row['output']=r.stdout+r.stderr;(out/(variant+'.run.log')).write_text(row['output'])
  row['passed']=b.returncode==0 and (row.get('exit')!=0 if a.expect_regression else row.get('exit')==0);results.append(row);print(variant,'PASS' if row['passed'] else 'FAIL')
 report={'passed':all(r['passed'] for r in results),'expected_regression':a.expect_regression,'results':results,'source_sha256':{f:hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ('src/vrik/lab58.inl','src/mod.cpp','src/subsystems/experimental/live_runtime_d40.inl','src/subsystems/masterstrike_combos/host.inl','src/subsystems/experimental/live_settings_d39.h')},'limitations':'INI IO substituted; no DLL/game/headset. Leak sanitizer disabled; ASAN+UBSAN variant labelled.'}
 (out/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n');return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
