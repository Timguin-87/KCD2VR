#!/usr/bin/env python3
"""Verify the actual D83R1 quiver arbiter and unchanged bow nock-commit body."""
from pathlib import Path
import argparse, hashlib, json, os, subprocess

ROOT=Path(__file__).resolve().parents[1]

def block(text,signature):
    start=text.index(signature);brace=text.index('{',start);end=brace+1;depth=1
    while depth:
        depth+=(text[end]=='{')-(text[end]=='}');end+=1
    return text[start:end],text[brace+1:end-1]

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out',type=Path,required=True)
    ap.add_argument('--baseline',type=Path,help='Reviewed D82 source root, for old-quiver negative control')
    ap.add_argument('--claude',type=Path,help='Claude D83 overlay root, for swallowed bow ACK negative control')
    a=ap.parse_args();out=a.out.resolve();assert ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
    holster=ROOT/'src/subsystems/throw_drop/holster_gestures.inl'
    ranged=ROOT/'src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl'
    staged=ROOT/'src/subsystems/ranged_reload/staged_reload596c2.inl'
    arbiter,_=block(holster.read_text(),'static bool PhysBowHeldR9(')
    enabled,_=block(ranged.read_text(),'static bool CrossbowReloadEnabledD65C(')
    _,commit=block(staged.read_text(),'else if(command==stage596::Command::Nock)')
    wiring=holster.read_text()
    for token in ['arrow_hand_blocks_draw(gesture,PhysBowHeldR9(e)',
                  'const bool bowHeldR9=PhysBowHeldR9(e);',
                  'arrow_hand_blocks_draw(hand,bowHeldR9,',
                  'arrow_hand_blocks_draw(drawHand,PhysBowHeldR9(e),']:
        assert token in wiring,token
    prefix=r'''#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include "vrik/bow_arm_aim_r9.h"
#include "vrik/instant_activities_d72.h"
#include "subsystems/ranged_reload/options_xr32.h"
struct Profile{int classId=0;bool bow()const{return classId==9;}};
struct PhysEquipment6698{int count=0;struct Item{bool held=false;Profile profile{};};std::array<Item,8>items{};};
static int mode=0;int ReloadMode47C2()noexcept{return mode;}
'''
    between=r'''
namespace ph{struct BowNockAck19c4r4{std::uint64_t key=0,generation=0,request=0,body=0,releaseSample56=0,releaseTick56=0,handEntity56=0,handIdentity56=0;bool releaseToNock56=false;};}
struct ManualBow596{int side=0;std::uint32_t hand=9;void*entity=reinterpret_cast<void*>(123);std::uint64_t weapon=2,generation=3,request=4,epoch=5;bool nockRequested=false,handReceipt=false;ph::BowNockAck19c4r4 nockRelease56{},nockAck{};struct{}policy{};};
struct Tracking{double right_grip=.1;std::uint64_t sequence=8,tick=99;};
std::atomic<bool>g_manual_bow_fault596{false};static bool nativeSuccess=true;
bool ManualNock596(ManualBow596&m){if(nativeSuccess)m.hand=0;return nativeSuccess;}
bool ManualRemove596(ManualBow596&m){m.hand=0;return true;}
void ManualNote596(const char*,const ManualBow596&,bool){}
void nockCommit(ManualBow596&m,Tracking tracking,bool crossbowD82,bool cockedD82,bool handAttached){(void)crossbowD82;(void)cockedD82;
'''
    suffix=r'''}
int checks=0,failed=0;void ck(bool value,const char*name){++checks;if(!value){++failed;std::printf("FAIL_D83 %s\n",name);}}
int main(){using namespace kcd2vr;namespace ba=bow_arm_aim_r9;
for(int classId:{7,9,10,14,15})for(int flags=0;flags<8;++flags){
instant_d72::activities.store(true);instant_d72::reloads.store((flags&1)!=0);mode=(flags&2)?2:0;ranged_options_d65c::crossbowReload.store((flags&4)!=0);
PhysEquipment6698 e{};e.count=1;e.items[0].held=true;e.items[0].profile.classId=classId;
const bool expected=classId==9||(ranged_kind_xr32::crossbow(classId)&&CrossbowReloadEnabledD65C());
const bool reserve=PhysBowHeldR9(e);
ck(reserve==expected,"held class matches the actual reload entry modes");
ck(ba::arrow_hand_blocks_draw(0,reserve,true)==expected,"right shoulder only belongs to current quiver owner");
ck(!ba::arrow_hand_blocks_draw(1,reserve,true),"left hand remains available");
ck(!ba::arrow_hand_blocks_draw(0,reserve,false),"belt draw remains available");
e.items[0].held=false;ck(!PhysBowHeldR9(e),"stowed ranged item cannot reserve quiver");
}
PhysEquipment6698 e{};e.count=-1;ck(!PhysBowHeldR9(e),"negative count rejected");e.count=9;ck(!PhysBowHeldR9(e),"oversized count rejected");
e.count=2;e.items[0]={true,{10}};e.items[1]={true,{9}};instant_d72::activities=false;instant_d72::reloads=true;mode=0;ranged_options_d65c::crossbowReload=false;ck(PhysBowHeldR9(e),"bow reservation survives disabled crossbow path");
e.count=1;ck(!PhysBowHeldR9(e),"global instant-off is respected");
mode=1;ck(!PhysBowHeldR9(e),"diagnostic-only rung one does not claim quiver");
ManualBow596 bow{};nockCommit(bow,{},false,false,true);ck(bow.nockRequested,"native nock result retained");ck(bow.nockRelease56.releaseToNock56,"release acknowledgement retained");
ck(bow.nockRelease56.handEntity56==9&&bow.nockRelease56.handIdentity56==123,"exact original hand identity retained before removal");
ck(bow.nockRelease56.releaseSample56==8&&bow.nockRelease56.releaseTick56==99,"original release sample retained");
nativeSuccess=false;ManualBow596 refused{};nockCommit(refused,{},false,false,true);ck(!refused.nockRequested&&!refused.nockRelease56.releaseToNock56,"refused native nock grants no release");
std::printf("D83R1 quiver / actual bow commit: %d checks, %d failures; native leaves controlled, game/headset NOT_RUN\n",checks,failed);return failed?1:0;
}
'''
    runs=[]
    def run(name,ar=arbiter,co=commit,flags=('-O2',),negative=False):
        d=out/name;d.mkdir(exist_ok=True);cpp=d/'actual.cpp';cpp.write_text(prefix+enabled+'\n'+ar+between+co+suffix)
        command=['g++','-std=c++20','-pthread','-Wall','-Wextra',*flags,'-I',str(ROOT/'src'),str(cpp),'-o',str(d/'test')]
        build=subprocess.run(command,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(d/'build.txt').write_text(build.stdout)
        if build.returncode:raise RuntimeError(name+' compile failed\n'+build.stdout)
        env=os.environ.copy();env['ASAN_OPTIONS']='detect_leaks=0'
        result=subprocess.run([str(d/'test')],text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,env=env)
        passed=result.returncode==(1 if negative else 0) and (not negative or 'FAIL_D83' in result.stdout)
        runs.append({'name':name,'command':command,'exit':result.returncode,'expected_failure':negative,'passed':passed,'output':result.stdout});print(name+': '+result.stdout,end='')
        if not passed:raise RuntimeError(name+' unexpected result')
    run('optimized');run('asan_ubsan',flags=('-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie'))
    if a.baseline:
        old,_=block((a.baseline/holster.relative_to(ROOT)).read_text(),'static bool PhysBowHeldR9(')
        run('old_crossbow_quiver',ar=old,negative=True)
    if a.claude:
        _,old=block((a.claude/staged.relative_to(ROOT)).read_text(),'else if(command==stage596::Command::Nock)')
        run('claude_bow_ack',co=old,negative=True)
    report={'passed':all(r['passed'] for r in runs),'runs':runs,
            'source_sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [holster,ranged,staged,Path(__file__).resolve()]},
            'slices_sha256':{name:hashlib.sha256(data.encode()).hexdigest() for name,data in [('arbiter',arbiter),('actual_reload_enable',enabled),('actual_bow_commit',commit)]},
            'limits':'Literal production quiver arbiter, native reload-enable predicate and bow nock-commit branch. Equipment container and native nock/remove leaves controlled. Full VR frame, real native API, Windows build and headset NOT_RUN.'}
    (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':main()
