#pragma once
#include "subsystems/throw_drop/hand_claim_t24.h"
#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
// Standalone consumers already model their native player/model/entity memory.
// Supply only current item-manager/vft/backlink leaves to the EXACT extracted
// private helper. No constant-Free substitute, copied policy or native write.
static std::atomic<void*> g_committed_character6689{nullptr};
namespace hand_claim_fixture_t24 {
inline void* player=nullptr;
inline std::array<unsigned char,0x60> manager{};
inline std::atomic<void*> g_henry_item_attach_manager{nullptr};
inline std::uintptr_t character=0;inline std::uint64_t body=0,request=0;
inline unsigned nativeReads=0;
inline bool identityKnown=true;
static bool PhysVft6698(void* p,std::uintptr_t expected) noexcept {
 ++nativeReads;return identityKnown&&p&&((p==player&&expected==0x3AA4B98)||(p==manager.data()&&expected==0x3EC9958));
}
static bool Read(const void* address,void*& value) noexcept {
 ++nativeReads;if(!identityKnown)return false;
 const auto a=reinterpret_cast<std::uintptr_t>(address);
 if(a==reinterpret_cast<std::uintptr_t>(manager.data())+0x38){value=player;return true;}
 if(a==reinterpret_cast<std::uintptr_t>(player)+0x9D0){value=manager.data();return true;}
 return false;
}
#include "generated_hand_claim_t24.inc"
inline void Reset(void* p,std::uintptr_t c,std::uint64_t b=1,std::uint64_t r=1) {
 assert(p&&c&&b&&r);player=p;character=c;body=b;request=r;nativeReads=0;identityKnown=true;
 g_henry_item_attach_manager=manager.data();g_committed_character6689=reinterpret_cast<void*>(c);
 assert(kcd2vr::ammo_carry_t18::claims[0].publish({})&&kcd2vr::ammo_carry_t18::claims[1].publish({}));
 kcd2vr::ammo_carry_t18::hands=0;
}
inline void Publish(int side,kcd2vr::ammo_carry_t18::Stage stage=kcd2vr::ammo_carry_t18::Stage::Held) {
 assert(side>=0&&side<2);kcd2vr::ammo_carry_t18::Claim c{};c.serial=1;c.player=reinterpret_cast<std::uintptr_t>(player);
 c.manager=reinterpret_cast<std::uintptr_t>(manager.data());c.character=character;c.body=body;c.request=request;c.stage=stage;
 assert(kcd2vr::ammo_carry_t18::claims[side].publish(c));kcd2vr::ammo_carry_t18::hands.fetch_or(1u<<side);
}
}
using hand_claim_fixture_t24::HandClaimBlockedT24;
