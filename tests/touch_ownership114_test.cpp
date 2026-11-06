// Exercise the real input transport against a spy, never Windows SendInput.
#include <windows.h>
#include <vector>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#undef assert
#define assert(condition) do {if(!(condition)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#condition);std::exit(1);}} while(false)
static std::vector<INPUT> events114;
static UINT SendInput114(UINT n,INPUT* events,int){events114.insert(events114.end(),events,events+n);return n;}
#define SendInput SendInput114
#include "../src/touch_bindings.cpp"
namespace kcd2vr::diagnostic_trace { void Write(const char*) noexcept {} } // V66.178R21: R3_EDGE trace
#undef SendInput
static unsigned edges(bool down){unsigned n=0;for(const auto& e:events114)
    if(e.type==INPUT_KEYBOARD&&e.ki.wScan=='E'&&bool(e.ki.dwFlags&KEYEVENTF_KEYUP)!=down)++n;return n;}
static unsigned specials162(){unsigned n=0;for(const auto& e:events114)
    if(e.type==INPUT_KEYBOARD&&e.ki.wScan=='F'&&!(e.ki.dwFlags&KEYEVENTF_KEYUP))++n;return n;}
static unsigned keyM1(WORD key,bool down){unsigned n=0;for(const auto& e:events114)
    if(e.type==INPUT_KEYBOARD&&e.ki.wScan==key&&bool(e.ki.dwFlags&KEYEVENTF_KEYUP)!=down)++n;return n;}
