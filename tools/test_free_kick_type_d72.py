#!/usr/bin/env python3
"""Extract and test the actual D72 owned free-request predicate; native reads mocked."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args()
root=Path(__file__).resolve().parents[1];a.out.mkdir(parents=True,exist_ok=True)
source=root/'src/vrik/native_bridges168.inl';text=source.read_text()
start=text.index('static int NativeFreeKickTypeD72(');brace=text.index('{',start);end=brace+1;depth=1
while depth:
    depth+=(text[end]=='{')-(text[end]=='}');end+=1
function=text[start:end]
host=r'''
#include "vrik/native_bridges168.h"
#include <map>
#include <cstdio>
namespace b168=kcd2vr::bridges168;
struct NativeOwner104 {void* player=nullptr;void* actor=nullptr;void* state=nullptr;std::uint64_t epoch=0;};
struct NativeInputScope117 {bool kickSpecialD72=false;std::uint64_t kickSerialD72=0;};
struct NativeInputEdgeD72 {NativeInputScope117* scope=nullptr;void* actor=nullptr;std::uint64_t source=0;bool owned=false;};
static const NativeInputEdgeD72* g_native_input_edge_d72=nullptr;
struct NativeFreeKickR24 {NativeOwner104 owner{};b168::SpecialRequest request{};b168::FreeKickLoadoutD72 loadout{};
 std::uint64_t manual=0,guard=0,source=0;bool ownsModifier=false,releaseQueued=false,dispatching=false,revoked=false;
 bool dedicatedD72=false;int inputD72=1;NativeInputScope117* scopeD72=nullptr;};
static NativeFreeKickR24 g_native_free_kick_r24;
static std::atomic<std::uintptr_t> g_free_type_contract_d72{0};
static std::atomic<std::uint64_t> g_pose_epoch6689{9},g_guard_manual_occurrence_r24{3},g_guard_lease_h14{4},g_special_context168{6};
static std::atomic<bool> g_special_allowed168{true},g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false};
static std::uint64_t generation=11,tick=1050;static int maximum=1;
static bool ownerOk=true,contextOk=true,retail=true,reference=false,trackingOk=true;
static NativeOwner104 current{};static b168::FreeKickLoadoutD72 loadout{};
struct TrackingSnapshot {bool allowed=true,head_valid=true,head_held=false;std::uint64_t tick=1050;};
static TrackingSnapshot tracking{};
static std::uintptr_t WhBase(){return 0x180000000ull;}
static std::uint64_t GetTickCount64(){return tick;}
static std::uint64_t NativePreparedSourceGenerationH26(){return generation;}
namespace retail_build112 {bool Ready(std::uintptr_t b){return retail&&b==WhBase();}}
namespace diagnostics::lab {bool NativeReference(){return reference;}}
static bool NativeFreeKickSameR24(const NativeFreeKickR24& k,NativeOwner104& out){out=current;return ownerOk&&k.owner.player==out.player&&k.owner.actor==out.actor&&k.owner.state==out.state;}
static bool NativeFreeKickContextR24(const NativeOwner104&,const b168::FreeKickLoadoutD72* expected){return contextOk&&b168::sameFreeKickLoadoutD72(*expected,loadout);}
static int NativeFreeKickMaximumR24(const NativeOwner104&){return maximum;}
static bool ReadTrackingSnapshot(TrackingSnapshot& out){out=tracking;return trackingOk;}
static std::map<std::uintptr_t,unsigned char> memory;
template<class T>void put(std::uintptr_t at,T value){auto* b=reinterpret_cast<unsigned char*>(&value);for(unsigned i=0;i<sizeof(T);++i)memory[at+i]=b[i];}
template<class T>bool Read(const void* at,T& value){auto* b=reinterpret_cast<unsigned char*>(&value);auto address=reinterpret_cast<std::uintptr_t>(at);for(unsigned i=0;i<sizeof(T);++i){auto it=memory.find(address+i);if(it==memory.end())return false;b[i]=it->second;}return true;}
'''
cases=r'''
int main(){unsigned checks=0,failures=0;auto check=[&](bool ok,const char* why){++checks;if(!ok){++failures;std::printf("FAIL %s\n",why);}};
 NativeInputScope117 scope{};NativeInputEdgeD72 edge{};constexpr std::uintptr_t factory=0x1000,actor=0x2000,state=0x3000,owner=0x4000;
 auto reset=[&](){memory.clear();generation=11;tick=1050;maximum=1;ownerOk=contextOk=retail=trackingOk=true;reference=false;
  g_pose_epoch6689=9;g_guard_manual_occurrence_r24=3;g_guard_lease_h14=4;g_special_context168=6;
  g_special_allowed168=g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=false;
  g_free_type_contract_d72=WhBase();tracking={};
  current={reinterpret_cast<void*>(0x5000),reinterpret_cast<void*>(actor),reinterpret_cast<void*>(state),9};
  loadout={};loadout.manager=1;loadout.right=2;loadout.left=3;loadout.generation=4;loadout.rightGeneration=5;loadout.weaponRequest=6;loadout.rightClass=4;loadout.leftClass=4;
  auto& k=g_native_free_kick_r24;k={};k.owner=current;k.request={77,1000,6,9,0x5000,actor,state};k.loadout=loadout;k.manual=3;k.guard=4;k.source=11;k.dedicatedD72=true;k.inputD72=2;k.dispatching=true;k.scopeD72=&scope;
  scope={true,77};edge={&scope,current.actor,11,true};g_native_input_edge_d72=&edge;
  put(factory+8,current.actor);put(actor+0x3A8,reinterpret_cast<void*>(owner));put(owner+0x50,reinterpret_cast<void*>(factory));
  put(state+0x2A8,std::uint8_t(1));put(state+0x248,2);
 };
 const auto call=[&](std::uintptr_t caller=0x687C7A){return NativeFreeKickTypeD72(reinterpret_cast<void*>(factory),0,WhBase()+caller);};
 for(auto caller:{0x687C7Au,0x12EDFF4u,0x12EDF4Du}){reset();check(call(caller)==4,"only exact retail request boundaries accept current owned R3");}
 reset();check(call(0x687C7B)==0,"nearby foreign caller retains original type");
 reset();g_native_free_kick_r24.dedicatedD72=false;check(call()==0,"legacy free-unarmed input retains native original type");
 reset();g_free_type_contract_d72=0;check(call()==0,"unarmed hook contract cannot grant dedicated capability");
 reset();put(factory+8,reinterpret_cast<void*>(0x6666));check(call()==0,"foreign actor retains its original selector");
 reset();put(owner+0x50,reinterpret_cast<void*>(0x6666));check(call()==0,"foreign factory cannot borrow actor request");
 reset();edge.owned=false;check(call()==0,"nested foreign native edge masks outer R3 scope");edge.owned=true;check(call()==4,"restored outer exact source is still valid absent foreign generation change");
 reset();edge.owned=false;generation++;check(call()==0,"foreign edge with replaced source stays original");edge.owned=true;check(call()==0x7fffffff,"old outer request cannot resume after source replacement");
 reset();scope.kickSerialD72++;check(call()==0,"another R3 occurrence cannot borrow the captured scope");
 reset();edge.scope=nullptr;check(call()==0,"missing scope cannot borrow dispatch");
 for(int failure=0;failure<26;++failure){reset();
  switch(failure){
   case 0:g_native_free_kick_r24.revoked=true;break;case 1:g_native_free_kick_r24.releaseQueued=true;break;
   case 2:retail=false;break;case 3:ownerOk=false;break;case 4:current.epoch++;break;case 5:g_pose_epoch6689++;break;
   case 6:contextOk=false;break;case 7:loadout.right++;break;case 8:loadout.left++;break;case 9:loadout.rightGeneration++;break;
   case 10:loadout.generation++;break;case 11:loadout.weaponRequest++;break;case 12:maximum=0;break;
   case 13:g_guard_manual_occurrence_r24++;break;case 14:g_guard_lease_h14++;break;case 15:g_special_allowed168=false;break;
   case 16:g_gameplay_allowed=false;break;case 17:g_special_context168++;break;case 18:reference=true;break;
   case 19:g_ctx_menu=true;break;case 20:g_ctx_dialogue=true;break;case 21:g_ctx_cutscene=true;break;case 22:g_ctx_scripted=true;break;
   case 23:tick=999;break;case 24:tick=2501;tracking.tick=tick;break;case 25:tracking.head_held=true;break;}
  check(call()==0x7fffffff,"invalid owned request asks for no row; never falls back to an ordinary weapon attack");
 }
 for(int failure=0;failure<5;++failure){reset();switch(failure){case 0:tracking.allowed=false;break;case 1:tracking.head_valid=false;break;case 2:tracking.tick=0;break;case 3:tracking.tick=1110;break;case 4:tracking.tick=989;break;}
  check(call()==0x7fffffff,"tracking loss/age revokes only the owned request");}
 reset();g_native_input_edge_d72=nullptr;g_native_free_kick_r24.dispatching=false;g_native_free_kick_r24.scopeD72=nullptr;
 check(call()==4,"original deferred native request retains same exact source");
 put(state+0x248,1);check(call()==0,"a different native input family cannot inherit deferred kick");put(state+0x248,2);
 generation++;check(call()==0,"a later native source retains original behavior");generation--;
 put(state+0x2A8,std::uint8_t(0));check(call()==0,"completed request is not replayed by later selector calls");
 std::printf("D72 actual owned free-request predicate: %u/%u; native reads mocked; game NOT_RUN\n",checks-failures,checks);return failures?1:0;
}
'''
unit=a.out/'free_kick_type_actual_d72.cpp';unit.write_text(host+function+cases);exe=a.out/'free_kick_type_actual_d72'
cmd=['g++','-std=c++20','-O1','-fsanitize=address,undefined','-fno-omit-frame-pointer','-I'+str(root/'src'),str(unit),'-o',str(exe)]
build=subprocess.run(cmd,capture_output=True,text=True);(a.out/'build.log').write_text(build.stdout+build.stderr)
if build.returncode:print(build.stdout+build.stderr);raise SystemExit(build.returncode)
env=dict(os.environ);env['ASAN_OPTIONS']='detect_leaks=0';run=subprocess.run([str(exe)],capture_output=True,text=True,env=env)
(a.out/'run.log').write_text(run.stdout+run.stderr)
(a.out/'result.json').write_text(json.dumps({'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'extracted_sha256':hashlib.sha256(function.encode()).hexdigest(),'build':cmd,'test_exit':run.returncode,'limits':'Actual production predicate; identity/read/native-service leaves mocked. Native machine-code selection is separately covered by native_kick_d72_test.cpp.'},indent=2)+'\n')
print(run.stdout+run.stderr);raise SystemExit(run.returncode)
