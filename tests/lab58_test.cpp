// C58 PHYSICS LAB (F9 rung 3): pure logic of src/vrik/lab58.h - rung cycling, gates, living impact distribution,
// cooldown/budget, corpse drag servo (clamp / leash / release / cooldown / duration), refusal reasons.
#include "../src/vrik/lab58.h"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace kcd2vr::lab58;
static unsigned n=0;static void ck(bool b){++n;if(!b){std::printf("check %u failed\n",n);std::fflush(stdout);}assert(b);}
static bool eq(const char* a,const char* b){return a&&b&&std::strcmp(a,b)==0;}
static bool near(double a,double b,double e=1e-9){return std::fabs(a-b)<=e;}
int main(){
    // ---- rung cycling: F9 0 -> 1 -> 2 -> 3 -> 0, clamp 0..3, rung 3 includes rung 2
    ck(nextRung(0)==1&&nextRung(1)==2&&nextRung(2)==3&&nextRung(3)==0);
    ck(clampRung(-5)==0&&clampRung(7)==3&&clampRung(3)==3&&nextRung(9)==0);
    ck(!corpseImpulse(0)&&!corpseImpulse(1)&&corpseImpulse(2)&&corpseImpulse(3));
    ck(eq(rungName(3),"PHYSICS_LAB")&&eq(rungName(2),"BOUNDED_NATIVE_CONTACT")&&eq(rungName(1),"OBSERVE_ONLY")&&eq(rungName(0),"OFF"));
    // ---- menu button: OFF -> F9 -> F9+LAB -> OFF
    MenuRagdoll m{};m=cycleMenu(m);ck(m.wanted&&!m.lab&&rungFromMenu(m)==2);
    m=cycleMenu(m);ck(m.wanted&&m.lab&&rungFromMenu(m)==3);
    m=cycleMenu(m);ck(!m.wanted&&!m.lab&&rungFromMenu(m)==0);
    ck(rungFromMenu({false,true})==0); // lab without F9 is nothing
    // ---- gates: rung 3 AND an explicit sub switch. D80 keeps the unproven
    // lab OFF at startup, including the corpse-drag request itself.
    rung.store(0);ck(!livingHitImpact.load()&&!corpseDrag.load()&&!propWake.load());
    ck(!active(Sub::LivingHitImpact)&&!active(Sub::CorpseDrag)&&!active(Sub::PropWake));
    livingHitImpact.store(true);corpseDrag.store(true);propWake.store(true);
    ck(!active(Sub::LivingHitImpact)&&active(Sub::CorpseDrag)&&!active(Sub::PropWake)); // corpse gameplay is independent of the former lab mode
    rung.store(2);ck(!active(Sub::LivingHitImpact)&&active(Sub::CorpseDrag)&&!active(Sub::PropWake)); // other experiments retain lab mode
    rung.store(3);ck(!active(Sub::LivingHitImpact)&&active(Sub::CorpseDrag)&&active(Sub::PropWake)); // D51 living actuation paused
    corpseDrag.store(false);ck(!active(Sub::LivingHitImpact)&&!active(Sub::CorpseDrag));
    ck(activeWith(3,true)&&!activeWith(3,false)&&!activeWith(2,true));
    // H17 saved fall-and-play must not leak from F4 into ordinary arrow hits.
    for(int f9=0;f9<=3;++f9){
        ck(!fallAndPlayRequestedH17(true,true,true,f9)); // D51 retires global forcing even in lab
        ck(!fallAndPlayRequestedH17(false,true,true,f9));
        ck(!fallAndPlayRequestedH17(true,false,true,f9));
        ck(!fallAndPlayRequestedH17(true,true,false,f9));
    }
    // ---- living impact distribution: J = min(400, sqrt(partMass or 4) * speed * k); parents 0.35 / 0.25
    {const int parents[2]{10,11};auto p=planImpact(12,parents,2,5.0,0,6.0);
     ck(p.count==3&&p.part[0]==12&&p.part[1]==10&&p.part[2]==11);
     ck(near(p.J[0],2*5*6)&&near(p.J[1],60*.35)&&near(p.J[2],60*.25));}
    {const int parents[2]{10,-1};auto p=planImpact(12,parents,2,5.0,9.0,6.0);ck(p.count==2&&near(p.J[0],3*5*6)&&near(p.J[1],90*.35));}
    {auto p=planImpact(12,nullptr,0,5.0,4.0,6.0);ck(p.count==1&&p.part[1]==-1);}
    {const int parents[2]{12,11};auto p=planImpact(12,parents,2,5.0,4.0,6.0);ck(p.count==1);} // a parent equal to the struck part ends the chain
    {const int parents[2]{10,11};auto p=planImpact(12,parents,2,50.0,4.0,6.0);ck(near(p.J[0],400)&&near(p.J[1],140)&&near(p.J[2],100));} // cap
    {auto p=planImpact(-1,nullptr,0,5.0,4.0,6.0);ck(p.count==0);auto q=planImpact(3,nullptr,0,0.0,4.0,6.0);ck(q.count==0);
     auto r=planImpact(3,nullptr,0,2.0,4.0,0.0);ck(r.count==0);}
    // ---- cooldown 250 ms per npc, 4 per second globally
    {ImpactBudget b{};ck(b.admit(7,1000)&&eq(b.refuse(7,1100),"NPC_COOLDOWN")&&!b.admit(7,1249)&&b.admit(7,1250));
     ck(b.admit(8,1300)&&b.admit(9,1310)); // 4 in window now: 1000? no - 1000 is <1000ms before 1310? yes 310ms -> count 4
     ck(eq(b.refuse(10,1320),"GLOBAL_BUDGET")&&!b.admit(10,1320));
     ck(b.admit(10,2001)); // the 1000 one aged out
     ck(eq(b.refuse(0,5000),"NO_ENTITY"));}
    // ---- corpse drag servo
    DragTuning t{};ck(near(t.gain,6)&&near(t.maxSpeed,3)&&near(t.grabRadius,.25)&&near(t.leash,.5)&&t.regrabMs==300&&t.maxMs==0);
    DragState s{};DragInput in{};in.now=1000;in.gripHeld=true;in.handEmpty=true;in.uiOk=true;in.handValid=true;in.hand={1,2,3};
    // refusals while idle are silent (event None) with a reason
    {DragInput x=in;x.gripHeld=false;auto r=stepDrag(s,x);ck(r.event==DragEvent::None&&eq(r.reason,"GRIP_NOT_HELD")&&!s.held);}
    {DragInput x=in;x.uiOk=false;ck(eq(stepDrag(s,x).reason,"UI_BLOCKED"));}
    {DragInput x=in;x.handEmpty=false;ck(eq(stepDrag(s,x).reason,"HAND_NOT_EMPTY"));}
    {DragInput x=in;x.handValid=false;ck(eq(stepDrag(s,x).reason,"HAND_POSE_UNAVAILABLE"));}
    {DragInput x=in;x.fault=true;ck(eq(stepDrag(s,x).reason,"FAULT"));}
    ck(eq(stepDrag(s,in).reason,"NO_CANDIDATE"));
    in.candidate.valid=true;in.candidate.entity=44;in.candidate.identity=0x1000;in.candidate.part=5;in.candidate.pos={1.3,2,3};
    ck(eq(stepDrag(s,in).reason,"OUT_OF_REACH")&&!s.held); // 0.30 > 0.25
    in.candidate.pos={1.2,2,3};
    {auto r=stepDrag(s,in);ck(r.event==DragEvent::Grab&&s.held&&s.entity==44&&s.part==5&&s.start==1000);}
    // drive: v = 6 * (hand - part), clamped to 3 m/s; direction kept
    in.entityQualified=true;in.partValid=true;in.part={1.2,2,3};in.now=1016;
    {auto r=stepDrag(s,in);ck(r.event==DragEvent::Drive&&near(r.v.x,6*(-.2))&&near(r.v.y,0)&&near(r.v.z,0));}
    in.hand={1,2,4};in.part={1,2,3};// 1 m error -> 6 m/s wanted -> clamped to 3 along +z
    {auto r=stepDrag(s,in);ck(r.event==DragEvent::Release&&eq(r.reason,"LEASH")&&!s.held);} // 1 m > 0.5 leash: released, nothing written
    // regrab cooldown 300 ms
    in.hand={1.1,2,3};in.candidate.pos={1,2,3};in.now=1200;ck(eq(stepDrag(s,in).reason,"REGRAB_COOLDOWN")&&!s.held);
    in.now=1316;{auto r=stepDrag(s,in);ck(r.event==DragEvent::Grab);}
    in.part={1,2,3};in.hand={1,2,3.4};in.now=1332;{auto r=stepDrag(s,in);ck(r.event==DragEvent::Drive&&near(r.v.z,2.4)&&near(r.v.x,0));}
    in.hand={1.3,2.4,3};{auto r=stepDrag(s,in);ck(r.event==DragEvent::Drive);const double l=length(r.v);ck(near(l,3,1e-9)&&r.v.x>0&&r.v.y>0&&near(r.v.z,0));} // 0.5 m error = the leash edge: exactly 3 m/s, direction kept
    {DragState h=s;DragInput x=in;DragTuning big=t;big.gain=20;auto r=stepDrag(h,x,big);ck(r.event==DragEvent::Drive&&near(length(r.v),3,1e-9)&&near(r.v.x/r.v.y,.3/.4,1e-9));} // clamp binds with a higher gain
    // release reasons while held
    {DragState h=s;DragInput x=in;x.gripHeld=false;auto r=stepDrag(h,x);ck(r.event==DragEvent::Release&&eq(r.reason,"GRIP_RELEASED")&&!h.held&&h.lastRelease==x.now);}
    {DragState h=s;DragInput x=in;x.uiOk=false;ck(eq(stepDrag(h,x).reason,"UI_BLOCKED"));}
    {DragState h=s;DragInput x=in;x.handEmpty=false;ck(eq(stepDrag(h,x).reason,"HAND_NOT_EMPTY"));}
    {DragState h=s;DragInput x=in;x.handValid=false;ck(eq(stepDrag(h,x).reason,"HAND_POSE_UNAVAILABLE"));}
    {DragState h=s;DragInput x=in;x.entityQualified=false;ck(eq(stepDrag(h,x).reason,"ENTITY_UNQUALIFIED"));}
    {DragState h=s;DragInput x=in;x.partValid=false;ck(eq(stepDrag(h,x).reason,"PART_POSE_UNAVAILABLE"));}
    {DragState h=s;DragInput x=in;x.fault=true;ck(eq(stepDrag(h,x).reason,"FAULT"));}
    {DragState h=s;DragInput x=in;x.now=h.start+20001;x.hand={1,2,3.1};ck(stepDrag(h,x).event==DragEvent::Drive);
     DragTuning finite=t;finite.maxMs=20000;ck(eq(stepDrag(h,x,finite).reason,"MAX_DURATION"));}
    {DragState h=s;DragInput x=in;x.now=h.start-1;ck(eq(stepDrag(h,x).reason,"TIME_REVERSED"));}
    // one grab at a time: a second candidate while held is ignored (held branch never grabs)
    {DragState h=s;DragInput x=in;x.hand={1,2,3.1};x.candidate.entity=99;x.candidate.part=1;auto r=stepDrag(h,x);ck(r.event==DragEvent::Drive&&h.entity==44&&h.part==5);}
    // a held state with the switch gone (fault/grip) releases without any output velocity
    {DragState h=s;DragInput x{};x.now=in.now;auto r=stepDrag(h,x);ck(r.event==DragEvent::Release&&near(length(r.v),0));}
    std::printf("lab58_test: %u checks OK\n",n);return 0;
}
