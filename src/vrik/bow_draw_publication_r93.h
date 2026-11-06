#pragma once
#include "pose_math.h"
// Visual draw data travels inside the completed wrists. Never join the latest
// physical mailbox to a different completed body or renew an older sample.
namespace kcd2vr::bow_draw_publication_r93 {
namespace m=kcd2vr::pose_core;
struct Source {
    m::Stamp stamp{};
    std::uint64_t request=0,key=0,generation=0;
    m::Vec3 weaponPosition{};m::Quat weaponRotation{};
    double fraction=0,drawLength=0;unsigned geometryRank=0;float rightGrip=0;
    bool valid=false,physical=false,held=false;
};
template<class Input,class Output,class Profile>
inline Source prepare(const Input& i,const Output& o,const Profile& p,
    m::Stamp body,std::uint64_t request,bool coherent,bool physical) noexcept {
    Source s{};
    if(!coherent||!request||i.epoch!=request||!body.actor||!body.epoch||!body.sample||!body.tick||
       i.bodyEpoch!=body.epoch||i.sample!=body.sample||i.tick!=body.tick||
       !i.allowed||!i.tracked[0]||!i.tracked[1]||!p.bow()||!p.key||!p.generation||!o.active[1]||
       !m::finite(o.weapons[1].p)||!m::unit(o.weapons[1].q))return s;
    s.stamp=body;s.request=request;s.key=p.key;s.generation=p.generation;
    s.weaponPosition=o.weapons[1].p;s.weaponRotation=o.weapons[1].q;
    s.physical=physical;s.held=o.bowHeld;s.fraction=o.bowFraction;
    s.drawLength=p.bowDrawLength166>=.12?p.bowDrawLength166:std::clamp(i.reach[0]*.85,.35,.65);
    s.geometryRank=p.bowGeometryRank48;s.rightGrip=float(i.grip[0]);
    s.valid=!physical||(std::isfinite(s.fraction)&&s.fraction>=0&&s.fraction<=1);
    return s;
}
inline bool current(const Source& s,m::Stamp body,std::uint64_t request,std::uint64_t now) noexcept {
    return s.valid&&request&&s.request==request&&s.stamp.actor==body.actor&&
        s.stamp.epoch==body.epoch&&s.stamp.sample==body.sample&&s.stamp.tick==body.tick&&
        body.tick&&now>=body.tick&&now-body.tick<=80;
}
}
