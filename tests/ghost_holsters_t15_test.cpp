#include "vrik/native_supply_t2.h"
#include "subsystems/throw_drop/hip_selection_t10.h"
#include "vrik/equipment_gesture114.h"
#include <cassert>
#include <limits>
#include <cstdio>
using namespace kcd2vr;
int main(){
 supply_t2::Input i{};i.manager=1;i.player=2;i.character=3;i.entity=4;i.body=5;i.request=6;i.sample=i.tick=1000;i.allowed=i.tracked=true;
 assert(!i.bone&&supply_t2::live(i,1000)); // invisible pockets need no attachment bone
 supply_t2::Gesture g;auto step=[&](unsigned dt,float grip){i.tick+=dt;++i.sample;i.grip=grip;return g.next(i,i.tick,true,false);};
 i.pouchNear[0]=true;assert(step(10,1)==-1);assert(step(10,0)==-1);assert(step(10,1)==-1);
 i.offset[0][0]=.14f;assert(step(79,1)==-1);assert(step(1,1)==0);assert(step(10,1)==-1);
 // A new native transfer/empty hand allows next ordinary gear, without supply pending state.
 hip_t10::Input h{};h.allowed=h.equipment=h.stock=true;h.key=44;h.generation=8;h.body=5;h.request=6;h.tick=1200;h.sample=20;
 h.available[0]=h.available[1]=false;h.distance[0]=.10;h.distance[1]=.02;h.distance[2]=.03;
 hip_t10::State hs;assert(hip_t10::update(hs,h).owner==hip_t10::Owner::Equipment);
 equipment114::State normal{};equipment114::Input n{};n.key=44;n.generation=8;n.epoch=6;n.tick=1200;n.eligible=n.mountKnown=true;n.mount={};n.hand={.1,0,0};n.radius=.24;
 assert(equipment114::update(normal,n)==equipment114::Request::None);n.grip=1;n.tick+=10;equipment114::update(normal,n);
 n.tick+=80;n.hand={.24,0,0};assert(equipment114::update(normal,n)==equipment114::Request::Draw);
 n.tick+=10;n.drawn=true;equipment114::update(normal,n);assert(!normal.pending);
 // Real axe rear pocket is a separate owner; same pull contract, not hip equipment.
 i.pouchNear[0]=false;i.pouchNear[1]=true;i.offset[1][0]=0;assert(step(10,0)==-1);assert(step(10,1)==-1);i.offset[1][0]=.15f;assert(step(80,1)==1);
 assert(step(10,1)==-1);assert(step(10,0)==-1);i.hand[0]=99;assert(step(10,1)==-1);i.hand[0]=0;assert(step(90,1)==-1);
 // Carrying a fist into a zone, interrupted tracking, stale pose and NaN never draw.
 assert(step(10,0)==-1);i.pouchNear[1]=false;assert(step(10,0)==-1);assert(step(10,1)==-1);i.pouchNear[1]=true;assert(step(100,1)==-1);
 assert(step(10,0)==-1);assert(step(10,1)==-1);i.allowed=false;assert(step(10,1)==-1);i.allowed=true;i.offset[1][0]=.35;assert(step(100,1)==-1);
 assert(g.next(i,i.tick+101,true,false)==-1);i.grip=std::numeric_limits<float>::quiet_NaN();assert(!supply_t2::live(i,i.tick));
 std::puts("T15 ghost pockets: deliberate pull, no-bone identity, empty-stock ordinary holster recovery and native hand ACK PASS");
}
