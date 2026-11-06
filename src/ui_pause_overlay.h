#pragma once
#include "ui_native_canvas.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::ui_pause_overlay {
using Geometry=ui_native_canvas::Geometry;
// Direct root children recovered from the supplied retail Menu.gfx PlaceObject
// tags and root setup ActionScript. setup attaches Container0/1 directly to this
// root; they are NOT children of cl/cr/titleBox. They contain the menu buttons.
// `background` is deliberately NOT in this list: it must remain full-field (or be
// hidden independently) while readable/menu children are fitted into the binocular box.
inline constexpr std::array<const char*,11> Anchors{
    "cl","cr","titleBox","HelpContent","tooltip","actionHint","mod","support","version","Container0","Container1"
};
struct Plan { std::array<Geometry,Anchors.size()> target{}; bool valid=false; };

inline ui_native_canvas::NativeFrame InferFrame(const std::array<Geometry,Anchors.size()>& a) noexcept {
    ui_native_canvas::NativeFrame f{};
    const auto& cl=a[0]; const auto& cr=a[1];
    const float bottom=std::max({a[6].y,a[7].y,a[8].y});
    f.left=cl.x; f.top=0.0f; f.width=cr.x-cl.x; f.height=bottom;
    f.screenWidth=cl.x+cr.x; f.screenHeight=bottom;
    const auto nearly_equal=[](float x,float y,float eps){return std::fabs(x-y)<=eps;};
    // Retail authoring baseline is cl=(0,~540), cr=(1920,540), footer Y=1080.
    // Native horizontal anchors follow screen width, while Y and content scale
    // follow height. A square VR source is not a 16:9 authoring-stage resize.
    // Infer both independently; the physical fit below preserves aspect.
    const float hs=f.screenHeight/1080.0f;
    f.valid=ui_native_canvas::Valid(cl)&&ui_native_canvas::Valid(cr)&&f.width>=640&&f.width<=16384&&
        f.height>=360&&f.height<=16384&&hs>.2f&&hs<8.0f&&
        nearly_equal(cl.y,540.0f*hs,6.0f*hs)&&nearly_equal(cr.y,540.0f*hs,6.0f*hs)&&
        cl.x>=-1.f&&f.screenWidth>=f.width&&f.screenWidth<=16384&&cl.xs>0&&cl.ys>0&&
        nearly_equal(cl.xs,cr.xs,.1f)&&nearly_equal(cl.ys,cr.ys,.1f);
    return f;
}
inline Geometry MapMenuLive(Geometry native,const ui_native_canvas::Fit& fit,
        const ui_native_canvas::NativeFrame& frame,const Geometry& reference) noexcept {
    auto target=ui_native_canvas::MapLive(native,fit,frame);
    // Menu.gfx cry_onResize uses independent g_diffScaleW/H on its content,
    // unlike hud.gfx's uniform live-height scale. Strip that native viewport
    // correction once before fitting to the physical HUD panel. Otherwise a
    // square source receives the vertical aspect correction twice.
    target.xs=native.xs/reference.xs*100.f*fit.sx*frame.screenWidth/1920.f;
    target.ys=native.ys/reference.ys*100.f*fit.sy*frame.screenHeight/1080.f;
    return target;
}
inline Plan Compute(const std::array<Geometry,Anchors.size()>& base,const ui_surface_geometry::Hud& hud,float size,float promptScale=1.f) noexcept {
    Plan p{}; const auto frame=ui_pause_overlay::InferFrame(base); const auto fit=ui_native_canvas::Compute(hud,size);
    if(!frame.valid||!fit.valid)return p;
    for(std::size_t i=0;i<Anchors.size();++i){
        p.target[i]=MapMenuLive(base[i],fit,frame,base[0]);
        if(i==5){p.target[i].xs*=promptScale;p.target[i].ys*=promptScale;}
        if(!ui_native_canvas::Valid(p.target[i]))return Plan{};
    }
    p.valid=true; return p;
}

