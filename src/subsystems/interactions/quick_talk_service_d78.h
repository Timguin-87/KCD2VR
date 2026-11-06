#pragma once
#include "quick_talk_candidate_d78.h"
#include "../../native_script_t2.h"
#include "../../frik_pose_probe.h"
#include "../../interaction_alignment.h"
#include <windows.h>
#include <cstdio>
#include <cstring>

namespace kcd2vr::quick_talk_target_d78 {
using Body=frik_pose_probe::MiscGestureBodyD59;
struct Request {Candidate target{};Body body{};std::uint64_t serial=0,tick=0,generation=0;};
struct Receipt {Request request{};std::uint64_t tick=0;bool eligible=false;};
inline core::SnapshotMailbox<Request> requests;
inline core::SnapshotMailbox<Receipt> receipts;
inline std::atomic<std::uint64_t> next{0},pending{0},generation{1};
inline std::atomic<bool> enabled{false};
inline bool SameBody(const Body& a,const Body& b) noexcept {
 return a.player&&a.playerId&&a.character&&a.body&&a.tracking&&a.player==b.player&&a.playerId==b.playerId&&
  a.character==b.character&&a.body==b.body&&a.tracking==b.tracking;
}
inline bool Current(const Request& r,std::uint64_t now) noexcept {
 Candidate c{};Body body{};
 return enabled.load(std::memory_order_acquire)&&Observing(now)&&r.serial&&r.generation==generation.load(std::memory_order_acquire)&&
  Fresh(r.tick,now)&&Fresh(r.body.tick,now)&&Fresh(r.target.tick,now)&&
  candidate.read(c)&&Fresh(c.tick,now)&&c.tick>=r.target.tick&&Same(c,r.target)&&c.player==r.body.player&&
  interaction_alignment::DoorAcquisitionCurrent65(c.generation)&&
  frik_pose_probe::ReadMiscGestureBodyD59(body)&&Fresh(body.tick,now)&&SameBody(body,r.body);
}
// Input owner only. A disabled gesture revokes pending replies immediately.
// A positive receipt is never extended by polling or by a new candidate tick.
inline bool Prepare(bool on,std::uint64_t now) noexcept {
 if(enabled.exchange(on,std::memory_order_acq_rel)!=on)generation.fetch_add(1,std::memory_order_acq_rel);
 wantedUntil.store(on?now+80:0,std::memory_order_release);
 if(!on){pending.store(0,std::memory_order_release);return false;}
 Candidate c{};Body body{};
 if(!candidate.read(c)||!Fresh(c.tick,now)||!interaction_alignment::DoorAcquisitionCurrent65(c.generation)||
    !frik_pose_probe::ReadMiscGestureBodyD59(body)||!Fresh(body.tick,now)||c.player!=body.player)return false;
 const auto gen=generation.load(std::memory_order_acquire);Receipt result{};
 const bool admitted=receipts.read(result)&&result.eligible&&Fresh(result.tick,now)&&Current(result.request,now);
 Request old{};const bool recent=requests.read(old)&&old.generation==gen&&Same(old.target,c)&&SameBody(old.body,body)&&
  now>=old.tick&&now-old.tick<40;
 if(!recent&&!pending.load(std::memory_order_acquire)){
  const auto serial=next.fetch_add(1,std::memory_order_acq_rel)+1;
  if(serial<=9007199254740991ull&&requests.publish({c,body,serial,now,gen}))pending.store(serial,std::memory_order_release);
 }
 return admitted;
}
inline bool Wanted() noexcept {return enabled.load(std::memory_order_acquire)&&pending.load(std::memory_order_acquire)!=0;}
inline void Service(const native_script_t2::Context& context) noexcept {
 Request r{};if(!requests.read(r)||!r.serial)return;
 auto serial=r.serial;if(!pending.compare_exchange_strong(serial,0,std::memory_order_acq_rel))return;
 Receipt out{r,std::uint64_t(GetTickCount64()),false};
 if(context.Current()&&Current(r,out.tick)){
  char input[128]{},reply[128]{},yes[128]{};
  std::snprintf(input,sizeof(input),"CHAT|%llu|%u|%u",(unsigned long long)r.serial,r.body.playerId,r.target.entity);
  std::snprintf(yes,sizeof(yes),"CHAT|%llu|%u|1",(unsigned long long)r.serial,r.target.entity);
  const bool called=context.CallPacket("kcd2vr_chat_target_d78",input,reply,sizeof(reply));
  // Context.now is the dispatcher's entry, possibly before other clients. A
  // delayed Lua return must not publish positive eligibility for that old time.
  out.tick=std::uint64_t(GetTickCount64());
  out.eligible=called&&!std::strcmp(reply,yes)&&context.Current()&&Current(r,out.tick);
 }
 // Wrong/revoked/negative results also supersede an older positive result.
 if(r.generation==generation.load(std::memory_order_acquire))
  (void)receipts.publish(out);
}
}
