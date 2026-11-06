#!/usr/bin/env python3
"""Run unchanged production free-kick dispatch, wait and cleanup against explicit leaves.

This is an input-ownership regression test. Exact retail row selection is tested
separately; native OnAction, animation and game outcomes are not emulated here.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--out', type=Path, required=True)
p.add_argument('--source', type=Path)
p.add_argument('--sanitize', action='store_true')
a = p.parse_args()
root = Path(__file__).resolve().parents[1]
source_root = (a.source or root).resolve()
out = a.out.resolve()
out.mkdir(parents=True, exist_ok=True)
source = source_root / 'src/vrik/native_bridges168.inl'
text = source.read_text()


def block(pattern):
    m = re.search(pattern, text)
    assert m, pattern
    begin = m.start()
    end = text.index('{', begin) + 1
    depth = 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    return text[begin:end]


has_wait = 'static void NativeFreeKickContinueD84(b168::Event& e)' in text
names = ['NativeFreeKickSameR24', 'NativeFreeKickTakeoverR24',
         'NativeFreeKickGuardRetainedR24', 'NativeFreeKickReleaseR24',
         'NativeFreeKickTickR24', 'NativeFreeKickDispatchR24']
if has_wait:
    names = ['NativeFreeKickGuardTypeD84', 'NativeFreeKickOwnerStatusD84'] + names + ['NativeFreeKickContinueD84']
definitions = '\n'.join(block(r'static (?:bool|void|int) ' + name + r'\([^;{]*\) noexcept \{') for name in names)
owner = block(r'struct NativeFreeKickR24 \{') + ';\nstatic NativeFreeKickR24 g_native_free_kick_r24;\n'

host = r'''
#include "vrik/native_bridges168.h"
#include "vrik/native_admission147.h"
#include <map>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
namespace b168=kcd2vr::bridges168;
namespace nc104=kcd2vr::native_combat104;
namespace native_admission147=kcd2vr::native_admission147;
struct NativeOwner104 {void* player=nullptr;void* actor=nullptr;void* state=nullptr;std::uint64_t epoch=0;};
namespace ph {enum class CommandKind {Slash};}
struct NativeInputScope117 {
 NativeOwner104 owner{};ph::CommandKind stroke{};bool prepare=false,kickSpecialD72=false,visited=false;
 std::uint64_t kickSerialD72=0,prepareSourceR18=0,releaseSourceR24=0;
 bool prepareStableR18=false,releaseStableR24=false;
};
static NativeInputScope117* g_input_scope117=nullptr;
static std::uint64_t now=1000,sourceGeneration=7;
static NativeOwner104 currentOwner{};static b168::FreeKickLoadoutD72 currentLoadout{};
static nc104::Observed observed{};
static bool ownerAvailable=true,contextAvailable=true,observeAvailable=true,trackingAvailable=true;
static bool retail=true,reference=false,inputHook=true,armReady=true,zoneOkay=true,immediateGuard=false;
static bool releaseAck=true,pressOkay=true,preparedOkay=true,releaseOkay=true;
static int modifier=0,modifierContext=0;
static unsigned blockPresses=0,blockReleases=0,attackPresses=0,attackReleases=0;
static std::vector<b168::Event> events;
static std::map<std::uintptr_t,unsigned char> memory;
static constexpr std::uintptr_t imageBase=0x180000000ull,propertyTable=0x190000000ull,getter=0x190001000ull;
template<class T>static void put(std::uintptr_t at,const T& value){const auto* p=reinterpret_cast<const unsigned char*>(&value);for(unsigned i=0;i<sizeof value;++i)memory[at+i]=p[i];}
template<class T>static bool Read(const void* raw,T& value){auto at=reinterpret_cast<std::uintptr_t>(raw);auto* p=reinterpret_cast<unsigned char*>(&value);for(unsigned i=0;i<sizeof value;++i){auto it=memory.find(at+i);if(it==memory.end())return false;p[i]=it->second;}return true;}
static std::uintptr_t WhBase(){return imageBase;}
static std::uint64_t GetTickCount64(){return now;}
static std::atomic<std::uint64_t> g_pose_epoch6689{9},g_guard_manual_occurrence_r24{3},g_guard_lease_h14{4},g_special_context168{6};
static std::atomic<bool> g_special_allowed168{true},g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_guard_manual112{false};
static std::atomic<void*> g_guard_manual_owner112{nullptr};
static struct {bool attackPrepared=false;} g_phys_native6698;
namespace kcd2vr::animation_filter {inline std::atomic<bool> nativeBodyOwned112{false};}
namespace animation_filter=kcd2vr::animation_filter;
namespace diagnostics::lab {static bool NativeReference(){return reference;}}
namespace retail_build112 {static bool Ready(std::uintptr_t b){return retail&&b==WhBase();}}
namespace diagnostics::intent168 {
enum class Source{R3};struct Intent{std::uint64_t serial;std::uintptr_t actor;Source source;};
struct Scope{explicit Scope(Intent){}};
}
struct TrackingSnapshot {bool allowed=true,head_valid=true,head_held=false;std::uint64_t tick=1000;};
static TrackingSnapshot tracking{};
static bool ReadTrackingSnapshot(TrackingSnapshot& out){out=tracking;return trackingAvailable;}
static bool NativeResolve104(void* player,NativeOwner104& out){out=currentOwner;return ownerAvailable&&out.player==player;}
static bool NativeObserve104(const NativeOwner104&,nc104::Observed& out){out=observed;return observeAvailable;}
static bool NativeFreeKickLoadoutD72(const NativeOwner104&,b168::FreeKickLoadoutD72& out){out=currentLoadout;return contextAvailable;}
static bool NativeFreeKickContextR24(const NativeOwner104&,const b168::FreeKickLoadoutD72* expected=nullptr){return contextAvailable&&(!expected||b168::sameFreeKickLoadoutD72(*expected,currentLoadout));}
static int NativeFreeKickMaximumR24(const NativeOwner104&){return modifier;}
static int NativeGuardReleaseAckH14(const NativeOwner104&){return modifierContext;}
static bool NativeInputArm117(){return armReady;}
static bool ArmFreeKickD72(){return armReady;}
static bool OwnCombatInputHook101(std::uintptr_t){return inputHook;}
static bool NativeZone104(void*,int zone){observed.zone=zone;return zoneOkay;}
static std::uint64_t NativePreparedSourceGenerationH26(){return sourceGeneration;}
static void BridgeEvent168(b168::Event e){events.push_back(e);}
static bool PhysCallAction6698(void*,int,int,bool,bool* =nullptr);
static void NativeFreeKickTickR24() noexcept;
static void NativeFreeKickContinueD84(b168::Event&) noexcept;
static void NativeFreeKickDispatchR24(const b168::SpecialRequest&,const NativeOwner104&,const nc104::Observed&,b168::Event&) noexcept;
'''

leaves = r'''
static bool PhysCallAction6698(void*,int action,int mode,bool,bool* entered){
 if(entered)*entered=true;
 if(action==3){
  if(mode==1){++blockPresses;if(!pressOkay)return false;modifier=modifierContext=1;
   if(immediateGuard)put(reinterpret_cast<std::uintptr_t>(currentOwner.state)+0x68,23);
  }else{++blockReleases;if(releaseAck)modifier=modifierContext=0;}
  return true;
 }
 if(action==0){++attackPresses;if(!preparedOkay)return false;
  auto* scope=g_input_scope117;if(scope){scope->visited=true;scope->prepareStableR18=true;scope->prepareSourceR18=++sourceGeneration;}
  observed.requested=true;return true;
 }
 if(action==1){++attackReleases;if(!releaseOkay)return false;
  auto* scope=g_input_scope117;if(scope){scope->visited=true;scope->releaseStableR24=true;scope->releaseSourceR24=sourceGeneration;}
  observed.requested=false;observed.type=4;return true;
 }
 observed.requested=false;return true;
}
static unsigned checks=0;
static void check(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL: %s (check %u)\n",why,checks);std::exit(1);}}
static b168::SpecialRequest reset(){
 now=1000;sourceGeneration=7;modifier=modifierContext=0;
 blockPresses=blockReleases=attackPresses=attackReleases=0;events.clear();memory.clear();
 ownerAvailable=contextAvailable=observeAvailable=trackingAvailable=retail=inputHook=armReady=zoneOkay=true;
 reference=immediateGuard=false;releaseAck=pressOkay=preparedOkay=releaseOkay=true;
 g_pose_epoch6689=9;g_guard_manual_occurrence_r24=3;g_guard_lease_h14=4;g_special_context168=6;
 g_special_allowed168=g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_guard_manual112=false;
 animation_filter::nativeBodyOwned112=false;g_guard_manual_owner112=nullptr;g_phys_native6698={};
 currentOwner={reinterpret_cast<void*>(0x1000),reinterpret_cast<void*>(0x2000),reinterpret_cast<void*>(0x3000),9};
 currentLoadout={};currentLoadout.manager=0x4000;currentLoadout.right=22;currentLoadout.rightGeneration=1;currentLoadout.rightClass=4;currentLoadout.leftClass=4;currentLoadout.weaponRequest=1;
 observed={};observed.phase=1;tracking={};g_native_free_kick_r24={};g_input_scope117=nullptr;
 const unsigned char use[]{0x48,0x8B,0x89,0x10,0x02,0,0,0x48,0x83,0xC1,0x60,0x48,0x8B,0x01,0xFF,0x50,0x08};
 const unsigned char get[]{0x8B,0x41,0x08,0xC3};
 put(imageBase+0x12EDF58,use);put(imageBase+0x5320D18,23);put(0x3060,propertyTable);put(propertyTable+8,getter);put(getter,get);put(0x3068,0);
 return {77,1000,6,9,0x1000,0x2000,0x3000};
}
static b168::Event start(const b168::SpecialRequest& r){b168::Event e{};e.kind=1;e.serial=r.serial;e.tick=now;e.actor=r.actor;NativeFreeKickDispatchR24(r,currentOwner,observed,e);return e;}
static void frame(std::uint64_t tick){now=tick;tracking.tick=now;NativeFreeKickTickR24();}
'''

cases = r'''
int main(){
 auto r=reset();auto first=start(r);
 check(blockPresses==1,"one original modifier press begins the transaction");
 check(attackPresses==0&&attackReleases==0,"MAX alone cannot dispatch before current native FreeGuard acknowledgement");
#ifdef HAS_D84_WAIT
 check(first.reason==unsigned(b168::KickGateH31::FreeGuardPendingD84)&&g_native_free_kick_r24.awaitingGuardD84,"missing guard ACK owns one bounded wait");
 check(NativeFreeKickGuardRetainedR24(),"physical guard cannot release our waiting modifier");
 frame(1011);check(blockPresses==1&&!attackPresses&&events.empty(),"native pump does not replay modifier or log each waiting frame");
 put(0x3068,23);frame(1022);
 check(attackPresses==1&&attackReleases==1&&blockPresses==1&&blockReleases==1,"real guard ACK dispatches one balanced native request and releases our modifier once");
 check(!g_native_free_kick_r24.owner.player&&!NativeFreeKickGuardRetainedR24(),"completed transaction retires its ownership");
 check(events.size()==1&&events[0].serial==77&&events[0].requestedTickD84==1000&&events[0].freeGuardD84==23&&events[0].modifierD84==1&&events[0].inputWitnessH31==3,"terminal receipt retains original identity and distinguishes guard ACK from handler boundaries");
 frame(1033);check(attackPresses==1&&blockReleases==1,"later frame cannot replay completed R3");

 r=reset();immediateGuard=true;start(r);
 check(attackPresses==1&&attackReleases==1&&blockReleases==1,"already acknowledged native guard takes the immediate path");
 r=reset();modifier=modifierContext=1;put(0x3068,23);start(r);
 check(!blockPresses&&!blockReleases&&attackPresses==1,"existing modifier is borrowed without claiming its release");

 r=reset();start(r);frame(1300);check(!attackPresses&&!blockReleases,"R99 300 ms guard-wait deadline is retained through its inclusive bound");
 frame(1301);check(!attackPresses&&blockReleases==1&&!g_native_free_kick_r24.owner.player,"timeout releases only our original modifier without an attack");
 check(events.size()==1&&events[0].reason==unsigned(b168::KickGateH31::FreeGuardExpiredD84),"timeout records the actual missing native ACK");
 check(events[0].freeGuardD84==0,"expiry row names the guard the game actually had");
 put(0x3068,23);frame(1310);check(!attackPresses,"late native ACK cannot revive an expired R3");

 for(int fault=0;fault<17;++fault){
  r=reset();start(r);put(0x3068,23);
  switch(fault){case 0:++g_pose_epoch6689;break;case 1:++currentOwner.epoch;break;
   case 2:++g_special_context168;break;case 3:++currentLoadout.rightGeneration;break;
   case 4:contextAvailable=false;break;case 5:tracking.allowed=false;break;case 6:tracking.head_valid=false;break;
   case 7:tracking.head_held=true;break;case 8:g_gameplay_allowed=false;break;case 9:g_special_allowed168=false;break;
   case 10:g_ctx_menu=true;break;case 11:animation_filter::nativeBodyOwned112=true;break;
   case 12:g_phys_native6698.attackPrepared=true;break;case 13:observed.requested=true;break;
   case 14:observed.phase=8;observed.type=1;break;case 15:inputHook=false;break;case 16:modifier=0;break;}
  frame(1011);check(!attackPresses&&!attackReleases,"changed context, owner, equipment, tracking or native input cannot inherit waiting R3");
  check(blockReleases==1,"revocation cleans the original modifier rather than leaving it held");
 }
 for(int takeover=0;takeover<2;++takeover){r=reset();start(r);if(takeover)++g_guard_lease_h14;else ++g_guard_manual_occurrence_r24;
  put(0x3068,23);frame(1011);check(!attackPresses&&!blockReleases&&!g_native_free_kick_r24.owner.player,"later manual/physical guard owns its own cleanup");}

 r=reset();start(r);ownerAvailable=false;frame(1011);
 check(!attackPresses&&!blockReleases&&g_native_free_kick_r24.owner.player&&g_native_free_kick_r24.revoked,"temporary owner read failure retains only revoked cleanup debt");
 check(NativeFreeKickGuardRetainedR24(),"unresolved original modifier stays fenced pending known-owner cleanup");
 ownerAvailable=true;put(0x3068,23);frame(1022);
 check(!attackPresses&&blockReleases==1&&!g_native_free_kick_r24.owner.player,"recovered owner permits cleanup but never replays the revoked R3");
 r=reset();start(r);currentOwner.actor=reinterpret_cast<void*>(0x9000);frame(1011);
 check(!attackPresses&&!blockReleases&&!g_native_free_kick_r24.owner.player,"positive actor replacement retires without sending old cleanup to a new actor");

 r=reset();start(r);releaseAck=false;frame(1301);check(blockReleases==1&&g_native_free_kick_r24.releaseQueued,"release return alone does not manufacture native LT-up acknowledgement");
 frame(1312);check(blockReleases==1&&g_native_free_kick_r24.owner.player,"pending native release is not replayed");
 modifierContext=0;frame(1323);check(!g_native_free_kick_r24.owner.player,"actual native LT-up acknowledgement retires cleanup debt");
 r=reset();memory[getter]=0;start(r);check(!attackPresses&&blockReleases==1,"unknown property getter refuses instead of assuming a FreeGuard value");
 r=reset();memory[imageBase+0x12EDF58]=0;start(r);check(!attackPresses&&blockReleases==1,"native reader pin mismatch refuses without any attack");
 r=reset();start(r);observed.phase=128;observed.type=-1;observed.input=-1;observed.zone=7;observed.hand=1;put(0x3068,23);frame(1011);
 check(attackPresses==1&&attackReleases==1,"known native block metadata may hand off after the distinct FreeGuard ACK");
 // R99: R97 log 00:36:34-00:37:47, every R3 refused INPUT_NOT_WRITABLE. Our modifier press leaves
 // free block phase 256, type -1, zone -1, hand 1 (V66.104 NATIVE_STATE row), not the zone-7 metadata.
 r=reset();start(r);observed.phase=256;observed.type=-1;observed.input=-1;observed.zone=-1;observed.hand=1;frame(1011);
 check(!attackPresses&&g_native_free_kick_r24.awaitingGuardD84,"R99 free block 256/zone -1 waits for FreeGuard instead of refusing");
 put(0x3068,23);frame(1022);
 check(attackPresses==1&&attackReleases==1&&blockReleases==1,"R99 free block 256/zone -1 + FreeGuard 23 dispatches the native kick");
 check(events.size()==1&&events[0].reason==0&&events[0].type==4,"R99 terminal row is a kick (type 4) and earns the cooldown");
 r=reset();observed.phase=256;observed.type=-1;observed.zone=-1;observed.hand=1;start(r);
 check(blockPresses==1,"R99 residual free block before the press (R97 serial 22) still starts the kick");
 put(0x3068,23);frame(1011);check(attackPresses==1&&attackReleases==1,"R99 residual free block kick completes");
 r=reset();start(r);observed.phase=256;observed.type=1;observed.zone=-1;observed.hand=1;observed.requestCandidate=true;frame(1011);
 check(!attackPresses&&blockReleases==1,"R99 an attack candidate in phase 256 is still busy");
 r=reset();start(r);observed.phase=256;observed.type=5;frame(1011);
 check(!attackPresses&&blockReleases==1,"R99 a punch (type 5) in phase 256 is still busy");
 std::printf("D84 actual free-kick dispatch/wait/cleanup: %u checks PASS; native leaves mocked; headset/animation/damage NOT_RUN\n",checks);
#endif
}
'''

unit = out / 'free_kick_ack_production.cpp'
unit.write_text(('#define HAS_D84_WAIT\n' if has_wait else '') + host + owner + leaves + definitions + cases)
exe = out / 'free_kick_ack_production'
cmd = ['g++', '-std=c++20', '-O1' if a.sanitize else '-O2', '-I'+str(root/'src'), str(unit), '-o', str(exe)]
if a.sanitize:
    cmd[1:1] = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer']
build = subprocess.run(cmd, capture_output=True, text=True)
(out/'build.log').write_text(build.stdout+build.stderr)
result = {'source':str(source), 'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),
          'extracted_sha256':hashlib.sha256((owner+definitions).encode()).hexdigest(),
          'command':cmd, 'compile_rc':build.returncode, 'native_game_outcome':'NOT_RUN'}
if build.returncode == 0:
    run = subprocess.run([str(exe)], capture_output=True, text=True,
                         env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0'), timeout=30)
    result.update(run_rc=run.returncode, output=run.stdout+run.stderr)
    (out/'run.log').write_text(run.stdout+run.stderr)
    print(run.stdout+run.stderr)
else:
    print(build.stdout+build.stderr)
exe.unlink(missing_ok=True)
(out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
raise SystemExit(result.get('run_rc',build.returncode))
