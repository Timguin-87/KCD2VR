#pragma once
// Pure distance to retail's copied LOCAL bounds transformed by its actual
// WORLD Matrix34. No native call, target selection, geometry lookup or polling.
// These are native selection bounds, not a collision mesh, authored hand grip
// or vessel mouth.
#include <algorithm>
#include <cmath>
#include <limits>

namespace kcd2vr::subsystems::alchemy::geometry68 {
struct Point {double x=0,y=0,z=0;};
struct Bounds {
    float local[6]{}; // native AABB: minXYZ, maxXYZ (24 bytes)
    float world[12]{}; // native Matrix34 rows: [m00 m01 m02 tx] ...
};
struct Result {bool valid=false;Point point{};double distance=0;};
inline bool finite(Point p) noexcept {return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);}
inline double dot(Point a,Point b) noexcept {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline Point add(Point a,Point b) noexcept {return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline Point sub(Point a,Point b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline Point mul(Point a,double x) noexcept {return {a.x*x,a.y*x,a.z*x};}
inline Point cross(Point a,Point b) noexcept {return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
inline Point transform(const Bounds& b,Point p) noexcept {
    return {b.world[0]*p.x+b.world[1]*p.y+b.world[2]*p.z+b.world[3],
            b.world[4]*p.x+b.world[5]*p.y+b.world[6]*p.z+b.world[7],
            b.world[8]*p.x+b.world[9]*p.y+b.world[10]*p.z+b.world[11]};
}
inline bool valid(const Bounds& b) noexcept {
    for(float f:b.local)if(!std::isfinite(f))return false;
    for(float f:b.world)if(!std::isfinite(f))return false;
    bool volume=false;
    for(int j=0;j<3;++j){if(b.local[j]>b.local[j+3])return false;volume=volume||b.local[j]<b.local[j+3];}
    if(!volume)return false; // native candidate loop also rejects a point AABB
    const Point a{b.world[0],b.world[4],b.world[8]},c{b.world[1],b.world[5],b.world[9]},d{b.world[2],b.world[6],b.world[10]};
    const double scale=std::sqrt(dot(a,a)*dot(c,c)*dot(d,d));
    return std::isfinite(scale)&&scale>0&&std::fabs(dot(a,cross(c,d)))>scale*1e-10;
}
// Exact convex-box closest point by the 27 active-face combinations. Solving
// the free coordinates in WORLD's Gram matrix also handles native nonuniform
// scale and shear; inverse-transform/clamp alone would give a wrong distance.
// Fixed work, stack storage, no allocation. Singular free sets are skipped;
// lower-dimensional faces still represent planar/line-shaped local bounds.
inline Result closest(const Bounds& b,Point p) noexcept {
    Result out{};if(!valid(b)||!finite(p))return out;
    Point centerLocal{},half{};
    centerLocal={(double(b.local[0])+b.local[3])*.5,(double(b.local[1])+b.local[4])*.5,(double(b.local[2])+b.local[5])*.5};
    half={(double(b.local[3])-b.local[0])*.5,(double(b.local[4])-b.local[1])*.5,(double(b.local[5])-b.local[2])*.5};
    const Point center=transform(b,centerLocal),q=sub(p,center);
    const Point axes[3]{{b.world[0]*half.x,b.world[4]*half.x,b.world[8]*half.x},
                        {b.world[1]*half.y,b.world[5]*half.y,b.world[9]*half.y},
                        {b.world[2]*half.z,b.world[6]*half.z,b.world[10]*half.z}};
    double best=std::numeric_limits<double>::infinity();
    for(int mask=0;mask<27;++mask){
        int code=mask,n=0,free[3]{};double coordinates[3]{};Point residual=q;
        for(int j=0;j<3;++j){const int state=code%3;code/=3;
            if(state==1)free[n++]=j;
            else {coordinates[j]=state==0?-1.:1.;residual=sub(residual,mul(axes[j],coordinates[j]));}
        }
        double equations[3][4]{},largest=0;
        for(int row=0;row<n;++row){
            for(int col=0;col<n;++col){equations[row][col]=dot(axes[free[row]],axes[free[col]]);largest=(std::max)(largest,std::fabs(equations[row][col]));}
            equations[row][n]=dot(axes[free[row]],residual);
        }
        bool usable=true;
        for(int col=0;col<n&&usable;++col){
            int pivot=col;for(int row=col+1;row<n;++row)if(std::fabs(equations[row][col])>std::fabs(equations[pivot][col]))pivot=row;
            if(largest==0||std::fabs(equations[pivot][col])<=largest*1e-12){usable=false;break;}
            if(pivot!=col)for(int j=col;j<=n;++j)std::swap(equations[col][j],equations[pivot][j]);
            const double divisor=equations[col][col];for(int j=col;j<=n;++j)equations[col][j]/=divisor;
            for(int row=0;row<n;++row)if(row!=col){const double factor=equations[row][col];for(int j=col;j<=n;++j)equations[row][j]-=factor*equations[col][j];}
        }
        for(int row=0;row<n&&usable;++row){const double value=equations[row][n];
            if(!std::isfinite(value)||value< -1.-1e-8||value>1.+1e-8)usable=false;
            else coordinates[free[row]]=std::clamp(value,-1.,1.);
        }
        if(!usable)continue;
        Point point=center;for(int j=0;j<3;++j)point=add(point,mul(axes[j],coordinates[j]));
        const Point gap=sub(p,point);const double squared=dot(gap,gap);
        if(finite(point)&&std::isfinite(squared)&&squared<best){best=squared;out.point=point;out.valid=true;}
    }
    if(out.valid)out.distance=std::sqrt((std::max)(0.,best));return out;
}
// Bounded WORLD ray against the exact copied affine native selection bounds.
// Parameter t remains WORLD metres because direction is normalized before the
// inverse transform. No centre point or guessed vessel mouth is substituted.
inline Result ray(const Bounds& b,Point origin,Point direction,double limit=2.) noexcept {
 Result out{};if(!valid(b)||!finite(origin)||!finite(direction)||!std::isfinite(limit)||limit<=0)return out;
 const double norm=std::sqrt(dot(direction,direction));if(!std::isfinite(norm)||norm<.99||norm>1.01)return out;
 direction=mul(direction,1./norm);
 const Point a{b.world[0],b.world[4],b.world[8]},c{b.world[1],b.world[5],b.world[9]},d{b.world[2],b.world[6],b.world[10]};
 const double det=dot(a,cross(c,d));if(!std::isfinite(det)||std::fabs(det)<1.e-12)return out;
 const auto inverse=[&](Point v){return Point{dot(v,cross(c,d))/det,dot(v,cross(d,a))/det,dot(v,cross(a,c))/det};};
 const Point o=inverse(sub(origin,{b.world[3],b.world[7],b.world[11]})),v=inverse(direction);
 const double os[3]{o.x,o.y,o.z},vs[3]{v.x,v.y,v.z};double enter=0,leave=limit;
 for(int axis=0;axis<3;++axis){if(std::fabs(vs[axis])<1.e-12){if(os[axis]<b.local[axis]||os[axis]>b.local[axis+3])return out;}
  else{double lo=(b.local[axis]-os[axis])/vs[axis],hi=(b.local[axis+3]-os[axis])/vs[axis];if(lo>hi)std::swap(lo,hi);
   enter=(std::max)(enter,lo);leave=(std::min)(leave,hi);if(enter>leave)return out;}}
 out.valid=true;out.distance=enter;out.point=add(origin,mul(direction,enter));return out;
}

}
