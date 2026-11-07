// Production recognizer: walking is not a punch; no movement-wide veto.
#define main ExistingTravelD65
#include "fist_stroke_d49_test.cpp"
#undef main
static pc::Quat headPitchD65(double a){return {std::sin(a*.5),0,0,std::cos(a*.5)};}
int main(){
 checks=failed=0;
 for(float base:{.045f,.10f,.20f})for(bool trial:{false,true}){
  kcd2vr::punch_travel_d65::minimumMeters.store(base);
  {State s{};auto x=sample(trial);ready(s,x);unsigned n=0;for(int i=0;i<80;++i){x.allowed[0].z-=.018;x.allowed[0].y+=i%2?.005:-.005;n+=advance(s,x).valid;}ck(!n,"joystick/native body translation and bob with steady raw hands cannot punch");}
  {State s{};auto x=sample(trial);ready(s,x);unsigned n=0;for(int i=0;i<25;++i){x.head.z-=.012;x.allowed[0].z-=.004;n+=advance(s,x).valid;}ck(!n,"HMD forward lean with stationary controller cannot punch");}
  {State s{};auto x=sample(trial);ready(s,x);unsigned n=0;for(int i=0;i<80;++i){x.head.z-=.018;for(int h=0;h<2;++h){x.controller[h].z-=.018;x.allowed[h].z-=.018;}n+=advance(s,x).valid;}ck(!n,"shared physical head and controller translation cannot punch");}
  {State s{};auto x=sample(trial);ready(s,x);unsigned n=0;for(int i=0;i<18;++i){x.head.z-=.012;for(int h=0;h<2;++h){x.controller[h].z-=.012;x.allowed[h].z-=.012;}x.controller[0].z-=.018;x.allowed[0].z-=.018;n+=advance(s,x).valid;}ck(n==1,"real punch during shared forward movement remains once-only");for(int i=0;i<18;++i){x.head.z-=.012;for(int h=0;h<2;++h){x.controller[h].z-=.012;x.allowed[h].z-=.012;}x.controller[0].z+=.018;x.allowed[0].z+=.018;n+=advance(s,x).valid;}ck(n==1,"return while walking cannot nominate another punch");}
  for(int kind=0;kind<3;++kind){State s{};auto x=sample(trial);ready(s,x);unsigned n=0;int observed=-1;for(int i=0;i<24;++i){x.head.z-=.012;for(int h=0;h<2;++h){x.controller[h].z-=.012;x.allowed[h].z-=.012;}x.headOrientation=headPitchD65(-double(i+1)*.003);const pc::Vec3 arm=kind==0?pc::Vec3{0,0,-.018}:kind==1?pc::Vec3{.018,0,-.008}:pc::Vec3{0,.018,0};x.controller[0]=x.controller[0]+arm;x.allowed[0]=x.allowed[0]+arm;auto e=advance(s,x);if(e.valid){++n;observed=e.motion167;}}ck(n==1&&observed==kind,"straight/hook/uppercut labels preserve motion geometry during walking and head turn");}
  {State s{};auto x=sample(trial);ready(s,x);unsigned n=0;const auto hand0=x.controller[0]-x.head,hand1=x.controller[1]-x.head;for(int i=0;i<30;++i){x.head.z-=.012;x.headOrientation=headPitchD65(-double(i+1)*.025);x.controller[0]=x.head+pc::rotate(x.headOrientation,hand0);x.controller[1]=x.head+pc::rotate(x.headOrientation,hand1);x.allowed[0]=x.controller[0];x.allowed[1]=x.controller[1];n+=advance(s,x).valid;}ck(!n,"whole body rigid rotation while walking remains non-punch");}
  {State s{};auto x=sample(trial);ready(s,x);unsigned n=0;for(int i=0;i<18;++i){x.head.z-=.012;for(int h=0;h<2;++h){x.controller[h].z-=.012;x.allowed[h].z-=.012;}x.headOrientation=headPitchD65(-double(i+1)*.010);x.controller[0].z-=.018;x.allowed[0].z-=.018;n+=advance(s,x).valid;}ck(n==1,"real extension survives simultaneous walk and head turn");}
  {State s{};auto x=sample(trial);ready(s,x);unsigned n=0;for(int i=0;i<20;++i){x.head.z-=.012;for(int h=0;h<2;++h){x.controller[h].z-=.012;x.allowed[h].z-=.012;}x.headOrientation=headPitchD65(-double(i+1)*.025);x.controller[0].z-=double(base)*.025;x.allowed[0].z-=double(base)*.025;n+=advance(s,x).valid;}ck(!n,"head turn cannot inflate half-threshold arm travel into a punch");}
  {State s{};auto x=sample(trial);ready(s,x);unsigned n=0;for(int i=0;i<30;++i){x.head.z-=.012;for(int h=0;h<2;++h){x.controller[h].z-=.012;x.allowed[h].z-=.012;}x.headOrientation=headPitchD65(-double(i+1)*.025);n+=advance(s,x).valid;}std::printf("common_walk_head_pitch base=%.3f trial=%d events=%u\n",base,trial?1:0,n);ck(!n,"shared translation plus head pitch cannot manufacture stroke travel");}
 }
 std::printf("fist_locomotion_d65=%u/%u native_execution=NONE\n",checks-failed,checks);return failed?1:0;
}
