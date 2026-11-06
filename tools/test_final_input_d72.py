#!/usr/bin/env python3
"""Actual production D72 input-edge/recipe-leaf extraction test. Native reads mocked."""
from pathlib import Path
import argparse,hashlib,json,os,subprocess
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--cxx',default='g++');a=p.parse_args()
root=Path(__file__).resolve().parents[1];a.out.mkdir(parents=True,exist_ok=True)
source=root/'src/vrik/native_input117.inl';text=source.read_text()
def extract(name):
 start=text.index(name+'(');start=text.rfind('static ',0,start);at=text.index('{',start);depth=1;end=at+1
 while depth:
  depth+=(text[end]=='{')-(text[end]=='}');end+=1
 return text[start:end]
functions='\n'.join(extract(n) for n in ['NativeInputOwnedLeaf51','NativeInputClass117','NativeFinalStepD72'])
host=r'''
#include "subsystems/masterstrike_combos/final_combo_contact_d72.h"
#include <map>
#include <cstdio>
#include <vector>
namespace ph {enum class CommandKind {None,Slash,Thrust,Pommel};}
namespace fd=kcd2vr::final_combo_d72;
struct NativeOwner104 {void* player=nullptr;void* actor=nullptr;void* state=nullptr;std::uint64_t epoch=0;};
struct NativeInputScope117 {NativeOwner104 owner{};ph::CommandKind stroke=ph::CommandKind::None;bool prepare=false,visited=false,fistPunchR19=false;int passed=-1;fd::Permit finalD72{};bool kickSpecialD72=false;std::uint64_t kickSerialD72=0;};
static NativeInputScope117* g_input_scope117=nullptr;
static std::atomic<std::uintptr_t> g_input_contract117{0};
static std::atomic<bool> g_thrust_as_swing55{true};
static std::uint64_t tick=1050;static bool retail=true,resolve=true;
static NativeOwner104 current{};
struct FistOwner101 {bool empty=false;void* player=nullptr;std::uint64_t epoch=0,request=0;};
static struct {std::uint64_t request=1;} g_fist_native101;
static bool FistReadOwner101(FistOwner101&){return false;}
static std::uintptr_t WhBase(){return 0x180000000ull;}
static std::uint64_t GetTickCount64(){return tick;}
namespace retail_build112 {static bool Ready(std::uintptr_t b){return retail&&b==WhBase();}}
static bool NativeResolve104(void* p,NativeOwner104& owner){owner=current;return resolve&&p==current.player;}
static std::map<std::uintptr_t,unsigned char> mem;
template<class T>void put(std::uintptr_t a,T v){auto* b=reinterpret_cast<unsigned char*>(&v);for(unsigned i=0;i<sizeof(T);++i)mem[a+i]=b[i];}
template<class T>bool Read(const void* a,T& value){auto* b=reinterpret_cast<unsigned char*>(&value);auto at=reinterpret_cast<std::uintptr_t>(a);for(unsigned i=0;i<sizeof(T);++i){auto it=mem.find(at+i);if(it==mem.end())return false;b[i]=it->second;}return true;}
'''
cases=r'''
int main(){unsigned checks=0,failed=0;auto check=[&](bool ok,const char* label){++checks;if(!ok){++failed;std::printf("FAIL %s\n",label);}};
 const auto base=WhBase();void* self=reinterpret_cast<void*>(0x1000);
 NativeInputScope117 scope{};
 auto reset=[&](bool prepare=true){mem.clear();scope={};tick=1050;retail=resolve=true;g_thrust_as_swing55=true;g_input_contract117=base;
  current={reinterpret_cast<void*>(0x2000),reinterpret_cast<void*>(0x3000),reinterpret_cast<void*>(0x4000),9};scope.owner=current;scope.prepare=prepare;scope.stroke=ph::CommandKind::Slash;g_input_scope117=&scope;
  put(std::uintptr_t(0x1018),current.actor);put(base+0x5321014,2);put(base+0x5321010,1);put(base+0x5321018,3);
  auto& p=scope.finalD72;p.serial=fd::mint();p.gesture=6;p.key=7;p.generation=8;p.body=9;p.request=10;p.started=1000;p.sourceSample=7;
  p.player=0x2000;p.entity=12;p.actor=0x3000;p.state=0x4000;p.manager=15;p.target=16;p.targetEntity=17;p.targetId=23;
  p.recipe=18;p.nativeStep=19;p.cursor=2;p.required=3;p.input=1;p.zone=1;p.contact.tick=1050;p.contact.target=23;p.contact.actual=true;fd::publish(p);
 };
 for(bool prep:{true,false})for(auto stroke:{ph::CommandKind::Slash,ph::CommandKind::Thrust,ph::CommandKind::Pommel}){
  reset(prep);scope.stroke=stroke;const auto caller=base+(prep?0x27C08E6:0x27C0916);
  check(NativeInputClass117(self,2,caller,prep)==1&&scope.visited&&scope.passed==1,"same native LIGHT at both exact edges independent of physical label");
  check(NativeInputOwnedLeaf51(self,caller,prep),"owned final Pommel uses normal prepared native input transport");
  reset(prep);scope.stroke=stroke;scope.finalD72.input=2;fd::publish(scope.finalD72);g_thrust_as_swing55=false;
  check(NativeInputClass117(self,2,caller,prep)==2&&scope.visited,"authored HEAVY wins even for a physical thrust with light comparison selected");
 }
 reset();scope.finalD72={};scope.stroke=ph::CommandKind::Thrust;g_thrust_as_swing55=false;
 check(NativeInputClass117(self,2,base+0x27C08E6,true)==1&&scope.visited,"legacy option OFF path still routes a real thrust to LIGHT");
 reset();scope.finalD72={};scope.stroke=ph::CommandKind::Thrust;
 check(NativeInputClass117(self,2,base+0x27C08E6,true)==2&&scope.visited,"legacy thrust-as-swing keeps HEAVY");
 for(int failure=0;failure<11;++failure){reset();auto caller=base+0x27C08E6;bool prep=true;int input=2;
  switch(failure){case 0:caller++;break;case 1:scope.owner.actor=reinterpret_cast<void*>(0x6666);break;case 2:current.state=reinterpret_cast<void*>(0x6666);break;
   case 3:current.epoch++;break;case 4:tick=1101;break;case 5:fd::revoke();break;case 6:scope.finalD72.actor++;break;
   case 7:retail=false;break;case 8:resolve=false;break;case 9:put(base+0x5321010,2);break;case 10:prep=false;break;}
  (void)NativeInputClass117(self,input,caller,prep);check(!scope.visited,"wrong caller/owner/epoch/age/permit/global/mode cannot borrow final input");
 }
 reset();scope.stroke=ph::CommandKind::Pommel;
 (void)NativeInputClass117(self,3,base+0x27C0998,true);check(!scope.visited,"a contextual special press cannot impersonate the ordinary final transport");
 for(bool prep:{true,false}){
  reset(prep);scope.finalD72={};scope.kickSpecialD72=true;scope.kickSerialD72=7;
  check(NativeInputClass117(self,2,base+(prep?0x27C08E6:0x27C0916),prep)==3&&scope.visited,
   "dedicated R3 transports live SPECIAL through both original primary-controller edges");
  check(NativeInputOwnedLeaf51(self,base+(prep?0x27C08E6:0x27C0916),prep),"dedicated R3 still proves original source edge");
  reset(prep);scope.finalD72={};scope.kickSpecialD72=true;
  check(NativeInputClass117(self,2,base+(prep?0x27C08E6:0x27C0916),prep)==2,"missing R3 occurrence cannot borrow dedicated input family");
 }
 // Actual original selector field +0x10 is used, not attack type +8 or zone.
 kcd2vr::diagnostics::combo::Candidate c{};c.recipe=0x10000;
 auto native=[&](int family,int attack,int cursor){put(c.recipe+0x40,std::uintptr_t(0x11000));put(c.recipe+0x48,std::uintptr_t(0x11020));put(std::uintptr_t(0x11008),attack);put(std::uintptr_t(0x11010),family);put(std::uintptr_t(0x11018),cursor);};
 std::uintptr_t step=0;int input=-1;
 for(int attack:{0,1,4,17}){reset();native(1,attack,2);check(NativeFinalStepD72(c,2,2,1,step,input)&&input==1&&step==0x11000,"native input family independent of authored attack type");}
 for(int family:{-1,1,2}){reset();native(family,1,2);check(NativeFinalStepD72(c,2,2,1,step,input)&&input==(family==-1?2:family),"only native ANY/HEAVY/LIGHT transport supported");}
 for(int family:{-2,3,99}){reset();native(family,1,2);check(!NativeFinalStepD72(c,2,2,1,step,input),"unknown native input family is not fabricated");}
 reset();native(1,1,1);check(!NativeFinalStepD72(c,2,2,1,step,input),"prefix step cannot supply final native transport");
 reset();native(1,-1,2);check(!NativeFinalStepD72(c,2,2,1,step,input),"native wildcard attack row not promoted to final action");
 reset();native(1,1,2);put(c.recipe+0x48,std::uintptr_t(0x11021));check(!NativeFinalStepD72(c,2,2,1,step,input),"malformed native step span refused");
 std::printf("D72 actual input edges + native recipe leaf: %u/%u; native read leaves mocked; game NOT_RUN\n",checks-failed,checks);return failed?1:0;
}
'''
unit=a.out/'final_input_actual_d72.cpp';unit.write_text(host+functions+cases);exe=a.out/'final_input_actual_d72'
cmd=[a.cxx,'-std=c++20','-O1','-fsanitize=address,undefined','-fno-omit-frame-pointer','-I'+str(root/'src'),str(unit),'-o',str(exe)]
build=subprocess.run(cmd,capture_output=True,text=True);(a.out/'build.log').write_text(build.stdout+build.stderr)
if build.returncode:print(build.stdout+build.stderr);raise SystemExit(build.returncode)
env=dict(os.environ);env['ASAN_OPTIONS']='detect_leaks=0';run=subprocess.run([str(exe)],capture_output=True,text=True,env=env)
(a.out/'run.log').write_text(run.stdout+run.stderr)
(a.out/'result.json').write_text(json.dumps({'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'extracted_sha256':hashlib.sha256(functions.encode()).hexdigest(),'build':cmd,'test_exit':run.returncode,'limits':'Actual production function bodies; native leaves mocked. No retail ABI or gameplay execution.'},indent=2)+'\n')
print(run.stdout+run.stderr);raise SystemExit(run.returncode)
