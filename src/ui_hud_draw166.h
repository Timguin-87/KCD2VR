#pragma once
#include "ui_contextual_hud162r1.h"

namespace kcd2vr::ui_hud_draw166 {
using namespace ui_contextual_hud162r1;
struct Display { float xs=100,ys=100; bool visible=true; };
// Only the lifetime of one native Render is owned. No retained child pointers,
// persistent visibility baseline, root transform or forced native show state.
// Retail getters/setters return success: do not replace it with "did not throw".
template<class Backend> class Mask {
    Backend& io_;
    std::array<Display,Clips.size()> before_{};
    std::array<unsigned,Clips.size()> fields_{};
public:
    unsigned resolved=0,reads=0,writes=0;
    static constexpr unsigned Visibility=1,Scale=2;
    explicit Mask(Backend& io):io_(io){}
    bool Begin(unsigned hidden,const std::array<float,3>& sizes){
        for(float v:sizes)if(!std::isfinite(v)||v<.25f||v>1.5f)return false;
        for(unsigned i=0;i<Clips.size();++i){
            const bool hide=(hidden&Clips[i].group)!=0;
            const bool size=i>=16&&i<19&&sizes[i-16]!=1.f;
            if(!hide&&!size)continue;
            ++resolved;if(!io_.Resolve(i,Clips[i].name))continue;
            ++reads;auto& before=before_[i];
            if(!io_.ReadDisplay(i,before)||!std::isfinite(before.xs)||!std::isfinite(before.ys)){
                Restore();return false;
            }
            // Native-hidden children need no mutation. They must stay hidden
            // when a palm is revealed or a preference is turned back on.
            if(!before.visible)continue;
            auto target=before;unsigned fields=0;
            if(hide){target.visible=false;fields|=Visibility;}
            else if(size){target.xs*=sizes[i-16];target.ys*=sizes[i-16];fields|=Scale;}
            fields_[i]=fields; // even a failed/partial write must be rolled back
            ++writes;if(!io_.WriteDisplay(i,target,fields)){Restore();return false;}
        }
        return true;
    }
    bool Restore(){
        bool ok=true;
        for(unsigned i=0;i<Clips.size();++i)if(fields_[i]){
            ++writes;if(io_.WriteDisplay(i,before_[i],fields_[i]))fields_[i]=0;else ok=false;
        }
        return ok;
    }
    ~Mask(){Restore();}
};
}
