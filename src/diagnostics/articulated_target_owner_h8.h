#pragma once
#include "../vrik/npc_contact172.h"
#include <array>
#include <cstdint>
#include <cstring>
// Read-only CURRENT owner join for a queued original contact. This creates no
// allocation reference, new contact, part mapping, native cost or action grant.
namespace kcd2vr::articulated_target_owner_h8 {
struct Pin {std::uintptr_t rva;const unsigned char* bytes;std::size_t size;};
inline constexpr unsigned char p0[]{0x48,0x83,0xec,0x28,0x0f,0xba,0xf2,0x1f,0xe8,0x33,0xe2,0x6e,0xfe};
inline constexpr unsigned char p1[]{0x48,0x8b,0x81,0x18,0x01,0x00,0x00,0x48,0x85,0xc0,0x75,0x12};
inline constexpr unsigned char p2[]{0x48,0x8b,0x81,0x88,0x00,0x00,0x00,0x48,0x8b,0x04,0xd0,0x48,0x8b,0x80,0x80,0x00,0x00,0x00};
inline constexpr unsigned char p3[]{0x4c,0x8b,0x89,0x88,0x00,0x00,0x00,0x48,0x8b,0x81,0x90,0x00,0x00,0x00,0x49,0x2b,0xc1};
inline constexpr unsigned char p4[]{0x48,0xc1,0xf8,0x03,0x3b,0xd0,0x7d,0x0c,0x48,0x63,0xc2,0x4d,0x39,0x04,0xc1,0x74,0x03,0x41,0xb0,0x01};
inline constexpr unsigned char p5[]{0x48,0x8d,0x81,0x98,0x06,0x00,0x00,0xc3};
inline constexpr unsigned char p6[]{0x48,0x8b,0x81,0x08,0x01,0x00,0x00,0xc3};
inline constexpr Pin pins[]{{0x2103EE0,p0,sizeof p0},{0x2103EF8,p1,sizeof p1},{0x2103F04,p2,sizeof p2},{0x7F2127,p3,sizeof p3},{0x7F2138,p4,sizeof p4},{0x1AA6BC0,p5,sizeof p5},{0x1A77FD0,p6,sizeof p6}};
template<class Copy> bool contract(std::uintptr_t base,Copy& copy) noexcept {
 std::array<unsigned char,24> actual{};
 for(const auto& p:pins)if(!copy(base+p.rva,actual.data(),p.size)||std::memcmp(actual.data(),p.bytes,p.size))return false;
 return true;
}
struct Owner {
 std::uintptr_t entity=0,physical=0,root=0,renderProxy=0,render=0,slots=0,end=0,slot=0,direct=0,character=0,main=0;
 bool operator==(const Owner& b)const noexcept{return entity==b.entity&&physical==b.physical&&root==b.root&&renderProxy==b.renderProxy&&render==b.render&&slots==b.slots&&end==b.end&&slot==b.slot&&direct==b.direct&&character==b.character&&main==b.main;}
};
template<class Copy> bool capture(std::uintptr_t base,std::uintptr_t entity,std::uint32_t id,std::uintptr_t proxy,Copy& copy,Owner& o) noexcept {
 o={};o.entity=entity;auto read=[&](std::uintptr_t at,auto& v){return copy(at,&v,sizeof(v));};
 std::uintptr_t vf=0,back=0,method=0;std::uint32_t currentId=0;
 if(!read(entity,vf)||vf!=base+0x4072C78||!read(entity+0xC,currentId)||currentId!=id||
    !npc_contact172::proxy(entity,1,read,o.physical)||o.physical!=proxy||
    !read(o.physical,vf)||vf!=base+0x3AC3478||!read(o.physical+0x30,back)||back!=entity||!read(o.physical+0x38,o.root)||
    !npc_contact172::proxy(entity,0,read,o.renderProxy)||o.renderProxy<0x50)return false;
 o.render=o.renderProxy-0x50;
 if(!read(o.renderProxy,vf)||vf!=base+0x3A4A268||!read(o.render,vf)||vf!=base+0x3A49FD8||
    !read(o.render+0x88,o.slots)||!read(o.render+0x90,o.end)||!o.slots||o.end<o.slots||o.end-o.slots<8||
    !read(o.slots,o.slot)||!o.slot||!read(o.render+0x118,o.direct))return false;
 // Exact GetCharacter(0): direct character takes precedence. A non-null bad
 // direct pointer must NOT silently switch to an unrelated slot character.
 o.character=o.direct;if(!o.character&&!read(o.slot+0x80,o.character))return false;
 if(!o.character||!read(o.character,vf)||vf!=base+0x40602F8||!read(vf+0x38,method)||method!=base+0x1AA6BC0)return false;
 const auto pose=o.character+0x698;
 return read(pose,vf)&&vf==base+0x3A71028&&read(vf+0x30,method)&&method==base+0x1A77FD0&&
     read(pose+0x210,back)&&back==o.character&&read(pose+0x108,o.main)&&o.main;
}
template<class Copy> bool current(std::uintptr_t base,std::uintptr_t entity,std::uint32_t id,std::uintptr_t body,std::uintptr_t proxy,bool exactRetail,Copy copy) noexcept {
 if(!base||!entity||!id||!body||!proxy||!exactRetail||!contract(base,copy))return false;
 Owner first{},after{};
 if(!capture(base,entity,id,proxy,copy,first)||first.main!=body)return false;
 // The CURRENT graph must nominate the original collider BEFORE we read it.
 // No saved raw collider is traversed as an independent lifetime authority.
 std::uintptr_t vf=0,foreign=0;std::int16_t type=-1;
 if(!copy(body,&vf,sizeof vf)||vf!=base+0x3A48B30||!copy(body+0x20,&foreign,sizeof foreign)||foreign!=entity||
    !copy(body+0x28,&type,sizeof type)||type!=2)return false;
 return capture(base,entity,id,proxy,copy,after)&&first==after&&after.main==body;
}
}
