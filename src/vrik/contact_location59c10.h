#pragma once
#include <cstdint>
#include <cstring>

namespace kcd2vr::contact_location59c10 {
// Exact retail data flow, not an SDK vtable assumption. Native geometry
// producer 13F5811 writes ORIGINAL contact material to details+68; +64 is
// the mapped effects surface. The retail ray producers 38C3BBF/38C47B0
// return that original material at ray_hit+1A. D56A30 consumes details+68.
// Physics part, material and RPG body subpart are separate domains.
inline bool writeOriginalMaterial(unsigned char* details,int originalMaterial) noexcept {
    if(!details||originalMaterial<0||originalMaterial>32767)return false;
    const std::int32_t value=originalMaterial;
    std::memcpy(details+0x68,&value,sizeof(value));
    return true;
}
}
