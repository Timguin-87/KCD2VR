// Generated includes contain literal production regions. The runner records
// exact excerpt hashes; only native/OS/format/output leaves are substituted.
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include "diagnostics/capture_ticket_d84.h"
#include "native_weapon_body_t4.h"
#include "native_physics_phase_t4.h"
#include "vrik/native_held_owner_t4_status.h"
#include "vrik/lab58.h"
#include "vrik/visible_failure.h"
#include "vrik/handling_profile59c10r2.h"
#include "core/collab_test_h21.h"
#include "subsystems/physics/f9_status_fd.h"
#include "ragdoll_lease59.h"

namespace cap=kcd2vr::diagnostics::capture168;
using D=cap::Domain;
static std::uint64_t now=10000;
static unsigned clocks=0,formats=0,cvarQueries=0,cvarWrites=0,phaseQueries=0,bodyQueries=0,sourceQueries=0;
static bool restartAtFormat=false;
static D restartDomain=D::Physics;
static std::uint64_t GetTickCount64() noexcept {++clocks;return now;}
namespace fixture {
template<class... Args> int format(char* out,std::size_t size,const char* text,Args... args) noexcept {
    ++formats;
    if(restartAtFormat){restartAtFormat=false;cap::gate.select(restartDomain,false,now);cap::gate.select(restartDomain,true,now);}
    return std::snprintf(out,size,text,args...);
}
}
namespace kcd2vr {
namespace diagnostic_trace {
static bool enabled=true;
static std::vector<std::string> rows;
bool Enabled() noexcept {return enabled;}
static void Enqueue(const char* text) noexcept {rows.emplace_back(text);}
#include "physics_receipt_sink_d84.inc"
}
namespace native_physics_phase_t4 {
Receipt ReadReceipt() noexcept {++phaseQueries;Receipt r{};r.steps=12;r.rejectedContexts=2;return r;}
}
namespace native_weapon_body_t4 {
static bool installOK=true,enabled=false,readOK=true;
static unsigned installs=0,enables=0,cancels=0;
static Achieved readValue{};
bool Install() noexcept {++installs;return installOK;}
void SetEnabled(bool value) noexcept {++enables;enabled=value;}
void Cancel() noexcept {++cancels;}
bool Read(Achieved& out) noexcept {++bodyQueries;out=readValue;return readOK;}
const char* ReasonName(Reason) noexcept {return "MODELED_NATIVE_REASON";}
}
namespace combat_diag_probe {
static unsigned mutations=0;
bool Install() noexcept {return true;}
void SetContactImpulse596(bool,float,float,float) {++mutations;}
void SetContactImpulseDiagnostics19c4r4(bool) {++mutations;}
}
namespace frik_pose_probe {
static unsigned bodies=0,prepares=0;
static HeldPhysicsSourceT4 source=HeldPhysicsSourceT4::Ready;
bool PrepareNativeHandPhysicsH24() noexcept {++prepares;return true;}
void SetPhysBodies59c10r8(int,float,float) {++bodies;}
HeldPhysicsSourceT4 HeldPhysicsSourceReasonT4() noexcept {++sourceQueries;return source;}
const char* HeldPhysicsSourceNameT4(HeldPhysicsSourceT4) noexcept {return "MODELED_NATIVE_SOURCE";}
}
static void* SafeGetCryConsole() noexcept {return reinterpret_cast<void*>(1);}
static void* SafeConsoleGetCVar(void*,const char*) noexcept {++cvarQueries;return reinterpret_cast<void*>(1);}
static float SafeCVarGetFloat(void*,float) noexcept {return 1.f;}
static bool SafeCVarSetFloat(void*,float) noexcept {++cvarWrites;return true;}
#define KCD2VR_M2_HOST_SLOT 5007
#include "physics_gate_host_d84.inc"
#undef KCD2VR_M2_HOST_SLOT
#define KCD2VR_M2_HOST_SLOT 5009
#include "physics_gate_host_d84.inc"
#undef KCD2VR_M2_HOST_SLOT
#define KCD2VR_M2_HOST_SLOT 5011
#include "physics_gate_host_d84.inc"
#undef KCD2VR_M2_HOST_SLOT

// Copied-data boundary values only. No geometry or handling policy is modeled.
struct FixtureV3 {double x=0,y=0,z=0;};
struct FixturePose {FixtureV3 p;};
struct FixtureProfile {
    std::uint64_t key=1,generation=1;int classId=4,side=0;
    bool primary=true,support=true,twoHanded=true,supportPositionOnly174r2=false;
    unsigned supportSource37=1,slotCheck37=0;
    FixturePose grip[2]{};FixtureV3 tip{};
    bool melee() const noexcept {return classId==4;}
};
struct FixtureGrip {
    bool ready[2]{true,true};unsigned stable[2]{1,1};double geometryScale[3]{1,1,1};
    struct Binding {bool valid=true;void* character=nullptr;int joint=2;
        struct Relative {double q[4]{0,0,0,1},t[3]{};} relative;
        double scale[3]{1,1,1};} captureBinding;
};
static std::atomic<bool> g_support_slot34{true};
#include "physics_handling_gate_d84.inc"
static void BaselineWorker(bool consoleReady){
#include "physics_baseline_worker_d84.inc"
}
namespace physical {
#include "physics_exhaustion_math_d84.inc"
}
namespace kick26 {
static std::atomic<bool> henryExhaustedCLE{false};
static std::atomic<std::uint64_t> exhaustedSeenCLE{0};
}
struct FixtureInput {physical::Attributes attributes;};
static void Exhaustion(FixtureInput& in){
#include "physics_exhaustion_gate_d84.inc"
}
}

