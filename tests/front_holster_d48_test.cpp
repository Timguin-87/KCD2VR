// Actual production front attachment adapter; only native memory/placement
// leaves are substituted. Both render and acquisition geometry are production.
#include "vrik/belt_layout164.h"
#include "core/snapshot_mailbox.h"
#include "core/haptics.h"
#include <cstdio>
#include <atomic>
#include <array>
#include <algorithm>
namespace ph=kcd2vr::physical;namespace pm=kcd2vr::pose_core;
namespace belt_layout164=kcd2vr::belt_layout164;
static std::uint64_t clockD48=1000;
static std::uint64_t GetTickCount64(){return clockD48;}
namespace diagnostics::lab {static bool native=false;static bool NativeReference(){return native;}}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
namespace diagnostic_trace {static void Write(const char*){}}
static bool PhysHolsterCaptureD48(std::uint64_t){return false;}
static std::atomic<int>g_weapon_control_mode{6};static std::atomic<bool>g_gameplay_allowed{true};
struct Stamp {std::uint64_t epoch=9,tick=1000,actor=77;};
struct CommittedFrame6695 {Stamp stamp{};};
struct PhysEquipmentItem6698 {ph::Profile profile{};void* entity=(void*)10;bool held=false;int set=0;};
struct PhysEquipment6698 {std::array<PhysEquipmentItem6698,8>items{};int count=1,selected=0;bool handKnown[2]{true,true};std::uint64_t handKey[2]{},epoch=9,request=8;void*player=(void*)1;void*manager=(void*)2;};
struct PhysFrame6698 {ph::Input input{};std::uint64_t request=8;void*player=(void*)1;void*manager=(void*)2;};
static kcd2vr::core::SnapshotMailbox<PhysFrame6698>g_phys_frame6698;
struct Grip {std::uint64_t key=0,generation=0;ph::Pose hand[2]{},mount{};bool ready[2]{},mountReady=false;Stamp mountStamp126{};};
struct PhysGripBank6698 {std::array<Grip,8>grip{};};
static PhysGripBank6698 g_phys_grip_bank6698;
static kcd2vr::core::SnapshotMailbox<PhysGripBank6698>g_phys_grips6698;
struct ModelWorld6692 {struct Q{float x=0,y=0,z=0,w=1;}q;struct V{float x=0,y=0,z=0;}t;float scale=1;};
static unsigned attachmentFlags=0,placements=0,nativeRestores=0;static bool placeOk=true;
static float placedWorld[8]{};
static bool Read(const void*,unsigned& flags){flags=attachmentFlags;return true;}
static bool PhysEntityIdentity135(void*,unsigned id){return id==100;}
static bool PhysAttachmentWorld164(void*,std::uint64_t,ModelWorld6692& out){out={};return true;}
static ph::Pose PhysModel6698(const float* v,const float*){return {{v[0],v[1],v[2],v[3]},{v[4],v[5],v[6]}};}
static ph::Pose PhysCurrentWorld164(ph::Pose p,const ModelWorld6692&){return p;}
static void InvalidateReadCache101R2(){}
static bool PlaceBothConsumers6689(void*,void*,const float* wanted,bool& child){++placements;child=false;std::copy_n(wanted,8,placedWorld);return placeOk;}
static void Restore(void*,void*){++nativeRestores;}
static auto g_entity_attachment_update_original=&Restore;
#include "subsystems/experimental/belt_attachment_experiment.inl"
static unsigned checks=0,failed=0;
static void ck(bool value,const char* text){++checks;if(!value){++failed;std::printf("FAIL %s\n",text);}}
int main(){
    PhysEquipment6698 equipment{};auto& item=equipment.items[0];
    item.profile.key=55;item.profile.generation=7;item.profile.entity=100;item.profile.classId=3;item.profile.side=0;item.profile.holsterSetR12=0;
    PhysFrame6698 frame{};frame.input.allowed=true;frame.input.bodyEpoch=9;frame.input.tick=clockD48;
    frame.input.torsoShouldersKnownR38=true;frame.input.torsoShouldersR38[0]={.2,0,1.45};frame.input.torsoShouldersR38[1]={-.2,0,1.45};
    frame.input.shoulders[0]={.4,.3,1.6};frame.input.shoulders[1]={-.4,0,1.4}; // independent arm accommodation
    g_phys_frame6698.publish(frame);
    auto& grip=g_phys_grip_bank6698.grip[0];grip.key=55;grip.generation=7;grip.ready[0]=true;grip.hand[0].p={.035,.09,-.025};
    CommittedFrame6695 body{};float natural[8]{0,0,0,1,-.22,.15,.97,1};
    const auto place=[&](){return PhysBeltExperiment164(equipment,0,(void*)20,(void*)30,natural,body);};
    ck(place()&&placements==1,"ordinary full mode places the front weapon without a diagnostic rung");
    ph::Vec3 mount{};ck(ph::holster_mount126(frame.input,item.profile,mount),"same production acquisition anchor exists");
    auto displayed=PhysModel6698(placedWorld,nullptr);
    ck(pm::length(ph::point(displayed,grip.hand[0].p)-mount)<1e-6,"rendered measured hilt equals acquisition mount");
    ck(pm::length(grip.mount.p-displayed.p)<1e-6&&grip.mountReady,"published mount is the actual displayed pose");
    const auto original=displayed;
    frame.input.shoulders[0]={2,2,2};frame.input.shoulders[1]={-2,-2,0};g_phys_frame6698.publish(frame);
    ck(place()&&pm::length(PhysModel6698(placedWorld,nullptr).p-original.p)<1e-6,"reaching arm shoulder correction cannot drag front holster");
    frame.input.torsoShouldersR38[0]={0,.2,1.45};frame.input.torsoShouldersR38[1]={0,-.2,1.45};g_phys_frame6698.publish(frame);
    ck(place(),"torso turn keeps front presentation available without gaze");
    ph::holster_mount126(frame.input,item.profile,mount);displayed=PhysModel6698(placedWorld,nullptr);
    ck(pm::length(ph::point(displayed,grip.hand[0].p)-mount)<1e-6,"turned rendered hilt stays on turned body acquisition anchor");
    grip.ready[0]=false;ck(place(),"first unseen native grip still has explicit model-origin presentation");
    ck(pm::length(PhysModel6698(placedWorld,nullptr).p-mount)<1e-6,"unknown grip uses visible model origin, not invented measured offset");
    const auto calls=placements;item.profile.holsterSetR12=2;
    ck(!place()&&placements==calls,"back-mapped third loadout never moves into front hip");item.profile.holsterSetR12=0;
    attachmentFlags=0x20000;ck(!place()&&placements==calls,"game shadow/recursion hide is not adopted or revealed");attachmentFlags=0;
    item.held=true;ck(!place(),"native held item never receives stored pose");item.held=false;
    placeOk=false;ck(!place()&&nativeRestores==1,"optional front placement failure restores native presentation locally");placeOk=true;
    frame.input.tick=clockD48-81;g_phys_frame6698.publish(frame);ck(!place(),"stale body source cannot relocate front equipment");
    namespace hp=kcd2vr::haptics101;hp::enabled=true;hp::allowed=true;hp::zoneRadius=.24f;
    hp::Cue cue{};hp::observe(cue,0,55,.26,true,1,1000,.28);hp::observe(cue,0,55,.26,true,2,1031,.28);
    ck(cue.proximityLatched,"outer shoulder shell uses actual .28m acquisition radius for haptic cue");
    hp::Cue legacy{};hp::observe(legacy,0,55,.26,true,3,2000);hp::observe(legacy,0,55,.26,true,4,2031);
    ck(!legacy.proximityLatched,"unspecialized haptic callers keep configured radius");
    std::printf("D48 front attachment production + haptics %u/%u; native leaves SIMULATED\n",checks-failed,checks);
    return failed?1:0;
}
