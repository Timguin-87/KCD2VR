#include "subsystems/masterstrike_combos/ms_reaction_owner_d62c.h"
#include <map>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cassert>
namespace np=kcd2vr::native_pair_d72;
namespace ms=kcd2vr::ms_reaction_d62c;
struct Memory {
 std::map<std::uintptr_t,unsigned char> bytes;
 template<class T>void put(std::uintptr_t a,T v){const auto* b=reinterpret_cast<const unsigned char*>(&v);for(unsigned i=0;i<sizeof(v);++i)bytes[a+i]=b[i];}
 template<class T>bool read(std::uintptr_t a,T& v){auto* b=reinterpret_cast<unsigned char*>(&v);for(unsigned i=0;i<sizeof(v);++i){auto p=bytes.find(a+i);if(p==bytes.end())return false;b[i]=p->second;}return true;}
};
constexpr std::uintptr_t B=0x180000000ull,A=0x100000,V=0x200000,AF=0x300000,VF=0x400000,AA=0x500000,VA=0x600000,AM=0x700000,VM=0x800000;
Memory fixture(bool combo,std::uintptr_t shift=0,unsigned firstId=11){Memory m;const auto& l=combo?np::combo:np::master;
 const auto A=::A+shift,V=::V+shift,AF=::AF+shift,VF=::VF+shift,AA=::AA+shift,VA=::VA+shift,AM=::AM+shift,VM=::VM+shift;
 m.put(A,B+l.attackVt);m.put(V,B+l.hitVt);m.put(A+l.attackPeer,V);m.put(V+l.hitPeer,A);
 m.put(A+l.attackActor,AA);m.put(V+0x68,VA);m.put(A+l.attackFragment,AF);m.put(V+0x78,VF);
 for(unsigned i=0;i<2;++i){const auto a=i?V:A,f=i?VF:AF,actor=i?VA:AA,manager=i?VM:AM;
  m.put(actor,B+(i?0x3E3FAC8:0x3A80610));m.put(actor+0x200,manager);m.put(a+0x40,manager);
  m.put(manager+0x38,manager+0x1000);m.put(manager+0x1000,a);m.put(f,B+np::fragmentVt);
  m.put(f+0x98,(unsigned char)0);m.put(f+0x158,f+0x1000);
  m.put(f+0x1000,B+0x3E3FA18);m.put(f+0x1040,f+0x2000);m.put(f+0x1048,f);m.put(f+0x2020,a);
  m.put(actor+0x1F8,actor+0x1000);m.put(actor+0x1000+0x30,std::uint32_t(firstId+i));m.put(f+0x9C,std::uint32_t(firstId+i));
 }return m;
}
int main(){unsigned checks=0;
 auto check=[&](bool v){++checks;assert(v);};
 for(bool c:{false,true}){const auto& l=c?np::combo:np::master;auto m=fixture(c);
  auto read=[&](std::uintptr_t a,auto& v){return m.read(a,v);};np::Pair x{},y{};
  check(np::read(read,B,AF,c,true,x));check(np::read(read,B,VF,c,false,y));check(np::same(x,y));
  check(x.attacker==AA&&x.victim==VA&&x.attackAction==A&&x.hitAction==V&&x.attackEntity==11&&x.hitEntity==12);
  check(!np::read(read,B,AF,!c,true,y));check(!np::read(read,B,VF,c,true,y));
  const std::vector<std::uintptr_t> identity{A,V,A+l.attackPeer,V+l.hitPeer,A+l.attackActor,V+0x68,A+l.attackFragment,V+0x78,
   AA,VA,AA+0x200,VA+0x200,A+0x40,V+0x40,AM+0x38,VM+0x38,AM+0x1000,VM+0x1000,AF,VF,AF+0x158,VF+0x158,
   AF+0x1000,VF+0x1000,AF+0x1040,VF+0x1040,AF+0x1048,VF+0x1048,AF+0x2020,VF+0x2020,
   AA+0x1F8,VA+0x1F8,AA+0x1030,VA+0x1030,AF+0x9C,VF+0x9C};
  for(auto at:identity){m=fixture(c);m.bytes.erase(at);check(!np::read(read,B,AF,c,true,y));}
  for(auto at:{A+l.attackPeer,V+l.hitPeer,AM+0x1000,VM+0x1000,AF+0x1000,VF+0x1000,
      AF+0x1040,VF+0x1040,AF+0x1048,VF+0x1048,AF+0x2020,VF+0x2020}){
   m=fixture(c);m.put(at,std::uintptr_t(0xDEAD000));check(!np::read(read,B,AF,c,true,y));}
  for(auto f:{AF,VF}){m=fixture(c);m.put(f+0x98,(unsigned char)1);check(!np::read(read,B,AF,c,true,y));}
  m=fixture(c);m.put(VF+0x9C,std::uint32_t(11));check(!np::read(read,B,AF,c,true,y));
  // Native reciprocal teardown in the middle of the sample must revoke it.
  m=fixture(c);unsigned reciprocalReads=0;auto race=[&](std::uintptr_t at,auto& value){
   if(at==A+l.attackPeer&&++reciprocalReads==3)m.put(at,std::uintptr_t(0));return m.read(at,value);};
  check(!np::read(race,B,AF,c,true,y));
  // Two contemporaneous valid pairs are separate authority, even with equal
  // clip families and queue times. Crossed links fail; a new valid pair still
  // cannot satisfy the old occurrence's cached proof.
  constexpr std::uintptr_t delta=0x1000000;
  m=fixture(c);const auto second=fixture(c,delta,21);m.bytes.insert(second.bytes.begin(),second.bytes.end());
  np::Pair one{},two{};check(np::read(read,B,AF,c,true,one));check(np::read(read,B,AF+delta,c,true,two));
  check(!np::same(one,two));m.put(A+l.attackPeer,V+delta);check(!np::read(read,B,AF,c,true,y));
  m.put(V+delta+l.hitPeer,A);check(np::read(read,B,AF,c,true,y));check(!np::same(one,y));
  ms::Owner aa{},vv{};aa.serial=1;vv.serial=2;aa.pair=2;vv.pair=1;aa.controller=1;vv.controller=2;
  aa.action=one.attackFragment;vv.action=one.hitFragment;aa.entity=one.attackEntity;vv.entity=one.hitEntity;aa.victim=vv.entity;
  aa.kind=c?ms::Kind::ComboAttack:ms::Kind::Master;vv.kind=c?ms::Kind::ComboHit:ms::Kind::Hit;
  aa.henry=true;aa.instantD72=true;aa.state=vv.state=ms::State::Playing;
  aa.nativeProofD72=vv.nativeProofD72=true;aa.nativeD72=vv.nativeD72=one;
  check(ms::ownsPairD72(aa,vv,one));check(!ms::ownsPairD72(aa,vv,two));check(!ms::ownsPairD72(aa,vv,y));
  auto alien=vv;alien.serial=3;alien.pair=4;alien.action=two.hitFragment;alien.entity=two.hitEntity;alien.nativeD72=two;
  check(!ms::ownsPairD72(aa,alien,two));check(!ms::stepInstantD72(aa,&alien,ms::ownsPairD72(aa,alien,two),true,100));
  check(aa.phase==ms::Phase::Idle&&!aa.leased&&!alien.leased);
  for(unsigned field=0;field<12;++field){auto changed=vv;
   switch(field){case 0:changed.pair=3;break;case 1:changed.serial=3;break;case 2:changed.controller=aa.controller;break;
    case 3:changed.action++;break;case 4:changed.entity++;break;case 5:changed.henry=true;break;
    case 6:changed.state=ms::State::Retired;break;case 7:changed.nativeProofD72=false;break;case 8:changed.nativeD72=two;break;
    case 9:changed.kind=ms::Kind::HitDeath;break;case 10:changed.nativeD72.hitManager++;break;case 11:changed.nativeD72.hitAction++;break;}
   check(!ms::ownsPairD72(aa,changed,one));
  }
  check(ms::stepInstantD72(aa,&vv,ms::ownsPairD72(aa,vv,one),true,101));
  check(aa.phase==ms::Phase::Done&&!aa.leased&&!vv.leased&&aa.writes==0&&vv.writes==0);
  check(std::strcmp(aa.endWhy,"PAIR_DAMAGE_CONTACT_UNPROVEN")==0);
  // Legacy lease restoration remains independently tested. The production
  // phase policy above grants none from pair identity alone.
  check(ms::lease(aa,1,64,64)==64&&ms::lease(vv,.5,64,64)==32);
  check(!alien.leased&&alien.writes==0&&alien.restores==0);
  aa.phase=ms::Phase::Accel;check(ms::stepInstantD72(aa,&alien,ms::ownsPairD72(aa,alien,two),true,102));
  check(ms::lease(aa,64,0,64)==1&&ms::lease(vv,32,0,64)==.5);
  check(!alien.leased&&alien.writes==0&&alien.restores==0);
 }
 ms::Owner a{},v{};a.henry=true;a.kind=ms::Kind::ComboAttack;a.instantD72=true;a.serial=1;a.pair=2;
 v.kind=ms::Kind::ComboHit;v.serial=2;v.pair=1;
 check(!ms::stepInstantD72(a,&v,false,true,100));check(a.phase==ms::Phase::Idle);
 check(!ms::stepInstantD72(a,&v,true,false,100));check(ms::stepInstantD72(a,&v,true,true,110));
 check(a.phase==ms::Phase::Done&&!a.leased&&a.writes==0);kcd2vr::instant_d72::combat=false;
 check(!ms::stepInstantD72(a,&v,true,true,120)); // admitted occurrence retains policy
 check(ms::lease(a,1.f,64.f,64.f)==64.f);check(a.saved==1&&a.leased);
 check(!std::isfinite(ms::lease(a,64.f,64.f,64.f)));
 a.phase=ms::Phase::Accel;check(ms::stepInstantD72(a,&v,false,true,130));check(a.phase==ms::Phase::Done);
 check(ms::lease(a,64.f,0.f,64.f)==1.f);check(!a.leased);
 ms::Owner foreign{};check(ms::lease(foreign,1,64,64)==64);check(!std::isfinite(ms::lease(foreign,2,0,64)));check(!foreign.leased);
 ms::Owner cap=a;cap.phase=ms::Phase::Accel;cap.accelStart=10;check(ms::stepInstantD72(cap,&v,true,true,610));
 ms::Owner off{};off.kind=ms::Kind::Master;off.henry=true;check(!ms::stepInstantD72(off,&v,true,true,100));
 std::printf("PASS native reciprocal pair D72: %u checks; native reads mocked; retail ABI/live game NOT_RUN\n",checks);
 return 0;
}
