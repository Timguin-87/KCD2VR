"""Read-only A34 composition proof. Extract production functions verbatim.

Native/memory/system leaves are substituted; this is not game execution.
"""
from pathlib import Path
import hashlib, json

import argparse
args=argparse.ArgumentParser()
args.add_argument('--out',type=Path,required=True)
args=args.parse_args()
OUT=args.out.resolve();OUT.mkdir(parents=True,exist_ok=True)
ACTIVE=Path(__file__).resolve().parents[1]
DONOR=ACTIVE
proof={'native_leaves':'SUBSTITUTED','game_executed':False,'extractions':[]}
def sha(data): return hashlib.sha256(data).hexdigest()
def read(p): return p.read_text(encoding='utf-8-sig')
def extract(p, start, end='\n}'):
    text=read(p); a=text.index(start); b=text.index(end,a)+len(end)
    result=text[a:b]
    proof['extractions'].append({'path':str(p),'file_sha256':sha(p.read_bytes()),
        'line':text[:a].count('\n')+1, 'fragment_sha256':sha(result.encode()), 'start':start})
    return result
action=DONOR/'src/action_time_owner59c11.cpp'
anim=DONOR/'src/animation_filter.cpp'
runtime=DONOR/'src/subsystems/interactions/runtime/native_passenger_a34.inl'
constant_text=read(action)
constants=constant_text[constant_text.index('constexpr std::uint32_t kTs'):constant_text.index('// ---- receipts')]
constants=constants[:constants.index('std::atomic<void*>') if 'std::atomic<void*>' in constants else len(constants)]
# Only the exact field constants block. No second copy of policy is introduced.
constants=constant_text[constant_text.index('constexpr std::uint32_t kTs'):constant_text.index('constexpr std::ptrdiff_t kEvTime')]
import re
constants+='\n'+re.search(r'constexpr std::ptrdiff_t kCharSkelAnim=.*?;',constant_text).group(0)+'\n'
constants+=re.search(r'constexpr unsigned kMaxScopes=.*?;',constant_text).group(0)+'\n'
parts=[extract(action,'bool FragmentCrc19c4('),extract(action,'void* HenryChar()'),
       extract(action,'std::uint32_t HenryEntityId18()'),extract(action,'bool IsHenryController('),
       extract(action,'struct CurrentRoot57 {','\n};'),extract(action,'static bool ReadCurrentRootIdentity57('),
       extract(ACTIVE/'src/native_grasp59c13.inl','static void* HenryCharacter18()')]
observer=read(runtime)
proof['extractions'].append({'path':str(runtime),'file_sha256':sha(runtime.read_bytes()),'line':1,'start':'entire file'})
update=extract(action,'void __fastcall UpdateDetour(')
body=[extract(anim,'static bool NativePassengerBodyA34('),
      extract(anim,'static bool InspectNativeBody112('),extract(anim,'bool NativeBodyRequired112(')]
renderer=read(ACTIVE/'src/frik_pose_probe.cpp')
ra=renderer.index('    bool committed=false;',renderer.index('void __fastcall SkinningTransformCoreDetour'))
rb=renderer.index('    if(transaction.traceH29',ra)
render_gate=renderer[ra:rb]
proof['extractions'].append({'path':str(ACTIVE/'src/frik_pose_probe.cpp'),
    'file_sha256':sha((ACTIVE/'src/frik_pose_probe.cpp').read_bytes()),'line':renderer[:ra].count('\n')+1,
    'fragment_sha256':sha(render_gate.encode()),'start':'bool committed=false; through BeginCoherent gate'})

