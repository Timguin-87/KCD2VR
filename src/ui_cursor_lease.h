#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
namespace kcd2vr::ui_cursor_lease {
// Native HardwareMouse uses balanced increments/decrements, NOT a visibility
// boolean. This owns only the extra cursor lease needed with locked pad prompts.
// Caller serializes the small fixed registry; no allocation or diagnostic I/O.
enum class Delta:int { None=0,Show=1,Hide=-1 };
class State {
    std::array<std::uintptr_t,64> owners_{};unsigned count_=0;bool lease_=false;
public:
    unsigned Count() const noexcept{return count_;}
    bool Visible() const noexcept{return lease_;}
    bool Overflow() const noexcept{return overflow_;}
    Delta Register(std::uintptr_t owner,bool on) noexcept {
        if(!owner)return Delta::None;
        std::size_t empty=owners_.size();
        for(std::size_t i=0;i<owners_.size();++i){
            if(owners_[i]==owner){if(on)return Delta::None;owners_[i]=0;--count_;return count_?Delta::None:Hide();}
            if(!owners_[i])empty=i;
        }
        if(on){if(empty==owners_.size()){overflow_=true;return Delta::None;}owners_[empty]=owner;++count_;}
        return Delta::None;
    }
    Delta MouseMoved() noexcept {if(!count_||lease_||overflow_)return Delta::None;lease_=true;return Delta::Show;}
    Delta Hide() noexcept {if(!lease_)return Delta::None;lease_=false;return Delta::Hide;}
    void Clear() noexcept{owners_={};count_=0;lease_=false;overflow_=false;}
private:bool overflow_=false;
};
}
