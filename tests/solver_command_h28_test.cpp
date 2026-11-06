#include "vrik/solver_command_h28.h"
#include <cstdio>
namespace s=kcd2vr::solver_command_h28;namespace b=kcd2vr::native_weapon_body_t4;
static unsigned checks=0,fails=0;
#define CK(x) do{++checks;if(!(x)){++fails;std::printf("FAIL %d %s\n",__LINE__,#x);}}while(0)
int main(){
 b::Achieved a{};a.owner={1,2,3,4,5,6,7,8,9,10,11,0};a.sample=100;a.sourceTick=1000;a.observedTick=1005;
 a.worldGeneration=12;a.bodyGeneration=13;a.trackingEpoch=14;a.cancelGeneration=16;a.colliderLease=17;a.bodyId=18;
 a.state=b::State::Active;a.poseValid=true;a.colliderRetired=false;
 b::ContactOwnerH24 c{};c.owner=a.owner;c.body=19;c.foreign=20;c.foreignType=0x5634;c.bodyId=a.bodyId;
 c.worldGeneration=a.worldGeneration;c.bodyGeneration=a.bodyGeneration;c.colliderLease=a.colliderLease;
 c.trackingEpoch=a.trackingEpoch;c.cancelGeneration=a.cancelGeneration;c.sourceSample=a.sample;c.sourceTick=a.sourceTick;c.observedTick=a.observedTick;c.active=true;
 s::Stamp p{};CK(s::make(a,c,21,1010,p));CK(!s::empty(p));CK(s::current(p,a,c,21,1010));
 auto checkBad=[&](s::Stamp bad){CK(!s::current(bad,a,c,21,1010));};
 #define BAD(field) {auto bad=p;++bad.field;checkBad(bad);}
 BAD(owner.key) BAD(owner.generation) BAD(owner.request) BAD(owner.epoch) BAD(owner.player) BAD(owner.manager) BAD(owner.character)
 BAD(owner.entityPointer) BAD(owner.attachment) BAD(owner.attachmentObject) BAD(owner.entityId) BAD(owner.side)
 BAD(worldGeneration) BAD(bodyGeneration) BAD(colliderLease) BAD(trackingEpoch) BAD(cancelGeneration) BAD(admissionSerial)
 BAD(body) BAD(foreign) BAD(bodyId) BAD(foreignType) BAD(sourceSample) BAD(sourceTick) BAD(observedTick)
 #undef BAD
 CK(!s::current(p,a,c,22,1010));CK(!s::current(p,a,c,21,1086));
 auto retired=a;retired.colliderRetired=true;CK(!s::current(p,retired,c,21,1010));
 auto wrong=c;wrong.foreignType=2;CK(!s::make(a,wrong,21,1010,p));CK(s::empty(p));
 CK(s::make(a,c,21,1010,p));
 auto latest=a;latest.sample=130;latest.sourceTick=1200;latest.observedTick=1205;
 auto live=c;live.sourceSample=130;live.sourceTick=1200;live.observedTick=1205;
 CK(!s::current(p,latest,live,21,1210));CK(s::currentIncarnation(p,latest,live,21,1210));
 auto event=c;event.sourceSample=125;event.sourceTick=1180;event.observedTick=1185;s::Stamp e{};
 CK(s::makeContact(event,latest,live,21,1210,e));CK(e.sourceSample==125&&e.sourceTick==1180&&e.observedTick==1185);
 event.sourceTick=1000;CK(!s::makeContact(event,latest,live,21,1210,e));CK(s::empty(e));
 s::Stamp empty{};CK(s::empty(empty));empty.owner.manager=1;CK(!s::empty(empty));empty={};empty.worldGeneration=1;CK(!s::empty(empty));
 empty={};empty.sourceTick=1;CK(!s::empty(empty));empty={};empty.bodyId=0;CK(!s::empty(empty));
 std::printf("H28 solver stamp=%u/%u\n",checks-fails,checks);return fails?1:0;}
