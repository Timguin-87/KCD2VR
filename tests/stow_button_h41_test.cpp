#include "subsystems/throw_drop/stow_button_h41.h"
#include <cassert>
#include <cstdio>
struct Pad{bool a_down=false,a_pressed=false,b_down=false,b_pressed=false,x_down=false,x_pressed=false,y_down=false,y_pressed=false,thumbstick_down=false,thumbstick_pressed=false;float grip=.9f;};
int main(){using namespace kcd2vr::grip_stow_cb1;ButtonOwnerH41 state;Pad r,l,rr,ll;int requests[2]{};bool accept=true;
 auto step=[&](int button=0,bool allowed=true){rr=r;ll=l;RouteH41(state,r,l,rr,ll,button,allowed,[&](int side){++requests[side];return accept;});};
 step();r.a_down=r.a_pressed=true; l.y_down=l.y_pressed=true;step();assert(requests[0]==1&&requests[1]==1&&!rr.a_down&&!rr.a_pressed&&!ll.y_down&&!ll.y_pressed);
 r.a_pressed=l.y_pressed=false;step();assert(requests[0]==1&&requests[1]==1&&!rr.a_down&&!ll.y_down);
 // A held press remains consumed even after option changes; new B held on change is not a new request.
 r.b_down=true;step(1,false);assert(!rr.a_down&&rr.b_down&&requests[0]==1);step(1,true);assert(requests[0]==1);
 r.a_down=l.y_down=false; r.b_down=false;step(1);r.b_down=r.b_pressed=true;accept=false;step(1);assert(requests[0]==2&&rr.b_down&&rr.b_pressed);
 r.b_down=false;step(1);r.b_down=true;accept=true;step(1);assert(requests[0]==3&&!rr.b_down&&ll.x_down==l.x_down);
 r.b_down=false;step(2);l.thumbstick_down=l.thumbstick_pressed=true;step(2);assert(requests[1]==2&&!ll.thumbstick_down&&!ll.thumbstick_pressed);
 l.thumbstick_down=false;step(2);r.thumbstick_down=true;step(2,false);assert(requests[0]==3);step(2,true);assert(requests[0]==3);
 std::puts("H41 actual stow input routing PASS: accepted chords consumed through release, no repeat, configuration/UI rearm, refused press preserved, independent sides.");
}
