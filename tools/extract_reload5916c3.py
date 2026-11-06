import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1];s=read_source(r/'src/vrik/staged_reload596c2.inl')
def span(a,b):return s[s.index(a):s.index(b,s.index(a))]
pieces=[span('struct ManualBow596 {','static void __fastcall DeferredReload596'),span('struct ManualItems596 {','static void* ManualEquipmentPtr59c9'),span('static void ManualNote596(','// Native leaf isolated'),span('static bool ManualRemove596(','static bool ManualCreate596('),s[s.index('static void ManualBowTick596()'):]]
# Definitions and complete policy functions are production verbatim. Only game
# leaves and owner/sample providers below are explicit substitutes.
pre=r'''#include <atomic>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include "vrik/physical_weapon.h"
#include "vrik/staged_hold596c2.h"
#include "core/snapshot_mailbox.h"
namespace pm=kcd2vr::pose_core;namespace ph=kcd2vr::physical;namespace stage596=kcd2vr::staged596c2;
using V3=pm::Vec3;using Q4=pm::Quat;
static V3 Sub(V3 a,V3 b){return a-b;} static double Dot(V3 a,V3 b){return pm::dot(a,b);}
static V3 Normalize(V3 a){auto u=pm::unit(a);return u?*u:V3{};}static V3 QRotateV(Q4 q,V3 p){return pm::rotate(q,p);}
static std::uint64_t now=1000;static std::uint64_t GetTickCount64(){return now;}
static std::vector<std::string> notes;namespace diagnostic_trace {static void Write(const char* s){notes.emplace_back(s);}}
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1};
static bool StagedBowEnabled59();
'''
mid=r'''
static bool experiment=true,defaultStaged59=false,equipmentOk=true,itemsOk=true,trackingOk=true,cleanupOk=true,playerOk=true;
static bool handAttached=true,nockAttached=true,nockSucceeds=true,nockPreflightOk40=true;
static unsigned removes=0,creates=0,nocks=0;
static int managerMemory=0,playerMemory=0,entityMemory=0;
static std::atomic<void*> g_henry_item_attach_manager{&managerMemory};
static std::map<unsigned,void*> entities;
static void* EntityById6694(unsigned id){return entities.count(id)?entities[id]:nullptr;}
static bool ManualPlayer596(void* p){return playerOk&&p==&playerMemory;}
static bool Read(const unsigned char*,void*& out){out=&playerMemory;return true;}
static bool StagedBowEnabled59(){return experiment||defaultStaged59;}
struct PhysFrame6698 {ph::Input input{};ph::Output output{};ph::Profile profiles[2]{};std::uint64_t request=1;};
static void ManualArrowVisualReceipt59(const ManualBow596&,const PhysFrame6698&,unsigned=0) noexcept {}
static void ManualArrowWithdraw64() noexcept {}
static void ManualArrowPublish64(const ManualBow596&,const PhysFrame6698&,std::uint64_t,std::uint64_t) noexcept {}
static void ManualArrowCue64(const PhysFrame6698&) noexcept {}
// Explicit presentation leaf substitute; actual socket/helper math is exercised
// by manual_arrow64_test including the actual production presentation code.
static double ManualArrowNockGap64(const ManualBow596&,const PhysFrame6698& f) noexcept {
 return pm::length(f.input.hands[0].p-ph::point(f.output.weapons[1],f.profiles[1].grip[0].p));
}
struct TrackingSnapshot {std::uint64_t sequence=1,tick=1000;V3 head{},right{.2,0,.2};Q4 hq{};bool allowed=true,head_valid=true,left_valid=true,right_valid=true,head_held=false,right_held=false,left_held=false;float right_grip=0;};
static PhysFrame6698 frame;static TrackingSnapshot track;static ManualItems596 items;
static bool advanceRecheck26=false;static unsigned equipmentCalls26=0,advanceTickMs26=5;
static bool ManualEquipment596(void*,PhysFrame6698& f){f=frame;if(advanceRecheck26&&++equipmentCalls26>=2){f.input.sample+=1;f.input.tick+=advanceTickMs26;}return StagedBowEnabled59()&&equipmentOk;}
static bool ManualItemsRead596(void*,ManualItems596& i){i=items;return itemsOk;}
static bool ReadTrackingSnapshot(TrackingSnapshot& t){t=track;return trackingOk;}
static bool ManualRemoveCall5916c3(void*,std::uint32_t& hand,int){++removes;if(cleanupOk){entities.erase(hand);hand=0;return true;}return false;}
static bool ManualCreate596(ManualBow596& m){++creates;m.hand=7;m.entity=&entityMemory;m.side=0;entities[7]=m.entity;return true;}
static bool ManualHandAttached19c4r4(const ManualBow596&){return handAttached;}
static bool ManualArrowVisible42(const ManualBow596&,bool* restored=nullptr,unsigned* layers=nullptr){if(restored)*restored=false;if(layers)*layers=0;return handAttached;}
static bool ManualNockAttached19c4r4(const ManualItems596& i){return nockAttached&&i.loaded;}
static bool ManualNock596(ManualBow596& m){if(!nockPreflightOk40)return false;++nocks;if(!ManualRemove596(m))return false;items.loaded=nockSucceeds;items.nockedKey=nockSucceeds?items.ammoKey:0;return nockSucceeds;}
'''
# NativeNock stub calls actual policy cleanup, so declare its signature first.
out=pre+pieces[0]+pieces[1]+'\nstatic bool ManualRemove596(ManualBow596& m) noexcept;\n'+mid+'\n'+''.join(pieces[2:])
(r/'tests/generated_reload5916c3.inc').write_text(out)
(r/'tests/reload5916c3_extraction.json').write_text(json.dumps({'source':'src/vrik/staged_reload596c2.inl','complete_policy_sha256':[hashlib.sha256(p.encode()).hexdigest() for p in pieces],'native_leaves':'equipment/player/items/tracking/read/create/nock/remove-call mocked; production cleanup and Tick unchanged'},indent=2))
