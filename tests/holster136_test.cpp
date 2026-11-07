// V66.136R1: sticky zone stow, explicit release-anywhere compatibility, user shoulder mapping.
#include "../src/vrik/physical_weapon.h"
#include "../src/vrik/equipment_gesture114.h"
#include <cstdio>
using namespace kcd2vr; using namespace kcd2vr::equipment114; namespace m=kcd2vr::pose_core;
static unsigned checks=0,failures=0; static void check(bool b,const char*n){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static Input base(){Input i{};i.key=1;i.generation=1;i.epoch=1;i.tick=1000;i.eligible=true;i.drawn=true;i.mountKnown=true;
    i.mount={0,0,1};i.hand={.8,0,1};i.grip=1;i.radius=.24;i.releaseAnywhere=false;return i;}
int main(){
 for(int side=0;side<2;++side){physical::Input physical{};physical.allowed=physical.tracked[side]=physical.intentTracked[side]=true;
  physical.epoch=7;physical.sample=12;physical.hands[side].p={0,0,0};physical::Vec3 mount{.2,-.1,1.4};
  auto run=[&](bool corrected){State state{};auto input=base();input.drawn=false;input.mount=mount;int draws=0;
   for(int n=0;n<30;++n){auto hand=mount+physical::Vec3{0,n<5?0.:-.015*(n-5),0};
    const auto tracking=physical::holster_tracking_input_h43(physical,side?physical.hands[0].p:hand,side?hand:physical.hands[1].p);
    check(tracking.epoch==physical.epoch&&tracking.sample==physical.sample&&tracking.allowed==physical.allowed,
        "tracking intent preserves source and gameplay eligibility");
    input.hand=corrected?tracking.hands[side].p:physical.hands[side].p;input.grip=n<3?0:1;input.tick=1000+n*11;
    draws+=update(state,input)==Request::Draw;}
   return draws;};
  check(run(false)==0&&run(true)==1,"both holsters: tracked reach/grip/pull draws once despite compressed visual hand");
 }

 check(CombatFistOwnsHandH39(true,1,1),"qualified empty fist owns only its own holster gesture");
 check(!CombatFistOwnsHandH39(false,1,1),"held equipment keeps normal holster release");
 check(!CombatFistOwnsHandH39(true,0,1)&&!CombatFistOwnsHandH39(true,1,0),"either single control is not a combat fist");
 check(!CombatFistOwnsHandH39(true,double(.60f),1)&&!CombatFistOwnsHandH39(true,1,double(.60f)),"both fist controls must strictly exceed sixty percent");
 // Default: release away from a holster does not stow.
 {State s{};auto i=base();update(s,i);i.tick+=20;i.grip=0;check(update(s,i)==Request::None,"default release away from zone keeps weapon held/drawn");}
 // Headset regression: enter 13 cm, move to 27 cm for 96 ms, then release -> still stow.
 {State s{};auto i=base();i.hand={.13,0,1};update(s,i);i.tick+=60;update(s,i);i.hand={.27,0,1};i.tick+=96;update(s,i);i.tick+=1;i.grip=0;
  check(update(s,i)==Request::Holster,"sticky latch survives 13cm->27cm movement until release");}
 // But staying clearly outside for >250 ms cancels the armed holster.
 {State s{};auto i=base();i.hand={.13,0,1};update(s,i);i.tick+=60;update(s,i);i.hand={.50,0,1};i.tick+=10;update(s,i);for(int n=0;n<6;++n){i.tick+=50;update(s,i);}i.grip=0;i.tick+=1;
  check(update(s,i)==Request::None,"sticky latch cancels after sustained move well outside zone");}
 // Transient mount disappearance after a valid latch must not erase the release gesture.
 {State s{};auto i=base();i.hand={.12,0,1};update(s,i);i.tick+=70;update(s,i);i.mountKnown=false;i.tick+=80;update(s,i);i.grip=0;i.tick+=1;
  check(update(s,i)==Request::Holster,"latched stow survives transient mount-read loss");}
 // Optional compatibility mode remains available but is not the default.
 {State s{};auto i=base();i.releaseAnywhere=true;i.hand={.8,0,1};update(s,i);i.tick+=20;i.grip=0;check(update(s,i)==Request::None,"compat release-anywhere begins debounce");
  for(int n=0;n<3;++n){i.tick+=50;auto r=update(s,i);if(n<2)check(r==Request::None,"compat release-anywhere continues debounce");else check(r==Request::Holster,"compat release-anywhere stows after debounce");}}
 // Draw remains reach, grip, pull.
 {State s{};auto i=base();i.drawn=false;i.grip=0;i.hand={.20,0,1};update(s,i);i.grip=1;i.tick+=10;update(s,i);
  i.hand={.52,0,1};i.tick+=90;check(update(s,i)==Request::Draw,"draw: grip in zone and pull beyond exit threshold");}
 // C10R2: a vertical mace draw may still be inside a generous acquisition sphere.
 {State s{};auto i=base();i.drawn=false;i.radius=.28;i.grip=0;i.hand={.20,0,1};update(s,i);
  i.grip=1;i.tick+=10;update(s,i);i.hand={.20,0,1.14};i.tick+=90;
  check(update(s,i)==Request::Draw,"12 cm outward mace lift draws even inside acquisition sphere");
  i.tick+=10;check(update(s,i)==Request::None,"mace lift remains one request until native acknowledgement");}
 {State s{};auto i=base();i.drawn=false;i.radius=.28;i.grip=0;i.hand={.20,0,1};update(s,i);
  i.grip=1;i.tick+=10;update(s,i);i.hand={.05,0,1};i.tick+=90;
  check(update(s,i)==Request::None,"moving grabbed hand inward is not withdrawal");}
 {State s{};auto i=base();i.drawn=false;i.radius=.28;i.grip=0;i.hand={.20,0,1};update(s,i);
  i.grip=1;i.tick+=10;update(s,i);i.hand.x+=.30;i.mount.x+=.30;i.tick+=90;
  check(update(s,i)==Request::None,"shared hand/body mount motion supplies no withdrawal");}
 // Grip-only acquisition may start outside the zone after a neutral hand.
 // Actual Trigger+Grip fist exclusion is tested in the full production producer.
 {State s{};auto i=base();i.drawn=false;i.grip=0;i.hand={.90,0,1};update(s,i);i.grip=1;i.tick+=10;update(s,i);
  i.hand={.20,0,1};i.tick+=10;update(s,i);i.hand={.52,0,1};i.tick+=90;check(update(s,i)==Request::Draw,"neutral then Grip before reaching draws after original dwell and pull");}
 {State s{};auto i=base();i.drawn=false;i.grip=0;i.hand={.20,0,1};update(s,i);i.grip=.4;i.tick+=10;update(s,i);
  i.hand={.90,0,1};i.tick+=10;update(s,i);i.grip=1;i.tick+=10;update(s,i);i.hand={.20,0,1};i.tick+=10;update(s,i);
  i.hand={.52,0,1};i.tick+=90;check(update(s,i)==Request::Draw,"neutral remains valid across intermediate Grip outside the zone");
  i.grip=0;i.hand={.20,0,1};i.tick+=10;update(s,i);i.grip=1;i.tick+=10;update(s,i);i.hand={.52,0,1};i.tick+=90;check(update(s,i)==Request::None&&s.pending,"new Grip cannot duplicate a draw before native acknowledgement");}
 // A real pending draw blocks only its current, unacknowledged live item.
 {State s{};auto i=base();i.drawn=false;i.grip=0;i.hand={.20,0,1};update(s,i);
  i.grip=1;i.tick+=10;update(s,i);i.hand={.52,0,1};i.tick+=90;
  check(update(s,i)==Request::Draw,"arbitration fixture produces an actual pending draw");
  auto blocks=[&](std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t now,bool drawn){
   return PendingDrawBlocks(s,key,generation,epoch,now,drawn);};
  check(blocks(i.key,i.generation,i.epoch,i.tick,false),"current native-unacknowledged draw excludes competing acquisition");
  check(!blocks(i.key,i.generation,i.epoch,i.tick,true),"native drawn acknowledgement releases cross-item exclusion before item loop order");
  check(!blocks(i.key+1,i.generation,i.epoch,i.tick,false),"reordered item key cannot inherit an old pending draw lock");
  check(!blocks(i.key,i.generation+1,i.epoch,i.tick,false),"replacement generation cannot inherit an old pending draw lock");
  check(!blocks(i.key,i.generation,i.epoch+1,i.tick,false),"new request epoch cannot inherit an old pending draw lock");
  check(!blocks(i.key,i.generation,i.epoch,i.tick-1,false),"backward source time cannot retain cross-item exclusion");
  check(blocks(i.key,i.generation,i.epoch,i.tick+100,false)&&!blocks(i.key,i.generation,i.epoch,i.tick+101,false),"skipped stale item releases draw exclusion at actual FSM 100ms boundary");
  s.tick=s.sent+1999;
  check(blocks(i.key,i.generation,i.epoch,s.sent+1999,false)&&!blocks(i.key,i.generation,i.epoch,s.sent+2000,false),"pending draw expires at actual FSM 2-second boundary");
  s.pending=false;check(!blocks(i.key,i.generation,i.epoch,s.tick,false),"retired request cannot exclude other holsters");
 }
 physical::Profile bow{};bow.classId=9;physical::Profile shield{};shield.classId=8;physical::Profile two{};two.classId=4;two.twoHanded=true;
 physical::Profile one{};one.classId=2;
 check(physical::holster_zone126(bow)==physical::HolsterZone126::LeftShoulderBow&&physical::holster_hand126(bow)==1,"HS2 bow: OVER the LEFT shoulder in its own zone, left hand (Miki 25 Oct)");
 check(physical::holster_zone126(shield)==physical::HolsterZone126::LeftShoulder&&physical::holster_hand126(shield)==1,"shield: LEFT shoulder, left hand (R14B)");
 check(physical::holster_zone126(two)==physical::HolsterZone126::NativeBelt,"V146R1 long sword flagged two-handed: native hip scabbard");
 physical::Profile pole{};pole.classId=7;check(physical::holster_zone126(pole)==physical::HolsterZone126::RightShoulder&&physical::holster_hand126(pole)==0,"C45 polearm: right shoulder, right hand");
 check(physical::holster_zone126(one)==physical::HolsterZone126::NativeBelt,"one-handed: native hip mount");
 bow.legacyHolsters147=shield.legacyHolsters147=pole.legacyHolsters147=true;
 check(physical::holster_zone126(bow)==physical::HolsterZone126::LeftShoulderBow&&physical::holster_zone126(shield)==physical::HolsterZone126::LeftShoulder&&physical::holster_zone126(pole)==physical::HolsterZone126::RightShoulder,"HS2 layout: bow own LEFT zone, shield LEFT, polearm RIGHT");
 check(physical::holster_hand126(bow)==1&&physical::holster_hand126(pole)==0,"H42: bow belongs to LEFT hand/shoulder, polearm RIGHT hand/shoulder");
 std::printf("%u checks, %u failures\n",checks,failures);return failures?1:0;}
