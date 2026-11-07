#include "vrik/shortcut_cycle167r1.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
namespace sc=kcd2vr::shortcut_cycle167r1;
using sc::Key;using sc::Result;using sc::Modes;
using kcd2vr::f11_tests167::same;
struct Fixture {
 sc::Cycle cycle{};Modes modes{};bool active=false,failLifecycle=false,failWeapon=false;int starts=0,stops=0;
 void apply(Modes value){sc::apply_modes(value,
  [&](int x){modes.interaction=x;},
  [&](int x){modes.combat=x;if(x==3){modes.weapon=6;modes.pose=3;}},
  [&](int x){modes.weapon=failWeapon&&x==7?0:x;if(modes.weapon>=3)modes.pose=3;},
  [&](int x){modes.pose=x;if(x==5)modes.weapon=0;});}
 bool lifecycle(bool on){if(on){++starts;active=!failLifecycle;}else {++stops;active=false;}return !on||!failLifecycle;}
 Result press(Key key,bool calibrated=true){return cycle.press(key,calibrated,[&](){return modes;},[&](Modes x){apply(x);},[&](bool x){return lifecycle(x);});}
 bool stop(bool restore){return cycle.stop(restore,[&](){return modes;},[&](Modes x){apply(x);},[&](bool x){return lifecycle(x);});}
 bool reconcile(){return cycle.reconcile([&](){return modes;},[&](Modes x){apply(x);},[&](bool x){return lifecycle(x);});}
};
int main(){
 for(auto base:{Modes{},Modes{0,0,3,0},Modes{0,0,3,3},Modes{0,2,4,0}}){
  Fixture f;f.modes=base;
  for(int n=0;n<3;++n){assert(f.press(Key::F4)==Result::On);assert(f.modes.combat==3&&f.modes.weapon==7);
   assert(f.press(Key::F4)==Result::Off&&same(f.modes,base));assert(f.cycle.key==Key::None&&!f.active);}
  for(int n=0;n<3;++n){assert(f.press(Key::F8)==Result::On&&f.cycle.rung==1&&same(f.modes,{0,0,4,6}));
   assert(f.press(Key::F8)==Result::On&&f.cycle.rung==2&&f.active&&f.modes.interaction==1&&f.modes.weapon==0);
   assert(f.press(Key::F8)==Result::Off&&same(f.modes,base)&&!f.active);}
 }
 {Fixture f;assert(f.press(Key::F4)==Result::On);assert(f.press(Key::F8)==Result::On&&f.cycle.rung==1);
  assert(f.press(Key::F8)==Result::On&&f.cycle.rung==2);assert(f.press(Key::F8)==Result::Off&&same(f.modes,{}));}
 {Fixture f;f.modes.pose=3;f.press(Key::F8);f.press(Key::F8);assert(f.active);
  assert(f.press(Key::F4)==Result::On&&!f.active);assert(f.press(Key::F4)==Result::Off&&same(f.modes,{0,0,3,0}));}
 {Fixture f;f.press(Key::F8);f.press(Key::F8);f.active=false; // capture timeout changes no selected modes
  assert(f.press(Key::F8)==Result::Off&&same(f.modes,{})&&f.starts==1);}
 {Fixture f;assert(f.press(Key::F4,false)==Result::Blocked&&same(f.modes,{})&&f.cycle.key==Key::None);
  assert(f.press(Key::F8,false)==Result::On&&f.cycle.rung==2&&f.active);assert(f.press(Key::F8,false)==Result::Off&&same(f.modes,{}));}
 {Fixture f;f.press(Key::F4);assert(f.press(Key::F4,false)==Result::Off&&same(f.modes,{}));}
 {Fixture f;f.modes.pose=3;f.failWeapon=true;assert(f.press(Key::F4)==Result::Failed&&same(f.modes,{0,0,3,0})&&f.cycle.key==Key::None);}
 {Fixture f;f.press(Key::F8);f.failLifecycle=true;assert(f.press(Key::F8)==Result::Failed&&same(f.modes,{})&&!f.active&&f.cycle.key==Key::None);}
 {Fixture f;f.press(Key::F8);f.press(Key::F8);f.modes.pose=3;const auto manual=f.modes;
  assert(f.reconcile()&&!f.active&&same(f.modes,manual)&&f.cycle.key==Key::None);assert(f.stop(true)&&same(f.modes,manual));}
 {Fixture f;f.press(Key::F8);const auto selected=f.modes;assert(f.stop(false)&&same(f.modes,selected));assert(f.stop(true)&&same(f.modes,selected));}
 // F11 transfers only after restoring this shared baseline; its OFF therefore
 // cannot re-enable an abandoned F4/F8 preset. Uses the actual F11 lease helper.
 {Fixture f;f.modes.pose=3;f.press(Key::F4);assert(f.stop(true)&&same(f.modes,{0,0,3,0}));
  kcd2vr::f11_tests167::Lease f11;f11.begin(f.modes);f.modes=f11.selected=kcd2vr::f11_tests167::preset(2);
  Modes restored{};assert(f11.end(f.modes,restored));f.apply(restored);
  assert(f.press(Key::F8)==Result::On);assert(f.press(Key::F8)==Result::On);assert(f.press(Key::F8)==Result::Off&&same(f.modes,{0,0,3,0}));}
 {Fixture f;f.modes={0,0,0,3};f.press(Key::F8);f.press(Key::F8);f.press(Key::F8);
  assert(same(f.modes,{0,0,0,3}));} // weapon setter's pose side effect must be restored last
 std::puts("PASS F4 toggle, F8 cycle, both switch orders, F11 transfer, baseline pose restoration, timeout, calibration absence, failed hooks and explicit mode supersession");
}
