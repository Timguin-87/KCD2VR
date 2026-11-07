#include "../src/vrik/physical_weapon.h"
#include "../src/vrik/weapon_sweep112.h"
#include <cstdio>
using namespace kcd2vr::physical;
namespace hg=kcd2vr::hilt169r2;
static int checks=0,failures=0;static void ck(bool v,const char* n){++checks;if(!v)++failures;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
int main(){
    const double lo[]{-.03,-.20,-.03},hi[]{.03,.9,.03};
    auto region=hg::qualify({0,0,0},{0,.9,0},{0,-.09,0},lo,hi);
    ck(region.valid&&std::abs(region.b.y+.195)<1e-9,"hilt endpoint clipped inside actual bounds, no +8 cm pommel assumption");
    ck(hg::select(region,{.01,-.18,0}).has_value(),"near-pommel hilt estimate is selectable");
    ck(!hg::select(region,{0,.02,0}),"radial tolerance cannot acquire on the blade side");
    ck(!hg::select(region,{0,-.3,0}),"cannot acquire beyond bounded hilt");
    ck(!hg::qualify({0,0,0},{0,.9,0},{0,.09,0},lo,hi).valid,"blade-side support reference cannot create a handle region");
    Profile p{};p.key=p.generation=1;p.side=0;p.classId=4;p.primary=p.support=p.twoHanded=p.geometry=true;
    p.grip[1].p={0,-.09,0};p.tip={0,.9,0};p.supportRegion169r2=region;
    p.supportPositionOnly174r2=true; // region selection is a geometry-only fallback; native sockets are tested in support_contract175
    std::array<Profile,2> profiles{p,Profile{}};Input i{};i.allowed=i.tracked[0]=i.tracked[1]=true;
    i.epoch=i.bodyEpoch=i.sample=1;i.tick=1000;i.grip[0]=1;i.contact=true;i.dynamics=i.attacks=i.bowActions=false;
    i.reach[0]=i.reach[1]=1;i.shoulders[0]={.1,0,0};i.shoulders[1]={-.1,0,0};
    i.hands[1].p={0,-.18,0};State state{};int mode=0,acquires=0;Vec3 queried{};
    auto ray=[&](const Profile& q,Pose a,Pose b,bool acquire){Contact103 c{};
        if(acquire){++acquires;queried=q.grip[1].p;if(mode==1){c.known=false;c.fraction=0;}if(mode==2){c.limited115=true;c.fraction=.8;}}
        return c;};
    auto next=[&](){++i.sample;i.tick+=10;return step(state,i,profiles,ray);};
    next();next();i.grip[1]=1;mode=1;Output out{};for(int j=0;j<5;++j)out=next();
    ck(!out.support[0]&&acquires>0,"UNKNOWN approach never acquires the expanded hilt");
    mode=2;for(int j=0;j<4;++j)out=next();ck(!out.support[0],"checked partial approach is not full clearance");
    mode=0;out=next();ck(out.support[0]&&state.weapon[0].supportGripReady169r2,"CLEAR approach acquires the chosen handle point");
    ck(m::length(queried-Vec3{0,-.18,0})<1e-9,"approach ray uses the chosen point, not original support socket");
    ck(m::length(out.hands[1].p-point(out.weapons[0],Vec3{0,-.18,0}))<1e-9,"displayed support wrist uses selected weapon-local grip");
    // Keep the selected synthetic-grasp cases in one coherent ownership lifetime.
    ck(m::angle(out.hands[1].q,compose(out.weapons[0],state.weapon[0].supportGrip169r2).q)<1e-8,"selected support rotation remains latched");
    i.supportConstraint130=true;i.supportMaxAngle=.96;i.hands[1].p.y=-.13;out=next();ck(out.support[0]&&m::length(state.weapon[0].supportGrip169r2.p-Vec3{0,-.18,0})<1e-9,"moving controller within hold hysteresis cannot slide/reselect latched grip");
    ck(out.owned[1]&&m::length(out.hands[1].p-point(out.weapons[0],state.weapon[0].supportGrip169r2.p))<1e-9,
       "two-hand constraint and final support hand share the selected local point");
    const auto beforeInvalid=state;profiles[0].supportRegion169r2.valid=false;out=next();
    ck(!out.support[0],"loss of hilt qualification releases instead of silently switching to another grip");
    profiles[0]=p;state=beforeInvalid;
    i.grip[1]=0;out=next();ck(!out.support[0],"release removes the support constraint");
    i.hands[1].p={0,.03,0};i.grip[1]=1;for(int j=0;j<5;++j)out=next();ck(!out.support[0],"no reacquisition at blade even after explicit release");
    // An independent synthetic dwell starts with a compatible controller wrist frame.
    state={};i.grip[1]=0;i.hands[1].q={};i.hands[1].p={0,-.10,0};mode=0;next();next();i.grip[1]=1;next();
    i.hands[1].p={0,-.16,0};next();i.hands[1].p={0,-.12,0};out=next();
    ck(out.support[0]&&m::length(state.weapon[0].supportGrip169r2.p-Vec3{0,-.10,0})<1e-9,
       "movement along the valid hilt during Grip dwell retains the chosen point instead of restarting");
    // Geometry-only support is a separate explicit acquisition from neutral.
    // The first calibrated opposite wrist defines its immutable local rotation.
    state={};profiles[0]=p;profiles[0].supportPositionOnly174r2=true;
    i.grip[1]=0;i.hands[1].p={0,-.18,0};i.hands[1].q=m::from_rotation_vector({0,0,2.5});
    next();next();i.grip[1]=1;for(int j=0;j<4;++j)out=next();
    ck(out.support[0]&&m::angle(out.hands[1].q,i.hands[1].q)<1e-8,
       "synthesized support captures the calibrated opposite wrist at acquisition");
    const auto selected=state.weapon[0].supportGrip169r2;
    i.hands[1].q=m::from_rotation_vector({0,0,2.6});out=next();
    ck(out.support[0]&&m::angle(state.weapon[0].supportGrip169r2.q,selected.q)<1e-8&&
       m::angle(out.hands[1].q,compose(out.weapons[0],selected).q)<1e-8,
       "synthesized support retains the selected local quaternion instead of a second raw-controller writer");
    ck(m::length(out.hands[1].p-point(out.weapons[0],state.weapon[0].supportGrip169r2.p))<1e-9,
       "rotation correction retains exact selected hilt position");
    profiles[0].supportPositionOnly174r2=false;out=next();
    ck(!out.support[0]&&state.weapon[0].supportRearm,
       "changing selected support provenance releases the constraint and requires neutral Grip");
    std::printf("hilt169r2=%d/%d\n",checks-failures,checks);return failures?1:0;
}
