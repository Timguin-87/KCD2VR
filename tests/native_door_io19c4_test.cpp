// The actual DoorIO/arm implementation, with only external memory observations
// and three native leaf calls substituted. Native pins are never bypassed.
#define KCD2VR_DOOR_HOST_TEST 1
#ifndef _MSC_VER
#define __fastcall
#define __try try
#define __except(x) catch(...)
#endif
#define EXCEPTION_EXECUTE_HANDLER 1
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "core/door_channel19c4r4.h"
#include "core/native_door19c4.h"
#include "core/door_use46.h"
#include "core/native_door_script19c4r4.h"
#include "core/native_door_hold65.h"
#include "core/native_door_motion66.h"
#include "core/door_surface65.h"
#include "core/native_door_start19c4.h"
#include "core/native_door_pins19c4.h"
#include "core/door_candidates38.h"
#include "interaction_alignment.h"
#include "vrik/vr_combat_ownership111.h"
#include "vrik/workstation178r20c1.h"
#include "vrik/pickup_use19c3.h"
#include "vrik/held_interaction114.h"
#include "vrik/hands_mode59c7f.h"
#include "tracking_sample59c6.h"

namespace interaction_alignment=kcd2vr::interaction_alignment;
namespace diagnostics {namespace capture168=kcd2vr::diagnostics::capture168;}
namespace diagnostics::lab {static bool reference=false;static bool NativeReference(){return reference;}}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
namespace retail_build112 {static bool ready=true;static bool Ready(std::uintptr_t){return ready;}}
static std::vector<std::string> receipts;
namespace diagnostic_trace {static void Write(const char* s){receipts.emplace_back(s);}}
static std::uint64_t now=1000,headTick=1000,handTick=1000;
static std::uint64_t GetTickCount64(){return now;}
static std::uintptr_t base=0x10000000;
static std::uintptr_t WhBase(){return base;}
static constexpr std::uintptr_t kCSkeletonAnimVftRva=0x3A711E0,kCCharInstanceVftRva=0x40602F8;
static constexpr std::uintptr_t kCEntityVftRva=0x4072C78,kCEntityGetCharacterRva=0x6411AC;
static std::unordered_map<std::uintptr_t,unsigned char> image;
static std::array<unsigned char,0x100> entity{};
static std::array<unsigned char,0x300> character{},replacement{};
static std::array<unsigned char,0x100> fifo{},nextFifo{};
static std::array<unsigned char,8> player{},entitySystem{};
static void* registryEntity=entity.data();
static bool registryFault=false,registryChanged=false,registryEntryChanged=false;
static unsigned lookupCalls=0;
static void* currentCharacter=character.data();
static std::uint32_t registeredId=42,handItem=0;
static unsigned reservations65=0;
static unsigned PhysicalHandReservations114(){return reservations65;}
static bool recenterOnHand=false,revokeSourceOnHand38=false,alignmentEnabled38=true;
static std::uint64_t sourceGeneration38=1;
static bool identity=true,handKnown=true,failSpeed=false,failTime=false,hideRate=false,hideRateAfterPause=false;
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_reading{false};
static std::atomic<float> g_left_grip{1},g_right_grip{1};
static std::atomic<std::uint64_t> g_pose_epoch6689{7};
static std::atomic<void*> g_last_player_entity{player.data()};
static kcd2vr::door19c4::Point currentHand{};
static interaction_alignment::HandCandidate candidate{};
static kcd2vr::door_candidates38::Records<interaction_alignment::HandCandidate> doorCandidates38;
namespace kcd2vr::interaction_alignment {
std::uint64_t DoorSourceGeneration38() noexcept {return alignmentEnabled38?sourceGeneration38:0;}
bool DoorSourceCurrent38(std::uint64_t generation) noexcept {return generation&&generation==DoorSourceGeneration38();}
std::uint64_t DoorAcquisitionGeneration65() noexcept {return alignmentEnabled38?sourceGeneration38:0;}
bool DoorAcquisitionCurrent65(std::uint64_t generation) noexcept {return generation&&generation==DoorAcquisitionGeneration65();}
bool LastHandCandidate(HandCandidate& out) noexcept {out=candidate;return out.entity!=0;}
bool NativeDirectDoorCandidate54r1(int,std::uint64_t,HandCandidate&) noexcept {return false;}
void SetPalmRays59c9(const palm59c9::Pair&) noexcept {}
void SetHandRayWorld(bool,float,float,float,float,float,float,float,float,int) noexcept {}
bool ExactDoorCandidate38(std::uint32_t target,int side,std::uint64_t tick,HandCandidate& out) noexcept {
    return doorCandidates38.find(target,side,tick,out);
}}
template<class T> static bool Read(const void* p,T& out){
    if(!p||(hideRate&&p==character.data()+0x160))return false;const auto at=reinterpret_cast<std::uintptr_t>(p);
    if(at>=base&&at<base+0x5000000){
        unsigned char bytes[sizeof(T)]{};
        for(std::size_t i=0;i<sizeof(T);++i){const auto it=image.find(at+i);if(it==image.end())return false;bytes[i]=it->second;}
        std::memcpy(&out,bytes,sizeof(T));return true;
    }
    std::memcpy(&out,p,sizeof(T));return true;
}
template<class M,class T> static void Put(M& m,std::size_t off,const T& v){assert(off+sizeof(v)<=m.size());std::memcpy(m.data()+off,&v,sizeof(v));}
template<class T> static void Image(std::uintptr_t at,const T& v){const auto* b=reinterpret_cast<const unsigned char*>(&v);for(std::size_t i=0;i<sizeof(v);++i)image[at+i]=b[i];}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t vf=0;return Read(p,vf)&&vf==base+rva;}
static bool PhysEntityIdentity135(void* p,std::uint32_t id){std::uint32_t actual=0;
    return identity&&p==entity.data()&&id==registeredId&&PhysVft6698(p,kCEntityVftRva)&&Read(entity.data()+0x0C,actual)&&actual==id;}
