#pragma once
#include "../crafting/options_d67.h"
// Pre-pickup station identity and Grip -> native contextual A. This never
// creates an attached Binding or grants a hammer/fixture/recipe result.
#include "../../vrik/smith_tool36.h"
#include "../alchemy/geometry68.h"
#include "../../vrik/workstation_sample46.h"
#include "bellows_geometry_d51.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
namespace kcd2vr::subsystems::smithing::source_h42 {
namespace geometry=::kcd2vr::subsystems::alchemy::geometry68;
inline constexpr unsigned Slot[4]{0x2E0,0x2E8,0x2F0,0x2F8};
inline constexpr std::uintptr_t HolderClass[4]{0x3A7BA48,0x3A7BA48,0x3A7BAA8,0x3A7BA18};
inline constexpr int Hand[4]{0,1,0,1};
enum class Action:unsigned {None,ToAnvil,ToForge};
enum class Geometry:unsigned {None,Bounds,NativeSlotPlacement,ForgeHandle};
struct Source {
 std::uintptr_t instance=0,player=0,model=0,holder=0;
 std::uint64_t key=0;std::uint32_t entity=0,host=0;
 int slot=-1,side=-1,phase=-1;unsigned char attached=0;
 geometry::Bounds bounds{};bool geometryKnown=false;std::uint32_t geometryEntity=0;
 Geometry geometryType=Geometry::None;geometry::Point slotWorld{};
};
inline bool sameIdentity(const Source& a,const Source& b) noexcept {
 return a.entity&&a.instance==b.instance&&a.player==b.player&&a.model==b.model&&a.holder==b.holder&&
  a.key==b.key&&a.entity==b.entity&&a.host==b.host&&a.slot==b.slot&&a.side==b.side&&a.phase==b.phase&&a.attached==b.attached;
}
// Holder +10 is the linked station source ID at B67A4C/B67D90. It exists
// before Attach; holder+14 and Henry's hand slots are deliberately NOT required.
template<class Reader> bool read(Reader&& rd,std::uintptr_t base,std::uintptr_t instance,
 std::uintptr_t player,int slot,Source& out) noexcept {
 out={};if(!base||!instance||!player||slot<0||slot>3)return false;
 Source s{};s.instance=instance;s.player=player;s.slot=slot;s.side=Hand[slot];
 std::uintptr_t v=0,back=0;unsigned char active=0,phase=255;std::uint32_t playerId=0;int side=-1;
 if(!rd(instance,v)||v!=base+smith_tool36::kInstanceVft||!rd(instance+0x2C,active)||active!=1||
    !rd(instance+0x18,back)||back!=player||!rd(player,v)||v!=base+smith_tool36::kPlayerVft||
    !rd(player+0x30,playerId)||!playerId||!rd(instance+0x98,s.model)||!s.model||
    !rd(s.model,v)||v!=base+smith_tool36::kModelVft||!rd(s.model+0x88,phase)||phase>20||
    !rd(s.model+Slot[slot],s.holder)||!s.holder||!rd(s.holder,v)||v!=base+HolderClass[slot]||
    !rd(s.holder+8,side)||side!=s.side||!rd(s.holder+0x10,s.entity)||!s.entity||
    !rd(s.holder+0xC,s.host)||!rd(s.holder+0x14,s.attached)||s.attached>1||
    (s.attached&&s.host!=playerId))return false;
 if(slot<2&&(!rd(s.holder+0x28,s.key)||!s.key))return false;
 s.phase=phase;
 std::uint32_t id=0;std::uint64_t key=0;unsigned char attached=255,againPhase=255;
 if(!rd(instance+0x98,back)||back!=s.model||!rd(s.model+Slot[slot],back)||back!=s.holder||
    !rd(s.holder+0x10,id)||id!=s.entity||!rd(s.holder+0x14,attached)||attached!=s.attached||
    !rd(s.model+0x88,againPhase)||againPhase!=phase||!rd(instance+0x18,back)||back!=player||
    !rd(instance+0x2C,active)||active!=1||(slot<2&&(!rd(s.holder+0x28,key)||key!=s.key)))return false;
 out=s;return true;
}
// The station-linked source can differ from a tool's live inventory
// representation. Resolve only the exact holder WUID through the same retail
// registry/manager proof used by attached smith tools; no attachment is created.
template<class Reader> bool liveSourceRepresentation(Reader&& rd,std::uintptr_t base,
 const Source& s,std::uint32_t& id,smith_tool36::InventoryRepresentation& out) noexcept {
 id=0;out={};if(s.slot<0||s.slot>1||!s.player||!s.key)return false;
 const auto index=std::uint32_t(s.key)&0x3FFFFu;
 if((s.key>>56)!=2||!index||index>0x3D090u)return false;
 std::uintptr_t registry=0,item=0,link=0;std::uint32_t entity=0;
 const auto offset=std::uintptr_t(0x168)+std::uintptr_t(index)*24;
 if(!rd(base+smith_tool36::kRegistryGlobal,registry)||!registry||
    registry>std::numeric_limits<std::uintptr_t>::max()-offset-0x38||
    !rd(registry+offset+0x38,item)||!item||!rd(item+0x88,link)||!link||
    !rd(link+0x30,entity)||!entity)return false;
 const char* why=nullptr;
 if(!smith_tool36::inventoryRepresentation(rd,base,s.player,s.key,entity,out,why))return false;
 id=entity;return true;
}
template<class Reader,class BoundsQuery> bool readGeometry(Reader&& rd,BoundsQuery&& query,
 std::uintptr_t base,Source& s) noexcept {
 s.bounds={};s.geometryKnown=false;s.geometryEntity=0;s.geometryType=Geometry::None;s.slotWorld={};Source after{};geometry::Bounds b{};
 const auto stillSource=[&](){return read(rd,base,s.instance,s.player,s.slot,after)&&sameIdentity(s,after);};
 if(query(s.entity,b)&&stillSource()){
  s.bounds=b;s.geometryEntity=s.entity;s.geometryKnown=true;s.geometryType=Geometry::Bounds;return true;
 }
 std::uint32_t entity=0,again=0;smith_tool36::InventoryRepresentation representation{},recheck{};
 if(!liveSourceRepresentation(rd,base,s,entity,representation)||entity==s.entity||
    !query(entity,b)||!stillSource()||!liveSourceRepresentation(rd,base,s,again,recheck)||
    again!=entity||!smith_tool36::same(representation,recheck))return false;
 s.bounds=b;s.geometryEntity=entity;s.geometryKnown=true;s.geometryType=Geometry::Bounds;return true;
}
// Explicit fallback to the slot's native item placement, not a fabricated AABB
// or the slot entity's origin. The adapter resolves the exact occupied ItemSlot
// before pickup and returns WORLD coordinates including its native local offset.
template<class Reader,class BoundsQuery,class PlacementQuery> bool readGeometry(Reader&& rd,
 BoundsQuery&& bounds,PlacementQuery&& placement,std::uintptr_t base,Source& s) noexcept {
 if(readGeometry(rd,bounds,base,s))return true;
 if(s.slot<0||s.slot>1||s.attached)return false;
 geometry::Point p{};Source after{};
 if(!placement(s,p)||!geometry::finite(p)||
    !read(rd,base,s.instance,s.player,s.slot,after)||!sameIdentity(s,after))return false;
 s.geometryKnown=true;s.geometryEntity=s.entity;s.geometryType=Geometry::NativeSlotPlacement;s.slotWorld=p;return true;
}
inline geometry::Result distance(const Source& s,geometry::Point palm) noexcept {
 if(!s.geometryKnown||!geometry::finite(palm))return {};
 if(s.geometryType==Geometry::Bounds)return geometry::closest(s.bounds,palm);
 if((s.geometryType!=Geometry::NativeSlotPlacement&&s.geometryType!=Geometry::ForgeHandle)||!geometry::finite(s.slotWorld))return {};
 const auto gap=geometry::sub(palm,s.slotWorld);const double squared=geometry::dot(gap,gap);
 if(!std::isfinite(squared))return {};return {true,s.slotWorld,std::sqrt(squared)};
}
struct Frame {
 std::uintptr_t instance=0,player=0,playerEntity=0;std::uint64_t lifetime=0,tick=0,sequence=0,modeEpoch=0;
 int phase=-1;std::array<Source,4> source{};
};
struct Palm {geometry::Point point{};float grip=0;bool valid=false;};
struct Input {Frame frame{};std::uint64_t now=0;std::uintptr_t instance=0;std::uint64_t lifetime=0,modeEpoch=0;
 int phase=-1;bool enabled=false,manualA=false,cancel=false;Palm palm[2]{};};
struct Output {bool a=false;Action action=Action::None;std::uint32_t entity=0;int side=-1,slot=-1;};
// Grip of the authored handle admits physical pumping through the native RT
// action, not contextual A. Native heat, animation and costs remain untouched.
// New gesture tuning: 20 cm acquisition / 30 cm withdrawal, 3 cm downstroke,
// 3 cm return to rearm, 18 mm/s deliberate motion. A stationary lowered hand
// does not hold/repeat RT; motion criteria use elapsed tracking time, not fps.
enum class PumpReasonD42:unsigned {SourceRequired,ManualInput,NewSource,GripOrContact,NewGripRequired,Acquired,Discontinuous,Pumping,Returning,Idle};
inline const char* pumpNameD42(PumpReasonD42 s) noexcept {
 switch(s){case PumpReasonD42::SourceRequired:return "CURRENT_HANDLE_SOURCE_REQUIRED";case PumpReasonD42::ManualInput:return "MANUAL_INPUT_OWNS";
 case PumpReasonD42::NewSource:return "SOURCE_BASELINE";case PumpReasonD42::GripOrContact:return "GRIP_OR_HANDLE_CONTACT_REQUIRED";
 case PumpReasonD42::NewGripRequired:return "FRESH_HANDLE_GRIP_REQUIRED";case PumpReasonD42::Acquired:return "HANDLE_ACQUIRED";
 case PumpReasonD42::Discontinuous:return "TRACKING_DISCONTINUITY";case PumpReasonD42::Pumping:return "DOWNSTROKE_RT_REQUEST";
 case PumpReasonD42::Returning:return "RETURN_STROKE";case PumpReasonD42::Idle:return "NO_DOWNSTROKE";}return "UNKNOWN";
}
struct PumpTraceD42 {
 PumpReasonD42 reason=PumpReasonD42::SourceRequired;bool current=false,sampled=false,edge=false,acquired=false,armed=false,pressed=false,returned=false;
 double gap=-1;float travel=0,relativeTravel=0;std::uint64_t lastMotion=0;
};
class ForgePump {
 Source source_{};std::uint64_t life_=0,mode_=0,pose_=0,epoch_=0,sequence_=0,time_=0,lastMotion_=0;
 bool held_=false,armed_=false,acquired_=false,pressed_=false,returned_=true;
 float lastY_=0,lastRelative_=0,topY_=0,topRelative_=0,bottomY_=0,bottomRelative_=0;
 void cancel() noexcept {acquired_=pressed_=false;lastMotion_=0;}
public:
 bool step(const Input& in,const workstation_sample46::Frame& f,float manualRT,bool contextualA=false,PumpTraceD42* trace=nullptr) noexcept {
  if(trace)*trace={};
  const auto done=[&](PumpReasonD42 why,bool result=false) noexcept {
   if(trace){trace->reason=why;trace->acquired=acquired_;trace->armed=armed_;trace->pressed=pressed_;trace->returned=returned_;trace->lastMotion=lastMotion_;
    trace->travel=topY_-lastY_;trace->relativeTravel=topRelative_-lastRelative_;}return result;
  };
  const auto& s=in.frame.source[2];const auto& hand=f.hand[0];
  const bool sampled=in.palm[0].valid&&workstation_sample46::fresh(f,in.now)&&f.allowed&&
   hand.valid&&workstation_sample46::finite(hand)&&std::isfinite(f.head[1])&&
   std::isfinite(hand.grip)&&hand.grip>=0&&hand.grip<=1;
  const bool down=sampled&&hand.grip>=(held_?.35f:.55f);
  const bool edge=down&&!held_&&armed_;held_=down;
  if(!sampled)armed_=false;else if(hand.grip<.35f)armed_=true;else if(edge)armed_=false;
  const bool current=in.enabled&&!in.cancel&&!in.manualA&&!contextualA&&in.phase==8&&sampled&&
   in.instance&&in.lifetime&&in.modeEpoch&&in.frame.instance==in.instance&&in.frame.lifetime==in.lifetime&&
   in.frame.modeEpoch==in.modeEpoch&&in.frame.phase==in.phase&&in.frame.sequence&&in.frame.tick&&
   in.now>=in.frame.tick&&in.now-in.frame.tick<=80&&
   s.slot==2&&s.side==0&&s.phase==8&&s.instance==in.instance&&s.player==in.frame.player&&s.entity&&
   s.geometryKnown&&s.geometryEntity==s.entity&&s.geometryType==Geometry::ForgeHandle;
  const bool same=life_==in.lifetime&&mode_==in.modeEpoch&&pose_==f.poseEpoch&&epoch_==f.sourceEpoch&&sameIdentity(source_,s);
  if(trace){trace->current=current;trace->sampled=sampled;trace->edge=edge;}
  if(!current||!std::isfinite(manualRT)||manualRT>.10f){
   // A diagnostic manual-RT row used to print gap=-1 even with a current
   // handle/palm, hiding the actual frame/distance comparison. Measure only
   // when an F5 trace was requested; manual input still owns the action.
   if(trace&&current){const auto measured=distance(s,in.palm[0].point);trace->gap=measured.valid?measured.distance:-1;}
   cancel();armed_=sampled&&!down;return done(current?PumpReasonD42::ManualInput:PumpReasonD42::SourceRequired);}
  const float y=hand.position[1],relative=y-f.head[1];
  const auto gap=distance(s,in.palm[0].point);
  if(trace)trace->gap=gap.valid?gap.distance:-1;
  if(!same){cancel();source_=s;life_=in.lifetime;mode_=in.modeEpoch;pose_=f.poseEpoch;epoch_=f.sourceEpoch;
   sequence_=f.sequence;time_=f.timeNs;armed_=!down;return done(PumpReasonD42::NewSource);}
  if(!down||!gap.valid||gap.distance>(acquired_?.30:.20)){cancel();return done(PumpReasonD42::GripOrContact);}
  if(!acquired_){
   // Current exact handle contact plus held Grip may acquire after its source
   // arrives. Acquisition only rebases; a fresh relative 3 cm downstroke is
   // still required, so source/phase changes cannot fabricate a pump.
   if(!edge&&!kcd2vr::crafting_d67::smithRecovery.load(std::memory_order_acquire))return done(PumpReasonD42::NewGripRequired);acquired_=true;returned_=true;
   lastY_=topY_=bottomY_=y;lastRelative_=topRelative_=bottomRelative_=relative;
   sequence_=f.sequence;time_=f.timeNs;return done(PumpReasonD42::Acquired);}
  const bool fresh=f.sequence!=sequence_;
  if(fresh){
   const auto elapsed=f.timeNs>time_?f.timeNs-time_:0;
   const float dy=y-lastY_,dr=relative-lastRelative_;
   const bool continuous=f.sequence>sequence_&&elapsed>=1000000&&elapsed<=80000000&&
    std::fabs(dy)<=.25f&&std::fabs(dr)<=.25f&&std::fabs(dy)<=float(elapsed*1.e-9)*6.f&&std::fabs(dr)<=float(elapsed*1.e-9)*6.f;
   sequence_=f.sequence;time_=f.timeNs;lastY_=y;lastRelative_=relative;
   if(!continuous){cancel();armed_=false;return done(PumpReasonD42::Discontinuous);}
   const float motion=float(double(elapsed)*1.e-9)*.018f;
   // Both raw vertical travel and head-relative travel must agree. Common
   // walking/crouching translation or moving the head alone cannot pump.
   if(returned_){topY_=std::max(topY_,y);topRelative_=std::max(topRelative_,relative);
    if(topY_-y>=.03f&&topRelative_-relative>=.03f&&dy<-motion&&dr<-motion){
     pressed_=true;returned_=false;bottomY_=y;bottomRelative_=relative;lastMotion_=in.now;}
   }else{
    bottomY_=std::min(bottomY_,y);bottomRelative_=std::min(bottomRelative_,relative);
    if(dy<-motion&&dr<-motion&&pressed_)lastMotion_=in.now;
    if(dy>motion&&dr>motion)pressed_=false;
    if(y-bottomY_>=.03f&&relative-bottomRelative_>=.03f){returned_=true;topY_=y;topRelative_=relative;}
   }
  }
  if(pressed_&&(!lastMotion_||in.now<lastMotion_||in.now-lastMotion_>80))pressed_=false;
  return done(pressed_?PumpReasonD42::Pumping:!returned_?PumpReasonD42::Returning:PumpReasonD42::Idle,pressed_);
 }
};
inline Action action(int phase,int slot) noexcept {
 // Exact 858188 phase -> retail defaultProfile.xml action map. Anvil work
 // phases14/16/17 are ongoing strokes, not a new request to travel to the forge.
 if(slot==0&&(phase==7||phase==8))return Action::ToAnvil;
 if((slot==1||slot==3)&&phase==11)return Action::ToForge;
 // ForgeBag is blacksmith_use_forgebag/RT, never contextual A -> ToAnvil.
 return Action::None;
}
// Copied decision facts only; populated solely for an explicit finite capture.
struct Trace {bool current=false,owner=false;unsigned sampled=0,edge=0,armed=0;std::uint64_t pulseUntil=0;};
class Controller {
 std::uintptr_t instance_=0;std::uint64_t lifetime_=0,mode_=0,until_=0;
 bool held_[2]{},armed_[2]{};Output press_{};
public:
 Output step(const Input& in,Trace* trace=nullptr) noexcept {
  const auto& f=in.frame;
  const bool current=in.enabled&&!in.cancel&&in.instance&&in.lifetime&&in.modeEpoch&&
   f.instance==in.instance&&f.lifetime==in.lifetime&&f.modeEpoch==in.modeEpoch&&f.phase==in.phase&&
   f.sequence&&f.tick&&in.now>=f.tick&&in.now-f.tick<=80;
  const bool owner=instance_==in.instance&&lifetime_==in.lifetime&&mode_==in.modeEpoch;
  bool grip[2]{},edge[2]{},sampled[2]{};
  for(int h=0;h<2;++h){const auto& p=in.palm[h];
   sampled[h]=p.valid&&geometry::finite(p.point)&&std::isfinite(p.grip)&&p.grip>=0&&p.grip<=1;
   if(!sampled[h])armed_[h]=false;else if(p.grip<.35f)armed_[h]=true;
   grip[h]=sampled[h]&&p.grip>=(held_[h]?.35f:.55f);
   edge[h]=grip[h]&&!held_[h]&&armed_[h];
   if(grip[h]&&!held_[h])armed_[h]=false;}
  if(trace){*trace={current,owner,0,0,0,until_};for(int h=0;h<2;++h){
   if(sampled[h])trace->sampled|=1u<<h;if(edge[h])trace->edge|=1u<<h;if(armed_[h])trace->armed|=1u<<h;}}
  if(!current||!owner||in.manualA){until_=0;press_={};
   for(int h=0;h<2;++h){held_[h]=grip[h];armed_[h]=sampled[h]&&in.palm[h].grip<.35f;}
   instance_=in.instance;lifetime_=in.lifetime;mode_=in.modeEpoch;return {};}
  if(until_&&in.now>=until_){until_=0;press_={};}
  // Physical Grip must be released before another A request. A phase change
  // from our own press cannot turn that same squeeze into ToForge/ToAnvil again.
  double best=.2000001;const Source* chosen=nullptr;
  for(const auto& s:f.source){if(s.slot<0||s.slot>3||s.side<0||s.side>1||!edge[s.side]||
    s.instance!=f.instance||s.player!=f.player||s.phase!=f.phase||!s.entity||!s.geometryKnown||
    action(in.phase,s.slot)==Action::None)continue;
   const auto d=distance(s,in.palm[s.side].point);
   if(d.valid&&d.distance<best){best=d.distance;chosen=&s;}}
  for(int h=0;h<2;++h)held_[h]=grip[h];
  if(chosen&&!until_){press_={true,action(in.phase,chosen->slot),chosen->entity,chosen->side,chosen->slot};until_=in.now+80;}
  if(until_&&in.now<until_&&press_.side>=0&&grip[press_.side])return press_;
  until_=0;press_={};return {};
 }
};
}
