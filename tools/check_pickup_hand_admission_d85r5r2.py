#!/usr/bin/env python3
"""Replay actual ordinary-pickup admission and its actual hand selection block.

This is a narrow integration check, not a Windows/DLL or native-physics build.
The actual capture gate, receipt gate, mass/bounds and input helpers are included.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

PRELUDE = r'''
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include "diagnostics/capture168.h"
#include "diagnostics/interaction_receipt59c10.h"
#include "vrik/lab_prop58.h"
#include "vrik/pickup_input_d80.h"
static int fault=0,lockDepth=0,writeWhileLocked=0,requestedSide=-1;
static unsigned freeHands=3,trackedHands=3;
static bool requestedPose[2]{true,true},usedPhysics=false,modeFlipDone=false;
static float handDistance[2]{0,0};
static unsigned entities=0,links=0,contactCalls=0;
static std::uint64_t clockNow=100;
static std::vector<std::string> lines;
static unsigned char henry[64]{},item[256]{},definition[128]{},linkObject[64]{},entity[64]{},character[8]{},proxy[8]{};
static std::uint64_t GetTickCount64(){return clockNow;}
static std::uint32_t GetCurrentThreadId(){return fault==2?99:7;}
static std::uintptr_t WhBase(){return 0x10000000;}
namespace cap=kcd2vr::diagnostics::capture168;
namespace diagnostic_trace {static void Write(const char* line){
 if(lockDepth)++writeWhileLocked;
 if(cap::accept(line,clockNow))lines.emplace_back(line);
}}
namespace kcd2vr::lab58 {
enum class Sub{PropWake};static std::atomic<int> f9Rung{0};static std::atomic<bool> uiBlocked{false};
static bool active(Sub){return false;}
}
namespace kcd2vr::workstation178r20c1 {static bool ownsWrist(std::uint64_t){return false;}}
namespace retail_build112 {static bool Ready(std::uintptr_t){return true;}}
namespace action_time_owner59c11 {
static bool RequestedPickupSideD85(std::uint32_t entity,int& side){side=requestedSide;return entity==51&&side>=0&&side<2;}
}
namespace prop5916 {static std::atomic<bool> enabled{true};}
namespace tracking59c6 {static std::atomic<std::uint64_t> epoch{1};}
static std::atomic<std::uint64_t> g_stage_tick596{100},g_pose_epoch6689{1};
static std::atomic<std::uint32_t> g_stage_thread596{7};
static std::atomic<int> g_grab_mode60{2};
static std::atomic<bool> g_prop_fault5916c1{false},g_committed_pose_valid6689{true};
static std::atomic<unsigned> g_lifecycle_coverage175{1};
static std::atomic<void*> g_last_player_entity{henry},g_committed_character6689{character};
static bool g_prop_replay596=false;
static struct {unsigned id=0;}g_prop596;
struct PropLock596 {bool owned=true;PropLock596(){++lockDepth;}~PropLock596(){--lockDepth;}};
template<class T>static bool Read(const void* ptr,T& value){
 if(!ptr)return false;
 std::memcpy(&value,ptr,sizeof(T));
 if(ptr==linkObject+0x30&&++links==2&&fault==13){std::memset(&value,0,sizeof(T));}
 return true;
}
static bool SmallInstantPickupAllowedD81(std::uint64_t){return fault!=1;}
static bool LifecycleHenry165(std::uint32_t actor){return actor==42;}
static bool HeldInteractionVisual43(){return fault==4;}
static void* SmallPickupItemR24(std::uint64_t){return fault==5?nullptr:item;}
static void* EntityById6694(std::uint32_t){return (++entities>1&&fault==11)?nullptr:entity;}
static bool LifecycleEntity165(std::uintptr_t p,std::uint32_t& id){id=51;return p==reinterpret_cast<std::uintptr_t>(entity)&&fault!=7;}
static bool SmallPickupBodyR24(void*,void*& body){
 body=nullptr;
 if(fault==15)cap::gate.select(cap::Domain::Interaction,false,clockNow);
 if(fault==16&&!modeFlipDone){modeFlipDone=true;prop5916::enabled=!prop5916::enabled.load();}
 return true;
}
struct Matrix34V6550{float m[12]{};};
namespace pm {
struct Vec3{float x=0,y=0,z=0;};
static bool finite(Vec3 p){return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);}
static Vec3 operator-(Vec3 a,Vec3 b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
static float length(Vec3 a){return std::sqrt(a.x*a.x+a.y*a.y+a.z*a.z);}
}
namespace ph {struct Pose{pm::Vec3 p{};};}
// Native TU packet types are represented by the exact members used by this function.
// This intentionally does not claim ABI layout verification of the full pose packet.
struct CommittedFrame6695{
 struct{std::uint64_t tick=0,epoch=0;std::uintptr_t actor=0;}stamp;
 bool mapFoldTrackedM39=false,headEvidence122=false;
 bool valid[2]{},propRequestedValid5916c1[2]{};
 float mapGripM39[2]{},pickupTriggerD80[2]{};
};
struct TrackingSnapshot{
 std::uint64_t tick=0,source_epoch=0;
 bool allowed=false,head_valid=false,head_held=false,left_held=false,right_held=false;
 float right_grip=0,left_grip=0;
};
static bool ReadCommittedPacket6695(CommittedFrame6695& f){
 f.stamp={clockNow,1,reinterpret_cast<std::uintptr_t>(character)};
 f.mapFoldTrackedM39=f.headEvidence122=true;
 for(unsigned side=0;side<2;++side){f.valid[side]=true;f.propRequestedValid5916c1[side]=requestedPose[side];f.mapGripM39[side]=.8f;}
 return fault!=8;
}
static bool ReadTrackingSnapshot(TrackingSnapshot&){return false;}
static bool GetEntityWorldTmV6552(void*,std::uint32_t,Matrix34V6550& m){m={};m.m[0]=m.m[5]=m.m[10]=1;return fault!=9;}
static bool SmallPickupBoundsR24(void*,void*& p,float(&b)[6]){
 p=proxy;for(unsigned i=0;i<3;++i){b[i]=-.05f;b[i+3]=.05f;}return true;
}
static bool TrackingHandUsable111(int side,std::uint64_t=0){return fault!=10&&(trackedHands&(1u<<side));}
static bool TrackingHeadUsable111(std::uint64_t){return fault!=12;}
static bool PropHandFree596(int side){return (freeHands&(1u<<side))!=0;}
static bool FistInputOwned101(){return false;}
static bool PropPickupHandD81(const CommittedFrame6695& f,int side,bool physics,ph::Pose& p){
 usedPhysics=physics;p={};p.p.x=handDistance[side];
 return f.valid[side]&&(!physics||f.propRequestedValid5916c1[side]);
}
static std::uint32_t SmallContactAdmissionR24(std::uint64_t){++contactCalls;return 17;}
'''

TAIL = r'''
template<class T>static void put(unsigned char* where,T value){std::memcpy(where,&value,sizeof(value));}
static void setup(int which,bool capture=true){
 fault=which;clockNow+=2000;g_stage_tick596=clockNow;entities=links=contactCalls=0;
 requestedSide=-1;freeHands=trackedHands=3;requestedPose[0]=requestedPose[1]=true;
 handDistance[0]=handDistance[1]=0;usedPhysics=false;modeFlipDone=false;prop5916::enabled=true;g_prop_fault5916c1=false;
 lines.clear();writeWhileLocked=0;
 std::memset(henry,0,sizeof(henry));std::memset(item,0,sizeof(item));
 std::memset(definition,0,sizeof(definition));std::memset(linkObject,0,sizeof(linkObject));
 put(henry+0xC,std::uint32_t(42));put(item+0x48,static_cast<void*>(definition));
 put(item+0x88,static_cast<void*>(linkObject));put(linkObject+0x30,std::uint32_t(51));
 put(definition,WhBase()+kcd2vr::lab_prop58::kPickupClassVftD85[0]);
 put(definition+0x58,which==6?0.f:1.f);
 g_last_player_entity=which==3?nullptr:henry;
 cap::gate.select(cap::Domain::Interaction,capture,clockNow);
}
static unsigned checks=0;
#define CK(v) do{++checks;if(!(v)){std::fprintf(stderr,"native admission check %u failed: %s (fault=%d)\n",checks,#v,fault);return 1;}}while(0)
int main(){
 const char* expected[]={"READINESS_OR_CONTEXT","CALLBACK_THREAD_OR_FRAME","PLAYER_IDENTITY",
  "STATION_OR_EXISTING_HOLD","ITEM_INSTANCE","NATIVE_ITEM_CLASS_OR_WEIGHT","ENTITY_OR_STATIC_BODY",
  "CURRENT_COMMITTED_HAND_FRAME","TRACKING_OR_RENDER_BOUNDS","CURRENT_IDENTITY_RECHECK",
  "CURRENT_IDENTITY_RECHECK","LATEST_FRAME_AND_HAND_FRESHNESS","FINAL_NATIVE_ADMISSION"};
 for(int which=1;which<=13;++which){
  setup(which);CK(SmallPickupAdmissionR24(0x0200000000000007ull,false)==0);
  CK(lines.size()==1);CK(lines[0].find(std::string("phase=")+expected[which-1])!=std::string::npos);
  CK(lines[0].find("grams=0 prepared=NOT_EXECUTED_BY_NATIVE_READER")!=std::string::npos);
  CK(lockDepth==0&&writeWhileLocked==0);
 }
 setup(0);CK(SmallPickupAdmissionR24(0x0200000000000007ull,false)==1000);
 CK(lines.size()==1&&lines[0].find("phase=FINAL_NATIVE_ADMISSION grams=1000")!=std::string::npos);
 CK(lockDepth==0&&writeWhileLocked==0);
 // Same request is coalesced by the actual production receipt gate.
 entities=links=0;CK(SmallPickupAdmissionR24(0x0200000000000007ull,false)==1000);CK(lines.size()==1);
 setup(0,false);CK(SmallPickupAdmissionR24(7,false)==1000&&lines.empty());
 setup(1,false);CK(SmallPickupAdmissionR24(7,false)==0&&lines.empty());
 setup(15);CK(SmallPickupAdmissionR24(7,false)==1000&&lines.empty());
 setup(0);CK(SmallPickupAdmissionR24(7,true)==17&&contactCalls==1&&lines.empty());
 // Ordinary kinematic hold needs the same rigid-body preparation, but it
 // consumes the allowed wrist and must not require a physics-requested pose.
 setup(0);prop5916::enabled=false;requestedPose[0]=requestedPose[1]=false;
 CK(SmallPickupAdmissionR24(7,false)==1000&&!usedPhysics);
 setup(0);prop5916::enabled=false;g_prop_fault5916c1=true;
 CK(SmallPickupAdmissionR24(7,false)==1000&&!usedPhysics);
 setup(0);g_prop_fault5916c1=true;CK(SmallPickupAdmissionR24(7,false)==0);
 // An unrelated closer closed hand cannot authorize preparation for the
 // requested hand when that requesting hand is outside reach or unavailable.
 setup(0);requestedSide=1;handDistance[0]=.01f;handDistance[1]=.30f;
 CK(SmallPickupAdmissionR24(7,false)==0);
 setup(0);requestedSide=0;handDistance[0]=.30f;handDistance[1]=.01f;
 CK(SmallPickupAdmissionR24(7,false)==0);
 setup(0);requestedSide=1;freeHands=1;CK(SmallPickupAdmissionR24(7,false)==0);
 setup(0);requestedSide=0;trackedHands=2;CK(SmallPickupAdmissionR24(7,false)==0);
 setup(0);requestedSide=1;handDistance[1]=.20f;CK(SmallPickupAdmissionR24(7,false)==1000);
 setup(0);requestedSide=0;handDistance[0]=.20f;CK(SmallPickupAdmissionR24(7,false)==1000);
 setup(16);CK(SmallPickupAdmissionR24(7,false)==0);
 CK(lines.size()==1&&lines[0].find("phase=LATEST_FRAME_AND_HAND_FRESHNESS")!=std::string::npos);
 setup(16);prop5916::enabled=false;requestedPose[0]=requestedPose[1]=false;
 CK(SmallPickupAdmissionR24(7,false)==0);
 CK(lines.size()==1&&lines[0].find("phase=LATEST_FRAME_AND_HAND_FRESHNESS")!=std::string::npos);
 CK(cap::domain("D85R5R1 F5_PICKUP_PREPARE_NATIVE key=7")==cap::Domain::Interaction);
 CK(cap::domain("D85R5R1 F5_PICKUP_PREPARE_SCRIPT reason=SCRIPT_EXCEPTION")==cap::Domain::Interaction);
 std::printf("verbatim SmallPickupAdmissionR24: %u checks PASS; native/pose leaves substituted\n",checks);
 return 0;
}
'''


def selection(text):
    start=text.index('    double best=.25;')
    end=text.index('    if(!StageLeafPins596(kPropLeaves596))',start)
    return r'''
static int selectedSide=-1;
static std::string selectionRefusal;
static bool SelectActual(bool prepareInstantD72){
 struct {bool physics=false;int side=-1;std::uint32_t id=51;}p;
 CommittedFrame6695 f{};ReadCommittedPacket6695(f);
 TrackingSnapshot t{};t.right_grip=t.left_grip=.8f;
 ph::Pose object{};unsigned validMask=0,trackingMask=0,freeMask=0,gripMask=0;
 double reach[2]{-1,-1};
 selectedSide=-1;selectionRefusal.clear();
 const auto reject=[&](const char* reason){selectionRefusal=reason;return false;};
''' + text[start:end] + r'''
 selectedSide=p.side;return true;
}
'''


SELECTION_TAIL = r'''
static unsigned checks=0;
#define CK(v) do{++checks;if(!(v)){std::fprintf(stderr,"hand selection check %u failed: %s\n",checks,#v);return 1;}}while(0)
static void reset(){
 requestedSide=-1;freeHands=trackedHands=3;requestedPose[0]=requestedPose[1]=true;
 handDistance[0]=handDistance[1]=0;prop5916::enabled=true;fault=0;
}
int main(){
 reset();requestedSide=1;handDistance[0]=.01f;handDistance[1]=.19f;
 CK(SelectActual(true)&&selectedSide==1);
 reset();requestedSide=0;handDistance[0]=.19f;handDistance[1]=.01f;
 CK(SelectActual(true)&&selectedSide==0);
 reset();requestedSide=1;handDistance[0]=.01f;handDistance[1]=.30f;
 CK(!SelectActual(true)&&selectedSide==-1);
 CK(selectionRefusal=="REQUESTING_HAND_NOT_QUALIFIED");
 reset();requestedSide=0;handDistance[0]=.30f;handDistance[1]=.01f;
 CK(!SelectActual(true));
 reset();requestedSide=1;freeHands=1;CK(!SelectActual(true));
 reset();requestedSide=0;trackedHands=2;CK(!SelectActual(true));
 reset();handDistance[0]=.02f;handDistance[1]=.01f;CK(SelectActual(true)&&selectedSide==1);
 reset();requestedSide=1;handDistance[0]=.01f;handDistance[1]=.02f;
 CK(SelectActual(false)&&selectedSide==0); // legacy F5 has no new request policy
 reset();requestedSide=1;prop5916::enabled=false;requestedPose[0]=requestedPose[1]=false;
 CK(SelectActual(true)&&selectedSide==1&&!usedPhysics);
 reset();requestedSide=0;prop5916::enabled=false;requestedPose[0]=requestedPose[1]=false;
 CK(SelectActual(true)&&selectedSide==0&&!usedPhysics);
 reset();requestedSide=1;requestedPose[1]=false;CK(!SelectActual(true));
 std::printf("actual BuildStagedPickup596 hand-selection block: %u checks PASS; native/pose leaves substituted\n",checks);
 return 0;
}
'''


def extract(text, marker):
    start = text.index(marker)
    end = text.index('{', start) + 1
    depth = 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    return text[start:end]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', required=True, type=Path)
    parser.add_argument('--out', required=True, type=Path)
    parser.add_argument('--baseline', type=Path)
    args = parser.parse_args()
    source = args.source.resolve()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    path = source / 'src/vrik/staged_pickup596c2.inl'
    function = extract(path.read_text(), 'static std::uint32_t SmallPickupAdmissionR24(')
    cpp = out / 'native_admission_actual.cpp'
    cpp.write_text(PRELUDE + function + TAIL)
    report = {'source': str(path), 'source_sha256': hashlib.sha256(path.read_bytes()).hexdigest(),
              'function_sha256': hashlib.sha256(function.encode()).hexdigest(),
              'full_translation_unit': False, 'native_leaves': 'substituted',
              'sanitizers': 'not run', 'runs': []}
    with tempfile.TemporaryDirectory(prefix='kcd2vr_pickup_admission_') as temp:
        for name, flags in [('optimized', ['-O2'])]:
            binary = Path(temp) / name
            command = ['g++', '-std=c++17', *flags, '-I'+str(source/'src'), str(cpp), '-o', str(binary)]
            compiled = subprocess.run(command, capture_output=True, text=True)
            if compiled.returncode:
                raise RuntimeError(compiled.stderr)
            environment = os.environ.copy()
            environment['ASAN_OPTIONS'] = 'detect_leaks=0'
            run = subprocess.run([str(binary)], capture_output=True, text=True, env=environment)
            report['runs'].append({'name': name, 'compiled': True, 'passed': run.returncode == 0,
                                   'stdout': run.stdout, 'stderr': run.stderr})
        for label, tree in [('current',source)]+([('baseline',args.baseline.resolve())] if args.baseline else []):
            value=(tree/'src/vrik/staged_pickup596c2.inl').read_text()
            for kind, code in [('admission',PRELUDE+extract(value,'static std::uint32_t SmallPickupAdmissionR24(')+TAIL),
                               ('selection',PRELUDE+selection(value)+SELECTION_TAIL)]:
                if label=='current' and kind=='admission':continue
                test=out/(label+'_'+kind+'.cpp');test.write_text(code)
                binary=Path(temp)/(label+'_'+kind)
                compiled=subprocess.run(['g++','-std=c++17','-O2','-I'+str(source/'src'),str(test),'-o',str(binary)],capture_output=True,text=True)
                if compiled.returncode:raise RuntimeError(compiled.stderr)
                result=subprocess.run([str(binary)],capture_output=True,text=True)
                report['runs'].append({'name':label+'_'+kind,'compiled':True,
                    'expected_failure':label=='baseline','passed':(result.returncode!=0 if label=='baseline' else result.returncode==0),
                    'stdout':result.stdout,'stderr':result.stderr})
    report['all_passed'] = all(r['passed'] for r in report['runs'])
    (out / 'NATIVE_ADMISSION_RESULT.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(report, indent=2))
    return 0 if report['all_passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