pre=r'''
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <array>
#include <utility>
#include <algorithm>
#include <cmath>
#include "subsystems/interactions/native_passenger_a34.h"
#include "vrik/vr_combat_ownership111.h"
#include "vrik/body_context115.h"
#include "vrik/left_holding_b2.h"
#include "vrik/body_context126.h"
namespace host {
std::uint64_t now=1000; unsigned thread=1; bool retail=true, transaction=true;
int nativeCalls=0, observeArms=0, doorTicks=0, beginCalls=0, retireCalls=0, txDepth=0;
std::string decision;
std::vector<std::pair<std::uintptr_t,std::size_t>> regions;
template<std::size_t N>struct Mem{alignas(16) unsigned char b[N]{};Mem(){regions.push_back({reinterpret_cast<std::uintptr_t>(b),N});} operator void*(){return b;}};
bool copy(const void* p,void* out,std::size_t n){const auto q=reinterpret_cast<std::uintptr_t>(p);
 for(auto [start,len]:regions)if(q>=start&&q-start<=len&&n<=len-(q-start)){std::memcpy(out,p,n);return true;}return false;}
template<class T>T rd(const void* p,std::ptrdiff_t off,T fallback){T out{};return p&&copy(static_cast<const unsigned char*>(p)+off,&out,sizeof(out))?out:fallback;}
template<class T>void wr(void* p,std::ptrdiff_t off,T value){std::memcpy(static_cast<unsigned char*>(p)+off,&value,sizeof(value));}
void (*nativeEffect)()=nullptr;
void Native(void*,float){if(txDepth)std::abort();++nativeCalls;if(nativeEffect)nativeEffect();}
int checks=0;
void check(bool value,const char* name){++checks;if(!value){std::printf("FAIL %s\n",name);std::exit(2);}}
}
std::uint64_t GetTickCount64(){return host::now;}
unsigned GetCurrentThreadId(){return host::thread;}
std::uintptr_t WhBase(){return 0x180000000ull;}
namespace kcd2vr {
namespace retail_build112 {bool Ready(std::uintptr_t){return host::retail;}}
namespace diagnostic_trace {void Write(const char*){}}
namespace frik_pose_probe {std::uint64_t epoch=7;int census=3,weaponMode=3;void* manager=nullptr;
 std::uint64_t PoseEpochH24(){return epoch;}int CombatCensusMode(){return census;}int WeaponControlMode(){return weaponMode;}
 void* HenryItemAttachManager59c15(){return manager;}void TickNativeDoor19c4(){++host::doorTicks;}}
namespace door_use46 {struct Intent{};bool read(Intent&,std::uint64_t){return false;}}
namespace semi_smith46 {std::atomic<bool>clockInterested{false};}
namespace activity_input163 {struct W{std::atomic<bool>active[2]{};}witness175;}
namespace combat_rules178r10 {std::atomic<int>f5Rung{0};}
namespace workstation178r20c1 {std::atomic<bool>experimentArmed36{false};}
namespace subsystems::alchemy::return67 {std::atomic<bool>interested{false};}
namespace subsystems::alchemy::mortar69 {std::atomic<bool>interested{false};}
namespace kick26 {std::atomic<std::uint64_t>nativeClipStart{0};}
namespace diagnostics {
namespace lab {enum class Key{F3};unsigned policy=0;bool reference=false;unsigned Get(Key){return policy;}bool NativeReference(){return reference;}}
namespace combat_capture {bool active(std::uint64_t){return false;}enum class Kind{State};struct Event{Kind kind{};std::uint64_t tick{};std::uintptr_t actor{};int id{},layer{};unsigned flags{};};void name(Event&,const char*){}void push(Event&){} }
}
namespace animation_filter {
std::atomic<void*>g_player_skeleton_anim{nullptr};std::atomic<bool>g_playback_layout_ready6690{true},nativeKickActive175{false},nativeBodyOwned112{false};
std::atomic<std::uint64_t>combatPoseTick112{0};
void* PlayerSkeletonAnim(){return g_player_skeleton_anim.load();}
}
}
namespace owner {
using namespace kcd2vr;
std::atomic<std::uintptr_t>g_base{0x180000000ull};std::atomic<void*>g_henry_ctrl{nullptr};
std::atomic<unsigned>g_owner_thread13{1};
template<class T>T Rd(const void* p,std::ptrdiff_t off,T fallback){return host::rd(p,off,fallback);}
bool SehCopy(const void* p,void* out,std::size_t n){return host::copy(p,out,n);}
bool OnOwnerThread13(){return g_owner_thread13.load()==GetCurrentThreadId();}
'''
owner_stubs=r'''
enum{Off=0,Idle=0};std::atomic<int>g_mode{0},g_state_pub{0};
struct {int state=0;}g_owner;
std::atomic<bool>g_alchemy_pending19c4{false},g_semi_pending46{false},g_semi_smith_pending46{false};
std::atomic<void*>g_tapped_skel{nullptr};
namespace weapon_release_t1 {int Mode(){return 0;}}
namespace supply_t2 {std::atomic<bool>enabled{false};}
namespace semi46 {std::atomic<bool>hooksReady{false};}
bool DiagOn(){return false;}bool DropWorkPendingT9(){return false;}bool NativeGraspEnabled(){return false;}
bool PendingPickupObserved63(std::uint64_t){return false;}bool PickupClockControllerH23(void*){return false;}
bool RestorePickupClipH23(){return true;}void TickPickupClipH23(std::uint64_t){}
struct OwnerTransaction19c4 {bool acquired=host::transaction;OwnerTransaction19c4(){if(acquired)++host::txDepth;}~OwnerTransaction19c4(){if(acquired)--host::txDepth;}operator bool()const{return acquired;}};
void RemoveTap(){}void FinishOwner(const char*){g_owner.state=0;}void ServiceWeaponDropT9(){}
void ObserveRootAndArm55(void*,std::uint64_t){++host::observeArms;}void AdoptPendingPickupAfterUpdateH5(void*,std::uint64_t){}void EnsureTap(){}void ScopeReceipts(void*,std::uint64_t){}void TickOwner(void*,std::uint64_t){}
void TickAlchemyStage19c4(void*,std::uint64_t){}void TickGestureAlchemyReturn67(void*,std::uint64_t){}
void TickMortarStop69(void*,std::uint64_t){}void TickSemiAlchemy46(void*,float,std::uint64_t){}
void TickSemiSmithClock46(void*,float,std::uint64_t){}
void (*g_update_original)(void*,float)=host::Native;
'''
anim_stubs=r'''
namespace kcd2vr::animation_filter {
using namespace kcd2vr;
constexpr std::uintptr_t kSkeletonAnimOffset=0x110,kCSkeletonAnimVftRva=0x3A711E0;
namespace hookcost178r22 {enum{AnimationBody};struct Scope{explicit Scope(int){}};}
bool MatchVft(void* p,std::uintptr_t want){return host::rd<std::uintptr_t>(p,0,0)==want;}
template<class T>bool Read(const void* p,T& out){return host::copy(p,&out,sizeof(out));}
bool NameForId6689(void*,int,char*,std::size_t){return false;}
bool BodyDecision115(bool native,const char* reason,int=-1,int=-1,const char*="-"){host::decision=reason;return native;}
'''
render_stubs=r'''
bool PlayerNativeBodyRequiredForMotion122(){return InspectNativeBody112(g_player_skeleton_anim.load());}
}
namespace render {
using namespace kcd2vr;
std::atomic<bool>g_phys_abort6698{false};std::atomic<std::uint64_t>g_phys_ready_sample6698{0};
struct Holsters{}g_phys_holsters6698;
struct Transaction{};struct Snapshot{std::uint64_t tick=1000;}poseSnapshot;
struct PerfScope101R2{explicit PerfScope101R2(int){}};
void PhysRetireMotion30c1(const char*,int){++host::retireCalls;}
bool BeginCoherent6689(Transaction&,void*,void*,void*,void*,unsigned,int,int,int,const Snapshot&){++host::beginCalls;return true;}
bool ActualCurrentRenderGate(void* skinAnim){
Transaction transaction;void* pose=nullptr;void* owner=nullptr;void* skinChar=nullptr;
unsigned count=500;int rig=1,b1fn=1,census=frik_pose_probe::census,f12=3;
bool rigOk=true,rawOk=true,haveSnapshot=true;poseSnapshot.tick=GetTickCount64();
'''
main=r'''
using namespace kcd2vr;
host::Mem<0x200> ctrl,scopes,contexts,context,definition,fragments,entries,action;
host::Mem<0x800> character,otherCharacter;
host::Mem<0xa00> player;host::Mem<0x100>manager;
void install(){using namespace host;using namespace owner;
 const auto base=WhBase();
 wr<std::uintptr_t>(ctrl,0,base+kCtrlVft);wr<std::uint32_t>(ctrl,kCtrlEntityId,42);
 wr<void*>(ctrl,kCtrlScopeContexts,contexts);wr<void*>(contexts,kScopeContextChar,character);
 wr<unsigned>(ctrl,kCtrlScopeCount,1);wr<void*>(ctrl,kCtrlScopes,scopes);wr<void*>(ctrl,kCtrlContext,context);
 wr<void*>(context,0,definition);wr<void*>(definition,0x18,fragments);
 wr<const unsigned char*>(fragments,8,static_cast<unsigned char*>(static_cast<void*>(entries))+4);
 wr<int>(entries,0,1);wr<std::uint32_t>(entries,4+0x10,0xeed2e729u);
 wr<void*>(scopes,kScopeAction,action);wr<int>(scopes,kScopeLayer,0);
 wr<std::uintptr_t>(action,0,base+0x3a40000);wr<void*>(action,kActRootScope,scopes);wr<void*>(action,kActContext,context);
 wr<int>(action,kActFragId,0);wr<int>(action,kActStatus,2);wr<std::uint32_t>(action,kActToken,9);
 auto* anim=static_cast<unsigned char*>(static_cast<void*>(character))+0x110;
 wr<std::uintptr_t>(anim,0,base+0x3A711E0);wr<void*>(anim,0x4E8,character);
 animation_filter::g_player_skeleton_anim=anim;
 wr<std::uintptr_t>(manager,0,base+0x3EC9958);wr<void*>(manager,0x38,player);
 wr<std::uintptr_t>(player,0,base+0x3AA4B98);wr<void*>(player,0x9D0,manager);wr<std::uint32_t>(player,0x30,42);
 frik_pose_probe::manager=manager;frik_pose_probe::census=3;frik_pose_probe::epoch=7;
 g_henry_ctrl=nullptr;g_owner_thread13=1;thread=1;now=1000;transaction=true;retail=true;nativeEffect=nullptr;
 animation_filter::nativeBodyOwned112=false;animation_filter::g_playback_layout_ready6690=true;
 diagnostics::lab::policy=0;diagnostics::lab::reference=false;
 native_passenger_a34::witness.publish({});
}
bool observed(){return native_passenger_a34::current(reinterpret_cast<std::uintptr_t>(static_cast<void*>(character)),frik_pose_probe::epoch,host::now);}
void Tick(){owner::UpdateDetour(ctrl,1.f/90.f);}
void SetAction(){host::wr<void*>(scopes,owner::kScopeAction,action);}
int main(){using namespace host;namespace a=native_passenger_a34;
 install();auto* anim=animation_filter::PlayerSkeletonAnim();
 Tick();check(nativeCalls==1&&txDepth==0,"native Update exactly once outside transaction");
 check(owner::g_henry_ctrl.load()==static_cast<void*>(ctrl)&&observed(),"quiet VRIK discovers native Henry and passenger");
 a::Root first{};a::witness.read(first);
 check(first.action==reinterpret_cast<std::uintptr_t>(static_cast<void*>(action))&&first.token==9&&first.layer==0,"actual installed root copied");
 for(unsigned policy: {0u,1u,2u,3u}){diagnostics::lab::policy=policy;beginCalls=retireCalls=0;
  check(!render::ActualCurrentRenderGate(anim)&&beginCalls==0&&retireCalls==1,"current renderer yields before all four combat policies");
  check(animation_filter::PlayerNativeBodyRequiredForMotion122(),"independent motion inspector yields");}
 diagnostics::lab::policy=0;
 for(auto crc:{0xeed2e729u,0x591f4107u,0x735721aau,0x04ab0eefu,0xc630055bu,0xd860d990u}){
  wr<std::uint32_t>(entries,4+0x10,crc);Tick();check(observed(),"six exact fragment graph classifications");}
 wr<std::uint32_t>(entries,4+0x10,0xeed2e729u);Tick();a::Root before{};a::witness.read(before);
 wr<std::uint32_t>(action,owner::kActToken,10);Tick();a::Root restarted{};a::witness.read(restarted);
 check(restarted.serial>before.serial&&restarted.token==10,"synchronous action token restart does not inherit serial");
 // Native Update can install the action. No queue/Use/input seed is supplied.
 wr<void*>(scopes,owner::kScopeAction,nullptr);nativeEffect=SetAction;Tick();nativeEffect=nullptr;
 check(observed(),"post-original observation sees original-installed root");
 wr<void*>(scopes,owner::kScopeAction,nullptr);Tick();check(!observed(),"empty root terminal clears witness");
 beginCalls=0;check(render::ActualCurrentRenderGate(anim)&&beginCalls==1,"terminal root restores existing tracked gate");
 SetAction();wr<int>(action,owner::kActStatus,3);Tick();check(!observed(),"retiring status is not presentation authority");
 wr<int>(action,owner::kActStatus,2);wr<int>(scopes,owner::kScopeLayer,1);Tick();check(!observed(),"non-FullBody layer rejected");
 wr<int>(scopes,owner::kScopeLayer,0);wr<void*>(action,owner::kActContext,definition);Tick();check(!observed(),"action context replacement rejected");
 wr<void*>(action,owner::kActContext,context);wr<void*>(action,owner::kActRootScope,contexts);Tick();check(!observed(),"action root-scope replacement rejected");
 wr<void*>(action,owner::kActRootScope,scopes);Tick();check(observed(),"current root reacquired after exact graph restoration");
 // Menu/real cutscene native-reference ownership is not changed by A34.
 diagnostics::lab::reference=true;beginCalls=0;
 check(!render::ActualCurrentRenderGate(anim)&&beginCalls==0,"native-reference ownership preserved during passenger");
 diagnostics::lab::reference=false;
 const auto oldEpoch=frik_pose_probe::epoch;++frik_pose_probe::epoch;
 check(!a::current(reinterpret_cast<std::uintptr_t>(static_cast<void*>(character)),frik_pose_probe::epoch,now),"body epoch replacement rejects old witness");
 Tick();check(observed(),"new current body epoch re-observed without XR input");
 check(!a::current(reinterpret_cast<std::uintptr_t>(static_cast<void*>(otherCharacter)),frik_pose_probe::epoch,now),"different current character cannot consume old passenger");
 transaction=false;now+=81;const int prior=nativeCalls;Tick();
 check(nativeCalls==prior+1&&!observed(),"contended observation still calls original and expires authority");
 transaction=true;Tick();check(observed(),"owner reacquires after contention");
 retail=false;Tick();check(!observed(),"exact retail mismatch cannot publish authority");retail=true;
 thread=2;now+=81;Tick();check(!observed(),"wrong owner thread cannot renew authority");thread=1;Tick();
 check(observed(),"owner refresh ignores old XR/gameplay acquisition after native yield");
 const auto calls=nativeCalls;owner::UpdateDetour(nullptr,0.01f);
 check(nativeCalls==calls+1&&observed(),"null/NPC route calls original without cancelling Henry witness");
 // Scope/character mismatch: copied old skinning character can still be used
 // by the entity-ID based controller identity. Record this read-only limitation.
 wr<void*>(contexts,owner::kScopeContextChar,otherCharacter);Tick();
 check(!observed(),"FullBody current character must match skinning body even with matching player ID");
 wr<void*>(contexts,owner::kScopeContextChar,character);
 frik_pose_probe::census=0;frik_pose_probe::weaponMode=2;Tick();check(observed(),"controller-only mode refreshes passenger independently of VRIK");
 std::printf("A34 independent production composition %d checks PASS; native/system leaves substituted; game/headset NOT_RUN\n",checks);
}
'''
content=pre+constants+'\n'+'\n'.join(parts)+'\n'+observer+'\n'+owner_stubs+'\n'+update+'\n}\n'+anim_stubs+'\n'+'\n'.join(body)+'\n'+render_stubs+render_gate+'\nreturn committed;\n}\n}\n'+main
# C++ lexical separation for one default argument in a substituted diagnostic leaf.
content=content.replace('const char*="-"','const char* = "-"')
(OUT/'a34_current_composition.cpp').write_text(content,encoding='utf-8')
proof['fixture_sha256']=sha(content.encode())
(OUT/'EXTRACTION.json').write_text(json.dumps(proof,indent=2),encoding='utf-8')
print(json.dumps({'generated':str(OUT/'a34_current_composition.cpp'),'extractions':len(proof['extractions']),
                  'fixture_sha256':proof['fixture_sha256']},indent=2))
