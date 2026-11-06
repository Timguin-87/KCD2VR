#pragma once
#include "core/snapshot_mailbox.h"
#include <cstdint>
#include <cmath>
// Input consumption is not physics permission. Native qualification owns the
// candidate; the worker owns ONE squeeze; the drag rechecks its native body.
namespace kcd2vr::corpse_grip_cg1 {
struct Key {
 std::uintptr_t actor=0,identity=0,root=0;
 std::uint64_t epoch=0,serial=0;std::uint32_t entity=0;int part=-1;
};
inline bool same(const Key&a,const Key&b) noexcept {
 return a.actor==b.actor&&a.identity==b.identity&&a.root==b.root&&a.epoch==b.epoch&&
  a.serial==b.serial&&a.entity==b.entity&&a.part==b.part;
}
inline bool valid(const Key&k) noexcept {return k.actor&&k.identity&&k.root&&k.epoch&&k.serial&&k.entity&&k.part>=0;}
inline bool fresh(std::uint64_t tick,std::uint64_t now) noexcept {return tick&&now>=tick&&now-tick<=80;}
struct Candidate {Key key{};std::uint64_t tick=0;bool active=false;};
struct Request {Key key{};std::uint64_t tick=0,press=0;};
inline core::SnapshotMailbox<Candidate> candidate;
inline core::SnapshotMailbox<Request> request;
struct Publisher {
 Candidate current{};std::uint64_t next=0;
 Candidate publish(Key k,std::uint64_t now,bool active) noexcept {
  k.serial=current.key.serial;
  if(!current.tick||!same(k,current.key)){k.serial=++next;if(!k.serial)k.serial=++next;}
  current={k,now,active};(void)candidate.publish(current);return current;
 }
 void clear() noexcept {current={};(void)candidate.publish({});}
};
inline bool permits(const Request&r,const Candidate&c,std::uint64_t now) noexcept {
 return r.press&&fresh(r.tick,now)&&fresh(c.tick,now)&&valid(r.key)&&same(r.key,c.key);
}
struct Output {bool suppressLeftUse=false;Request native{};};
struct Gate {
 bool armed=false,down=false,consumed=false,permission=false;std::uint64_t next=0,press=0;Key owner{};
 Output update(std::uint64_t now,bool inputCurrent,bool context,float grip,const Candidate&c) noexcept {
  Output out{};
  const bool raw=inputCurrent&&std::isfinite(grip)&&grip>=0&&grip<=1;
  const bool released=raw&&grip<.30f;
  if(consumed){
   // Losing qualification retires native authority, never releases this old
   // squeeze into a delayed A. Only a real current opened hand consumes release.
   out.suppressLeftUse=true;
   if(!raw||!context||!fresh(c.tick,now)||!same(owner,c.key))permission=false;
   if(released){consumed=permission=down=false;armed=true;owner={};press=0;return out;}
   if(permission)out.native={owner,now,press};
   return out;
  }
  if(!raw){armed=false;return out;}
  if(released){armed=true;down=false;return out;}
  if(grip>.58f&&!down){
   down=true;
   if(armed&&context&&fresh(c.tick,now)&&valid(c.key)){
    consumed=permission=true;owner=c.key;press=++next;if(!press)press=++next;
    out.suppressLeftUse=true;out.native={owner,now,press};
   }
   armed=false;
  }
  return out;
 }
};
}
