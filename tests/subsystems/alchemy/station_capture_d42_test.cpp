#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
#include "../../../src/subsystems/alchemy/capture_d42.h"
#include "../../../src/subsystems/smithing/source_grip_h42.h"
#include <string>
#include <vector>
namespace d42=kcd2vr::subsystems::alchemy::capture_d42;
namespace pump=kcd2vr::subsystems::smithing::source_h42;
static unsigned checksD42=0;
static void expectD42(bool b,const char* text){++checksD42;if(!b){std::fprintf(stderr,"D42 FAIL: %s\n",text);std::exit(1);}}
int main(){
 {d42::Gate gate;d42::Key key{};expectD42(gate.observe(0,1000,key)==0,"off session has no receipt");
  expectD42(gate.observe(1,1000,key)==1,"first state captured");key.value[0]=1;
  expectD42(gate.observe(1,1001,key)==1,"one millisecond transition captured");key.value[0]=0;
  expectD42(gate.observe(1,1002,key)==1,"brief returning transition captured");
  expectD42(gate.observe(1,1003,key)==0,"duplicate throttled");
  expectD42(gate.observe(1,2002,key)==1,"unchanged bounded heartbeat");
  gate.limit=4;key.value[1]=1;expectD42(gate.observe(1,2003,key)==2,"explicit first exhausted event");
  key.value[1]=2;expectD42(gate.observe(1,2004,key)==0,"exhausted changes counted without formatting");
  char text[600]{};expectD42(gate.close(2005,"ALCHEMY","TEST",text,sizeof(text)),"summary flushed");
  expectD42(std::string(text).find("emitted=4 throttled=1 dropped=2 budget=4 exhausted=1")!=std::string::npos,"complete finite accounting");
  expectD42(!gate.close(2006,"ALCHEMY","TEST",text,sizeof(text)),"summary exactly once");}
 {Fixture fixture;fixture.admit();auto out=fixture.step();d42::Alchemy capture;std::vector<std::string> lines;
  const auto writer=[&](const char* line){lines.emplace_back(line);};
  kcd2vr::subsystems::alchemy::return67::Result ret{};
  capture.record(3,fixture.in,out,fixture.state,fixture.f,ret,writer);
  expectD42(lines.size()==1&&lines.back().find("request=")!=std::string::npos,"actual input adapter correlation emitted");
  fixture.in.tool[1].joined=true;fixture.in.tool[1].entity=114;fixture.in.tool[1].binding.object=44;
  capture.record(3,fixture.in,out,fixture.state,fixture.f,ret,writer);
  expectD42(lines.size()==2,"Attach witness change cannot hide in same timestamp");
  ret.serial=fixture.state.admittedTick;ret.status=kcd2vr::subsystems::alchemy::return67::Status::NativeReturnDispatched;ret.detachReturned=true;
  capture.record(3,fixture.in,out,fixture.state,fixture.f,ret,writer);
  expectD42(lines.size()==3&&lines.back().find("detachReturned=1")!=std::string::npos,"actual return witness captured independently from gesture");
  expectD42(lines.back().size()<1899&&lines.back().find("nativeCompletion=NOT_INFERRED")!=std::string::npos,"whole record fits journal and does not claim effects");
  capture.record(4,fixture.in,out,fixture.state,fixture.f,ret,writer);
  expectD42(lines.size()==5&&lines[3].find("D42_F5_END")==0,"new session closes old accounting before state");}
 {d42::Sources source;
  expectD42(source.due(1,1000,2,55,3,90,4),"source table first identity");
  expectD42(!source.due(1,1500,2,55,3,90,4),"same table omitted at legacy half second cadence");
  expectD42(source.due(1,1501,2,55,1,90,4),"geometry validity change immediate");
  expectD42(source.due(1,1502,2,55,1,90,5),"same address new station lifetime immediate");
  expectD42(source.due(1,6502,2,55,1,90,5),"periodic geometry summary retained");}
 {pump::Input in{};in.enabled=true;in.now=1000;in.instance=1;in.lifetime=2;in.modeEpoch=3;in.phase=8;
  in.frame.instance=1;in.frame.lifetime=2;in.frame.modeEpoch=3;in.frame.phase=8;in.frame.sequence=1;in.frame.tick=in.now;in.frame.player=7;
  auto& src=in.frame.source[2];src.instance=1;src.player=7;src.entity=9;src.holder=10;src.phase=8;src.slot=2;src.side=0;
  src.geometryKnown=true;src.geometryEntity=9;src.geometryType=pump::Geometry::ForgeHandle;src.slotWorld={0,0,0};
  in.palm[0].valid=true;in.palm[0].point={0,0,0};
  Frame frame{};frame.allowed=frame.headValid=true;frame.poseEpoch=4;frame.sourceEpoch=5;frame.sequence=1;frame.timeNs=1000000000;frame.tick=1000;
  frame.hand[0].valid=true;frame.hand[0].rotation[3]=1;
  pump::ForgePump traced,plain;pump::PumpTraceD42 trace{};
  const auto step=[&](float grip,float y,unsigned dt=16){in.now+=dt;in.frame.tick=frame.tick=in.now;++in.frame.sequence;++frame.sequence;frame.timeNs+=std::uint64_t(dt)*1000000;
   frame.hand[0].grip=grip;frame.hand[0].position[1]=y;in.palm[0].grip=grip;
   const bool a=traced.step(in,frame,0,false,&trace),b=plain.step(in,frame,0,false);expectD42(a==b,"tracing cannot change pump request");return a;};
  expectD42(!step(0,0)&&trace.reason==pump::PumpReasonD42::NewSource,"new handle baseline");
  expectD42(!step(.8f,0)&&trace.reason==pump::PumpReasonD42::Acquired,"actual handle squeeze acquired");
  expectD42(step(.8f,-.04f)&&trace.reason==pump::PumpReasonD42::Pumping,"real downstroke requests original RT");
  expectD42(!step(.8f,0)&&!trace.pressed,"return stops RT");
  expectD42(step(.8f,-.04f),"second deliberate downstroke");
  expectD42(!step(.8f,-.04f,90)&&trace.reason==pump::PumpReasonD42::Discontinuous,"tracking gap cancels unchanged");
  expectD42(!step(.8f,-.04f)&&trace.reason==pump::PumpReasonD42::NewGripRequired,"cancelled held squeeze does not revive");
  expectD42(!step(0,-.04f),"open hand rearms");
  expectD42(!step(.8f,-.04f)&&trace.reason==pump::PumpReasonD42::Acquired,"new deliberate squeeze reacquires");
 }
 std::printf("station_capture_d42: %u checks PASS\n",checksD42);return 0;
}
