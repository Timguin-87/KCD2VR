// Executes the ACTUAL production queue decoder and mode lease on synthetic memory.
// No WHGame, headset, Windows hooking or native queue synchronization is simulated.
#include "../src/diagnostics/playback_snapshot.h"
#include "../src/diagnostics/lab_state.h"
#include <vector>
#include <cstring>
#include <cstdio>
#include <limits>
#include <algorithm>
#include <string>
using namespace kcd2vr::diagnostics;
static int total=0,passed=0;
static void Check(bool ok,const char*name){++total;passed+=ok;std::printf("%s %s\n",ok?"PASS":"FAIL",name);}
struct Memory {
 std::vector<unsigned char> data=std::vector<unsigned char>(0x20000);
 static constexpr std::uintptr_t base=0x1000,queue=0x4004;
 int pointerReads=0;bool race=false,deny=false;
 template<class T> void put(std::uintptr_t a,T v){std::memcpy(data.data()+a,&v,sizeof(v));}
 bool read(std::uintptr_t a,void*out,std::size_t n){if(deny||a>data.size()||n>data.size()-a)return false;
  if(a==base+0x48&&++pointerReads==2&&race){std::uintptr_t q=queue+0x100;std::memcpy(out,&q,8);return true;}
  std::memcpy(out,data.data()+a,n);return true;}
 void layer(unsigned l,unsigned count){const std::uintptr_t q=queue+l*0x1000;put(base+l*0x40+0x48,q);put(q-4,std::uint32_t(count));put(base+l*0x40+0x5c,0.8f);}
 void clip(unsigned layer,unsigned index,int id,float time,bool activated,unsigned segment=0){const auto p=queue+layer*0x1000+index*0x78;put(p+0x10,id);put(p+0x44,time);put(p+0x50,std::uint32_t(activated));put(p+0x16,std::uint8_t(segment));}
 playback::Result capture(playback::Snapshot&s){pointerReads=0;return playback::Capture(base,[this](auto a,auto p,auto n){return read(a,p,n);},s);}
};
int main(){
 Memory m;playback::Snapshot s{},t{};
 Check(m.capture(s)==playback::Result::Ok&&s.count==0,"empty queue is a valid empty observation");
 m.layer(0,2);m.clip(0,0,0,.3f,true);m.clip(0,1,42,.0f,false);
 Check(m.capture(s)==playback::Result::Ok&&s.count==2,"already-running clips captured without a start hook");
 Check(s.entries[0].id==0&&s.entries[0].activated&&!s.entries[1].activated,"ID zero valid; queued and activated distinguished");
 Check(std::string(playback::PhaseRelation(s.entries[0],nullptr))=="FIRST_SAMPLE","first observation not proof of progression");
 m.clip(0,0,0,.4f,true);m.capture(t);
 Check(std::string(playback::PhaseRelation(t.entries[0],&s.entries[0]))=="ADVANCED","observed normalized-segment advance");
 Check(playback::SameMembership(s,t),"time advancement is not a new action identity");
 Check(std::string(playback::PhaseRelation(t.entries[0],&t.entries[0]))=="UNCHANGED","stationary sample not labelled moving");
 m.clip(0,0,0,.1f,true);m.capture(t);
 Check(std::string(playback::PhaseRelation(t.entries[0],&s.entries[0]))=="REWOUND_OR_LOOPED","rewind or loop is not guessed completion");
 m.clip(0,0,0,.1f,true,1);m.capture(t);
 Check(std::string(playback::PhaseRelation(t.entries[0],&s.entries[0]))=="SEGMENT_CHANGED","segment phase distinguished from whole clip phase");
 m.clip(0,1,42,.0f,true);m.capture(t);Check(!playback::SameMembership(s,t),"activation change detected");
 m.layer(4,1);m.clip(4,0,73,.5f,true);m.capture(t);Check(t.count==3&&t.entries[2].layer==4,"simultaneous layers retained rather than single last clip");
 m.put(Memory::queue+4*0x1000+8,std::uintptr_t(0x7777));m.capture(t);Check(t.entries[2].sampler,"parametric sampler explicitly reported, subclips not invented");
 m.clip(0,0,0,std::numeric_limits<float>::quiet_NaN(),true);Check(m.capture(t)==playback::Result::Invalid,"nonfinite phase rejected");
 m.clip(0,0,0,2.f,true);Check(m.capture(t)==playback::Result::Invalid,"out of range segment component rejected");
 m.clip(0,0,-1,.5f,true);Check(m.capture(t)==playback::Result::Invalid,"negative animation ID rejected");
 m.clip(0,0,5,.5f,true);m.race=true;Check(m.capture(t)==playback::Result::Changed,"queue replacement rejects mixed snapshot");
 m.race=false;m.layer(0,17);Check(m.capture(t)==playback::Result::Overflow,"queue over bounded capacity reports overflow");
 m.layer(0,2);m.deny=true;Check(m.capture(t)==playback::Result::Unreadable,"unreadable memory is not an empty successful queue");m.deny=false;
 m.put(Memory::base+0x5c,std::numeric_limits<float>::infinity());Check(m.capture(t)==playback::Result::Invalid,"invalid raw layer field rejects sample");
 Check(t.failureLayer==0&&t.failureField==1,"invalid active layer field reports exact failed boundary");
 m=Memory{};m.layer(0,0);m.put(Memory::base+0x5c,std::numeric_limits<float>::quiet_NaN());
 Check(m.capture(t)==playback::Result::Ok&&t.count==0,"empty allocated layer does not reject unrelated valid object animation");
 m.layer(4,1);m.clip(4,0,17,2.5f,true);
 Check(m.capture(t)==playback::Result::Invalid&&t.failureLayer==4&&t.failureEntry==0&&t.failureField==4,"active out-of-range phase remains rejected and precisely located");
 std::uint64_t modes=0;modes=lab::Change(modes,lab::Key::F3,1);
 Check(lab::Get(modes,lab::Key::F3)==1&&lab::Get(modes,lab::Key::F5)==0,"F3 native reference exclusive");
 modes=lab::Change(modes,lab::Key::F5,2);Check(lab::Get(modes,lab::Key::F3)==1&&lab::Get(modes,lab::Key::F5)==2,"F5 observation preserves F3 reference");
 modes=lab::Change(modes,lab::Key::F3,2);Check(lab::Get(modes,lab::Key::F3)==2&&lab::Get(modes,lab::Key::F5)==2,"two observation channels share production pose");
 const auto epoch=modes>>4;modes=lab::Change(modes,lab::Key::F5,1);Check((modes>>4)==epoch+1&&lab::Get(modes,lab::Key::F3)==2,"F5 reference preserves F3 live channel and increments epoch");
 lab::PoseLease lease;lease.Begin(0);Check(lease.Poll(false,true,0)==-1,"reference does not enable/reset posing");lease.RequestLive();
 Check(lease.Poll(true,false,0)==-1,"uncalibrated observation waits without extra T pose");
 Check(lease.Poll(true,true,0)==2,"calibrated live request defaults to recovery arms, not collision");
 Check(lease.Poll(true,true,0)==-1,"auto enable is one-shot, no repeated F10 overwrite");
 Check(lease.End()==0,"last lab OFF restores original OFF mode");
 lease.Begin(5);lease.RequestLive();Check(lease.Poll(true,true,5)==-1&&lease.End()==-1,"existing production mode preserved without reset");
 lease.Begin(0);lease.RequestLive();lease.Poll(true,true,0);lease.ExternalPoseChoice(3);
 Check(lease.End()==-1,"explicit F10 choice supersedes automatic lease");
 lease.Begin(0);lease.RequestLive();lease.ExternalPoseChoice(0);Check(lease.Poll(true,true,0)==-1,"explicit F10 OFF is not silently undone");lease.End();
 Check(!lab::PhysicalGrabImplemented&&lab::HolsterGestureImplemented&&!lab::NativeHitInjectionImplemented,"holster requests are implemented; physical grabs and damage injection remain unavailable");
 std::printf("playback_and_lab=%d/%d; native=NOT_TESTED\n",passed,total);return passed==total?0:1;
}
