#pragma once
#include "native_door19c4.h"
namespace kcd2vr::door_script19c4r4 {
using door19c4::Point;using door19c4::Request;
struct Command {std::uint32_t sequence=0,request=0,target=0;int phase=0;float fraction=0;int mode69=2;};
// Internal native identity only; never exposed as a Lua number or dereferenced.
struct CleanupIdentity {std::uint32_t request=0,target=0,source=0;std::uint64_t epoch=0;std::uintptr_t entity=0;};
struct Ack {std::uint32_t source=0,request=0;int state=0;float fraction=0;std::uint64_t tick=0;};
// C++ only publishes POD. The exact native door's Lua OnUpdate owns all animation
// setters and original completion; no cross-character animation call lives here.
struct Owner {
 Request request{};Point axis{},anchor{};std::uint64_t seen=0;std::uint32_t sequence=0;
 bool pending=false,active=false,haveAxis=false;float initial=0,wanted=0;
 // Copied Lua ACK progress only; receipt never grants or renews ownership.
 float reported64=0;unsigned reports64=0;
 static bool fraction(float f) noexcept{return std::isfinite(f)&&f>=0&&f<=.9995f;}
 template<class IO> bool command(IO& io,int phase,float t) noexcept {
  if(sequence>8388602u)return false;sequence+=2;
  return io.publish(Command{sequence,std::uint32_t(request.serial),request.id,phase,t});
 }
 template<class IO> void release(IO& io,const char* reason) noexcept {
  if(active||pending){(void)command(io,0,wanted);io.receipt(request,reason,wanted,false);}
  pending=active=haveAxis=false;
 }
 template<class IO> void tick(IO& io,const Request& incoming,std::uint64_t now) noexcept {
  if(incoming.serial&&incoming.serial!=seen){
   release(io,"REPLACED");seen=incoming.serial;request=incoming;pending=true;wanted=initial=0;haveAxis=false;reports64=0;
  }
  if(!active&&!pending)return;
  Point hand{};
  if(!request.serial||request.serial>8388607ull||!request.id||request.id>16777215u||!request.tick||now<request.tick||
     now-request.tick>(active?15000u:250u)||!io.allowed(request)||!io.hand(request,hand)||
     !door19c4::finite(hand)||!door19c4::finite(request.start)){
   release(io,"RELEASE_OR_CURRENT_OWNER_LOST");return;
  }
  Ack ack{};const bool current=io.ack(ack)&&ack.request==request.serial&&ack.source&&ack.source<=sequence&&
    ack.tick&&now>=ack.tick&&now-ack.tick<=80&&fraction(ack.fraction);
  if(pending){
   if(current&&ack.state==1){
    initial=wanted=reported64=ack.fraction;active=true;pending=false;
    // Native admission/approach and the first real door update can be separated
    // by several frames. Motion before the confirmed pause is not work.
    anchor=request.inputToken46?hand:request.start;
    io.receipt(request,"LUA_OWN_UPDATE_HELD",wanted,true);
   }
   else{if(!command(io,1,0))release(io,"SIGNAL_REFUSED");return;}
  }
  if(!current||ack.state!=1){release(io,"LUA_ACK_STALE_OR_RELEASED");return;}
  // Requested motion is not achieved motion. Log only a significant change in
  // a fresh, matching Lua readback, at most16 times per accepted door lifetime.
  // Production receipt() additionally requires the finite F5 recorder.
  if(reports64<16&&std::fabs(ack.fraction-reported64)>=.1f){
   reported64=ack.fraction;++reports64;io.receipt(request,"LUA_PROGRESS_READBACK",ack.fraction,true);
  }
  const auto delta=door19c4::sub(hand,anchor);const float d2=door19c4::dot(delta,delta);
  if(!std::isfinite(d2)||d2>1.44f){release(io,"HAND_LEFT_DOOR");return;}
  if(!haveAxis&&d2>=.0009f){const float inv=1/std::sqrt(d2);axis={delta.x*inv,delta.y*inv,delta.z*inv};haveAxis=true;}
  if(haveAxis){
   const float travel=request.inputToken46?.30f:.55f;
   const float next=std::clamp(initial+door19c4::dot(delta,axis)/travel,0.f,.9995f);
   // Keep the command heartbeat alive while a stationary controller remains
   // still; sub-2-mm tracking jitter must not scrub the authored door.
   if(!request.inputToken46||std::fabs(next-wanted)>=.002f/travel)wanted=next;
  }
  if(!command(io,2,wanted))release(io,"SIGNAL_REFUSED");
 }
};
}
