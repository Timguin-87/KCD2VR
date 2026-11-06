#pragma once
// V66.100: bounded native-only hand capture. No game ABI, poses or input writes.
// A trigger chord marks intent; geometry must independently qualify each fist.
#include "pose_math.h"
#include <array>
#include <cstdint>
namespace kcd2vr::finger_capture {
namespace pc=pose_core;
constexpr int MaxJoints=32;
struct Joint { int id=-1,parent=-1,digit=-1; std::uint32_t crc=0; pc::Quat q{}; pc::Vec3 t{}; };
struct Hand { int count=0; std::array<Joint,MaxJoints> joints{}; std::array<double,5> bend{}; };
struct Pair { std::uint64_t rig=0; std::array<Hand,2> open{},closed{}; };
inline bool valid(const Hand& h) noexcept {
 if(h.count<10||h.count>MaxJoints)return false;
 unsigned digits=0;
 for(int i=0;i<h.count;++i){const auto& j=h.joints[i];
  const double n=j.q.x*j.q.x+j.q.y*j.q.y+j.q.z*j.q.z+j.q.w*j.q.w;
  if(j.id<0||j.parent<0||j.id==j.parent||j.digit<0||j.digit>4||!j.crc||!std::isfinite(n)||n<.98||n>1.02||!pc::finite(j.t)||pc::length(j.t)>.15)return false;
  for(int k=0;k<i;++k)if(h.joints[k].id==j.id||h.joints[k].crc==j.crc)return false;
  digits|=1u<<j.digit;
 }
 for(double v:h.bend)if(!std::isfinite(v)||v<0||v>12.57)return false;
 return digits==31;
}
inline bool same_rig(const Hand&a,const Hand&b) noexcept {
 if(a.count!=b.count)return false;
 for(int i=0;i<a.count;++i){const auto&x=a.joints[i];const auto&y=b.joints[i];
  if(x.id!=y.id||x.parent!=y.parent||x.crc!=y.crc||x.digit!=y.digit||pc::length(x.t-y.t)>.003)return false;}
 return true;
}
inline double change(const Hand&a,const Hand&b) noexcept {
 if(!same_rig(a,b))return 1000;
 double v=0;for(int i=0;i<a.count;++i)v=std::max(v,pc::angle(a.joints[i].q,b.joints[i].q));return v;
}
inline bool clenched(const Hand&open,const Hand&closed) noexcept {
 if(!valid(open)||!valid(closed)||!same_rig(open,closed))return false;
 int bent=0;double total=0;
 for(int f=1;f<5;++f){const double delta=closed.bend[f]-open.bend[f];bent+=delta>.35;total+=delta;}
 // Sanity gate only, NOT a claimed anatomical maximum or mesh-clearance test.
 return bent>=3 && total>1.4;
}
inline bool valid(const Pair&p) noexcept {
 if(!p.rig)return false;
 for(int s=0;s<2;++s)if(!clenched(p.open[s],p.closed[s]))return false;
 return true;
}
struct Stable {
 Hand anchor{},latest{};std::uint64_t since=0,tick=0;unsigned samples=0;bool ready=false;
 void reset() noexcept {since=tick=0;samples=0;ready=false;}
 bool push(const Hand&h,std::uint64_t now) noexcept {
  if(!valid(h)){reset();return false;}
  if(!samples||now<tick||now-tick>150||change(anchor,h)>.105){anchor=h;since=now;samples=0;ready=false;}
  latest=h;tick=now;++samples;ready=samples>=4&&now>=since&&now-since>=250;return ready;
 }
};
enum class Stage:int { NeedRelease=1,Open=2,WaitBothTriggers=3,Clenching=4,Ready=5,Paused=6,NeedNativeFists=7,Qualified=8 };
struct Frame {std::uint64_t sample=0,tick=0,rig=0;bool allowed=false,empty=false,tracked=false;double trigger[2]{},grip[2]{};std::array<Hand,2> hand{};};
struct Recorder {
 Stage stage=Stage::NeedRelease;std::uint64_t sample=0,rig=0,pressTick=0,lastTick=0;bool haveOpen=false,offered=false;
 std::array<Stable,2> stable{};std::array<Hand,2> open{},best{};bool haveBest[2]{};
 void reset() noexcept {*this={};}
 void interrupt() noexcept {stage=Stage::Paused;haveOpen=false;haveBest[0]=haveBest[1]=false;for(auto&s:stable)s.reset();}
 bool update(const Frame& f,Pair&result) noexcept {
  if(!f.sample||f.sample==sample)return false;sample=f.sample;
  if(lastTick&&(f.tick<lastTick||f.tick-lastTick>150))interrupt();lastTick=f.tick;
  if(!f.allowed||!f.empty||!f.tracked||!f.rig||!valid(f.hand[0])||!valid(f.hand[1])){interrupt();return false;}
  for(int s=0;s<2;++s)if(!std::isfinite(f.trigger[s])||!std::isfinite(f.grip[s])){interrupt();return false;}
  if(rig&&rig!=f.rig){interrupt();offered=false;}rig=f.rig;
  const bool release=f.trigger[0]<.15&&f.trigger[1]<.15;
  const bool openInput=release&&f.grip[0]<.15&&f.grip[1]<.15;
  const bool both=f.trigger[0]>.8&&f.trigger[1]>.8;
  if(stage==Stage::Ready)return false;
  if(stage==Stage::NeedRelease||stage==Stage::Paused){if(!openInput)return false;stage=Stage::Open;for(auto&s:stable)s.reset();}
  if(stage==Stage::Open){
   if(openInput){for(int s=0;s<2;++s)stable[s].push(f.hand[s],f.tick);return false;}
   if(f.trigger[0]>.2||f.trigger[1]>.2){
    if(!stable[0].ready||!stable[1].ready||f.tick<stable[0].tick||f.tick<stable[1].tick||f.tick-stable[0].tick>200||f.tick-stable[1].tick>200){interrupt();return false;}
    for(int s=0;s<2;++s){open[s]=stable[s].latest;stable[s].reset();}haveOpen=true;pressTick=f.tick;stage=Stage::WaitBothTriggers;
   }else{for(auto&s:stable)s.reset();return false;}
  }
  if(stage==Stage::WaitBothTriggers){
   if(both){pressTick=f.tick;stage=Stage::Clenching;}
   else if(release||f.tick<pressTick||f.tick-pressTick>1000)interrupt();
   return false;
  }
  if(stage==Stage::Clenching||stage==Stage::NeedNativeFists||stage==Stage::Qualified){
   if(!both){
    if(release){
     if(haveBest[0]&&haveBest[1]){result={rig,open,best};if(valid(result)){stage=Stage::Ready;offered=true;return true;}}
     // A release without two qualified hands never replaces the saved pair.
     interrupt();
    }return false;
   }
   if(f.tick<pressTick||f.tick-pressTick<350)return false;
   for(int s=0;s<2;++s)if(stable[s].push(f.hand[s],f.tick)&&clenched(open[s],stable[s].latest)){
    double score=0,old=0;for(int d=1;d<5;++d){score+=stable[s].latest.bend[d];old+=best[s].bend[d];}
    // Select a coherent WHOLE HAND from the intended, settled native clench interval.
    // No per-joint extrema, wrist compactness metric, or left/right mirroring.
    if(!haveBest[s]||score>old){best[s]=stable[s].latest;haveBest[s]=true;}
   }
   stage=(haveBest[0]&&haveBest[1])?Stage::Qualified:Stage::NeedNativeFists;
  }
  return false;
 }
 bool candidate(Pair& p) const noexcept {if(!haveOpen||!haveBest[0]||!haveBest[1])return false;p={rig,open,best};return valid(p);}
};
// F10 1.5 is a substage of existing arms-only mode 2; native 0..4 IDs are unchanged.
}
