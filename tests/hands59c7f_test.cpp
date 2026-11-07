#include "vrik/hands_mode59c7f.h"
#include "vrik/hand_shape59c7f.h"
#include "activity_input163.h"
#include "diagnostics/capture168.h"
#include <cassert>
#include <limits>
using namespace kcd2vr;
int main(){
    using namespace hands59c7f;
    Hand hands[2]{{true,true,0,0},{true,true,0,0}};
    Candidate apple{true,0,100,.15f};Arbitration a;
    assert(a.update(true,1,100,hands,apple)==0);
    hands[0].grip=1;apple.tick=111;
    assert(a.update(true,1,111,hands,apple)==1);
    apple={};assert(a.update(true,1,122,hands,apple)==1); // held object leaves ray
    hands[0].grip=0;assert(a.update(true,1,133,hands,apple)==1);
    assert(a.update(true,1,193,hands,apple)==0); // native release delivery interval ends
    hands[0].grip=1;apple={false,0,200,.1f};assert(a.update(true,1,200,hands,apple)==0); // NPC is not an object
    apple.object=true;apple.tick=210;assert(a.update(true,1,210,hands,apple)==0); // closed fist entering target never grabs
    hands[0].grip=0;a.update(true,1,220,hands,apple);
    hands[0].grip=hands[0].trigger=1;apple.tick=231;
    assert(a.update(true,1,231,hands,apple)==0); // closed index + grip is boxing
    hands[0].grip=hands[0].trigger=0;a.update(true,1,240,hands,apple);
    hands[0].grip=1;apple.tick=251;assert(a.update(true,1,251,hands,apple)==1);
    hands[0].tracked=false;assert(a.update(true,1,252,hands,apple)==0); // tracking never inherits release grace
    hands[0].tracked=true;apple.tick=260;assert(a.update(true,1,260,hands,apple)==0);
    hands[0].grip=0;a.update(true,1,270,hands,apple);
    hands[0].grip=1;apple.tick=280;apple.distance=.251f;assert(a.update(true,1,280,hands,apple)==0);
    hands[0].grip=0;a.update(true,1,290,hands,apple);
    hands[0].grip=1;apple.tick=301;apple.distance=.1f;assert(a.update(true,1,301,hands,apple)==1);
    assert(a.update(false,1,302,hands,apple)==0); // activity/menu disables gameplay grab
    assert(a.update(true,2,310,hands,apple)==0); // new mode requires opening again
    rung=2;admitted=true;grabMask=1;sampleTick=100;
    assert(grabs(180)==1&&grabs(181)==0&&grabs(99)==0);clear();assert(grabs(100)==0);
    namespace cap=diagnostics::capture168;
    cap::gate.select(cap::Domain::Hands,true,100);
    assert(cap::accept("C7F F10_HANDS",101)&&cap::accept("FIST_INTENT",101));
    assert(!cap::accept("GEOM_HIT",101)&&!cap::active(cap::Domain::Combat,101));
    assert(!cap::accept("FIST_INTENT",100+cap::Duration));
    using namespace activity_input163;
    witness175.ready=true;
    for(unsigned i=0;i<4;++i){witness175.Clear();trackedHands59c7f[i]=false;
        witness175.Begin(Kind175(i),100+i);
        assert(witness175.Active()&&!TrackedHands59c7f());
        trackedHands59c7f[i]=true;assert(witness175.Active()&&TrackedHands59c7f());}
    witness175.Clear();assert(!TrackedHands59c7f());
    using namespace hand_shape59c7f;
    Shape r{},l{};r.valid=l.valid=true;r.count=l.count=1;
    r.part[0]={{-.04,0,0},{.04,0,0},.007};
    l.part[0]={{0,-.04,0},{0,.04,0},.007};
    auto c=closest(r.part[0].a,r.part[0].b,l.part[0].a,l.part[0].b);
    assert(length(c.a-c.b)<1e-10); // crossing shafts, endpoints alone would miss
    auto result=separate(r,l,{0,0,.006},{0,0,0});
    assert(result.valid&&result.contact&&result.correction[0].z>0&&result.correction[1].z<0);
    assert(length(result.correction[0]+result.correction[1])<1e-10); // no preferred hand
    assert(!separate(r,l,{1,0,0},{0,0,0}).contact);
    r.valid=false;assert(!separate(r,l,{},{0,0,.005}).valid);
    pose_core::FingerCurve103 curves[5]{};float curls[5]{};
    for(unsigned f=0;f<5;++f){curves[f].count=3;curves[f].offset[0]={double(f)*.015,.04,0};
        curves[f].offset[1]=curves[f].offset[2]={0,.02,0};}
    auto shape=make(curves,curls,{});assert(shape.valid&&shape.count==11);
    curves[2].offset[1].x=std::numeric_limits<double>::quiet_NaN();assert(!make(curves,curls,{}).valid);
}
