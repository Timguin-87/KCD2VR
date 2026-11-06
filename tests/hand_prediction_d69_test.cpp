#include "vrik/hand_prediction_d69.h"
#pragma warning(push)
#pragma warning(disable:4127) // existing constexpr runtime branch in unchanged fist recognizer
#include "vrik/fist_motion.h"
#pragma warning(pop)
#include <cstdio>
#include <cstring>
#include <limits>
namespace t=kcd2vr::tracking59c6;namespace h=kcd2vr::hand_prediction_d69;namespace o=kcd2vr::claude_motion_d69;namespace f=kcd2vr::fists;
static unsigned count=0,fail=0;
static void ck(bool b,const char* s){++count;fail+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",s);}
static t::Frame family(){t::Frame f{};f.epoch=3;f.sequence=22;f.tick=1000;f.ns=1000000000;f.locateTime=2000000000;f.controllersSampled=true;for(unsigned i=0;i<3;++i){f.valid[i]=f.tracked[i]=true;f.poses[i]={float(i)*.2f,1.5f-float(i)*.4f,-.3f,0,0,0,1};f.goodTick[i]=f.tick;}return f;}
static bool same(const t::Pose& a,const t::Pose& b){return std::memcmp(&a,&b,sizeof(a))==0;}
int main(){
 ck(!o::HandLead()&&!o::ElbowHysteresis()&&o::HandLeadMs()==11&&o::ExtraNs()==0,"all trials default OFF despite stored11ms");
 h::Velocity v[2]{{{1,0,0},{0,2,0},true,true},{{0,0,-2},{0,0,0},true,true}};
 unsigned calls=0;bool allow=true,tracked=true;
 auto locate=[&](std::int64_t at,t::Pose (&p)[2],bool (&valid)[2]){++calls;ck(at==2011000000,"fallback uses base target plus explicit11ms");p[0]={.7f,1,-.2f,0,0,0,1};p[1]={-.7f,1,-.2f,0,0,0,1};valid[0]=valid[1]=tracked;return allow;};
 auto a=family(),original=a;h::Apply(a,v,o::ExtraNs(),locate);ck(!std::memcmp(&a,&original,sizeof(a))&&!calls,"OFF exact family bytes and no extra locate");
 o::SetHandLead(true);o::SetHandLeadMs(0);h::Apply(a,v,o::ExtraNs(),locate);ck(!std::memcmp(&a,&original,sizeof(a))&&!calls,"zero exact family bytes and no extra locate");
 o::SetHandLeadMs(11);h::Apply(a,v,o::ExtraNs(),locate);a.handPolicyD69=h::EffectivePolicy(a,o::ExtraNs());
 ck(!calls&&a.handLeadSourceD69[0]==1&&a.handLeadSourceD69[1]==1,"both hands use same-locate valid runtime velocity");
 ck(std::abs(a.poses[1].x-original.poses[1].x-.011)<1e-7&&std::abs(a.poses[2].z-original.poses[2].z+.022)<1e-7,"translation extrapolation follows each controller velocity");
 ck(std::abs(a.poses[1].qy-std::sin(.011))<1e-7&&std::abs(a.poses[1].qw-std::cos(.011))<1e-7,"reference-space angular delta multiplies quaternion on left");
 ck(same(a.poses[0],original.poses[0])&&a.locateTime==original.locateTime&&a.ns==original.ns&&a.tick==original.tick&&a.epoch==original.epoch&&a.goodTick==original.goodTick,"head target and source/last-good ages unchanged");
 const auto both=h::EffectivePolicy(a,11000000);ck(both&&a.handTargetD69[0]==2011000000&&a.handTargetD69[1]==2011000000,"actual per-hand future targets retained separately");
 auto b=family();v[0].angularValid=false;h::Apply(b,v,11000000,locate);ck(calls==1&&b.handLeadSourceD69[0]==2&&b.handLeadSourceD69[1]==1,"missing one velocity component uses one pair locate, leaves valid opposite velocity path");ck(h::EffectivePolicy(b,11000000)==both,"velocity vs future-locate same target does not reset intent");
 b=family();allow=false;h::Apply(b,v,11000000,locate);ck(same(b.poses[1],original.poses[1])&&b.handTargetD69[0]==0&&h::EffectivePolicy(b,11000000)!=both,"future failure retains exact baseline side and distinct effective policy");
 b=family();allow=true;tracked=false;h::Apply(b,v,11000000,locate);ck(b.handLeadSourceD69[0]==0&&same(b.poses[1],original.poses[1]),"valid but untracked future pose cannot lead");
 b=family();b.tracked[1]=false;auto n=calls;h::Apply(b,v,11000000,locate);ck(calls==n&&same(b.poses[1],original.poses[1]),"untracked current hand never gets predicted or future-located");
 b=family();b.tracked[0]=false;n=calls;h::Apply(b,v,11000000,locate);ck(calls==n&&same(b.poses[1],original.poses[1])&&same(b.poses[2],original.poses[2]),"untracked HMD preserves all baseline hand poses");
 for(auto extra:{-1ll,0ll,33000001ll}){b=family();n=calls;h::Apply(b,v,extra,locate);ck(calls==n&&!std::memcmp(&b,&original,sizeof(b)),"out-of-bounds or zero horizon cannot lead");}
 auto p=original.poses[1];auto old=p;v[0].angularValid=true;v[0].linear[0]=std::numeric_limits<float>::quiet_NaN();ck(!h::Extrapolate(p,v[0],11000000)&&same(p,old),"nonfinite runtime velocity leaves whole pose unchanged");
 v[0].linear[0]=21;ck(!h::Extrapolate(p,v[0],11000000)&&same(p,old),"implausible runtime velocity leaves whole pose unchanged");
 o::SetHandLeadMs(99);ck(o::HandLeadMs()==33,"slider upper bound33ms");o::SetHandLeadMs(-1);ck(o::ExtraNs()==0,"slider lower bound really zero");o::SetHandLead(false);ck(o::ExtraNs()==0,"OFF retires requested prediction");
 // A predictor setting/fallback transition cannot borrow an ordinary fist onset.
 struct Change{std::uint64_t before,after;};
 for(bool trial:{false,true})for(auto change:{Change{0,both},Change{both,0},Change{both,both-1},Change{both-1,both},Change{both,(22000000ull<<2)|3}}){
  f::State state{};f::Sample s{};s.epoch=s.sequence=1;s.tick=1000;s.actor=123;s.valid=s.empty[0]=s.empty[1]=true;s.head={0,1.6,0};s.controller[0]=s.allowed[0]={.2,1.25,-.2};s.controller[1]=s.allowed[1]={-.2,1.25,-.2};s.strokeExperimentD49=trial;s.predictionPolicyD69=change.before;
  f::step(state,s);s.grip[0]=s.trigger[0]=1;for(unsigned i=0;i<20;++i){++s.sequence;s.tick+=10;f::step(state,s);}
  s.controller[0].z-=.02;s.allowed[0].z-=.02;++s.sequence;s.tick+=10;f::step(state,s);
  s.predictionPolicyD69=change.after;s.controller[0].z-=.15;s.allowed[0].z-=.15;++s.sequence;s.tick+=10;
  ck(!f::step(state,s).valid&&state.predictionPolicyD69==s.predictionPolicyD69,"prediction policy transition cannot turn old onset into a punch");
 }
 std::printf("hand_prediction_d69 %u/%u\n",count-fail,count);return fail?1:0;
}
