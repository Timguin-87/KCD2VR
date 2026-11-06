"""Actual native fist-state admission getter and copied-claim free fastpath."""
from pathlib import Path
import argparse
import hashlib
import json
import subprocess
from test_throw_authority_t22 import ROOT, function


def item(text, name):
    at=0
    while True:
        a=text.index(name,at);a=text.rfind('\n',0,a)+1;b=text.index('{',a)
        semicolon=text.find(';',a,b)
        if semicolon<0:break
        at=semicolon+1 # skip a qualified forward declaration before the definition
    depth=1;c=b+1
    while depth:depth+=(text[c]=='{')-(text[c]=='}');c+=1
    if text[c:c+1]==';':c+=1
    return text[a:c]+'\n'



def entry_probe(text):
    # Compile the exact entry owner. The remaining action/native body is an
    # explicit callback probe, never a replacement TLS flag or admission policy.
    if 'static void FistNativeTickBodyT22() noexcept {' in text:
        wrapper=item(text,'static void FistNativeTick101() noexcept {')
        body=item(text,'static void FistNativeTickBodyT22() noexcept {')
        prefix=body[body.index('{')+1:body.index('    auto& n=g_fist_native101;')]
        assert 'std::unique_lock<std::recursive_mutex> guard' in prefix and 'if(!guard.owns_lock())return;' in prefix
        assert '__try {FistNativeTickBodyT22();}' in wrapper
        assert '__finally {g_fist_native_tick_active_t22=previous;}' in wrapper
        driver=('static thread_local void(*admission_callback_t22)()=nullptr;\n'
            'static void FistNativeTickBodyT22() noexcept {\n'+prefix+
            ' assert(admission_callback_t22);admission_callback_t22();\n}\n'+wrapper+
            'static void entry(void(*callback)()){\n'
            ' const auto previous=admission_callback_t22;admission_callback_t22=callback;\n'
            ' FistNativeTick101();admission_callback_t22=previous;\n}\n')
        return driver,{'kind':'actual_try_finally_wrapper_and_actual_mutex_prefix',
            'wrapper_sha256':hashlib.sha256(wrapper.encode()).hexdigest(),
            'body_prefix_sha256':hashlib.sha256(prefix.encode()).hexdigest(),
            'full_native_body_sha256':hashlib.sha256(body.encode()).hexdigest(),
            'body_tail':'Explicit callback probe; native action/owner/body tail NOT executed.',
            'escaping_SEH':'Covered by separate full-function T22_FIST_SCOPE_SEH receipt, not by this60-check observer fixture.'}
    scope=text.split('// T22_ADMISSION_SCOPE_BEGIN\n',1)[1].split('// T22_ADMISSION_SCOPE_END',1)[0]
    return 'static void entry(void(*callback)()){\n'+scope+' callback();\n}\n',{
        'kind':'actual_legacy_marked_scope','scope_sha256':hashlib.sha256(scope.encode()).hexdigest()}

