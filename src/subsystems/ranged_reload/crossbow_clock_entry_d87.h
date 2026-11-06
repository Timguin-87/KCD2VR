#pragma once
#include <cstddef>
#include <cstdint>
#include <cmath>

namespace kcd2vr::crossbow_clock_entry_d87 {
struct TerminalObservation {
    std::uintptr_t entry=0,sampler=0;float previous=-1,current=-1;
    unsigned flags=0,known=0;std::uint16_t dynamic=0;
    unsigned char previousSegment=255,currentSegment=255;
};
struct SelectionObservation {
    const char* why=nullptr;unsigned layer=0,layers=0,count=0;
    std::uintptr_t entries=0,selected=0;bool duplicate=false;
    TerminalObservation earlier{},later{};
};
// WHGame bdf8f9e4: 84187E..8418AD reads dynamic flags +50, previous time
// +3C/current +44 and segments +14/+16. Equal times/segments with no force
// event bit 100 return without scanning. This recognizes only the terminal,
// non-looping, direct CAF case; the caller must separately prove that this
// CAF has no native reload effects. It never grants event or time authority.
template<class Read> bool inertTerminal(Read&& read,std::uintptr_t entry,TerminalObservation* observed=nullptr) noexcept {
    std::uintptr_t sampler=~std::uintptr_t(0);float previous=-1,current=-1;
    unsigned flags=~0u;std::uint16_t dynamic=0x100;
    unsigned char previousSegment=255,currentSegment=255;
    unsigned known=0;
    const auto field=[&](std::uintptr_t at,void* out,std::size_t size,unsigned bit) noexcept {
        const bool ok=read(at,out,size);if(ok)known|=bit;return ok;
    };
    const bool inert=entry&&field(entry+8,&sampler,sizeof(sampler),1u)&&!sampler&&
        field(entry+0x14,&previousSegment,sizeof(previousSegment),2u)&&previousSegment==0&&
        field(entry+0x16,&currentSegment,sizeof(currentSegment),4u)&&currentSegment==0&&
        field(entry+0x3C,&previous,sizeof(previous),8u)&&previous==1.f&&
        field(entry+0x44,&current,sizeof(current),16u)&&current==1.f&&
        field(entry+0x4C,&flags,sizeof(flags),32u)&&!(flags&1u)&&
        field(entry+0x50,&dynamic,sizeof(dynamic),64u)&&!(dynamic&0x100u);
    if(observed)*observed={entry,sampler,previous,current,flags,known,dynamic,previousSegment,currentSegment};
    return inert;
}
inline bool nativeEffect(unsigned crc) noexcept {
    switch(crc){
    case 0xAFE0B3FFu:case 0x34857AFAu:case 0x3D6870EBu:case 0x863A26ABu:
    case 0x864932ACu:case 0x8ABB129Au:case 0xAE150612u:case 0xB337FBDCu:
    case 0xC14030E2u:case 0xDF6F00DFu:case 0xEF667DEAu:case 0xF04FE686u:return true;
    default:return false;
    }
}
}
