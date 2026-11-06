#pragma once
#include "dialogue_state59c10r2.h"
#include "ui_menu_router.h"
#include <cstdint>

namespace kcd2vr::dialogue_ratio_d79 {
// Worker presentation policy only. A speech/bark, selected dialogue camera,
// stale witness, authored scene or menu never earns ownership of this mask.
inline bool Wanted(const dialogue_state59c10r2::State& state,int mode,
    bool theater,bool matrix,bool nativeReference,bool activity,
    const ui_menu_router::Decision& ui,std::uint64_t now) noexcept {
    return mode>=1&&dialogue_state59c10r2::fresh(state,now)&&state.active&&state.instance&&
        state.camera==dialogue_state59c10r2::Camera::FirstPerson&&
        !theater&&!matrix&&!nativeReference&&!activity&&
        ui.owner==ui_menu_router::Owner::None&&ui.uncertain==0;
}
struct Result {bool applied=false;std::uint64_t generation=0;};
// No periodic keepalive: at most three entry writes per conversation/HUD
// generation (0/80/200 ms). Exit restores only a generation this owner hid;
// failed restoration retries at 200 ms until success or generation retirement.
// This is an earlier bounded worker repair, NOT a pre-render/first-frame claim.
class Lease {
    std::uint64_t generation_=0,conversation_=0,ownedGeneration_=0,started_=0;
    unsigned attempts_=0;bool wanted_=false,owned_=false;
public:
    template<class Apply> void Update(bool wanted,std::uint64_t conversation,
        std::uint64_t generation,std::uint64_t now,Apply&& apply) noexcept {
        if(owned_&&generation!=ownedGeneration_)owned_=false;
        if(wanted!=wanted_||generation!=generation_||(wanted&&conversation!=conversation_)){
            wanted_=wanted;generation_=generation;conversation_=conversation;
            started_=now;attempts_=0;
        }
        if(!wanted_&&!owned_)return;
        if(!wanted_){
            // Do not abandon a hidden HUD after a transient native failure.
            // This is restoration debt, not ordinary periodic HUD upkeep.
            if(now<started_||(attempts_&&now-started_<200))return;
            started_=now;attempts_=1;
        }else{
            constexpr std::uint64_t delay[]{0,80,200};
            if(attempts_>=3||now<started_||now-started_<delay[attempts_])return;
            ++attempts_;
        }
        const auto result=apply(!wanted_,wanted_?0:ownedGeneration_);
        if(!result.applied||!result.generation)return;
        if(wanted_){
            owned_=true;ownedGeneration_=generation_=result.generation;
        }else owned_=false;
    }
};
} // namespace kcd2vr::dialogue_ratio_d79
