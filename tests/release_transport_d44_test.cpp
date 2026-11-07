// Execute the actual release-history producer. Copied frame/equipment POD and
// native collider read/age gate are host boundaries; no native game function is called.
#include "vrik/physical_weapon.h"
#include "vrik/weapon_release_t1.h"
#include "native_weapon_body_t4.h"
#include "core/snapshot_mailbox.h"
#include <cstdio>

namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
namespace nf=kcd2vr::native_weapon_body_t4;
static nf::Achieved hostAchieved{};
static bool hostAchievedReady=false;
namespace kcd2vr::native_weapon_body_t4 {bool Read(Achieved& a) noexcept {a=hostAchieved;return hostAchievedReady;}}
// The full native-held apply header is a separate Windows integration target.
// Its 80 ms receipt guard is supplied at this host boundary; these fixtures
// exercise the real release producer and its WORLD ownership checks.
namespace kcd2vr::native_held_pose_t4 {
inline bool fresh(std::uint64_t now,std::uint64_t tick) noexcept {return tick&&now>=tick&&now-tick<=80;}
}
static std::uint64_t GetTickCount64(){return 1100;}
struct PhysEquipment6698 {void* manager=nullptr;void* player=nullptr;std::uint64_t epoch=0,request=0;};
struct PhysEquipmentItem6698 {ph::Profile profile{};void* item=nullptr;void* entity=nullptr;};
struct PhysFrame6698 {
    ph::Input input{};ph::Output output{};
    struct Correlation {pm::Stamp stamp{};} correlation126[2];
};
#include "subsystems/throw_drop/allowed_release_history.inl"

static unsigned checks=0,failed=0;
static void ck(bool x,const char* why){++checks;if(!x){++failed;std::printf("FAIL %s\n",why);}}
namespace w=kcd2vr::weapon_release_t1;
struct Fixture {
    PhysEquipment6698 equipment{};PhysEquipmentItem6698 item{};PhysFrame6698 frame{};
    float placed[8]{0,0,0,1,0,0,0,1};void* attachment=reinterpret_cast<void*>(0x4000);
    Fixture(){
        w::SetMode(1);hostAchievedReady=false;
        g_weapon_release_history_t1[0]={};(void)g_weapon_release_sample_t1[0].publish({});
        equipment.manager=reinterpret_cast<void*>(0x1000);equipment.player=reinterpret_cast<void*>(0x1800);
        equipment.epoch=9;equipment.request=10;item.item=reinterpret_cast<void*>(0x2000);item.entity=reinterpret_cast<void*>(0x3000);
        item.profile.key=55;item.profile.generation=7;item.profile.side=0;item.profile.entity=72;
        frame.correlation126[0].stamp.actor=77;
        frame.input.sample=100;frame.input.tick=1000;frame.input.grip[0]=1;
        frame.input.tracked[0]=frame.input.intentTracked[0]=true;frame.output.owned[0]=true;
        frame.input.haveWorld=true;frame.input.world={{0,0,0,1},{30,40,50}};frame.input.worldScale=1;
        frame.output.weapons[0].p={.14,.416,1.289};
        place(frame.input.world);record();next();frame.output.weapons[0].p.x+=.02;place(frame.input.world);record();
    }
    void place(ph::Pose root){auto local=frame.output.weapons[0];local.p=local.p*frame.input.worldScale;
        const auto p=ph::compose(root,local);placed[0]=float(p.q.x);placed[1]=float(p.q.y);placed[2]=float(p.q.z);placed[3]=float(p.q.w);
        placed[4]=float(p.p.x);placed[5]=float(p.p.y);placed[6]=float(p.p.z);}
    void next(){++frame.input.sample;frame.input.tick+=10;}
    void record(){PhysRecordReleaseT1(equipment,item,frame,attachment,placed);}
    w::Sample sample(){w::Sample s{};(void)g_weapon_release_sample_t1[0].read(s);return s;}
};
int main(){
    {Fixture f;ck(f.sample().motion.pair,"two actual checked placements form release motion");
     float v[3]{},omega[3]{};f.sample().motion.velocity(1011,true,v,omega);
     ck(v[0]>1.99f&&v[0]<2.01f,"checked movement retains expected two metre per second release");
     f.next();auto liveRoot=f.frame.input.world;liveRoot.q=pm::from_rotation_vector({0,0,.941535309});f.place(liveRoot);f.record();
     ck(f.sample().motion.pair&&f.sample().motion.newer.sample==102,
        "mature actual placement retains moving-root history rather than starvation by a new mismatch veto");
     f.next();f.frame.input.world=liveRoot;f.place(liveRoot);f.record();
     ck(f.sample().motion.have&&f.sample().motion.pair,"next current root retains actual world placement history");
     f.next();f.frame.output.weapons[0].p.x+=.01;f.place(liveRoot);f.record();
     ck(f.sample().motion.pair,"new pair of actual checked world poses restores release normally");}
    for(int presentation=0;presentation<3;++presentation){
        Fixture f;f.next();
        if(presentation==0)f.frame.output.displayRejoinR13[0]=true;
        if(presentation==1)f.frame.output.displayLatched[0]=true;
        if(presentation==2)f.frame.output.handlingOnly55c1[0]=true;
        f.record();ck(!f.sample().motion.have,"display-only pending acquisition impale and handling frames revoke stale release velocity");
    }
    {Fixture f;f.next();f.frame.input.haveWorld=false;f.record();
     ck(f.sample().motion.pair,"mature producer retains qualified actual placement without a redundant root requirement");}
    {Fixture f;
     for(int n=0;n<20;++n){f.next();f.frame.input.world.p.x+=.01;auto live=f.frame.input.world;live.p.x+=.006;
        f.frame.output.weapons[0].p.x+=.02;f.place(live);f.record();}
     float v[3]{},angular[3]{};f.sample().motion.velocity(f.frame.input.tick+1,true,v,angular);
     ck(f.sample().motion.pair&&v[0]>2.99f&&v[0]<3.01f,
        "repeated6mm LIVE-root offset retains3m/s actual world throw while walking");
     f.next();f.frame.input.grip[0]=0;f.record();
     ck(f.sample().motion.pair&&f.sample().motion.newer.sample==121,"moving grip release retains last allowed pair for drop consumer");}
    {Fixture f;f.next();f.frame.input.grip[0]=0;f.placed[4]=100;f.record();
     ck(f.sample().motion.newer.sample==101,"actual grip release retains preceding allowed motion for native drop consumer");}
    {Fixture f;f.next();f.frame.output.handlingOnly55c1[0]=f.frame.output.solverOwnedT4[0]=true;
     f.frame.input.haveWorld=false;hostAchievedReady=true;hostAchieved={};hostAchieved.poseValid=true;hostAchieved.colliderRetired=false;
     hostAchieved.sourceTick=1100;hostAchieved.owner={55,7,10,9,0x1800,0x1000,77,0x3000,0x4000,6,72,0};
     hostAchieved.weaponWorld={{f.placed[0],f.placed[1],f.placed[2],f.placed[3]},{f.placed[4],f.placed[5],f.placed[6]}};
     f.record();ck(f.sample().motion.newer.sample==102,"T4 exact achieved native world pose keeps its separate qualified release path");
     f.next();hostAchieved.weaponWorld.p.x+=.1;f.record();
     ck(!f.sample().motion.have,"T4 displayed versus achieved mismatch also revokes stale release motion");}
    std::printf("RESULT %u/%u passed; actual release-history producer with native read substituted\n",checks-failed,checks);
    return failed?1:0;
}
