#pragma once
// D72 exact-retail semantic pair. This is a reciprocal native relationship,
// not a correlation between nearby Queue calls. See tools/audit_combat_pair_d72.py.
#include <cstdint>
namespace kcd2vr::native_pair_d72 {
struct Layout {std::uintptr_t attackVt,hitVt;unsigned attackActor,attackFragment,attackPeer,hitPeer;};
inline constexpr Layout combo{0x3A8B608,0x3A993D8,0x70,0x80,0xA0,0xC0};
inline constexpr Layout master{0x3B933B0,0x3E40740,0x88,0x98,0xF0,0xA0};
inline constexpr std::uintptr_t fragmentVt=0x3A60C88;
// 90CEC8 -> 90CF5C -> 90D04C stores a wrapper around the original member
// callback. Its +40 points to that callback and +48 points back to the fragment.
// The semantic action is at INNER callback +20, never at wrapper +20.
inline constexpr std::uintptr_t completionWrapperVtD84=0x3E3FA18;
enum class ReadGateD84 : unsigned char {None,Pins,Token,Fragment,Callback,Semantic,Reciprocal,Attacker,Victim,Recheck,Proven};
inline const char* name(ReadGateD84 g) noexcept {
 switch(g){case ReadGateD84::Pins:return "EXACT_RETAIL_PINS";case ReadGateD84::Token:return "FRAGMENT_TOKEN";
  case ReadGateD84::Fragment:return "FRAGMENT";case ReadGateD84::Callback:return "COMPLETION_WRAPPER";
  case ReadGateD84::Semantic:return "SEMANTIC_ACTION";case ReadGateD84::Reciprocal:return "RECIPROCAL_LINK";
  case ReadGateD84::Attacker:return "ATTACKER_ENDPOINT";case ReadGateD84::Victim:return "VICTIM_ENDPOINT";
  case ReadGateD84::Recheck:return "CURRENT_RECHECK";case ReadGateD84::Proven:return "PROVEN";default:return "NOT_READ";}
}
struct ReadTraceD84 {ReadGateD84 gate=ReadGateD84::None;std::uintptr_t at=0,observed=0,expected=0;bool readable=false,nonzero=false;};
struct Pair {
 std::uintptr_t attacker=0,victim=0,attackAction=0,hitAction=0,attackFragment=0,hitFragment=0;
 std::uintptr_t attackManager=0,hitManager=0;
 std::uint32_t attackEntity=0,hitEntity=0;
 bool combo=false;
};
inline bool same(const Pair& a,const Pair& b) noexcept {
 return a.attackAction&&a.attackAction==b.attackAction&&a.hitAction==b.hitAction&&
  a.attackFragment==b.attackFragment&&a.hitFragment==b.hitFragment&&
  a.attacker==b.attacker&&a.victim==b.victim&&a.attackManager==b.attackManager&&
  a.hitManager==b.hitManager&&a.attackEntity==b.attackEntity&&a.hitEntity==b.hitEntity&&a.combo==b.combo;
}
// rd(address,T&) must copy only, return false on unreadable memory, and never
// invoke a game service. Native manager current-main membership is mandatory.
template<class Read> bool read(Read&& rd,std::uintptr_t base,std::uintptr_t fragment,bool isCombo,bool attackSide,Pair& out,ReadTraceD84* trace=nullptr) noexcept {
 out={};if(trace)*trace={};if(!base||!fragment)return false;const auto& l=isCombo?combo:master;
 const auto stage=[&](ReadGateD84 gate){if(trace){*trace={};trace->gate=gate;}};
 const auto eq=[&](std::uintptr_t a,auto expected){decltype(expected) got{};const bool ok=rd(a,got);
  if(trace){trace->at=a;trace->observed=got;trace->expected=expected;trace->readable=ok;trace->nonzero=false;}return ok&&got==expected;};
 const auto ptr=[&](std::uintptr_t a,std::uintptr_t& v){v=0;const bool ok=rd(a,v);
  if(trace){trace->at=a;trace->observed=v;trace->expected=0;trace->readable=ok;trace->nonzero=true;}return ok&&v;};
 const auto callback=[&](std::uintptr_t f,std::uintptr_t& action){std::uintptr_t wrapper=0,inner=0;
  return ptr(f+0x158,wrapper)&&eq(wrapper,base+completionWrapperVtD84)&&eq(wrapper+0x48,f)&&
   ptr(wrapper+0x40,inner)&&ptr(inner+0x20,action);};
 std::uintptr_t own=0,peer=0;
 stage(ReadGateD84::Fragment);if(!eq(fragment,base+fragmentVt))return false;
 stage(ReadGateD84::Callback);if(!callback(fragment,own))return false;
 stage(ReadGateD84::Semantic);
 if(!eq(own,base+(attackSide?l.attackVt:l.hitVt))||
    !eq(own+(attackSide?l.attackFragment:0x78),fragment)||
    !ptr(own+(attackSide?l.attackPeer:l.hitPeer),peer))return false;
 Pair p{};p.combo=isCombo;p.attackAction=attackSide?own:peer;p.hitAction=attackSide?peer:own;
 stage(ReadGateD84::Reciprocal);
 if(p.attackAction==p.hitAction||!eq(p.attackAction,base+l.attackVt)||!eq(p.hitAction,base+l.hitVt)||
    !eq(p.attackAction+l.attackPeer,p.hitAction)||!eq(p.hitAction+l.hitPeer,p.attackAction)||
    !ptr(p.attackAction+l.attackActor,p.attacker)||!ptr(p.hitAction+0x68,p.victim)||p.attacker==p.victim||
    !ptr(p.attackAction+l.attackFragment,p.attackFragment)||!ptr(p.hitAction+0x78,p.hitFragment)||p.attackFragment==p.hitFragment)return false;
 const auto endpoint=[&](std::uintptr_t action,std::uintptr_t f,std::uintptr_t actor,std::uintptr_t& manager,std::uint32_t& id){
  std::uintptr_t vt=0,main=0,entityOwner=0,callbackAction=0;
  if(!ptr(actor,vt)||(vt!=base+0x3E3FAC8&&vt!=base+0x3A80610)||
     !ptr(actor+0x200,manager)||!eq(action+0x40,manager)||!ptr(manager+0x38,main)||!eq(main,action)||
     !eq(f,base+fragmentVt)||!eq(f+0x98,static_cast<unsigned char>(0))||!callback(f,callbackAction))return false;
  if(callbackAction!=action){if(trace){trace->expected=action;trace->nonzero=false;}return false;}
  if(!ptr(actor+0x1F8,entityOwner)||!rd(entityOwner+0x30,id)||!id||!eq(f+0x9C,id))return false;
  return true;
 };
 stage(ReadGateD84::Attacker);if(!endpoint(p.attackAction,p.attackFragment,p.attacker,p.attackManager,p.attackEntity))return false;
 stage(ReadGateD84::Victim);if(!endpoint(p.hitAction,p.hitFragment,p.victim,p.hitManager,p.hitEntity)||p.attackManager==p.hitManager||p.attackEntity==p.hitEntity)return false;
 // Recheck the relationship after walking both endpoints. Teardown clears the
 // reciprocal native link before releasing either reference.
 stage(ReadGateD84::Recheck);
 if(!eq(p.attackAction+l.attackPeer,p.hitAction)||!eq(p.hitAction+l.hitPeer,p.attackAction)||
    !eq(p.attackAction+l.attackFragment,p.attackFragment)||!eq(p.hitAction+0x78,p.hitFragment))return false;
 out=p;stage(ReadGateD84::Proven);if(trace)trace->readable=true;return true;
}
}
