#pragma once
#include <cstdint>
namespace kcd2vr::ui_back_hold135r2 {
// Preserve native Press/Down/Release. Rearm once only when a proven native
// owner changes during a hold (e.g. builder assets -> slots). No timer spam.
class Hold {
    bool physical_=false,owned_=false,pending_=false;
    std::uint64_t epoch_=0,press_=0,release_until_=0;
public:
    void Reset() noexcept{*this={};}
    bool Consumed()const noexcept{return physical_&&owned_;}
    bool Update(bool down,bool context,std::uint64_t epoch,std::uint64_t now)noexcept{
        if(!down){Reset();return false;}
        if(!physical_){physical_=true;owned_=context;epoch_=epoch;press_=now;}
        if(!context||!owned_)return false;
        if(epoch_!=epoch){epoch_=epoch;pending_=true;}
        if(release_until_){if(now<release_until_)return false;release_until_=0;press_=now;}
        if(pending_&&now>=press_&&now-press_>=350){pending_=false;release_until_=now+50;return false;}
        return true;
    }
};
}
