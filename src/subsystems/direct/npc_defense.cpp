#include "npc_defense.h"
#include <array>
#include <cmath>
#include <cstring>
#include <mutex>
#include <optional>
#include <utility>
#include "vrik/motion_owned59c4.h"
#if !defined(KCD2VR_NPC_DEFENSE_HOST_TEST)
#include <Windows.h>
#include <intrin.h>
#include <DetourModKit/hook.hpp>
#include "retail_build112.h"
#else
#include <DetourModKit/hook.hpp>
#endif

namespace kcd2vr::npc_defense {
namespace {
using Address=std::uintptr_t;
using EnterFn=bool(__fastcall*)(void*);
using RateFn=void(__fastcall*)(void*,float);
std::atomic<Address> image{0};
std::atomic<bool> ready{false};
std::atomic<const char*> failure{"NOT_REQUESTED"};
std::atomic<EnterFn> enterOriginal{nullptr};
std::atomic<RateFn> rateOriginal{nullptr};

bool Copy(Address at,void* value,std::size_t size) noexcept {
#if defined(KCD2VR_NPC_DEFENSE_HOST_TEST)
    return HostNpcDefenseRead(at,value,size);
#else
    if(!at||!value)return false;
    __try {std::memcpy(value,reinterpret_cast<const void*>(at),size);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
#endif
}
template<class T> bool Read(Address at,T& value) noexcept {return at&&Copy(at,&value,sizeof(value));}
bool Equal(Address at,Address value) noexcept {Address actual=0;return Read(at,actual)&&actual==value;}
// R146: the defender's own best melee skill (sword/heavy/large/dagger/unarmed), the exact read C39
// OpponentSkill39 uses: combat actor +1F8 -> C_Human (+278 links back) -> +668 C_Soul (vft 3F44F28),
// base skill id i at soul+5E8+8*i, as the pinned native getter 1123C34 reads it. -1 = unread (-> Low).
int ActorSkill(Address actor) noexcept {
    static constexpr unsigned char getter[]{0x83,0xFA,0x23,0x73,0x0B,0x8B,0xC2,0x8B,0x84,0xC1,0xE8,0x05,0x00,0x00};
    static constexpr int ids[5]{16,17,23,22,24};
    const auto b=image.load(std::memory_order_acquire);
    std::array<unsigned char,sizeof(getter)> code{};Address human=0,soul=0;
    if(!b||!actor||!Copy(b+0x1123C4B,code.data(),code.size())||std::memcmp(code.data(),getter,sizeof(getter)))return -1;
    if(!Read(actor+0x1F8,human)||!human||!Equal(human+0x278,actor)||!Read(human+0x668,soul)||!soul||!Equal(soul,b+0x3F44F28))return -1;
    int best=-1;
    for(const int id:ids){int v=-1;if(Read(soul+0x5E8+8*Address(id),v)&&v>=0&&v<=40&&v>best)best=v;}
    return best;
}
// Per-defender factors. BY SKILL OFF: exactly the old global factors.
float DodgeFor(Address actor) noexcept {
    return bySkill.load(std::memory_order_acquire)?SkillFactor(ActorSkill(actor)):DodgeFactor();
}
float RaiseFor(Address actor,bool parry) noexcept {
    if(!bySkill.load(std::memory_order_acquire))return RaiseFactor(parry);
    // VR DIRECT route (F4 press 3 / motion preview): C39 already shortened Henry's announced +450 by this
    // same skill factor before the NPC reads it. Do not apply it a second time here.
    if(kcd2vr::motion59c4::selected.load(std::memory_order_acquire))return 1.f;
    return SkillFactor(ActorSkill(actor));
}

struct Owner {Address actor=0,state=0,action=0,manager=0,timing=0;};
// Fresh identity of a native NPC at the callback. No cross-frame borrowed
// action/actor lease is kept and no player-input decision calls this policy.
bool Actor(Address actor,Owner& out) noexcept {
    const auto b=image.load(std::memory_order_acquire);
    out={};out.actor=actor;
    return b&&Equal(actor,b+0x3E3FAC8)&&Read(actor+0x210,out.state)&&out.state&&
        Equal(out.state+0xCE0,actor)&&Read(actor+0x200,out.manager)&&out.manager&&
        Read(actor+0x3C0,out.timing)&&out.timing&&Equal(out.timing+8,actor);
}
bool CurrentAction(Owner& owner) noexcept {
    Address first=0,end=0;
    return Read(owner.manager+0x38,first)&&Read(owner.manager+0x40,end)&&first&&end>first&&
        Read(first,owner.action)&&owner.action&&Equal(owner.action+0x40,owner.manager);
}
bool RaiseOwner(Address timing,Address opponent,Address property,Owner& owner) noexcept {
    const auto b=image.load(std::memory_order_acquire);Address actor=0,incoming=0,henryState=0,vft=0;
    int phase=0;
    if(!Read(timing+8,actor)||!Actor(actor,owner)||owner.timing!=timing||
       !Read(owner.state+0x38,phase)||phase!=128||!CurrentAction(owner)||
       !Read(owner.action,vft))return false;
    if(vft==b+0x3A89688){if(!Equal(owner.action+0x68,actor))return false;}
    else if(vft==b+0x3BE12C0){if(!Equal(owner.action+0x88,actor))return false;}
    else return false; // The separate SyncPerfectBlock classes are not this owner.
    // Ordinary PB can itself contain a counterattack later in its clip. Only
    // the native phase128 block-raise prefix is retimed here; no resolved-row
    // contains_attack flag or paired attacker ownership is being invented.
    // Both native call sites read the opponent's original +450 property. This
    // is a private read view for the current defender; Henry is never written.
    if(!Read(owner.state+0xCF8,incoming)||!incoming||incoming==actor||
       (opponent&&incoming!=opponent)||!Equal(incoming,b+0x3A80610)||
       !Read(incoming+0x210,henryState)||!henryState||!Equal(henryState+0xCE0,incoming)||
       property!=henryState+0x450||!Equal(property,b+0x3B0A790))return false;
    return true;
}
struct alignas(16) PropertyView {std::array<unsigned char,16> bytes{};};
thread_local PropertyView initialView,followView;
Address EtaView(Address timing,Address opponent,Address property,Address vft,PropertyView& view) noexcept {
    const auto b=image.load(std::memory_order_acquire);
    if(!ready.load(std::memory_order_acquire)||vft!=b+0x3B0A790)return property;
    Owner owner{};float original=0;Address actionVft=0;
    if(!RaiseOwner(timing,opponent,property,owner)||!Read(owner.action,actionVft))return property;
    const float factor=RaiseFor(owner.actor,actionVft==b+0x3BE12C0);
    if(factor==1.f||!Copy(property,view.bytes.data(),view.bytes.size()))return property;
    std::memcpy(&original,view.bytes.data()+8,sizeof(original));
    if(!std::isfinite(original)||original<=0)return property;
    const float shortened=original/factor;
    std::memcpy(view.bytes.data()+8,&shortened,sizeof(shortened));
    // The next instruction is the pinned native pure getter: movss [rcx+8];
    // ret. Only RCX changes, for that immediate call. DMK XMM views are readonly.
    return reinterpret_cast<Address>(&view);
}
void InitialEta(DetourModKit::hook::MidContext& ctx) noexcept {
    using namespace DetourModKit::hook;
    auto& property=gpr(ctx,Gpr::Rcx);
    property=EtaView(gpr(ctx,Gpr::Rdi),gpr(ctx,Gpr::Rbx),property,gpr(ctx,Gpr::Rax),initialView);
}
void FollowEta(DetourModKit::hook::MidContext& ctx) noexcept {
    using namespace DetourModKit::hook;
    auto& property=gpr(ctx,Gpr::Rcx);
    property=EtaView(gpr(ctx,Gpr::Rbx),0,property,gpr(ctx,Gpr::Rax),followView);
}

bool DodgeBoundary(Address action,Owner& owner) noexcept {
    const auto b=image.load(std::memory_order_acquire);Address actor=0,animation=0;
    int phase=0;
    return Equal(action,b+0x3B92428)&&Read(action+0x88,actor)&&Actor(actor,owner)&&
        Equal(action+0x40,owner.manager)&&Read(owner.state+0x38,phase)&&phase==512&&
        Read(actor+0x3A0,animation)&&animation&&Equal(animation,actor)&&
        (owner.action=action,true);
}
bool CurrentDodge(Address animation,Owner& owner) noexcept {
    Address actor=0,current=0;
    if(!Read(animation,actor)||!Actor(actor,owner)||!Equal(actor+0x3A0,animation)||
       !CurrentAction(owner))return false;
    current=owner.action;
    return DodgeBoundary(current,owner);
}
bool CurrentRate(Address actor,float& rate) noexcept {
    Address timing=0;
    // Exact source of the native follower's requested D4E2FC rate at9E3D27.
    // Unlike controller+9C readback this field is never multiplied by us.
    return Read(actor+0x3C0,timing)&&timing&&Equal(timing+8,actor)&&
        Read(timing+0x10,rate)&&std::isfinite(rate)&&rate>0;
}
struct EnterScope {Address action=0,actor=0;bool rateApplied=false;EnterScope* previous=nullptr;};
thread_local EnterScope* entering=nullptr;
struct ScopeGuard {EnterScope* old;explicit ScopeGuard(EnterScope& next):old(entering){next.previous=old;entering=&next;}
    ~ScopeGuard(){entering=old;}};
// Read-only caller identity on the native setter is important: its known Exit
// restore and all unrelated rate writers must remain exactly native.
void ApplyRate(void* animation,float requested,Address returnPc) {
    float applied=requested;Owner owner{};const auto b=image.load(std::memory_order_acquire);
    const auto at=reinterpret_cast<Address>(animation);float factor=1.f;
    if(ready.load(std::memory_order_acquire)&&Wanted()&&std::isfinite(requested)&&requested>0){
        if(returnPc==b+0x1648C08&&entering&&DodgeBoundary(entering->action,owner)&&
           owner.actor==entering->actor&&Equal(owner.actor+0x3A0,at)){
            if((factor=DodgeFor(owner.actor))>1.f){applied=requested*factor;entering->rateApplied=true;}
        }else if(returnPc==b+0x9E3D38&&CurrentDodge(at,owner)){
            if((factor=DodgeFor(owner.actor))>1.f){applied=requested*factor;
            if(entering&&entering->action==owner.action&&entering->actor==owner.actor)entering->rateApplied=true;}
        }
    }
    // Native follower supplies a new RAW working rate, not the last controller
    // readback. Scale each request once. Exit19BE26B is deliberately untouched.
    if(const auto original=rateOriginal.load(std::memory_order_acquire))original(animation,applied);
}
void __fastcall SetRate(void* animation,float requested) {
#if defined(KCD2VR_NPC_DEFENSE_HOST_TEST)
    const Address pc=HostNpcDefenseReturnAddress();
#else
    const Address pc=reinterpret_cast<Address>(_ReturnAddress());
#endif
    ApplyRate(animation,requested,pc);
}
bool __fastcall DodgeEnter(void* action) {
    const auto original=enterOriginal.load(std::memory_order_acquire);
    if(!ready.load(std::memory_order_acquire))return original?original(action):false;
    const auto self=reinterpret_cast<Address>(action);Address actor=0;
    (void)Read(self+0x88,actor);Owner before{};Address priorAction=0;
    if(Actor(actor,before)&&CurrentAction(before))priorAction=before.action;
    EnterScope scope{self,actor,false,nullptr};ScopeGuard guard(scope);
    // This is the native acceptance boundary. It is never vetoed or retried by
    // this feature, including Henry, unknown owners, false results or throws.
    const bool accepted=original?original(action):false;
    float factor=1.f;Owner owner{};float nativeRate=0;Address animation=0;
    if(accepted&&ready.load(std::memory_order_acquire)&&Wanted()&&!scope.rateApplied&&
       DodgeBoundary(self,owner)&&(factor=DodgeFor(owner.actor))>1.f&&CurrentRate(owner.actor,nativeRate)&&Read(owner.actor+0x3A0,animation)&&animation){
        // Enter is native ownership even before channel installation. If a
        // synchronous callback already installed a DIFFERENT action, however,
        // this old entry must not change that replacement's animation rate.
        Owner installed=owner;
        if(!CurrentAction(installed)||installed.action==self||installed.action==priorAction)
            if(const auto rate=rateOriginal.load(std::memory_order_acquire))rate(reinterpret_cast<void*>(animation),nativeRate*factor);
    }
    return accepted;
}

struct Pin {Address rva;const char* bytes;std::size_t size;};
constexpr Pin pins[]{
    {0xD4D62F,"\x48\x8B\x01\xFF\x50\x08\x80\x7F\x2D\x00\x44\x0F\x28\xD0",14},
    {0x9E3D84,"\x48\x8B\x01\xFF\x50\x08\x0F\x28\xD0\x48\x85\xFF",12},
    {0x9E3D23,"\x48\x8B\x4B\x08\xF3\x0F\x10\x4B\x10\x48\x8B\x89\xA0\x03\x00\x00\xE8\xC4\xA5\x36\x00",21},
    {0x1A7DE00,"\xF3\x0F\x10\x41\x08\xC3",6},
    {0x1648A28,"\x48\x89\x5C\x24\x10\x48\x89\x74\x24\x18\x55\x57\x41\x56\x48\x8B\xEC",17},
    {0x1648BEC,"\xF3\x0F\x10\x8B\x8C\x00\x00\x00\x48\x8B\x0E\xF3\x0F\x5E\x48\x70\x48\x8B\x89\xA0\x03\x00\x00\xE8\xF4\x56\x70\xFF",28},
    {0x19BE24D,"\xF3\x0F\x10\x0D\xEF\xBC\x6D\x02\x48\x8B\xD9\x48\x8B\x89\x88\x00\x00\x00\x48\x8B\x89\xA0\x03\x00\x00\xE8\x91\x00\x39\xFF",30},
    {0xD4E2FC,"\x48\x83\xEC\x38\x48\x8B\x01\x0F\x29\x74\x24\x20\x0F\x28\xF1",15},
    {0xD4E30B,"\x48\x8B\x88\xF8\x01\x00\x00\x48\x8B\x01\xFF\x90\xD0\x02\x00\x00\x48\x8B\x08\x48\x8B\x91\x30\x01\x00\x00\x48\x8B\xC8\xFF\xD2",31},
    {0xD4E397,"\x48\x8B\x5C\x24\x40\x48\x8B\x74\x24\x48\xF3\x0F\x11\xB7\x9C\x00\x00\x00",18},
    {0x1A73130,"\xF3\x0F\x10\x81\x9C\x00\x00\x00\xC3",9}
};
bool Contract(Address b) noexcept {
    std::array<unsigned char,40> actual{};
    for(const auto& pin:pins)if(!Copy(b+pin.rva,actual.data(),pin.size)||std::memcmp(actual.data(),pin.bytes,pin.size))return false;
    return Equal(b+0x3B0A798,b+0x1A7DE00)&&Equal(b+0x3B925F0,b+0x1648A28)&&
        Equal(b+0x3B925F8,b+0x19BE238)&&Equal(b+0x4051AC0,b+0xD4E34C);
}
#if !defined(KCD2VR_NPC_DEFENSE_HOST_TEST)
std::mutex controlMutex;
std::optional<DetourModKit::hook::Hook> initialHook,followHook,enterHook,rateHook;
void DisableHooks() noexcept {
    ready.store(false,std::memory_order_release);
    if(initialHook)(void)initialHook->disable();
    if(followHook)(void)followHook->disable();
    if(enterHook)(void)enterHook->disable();
    if(rateHook)(void)rateHook->disable();
    // Keep handles and originals alive: a callback may already be inside its
    // native original. Disabled callbacks are native passthrough, never false
    // acceptance or a null rate route. No trampoline is reclaimed here.
}
#endif
}
bool Ready() noexcept {return ready.load(std::memory_order_acquire);}
const char* Failure() noexcept {return failure.load(std::memory_order_acquire);}
bool Install(Address b) noexcept {
#if defined(KCD2VR_NPC_DEFENSE_HOST_TEST)
    (void)b;return false; // Host fixture invokes the real callbacks, not a fake native patch.
#else
    std::lock_guard<std::mutex> lock(controlMutex);
    if(Ready())return image.load()==b;
    if(initialHook||followHook||enterHook||rateHook){
        if(image.load()!=b||!initialHook||!followHook||!enterHook||!rateHook){failure="RETAINED_HOOK_RESTART_REQUIRED";return false;}
        if(!rateHook->enable()||!enterHook->enable()||!initialHook->enable()||!followHook->enable()){
            failure="NATIVE_REENABLE_FAILED";DisableHooks();return false;
        }
        ready.store(true,std::memory_order_release);failure="NONE";return true;
    }
    if(!b||!retail_build112::Ready(b)){failure="RETAIL_IDENTITY";return false;}
    if(!Contract(b)){failure="EXACT_NATIVE_PINS";return false;}
    using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
    auto first=mid_at(MidRequest{"kcd2vr_npc_raise_eta_initial",Target{DetourModKit::Address{b+0xD4D632}},options},InitialEta);
    if(!first){failure="INITIAL_MIDHOOK_UNAVAILABLE";return false;}
    auto follow=mid_at(MidRequest{"kcd2vr_npc_raise_eta_follow",Target{DetourModKit::Address{b+0x9E3D87}},options},FollowEta);
    if(!follow){failure="FOLLOW_MIDHOOK_UNAVAILABLE";return false;}
    auto enter=inline_at(InlineRequest{"kcd2vr_npc_dodge_enter",Target{DetourModKit::Address{b+0x1648A28}},options},DodgeEnter);
    auto rate=inline_at(InlineRequest{"kcd2vr_npc_dodge_native_rate",Target{DetourModKit::Address{b+0xD4E2FC}},options},SetRate);
    if(!enter||!rate){failure="DODGE_INLINE_UNAVAILABLE";return false;}
    auto originalEnter=enter->original<EnterFn>();auto originalRate=rate->original<RateFn>();
    if(!originalEnter||!originalRate){failure="NATIVE_TRAMPOLINE_UNAVAILABLE";return false;}
    initialHook=std::move(*first);followHook=std::move(*follow);enterHook=std::move(*enter);rateHook=std::move(*rate);
    image.store(b,std::memory_order_release);enterOriginal=originalEnter;rateOriginal=originalRate;
    if(!rateHook->enable()||!enterHook->enable()||!initialHook->enable()||!followHook->enable()){
        failure="NATIVE_ENABLE_FAILED";DisableHooks();return false;
    }
    ready.store(true,std::memory_order_release);failure="NONE";return true;
#endif
}
void Shutdown() noexcept {
#if defined(KCD2VR_NPC_DEFENSE_HOST_TEST)
    ready=false;
#else
    std::lock_guard<std::mutex> lock(controlMutex);DisableHooks();
#endif
}
}
