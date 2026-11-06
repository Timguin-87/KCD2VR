#pragma once
// R125: exact pin for CEntity::Hide (WHGame+0x87963C, retail 0x6A350E20).
// R124 log: every R120 STOWED_ENTITY row said HIDE_PIN_REFUSED. cause: the F2
// recorder D77 puts its own mid hook 'd77_draw_entity_visibility' on exactly
// this address (log 17:49:13.381, 1.7 s before the first hide). the hook
// rewrites the first instruction (mov [rsp+18],rbx, 5 bytes) with a jmp, so the
// old 16 byte compare failed, and the result was kept in a static for the whole
// session. now: the function identity is proven by the CEntity vtable slot
// (vft 0x4072C78 + 0x1F8 == 0x87963C, .rdata, never hooked) and the code bytes
// are accepted as the original 16 bytes OR a jmp detour on the first
// instruction with the remaining original bytes intact. checked every call.
#include <cstdint>
#include <cstring>
namespace kcd2vr::entity_hide_pin_r125 {
inline constexpr std::uint32_t kHideRva=0x87963C;
inline constexpr std::uint32_t kEntityVftRva=0x4072C78;
inline constexpr std::uint32_t kHideSlot=0x1F8;
inline constexpr unsigned char kHide[16]{0x48,0x89,0x5C,0x24,0x18,0x55,0x56,0x57,0x48,0x81,0xEC,0xB0,0x00,0x00,0x00,0x48};
// 0 = original bytes, 1 = rel32 jmp (E9) over the first 5 byte instruction,
// 2 = absolute jmp (FF 25 + 8 byte address, 14 bytes) over the first four
// instructions (5+1+1+1+7 = 15 bytes, byte 15 still original), -1 = refused.
inline int codeShape(const unsigned char (&actual)[16]) noexcept {
    if(!std::memcmp(actual,kHide,16))return 0;
    if(actual[0]==0xE9&&!std::memcmp(actual+5,kHide+5,11))return 1;
    if(actual[0]==0xFF&&actual[1]==0x25&&actual[15]==kHide[15])return 2;
    return -1;
}
inline bool accept(std::uintptr_t base,std::uintptr_t vftSlotValue,const unsigned char (&actual)[16]) noexcept {
    return base&&vftSlotValue==base+kHideRva&&codeShape(actual)>=0;
}
}