FIXTURE=r'''
#include <atomic>
#include <barrier>
#include <mutex>
#include <thread>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "vrik/fist_motion.h"
#include "subsystems/throw_drop/hand_claim_t24.h"
namespace fm=kcd2vr::fists;
namespace ac=kcd2vr::ammo_carry_t18;
#include "actual_state.inc"
#include "actual_enum.inc"
static std::recursive_mutex g_fist_native_mutex101;
static FistNativeState101 g_fist_native101;
static std::atomic<bool> g_fist_fault101{false};
static std::atomic<std::uint64_t> g_pose_epoch6689{44},g_weapon_request_epoch6694{55};
static std::atomic<void*> g_committed_character6689{reinterpret_cast<void*>(33)};
#include "actual_entry_flag.inc"
static unsigned nativeReads=0;
struct Memory{unsigned char bytes[0xA00]{};}manager,player;
template<class T>static void put(Memory& m,unsigned off,T value){std::memcpy(m.bytes+off,&value,sizeof(value));}
static std::atomic<void*>g_henry_item_attach_manager{&manager};
static bool PhysVft6698(void* p,std::uintptr_t vft){++nativeReads;return (p==&manager&&vft==0x3EC9958)||(p==&player&&vft==0x3AA4B98);}
static bool Read(void* p,void*& value){++nativeReads;std::memcpy(&value,p,sizeof(value));return true;}
#include "actual_claim.inc"
#include "actual_admission.inc"
static unsigned checks=0;
static void ck(bool ok){assert(ok);++checks;}
static void reset(){g_fist_native101={};g_fist_fault101=false;g_pose_epoch6689=44;g_weapon_request_epoch6694=55;
 g_committed_character6689=reinterpret_cast<void*>(33);ac::hands=0;for(auto& c:ac::claims)c.publish({});
 put(manager,0x38,static_cast<void*>(&player));put(player,0x9D0,static_cast<void*>(&manager));nativeReads=0;}
static ThrowAuthorityT22 call(int side=0,std::uintptr_t actor=11){return ThrowHandAdmissionT22(side,actor,33,44,55);}
#include "actual_scope.inc"
static void claim(int side,bool foreign=false){ac::claims[side].publish({foreign?111u:reinterpret_cast<std::uintptr_t>(&player),foreign?222u:reinterpret_cast<std::uintptr_t>(&manager),33,77,101,202,44,55,1,1000,9,ac::Stage::Held});ac::hands=1u<<side;}
int main(){
 reset();ck(call()==ThrowAuthorityT22::Allowed);ck(call(1)==ThrowAuthorityT22::Allowed);
 ck(call(-1)==ThrowAuthorityT22::Unknown);ck(call(2)==ThrowAuthorityT22::Unknown);
 ck(ThrowHandAdmissionT22(0,0,33,44,55)==ThrowAuthorityT22::Unknown);
 ck(ThrowHandAdmissionT22(0,11,0,44,55)==ThrowAuthorityT22::Unknown);
 ck(ThrowHandAdmissionT22(0,11,33,0,55)==ThrowAuthorityT22::Unknown);
 ck(ThrowHandAdmissionT22(0,11,33,44,0)==ThrowAuthorityT22::Unknown);
 for(int side:{0,1}){reset();auto& n=g_fist_native101;n.player=reinterpret_cast<void*>(11);n.prepared=true;n.side=side;n.epoch=44;n.request=55;
  ck(call(side)==ThrowAuthorityT22::Denied);ck(call(1-side)==ThrowAuthorityT22::Allowed);
  ck(n.prepared&&n.side==side&&n.epoch==44&&n.request==55); // observer never cancels/retags original action
  n.epoch=43;n.request=54;ck(call(side)==ThrowAuthorityT22::Denied);ck(call(1-side)==ThrowAuthorityT22::Allowed);
  ck(call(side,22)==ThrowAuthorityT22::Allowed); // positive new player never inherits old request
 }
 for(int side:{0,1}){reset();auto& n=g_fist_native101;n.player=reinterpret_cast<void*>(11);n.waitingGuardU16=true;n.guardPunchU16.valid=true;n.guardPunchU16.side=side;
  ck(call(side)==ThrowAuthorityT22::Denied);ck(call(1-side)==ThrowAuthorityT22::Allowed);ck(n.waitingGuardU16&&n.guardPunchU16.valid);
  n.guardPunchU16.valid=false;ck(call(side)==ThrowAuthorityT22::Unknown);
 }
 for(int bad:{-1,2}){reset();auto& n=g_fist_native101;n.player=reinterpret_cast<void*>(11);n.prepared=true;n.side=bad;ck(call()==ThrowAuthorityT22::Unknown);}
 reset();g_fist_native101.prepared=true;ck(call()==ThrowAuthorityT22::Unknown);
 reset();g_fist_fault101=true;ck(call()==ThrowAuthorityT22::Unknown);g_fist_native101.player=reinterpret_cast<void*>(11);
 ck(call()==ThrowAuthorityT22::Unknown);ck(call(0,22)==ThrowAuthorityT22::Allowed);
 reset();++g_pose_epoch6689;ck(call()==ThrowAuthorityT22::Unknown);
 reset();++g_weapon_request_epoch6694;ck(call()==ThrowAuthorityT22::Unknown);
 reset();g_committed_character6689=reinterpret_cast<void*>(34);ck(call()==ThrowAuthorityT22::Unknown);
 reset();std::barrier gate(2);std::thread contender([&]{std::lock_guard<std::recursive_mutex>lock(g_fist_native_mutex101);gate.arrive_and_wait();gate.arrive_and_wait();});
 gate.arrive_and_wait();ck(call()==ThrowAuthorityT22::Unknown);ck(call(1)==ThrowAuthorityT22::Unknown);gate.arrive_and_wait();contender.join();ck(call()==ThrowAuthorityT22::Allowed);
 // Native callbacks can reenter on this SAME thread while the recursive lock
 // is held and before prepared=true. Execute the exact production entry scope.
 reset();entry([]{std::unique_lock<std::recursive_mutex>same(g_fist_native_mutex101);
  ck(call()==ThrowAuthorityT22::Unknown);ck(call(1)==ThrowAuthorityT22::Unknown);
  entry([]{ck(call()==ThrowAuthorityT22::Unknown);});ck(call()==ThrowAuthorityT22::Unknown);
 });ck(!g_fist_native_tick_active_t22);ck(call()==ThrowAuthorityT22::Allowed);
 // OFF/free path: actual production helper must not read native manager/vfts.
 reset();ck(!HandClaimBlockedT24(0,33,44,55));ck(!HandClaimBlockedT24(1,33,44,55));ck(nativeReads==0);
 ck(HandClaimBlockedT24(-1,33,44,55)); // invalid side stays unknown/blocked
 for(int side:{0,1}){reset();claim(side);ck(HandClaimBlockedT24(side,33,44,55));ck(!HandClaimBlockedT24(1-side,33,44,55));ck(nativeReads>0);}
 reset();ac::hands=1;ck(HandClaimBlockedT24(0,33,44,55)); // partial publication cannot become free
 reset();claim(0,true);ck(!HandClaimBlockedT24(0,33,44,55));ck(nativeReads>0);
 reset();claim(0);ck(HandClaimBlockedT24(0,33,45,56)); // sameactor old cleanup debt
 std::printf("{\"checks\":%u,\"passed\":true,\"native_execution\":false}\n",checks);
}
'''


