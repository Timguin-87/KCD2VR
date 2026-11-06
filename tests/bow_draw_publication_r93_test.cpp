// Actual draw-consumer slice; only native memory/mailboxes are fixture leaves.
#include "../src/vrik/physical_weapon.h"
#include "../src/vrik/bow_draw_publication_r93.h"
#include "../src/vrik/bow_grip178r36c1.h"
#include <atomic>
#include <cstdio>
namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
namespace bd=kcd2vr::bow_draw_publication_r93;
using V3=pm::Vec3;
struct QuatT28 {pm::Quat q{};V3 p{};};
V3 FromT(const QuatT28& a){return a.p;}
pm::Quat QFromQuatT(const QuatT28& a){return a.q;}
V3 PV(V3 p){return p;} V3 VP(V3 p){return p;} pm::Quat PQ(pm::Quat q){return q;}
V3 Sub(V3 a,V3 b){return a-b;} V3 Mul(V3 a,float f){return a*f;}
float Dot(V3 a,V3 b){return float(pm::dot(a,b));}float Len(V3 p){return float(pm::length(p));}
struct CommittedFrame6695 {bd::Source bowDrawR93{};pm::Stamp stamp{};QuatT28 allowed[2]{};float world[8]{0,0,0,1,0,0,0,1};bool valid[2]{true,true};};
struct BowCharacterIdentity166 {void* character=nullptr;void* entity=nullptr;std::uint64_t tick=0;};
struct PhysEquipment6698 {bool valid=true;std::uint64_t request=0,epoch=0,tick=0;int heldIndex[2]{-1,0},count=1;struct Item{bool held=true;void* entity=nullptr;ph::Profile profile{};}items[1];};
struct ModelWorld6692 {pm::Quat q{};V3 t{};double scale=1;};
struct PhysFrame6698 {ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};std::uint64_t request=0;float world[8]{0,0,0,1,0,0,0,1};};
template<class T>struct Mail {T value{};bool readable=true;bool read(T& o){o=value;return readable;}};
Mail<BowCharacterIdentity166> g_bow_identity166;Mail<PhysEquipment6698> g_phys_equipment6698;Mail<PhysFrame6698> g_phys_frame6698;
std::atomic<std::uint64_t> g_weapon_request_epoch6694{7},g_phys_ready_sample6698{0};
std::atomic<float> g_bow_finger_bias128{0};
CommittedFrame6695 body;bool bodyReadable=true,tracking[2]{true,true};ModelWorld6692 nativeBow;
bool ReadCommittedPacket6695(CommittedFrame6695& o){o=body;return bodyReadable;}
bool TrackingHandUsable111(int side){return tracking[side];}
bool ReadModelWorld6692(void*,ModelWorld6692& o){o=nativeBow;return true;}
ph::Pose PhysWorld6698(const float* p){return {{p[0],p[1],p[2],p[3]},{p[4],p[5],p[6]}};}
struct Rig {bool analysed=true,drawReceipt48=false;V3 stringRest{},axis{0,-1,0};float drawLen=.5;};
static double result=-1;
void consume(){void* ci=reinterpret_cast<void*>(44);const std::uint64_t now=1000;Rig r{};
#include "bow_draw_consumer_r93.inc"
 result=fraction;
}
unsigned checks=0,failures=0;
void check(bool ok,const char* what){++checks;if(!ok){++failures;std::printf("FAIL %s\n",what);}}
void reset(){
 body={};body.stamp={3,101,1000,44};body.allowed[0].p={0,-.4885,0};bodyReadable=true;tracking[0]=tracking[1]=true;nativeBow={};
 ph::Input i{};i.epoch=7;i.bodyEpoch=3;i.sample=101;i.tick=1000;i.allowed=i.tracked[0]=i.tracked[1]=true;i.grip[0]=.8;i.reach[0]=.6;
 ph::Profile p{};p.classId=9;p.side=1;p.key=11;p.generation=12;p.bowDrawLength166=.5;
 ph::Output o{};o.active[1]=true;o.bowHeld=true;o.bowFraction=.977;
 body.bowDrawR93=bd::prepare(i,o,p,body.stamp,7,true,true);
 g_weapon_request_epoch6694=7;g_bow_identity166.value={reinterpret_cast<void*>(44),reinterpret_cast<void*>(55),1000};g_bow_identity166.readable=true;
 g_phys_equipment6698.value={};auto& e=g_phys_equipment6698.value;e.request=7;e.epoch=3;e.tick=1000;e.items[0].entity=reinterpret_cast<void*>(55);e.items[0].profile=p;g_phys_equipment6698.readable=true;
 auto& f=g_phys_frame6698.value;f={};f.input=i;f.input.sample=100;f.input.bowDrawPolicy175=ph::BowDrawPolicy175::PhysicalString;f.output=o;f.output.bowFraction=.557;f.profiles[1]=p;f.request=7;g_phys_frame6698.readable=true;g_phys_ready_sample6698=100;result=-1;
}
bool accepted(){result=-1;consume();return result>=0;}
int main(){
 reset();check(accepted()&&std::abs(result-.977)<1e-6,"body101 with physical100 keeps completed draw101 instead of native .557");
 reset();g_phys_frame6698.value.input.sample=102;g_phys_ready_sample6698=102;check(accepted()&&std::abs(result-.977)<1e-6,"newer independent physical packet cannot reject completed draw");
 reset();g_phys_frame6698.readable=false;check(accepted(),"physical mailbox contention cannot break published visual transaction");
 reset();body.bowDrawR93.held=false;check(!accepted(),"release is not retained as held draw");
 reset();tracking[0]=false;check(!accepted(),"live right-hand loss rejects");
 reset();tracking[1]=false;check(!accepted(),"live bow-hand loss rejects");
 reset();bodyReadable=false;check(!accepted(),"no completed body rejects");
 reset();body.valid[0]=false;check(!accepted(),"invalid completed wrist rejects");
 reset();body.stamp.sample++;check(!accepted(),"mismatched receipt sample rejects");
 reset();body.stamp.actor++;check(!accepted(),"foreign actor rejects");
 reset();body.stamp.epoch++;check(!accepted(),"foreign body epoch rejects");
 reset();body.stamp.tick=body.bowDrawR93.stamp.tick=919;check(!accepted(),"81ms stale draw rejects");
 reset();body.stamp.tick=body.bowDrawR93.stamp.tick=1001;check(!accepted(),"future draw rejects");
 reset();g_weapon_request_epoch6694++;check(!accepted(),"weapon request change rejects");
 reset();g_phys_equipment6698.value.items[0].profile.generation++;check(!accepted(),"replacement bow generation rejects");
 reset();g_phys_equipment6698.value.items[0].entity=reinterpret_cast<void*>(66);check(!accepted(),"replacement bow entity rejects");
 reset();g_bow_identity166.value.character=reinterpret_cast<void*>(66);check(!accepted(),"wrong bow skeleton rejects");
 reset();nativeBow.t.x=.031;check(!accepted(),"native bow world disagreement remains rejected");
 reset();body.bowDrawR93.physical=false;check(accepted()&&std::abs(result-.977)<1e-5,"native/legacy draw still derives identical wrist-axis fraction");
 reset();body.bowDrawR93.physical=false;body.allowed[0].p.x=.26;check(!accepted(),"legacy lateral limit unchanged");
 for(int failure=0;failure<8;++failure){reset();auto i=g_phys_frame6698.value.input;i.sample=101;auto p=g_phys_equipment6698.value.items[0].profile;auto o=g_phys_frame6698.value.output;
  if(failure==0)i.allowed=false;if(failure==1)i.tracked[0]=false;if(failure==2)i.sample=99;if(failure==3)i.epoch=99;
  if(failure==4)p.classId=10;if(failure==5)p.generation=0;if(failure==6)o.active[1]=false;if(failure==7)o.bowFraction=NAN;
  check(!bd::prepare(i,o,p,body.stamp,7,true,true).valid,"invalid producer cannot stamp a current visual draw");
 }
 std::printf("%u checks, %u failures\n",checks,failures);return failures?1:0;
}
