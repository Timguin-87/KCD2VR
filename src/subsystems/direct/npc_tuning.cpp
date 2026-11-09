#include "npc_tuning.h"
#include "training_vanilla_r163.h" // R163: native AI inputs / spacing in training fights (practice weapon)
#include "retail_build112.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <intrin.h>
#include <DetourModKit/hook.hpp>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <mutex>
#include <optional>
#include <utility>

namespace kcd2vr::npc_difficulty {
namespace {
using Getter=void*(__fastcall*)();
constexpr std::uintptr_t getterRva=0x649D30,constantsRva=0x55E63D0,constantsVft=0x3F18BD0;
// Full/cold branches of these six readers use scalar members only, never
// retain/pass the Constants pointer. D08+float is the largest observed read.
constexpr std::size_t extent=0xD0C;
struct Field {std::size_t offset;float c8;};
constexpr std::array<Field,10> fields{{
    {0xC8C,.8f},{0xCA4,2.f},
    {0xCC8,1.f},{0xCCC,2.f},{0xCD0,2.f},{0xCD4,3.f},
    {0xCE0,.5f},{0xCE4,1.f},{0xC60,3.f},{0xD00,.6f}}};
struct Reader {std::uintptr_t call;std::size_t first,count;};
constexpr std::array<Reader,6> readers{{
    {0x72C0D9,0,1},{0x4C7362,1,1},{0x9130BD,2,6},
    {0x2CF7945,8,1},{0xADBBB1,9,1},{0x122478D,1,1}}};
struct alignas(16) Shadow {std::array<unsigned char,extent> bytes{};};
// Independent sites and threads cannot overwrite another reader's snapshot.
// The native getter itself exposes one mutable global; each invocation here
// deliberately samples its current native contents, without changing them.
thread_local std::array<Shadow,readers.size()> shadows;
std::atomic<Getter> original{nullptr};
std::atomic<std::uintptr_t> module{0};
std::atomic<bool> ready{false};
std::optional<DetourModKit::hook::Hook> hook;

bool Match(std::uintptr_t base,std::uintptr_t rva,const unsigned char* bytes,std::size_t n) noexcept {
    __try {return std::memcmp(reinterpret_cast<const void*>(base+rva),bytes,n)==0;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool CopyCurrent(void* current,std::uintptr_t base,Shadow& shadow) noexcept {
    __try {
        // This is the exact module-static native object returned by649D30,
        // not the separate table baseline532E588 or an inferred stack node.
        if(reinterpret_cast<std::uintptr_t>(current)!=base+constantsRva||
           *static_cast<const std::uintptr_t*>(current)!=base+constantsVft)return false;
        std::memcpy(shadow.bytes.data(),current,extent);return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void* ForReader(void* current,std::uintptr_t returnAddress) noexcept {
    const auto amount=difficulty.load(std::memory_order_relaxed);
    if(!(amount>0.f)||!ready.load(std::memory_order_acquire))return current;
    {static std::atomic<std::uintptr_t> onceR163{0}; // R163: training fight = the native constants object (global, counted once per period)
     if(kcd2vr::training_vanilla_r163::skipOnce(kcd2vr::training_vanilla_r163::NpcDifficulty,onceR163,0))return current;}
    const auto base=module.load(std::memory_order_acquire);
    if(!base||returnAddress<base)return current;
    const auto rva=returnAddress-base;
    for(std::size_t site=0;site<readers.size();++site){const auto& reader=readers[site];
        if(rva!=reader.call+5)continue;
        auto& shadow=shadows[site];if(!CopyCurrent(current,base,shadow))return current;
        const float weight=amount>=1.f?1.f:amount;
        for(std::size_t i=reader.first;i<reader.first+reader.count;++i){const auto& field=fields[i];
            float native=0;std::memcpy(&native,shadow.bytes.data()+field.offset,sizeof(native));
            const float value=weight==1.f?field.c8:native+(field.c8-native)*weight;
            std::memcpy(shadow.bytes.data()+field.offset,&value,sizeof(value));
        }
        return shadow.bytes.data();
    }
    return current;
}
__declspec(noinline) void* __fastcall Get() {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const auto native=original.load(std::memory_order_acquire);
    return native?ForReader(native(),caller):nullptr;
}
bool Pins(std::uintptr_t base) noexcept {
    constexpr unsigned char entry[]{0x48,0x83,0xEC,0x28,0x8B,0x0D,0x62,0xE8,0xA7,0x04,
        0x65,0x48,0x8B,0x04,0x25,0x58,0,0,0,0xBA,0x18,0,0,0};
    if(!Match(base,getterRva,entry,sizeof(entry)))return false;
    // Exact original direct CALLs into the proven no-argument getter. These
    // checks run once at activation, not as permission checks on game actions.
    constexpr std::array<std::array<unsigned char,5>,6> calls{{
        {{0xE8,0x52,0xDC,0xF1,0xFF}},{{0xE8,0xC9,0x29,0x18,0x00}},
        {{0xE8,0x6E,0x6C,0xD3,0xFF}},{{0xE8,0xE6,0x23,0x95,0xFD}},
        {{0xE8,0x7A,0xE1,0xB6,0xFF}},{{0xE8,0x9E,0x55,0x42,0xFF}}}};
    for(std::size_t i=0;i<readers.size();++i)
        if(!Match(base,readers[i].call,calls[i].data(),calls[i].size()))return false;
    return true;
}
} // namespace

bool Ready() noexcept {return ready.load(std::memory_order_acquire);}
bool Install(std::uintptr_t base) noexcept {
    if(Ready())return module.load(std::memory_order_acquire)==base;
    if(!base||!retail_build112::Verify(base))return false;
    if(hook){
        if(module.load(std::memory_order_acquire)!=base||!original.load(std::memory_order_acquire))return false;
        if(!hook->is_enabled()&&!hook->enable())return false;
        if(!hook->is_enabled())return false;
        ready.store(true,std::memory_order_release);return true;
    }
    if(!Pins(base))return false;
    DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
    auto candidate=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{
        "npc_c8_readonly_formula_inputs",DetourModKit::hook::Target{DetourModKit::Address{base+getterRva}},options},&Get);
    if(!candidate)return false;
    const auto native=candidate->original<Getter>();if(!native)return false;
    hook=std::move(*candidate);original.store(native,std::memory_order_release);module.store(base,std::memory_order_release);
    if(!hook->enable()||!hook->is_enabled()){if(hook->is_enabled())(void)hook->disable();return false;}
    ready.store(true,std::memory_order_release);return true;
}
void Shutdown() noexcept {
    ready.store(false,std::memory_order_release);
    // Retain the trampoline handle through teardown: an already-entered native
    // reader may still be returning. A retained/failed disarm routes original.
    if(hook&&hook->is_enabled())(void)hook->disable();
}
}

namespace kcd2vr::npc_unarmed_spacing {
namespace {
using Address=std::uintptr_t;
constexpr Address sampleLoads=0x207967C;
std::atomic<Address> image{0};
std::atomic<bool> ready{false};
std::atomic<const char*> failure{"NOT_REQUESTED"};
std::optional<DetourModKit::hook::Hook> hook;
std::mutex control;

bool Copy(Address at,void* value,std::size_t bytes) noexcept {
    if(!at||!value)return false;
    __try {std::memcpy(value,reinterpret_cast<const void*>(at),bytes);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class T> bool Read(Address at,T& value) noexcept {return Copy(at,&value,sizeof(value));}
bool Equal(Address at,Address expected) noexcept {Address value=0;return Read(at,value)&&value==expected;}
float Factor() noexcept {
    const float value=scale.load(std::memory_order_relaxed);
    return std::isfinite(value)&&value>0.f?value:1.f;
}
Address CombatActor(Address entity,Address base) noexcept {
#if defined(KCD2VR_NPC_SPACING_HOST_TEST)
    return HostSpacingCombatActor(entity);
#else
    __try {return reinterpret_cast<Address(__fastcall*)(void*)>(base+0x3EF4B0)(reinterpret_cast<void*>(entity));}
    __except(EXCEPTION_EXECUTE_HANDLER){return 0;}
#endif
}
Address WeaponClass(Address equipment,Address base) noexcept {
#if defined(KCD2VR_NPC_SPACING_HOST_TEST)
    return HostSpacingWeaponClass(equipment+0x18);
#else
    // The native effective-class getter checks the other selected hand when
    // the primary is class12. No mod weapon/empty-hand cache is consulted.
    __try {return reinterpret_cast<Address(__fastcall*)(void*)>(base+0x9D33F8)(reinterpret_cast<void*>(equipment+0x18));}
    __except(EXCEPTION_EXECUTE_HANDLER){return 0;}
#endif
}
struct alignas(16) View {
    std::array<unsigned char,0x28> unused{};
    float activity=0,inner=0,outer=0;
};
static_assert(offsetof(View,activity)==0x28&&offsetof(View,inner)==0x2C&&offsetof(View,outer)==0x30);
thread_local View view;

Address ReadView(Address move,Address capture,Address nativeNpc) noexcept {
    const float factor=Factor();
    if(factor==1.f||!ready.load(std::memory_order_acquire))return move;
    const Address base=image.load(std::memory_order_acquire);
    Address capturedMove=0,capturedNpc=0,owner=0,current=0,entity=0,equipment=0,row=0;
    if(!base||!Equal(move,base+0x3A836A0)||
       !Read(capture,capturedMove)||capturedMove!=move||
       !Read(capture+8,capturedNpc)||capturedNpc!=nativeNpc||
       !Read(move+0x20,owner)||owner!=nativeNpc||!Equal(owner,base+0x3FF1B18)||
       !Read(owner+0x640,current)||current!=move||
       !Read(owner+0x638,entity)||!entity)return move;
    const Address actor=CombatActor(entity,base);
    if(!actor||!Equal(actor,base+0x3E3FAC8)||
       !Read(actor+0x3E8,equipment)||!equipment||!Equal(equipment+8,actor))return move;
    row=WeaponClass(equipment,base);std::uint32_t id=0;
    if(!row||!Read(row+8,id)||id!=12)return move;
    {static std::atomic<std::uintptr_t> npcR163{0}; // R163: training fight = native preferred distance (counted once per NPC)
     if(kcd2vr::training_vanilla_r163::skipOnce(kcd2vr::training_vanilla_r163::UnarmedSpacing,npcR163,actor,actor))return move;}
    // One fresh copy. In particular, never scale the stored native profile:
    // native731F48 can leave its old fields intact on an early return.
    if(!Copy(move+0x28,&view.activity,3*sizeof(float))||
       !std::isfinite(view.inner)||!std::isfinite(view.outer))return move;
    const float inner=view.inner*factor,outer=view.outer*factor;
    if(!std::isfinite(inner)||!std::isfinite(outer))return move;
    view.inner=inner;view.outer=outer;
    return reinterpret_cast<Address>(&view);
}
void Sample(DetourModKit::hook::MidContext& context) noexcept {
    using namespace DetourModKit::hook;
    auto& source=gpr(context,Gpr::Rax);
    source=ReadView(source,gpr(context,Gpr::Rdi),gpr(context,Gpr::Rcx));
    // Only RAX changes. The next three native loads read +2C,+28,+30,
    // with no intervening call;207969A overwrites RAX with the NPC vtable.
    // The original move remains in [RDI]. Native terrain/target/tactical
    // adjustments, both output writes and the shared native cache continue.
}
struct Pin {Address rva;const char* bytes;std::size_t size;};
constexpr Pin pins[]{
    {0x207967C,"\xF3\x44\x0F\x10\x40\x2C\x44\x0F\x29\x4C\x24\x60\xF3\x44\x0F\x10\x48\x28\x44\x0F\x29\x54\x24\x50\xF3\x44\x0F\x10\x50\x30\x48\x8B\x01",33},
    {0x207963F,"\x48\x8B\x01\x49\x8B\xE9\x49\x8B\xF0\x48\x8B\xF9\x48\x83\x78\x50\x00\x0F\x84\xEF\x02\x00\x00\x48\x8B\x51\x08\xE8\x8D\x72\x6B\xFE\x84\xC0\x0F\x84\xDE\x02\x00\x00",40},
    {0x20797DF,"\xF3\x44\x0F\x11\x06\xF3\x44\x0F\x11\x55\x00",11},
    {0x3EF4B0,"\x48\x83\xEC\x28\x48\x8B\x89\x98\x00\x00\x00\x33\xC0\x48\x85\xC9\x74\x10\x48\x8B\x01\xFF\x90\xE8\x00\x00\x00\x48\x8B\x80\x78\x02\x00\x00\x48\x83\xC4\x28\xC3",39},
    {0x9D33F8,"\x40\x53\x48\x83\xEC\x20\x33\xD2\x48\x8B\xD9\xE8\x6C\x01\x00\x00\x83\x78\x08\x0C\x75\x0D\xBA\x01\x00\x00\x00\x48\x8B\xCB\xE8\x59\x01\x00\x00\x48\x83\xC4\x20\x5B\xC3",41},
    {0x730B30,"\x48\x8B\x81\x38\x06\x00\x00\xC3",8},
    {0x207C4A0,"\x48\x83\xC1\x08\xE9\x87\xD1\xFF\xFF",9}
};
bool Contract(Address base) noexcept {
    std::array<unsigned char,48> bytes{};
    for(const auto& pin:pins)
        if(!Copy(base+pin.rva,bytes.data(),pin.size)||std::memcmp(bytes.data(),pin.bytes,pin.size))return false;
    return Equal(base+0x3FF1B18+0x250,base+0x730B30)&&
        Equal(base+0x3FE3F70+0x10,base+0x207C4A0);
}
} // namespace

bool Ready() noexcept {return ready.load(std::memory_order_acquire);}
const char* Failure() noexcept {return failure.load(std::memory_order_acquire);}
bool Install(Address base) noexcept {
    std::lock_guard lock(control);
    if(Ready())return image.load(std::memory_order_acquire)==base;
    if(Factor()==1.f){failure.store("NATIVE_NO_HOOK",std::memory_order_release);return false;}
    if(!base){failure.store("MODULE_UNAVAILABLE",std::memory_order_release);return false;}
    if(!retail_build112::Verify(base)){failure.store("RETAIL_MISMATCH",std::memory_order_release);return false;}
    if(hook){
        if(image.load(std::memory_order_acquire)!=base){failure.store("DIFFERENT_MODULE",std::memory_order_release);return false;}
        if((!hook->is_enabled()&&!hook->enable())||!hook->is_enabled()){
            failure.store("HOOK_ENABLE_FAILED",std::memory_order_release);return false;}
        ready.store(true,std::memory_order_release);failure.store("ACTIVE",std::memory_order_release);return true;
    }
    if(!Contract(base)){failure.store("EXACT_PIN_MISMATCH",std::memory_order_release);return false;}
    DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
    auto candidate=DetourModKit::hook::mid_at(DetourModKit::hook::MidRequest{
        "kcd2vr_npc_unarmed_movement_base_ring",DetourModKit::hook::Target{DetourModKit::Address{base+sampleLoads}},options},&Sample);
    if(!candidate){
#if !defined(KCD2VR_TEST_HOOK_SPY115)
        failure.store(candidate.error().code==DetourModKit::ErrorCode::MidHookCapacityExhausted?
            "DMK_MID_ADAPTER_CAPACITY_EXHAUSTED":"DMK_MID_CREATE_FAILED",std::memory_order_release);
#else
        failure.store("DMK_MID_CREATE_FAILED",std::memory_order_release);
#endif
        return false;
    }
    hook=std::move(*candidate);image.store(base,std::memory_order_release);
    if(!hook->enable()||!hook->is_enabled()){if(hook->is_enabled())(void)hook->disable();failure.store("HOOK_ENABLE_FAILED",std::memory_order_release);return false;}
    ready.store(true,std::memory_order_release);failure.store("ACTIVE",std::memory_order_release);return true;
}
void Shutdown() noexcept {
    std::lock_guard lock(control);
    ready.store(false,std::memory_order_release);
    failure.store("STOPPED",std::memory_order_release);
    if(hook&&hook->is_enabled())(void)hook->disable();
    // Retain the backend through in-flight native callbacks. The TLS read
    // view has no actor/item lifetime and is consumed immediately by native
    // scalar loads; it is never installed into a native field or cache.
}
}
