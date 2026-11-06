#pragma once
#include "core/snapshot_mailbox.h"
#include <cstdint>

namespace kcd2vr::pad_dispatch_d84 {
// A native input pass must use the axes, direction keys and button edges from
// the same producer call. combat18 retains the existing XI trigger values,
// ownership, stroke, serial and acknowledgement nonce without repacking them.
struct Frame {
    float lx=0,ly=0,rx=0,ry=0;
    std::uint64_t combat18=0;
    bool b=false,x=false,y=false,rb=false,l3=false,r3=false,start=false,back=false;
    bool du=false,dd=false,dl=false,dr=false;
};
}
