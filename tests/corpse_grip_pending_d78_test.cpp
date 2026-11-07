// Actual CG1 input policy; the separate dual-hand worker fixture also executes
// ordinary producers. Nomination/body/physics leaves here never prove engine success.
#include <subsystems/physics/corpse_grip_cg1.h>
#include <cstdio>
namespace c=kcd2vr::corpse_grip_cg1;
static unsigned checks=0,failed=0;
static void check(bool ok,const char* why){++checks;if(!ok){++failed;std::printf("FAIL %s\n",why);}}
template<class C> static C near(C in){
 if constexpr(requires(C x){x.pendingOnlyD78;x.requestEpochD78;x.trackingEpochD78;}){
  in.pendingOnlyD78=true;in.requestEpochD78=5;in.trackingEpochD78=6;return in;
 }else return {}; // baseline's script supplies no near nomination
}
template<class C> static void ready(C& in){if constexpr(requires(C x){x.pendingOnlyD78;})in.pendingOnlyD78=false;}
template<class C> static void changeEpoch(C& in,int which){
 if constexpr(requires(C x){x.requestEpochD78;x.trackingEpochD78;}){
  if(which==0)++in.requestEpochD78;else ++in.trackingEpochD78;
 }
}
template<class O> static std::uint64_t returned(const O& out){
 if constexpr(requires(O x){x.returnedPressD78;})return out.returnedPressD78;else return 0;
}
template<class G> static c::Output update(G& gate,std::uint64_t now,bool current,bool context,float grip,const c::Candidate& in,bool neutral=true){
 if constexpr(requires(G x){x.update(now,current,context,grip,in,neutral);})return gate.update(now,current,context,grip,in,neutral);
 else return gate.update(now,current,context,grip,in);
}
static c::Candidate candidate(std::uint64_t now){
 c::Key key{11,22,33,44,1,55,-1};key.physicsEpoch=8;key.route=c::Route::EntityVelocityD72;key.scriptToken=7;
 return {key,now,false};
}
struct Admission {static inline bool allow=true;bool take(unsigned){return allow;}};
int main(){
 // A missing nomination never parks an arbitrary world press or steals an
 // already-started ordinary Use when a candidate appears later.
 {c::Gate g;update(g,1000,true,false,0,{});auto out=update(g,1010,true,false,1,{});
  check(!out.suppressLeftUse&&!out.native.press,"no witness leaves ordinary press untouched");
  out=update(g,1070,true,true,1,candidate(1070));check(!out.native.press,"late first witness cannot steal ordinary press");}
 {c::Gate g;auto n=near(candidate(1000));update(g,1000,true,true,0,n);n.tick=1010;
  auto out=update(g,1010,true,true,1,n);const auto original=g.press;
  check(out.suppressLeftUse&&!out.native.press&&original,"near witness reserves but grants no body drive");
  n.tick=1070;ready(n);out=update(g,1070,true,true,1,n);
  check(out.native.press==original&&out.native.press&&c::permits(out.native,n,1070),"same witnessed target promotes original squeeze");
  n=near(n);check(!c::permits(out.native,n,1070),"near-only nomination cannot authorize native acquisition");}
 // Current unspent expiry is explicit; the same held squeeze cannot later
 // become corpse authority after its return to ordinary input.
 {c::Gate g;auto n=near(candidate(1000));update(g,1000,true,true,0,n);
  n.tick=1010;update(g,1010,true,true,1,n);n.tick=1080;update(g,1080,true,true,1,n);
  n.tick=1131;auto out=update(g,1131,true,true,1,n);
  check(returned(out)==1010&&!out.native.press&&!out.suppressLeftUse&&!g.consumed,"expiry returns exact unspent press timestamp");
  n.tick=1140;ready(n);out=update(g,1140,true,true,1,n);check(!out.native.press,"returned occurrence cannot acquire later");}
 // Every identity/continuity change spends the old press without returning it.
 for(int cause=0;cause<6;++cause){
  c::Gate g;auto n=near(candidate(2000));update(g,2000,true,true,0,n);n.tick=2010;update(g,2010,true,true,1,n);
  n.tick=2070;if(cause==0)++n.key.identity;if(cause==1)++n.key.epoch;
  if(cause==2)changeEpoch(n,0);if(cause==3)changeEpoch(n,1);
  auto out=update(g,2070,cause!=4,cause!=5,1,n);
  n.tick=2132;ready(n);out=update(g,2132,true,true,1,n);
  check(!out.native.press&&!returned(out)&&out.suppressLeftUse,"changed target/body/request/tracking/input/context never replays");
 }
 {c::Gate g;auto n=near(candidate(3000));update(g,3000,true,true,0,n);n.tick=3010;update(g,3010,true,true,1,n);
  n.tick=3080;update(g,3080,true,true,1,n,false);n.tick=3131;auto out=update(g,3131,true,true,1,n);
  check(!returned(out)&&out.suppressLeftUse&&!out.native.press,"lost ordinary neutral eligibility cannot replay on expiry");
  out=update(g,3140,true,true,0,n);check(!out.suppressLeftUse&&!g.consumed,"genuine release always retires pending press");}
 {c::Gate g;auto n=near(candidate(4000));update(g,4000,true,true,0,n);n.tick=4010;update(g,4010,true,true,1,n);
  n.tick=4131;auto out=update(g,4131,true,true,1,n);check(!returned(out)&&out.suppressLeftUse,"unobserved input gap cannot return a press");}
 // Admission refusal happens BEFORE the ordinary producers. No delayed corpse
 // acquisition may consume that rejected original press on the next callback.
 {c::Gate g;kcd2vr::core::SnapshotMailbox<c::Request> mailbox;auto n=near(candidate(5000));
  c::claims.consumed[0]=false;c::claims.fault[0]=false;c::claims.nativeHeld[0]=false;c::claims.livePress[0]=0;
  c::updateShared<Admission>(g,0,5000,true,true,0,n,mailbox);n.tick=5010;Admission::allow=false;
  auto out=c::updateShared<Admission>(g,0,5010,true,true,1,n,mailbox);Admission::allow=true;
  check(!out.suppressLeftUse&&!out.native.press&&!c::claims.consumed[0],"busy existing owner preserves ordinary press");
  n.tick=5060;ready(n);out=c::updateShared<Admission>(g,0,5060,true,true,1,n,mailbox);
  check(!out.native.press,"admission retry cannot steal already-delivered squeeze");}
 std::printf("D78 pending corpse input: %u checks, %u failures\n",checks,failed);return failed?1:0;
}
