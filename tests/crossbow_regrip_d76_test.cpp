// Actual D76 reload client and adapter; native animation outcomes and transport
// are explicit fixture leaves. No prop pixels, Windows SEH or headset claim.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "crossbow_native_xr34_test.cpp"
static unsigned checksRegripD76=0,failedRegripD76=0;
#define CHECK_REGRIP_D76(c) do{++checksRegripD76;if(!(c)){++failedRegripD76;std::cerr<<"FAIL_REGRIP "<<checksRegripD76<<" line="<<__LINE__<<" " #c "\n";}}while(0)
static void acquiredToolD76(Fixture& a,int cls){
    a.source.instantD72=true;a.source.classId=cls;
    if(cls==15){a.spatial.axis={0,1,0};a.spatial.travel=.08;a.spatial.crankQualified=true;}
    a.source.entryGripD72=2;a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;
    a.source.chestKnown=true;a.source.handInChest[0]=true;setup(a);
    a.source.handInChest[0]=false;
    for(int n=0;n<3;++n){a.input(.9,{.3,0,0});nativeFrame(a);}
    CHECK_REGRIP_D76(g_crossbow_xr34.status().phase==x::Phase::Span);
    CHECK_REGRIP_D76(g_crossbow_xr34.status().toolTakenD72&&a.spawns==1&&a.cocks==0&&a.loads==0);
}
static void stopRegripD76(Fixture& a){
    XR34BeforeStopNative(a.parent.data());
    CHECK_REGRIP_D76(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);
}
static bool staleSpatialRegripD76(const x::Owner& owner,const x::Source& accepted,x::Spatial& result) noexcept {
    if(!spatial(owner,accepted,result))return false;
    result.tick=result.tick>81?result.tick-81:0;return true;
}
int main(){
    for(int cls:{14,15}){
        Fixture a;acquiredToolD76(a,cls);
        a.input(0,{.3,0,0});nativeFrame(a);
        CHECK_REGRIP_D76(!g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);
        // Re-grip the same actually bound tool away from the string. This
        // repairs grasp state only; it grants no mounting or cocking progress.
        a.input(.9,{.3,0,0});nativeFrame(a);
        CHECK_REGRIP_D76(!g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0&&a.spawns==1);
        a.input(.9);nativeFrame(a);
        CHECK_REGRIP_D76(g_crossbow_xr34.status().toolEngagedD72);
        a.input(.9,{0,cls==15?.09:-.15,0});nativeFrame(a);
        CHECK_REGRIP_D76(a.cocks==1&&a.spawns==1&&a.loads==0);
        if(!a.finals){a.input(.9);nativeFrame(a);}
        CHECK_REGRIP_D76(a.cocks==1&&a.finals==1);stopRegripD76(a);
    }
    for(int cls:{14,15}){
        Fixture a;acquiredToolD76(a,cls);
        a.input(0);nativeFrame(a);
        // A newer grip value without a new tracked sample is not an edge.
        a.source.grip[0]=.9;x::PublishSource(a.source);nativeFrame(a);
        CHECK_REGRIP_D76(!g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);
        a.input(.9);nativeFrame(a);
        CHECK_REGRIP_D76(g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);stopRegripD76(a);
    }
    for(int cls:{14,15})for(int changed=0;changed<7;++changed){
        Fixture a;acquiredToolD76(a,cls);a.input(0);nativeFrame(a);
        switch(changed){
            case 0:a.spatial.toolPoseReady=false;break;
            case 1:a.spatial.toolBound=false;break;
            case 2:++a.spatial.toolKey;break;
            case 3:++a.spatial.toolId;break;
            case 4:++a.spatial.toolItem;break;
            case 5:++a.spatial.toolEntity;break;
            case 6:++a.spatial.toolAttachment;break;
        }
        a.input(.9);tick(a);
        CHECK_REGRIP_D76(!g_crossbow_xr34.status().clockOwned&&g_crossbow_xr34.status().reason==x::Reason::PhysicalBindingLost);
        CHECK_REGRIP_D76(!g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0&&a.spawns==1&&a.loads==0);
        stopRegripD76(a);
    }
    for(int cls:{14,15}){
        Fixture a;acquiredToolD76(a,cls);a.input(0);nativeFrame(a);
        x::SetSpatialReader(&staleSpatialRegripD76);a.input(.9);tick(a);
        CHECK_REGRIP_D76(!g_crossbow_xr34.status().clockOwned&&g_crossbow_xr34.status().reason==x::Reason::GeometryUnavailable);
        CHECK_REGRIP_D76(!g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0&&a.spawns==1);stopRegripD76(a);
    }
    for(int cls:{14,15}){
        Fixture a;acquiredToolD76(a,cls);a.input(0);nativeFrame(a);
        // A new equipment request cannot inherit a prior prop/regrip debt.
        ++a.source.request;a.input(.9);tick(a);
        CHECK_REGRIP_D76(!g_crossbow_xr34.pending()&&a.cocks==0&&a.loads==0);stopRegripD76(a);
    }
    // Regrip is a Span/tool rule. A bolt released away from the rail retains
    // its existing native cleanup path and cannot become a tool acquisition.
    for(int cls:{10,14,15}){
        Fixture a;a.source.instantD72=true;a.source.classId=cls;
        a.source.entryGripD72=3;a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;
        a.source.quiverKnownD72=true;a.source.handInQuiverD72[0]=true;setup(a,true);
        a.source.handInQuiverD72[0]=false;
        for(int n=0;n<3;++n){a.input(.9,{.3,0,0});nativeFrame(a);}
        CHECK_REGRIP_D76(g_crossbow_xr34.status().phase==x::Phase::PlaceBolt&&a.loads==0&&a.spawns==0);
        a.spatial.handBoltPlaced=false;a.input(0,{.3,0,0});tick(a);
        CHECK_REGRIP_D76(!g_crossbow_xr34.status().clockOwned&&a.loads==0&&a.cocks==0&&a.spawns==0);
        a.input(.9);tick(a);
        CHECK_REGRIP_D76(!g_crossbow_xr34.pending()&&a.loads==0&&a.cocks==0);stopRegripD76(a);
    }
    std::cout<<"D76 exact tool regrip: "<<(checksRegripD76-failedRegripD76)<<"/"<<checksRegripD76<<" PASS; native outcome leaves mocked\n";
    return failedRegripD76?1:0;
}
