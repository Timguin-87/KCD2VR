// Complete actual HYBRID producer, with only the collision-query leaf synthetic.
#include "gesture_recovery_h30_fixture.h"
#include <cassert>
namespace cut=kcd2vr::gore_cut_d56;
static void physical(RigH30& r){r.in.dynamics=r.in.constraintDrive129=true;r.in.nativeStateKnown178=true;r.profiles[0].weight=1.6;}
static auto impact(RigH30& r){auto e=r.out.impactRecord178r30[0];e.goreCutD56.power=cut::powerRatio(e.goreCutD56,r.profiles[0].weight,75.);return e;}
int main(){
 {
  RigH30 r(false);physical(r);for(int i=0;i<6;++i)r.step({0,.14,0});
  assert(r.count(ph::CommandKind::Slash)==1);r.blocked=r.actor=true;r.step({0,.14,0});
  const auto hit=impact(r);const auto frozen=hit.goreCutD56;
  assert(hit.valid&&frozen.committed&&frozen.netTravel>=.22&&frozen.fastTravel>=.30);
  assert(frozen.transverseSpeed>13&&cut::qualify(frozen,.6)==cut::Why::Ok);
  assert(cut::owned(frozen,hit.gesture59c3,hit.key,hit.generation,hit.epoch,hit.request,hit.sample,hit.tick));
  r.step({0,.14,0});const auto pressure=impact(r);
  assert(pressure.gesture59c3==hit.gesture59c3&&r.count(ph::CommandKind::Slash)==1);
  assert(cut::qualify(pressure.goreCutD56,.6)!=cut::Why::Ok);
  assert(frozen.tick==hit.tick&&frozen.transverseSpeed>13); // later stopped motion cannot rewrite the first contact
  std::printf("PASS actual HYBRID full cut gesture=%llu net=%.3f fast=%.3f contact=%.3f power=%.3f; later pressure refused\n",
    (unsigned long long)frozen.gesture,frozen.netTravel,frozen.fastTravel,frozen.transverseSpeed,frozen.power);
 }
 {
  RigH30 r(false,{0,1,0});physical(r);for(int i=0;i<6;++i)r.step({0,.14,0});
  r.blocked=r.actor=true;r.step({0,.14,0});const auto hit=impact(r);
  assert(r.count(ph::CommandKind::Thrust)==1&&hit.valid&&hit.kind==1);
  assert(cut::qualify(hit.goreCutD56,.6)==cut::Why::NotCut&&hit.goreCutD56.transverseSpeed<1e-6);
  std::puts("PASS actual high-energy native thrust still commits once but cannot cut");
 }
 {
  RigH30 r(false,{0,-1,0});physical(r);for(int i=0;i<6;++i)r.step({0,.14,0});
  r.blocked=r.actor=true;r.step({0,.14,0});const auto hit=impact(r);
  assert(r.count(ph::CommandKind::Pommel)==1&&hit.valid&&hit.kind==0);
  assert(!hit.goreCutD56.committed&&cut::qualify(hit.goreCutD56,.6)!=cut::Why::Ok);
  std::puts("PASS actual native pommel retains kind0 dispatch and cannot borrow full CUT credential");
 }
 {
  RigH30 r(false);physical(r);r.blocked=r.actor=true;r.step({0,.02,0});
  const auto hit=impact(r);assert(hit.valid&&!r.hits()&&!hit.gesture59c3&&hit.kind==-1);
  assert(cut::qualify(hit.goreCutD56,.6)==cut::Why::Identity);
  std::puts("PASS actual uncommitted body touch remains UNKNOWN, never default CUT");
 }
 {
  RigH30 r(false);physical(r);for(int i=0;i<4;++i)r.step({0,.03,0});
  r.blocked=r.actor=true;r.step({0,.03,0});const auto hit=impact(r);
  assert(hit.valid&&r.count(ph::CommandKind::Slash)==1&&hit.goreCutD56.committed);
  assert(hit.goreCutD56.netTravel<.22&&cut::qualify(hit.goreCutD56,.6)==cut::Why::ShortStroke);
  std::puts("PASS actual short contact-assisted native cut remains valid combat but cannot sever");
 }
}
