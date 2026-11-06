// Actual production tracking packet and finger writer. The default full-TU
// variant retains Windows requirements; portable literal extraction uses the
// explicitly controlled fixture leaves below. Neither executes a headset.
#ifdef KCD2VR_DIALOGUE_PORTABLE_D78
#include "dialogue_fingers_fixture_d78.h"
#else
#ifndef PRODUCTION_FRIK_SOURCE_D78
#define PRODUCTION_FRIK_SOURCE_D78 "../src/frik_pose_probe.cpp"
#endif
#include PRODUCTION_FRIK_SOURCE_D78
#endif
#include "core/trigger_ownership.h"
#include <cstdio>
#include <limits>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::frik_pose_probe;
namespace frik_pose_probe=kcd2vr::frik_pose_probe;
static unsigned checksD78=0,failuresD78=0;
static void ckD78(bool v,const char* message){++checksD78;if(!v){++failuresD78;std::printf("FAIL %s\n",message);}}
struct ControllerD78 {TrackingPose pose{};float grip=1,trigger=1;bool active=true;};
struct HostD78 {
    kcd2vr::core::TriggerOwnership gameplay_trigger_[2]{};
    ControllerD78 lc{},rc{},routeL{},routeR{};
    TrackingPose hp{},lp{},rp{};
    kcd2vr::tracking59c6::Stamp source59c6{};
    bool xr_head_valid=true,dialogueBody47c1=true,vrik_gameplay_allowed=false;
    bool trigger_ui_after=true,trigger_activation=false;
    unsigned stampMaskD78=7;bool futureSourceD78=false;
    void publish(){
        hp.qw=lp.qw=rp.qw=1;hp.py=1.7f;lp.px=-.25f;rp.px=.25f;
        source59c6.tick=hostTickCount64;source59c6.ns=kcd2vr::tracking59c6::NowNs();
        source59c6.epoch=kcd2vr::tracking59c6::epoch.load();
        if(!(stampMaskD78&1))source59c6.tick=0;
        if(!(stampMaskD78&2))source59c6.ns=0;
        if(!(stampMaskD78&4))source59c6.epoch=0;
        if(futureSourceD78)source59c6.ns+=1000000000ull;
        const auto& trigger_l=lc;const auto& trigger_r=rc;
        // Literal mod.cpp input-owner, publisher and mapped-route statements.
#include "generated_dialogue_host_d78.inc"
    }
};
static bool actualWriterD78(CoherentState6689& s,const TrackingSnapshot& ts,int side,
                            bool nativeFingerCapture=false,bool objectFingers=false){
    CoherentTransaction6689 tx{};tx.epoch=g_pose_epoch6689.load();
    bool freeContact164R1[2]{},stationContactD75=false;
    HandContact6695 stagedContact[2]{};bool combatFistH34=false;
    // Literal free-writer owner dispatch; rejection returns false in this fixture.
#include "generated_dialogue_writer_d78.inc"
    ckD78(!combatFistH34,"visual dialogue curl never qualifies a gameplay fist");
    return true;
}
static void initD78(CoherentState6689& s){
    s.count=50;s.rig.hand_r=0;s.rig.hand_l=1;
    for(int i=0;i<50;++i){s.parent[i]=-1;s.restLocal[i].q[3]=s.local[i].q[3]=s.rest[i].q[3]=1;}
    for(int side=0;side<2;++side){s.fingerMap[side]=s.open[side].ready=s.closed[side].ready=true;
        for(int digit=0;digit<5;++digit){const int id=2+side*5+digit;
            s.fingers[side][digit].count=1;s.fingers[side][digit].id[0]=id;s.parent[id]=side;
            s.restLocal[id].t[0]=.03f;s.open[side].local[digit].q[3]=1;
            QToQuatT(s.closed[side].local[digit],QP(pc::from_rotation_vector({0,.8,0})));
            for(int parameter=0;parameter<4;++parameter)kcd2vr::finger40::set(side,digit,parameter,kcd2vr::finger40::defaults59[side][digit][parameter]);
        }
    }
    g_open_bind_enabled105=false;
}
static double indexAngleD78(const CoherentState6689& s,int side){
    return pc::angle(PQ(QFromQuatT(s.local[s.fingers[side][1].id[0]])),pc::Quat{});
}
int main(){
    hostTickCount64=10000;kcd2vr::tracking59c6::epoch=9;kcd2vr::tracking59c6::enabled=false;
    g_pose_epoch6689=7;g_dialogue_body178r9=true;g_ctx_dialogue=true;
    kcd2vr::reading_pose32::blocked=true;
    g_census_mode=3;g_cal_state=2;g_mode=0;
    static CoherentState6689 s{};initD78(s);HostD78 host;
    auto read=[&](){TrackingSnapshot t{};ckD78(ReadTrackingSnapshot(t),"actual tracking packet is readable");return t;};
    auto step=[&](){++hostTickCount64;host.publish();return read();};
    auto t=step();
    for(int side=0;side<2;++side){ckD78(actualWriterD78(s,t,side),"actual free-finger writer accepts dialogue packet");
        ckD78(indexAngleD78(s,side)>.79,"D78R2 raw dialogue trigger closes the actual index joint");}
    ckD78(!t.left_trigger&&!t.right_trigger&&!g_left_trigger.load()&&!g_right_trigger.load(),"gameplay trigger fields and atomics remain zero");
    ckD78(!host.routeL.trigger&&!host.routeR.trigger,"actual mapped gameplay/chooser route remains zero");
    ckD78(!g_gameplay_allowed.load()&&!kcd2vr::haptics101::allowed.load(),"visual input cannot enable gameplay or haptics");
    const auto before=s.local;
    ckD78(actualWriterD78(s,t,0,true,false)&&std::memcmp(before.data(),s.local.data(),sizeof(before))==0,"native capture retains all finger locals");
    ckD78(actualWriterD78(s,t,0,false,true)&&std::memcmp(before.data(),s.local.data(),sizeof(before))==0,"native object owner retains all finger locals");
    // Current context must win even before a replacement input packet arrives.
    g_ctx_dialogue=false;actualWriterD78(s,t,0);ckD78(indexAngleD78(s,0)<.001,"old dialogue packet cannot survive current context exit");g_ctx_dialogue=true;
    g_dialogue_body178r9=false;actualWriterD78(s,t,0);ckD78(indexAngleD78(s,0)<.001,"current embodiment withdrawal retires visual trigger");g_dialogue_body178r9=true;
    g_gameplay_allowed=true;actualWriterD78(s,t,0);ckD78(indexAngleD78(s,0)<.001,"gameplay transition uses original gated trigger");g_gameplay_allowed=false;
    hostTickCount64+=81;actualWriterD78(s,t,0);ckD78(indexAngleD78(s,0)<.001,"fresh pose cannot renew stale original visual input");
    t=step();kcd2vr::tracking59c6::epoch=10;actualWriterD78(s,t,0);ckD78(indexAngleD78(s,0)<.001,"tracking epoch switch retires visual input");kcd2vr::tracking59c6::epoch=9;
    host.rc.active=false;t=step();actualWriterD78(s,t,0);actualWriterD78(s,t,1);
    ckD78(indexAngleD78(s,0)<.001&&indexAngleD78(s,1)>.79,"untracked right hand cannot borrow left hand validity or pose grace");host.rc.active=true;
    host.rc.trigger=std::numeric_limits<float>::quiet_NaN();t=step();actualWriterD78(s,t,0);
    ckD78(indexAngleD78(s,0)<.001,"invalid raw visual input is refused");host.rc.trigger=1;
    for(unsigned missing:{1u,2u,4u}){
        host.stampMaskD78=7u^missing;t=step();actualWriterD78(s,t,0);
        ckD78(indexAngleD78(s,0)<.001,"generic fallback stamps cannot admit missing original visual input identity");
    }
    host.stampMaskD78=7;host.futureSourceD78=true;t=step();actualWriterD78(s,t,0);
    ckD78(indexAngleD78(s,0)<.001,"future original source time cannot admit visual raw input");host.futureSourceD78=false;
    host.dialogueBody47c1=false;t=step();actualWriterD78(s,t,0);ckD78(indexAngleD78(s,0)<.001,"unqualified dialogue/menu keeps routed trigger semantics");
    // A held trigger leaving dialogue is still owned by the existing UI latch.
    g_ctx_dialogue=false;g_dialogue_body178r9=false;host.trigger_ui_after=false;host.vrik_gameplay_allowed=true;
    t=step();actualWriterD78(s,t,0);ckD78(!t.right_trigger&&!host.routeR.trigger&&indexAngleD78(s,0)<.001,"held trigger cannot turn dialogue exit into a gameplay pull");
    host.lc.trigger=host.rc.trigger=0;t=step();ckD78(!host.gameplay_trigger_[0].blocked&&!host.gameplay_trigger_[1].blocked,"tracked release rearms the original gameplay owner");
    host.lc.trigger=host.rc.trigger=1;t=step();
    ckD78(t.left_trigger==1&&t.right_trigger==1&&host.routeR.trigger==1&&host.routeL.trigger==1,"next deliberate pull preserves normal gameplay inputs");
    std::printf("dialogue_fingers_d78 %u/%u; actual producer/writer, native leaves mocked, headset NOT_RUN\n",checksD78-failuresD78,checksD78);
    return failuresD78?1:0;
}
