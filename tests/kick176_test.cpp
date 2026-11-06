#include "../src/vrik/kick176.h"
#include <cstdio>
#include <limits>
using namespace kcd2vr::kick176;
namespace pose_core=kcd2vr::pose_core;
static unsigned checks=0,failures=0;
static void check(bool pass,const char* text){++checks;if(!pass){++failures;std::printf("FAIL %s\n",text);}}
namespace query176 {
using V3=Vec3;
static V3 Sub(V3 a,V3 b){return a-b;}static V3 Add(V3 a,V3 b){return a+b;}
static V3 Mul(V3 a,float b){return a*b;}static double Len(V3 a){return pose_core::length(a);}
static V3 Normalize(V3 a){return a*(1/Len(a));}static V3 Cross(V3 a,V3 b){return pose_core::cross(a,b);}
namespace wc {enum class Result{Clear,Hit,Busy,Fault};}
struct NpcQuery172 {bool requested=false;unsigned token=0;};
static NpcQuery172 g_npc_query172;static thread_local bool g_kick_query40=false; // C40 mirrors frik_pose_probe.cpp
static bool g_contact_budget_active6695=true,g_contact_pass6695=true,transformValid=true,bodyObserved=false;
struct ModelWorld6692{};
static bool ReadModelWorld6692(void*,ModelWorld6692&){return transformValid;}
struct Hit {wc::Result state=wc::Result::Clear;bool plane=false;Vec3 point{},normal{};
    struct Witness {std::uint32_t entityId=0;int part=-1;unsigned body=0;bool ownerRead=false;int bodyMaterial=-1;std::uintptr_t collider=0;}native172;}; // C37: + idmatOrg/collider
static Hit hits[6]{};static unsigned calls=0;static V3 segments[6]{};
static Hit WorldRayDistance6692(V3,V3 segment){bodyObserved=bodyObserved||g_npc_query172.requested;segments[std::min(calls,5u)]=segment;return hits[std::min(calls++,5u)];}
#include "../src/vrik/kick_contact176.inl"
static void reset(){for(auto& h:hits)h={};calls=0;transformValid=g_contact_pass6695=g_contact_budget_active6695=true;
    bodyObserved=false;g_npc_query172={false,77};}
static void run(){
    reset();auto contact=ProbeKickContact176(nullptr,{0,0,0},{0,.1,0});
    check(contact.kind==ContactKind::Clear&&calls==3&&bodyObserved,"actual adapter requires all three clear body/world rays");
    check(!g_npc_query172.requested&&g_npc_query172.token==77,"actual adapter restores prior query context after clear");
    reset();hits[0].state=wc::Result::Busy;contact=ProbeKickContact176(nullptr,{0,0,0},{0,.1,0});
    check(contact.kind==ContactKind::Unknown&&calls==1,"budget/unavailable native query cannot become clearance");
    check(g_npc_query172.token==77&&!g_npc_query172.requested,"context restored after unavailable query");
    reset();hits[1].state=wc::Result::Hit;hits[1].plane=true;hits[1].native172={123,456,2,true};
    contact=ProbeKickContact176(nullptr,{0,0,0},{0,.1,0});
    check(contact.kind==ContactKind::Hit&&calls==2&&contact.entity==123&&contact.part==456&&contact.bodyOwner,"actual adapter publishes only known contact identity");
    check(g_npc_query172.token==77&&!g_npc_query172.requested,"context restored after hit");
    {reset();hits[0].state=wc::Result::Hit;hits[0].plane=true;hits[0].native172={123,456,2,true,8,0xABC};int material=-1;std::uintptr_t collider=0;
     contact=ProbeKickContact176(nullptr,{0,0,0},{0,.1,0},&material,&collider);
     check(contact.kind==ContactKind::Hit&&material==8&&collider==0xABC,"C37: the foot ray hands its own body material (idmatOrg) and collider to the kick transaction");}
    reset();hits[0].state=wc::Result::Hit;contact=ProbeKickContact176(nullptr,{0,0,0},{0,.1,0});
    check(contact.kind==ContactKind::Unknown,"unvalidated hit plane remains unknown");
    reset();float gap=-1;hits[1].state=wc::Result::Hit;hits[1].plane=true;hits[1].point={0,.18,0};
    contact=ProbeKickContact176(nullptr,{0,0,0},{0,.1,0},nullptr,nullptr,.1f,&gap);
    check(contact.kind==ContactKind::Hit&&calls==2&&std::abs(gap-.08f)<1e-5&&std::abs(Len(segments[1])-.2)<1e-5,"H8 explicit assist extends only the current segment and reports visible gap");
    reset();hits[0].state=wc::Result::Busy;contact=ProbeKickContact176(nullptr,{0,0,0},{0,.1,0},nullptr,nullptr,.3f);
    check(contact.kind==ContactKind::Unknown&&calls==1,"H8 busy actual sweep never becomes an assist-only hit");
    reset();contact=ProbeKickContact176(nullptr,{0,0,0},{0,.1,0},nullptr,nullptr,.1f);
    check(contact.kind==ContactKind::Clear&&calls==6&&!g_npc_query172.requested,"H8 assisted clear is bounded to six rays and restores context");
    reset();contact=ProbeKickContact176(nullptr,{0,0,0},{.1,0,0},nullptr,nullptr,.1f);
    check(calls==6&&std::abs(segments[1].x-.2)<1e-5&&std::abs(segments[1].y)<1e-7,"H8 next segment never inherits prior kick direction");
    for(float assist:{.31f,std::numeric_limits<float>::quiet_NaN()}){reset();contact=ProbeKickContact176(nullptr,{0,0,0},{0,.1,0},nullptr,nullptr,assist);
     check(calls==3,"H8 invalid assist cannot extend the sweep");}
    for(auto end:{Vec3{},Vec3{0,.6,0}}){reset();contact=ProbeKickContact176(nullptr,{0,0,0},end,nullptr,nullptr,.3f);
     check(calls==0&&contact.kind==ContactKind::Unknown,"H8 zero/jumped segment refuses even with assist on");}
    for(int failure=0;failure<4;++failure){reset();Vec3 end{0,.1,0};
        if(failure==0)transformValid=false;if(failure==1)g_contact_pass6695=false;
        if(failure==2)end={0,.6,0};if(failure==3)end={};
        contact=ProbeKickContact176(nullptr,{0,0,0},end);
        check(contact.kind==ContactKind::Unknown&&calls==0,"invalid transform/pass/segment performs no native query");}
}
}
static Input input(){Input i{};i.tick=i.now=1000;i.sample=1;i.epoch=i.cancel=1;i.eligible=i.legsReady=true;
    i.request={1,1000,1};i.profile=kcd2vr::kick52::Profile::Old;i.forward={0,1,0};
    for(int side=0;side<2;++side){i.hip[side]={side?-.1:.1,0,.9};i.ankle[side]={side?-.1:.1,0,.1};i.reach[side]=.82;}return i;}
