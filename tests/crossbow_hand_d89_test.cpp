// R89 xbow host test: the real policy + native sequences of crossbow_hand_d89.h
// against controlled native leaves that behave like the disassembled retail
// functions (2895F74/1741C80, 2ACD5D4, 28959D8, 40B730, 289C988/2A8A0C0,
// 393524, C5F484). Build: g++ -std=c++20 -I src tests/crossbow_hand_d89_test.cpp
#include "subsystems/ranged_reload/crossbow_hand_d89.h"
#include "subsystems/ranged_reload/crossbow_refusal_d85.h"
#include <cstdio>
#include <string>
#include <vector>
namespace x=kcd2vr::crossbow_hand_d89;
static int g_fail=0,g_pass=0;
#define CHECK(c) do{if(c)++g_pass;else{++g_fail;std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);}}while(0)
// Fake game: only the state the leaves touch.
struct Game {
    unsigned char cocked=0;std::uint64_t nocked=0;const std::uint64_t empty=0,ammoKey=0x0200000000019613ull;
    std::uint32_t hand=0,nextId=2363388;int createSide=0;bool bindWorks=true,tagCocked=false,tagLoaded=false;
    int reserved=0;std::vector<std::string> log;
};
struct Leaves {
    Game& g;
    bool cocked() const {return g.cocked==1;}
    bool loaded() const {return g.nocked==g.ammoKey;}
    // 2895F74 -> 1741C80(component,1): stores D8 only when it differs
    bool cock(){g.log.push_back("2895F74");if(g.cocked!=1)g.cocked=1;return true;}
    // 2ACD5D4(eq,2) = 393524(eq+28,2); C5F484(eq)
    bool cockNotify(){g.log.push_back("2ACD5D4:2");g.tagCocked=g.cocked;g.tagLoaded=g.nocked!=g.empty;return true;}
    // 28959D8: *out must be 0, else returns 2 and creates nothing
    bool create(int& side){g.log.push_back("28959D8");if(g.hand){side=2;return true;}g.hand=g.nextId;side=g.createSide;return true;}
    bool remove(){g.log.push_back("40B730");g.hand=0;return true;}
    // 289C988: binds only when component+E0 is empty (2A8A0C0 -> 2A8A030)
    bool bind(){g.log.push_back("289C988");if(g.bindWorks&&g.nocked==g.empty){g.nocked=g.ammoKey;++g.reserved;}return true;}
    bool notify(int v){g.log.push_back("393524:"+std::to_string(v));return true;}
    bool refresh(){g.log.push_back("C5F484");g.tagCocked=g.cocked;g.tagLoaded=g.nocked!=g.empty;return true;}
    std::uint32_t handId() const {return g.hand;}
};
// Native event-handler order read from the retail disassembly and seen in
// every NATIVE load row (HAND_BEGIN/END, TRANSFER, AMMO_BIND, NOCK) of the logs.
static const std::vector<std::string> kCock{"2895F74","2ACD5D4:2"};
static const std::vector<std::string> kAmmo{"28959D8","393524:0","C5F484"};
static const std::vector<std::string> kLoad{"40B730","289C988","393524:1","C5F484"};
struct Sim {
    Game g;x::Machine m;x::Input in{};std::vector<x::Note> notes;
    Sim(int cls=10){in.context=true;in.leftHand=true;in.classId=cls;in.cocked=0;in.bolts=true;in.handFree=true;in.geometry=true;
        in.rest={0,cls==10?.231734782457:cls==14?.269499987364:.312299996614,.059749998152};in.rail={0,.137999981642,.059619996697};
        in.hand={.3,0,.3};in.grip=0;}
    x::Step frame(){
        in.cocked=g.cocked;in.loaded=g.nocked==g.ammoKey;in.boltInHand=g.hand!=0;
        auto s=m.update(in);Leaves l{g};bool ok=true;
        switch(s.command){
        case x::Command::Cock:ok=x::cockSequence(l);break;
        case x::Command::TakeBolt:ok=x::takeBoltSequence(l);break;
        case x::Command::DropBolt:ok=x::dropBoltSequence(l);break;
        case x::Command::Nock:ok=x::nockSequence(l);break;
        default:break;}
        if(s.command!=x::Command::None)m.commandResult(s.command,ok);
        if(s.note!=x::Note::None)notes.push_back(s.note);
        return s;
    }
    void release(){in.grip=0;frame();}
    void grab(){in.grip=.9;frame();}
    bool noted(x::Note n) const {for(auto v:notes)if(v==n)return true;return false;}
    // pull from current hand by d metres along dir in steps of 1 cm
    void move(x::Vec dir,double d){double l=x::length(dir);for(double s=.01;s<=d+1e-9;s+=.01){in.hand=in.hand+dir*(.01/l);frame();}}
};
static void lightCycle(){
    Sim s;s.release();
    s.in.hand=s.in.rest+x::Vec{0,0,.03};s.grab();CHECK(s.m.stage()==x::Stage::Span);CHECK(s.g.log.empty());
    s.move({0,-1,0},.05);CHECK(s.g.cocked==0); // half pull: nothing yet
    s.move({0,-1,0},.04);CHECK(s.g.cocked==1);CHECK(s.g.log==kCock);CHECK(s.g.tagCocked);
    s.release();s.g.log.clear();
    // quiver grip: native bolt into the RIGHT hand, then the ammo-event notify
    s.in.inQuiver=true;s.in.hand={.2,.3,-.2};s.grab();CHECK(s.g.log==kAmmo);CHECK(s.g.hand==2363388);CHECK(s.m.stage()==x::Stage::Bolt);
    s.in.inQuiver=false;s.in.hand=(s.in.rest+s.in.rail)*.5+x::Vec{.02,0,.04};s.frame();CHECK(s.g.nocked==0);
    s.g.log.clear();s.release();CHECK(s.g.log==kLoad);CHECK(s.g.nocked==s.g.ammoKey);CHECK(s.g.hand==0);CHECK(s.g.tagLoaded);CHECK(s.g.reserved==1);
    s.in.inQuiver=true;s.g.log.clear();s.grab();CHECK(s.g.log.empty());CHECK(s.noted(x::Note::AlreadyLoaded));
}
static void pushIsNotPull(){ // Codex D83 finding: abs(dot) cocked on a push
    Sim s;s.release();s.in.hand=s.in.rest;s.grab();s.move({0,1,0},.09);CHECK(s.g.cocked==0);CHECK(s.g.log.empty());
}
static void releaseHalfway(){
    Sim s;s.release();s.in.hand=s.in.rest;s.grab();s.move({0,-1,0},.05);s.release();
    CHECK(s.g.cocked==0);CHECK(s.noted(x::Note::PullFurther));
    s.in.hand=s.in.rest;s.grab();s.move({0,-1,0},.09);CHECK(s.g.cocked==1);
}
static void slip(){
    Sim s;s.release();s.in.hand=s.in.rest;s.grab();s.move({1,0,0},.16);CHECK(s.noted(x::Note::StringSlipped));
    s.move({0,-1,0},.09);CHECK(s.g.cocked==0);
}
static void noGrabFarFromString(){
    Sim s;s.release();s.in.hand=s.in.rest+x::Vec{0,0,.15};s.grab();CHECK(s.m.stage()==x::Stage::Idle);
    s.move({0,-1,0},.09);CHECK(s.g.cocked==0);
}
static void heldGripNeedsNeutral(){
    Sim s;s.in.grip=.9;s.in.hand=s.in.rest;s.frame();CHECK(s.m.stage()==x::Stage::Idle);
    s.move({0,-1,0},.09);CHECK(s.g.cocked==0);
}
static void quiverBeforeCock(){
    Sim s;s.release();s.in.inQuiver=true;s.grab();CHECK(s.g.log.empty());CHECK(s.noted(x::Note::CockFirst));
}
static void dropAwayFromRail(){
    Sim s;s.g.cocked=1;s.release();s.in.inQuiver=true;s.grab();CHECK(s.g.hand!=0);
    s.in.inQuiver=false;s.in.hand={.3,.2,.4};s.g.log.clear();s.release();
    CHECK(s.g.log==std::vector<std::string>{"40B730"});CHECK(s.g.hand==0);CHECK(s.g.nocked==0);CHECK(s.noted(x::Note::BoltDropped));
    s.in.inQuiver=true;s.g.log.clear();s.grab();CHECK(s.g.log==kAmmo); // a new one can be taken
}
static void wrongHandRefused(){
    Sim s;s.g.cocked=1;s.g.createSide=1;s.release();s.in.inQuiver=true;s.grab();
    CHECK((s.g.log==std::vector<std::string>{"28959D8","40B730"}));CHECK(s.g.hand==0);CHECK(s.m.stage()==x::Stage::Idle);
}
static void bindRefused(){
    Sim s;s.g.cocked=1;s.g.bindWorks=false;s.release();s.in.inQuiver=true;s.grab();s.in.inQuiver=false;
    s.in.hand=s.in.rail;s.g.log.clear();s.release();
    CHECK((s.g.log==std::vector<std::string>{"40B730","289C988"}));CHECK(s.g.nocked==0); // no loaded notify without a bind
}
static void handBusyAndNoBolts(){
    Sim s;s.g.cocked=1;s.in.handFree=false;s.release();s.in.inQuiver=true;s.grab();CHECK(s.g.log.empty());CHECK(s.noted(x::Note::HandBusy));
    Sim t;t.g.cocked=1;t.in.bolts=false;t.release();t.in.inQuiver=true;t.grab();CHECK(t.g.log.empty());CHECK(t.noted(x::Note::NoBolts));
}
static void contextLostDropsBolt(){
    Sim s;s.g.cocked=1;s.release();s.in.inQuiver=true;s.grab();CHECK(s.g.hand!=0);
    s.in.context=false;s.g.log.clear();s.frame();CHECK(s.g.log==std::vector<std::string>{"40B730"});CHECK(s.g.hand==0);
}
static void goatsFoot(){
    Sim s(14);s.release();s.in.inQuiver=true;s.grab();CHECK(s.noted(x::Note::ToolFirst));s.in.inQuiver=false;s.release();
    s.in.inChest=true;s.in.hand={0,-.3,.2};s.grab();CHECK(s.m.stage()==x::Stage::Tool);s.in.inChest=false;
    s.in.hand=s.in.rest;s.frame();CHECK(s.m.stage()==x::Stage::Span);
    s.move({0,-1,0},.10);CHECK(s.g.cocked==0); // medium travel is 13.15 cm
    s.move({0,-1,0},.03);CHECK(s.g.cocked==1);CHECK(s.g.log==kCock);
    Sim t(14);t.release();t.in.inChest=true;t.grab();t.in.inChest=false;t.release();CHECK(t.m.stage()==x::Stage::Idle); // tool put away
}
static void cranequin(){
    Sim s(15);s.release();s.in.inChest=true;s.grab();s.in.inChest=false;s.in.hand=s.in.rest;s.frame();
    s.move({0,-1,0},.09);CHECK(s.g.cocked==0); // backwards is not the crank direction
    Sim t(15);t.release();t.in.inChest=true;t.grab();t.in.inChest=false;t.in.hand=t.in.rest;t.frame();
    t.move({0,1,0},.08);CHECK(t.g.cocked==1);
}
// R111: VrHeavyCrossbowHandPullR111 ON - the heavy crossbow is a string class:
// grip at the heavy string rest (y .3123), pull back to the nut, 90 % of 17.43 cm.
static void heavyHandPull(){
    CHECK(x::stringPullClass(15,true)&&!x::stringPullClass(15,false)&&x::stringPullClass(10,false)&&!x::stringPullClass(14,true));
    CHECK(!x::toolCockingClass(15,true)&&x::toolCockingClass(15,false)&&x::toolCockingClass(14,true)&&!x::toolCockingClass(10,true));
    Sim s(15);s.in.heavyHandPull=true;s.release();
    s.in.hand=s.in.rest+x::Vec{0,0,.03};s.grab();CHECK(s.m.stage()==x::Stage::Span);CHECK(s.g.log.empty());
    s.move({0,-1,0},.13);CHECK(s.g.cocked==0);          // more than the medium travel: not yet
    s.move({0,-1,0},.03);CHECK(s.g.cocked==1);CHECK(s.g.log==kCock);CHECK(s.g.tagCocked); // 16 cm >= 15.69
    s.release();s.g.log.clear();
    s.in.inQuiver=true;s.in.hand={.2,.3,-.2};s.grab();CHECK(s.g.log==kAmmo);CHECK(s.m.stage()==x::Stage::Bolt); // bolt like the others
    s.in.inQuiver=false;s.in.hand=(s.in.rest+s.in.rail)*.5;s.frame();s.g.log.clear();s.release();CHECK(s.g.log==kLoad);
    // the old crank direction (forward) does nothing now
    Sim p(15);p.in.heavyHandPull=true;p.release();p.in.hand=p.in.rest;p.grab();p.move({0,1,0},.10);CHECK(p.g.cocked==0);
    // chest grip = no tool, says how
    Sim c(15);c.in.heavyHandPull=true;c.release();c.in.inChest=true;c.in.hand={0,-.3,.2};c.grab();
    CHECK(c.m.stage()==x::Stage::Idle);CHECK(c.noted(x::Note::PullByHand));CHECK(c.g.log.empty());
    // let go halfway: no latch, the next pull cocks
    Sim r(15);r.in.heavyHandPull=true;r.release();r.in.hand=r.in.rest;r.grab();r.move({0,-1,0},.08);r.release();
    CHECK(r.g.cocked==0);CHECK(r.noted(x::Note::PullFurther));
    r.in.hand=r.in.rest;r.grab();r.move({0,-1,0},.16);CHECK(r.g.cocked==1);
    // slipped sideways: no latch either
    Sim q(15);q.in.heavyHandPull=true;q.release();q.in.hand=q.in.rest;q.grab();q.move({1,0,0},.16);CHECK(q.noted(x::Note::StringSlipped));
    q.release();q.in.hand=q.in.rest;q.grab();q.move({0,-1,0},.16);CHECK(q.g.cocked==1);
}
// R111: the chest/string zones of the Grip request follow the same switch.
static void heavyGripZones(){
    namespace cr=kcd2vr::crossbow_refusal_d85;
    cr::GripInput in{};in.classId=15;in.cocked=0;in.handFree=true;in.itemsRead=true;in.callerPinned=true;
    in.inChest=true;CHECK(cr::decideGrip(in).kind==2);             // OFF: chest = the game's cranequin action
    in.heavyHandPull=true;CHECK(cr::decideGrip(in).kind==0);       // ON: chest starts nothing
    in.inChest=false;in.atString=true;CHECK(cr::decideGrip(in).kind==1); // ON: the string
    in.atString=false;in.inQuiver=true;CHECK(cr::decideGrip(in).refusal==cr::Grip::SpanFirst);
    in.classId=14;in.inQuiver=false;in.inChest=true;CHECK(cr::decideGrip(in).kind==2); // goat's foot unchanged
}
// R117: VrMediumCrossbowHandPullR117 ON - the goat's foot (14) is a string class:
// grip at the medium string rest (y .2695), pull back to the nut, 90 % of 13.15 cm.
static void mediumHandPull(){
    CHECK(x::stringPullClass(14,false,true)&&!x::stringPullClass(14,true,false)&&x::stringPullClass(15,true,true)&&x::stringPullClass(10,false,false));
    CHECK(!x::toolCockingClass(14,false,true)&&x::toolCockingClass(14,true,false)&&x::toolCockingClass(15,false,true));
    Sim s(14);s.in.mediumHandPull=true;s.release();
    s.in.hand=s.in.rest+x::Vec{0,0,.03};s.grab();CHECK(s.m.stage()==x::Stage::Span);CHECK(s.g.log.empty());
    s.move({0,-1,0},.11);CHECK(s.g.cocked==0);          // 11 cm < 11.84 cm
    s.move({0,-1,0},.01);CHECK(s.g.cocked==1);CHECK(s.g.log==kCock);CHECK(s.g.tagCocked); // 12 cm cocks
    s.release();s.g.log.clear();
    s.in.inQuiver=true;s.in.hand={.2,.3,-.2};s.grab();CHECK(s.g.log==kAmmo);CHECK(s.m.stage()==x::Stage::Bolt);
    s.in.inQuiver=false;s.in.hand=(s.in.rest+s.in.rail)*.5;s.frame();s.g.log.clear();s.release();CHECK(s.g.log==kLoad);
    // chest grip (old spanner habit): no tool stage, one line how
    Sim c(14);c.in.mediumHandPull=true;c.release();c.in.inChest=true;c.in.hand={0,-.3,.2};c.grab();
    CHECK(c.m.stage()==x::Stage::Idle);CHECK(c.noted(x::Note::PullByHandNoTool));CHECK(c.g.log.empty());
    CHECK(x::text(x::Note::PullByHandNoTool)!=nullptr);
    // quiver before cocking: "pull the string back first", not "take the tool"
    Sim q(14);q.in.mediumHandPull=true;q.release();q.in.inQuiver=true;q.grab();CHECK(q.noted(x::Note::CockFirst));CHECK(!q.noted(x::Note::ToolFirst));
    // let go halfway / slip: no latch, the next pull cocks
    Sim r(14);r.in.mediumHandPull=true;r.release();r.in.hand=r.in.rest;r.grab();r.move({0,-1,0},.06);r.release();
    CHECK(r.g.cocked==0);CHECK(r.noted(x::Note::PullFurther));
    r.in.hand=r.in.rest;r.grab();r.move({0,-1,0},.12);CHECK(r.g.cocked==1);
    Sim p(14);p.in.mediumHandPull=true;p.release();p.in.hand=p.in.rest;p.grab();p.move({1,0,0},.16);CHECK(p.noted(x::Note::StringSlipped));
    p.release();p.in.hand=p.in.rest;p.grab();p.move({0,-1,0},.12);CHECK(p.g.cocked==1);
    // switch OFF: exactly the old spanner route (chest = Tool stage)
    Sim o(14);o.release();o.in.inChest=true;o.in.hand={0,-.3,.2};o.grab();CHECK(o.m.stage()==x::Stage::Tool);
    // grip zones: ON chest starts nothing, string = kind 1; OFF chest = kind 2
    namespace cr=kcd2vr::crossbow_refusal_d85;
    cr::GripInput in{};in.classId=14;in.cocked=0;in.handFree=true;in.itemsRead=true;in.callerPinned=true;in.inChest=true;
    CHECK(cr::decideGrip(in).kind==2);in.mediumHandPull=true;CHECK(cr::decideGrip(in).kind==0);
    in.inChest=false;in.atString=true;CHECK(cr::decideGrip(in).kind==1);
    in.atString=false;in.inQuiver=true;CHECK(cr::decideGrip(in).refusal==cr::Grip::SpanFirst);
    // give-back window: 3 s for the uncocked medium with the pull, 1.5 s otherwise
    CHECK(x::giveBackWindowMs(14,0,false,true)==x::kNativeWindowHeavyPullMs);
    CHECK(x::giveBackWindowMs(14,0,false,false)==x::kNativeWindowMs);
    CHECK(x::giveBackWindowMs(14,1,false,true)==x::kNativeWindowMs);
    CHECK(x::giveBackWindowMs(15,0,true,false)==x::kNativeWindowHeavyPullMs);
    CHECK(x::giveBackWindowMs(10,0,true,true)==x::kNativeWindowMs);
}
// R96/R111 give-back window (pure policy).
static void giveBackWindow(){
    CHECK(x::parkDecision(1000,2600,true,false,false,false)==x::Park::GiveBack);
    CHECK(x::parkDecision(1000,2600,true,false,false,false,x::kNativeWindowHeavyPullMs)==x::Park::Keep);
    CHECK(x::parkDecision(1000,4000,true,false,false,false,x::kNativeWindowHeavyPullMs)==x::Park::GiveBack);
    CHECK(x::parkDecision(1000,9000,true,true,false,false)==x::Park::Drop);   // hands at work: ours
    CHECK(x::parkDecision(1000,1100,true,false,true,false)==x::Park::Drop);   // never into a running action
    x::NativePass p{};p.begin(1000,false);CHECK(p.update(false,false,2000));CHECK(!p.update(false,false,2600));CHECK(!p.active);
}
static void rightHandCrossbow(){
    Sim s;s.in.leftHand=false;s.release();s.in.inQuiver=true;s.grab();CHECK(s.g.log.empty());CHECK(s.noted(x::Note::LeftHandOnly));
}
static void frameMath(){
    const float m[12]{0,-1,0,1.5f, 1,0,0,-2, 0,0,1,.25f};
    const x::Vec p{.1,.2,.3};const auto w=x::apply(m,p);x::Vec back{};CHECK(x::unapply(m,w,back));
    CHECK(x::length(back-p)<1e-6);
}
static void deferral(){
    x::DeferInput d{};d.enabled=true;d.henryEquipment=true;d.weaponClass=10;CHECK(x::deferRequest(d));
    d.weaponClass=14;CHECK(x::deferRequest(d));d.weaponClass=15;CHECK(x::deferRequest(d));
    auto e=d;e.b=1;CHECK(!x::deferRequest(e));          // native immediate cock+bind path stays native
    e=d;e.enabled=false;CHECK(!x::deferRequest(e));
    e=d;e.fault=true;CHECK(!x::deferRequest(e));
    e=d;e.ownReplay=true;CHECK(!x::deferRequest(e));
    e=d;e.henryEquipment=false;CHECK(!x::deferRequest(e)); // NPC crossbows untouched
    e=d;e.weaponClass=9;CHECK(!x::deferRequest(e));      // bows keep their own route
    e=d;e.a=1;CHECK(x::deferRequest(e));                   // trigger request (dl=1) too
}
int main(){
    lightCycle();pushIsNotPull();releaseHalfway();slip();noGrabFarFromString();heldGripNeedsNeutral();quiverBeforeCock();
    dropAwayFromRail();wrongHandRefused();bindRefused();handBusyAndNoBolts();contextLostDropsBolt();goatsFoot();cranequin();
    rightHandCrossbow();frameMath();deferral();heavyHandPull();heavyGripZones();giveBackWindow();mediumHandPull();
    std::printf("crossbow_hand_d89: %d passed, %d failed\n",g_pass,g_fail);
    return g_fail?1:0;
}
