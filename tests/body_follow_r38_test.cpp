// Execute the production body/foot helper, station lean rule and arm geometry.
// These tests do not claim native capsule movement, physics, or headset results.
#include "vrik/body_follow_r38.h"
#include "vrik/station_ws_j27.h"
#include "vrik/upper_body.h"
#include <cstdio>
#include <limits>

namespace b=kcd2vr::body_follow_r38;
namespace p=kcd2vr::pose_core;
static int checks=0,failed=0;
static void check(bool ok,const char* message){++checks;if(!ok)++failed;std::printf("%s %s\n",ok?"PASS":"FAIL",message);}
static bool near(double a,double c,double e=1e-8){return std::abs(a-c)<e;}
static bool near(p::Vec3 a,p::Vec3 c,double e=1e-8){return p::length(a-c)<e;}
static b::Input input(){b::Input i{};i.owner.character=1;i.owner.pose=2;i.owner.tracking=3;i.owner.control=4;
    i.sample=1;i.tick=i.now=1000;i.timeNs=1000000000;i.active=true;return i;}
static void next(b::Input& i,double degrees){++i.sample;i.tick+=11;i.now=i.tick;i.timeNs+=11000000;i.headYaw=degrees*b::rad;}
static void HeadingAndWrap(){
    b::State s{};auto i=input();const auto first=b::advance(s,i);
    check(first.active&&!first.continuous&&near(first.body,0),"neutral activation has no manufactured hip turn");
    for(int k=0;k<60;++k){next(i,90);b::advance(s,i);}
    check(s.body>60*b::rad&&s.body<90*b::rad,"sustained 90-degree head turn recruits the hips beyond capped chest quarter-turn");
    b::State turn{};i=input();i.headYaw=170*b::rad;b::advance(turn,i);
    bool continuous=true,rate=true;double prior=turn.body;auto priorQ=b::yaw(turn.body);
    for(int k=171;k<=190;++k){next(i,b::wrapped(k*b::rad)/b::rad);const auto h=b::advance(turn,i);
        continuous=continuous&&h.continuous&&near(h.head,k*b::rad,1e-7)&&p::angle(priorQ,b::yaw(h.body))<4.01*b::rad;
        rate=rate&&std::abs(h.body-prior)<=360*b::rad*.011+1e-8;prior=h.body;priorQ=b::yaw(h.body);}
    check(continuous,"crossing +179 to -179 degrees keeps the body and neck on the same turn");
    check(rate,"only inferred hip posture is bounded to the configured angular speed");
    b::State loops{};i=input();b::advance(loops,i);bool unwrapped=true;
    for(int degrees=1;degrees<=1080;++degrees){next(i,b::wrapped(degrees*b::rad)/b::rad);auto h=b::advance(loops,i);
        unwrapped=unwrapped&&h.active&&near(h.head,degrees*b::rad,1e-7);}
    for(int degrees=1079;degrees>=-720;--degrees){next(i,b::wrapped(degrees*b::rad)/b::rad);auto h=b::advance(loops,i);
        unwrapped=unwrapped&&h.active&&near(h.head,degrees*b::rad,1e-7);}
    check(unwrapped&&loops.body< -650*b::rad,"multiple full turns and reversal do not wrap back to the original torso heading");
    const auto saved=loops;for(int k=0;k<100;++k)b::advance(loops,i);
    check(near(loops.body,saved.body)&&near(loops.head,saved.head)&&loops.tick==saved.tick,
        "repeated native callbacks for one XR sample never advance yaw again");
}
static void LifetimeAndTransaction(){
    auto seed=input();b::State committed{};b::advance(committed,seed);
    auto i=seed;next(i,90);auto staged=committed;const auto proposed=b::advance(staged,i);
    b::step_foot(staged.foot[0],{.3,0,0},proposed.body,i.sample,i.tick,false,false);
    check(near(committed.body,0)&&!committed.foot[0].have&&committed.sample==seed.sample,
        "rejected staged pose cannot change committed hip or foot history");
    auto retry=committed;b::advance(retry,i);
    check(near(retry.body,staged.body),"retry of the failed pose uses one temporal step");
    committed=staged;const auto before=committed;b::advance(committed,i);
    check(near(committed.body,before.body)&&committed.foot[0].sample==before.foot[0].sample,
        "successful publication followed by duplicate consumer cannot advance history twice");
    for(int kind=0;kind<5;++kind){auto s=committed;auto changed=i;next(changed,-10);
        if(kind==0)++changed.owner.pose;
        if(kind==1)++changed.owner.tracking;
        if(kind==2)++changed.owner.control;
        if(kind==3){changed.owner.kind=1;changed.owner.station=19;changed.owner.mode=20;changed.owner.lifetime=21;}
        if(kind==4)changed.trackingBasis=b::yaw(.7);
        const auto h=b::advance(s,changed);
        check(h.active&&!h.continuous&&near(h.head,-10*b::rad)&&!s.foot[0].have,
            kind==0?"recenter pose epoch retires yaw and foot history":kind==1?"tracking origin epoch retires yaw and foot history":
            kind==2?"body-control epoch retires yaw and foot history":kind==3?"station visit gets its own posture history":
            "changed calibrated model basis cannot borrow an old heading");}
    auto station=i;station.owner.kind=2;station.owner.station=88;station.owner.mode=9;station.owner.lifetime=10;
    b::State s{};b::advance(s,station);next(station,20);b::advance(s,station);++station.owner.lifetime;next(station,21);
    check(!b::advance(s,station).continuous,"same station address with a replaced lifetime resets posture");
    s=committed;auto stale=i;stale.now=stale.tick+81;check(!b::advance(s,stale).active&&!s.ready,"stale HMD cannot own body posture");
    s=committed;auto backwards=seed;check(!b::advance(s,backwards).active&&!s.ready,"out-of-order tracking cannot rewind body posture");
    s=committed;auto bad=i;bad.headYaw=std::numeric_limits<double>::quiet_NaN();check(!b::advance(s,bad).active,"invalid heading retires the owner");
}
static void FeetAndFrames(){
    const p::Vec3 pelvis{0,0,.9},ankle{.11,.08,.02},translation{.3,-.2,0};const double turn=90*b::rad;
    const auto offset=b::foot_offset(ankle,pelvis,translation,turn);
    const auto target=ankle+offset;const auto expected=pelvis+translation+p::rotate(b::yaw(turn),ankle-pelvis);
    check(near(target,expected)&&near(target.z,ankle.z),"turning foot target rotates about native pelvis and retains ground height");
    const auto world=b::yaw(1.3);const p::Vec3 worldRoot{10,-2,3};
    const auto worldTarget=p::rotate(world,target)+worldRoot;
    const auto covariance=p::rotate(world,pelvis+translation)+worldRoot+
        p::rotate(p::multiply(world,b::yaw(turn)),ankle-pelvis);
    check(near(worldTarget,covariance),"native entity/world turn is composed once around the model-local body pose");
    b::Foot right{},left{};b::step_foot(right,{},0,1,1000,false,false);b::step_foot(left,{},0,1,1000,false,false);
    b::step_foot(right,{.02,0,0},turn,2,1011,true,false);
    b::step_foot(left,{-.02,0,0},turn,2,1011,true,right.stepping);
    check(right.stepping&&!left.stepping,"foot yaw starts an alternating turn step even below the distance trigger");
    const auto mid=b::step_foot(right,{.02,0,0},turn,3,1101,true,false);
    check(near(mid.currentYaw,45*b::rad)&&near(mid.current.z,.05)&&near(mid.current.x,.01),
        "foot turns with its 180 ms step and lifts above native ground");
    const auto duplicate=b::step_foot(right,{.02,0,0},turn,3,1120,true,false);
    check(near(duplicate.current,mid.current)&&near(duplicate.currentYaw,mid.currentYaw),"duplicate XR sample cannot advance a foot step");
    const auto end=b::step_foot(right,{.02,0,0},turn,4,1191,true,false);
    check(!end.stepping&&near(end.currentYaw,turn)&&near(end.current,{.02,0,0}),"completed step plants at the turned target on the native ground");
    b::step_foot(left,{-.02,0,0},turn,4,1191,true,right.stepping);
    check(left.stepping,"other foot may step after the first is planted");
}
static void HandsStayAuthoritative(){
    auto i=input();b::State state{};b::advance(state,i);bool endpoints=true,lengths=true;
    const p::Vec3 authoritative[2]{{.48,.31,1.1},{-.46,.37,1.18}};
    for(int degrees=0;degrees<=400;++degrees){next(i,degrees);const auto h=b::advance(state,i);
        kcd2vr::station_ws_j27::TorsoIn torso{};torso.follow=true;torso.spine=.65;torso.clear=0;torso.hy=.4;
        const auto hips=kcd2vr::station_ws_j27::torso(torso);
        for(int side=0;side<2;++side){
            const auto shoulder=p::Vec3{hips.px,hips.py,1.4}+p::rotate(b::yaw(h.body),p::Vec3{side?-.18:.18,0,0});
            const auto fit=p::controller_shoulder103(shoulder,authoritative[side],.26628,.26153,{0,1,0});
            const auto axis=fit?p::unit(authoritative[side]-fit->shoulder):std::nullopt;
            const auto pole=axis?p::unit(p::cross(*axis,p::Vec3{0,0,1})):std::nullopt;
            const auto arm=fit&&pole?p::solve_arm(fit->shoulder,authoritative[side],.26628,.26153,*pole,*pole):std::nullopt;
            endpoints=endpoints&&arm&&near(arm->hand,authoritative[side]);
            lengths=lengths&&arm&&near(p::length(arm->elbow-fit->shoulder),.26628)&&near(p::length(arm->hand-arm->elbow),.26153);
        }
    }
    check(endpoints&&lengths,"upright body steps and a full hip turn cannot move either authoritative wrist or stretch arms");
}
int main(){HeadingAndWrap();LifetimeAndTransaction();FeetAndFrames();HandsStayAuthoritative();
    std::printf("body_follow_r38 %d/%d; native_game_and_headset=NOT_EXECUTED\n",checks-failed,checks);return failed?1:0;}
