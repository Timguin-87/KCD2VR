// Actual final pad adapter. Only trace output is replaced; no game is loaded.
#include "vrik/semi_workstations46.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
static std::vector<std::string> lines;
namespace kcd2vr::diagnostic_trace {void Write(const char* s) noexcept {lines.emplace_back(s);}}
namespace k=kcd2vr;
static unsigned checks=0;
static void ck(bool b,const char* why){++checks;if(!b){std::fprintf(stderr,"FAIL %u: %s\n",checks,why);std::exit(1);}}
struct Pad {bool a=false,b=false,x=false,lb=false,rb=false;float lx=0,ly=0,rt=0,lt=0;};
int main(){
 k::minigame_hands45::mode=k::minigame_hands45::SemiPassenger;
 k::minigame_hands45::smithingMode67=-1;k::minigame_hands45::alchemyMode67=-1;
 k::minigame_hands45::modeEpoch67=9;
 k::semi_smith46::clockReady=false;k::semi_smith46::clockControl.publish({});
 k::diagnostics::capture168::gate.select(k::diagnostics::capture168::Domain::Interaction,true,1000);
 k::semi_workstations46::State state;k::semi_workstations46::Context context{};
 context.enabled=context.qualified=context.smithTracked=context.smithGestures=true;
 context.smith=10;context.lifetime=7;context.smithPhase=11;
 k::workstation_sample46::Frame f{};f.sequence=1;f.tick=1000;f.timeNs=1000000000;
 f.poseEpoch=4;f.sourceEpoch=8;f.allowed=f.headValid=true;f.head[1]=1.7f;
 for(auto& h:f.hand){h.valid=true;h.position[1]=1.2f;h.position[2]=-.3f;}
 Pad pad{};k::semi_smith46::ClockControl command{};
 const auto step=[&](){
  // Supply the same copied, current tool witness that the production native
  // callback publishes; the adapter must still enforce its real tool gate.
  k::subsystems::smithing::bridge67::NativeTool tool{};tool.joined=true;tool.tick=f.tick;tool.poseEpoch=4;tool.modeEpoch=9;
  auto& b=tool.binding;b.instance=10;b.lifetime=7;b.player=20;b.character=30;b.attachment=40;b.model=50;b.holder=60;b.object=70;
  b.key=0x0200000000000005ull;b.entity=42;b.sourceEntity=42;b.side=0;b.tool=0;b.phase=context.smithPhase;
  k::subsystems::smithing::bridge67::tools[0].publish(tool);
  pad={};state.apply(context,f,f.tick,pad);ck(k::semi_smith46::clockControl.read(command),"existing snapshot published");
 };
 const auto next=[&](){++f.sequence;f.tick+=10;f.timeNs+=10000000;};
 step();next();step();next();step();next();f.hand[0].position[1]+=.04f;step();
 ck(pad.rt==1.f&&!command.enabled&&command.diagnosticD57.id==1,"actual native-height raise publishes diagnostic ID without enabling legacy clock");
 ck(command.diagnosticD57.nativeHeight&&command.diagnosticD57.mode==1,"producer explicitly retains actual saved mode");
 next();context.smithPhase=14;f.hand[0].position[1]-=.02f;step();
 ck(pad.rt==0.f&&command.diagnosticD57.releaseTick==f.tick,"actual downward reversal preserves existing release and stamps its sample");
 bool logged=false;for(const auto& line:lines)if(line.find("F5_CRAFT_OUTPUT")!=std::string::npos&&line.find("strokeIdD57=1")!=std::string::npos&&line.find("strokeRelease=1040")!=std::string::npos)logged=true;
 ck(logged,"existing output receipt includes raise/release identity");
 next();context.smithPhase=16;f.hand[0].position[1]=1.18f;step();ck(pad.rt==0.f,"native fall keeps RT released");
 next();context.smithPhase=17;f.hand[0].position[1]=1.19f;step();
 next();context.smithPhase=11;f.hand[0].position[1]=1.20f;step();ck(pad.rt==0.f,"native idle requires a fresh lift");
 next();f.hand[0].position[1]=1.24f;step();
 ck(pad.rt==1.f&&!command.enabled&&command.diagnosticD57.id==2,"D59R1 continuous second lift reaches actual final pad without a legacy clock lease");
 k::diagnostics::capture168::gate.select(k::diagnostics::capture168::Domain::Interaction,false,f.tick);
 next();step();ck(!command.diagnosticD57.id,"recorder off empties the same snapshot");
 k::diagnostics::capture168::gate.select(k::diagnostics::capture168::Domain::Interaction,true,f.tick);
 next();context.smithPhase=16;step();ck(!command.diagnosticD57.id,"enabling mid-native-fall cannot invent earlier gesture");
 context={};next();step();ck(!command.diagnosticD57.id&&!command.enabled,"station exit revokes metadata and native clock");
 std::printf("smith_stroke_wiring_d57_test: %u checks PASS\n",checks);
}
