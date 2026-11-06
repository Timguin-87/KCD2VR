// Include at GLOBAL scope, once, in activity_input163.cpp. This optional
// diagnostic observer never changes native point, cursor, event or result.
#ifndef KCD2VR_SMITH_CONTACT68_FIXTURE
#include "subsystems/smithing/smith_contact68.h"
#include "activity_input163.h"
#include "diagnostics/capture168.h"
#include "vrik/combat_rules178r10.h"
#include "retail_build112.h"
#include "diagnostic_trace.h"
#include <DetourModKit/hook.hpp>
#include <intrin.h>
#include <windows.h>
#include <optional>
#include <utility>
#endif
#include "anvil_a2.h"
#ifdef KCD2VR_SMITH_CONTACT68_FIXTURE
#define KCD2VR_ANVIL_A3_PURE
#endif
#include "anvil_physics_a3.h"
#include "vrik/visible_failure.h"
#include "vrik/semi_smith46.h"
#include <cstdio>
#include <cstring>

namespace kcd2vr::subsystems::smithing::contact68 {
namespace detail68 {
using Original=Vec3*(__fastcall*)(void*,Vec3*);
inline Original original=nullptr;
inline std::uintptr_t base=0;
inline std::atomic<bool> ready{false};
inline Budget budget{};
inline std::atomic_flag busy=ATOMIC_FLAG_INIT;
template<class T> bool read(std::uintptr_t at,T& value) noexcept {
#ifdef KCD2VR_SMITH_CONTACT68_FIXTURE
    return fixtureRead68(at,&value,sizeof(value));
#else
    __try {std::memcpy(&value,reinterpret_cast<const void*>(at),sizeof(value));return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
#endif
}
// A2: every native StrokeHit of the CURRENT smith minigame, recorder or not, publishes its point with the full
// station identity and Henry's native root. Copied reads only; nothing native is changed or retried.
inline std::atomic_flag anvilBusyA2=ATOMIC_FLAG_INIT;
// A3 REAL ANVIL: query only on an exact native StrokeHit callback, then cache
// actual static geometry for that smith identity. A transient miss is retried
// on a later real blow at most once/second; no pose-thread query or fake blow.
// A blow outside the cached box (+15 cm) may discover a different actual box.
namespace a3 {
inline kcd2vr::subsystems::smithing::anvil_a2::Identity lastId{};inline bool tried=false,found=false,missNotified=false;
inline std::uint64_t lastAttempt=0,modeRevision=0;
inline kcd2vr::subsystems::smithing::anvil_physics_a3::Box box{};
#ifdef KCD2VR_SMITH_CONTACT68_FIXTURE
inline bool (*fixtureQuery)(const double (&)[3],kcd2vr::subsystems::smithing::anvil_physics_a3::Box*,unsigned&,const char*&)=nullptr;
#endif
}
static void attachPhysicsAnvilA3(kcd2vr::subsystems::smithing::anvil_a2::Blow& b) noexcept {
    namespace ap=kcd2vr::subsystems::smithing::anvil_physics_a3;
    const double blow[3]={b.point.x,b.point.y,b.point.z};
    const bool sameId=a3::lastId==b.id&&a3::modeRevision==b.modeRevisionD39;
    const bool outside=sameId&&a3::found&&(blow[0]<a3::box.mn[0]-.15||blow[0]>a3::box.mx[0]+.15||blow[1]<a3::box.mn[1]-.15||blow[1]>a3::box.mx[1]+.15);
    if(!sameId){a3::lastId=b.id;a3::modeRevision=b.modeRevisionD39;a3::tried=false;a3::found=false;a3::missNotified=false;a3::lastAttempt=0;a3::box={};}
    const bool retryMiss=a3::tried&&!a3::found&&(b.tick<a3::lastAttempt||b.tick-a3::lastAttempt>=1000);
    if(!a3::tried||outside||retryMiss){
        a3::tried=true;a3::found=false;a3::lastAttempt=b.tick;
        ap::Box boxes[ap::kMaxCandidates]{};unsigned n=0;const char* why="BINDING";bool queried=false;
#ifdef KCD2VR_SMITH_CONTACT68_FIXTURE
        queried=a3::fixtureQuery&&a3::fixtureQuery(blow,boxes,n,why);
#else
        queried=ap::query(base,blow,boxes,n,why);
#endif
        ap::Box pick{};const char* choice=why;
        if(queried&&n)a3::found=ap::choose(blow,boxes,n,pick,choice);
        if(a3::found)a3::box=pick;
        const bool diagnosticD42=kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,b.tick);
        if(diagnosticD42){char m[640]{};
        const auto sessionD42=kcd2vr::diagnostics::capture168::gate.session[unsigned(kcd2vr::diagnostics::capture168::Domain::Interaction)].load(std::memory_order_acquire);
        if(a3::found){
            std::snprintf(m,sizeof(m),"D42_F5 kind=SMITH_ANVIL source=PHYSICS entity=%p box=min(%.3f,%.3f,%.3f) max(%.3f,%.3f,%.3f) blow=(%.3f,%.3f,%.3f) candidates=%u instance=%p session=%llu tick=%llu life=%llu",
                reinterpret_cast<void*>(pick.entity),pick.mn[0],pick.mn[1],pick.mn[2],pick.mx[0],pick.mx[1],pick.mx[2],blow[0],blow[1],blow[2],n,
                reinterpret_cast<void*>(b.id.instance),(unsigned long long)sessionD42,(unsigned long long)b.tick,(unsigned long long)b.id.lifetime);
        } else {
            std::snprintf(m,sizeof(m),"D42_F5 kind=SMITH_ANVIL event=PHYSICS_MISS reason=%s candidates=%u blow=(%.3f,%.3f,%.3f) instance=%p session=%llu tick=%llu life=%llu",
                queried?choice:why,n,blow[0],blow[1],blow[2],reinterpret_cast<void*>(b.id.instance),(unsigned long long)sessionD42,(unsigned long long)b.tick,(unsigned long long)b.id.lifetime);
        }
#ifndef KCD2VR_SMITH_CONTACT68_FIXTURE
        diagnostic_trace::Write(m);
#endif
        }
        if(!a3::found){
#ifndef KCD2VR_SMITH_CONTACT68_FIXTURE
            if(!a3::missNotified)(void)kcd2vr::visible_failure::publish("smithing: anvil collision not found here",b.tick);
#endif
            a3::missNotified=true;
        }
    }
    if(a3::found){b.boxValid=true;b.phys=a3::box.entity;b.boxMin={a3::box.mn[0],a3::box.mn[1],a3::box.mn[2]};b.boxMax={a3::box.mx[0],a3::box.mx[1],a3::box.mx[2]};}
}
static void publishAnvilA2(void* self,Vec3* result,Vec3* output,std::uintptr_t caller) noexcept {
    namespace av=kcd2vr::subsystems::smithing::anvil_a2;
    if(!av::enabled.load(std::memory_order_acquire)||caller!=base+0x859A76||!result||result!=output)return;
    const auto modeRevision=av::modeRevisionD39.load(std::memory_order_acquire);
    const int contactMode=av::mode.load(std::memory_order_acquire);if(contactMode==int(av::Mode::Off))return;
    if(anvilBusyA2.test_and_set(std::memory_order_acquire))return;
    struct Clear {~Clear(){anvilBusyA2.clear(std::memory_order_release);}} clear;
    using namespace kcd2vr::activity_input163;
    if(!witness175.ready.load(std::memory_order_acquire)||witness175.transitionDepth175.load(std::memory_order_acquire))return;
    const auto lifetime=witness175.lifetime175.load(std::memory_order_acquire);
    const auto epoch=witness175.smithEpochC1.load(std::memory_order_acquire);
    const auto instance=witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire);
    Vec3 actual{};if(!instance||!read(reinterpret_cast<std::uintptr_t>(result),actual))return;
    const auto rd=[](std::uintptr_t at,auto& value){return read(at,value);};
    Event e{};const char* reason=nullptr;
    if(!context(rd,[](std::uint32_t id){return findEntity?findEntity(id):std::uintptr_t(0);},
        base,reinterpret_cast<std::uintptr_t>(self),instance,lifetime,actual,e,reason))return;
    float root[12]{};av::Blow b{};
    if(!read(e.henry+0x58,root)||!av::rootFrom(root,b.rootPos,b.rootFwd))return;
    if(witness175.lifetime175.load(std::memory_order_acquire)!=lifetime||witness175.smithEpochC1.load(std::memory_order_acquire)!=epoch||
       witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire)!=instance)return;
    b.id={instance,e.henry,lifetime,epoch};b.point={e.point.x,e.point.y,e.point.z};b.tick=GetTickCount64();
    b.modeRevisionD39=modeRevision;
    if(contactMode==int(av::Mode::Real))attachPhysicsAnvilA3(b);
    // A menu change while the native read was in flight cannot publish old
    // geometry under the new source choice.
    if(!av::enabled.load(std::memory_order_acquire)||av::mode.load(std::memory_order_acquire)!=contactMode||
       av::modeRevisionD39.load(std::memory_order_acquire)!=modeRevision)return;
    (void)av::publish(b);
}
static Vec3* __fastcall detour(void* self,Vec3* output) {
#ifdef KCD2VR_SMITH_CONTACT68_FIXTURE
    const auto caller=fixtureCaller68;
#else
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
#endif
    // Always execute the exact original ONCE, including recorder-off/native
    // reference/foreign callers and native failures. Return its exact result.
    Vec3* const result=original(self,output);
    if(!ready.load(std::memory_order_acquire))return result;
    publishAnvilA2(self,result,output,caller);
    using namespace kcd2vr::diagnostics::capture168;
    if(kcd2vr::combat_rules178r10::f5Rung.load(std::memory_order_relaxed)<=0||caller!=base+0x859A76)return result;
    const auto now=std::uint64_t(GetTickCount64());
    if(!active(Domain::Interaction,now))return result;
    const auto session=gate.session[unsigned(Domain::Interaction)].load(std::memory_order_acquire);
    if(busy.test_and_set(std::memory_order_acquire))return result;
    struct Clear {~Clear(){busy.clear(std::memory_order_release);}} clear;
    if(!budget.admit(true,true,session,caller,base))return result;
    if(budget.used>=128)exhaustedSession.store(session,std::memory_order_release);
    using namespace kcd2vr::activity_input163;
    const auto lifetime=witness175.lifetime175.load(std::memory_order_acquire);
    const auto instance=witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire);
    const bool lifecycleReady=witness175.ready.load(std::memory_order_acquire)&&
        !witness175.transitionDepth175.load(std::memory_order_acquire);
    Event e{};const char* reason="NATIVE_LIFECYCLE_UNAVAILABLE";Vec3 actual{};
    const auto rd=[](std::uintptr_t at,auto& value){return read(at,value);};
    bool observed=false;
    if(lifecycleReady&&result==output&&result&&read(reinterpret_cast<std::uintptr_t>(result),actual)){
        observed=context(rd,[](std::uint32_t id){return findEntity?findEntity(id):std::uintptr_t(0);},
            base,reinterpret_cast<std::uintptr_t>(self),instance,lifetime,actual,e,reason);
    }else if(lifecycleReady)reason="NATIVE_RETURN_IDENTITY_OR_POINT";
    if(observed&&readHammer){HammerFrame h{};
        if(readHammer(h,now)&&joinHammer(rd,base,now,e,h)){e.hammer=h;e.hammerJoined=true;}}
    // A changed native lifetime cannot label an old tool/point current. Nothing
    // native is retried, suppressed or evaluated by this observer.
    if(!witness175.ready.load(std::memory_order_acquire)||witness175.transitionDepth175.load(std::memory_order_acquire)||
       witness175.lifetime175.load(std::memory_order_acquire)!=lifetime||
       witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire)!=instance){
        observed=false;e={};reason="NATIVE_LIFECYCLE_CHANGED";
    }
    if(!ready.load(std::memory_order_acquire))return result;
    Vec3 local{};const bool localKnown=observed&&localPoint(e.workpieceWorld,e.point,local);
    // Copied metadata from the already-published input/work-clock snapshot.
    // This existing finite F5 callback does no new native state readback.
    semi_smith46::ClockControl commandD57{};
    (void)semi_smith46::clockControl.read(commandD57);
    const auto& strokeD57=commandD57.diagnosticD57;
    const char* strokeJoinD57=stroke_receipt_d57::join(strokeD57,session,now,e.instance,e.lifetime,e.hammerJoined,
        e.hammer.poseEpoch,e.hammer.sourceEpoch,e.hammer.modeEpoch);
    char line[2048]{};
    std::snprintf(line,sizeof(line),
        "G1R1 SMITH_CONTACT68 event=StrokeHit serial=%llu session=%llu sample=%u/128 reason=%s instance=%p life=%llu phase=%d workpiece=%u holder=%p pointWorld=(%.4f,%.4f,%.4f) cursorNative=(%.5f,%.5f) pointLocalKnown=%d pointLocal=(%.4f,%.4f,%.4f) mapSize=(%.4f,%.4f) mapOffset=(%.4f,%.4f) mapAngles=(%.4f,%.4f,%.4f) hammerJoined=%d hammerKey=%016llX hammerSample=%llu hammerPoseEpoch=%llu sourceEpoch=%llu hammerAge=%llu hammerWorld=(%.4f,%.4f,%.4f) nativeCaller=859A76 contact3D=UNPROVEN cursorInverse=UNPROVEN nativeEvaluation=CONTINUES_UNCHANGED writes=NONE budgetExhausted=%d callbackKind=NATIVE_POINT_CALLBACK strokeIdD57=%llu strokeJoin=%s strokeProducer=%s strokeMode=%d strokeStart=%llu strokeRelease=%llu releaseToPointMs=%llu strokeInputSample=%llu acceptance=UNOBSERVED",
        (unsigned long long)budget.serial,(unsigned long long)session,budget.used,reason,
        reinterpret_cast<void*>(e.instance),(unsigned long long)e.lifetime,e.phase,e.workpieceId,reinterpret_cast<void*>(e.holder),
        e.point.x,e.point.y,e.point.z,e.cursor.x,e.cursor.y,localKnown,local.x,local.y,local.z,
        e.mapSize.x,e.mapSize.y,e.mapOffset.x,e.mapOffset.y,e.mapRotation.x,e.mapRotation.y,e.mapRotation.z,
        e.hammerJoined,(unsigned long long)e.hammer.binding.key,(unsigned long long)e.hammer.sample,
        (unsigned long long)e.hammer.poseEpoch,(unsigned long long)e.hammer.sourceEpoch,(unsigned long long)(e.hammerJoined?now-e.hammer.tick:0),
        e.hammer.world[4],e.hammer.world[5],e.hammer.world[6],budget.used>=128,
        (unsigned long long)strokeD57.id,strokeJoinD57,strokeD57.id?(strokeD57.nativeHeight?"VR_NATIVE_HEIGHT":"VR_WORK_CLOCK"):"NONE_NATIVE_OR_MANUAL",strokeD57.mode,
        (unsigned long long)strokeD57.startTick,(unsigned long long)strokeD57.releaseTick,
        (unsigned long long)(strokeD57.releaseTick&&now>=strokeD57.releaseTick?now-strokeD57.releaseTick:0),(unsigned long long)strokeD57.sample);
    diagnostic_trace::Write(line);return result;
}
}
#ifndef KCD2VR_SMITH_CONTACT68_FIXTURE
inline std::optional<DetourModKit::hook::Hook> nativeHook68;
inline bool pins(std::uintptr_t b) noexcept {
    if(!retail_build112::Ready(b))return false;
    const auto eq=[&](std::uintptr_t r,const auto& expected){unsigned char got[sizeof(expected)]{};
        return detail68::read(b+r,got)&&!std::memcmp(got,expected,sizeof(expected));};
    constexpr unsigned char entry[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,0x55,0x48,0x8D,0x68,0xA1,0x48,0x81,0xEC,0xA0,0x00,0x00,0x00};
    constexpr unsigned char pointCall[]{0xE8,0xEA,0x75,0x3F,0x00};
    constexpr unsigned char eventCall[]{0xE8,0x95,0x0D,0x00,0x00};
    constexpr unsigned char cursorGetter[]{0x48,0x8B,0x41,0x08,0x48,0x89,0x02,0x48,0x8B,0xC2,0xC3};
    constexpr unsigned char pointSelector[]{0x41,0xB8,0x15,0x00,0x00,0x00};
    std::uintptr_t getter=0;
    return eq(0xC51060,entry)&&eq(0x859A71,pointCall)&&eq(0x858C3E,eventCall)&&
        eq(0xC510A2,pointSelector)&&eq(0x9D3820,cursorGetter)&&
        detail68::read(b+0x3A6A1E8,getter)&&getter==b+0x9D3820;
}
inline bool install(std::uintptr_t b) noexcept {
    // A retained, retired or partially enabled handle is NEVER a new install.
    if(nativeHook68)return detail68::ready.load(std::memory_order_acquire)&&detail68::base==b&&detail68::original;
    if(!pins(b))return false;
    DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
    DetourModKit::hook::InlineRequest request{"kcd2vr_smith_contact68_readonly",
        DetourModKit::hook::Target{DetourModKit::Address{b+0xC51060}},options};
    auto hook=DetourModKit::hook::inline_at(std::move(request),&detail68::detour);if(!hook)return false;
    nativeHook68=std::move(*hook);detail68::original=nativeHook68->original<detail68::Original>();detail68::base=b;
    if(!detail68::original){nativeHook68.reset();detail68::base=0;return false;}
    HMODULE module=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&detail68::detour),&module)){
        nativeHook68.reset();detail68::original=nullptr;detail68::base=0;return false;
    }
    if(nativeHook68->enable()){detail68::ready.store(true,std::memory_order_release);return true;}
    detail68::ready.store(false,std::memory_order_release);
    // Even a failed enable may have partially patched a target. Retain the
    // pinned module, original trampoline and hook on BOTH disarm outcomes.
    // An already-entered detour can safely finish forwarding native execution.
    (void)nativeHook68->disable();
    return false;
}
inline bool uninstall() noexcept {
    detail68::ready.store(false,std::memory_order_release);
    // Process-lifetime optional observer: keep the small trampoline even after
    // successful disable, because a native callback may already have entered.
    // There is no unsafe free/reinstall race and no lingering diagnostic work.
    return !nativeHook68||bool(nativeHook68->disable());
}
#endif
}
