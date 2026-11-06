#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include "geometry68.h"
#include "reach_policy_craft1.h"

// Pure source arbitration. All table references and points must come from the
// same native AlchemyItem table in a PRIOR completed native frame. This code
// never discovers geometry, guesses a target type, or requests a native action.
namespace kcd2vr::subsystems::alchemy::near67 {
inline constexpr unsigned Capacity=21;
inline constexpr double Enter=.10,Leave=.14;
inline constexpr std::uint64_t MaxAge=80;
struct Point {double x=0,y=0,z=0;};
inline bool finite(Point p){return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);}
inline double distance(Point a,Point b){const double x=a.x-b.x,y=a.y-b.y,z=a.z-b.z;return std::sqrt(x*x+y*y+z*z);}
struct Context {
 std::uintptr_t instance=0,player=0;
 std::uint64_t lifetime=0,poseEpoch=0,sourceEpoch=0,generation=0,tableGeneration=0;
};
inline bool valid(const Context& c){return c.instance&&c.player&&c.lifetime&&c.poseEpoch&&c.sourceEpoch&&c.generation&&c.tableGeneration;}
inline bool same(const Context& a,const Context& b){return a.instance==b.instance&&a.player==b.player&&a.lifetime==b.lifetime&&
 a.poseEpoch==b.poseEpoch&&a.sourceEpoch==b.sourceEpoch&&a.generation==b.generation&&a.tableGeneration==b.tableGeneration;}
