// Actual physical guard arbitration, including the existing guard->attack regression suite.
#define main ExistingGuardAttackChecksR92
#include "guard_cut36_test.cpp"
#undef main
#include "vrik/physical_guard112.h"
int main(){
 using kcd2vr::release_combat_r92::fastBlock;
 using kcd2vr::release_combat_r92::guardDwell;
 fastBlock=false;
 if(ExistingGuardAttackChecksR92())return 1;
 fastBlock=true;
 if(ExistingGuardAttackChecksR92())return 1;
 check36(guardDwell(350,false)==100,"fast mode fixes blade entry at100ms");
 check36(guardDwell(350,true)==350,"shield configured timing unchanged");
 check36(guardDwell(0,false)==100,"fast mode cannot bypass dwell at zero configured delay");
 for(bool enabled:{false,true}){
   fastBlock=enabled;Rig36 rig(false);rig.in.guardHeadKnown=true;rig.in.guardHead={0,0,1.65};
   rig.in.guardEnterDwellMs178r29=350;
   rig.state.weapon[0].guard117=false;rig.state.weapon[0].guardEntry30c1={};
   rig.commands.clear();const auto start=rig.in.tick;std::uint64_t entered=0;
   for(int i=0;i<40;++i){rig.step();if(rig.out.guard117&&!entered)entered=rig.in.tick-start;}
   check36(entered==(enabled?110u:360u),enabled?"actual kernel enters100ms after first valid posture":"disabled mode preserves350ms configured dwell");
   check36(rig.hits()==0&&rig.count(ph::CommandKind::Prepare)==0,"quiet guard entry emits no fabricated attack");
 }
 fastBlock=true;
 {Rig36 rig(false);rig.in.guardHeadKnown=true;rig.in.guardHead={0,0,1.65};
  auto pose=rig.in.hands[0];
  check36(ph::natural_guard117(rig.in,rig.profiles[0],pose,false),"parallel chest-plane blade intersects tuned face box");
  pose.p.y=-.2;check36(!ph::natural_guard117(rig.in,rig.profiles[0],pose,false),"behind-body blade stays excluded");
  pose=rig.in.hands[0];pose.p.z=.7;check36(!ph::natural_guard117(rig.in,rig.profiles[0],pose,false),"lowered blade cannot enter face block");
  pose=rig.in.hands[0];auto forward=rig.profiles[0];forward.tip={0,.6,0};forward.axis={0,1,0};
  check36(!ph::natural_guard117(rig.in,forward,pose,false),"forward stabbing orientation remains attack capable");
  kcd2vr::guard_parry176::Entry entry{};
  check36(!kcd2vr::guard_parry176::enter(entry,true,false,1000,3.,2.5,guardDwell(350,false)),"fast block keeps configured motion speed gate");
 }
 fastBlock=false;check36(guardDwell(350,false)==350,"switch off restores configured dwell");
 std::printf("R92 fast block checks=%u failures=%u\n",checks36,failures36);
 return failures36?1:0;
}
