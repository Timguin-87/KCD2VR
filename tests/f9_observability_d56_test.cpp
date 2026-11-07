#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include "diagnostics/capture_ticket_d84.h"
#include "ragdoll_lease59.h"
#include "native_weapon_body_t4.h"
#include "native_physics_phase_t4.h"
#include "vrik/native_held_owner_t4_status.h"
#include "vrik/lab58.h"
#include "vrik/visible_failure.h"
#include "core/collab_test_h21.h"
#include "subsystems/physics/f9_status_fd.h"

// Native API leaves are controlled; the F9 branch, preferences, publication,
// mailbox validation/limiter and recorder routing below are production code.
static std::uint64_t tickD56=10000;
static std::uint64_t GetTickCount64() noexcept {return tickD56;}
namespace kcd2vr {
namespace native_physics_phase_t4 {Receipt ReadReceipt() noexcept {return {};}}
namespace native_weapon_body_t4 {
static bool installOK=false,enabled=false;
static unsigned installs=0,mutations=0;
bool Install() noexcept {++installs;return installOK;}
void SetEnabled(bool on) noexcept {++mutations;enabled=on;}
void Cancel() noexcept {++mutations;}
bool Read(Achieved& out) noexcept {out={};return true;}
const char* ReasonName(Reason) noexcept {return "TEST_REASON";}
}
namespace combat_diag_probe {
static unsigned mutations=0;
bool Install() noexcept {return true;}
void SetContactImpulse596(bool,float,float,float) {++mutations;}
void SetContactImpulseDiagnostics19c4r4(bool) {++mutations;}
}
namespace frik_pose_probe {
static unsigned mutations=0;
bool PrepareNativeHandPhysicsH24() noexcept {return true;}
void SetPhysBodies59c10r8(int,float,float) {++mutations;}
HeldPhysicsSourceT4 HeldPhysicsSourceReasonT4() noexcept {return HeldPhysicsSourceT4::Ready;}
const char* HeldPhysicsSourceNameT4(HeldPhysicsSourceT4) noexcept {return "TEST_SOURCE";}
}
namespace diagnostic_trace {
static std::vector<std::string> lines;
void Write(const char* s) {lines.emplace_back(s);}
void WriteReceiptFor(diagnostics::capture168::Domain d,std::uint64_t session,const char* s) {
    if(diagnostics::capture168::gate.receipt(d,session,tickD56))lines.emplace_back(s);
}
}
static unsigned ragdollMutationsD56=0;
static void* SafeGetCryConsole() noexcept {return nullptr;}
static void* SafeConsoleGetCVar(void*,const char*) noexcept {return nullptr;}
static float SafeCVarGetFloat(void*,float fallback) noexcept {return fallback;}
static bool SafeCVarSetFloat(void*,float) noexcept {++ragdollMutationsD56;return false;}
#define KCD2VR_M2_HOST_SLOT 5007
#include "subsystems/physics/host.inl"
#undef KCD2VR_M2_HOST_SLOT
#define KCD2VR_M2_HOST_SLOT 5009
#include "subsystems/physics/host.inl"
#undef KCD2VR_M2_HOST_SLOT
#define KCD2VR_M2_HOST_SLOT 5011
#include "subsystems/physics/host.inl"
#undef KCD2VR_M2_HOST_SLOT
}

