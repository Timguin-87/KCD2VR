#pragma once
#include <cstdint>
namespace kcd2vr::original_contact_h8 {
// Copied immutable queued occurrence. Wait is never contact/command authority.
struct Source {std::uint64_t gesture=0,key=0,generation=0,body=0,request=0,recovery=0,sample=0,tick=0;
    std::uintptr_t player=0,entity=0,collider=0;};
enum class Decision {Reject,Wait};
}
