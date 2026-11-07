// HJ1 prediction/cache/family regressions. Synthetic timing does not prove headset smoothness.
#include "vrik/hand_same_frame_hj1.h"
#include <cstdio>
namespace h=kcd2vr::hj1;
static int total=0,failed=0;
static void check(bool v,const char* n){++total;if(!v)++failed;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
int main(){
    const std::int64_t P=11111111; // 90 Hz
    check(!h::enabled.load(),"HJ1 disabled until explicitly selected");h::enabled.store(true);
    check(h::Ahead(2000000,4000000,P)==0,"read 2 ms after the head publish, camera 4 ms later -> same XR frame");
    check(h::Ahead(8000000,6000000,P)==1,"read 8 ms after publish, camera 6 ms later -> the next head (the old bug: hands 11 ms older)");
    check(h::Ahead(1000000,30000000,P)==2&&h::Ahead(1000000,49000000,P)==2,"more than 2 frames ahead is clamped to 2");
    check(h::Ahead(-5,0,P)==0&&h::Ahead(1000,-7,P)==0,"negative phase/lead treated as 0");
    check(h::Ahead(5000000,5000000,0)==0,"unknown period -> 0 (no prediction)");
    check(h::Target(1000,1,P)==1000+P&&h::Target(1000,0,P)==1000,"target = published head time + ahead * period");
    // the 75-on-90 Hz pattern: phase sweeps through the period, lead constant 6 ms -> ahead flips exactly when the
    // camera would cross the next publish, so the hand target always equals the camera's head target.
    int mism=0;for(int i=0;i<200;++i){const std::int64_t phase=(i*13333333LL)%P;const std::int64_t lead=6000000;
        const std::int64_t cameraAt=phase+lead;const int cameraFrames=int(cameraAt/P);if(h::Ahead(phase,lead,P)!=cameraFrames)++mism;}
    check(mism==0,"75 fps on 90 Hz: predicted frame == camera frame for 200 frames with a stable lead");
    std::int64_t l=h::UpdateLead(0,false,6000000);check(l==6000000,"first lead sample seeds the EMA");
    l=h::UpdateLead(l,true,16000000);check(l==7000000,"EMA alpha 0.1");
    check(h::UpdateLead(l,true,-1)==l&&h::UpdateLead(l,true,60000000)==l,"samples outside 0..50 ms ignored");
    // Camera XR identity is observational, not the native gameplay frame identity.
    static thread_local std::uint64_t nativeFrame=17;
    h::nativeFrame.store(+[]() noexcept -> std::uint64_t {return nativeFrame;});
    static int calls=0;h::locate.store(+[](h::Sample& s) noexcept {
        ++calls;s.ok=true;s.valid[0]=s.valid[1]=s.tracked[0]=s.tracked[1]=true;
        s.headValid=s.headTracked=true;s.head.p[0]=10;s.hand[0].p[0]=20;s.hand[1].p[0]=30;
        s.sourceEpoch=9;s.tick=100;s.sequence=200+calls;s.target=5000+calls;s.readXrNow=100000;s.ahead=1;return true;
    });
    h::Sample a{},b{};check(h::Current(a)&&h::Current(b)&&calls==1&&a.target==b.target,"one locate in proven native frame");
    h::NoteCamera(a.target,100000+6000000,P,777);h::NoteCamera(a.target+P,100000+7000000,P,777);
    check(h::matched.load()==1&&h::early.load()==0&&h::late.load()==0,"camera receipt counted once");
    check(h::leadNs.load()==6000000,"successful camera note teaches lead");
    check(h::Current(b)&&calls==1&&b.target==a.target,"camera note does not renew same-frame family");
    ++nativeFrame;check(h::Current(b)&&calls==2&&b.nativeFrame==nativeFrame,"next native frame renews independently of XR camera ID");
    h::NoteCamera(b.target,100000+6000000,P,777);
    check(h::lastCameraFrame.load()==777&&h::cameraNotes.load()==2,"same XR ID can cover separate native frames");
    h::cache.steadyNs=1;check(!h::Current(b)&&calls==2,"expiry falls back without resampling same frame");
    nativeFrame=0;check(!h::Current(b)&&calls==2,"unassociated caller retains ordinary family");
    nativeFrame=19;check(h::Current(b)&&calls==3,"valid next native scope can locate again");
    struct P3{float x=0,y=0,z=0;};struct Q{float x=0,y=0,z=0,w=1;};
    struct Snapshot{
        std::uint64_t source_epoch=9,input_tick=70,sequence=0,tick=0,timeNs=0,head_good_tick=60,left_good_tick=61,right_good_tick=62;
        std::int64_t poseTime=0;P3 head{},left{},right{};Q hq{},lq{},rq{};
        bool head_valid=true,left_valid=true,right_valid=false,head_held=false,left_held=true,right_held=false,allowed=false;
        float left_grip=.4f,right_trigger=.8f;
    } out;
    check(h::ApplyCompleteFamily(out,b),"complete family accepted");
    check(out.head.x==10&&out.left.x==20&&out.right.x==30&&out.poseTime==b.target,"head and both hands share target");
    check(out.sequence==b.sequence&&out.tick==b.tick&&out.timeNs==b.steadyNs,"pose provenance follows family");
    check(out.input_tick==70&&out.left_grip==.4f&&out.right_trigger==.8f&&!out.allowed,"input permission and buttons not renewed");
    check(out.left_held&&!out.right_valid&&out.head_good_tick==60&&out.left_good_tick==61&&out.right_good_tick==62,"held/valid/grace not promoted");
    const auto good=b;b.sourceEpoch=8;check(!h::ApplyCompleteFamily(out,b),"epoch mismatch rejected");b=good;
    b.headTracked=false;check(!h::ApplyCompleteFamily(out,b),"untracked late head retains ordinary family");b=good;
    b.tracked[0]=false;check(!h::ApplyCompleteFamily(out,b)&&out.left_valid,"estimated hand keeps entire ordinary family without tracking veto");b=good;
    b.hand[1].q[3]=0;check(!h::ApplyCompleteFamily(out,b),"invalid late quaternion rejected");
    h::enabled.store(false);h::Sample c{};check(!h::Current(c),"VrHandSameFrameHJ1=false -> family hands");
    std::printf("hand_same_frame_hj1 %d/%d\n",total-failed,total);return failed?1:0;
}
