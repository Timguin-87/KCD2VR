#if defined(CG1_REPRO_PARENT)
#include "cg1_parent_policy.h"
#else
#include "../src/subsystems/physics/corpse_grip_cg1.h"
#endif
#include "../src/vrik/physical_interaction.h"
#include "../src/vrik/held_interaction114.h"
#include "../src/tracking_sample59c6.h"
#include "../src/vrik/interaction_ownership111.h"
#include <cstdio>
#include <cstdlib>
#include <limits>
using namespace kcd2vr;
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);std::abort();}}while(0)
// Actual worker include -> actual pulse/held producers. Native candidate reader
// and XR transport are explicit leaves, not simulated acceptance decisions.
static std::uint64_t clockCg1=1000;
static std::uint64_t GetTickCount64(){return clockCg1;}
namespace frik_pose_probe {
inline bool current=true,qualified=true;
inline corpse_grip_cg1::Candidate native{};
bool ReadCorpseGrabCandidateCg1(corpse_grip_cg1::Candidate& out,std::uint64_t,bool& input){input=current;out=qualified?native:corpse_grip_cg1::Candidate{};return qualified;}
}
namespace kcd2vr::hand_only_door_h40 {struct Admission{bool take(unsigned){return true;}};}
struct Fixture {
 struct Controller {float grip=0;bool grip_valid=true;};
 struct XR {Controller left;std::uint64_t tick=0,epoch=1;bool read=true;
  const Controller& LeftController(){return left;}
  bool CopyInputSampleStamp59c6(tracking59c6::Stamp& s){s.tick=tick;s.epoch=epoch;return read;}
 } xr;
 std::uint64_t stampAge=0;
 unsigned f5Mode=0;bool ordinaryService=true,calibrated=true,tracked=true,empty=true;
 XR* xr_manager_=&xr;
 physical_interaction106::State pulse{};held_interaction114::State held{};
 physical_interaction106::Output po{};held_interaction114::Output ho{};
 unsigned aCalls=0,holdCalls=0;bool actualA=false,deliveredA=false;
 corpse_grip_cg1::Request request{};
 void frame(std::uint64_t now,float left,float right=0,bool menu=false){
  clockCg1=now;xr.left.grip=left;xr.tick=now-stampAge;
  physical_interaction106::Input pi{};
  pi.enabled=(interaction_ownership111::Comparison114(0,f5Mode,menu,calibrated)||
   interaction_ownership111::Live(0,f5Mode,menu,calibrated,false))&&ordinaryService;
  pi.menu=menu;pi.now_ms=now;
  for(int s=0;s<2;++s){pi.hand[s]={tracked,empty,true,s?left:right,false};pi.hover_age_ms[s]=200;}
  auto& f5Physical=pulse;auto& heldUse114=held;std::uint64_t corpseReturnedPressD78[2]{};
#if defined(CG1R2_REPRO_PARENT)
#include "cg1r1_worker_parent.inc"
#else
#include "generated_corpse_grip_worker_cg1.inc"
#endif
  po=physical_interaction106::Step(pulse,pi);ho=held_interaction114::update(held,pi);
  if(po.fire_use)++aCalls;if(ho.hold)++holdCalls;
  deliveredA=actualA; // physical producer does not alter the controller A path
  (void)corpse_grip_cg1::request.read(request);
 }
};
int main(){
 namespace c=corpse_grip_cg1;
 c::Key key{11,22,33,44,0,55,6};c::Publisher pub;
 auto at=[&](std::uint64_t now,bool active=false){frik_pose_probe::native=pub.publish(key,now,active);};
 Fixture f;at(1000);f.frame(1000,0);CHECK(!f.request.press&&!f.aCalls);
 at(1010);f.frame(1010,1);const auto token=f.request.press;
 CHECK(token&&c::permits(f.request,frik_pose_probe::native,1010));CHECK(!f.po.ray_active&&!f.ho.ray);
 at(1040,true);f.frame(1040,1);CHECK(f.request.press==token&&!f.aCalls&&!f.holdCalls);
 // Native permission loss cannot turn the same consumed squeeze into A.
 frik_pose_probe::qualified=false;f.frame(1050,1);CHECK(!f.request.press&&!f.po.ray_active);
 at(1070);frik_pose_probe::qualified=true;f.frame(1070,1);CHECK(!f.request.press&&!f.aCalls);
 at(1090);f.frame(1090,0);CHECK(!f.request.press&&!f.aCalls&&!f.po.ray_active);
 at(1100);f.frame(1100,1);CHECK(f.request.press&&f.request.press!=token);
 // Replacement actor, root, epoch or part retires native authority until release.
 for(int field=0;field<4;++field){
  f.frame(clockCg1+10,0);const auto now=clockCg1+10;at(now);f.frame(now,1);CHECK(f.request.press);
  if(field==0)++key.actor;if(field==1)++key.root;if(field==2)++key.epoch;if(field==3)++key.part;
  at(now+10);f.frame(now+10,1);CHECK(!f.request.press&&!f.po.ray_active);
 }
 at(1250);f.frame(1250,0);at(1260);f.frame(1260,1);CHECK(f.request.press);
 frik_pose_probe::current=false;f.frame(1270,0);CHECK(!f.request.press&&!f.po.ray_active);
 frik_pose_probe::current=true;at(1280);f.frame(1280,1);CHECK(!f.request.press);
 at(1290);f.frame(1290,0);at(1300);f.frame(1300,1);CHECK(f.request.press);
 f.actualA=true;at(1310);f.frame(1310,1);CHECK(f.deliveredA&&!f.aCalls);
 at(1320);f.frame(1320,1,1);at(1350);f.frame(1350,1,1);CHECK(f.po.fire_use&&f.po.ray_owner==0&&f.ho.hold&&f.ho.owner==0);
 // After a genuine release with no candidate, ordinary LEFT Use still works.
 frik_pose_probe::qualified=false;f.frame(1360,0);f.frame(1370,1);f.frame(1400,1);
 CHECK(f.po.fire_use&&f.po.ray_owner==1&&f.ho.owner==1&&f.ho.hold);
 // Inactive squeeze reports0 in legacy input, but is NOT a genuine release.
 at(1450);frik_pose_probe::qualified=true;f.frame(1450,0);
 at(1460);f.frame(1460,1);CHECK(f.request.press);
 f.xr.left.grip_valid=false;at(1470);f.frame(1470,0);CHECK(!f.request.press&&!f.po.ray_active);
 f.xr.left.grip_valid=true;at(1480);f.frame(1480,1);CHECK(!f.request.press&&!f.po.ray_active);
 at(1490);f.frame(1490,0);frik_pose_probe::qualified=false;
 f.frame(1500,1);f.frame(1530,1);CHECK(f.po.fire_use&&f.ho.hold&&f.ho.owner==1);
 // Fresh pose cannot renew stale action time or a previous input epoch.
 frik_pose_probe::qualified=true;at(1550);f.frame(1550,0);at(1560);f.frame(1560,1);CHECK(f.request.press);
 f.stampAge=81;at(1570);f.frame(1570,0);CHECK(!f.request.press&&!f.po.ray_active);
 f.stampAge=0;at(1580);f.frame(1580,1);CHECK(!f.request.press);
 at(1590);f.frame(1590,0);at(1600);f.frame(1600,1);CHECK(f.request.press);
 f.xr.epoch=99;at(1610);f.frame(1610,0);CHECK(!f.request.press&&!f.po.ray_active);
 f.xr.epoch=1;at(1620);f.frame(1620,1);CHECK(!f.request.press);
 at(1630);f.frame(1630,0);at(1640);f.frame(1640,1);CHECK(f.request.press);
 // F5/backend choose ordinary Use, not native corpse physics authority.
 at(1650);f.frame(1650,0);f.f5Mode=1;
 at(1660);f.frame(1660,1);CHECK(f.request.press&&!f.po.fire_use&&!f.ho.hold);
 const auto f9Token=f.request.press;
 at(1670,true);f.frame(1670,1);CHECK(f.request.press==f9Token);
 f.f5Mode=0;f.ordinaryService=false;
 at(1680,true);f.frame(1680,1);CHECK(f.request.press==f9Token&&!f.po.fire_use&&!f.ho.hold);
 // Menu blocks corpse and generic Use even if the native reader leaf is stale.
 at(1690,true);f.frame(1690,1,0,true);CHECK(!f.request.press&&!f.po.fire_use&&!f.ho.hold);
 at(1700);f.frame(1700,1);CHECK(!f.request.press);
 at(1710);f.frame(1710,0);at(1720);f.frame(1720,1);CHECK(f.request.press);
 // Other-hand/body ownership and unknown candidates remain refusals.
 f.tracked=false;at(1730);f.frame(1730,1);CHECK(!f.request.press);
 f.tracked=true;at(1740);f.frame(1740,0);f.empty=false;at(1750);f.frame(1750,1);CHECK(!f.request.press);
 f.empty=true;f.frame(1760,0);frik_pose_probe::qualified=false;f.frame(1770,1);f.frame(1800,1);
 CHECK(!f.request.press&&!f.po.fire_use&&!f.ho.hold);
 // Generic Use still obeys its own backend when no corpse has claimed Grip.
 f.ordinaryService=true;f.frame(1810,0);f.frame(1820,1);f.frame(1850,1);
 CHECK(!f.request.press&&f.po.fire_use&&f.ho.hold);
 // Pending generic pulse is retired if corpse input claims the hand before due.
 c::Gate g;auto candidate=pub.publish(key,2000,false);CHECK(!g.update(2000,true,true,0,candidate).suppressLeftUse);
 auto out=g.update(2010,true,true,1,candidate);CHECK(out.suppressLeftUse&&out.native.press);
 CHECK(!g.update(2020,true,false,1,candidate).native.press);
 CHECK(!g.update(2030,true,true,1,candidate).native.press);
 CHECK(!g.update(2040,true,false,0,{}).suppressLeftUse);
 // Cross-tick/future/stale/unknown evidence never authorizes native drive.
 CHECK(!c::permits(out.native,candidate,1999));CHECK(!c::permits(out.native,candidate,2100));
 auto changed=candidate;++changed.key.part;CHECK(!c::permits(out.native,changed,2020));
 CHECK(!c::permits({},candidate,2020));
 c::Gate nan;nan.update(2000,true,true,0,candidate);
 CHECK(!nan.update(2010,true,true,std::numeric_limits<float>::quiet_NaN(),candidate).native.press);
 CHECK(!nan.update(2020,true,true,1,candidate).native.press);
 std::printf("CG1 worker/pulse/held/permission checks PASS %d\n",checks);
}
