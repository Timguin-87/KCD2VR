// Current production adapter/client. Only engine memory/event leaves are mocked.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "crossbow_native_xr34_test.cpp"

static unsigned checksD81=0;
#define CHECK81(c) do{++checksD81;if(!(c)){std::cerr<<"FAIL_D81 line="<<__LINE__<<" " #c "\n";std::exit(1);}}while(0)
static void start81(Fixture& a){
    g_crossbow_xr34=x::Client{};g_crossbow_base_xr34=0;x::revokedNativeLease=0;
    g_henry_ctrl=a.controller.data();x::registrationReader=&FixtureRegistrationXR34;
    x::identityReader=&FixtureIdentityXR34;x::RefreshRegistration();x::SetSpatialReader(&spatial);x::PublishSource(a.source);
    XR34PostStartNative(a.parent.data(),true);a.activate();CHECK81(g_crossbow_xr34.pending());
}
int main(){
    // Native289D788 ignores callbacks from another scope, including a callback
    // arriving before the root CAF. No rejection or new owner may follow it.
    {Fixture a;start81(a);const auto serial=g_crossbow_xr34.status().owner.serial;
        XR34PostMetadataNative(a.parent.data(),a.child.data(),999,7);
        CHECK81(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().phase==x::Phase::AwaitClip);
        CHECK81(a.get<float>(a.p(a.child)+0x5C)==0&&g_crossbow_xr34.status().owner.serial==serial);
        XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
        CHECK81(g_crossbow_xr34.status().phase==x::Phase::Setup);
        XR34BeforeStopNative(a.parent.data());}
    // The same root metadata still rejects a corrupt character/animation set.
    {Fixture a;start81(a);a.put(a.p(a.set),base+1);
        XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
        CHECK81(!g_crossbow_xr34.pending());}
    // Native idle -> reload FIFO contains old idle first, selected reload second.
    // Both adapter and passenger use the same exact-entry selector.
    {Fixture a;setup(a);const auto first=a.entry(),second=first+0x78;
        std::memcpy(reinterpret_cast<void*>(second),reinterpret_cast<void*>(first),0x78);
        a.put(first-4,2u);a.put(first+0x10,1234u);
        x::Clock clock{};CHECK81(XR34NativeClock(g_crossbow_xr34.status().owner,g_crossbow_xr34.clip(),clock));
        CHECK81(clock.entry==second);
        std::uintptr_t selected=0;CHECK81(x::selectedClockEntryD81(&XR34ReadBytes,a.source.character,0,5,selected)&&selected==second);
        a.put(first+0x10,5u);CHECK81(!XR34NativeClock(g_crossbow_xr34.status().owner,g_crossbow_xr34.clip(),clock));
        a.put(first+0x10,1234u);a.put(first-4,17u);
        CHECK81(!x::selectedClockEntryD81(&XR34ReadBytes,a.source.character,0,5,selected));
        XR34BeforeStopNative(a.parent.data());}
    // Metadata can precede installation of its clock. The already-owned zero
    // waits within the original deadline; no timestamp or event is fabricated.
    {Fixture a;start81(a);XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
        const auto deadline=g_crossbow_xr34.status().firstPoseDeadline;
        a.put(a.entry()+0x10,1234u);a.now+=16;tick(a);
        CHECK81(g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==0);
        CHECK81(g_crossbow_xr34.status().reason==x::Reason::WaitingFirstPose&&a.cocks==0&&a.loads==0);
        a.put(a.entry()+0x10,5u);a.source.completedLease=g_crossbow_xr34.status().owner.serial;
        a.input(0);tick(a);CHECK81(g_crossbow_xr34.status().sourceEstablished&&g_crossbow_xr34.pending());
        CHECK81(g_crossbow_xr34.status().firstPoseDeadline==deadline);
        XR34BeforeStopNative(a.parent.data());}
    {Fixture a;start81(a);XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
        a.put(a.entry()+0x10,1234u);a.now+=81;tick(a);
        CHECK81(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);}
    {Fixture a;start81(a);XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
        a.put(a.p(a.child)+0x54,99u);tick(a);CHECK81(!g_crossbow_xr34.pending());}

    // Physical reload with the user's InstantReloads=false: hand displacement
    // controls the semantic milestone; stat-paced native tail remains optional.
    for(int kind:{10,14,15}){Fixture a;a.source.physicalD81=true;a.source.instantD72=false;a.source.classId=kind;
        a.source.entryGripD72=kind==10?1:2;a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;
        a.source.chestKnown=true;a.source.handInChest[0]=true;
        if(kind==15){a.spatial.axis={0,1,0};a.spatial.travel=.08;a.spatial.crankQualified=true;}
        setup(a);for(int i=0;i<4;++i){a.input(.9);nativeFrame(a);}
        CHECK81(g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);
        for(int i=0;i<8;++i){a.input(.9);nativeFrame(a);}CHECK81(a.cocks==0&&g_crossbow_xr34.status().progress==0);
        a.input(.9,{0,kind==15?.09:-.15,0});nativeFrame(a);
        CHECK81(a.cocks==1&&a.loads==0&&g_crossbow_xr34.status().phase==x::Phase::Tail);
        CHECK81(a.get<float>(a.entry()+0x44)>.8f&&a.get<float>(a.entry()+0x44)<.81f);
        a.input(.9);tick(a);CHECK81(a.get<float>(a.p(a.child)+0x5C)<=2.5f);
        XR34BeforeStopNative(a.parent.data());}
    // Right-shoulder retrieval remains required. Release away from the rail
    // retains exactly one real bolt and permits regrip, never native autoload.
    {Fixture a;a.source.classId=10;a.source.physicalD81=true;a.source.instantD72=false;
        a.source.quiverKnownD72=true;setup(a,true);
        a.input(0);nativeFrame(a);a.input(.9);nativeFrame(a);
        CHECK81(!a.spatial.handBoltBound&&a.loads==0);
        a.input(0);nativeFrame(a);a.source.handInQuiverD72[0]=true;a.input(.9);nativeFrame(a);
        a.source.handInQuiverD72[0]=false;a.input(.9);nativeFrame(a);
        CHECK81(g_crossbow_xr34.status().phase==x::Phase::PlaceBolt&&a.spatial.handBoltBound);
        const auto serial=g_crossbow_xr34.status().owner.serial;const auto events=a.originalEvents;
        a.spatial.handBoltPlaced=false;a.input(0);nativeFrame(a);
        for(int i=0;i<25;++i){a.input(0);nativeFrame(a);}
        CHECK81(g_crossbow_xr34.pending()&&a.loads==0&&a.originalEvents==events);
        CHECK81(a.get<float>(a.p(a.child)+0x5C)==0&&g_crossbow_xr34.status().owner.serial==serial);
        a.input(.9);nativeFrame(a);a.spatial.handBoltPlaced=true;a.input(0);nativeFrame(a);
        CHECK81(a.loads==1&&a.spatial.nockBound&&g_crossbow_xr34.status().owner.serial==serial);
        XR34BeforeStopNative(a.parent.data());}
    std::cout<<"D81 crossbow ownership: "<<checksD81<<" checks passed; native leaves mocked\n";
}
