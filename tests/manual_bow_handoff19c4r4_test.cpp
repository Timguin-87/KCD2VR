#define main inherited_bow_coupling_tests
#include "bow_coupling39c1_test.cpp"
#undef main
#include <atomic>
#include "core/snapshot_mailbox.h"
static bool manual=true;
static bool StagedBowEnabled59(){return manual;}
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1};
#include "manual_bow_handoff19c4r4.inc"
static void empty(Rig& r){
 manual=true;g_weapon_request_epoch6694=1;g_manual_nock_claim19c4r4=0;g_manual_bow_ready596.publish({});
 r.s={};r.i.epoch=r.i.bodyEpoch=1;r.i.bowGripDraw47c2=true;r.i.grip[0]=r.i.trigger[0]=0;r.next();r.next();
 r.i.bowActions=false;r.next();r.i.grip[0]=1;r.next();r.i.bowActions=true;r.begins=r.fires=r.cancels=0;
}
static ManualReady596 receipt(const Rig& r){ManualReady596 q{r.p[1].key,1,1,r.i.tick,true,r.p[1].generation};q.nockAck={1,q.key,q.generation,1,1,r.i.sample,r.i.tick};
 q.nockAck.releaseToNock56=true;q.nockAck.releaseSample56=r.i.sample-1;q.nockAck.releaseTick56=r.i.tick-10;
 q.nockAck.handEntity56=77;q.nockAck.handIdentity56=0x1234;return q;}
static void frame(Rig& r){
 ++r.i.sample;r.i.tick=1000+10*r.i.sample;r.i.timeNs=r.i.tick*1000000;
 ManualNockInput19c4r4(r.i,r.p[1],r.s.sample);
 r.o=ph::step(r.s,r.i,r.p,[&](const ph::Profile&,ph::Pose,ph::Pose,bool){ph::Contact103 c{};c.known=true;c.fraction=1;return c;});
 r.begins+=commands(r.o,ph::CommandKind::BowBegin);r.fires+=commands(r.o,ph::CommandKind::BowFire);r.cancels+=commands(r.o,ph::CommandKind::BowCancel);
}
int main(){
 Rig r;empty(r);auto q=receipt(r);g_manual_bow_ready596.publish(q);frame(r);
 ck(r.o.bowHeld&&r.begins==1&&r.fires==0,"native attached nock after real release bridges one NEW Grip into exactly one ordinary BowBegin");
 frame(r);frame(r);ck(r.begins==1&&r.i.manualNockAck19c4r4.serial==0,"same published native ACK cannot reissue edge");
 r.pull();r.i.grip[0]=0;frame(r);ck(r.fires==1&&!r.o.bowHeld,"actual pull and later physical release still required for one shot");
 r.i.grip[0]=1;frame(r);ck(r.begins==1&&r.fires==1,"old acknowledgment cannot rearm after a shot");
 Rig duplicate;empty(duplicate);q=receipt(duplicate);g_manual_bow_ready596.publish(q);
 ManualNockInput19c4r4(duplicate.i,duplicate.p[1],duplicate.s.sample);
 ck(!duplicate.i.manualNockAck19c4r4.serial&&g_manual_nock_claim19c4r4.load()==0,"cached sample cannot consume newly arriving native nock ACK");
 frame(duplicate);ck(duplicate.o.bowHeld&&duplicate.begins==1,"next genuinely new sample consumes the pending ACK once");
 Rig delay;empty(delay);q=receipt(delay);g_manual_bow_ready596.publish(q);delay.next();delay.next();frame(delay);
 ck(delay.o.bowHeld&&delay.begins==1,"fresh ACK survives two unchanged continuous frames, without refreshing original tick");
 Rig far;empty(far);q=receipt(far);g_manual_bow_ready596.publish(q);far.i.hands[0].p.y-=.30;frame(far);far.i.hands[0].p.y+=.30;frame(far);
 ck(!far.o.bowHeld&&far.begins==0,"failed near-string acquisition burns edge; return does not use stale acknowledgment");
 for(unsigned reason=0;reason<6;++reason){
  Rig x;empty(x);q=receipt(x);g_manual_bow_ready596.publish(q);
  if(reason==0)x.i.allowed=false;if(reason==1)x.i.tracked[0]=false;if(reason==2)x.i.intentTracked[0]=false;
  if(reason==3)x.i.bowGeometryReady175=false;if(reason==4)x.i.bowActions=false;if(reason==5)x.i.intentTracked[1]=false;
  frame(x);x.i.allowed=x.i.tracked[0]=x.i.intentTracked[0]=x.i.intentTracked[1]=x.i.bowGeometryReady175=x.i.bowActions=true;
  frame(x);frame(x);ck(!x.o.bowHeld&&x.begins==0&&x.fires==0,"tracking/menu/geometry/context failure consumes handoff without granting recovery draw");
 }
 for(unsigned reason=0;reason<15;++reason){
  Rig x;empty(x);q=receipt(x);
  if(reason==0)q.key++;if(reason==1)q.generation++;if(reason==2)q.request++;if(reason==3)q.epoch++;
  if(reason==4)q.nockAck.key++;if(reason==5)q.nockAck.generation++;if(reason==6)q.nockAck.tick=x.i.tick-81;
  if(reason==7)q.nockAck.sample=x.i.sample+2;if(reason==8)q.loaded=false;
  if(reason==9)q.nockAck.releaseToNock56=false;if(reason==10)q.nockAck.handEntity56=0;
  if(reason==11)q.nockAck.handIdentity56=0;if(reason==12)q.nockAck.releaseSample56=x.i.sample+2;
  if(reason==13)q.nockAck.releaseTick56=x.i.tick-81;if(reason==14)q.nockAck.releaseTick56=q.nockAck.tick+1;
  g_manual_bow_ready596.publish(q);frame(x);ck(!x.o.bowHeld&&x.begins==0&&x.fires==0,"wrong/expired/future/unattached native owner cannot hand off");
 }
 Rig initial;empty(initial);q=receipt(initial);q.nockAck={};g_manual_bow_ready596.publish(q);frame(initial);
 ck(!initial.o.bowHeld&&initial.begins==0,"already loaded entry without manual nock cycle does not bypass neutral");
 Rig off;empty(off);q=receipt(off);g_manual_bow_ready596.publish(q);manual=false;frame(off);
 ck(!off.o.bowHeld&&off.i.manualNockAck19c4r4.serial==0,"F11 off ignores manual proof");
 off.i.grip[0]=0;frame(off);off.i.grip[0]=1;frame(off);ck(off.o.bowHeld&&off.begins==1,"default normal Grip draw still uses ordinary neutral press");
 std::printf("PASS %d actual manual ACK producer and physical-kernel handoff checks\n",checks);
}
