#include <windows.h>
#include <cassert>
#include <vector>
#include <cstdio>
#include <limits>
#include <string>
static std::vector<INPUT> sent175;
static std::vector<std::string> laserReceiptsM1;
static unsigned navigation175=0;
static bool suspended175=false;
static bool corrected36=false;
static UINT InputSpy175(UINT n,INPUT*p,int){sent175.insert(sent175.end(),p,p+n);return n;}
#define SendInput InputSpy175
#include "../src/ui_menu_input.cpp"
#include "../src/touch_bindings.cpp"
#include "../src/native_vr_pad_premap.cpp"
#undef SendInput
#include "../src/ui_menu_router.h"
namespace kcd2vr::diagnostic_trace {void Write(const char*s)noexcept{if(s&&std::string(s).rfind("M1 UI_LASER_INPUT ",0)==0)laserReceiptsM1.emplace_back(s);}}
namespace kcd2vr::flashui_probe {bool HudExperiment36Enabled() noexcept{return corrected36;}}
namespace kcd2vr::prompt_device_lock {
void SetSuspended(bool b)noexcept{suspended175=b;}
bool Suspended()noexcept{return suspended175;}
bool MixedCursorReady()noexcept{return true;}
void NotifyNavigation()noexcept{++navigation175;}
void WriteCursorSnapshot()noexcept{}
}
namespace kcd2vr::native_input_look_lab {bool QueueNativeAction168(const char*,int,float)noexcept{return false;}}
unsigned KeyDown175(unsigned scan){unsigned n=0;for(auto&e:sent175)
    if(e.type==INPUT_KEYBOARD&&e.ki.wScan==scan&&!(e.ki.dwFlags&KEYEVENTF_KEYUP))++n;return n;}
unsigned Mouse175(DWORD flags){unsigned n=0;for(auto&e:sent175)
    if(e.type==INPUT_MOUSE&&e.mi.dwFlags==flags)++n;return n;}
