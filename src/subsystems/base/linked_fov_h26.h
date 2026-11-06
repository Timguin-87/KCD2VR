#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace kcd2vr::linked_fov_h26 {
inline float Value(float selected) noexcept {return std::isfinite(selected)?std::clamp(selected,70.f,130.f):95.f;}
// Observe startup and, once, the first positively identified gameplay HUD.
// A late profile load cannot silently become the user's saved FOV. No permanent
// runtime chase, transient camera pumping, projection pulse or loading hook.
struct Startup {
 bool active=false,gameplaySeen=false;std::uint64_t start=0,next=0;
 bool Poll(std::uint64_t now,std::uint64_t samples,bool gameplay) noexcept {
  if(!samples)return false;
  if(!active){active=true;start=next=now;}
  if(gameplay&&!gameplaySeen){gameplaySeen=true;start=next=now;}
  if(now<start||now-start>=10000||now<next)return false;
  next=now+500;return true;
 }
};
}