def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True);args=ap.parse_args()
    out=args.out.resolve();assert out!=ROOT and ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
    cpp=ROOT/'src/frik_pose_probe.cpp';header=ROOT/'src/frik_pose_probe.h';native=ROOT/'src/vrik/fist_native.inl'
    getter=function(cpp.read_text(),'ThrowHandAdmissionT22')
    claim=item(cpp.read_text(),'static bool HandClaimBlockedT24(')
    state=item(native.read_text(),'struct FistNativeState101 ')
    entry_flag=next(line for line in native.read_text().splitlines() if line.startswith('static thread_local bool g_fist_native_tick_active_t22'))
    scope,scope_provenance=entry_probe(native.read_text())
    enum=next(line for line in header.read_text().splitlines() if line.startswith('enum class ThrowAuthorityT22'))
    assert not any(s in getter for s in ('FistCancel','FistAbort','Native13<','ReadCommittedPacket','g_frame_publication'))
    fixture=FIXTURE;source_files=[cpp,header,native];reload_dependency={'kind':'none'}
    if 'reload_throw_claim_t25::observeReloadDebt' in getter:
        dependency=ROOT/'src/subsystems/ranged_reload/reload_throw_claim_t25.h'
        raw=ROOT/'src/subsystems/ranged_reload/crossbow_native_xr34.h'
        mutual=ROOT/'src/core/mutual_admission_t25.h'
        source_files.extend([dependency,raw,mutual])
        fixture=fixture.replace('#include "subsystems/throw_drop/hand_claim_t24.h"',
            '#include "subsystems/throw_drop/hand_claim_t24.h"\n'
            '#include "subsystems/ranged_reload/reload_throw_claim_t25.h"\n'
            '// Explicit native callback-counter leaf: ordinary fist fixture has no XR34 native entry.\n'
            'namespace kcd2vr::action_time_owner59c11 {bool CrossbowNativeAdmissionBusyXR34() noexcept{return false;}}')
        fixture=fixture.replace('static void reset(){',
            'static void reset(){\n'
            ' kcd2vr::crossbow_native_xr34::activeNativeLease.store(0);\n'
            ' kcd2vr::crossbow_native_xr34::cleanupInterested.store(false);\n'
            ' assert(kcd2vr::crossbow_native_xr34::statusMailbox.publish({}));\n')
        reload_dependency={'kind':'actual_typed_reload_debt_header_and_raw_atomics',
            'native_entry_counter_leaf':'explicit idle false; no native XR34 entry in these60 fist cases',
            'raw_reload_state':'actual activeNativeLease=0/cleanupInterested=false/statusMailbox empty per fixture reset',
            'policy':'actual observeReloadDebt/classifyReloadDebt, never a constant-Free substitute',
            'nonidle_coverage':'Separate actual raw-debt/threadentry/shared-native116/311 fixtures, not claimed here.'}

    for name,data in [('actual_state.inc',state),('actual_enum.inc',enum),('actual_claim.inc',claim),('actual_admission.inc',getter),('actual_entry_flag.inc',entry_flag),('actual_scope.inc',scope),('fixture.cpp',fixture)]:
        (out/name).write_text(data+'\n')
    vc='C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
    cmd=out/'run.cmd';cmd.write_text('@echo off\ncall "'+vc+'" >nul\ncd /d "'+str(out)+'"\ncl /nologo /std:c++latest /EHsc /Od /Zi /UNDEBUG /fsanitize=address /I "'+str(ROOT/'src')+'" fixture.cpp /Fe:test.exe\nif errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n')
    p=subprocess.run(['cmd','/c',str(cmd)],capture_output=True,text=True);log=p.stdout+p.stderr;(out/'RUN.log').write_text(log);print(log)
    rows=[line for line in log.splitlines() if line.startswith('{"checks":')]
    report={'passed':p.returncode==0 and bool(rows),'fixture':json.loads(rows[-1]) if rows else None,
      'source_hashes':{str(f.relative_to(ROOT)):hashlib.sha256(f.read_bytes()).hexdigest() for f in source_files},
      'actual_getter_sha256':hashlib.sha256(getter.encode()).hexdigest(),
      'entry_scope':scope_provenance,
      'reload_dependency':reload_dependency,
      'limits':['Actual state layout/getter/claim observer; native identity reads are explicit allocated-memory leaves.',
                'New admission only; no native request cancel, cleanup or headset execution.']}
    (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n')
    return not report['passed']


if __name__=='__main__':raise SystemExit(main())
