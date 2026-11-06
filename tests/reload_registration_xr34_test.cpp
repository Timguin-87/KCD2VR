// Composes actual registration publisher, native executor/clock/admission,
// and the full coherent working-arm transaction in a separate TU. Native
// memory/equipment/socket/binding/event leaves are explicit fixtures.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "crossbow_native_xr34_test.cpp"
namespace xcXR34=kcd2vr::crossbow_native_xr34;
static std::uintptr_t WhBase() noexcept{return base;}
static std::atomic<bool> g_reload_pose_ready_xr34{false},g_reload_ready36{false};
// Explicit installed native helper capability leaves, not helper execution.
#include <optional>
struct HelperHookCapabilityXR34 {bool enabled=true;bool is_enabled()const noexcept{return enabled;}};
static std::optional<HelperHookCapabilityXR34> g_attachment_helper_transform_hook;
static void* g_attachment_helper_transform_original=nullptr;
namespace action_time_owner59c11 {static bool ready=false;static bool CrossbowNativeReadyXR34(){return ready;}}
static int reloadModeXR34=0;static bool nativeBodyXR34=true,haveCompletedXR34=false;
static x::Source oldPhysicalXR34{},completedXR34{};static unsigned liveEquipmentReadsXR34=0;
static bool RangedSpatialXR34(const x::Owner& o,const x::Source& accepted,x::Spatial& out) noexcept {
    if(!x::fresh(accepted,f->now)||!XR34Current(o))return false;
    out=f->spatial;out.serial=o.serial;out.sample=accepted.sample;out.tick=accepted.tick;return true;
}
static void RangedReloadEquipmentXR34() noexcept {if(f&&f->current)++liveEquipmentReadsXR34;}
static bool RangedSourceXR34(x::Source& out) noexcept {
    out={};if(reloadModeXR34!=2||!x::Registered())return false;
    // Native source acquisition itself is exercised by the companion actual
    // 82-case fixture. These are explicit current-equipment/pose mailbox leaves.
    if(nativeBodyXR34){if(!haveCompletedXR34||!x::fresh(completedXR34,f->now))return false;out=completedXR34;return true;}
    if(!x::fresh(oldPhysicalXR34,f->now))return false;out=oldPhysicalXR34;return true;
}
#include "reload_registration_xr34_actual.inc"
using BootstrapCurrentXR34=bool(*)(const x::Passenger&) noexcept;
extern "C" bool ActualBootstrapPoseXR34(const x::Passenger&,std::uint64_t,std::uint64_t,double,double,BootstrapCurrentXR34,x::Source&);
static unsigned registeredChecksXR34=0;
static void requireXR34(bool ok,const char* why){++registeredChecksXR34;if(!ok){std::cerr<<"FAIL "<<why<<'\n';std::exit(1);}}
static bool BindingXR34(const x::Owner& o,x::Spatial& b) noexcept {if(!XR34Current(o))return false;
    b=f->spatial;b.serial=o.serial;b.qualified=true;return true;}
