#pragma once
#include "native_door_hold65.h"
namespace kcd2vr::door_script19c4r4 {
// Grip is attachment only. The IO projects current controller intent onto the
// exact resident door-leaf trajectory; no wall-clock autoplay exists here.
struct MotionOwner66 {
 Request request{};door_hold65::Hand source{};std::uint64_t seen=0,press=0,bindingSince=0;
 std::uint32_t sequence=0;bool pending=false,active=false,paused=true,finishing=false,rebind=true,everBound=false;
 float wanted=0,reported=0;unsigned reports=0;
 template<class IO> bool command(IO& io,int phase) noexcept {
  if(sequence>8388602u)return false;sequence+=2;
  return io.publish(Command{sequence,std::uint32_t(request.serial),request.id,phase,wanted});
 }
 void clearOperation() noexcept {(void)door_hold65::operation.publish({});}
 template<class IO> void release(IO& io,const char* why) noexcept {
  if(active||pending){(void)command(io,0);io.receipt(request,why,wanted,false);}
  active=pending=finishing=false;paused=rebind=true;press=0;clearOperation();io.retire66(request);
 }
 template<class IO> void tick(IO& io,const Request& incoming,std::uint64_t now) noexcept {
  if(incoming.serial&&incoming.serial!=seen){release(io,"REPLACED");seen=incoming.serial;request=incoming;
   source={};pending=true;paused=rebind=true;finishing=everBound=false;press=0;wanted=reported=0;reports=0;bindingSince=now;}
  if(!pending&&!active)return;
  door_hold65::Hand hand{};
  if(!request.serial||request.serial>8388607ull||!request.id||request.id>16777215u||!request.tick||now<request.tick||
     (pending&&now-request.tick>250)||!io.control65(request,hand)||!door_hold65::current(hand,now)){
   release(io,"CONTEXT_OR_CURRENT_SOURCE_LOST");return;}
  if(!source.player){if(hand.grip<.30f||!hand.press||(request.inputPress65&&hand.press!=request.inputPress65)){
    release(io,"INITIAL_GRIP_RELEASED_OR_REPLACED");return;}source=hand;press=hand.press;
  }else if(!door_hold65::sameSource(source,hand)){release(io,"SOURCE_FAMILY_CHANGED");return;}
  Ack ack{};const bool current=io.ack(ack)&&ack.request==request.serial&&ack.source&&ack.source<=sequence&&
   door_hold65::fresh(ack.tick,now)&&Owner::fraction(ack.fraction);
  if(active&&current&&ack.state==2){wanted=ack.fraction;io.receipt(request,"NATIVE_OPERATION_COMPLETED",wanted,true);
   active=pending=finishing=false;paused=true;press=0;clearOperation();io.retire66(request);return;}
  if(pending){if(current&&ack.state==1){wanted=reported=ack.fraction;pending=false;active=true;paused=false;bindingSince=now;
    io.receipt(request,"NATIVE_PAUSE_CONFIRMED",wanted,true);
   }else{if(!command(io,1))release(io,"CLAIM_SIGNAL_REFUSED");return;}}
  if(!current||ack.state!=1){release(io,"LUA_ACK_STALE_OR_RELEASED");return;}
  if(!everBound&&!finishing&&(now<bindingSince||now-bindingSince>250)){
   release(io,"RESIDENT_HANDLE_CURVE_UNAVAILABLE");return;}
  if(reports<16&&std::fabs(ack.fraction-reported)>=.1f){reported=ack.fraction;++reports;io.receipt(request,"LUA_PROGRESS_READBACK",ack.fraction,true);}
  if(finishing){if(!command(io,5))release(io,"ENDPOINT_SIGNAL_REFUSED");return;}
  const bool held=hand.grip>=.30f&&hand.press;const bool wasPaused=paused;
  if(!held||(!paused&&hand.press!=press)){paused=true;press=0;rebind=true;wanted=ack.fraction;}
  door_hold65::Operation op{request,hand,now,paused};
  if(paused&&held){door_hold65::Resume resume{};
   if(door_hold65::resume.read(resume)&&door_hold65::resumeMatches(resume,op,hand,now)){
    paused=false;press=hand.press;rebind=true;bindingSince=now;op.paused=false;}}
  if(!door_hold65::operation.publish(op)){release(io,"SESSION_PUBLICATION_REFUSED");return;}
  if(wasPaused!=paused)io.receipt(request,paused?"GRIP_RELEASE_PAUSE_REQUEST":"SAME_DOOR_GRIP_RESUME_REQUEST",wanted,true);
  if(paused){io.detach66(request);if(!held&&(everBound||wanted<=.0005f)&&(wanted<=.0005f||wanted>=.999f)){
    // An immediate release at the proven native clip's ORIGINAL start cancels
    // that Use back to its original native state. It is not a physical-curve
    // endpoint assertion; no controller motion or successful bind is invented.
    finishing=true;io.receipt(request,everBound?"RELEASE_AT_NATIVE_ENDPOINT":"EARLY_RELEASE_NATIVE_START_CANCEL",wanted,true);
    if(!command(io,5))release(io,"ENDPOINT_SIGNAL_REFUSED");
   }else if(!command(io,4))release(io,"PAUSE_SIGNAL_REFUSED");return;}
  float next=ack.fraction;const int drive=io.drive66(request,hand,ack.fraction,rebind,next);
  if(drive<0){release(io,"RIGID_HANDLE_OR_CONTROLLER_PROOF_REFUSED");return;}
  if(!drive){if(now<bindingSince||now-bindingSince>250){release(io,"RESIDENT_HANDLE_CURVE_UNAVAILABLE");return;}
   if(!command(io,1))release(io,"BIND_WAIT_SIGNAL_REFUSED");return;}
  if(!Owner::fraction(next)){release(io,"PROJECTED_PHASE_INVALID");return;}
  rebind=false;everBound=true;wanted=next;
  if(!command(io,2))release(io,"CONTROLLER_TIME_SIGNAL_REFUSED");
 }
};
}
