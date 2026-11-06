// Reuse the existing production-kernel contact fixture, without running its main.
#define main retained_physical_contract175_main
#include "physical_contract175_test.cpp"
#undef main
static void worldH31(MeleeReplay& r,double scale){
 r.in.haveWorld=r.state.haveWorld=true;r.in.worldScale=r.state.worldScale=scale;
 r.in.world.q=r.state.world.q=m::from_rotation_vector({0,0,.6});r.in.world.p=r.state.world.p={4,-3,1};
}
static Contact103 wallH31(Pose from,int kind){
 Contact103 c{};c.fraction=0;c.plane=true;c.normal={1,0,0};c.point=from.p;
 if(kind>=6&&kind<=10){namespace n=kcd2vr::npc_contact172;c.native172.mask=n::BodiesAndWorld;
  c.native172.body=kind==6?n::Body::Living:kind==7?n::Body::Rigid:kind==8?n::Body::Unknown:n::Body::Physical;
  c.native172.vft=kind>=9?0x3a578c0:0;c.native172.collider=99;
  if(kind==10){c.native172.ownerRead=true;c.native172.owner=100;c.native172.entityId=17;}}
 if(kind==11)c.limited115=true;if(kind==12)c.known=false;return c;
}
int main(){
 ck(!Input{}.wallBackoutSpringH31,"wall assist is OFF by default");
 // Real step instantiation with the retained scalar sweep ABI. A scalar can
 // report clearance/fraction only; it cannot create a wall plane or assist.
 for(bool enabled:{false,true})for(double fraction:{1.,.5}){
  MeleeReplay r;r.in.wallBackoutSpringH31=enabled;r.in.attacks=false;
  const auto start=r.out.weapons[0];r.in.hands[0].p.x+=.05;
  ++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;unsigned queries=0;
  r.out=step(r.state,r.in,r.profiles,[&](const Profile&,Pose,Pose,bool)->double{++queries;return fraction;});
  ck(queries==1&&finite(r.out.weapons[0])&&!r.out.contact119[0].plane&&!r.out.wallBackout58[0]&&
     std::abs(r.out.weapons[0].p.x-start.p.x-.05*fraction)<1e-8&&r.out.commandCount==0,
     "actual scalar sweep callback compiles/runs with experiment OFF/ON and never invents a wall normal or assist");
 }
 {MeleeReplay r;r.in.wallBackoutSpringH31=true;const auto start=r.out.weapons[0];
  for(int bad=0;bad<6;++bad){auto p=r.profiles[0];auto c=wallH31(start,0);unsigned queried=0;Pose rejected{};
   if(bad==0)p.side=-1;if(bad==1)p.side=2;if(bad==2)c.normal={};if(bad==3)c.normal={2,0,0};
   if(bad==4)c.normal.x=std::numeric_limits<double>::quiet_NaN();if(bad==5)c.normal.y=std::numeric_limits<double>::infinity();
   ck(!checked_wall_spring_h31(r.in,p,c,start,start,{.06,0,0},true,rejected,[&](const Profile&,Pose,Pose,bool){++queried;return Contact103{};})&&queried==0,
      "malformed profile side or nonfinite/nonunit plane cannot index arrays or issue experimental queries");
  }
 }
 for(double scale:{.25,1.,3.})for(bool enabled:{false,true})for(int kind=0;kind<=16;++kind){
  MeleeReplay r;worldH31(r,scale);r.in.attacks=false;r.in.wallBackoutSpringH31=enabled;
  r.in.contactFaultH30=kind==13;r.in.contactSuspended59c1=kind==14;
  const auto start=r.out.weapons[0];r.state.weapon[0].wallStuckSince58=kind==15?0:r.in.tick-100;
  r.in.hands[0].p=start.p+Vec3{.06/scale,0,0};
  if(kind==16)r.in.hands[0].q=m::from_rotation_vector({0,0,.3});
  ++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;unsigned queries=0;bool sawRotation=false;
  r.out=step(r.state,r.in,r.profiles,[&](const Profile&,Pose from,Pose to,bool){
   ++queries;if(queries==1)return wallH31(from,kind);Contact103 c{};
   if(enabled&&(kind==1||kind==2)&&queries==2){c.known=kind!=1;c.fraction=.5;c.plane=kind==2;c.normal={-1,0,0};c.point=from.p;}
   if(enabled&&queries==3&&(kind==3||kind==4||kind==5||kind==16)){
    c.known=kind!=3;c.fraction=kind==4||kind==16?0:.8;c.plane=kind==4||kind==16;c.limited115=kind==5;c.normal={0,0,1};c.point=from.p;
    sawRotation=m::angle(from.q,to.q)>.01;}
   return c;});
  const bool assisted=enabled&&(kind==0||kind==9);
  const bool noMove=kind==11||kind==12||kind==15;
  const double moved=(r.out.weapons[0].p.x-start.p.x)*scale,expected=noMove?0.:assisted?.075:.06;
  ck(std::abs(moved-expected)<1e-8,"actual ordinary wall callsite retains metric-capped assist or original checked recovery only");
  ck(r.out.commandCount==0&&r.out.impactSpeed[0]==0&&!r.out.impactRecord178r30[0].valid,"wall extra never fabricates attack, target impact or effort energy");
  if(!enabled)ck(queries==unsigned(noMove?1:2),"OFF preserves original query count and collision behavior");
  if(assisted)ck(queries==3&&m::angle(r.out.weapons[0].q,start.q)<1e-9,"ON retains translation-only pose after forward and independent retention sweeps");
  if(enabled&&kind==16)ck(sawRotation&&queries==4,"blocked requested rotation vetoes extra and falls back to original translation");
  const auto once=r.out.weapons[0];const auto queried=queries;
  const auto repeat=step(r.state,r.in,r.profiles,[&](const Profile&,Pose,Pose,bool){++queries;return Contact103{};});
  ck(queries==queried&&m::length(repeat.weapons[0].p-once.p)<1e-10,"same sample cannot replay wall offset or its extra queries");
 }
 for(double scale:{.25,1.,3.})for(int veto=0;veto<4;++veto){
  MeleeReplay r;worldH31(r,scale);r.in.contact=r.in.physicalCombat177=true;r.in.wallBackoutSpringH31=true;r.in.attacks=false;r.in.reach[0]=.55;
  r.in.hands[0].q=m::from_rotation_vector({0,0,.7});auto& retained=r.state.weapon[0];retained.weapon.p={-1.1,.2,1.2};retained.acquiring124=true;
  const auto seed=retained.weapon;const double requested=r.in.hands[0].p.x-seed.p.x;
  ++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;unsigned queries=0;
  r.out=step(r.state,r.in,r.profiles,[&](const Profile&,Pose from,Pose,bool){++queries;if(queries==1)return wallH31(from,0);
   Contact103 c{};if((veto==1&&queries==2)||(veto==2&&queries==3))c.known=false;
   if(veto==3&&queries==3){c.fraction=0;c.plane=true;c.normal={0,-1,0};c.point=from.p;}return c;});
  const double extra=(r.out.weapons[0].p.x-seed.p.x-requested)*scale;
  ck(std::abs(extra-(veto==0?std::min(.025,requested*scale*.25):0.))<1e-8,"retained acquisition callsite converts world cap into current scaled model frame");
  ck(r.out.owned[0]&&!r.out.handlingOnly55c1[0]&&m::angle(r.out.weapons[0].q,seed.q)<1e-9,"retained recovery preserves checked orientation and anatomical reach");
  ck(queries==unsigned(veto==0||veto==1?3:4)&&r.out.commandCount==0&&r.out.impactSpeed[0]==0,"acquisition failure/secondary veto cannot create native attack or impact energy");
 }
 // Existing raw forward classifier sees the controller, never the assisted pose.
 for(bool backwards:{false,true}){
  MeleeReplay r;worldH31(r,1.);r.in.wallBackoutSpringH31=true;r.in.attacks=true;
  r.in.physicalCombat177=true;select_forward_motion36(r.in,false,true);
  r.state.weapon[0].wallStuckSince58=r.in.tick-100;
  unsigned assistedFrames=0;
  for(int j=0;j<30;++j){++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;unsigned queries=0;
   r.in.hands[0].p.x+=.035;if(backwards){r.in.hands[0].p.y-=.025;r.in.intentHands[0].p.y-=.025;}
   r.out=step(r.state,r.in,r.profiles,[&](const Profile&,Pose from,Pose,bool){return ++queries==1?wallH31(from,0):Contact103{};});
   assistedFrames+=queries==3&&r.out.wallBackout58[0];
   for(unsigned c=0;c<r.out.commandCount;++c)r.commands.push_back(r.out.commands[c]);}
  ck(assistedFrames>0&&!r.count(CommandKind::Prepare)&&!r.count(CommandKind::Slash)&&!r.count(CommandKind::Thrust)&&!r.out.impactRecord178r30[0].valid&&r.out.impactSpeed[0]==0,"actual assisted model movement with stationary raw input or backward windup cannot become attack/energy");
 }
 std::printf("wall_backout_h31=%u/%u actual_kernel=YES native_calls=NONE\n",checks-failures,checks);return failures?1:0;
}
