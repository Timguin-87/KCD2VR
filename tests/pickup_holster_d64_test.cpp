// Actual production holster producer with existing explicit host/native leaves.
#define main ExistingHolsterD44Tests
#include "holster_instant_d44_test.cpp"
#undef main
int main(){
 const int previous=ExistingHolsterD44Tests();
 auto& retirement=kcd2vr::pickup_grip_d64::retirement;
 kcd2vr::grasp13::AcquisitionInput55 raw{};raw.character=0x1234;raw.epoch=1;raw.trackingEpoch=3;
 raw.sample=10000;raw.tick=tickD44;raw.tracked[0]=raw.tracked[1]=true;raw.grip[0]=raw.grip[1]=1;
 g_committed_character6689=reinterpret_cast<void*>(raw.character);
 claimD44[0]=claimD44[1]=false;nativeBusyD48=false;g_native_held_release_inhibit_t4=false;
 {Case c;
  check(retirement.consume(raw.character,1,991,1,raw),"pickup integration captures the exact left-hand terminal occurrence");
  c.step();c.step();check(!c.draws&&!c.state[0].comparison114.requireReleaseR15,
   "consumed pickup Grip cannot Draw and does not add a redundant pose-local rearm");
  // Opening is observed by XR while no pose/holster callback runs.
  raw.sample++;raw.tick++;raw.grip[1]=0;retirement.observe(raw);
  raw.sample++;raw.tick++;raw.grip[1]=1;retirement.observe(raw);
  c.step();check(c.draws==1,"next valid closed Grip draws after a raw opening even if all open-hand poses were skipped");
  c.step();check(c.draws==1,"native draw stays exactly once after occurrence retirement");
 }
 {Case c;raw.sample++;raw.tick++;check(retirement.consume(raw.character,1,992,1,raw),"second pickup terminal has its own occurrence");
  claimD44[1]=true;c.step();check(c.state[0].comparison114.requireReleaseR15,
   "simultaneous real hand claim preserves its existing independent release requirement");
  claimD44[1]=false;raw.sample++;raw.tick++;raw.grip[1]=0;retirement.observe(raw);c.in.grip[1]=0;c.step();
  c.in.grip[1]=1;c.step();check(c.draws==1,"ordinary actual release still rearms after a separate hand claim");
 }
 std::printf("D64 pickup production holster %u/%u\n",total-failed,total);
 return previous||failed?1:0;
}
