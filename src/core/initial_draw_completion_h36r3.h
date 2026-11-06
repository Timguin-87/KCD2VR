#pragma once
#include "snapshot_mailbox.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <memory>
namespace kcd2vr::initial_draw_h36r3 {
// Copies only. Callback storage survives the process; no queued native job owns
// a pointer to a command record, host stack, item or attachment borrowed by us.
struct Watch {
 std::uint64_t serial=0,key=0,body=0,request=0,sample=0,tick=0,membership=0;
 std::uintptr_t manager=0,player=0,character=0,item=0;
};
struct Submitted {Watch original{};std::uintptr_t world=0;std::uint64_t token=0;};
struct Returned { Watch original{};std::uintptr_t link=0,entity=0;std::uint32_t entityId=0;std::uintptr_t world=0;std::uint64_t token=0; };
struct Slot {core::SnapshotMailbox<Watch> watch;core::SnapshotMailbox<Returned> returned;core::SnapshotMailbox<Submitted> submitted;std::atomic<unsigned> submission{0};std::atomic<std::uint64_t> active{0};std::atomic_flag writing=ATOMIC_FLAG_INIT;};
// Metadata-only nonblocking writer lease. No native read/call or wait is held.
// A retired slot cannot be reused while its prior writer can still publish.
struct WriteLease {
 Slot* slot=nullptr;
 explicit WriteLease(Slot&s) noexcept {if(!s.writing.test_and_set(std::memory_order_acquire))slot=&s;}
 WriteLease(const WriteLease&)=delete;WriteLease&operator=(const WriteLease&)=delete;
 ~WriteLease(){if(slot)slot->writing.clear(std::memory_order_release);}
 explicit operator bool()const noexcept{return slot!=nullptr;}
};
inline std::array<Slot,8> slots;
inline std::atomic<std::uint64_t> next{0};
inline std::atomic<bool> callbackReady{false};
inline bool same(const Watch&a,const Watch&b) noexcept {return a.serial&&a.serial==b.serial&&a.key==b.key&&a.body==b.body&&a.request==b.request&&a.sample==b.sample&&a.tick==b.tick&&a.membership==b.membership&&a.manager==b.manager&&a.player==b.player&&a.character==b.character&&a.item==b.item;}
inline bool arm(Watch& w) noexcept {
 if(!callbackReady.load(std::memory_order_acquire))return false;
 auto previous=next.load(std::memory_order_acquire);
 do{if(previous>=UINT64_MAX-1)return false;}while(!next.compare_exchange_weak(previous,previous+1,std::memory_order_acq_rel));
 const auto id=previous+1;w.serial=id;
 for(auto& s:slots){if(s.active.load(std::memory_order_acquire))continue;WriteLease lease(s);if(!lease)continue;
  std::uint64_t free=0;if(!s.active.compare_exchange_strong(free,UINT64_MAX))continue;
  if(!s.watch.publish(w)){s.active.store(0,std::memory_order_release);return false;}
  s.submission.store(0,std::memory_order_release);
  s.active.store(id,std::memory_order_release);return true;}
 return false;
}
inline bool read(const Watch&w,Returned&out) noexcept {
 for(auto& s:slots){Submitted submitted{};
  if(s.active.load(std::memory_order_acquire)==w.serial&&s.submission.load(std::memory_order_acquire)==2&&s.submitted.read(submitted)&&same(w,submitted.original)&&s.returned.read(out)&&same(w,out.original)&&out.world==submitted.world&&out.token==submitted.token&&s.active.load(std::memory_order_acquire)==w.serial&&s.submission.load(std::memory_order_acquire)==2)return true;
 }
 return false;
}
inline thread_local Watch invoking{};
// POD-only native SEH finally: /EHsc does NOT unwind a C++ RAII scope
// when the original game raises a native SEH handled by our existing outer leaf.
// Keep the actual same production helper in host tests; never emulate this with
// the test Windows header's __try->try macro.
#if defined(_MSC_VER)
#pragma push_macro("__try")
#undef __try
inline bool invokeScoped(const Watch&w,bool(*invoke)(void*),void*context) {
 const Watch previous=invoking;invoking=w;
 __try {return invoke(context);}
 __finally {invoking=previous;}
}
#pragma pop_macro("__try")
#else
// Portable host checks cannot exercise Windows native SEH. Preserve ordinary
// C++ exception and nested-scope cleanup without introducing MS-only syntax.
inline bool invokeScoped(const Watch&w,bool(*invoke)(void*),void*context) {
 const Watch previous=invoking;invoking=w;
 try {const bool returned=invoke(context);invoking=previous;return returned;}
 catch (...) {invoking=previous;throw;}
}
#endif
inline void submitted(const Watch&w,std::uintptr_t world,std::uint64_t token) noexcept {
 if(!w.serial||!world||!token)return;
 for(auto&s:slots){if(s.active.load(std::memory_order_acquire)!=w.serial)continue;WriteLease lease(s);if(!lease)continue;
  Watch current{};if(s.active.load(std::memory_order_acquire)!=w.serial||!s.watch.read(current)||!same(current,w))continue;
  unsigned none=0;if(!s.submission.compare_exchange_strong(none,1)){Submitted old{};if(none!=2||!s.submitted.read(old)||!same(w,old.original)||old.world!=world||old.token!=token)s.submission.store(3,std::memory_order_release);return;}
  const bool done=s.submitted.publish({w,world,token});s.submission.store(done?2u:3u,std::memory_order_release);return;
 }
}
inline bool publishReturned(const Submitted&expected,const Returned&out) noexcept {
 if(!same(expected.original,out.original)||expected.world!=out.world||expected.token!=out.token)return false;
 for(auto&s:slots){if(s.active.load(std::memory_order_acquire)!=expected.original.serial)continue;WriteLease lease(s);if(!lease)continue;
  Submitted current{};
  if(s.active.load(std::memory_order_acquire)!=expected.original.serial||s.submission.load(std::memory_order_acquire)!=2||
     !s.submitted.read(current)||!same(current.original,expected.original)||current.world!=expected.world||current.token!=expected.token)continue;
  return s.returned.publish(out);
 }
 return false;
}
inline void retire(const Watch&w) noexcept {if(!w.serial)return;for(auto& s:slots){auto id=w.serial;(void)s.active.compare_exchange_strong(id,0);}}
}
