// R8d: F2 rung 1 holster attempt observer. Replays the H50 10:33:48 polearm sample (gap .248, Grip 1, latched 0,
// pending 0, eligible 1 -> never drew) through the REAL equipment114 FSM, and a bow drawn from the chest.
#include "vrik/physical_weapon.h"
#include "vrik/equipment_gesture114.h"
#include "vrik/holster_diag_r8d.h"
#include "vrik/visible_failure.h"
#include <cstdio>
#include <cstring>
#include <cmath>
namespace p=kcd2vr::physical;namespace eq=kcd2vr::equipment114;namespace m=kcd2vr::pose_core;namespace hd=kcd2vr::holster_diag_r8d;
static unsigned tests=0,failures=0;
static void check(bool b,const char* text){++tests;failures+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",text);}
struct Rig {
    p::Input in{};p::Profile prof{};eq::State st{};hd::Track track{};std::uint64_t key=0x0200000000023C27ull;
    bool held=false,handKey=false;int hand=0;
    hd::Attempt last{};int ended=0;
    double rad() const {return .28+(p::over_shoulder_zone_hs2(p::holster_zone126(prof))||p::holster_zone126(prof)==p::HolsterZone126::LeftShoulder?.04:0.);} // .24 + HolsterZoneRadius shoulder bonus as the adapter
    // one producer update: real FSM + the observer frame the adapter would build.
    void frame(std::uint64_t tick,m::Vec3 h,double grip,unsigned char request=0){
        in.hands[hand].p=h;in.grip[hand]=grip;
        p::Vec3 mount{};bool known=p::shoulder_holster_closest131(in,prof,h,mount)||p::holster_mount126(in,prof,mount);
        const bool vol=p::draw_volume_ok_hs2(in,prof,h);
        eq::Input ei{};ei.key=key;ei.generation=1;ei.epoch=1;ei.tick=tick;ei.hand=h;ei.grip=grip;ei.drawn=held;
        ei.eligible=st.latched?true:vol;ei.mount=mount;ei.mountKnown=known;ei.radius=rad();ei.releaseAnywhere=true;ei.releaseDebounceMs=0;
        const auto r=eq::update(st,ei);
        hd::HandFrame f{};f.grip=grip;f.count=1;auto& x=f.items[0];
        x.key=key;x.classId=prof.classId;x.zone=unsigned(p::holster_zone126(prof));x.hand=hand;x.held=held;x.handKeyMatch=handKey;
        x.mountKnown=known;x.gap=known?m::length(h-mount):1.0e9;x.radius=rad();x.volumeOk=vol;
        x.neutral=st.neutral;x.latched=st.latched;x.pending=st.pending;x.blocked=st.blocked;x.wantDrawn=st.wantDrawn;
        x.latchedMs=st.latched?tick-st.started:0;
        x.request=request?request:(r==eq::Request::Draw?1:r==eq::Request::Holster?2:0);
        hd::Attempt a{};if(hd::step(track,f,hand,tick,a)){last=a;++ended;}
    }
};
static void bodyAt(p::Input& in){
    in.shoulders[0]=m::Vec3{.20,-.09,1.35};in.shoulders[1]=m::Vec3{-.20,-.09,1.35};
    in.allowed=true;in.tracked[0]=in.tracked[1]=true;
}
int main(){
    char line[512]{},vis[kcd2vr::visible_failure::kMaxText+1]{};
    // 1) 10:33:48 polearm: right hand arrives at the right shoulder with Grip ALREADY closed. The FSM never
    // reaches neutral -> no latch -> no draw. Release later -> REFUSED GRIP_CLOSED_AT_ENTRY at FSM_LATCH.
    {Rig r{};bodyAt(r.in);r.prof.classId=7;r.hand=0;
     const auto a=p::body_axes(r.in);const auto far0=r.in.shoulders[0]-a.up*.45+a.forward*.30;
     p::Vec3 zone{};p::holster_mount126(r.in,r.prof,zone);
     std::uint64_t t=1000;for(int i=0;i<10;++i,t+=11)r.frame(t,far0,1.0);              // closed fist, far away
     check(!r.track.active,"polearm: closed hand outside the zone starts nothing");
     for(int i=0;i<30;++i,t+=11)r.frame(t,zone+a.up*.02,1.0);                         // inside, Grip still 1
     check(r.track.active&&r.track.a.key==r.key,"polearm: entering the zone with Grip closed starts an attempt");
     check(!r.st.latched&&!r.st.pending&&!r.st.neutral,"polearm: real FSM latched=0 pending=0 (H50 10:33:48)");
     for(int i=0;i<5;++i,t+=11)r.frame(t,zone+a.up*.02,0.0);                          // opens the hand
     check(r.ended==1,"polearm: Grip release ends the attempt");
     check(r.last.result==hd::Result::Refused,"polearm: result REFUSED");
     check(r.last.why==hd::Refuse::GripClosedAtEntry,"polearm: refuse GRIP_CLOSED_AT_ENTRY");
     check(r.last.firstMissing==hd::Stage::FsmLatch,"polearm: firstMissing FSM_LATCH");
     hd::format_attempt(r.last,line,sizeof(line));std::printf("  %s\n",line);
     check(std::strstr(line,"class=polearm")&&std::strstr(line,"arbitration=WON")&&std::strstr(line,"fsm=closed")&&std::strstr(line,"firstMissing=FSM_LATCH"),"polearm: attempt line fields");
     hd::visible_text(r.last,vis,sizeof(vis));std::printf("  visible: %s\n",vis);
     check(!std::strcmp(vis,"polearm: open the hand at the shoulder, then squeeze"),"polearm: visible line");
     check(kcd2vr::visible_failure::safe(vis),"polearm: visible line passes the publish filter");
     // now the right way: open hand in the zone, squeeze, pull out -> DRAW request -> native ACK -> DRAWN
     for(int i=0;i<5;++i,t+=11)r.frame(t,zone+a.up*.02,1.0);
     check(r.st.latched&&r.track.active,"polearm: open-then-squeeze latches and starts an attempt");
     m::Vec3 h=zone+a.up*.02;bool requested=false;
     for(int i=0;i<30&&!requested;++i,t+=11){h=h+a.up*.015+a.right*.004;r.frame(t,h,1.0);requested=r.track.a.request==1;}
     check(requested,"polearm: pull issues the DRAW request");
     t+=11;r.frame(t,h,1.0);check(r.track.active&&r.track.best==hd::Stage::NativeAck,"polearm: waits at NATIVE_ACK");
     r.held=true;r.handKey=true;t+=140;r.frame(t,h,1.0);
     check(r.ended==2&&r.last.result==hd::Result::Drawn&&r.last.acked&&r.last.ackMs>=140,"polearm: native ACK -> DRAWN with ackMs");
     hd::format_attempt(r.last,line,sizeof(line));std::printf("  %s\n",line);
     check(std::strstr(line,"result=DRAWN")&&std::strstr(line,"firstMissing=none"),"polearm: drawn line");
     // drawn polearm swung through its own zone for 2.5 s: the FSM latches (sticky stow) but no attempt = no spam
     const int drawnEnds=r.ended;
     for(int i=0;i<230;++i,t+=11)r.frame(t,zone+a.up*.02,1.0);
     check(r.st.latched&&!r.track.active&&r.ended==drawnEnds,"held polearm latched in its zone: no attempt while Grip stays closed");
     t+=11;r.frame(t,zone+a.up*.02,0.0);
     check(r.track.active&&r.track.a.request==2,"release issues HOLSTER and starts a stow attempt");
     r.held=false;r.handKey=false;t+=90;r.frame(t,zone+a.up*.02,0.0);
     check(r.ended==drawnEnds+1&&r.last.result==hd::Result::Stowed&&r.last.ackMs>=90,"native ACK -> STOWED");
    }
    // 2) bow on the chest: left hand squeezes at the sternum, inside the bow sphere but under the shoulder.
    {Rig r{};bodyAt(r.in);r.prof.classId=9;r.hand=1;
     const auto a=p::body_axes(r.in);p::Vec3 bowZone{};p::holster_mount126(r.in,r.prof,bowZone);
     const auto chest=(r.in.shoulders[0]+r.in.shoulders[1])*.5-a.up*.10+a.forward*.12;
     const auto close=r.in.shoulders[1]-a.up*.06-a.forward*.02; // under the shoulder, inside the bow sphere
     std::uint64_t t=5000;
     for(int i=0;i<5;++i,t+=11)r.frame(t,close,0.0);
     for(int i=0;i<20;++i,t+=11)r.frame(t,close,1.0);
     check(r.track.active,"bow: Grip rise near the bow zone starts an attempt");
     for(int i=0;i<3;++i,t+=11)r.frame(t,close,0.0);
     check(r.ended==1&&r.last.result==hd::Result::Refused,"bow: REFUSED");
     check(r.last.why==hd::Refuse::OutsideVolume&&r.last.firstMissing==hd::Stage::Volume,"bow: OUTSIDE_VOLUME at VOLUME");
     hd::visible_text(r.last,vis,sizeof(vis));std::printf("  visible: %s\n",vis);
     check(!std::strcmp(vis,"bow: hand not above the left shoulder"),"bow: visible line");
     // a squeeze on the sternum (outside the near band) never starts an attempt: no spam while fighting.
     const int before=r.ended;for(int i=0;i<3;++i,t+=11)r.frame(t,chest-a.forward*.0,0.0);
     for(int i=0;i<10;++i,t+=11)r.frame(t,chest+a.forward*.25,1.0);
     check(r.ended==before&&!r.track.active,"bow: squeeze far from any zone starts nothing");
    }
    // 3) gates and claims map to reasons; native ACK timeout; summary.
    {hd::HandFrame f{};f.grip=1;f.count=1;auto& x=f.items[0];x.key=7;x.classId=7;x.zone=1;x.mountKnown=true;x.gap=.10;x.radius=.28;x.volumeOk=true;x.neutral=true;
     hd::Refuse w{};
     x.arbitrationWon=false;x.lostToClass=9;check(hd::classify(x,f,w)==hd::Stage::Arbitration&&w==hd::Refuse::LostArbitration,"arbitration loss");
     x.arbitrationWon=true;f.claim=hd::Refuse::QuickTalk;check(hd::classify(x,f,w)==hd::Stage::Eligible&&w==hd::Refuse::QuickTalk,"QuickTalk claim");
     f.claim=hd::Refuse::None;x.gate=hd::Refuse::OtherLoadoutHeld;check(hd::classify(x,f,w)==hd::Stage::Eligible&&w==hd::Refuse::OtherLoadoutHeld,"PhysDrawCandidate126 gate");
     x.gate=hd::Refuse::None;x.blocked=true;check(hd::classify(x,f,w)==hd::Stage::FsmLatch&&w==hd::Refuse::BlockedAfterTimeout,"blocked after pending timeout");
     x.blocked=false;x.latched=true;check(hd::classify(x,f,w)==hd::Stage::Pull&&w==hd::Refuse::NoPull,"latched, no pull");
     x.latched=false;f.switchState=1;check(hd::classify(x,f,w)==hd::Stage::Request&&w==hd::Refuse::SwitchWait,"HS2 switch wait");
     f.switchState=0;
     // request without ACK for 2 s -> TIMEOUT at NATIVE_ACK
     hd::Track t{};hd::Attempt a{};x.request=1;x.latched=true;std::uint64_t now=100;int n=0;
     for(int i=0;i<260;++i,now+=11){if(hd::step(t,f,0,now,a)){++n;break;}x.request=0;x.pending=true;}
     check(n==1&&a.result==hd::Result::Timeout&&a.firstMissing==hd::Stage::NativeAck&&a.why==hd::Refuse::NoAck,"no native ACK in 2 s -> TIMEOUT NATIVE_ACK");
     hd::visible_text(a,vis,sizeof(vis));std::printf("  visible: %s\n",vis);
     check(std::strstr(vis,"polearm: the game didn't attach it (no ACK in ")!=nullptr,"no-ACK visible line");
     hd::Summary s{};s.add(a);hd::Attempt b=a;b.result=hd::Result::Refused;b.why=hd::Refuse::GripClosedAtEntry;s.add(b);
     b.result=hd::Result::Drawn;b.why=hd::Refuse::None;s.add(b);
     char sum[1024]{};hd::format_summary(s,sum,sizeof(sum));std::printf("  %s\n",sum);
     check(std::strstr(sum,"attempts=3")&&std::strstr(sum,"polearm#7 a=3 drawn=1 stowed=0 refused=1 timeout=1")&&std::strstr(sum,"GRIP_CLOSED_AT_ENTRY:1")&&std::strstr(sum,"NO_NATIVE_ACK:1"),"summary counts by reason");
     hd::Diag d{};check(hd::visible_due(d,1000)&&!hd::visible_due(d,2500)&&hd::visible_due(d,3001),"visible line limited to one per 2 s");
    }
    // 4) every visible text fits the publish filter (ASCII, no quotes, <= 96).
    {bool ok=true;for(unsigned k=0;k<unsigned(hd::Refuse::Count);++k){hd::Attempt a{};a.classId=15;a.zone=5;a.gap=.371;a.why=hd::Refuse(k);a.request=1;a.end=600;a.lostToClass=8;a.heldClass=13;
        a.result=hd::Result::Refused;hd::visible_text(a,vis,sizeof(vis));if(!kcd2vr::visible_failure::safe(vis)){ok=false;std::printf("  bad: %s\n",vis);}}
     check(ok,"all visible reasons pass the visible_failure filter");}
    std::printf("holster_diag_r8d %u/%u\n",tests-failures,tests);return failures?1:0;
}
