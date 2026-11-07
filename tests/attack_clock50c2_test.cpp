#include "../src/vrik/attack_clock50c2.h"
#include <cassert>
#include <limits>
#include <vector>
using namespace kcd2vr::attackclock50;
static void advance(View& v,float dt){
    v.elapsed+=dt;v.scaledElapsed+=dt*v.rate;
    for(auto& r:v.rows){r.left=std::clamp(r.left-dt,0.f,r.left);r.scaledLeft=std::clamp(r.scaledLeft-dt*v.rate,0.f,r.scaledLeft);}
}
static bool close(float a,float b){return std::fabs(a-b)<.000005f;}
int main(){
    Binding b{};b.clock=0x1000;b.intent.kind=kcd2vr::attack_timing178r22c2::Kind::Slash;
    View v{};v.records=0x2000;v.phase=4;v.elapsed=v.scaledElapsed=.1f;v.rate=1;
    v.rows={Record{2,1,1,.9f,.9f},Record{0,.4f,.4f,.3f,.3f},Record{1,.6f,.6f,.5f,.5f}};
    for(float rate:{.5f,1.f,1.7f})for(float dt:{.005f,.016f,.04f,.08f})for(double speed:{.35,2.5,4.5,9.,30.})for(bool contact:{false,true}){
        auto native=v;native.rate=rate;const auto p=plan(b,native,dt,speed,contact);
        if(!p.adjusted)continue;
        assert(p.effectiveDt>0&&p.effectiveDt<=.10001f&&p.effectiveDt<=dt*6.0001f);
        auto actual=native,expected=native;
        actual.scaledElapsed=p.writes[0].after;actual.elapsed=p.writes[1].after;
        for(unsigned j=0;j<3;++j){actual.rows[j].left=p.writes[2+j*2].after;actual.rows[j].scaledLeft=p.writes[3+j*2].after;}
        advance(actual,dt);advance(expected,p.effectiveDt);
        assert(close(actual.elapsed,expected.elapsed)&&close(actual.scaledElapsed,expected.scaledElapsed));
        for(unsigned j=0;j<3;++j)assert(close(actual.rows[j].left,expected.rows[j].left)&&close(actual.rows[j].scaledLeft,expected.rows[j].scaledLeft));
        for(auto& w:p.writes)assert(w.address>=0x1000&&w.address<0x2040); // no phase or damage writes
    }
    // Exercise the native prepare crossing, not just the comfortable middle
    // of phase4. Post-stroke motion must never delay a native frame here.
    for(float dt:{.005f,.016f,.04f,.08f})for(float left:{.0001f,.005f,.015f,.03f})
    for(double speed:{.35,4.5,18.}){
        auto boundary=v;boundary.rows[1].left=boundary.rows[1].scaledLeft=left;
        const auto q=plan(b,boundary,dt,speed,false);
        assert(q.effectiveDt>=dt-.000001f);
        if(left<=dt){
            if(q.adjusted){boundary.scaledElapsed=q.writes[0].after;boundary.elapsed=q.writes[1].after;
                for(unsigned n=0;n<3;++n){boundary.rows[n].left=q.writes[2+n*2].after;boundary.rows[n].scaledLeft=q.writes[3+n*2].after;}}
            advance(boundary,dt);assert(remaining(boundary,0)==0);
        }
    }
    assert(!plan(b,v,.1f,9,true).adjusted); // hitch, pause, NaN and recovery stay native
    assert(!plan(b,v,0,9,true).adjusted);
    assert(!plan(b,v,std::numeric_limits<float>::quiet_NaN(),9,true).adjusted);
    v.phase=16;assert(!plan(b,v,.016f,9,true).adjusted);v.phase=4;
    auto p=plan(b,v,.016f,18,true);assert(p.adjusted&&p.contactCatchup);
    // Transaction preflight and rollback, including failure on the last write.
    for(int fail=-1;fail<8;++fail){
        std::array<float,8> memory{};for(unsigned j=0;j<8;++j)memory[j]=p.writes[j].before;
        auto index=[&](std::uintptr_t a){for(unsigned j=0;j<8;++j)if(p.writes[j].address==a)return j;assert(false);return 0u;};
        auto read=[&](std::uintptr_t a,void* dst,std::size_t n){assert(n==4);std::memcpy(dst,&memory[index(a)],4);return true;};
        bool failed=false;int writes=0;
        auto write=[&](std::uintptr_t a,const void* src,std::size_t n){assert(n==4);const auto j=index(a);++writes;
            if(int(j)==fail&&!failed){failed=true;return false;}std::memcpy(&memory[j],src,4);return true;};
        const auto result=apply(p,read,write);assert(result==(fail<0?Applied::Changed:Applied::Fault));
        for(unsigned j=0;j<8;++j)assert(memory[j]==(fail<0?p.writes[j].after:p.writes[j].before));
        memory[0]=123;writes=0;assert(apply(p,read,write)==Applied::Stale&&writes==0);
    }
    // Contact catch-up makes the native prepare deadline expire sooner, while
    // advancing the hit and total deadlines by exactly the same effective dt.
    View regular=v,tracked=v;int normalFrames=0,trackedFrames=0;
    while(remaining(regular,0)>0&&normalFrames++<100)advance(regular,.016f);
    while(remaining(tracked,0)>0&&trackedFrames++<100){const auto q=plan(b,tracked,.016f,18,true);
        if(q.adjusted){tracked.scaledElapsed=q.writes[0].after;tracked.elapsed=q.writes[1].after;
            for(unsigned j=0;j<3;++j){tracked.rows[j].left=q.writes[2+j*2].after;tracked.rows[j].scaledLeft=q.writes[3+j*2].after;}}
        advance(tracked,.016f);}
    assert(trackedFrames<normalFrames&&remaining(tracked,1)>0&&remaining(tracked,2)>0);
    // R51C1: run the WHOLE clock, not just preparation. R50C2 could get stuck
    // at 2 ms forever (also observed as effectiveDt=0.00000 in the supplied log).
    for(float rate:{.5f,1.f,1.7f})for(float dt:{.005f,.01036f,.016f,.04f})
    for(double speed:{.35,4.5,18.})for(bool touch:{false,true}){
        auto moving=v;moving.rate=rate;
        for(int frame=0;frame<400&&remaining(moving,1)>0;++frame){
            moving.phase=remaining(moving,0)>0?4:8;
            const auto q=plan(b,moving,dt,speed,touch);
            assert(q.effectiveDt>0);
            assert(q.effectiveDt>=dt-.000001f); // no phase may rewind the native step
            if(q.adjusted){moving.scaledElapsed=q.writes[0].after;moving.elapsed=q.writes[1].after;
                for(unsigned j=0;j<3;++j){moving.rows[j].left=q.writes[2+j*2].after;moving.rows[j].scaledLeft=q.writes[3+j*2].after;}}
            advance(moving,dt);
        }
        assert(remaining(moving,1)==0);
        assert(!plan(b,moving,dt,speed,touch).adjusted); // spent strike stays native
    }
    // Contact completes preparation in one native update, retaining positive
    // strike/total deadlines. This advances the existing coordinator only.
    for(float rate:{.5f,1.f,1.7f})for(float dt:{.005f,.016f,.04f,.08f}){
        auto actual=v;actual.rate=rate;auto expected=actual;
        auto q=contactPlan59c3(b,actual,dt,4.5,true);
        assert(q.adjusted&&q.contactCatchup&&close(q.effectiveDt,.498f)); // R59.16C10R17: to 2 ms before the HIT, not prepare expiry
        actual.scaledElapsed=q.writes[0].after;actual.elapsed=q.writes[1].after;
        for(unsigned j=0;j<3;++j){actual.rows[j].left=q.writes[2+j*2].after;actual.rows[j].scaledLeft=q.writes[3+j*2].after;}
        advance(actual,dt);advance(expected,q.effectiveDt);
        assert(remaining(actual,0)==0&&remaining(actual,1)>0&&remaining(actual,1)<.0021f&&remaining(actual,2)>0);
        assert(actual.phase==4); // original engine owns phase transition
        assert(close(actual.elapsed,expected.elapsed)&&close(actual.scaledElapsed,expected.scaledElapsed));
        for(unsigned j=0;j<3;++j)assert(close(actual.rows[j].left,expected.rows[j].left)&&close(actual.rows[j].scaledLeft,expected.rows[j].scaledLeft));
        assert(close(contactPlan59c3(b,v,dt,4.5,false).effectiveDt,plan(b,v,dt,4.5,false).effectiveDt));
    }
    assert(!contactPlan59c3(b,v,.2f,4.5,true).adjusted);
    {auto strike=v;strike.phase=8;strike.rows={Record{2,1,1,.4f,.4f},Record{0,.4f,.4f,0,0},Record{1,.6f,.6f,.1f,.1f}}; // R59.16C10R17: phase 8 catches up too
     auto q=contactPlan59c3(b,strike,.016f,4.5,true);assert(q.adjusted&&close(q.effectiveDt,.098f));
     strike.rows[2].left=.003f;assert(!contactPlan59c3(b,strike,.016f,4.5,true).adjusted||contactPlan59c3(b,strike,.016f,4.5,true).effectiveDt<=.016f);}
    {Contact c{};c.key=8;c.generation=9;c.epoch=3;c.request=4;c.gesture59c17=77;kcd2vr::attack_timing178r22c2::Intent i{};
     i.key=8;i.generation=9;i.epoch=3;i.request=4;i.gesture59c3=77;i.sample=5;i.tick=6;assert(touches(c,i)); // same gesture, deferred release
     c.gesture59c17=78;assert(!touches(c,i));c.sample=5;c.releaseTick=6;assert(touches(c,i));++c.key;assert(!touches(c,i));}
    auto recovery=v;recovery.phase=16;assert(!contactPlan59c3(b,recovery,.016f,4.5,true).adjusted);
    b.entity=2;b.intent.player=1;b.intent.epoch=3;b.intent.request=4;b.intent.key=8;
    b.intent.generation=9;b.intent.sample=7;b.intent.tick=1000;
    kcd2vr::contact176::Ticket ticket{1,2,10,1000,7,3,4,8,9,11};
    assert(sameRelease(b,ticket));
    ++ticket.policy;assert(sameRelease(b,ticket)); // pending-input policy is not admitted-action identity
    ++ticket.sample;assert(!sameRelease(b,ticket));--ticket.sample;
    ++ticket.tick;assert(!sameRelease(b,ticket));--ticket.tick;
    ++ticket.generation;assert(!sameRelease(b,ticket));--ticket.generation;
    ++ticket.epoch;assert(!sameRelease(b,ticket));--ticket.epoch;
    ++ticket.key;assert(!sameRelease(b,ticket));--ticket.key;
    ++ticket.gesture59c3;assert(!sameRelease(b,ticket));--ticket.gesture59c3;
    ++ticket.player;assert(!sameRelease(b,ticket));
}