static unsigned characterReads=0,handReads=0;
static void* EntityById6694(std::uint32_t id){return id==registeredId?entity.data():nullptr;}
static void* CharacterForEntity6689(void* p){++characterReads;return p==entity.data()?currentCharacter:nullptr;}
static bool ValidateCharInstance(void* p,void*& anim){
    if(!PhysVft6698(p,kCCharInstanceVftRva))return false;anim=static_cast<unsigned char*>(p)+0x110;
    return PhysVft6698(anim,kCSkeletonAnimVftRva);
}
static bool TrackingHeadUsable111(std::uint64_t t){return t>=headTick&&t-headTick<=60;}
static bool TrackingHandUsable111(int,std::uint64_t t){return t>=handTick&&t-handTick<=80;}
static int ExactHandItemState(int){return int(handItem);}
static bool ReadInteractionHandWorld123R1(int side,float& x,float& y,float& z){
    ++handReads;    if(!handKnown||side<0||side>1)return false;if(recenterOnHand){g_pose_epoch6689=8;recenterOnHand=false;}if(revokeSourceOnHand38){++sourceGeneration38;revokeSourceOnHand38=false;}x=currentHand.x;y=currentHand.y;z=currentHand.z;return true;
}
static unsigned getCalls=0,speedCalls=0,timeCalls=0;
static float clipTime=.1f;
static float NativeGetTime(void* a,int layer,int index){assert(a==character.data()+0x110&&layer==0&&index==0);++getCalls;return clipTime;}
static void NativeSpeed(void* a,int layer,float rate){assert(a==character.data()+0x110&&layer==0);++speedCalls;if(!failSpeed)std::memcpy(static_cast<unsigned char*>(a)+0x50,&rate,4);if(rate==0&&hideRateAfterPause)hideRate=true;}
static void NativeTime(void* a,int layer,float time){assert(a==character.data()+0x110&&layer==0);++timeCalls;if(!failTime)clipTime=time;}
static void* NativeLookup(void* system,std::uint32_t id){
    assert(system==entitySystem.data()&&id==42);++lookupCalls;
    if(registryFault){
#ifdef _MSC_VER
        *static_cast<volatile int*>(nullptr)=1;
#else
        throw 1;
#endif
    }
    if(registryChanged)Image(base+0x492D8A0,std::uintptr_t(0));
    if(registryEntryChanged)image[base+0x4B4F98]^=1;
    return registryEntity;
}
std::uintptr_t HostDoorNative19c4(std::uintptr_t rva) noexcept {
    if(rva==0x4B4F98)return reinterpret_cast<std::uintptr_t>(&NativeLookup);
    if(rva==0xA98B30)return reinterpret_cast<std::uintptr_t>(&NativeGetTime);
    if(rva==0xAA8448)return reinterpret_cast<std::uintptr_t>(&NativeSpeed);
    if(rva==0xAB1124)return reinterpret_cast<std::uintptr_t>(&NativeTime);
    assert(false);return 0;
}
namespace kcd2vr::door_signal19c4r4 {
static std::unique_ptr<door_channel19c4r4::Channel> channel;
static bool ready=false,haveAck=false;static door_script19c4r4::Ack answer{};
static std::vector<door_script19c4r4::Command> sent;
bool Ready() noexcept{return ready;}
bool Publish(const door_script19c4r4::Command& c,std::uintptr_t acceptedEntity=0) noexcept {if(!ready||!channel->publish(c,now,acceptedEntity))return false;sent.push_back(c);return true;}
bool ReadCleanupIdentity(door_script19c4r4::CleanupIdentity& c) noexcept {return channel->cleanup(c);}
bool RetireKnownAbsent(const door_script19c4r4::CleanupIdentity& c,std::uintptr_t current) noexcept {return channel->retireKnownAbsent(c,current);}
bool ReadAck(door_script19c4r4::Ack& a) noexcept {a=answer;return haveAck;}
}
namespace kcd2vr::frik_pose_probe {void PublishVisibleFailure(const char*) noexcept{}}
namespace frik_pose_probe=kcd2vr::frik_pose_probe;
#include "../src/vrik/native_door19c4.inl"
// External rigid-curve/controller projection leaves only. This suite executes
// actual current Arm, control, source, native-Use join and MotionOwner66 logic.
// The actual path inverse/binder have separate production-include fixtures.
static int driveStatus66=1;
static float projectedPhase66=std::nanf("");
static unsigned driveCalls66=0,detachCalls66=0,retireCalls66=0;
static int DriveDoor66(const door4::Request&,const kcd2vr::door_hold65::Hand&,
                       float actual,bool bind,float& out) noexcept {
    ++driveCalls66;out=bind||!std::isfinite(projectedPhase66)?actual:projectedPhase66;
    return driveStatus66;
}
static void DetachDoor66(const door4::Request&) noexcept {++detachCalls66;}
static void RetireDoor66(const door4::Request& r) noexcept {
    ++retireCalls66;auto expected=r.serial;
    if(expected)(void)g_door_live_serial66.compare_exchange_strong(expected,0);
    DetachDoor66(r);
}
namespace kcd2vr::action_time_owner59c11 {
static bool nativeGrasp62=false;
bool NativeGraspEnabled() noexcept{return nativeGrasp62;}
bool NativeGraspAvailable63() noexcept {return nativeGrasp62;}
void PreparePhysicalPickup63(bool,int,std::uint64_t) noexcept {}
void NotePhysicalPickupUse63(int,std::uint64_t,std::uint32_t) noexcept {}
void NoteUseInput(bool,bool,int,std::uint64_t) noexcept{}
}
namespace kcd2vr::frik_pose_probe {
void NoteVrPickupUse43(int,std::uint32_t,bool,std::uint64_t) noexcept{}
void SetInteractionHandContext158(int,bool,float,float,float,float) noexcept{}
}
namespace pickup_use19c3=kcd2vr::pickup_use19c3;
namespace door_use46=kcd2vr::door_use46;
namespace door_hold65=kcd2vr::door_hold65;
namespace door_surface65=kcd2vr::door_surface65;
namespace held_interaction114=kcd2vr::held_interaction114;
namespace physical_interaction106=kcd2vr::physical_interaction106;
namespace tracking59c6=kcd2vr::tracking59c6;
namespace palm59c9=kcd2vr::palm59c9;
namespace hands59c7f=kcd2vr::hands59c7f;
namespace action_time_owner59c11=kcd2vr::action_time_owner59c11;
static auto& clock54=now;
#include "door_producer_fixture46.h"

// Explicit copied tracking and physical-query leaves. The legacy no-entry Arm
// below remains useful only for the refused cross-character clock tests. Script
// owner positives use the actual Use producer and captured native entry.
static void FreshHand65(){
    door_hold65::Frame f{};(void)door_hold65::frame.read(f);
    for(int side=0;side<2;++side){auto& h=f.hand[side];
        h.available=handKnown;h.side=side;h.grip=side?g_left_grip.load():g_right_grip.load();
        h.tick=h.poseTick=now;h.player=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load());
        h.poseEpoch=g_pose_epoch6689.load();h.sourceGeneration=sourceGeneration38;
        if(!h.trackingEpoch)h.trackingEpoch=3;
        h.settingsEpoch=door_use46::settingsEpoch.load();
        h.experimentEpoch=kcd2vr::workstation178r20c1::experimentEpoch36.load();h.point=currentHand;
        if(h.grip<.30f)h.press=0;else if(!h.press)h.press=now-20;
    }
    assert(door_hold65::frame.publish(f));
}
static bool Surface65(int side=0,float distance=.08f){
    door_hold65::Frame f{};if(!door_hold65::frame.read(f))return false;
    const auto& h=f.hand[side];door_surface65::Witness w{};
    w.source=door_surface65::request(42,h,now,{});w.queryTick=now;w.sample=now;
    w.actualPalm=h.point;w.point=h.point;w.point.x+=distance;w.normal={1,0,0};
    w.entityId=42;w.opaqueOwner=reinterpret_cast<std::uintptr_t>(entity.data());
    w.opaqueCollider=0x4200;w.distance=distance;w.valid=true;
    return door_surface65::publish(w);
}

