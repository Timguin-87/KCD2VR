#include <atomic>
#include <cstdio>
#include <cstring>
#include "subsystems/throw_drop/hand_claim_t24.h"
#include "actual_fist_motion_t24.h"
#include <windows.h>
#undef near
static unsigned checks=0,failures=0;
#define CHECK(x) do{++checks;if(!(x)){++failures;std::printf("FAIL line=%d %s\n",__LINE__,#x);}}while(0)
static ULONGLONG clockT24=1000;
static ULONGLONG ClockT24(){return clockT24;}
#define GetTickCount64 ClockT24
// Actual utility implementation, not a duplicate of its button policy.
#include "subsystems/hud_ui_controls/runtime/native_vr_pad_premap.cpp"
namespace hc=kcd2vr::hand_claim_t24;
namespace ac=kcd2vr::ammo_carry_t18;
static hc::Owner current{11,22,33,44,55};
static ac::Claim claim{11,22,33,77,101,202,44,55,1,1,9,ac::Stage::Pending};
static bool HandClaimBlockedT24(int side,std::uintptr_t character,std::uint64_t body,std::uint64_t request) noexcept {
    auto owner=current;owner.character=character;owner.body=body;owner.request=request;
    return hc::competing(side,owner); // only current actor lookup is a fixture leaf
}
static bool NoClaimT24(int,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept{return false;}
static void Clear(){
    ac::hands=0;for(auto& m:ac::claims)CHECK(m.publish({}));
    current={11,22,33,44,55};claim={11,22,33,77,101,202,44,55,1,1,9,ac::Stage::Pending};
}
static void Own(int side,ac::Stage stage=ac::Stage::Held){claim.stage=stage;CHECK(ac::claims[side].publish(claim));ac::hands=1u<<side;}

// Native leaves/identity snapshots for the FULL actual map + attachment readers.
struct Pose{float q[4]{0,0,0,1},t[3]{};};
struct Stamp{std::uintptr_t actor=33;std::uint64_t epoch=44,tick=1000;};
struct CommittedFrame6695{Stamp stamp{};bool valid[2]{true,true};Pose allowed[2]{};float world[8]{};std::uintptr_t doorPlayer38=0;};
static CommittedFrame6695 packet{};
static std::atomic<int> g_census_mode{4};
static std::atomic<std::uint64_t> g_pose_epoch6689{44},g_weapon_request_epoch6694{55};
static std::atomic<void*> g_last_player_entity{nullptr},g_committed_character6689{reinterpret_cast<void*>(33)};
namespace diagnostics::lab{static bool NativeReference(){return false;}}
namespace animation_filter{static std::atomic<bool> nativeBodyOwned112{false};}
static bool CalibrationReady(){return true;}
static int ExactHandItemState(int){return 0;}
static int NativeHeadAimEquipment120R3(){return 0;}
static bool ReadCommittedPacket6695(CommittedFrame6695& p){p=packet;return true;}
static bool TrackingHandUsable111(int,std::uint64_t){return true;}
static Pose HandWorld6694(Pose p,const float*){return p;}
struct Memory{unsigned char data[3072]{};};
static Memory playerMemory,modelMemory;
static const char* handNames[2]{"entity_right","entity_left"};
static unsigned finds=0;
static std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(handNames)-0x40AC298;}
template<class T> static bool Read(const void* p,T& v){std::memcpy(&v,p,sizeof(v));return true;}
template<class T> static void Put(Memory& m,unsigned offset,T v){std::memcpy(m.data+offset,&v,sizeof(v));}
struct ManualBow596{void* player=nullptr;void* entity=nullptr;unsigned hand=9;int side=0;std::uint64_t epoch=44,request=55;};
static bool ManualPlayer596(void* p){return p==&playerMemory;}
static void* ManualFindHandAttachment19c4r4(void*,const char*){++finds;return &modelMemory;}
static bool ManualBoundEntity19c4r4(void*,unsigned,void*){return true;}
static void PrepareManual(){Put(modelMemory,0x20,static_cast<void*>(&playerMemory));Put(modelMemory,0x2C,9u);Put(playerMemory,0x1B0,static_cast<void*>(&modelMemory));finds=0;}
struct {kcd2vr::fists::Sample sample{};} f;
struct {struct {float grip[2]{.8f,.9f},trigger[2]{.6f,.7f};}sample;} frame;
struct {int side=0;std::uint64_t pendingTick=1;} n;
static float loggedGrip=0,loggedTrigger=0;
namespace fp3{enum class Stage{Pending};struct Join{};static void native(Stage,unsigned,Join,std::uint64_t,std::nullptr_t,unsigned,std::uint64_t,float grip,float trigger){loggedGrip=grip;loggedTrigger=trigger;}}
static fp3::Join FistJoinC54R3(){return {};}
struct {bool valid[2]{true,true};} contactH11;
#include "actual_consumers_t24.inc"

