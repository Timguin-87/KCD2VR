#pragma once
// One bounded free-hand contact episode. This is presentation geometry, never
// a native rigid body or gameplay hit. The pose adapter owns staging/commit.
#include "hand_shape59c7f.h"
#include <cstdint>

namespace kcd2vr::hand_pair_contact_d39 {
namespace m=pose_core;
namespace shape=hand_shape59c7f;
struct Key {
    std::uintptr_t character=0,skeleton=0;
    std::uint64_t poseEpoch=0,trackingEpoch=0,policy=0,contactControl=0,frameControl=0,nativeAdmission=0;
    unsigned geometry=0;
};
inline bool same(const Key& a,const Key& b) noexcept {
    return a.character==b.character&&a.skeleton==b.skeleton&&a.poseEpoch==b.poseEpoch&&
        a.trackingEpoch==b.trackingEpoch&&a.policy==b.policy&&a.contactControl==b.contactControl&&
        a.frameControl==b.frameControl&&a.nativeAdmission==b.nativeAdmission&&a.geometry==b.geometry;
}
struct Input {
    Key key{};
    std::uint64_t sample=0,tick=0,timeNs=0,now=0;
    m::Quat basis{}; // calibrated tracking-to-model rotation, not an elbow/bone
    bool eligible=false;
};
struct State {
    Key key{};
    std::uint64_t sample=0,tick=0,timeNs=0;
    m::Quat basis{};
    m::Vec3 normal{},sampleReference{},relative{};
    shape::Shape shape[2]{};
    bool touching=false,haveSampleReference=false;
};
struct Staged {shape::Result contact{};State state{};bool continued=false,duplicate=false;};
inline bool validShape(const shape::Shape& s) noexcept {
    if(!s.valid||!s.count||s.count>s.part.size())return false;
    for(unsigned n=0;n<s.count;++n){const auto& p=s.part[n];
        if(!m::finite(p.a)||!m::finite(p.b)||!std::isfinite(p.radius)||
           p.radius<=0||p.radius>.15||m::length(p.a)>.35||m::length(p.b)>.35)return false;
    }
    return true;
}
inline bool geometryContinuous(const shape::Shape& old,const shape::Shape& next,m::Quat transport) noexcept {
    if(!validShape(old)||old.count!=next.count)return false;
    for(unsigned n=0;n<old.count;++n){
        if(std::abs(old.part[n].radius-next.part[n].radius)>.001||
           m::length(m::rotate(transport,old.part[n].a)-next.part[n].a)>.05||
           m::length(m::rotate(transport,old.part[n].b)-next.part[n].b)>.05)return false;
    }
    return true;
}
// Pure staging: unsuccessful body transactions cannot overwrite a committed
// witness. Equal XR samples reuse the reference with which that sample began;
// repeated native consumers never integrate another normal update.
inline Staged stage(const State& previous,const Input& in,const shape::Shape& rightShape,
                    const shape::Shape& leftShape,m::Vec3 right,m::Vec3 left) noexcept {
    Staged out{};const auto basis=m::unit(in.basis);
    if(!in.eligible||!in.key.character||!in.key.skeleton||!in.key.poseEpoch||!in.key.trackingEpoch||
       !in.key.frameControl||(in.key.policy&1)||!in.sample||!in.tick||in.now<in.tick||in.now-in.tick>80||
       !basis||!validShape(rightShape)||!validShape(leftShape)||!m::finite(right)||!m::finite(left))return out;
    const auto relative=m::rotate(m::conjugate(*basis),right-left);
    const auto transport=m::multiply(*basis,m::conjugate(previous.basis));
    const bool identity=previous.touching&&same(previous.key,in.key);
    // A reversed or inconsistently repeated source stamp is not a new
    // controller observation and cannot establish another contact episode.
    if(identity&&(in.sample<previous.sample||in.tick<previous.tick||
       (in.timeNs&&previous.timeNs&&in.timeNs<previous.timeNs)||
       (in.sample==previous.sample&&(in.tick!=previous.tick||in.timeNs!=previous.timeNs))))return out;
    const bool monotonic=identity&&in.sample>=previous.sample&&in.tick>=previous.tick&&
        (!in.timeNs||!previous.timeNs||in.timeNs>=previous.timeNs);
    const bool duplicate=monotonic&&in.sample==previous.sample&&in.tick==previous.tick&&in.timeNs==previous.timeNs;
    const bool continuous=monotonic&&(duplicate||in.sample>previous.sample)&&in.tick-previous.tick<=80&&
        (!in.timeNs||!previous.timeNs||in.timeNs-previous.timeNs<=80000000ull)&&
        m::length(relative-previous.relative)<=.05&&geometryContinuous(previous.shape[0],rightShape,transport)&&
        geometryContinuous(previous.shape[1],leftShape,transport);
    std::optional<m::Vec3> reference;
    if(continuous){
        if(duplicate){if(previous.haveSampleReference)reference=m::unit(m::rotate(transport,previous.sampleReference));}
        else reference=m::unit(m::rotate(transport,previous.normal));
    }
    out.contact=shape::separate(rightShape,leftShape,right,left,reference);
    out.continued=continuous&&bool(reference);out.duplicate=duplicate;
    if(!out.contact.valid||!out.contact.contact)return out; // clear separation retires the episode
    const auto normal=m::unit(out.contact.correction[0]);if(!normal)return out;
    auto& next=out.state;next.key=in.key;next.sample=in.sample;next.tick=in.tick;next.timeNs=in.timeNs;
    next.basis=*basis;next.normal=*normal;next.relative=relative;next.shape[0]=rightShape;next.shape[1]=leftShape;
    next.haveSampleReference=bool(reference);if(reference)next.sampleReference=*reference;
    next.touching=true;
    return out;
}
// Commit only after restored native pose and both race-free packets succeeded,
// and the final policy/owner checks still agree. A failed attempt cannot donate
// staged history to the next callback.
inline void commit(State& current,const State& staged,bool published,bool ownerCurrent) noexcept {
    if(published&&ownerCurrent)current=staged;
}
}
