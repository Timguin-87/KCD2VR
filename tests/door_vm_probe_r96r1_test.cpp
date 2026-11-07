// Actual H40 CandidatePress/ServiceState. The script reply and current-pose
// permission are explicit native leaves; no retail Lua or headset execution.
#include "subsystems/doors/hand_only_h40.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace h=kcd2vr::hand_only_door_h40;
namespace m=kcd2vr::mutual_admission_t25;
static unsigned checks=0,failures=0;
static void check(bool ok,const char* why){++checks;if(!ok){++failures;std::printf("FAIL_DOOR_VM_R96R1 %s\n",why);}}
static bool current(const h::Frame&,int,std::uint64_t) noexcept{return true;}
struct IO {
 bool installed=true,rest=false,legacy=false;
 unsigned probes=0,feeds=0,clears=0,held=0;
 std::vector<std::string> operations;
 bool call(const char* in,char* out,std::size_t cap) noexcept {
  unsigned long long serial=0,body=0,request=0;unsigned player=0;char action[16]{};
  if(std::sscanf(in,"%llu|%15[^|]|%u|%llu|%llu",&serial,action,&player,&body,&request)!=5)return false;
  operations.emplace_back(action);
  if(!std::strcmp(action,"probe"))++probes;
  else if(!std::strcmp(action,"feed"))++feeds;
  else if(!std::strcmp(action,"clear"))++clears;
  else return false;
  // Native T2 CallPacket returns false if the new VM has not installed the
  // named function. It cannot provide a successful native entry receipt.
  if(!installed)return false;
  if(!std::strcmp(action,"feed"))held=rest?0:h::Reserved();
  else if(!std::strcmp(action,"clear"))held=0;
  const unsigned mask=!std::strcmp(action,"probe")?0:held;
  if(legacy)std::snprintf(out,cap,"OK|%llu|%u|%llu|%llu|%u|0|%llu|%u|%u",serial,player,body,request,mask,serial,mask&1?101:0,mask&2?102:0);
  else std::snprintf(out,cap,"H40R1|%llu|%u|%llu|%llu|%u|0|%llu|%u|%u|%u",serial,player,body,request,mask,serial,mask&1?101:0,mask&2?102:0,
    rest&&std::strcmp(action,"probe")?1u:0u);
  return true;
 }
};
static h::Frame frame(std::uint64_t tick,float grip){
 h::Frame f{};f.tick=tick;f.body=9;f.request=10;f.sample=tick;f.tracking=1;f.policy=2;
 f.actor=100;f.character=200;f.nativePlayer=300;f.manager=400;f.player=42;
 f.permitted=true;f.available[0]=f.available[1]=true;
 f.grip[0]=f.grip[1]=grip;f.point[0][2]=f.point[1][2]=1;
 return f;
}
static void reset(){
 h::service={};h::ready=false;h::enabled=h::transportInstalled=true;h::vmQuarantine=false;
 h::clockR96=nullptr;h::currentReader=&current;h::inputIdentityReader=nullptr;h::nearTick=0;
 h::nextClaim=1;h::admissionBusy=h::resting=0;h::displayEnabled=false;
 for(int s=0;s<2;++s){h::claims[s]=h::entered[s]=h::acquiredD49[s]=h::fallbackToken[s]=0;
  h::neutral[s]=true;h::pendingInput[s]={};h::nativeFallback[s]={};h::pressWhy[s]={};h::lastBusyR9[s]=0;
  m::active[s]=m::doorClaimsH41[s]=0;}
 h::PublishCompleted(frame(1000,.1f));
}
static std::uint64_t pending(int side,IO& io){
 reset();check(h::CandidatePress(side,101+side,1000,true,.8f,0),"raw press reserves before completed grip");
 const auto token=h::claims[side].load();h::service.tick(io,1000,1);
 check(io.probes==1&&!io.feeds&&!h::Entered()&&h::Reserved()==(1u<<side),"first VM probes but neutral completed grip does not enter native action");
 check(h::service.probeToken[side]==token&&h::ready,"same original token has successful first-VM readiness");
 return token;
}
int main(){
 for(int side=0;side<2;++side){
  IO first;const auto token=pending(side,first);
  h::PublishCompleted(frame(1010,.8f));IO missing;missing.installed=false;
  h::service.tick(missing,1010,2);
  check(missing.probes==1&&missing.feeds==0,"replacement VM must probe original token before any feed");
  check(!h::Entered()&&h::claims[side]==token&&!h::vmQuarantine,"failed replacement probe cannot create entered native debt");
  h::Candidate c{};check(h::candidates[side].read(c)&&c.token==token&&c.tick==1000,"VM change does not renew original press timestamp");
  h::PublishCompleted(frame(1020,.8f));IO third;
  h::service.tick(third,1020,3);
  check(third.probes==1&&third.feeds==1&&!h::vmQuarantine,"next qualified VM recovers never-entered original press");
  check(h::Acquired()==(1u<<side)&&h::claims[side]==token&&!h::service.fault,"actual current reply acquires same still-fresh token once");

  IO unchanged;pending(side,unchanged);h::PublishCompleted(frame(1010,.8f));h::service.tick(unchanged,1010,1);
  check(unchanged.probes==1&&unchanged.feeds==1,"same VM reuses its own readiness without repeated probe");
  const auto calls=unchanged.operations.size();h::service.tick(unchanged,1010,2);
  check(h::vmQuarantine&&h::Entered()==(1u<<side)&&unchanged.operations.size()==calls,"actual entered old-VM debt remains quarantined with no foreign call");

  IO version;pending(side,version);h::PublishCompleted(frame(1010,.8f));IO legacy;legacy.legacy=true;
  h::service.tick(legacy,1010,2);
  check(legacy.probes==1&&legacy.feeds==1&&!h::service.restProtocol,"replacement probe supplies its own protocol version");

  IO rested;pending(side,rested);rested.rest=true;h::PublishCompleted(frame(1010,.8f));h::service.tick(rested,1010,1);
  check(!h::Reserved()&&h::resting==1,"same-VM versioned rest frees only hand claim");
  const auto restCalls=rested.operations.size();h::service.tick(rested,1010,2);
  check(h::vmQuarantine&&h::resting==1&&rested.operations.size()==restCalls,"replacement VM cannot acknowledge old detached rest");

  IO expire;pending(side,expire);IO unavailable;unavailable.installed=false;h::PublishCompleted(frame(1010,.8f));h::service.tick(unavailable,1010,2);
  h::PublishCompleted(frame(1020,.8f));h::service.tick(unavailable,1020,2);
  check(unavailable.probes==1&&!unavailable.feeds&&!h::Reserved()&&!h::Entered(),"failed current-VM probe is not retried every frame and unentered claim retires");
 }
 std::printf("R96R1 actual H40 per-VM readiness: %u checks, %u failures; script/native leaves controlled\n",checks,failures);
 return failures?1:0;
}
