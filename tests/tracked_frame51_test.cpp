#include "vrik/tracked_frame51.h"
#include <cstdio>
#include <limits>

namespace f=kcd2vr::tracked_frame51;namespace m=kcd2vr::pose_core;
static int checks=0,failures=0;
static void ck(bool value,const char* label){++checks;if(!value)++failures;std::printf("%s %s\n",value?"PASS":"FAIL",label);}
static bool near(m::Vec3 a,m::Vec3 b,double e=1e-8){return m::length(a-b)<e;}
static f::Input input(){f::Input i{};i.stamp={3,1,1000,0x100};i.control=1;i.timeNs=1000000000;
    i.nativeHead=i.restHead=i.calibratedHead={0,0,1.7};i.nativePelvis=i.restPelvis={0,0,.9};i.allowed=i.eyeNeutral=true;return i;}
static void next(f::Input& i,unsigned ms=10){++i.stamp.sample;i.stamp.tick+=ms;i.timeNs+=std::uint64_t(ms)*1000000;}
int main(){
    f::State s{};auto i=input();f::Frame a{};
    ck(f::advance(s,i,a)&&near(a.anchor,i.calibratedHead),"standing rest frame starts without a sampled native posture");
    next(i);i.nativeHead={.12,.2,1.46};f::Frame slump{};
    ck(f::advance(s,i,slump)&&near(slump.anchor,a.anchor),"authored upper-body bow/attack slump cannot sink or push tracked frame");
    next(i);i.nativePelvis={.025,.04,.55};f::Frame crouch{};
    ck(f::advance(s,i,crouch)&&near(crouch.anchor,{0,0,1.35}),"explicit native height moves Z while authored pelvis XY cannot move tracked anatomical basis");
    ck(near(crouch.bodyOffset,{-.025,-.04,0}),"same-body horizontal compensation is immediate without tracking smoothing");
    next(i);i.nativePelvis.z=.9;f::Frame standing{};
    ck(f::advance(s,i,standing)&&std::fabs(standing.anchor.z-1.7)<1e-8,"standing restores immediately without a height threshold plateau");
    // First sample while already crouched must not become another standing calibration.
    s={};i=input();i.nativeHead.z=1.15;i.nativePelvis.z=.4;
    ck(f::advance(s,i,a)&&std::fabs(a.anchor.z-1.2)<1e-8,"crouched first sample keeps validated standing anatomy");
    s={};i=input();i.smooth=true;f::advance(s,i,a);next(i);i.nativePelvis.z=.84;f::advance(s,i,slump);
    ck(slump.anchor.z>1.69&&slump.anchor.z<1.7&&near(slump.rawAnchor+slump.bodyOffset,slump.anchor),
       "comfort smooths shared native frame and publishes identical pelvis compensation");
    const auto before=s;f::Frame duplicate{};
    ck(f::advance(s,i,duplicate)&&near(duplicate.anchor,slump.anchor)&&s.timeNs==before.timeNs,
       "repeated cull/eye/pose consumer cannot advance filter twice");
    {auto changed=i;auto staged=s;changed.nativeHead.y+=.18;changed.nativeHead.z-=.24;f::Frame updated{};
     ck(f::advance(staged,changed,updated)&&near(updated.anchor,slump.anchor)&&near(updated.nativeHead,changed.nativeHead)&&
        staged.timeNs==s.timeNs,"same-sample native head animation updates its witness without snapping or advancing smoothing");}
    {auto changed=i;auto staged=s;changed.nativePelvis.z-=.02;f::Frame updated{};
     ck(f::advance(staged,changed,updated)&&near(updated.anchor,slump.anchor)&&near(updated.rawAnchor+updated.bodyOffset,updated.anchor),
        "same-sample current pelvis update keeps zero-dt shared compensation coherent");}
    const m::Vec3 hmd{.03,.04,-.12},hand{.25,.22,-.35};
    ck(near((slump.anchor+hand)-(slump.anchor+hmd),hand-hmd),"smoothed frame cancels exactly from hand versus headset relationship");
    i.smooth=false;++i.control;next(i);f::advance(s,i,a);
    ck(near(a.anchor,a.rawAnchor),"turning smooth movement off retires filter immediately");
    i.smooth=true;++i.control;next(i);f::advance(s,i,a);
    ck(near(a.anchor,a.rawAnchor),"turning smooth movement on seeds current stance without borrowed lag");
    next(i);i.nativePelvis.z=.55;f::advance(s,i,a);
    for(int n=0;n<120;++n){next(i);f::advance(s,i,a);}
    ck(std::fabs(a.anchor.z-1.35)<.006,"comfort settles to real crouch rather than fixed calibrated eye height");
    // C55 draw comfort with the existing strict completed-frame ownership.
    s={};i=input();i.bow=true;i.bowKey=7;i.bowGeneration=8;i.bowRequest=9;f::advance(s,i,a);
    next(i,20);i.drawing=true;i.nativePelvis={.1,.2,.7};f::advance(s,i,a);
    ck(near(a.anchor,{0,0,1.7}),"first draw immediately pins the shared XYZ foundation before native aim can sway it");
    for(int n=0;n<10;++n){next(i,20);f::advance(s,i,a);}
    ck(near(a.anchor,{0,0,1.7}),"sustained bow draw excludes authored aim push and pelvis lowering from all shared goals");
    {auto changed=i;auto staged=s;++changed.bowGeneration;f::Frame fresh{};
     ck(f::advance(staged,changed,fresh)&&near(fresh.anchor,{changed.restHead.x,changed.restHead.y,fresh.rawAnchor.z})&&!staged.bowReady,
        "same XR sample with a changed bow generation cannot reuse old bow correction");}
    {auto changed=i;auto staged=s;changed.drawing=false;f::Frame fresh{};
     ck(f::advance(staged,changed,fresh)&&near(fresh.anchor,a.anchor)&&staged.bowWeight==1,
        "same XR sample release retains free-view foundation without second temporal motion credit");}
    next(i);i.nativePelvis.z=.65;f::advance(s,i,a);
    ck(std::fabs(a.anchor.z-1.7)<1e-8,"native-button-only crouch cannot reintroduce authored view-height movement during exact draw");
    next(i);i.drawing=false;f::advance(s,i,a);
    ck(near(a.anchor,{0,0,1.7}),"release never blends native bow sway back into the free-view foundation");
    for(int phase=0;phase<6;++phase){next(i);i.drawing=(phase%2)==0;
        i.nativePelvis={phase%2?.12:-.12,phase%2?.15:-.15,.75};f::advance(s,i,a);
        ck(near(a.anchor,{0,0,1.7}),"idle/quiver/nock/draw/release changes cannot swing the bow free-view foundation");}
    next(i);++i.bowGeneration;i.drawing=true;f::advance(s,i,a);
    ck(near(a.anchor,{i.restHead.x,i.restHead.y,a.rawAnchor.z})&&!s.bowReady,"replacement bow cannot borrow prior idle eye");
    for(unsigned kind=0;kind<8;++kind){s={};i=input();i.smooth=true;f::advance(s,i,a);next(i);i.nativePelvis.z-=.1;f::advance(s,i,a);
        next(i);switch(kind){case 0:++i.stamp.epoch;break;case 1:i.stamp.actor=0x200;break;case 2:++i.control;break;
        case 3:i.scale=.8;break;case 4:i.stamp.tick+=81;i.timeNs+=81000000;break;
        case 5:i.calibratedHead.z+=.03;break;case 6:i.restHead.z+=.03;break;case 7:i.smooth=false;break;}
        ck(f::advance(s,i,a)&&near(a.anchor,a.rawAnchor),"changed body/mode/scale/origin/anatomy or stale gap resets temporal state");}
    for(unsigned kind=0;kind<6;++kind){s={};i=input();f::advance(s,i,a);next(i);
        switch(kind){case 0:i.allowed=false;break;case 1:i.nativeHead.x=std::numeric_limits<double>::quiet_NaN();break;
        case 2:i.scale=0;break;case 3:i.restHead=i.restPelvis;break;case 4:i.stamp.sample=0;break;case 5:i.stamp.tick=999;break;}
        ck(!f::advance(s,i,a)&&!s.ready&&!a.valid,"unqualified or regressed input cannot retain a camera correction");}
    // A first draw without an equipped-idle sample may use only an immediately
    // preceding same-body positively neutral ordinary frame, not another draw.
    s={};i=input();f::advance(s,i,a);next(i);i.bow=i.drawing=true;i.bowKey=7;i.bowGeneration=8;i.bowRequest=9;
    i.nativePelvis={.08,.11,.77};f::advance(s,i,a);
    ck(near(a.anchor,{0,0,1.7})&&s.bowReady,"first equip-and-draw reuses this body's adjacent ordinary neutral foundation");
    s={};i=input();i.bow=i.drawing=true;i.bowKey=7;i.bowGeneration=8;i.bowRequest=9;i.eyeNeutral=false;
    f::advance(s,i,a);ck(!s.bowReady&&near(a.anchor,{i.restHead.x,i.restHead.y,a.rawAnchor.z}),"unknown first draw cannot invent a neutral bow foundation");
    s={};i=input();f::advance(s,i,a);
    i.bow=i.drawing=true;i.bowKey=7;i.bowGeneration=8;i.bowRequest=9;i.eyeNeutral=false;
    i.nativeHead={.12,.18,1.45};i.nativePelvis={.08,.11,.77};
    ck(f::advance(s,i,a)&&near(a.anchor,{0,0,1.7})&&s.bowReady&&s.bowWeight==1,
       "same-XR-stamp neutral ordinary to first equip-and-draw retains positive neutral anatomy without temporal credit");
    // Camera consumes the exact completed frame, current model transform and
    // independently learned native anatomical eye offset; no HMD input here.
    s={};i=input();f::advance(s,i,a);a.root={10,20,5};f::EyeState eye{};m::Vec3 out{};
    {auto unknown=a;unknown.neutral=false;f::EyeState first{};
     ck(!f::eye(first,unknown,unknown.stamp,1,1000,{10,20,6.756},out)&&!first.ready,
        "unknown equipment/neutral witness cannot train the first eye offset");}
    ck(f::eye(eye,a,a.stamp,1,1000,{10,20,6.756},out)&&near(out,{10,20,6.756}),"neutral camera offset is learned before one HMD/IPD composition");
    next(i);i.nativeHead={.2,.25,1.45};i.nativePelvis.z=.8;f::advance(s,i,a);a.root={12,23,5.1};
    const double q=std::sqrt(.5);a.rotation={0,0,q,q};
    ck(f::eye(eye,a,a.stamp,1,i.stamp.tick,{12.1,23.2,6.60},out)&&near(out,{12,23,6.756}),
       "camera uses current root/rotation/stance while authored native eye sway stays outside tracking");
    ck(near(out+m::Vec3{.03,.04,.05},m::Vec3{12.03,23.04,6.806}),"HMD translation remains additive once outside the frame adapter");
    const auto stamp=a.stamp;a.stamp.sample++;
    ck(!f::eye(eye,a,stamp,1,i.stamp.tick,{12,23,6.7},out),"torn body/frame publications cannot acquire camera authority");
    a.stamp=stamp;ck(!f::eye(eye,a,stamp,1,i.stamp.tick+81,{12,23,6.7},out),"camera keeps existing 80 ms completed-pose deadline");
    ck(!f::eye(eye,a,stamp,2,i.stamp.tick,{12,23,6.7},out),"camera rejects the old control generation immediately");
    // Neutral yaw belongs to the current character-world root, never the bow
    // AimPose or a torso bone. No HMD quaternion is accepted by this adapter.
    s={};i=input();f::advance(s,i,a);a.root={10,20,5};eye={};m::Vec3 heading{};
    ck(f::eye(eye,a,a.stamp,1,i.stamp.tick,{10,20,6.756},out,{0,1,0},&heading)&&
       eye.headingReady&&near(heading,{}),"ordinary neutral camera trains local yaw without changing ordinary camera heading");
    next(i);i.bow=i.drawing=true;i.bowKey=7;i.bowGeneration=8;i.bowRequest=9;i.eyeNeutral=false;
    f::advance(s,i,a);a.root={10,20,5};
    ck(f::eye(eye,a,a.stamp,1,i.stamp.tick,{10.03,20,6.72},out,{.12,.9928,.03},&heading)&&near(heading,{0,1,0}),
       "authored bow camera yaw/pitch sway cannot rotate the qualified neutral heading");
    a.root={11,22,5.2};a.rotation={0,0,q,q};
    ck(f::eye(eye,a,a.stamp,1,i.stamp.tick,{11,22,6.9},out,{-.9928,.12,0},&heading)&&
       near(heading,{-1,0,0})&&near(out,{11,22,6.956}),
       "current entity-world turn and stairs/travel remain live while native bow aim yaw is excluded");
    const auto unchanged=eye;auto blocked=a;blocked.valid=false;
    ck(!f::eye(eye,blocked,blocked.stamp,1,i.stamp.tick,{11,22,6.9},out,{0,1,0},&heading)&&near(heading,{})&&
       near(eye.forward,unchanged.forward),"unavailable current frame grants neither bow heading nor a new measurement");
    ++a.stamp.epoch;a.neutral=false;
    ck(!f::eye(eye,a,a.stamp,1,i.stamp.tick,{11,22,6.9},out,{0,1,0},&heading)&&!eye.headingReady,
       "replacement epoch cannot borrow previous neutral heading while drawn");
    // The acquisition's exact admission metadata is still current even though
    // it no longer chooses a different spatial XY coordinate system.
    for(bool smooth:{false,true}){
        s={};i=input();i.smooth=smooth;i.hmdHeight=true;f::advance(s,i,a);
        const auto neutral=a.anchor;
        i.acquisitionSerial=17;i.acquisitionAction=0x2000;i.acquisitionScope=0x3000;
        i.acquisitionToken=5;i.acquisitionAnchor={.15,-.20,1.7};i.nativePelvis={.2,-.3,.85};
        ck(f::advance(s,i,a)&&near(a.anchor,neutral),"same-source installed acquisition keeps anatomical XY");
        ck(a.acquisitionSerial==17&&a.acquisitionAction==0x2000&&a.acquisitionScope==0x3000&&a.acquisitionToken==5,
           "same-source installed acquisition publishes exact CURRENT authority metadata");
        i.acquisitionSerial=i.acquisitionAction=i.acquisitionScope=0;i.acquisitionToken=0;
        ck(f::advance(s,i,a)&&near(a.anchor,neutral),"same-source acquisition retirement keeps anatomical XY");
        ck(!a.acquisitionSerial&&!a.acquisitionAction&&!a.acquisitionScope&&!a.acquisitionToken,
           "same-source acquisition retirement cannot reuse stale cached authority metadata");
        for(int n=0;n<20;++n){next(i);i.nativePelvis={.04*n,-.03*n,.85};f::advance(s,i,a);}
        ck(a.valid&&near(a.anchor,neutral),"authored local XY travel beyond45cm does not become a new correction veto");
        next(i);i.nativePelvis=i.restPelvis;
        ck(f::advance(s,i,a)&&near(a.anchor,neutral)&&near(a.bodyOffset,{}),
           "native pelvis return leaves neither delayed jump nor permanent corrective offset");
        next(i);i.allowed=false;
        ck(!f::advance(s,i,a)&&!a.valid&&!s.ready,"anatomical XY reference never retains expired pose admission");
    }
    s={};i=input();i.smooth=true;i.nativePelvis.x=-std::numeric_limits<double>::max()*.75;
    ck(f::advance(s,i,a)&&m::finite(s.filtered),"finite native filter state is preserved before XY presentation selection");
    next(i);i.nativePelvis.x=std::numeric_limits<double>::max()*.75;
    ck(!f::advance(s,i,a)&&!s.ready&&!a.valid,"XY presentation cannot conceal overflowing native filter state");
    // R14: independent scalar preserves the baseline anatomy and bow donor.
    for(bool priorSmooth:{false,true}){
        s={};i=input();i.smooth=priorSmooth;i.nativeAnchorPolicyR14=7;f::advance(s,i,a);
        next(i);i.nativePelvis.z-=.04;f::advance(s,i,a);
        const double expected=1.7-.04*(1-std::exp(-.01/.100));
        ck(std::fabs(a.anchor.z-expected)<1e-10,"rung5 exactly100ms native-Z response without baseline filter stacking");
        ck(a.smooth&&near(a.anchor-a.bodyOffset,a.rawAnchor),"rung5 identical compensated body-camera frame");
        ck(near((a.anchor+hand)-(a.anchor+hmd),hand-hmd),"rung5 live hand-versus-HMD displacement unfiltered");
        const auto live=s;auto staged=s;auto stagedInput=i;next(stagedInput);stagedInput.nativePelvis.z-=.02;
        f::Frame stagedFrame{};f::advance(staged,stagedInput,stagedFrame);
        ck(s.nativeAnchorZ100==live.nativeAnchorZ100&&s.timeNs==live.timeNs,"uncommitted staged pose cannot advance live anchor history");
        auto repeat=i;repeat.nativeHead.x+=.04;repeat.nativePelvis.z-=.02;f::Frame repeated{};
        ck(f::advance(s,repeat,repeated)&&std::fabs(repeated.anchor.z-a.anchor.z)<1e-10,"same-source native pose cannot advance rung5 twice");
        i=repeat;next(i);++i.nativeAnchorPolicyR14;f::advance(s,i,a);
        ck(std::fabs(a.anchor.z-a.rawAnchor.z)<1e-10,"rung5 policy revision seeds current native height");
    }
    for(unsigned owner=0;owner<4;++owner){
        s={};i=input();i.smooth=true;
        if(owner==0){i.bow=true;i.bowKey=7;i.bowGeneration=8;i.bowRequest=9;}
        if(owner==1){i.acquisitionSerial=17;i.acquisitionAction=2;i.acquisitionScope=3;i.acquisitionToken=5;i.acquisitionAnchor={0,0,1.65};}
        if(owner==2){i.workstationKindH51=1;i.workstationModeEpochH51=7;i.workstationLifetimeH51=8;i.workstationInstanceH51=9;i.hmdHeight=true;}
        if(owner==3)i.hmdHeight=true;
        f::advance(s,i,a);next(i);i.nativePelvis={.03,.04,.82};if(owner==0)i.drawing=true;f::advance(s,i,a);
        const auto baseline=s;const auto anchor=a.anchor;const auto body=a.bodyOffset;
        for(auto policy:{7ull,8ull,0ull,9ull}){i.nativeAnchorPolicyR14=policy;f::advance(s,i,a);
            ck(near(a.anchor,anchor)&&near(a.bodyOffset,body)&&near(s.filtered,baseline.filtered)&&
               near(s.bowIdle,baseline.bowIdle)&&s.bowReady==baseline.bowReady&&s.bowWeight==baseline.bowWeight,
               "rung5 toggle preserves protected frame and bow donor even at duplicate source");
            ck(!s.nativeAnchorReady100,"protected owner cannot activate native-height experiment");}
    }
    for(unsigned reason=0;reason<9;++reason){
        s={};i=input();i.nativeAnchorPolicyR14=7;f::advance(s,i,a);next(i);i.nativePelvis.z-=.04;f::advance(s,i,a);next(i);
        switch(reason){case 0:++i.stamp.epoch;break;case 1:i.stamp.actor+=8;break;case 2:++i.control;break;
        case 3:i.scale=.9;break;case 4:i.calibratedHead.z+=.01;break;case 5:i.restHead.z+=.01;break;
        case 6:i.stamp.tick+=81;i.timeNs+=81000000;break;case 7:++i.nativeAnchorPolicyR14;break;
        case 8:i.hmdHeight=true;break;}
        ck(f::advance(s,i,a)&&std::fabs(a.anchor.z-(i.hmdHeight?i.restHead.z:a.rawAnchor.z))<1e-10,
           "rung5 lifecycle transition reseeds rather than retaining old animation lag");
    }
    s={};i=input();i.nativeAnchorPolicyR14=7;f::advance(s,i,a);a.root={10,20,5};eye={};
    ck(f::eye(eye,a,a.stamp,i.control,i.stamp.tick,{10,20,6.756},out),"rung5 camera learns existing neutral anatomy");
    next(i);i.nativePelvis.z-=.04;f::advance(s,i,a);a.root={12,23,5.2};a.rotation={0,0,q,q};
    ck(f::eye(eye,a,a.stamp,i.control,i.stamp.tick,{12,23,6.916},out)&&near(out,{12,23,5.2+a.anchor.z+.056}),
       "rung5 camera retains current entity translation and turn");
    const auto currentEye=out;a.root.z+=.25;
    ck(f::eye(eye,a,a.stamp,i.control,i.stamp.tick,{12,23,7.166},out)&&std::fabs(out.z-currentEye.z-.25)<1e-10,
       "world stairs/root travel passes immediately in same source frame");
    i.allowed=false;ck(!f::advance(s,i,a)&&!s.nativeAnchorReady100&&!a.valid,
       "native-theater-unqualified frame cannot retain rung5 state");
    std::printf("tracked_frame51=%d/%d; native/headset=NOT_EXECUTED\n",checks-failures,checks);return failures?1:0;
}
