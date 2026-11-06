#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace kcd2vr::door66 {
bool OwnedMotionFor(std::uintptr_t actorEntity,std::uint32_t targetId,std::uintptr_t targetEntity,
                    std::uint64_t now,std::uint64_t* serial) noexcept;
}

// Exact-retail read contract. The opaque 702288 return is never an IAction.
// The live CActionScope instead proves its currently installed interactive root.
namespace kcd2vr::door_motion_native66 {
constexpr std::uintptr_t Apply=0x8443B4, ApplyCaller=0x84433B;
// The native call is534; gate at528 before preparing its scratch arguments.
// Whole-instruction5/14-byte patches both leave branch target53A untouched.
constexpr std::uintptr_t AnimatedCall=0x844528, AnimatedAfter=0x84453A;
// Gate BEFORE the argument-preparation block. The call at631 is only2 bytes
// and633 is an externally reached branch target; never patch across633.
constexpr std::uintptr_t EntityCall=0x224B61C, EntityAfter=0x224B633;
constexpr std::uint32_t AnimationControlledCrc=0x387092C6u;
struct Root {
    std::uintptr_t clip=0,state=0,scope=0,controller=0,context=0,scopeContext=0;
    std::uintptr_t action=0,actor=0,entity=0,targetEntity=0,animated=0,character=0,label=0;
    std::uint32_t target=0,token=0;int fragment=-1;char name[11]{};
};
inline bool same(const Root& a,const Root& b) noexcept {
    return a.clip==b.clip&&a.state==b.state&&a.scope==b.scope&&a.controller==b.controller&&
        a.context==b.context&&a.scopeContext==b.scopeContext&&a.action==b.action&&a.actor==b.actor&&
        a.entity==b.entity&&a.targetEntity==b.targetEntity&&a.animated==b.animated&&
        a.character==b.character&&a.label==b.label&&a.target==b.target&&a.token==b.token&&
        a.fragment==b.fragment&&!std::memcmp(a.name,b.name,sizeof(a.name));
}
inline bool doorName(const char (&s)[11]) noexcept {
    return !std::memcmp(s,"door_",5)&&(s[5]=='l'||s[5]=='r')&&s[6]=='_'&&
        (s[7]=='b'||s[7]=='f')&&s[8]=='_'&&(s[9]=='o'||s[9]=='c')&&s[10]==0;
}
template<class Copy> bool readRoot(Copy& copy,std::uintptr_t base,std::uintptr_t state,
                                 std::uintptr_t entity,Root& out) noexcept {
    out={};if(!base||state<0x40||!entity)return false;Root r{};r.clip=state-0x40;r.entity=entity;
    std::uintptr_t vft=0,p=0,q=0,definition=0,fragments=0,entries=0,helper=0;
    std::uint32_t id=0,otherId=0;unsigned count=0;int status=-1,fragCount=-1;
    std::uint64_t length=0;std::uint32_t crc=0;
    const auto get=[&](std::uintptr_t address,auto& value){return address&&copy(address,&value,sizeof(value));};
    const auto eq=[&](std::uintptr_t address,std::uintptr_t value){std::uintptr_t got=0;return get(address,got)&&got==value;};
    if(!eq(r.clip,base+0x3AB60A8)||!eq(r.clip+8,entity)||!get(r.clip+0x18,r.scope)||!r.scope||
       !get(r.clip+0x10,r.character)||!r.character||!eq(entity,base+0x4072C78)||
       !get(entity+0xC,id)||!id||!eq(r.scope,base+0x3AEA100)||
       !get(r.scope+0x28,r.controller)||!r.controller||!eq(r.controller,base+0x40519E8)||
       !get(r.controller+0x34,count)||count<1||count>64||!eq(r.controller+0x38,r.scope)||
       !get(r.controller+0x30,otherId)||otherId!=id||!get(r.controller+0x20,r.context)||!r.context||
       !get(r.scope+0x20,r.scopeContext)||!r.scopeContext||
       !eq(r.scopeContext+0x20,entity)||!eq(r.scopeContext+0x10,r.character)||
       !get(r.scope+0x148,r.action)||!r.action||!eq(r.action,base+0x3ADA108)||
       !get(r.action+0x28,status)||status!=2||!eq(r.action+0x30,r.scope)||!eq(r.action+8,r.context)||
       !get(r.action+0x38,r.fragment)||r.fragment<0||!get(r.action+0x54,r.token)||
       !get(r.action+0x90,r.actor)||!r.actor||!eq(r.actor+0x38,entity)||
       !get(r.action+0xA0,r.target)||!r.target||r.target>16777215u||
       !get(r.action+0x98,r.targetEntity)||!r.targetEntity||!eq(r.targetEntity,base+0x4072C78)||
       !get(r.targetEntity+0xC,otherId)||otherId!=r.target||
       !get(r.action+0xC0,helper)||!helper||!eq(helper+0x60,r.action)||
       !get(helper+0x54,otherId)||otherId!=r.target||!get(helper,length)||length!=10||
       !get(helper+0x10,r.label)||!r.label||!copy(r.label,r.name,sizeof(r.name))||!doorName(r.name)||
       !get(state+0x60,r.animated)||!r.animated||!eq(r.animated,base+0x3A49468)||
       !eq(base+0x3A49468+0x2F0,base+0x8447B0))return false;
    r.state=helper;
    // The exact getter83A8F4 returns entity+98->+18. Parented/mounted
    // entities take a separate native transform path and remain fully native.
    if(!get(entity+0x98,p)||(p&&(!get(p+0x18,q)||q)))return false;
    if(!get(r.context,definition)||!definition||!get(definition+0x18,fragments)||!fragments||
       !get(fragments+8,entries)||entries<4||!get(entries-4,fragCount)||fragCount<1||fragCount>65536||
       r.fragment>=fragCount||!get(entries+std::size_t(r.fragment)*0x20+0x10,crc)||crc!=AnimationControlledCrc)return false;
    // Replacement during scalar sampling never acquires a lease.
    if(!eq(r.clip,base+0x3AB60A8)||!eq(r.clip+0x18,r.scope)||!eq(r.clip+8,entity)||
       !eq(r.scope,base+0x3AEA100)||!eq(r.scope+0x28,r.controller)||
       !eq(r.controller+0x38,r.scope)||!eq(r.controller+0x20,r.context)||
       !eq(r.scope+0x148,r.action)||!eq(r.action,base+0x3ADA108)||
       !get(r.action+0x28,status)||status!=2||!eq(r.action+0x30,r.scope)||!eq(r.action+8,r.context)||
       !get(r.action+0x54,id)||id!=r.token||!eq(r.action+0xC0,helper)||!eq(helper+0x60,r.action)||
       !eq(helper+0x10,r.label)||!eq(r.action+0x90,r.actor)||!eq(r.actor+0x38,entity)||
       !eq(r.action+0x98,r.targetEntity)||!get(r.action+0xA0,id)||id!=r.target||
       !eq(r.context,definition)||!eq(definition+0x18,fragments)||!eq(fragments+8,entries))return false;
    out=r;return true;
}
struct Pin {std::uintptr_t rva;unsigned size;unsigned char bytes[24];};
inline constexpr Pin Pins[]{
    {Apply,16,{0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x55}},
    {0x844331,10,{0x48,0x89,0x44,0x24,0x20,0xE8,0x79,0,0,0}},
    {0x844528,18,{0x48,0x8B,0x4F,0x60,0x48,0x8D,0x54,0x24,0x78,0x48,0x8B,0x01,0xFF,0x90,0xF0,0x02,0,0}},
    {0x224B624,15,{0x4C,0x8D,0x44,0x24,0x60,0x48,0x8B,0xCE,0x48,0x8D,0x54,0x24,0x40,0xFF,0xD3}},
    {0x224B61C,8,{0x4C,0x8B,0xCF,0x44,0x89,0x7C,0x24,0x20}},
    {0x83A8F4,20,{0x48,0x8B,0x91,0x98,0,0,0,0x33,0xC0,0x48,0x85,0xD2,0x75,0x01,0xC3,0x48,0x8B,0x42,0x18,0xC3}},
    {0x947C48,17,{0x48,0x89,0x51,0x08,0x48,0x8B,0x54,0x24,0x28,0x4C,0x89,0x41,0x10,0x4C,0x89,0x49,0x18}},
    {0xF46EF2,4,{0x48,0x89,0x7B,0x60}}
};
}
