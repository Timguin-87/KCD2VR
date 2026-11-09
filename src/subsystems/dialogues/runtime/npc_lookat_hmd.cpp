#include "npc_lookat_hmd.h"

#include "camera_hook.h"
#include "diagnostic_trace.h"
#include "frik_pose_probe.h"
#include "npc_target47c1.h"
#include "npc_look_policy63.h"
#include "subsystems/dialogues/lively_npc_d73.h"
#include "subsystems/direct/training_vanilla_r163.h" // R163: no NPC look / life writes in training fights (practice weapon)
#include "native_script_t2.h"
#include "core/snapshot_mailbox.h"
#include "retail_build112.h"
#include "diagnostics/npc_dialogue_capture_d52.h"

#include <DetourModKit/hook.hpp>
#include <intrin.h>
#include <windows.h>

#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <utility>

#include "lively_npc_d73.inl"

namespace kcd2vr::npc_lookat_hmd {
namespace {

constexpr std::uint32_t kSupportedWhTimestamp = 0x6A350E20u;
constexpr std::uintptr_t kResolveLookTargetWorldPosRva = 0x59B0F0ull;

// Retail +59B0F0 takes a target wrapper. Its +18 payload can be an AI/SO
// puppet; +59B348 resolves its engine entity via the exact +602360 getter.
// Never compare a puppet object directly to Henry's CEntity or accept NPCs by distance.
using ResolveLookTargetWorldPosFn = bool(__fastcall*)(void* lookAtOwner, void* targetWrapper, float* outWorldPos);

std::optional<DetourModKit::hook::Hook> g_hook;
ResolveLookTargetWorldPosFn g_original = nullptr;
std::atomic<bool> g_installed{false};
std::atomic<bool> g_retired63{false};
std::atomic<bool> g_enabled{false};
std::atomic<bool> g_dialogue_active{false};
std::atomic<std::uint64_t> g_overrides{0};
std::atomic<std::uint64_t> g_dialogue_calls{0};
diagnostics::npc_dialogue_d52::Budget g_detailD52,g_summaryD52,g_stateF5D52,g_stateF2D52;
std::atomic<std::uintptr_t> g_base63{0}; // exact SHA/entry-qualified once
core::SnapshotMailbox<npc_look_policy63::Owner> g_owner63;


bool SafeReadable(const void* p, std::size_t need=1) noexcept {
    if(!p) return false;
    MEMORY_BASIC_INFORMATION mbi{};
    if(!VirtualQuery(p,&mbi,sizeof(mbi)) || mbi.State!=MEM_COMMIT || (mbi.Protect&(PAGE_NOACCESS|PAGE_GUARD))) return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p);
    const auto end=reinterpret_cast<std::uintptr_t>(mbi.BaseAddress)+mbi.RegionSize;
    return a<=end && need<=end-a;
}

bool ExactWhBuild(std::uintptr_t base) noexcept {
    if(!base) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        return nt->Signature==IMAGE_NT_SIGNATURE && nt->FileHeader.TimeDateStamp==kSupportedWhTimestamp;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

template<class T> bool ReadTarget47c1(void* address,T& out) noexcept {
    if(!address)return false;
    // Already inside the original native target resolver. Fixed scalar reads
    // are fault-contained; no per-target VirtualQuery/world polling is needed.
    __try {std::memcpy(&out,address,sizeof(T));return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool MatchEntry(std::uintptr_t address,const unsigned char* bytes,std::size_t n) noexcept {
    if(!SafeReadable(reinterpret_cast<const void*>(address),n)) return false;
    __try { return std::memcmp(reinterpret_cast<const void*>(address),bytes,n)==0; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool __fastcall ResolveLookTargetWorldPosDetour(void* lookAtOwner,void* targetWrapper,float* outWorldPos) noexcept {
    // INVESTIGATE_D67R1 NPC_VISIBLE_GAZE: this hook substitutes an exact Henry
    // entity target only. Point-target callers bypass it, and a target write
    // does not identify the visible dialogue twin or the final neck/eye pose.
    // Join nativeReturnD57 -> requesting actor -> visible participant -> final
    // animation/look layer before adding a second pose writer. Existing D52
    // receipts explicitly retain these UNPROVEN fields; see OPEN_SOURCE_POINTS.
    if(!g_original)return false;
    // Capture mode before original: off/on during native callbacks cannot lend
    // permission to this older invocation. Native always runs exactly once.
    const auto mode=npc_look_policy63::modeD52.load(std::memory_order_acquire);
    const bool ok=g_original(lookAtOwner,targetWrapper,outWorldPos);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    using diagnostics::capture168::Domain;
    const bool detail=g_detailD52.allow(Domain::Interaction,now,250,1024);
    // Observe the native caller in this detour, never inside the finish lambda.
    // F5 detail only; the retained base was already exact-retail/entry verified.
    const auto nativeReturnD57=detail?reinterpret_cast<std::uintptr_t>(_ReturnAddress()):0;
    const auto whBaseD57=detail?g_base63.load(std::memory_order_acquire):0;
    const bool summary=g_summaryD52.allow(Domain::Bug,now,1000,256);
    npc_look_policy63::Owner owner63{};
    lively_npc_d73::Frame life{};const auto lifeMode=lively_npc_d73::mode.load(std::memory_order_acquire);
    const bool liveDialogue=(lifeMode&1)&&lively_npc_d73::ready.load(std::memory_order_acquire)&&lively_npc_d73::frames.read(life)&&
        lively_npc_d73::Current(life,now,lifeMode)&&life.dialogue&&life.scene;
    void* payload=nullptr;void* playerEntity=nullptr;void* targetEntity=nullptr;
    std::uintptr_t payloadVft=0;
    float hx=0,hy=0,hz=0,ox=0,oy=0,oz=0,delta=-1;
    bool haveNative=false,haveHmd=false,written=false;
    const auto finish=[&](const char* reason) noexcept {
        if(detail){
            char m[1200]{};std::snprintf(m,sizeof(m),
                "D52 F5_DIALOGUE_GAZE tick=%llu sourceTick=%llu scene=%p instance=%p playerSoul=%llu mode=%s modeGeneration=%llu reason=%s nativeOk=%d wrote=%d opaqueLookOwner=%p nativeReturn=0x%llX whBase=0x%llX wrapper=%p payload=%p payloadVft=0x%llX targetHenry=%p resolvedTarget=%p nativeKnown=%d native=(%.3f,%.3f,%.3f) hmdKnown=%d hmd=(%.3f,%.3f,%.3f) delta=%.3f requesterEntity=UNPROVEN visibleTwin=UNPROVEN finalNpcPose=NOT_OBSERVED pointTargets=BYPASS_THIS_HOOK",
                (unsigned long long)now,(unsigned long long)owner63.tick,reinterpret_cast<void*>(owner63.scene),reinterpret_cast<void*>(owner63.instance),
                (unsigned long long)owner63.playerSoul,(mode&1)?"FOLLOW_HMD":"NATIVE",(unsigned long long)(mode>>1),reason,ok,written,
                lookAtOwner,(unsigned long long)nativeReturnD57,(unsigned long long)whBaseD57,targetWrapper,payload,(unsigned long long)payloadVft,playerEntity,targetEntity,haveNative,double(ox),double(oy),double(oz),haveHmd,double(hx),double(hy),double(hz),double(delta));
            diagnostic_trace::Write(m);
        }
        if(summary){char m[280]{};std::snprintf(m,sizeof(m),
            "D52 DIALOGUE_GAZE_SUMMARY reason=%s mode=%s wrote=%d scene=%p sourceTick=%llu substitutions=%llu result=TARGET_ONLY",
            reason,(mode&1)?"FOLLOW_HMD":"NATIVE",written,reinterpret_cast<void*>(owner63.scene),
            (unsigned long long)owner63.tick,(unsigned long long)g_overrides.load(std::memory_order_relaxed));diagnostic_trace::Write(m);}
        return ok;
    };
    if(!(mode&1))return finish("MODE_NATIVE");
    if(!g_enabled.load(std::memory_order_acquire)||!g_installed.load(std::memory_order_acquire)||g_retired63.load(std::memory_order_acquire))return finish("HOOK_DISABLED");
    const bool admittedDialogue=g_dialogue_active.load(std::memory_order_acquire)&&g_owner63.read(owner63)&&npc_look_policy63::fresh(owner63,now);
    if(!admittedDialogue&&!liveDialogue)return finish("NO_CURRENT_ADMITTED_SCENE");
    if(!admittedDialogue)owner63.entity=life.actor;
    g_dialogue_calls.fetch_add(1,std::memory_order_relaxed);
    if(!ok)return finish("NATIVE_RESOLVE_FALSE");
    if(!targetWrapper||!outWorldPos)return finish("BAD_WRAPPER_OR_OUT");
    if(!ReadTarget47c1(static_cast<unsigned char*>(targetWrapper)+0x18,payload)||!payload)return finish("PAYLOAD_UNAVAILABLE");
    playerEntity=frik_pose_probe::LastPlayerEntity();
    if(!playerEntity||reinterpret_cast<std::uintptr_t>(playerEntity)!=owner63.entity)return finish("PLAYER_CHANGED");
    const auto base=g_base63.load(std::memory_order_acquire);
    // D63 exposes the resolver's already-read output in the existing bounded
    // F5 record. No extra native read, hook, target substitution or pose write.
    (void)ReadTarget47c1(payload,payloadVft);
    auto read=[](void* address,auto& value) noexcept {return ReadTarget47c1(address,value);};
    if(!npc_target47c1::Resolve(read,base,payload,playerEntity,targetEntity))return finish("TARGET_NOT_EXACT_HENRY");
    float native[3]{};
    if(!ReadTarget47c1(outWorldPos,native))return finish("NATIVE_POINT_UNREADABLE");
    ox=native[0];oy=native[1];oz=native[2];haveNative=true;
    haveHmd=CameraHook::GetLatestVisualPositionWorld(hx,hy,hz,60)&&std::isfinite(hx)&&std::isfinite(hy)&&std::isfinite(hz);
    if(!haveHmd)return finish("NO_FRESH_HMD_WORLD_POS");
    const float dx=hx-ox,dy=hy-oy,dz=hz-oz;delta=std::sqrt(dx*dx+dy*dy+dz*dz);
    if(!std::isfinite(delta))return finish("NONFINITE_DELTA");
    if(delta>2.f)return finish("DELTA_GT_2M");
    npc_look_policy63::Owner current{};lively_npc_d73::Frame lifeCurrent{};
    const auto finalNow=std::uint64_t(GetTickCount64());
    const bool ownerCurrent=admittedDialogue?
        (g_dialogue_active.load(std::memory_order_acquire)&&g_owner63.read(current)&&npc_look_policy63::same(owner63,current)&&npc_look_policy63::fresh(current,finalNow)):
        (lively_npc_d73::mode.load(std::memory_order_acquire)==lifeMode&&lively_npc_d73::frames.read(lifeCurrent)&&
         lively_npc_d73::Current(lifeCurrent,finalNow,lifeMode)&&lifeCurrent.dialogue&&lifeCurrent.scene&&lifeCurrent.actor==life.actor&&lifeCurrent.scene==life.scene&&lifeCurrent.tick==life.tick);
    if(npc_look_policy63::modeD52.load(std::memory_order_acquire)!=mode||!g_enabled.load(std::memory_order_acquire)||
       !g_installed.load(std::memory_order_acquire)||g_retired63.load(std::memory_order_acquire)||!ownerCurrent||
       frik_pose_probe::LastPlayerEntity()!=playerEntity)return finish("FINAL_OWNER_OR_MODE_CHANGED");
    {static std::atomic<std::uintptr_t> sceneR163{0}; // R163: training fight (practice weapon) = the native look point stays
     if(kcd2vr::training_vanilla_r163::skipOnce(kcd2vr::training_vanilla_r163::DialogueLook,sceneR163,owner63.scene?owner63.scene:life.scene))return finish("TRAINING_VANILLA_R163");}
    __try {outWorldPos[0]=hx;outWorldPos[1]=hy;outWorldPos[2]=hz;}
    __except(EXCEPTION_EXECUTE_HANDLER){return finish("WRITE_FAILED");}
    written=true;g_overrides.fetch_add(1,std::memory_order_relaxed);
    return finish("APPLIED");
}

} // namespace

bool Install() noexcept {
    if(g_retired63.load(std::memory_order_acquire))return false;
    if(g_installed.load(std::memory_order_acquire)) return true;
    const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    static constexpr unsigned char head[]={0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x30};
    static constexpr unsigned char entityGetter[]={0x48,0x8B,0x41,0x38,0xC3};
    if(!ExactWhBuild(base) || !retail_build112::Verify(base) ||
       !MatchEntry(base+0x602360,entityGetter,sizeof(entityGetter)) ||
       !MatchEntry(base+kResolveLookTargetWorldPosRva,head,sizeof(head))){
        diagnostic_trace::Write("V65.10-F5-HMD-LOOK install refused: exact WHGame build/entry mismatch");
        return false;
    }
    // A retained/in-flight trampoline must never point into an unloaded mod.
    // Pin by this exact callback address on the explicit startup thread.
    HMODULE module63=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&ResolveLookTargetWorldPosDetour),&module63)||!module63)return false;
    DetourModKit::hook::InlineRequest req{
        "kcd2vr_v6510_dialogue_player_look_point",
        DetourModKit::hook::Target{DetourModKit::Address{base+kResolveLookTargetWorldPosRva}},
        DetourModKit::hook::Options{}};
    auto r=DetourModKit::hook::inline_at(std::move(req),&ResolveLookTargetWorldPosDetour);
    if(!r){ diagnostic_trace::Write("V65.10-F5-HMD-LOOK hook install failed"); return false; }
    g_hook=std::move(*r);
    g_original=g_hook->original<ResolveLookTargetWorldPosFn>();
    if(!g_original){g_hook.reset();return false;}
    g_base63=base;
    if(!g_hook->enable()){
        // A failed enable is not proof that no callback can be reachable.
        // Retain the exact handle/original and prohibit another install.
        g_retired63.store(true,std::memory_order_release);
        g_enabled.store(false,std::memory_order_release);
        diagnostic_trace::Write("V65.10-F5-HMD-LOOK hook enable failed");
        return false;
    }
    g_installed.store(true,std::memory_order_release);
    npc_look_policy63::hookReadyD52.store(true,std::memory_order_release);
    // D81: load/save preserves the user's setting before hooks are available.
    // Prepare only after this exact-build hook has its original and is enabled.
    // A failed optional observer leaves native dialogue/head target handling intact.
    if(lively_npc_d73::Enabled()&&!lively_npc_d73::Prepare())
        diagnostic_trace::Write("D81 NPC listening observers unavailable; saved setting retained; native dialogue continues");
    diagnostic_trace::Write("C63 DIALOGUE_HMD_LOOK installed exact +59B0F0; native original first; input-admitted current dialogue scene; Native/FollowHMD live policy; exact Henry entity target only; no AI tasks or participant animation control");
    return true;
}

void Uninstall() noexcept {
    lively_npc_d73::Retire();
    npc_look_policy63::hookReadyD52.store(false,std::memory_order_release);
    npc_look_policy63::sceneAdmittedD52.store(false,std::memory_order_release);
    g_enabled.store(false,std::memory_order_release);
    g_dialogue_active.store(false,std::memory_order_release);
    (void)g_owner63.publish({});
    if(g_hook){
        g_retired63.store(true,std::memory_order_release);
        (void)g_hook->disable();
        // Disable does not drain native callback frames. Both successful and
        // failed disarm retain the original trampoline/handle for this pinned
        // module's process lifetime; no second install may replace them.
    }
    g_installed.store(false,std::memory_order_release);
}

void SetEnabled(bool enabled) noexcept {
    g_enabled.store(enabled&&g_installed.load(std::memory_order_acquire)&&
        !g_retired63.load(std::memory_order_acquire),std::memory_order_release);
}
void SetDialogueActive(bool active) noexcept { g_dialogue_active.store(active,std::memory_order_release); }
bool Enabled() noexcept { return g_enabled.load(std::memory_order_acquire); }
void PublishDialogueOwner63(const dialogue_state59c10r2::State& state,bool /*embodied*/) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    npc_look_policy63::Owner previous{};
    if(g_dialogue_active.load(std::memory_order_acquire))(void)g_owner63.read(previous);
    const auto result=npc_look_policy63::capture(state,previous,now);
    const bool published=g_owner63.publish(result.owner);
    const bool admitted=published&&result.owner.entity!=0;
    g_dialogue_active.store(admitted,std::memory_order_release);
    npc_look_policy63::sceneTickD52.store(state.tick,std::memory_order_release);
    npc_look_policy63::sceneAdmittedD52.store(admitted,std::memory_order_release);
    using diagnostics::capture168::Domain;
    const bool f5=g_stateF5D52.allow(Domain::Interaction,now,250,1024);
    const bool f2=g_stateF2D52.allow(Domain::Bug,now,1000,256);
    if(f5||f2){char m[700]{};std::snprintf(m,sizeof(m),
        "D52 DIALOGUE_GAZE_STATE tick=%llu sourceTick=%llu scene=%p instance=%p playerSoul=%llu admitted=%d reason=%s published=%d nativeMembership=%d inputKnown=%d inputEnabled=%d gameplayDialogueActive=%d camera=%u mode=%s scope=TARGET_SUBSTITUTION_ONLY",
        (unsigned long long)now,(unsigned long long)state.tick,reinterpret_cast<void*>(state.membership.sceneD52),reinterpret_cast<void*>(state.instance),
        (unsigned long long)state.membership.playerSoulD52,admitted,npc_look_policy63::name(result.reason),published,state.membership.active,
        state.inputOwner.known,state.inputOwner.enabled,state.active,unsigned(state.camera),npc_look_policy63::followHmd()?"FOLLOW_HMD":"NATIVE");diagnostic_trace::Write(m);}
}

} // namespace kcd2vr::npc_lookat_hmd
