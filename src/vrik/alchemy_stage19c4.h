#pragma once
#include "alchemy_tool18.h"
#include "../core/snapshot_mailbox.h"
namespace kcd2vr::alchemy_stage19c4 {
struct Control {
    std::uintptr_t instance=0;std::uint64_t lifetime=0,request=0;
    bool enabled=false,held[2]{};std::uint64_t tick[2]{};
};
struct Proof {
    alchemy_tool18::Binding binding{};
    std::uintptr_t child=0;std::uint32_t token=0;
    std::uint64_t request=0,epoch=0,sample=0,tick=0;
};
inline core::SnapshotMailbox<Control> control;
inline core::SnapshotMailbox<Proof> attached;
}
