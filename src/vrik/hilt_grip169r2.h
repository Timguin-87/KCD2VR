#pragma once
#include "support_grip166.h"
namespace kcd2vr::hilt169r2 {
namespace m=pose_core;
struct Region {m::Vec3 a{},b{};bool valid=false;};
// A conservative bounded HANDLE estimate for an already qualified longsword
// grip. Model bounds are not a pommel socket or surface mesh. No blade/shaft
// range and no fixed +8 cm extension. Fall back to the existing socket if the
// current profile cannot establish this interval.
inline Region qualify(m::Vec3 primary,m::Vec3 tip,m::Vec3 support,
        const double* lo,const double* hi) noexcept {
    Region r{};
    if(!lo||!hi||!support_grip166::OnHilt(primary,tip,support))return r;
    const auto d=m::unit(support-primary),blade=m::unit(tip-primary);
    if(!d||!blade)return r;
    double end=.22;
    const double p[]{primary.x,primary.y,primary.z},v[]{d->x,d->y,d->z};
    for(int i=0;i<3;++i){
        if(!std::isfinite(lo[i])||!std::isfinite(hi[i])||lo[i]>hi[i]||p[i]<lo[i]||p[i]>hi[i])return r;
        if(v[i]>1e-8)end=std::min(end,(hi[i]-p[i])/v[i]-.005);
        else if(v[i]<-1e-8)end=std::min(end,(lo[i]-p[i])/v[i]-.005);
    }
    const double behind=-m::dot(*d,*blade),side=m::length(*d+*blade*behind);
    if(behind<=0)return r;
    end=std::min(end,.22/behind);if(side>1e-8)end=std::min(end,.095/side);
    if(end<.06)return r;
    r.a=primary+*d*std::max(.035,.026/behind);r.b=primary+*d*end;
    r.valid=m::length(r.b-r.a)>=.02&&support_grip166::OnHilt(primary,tip,r.a)&&support_grip166::OnHilt(primary,tip,r.b);
    return r;
}
inline std::optional<m::Vec3> select(const Region& r,m::Vec3 hand,double radius=.14) noexcept {
    if(!r.valid||!m::finite(r.a)||!m::finite(r.b)||!m::finite(hand)||!std::isfinite(radius)||radius<=0)return {};
    const auto span=r.b-r.a;const double len=m::length(span);if(len<.02||len>.25)return {};
    const auto axis=span*(1/len);const double along=m::dot(hand-r.a,axis);
    // Radial tolerance must not create a spherical acquisition zone on blade
    // or beyond the pommel estimate. Longitudinal tolerance is only 10 mm.
    if(along<-.01||along>len+.01)return {};
    const auto nearest=r.a+axis*std::clamp(along,0.0,len);
    if(m::length(hand-nearest)>radius)return {};
    return nearest;
}
inline bool contains(const Region& r,m::Vec3 p) noexcept {
    const auto selected=select(r,p,.001);return selected&&m::length(*selected-p)<.001;
}
}
