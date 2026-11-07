// DS-F63: grab request policy (pure, host-tested).
#include "../src/vrik/grab_request60.h"
#include <cstdio>
#include <cstring>
namespace gr=kcd2vr::grab_request60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
int main(){
    // full lifecycle: grip -> body -> proxy -> requested -> held -> release -> released -> idle
    {gr::Policy p{};
     CK(p.observeGrip(100,0,0xB0D,7,1000)==gr::Stage::BodyResolved);
     CK(p.proxyReady(1010)==gr::Stage::ProxyDriving);
     CK(p.requested(1020)==gr::Stage::Requested);
     CK(p.held(1030)==gr::Stage::Held);
     gr::Request r=p.req;CK(p.current(r,1040)); // the executor's requalification passes
     CK(p.release(1050)==gr::Stage::Releasing);
     CK(p.released(1060)==gr::Stage::Released);
     CK(p.reset()==gr::Stage::Idle);
     CK(p.req.entity==0);}
    // out-of-order transitions refused; one grab at a time
    {gr::Policy p{};
     CK(p.observeGrip(100,0,0xB0D,7,1000)==gr::Stage::BodyResolved);
     CK(p.held(1010)==gr::Stage::BodyResolved); // no ACK before the request
     CK(p.observeGrip(101,1,0xB0E,8,1020)==gr::Stage::BodyResolved); // second grip refused
     CK(p.release(1030)==gr::Stage::BodyResolved); // release refused before Held
     p.fault("x");CK(p.stage==gr::Stage::Fault);
     CK(p.reset()==gr::Stage::Idle);
     CK(p.observeGrip(102,0,0xB0F,9,1100)==gr::Stage::BodyResolved);} // fault resets
    // invalid grips fault; the requalification rejects a stale request
    {gr::Policy p{};
     CK(p.observeGrip(0,0,0xB0D,7,1000)==gr::Stage::Fault);
     CK(p.observeGrip(100,0,0,7,1000)==gr::Stage::Fault);
     CK(p.reset()==gr::Stage::Idle);
     CK(p.observeGrip(100,0,0xB0D,7,1000)==gr::Stage::BodyResolved);
     CK(p.proxyReady(1010)==gr::Stage::ProxyDriving);
     CK(p.requested(1020)==gr::Stage::Requested);
     gr::Request stale=p.req;stale.body=0xCAFE;CK(!p.current(stale,1030)); // wrong body: rejected
     CK(p.release(1040)==gr::Stage::Releasing);
     CK(p.released(1050)==gr::Stage::Released);
     CK(p.reset()==gr::Stage::Idle);}
    // DS-F66 (Claude review 16): a grab held LONGER than 5 s still requalifies
    // and releases (the time window applies only to Requested)
    {gr::Policy p{};
     CK(p.observeGrip(100,0,0xB0D,7,1000)==gr::Stage::BodyResolved);
     CK(p.proxyReady(1010)==gr::Stage::ProxyDriving);
     CK(p.requested(1020)==gr::Stage::Requested);
     CK(p.held(1030)==gr::Stage::Held);
     gr::Request r=p.req;
     CK(p.current(r,20000)); // 19 s hold: identity-based, still valid
     CK(p.release(20100)==gr::Stage::Releasing);
     CK(p.current(r,20200));
     CK(p.released(20300)==gr::Stage::Released);
     CK(p.reset()==gr::Stage::Idle);}
    // DS-F66: the fault reason survives for the visible line
    {gr::Policy p{};
     p.fault("NATIVE_BODY_REFUSED");
     CK(p.stage==gr::Stage::Fault);
     CK(std::strcmp(p.reason,"NATIVE_BODY_REFUSED")==0);
     CK(p.reset()==gr::Stage::Idle);
     CK(p.reason[0]==0);}
    if(fails)return 1;std::puts("grab request policy OK");return 0;
}
