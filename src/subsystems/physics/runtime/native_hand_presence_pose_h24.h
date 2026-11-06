#pragma once
#include "native_hand_proxy_owner60.h"
namespace kcd2vr::physics::hand_presence_h24 {
using Pose=native_hand_proxy_t4::Pose;
// Context is a copy of existing atomic/semantic owners. It grants no native
// pointer authority; protected phase validation still owns body lifetime.
struct Context {bool enabled=true,calibrated=false,gameplay=false,tracked=false;
    bool menu=false,dialogue=false,dialoguePresence=false,cutscene=false,reading=false,scripted=false,nativeBody=false,nativeReference=false,grabBusy=false;
    std::uint64_t epoch=0,cancel=0,trackingEpoch=0,nativeUseUntil=0;
    std::uintptr_t character=0,entity=0;
};
inline bool current(const native_hand_proxy_t4::Input& in,std::uint64_t now,const Context& c) noexcept {
    const bool context=(c.gameplay&&!c.dialogue)||(c.dialogue&&c.dialoguePresence);
    return c.enabled&&c.calibrated&&context&&c.tracked&&!c.menu&&!c.cutscene&&!c.reading&&!c.scripted&&
        !c.nativeBody&&!c.nativeReference&&!c.grabBusy&&now>=c.nativeUseUntil&&
        native_hand_proxy_t4::ProxyPolicy::validInput(in,now)&&in.source.physicalPresenceH24&&
        in.source.sourceGeneration==c.epoch&&in.source.characterH24==c.character&&in.playerEntityH24==c.entity&&
        !(in.source.cancelGeneration&1)&&in.source.cancelGeneration==c.cancel&&in.source.trackingEpoch==c.trackingEpoch;
}
inline pose_core::Quat wristRotation(bool physicalOwned,const Pose& measured,pose_core::Quat controllerOrNative) noexcept {
    return physicalOwned?measured.q:controllerOrNative;
}
inline bool allowed(const native_hand_proxy_t4::Input& raw,const hand_proxy60::PresenceFeedbackH24& measured,
    std::uint64_t now,const Pose& modelWorld,double scale,pose_core::Vec3 shoulder,double reach,Pose& local) noexcept {
    namespace pc=pose_core;
    const auto normalized=[](pc::Quat q){const double n=q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;return std::isfinite(n)&&std::abs(n-1)<.01;};
    if(!native_hand_proxy_t4::ProxyPolicy::validInput(raw,now)||!raw.source.physicalPresenceH24||
       !measured.valid||!measured.incarnation||!(measured.source==raw.source)||!measured.tick||
       now<measured.tick||now-measured.tick>80||!measured.source.poseTick||now<measured.source.poseTick||now-measured.source.poseTick>80||
       !std::isfinite(scale)||scale<.1||scale>10||!pc::finite(measured.pose.p)||
       !normalized(measured.pose.q)||!pc::finite(modelWorld.p)||!normalized(modelWorld.q)||
       pc::length(measured.pose.p-raw.target.p)>.65*scale)return false;
    const auto inverse=pc::conjugate(modelWorld.q);
    local={pc::multiply(inverse,measured.pose.q),pc::rotate(inverse,measured.pose.p-modelWorld.p)*(1./scale)};
    return pc::finite(local.p)&&std::isfinite(reach)&&reach>0&&pc::length(local.p-shoulder)<=reach;
}
}
