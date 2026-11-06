// Actual D52 context/reader/receipt, with explicit in-memory native leaves.
// These tests establish code behavior, not game admission or headset contact.
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "../src/vrik/contact_occupancy_d52.h"
#include "../src/subsystems/ranged_reload/crossbow_native_xr34.h"
#include "../src/core/snapshot_mailbox.h"
#include "../src/diagnostics/capture168.h"
#include "../src/subsystems/physics/runtime/dialogue_hand_presence_h24.h"
namespace co=kcd2vr::contact_occupancy_d52;
namespace diagnostics=kcd2vr::diagnostics;
static std::uint64_t clockD52=1000;
static std::uint64_t GetTickCount64(){return clockD52;}
static std::uintptr_t WhBase(){return 0x100000000ull;}
namespace retail_build112 {static bool ready=true;static bool Ready(std::uintptr_t){return ready;}}
static std::atomic<bool> g_gameplay_allowed{false},g_dialogue_body178r9{true},g_pose_restore_fault6689{false},g_ctx_dialogue{false};
static std::atomic<std::uint64_t> g_pose_epoch6689{1},g_pose_policy_generation57{2},g_weapon_request_epoch6694{3},g_tool_context19c4r2{0};
static std::atomic<void*> g_committed_character6689{nullptr},g_henry_item_attach_manager{nullptr},g_last_player_entity{nullptr};
namespace kcd2vr::tracking59c6 {static std::atomic<std::uint64_t> epoch{4};}
namespace kcd2vr::minigame_hands45 {static std::atomic<std::uint64_t> modeEpoch67{5};}
namespace kcd2vr::activity_input163 {
enum class Kind175:unsigned {Alchemy,Smithing};
struct Witness {std::atomic<bool> ready{true};std::atomic<unsigned> transitionDepth175{0};std::atomic<std::uint64_t> lifetime175{6};std::atomic<std::uintptr_t> active[2]{};} witness175;
}
namespace kcd2vr::diagnostics::lab {static bool reference=false;static bool NativeReference(){return reference;}}
static bool head=true,stationAdmitted=true;
static bool TrackingHeadUsable111(std::uint64_t){return head;}
static const char* WorkstationToolReason19c4r2(std::uint64_t){return stationAdmitted?nullptr:"NO_STATION_POSE";}
static kcd2vr::core::SnapshotMailbox<kcd2vr::physics::dialogue_hand_h24::Witness> g_dialogue_hand_presence_h24;
static std::atomic<bool> g_dialogue_hand_presence_enabled_h24{false};
static std::vector<std::string> lines;
namespace diagnostic_trace {static void Write(const char* s){lines.emplace_back(s);}}
static int legacyReads=0,nativeReads=0,lookups=0;static int legacy[2]{-1,-1};
namespace kcd2vr::frik_pose_probe {static int ExactHandItemState(int s){++legacyReads;return legacy[s];}}
static std::array<unsigned char,0x100> manager{},attachments[2]{};
static std::array<unsigned char,0x1000> player{},character{};
static const char names[2][13]{"entity_right","entity_left"};
static std::uintptr_t failAt=0,mutateAt=0;static int mutation=0;
struct Region{const void* p;std::size_t n;};
static std::vector<Region> regions{{manager.data(),manager.size()},{player.data(),player.size()},
 {character.data(),character.size()},{attachments[0].data(),attachments[0].size()},
 {attachments[1].data(),attachments[1].size()},{names,sizeof(names)}};
