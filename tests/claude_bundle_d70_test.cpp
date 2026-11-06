#include "gesture_recovery_h30_fixture.h"
#include "vrik/master_gate47.h"
#include <limits>
namespace d70=kcd2vr::claude_d70;
namespace mg=kcd2vr::master_gate47;
static void advanceCase(bool on,bool retreat,bool source,int age,bool expect) {
 d70::SetCombatFix(on); RigH30 r(false,{0,1,0});moveH30(r,14);
 auto &s=r.state.weapon[0];auto &c=s.committedGestureH26;
 ckH30(c.gesture&&c.kind==1,"fixture has real committed thrust");
 const auto gesture=c.gesture;
 r.in.tick+=10;r.in.timeNs+=10000000;++r.in.sample;
 c.tick=r.in.tick-age;
 r.in.hands[0].p=s.intentHand+ph::Vec3{0,-.01,0};
 r.in.intentHands[0].p=s.relativeIntentHand+ph::Vec3{0,-.01,0};
 const ph::Pose model{{},s.intentTip+ph::Vec3{0,retreat?-.02:.02,0}-r.profiles[0].tip};
 const ph::Pose tracked{{},s.relativeIntentTip+ph::Vec3{0,retreat?-.02:.02,0}-r.profiles[0].tip};
 ph::AdvanceCommittedGestureH8(r.in,r.profiles[0],s,0,model,tracked,source);
 ckH30(bool(c.gesture)==expect,"actual committed-thrust runtime preserves only current forward blade");
 if(expect)ckH30(c.gesture==gesture&&c.forwardSampleH8==r.in.sample&&c.forwardTickH8==r.in.tick,
   "same original serial, current forward stamp, no old contact replay");
}
int main(){
 ckH30(!d70::CombatFix()&&!d70::HudWalkoff(),"both requested experiments default OFF");
 auto revision=d70::combatRevision.load();d70::SetCombatFix(false);
 ckH30(d70::combatRevision.load()==revision,"unchanged option creates no policy transition");
 d70::SetCombatFix(true);ckH30(d70::combatRevision.load()==revision+1,"real toggle invalidates prior mod admission");
 for(int age:{90,154,240,250})ckH30(d70::ForwardThrust(1000,1000-age,1,.02,.02,.01,.01),"forward90..250ms is eligible");
 ckH30(!d70::ForwardThrust(1000,749,1,.02,.02,.01,.01),"251ms is not grace");
 ckH30(!d70::ForwardThrust(1000,900,1,-.001,.02,.01,.01),"return in one independent tip family is not a forward stab");
 ckH30(!d70::ForwardThrust(1000,900,1,.02,.02,-.001,.01),"forward axis cannot mask a backward world increment");
 ckH30(!d70::ForwardThrust(1000,900,0,.02,.02,.01,.01),"cut unchanged");
 ckH30(!d70::ForwardThrust(1000,900,1,std::numeric_limits<double>::quiet_NaN(),.02,.01,.01),"nonfinite increment rejected");
 advanceCase(false,false,true,90,false);advanceCase(true,false,true,90,true);advanceCase(true,false,true,240,true);
 advanceCase(true,true,true,90,false);advanceCase(true,false,false,90,false);advanceCase(true,false,true,251,false);
 for(bool trial:{false,true})for(int z:{0,1,2,5}){
  mg::In i{};i.perk=1;i.stroke=mg::Stroke::Thrust;i.origin=mg::masterZone(z);i.enemyZone=z;i.stabFamiliesD70=trial;
  ckH30(mg::allowed(mg::decide(i))==(!trial||z!=1),"only requested slash-family selection falls back to ordinary attack");
  if(trial&&z==1)ckH30(mg::sideRule(mg::decide(i),false)==mg::Reason::StabFamilyD70,"relaxed side policy cannot erase family choice");
  i.perk=0;ckH30(mg::decide(i)==mg::Reason::NoPerk,"native perk remains authoritative");
 }
 ckH30(mg::commitReason(mg::Reason::StabFamilyD70,true)==mg::Reason::Qualified,"accepted prepare keeps native release");
 d70::SetCombatFix(false);d70::SetHudWalkoff(true);ckH30(!d70::CombatFix()&&d70::HudWalkoff(),"HUD option independent");
 d70::SetHudWalkoff(false);std::printf("D70 bundle %u/%u checks PASS\n",checksH30-failuresH30,checksH30);return failuresH30?1:0;
}
