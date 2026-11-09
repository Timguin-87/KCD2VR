#pragma once
// R141 (Miki 7 Nov: "there was some black frames visible (like sometimes nearly half a second long) in FSR or
// DLSS (others noticed it too)").
// cause: RunFrame drops the projection layer whenever the eye copy for that XR frame did not happen
// (render-thread handoff timed out after 500 ms because the game stalled, no presentable stereo pair, copy
// refused). the frame then goes out with no projection (and no HUD, the HUD needs have_layer) = BLACK, and the
// runtime keeps showing that black frame until the next good one. during a stall the next XR frame waits 500 ms
// again, so the black stays up to ~0.5 s. evidence: R124 log /tmp/r125/in 17:58:23.143 "R106 XR_LAYERS
// submitted=NONE" right after FRAME_STALL_LOOP ms=516 (handoffPickup maxEverMs 397.9 -> 500.7 in that window).
// fix: every eye swapchain image remembers the pose/fov it was last written with. when this frame's copy did not
// write the image, the image still holds that older picture, so the projection layer is sent again with the
// pose/fov that belong to those pixels (the runtime reprojects it world-locked, like a normal late frame).
// bounded: only pictures younger than kHoldMs, both eyes from the same frame, same eye mapping. no latch: every
// frame decides again, a written frame always wins. INI VrEyeHoldR141 (Experiments GEAR page 3), live.
#include <atomic>
#include <cstdint>
#include <vector>
#include <openxr/openxr.h>
namespace kcd2vr::xr_eye_hold_r141 {
inline std::atomic<bool> enabled{true};
inline constexpr std::uint64_t kHoldMs=2000;
struct Held {XrPosef pose{};XrFovf fov{};std::uint64_t frame=0,tickMs=0;int slot=-1;bool valid=false;};
using Images=std::vector<Held>;
// the image was written for projection output eye `slot` with this pose/fov
inline void Record(Images& images,std::uint32_t index,int slot,const XrPosef& pose,const XrFovf& fov,
    std::uint64_t frame,std::uint64_t nowMs) {
    if(index>=64)return; // swapchains have 2-4 images
    if(images.size()<=index)images.resize(index+1);
    images[index]={pose,fov,frame,nowMs,slot,true};
}
// the image was written with something that is not that projection (theater/map/mono quad, cleared image)
inline void Forget(Images& images,std::uint32_t index) noexcept {if(index<images.size())images[index].valid=false;}
enum class Result {Held,Missing,TooOld,Mismatch};
// both XR eyes re-shown from the images released this frame, or nothing (never one eye only)
inline Result Pick(const Images& sc0,std::uint32_t i0,const Images& sc1,std::uint32_t i1,bool swap,std::uint64_t nowMs,
    XrCompositionLayerProjectionView (&views)[2]) noexcept {
    const Held* h[2]{nullptr,nullptr};
    for(int eye=0;eye<2;++eye){
        const int source=swap?(eye^1):eye;
        const Images& sc=source==0?sc0:sc1;const std::uint32_t idx=source==0?i0:i1;
        if(idx>=sc.size()||!sc[idx].valid)return Result::Missing;
        if(sc[idx].slot!=eye)return Result::Mismatch;
        h[eye]=&sc[idx];
    }
    if(h[0]->frame!=h[1]->frame)return Result::Mismatch;
    for(int eye=0;eye<2;++eye)if(nowMs<h[eye]->tickMs||nowMs-h[eye]->tickMs>kHoldMs)return Result::TooOld;
    for(int eye=0;eye<2;++eye){views[eye].pose=h[eye]->pose;views[eye].fov=h[eye]->fov;}
    return Result::Held;
}
// R153 (Miki 8 Nov: "some black frames still showing"): the same stall in Theater (main menu, loading screens,
// cutscene/menu theater) still went out with NO layer at all = black, because the hold above is projection only.
// Theater shows ONE quad from left eye swapchain image (eyes_[0]); the pose comes from the theater anchor of
// this frame, so only "this image holds a theater picture of this same presentation scene" is remembered.
// slot kTheaterSlot never matches a projection eye (0/1), so Pick() above never re-shows a theater picture.
inline constexpr int kTheaterSlot=2;
inline void RecordTheater(Images& images,std::uint32_t index,std::uint64_t ticket,std::uint64_t nowMs) {
    if(index>=64)return;
    if(images.size()<=index)images.resize(index+1);
    images[index]=Held{};images[index].frame=ticket;images[index].tickMs=nowMs;images[index].slot=kTheaterSlot;images[index].valid=true;
}
// the left image released this frame still holds a theater picture of this scene (ticket), younger than kHoldMs
inline Result PickTheater(const Images& sc0,std::uint32_t i0,std::uint64_t ticket,std::uint64_t nowMs) noexcept {
    if(i0>=sc0.size()||!sc0[i0].valid)return Result::Missing;
    const Held& h=sc0[i0];
    if(h.slot!=kTheaterSlot||h.frame!=ticket)return Result::Mismatch;
    if(nowMs<h.tickMs||nowMs-h.tickMs>kHoldMs)return Result::TooOld;
    return Result::Held;
}
}
