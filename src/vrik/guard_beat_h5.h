#pragma once
#include <cstdint>

namespace kcd2vr::guardH5 {
// One deliberate-motion input opportunity per observed incoming attack, not an
// automatic block on attack onset and not a perfect-block result. Key carries
// the complete player/item/opponent identity. No native reads or writes here.
template<class Key> struct Beat {
    Key key{};
    std::uint64_t tick=0,serial=0,onset=0;
    int phase=-1;
    bool known=false,active=false,spent=false;
    void observe(const Key& current,int p,std::uint64_t now,bool readable) noexcept {
        if(!readable||!current.valid()||!now||p<0){known=active=false;return;}
        const bool continuous=known&&key.same(current)&&now>=tick&&now-tick<=80;
        const bool attacking=p==4||p==8;
        const bool begins=continuous&&attacking&&
            ((phase!=4&&phase!=8)||(p==4&&phase==8));
        if(!continuous){active=false;spent=false;}
        else if(begins){active=true;spent=false;onset=now;if(++serial==0)++serial;}
        else if(!attacking){active=false;spent=false;}
        key=current;phase=p;tick=now;known=true;
    }
    bool ready()const noexcept {return known&&active&&!spent&&serial;}
    void consume()noexcept {if(active)spent=true;}
};
}
