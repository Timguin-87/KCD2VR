#pragma once
#include <cstdint>
namespace kcd2vr::ui_adaptive_hud57c1 {
// Event-owned CVar lease. Native callback releases the adaptive fade and restores
// alpha itself; never force Stats _visible or invent a value for a missing CVar.
class Lease {
    bool attempted_=false,wanted_=false,owned_=false;
    std::uint64_t epoch_=0;
    int original_=0;
public:
    bool Owned()const noexcept{return owned_;}
    template<class IO> bool Update(IO& io,bool want,std::uint64_t epoch) {
        if(attempted_&&wanted_==want&&epoch_==epoch)return false;
        attempted_=true;wanted_=want;epoch_=epoch;
        if(!want&&!owned_)return false;
        int current=0;
        if(!io.Read(current))return true;
        if(want){
            if(owned_&&current==0)return true;
            owned_=false;original_=current;
            if(current==0)return true; // already native-off: no ownership to restore
            if(io.Write(0)){int read=0;owned_=io.Read(read)&&read==0;}
        }else{
            if(current!=0){owned_=false;return true;} // another writer wins
            if(io.Write(original_)){int read=0;if(io.Read(read)&&read==original_)owned_=false;}
        }
        return true;
    }
};
}