static bool CurrentPoseWitnessXR34(const x::Passenger& p) noexcept {
    x::Passenger current{};if(p.bootstrap){x::Admission admission{};if(!x::ReadAdmission(admission,f->now))return false;current=x::PassengerFromAdmission(admission);}
    else if(!x::ReadPassenger(current,f->now))return false;
    x::Clock clock{};return x::sameOwner(current.owner,p.owner)&&current.publication==p.publication&&current.root==p.root&&
        XR34Current(p.owner)&&XR34NativeClock(p.owner,p.clip,clock)&&clock.entry==p.clock.entry&&clock.normalized==p.clock.normalized&&
        current.binding.toolKey==p.binding.toolKey&&current.binding.toolEntity==p.binding.toolEntity&&
        current.binding.boltEntity==p.binding.boltEntity;
}
static void registerLeavesXR34(Fixture& a){
    g_crossbow_xr34=x::Client{};g_crossbow_base_xr34=0;x::revokedNativeLease=0;x::completedNativeLease=0;
    x::WithdrawRegistration();x::activeNativeLease=0;x::statusMailbox.publish({});x::passengerMailbox.publish({});x::admissionMailbox.publish({});
    g_henry_ctrl=a.controller.data();g_reload_pose_ready_xr34=g_reload_ready36=true;action_time_owner59c11::ready=true;
    g_attachment_helper_transform_hook=HelperHookCapabilityXR34{};g_attachment_helper_transform_original=a.player.data();
    x::registrationReader=&RangedRegistrationReadyXR34;x::identityReader=&FixtureIdentityXR34;x::bindingReader=&BindingXR34;
    reloadModeXR34=0;nativeBodyXR34=false;haveCompletedXR34=false;liveEquipmentReadsXR34=0;
    a.source.tick=a.now-60;oldPhysicalXR34=a.source;
    const float world[8]{0,0,0,1,100,200,10,1};std::memcpy(a.character.data()+0xA08,world,sizeof(world));
    a.spatial.toolBound=a.spatial.toolVisible=false;
    a.spatial.toolKey=91;a.spatial.toolId=77;a.spatial.toolItem=a.p(a.item)+0xF0;
    a.spatial.toolEntity=a.p(a.parent)+0xC0;a.spatial.toolAttachment=a.p(a.parent)+0xD0;
    a.spatial.boltKey=a.source.ammoKey;a.spatial.boltId=78;a.spatial.boltEntity=a.p(a.parent)+0xE0;a.spatial.boltAttachment=a.p(a.parent)+0xF0;
}
static void startRegisteredXR34(Fixture& a,bool load=false){registerLeavesXR34(a);a.events(load);
    RangedPublishXR34();XR34PostStartNative(a.parent.data(),true);
    requireXR34(!g_crossbow_xr34.pending()&&!x::sourceInterested.load(),"default-OFF owner leaves the original native reload untouched");
    reloadModeXR34=2;RangedPublishXR34();XR34PostStartNative(a.parent.data(),true);
    requireXR34(g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==0,"mode2 actual publisher admits original queued action and owns zero once");
    a.activate();a.now+=50;nativeBodyXR34=true;RangedPublishXR34();
    requireXR34(a.now-oldPhysicalXR34.tick>100&&!x::sourceInterested.load(),"old physical/equipment source has expired and cannot be restamped");
    XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);nativeFrame(a);
    requireXR34(g_crossbow_xr34.pending()&&a.originalEvents==0&&a.get<float>(a.p(a.child)+0x5C)==0,
        "current native metadata survives source expiry while bootstrap advances no native event");
}
static bool poseXR34(Fixture& a,double grip=0,double pull=0){x::Passenger p{};
    if(!x::ReadPassenger(p,a.now)){x::Admission admission{};if(!x::ReadAdmission(admission,a.now))return false;p=x::PassengerFromAdmission(admission);}
    x::Source output{};if(!ActualBootstrapPoseXR34(p,a.now,a.source.sample+1,grip,pull,&CurrentPoseWitnessXR34,output))return false;
    completedXR34=output;haveCompletedXR34=true;a.source=output;a.spatial.grip=output.hand[1-output.primary];RangedPublishXR34();return true;
}
static bool stepXR34(Fixture& a,double grip=0,double pull=0){a.now+=20;nativeFrame(a);
    if(a.spawns)a.spatial.toolBound=true;return poseXR34(a,grip,pull);}
