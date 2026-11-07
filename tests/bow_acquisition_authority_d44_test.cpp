// Actual native bow command cases and pending-release predicate. Native calls,
// game clock and validated owner/frame POD are host boundaries.
#include "vrik/physical_weapon.h"
#include <atomic>
#include <cstdio>
namespace ph=kcd2vr::physical;
struct PhysNativeOwner6698 {
    std::uint64_t key=55,request=8,epoch=9,bowGeneration175=7,bowBeginSample175=100,bowBeginFrame=0;
    double nativeChargeCap=0;bool bowHeld=true,bowChargeObserved114=true,physicalBow175=false;
};
struct PhysFrame6698 {ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};std::uint64_t request=8;};
#include "subsystems/ranged_reload/bow_pending_release.inl"
static std::atomic<void*> g_phys_charge_action175{nullptr},g_phys_charge_player175{nullptr};
static std::atomic<bool> g_right_valid{true},g_left_valid{true};
static unsigned beginCalls=0,fireCalls=0,cancels=0;
static PhysNativeOwner6698* currentOwner=nullptr;
namespace diagnostic_trace {static void Write(const char*){}}
static bool PhysBowCall6698(void*,int op){if(op==0)++beginCalls;if(op==1)++fireCalls;return true;}
static void PhysCancelNative6698(bool){++cancels;if(currentOwner)currentOwner->bowHeld=false;}
struct Fixture {
    PhysNativeOwner6698 owner{};PhysFrame6698 frame{};ph::Command command{};
    struct Equipment {void* player=reinterpret_cast<void*>(0x1000);} equipment{};
    std::uint64_t now=1020,nativeFrame=20;
    Fixture(){
        beginCalls=fireCalls=cancels=0;
        frame.input.sample=102;frame.input.tick=1020;frame.input.epoch=8;frame.input.bodyEpoch=9;
        frame.input.allowed=frame.input.bowActions=frame.input.nativeBowTrigger126=true;
        frame.input.tracked[0]=frame.input.tracked[1]=true;
        auto& p=frame.profiles[1];p.key=55;p.generation=7;p.side=1;p.classId=9;
        command.kind=ph::CommandKind::BowFire;command.key=55;command.generation=7;command.side=1;
        command.epoch=8;command.bodyEpoch=9;command.sample=101;command.tick=1010;command.fraction=.8;
    }
    bool ready(){return PhysBowReleaseReady175(owner,command,frame,now,true,true);}
    bool dispatch(){auto& n=owner;auto& c=command;auto& e=equipment;bool called=false;currentOwner=&owner;
        switch(c.kind){
#include "subsystems/ranged_reload/native_bow_commands.inl"
        default:break;
        }
        currentOwner=nullptr;return called;
    }
};
static unsigned checks=0,failed=0;
static void ck(bool x,const char* why){++checks;if(!x){++failed;std::printf("FAIL %s\n",why);}}
int main(){
    {Fixture f;ck(f.ready(),"same current native charge and original queued release are ordinarily eligible");
     ck(f.dispatch()&&fireCalls==1&&!f.owner.bowHeld,"actual ordinary BowFire invokes native release exactly once");}
    for(int flag=0;flag<2;++flag){
        Fixture f;
        if(flag==0)f.frame.output.handlingOnly55c1[1]=true;else f.frame.output.displayRejoinR13[1]=true;
        ck(!f.ready(),"pending draw cannot borrow a previously queued same-item BowFire");
        ck(!f.dispatch()&&fireCalls==0&&cancels==1&&!f.owner.bowHeld,
           "actual BowFire case cancels pending native charge instead of firing through display acquisition");
        f.frame.output.handlingOnly55c1[1]=f.frame.output.displayRejoinR13[1]=false;
        ck(!f.ready()&&!f.dispatch()&&fireCalls==0&&cancels==1,
           "becoming checked cannot automatically fire the cancelled old charge");
    }
    {Fixture f;f.owner.bowHeld=false;f.command.kind=ph::CommandKind::BowBegin;
     ck(f.dispatch()&&beginCalls==1&&f.owner.bowHeld,"actual ordinary BowBegin retains native charge behavior");}
    for(int flag=0;flag<2;++flag){
        Fixture f;f.owner.bowHeld=false;f.command.kind=ph::CommandKind::BowBegin;
        if(flag==0)f.frame.output.handlingOnly55c1[1]=true;else f.frame.output.displayRejoinR13[1]=true;
        ck(!f.dispatch()&&beginCalls==0&&!f.owner.bowHeld,"actual BowBegin cannot start charge from a display-only grip");
        f.frame.output.handlingOnly55c1[1]=f.frame.output.displayRejoinR13[1]=false;
        f.command.sample=++f.frame.input.sample;f.command.tick=++f.frame.input.tick;++f.now;
        ck(f.dispatch()&&beginCalls==1&&f.owner.bowHeld&&fireCalls==0,
           "a fresh current Begin after checked acquisition is accepted without a consumer-side release-repress latch");
    }
    {Fixture f;f.frame.output.handlingOnly55c1[0]=true;
     ck(f.ready()&&f.dispatch()&&fireCalls==1,"unrelated right-hand handling flag cannot steal a qualified left bow charge");}
    {Fixture f;f.frame.output.blocked[1]=true;f.frame.output.contact119[1].plane=true;
     ck(f.ready()&&f.dispatch()&&fireCalls==1,"an actual checked surface alone is not mistaken for display-only acquisition");}
    {Fixture f;f.command.kind=ph::CommandKind::BowCancel;f.frame.output.handlingOnly55c1[1]=true;
     ck(f.dispatch()&&cancels==1&&fireCalls==0,"native cancellation remains available during pending acquisition");}
    {Fixture f;f.owner.bowHeld=false;
     for(unsigned shot=0;shot<3;++shot){
      f.command.kind=ph::CommandKind::BowBegin;f.command.sample=103+shot*2;f.frame.input.sample=f.command.sample;
      f.command.tick=f.frame.input.tick=++f.now;
      ck(f.dispatch()&&beginCalls==shot+1&&f.owner.bowHeld,"each fresh native reload accepts Begin");
      f.owner.bowChargeObserved114=true; // exact native charge observation leaf
      f.command.kind=ph::CommandKind::BowFire;++f.command.sample;++f.frame.input.sample;f.command.tick=++f.now;
      ck(f.ready()&&f.dispatch()&&fireCalls==shot+1&&!f.owner.bowHeld,"qualified subsequent release fires once after each reload");
      ck(!f.dispatch()&&fireCalls==shot+1,"duplicate drained release cannot refire");
     }}
    std::printf("RESULT %u/%u passed; actual bow Begin/Fire/Cancel consumers and release predicate, native calls substituted\n",checks-failed,checks);
    return failed?1:0;
}
