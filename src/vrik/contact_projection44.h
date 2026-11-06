#pragma once
#include "pose_math.h"
#include <limits>
namespace kcd2vr::contact_projection44 {
using namespace pose_core;
struct Plane {Vec3 normal{};double minimum=0;};
// Nearest feasible wrist against up to seven observed feature planes.
// In 3D a closest point needs at most three independent active constraints.
// This bounded active-set solve avoids order-dependent corner pushes. No IO.
inline bool project(Vec3 requested,const Plane* planes,int count,Vec3& out) noexcept {
    if(!finite(requested)||count<0||count>7)return false;
    auto feasible=[&](Vec3 p){for(int i=0;i<count;++i)
        if(!finite(planes[i].normal)||!std::isfinite(planes[i].minimum)||
           dot(p,planes[i].normal)<planes[i].minimum-1e-7)return false;return finite(p);};
    if(feasible(requested)){out=requested;return true;}
    double best=std::numeric_limits<double>::infinity();bool found=false;
    for(unsigned mask=1;mask<(1u<<count);++mask){
        int ids[3]{},n=0;bool tooMany=false;
        for(int i=0;i<count;++i)if(mask&(1u<<i)){if(n==3){tooMany=true;break;}ids[n++]=i;}
        if(tooMany)continue;
        double a[3][4]{};
        for(int i=0;i<n;++i){for(int j=0;j<n;++j)a[i][j]=dot(planes[ids[i]].normal,planes[ids[j]].normal);
            a[i][n]=planes[ids[i]].minimum-dot(requested,planes[ids[i]].normal);}
        bool singular=false;
        for(int col=0;col<n;++col){int pivot=col;
            for(int i=col+1;i<n;++i)if(std::abs(a[i][col])>std::abs(a[pivot][col]))pivot=i;
            if(std::abs(a[pivot][col])<1e-9){singular=true;break;}
            for(int j=0;j<=n;++j)std::swap(a[pivot][j],a[col][j]);
            const double divisor=a[col][col];for(int j=col;j<=n;++j)a[col][j]/=divisor;
            for(int i=0;i<n;++i)if(i!=col){const double factor=a[i][col];for(int j=col;j<=n;++j)a[i][j]-=factor*a[col][j];}
        }
        if(singular)continue;
        Vec3 candidate=requested;bool positive=true;
        for(int i=0;i<n;++i){if(a[i][n]<-1e-7){positive=false;break;}candidate=candidate+planes[ids[i]].normal*a[i][n];}
        if(!positive||!feasible(candidate))continue;
        const double distance=dot(candidate-requested,candidate-requested);
        if(distance<best){best=distance;out=candidate;found=true;}
    }
    return found;
}
}
