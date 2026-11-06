#include "core/transfer_clip_d65.h"
#include "subsystems/masterstrike_combos/ms_reaction_owner_d62c.h"
#include <cstdio>
#include <limits>
namespace tr=kcd2vr::transfer_clip_d65;
namespace ms=kcd2vr::ms_reaction_d62c;
static unsigned checks=0,failed=0;
static void ck(bool v,const char* name){++checks;if(!v){++failed;std::printf("FAIL %s\n",name);}}
static tr::Key key(){tr::Key k{};k.controller=1;k.character=2;k.skeleton=3;k.instance=4;k.parent=5;k.child=6;k.scope=7;k.lifetime=8;k.mode=9;k.token=10;k.layer=0;return k;}
int main(){
 tr::Admission a{true,true,true,true,true,false};
 ck(tr::eligible(a,3),"qualified transfer");ck(!tr::eligible(a,1),"default off");
 a.fullVr=false;ck(!tr::eligible(a,3),"Native/Semi excluded");a.fullVr=true;
 a.exactParent=false;ck(!tr::eligible(a,3),"nontransfer stage excluded");a.exactParent=true;
 a.currentInput=false;ck(!tr::eligible(a,3),"tracking/control loss");a.currentInput=true;
 a.root=false;ck(!tr::eligible(a,3),"exiting child");a.root=true;
 a.singleClip=false;ck(!tr::eligible(a,3),"shared layer");a.singleClip=true;
 a.foreign=true;ck(!tr::eligible(a,3),"existing owner");
 ck(tr::rate(8)==8&&tr::rate(9)==8&&tr::rate(-3)==1&&tr::rate(std::numeric_limits<float>::quiet_NaN())==1,"rate clamps");
 tr::Ledger l;auto k=key();ck(l.begin(k,1,3),"begin uncertain before write");
 ck(l.active&&l.uncertain,"cleanup already pending before setter");
 ck(!l.begin(k,1,2),"no overlapping lease");
 ck(l.restore(false,true,3)==tr::Restore::Pending&&l.active,"unknown retains lease");
 ck(l.restore(true,true,-1)==tr::Restore::Pending&&l.active,"unreadable layer retains lease");
 ck(l.restore(true,true,std::numeric_limits<float>::quiet_NaN())==tr::Restore::Pending,"NaN retains lease");
 ck(l.restore(true,true,1)==tr::Restore::AlreadyNative,"failed setter did not change native");
 ck(l.restore(true,true,3)==tr::Restore::GiveBack,"uncertain successful setter cleanup");
 l.confirmed();ck(!l.uncertain,"confirmed write");
 ck(l.restore(true,true,3)==tr::Restore::GiveBack&&l.active,"failed restore leaves pending");
 ck(l.restore(true,true,2)==tr::Restore::Foreign,"foreign writer wins");
 l.settled(true);ck(!l.active&&!l.begin(k,1,3),"same token permanent standdown");
 auto changed=k;changed.parent=51;changed.scope=71;changed.mode=91;
 ck(!l.begin(changed,1,3),"parent stage switch does not regain same child token");
 changed.token++;ck(l.begin(changed,1,2),"new token allowed");
 ck(l.restore(true,false,-1)==tr::Restore::Gone,"skeleton replacement does not touch old memory");
 l.settled();ck(!l.active,"proven replacement cleanup");
 ck(!l.begin(k,2,3),"foreign initial speed excluded");
 changed=key();changed.token+=2;ck(l.begin(changed,1,2),"new action cycle");l.confirmed();
 ck(l.restore(true,true,1)==tr::Restore::Foreign,"native wrote speed1 after confirmed lease");
 l.settled(true);ck(!l.begin(changed,1,2),"native restored speed does not get overridden again");
 tr::PickupJoin j;tr::PickupKey p{1,1,50,60,2};
 ck(!j.accept(p,false,true),"retired or released Use not admitted");
 ck(!j.accept(p,true,false),"preexisting native Picking not admitted");
 ck(j.accept(p,true,true),"new current Use joins changed root");
 ck(j.accept(p,true,true),"same request same action can continue");
 auto q=p;q.action++;ck(!j.accept(q,true,true),"one Use cannot accelerate second action");
 q=p;q.token++;ck(!j.accept(q,true,true),"one Use cannot reacquire recycled token");
 q=p;q.generation++;ck(!j.accept(q,true,true),"request generation mismatch");
 q=p;q.serial++;q.generation++;q.action++;ck(j.accept(q,true,true),"new request can join next action");
 ck(!j.accept(p,true,true),"older receipt cannot roll join backward");
 ms::Table<> t;auto* v=t.offer(10,20,30,1,0xD62EA2F9,-1,-1,nullptr,false,100);
 auto* h=t.offer(11,21,31,2,0xBF91494F,-1,-1,nullptr,true,110);
 ck(v&&h&&v->candidateD65==h->serial&&h->candidateD65==v->serial,"victim-first candidate visible");
 ck(!v->pair&&!h->pair,"candidate never native pair");
 h->state=ms::State::Playing;h->playing=110;v->state=ms::State::Playing;
 ms::step(*h,v,true,110,0,400);ck(h->phase!=ms::Phase::Accel,"queue plus contact not authority");
 auto* late=t.offer(12,22,32,3,0xD62EA2F9,-1,-1,nullptr,false,115);
 ck(late&&h->candidatesD65==2&&!h->candidateD65&&!v->candidateD65&&!late->candidateD65,"late ambiguity invalidates earlier mutual candidate");
 t.observe(*late,ms::Seen::Absent,116);ck(h->candidateD65==v->serial,"live graph retires absent candidate");
 t={};h=t.offer(11,21,31,2,0xBF91494F,-1,-1,nullptr,true,100);v=t.offer(10,20,30,1,0xD62EA2F9,-1,-1,nullptr,false,110);
 ck(h&&v&&h->candidateD65==v->serial,"attacker-first candidate visible");
 t={};h=t.offer(11,21,31,2,0x17106D4F,-1,-1,nullptr,true,100);v=t.offer(10,20,30,1,0xD62EA2F9,-1,-1,nullptr,false,110);
 ck(h&&v&&!h->candidateD65,"paired death kind mismatch");
 t={};h=t.offer(11,21,31,2,0xBF91494F,-1,-1,nullptr,true,100);v=t.offer(10,20,30,1,0xD62EA2F9,-1,-1,nullptr,false,141);
 ck(h&&v&&!h->candidateD65,"outside observation window");
 std::printf("D65 transfer/request/paired-candidate checks %u/%u\n",checks-failed,checks);return failed?1:0;
}
