#define main original_guard_cut36_main_h27
#include "guard_cut36_test.cpp"
#undef main
#include <memory>
int main(){
 for(bool direct:{false,true}){
  auto r=std::make_unique<Rig36>(direct,ph::Vec3{1,0,0});
  check36(r->out.guard117,"current frontal guard geometry initially owns the weapon");
  // Real producer increments in BOTH frame families. The wrist stays fixed;
  // the edge sweeps forward after the guard face turns away from Henry.
  bool heldGuardAttempt=false;
  for(unsigned i=1;i<=12;++i){const double angle=.1*i;
   r->in.hands[0].q=r->in.intentHands[0].q={0,0,std::sin(angle*.5),std::cos(angle*.5)};
   r->step();for(unsigned n=0;n<r->out.commandCount;++n){const auto kind=r->out.commands[n].kind;
    if(kind==ph::CommandKind::Prepare||kind==ph::CommandKind::Slash||kind==ph::CommandKind::Thrust)
     heldGuardAttempt|=r->out.guardBySide30c1[0];}}
  check36(!r->out.guard117,"actual current guard spatial predicate releases as the blade leaves guard");
  check36(r->hits()==1,"C54 deliberate rotating edge after guard release prepares/commits exactly once");
  std::uint64_t gesture=0;unsigned joined=0;bool exact=true;
  for(const auto& c:r->commands)if(c.kind==ph::CommandKind::Prepare||c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust){
   if(!gesture)gesture=c.gesture59c3;
   exact&=c.gesture59c3&&c.gesture59c3==gesture&&c.key==77&&c.generation==1&&c.epoch==r->in.epoch&&c.bodyEpoch==r->in.bodyEpoch;
   ++joined;}
  check36(exact&&joined==2&&r->count(ph::CommandKind::Prepare)==1,"one original physical gesture owns exact item/generation/body/request Prepare and Commit");
  check36(!heldGuardAttempt,"no restored input is offered while the CURRENT guard still owns the weapon");
  const auto count=r->hits();for(unsigned i=0;i<30;++i)r->step();
  check36(r->hits()==count,"holding the blade stationary after a restored cut cannot repeat an attack");
  // No broad positional toggle: the held frontal guard and tiny jitter still
  // cannot turn into a native attack, even though C54 wrist cuts are admitted.
  auto stationary=std::make_unique<Rig36>(direct,ph::Vec3{1,0,0});
  for(unsigned i=0;i<40;++i)stationary->step({0,i%2?.0005:-.0005,0});
  check36(stationary->out.guard117&&!stationary->hits(),"front-guard holding/jitter remains defensive, with no attack");
  auto mismatch=std::make_unique<Rig36>(direct,ph::Vec3{1,0,0});
  for(unsigned i=1;i<=12;++i){const double angle=.1*i;
   mismatch->in.hands[0].q={0,0,std::sin(angle*.5),std::cos(angle*.5)};
   mismatch->in.intentHands[0].q={0,0,-std::sin(angle*.5),std::cos(angle*.5)};
   mismatch->step();}
  check36(!mismatch->hits(),"opposing model/tracking edge trajectories cannot borrow C54 admission");
 }
 std::printf("c54_cut_admission_h27: %u checks, %u failures; native results unclaimed\n",checks36,failures36);
 return failures36?1:0;
}
