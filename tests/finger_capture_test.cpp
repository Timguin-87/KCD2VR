#include "../src/vrik/finger_capture.h"
#include <cstdio>
#include <limits>
using namespace kcd2vr::finger_capture;
static int tests=0,failed=0;
static void Check(bool ok,const char*name){++tests;failed+=!ok;std::printf("%s %s\n",ok?"PASS":"FAIL",name);}
static Hand hand(int side,double curl=0){Hand h;h.count=15;for(int d=0;d<5;++d){h.bend[d]=curl;for(int n=0;n<3;++n){int i=d*3+n;h.joints[i]={100+side*100+i,n?99+side*100+i:side+1,d,unsigned(1+side*100+i),{0,0,std::sin(curl*.25),std::cos(curl*.25)},{.03,0,0}};}}return h;}
struct Run {
 Recorder recorder;Frame f;Pair result;bool completed=false;
 Run(){f.rig=123;f.allowed=f.empty=f.tracked=true;f.tick=1000;f.hand={hand(0),hand(1)};}
 void tick(unsigned dt=50){++f.sample;f.tick+=dt;completed=recorder.update(f,result)||completed;}
 void open(){f.trigger[0]=f.trigger[1]=f.grip[0]=f.grip[1]=0;for(int i=0;i<8;++i)tick();}
 void press(){f.trigger[0]=f.trigger[1]=1;tick();}
 void close(){f.hand={hand(0,1.5),hand(1,1.7)};for(int i=0;i<24;++i)tick();}
 void release(){f.trigger[0]=f.trigger[1]=0;tick();}
};
int main(){
 auto h=hand(0);Check(valid(h),"complete native hand accepted");h.joints[1].crc=h.joints[0].crc;Check(!valid(h),"duplicate joint identity rejected");
 h=hand(0);h.joints[4].q.w=std::numeric_limits<double>::quiet_NaN();Check(!valid(h),"nonfinite quaternion rejected");h=hand(0);h.joints[4].q={0,0,0,0};Check(!valid(h),"zero quaternion rejected");
 h=hand(0);h.joints[4].t.x=.3;Check(!valid(h),"unreasonable finger offset rejected");h=hand(0);h.count=9;Check(!valid(h),"incomplete hand rejected");
 Check(!clenched(hand(0),hand(0)),"button labels cannot turn same geometry into fist");Check(clenched(hand(0),hand(0,1.5)),"independent bent geometry qualifies");
 auto one=hand(0);one.bend[1]=2;Check(!clenched(hand(0),one),"one bent digit does not qualify a full fist");
 Run r;r.open();Check(r.recorder.stable[0].ready&&r.recorder.stage==Stage::Open,"released interval stabilizes native open hands");
 r.f.hand={hand(0,.3),hand(1,.4)};for(int i=0;i<10;++i)r.tick();r.press();Check(r.recorder.open[0].bend[1]==.3&&r.recorder.open[1].bend[1]==.4,"open capture follows latest settled native pose instead of first category frame");
 Check(r.recorder.stage==Stage::Clenching&&!r.recorder.haveBest[0],"press enters settling not false qualified status");
 r.close();Pair candidate;Check(r.recorder.candidate(candidate)&&r.recorder.stage==Stage::Qualified,"both native fists produce a preview candidate while held");
 Check(candidate.closed[1].bend[1]==1.7&&candidate.closed[0].bend[1]==1.5,"left and right saved independently without mirroring");r.release();Check(r.completed&&valid(r.result)&&r.recorder.stage==Stage::Ready,"deliberate release completes one valid pair");
 bool again=false;for(int i=0;i<20;++i){++r.f.sample;r.f.tick+=50;again|=r.recorder.update(r.f,candidate);}Check(!again,"finished capture cannot emit repeatedly");
 Run buttons;buttons.open();buttons.press();for(int i=0;i<30;++i)buttons.tick();buttons.release();Check(!buttons.completed&&!buttons.recorder.candidate(candidate),"both triggers alone never replace endpoints");
 Run half;half.open();half.press();half.f.hand[0]=hand(0,2);for(int i=0;i<24;++i)half.tick();half.release();Check(!half.completed,"one native fist cannot replace the two-hand profile");
 Run held;held.f.trigger[0]=held.f.trigger[1]=1;for(int i=0;i<10;++i)held.tick();Check(held.recorder.stage==Stage::NeedRelease,"entering while holding triggers requires release");held.open();held.press();held.close();held.release();Check(held.completed,"held-entry workflow rearms on released input");
 Run grip;grip.f.grip[0]=1;for(int i=0;i<10;++i)grip.tick();Check(grip.recorder.stage==Stage::NeedRelease,"grip-held pose is not used as relaxed hand");
 Run shortOpen;shortOpen.tick();shortOpen.press();Check(!shortOpen.recorder.haveOpen,"trigger before open stability does not snapshot transient");
 Run delayed;delayed.open();delayed.f.trigger[0]=1;delayed.tick();for(int i=0;i<22;++i)delayed.tick();Check(delayed.recorder.stage==Stage::Paused,"one trigger past chord interval cancels instead of attacking capture assumptions");
 Run stagger;stagger.open();stagger.f.trigger[0]=1;stagger.tick();stagger.tick(100);stagger.f.trigger[1]=1;stagger.tick();stagger.close();stagger.release();Check(stagger.completed,"small inter-hand trigger timing difference accepted");
 Run loss;loss.open();loss.press();loss.close();loss.f.tracked=false;loss.tick();loss.f.tracked=true;loss.release();Check(!loss.completed,"tracking loss cancels without treating it as release");
 Run menu;menu.open();menu.press();menu.close();menu.f.allowed=false;menu.tick();menu.f.allowed=true;menu.release();Check(!menu.completed,"menu interruption does not commit partial pair");
 Run weapon;weapon.open();weapon.press();weapon.close();weapon.f.empty=false;weapon.tick();weapon.release();Check(!weapon.completed,"held native item cancels empty-hand capture");
 Run changeRig;changeRig.open();changeRig.press();changeRig.close();changeRig.f.rig=456;changeRig.release();Check(!changeRig.completed,"rig change cannot accept old fist endpoints");
 Run gap;gap.open();gap.press();gap.close();gap.tick(250);gap.release();Check(!gap.completed,"a sample gap invalidates pending fist pair");
 Run back;back.open();back.press();back.close();back.f.tick=500;back.release();Check(!back.completed,"time reversal cannot finalize old pair");
 Run dup;dup.open();dup.press();for(int i=0;i<20;++i){dup.f.tick+=50;dup.recorder.update(dup.f,candidate);}Check(!dup.recorder.haveBest[0],"duplicate tracking sample never advances pose stability");
 Run drift;drift.open();drift.press();for(int i=0;i<30;++i){drift.f.hand[0]=hand(0,i%2?2:.5);drift.f.hand[1]=hand(1,i%2?2:.5);drift.tick();}drift.release();Check(!drift.completed,"changing attack fingers must settle before acceptance");
 Run sign;sign.open();sign.press();sign.f.hand={hand(0,2),hand(1,2)};for(int i=0;i<24;++i){for(auto& a:sign.f.hand)for(int j=0;j<a.count;++j){auto&q=a.joints[j].q;q={-q.x,-q.y,-q.z,-q.w};}sign.tick();}sign.release();Check(sign.completed,"equivalent quaternion signs do not reset stable pose");
 Run anatomy;anatomy.open();anatomy.press();anatomy.f.hand={hand(0,2),hand(1,2)};anatomy.f.hand[0].joints[1].t.x+=.01;for(int i=0;i<24;++i)anatomy.tick();anatomy.release();Check(!anatomy.completed,"segment translation drift cannot qualify a matching rig");
 Run maximum;maximum.open();maximum.press();maximum.close();maximum.f.hand={hand(0,2),hand(1,2.2)};for(int i=0;i<24;++i)maximum.tick();maximum.release();Check(maximum.completed&&maximum.result.closed[0].bend[1]==2,"later more bent whole pose replaces initial settled fist candidate");
 Run less;less.open();less.press();less.close();less.f.hand={hand(0,1),hand(1,1)};for(int i=0;i<24;++i)less.tick();less.release();Check(less.completed&&less.result.closed[0].bend[1]==1.5,"later less closed pose does not lose qualified coherent fist");
 std::printf("finger_capture=%d/%d (geometry gate, not anatomical maxima or headset validation)\n",tests-failed,tests);return failed?1:0;
}
