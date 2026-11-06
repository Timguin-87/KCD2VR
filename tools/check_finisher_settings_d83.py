#!/usr/bin/env python3
"""Execute the actual two finisher menu rows and their literal production
read/apply/save/load statements. INI storage is an explicit in-memory leaf.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
from check_finisher_upper_d83 import block

p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True)
a=p.parse_args();root=Path(__file__).resolve().parents[1];out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
runtime=root/'src/subsystems/experimental/live_runtime_d40.inl'
text=runtime.read_text();parts={}
for mode in ('READ','APPLY','SAVE','LOAD'):
    region=text.split(f'defined(KCD2VR_D40_LIVE_{mode})',1)[1].split('#elif',1)[0].split('#endif',1)[0]
    parts[mode]='\n'.join(line for line in region.splitlines() if 'finisher_upper_d83::' in line)
    assert len(parts[mode].splitlines())==2,(mode,parts[mode])
menu=block((root/'tests/live_settings_d39_test.cpp').read_text(),'struct MenuState ')
fixture='''#include <map>
#include <string>
#include <stdexcept>
#include <iostream>
#include "vrik/finisher_upper_d83.h"
#include "subsystems/experimental/live_settings_d39.h"
'''+menu+'''
namespace kcd2vr {
std::map<std::string,bool> ini;
bool ReadIniBoolExplicit(const char*,const char*,const char* key,bool fallback){auto it=ini.find(key);return it==ini.end()?fallback:it->second;}
void Read(live_settings_d39::Settings& d40){
'''+parts['READ']+'''
}
void Apply(const live_settings_d39::Settings& d40){
'''+parts['APPLY']+'''
}
bool Save(bool good=true){bool ok=true;auto wb=[&](const char* key,bool value){if(good)ini[key]=value;return good;};
'''+parts['SAVE']+'''
return ok;}
void Load(){const char* explicit_ini="fixture.ini";
'''+parts['LOAD']+'''
}}
int main(){using namespace kcd2vr;namespace live=live_settings_d39;unsigned checks=0;
auto require=[&](bool v,const char* why){++checks;if(!v)throw std::runtime_error(why);};
Load();MenuState state;Read(state.live_d39);
require(state.live_d39.finisherUpperPlayerD83&&state.live_d39.finisherUpperNpcD83,"missing key ON defaults");
unsigned rows=0;
for(const auto& row:live::rows(live::kCombatPage2D70)){
 if(row.id!=live::Id::FinisherUpperPlayerD83&&row.id!=live::Id::FinisherUpperNpcD83)continue;
 ++rows;wchar_t label[160]{};live::label(state,row,label,160);
 require(std::wcsstr(label,L"NATIVE + HEAD TURNS")!=nullptr,"menu overclaims upper body");
 require(live::advance(state,row.id),"actual menu reducer refused finisher toggle");
 Apply(state.live_d39);Read(state.live_d39);live::label(state,row,label,160);
 require(std::wcsstr(label,L"NATIVE + HEAD TURNS")==nullptr,"OFF label unchanged");
 if(row.id==live::Id::FinisherUpperPlayerD83)require(!finisher_upper_d83::player&&finisher_upper_d83::npc,"player toggle changed NPC owner");
}
require(rows==2,"two independent menu rows");
require(!finisher_upper_d83::player&&!finisher_upper_d83::npc,"OFF did not reach actual owners");
require(Save()&&ini.size()==2&&!ini["VrFinisherUpperBodyPlayerD83"]&&!ini["VrFinisherUpperBodyNpcD83"],"canonical OFF values not saved");
finisher_upper_d83::player=true;finisher_upper_d83::npc=true;Load();Read(state.live_d39);
require(!state.live_d39.finisherUpperPlayerD83&&!state.live_d39.finisherUpperNpcD83,"ON defaults replaced saved OFF");
require(!Save(false),"failed save reported success");
std::cout<<"PASS "<<checks<<" actual finisher menu/runtime checks; INI IO is a host leaf\\n";
}
'''
(out/'finisher_settings.cpp').write_text(fixture)
compiler=shutil.which('clang++') or shutil.which('g++')
cmd=[compiler,'-std=c++20','-O2','-I',str(root/'src'),str(out/'finisher_settings.cpp'),'-o',str(out/'finisher_settings')]
b=subprocess.run(cmd,capture_output=True,text=True);(out/'compile.txt').write_text(b.stdout+b.stderr)
r=subprocess.run([str(out/'finisher_settings')],capture_output=True,text=True) if b.returncode==0 else None
if r:(out/'run.txt').write_text(r.stdout+r.stderr)
files=[runtime,root/'src/subsystems/experimental/live_settings_d39.h',root/'src/vrik/finisher_upper_d83.h',out/'finisher_settings.cpp']
receipt={'scope':'Actual finisher menu reducer/header and eight literal production runtime statements; only INI IO is substituted. No whole Mod or Windows build claim.','command':cmd,'compile_exit':b.returncode,'exit':r.returncode if r else None,'output':r.stdout if r else None,'sha256':{str(f.relative_to(root)) if f.is_relative_to(root) else f.name:hashlib.sha256(f.read_bytes()).hexdigest() for f in files}}
(out/'RESULT.json').write_text(json.dumps(receipt,indent=2)+'\n')
print(json.dumps({'passed':r is not None and r.returncode==0,'output':receipt['output'],'result':str(out/'RESULT.json')}))
raise SystemExit(0 if r is not None and r.returncode==0 else 1)
