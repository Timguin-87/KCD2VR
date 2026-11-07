#define main original_guard_cut36_main_h26
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
static void provisionalEnded26(Rig36& r){
    // State at the production candidate-start/withdraw seam: the prior real
    // commit remains, but the provisional candidate cleared released44c1 and
    // replaced gesture59c3. No native action/contact is mocked into existence.
    auto& s=r.state.weapon[0];s.released44c1=false;s.gesture59c3=r.in.sample+1;
    s.stroke118={};s.stroke118.ready=false;s.guard117=false;
    r.blocked=r.actor=true;
}
int main(){
    for(bool direct:{false,true})for(ph::Vec3 axis:{ph::Vec3{0,1,0},ph::Vec3{1,0,0}}){
     auto r=committed26(direct,axis);const auto gesture=r->state.weapon[0].gesture59c3;
     const int kind=r->state.weapon[0].stroke117==ph::CommandKind::Thrust?1:0;
     provisionalEnded26(*r);r->step({0,.02,0});const auto hit=r->out.impactRecord178r30[0];
     std::printf("CONTACT_CASE direct=%d expectedKind=%d valid=%d actualKind=%d expectedGesture=%llu actualGesture=%llu tick=%llu/%llu sample=%llu/%llu\n",direct,kind,hit.valid,hit.kind,(unsigned long long)gesture,(unsigned long long)hit.gesture59c3,(unsigned long long)r->in.tick,(unsigned long long)hit.tick,(unsigned long long)r->in.sample,(unsigned long long)hit.sample);
     check36(hit.valid&&hit.tick==r->in.tick&&hit.sample==r->in.sample&&hit.gesture59c3==gesture&&hit.kind==kind,
         "first actual HYBRID/DIRECT cut/thrust contact retains committed source after provisional identity was withdrawn");
     check36(r->hits()==1,"retained contact does not enqueue another native attack");
     r->step({0,.02,0});provisionalEnded26(*r);
     r->in.tick+=161;r->in.timeNs+=161000000;r->step({0,.02,0});
     check36(!r->out.impactRecord178r30[0].gesture59c3,
         "contacts cannot renew original command lifetime or replay prior target/part/tick");}
    for(unsigned failure=0;failure<9;++failure){auto r=committed26();provisionalEnded26(*r);
     if(failure==0){r->in.tick+=161;r->in.timeNs+=161000000;}
     if(failure==1)++r->profiles[0].generation;
     if(failure==2)++r->in.bodyEpoch;
     if(failure==3)++r->in.epoch;
     if(failure==4)r->in.attacks=false;
     if(failure==5)r->in.intentTracked[0]=false;
     // Opposite model/tracking motion, or motion in only one frame family,
     // cannot spend the prior committed source on a different motion.
     const auto delta=failure==6?ph::Vec3{0,-.02,0}:ph::Vec3{0,.02,0};
     r->step(delta,failure!=7,failure!=8);
     check36(!r->out.impactRecord178r30[0].gesture59c3,
         "expired/replaced/context-lost/withdrawn/single-frame motion cannot inherit committed identity");
    }
    std::printf("combat_contact_h26: %u checks, %u failures; native outcomes unclaimed\n",checks36,failures36);
    return failures36?1:0;
}
