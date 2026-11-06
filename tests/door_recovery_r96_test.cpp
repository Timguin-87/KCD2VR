// Production press admission, current native hand reader and script service.
// Native memory/tracking and the typed Lua reply are explicit host leaves;
// pinned retail Lua OnUsed/finish actions are tested separately.
#define main retainedDoorPressR96
#include "door_press_why_r9_test.cpp"
#undef main
#include <string>
static std::uint64_t clockNowR96=1000;static unsigned clockReadsR96=0;static bool advanceClockR96=false;
static std::uint64_t clockR96() noexcept {++clockReadsR96;return advanceClockR96&&clockReadsR96>1?clockNowR96+1:clockNowR96;}
struct DoorIOR96 {
 unsigned calls=0,starts=0,returns=0,held=0;bool loseReply=false;std::string last;
 bool call(const char* input,char* out,std::size_t cap) noexcept {
  ++calls;last=input;unsigned long long serial=0,body=0,request=0;unsigned player=0;
  char action[16]{};if(std::sscanf(input,"%llu|%15[^|]|%u|%llu|%llu",&serial,action,&player,&body,&request)!=5)return false;
  if(!std::strcmp(action,"feed")){if(!held){held=h::Reserved();++starts;}}
  else if(!std::strcmp(action,"clear")){if(held){held=0;++returns;}}
  else if(std::strcmp(action,"probe"))return false;
  std::snprintf(out,cap,"H40R1|%llu|%u|%llu|%llu|%u|0|%llu|%u|%u|0",serial,player,body,request,held,serial,held&1?101:0,held&2?102:0);
  if(loseReply){loseReply=false;return false;}return true;
 }
};
static void completeR96(std::uint64_t tick,std::uint64_t body=9,std::uint64_t request=10){
 auto f=frame();f.tick=tick;f.body=body;f.request=request;f.sample=tick;
 tracking.tick=tick;tracking.sequence=tick;auto e=PhysEquipment6698{};e.tick=tick;e.epoch=body;e.request=request;
 g_pose_epoch6689=body;g_weapon_request_epoch6694=request;g_phys_equipment6698.publish(e);h::PublishCompleted(f);
}
static void setupR96(){h::clockR96=nullptr;resetR9();h::resting=0;h::acquiredD49[0]=h::acquiredD49[1]=0;
 clockNowR96=1000;clockReadsR96=0;advanceClockR96=false;h::clockR96=&clockR96;completeR96(1000);}
int main(){
 setupR96();tracking.tick=1001;PhysEquipment6698 e{};e.tick=1001;g_phys_equipment6698.publish(e);advanceClockR96=true;
 CK(HandOnlyDoorCurrentH40(frame(),1,1000));CK(clockReadsR96>=3);
 setupR96();tracking.tick=1001;CK(!HandOnlyDoorCurrentH40(frame(),1,1000)&&h::currentWhy[1]==4);
 setupR96();tracking.tick=919;CK(!HandOnlyDoorCurrentH40(frame(),1,1000)&&h::currentWhy[1]==4);
 for(int side=0;side<2;++side){
  setupR96();DoorIOR96 io;clockNowR96=1001;completeR96(1001);
  CK(h::CandidatePress(side,101+side,1000,true,.8f,0));
  h::Candidate c{};CK(h::candidates[side].read(c)&&c.tick==1000&&c.original.tick==1001);
  h::service.tick(io,1000,1);
  CK(io.starts==1&&io.held==(1u<<side)&&h::Acquired()==(1u<<side));
  // A later script transport refusal retains may-own, then the same original
  // native operation is explicitly cleared once; no fake empty acknowledgment.
  io.loseReply=true;clockNowR96=1020;completeR96(1020);h::service.tick(io,1019,1);
  CK(h::service.fault&&h::Reserved()==(1u<<side)&&io.starts==1);
  clockNowR96=1040;completeR96(1040);h::service.tick(io,1039,1);
  CK(!h::Reserved()&&!h::Entered()&&!h::service.fault&&io.returns==1);
  // A new saved-state body/request is independent of the old action. Do not
  // reset ServiceState, claims, serial or the native-operation spy here.
  clockNowR96=1060;completeR96(1060,10,11);
  CK(!h::CandidatePress(side,101+side,1060,true,.1f,0));
  CK(h::CandidatePress(side,101+side,1060,true,.8f,0));h::service.tick(io,1059,1);
  CK(io.starts==2&&io.held==(1u<<side)&&h::Acquired()==(1u<<side));
  g_ctx_menu=true;clockNowR96=1080;completeR96(1080,10,11);h::service.tick(io,1079,1);g_ctx_menu=false;
  CK(io.returns==2&&!h::Reserved()&&!h::Entered());
  // Cleared hand exclusion permits the real shared admission used by holster
  // and draw producers; completion is independent on each hand.
  auto next=m::TryBegin(side,m::Kind::Throw);CK(m::valid(next));CK(m::Finish(next,+[](void*)noexcept{},nullptr));
 }
 setupR96();DoorIOR96 io;CK(h::CandidatePress(1,102,1000,true,.8f,0));h::service.tick(io,1000,1);
 const auto calls=io.calls;h::service.tick(io,1000,2);
 CK(h::vmQuarantine&&h::Reserved()==2&&io.calls==calls&&io.returns==0); // replacement VM is not old native cleanup proof
 std::printf("R96 production door admission/current/service: %u checks PASS; both hands perform operations and recover\n",checks);
}
