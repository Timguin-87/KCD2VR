#define main h26_previous_main_h29
#include "guard_cut36_test.cpp"
#undef main
#include <memory>
#include <cassert>
static std::unique_ptr<Rig36> committed26(bool direct=true,ph::Vec3 axis={0,1,0}){
    auto r=std::make_unique<Rig36>(direct,axis);
    for(unsigned n=0;n<20&&!r->hits();++n)r->step({0,.025,0});
    assert(r->hits()==1&&r->state.weapon[0].released44c1);
    return r;
}
int main(){
 for(bool direct:{false,true})for(ph::Vec3 axis:{ph::Vec3{0,1,0},ph::Vec3{1,0,0}}){
  auto r=committed26(direct,axis);const auto gesture=r->state.weapon[0].gesture59c3,commit=r->state.weapon[0].committedGestureH26.tick;
  // Native grip/pose refresh changes the model's desired pose; ORIGINAL tracked
  // hand positions/rotation, owner, item, epochs and committed action stay current.
  r->profiles[0].grip[0].q=pm::from_rotation_vector({0,0,1.8});r->step({0,.02,0});
#ifndef H29_BASELINE
  check36(r->out.resetH29[0].retained,"checked pose rebase retains source identity, not a collision or a new attack");
#endif
  check36(r->state.weapon[0].committedGestureH26.gesture==gesture&&r->state.weapon[0].committedGestureH26.tick==commit,"pose rebase keeps original immutable gesture and original deadline");
  r->blocked=r->actor=true;r->step({0,.02,0});
  check36(r->out.impactRecord178r30[0].valid&&r->out.impactRecord178r30[0].gesture59c3==gesture,"new checked actor contact joins same original cut/stab after rebase");
  check36(r->hits()==1,"contact identity retention never emits another native attack");
 }
 for(unsigned failure=0;failure<6;++failure){auto r=committed26();r->profiles[0].grip[0].q=pm::from_rotation_vector({0,0,1.8});
  if(failure==0)r->in.intentTracked[0]=false;if(failure==1)++r->profiles[0].generation;if(failure==2){r->in.tick+=101;r->in.timeNs+=101000000;}
  if(failure==3)r->in.attacks=false;if(failure==4)r->known=false;if(failure==5)r->in.intentHands[0].p.x+=.5;
  r->step({0,.02,0});check36(!r->state.weapon[0].committedGestureH26.gesture,"tracking/owner/time/policy/unknown-query/jump cannot retain identity");
 }
 std::printf("stroke_rebase_h29: %u checks, %u failures; native outcomes unclaimed\n",checks36,failures36);return failures36?1:0;
}
