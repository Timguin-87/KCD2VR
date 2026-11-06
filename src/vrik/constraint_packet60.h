#pragma once
#include <cstdint>
#include <cstring>
#include <type_traits>
// DS-F43/F46/F57: the retail pe_action_add_constraint packet builder. Pure,
// host-tested. The values are the PROVEN default-ctor bytes Claude read from
// B35294..B35337: type=5, id sentinel 0x80000000, pBuddy=0, float unset
// sentinel 0xFFBFFFFF at the PROVEN pt x-slots (+0x10/+0x1C) plus the other
// sentinel-tested floats (a defensive SENTINEL-FILL choice, not ctor bytes -
// the ctor's array callbacks are 5F5DA0 no-ops), partid sentinels, flags
// default 2, terminal qword all-ones. Zero-filling is WRONG. Unproven
// offsets stay prior storage in this fresh buffer and are never described as
// known. 0x88 is now PROVEN the exact queue copy size (Claude's
// action-copy-size answer: 0x51DCA30 table, type 5 = 136).
// DS-F46 (Codex F39 review): alignas(8) storage and memcpy-based scalar
// accessors - no unaligned or type-aliasing reinterpret_cast.
namespace kcd2vr::constraint_packet60 {
inline constexpr std::size_t kSize=0x88;
inline constexpr std::uint32_t kFloatUnset=0xFFBFFFFFu; // NaN pattern
inline constexpr std::uint32_t kIntUnset=0x80000000u;
inline constexpr int kTypeAddConstraint=5;
struct alignas(8) Packet {unsigned char bytes[kSize]{};};
template<class T,class P> static void put(P& p,std::size_t o,const T& v) noexcept {
    static_assert(std::is_trivially_copyable<T>::value&&sizeof(T)<=8,"scalar accessor");
    std::memcpy(p.bytes+o,&v,sizeof(T));
}
template<class T,class P> static T get(const P& p,std::size_t o) noexcept {
    static_assert(std::is_trivially_copyable<T>::value&&sizeof(T)<=8,"scalar accessor");
    T v{};std::memcpy(&v,p.bytes+o,sizeof(T));return v;
}
inline Packet defaults() noexcept {
    Packet p{};
    put<std::uint32_t>(p,0x00,kTypeAddConstraint);
    put<std::uint32_t>(p,0x04,kIntUnset);
    put<std::uintptr_t>(p,0x08,std::uintptr_t(0)); // pBuddy
    // DS-F46 wording: the ctor PROVES the x-slots (+0x10/+0x1C); the remaining
    // sentinel-tested float offsets are filled defensively (the consumer tests
    // each for the sentinel; a fresh buffer starts at zero otherwise).
    {static constexpr unsigned unset[]={0x10u,0x14u,0x18u,0x1Cu,0x20u,0x24u,0x50u,0x58u,0x64u,0x68u,0x6Cu,0x70u,0x74u,0x78u,0x3Cu,0x4Cu};
     for(unsigned o: unset)put<std::uint32_t>(p,o,kFloatUnset);}
    put<std::uint32_t>(p,0x28,kIntUnset); // partid0
    put<std::uint32_t>(p,0x2C,kIntUnset); // partid1
    put<std::uint32_t>(p,0x60,2);         // flags default 2 (proven ctor)
    put<std::uint64_t>(p,0x80,0xFFFFFFFFFFFFFFFFull); // terminal user sentinel
    return p;
}
inline void setBuddy(Packet& p,std::uintptr_t buddy) noexcept {put<std::uintptr_t>(p,0x08,buddy);}
inline void setId(Packet& p,std::uint32_t id) noexcept {put<std::uint32_t>(p,0x04,id);}
inline void setPoint(Packet& p,unsigned which,float x,float y,float z) noexcept {
    if(which>1)return;const unsigned o=0x10+which*0x0C;
    put<float>(p,o,x);put<float>(p,o+4,y);put<float>(p,o+8,z);
}
inline void setPartIds(Packet& p,std::uint32_t a,std::uint32_t b) noexcept {
    put<std::uint32_t>(p,0x28,a);put<std::uint32_t>(p,0x2C,b);
}
inline bool setFlags(Packet& p,std::uint32_t flags) noexcept {
    // DS-F67 (Codex F59 review): FAIL CLOSED - bit 0x100 is INACTIVE (the
    // solver skips the constraint), so an invalid mask must NOT leave the
    // old flags and reach Action as an unintended active constraint. The
    // caller must check the return.
    if(flags&0x100u)return false;
    put<std::uint32_t>(p,0x60,flags);return true;
}
inline void setIgnorePairOnly(Packet& p) noexcept {put<std::uint32_t>(p,0x60,0x300u);} // DS-F65: inactive+ignore-buddy, never a grip
inline bool isDefault(const Packet& p) noexcept {
    return get<std::uint32_t>(p,0x00)==kTypeAddConstraint&&
        get<std::uint32_t>(p,0x04)==kIntUnset&&
        get<std::uintptr_t>(p,0x08)==0&&
        get<std::uint32_t>(p,0x28)==kIntUnset&&
        get<std::uint32_t>(p,0x2C)==kIntUnset&&
        get<std::uint32_t>(p,0x60)==2&&
        get<std::uint64_t>(p,0x80)==0xFFFFFFFFFFFFFFFFull&&
        get<std::uint32_t>(p,0x3C)==kFloatUnset&&
        get<std::uint32_t>(p,0x4C)==kFloatUnset;
}
// ---------------------------------------------------------------------------
// DS-F49/F57: the type-6 (update/remove constraint) packet, exact proven
// default ctor AF5744..AF57EB (Claude's answer): type=6, id unset, flagsOR=0,
// flagsAND=0xFFFFFFFF, bRemove=0, pt x-slots unset, quaternion w-slots unset,
// +0x4C..+0x5C unset, flags=2. The EXACT queue copy size is PROVEN 0x64
// (Claude's action-copy-size answer: the startup table at 0x51DCA30+type*4,
// filled at 13EF000 from the constant at 0x40A0450). The retail REMOVE
// sequence is: ctor -> id -> bRemove=1 -> Action(entity, packet,
// bThreadSafe=0) -> owner handle zeroed.
inline constexpr int kTypeUpdateRemoveConstraint=6;
inline constexpr std::size_t kRemoveSize=0x64; // DS-F57: the PROVEN copy size, not a lower bound
struct alignas(8) RemovePacket {unsigned char bytes[kRemoveSize]{};};
inline RemovePacket removeDefaults() noexcept {
    RemovePacket p{};
    put<std::uint32_t>(p,0x00,kTypeUpdateRemoveConstraint);
    put<std::uint32_t>(p,0x04,kIntUnset);          // id
    put<std::uint32_t>(p,0x08,0);                  // flagsOR
    put<std::uint32_t>(p,0x0C,0xFFFFFFFFu);        // flagsAND
    put<std::uint32_t>(p,0x10,0);                  // bRemove
    {static constexpr unsigned unset[]={0x14u,0x20u,0x38u,0x48u,0x4Cu,0x50u,0x54u,0x58u,0x5Cu};
     for(unsigned o: unset)put<std::uint32_t>(p,o,kFloatUnset);} // pt x-slots, quaternion w-slots, limits family
    put<std::uint32_t>(p,0x60,2);                  // flags default 2 (same proven value family)
    return p;
}
inline void setRemoveId(RemovePacket& p,std::uint32_t id) noexcept {put<std::uint32_t>(p,0x04,id);put<std::uint32_t>(p,0x10,1);} // bRemove=1
}