int main(){
    for(int missing=0;missing<3;++missing){Fixture a;registerLeavesXR34(a);a.events(false);reloadModeXR34=2;
        if(missing==0)g_attachment_helper_transform_original=nullptr;
        if(missing==1)g_attachment_helper_transform_hook.reset();
        if(missing==2)g_attachment_helper_transform_hook->enabled=false;
        RangedPublishXR34();XR34PostStartNative(a.parent.data(),true);
        requireXR34(!x::Registered()&&!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f,
            "partial helper install cannot admit or zero the original native reload clock");
        g_attachment_helper_transform_hook=HelperHookCapabilityXR34{};g_attachment_helper_transform_original=a.player.data();
        RangedPublishXR34();XR34PostStartNative(a.parent.data(),true);
        requireXR34(x::Registered()&&g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==0,
            "a fully restored helper capability permits a new original native start");
    }
    for(bool load:{false,true}){Fixture a;startRegisteredXR34(a,load);x::Admission first{};
        requireXR34(x::ReadAdmission(first,a.now)&&first.started==1000&&first.deadline==1080,
            "original postUpdate admission has a fixed original-start deadline, no prior wrist required");
        const auto originalTick=g_crossbow_xr34.status().owner.source.tick;
        requireXR34(poseXR34(a)&&completedXR34.completedLease==g_crossbow_xr34.status().owner.serial&&completedXR34.tick==a.now&&
            g_crossbow_xr34.status().owner.source.tick==originalTick,"first actual completed working wrist establishes fresh source without renewing original owner pose");
        requireXR34(!x::ReadAdmission(first,a.now),"first-source publication irreversibly closes bootstrap before native tick consumes it");
        nativeFrame(a);
        requireXR34(g_crossbow_xr34.status().sourceEstablished,"native consumer rejoins the actual completed source lease");
        for(int n=0;n<150&&g_crossbow_xr34.status().phase==x::Phase::Setup;++n)requireXR34(stepXR34(a),"fresh working wrist continues across native setup");
        requireXR34(liveEquipmentReadsXR34>2&&g_crossbow_xr34.pending(),"native setup uses current equipment reads after old equipment expiry");
        requireXR34(stepXR34(a,0)&&stepXR34(a,.8),"neutral and genuine current grip enter the native work phase");
        if(load){a.spatial.handBoltPlaced=true;requireXR34(stepXR34(a,0),"current native bolt placement release grants only native load tail");}
        else{requireXR34(stepXR34(a,.8,.15)&&stepXR34(a,.8,.15),"actual controller-relative pull earns native span progress");}
        for(int n=0;n<300&&!(load?a.loads:a.cocks);++n){if(!stepXR34(a,load?0:.8,load?0:.15)&&!(load?a.loads:a.cocks))requireXR34(false,"current wrist remains owned until native transfer event");}
        requireXR34((load?a.loads:a.cocks)==1&&a.originalEvents>0,"cock/load happens once in the original native event leaf, never in the adapter");
    }
    {Fixture a;startRegisteredXR34(a);a.now=1081;nativeFrame(a);x::Admission p{};
        requireXR34(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f&&!x::ReadAdmission(p,a.now),
            "no first completed wrist before original deadline restores original native rate");}
    {Fixture a;startRegisteredXR34(a);requireXR34(poseXR34(a),"source-loss setup completes first actual wrist");
        haveCompletedXR34=false;RangedPublishXR34();nativeFrame(a);x::Admission p{};
        requireXR34(!g_crossbow_xr34.pending()&&!x::ReadAdmission(p,a.now),"later source loss cannot reopen first-pose bootstrap");}
    for(int seam=0;seam<6;++seam){Fixture a;startRegisteredXR34(a);
        if(seam==0)g_reload_pose_ready_xr34=false;if(seam==1)g_reload_ready36=false;if(seam==2)action_time_owner59c11::ready=false;
        if(seam==3)g_attachment_helper_transform_original=nullptr;if(seam==4)g_attachment_helper_transform_hook.reset();
        if(seam==5)g_attachment_helper_transform_hook->enabled=false;
        RangedPublishXR34();a.readFailure=true;XR34PreControllerNative(a.controller.data(),.02f);
        requireXR34(!x::Registered()&&g_crossbow_xr34.pending()&&g_crossbow_xr34.status().phase==x::Phase::Retiring,
            "partial hook/reset withdraws admission but retains unknown native restore obligation");
        a.readFailure=false;XR34PreControllerNative(a.controller.data(),.02f);
        requireXR34(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f,
            "retained original owner restores after native membership becomes readable");}
    {Fixture a;setup(a);a.spatial.toolVisible=true;a.input(0);tick(a);a.input(.8);tick(a);
        a.spatial.toolKey++;a.input(.8,{0,-.15,0});tick(a);
        requireXR34(!g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::PhysicalBindingLost&&a.cocks==0,
            "a replacement native tool cannot inherit a previously latched physical grip or span credit");}
    std::cout<<"PASS "<<registeredChecksXR34<<" actual registered XR34 bootstrap/pose/native-clock checks\n";
}
