#include "vrik/physical_weapon.h"
#include "vrik/gore_eligibility_d56.h"
#include "diagnostics/capture168.h"
#include <cassert>
#include <limits>
#include <cstdio>
namespace ph=kcd2vr::physical;
namespace cut=kcd2vr::gore_cut_d56;
namespace capture=kcd2vr::diagnostics::capture168;
int main(){
 cut::Evidence e{1,2,3,4,5,6,100,.22,.30,12,13,1.,0,true};
 assert(cut::qualify(e,.6)==cut::Why::Ok);
 for(int k:{-1,1,2}){auto x=e;x.kind=k;assert(cut::qualify(x,.6)==cut::Why::NotCut);}
 auto x=e;x.gesture=0;assert(cut::qualify(x,.6)==cut::Why::Identity);
 x=e;x.committed=false;assert(cut::qualify(x,.6)==cut::Why::NotCommitted);
 x=e;x.netTravel=.219;assert(cut::qualify(x,.6)==cut::Why::ShortStroke);
 x=e;x.fastTravel=.299;assert(cut::qualify(x,.6)==cut::Why::ShortStroke);
 x=e;x.transverseSpeed=1;assert(cut::qualify(x,.6)==cut::Why::Motion);
 x=e;x.power=.899;assert(cut::qualify(x,.6)==cut::Why::Power);
 assert(cut::qualify(e,1.1)==cut::Why::Power);
 x=e;x.totalSpeed=std::numeric_limits<double>::quiet_NaN();assert(cut::qualify(x,.6)==cut::Why::Motion);
 assert(cut::owned(e,1,2,3,4,5,6,100));assert(!cut::owned(e,1,2,3,4,5,7,100));
 cut::Ledger<16> used;cut::Claim claim{42,0x123,1,2,3,4,5};
 assert(used.take(claim));assert(!used.take(claim));++claim.gesture;assert(used.take(claim));++claim.epoch;assert(used.take(claim));
 // Exercise the production pre-impact velocity decomposition. Axial energy is
 // still valid native thrust effort, but never a transverse cosmetic cut.
 ph::Profile sword{};sword.classId=4;sword.weight=1.6;sword.tip={0,1,0};sword.grip[0].p={};
 ph::ImpactRecord178r30 impact{};assert(impact.kind==-1);
 impact.gesture59c3=1;impact.kind=0;
 ph::impact_effort19(impact,sword,0,{}, {12,0,0},{},1.,{0,.75,0});
 assert(impact.effortKnown19&&std::abs(impact.goreCutD56.transverseSpeed-12)<1e-9&&std::abs(impact.goreCutD56.totalSpeed-12)<1e-9);
 ph::impact_effort19(impact,sword,0,{}, {0,12,0},{},1.,{0,.75,0});
 assert(impact.effortKnown19&&impact.goreCutD56.transverseSpeed==0&&impact.goreCutD56.totalSpeed==12);
 ph::impact_effort19(impact,sword,0,{}, {},{0,0,12},1.,{0,.75,0});
 assert(std::abs(impact.goreCutD56.transverseSpeed-9)<1e-9); // actual 75 cm contacted point, not 1 m tip
 // F3 alone must retain exactly these bounded gore families, even payloads
 // mentioning HUD/camera; F2 alone must not acquire them.
 const char* lines[]{"C43T DECAP_CORPSE skipped", "C60 LIMB_CORPSE candidate", "C60 DECAP_GATE verdict", "C61 CORPSE_CUT_ACK result",
     "C39 DECAP skipped", "D43 LIMB_CUT pins", "D54 GORE_MATERIAL HUD result", "D56 GORE_ELIGIBILITY result"};
 capture::gate.select(capture::Domain::Combat,true,1000);
 for(auto s:lines)assert(capture::accept(s,1010));
 capture::gate.select(capture::Domain::Combat,false,1020);capture::gate.select(capture::Domain::Bug,true,1020);
 for(auto s:lines)assert(!capture::accept(s,1030));
 // The shared guard predicate is used both by the hand-driven and native-solver routes.
 namespace guard=kcd2vr::guard_parry176;namespace tuning=kcd2vr::melee_tuning_c54;
 guard::Position p{{0,.58,0},{.5,.58,.15},{},{1,0,0}};
 assert(guard::position(p,false));p.grip.y=p.tip.y=.70;assert(!guard::position(p,false));
 p.grip.y=p.tip.y=.68;assert(!guard::position(p,false)&&guard::position(p,true));
 p.grip.y=p.tip.y=.71;assert(!guard::position(p,true));
 tuning::guardBodyDistanceD56=.90f;assert(guard::position(p,false));
 tuning::guardBodyDistanceD56=std::numeric_limits<float>::quiet_NaN();assert(!guard::position(p,false));
 tuning::guardBodyDistanceD56=.65f;
 std::puts("gore_admission_d56: full-swing, contact motion, dedupe, F3 routes and shared guard distance PASS");
}
