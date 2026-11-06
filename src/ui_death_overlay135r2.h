#pragma once
#include "ui_native_canvas.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::ui_death_overlay135r2 {
using Geometry=ui_native_canvas::Geometry;
// Retail GameOver.gfx: the background is gameOverScreen.bc. Never write it
// or gameOverScreen. Fit only its picture/message card and the separate prompt.
inline constexpr std::array<const char*,2> Anchors{"gameOverScreen.picture","infoText"};
struct Plan { std::array<Geometry,Anchors.size()> target{}; bool valid=false; };

inline Plan Compute(const std::array<Geometry,2>& base,Geometry parent,const ui_surface_geometry::Hud& hud,float size) noexcept {
    Plan p{};const auto fit=ui_native_canvas::Compute(hud,size);
    const float w=parent.x*2.f,h=parent.y*2.f;
    const auto& info=base[1];
    if(!fit.valid||!ui_native_canvas::Valid(parent)||w<640||w>16384||h<360||h>16384||
       std::fabs(info.x-w)>6||std::fabs(info.y-h)>6||parent.xs<1||parent.ys<1)return p;
    ui_native_canvas::NativeFrame f{0,0,w,h,w,h,true};
    Geometry absolute=base[0];
    absolute.x=parent.x+base[0].x*parent.xs/100.f;
    absolute.y=parent.y+base[0].y*parent.ys/100.f;
    absolute.xs*=parent.xs/100.f;absolute.ys*=parent.ys/100.f;
    const auto mapped=ui_native_canvas::MapLive(absolute,fit,f);
    p.target[0]={(mapped.x-parent.x)*100.f/parent.xs,(mapped.y-parent.y)*100.f/parent.ys,
                 mapped.xs*100.f/parent.xs,mapped.ys*100.f/parent.ys,base[0].rotation};
    p.target[1]=ui_native_canvas::MapLive(info,fit,f);
    p.valid=ui_native_canvas::Valid(p.target[0])&&ui_native_canvas::Valid(p.target[1]);return p;
}

class Transaction {
    std::array<Geometry,Anchors.size()> base_{},last_{};
    std::array<std::uintptr_t,Anchors.size()> identity_{};
    std::uintptr_t menu_identity_=0; bool active_=false,repair_=false;
public:
    bool Active()const noexcept{return active_;}
    const auto& Baseline163()const noexcept{return base_;}
    const auto& Applied163()const noexcept{return last_;}
    void Forget(std::uintptr_t id=0)noexcept{base_={};last_={};identity_={};menu_identity_=id;active_=repair_=false;}
    template<class Backend> bool Run(Backend& io,std::uintptr_t menuIdentity,bool enable,const ui_surface_geometry::Hud& hud={},float size=1.0f) noexcept {
        if(menu_identity_!=menuIdentity)Forget(menuIdentity);
        if(!enable&&!active_)return true;
        std::array<Geometry,Anchors.size()> now{},base=base_,target{};
        std::array<std::uintptr_t,Anchors.size()> id{};
        for(std::size_t i=0;i<Anchors.size();++i){
            if(!io.Resolve(i,Anchors[i],id[i])||!id[i]||!io.Get(i,now[i])||!ui_native_canvas::Valid(now[i]))return false;
            for(std::size_t j=0;j<i;++j)if(id[j]==id[i])return false;
            const bool ours=active_&&(ui_native_canvas::Same(now[i],last_[i])||(repair_&&id[i]==identity_[i]));
            if(!ours)base[i]=now[i];
        }
        if(enable){Geometry parent{};if(!io.Parent(parent))return false;const auto plan=Compute(base,parent,hud,size);if(!plan.valid)return false;target=plan.target;}
        else target=base;
        std::uint32_t written=0;
        for(std::size_t i=0;i<Anchors.size();++i){
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
        base_=base;last_=target;identity_=id;active_=enable;repair_=false;return true;
    }
};
} // namespace kcd2vr::ui_death_overlay135r2
