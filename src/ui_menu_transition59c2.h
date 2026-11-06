#pragma once
#include "ui_scene_presentation.h"
#include <atomic>

namespace kcd2vr::ui_menu_transition59c2 {
enum class Camera : unsigned { Other, Apse, Shop };
struct Observation { bool installed=false, known=false; Camera camera=Camera::Other; std::uint64_t serial=0; };
// A single publication contains both the selection and its generation. No native
// pointer escapes the callback, and a failed publication cannot retain authority.
class Mailbox {
    std::atomic<std::uint64_t> value_{0};
    std::atomic<bool> lost_{false};
public:
    void Publish(Camera camera) noexcept {
        auto old=value_.load(std::memory_order_relaxed);
        for(unsigned n=0;n<8;++n) {
            const auto next=(((old>>2)+1)<<2)|unsigned(camera);
            if(value_.compare_exchange_weak(old,next,std::memory_order_release,std::memory_order_relaxed))return;
        }
        lost_.store(true,std::memory_order_release);
    }
    Observation Read(bool installed) const noexcept {
        const auto v=value_.load(std::memory_order_acquire);
        return {installed,(v>>2)!=0&&!lost_.load(std::memory_order_acquire),Camera(v&3),v>>2};
    }
};
class Gate {
    std::uint64_t outside_serial_=0;
public:
    bool Theater(const ui_menu_router::Decision& d,bool requested,const Observation& s) noexcept {
        using O=ui_menu_router::Owner;
        const bool apse=d.owner==O::InventoryMap,shop=d.owner==O::Shop;
        if(!apse&&!shop){outside_serial_=s.serial;return requested;}
        if(!requested||!s.installed||!s.known)return requested; // No observation: original policy.
        // A previous menu's final observation cannot open the next one. Native
        // resolver callbacks, not a delay or input request, admit presentation.
        // C52: retail trading can expose an Inventory APSE root with the Shop
        // camera (12:13:12/33 headset receipt). Both are native APSE menu cameras;
        // an exact owner/camera spelling match incorrectly suppresses Theater.
        // Other still closes immediately; stale prior-menu serials stay refused.
        return s.serial!=outside_serial_&&
            (s.camera==Camera::Apse||s.camera==Camera::Shop);
    }
};

struct FramePolicy {
    int stereo=0;
    bool flat=false,native_menu=false,physical_map=false,drain=false;
};
// Production commits owner+source in one atomic word. Manual/diagnostic modes
// retain their selected policy; neither mode introduces a second scene layer.
constexpr unsigned ProductionWord(bool enabled,bool theater) noexcept {return enabled?(theater?3u:1u):0u;}
class Production {
    std::atomic<std::uint64_t> value_{0};
public:
    void Publish(bool enabled,bool theater) noexcept {
        const auto bits=ProductionWord(enabled,theater);
        auto old=value_.load(std::memory_order_relaxed);
        while((old&3)!=bits&&!value_.compare_exchange_weak(old,((old&~3ull)+4)|bits,std::memory_order_release,std::memory_order_relaxed)){}
    }
    std::uint64_t Read() const noexcept {return value_.load(std::memory_order_acquire);}
    bool Current(std::uint64_t ticket) const noexcept {return Read()==ticket;}
};
constexpr FramePolicy ResolveFrame(std::uint64_t production,FramePolicy legacy) noexcept {
    if(production&1u){const bool flat=(production&2u)!=0;
        legacy.stereo=flat?0:2;legacy.flat=flat;legacy.native_menu=flat;legacy.drain=flat;}
    return legacy;
}
}
