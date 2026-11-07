#include "../src/vrik/elbow_destination_d86.h"
#include "../src/vrik/elbow_r89.h"
#include <cstdio>
using namespace kcd2vr::elbow_destination_d86;
int main(){
 int checks=0,failed=0;auto ck=[&](bool ok,const char* why){++checks;if(!ok){++failed;std::printf("FAIL %s\n",why);}};
 Input in{};in.shoulder={.19,0,1.4};in.hand={.18,.33,1.49};in.upper=.28;in.fore=.27;
 in.relaxed={.2,-.45,-1};in.outward={1,0,0};in.up={0,0,1};in.backward={0,-1,0};
 in.torsoA={0,0,.85};in.torsoB={0,0,1.4};in.torsoRadius=.115;in.head={0,0,1.66};in.haveHead=true;
 in.haveForearm=true;in.forearmDirection={0,.2,-1};
 const auto ordinary=choose(in);auto ordinaryArm=m::solve_arm(in.shoulder,in.hand,in.upper,in.fore,ordinary.pole,ordinary.pole);
 in.crossbowSupportR93=true;const auto tuck=choose(in);auto arm=m::solve_arm(in.shoulder,in.hand,in.upper,in.fore,tuck.pole,tuck.pole);
 ck(ordinary.valid&&tuck.valid&&ordinaryArm&&arm,"valid crossbow fixture");
 if(!arm||!ordinaryArm)return 1;
 std::printf("ordinary_elbow_z=%.9f support_elbow_z=%.9f wrist_z=%.9f\n",ordinaryArm->elbow.z,arm->elbow.z,in.hand.z);
 ck(arm->elbow.z<ordinaryArm->elbow.z-.05,"support preference lowers view-obstructing elbow");
 ck(arm->elbow.z<in.hand.z-.02,"support elbow stays below stock wrist");
 ck(tuck.intrusion<1e-10,"tuck does not enter torso/head proxies");
 for(int i=0;i<25;++i){
  auto a=in;a.hand={.17+.002*i,.30+.001*i,1.43+.006*i};
  const auto r=choose(a);auto solved=m::solve_arm(a.shoulder,a.hand,a.upper,a.fore,r.pole,r.pole);
  ck(r.valid&&solved&&m::length(solved->hand-a.hand)<1e-9,"controller endpoint unchanged");
  ck(solved&&std::abs(m::length(solved->elbow-a.shoulder)-a.upper)<1e-9&&std::abs(m::length(solved->hand-solved->elbow)-a.fore)<1e-9,"both anatomical lengths preserved");
  ck(r.intrusion<1e-10,"clearance preserved through aimed movement");
 }
 kcd2vr::elbow_r89::Input held{};held.shoulder=in.shoulder;held.wrist=in.hand;held.upper=in.upper;held.fore=in.fore;
 held.body=tuck.pole;held.torsoA=in.torsoA;held.torsoB=in.torsoB;held.torsoRadius=in.torsoRadius;held.seedElbow=ordinaryArm->elbow;
 m::ElbowContinuity state{};std::optional<m::Vec3> pole;
 for(int i=0;i<=90;++i){pole=kcd2vr::elbow_r89::elbow(state,held,7,i+1,1000+i*11,1000000000ull+i*11111111ull);ck(bool(pole),"existing hand-guided clock stays valid");}
 const auto samples=state.updates;auto second=kcd2vr::elbow_r89::elbow(state,held,7,91,1990,1999999990ull);
 ck(pole&&second&&state.updates==samples&&m::length(*pole-*second)<1e-12,"second eye never advances elbow");
 auto filtered=m::solve_arm(in.shoulder,in.hand,in.upper,in.fore,*pole,*pole);
 ck(filtered&&filtered->elbow.z<in.hand.z-.02,"hand-guided mode converges below stock");
 // Setting false is the exact former objective; other arms and weapons do not enter the added branch.
 in.crossbowSupportR93=false;const auto restored=choose(in);
 ck(m::length(restored.pole-ordinary.pole)<1e-12&&restored.objective==ordinary.objective,"ordinary destination unchanged after support release");
 std::printf("R93 crossbow support elbow: %d/%d PASS\n",checks-failed,checks);return failed?1:0;
}
