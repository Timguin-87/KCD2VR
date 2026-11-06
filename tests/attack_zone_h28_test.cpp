// Production body-frame -> native zone -> directional announcement contract.
// Native accepted blocks/hits are deliberately outside this host fixture.
#include "../src/vrik/physical_weapon.h"
#include <cstdio>
#include <limits>
namespace ph=kcd2vr::physical;
namespace m=kcd2vr::pose_core;
namespace d=kcd2vr::dirblock39;
static unsigned checks=0,failed=0;
static void check(bool yes,const char* what){++checks;if(!yes){++failed;std::printf("FAIL %s\n",what);}}
int main(){
    struct Stroke{m::Vec3 local;int zone;};
    const Stroke strokes[]{{{3,.6,0},1},{{-3,.6,0},2},{{.2,.6,-3},0},{{1.8,.6,2.4},1},{{-1.8,.6,2.4},2}};
    for(int yaw=0;yaw<360;yaw+=45){
        const auto q=m::from_rotation_vector({0,0,yaw*3.141592653589793/180.});
        ph::Input in{};const m::Vec3 root{14,-9,1};
        in.shoulders[0]=root+m::rotate(q,{.2,0,0});
        in.shoulders[1]=root+m::rotate(q,{-.2,0,0});
        for(const auto& s:strokes){
            const auto local=ph::body_vector117(in,m::rotate(q,s.local));
            const int zone=ph::stroke_zone117(ph::CommandKind::Slash,local);
            check(zone==s.zone,"cut uses Henry-relative FROM side under world/body yaw");
            const auto predicted=d::predictCut({local.x,local.y,local.z},{});
            check(predicted.valid&&!predicted.nearBoundary&&predicted.zone==zone,"NPC announcement agrees with physical cut sector");
            const auto word=d::pack(910,zone,predicted.angle,false,1000);
            const auto sample=d::unpack(word,910,1010,d::kFreshMs);d::Choice why{};
            check(d::chooseInitial(true,true,0,zone,sample,why)==zone,"fresh same-gesture native announcement keeps measured zone");
            check(!d::unpack(word,911,1010,d::kFreshMs).valid,"another swing cannot borrow the announced zone");
            check(!d::unpack(word,910,1061,d::kFreshMs).valid,"expired direction cannot replace current stroke zone");
        }
    }
    check(ph::stroke_zone117(ph::CommandKind::Thrust,{0,3,0})==5,"classified thrust native lower/stab zone remains distinct");
    check(ph::stroke_zone117(ph::CommandKind::Slash,{std::numeric_limits<double>::quiet_NaN(),0,0})==-1,"invalid motion cannot announce a zone");
    check(d::pack(910,3,270,false,1000)==0&&d::pack(910,4,270,false,1000)==0,"disabled retail cut sectors are never invented");
    std::printf("%u/%u attack zone contract checks PASS\n",checks-failed,checks);return failed?1:0;
}
