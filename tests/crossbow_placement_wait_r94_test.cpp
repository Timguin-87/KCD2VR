// Actual native adapter + reload state machine. The native entity/render
// readback and tracked input are explicit leaves, never invented game results.
#define KCD2VR_SCOPE_NO_MAIN
#include "crossbow_scope_d82_test.cpp"
#undef KCD2VR_SCOPE_NO_MAIN
static unsigned checks94=0;
#define C94(c) do{++checks94;if(!(c)){std::cerr<<"FAIL_R94 line="<<__LINE__<<" " #c " reason="<<x::reasonName(g_crossbow_xr34.status().reason)<<"\n";return false;}}while(0)
static bool poseReady94=true;
static std::uint64_t receipt94=0;
static bool spatial94(const x::Owner& o,const x::Source& accepted,x::Spatial& out) noexcept {
 if(!spatial82(o,accepted,out))return false;
 out.toolPoseReady=out.toolPoseReady&&poseReady94;out.boltPoseReady=out.boltPoseReady&&poseReady94;
 out.placementTickR94=(out.toolPoseReady||out.boltPoseReady)?receipt94:0;
 return true;
}
static void frame94(Fixture& a,double grip=.9,x::Vec position={.3,0,0},bool newReceipt=true){
 input82(a,grip,position);if(newReceipt&&poseReady94)receipt94=a.source.tick;nativeFrame(a);
}
static bool begin94(Fixture& a,int kind,bool bolt,bool firstReceipt=true){
 poseReady94=true;receipt94=0;animateMechanismD87=true;
 a.source.classId=kind;a.source.physicalD81=true;a.source.instantD72=false;a.source.intentKnownD78=true;
 a.source.chestKnown=a.source.quiverKnownD72=true;a.spatial.toolBound=!bolt;
 a.spatial.toolPoseReady=!bolt;a.spatial.crankQualified=kind==15;a.spatial.travel=kind==15?.08:.1315;
 a.spatial.axis={0,kind==15?1.:-1.,0};
 a.spatial.boltId=77;a.spatial.boltKey=a.source.ammoKey;a.spatial.boltEntity=a.p(a.player)+0x80;a.spatial.boltAttachment=a.p(a.item)+0x100;
 start82(a,bolt);completeFirstPose82(a,admission82(a));x::SetSpatialReader(&spatial94);
 frame94(a,0,{},firstReceipt);
 if(bolt)a.source.handInQuiverD72[0]=true;else a.source.handInChest[0]=true;
 frame94(a,.9,{},firstReceipt);
 a.source.handInQuiverD72[0]=a.source.handInChest[0]=false;
 // Same held item, away from the rail/string: it must remain paused.
 for(int n=0;n<3;++n)frame94(a,.9,{.3,0,0},firstReceipt);
 C94(g_crossbow_xr34.status().phase==(bolt?x::Phase::PlaceBolt:x::Phase::Span));
 C94(g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==0);
 C94(!g_crossbow_xr34.status().toolMountedD78&&a.cocks==0&&a.loads==0);
 return true;
}
static bool recover94(int kind,bool bolt){
 Fixture a;RetailScopeD82 profile(a,kind);if(!begin94(a,kind,bolt))return false;
 const auto nativeEvents=a.originalEvents,loads=a.loads,cocks=a.cocks;const auto oldReceipt=receipt94;
 const auto progress=g_crossbow_xr34.status().progress,clock=a.get<float>(a.entry()+0x44);
 const auto serial=g_crossbow_xr34.status().owner.serial;
 poseReady94=false;frame94(a);
 C94(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::WaitingPlacementPoseR94);
 C94(!g_crossbow_xr34.status().constraintD78.active&&g_crossbow_xr34.status().progress==progress);
 C94(a.originalEvents==nativeEvents&&a.get<float>(a.entry()+0x44)==clock&&receipt94==oldReceipt);
 poseReady94=true;frame94(a);
 C94(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().owner.serial==serial&&g_crossbow_xr34.status().reason!=x::Reason::WaitingPlacementPoseR94);
 C94(a.loads==loads&&a.cocks==cocks&&a.originalEvents==nativeEvents);
 if(bolt){
  a.spatial.handBoltPlaced=true;frame94(a,0,{});C94(a.loads==1&&a.spatial.nockBound);
  for(int n=0;n<5;++n)frame94(a,0,{});C94(a.loads==1);
 }
 XR34BeforeStopNative(a.parent.data());return true;
}
static bool expiry94(bool bolt){
 Fixture a;RetailScopeD82 profile(a,14);if(!begin94(a,14,bolt))return false;
 const auto original=receipt94;const auto events=a.originalEvents;poseReady94=false;
 for(int n=0;n<4;++n){frame94(a);C94(a.now-original==20u*(n+1)&&receipt94==original);
  C94(g_crossbow_xr34.pending()&&a.originalEvents==events&&g_crossbow_xr34.status().reason==x::Reason::WaitingPlacementPoseR94);}
 frame94(a);C94(a.now-original==100&&!g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::PhysicalBindingLost);
 return true;
}
static bool reject94(bool bolt,int defect){
 Fixture a;RetailScopeD82 profile(a,14);if(!begin94(a,14,bolt,defect!=2))return false;
 poseReady94=false;
 if(defect==0){if(bolt)++a.spatial.boltEntity;else ++a.spatial.toolEntity;}
 if(defect==1){if(bolt)++a.spatial.boltAttachment;else ++a.spatial.toolAttachment;}
 if(defect==3){if(bolt)a.spatial.handBoltBound=false;else a.spatial.toolBound=false;}
 if(defect==4){a.source.allowed=false;x::PublishSource(a.source);tick(a);C94(!g_crossbow_xr34.pending());return true;}
 frame94(a);C94(!g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::PhysicalBindingLost);
 return true;
}
static bool movingClock94(){
 Fixture a;RetailScopeD82 profile(a,14);if(!begin94(a,14,false))return false;
 frame94(a,0,{});frame94(a,.9,{});C94(g_crossbow_xr34.status().toolEngagedD72);
 frame94(a,.9,{0,-a.spatial.travel*.5,0});
 C94(a.get<float>(a.p(a.child)+0x5C)>0&&a.cocks==0);
 const auto serial=g_crossbow_xr34.status().owner.serial,receipt=receipt94;const auto events=a.originalEvents;
 const auto progress=g_crossbow_xr34.status().progress,clock=a.get<float>(a.entry()+0x44);
 poseReady94=false;frame94(a,.9,{0,-a.spatial.travel*.5,0});
 C94(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::WaitingPlacementPoseR94);
 C94(a.get<float>(a.p(a.child)+0x5C)==0&&a.get<float>(a.entry()+0x44)==clock);
 C94(g_crossbow_xr34.status().progress==progress&&!g_crossbow_xr34.status().constraintD78.active&&receipt94==receipt);
 C94(a.originalEvents==events&&a.cocks==0&&a.loads==0);
 poseReady94=true;frame94(a,.9,{0,-a.spatial.travel*.5,0});
 C94(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().owner.serial==serial&&g_crossbow_xr34.status().reason!=x::Reason::WaitingPlacementPoseR94);
 C94(a.originalEvents==events&&a.cocks==0&&a.loads==0);
 XR34BeforeStopNative(a.parent.data());return true;
}
static bool release94(){
 Fixture a;RetailScopeD82 profile(a,10);if(!begin94(a,10,true))return false;
 const auto events=a.originalEvents;poseReady94=false;a.spatial.handBoltPlaced=true;
 frame94(a,0,{});C94(g_crossbow_xr34.pending()&&a.loads==0&&!g_crossbow_xr34.status().constraintD78.active);
 poseReady94=true;frame94(a,0,{});C94(a.loads==0&&a.originalEvents==events);
 frame94(a,.9,{});frame94(a,0,{});C94(a.loads==1);
 XR34BeforeStopNative(a.parent.data());return true;
}
int main(){
 for(int kind:{10,14,15})if(!recover94(kind,true))return 1;
 for(int kind:{14,15})if(!recover94(kind,false))return 1;
 for(bool bolt:{false,true}){if(!expiry94(bolt))return 1;for(int bad=0;bad<5;++bad)if(!reject94(bolt,bad))return 1;}
 if(!release94()||!movingClock94())return 1;
 std::cout<<"R94 same-item placement wait: "<<checks94<<" production adapter checks passed; native render/effect leaves explicit\n";
}
