"""Execute the actual native retirement/rearm block with native leaves replaced."""
from pathlib import Path
import sys
from source_text import read_source

root = Path(__file__).resolve().parents[1]
source = read_source(root / 'src/vrik/physical_native.inl')
start = source.index('    // T15 retirement recovery:')
end = source.index('    // Cancel old prepared native combat', start)
block = source[start:end]
assert block.count('TrackingHandUsable111(0,now)') == 1
assert block.count('now-frame.input.tick<=80') == 1

prefix = r'''
#include "subsystems/throw_drop/release_rearm_t15.h"
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <limits>
std::atomic<bool> g_native_held_release_inhibit_t4{true};
std::atomic<float> g_right_grip{0};
std::atomic<std::uint64_t> g_weapon_request_epoch6694{23},g_pose_epoch6689{29};
namespace heldT4 {
struct Achieved {bool colliderRetired=false;};
bool enabled=true,read=true,retired=true;unsigned reads=0;
bool Enabled(){return enabled;}
bool Read(Achieved& a){++reads;a.colliderRetired=retired;return read;}
}
bool busy=false,active=true,have=true,handUsable=true;
std::uint64_t now=1000;
bool NativeHeldBusyT4(){return busy;}
bool TrackingHandUsable111(int side,std::uint64_t tick){assert(side==0&&tick==now);return handUsable;}
struct Equipment {
 bool valid=true,handKnown[2]{true,true};std::uint64_t handKey[2]{};
 std::uint64_t request=23,epoch=29;std::uintptr_t player=31,manager=37;
} e;
struct Frame {
 struct Input {
  bool allowed=true,tracked[2]{true,true},intentTracked[2]{true,true};
  std::uint64_t sample=41,tick=1000,epoch=23,bodyEpoch=29;
 } input;
 std::uint64_t request=23;std::uintptr_t player=31,manager=37;
} frame;
unsigned checks=0;
void check(bool value){++checks;assert(value);}
void reset(){
 g_native_held_release_inhibit_t4=true;g_right_grip=0;
 g_weapon_request_epoch6694=23;g_pose_epoch6689=29;
 busy=false;active=have=handUsable=true;now=1000;e={};frame={};
 heldT4::enabled=heldT4::read=heldT4::retired=true;heldT4::reads=0;
}
void run(){
'''
suffix = r'''
}
int main(){
 reset();run();check(!g_native_held_release_inhibit_t4.load());check(heldT4::Enabled());
 check(heldT4::reads==1); // read-only retirement proof, no physics toggle/body operation
 reset();frame.input.tick=now-80;g_right_grip=.249f;run();check(!g_native_held_release_inhibit_t4.load());
 for(unsigned cause=0;cause<29;++cause){
  reset();
  switch(cause){
  case 0:active=false;break;case 1:have=false;frame.input.sample=0;break;
  case 2:frame.input.tick=0;break;case 3:frame.input.tick=now+1;break;
  case 4:frame.input.tick=now-81;break;case 5:frame.input.tick=now-101;break;
  case 6:frame.input.allowed=false;break;case 7:frame.input.tracked[0]=false;break;
  case 8:frame.input.intentTracked[0]=false;break;case 9:handUsable=false;break;
  case 10:e.valid=false;break;case 11:e.player=0;break;case 12:e.manager=0;break;
  case 13:++e.player;break;case 14:++e.manager;break;case 15:++e.epoch;break;
  case 16:++e.request;break;case 17:++frame.input.epoch;break;
  case 18:++frame.request;++e.request;++frame.input.epoch;break; // same tuple, old global request
  case 19:++g_weapon_request_epoch6694;break;case 20:++g_pose_epoch6689;break;
  case 21:e.handKnown[0]=false;break;case 22:e.handKey[0]=99;break;
  case 23:busy=true;break;case 24:heldT4::read=false;break;case 25:heldT4::retired=false;break;
  case 26:g_right_grip=std::numeric_limits<float>::quiet_NaN();break;
  case 27:g_right_grip=.25f;break;case 28:g_right_grip=.55f;break;
  }
  run();check(g_native_held_release_inhibit_t4.load());check(heldT4::Enabled());
  if(cause<=23)check(heldT4::reads==0); // stale/changed/unavailable owners never consult old receipt
 }
 // Legacy OFF behavior is the original independent >=.55 close-Grip edge.
 reset();heldT4::enabled=false;active=have=handUsable=false;e.valid=false;
 frame.input.tick=0;g_weapon_request_epoch6694=999;g_right_grip=.55f;
 run();check(!g_native_held_release_inhibit_t4.load());check(heldT4::reads==0);
 for(float grip:{0.f,.54f,std::numeric_limits<float>::quiet_NaN()}){
  reset();heldT4::enabled=false;g_right_grip=grip;run();check(g_native_held_release_inhibit_t4.load());
 }
 reset();heldT4::enabled=false;busy=true;g_right_grip=1;run();check(g_native_held_release_inhibit_t4.load());
 reset();g_native_held_release_inhibit_t4=false;run();check(!g_native_held_release_inhibit_t4.load());check(heldT4::reads==0);
 std::printf("T15 actual native rearm block: %u checks PASS; fresh<=80ms exact owner/epochs, current empty/open Grip, retired collision fence; legacyOFF preserved; engine_calls=NONE\n",checks);
}
'''
Path(sys.argv[1]).write_text(prefix + block + suffix, encoding='utf-8', newline='\n')
