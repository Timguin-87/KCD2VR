#include "native_svo_right_v6654.h"
#include "native_svo_prev_per_eye_v6649.h"
#include "diagnostic_trace.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <optional>
#include <utility>
#include <cstdio>

namespace kcd2vr::native_svo_right_v6654 { namespace {

// ---- Static proof (retail WHGame.dll SHA-256 bdf8f9e4..., timestamp 0x6A350E20) ----
// Pipeline vtable 0x183DFEBD0: +0x28 Update 0x18057278C, +0x40 Execute 0x18057362C,
//   +0x48 SetPhase 0x180864984 (phase 4 -> Update). The RIGHT pipeline call 0x180864728
//   runs SetPhase(3),(4),(1) then Execute, so Update runs for LEFT and for RIGHT.
// Update 0x18057278C: for every stage in [pipe+0x30..+0x1A0): if IsStageActive -> vcall +0x20.
// CSvoGiStage vtable 0x183C26198: +0x20 = 0x180FF33EC ("Update"):
//   svo=[stage+0x16B0]; if !svo || [svo+0x2CF0] != [[crv+0x8D60]+0x19B0] -> destroy+recreate
//   (0x180F96E28 + free 0x2CF8), then CheckAllocateRT 0x18057827C(stage, spec=0) and, when
//   integration mode==2 and specular on, (stage, spec=1).
// 0x18057827C: creates each slot with 0x180578644 and then SWAPS the ping-pong pairs:
//   diff set svo+0x00..0x68 (14 CTexture*), spec set svo+0xDA8..0xE10 (same layout),
//   and stage+0x50/+0x58 (SVO_SHAD_FIN / SV1_SHAD_FIN, diff path only).
// 0x180578644(unused, CTexture** slot, int w, int h, a5, a6, a7, const char* name):
//   no-op if *slot backed (+0x80) with w/h (+0x90/+0x92) and fmt 0x14 (+0x96); else
//   0x18073CEF4 -> resize an existing backed object, or create BY NAME (0x1806A10A4 looks
//   the name up, so a new RIGHT set needs UNIQUE names). Slot ends with exactly one ref.
// 0x180771960 = CTexture release (lock xadd -1 on refcount).
// SVO render 0x180F80D6C (called from pipeline Execute at 0x180573839 in BOTH passes):
//   compute part, then atmosphere/sun-shadow/cone-trace/demosaic/upscale (site B callers),
//   then stage+0xA0 = current view-proj (uploaded as SVO_ViewProjPrev next time).
// Site B 0x18057B764: if (setArg != svo || [crv+0x2C] != [0x18491D2E4]) it computes the
//   reprojection (stage+0x60) from the static previous camera and stores the frame number
//   into 0x18491D2E4 (0x18057C3F8). Same frame number in the RIGHT pass -> RIGHT skipped it.
constexpr std::uint32_t kTimestamp = 0x6A350E20u;
constexpr std::uintptr_t kUpdateRva = 0xFF33ECull;
constexpr std::uintptr_t kCreateRtRva = 0x578644ull;
constexpr std::uintptr_t kReleaseRva = 0x771960ull;
constexpr std::uintptr_t kCacheCmpRva = 0x57B83Aull;    // mov eax,[rbx+2C]; cmp [rip+X],eax; jne
constexpr std::uintptr_t kCacheStoreRva = 0x57C3F8ull;  // mov [rip+X],eax
constexpr std::uintptr_t kCacheGlobalRva = 0x491D2E4ull;
constexpr std::uintptr_t kRecreateCheckRva = 0xFF33F6ull;

constexpr unsigned char kUpdateEntry[] = {0x48,0x89,0x5C,0x24,0x18,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0x41,0x28,0x48};
constexpr unsigned char kRecreateCheck[] = {  // mov rax,[rcx+28]; mov rbx,rcx; mov rcx,[rcx+16B0]; mov rax,[rax+8D60]
    0x48,0x8B,0x41,0x28,0x48,0x8B,0xD9,0x48,0x8B,0x89,0xB0,0x16,0x00,0x00,0x48,0x8B,0x80,0x60,0x8D,0x00,0x00};
constexpr unsigned char kCreateRtEntry[] = {0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x4C,0x24,0x08,0x57,0x48,0x83,0xEC,0x60,0x4C};
constexpr unsigned char kReleaseEntry[] = {0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0xF7};
constexpr unsigned char kCacheCmpHead[] = {0x8B,0x43,0x2C,0x39,0x05};
constexpr unsigned char kCacheStoreHead[] = {0x89,0x05};

// Layout
constexpr std::uintptr_t kStageCrv = 0x28, kStageSvo = 0x16B0, kStagePrevVP = 0xA0;
constexpr std::uintptr_t kCrvViewType = 0x14, kCrvFrame = 0x2C, kCrvPipe = 0x8D60, kCrvSelector = 0x8EE0;
constexpr std::uintptr_t kPipeSvoStamp = 0x19B0, kSvoStamp = 0x2CF0;
constexpr std::uintptr_t kTexBacking = 0x80, kTexW = 0x90, kTexH = 0x92, kTexFmt = 0x96;
constexpr std::uint8_t kSvoFmt = 0x14;

struct SlotDef { std::uintptr_t off; bool inStage; const char* name; };
constexpr SlotDef kSlots[] = {
    // diffuse set, svo+0x000
    {0x000,false,"VR1_SVO_DIFF_RGB"},{0x008,false,"VR1_SVO_DIFF_ALD"},
    {0x010,false,"VR1_SV1_DIFF_RGB"},{0x018,false,"VR1_SV1_DIFF_ALD"},
    {0x020,false,"VR1_SVO_DIFF_FIN_RGB_MIN"},{0x028,false,"VR1_SVO_DIFF_FIN_ALD_MIN"},
    {0x030,false,"VR1_SVO_DIFF_FIN_RGB_MAX"},{0x038,false,"VR1_SVO_DIFF_FIN_ALD_MAX"},
    {0x040,false,"VR1_SV1_DIFF_FIN_RGB_MIN"},{0x048,false,"VR1_SV1_DIFF_FIN_ALD_MIN"},
    {0x050,false,"VR1_SV1_DIFF_FIN_RGB_MAX"},{0x058,false,"VR1_SV1_DIFF_FIN_ALD_MAX"},
    {0x060,false,"VR1_SVO_FIN_DIFF_OUT"},{0x068,false,"VR1_SV1_FIN_DIFF_OUT"},
    // specular set, svo+0xDA8
    {0xDA8,false,"VR1_SVO_SPEC_RGB"},{0xDB0,false,"VR1_SVO_SPEC_ALD"},
    {0xDB8,false,"VR1_SV1_SPEC_RGB"},{0xDC0,false,"VR1_SV1_SPEC_ALD"},
    {0xDC8,false,"VR1_SVO_SPEC_FIN_RGB_MIN"},{0xDD0,false,"VR1_SVO_SPEC_FIN_ALD_MIN"},
    {0xDD8,false,"VR1_SVO_SPEC_FIN_RGB_MAX"},{0xDE0,false,"VR1_SVO_SPEC_FIN_ALD_MAX"},
    {0xDE8,false,"VR1_SV1_SPEC_FIN_RGB_MIN"},{0xDF0,false,"VR1_SV1_SPEC_FIN_ALD_MIN"},
    {0xDF8,false,"VR1_SV1_SPEC_FIN_RGB_MAX"},{0xE00,false,"VR1_SV1_SPEC_FIN_ALD_MAX"},
    {0xE08,false,"VR1_SVO_FIN_SPEC_OUT"},{0xE10,false,"VR1_SV1_FIN_SPEC_OUT"},
    // sun-shadow history, stage+0x50/0x58
    {0x050,true,"VR1_SVO_SHAD_FIN"},{0x058,true,"VR1_SV1_SHAD_FIN"},
};
constexpr int kSlotCount = int(sizeof(kSlots)/sizeof(kSlots[0]));
static_assert(kSlotCount<=32,"mask is 32-bit");

using UpdateFn = std::uintptr_t(__fastcall*)(void*,void*,void*,void*);
using CreateRtFn = void(__fastcall*)(void*,void**,int,int,int,int,int,const char*);
using ReleaseFn = int(__fastcall*)(void*);

std::optional<DetourModKit::hook::Hook> g_hook;
UpdateFn g_original = nullptr;
CreateRtFn g_create = nullptr;
ReleaseFn g_release = nullptr;
volatile std::uint32_t* g_cache = nullptr;
std::atomic<bool> g_installed{false};
std::atomic<bool> g_enabled{true};

// ---- render-thread state ----
void* g_right[kSlotCount]{};          // one reference each; holds LEFT's pointers while swapped in
std::uint32_t g_swapMask = 0;         // slots exchanged by the current install
unsigned char g_prevVP[64]{};         // RIGHT previous view-proj (LEFT's while swapped in)
bool g_prevVPValid = false;
bool g_swappedIn = false;
std::uintptr_t g_stage = 0;           // the stage that runs the RIGHT pass (locked on first use)
std::uintptr_t g_svoSeen = 0;
std::uint32_t g_installFrame = 0;
std::uint64_t g_lastLogMs = 0;

// stats
std::atomic<std::uint64_t> s_updL{0}, s_updR{0}, s_installs{0}, s_restoreEnd{0}, s_restoreFallback{0},
    s_created{0}, s_provisionFail{0}, s_skipRecreate{0}, s_resets{0}, s_rightReproj{0}, s_rightNoReproj{0},
    s_otherStage{0}, s_disabledR{0};

template<class T> bool Rd(std::uintptr_t a,T& out) noexcept {
    if(a<0x10000) return false;
    __try { out=*reinterpret_cast<const T*>(a); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
template<class T> bool Wr(std::uintptr_t a,const T& v) noexcept {
    if(a<0x10000) return false;
    __try { *reinterpret_cast<T*>(a)=v; return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
bool Swap64(std::uintptr_t a,unsigned char* stash) noexcept {
    if(a<0x10000) return false;
    __try {
        unsigned char tmp[64];
        std::memcpy(tmp,reinterpret_cast<const void*>(a),64);
        std::memcpy(reinterpret_cast<void*>(a),stash,64);
        std::memcpy(stash,tmp,64);
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
bool Copy64(std::uintptr_t a,unsigned char* out) noexcept {
    if(a<0x10000) return false;
    __try { std::memcpy(out,reinterpret_cast<const void*>(a),64); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
bool Matches(std::uintptr_t a,const unsigned char* b,std::size_t n) noexcept {
    __try { return std::memcmp(reinterpret_cast<const void*>(a),b,n)==0; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
void CallCreate(void** slot,int w,int h,const char* name) noexcept {
    __try { g_create(nullptr,slot,w,h,0,0,0,name); }
    __except(EXCEPTION_EXECUTE_HANDLER) {}
}
void CallRelease(void* t) noexcept {
    if(!t || !g_release) return;
    __try { (void)g_release(t); }
    __except(EXCEPTION_EXECUTE_HANDLER) {}
}

void Log(const char* m) noexcept { diagnostic_trace::Write(m); }

struct TexInfo { bool ok=false, backed=false; std::uint16_t w=0,h=0; std::uint8_t fmt=0; };
TexInfo ReadTex(std::uintptr_t t) noexcept {
    TexInfo i{}; if(!t) return i;
    std::uintptr_t backing=0;
    if(!Rd(t+kTexBacking,backing) || !Rd(t+kTexW,i.w) || !Rd(t+kTexH,i.h) || !Rd(t+kTexFmt,i.fmt)) return i;
    i.ok=true; i.backed=backing!=0; return i;
}
std::uintptr_t SlotAddr(int i,std::uintptr_t stage,std::uintptr_t svo) noexcept {
    return (kSlots[i].inStage?stage:svo)+kSlots[i].off;
}

// eye of a NORMAL main render view, else -1
int EyeOf(std::uintptr_t stage,std::uintptr_t& crv) noexcept {
    crv=0; std::uint32_t type=~0u; std::int32_t sel=-1;
    if(!Rd(stage+kStageCrv,crv) || crv<0x10000) return -1;
    if(!Rd(crv+kCrvViewType,type) || type!=0u) return -1;
    if(!Rd(crv+kCrvSelector,sel)) return -1;
    return (sel==0||sel==1)?sel:-1;
}

// true when retail Update will NOT destroy/recreate the SVO object this call
bool SvoStable(std::uintptr_t stage,std::uintptr_t crv,std::uintptr_t& svo) noexcept {
    svo=0; std::uintptr_t pipe=0; std::uint32_t want=0, have=0;
    if(!Rd(stage+kStageSvo,svo) || !svo) return false;
    if(!Rd(crv+kCrvPipe,pipe) || !pipe) return false;
    if(!Rd(pipe+kPipeSvoStamp,want) || !Rd(svo+kSvoStamp,have)) return false;
    return want==have;
}

void ReleaseRightSet(const char* why) noexcept {
    unsigned n=0;
    for(int i=0;i<kSlotCount;++i){ if(g_right[i]){ CallRelease(g_right[i]); g_right[i]=nullptr; ++n; } }
    g_swapMask=0; g_prevVPValid=false; g_swappedIn=false;
    s_resets.fetch_add(1,std::memory_order_relaxed);
    char m[200]{}; std::snprintf(m,sizeof(m),"V66.54-SVO-RIGHT RESET reason=%s released=%u",why,n); Log(m);
}

// Make sure every RIGHT slot whose LEFT twin is in use exists at the LEFT size/format.
// Returns the mask of slots ready to be exchanged, or ~0u on failure.
std::uint32_t Provision(std::uintptr_t stage,std::uintptr_t svo) noexcept {
    std::uint32_t mask=0; unsigned created=0;
    for(int i=0;i<kSlotCount;++i){
        std::uintptr_t left=0;
        if(!Rd(SlotAddr(i,stage,svo),left)) return ~0u;
        const TexInfo l=ReadTex(left);
        if(!left || !l.ok || !l.backed || !l.w || !l.h) continue;  // LEFT does not use this slot
        if(l.fmt!=kSvoFmt) { char m[200]{}; std::snprintf(m,sizeof(m),
            "V66.54-SVO-RIGHT REFUSED slot=%s leftFmt=0x%02X (expected 0x14)",kSlots[i].name,unsigned(l.fmt)); Log(m); return ~0u; }
        const TexInfo r=ReadTex(reinterpret_cast<std::uintptr_t>(g_right[i]));
        if(!(g_right[i] && r.ok && r.backed && r.w==l.w && r.h==l.h && r.fmt==kSvoFmt)) {
            CallCreate(&g_right[i],int(l.w),int(l.h),kSlots[i].name);
            const TexInfo a=ReadTex(reinterpret_cast<std::uintptr_t>(g_right[i]));
            if(!(g_right[i] && a.ok && a.backed && a.w==l.w && a.h==l.h && a.fmt==kSvoFmt)) {
                s_provisionFail.fetch_add(1,std::memory_order_relaxed);
                char m[240]{}; std::snprintf(m,sizeof(m),"V66.54-SVO-RIGHT CREATE-FAILED slot=%s want=%ux%u got=%p %ux%u backed=%d",
                    kSlots[i].name,unsigned(l.w),unsigned(l.h),g_right[i],unsigned(a.w),unsigned(a.h),a.backed?1:0); Log(m);
                return ~0u;
            }
            ++created;
        }
        mask|=1u<<i;
    }
    if(created){
        s_created.fetch_add(created,std::memory_order_relaxed);
        char m[200]{}; std::snprintf(m,sizeof(m),"V66.54-SVO-RIGHT created=%u RIGHT history textures (slotsInUse=0x%08X)",created,mask); Log(m);
    }
    return mask;
}

bool Exchange(std::uintptr_t stage,std::uintptr_t svo,std::uint32_t mask) noexcept {
    for(int i=0;i<kSlotCount;++i){
        if(!(mask&(1u<<i))) continue;
        const auto a=SlotAddr(i,stage,svo);
        std::uintptr_t live=0;
        if(!Rd(a,live)) return false;
        if(!Wr(a,reinterpret_cast<std::uintptr_t>(g_right[i]))) return false;
        g_right[i]=reinterpret_cast<void*>(live);
    }
    return Swap64(stage+kStagePrevVP,g_prevVP);
}

void Restore(const char* how) noexcept {
    if(!g_swappedIn) return;
    std::uintptr_t svo=0; (void)Rd(g_stage+kStageSvo,svo);
    if(svo!=g_svoSeen) { // object changed under us: textures in the old slots were released by retail
        g_swappedIn=false; ReleaseRightSet("svo-changed-while-swapped"); return;
    }
    if(!Exchange(g_stage,svo,g_swapMask)) { Log("V66.54-SVO-RIGHT RESTORE-FAILED (memory unreadable)"); }
    g_swappedIn=false;
    std::uint32_t cache=0;
    if(g_cache && Rd(reinterpret_cast<std::uintptr_t>(g_cache),cache)) {
        if(cache==g_installFrame) s_rightReproj.fetch_add(1,std::memory_order_relaxed);
        else s_rightNoReproj.fetch_add(1,std::memory_order_relaxed);
    }
    (std::strcmp(how,"end")==0?s_restoreEnd:s_restoreFallback).fetch_add(1,std::memory_order_relaxed);
}

void MaybePeriodicLog() noexcept {
    const auto now=GetTickCount64();
    if(!g_lastLogMs) { g_lastLogMs=now; return; }
    if(now-g_lastLogMs<5000) return;
    g_lastLogMs=now;
    LogStats("5s");
}

std::uintptr_t __fastcall UpdateDetour(void* stagePtr,void* a2,void* a3,void* a4) noexcept {
    const auto stage=reinterpret_cast<std::uintptr_t>(stagePtr);
    std::uintptr_t crv=0;
    const int eye=EyeOf(stage,crv);
    if(eye==0) s_updL.fetch_add(1,std::memory_order_relaxed);
    if(eye==1) s_updR.fetch_add(1,std::memory_order_relaxed);

    // Safety: the RIGHT set must never stay in place outside the RIGHT pass. Only this
    // stage's own next Update counts (other pipelines have their own SVO objects).
    if(g_swappedIn && stage==g_stage && eye!=1) Restore("fallback");

    if(eye==1) {
        if(!g_stage) g_stage=stage;
        if(stage!=g_stage) {
            if(s_otherStage.fetch_add(1,std::memory_order_relaxed)==0) Log("V66.54-SVO-RIGHT note: a second SVO stage ran a RIGHT pass; left retail");
        } else if(!g_enabled.load(std::memory_order_relaxed)) {
            s_disabledR.fetch_add(1,std::memory_order_relaxed);
        } else if(!g_swappedIn) {
            std::uintptr_t svo=0; std::uint32_t frame=0;
            if(!SvoStable(stage,crv,svo)) {
                s_skipRecreate.fetch_add(1,std::memory_order_relaxed); // retail rebuilds SVO now: stay retail this pass
            } else if(Rd(crv+kCrvFrame,frame)) {
                if(g_svoSeen && svo!=g_svoSeen) ReleaseRightSet("svo-recreated");
                g_svoSeen=svo;
                const std::uint32_t mask=Provision(stage,svo);
                if(mask!=~0u && mask!=0u) {
                    if(!g_prevVPValid) { g_prevVPValid=Copy64(stage+kStagePrevVP,g_prevVP); } // first frame: start from LEFT's
                    if(g_prevVPValid && Exchange(stage,svo,mask)) {
                        g_swapMask=mask; g_swappedIn=true; g_installFrame=frame;
                        // Let site B compute the RIGHT eye's own reprojection this pass.
                        if(g_cache) (void)Wr(reinterpret_cast<std::uintptr_t>(g_cache),frame^0x80000000u);
                        if(s_installs.fetch_add(1,std::memory_order_relaxed)==0) {
                            char m[240]{}; std::snprintf(m,sizeof(m),
                                "V66.54-SVO-RIGHT first install stage=%p svo=%p frame=%u slots=0x%08X v6649=%d",
                                stagePtr,reinterpret_cast<void*>(svo),frame,mask,native_svo_prev_per_eye_v6649::Enabled()?1:0);
                            Log(m);
                        }
                    }
                }
            }
        }
    }

    const auto r=g_original?g_original(stagePtr,a2,a3,a4):0;

    // Retail may have destroyed/recreated the SVO object inside Update.
    if(stage==g_stage) {
        std::uintptr_t svoAfter=0; (void)Rd(stage+kStageSvo,svoAfter);
        if(g_svoSeen && svoAfter!=g_svoSeen) {
            if(g_swappedIn) { g_swappedIn=false; ReleaseRightSet("svo-recreated-during-right-update"); }
            else ReleaseRightSet("svo-recreated");
            g_svoSeen=svoAfter;
        }
    }
    MaybePeriodicLog();
    return r;
}

bool ExactBuild(std::uintptr_t b) noexcept {
    if(!b) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(b);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(b+dos->e_lfanew);
        if(nt->Signature!=IMAGE_NT_SIGNATURE || nt->FileHeader.TimeDateStamp!=kTimestamp) return false;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    if(!Matches(b+kUpdateRva,kUpdateEntry,sizeof(kUpdateEntry))) return false;
    if(!Matches(b+kRecreateCheckRva,kRecreateCheck,sizeof(kRecreateCheck))) return false;
    if(!Matches(b+kCreateRtRva,kCreateRtEntry,sizeof(kCreateRtEntry))) return false;
    if(!Matches(b+kReleaseRva,kReleaseEntry,sizeof(kReleaseEntry))) return false;
    if(!Matches(b+kCacheCmpRva,kCacheCmpHead,sizeof(kCacheCmpHead))) return false;
    if(!Matches(b+kCacheStoreRva,kCacheStoreHead,sizeof(kCacheStoreHead))) return false;
    // Both instructions must address the same frame-cache global.
    std::int32_t d1=0,d2=0;
    if(!Rd(b+kCacheCmpRva+5,d1) || !Rd(b+kCacheStoreRva+2,d2)) return false;
    const auto t1=b+kCacheCmpRva+9+static_cast<std::intptr_t>(d1);
    const auto t2=b+kCacheStoreRva+6+static_cast<std::intptr_t>(d2);
    return t1==t2 && t1==b+kCacheGlobalRva;
}

} // namespace

bool Install() noexcept {
    if(g_installed.load(std::memory_order_acquire)) return true;
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!ExactBuild(b)) { Log("V66.54-SVO-RIGHT install REFUSED exact WHGame / static contract mismatch"); return false; }
    g_create=reinterpret_cast<CreateRtFn>(b+kCreateRtRva);
    g_release=reinterpret_cast<ReleaseFn>(b+kReleaseRva);
    g_cache=reinterpret_cast<volatile std::uint32_t*>(b+kCacheGlobalRva);
    DetourModKit::hook::InlineRequest req{
        "kcd2vr_v6654_svo_right_update",
        DetourModKit::hook::Target{DetourModKit::Address{b+kUpdateRva}},
        DetourModKit::hook::Options{}
    };
    auto h=DetourModKit::hook::inline_at(std::move(req),&UpdateDetour);
    if(!h) { g_create=nullptr; g_release=nullptr; g_cache=nullptr; Log("V66.54-SVO-RIGHT install FAILED hook-create"); return false; }
    g_hook=std::move(*h);
    g_original=g_hook->original<UpdateFn>();
    if(!g_original || !g_hook->enable()) {
        g_original=nullptr; g_hook.reset(); g_create=nullptr; g_release=nullptr; g_cache=nullptr;
        Log("V66.54-SVO-RIGHT install FAILED hook-enable"); return false;
    }
    g_installed.store(true,std::memory_order_release);
    Log("V66.54-SVO-RIGHT installed: RIGHT eye gets its own SVO history (30 slots), own reprojection, own previous view-proj; default ON (F8)");
    return true;
}
bool Installed() noexcept { return g_installed.load(std::memory_order_acquire); }
void SetEnabled(bool on) noexcept { g_enabled.store(on,std::memory_order_release); } // applied at the next RIGHT Update
bool Enabled() noexcept { return g_enabled.load(std::memory_order_acquire); }

void OnRightPassEnd() noexcept {
    if(!g_installed.load(std::memory_order_acquire)) return;
    Restore("end");
}

void LogStats(const char* why) noexcept {
    char m[640]{}; std::snprintf(m,sizeof(m),
        "V66.54-SVO-RIGHT stats(%s) enabled=%d updates={L:%llu R:%llu} installs=%llu restores={end:%llu fallback:%llu} "
        "rightReproj={ran:%llu skipped:%llu} texturesCreated=%llu createFail=%llu skipRecreate=%llu resets=%llu "
        "disabledRight=%llu otherStage=%llu slots=0x%08X v6649=%d",
        why?why:"",g_enabled.load()?1:0,
        (unsigned long long)s_updL.load(),(unsigned long long)s_updR.load(),(unsigned long long)s_installs.load(),
        (unsigned long long)s_restoreEnd.load(),(unsigned long long)s_restoreFallback.load(),
        (unsigned long long)s_rightReproj.load(),(unsigned long long)s_rightNoReproj.load(),
        (unsigned long long)s_created.load(),(unsigned long long)s_provisionFail.load(),(unsigned long long)s_skipRecreate.load(),
        (unsigned long long)s_resets.load(),(unsigned long long)s_disabledR.load(),(unsigned long long)s_otherStage.load(),
        g_swapMask,native_svo_prev_per_eye_v6649::Enabled()?1:0);
    diagnostic_trace::Write(m);
}

} // namespace kcd2vr::native_svo_right_v6654
