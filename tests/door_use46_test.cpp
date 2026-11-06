#include "../src/core/door_use46.h"
#include "../src/core/native_door_script19c4r4.h"
#include <cassert>
#include <cstdio>
#include <vector>
using namespace kcd2vr;
static unsigned checks=0;
static void checkAt(bool ok,int line){++checks;if(!ok)std::fprintf(stderr,"door use check %u failed at line %d\n",checks,line);assert(ok);}
#define check(x) checkAt((x),__LINE__)
static door_use46::Input input(std::uint64_t now=1000,int side=0){
    return {true,true,side,now,900,now,7,3,11,0x1234,{.1f,1.2f,.3f}};
}
static door_use46::Selection selection(int side=0){return {true,42,side,1000,1000,7,3,0x1234,{0,0,.08f},.08f};}
struct IO {
    bool permitted=true,haveAck=false;door19c4::Point point{};
    door_script19c4r4::Ack answer{};std::vector<door_script19c4r4::Command> sent;
    bool publish(door_script19c4r4::Command c){sent.push_back(c);return true;}
    bool ack(door_script19c4r4::Ack& a){a=answer;return haveAck;}
    bool allowed(const door19c4::Request&){return permitted;}
    bool hand(const door19c4::Request&,door19c4::Point& p){p=point;return true;}
    void receipt(const door19c4::Request&,const char*,float,bool){}
    void held(std::uint64_t now,float f=.1f){const auto& c=sent.back();answer={c.sequence,c.request,1,f,now};haveAck=true;}
};
int main(){
    // H23 regression: a native pre-press bounds-centre nominee is not a
    // physical surface and cannot mint a token, fake distance0, or pulse Use.
    for(int side=0;side<2;++side){
        door_use46::Controller c;auto i=input(1000,side);
        door_pre_press66::Entry e{};e.target=42;e.side=side;e.entity=i.player;
        e.tick=999;e.poseEpoch=i.poseEpoch;e.sourceGeneration=i.sourceGeneration;
        e.point[0]=20.f; // deliberately nowhere near the palm
        door_pre_press66::store(side,e);
        auto p=c.update(i,{});check(p.owns&&!p.pulse&&!p.target&&!c.chosen.token);
        i.now=i.poseTick=1020;p=c.update(i,{});check(p.owns&&!p.pulse&&!p.target&&!c.chosen.token);
        // The original native nominee is now joined to an actual20cm surface
        // by the producer; that real selection, rather than cache age, permits.
        auto s=selection(side);s.tick=s.poseTick=1020;
        p=c.update(i,s);check(p.owns&&p.pulse&&p.target==42&&c.chosen.distance==.08f);
        c.complete(p,true);i.available=false;i.now=1040;c.update(i,{});
        door_pre_press66::clear(side);
    }
    for(int side=0;side<2;++side){
        door_use46::Controller c;auto i=input(1000,side);auto s=selection(side);
        auto p=c.update(i,s);check(p.owns&&p.pulse&&p.target==42&&p.side==side);
        c.complete(p,true);door_use46::Intent captured{};check(door_use46::read(captured,1000));
        check(captured.motion.y==0&&captured.distance==.08f);
        for(unsigned n=1;n<=40;++n){i.now+=10;i.poseTick=i.now;s.target=99;s.point={8,8,8};
            p=c.update(i,s);check(p.owns&&!p.pulse&&p.target==42);}
        check(door_use46::current(captured,i.now)); // Native selection may leave inUse door; input stays exact.
        check(door_use46::consume(captured,i.now));check(!door_use46::consume(captured,i.now));
        i.available=false;p=c.update(i,s);check(!door_use46::current(captured,i.now));
        i.available=true;i.now+=10;i.poseTick=i.now;p=c.update(i,s);check(p.owns&&!p.pulse);
        i.press=i.now;i.now+=20;i.poseTick=i.now;s=selection(side);s.tick=s.poseTick=i.now;
        p=c.update(i,s);check(p.pulse&&p.target==42);c.complete(p,false);
        i.now+=10;i.poseTick=i.now;check(!c.update(i,s).pulse);
    }
    {door_use46::Controller c;auto i=input();i.available=false;i.waitingForRay49=true;
     auto p=c.update(i,{});check(p.owns&&!p.pulse&&!c.cancelled);
     i.now+=20;i.poseTick=i.now;i.available=true;i.waitingForRay49=false;auto s=selection();s.tick=s.poseTick=i.now;
     p=c.update(i,s);check(p.owns&&p.pulse&&p.target==42);c.complete(p,true);
     i.available=false;i.waitingForRay49=true;p=c.update(i,s);check(!door_use46::current(c.chosen,i.now));}

    for(int loss=0;loss<8;++loss){door_use46::Controller c;auto i=input();auto s=selection();
        auto p=c.update(i,s);c.complete(p,true);door_use46::Intent before{};check(door_use46::read(before,1000));
        i.now=1010;i.poseTick=1010;
        if(loss==0)i.available=false;if(loss==1)++i.poseEpoch;if(loss==2)++i.player;
        if(loss==3)++i.sourceGeneration;if(loss==4)++i.trackingEpoch;if(loss==5)i.poseTick=800;
        if(loss==6)i.motion.x=std::nanf("");if(loss==7)i.now=1100;
        p=c.update(i,s);check(!p.pulse&&!door_use46::current(before,i.now));
    }
    for(int bad=0;bad<8;++bad){door_use46::Controller c;auto i=input();auto s=selection();
        if(bad==0)s.distance=.201f;if(bad==1)s.tick=919;if(bad==2)s.poseTick=919;if(bad==3)s.poseEpoch=8;
        if(bad==4)s.player=88;if(bad==5)s.sourceGeneration=4;if(bad==6)s.point.z=std::nanf("");if(bad==7)s.tick=1001;
        auto p=c.update(i,s);check(p.owns&&!p.pulse);
    }
    {door_use46::Controller c;auto s=selection();s.ordinary=false;check(!c.update(input(),s).owns);}
    {door_use46::Controller c;auto i=input();auto s=selection(1);
     auto p=c.update(i,s);check(p.owns&&!p.pulse&&p.target==0);
     i.now=i.poseTick=1020;s=selection();s.tick=s.poseTick=1020;p=c.update(i,s);check(p.owns&&p.pulse&&p.target==42);c.complete(p,true);
    }
    {door_use46::Controller c;auto i=input();auto s=selection(1);auto p=c.update(i,s);check(p.owns&&!p.pulse);
     i.now=i.poseTick=1020;i.available=false;c.update(i,s);i.now=i.poseTick=1040;i.available=true;s=selection();s.tick=s.poseTick=1040;
     p=c.update(i,s);check(p.owns&&!p.pulse); // Tracking loss while waiting cannot revive a squeeze.
    }
    {door_script19c4r4::Owner o;IO io;door19c4::Request r{};
     r.serial=1;r.tick=1000;r.id=42;r.inputToken46=1;
     o.tick(io,r,1000);check(o.pending&&io.sent.back().phase==1);
     io.point={.2f,0,0};io.held(1050);o.tick(io,r,1050);
     check(o.active&&io.sent.back().fraction==.1f); // Pre-ACK hand/body approach is discarded.
     io.point={.32f,0,0};io.held(1060);o.tick(io,r,1060);check(std::fabs(io.sent.back().fraction-.5f)<.00001f);
     const float held=io.sent.back().fraction;
     for(unsigned n=1;n<=20;++n){io.point={.32f+(n%2?.0005f:-.0005f),0,0};io.held(1060+n*10);o.tick(io,r,1060+n*10);
         check(io.sent.back().fraction==held);}
     io.point={.26f,0,0};io.held(1270);o.tick(io,r,1270);check(std::fabs(io.sent.back().fraction-.3f)<.00001f);
     io.permitted=false;o.tick(io,r,1280);check(!o.active&&io.sent.back().phase==0);
    }
    {door_use46::Controller c;auto i=input();auto s=selection();auto p=c.update(i,s);c.complete(p,true);
     door_use46::Intent old{};check(door_use46::read(old,1000));
     door_use46::SetEnabled(false);door_use46::SetEnabled(true); // No intervening input frame.
     i.now=i.poseTick=1010;p=c.update(i,s);check(p.owns&&!p.pulse&&!door_use46::current(old,1010));
     i.press=1020;i.now=i.poseTick=1040;s.tick=s.poseTick=1040;p=c.update(i,s);check(p.pulse);c.complete(p,true);
     door_use46::Intent next{};check(door_use46::read(next,1040)&&next.token!=old.token&&next.settingsEpoch!=old.settingsEpoch);
    }
    {door_use46::Controller c;door_use46::SetEnabled(false);auto i=input();auto s=selection();
     check(!c.update(i,s).owns); // Native controls remain available while OFF.
     door_use46::SetEnabled(true);i.now=i.poseTick=1010;auto p=c.update(i,s);check(p.owns&&!p.pulse);
     i.press=1020;i.now=i.poseTick=1040;s.tick=s.poseTick=1040;check(c.update(i,s).pulse);
    }
    {door_use46::Controller c;auto i=input();door_use46::Selection absent{};
     for(unsigned n=0;n<8;++n){i.now=i.poseTick=1000+n*16;i.available=(n%2)!=0;
         auto p=c.update(i,absent);check(p.owns&&!p.pulse&&c.cancelled);}
     door_use46::SetEnabled(false);check(c.update(i,absent).owns);door_use46::SetEnabled(true);
     i.press=1120;i.now=i.poseTick=1140;auto s=selection();s.tick=s.poseTick=i.now;
     auto p=c.update(i,s);check(p.owns&&p.pulse);c.complete(p,true);
    }
    std::printf("door_use46: %u checks PASS; actual input token and script-owner motion policy\n",checks);
}
