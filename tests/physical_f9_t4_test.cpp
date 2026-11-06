#include <atomic>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
#include "native_weapon_body_t4.h"
#include "vrik/lab58.h"
#include "native_physics_phase_t4.h"
#include "vrik/native_held_owner_t4_status.h"
#include "core/collab_test_h21.h"
namespace kcd2vr {
namespace native_physics_phase_t4 {Receipt ReadReceipt() noexcept {return {};}}
namespace native_weapon_body_t4 {
static Achieved result{};
static bool readable=true,installOK=true,enabled=false;
static int installs=0,cancels=0;
void SetEnabled(bool value) noexcept {enabled=value;}
bool Install() noexcept {++installs;return installOK;}
void Cancel() noexcept {++cancels;}
bool Read(Achieved& out) noexcept {out=result;return readable;}
const char* ReasonName(Reason) noexcept {return "TEST_REASON";}
}
namespace combat_diag_probe {
static bool observed=false;
static bool physicalNpc=false;
void SetContactImpulse596(bool on,float,float,float) {physicalNpc=on;}
void SetContactImpulseDiagnostics19c4r4(bool on) {observed=on;}
}
namespace frik_pose_probe {
static int runtime=-1;
void SetPhysBodies59c10r8(int mode,float,float) {runtime=mode;}
HeldPhysicsSourceT4 HeldPhysicsSourceReasonT4() noexcept {return HeldPhysicsSourceT4::Ready;}
const char* HeldPhysicsSourceNameT4(HeldPhysicsSourceT4) noexcept {return "TEST_SOURCE";}
}
namespace diagnostic_trace {
static std::vector<std::string> lines;
void Write(const char* s) {lines.emplace_back(s);}
}
static std::atomic<bool> g_ragdoll_wanted59{false};
static std::atomic<int> g_ragdoll_rung19c4r4{0};
static std::atomic<bool> g_contact_impulse596{true};static std::atomic<float> g_contact_impulse_scale596{1};
static float RagdollPushScale33() noexcept{return 1.f;}static float RagdollPushCap33() noexcept{return 40.f;}
static void ApplyRagdollRung19c4r4(int rung) noexcept {g_ragdoll_rung19c4r4.store(rung);}
struct RagdollH6 {bool on=false;bool On()const noexcept{return on;}} g_ragdoll59;
static bool ragdollInstallH6=true;static int ragdollCallsH6=0;
static void ApplyRagdoll59(bool want) noexcept {++ragdollCallsH6;g_ragdoll59.on=want&&ragdollInstallH6;g_ragdoll_wanted59=g_ragdoll59.on;combat_diag_probe::physicalNpc=g_ragdoll59.on&&g_contact_impulse596;combat_diag_probe::observed=false;}
static std::atomic<int> g_phys_bodies59c10r8{1};
static std::atomic<float> g_phys_hand_mass59c10r8{3},g_phys_blade_mass59c10r8{4};
#include <physical_f9_t4.inc> // Build-directory extraction of current production control plane.
}
int main() {
 using namespace kcd2vr;namespace body=native_weapon_body_t4;
 ApplyLegacyPhysBodiesT4();assert(frik_pose_probe::runtime==1);
 ApplyPhysicalHeldF9T4(1);
 assert(combat_diag_probe::observed&&!body::enabled&&g_phys_bodies59c10r8==1);
 assert(!g_ragdoll_wanted59&&g_f9_saved_ragdoll64==0&&!RagdollPreference64()&&!combat_diag_probe::physicalNpc&&!collab_h21::enabled);
 const int beforeRefusalCancels=body::cancels;
 g_ragdoll_wanted59=true;g_ragdoll_rung19c4r4=1;
 body::installOK=false;ApplyPhysicalHeldF9T4(2);
 assert(g_physical_held_f9_t4==1&&!body::enabled&&combat_diag_probe::observed);
 assert(!g_legacy_phys_suspended_t4&&frik_pose_probe::runtime==1&&g_phys_bodies59c10r8==1);
 assert(body::cancels==beforeRefusalCancels&&g_ragdoll_wanted59&&g_ragdoll_rung19c4r4==1);
 assert(g_f9_saved_ragdoll64==0&&kcd2vr::lab58::rung==0);
 assert(diagnostic_trace::lines.back().find("NATIVE_UNAVAILABLE")!=std::string::npos);
 assert(NextPhysicalHeldF9H6()==0); // A failed installer cannot trap the cycle at rung 1.
 body::installOK=true;ApplyPhysicalHeldF9T4(2);assert(body::enabled);
 body::readable=false;ApplyPhysicalHeldF9T4(0);ServicePhysicalHeldF9T4();
 assert(g_legacy_phys_suspended_t4&&frik_pose_probe::runtime==0&&!body::enabled);
 body::readable=true;body::result.safeNativeRestore=false;ServicePhysicalHeldF9T4();
 assert(g_legacy_phys_suspended_t4&&frik_pose_probe::runtime==0);
 g_phys_bodies59c10r8=2;ApplyLegacyPhysBodiesT4();assert(frik_pose_probe::runtime==0);
 const auto logCount=diagnostic_trace::lines.size();ServicePhysicalHeldF9T4();
 assert(logCount==diagnostic_trace::lines.size());
 // A quick second cycle must not resume the old proxy between native retirements.
 ApplyPhysicalHeldF9T4(1);ApplyPhysicalHeldF9T4(2);
 ApplyPhysicalHeldF9T4(3);assert(g_physical_held_f9_t4==3&&!body::enabled&&kcd2vr::lab58::rung==3&&collab_h21::npcPhysics()&&!combat_diag_probe::physicalNpc);
 ApplyPhysicalHeldF9T4(2);assert(body::enabled&&kcd2vr::lab58::rung==0);
 body::result.safeNativeRestore=true;ServicePhysicalHeldF9T4();
 assert(g_legacy_phys_suspended_t4&&frik_pose_probe::runtime==0);
 ApplyPhysicalHeldF9T4(0);body::result.safeNativeRestore=false;ServicePhysicalHeldF9T4();
 assert(g_legacy_phys_suspended_t4&&frik_pose_probe::runtime==0);
 ApplyPhysicalHeldF9T4(1);body::result.safeNativeRestore=true;ServicePhysicalHeldF9T4();
 assert(!g_legacy_phys_suspended_t4&&frik_pose_probe::runtime==2&&g_phys_bodies59c10r8==2);
 assert(g_physical_held_f9_t4==1&&combat_diag_probe::observed);
 std::puts("T4 actual F9 control: persisted preference, refused install, retirement and rapid cycling PASS");
 // Menu snapshot/save must retain suspended preference while F9 owns runtime.
 ApplyPhysicalHeldF9T4(0);g_ragdoll_wanted59=true;ApplyPhysicalHeldF9T4(2);
 assert(!g_ragdoll_wanted59&&RagdollPreference64()&&g_f9_saved_ragdoll64==1);
 ApplyPhysicalHeldF9T4(3);assert(RagdollPreference64());
 body::result.safeNativeRestore=true;ServicePhysicalHeldF9T4();ServiceRagdollPushH6(true);ApplyPhysicalHeldF9T4(0);ServiceRagdollPushH6(true);assert(g_ragdoll_wanted59&&RagdollPreference64()&&g_f9_saved_ragdoll64==-1&&combat_diag_probe::physicalNpc);
 ApplyPhysicalHeldF9T4(2);g_f9_saved_ragdoll64=0;ApplyPhysicalHeldF9T4(0);
 assert(!g_ragdoll_wanted59&&!RagdollPreference64());
 // H6: all cycle paths restore exactly the user preference; observation rung
 // retains its active request across menu snapshots/repeated selection.
 for(const bool preference:{false,true}){
  ApplyPhysicalHeldF9T4(0);g_ragdoll_wanted59=preference;
  ApplyPhysicalHeldF9T4(1);assert(!g_ragdoll_wanted59&&RagdollPreference64()==preference);
  ApplyPhysicalHeldF9T4(1);assert(RagdollPreference64()==preference);
  ApplyPhysicalHeldF9T4(2);assert(!g_ragdoll_wanted59&&RagdollPreference64()==preference);
  ApplyPhysicalHeldF9T4(3);assert(g_ragdoll_wanted59&&RagdollPreference64()==preference);
  ApplyPhysicalHeldF9T4(1);assert(!g_ragdoll_wanted59&&RagdollPreference64()==preference);
  ApplyPhysicalHeldF9T4(0);assert(g_ragdoll_wanted59==preference&&g_f9_saved_ragdoll64==-1);
 }
 ApplyPhysicalHeldF9T4(1);g_f9_saved_ragdoll64=1;ApplyPhysicalHeldF9T4(0);assert(g_ragdoll_wanted59);
 ApplyPhysicalHeldF9T4(1);g_f9_saved_ragdoll64=0;ApplyPhysicalHeldF9T4(2);ApplyPhysicalHeldF9T4(0);assert(!g_ragdoll_wanted59);

 // Runtime corpse push cannot resume until native collider retirement ACK.
 ApplyPhysicalHeldF9T4(1);g_legacy_phys_suspended_t4=true;g_ragdoll59.on=false;
 const int beforeRetireH6=ragdollCallsH6;ServiceRagdollPushH6(true);
 assert(!g_ragdoll59.On()&&!g_ragdoll_wanted59&&ragdollCallsH6==beforeRetireH6);
 g_legacy_phys_suspended_t4=false;ServiceRagdollPushH6(true);assert(!g_ragdoll59.On()&&combat_diag_probe::observed&&ragdollCallsH6==beforeRetireH6);
 ApplyPhysicalHeldF9T4(0);ServiceRagdollPushH6(true);assert(!g_ragdoll59.On());
 // Native install refusal settles OFF rather than retrying each frame.
 ragdollInstallH6=false;ApplyPhysicalHeldF9T4(1);ServiceRagdollPushH6(true);
 assert(!g_ragdoll59.On()&&!g_ragdoll_wanted59);
 const int refusedH6=ragdollCallsH6;ServiceRagdollPushH6(true);assert(ragdollCallsH6==refusedH6);
 ragdollInstallH6=true;ApplyPhysicalHeldF9T4(0);g_ragdoll_wanted59=true;ServiceRagdollPushH6(true);
 assert(g_ragdoll59.On()&&g_ragdoll_wanted59&&combat_diag_probe::physicalNpc); // H26 user restores default weapon corpse push

 // H26 C46 restoration: entering corpse rung never installs/enables a T4
 // weapon. A prior collider must retire before the tracked contact producer
 // and native corpse impulse can run; old settings are not the authority.
 ragdollInstallH6=true;body::installOK=true;
 ApplyPhysicalHeldF9T4(0);g_ragdoll_wanted59=false;
 ApplyPhysicalHeldF9T4(2);assert(body::enabled&&g_legacy_phys_suspended_t4);
 const int installsBeforeCorpse=body::installs;
 body::result.safeNativeRestore=false;
 ApplyPhysicalHeldF9T4(3);
 assert(!body::enabled&&body::installs==installsBeforeCorpse&&g_ragdoll_wanted59);
 assert(!combat_diag_probe::physicalNpc&&collab_h21::npcPhysics());
 ServicePhysicalHeldF9T4();ServiceRagdollPushH6(true);
 assert(g_legacy_phys_suspended_t4&&!g_ragdoll59.On()&&!combat_diag_probe::physicalNpc);
 body::result.safeNativeRestore=true;ServicePhysicalHeldF9T4();ServiceRagdollPushH6(true);
 assert(!g_legacy_phys_suspended_t4&&g_ragdoll59.On()&&combat_diag_probe::physicalNpc);
 assert(frik_pose_probe::runtime==g_phys_bodies59c10r8);
 ApplyPhysicalHeldF9T4(1);ServiceRagdollPushH6(true);
 assert(!g_ragdoll59.On()&&!combat_diag_probe::physicalNpc&&combat_diag_probe::observed);
 ApplyPhysicalHeldF9T4(0);assert(!g_ragdoll_wanted59&&!RagdollPreference64());
 // Default survives leaving the explicit corpse rung while lease remains ON.
 g_ragdoll_wanted59=true;ApplyPhysicalHeldF9T4(3);ServiceRagdollPushH6(true);
 const int callsBeforeDefault=ragdollCallsH6;ApplyPhysicalHeldF9T4(0);ServiceRagdollPushH6(true);
 assert(g_ragdoll59.On()&&combat_diag_probe::physicalNpc&&ragdollCallsH6==callsBeforeDefault);
 g_ragdoll_wanted59=false;ServiceRagdollPushH6(true);
 // Direct entry remains usable if the unrelated T4 weapon installer refuses.
 body::installOK=false;ApplyPhysicalHeldF9T4(3);ServiceRagdollPushH6(true);
 assert(g_physical_held_f9_t4==3&&!body::enabled&&g_ragdoll59.On());
 ApplyPhysicalHeldF9T4(0);ServiceRagdollPushH6(true);
 assert(!combat_diag_probe::physicalNpc&&!g_ragdoll59.On());
 // Failed observer installation settles OFF; never a per-frame retry storm.
 ragdollInstallH6=false;ApplyPhysicalHeldF9T4(3);ServiceRagdollPushH6(true);
 const int refusedCorpse=ragdollCallsH6;ServiceRagdollPushH6(true);
 assert(!g_ragdoll59.On()&&!g_ragdoll_wanted59&&ragdollCallsH6==refusedCorpse);
 std::puts("H26 C46 tracked-weapon corpse push owner: rung3 enable, retirement, OFF, saved preference and refusal PASS");

}
