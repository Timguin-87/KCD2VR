#pragma once
#include <cstdint>
namespace kcd2vr::npc_target47c1 {
// Retail +59B348 follows the +18 payload's GetEntity. RTTI +3A399A8
// (C_AIPuppet) / +3A6B9E8 (C_SOPuppet), slot +18 -> +602360:
// 48 8B 41 38 C3. +38 is the engine IEntity; +18 is NOT IEntity.
inline constexpr std::uintptr_t AiPuppet=0x3A399A8,SOPuppet=0x3A6B9E8;
inline constexpr bool Puppet(std::uintptr_t vft) noexcept {return vft==AiPuppet||vft==SOPuppet;}
template<class Read>
bool Resolve(Read&& read,std::uintptr_t base,void* payload,void* local,void*& entity) noexcept {
 entity=nullptr;if(!base||!payload||!local)return false;
 std::uintptr_t vft=0;
 if(!read(payload,vft))return false;
 if(vft==base+0x4072C78)entity=payload; // retain direct CEntity route
 else {
  if(!Puppet(vft-base))return false;
  std::uintptr_t getter=0;
  if(!read(reinterpret_cast<void*>(vft+0x18),getter)||getter!=base+0x602360||
     !read(static_cast<unsigned char*>(payload)+0x38,entity))return false;
 }
 if(entity!=local)return false;
 return read(entity,vft)&&vft==base+0x4072C78;
}
}
