// D63 keeps this historical filename for the pure-policy gate. The explicit
// verified joins below model a FUTURE proven native contract; the actual runtime
// regression separately requires ZERO leases because that join is not known.
#include "../src/subsystems/masterstrike_combos/ms_reaction_owner_d62c.h"
#include "../src/subsystems/masterstrike_combos/paired_signals_d62c.h"
#include <cstdio>
#include <cmath>
#include <string>
using namespace kcd2vr::ms_reaction_d62c;
static int t=0,p=0;static void ck(bool b,const char* m){++t;p+=b;std::printf("%s %s\n",b?"PASS":"FAIL",m);}
constexpr std::uint32_t M=0xBF91494Fu,MD=0x17106D4Fu,H=0xD62EA2F9u,HD=0xF3D43F01u,CA=0xDEEB2887u,CH=0xFB64521Fu;
int main(){
 // queue observations alone never identify a native pair
 {Table<> tb;ck(!tb.offer(1,2,10,1,0x12345678u,3,0,nullptr,false,100),"other fragments are never tracked");}
 {Table<> tb;auto* m=tb.offer(100,1000,7,5,M,40,0,nullptr,true,1000);auto* h=tb.offer(200,2000,9,6,H,41,2,nullptr,false,1100);
  ck(m&&h&&!h->pair&&!m->pair&&!m->victim,"matching fragment proximity is not native pair evidence");
  ck(tb.liveReaction(9)==h,"live reaction by victim entity");}
 {Table<> tb;auto* m=tb.offer(100,1000,7,5,CA,40,0,nullptr,true,1000);auto* h=tb.offer(200,2000,9,6,H,41,2,nullptr,false,1100);
  ck(!h->pair&&!m->pair,"a combo attack never pairs with a masterstrike reaction");
  auto* ch=tb.offer(300,3000,9,7,CH,41,2,nullptr,false,1150);ck(!ch->pair&&!m->pair,"matching combo proximity still grants no pair");}
 {Table<> tb;auto* m=tb.offer(100,1000,7,5,MD,40,0,nullptr,true,1000);auto* h=tb.offer(200,2000,9,6,H,41,2,nullptr,false,1100);
  ck(!h->pair&&!m->pair,"survive reaction never pairs with a death master");auto* hd=tb.offer(300,3000,9,7,HD,42,0,nullptr,false,1200);ck(!hd->pair&&!m->pair,"matching death proximity still grants no pair");}
 // timing
 auto pair=[](Table<>& tb){auto* m=tb.offer(100,1000,7,5,M,40,0,nullptr,true,1000);auto* h=tb.offer(200,2000,9,6,H,41,2,nullptr,false,1010);
   tb.observe(*m,Seen::Root,1020);tb.observe(*h,Seen::Root,1020);m->pair=h->serial;h->pair=m->serial;m->victim=h->entity;return std::pair{m,h};};
 {Table<> tb;auto [m,h]=pair(tb);
  ck(!step(*m,h,true,0,0,1050,true)&&m->phase==Phase::Idle,"no contact yet, playing 30 ms: wait");
  ck(step(*m,h,true,990,0,1060,true)&&m->phase==Phase::Accel&&m->contactTick==990,"blade contact 10 ms before queue starts the fast part");
  ck(wanted(*m,m,4.f)==4.f&&wanted(*h,m,4.f)==4.f,"both actions of the pair want the same speed");
  ck(step(*m,h,true,990,1200,1210,true)&&m->phase==Phase::Done&&std::string(m->endWhy)=="NATIVE_HIT_APPLIED"&&m->hitTick==1200,"applied native hit ends it");
  ck(wanted(*m,m,4.f)==0.f&&wanted(*h,m,4.f)==0.f,"after the hit both get their own speed back");}
 {Table<> tb;auto [m,h]=pair(tb);step(*m,h,true,0,0,1100,true);ck(m->phase==Phase::Idle,"no contact, playing 80 ms: still waiting");
  step(*m,h,true,0,0,1230,true);ck(m->phase==Phase::Accel,"no contact: start 200 ms after the attacker plays");}
 {Table<> tb;auto [m,h]=pair(tb);step(*m,h,true,1000,0,1030,true);step(*m,h,true,1000,0,1750,true);ck(m->phase==Phase::Done&&std::string(m->endWhy)=="TIME_CAP","700 ms cap");}
 {Table<> tb;auto [m,h]=pair(tb);step(*m,h,true,1000,0,1030,true);step(*m,h,true,1000,900,1100,true);ck(m->phase==Phase::Accel,"an applied hit from BEFORE the fast part does not end it");}
 {Table<> tb;auto [m,h]=pair(tb);step(*m,h,true,1000,0,1030,true);tb.observe(*h,Seen::Absent,1040);step(*m,h,true,1000,0,1050,true);
  ck(m->phase==Phase::Done&&std::string(m->endWhy)=="VICTIM_NOT_PLAYING","victim reaction gone ends it");}
 {Table<> tb;auto [m,h]=pair(tb);step(*m,h,false,1000,0,1030,true);ck(m->phase==Phase::Idle,"switched off: never starts");
  step(*m,h,true,1000,0,1031,true);step(*m,h,false,1000,0,1040,true);ck(m->phase==Phase::Done&&std::string(m->endWhy)=="SWITCHED_OFF","switched off mid way ends it");}
 {Table<> tb;auto* m=tb.offer(100,1000,7,5,M,40,0,nullptr,false,1000);auto* h=tb.offer(200,2000,9,6,H,41,2,nullptr,true,1010);
  tb.observe(*m,Seen::Root,1020);tb.observe(*h,Seen::Root,1020);ck(!step(*m,h,true,1000,0,1300,true)&&m->phase==Phase::Idle&&wanted(*h,m,4.f)==0.f,"NPC masterstriking Henry: never retimed");}
 {Table<> tb;auto* m=tb.offer(100,1000,7,5,CA,40,0,nullptr,true,1000);tb.observe(*m,Seen::Root,1010);
  ck(step(*m,nullptr,true,1005,0,1020)&&m->phase==Phase::Done&&std::string(m->endWhy)=="PAIR_NATIVE_JOIN_UNVERIFIED","unpaired combo cannot accelerate even after blade contact");}
 // Missing/foreign proof rejects even a structurally plausible pair.
 {Table<> tb;auto [m,h]=pair(tb);step(*m,h,true,1000,0,1300);
  ck(m->phase==Phase::Done&&wanted(*m,m,4.f)==0.f,"omitted native proof forbids timeout/contact acceleration");}
 for(unsigned bad=0;bad<6;++bad){Table<> tb;auto [m,h]=pair(tb);
  if(bad==0)h->pair=0;if(bad==1)h->henry=true;if(bad==2)h->controller=m->controller;
  if(bad==3)h->entity=m->entity;if(bad==4)h->kind=Kind::ComboHit;if(bad==5)m->victim++;
  step(*m,h,true,1000,0,1300,true);ck(m->phase!=Phase::Accel,"changed actor/controller/reciprocal/type relationship cannot lease speed");}
 {Table<> tb;auto [m,h]=pair(tb);h->state=State::Queued;step(*m,h,true,1000,0,1300,true);
  ck(m->phase!=Phase::Accel,"a queued victim cannot permit attacker-only acceleration");}
 // lease
 {Owner r{};r.state=State::Playing;
  float w=lease(r,1.f,4.f);ck(w==4.f&&r.leased&&r.saved==1.f,"lease writes speed x4 and keeps the native 1.0");
  ck(std::isnan(lease(r,4.f,4.f)),"still ours: no write");
  w=lease(r,4.f,0.f);ck(w==1.f&&!r.leased&&r.restores==1,"restore gives exactly the saved speed back");
  ck(std::isnan(lease(r,1.f,4.f)),"never a second lease on the same row");}
 {Owner r{};r.state=State::Playing;lease(r,1.f,4.f);ck(std::isnan(lease(r,0.f,4.f))&&!r.leased&&r.refused==1,"someone else changed the speed: stand down");
  ck(std::isnan(lease(r,0.f,0.f)),"and never restore over it");}
 {Owner r{};r.state=State::Playing;ck(std::isnan(lease(r,0.f,4.f))&&!r.leased,"a held (speed 0) action is another owner's: never sped");}
 {Owner r{};r.state=State::Playing;ck(lease(r,1.5f,8.f)==8.f,"speed never above 8");}
 {Owner r{};ck(std::isnan(lease(r,NAN,4.f)),"unreadable speed: nothing");}
 // signals
 {namespace s=kcd2vr::paired_signals_d62c;s::live=false;s::contacts.note(9,500);s::NoteVrContact(9,1000);ck(s::contacts.latest(9,900,1100)==0,"signals are not recorded while no Henry pair is live");
  s::live=true;s::NoteVrContact(9,1000);ck(s::contacts.latest(9,900,1100)==0,"live gameplay pair does not turn on F3 observations");
  namespace capture=kcd2vr::diagnostics::capture168;capture::gate.select(capture::Domain::Combat,true,990);
  s::NoteVrContact(9,1000);s::NoteVrContact(5,1001);ck(s::contacts.latest(9,900,1100)==1000&&!s::contacts.latest(9,1001,1100),"newest contact per entity in the requested F3 window");
  capture::gate.select(capture::Domain::Combat,false,1002);s::NoteAppliedHit(7,9,1003);
  ck(s::applied.latest(9,1002,1100,7)==0,"native applied-hit observer stops with F3");}
 std::printf("ms_paired_timing_d62c=%d/%d\n",p,t);return p!=t;}
