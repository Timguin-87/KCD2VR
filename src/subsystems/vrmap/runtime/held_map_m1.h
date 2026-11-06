#pragma once
#include "core/snapshot_mailbox.h"
#include <atomic>
#include "native_map_open_h25.h"
#include <cstdint>
#include <cmath>
namespace kcd2vr::held_map_m1 {
// Utility presentation is separate from native equipment/combat identity.
inline std::atomic<bool> selected{false},requested{false};
inline std::atomic<float> paperSizeD64{1.f};
inline std::atomic<bool> twoHandD64{true};
// Desired renderer only; native readiness and cleanup remain independent.
inline std::atomic<bool> nativePaperM43{false};
// Exact close tickets, not bool ACKs: an old reply cannot clear a newer close.
inline std::atomic<std::uint64_t> closePending31{0},closeWork31{0};
inline std::atomic<std::uint64_t> allowedUntil{0};
inline std::atomic<bool> backConsumed{false};
inline std::atomic<std::uint64_t> activation{0},closingUntil31{0},closedAt31{0};
// Publishers and ACK retirement contain only scalar stores; never hold this
// lock through native/VM callbacks. Region and user intent have two issuers.
inline std::atomic_flag intentLock31=ATOMIC_FLAG_INIT;
struct IntentGuard31 {
 IntentGuard31() noexcept {while(intentLock31.test_and_set(std::memory_order_acquire)){} }
 ~IntentGuard31(){intentLock31.clear(std::memory_order_release);}
 IntentGuard31(const IntentGuard31&)=delete;
};
struct IntentSnapshot31 {std::uint64_t serial=0;bool open=false;};
inline IntentSnapshot31 Intent31() noexcept {IntentGuard31 guard;return {activation.load(),requested.load()};}
inline void AckClosed31(std::uint64_t serial) noexcept {
 IntentGuard31 guard;
 if(activation.load()!=serial||requested.load())return;
 closedAt31.store(serial);
 if(closePending31.load()==serial)closePending31.store(0);
 if(closeWork31.load()==serial)closeWork31.store(0);
}
inline void Fault31(std::uint64_t serial) noexcept {
 IntentGuard31 guard;if(activation.load()!=serial)return;
 requested.store(false);closePending31.store(0);closeWork31.store(0);closedAt31.store(serial);
}
inline std::uint64_t CloseTicket31() noexcept {
 IntentGuard31 guard;const auto work=closeWork31.load();
 return work&&work==closePending31.load()&&work==activation.load()&&!requested.load()?work:0;
}
inline bool FinishClose31(std::uint64_t serial,bool acknowledged,bool closed) noexcept {
 IntentGuard31 guard;
 if(requested.load()||activation.load()!=serial||closeWork31.load()!=serial||closePending31.load()!=serial)return false;
 closeWork31.store(0);if(acknowledged)closePending31.store(0);
 if(acknowledged&&closed)closedAt31.store(serial);
 return true;
}
struct FoldM39 {std::uint64_t serial=0,tick=0;float fraction=-1;};
inline core::SnapshotMailbox<FoldM39> foldM39;
inline bool ReadFoldM39(std::uint64_t now,FoldM39& f) noexcept {
 return foldM39.read(f)&&f.tick&&now>=f.tick&&now-f.tick<=80&&
  f.serial==activation.load(std::memory_order_acquire)&&requested.load(std::memory_order_acquire)&&
  std::isfinite(f.fraction)&&f.fraction>=0&&f.fraction<=1;
}
inline void SetRequested31(bool on,std::uint64_t now) noexcept {
 // DS7 (DeepSeek, 27 Oct): the held parchment map is re-enabled behind the
 // caller's choice. The native ordinary map remains the owner whenever the
 // caller passes false; the chord routes by Selected() before this call.
 IntentGuard31 guard;
 if(requested.exchange(on)!=on){closingUntil31.store(0);const auto serial=activation.fetch_add(1)+1;closePending31.store(on?0:serial);closeWork31.store(on?0:serial);}
}
inline bool NativePaperSelectedM43() noexcept {return nativePaperM43.load(std::memory_order_acquire);}
inline void SetNativePaperM43(bool on) noexcept {
 // UI/config requests only. Never call the engine/VM or inherit an old renderer's
 // open receipt. A changed choice closes the current map; the next deliberate
 // opening must qualify the new backend after old presentation cleanup.
 IntentGuard31 guard;
 if(nativePaperM43.exchange(on,std::memory_order_acq_rel)==on)return;
 const bool wasOpen=requested.exchange(false);
 closingUntil31.store(0);
 const auto serial=activation.fetch_add(1)+1;
 if(wasOpen||closePending31.load()){closePending31.store(serial);closeWork31.store(serial);}
}
// Use one coherent snapshot for renderer/readiness decisions across a UI change.
struct RendererIntentM43 {std::uint64_t serial=0;bool open=false,native=false;};
inline RendererIntentM43 RendererIntent43() noexcept {
 IntentGuard31 guard;return {activation.load(),requested.load(),nativePaperM43.load()};
}
inline bool RendererIntentMatches43(const RendererIntentM43& expected) noexcept {
 const auto current=RendererIntent43();
 return current.serial==expected.serial&&current.open==expected.open&&current.native==expected.native;
}
// A completed gesture cannot close a newer input/region activation.
inline bool CloseIfCurrentM39(std::uint64_t serial,std::uint64_t now) noexcept {
 IntentGuard31 guard;
 if(!requested.load()||activation.load()!=serial)return false;
 requested.store(false);closingUntil31.store(0);
 const auto closeSerial=activation.fetch_add(1)+1;
 closePending31.store(closeSerial);closeWork31.store(closeSerial);return true;
}
inline bool Present31(std::uint64_t now) noexcept {
 (void)now;IntentGuard31 guard;return requested.load(); // D44: close hides immediately; ACK cleanup is independent.
}
// Qualified current-player feed selects region; explicit printed fallback remains.
enum class PrintedRegion : unsigned { Trosky=0, Kuttenberg=1 };
inline std::atomic<PrintedRegion> printedRegion{PrintedRegion::Trosky};
inline PrintedRegion GetPrintedRegion() noexcept {return printedRegion.load(std::memory_order_acquire);}
inline bool SetPrintedRegion(PrintedRegion region) noexcept {
 if(region!=PrintedRegion::Trosky&&region!=PrintedRegion::Kuttenberg)return false;
 if(printedRegion.load(std::memory_order_acquire)==region)return true;
 IntentGuard31 guard;
 if(printedRegion.exchange(region,std::memory_order_acq_rel)!=region){const auto serial=activation.fetch_add(1)+1;if(!requested.load()&&closePending31.load()){closePending31.store(serial);closeWork31.store(serial);}}
 return true;
}
inline bool Selected() noexcept{return selected.load(std::memory_order_acquire);} // DS7: re-enabled
inline bool Requested() noexcept{return requested.load(std::memory_order_acquire);}
inline void Select(bool on) noexcept {
 IntentGuard31 guard;
 const bool previous=requested.exchange(on);selected.store(on);const auto serial=activation.fetch_add(1)+1;
 if(on){closePending31.store(0);closeWork31.store(0);}
 else if(previous||closePending31.load()){closePending31.store(serial);closeWork31.store(serial);}
}
// C10R10 (merged in R14): the standalone Map is available in normal play (INI VrHeldMap, default on): selected, but CLOSED until
// the player opens it (right thumbrest + left stick up 1 s -> direct native paper animation; B closes). F2 only arms availability.
inline void Enable(bool on) noexcept {
 IntentGuard31 guard;
 selected.store(on);const bool previous=!on&&requested.exchange(false);
 if(!on)closingUntil31.store(0);
 const auto serial=activation.fetch_add(1)+1;
 if(!requested.load()&&(previous||closePending31.load())){closePending31.store(serial);closeWork31.store(serial);}
}
inline void Toggle() noexcept {if(Selected())SetRequested31(!Requested(),0);}
inline void PublishAllowed(bool on,std::uint64_t now) noexcept {allowedUntil.store(on?now+150:0,std::memory_order_release);}
inline bool Allowed(std::uint64_t now) noexcept {const auto until=allowedUntil.load(std::memory_order_acquire);return until&&now<=until;}
class BackEdge {
 bool previous_=false,owned_=false;
public:
 bool Filter(bool down,bool canOwn,std::uint64_t now=0) noexcept {
  if(!down){owned_=false;previous_=false;backConsumed.store(false);return false;}
  if(!previous_&&canOwn){owned_=true;SetRequested31(false,now);}
  previous_=true;backConsumed.store(owned_);return owned_;
 }
};
// Native frame pump only. No foreign-thread entity calls or new XR location.
bool Install() noexcept;
void NativeFrameTick() noexcept;
void Shutdown() noexcept;
struct Counts {std::uint64_t placements=0,shows=0,hides=0,missing=0,stale=0;unsigned poseRefusal=0,lastHideReasonM4=0;};
Counts Snapshot() noexcept;
bool VisibleD72() noexcept;
}
