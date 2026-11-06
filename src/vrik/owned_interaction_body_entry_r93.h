#pragma once
#include "../core/snapshot_mailbox.h"
#include <atomic>
#include <cstdint>
#include <cstring>
namespace kcd2vr::interaction_body_entry_r93 {
inline constexpr std::uintptr_t kEntry=0x8A49FC,kCallsite=0x37CFD23,kMethodSlot=0x4072EF0;
inline constexpr unsigned char kPristine[]{0x40,0x53,0x48,0x83,0xEC,0x20,0xBA,0x01,0x00,0x00,0x00,0x48,0x8B,0xD9,0xE8,0xB5};
inline constexpr unsigned char kCall[]{0xE8,0xD4,0x4C,0x0D,0xFD};
// These callers still identify the exact retail getter even when our passive
// observer owns its entry. An arbitrary detour is never a substitute.
template<class Copy> bool context(std::uintptr_t base,Copy copy) noexcept {
    unsigned char call[sizeof(kCall)]{};std::uintptr_t method=0;
    return base&&copy(base+kCallsite,call,sizeof(call))&&!std::memcmp(call,kCall,sizeof(call))&&
        copy(base+kMethodSlot,&method,sizeof(method))&&method==base+kEntry;
}
template<class Copy> bool pristine(std::uintptr_t base,Copy copy) noexcept {
    unsigned char entry[sizeof(kPristine)]{};
    return base&&copy(base+kEntry,entry,sizeof(entry))&&!std::memcmp(entry,kPristine,sizeof(entry));
}
class OwnedEntry {
    struct Receipt {std::uintptr_t base=0,trampoline=0;unsigned char bytes[sizeof(kPristine)]{};};
    core::SnapshotMailbox<Receipt> receipt_;
    std::atomic<std::uintptr_t> activeBase_{0};
public:
    void clear() noexcept {activeBase_.store(0,std::memory_order_release);}
    // Called only after pristine contract validation and our own successful
    // enable. Keep the exact installed bytes, not a generic JMP signature.
    template<class Copy> bool publish(std::uintptr_t base,std::uintptr_t trampoline,bool enabled,Copy copy) noexcept {
        clear();Receipt r{};r.base=base;r.trampoline=trampoline;
        if(!base||!trampoline||!enabled||!context(base,copy)||!copy(base+kEntry,r.bytes,sizeof(r.bytes))||
           !std::memcmp(r.bytes,kPristine,sizeof(kPristine))||!receipt_.publish(r))return false;
        activeBase_.store(base,std::memory_order_release);return true;
    }
    template<class Copy> bool qualifies(std::uintptr_t base,std::uintptr_t trampoline,bool enabled,Copy copy) const noexcept {
        Receipt r{};unsigned char current[sizeof(kPristine)]{};
        return base&&trampoline&&enabled&&activeBase_.load(std::memory_order_acquire)==base&&receipt_.read(r)&&
            r.base==base&&r.trampoline==trampoline&&context(base,copy)&&copy(base+kEntry,current,sizeof(current))&&
            !std::memcmp(current,r.bytes,sizeof(current))&&activeBase_.load(std::memory_order_acquire)==base;
    }
};
} // namespace kcd2vr::interaction_body_entry_r93
