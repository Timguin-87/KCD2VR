#pragma once
// D30-PERF: direct-mapped per-thread memo of RESIDENT READABLE single pages.
// Same permission evidence and the same freshness window as the V169 ring memo
// (frik_pose_probe Safe): a page is trusted for at most ttlMs after the last real
// QueryWorkingSetEx/VirtualQuery that proved it, every Read stays SEH-guarded.
// Only the lookup changes: O(1) by page number instead of a linear scan of 32 ring
// entries per read (~10k reads per frame), and room for 512 pages instead of 32 so
// the frame's working set no longer thrashes back into ~20 syscalls (~24 us each)
// per pose solve.
#include <array>
#include <cstddef>
#include <cstdint>

namespace kcd2vr::page_memo_pf {
struct Table {
    struct Slot {std::uintptr_t page=0;std::uint64_t stamp=0;};
    static constexpr std::size_t kSlots=512;
    std::array<Slot,kSlots> slot{};
    std::size_t pageSize=4096;
    static std::size_t index(std::uintptr_t page,std::size_t pageSize) noexcept {
        return std::size_t((page/pageSize)&(kSlots-1));
    }
    // first/end: the page-aligned span a real query just proved readable. Only spans of
    // one or two pages are memoised (larger VirtualQuery regions keep the existing ring).
    void insert(std::uintptr_t first,std::uintptr_t end,std::uint64_t now) noexcept {
        if(!pageSize||!first||end<=first||first%pageSize||end%pageSize||end-first>2*pageSize)return;
        for(std::uintptr_t p=first;p<end;p+=pageSize){auto& s=slot[index(p,pageSize)];s.page=p;s.stamp=now;}
    }
    bool fresh(std::uintptr_t page,std::uint64_t now,std::uint64_t ttlMs) const noexcept {
        const auto& s=slot[index(page,pageSize)];
        return s.page==page&&s.page&&now>=s.stamp&&now-s.stamp<=ttlMs;
    }
    // true only when every page touched by [address,address+need) is present and fresh.
    bool lookup(std::uintptr_t address,std::size_t need,std::uint64_t now,std::uint64_t ttlMs,
                std::uintptr_t& first,std::uintptr_t& end) const noexcept {
        if(!pageSize||!address||!need||address>UINTPTR_MAX-(need-1))return false;
        const auto a=address-address%pageSize;const auto last=address+need-1;const auto b=last-last%pageSize;
        if(b-a>pageSize)return false;
        if(!fresh(a,now,ttlMs))return false;
        if(b!=a&&!fresh(b,now,ttlMs))return false;
        first=a;end=b+pageSize;return true;
    }
    void clear() noexcept {slot={};}
};
}
