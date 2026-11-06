#pragma once
#include "npc_contact172.h"
#include <array>
#include <cstdint>

namespace kcd2vr::part_namespace62 {
// Diagnostic values only. The saved collider is NEVER dereferenced or retained
// as a live object. This record does not grant hit/impale/part ownership.
struct Ticket {
    std::uint64_t serial=0,sample=0,tick=0,key=0,generation=0,epoch=0,request=0;
    std::uint32_t target=0;
    std::uintptr_t actor=0,entity=0,collider=0;
    int rawPart=-1;
};
inline constexpr std::array<unsigned char,15> characterEntry{{0x40,0x53,0x48,0x83,0xEC,0x20,0x8B,0xDA,0x33,0xD2,0xE8,0x09,0x5F,0xF9,0xFF}};
inline constexpr std::array<unsigned char,8> poseGetter{{0x48,0x8D,0x81,0x98,0x06,0x00,0x00,0xC3}};
inline constexpr std::array<unsigned char,8> physicsGetter{{0x48,0x8B,0x81,0x08,0x01,0x00,0x00,0xC3}};
inline constexpr std::array<unsigned char,8> countGetter{{0x48,0x8B,0x41,0x08,0x8B,0x40,0xFC,0xC3}};
template<class Read> bool pins(std::uintptr_t b,Read& read,bool qualifiedOwnCharacter=false) noexcept {
    auto a=characterEntry;auto p=poseGetter;auto f=physicsGetter;auto c=countGetter;
    // C63: only the native bridge can supply the exact passive-hook owner's
    // post-enable receipt. A foreign patched entry stays refused by default;
    // all other getter/vtable observations retain their pristine retail pins.
    return b&&((read(b+0x6411AC,a)&&a==characterEntry)||qualifiedOwnCharacter)&&read(b+0x1AA6BC0,p)&&p==poseGetter&&
        read(b+0x1A77FD0,f)&&f==physicsGetter&&read(b+0x1AAF020,c)&&c==countGetter;
}
struct Binding {
    std::uintptr_t getCharacter=0,proxy=0,entityRoot=0,character=0,pose=0,model=0,joints=0,mainPhysics=0;
    std::uint32_t count=0;
    friend bool operator==(const Binding& a,const Binding& z) noexcept {
        return a.getCharacter==z.getCharacter&&a.proxy==z.proxy&&a.entityRoot==z.entityRoot&&
            a.character==z.character&&a.pose==z.pose&&a.model==z.model&&a.joints==z.joints&&
            a.mainPhysics==z.mainPhysics&&a.count==z.count;
    }
};
template<class Read> bool entity(std::uintptr_t b,const Ticket& t,Read& read,Binding& o) noexcept {
    std::uintptr_t vf=0,back=0;std::uint32_t id=0;
    return t.entity&&t.target&&read(t.entity,vf)&&vf==b+0x4072C78&&read(t.entity+0xC,id)&&id==t.target&&
        read(vf+0x328,o.getCharacter)&&o.getCharacter==b+0x6411AC&&
        npc_contact172::proxy(t.entity,1,read,o.proxy)&&o.proxy&&
        read(o.proxy,vf)&&vf==b+0x3AC3478&&read(o.proxy+0x30,back)&&back==t.entity&&
        read(o.proxy+0x38,o.entityRoot)&&o.entityRoot;
}
template<class Read> bool skeleton(std::uintptr_t b,std::uintptr_t ci,Read& read,Binding& o) noexcept {
    o.character=ci;o.pose=ci+0x698;
    std::uintptr_t vf=0,fn=0,back=0,poseModel=0;std::uint32_t poseCount=0;
    return ci&&read(ci,vf)&&vf==b+0x40602F8&&read(vf+0x38,fn)&&fn==b+0x1AA6BC0&&
        read(o.pose,vf)&&vf==b+0x3A71028&&read(vf+0x30,fn)&&fn==b+0x1A77FD0&&
        read(o.pose+0x210,back)&&back==ci&&read(o.pose+0x108,o.mainPhysics)&&
        read(ci+0x8E0,vf)&&vf==b+0x3A71510&&read(ci+0x940,poseCount)&&
        read(ci+0xA70,o.model)&&o.model&&read(ci+0x8E0+0x190,poseModel)&&poseModel==o.model&&
        read(o.model,vf)&&vf==b+0x406B930&&read(vf+8,fn)&&fn==b+0x1AAF020&&
        read(o.model+8,o.joints)&&o.joints>=4&&read(o.joints-4,o.count)&&
        o.count>=8&&o.count<=2048&&poseCount==o.count;
}
// The native caller has already qualified actor/entity. Recheck every observed
// current owner around GetCharacter; a changed object is UNKNOWN, not a join.
template<class Read,class Character> const char* capture(std::uintptr_t b,const Ticket& t,Read& read,
    Character& getCharacter,Binding& out) noexcept {
    Binding a{},z{};std::uintptr_t ca=0,cz=0;
    if(!entity(b,t,read,a))return "CURRENT_ENTITY_OR_ROOT";
    if(!getCharacter(a.getCharacter,t.entity,ca)||!skeleton(b,ca,read,a))return "CURRENT_CHARACTER_OR_SKELETON";
    if(!entity(b,t,read,z)||!getCharacter(z.getCharacter,t.entity,cz)||!skeleton(b,cz,read,z)||!(a==z))
        return "CURRENT_IDENTITY_CHANGED";
    out=a;return nullptr;
}
struct Budget {
    std::uint64_t session=0,last=0,serial=0,sample=0;unsigned lines=0;
    bool take(std::uint64_t s,std::uint64_t now,const Ticket& t) noexcept {
        if(!s||!now)return false;
        if(s!=session){*this={};session=s;}
        if(lines>=96||(last&&(now<last||now-last<250))||(t.serial==serial&&t.sample==sample))return false;
        last=now;serial=t.serial;sample=t.sample;++lines;return true;
    }
};
}
