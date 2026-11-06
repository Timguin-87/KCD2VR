#pragma once
#include <array>
namespace kcd2vr::ui_pause_blur124r1 {
// Event-driven lease: only console CVars; original values are restored if still ours.
// An external writer revokes the lease until pause closes. No continuous hammering.
class Lease {
    struct Entry {const char* name;int wanted,base=0;bool owned=false,revoked=false;};
    std::array<Entry,3> e_{{{"r_DepthOfField",2},{"wh_cl_NearDof",1},{"r_DepthOfFieldMode",1}}};
    bool revoked_=false;
public:
    template<class IO> bool Apply(IO& io,bool enable) noexcept {
        if(enable&&revoked_)return false;
        bool ok=true,foreign=false;
        for(auto& e:e_){
            if(!enable&&!e.owned){e.revoked=false;continue;}
            int value=-99;if(!io.Get(e.name,value)||value<0||value>2){ok=false;continue;}
            if(enable){
                if(e.revoked){ok=false;continue;}
                if(e.owned&&value!=e.wanted){e.owned=false;e.revoked=true;foreign=true;ok=false;continue;}
                if(!e.owned){e.base=value;e.owned=true;}
                if(value!=e.wanted&&(!io.Set(e.name,e.wanted)||!io.Get(e.name,value)||value!=e.wanted))ok=false;
            }else{
                if(value!=e.wanted&&value!=e.base){e.owned=false;e.revoked=false;continue;}
                if(value!=e.base&&(!io.Set(e.name,e.base)||!io.Get(e.name,value)||value!=e.base)){ok=false;continue;}
                e.owned=false;e.revoked=false;
            }
        }
        // Partial acquisition must not leave a renderer override without its near blur.
        if(enable&&!ok){(void)Apply(io,false);revoked_=foreign;}
        if(!enable)revoked_=false;
        return ok;
    }
};
}