template<class T>static bool Read(const void* p,T& out){
 ++nativeReads;const auto a=reinterpret_cast<std::uintptr_t>(p);if(a==failAt)return false;
 if(a==mutateAt){mutateAt=0;if(mutation==1)++g_pose_epoch6689;else if(mutation==2)++kcd2vr::minigame_hands45::modeEpoch67;
  else if(mutation==3)++kcd2vr::activity_input163::witness175.lifetime175;else if(mutation==4)++g_weapon_request_epoch6694;
  else if(mutation==5)++kcd2vr::tracking59c6::epoch;}
 for(auto r:regions){const auto start=reinterpret_cast<std::uintptr_t>(r.p);if(a>=start&&a-start<=r.n&&sizeof(T)<=r.n-(a-start)){std::memcpy(&out,p,sizeof(T));return true;}}
 return false;
}
template<class T>static void put(void* p,T v){std::memcpy(p,&v,sizeof(v));}
static std::uint64_t keys[2]{};static bool keyKnown[2]{true,true},missing[2]{};static int changeBinding=-1,keyChange=-1,keyCalls[2]{};
static bool NativeHandKey6697(void*,int s,std::uint64_t& k){++keyCalls[s];if(keyChange==s&&keyCalls[s]>1)k=77;else k=keys[s];return keyKnown[s];}
static bool PhysEmpty6698(std::uint64_t k){return !k||k==~std::uint64_t(0);}
static void* CharAttachment59c15(void*,const char* name){++lookups;int s=std::strcmp(name,"entity_right")==0?0:1;
 if(s==1&&changeBinding==0)put(attachments[0].data()+0x20,std::uintptr_t(0xaaa));
 return missing[s]?nullptr:attachments[s].data();}
