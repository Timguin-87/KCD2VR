#include "vrik/physical_weapon.h"
#include <atomic>
#include <cstdio>
namespace ph=kcd2vr::physical;
struct PhysEquipment6698 {std::uint64_t request=8,epoch=9;};
struct PhysEquipmentItem6698 {ph::Profile profile{};void* entity=nullptr;bool held=true;};
static std::atomic<void*> g_hidden_bow_entity59c14{nullptr};
static unsigned shows=0;static bool showFails=false;
static void StowedBowShow59c14(void* entity,std::uint64_t){++shows;if(!showFails&&g_hidden_bow_entity59c14==entity)g_hidden_bow_entity59c14=nullptr;}
static bool PhysHolsterCaptureD48(std::uint64_t){return false;}
namespace diagnostic_trace{static void Write(const char*){}}
#include "subsystems/ranged_reload/draw_visibility_d45.inl"
static unsigned checks=0,failed=0;
static void ck(bool ok,const char* why){++checks;if(!ok){++failed;std::printf("FAIL %s\n",why);}}
int main(){
 PhysEquipment6698 equipment{};PhysEquipmentItem6698 item{};
 item.entity=reinterpret_cast<void*>(0x1000);item.profile.side=1;item.profile.key=55;item.profile.generation=7;item.profile.classId=9;
 const auto call=[&](std::uint64_t tick,bool placed=false,bool fallback=false){PhysRevealDrawnBowD45(equipment,item,tick,placed,fallback);};
 g_hidden_bow_entity59c14=item.entity;
 call(1000);ck(shows==0,"held state alone does not reveal the native socket before physical ownership");
 call(1011);ck(shows==0,"waiting first coherent packet keeps original owned hide briefly");
 call(1012,true);ck(shows==1&&g_hidden_bow_entity59c14==nullptr,"successful current physical placement reveals bow immediately");
 g_hidden_bow_entity59c14=item.entity;++item.profile.generation;
 call(2000);call(2079);ck(shows==1,"missing placement does not become presentation authority");
 call(2080);call(2500);ck(shows==1&&g_hidden_bow_entity59c14==item.entity,"80ms and longer elapsed time cannot expose animated native socket");
 call(2501,true);ck(shows==2&&!g_hidden_bow_entity59c14,"actual placement completes pending reveal without a timer");
 g_hidden_bow_entity59c14=item.entity;++item.profile.generation;
 call(3000);call(3001,false,true);ck(shows==3&&!g_hidden_bow_entity59c14,"real native owner/context takeover restores visibility immediately");
 g_hidden_bow_entity59c14=item.entity;++item.profile.generation;call(4000);++equipment.request;call(4079);call(4080);
 ck(shows==3,"new request cannot inherit preceding presentation readiness");call(4159);ck(shows==3,"new request remains hidden until a real presentation owner");call(4160,true);ck(shows==4,"new request reveals at its own actual placement");
 g_hidden_bow_entity59c14=item.entity;++item.profile.generation;call(5000);showFails=true;call(5010,true);
 ck(shows==5&&g_hidden_bow_entity59c14==item.entity,"failed native show preserves owned hide receipt for retry");
 showFails=false;call(5011,true);ck(shows==6&&!g_hidden_bow_entity59c14,"next actual physical placement retries show");
 g_hidden_bow_entity59c14=nullptr;call(6000,true);ck(shows==6,"never adopts or changes unowned native hidden state");
 g_hidden_bow_entity59c14=item.entity;item.held=false;call(7000,true);ck(shows==6,"stowed item remains hidden");
 item.held=true;item.profile.classId=4;call(7100,true);ck(shows==6,"other item families unaffected");
 std::printf("RESULT %u/%u passed; actual draw visibility service, native show substituted\n",checks-failed,checks);return failed?1:0;
}
