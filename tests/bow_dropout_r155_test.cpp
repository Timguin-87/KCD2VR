// R155: a held, nocked physical draw survives a short readiness dropout (bowActions /
// bowGeometryReady175 false for one or two solves while the Grip stays closed), instead
// of cancelling the whole draw with a native uncharge and a rearm (8 Nov 11:08:52.763,
// 11:09:05.6: BOW_STATE HAND_UNAVAILABLE gap=0 rearm=1 gripRL=1.000 at full draw).
// Longer dropouts, tracking loss, an open Grip and the carry phase keep the old cancel.
#define main baseline_bow_coupling_tests
#include "bow_coupling39c1_test.cpp"
#undef main
static void gripDraw(Rig& r){
 r.i.bowGripDraw47c2=true;r.next();
 r.i.trigger[0]=0;r.i.grip[0]=0;r.next();
 r.begins=r.fires=r.cancels=0;
 r.i.grip[0]=1;r.next();r.next();
 ck(r.o.bowHeld&&r.begins==1,"Grip acquires the string");
 r.pull();ck(r.o.bowHeld&&r.o.bowFraction>.5&&r.cancels==0,"the draw is held past half");
}
int main(){
 // one solve of bowActions=false (a tracking sample reported held, an expired 80 ms receipt)
 Rig one;gripDraw(one);const double drawn=one.o.bowFraction;
 one.i.bowActions=false;one.next();
 ck(one.o.bowHeld&&one.cancels==0&&one.fires==0,"one dropout solve keeps the held draw (no cancel, no fire)");
 ck(std::abs(one.o.bowFraction-drawn)<1e-9,"the held draw keeps its fraction through the dropout");
 one.i.bowActions=true;one.next();
 ck(one.o.bowHeld&&one.cancels==0&&one.s.weapon[1].bowDropoutSince155==0,"readiness back: draw continues, dropout clock cleared (no latch)");
 one.i.grip[0]=0;one.next();ck(one.fires==1&&!one.o.bowHeld,"the release after a dropout still shoots once");
 // geometry readiness alone (select_bow_policy175 clears both together in production)
 Rig geo;gripDraw(geo);geo.i.bowGeometryReady175=false;geo.i.bowActions=false;geo.next();geo.next();
 ck(geo.o.bowHeld&&geo.cancels==0,"two solves without geometry readiness keep the held draw");
 geo.i.bowGeometryReady175=geo.i.bowActions=true;geo.next();ck(geo.o.bowHeld&&geo.cancels==0,"geometry readiness back: still held");
 // a dropout longer than kBowDropoutMs155 is the ordinary cancel (10 ms per solve)
 Rig longDrop;gripDraw(longDrop);longDrop.i.bowActions=false;
 for(int n=0;n<int(ph::kBowDropoutMs155/10)+2;++n)longDrop.next();
 ck(!longDrop.o.bowHeld&&longDrop.cancels==1&&longDrop.fires==0,"a long dropout cancels once, never fires");
 ck(longDrop.o.bow116.reason==ph::BowReason116::HandUnavailable&&longDrop.o.bow116.rearm,"long dropout reports HAND_UNAVAILABLE with rearm as before");
 // tracking loss inside the dropout is still an immediate cancel
 Rig lost;gripDraw(lost);lost.i.bowActions=false;lost.i.tracked[0]=false;lost.next();
 ck(!lost.o.bowHeld&&lost.cancels==1&&lost.fires==0,"tracking loss during a dropout cancels at once");
 // an open Grip during the dropout is not bridged (no synthesized hold)
 Rig open;gripDraw(open);open.i.bowActions=false;open.i.grip[0]=0;open.next();
 ck(!open.o.bowHeld&&open.fires==0,"open Grip during a dropout does not keep the draw");
 // F11 off (bowGripDraw47c2 false) still cancels a held draw (unchanged rule)
 Rig mode;gripDraw(mode);mode.i.bowGripDraw47c2=false;mode.next();
 ck(mode.cancels==1&&mode.fires==0&&!mode.o.bowHeld,"Grip mode off cancels a held draw rather than firing it");
 // no draw held: a dropout acquires nothing
 Rig idle;idle.i.bowGripDraw47c2=true;idle.next();idle.i.trigger[0]=0;idle.i.grip[0]=0;idle.next();idle.begins=0;
 idle.i.bowActions=false;idle.i.grip[0]=1;idle.next();idle.next();
 ck(idle.begins==0&&!idle.o.bowHeld,"without readiness a new Grip at the string acquires nothing");
 std::printf("bow_dropout_r155 %d checks ok\n",checks);return 0;}
