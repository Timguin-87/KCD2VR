// Execute the production physical kernel; no native game code is simulated.
#define main baseline_bow_coupling_tests
#include "bow_coupling39c1_test.cpp"
#undef main
#include <limits>
static void grip(Rig& r){
 r.i.bowGripDraw47c2=true;r.next();
 r.i.trigger[0]=0;r.i.grip[0]=0;r.next();
 r.begins=r.fires=r.cancels=0;
 r.i.grip[0]=1;r.next();r.next();
 ck(r.o.bowHeld&&r.begins==1,"F11 Grip acquires string without Trigger");
}
int main(){
 Rig cold;cold.s={};cold.i.trigger[0]=cold.i.grip[0]=0;
 ph::select_bow_policy175(cold.i,cold.p[1],true,true,true,false,false,true);
 ck(cold.i.bowGripDraw47c2&&cold.i.bowLoadPolicy175==ph::BowLoadPolicy175::NativeAutomatic,"default Grip keeps automatic native loading");
 cold.begins=cold.fires=cold.cancels=0;cold.next();cold.i.grip[0]=1;cold.next();
 ck(cold.o.bowHeld&&!cold.s.weapon[1].bowGrip38,"first default Grip press acquires native draw before physical grasp publication");
 cold.i.hands[1].q=m::from_rotation_vector({0,.30,0});
 cold.i.hands[0].q=m::from_rotation_vector({.12,0,.08});cold.next();
 ck(cold.o.bowHeld&&cold.begins==1&&cold.cancels==0,"first press after neutral calibration acquires default Grip draw");
 ck(m::angle(cold.o.hands[0].q,cold.i.hands[0].q)<1e-7,"first default Grip latch uses current root during the draw-to-aim rotation");
 cold.pull();cold.i.grip[0]=0;cold.next();ck(cold.fires==1,"default first draw releases once");

 Rig r;grip(r);r.pull();ck(r.o.bowFraction>.5&&r.s.weapon[1].bowGrip38,"Grip hand pulls and couples the whole bow");
 r.i.trigger[0]=1;r.next();r.i.trigger[0]=0;r.next();
 ck(r.fires==0&&r.o.bowHeld,"Trigger does not fire or cancel a Grip-owned draw");
 for(int n=0;n<25;++n){r.i.hands[0].p.x+=.004;r.next();}
 const auto aim=r.o.weapons[1].q;const auto left=r.o.hands[1].p;
 r.i.grip[0]=0;r.next();
 ck(r.fires==1&&!r.o.bowHeld&&r.cancels==0,"deliberate Grip release requests one shot");
 ck(m::angle(r.o.weapons[1].q,aim)<1e-6,"Grip release preserves the coupled aim on the native fire frame");
 ck(m::length(r.o.hands[1].p-left)<1e-8,"left grip stays the bow pivot");
 r.next();r.next();ck(r.fires==1,"Grip release cannot repeat a shot");
 Rig mode;grip(mode);mode.pull();mode.i.bowGripDraw47c2=false;mode.next();
 ck(mode.cancels==1&&mode.fires==0&&!mode.o.bowHeld,"F11 off cancels a held draw rather than firing it");
 Rig lost;grip(lost);lost.pull();lost.i.tracked[0]=false;lost.next();
 ck(lost.cancels==1&&lost.fires==0&&!lost.o.bowHeld,"tracking loss cancels Grip draw");
 Rig bad;grip(bad);bad.pull();bad.i.grip[0]=std::numeric_limits<double>::quiet_NaN();bad.next();
 ck(!bad.o.bowHeld&&bad.fires==0,"invalid Grip cannot sustain or fire the draw");
 Rig shortDraw;grip(shortDraw);shortDraw.i.grip[0]=0;shortDraw.next();
 ck(shortDraw.fires==0&&shortDraw.cancels==1,"release without a pull is an uncharge");
 std::printf("PASS %d Grip draw/coupling/release checks\n",checks);
}
