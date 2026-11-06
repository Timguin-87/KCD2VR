// Compile and execute the COMPLETE production resource callback. Native event
// dispatch is an explicit leaf; no retail recipe/effect outcome is claimed.
// Retain actual MSVC SEH syntax in the two unrelated unconditional native
// finally scopes; the fixture's platform functions/memory remain explicit leaves.
#ifdef _MSC_VER
#include "syntax_shims/windows.h"
#include "syntax_shims/excpt.h"
#undef __try
#undef __except
#endif
#define KCD2VR_ALCHEMY_SEMI46_NO_MAIN
#include "alchemy_semi46_native_test.cpp"
// Unrelated retained knife/crossbow service leaves. They are not exercised by
// resource callbacks; fail closed and do not substitute any tested alchemy path.
namespace kcd2vr::native_weapon_body_t4 {
 bool ThrowAdmissionSuspendedT23() noexcept{return false;}bool HasUnretiredCollider() noexcept{return false;}
 bool EquipmentAdmissionSuspendedH36() noexcept{return false;}std::uint64_t AdmissionSerial() noexcept{return 0;}
}
namespace kcd2vr::native_script_t2 {bool Context::Current()const noexcept{return false;}}
namespace kcd2vr::frik_pose_probe {
 int CombatCensusMode() noexcept{return 0;}int WeaponControlMode() noexcept{return 0;}
 int ReloadMode47C2() noexcept{return 0;}
 std::uint64_t PoseEpochH24() noexcept{return 0;}
 bool HandClaimBlocksT24(int) noexcept{return false;}
 ThrowAuthorityT22 ThrowEquipmentAdmissionT25(int,std::uintptr_t,std::uintptr_t,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept{return ThrowAuthorityT22::Unknown;}
 bool HoverHaptic178r14(int,std::uint64_t)noexcept{return false;}
 bool NativeReloadHandRemoveEntryQualifiedT30(std::uintptr_t)noexcept{return false;}
 ThrowAuthorityT22 ThrowNativeAuthorityT22(int,std::uintptr_t,std::uintptr_t,std::uintptr_t,std::uint64_t,std::uint64_t)noexcept{return ThrowAuthorityT22::Unknown;}
 ThrowAuthorityT22 ThrowHandAdmissionT22(int,std::uintptr_t,std::uintptr_t,std::uintptr_t,std::uint64_t)noexcept{return ThrowAuthorityT22::Unknown;}
 bool ReadThrowModelGripT18(void*,float (&)[7])noexcept{return false;}
}
namespace kcd2vr::diagnostic_trace {void WriteFor(diagnostics::capture168::Domain,const char* text) noexcept{Write(text);}}
namespace kcd2vr::alchemy_finish_cb1 {bool Last(Result&) noexcept{return false;}}
namespace kcd2vr::action_time_owner59c11 { namespace {
static unsigned callsD60=0,checksD60=0;static bool offInHandlerD60=false;
static void checkD60(bool ok,const char* why){++checksD60;if(!ok){std::fprintf(stderr,"FAIL D60: %s\n",why);std::exit(1);}}
static void nativeD60(void* p,void* c,const void* e){++callsD60;NativeResourceEvent46(p,c,e);
 if(offInHandlerD60)kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,false,hostTickCount64);}
static std::vector<std::string> receiptsD60(){std::vector<std::string> out;for(const auto& s:lines)if(s.find("D60 ALCHEMY_RESOURCE_EVENT ")==0)out.push_back(s);return out;}
static void setupD60(bool capture=true){
 ResourceFixture46();lines.clear();traceEnabled=true;callsD60=0;offInHandlerD60=false;g_semi_resource_original46=nativeD60;
 namespace cap=kcd2vr::diagnostics::capture168;cap::gate.select(cap::Domain::Interaction,false,hostTickCount64);
 if(capture)cap::gate.select(cap::Domain::Interaction,true,hostTickCount64);
 kcd2vr::subsystems::alchemy::bridge67::control.publish({});
 kcd2vr::subsystems::alchemy::bridge67::tools[0].publish({});kcd2vr::subsystems::alchemy::bridge67::tools[1].publish({});
}
static void scanD60(float from,float to){SemiResourceScanDetour46(testSkel,false,from,to,&eventInfo46,nullptr,0);}
static int scenarioD60(){
 setupD60(false);scanD60(.14f,.85f);
 checkD60(callsD60==4&&nativeAttach46==1&&nativeUse46==1&&receiptsD60().empty(),"F5 off still calls all native events exactly once and emits no new receipts");
 setupD60();scanD60(.14f,.16f);scanD60(.30f,.40f);scanD60(.40f,.43f);scanD60(.80f,.85f);
 auto rows=receiptsD60();checkD60(rows.size()==4&&callsD60==4&&nativeUse46==1,"all four authored stages get one handler-return receipt");
 const char* names[]{"event=ATTACH ","event=DETACH_RESOURCE ","event=USE ","event=DETACH "};
 for(unsigned i=0;i<4;++i){checkD60(rows[i].find(names[i])!=std::string::npos,"native marker order remains Attach/DetachResource/Use/Detach");
  checkD60(rows[i].find("nativeHandler=RETURNED_ONCE")!=std::string::npos&&rows[i].find("semanticEffect=NOT_OBSERVED")!=std::string::npos&&
   rows[i].find("mouth=UNPROVEN emitter=UNJOINED")!=std::string::npos,"dispatch receipt cannot masquerade as a proven effect/mouth emitter");}
 checkD60(rows[1].find("intervalJoined=1")!=std::string::npos&&rows[1].find("nativeTime=0.394543")!=std::string::npos,"detach-resource has real current interval and authored event time");
 checkD60(rows[2].find("animation=42")!=std::string::npos&&rows[2].find("token=55")!=std::string::npos,"native child/token/animation stay joinable");
 // Copied presentation/admission joins are checked independently from event dispatch.
 setupD60();namespace ab=kcd2vr::subsystems::alchemy::bridge67;ab::Control c{};
 c.instance=reinterpret_cast<std::uintptr_t>(alchemy46.data());c.lifetime=kcd2vr::activity_input163::witness175.lifetime175.load();
 c.tick=hostTickCount64;c.parent=reinterpret_cast<std::uintptr_t>(parent46.data());c.poseEpoch=8;c.sourceEpoch=9;
 c.modeEpoch=kcd2vr::minigame_hands45::modeEpoch67.load();c.enabled=true;c.entity=77;c.type=13;c.side=1;c.held[1]=true;c.admissionTickR22=990;
 ab::NativeTool tool{};tool.instance=c.instance;tool.lifetime=c.lifetime;tool.tick=c.tick;tool.poseEpoch=c.poseEpoch;
 tool.parent=c.parent;tool.entity=c.entity;tool.type=c.type;tool.side=1;tool.joined=true;
 auto& b=tool.binding;b.instance=c.instance;b.lifetime=c.lifetime;b.action=c.parent;b.entity=c.entity;b.type=c.type;b.side=1;
 b.player=10;b.character=11;b.attachment=12;b.object=13;b.actionClass=14;
 ab::control.publish(c);ab::tools[1].publish(tool);Sample46(1000,.04);scanD60(.14f,.16f);
 rows=receiptsD60();checkD60(rows.size()==1&&rows[0].find("controlJoined=1 admission=990 heldEntity=77 type=13 side=1 heldMask=2")!=std::string::npos&&
  rows[0].find("toolJoined=1 attachment=C object=D")!=std::string::npos,"same current native owner joins held resource and pre-handler binding");
 checkD60(rows[0].find("producerJoined=1")!=std::string::npos,"same child/token joins the actual physical work ledger");
 ab::control.publish(c);tool.binding.lifetime++;ab::tools[1].publish(tool);scanD60(.30f,.40f);rows=receiptsD60();
 checkD60(rows.back().find("toolJoined=0 attachment=0 object=0")!=std::string::npos&&callsD60==2,"stale tool generation is not borrowed; original event still runs");
 c.lifetime++;ab::control.publish(c);scanD60(.40f,.43f);rows=receiptsD60();
 checkD60(rows.back().find("controlJoined=0 admission=0 heldEntity=0")!=std::string::npos,"foreign station lifetime cannot lend held item metadata");
 setupD60();offInHandlerD60=true;scanD60(.14f,.16f);
 checkD60(callsD60==1&&receiptsD60().empty(),"capture disabled inside original suppresses late receipt without changing dispatch");
 setupD60();for(int i=0;i<300;++i)scanD60(.30f,.40f);rows=receiptsD60();
 checkD60(callsD60==300&&rows.size()==257&&rows.back().find("BUDGET_EXHAUSTED")!=std::string::npos,"finite per-thread capture stops at256 plus one exhaustion notice; no event suppression");
 setupD60();pickupThread19c4=2;scanD60(.14f,.16f);pickupThread19c4=1;rows=receiptsD60();
 checkD60(callsD60==1&&rows.size()==1&&rows[0].find("foreign=1")!=std::string::npos,"foreign native callback still dispatches once and carries an explicit observer-lane receipt");
 // D64 regression: foreign Attach succeeds but the independent clock/FIFO
 // contract refuses. Keep the event identity without granting clock ownership.
 setupD60();g_semi_resource_pending19={};pickupThread19c4=2;scanD60(.14f,.16f);pickupThread19c4=1;
 checkD60(nativeAttach46==1&&g_semi_resource_pending19.event==0xDEC97C81u&&!g_semi_alchemy46.written,"foreign native Attach produces one pending observed event");
 {OwnerTransaction19c4 transaction;checkD60(bool(transaction),"test owns actual owner transaction");SemiResourceConsume19(controller.data(),hostTickCount64);}
 checkD60(g_semi_resource46.owner.child==reinterpret_cast<std::uintptr_t>(action.data())&&g_semi_resource46.phase==0&&!g_semi_alchemy46.written&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f,"refused clock leaves an Attach ledger without speed write");
 pickupThread19c4=2;scanD60(.40f,.43f);pickupThread19c4=1;
 checkD60(nativeUse46==1&&g_semi_resource_pending19.event==0x94B1CC4Bu,"real Use remains joinable after refused Attach clock lease");
 {OwnerTransaction19c4 transaction;SemiResourceConsume19(controller.data(),hostTickCount64);}
 checkD60(g_semi_resource46.phase==2&&!g_semi_alchemy46.written&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f,"real Use advances only native completion ledger, no replay or lease");
 // D64 knife diagnostics use the same production owner TU. The first
 // refused predicate is named without evaluating later native dependencies.
 native_script_t2::Context knifeContext{};st2::Input knifeInput{};const char* reasonD64=nullptr;
 checkD60(!AmmoChestAdmissionK35(knifeContext,knifeInput,false,&reasonD64)&&!std::strcmp(reasonD64,"CHEST_NOT_SELECTED"),"knife disabled reason preserves first short circuit");
 checkD60(!AmmoChestAdmissionK35(knifeContext,knifeInput,true,&reasonD64)&&!std::strcmp(reasonD64,"CHEST_CONTEXT"),"knife context refusal precedes native launcher reads");
 // Run established native interval/cost-preservation negatives on the changed TU.
 ResourceCases46();std::printf("D60 alchemy native resource receipts: %u checks PASS; existing resource cases=%u PASS\n",checksD60,checks46);
 return 0;
}
}}
#ifndef KCD2VR_RESOURCE_D60_NO_MAIN
int main(){return kcd2vr::action_time_owner59c11::scenarioD60();}
#endif
