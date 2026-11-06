#pragma once
#include "door_hold65.h"
#include <array>
#include <cmath>
namespace kcd2vr::door_origin_h12 {
// Native point/box input and copied provenance only; never Use/animation authority.
inline bool localPoint(const float* inverse,const float* world,float* out) noexcept {
    if(!inverse||!world||!out)return false;
    for(unsigned n=0;n<12;++n)if(!std::isfinite(inverse[n]))return false;
    for(unsigned n=0;n<3;++n)if(!std::isfinite(world[n]))return false;
    for(unsigned row=0;row<3;++row){const float* m=inverse+4*row;
        out[row]=(world[1]*m[1]+world[0]*m[0])+world[2]*m[2]+m[3];
        if(!std::isfinite(out[row]))return false;}
    return true;
}
inline bool prepare(const float* inverse,const float* nativeWorld,const float* nativeLocal,
                    const float* bounds,const door_hold65::Hand& hand,float* out) noexcept {
    if(!nativeLocal||!bounds)return false;
    for(unsigned n=0;n<6;++n)if(!std::isfinite(bounds[n]))return false;
    for(unsigned n=0;n<3;++n)if(bounds[n]>bounds[n+3])return false;
    float check[3]{};if(!localPoint(inverse,nativeWorld,check))return false;
    for(unsigned n=0;n<3;++n)if(!std::isfinite(nativeLocal[n])||std::fabs(check[n]-nativeLocal[n])>.001f)return false;
    const float palm[3]{hand.point.x,hand.point.y,hand.point.z};if(!localPoint(inverse,palm,out))return false;
    // The selected door keeps its native point when the palm belongs to another
    // nearby interaction. A remote apple Grip cannot redirect this door ray.
    for(unsigned n=0;n<3;++n)if(out[n]<bounds[n]-.25f||out[n]>bounds[n+3]+.25f)return false;
    return true;
}
// Invert only the copied, validated native affine transform. This is a ray
// direction hint, never a fabricated surface or a replacement native bound.
inline bool worldCenter(const float* m,const float* bounds,float* out) noexcept {
    if(!m||!bounds||!out)return false;
    for(unsigned n=0;n<12;++n)if(!std::isfinite(m[n]))return false;
    double v[3]{};for(unsigned n=0;n<3;++n){
        if(!std::isfinite(bounds[n])||!std::isfinite(bounds[n+3])||bounds[n]>bounds[n+3])return false;
        v[n]=(double(bounds[n])+bounds[n+3])*.5-m[n*4+3];}
    const double a=m[0],b=m[1],c=m[2],d=m[4],e=m[5],f=m[6],g=m[8],h=m[9],i=m[10];
    const double det=a*(e*i-f*h)-b*(d*i-f*g)+c*(d*h-e*g);
    if(!std::isfinite(det)||std::fabs(det)<1e-12)return false;
    out[0]=float(((e*i-f*h)*v[0]+(c*h-b*i)*v[1]+(b*f-c*e)*v[2])/det);
    out[1]=float(((f*g-d*i)*v[0]+(a*i-c*g)*v[1]+(c*d-a*f)*v[2])/det);
    out[2]=float(((d*h-e*g)*v[0]+(b*g-a*h)*v[1]+(a*e-b*d)*v[2])/det);
    float check[3]{};if(!localPoint(m,out,check))return false;
    for(unsigned n=0;n<3;++n)if(std::fabs(double(check[n])-(double(bounds[n])+bounds[n+3])*.5)>.001)return false;
    return true;
}
struct Receipt {
    std::uint32_t target=0;std::uintptr_t entity=0,policy=0,nativePlayer=0,frame=0;
    door_hold65::Hand hand{};std::uint64_t tick=0;float local[3]{},distance=0;
};
inline thread_local std::array<Receipt,16> receipts{};
inline thread_local unsigned next=0;
inline void withdraw(std::uint32_t target) noexcept {for(auto& r:receipts)if(r.target==target)r={};}
inline void publish(const Receipt& r) noexcept {
    if(!r.target||!r.tick||!r.entity||!r.frame)return;
    for(auto& old:receipts)if(old.target==r.target){old=r;return;}
    receipts[next++%receipts.size()]=r;
}
// None means this descriptor used the unchanged native origin. A recorded
// substituted native AL0 requires exact same descriptor/frame/source identity.
inline const Receipt* find(std::uint32_t target) noexcept {for(const auto& r:receipts)if(r.target==target)return &r;return nullptr;}
inline bool joins(const Receipt& r,std::uint32_t target,std::uintptr_t entity,std::uintptr_t policy,
                  std::uintptr_t nativePlayer,std::uintptr_t frame,const door_hold65::Hand& h,std::uint64_t now) noexcept {
    return r.target==target&&r.entity==entity&&r.policy==policy&&r.nativePlayer==nativePlayer&&r.frame==frame&&
        door_hold65::fresh(r.tick,now)&&door_hold65::current(r.hand,now)&&door_hold65::current(h,now)&&
        door_hold65::sameSource(r.hand,h)&&r.hand.press&&r.hand.press==h.press&&h.grip>=.30f&&
        r.hand.poseTick==h.poseTick&&r.hand.point.x==h.point.x&&r.hand.point.y==h.point.y&&r.hand.point.z==h.point.z;
}
}
