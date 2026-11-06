#pragma once
#include <cmath>
#include <cstdint>

// Discovery remains available BEFORE Grip. A closed-Grip-only query would miss
// a fast squeeze because CG1 deliberately consumes an already qualified target.
// This budget therefore removes idle per-frame work, not the input contract.
namespace kcd2vr::corpse_search_d85r5r2 {
inline constexpr std::uint64_t kSearchMs=100;
inline constexpr double kBoundsMargin=.65; // 45 cm nomination + 20 cm render-bound tolerance
inline bool due(std::uint64_t last,std::uint64_t now) noexcept {
    return !last||now<last||now-last>=kSearchMs;
}
inline bool deadlineDue(std::uint64_t deadline,std::uint64_t now) noexcept {
    return !deadline||now>=deadline;
}

// Bounds are a read-only broad-phase rejection, never a grab authority. A
// missing/degenerate/nonfinite render box MUST leave the existing exact part
// search available. All eight corners handle rotated and scaled characters.
inline bool outsideWorldBounds(const float* bounds,const float* world,
    double x,double y,double z) noexcept {
    if(!bounds||!world||!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z))return false;
    for(int i=0;i<6;++i)if(!std::isfinite(bounds[i]))return false;
    for(int i=0;i<12;++i)if(!std::isfinite(world[i]))return false;
    const double determinant=
        double(world[0])*(double(world[5])*world[10]-double(world[6])*world[9])-
        double(world[1])*(double(world[4])*world[10]-double(world[6])*world[8])+
        double(world[2])*(double(world[4])*world[9]-double(world[5])*world[8]);
    if(!std::isfinite(determinant)||std::fabs(determinant)<1e-9)return false;
    double extent=0;
    for(int a=0;a<3;++a){
        const double d=double(bounds[a+3])-bounds[a];
        if(d<0||d>20)return false;
        extent+=d;
    }
    if(extent<.05)return false;
    double lo[3]{},hi[3]{};
    for(int corner=0;corner<8;++corner){
        const double p[3]{bounds[(corner&1)?3:0],bounds[(corner&2)?4:1],bounds[(corner&4)?5:2]};
        for(int a=0;a<3;++a){
            const int row=4*a;
            const double v=world[row]*p[0]+world[row+1]*p[1]+world[row+2]*p[2]+world[row+3];
            if(!std::isfinite(v))return false;
            if(!corner||v<lo[a])lo[a]=v;
            if(!corner||v>hi[a])hi[a]=v;
        }
    }
    const double hand[3]{x,y,z};double d2=0;
    for(int a=0;a<3;++a){const double d=hand[a]<lo[a]?lo[a]-hand[a]:hand[a]>hi[a]?hand[a]-hi[a]:0;d2+=d*d;}
    return d2>kBoundsMargin*kBoundsMargin;
}
}
