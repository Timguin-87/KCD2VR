// Runs the actual native adapter + client. Native clip/event/PRS leaves remain
// explicit mocks from the established fixture; this is not a headset claim.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "crossbow_native_xr34_test.cpp"
static unsigned checksD76=0,failuresD76=0;
#define CHECK_D76(c) do{++checksD76;if(!(c)){++failuresD76;std::cerr<<"FAIL "<<checksD76<<" line="<<__LINE__<<" " #c "\n";}}while(0)
static void cleanupD76(Fixture& a){XR34BeforeStopNative(a.parent.data());CHECK_D76(!g_crossbow_xr34.pending());}
int main(){
    // The actual admitted string Grip precedes metadata/first-pose setup.
    // Pulling beyond its12cm acquisition region during that interval must not
    // throw away the beginning of the same physical stroke.
    {Fixture a;a.source.instantD72=true;a.source.classId=10;a.source.grip[0]=.9;
        a.source.entryGripD72=1;a.source.entryGripTickD72=a.now;setup(a);
        a.input(.9,{0,-.15,0});nativeFrame(a);
        CHECK_D76(g_crossbow_xr34.status().toolEngagedD72);
        CHECK_D76(a.cocks==1&&a.loads==0&&a.spawns==1);
        a.input(.9,{0,-.15,0});nativeFrame(a);CHECK_D76(a.cocks==1&&a.finals==1);cleanupD76(a);}
    // Both tool families acquire at the chest; held intent survives leaving
    // the pouch while the actual hidden tool is still being made ready.
    for(int cls:{14,15}){Fixture a;a.source.instantD72=true;a.source.classId=cls;
        if(cls==15){a.spatial.axis={0,1,0};a.spatial.travel=.08;a.spatial.crankQualified=true;}
        a.source.entryGripD72=2;a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;
        a.source.chestKnown=true;a.source.handInChest[0]=true;setup(a);
        a.source.handInChest[0]=false;a.spatial.toolPoseReady=false;
        a.input(.9,{.3,0,0});nativeFrame(a);CHECK_D76(a.spawns==0&&!g_crossbow_xr34.status().toolTakenD72);
        a.input(.9,{.3,0,0});nativeFrame(a);CHECK_D76(a.spawns==0);
        a.spatial.toolPoseReady=true;a.input(.9,{.3,0,0});nativeFrame(a);
        CHECK_D76(a.spawns==1&&g_crossbow_xr34.status().toolTakenD72&&a.cocks==0);
        a.input(.9,{.3,0,0});nativeFrame(a);CHECK_D76(!g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);
        a.input(.9);nativeFrame(a);CHECK_D76(g_crossbow_xr34.status().toolEngagedD72);
        a.input(.9,{0,cls==15?.09:-.15,0});nativeFrame(a);CHECK_D76(a.cocks==1&&a.loads==0&&a.spawns==1);
        if(!a.finals){a.input(.9);nativeFrame(a);}CHECK_D76(a.cocks==1&&a.finals==1);cleanupD76(a);}
    for(int cls:{10,14,15}){Fixture a;a.source.instantD72=true;a.source.classId=cls;
        a.source.entryGripD72=3;a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;
        a.source.quiverKnownD72=true;a.source.handInQuiverD72[0]=true;setup(a,true);
        a.source.handInQuiverD72[0]=false;a.input(.9,{.3,0,0});nativeFrame(a);
        CHECK_D76(a.spatial.handBoltBound&&g_crossbow_xr34.status().boltTakenD72&&a.loads==0);
        a.input(.9);nativeFrame(a);CHECK_D76(g_crossbow_xr34.status().phase==x::Phase::PlaceBolt&&a.loads==0);
        a.spatial.handBoltPlaced=true;a.input(0);nativeFrame(a);CHECK_D76(a.loads==1&&a.spatial.nockBound);
        a.input(0);nativeFrame(a);CHECK_D76(a.loads==1&&a.finals==1);cleanupD76(a);}
    // Releasing before acquisition retires its one-use intent. Re-squeezing
    // elsewhere must not reuse an old pouch/string/quiver receipt.
    for(int kind:{1,2,3}){Fixture a;a.source.instantD72=true;a.source.classId=kind==1?10:14;
        a.source.entryGripD72=kind;a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;
        a.source.chestKnown=a.source.quiverKnownD72=true;
        a.source.handInChest[0]=a.source.handInQuiverD72[0]=true;setup(a,kind==3);
        a.source.handInChest[0]=a.source.handInQuiverD72[0]=false;
        a.input(0,{.3,0,0});nativeFrame(a);a.input(.9,{.3,0,0});nativeFrame(a);
        CHECK_D76(!g_crossbow_xr34.status().toolTakenD72&&!g_crossbow_xr34.status().boltTakenD72&&!g_crossbow_xr34.status().toolEngagedD72);
        CHECK_D76(a.cocks==0&&a.loads==0);cleanupD76(a);}
    // Wrong origin regions and stale/future admission receipts do not count.
    for(int invalid=0;invalid<4;++invalid){Fixture a;a.source.instantD72=true;a.source.classId=14;
        a.source.entryGripD72=2;a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;
        a.source.chestKnown=true;a.source.handInChest[0]=true;
        if(invalid==0)a.source.chestKnown=false;if(invalid==1)a.source.handInChest[0]=false;
        if(invalid==2)a.source.entryGripTickD72-=81;if(invalid==3)++a.source.entryGripTickD72;
        setup(a);a.source.chestKnown=true;a.source.handInChest[0]=false;
        a.input(.9,{.3,0,0});nativeFrame(a);a.input(.9,{.3,0,0});nativeFrame(a);
        CHECK_D76(!g_crossbow_xr34.status().toolTakenD72&&a.spawns==0&&a.cocks==0);cleanupD76(a);}
    {Fixture a;a.source.instantD72=true;a.source.classId=10;a.source.grip[0]=.9;
        a.source.entryGripD72=1;a.source.entryGripTickD72=a.now;a.source.hand[0]={.3,0,0};setup(a);
        a.input(.9,{0,-.15,0});nativeFrame(a);CHECK_D76(!g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);cleanupD76(a);}
    // Immutable identity still wins over captured intent.
    {Fixture a;a.source.instantD72=true;a.source.classId=10;a.source.grip[0]=.9;
        a.source.entryGripD72=1;a.source.entryGripTickD72=a.now;setup(a);++a.source.request;
        a.input(.9,{0,-.15,0});tick(a);CHECK_D76(!g_crossbow_xr34.pending()&&a.cocks==0&&a.loads==0);}
    std::cout<<"D76 captured reload Grip intent: "<<(checksD76-failuresD76)<<"/"<<checksD76<<" PASS; native outcome leaves mocked\n";
    return failuresD76?1:0;
}
