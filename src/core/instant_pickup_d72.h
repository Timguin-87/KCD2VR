#pragma once
#include <atomic>
#include <cstdint>

// D72 ordinary pickup has an accepted native parent, but deliberately owns the
// world item until chest stow. This bridge joins the existing, sole transfer and
// notice detours; it must never install a second hook on those native leaves.
namespace kcd2vr::instant_pickup_d72 {
using Transfer = bool(__fastcall*)(void*,const void*,bool);
using Notice = void(__fastcall*)(void*,void*,void*);
using Intercept = bool(*)(void*,const void*,bool,Transfer,Notice,bool&) noexcept;
inline std::atomic<Intercept> intercept{nullptr};
inline std::atomic<unsigned> active{0};
inline std::atomic<bool> stopping{false};
struct NoticeScope { void* item=nullptr;void* actor=nullptr; };
inline thread_local NoticeScope noticeScope{};
inline bool suppressNotice(void* item,void* actor) noexcept {
    return item&&actor&&noticeScope.item==item&&noticeScope.actor==actor;
}
}
