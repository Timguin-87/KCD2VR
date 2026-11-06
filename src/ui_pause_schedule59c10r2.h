#pragma once
#include <cstdint>
namespace kcd2vr::ui_pause_schedule59c10r2 {
// Entry/layout-change verification only. Native menu initialization can resize
// the clips AFTER our first successful DisplayInfo readback. Never turn this
// into a keepalive: at most three transactions, then zero calls until an event.
class Settling {
    unsigned remaining_=0;
    std::uint64_t next_=0;
    bool restoring_=false;
public:
    void Begin(bool enabled,std::uint64_t now) noexcept {remaining_=3u;next_=now;restoring_=!enabled;}
    bool Take(std::uint64_t now,bool ready) noexcept {
        if(!remaining_||!ready||now<next_)return false;
        --remaining_;next_=now+(remaining_==2?150u:300u);return true;
    }
    void Complete(bool success) noexcept {if(restoring_&&success)remaining_=0;}
    bool Pending()const noexcept{return remaining_!=0;}
};
}
