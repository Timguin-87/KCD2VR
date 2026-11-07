#define main station_reposition_original_main
#include "station_reposition_d53_test.cpp"
#undef main
int main(){
 for(unsigned kind:{unsigned(mh::Alchemy),unsigned(mh::Smithing)}){
  Fixture f(kind);
  for(unsigned n=0;n<30;++n){
   f.next(20);f.tracking.sequence+=2;f.send(1,0);
   // The independent XR input loop published its fresh joystick after this
   // body solve sampled tracking. Body sample and input both remain current.
   f.fi.stamp.sample-=2;f.move.sample=f.fi.stamp.sample;
   f.fi.stamp.tick-=10;f.move.tick=f.fi.stamp.tick;
   f.solve();
  }
  ck(std::fabs(f.frame.stationOffsetD53.x-.18)<1e-7,"fresh asynchronous joystick moves station despite older body sample");
  const auto held=f.frame.stationOffsetD53;f.solve();
  ck(close(f.frame.stationOffsetD53,held),"repeated body callback cannot integrate joystick twice");
  // Time-travel, old commands, wrong identity and a new station still fail.
  f.next();f.send(1,0);f.move.command.tick=f.move.now+1;f.solve();
  ck(close(f.frame.stationOffsetD53,held),"future WALLCLOCK input still refuses");
  f.next();f.send(1,0);f.move.command.tick=f.move.now-81;f.solve();
  ck(close(f.frame.stationOffsetD53,held),"stale input still refuses without removing the retained position");
  f.next();f.send(1,0);++f.move.command.instance;f.solve();
  ck(close(f.frame.stationOffsetD53,held),"foreign station command never moves this station");
  f.next();f.send(1,0);++f.move.command.tracking;f.solve();
  ck(close(f.frame.stationOffsetD53,held),"wrong tracking epoch never moves this station");
  f.next();f.send(0,0);f.solve();
  ck(close(f.frame.stationOffsetD53,held),"neutral recovery never replays a dropped interval");
 }
 std::printf("D74 async station: %u checks PASS; bounded shared station offset, no native capsule movement\n",checks);return 0;
}
