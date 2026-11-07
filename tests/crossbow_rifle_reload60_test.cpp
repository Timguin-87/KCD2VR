// DS-F94: crossbow/rifle reload stage policy (pure, host-tested).
#include "../src/subsystems/ranged_reload/crossbow_rifle_reload60.h"
#include <cstdio>
namespace rr=kcd2vr::crossbow_rifle_reload60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static rr::Request req(rr::Weapon w,std::uint64_t tick){rr::Request r{};r.weapon=w;r.weaponDrawn=true;r.current=true;r.ammoAvailable=true;r.handTracked=true;r.steady=false;r.tick=tick;return r;}
int main(){
    // crossbow: span held long enough -> place the bolt -> steady -> ready
    {rr::Machine m{};m.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(m.step(req(rr::Weapon::Crossbow,1100),s,1100)==rr::Stage::Empty); // span held
     CK(m.step(req(rr::Weapon::Crossbow,1400),s,1400)==rr::Stage::Empty); // 400ms span
     s.released=true; // the span completes
     CK(m.step(req(rr::Weapon::Crossbow,1500),s,1500)==rr::Stage::Working);
     s.released=false;s.placed=true;s.stage=rr::Machine::kPlace;
     CK(m.step(req(rr::Weapon::Crossbow,1600),s,1600)==rr::Stage::Loaded);
     auto r=req(rr::Weapon::Crossbow,1700);r.steady=true;
     CK(m.step(r,s,1700)==rr::Stage::Ready);}
    // a tap (span released too soon) faults
    {rr::Machine m{};m.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;s.released=true;
     CK(m.step(req(rr::Weapon::Crossbow,1100),s,1100)==rr::Stage::Fault);}
    // no ammo: the placement faults
    {rr::Machine m{};m.begin(rr::Weapon::Rifle,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(m.step(req(rr::Weapon::Rifle,1200),s,1200)==rr::Stage::Empty); // the dwell starts now
     CK(m.step(req(rr::Weapon::Rifle,1500),s,1500)==rr::Stage::Empty); // 300ms real hold
     s.released=true;CK(m.step(req(rr::Weapon::Rifle,1600),s,1600)==rr::Stage::Working);
     s.released=false;s.placed=true;s.stage=rr::Machine::kPlace;
     auto r=req(rr::Weapon::Rifle,1700);r.ammoAvailable=false;
     CK(m.step(r,s,1700)==rr::Stage::Fault);}
    // hand loss aborts to idle
    {rr::Machine m{};m.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(m.step(req(rr::Weapon::Crossbow,1400),s,1400)==rr::Stage::Empty);
     auto r=req(rr::Weapon::Crossbow,1500);r.handTracked=false;
     CK(m.step(r,s,1500)==rr::Stage::Idle);}
    // DS-F102: stage-matched evidence - an armed but untouched weapon earns NO
    // held-span credit (no valid span-stage evidence between the ticks)
    {rr::Machine m{};m.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=false;s.stage=rr::Machine::kSpan;
     CK(m.step(req(rr::Weapon::Crossbow,2000),s,2000)==rr::Stage::Empty);
     s.valid=true;
     CK(m.step(req(rr::Weapon::Crossbow,2200),s,2200)==rr::Stage::Empty); // span dwell starts NOW, not at begin
     s.released=true;
     CK(m.step(req(rr::Weapon::Crossbow,2300),s,2300)==rr::Stage::Fault);} // only ~100ms of real hold: a tap
    // DS-F102: a wrong-stage release cannot advance the machine
    {rr::Machine m{};m.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kPlace; // WRONG stage: the span must be kSpan
     CK(m.step(req(rr::Weapon::Crossbow,1400),s,1400)==rr::Stage::Empty); // no span credit
     s.stage=rr::Machine::kSpan;
     CK(m.step(req(rr::Weapon::Crossbow,1400),s,1400)==rr::Stage::Empty); // the dwell starts now
     CK(m.step(req(rr::Weapon::Crossbow,1700),s,1700)==rr::Stage::Empty); // 300ms real hold
     s.released=true;CK(m.step(req(rr::Weapon::Crossbow,1800),s,1800)==rr::Stage::Working);}
    // DS-F102: wrong-stage placement cannot place the ammo
    {rr::Machine m{};m.begin(rr::Weapon::Rifle,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(m.step(req(rr::Weapon::Rifle,1200),s,1200)==rr::Stage::Empty); // the dwell starts now
     CK(m.step(req(rr::Weapon::Rifle,1500),s,1500)==rr::Stage::Empty); // 300ms real hold
     s.released=true;CK(m.step(req(rr::Weapon::Rifle,1600),s,1600)==rr::Stage::Working);
     s.released=false;s.placed=true;s.stage=rr::Machine::kSpan; // WRONG stage
     CK(m.step(req(rr::Weapon::Rifle,1700),s,1700)==rr::Stage::Working); // still working
     s.stage=rr::Machine::kPlace;
     CK(m.step(req(rr::Weapon::Rifle,1800),s,1800)==rr::Stage::Loaded);}
    // H21: begin must not inherit a prior span clock, even when reused without release().
    {rr::Machine m{};m.begin(rr::Weapon::Crossbow,1000);rr::Step s{};s.valid=true;
     m.step(req(rr::Weapon::Crossbow,1100),s,1100);m.step(req(rr::Weapon::Crossbow,1400),s,1400);
     m.begin(rr::Weapon::Rifle,2000);CK(m.spanSince==0&&m.spanMs==0);
     m.step(req(rr::Weapon::Rifle,2100),s,2100);s.released=true;
     CK(m.step(req(rr::Weapon::Rifle,2200),s,2200)==rr::Stage::Fault);}
    // H21: the release sample itself counts toward continuous held dwell.
    {rr::Machine m{};m.begin(rr::Weapon::Crossbow,1000);rr::Step s{};s.valid=true;
     m.step(req(rr::Weapon::Crossbow,1100),s,1100);m.step(req(rr::Weapon::Crossbow,1300),s,1300);
     s.released=true;CK(m.step(req(rr::Weapon::Crossbow,1400),s,1400)==rr::Stage::Working);}
    // H21: a late release cannot bypass the maximum span time using cached dwell.
    {rr::Machine m{};m.begin(rr::Weapon::Crossbow,1000);rr::Step s{};s.valid=true;
     m.step(req(rr::Weapon::Crossbow,1100),s,1100);m.step(req(rr::Weapon::Crossbow,1400),s,1400);
     s.released=true;CK(m.step(req(rr::Weapon::Crossbow,5200),s,5200)==rr::Stage::Fault);}
    if(fails)return 1;std::puts("crossbow/rifle reload F102 OK");return 0;
}
