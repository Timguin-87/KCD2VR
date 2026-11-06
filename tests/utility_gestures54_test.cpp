#include "../src/vrik/utility_gestures54.h"
#include <cassert>
#include <limits>
#include <cstring>
using namespace kcd2vr::utility54;
int main(){
 // A face-level fist must never lower the visor, consume its Trigger, or
 // become a utility gesture when Grip is released before Trigger.
 {State f{};Input j{1000,true,true,true,false,-.16,.04,0,0};update(f,j);
  j.tick+=20;j.trigger=.9;j.grip=.9;assert(!update(f,j).consumesTrigger);
  j.tick+=20;j.up=-.12;assert(update(f,j).action==Action::None);
  j.tick+=20;j.grip=0;assert(!update(f,j).consumesTrigger);
  j.tick+=20;j.up=.10;assert(update(f,j).action==Action::None);
  j.tick+=20;j.trigger=0;j.up=0;update(f,j);
  j.tick+=20;j.trigger=.9;assert(update(f,j).consumesTrigger);
  j.tick+=20;j.grip=.9;j.up=-.12;auto out=update(f,j);
  assert(out.action==Action::None&&!out.consumesTrigger);
 }
 State s{};Input i{1000,true,true,true,false,-.16,.04,0,0};update(s,i);
 i.tick+=20;i.trigger=.9;assert(update(s,i).consumesTrigger);
 i.tick+=20;i.up=-.10;assert(update(s,i).action==Action::VisorClose);
 i.tick+=20;i.up=.10;assert(update(s,i).action==Action::None); // held trigger cannot toggle back
 i.tick+=20;i.trigger=0;update(s,i);i.tick+=20;i.trigger=.9;update(s,i);
 i.tick+=20;i.up=.20;assert(update(s,i).action==Action::VisorOpen);
 i.tick+=20;i.trigger=0;i.up=-.22;i.forward=-.23;i.right=-.30;update(s,i);
 i.tick+=20;i.trigger=.9;update(s,i);for(int k=0;k<7;++k){i.tick+=20;assert(update(s,i).action==Action::None);}
 i.tick+=20;assert(update(s,i).action==Action::Torch);
 i.tick+=20;assert(update(s,i).action==Action::None);
 // Native held torch permits a stow gesture; held sword/unknown left hand does not.
 i.tick+=20;i.trigger=0;update(s,i);i.tick+=20;i.empty=false;i.torch=true;i.trigger=.9;assert(update(s,i).consumesTrigger);
 i.tick+=20;i.torch=false;assert(update(s,i).action==Action::None);
 i.tick+=20;i.trigger=0;update(s,i);i.tick+=20;i.trigger=.9;assert(!update(s,i).consumesTrigger);
 // Lost tracking/context, frame gaps and non-finite poses quarantine until release.
 i.tick+=20;i.trigger=0;i.empty=true;update(s,i);i.tick+=20;i.trigger=.9;update(s,i);
 i.tick+=100;assert(update(s,i).action==Action::None);i.tick+=20;assert(update(s,i).consumesTrigger);
 i.tick+=20;i.trigger=0;update(s,i);i.tick+=20;i.trigger=.9;update(s,i);
 i.tick+=20;i.enabled=false;assert(update(s,i).consumesTrigger);i.tick+=20;i.enabled=true;assert(update(s,i).consumesTrigger);
 i.tick+=20;i.trigger=0;assert(!update(s,i).consumesTrigger);i.tick+=20;i.trigger=.9;i.up=std::numeric_limits<double>::quiet_NaN();assert(!update(s,i).consumesTrigger);
 assert(kVisorOpen54!=kVisorClose54&&kVisorPlayerVft54&&sizeof(kVisorOpenEntry54)==27&&sizeof(kVisorCloseEntry54)==22);
 // Actual Windows millisecond clock can repeat at90Hz. It must not reset
 // trigger ownership or manufacture elapsed time for the shoulder hold.
 {State repeated{};Input j{5000,true,true,true,false,-.16,.04,0,0};update(repeated,j);
  j.trigger=.9;assert(update(repeated,j).consumesTrigger);
  j.tick=5015;update(repeated,j);j.up=-.10;
  assert(update(repeated,j).action==Action::VisorClose);
  assert(update(repeated,j).action==Action::None);
  j.trigger=0;update(repeated,j);j.tick=5030;j.up=-.22;j.forward=-.23;j.right=-.30;j.trigger=.9;update(repeated,j);
  for(int n=0;n<9;++n){assert(update(repeated,j).action==Action::None);j.tick+=15;assert(update(repeated,j).action==Action::None);}
  j.tick=5180;assert(update(repeated,j).action==Action::Torch);
  assert(update(repeated,j).action==Action::None);
  // A genuinely backwards clock retains the original fail-closed policy.
  j.tick=5179;assert(update(repeated,j).action==Action::None);
  j.tick=5195;assert(update(repeated,j).action==Action::None);
 }
 // R114 torch stow: holding the torch, a press low behind the back stows it (old zone refused it).
 {State t{};Input j{9000,true,true,false,true,-.05,-.25,-.70,0};update(t,j);
  j.tick+=20;j.trigger=.9;assert(update(t,j).consumesTrigger);
  for(int n=0;n<7;++n){j.tick+=20;j.right+=.02;assert(update(t,j).action==Action::None);} // hand still travels
  j.tick+=20;assert(update(t,j).action==Action::Torch);
  // the same spot with an EMPTY hand never takes a torch (take zone stays small)
  j.tick+=20;j.trigger=0;update(t,j);j.empty=true;j.torch=false;j.right=-.05;
  j.tick+=20;j.trigger=.9;assert(!update(t,j).consumesTrigger);
  for(int n=0;n<10;++n){j.tick+=20;assert(update(t,j).action==Action::None);}
 }
 // R114: torch held at the hip / in front (block) never stows, also not with Grip on the torch.
 {State t{};Input j{12000,true,true,false,true,-.25,-.05,-.70,0};j.grip=1;update(t,j);
  j.tick+=20;j.trigger=.9;assert(!update(t,j).consumesTrigger);
  for(int n=0;n<20;++n){j.tick+=20;assert(update(t,j).action==Action::None);}
  j.tick+=20;j.trigger=0;update(t,j);j.forward=.20;j.up=-.30;
  j.tick+=20;j.trigger=.9;assert(!update(t,j).consumesTrigger);
  for(int n=0;n<20;++n){j.tick+=20;assert(update(t,j).action==Action::None);}
 }
 // R114: trigger squeezed a moment before the hand is behind the head still stows (<=350 ms), later never.
 {State t{};Input j{15000,true,true,false,true,-.30,.10,-.20,0};update(t,j);
  j.tick+=20;j.trigger=.9;assert(!update(t,j).consumesTrigger);
  for(int n=0;n<4;++n){j.tick+=20;assert(!update(t,j).consumesTrigger);} // block passes to the game meanwhile
  j.tick+=20;j.forward=-.10;assert(update(t,j).consumesTrigger);
  for(int n=0;n<7;++n){j.tick+=20;assert(update(t,j).action==Action::None);}
  j.tick+=20;assert(update(t,j).action==Action::Torch);
  j.tick+=20;j.trigger=0;j.forward=.10;update(t,j);
  j.tick+=20;j.trigger=.9;update(t,j);
  for(int n=0;n<20;++n){j.tick+=20;update(t,j);} // 400 ms in front
  j.forward=-.10;j.tick+=20;assert(!update(t,j).consumesTrigger);
  for(int n=0;n<20;++n){j.tick+=20;assert(update(t,j).action==Action::None);}
 }
 // R114: leaving the stow zone during the 150 ms hold cancels (no stow while swinging the torch forward).
 {State t{};Input j{20000,true,true,false,true,-.30,-.20,-.30,0};update(t,j);
  j.tick+=20;j.trigger=.9;assert(update(t,j).consumesTrigger);
  j.tick+=20;j.forward=.10;assert(update(t,j).action==Action::None);
  for(int n=0;n<10;++n){j.tick+=20;assert(update(t,j).action==Action::None);}
 }
}