struct Ref {int slot=-1;std::uint32_t entity=0;}; // native registered table index; no action semantics
inline bool valid(Ref r){return r.slot>=0&&r.slot<int(Capacity)&&r.entity;}
inline bool same(Ref a,Ref b){return a.slot==b.slot&&a.entity==b.entity;}
struct Offer {
 Ref ref{};Point point{};std::uint64_t tick=0;bool valid=false;
 // G1R1: copied retail LOCAL bounds + WORLD transform. `point` is retained
 // only for old pure/copy fixtures; it cannot authorize a live palm override.
 geometry68::Bounds bounds{};bool boundsValid=false;
};
inline geometry68::Result closest(const Offer& o,Point palm) noexcept {
 if(o.boundsValid)return geometry68::closest(o.bounds,{palm.x,palm.y,palm.z});
 if(!finite(o.point)||!finite(palm))return {};
 return {true,{o.point.x,o.point.y,o.point.z},distance(palm,o.point)};
}
inline double distanceToOffer(const Offer& o,Point palm) noexcept {
 const auto c=closest(o,palm);return c.valid?c.distance:std::numeric_limits<double>::infinity();
}
struct Completed {
 Context context{};std::uint64_t tick=0,sequence=0;
 std::array<Ref,Capacity> table{};
 std::array<Offer,Capacity> offers{};unsigned count=0;bool complete=false;
};
struct Palm {
 Point point{};std::uint64_t tick=0,poseEpoch=0,sourceEpoch=0,generation=0;
 int side=-1;double grip=0;bool valid=false;
};
struct AimH21 {Point direction{};std::uint64_t tick=0,poseEpoch=0,sourceEpoch=0,generation=0;int side=-1;bool valid=false;};
inline bool mortarDestinationH21(int slot) noexcept {return slot==0||slot==6;} // proven native PlaceMortar resolver
struct Selected {Context context{};Ref ref{};std::uint64_t tick=0;int side=-1;bool valid=false;};
struct Input {
 Context context{};std::uint64_t now=0,currentFrameSequence=0;
 std::array<Palm,2> palms{};Selected selected{};
 // Optional source-acquisition policy. Existing generic and already admitted
 // destination callers retain ALL; no native action semantics live here.
 std::array<std::uint32_t,2> acquisitionMaskH5{{(1u<<Capacity)-1,(1u<<Capacity)-1}};
 // Internal policy selected by the actual Free-Alchemy adapter; not another INI
 // mode. Legacy pure callers keep their original 10/14 cm contract.
 bool proximityCraft1=false,selectionFrozenCraft1=false;
 bool handsOnlyD53=false; // mode3:20cm and current empty hands suppress station gaze
 bool sourceAcquisitionD39=false;
 int pickupReachD39=int(reach_craft1::PickupReachD39::Assisted);
};
inline bool acquisitionAllowedH5(const Input& in,int side,Ref ref) noexcept {
 return side>=0&&side<2&&valid(ref)&&(in.acquisitionMaskH5[unsigned(side)]&(1u<<unsigned(ref.slot)))!=0&&
  (!in.proximityCraft1||reach_craft1::allowed(ref.slot,side));
}
// D44: the carafe is already the current native-selected target. A
// deliberate matching hand Grip may use the existing local assisted region,
// even in contact pickup mode. This does not promote a nearby different item,
// change idle hover, other source slots, work/destination reach or manufacture selection.
inline bool selectedPickupD44(const Input& in,Ref ref,int side) noexcept {
 if(!in.proximityCraft1||!in.sourceAcquisitionD39||side!=1||!valid(ref)||ref.slot<13||ref.slot>16)return false;
 const auto& p=in.palms[unsigned(side)];const auto& s=in.selected;
 return p.valid&&p.side==side&&finite(p.point)&&std::isfinite(p.grip)&&p.grip>=.55&&p.grip<=1&&
  p.tick&&in.now>=p.tick&&in.now-p.tick<=MaxAge&&p.poseEpoch==in.context.poseEpoch&&
  p.sourceEpoch==in.context.sourceEpoch&&p.generation==in.context.generation&&
  s.valid&&same(s.context,in.context)&&same(s.ref,ref)&&(s.side==-1||s.side==side)&&
  s.tick&&in.now>=s.tick&&in.now-s.tick<=MaxAge&&acquisitionAllowedH5(in,side,ref);
}
inline double region(const Input& in,Ref ref,int side) noexcept {
 if(in.proximityCraft1&&in.handsOnlyD53)return reach_craft1::handsRadiusD53(ref.slot,side);
 return in.sourceAcquisitionD39&&!selectedPickupD44(in,ref,side)?
  reach_craft1::pickupRadiusD39(ref.slot,side,in.pickupReachD39):reach_craft1::radius(ref.slot,side);
}
inline bool contains(const Input& in,Ref ref,int side,double gap) noexcept {
 return reach_craft1::allowed(ref.slot,side)&&std::isfinite(gap)&&gap>=0&&gap<=region(in,ref,side)+reach_craft1::Tolerance;
}
inline double enter(const Input& in,Ref ref,int side) noexcept {
 return in.proximityCraft1?region(in,ref,side)+reach_craft1::Tolerance:Enter;
}
inline double leave(const Input& in,Ref ref,int side) noexcept {
 return in.proximityCraft1?region(in,ref,side)+reach_craft1::Tolerance:Leave;
}
enum class Source {Gaze=0,Right=1,Left=2};
struct Decision {
 Source source=Source::Gaze;Ref ref{};double gap=0;
 std::uint64_t frameSequence=0,frameTick=0,latchedTick=0;
 bool hapticEdge=false,gripping=false,admissionReady=false;
 bool proximityCraft1=false;
 bool handsOnlyD53=false; // consumer must recheck actual current mode3
 bool sourceAcquisitionD39=false;
 int pickupReachD39=int(reach_craft1::PickupReachD39::Assisted);
 bool gripSelectionOwnedCraft1=false; // current held hand may own an empty selection; never grants Use
 bool selectedPickupD44=false; // consumer must independently re-read the same native-selected entity
 bool nativeAgreeW4=false; // retired in R8b (always false; kept for the G2 receipt field)
 bool palmRayD70=false;
 bool selfReturnRecipientD74=false; // release to the same native-owned logical ingredient supply
 bool pourRecipientD72=false; // exact held native source; current recipient bounds rechecked by consumer
 bool aimedDestinationH21=false,aimReleaseH21=false;Ref heldSourceH21{};AimH21 aimH21{};
 Palm palm{}; // actual WORLD palm provenance; no XR/world coordinate mixing
 std::array<Ref,Capacity> table{}; // exact completed topology behind this decision
};
inline bool fresh(std::uint64_t tick,std::uint64_t now){return tick&&now>=tick&&now-tick<=MaxAge;}
inline bool registered(const Completed& f,Ref r){return valid(r)&&same(f.table[unsigned(r.slot)],r);}
inline bool usable(const Palm& p,int side,const Input& in){return p.valid&&p.side==side&&finite(p.point)&&
 fresh(p.tick,in.now)&&p.poseEpoch==in.context.poseEpoch&&p.sourceEpoch==in.context.sourceEpoch&&p.generation==in.context.generation&&
 std::isfinite(p.grip)&&p.grip>=0&&p.grip<=1;}
