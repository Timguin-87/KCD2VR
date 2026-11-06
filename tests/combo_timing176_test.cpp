// Actual native combo detours; native execution replaced by counted leaf calls.
#include <windows.h>
#include <intrin.h>
#if defined(_MSC_VER)
#define _ReturnAddress HostReturnAddress
#endif
#include <vector>
#include <string>
static unsigned reads176=0,readsAtNativeExit176=0;
static std::size_t ReadableSpy176(const void* p,MEMORY_BASIC_INFORMATION* m,std::size_t n){++reads176;return VirtualQuery(p,m,n);}
#define VirtualQuery ReadableSpy176
static unsigned short HostComboCapture176(unsigned long,unsigned long,void**,unsigned long*);
#undef CaptureStackBackTrace
#define CaptureStackBackTrace HostComboCapture176
#include "../src/combat_diag_probe.cpp"
#undef VirtualQuery
#undef CaptureStackBackTrace
static void* entity176=nullptr;
static unsigned calls176=0,checks176=0,failed176=0;
static void* managerMemory176=nullptr;
static kcd2vr::combo_lineage19::Owner* offerMemoryH15=nullptr;
static int behavior176=0;
static std::vector<std::string> lines176;
static bool routeCaptureH14=false;
static unsigned stackCalls176=0;
static std::vector<std::uintptr_t> stackFrames176;
static unsigned short HostComboCapture176(unsigned long skip,unsigned long count,void** out,unsigned long*){
    ++stackCalls176;if(skip)return 0;
    const auto n=std::min<std::size_t>(count,stackFrames176.size());
    for(std::size_t i=0;i<n;++i)out[i]=reinterpret_cast<void*>(stackFrames176[i]);
    return static_cast<unsigned short>(n);
}
namespace kcd2vr::diagnostic_trace {void Write(const char* p) noexcept {
    if(!routeCaptureH14||diagnostics::capture168::accept(p,hostTickCount64))lines176.emplace_back(p);
}}
namespace kcd2vr::frik_pose_probe {
// This passive native probe fixture has no physical fist owner.
int FistContactAdmissionH12(const void*,const float*,int,const void*) noexcept {return -1;}
void* LastPlayerEntity() noexcept {return entity176;}
// Other production hooks in the included TU are not exercised by this passive
// combo observer fixture. Explicit fail-closed leaves keep MSVC linking honest.
bool ReadNativeComboSlotF1(const void*,subsystems::masterstrike_combos::NativeSlotF1& out) noexcept {out={};return false;}
std::uint64_t NativeComboInputGenerationH10() noexcept {return 0;}
bool GeometryIntentOwnerH30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {return false;}
unsigned GeometryIntentOwnerStateH30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {return 0;}
bool HybridQuietGuardOwnedH20(const void*,const void*) noexcept {return false;}
bool OriginalGeometryContactOwnerH30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {return false;}
original_contact_h8::Decision WaitOriginalGeometryContactH8(const original_contact_h8::Source&,const contact176::Ticket&,std::uint64_t) noexcept {std::abort();}
bool GeometrySolverOwnerValidH28(const solver_command_h28::Stamp&,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {return false;}
bool GeometrySolverCostOwnerValidH28(const solver_command_h28::Stamp&,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uintptr_t) noexcept {return false;}
bool BlockIndicatorGameplayH24(std::uint64_t) noexcept {return false;}
std::uint64_t PoseEpochH24() noexcept {return 0;}
void PublishVisibleFailure(const char*) noexcept {std::abort();}
bool ReadF4ContactLive176(contact176::Live&) noexcept {return false;}
bool GeometryAdmittedRelease51(const contact176::Ticket&,const void*) noexcept {return false;}
bool GeometryNativeAnswer19c4r2(const void*,const void*,int,answer59c4r2::Lineage&) noexcept {return false;}
bool CombatExperiment175Enabled() noexcept {return false;}
int F4ContactGateMode178r26() noexcept {return 1;}
bool PhysicalCombat177Enabled() noexcept {return false;}
bool GeometryAutoLock178r29() noexcept {return false;}
bool GuardOwnedByUs59c7e(const void*) noexcept {return false;}
int GeometryOwnerStage49() noexcept {return 0;}
bool GeometryOwnerValid178r30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {return false;}
bool PhysicalWeaponInputOwned6698() noexcept {return false;}
void NotifyNativeWeaponImpact136R2(float,float,float,bool,bool,bool,std::uint32_t) noexcept {}
int WeaponControlMode() noexcept {return 0;}
void* WeaponControlEntity(int) noexcept {return nullptr;}
void ObserveCollisionAnimationCharacter(void*,std::uint32_t,std::uint64_t) noexcept {}
void ObserveHitPartNamespace62(const part_namespace62::Ticket&) noexcept {}
bool GeometryCorpseBodyH7(std::uint32_t,std::uintptr_t,std::uintptr_t,int,bool) noexcept {return false;}
bool GeometryCorpseContactFaultH7() noexcept {return false;}
}
namespace kcd2vr::native_weapon_body_t4 {bool ReadContactOwnerH24(ContactOwnerH24&) noexcept {return false;}}
namespace kcd2vr::animation_filter {PlaybackStamp DiagnosticPlaybackStamp() noexcept {return {};}
void MarkContactForPlayback(std::uint64_t) noexcept {}}
namespace kcd2vr::retail_build112 {bool Verify(std::uintptr_t) noexcept {return false;}bool Ready(std::uintptr_t) noexcept {return false;}}
using namespace kcd2vr::combat_diag_probe;
namespace cap176=kcd2vr::diagnostics::capture168;
namespace lab176=kcd2vr::diagnostics::lab;
template<class T> static void put176(void* p,std::size_t o,T v){std::memcpy(static_cast<unsigned char*>(p)+o,&v,sizeof(v));}
static void ck176(bool b,const char* why){++checks176;if(!b){++failed176;std::printf("FAIL %s\n",why);}}
static void Native176(){
    ++calls176;
    if(behavior176==1)cap176::gate.select(cap176::Domain::Combat,false,hostTickCount64);
    if(behavior176==2)hostTickCount64=cap176::gate.until[0].load();
    if(behavior176==3)lab176::Set(lab176::Key::F5,1);
    if(behavior176==4){cap176::gate.select(cap176::Domain::Combat,false,hostTickCount64);cap176::gate.select(cap176::Domain::Combat,true,hostTickCount64);}
    if(behavior176==5){auto* flag=static_cast<unsigned char*>(managerMemory176)+0xE8;*flag=static_cast<unsigned char>(*flag+1);}
    if(behavior176==6&&offerMemoryH15)++offerMemoryH15->key.gesture;
    readsAtNativeExit176=reads176;
}
static void Step176(void*,const std::int32_t*,const std::int32_t*){Native176();}
static void Reset176(void*){Native176();}
static unsigned count176(const char* text){unsigned n=0;for(const auto& line:lines176)n+=line.find(text)!=std::string::npos;return n;}
int main(){
    std::vector<unsigned char> image(0x55D6000);
    const auto base=reinterpret_cast<std::uintptr_t>(image.data());
    g_combo_base6691=base;g_combo_timing_ready176=true;
    put176(image.data(),0x55D4D70+0x184,.625f);put176(image.data(),0x55D4D70+0x18C,.375f);
    std::array<unsigned char,0xF0> manager{};std::array<unsigned char,0x300> actor{},player{};std::array<unsigned char,0x80> entity{};
    managerMemory176=manager.data();
    put176(manager.data(),0,base+0x3B0A9A0);put176(manager.data(),8,actor.data());
    put176(actor.data(),0,base+0x3A80610);put176(actor.data(),0x1F8,player.data());
    put176(player.data(),0,base+0x3AA4B98);put176(player.data(),0x38,entity.data());
    put176(entity.data(),0,base+0x4072C78);put176(entity.data(),0xC,101u);entity176=entity.data();
    g_combo_step_original6691=Step176;g_combo_reset_original6691=Reset176;
    const auto reset=[&](){
        hostTickCount64+=2000;behavior176=0;lines176.clear();g_combo_burst176=0;
        g_combo_empty_reset_last6691=0;g_combo_detail6691=0;g_combo_depth6691=0;
        stackCalls176=0;stackFrames176.clear();hostReturnAddressValue=nullptr;g_combo_close_stack_ready=true;
        cap176::gate.select(cap176::Domain::Combat,true,hostTickCount64);
    };
    reset();cap176::gate.select(cap176::Domain::Combat,false,hostTickCount64);
    const auto reads=reads176,calls=calls176;const auto counters=g_combo_calls6691.load();
    ComboStepDetour6691(manager.data(),nullptr,nullptr);ComboResetDetour6691(manager.data());
    ck176(calls176==calls+2&&reads176==reads&&lines176.empty()&&g_combo_calls6691==counters,"OFF forwards both native calls without reads, counters or formatting");
    reset();const auto before=calls176;ComboResetDetour6691(manager.data());ComboStepDetour6691(manager.data(),nullptr,nullptr);
    ck176(calls176==before+2&&count176("op=RESET callerRva=")==1&&count176("op=STEP_FILTER callerRva=")==1,"adjacent RESET and STEP both survive recorder burst");
    ck176(count176("copiedSettingsValid=1 comboSlotDuration=0.625 windowMinWeight=0.375")==2,"actual callbacks copy current pinned storage, not registration defaults");
    ck176(count176("effectiveDuration=UNKNOWN startEvent=UNQUALIFIED")==2,"copied settings never claim measured effective window/start");
    reset();const auto flood=calls176;for(unsigned i=0;i<100;++i)ComboStepDetour6691(manager.data(),nullptr,nullptr);
    ck176(calls176==flood+100&&count176("op=STEP_FILTER callerRva=")==4,"100 native callbacks forward once each while bounded burst records four");
    hostTickCount64+=99;ComboStepDetour6691(manager.data(),nullptr,nullptr);ck176(count176("op=STEP_FILTER callerRva=")==4,"burst cannot refill before100ms");
    ++hostTickCount64;ComboStepDetour6691(manager.data(),nullptr,nullptr);ck176(count176("op=STEP_FILTER callerRva=")==5,"burst refills at100ms");
    reset();ComboResetDetour6691(manager.data());hostTickCount64+=100;ComboResetDetour6691(manager.data());
    ck176(count176("op=RESET callerRva=")==1,"quiet empty reset retains one-second suppression");
    for(int behavior:{1,2,3,4})for(bool step:{false,true}){
        reset();behavior176=behavior;const auto c=calls176;const auto failures=g_combo_decode6691.load();
        if(step)ComboStepDetour6691(manager.data(),nullptr,nullptr);else ComboResetDetour6691(manager.data());
        ck176(calls176==c+1&&reads176==readsAtNativeExit176&&lines176.empty()&&g_combo_decode6691==failures,"OFF/expiry/epoch/new-session during native suppresses all post reads, counters and output");
    }
    reset();put176(manager.data(),0,std::uintptr_t(0));const auto rejected=calls176;
    ComboStepDetour6691(manager.data(),nullptr,nullptr);
    ck176(calls176==rejected+1&&lines176.empty(),"identity rejection forwards native without settings record");
    put176(manager.data(),0,base+0x3B0A9A0);
    reset();g_combo_timing_ready176=false;ComboStepDetour6691(manager.data(),nullptr,nullptr);
    ck176(count176("copiedSettingsValid=0")==1,"unqualified optional settings pins remain explicit UNKNOWN");
    ck176(!ComboTimingFingerprint176(base),"wrong retail settings pins are rejected");
    // H14 actual STEP_FILTER/RESET -> production formatter -> real recorder
    // domain. The semantic label contains uppercase CURSORS, which previously
    // routed only this essential selector record to F2 instead of F3.
    reset();routeCaptureH14=true;cap176::gate.select(cap176::Domain::Hud,false,hostTickCount64);
    const auto h14Calls=calls176;const int nativeZone=2,nativeInput=1;
    ComboResetDetour6691(manager.data());ComboStepDetour6691(manager.data(),&nativeZone,&nativeInput);
    ck176(calls176==h14Calls+2&&count176("COMBO_EVENT ")==2,"F3-only actual native RESET and STEP_FILTER records survive capture routing");
    std::vector<std::string> comboEventsH14;
    for(const auto& line:lines176)if(line.find("COMBO_EVENT ")!=std::string::npos){
        comboEventsH14.push_back(line);
        ck176(cap176::domain(line.c_str())==cap176::Domain::Combat&&
            line.find("observedScope=CURSORS_AND_CANDIDATES_ONLY")!=std::string::npos&&
            line.find("comboAccepted=UNKNOWN")!=std::string::npos,"actual formatter remains read-only combo evidence, not HUD or successful combo");
    }
    ck176(count176("arg0Readable=1 arg0Raw=2 arg1Readable=1 arg1Raw=1")==1,"native selector arguments survive the actual STEP_FILTER formatter");
    ck176(!cap176::accept("V66.111 HUD_CURSOR mode=VISIBLE",hostTickCount64)&&
        cap176::domain("V66.111 HUD_CURSOR mode=VISIBLE")==cap176::Domain::Hud,"legitimate HUD cursor stays F2-owned in F3-only capture");
    cap176::gate.select(cap176::Domain::Combat,false,hostTickCount64);cap176::gate.select(cap176::Domain::Hud,true,hostTickCount64);
    for(const auto& line:comboEventsH14)ck176(!cap176::accept(line.c_str(),hostTickCount64),"F2-only rejects actual native combo event text");
    ck176(cap176::accept("V66.111 HUD_CURSOR mode=VISIBLE",hostTickCount64),"F2-only still accepts legitimate HUD cursor");
    lines176.clear();const auto h14F2Calls=calls176;
    ComboResetDetour6691(manager.data());ComboStepDetour6691(manager.data(),&nativeZone,&nativeInput);
    ck176(calls176==h14F2Calls+2&&lines176.empty(),"F2-only actual combo callbacks forward without capture");
    cap176::gate.select(cap176::Domain::Hud,false,hostTickCount64);
    for(const auto& line:comboEventsH14)ck176(!cap176::accept(line.c_str(),hostTickCount64),"default-off rejects actual native combo event text");
    const auto h14OffCalls=calls176;ComboResetDetour6691(manager.data());ComboStepDetour6691(manager.data(),&nativeZone,&nativeInput);
    ck176(calls176==h14OffCalls+2&&lines176.empty(),"default-off actual combo callbacks remain quiet original-once");
    reset();routeCaptureH14=true;
    namespace lineageH15=kcd2vr::combo_lineage19;
    lineageH15::Owner ownerH15{};ownerH15.key={42,11,12,13,14,15,reinterpret_cast<std::uintptr_t>(player.data())};
    ownerH15.actor=reinterpret_cast<std::uintptr_t>(actor.data());ownerH15.state=0x1234;ownerH15.manager=reinterpret_cast<std::uintptr_t>(manager.data());
    ownerH15.target=0x2345;ownerH15.targetEntity=0x3456;ownerH15.targetId=77;ownerH15.action=0x4567;ownerH15.release=123456;
    ownerH15.modeGeneration=lineageH15::grace.generation();ownerH15.mode=3;ownerH15.tick=hostTickCount64-10;
    offerMemoryH15=&ownerH15;behavior176=6;
    {lineageH15::OfferScope offer(&ownerH15);ComboStepDetour6691(manager.data(),&nativeZone,&nativeInput);}
    ck176(count176("link=SAME_STACK_OFFER_NOT_ACCEPTANCE")==1&&count176("gesture=42 key=000000000000000B generation=12 body=13 request=14 recovery=15")==1,"actual callback copies exact same-stack provenance before original can replace it");
    ck176(ownerH15.key.gesture==43&&count176("target=77")==1&&count176("release=123456 action=4567")==1&&count176("nativeCause=UNKNOWN nativeAcceptance=UNKNOWN")==1,"exact target/action/release retained without claiming native acceptance or causal inference");
    reset();ck176(lineageH15::nativeSlot.consume(ownerH15,1500),"existing real native-slot owner published for read-only callback fixture");
    ComboStepDetour6691(manager.data(),&nativeZone,&nativeInput);
    ck176(count176("link=STORED_NATIVE_SLOT_NOT_REVALIDATED")==1&&count176("nativeSlotSnapshotKnown=1 slotWindowMsRaw=1500 consumedReleaseHighwaterRaw=123456")==1,"stored native-slot provenance is explicitly unvalidated, not same-stack action attribution");
    reset();lineageH15::nativeSlot.retire();ck176(lineageH15::grace.publish(ownerH15),"existing grace publication remains unchanged");
    ComboStepDetour6691(manager.data(),&nativeZone,&nativeInput);ck176(count176("link=STORED_HIT_GRACE_NOT_REVALIDATED")==1&&count176("nativeSlotSnapshotKnown=0 slotWindowMsRaw=0 consumedReleaseHighwaterRaw=0")==1,"existing grace copied only as labeled stored evidence; absent native-slot metadata is UNKNOWN, not zero-duration permission");
    reset();ownerH15.manager+=8;{lineageH15::OfferScope offer(&ownerH15);lineageH15::grace.clear();ComboStepDetour6691(manager.data(),&nativeZone,&nativeInput);}
    ck176(count176("link=NONE")==1&&count176("gesture=0 key=0000000000000000")==1,"another manager cannot lend a gesture to this native callback");
    reset();behavior176=5;g_combo_detail6691=hostTickCount64;
    ComboStepDetour6691(manager.data(),&nativeZone,&nativeInput);
    ck176(count176("observedFieldsChanged=1")==1&&count176("details=1")==2,"actual short changed episode retains BEFORE/AFTER details inside the500ms budget");
    reset();lineageH15::grace.clear();lineageH15::nativeSlot.retire();ComboStepDetour6691(manager.data(),&nativeZone,&nativeInput);
    ck176(count176("H15 F3_COMBO_LINK ")==1,"new link remains F3-routed under production capture policy");
    for(const auto& line:lines176)if(line.find("H15 F3_COMBO_LINK ")!=std::string::npos)ck176(line.size()<1799&&cap176::domain(line.c_str())==cap176::Domain::Combat,"bounded native link formatter remains in actual Combat domain");
    // Exact native close caller is observed through Windows unwind frames only
    // for an active, changed, previously nonempty Henry window-false reset.
    alignas(8) std::array<unsigned char,40> cursorState{};
    const auto begin=reinterpret_cast<std::uintptr_t>(cursorState.data());
    const kcd2vr::diagnostics::combo::Vector activeStates{begin,begin+40,begin+40};
    const auto closeSetup=[&](){reset();behavior176=5;
        put176(manager.data(),0xC0,activeStates);hostReturnAddressValue=reinterpret_cast<void*>(base+0xC5B45D);
        stackFrames176={base+0xC5B45D,base+0x91496D,base+0x2256E33};};
    closeSetup();const auto beforeClose=calls176;ComboResetDetour6691(manager.data());
    ck176(calls176==beforeClose+1&&stackCalls176==1&&count176("closeSampled=1 closeCallerRva=2256E33 closePath=UPDATE_ELIGIBILITY_FALSE")==1,"same actual reset forwards once and records exact native eligibility-close caller");
    for(const auto& line:lines176)if(line.find("COMBO_EVENT ")!=std::string::npos)
        ck176(line.size()<1799&&cap176::domain(line.c_str())==cap176::Domain::Combat,"close fields stay bounded and F3-owned through actual formatter");
    closeSetup();stackFrames176.back()=base+0x9087C5;ComboResetDetour6691(manager.data());
    ck176(stackCalls176==1&&count176("closePath=UPDATE_TIMER_EXPIRED")==1,"native timer close is distinguished from update eligibility");
    closeSetup();stackFrames176={base+0x91496D,base+0x2256E33,base+0x91496D,base+0x9087C5};ComboResetDetour6691(manager.data());
    ck176(count176("closeCallerRva=0 closePath=AMBIGUOUS_CLOSE_STACK")==1,"nested close frames do not guess a reset cause");
    closeSetup();stackFrames176={base+0xC5B45D};ComboResetDetour6691(manager.data());
    ck176(count176("closeSampled=1 closeCallerRva=0 closePath=NO_CLOSE_FRAME")==1,"incomplete Windows unwind remains unknown");
    closeSetup();stackFrames176.clear();ComboResetDetour6691(manager.data());
    ck176(stackCalls176==1&&count176("closeSampled=1 closeCallerRva=0 closePath=NO_CLOSE_FRAME")==1,"failed unwind forwards and reports no causal frame");
    closeSetup();g_combo_close_stack_ready=false;ComboResetDetour6691(manager.data());
    ck176(stackCalls176==0&&count176("closeSampled=0")==1,"unqualified retail provenance never attempts close decoding");
    closeSetup();behavior176=0;ComboResetDetour6691(manager.data());
    ck176(stackCalls176==0,"unchanged nonempty reset does not unwind");
    closeSetup();hostReturnAddressValue=reinterpret_cast<void*>(base+0xC5C9C7);ComboResetDetour6691(manager.data());
    ck176(stackCalls176==0,"phase-property reset cannot borrow window-close provenance");
    closeSetup();ComboStepDetour6691(manager.data(),&nativeZone,&nativeInput);
    ck176(stackCalls176==0,"STEP_FILTER never performs reset close unwinding");
    for(int behavior:{1,2,3,4}){closeSetup();behavior176=behavior;ComboResetDetour6691(manager.data());
        ck176(stackCalls176==0&&lines176.empty(),"OFF/expiry/epoch/session change during original cannot trigger close unwinding");}
    closeSetup();cap176::gate.select(cap176::Domain::Combat,false,hostTickCount64);const auto offCalls=calls176;
    ComboResetDetour6691(manager.data());ck176(calls176==offCalls+1&&stackCalls176==0&&lines176.empty(),"F3 OFF remains native-only without stack work");
    closeSetup();put176(manager.data(),0xC0,kcd2vr::diagnostics::combo::Vector{});ComboResetDetour6691(manager.data());
    ck176(stackCalls176==0,"empty reset does not perform stack work");
    std::printf("combo_timing176=%u/%u actual_detours=YES native_execution=NONE\n",checks176-failed176,checks176);
    return failed176?1:0;
}