// Exact native passenger admission is a leaf here; the reload source/clock
// suite executes that production gate separately. D52's owner, native slot
// observation, legacy exclusion and rejoin execute without replacement.
namespace xcXR34=kcd2vr::crossbow_native_xr34;
static xcXR34::Passenger reloadPassengerR95{};static bool reloadAvailableR95=false;
static bool ReadReloadPassengerXR34(xcXR34::Passenger& p,std::uintptr_t c,std::uint64_t epoch,std::uint64_t now) noexcept {
 p=reloadPassengerR95;return reloadAvailableR95&&p.valid&&p.owner.serial==xcXR34::activeNativeLease.load()&&
    p.frame.character==c&&p.frame.body==epoch&&p.frame.body==g_pose_epoch6689.load()&&
    p.frame.tracking==kcd2vr::tracking59c6::epoch.load()&&p.frame.request==g_weapon_request_epoch6694.load()&&
    p.frame.tick&&now>=p.frame.tick&&now-p.frame.tick<=80&&head&&!g_pose_restore_fault6689.load();
}
#include "../src/vrik/contact_occupancy_d52.inl"
static unsigned count=0;static void check(bool b,const char* m){++count;if(!b){std::fprintf(stderr,"FAIL %u %s\n",count,m);std::exit(1);}}
static void reset(){
 manager.fill(0);player.fill(0);character.fill(0);for(auto& a:attachments)a.fill(0);
 clockD52=1000;reloadAvailableR95=false;xcXR34::activeNativeLease=0;reloadPassengerR95={};g_gameplay_allowed=false;g_dialogue_body178r9=true;g_ctx_dialogue=false;g_pose_restore_fault6689=false;
 g_pose_epoch6689=1;g_pose_policy_generation57=2;g_weapon_request_epoch6694=3;kcd2vr::tracking59c6::epoch=4;kcd2vr::minigame_hands45::modeEpoch67=5;
 g_tool_context19c4r2=(clockD52<<8)|9;g_committed_character6689=character.data();g_henry_item_attach_manager=manager.data();g_last_player_entity=player.data()+0x100;
 auto& w=kcd2vr::activity_input163::witness175;w.ready=true;w.transitionDepth175=0;w.lifetime175=6;w.active[0]=0xabc;w.active[1]=0;
 retail_build112::ready=true;head=true;stationAdmitted=true;diagnostics::lab::reference=false;g_dialogue_hand_presence_enabled_h24=false;
 put(manager.data(),WhBase()+0x3EC9958);put(manager.data()+0x38,reinterpret_cast<std::uintptr_t>(player.data()));
 put(player.data(),WhBase()+0x3AA4B98);put(player.data()+0x9D0,reinterpret_cast<std::uintptr_t>(manager.data()));
 put(character.data(),WhBase()+0x40602F8);put(character.data()+8,WhBase()+0x4060548);put(character.data()+0x78,reinterpret_cast<std::uintptr_t>(character.data()));
 for(int s=0;s<2;++s){put(attachments[s].data(),WhBase()+0x3A657C0);put(attachments[s].data()+0x28,reinterpret_cast<std::uintptr_t>(character.data()+8));put(attachments[s].data()+0x10,reinterpret_cast<std::uintptr_t>(names[s]));keys[s]=0;keyKnown[s]=true;missing[s]=false;keyCalls[s]=0;legacy[s]=-1;}
 failAt=mutateAt=0;mutation=0;keyChange=changeBinding=-1;legacyReads=nativeReads=lookups=0;
}
static co::Observation sample(){return ReadContactOccupancyD52(character.data(),1,4,2,clockD52);}
int main(){
 reset();auto a=sample();check(a.hand[0].state==0&&a.hand[1].state==0&&legacyReads==0,"pose-only alchemy ignores expired gameplay cache");
 check(lookups==2,"two bounded attachment lookups per admitted observation");
 clockD52+=10000;g_tool_context19c4r2=(clockD52<<8)|9;a=sample();check(a.hand[0].state==0&&a.hand[1].state==0,"sustained activity remains current without stretching cache age");
 reset();keys[0]=11;a=sample();check(a.hand[0].state==1&&a.hand[0].reason==co::Reason::Weapon&&a.hand[1].state==0,"weapon in right preserves free left");
 reset();put(attachments[1].data()+0x20,std::uintptr_t(0x555));a=sample();check(a.hand[0].state==0&&a.hand[1].state==1&&a.hand[1].reason==co::Reason::EntityBinding,"non-CItem left prop is occupied");
 put(attachments[1].data()+0x20,std::uintptr_t(0));a=sample();check(a.hand[1].state==0,"release follows exact current null binding immediately");
 reset();keys[0]=~std::uint64_t(0);a=sample();check(a.hand[0].state==0,"native empty key sentinel normalized");
 reset();keyKnown[0]=false;a=sample();check(a.hand[0].state==-1&&a.hand[1].state==0,"unknown one hand does not discard known other hand");
 reset();missing[0]=true;a=sample();check(a.hand[0].state==-1&&a.hand[0].reason==co::Reason::AttachmentLookup,"missing slot is not a null binding");
 reset();failAt=reinterpret_cast<std::uintptr_t>(attachments[0].data()+0x20);a=sample();check(a.hand[0].state==-1&&a.hand[1].state==0,"binding read fault stays unknown independently");
 reset();put(attachments[0].data()+0x28,std::uintptr_t(0x777));a=sample();check(a.hand[0].state==-1,"foreign attachment owner rejected");
 reset();put(attachments[0].data()+0x10,reinterpret_cast<std::uintptr_t>(names[1]));a=sample();check(a.hand[0].state==-1,"wrong attachment name rejected");
 reset();keyChange=0;a=sample();check(a.hand[0].state==-1&&a.hand[0].reason==co::Reason::Changed,"key change during empty read never yields free hand");
 reset();changeBinding=0;a=sample();check(a.hand[0].state==-1&&a.hand[1].state==0,"first hand binding changed during other lookup cannot stay empty");
 reset();put(player.data()+0x9D0,std::uintptr_t(0x777));a=sample();check(a.hand[0].state==-1&&a.hand[1].state==-1,"Henry backlink mismatch rejects identity");
 for(int m=1;m<=5;++m){reset();mutateAt=reinterpret_cast<std::uintptr_t>(attachments[0].data()+0x20);mutation=m;a=sample();check(a.hand[0].state==-1&&a.hand[1].state==-1,"body/mode/lifetime/request/tracking race retires whole observation");}
 reset();g_gameplay_allowed=true;legacy[0]=1;legacy[1]=0;a=sample();check(a.hand[0].state==1&&a.hand[1].state==0&&legacyReads==2&&nativeReads==0&&lookups==0,"ordinary gameplay delegates unchanged without new native reads");
 reset();kcd2vr::activity_input163::witness175.active[0]=0;legacy[0]=0;a=sample();check(a.hand[0].state==0&&a.hand[1].state==-1&&legacyReads==2&&lookups==0,"unrelated contexts retain original behavior");
 for(int m=0;m<10;++m){reset();if(m==0)g_dialogue_body178r9=false;if(m==1)retail_build112::ready=false;if(m==2)head=false;if(m==3)g_pose_restore_fault6689=true;if(m==4)diagnostics::lab::reference=true;if(m==5)stationAdmitted=false;if(m==6)g_tool_context19c4r2=((clockD52-81)<<8)|9;if(m==7)g_tool_context19c4r2=(clockD52<<8)|4;if(m==8)g_tool_context19c4r2=(clockD52<<8)|16;if(m==9)kcd2vr::activity_input163::witness175.transitionDepth175=1;a=sample();check(a.hand[0].state==-1&&a.hand[1].state==-1&&lookups==0,"unadmitted presentation performs no attachment lookup");}
 reset();g_ctx_dialogue=true;kcd2vr::activity_input163::witness175.active[0]=0;g_tool_context19c4r2=(clockD52<<8)|2;
 kcd2vr::physics::dialogue_hand_h24::Witness w{};w.presentation=true;w.policy=2;w.native.known=true;w.native.active=true;w.native.tick=clockD52;w.native.instance=123;w.native.player=reinterpret_cast<std::uintptr_t>(player.data());w.native.entity=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load());w.native.camera=kcd2vr::dialogue_state59c10r2::Camera::FirstPerson;
 g_dialogue_hand_presence_h24.publish(w);g_dialogue_hand_presence_enabled_h24=true;a=sample();check(a.owner.kind==1&&a.hand[0].state==0&&a.hand[1].state==0,"fresh actual dialogue presentation witness supports free hands");
 w.native.player=777;g_dialogue_hand_presence_h24.publish(w);a=sample();check(a.hand[0].state==-1,"dialogue other player cannot borrow observation");
 w.native.player=reinterpret_cast<std::uintptr_t>(player.data());w.native.tick=clockD52-81;g_dialogue_hand_presence_h24.publish(w);a=sample();check(a.hand[0].state==-1,"expired dialogue scene rejected");
 reset();a=sample();lines.clear();TraceContactOccupancyD52(a,clockD52);check(lines.empty(),"diagnostics quiet when capture is off");
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,clockD52);TraceContactOccupancyD52(a,clockD52);check(lines.size()==1&&lines[0].find("VERIFIED_EMPTY")!=std::string::npos,"F5 records explicit source and result");
 TraceContactOccupancyD52(a,clockD52);check(lines.size()==1,"unchanged observation does not spam");
 for(unsigned n=0;n<120;++n){a.owner.scene=100+n;TraceContactOccupancyD52(a,clockD52);}check(lines.size()==96&&lines.back().find("budgetExhausted=1")!=std::string::npos,"finite edge budget terminates capture");
 // R95: the reload's own read-only occupancy survives an expired ordinary
 // gameplay mailbox, and cannot fall back to it after a native owner loss.
 for(int kind:{10,14,15}){
  reset();g_gameplay_allowed=true;legacy[0]=legacy[1]=0;keys[1]=99;
  reloadAvailableR95=true;xcXR34::activeNativeLease=77;auto& p=reloadPassengerR95;p.valid=true;p.owner.serial=77;p.owner.child=0x777;
  p.frame.character=reinterpret_cast<std::uintptr_t>(character.data());p.frame.manager=reinterpret_cast<std::uintptr_t>(manager.data());p.frame.player=reinterpret_cast<std::uintptr_t>(player.data());
  p.frame.body=1;p.frame.request=3;p.frame.tracking=4;p.frame.tick=clockD52;p.frame.primary=1;p.frame.classId=kind;p.frame.physicalD81=true;p.owner.source=p.frame;p.workingHand=0;
  a=sample();check(a.owner.kind==4&&a.owner.lifetime==77&&a.hand[0].state==0&&a.hand[1].state==1&&legacyReads==0,"reload exact free working hand; primary occupied; no gameplay cache");
  check(RejoinContactOccupancyD53(a),"reload native slots and same occurrence rejoin");
  put(attachments[0].data()+0x20,std::uintptr_t(0x999));check(!RejoinContactOccupancyD53(a),"new native tool binding cannot borrow free hand contact");
  a=sample();check(a.hand[0].state==1&&legacyReads==0,"native reload tool becomes occupied independently");
  reloadAvailableR95=false;const auto lookupsBefore=lookups;a=sample();check(a.hand[0].state==-1&&a.hand[1].state==-1&&legacyReads==0&&lookups==lookupsBefore,"lost reload owner remains unknown without stale gameplay fallback");
 }
 std::printf("contact_occupancy_d52: %u checks passed\n",count);return 0;
}
