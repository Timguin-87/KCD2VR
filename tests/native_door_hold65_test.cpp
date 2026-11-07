#include "../src/core/native_door_hold65.h"
#include "../src/core/door_channel19c4r4.h"
#include <cassert>
#include <cstdio>
#include <vector>
using namespace kcd2vr;
static unsigned checks=0;
static void check(bool v){++checks;assert(v);}
struct IO {
    bool allowed=true;door_hold65::Hand hand{};door_script19c4r4::Ack answer{};
    std::vector<door_script19c4r4::Command> commands;
    bool control65(const door19c4::Request&,door_hold65::Hand& out){out=hand;return allowed;}
    bool publish(door_script19c4r4::Command c){commands.push_back(c);return true;}
    bool ack(door_script19c4r4::Ack& out){out=answer;return answer.request!=0;}
    void receipt(const door19c4::Request&,const char*,float,bool){}
    void fresh(std::uint64_t tick,float grip,std::uint64_t press){hand.tick=hand.poseTick=tick;hand.grip=grip;hand.press=press;}
    void ackLast(std::uint64_t tick,int state=1,float fraction=.2f){const auto c=commands.back();answer={c.sequence,c.request,state,fraction,tick};}
};
static door19c4::Request request(){door19c4::Request r{};r.serial=1;r.tick=1000;r.id=42;r.entity=0x4200;r.player=0x100;r.epoch=7;r.side=0;r.inputPress65=990;
    r.sourceGeneration=9;r.trackingEpoch46=8;r.inputSettingsEpoch46=10;r.inputExperimentEpoch65=11;return r;}
static IO input(){IO io;io.hand.available=true;io.hand.side=0;io.hand.player=0x100;io.hand.poseEpoch=7;
    io.hand.trackingEpoch=8;io.hand.sourceGeneration=9;io.hand.settingsEpoch=10;io.hand.experimentEpoch=11;io.fresh(1000,1,990);return io;}
int main(){
    using namespace door_script19c4r4;
    HoldOwner65 o;auto io=input();const auto r=request();
    o.tick(io,r,1000);check(o.pending&&!o.active&&io.commands.back().phase==1);
    io.fresh(1010,1,990);io.ackLast(1010);o.tick(io,r,1010);
    check(o.active&&!o.paused&&io.commands.back().phase==3);
    door_hold65::Operation operation{};check(door_hold65::operation.read(operation)&&!operation.paused);
    io.fresh(1020,0,0);io.ackLast(1020,1,.3f);o.tick(io,r,1020);
    check(o.active&&o.paused&&io.commands.back().phase==4);
    check(door_hold65::operation.read(operation)&&operation.paused);
    // Long normal release is a pause, not the old fifteen-second native finish.
    for(unsigned n=1;n<=1000;++n){const auto now=1020+n*20;io.fresh(now,0,0);io.hand.point={float(n),-4,2};
        io.ackLast(now,1,.3f);o.tick(io,r,now);check(o.active&&o.paused&&io.commands.back().phase==4&&o.wanted==.3f);}
    std::uint64_t now=21040;io.fresh(now,1,21030);io.ackLast(now,1,.3f);o.tick(io,r,now);
    check(o.paused&&io.commands.back().phase==4); // A new squeeze alone is no same-door proof.
    check(door_hold65::operation.read(operation));
    auto proof=door_hold65::Resume{r.serial,r.id,r.entity,io.hand};
    auto bad=proof;++bad.target;check(door_hold65::resume.publish(bad));
    now+=20;io.fresh(now,1,21030);io.ackLast(now,1,.3f);o.tick(io,r,now);check(o.paused);
    bad=proof;++bad.hand.poseEpoch;bad.hand.tick=bad.hand.poseTick=now;
    check(door_hold65::resume.publish(bad));now+=20;io.fresh(now,1,21030);io.ackLast(now);o.tick(io,r,now);check(o.paused);
    proof.hand=io.hand;check(door_hold65::resume.publish(proof));
    now+=20;io.fresh(now,1,21030);io.ackLast(now);o.tick(io,r,now);check(!o.paused&&io.commands.back().phase==3);
    now+=20;io.fresh(now,1,21030);io.ackLast(now,2,.9995f);o.tick(io,r,now);
    check(!o.active&&!o.pending);check(door_hold65::operation.read(operation)&&!operation.request.serial);
    const auto sent=io.commands.size();o.tick(io,r,now+20);check(io.commands.size()==sent);
    {HoldOwner65 t;auto in=input();in.hand.press=999;t.tick(in,r,1000);
        check(!t.active&&!t.pending&&in.commands.back().phase==0);}
    // True context loss still cleans up via phase0, rather than retaining stale control.
    for(int failure=0;failure<4;++failure){HoldOwner65 t;auto in=input();t.tick(in,r,1000);
        in.fresh(1020,1,990);in.ackLast(1020);t.tick(in,r,1020);check(t.active);
        in.fresh(1040,1,990);in.ackLast(1040);
        if(failure==0)in.allowed=false;if(failure==1)++in.hand.poseEpoch;
        if(failure==2)++in.hand.settingsEpoch;if(failure==3)in.hand.poseTick=900;
        t.tick(in,r,1040);check(!t.active&&in.commands.back().phase==0);
    }
    // Historical65 owner above; current66 channel accepts only the new motion protocol.
    door_channel19c4r4::Channel c;c.enabled(true);std::uint32_t v=0;
    check(c.field("kcd2vr_door_version",1000,v)&&v==1966);
    check(c.field("kcd2vr_door_companion_46",1000,v)&&v==0&&!c.companion46());
    check(c.field("kcd2vr_door_companion_66",1000,v)&&v==66&&c.companion46());
    check(!c.publish({2,1,42,3,.2f},1000,r.entity));
    check(c.publish({2,1,42,1,.2f},1000,r.entity)&&c.claim(2,1,1000));
    check(c.publish({4,1,42,2,.2f},1020,r.entity));
    check(c.acknowledge(4,1,1,200000,1020));
    check(c.publish({6,1,42,4,.3f},1040,r.entity)&&c.pending());
    check(c.acknowledge(6,1,1,300000,1040)&&c.pending());
    check(c.publish({8,1,42,2,.3f},1060,r.entity));
    check(c.acknowledge(8,1,2,999500,1060)&&!c.pending());
    check(c.acknowledge(8,1,2,999500,1061));
    check(!c.acknowledge(6,1,2,999500,1061));
    std::printf("native_door_hold65: %u checks PASS; actual hold/pause/resume owner and channel, native leaves doubled\n",checks);
}
