// Actual adapter/client with existing explicit native leaves. No game execution.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "crossbow_native_xr34_test.cpp"
int main(){
    {Fixture a;a.ownerThreadD72=false;setup(a);const auto owner=g_crossbow_xr34.status().owner;
        assert(!XR34Current(owner)); // outside this callback there is no authority
        a.input(0);nativeFrame(a);assert(g_crossbow_xr34.pending());
        a.input(0);XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
        assert(g_crossbow_xr34.pending());
        XR34BeforeStopNative(a.parent.data());assert(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);
        assert(!XR34Current(owner));}
    {Fixture a;a.source.instantD72=true;a.source.classId=10;
        a.source.entryGripD72=1;a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;
        setup(a);nativeFrame(a);assert(g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);
        // The exact originating Grip starts the string: no open/regrip after native admission.
        a.source.instantD72=false; // live setting/source cannot downgrade captured lifetime
        a.input(.9,{0,-.15,0});nativeFrame(a);
        assert(a.cocks==1&&a.loads==0&&g_crossbow_xr34.status().phase==x::Phase::Tail);
        assert(a.get<float>(a.p(a.child)+0x5C)>2.5f);
        a.input(.9,{0,-.15,0});nativeFrame(a);assert(a.cocks==1&&a.finals==1);
        XR34BeforeStopNative(a.parent.data());assert(!g_crossbow_xr34.pending());}
    for(int cls:{14,15}){Fixture a;a.source.instantD72=true;a.source.classId=cls;
        if(cls==15){a.spatial.axis={0,1,0};a.spatial.travel=.08;a.spatial.crankQualified=true;}
        a.source.entryGripD72=2;a.source.entryGripTickD72=a.now;
        a.source.grip[0]=.9;a.source.chestKnown=true;a.source.handInChest[0]=true;
        setup(a);nativeFrame(a);assert(a.spawns==0&&a.cocks==0&&!g_crossbow_xr34.status().toolTakenD72);
        // A real placed/read-back tool is required before the native reveal.
        a.spatial.toolPoseReady=false;a.input(.9);nativeFrame(a);assert(a.spawns==0);
        a.spatial.toolPoseReady=true;a.input(.9);nativeFrame(a);
        assert(a.spawns==1&&g_crossbow_xr34.status().toolTakenD72&&a.cocks==0);
        a.source.handInChest[0]=false;a.input(.9);nativeFrame(a);
        assert(g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);
        a.input(.9,{0,cls==15?.09:-.15,0});nativeFrame(a);assert(a.spawns==1&&a.cocks==1&&a.loads==0);
        XR34BeforeStopNative(a.parent.data());}
    {Fixture a;a.source.instantD72=true;a.source.quiverKnownD72=true;setup(a,true);
        a.input(0);nativeFrame(a);assert(!a.spatial.handBoltBound&&a.loads==0);
        a.input(.9);nativeFrame(a);assert(!a.spatial.handBoltBound&&a.loads==0); // squeeze elsewhere creates no bolt
        a.input(0);nativeFrame(a);a.source.handInQuiverD72[0]=true;a.input(.9);nativeFrame(a);
        assert(a.spatial.handBoltBound&&a.loads==0&&g_crossbow_xr34.status().boltTakenD72);
        a.source.handInQuiverD72[0]=false;a.input(.9);nativeFrame(a);
        assert(g_crossbow_xr34.status().phase==x::Phase::PlaceBolt&&a.loads==0);
        a.spatial.handBoltPlaced=true;a.input(0);nativeFrame(a);assert(a.loads==1&&a.spatial.nockBound);
        a.input(0);nativeFrame(a);assert(a.loads==1&&a.finals==1);XR34BeforeStopNative(a.parent.data());}
    {Fixture a;a.ownerThreadD72=false;setup(a);const auto n=a.writes;
        a.put(a.p(a.child)+0x54,18u);tick(a);assert(!g_crossbow_xr34.pending()&&a.writes==n);}
    std::cout<<"D72 actual callback migration + instant string/tool/quiver semantics PASS; original native outcomes mocked\n";
}
