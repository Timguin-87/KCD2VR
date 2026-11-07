#include "../src/core/native_door_motion66.h"
#include <cassert>
#include <algorithm>
#include <cstdio>
#include <limits>
#include <string>
#include <vector>
using namespace kcd2vr;
using namespace kcd2vr::door_script19c4r4;
static unsigned checks66=0;static void ck66(bool b){++checks66;assert(b);}
struct IO66 {
    bool allowed=true,publication=true;int driveResult=1;float projection=.2f;
    door_hold65::Hand hand{};Ack answer{};unsigned drives=0,detaches=0,retires=0,rebinds=0;
    std::vector<Command> sent;std::vector<std::string> receipts;
    bool control65(const Request&,door_hold65::Hand& h){h=hand;return allowed;}
    bool publish(Command c){sent.push_back(c);return publication;}
    bool ack(Ack& a){a=answer;return answer.request!=0;}
    void receipt(const Request&,const char* why,float,bool){receipts.emplace_back(why);}
    void retire66(const Request& r){if(r.serial)++retires;}
    void detach66(const Request&){++detaches;}
    int drive66(const Request&,const door_hold65::Hand&,float,bool rebind,float& next){++drives;if(rebind)++rebinds;next=projection;return driveResult;}
    void fresh(std::uint64_t now,float grip=1,std::uint64_t press=990){hand.tick=hand.poseTick=now;hand.grip=grip;hand.press=press;}
    void ackLast(std::uint64_t now,float fraction=.2f,int state=1){assert(!sent.empty());const auto& c=sent.back();answer={c.sequence,c.request,state,fraction,now};}
};
static Request request66(){Request r{};r.serial=1;r.tick=1000;r.epoch=7;r.player=0x100;r.entity=0x4200;r.id=42;r.side=0;
    r.sourceGeneration=9;r.trackingEpoch46=8;r.inputSettingsEpoch46=10;r.inputExperimentEpoch65=11;r.inputPress65=990;return r;}
static IO66 input66(){IO66 i;i.hand.available=true;i.hand.side=0;i.hand.player=0x100;i.hand.poseEpoch=7;
    i.hand.trackingEpoch=8;i.hand.sourceGeneration=9;i.hand.settingsEpoch=10;i.hand.experimentEpoch=11;i.fresh(1000);return i;}
