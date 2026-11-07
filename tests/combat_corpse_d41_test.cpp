// Executes literal C43T/N2 production producers extracted from frik_pose_probe.
// Memory/clock/native lookup leaves are explicit; no game-runtime claim.
#include "vrik/gore32.h"
#include "vrik/contact_receipt58c1.h"
#include "vrik/body_combo59c7g.h"
#include "vrik/impale40.h"
#include "vrik/impale_corpse_r15.h"
#include "vrik/npc_contact172.h"
#include "core/impact_identity59c6.h"
#include <cstdio>
#include <string>
#include <vector>
namespace bc=kcd2vr::bodycombo59c7g;
static std::uint64_t nowD41=1000;
static std::uint64_t GetTickCount64(){return nowD41;}
static std::atomic<std::uint64_t> g_pose_epoch6689{2},g_weapon_request_epoch6694{3};
static std::atomic<void*> g_last_player_entity{reinterpret_cast<void*>(0x110)};
struct V3D41 {double x=0,y=0,z=0;};
struct ImpactD41 {std::uint64_t tick=0,sample=0,key=0,generation=0,epoch=0,request=0,gesture59c3=0;
 std::uint32_t entityId=0;std::uintptr_t collider=0;int part=-1,bodyMaterial=-1,kind=0,body=0,feature59c7g=0;
 bool valid=false;double speed=0;V3D41 point{},normal{};};
struct ProfileD41 {std::uint64_t key=0,generation=0;int classId=-1;};
struct PhysFrame6698 {struct Output {ImpactD41 impactRecord178r30[2];bool active[2]{},owned[2]{};} output;ProfileD41 profiles[2];};
static PhysFrame6698 frameD41{};
struct FrameBoxD41 {bool read(PhysFrame6698& out){out=frameD41;return true;}}g_phys_frame6698;
static void* FlinchEntityById178r11(unsigned id){return id==42?reinterpret_cast<void*>(0x420):nullptr;}
static void* CharacterForEntity6689(void* entity){return entity;}
namespace gore32n {
 struct PartJoint47 {char joint[16]="head";int head=10,neck=9;};
 static bool CorpseCutAcked61(unsigned,std::uintptr_t){return false;}
 static PartJoint47 PartHeadNeck47(void*,int){return {};}
 static bool IsHeadNeckJoint47(const PartJoint47&){return true;}
 static kcd2vr::gorepak50::Cut LimbCutReceipt60(void*,int){return kcd2vr::gorepak50::Cut::None;}
}
namespace diagnostics::capture168 {
 enum class Domain {Combat};struct Gate {std::atomic<std::uint64_t> session[1];};static Gate gate{};
 static bool active(Domain,std::uint64_t){return false;}
}
namespace diagnostic_trace {static std::vector<std::string> lines;static void Write(const char* text){lines.emplace_back(text);}}
namespace frik_pose_probe {static void PublishVisibleFailure(const char*){}}
static void corpseCutD41(){
#include "corpse_cut_d41_production.inc"
}
static void corpseStabD41(){
#include "corpse_stab_d41_production.inc"
}
static unsigned checks=0,failed=0;
static void ck(bool value,const char* label){++checks;if(!value)++failed;std::printf("%s %s\n",value?"PASS":"FAIL",label);}
static void seed(int kind){
 ++nowD41;frameD41={};auto& r=frameD41.output.impactRecord178r30[0];
 r.valid=true;r.entityId=42;r.collider=0x440;r.part=10;r.bodyMaterial=38;r.kind=kind;
 r.feature59c7g=int(kind?bc::Feature::Tip:bc::Feature::Blade);r.body=int(kcd2vr::npc_contact172::Body::Articulated);
 r.speed=6;r.tick=nowD41;r.sample=nowD41;r.key=4;r.generation=5;r.epoch=2;r.request=3;r.gesture59c3=nowD41;
 r.point={1,2,1.5};r.normal={0,0,1};frameD41.profiles[0]={4,5,4};frameD41.output.active[0]=frameD41.output.owned[0]=true;
 kcd2vr::gore32::NoteDead(42,0x420,nowD41);kcd2vr::gore32::decap=true;kcd2vr::gore32::impaleHang=true;
 kcd2vr::gore32::lethalCut.publish({});kcd2vr::impale_corpse_r15::latest.publish({});
}
static bool cut(){corpseCutD41();kcd2vr::gore32::CutCandidate c{};return kcd2vr::gore32::lethalCut.read(c)&&c.target==42;}
static bool stab(){corpseStabD41();kcd2vr::impale_corpse_r15::Candidate c{};return kcd2vr::impale_corpse_r15::latest.read(c)&&
 kcd2vr::impale_corpse_r15::admissible(c,nowD41,4,5,2,3,0)&&kcd2vr::impale_corpse_r15::requestOwned(c,c.serial,42,0);}
int main(){
 seed(0);ck(cut(),"literal C43T producer retains current sword-edge corpse cut");
 seed(1);ck(!cut(),"corpse stab cannot request head removal");
 for(auto feature:{bc::Feature::Unknown,bc::Feature::Pommel,bc::Feature::Fist,bc::Feature::Shield}){seed(0);
  frameD41.output.impactRecord178r30[0].feature59c7g=int(feature);ck(!cut(),"literal corpse sever producer rejects non-edge feature");}
 seed(0);frameD41.profiles[0].classId=5;ck(!cut(),"literal corpse sever producer rejects mace");
 for(unsigned change=0;change<6;++change){seed(0);
  if(change==0)++frameD41.profiles[0].key;if(change==1)++frameD41.profiles[0].generation;
  if(change==2)++frameD41.output.impactRecord178r30[0].epoch;if(change==3)++frameD41.output.impactRecord178r30[0].request;
  if(change==4)frameD41.output.active[0]=false;if(change==5)frameD41.output.owned[0]=false;
  ck(!cut(),"retired or mismatched physical weapon cannot cut corpse from old impact");}
 seed(1);ck(stab(),"literal N2 producer and request copy retain valid sword-tip corpse stab");
 for(auto feature:{bc::Feature::Unknown,bc::Feature::Blade,bc::Feature::Pommel,bc::Feature::Fist,bc::Feature::WeaponHead}){seed(1);
  frameD41.output.impactRecord178r30[0].feature59c7g=int(feature);ck(!stab(),"literal N2 producer refuses non-piercing contact");}
 seed(1);frameD41.profiles[0].classId=5;ck(!stab(),"forward mace motion cannot create corpse impale");
 seed(1);++frameD41.profiles[0].generation;ck(!stab(),"corpse stab retains original generation check");
 std::printf("combat_corpse_d41 %u/%u\n",checks-failed,checks);return failed?1:0;
}