static void Setup(){
    base+=0x10000000;image.clear(); // pin cache is by exact image base
    for(const auto& p:kcd2vr::native_door_pins19c4::all)for(std::size_t i=0;i<p.size;++i)image[base+p.rva+i]=p.bytes[i];
    Image(base+kCSkeletonAnimVftRva+0xB8,base+0xAA8448);
    Image(base+kCSkeletonAnimVftRva+0xE8,base+0xAB1124);
    Image(base+kCCharInstanceVftRva+0x28,base+0x1A7FB10);
    Image(base+kCEntityVftRva+0x328,base+kCEntityGetCharacterRva);
    Image(base+0x492D8A0,reinterpret_cast<std::uintptr_t>(entitySystem.data()));
    Put(entitySystem,0,base+0x4077048);Image(base+0x4077048+0x70,base+0x4B4F98);
    registryEntity=entity.data();registryFault=registryChanged=registryEntryChanged=false;lookupCalls=0;
    entity.fill(0);character.fill(0);replacement.fill(0);fifo.fill(0);nextFifo.fill(0);
    Put(entity,0,base+kCEntityVftRva);Put(entity,0x0C,42u);Put(character,0,base+kCCharInstanceVftRva);
    Put(character,0x110,base+kCSkeletonAnimVftRva);Put(character,0x110+0x48,reinterpret_cast<std::uintptr_t>(fifo.data()+4));
    Put(character,0x110+0x50,1.f);Put(fifo,0,1u);Put(fifo,4+0x10,20);
    Put(nextFifo,0,1u);Put(nextFifo,4+0x10,21);
    now+=1000;headTick=handTick=now;registeredId=42;handItem=0;reservations65=0;currentCharacter=character.data();
    recenterOnHand=false;revokeSourceOnHand38=false;alignmentEnabled38=true;sourceGeneration38=1;identity=handKnown=true;failSpeed=failTime=hideRate=hideRateAfterPause=false;retail_build112::ready=true;diagnostics::lab::reference=false;
    g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_reading=false;animation_filter::nativeBodyOwned112=false;
    g_left_grip=g_right_grip=1;kcd2vr::tracking59c6::epoch=3;g_pose_epoch6689=7;g_last_player_entity=player.data();currentHand={};
    kcd2vr::combat_rules178r10::f5Rung=2;kcd2vr::workstation178r20c1::experimentArmed36=false;
    candidate={};candidate.tick=now;candidate.distance=.2f;candidate.owner=0;candidate.object=true;candidate.entity=42;std::strcpy(candidate.cls,"AnimDoor");
    candidate.sourceGeneration=sourceGeneration38;candidate.sourceEpoch=7;candidate.sourceTick=now;
    candidate.sourcePlayer=reinterpret_cast<std::uintptr_t>(player.data());
    doorCandidates38.clear();
    g_door_owner19c4={};g_door_stop19c4=false;g_door_active19c4=false;g_door_request19c4.publish({});kcd2vr::door_start19c4::watched.publish({});kcd2vr::door_start19c4::accepted.publish({});
    getCalls=speedCalls=timeCalls=characterReads=handReads=0;clipTime=.1f;receipts.clear();
    g_door_live_serial66=0;driveStatus66=1;projectedPhase66=std::nanf("");driveCalls66=detachCalls66=retireCalls66=0;
    kcd2vr::door_signal19c4r4::channel=std::make_unique<kcd2vr::door_channel19c4r4::Channel>();
    kcd2vr::door_signal19c4r4::channel->enabled(true);
    g_door_script_owner19c4r4={};g_door_legacy_owner69={};kcd2vr::door_signal19c4r4::ready=false;kcd2vr::door_signal19c4r4::haveAck=false;
    kcd2vr::door_signal19c4r4::answer={};kcd2vr::door_signal19c4r4::sent.clear();
    kcd2vr::door_use46::intent.publish({});
    kcd2vr::door_use46::currentInput62.publish({});kcd2vr::door_use46::physicalUse62.publish({});kcd2vr::door_use46::acceptedIntent62.publish({});
    kcd2vr::action_time_owner59c11::nativeGrasp62=false;
    door_hold65::frame.publish({});door_hold65::operation.publish({});door_hold65::resume.publish({});
    door_surface65::requestMailbox.publish({});door_surface65::witnessMailbox.publish({});
    kcd2vr::door_use46::SetEnabled(true);FreshHand65();assert(Surface65());
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,false,now);
}
static door4::Request Arm(){doorCandidates38.record(candidate);ArmDoor19c4(42,0x12345,"door_l_f_o",0,now);door4::Request r{};assert(g_door_request19c4.read(r));return r;}
static void Ack(int id=20){kcd2vr::door_start19c4::started(reinterpret_cast<std::uintptr_t>(character.data()+0x110),0,id,now);}
static float Rate(){float r=-1;Read(character.data()+0x160,r);return r;}
static void Tick(const door4::Request& r){DoorIO19c4 io;g_door_owner19c4.tick(io,r,now);}
static void Step(unsigned ms=10){now+=ms;headTick=handTick=now;FreshHand65();}
// The generic palm's first rank can differ from the exact native door accepted
// in the same input transaction. Run the real producer before the native-entry
// snapshot; no hand-built Door Intent is permitted in this connected fixture.
static DoorRequestWitness38 PhysicalDoorEntry62(Producer& producer,bool transport=true,int hand=0){
    producer.player=reinterpret_cast<std::uintptr_t>(player.data());
    producer.touch_bindings_.allow=transport;
    candidate.owner=hand;const auto exact=candidate;
    candidate.entity=99;std::strcpy(candidate.cls,"PickableItem");
    producer.tick(now,0,hand);Step(20);candidate.tick=candidate.sourceTick=now;producer.tick(now,1,hand);
    Step(20);candidate.tick=candidate.sourceTick=now;producer.tick(now,1,hand);
    assert(!producer.doorUse46.door&&producer.touch_bindings_.requests==0);
    kcd2vr::door_use46::Intent none{};assert(!kcd2vr::door_use46::read(none,now));
    DoorRequestWitness38 entry{};entry.candidate=exact;entry.candidate.tick=entry.candidate.sourceTick=now;
    entry.epoch=7;entry.player=producer.player;
    assert(producer.surfaceHit65(42,now,hand));
    CaptureDoorRequest46(entry,42,hand,now);return entry;
}
static std::unique_ptr<Producer> scriptProducer65;
static door4::Request ScriptArm65(){
    scriptProducer65=std::make_unique<Producer>();
    scriptProducer65->f5Mode=kcd2vr::combat_rules178r10::f5Rung.load();
    scriptProducer65->comparison114=scriptProducer65->f5Mode!=0;
    auto entry=PhysicalDoorEntry62(*scriptProducer65);
    if(!entry.acceptedUse62.valid)std::fprintf(stderr,"accepted failure mode=%u compare=%d now=%llu requests=%u epoch=%llu\n",scriptProducer65->f5Mode,int(scriptProducer65->comparison114),(unsigned long long)now,scriptProducer65->touch_bindings_.requests,(unsigned long long)kcd2vr::workstation178r20c1::experimentEpoch36.load());
    assert(entry.acceptedUse62.valid);
    ArmDoor19c4(42,0x12345,"door_l_f_o",0,now,&entry);
    door4::Request r{};assert(g_door_request19c4.read(r)&&r.serial&&r.trackingEpoch46&&
        r.inputSettingsEpoch46&&r.inputPress65);
    return r;
}
int main(){
    // Ordinary accepted native door clips never belonged to the protected-body
    // combat allowlist. Keep that fact and the paired/death owners unchanged;
    // an indiscriminate body/gameplay exception is neither needed nor allowed.
    for(const char* alias:{"door_l_b_o_player","door_l_b_c_player","door_l_f_o_player","door_l_f_c_player",
                         "door_r_b_o_player","door_r_b_c_player","door_r_f_o_player","door_r_f_c_player"})
        assert(!kcd2vr::vr_combat111::NativeBodyAlias112(alias));
    assert(kcd2vr::vr_combat111::NativeBodyAlias112("combat_clinch1_sz0_az0_ripostesync_step0_lngsw_master"));
    assert(kcd2vr::vr_combat111::NativeBodyAlias112("combat_combodeath_c05_left_blunt_master"));
    // Static retail API proof is necessary but not native job ownership.
    for(const auto& p:kcd2vr::native_door_pins19c4::all){Setup();image[base+p.rva]^=1;assert(!DoorPins19c4());assert(!Arm().serial&&speedCalls==0);}
    for(const auto slot:{kCSkeletonAnimVftRva+0xB8,kCSkeletonAnimVftRva+0xE8,kCCharInstanceVftRva+0x28}){
        Setup();Image(base+slot,std::uintptr_t(0));assert(!DoorPins19c4());}
    Setup();retail_build112::ready=false;assert(!DoorPins19c4());retail_build112::ready=true;assert(DoorPins19c4());
    // Actual native acceptance, exact door and accepted animation-start still
    // cannot authorize a cross-character FIFO mutation on Henry's update.
    Setup();auto r=ScriptArm65();assert(r.serial&&r.id==42);Ack();
    for(int i=0;i<15;++i){Tick(r);Step();scriptProducer65->tick(now,1);currentHand.x+=.02f;}
    assert(!g_door_owner19c4.active&&getCalls==0&&speedCalls==0&&timeCalls==0&&Rate()==1&&receipts.empty());
    Step(251);Tick(r);assert(!g_door_owner19c4.pending&&speedCalls==0&&timeCalls==0);
    // One reason per accepted request while recording; no per-tick formatting.
    Setup();diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,now);
    r=ScriptArm65();receipts.clear();Ack();for(int i=0;i<10;++i){Tick(r);Step();scriptProducer65->tick(now,1);}
    assert(receipts.size()==1&&receipts[0].find("DOOR_ANIMATION_JOB_BOUNDARY_UNPROVEN")!=std::string::npos);
    assert(getCalls==0&&speedCalls==0&&timeCalls==0&&!g_door_owner19c4.active);
    // Native Use is not canceled: Arm observes acceptance and publishes only a
    // request. Original native action/door animation remain untouched.
    assert(r.action==0x12345&&Rate()==1&&clipTime==.1f);
    // Existing cleanup is retained, rather than discarding a possible old pause.
    // These leaf substitutions prove compare-owned cleanup only, NOT job sync.
    Setup();door4::Binding old{};old.id=42;old.entity=reinterpret_cast<std::uintptr_t>(entity.data());
    old.character=reinterpret_cast<std::uintptr_t>(character.data());old.anim=old.character+0x110;old.savedRate=1;
    Put(character,0x160,0.f);DoorIO19c4 io;assert(io.restore(old)==1&&Rate()==1&&speedCalls==1);
    Put(character,0x160,.7f);assert(io.restore(old)==0&&Rate()==.7f&&speedCalls==1);
    // Retired no-entry calls remain refused regardless of candidate, surface or action.
    // Current source/candidate refusals are exercised through native entry below.
    for(int why=0;why<11;++why){Setup();
        if(why==0)candidate.owner=1;if(why==1)candidate.object=false;if(why==2)candidate.tick=now-81;
        if(why==3)door_surface65::witnessMailbox.publish({});if(why==4)candidate.entity=99;if(why==5)std::strcpy(candidate.cls,"PickableItem");
        if(why==6)g_door_stop19c4=true;if(why==7)handKnown=false;
        if(why==8)Put(character,0,std::uintptr_t(0));if(why==9)g_last_player_entity=nullptr;
        if(why==10){door_surface65::Witnesses w{};door_surface65::witnessMailbox.read(w);for(auto& v:w.value)v.distance=std::nanf("");door_surface65::witnessMailbox.publish(w);}
        assert(!Arm().serial&&speedCalls==0&&timeCalls==0);}
    for(const char* action:{"lockpick","door_knock","door_l_f_other",""}){Setup();ArmDoor19c4(42,0x123,action,0,now);door4::Request no{};g_door_request19c4.read(no);assert(!no.serial);}
    // G2 intentionally assigns the two door implementations to F5=2/3.
    // Both retain native admission and the same source-qualified acquisition.
    for(unsigned rung46:{0u,2u,3u}){Setup();kcd2vr::combat_rules178r10::f5Rung=rung46;
        kcd2vr::workstation178r20c1::experimentArmed36=false;
        const auto ordinary46=ScriptArm65();assert(ordinary46.serial&&io.allowed(ordinary46));}
    Setup();kcd2vr::door_use46::SetEnabled(false);assert(!Arm().serial);
    kcd2vr::door_use46::SetEnabled(true);
    // C35 copied admission evidence does not relax any of these gates.
    Setup();diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,now);
    candidate.entity=99;std::strcpy(candidate.cls,"BasicEntity");ArmDoor19c4(42,0x123,"door_l_f_o",0,now);
    assert(receipts.size()==1&&receipts.back().find("NO_EXACT_DOOR_HAND_WITNESS")!=std::string::npos&&
        receipts.back().find("candidateId=99 class=BasicEntity")!=std::string::npos&&getCalls==0&&speedCalls==0);
    for(int i=0;i<40;++i)ArmDoor19c4(42,0x123,"door_l_f_o",-1,now);
    assert(receipts.size()==32&&receipts.back().find("NO_FRESH_HAND_CONTEXT")!=std::string::npos);
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,false,now);
    ArmDoor19c4(42,0x123,"door_l_f_o",-1,now);assert(receipts.size()==32);
    // C38 reproduces the observed OTHER-hand pickup overwrite. The exact
    // native accepted door/hand uses its own copied witness, never that pickup.
    Setup();Producer opposite38;DoorRequestWitness38 exact38=PhysicalDoorEntry62(opposite38);
    candidate.entity=263342;candidate.owner=1;std::strcpy(candidate.cls,"PickableItem");
    ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&exact38);door4::Request joined38{};
    g_door_request19c4.read(joined38);assert(joined38.serial&&joined38.id==42&&joined38.side==0);
    assert(getCalls==0&&speedCalls==0&&timeCalls==0);
    // C46 native hook entry always needs its explicit input token. A native
    // manual Use with an otherwise exact close candidate keeps native timing.
    Setup();exact38={candidate,g_pose_epoch6689.load(),reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())};doorCandidates38.clear();
    ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&exact38);g_door_request19c4.read(joined38);
    assert(!joined38.serial);
    // Repeat the exact native entry/source/geometry refusals with a real
    // transported Use. A legacy no-entry helper no longer grants ownership.
    for(int bad38=0;bad38<16;++bad38){Setup();Producer p;auto transported=PhysicalDoorEntry62(p);
        exact38={transported.candidate,transported.epoch,transported.player};
        if(bad38==0)exact38.candidate.entity=99;if(bad38==1)exact38.candidate.owner=1;
        if(bad38==2)exact38.candidate.tick=now-81;if(bad38==3)exact38.candidate.tick=now+1;
        if(bad38==4){currentHand.x=.36f;FreshHand65();}if(bad38==5){++g_pose_epoch6689;FreshHand65();}
        if(bad38==6)exact38={};if(bad38==7)++g_pose_epoch6689;
        if(bad38==8)g_last_player_entity=replacement.data();
        if(bad38==9)++sourceGeneration38;if(bad38==10)alignmentEnabled38=false;
        if(bad38==11)exact38.candidate.sourceEpoch=6;
        if(bad38==12)exact38.candidate.sourcePlayer=reinterpret_cast<std::uintptr_t>(replacement.data());
        if(bad38==13||bad38==14){door_hold65::Frame f{};door_hold65::frame.read(f);
            f.hand[0].poseTick=bad38==13?now-81:now+1;door_hold65::frame.publish(f);}
        if(bad38==15)kcd2vr::tracking59c6::epoch.fetch_add(1);
        CaptureDoorRequest46(exact38,42,0,now);
        ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&exact38);g_door_request19c4.read(joined38);
        if(joined38.serial)std::fprintf(stderr,"native entry refusal case %d unexpectedly admitted\n",bad38);
        assert(!joined38.serial&&getCalls==0&&speedCalls==0&&timeCalls==0);
    }
    // Withdrawal must revoke the previous target even inside the age budget.
    Setup();doorCandidates38.record(candidate);doorCandidates38.withdraw(42);
    ArmDoor19c4(42,0x123,"door_l_f_o",0,now);g_door_request19c4.read(joined38);assert(!joined38.serial);
    // The actual native adapter publishes POD only. A request admitted from the
    // real Use producer + native entry + current surface must receive Lua ACK1
    // before controller motion. The command does not write another character's animation.
    auto& commands=kcd2vr::door_signal19c4r4::sent;
    auto acknowledge65=[&](float progress=.1f,int state=1){
        assert(!commands.empty());const auto& c=commands.back();
        kcd2vr::door_signal19c4r4::haveAck=true;
        kcd2vr::door_signal19c4r4::answer={c.sequence,std::uint32_t(r.serial),state,progress,now};};
    auto activate65=[&](){
        TickDoorScript19c4r4();assert(commands.size()==1&&commands.back().phase==1);
        acknowledge65();Step();TickDoorScript19c4r4();
        assert(g_door_script_owner19c4r4.active&&!g_door_script_owner19c4r4.pending&&commands.back().phase==2);};
    // Default gameplay owns the same spatial route with no F5 experiment.
    // Preserve raw mode 0 in the accepted token, map only the Lua command to 2.
    Setup();kcd2vr::combat_rules178r10::f5Rung=0;
    kcd2vr::workstation178r20c1::experimentEpoch36=0;
    kcd2vr::door_signal19c4r4::ready=true;r=ScriptArm65();
    assert(r.mode69==0&&r.inputExperimentEpoch65==0);activate65();
    assert(commands.back().mode69==2&&g_door_script_owner19c4r4.active&&!g_door_legacy_owner69.active);
    acknowledge65(.4f);g_right_grip=0;Step();TickDoorScript19c4r4();
    assert(commands.back().phase==4&&commands.back().mode69==2&&g_door_script_owner19c4r4.paused);
    // Even switching between two spatial modes revokes the original source.
    kcd2vr::combat_rules178r10::f5Rung=2;Step();TickDoorScript19c4r4();
    assert(!g_door_script_owner19c4r4.active&&commands.back().phase==0);
    Setup();kcd2vr::door_signal19c4r4::ready=true;r=ScriptArm65();activate65();
    acknowledge65(.3f);Step();currentHand.x=.11f;FreshHand65();TickDoorScript19c4r4();
    assert(commands.back().phase==2&&std::fabs(commands.back().fraction-.3f)<.0001f);
    // No fraction is learned from hand travel: only the native Lua time ACK.
    acknowledge65(.3f);Step();currentHand.x=1.3f;FreshHand65();TickDoorScript19c4r4();
    assert(commands.back().phase==2&&commands.back().fraction==.3f);
    assert(getCalls==0&&speedCalls==0&&timeCalls==0&&Rate()==1);
    reservations65=2;acknowledge65(.3f);Step();TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.active&&commands.back().phase==2); // Other hand is not this lease.
    reservations65=0;g_pose_epoch6689=8;Step();TickDoorScript19c4r4();assert(!g_door_script_owner19c4r4.active&&commands.back().phase==0);
    const auto count=commands.size();g_pose_epoch6689=7;Step();TickDoorScript19c4r4();assert(commands.size()==count);
    // Initial source loss cannot claim/pause the door. Release before the first
    // native ACK is different from releasing an already-owned operation.
    for(int bad=0;bad<10;++bad){Setup();kcd2vr::door_signal19c4r4::ready=true;r=ScriptArm65();
        if(bad==0)g_ctx_menu=true;if(bad==1)g_ctx_dialogue=true;if(bad==2)g_ctx_cutscene=true;
        if(bad==3)animation_filter::nativeBodyOwned112=true;if(bad==4){g_right_grip=0;FreshHand65();}
        if(bad==5)headTick=now-61;if(bad==6)handTick=now-81;
        if(bad==7)reservations65=1;if(bad==8){g_pose_epoch6689=8;FreshHand65();}
        if(bad==9){door_hold65::Frame f{};door_hold65::frame.read(f);++f.hand[0].press;door_hold65::frame.publish(f);}
        TickDoorScript19c4r4();assert(!g_door_script_owner19c4r4.pending&&!g_door_script_owner19c4r4.active);
        for(const auto& c:commands)assert(c.phase==0);
        assert(getCalls==0&&speedCalls==0&&timeCalls==0);}
    // Controller intent requests time. Release pauses the SAME native operation at its readback
    // time, including past the retired 15-second timeout. A new squeeze alone
    // cannot resume: it needs a new same-door surface-qualified Resume lease.
    Setup();kcd2vr::door_signal19c4r4::ready=true;r=ScriptArm65();activate65();
    acknowledge65(.5f);g_right_grip=0;Step();TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.active&&g_door_script_owner19c4r4.paused&&commands.back().phase==4&&commands.back().fraction==.5f);
    for(int paused=0;paused<801;++paused){acknowledge65(.5f);Step(20);TickDoorScript19c4r4();
        assert(g_door_script_owner19c4r4.active&&commands.back().phase==4&&commands.back().fraction==.5f);}
    g_right_grip=1;acknowledge65(.5f);Step();TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.active&&g_door_script_owner19c4r4.paused&&commands.back().phase==4);
    door_hold65::Frame resumeFrame65{};door_hold65::frame.read(resumeFrame65);
    door_hold65::resume.publish({r.serial,r.id,r.entity,resumeFrame65.hand[0]});
    acknowledge65(.5f);Step();TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.active&&!g_door_script_owner19c4r4.paused&&commands.back().phase==2);
    // Completion is ACK2 from the native owner, never endpoint readback or release.
    acknowledge65(.9995f);Step();TickDoorScript19c4r4();assert(g_door_script_owner19c4r4.active&&commands.back().phase==2);
    const auto beforeCompletion65=commands.size();acknowledge65(.9995f,2);Step();TickDoorScript19c4r4();
    assert(!g_door_script_owner19c4r4.active&&!g_door_script_owner19c4r4.pending&&commands.size()==beforeCompletion65);
    door_hold65::Operation completed65{};door_hold65::operation.read(completed65);assert(!completed65.request.serial);
    assert(getCalls==0&&speedCalls==0&&timeCalls==0);
    // C66 adapter reaches the real MotionOwner66. Geometry is the explicit
    // leaf above; only a verified projection result may become phase2. Native
    // curve binding and inverse-path math execute in their separate fixtures.
    Setup();kcd2vr::door_signal19c4r4::ready=true;r=ScriptArm65();activate65();
    projectedPhase66=.44f;acknowledge65(.1f);Step();TickDoorScript19c4r4();
    assert(commands.back().phase==2&&std::fabs(commands.back().fraction-.44f)<.00001f&&driveCalls66>=2);
    assert(g_door_live_serial66==r.serial&&getCalls==0&&speedCalls==0&&timeCalls==0);
    // The owner keeps a held endpoint below native completion. Grip RELEASE
    // requests phase5; only an actual Lua ACK2 retires its live presentation.
    acknowledge65(.9995f);projectedPhase66=.9995f;Step();TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.active&&!g_door_script_owner19c4r4.finishing&&commands.back().phase==2);
    acknowledge65(.9995f);g_right_grip=0;Step();TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.active&&g_door_script_owner19c4r4.finishing&&
        commands.back().phase==5&&g_door_live_serial66==r.serial&&detachCalls66);
    acknowledge65(.9995f,2);Step();TickDoorScript19c4r4();
    assert(!g_door_script_owner19c4r4.active&&!g_door_live_serial66&&retireCalls66);
    Setup();kcd2vr::door_signal19c4r4::ready=true;r=ScriptArm65();activate65();
    acknowledge65(0);g_right_grip=0;Step();TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.active&&g_door_script_owner19c4r4.finishing&&commands.back().phase==5);
    acknowledge65(0,2);Step();TickDoorScript19c4r4();assert(!g_door_live_serial66);
    // A not-yet-resident current rigid curve may wait only250ms after ACK1.
    // Repeated fresh controller/source/ACK heartbeats cannot prolong that wait.
    Setup();kcd2vr::door_signal19c4r4::ready=true;r=ScriptArm65();driveStatus66=0;
    TickDoorScript19c4r4();acknowledge65();Step();TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.active&&commands.back().phase==1);
    for(int i=0;i<5;++i){acknowledge65();Step(40);TickDoorScript19c4r4();assert(g_door_script_owner19c4r4.active&&commands.back().phase==1);}
    acknowledge65();Step(51);TickDoorScript19c4r4();
    assert(!g_door_script_owner19c4r4.active&&commands.back().phase==0&&!g_door_live_serial66);
    // A transient pending curve can become ready inside the same request;
    // a hard geometry refusal instead clears current Door presentation.
    Setup();kcd2vr::door_signal19c4r4::ready=true;r=ScriptArm65();driveStatus66=0;
    TickDoorScript19c4r4();acknowledge65();Step();TickDoorScript19c4r4();
    driveStatus66=1;acknowledge65();Step(20);TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.active&&commands.back().phase==2);
    driveStatus66=-1;acknowledge65();Step();TickDoorScript19c4r4();
    assert(!g_door_script_owner19c4r4.active&&commands.back().phase==0&&!g_door_live_serial66&&retireCalls66);
    assert(getCalls==0&&speedCalls==0&&timeCalls==0);
    // Every context/identity/source loss still releases an owned or paused
    // operation for native cleanup. Ordinary open Grip is deliberately absent.
    for(int cancel=0;cancel<22;++cancel){Setup();kcd2vr::door_signal19c4r4::ready=true;r=ScriptArm65();activate65();
        acknowledge65();Step();
        if(cancel==0)g_ctx_menu=true;if(cancel==1)g_ctx_dialogue=true;if(cancel==2)g_ctx_cutscene=true;
        if(cancel==3)animation_filter::nativeBodyOwned112=true;if(cancel==4)reservations65=1;
        if(cancel==5)headTick=now-61;if(cancel==6)handTick=now-81;
        if(cancel==7)handItem=1;if(cancel==8)g_pose_epoch6689=8;
        if(cancel==9)alignmentEnabled38=false;if(cancel==10)++sourceGeneration38;
        if(cancel==11)g_last_player_entity=nullptr;
        if(cancel==18)g_ctx_reading=true;if(cancel==19)kcd2vr::workstation178r20c1::experimentEpoch36.fetch_add(1);
        if(cancel==20)kcd2vr::door_use46::SetEnabled(false);if(cancel==21)kcd2vr::tracking59c6::epoch.fetch_add(1);
        if(cancel>=12&&cancel<18){door_hold65::Frame f{};door_hold65::frame.read(f);
            if(cancel==12)++f.hand[0].trackingEpoch;if(cancel==13)++f.hand[0].settingsEpoch;
            if(cancel==14)++f.hand[0].experimentEpoch;if(cancel==15)f.hand[0].tick=now-81;
            if(cancel==16)f.hand[0].poseTick=now-81;if(cancel==17)f.hand[0].available=false;
            door_hold65::frame.publish(f);}
        TickDoorScript19c4r4();assert(!g_door_script_owner19c4r4.active&&commands.back().phase==0);
        assert(getCalls==0&&speedCalls==0&&timeCalls==0);
    }
    // Actual Tick must revisit the bridge cleanup even after local release and
    // a newer accepted request, with no native time/speed mutation.
    for(int state=0;state<10;++state){
        Setup();kcd2vr::door_signal19c4r4::ready=true;r=ScriptArm65();TickDoorScript19c4r4();
        auto& channel=*kcd2vr::door_signal19c4r4::channel;
        assert(channel.claim(kcd2vr::door_signal19c4r4::sent.back().sequence,std::uint32_t(r.serial),now));
        kcd2vr::door_script19c4r4::CleanupIdentity lease{};assert(channel.cleanup(lease)&&lease.entity==r.entity);
        g_door_script_owner19c4r4.active=g_door_script_owner19c4r4.pending=false;
        if(state==0)registryEntity=nullptr; // known native null retires
        if(state==1){Put(replacement,0x0C,42u);registryEntity=replacement.data();} // known current replacement
        if(state==2)registryFault=true; // native exception is UNKNOWN
        if(state==3)Image(base+0x492D8A0,std::uintptr_t(0)); // unavailable system
        if(state==4)Image(base+0x4077048+0x70,std::uintptr_t(0)); // unknown slot
        if(state==5)registryChanged=true; // owner changes during lookup
        if(state==6){channel.enabled(false);registryEntity=nullptr;g_door_stop19c4=true;}
        if(state==7){auto newer=r;++newer.serial;g_door_request19c4.publish(newer);
            g_door_script_owner19c4r4.seen=newer.serial;g_door_script_owner19c4r4.request=newer;registryEntity=nullptr;}
        if(state==8)image[base+0x4B4F98]^=1; // cached Arm pins cannot authorize changed entry
        if(state==9)registryEntryChanged=true; // changed during native call is UNKNOWN
        Step(200);TickDoorScript19c4r4();
        const bool retired=state==0||state==1||state==6||state==7;
        assert(channel.pending()!=retired);assert(getCalls==0&&speedCalls==0&&timeCalls==0);
        if(state==0){registryEntity=entity.data();candidate.entity=42;std::strcpy(candidate.cls,"AnimDoor");candidate.tick=candidate.sourceTick=now;r=ScriptArm65();assert(r.serial);TickDoorScript19c4r4();
            assert(channel.enabled()&&channel.claim(kcd2vr::door_signal19c4r4::sent.back().sequence,std::uint32_t(r.serial),now));}
        if(!retired){registryFault=registryChanged=registryEntryChanged=false;registryEntity=nullptr;
            image[base+0x4B4F98]=kcd2vr::native_door_pins19c4::entityLookup[0];
            Image(base+0x492D8A0,reinterpret_cast<std::uintptr_t>(entitySystem.data()));
            Image(base+0x4077048+0x70,base+0x4B4F98);TickDoorScript19c4r4();assert(!channel.pending());}
    }
    Setup();kcd2vr::door_signal19c4r4::ready=true;r=ScriptArm65();TickDoorScript19c4r4();
    {auto& channel=*kcd2vr::door_signal19c4r4::channel;
     assert(channel.claim(2,std::uint32_t(r.serial),now));
     g_door_script_owner19c4r4.active=g_door_script_owner19c4r4.pending=false;
     Step(1000);TickDoorScript19c4r4();assert(channel.pending()); // unchanged full ID/address retains, no ABA guess
    }
    // An explicit sent native-door input still joins the exact captured token.
    // Here the real producer supplies it (rather than constructing an unrelated
    // tracking epoch/press by hand). Native descriptor centre is not a handle.
    Setup();Producer explicit65;explicit65.player=reinterpret_cast<std::uintptr_t>(player.data());
    candidate.z=1.4f;candidate.distance=1.4f;doorCandidates38.record(candidate);
    explicit65.tick(now,0);Step(20);candidate.tick=candidate.sourceTick=now;doorCandidates38.record(candidate);explicit65.tick(now,1);
    Step(20);candidate.tick=candidate.sourceTick=now;doorCandidates38.record(candidate);explicit65.tick(now,1);
    kcd2vr::door_use46::Intent semi46{};assert(kcd2vr::door_use46::read(semi46,now)&&semi46.sent);
    DoorRequestWitness38 captured46{};captured46.epoch=7;captured46.player=explicit65.player;
    CaptureDoorRequest46(captured46,42,0,now);assert(captured46.input46.token==semi46.token);
    doorCandidates38.clear(); // Native inUse may stop enumerating the door.
    ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&captured46);g_door_request19c4.read(r);
    assert(r.serial&&r.inputToken46==semi46.token&&r.trackingEpoch46==explicit65.trackingEpoch&&r.inputPress65==semi46.press);
    const auto accepted46=r.serial;
    ArmDoor19c4(42,0x999,"door_l_f_o",0,now,&captured46);g_door_request19c4.read(r);
    assert(r.serial==accepted46); // One squeeze cannot be consumed twice.
    // Missing, replaced or settings-revoked explicit tokens never acquire a
    // clock, even with a nearby fresh exact native candidate and surface.
    for(int bad=0;bad<10;++bad){Setup();Producer p;p.player=reinterpret_cast<std::uintptr_t>(player.data());
        p.tick(now,0);Step(20);candidate.tick=candidate.sourceTick=now;doorCandidates38.record(candidate);p.tick(now,1);
        Step(20);candidate.tick=candidate.sourceTick=now;doorCandidates38.record(candidate);p.tick(now,1);
        assert(kcd2vr::door_use46::read(semi46,now));captured46={};captured46.epoch=7;captured46.player=p.player;
        CaptureDoorRequest46(captured46,42,0,now);assert(captured46.input46.token);
        if(bad==0)kcd2vr::door_use46::intent.publish({});
        if(bad==1){++semi46.token;kcd2vr::door_use46::intent.publish(semi46);}
        if(bad==2)++g_pose_epoch6689;if(bad==3)++sourceGeneration38;
        if(bad==4){++semi46.trackingEpoch;kcd2vr::door_use46::intent.publish(semi46);}
        if(bad==5)captured46.input46.selectedTick=now-251;
        if(bad==6)captured46.input46.target=99;
        if(bad==7){kcd2vr::door_use46::SetEnabled(false);kcd2vr::door_use46::SetEnabled(true);}
        if(bad==8){captured46.input46={};captured46.inputPresent46=true;captured46.candidate=candidate;}
        if(bad==9){ // Released before a later native entry cannot revive this exact close offer.
            Step(20);p.tick(now,0);captured46={candidate,7,p.player};
            CaptureDoorRequest46(captured46,42,0,now);assert(!captured46.input46.token&&!captured46.acceptedUse62.valid);}
        ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&captured46);g_door_request19c4.read(r);
        assert(!r.serial&&getCalls==0&&speedCalls==0&&timeCalls==0);
    }
    // C62: a successful physical held Use, then native accepted Door, recovers
    // exactly that press. A nearby projected item cannot steal the selection,
    // and the worker does not send another Use to obtain its door owner.
    Setup();Producer physical62;auto entry62=PhysicalDoorEntry62(physical62);
    assert(entry62.acceptedUse62.valid&&!entry62.input46.token&&physical62.touch_bindings_.held);
    const auto sourcePress62=entry62.acceptedUse62.pulse.input.press;
    const auto sourceTick62=entry62.acceptedUse62.pulse.input.now;
    ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&entry62);assert(g_door_request19c4.read(r)&&r.serial&&r.inputToken46);
    const auto nativeSerial62=r.serial;
    kcd2vr::door_use46::Intent accepted62{};assert(kcd2vr::door_use46::read(accepted62,now));
    assert(accepted62.press==sourcePress62&&accepted62.physicalUseSerial62==entry62.acceptedUse62.pulse.serial);
    ArmDoor19c4(42,0x456,"door_l_f_o",0,now,&entry62);g_door_request19c4.read(r);assert(r.serial==nativeSerial62);
    Step(20);physical62.tick(now,1);
    assert(physical62.doorUse46.chosen.token==r.inputToken46&&physical62.doorUse46.chosen.target==42&&
        physical62.doorUse46.chosen.press==sourcePress62&&!physical62.touch_bindings_.held&&physical62.touch_bindings_.requests==0);
    kcd2vr::door_use46::PhysicalUse62 original62{};assert(kcd2vr::door_use46::physicalUse62.read(original62)&&original62.input.now==sourceTick62);
    kcd2vr::door_signal19c4r4::ready=true;TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.pending&&!g_door_script_owner19c4r4.active&&commands.back().phase==1);
    kcd2vr::door_signal19c4r4::haveAck=true;acknowledge65();Step(20);physical62.tick(now,1);TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.active&&commands.back().fraction==.1f);
    acknowledge65();Step(20);physical62.xr.hands[0].pose.pos_x=.12f;physical62.tick(now,1);TickDoorScript19c4r4();
    assert(commands.back().phase==2&&commands.back().fraction==.1f&&physical62.touch_bindings_.requests==0);
    acknowledge65();Step(20);physical62.tick(now,0);g_right_grip=0;TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.active&&!g_door_script_owner19c4r4.pending&&g_door_script_owner19c4r4.paused&&commands.back().phase==4);
    assert(getCalls==0&&speedCalls==0&&timeCalls==0&&physical62.touch_bindings_.requests==0);
    // Regrip traverses the ACTUAL producer again. Its current physical surface
    // matches the retained native operation, so it publishes Resume instead of
    // a second native Use. A close descriptor by itself cannot resume it.
    candidate.entity=42;std::strcpy(candidate.cls,"AnimDoor");candidate.owner=0;
    acknowledge65();Step(20);candidate.tick=candidate.sourceTick=now;physical62.tick(now,1);
    TickDoorScript19c4r4();
    assert(g_door_script_owner19c4r4.active&&!g_door_script_owner19c4r4.paused&&commands.back().phase==2);
    assert(physical62.touch_bindings_.requests==0&&!physical62.touch_bindings_.held);
    acknowledge65(.9995f,2);Step(20);physical62.tick(now,1);TickDoorScript19c4r4();
    assert(!g_door_script_owner19c4r4.active&&!g_door_script_owner19c4r4.pending&&physical62.touch_bindings_.requests==0);
    Setup();Producer left62;entry62=PhysicalDoorEntry62(left62,true,1);assert(entry62.acceptedUse62.valid);
    ArmDoor19c4(42,0x123,"door_l_f_o",1,now,&entry62);assert(g_door_request19c4.read(r)&&r.serial&&r.side==1);
    Step(20);left62.tick(now,1,1);assert(left62.doorUse46.chosen.target==42&&left62.doorUse46.chosen.side==1&&!left62.touch_bindings_.held);
    // Current committed palm, not the discarded wrist getter, owns reach.
    // A stale/displaced wrist must not refuse the same real near-door palm.
    Setup();Producer palm65;entry62=PhysicalDoorEntry62(palm65);
    currentHand={1.3f,0,0}; // old getter only; copied committed palm remains near the door.
    ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&entry62);assert(g_door_request19c4.read(r)&&r.serial);
    assert(handReads==0&&getCalls==0&&speedCalls==0&&timeCalls==0);
    // Source revocation between native entry and accepted return cannot grant
    // a clock, even if all native retail/target/character pins remain valid.
    for(int bad62=0;bad62<19;++bad62){Setup();Producer p;auto entry=PhysicalDoorEntry62(p,bad62!=0);
        if(bad62==0)assert(!entry.acceptedUse62.valid);
        if(bad62==1){Step(20);p.tick(now,0);}
        if(bad62==2){Step(20);p.tracked=false;p.tick(now,1);}
        if(bad62==3)g_ctx_menu=true;if(bad62==4)g_ctx_dialogue=true;if(bad62==5)g_ctx_cutscene=true;
        if(bad62==6)g_right_grip=0;if(bad62==7)animation_filter::nativeBodyOwned112=true;
        if(bad62==8)g_pose_epoch6689=8;if(bad62==9)g_last_player_entity=nullptr;
        if(bad62==10)++sourceGeneration38;if(bad62==11)handItem=1;
        if(bad62==12){auto current=entry.acceptedUse62.pulse.input;++current.trackingEpoch;kcd2vr::door_use46::currentInput62.publish(current);}
        if(bad62==13){kcd2vr::door_use46::SetEnabled(false);kcd2vr::door_use46::SetEnabled(true);}
        if(bad62>=14){door_surface65::Witnesses w{};door_surface65::witnessMailbox.read(w);
            for(auto& surface:w.value){
                if(bad62==14)surface={}; // Actual world query absent.
                if(bad62==15){surface.distance=.21f;surface.point.y=surface.actualPalm.y+.21f;}
                if(bad62==16)surface.entityId=99; // Another collider cannot stand in for this door.
                if(bad62==17)surface.queryTick=now-81;
                if(bad62==18)++surface.source.press;
            }door_surface65::witnessMailbox.publish(w);}
        ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&entry);g_door_request19c4.read(r);
        assert(!r.serial&&getCalls==0&&speedCalls==0&&timeCalls==0);
    }
    // Independent-review regression: a native Use may return after an
    // experiment/settings/tracking/press family changes and a new Frame has
    // already arrived. It must retain ENTRY identity, never adopt that Frame.
    for(int changed65=0;changed65<5;++changed65){Setup();Producer p;auto entry=PhysicalDoorEntry62(p);
        assert(entry.handSource65);
        if(changed65==0)kcd2vr::workstation178r20c1::experimentEpoch36.fetch_add(1);
        if(changed65==1){kcd2vr::door_use46::SetEnabled(false);kcd2vr::door_use46::SetEnabled(true);}
        if(changed65==2)kcd2vr::tracking59c6::epoch.fetch_add(1);
        FreshHand65();door_hold65::Frame updated{};door_hold65::frame.read(updated);
        if(changed65==2)updated.hand[0].trackingEpoch=kcd2vr::tracking59c6::epoch.load();
        if(changed65==3)++updated.hand[0].press;
        if(changed65==4)++updated.hand[0].sourceGeneration;
        door_hold65::frame.publish(updated);assert(Surface65());
        ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&entry);g_door_request19c4.read(r);
        assert(!r.serial&&getCalls==0&&speedCalls==0&&timeCalls==0);
    }
    // The original receipt cannot outlive CURRENT tracking/input. A missing
    // producer heartbeat still fails at80ms, independently of queue duration.
    Setup();Producer stale62;entry62=PhysicalDoorEntry62(stale62);
    Step(81);
    entry62.candidate.tick=entry62.candidate.sourceTick=now;entry62.acceptedUse62={};CaptureDoorRequest46(entry62,42,0,now);
    assert(!entry62.acceptedUse62.valid);ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&entry62);g_door_request19c4.read(r);assert(!r.serial);
    // Same-target and side identity come only from the native ENTRY witness.
    for(int bad62=0;bad62<5;++bad62){Setup();Producer p;entry62=PhysicalDoorEntry62(p);entry62.acceptedUse62={};
        if(bad62==0)entry62.candidate.entity=99;if(bad62==1)entry62.candidate.owner=1;
        if(bad62==2)entry62.candidate.sourcePlayer=1;if(bad62==3)entry62.candidate.sourceEpoch=8;
        if(bad62==4)entry62.candidate.sourceGeneration=2;
        CaptureDoorRequest46(entry62,42,0,now);assert(!entry62.acceptedUse62.valid);
    }
    // Real C61 XI-A pulse-to-native request delays were100/111ms. They must not
    // be confused with a stale pose: current hand/selection heartbeats remain
    // real and fresh while the immutable original Use edge waits for release.
    for(unsigned transportAge62:{100u,111u,250u,251u,266u,1000u}){Setup();Producer p;auto entry=PhysicalDoorEntry62(p);
        const auto original=entry.acceptedUse62.pulse;unsigned remaining=transportAge62;
        while(remaining){const auto step=remaining>60?60u:remaining;Step(step);p.tick(now,1);remaining-=step;}
        entry.acceptedUse62={};entry.candidate.tick=entry.candidate.sourceTick=now;assert(p.surfaceHit65(42,now));CaptureDoorRequest46(entry,42,0,now);
        const bool admitted=true;assert(entry.acceptedUse62.valid==admitted);
        ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&entry);g_door_request19c4.read(r);assert(bool(r.serial)==admitted);
        kcd2vr::door_use46::PhysicalUse62 unchanged{};assert(kcd2vr::door_use46::physicalUse62.read(unchanged)&&
            unchanged.serial==original.serial&&unchanged.input.now==original.input.now&&unchanged.input.press==original.input.press);
    }
    // A fresh heartbeat after a loss must NOT revive the original receipt,
    // even if the producer is handed the same press/source identity again.
    for(int failure62=0;failure62<8;++failure62){Setup();Producer p;auto entry=PhysicalDoorEntry62(p);
        const auto original=entry.acceptedUse62.pulse;
        Step(failure62==2?81:20);
        if(failure62==0)p.tracked=false;
        if(failure62==3)++p.poseEpoch;
        if(failure62==4)++p.trackingEpoch;
        if(failure62==5)++sourceGeneration38;
        if(failure62==6){door_use46::SetEnabled(false);door_use46::SetEnabled(true);}
        if(failure62==7)p.player+=8;
        p.tick(now,failure62==1?0:1);
        assert(door_use46::retiredPhysical62.load()>=original.serial);
        // Publish a forged fresh same-family snapshot too: sticky native-side
        // revocation does not depend on the controller keeping it invalid.
        auto restored=original.input;restored.now=restored.poseTick=now;
        door_use46::currentInput62.publish(restored);
        auto freshSelection=entry.acceptedUse62.selection;freshSelection.tick=freshSelection.poseTick=now;
        door_use46::AcceptedUse62 denied{};assert(!door_use46::captureAccepted62(freshSelection,now,denied));
        entry.acceptedUse62={};entry.candidate.tick=entry.candidate.sourceTick=now;
        (void)p.surfaceHit65(42,now);CaptureDoorRequest46(entry,42,0,now);
        assert(!entry.acceptedUse62.valid);
        ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&entry);g_door_request19c4.read(r);assert(!r.serial);
    }
    // Published but not yet adopted: release is visible to the native worker
    // immediately. It cannot start/hold a door waiting for the next input loop.
    Setup();Producer revoke62;entry62=PhysicalDoorEntry62(revoke62);ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&entry62);
    assert(g_door_request19c4.read(r)&&r.serial);Step(20);revoke62.tick(now,0);kcd2vr::door_signal19c4r4::ready=true;
    TickDoorScript19c4r4();assert(!g_door_script_owner19c4r4.active&&!g_door_script_owner19c4r4.pending);
    for(const auto& c:commands)assert(c.phase==0);
    // The actual short-pulse transport is joined identically. A successful
    // queue is request evidence only; it still needs accepted native Door.
    Setup();kcd2vr::action_time_owner59c11::nativeGrasp62=true;Producer pulse62;
    pulse62.player=reinterpret_cast<std::uintptr_t>(player.data());const auto pulseDoor62=candidate;
    candidate.entity=99;std::strcpy(candidate.cls,"PickableItem");
    pulse62.tick(now,0);Step(20);candidate.tick=candidate.sourceTick=now;pulse62.tick(now,1);
    Step(20);candidate.tick=candidate.sourceTick=now;pulse62.tick(now,1);
    assert(pulse62.touch_bindings_.requests==0&&!pulse62.touch_bindings_.held);
    // C63 Pickable Grip transport requires a deliberate continuous 150 ms
    // squeeze. Keep native selection/XR heartbeats fresh until that transport;
    // the accepted native Door must still join the exact immutable pulse below.
    for(unsigned wait63:{50u,50u,30u}){Step(wait63);candidate.tick=candidate.sourceTick=now;pulse62.tick(now,1);}
    assert(pulse62.touch_bindings_.requests==1&&!pulse62.touch_bindings_.held);
    entry62={pulseDoor62,7,pulse62.player};entry62.candidate.tick=entry62.candidate.sourceTick=now;
    assert(pulse62.surfaceHit65(42,now));CaptureDoorRequest46(entry62,42,0,now);assert(entry62.acceptedUse62.valid);
    ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&entry62);assert(g_door_request19c4.read(r)&&r.serial);
    Step(20);pulse62.tick(now,1);assert(pulse62.doorUse46.chosen.target==42&&pulse62.touch_bindings_.requests==1);
    // New receipt snapshots the same real Use producer and native entry. It
    // cannot turn a missing held source/transport/palm witness into admission.
    for(int reason63=0;reason63<3;++reason63){
        Setup();diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,now);
        Producer p;p.player=reinterpret_cast<std::uintptr_t>(player.data());
        if(reason63==1)p.touch_bindings_.allow=false;
        if(reason63==2){candidate.entity=99;std::strcpy(candidate.cls,"PickableItem");}
        p.tick(now,0);Step(20);candidate.tick=candidate.sourceTick=now;p.tick(now,reason63?1:0);
        if(reason63){Step(20);candidate.tick=candidate.sourceTick=now;p.tick(now,1);}
        DoorRequestWitness38 w{};w.epoch=7;w.player=p.player;
        if(reason63!=2)w.candidate=candidate;
        CaptureDoorRequest46(w,42,0,now);assert(w.receipt63);
        ArmDoor19c4(42,0x123,"door_l_f_o",0,now,&w);
        const char* expected63=reason63==0?"NO_MATCHING_HELD_SOURCE":reason63==1?"NO_PHYSICAL_USE_TRANSPORT":"ANOTHER_PALM_CANDIDATE";
        bool found63=false;for(const auto& line:receipts)if(line.find("C63 F5_DOOR_JOIN")!=std::string::npos&&line.find(expected63)!=std::string::npos)found63=true;
        assert(found63);g_door_request19c4.read(r);assert(!r.serial);
    }
    // G2: actual native-entry -> private channel -> legacy owner. The old
    // algorithm is separate: it does not borrow the spatial curve proof.
    Setup();kcd2vr::combat_rules178r10::f5Rung=3;kcd2vr::door_signal19c4r4::ready=true;
    std::uint32_t cap69=0;assert(kcd2vr::door_signal19c4r4::channel->field("kcd2vr_door_companion_69",now,cap69)&&cap69==69);
    r=ScriptArm65();assert(r.mode69==3);TickDoorScript19c4r4();
    assert(commands.back().phase==1&&commands.back().mode69==3&&!g_door_script_owner19c4r4.active);
    acknowledge65();Step();TickDoorScript19c4r4();
    assert(g_door_legacy_owner69.active&&commands.back().phase==2&&driveCalls66==0);
    acknowledge65();Step();scriptProducer65->xr.hands[0].pose.pos_x=.15f;scriptProducer65->tick(now,1);TickDoorScript19c4r4();
    assert(g_door_legacy_owner69.active&&commands.back().fraction>.5f&&driveCalls66==0);
    const auto lastSeq69=commands.back().sequence;
    kcd2vr::combat_rules178r10::f5Rung=2;Step();TickDoorScript19c4r4();
    assert(!g_door_legacy_owner69.active&&commands.back().phase==0&&commands.back().sequence>lastSeq69);
    std::puts("native_door_io19c4: actual Use producer, native accepted entry, immutable C62 press handoff, ACK/controller-time/pause/resume/native completion, bounded door join receipts and refusal graph PASS");
}
