// Executes the existing native adapter/client with fixture-owned memory.
// Native render, animation/event execution and headset behaviour are not simulated.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#ifndef KCD2VR_XR34_FIXTURE_D81
#define KCD2VR_XR34_FIXTURE_D81 "crossbow_native_xr34_test.cpp"
#endif
#include KCD2VR_XR34_FIXTURE_D81
static unsigned checksD81=0,failuresD81=0;
static void checkD81(bool ok,const char* message){
    ++checksD81;if(!ok){++failuresD81;std::cerr<<"FAIL "<<message<<'\n';}
}
static bool bindingD81(const x::Owner& owner,x::Spatial& result) noexcept {
    result=f->spatial;result.serial=owner.serial;return true;
}
static void startD81(Fixture& a){
    g_crossbow_xr34=x::Client{};g_crossbow_base_xr34=0;
    x::revokedNativeLease=0;x::passengerPublication=1;
    x::passengerMailbox.publish({});x::admissionMailbox.publish({});
    g_henry_ctrl.store(a.controller.data());
    x::registrationReader=&FixtureRegistrationXR34;x::identityReader=&FixtureIdentityXR34;
    x::bindingReader=&bindingD81;x::RefreshRegistration();x::SetSpatialReader(&spatial);
    a.source.classId=10;x::PublishSource(a.source);
    XR34PostStartNative(a.parent.data(),true);
    checkD81(g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==0,"exact queued child admitted at zero");
}
static void selectD81(Fixture& a){XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);}
static void noNativeEventsD81(const Fixture& a){
    checkD81(a.spawns==0&&a.cocks==0&&a.loads==0&&a.finals==0&&a.originalEvents==0,
        "admission does not create prop, loaded/cocked state, events or finalizer credit");
}
int main(){
    // Match native +289D788's early return for another scope. A legitimate
    // callback must then work without a second LT/reload admission.
    {Fixture a;startD81(a);a.activate();const auto serial=g_crossbow_xr34.status().owner.serial;
        const auto deadline=g_crossbow_xr34.status().firstPoseDeadline;
        const auto publication=x::passengerPublication.load();const auto writes=a.writes;
        XR34PostMetadataNative(a.parent.data(),a.child.data(),-1,7);
        checkD81(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().phase==x::Phase::AwaitClip,
            "foreign-scope metadata does not cancel current child");
        checkD81(a.writes==writes&&x::passengerPublication.load()==publication,
            "native-ignored scope does not write rate or invalidate passenger");
        selectD81(a);checkD81(g_crossbow_xr34.status().phase==x::Phase::Setup&&g_crossbow_xr34.status().clockOwned,
            "own-scope callback still selects exact clip");
        checkD81(g_crossbow_xr34.status().owner.serial==serial&&g_crossbow_xr34.status().firstPoseDeadline==deadline,
            "ignored scope does not rebase lease or bootstrap deadline");noNativeEventsD81(a);}
    // The native callback can select metadata before queue installation ends.
    {Fixture a;startD81(a);a.put(a.p(a.child)+0x30,a.p(a.scope));selectD81(a);
        checkD81(g_crossbow_xr34.clip().qualified,"queued child can receive real metadata");
        const auto deadline=g_crossbow_xr34.status().firstPoseDeadline;a.now+=16;tick(a);
        checkD81(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::WaitingFirstPose,
            "qualified queued child keeps bounded zero instead of false InputLost");
        x::Admission admission{};checkD81(!x::ReadAdmission(admission,a.now),"queued child cannot publish first-pose admission");
        a.now+=16;a.activate();tick(a);
        checkD81(x::ReadAdmission(admission,a.now)&&admission.valid&&admission.childStatus==2&&admission.clock.qualified,
            "installed qualified root now publishes actual native first-pose admission");
        checkD81(admission.started==1000&&admission.deadline==deadline&&admission.owner.source.tick==1000,
            "first admission retains original request time and fixed deadline");
        // Completed tracking is an explicit input leaf here; the retained
        // native-passenger/source fixture separately executes its producer.
        a.source.completedLease=g_crossbow_xr34.status().owner.serial;a.input(.8);tick(a);
        checkD81(g_crossbow_xr34.status().sourceEstablished,"first completed tracking closes bootstrap");
        checkD81(!x::ReadAdmission(admission,a.now),"first-pose admission cannot reopen after completion");
        x::Passenger passenger{};checkD81(x::ReadPassenger(passenger,a.now)&&passenger.frame.tick==a.source.tick,
            "original post-Update publishes current passenger after completed wrist");
        a.source.allowed=false;x::PublishSource(a.source);tick(a);
        checkD81(!g_crossbow_xr34.pending(),"lost source after first wrist immediately releases and cannot bootstrap again");
        noNativeEventsD81(a);}
    // Root is installed before its CAF queue is ready. Only zero may survive;
    // the exact clock/binding checks still withhold pose admission until ready.
    {Fixture a;startD81(a);a.activate();selectD81(a);a.put(a.p(a.scope)+0x60,.01f);
        const auto deadline=g_crossbow_xr34.status().firstPoseDeadline;a.now+=16;tick(a);
        checkD81(g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==0,
            "unready first root clock waits at original zero rate");
        x::Admission admission{};checkD81(!x::ReadAdmission(admission,a.now),"unready root never admits a pose");
        a.put(a.p(a.scope)+0x60,0.f);a.now+=16;tick(a);
        checkD81(x::ReadAdmission(admission,a.now),"clock becoming ready inside original window admits first pose");
        a.now=deadline+1;tick(a);checkD81(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f,
            "no completed pose by fixed 80 ms restores native rate");
        checkD81(g_crossbow_xr34.status().firstPoseDeadline==deadline,"waiting never extends bootstrap deadline");noNativeEventsD81(a);}
    // Negative controls: the foreign-scope early return requires proven scope
    // and current owner, not a blanket ignore of failed metadata.
    {Fixture a;startD81(a);a.activate();a.put(a.p(a.scope),base+0x1234);
        XR34PostMetadataNative(a.parent.data(),a.child.data(),5,7);
        checkD81(!g_crossbow_xr34.pending(),"unknown scope type still fails closed");}
    {Fixture a;startD81(a);a.activate();a.event(0,.2f,0x8ABB129Au);a.event(1,.3f,0x8ABB129Au);
        selectD81(a);checkD81(!g_crossbow_xr34.pending(),"ambiguous own-scope semantic markers still fail closed");}
    {Fixture a;startD81(a);a.activate();a.source.body++;x::PublishSource(a.source);
        XR34PostMetadataNative(a.parent.data(),a.child.data(),5,7);
        checkD81(!g_crossbow_xr34.pending(),"foreign-scope callback cannot preserve replaced source owner");}
    {Fixture a;startD81(a);a.put(a.p(a.child)+0x30,a.p(a.scope));selectD81(a);
        a.put(a.p(a.child)+0x5C,.75f);tick(a);
        checkD81(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==.75f,
            "bootstrap never overwrites a foreign rate");}
    {Fixture a;startD81(a);a.put(a.p(a.child)+0x30,a.p(a.scope));selectD81(a);
        a.put(a.p(a.child)+0x54,99u);tick(a);
        checkD81(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==0,
            "recycled child token is not retained or restored through stale identity");}
    {Fixture a;startD81(a);a.put(a.p(a.child)+0x30,a.p(a.scope));selectD81(a);
        a.now+=81;tick(a);checkD81(!g_crossbow_xr34.pending(),"queued metadata never extends fixed first-pose deadline");}
    std::cout<<"D81 actual reload admission "<<(checksD81-failuresD81)<<'/'<<checksD81<<" checks; explicit native leaves, headset NOT_RUN\n";
    return failuresD81?1:0;
}
