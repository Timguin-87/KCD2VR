#include "core/native_door_fingers66.h"
#include <cassert>
#include <cstdio>
#include <limits>
namespace d=kcd2vr::door_fingers66;
namespace m=kcd2vr::pose_core;
static unsigned checks=0;
static void check(bool value){++checks;assert(value);}
static m::Quat axis(m::Vec3 direction,double angle){const auto v=m::unit(direction);assert(v);
    const double s=std::sin(angle*.5);return {v->x*s,v->y*s,v->z*s,std::cos(angle*.5)};}
static m::Quat negative(m::Quat q){return {-q.x,-q.y,-q.z,-q.w};}
static d::References references(m::Vec3 direction){d::References r;r.ready=true;r.layout.joints={3,3,3,3,3};
    for(unsigned i=0;i<d::count(r.layout);++i){r.open[i]=axis({0,0,1},.03*(i+1));
        r.closed[i]=m::multiply(axis(direction,.4+.02*i),r.open[i]);r.rest[i]={0,.01+.001*i,0};}return r;}
int main(){
    const auto from=references({1,0,0}),to=references({0,-1,0});
    d::NativeShape native;native.side=0;native.layout=from.layout;
    const unsigned n=d::count(native.layout);check(n==15);
    for(double t:{0.,.2,.55,1.}){
        for(unsigned i=0;i<n;++i)native.local[i]=*m::slerp(from.open[i],from.closed[i],t);
        d::Retargeted result;check(d::retarget(native,1,from,to,result)&&result.count==n&&!result.exact);
        for(unsigned i=0;i<n;++i){const auto expected=*m::slerp(to.open[i],to.closed[i],t);
            check(m::angle(result.local[i].rotation,expected)<1e-7);
            check(m::length(result.local[i].fixed_offset-to.rest[i])<1e-12);}
        // Quaternion signs cannot change the curl or the retargeted shape.
        auto signedNative=native;auto signedFrom=from;for(unsigned i=0;i<n;++i){
            signedNative.local[i]=negative(native.local[i]);signedFrom.open[i]=negative(from.open[i]);}
        d::Retargeted signedResult;check(d::retarget(signedNative,1,signedFrom,to,signedResult));
        for(unsigned i=0;i<n;++i)check(m::angle(result.local[i].rotation,signedResult.local[i].rotation)<1e-7);
    }
    // Same-side contact retains the full observed native shape, including an
    // off-axis component which opposite-hand scalar retargeting cannot copy.
    for(unsigned i=0;i<n;++i)native.local[i]=m::multiply(axis({0,1,0},.13),*m::slerp(from.open[i],from.closed[i],.6));
    d::Retargeted exact;check(d::retarget(native,0,{},from,exact)&&exact.exact);
    for(unsigned i=0;i<n;++i){check(m::angle(exact.local[i].rotation,native.local[i])<1e-7);
        check(m::length(exact.local[i].fixed_offset-from.rest[i])<1e-12);}
    // Frame changes on the SOURCE side do not transplant that frame's axes to
    // the destination: the scalar projection is conjugation invariant.
    const auto rotate=axis({1,2,3},.8);const auto undo=m::conjugate(rotate);
    for(double t:{.1,.7}){const auto o=axis({1,0,0},.2),c=m::multiply(axis({0,1,0},.9),o),v=*m::slerp(o,c,t);
        auto transform=[&](m::Quat q){return m::multiply(m::multiply(rotate,q),undo);};
        const auto a=d::curl(o,c,v),b=d::curl(transform(o),transform(c),transform(v));
        check(a&&b&&std::fabs(*a-*b)<1e-9&&std::fabs(*a-t)<1e-9);}
    check(d::curl({}, {}, {})==0.);
    check(!d::curl({}, {}, axis({1,0,0},.2)));
    check(d::curl({},axis({1,0,0},.5),axis({1,0,0},.9))==1.);
    check(d::curl({},axis({1,0,0},.5),axis({1,0,0},-.3))==0.);
    for(int bad=0;bad<10;++bad){auto s=native;auto f=from,t=to;int side=1;
        if(bad==0)s.side=-1;if(bad==1)side=2;if(bad==2)t.ready=false;if(bad==3)f.ready=false;
        if(bad==4)t.layout.joints[1]=2;if(bad==5)f.layout.joints[2]=4;if(bad==6)s.local[3]={0,0,0,0};
        if(bad==7)t.rest[4].x=std::numeric_limits<double>::infinity();
        if(bad==8)f.closed[0].y=std::numeric_limits<double>::quiet_NaN();if(bad==9)t.closed[1]={0,0,0,0};
        d::Retargeted rejected;rejected.count=99;check(!d::retarget(s,side,f,t,rejected)&&!rejected.count);}
    // Invalid late joints cannot leak a partially constructed output.
    auto invalid=native;invalid.local[n-1].x=std::numeric_limits<double>::quiet_NaN();
    d::Retargeted rejected;check(!d::retarget(invalid,0,{},from,rejected)&&rejected.count==0);
    std::printf("native_door_fingers66: %u checks PASS; native-shape retarget math only, no lease/handle authority\n",checks);
}
