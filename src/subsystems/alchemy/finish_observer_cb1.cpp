// Exact retail OnAlchemyRecipeFinish entry: original ABI/return/exceptions
// unchanged. Only a current Henry soul + Alchemy lifetime records its native
// recipe verdict. This neither identifies delivered items nor ends a minigame.
#include "finish_observer_cb1.h"
#include "diagnostic_trace.h"
#include "retail_build112.h"
#include "core/snapshot_mailbox.h"
#include "vrik/visible_failure.h"
#include "diagnostics/capture168.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <optional>
#include <utility>

namespace kcd2vr::alchemy_finish_cb1 {
namespace {
constexpr std::uint32_t kWhTimestamp=0x6A350E20u;
constexpr std::uintptr_t kFinishRva=0x1FD43E0ull;

constexpr unsigned char kEntry[]={0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,
                                 0x48,0x8D,0x68,0xB8,0x48,0x81,0xEC,0x10,0x01,0x00,0x00,0x0F,0x29,0x70,0xB8,0x0F,0x29,0x78,0xA8};
using FinishFn=bool(__fastcall*)(void* minigames,void* soul,const unsigned char* data,unsigned char* outClass,int* outCount);
std::optional<DetourModKit::hook::Hook> g_hook;
FinishFn g_original=nullptr;
std::atomic<bool> g_installed{false},g_enabled{false};
std::atomic<std::uint64_t> g_captureRequestedD42{0},g_captureSessionD42{0};
std::atomic<CurrentReader> g_current{nullptr};
std::atomic<std::uint64_t> g_count{0},g_serial{0};
core::SnapshotMailbox<Result> g_last;

bool SameBytes(std::uintptr_t p,const unsigned char* e,std::size_t n) noexcept {
    __try{return std::memcmp(reinterpret_cast<const void*>(p),e,n)==0;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool ReadTimestamp(std::uintptr_t base,std::uint32_t& out) noexcept {
    out=0;if(!base)return false;
    __try{const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);if(dos->e_magic!=IMAGE_DOS_SIGNATURE)return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);if(nt->Signature!=IMAGE_NT_SIGNATURE)return false;
        out=nt->FileHeader.TimeDateStamp;return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class T> bool Rd(const void* p,std::size_t off,T& out) noexcept {
    __try{std::memcpy(&out,static_cast<const unsigned char*>(p)+off,sizeof(T));return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// The S_AlchemyRecipeFinishData fields (caller frame, valid for the whole call: 0x2E163F4 keeps it at [rbp-0x31]).
void ReadData(const unsigned char* data,Result& r) noexcept {
    int v=0;
    if(!data||!Rd(data,0x00,v))return;r.recipeId=v;
    if(!Rd(data,0x04,v))return;r.performedSteps=v;
    if(!Rd(data,0x08,v))return;r.requiredSteps=v;
    if(!Rd(data,0x14,v))return;r.qualityTier=v;
    if(!Rd(data,0x18,v))return;r.requestedQty=v;
    unsigned char g[16]{};if(!Rd(data,0x1C,g))return;std::memcpy(r.productClass,g,16);
    r.dataRead=1;
}
static bool Current(Context& c) noexcept {
    c={};auto read=g_current.load(std::memory_order_acquire);
    return read&&read(c)&&c.instance&&c.player&&c.actor&&c.soul&&c.lifetime;
}
static bool Same(const Context&a,const Context&b) noexcept {
    return a.instance==b.instance&&a.player==b.player&&a.actor==b.actor&&a.soul==b.soul&&a.lifetime==b.lifetime;
}
static std::uint64_t CaptureSessionD42() noexcept {
    const auto session=g_captureSessionD42.load(std::memory_order_acquire);if(!session)return 0;
    namespace cap=diagnostics::capture168;
    return session==cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire)&&
        cap::active(cap::Domain::Interaction,GetTickCount64())?session:0;
}
bool __fastcall Detour(void* minigames,void* soul,const unsigned char* data,unsigned char* outClass,int* outCount) {
    Context before{};
    const bool explicitFeedback=g_enabled.load(std::memory_order_acquire);
    const auto captureSession=CaptureSessionD42();
    const bool capture=(explicitFeedback||captureSession)&&Current(before)&&before.soul==reinterpret_cast<std::uintptr_t>(soul);
    const bool ok=g_original(minigames,soul,data,outClass,outCount); // original once, same ABI/return/exceptions, also while OFF
    Context after{};
    const bool explicitStill=explicitFeedback&&g_enabled.load(std::memory_order_acquire);
    const bool captureStill=captureSession&&captureSession==CaptureSessionD42();
    if(!capture||(!explicitStill&&!captureStill)||!Current(after)||!Same(before,after))return ok;
    Result r{};r.serial=g_serial.fetch_add(1,std::memory_order_acq_rel)+1;r.tick=std::uint64_t(GetTickCount64());
    r.valid=1;r.success=ok?1:0;r.context=before;ReadData(data,r);
    int n=0;if(outCount&&Rd(outCount,0,n))r.outCount=n;
    (void)g_last.publish(r);g_count.fetch_add(1,std::memory_order_relaxed);
    // Native recipe verdict is sufficient feedback. No created-item scan,
    // delivery requirement, Stop/action/event or inventory operation is added.
    if(explicitStill){char line[160]{};std::snprintf(line,sizeof(line),"native brew %s (recipe %d, quality %d)",ok?"evaluated successfully":"evaluation failed",r.recipeId,r.qualityTier);
        (void)visible_failure::publish(line,r.tick);}
    char m[400]{};std::snprintf(m,sizeof(m),"CB1 ALCHEMY_RECIPE_RESULT serial=%llu instance=%llX lifetime=%llu verdict=%d recipe=%d steps=%d/%d tier=%d reqQty=%d outCount=%d dataRead=%d session=%llu tick=%llu captureOnly=%d delivery=UNOBSERVED nativeProgress=UNCHANGED",
        (unsigned long long)r.serial,(unsigned long long)r.context.instance,(unsigned long long)r.context.lifetime,r.success,r.recipeId,r.performedSteps,r.requiredSteps,r.qualityTier,r.requestedQty,r.outCount,r.dataRead,
        (unsigned long long)(captureStill?captureSession:0),(unsigned long long)r.tick,!explicitStill);
    diagnostic_trace::Write(m);
    return ok;
}
}

bool Install() noexcept {
    if(g_installed.load(std::memory_order_acquire))return true;
    HMODULE wh=GetModuleHandleW(L"WHGame.dll");if(!wh){diagnostic_trace::Write("CB1 ALCHEMY_FINISHED observer: WHGame.dll missing");return false;}
    const auto base=reinterpret_cast<std::uintptr_t>(wh);std::uint32_t ts=0;
    if(!(retail_build112::Ready(base)||retail_build112::Verify(base))||!ReadTimestamp(base,ts)||ts!=kWhTimestamp||!SameBytes(base+kFinishRva,kEntry,sizeof(kEntry))){
        diagnostic_trace::Write("CB1 ALCHEMY_FINISHED observer: unsupported WHGame (timestamp/entry bytes) - OFF");return false;}
    DetourModKit::hook::InlineRequest req{"kcd2vr_alchemy_finish_cb1",DetourModKit::hook::Target{DetourModKit::Address{base+kFinishRva}},DetourModKit::hook::Options{}};
    auto r=DetourModKit::hook::inline_at(std::move(req),&Detour);
    if(!r){diagnostic_trace::Write("CB1 ALCHEMY_FINISHED observer: hook install failed");return false;}
    g_hook=std::move(*r);g_original=g_hook->original<FinishFn>();
    if(!g_original||!g_hook->enable()){g_original=nullptr;g_hook.reset();diagnostic_trace::Write("CB1 ALCHEMY_FINISHED observer: enable failed");return false;}
    g_installed.store(true,std::memory_order_release);
    diagnostic_trace::Write("CB1 ALCHEMY_FINISHED observer ON: C_RPGMinigames::OnAlchemyRecipeFinish 1FD43E0, read-only, original once");
    return true;
}
void Uninstall() noexcept {
    SetEnabled(false);
    SetCaptureSessionD42(0);
    if(g_hook){if(!g_hook->disable()){diagnostic_trace::Write("H41 alchemy observer hook retained: disable unconfirmed");return;}g_hook.reset();}
    g_original=nullptr;g_installed.store(false,std::memory_order_release);
}
void SetCurrentReader(CurrentReader r) noexcept {g_current.store(r,std::memory_order_release);}
bool Enabled() noexcept {return g_enabled.load(std::memory_order_acquire);}
void SetEnabled(bool on) noexcept {g_enabled.store(on,std::memory_order_release);}
void SetCaptureSessionD42(std::uint64_t session) noexcept {
    if(g_captureRequestedD42.exchange(session,std::memory_order_acq_rel)==session)return;
    g_captureSessionD42.store(0,std::memory_order_release);
    if(session&&Install())g_captureSessionD42.store(session,std::memory_order_release);
}
bool Installed() noexcept {return g_installed.load(std::memory_order_acquire);}
bool Last(Result& out) noexcept {return (Enabled()||CaptureSessionD42())&&g_last.read(out);}
std::uint64_t Count() noexcept {return g_count.load(std::memory_order_relaxed);}
}
