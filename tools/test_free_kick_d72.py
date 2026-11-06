#!/usr/bin/env python3
"""Exercise the actual D72 free-kick admission functions with native read leaves.

The functions are extracted unchanged from the production inl. This checks
loadout/frame/occurrence admission, not retail animation or damage execution.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import re
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('--out', type=Path, required=True)
parser.add_argument('--cxx', default='g++')
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
out = args.out.resolve()
out.mkdir(parents=True, exist_ok=True)
source = root / 'src/vrik/native_bridges168.inl'
text = source.read_text()


def function(name):
    match = re.search(r'static bool ' + re.escape(name) + r'\([^;{]*\) noexcept \{', text)
    assert match, name
    begin = match.start()
    brace = text.index('{', begin)
    depth = 1
    end = brace + 1
    while depth:
        if text[end] == '{':
            depth += 1
        elif text[end] == '}':
            depth -= 1
        end += 1
    return text[begin:end]


functions = '\n'.join(function(name) for name in
                      ('NativeFreeKickLoadoutD72', 'NativeFreeKickContextR24'))
host = r'''
#include "vrik/native_bridges168.h"
#include "vrik/kick_route_d85.h"
#include <array>
#include <cstring>
#include <cstdio>
namespace b168=kcd2vr::bridges168;
namespace kcd2vr::combat_rules178r10 {inline std::atomic<int> kickTransport{3};}
struct NativeOwner104 {void* player=nullptr;void* actor=nullptr;void* state=nullptr;std::uint64_t epoch=0;};
struct FistOwner101 {void* player=nullptr;void* manager=nullptr;std::uint64_t epoch=0;};
struct PhysFrame6698 {
 void* player=nullptr;void* manager=nullptr;std::uint64_t request=0;
 struct {std::uint64_t bodyEpoch=0,tick=0;} input;
 struct Profile {std::uint64_t key=0,generation=0;int classId=-1;} profiles[2];
};
struct Mailbox {PhysFrame6698 frame{};bool available=true;
 bool read(PhysFrame6698& out){if(!available)return false;out=frame;return true;}
} g_phys_frame6698;
std::atomic<std::uint64_t> g_weapon_request_epoch6694{7};
std::array<unsigned char,0xD20> actor{},state{};
std::uint64_t keys[2]{0,0},now=1000;
FistOwner101 hands{};bool ownerAvailable=true,readAvailable=true,keysAvailable=true;
bool FistReadOwner101(FistOwner101& out){if(!ownerAvailable)return false;out=hands;return true;}
bool NativeHandKey6697(void* manager,int side,std::uint64_t& key){
 if(!keysAvailable||manager!=hands.manager)return false;key=keys[side];return true;}
bool PhysEmpty6698(std::uint64_t key){return key==0||key==~std::uint64_t(0);}
std::uint64_t GetTickCount64(){return now;}
// D85: current guard-type reader leaf (the reader itself is tested by D84/D85).
bool guardKnownD85=false;int guardValueD85=-1;
bool NativeFreeKickGuardTypeD84(const NativeOwner104&,int& value){value=guardValueD85;return guardKnownD85;}
template<class T>bool Read(const void* address,T& value){
 if(!readAvailable)return false;std::memcpy(&value,address,sizeof(value));return true;}
'''
cases = r'''
int main(){
 unsigned total=0,failed=0;
 auto check=[&](bool ok,const char* label){++total;if(!ok){++failed;std::printf("FAIL %s\n",label);}};
 NativeOwner104 owner{reinterpret_cast<void*>(0x1000),actor.data(),state.data(),2};
 auto reset=[&](){
  actor.fill(0);state.fill(0);keys[0]=keys[1]=0;now=1000;
  ownerAvailable=readAvailable=keysAvailable=true;guardKnownD85=false;guardValueD85=-1;
  hands={owner.player,reinterpret_cast<void*>(0x2000),owner.epoch};
  g_weapon_request_epoch6694=7;kcd2vr::combat_rules178r10::kickTransport=3;
  g_phys_frame6698={};g_phys_frame6698.available=true;
  auto& f=g_phys_frame6698.frame;f.player=owner.player;f.manager=hands.manager;
  f.request=7;f.input={2,1000};f.profiles[1]={88,3,11};
 };
 auto torch=[&](){reset();keys[1]=88;};
 reset();g_phys_frame6698.available=false;
 check(NativeFreeKickContextR24(owner),"existing both-empty path needs no held-weapon frame");
 reset();keys[0]=keys[1]=~std::uint64_t(0);
 check(NativeFreeKickContextR24(owner),"native empty-key sentinel remains supported");
 torch();check(NativeFreeKickContextR24(owner),"actual empty-right/left-torch native free row is admitted");
 b168::FreeKickLoadoutD72 binding{};
 check(NativeFreeKickLoadoutD72(owner,binding)&&binding.left==88&&binding.generation==3,
       "torch occurrence captures actual item generation");
 check(NativeFreeKickContextR24(owner,&binding),"same current item continues the admitted occurrence");
 ++g_phys_frame6698.frame.profiles[1].generation;
 check(!NativeFreeKickContextR24(owner,&binding),"same key with a replaced attachment generation cannot inherit occurrence");
 torch();NativeFreeKickLoadoutD72(owner,binding);keys[1]=89;g_phys_frame6698.frame.profiles[1].key=89;
 check(!NativeFreeKickContextR24(owner,&binding),"new torch identity cannot inherit an old modifier transaction");
 torch();NativeFreeKickLoadoutD72(owner,binding);g_weapon_request_epoch6694=8;g_phys_frame6698.frame.request=8;
 check(!NativeFreeKickContextR24(owner,&binding),"new weapon request epoch cannot inherit occurrence");
 torch();NativeFreeKickLoadoutD72(owner,binding);keys[1]=0;
 check(!NativeFreeKickContextR24(owner,&binding),"stowed torch invalidates old authorization while cleanup stays separate");
 for(int cls:{8,17,0,9,10,14,15,-1}){
  torch();g_phys_frame6698.frame.profiles[1].classId=cls;
  check(NativeFreeKickContextR24(owner)==(cls>=0),"known authored offhand passes identity gate; native row still decides capability");
  if(cls>=0){NativeFreeKickLoadoutD72(owner,binding);check(binding.leftClass==cls&&b168::dedicatedFreeKickD72(binding),"new gear uses dedicated SPECIAL family, never torch semantics");}
 }
 torch();keys[0]=77;check(!NativeFreeKickContextR24(owner),"unobserved right equipment is refused");
 for(int cls:{1,2,3,4,5,6,7,13,16}){
  reset();keys[0]=77;g_phys_frame6698.frame.profiles[0]={77,5,cls};
  check(NativeFreeKickContextR24(owner),"fresh armed equipment reaches dedicated native free query");
  NativeFreeKickLoadoutD72(owner,binding);check(binding.rightClass==cls&&binding.rightGeneration==5,"right equipment identity captured");
  ++g_phys_frame6698.frame.profiles[0].generation;
  check(!NativeFreeKickContextR24(owner,&binding),"new right attachment generation cannot inherit old kick");
 }
 torch();g_phys_frame6698.frame.profiles[1].classId=18;check(!NativeFreeKickContextR24(owner),"unknown equipment class stays refused");
 torch();g_phys_frame6698.frame.profiles[1].generation=0;
 check(!NativeFreeKickContextR24(owner),"missing torch generation is not current ownership");
 torch();g_phys_frame6698.frame.profiles[1].key=89;
 check(!NativeFreeKickContextR24(owner),"frame key must match actual native held key");
 torch();g_phys_frame6698.frame.manager=nullptr;
 check(!NativeFreeKickContextR24(owner),"foreign frame manager refused");
 torch();g_phys_frame6698.frame.player=nullptr;
 check(!NativeFreeKickContextR24(owner),"foreign frame player refused");
 torch();g_phys_frame6698.frame.input.bodyEpoch=3;
 check(!NativeFreeKickContextR24(owner),"foreign body epoch refused");
 torch();g_phys_frame6698.frame.request=6;
 check(!NativeFreeKickContextR24(owner),"outdated frame request refused");
 torch();now=1081;check(!NativeFreeKickContextR24(owner),"stale torch frame refused");
 torch();now=999;check(!NativeFreeKickContextR24(owner),"future torch frame refused");
 torch();g_phys_frame6698.available=false;
 check(!NativeFreeKickContextR24(owner),"unknown torch source refused without granting unarmed capability");
 torch();hands.player=nullptr;check(!NativeFreeKickContextR24(owner),"foreign actual hand owner refused");
 torch();hands.epoch=3;check(!NativeFreeKickContextR24(owner),"foreign actual hand epoch refused");
 torch();ownerAvailable=false;check(!NativeFreeKickContextR24(owner),"unreadable native owner refused");
 torch();keysAvailable=false;check(!NativeFreeKickContextR24(owner),"unreadable native held key refused");
 for(int route:{1,2}){torch();kcd2vr::combat_rules178r10::kickTransport=route;
  check(!NativeFreeKickContextR24(owner),"legacy transports retain their existing paths");}
 torch();void* opponent=reinterpret_cast<void*>(0x3000);std::memcpy(state.data()+0xCF8,&opponent,sizeof(opponent));
 check(!NativeFreeKickContextR24(owner),"ordinary opponent context is not claimed by free-stance owner");
 torch();std::memcpy(actor.data()+0x450,&opponent,sizeof(opponent));
 check(!NativeFreeKickContextR24(owner),"ordinary combat context with unknown guard is not claimed by free-stance owner");
 guardKnownD85=true;guardValueD85=0;
 check(!NativeFreeKickContextR24(owner),"D85: +450 object with combat guard 0 keeps the combat route");
 guardValueD85=23;
 check(NativeFreeKickContextR24(owner),"D85: +450 object with current FreeGuard 23 and no opponent keeps the free route");
 std::memcpy(state.data()+0xCF8,&opponent,sizeof(opponent));
 check(!NativeFreeKickContextR24(owner),"D85: FreeGuard with an opponent is still the combat route");
 torch();readAvailable=false;check(!NativeFreeKickContextR24(owner),"unknown context cannot grant free stance");
 std::printf("D72 actual free-kick admission: %u/%u; native reads mocked; game NOT_RUN\n",total-failed,total);
 return failed?1:0;
}
'''
unit = out / 'free_kick_d72_actual.cpp'
unit.write_text(host + functions + cases)
exe = out / 'free_kick_d72_actual'
cmd = [args.cxx, '-std=c++20', '-O1', '-fsanitize=address,undefined',
       '-fno-omit-frame-pointer', '-I'+str(root/'src'), str(unit), '-o', str(exe)]
build = subprocess.run(cmd, text=True, capture_output=True)
(out/'build.log').write_text(build.stdout+build.stderr)
if build.returncode:
    print(build.stdout+build.stderr)
    raise SystemExit(build.returncode)
env = dict(os.environ)
env['ASAN_OPTIONS'] = 'detect_leaks=0'
run = subprocess.run([str(exe)], text=True, capture_output=True, env=env)
(out/'run.log').write_text(run.stdout+run.stderr)
record = {'source':str(source), 'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),
          'extracted_functions_sha256':hashlib.sha256(functions.encode()).hexdigest(),
          'build':cmd,'build_exit':build.returncode,'test_exit':run.returncode,
          'limits':'Actual production admission functions, native memory/read leaves mocked; native selector/animation/damage not executed.'}
(out/'result.json').write_text(json.dumps(record,indent=2)+'\n')
print(run.stdout+run.stderr)
raise SystemExit(run.returncode)
