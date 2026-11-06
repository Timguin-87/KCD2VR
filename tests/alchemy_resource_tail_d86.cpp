#include "subsystems/alchemy/input67.h"
#include <cstdio>
#include <cstdlib>
namespace a=kcd2vr::subsystems::alchemy::input67;
namespace n=kcd2vr::subsystems::alchemy::near67;
namespace g=kcd2vr::subsystems::alchemy::gesture67;
namespace s=kcd2vr::alchemy_semi46;
using Frame=kcd2vr::workstation_sample46::Frame;
static unsigned checks=0;
static void checkAt(bool b,int line){++checks;if(!b){std::fprintf(stderr,"FAIL_RESOURCE_TAIL check%u line%d\n",checks,line);std::exit(1);}}
#define check(v) checkAt((v),__LINE__)
struct Fixture {
 a::State state{};a::Input in{};Frame f{};
 Fixture(){in.enabled=in.qualified=true;in.now=1000;in.modeEpoch=6;in.context={1,2,3,4,5,6,7};
  auto& t=in.nativeFrame.table;t.instance=1;t.player=2;t.playerEntity=9;t.lifetime=3;t.tick=1000;t.sequence=20;
  for(int k=0;k<21;++k)t.refs[k]={k,unsigned(100+k)};
  in.nativeFrame.offers[14]={{14,114},{0,0,0},1000,true};in.nativeFrame.offers[6]={{6,106},{.5,0,0},1000,true};
  for(int h=0;h<2;++h){in.palm[h]={{h?0.:2.,0,0},1000,4,5,6,h,0,true};
   in.world[h]={true,{float(h?0.:2.),0,0},1000,4,5};f.hand[h].valid=true;}
  f.sequence=10;f.tick=1000;f.timeNs=1000000000;f.poseEpoch=4;f.sourceEpoch=5;f.allowed=f.headValid=true;
  in.action={1,0,3,1000,true};}
 void advance(unsigned ms=10){in.now+=ms;f.tick=in.now;f.timeNs+=std::uint64_t(ms)*1000000;++f.sequence;
  auto& t=in.nativeFrame.table;t.tick=in.now;++t.sequence;
  for(auto& o:in.nativeFrame.offers)if(o.valid)o.tick=in.now;
  for(int h=0;h<2;++h){in.palm[h].tick=in.world[h].tick=in.now;if(in.tool[h].joined)in.tool[h].tick=in.now;}
  in.action.tick=in.now;if(in.work.parent)in.work.tick=in.now;if(in.nativeSelectedKnown)in.nativeSelected.tick=in.now;}
 void grip(int side,float value){in.palm[side].grip=f.hand[side].grip=value;}
 void selected(int type=14,bool hold=false){in.nativeSelectedKnown=true;in.nativeSelected={1,3,in.now,unsigned(100+type),unsigned(type),hold};}
 a::Output step(){return state.step(in,f);}
 void admit(int type=14,int side=1,bool hold=false){
  in.nativeFrame.offers[14].valid=false;in.nativeFrame.offers[type]={{type,unsigned(100+type)},{0,0,0},in.now,true};
  auto& source=in.nativeFrame.offers[type];source.boundsValid=true;source.bounds={{-.04f,-.04f,-.04f,.04f,.04f,.04f},{1,0,0,0,0,1,0,0,0,0,1,0}};
  auto& pot=in.nativeFrame.offers[6];pot.boundsValid=true;pot.bounds={{-.04f,-.04f,-.04f,.04f,.04f,.04f},{1,0,0,.5f,0,1,0,0,0,0,1,0}};
  in.palm[side].point={0,0,0};in.palm[1-side].point={2,0,0};grip(side,1);step();advance();selected(type,hold);
  check(step().admitted);}
 void tool(int side,int type){in.tool[side]={1,0,888,3,in.now,4,unsigned(100+type),side,type,true};}
 void resource(int phase=1,bool working=true){in.action.parent=999;in.work={1,999,555,3,in.now,44,s::Resource,phase,2,working,false};tool(1,14);}
};
static void nativeResourceR17(Fixture& q,int type,int phase=0,bool attached=true){
 q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Resource,phase,2,phase==1,false};
 if(attached){q.tool(1,type);q.in.tool[1].parent=999;
  auto& b=q.in.tool[1].binding;b={1,2,888,777,unsigned(100+type),1,type,999,0x3A5C670,666,3};}
 else q.in.tool[1]={};
}

static void tilt(Fixture& q,float angle){q.f.hand[1].rotation[0]=std::sin(angle*.5f);q.f.hand[1].rotation[3]=std::cos(angle*.5f);}
int main(){
 Fixture q;q.admit();q.advance();nativeResourceR17(q,14,1);q.step();
 for(int n=0;n<10;++n){q.advance();q.step();}
 q.in.palm[1].point={.5,0,0};q.in.world[1].point[0]=.5f;tilt(q,.9f);q.advance();auto o=q.step();
 check(!o.semiControl.clockStop&&o.semiControl.totalSeconds>0&&!o.use);
 const auto before=o.semiControl.totalSeconds;
 q.in.work.phase=2;q.in.work.working=true;q.advance();o=q.step();
 check(o.gesture.completedHeld&&!o.semiControl.clockStop&&o.semiControl.totalSeconds>before&&!o.use);
 check(!o.semiControl.resourceReturnHomeD86&&o.control.held[1]);
 const auto poured=o.semiControl.totalSeconds;o=q.step();check(o.semiControl.totalSeconds==poured);
 tilt(q,0);q.advance();o=q.step();check(o.semiControl.clockStop&&!o.semiControl.resourceReturnHomeD86&&o.semiControl.totalSeconds==poured);
 tilt(q,.9f);q.advance();o=q.step();check(!o.semiControl.clockStop&&o.semiControl.totalSeconds>poured);
 q.in.palm[1].point={0,0,0};q.in.world[1].point[0]=0;tilt(q,0);q.advance();o=q.step();
 check(o.semiControl.resourceReturnHomeD86&&!o.semiControl.clockStop&&o.control.held[1]&&!o.use);
 q.in.palm[1].point={.5,0,0};q.in.world[1].point[0]=.5f;q.advance();o=q.step();check(!o.semiControl.resourceReturnHomeD86&&o.semiControl.clockStop);
 q.in.tool[1].binding.attachment=0;q.advance();o=q.step();check(!o.semiControl.resourceReturnHomeD86&&o.semiControl.clockStop);
 q.in.tool[1].binding.attachment=777;q.grip(1,0);q.advance();o=q.step();check(o.semiControl.manualUse&&!o.control.held[1]&&!o.use);
 q.grip(1,1);q.advance();o=q.step();check(o.semiControl.manualUse&&!o.control.held[1]&&!o.use);
 std::printf("alchemy_resource_tail_d86: %u/%u passed\n",checks,checks);
}
