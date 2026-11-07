#include "../../../src/subsystems/alchemy/geometry68.h"
#include <cstdio>
#include <cstdlib>
#include <limits>
namespace geo=kcd2vr::subsystems::alchemy::geometry68;
static unsigned checks=0;
static void ck(bool value,const char* what){++checks;if(!value){std::fprintf(stderr,"FAIL %u %s\n",checks,what);std::exit(1);}}
static bool near(double a,double b,double tolerance=1e-7){return std::fabs(a-b)<tolerance;}
static geo::Bounds unit(){geo::Bounds b{};for(int j=0;j<3;++j){b.local[j]=-1;b.local[j+3]=1;}b.world[0]=b.world[5]=b.world[10]=1;return b;}
int main(){
    auto b=unit();ck(geo::valid(b),"valid identity bounds");
    auto r=geo::closest(b,{.5,.4,.3});ck(r.valid&&near(r.distance,0)&&near(r.point.x,.5)&&near(r.point.y,.4),"inside box is zero distance");
    r=geo::closest(b,{1.1,.2,.3});ck(r.valid&&near(r.distance,.1)&&near(r.point.x,1)&&near(r.point.y,.2),"face distance");
    r=geo::closest(b,{2,3,4});ck(r.valid&&near(r.distance,std::sqrt(14.))&&near(r.point.x,1)&&near(r.point.y,1)&&near(r.point.z,1),"corner distance");
    ck(geo::closest(b,{1.099999,0,0}).distance<.1&&geo::closest(b,{1.100001,0,0}).distance>.1,"ten centimeter qualification boundary");
    const float transform[12]{0,-3,0,10,2,0,0,20,0,0,4,30};for(int j=0;j<12;++j)b.world[j]=transform[j];
    r=geo::closest(b,{13.08,20,30});ck(r.valid&&near(r.distance,.08)&&near(r.point.x,13)&&near(r.point.y,20),"rotation/nonuniform scale/native row ordering");
    r=geo::closest(b,{13.08,22.06,34});ck(r.valid&&near(r.distance,.1)&&near(r.point.x,13)&&near(r.point.y,22)&&near(r.point.z,34),"rotated corner offset measured in world meters");
    b=unit();b.world[1]=1;
    r=geo::closest(b,{3,0,0});ck(r.valid&&near(r.distance,std::sqrt(2.))&&near(r.point.x,2)&&near(r.point.y,1),"sheared box nearest corner");
    r=geo::closest(b,{2,.5,0});ck(r.valid&&near(r.distance,std::sqrt(.125))&&near(r.point.x,1.75)&&near(r.point.y,.75),"sheared box projection; inverse clamp would be wrong");
    b=unit();b.world[0]=-2;b.world[5]=3;b.world[10]=4;
    r=geo::closest(b,{2.05,0,0});ck(r.valid&&near(r.distance,.05)&&near(r.point.x,2),"negative native scale");
    b=unit();b.local[0]=-2;b.local[3]=4;b.local[1]=-4;b.local[4]=8;b.local[2]=-6;b.local[5]=10;
    r=geo::closest(b,{0,8.05,0});ck(r.valid&&near(r.distance,.05)&&near(r.point.y,8),"off-origin model bounds");
    b=unit();b.local[2]=b.local[5]=0;r=geo::closest(b,{.2,.3,.07});
    ck(r.valid&&near(r.distance,.07)&&near(r.point.x,.2)&&near(r.point.y,.3)&&near(r.point.z,0),"native planar bounds remain precise");
    b=unit();b.local[0]=2;ck(!geo::closest(b,{}).valid,"reversed bounds reject");
    b={};ck(!geo::closest(b,{}).valid,"empty transform and point box reject");
    b=unit();for(int j=0;j<6;++j)b.local[j]=0;ck(!geo::closest(b,{}).valid,"point bounds reject as native enumeration does");
    b=unit();b.world[10]=0;ck(!geo::closest(b,{}).valid,"singular native transform rejects");
    b=unit();b.world[1]=1;b.world[5]=0;ck(!geo::closest(b,{}).valid,"parallel transform axes reject");
    b=unit();b.world[3]=std::numeric_limits<float>::infinity();ck(!geo::closest(b,{}).valid,"nonfinite native transform rejects");
    b=unit();b.local[2]=std::numeric_limits<float>::quiet_NaN();ck(!geo::closest(b,{}).valid,"nonfinite bounds reject");
    b=unit();ck(!geo::closest(b,{std::numeric_limits<double>::quiet_NaN(),0,0}).valid,"nonfinite world palm rejects");
    // Independent analytic distance for ordinary orthogonal scaled rotations.
    // The production solver uses convex active faces, not this inverse clamp.
    for(double angle:{0.,.3,1.1,1.5707963267948966})for(double x:{-3.,-.8,.4,3.})for(double y:{-4.,-.2,1.,4.}){
        b=unit();const double c=std::cos(angle),s=std::sin(angle);
        b.world[0]=float(2*c);b.world[1]=float(-3*s);b.world[4]=float(2*s);b.world[5]=float(3*c);
        b.world[3]=10;b.world[7]=20;b.world[11]=30;
        const geo::Point p{10+x,20+y,30+.3};
        const double lx=(c*x+s*y)/2,ly=(-s*x+c*y)/3;
        const double cx=std::clamp(lx,-1.,1.),cy=std::clamp(ly,-1.,1.);
        const geo::Point expected{10+2*c*cx-3*s*cy,20+2*s*cx+3*c*cy,30+.3};
        const auto delta=geo::sub(p,expected);r=geo::closest(b,p);
        ck(r.valid&&near(r.distance,std::sqrt(geo::dot(delta,delta)),1e-6)&&near(r.point.x,expected.x,1e-6)&&near(r.point.y,expected.y,1e-6),"analytic rotated-box regression");
    }
    std::printf("geometry68: %u checks PASS; actual native copied-bounds math, no mouth/eligibility claim\n",checks);
}
