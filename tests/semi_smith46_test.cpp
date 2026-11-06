#include "../src/vrik/semi_smith46.h"
#include "../src/vrik/semi_smith_native46.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <limits>
#include <unordered_map>
namespace s=kcd2vr::semi_smith46;
namespace n=s::native;
namespace sample=kcd2vr::workstation_sample46;
static unsigned checks=0;
static void ck(bool b){++checks;if(!b){std::fprintf(stderr,"check %u failed\n",checks);std::abort();}}
static bool close(double a,double b,double eps=.00001){return std::fabs(a-b)<eps;}
struct Driver {
    s::Controller controller;s::Context context{true,true,true,0x10000,7,11};sample::Frame frame{};
    Driver(){s::clockStatus.publish({});frame.allowed=frame.headValid=true;frame.poseEpoch=8;frame.sourceEpoch=9;
        frame.tick=1000;frame.timeNs=1000000000;frame.sequence=1;frame.head[1]=1.65f;
        for(auto& h:frame.hand){h.valid=true;h.position[1]=1.1f;}}
    void next(unsigned ms=10){frame.tick+=ms;frame.timeNs+=std::uint64_t(ms)*1000000;++frame.sequence;}
    s::Output step(float x=0,float y=0,float rt=0){return controller.step(context,frame,frame.tick,x,y,rt);}
    void baseline(){step();next();step();next();step();}
    void acknowledge(int phase){context.phase=phase;s::clockStatus.publish({context.instance,context.lifetime,controller.request(),frame.tick,phase,true});}
};
static void producer(){
    Driver d;ck(d.step().reason==s::Reason::Rebased);d.next();d.frame.hand[1].position[2]-=.0028f;
    auto out=d.step();ck(out.axesOwned&&close(out.y,1)&&out.x==0);d.next();out=d.step();ck(out.axesOwned&&out.x==0&&out.y==0);
    d.next();d.frame.hand[1].position[0]-=.0014f;out=d.step();ck(close(out.x,-.5)&&out.y==0);
    // Looking left after entry does not rotate the workpiece travel plane.
    d.next();d.frame.headQ[1]=.70710678f;d.frame.headQ[3]=.70710678f;d.frame.head[0]+=.25f;out=d.step();ck(out.x==0&&out.y==0);
    d.next();d.frame.hand[1].position[2]+=.0014f;out=d.step();ck(close(out.y,-.5)&&out.x==0);
    // A duplicate sample is a bounded rate hold, never an extra displacement.
    const auto held=d.controller.step(d.context,d.frame,d.frame.tick+20,0,0,0);ck(close(held.y,-.5));
    const auto expired=d.controller.step(d.context,d.frame,d.frame.tick+33,0,0,0);ck(!expired.axesOwned&&expired.y==0);
    d.next();d.frame.hand[1].position[0]+=.0028f;out=d.step(.4f,.3f);ck(!out.axesOwned);
    d.next();out=d.step();ck(!out.axesOwned);d.next();out=d.step();ck(out.axesOwned&&out.x==0&&out.y==0);
    for(int phase:{12,13,0,15,20}){d.context.phase=phase;d.next();out=d.step();ck(!out.axesOwned&&!out.requestRT&&!out.clock.enabled);}
    for(int phase:{11,14,16,17}){Driver a;a.context.phase=phase;a.step();a.next();a.frame.hand[1].position[2]-=.0014f;out=a.step();ck(out.axesOwned&&close(out.y,.5));}
    Driver invalid;invalid.baseline();invalid.next();invalid.frame.hand[1].position[0]=std::numeric_limits<float>::quiet_NaN();out=invalid.step();ck(!out.axesOwned);
    invalid.next();invalid.frame.hand[1].position[0]=0;out=invalid.step();ck(!out.axesOwned&&std::isfinite(out.x));
    invalid.next();out=invalid.step();ck(out.axesOwned&&out.x==0&&out.y==0);
    Driver forgeRecover;forgeRecover.context.phase=8;forgeRecover.baseline();forgeRecover.next();forgeRecover.frame.hand[1].valid=false;forgeRecover.step();
    forgeRecover.next();forgeRecover.frame.hand[1].valid=true;forgeRecover.frame.hand[1].position[2]=-.30f;out=forgeRecover.step();ck(!out.forgeStep);
    forgeRecover.next();out=forgeRecover.step();ck(!out.forgeStep&&out.y==0);
    Driver gap;gap.baseline();gap.next(90);gap.frame.hand[1].position[2]-=.02f;out=gap.step();ck(out.reason==s::Reason::Discontinuity&&!out.axesOwned);
    Driver reset;reset.baseline();reset.next();++reset.frame.poseEpoch;reset.frame.hand[1].position[2]-=.2f;out=reset.step();ck(out.reason==s::Reason::Rebased&&!out.axesOwned);
    Driver forge;forge.context.phase=8;forge.baseline();forge.next(20);forge.frame.hand[1].position[2]-=.07f;out=forge.step();ck(out.forgeStep&&out.y==1);
    unsigned repeats=1;
    for(unsigned i=0;i<60;++i){forge.next();out=forge.step();if(out.forgeStep)++repeats;if(i>=6)ck(out.y==0);}
    ck(repeats==1);forge.next(20);forge.frame.hand[1].position[2]+=.07f;out=forge.step();ck(!out.forgeStep);
    forge.next(20);forge.frame.hand[1].position[2]+=.07f;out=forge.step();ck(out.forgeStep&&out.y==-1);
    // Queue/request lifecycle is exercised on the real hand producer.
    Driver h;h.baseline();h.next(20);h.frame.hand[0].position[1]+=.032f;out=h.step();
    ck(out.strokeStarted&&out.requestRT&&out.clock.enabled&&out.clock.requestedPhase==14);const auto request=out.clock.request;
    const double first=out.clock.raiseWork;h.next();h.acknowledge(14);out=h.step();ck(out.requestRT&&out.clock.request==request&&out.clock.raiseWork==first);
    h.next(20);h.frame.hand[0].position[1]+=.04f;h.acknowledge(14);out=h.step();ck(out.requestRT&&out.clock.raiseWork>first);
    const double raised=out.clock.raiseWork;h.next();h.acknowledge(14);out=h.step();ck(out.clock.raiseWork==raised&&out.reason==s::Reason::Pause);
    h.next(20);h.frame.hand[0].position[1]-=.025f;h.acknowledge(14);out=h.step();ck(!out.requestRT&&out.strokeReleased&&out.clock.requestedPhase==16&&out.clock.finishWork>0);
    h.next();h.acknowledge(16);out=h.step();ck(out.clock.enabled&&out.clock.request==request);const double down=out.clock.finishWork;
    h.next(20);h.frame.hand[0].position[1]-=.035f;h.acknowledge(16);out=h.step();ck(out.clock.finishWork>down&&!out.requestRT);
    h.next();h.acknowledge(16);out=h.step();const auto stopped=out.clock.finishWork;h.next();h.acknowledge(16);out=h.step();ck(out.clock.finishWork==stopped);
    h.context.phase=17;h.next();out=h.step();ck(!out.clock.enabled&&!out.requestRT&&out.reason==s::Reason::NativeComplete);
    Driver lose;lose.baseline();lose.next(20);lose.frame.hand[0].position[1]+=.04f;lose.step();lose.next();lose.acknowledge(14);lose.step();
    lose.next();lose.frame.hand[0].valid=false;out=lose.step();ck(!out.clock.enabled&&!out.requestRT&&out.strokeReleased&&out.reason==s::Reason::NativeContinuation);
    Driver manual;manual.baseline();manual.next(20);manual.frame.hand[0].position[1]+=.04f;manual.step();manual.next();manual.acknowledge(14);out=manual.step(0,0,.8f);ck(out.reason==s::Reason::ManualStroke&&!out.clock.enabled&&!out.requestRT);
    Driver refused;refused.baseline();refused.next(20);refused.frame.hand[0].position[1]+=.04f;refused.step();
    refused.context.phase=14;for(int i=0;i<17;++i){refused.next();out=refused.step();}
    ck(out.reason==s::Reason::ClockNotOwned&&!out.requestRT&&!out.clock.enabled);
    Driver noAck;noAck.baseline();noAck.next(20);noAck.frame.hand[0].position[1]+=.04f;noAck.step();for(int i=0;i<46;++i){noAck.next();out=noAck.step();}
    ck(out.reason==s::Reason::PressNotAccepted&&!out.requestRT&&!out.clock.enabled);
    Driver delayed;delayed.baseline();delayed.next(20);delayed.frame.hand[0].position[1]+=.04f;delayed.step();
    delayed.next(20);delayed.context.phase=14;delayed.frame.hand[0].position[1]-=.025f;out=delayed.step();
    ck(out.requestRT&&out.clock.requestedPhase==14&&!out.strokeReleased); // phase alone never authorizes finish ownership
    delayed.next();delayed.acknowledge(14);out=delayed.step();ck(!out.requestRT&&out.strokeReleased&&out.clock.requestedPhase==16);
    for(int i=0;i<46;++i){delayed.next();out=delayed.step();}
    ck(out.reason==s::Reason::FinishNotAccepted&&!out.clock.enabled&&!out.requestRT);
    for(int phase:{11,16,17}){Driver unexpected;unexpected.baseline();unexpected.next(20);unexpected.frame.hand[0].position[1]+=.04f;unexpected.step();
        unexpected.next();unexpected.acknowledge(14);unexpected.step();unexpected.next();unexpected.context.phase=phase;out=unexpected.step();
        ck(!out.clock.enabled&&!out.requestRT&&out.reason==s::Reason::NativeContinuation);}
    Driver off;off.baseline();off.context.clockAvailable=false;off.next();off.frame.hand[1].position[0]+=.0014f;out=off.step();ck(out.axesOwned&&out.x>0&&!out.requestRT);
    struct Pad{float lx=0,ly=0,rt=.7f;} pad;s::apply(out,pad);ck(pad.rt==.7f&&pad.lx>0);
}
struct Memory {
    std::unordered_map<std::uintptr_t,unsigned char> bytes;std::uintptr_t unreadable=0,unwritable=0;unsigned childReads=0;
    template<class T>void put(std::uintptr_t p,T v){const auto* b=reinterpret_cast<const unsigned char*>(&v);for(std::size_t i=0;i<sizeof(v);++i)bytes[p+i]=b[i];}
    template<class T>bool read(std::uintptr_t p,T& v){if(p>=0x6000&&p<0x6100)++childReads;if(p==unreadable)return false;
        auto* b=reinterpret_cast<unsigned char*>(&v);for(std::size_t i=0;i<sizeof(v);++i){const auto at=bytes.find(p+i);if(at==bytes.end())return false;b[i]=at->second;}return true;}
    bool write(std::uintptr_t p,float v){if(p==unwritable)return false;put(p,v);return true;}
};
static Memory fixture(){
    Memory m;const std::uintptr_t b=0x10000000,a=0x1000,p=0x2000,model=0x3000,player=0x4000,ctrl=0x5000,child=0x6000,scope=0x7000;
    m.put(a,b+n::kInstanceVft);m.put(a+0x2C,std::uint8_t(1));m.put(a+0x18,player);m.put(player,b+n::kPlayerVft);m.put(player+0x30,42u);
    m.put(a+0x98,model);m.put(model,b+n::kModelVft);m.put(model+0x88,std::uint8_t(14));m.put(a+0x70,b+n::kListenerVft);m.put(a+0xC8,p);
    m.put(p,b+n::kParentVft);m.put(p+0x10,b+n::kParentActionVft);m.put(p+0x28,std::uint8_t(1));m.put(p+0x50,std::uint8_t(10));m.put(p+0x78,player);m.put(p+0x80,a+0x70);m.put(p+0x88,child);
    m.put(ctrl,b+n::kControllerVft);m.put(ctrl+0x20,std::uintptr_t(0x9000));m.put(ctrl+0x34,1u);m.put(ctrl+0x38,scope);m.put(ctrl+0x48,std::uintptr_t(0x8000));m.put(ctrl+0x50,std::uintptr_t(0x8008));m.put(0x8000,child);
    m.put(scope+0x148,std::uintptr_t(0));m.put(scope+0x150,std::uintptr_t(0));m.put(child,b+n::kChildVft);m.put(child+8,std::uintptr_t(0x9000));m.put(child+0x30,std::uintptr_t(0));m.put(child+0x28,int(1));m.put(child+0x54,55u);m.put(child+0x5C,1.f);m.put(child+0x10,0.f);return m;
}
static void nativeReader(){
    const std::uintptr_t base=0x10000000;
    auto m=fixture();auto rd=[&](auto at,auto& v){return m.read(at,v);};auto wr=[&](auto at,float v){return m.write(at,v);};
    n::Parent p{};ck(n::parent(rd,base,0x1000,0,0,42,false,p)&&p.parent==0x2000&&p.code==10);
    m.put(0x10C8,std::uintptr_t(0));m.put(0x2088,std::uintptr_t(0));ck(!n::parent(rd,base,0x1000,0x2000,0x6000,42,false,p));
    ck(n::parent(rd,base,0x1000,0x2000,0x6000,42,true,p));
    m.put(0x2050,std::uint8_t(11));ck(!n::parent(rd,base,0x1000,0x2000,0x6000,42,true,p));m.put(0x3088,std::uint8_t(16));ck(n::parent(rd,base,0x1000,0x2000,0x6000,42,true,p));
    auto member=n::membership(rd,base,0x5000,0x6000);ck(member.kind==n::Membership::Queued);n::Child c{};ck(n::child(rd,base,0x5000,0x6000,member.kind,c)&&c.status==1);
    n::Lease lease{};lease.owner=p;lease.controller=0x5000;lease.token=55;lease.saved=1.f;lease.lastWritten=0;lease.written=true;m.put(0x605C,0.f);
    // Native queue -> installed root preserves the same lease and token.
    m.put(0x5050,std::uintptr_t(0x8000));m.put(0x7148,std::uintptr_t(0x6000));m.put(0x6030,std::uintptr_t(0x7000));m.put(0x6028,int(2));
    ck(n::membership(rd,base,0x5000,0x6000).kind==n::Membership::Root);ck(n::restore(rd,wr,base,lease)==n::Restore::Restored);float speed=0;ck(m.read(0x605C,speed)&&speed==1);
    auto claim=[&]{lease={};lease.owner.child=0x6000;lease.controller=0x5000;lease.token=55;lease.saved=1;lease.lastWritten=0;lease.written=true;m.put(0x605C,0.f);};
    claim();m.unreadable=0x5050;ck(n::restore(rd,wr,base,lease)==n::Restore::Pending&&lease.written);m.unreadable=0;
    m.unwritable=0x605C;ck(n::restore(rd,wr,base,lease)==n::Restore::Pending&&lease.written);m.unwritable=0;ck(n::restore(rd,wr,base,lease)==n::Restore::Restored);
    claim();m.put(0x605C,.7f);ck(n::restore(rd,wr,base,lease)==n::Restore::Foreign&&m.read(0x605C,speed)&&speed==.7f);
    claim();lease.uncertainWrite=true;lease.candidate=.6f;m.put(0x605C,.6f);ck(n::restore(rd,wr,base,lease)==n::Restore::Restored&&m.read(0x605C,speed)&&speed==1);
    claim();m.put(0x6054,56u);ck(n::restore(rd,wr,base,lease)==n::Restore::Ended&&m.read(0x605C,speed)&&speed==0);m.put(0x6054,55u);
    claim();m.put(0x6030,std::uintptr_t(0));ck(n::restore(rd,wr,base,lease)==n::Restore::Pending);m.put(0x6030,std::uintptr_t(0x7000));
    claim();m.put(0x7148,std::uintptr_t(0));const auto reads=m.childReads;ck(n::restore(rd,wr,base,lease)==n::Restore::Ended&&m.childReads==reads);
    n::WorkBudget budget;budget.begin(0,1000);ck(budget.rate(.013,0,.02f,1,1010)>.6499f);ck(budget.rate(.013,.013f,.02f,1,1020)<.000001f);
    budget.begin(0,1000);ck(close(budget.rate(.02,0,.01f,2,1010),1)); // same work under changed native time scale
    ck(budget.rate(.02,.02f,.01f,2,1020)<.000001f);ck(budget.rate(.02,.02f,.01f,2,1020)<.000001f);
    budget.begin(0,1000);ck(budget.rate(.1f,0,.01f,1,1100)==0);ck(budget.rate(.1f,0,.01f,1,1110)==0);
    ck(budget.rate(std::numeric_limits<double>::quiet_NaN(),0,.01f,1,1120)==0);
    budget.begin(0,1000);ck(close(budget.rate(.2,0,.01f,.01f,1010),8));
    ck(budget.rate(.2,.0008f,.01f,.01f,1020)==0); // no stored swing in native slow-motion
    ck(budget.rate(.2,.0008f,.01f,.01f,1030)==0);
}
int main(){producer();nativeReader();std::printf("PASS C46 semi smith: %u producer/native reader/lease/work-budget checks\n",checks);return 0;
}
