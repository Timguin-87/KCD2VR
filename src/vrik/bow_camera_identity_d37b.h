#pragma once
#include <cstdint>

namespace kcd2vr::bow_camera_identity_d37b {
struct Current {
    const void* manager=nullptr;
    std::uint64_t request=0,epoch=0,preparedRequest=0,bodyEpoch=0,now=0;
    bool prepared=false;
};
struct Identity {
    std::uint64_t key=0,generation=0,request=0;
    explicit operator bool() const noexcept {return key&&generation&&request;}
};
// Shared camera/body presentation foundation only. A grip/socket geometry read
// may be unavailable while this fresh native equipment packet still proves the
// same held bow. No physical grip, weapon action, reload or fire permission is
// created by this classification.
// Equipment is the existing copied PhysEquipment6698; no native read occurs.
template<class Equipment>
inline Identity qualify(const Equipment& e,const Current& current) noexcept {
    if(!current.prepared||!current.preparedRequest||!current.bodyEpoch||
       !e.valid||!e.player||!e.manager||!current.manager||e.manager!=current.manager||
       e.request!=current.preparedRequest||current.preparedRequest!=current.request||
       e.epoch!=current.bodyEpoch||current.bodyEpoch!=current.epoch||
       !e.tick||current.now<e.tick||current.now-e.tick>100||
       !e.handKnown[1]||!e.handKey[1]||e.count<=0||e.count>int(e.items.size()))return {};
    const int index=e.heldIndex[1];
    if(index<0||index>=e.count||index>=int(e.items.size()))return {};
    const auto& item=e.items[index];const auto& profile=item.profile;
    if(!item.held||profile.side!=1||!profile.bow()||!profile.key||
       !profile.generation||profile.key!=e.handKey[1])return {};
    // The existing native equipment producer validated manager+38 == player
    // and player+9D0 == manager before publishing this copied identity. Its
    // current manager, request, body and original source age are checked above.
    return {profile.key,profile.generation,e.request};
}
} // namespace kcd2vr::bow_camera_identity_d37b