static void stopAll(){for(auto d:{D::Physics,D::Vrik,D::Combat,D::Interaction,D::Reload,D::Bug})cap::gate.select(d,false,now);}
static void resetCounts(){clocks=formats=cvarQueries=cvarWrites=phaseQueries=bodyQueries=sourceQueries=0;kcd2vr::diagnostic_trace::rows.clear();}
static unsigned checks=0;
#define CK(x) do {++checks;if(!(x)){std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(){
    using namespace kcd2vr;
    namespace body=native_weapon_body_t4;
    namespace vf=visible_failure;
    FixtureProfile p;FixtureGrip grip;
    // All recorder gates are cold. Host gameplay snapshots continue, while
    // diagnostic clocks, formatting, phase reads and baseline CVars stay cold.
    for(unsigned i=0;i<1000;++i){HandlingDiagnostics(p,grip);ServicePhysicalHeldF9T4();BaselineWorker(true);}
    ApplyRagdollRung19c4r4(0);
    CK(clocks==0&&formats==0&&cvarQueries==0&&phaseQueries==0&&diagnostic_trace::rows.empty());
    CK(bodyQueries==1000&&sourceQueries==1000);
    // OFF did not consume Mod's actual once-only baseline latch.
    cap::gate.select(D::Physics,true,now);BaselineWorker(true);
    CK(cvarQueries>0&&formats>0&&diagnostic_trace::rows.size()==1&&cvarWrites==0);
    const auto baselineReads=cvarQueries;BaselineWorker(true);CK(cvarQueries==baselineReads);
    stopAll();cap::gate.select(D::Physics,true,now);BaselineWorker(true);CK(cvarQueries==baselineReads);
    stopAll();
    for(auto domain:{D::Physics,D::Vrik,D::Combat,D::Interaction,D::Reload}){
        now+=20;cap::gate.select(domain,true,now);resetCounts();++p.key;
        HandlingDiagnostics(p,grip);
        CK(formats==2&&diagnostic_trace::rows.size()==2&&cvarQueries==0&&phaseQueries==0);
        CK(diagnostic_trace::rows[0].find("GRIP_SOCKET")!=std::string::npos);
        CK(diagnostic_trace::rows[1].find("SUPPORT_SOURCE")!=std::string::npos);
        CK(HandlingCaptureD84().domain==domain);stopAll();
    }
    cap::gate.select(D::Bug,true,now);resetCounts();++p.key;HandlingDiagnostics(p,grip);
    CK(clocks==0&&formats==0&&diagnostic_trace::rows.empty());stopAll();
    // Status routes are deliberately narrower: F9/F3/F2 only. No extra phase
    // read or formatting for an unchanged observation in the same session.
    for(auto domain:{D::Physics,D::Combat,D::Bug}){
        cap::gate.select(domain,true,now);resetCounts();ServicePhysicalHeldF9T4();
        CK(phaseQueries==1&&formats==1&&diagnostic_trace::rows.size()==1);
        ServicePhysicalHeldF9T4();CK(phaseQueries==1&&formats==1&&diagnostic_trace::rows.size()==1);
        CK(PhysicsStatusCaptureD84().domain==domain);stopAll();
    }
    for(auto domain:{D::Vrik,D::Interaction,D::Reload}){
        cap::gate.select(domain,true,now);resetCounts();ServicePhysicalHeldF9T4();
        CK(clocks==0&&phaseQueries==0&&formats==0&&bodyQueries==1&&sourceQueries==1);stopAll();
    }
    // Genuine native retirement acknowledgment remains effective while quiet.
    resetCounts();body::readValue.safeNativeRestore=true;g_legacy_phys_suspended_t4=true;
    const auto bodiesBefore=frik_pose_probe::bodies;ServicePhysicalHeldF9T4();
    CK(!g_legacy_phys_suspended_t4&&frik_pose_probe::bodies==bodiesBefore+1);
    CK(clocks==0&&phaseQueries==0&&formats==0&&diagnostic_trace::rows.empty());
    // Existing D80 status publication remains live OFF. Session-zero messages
    // use the log-only mailbox; they do not authorize an on-screen popup.
    g_physical_held_f9_t4=2;frik_pose_probe::source=frik_pose_probe::HeldPhysicsSourceT4::NoHeldSword;
    now+=3000;vf::lastDispatched=0;const auto visibleBefore=vf::nextSerial.load();resetCounts();
    ServicePhysicalHeldF9T4();
    CK(vf::nextSerial==visibleBefore+1&&clocks==1&&formats==0&&phaseQueries==0);
    vf::Message visible{};CK(vf::logMailboxD80.read(visible)&&visible.tick==now&&vf::safe(visible.text));
    CK(visible.diagnosticSessionD74==0&&!vf::diagnosticAllowedD74(visible,now));
    body::installOK=false;now+=3000;vf::lastDispatched=0;const auto refusalBefore=vf::nextSerial.load();
    const auto modeBefore=g_physical_held_f9_t4.load();resetCounts();ApplyPhysicalHeldF9T4(2);
    CK(g_physical_held_f9_t4==modeBefore&&g_f9_failed_request_h6&&vf::nextSerial==refusalBefore+1);
    CK(clocks==1&&formats==0&&phaseQueries==0&&diagnostic_trace::rows.empty());
    // A recorder restart during formatting cannot adopt the old ticket.
    g_physical_held_f9_t4=0;frik_pose_probe::source=frik_pose_probe::HeldPhysicsSourceT4::Ready;
    cap::gate.select(D::Physics,true,now);resetCounts();restartAtFormat=true;restartDomain=D::Physics;
    ServicePhysicalHeldF9T4();CK(formats==1&&phaseQueries==1&&diagnostic_trace::rows.empty());
    ServicePhysicalHeldF9T4();CK(formats==2&&phaseQueries==2&&diagnostic_trace::rows.size()==1);
    stopAll();cap::gate.select(D::Interaction,true,now);resetCounts();++p.key;
    restartAtFormat=true;restartDomain=D::Interaction;HandlingDiagnostics(p,grip);
    CK(formats==2&&diagnostic_trace::rows.size()==1); // first old GRIP rejected, fresh SUPPORT accepted
    CK(diagnostic_trace::rows[0].find("SUPPORT_SOURCE")!=std::string::npos);stopAll();
    // Exhaustion's gameplay state/timestamp still updates with capture OFF.
    FixtureInput input;input.attributes.live=true;input.attributes.stamina=0;resetCounts();Exhaustion(input);
    CK(kick26::henryExhaustedCLE&&kick26::exhaustedSeenCLE==now&&clocks==1&&formats==0);
    input.attributes.stamina=1;resetCounts();Exhaustion(input);
    CK(!kick26::henryExhaustedCLE&&kick26::exhaustedSeenCLE==0&&clocks==0&&formats==0);
    for(auto domain:{D::Physics,D::Combat,D::Vrik,D::Interaction,D::Reload}){
        cap::gate.select(domain,true,now);resetCounts();input.attributes.stamina=0;Exhaustion(input);
        const bool allowed=domain==D::Physics||domain==D::Combat;
        CK(formats==(allowed?1u:0u)&&diagnostic_trace::rows.size()==(allowed?1u:0u));
        CK(kick26::henryExhaustedCLE&&kick26::exhaustedSeenCLE==now);
        input.attributes.stamina=1;Exhaustion(input);stopAll();
    }
    std::printf("physics_gate_producers_d84: %u checks; literal profile/host/Mod/sink bodies, quiet work counters, all allowed routes, restart, retirement, quiet failure-status publication, baseline latch and exhaustion PASS\n",checks);
}