inline bool usable(const Offer& p,const Completed& f,std::uint64_t now){return p.valid&&registered(f,p.ref)&&
 (p.boundsValid?geometry68::valid(p.bounds):finite(p.point))&&
 fresh(p.tick,now)&&p.tick<=f.tick;}
// D70: palm direction, up to20cm forward and at most5cm sideways. Exact
// native bounds remain the geometry. The nearest point is a conservative
// corridor witness; the exact ray handles large boxes whose nearest face is
// off-axis. No extra native read or unbounded search is introduced.
inline bool palmCorridorD70(const Offer& o,const Palm& p,const AimH21& aim,std::uint64_t now) noexcept {
 if(!o.boundsValid||!aim.valid||aim.side!=p.side||aim.tick!=p.tick||
    aim.poseEpoch!=p.poseEpoch||aim.sourceEpoch!=p.sourceEpoch||aim.generation!=p.generation||
    !fresh(aim.tick,now)||!finite(aim.direction))return false;
 const geometry68::Point origin{p.point.x,p.point.y,p.point.z},direction{aim.direction.x,aim.direction.y,aim.direction.z};
 const double norm=std::sqrt(geometry68::dot(direction,direction));if(!std::isfinite(norm)||norm<.99||norm>1.01)return false;
 if(geometry68::ray(o.bounds,origin,direction,.20).valid)return true;
 // Cheap conservative rejection before the27-face closest-point solver.
 // A corridor hit must lie in both these worldAABBs; overlap alone never admits.
 const auto& b=o.bounds;const geometry68::Point center=geometry68::transform(b,
  {(double(b.local[0])+b.local[3])*.5,(double(b.local[1])+b.local[4])*.5,(double(b.local[2])+b.local[5])*.5});
 const double half[3]{(double(b.local[3])-b.local[0])*.5,(double(b.local[4])-b.local[1])*.5,(double(b.local[5])-b.local[2])*.5};
 const auto end=geometry68::add(origin,geometry68::mul(direction,.20/norm));
 const double c[3]{center.x,center.y,center.z},a[3]{origin.x,origin.y,origin.z},e[3]{end.x,end.y,end.z};
 for(int j=0;j<3;++j){const double extent=std::abs(b.world[4*j])*half[0]+std::abs(b.world[4*j+1])*half[1]+std::abs(b.world[4*j+2])*half[2];
  if(c[j]+extent<(std::min)(a[j],e[j])-.050001||c[j]-extent>(std::max)(a[j],e[j])+.050001)return false;}
 const auto hit=closest(o,p.point);if(!hit.valid)return false;
 const auto delta=geometry68::sub(hit.point,origin),unit=geometry68::mul(direction,1./norm);
 const double along=geometry68::dot(delta,unit);
 return along>=-1.e-6&&along<=.20+1.e-6&&
  geometry68::dot(delta,delta)-along*along<=.05*.05+1.e-9;
}
// D72: releasing/overturning contents is a gravity-directed destination,
// not the source-acquisition palm ray. Rotating a held vessel must not rotate
// its intended recipient away. The closest point of CURRENT native bounds
// must be below the palm, within20cm vertically and5cm laterally. This is a
// conservative bounds nomination, not an invented vessel mouth or effect.
inline bool pourRecipientD72(const Offer& o,const Palm& p) noexcept {
 if(!o.boundsValid||!p.valid||!finite(p.point)||(o.ref.slot!=0&&o.ref.slot!=2&&o.ref.slot!=6))return false;
 const auto recipientPointD72=closest(o,p.point);if(!recipientPointD72.valid)return false;
 const double x=recipientPointD72.point.x-p.point.x,y=recipientPointD72.point.y-p.point.y,z=p.point.z-recipientPointD72.point.z;
 return z>=-.005&&z<=.20&&x*x+y*y<=.05*.05+1.e-9;
}
// Destination nomination only, after an actual mortar attachment pair. It is
// never source acquisition, a native action/effect or a replacement gaze ray.
inline Decision mortarAimH21(const Completed& f,const Input& in,const AimH21& aim,Ref held,bool releaseEdge) noexcept {
 Decision out{};const int side=1;const auto& p=in.palms[side];
 if(!valid(in.context)||!f.complete||!same(f.context,in.context)||!fresh(f.tick,in.now)||!f.sequence||
    !registered(f,held)||held.slot!=2||!usable(p,side,in)||!aim.valid||aim.side!=side||aim.tick!=p.tick||
    aim.poseEpoch!=p.poseEpoch||aim.sourceEpoch!=p.sourceEpoch||aim.generation!=p.generation||
    !fresh(aim.tick,in.now)||(!releaseEdge&&p.grip<.35))return out;
 double best=std::numeric_limits<double>::infinity();
 for(const auto& o:f.offers)if(mortarDestinationH21(o.ref.slot)&&usable(o,f,in.now)&&o.boundsValid){
  const auto hit=geometry68::ray(o.bounds,{p.point.x,p.point.y,p.point.z},{aim.direction.x,aim.direction.y,aim.direction.z});
  if(hit.valid&&(hit.distance<best-1.e-9||(std::abs(hit.distance-best)<=1.e-9&&o.ref.slot<out.ref.slot))){
   best=hit.distance;out.ref=o.ref;out.gap=distanceToOffer(o,p.point);}}
 if(!valid(out.ref))return {};
 out.source=Source::Left;out.palm=p;out.aimedDestinationH21=true;out.aimReleaseH21=releaseEdge;
 out.heldSourceH21=held;out.aimH21=aim;out.frameSequence=f.sequence;out.frameTick=f.tick;out.table=f.table;
 return out;
}
struct State {
 Context context{};std::uint64_t lastSequence=0,latchedTick=0;
 Ref ref{};int side=-1;bool latched=false;std::array<bool,2> quarantined{};
 std::array<Ref,Capacity> priorTable{};bool tableSeen=false;
 void retire(const Input& in){
  for(unsigned h=0;h<2;++h)quarantined[h]=quarantined[h]||
   (latched&&side==int(h))||(std::isfinite(in.palms[h].grip)&&in.palms[h].grip>=.35);
  side=-1;ref={};latched=false;latchedTick=0;lastSequence=0;context={};tableSeen=false;priorTable={};
 }
 Decision step(const Completed& f,const Input& in){
  Decision out{};out.proximityCraft1=in.proximityCraft1;out.handsOnlyD53=in.proximityCraft1&&in.handsOnlyD53;
  out.sourceAcquisitionD39=in.sourceAcquisitionD39;out.pickupReachD39=in.pickupReachD39;
  for(unsigned h=0;h<2;++h)if(usable(in.palms[h],int(h),in)&&in.palms[h].grip<.35)quarantined[h]=false;
  if(!valid(in.context)||!f.complete||!same(f.context,in.context)||!fresh(f.tick,in.now)||!f.sequence||f.count>Capacity||
     (in.currentFrameSequence&&f.sequence>=in.currentFrameSequence)||(lastSequence&&f.sequence<lastSequence)){
   retire(in);return out;
  }
  if(valid(context)&&!same(context,in.context))retire(in);
  if(tableSeen)for(unsigned i=0;i<Capacity;++i)if(!same(priorTable[i],f.table[i])){retire(in);break;}
  priorTable=f.table;tableSeen=true;
  context=in.context;lastSequence=f.sequence;out.frameSequence=f.sequence;out.frameTick=f.tick;
  out.table=f.table;
  const int priorSide=side;const Ref priorRef=ref;
  if(side>=0&&(!usable(in.palms[unsigned(side)],side,in)||!registered(f,ref))){retire(in);return out;}
  // A never-admitted incompatible source is not an accepted work owner. Drop
  // only its hover/latch; do not quarantine the valid other hand's real Grip.
  if(side>=0&&!acquisitionAllowedH5(in,side,ref)){side=-1;ref={};latched=false;latchedTick=0;}
  if(latched&&in.proximityCraft1&&!in.selectionFrozenCraft1){
   bool stillNear=false;
   for(unsigned n=0;n<f.count;++n)if(usable(f.offers[n],f,in.now)&&same(f.offers[n].ref,ref))
    stillNear=stillNear||contains(in,ref,side,distanceToOffer(f.offers[n],in.palms[unsigned(side)].point));
   if(!stillNear){latched=false;latchedTick=0;side=-1;ref={};}
  }
  if(latched){
   if(in.palms[unsigned(side)].grip<.35){latched=false;latchedTick=0;}
   else {
    out.source=side?Source::Left:Source::Right;out.ref=ref;out.gripping=true;out.latchedTick=latchedTick;
    out.selectedPickupD44=selectedPickupD44(in,ref,side);
    out.palm=in.palms[unsigned(side)];out.gap=std::numeric_limits<double>::infinity();
    for(unsigned n=0;n<f.count;++n)if(usable(f.offers[n],f,in.now)&&same(f.offers[n].ref,ref))
     out.gap=(std::min)(out.gap,distanceToOffer(f.offers[n],out.palm.point));
    // Moving a held source outside its selection region does not delete the
    // held-object session, but it must stop overriding the native head selector.
    if(in.proximityCraft1&&!contains(in,ref,side,out.gap)){
     if(!out.handsOnlyD53)out.source=Source::Gaze;
     out.ref={};out.gripping=false;return out;
    }
    const auto& s=in.selected;
    out.admissionReady=s.valid&&same(s.context,context)&&registered(f,s.ref)&&same(s.ref,ref)&&
     fresh(s.tick,in.now)&&s.tick>=latchedTick&&(s.side==-1||s.side==side);
    return out;
   }
  }
  // Retain a previously nominated hand/reference inside the wider leave radius.
  // New callbacks cannot steal it merely by enumerating a closer item first.
  double gap=std::numeric_limits<double>::infinity();bool retained=false;
  if(side>=0)for(unsigned n=0;n<f.count;++n){const auto& p=f.offers[n];if(!usable(p,f,in.now)||!same(p.ref,ref))continue;
   const double d=distanceToOffer(p,in.palms[unsigned(side)].point);
   if(d<=leave(in,ref,side)){gap=(std::min)(gap,d);retained=true;}}
  // A deliberate close Grip outranks an unlatched idle hover, including one
  // retained by the leave-radius hysteresis. An already latched owner returned
  // above; a current deliberate hover inside Enter keeps its existing target.
  if(in.proximityCraft1&&retained){
   // Prefer a materially nearer eligible hand/target while merely hovering.
   // Keep a one-centimetre tie band to avoid alternating wrists on equal gaps.
   for(int h=0;h<2;++h)if(usable(in.palms[unsigned(h)],h,in)&&!quarantined[unsigned(h)])
    for(unsigned n=0;n<f.count;++n){const auto& o=f.offers[n];
     if(!usable(o,f,in.now)||!acquisitionAllowedH5(in,h,o.ref))continue;
     const double d=distanceToOffer(o,in.palms[unsigned(h)].point);
     if(d<=enter(in,o.ref,h)&&d+reach_craft1::SwitchMargin<gap&&
       (in.palms[unsigned(h)].grip>=.55||in.palms[unsigned(side)].grip<.55))retained=false;
    }
  }
  const bool retainedGrip=retained&&!quarantined[unsigned(side)]&&in.palms[unsigned(side)].grip>=.55&&gap<=enter(in,ref,side);
  if(!retainedGrip){int gripSide=-1;Ref gripRef{};double gripGap=std::numeric_limits<double>::infinity();
   for(int h=0;h<2;++h)if(usable(in.palms[unsigned(h)],h,in)&&!quarantined[unsigned(h)]&&in.palms[unsigned(h)].grip>=.55)
    for(unsigned n=0;n<f.count;++n){const auto& p=f.offers[n];if(!usable(p,f,in.now)||!acquisitionAllowedH5(in,h,p.ref))continue;
     const double d=distanceToOffer(p,in.palms[unsigned(h)].point);if(d>enter(in,p.ref,h))continue;
     if(gripSide<0||d<gripGap-1e-9||(std::abs(d-gripGap)<=1e-9&&(h<gripSide||(h==gripSide&&p.ref.slot<gripRef.slot)))){
      gripSide=h;gripRef=p.ref;gripGap=d;}
    }
   if(gripSide>=0){side=gripSide;ref=gripRef;gap=gripGap;retained=true;}
  }
  if(!retained){side=-1;ref={};
   for(int h=0;h<2;++h)if(usable(in.palms[unsigned(h)],h,in)&&!quarantined[unsigned(h)])
    for(unsigned n=0;n<f.count;++n){const auto& p=f.offers[n];if(!usable(p,f,in.now)||!acquisitionAllowedH5(in,h,p.ref))continue;
     const double d=distanceToOffer(p,in.palms[unsigned(h)].point);if(d>enter(in,p.ref,h))continue;
     // Deterministic tie order is hand then native table slot, independent of
     // OnScreen callback order. No entity-centre substitute is introduced.
     if(side<0||d<gap-1e-9||(std::abs(d-gap)<=1e-9&&(h<side||(h==side&&p.ref.slot<ref.slot)))){
      side=h;ref=p.ref;gap=d;}
    }
  }
  if(side<0){
   // D74: idle empty palms yield native hover/names to the head. A deliberately
   // closed hand still owns an empty selection, so Grip cannot borrow a distant
   // head target. Source admission continues to require the exact hand nominee.
   if(out.handsOnlyD53)for(int h=0;h<2;++h)if(usable(in.palms[unsigned(h)],h,in)&&in.palms[unsigned(h)].grip>=.35){
    out.source=h?Source::Left:Source::Right;out.palm=in.palms[unsigned(h)];break;}
   return out;
  }
  // R8b (Codex review of R8): the W4 swap to the game's own in-reach pick is gone. the hand's item stays the
  // nomination; the native selector is steered to it (interaction_native_selection68.inl) and admission waits for that.
  out.source=side?Source::Left:Source::Right;out.ref=ref;out.gap=gap;
  out.selectedPickupD44=selectedPickupD44(in,ref,side);
  out.palm=in.palms[unsigned(side)];
  out.hapticEdge=side!=priorSide||!same(ref,priorRef);
  const auto& palm=in.palms[unsigned(side)];
  if(!quarantined[unsigned(side)]&&palm.grip>=.55&&gap<=enter(in,ref,side)){latched=true;latchedTick=in.now;out.gripping=true;out.latchedTick=latchedTick;}
  // A release is not another admission. The new native final-selected witness
  // must be observed at or after this exact latch before readiness is returned.
  const auto& s=in.selected;
  out.admissionReady=latched&&s.valid&&same(s.context,context)&&registered(f,s.ref)&&same(s.ref,ref)&&
   fresh(s.tick,in.now)&&s.tick>=latchedTick&&(s.side==-1||s.side==side);
  return out;
 }
};
}
