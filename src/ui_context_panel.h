#pragma once
#include "ui_native_canvas.h"

namespace kcd2vr::ui_context_panel {
using Geometry=ui_native_canvas::Geometry;
// ItemTransfer and ItemSelection: native cry_onResize sets modalAnchor.x to
// main._width/2 + 20*main._xscale/100. This supplies actual panel width without
// guessing an IFlashVariableObject GetMember ABI. Main's authored Y origin is
// its vertical centre (540); fullscreen popUp/popUp2 are separate siblings.
struct Plan {std::array<Geometry,2> target{};float width=0,height=0;bool valid=false;};
inline Plan Compute(const std::array<Geometry,2>& base,ui_surface_geometry::Rect safe,float size) noexcept {
    Plan p{};const auto&m=base[0];const auto&a=base[1];
    if(!ui_native_canvas::Valid(m)||!ui_native_canvas::Valid(a)||m.xs<=0||m.ys<=0||
       std::fabs(m.x)>2||std::fabs(m.y-540)>2||std::fabs(a.y-m.y)>2||
       std::fabs(a.xs-m.xs)>.1f||std::fabs(a.ys-m.ys)>.1f||!std::isfinite(size)||size<.45f||size>1.25f)return p;
    p.width=2*(a.x-20*m.xs/100.f);p.height=1080*m.ys/100.f;
    const float w=(safe.right-safe.left)*size,h=(safe.bottom-safe.top)*size;
    if(!std::isfinite(w)||!std::isfinite(h)||p.width<160||p.width>1900||p.height<160||p.height>2000||w<=0||h<=0)return p;
    const float k=std::min(w/p.width,h/p.height);
    const float cx=(safe.left+safe.right)*.5f,cy=(safe.top+safe.bottom)*.5f;
    p.target[0]={cx-p.width*k*.5f,cy,m.xs*k,m.ys*k,m.rotation};
    p.target[1]={p.target[0].x+a.x*k,cy+(a.y-m.y)*k,a.xs*k,a.ys*k,a.rotation};
    p.valid=ui_native_canvas::Valid(p.target[0])&&ui_native_canvas::Valid(p.target[1]);return p;
}
class Transaction {
    std::array<Geometry,2> base_{},last_{};std::array<std::uintptr_t,2> identity_{};
    std::uint64_t generation_=0;bool active_=false,repair_=false;
public:
    bool Active()const noexcept{return active_;}
    void Forget(std::uint64_t g)noexcept{base_={};last_={};identity_={};generation_=g;active_=repair_=false;}
    template<class Backend> bool Run(Backend& io,std::uint64_t g,bool enable,ui_surface_geometry::Rect safe={},float size=.9f)noexcept{
        if(generation_!=g)Forget(g);
        if(!enable&&!active_)return true;
        std::array<Geometry,2> now{},base=base_,target{};std::array<std::uintptr_t,2> id{};
        constexpr const char* names[]={"main","modalDialogAnchor"};
        for(unsigned i=0;i<2;++i){
            if(!io.Resolve(i,names[i],id[i])||!id[i]||!io.Get(i,now[i])||!ui_native_canvas::Valid(now[i]))return false;
            const bool ours=active_&&(ui_native_canvas::Same(now[i],last_[i])||(repair_&&id[i]==identity_[i]));
            if(!ours)base[i]=now[i];
        }
        if(id[0]==id[1])return false;
        if(enable){const auto plan=Compute(base,safe,size);if(!plan.valid)return false;target=plan.target;}
        else target=base;
        unsigned written=0;
        for(unsigned i=0;i<2;++i){
            if(ui_native_canvas::Same(now[i],target[i]))continue;
            written|=1u<<i;Geometry read{};
            if(!io.Set(i,target[i])||!io.Get(i,read)||!ui_native_canvas::Same(read,target[i])){
                bool restored=true;
                for(unsigned j=0;j<=i;++j)if(written&(1u<<j)){
                    Geometry back{};restored=io.Set(j,now[j])&&io.Get(j,back)&&ui_native_canvas::Same(back,now[j])&&restored;
                }
                if(!restored){base_=base;last_=target;identity_=id;active_=repair_=true;}
                return false;
            }
        }
        base_=base;last_=target;identity_=id;active_=enable;repair_=false;return true;
    }
};
} // namespace
