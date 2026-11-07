#include "../src/vrik/guard_motion168.h"
#include "../src/vrik/native_bridges168.h"
#include "../src/diagnostics/capture168.h"
#include "../src/diagnostics/intent168.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <thread>
int main(){
 namespace c=kcd2vr::diagnostics::capture168; c::Gate gate;
 assert(!gate.any(1000));gate.select(c::Domain::Combat,true,1000);
 assert(gate.active(c::Domain::Combat,1000)&&!gate.active(c::Domain::Interaction,1000));
 assert(!gate.active(c::Domain::Combat,999)&&gate.active(c::Domain::Combat,180999));
 assert(!gate.active(c::Domain::Combat,181000)&&gate.poll(181000)==1&&gate.mask.load()==0);
 gate.select(c::Domain::Interaction,true,2000);gate.select(c::Domain::Hud,true,2100);gate.select(c::Domain::Interaction,false,2110);
 assert(gate.active(c::Domain::Hud,2120)&&!gate.active(c::Domain::Interaction,2120));
 gate.select(c::Domain::Combat,true,UINT64_MAX-12);assert(!gate.active(c::Domain::Combat,UINT64_MAX-12));
 assert(c::essential("KCD2VR BUILD id")&&!c::essential("V168 periodic status"));
 assert(c::domain("V168 F8_LIFECYCLE event") == c::Domain::Interaction && c::domain("V166 HUD_COST") == c::Domain::Hud);
 namespace gm=kcd2vr::guard_motion168;gm::State motion;
 gm::Sample s{1000,1,1,77,{0,0,0},{0,0,1.6},true};assert(!motion.update(s));
 for(int i=0;i<8;++i){s.tick+=20;++s.sequence;assert(!motion.update(s));}assert(motion.armed);
 s.tick+=20;++s.sequence;s.hand.x=.03;assert(!motion.update(s));s.tick+=20;++s.sequence;s.hand.x=.06;assert(!motion.update(s));
 s.tick+=20;++s.sequence;s.hand.x=.09;assert(motion.update(s));assert(!motion.update(s));
 s.tick+=20;++s.sequence;s.hand.x=.14;assert(!motion.update(s));
 for(int i=0;i<7;++i){s.tick+=20;++s.sequence;assert(!motion.update(s));}assert(!motion.armed);
 for(int i=0;i<8;++i){s.tick+=20;++s.sequence;assert(!motion.update(s));}assert(motion.armed);
 s.tick+=20;++s.sequence;s.head.x=.15;assert(!motion.update(s)); // head alone
 s.tick+=20;++s.sequence;s.hand.x+=.4;s.head.x+=.4;assert(!motion.update(s)); // whole-body translation
 s.eligible=false;assert(!motion.update(s)&&!motion.armed);
 s.eligible=true;s.hand.x=NAN;assert(!motion.update(s)&&!motion.armed);
 namespace b=kcd2vr::bridges168; b::Ticket t{1,1000,2,3,77,4,5,9,11,12,13,14,1,true};
 b::Live l{1050,2,3,77,4,5,12,13,14,2,true,true};assert(b::qualify(t,l,0)==b::Denial::None);
 auto change=[&](b::Live x,b::Denial d){assert(b::qualify(t,x,0)==d);};
 auto x=l;x.now=1151;change(x,b::Denial::Expired);x=l;x.now=999;change(x,b::Denial::Expired);
 x=l;x.key++;change(x,b::Denial::Identity);x=l;x.policy++;change(x,b::Denial::Identity);
 x=l;x.opponent++;change(x,b::Denial::Identity);x=l;x.tracked=false;change(x,b::Denial::Tracking);
 x=l;x.enemyAttacking=false;change(x,b::Denial::OpponentNotAttacking);x=l;x.enemyZone=3;change(x,b::Denial::UnknownZone);
 x=l;x.enemyZone=1;change(x,b::Denial::NotOpposite);assert(b::qualify(t,l,1)==b::Denial::AlreadyConsumed);
 for(int z: {0,1,2,5})for(int e:{0,1,2,5}){t.origin=z;l.enemyZone=e;assert((b::qualify(t,l,0)==b::Denial::None)==b::opposite(z,e));}
 t.thrust=false;assert(b::qualify(t,l,0)==b::Denial::NoThrust);
 b::SpecialRequest r{1,1000,2,3,4,5,6};assert(b::fresh(r,1120,2,3)&&!b::fresh(r,1121,2,3)&&!b::fresh(r,999,2,3)&&!b::fresh(r,1020,3,3));
 b::Queue queue;for(int i=0;i<127;++i){b::Event e{};e.serial=i+1;assert(queue.push(e));}assert(!queue.push({})&&queue.dropped==1);
 b::Event e{};for(int i=0;i<127;++i){assert(queue.pop(e)&&e.serial==unsigned(i+1));}assert(!queue.pop(e));
 namespace in=kcd2vr::diagnostics::intent168;
 {in::Scope s1{{1,42,in::Source::R3}};assert(in::current.serial==1);{in::Scope s2{{2,42,in::Source::Thrust}};assert(in::current.serial==2);}assert(in::current.serial==1);}
 assert(in::current.serial==0);
 std::puts("PASS finite capture/expiry, motion/rearm/head/body/invalid tracking rejection, opposite-thrust identity/timing, special TTL, queue and nested provenance");
}