void ResetPointer175(){
    using namespace kcd2vr::ui_menu_input;
    kcd2vr::ui_menu_input::Frame f{};f.window=ui175Foreground;Update(f);
    sent175.clear();ui175Moves=0;navigation175=0;
}
int main(){
    using namespace kcd2vr;
    // C10: F5 requests use the selected device family, with no keyboard events
    // under XI. Test actual TouchBindings production methods, including cleanup.
    {
        TouchBindings f5;f5.SetEnabled(true);f5.SetNativeGamepadActions(true);
        sent175.clear();hostTickCount64+=1000;
        assert(f5.SetPhysicalInteractionHeld114(true));
        assert(f5.PhysicalInteractionGamepad59c10(hostTickCount64,true));
        assert(sent175.empty());
        f5.SetPhysicalInteractionHeld114(false);
        assert(!f5.PhysicalInteractionGamepad59c10(hostTickCount64,true));
        assert(f5.RequestPhysicalInteractionUse());
        assert(f5.PhysicalInteractionGamepad59c10(hostTickCount64+79,true));
        assert(!f5.PhysicalInteractionGamepad59c10(hostTickCount64+80,true));
        assert(sent175.empty());
        assert(f5.RequestPhysicalInteractionUse());
        assert(!f5.PhysicalInteractionGamepad59c10(hostTickCount64,false));
        assert(!f5.PhysicalInteractionGamepad59c10(hostTickCount64+1,true)); // no menu-to-world replay
        f5.SetPhysicalInteractionHeld114(true);f5.Reset();
        assert(!f5.PhysicalInteractionGamepad59c10(hostTickCount64,true));
        f5.SetPhysicalInteractionHeld114(true);f5.SetNativeGamepadActions(false);
        assert(!f5.PhysicalInteractionGamepad59c10(hostTickCount64,true));
        sent175.clear();assert(f5.SetPhysicalInteractionHeld114(true));assert(!sent175.empty());
        const auto events=sent175.size();
        f5.SetNativeGamepadActions(true);assert(sent175.size()==events+1); // release E exactly once
        assert(!f5.PhysicalInteractionGamepad59c10(hostTickCount64,true));
        sent175.clear();f5.SetPhysicalInteractionHeld114(true);f5.SetEnabled(false);
        assert(!f5.PhysicalInteractionGamepad59c10(hostTickCount64,true));assert(sent175.empty());
    }
    // A queued Quick Speak pulse cannot cross a menu/modifier owner or Reset.
    for(int boundary=0;boundary<3;++boundary){
        native_vr_pad_premap::Reset();native_vr_pad_premap::SetXQuickTalk58(true);
        ControllerState l{},r{};l.active=r.active=true;native_vr_pad_premap::Context c{};
        hostTickCount64+=1000;l.x_down=true;native_vr_pad_premap::Build(l,r,c);
        hostTickCount64+=20;l.x_down=false;auto p=native_vr_pad_premap::Build(l,r,c);assert(p.x);
        if(boundary==0)native_vr_pad_premap::Reset();
        if(boundary==1){c.system_menu=true;native_vr_pad_premap::Build(l,r,c);c.system_menu=false;}
        if(boundary==2){r.quick_access_touch=true;native_vr_pad_premap::Build(l,r,c);r.quick_access_touch=false;}
        p=native_vr_pad_premap::Build(l,r,c);assert(!p.x&&!p.dpad_down&&!p.dpad_up);
    }
    // Hold alone publishes one original-time native torch request, without an XI duplicate.
    {native_vr_pad_premap::Reset();native_vr_pad_premap::SetXQuickTalk58(true);
     ControllerState l{},r{};l.active=r.active=true;native_vr_pad_premap::Context c{};
     hostTickCount64+=1000;l.x_down=true;auto p=native_vr_pad_premap::Build(l,r,c);assert(!p.dpad_down);
     hostTickCount64+=500;p=native_vr_pad_premap::Build(l,r,c);assert(p.torchRequestTick62==hostTickCount64&&!p.dpad_down&&!p.x&&!p.dpad_up);
     hostTickCount64+=2000;p=native_vr_pad_premap::Build(l,r,c);assert(!p.torchRequestTick62&&!p.dpad_down&&!p.dpad_up&&!p.x);
     l.x_down=false;p=native_vr_pad_premap::Build(l,r,c);assert(!p.x&&!p.dpad_down);
    }
    // Actual production adapter, not a copied focus predicate: native scroll
    // on each axis and native selection must suppress hand-jitter cursor writes.
    for(unsigned axis=0;axis<8;++axis){
        ResetPointer175();ui_menu_input::Frame f{};f.window=ui175Foreground;
        f.menu=true;f.native_gamepad=true;f.context=100+axis;
        f.left.active=f.right.active=true;f.right.grip=1;f.ray_hand=1;f.ray_hit=true;
        ui_menu_input::Update(f);const auto before=ui175Moves;
        if(axis==0)f.left.thumb_x=.8f;if(axis==1)f.left.thumb_y=.8f;
        if(axis==2)f.right.thumb_x=.8f;if(axis==3)f.right.thumb_y=.8f;
        if(axis==4)f.right.a_down=true;if(axis==5)f.left.x_down=true;
        if(axis==6)f.right.thumbstick_down=true;
        if(axis==7){f.right.quick_access_touch=true;f.left.thumb_y=-1;}
        for(unsigned j=0;j<20;++j){f.u=.5005f+.0001f*float(j%3);ui_menu_input::Update(f);}
        if(ui175Moves!=before||navigation175!=1)std::fprintf(stderr,"axis=%u cursorBefore=%u after=%u navigation=%u\n",axis,before,ui175Moves,navigation175);
        assert(ui175Moves==before&&navigation175==1);
        f.left.thumb_x=f.left.thumb_y=f.right.thumb_x=f.right.thumb_y=0;
        f.right.a_down=f.left.x_down=f.right.thumbstick_down=f.right.quick_access_touch=false;
        ui_menu_input::Update(f);const auto quiet=ui175Moves;
        f.u=.65f;ui_menu_input::Update(f);assert(ui175Moves>quiet);
    }
    ResetPointer175();ui_menu_input::Frame f{};f.window=ui175Foreground;f.context=200;
    f.menu=f.native_gamepad=f.left.active=f.right.active=true;
    f.right.grip=1;f.ray_hand=1;f.ray_hit=true;ui_menu_input::Update(f);
    f.right.trigger=1;ui_menu_input::Update(f);assert(Mouse175(MOUSEEVENTF_LEFTDOWN)==1);
    f.ray_hand=-1;f.ray_hit=false;f.right.trigger=0;ui_menu_input::Update(f);
    assert(Mouse175(MOUSEEVENTF_LEFTUP)==1); // release after invalidated ray/drag
    f.right.trigger=1;f.ray_hand=1;f.ray_hit=true;ui_menu_input::Update(f);
    assert(Mouse175(MOUSEEVENTF_LEFTDOWN)==1); // no stale click after a miss
    f.right.trigger=0;ui_menu_input::Update(f);f.right.trigger=1;ui_menu_input::Update(f);
    assert(Mouse175(MOUSEEVENTF_LEFTDOWN)==2);
    f.right.trigger=0;f.menu=false;ui_menu_input::Update(f);
    assert(ui_menu_input::WorldGripBlockedMask()==2);
    f.right.grip=0;ui_menu_input::Update(f);assert(ui_menu_input::WorldGripBlockedMask()==0);

    // Frontend and ordinary death use pointer + real navigation. A confirmation
    // retains the proven parent even if that parent is temporarily native-hidden.
    using namespace ui_native_state;using Router=ui_menu_router::Router;
    for(auto root:{Menu,GameOver,Inventory,Shop}){
        Router router;Snapshot state{};state.known=state.visible=Bit(root);state.epoch=1;
        router.Resolve(state);assert(router.Read().GripPointerAllowed175());
        auto owner=router.Read().owner;
        state.known|=Bit(Modal);state.visible|=Bit(Modal);++state.epoch;router.Resolve(state);
        state.visible=Bit(Modal);++state.epoch;router.Resolve(state);
        assert(router.Read().owner==owner&&router.Read().GripPointerAllowed175());
        state.visible=0;++state.epoch;router.Resolve(state);
        assert(!router.Read().GripPointerAllowed175());
    }
    Router pause;Snapshot state{};state.known=Bit(Menu)|Bit(Hud);state.visible=Bit(Hud);
    pause.Resolve(state);state.visible|=Bit(Menu);++state.epoch;pause.Resolve(state);
    assert(pause.Read().owner==ui_menu_router::Owner::Pause&&!pause.Read().GripPointerAllowed175());

    for(bool xi:{false,true}){
        TouchBindings input;ControllerState l{},r{};l.active=r.active=true;
        input.SetNativeGamepadActions(xi);input.Update(l,r,false);sent175.clear();
        r.b_down=true;input.Update(l,r,false);r.b_down=false;input.Update(l,r,false);
        assert(KeyDown175('Q')==(xi?0u:1u));
        sent175.clear();r.b_down=true;input.Update(l,r,false,false,false,true);
        r.b_down=false;input.Update(l,r,false,false,false,true);assert(KeyDown175('Q')==0);
        input.SetNativeMinigameOwnership162r1(true);l.menu_down=true;input.Update(l,r,false);
        input.SetNativeMinigameOwnership162r1(false);input.Reset();sent175.clear();
        for(int j=0;j<8;++j)assert(!input.Update(l,r,false).toggle_inventory_comfort);
        l.menu_down=false;assert(!input.Update(l,r,false).toggle_inventory_comfort&&KeyDown175('I')==0);
        l.menu_down=true;input.Update(l,r,false);l.menu_down=false;
        assert(input.Update(l,r,false).toggle_inventory_comfort&&KeyDown175('I')==1);
    }
    // R58C2: actual caller boundary, both transports. Pause can close while
    // gameplay Update is intentionally skipped; held chord survives UI Reset.
    for(bool xi:{false,true}){
        TouchBindings input;ControllerState l{},r{};l.active=r.active=true;
        input.SetNativeGamepadActions(xi);input.UpdatePauseChord58c2(l,r,true);sent175.clear();
        l.menu_down=true;r.quick_access_touch=true;
        assert(input.UpdatePauseChord58c2(l,r,true)&&!l.menu_down&&KeyDown175(27)==1);
        input.Reset();l.menu_down=true;
        assert(!input.UpdatePauseChord58c2(l,r,true)&&!l.menu_down);
        input.Update(l,r,false);assert(KeyDown175('I')==0);
        r.quick_access_touch=false;l.menu_down=false;input.UpdatePauseChord58c2(l,r,true);input.Update(l,r,false);
        assert(KeyDown175('I')==0);
        l.menu_down=true;r.quick_access_touch=true;assert(input.UpdatePauseChord58c2(l,r,true)&&KeyDown175(27)==2);
    }
    // Genuine XI payload remains native in authored activity: B hold, LT
    // bellows, RT secondary, both stick axes, View Help and full D-pad.
    ControllerState l{},r{};l.active=r.active=true;
    native_vr_pad_premap::Context context{};context.semantic_minigame=true;
    native_vr_pad_premap::Reset();r.b_down=true;l.trigger=.7f;r.trigger=.8f;
    l.thumb_x=.8f;l.thumb_y=-.8f;r.thumb_x=-.9f;r.thumb_y=.9f;l.menu_down=true;
    for(unsigned j=0;j<30;++j){hostTickCount64+=30;auto p=native_vr_pad_premap::Build(l,r,context);
        assert(p.b&&!p.rb&&p.back&&p.lt==.7f&&p.rt==.8f&&p.lx>0&&p.ly<0&&p.rx<0&&p.ry>0);}
    r.quick_access_touch=true;l.thumb_x=0;l.thumb_y=-1;
    auto pad=native_vr_pad_premap::Build(l,r,context);assert(pad.dpad_down&&pad.lx==0&&pad.ly==0&&pad.b);
    r.b_down=false;pad=native_vr_pad_premap::Build(l,r,context);assert(!pad.b&&!pad.rb);
    r.quick_access_touch=false;l.thumb_x=l.thumb_y=r.thumb_x=r.thumb_y=0;
    r.b_down=true;pad=native_vr_pad_premap::Build(l,r,context);assert(pad.b);
    context.semantic_minigame=false;
    for(unsigned j=0;j<20;++j){pad=native_vr_pad_premap::Build(l,r,context);assert(!pad.b&&!pad.rb);}
    r.b_down=false;native_vr_pad_premap::Build(l,r,context);r.b_down=true;
    pad=native_vr_pad_premap::Build(l,r,context);assert(!pad.rb&&!pad.b); // C45 default: gameplay B is not a second Quick Speak
    native_vr_pad_premap::SetBQuickTalk45(true);pad=native_vr_pad_premap::Build(l,r,context);assert(pad.rb&&!pad.b);native_vr_pad_premap::SetBQuickTalk45(false); // fresh gameplay press after the exit hold (legacy VrBQuickTalk=1)
    // Normal production and the optional F2 experiment must use the same
    // windowed screen coordinates. The window or native cursor can move while
    // a held VR ray remains at exactly the same client UV.
    for(bool experiment36:{false,true}){
    ResetPointer175();corrected36=experiment36;
    ui175OriginX=60;ui175OriginY=70;ui175Width=1280;ui175Height=720;
    ui_menu_input::Frame windowed36{};windowed36.window=ui175Foreground;windowed36.context=700;
    windowed36.menu=windowed36.native_gamepad=true;windowed36.right.active=true;
    windowed36.right.grip=1;windowed36.ray_hand=1;windowed36.ray_hit=true;windowed36.u=windowed36.v=.5f;
    ui_menu_input::Update(windowed36);assert(ui175X==700&&ui175Y==430);
    const auto still36=ui175Moves;ui_menu_input::Update(windowed36);assert(ui175Moves==still36);
    ui175OriginX+=50;ui175OriginY+=30;ui_menu_input::Update(windowed36);assert(ui175X==750&&ui175Y==460);
    ui175X=0;ui175Y=0;ui_menu_input::Update(windowed36);assert(ui175X==750&&ui175Y==460);
    windowed36.right.trigger=1;ui_menu_input::Update(windowed36);assert(Mouse175(MOUSEEVENTF_LEFTDOWN)==1);
    windowed36.right.trigger=0;ui_menu_input::Update(windowed36);assert(Mouse175(MOUSEEVENTF_LEFTUP)==1);
    }
    // Sparse receipts are qualified by the caller's native Inventory/Shop
    // owner, capped at four state changes in one menu context, not a UV trace.
    ResetPointer175();laserReceiptsM1.clear();ui_menu_input::Frame receiptM1{};
    receiptM1.window=ui175Foreground;receiptM1.context=811;receiptM1.pointer_menu_kind=1;
    receiptM1.menu=receiptM1.native_gamepad=receiptM1.right.active=true;
    ui_menu_input::Update(receiptM1);assert(laserReceiptsM1.size()==1);
    for(unsigned i=0;i<1000;++i)ui_menu_input::Update(receiptM1);
    assert(laserReceiptsM1.size()==1);
    receiptM1.right.grip=1;receiptM1.ray_hand=1;ui_menu_input::Update(receiptM1);
    receiptM1.ray_hit=true;ui_menu_input::Update(receiptM1);
    receiptM1.right.quick_access_touch=true;ui_menu_input::Update(receiptM1);
    assert(laserReceiptsM1.size()==4);
    for(unsigned i=0;i<100;++i){receiptM1.right.quick_access_touch=!receiptM1.right.quick_access_touch;ui_menu_input::Update(receiptM1);}
    assert(laserReceiptsM1.size()==4);
    receiptM1.pointer_menu_kind=0;++receiptM1.context;ui_menu_input::Update(receiptM1);
    assert(laserReceiptsM1.size()==4); // diagnostic kind never invents another owner
    {native_vr_pad_premap::Reset();ControllerState ql{},qr{};ql.active=qr.active=true;
     native_vr_pad_premap::Context chat54{};chat54.back_context=true;chat54.quick_talk_open54=true;
     ql.x_down=true;auto q=native_vr_pad_premap::Build(ql,qr,chat54);assert(q.x&&!q.dpad_down&&!q.dpad_up);
     chat54.quick_talk_open54=false;q=native_vr_pad_premap::Build(ql,qr,chat54);assert(!q.x); // generic back context is not a chat choice
     chat54.system_menu=true;q=native_vr_pad_premap::Build(ql,qr,chat54);assert(q.x); // real physical X remains native menu navigation
    }
    std::puts("PASS production UI focus/input, plus windowed UV mapping, stationary ray, window movement and moved desktop cursor; no headset claim");
}
