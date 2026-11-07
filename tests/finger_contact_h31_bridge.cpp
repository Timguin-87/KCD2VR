// Full production writer/H31 FK, joined to actual extracted ContactHand producer.
#define main retained_finger_translation_main_h31
#include "finger_translation_h31_bridge.cpp"
#undef main
extern "C" bool FingerWorldMeasureH31(void*,unsigned,const void*,unsigned,unsigned long long,unsigned long long,float,int,int,int*);
namespace fc31=kcd2vr::finger_contact_h31;
static bool measure31(HandContact6695& c,FingerContactGeometry103& g,std::uint64_t tick,std::uint64_t sample,float scale,int mode=4,int budget=-1,int* calls=nullptr){
 return FingerWorldMeasureH31(&c,sizeof(c),&g,sizeof(g),tick,sample,scale,mode,budget,calls);
}
static void world31(float scale){const float world[]{0,0,0,1,0,0,0,scale};std::memcpy(static_cast<unsigned char*>(g_pose6689.character)+0xA08,world,sizeof(world));}
static void dispatch31(CoherentState6689& s,CoherentTransaction6689& tx,HandContact6695 (&stagedContact)[2],int side,
 const TrackingSnapshot& ts,bool nativeFingerCapture,bool objectFingers,bool freeHand,std::uint64_t contactControl){
 void* character=s.character;bool freeContact164R1[2]{freeHand,freeHand};const bool preciseContactModeR24=true;
 #include "finger_contact_dispatch_h31.inc"
}
static void set31(void*,int,const QuatT28*){}
static bool publication31(CoherentState6689& s,CoherentTransaction6689& tx,HandContact6695 (&stagedContact)[2]){
 const auto count=s.count;void* pose=&s;bool collisionMode=true;const bool pairedFramePublishedR38=true;
 tx.motionD47.ready=true;tx.motionD47.contactMode=true;
 for(int side=0;side<2;++side){tx.motionD47.valid[side]=true;tx.motionD47.contact[side]=stagedContact[side];}
 #include "finger_contact_commit_h31.inc"
 return true;
}
int main(){
 static CoherentState6689 s{};static std::array<unsigned char,0xB00> character31{};
 init31(s);s.character=character31.data();g_pose6689.character=s.character;
 for(int side=0;side<2;++side){const int h=side?s.rig.hand_l:s.rig.hand_r;s.work[h]=s.local[h]=s.rest[h]={};s.work[h].q[3]=s.local[h].q[3]=s.rest[h].q[3]=1;
  s.restLocal[s.fingers[side][0].id[0]].t[0]=.015f; // fixture's naturally shorter thumb does not veto four-finger wall fitting
 }
 for(int side=0;side<2;++side)for(float scale:{.25f,1.f,3.f}){
  world31(scale);FingerWorldMeasureH31(nullptr,0,nullptr,0,0,0,scale,0,-1,nullptr);
  HandContact6695 c{};FingerContactGeometry103 geometry{};V3 palm[6]{};
  ck31(PalmGeometry103(s,side,{},0,0,0,palm,geometry),"actual H31 requested FK");
  int queryCount=0;geometry.springSourceH31={0x180001000ull,1,1,3,1,1000,true};
  ck31(measure31(c,geometry,1000,1,scale,4,-1,&queryCount),"actual checked producer completes");
  ck31(queryCount<=7&&c.fresh169r1&&c.planeCount>0,"existing feature ray budget only");
  // Copy only the fixture actor identity; all measured clocks/body/plane data stay original.
  c.character=s.character;c.springMeasuredH31.actor=reinterpret_cast<std::uintptr_t>(s.character);
  auto source=c.springMeasuredH31;ApplyFingerContactSpringH31(s,side,c,source,0,0,0,1000);
  const auto baseline=c;const auto hardCurl=c.curl[2];
  ck31(ApplyFingerLocals6689(s,side,0,0,&c),"actual hard-fit writer");const auto hardPose=s.local;
  // Subsequent pressure must use a real source interval and current checked planes.
  geometry.springSourceH31={0x180001000ull,1,1,3,2,1008,true};c.character=reinterpret_cast<void*>(0x180001000ull);
  ck31(measure31(c,geometry,1008,2,scale,4,-1,&queryCount),"next actual checked producer");
  c.character=s.character;c.springMeasuredH31.actor=reinterpret_cast<std::uintptr_t>(s.character);source=c.springMeasuredH31;
  ApplyFingerContactSpringH31(s,side,c,source,0,0,0,1008);
  bool changed=false;for(int f=0;f<5;++f)changed=changed||std::abs(c.curl[f]-c.hardCurlH31[f])>1e-6;
  ck31(changed,"ON produces functional extra clearance curl");
  ck31(c.springH31.have&&c.springH31.source.tick==1008&&c.springH31.digit[2].gapWorld<=.004,"original source clock and4mm WORLD cap");
  ck31(ApplyFingerLocals6689(s,side,0,0,&c),"actual compliant writer");
  for(int f=0;f<5;++f)if(c.fingerContact[f]){
   auto curve=geometry.curve[f];for(unsigned k=0;k<curve.count;++k)curve.offset[k]=curve.offset[k]*scale;
   for(int p=0;p<c.planeCount;++p){const auto& plane=c.planes[p];const auto fit=pc::finger_clearance103(curve,c.curl[f],{}, {},{PV(plane.point),PV(plane.normal)},.008*scale);
    ck31(fit.valid&&fit.fits,"applied curl safe against every current measured plane");}
   ck31(near31(PV(c.offset[f+1]),PV(FromT(joint31(s,side,f,3)))*scale,3e-7),"actual rendered H31 fingertip equals next ray start FK");
   for(int k=1;k<4;++k){const auto id=s.fingers[side][f].id[k];ck31(near31(PV(FromT(s.local[id])),PV(FromT(s.restLocal[id])),1e-12),"all phalanx lengths unchanged");}
  }
  const auto once=c.springH31;const auto curls=c.curl[2];
  const auto onContact=c;
  ApplyFingerContactSpringH31(s,side,c,source,0,0,0,1008);
  ck31(std::memcmp(&once,&c.springH31,sizeof(once))==0&&c.curl[2]==curls,"duplicate eye no temporal echo");
  c.character=reinterpret_cast<void*>(0x180001000ull);ck31(measure31(c,geometry,1008,2,scale,4,-1,&queryCount)&&queryCount==0,"actual repeated source spends zero native rays");c.character=s.character;
  ApplyFingerContactSpringH31(s,side,c,source,0,0,0,1008);ck31(c.springH31.source.tick==1008&&c.curl[2]==curls,"held same sample has original response without advancing");
  auto switchedOff=onContact;auto offSource=source;offSource.enabled=false;ApplyFingerContactSpringH31(s,side,switchedOff,offSource,0,0,0,1008);
  ck31(!switchedOff.springH31.have&&switchedOff.curl[2]==switchedOff.hardCurlH31[2],"ON to OFF immediately restores mandatory fit");
  for(int i=0;i<7;++i)ck31(near31(PV(switchedOff.offset[i]),onContact.springH31.hardOffsets[i],1e-12),"ON to OFF restores original hard probe offsets");
  auto negative=c;source.tick=1007;ApplyFingerContactSpringH31(s,side,negative,source,0,0,0,1008);ck31(!negative.springH31.have,"different original clock denies cached proof");
  for(int n=0;n<5;++n){negative=c;source=c.springMeasuredH31;if(n==0)source.actor++;if(n==1)source.epoch++;if(n==2)source.body++;if(n==3)source.request++;if(n==4)source.enabled=false;
   ApplyFingerContactSpringH31(s,side,negative,source,0,0,0,1008);ck31(!negative.springH31.have,"owner/context/disable resets optional state");}
  negative=c;source=c.springMeasuredH31;ApplyFingerContactSpringH31(s,side,negative,source,0,0,0,1089);ck31(!negative.springH31.have,"original hand80ms age enforced");
  negative=c;negative.planes[0].normal={};ApplyFingerContactSpringH31(s,side,negative,c.springMeasuredH31,0,0,0,1008);ck31(!negative.springH31.have,"invalid measured normal fails closed");
  negative=onContact;negative.planes[0].native172.collider=123;ApplyFingerContactSpringH31(s,side,negative,negative.springMeasuredH31,0,0,0,1008);
  ck31(negative.springH31.digit[2].gapWorld==0&&negative.curl[2]==negative.hardCurlH31[2],"current contacted body switch resets air gap");
  // Final-wrist transform and the actual hover recipe, not early desired wrist.
  negative=onContact;const int finalHand=side?s.rig.hand_l:s.rig.hand_r;const auto originalHand=s.work[finalHand];
  QToQuatT(s.work[finalHand],QP(pc::from_rotation_vector({.02,.01,.03})));ToT(s.work[finalHand],{-.002f,.001f,0});
  ApplyFingerContactSpringH31(s,side,negative,negative.springMeasuredH31,0,0,.4f,1008);
  ck31(ApplyFingerLocals6689(s,side,0,0,&negative,.4f),"actual final rotated wrist plus hover writer");
  FingerContactGeometry103 finalGeometry{};ck31(PalmGeometry103(s,side,QFromQuatT(s.work[finalHand]),0,0,0,palm,finalGeometry,.4f),"actual final hover contact recipe");
  for(int f=0;f<5;++f){auto parent=s.work[finalHand];for(int k=0;k<4;++k)parent=AbsoluteFromRelative(parent,s.local[s.fingers[side][f].id[k]]);
   ck31(near31(PV(FromT(parent))-PV(FromT(s.work[finalHand])),PV(negative.offset[f+1])*(1./scale),3e-7),"final wrist hover renderer equals actual contact tip");
   ck31(std::abs(negative.curl[f]-negative.hardCurlH31[f])<=.120001,"optional curl never jumps to distant FK branch");
   if(negative.fingerContact[f]){auto curve=finalGeometry.curve[f];for(unsigned k=0;k<curve.count;++k)curve.offset[k]=curve.offset[k]*scale;
    for(int p=0;p<negative.planeCount;++p){const auto& plane=negative.planes[p];const auto fit=pc::finger_clearance103(curve,negative.curl[f],PQ(QFromQuatT(s.work[finalHand])),PV(FromT(s.work[finalHand]))*scale,{PV(plane.point),PV(plane.normal)},.008*scale);
     ck31(fit.valid&&fit.fits,"final solved rotated wrist plus hover fits all measured planes");}}
  }
  s.work[finalHand]=originalHand;
  // Actual dispatch excludes native object/capture/non-free ownership, and the
  // actual commit failure exits before persistent contact state publication.
  for(int owner=0;owner<3;++owner){HandContact6695 staged[2]{onContact,onContact};TrackingSnapshot ts{};ts.tick=1008;ts.sequence=2;
   CoherentTransaction6689 tx;tx.epoch=1;tx.fingerSpringH31=true;kcd2vr::deepseek_h29::fingerContactSpringH31=true;
   dispatch31(s,tx,staged,side,ts,owner==0,owner==1,owner!=2,3);ck31(!staged[side].springH31.have,"actual writer dispatch respects native/held/non-free owner");}
  {HandContact6695 staged[2]{onContact,onContact};s.contact[side]=baseline;const auto originalPersistent=s.contact[side];
   CoherentTransaction6689 tx;tx.selected[0]=true;tx.setter=nullptr;
   ck31(!publication31(s,tx,staged),"actual native-write transaction failure rejects publication");
   ck31(std::memcmp(&originalPersistent,&s.contact[side],sizeof(originalPersistent))==0,"failed whole pose cannot advance persistent finger response");}
  {HandContact6695 staged[2]{onContact,onContact};CoherentTransaction6689 tx;tx.selected[0]=true;tx.setter=set31;
   ck31(publication31(s,tx,staged)&&s.contact[side].springH31.source.tick==1008,"actual successful publication commits original response clock");tx.restored=true;}
  kcd2vr::deepseek_h29::fingerContactSpringH31=false;
  // Partial new sample must discard its local queries and never advance spring.
  geometry.springSourceH31={0x180001000ull,1,1,3,3,1016,true};c.character=reinterpret_cast<void*>(0x180001000ull);
  ck31(measure31(c,geometry,1016,3,scale,4,3,&queryCount)&&queryCount<=3,"partial shared budget after earlier hit retains original hard owner only");
  c.character=s.character;source=geometry.springSourceH31;source.actor=reinterpret_cast<std::uintptr_t>(s.character);
  ApplyFingerContactSpringH31(s,side,c,source,0,0,0,1016);ck31(!c.springH31.have,"partial pass cannot integrate or renew original clock");
  geometry.springSourceH31={0x180001000ull,1,1,3,4,1024,true};c.character=reinterpret_cast<void*>(0x180001000ull);
  ck31(measure31(c,geometry,1024,4,scale,0,-1,&queryCount)&&c.planeCount==0&&c.fresh169r1,"actual fresh clear pass releases measured contact");
  c.character=s.character;c.springMeasuredH31.actor=reinterpret_cast<std::uintptr_t>(s.character);source=c.springMeasuredH31;
  ApplyFingerContactSpringH31(s,side,c,source,0,0,0,1024);ck31(!c.springH31.have&&!c.fingerContact[2],"clear uses accepted raw pose without retained spring/body lease");
  // Native object geometry is disabled for free-finger fitting, including H31 root shift.
  FingerContactGeometry103 held{};ck31(PalmGeometry103(s,side,{},0,0,123,palm,held),"native held geometry available");
  ck31(!held.enabled[2]&&pc::length(held.curve[2].rootClosedShiftRatioH31)==0,"native grip excluded from optional contact owner");
  // OFF does not change baseline hard-fit byte state/curls or local writer output.
  auto off=baseline;source=off.springMeasuredH31;source.enabled=false;ApplyFingerContactSpringH31(s,side,off,source,0,0,0,1000);
  ck31(off.curl[2]==hardCurl&&ApplyFingerLocals6689(s,side,0,0,&off),"OFF retains original mandatory fit");
  for(int f=0;f<5;++f)for(int k=0;k<4;++k){const int id=s.fingers[side][f].id[k];ck31(std::memcmp(&s.local[id],&hardPose[id],sizeof(QuatT28))==0,"OFF actual writer byte identical");}
 }
 std::printf("FINGER_CONTACT_H31 %u actual producer/writer checks PASS; native/headset NOT_EXECUTED\n",checks31);
}
