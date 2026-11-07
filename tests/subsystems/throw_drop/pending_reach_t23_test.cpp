#include "subsystems/throw_drop/native_ammo_service_t22.h"
#include "vrik/physical_weapon.h"
#include "subsystems/throw_drop/pending_reach_t23.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <limits>
namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
namespace heldT4=kcd2vr::native_weapon_body_t4;
namespace t23=kcd2vr::throw_admission_t23;
namespace equipment114=kcd2vr::equipment114;
namespace tracking59c6=kcd2vr::tracking59c6;
using kcd2vr::tracking59c6::epoch;
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
struct Stamp {std::uint64_t epoch=0,sample=0,tick=0;std::uintptr_t actor=0;};
struct CommittedFrame6695 {Stamp stamp{};QuatT28 allowed[2]{};float world[8]{0,0,0,1,0,0,0,1};bool valid[2]{true,true};std::uintptr_t doorPlayer38=0;std::uint64_t doorTrackingEpoch66=0;};
struct PhysEquipmentItem6698 {ph::Profile profile{};int set=-1,slot126=-1;bool held=false;};
struct PhysEquipment6698 {std::array<PhysEquipmentItem6698,8> items{};int count=0,selected=-1;void* player=nullptr;void* manager=nullptr;std::uint64_t epoch=0,request=0,handKey[2]{};bool valid=false,handKnown[2]{};};
struct PhysFrame6698 {ph::Input input{};ph::Output output{};void* player=nullptr;void* manager=nullptr;std::uint64_t request=0;float world[8]{0,0,0,1,0,0,0,1};std::uint64_t trackingEpochT23=0,characterT23=0,publicationT23=0;};
// Explicit native-held-item leaf: these fixtures exercise stock/empty hands.
static bool PhysSupplyHeldT2(const PhysEquipment6698&,const PhysEquipmentItem6698&,int) noexcept{return false;}
static unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL line%d: %s\n",__LINE__,#x);std::abort();}}while(0)
alignas(16) static unsigned char entityBytes[64]{},characterBytes[32]{};
static std::atomic<void*> g_last_player_entity{entityBytes},g_henry_char_attachment_manager{characterBytes+8};
static bool feature=true,packetRead=true,entityValid=true,identity=true,tracked[2]{true,true};
static bool busy=false,ownerKnown=true;static unsigned physicsControl=1,retireCalls=0;
static std::uint64_t physicsAdmission=4;
static heldT4::Owner retained{};
static CommittedFrame6695 packet{};
static kcd2vr::supply_t2::Input lastInput{};
static std::uint64_t completedPublication=2,publicationAfterRead=0;
static void* HenryCharacter59c15(){return characterBytes;}
static bool PlayerIdentityAccepts37(void* p,std::uint64_t){return identity&&p==characterBytes;}
static constexpr std::uintptr_t kCEntityVftRva=0x4072C78;
static bool PhysVft6698(void* p,std::uintptr_t){return entityValid&&p==entityBytes;}
template<class T> static bool Read(const void* p,T& out){std::memcpy(&out,p,sizeof(out));return true;}
static bool TrackingHandUsable111(int side,std::uint64_t){return tracked[side];}
static bool ReadCommittedPacket6695(CommittedFrame6695& out){out=packet;if(publicationAfterRead)completedPublication=publicationAfterRead;return packetRead;}
static ph::Pose PhysPose6698(const QuatT28& q){return {{q.q[0],q.q[1],q.q[2],q.q[3]},{q.t[0],q.t[1],q.t[2]}};}
static QuatT28 HandWorld6694(const QuatT28& p,const float* world){
 const auto w=ph::compose({{world[0],world[1],world[2],world[3]},{world[4],world[5],world[6]}},{{p.q[0],p.q[1],p.q[2],p.q[3]},{p.t[0]*world[7],p.t[1]*world[7],p.t[2]*world[7]}});
 return {{float(w.q.x),float(w.q.y),float(w.q.z),float(w.q.w)},{float(w.p.x),float(w.p.y),float(w.p.z)}};
}
// Existing public helper deliberately offers another 80ms-fresh sample; the
// frozen old producer cannot join it to PhysFrame's original source sample.
static bool ReadGrabHandWorld60(int,float pos[3],float q[4],std::uint64_t& body,std::uint64_t& tracking,std::uintptr_t& actor,std::uint64_t& tick){
 pos[0]=9;pos[1]=pos[2]=0;q[0]=q[1]=q[2]=0;q[3]=1;body=packet.stamp.epoch;tracking=packet.doorTrackingEpoch66;actor=reinterpret_cast<std::uintptr_t>(entityBytes);tick=packet.stamp.tick;return packetRead;
}
namespace kcd2vr::action_time_owner59c11 {
bool ThrowingSupplyT2() noexcept{return feature;}
void PublishThrowingSupplyT2(const supply_t2::Input& in) noexcept{lastInput=in;}
}
// Real release-mode header replaces the former constant-true leaf.
namespace kcd2vr::native_weapon_body_t4 {
void SetThrowAdmissionSuspendedT23(bool value) noexcept{if(value)physicsControl|=8;else physicsControl&=~8u;}
bool ThrowAdmissionSuspendedT23() noexcept{return (physicsControl&8)!=0;}
bool HasUnretiredCollider() noexcept{return busy;}
std::uint64_t AdmissionSerial() noexcept{return physicsAdmission;}
bool ReadRetainedOwner(Owner& out) noexcept{out=retained;return busy&&ownerKnown;}
void Retire(const Owner& owner) noexcept{CHECK(owner==retained);++retireCalls;/* only request, no retirement ACK */}
}
namespace legacy {
#include "frozen_native_supply_publication.inl"
}
namespace current {
std::atomic<float> g_holster_radius136{.24f};
static std::uint64_t ReadCompletedPublicationT23() noexcept{return completedPublication;}
#include "subsystems/throw_drop/native_supply_publication.inl"
#include "draw_candidate_t23.inc"
}
namespace unknown_old {
std::atomic<float> g_holster_radius136{.24f};
static std::uint64_t ReadCompletedPublicationT23() noexcept{return completedPublication;}
#include "unknown_as_absent_t23.inc"
#include "draw_candidate_t23.inc"
}
namespace publication_old {
std::atomic<float> g_holster_radius136{.24f};
static std::uint64_t ReadCompletedPublicationT23() noexcept{return completedPublication;}
#include "publication_unjoined_t23.inc"
#include "draw_candidate_t23.inc"
}
static PhysFrame6698 frame{};static PhysEquipment6698 equipment{};static std::uint64_t now=1000;
static void setup(){
 frame={};equipment={};packet={};feature=packetRead=entityValid=identity=true;tracked[0]=tracked[1]=true;
 kcd2vr::ammo_carry_t18::hands.store(0);
 completedPublication=2;publicationAfterRead=0;
 const unsigned id=77;std::memcpy(entityBytes+12,&id,sizeof(id));
 frame.player=equipment.player=reinterpret_cast<void*>(0x1110);frame.manager=equipment.manager=reinterpret_cast<void*>(0x2220);
 frame.request=equipment.request=frame.input.epoch=8;frame.input.bodyEpoch=equipment.epoch=3;
 frame.input.sample=10;frame.input.tick=now;frame.input.allowed=true;frame.input.tracked[0]=frame.input.tracked[1]=true;
 frame.input.intentTracked[0]=frame.input.intentTracked[1]=true;equipment.valid=true;equipment.handKnown[0]=equipment.handKnown[1]=true;
 frame.input.shoulders[0]={0,0,1.5};frame.input.shoulders[1]={-.4,0,1.5};
 const auto a=ph::body_axes(frame.input);frame.input.hands[0].p=frame.input.shoulders[0]-a.up*.50+a.right*.03+a.forward*.13;
 frame.input.hands[1].p={2,0,0};
 frame.trackingEpochT23=epoch.load();frame.characterT23=reinterpret_cast<std::uintptr_t>(characterBytes);frame.publicationT23=completedPublication;
 packet.stamp={3,10,now,reinterpret_cast<std::uintptr_t>(characterBytes)};packet.doorPlayer38=reinterpret_cast<std::uintptr_t>(entityBytes);packet.doorTrackingEpoch66=epoch.load();
 packet.allowed[0].t[0]=1;packet.allowed[1].t[0]=2;
 kcd2vr::inventory_lifecycle_t8::daggers.store(5);kcd2vr::inventory_lifecycle_t8::axes.store(3);
 current::g_supply_admission_t23[0]={};current::g_supply_admission_t23[1]={};
}
static void publish(){current::PhysPublishSupplyT2(frame,equipment,true,now);}
static void advance(){++frame.input.sample;++packet.stamp.sample;frame.input.tick=packet.stamp.tick=++now;}
static t23::Pending request(std::uint64_t serial,int side=0){return {lastInput,serial,side,0};}
int main(){
 kcd2vr::weapon_release_t1::SetMode(1);
 setup();++packet.stamp.sample;legacy::PhysPublishSupplyT2(frame,equipment,true,now);
 CHECK(lastInput.pose18[0]&&lastInput.wrist18[0][4]==9); // frozen RED: mixed fresh source admitted
 publish();CHECK(!lastInput.allowed&&!lastInput.pose18[0]);
 setup();legacy::PhysPublishSupplyT2(frame,equipment,true,now);CHECK(!lastInput.pouchNear[0]); // frozen RED: F9-final grant empty
 publish();CHECK(lastInput.allowed&&lastInput.pose18[0]&&lastInput.wrist18[0][4]==1&&lastInput.poseTick18[0]==frame.input.tick);
 CHECK(lastInput.pouchNear[0]&&lastInput.near18[0][0]);
 t23::Source proof{};CHECK(t23::CurrentSource(lastInput,now,&proof)&&proof.tracking==epoch.load());
 setup();completedPublication=4;packet.allowed[0].t[0]=8;publication_old::PhysPublishSupplyT2(frame,equipment,true,now);
 CHECK(lastInput.allowed&&lastInput.wrist18[0][4]==8); // controlled RED: same XR stamp/world, newer native paired publication
 publish();CHECK(!lastInput.allowed&&!lastInput.pose18[0]);
 setup();publicationAfterRead=4;publish();CHECK(!lastInput.allowed); // concurrent paired replacement refuses
 setup();frame.publicationT23=0;publish();CHECK(!lastInput.allowed);
 setup();completedPublication=frame.publicationT23=3;publish();CHECK(!lastInput.allowed); // in-flight publication is never complete
 for(int cause=0;cause<12;++cause){setup();
  if(cause==0)++packet.stamp.sample;if(cause==1)++packet.stamp.epoch;if(cause==2)++packet.stamp.tick;
  if(cause==3)++packet.stamp.actor;if(cause==4)++packet.doorTrackingEpoch66;if(cause==5)++frame.characterT23;
  if(cause==6)++frame.trackingEpochT23;if(cause==7)++packet.doorPlayer38;if(cause==8)packet.world[4]=.01f;
  if(cause==9)frame.trackingEpochT23=0;if(cause==10)frame.characterT23=0;if(cause==11)now+=81;
  publish();CHECK(!lastInput.allowed&&!lastInput.pose18[0]);
 }
 setup();equipment.handKnown[0]=false;publish();CHECK(!lastInput.pouchNear[0]);auto bad=request(1);CHECK(!t23::PublishPending(bad));
 setup();equipment.handKey[0]=123;publish();CHECK(!lastInput.pouchNear[0]);bad=request(2);CHECK(!t23::PublishPending(bad));
 setup();kcd2vr::inventory_lifecycle_t8::daggers.store(0);publish();CHECK(!lastInput.pouchNear[0]);
 setup();equipment.count=1;equipment.selected=0;auto& item=equipment.items[0];item.profile.classId=3;item.profile.side=0;item.profile.key=88;item.profile.generation=9;item.set=item.slot126=0;
 const auto axes=ph::body_axes(frame.input);frame.input.hands[0].p=frame.input.shoulders[0]-axes.up*.50+axes.right*.03;publish();CHECK(!lastInput.pouchNear[0]);
 frame.input.hands[0].p=frame.input.hands[0].p+axes.forward*.065;advance();publish();CHECK(!lastInput.pouchNear[0]); // actual normal Equipment-vs-stock ambiguity
 setup();frame.input.grip[1]=1;publish();
 kcd2vr::ammo_carry_t18::Claim heldLeft{};heldLeft.player=lastInput.player;heldLeft.manager=lastInput.manager;heldLeft.character=lastInput.character;
 heldLeft.body=lastInput.body;heldLeft.request=lastInput.request;heldLeft.serial=400;heldLeft.stage=kcd2vr::ammo_carry_t18::Stage::Held;
 CHECK(kcd2vr::ammo_carry_t18::claims[1].publish(heldLeft));kcd2vr::ammo_carry_t18::hands.store(2);
 frame.input.grip[0]=1;advance();publish();CHECK(lastInput.pouchNear[0]&&!lastInput.hand[0]&&!lastInput.hand[1]);
 setup();frame.input.grip[1]=1;publish();++heldLeft.manager;heldLeft.serial=401;
 CHECK(kcd2vr::ammo_carry_t18::claims[1].publish(heldLeft));kcd2vr::ammo_carry_t18::hands.store(2);
 // A foreign LEFT claim cannot turn RIGHT Grip-only utility into a fist.
 frame.input.grip[0]=1;advance();publish();CHECK(lastInput.pouchNear[0]);
 frame.input.trigger[1]=1;advance();publish();CHECK(lastInput.pouchNear[0]);
 frame.input.trigger[0]=1;advance();publish();CHECK(!lastInput.pouchNear[0]&&!lastInput.near18[0][0]);
 setup();publish();frame.input.grip[0]=1;advance();publish();frame.input.hands[0].p=frame.input.hands[0].p+axes.forward*.2;advance();publish();CHECK(lastInput.pouchNear[0]);
 const auto p=request(100);CHECK(t23::PublishPending(p));CHECK(t23::PublishPending(p));
 retained={};retained.key=99;retained.generation=7;retained.side=0;retained.player=p.owner.player;retained.manager=p.owner.manager;retained.character=p.owner.character;retained.epoch=p.owner.body;retained.request=p.owner.request;
 busy=true;physicsControl=1|4;retireCalls=0;publish();CHECK(retireCalls==1&&heldT4::ThrowAdmissionSuspendedT23()&&(physicsControl&4)&&(physicsControl&1));
 CHECK(!t23::CurrentGrant(p,lastInput,now)); // cancel/retire request not completion
 t23::Source exactSource{};CHECK(t23::CurrentSource(lastInput,now,&exactSource));unknown_old::PhysSupplyAdmissionTickT23(exactSource);
 {t23::Writer writer(0);CHECK(bool(writer));t23::Pending copy{};CHECK(t23::ObservePending(0,copy)==t23::PendingState::Unknown);
  unknown_old::PhysSupplyAdmissionTickT23(exactSource);CHECK(!(physicsControl&8)); // controlled RED: UNKNOWN falsely clears fence
  physicsControl|=8;
  const auto calls=retireCalls;publish();CHECK((physicsControl&8)&&current::g_supply_admission_t23[0].captured&&retireCalls==calls);
  CHECK(!t23::CurrentGrant(p,lastInput,now));CHECK(!t23::ClearPending(p));CHECK(!t23::PublishPending(p));}
 CHECK(t23::ObservePending(0,bad)==t23::PendingState::Present);
 busy=false;advance();publish();t23::Grant grant{};CHECK(t23::CurrentGrant(p,lastInput,now,&grant));CHECK(grant.admission==physicsAdmission&&grant.pending.serial==p.serial);
 auto mixed=lastInput;mixed.wrist18[0][4]+=1;CHECK(!t23::CurrentGrant(p,mixed,now));
 mixed=lastInput;++mixed.sample;CHECK(!t23::CurrentGrant(p,mixed,now));
 mixed=lastInput;++mixed.tick;CHECK(!t23::CurrentGrant(p,mixed,now));
 mixed=lastInput;++mixed.poseTick18[0];CHECK(!t23::CurrentGrant(p,mixed,now));
 CHECK(!t23::CurrentGrant(p,lastInput,now+81));
 ++physicsAdmission;CHECK(!t23::CurrentGrant(p,lastInput,now));publish();CHECK(!t23::CurrentGrant(p,lastInput,now));
 CHECK(t23::ClearPending(p));CHECK(t23::ClearPending(p));publish();CHECK(!(physicsControl&8)&&(physicsControl&4)&&(physicsControl&1));CHECK(!t23::PublishPending(p));
 setup();publish();auto next=request(101);CHECK(t23::PublishPending(next));CHECK(!t23::ClearPending(p));
 busy=true;retained.side=1;retireCalls=0;publish();CHECK(!retireCalls&&!t23::CurrentGrant(next,lastInput,now));CHECK(t23::ClearPending(next));
 setup();publish();auto left=request(200,1);CHECK(t23::PublishPending(left));physicsControl=1|4;busy=true;retireCalls=0;publish();CHECK(t23::CurrentGrant(left,lastInput,now)&&!retireCalls&&!(physicsControl&8));CHECK(t23::ClearPending(left));
 setup();publish();auto final=request(102);CHECK(t23::PublishPending(final));busy=false;publish();t23::Grant captured{};CHECK(t23::CurrentGrant(final,lastInput,now,&captured));
 const auto original=lastInput;completedPublication+=2;packet.allowed[0].t[0]=7;publish();CHECK(!lastInput.allowed); // latest old PhysFrame cannot borrow new wrists
 CHECK(t23::ValidateGrant(final,original,now,captured)); // new unrelated publication does not revoke original immutable lease
 CHECK(t23::ValidateGrant(final,original,now+80,captured)&&!t23::ValidateGrant(final,original,now+81,captured));
 auto wrong=captured;++wrong.publication;CHECK(!t23::ValidateGrant(final,original,now,wrong));
 auto newWrist=original;newWrist.wrist18[0][4]=7;CHECK(!t23::ValidateGrant(final,newWrist,now,captured));
 const auto sourceFrame=frame;const auto sourceEquipment=equipment;
 for(int cause=0;cause<11;++cause){frame=sourceFrame;equipment=sourceEquipment;tracked[0]=true;
  if(cause==0){current::PhysPublishSupplyT2(frame,equipment,false,now);} // actual current native context denied
  else{
   if(cause==1)frame.input.allowed=false;
   if(cause==2)frame.input.tracked[0]=false;
   if(cause==3)tracked[0]=false;
   if(cause==4)equipment.handKnown[0]=false;
   if(cause==5)equipment.handKey[0]=444;
   if(cause==6)equipment.player=reinterpret_cast<void*>(0x3330);
   if(cause==7)equipment.manager=reinterpret_cast<void*>(0x4440);
   if(cause==8)frame.request=equipment.request=frame.input.epoch=9;
   if(cause==9)frame.input.bodyEpoch=equipment.epoch=4;
   if(cause==10)equipment.count=999;
   publish();
  }
  CHECK(!t23::ValidateGrant(final,original,now,captured));
 }
 frame=sourceFrame;equipment=sourceEquipment;tracked[0]=true;publish();CHECK(t23::ValidateGrant(final,original,now,captured));
 CHECK(!t23::liveWriter.test_and_set());CHECK(!t23::PublishLive({})); // actual failed newer Live publication
 CHECK(!t23::ValidateGrant(final,original,now,captured)); // cannot borrow older allowed facts after UNKNOWN update
 t23::liveWriter.clear();publish();CHECK(t23::ValidateGrant(final,original,now,captured));
 CHECK(t23::PublishLive({}));CHECK(!t23::ValidateGrant(final,original,now,captured));publish();
 CHECK(!t23::LiveCurrent(original,0,now+81));
 now=original.tick+81;publish();CHECK(t23::LiveCurrent(original,0,now)&&!t23::ValidateGrant(final,original,now,captured)); // fresh facts never renew old pose deadline
 ++epoch;CHECK(!t23::ValidateGrant(final,original,now,captured));CHECK(t23::ClearPending(final));
 std::printf("{\"checks\":%u,\"passed\":true,\"old_mixed_source_red\":true,\"old_f9_zero_grant_red\":true,\"unknown_as_absent_red\":true,\"same_xr_new_publication_red\":true,\"dual_hand_claim_green\":true,\"native_execution\":false}\n",checks);return 0;
}
