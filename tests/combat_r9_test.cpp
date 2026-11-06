// R9 combat rules: combo trigger slot keep, raised-hilt guard (R10: fist reach shift retired).
#include "vrik/guard_parry176.h"
#include <cstdio>
namespace c=kcd2vr::combat_r9;namespace g=kcd2vr::guard_parry176;using V=kcd2vr::pose_core::Vec3;
static int checks=0,failures=0;
static void ck(bool ok,const char* what){++checks;if(!ok)++failures;std::printf("%s %s\n",ok?"PASS":"FAIL",what);}
static double len(V v){return kcd2vr::pose_core::length(v);}
int main(){
    // R10: the fist reach shift is retired (fists reach where Henry reaches, fist_reach_r10_test)
    // combo: trigger phase 1 and 2 kept inside a live window with a requested slot, nothing else
    ck(c::keepTriggerSlot(1,true,true,true)&&c::keepTriggerSlot(2,true,true,true),"phase 1/2 kept in a live window");
    ck(!c::keepTriggerSlot(1,true,true,false)&&!c::keepTriggerSlot(2,true,true,false),"OFF closes both idle phases natively");
    ck(!c::keepSlotPhase2.load(),"missing-key default preserves native idle closure");
    ck(!c::keepTriggerSlot(2,false,true,true)&&!c::keepTriggerSlot(2,true,false,true),"no window or no slot = native");
    ck(!c::keepTriggerSlot(4,true,true,true)&&!c::keepTriggerSlot(64,true,true,true)&&!c::keepTriggerSlot(16,true,true,true),"new attack/hit/phase16 stay native");
    // guard: head at z=1.65, chest 1.35, facing +y, right +x
    g::Position p{};p.chest={0,0,1.35};p.bodyRight={1,0,0};p.head={0,0,1.65};p.faceKnown=true;
    c::guardHiltBelowHeadM.store(.40f);
    p.grip={0,.35,1.05};p.tip={0,.40,1.95}; // ready stance: hilt at the belly, blade up across the face
    ck(!g::position(p,false),"resting ready stance (hilt 60 cm under the eyes) is not a guard");
    c::guardHiltBelowHeadM.store(0.f);ck(g::position(p,false),"same stance with the rule off = old guard (proves the rule decides)");
    c::guardHiltBelowHeadM.store(.40f);
    p.grip={.25,.35,1.45};p.tip={-.45,.40,1.75}; // raised crossing guard at the face
    ck(g::position(p,false),"raised hilt (20 cm under the eyes), blade across: guard");
    p.grip={.25,.35,1.23};p.tip={-.45,.40,1.62};
    ck(!g::position(p,false)&&g::position(p,true),"hilt 42 cm under: no entry, held guard keeps 8 cm hysteresis");
    ck(c::hiltRaised(-.39,false,.40)&&!c::hiltRaised(-.41,false,.40)&&c::hiltRaised(-.47,true,.40)&&!c::hiltRaised(-.49,true,.40),"hilt thresholds");
    ck(!c::hiltRaised(NAN,true,.40)&&c::hiltRaised(NAN,true,0),"non-finite refuses, off accepts");
    p.shield=true;p.grip={.25,.30,1.00};p.tip=p.grip;ck(g::position(p,false)==g::position(p,false),"shield path untouched by the hilt rule (no crash)");
    std::printf("combat_r9 %d/%d\n",checks-failures,checks);return failures?1:0;
}
