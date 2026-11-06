#pragma once
#include <array>
#include <atomic>
#include <cstdint>

namespace kcd2vr::weapon_draw_d75 {
// Retail weapon classes already handled by the physical presentation path:
// dagger through torch, rifle, the two larger crossbows, hunting sword, shield.
// Fists (12), ammunition and unrelated activity props are not draw candidates.
constexpr bool weaponClass(int id) noexcept {
    return (id>=0&&id<=11)||(id>=13&&id<=17);
}

// The native attach operation can invoke UpdateAttachment before its return
// publishes the normal hand/equipment cache. This stack-local candidate bridges
// that observation gap only. It does NOT prove acceptance or grant a write:
// consumers must independently re-read current hand membership and binding.
struct NativeAttachScope;
inline thread_local NativeAttachScope* currentAttach=nullptr;
inline std::atomic<std::uint64_t> attachSerial{0};
struct NativeAttachScope {
    struct Callback {void* self=nullptr;void* attachment=nullptr;};
    void* owner=nullptr;void* item=nullptr;int side=-1;
    std::uint64_t serial=0;
    std::array<Callback,8> observed{};
    NativeAttachScope* previous=nullptr;
    NativeAttachScope(void* owner_,void* item_,int side_) noexcept
        :owner(owner_),item(item_),side(side_),serial(attachSerial.fetch_add(1,std::memory_order_relaxed)+1),
         previous(currentAttach){currentAttach=this;}
    // Native faults may unwind through an /EHsc caller without running C++
    // destructors. The native-call helper invokes leave in __finally; the
    // destructor keeps ordinary lexical and C++-exception use safe as well.
    void leave() noexcept {if(currentAttach==this)currentAttach=previous;}
    ~NativeAttachScope(){leave();}
    NativeAttachScope(const NativeAttachScope&)=delete;
    NativeAttachScope& operator=(const NativeAttachScope&)=delete;
    void remember(void* self,void* attachment) noexcept {
        for(auto& c:observed){
            if(c.self==self&&c.attachment==attachment)return;
            if(!c.self){c={self,attachment};return;}
        }
    }
};
}