static void activate66(MotionOwner66& o,IO66& i,const Request& r){
    o.tick(i,r,1000);ck66(o.pending&&!o.active&&i.sent.back().phase==1&&i.drives==0);
    i.fresh(1010);i.ackLast(1010);o.tick(i,r,1010);
    ck66(o.active&&!o.pending&&!o.paused&&i.sent.back().phase==2&&i.sent.back().fraction==.2f);
}
int main(){
    const auto r=request66();MotionOwner66 o;auto i=input66();activate66(o,i,r);
    i.fresh(1020);i.ackLast(1020,.2f);i.projection=.6f;o.tick(i,r,1020);ck66(o.active&&o.wanted==.6f&&i.sent.back().phase==2);
    // Grip does not autoplay. A stationary controller keeps exactly the same
    // authored fraction, and reverse projection moves the same operation back.
    i.fresh(1030);i.ackLast(1030,.6f);o.tick(i,r,1030);ck66(o.wanted==.6f&&i.sent.back().fraction==.6f);
    i.fresh(1040);i.ackLast(1040,.6f);i.projection=.35f;o.tick(i,r,1040);ck66(o.wanted==.35f&&i.sent.back().phase==2);
    const auto drives=i.drives;i.fresh(1050,0,0);i.ackLast(1050,.35f);o.tick(i,r,1050);
    ck66(o.active&&o.paused&&i.sent.back().phase==4&&i.drives==drives&&i.detaches==1);
    door_hold65::Operation operation{};ck66(door_hold65::operation.read(operation)&&operation.paused&&operation.request.serial==r.serial);
    // A long ordinary release is a pause, without resurrecting native Use or a
    // fifteen-second finish timeout. Only fresh current source keeps it alive.
    for(unsigned n=1;n<=1000;++n){const auto now=1050+n*20;i.fresh(now,0,0);i.ackLast(now,.35f);o.tick(i,r,now);
        ck66(o.active&&o.paused&&i.sent.back().phase==4&&o.wanted==.35f&&i.drives==drives);}
    std::uint64_t now=21070;i.fresh(now,1,21060);i.ackLast(now,.35f);o.tick(i,r,now);
    ck66(o.paused&&i.sent.back().phase==4); // Grip alone cannot change the operation.
    auto resume=door_hold65::Resume{r.serial,r.id,r.entity,i.hand};auto wrong=resume;++wrong.target;
    ck66(door_hold65::resume.publish(wrong));now+=10;i.fresh(now,1,21060);i.ackLast(now,.35f);o.tick(i,r,now);ck66(o.paused);
    resume.hand=i.hand;ck66(door_hold65::resume.publish(resume));now+=10;i.fresh(now,1,21060);i.ackLast(now,.35f);o.tick(i,r,now);
    ck66(o.active&&!o.paused&&o.press==21060&&o.request.serial==r.serial&&i.sent.back().phase==2&&i.rebinds>=2);
    // A projected endpoint while Grip remains held is still owned and paused
    // in the authored clip. Only release requests original endpoint completion.
    now+=10;i.fresh(now,1,21060);i.ackLast(now,.35f);i.projection=.9995f;o.tick(i,r,now);
    ck66(o.active&&!o.finishing&&i.sent.back().phase==2);
    now+=10;i.fresh(now,0,0);i.ackLast(now,.9995f);o.tick(i,r,now);ck66(o.active&&o.finishing&&i.sent.back().phase==5);
    now+=10;i.fresh(now,0,0);i.ackLast(now,.9995f,2);o.tick(i,r,now);
    ck66(!o.active&&!o.pending&&i.retires==1&&door_hold65::operation.read(operation)&&!operation.request.serial);
    const auto count=i.sent.size();o.tick(i,r,now+10);ck66(i.sent.size()==count);
    // Source retirement, foreign/stale ACKs and invalid projection all use
    // the ordinary cleanup phase0 rather than continuing controller authority.
    for(int bad=0;bad<25;++bad){MotionOwner66 t;auto in=input66();activate66(t,in,r);in.fresh(1020);in.ackLast(1020);
        if(bad==0)in.allowed=false;if(bad==1)++in.hand.player;if(bad==2)++in.hand.poseEpoch;
        if(bad==3)++in.hand.trackingEpoch;if(bad==4)++in.hand.sourceGeneration;if(bad==5)++in.hand.settingsEpoch;
        if(bad==6)++in.hand.experimentEpoch;if(bad==7)in.hand.side=1;if(bad==8)in.hand.available=false;
        if(bad==9)in.hand.tick=900;if(bad==10)in.hand.poseTick=900;
        if(bad==11)in.hand.grip=std::numeric_limits<float>::quiet_NaN();
        if(bad==12)in.hand.point.x=std::numeric_limits<float>::quiet_NaN();
        if(bad==13)++in.answer.request;if(bad==14)in.answer.source=t.sequence+2;if(bad==15)in.answer.source=0;
        if(bad==16)in.answer.tick=900;if(bad==17)in.answer.fraction=std::numeric_limits<float>::quiet_NaN();
        if(bad==18)in.answer.fraction=1;if(bad==19)in.answer.state=3;
        if(bad==20)in.projection=std::numeric_limits<float>::quiet_NaN();if(bad==21)in.projection=-.1f;
        if(bad==22)in.projection=1.1f;if(bad==23)in.driveResult=-1;if(bad==24)in.publication=false;
        t.tick(in,r,1020);ck66(!t.active&&!t.pending&&in.sent.back().phase==0&&in.retires==1);
        ck66(door_hold65::operation.read(operation)&&!operation.request.serial);
    }
    // No source can be reused across context changes while waiting for the
    // first ACK. Binding has its own bounded timeout after native acceptance.
    for(int bad=0;bad<5;++bad){MotionOwner66 t;auto in=input66();t.tick(in,r,1000);ck66(t.pending);
        in.fresh(1010);if(bad==0)in.hand.press=999;if(bad==1)++in.hand.poseEpoch;
        if(bad==2)++in.hand.settingsEpoch;if(bad==3)in.allowed=false;
        const auto clock=bad==4?1251u:1010u;in.fresh(clock,1,bad==0?999:990);t.tick(in,r,clock);
        // A changed initial press after qualification is held pending until
        // ACK, but it must never get controller motion or phase2 permission.
        if(bad==0)ck66(t.pending&&in.sent.back().phase==1&&in.drives==0);
        else ck66(!t.pending&&!t.active&&in.sent.back().phase==0);
    }
    {MotionOwner66 t;auto in=input66();in.hand.press=999;t.tick(in,r,1000);ck66(!t.pending&&in.sent.back().phase==0);}
    {MotionOwner66 t;auto in=input66();in.driveResult=0;t.tick(in,r,1000);in.fresh(1010);in.ackLast(1010);t.tick(in,r,1010);
        ck66(t.active&&t.rebind&&in.sent.back().phase==1);in.fresh(1261);in.ackLast(1261);t.tick(in,r,1261);
        ck66(!t.active&&in.sent.back().phase==0&&in.retires==1);}
    // Release can precede the first successful resident-curve bind. Only the
    // original native START may cancel back through phase5 without a bind;
    // mid/high fractions remain paused and expire the250ms binding lease.
    for(const float initial:{0.f,.5f,.9995f}){MotionOwner66 t;auto in=input66();in.driveResult=0;
        t.tick(in,r,1000);ck66(t.pending&&!t.everBound&&in.sent.back().phase==1);
        in.fresh(1010,0,0);in.ackLast(1010,initial);t.tick(in,r,1010);
        const auto receipt=[&](const char* label){return std::find(in.receipts.begin(),in.receipts.end(),label)!=in.receipts.end();};
        ck66(t.active&&t.paused&&!t.everBound&&in.drives==0);
        if(initial==0){
            ck66(t.finishing&&in.sent.back().phase==5&&receipt("EARLY_RELEASE_NATIVE_START_CANCEL"));
            ck66(!receipt("RELEASE_AT_NATIVE_ENDPOINT"));
            in.fresh(1020,0,0);in.ackLast(1020,0,2);t.tick(in,r,1020);
            ck66(!t.active&&!t.pending&&in.retires==1);
        }else{
            ck66(!t.finishing&&in.sent.back().phase==4&&!receipt("EARLY_RELEASE_NATIVE_START_CANCEL"));
            ck66(std::none_of(in.sent.begin(),in.sent.end(),[](const Command& c){return c.phase==5;}));
            in.fresh(1261,0,0);in.ackLast(1261,initial);t.tick(in,r,1261);
            ck66(!t.active&&!t.pending&&in.sent.back().phase==0&&in.retires==1);
            ck66(receipt("RESIDENT_HANDLE_CURVE_UNAVAILABLE"));
        }
    }
    std::printf("native_door_motion66: %u actual owner claim/stop/reverse/release/resume/completion/refusal checks PASS; projection and native leaves doubled\n",checks66);
}