static void back_escape_testsM1(){
    using namespace kcd2vr;
    for(bool xi:{false,true}){
        TouchBindings b;ControllerState l{},r{};l.active=r.active=r.b_valid=true;r.grip=.8f;
        b.SetNativeGamepadActions(xi);events114.clear();
        // Neither unknown menu classification nor XI readiness gates the gesture.
        hostTickCount64=1000;r.b_down=true;assert(!b.UpdateBackEscapeM1(r,true,1));
        for(unsigned t=1100;t<=3200;t+=100){hostTickCount64=t;assert(!b.UpdateBackEscapeM1(r,true,1));}
        assert(events114.empty()); // startup/reconnected held button needs neutral
        hostTickCount64=3300;r.b_down=false;assert(!b.UpdateBackEscapeM1(r,true,1));
        hostTickCount64=3400;r.b_down=true;assert(!b.UpdateBackEscapeM1(r,true,1));
        for(unsigned t=3500;t<5400;t+=100){hostTickCount64=t;assert(!b.UpdateBackEscapeM1(r,true,1));}
        hostTickCount64=5399;assert(!b.UpdateBackEscapeM1(r,true,1));
        hostTickCount64=5400;assert(b.UpdateBackEscapeM1(r,true,1));
        assert(keyM1(VK_ESCAPE,true)==1&&keyM1(VK_ESCAPE,false)==1&&r.b_down&&r.grip==.8f&&b.BackEscapeConsumedM1());
        b.Reset();b.SetNativeGamepadActions(!xi); // context/transport reset never rearms same hold
        for(unsigned t=5500;t<=6400;t+=100){hostTickCount64=t;assert(!b.UpdateBackEscapeM1(r,true,1));}
        assert(keyM1(VK_ESCAPE,true)==1&&keyM1(VK_ESCAPE,false)==1);
        hostTickCount64=6500;r.b_down=false;assert(!b.UpdateBackEscapeM1(r,true,1)&&b.BackEscapeConsumedM1());
        b.Update(l,r,false); // release after recovery must not add fallback Quick Speak
        assert(keyM1('Q',true)==0&&keyM1(VK_ESCAPE,true)==1);
        hostTickCount64=6600;r.b_down=true;assert(!b.UpdateBackEscapeM1(r,true,1)&&!b.BackEscapeConsumedM1());
        for(unsigned t=6700;t<8600;t+=100){hostTickCount64=t;assert(!b.UpdateBackEscapeM1(r,true,1));}
        hostTickCount64=8600;assert(b.UpdateBackEscapeM1(r,true,1));
        assert(keyM1(VK_ESCAPE,true)==2&&keyM1(VK_ESCAPE,false)==2);
    }
    // A short B remains an ordinary tap; this service adds no key transaction.
    {TouchBindings b;ControllerState r{};r.active=r.b_valid=true;events114.clear();
     hostTickCount64=1000;b.UpdateBackEscapeM1(r,true,1);r.b_down=true;hostTickCount64=1100;
     assert(!b.UpdateBackEscapeM1(r,true,1));r.b_down=false;hostTickCount64=1200;
     assert(!b.UpdateBackEscapeM1(r,true,1)&&events114.empty());}
    for(int loss=0;loss<5;++loss){
        TouchBindings b;ControllerState r{};r.active=r.b_valid=true;events114.clear();
        hostTickCount64=1000;b.UpdateBackEscapeM1(r,true,1);r.b_down=true;
        hostTickCount64=1100;b.UpdateBackEscapeM1(r,true,1);
        if(loss==1)r.active=false;if(loss==2)r.b_valid=false;if(loss==3)b.SetEnabled(false);
        hostTickCount64=1200;assert(!b.UpdateBackEscapeM1(r,loss!=0&&loss!=4,1));
        r.active=r.b_valid=true;b.SetEnabled(true);
        for(unsigned t=1300;t<=3600;t+=100){hostTickCount64=t;assert(!b.UpdateBackEscapeM1(r,true,1));}
        assert(events114.empty()); // focus/matrix/tracking/action-read/enable loss cannot reconnect-fire
        r.b_down=false;hostTickCount64=3700;b.UpdateBackEscapeM1(r,true,1);
        r.b_down=true;hostTickCount64=3800;b.UpdateBackEscapeM1(r,true,1);
        for(unsigned t=3900;t<5800;t+=100){hostTickCount64=t;assert(!b.UpdateBackEscapeM1(r,true,1));}
        hostTickCount64=5800;assert(b.UpdateBackEscapeM1(r,true,1));
        assert(keyM1(VK_ESCAPE,true)==1&&keyM1(VK_ESCAPE,false)==1);
    }
    // A source callback discontinuity cannot retrospectively count as held time.
    {TouchBindings b;ControllerState r{};r.active=r.b_valid=true;events114.clear();
     hostTickCount64=1000;b.UpdateBackEscapeM1(r,true,1);r.b_down=true;hostTickCount64=1100;b.UpdateBackEscapeM1(r,true,1);
     hostTickCount64=1351;assert(!b.UpdateBackEscapeM1(r,true,1));
     for(unsigned t=1400;t<=3600;t+=100){hostTickCount64=t;assert(!b.UpdateBackEscapeM1(r,true,1));}
     assert(events114.empty());}
    {TouchBindings b;ControllerState r{};r.active=r.b_valid=true;events114.clear();
     hostTickCount64=1000;b.UpdateBackEscapeM1(r,true,1);r.b_down=true;
     hostTickCount64=1100;b.UpdateBackEscapeM1(r,true,1);
     for(unsigned t=1200;t<=3300;t+=100){hostTickCount64=t;assert(!b.UpdateBackEscapeM1(r,true,2));}
     assert(events114.empty()); // recenter/current input generation change is not a continuation
     r.b_down=false;hostTickCount64=3400;b.UpdateBackEscapeM1(r,true,2);
     r.b_down=true;hostTickCount64=3500;b.UpdateBackEscapeM1(r,true,2);
     for(unsigned t=3600;t<5500;t+=100){hostTickCount64=t;assert(!b.UpdateBackEscapeM1(r,true,2));}
     hostTickCount64=5500;assert(b.UpdateBackEscapeM1(r,true,2));
     assert(keyM1(VK_ESCAPE,true)==1&&keyM1(VK_ESCAPE,false)==1);}
    // Existing fallback Back may already own Escape. Recovery ends only that
    // owned hold, delivers one pair, and never immediately re-holds Escape.
    {TouchBindings b;ControllerState l{},r{};l.active=r.active=r.b_valid=true;events114.clear();
     hostTickCount64=1000;b.UpdateBackEscapeM1(r,true,1);r.b_down=true;
     hostTickCount64=1100;b.UpdateBackEscapeM1(r,true,1);b.Update(l,r,false,false,false,true);
     assert(keyM1(VK_ESCAPE,true)==1&&keyM1(VK_ESCAPE,false)==0);
     for(unsigned t=1200;t<3100;t+=100){hostTickCount64=t;assert(!b.UpdateBackEscapeM1(r,true,1));}
     hostTickCount64=3100;assert(b.UpdateBackEscapeM1(r,true,1));b.Update(l,r,false,false,false,true);
     assert(keyM1(VK_ESCAPE,true)==2&&keyM1(VK_ESCAPE,false)==2);
     r.b_down=false;hostTickCount64=3200;b.UpdateBackEscapeM1(r,true,1);b.Update(l,r,false,false,false,true);
     assert(keyM1(VK_ESCAPE,true)==2&&keyM1(VK_ESCAPE,false)==2&&keyM1('Q',true)==0);}
    std::puts("M1 actual TouchBindings B-hold recovery: 2s once, paired Escape, XI/tap preserved, loss/reset quarantined; OS NOT_EXECUTED");
}
static void r3_tests162(){
    using namespace kcd2vr;
    for(bool xi:{false,true})for(float grip:{0.f,1.f}){
        TouchBindings b;ControllerState l{},r{};l.active=r.active=true;
        b.SetNativeGamepadActions(xi);b.SetRightGripFreeLookEnabled(false);
        r.grip=grip;events114.clear();b.Update(l,r,false);
        r.thumbstick_down=r.thumbstick_pressed=true;
        assert(b.Update(l,r,false).special_attack_requested&&specials162()==1);
        // Duplicate press metadata and XI ownership changes while held cannot repeat it.
        b.SetNativeGamepadActions(!xi);b.Update(l,r,false);assert(specials162()==1);
        r.thumbstick_pressed=false;b.Update(l,r,false);assert(specials162()==1);
        r.thumbstick_down=false;b.Update(l,r,false);
        r.thumbstick_down=r.thumbstick_pressed=true;b.Update(l,r,false);assert(specials162()==2);
    }
    // Each excluded context consumes the edge; leaving it held never emits a delayed attack.
    for(int context=0;context<9;++context){
        TouchBindings b;ControllerState l{},r{};l.active=r.active=true;
        r.thumbstick_down=r.thumbstick_pressed=true;
        if(context==3)b.SetAuthoredMinigameContext(true,false);
        if(context==4)l.thumbstick_down=true; // panic chord remains reserved
        if(context==5){r.quick_access_touch=true;l.thumb_x=1;}
        if(context==6)r.active=false;
        if(context==8)b.SetEnabled(false);
        events114.clear();
        auto e=b.Update(l,r,context==0,context==1,context==2,context==7);
        // V66.178R21: a back-context map enabled during combat no longer swallows the click (context 7).
        if(context==7){assert(e.special_attack_requested&&specials162()==1);events114.clear();}
        else assert(!e.special_attack_requested&&specials162()==0);
        b.SetAuthoredMinigameContext(false,false);b.SetEnabled(true);
        l.thumbstick_down=false;l.thumb_x=0;r.quick_access_touch=false;r.active=true;
        r.thumbstick_pressed=false;b.Update(l,r,false);assert(specials162()==0);
    } 
    // V168 production route queues one semantic request and never sends OS F.
    for(bool xi:{false,true})for(bool activity:{false,true}){
        TouchBindings b;ControllerState l{},r{};l.active=r.active=true;r.grip=1;
        b.SetNativeGamepadActions(xi);b.SetNativeSpecialRoute168(true);
        b.SetNativeMinigameOwnership162r1(activity);b.SetRightGripFreeLookEnabled(false);
        events114.clear();b.Update(l,r,false);r.thumbstick_down=r.thumbstick_pressed=true;
        auto e=b.Update(l,r,false);assert(e.special_attack_requested==!activity&&specials162()==0);
        e=b.Update(l,r,false);assert(!e.special_attack_requested&&specials162()==0);
        b.SetNativeMinigameOwnership162r1(false);r.thumbstick_pressed=false;
        e=b.Update(l,r,false);assert(!e.special_attack_requested&&specials162()==0);
    }

}
int main(){
    back_escape_testsM1();r3_tests162();events114.clear();
    kcd2vr::TouchBindings bindings;kcd2vr::ControllerState left{},right{};
    left.active=right.active=true;
    assert(bindings.SetPhysicalInteractionHeld114(true));
    bindings.Update(left,right,false);
    bindings.Update(left,right,false);
    assert(edges(true)==1&&edges(false)==0); // normal Update may not release the physical owner
    right.a_down=true;bindings.Update(left,right,false);
    bindings.SetPhysicalInteractionHeld114(false);
    assert(edges(false)==0); // manual A now owns the same held key
    right.a_down=false;bindings.Update(left,right,false);
    assert(edges(false)==1);
    bindings.SetPhysicalInteractionHeld114(true);
    bindings.Update(left,right,true);
    assert(edges(false)==2); // menu entry releases both owners
    bindings.SetPhysicalInteractionHeld114(true);bindings.SetEnabled(false);
    assert(edges(false)==3&&!bindings.SetPhysicalInteractionHeld114(true));
    bindings.SetEnabled(true);left={};right={};left.active=right.active=true;
    bindings.SetRightGripFreeLookEnabled(true);bindings.SetGripWeaponComparison114(true);
    right.grip=1;right.thumb_y=.8f;bindings.Update(left,right,false);
    events114.clear();hostTickCount64+=20;bindings.Update(left,right,false);
    bool pitch=false;for(const auto& e:events114)if(e.type==INPUT_MOUSE&&e.mi.dy) pitch=true;
    assert(pitch); // F12 selection must not suppress explicit non-VRIK free look.
    bindings.SetRightGripFreeLookEnabled(false);events114.clear();hostTickCount64+=20;
    bindings.Update(left,right,false);pitch=false;
    for(const auto& e:events114)if(e.type==INPUT_MOUSE&&e.mi.dy) pitch=true;
    assert(!pitch); // VRIK owns Grip; no mouse pitch.
    // The F5 pickup pulse releases native XI-A while physical Grip stays down.
    bindings.SetNativeGamepadActions(true);right.grip=1;hostTickCount64=10000;
    bindings.SetPhysicalInteractionHeld114(false);assert(bindings.RequestPhysicalInteractionUse());
    assert(bindings.PhysicalInteractionGamepad59c10(10000,true));
    assert(bindings.PhysicalInteractionGamepad59c10(10079,true));
    assert(!bindings.PhysicalInteractionGamepad59c10(10080,true));
    assert(right.grip==1); // no physical Grip release was needed
    hostTickCount64=10100;assert(bindings.RequestPhysicalInteractionUse());
    assert(!bindings.PhysicalInteractionGamepad59c10(10101,false));
    assert(!bindings.PhysicalInteractionGamepad59c10(10102,true)); // context loss clears, no delayed pulse
    std::puts("V114 real TouchBindings ownership spy passed; OS input NOT executed");
}
