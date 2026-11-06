// DS-F94: crossbow/rifle reload DRIVER (pure per-frame policy drive, host-tested).
// Exercises the full span->place->ready cycle and the fault/cancellation paths
// through the per-frame driver, including the bow drive's once-per-source-tick
// gate and the stale/reversed source cancellation. No native code is reachable
// from this header (the UNPROVEN seam is a comment).
#include "../src/subsystems/ranged_reload/crossbow_rifle_reload_drive60.h"
#include <cstdio>
#include <cstring>
namespace rd=kcd2vr::crossbow_rifle_reload_drive60;
namespace rr=kcd2vr::crossbow_rifle_reload60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static rr::Request req(rr::Weapon w,std::uint64_t tick){rr::Request r{};r.weapon=w;r.weaponDrawn=true;r.current=true;r.ammoAvailable=true;r.handTracked=true;r.steady=false;r.tick=tick;return r;}
static const char* R(const rd::Output& o){return o.reason?o.reason:"(none)";}
int main(){
    // 1. crossbow: full span->place->ready cycle driven per source tick
    {rd::Driver d{};rd::Output o{};
     CK(!d.armed());d.begin(rr::Weapon::Crossbow,1000);CK(d.armed());
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(d.drive(req(rr::Weapon::Crossbow,1000),s,1000,&o)==rr::Stage::Empty&&!o.changed); // armed Empty, span clock starts
     // same source tick reread (stereo/game loop) must not dwell or advance
     CK(d.drive(req(rr::Weapon::Crossbow,1000),s,1040,&o)==rr::Stage::Empty&&!o.changed);
     CK(d.drive(req(rr::Weapon::Crossbow,1250),s,1250,&o)==rr::Stage::Empty); // 250ms span held
     s.released=true; // span completes
     CK(d.drive(req(rr::Weapon::Crossbow,1300),s,1300,&o)==rr::Stage::Working&&o.changed&&!o.faulted);
     s.released=false;s.placed=true;s.stage=rr::Machine::kPlace; // bolt placed
     CK(d.drive(req(rr::Weapon::Crossbow,1400),s,1400,&o)==rr::Stage::Loaded&&o.changed);
     auto r=req(rr::Weapon::Crossbow,1500);r.steady=true;
     CK(d.drive(r,s,1500,&o)==rr::Stage::Ready&&o.changed);
     // fired: ammo gone -> normal ready exit to Idle (caller re-arms Empty)
     auto r2=req(rr::Weapon::Crossbow,1600);r2.steady=true;r2.ammoAvailable=false;
     CK(d.drive(r2,s,1600,&o)==rr::Stage::Idle&&!o.faulted&&std::strcmp(R(o),"READY_EXIT")==0);
     CK(!d.armed());}
    // 2. rifle: the same span->place->steady ladder
    {rd::Driver d{};rd::Output o{};d.begin(rr::Weapon::Rifle,2000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(d.drive(req(rr::Weapon::Rifle,2300),s,2300,&o)==rr::Stage::Empty); // the dwell starts now
     CK(d.drive(req(rr::Weapon::Rifle,2600),s,2600,&o)==rr::Stage::Empty); // 300ms real hold
     s.released=true;CK(d.drive(req(rr::Weapon::Rifle,2700),s,2700,&o)==rr::Stage::Working);
     s.released=false;s.placed=true;s.stage=rr::Machine::kPlace;
     CK(d.drive(req(rr::Weapon::Rifle,2800),s,2800,&o)==rr::Stage::Loaded);
     auto r=req(rr::Weapon::Rifle,2900);r.steady=true;
     CK(d.drive(r,s,2900,&o)==rr::Stage::Ready);}
    // 3. a tap (span released too soon) faults with the tap reason
    {rd::Driver d{};rd::Output o{};d.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;s.released=true;
     CK(d.drive(req(rr::Weapon::Crossbow,1100),s,1100,&o)==rr::Stage::Fault&&o.faulted&&std::strcmp(R(o),"TAP_NOT_SPAN")==0);
     // the fault latches until release()
     CK(d.drive(req(rr::Weapon::Crossbow,1200),s,1200,&o)==rr::Stage::Fault&&o.faulted&&!o.changed);}
    // 4. placement without ammo faults
    {rd::Driver d{};rd::Output o{};d.begin(rr::Weapon::Rifle,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(d.drive(req(rr::Weapon::Rifle,1200),s,1200,&o)==rr::Stage::Empty); // the dwell starts now
     CK(d.drive(req(rr::Weapon::Rifle,1500),s,1500,&o)==rr::Stage::Empty); // 300ms real hold
     s.released=true;CK(d.drive(req(rr::Weapon::Rifle,1600),s,1600,&o)==rr::Stage::Working);
     s.released=false;s.placed=true;s.stage=rr::Machine::kPlace;
     auto r=req(rr::Weapon::Rifle,1700);r.ammoAvailable=false;
     CK(d.drive(r,s,1700,&o)==rr::Stage::Fault&&o.faulted&&std::strcmp(R(o),"NO_AMMO")==0);}
    // 5. hand loss aborts to Idle (identity loss, not a fault)
    {rd::Driver d{};rd::Output o{};d.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(d.drive(req(rr::Weapon::Crossbow,1400),s,1400,&o)==rr::Stage::Empty);
     auto r=req(rr::Weapon::Crossbow,1500);r.handTracked=false;
     CK(d.drive(r,s,1500,&o)==rr::Stage::Idle&&!o.faulted&&std::strcmp(R(o),"IDENTITY_LOST")==0);}
    // 6. an over-long stage times out
    {rd::Driver d{};rd::Output o{};d.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(d.drive(req(rr::Weapon::Crossbow,3000),s,3000,&o)==rr::Stage::Empty);
     CK(d.drive(req(rr::Weapon::Crossbow,8000),s,8000,&o)==rr::Stage::Fault&&o.faulted&&std::strcmp(R(o),"STAGE_TIMEOUT")==0);}
    // 7. a stale source tick cancels the policy (bow's 80 ms freshness window)
    {rd::Driver d{};rd::Output o{};d.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(d.drive(req(rr::Weapon::Crossbow,1100),s,1100,&o)==rr::Stage::Empty);
     CK(d.drive(req(rr::Weapon::Crossbow,1100),s,1300,&o)==rr::Stage::Idle&&!o.faulted&&std::strcmp(R(o),"SOURCE_STALE")==0);}
    // 8. a reversed source tick cancels the policy
    {rd::Driver d{};rd::Output o{};d.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(d.drive(req(rr::Weapon::Crossbow,1500),s,1500,&o)==rr::Stage::Empty);
     CK(d.drive(req(rr::Weapon::Crossbow,1400),s,1400,&o)==rr::Stage::Idle&&!o.faulted&&std::strcmp(R(o),"SOURCE_REVERSED")==0);}
    // 9. an invalid weapon never arms
    {rd::Driver d{};rd::Output o{};d.begin(rr::Weapon::None,1000);CK(!d.armed());
     CK(d.drive(req(rr::Weapon::None,1100),rr::Step{},1100,&o)==rr::Stage::Idle&&!o.changed);}
    // 10. DS-F103: a SAME-TICK context loss releases immediately (the dedup
    // must not swallow the cancellation)
    {rd::Driver d{};rd::Output o{};d.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(d.drive(req(rr::Weapon::Crossbow,1500),s,1500,&o)==rr::Stage::Empty);
     auto r=req(rr::Weapon::Crossbow,1500);r.handTracked=false; // same tick: tracking invalidated
     CK(d.drive(r,s,1500,&o)==rr::Stage::Idle&&!o.faulted&&std::strcmp(R(o),"IDENTITY_LOST")==0);}
    // 11. DS-F103: a same-tick weapon replacement releases immediately
    {rd::Driver d{};rd::Output o{};d.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(d.drive(req(rr::Weapon::Crossbow,1500),s,1500,&o)==rr::Stage::Empty);
     auto r=req(rr::Weapon::Rifle,1500); // same tick: the weapon changed
     CK(d.drive(r,s,1500,&o)==rr::Stage::Idle&&!o.faulted&&std::strcmp(R(o),"IDENTITY_LOST")==0);}
    // 12. DS-F103: a same-tick menu/context loss (current=false) releases
    {rd::Driver d{};rd::Output o{};d.begin(rr::Weapon::Crossbow,1000);
     rr::Step s{};s.valid=true;s.stage=rr::Machine::kSpan;
     CK(d.drive(req(rr::Weapon::Crossbow,1500),s,1500,&o)==rr::Stage::Empty);
     auto r=req(rr::Weapon::Crossbow,1500);r.current=false; // same tick: menu opened
     CK(d.drive(r,s,1500,&o)==rr::Stage::Idle&&!o.faulted&&std::strcmp(R(o),"IDENTITY_LOST")==0);}
    if(fails)return 1;std::puts("crossbow/rifle reload drive F103 OK");return 0;
}
