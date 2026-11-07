#include "../src/core/door_channel19c4r4.h"
#include <cassert>
#include <cstdio>
#include <thread>
#include <atomic>
using namespace kcd2vr;
static door_channel19c4r4::Channel g_channel19c4r4;
static int originalCalls=0,nameCalls=0,endCalls=0,clockCalls=0;
static void* seenSelf=nullptr;
static void* seenHandler=nullptr;
static const char* nextName=nullptr;
static bool readable=true;
static std::uint64_t clockValue=1000;
static std::uint32_t returnedValue=0;
static int Original(void* self,void* handler){++originalCalls;seenSelf=self;seenHandler=handler;return 77;}
static auto g_get_cvar_original19c4r4=&Original;
static bool DoorNativeName19c4r4(void*,char* out,std::size_t cap){
 ++nameCalls;if(!readable||!nextName)return false;
 if(std::strlen(nextName)>=cap)return false;
 std::strcpy(out,nextName);return true;
}
static int DoorNativeEndInteger19c4r4(void*,std::uint32_t value){++endCalls;returnedValue=value;return 1;}
static std::uint64_t DoorClock19c4r4(){++clockCalls;return clockValue;}
#ifndef _MSC_VER
#define __fastcall
#endif
#include "../src/core/door_signal_hook19c4r4.inl"
static unsigned checks=0;
static void check(bool value){++checks;assert(value);}
using Channel=door_channel19c4r4::Channel;
using Command=door_script19c4r4::Command;
static Command command(std::uint32_t seq=2,std::uint32_t request=1,int phase=1){return {seq,request,100,phase,.25f};}
static std::uint32_t field(Channel& c,const char* name,std::uint64_t now=1000){std::uint32_t v=999;check(c.field(name,now,v));return v;}
int main(){
 auto* self=reinterpret_cast<void*>(1);auto* handler=reinterpret_cast<void*>(2);
 nextName="r_DisplayInfo";
 check(DoorGetCVarDetour19c4r4(self,handler)==77);
 check(originalCalls==1&&nameCalls==0&&endCalls==0&&seenSelf==self&&seenHandler==handler);
 g_channel19c4r4.enabled(true);
 check(DoorGetCVarDetour19c4r4(self,handler)==77);
 check(originalCalls==2&&nameCalls==1&&endCalls==0);
 readable=false;check(DoorGetCVarDetour19c4r4(self,handler)==77);check(originalCalls==3);readable=true;
 nextName="kcd2vr_door_version";check(DoorGetCVarDetour19c4r4(self,handler)==1);check(returnedValue==1966&&endCalls==1&&originalCalls==3);
 nextName="kcd2vr_door_unknown";check(DoorGetCVarDetour19c4r4(self,handler)==1);check(returnedValue==0&&originalCalls==3);
 check(!g_channel19c4r4.companion46());
 nextName="kcd2vr_door_companion_66";check(DoorGetCVarDetour19c4r4(self,handler)==1&&returnedValue==66);
 check(g_channel19c4r4.companion46()); // Actual private Lua getter, not hook-installed optimism.
 check(field(g_channel19c4r4,"kcd2vr_door_seq")==2);
 check(field(g_channel19c4r4,"kcd2vr_door_request")==0);
 check(g_channel19c4r4.publish(command(),1000,0x1234));
 nextName="kcd2vr_door_claim_2_1";check(DoorGetCVarDetour19c4r4(self,handler)==1&&returnedValue==1);
 check(g_channel19c4r4.pending());
 g_channel19c4r4.enabled(false);nextName="kcd2vr_door_phase";
 check(DoorGetCVarDetour19c4r4(self,handler)==1&&returnedValue==0);
 nextName="kcd2vr_door_ack_2_1_1_250000";check(DoorGetCVarDetour19c4r4(self,handler)==1&&returnedValue==0);
 clockValue=2000;nextName="kcd2vr_door_ack_2_1_0_250000";
 check(DoorGetCVarDetour19c4r4(self,handler)==1&&returnedValue==1&&!g_channel19c4r4.pending());
 // Once cleanup is retired an off bridge resumes its exact native fast path.
 const int names=nameCalls;check(DoorGetCVarDetour19c4r4(self,handler)==77&&nameCalls==names);
 {
  Channel c;c.enabled(true);check(!c.publish(command(),0,0x1234));check(c.publish(command(),1000,0x1234));
  check(!c.publish(command(),1001,0x1234));check(!c.claim(2,2,1001));check(!c.claim(4,1,1001));
  check(!c.claim(2,1,1081));check(!c.pending());
  check(c.claim(2,1,1080));check(!c.claim(2,1,1080));
  check(!c.acknowledge(2,1,1,250000,1081));check(c.acknowledge(2,1,1,250000,1080));
  door_script19c4r4::Ack a{};check(c.ack(a,1081)&&a.request==1&&a.state==1&&a.fraction==.25f);
  check(!c.ack(a,1079));check(!c.ack(a,1161));
  check(!c.publish(command(4,2),1080,0x1234));check(c.publish(command(4,1,2),1080,0x1234));
  check(c.acknowledge(4,1,1,250000,1081));
  check(!c.acknowledge(6,1,0,0,1081));check(!c.acknowledge(4,2,0,0,1081));
  c.enabled(false);check(c.pending());check(!c.claim(4,1,1081));check(!c.acknowledge(4,1,1,0,1081));
  check(c.acknowledge(4,1,0,250000,5000)&&!c.pending());
  check(c.acknowledge(4,1,0,250000,5001));check(!c.acknowledge(2,1,0,0,5001));
  c.enabled(true);check(c.publish(command(6,2),5002,0x1234));check(!c.claim(2,1,5002));check(c.claim(6,2,5002));
  check(!c.acknowledge(4,1,1,0,5002));check(c.acknowledge(6,2,0,0,5002));
 }
 for(int invalid=0;invalid<11;++invalid){
  Channel c;c.enabled(true);auto q=command();
  switch(invalid){case 0:q.sequence=1;break;case 1:q.sequence=8388608;break;case 2:q.sequence=3;break;
   case 3:q.request=0;break;case 4:q.request=8388608;break;case 5:q.target=0;break;
   case 6:q.target=16777216;break;case 7:q.phase=-1;break;case 8:q.phase=3;break;
   case 9:q.fraction=1.f;break;case 10:q.fraction=std::nanf("");break;}
  check(!c.publish(q,1000,0x1234));
 }
 for(const char* name:{"kcd2vr_door_claim_99999999_1","kcd2vr_door_claim_2_8388608","kcd2vr_door_claim_2_1x",
  "kcd2vr_door_claim__1","kcd2vr_door_claim_2_1_","kcd2vr_door_ack_2_1_2_0","kcd2vr_door_ack_2_1_1_999501",
  "kcd2vr_door_ack_2_1_1_000000000","kcd2vr_door_ack_2_1_1_-1"}){
  Channel c;c.enabled(true);check(c.publish(command(),1000,0x1234));check(field(c,name)==0&&!c.pending());
 }
 {
  Channel c;c.enabled(true);auto q=command();q.target=16777215;q.request=8388607;q.sequence=8388606;
  check(c.publish(q,1000,0x1234));check(field(c,"kcd2vr_door_target_hi")==255);
  check(field(c,"kcd2vr_door_target_lo")==65535);check(field(c,"kcd2vr_door_fraction")==250000);
  check(c.claim(q.sequence,q.request,1000));check(c.acknowledge(q.sequence,q.request,0,999500,1000));
 }
 {Channel c;check(field(c,"kcd2vr_door_companion_66")==0&&!c.companion46());
  c.enabled(true);check(c.publish(command(),1000,0));check(!c.claim(2,1,1000)&&!c.pending());}
 // Known retirement is local identity cleanup, never an ACK/rate assertion.
 {
  Channel c;c.enabled(true);check(c.publish(command(),1000,0x1234));check(c.claim(2,1,1000));
  door_script19c4r4::CleanupIdentity lease{};check(c.cleanup(lease)&&lease.entity==0x1234&&lease.target==100);
  check(!c.retireKnownAbsent(lease,0x1234)&&c.pending()); // same-address ABA is unknown
  auto wrong=lease;++wrong.request;check(!c.retireKnownAbsent(wrong,0)&&c.pending());
  wrong=lease;++wrong.target;check(!c.retireKnownAbsent(wrong,0)&&c.pending());
  wrong=lease;++wrong.source;check(!c.retireKnownAbsent(wrong,0)&&c.pending());
  wrong=lease;++wrong.epoch;check(!c.retireKnownAbsent(wrong,0)&&c.pending());
  wrong=lease;++wrong.entity;check(!c.retireKnownAbsent(wrong,0)&&c.pending());
  c.enabled(false);check(c.retireKnownAbsent(lease,0)&&!c.pending()&&!c.enabled());
  door_script19c4r4::Ack a{};check(!c.ack(a,1000));
  check(!c.claim(2,1,1000)&&!c.acknowledge(2,1,1,0,1000));
  check(!c.acknowledge(2,1,0,0,1000)); // no fabricated normal cleanup receipt
  c.enabled(true);check(c.publish(command(4,2),1001,0x5678));check(c.claim(4,2,1001));
  check(!c.retireKnownAbsent(lease,0)&&c.pending()); // old token cannot retire new lease
  check(c.cleanup(lease)&&c.retireKnownAbsent(lease,0x7777)&&!c.pending());
 }
 // Stop/claim must either deny the pause or leave a visible cleanup lease.
 for(unsigned n=0;n<300;++n){
  Channel c;c.enabled(true);check(c.publish(command(),1000,0x1234));std::atomic<bool> start{false};bool granted=false;
  std::thread t([&]{while(!start.load())std::this_thread::yield();granted=c.claim(2,1,1000);});
  start.store(true);c.enabled(false);t.join();check(!granted||c.pending());
  check(!c.enabled());if(c.pending())check(c.acknowledge(2,1,0,0,2000));
 }
 // D48: companion logging reads finite recorder state without enabling any
 // door owner or registering/mutating a CVar. Exercise the ACTUAL callback.
 {
  namespace cap=diagnostics::capture168;
  g_channel19c4r4.enabled(false);check(!g_channel19c4r4.pending());
  clockValue=10000;nextName="kcd2vr_door_capture_f5";
  const int namesOff=nameCalls,clocksOff=clockCalls;
  check(DoorGetCVarDetour19c4r4(self,handler)==77&&nameCalls==namesOff&&clockCalls==clocksOff);
  const cap::Domain domains[]{cap::Domain::Bug,cap::Domain::Combat,cap::Domain::Interaction,cap::Domain::Vrik,cap::Domain::Reload};
  const char* queries[]{"kcd2vr_door_capture_f2","kcd2vr_door_capture_f3","kcd2vr_door_capture_f5","kcd2vr_door_capture_f7","kcd2vr_door_capture_f11"};
  for(unsigned i=0;i<5;++i){
   cap::gate.select(domains[i],true,clockValue);
   for(unsigned j=0;j<5;++j){nextName=queries[j];check(DoorGetCVarDetour19c4r4(self,handler)==1&&returnedValue==(i==j?1u:0u));}
   for(const char* malformed:{"kcd2vr_door_capture_","kcd2vr_door_capture_f8","kcd2vr_door_capture_F5","kcd2vr_door_capture_f5_1"}){
    nextName=malformed;check(DoorGetCVarDetour19c4r4(self,handler)==1&&returnedValue==0);
   }
   check(!g_channel19c4r4.enabled()&&!g_channel19c4r4.pending());
   nextName="kcd2vr_door_version";check(DoorGetCVarDetour19c4r4(self,handler)==1&&returnedValue==0);
   nextName="r_DisplayInfo";check(DoorGetCVarDetour19c4r4(self,handler)==77);
   nextName=queries[i];clockValue+=cap::Duration;
   check(DoorGetCVarDetour19c4r4(self,handler)==1&&returnedValue==0); // expiry does not require worker poll first
   cap::gate.poll(clockValue);
   const int namesAfter=nameCalls,clocksAfter=clockCalls;
   check(DoorGetCVarDetour19c4r4(self,handler)==77&&nameCalls==namesAfter&&clockCalls==clocksAfter);
   ++clockValue;
  }
 }
 std::printf("door actual getter/channel %u checks PASS\n",checks);
}
