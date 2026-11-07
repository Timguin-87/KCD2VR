#define __fastcall
#include "subsystems/interactions/quick_talk_service_d78.h"
#include <cassert>
#include <cstdio>
#include <cstring>
namespace q=kcd2vr::quick_talk_target_d78;
static q::Body body{11,22,33,1000,44,55};
static bool bodyValid=true,contextValid=true,sourceValid=true,replyYes=true;
static unsigned calls=0;static void (*duringCall)()=nullptr;
namespace kcd2vr::frik_pose_probe {bool ReadMiscGestureBodyD59(MiscGestureBodyD59& out) noexcept {out=body;return bodyValid;}}
namespace kcd2vr::interaction_alignment {
bool DoorAcquisitionCurrent65(std::uint64_t generation) noexcept {return sourceValid&&generation==66;}
std::uint64_t DoorAcquisitionGeneration65() noexcept {return sourceValid?66:0;}
struct Candidate57c1 {std::uintptr_t player=101;std::uint64_t tick=1000;std::uint32_t id=77;char cls[32]="NPC";};
static bool playerVft=true,entityVft=true,entityMapped=true,entityTurnover=false;static unsigned playerReads=0;
static bool SelectionVft68(std::uintptr_t p,std::uintptr_t rva){
 return (p==101&&rva==0x3AA4B98&&playerVft)||(p==11&&rva==0x4072C78&&entityVft);
}
static bool SelectionRead68(std::uintptr_t p,std::uintptr_t& out){
 ++playerReads;out=entityTurnover&&playerReads>1?12:11;return p==101+0x38&&entityMapped;
}
#include "subsystems/interactions/quick_talk_candidate_d78.inl"
}
namespace kcd2vr::native_script_t2 {
bool Context::Current() const noexcept {return contextValid;}
bool Context::CallPacket(const char* function,const char* input,char* out,std::size_t size) const noexcept {
 ++calls;assert(!std::strcmp(function,"kcd2vr_chat_target_d78"));unsigned long long serial=0;unsigned player=0,target=0;
 assert(std::sscanf(input,"CHAT|%llu|%u|%u",&serial,&player,&target)==3&&player==33);
 std::snprintf(out,size,"CHAT|%llu|%u|%d",serial,target,replyYes?1:0);
 if(duringCall){auto callback=duringCall;duringCall=nullptr;callback();}return true;
}}
static void reset(){
 hostTickCount64=1000;
 (void)q::Prepare(false,1000);q::requests.publish({});q::receipts.publish({});q::candidate.publish({11,1000,66,77});
 body={11,22,33,1000,44,55};bodyValid=contextValid=sourceValid=replyYes=true;duringCall=nullptr;calls=0;
}
static void service(){q::Service({1,2,1000,1});}
int main(){
 reset();assert(!q::Prepare(true,1000)&&q::Wanted());service();assert(calls==1&&q::Prepare(true,1010));
 q::candidate.publish({11,1010,66,78});assert(!q::Prepare(true,1010)); // previous NPC cannot lend eligibility
 for(int fault=0;fault<9;++fault){reset();assert(!q::Prepare(true,1000));switch(fault){
  case 0:replyYes=false;break;case 1:sourceValid=false;break;case 2:bodyValid=false;break;
  case 3:body.body=45;break;case 4:contextValid=false;break;
  case 5:duringCall=+[](){++body.tracking;};break;
  case 6:duringCall=+[](){q::candidate.publish({11,1000,66,78});};break;
  case 7:duringCall=+[](){q::Prepare(false,1000);};break;
  case 8:duringCall=+[](){contextValid=false;};break;
 }service();assert(!q::Prepare(true,1010));}
 reset();q::candidate.publish({12,1000,66,77});assert(!q::Prepare(true,1000)&&!q::Wanted()&&calls==0);
 reset();assert(!q::Prepare(true,1000));service();assert(!q::Prepare(true,1081));
 reset();assert(!q::Prepare(true,1000));hostTickCount64=1081;service();
 assert(calls==0&&!q::Prepare(true,1081)); // previous dispatcher clients consumed the receipt lease
 reset();assert(!q::Prepare(true,1000));duringCall=+[](){hostTickCount64=1081;};service();
 q::Receipt late{};assert(q::receipts.read(late)&&!late.eligible&&!q::Prepare(true,1081));
 // Execute the actual existing-policy observer with explicit native identity
 // leaves. Publishing a candidate never publishes descriptor eligibility.
 namespace ia=kcd2vr::interaction_alignment;
 for(int fault=-1;fault<9;++fault){
  reset();q::candidate.publish({});assert(!q::Prepare(true,1000));
  ia::playerVft=ia::entityVft=ia::entityMapped=true;ia::entityTurnover=false;ia::playerReads=0;
  ia::Candidate57c1 native{};switch(fault){
   case 0:std::strcpy(native.cls,"Dog");break;case 1:native.tick=919;break;
   case 2:ia::playerVft=false;break;case 3:ia::entityVft=false;break;
   case 4:ia::entityMapped=false;break;case 5:ia::entityTurnover=true;break;
   case 6:sourceValid=false;break;case 7:q::wantedUntil=0;break;
   case 8:std::strcpy(native.cls,"Horse");break;
  }
  ia::ObserveQuickTalkCandidateD78(native,true,1000);q::Candidate observed{};
  assert(q::candidate.read(observed));assert((observed.entity==77)==(fault<=0));
  if(fault<0){assert(observed.player==11&&observed.generation==66);ia::ObserveQuickTalkCandidateD78(native,false,1000);
   assert(q::candidate.read(observed)&&observed.entity==0);}
 }
 std::puts("PASS actual chat receipt service: exact NPC-or-Dog/player/body/source/generation, stale and context refusals, no native action calls");
}
