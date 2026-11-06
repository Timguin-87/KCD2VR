#pragma once
#include <cmath>
#include <cstdint>
#include <algorithm>
namespace kcd2vr::map_fold_m39 {
struct Result {float fraction=-1;bool close=false;};
struct Gesture {
 std::uint64_t serial=~std::uint64_t(0),nearClosed=0;
 bool armed=false,captured=false;float start=0;
 Result next(std::uint64_t intent,std::uint64_t now,bool open,bool rightCurrent,float grip,
             float edgeDistance,float handDistance) noexcept {
  if(serial!=intent){*this={};serial=intent;}
  if(!open||!rightCurrent||!std::isfinite(grip)||!std::isfinite(edgeDistance)||!std::isfinite(handDistance)){
   armed=captured=false;nearClosed=0;return {};
  }
  if(grip<.25f){armed=true;captured=false;nearClosed=0;return {};}
  if(!captured){
   if(!armed||grip<=.55f||edgeDistance>.12f||handDistance<.12f||handDistance>.55f)return {};
   armed=false;captured=true;start=handDistance;
  }
  if(grip<=.55f){captured=false;nearClosed=0;return {};}
  const float fraction=std::clamp((handDistance-.065f)/(start-.065f),0.f,1.f);
  if(fraction>.025f)nearClosed=0;
  else if(!nearClosed)nearClosed=now;
  return {fraction,nearClosed&&now>=nearClosed&&now-nearClosed>=80};
 }
};
}
