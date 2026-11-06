#pragma once
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace kcd2vr::alchemy_resource_scope_d67r1 {
// Retail kcd_male_controllerdefs.xml: FullBody is layer 0/count 3. This
// profile admits only that root with ONE resource CAF and two EMPTY slots.
// It is not a generic multi-layer or event-seek capability. Read is bounded,
// value-copy only; the native adapter retains all existing CAF/event checks.
struct Key {
 std::uintptr_t base=0,controller=0,root=0,child=0,instance=0,character=0,skeleton=0;
 std::uint32_t token=0;int animation=-1;
};
struct Rates {float saved=1.f,last=1.f,previous=1.f;bool otherWriter=false;};
struct Proof {
 std::uintptr_t scopes=0,context=0,character=0,skeleton=0,child=0,instance=0;
 std::array<std::uintptr_t,3> fifo{};
 std::uint32_t token=0,ownedScopes=0,scopeCount=0;int animation=-1;
};
enum class Reason {Ok,Read,Pins,Identity,Profile,ScopeOwner,Overlap,ExtraCaf,RootCaf,Rate,Pending,Migration};
inline const char* name(Reason r) noexcept {
 switch(r){
  case Reason::Ok:return "ROOT3_QUALIFIED_D67R1";
  case Reason::Read:return "ROOT3_READ_D67R1";
  case Reason::Pins:return "ROOT3_NATIVE_PINS_D67R1";
  case Reason::Identity:return "ROOT3_IDENTITY_D67R1";
  case Reason::Profile:return "ROOT3_PROFILE_D67R1";
  case Reason::ScopeOwner:return "ROOT3_SCOPE_OWNER_D67R1";
  case Reason::Overlap:return "ROOT3_OVERLAPPING_WRITER_D67R1";
  case Reason::ExtraCaf:return "ROOT3_EXTRA_CAF_D67R1";
  case Reason::RootCaf:return "ROOT3_ROOT_CAF_D67R1";
  case Reason::Rate:return "ROOT3_RATE_WRITER_D67R1";
  case Reason::Pending:return "ROOT3_PENDING_TIME_D67R1";
  case Reason::Migration:return "ROOT3_IDENTITY_MIGRATED_D67R1";
 }return "ROOT3_UNKNOWN_D67R1";
}
struct Result {Reason reason=Reason::Read;Proof proof{};explicit operator bool() const noexcept{return reason==Reason::Ok;}};
inline bool same(const Proof& a,const Proof& b) noexcept {
 return a.scopes==b.scopes&&a.context==b.context&&a.character==b.character&&a.skeleton==b.skeleton&&
  a.child==b.child&&a.instance==b.instance&&a.fifo==b.fifo&&a.token==b.token&&a.ownedScopes==b.ownedScopes&&
  a.scopeCount==b.scopeCount&&a.animation==b.animation;
}
template<class Read> Result inspect(Read&& read,const Key& k,const Rates& rates,const Proof* previous=nullptr) noexcept {
 Result out;auto fail=[&](Reason why){out.reason=why;return out;};
 if(!k.base||!k.controller||!k.root||!k.child||!k.instance||!k.character||!k.skeleton||k.animation<0)
  return fail(Reason::Identity);
 if(rates.otherWriter||rates.saved!=1.f)return fail(Reason::Rate);
 const auto ours=[&](float v){return std::isfinite(v)&&v>=0.f&&v<=8.f&&
  (v==rates.saved||v==rates.last||v==rates.previous);};
 std::uintptr_t v=0,context=0,scopes=0,installed=0,back=0;unsigned count=0,layer=99,layers=0,token=0;int status=0;
 float rate=-1.f,pending=0;
 if(!read(k.controller+0x34,count)||!read(k.controller+0x38,scopes)||!read(k.root,v)||
    !read(k.root+0x20,context)||!read(k.root+0x30,layer)||!read(k.root+0x34,layers))return out;
 if(count==0||count>24||!scopes||k.root!=scopes||v!=k.base+0x3AEA100||layer!=0||layers!=3)
  return fail(Reason::Profile);
 if(!context||!read(context+0x10,back)||back!=k.character||k.skeleton!=k.character+0x110||
    !read(k.skeleton+0x4E8,back)||back!=k.character||!read(k.skeleton,v)||v!=k.base+0x3A711E0||
    !read(k.child,v)||v!=k.base+0x3A47948||!read(k.child+0x30,back)||back!=k.root||
    !read(k.child+0x268,back)||back!=k.instance||!read(k.child+0x54,token)||token!=k.token||
    !read(k.child+0x28,status)||status!=2)return fail(Reason::Identity);
 if(!read(k.child+0x5C,rate)||!ours(rate))return fail(Reason::Rate);
 out.proof.scopes=scopes;out.proof.context=context;out.proof.character=k.character;
 out.proof.skeleton=k.skeleton;out.proof.child=k.child;out.proof.instance=k.instance;out.proof.token=k.token;
 out.proof.scopeCount=count;out.proof.animation=k.animation;
 unsigned rootOwners=0;
 for(unsigned i=0;i<count;++i){
  const auto s=scopes+std::uintptr_t(i)*0x168;std::uintptr_t sc=0,ch=0;
  if(!read(s+0x20,sc)||!read(s+0x148,installed)||!read(s+0x30,layer)||!read(s+0x34,layers))return fail(Reason::Read);
  if(sc&&!read(sc+0x10,ch))return fail(Reason::Read);
  if(installed==k.child){
   // The captured water action also owns LookPose/Looking (indices 14/15).
   // A slave character or any other scope may not inherit this clock claim.
   const bool expected=i==0||(i==14&&layer==15&&layers==1)||(i==15&&layer==15&&layers==0);
   if(ch!=k.character||!expected||!read(s,v)||v!=k.base+0x3AEA100)return fail(Reason::ScopeOwner);
   if(!read(s+0x58,rate)||!ours(rate))return fail(Reason::Rate);
   if(!read(s+0x60,pending)||pending!=0.f||!read(s+0x64,pending)||pending!=0.f)return fail(Reason::Pending);
   out.proof.ownedScopes|=std::uint32_t(1u<<i);
  }
  if(ch!=k.character||!layers)continue;
  if(layer>15||layers>16-layer)return fail(Reason::Profile);
  if(layer<3){if(i!=0||installed!=k.child)return fail(Reason::Overlap);++rootOwners;}
 }
 if(rootOwners!=1||!(out.proof.ownedScopes&1u))return fail(Reason::ScopeOwner);
 for(unsigned i=0;i<3;++i){
  auto& entries=out.proof.fifo[i];unsigned n=0;
  // Native layer constructor ABB5A4 stores a nonnull shared empty array at +8;
  // GetNumAnims83A8B0 reads [entries-4] unconditionally. Null is not empty.
  if(!read(k.skeleton+0x48+std::uintptr_t(i)*0x40,entries)||!entries||!read(entries-4,n))return fail(Reason::Read);
  if(n!=(i?0u:1u))return fail(i?Reason::ExtraCaf:Reason::RootCaf);
  // Empty slots have no CAF to advance. Their dormant rate values need not
  // match a live clip; exclusive scope ownership and continued emptiness do.
  if(!i&&(!read(k.skeleton+0x50,rate)||!ours(rate)))return fail(Reason::Rate);
 }
 const auto entries=out.proof.fifo[0];unsigned animation=~0u,flags=0;unsigned char segment=255,prior=255;
 if(!read(entries,v)||v!=k.base+0x3A70A60||!read(entries+8,back)||back||
    !read(entries+0x10,animation)||animation!=unsigned(k.animation)||
    !read(entries+0x14,prior)||prior!=0||!read(entries+0x16,segment)||segment!=0||
    !read(entries+0x4C,flags)||(flags&1u))return fail(Reason::RootCaf);
 if(previous&&!same(*previous,out.proof))return fail(Reason::Migration);
 out.reason=Reason::Ok;return out;
}
}
