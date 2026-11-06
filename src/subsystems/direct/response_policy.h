#pragma once
#include "../../vrik/pose_math.h"
namespace kcd2vr::response36 {
namespace m=pose_core;
// Reference geometry for OBSERVATION, not the NPC's collider/hitbox. No speed gate:
// a stationary blade may threaten. No damage/block/master outcome is produced.
// Sector is NPC-relative CURRENT blade position, never the native attack zone150.
// Zero/near-center position cannot establish left/right/up/down. Behind is separate.
enum class Sector36 { Unknown, Left, Right, Up, Down, Center };
inline const char* sectorName36(Sector36 s) noexcept {
    switch(s){case Sector36::Left:return "LEFT";case Sector36::Right:return "RIGHT";
    case Sector36::Up:return "UP";case Sector36::Down:return "DOWN";
    case Sector36::Center:return "CENTER";default:return "UNKNOWN";}
}
inline Sector36 sector36(double x,double z) noexcept {
    if(!std::isfinite(x)||!std::isfinite(z))return Sector36::Unknown;
    if(std::hypot(x,z)<.05)return Sector36::Center;
    return std::abs(x)>=std::abs(z)?(x<0?Sector36::Left:Sector36::Right):
        (z>0?Sector36::Up:Sector36::Down);
}
struct Near {bool known=false,inside=false,withinRadius=false;double distance=-1;
    int zone=-1;Sector36 sector=Sector36::Unknown;double lateral=0,vertical=0,depth=0;m::Vec3 point{};};
inline Near proximity(m::Vec3 base,m::Vec3 tip,m::Vec3 torso,m::Vec3 right,m::Vec3 forward,m::Vec3 up,double radius) noexcept {
    Near n{};auto rr=m::unit(right),ff=m::unit(forward),uu=m::unit(up);
    if(!m::finite(base)||!m::finite(tip)||!m::finite(torso)||!rr||!ff||!uu||
       !std::isfinite(radius)||radius<.199999||radius>.600001)return n;
    const auto d=tip-base;const double l2=m::dot(d,d);if(l2<.0025||l2>16)return n;
    const double t=std::clamp(m::dot(torso-base,d)/l2,0.,1.);n.point=base+d*t;
    const auto delta=n.point-torso;n.distance=m::length(delta);n.known=std::isfinite(n.distance);
    n.withinRadius=n.known&&n.distance<=radius;
    n.depth=m::dot(delta,*ff);n.inside=n.withinRadius&&n.depth>=-.05; // preserve legacy front candidate
    const double x=m::dot(delta,*rr),z=m::dot(delta,*uu);
    n.lateral=x;n.vertical=z;n.sector=sector36(x,z);
    n.zone=z>.15&&z>std::abs(x)?0:x<-.08?1:x>.08?2:5;return n;
}
inline bool attackPhase(int phase) noexcept {return phase==4||phase==8;}
inline bool opposite(int a,int b) noexcept {return (a==0&&b==5)||(a==5&&b==0)||(a==1&&b==2)||(a==2&&b==1);}
// Classification only. A phase transition is not proof of causal interruption,
// and a combo window closing is not proof of a broken attack chain.
inline bool reactionTransition(int before,int after,int dispatch) noexcept {return dispatch==1&&attackPhase(before)&&after==64;}
inline bool comboWindowClosed(int before,int after) noexcept {return before==1&&after==0;}
inline bool fresh(std::uint64_t start,std::uint64_t now) noexcept {return start&&now>=start&&now-start<=500;}
}
