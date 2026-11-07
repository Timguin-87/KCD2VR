#include "../src/subsystems/smithing/stroke_receipt_d57.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
namespace d=kcd2vr::subsystems::smithing::stroke_receipt_d57;
static unsigned checks=0;
static void ck(bool b,const char* why){++checks;if(!b){std::fprintf(stderr,"FAIL %u: %s\n",checks,why);std::exit(1);}}
static d::Input input(){d::Input i{};i.capture=i.eligible=i.sampleValid=i.nativeHeight=true;i.instance=10;
 i.session=1;i.lifetime=7;i.pose=4;i.source=8;i.modeEpoch=9;i.sample=1;i.sampleTick=i.now=1000;i.phase=11;i.mode=3;return i;}
static void next(d::Input& i){++i.sample;i.now+=10;i.sampleTick=i.now;i.start=i.release=false;}
int main(){
 d::Tracker t;auto i=input();auto r=t.step(i);ck(!r.id,"capture during existing/native work cannot adopt");
 next(i);i.start=true;r=t.step(i);ck(r.id==1&&r.startTick==1010&&r.startSample==2&&!r.releaseTick,"raise edge creates first ID");
 r=t.step(i);ck(r.id==1,"duplicate XR frame does not create another ID");
 next(i);i.phase=14;r=t.step(i);ck(r.id==1,"native raise phase preserves identity");
 next(i);i.release=true;r=t.step(i);ck(r.releaseTick==1030&&r.releaseSample==4,"downward reversal records exact input sample");
 ck(!std::strcmp(d::join(r,1,1030,10,7,true,4,8,9),"CORRELATED_NATIVE_POINT_ONLY"),"exact native point and placed hammer may correlate");
 ck(!std::strcmp(d::join(r,2,1030,10,7,true,4,8,9),"CAPTURE_CHANGED"),"new capture never joins old ID");
 ck(!std::strcmp(d::join(r,1,1030,11,7,true,4,8,9),"STATION_CHANGED"),"station identity exact");
 ck(!std::strcmp(d::join(r,1,1111,10,7,true,4,8,9),"INPUT_STALE"),"81ms-old input not freshened by native call");
 ck(!std::strcmp(d::join(r,1,1030,10,7,false,4,8,9),"NO_PLACED_HAMMER"),"unplaced hammer not accepted");
 ck(!std::strcmp(d::join(r,1,1030,10,7,true,4,99,9),"EPOCH_CHANGED"),"source epoch mismatch refused");
 next(i);i.phase=16;r=t.step(i);ck(r.id==1&&r.nativeFallSeen,"native falling keeps same released input ID");
 next(i);i.phase=17;r=t.step(i);ck(r.id==1,"impact/finish family preserves pending observation");
 next(i);i.phase=11;r=t.step(i);ck(!r.id,"return to idle retires completed observation");
 next(i);i.start=true;r=t.step(i);ck(r.id==2,"later stroke gets new ID");
 next(i);i.abort=true;r=t.step(i);ck(!r.id,"manual takeover or discontinuity revokes ID");
 next(i);i.abort=false;i.release=true;r=t.step(i);ck(!r.id,"release after revocation cannot adopt");
 for(unsigned field=0;field<5;++field){t={};i=input();i.start=true;ck(t.step(i).id==1,"new fixture start");next(i);
  if(field==0)++i.lifetime;if(field==1)++i.pose;if(field==2)++i.source;if(field==3)++i.modeEpoch;if(field==4)i.mode=2;
  ck(!t.step(i).id,"context/epoch/mode changes revoke");}
 t={};i=input();i.start=true;r=t.step(i);next(i);i.sampleTick=i.now-81;ck(!t.step(i).id,"stale tracking revokes immediately");
 t={};i=input();i.start=true;t.step(i);next(i);i.sample=1;ck(t.step(i).id==1,"identical sample alone remains bounded observation");
 i.sample=0;ck(!t.step(i).id,"zero sample invalid");
 t={};i=input();i.sample=10;i.start=true;t.step(i);next(i);i.sample=2;ck(!t.step(i).id,"sample regression revokes");
 t={};i=input();i.start=true;t.step(i);i.capture=false;ck(!t.step(i).id,"capture off publishes empty");
 i=input();i.release=true;ck(!t.step(i).id,"capture resume cannot adopt old stroke");
 t={};i=input();i.start=true;i.nativeHeight=false;r=t.step(i);ck(r.id==1&&!r.nativeHeight,"legacy clock producer explicitly distinguished");
 t={};i=input();i.start=true;i.eligible=false;ck(!t.step(i).id,"native/non-gesture mode cannot create VR ID");
 t={};i=input();for(unsigned n=0;n<128;++n){next(i);i.start=true;ck(t.step(i).id==n+1,"finite stroke budget admits known raise edge");}
 next(i);i.start=true;r=t.step(i);ck(!r.id,"129th start must not reuse 128th stroke");
 ++i.session;next(i);i.start=true;ck(t.step(i).id==1,"new explicit capture resets finite budget");
 std::printf("smith_stroke_receipt_d57_test: %u checks PASS\n",checks);
}
