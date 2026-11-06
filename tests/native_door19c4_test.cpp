#include "../src/core/native_door19c4.h"
#include "../src/core/native_door_start19c4.h"
#include <cassert>
#include <cstring>
#include <string>
#include <vector>
using namespace kcd2vr::door19c4;
// Native leaves only: the production Owner above performs every transition.
struct Native {
    bool permitted=true,fresh=true,same=true,layerSame=true,speedOK=true,timeOK=true;
    float t=.1f,rate=1;Point p{};int writes=0,speeds=0;
    std::vector<std::string> events;
    bool allowed(const Request&)const{return permitted;}
    bool hand(const Request&,Point& out)const{out=p;return fresh;}
    bool acquire(const Request& r,Binding& b){b={r.entity,22,33,44,r.id,7,t,rate};return same;}
    bool current(const Binding&,float& out,float& scale){out=t;scale=rate;return same;}
    int restore(const Binding& b){if(!layerSame||rate!=0.f)return 0;return speed(b,b.savedRate)?1:-1;}
    bool speed(const Binding&,float v){if(!speedOK)return false;rate=v;++speeds;return true;}
    bool time(const Binding&,float v){if(!timeOK)return false;t=v;++writes;return true;}
    void receipt(const Request&,const char* s,float,bool){events.emplace_back(s);}
};
static Request request(){return {1,1000,5,11,12,13,21,0,{}};}
int main(){
    namespace start=kcd2vr::door_start19c4;
    start::arm(33,1,1000);assert(!start::matches(33,1,1000,7)); // no old-clip adoption
    start::started(34,0,7,1001);assert(!start::matches(33,1,1001,7));
    start::started(33,1,7,1001);assert(!start::matches(33,1,1001,7));
    start::started(33,0,7,1002);assert(start::matches(33,1,1003,7));
    assert(!start::matches(33,1,1003,8)&&!start::matches(33,2,1003,7)&&!start::matches(33,1,1253,7));
    start::arm(33,2,1004);assert(!start::matches(33,2,1004,7)); // reused FIFO/ID needs new start
    start::started(33,0,7,1005);assert(start::matches(33,2,1006,7));
    Owner o;Native n;auto r=request();o.tick(n,r,1000);
    assert(o.active&&n.rate==0&&n.writes==0&&n.events.back()=="ACQUIRED");
    n.p={.01f,0,0};o.tick(n,r,1010);assert(n.writes==0); // tremor does not drive door
    n.p={.11f,0,0};o.tick(n,r,1020);assert(n.writes==1&&std::fabs(n.t-.3f)<.001f);
    n.p={.055f,0,0};o.tick(n,r,1030);assert(std::fabs(n.t-.2f)<.001f); // reverse same clip
    n.p={.9f,0,0};o.tick(n,r,1040);assert(n.t==.9995f&&n.rate==0); // never fake native completion
    n.permitted=false;o.tick(n,r,1050);assert(!o.active&&n.rate==1&&n.speeds==2);
    n.permitted=true;o.tick(n,r,1060);assert(!o.active); // same request cannot reacquire
    // Native replacement or another speed writer is never overwritten on cleanup.
    for(int kind=0;kind<2;++kind){Owner a;Native b;a.tick(b,r,1000);if(kind==0){b.same=false;b.layerSame=false;}else b.rate=.7f;
        a.tick(b,r,1010);assert(!a.active&&b.speeds==1);}
    // Sustained loss, menu/rung change, recenter/leave, and timeout restore once.
    for(int kind=0;kind<4;++kind){Owner a;Native b;a.tick(b,r,1000);std::uint64_t tick=1010;
        if(kind==0)b.fresh=false;if(kind==1)b.permitted=false;if(kind==2)b.p={2,0,0};if(kind==3)tick=16001;
        a.tick(b,r,tick);a.tick(b,r,tick+1);assert(!a.active&&b.rate==1&&b.speeds==2);}
    // Refused native action never publishes a Request; stale/foreign/no-clip requests cannot freeze.
    {Owner a;Native b;Request none{};a.tick(b,none,1000);assert(!a.active&&b.speeds==0);}
    for(int kind=0;kind<4;++kind){Owner a;Native b;if(kind==0)b.permitted=false;if(kind==1)b.same=false;
        if(kind==2)b.rate=.5f;a.tick(b,r,kind==3?1300:1000);assert(!a.active&&b.speeds==0);}
    {Owner a;Native b;a.tick(b,r,1000);b.timeOK=false;b.p={.1f,0,0};a.tick(b,r,1010);assert(!a.active&&b.rate==1);}
    // Native Lua starts its leaf AFTER StartInteractiveAction returns; a pending
    // accepted request waits within 250 ms, without freezing the previous clip.
    {Owner a;Native b;b.same=false;a.tick(b,r,1000);assert(!a.active&&a.pending&&b.speeds==0);
        b.same=true;a.tick(b,r,1010);assert(a.active&&b.speeds==1);}
    // Native Lua starts its door clip AFTER the accepted Henry action returns.
    {Owner a;Native b;b.same=false;a.tick(b,r,1000);assert(!a.active&&a.pending&&b.speeds==0);
        b.same=true;a.tick(b,r,1010);assert(a.active&&b.speeds==1);}
    // Replacement clip never receives time writes, but our layer0 pause is released.
    {Owner a;Native b;a.tick(b,r,1000);b.same=false;a.tick(b,r,1010);
        assert(!a.active&&b.rate==1&&b.writes==0);}
    {Owner a;Native b;a.tick(b,r,1000);b.t=.7f;a.tick(b,r,1010);
        assert(!a.active&&b.rate==1&&b.writes==0);} // foreign same-clip time writer wins
    // A failed owned-layer restore retains the lease and retries without motion.
    {Owner a;Native b;a.tick(b,r,1000);b.permitted=false;b.speedOK=false;a.tick(b,r,1010);
        assert(a.active&&a.restorePending&&b.rate==0);b.speedOK=true;a.tick(b,r,1020);
        assert(!a.active&&!a.restorePending&&b.rate==1);}
    // New accepted door request releases the old leaf before taking the new one.
    {Owner a;Native b;a.tick(b,r,1000);auto r2=r;r2.serial=2;r2.tick=1010;a.tick(b,r2,1010);
        assert(a.active&&b.speeds==3&&b.events[1]=="REPLACED");}
}
