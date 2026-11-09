// R160 host test: VrCombatNoStep counter-step rules (subsystems/base/counter_step_r160.h), used by
// CounterStepAfterUpdateImpl160 in native_motion_owner.inl after Henry's native combat update.
// g++ -std=c++20 -I src tests/counter_step_r160_test.cpp -o counter_step_r160_test && ./counter_step_r160_test
#include "subsystems/base/counter_step_r160.h"
#include <cmath>
#include <cstdio>
using namespace kcd2vr;
namespace cs=counter_step160;
static unsigned total=0,fail=0;
static void ck(bool b,const char* s){++total;if(!b){++fail;std::printf("FAIL %s\n",s);}}
static bool near(float a,float b,float e=1e-4f){return std::fabs(a-b)<=e;}
// one frame of Henry's own attack: R112 veto of this frame (lease=now), facing +Y, entity 0x1000
static cs::Frame attack(std::uint64_t now,int phase,float x,float y,bool lease=true){
    cs::Frame f{};f.enabled=true;f.context=true;f.ordinary=true;f.phase=phase;f.now=now;f.lease=lease?now:0;
    f.entity=0x1000;f.x=x;f.y=y;f.fx=0;f.fy=1;return f;
}
int main(){
    // the pure phase sets
    ck(cs::leasePhase(4)&&cs::leasePhase(8)&&cs::leasePhase(16)&&!cs::leasePhase(32)&&!cs::leasePhase(2)&&!cs::leasePhase(64),"lease phases 4/8/16");
    ck(cs::holdPhase(32)&&!cs::holdPhase(2)&&!cs::holdPhase(64)&&!cs::holdPhase(1)&&!cs::holdPhase(0),"hold adds only phase 32");
    ck(cs::leaseFresh(1000,1100)&&!cs::leaseFresh(1000,1101)&&!cs::leaseFresh(0,5)&&!cs::leaseFresh(1000,999),"lease freshness 100 ms, never backwards");
    ck(!cs::stickActive(0,0)&&!cs::stickActive(.1f,.1f)&&cs::stickActive(0,.5f)&&cs::stickActive(-.3f,0)&&!cs::stickActive(NAN,1),"stick threshold 0.2");
    {   // switch OFF = never anything (ON means it runs, OFF means native)
        cs::State s{};auto f=attack(1000,4,0,0);f.enabled=false;auto r=cs::step(s,f);
        ck(!r.write&&!s.armed&&r.why==cs::Why::Idle,"OFF never arms");
        f=attack(1010,8,0,.5f);f.enabled=false;r=cs::step(s,f);ck(!r.write,"OFF never writes");
    }
    {   // phase 32 or a refused frame never STARTS a sequence
        cs::State s{};auto r=cs::step(s,attack(1000,32,0,0));ck(!s.armed&&!r.write,"phase 32 alone does not arm");
        r=cs::step(s,attack(1010,4,0,0,false));ck(!s.armed&&!r.write,"no R112 veto this frame = no arm");
    }
    {   // the R158 chain: natk 4->8->16->32, then the chained special 4->8->32 lunging +0.70 m
        cs::State s{};std::uint64_t t=1000;
        auto r=cs::step(s,attack(t,4,10,20));ck(s.armed&&r.why==cs::Why::Armed&&!r.write,"arm at the attack start");
        r=cs::step(s,attack(t+=11,4,10,20.02f));ck(!r.write&&r.why==cs::Why::Held,"2 cm settling inside the budget");
        r=cs::step(s,attack(t+=11,8,10,20.05f));ck(!r.write,"exactly the budget is not corrected");
        r=cs::step(s,attack(t+=11,8,10,20.30f));
        ck(r.write&&near(r.x,10)&&near(r.y,20.05f)&&near(r.removed,.25f),"forward 30 cm -> back to anchor + 5 cm");
        r=cs::step(s,attack(t+=11,16,10,20.07f));ck(r.write&&near(r.y,20.05f),"phase 16 still held");
        r=cs::step(s,attack(t+=11,32,10,20.20f,false));ck(r.write&&near(r.y,20.05f),"phase 32 after-strike held without its own veto");
        r=cs::step(s,attack(t+=11,4,10,20.06f));ck(s.armed&&near(s.ay,20)&&r.write&&near(r.y,20.05f),"chained follow-up keeps the FIRST anchor");
        r=cs::step(s,attack(t+=11,8,10,20.75f));ck(r.write&&near(r.y,20.05f)&&near(r.removed,.70f),"special lunge +70 cm removed");
        r=cs::step(s,attack(t+=11,32,10,20.06f,false));ck(r.write&&near(r.y,20.05f),"special phase 32 held");
        r=cs::step(s,attack(t+=11,2,10,20.05f));ck(!s.armed&&!r.write&&r.why==cs::Why::Released,"attack end releases at once, nothing written");
        r=cs::step(s,attack(t+=11,2,10,20.40f));ck(!r.write&&!s.armed,"walking after the attack is free");
        r=cs::step(s,attack(t+=11,4,10,20.40f));ck(s.armed&&near(s.ay,20.40f),"next attack arms a new anchor (no latch)");
    }
    {   // sideways and backward travel stay native (shoves, clinch back-off, hit push)
        cs::State s{};cs::step(s,attack(1000,4,0,0));
        auto r=cs::step(s,attack(1011,8,.40f,0));ck(!r.write,"sideways 40 cm not touched");
        r=cs::step(s,attack(1022,8,0,-.40f));ck(!r.write,"backward 40 cm not touched");
        r=cs::step(s,attack(1033,8,.40f,.30f));ck(r.write&&near(r.x,.40f)&&near(r.y,.05f),"only the forward part of a diagonal is removed");
    }
    {   // forward = Henry's CURRENT facing (the lunge goes along the clip's facing)
        cs::State s{};cs::step(s,attack(1000,4,0,0));
        auto f=attack(1011,8,.30f,0);f.fx=2;f.fy=0; // facing +X now, not normalised
        auto r=cs::step(s,f);ck(r.write&&near(r.x,.05f)&&near(r.y,0),"facing +X: 30 cm forward -> 5 cm");
        f=attack(1022,8,.30f,0);f.fx=0;f.fy=0;r=cs::step(s,f);ck(!r.write&&s.armed,"no facing = nothing written");
    }
    {   // the player's own stick / roomscale walking moves him and the anchor
        cs::State s{};cs::step(s,attack(1000,4,0,0));
        auto f=attack(1011,8,0,.40f);f.stick=true;auto r=cs::step(s,f);
        ck(!r.write&&r.why==cs::Why::Stick&&near(s.ay,.40f),"stick: free, anchor walks with him");
        r=cs::step(s,attack(1022,8,0,.43f));ck(!r.write,"after the stick: no snap back");
        r=cs::step(s,attack(1033,16,0,.60f));ck(r.write&&near(r.y,.45f),"lunge after the stick held from the new anchor");
    }
    {   // refusals release at once: kick, not ordinary (punch/clinch/masterstrike row), context, R112 refused a lease phase
        const auto check=[](auto mutate,const char* what){
            cs::State s{};cs::step(s,attack(1000,4,0,0));auto f=attack(1011,8,0,.5f);mutate(f);
            auto r=cs::step(s,f);ck(!r.write&&!s.armed&&r.why==cs::Why::Released,what);
            r=cs::step(s,attack(1022,32,0,.6f,false));ck(!r.write&&!s.armed,"released sequence is not resumed by phase 32");
        };
        check([](cs::Frame& f){f.kick=true;},"native kick releases (kicks may step)");
        check([](cs::Frame& f){f.ordinary=false;},"non-ordinary action releases");
        check([](cs::Frame& f){f.context=false;},"menu/dialogue/protected body releases");
        check([](cs::Frame& f){f.lease=0;},"R112 refusal in phase 8 releases");
        check([](cs::Frame& f){f.enabled=false;},"switch OFF mid-attack releases");
        check([](cs::Frame& f){f.phase=64;},"hit reaction / clinch phase 64 releases");
        check([](cs::Frame& f){f.x=NAN;},"non-finite position releases");
    }
    {   // a stale lease inside the 100 ms tolerance is kept (the query skipped a frame), beyond is a refusal
        cs::State s{};cs::step(s,attack(1000,4,0,0));
        auto f=attack(1090,8,0,.3f);f.lease=1000;auto r=cs::step(s,f);ck(r.write,"lease 90 ms old still holds");
        f=attack(1201,8,0,.3f);f.lease=1000;r=cs::step(s,f);ck(!r.write&&!s.armed,"lease 201 ms old releases");
    }
    {   // teleport/load and entity change
        cs::State s{};cs::step(s,attack(1000,4,0,0));
        auto r=cs::step(s,attack(1011,8,0,1.6f));ck(!r.write&&!s.armed,"jump > 1.5 m is never corrected");
        cs::step(s,attack(1022,4,0,0));auto f=attack(1033,8,5,5);f.entity=0x2000;r=cs::step(s,f);
        ck(!r.write&&s.armed&&s.entity==0x2000&&near(s.ax,5),"new entity re-anchors, nothing written");
    }
    {   // safety window: 6 s, then the next vetoed frame arms again at the current place (no latch)
        cs::State s{};cs::step(s,attack(1000,4,0,0));
        auto r=cs::step(s,attack(7000,8,0,.3f));ck(r.write,"6.0 s still held");
        r=cs::step(s,attack(7001,8,0,.3f));ck(!r.write&&!s.armed,"past 6 s released");
        r=cs::step(s,attack(7012,8,0,.3f));ck(s.armed&&near(s.ay,.3f)&&!r.write,"re-armed at once at the current place");
        r=cs::step(s,attack(6000,8,0,.9f));ck(!r.write&&!s.armed,"clock backwards releases");
    }
    {   // R164 stand-off: the native step stays while it only closes the distance to the opponent (1.10 m centre to centre)
        const auto tgt=[](cs::Frame f,float tx,float ty){f.target=true;f.tx=tx;f.ty=ty;return f;};
        cs::State s{};cs::step(s,tgt(attack(1000,4,0,0),0,2.0f));            // Capon 2.0 m straight ahead
        auto r=cs::step(s,tgt(attack(1011,8,0,.50f),0,2.0f));ck(!r.write&&r.why==cs::Why::Held,"50 cm step toward a 2 m opponent is native");
        r=cs::step(s,tgt(attack(1022,8,0,.85f),0,2.0f));ck(!r.write,"85 cm: still 1.15 m away, native");
        r=cs::step(s,tgt(attack(1033,16,0,1.20f),0,2.0f));ck(r.write&&near(r.y,.90f)&&near(r.removed,.30f),"past the stand-off: stopped at 1.10 m from him");
        ck(near(cs::allowedForward(0,0,0,1,tgt(cs::Frame{},0,1.0f)),cs::kForwardBudgetM),"already inside 1.10 m: only the 5 cm budget");
        ck(near(cs::allowedForward(0,0,0,1,tgt(cs::Frame{},0,-2.0f)),cs::kForwardBudgetM),"opponent behind: only the budget");
        ck(cs::allowedForward(0,0,0,1,tgt(cs::Frame{},1.5f,1.0f))>100.f,"line passes him wider than 1.10 m: the whole step");
        ck(near(cs::allowedForward(0,0,0,1,tgt(cs::Frame{},.66f,1.5f)),1.5f-std::sqrt(1.21f-.4356f),1e-3f),"diagonal: first point 1.10 m from him");
        ck(near(cs::allowedForward(0,0,0,1,cs::Frame{}),cs::kForwardBudgetM),"no opponent position: R160 rule unchanged");
        auto f=tgt(attack(1044,8,0,1.2f),NAN,2.0f);cs::State s2{};cs::step(s2,tgt(attack(1040,4,0,0),0,2.0f));
        r=cs::step(s2,f);ck(r.write&&near(r.y,.05f),"non-finite opponent = R160 budget");
    }
    std::printf("%s %u/%u\n",fail?"FAILED":"PASS",total-fail,total);return fail?1:0;
}
