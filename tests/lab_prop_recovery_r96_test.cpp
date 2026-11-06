#include "vrik/physical_weapon.h"
#include "vrik/prop_drive5916c1.h"
#include "vrik/lab_prop58.h"
#include "vrik/npc_contact172.h"
#include "vrik/static_grab_r89.h"
#include "core/native_grasp13.h"
#include "core/impact_identity59c6.h"
#include <string>
#include <cstdio>
#include <cstdlib>
namespace ph=kcd2vr::physical;
static std::uint64_t clockR96=1000;static std::uint64_t GetTickCount64(){return clockR96;}
namespace kcd2vr::lab58 {enum class Sub{PropWake};static bool on=true;static bool active(Sub){return on;}}
struct PhysFrame6698 {ph::Output output{};};
static struct {PhysFrame6698 value{};std::uint64_t advance=0;bool read(PhysFrame6698& out){out=value;if(advance)clockR96=advance;return true;}} g_phys_frame6698;
static unsigned entityR96=17;static int entityMemoryR96=0;
static std::atomic<void*> g_last_player_entity{nullptr};
static void* FlinchEntityById178r11(unsigned id){return id==entityR96?&entityMemoryR96:nullptr;}
static bool ClinchEntityPosition32(void*,unsigned& id,ph::Vec3& p){id=entityR96;p={};return true;}
static int ExactHandItemState(int){return 0;}
static bool HeldInteractionVisual43(){return false;}
namespace action_time_owner59c11 {static bool ReadGraspBinding(kcd2vr::grasp13::Binding&){return false;}}
static unsigned nativeCallsR96=0;static bool nativeOKR96=true;static std::string chunkR96;
static bool executeR96(const char* text){++nativeCallsR96;chunkR96=text;return nativeOKR96;}
namespace gore32n {static std::atomic<bool(*)(const char*)> g_lua_exec43{&executeR96};}
// Extracted declarations and complete Tick from lab_prop58.inl. Manifest and
// log sinks are leaves; production admission/Seen/budget/fault transitions run.
#ifndef KCD2VR_LAB_PROP_R96
#error KCD2VR_LAB_PROP_R96 must name the current literal extraction
#endif
#include KCD2VR_LAB_PROP_R96
static unsigned checksR96=0;
static void needR96(bool ok,const char* why){++checksR96;if(!ok){std::fprintf(stderr,"FAIL %u %s\n",checksR96,why);std::exit(1);}}
static void contactR96(std::uint64_t tick,int side=0){
 clockR96=tick;auto& f=g_phys_frame6698.value;f={};auto& r=f.output.impactRecord178r30[side];
 r.valid=true;r.tick=tick;r.sample=tick;r.key=1;r.generation=2;r.epoch=3;r.request=4;r.entityId=entityR96;r.collider=5;r.part=0;
 r.body=int(kcd2vr::npc_contact172::Body::Rigid);r.speed=1;r.normal={-1,0,0};
}
int main(){namespace P=kcd2vr::lab_prop58;namespace life=kcd2vr::inventory_lifecycle_t8;
 life::state=2;life::loadEpisode18=1;life::loadingObserved18=false;P::pakState=1;nativeOKR96=false;
 for(unsigned i=0;i<3;++i){contactR96(1000+i*20);lab_prop58n::Tick(clockR96);}
 needR96(nativeCallsR96==3&&P::faultBlockedR96(),"three actual uncertain route entries quarantine current load");
 nativeOKR96=true;contactR96(1080);lab_prop58n::Tick(clockR96);
 needR96(nativeCallsR96==3,"healthy-looking old route never retries while quarantined");
 life::Publish(false,false);life::RequestRecovery();life::Recheck();contactR96(1100);lab_prop58n::Tick(clockR96);
 needR96(nativeCallsR96==3&&P::faultBlockedR96(),"unknown/recheck is not load recovery");
 life::Publish(true,true);contactR96(1120);lab_prop58n::Tick(clockR96);needR96(nativeCallsR96==3,"loading cannot recover old calls");
 life::Publish(true,false);lab_prop58n::Tick(clockR96);needR96(nativeCallsR96==3,"completed load does not replay previously consumed contact");
 contactR96(1140,1);lab_prop58n::Tick(clockR96);
 needR96(nativeCallsR96==4&&!P::faultBlockedR96()&&P::faults==0&&chunkR96.find("KCD2VR_Lab.Wake")!=std::string::npos,"fresh left contact enters actual Lua route after load");
 // Consecutive failures only: an actual successful contact resets the count.
 nativeOKR96=false;contactR96(1700);lab_prop58n::Tick(clockR96);needR96(P::faults==1,"one later failure starts its own consecutive count");
 nativeOKR96=true;contactR96(1720);lab_prop58n::Tick(clockR96);needR96(P::faults==0&&nativeCallsR96==6,"successful actual route breaks failure streak");
 nativeOKR96=false;for(unsigned i=0;i<3;++i){contactR96(2300+i*20);lab_prop58n::Tick(clockR96);}
 needR96(P::faultBlockedR96()&&nativeCallsR96==9,"second failure episode quarantines only its own load");
 life::Publish(true,true);life::Publish(true,false);nativeOKR96=true;contactR96(2401);clockR96=2400;g_phys_frame6698.advance=2401;
 lab_prop58n::Tick(2400);needR96(nativeCallsR96==10&&!P::faultBlockedR96(),"post-read contact clock and second completed load resume real route");
 g_phys_frame6698.advance=0;contactR96(3001);clockR96=3000;lab_prop58n::Tick(3000);needR96(nativeCallsR96==10,"genuinely future impact is denied");
 contactR96(3400);g_phys_frame6698.value.output.impactRecord178r30[0].body=int(kcd2vr::npc_contact172::Body::Living);lab_prop58n::Tick(clockR96);
 needR96(nativeCallsR96==10,"living body never enters prop wake route");
 std::printf("R96 literal lab prop native Tick: %u checks PASS; ExecuteBuffer/manifest/native-object leaves substituted\n",checksR96);
}
