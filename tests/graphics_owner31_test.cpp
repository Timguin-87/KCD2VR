// Actual production graphics service classes. Only native API leaves are mocked.
#include "../src/graphics_owner31.h"
#include <map>
#include <string>
#include <cassert>
#include <cstdio>
#include <limits>
namespace g=kcd2vr::graphics_owner31;
namespace e=kcd2vr::exposure26;
struct IO {
    std::map<std::string,float> value;
    bool retail=true,failWrites=false,writeBeforeFailure=false;
    unsigned reads=0,writes=0,ready=0,failRead=0,failWrite=0;
    std::string unchangedName,unreadableAfterWrite,clampName;float clampMax=0;
    bool pendingUnread=false,alwaysUnreadAfterWrite=false;
    IO(){for(unsigned i=0;i<5;++i)value[g::Shadows::names[i]]=float(i+10);value["r_HDREyeAdaptationSpeed"]=3;}
    bool Ready(){++ready;return retail;}
    bool Read(const char* name,bool,float& out){++reads;if(alwaysUnreadAfterWrite&&writes)return false;if(pendingUnread&&unreadableAfterWrite==name){pendingUnread=false;return false;}if(reads==failRead)return false;auto it=value.find(name);if(it==value.end())return false;out=it->second;return true;}
    bool Write(const char* name,bool,float wanted){++writes;pendingUnread=unreadableAfterWrite==name;if(failWrites||writes==failWrite){if(writeBeforeFailure)value[name]=wanted;return false;}if(unchangedName!=name)value[name]=clampName==name?std::min(wanted,clampMax):wanted;return true;}
    void ClearCounts(){reads=writes=ready=0;failRead=failWrite=0;}
};
static e::Q yaw(float radians){return {0,std::sin(radians*.5f),0,std::cos(radians*.5f)};}
int main(){unsigned checks=0;const auto ck=[&](bool ok){++checks;assert(ok);};
 const char* eyeName="r_HDREyeAdaptationSpeed";
 {g::Eye eye;IO io;for(unsigned n=0;n<100;++n)eye.Service(false,false,false,{},100+n,io);
  ck(io.reads==0&&io.writes==0&&io.ready==0);}
 {g::Eye eye;IO io;eye.Service(true,true,true,yaw(0),100,io);eye.Service(true,true,true,yaw(.2f),110,io);
  ck(eye.lease.owned&&g::equal(io.value[eyeName],.01f));io.ClearCounts();
  for(unsigned n=0;n<15;++n)eye.Service(true,true,true,yaw(.2f+n*.02f),120+n*10,io);
  ck(io.reads==0&&io.writes==0&&io.ready==0);
  eye.Service(true,false,true,yaw(.5f),300,io);ck(g::equal(io.value[eyeName],3)&&!eye.lease.owned);
  io.ClearCounts();eye.Service(false,false,false,{},310,io);ck(io.reads==0&&io.writes==0&&io.ready==0);}
 {g::Eye eye;IO io;eye.Service(true,true,true,yaw(0),100,io);eye.Service(true,true,true,yaw(.2f),110,io);
  eye.Service(true,true,false,{},120,io);ck(g::equal(io.value[eyeName],3)&&!eye.lease.owned&&eye.state.level==e::Level::Native);}
 {g::Eye eye;IO io;eye.Service(true,true,true,yaw(0),100,io);eye.Service(true,true,true,yaw(.2f),110,io);
  eye.Service(true,true,true,{0,0,0,0},120,io);ck(g::equal(io.value[eyeName],3)&&!eye.lease.owned);}
 {g::Eye eye;IO io;eye.Service(true,true,true,yaw(0),100,io);eye.Service(true,true,true,yaw(.2f),110,io);
  io.value[eyeName]=8;io.ClearCounts();eye.Service(false,false,false,{},120,io);
  ck(io.value[eyeName]==8&&io.writes==0&&!eye.lease.owned);}
 {g::Eye eye;IO io;io.retail=false;eye.Service(true,true,true,yaw(0),100,io);eye.Service(true,true,true,yaw(.2f),110,io);
  ck(io.reads==0&&io.writes==0&&io.ready==1);io.ClearCounts();
  for(unsigned n=0;n<20;++n)eye.Service(true,true,true,yaw(.3f+n*.02f),120+n*10,io);
  ck(io.ready==0&&io.reads==0&&io.writes==0);}
 {g::Eye eye;IO io;io.failWrites=true;eye.Service(true,true,true,yaw(0),100,io);eye.Service(true,true,true,yaw(.2f),110,io);
  ck(io.writes==1);io.ClearCounts();
  for(unsigned n=0;n<20;++n)eye.Service(true,true,true,yaw(.3f+n*.02f),120+n*10,io);
  ck(io.ready==0&&io.reads==0&&io.writes==0);}
 {g::Eye eye;IO io;eye.Service(true,true,true,yaw(0),100,io);eye.Service(true,true,true,yaw(.2f),110,io);
  io.failWrites=true;io.ClearCounts();eye.Service(false,false,false,{},120,io);ck(io.writes==1&&eye.lease.owned);
  io.ClearCounts();for(unsigned n=0;n<30;++n)eye.Service(false,false,false,{},130+n,io);
  ck(io.ready==0&&io.writes==0&&io.reads==0);io.failWrites=false;eye.Service(false,false,false,{},620,io);
  ck(g::equal(io.value[eyeName],3)&&!eye.lease.owned);}
 {g::Shadows shadows;IO io;for(unsigned n=0;n<100;++n)shadows.Service(false,100+n,io);
  ck(io.reads==0&&io.writes==0&&io.ready==0);shadows.Service(true,200,io);ck(shadows.applied&&shadows.Owned());
  io.ClearCounts();for(unsigned n=0;n<100;++n)shadows.Service(true,210+n,io);
  ck(io.reads==0&&io.writes==0&&io.ready==0);shadows.Service(false,400,io);
  for(unsigned i=0;i<5;++i)ck(io.value[g::Shadows::names[i]]==float(i+10));ck(!shadows.Owned());}
 for(unsigned failed=1;failed<=5;++failed){g::Shadows shadows;IO io;io.failWrite=failed;
  shadows.Service(true,100,io);ck(!shadows.applied&&!shadows.Owned());
  for(unsigned i=0;i<5;++i)ck(io.value[g::Shadows::names[i]]==float(i+10));
  io.ClearCounts();shadows.Service(false,110,io);ck(io.reads==0&&io.writes==0);}
 {g::Shadows shadows;IO io;io.failWrite=3;io.writeBeforeFailure=true;shadows.Service(true,100,io);
  ck(!shadows.applied&&!shadows.Owned());for(unsigned i=0;i<5;++i)ck(io.value[g::Shadows::names[i]]==float(i+10));}
 {g::Shadows shadows;IO io;shadows.Service(true,100,io);io.value[g::Shadows::names[2]]=9;
  io.ClearCounts();shadows.Service(false,110,io);ck(io.value[g::Shadows::names[2]]==9&&!shadows.Owned());
  ck(io.writes==4);}
 {g::Shadows shadows;IO io;io.retail=false;shadows.Service(true,100,io);ck(io.ready==1&&io.reads==0&&io.writes==0);
  io.ClearCounts();for(unsigned n=0;n<100;++n)shadows.Service(true,110+n,io);
  ck(io.ready==0&&io.reads==0&&io.writes==0);}
 {g::Lease lease;IO io;io.failRead=2;ck(lease.Set(io,eyeName,true,.01f)==g::Result::Retry);
  ck(lease.owned&&lease.uncertain);ck(lease.Restore(io,eyeName,true));ck(io.value[eyeName]==3&&!lease.owned);}
 {g::Lease lease;IO io;ck(lease.Set(io,eyeName,true,.01f)==g::Result::Done);
  io.ClearCounts();io.failWrite=1;io.failRead=2;
  ck(lease.Set(io,eyeName,true,.25f,true)==g::Result::Retry);ck(lease.owned&&lease.uncertain);
  ck(lease.Restore(io,eyeName,true));ck(io.value[eyeName]==3&&!lease.owned);}
 {e::State state;ck(e::Step(state,true,yaw(0),100)==e::Level::Native);
  ck(e::Step(state,true,yaw(.2f),110)==e::Level::Paused);
  ck(e::Step(state,true,yaw(.8f),110)==e::Level::Paused);
  ck(e::Step(state,true,yaw(.8f),1000)==e::Level::Native);
  ck(e::Step(state,false,{},1010)==e::Level::Native&&!state.have);}
 {g::Eye eye;IO io;
  eye.Service(true,true,true,yaw(0),1000,io,990);eye.Service(true,true,true,yaw(.2f),1050,io,1000);
  ck(eye.state.lastTick==1000&&eye.lease.owned); // source10ms turn, not consumed50ms
  io.ClearCounts();eye.Service(true,true,true,yaw(.2f),1060,io,1000);
  ck(eye.state.lastTick==1000&&io.reads==0&&io.writes==0); // duplicate source not refreshed
  eye.Service(true,true,true,yaw(.2f),1101,io,1000);ck(!eye.lease.owned&&g::equal(io.value[eyeName],3));
  eye.Service(true,true,true,yaw(.2f),1120,io,1200);ck(!eye.state.have&&!eye.lease.owned); // future source
  eye.Service(true,true,false,{},1300,io,0);ck(!eye.state.have&&!eye.lease.owned);}
 // Confirmed C31 defects: repeated unchanged/unreadable results and engine
 // clamp readback. Exercise the actual services, not a replacement retry loop.
 for(bool unreadable:{false,true}){g::Shadows shadows;IO io;
  if(unreadable)io.unreadableAfterWrite=g::Shadows::names[2];else io.unchangedName=g::Shadows::names[2];
  for(std::uint64_t now=100;now<10100;now+=100)shadows.Service(true,now,io);
  ck(shadows.gaveUp&&!shadows.pending&&!shadows.applied&&shadows.misses==3);
  const auto writes=io.writes,reads=io.reads,ready=io.ready;
  for(std::uint64_t now=10100;now<20100;now+=100)shadows.Service(true,now,io);
  ck(io.writes==writes&&io.reads==reads&&io.ready==ready); // no hidden 500ms retry forever
  shadows.Service(false,20200,io);io.unchangedName.clear();io.unreadableAfterWrite.clear();
  shadows.Service(true,20300,io);ck(shadows.applied&&!shadows.gaveUp);
  shadows.Service(false,20400,io);ck(!shadows.Owned());}
 {g::Shadows shadows;IO io;io.value[g::Shadows::names[3]]=1024;
  io.clampName=g::Shadows::names[3];io.clampMax=2048;shadows.Service(true,100,io);
  ck(shadows.applied&&shadows.lease[3].owned&&shadows.lease[3].clamped&&io.value[g::Shadows::names[3]]==2048);
  shadows.Service(false,110,io);ck(io.value[g::Shadows::names[3]]==1024&&!shadows.Owned());}
 {g::Eye eye;
  // Simulate a native minimum rather than maximum via the Read/Write leaf below.
  struct MinimumIO:IO {bool Write(const char* n,bool f,float v){return IO::Write(n,f,std::max(v,.1f));}} min;
  eye.Service(true,true,true,yaw(0),100,min);eye.Service(true,true,true,yaw(.2f),110,min);
  ck(eye.lease.clamped&&eye.lease.owned&&eye.handled==e::Level::Paused&&g::equal(min.value[eyeName],.1f));
  eye.Service(false,false,false,{},120,min);ck(!eye.lease.owned&&g::equal(min.value[eyeName],3));}
 for(bool unreadable:{false,true}){g::Eye eye;IO io;
  if(unreadable)io.alwaysUnreadAfterWrite=true;else io.unchangedName=eyeName;
  eye.Service(true,true,true,yaw(0),100,io);
  for(std::uint64_t now=110;now<10110;now+=10)eye.Service(true,true,true,yaw(float(now)*.01f),now,io,now);
  ck(eye.gaveUp&&!eye.pending&&eye.misses==3);
  const auto writes=io.writes,reads=io.reads,ready=io.ready;
  for(std::uint64_t now=10110;now<11110;now+=10)eye.Service(true,true,true,yaw(float(now)*.01f),now,io,now);
  ck(io.writes==writes&&io.reads==reads&&io.ready==ready);
  eye.Service(false,false,false,{},12000,io);io.unchangedName.clear();io.alwaysUnreadAfterWrite=false;
  eye.Service(true,true,true,yaw(0),12100,io);eye.Service(true,true,true,yaw(.2f),12110,io);
  ck(!eye.gaveUp&&eye.lease.owned&&eye.handled==e::Level::Paused);
  eye.Service(false,false,false,{},12200,io);ck(!eye.lease.owned);}
 {g::Eye eye;IO io;eye.Service(true,true,true,yaw(0),100,io);eye.Service(true,true,true,yaw(.2f),110,io);
  io.failWrites=true;for(auto now:{120u,620u,1120u,1620u})eye.Service(false,false,false,{},now,io);
  ck(eye.gaveUp&&!eye.pending&&eye.lease.owned&&eye.restoreMisses==3);
  const auto writes=io.writes;eye.Service(false,false,false,{},5000,io);ck(io.writes==writes&&eye.lease.owned);
  io.failWrites=false;eye.RearmRestore();eye.Service(false,false,false,{},5010,io);
  ck(!eye.lease.owned&&g::equal(io.value[eyeName],3));}
 {g::Shadows shadows;IO io;shadows.Service(true,100,io);io.failWrites=true;
  for(auto now:{110u,610u,1110u,1610u})shadows.Service(false,now,io);
  ck(shadows.gaveUp&&!shadows.pending&&shadows.Owned());const auto writes=io.writes;
  shadows.Service(false,5000,io);ck(io.writes==writes&&shadows.Owned());io.failWrites=false;
  shadows.RearmRestore();shadows.Service(false,5010,io);ck(!shadows.Owned());}
 {g::Lease lease;IO io;io.unreadableAfterWrite=eyeName;
  ck(lease.Set(io,eyeName,true,.01f)==g::Result::Retry&&lease.uncertain);
  ck(!lease.Restore(io,eyeName,true)&&lease.owned); // original was written but readback unavailable
  ck(lease.Restore(io,eyeName,true)&&!lease.owned&&g::equal(io.value[eyeName],3));}
 {g::Lease lease;IO io;io.clampName=eyeName;io.clampMax=.1f;
  ck(lease.Set(io,eyeName,true,1)==g::Result::Done&&lease.clamped);
  io.value[eyeName]=8;const auto writes=io.writes;
  ck(lease.Restore(io,eyeName,true)&&io.value[eyeName]==8&&io.writes==writes&&!lease.owned);}
 {g::Lease lease;
  struct FaultIO:IO {bool Write(const char* n,bool f,float){(void)IO::Write(n,f,.1f);return false;}} io;
  ck(lease.Set(io,eyeName,true,1)==g::Result::Foreign&&!lease.owned&&lease.blocked);
  const auto writes=io.writes;ck(lease.Restore(io,eyeName,true)&&io.writes==writes&&g::equal(io.value[eyeName],.1f));}
 {g::Shadows shadows;IO io;io.retail=false;
  for(std::uint64_t t=100;t<10100;t+=100)shadows.Service(true,t,io);
  ck(shadows.gaveUp&&io.ready==3&&io.reads==0&&io.writes==0);}
 {g::Eye eye;IO io;io.retail=false;eye.Service(true,true,true,yaw(0),100,io);
  for(std::uint64_t t=110;t<10110;t+=10)eye.Service(true,true,true,yaw(float(t)*.01f),t,io,t);
  ck(eye.gaveUp&&io.ready==3&&io.reads==0&&io.writes==0);}
 // A proven engine clamp is an owned result, not a new failure on every
 // exposure-level transition. Unknown initial no-op remains unowned/refused.
 {g::Lease lease;IO io;io.clampName=eyeName;io.clampMax=.1f;
  ck(lease.Set(io,eyeName,true,1)==g::Result::Done&&lease.clamped&&lease.completedWrite);
  const auto writes=io.writes;ck(lease.Set(io,eyeName,true,1)==g::Result::Done);
  ck(io.writes==writes&&!lease.completedWrite&&g::equal(lease.requested,1));
  ck(lease.Set(io,eyeName,true,2)==g::Result::Done&&lease.completedWrite&&g::equal(lease.requested,2));
  ck(io.writes==writes+1&&g::equal(io.value[eyeName],.1f));
  ck(lease.Set(io,eyeName,true,2)==g::Result::Done&&!lease.completedWrite&&io.writes==writes+1);
  io.clampName.clear();ck(lease.Restore(io,eyeName,true)&&lease.completedWrite&&!lease.owned&&g::equal(io.value[eyeName],3));}
 {g::Lease lease;IO io;io.unchangedName=eyeName;
  ck(lease.Set(io,eyeName,true,1)==g::Result::Retry&&!lease.owned&&!lease.clamped&&!lease.completedWrite);}
 {g::Lease lease;IO io;io.clampName=eyeName;io.clampMax=.1f;
  ck(lease.Set(io,eyeName,true,1)==g::Result::Done);io.failWrites=true;
  ck(lease.Set(io,eyeName,true,2)==g::Result::Retry&&!lease.completedWrite&&g::equal(lease.requested,1));
  io.failWrites=false;io.value[eyeName]=9;const auto writes=io.writes;
  ck(lease.Set(io,eyeName,true,1)==g::Result::Foreign&&io.writes==writes&&!lease.owned);}
 {g::Eye eye;IO io;
  auto drive=[&](e::Level level,std::uint64_t now){eye.state.have=true;eye.state.lastTick=now;eye.state.level=level;
    eye.Service(true,true,true,yaw(0),now,io,now);};
  io.failWrites=true;drive(e::Level::Paused,100);ck(eye.misses==1&&!eye.gaveUp);
  io.failWrites=false;drive(e::Level::Slow,700);ck(eye.misses==0&&eye.lease.completedWrite&&eye.handled==e::Level::Slow);
  io.failWrites=true;drive(e::Level::Paused,800);ck(eye.misses==1&&!eye.gaveUp);
  io.failWrites=false;drive(e::Level::Slow,900);ck(eye.misses==1&&!eye.lease.completedWrite); // unchanged is not real IO recovery
  drive(e::Level::Paused,1000);ck(eye.misses==0&&eye.lease.completedWrite);
  io.failWrites=true;drive(e::Level::Slow,1100);ck(eye.misses==1);
  io.failWrites=false;drive(e::Level::Native,1200);ck(eye.misses==0&&eye.restoreMisses==0&&!eye.lease.owned);}
 {g::Eye eye;IO io;io.clampName=eyeName;io.clampMax=.001f;
  auto drive=[&](e::Level level,std::uint64_t now){eye.state.have=true;eye.state.lastTick=now;eye.state.level=level;
    eye.Service(true,true,true,yaw(0),now,io,now);};
  drive(e::Level::Paused,100);ck(eye.lease.clamped&&eye.misses==0);
  for(unsigned n=0;n<12;++n)drive(n%2?e::Level::Paused:e::Level::Slow,200+n*100);
  ck(!eye.gaveUp&&eye.misses==0&&eye.lease.owned);
  io.clampName.clear();drive(e::Level::Native,1500);ck(!eye.lease.owned&&g::equal(io.value[eyeName],3));}
 // C49 SHADOW QUALITY: LOW = native (only the VR2 GSM layout, owned elsewhere), MEDIUM = C26 fix + 2048, HIGH = C26 fix + 4096
 ck(g::sunResForLevel(1)==2048.f&&g::sunResForLevel(2)==4096.f&&g::sunResForLevel(9)==4096.f&&g::clampShadowLevel(-3)==0);
 ck(g::migrateShadowLevel(-1,false)==0&&g::migrateShadowLevel(-1,true)==2&&g::migrateShadowLevel(1,true)==1&&g::migrateShadowLevel(0,true)==0&&g::migrateShadowLevel(7,false)==2);
 {g::Shadows sh;IO io;for(unsigned n=0;n<50;++n)sh.ServiceLevel(0,100+n,io);ck(io.reads==0&&io.writes==0&&io.ready==0&&!sh.Owned());
  sh.ServiceLevel(1,200,io);ck(sh.applied&&sh.Owned()&&io.value[g::Shadows::names[3]]==2048.f);
  ck(io.value[g::Shadows::names[0]]==1&&io.value[g::Shadows::names[1]]==1&&g::equal(io.value[g::Shadows::names[2]],.3f)&&io.value[g::Shadows::names[4]]==1);
  io.ClearCounts();for(unsigned n=0;n<50;++n)sh.ServiceLevel(1,210+n,io);ck(io.reads==0&&io.writes==0&&io.ready==0); // steady: edge-only
  sh.ServiceLevel(2,300,io);ck(io.writes==1&&io.value[g::Shadows::names[3]]==4096.f&&sh.applied); // MEDIUM->HIGH rewrites only the sun map
  io.ClearCounts();sh.ServiceLevel(1,310,io);ck(io.writes==1&&io.value[g::Shadows::names[3]]==2048.f);
  sh.ServiceLevel(0,320,io);for(unsigned i=0;i<5;++i)ck(io.value[g::Shadows::names[i]]==float(i+10));ck(!sh.Owned());
  io.ClearCounts();sh.ServiceLevel(2,330,io);ck(io.value[g::Shadows::names[3]]==4096.f&&sh.applied);
  sh.ServiceLevel(0,340,io);ck(io.value[g::Shadows::names[3]]==13.f&&!sh.Owned());}
 {g::Shadows sh;IO io;sh.ServiceLevel(1,100,io);io.value[g::Shadows::names[3]]=1024;sh.ServiceLevel(2,110,io); // foreign write is kept
  ck(io.value[g::Shadows::names[3]]==1024.f);sh.ServiceLevel(0,120,io);ck(io.value[g::Shadows::names[3]]==1024.f);}
 {g::Shadows sh;IO io;io.clampName=g::Shadows::names[3];io.clampMax=3000;sh.ServiceLevel(2,100,io); // a clamping setter: owned at its value
  ck(sh.applied&&io.value[g::Shadows::names[3]]==3000.f&&sh.lease[3].clamped);sh.ServiceLevel(1,110,io);ck(io.value[g::Shadows::names[3]]==2048.f);
  sh.ServiceLevel(0,120,io);ck(io.value[g::Shadows::names[3]]==13.f);}
 std::printf("graphics_owner31: %u production-service ownership checks PASS\n",checks);
}
