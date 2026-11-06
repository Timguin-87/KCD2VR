#pragma once
#include "core/native_grasp13.h"
#include "projectile_flight_t17.h"
#include "core/snapshot_mailbox.h"
#include "inventory_lifecycle_t8.h"
#include <atomic>
namespace kcd2vr::ammo_carry_t18 {
inline std::atomic<unsigned> hands{0},flights{0};
inline std::atomic<bool> unavailable{false};
enum class Stage : unsigned char { Pending, Held, Releasing, Fault };
struct Claim {
    std::uintptr_t player=0,manager=0,character=0,preview=0;
    std::uint64_t serial=0,key=0,body=0,request=0,load=0,tick=0;
    std::uint32_t entity=0;Stage stage=Stage::Pending;
};
inline core::SnapshotMailbox<Claim> claims[2];
inline bool read(int side,Claim& out) noexcept {
    if(side<0||side>1||!(hands.load(std::memory_order_acquire)&(1u<<side))||!claims[side].read(out)||!out.serial)return false;
    // Cleanup ownership survives missing UI knowledge and loading episodes.
    // Consumers join current player/body/request with owns(), never a timer or
    // UI epoch that would falsely establish native destruction.
    return true;
}
inline bool busy(int side) noexcept {Claim c{};return read(side,c);}
inline bool owns(int side,std::uintptr_t player,std::uint64_t body,std::uint64_t request) noexcept {
    Claim c{};return read(side,c)&&c.player==player&&c.body==body&&c.request==request;
}
inline grasp13::Pose compose(const grasp13::Pose& a,const grasp13::Pose& b) noexcept {
    grasp13::Pose o{};const auto* q=a.q;const auto* r=b.q;
    o.q[0]=q[3]*r[0]+q[0]*r[3]+q[1]*r[2]-q[2]*r[1];
    o.q[1]=q[3]*r[1]-q[0]*r[2]+q[1]*r[3]+q[2]*r[0];
    o.q[2]=q[3]*r[2]+q[0]*r[1]-q[1]*r[0]+q[2]*r[3];
    o.q[3]=q[3]*r[3]-q[0]*r[0]-q[1]*r[1]-q[2]*r[2];
    const float x=b.p[0]*a.scale,y=b.p[1]*a.scale,z=b.p[2]*a.scale;
    const float tx=2*(q[1]*z-q[2]*y),ty=2*(q[2]*x-q[0]*z),tz=2*(q[0]*y-q[1]*x);
    o.p[0]=a.p[0]+x+q[3]*tx+q[1]*tz-q[2]*ty;
    o.p[1]=a.p[1]+y+q[3]*ty+q[2]*tx-q[0]*tz;
    o.p[2]=a.p[2]+z+q[3]*tz+q[0]*ty-q[1]*tx;o.scale=a.scale*b.scale;return o;
}
inline grasp13::Pose inverse(grasp13::Pose p) noexcept {
    grasp13::Pose q{};q.q[0]=-p.q[0];q.q[1]=-p.q[1];q.q[2]=-p.q[2];q.q[3]=p.q[3];q.scale=1/p.scale;
    grasp13::Pose t{};for(int j=0;j<3;++j)t.p[j]=-p.p[j];return compose(q,t);
}
struct Motion {
    grasp13::Pose last{},previous{};std::uint64_t tick=0,prior=0,sample=0;
    bool add(const grasp13::Pose& p,std::uint64_t t,std::uint64_t s) noexcept {
        if(!grasp13::valid(p)||!t||!s||s<=sample||t<=tick)return false;
        previous=last;prior=tick;last=p;tick=t;sample=s;return true;
    }
    bool velocity(std::uint64_t now,float (&v)[3]) const noexcept {
        for(float& x:v)x=0;
        if(!prior||tick<=prior||tick-prior>80||!grasp13::fresh(now,tick,80))return false;
        const float dt=float(tick-prior)*.001f;double speed2=0;
        for(int j=0;j<3;++j){v[j]=(last.p[j]-previous.p[j])/dt;speed2+=double(v[j])*v[j];}
        if(!std::isfinite(speed2)||speed2>64*64)return false;
        const float k=speed2>64?8.f/float(std::sqrt(speed2)):1.f;
        for(float& x:v)x*=k;return true;
    }
};
}
