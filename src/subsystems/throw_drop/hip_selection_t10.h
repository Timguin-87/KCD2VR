#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace kcd2vr::hip_t10 {
enum class Owner : unsigned char { None, Equipment, Dagger, Axe };
struct Grant {Owner owner=Owner::None;std::uint64_t key=0,generation=0,body=0,request=0,sample=0;};
struct Input {
 std::uint64_t key=0,generation=0,body=0,request=0,sample=0,tick=0;
 double grip=0,distance[3]{1e6,1e6,1e6},equipmentRadius=.24;
 bool allowed=false,equipment=false,stock=false,available[2]{true,true};
};
struct State {
 std::uint64_t key=0,generation=0,body=0,request=0,sample=0,tick=0;
 Owner owner=Owner::None;bool stock=false,closed=false,open=false;
};
inline Owner closest(const Input& i) noexcept {
 double best=1e6,second=1e6;Owner owner=Owner::None;
 for(int k=0;k<3;++k){
  const double radius=k==0?std::clamp(i.equipmentRadius,.10,.40):.18;
  if((k==0?(!i.equipment||!i.key||!i.generation):(!i.stock||!i.available[k-1]))||!std::isfinite(i.distance[k])||i.distance[k]<0||i.distance[k]>=radius)continue;
  if(i.distance[k]<best){second=best;best=i.distance[k];owner=static_cast<Owner>(k+1);}
  else second=std::min(second,i.distance[k]);
 }
 return second-best<.015?Owner::None:owner; // ambiguous boundary requires a clearer reach
}
inline Grant update(State& s,const Input& i) noexcept {
 if(!i.allowed||!i.body||!i.request||!i.sample||!i.tick||!std::isfinite(i.grip)||!std::isfinite(i.equipmentRadius)){
  s={};return {};}
 if(s.body!=i.body||s.request!=i.request||s.key!=i.key||s.generation!=i.generation||s.stock!=i.stock||
    i.tick<s.tick||(s.tick&&i.tick-s.tick>100)||i.sample<s.sample){s={};}
 s.key=i.key;s.generation=i.generation;s.body=i.body;s.request=i.request;s.stock=i.stock;s.tick=i.tick;s.sample=i.sample;
 const Owner nearest=closest(i);
 if(i.grip<.25){s.open=true;s.closed=false;s.owner=nearest;}
 else if(i.grip>.55&&!s.closed){
  s.closed=true;
  if(!s.open||nearest!=s.owner)s.owner=Owner::None;
  s.open=false;
 }
 Grant out{};out.owner=s.owner;out.body=i.body;out.request=i.request;out.sample=i.sample;
 if(out.owner==Owner::Equipment){out.key=i.key;out.generation=i.generation;}
 return out;
}
}
