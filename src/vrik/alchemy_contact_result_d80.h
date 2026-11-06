#pragma once
// POD only; staged with the existing coherent transaction. A rejected pose
// cannot seed contact history; this is one sample result, never a stale plane.
struct AlchemyContactResultD80 {
    kcd2vr::alchemy_tool18::Binding binding{};
    std::uint64_t sample=0,tracking=0,epoch=0,request=0,mode=0;
    float input[7]{},output[7]{},world[8]{};
    bool valid=false;
};
