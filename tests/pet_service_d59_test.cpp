#include "../src/subsystems/interactions/pet_service_d59.h"
#include <cassert>
#include <cstring>
using namespace kcd2vr::petting_d59;
static bool nativeCurrent=true,identityCurrent=true,transport=true;
static unsigned calls=0,reports=0;static char captured[128]{};
namespace kcd2vr::native_script_t2 {
bool Context::Current() const noexcept {return nativeCurrent;}
bool Context::CallPacket(const char*name,const char*input,char*out,std::size_t size)const noexcept {
 assert(!std::strcmp(name,"kcd2vr_pet_d59"));++calls;std::snprintf(captured,sizeof(captured),"%s",input);
 unsigned long long serial;unsigned player,target;
 assert(std::sscanf(input,"PET|%llu|%u|%u",&serial,&player,&target)==3);
 std::snprintf(out,size,"PET|%llu|%u|NATIVE_HANDLER_RETURNED",serial,target);return transport;
}
}
static bool read(const Frame&,int side,std::uint64_t)noexcept {assert(side==0);return identityCurrent;}
static void report(const char*str)noexcept {assert(std::strstr(str,"nativeAccept=UNCLAIMED"));++reports;}
static Frame frame(std::uint64_t tick,float x){
 Frame f{};f.tick=tick;f.body=2;f.tracking=3;f.policy=4;f.player=5;f.playerIdentity=6;f.permitted=true;
 for(int s=0;s<2;++s){auto&h=f.hand[s];h.tracked=h.empty=h.free=true;h.palm[0]=s?1.f:x;h.target=7;
  h.dogCandidate=!s;h.distance=.1f;h.candidateTick=tick;h.candidateGeneration=8;h.candidateContext=9;}
 return f;
}
static void gesture(std::uint64_t start){
 SetEnabled(false);SetEnabled(true);
 Publish(frame(start,0));Publish(frame(start+60,.04f));Publish(frame(start+120,.081f));
 assert(Wanted());
}
int main(){
 SetCurrentReader(read);SetReporter(report);
 for(unsigned test=0;test<7;++test){
  identityCurrent=nativeCurrent=transport=true;const auto start=1000+test*1000;
  gesture(start);const auto before=calls;
  kcd2vr::native_script_t2::Context ctx{};ctx.now=start+130;
  if(test==1)identityCurrent=false;
  if(test==2)nativeCurrent=false;
  if(test==3)ctx.now=start+201; // stale committed hand is cancelled
  if(test==4)SetEnabled(false);
  if(test==5){SetEnabled(false);SetEnabled(true);} // stale generation cannot revive
  if(test==6)transport=false; // ambiguous native outcome gets no replay
  Service(ctx);assert(!Wanted());
  assert(calls==before+(test==0||test==6?1:0));
  Service(ctx);assert(calls==before+(test==0||test==6?1:0));
 }
 assert(reports==7);
 SetEnabled(false);Publish(frame(9000,0));assert(!Wanted());
}
