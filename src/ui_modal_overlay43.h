#pragma once
#include "ui_native_canvas.h"

namespace kcd2vr::ui_modal_overlay43 {
// Retail ApseModalDialog.gfx: all question text/buttons belong to this ONE
// direct child (960,540). Menu.gfx's card fitter cannot reach this other movie.
inline constexpr const char* Anchor="modalDialogAnchor";
using Geometry=ui_native_canvas::Geometry;
inline bool Fit(Geometry native,const ui_surface_geometry::Hud& hud,float size,Geometry& out) noexcept {
    const auto fit=ui_native_canvas::Compute(hud,size);
    if(!fit.valid||!ui_native_canvas::Valid(native)||native.x<320||native.x>8192||
       native.y<180||native.y>8192||native.xs<=0||native.ys<=0)return false;
    // The live center supplies the viewport dimensions. Remove the native
    // height resize once; fit the 1920x1080 content, preserving physical aspect.
    const float width=2*native.x,height=2*native.y,heightScale=height/1080.f;
    // R98: ONE physical scale for X and Y. Flash movie units are square on the
    // texture (noScale live pixels, or ShowAll/dynamic stage units such as
    // LockPicking mc_LockPick at 960,540 and ApseModalDialog). The old Y term
    // (fit.sy*height/1080) assumed the 1920x1080 stage was stretched over the
    // whole HUD texture; on the 2064x2208 headset texture that made the lock,
    // wait clock and craft/alchemy result popups 1.9x too wide. For live-pixel
    // movies (center = texture center, quad aspect = texture aspect) the values
    // below are identical to the old ones; only stage-unit movies change.
    const float unit=hud.surface.width/width; // metres per movie unit, both axes
    const float cy=(hud.safe.top+hud.safe.bottom)*.5f/1080.f; // 0..1 on the quad
    const float xs=native.xs/heightScale*fit.sx*width/1920.f;
    out={(hud.safe.left+hud.safe.right)*.5f*width/1920.f,
         native.y+(cy-.5f)*hud.surface.height/unit,
         xs,xs*(native.ys/native.xs),native.rotation};
    return std::isfinite(unit)&&unit>0.f&&ui_native_canvas::Valid(out);
}
class Transaction {
    Geometry base_{},last_{};std::uintptr_t element_=0,clip_=0;bool active_=false;
public:
    bool Active()const noexcept{return active_;}
    void Forget()noexcept{element_=clip_=0;active_=false;}
    template<class IO> bool Run(IO& io,std::uintptr_t element,bool enabled,
                               const ui_surface_geometry::Hud& hud,float size,const char* anchor=Anchor)noexcept {
        if(element!=element_){Forget();element_=element;}
        if(!enabled&&!active_)return true;
        std::uintptr_t clip=0;Geometry now{};
        if(!io.Resolve(anchor,clip)||!clip||!io.Get(now)||!ui_native_canvas::Valid(now))return false;
        if(!active_||clip!=clip_)base_=now;
        else { // Native resize may update only some fields; don't adopt our scale twice.
            if(std::fabs(now.x-last_.x)>=.05f)base_.x=now.x;
            if(std::fabs(now.y-last_.y)>=.05f)base_.y=now.y;
            if(std::fabs(now.xs-last_.xs)>=.02f)base_.xs=now.xs;
            if(std::fabs(now.ys-last_.ys)>=.02f)base_.ys=now.ys;
        }
        Geometry target=base_;if(enabled&&!Fit(base_,hud,size,target))return false;
        if(!ui_native_canvas::Same(now,target)){
            Geometry check{};
            if(!io.Set(target)||!io.Get(check)||!ui_native_canvas::Same(check,target)){
                (void)io.Set(now);return false;
            }
        }
        last_=target;clip_=clip;active_=enabled;return true;
    }
};
}
