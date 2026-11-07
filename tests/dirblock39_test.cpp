// C39 DIRBLOCK: directional NPC block for VR DIRECT - sector prediction, combat_zone.xml mapping, mailbox, update bounding.
#include "../src/vrik/dirblock39.h"
#include <cmath>
#include <cstdio>
#include <limits>
namespace d=kcd2vr::dirblock39;
static int failures=0,checks=0;
#define CHECK(c,msg) do{++checks;if(!(c)){++failures;std::printf("FAIL %s\n",msg);}else std::printf("PASS %s\n",msg);}while(0)
int main(){
    const d::V none{};
    // combat_zone.xml angle table (side the attack comes from; 0 deg = Henry's right, counter-clockwise)
    CHECK(d::zoneFromAngle(90)==0&&d::zoneFromAngle(45)==0&&d::zoneFromAngle(134.9)==0,"45..135 = zone 0 head");
    CHECK(d::zoneFromAngle(180)==1&&d::zoneFromAngle(135)==1&&d::zoneFromAngle(224.9)==1,"135..225 = zone 1 upper_left");
    CHECK(d::zoneFromAngle(270)==5&&d::zoneFromAngle(225)==5,"225..315 = zone 5 lower");
    CHECK(d::zoneFromAngle(0)==2&&d::zoneFromAngle(315)==2&&d::zoneFromAngle(44.9)==2&&d::zoneFromAngle(-10)==2&&d::zoneFromAngle(370)==2,"315..45 = zone 2 upper_right (wraps)");
    CHECK(d::zoneFromAngle(std::numeric_limits<double>::quiet_NaN())==-1,"NaN angle is no zone");
    // cuts: the sector is where the blade comes FROM
    auto p=d::predictCut({3,0.5,0},none);CHECK(p.valid&&p.zone==1&&p.angle==180,"blade moving to Henry's right came from his left: zone 1 (same as stroke_zone117)");
    p=d::predictCut({-3,0.5,0},none);CHECK(p.valid&&p.zone==2&&p.angle==0,"blade moving left came from the right: zone 2");
    p=d::predictCut({0.3,0.5,-3},none);CHECK(p.valid&&p.zone==0&&!p.nearBoundary,"downward cut came from above: zone 0");
    p=d::predictCut({2.5,0.5,-2.5},none);CHECK(p.valid&&p.nearBoundary,"a 45 degree diagonal is on the 0/1 boundary: not confident");
    p=d::predictCut({-1.5,0.5,2.5},none);CHECK(p.valid&&p.zone==2&&!p.nearBoundary,"upward cut keeps its lateral side (came from lower right -> 2), never table zone 5");
    p=d::predictCut({1.5,0.5,2.5},none);CHECK(p.valid&&p.zone==1,"upward cut from lower left -> 1");
    p=d::predictCut({0.1,0.2,3},none);CHECK(p.valid&&p.nearBoundary,"a vertical upward cut has no confident side");
    p=d::predictCut({0.5,0.2,0.4},none);CHECK(!p.valid,"slow tip motion is no prediction");
    p=d::predictCut({0.6,4,0.6},none);CHECK(!p.valid,"mostly forward motion (a push) is no cut sector");
    p=d::predictCut({std::numeric_limits<double>::infinity(),0,0},none);CHECK(!p.valid,"non-finite velocity is no prediction");
    // arc curvature: a blade rotating about the forward axis turns its velocity
    p=d::predictCut({0,0.3,-3},{0,0,0});const int straight=p.zone;
    p=d::predictCut({0,0.3,-3},{0,-15,0},0.05);CHECK(straight==0&&p.valid&&p.zone==0,"0.75 rad turn limited to 45 deg keeps a downward cut near the head zone");
    {const d::V v=d::turn({1,0,0},{0,0,10},0.05);CHECK(std::fabs(v.x-std::cos(0.5))<1e-9&&std::fabs(v.y-std::sin(0.5))<1e-9,"Rodrigues turn by |w|*t");}
    {const d::V v=d::turn({1,0,0},{0,0,100},1.0);CHECK(std::fabs(std::atan2(v.y,v.x)-d::kMaxTurn)<1e-9,"turn clamped to 45 deg");}
    // mailbox
    const auto w=d::pack(123456789012ull,1,180,false,1000);
    auto s=d::unpack(w,123456789012ull,1030,d::kFreshMs);CHECK(s.valid&&s.zone==1&&s.angle==180&&!s.nearBoundary&&s.ageMs==30,"mailbox round trip");
    CHECK(!d::unpack(w,123456789013ull,1030,d::kFreshMs).valid,"other gesture is not read");
    CHECK(!d::unpack(w,123456789012ull,1100,d::kFreshMs).valid&&d::unpack(w,123456789012ull,1100,d::kContactFreshMs).valid,"stale sample refused, contact window is wider");
    CHECK(d::pack(5,3,10,false,1)==0&&d::pack(5,-1,10,false,1)==0&&d::pack(0,1,10,false,1)==0&&d::pack(5,1,400,false,1)==0,"zones 3/4/-1, gesture 0 and bad angles are never packed");
    {const auto w2=d::pack(7,5,270,true,(1ull<<19)-5);const auto s2=d::unpack(w2,7,(1ull<<19)+10,d::kFreshMs);CHECK(s2.valid&&s2.zone==5&&s2.nearBoundary&&s2.ageMs==15,"tick wrap handled");}
    // initial choice
    d::Choice why{};d::Sample good{};good.valid=true;good.zone=2;good.angle=10;
    CHECK(d::chooseInitial(true,true,0,0,good,why)==2&&why==d::Choice::Predicted,"waiting enemy + fresh confident cut sector -> predicted zone");
    CHECK(d::chooseInitial(true,true,0,2,good,why)==2&&why==d::Choice::SameAsOrigin,"same sector as origin reported");
    CHECK(d::chooseInitial(true,false,0,0,good,why)==0&&why==d::Choice::NotWaiting,"busy enemy keeps origin (no change)");
    CHECK(d::chooseInitial(true,true,1,5,good,why)==5&&why==d::Choice::NotCut,"thrust keeps the native zone 5");
    CHECK(d::chooseInitial(false,true,0,0,good,why)==0&&why==d::Choice::Disabled,"menu off keeps origin");
    {d::Sample b=good;b.nearBoundary=true;CHECK(d::chooseInitial(true,true,0,0,b,why)==0&&why==d::Choice::NearBoundary,"boundary sample keeps origin");}
    CHECK(d::chooseInitial(true,true,0,1,d::Sample{},why)==1&&why==d::Choice::NoPrediction,"no sample keeps origin");
    // update bounding
    d::Plan pl{};pl.begin(0,0,0,1000);
    auto smp=[](int zone,std::uint64_t t,bool nearB=false){d::Sample x{};x.valid=true;x.zone=zone;x.angle=0;x.tickLow=t;x.nearBoundary=nearB;return x;};
    CHECK(pl.consider(smp(0,1001),false,1001)==d::Step::None,"same zone: nothing");
    CHECK(pl.consider(smp(1,1002),false,1002)==d::Step::Wait,"first different sample waits for stability");
    CHECK(pl.consider(smp(1,1002),false,1003)==d::Step::None,"the same pose sample twice is not stability");
    CHECK(pl.consider(smp(1,1010),false,1010)==d::Step::Wait,"stable but inside the minimum gap");
    CHECK(pl.consider(smp(1,1045),false,1045)==d::Step::Update,"stable and gap elapsed -> update");
    pl.committed(1,1045);CHECK(pl.zone==1&&pl.updates==1,"update committed");
    CHECK(pl.consider(smp(2,1050),false,1050)==d::Step::Wait&&pl.consider(smp(2,1060),false,1060)==d::Step::Wait&&pl.consider(smp(2,1090),false,1090)==d::Step::Update,"second update after its gap");
    pl.committed(2,1090);
    CHECK(pl.consider(smp(0,1200),false,1200)==d::Step::Wait&&pl.consider(smp(0,1250),false,1250)==d::Step::Budget,"third change refused: at most 2 updates per approach");
    {d::Plan q{};q.begin(0,0,0,0);CHECK(q.consider(smp(1,10,true),false,100)==d::Step::None&&q.consider(smp(1,20,true),false,200)==d::Step::None,"boundary samples never update");}
    {d::Plan q{};q.begin(0,0,0,0);(void)q.consider(smp(1,10),false,100);CHECK(q.consider(smp(1,20),true,200)==d::Step::Frozen&&q.consider(smp(1,30),false,300)==d::Step::Frozen,"NPC block started: frozen for the rest of the approach");}
    {d::Plan q{};q.begin(0,0,0,0);(void)q.consider(smp(1,10),false,100);(void)q.consider(smp(2,20),false,200);CHECK(q.consider(smp(1,30),false,300)==d::Step::Wait,"a flipping prediction restarts stability");}
    {d::Plan q{};q.begin(5,5,1,0);CHECK(q.consider(smp(1,10),false,100)==d::Step::None&&q.consider(smp(1,20),false,200)==d::Step::None,"thrust plans never take cut sectors");}
    CHECK(d::Plan{}.consider(smp(1,1),false,1)==d::Step::None,"no plan, no step");
    // later publishes of the same attempt
    {d::Plan q{};q.begin(1,2,0,0);bool kept=false;CHECK(q.keep(0,2,false,kept)==1&&!kept,"commit/contact of the same cut keeps our requested zone (not the origin)");
     CHECK(q.keep(1,5,true,kept)==1&&kept&&q.frozen,"cut->thrust reclassification after the NPC block started keeps the row the NPC took (C37C2 serial 76)");}
    {d::Plan q{};q.begin(2,2,0,0);bool kept=false;CHECK(q.keep(1,5,false,kept)==5&&!kept&&q.technique==1&&q.zone==5&&q.frozen,"before the NPC block, a reclassified thrust announces zone 5 and stops sector updates");}
    {d::Plan q{};bool kept=true;CHECK(q.keep(0,2,true,kept)==2&&!kept,"no plan: proposed zone untouched");}
    std::printf("%d/%d checks passed\n",checks-failures,checks);
    return failures?1:0;
}
