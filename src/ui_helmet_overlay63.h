#pragma once
#include "ui_native_canvas.h"
namespace kcd2vr::ui_helmet_overlay63 {
// This is the helmet parent INSIDE Overlay.gfx, never the full-field movie,
// HUD root, visor animation child, blood or cinematic layer.
inline constexpr const char* Anchor="overlay0";
inline constexpr float WidthH15=1.20f; // user:20% wider visor, height/center unchanged
using Geometry=ui_native_canvas::Geometry;
class Transaction {
 Geometry base_{},last_{};std::uintptr_t element_=0,clip_=0;bool active_=false;
public:
 bool Active()const noexcept{return active_;}
 void Forget()noexcept{element_=clip_=0;active_=false;}
 template<class IO> bool Run(IO& io,std::uintptr_t element,bool enabled,
                            float surfaceWidth,float size=.792f)noexcept {
  if(element!=element_){Forget();element_=element;}
  if(!enabled&&!active_)return true;
  std::uintptr_t clip=0;Geometry now{};
  if(!io.Resolve(Anchor,clip)||!clip||!io.Get(now)||!ui_native_canvas::Valid(now))return false;
  if(!active_||clip!=clip_)base_=now;
  else {
   if(std::fabs(now.x-last_.x)>=.05f)base_.x=now.x;
   if(std::fabs(now.y-last_.y)>=.05f)base_.y=now.y;
   if(std::fabs(now.xs-last_.xs)>=.02f)base_.xs=now.xs;
   if(std::fabs(now.ys-last_.ys)>=.02f)base_.ys=now.ys;
   base_.rotation=now.rotation;
  }
  Geometry target=now;
  if(enabled){
   if(!std::isfinite(surfaceWidth)||surfaceWidth<.2f||surfaceWidth>30.f||
      !std::isfinite(size)||size<.25f||size>1.f)return false;
   // Keep the authored center; retire C63's arbitrary ten-centimeter left shift.
   // H15: widen this helmet parent only. Preserve its authored center and the
   // accepted vertical fit; readable HUD/effect roots are separate owners.
   target=base_;target.xs*=size*WidthH15;target.ys*=size;
  }else target=base_;
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
