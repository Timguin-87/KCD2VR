#pragma once
#include <cstdint>

// The production capture owner. DeviceHook serializes every call with its
// recursive transaction mutex. A recorded copy reserves its exact buffers until
// that command-list recording is submitted or SUCCESSFULLY reset. A new CPU
// frame/mode cannot turn an old queue callback into a new frame's publication.
namespace kcd2vr::temporal_capture_d85 {
struct Ticket {
    std::uint64_t generation{},epoch{},recording{},leftPose{},rightPose{};
    std::uint32_t frame{};std::uint32_t leftState{};
    std::uintptr_t source{},left{},right{},list{};
    bool leftBoundary{},rightComplete{},leftQueued{},recorded{},cancelled{};
};
class Owner {
    Ticket current_{};
    std::uint64_t generation_=0,recording_=0;
public:
    const Ticket& Current() const noexcept {return current_;}
    bool Pending() const noexcept {return current_.recorded;}
    bool Begin(std::uint64_t epoch,std::uint32_t frame) noexcept {
        if(Pending()||!epoch||!frame)return false;
        current_={};current_.generation=++generation_;
        current_.epoch=epoch;current_.frame=frame;return true;
    }
    void Cancel() noexcept {
        if(Pending())current_.cancelled=true;else current_={};
    }
    bool CompleteRight(std::uint64_t epoch,std::uint32_t frame,bool sdkReady,
                       std::uint64_t pose) noexcept {
        if(!sdkReady||!current_.generation||current_.cancelled||
           current_.epoch!=epoch||current_.frame!=frame)return false;
        current_.rightComplete=true;current_.rightPose=pose;return true;
    }
    void NoteLeftBoundary(std::uint64_t epoch,std::uint32_t frame,std::uintptr_t source,
                          std::uint32_t state,std::uint64_t pose) noexcept {
        if(current_.epoch==epoch&&current_.frame==frame&&!current_.cancelled&&source){
            current_.source=source;current_.leftState=state;current_.leftPose=pose;current_.leftBoundary=true;
        }
    }
    bool MarkLeft(std::uint64_t generation,std::uintptr_t source,std::uintptr_t left,
                  bool queued,std::uint64_t pose) noexcept {
        if(!queued||!current_.leftBoundary||!source||!left||generation!=current_.generation||current_.cancelled||
           (current_.source&&current_.source!=source)||(current_.left&&current_.left!=left)) {
            if(generation==current_.generation)current_.cancelled=true;
            return false;
        }
        current_.source=source;current_.left=left;current_.leftPose=pose;
        current_.leftQueued=true;return true;
    }
    bool CanRecord(std::uint64_t epoch,std::uint32_t frame,std::uintptr_t source,
                   bool sdkReady) const noexcept {
        return !Pending()&&current_.generation&&!current_.cancelled&&current_.leftBoundary&&current_.rightComplete&&
            current_.epoch==epoch&&current_.frame==frame&&source&&sdkReady&&
            (!current_.source||current_.source==source);
    }
    bool Record(std::uintptr_t source,std::uintptr_t left,std::uintptr_t right,std::uintptr_t list) noexcept {
        if(Pending()||!current_.generation||current_.cancelled||!current_.rightComplete||
           !source||!left||!right||left==right||!list||
           (current_.source&&current_.source!=source)||(current_.left&&current_.left!=left))return false;
        current_.source=source;current_.left=left;current_.right=right;current_.list=list;
        current_.recording=++recording_;current_.recorded=true;return true;
    }
    Ticket Incoming(std::uintptr_t list) const noexcept {
        return Pending()&&current_.list==list?current_:Ticket{};
    }
    bool Matches(const Ticket& receipt) const noexcept {
        return Pending()&&receipt.recorded&&receipt.generation==current_.generation&&
            receipt.recording==current_.recording&&receipt.list==current_.list&&
            receipt.epoch==current_.epoch&&receipt.frame==current_.frame&&
            receipt.source==current_.source&&receipt.left==current_.left&&receipt.right==current_.right;
    }
    bool Publishable(const Ticket& receipt,bool currentModeAndSdkReady,bool gameDirectQueue) const noexcept {
        return Matches(receipt)&&!current_.cancelled&&current_.leftQueued&&current_.rightComplete&&
            currentModeAndSdkReady&&gameDirectQueue;
    }
    // The caller retires the reference pins behind a GPU fence before Finish.
    bool Finish(const Ticket& receipt) noexcept {
        if(!Matches(receipt))return false;
        current_={};return true;
    }
    // Failed Reset does not discard D3D12 commands: retain their resource pins.
    bool ResetList(std::uintptr_t list,bool succeeded) noexcept {
        if(!succeeded||!Pending()||current_.list!=list)return false;
        current_={};return true;
    }
};
}
