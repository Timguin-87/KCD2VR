// Actual XR34 clock, native adapter and completed-source functions.
// Retail event execution and weapon skinning are explicit fixture leaves.
#define KCD2VR_SCOPE_NO_MAIN
#include "crossbow_scope_d82_test.cpp"
#undef KCD2VR_SCOPE_NO_MAIN
static unsigned checksR92=0;
#define C92(c) do { ++checksR92; if(!(c)){std::cerr<<"FAIL_R92 line="<<__LINE__<<" " #c "\n";return false;} } while(0)
static bool beginPull92(Fixture& a,int kind){
    a.source.classId=kind;a.source.physicalD81=true;a.source.instantD72=false;
    a.source.intentKnownD78=true;a.source.chestKnown=true;a.source.quiverKnownD72=true;
    a.spatial.toolBound=kind!=10;a.spatial.toolPoseReady=kind!=10;
    a.spatial.crankQualified=kind==15;a.spatial.travel=kind==15?.08:.1315;
    a.spatial.axis={0,kind==15?1.:-1.,0};animateMechanismD87=true;
    start82(a);completeFirstPose82(a,admission82(a));
    input82(a,0);nativeFrame(a);
    if(kind!=10){
        a.source.handInChest[0]=true;input82(a,.9);nativeFrame(a);
        C92(a.spawns==1&&a.cocks==0);
        a.source.handInChest[0]=false;
    }
    input82(a,.9);nativeFrame(a);
    C92(g_crossbow_xr34.status().phase==x::Phase::Span);
    C92(g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0&&a.loads==0);
    return true;
}
static bool cycle92(int kind){
    Fixture a;RetailScopeD82 profile(a,kind);
    if(!beginPull92(a,kind))return false;
    const double direction=kind==15?1.:-1.;const double travel=a.spatial.travel;
    input82(a,.9,{0,direction*travel*.5,0});nativeFrame(a);
    C92(g_crossbow_xr34.status().progress>.49f&&g_crossbow_xr34.status().progress<.51f);
    C92(a.cocks==0&&a.loads==0);
    // A genuinely released and re-gripped half-span resumes at the current
    // native mechanism, without granting a whole extra stroke or moving back.
    input82(a,0,{0,direction*travel*.5,0});nativeFrame(a);
    C92(!g_crossbow_xr34.status().toolEngagedD72);
    input82(a,.9,{0,direction*travel*.5,0});nativeFrame(a);
    C92(g_crossbow_xr34.status().toolEngagedD72);
    input82(a,.9,{0,direction*travel*.75,0});nativeFrame(a);
    C92(g_crossbow_xr34.status().progress>.74f&&g_crossbow_xr34.status().progress<.76f);
    C92(a.cocks==0&&a.loads==0);
    input82(a,.9,{0,direction*travel*1.001,0});nativeFrame(a);
    C92(a.cocks==1&&a.loads==0&&g_crossbow_xr34.status().phase==x::Phase::Tail);
    for(int n=0;n<300&&!a.finals;++n){input82(a,.9,{0,direction*travel,0});nativeFrame(a);}
    C92(a.cocks==1&&a.finals==1&&a.loads==0);
    const auto cockSerial=g_crossbow_xr34.status().owner.serial;
    XR34BeforeStopNative(a.parent.data());
    C92(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);
    // A completed cock authorizes only its same-source load continuation.
    // Save/world/weapon identities cannot inherit it.
    for(int changed=0;changed<5;++changed){auto other=a.source;other.entryGripD72=0;
        if(changed==0)++other.body;if(changed==1)++other.tracking;if(changed==2)++other.request;
        if(changed==3)++other.key;if(changed==4)++other.ammoKey;
        C92(!g_crossbow_xr34.acceptsNewActionR95(other,a.now));}
    // Native schedules its separate load action. Reuse the same native
    // storage deliberately: serial/token, event list, clock and admission
    // must be new; none of the prior physical progress may authorize load.
    a.put(a.p(a.child)+0x54,18u);a.put(a.p(a.child)+0x28,1);
    a.put(a.p(a.child)+0x10,0.f);a.put(a.entry()+0x44,0.f);a.remaining=5;
    a.put(a.p(a.scope)+0x148,std::uintptr_t(0));a.put(a.p(a.controller)+0x50,a.p(a.queued)+8);
    a.put(a.p(a.parent)+0x9A,(unsigned char)0);
    a.source.tick=++a.now;++a.source.sample;a.source.grip[0]=0;a.source.completedLease=0;
    a.source.entryGripD72=0;a.source.entryGripTickD72=0;
    a.spatial.toolVisible=false;a.spatial.handBoltBound=false;a.spatial.boltPoseReady=false;
    a.spatial.boltId=77;a.spatial.boltKey=a.source.ammoKey;
    a.spatial.boltEntity=a.p(a.player)+0x80;a.spatial.boltAttachment=a.p(a.item)+0x100;
    setup(a,true,false); // actual original-first Start/metadata and fresh pause
    C92(g_crossbow_xr34.status().owner.serial>cockSerial);
    C92(g_crossbow_xr34.status().progress==0&&!g_crossbow_xr34.status().boltTakenD72);
    input82(a,0);nativeFrame(a);input82(a,.9);nativeFrame(a);
    C92(!a.spatial.handBoltBound&&a.loads==0);
    input82(a,0);nativeFrame(a);a.source.handInQuiverD72[0]=true;input82(a,.9);nativeFrame(a);
    a.source.handInQuiverD72[0]=false;input82(a,.9);nativeFrame(a);
    C92(a.spatial.handBoltBound&&g_crossbow_xr34.status().phase==x::Phase::PlaceBolt&&a.loads==0);
    // A release away from the rail cannot spend or auto-nock ammunition.
    a.spatial.handBoltPlaced=false;input82(a,0,{.3,0,0});nativeFrame(a);
    C92(g_crossbow_xr34.pending()&&a.loads==0);
    input82(a,.9);nativeFrame(a);a.spatial.handBoltPlaced=true;input82(a,0);nativeFrame(a);
    C92(a.loads==1&&a.cocks==1&&a.spatial.nockBound);
    for(int n=0;n<8;++n){input82(a,0);nativeFrame(a);}C92(a.loads==1&&a.cocks==1);
    XR34BeforeStopNative(a.parent.data());C92(!g_crossbow_xr34.pending());
    const auto firstLoadSerial=g_crossbow_xr34.status().owner.serial;
    // Native firing is an explicit fixture leaf. A second actual reload
    // occurrence reuses storage but must earn a NEW complete stroke.
    a.spatial.nativeCocked=a.spatial.nativeLoaded=a.spatial.nockBound=false;
    a.spatial.toolVisible=false;a.spatial.handBoltBound=false;a.spatial.handBoltPlaced=false;
    a.put(a.p(a.parent)+0x88,0u);a.put(a.p(a.parent)+0x99,(unsigned char)0);
    a.put(a.p(a.child)+0x54,19u);a.put(a.p(a.child)+0x28,1);
    a.put(a.p(a.child)+0x10,0.f);a.put(a.entry()+0x44,0.f);a.remaining=5;
    a.put(a.p(a.scope)+0x148,std::uintptr_t(0));a.put(a.p(a.controller)+0x50,a.p(a.queued)+8);
    a.source.tick=++a.now;++a.source.sample;a.source.grip[0]=0;a.source.completedLease=0;
    a.source.entryGripD72=kind==10?1:2;a.source.entryGripTickD72=a.now;
    setup(a,false,false);x::SetSpatialReader(&spatial82);
    C92(g_crossbow_xr34.status().owner.serial>firstLoadSerial&&g_crossbow_xr34.status().progress==0);
    input82(a,0);nativeFrame(a);C92(a.cocks==1&&a.loads==1);
    if(kind!=10){a.source.handInChest[0]=true;input82(a,.9);nativeFrame(a);a.source.handInChest[0]=false;}
    input82(a,.9);nativeFrame(a);input82(a,.9,{0,direction*travel*.5,0});nativeFrame(a);
    C92(a.cocks==1&&a.loads==1&&g_crossbow_xr34.status().progress<.51f);
    input82(a,.9,{0,direction*travel*1.001,0});nativeFrame(a);C92(a.cocks==2&&a.loads==1);
    XR34BeforeStopNative(a.parent.data());C92(!g_crossbow_xr34.pending());
    return true;
}
static bool cancel92(int kind){
    Fixture a;RetailScopeD82 profile(a,kind);if(!beginPull92(a,kind))return false;
    const double direction=kind==15?1.:-1.;
    input82(a,.9,{0,direction*a.spatial.travel*.5,0});nativeFrame(a);
    C92(a.cocks==0&&a.loads==0);
    // Explicit native cancellation clears clock and presentation. It neither
    // cocks nor loads, and repeated stop cannot perform a second restore.
    XR34BeforeStopNative(a.parent.data());const int writes=a.writes;
    C92(!g_crossbow_xr34.pending()&&!g_crossbow_xr34.status().constraintD78.active);
    XR34BeforeStopNative(a.parent.data());C92(a.writes==writes&&a.cocks==0&&a.loads==0);
    return true;
}
static bool transition92(){
    // The next CAF may scan inside the same native Update as metadata.
    // Its harmless prefix must not discard the lease; ALL native effects,
    // including flags rather than just ammo/cock, still bound that prefix.
    for(bool earlierFlag:{false,true}){
        Fixture a;a.source.classId=10;a.source.physicalD81=true;
        RetailScopeD82 profile(a,10);start82(a,false,true);completeFirstPose82(a,admission82(a));
        const auto serial=g_crossbow_xr34.status().owner.serial,deadline=g_crossbow_xr34.status().firstPoseDeadline;
        input82(a,0);nativeFrame(a);a.events(true);
        if(earlierFlag){a.put(a.p(a.eventStorage)+12,3u);a.event(2,.005f,0xAE150612u);}
        a.put(a.entry()+0x44,0.f);a.put(a.p(a.child)+0x10,0.f);
        XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
        C92(g_crossbow_xr34.status().phase==x::Phase::Setup&&g_crossbow_xr34.status().loading);
        C92(g_crossbow_xr34.status().owner.serial==serial&&g_crossbow_xr34.status().firstPoseDeadline==deadline);
        C92(a.get<float>(a.p(a.child)+0x5C)==0);
        const float first=earlierFlag?.005f:.01f;
        C92(g_crossbow_xr34.clip().firstNativeEffectR92==first);
        ScanInfoD82 info{a.p(a.caf)+0x48,base+0x406B7C0,0,5,earlierFlag?2:1,1};
        a.blocks.emplace_back(reinterpret_cast<std::uintptr_t>(&info),sizeof(info));
        {XR34ScanNative scan(a.character.data()+0x110,false,0,first*.5f,&info,0);C92(scan.current);}
        C92(g_crossbow_xr34.pending()&&a.loads==0&&a.cocks==0);
        // A scan of a different, retired CAF is inert and grants nothing.
        ++info.animation;
        {XR34ScanNative scan(a.character.data()+0x110,false,.9f,1.f,&info,0);C92(!scan.current);}
        C92(g_crossbow_xr34.pending());--info.animation;
        {XR34ScanNative scan(a.character.data()+0x110,false,first*.5f,first+1e-5f,&info,0);C92(!scan.current);}
        C92(!g_crossbow_xr34.pending()&&a.loads==0&&a.cocks==0);
        a.blocks.pop_back();
    }
    return true;
}
int main(){
    for(int kind:{10,14,15}){if(!cycle92(kind)||!cancel92(kind))return 1;}
    if(!transition92())return 1;
    std::cout<<"R92 native reload cycles: "<<checksR92<<" checks passed; events/weapon skinning are explicit native fixture leaves\n";
}
