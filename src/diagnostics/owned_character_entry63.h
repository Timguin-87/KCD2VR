#pragma once
#include "../core/snapshot_mailbox.h"
#include <atomic>
#include <cstdint>
#include <cstring>
namespace kcd2vr::character_entry63 {
inline constexpr std::uintptr_t kEntry=0x6411AC;
inline constexpr unsigned char kPristine[]{0x40,0x53,0x48,0x83,0xec,0x20,0x8b,0xda,0x33,0xd2,0xe8,0x09,0x5f,0xf9,0xff,0x4c};
// Receipt is captured only after THIS exact passive hook enables. A jump at the
// retail address, an enabled recorder, or a remembered base is never authority.
class OwnedEntry {
    struct Receipt {std::uintptr_t base=0,trampoline=0;unsigned char bytes[16]{};};
    core::SnapshotMailbox<Receipt> receipt_;
    std::atomic<std::uintptr_t> activeBase_{0};
public:
    void clear() noexcept {activeBase_.store(0,std::memory_order_release);}
    template<class Copy> bool publish(std::uintptr_t base,std::uintptr_t trampoline,bool enabled,Copy copy) noexcept {
        clear();Receipt r{};r.base=base;r.trampoline=trampoline;
        if(!base||!trampoline||!enabled||!copy(base+kEntry,r.bytes,sizeof(r.bytes))||
           !std::memcmp(r.bytes,kPristine,sizeof(kPristine))||!receipt_.publish(r))return false;
        activeBase_.store(base,std::memory_order_release);return true;
    }
    template<class Copy> bool qualifies(std::uintptr_t base,Copy copy) const noexcept {
        Receipt r{};unsigned char current[16]{};
        return base&&activeBase_.load(std::memory_order_acquire)==base&&receipt_.read(r)&&
            r.base==base&&r.trampoline&&copy(base+kEntry,current,sizeof(current))&&
            !std::memcmp(current,r.bytes,sizeof(current))&&activeBase_.load(std::memory_order_acquire)==base;
    }
};
} // namespace kcd2vr::character_entry63
