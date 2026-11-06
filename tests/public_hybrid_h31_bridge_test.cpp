// Actual public mode application -> actual pose-adapter policy -> real kernel.
// Cached synthetic installer receipts replace retail hook installation only.
#define ArmF4ContactGate176 FixtureContactGateH31
#include "../src/frik_pose_probe.cpp"
#undef ArmF4ContactGate176
#include "gesture_recovery_h30_fixture.h"
static bool contactGateH31=true;
namespace kcd2vr::combat_diag_probe {bool FixtureContactGateH31() noexcept{return contactGateH31;}}
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::animation_filter {
void SetMode(SuppressionMode) noexcept {} bool Install() noexcept{return false;} bool Ready() noexcept{return false;}
}
namespace kcd2vr::native_stereo_outer_lab {bool EnsurePhysicalFramePump6698() noexcept{return true;}}
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
using namespace kcd2vr::frik_pose_probe;
// These are the actual statements extracted from the pose adapter by
// tools/extract_public_hybrid_h31.py, not a hand-maintained test selector.
namespace kcd2vr::frik_pose_probe {static void actualAdapterH31(ph::Input& pi) {
#include "generated_public_hybrid_h31.inc"
}}
int main(){
 std::vector<unsigned char> image(0x5400000);hostModuleHandleW=image.data();g_wh_pinned104=0;hostTickCount64=1000;
 const auto base=WhBase();kcd2vr::retail_build112::hostRetailVerified112=true;
 g_saved_arm_ready66100=true;g_arm_capture_active66100=false;
 g_native_contract104=g_input_contract117=g_combat_contract101=base;
 g_phys_armed6698=true;g_prediction_contract120=g_bow_zoom_contract121=g_attack_timing_contract22=base;
 g_close_combat_ready175=g_master_gate_ready168=true;
 g_combat_input_original101=+[](void*,const std::uint64_t*,int,float){};
 SetStagedBowDefault59(false);
 namespace modes=kcd2vr::user175;
 for(auto modeH31:{modes::Mode::CombatSynchronized,modes::default59(modes::Mode::Calibrated,true)}){
  ckH30(ApplyUserMode175(modeH31),"actual ApplyUserMode175 admits public F4 rung1/default HYBRID with qualified native installation");
  ckH30(CombatSynchronized59c10r9()&&PhysicalCombat177Enabled()&&FullWeaponMode120(WeaponControlMode()),"actual public API sets synchronized/full-weapon/physical policy");
  ckH30(!CombatIntent59c16()&&!g_intent_pi59c16.load()&&!g_intent59c16.load(),"public HYBRID leaves all legacy R16 intent route switches OFF");
  ckH30(!kcd2vr::motion59c4::selected.load(),"public HYBRID does not borrow DIRECT motion ownership");
  RigH30 cut(false);actualAdapterH31(cut.in);
  ckH30(ph::normal_hybrid_intent_h31(cut.in)&&ph::normal_intent_enabled_h30(cut.in)&&!cut.in.intent59c16,"actual pose adapter automatically enables current HYBRID intent with no extra switch");
  moveH30(cut,10);const auto* prepare=firstH30(cut,ph::CommandKind::Prepare);const auto gesture=prepare?prepare->gesture59c3:0;
  gapH30(cut,3);restoreH30(cut,6);const auto* committed=firstH30(cut,ph::CommandKind::Slash);
  ckH30(gesture&&committed&&cut.count(ph::CommandKind::Prepare)==1&&cut.count(ph::CommandKind::Slash)==1&&!cut.count(ph::CommandKind::Abort)&&committed->gesture59c3==gesture,"public-mode qualified cut retains one actual gesture through transient presentation recovery");
  RigH30 stab(false,{0,1,0});actualAdapterH31(stab.in);moveH30(stab,14);
  ckH30(stab.count(ph::CommandKind::Prepare)==1&&stab.count(ph::CommandKind::Thrust)==1&&!stab.count(ph::CommandKind::Abort),"public-mode forward stab produces one native attack command with legacy intent OFF");
  RigH30 windup(false);actualAdapterH31(windup.in);for(unsigned i=0;i<22;++i)windup.step({0,-.025,0});
  ckH30(!windup.hits()&&windup.count(ph::CommandKind::Prepare)==0,"public-mode backward strokes stay windups, never attacks");
  // Recorder leases are observational: neither OFF nor ON is producer authority.
  kcd2vr::diagnostics::capture168::gate.mask=0;RigH30 recorded(false);actualAdapterH31(recorded.in);
  const auto policy=ph::normal_hybrid_intent_h31(recorded.in);
  kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,hostTickCount64);
  actualAdapterH31(recorded.in);ckH30(policy&&ph::normal_hybrid_intent_h31(recorded.in),"F3 recorder changes do not enable or disable automatic normal HYBRID intent");
 }
 ckH30(ApplyUserMode175(modes::Mode::CombatContactLab),"actual public API selects DIRECT comparison");
 RigH30 direct(true);actualAdapterH31(direct.in);
 ckH30(direct.in.motionOwned59c5&&direct.in.forwardOnly19&&!ph::normal_hybrid_intent_h31(direct.in)&&!direct.in.intent59c16,"actual DIRECT mode remains its own owner, without normal-HYBRID or R16 opt-in");
 ckH30(ApplyUserMode175(modes::Mode::Off),"actual public API releases combat mode");
 auto off=direct.in;actualAdapterH31(off);
 ckH30(!off.physicalCombat177&&!off.forwardOnly19&&!ph::normal_intent_enabled_h30(off),"actual OFF mode removes automatic normal intent");
 g_saved_arm_ready66100=false;g_cal_state=0;
 ckH30(!ApplyUserMode175(modes::Mode::CombatSynchronized),"removing redundant intent flag does not bypass calibration/native ownership prerequisites");
 actualAdapterH31(off);ckH30(!ph::normal_intent_enabled_h30(off),"refused mode cannot manufacture producer authority");
 g_saved_arm_ready66100=true;contactGateH31=false;
 ckH30(!ApplyUserMode175(modes::Mode::CombatSynchronized),"actual F4 installer refusal still rejects HYBRID instead of bypassing native contract");
 actualAdapterH31(off);ckH30(!ph::normal_intent_enabled_h30(off),"native-installation refusal does not silently activate normal combat producer");
 contactGateH31=true;kcd2vr::retail_build112::hostRetailVerified112=false;
 ckH30(!ApplyUserMode175(modes::Mode::CombatSynchronized),"exact retail readiness remains necessary even with cached synthetic native receipts");
 actualAdapterH31(off);ckH30(!ph::normal_intent_enabled_h30(off),"retail mismatch cannot become normal intent authority");
 std::printf("%s %u/%u actual public-mode/adapter/kernel checks; native installers substituted, retail instructions NOT executed\n",failuresH30?"FAIL":"PASS",checksH30-failuresH30,checksH30);return failuresH30?1:0;
}
