#include "../src/vrik/weapon_sweep112.h"
#include "../src/diagnostics/bow_contact169r2.h"
#include <cstdio>
#include <string>
using namespace kcd2vr::physical;
namespace se=kcd2vr::sweep169r2;
static int checks=0,failures=0;static void ck(bool v,const char* n){++checks;if(!v)++failures;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
int main(){
    // Preserve the historical uniform-grid reason/counter fixture explicitly.
    // Current five-feature radius planning is covered by weapon_sweep_d67r1.
    kcd2vr::held_motion_d67::setCurrentArcCoverage(false);
    Profile p{};p.side=0;p.tip={0,1,0};Pose from{},to{};to.q=m::from_rotation_vector({0,0,64*3.141592653589793/180});
    auto c=sweep_weapon112(p,from,to,false,[](Vec3,Vec3,double){return Contact103{};});
    ck(c.known&&c.limited115&&c.coverage169r2.reason==se::Reason::AngularPrefix,"64-degree all-CLEAR sweep is ANGULAR_PREFIX, not timer starvation");
    ck(c.coverage169r2.completedRays==15&&c.coverage169r2.completed==3&&c.coverage169r2.checkedAngle<c.coverage169r2.requestedAngle,"complete 15 rays remain distinguishable from complete requested rotation");
    for(auto reason:{se::Reason::TimeBudget,se::Reason::RayCap,se::Reason::SameSample,se::Reason::CallFault}){
        int n=0;c=sweep_weapon112(p,from,to,false,[&](Vec3,Vec3,double){Contact103 x{};if(++n==7){x.known=false;x.fraction=0;x.coverage169r2.reason=reason;}return x;});
        ck(c.known&&c.limited115&&c.coverage169r2.reason==reason&&c.coverage169r2.completed==1&&c.coverage169r2.completedRays==6,"query failure retains only completed prefix and preserves its actual cause");
    }
    c=sweep_weapon112(p,from,to,false,[](Vec3,Vec3,double){Contact103 x{};x.known=false;x.fraction=0;x.coverage169r2.reason=se::Reason::TimeBudget;return x;});
    ck(!c.known&&c.fraction==0&&c.coverage169r2.reason==se::Reason::TimeBudget,"no complete segment means zero allowed motion");
    namespace q=kcd2vr::diagnostics::bow169r2;namespace cap=kcd2vr::diagnostics::capture168;
    q::Record r{};r.kind=1;r.tick=1000;r.sample=10;r.sweep=c.coverage169r2;
    ck(!q::push(r)&&q::sweepCounts[unsigned(se::Reason::TimeBudget)].load()==0,"recording OFF does not collect query counters or queue records");
    cap::gate.select(cap::Domain::Interaction,true,1000);ck(q::push(r),"F5 alone can record bow/sweep evidence");
    ck(!q::push(r)&&q::sweepCounts[unsigned(se::Reason::TimeBudget)].load()==2,"detail cap does not erase aggregate query counts");
    std::string emitted;q::drain(1001,[&](const char* line){emitted+=line;emitted+='\n';});
    ck(emitted.find("TIME_BUDGET=2")!=std::string::npos&&emitted.find("countBasis=CAPTURED_QUERY_CALLS_NOT_FRAMES")!=std::string::npos,"worker distinguishes call counts from sparse samples or frame percentages");
    std::printf("sweep169r2=%d/%d\n",checks-failures,checks);return failures?1:0;
}
