#include "vrik/weapon_sweep112.h"
#include "vrik/tracking_grace111.h"
#include "vrik/interaction_ray112.h"
#include "vrik/physical_guard112.h"
#include "vrik/handling_profile59c10r2.h"
#include <cstdio>
#include <cstdlib>
#include <limits>
using namespace kcd2vr;
static int n=0;static void ck(bool v,const char* m){++n;if(!v){std::fprintf(stderr,"FAIL %s\n",m);std::exit(1);}}
struct Tracking {
    std::uint64_t tick=0,head_good_tick=0,left_good_tick=0,right_good_tick=0;
    physical::Pose head{},left{},right{};int hq=0,lq=0,rq=0;
    bool head_valid=false,left_valid=false,right_valid=false,allowed=true;
    bool head_held=false,left_held=false,right_held=false;
};
int main(){
    using namespace physical;
    {
        handling59c10r2::GripTraceGate gate;
        ck(gate.due(0,50,1,1000)&&gate.due(1,80,1,1000),"both equipped hands produce one profile edge");
        for(unsigned n=1;n<900;++n)
            ck(!gate.due(0,50,1,1000+n*10)&&!gate.due(1,80,1,1000+n*10),"alternating mace/shield cannot defeat per-side throttle");
        ck(gate.due(0,50,3,10000)&&!gate.due(1,80,1,10000),"support readiness edge does not unthrottle other hand");
        ck(gate.due(1,80,1,11000)&&!gate.due(0,50,3,11000),"periodic diagnostics remain bounded per side");
        ck(!gate.due(2,80,1,11000),"invalid side cannot index profile diagnostic state");
        ck(handling59c10r2::swordFallback(4,true)&&!handling59c10r2::swordFallback(7,true),"accepted sword fallback never shortens polearm support spacing");
        ck(std::abs(handling59c10r2::supportDistance(7,false,.32,2.0)-.32)<1e-9,"polearm retains configured 32 cm support when sword comparison enabled");
        ck(std::abs(handling59c10r2::supportDistance(4,false,.32,1.1)-.12)<1e-9,"legacy longsword fallback remains 12 cm");
    }
    {
        Profile shield{};shield.side=1;shield.classId=8;shield.geometry=true;
        shield.diagnosticBounds59c6=true;shield.diagnosticBoundsMin59c6={-.3,-.02,-.4};
        shield.diagnosticBoundsMax59c6={.3,.02,.4};shield.tip={0,0,.4};
        std::array<Vec3,5> features{};double radius=0,thickness=0;
        ck(shield_features59c10r2(shield,features,radius,thickness)&&std::abs(thickness-.02)<1e-9,"shield features use exact item face bounds and thickness");
        Pose from{},to{};to.p.y=.1;
        // Obstacle at the left rim: every old grip-to-top probe missed it.
        int count=0;auto hit=sweep_weapon112(shield,from,to,false,[&](Vec3 a,Vec3,double){
            ++count;Contact103 c{};if(a.x<-.29){c.fraction=.3;c.plane=true;c.normal={0,-1,0};c.point={a.x,.03,a.z};}return c;
        });
        ck(hit.known&&hit.plane&&hit.fraction<1&&hit.feature.x<-.29&&count<=5,"shield lateral rim stops at obstacle under existing five-ray translation budget");
        count=0;hit=sweep_weapon112(shield,from,to,false,[&](Vec3,Vec3,double){++count;return Contact103{};});
        ck(hit.known&&!hit.limited115&&hit.fraction==1&&count==5,"clear shield checks both rims with five rays");
        count=0;hit=sweep_weapon112(shield,from,to,false,[&](Vec3,Vec3,double){Contact103 c{};if(++count==3)c.known=false;return c;});
        ck(!hit.known&&hit.fraction==0,"unknown shield rim cannot authorize face motion");
        g_sweep_coverage_scale178r6=4;to.q=pose_core::from_rotation_vector({0,0,2});count=0;
        hit=sweep_weapon112(shield,from,to,false,[&](Vec3,Vec3,double){++count;return Contact103{};});
        ck(hit.known&&hit.limited115&&count==15,"large shield turn keeps complete face probes and original 15-ray ceiling");
        g_sweep_coverage_scale178r6=1;shield.diagnosticBounds59c6=false;count=0;
        hit=sweep_weapon112(shield,from,to,false,[&](Vec3,Vec3,double){++count;return Contact103{};});
        ck(!hit.known&&count==0,"unqualified shield bounds fail closed without invented face");
    }
    Profile p{};p.side=0;p.tip={1,0,0};p.geometry=true;
    Pose a{},b{};a.q=pose_core::from_rotation_vector({0,0,-.21});b.q=pose_core::from_rotation_vector({0,0,.21});
    int queries=0;
    auto wall=[&](Vec3 start,Vec3 end,double margin){
        ++queries;Contact103 c{};
        if(start.x<.995&&end.x>=.995){c.fraction=std::max(0.0,(.995-start.x-margin)/(end.x-start.x));c.plane=true;c.normal={-1,0,0};c.point={.995,0,0};}
        return c;
    };
    ck(point(a,p.tip).x<.995&&point(b,p.tip).x<.995,"straight chord misses wall");
    auto hit=sweep_weapon112(p,a,b,false,wall);
    ck(hit.known&&hit.fraction<1&&hit.plane,"rotating blade arc detects wall");
    ck(queries<=15&&queries>=5,"bounded curved sampling; stop at earliest segment hit");
    queries=0;auto unknown=sweep_weapon112(p,a,b,false,[&](Vec3,Vec3,double){Contact103 c{};if(++queries==6)c.known=false;return c;});
    ck(unknown.known&&unknown.limited115&&std::abs(unknown.fraction-1.0/3)<1e-9,"budget failure retains only the complete first segment");
    b.q=pose_core::from_rotation_vector({0,0,2});queries=0;
    unknown=sweep_weapon112(p,a,b,false,[&](Vec3,Vec3,double){++queries;return Contact103{};});
    ck(unknown.known&&unknown.limited115&&unknown.fraction>0&&unknown.fraction<1&&queries<=15,"large arc advances only a checked prefix within the ray cap");
    Pose progress=a;bool reached=false;int maxQueries=0;
    for(int i=0;i<12;++i){queries=0;auto part=sweep_weapon112(p,progress,b,false,[&](Vec3,Vec3,double){++queries;return Contact103{};});
        maxQueries=std::max(maxQueries,queries);progress=blend(progress,b,part.fraction);if(!part.limited115){reached=true;break;}}
    ck(reached&&maxQueries<=15&&pose_core::angle(progress.q,b.q)<1e-6,"repeated clear prefixes recover a large native-to-controller gap without teleporting");
    queries=0;unknown=sweep_weapon112(p,a,b,false,[&](Vec3,Vec3,double){Contact103 c{};if(++queries==6)c.known=false;return c;});
    ck(unknown.known&&unknown.limited115&&unknown.fraction>0&&unknown.fraction<.1,"partial-prefix failure retains only the completed small arc");
    a={};b={};b.p={0,.1,0};queries=0;
    hit=sweep_weapon112(p,a,b,false,wall);ck(hit.known&&hit.fraction==1&&queries==5,"translation keeps five probes");
    State recovery{};Input ri{};ri.epoch=1;ri.sample=1;ri.tick=1000;ri.allowed=ri.tracked[0]=true;
    ri.grip[0]=.8;ri.contact=ri.attacks=ri.heldMotionAttacks=true;
    auto rp=p;rp.key=77;rp.generation=1;rp.classId=4;rp.primary=rp.nativeSeed=true;rp.nativeWeapon.q=pose_core::from_rotation_vector({0,0,2.4});
    std::array<Profile,2> profiles{rp,Profile{}};
    bool deferred=false,restored=false,quiet=true;
    for(int i=0;i<20;++i){auto o=step(recovery,ri,profiles,[&](const Profile& pr,Pose from,Pose to,bool acquire){return sweep_weapon112(pr,from,to,acquire,[](Vec3,Vec3,double){return Contact103{};});});
        if(o.sweepDeferred115[0]){deferred=true;quiet=quiet&&!o.contactKnown[0]&&o.contactPrefixKnown126[0]&&!o.commandCount&&o.impactSpeed[0]==0;}
        if(o.active[0]&&o.contactKnown[0]&&pose_core::angle(o.weapons[0].q,{})<1e-6){restored=true;break;}
        ++ri.sample;ri.tick+=10;}
    ck(deferred&&restored&&quiet,"actual weapon kernel recovers initial 137-degree gap without attack or haptic claims on deferred arc");
    Tracking old{};old.tick=1000;old.head_valid=old.left_valid=old.right_valid=true;
    old.head.p={1,2,3};old.hq=42;old.left.p={4,5,6};
    tracking_grace111::HoldCoherent112(old,Tracking{});
    Tracking held{};held.tick=1050;tracking_grace111::HoldCoherent112(held,old);
    ck(held.head_held&&held.head.p.x==1&&held.hq==42&&held.head_good_tick==1000,"pose/quaternion/age held together");
    Tracking expired{};expired.tick=1061;tracking_grace111::HoldCoherent112(expired,held);
    ck(!expired.head_valid&&expired.left_valid,"original head age expires independently");
    expired={};expired.tick=1081;tracking_grace111::HoldCoherent112(expired,held);
    ck(!expired.left_valid&&!expired.right_valid,"invalid updates never renew grace");
    held.allowed=false;expired={};expired.tick=1051;tracking_grace111::HoldCoherent112(expired,held);
    ck(!expired.head_valid,"menu/session boundary discards old pose");
    old.tick=2000;expired={};expired.tick=999;tracking_grace111::HoldCoherent112(expired,old);
    ck(!expired.head_valid,"reversed clock rejects grace");
    interaction112::Ray ray{};ray.tick=1000;ray.y=2;ray.limit=.4;ray.radius=.14;
    ck(interaction112::Normalize(ray)&&ray.y==1,"hand ray normalized");
    ck(interaction112::Fresh(ray,1100)&&!interaction112::Fresh(ray,1101)&&!interaction112::Fresh(ray,999),"ray expiry and reversal");
    ray.ox=std::numeric_limits<float>::infinity();ck(!interaction112::Normalize(ray),"nonfinite origin rejected");
    ray.ox=0;ray.radius=std::numeric_limits<float>::quiet_NaN();ck(!interaction112::Normalize(ray),"nonfinite radius rejected");
    ck(guard112::sector({0,0,.3})==0&&guard112::sector({-.3,0,0})==1&&
       guard112::sector({.3,0,0})==2&&guard112::sector({0,0,-.3})==5,"four retail guard sectors");
    ck(guard112::sector({})==-1,"center dead zone");
    guard112::State guard{};Input in{};in.allowed=true;in.tracked[0]=true;in.sample=1;in.tick=1000;in.epoch=1;
    p.key=77;p.generation=2;p.classId=4;p.tip={.6,0,0};Pose guardPose{};guardPose.p={-.3,.4,.3};
    auto gd=guard112::update(guard,in,p,guardPose,true);ck(!gd.block,"guard acquisition has no instant press");
    for(int i=0;i<10;++i){++in.sample;in.tick+=10;gd=guard112::update(guard,in,p,guardPose,true);}
    ck(gd.block&&gd.zone==0,"steady transverse high guard requests native block");
    gd=guard112::update(guard,in,p,guardPose,true);ck(gd.block,"duplicate eye sample does not change guard");
    guardPose.p.y=-.4;++in.sample;in.tick+=10;gd=guard112::update(guard,in,p,guardPose,true);ck(!gd.block,"behind body immediately releases");
    gd=guard112::update(guard,in,p,guardPose,false);ck(gd.zone==-1&&!gd.block&&!guard.sample,"protected context clears guard lease");
    p.classId=9;gd=guard112::update(guard,in,p,guardPose,true);ck(gd.zone==-1&&!gd.block,"bow never becomes melee guard");
    std::printf("PASS %d V66.112 regression checks\n",n);
}
