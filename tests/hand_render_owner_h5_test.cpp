// Actual coherent pose/publication + verbatim render getter decision.
// Native memory/setter services are existing explicit pose_adapter leaves.
#define main retained_pose_adapter_h5
#ifndef KCD2VR_HAND_RENDER_POSE_FIXTURE_H5
#define KCD2VR_HAND_RENDER_POSE_FIXTURE_H5 "pose_adapter_test.cpp"
#endif
#include KCD2VR_HAND_RENDER_POSE_FIXTURE_H5
#include "../src/vrik/tracking_grace111.h"
#undef main
namespace rb63=kcd2vr::render_bounds63;
using HenryBoundsGetter63=const float*(__fastcall*)(void*);
static HenryBoundsGetter63 g_henry_bounds_original63=nullptr;
static std::atomic<int> g_census_mode{5};
static void* ReturnLeafH5(){return reinterpret_cast<void*>(rb63::kCaller);}
#ifdef _ReturnAddress
#undef _ReturnAddress
#endif
#define _ReturnAddress() ReturnLeafH5()
#include <hand_render_owner_h5_actual.inc>
#undef _ReturnAddress
static TrackingSnapshot sourceH5(){auto t=Tracking();t.source_epoch=kcd2vr::tracking59c6::epoch.load();t.input_tick=t.tick;
 t.head_good_tick=t.right_good_tick=t.left_good_tick=t.tick;t.timeNs=t.tick*1000000ull;return t;}
static const rb63::Box nativeBoxH5{{-.1f,-.1f,0,.1f,.1f,1.8f}};
static void beginH5(){Init();g_frame_publication51=0;g_frame_control51=2;diagnostics::lab::Set(diagnostics::lab::Key::F3,0);g_census_mode=5;g_henry_bounds_enabled63=true;animation_filter::nativeBodyOwned112=false;
 for(auto& c:g_henry_bounds_reject63)c=0;g_henry_bounds_calls63=0;g_henry_bounds_applied63=0;}
static const float* renderH5(std::uint64_t now){
 const std::uintptr_t vf=WhBase()+rb63::kCharacterVft;std::memcpy(character.data(),&vf,sizeof(vf));
 std::memcpy(character.data()+0x9E0,&nativeBoxH5,sizeof(nativeBoxH5));
 return SelectHenryBounds63(character.data(),reinterpret_cast<const float*>(character.data()+0x9E0),WhBase()+rb63::kCaller,now);
}
static bool nativeH5(const float* p){return p==reinterpret_cast<const float*>(character.data()+0x9E0);}
static bool ownedRunH5(TrackingSnapshot t,int strip=0){CoherentTransaction6689 tx;
 bool ok=BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,3,t);
 if(strip)for(int j=0;j<Count;++j)if(g_pose6689.armL[j]||j==rig.clav_l)tx.selected[j]=(strip>=2&&(j==rig.clav_l||j==rig.upper_l||j==rig.fore_l||(strip==3&&j==rig.hand_l)));
 tx.Restore();PublishCommitted6689(tx);return ok;}
