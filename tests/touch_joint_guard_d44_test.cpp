// Retained production H9 suite plus the actual legacy and answered-hit callers.
#define main RetainedH9MainD44
#include "native_touch_existing.inc"
#undef main
#include "core/impact_identity59c6.h"
#include <fstream>
namespace kcd2vr::diagnostics::capture168{enum class Domain{Combat};static bool active(Domain,std::uint64_t){return true;}}
namespace kcd2vr::combat_diag_probe{
 struct LabAnsweredLiving58{std::uint32_t entity=0;int part=-1;std::uint64_t sample=0,tick=0;std::uint32_t serial=0;int reaction=-1;};
 static LabAnsweredLiving58 reply;static bool haveReply=false;
 static unsigned LabTakeAnsweredLiving58(LabAnsweredLiving58*out,unsigned cap){if(!haveReply||!cap)return 0;*out=reply;haveReply=false;return 1;}
}
namespace kcd2vr::frik_pose_probe{
 struct TestImpactD44{std::uintptr_t collider=0;std::uint64_t gesture59c3=0,tick=0,key=0,generation=0,epoch=0,request=0,sample=0;double speed=0;std::uint32_t entityId=0;int body=0,part=-1;pose_core::Vec3 point{},normal{};bool valid=false;};
 struct PhysFrame6698{struct{std::array<TestImpactD44,2>impactRecord178r30;}output;};
 static core::SnapshotMailbox<PhysFrame6698>g_phys_frame6698;
 static void LegacyTouchD44(){
#include "touch_legacy_actual.inc"
 }
#include "lab_living_actual.inc"
}
int main(int argc,char**argv){
 (void)RetainedH9MainD44();
 if(!lab::livingReactionsAvailableD51()){
  fixture();fp::touch43::g_on=true;fp::PhysFrame6698 f{};auto&r=f.output.impactRecord178r30[0];
  r.valid=true;r.tick=now;r.sample=++serial;r.key=123;r.generation=4;r.epoch=2;r.request=3;r.entityId=456;r.collider=0xB000;
  r.body=int(kcd2vr::npc_contact172::Body::Living);r.part=5;r.point={.1,.2,1.1};r.normal={0,0,1};r.speed=.5;
  fp::g_phys_frame6698.publish(f);fp::LegacyTouchD44();
  check(!impactCalls&&!capsuleCalls,"actual legacy caller cannot revive living actuation");
  kcd2vr::combat_diag_probe::reply={456,5,r.sample,now,17,2};kcd2vr::combat_diag_probe::haveReply=true;
  fp::lab58n::LivingTick(now);
  check(!impactCalls&&!capsuleCalls&&!kcd2vr::combat_diag_probe::haveReply,"direct old answered-hit consumer drains without added actuation");
  std::printf("D44 actual callers with D51 suspension %u/%u; native_calls=SIMULATED\n",checks-failed,checks);return failed?1:0;
 }
 std::vector<std::string> receipts;
 for(int raw:{-1,5,203,745}){
  fixture();fp::touch43::g_on=true;fp::PhysFrame6698 f{};auto&r=f.output.impactRecord178r30[0];
  r.valid=true;r.tick=now;r.sample=++serial;r.key=123;r.generation=4;r.epoch=2;r.request=3;r.entityId=456;r.collider=0xB000;
  r.body=int(kcd2vr::npc_contact172::Body::Living);r.part=raw;r.point={.1,.2,1.1};r.normal={0,0,1};r.speed=.5;
  fp::g_phys_frame6698.publish(f);fp::LegacyTouchD44();
  check(!impactCalls,"actual legacy PE_LIVING contact never uses raw capsule part as skeleton joint");
  receipts.insert(receipts.end(),lines.begin(),lines.end());
 }
 for(int raw:{5,203}){
  fixture();lab::impactLeaseOk=true;fp::PhysFrame6698 f{};auto&r=f.output.impactRecord178r30[0];
  r.valid=true;r.tick=now;r.sample=++serial;r.entityId=456;r.body=int(kcd2vr::npc_contact172::Body::Living);r.part=raw;r.point={.1,.2,1.1};r.normal={0,0,1};r.speed=.5;
  fp::lab58n::g_budget={};fp::lab58n::RememberLiving(f);
  kcd2vr::combat_diag_probe::reply={456,raw,r.sample,now,17,2};kcd2vr::combat_diag_probe::haveReply=true;
  fp::lab58n::LivingTick(now);
  check(!impactCalls,"actual answered-hit consumer refuses unsupported extra joint injection");
  check(!kcd2vr::combat_diag_probe::haveReply,"native hit answer is consumed once without changing native gameplay");
  receipts.insert(receipts.end(),lines.begin(),lines.end());
 }
 fixture();auto e=publish();fp::touch43::F9ContactH9 proof{};int joint=-1;
 check(fp::touch43::MapJointH9(e,proof,joint)&&joint==5,"current exact shape still proves joint5");
 const double p[3]{.1,.2,1.1},n[3]{0,0,1};
 check(!fp::touch43::OnContact(0,456,745,p,n,.5,now,&proof)&&!impactCalls,"correct owner proof cannot be reused for a different raw part");
 check(fp::touch43::OnContact(0,456,joint,p,n,.5,now,&proof)&&impactCalls==1,"same exact mapped proof still delivers its qualified joint");
 receipts.insert(receipts.end(),lines.begin(),lines.end());
 lab::f9Rung=0;fp::touch43::g_on=true;now+=2000;lines.clear();
 check(!fp::touch43::OnContact(0,456,745,p,n,.5,now)&&lines.empty(),"raw joint refusal produces no diagnostics while F9 capture is off");
 lab::f9Rung=3;now+=2000;
 check(!fp::touch43::OnContact(0,456,745,p,n,.5,now)&&!lines.empty(),"F9 captures exact unsupported joint refusal without native call");
 if(argc>1){std::ofstream f(argv[1]);for(const auto&s:receipts)f<<s<<'\n';}
 std::printf("D44 touch production routes %u/%u; native_calls=SIMULATED\n",checks-failed,checks);return failed?1:0;
}
