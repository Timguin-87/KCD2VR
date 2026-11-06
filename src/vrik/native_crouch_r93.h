#pragma once
#include "core/snapshot_mailbox.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>

namespace kcd2vr::native_crouch_r93 {
// Exact retail C_Player::IsCrouched, also consumed by player_in_crouch audio.
// Unlike AI.GetStance, this reads the current native player state, not a request.
inline constexpr std::uintptr_t getterRva=0x72F628;
inline constexpr std::array<unsigned char,27> getterBytes{
 0x48,0x83,0xec,0x28,0x48,0x8b,0x89,0x90,0x09,0x00,0x00,0x48,0x8b,0x01,
 0xff,0x50,0x10,0x3c,0x01,0x0f,0x94,0xc0,0x48,0x83,0xc4,0x28,0xc3};
inline constexpr double heightMeters=.5; // BasicActor normal/combat1.6 - crouch1.1
struct Identity {
 std::uintptr_t base=0,player=0,entity=0,character=0;
 std::uint64_t body=0,tracking=0,control=0,lifetime=0,tick=0;
};
struct Receipt {Identity owner{};std::uint64_t generation=0;bool crouched=false;};
inline std::atomic<bool> enabled{false};
inline std::atomic<std::uint64_t> generation{1};
inline core::SnapshotMailbox<Receipt> receipt;
inline void invalidate() noexcept {generation.fetch_add(1,std::memory_order_acq_rel);}
inline bool sameOwner(const Identity& a,const Identity& b) noexcept {
 return a.base&&a.player&&a.entity&&a.character&&a.body&&a.tracking&&a.control&&a.lifetime&&
  a.base==b.base&&a.player==b.player&&a.entity==b.entity&&a.character==b.character&&
  a.body==b.body&&a.tracking==b.tracking&&a.control==b.control&&a.lifetime==b.lifetime;
}
inline bool current(const Receipt& r,const Identity& now,std::uint64_t token) noexcept {
 return token&&r.generation==token&&sameOwner(r.owner,now)&&r.owner.tick&&
  now.tick>=r.owner.tick&&now.tick-r.owner.tick<=80;
}
// Called only by the game-thread frame-end adapter after full retail Ready().
// The native function itself dispatches the current state; no private vslot is
// transplanted or called here. Before/after identity rejects state replacement.
template<class Read,class Call> bool query(const Identity& i,bool qualified,Read&& read,Call&& call,bool& crouched) noexcept {
 crouched=false;if(!qualified||!sameOwner(i,i)||!i.tick)return false;
 std::uintptr_t manager=0,state=0,stateVft=0,stateMethod=0;std::uint32_t id=0;
 const auto pins=[&]() noexcept {std::array<unsigned char,27> bytes{};std::uintptr_t v=0;
  return read(i.base+getterRva,bytes)&&bytes==getterBytes&&
   read(i.base+0x3AA4B98+0x758,v)&&v==i.base+getterRva;};
 const auto identity=[&](bool first) noexcept {std::uintptr_t v=0,m=0,st=0,sv=0,sm=0;std::uint32_t pid=0,eid=0;
  if(!read(i.player,v)||v!=i.base+0x3AA4B98||!read(i.player+0x38,v)||v!=i.entity||
   !read(i.player+0x30,pid)||!pid||!read(i.entity,v)||v!=i.base+0x4072C78||
   !read(i.entity+0x0c,eid)||eid!=pid||!read(i.player+0x9d0,m)||!m||
   !read(m,v)||v!=i.base+0x3EC9958||!read(m+0x38,v)||v!=i.player||
   !read(i.player+0x990,st)||!st||!read(st,sv)||!sv||!read(sv+0x10,sm)||!sm)return false;
  if(first){manager=m;state=st;stateVft=sv;stateMethod=sm;id=pid;return true;}
  return manager==m&&state==st&&stateVft==sv&&stateMethod==sm&&id==pid;
 };
 bool value=false;if(!pins()||!identity(true)||!call(i.base+getterRva,i.player,value)||!identity(false)||!pins())return false;
 crouched=value;return true;
}
// R98: crouch choice only. Body height has its own 3-state switch now
// (body_height_mode98); choosing a crouch mode no longer rewrites it.
template<class S> void select(S& s) noexcept {
 s.native_crouch_r93=!s.native_crouch_r93;
}
template<class S> const wchar_t* label(const S& s) noexcept {
 return s.body_height_mode98==1?L"NATIVE (NATIVE HEIGHT)":s.native_crouch_r93?L"NATIVE BUTTON":L"PHYSICAL";
}
// R98 body height: one value, three choices, cycled by one switch.
inline int next_body_height_r98(int mode) noexcept {return mode==0?1:mode==1?2:0;}
inline const wchar_t* body_height_label_r98(int mode) noexcept {
 return mode==1?L"NATIVE HEIGHT":mode==2?L"HMD + NATIVE (RIDE/CROUCH)":L"HMD HEIGHT";
}
}
