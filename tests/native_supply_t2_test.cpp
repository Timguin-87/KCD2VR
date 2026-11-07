#include "../src/vrik/native_supply_t2.h"
#include <cassert>
#include <limits>
#include <cstdio>
int main(){
 namespace s=kcd2vr::supply_t2;
 s::Input i{};i.manager=1;i.player=2;i.character=3;i.body=4;i.request=5;i.sample=i.tick=1000;i.entity=6;
 i.allowed=i.tracked=true;i.pouchNear[0]=true;
 assert(!i.bone&&s::live(i,i.tick)); // invisible inventory zones need no attachment bone
 s::Gesture g{};const auto step=[&](unsigned dt,float grip,bool selected=true,bool reserved=false){
  i.tick+=dt;++i.sample;i.grip=grip;return g.next(i,i.tick,selected,reserved);};
 assert(step(10,1)==-1); // closed at startup cannot draw
 assert(step(10,0)==-1);assert(step(10,1)==-1); // closing only seeds the deliberate pull
 i.offset[0][0]=.14f;assert(step(79,1)==-1);assert(step(1,1)==0);
 assert(step(10,1)==-1); // once per Grip, including later samples
 i.pouchNear[0]=false;assert(step(10,0)==-1);assert(step(10,1)==-1);
 i.pouchNear[0]=true;assert(step(100,1)==-1); // carried fist is not a grab
 i.pouchNear[0]=false;i.pouchNear[1]=true;assert(step(10,0)==-1);assert(step(10,1)==-1);
 i.offset[1][0]=.119f;assert(step(80,1)==-1);i.offset[1][0]=.14f;assert(step(10,1)==1); // axe pull, not instant squeeze
 assert(step(10,0)==-1);i.hand[0]=22;assert(step(10,1)==-1); // occupied hand
 i.hand[0]=0;assert(step(90,1)==-1); // occupancy cancellation cannot resume the old pull
 assert(step(10,0)==-1);assert(step(10,1,true,true)==-1); // reserved/pending unit
 assert(step(10,1)==-1);assert(step(10,1,false)==-1);
 assert(step(100,1)==-1); // toggle-on while gripping cannot draw
 assert(step(10,0)==-1);assert(step(10,1)==-1);
 assert(g.next(i,i.tick+101,true,false)==-1); // source age cannot be refreshed by consumer
 assert(step(10,1)==-1); // recovering freshness requires a new neutral Grip
 assert(step(10,0)==-1);++i.body;assert(step(10,1)==-1); // new body requires neutral
 assert(step(10,0)==-1);assert(step(10,1)==-1);i.tracked=false;assert(step(10,1)==-1);
 i.tracked=true;i.offset[1][0]+=.20f;assert(step(90,1)==-1); // tracking loss cancels the unfinished pull
 i.grip=std::numeric_limits<float>::quiet_NaN();assert(!s::live(i,i.tick));
 i.grip=1;i.hand[0]=88;
 s::Lease l{};l.manager=i.manager;l.player=i.player;l.character=i.character;l.body=i.body;l.request=i.request;
 l.key=88;l.side=0;l.entity=72;l.attachment=12;l.tick=i.tick;l.held=true;
 assert(s::owns(l,i,88,0,i.tick));assert(!s::owns(l,i,88,-1,i.tick));assert(!s::owns(l,i,88,2,i.tick));
 assert(!s::owns(l,i,88,0,i.tick+151));i.hand[0]=89;assert(!s::owns(l,i,88,0,i.tick));
 // D48: the real finite-stock LEFT chest kernel separates intent from dispatch.
 s::Input chest{};chest.manager=1;chest.player=2;chest.character=3;chest.body=4;chest.request=5;chest.entity=6;
 chest.allowed=chest.tracked=chest.chestKnivesT26=true;chest.pouchNear[0]=true;chest.tick=chest.sample=1000;
 s::Gesture pouch{};
 const auto chestStep=[&](float grip,bool ready,bool selected=true,bool reserved=false){
  chest.tick+=16;++chest.sample;chest.grip=chest.grip18[1]=grip;
  return pouch.next(chest,chest.tick,selected,reserved,1,ready);};
 assert(chestStep(0,true)==-1&&pouch.armed);
 assert(chestStep(1,false)==-1&&pouch.armed);
 assert(chestStep(1,false)==-1&&pouch.armed);
 assert(chestStep(1,true)==0&&!pouch.armed); // same grip, no duplicate native dispatch
 assert(chestStep(1,true)==-1);
 assert(chestStep(0,true)==-1);assert(chestStep(1,false)==-1);
 assert(chestStep(0,false)==-1);assert(chestStep(0,true)==-1); // open hand cannot draw on admission recovery
 assert(chestStep(1,true)==0);
 assert(chestStep(0,true)==-1);assert(chestStep(1,false)==-1);
 chest.pouchNear[0]=false;assert(chestStep(1,false)==-1&&!pouch.armed);
 chest.pouchNear[0]=true;assert(chestStep(1,true)==-1); // leaving ends the deferred intent
 assert(chestStep(0,true)==-1);assert(chestStep(1,false)==-1);
 assert(chestStep(1,true,true,true)==-1&&!pouch.armed);assert(chestStep(1,true)==-1); // real hand owner cancels
 assert(chestStep(0,true)==-1);chest.trigger18[1]=1;
 assert(chestStep(1,false)==-1&&!pouch.armed);chest.trigger18[1]=0;assert(chestStep(1,true)==-1);
 assert(chestStep(0,true)==-1);assert(chestStep(1,false)==-1);assert(chestStep(1,true,false)==-1&&!pouch.armed);
 assert(chestStep(1,true)==-1); // OFF never revives a held old Grip
 // R109: a sample whose packet is not the exact completed source (allowed=0) or is stale keeps the neutral arm;
 // the squeeze at the pouch then draws on the next current sample. a squeeze elsewhere during it still spends the arm.
 assert(chestStep(0,true)==-1&&pouch.armed);
 chest.allowed=false;assert(chestStep(0,true,false)==-1&&pouch.armed);assert(chestStep(1,true,false)==-1&&pouch.armed);
 chest.allowed=true;assert(chestStep(1,true)==0&&!pouch.armed);
 assert(chestStep(0,true)==-1&&pouch.armed);
 chest.allowed=false;chest.pouchNear[0]=false;assert(chestStep(1,true,false)==-1&&!pouch.armed); // closed elsewhere
 chest.allowed=true;chest.pouchNear[0]=true;assert(chestStep(1,true)==-1);
 assert(chestStep(0,true)==-1&&pouch.armed);
 { const auto keep=chest.tick;chest.tick=0;++chest.sample;chest.grip=chest.grip18[1]=0;
   assert(pouch.next(chest,keep+16,false,false,1,true)==-1&&pouch.armed);chest.tick=keep; } // stale tick = transient
 assert(chestStep(1,true,false)==-1&&!pouch.armed); // allowed+fresh and really not selected (OFF) clears as before
 std::puts("PASS D48 finite LEFT chest dispatch deferral, release/outside/owner/fist/OFF cancellation; real production Gesture");
 std::puts("PASS T15 no-display supply: deliberate80ms/12cm pull; neutral/identity/source-age/occupied/reserved/tracking gates; actual-side lease bounds");
}
