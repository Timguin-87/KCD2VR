// Actual final-pad adapter. Native tool membership is modeled; the native
// workpiece transform and stroke evaluator are deliberately not replaced here.
#include "vrik/semi_workstations46.h"
#include <cstdio>
#include <cstdlib>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace sw=kcd2vr::semi_workstations46;
namespace sb=kcd2vr::subsystems::smithing::bridge67;
namespace mh=kcd2vr::minigame_hands45;
static unsigned checks=0,failures=0;
static void ck(bool x,const char* reason){++checks;if(!x){++failures;std::fprintf(stderr,"FAIL %s\n",reason);}}
struct Pad {bool a=false,b=false,x=false;float lx=0,ly=0,rt=0,lt=0;};
struct Scene {
 sw::State state;sw::Context c{};kcd2vr::workstation_sample46::Frame f{};
 Scene(){c.enabled=c.qualified=c.smithTracked=c.smithGestures=c.gestureSmith67=true;c.smith=100;c.lifetime=7;c.smithPhase=11;
  f.allowed=f.headValid=true;f.tick=1000;f.sequence=1;f.timeNs=1000000000;f.poseEpoch=8;f.sourceEpoch=9;f.head[1]=1.7f;
  for(auto& h:f.hand){h.valid=true;h.position[1]=1.1f;}kcd2vr::semi_smith46::clockReady=true;
  sb::tools[0].publish({});sb::tools[1].publish({});publish();}
 sb::NativeTool tool(int side){sb::NativeTool t{};t.joined=true;t.tick=f.tick;t.poseEpoch=f.poseEpoch;t.modeEpoch=mh::modeEpoch67.load();
  auto& b=t.binding;b.instance=c.smith;b.lifetime=c.lifetime;b.player=1;b.character=2;b.attachment=3;b.model=4;b.holder=5;
  b.object=6;b.entity=7;b.sourceEntity=7;b.side=side;b.phase=c.smithPhase;b.tool=side?2:0;b.key=side?0:9;return t;}
 void publish(){sb::tools[0].publish(tool(0));sb::tools[1].publish(tool(1));}
 void next(){f.tick+=10;f.timeNs+=10000000;++f.sequence;publish();}
 Pad step(Pad p={}){state.apply(c,f,f.tick,p);return p;}
};
int main(){
 ck(!kcd2vr::subsystems::alchemy::reach_craft1::enabled.load(),"optional crafting policy defaults OFF");
 // Explicit saved FREE mode; keep the legacy trial OFF.
 mh::setMode(mh::Smithing,mh::Gesture);
 // Accepted R14/D38 native anvil polarity: head-forward -Z maps to negative
 // native pad Y. This fixture previously asserted the retired opposite sign.
 kcd2vr::subsystems::alchemy::reach_craft1::enabled=false;
 Scene q;q.step();q.next();q.step();
 q.next();q.f.hand[1].position[0]+=.003f;auto p=q.step();
 ck(q.state.lastSmith.axesOwned&&p.lx==0&&p.ly==0,"Free Smithing lateral hand jitter must not slide iron sideways");
 q.next();q.f.hand[1].position[0]-=.003f;p=q.step();ck(p.lx==0&&p.ly==0,"opposite lateral motion remains fixture-neutral");
 q.next();q.f.hand[1].position[2]-=.003f;p=q.step();ck(p.ly<0&&p.ly>=-1&&p.lx==0,"forward hand travel feeds accepted negative native fore/aft axis");
 q.next();p=q.step();ck(p.lx==0&&p.ly==0,"stationary held offset stops travel");
 q.next();q.f.hand[1].position[2]+=.003f;p=q.step();ck(p.ly>0&&p.ly<=1&&p.lx==0,"backward hand travel reverses permitted axis");
 q.next();q.f.hand[1].position[0]+=.003f;p=q.step({false,false,false,.7f,-.4f,0});
 ck(p.lx==.7f&&p.ly==-.4f,"real stick owns the complete native vector without summing hand motion");
 q.next();q.f.hand[1].position[2]-=.003f;p=q.step();ck(p.lx==0&&p.ly==0,"manual neutral rebase has no deferred jump");
 q.next();q.f.hand[1].position[2]-=.003f;p=q.step();ck(p.lx==0&&p.ly<0,"fresh movement resumes after stick rebase");
 q.next();sb::tools[1].publish({});q.f.hand[1].position[2]-=.003f;p=q.step();ck(!q.state.lastSmith.axesOwned&&p.lx==0&&p.ly==0,"missing left native tool cannot authorize fixture command");
 for(int side=0;side<2;++side)for(int tool=0;tool<3;++tool){auto b=q.tool(side).binding;b.tool=tool;
  ck(sb::fixtureOwnedH21(b)==(side==1&&(tool==1||tool==2)),"native tongs/workpiece fixture ownership preserved independently of input");}
 q.next();q.c.enabled=false;p=q.step({false,false,false,.5f,.3f,0});ck(p.lx==.5f&&p.ly==.3f,"inactive mode preserves native stick input");
 std::printf("CRAFT1 smithing: %u checks, %u failures; native physics/scoring not executed\n",checks,failures);return failures?1:0;
}
