#pragma once
#include "dialogue_state59c10r2.h"
#include <array>
#include <atomic>

namespace kcd2vr::activity_eye59c19 {
namespace ds=dialogue_state59c10r2;
// Copied only by the exact C_Player::UpdateView callback. Native activity
// lifecycle serials change on Begin/End/Clear, never because a frame was read.
struct State {
    std::uint64_t tick=0,lifetime=0,enteredAt=0;
    std::uintptr_t entity=0;
    std::array<std::uintptr_t,4> instances{};
    unsigned kindMask=0;
    bool known=false,active=false,neutral=false,eligible=false;
};
struct Neutral {
    std::uint64_t tick=0,lifetime=0;
    std::uintptr_t entity=0,character=0;
    ds::V local{};ds::V basis[3]{};
    float scale=1;bool ready=false;
    ds::V headLocalD81{};bool headKnownD81=false;
};
struct Anchor {Neutral eye{};State owner{};};
enum class Reason:unsigned {Applied,Inactive,OwnerUnavailable,OwnerRefused,NoFreshBody,
    NoNeutralEye,LifetimeChanged,BodyChanged,InvalidEye,NativeAlchemy,NativeSharpening};
// Dice has the same model-local eye anchor as other seated activities.
// Admit dice alone; never accidentally admit an overlapping station owner.
inline bool eligibleKindsD81(unsigned kinds) noexcept {
    return kinds==32u||((kinds&13u)&&!(kinds&~13u));
}
inline bool fresh(const State& s,std::uint64_t now) noexcept {
    return s.known&&s.entity&&s.lifetime&&ds::age(s.tick,now,80);
}
// C63: the alchemy passenger camera follows its authored native eye. Parking
// it at the pre-activity eye separates the visible native body by up to 69 cm.
// A nested alchemy book is the same workstation; ordinary Reading is unchanged.
inline bool nativeAlchemy63(const State& s,std::uint64_t now) noexcept {
    return fresh(s,now)&&s.active&&s.eligible&&s.instances[0]&&(s.kindMask&1u)&&!(s.kindMask&~5u);
}
// Sharpening seats Henry within his current model origin. Replaying the
// pre-entry standing eye would keep the view above the grindstone. The exact
// current activity eye supplies that authored seat translation; FOV and HMD
// rotation remain owned by the existing separate composition paths.
inline bool nativeSharpeningR92(const State& s,std::uint64_t now) noexcept {
    return fresh(s,now)&&s.active&&s.eligible&&s.kindMask==8u&&s.instances[3]&&
        !s.instances[0]&&!s.instances[1]&&!s.instances[2];
}
inline bool body(const ds::NativeHead& h,std::uint64_t now) noexcept {
    return ds::fresh(h,now)&&h.modelOriginKnown&&ds::finite(h.modelOrigin);
}
inline bool sameBody(const Neutral& n,const ds::NativeHead& h) noexcept {
    return n.ready&&n.entity==h.entity&&n.character==h.character&&
        std::isfinite(n.scale)&&std::fabs(n.scale-h.scale)<.001f;
}
inline bool learn(const State& s,const ds::NativeHead& h,const ds::Eye& native,
                  std::uint64_t now,Neutral& out) noexcept {
    if(!fresh(s,now)||s.active||!s.neutral||!body(h,now)||s.entity!=h.entity||!ds::valid(native))return false;
    const auto nearHead=ds::sub(native.position,h.world);
    if(!ds::finite(nearHead)||ds::dot(nearHead,nearHead)>.25f)return false;
    // This matrix is captured NOW, separately from the UpdateView state copy.
    // Never backdate it to s.tick: a Start between those publications must not
    // bless a later authored matrix as pre-entry neutral anatomy. A missed
    // entry sample fails closed until a real neutral view can be learned.
    Neutral next{};next.tick=now;next.lifetime=s.lifetime;next.entity=h.entity;next.character=h.character;
    next.local=ds::mul(ds::rotate(ds::conjugate(h.body),ds::sub(native.position,h.modelOrigin)),1.f/h.scale);
    next.headLocalD81=ds::mul(ds::rotate(ds::conjugate(h.body),ds::sub(h.world,h.modelOrigin)),1.f/h.scale);
    next.headKnownD81=ds::finite(next.headLocalD81);
    for(unsigned i=0;i<3;++i)next.basis[i]=ds::rotate(ds::conjugate(h.body),native.basis[i]);
    next.scale=h.scale;next.ready=ds::finite(next.local);if(!next.ready)return false;
    out=next;return true;
}
inline bool sameOwner(const State& a,const State& b) noexcept {
    return a.lifetime==b.lifetime&&a.entity==b.entity&&a.kindMask==b.kindMask&&a.instances==b.instances;
}
inline bool nestedContinuation(const State& previous,const State& next) noexcept {
    // Exactly one witnessed Begin/End and an unchanged still-live parent
    // permits alchemy -> its reading book -> alchemy. An unseen stop/start or
    // same-address allocator reuse (two lifecycle changes) cannot inherit it.
    if(previous.entity!=next.entity||next.lifetime!=previous.lifetime+1)return false;
    for(unsigned i=0;i<previous.instances.size();++i)
        if(previous.instances[i]&&previous.instances[i]==next.instances[i])return true;
    return false;
}
inline Reason latch(const State& s,const ds::NativeHead& h,const Neutral& neutral,
                    std::uint64_t now,Anchor& out) noexcept {
    if(!fresh(s,now))return Reason::OwnerUnavailable;
    if(!s.active){out={};return Reason::Inactive;}
    if(!s.eligible)return Reason::OwnerRefused;
    if(!body(h,now)||s.entity!=h.entity)return Reason::NoFreshBody;
    if(sameBody(out.eye,h)&&(sameOwner(out.owner,s)||nestedContinuation(out.owner,s))){out.owner=s;return Reason::Applied;}
    // First activation must immediately follow the neutral sample. Native
    // activity/dialogue poses can never train this cache, even if close to head.
    if(!neutral.ready||!s.enteredAt||s.enteredAt>now||
       !ds::age(neutral.tick,s.enteredAt,250)||neutral.tick>s.enteredAt||
       s.lifetime!=neutral.lifetime+1)return Reason::NoNeutralEye;
    if(!sameBody(neutral,h))return Reason::BodyChanged;
    out={neutral,s};return Reason::Applied;
}
inline Reason reconstruct(const State& s,const ds::NativeHead& h,const Anchor& a,
                          std::uint64_t now,ds::Eye& out) noexcept {
    if(!fresh(s,now))return Reason::OwnerUnavailable;
    if(!s.active)return Reason::Inactive;
    if(!s.eligible)return Reason::OwnerRefused;
    if(!body(h,now)||s.entity!=h.entity)return Reason::NoFreshBody;
    if(!a.eye.ready)return Reason::NoNeutralEye;
    if(!sameOwner(a.owner,s))return Reason::LifetimeChanged;
    if(!sameBody(a.eye,h))return Reason::BodyChanged;
    ds::Eye next{};next.position=ds::add(h.modelOrigin,ds::rotate(h.body,ds::mul(a.eye.local,h.scale)));
    for(unsigned i=0;i<3;++i)next.basis[i]=ds::rotate(h.body,a.eye.basis[i]);
    if(!ds::valid(next))return Reason::InvalidEye;
    // Only a model-local native eye is retained. Recenter/calibration changes
    // XR origin, not this value. The caller requires CURRENT epoch body data
    // and applies the new recentered HMD/IPD exactly once after this base.
    out=next;return Reason::Applied;
}
// Actual camera-composition policy, including unavailable head publication.
// Evaluate known owner refusal before body availability so diagnostics cannot
// mislabel a forbidden camera/policy as mere head staleness. No cached world
// eye or elapsed-time fallback authorizes a replacement.
inline Reason resolveCurrent(const State& s,const ds::NativeHead& h,bool haveHead,
                             const Anchor& a,std::uint64_t now,ds::Eye& out) noexcept {
    if(!fresh(s,now))return Reason::OwnerUnavailable;
    if(!s.active)return Reason::Inactive;
    if(!s.eligible)return Reason::OwnerRefused;
    if(!haveHead)return Reason::NoFreshBody;
    return reconstruct(s,h,a,now,out);
}
}

// H51 legacy F5 own-head trial (default OFF, INI VrAlchemyOwnHeadH51). Saved FREE VRIK
// alchemy/smithing selects the shared C51 body/camera directly through its current native owner.
namespace kcd2vr::alchemy_own_head_h51 {inline std::atomic<bool> enabled{false};}
