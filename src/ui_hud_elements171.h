#pragma once
#include "ui_native_canvas.h"

namespace kcd2vr::ui_hud_elements171 {
using Geometry=ui_native_canvas::Geometry;
inline constexpr std::array<const char*,4> Names{"Compass","Stats","QAMWeapon","QAMFood"};
struct Adjustment {float scale=1,x=0,y=0;};
struct Report {unsigned resolved=0,applied=0,pending=0,failed=0,queried=0;bool complete=false;};
template<class B>auto Revision(B& b,unsigned i,int)->decltype(b.ElementRevision59c7(i)){return b.ElementRevision59c7(i);}
template<class B>std::uint64_t Revision(B&,unsigned,long){return 0;}
inline bool Same(Adjustment a,Adjustment b){return a.scale==b.scale&&a.x==b.x&&a.y==b.y;}
// Local clip geometry only; never visibility, alpha, timeline or child contents.
// Readable parents own the safe zones; these factors are relative to native clips.
class Controller {
    struct Entry {std::uintptr_t id=0;Geometry native{},last{};bool owned=false;Adjustment adjustment{};std::uint64_t revision=0;};
    std::array<Entry,4> entries_{};std::uint64_t generation_=0;Report report_{};
public:
    Report LastReport()const noexcept{return report_;}
    bool Active()const{for(const auto&e:entries_)if(e.owned)return true;return false;}
    template<class Backend> bool Run(Backend& io,std::uint64_t generation,bool enabled,
                                     const std::array<Adjustment,4>& adjustment){
        report_={};
        if(generation_!=generation){entries_={};generation_=generation;}
        auto staged=entries_;std::array<Geometry,4> current{},target{};
        std::array<bool,4> use{};
        for(unsigned i=0;i<4;++i){
            auto&e=staged[i];if(!enabled&&!e.owned)continue;
            const auto revision=Revision(io,i,0);
            if(enabled&&e.owned&&revision&&e.revision==revision&&Same(e.adjustment,adjustment[i])){
                report_.resolved|=1u<<i;report_.applied|=1u<<i;continue;
            }
            report_.queried|=1u<<i;
            std::uintptr_t id=0;if(!io.Resolve(i,Names[i],id)){report_.pending|=1u<<i;continue;}
            report_.resolved|=1u<<i;
            if(!io.Get(i,current[i])||!ui_native_canvas::Valid(current[i])){report_.failed|=1u<<i;return false;}
            if(e.id!=id||(e.revision&&Revision(io,i,0)&&e.revision!=Revision(io,i,0))){e={id,current[i],current[i],false};}
            if(!e.owned)e.native=current[i];
            target[i]=current[i];
            if(enabled){
                const auto&a=adjustment[i];
                if(!std::isfinite(a.scale)||a.scale<.15f||a.scale>2.f||
                   !std::isfinite(a.x)||!std::isfinite(a.y))return false;
                target[i]={e.native.x+a.x,e.native.y+a.y,e.native.xs*a.scale,e.native.ys*a.scale,e.native.rotation};
            }else if(e.owned&&ui_native_canvas::Same(current[i],e.last))target[i]=e.native;
            if(!ui_native_canvas::Valid(target[i]))return false;
            use[i]=true;
        }
        unsigned attempted=0;
        for(unsigned i=0;i<4;++i)if(use[i]){
            if(!ui_native_canvas::Same(current[i],target[i])){
                attempted|=1u<<i;Geometry back{};
                if(!io.Set(i,target[i])||!io.Get(i,back)||!ui_native_canvas::Same(back,target[i])){
                    report_.failed|=1u<<i;
                    for(unsigned j=0;j<=i;++j)if(attempted&(1u<<j)){
                        // Preserve repair ownership if an unsuccessful setter wrote partially.
                        if(!io.Set(j,current[j])){entries_[j]=staged[j];entries_[j].last=target[j];entries_[j].owned=true;}
                    }
                    return false;
                }
            }
            staged[i].last=target[i];staged[i].owned=enabled;
            staged[i].adjustment=adjustment[i];staged[i].revision=Revision(io,i,0);
            report_.applied|=1u<<i;
        }
        entries_=staged;
        // Stats is required when enabled; optional selectors may appear later.
        report_.complete=!enabled||!(report_.pending&(1u<<1));
        return report_.complete;
    }
};
}