static unsigned checksD56=0;
#define CK(x) do {++checksD56;if(!(x)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}} while(0)
int main() {
    using namespace kcd2vr;
    namespace body=native_weapon_body_t4;
    namespace vf=visible_failure;
    namespace cap=diagnostics::capture168;
    // Execute the actual installation-failure branch from all previous modes.
    // No mode, body, corpse, proxy, pair epoch or saved preference may change.
    for(int previous=0;previous<=3;++previous)for(bool preference:{false,true}) {
        tickD56+=3000;
        g_physical_held_f9_t4=previous;g_f9_saved_ragdoll64=preference?1:0;
        g_f9_failed_request_h6=false;g_ragdoll_wanted59=preference;
        g_ragdoll_rung19c4r4=previous;g_legacy_phys_suspended_t4=previous==2;
        g_phys_bodies59c10r8=2;
        lab58::rung=previous==3?3:0;lab58::f9Rung=previous;
        collab_h21::setPhysicsMode(previous);body::enabled=previous==2;
        body::installOK=false;vf::lastDispatched=0;
        const auto epoch=collab_h21::epoch.load();
        const auto serial=vf::nextSerial.load(),installs=body::installs;
        const std::array<unsigned,4> before{body::mutations,combat_diag_probe::mutations,
            frik_pose_probe::mutations,ragdollMutationsD56};
        const auto captureMask=cap::gate.mask.load();
        ApplyPhysicalHeldF9T4(2);
        CK(body::installs==installs+1&&g_f9_failed_request_h6&&NextPhysicalHeldF9H6()==0);
        CK(g_physical_held_f9_t4==previous&&g_f9_saved_ragdoll64==(preference?1:0));
        CK(g_ragdoll_wanted59==preference&&g_ragdoll_rung19c4r4==previous);
        CK(body::enabled==(previous==2)&&g_legacy_phys_suspended_t4==(previous==2));
        CK(g_phys_bodies59c10r8==2&&g_phys_hand_mass59c10r8==3.f&&g_phys_blade_mass59c10r8==4.f);
        CK(lab58::rung==(previous==3?3:0)&&lab58::f9Rung==previous);
        CK(collab_h21::physicsMode==previous&&collab_h21::epoch==epoch);
        CK((before==std::array<unsigned,4>{body::mutations,combat_diag_probe::mutations,
            frik_pose_probe::mutations,ragdollMutationsD56}));
        CK(cap::gate.mask==captureMask);
        CK(diagnostic_trace::lines.back().find("NATIVE_UNAVAILABLE")!=std::string::npos);
        vf::Message message{};
        CK(vf::logMailboxD80.read(message)&&message.serial==serial+1&&message.tick==tickD56);
        CK(vf::safe(message.text)&&std::string(message.text)==f9_status_fd::installRefusalText());
        CK(message.diagnosticSessionD74==0&&!vf::diagnosticAllowedD74(message,tickD56));
    }
    // D80 quiet-mode failures use the bounded log-only mailbox. A zero-session
    // status is not an on-screen diagnostic and cannot borrow another recorder.
    vf::lastDispatched=tickD56;const auto limitedSerial=vf::nextSerial.load();
    ApplyPhysicalHeldF9T4(2);CK(vf::nextSerial==limitedSerial);
    tickD56+=vf::kMinIntervalMs;ApplyPhysicalHeldF9T4(2);CK(vf::nextSerial==limitedSerial+1);
    // A later successful request still uses the unchanged rung-two ownership.
    vf::lastDispatched=0;body::installOK=true;ApplyPhysicalHeldF9T4(2);
    CK(g_physical_held_f9_t4==2&&!g_f9_failed_request_h6&&body::enabled&&g_legacy_phys_suspended_t4);

    constexpr std::array<const char*,10> families{
        "T4 F9 rung=2", "B2 F9_INSTALL stage=ENTER", "B3 F9 requested=PHYSICAL_HELD",
        "H21 F9 rung=1", "H26 F9 rung=3", "C61 F9 rung=2", "C57R1 F9 status=OFF",
        "C4R4 F9 refused: exact native contact observer unavailable",
        "C4R4 RAGDOLL_F9 rung=3", "C58 LAB RUNG_NOW=3"};
    for(const auto* family:families) {
        // Misleading reason words must not redirect the family to another key.
        const std::string message=std::string(family)+" reason=HUD_INPUT_OWNER_FIST_FAULT";
        CK(cap::f9FamilyD56(message.c_str())&&cap::domain(message.c_str())==cap::Domain::Physics);
        CK(!cap::essential(message.c_str()));
        cap::gate.mask=0;CK(!cap::accept(message.c_str(),1000));
        for(auto domain:{cap::Domain::Physics,cap::Domain::Combat,cap::Domain::Bug,cap::Domain::Interaction,
                         cap::Domain::Vrik,cap::Domain::Reload}) {
            cap::gate.mask=0;cap::gate.select(domain,true,1000);
            CK(cap::accept(message.c_str(),1001)==(domain==cap::Domain::Physics||domain==cap::Domain::Combat||domain==cap::Domain::Bug));
            CK(!cap::accept(message.c_str(),1000+cap::Duration));
            cap::gate.select(domain,false,1002);CK(!cap::accept(message.c_str(),1002));
        }
    }
    cap::gate.mask=0;cap::gate.select(cap::Domain::Combat,true,1000);
    for(const auto* near: {"B2 F9_INSTALLATION stage=ENTER","C58 LAB RUNG_NOWISH=3",
            "B3 F9 requestedElsewhere=1","unrelated T4 F9 rung=2","F2 F9 NATIVE_ATTACHMENT key=1"}) {
        CK(!cap::f9FamilyD56(near)&&!cap::accept(near,1001));
    }
    CK(!cap::f9FamilyD56(nullptr));
    std::printf("f9_observability_d56: %u checks; actual failure branch, bounded quiet-status mailbox, unchanged ownership and finite F9/F2/F3 routing PASS\n",checksD56);
}
