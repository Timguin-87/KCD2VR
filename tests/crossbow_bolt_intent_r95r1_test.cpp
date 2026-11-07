// Native adapter/event loop is production. Memory and native inventory effects
// are fixture leaves; this does not claim a headset/native factory execution.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "crossbow_native_xr34_test.cpp"
static x::QuiverInputR95 rawBolt{};static bool rawValid=false;
static bool rawInput(const x::Owner& o,x::QuiverInputR95& out) noexcept {
 if(!rawValid||!XR34Current(o))return false;out=rawBolt;return true;
}
static int checks=0;
#define B(c) do{++checks;if(!(c)){std::cerr<<"FAIL_BOLT_R95R1 line="<<__LINE__<<" " #c <<" reason="<<x::reasonName(g_crossbow_xr34.status().reason)<<"\n";return 1;}}while(0)
static void begin(Fixture& a,int kind){
 rawValid=false;rawBolt={};x::quiverReaderR95=&rawInput;
 a.source.classId=kind;a.source.physicalD81=true;a.source.quiverKnownD72=true;
 a.source.entryGripD72=2;a.source.grip[0]=.9;a.spatial.toolBound=false;
 a.spatial.boltId=321;a.spatial.boltKey=a.source.ammoKey;a.spatial.boltEntity=456;a.spatial.boltAttachment=789;
 setup(a,true);a.input(.9);nativeFrame(a);
}
static int heldReach(int kind){Fixture a;begin(a,kind);
 B(g_crossbow_xr34.pending()&&a.originalEvents==0&&g_crossbow_xr34.status().reason==x::Reason::WaitingQuiverD72);
 // No neutral sample in this load child, exactly the cock -> load handover.
 a.source.handInQuiverD72[0]=true;a.input(.9);nativeFrame(a);
 B(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().boltTakenD72&&a.originalEvents==1);
 B(a.spatial.handBoltBound&&a.loads==0);
 a.source.handInQuiverD72[0]=false;a.input(.9);nativeFrame(a);B(g_crossbow_xr34.status().phase==x::Phase::PlaceBolt);
 for(int i=0;i<4;++i){a.input(.9);nativeFrame(a);}B(a.originalEvents==1&&a.loads==0);
 // A release away from rail grants no nock. Regrip SAME bolt and release at rail.
 a.input(0);nativeFrame(a);B(a.loads==0&&g_crossbow_xr34.status().phase==x::Phase::PlaceBolt);
 a.input(.9);nativeFrame(a);a.spatial.handBoltPlaced=true;a.input(0);nativeFrame(a);
 B(a.loads==1&&a.originalEvents==2&&g_crossbow_xr34.status().phase==x::Phase::Tail);
 for(int i=0;i<4;++i){a.input(0);nativeFrame(a);}B(a.loads==1&&a.originalEvents==2);
 XR34BeforeStopNative(a.parent.data());return 0;
}
static int reachDuringGap(int kind,int cancel){Fixture a;begin(a,kind);
 const auto poseTick=a.source.tick,poseSample=a.source.sample;
 a.source.auxiliaryUnavailableR95=true;x::PublishSource(a.source);
 rawValid=true;rawBolt={poseSample+1,++a.now,.9,true};nativeFrame(a);
 B(a.originalEvents==0&&g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::WaitingSourceR95);
 B(a.source.tick==poseTick&&a.source.sample==poseSample);
 rawBolt={poseSample+2,++a.now,cancel==1?0.:.9,false};nativeFrame(a);B(a.originalEvents==0);
 // The hand has already left the quiver when the socket publication recovers.
 a.source.auxiliaryUnavailableR95=false;a.source.sample=rawBolt.sample;
 if(cancel==2){a.now+=81;rawValid=false;}
 a.input(.9);nativeFrame(a);
 B(g_crossbow_xr34.pending());B((a.originalEvents==1)==(cancel==0));
 B(g_crossbow_xr34.status().boltTakenD72==(cancel==0));
 XR34BeforeStopNative(a.parent.data());rawValid=false;return 0;
}
static int newerRelease(int kind){Fixture a;begin(a,kind);
 a.source.auxiliaryUnavailableR95=true;x::PublishSource(a.source);
 rawValid=true;rawBolt={a.source.sample+2,++a.now,0.,true};nativeFrame(a);B(a.originalEvents==0);
 // Next completed body has an older held Grip inside quiver. It must not
 // replay acquisition after the newer raw release has already happened.
 a.source.auxiliaryUnavailableR95=false;a.source.handInQuiverD72[0]=true;
 ++a.source.sample;a.source.tick=a.now-1;x::PublishSource(a.source);nativeFrame(a);
 B(a.originalEvents==0&&!g_crossbow_xr34.status().boltTakenD72);
 XR34BeforeStopNative(a.parent.data());rawValid=false;return 0;
}
static int freshOwner(){Fixture a;begin(a,14);rawValid=true;rawBolt={a.source.sample+1,++a.now,.9,true};
 a.source.auxiliaryUnavailableR95=true;x::PublishSource(a.source);nativeFrame(a);
 B(a.originalEvents==0);XR34BeforeStopNative(a.parent.data());
 Fixture b;begin(b,14);b.input(.9);nativeFrame(b);B(b.originalEvents==0&&!g_crossbow_xr34.status().boltTakenD72);
 XR34BeforeStopNative(b.parent.data());return 0;
}
int main(){for(int kind:{10,14,15}){if(heldReach(kind)||newerRelease(kind))return 1;for(int cancel:{0,1,2})if(reachDuringGap(kind,cancel))return 1;}
 if(freshOwner())return 1;x::quiverReaderR95=nullptr;
 std::cout<<"PASS_BOLT_R95R1 checks="<<checks<<" classes=10,14,15 native Ammo/Load once; held reach, raw gap reach, release, expiry, owner reset\n";
}
