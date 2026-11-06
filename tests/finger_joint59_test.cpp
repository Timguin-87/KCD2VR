#include <memory>
#define main legacy_pose_adapter59_main
#include "pose_adapter_test.cpp"
#undef main
#include <cassert>
int main(){
 for(int side=0;side<2;++side){
  auto state=std::make_unique<CoherentState6689>();auto&s=*state;s.count=7;s.rig.hand_r=s.rig.hand_l=0;
  s.fingerMap[side]=true;s.fingerCount[side]=6;s.open[side].ready=s.closed[side].ready=true;
  s.fingers[side][2].count=s.fingers[side][3].count=3;
  for(int k=0;k<3;++k){s.fingers[side][2].id[k]=k+1;s.fingers[side][3].id[k]=k+4;}
  for(int k=0;k<7;++k){s.rest[k].q[3]=s.restLocal[k].q[3]=1;s.parent[k]=k==2?1:k==3?2:k==5?4:k==6?5:0;}
  const float sign=side?-1.f:1.f;s.restLocal[4].t[0]=sign*.02f;
  s.restLocal[2].t[1]=s.restLocal[5].t[1]=.04f;s.restLocal[3].t[1]=s.restLocal[6].t[1]=.03f;
  for(int k=1;k<7;++k)s.rest[k]=AbsoluteFromRelative(s.rest[s.parent[k]],s.restLocal[k]);
  for(int k=0;k<6;++k)s.open[side].local[k].q[3]=s.closed[side].local[k].q[3]=1;
  for(int f=0;f<5;++f)for(int k=0;k<4;++k)kcd2vr::finger40::set(side,f,k,k==1?1.f:0.f);
  const auto before=s.restLocal;pc::Quat open{},closed{};
  TuneFinger40(s,side,2,1,2,open,closed);const auto tip=pc::rotate(closed,{0,.03,0});
  assert(tip.x*sign>0&&pc::angle(open,{})<1e-9);
  assert(std::abs(pc::angle(closed,{})-3*3.141592653589793/180)<1e-6);
  assert(!std::memcmp(before.data(),s.restLocal.data(),sizeof(before))); // fixed offsets and all other joints unchanged
  pc::Quat otherOpen{},otherClosed{};TuneFinger40(s,side,3,1,5,otherOpen,otherClosed);assert(pc::angle(otherClosed,{})<1e-9);
  s.closed[side].ready=false;open=closed={};TuneFinger40(s,side,2,1,2,open,closed);assert(pc::angle(closed,{})<1e-9);
 }

 // Actual production native-curve acquisition: identical shared skeleton/anim
 // pointers survive replacement while the new character's hand endpoints reset.
 Init();auto track=Tracking();assert(Run(track,3,true));auto& cs=g_pose6689;
 std::array<unsigned char,Count*0x30> joints59{};struct Owner59{void*v;void*j;} owner59{nullptr,joints59.data()};
 for(int j=0;j<Count;++j){const std::uint32_t crc=j+1;std::memcpy(joints59.data()+j*0x30+8,&crc,4);}
 cs.owner=&owner59;animation_filter::curveAvailable=true;
 for(int i=0;i<100;++i)AcquireCurveReferences6694(cs,reinterpret_cast<void*>(1),20000+i*12);
 assert(cs.open[0].ready&&cs.closed[1].ready&&g_finger_acquire6694.done);
 const auto immutable59=g_finger_file6694;assert(g_finger_loaded6694==g_finger_acquire6694.rig); // no worker disk read is required
 auto resetEndpoints59=[&]{for(int h=0;h<2;++h)cs.open[h].ready=cs.closed[h].ready=false;};
 resetEndpoints59();cs.character=reinterpret_cast<void*>(0x55555);++cs.initRuns137;animation_filter::curveSamples=0;
 AcquireCurveReferences6694(cs,reinterpret_cast<void*>(1),24000);
 assert(cs.open[0].ready&&cs.closed[1].ready&&animation_filter::curveSamples==0);
 assert(!std::memcmp(&immutable59,&g_finger_file6694,sizeof(immutable59)));
 resetEndpoints59();g_pose_epoch6689.fetch_add(1);AcquireCurveReferences6694(cs,reinterpret_cast<void*>(1),25000);
 assert(cs.closed[0].ready&&cs.open[1].ready&&animation_filter::curveSamples==0);
 resetEndpoints59();++cs.initRuns137;g_finger_file6694.crcs[0][0]^=1;animation_filter::curveAvailable=false;
 AcquireCurveReferences6694(cs,reinterpret_cast<void*>(1),26000);
 assert(!cs.open[0].ready&&!cs.closed[1].ready); // changed cached identity still refuses
 std::puts("C59 actual coherent PIP correction: both native chains, only middle finger, closed-only, anatomical lengths and missing-reference refusal PASS");
}
