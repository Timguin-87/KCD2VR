#include "generated_melee5916c3.inc"
#include "vrik/prop_drive5916c1.h"
#include "vrik/workstation178r20c1.h"
#include "diagnostics/capture168.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
static unsigned assertions=0;
static void verify(bool ok){++assertions;assert(ok);}
int main(){
    workstation178r20c1::alchemyPassenger42=false; // explicit tracked-alchemy comparison, not new default

    // Entire production physical kernel, controlled clear sweeps from MeleeReplay.
    {MeleeReplay r;r.in.physicalCombat177=true;r.in.naturalCombat117=true;r.in.positionGuard176=true;
     r.in.hands[0].q=m::from_rotation_vector({0,0,-1.5707963267948966});r.in.intentHands[0]=r.in.hands[0];
     for(int n=0;n<30;++n)r.move({});verify(r.out.guard117);r.commands.clear();
     r.in.kickRequest59=r.in.tick;
     r.in.kickTicket59c3={1,r.in.tick,{55,r.profiles[0].key,r.profiles[0].generation,r.in.bodyEpoch,r.in.epoch}};
     kick5916c3::Delivery receipt;
     r.move({});r.move({});unsigned count=0;
     for(const auto&c:r.commands)if(c.kick59){++count;
         verify(!kick5916c3::guardReject(r.in.naturalCombat117,r.out.guard117,c.kick59));
         verify(c.kickTicket59c3.id==1);
         verify(c.kind==CommandKind::Prepare?receipt.begin(c.kickTicket59c3,c.kickTicket59c3.owner,c.tick):
                                              receipt.release(c.kickTicket59c3,c.kickTicket59c3.owner,c.tick));}
     verify(count==2);verify(kick5916c3::guardReject(true,true,false));
     ++r.profiles[0].generation;r.profiles[0].key=2;for(int n=0;n<12;++n)r.move({});
     count=0;for(const auto&c:r.commands)count+=c.kick59;verify(count==2);
    }
    {MeleeReplay r;r.in.physicalCombat177=true;r.move({});r.commands.clear();
     r.in.tick+=800;r.in.timeNs+=800000000;r.in.kickRequest59=r.in.tick-790;
     for(int n=0;n<5;++n)r.move({});unsigned count=0;for(const auto&c:r.commands)count+=c.kick59;
     verify(count==0);verify(r.out.kickDrop59c2==5);
    }
    // R59.16C4: production identity. The ticket's request field is the WEAPON request epoch, which differs from the
    // pose epoch after any VR-mode/F11/holster change. C3 compared it with in.epoch and dropped every such kick.
    {MeleeReplay r;r.in.physicalCombat177=true;for(int n=0;n<5;++n)r.move({});r.commands.clear();
     const std::uint64_t weaponRequest=r.in.epoch+7;
     r.in.kickRequest59=r.in.tick;
     r.in.kickTicket59c3={2,r.in.tick,{55,r.profiles[0].key,r.profiles[0].generation,r.in.bodyEpoch,weaponRequest}};
     r.move({});r.move({});unsigned count=0;kick5916c3::Delivery d;bool begun=false,released=false;
     for(const auto&c:r.commands)if(c.kick59){++count;const kick5916c3::Owner live{55,c.key,c.generation,c.bodyEpoch,weaponRequest};
         if(c.kind==CommandKind::Prepare)begun=d.begin(c.kickTicket59c3,live,c.tick);else released=d.release(c.kickTicket59c3,live,c.tick);}
     verify(count==2&&begun&&released&&r.out.kickDrop59c2!=6);
     // a ticket from a different weapon is still refused
     r.in.kickRequest59=r.in.tick;r.in.kickTicket59c3={3,r.in.tick,{55,r.profiles[0].key+1,r.profiles[0].generation,r.in.bodyEpoch,weaponRequest}};
     r.commands.clear();r.move({});r.move({});count=0;for(const auto&c:r.commands)count+=c.kick59;verify(count==0&&r.out.kickDrop59c2==6);
    }
    // R59.16C4 (user decision): a HELD wind-up is cancelled for the kick in the same frame.
    {MeleeReplay r;r.in.physicalCombat177=true;for(int n=0;n<5;++n)r.move({});r.commands.clear();
     r.in.windupHold57=r.in.thrustAsSwing55=true; // the held wind-up feature as in F4 third mode
     auto& w=r.state.weapon[0];w.prepared=true;w.cocked57.active=true;w.released44c1=false;
     r.in.kickRequest59=r.in.tick+10;r.in.kickTicket59c3={4,r.in.tick+10,{55,r.profiles[0].key,r.profiles[0].generation,r.in.bodyEpoch,9}};
     r.move({});bool abort=false,kickPrepare=false;
     for(const auto&c:r.commands){if(c.kind==CommandKind::Abort&&!c.kick59&&!kickPrepare)abort=true;if(c.kick59&&c.kind==CommandKind::Prepare)kickPrepare=true;}
     verify(abort&&kickPrepare);
    }
    // R59.16C4 (user decision): the sword Grip may be open; the kick is still prepared and released exactly once.
    {MeleeReplay r;r.in.physicalCombat177=true;for(int n=0;n<5;++n)r.move({});
     r.in.grip[0]=0;r.move({});r.commands.clear();
     r.in.kickRequest59=r.in.tick+10;r.in.kickTicket59c3={5,r.in.tick+10,{55,r.profiles[0].key,r.profiles[0].generation,r.in.bodyEpoch,9}};
     for(int n=0;n<6;++n)r.move({});unsigned prep=0,rel=0;
     for(const auto&c:r.commands)if(c.kick59){if(c.kind==CommandKind::Prepare)++prep;else if(c.kind==CommandKind::Slash)++rel;verify(c.kickTicket59c3.id==5);}
     verify(prep==1&&rel==1&&r.out.releaseReason121[0]==2);
     // stale open-hand press is reported, not fired
     r.commands.clear();r.in.kickRequest59=r.in.tick>700?r.in.tick-700:1;r.in.kickTicket59c3={6,r.in.kickRequest59,{55,r.profiles[0].key,r.profiles[0].generation,r.in.bodyEpoch,9}};
     r.move({});unsigned any=0;for(const auto&c:r.commands)any+=c.kick59;verify(any==0&&r.out.kickDrop59c2==5);
    }
    {kick5916c3::Owner o{1,2,3,4,5};kick5916c3::Ticket a{100,1000,o},b{101,1040,o};kick5916c3::Delivery d;
     verify(d.begin(a,o,1000));verify(!d.begin(a,o,1001));verify(d.active(o,1020));
     auto other=o;other.generation++;verify(!d.release(a,other,1020));verify(!d.release(b,o,1050));
     verify(d.release(a,o,1020));verify(!d.release(a,o,1021));verify(!d.begin(a,o,1030));
     verify(d.begin(b,o,1050));d.cancel();verify(!d.release(b,o,1060));verify(!d.begin(b,o,1061));
     auto late=b;late.id++;verify(!d.begin(late,o,1641));verify(!d.begin(late,o,1039));
    }
    // Actual servo; model supplies native gravity in 1 ms substeps. No real engine simulation.
    for(unsigned dt:{11u,33u,50u,80u})for(double mass:{.3,5.,15.}){
        namespace q=prop5916c1; q::Controller c;q::Dynamics d{};d.mass=mass;q::p::Pose actual{},target{};
        c.seed(target,1,1000);
        for(unsigned n=1;n<=3000/dt;++n){auto cmd=c.update(target,actual,d,n+1,1000+dt*n,1000+dt*n);
            assert(cmd.decision==q::Decision::Drive);assert(cmd.dt<=.033001);
            d.linear=cmd.linear;d.angular=cmd.angular;
            for(unsigned j=0;j<dt;++j){d.linear.z-=9.81*.001;actual.p=actual.p+d.linear*.001;}d.centre=actual.p;}
        verify(std::abs(actual.p.z)<.08);
    }
    {namespace w=workstation178r20c1; w::Controller c;w::setExperiment36(true);combat_rules178r10::f5Rung=2;
     std::array<w::Sample,2> h{};for(auto& s:h){s.valid=true;s.sequence=1;s.tick=1000;}
     c.update(w::Kind::Alchemy,42,1,h,1000);
     bool a=false,lb=true,rb=true;float rt=.6f;c.apply(1000,a,lb,rb,rt);verify(!a&&lb&&rb&&rt==.6f);
     h[0].grip=.8;h[0].tick=h[1].tick=1020;++h[0].sequence;++h[1].sequence;
     verify(c.update(w::Kind::Alchemy,42,1,h,1020).use);verify(c.ownsGrip(0,1020)&&!c.ownsGrip(1,1020));
     c.apply(1020,a,lb,rb,rt);verify(a&&lb&&rb);w::setExperiment36(false);verify(!c.ownsGrip(0,1020));
    }
    namespace cap=diagnostics::capture168;
    for(unsigned i=0;i<cap::DomainCount;++i)cap::gate.select(cap::Domain(i),false,1000);
    // R59.16C7E-DECAP1: kick receipts are F3 material (quiet mode drops them, the combat recorder accepts them).
    verify(!cap::accept("R59.16C3 R3_KICK59 stage=CONSUMER_REJECT reason=STALE",1001));
    cap::gate.select(cap::Domain::Combat,true,1000);
    verify(cap::accept("R59.16C3 R3_KICK59 stage=CONSUMER_REJECT reason=STALE",1001));
    verify(cap::accept("R59.16C2 R3_KICK59 stage=DROPPED reason=HELD_WINDUP",1001));
    cap::gate.select(cap::Domain::Combat,false,1000);
    verify(!cap::accept("R59.16C3 F5_PROP_BODY sample=1",1001));
    cap::gate.select(cap::Domain::Interaction,true,1000);verify(cap::accept("R59.16C3 F5_PROP_BODY sample=1",1001));
    std::printf("C3 regression %u assertions; native leaves/model explicitly substituted\n",assertions);
}
