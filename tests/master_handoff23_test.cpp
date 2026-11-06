// C23 guard->strike handoff inside the live native master-strike window.
// Executes the PRODUCTION physical kernel (physical_weapon.h -> melee_stroke118.h)
// and the production edge observer (native_master_window23.h). No native call,
// no WHGame image, no forced result: the native layer is represented only by the
// published window copy that the exact-owner guard pump would hand to the pose thread.
#include "vrik/physical_weapon.h"
#include "vrik/native_master_window23.h"
#include <cstdio>
#include <vector>
namespace ph=kcd2vr::physical;
namespace nm=kcd2vr::native_master23;
static int total=0,failed=0;
static void ck(bool b,const char* s){++total;failed+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",s);}
struct Rig {
    ph::Input in{};ph::State st{};std::array<ph::Profile,2> profiles{};ph::Output out{};
    std::vector<ph::Command> commands;
    Rig(bool onset=false){
        in.allowed=true;in.tracked[0]=in.tracked[1]=true;in.epoch=in.bodyEpoch=1;in.sample=1;in.tick=1000;
        in.shoulders[0]={.15,0,1.3};in.shoulders[1]={-.15,0,1.3};in.grip[0]=1;
        in.naturalCombat117=in.coherentMelee119=in.positionGuard176=in.attacks=true;
        in.dynamics=false;in.contact=false;in.controllerAnchor119=true;in.heldMotionAttacks=true;
        in.guardEnterMaxSpeed178r29=2.5;in.guardEnterDwellMs178r29=0;in.guardExitSpeed178r45=3.5;
        in.masterHandoff23=true;in.masterHandoffSpeed23=in.guardExitSpeed178r45; // production default: follow VrGuardExitSpeed
        if(onset){in.physicalCombat177=true;in.forwardOnly19=true;} // SYNC-style first-sample onset policy
        in.hands[0].p={-.3,.4,1.5};in.intentHands[0]=in.hands[0];in.intentTracked[0]=true;
        auto& p=profiles[0];p.key=77;p.generation=1;p.side=0;p.classId=4;p.primary=p.geometry=true;p.tip={.6,0,0};p.axis={1,0,0};
        for(int n=0;n<20;++n)tick({});
        commands.clear();
    }
    void window(std::uint64_t serial,std::uint64_t key=77){
        in.masterWindowSerial23=serial;in.masterWindowKey23=key;in.masterWindowGeneration23=1;
        in.masterWindowBody23=in.bodyEpoch;in.masterWindowRequest23=in.epoch;
    }
    void tick(ph::Vec3 d){
        ++in.sample;in.tick+=10;in.hands[0].p=in.hands[0].p+d;in.intentHands[0]=in.hands[0];
        out=ph::step(st,in,profiles,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.;});
        for(unsigned i=0;i<out.commandCount;++i)commands.push_back(out.commands[i]);
    }
    int prepares(std::uint64_t tagged=0) const {int n=0;for(const auto& c:commands)n+=c.kind==ph::CommandKind::Prepare&&(!tagged||c.masterHandoff23==tagged);return n;}
    int taggedAny() const {int n=0;for(const auto& c:commands)n+=c.kind==ph::CommandKind::Prepare&&c.masterHandoff23!=0;return n;}
    void settle(){in.hands[0].p={-.3,.4,1.5};in.intentHands[0]=in.hands[0];for(int n=0;n<60;++n)tick({});commands.clear();}
};
struct Strike {int firstGuardOff=-1,firstCandidate=-1,firstPrepare=-1,handoffTick=-1;std::uint64_t handoffSerial=0;};
static Strike strike(Rig& r,ph::Vec3 perTick,int ticks){
    Strike s{};
    for(int n=0;n<ticks;++n){
        const auto before=r.commands.size();r.tick(perTick);
        if(s.firstGuardOff<0&&!r.out.guard117)s.firstGuardOff=n;
        if(s.firstCandidate<0&&(r.out.melee118.reason==kcd2vr::melee118::Reason::Candidate||r.out.melee118.reason==kcd2vr::melee118::Reason::Prepared))s.firstCandidate=n;
        for(auto i=before;i<r.commands.size();++i)if(s.firstPrepare<0&&r.commands[i].kind==ph::CommandKind::Prepare)s.firstPrepare=n;
        if(r.out.masterHandoff23.serial&&r.out.masterHandoff23.tick==r.in.tick){s.handoffTick=n;s.handoffSerial=r.out.masterHandoff23.serial;}
    }
    std::printf("  strike v=(%.1f,%.1f,%.1f)m/s guardOff@%d candidate@%d prepare@%d handoff@%d serial=%llu prepares=%d tagged=%d\n",
        perTick.x*100,perTick.y*100,perTick.z*100,s.firstGuardOff,s.firstCandidate,s.firstPrepare,s.handoffTick,(unsigned long long)s.handoffSerial,r.prepares(),r.taggedAny());
    return s;
}
int main(){
    const ph::Vec3 fwd25{0,.025,0},fwd40{0,.040,0},lateral{.045,.010,0};
    // 1. The held-guard gate before the recognizer (unchanged without a window).
    {Rig r;ck(r.out.guard117,"fixture settles in the positional guard");
     auto s=strike(r,fwd25,8);
     ck(s.firstGuardOff<0&&r.prepares()==0&&r.out.melee118.reason==kcd2vr::melee118::Reason::Guard,
        "GATE: 2.5 m/s forward strike from a held guard never reaches the recognizer (sample.guard wipes it; no PREPARE)");}
    {Rig r;auto s=strike(r,fwd40,8);
     ck(s.firstGuardOff==0&&s.firstCandidate>=1&&r.taggedAny()==0,
        "GATE: 4 m/s leaves guard on the exit tick, but that exit sample is still wiped (candidate starts one tick later)");}
    // Inert equivalence: enabled handoff without a window is bit-for-bit the old command stream.
    for(ph::Vec3 v:{fwd25,fwd40,lateral}){
        Rig a,b;b.in.masterHandoff23=false;strike(a,v,8);strike(b,v,8);
        bool same=a.commands.size()==b.commands.size()&&a.out.guard117==b.out.guard117;
        for(std::size_t i=0;same&&i<a.commands.size();++i)same=a.commands[i].kind==b.commands[i].kind&&a.commands[i].sample==b.commands[i].sample&&a.commands[i].zone117==b.commands[i].zone117;
        ck(same,"UNCHANGED: no published window = identical guard/command stream with the feature on or off");}
    // 2. Handoff inside the window.
    {Rig r;r.window(5);auto s=strike(r,fwd40,8);
     ck(s.handoffTick==0&&s.handoffSerial==5&&s.firstGuardOff==0&&s.firstCandidate==0,
        "WINDOW (default threshold = exit speed): the exit sample itself reaches the same recognizer (candidate on the handoff tick)");
     ck(r.prepares()==1&&r.prepares(5)==1,"WINDOW: exactly one PREPARE, carrying the window serial for the native live recheck");}
    {Rig r;r.window(5);auto s=strike(r,fwd25,8);
     ck(s.handoffTick<0&&r.prepares()==0&&s.firstGuardOff<0,"WINDOW: 2.5 m/s stays guarded while VrMasterHandoffForwardSpeed follows the 3.5 m/s exit");}
    {Rig r;r.in.masterHandoffSpeed23=2.0;r.window(5);auto s=strike(r,fwd25,8);
     ck(s.handoffTick==0&&s.firstGuardOff==0&&s.firstCandidate==0&&r.prepares(5)==1&&r.prepares()==1,
        "WINDOW (VrMasterHandoffForwardSpeed=2.0): a 2.5 m/s forward cut ends the guard and prepares through the same recognizer");}
    {Rig r(true);r.window(9);auto s=strike(r,fwd40,6);
     ck(s.handoffTick==0&&s.firstPrepare>0&&s.firstPrepare<=4&&r.prepares(9)==1&&r.prepares()==1,
        "C42 native window handoff preserves one tagged Prepare after real cut travel, not the first wrist sample");
     Rig n(true);auto t=strike(n,fwd40,6);
     ck(t.firstPrepare>=1&&n.taggedAny()==0,"control: same onset policy without a window prepares only after the wiped exit tick");}
    // C32 postguard recovery filtering must not be bypassed by a mere serial.
    // Only the existing fully qualified handoff retains first-sample onset.
    for(unsigned bad=0;bad<8;++bad){Rig r(true);r.window(9);
     if(bad==0)r.in.masterWindowKey23++;
     if(bad==1)r.in.masterWindowGeneration23++;
     if(bad==2)r.in.masterWindowBody23++;
     if(bad==3)r.in.masterWindowRequest23++;
     if(bad==4)r.in.masterWindowSerial23=0;
     if(bad==5)r.st.weapon[0].masterHandoffSerial23=9;
     if(bad==6)r.in.masterHandoff23=false;
     auto s=strike(r,bad==7?lateral:fwd40,6);
     ck(s.handoffTick<0&&r.taggedAny()==0&&s.firstPrepare!=0,
        "C32 stale/consumed/disabled/off-cone window cannot bypass ordinary postguard onset evidence");}
    {Rig r(true);r.profiles[0].axis={.5,.866025403784,0};r.profiles[0].tip=r.profiles[0].axis*.6;r.settle();r.window(19);
     r.tick({-.015,.04,0});
     ck(r.prepares(19)==0&&r.out.masterHandoff23.serial==19,
        "C42 short native-window adjustment has no premature cut offer");
     for(int i=0;i<8;++i)r.tick(r.profiles[0].axis*.025);
     unsigned refined=0;for(const auto& c:r.commands)refined+=c.kind==ph::CommandKind::Zone57&&c.stroke117==ph::CommandKind::Thrust;
     ck(refined==0&&r.prepares()==1&&r.prepares(19)==1,
        "C42 unannounced adjustment refines into one tagged stab, without a phantom cut offer");}
    // Lateral/upward parry motion inside the window keeps the guard (cone + threshold).
    {Rig r;r.in.masterHandoffSpeed23=0.5;r.window(5);auto s=strike(r,lateral,6);
     ck(s.handoffTick<0&&r.taggedAny()==0,"WINDOW: a fast lateral parry beat (4.5 m/s, 1 m/s forward) is not an enemy-directed strike");}
    {Rig r(true);r.st.weapon[0].guard117=false;r.st.weapon[0].guardEntry30c1={};
     r.in.guardEnterDwellMs178r29=1000;r.window(31);auto s=strike(r,fwd40,6);
     ck(s.handoffTick==0&&r.prepares(31)==1,
        "C54 a fresh unguarded forward sample may claim the live counter window before auto-block closes it");}
    // 3. No handoff after window close.
    {Rig r;r.in.masterHandoffSpeed23=2.0;r.window(5);r.tick({});r.window(0);auto s=strike(r,fwd25,8);
     ck(s.handoffTick<0&&r.prepares()==0&&s.firstGuardOff<0,"CLOSED: the publication withdrawn (serial 0) restores the unchanged held-guard gate");}
    // 4. One use per window serial.
    {Rig r;r.in.masterHandoffSpeed23=2.0;r.window(5);auto s=strike(r,fwd25,10);
     ck(s.handoffSerial==5,"ONE USE: first strike consumes serial 5");
     r.settle();ck(r.out.guard117,"ONE USE: guard re-entered after the stroke");
     auto again=strike(r,fwd25,8);
     ck(again.handoffTick<0&&r.prepares()==0,"ONE USE: the same serial cannot release the guard a second time");
     r.settle();r.window(6);auto next=strike(r,fwd25,8);
     ck(next.handoffSerial==6&&r.prepares(6)==1,"ONE USE: a new native OPEN edge (serial 6) may hand off once");}
    // Weapon/body/request identity copied at OPEN must still match.
    {Rig r;r.in.masterHandoffSpeed23=2.0;r.window(5,78);auto s=strike(r,fwd25,8);
     ck(s.handoffTick<0&&r.prepares()==0,"IDENTITY: a window opened under another item/generation cannot release this guard");}
    {Rig r;r.in.masterHandoffSpeed23=2.0;r.window(5);r.in.masterWindowBody23=r.in.bodyEpoch+1;auto s=strike(r,fwd25,8);
     ck(s.handoffTick<0,"IDENTITY: a window from another body epoch (recenter) cannot release this guard");}
    // 5. Edge-only observer (production native_master_window23.h): opponent change cancels, failed reads never mint serials.
    {nm::Window w;nm::Identity a{};a.player=1;a.actor=2;a.state=3;a.opponent=10;a.opponentState=11;a.opponentId=100;a.key=77;a.generation=1;a.body=1;a.request=1;
     auto b=a;b.opponent=20;b.opponentState=21;b.opponentId=200;
     ck(w.observe(a,true,1,1000)==nm::Edge::None&&!w.available(a),"OBSERVER: a positive count first seen mid-window is not adopted");
     ck(w.observe(a,true,0,1010)==nm::Edge::None&&w.observe(a,true,1,1020)==nm::Edge::Open&&w.serial==1&&w.available(a)&&w.openTick==1020,
        "OBSERVER: known zero then positive = OPEN, serial 1, open tick copied");
     ck(w.observe(a,false,-1,1030)==nm::Edge::Unknown&&w.serial==1&&!w.available(a)&&w.open,"OBSERVER: failed read = UNKNOWN (not CLOSE), no serial, not available");
     ck(w.observe(a,false,-1,1040)==nm::Edge::None,"OBSERVER: repeated failed reads emit one UNKNOWN edge");
     ck(w.observe(a,true,2,1050)==nm::Edge::Resumed&&w.serial==1&&w.available(a),"OBSERVER: the same window resumes with the SAME serial after an unreadable sample");
     ck(w.observe(a,true,99,1055)==nm::Edge::Unknown&&w.serial==1,"OBSERVER: an out-of-bound count is unknown, never a new serial");
     (void)w.observe(a,true,1,1056);
     auto aItem=a;aItem.key=99;
     ck(w.observe(aItem,true,1,1058)==nm::Edge::None&&w.id.key==77&&w.available(aItem),"OBSERVER: item fields are copied at OPEN, not rewritten mid-window (consumer compares them)");
     ck(w.observe(b,true,1,1060)==nm::Edge::OwnerChanged&&!w.available(b)&&!w.available(a)&&w.serial==1,"OPPONENT CHANGE: retires the window; no serial for the new opponent's positive count");
     ck(w.observe(b,true,1,1070)==nm::Edge::None&&!w.available(b),"OPPONENT CHANGE: the new opponent needs its own native 0 -> >0 edge");
     ck(w.observe(b,true,0,1080)==nm::Edge::None&&w.observe(b,true,1,1090)==nm::Edge::Open&&w.serial==2,"OPPONENT CHANGE: a genuine new opening gets serial 2");
     ck(w.observe(b,true,0,1100)==nm::Edge::Close&&!w.available(b)&&w.serial==2,"OBSERVER: count 0 = CLOSE, serial retained for one-use bookkeeping");
     auto c=b;c.opponentId=0;ck(w.observe(c,true,1,1110)==nm::Edge::Unknown,"OBSERVER: unresolved opponent identity is UNKNOWN");}
    // Opponent change through the pose-thread consumer: the pump stops publishing the old serial.
    {Rig r;r.in.masterHandoffSpeed23=2.0;r.window(5);r.tick({});r.window(0);r.tick({});auto s=strike(r,fwd25,8);
     ck(s.handoffTick<0&&r.prepares()==0,"OPPONENT CHANGE: withdrawn publication cancels the handoff before any motion");}
    std::printf("master_handoff23=%d/%d native_calls=NONE forced_results=NONE\n",total-failed,total);return failed?1:0;
}