static void Policy(){
    Clear();CHECK(hc::observe(0,current).state==hc::State::Free);CHECK(hc::observe(1,current).state==hc::State::Free);
    for(auto stage:{ac::Stage::Pending,ac::Stage::Held,ac::Stage::Releasing,ac::Stage::Fault}){
        Clear();Own(0,stage);CHECK(hc::observe(0,current).state==hc::State::CurrentGrant);CHECK(!hc::competing(1,current));
        auto changed=current;++changed.body;++changed.request;++changed.character;
        CHECK(hc::observe(0,changed).state==hc::State::SameActorBusy);
        changed=current;++changed.manager;CHECK(hc::observe(0,changed).state==hc::State::SameActorBusy);
        changed=current;++changed.player;CHECK(hc::observe(0,changed).state==hc::State::Unknown);
        ++changed.manager;CHECK(hc::observe(0,changed).state==hc::State::ForeignActor);CHECK(!hc::competing(0,changed));
        claim.tick=1;claim.load=999;CHECK(ac::claims[0].publish(claim));CHECK(hc::competing(0,current));
    }
    Clear();claim.key=0;claim.preview=0;claim.entity=0;Own(0,ac::Stage::Pending);CHECK(hc::competing(0,current)&&!hc::competing(1,current));
    hc::Reads reads{};reads.before=reads.after=1;CHECK(hc::classify(0,current,reads).state==hc::State::Unknown);
    reads.firstKnown=reads.secondKnown=true;CHECK(hc::classify(0,current,reads).state==hc::State::Unknown);
    reads.first=reads.second=claim;CHECK(hc::classify(0,current,reads).state==hc::State::CurrentGrant);
    reads.firstKnown=false;CHECK(hc::classify(0,current,reads).state==hc::State::Unknown);
    reads.firstKnown=true;++reads.second.serial;CHECK(hc::classify(0,current,reads).state==hc::State::Unknown);
    reads.second=reads.first;reads.before=reads.after=0;CHECK(hc::classify(0,current,reads).state==hc::State::CurrentGrant);
    reads.after=1;reads.second={};CHECK(hc::classify(0,current,reads).state==hc::State::Unknown);
    CHECK(hc::observe(-1,current).state==hc::State::Unknown);CHECK(hc::competing(2,current));
}
static void ActualConsumers(){
    float pose[8]{};unsigned reason=0;
    Clear();Own(0);CHECK(ReadHeldMapLeftPoseM1(pose,&reason,nullptr,nullptr,nullptr)); // other hand remains available
    Clear();Own(1);CHECK(!ReadHeldMapLeftPoseM1(pose,&reason,nullptr,nullptr,nullptr)&&reason==9);
    ++current.player;++current.manager;CHECK(ReadHeldMapLeftPoseM1(pose,&reason,nullptr,nullptr,nullptr));
    Clear();PrepareManual();ManualBow596 m{&playerMemory,&modelMemory};CHECK(ManualHandAttached19c4r4(m)&&finds==1);
    Own(0);CHECK(!ManualHandAttached19c4r4(m)&&finds==1); // no native attachment lookup on claimed right
    Clear();PrepareManual();Own(1);CHECK(ManualHandAttached19c4r4(m)&&finds==1);
    for(int invalid:{-1,2}){n.side=invalid;ActualPendingTraceT24();CHECK(loggedGrip==0&&loggedTrigger==0);}
    n.side=0;ActualPendingTraceT24();CHECK(loggedGrip==.8f&&loggedTrigger==.6f);
    for(int side=0;side<2;++side){
        Clear();Own(side);f.sample.empty[0]=f.sample.empty[1]=true;contactH11.valid[0]=contactH11.valid[1]=true;
        ActualFistClaimBlockT24();CHECK(f.sample.empty[0]&&f.sample.empty[1]&&f.sample.claimedT24[side]&&!f.sample.claimedT24[1-side]);
        CHECK(!contactH11.valid[side]&&contactH11.valid[1-side]);
    }
}
static void FistMotion(){
    namespace fm=kcd2vr::fists;
    for(int claimed=0;claimed<2;++claimed){
        Clear();Own(claimed);f.sample.empty[0]=f.sample.empty[1]=true;ActualFistClaimBlockT24();
        fm::Sample in{};in.epoch=in.sequence=1;in.tick=1000;in.actor=123;in.valid=in.empty[0]=in.empty[1]=true;
        in.head={0,1.6,0};in.controller[0]=in.allowed[0]={.2,1.25,-.2};in.controller[1]=in.allowed[1]={-.2,1.25,-.2};
        in.claimedT24[0]=f.sample.claimedT24[0];in.claimedT24[1]=f.sample.claimedT24[1];
        fm::State state{};fm::step(state,in);
        const auto advance=[&](){++in.sequence;in.tick+=10;return fm::step(state,in);};
        in.grip[0]=in.grip[1]=in.trigger[0]=in.trigger[1]=1;
        for(int k=0;k<20;++k)(void)advance();
        int freePunches=0,claimedPunches=0;
        for(int k=0;k<12;++k){in.controller[1-claimed].z-=.018;in.allowed[1-claimed].z-=.018;freePunches+=advance().valid;}
        for(int k=0;k<12;++k){in.controller[claimed].z-=.018;in.allowed[claimed].z-=.018;claimedPunches+=advance().valid;}
        CHECK(freePunches==1);CHECK(claimedPunches==0);
        // The original native equipped-opposite-hand exclusion is preserved.
        state={};in.empty[1-claimed]=false;in.claimedT24[0]=in.claimedT24[1]=false;
        CHECK(!fm::step(state,in).valid);
        for(int k=0;k<20;++k)CHECK(!advance().valid);
    }
}
static void Utilities(){
    namespace pad=kcd2vr::native_vr_pad_premap;
    kcd2vr::ControllerState l{},r{};l.active=r.active=true;pad::Context c{};
    pad::SetXQuickTalk58(true);pad::SetBQuickTalk45(true);pad::Reset();clockT24=1000;
    l.x_down=r.b_down=true;c.hand_claim_blocked_t24[1]=true;
    auto s=pad::Build(l,r,c);CHECK(!s.x&&s.rb); // right utility independent
    clockT24+=20;l.x_down=false;s=pad::Build(l,r,c);CHECK(!s.x);
    l.x_down=true;s=pad::Build(l,r,c);CHECK(!s.x);
    c.hand_claim_blocked_t24[1]=false;clockT24+=20;s=pad::Build(l,r,c);CHECK(!s.x);
    l.x_down=false;s=pad::Build(l,r,c);CHECK(!s.x); // no synthetic release
    l.x_down=true;clockT24+=20;s=pad::Build(l,r,c);CHECK(!s.x);
    l.x_down=false;clockT24+=20;s=pad::Build(l,r,c);CHECK(s.x); // fresh unclaimed press/release works
    pad::Reset();l.x_down=true;r.b_down=true;c.hand_claim_blocked_t24[0]=true;s=pad::Build(l,r,c);CHECK(!s.rb);
    clockT24+=20;l.x_down=false;s=pad::Build(l,r,c);CHECK(s.x&&!s.rb);
    c.hand_claim_blocked_t24[0]=false;s=pad::Build(l,r,c);CHECK(!s.rb);
    r.b_down=false;(void)pad::Build(l,r,c);r.b_down=true;s=pad::Build(l,r,c);CHECK(s.rb);
    // Actual native dialogue/menu choices are intentionally untouched.
    c.quick_talk_open54=true;c.hand_claim_blocked_t24[0]=c.hand_claim_blocked_t24[1]=true;
    l.x_down=true;r.a_down=true;s=pad::Build(l,r,c);CHECK(s.x&&s.a);
}
int main(){Policy();ActualConsumers();FistMotion();Utilities();std::printf("{\"checks\":%u,\"failures\":%u,\"red\":%d,\"native_execution\":false}\n",checks,failures,T24_CAUSAL_RED);return failures?3:0;}