class Transaction {
    std::array<Geometry,Anchors.size()> base_{},last_{};
    std::array<std::uintptr_t,Anchors.size()> identity_{};
    std::uintptr_t menu_identity_=0; bool active_=false,repair_=false,centered_=false;
public:
    bool Active()const noexcept{return active_;}
    const auto& Baseline163()const noexcept{return base_;}
    const auto& Applied163()const noexcept{return last_;}
    void Forget(std::uintptr_t id=0)noexcept{base_={};last_={};identity_={};menu_identity_=id;active_=repair_=false;}
    unsigned failedMask=0,presentMask=0;
    template<class Backend> bool Run(Backend& io,std::uintptr_t menuIdentity,bool enable,const ui_surface_geometry::Hud& hud={},float size=1.0f,float promptScale=1.f,bool centered=false) noexcept {
        if(menu_identity_!=menuIdentity)Forget(menuIdentity);
        if(active_&&enable&&centered_!=centered){
            if(!Run(io,menuIdentity,false,hud,size,promptScale,centered_))return false;
        }
        if(!enable&&!active_)return true;
        std::array<Geometry,Anchors.size()> now{},base=base_,target{};
        std::array<std::uintptr_t,Anchors.size()> id{};failedMask=presentMask=0;
        for(std::size_t i=0;i<Anchors.size();++i){
            if(!io.Resolve(i,Anchors[i],id[i])||!id[i]||!io.Get(i,now[i])||!ui_native_canvas::Valid(now[i])){
                failedMask|=1u<<i;id[i]=0;
                if(centered&&i>=3&&i<9)continue; // helpers/footer optional; dynamic menu card is required
                return false;
            }
            presentMask|=1u<<i;
            for(std::size_t j=0;j<i;++j)if(id[j]&&id[j]==id[i])return false;
            const bool ours=active_&&(ui_native_canvas::Same(now[i],last_[i])||(repair_&&id[i]==identity_[i]));
            if(!ours){
                if(active_&&id[i]==identity_[i]){
                    // Menu.gfx repositionContainer changes only _y. Treat each
                    // display field independently: adopting our fitted scales
                    // as native merely because Y changed would shrink the card
                    // again on each bounded settling pass. A replacement clip
                    // or menu lifetime still starts with a fresh full baseline.
                    if(std::fabs(now[i].x-last_[i].x)>=.05f)base[i].x=now[i].x;
                    if(std::fabs(now[i].y-last_[i].y)>=.05f)base[i].y=now[i].y;
                    if(std::fabs(now[i].xs-last_[i].xs)>=.02f)base[i].xs=now[i].xs;
                    if(std::fabs(now[i].ys-last_[i].ys)>=.02f)base[i].ys=now[i].ys;
                    if(std::fabs(now[i].rotation-last_[i].rotation)>=.01f)base[i].rotation=now[i].rotation;
                }else base[i]=now[i];
            }
        }
        if(enable){
            auto fittedBase=base;
            if(centered){
                // Native title is the card's horizontal alignment reference.
                // Infer height from the live center anchors, independently of optional footers.
                const float height=2.f*base[1].y;
                for(unsigned i=6;i<9;++i)fittedBase[i].y=height;
            }
            const auto plan=Compute(fittedBase,hud,size,promptScale);if(!plan.valid)return false;target=plan.target;
            if(centered){
                const auto frame=ui_pause_overlay::InferFrame(fittedBase);const auto fit=ui_native_canvas::Compute(hud,size);
                const float shift=(frame.left+frame.width*.5f-base[2].x)/frame.width*fit.sx*frame.screenWidth;
                for(unsigned i=0;i<Anchors.size();++i)if(id[i]){
                    // Map actual optional geometry, not the synthetic frame footer used above.
                    target[i]=MapMenuLive(base[i],fit,frame,base[0]);target[i].x+=shift;
                    if(i==5){target[i].xs*=promptScale;target[i].ys*=promptScale;}
                    if(!ui_native_canvas::Valid(target[i]))return false;
                }
            }
        }
        else target=base;
        std::uint32_t written=0;
        for(std::size_t i=0;i<Anchors.size();++i){
            if(!id[i])continue;
            if(ui_native_canvas::Same(now[i],target[i]))continue;
            written|=std::uint32_t(1u<<i); Geometry read{};
            if(!io.Set(i,target[i])||!io.Get(i,read)||!ui_native_canvas::Same(read,target[i])){
                bool restored=true;
                for(std::size_t j=0;j<=i;++j)if(written&(1u<<j)){
                    Geometry back{}; restored=io.Set(j,now[j])&&io.Get(j,back)&&ui_native_canvas::Same(back,now[j])&&restored;
                }
                if(!restored){base_=base;last_=target;identity_=id;active_=repair_=true;}
                return false;
            }
        }
        base_=base;last_=target;identity_=id;active_=enable;centered_=centered;repair_=false;return true;
    }
};
} // namespace kcd2vr::ui_pause_overlay
