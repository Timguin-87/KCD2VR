#include "../src/vrik/native_held_pose_t4.h"
#include <cassert>
#include <cstdio>
#include <limits>
using namespace kcd2vr;
namespace h=native_held_pose_t4;namespace b=native_weapon_body_t4;namespace p=physical;
struct Fixture {
 b::Input source{};b::Achieved a{};p::Input in{};std::array<p::Profile,2> profiles{};p::Output out{};p::State state{};
 Fixture(){
  source.owner={1,2,3,4,5,6,7,8,9,10,11,0};source.colliderLease=12;source.trackingEpoch=13;source.cancelGeneration=14;
  a.owner=source.owner;a.colliderLease=12;a.trackingEpoch=13;a.cancelGeneration=14;a.worldGeneration=15;a.bodyGeneration=16;a.bodyId=17;
  a.state=b::State::Active;a.poseValid=true;a.colliderRetired=false;a.sample=20;a.sourceTick=995;a.observedTick=1000;a.weaponWorld.p={3,.2,0};
  in.haveWorld=in.allowed=in.tracked[0]=true;in.world.p={3,0,0};in.sample=21;in.tick=1000;in.reach[0]=1;
  auto&w=profiles[0];w.key=1;w.generation=2;w.entity=11;w.primary=w.geometry=true;w.grip[0].p={0,.1,0};
 }
 bool apply(h::ApplyReasonD42* reason=nullptr){return h::apply(source,a,1000,in,profiles,out,state,nullptr,0,reason);}
};
int main(){unsigned checks=0;
 for(unsigned i=0;i<13;++i){Fixture f;h::ApplyReasonD42 expected=h::ApplyReasonD42::Applied;
  switch(i){
   case 0:f.a.poseValid=false;expected=h::ApplyReasonD42::State;break;
   case 1:++f.a.owner.request;expected=h::ApplyReasonD42::Owner;break;
   case 2:++f.a.colliderLease;expected=h::ApplyReasonD42::Lease;break;
   case 3:++f.a.trackingEpoch;expected=h::ApplyReasonD42::Tracking;break;
   case 4:++f.a.cancelGeneration;expected=h::ApplyReasonD42::CancelGeneration;break;
   case 5:f.a.bodyGeneration=0;expected=h::ApplyReasonD42::BodyIdentity;break;
   case 6:f.a.sample=22;expected=h::ApplyReasonD42::SourceOrder;break;
   case 7:f.a.sourceTick=919;expected=h::ApplyReasonD42::Freshness;break;
   case 8:f.in.worldScale=0;expected=h::ApplyReasonD42::WorldFrame;break;
   case 9:++f.profiles[0].generation;expected=h::ApplyReasonD42::Profile;break;
   case 10:f.a.weaponWorld.q.w=std::numeric_limits<double>::quiet_NaN();expected=h::ApplyReasonD42::WorldPose;break;
   case 11:f.in.reach[0]=.1;expected=h::ApplyReasonD42::Reach;break;
   default:break;
  }
  h::ApplyReasonD42 why=h::ApplyReasonD42::NotAttempted;const auto untouched=f;const bool accepted=f.apply(&why);
  assert(why==expected);++checks;assert(accepted==(expected==h::ApplyReasonD42::Applied));++checks;
  auto originalCall=untouched;assert(originalCall.apply()==accepted);++checks;
  if(accepted){assert(f.out.solverOwnedT4[0]&&originalCall.out.solverOwnedT4[0]);assert(f.out.hands[0].p.y==originalCall.out.hands[0].p.y);++checks;}
  else {assert(!f.out.solverOwnedT4[0]&&!f.state.weapon[0].ready);++checks;}
 }
 std::printf("native_held_reason_d42: %u actual apply checks PASS\n",checks);
}