int main(){tests=failures=0;beginH5();auto t=sourceH5();
 Check(ownedRunH5(t),"current two-hand actual pose commits");
 auto baseRender=renderH5(t.tick);Check(!nativeH5(baseRender),"current two-hand completed envelope reaches actual render union");
 for(int side=0;side<2;++side)for(int state=0;state<2;++state){
  beginH5();t=sourceH5();const int live=1-side;
  if(side==0){t.right_held=state==0;t.right_valid=state==0;}else{t.left_held=state==0;t.left_valid=state==0;}
  Check(ownedRunH5(t),"held-grace or invalid other controller does not reject current opposite pose");
  CommittedFrame6695 p{};rb63::Envelope e{};const bool packets=g_committed_frame6695.read(p)&&g_render_bounds63.read(e);
  Check(packets&&p.valid[live]&&e.character==p.stamp.actor&&e.epoch==p.stamp.epoch&&e.sample==p.stamp.sample&&e.tick==p.stamp.tick&&e.publication==g_frame_publication51.load()&&p.doorTrackingEpoch66==t.source_epoch,"actual same actor/body/source/sample/clock/publication joins unchanged");
  const auto rendered=renderH5(t.tick);const bool applied=!nativeH5(rendered);
  std::printf("CASE lostSide=%d state=%s posePublished=%d validR/L=%d/%d envelopeFresh=%d points=%u applied=%d freshnessRefusal=%llu\n",side,state==0?"VALID_HELD_GRACE":"INVALID",g_committed_pose_valid6689.load(),p.valid[0],p.valid[1],e.freshTracked,e.points,applied,(unsigned long long)g_henry_bounds_reject63[unsigned(BoundsReject63::EnvelopeFreshness)].load());
  Check(applied,"still-current opposite hand retains render correction independently of unavailable hand");
  Check(e.points==19&&((side==0&&e.box.v[3]<=0)||(side==1&&e.box.v[0]>=0)),"unavailable side contributes no stale wrist/finger/clavicle point; current selected side owns19 actual points");
  const int u=live?rig.upper_l:rig.upper_r,f=live?rig.fore_l:rig.fore_r,h=live?rig.hand_l:rig.hand_r;
  Check(std::abs(Len(Sub(FromT(g_pose6689.work[f]),FromT(g_pose6689.work[u])))-g_pose6689.upper[live])<.0001&&std::abs(Len(Sub(FromT(g_pose6689.work[h]),FromT(g_pose6689.work[f])))-g_pose6689.fore[live])<.0001,"current arm preserves both exact anatomical segment lengths");
  unsigned long long digest=1469598103934665603ull;const auto* bytes=reinterpret_cast<const unsigned char*>(g_pose6689.work.data());for(unsigned k=0;k<Count*sizeof(QuatT28);++k){digest^=bytes[k];digest*=1099511628211ull;}
  std::printf("POSE_BYTES lostSide=%d state=%d digest=%016llX\n",side,state,digest);
  Check(std::memcmp(character.data()+0x9E0,&nativeBoxH5,sizeof(nativeBoxH5))==0&&Same(fixture.live,fixture.original),"render selection does not alter native box/restored pose or fixed anatomy");
  Check(nativeH5(renderH5(t.tick+81)),"original80ms envelope expiry cannot be renewed by renderer reads");
 }
 beginH5();t=sourceH5();t.right_valid=false;Check(ownedRunH5(t,true),"actual live controller can be unselected after pose staging");Check(nativeH5(renderH5(t.tick)),"live but unselected arm cannot mint render points");
 beginH5();t=sourceH5();t.head_held=true;Check(ownedRunH5(t),"accepted short head grace pose policy is unchanged");Check(nativeH5(renderH5(t.tick)),"held HMD never grants new render correction");
 beginH5();t=sourceH5();t.right_held=t.left_held=true;Check(ownedRunH5(t),"two held valid hands keep existing pose grace policy");Check(nativeH5(renderH5(t.tick)),"no individually current hand grants no experimental envelope");
 beginH5();t=sourceH5();t.right_valid=t.left_valid=false;Check(!ownedRunH5(t)&&nativeH5(renderH5(t.tick)),"both invalid controller pose remains failclosed");

 // Actual tracking producer grace helper, original good clocks unchanged.
 for(unsigned age:{60u,61u,80u,81u}){
  beginH5();auto old=sourceH5();old.head_good_tick=old.left_good_tick=old.right_good_tick=old.tick;
  auto next=old;next.tick+=age;++next.sequence;next.right_valid=false;
  kcd2vr::tracking_grace111::HoldCoherent112(next,old);
  Check(next.right_valid==(age<=80)&&next.right_held==(age<=80)&&next.right_good_tick==(age<=80?old.tick:0),"actual hand producer keeps original good clock only through80ms");
  Check(ownedRunH5(next)&&!nativeH5(renderH5(next.tick)),"current left-arm render correction survives actual right grace or expiry without extending right pose");
  rb63::Envelope e{};g_render_bounds63.read(e);Check(e.tick==next.tick&&e.sample==next.sequence&&e.points==19&&e.box.v[3]<=0,"new completed source clock belongs only to current left points, never renewed held right pose");
 }
 for(unsigned age:{60u,61u}){
  beginH5();auto old=sourceH5();old.head_good_tick=old.left_good_tick=old.right_good_tick=old.tick;
  auto next=old;next.tick+=age;++next.sequence;next.head_valid=false;
  kcd2vr::tracking_grace111::HoldCoherent112(next,old);
  Check(next.head_valid==(age<=60)&&next.head_held==(age<=60)&&next.head_good_tick==(age<=60?old.tick:0),"actual HMD producer stops original head grace at60ms");
  const bool ok=ownedRunH5(next);Check(ok==(age<=60)&&nativeH5(renderH5(next.tick)),"held or expired head never grants a new union; original pose grace remains unchanged");
 }
 for(int strip:{2,3}){beginH5();t=sourceH5();t.right_valid=false;Check(ownedRunH5(t,strip),"actual selected ownership can expose a bounded current partial envelope");
  rb63::Envelope e{};g_render_bounds63.read(e);Check(e.points==unsigned(strip==2?3:4),"only current selected bones contribute to the actual envelope point count");
  Check(nativeH5(renderH5(t.tick))==(strip==2),"actual renderer preserves exact four-point minimum and rejects three-point envelope");}
 for(int mismatch=0;mismatch<10;++mismatch){beginH5();t=sourceH5();t.right_valid=false;Check(ownedRunH5(t)&&g_committed_pose_valid6689,"negative starts from an actual current one-hand publication");
  rb63::Envelope e{};CommittedFrame6695 p{};g_render_bounds63.read(e);g_committed_frame6695.read(p);
  switch(mismatch){case 0:++e.character;g_render_bounds63.publish(e);break;case 1:++g_pose_epoch6689;break;
   case 2:g_frame_publication51.fetch_add(2);break;case 3:g_frame_publication51.fetch_add(1);break;
   case 4:g_pose_policy_generation57.fetch_add(2);break;case 5:g_frame_control51.fetch_add(2);break;
   case 6:p.world[4]+=.01f;g_committed_frame6695.publish(p);break;
   case 7:{float scale=.5f;std::memcpy(character.data()+0xA24,&scale,4);break;}
   case 8:animation_filter::nativeBodyOwned112=true;break;
   case 9:e.box.v[0]=std::numeric_limits<float>::quiet_NaN();g_render_bounds63.publish(e);break;}
  Check(nativeH5(renderH5(t.tick)),"actor/body/publication/inflight/policy/control/packet-world/current-scale/native-body mismatch remains refused");
 }
 beginH5();t=sourceH5();t.right_valid=false;Check(ownedRunH5(t),"one-hand original clock boundary starts from actual pose");
 Check(!nativeH5(renderH5(t.tick+80))&&nativeH5(renderH5(t.tick+81))&&nativeH5(renderH5(t.tick-1)),"render reads preserve original80ms expiry and future-clock rejection");
 std::printf("HAND_ENVELOPE_H5 %d/%d actual_pose_to_actual_bounds; native/GPU/headset=NOT_EXECUTED\n",tests-failures,tests);return failures?1:0;
}
