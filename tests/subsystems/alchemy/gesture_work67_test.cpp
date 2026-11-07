#ifndef KCD2VR_GESTURE_WORK_HEADER_D62
#define KCD2VR_GESTURE_WORK_HEADER_D62 "../../../src/subsystems/alchemy/gesture_work67.h"
#endif
#include KCD2VR_GESTURE_WORK_HEADER_D62
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <initializer_list>
namespace ga=kcd2vr::subsystems::alchemy::gesture67;
static unsigned checks=0;
static void check(bool ok,const char* text){++checks;if(!ok){std::fprintf(stderr,"FAIL %u %s\n",checks,text);std::exit(1);}}
struct Frame {
    bool allowed=true,headValid=true;std::uint64_t tick=1000,sequence=1,timeNs=1000000000,poseEpoch=10,sourceEpoch=20;
    float head[3]{};
    struct Hand {bool valid=true;float position[3]{},rotation[4]{0,0,0,1},grip=.8f;} hand[2];
};
struct Rig {
    ga::Controller controller;ga::Input in;Frame frame;
    Rig(unsigned kind=ga::Resource){
        in.enabled=true;in.work={100,200,300,7,1000,41,kind,1,true,false};
        for(int s=0;s<2;++s)in.grip[s]={true,100,200,7,1000,10,20,std::uint32_t(500+s),s,s?14:3};
        in.pot={true,100,7,1000,10,20,600,6,{0,0,0},.20f};
        in.mortar={true,100,7,1000,10,20,501,2,{0,0,0},.20f};
        if(kind==ga::Mortar)in.grip[1].type=2;
        if(kind==ga::Bellows){in.grip[0]=in.grip[1]={};
            in.tableContact={true,true,100,200,7,1000,10,20,518,1,18,18,{0,0,0}};}
    }
    void advance(unsigned ms=16){
        frame.tick+=ms;frame.timeNs+=std::uint64_t(ms)*1000000;frame.sequence++;
        in.work.tick=frame.tick;
        for(auto& g:in.grip){g.tick=frame.tick;g.poseEpoch=frame.poseEpoch;g.sourceEpoch=frame.sourceEpoch;}
        for(auto* g:{&in.pot,&in.mortar}){g->tick=frame.tick;g->poseEpoch=frame.poseEpoch;g->sourceEpoch=frame.sourceEpoch;}
        in.tableContact.tick=frame.tick;
    }
    ga::Output step(){for(int s=0;s<2;++s)in.palm[s]={true,{frame.hand[s].position[0],-frame.hand[s].position[2],frame.hand[s].position[1]},frame.tick,frame.poseEpoch,frame.sourceEpoch};
        return controller.step(in,frame,frame.tick);}
    ga::Output next(){advance();return step();}
    void tilt(float angle){frame.hand[1].rotation[0]=std::sin(angle*.5f);frame.hand[1].rotation[1]=frame.hand[1].rotation[2]=0;frame.hand[1].rotation[3]=std::cos(angle*.5f);}
};
#ifndef KCD2VR_GESTURE_WORK67_FIXTURE_ONLY_D62
int main(){
    {Rig r;auto a=r.step();check(a.enabled&&a.clockStop&&a.totalSeconds==0,"acquire real held resource stopped");
        r.tilt(.9f);a=r.next();check(a.totalSeconds>0&&!a.clockStop&&a.status==ga::Status::Pouring,"tilt over current pot advances native work");
        const auto first=a.totalSeconds;a=r.next();check(a.totalSeconds>first,"fixed absolute tilt sustains flow");
        const auto held=a.totalSeconds;a=r.step();check(a.totalSeconds==held&&!a.clockStop&&a.status==ga::Status::Pouring,"duplicate tracking retains current motion permission without duplicating credit");
        r.tilt(0);a=r.next();check(a.totalSeconds==held&&a.clockStop,"upright stops pouring");
        r.tilt(.9f);r.frame.hand[1].position[0]=.21f;a=r.next();check(a.totalSeconds==held&&a.status==ga::Status::MoveToPot,"outside recipient volume stops work");
        r.frame.hand[1].position[0]=0;a=r.next();check(a.totalSeconds>held,"returning over pot resumes bounded credit");
    }
    {Rig r;r.step();r.frame.hand[1].rotation[1]=std::sin(.7f);r.frame.hand[1].rotation[3]=std::cos(.7f);
        auto a=r.next();check(a.totalSeconds==0&&a.status==ga::Status::TiltToPour,"local-up yaw is not pouring");}
    {Rig r;r.in.referenceRotationValid=true;r.tilt(.9f);r.step();auto a=r.next();
        check(a.totalSeconds>0&&a.status==ga::Status::Pouring,"late native Attach uses original physical admission neutral");}
    {Rig r;r.in.referenceRotationValid=true;r.in.referenceRotation[0]=std::numeric_limits<float>::quiet_NaN();
        const auto a=r.step();check(!a.enabled&&a.status==ga::Status::InvalidFrame,"invalid supplied admission reference cannot fall back silently");}
    {Rig r;r.step();r.tilt(1.f);r.in.pot.valid=false;auto a=r.next();check(a.enabled&&a.clockStop&&a.totalSeconds==0&&a.status==ga::Status::GeometryRequired,"no invented recipient");
        r.in.pot.valid=true;r.in.pot.type=2;a=r.next();check(a.totalSeconds==0,"wrong recipient type rejected");
        r.in.pot.type=6;r.in.pot.poseEpoch=999;a=r.step();check(a.totalSeconds==0,"geometry frame mismatch rejected");
        r.in.pot.poseEpoch=10;r.in.pot.tick=r.frame.tick+1;a=r.step();check(a.totalSeconds==0,"future geometry rejected");
        r.in.pot.tick=r.frame.tick-81;a=r.step();check(a.totalSeconds==0,"stale geometry rejected");
        r.in.pot.tick=r.frame.tick;r.in.pot.radius=5;a=r.step();check(a.totalSeconds==0,"unbounded recipient rejected");}
    {Rig r;r.step();r.frame.hand[1].grip=.1f;auto a=r.next();check(a.enabled&&a.clockStop&&a.releaseRequested&&a.status==ga::Status::ReleaseAwaitingNativeReturn,"early release cannot secretly apply resource");
        r.frame.hand[1].grip=.8f;r.tilt(1.f);a=r.next();check(a.releaseRequested&&a.totalSeconds==0,"squeeze cannot revive a released transaction");
        r.in.nativeReturnAuthorized=true;a=r.next();check(!a.enabled&&a.status==ga::Status::NativeReturn,"only explicit native return authorizes release cleanup");}
    {Rig r;r.step();r.tilt(1.f);auto a=r.next();const auto before=a.totalSeconds;r.in.work.phase=2;r.in.work.working=false;
        a=r.next();check(a.enabled&&a.completedHeld&&a.clockStop&&a.totalSeconds==before,"native Use phase becomes completed hold");
        a=r.next();check(a.totalSeconds==before,"completed hold never advances duplicate recipe work");
        r.frame.hand[1].grip=.1f;a=r.next();check(!a.enabled&&a.releaseRequested&&a.status==ga::Status::NativeReturn,"release after real Use permits native detach");}
    {Rig r;r.step();r.in.nativePassenger=true;auto a=r.next();check(!a.enabled&&a.status==ga::Status::NativePassenger,"native A passenger keeps original clock authority");
        r.in.nativePassenger=false;r.tilt(1.f);a=r.next();check(a.totalSeconds==0,"new lease starts from actual neutral");}
    {Rig r;r.step();r.tilt(1.f);r.frame.hand[1].valid=false;auto a=r.next();check(!a.enabled&&a.status==ga::Status::GripRequired,"lost joined tracking does not earn work");
        r.frame.hand[1].valid=true;r.frame.hand[1].rotation[0]=std::numeric_limits<float>::quiet_NaN();a=r.next();check(!a.enabled&&a.status==ga::Status::InvalidFrame,"invalid quaternion rejected");}
    {Rig r;r.step();r.tilt(1.f);r.frame.allowed=false;auto a=r.next();check(!a.enabled,"context revocation clears gesture authority");
        r.frame.allowed=true;r.frame.headValid=false;a=r.next();check(!a.enabled,"head validity required for coherent basis");}
    {Rig r;r.step();r.tilt(1.f);r.in.grip[1].parent=999;auto a=r.next();check(!a.enabled,"foreign native parent cannot borrow held grip");}
    {Rig r;r.step();r.tilt(1.f);r.frame.poseEpoch++;auto a=r.next();check(a.totalSeconds==0,"recenter starts new neutral with no accumulated credit");
        r.tilt(0);a=r.next();check(a.totalSeconds>0,"tilt measured relative to new acquisition neutral");}
    {Rig r;r.step();r.tilt(1.f);r.advance(90);auto a=r.step();check(!a.enabled&&a.status==ga::Status::InvalidFrame,"long source gap resets without catchup");}
    {Rig r;r.step();r.tilt(1.f);r.frame.sequence=0;auto a=r.step();check(!a.enabled,"zero sample sequence refused");}
    {Rig r;r.step();r.tilt(1.f);r.in.work.tick=r.frame.tick+1;auto a=r.step();check(!a.enabled,"future native work refused");}
    {Rig r;r.step();r.tilt(1.f);r.frame.sequence++;r.frame.timeNs--;auto a=r.step();check(!a.enabled,"reversed source clock refused");}
    {Rig r(ga::Mortar);r.in.work.phase=2;r.step();r.frame.hand[0].position[0]=.02f;auto a=r.next();check(a.totalSeconds>0&&a.status==ga::Status::Grinding,"relative pestle movement advances actual mortar child");
        const auto before=a.totalSeconds;r.in.work.phase=3;r.frame.hand[0].position[0]=.04f;a=r.next();check(a.totalSeconds>before,"same mortar queue-to-live phase preserves cumulative ledger");
        const auto held=a.totalSeconds;a=r.next();check(a.totalSeconds==held&&a.clockStop,"stationary pestle stops work");
        r.frame.hand[0].position[0]=.14f;r.frame.hand[1].position[0]=.10f;r.in.mortar.point[0]=.10f;a=r.next();check(std::fabs(a.totalSeconds-held)<1e-6,"moving both held tools together is not grinding");
        r.frame.hand[0].position[0]=.5f;a=r.next();check(a.totalSeconds==held,"pestle outside mortar cannot grind");}
    {Rig r(ga::Mortar);r.frame.hand[0].position[0]=.04f;r.step();
        r.frame.hand[1].rotation[2]=std::sin(.4f);r.frame.hand[1].rotation[3]=std::cos(.4f);
        r.frame.hand[0].position[0]=.04f*std::cos(.8f);r.frame.hand[0].position[1]=.04f*std::sin(.8f);
        const auto a=r.next();check(a.totalSeconds<1e-6,"common rigid rotation of mortar and pestle is not grinding");}
    {Rig r(ga::Mortar);r.step();r.frame.hand[1].grip=.1f;r.frame.hand[0].position[0]=.05f;const auto a=r.next();check(a.releaseRequested&&a.totalSeconds==0,"mortar requires both held native grips");}
    {Rig r(ga::Mortar);r.step();r.in.mortar.entity=999;r.frame.hand[0].position[0]=.05f;const auto a=r.next();check(a.totalSeconds==0&&a.status==ga::Status::GeometryRequired,"foreign mortar volume rejected");}
    {Rig r(ga::Mortar);r.in.work.working=false;r.in.work.ready=true;r.step();r.frame.hand[0].position[0]=.01f;r.next();r.frame.hand[0].position[0]=.03f;auto a=r.next();check(a.use&&a.useUntil==r.frame.tick+80&&a.totalSeconds==0,"ready gesture requests native next cycle, does not add fake progress");
        a=r.next();check(a.use,"ready pulse survives asynchronous native frame");r.advance(80);a=r.step();check(!a.use,"ready request finite, no held retry");}
    {Rig r(ga::Bellows);r.step();r.frame.hand[1].position[1]=-.02f;auto a=r.next();check(a.totalSeconds>0&&a.status==ga::Status::Pumping,"actual left native bellows contact earns clock credit without a fake held attachment");
        const auto before=a.totalSeconds;r.frame.hand[1].position[0]=.03f;a=r.next();check(a.totalSeconds==before,"bellows lateral motion earns no pump credit");
        r.frame.hand[1].grip=0;a=r.next();check(a.status==ga::Status::NativeReturn&&!a.enabled&&a.releaseRequested,"bellows release resumes genuine native pump/return, not resource abort");}
    {Rig r(ga::Bellows);r.in.work.working=false;r.in.work.ready=true;r.step();r.frame.hand[1].position[1]=-.03f;auto a=r.next();check(a.use&&a.totalSeconds==0,"ready bellows stroke requests real work child only");}
    {Rig r(ga::Bellows);r.step();r.frame.hand[1].position[1]=-.02f;auto a=r.next();const auto before=a.totalSeconds;
        r.in.tableContact.geometryValid=false;r.frame.hand[1].position[1]=-.04f;a=r.next();
        check(a.enabled&&a.clockStop&&a.totalSeconds==before&&a.status==ga::Status::GeometryRequired,"missing native bellows geometry pauses and preserves consumed ledger");
        r.in.tableContact.geometryValid=true;a=r.next();check(a.totalSeconds==before,"returning contact starts a new movement baseline, no catchup credit");
        r.frame.hand[1].position[1]=-.06f;a=r.next();check(a.totalSeconds>before,"fresh pumping can resume without replaying old work");}
    {Rig r(ga::Bellows);r.step();r.frame.hand[1].position[1]=r.frame.head[1]=-.03f;auto a=r.next();
        check(a.totalSeconds==0,"whole body/head common vertical motion is not bellows pumping");}
    {Rig r(ga::Bellows);r.step();r.frame.hand[1].position[1]=-.2f;auto a=r.next();check(a.totalSeconds==0&&a.status==ga::Status::GeometryRequired,"hand outside actual10cm bellows surface volume earns no work");}
    {Rig r(ga::Bellows);r.frame.hand[1].grip=0;auto a=r.step();check(a.totalSeconds==0&&!a.enabled&&a.status==ga::Status::GripRequired,"bellows contact without physical Grip cannot work");}
    for(unsigned fault=0;fault<11;++fault){Rig r(ga::Bellows);
        switch(fault){case 0:r.in.tableContact.valid=false;break;case 1:r.in.tableContact.parent++;break;
        case 2:r.in.tableContact.instance++;break;case 3:r.in.tableContact.lifetime++;break;
        case 4:r.in.tableContact.poseEpoch++;break;case 5:r.in.tableContact.sourceEpoch++;break;
        case 6:r.in.tableContact.tick=919;break;case 7:r.in.tableContact.entity=0;break;
        case 8:r.in.tableContact.side=0;break;case 9:r.in.tableContact.type=17;break;case 10:r.in.tableContact.nativeHandType=21;break;}
        const auto a=r.step();check(!a.enabled&&a.totalSeconds==0&&a.status==ga::Status::GripRequired,"foreign/stale/unowned Bellows contact cannot borrow real work");}
    std::printf("gesture_work67: %u checks PASS\n",checks);
}
#endif
