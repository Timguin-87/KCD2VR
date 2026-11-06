// Actual input/gesture controllers; native binding, contact and action facts
// are explicit fixture inputs. These checks do not claim recipe execution.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
#include <cmath>

static unsigned d41Checks=0,d41Failures=0;
static void expectD41(bool ok,const char* text){++d41Checks;if(!ok){++d41Failures;std::fprintf(stderr,"FAIL %s\n",text);}}
static void rotationD41(float* q,float angle){q[0]=q[1]=0;q[2]=std::sin(angle*.5f);q[3]=std::cos(angle*.5f);}
static void pairD41(Fixture& q){
 q.in.proximityCraft1=true;q.admit(2,1);q.grip(0,1);q.tool(0,3);q.tool(1,2);
 for(int h=0;h<2;++h){auto& t=q.in.tool[h];t.parent=999;
  t.binding={1,2,888,std::uintptr_t(800+h),t.entity,h,t.type};t.binding.object=900+h;t.binding.lifetime=3;}
 q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Mortar,3,3,true,false};
 q.in.palm[0].point=q.in.palm[1].point={0,0,0};
 for(int h=0;h<2;++h){q.in.world[h].point[0]=q.in.world[h].point[1]=q.in.world[h].point[2]=0;}
 for(int slot:{2,3}){auto& o=q.in.nativeFrame.offers[slot];o={{slot,unsigned(100+slot)},{},q.in.now,true};o.boundsValid=true;
  auto& b=o.bounds;b.world[0]=b.world[5]=b.world[10]=1;
  if(slot==2){b.local[0]=b.local[1]=-.08f;b.local[2]=-.04f;b.local[3]=b.local[4]=.08f;b.local[5]=.04f;}
  else {b.local[0]=b.local[1]=-.02f;b.local[3]=b.local[4]=.02f;b.local[5]=.20f;b.world[11]=.03f;}}
 q.advance(16);q.step();
}
int main(){
 {Fixture q;pairD41(q);q.advance(16);rotationD41(q.f.hand[0].rotation,.12f);auto out=q.step();
  expectD41(out.gesture.totalSeconds>0&&out.gesture.status==g::Status::Grinding,"qualified pestle twist at fixed wrist position earns grind work");
  const auto amount=out.gesture.totalSeconds;out=q.step();expectD41(out.gesture.totalSeconds==amount,"duplicate sample cannot repeat angular grinding");
  q.advance(16);out=q.step();expectD41(out.gesture.totalSeconds==amount&&out.gesture.clockStop,"stationary rotated pestle adds no work");
  q.advance(16);for(float& x:q.f.hand[0].rotation)x=-x;out=q.step();
  expectD41(out.gesture.totalSeconds==amount,"equivalent quaternion sign change cannot grind");
 }
 {Fixture q;pairD41(q);q.advance(16);rotationD41(q.f.hand[0].rotation,.12f);rotationD41(q.f.hand[1].rotation,.12f);
  expectD41(q.step().gesture.totalSeconds==0,"common rotation of both tools cannot grind");}
 {Fixture q;pairD41(q);q.advance(16);rotationD41(q.f.hand[0].rotation,.73f);auto out=q.step();
  const auto before=out.gesture.totalSeconds;
  for(int i=0;i<500;++i){q.advance(10);out=q.step();}
  expectD41(out.gesture.totalSeconds==before,"five seconds of held quaternion roundoff cannot accumulate grinding");}
 for(int fault=0;fault<6;++fault){Fixture q;pairD41(q);q.advance(16);rotationD41(q.f.hand[0].rotation,.12f);
  switch(fault){case 0:q.in.nativeFrame.offers[3].boundsValid=false;break;
   case 1:q.in.nativeFrame.offers[3].bounds.world[3]=2.f;break;
   case 2:q.in.tool[0].entity++;break;case 3:q.grip(0,.1f);break;
   case 4:q.f.allowed=false;break;case 5:rotationD41(q.f.hand[0].rotation,2.f);break;}
  expectD41(q.step().gesture.totalSeconds==0,"unqualified contact, release, cutscene or rotation discontinuity earns no work");}
 {Fixture q;pairD41(q);q.advance(16);q.f.hand[0].position[0]=.01f;auto out=q.step();
  const auto amount=out.gesture.totalSeconds;expectD41(amount>0,"translation fixture established prior native work credit");
  for(int i=0;i<8;++i){q.advance(16);q.grip(0,.1f);out=q.step();
   expectD41(out.gesture.totalSeconds==amount&&out.gesture.clockStop,"observed open pestle hand pauses without losing its work ledger");}
  q.advance(16);q.grip(0,.45f);out=q.step();expectD41(out.gesture.releaseRequested&&out.gesture.totalSeconds==amount,"partial squeeze cannot reacquire a released pestle");
  q.advance(16);q.grip(0,.8f);q.f.hand[0].position[0]=.04f;out=q.step();
  expectD41(!out.gesture.releaseRequested&&out.gesture.totalSeconds==amount,"deliberate same-pair regrip resumes with new motion baseline");
  q.advance(16);q.grip(0,.45f);q.f.hand[0].position[0]=.05f;out=q.step();
  expectD41(out.gesture.totalSeconds>amount&&out.gesture.status==g::Status::Grinding,"new motion after regrip continues the same grind ledger without another native Use");
  expectD41(!out.use,"grind regrip never injects another acquisition or native Use");
 }
 {Fixture q;pairD41(q);q.advance(16);q.f.hand[0].position[0]=.01f;q.step();q.advance(16);q.grip(0,.1f);q.step();
  q.advance(16);q.grip(0,.8f);q.in.tool[0].binding.object++;auto out=q.step();
  expectD41(!out.admitted&&out.gesture.totalSeconds==0,"replacement pestle cannot reuse the previous hand admission or grind ledger");}
 {Fixture q;pairD41(q);q.advance(16);q.grip(0,.1f);q.step();q.advance(16);q.grip(0,.8f);q.in.nativeReturnAuthorized=true;
  const auto out=q.step();expectD41(out.gesture.releaseRequested&&out.gesture.status==g::Status::NativeReturn,"same-pair regrip cannot revoke a native-authorized return");}
 std::printf("mortar_motion_d41: %u checks, %u failures\n",d41Checks,d41Failures);return d41Failures?1:0;
}