int main(){
    State s{};auto i=input();auto f=advance(s,i);
    check(f.started&&f.active&&f.side==0&&f.status==Status::Lift,"right leg begins from explicit request");
    check(pose_core::length(f.target-i.ankle[0])<1e-12,"first target equals native ankle");
    const auto base=s;
    auto repeat=advance(s,i);check(repeat.active&&!repeat.started&&!repeat.newSample,"repeated eye cannot restart or advance sample");
    auto duplicateInput=i;duplicateInput.tick+=50;repeat=advance(s,duplicateInput);
    check(repeat.status==Status::Lift&&pose_core::length(repeat.target-i.ankle[0])<1e-12&&s.lastTick==1000,
        "same sample with changed tick cannot advance the trajectory clock");
    i.tick+=60;++i.sample;f=advance(s,i);check(f.target.z>i.ankle[0].z&&f.target.y>0,"knee-up phase lifts forward");
    i.tick=1120;++i.sample;f=advance(s,i);check(f.status==Status::Extend,"extension begins at 120ms");
    i.tick=1200;++i.sample;f=advance(s,i);check(f.target.y>.3&&f.leanRadians>0&&f.leanRadians<=.07,"bounded forward extension and lean");
    const auto beforeTurn=f.target;i.forward={1,0,0};++i.sample;f=advance(s,i);
    check(pose_core::length(f.target-beforeTurn)<1e-12,"looking aside after request does not steer kick");
    i.tick=1250;++i.sample;f=advance(s,i);check(f.status==Status::Extend&&pose_core::length(f.target-i.hip[0])>.85*i.reach[0],"C34b: foot held out on the target, leg nearly straight");
    i.tick=1310;++i.sample;f=advance(s,i);check(f.status==Status::Recover,"recovery starts at 300ms");
    for(auto t:{1360u,1410u,1460u}){i.tick=t;++i.sample;f=advance(s,i);}
    i.tick=1520;++i.sample;f=advance(s,i);check(f.terminal&&!f.active&&!s.active&&f.status==Status::Complete,"520ms ends ownership");
    check(s.nextSide==1,"next completed attempt uses other leg");
    auto idle=i;idle.eligible=false;
    for(int n=0;n<20;++n){idle.tick+=10;++idle.sample;auto r=advance(s,idle);check(!r.terminal&&!r.serial,"retired attempt emits no repeated idle terminal");}
    i.tick=i.now=1500;i.request={2,1500,1};++i.sample;f=advance(s,i);check(f.started&&f.side==1,"left leg available without extra tracking");
    const auto second=s;i.request={3,1500,1};++i.sample;f=advance(s,i);
    check(s.serial==2&&s.seenRequest==3,"busy input edge consumed, never queued");
    i.eligible=false;f=advance(s,i);check(f.terminal&&f.status==Status::Cancelled&&!s.active,"eligibility loss cancels");
    i.eligible=true;i.tick+=10;++i.sample;f=advance(s,i);check(!f.active,"lost busy request never replays");
    for(int failure=0;failure<7;++failure){State bad{};auto q=input();
        if(failure==0)q.request.tick=899;if(failure==1)q.request.tick=1001;if(failure==2)q.request.tick=0;
        if(failure==3)q.request.cancel=2;if(failure==4)q.legsReady=false;
        if(failure==5)q.forward={0,0,1};if(failure==6)q.reach[0]=std::numeric_limits<double>::quiet_NaN();
        auto r=advance(bad,q);check(r.terminal&&!r.active,"invalid request/rig fails closed");
    }
    State pending{};auto pendingInput=input();pendingInput.request.tick=1001;pendingInput.now=1001;
    auto waiting=advance(pending,pendingInput);
    check(!waiting.active&&!waiting.terminal&&!waiting.serial&&!pending.seenRequest,
        "new request waits unconsumed for the first tracking sample at or after its timestamp");
    pendingInput.tick=1001;++pendingInput.sample;
    auto began=advance(pending,pendingInput);
    check(began.active&&began.started&&pending.seenRequest==1,"next fresh sample starts pending request exactly once");
    check(!advance(pending,pendingInput).started,"repeated eligible sample cannot restart the pending request");
    State expired{};pendingInput=input();pendingInput.request.tick=1001;pendingInput.now=1102;
    const auto stale=advance(expired,pendingInput);
    check(stale.terminal&&stale.status==Status::StaleRequest&&expired.seenRequest==1,
        "old callback cannot keep pending request alive beyond actual wall-clock deadline");
    pendingInput.tick=pendingInput.now=1102;++pendingInput.sample;
    check(!advance(expired,pendingInput).active,"later fresh tracking cannot revive consumed expired request");
    for(int failure=0;failure<4;++failure){auto bad=base;auto q=input();q.request={};++q.sample;
        if(failure==0)q.tick=999;if(failure==1)q.tick=1101;if(failure==2)++q.cancel;if(failure==3)++q.epoch;
        auto r=advance(bad,q);check(!r.active,"time discontinuity/cancel/pose epoch discards trajectory");
    }
    auto moved=base;auto q=input();q.tick=1050;++q.sample;auto unchanged=advance(moved,q);
    auto rollback=moved;auto oldSample=q;--oldSample.sample;++oldSample.tick;
    check(!advance(rollback,oldSample).active,"sequence rollback cancels active attempt");
    moved=base;q.hip[0]=q.hip[0]+Vec3{4,2,.1};q.ankle[0]=q.ankle[0]+Vec3{4,2,.1};auto translated=advance(moved,q);
    check(pose_core::length(translated.target-unchanged.target-Vec3{4,2,.1})<1e-9,"current native hip anchors trajectory after locomotion");
    check(Contact{}.kind==ContactKind::Unknown,"unavailable probe is never clear");
    query176::run();
    std::printf("kick176=%u/%u trajectory_only=YES native_damage_stamina=DISABLED\n",checks-failures,checks);
    return failures?1:0;
}
