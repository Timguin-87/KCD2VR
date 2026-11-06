#include "ui_menu_input.h"
#include "ui_hud_experiment36c5.h"
#include "flashui_probe.h"
#include "ui_back_hold135r2.h"
#include "ui_menu_policy.h"
#include "diagnostic_trace.h"
#include "diagnostics/capture168.h"
#include "prompt_device_lock.h"
#include <DetourModKit/hook.hpp>
#include <intrin.h>
#include <optional>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>

bool KCD2VR_PresentedImageSizeR16(std::uint64_t* width,std::uint32_t* height) noexcept; // device_hook.cpp
namespace kcd2vr::ui_menu_input { namespace {
using namespace ui_menu_policy;
// R16b WINDOWED LASER (review of R16: it stayed active after the laser stopped, X and Y were two separate atomics that could
// tear, and it overrode GetCursorPos for the whole process). now:
//  * X and Y packed into ONE 64-bit atomic (one store, one load) - a reader always gets a matching pair
//  * explicitly cleared on EVERY pointer exit (no hit, pointer off, native focus, reset) plus a 150 ms expiry as a backstop
//  * only calls made FROM WHGame.dll get the virtual point; KCD2VR, overlays, Windows code read the real cursor
//  * VrWindowedLaserImageSpace (KCD2VR.ini, default true) - false = the old client mapping, a one-line A/B for verifying
//    that the game's menus really read render-image coordinates in windowed mode
using GetCursorPosFnR16=BOOL(WINAPI*)(LPPOINT);
std::optional<DetourModKit::hook::Hook> g_cursor_pos_hook_r16;
GetCursorPosFnR16 g_get_cursor_pos_r16=nullptr;
std::atomic<bool> g_vc_tried_r16{false},g_vc_mode_r16{true};
std::atomic<std::uint64_t> g_vc_xy_r16{0};    // x (low 32) and y (high 32) of ONE point, written in one store
std::atomic<std::uint64_t> g_vc_until_r16{0}; // 0 = off; else GetTickCount64 expiry
std::uintptr_t g_whgame_lo_r16=0,g_whgame_hi_r16=0;
void ClearVirtualCursorR16() noexcept {g_vc_until_r16.store(0,std::memory_order_release);}
void SetVirtualCursorR16(long x,long y) noexcept {
    g_vc_xy_r16.store((std::uint64_t(std::uint32_t(y))<<32)|std::uint64_t(std::uint32_t(x)),std::memory_order_release);
    g_vc_until_r16.store(GetTickCount64()+150,std::memory_order_release);}
BOOL WINAPI GetCursorPosDetourR16(LPPOINT p) {
    const auto until=g_vc_until_r16.load(std::memory_order_acquire);
    const auto ret=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    if(p&&until&&GetTickCount64()<until&&ret>=g_whgame_lo_r16&&ret<g_whgame_hi_r16){
        const auto xy=g_vc_xy_r16.load(std::memory_order_acquire); // one load = a matching X/Y pair
        p->x=LONG(std::int32_t(xy&0xFFFFFFFFu));p->y=LONG(std::int32_t(xy>>32));
        if(kcd2vr::diagnostics::ownership48::active(GetTickCount64())){
            static thread_local std::uint64_t last48=0;const auto now48=std::uint64_t(GetTickCount64());
            if(!last48||now48<last48||now48-last48>=100){last48=now48;kcd2vr::diagnostics::ownership48::Event e{};e.stage=8;e.tick=now48;e.thread=GetCurrentThreadId();e.code=2;e.object=ret-g_whgame_lo_r16;e.v[0]=float(p->x);e.v[1]=float(p->y);kcd2vr::diagnostics::ownership48::name(e,"WHGAME_VIRTUAL_CURSOR_READER_RVA_SCREEN_XY");(void)kcd2vr::diagnostics::ownership48::push(e);}
        }
        return TRUE;
    }
    return g_get_cursor_pos_r16?g_get_cursor_pos_r16(p):FALSE;
}
bool RealCursorPosR16(POINT& p) noexcept {return g_get_cursor_pos_r16?g_get_cursor_pos_r16(&p)!=0:GetCursorPos(&p)!=0;}
bool EnsureVirtualCursorR16() noexcept {
    if(g_get_cursor_pos_r16)return true;
    if(g_vc_tried_r16.exchange(true,std::memory_order_acq_rel))return false;
    const auto game=reinterpret_cast<const unsigned char*>(GetModuleHandleW(L"WHGame.dll"));std::uint32_t imageSize=0;
    if(game&&reinterpret_cast<const IMAGE_DOS_HEADER*>(game)->e_magic==IMAGE_DOS_SIGNATURE){
        const auto nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(game+reinterpret_cast<const IMAGE_DOS_HEADER*>(game)->e_lfanew);
        if(nt->Signature==IMAGE_NT_SIGNATURE)imageSize=nt->OptionalHeader.SizeOfImage;}
    if(!game||!imageSize){kcd2vr::diagnostic_trace::Write("R16b VIRTUAL_CURSOR unavailable: WHGame range unknown - old windowed mapping");return false;}
    g_whgame_lo_r16=reinterpret_cast<std::uintptr_t>(game);g_whgame_hi_r16=g_whgame_lo_r16+imageSize;
    const auto user=GetModuleHandleW(L"user32.dll");
    const auto address=user?reinterpret_cast<std::uintptr_t>(GetProcAddress(user,"GetCursorPos")):0;
    if(!address){kcd2vr::diagnostic_trace::Write("R16b VIRTUAL_CURSOR unavailable: GetCursorPos not found - old windowed mapping");return false;}
    auto hook=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{"r16b_get_cursor_pos",DetourModKit::hook::Target{DetourModKit::Address{address}},DetourModKit::hook::Options{}},&GetCursorPosDetourR16);
    if(!hook){kcd2vr::diagnostic_trace::Write("R16b VIRTUAL_CURSOR unavailable: hook create failed - old windowed mapping");return false;}
    g_cursor_pos_hook_r16=std::move(*hook);g_get_cursor_pos_r16=g_cursor_pos_hook_r16->original<GetCursorPosFnR16>();
    if(!g_get_cursor_pos_r16||!g_cursor_pos_hook_r16->enable()){g_get_cursor_pos_r16=nullptr;g_cursor_pos_hook_r16.reset();
        kcd2vr::diagnostic_trace::Write("R16b VIRTUAL_CURSOR unavailable: hook enable failed - old windowed mapping");return false;}
    kcd2vr::diagnostic_trace::Write("R16b VIRTUAL_CURSOR ready: WHGame cursor reads follow the vr laser in image pixels (windowed, image != client)");return true;
}
bool PresentedImageR16(long& w,long& h) noexcept {
    std::uint64_t width=0;std::uint32_t height=0;
    if(!KCD2VR_PresentedImageSizeR16(&width,&height)||width<16||height<16||width>32768||height>32768)return false;
    w=long(width);h=long(height);return true;
}
OwnerState g_owner;GripRearm g_grip_rearm;
std::atomic<int> g_hand{-1};std::atomic<std::uint8_t> g_grips{0};
bool g_installed=false,g_captured=false,g_neutral_exit=false;
bool g_keys[256]{};bool g_mouse=false,g_pointer_armed=false,g_pixel=false;
bool g_menu_prev=false,g_b_prev=false,g_b_eligible=false,g_b_long=false;
bool g_native_pad=false;
ui_back_hold135r2::Hold g_back_hold;
bool g_navigation_was=false;
bool g_focus_owner=false,g_focus_uv=false;float g_focus_u=.5f,g_focus_v=.5f;
int g_x=0,g_y=0,g_last_owner=-1;std::uint64_t g_b_since=0,g_pulse_until=0,g_wheel_at=0;
unsigned g_pulse_key=0;std::uint64_t g_errors=0,g_keys_sent=0,g_mouse_moves=0,g_context=0;
std::uint64_t g_laser_receipt_context=~std::uint64_t{0};
std::uint32_t g_laser_receipt_state=~std::uint32_t{0};unsigned g_laser_receipts=0;

bool Key(unsigned vk,bool down,bool allowNew=true) noexcept {
    if(vk>=256)return false;
    if(g_keys[vk]==down||(!allowNew&&down))return true;
    INPUT e{};e.type=INPUT_KEYBOARD;
    // The donor's physical scan-code execution is retained, but arrow keys must
    // carry E0. Navigation uses the authored W/A/S/D alternatives to avoid the
    // donor's unextended arrow -> numpad ambiguity and the new virtual-key path.
    const UINT code=MapVirtualKeyW(vk,MAPVK_VK_TO_VSC);
    if(!code){++g_errors;return false;}
    e.ki.wScan=static_cast<WORD>(code&0xffu);
    e.ki.dwFlags=KEYEVENTF_SCANCODE|(down?0:KEYEVENTF_KEYUP);
    if(vk==VK_UP||vk==VK_DOWN||vk==VK_LEFT||vk==VK_RIGHT||vk==VK_DELETE||vk==VK_INSERT||vk==VK_HOME||vk==VK_END||vk==VK_PRIOR||vk==VK_NEXT)
        e.ki.dwFlags|=KEYEVENTF_EXTENDEDKEY;
    if(SendInput(1,&e,sizeof(e))!=1){++g_errors;return false;}
    g_keys[vk]=down;++g_keys_sent;return true;
}
void ReleaseKeys() noexcept {g_back_hold.Reset();for(unsigned i=0;i<256;++i)if(g_keys[i])Key(i,false);g_pulse_key=0;g_pulse_until=0;}
void Mouse(bool down) noexcept {
    if(down==g_mouse)return;INPUT e{};e.type=INPUT_MOUSE;e.mi.dwFlags=down?MOUSEEVENTF_LEFTDOWN:MOUSEEVENTF_LEFTUP;
    const bool sent48=SendInput(1,&e,sizeof(e))==1;if(sent48)g_mouse=down;else ++g_errors;
    const auto now48=std::uint64_t(GetTickCount64());if(kcd2vr::diagnostics::ownership48::active(now48)){
        kcd2vr::diagnostics::ownership48::Event trace{};trace.stage=8;trace.tick=now48;trace.thread=GetCurrentThreadId();trace.code=3;trace.item=g_context;trace.flags=(down?1u:0u)|(sent48?2u:0u);trace.v[0]=float(GetLastError());
        kcd2vr::diagnostics::ownership48::name(trace,"OS_MOUSE_BUTTON_SUBMIT_DOWN_SUCCESS_LASTERROR");(void)kcd2vr::diagnostics::ownership48::push(trace);
    }
}
void ResetPointer() noexcept {ClearVirtualCursorR16();Mouse(false);g_pointer_armed=false;g_pixel=false;g_focus_owner=false;g_focus_uv=false;}
void Pulse(unsigned vk,std::uint64_t now) noexcept {
    if(g_pulse_key)Key(g_pulse_key,false);
    g_pulse_key=vk;g_pulse_until=now+80;Key(vk,true);
}
ui_menu_policy::Frame ToPolicy(const Frame&f) noexcept {
    ui_menu_policy::Frame p{};p.menu=f.menu&&!f.block_gameplay;p.pointer_allowed=f.allow_pointer&&!f.passive_back_only;
    p.foreground=f.window&&GetForegroundWindow()==f.window;p.context=f.context;
    ControllerState empty{};const auto&l=f.left.active||f.left.aim_active?f.left:empty;
    const auto&r=f.right.active||f.right.aim_active?f.right:empty;
    p.valid=l.active||l.aim_active||r.active||r.aim_active;
    p.hands={Hand{l.active||l.aim_active,l.grip,l.trigger},Hand{r.active||r.aim_active,r.grip,r.trigger}};
    // Native thumbrest+Grip is LB/RB. Reserve that chord from mouse ownership
    // while GripRearm below still sees the actual held grips on menu exit.
    if(f.native_gamepad&&r.quick_access_touch){p.pointer_allowed=false;p.hands[0].grip=p.hands[1].grip=0.f;}
    p.axes={l.thumb_x,l.thumb_y,r.thumb_x,r.thumb_y};
    if(r.a_down)p.buttons|=0x1000;if(r.b_down)p.buttons|=0x2000;
    if(l.x_down)p.buttons|=0x4000;if(l.y_down)p.buttons|=0x8000;
    if(l.menu_down)p.buttons|=0x10000; // Inventory toggle is NOT Back
    if(l.thumbstick_down)p.buttons|=0x40;if(r.thumbstick_down)p.buttons|=0x80;
    if(f.passive_back_only&&p.menu){p.buttons&=0x2000u;p.axes={};p.hands[0].trigger=p.hands[1].trigger=0;}
    return p;
}
void Keyboard(const Frame&f,const ui_menu_policy::Frame&p,bool acceptNew,std::uint64_t now) noexcept {
    const bool passive=f.passive_back_only;
    if(passive){Key(VK_SPACE,(p.buttons&0x2000)!=0,acceptNew);return;}
    Key('W',p.axes[1]>.55f,acceptNew);Key('S',p.axes[1]<-.55f,acceptNew);
    // Same primary_action superaction in menu, selection and transfer maps.
    Key('E',(p.buttons&0x1000)!=0,acceptNew);
    Key('X',(p.buttons&0x4000)!=0,acceptNew);
    Key('Q',!f.system_menu&&(p.buttons&0x8000)!=0,acceptNew);
    // Fallback-only scan actions. In V66.96 native XI owns these controller-only
    // semantics directly; no defaultProfile companion is shipped or required.
    Key(VK_F15,f.system_menu&&(p.buttons&0x8000)!=0,acceptNew);
    const bool sort=!f.system_menu&&(p.buttons&0x80)!=0;
    const bool prev=p.axes[2]<-.55f,next=p.axes[2]>.55f;
    Key(VK_F13,!sort&&!f.system_menu&&prev,acceptNew);
    Key(VK_TAB,!sort&&!f.system_menu&&next,acceptNew);
    Key(VK_END,sort&&prev,acceptNew);Key(VK_INSERT,sort&&next,acceptNew);
    Key(VK_F14,!f.system_menu&&(p.buttons&0x40)!=0,acceptNew);
    // On native settings/title pages both sticks may navigate; there is no
    // APSE tab namespace here. A/D are the same authored navigation superactions.
    Key('A',p.axes[0]<-.55f||(f.system_menu&&prev),acceptNew);
    Key('D',p.axes[0]>.55f||(f.system_menu&&next),acceptNew);
    const bool b=(p.buttons&0x2000)!=0;
    if(b&&!g_b_prev){g_b_since=now;g_b_long=false;g_b_eligible=acceptNew;}
    const bool back=g_back_hold.Update(b,g_b_eligible,f.context,now);
    Key(VK_ESCAPE,back||(g_pulse_key==VK_ESCAPE&&now<g_pulse_until),acceptNew||g_b_eligible);
    if(!b){g_b_since=0;g_b_long=false;g_b_eligible=false;}
    g_b_prev=b;
    const bool menu=(p.buttons&0x10000)!=0;
    if(menu&&!g_menu_prev&&acceptNew)Pulse(f.inventory_root&&!f.modal?f.inventory_key:VK_ESCAPE,now);
    g_menu_prev=menu;
}
} // namespace

bool Install() noexcept {
    if(g_installed)return true;g_installed=true;
    diagnostic_trace::Write("V66.96-UI-INPUT native XI preferred; scan-code focus is fallback only; mixed cursor lifecycle is independent of controller glyph family");return true;
}
void Uninstall() noexcept {ClearVirtualCursorR16();if(g_cursor_pos_hook_r16){if(!g_cursor_pos_hook_r16->is_enabled()||g_cursor_pos_hook_r16->disable()){g_cursor_pos_hook_r16.reset();g_get_cursor_pos_r16=nullptr;g_vc_tried_r16.store(false);}}ReleaseKeys();ResetPointer();g_hand.store(-1);g_installed=false;prompt_device_lock::SetSuspended(false);}
Result Update(const Frame&f) noexcept {
    Result out{};const auto now=GetTickCount64();const auto p=ToPolicy(f);
    const bool captured=p.menu||f.block_gameplay;
    const bool gripsConsumed=captured||f.consume_grips;
    const std::array<Hand,2> hands{{{f.left.active||f.left.aim_active,f.left.grip,f.left.trigger},{f.right.active||f.right.aim_active,f.right.grip,f.right.trigger}}};
    out.grip_rearm_mask=g_grip_rearm.Update(gripsConsumed,hands);g_grips.store(gripsConsumed?3:out.grip_rearm_mask);
    if(captured!=g_captured){ReleaseKeys();ResetPointer();g_neutral_exit=true;g_b_eligible=false;g_b_long=false;g_captured=captured;}
    const auto d=g_owner.Update(p);
    const bool blocked=d.owner==Owner::Blocked||!captured||f.block_gameplay;
    if(blocked){ReleaseKeys();ResetPointer();g_b_prev=(p.buttons&0x2000)!=0;g_menu_prev=(p.buttons&0x10000)!=0;
        g_b_eligible=false;g_b_long=false;}
    const bool pointer=!blocked&&d.hand>=0;
    if(d.changed){g_pointer_armed=false;g_pixel=false;g_focus_owner=false;g_focus_uv=false;g_navigation_was=false;}
    // With the complete balanced-cursor provider the prompt family may remain
    // controller while real keyboard/mouse actions execute. Only a failed provider
    // falls back to native KBM classification for the HELD pointer, never all menus.
    const bool mixed_cursor_ready=prompt_device_lock::MixedCursorReady();
    // V66.96b: the tested scan-code fallback must NOT disable the controller
    // presentation lock when the balanced native cursor provider is available.
    // Keyboard navigation and the cursor are execution mechanisms; prompt family
    // remains independent. Only a held pointer with no mixed-cursor bridge needs
    // to expose KB/M classification.
    // Frontend uses native mouse classification while its ray owns clicks. Cursor-provider readiness alone does not prove controller-family mouse activation.
    prompt_device_lock::SetSuspended(pointer&&(!mixed_cursor_ready||f.pointer_menu_kind==3));
    g_hand.store(pointer?d.hand:-1);out.hand=pointer?d.hand:-1;
    const bool acceptNew=!blocked&&(d.nav||pointer);
    if(kcd2vr::diagnostics::ownership48::active(now)){
        static std::uint64_t last48=0;if(!last48||now<last48||now-last48>=100){last48=now;kcd2vr::diagnostics::ownership48::Event e{};e.stage=8;e.tick=now;e.thread=GetCurrentThreadId();e.code=4;e.item=f.context;e.side=d.hand;
            e.flags=(blocked?1u:0u)|(pointer?2u:0u)|(p.foreground?4u:0u)|(g_pointer_armed?8u:0u)|(g_focus_owner?16u:0u)|(f.ray_hit?32u:0u)|(f.native_gamepad?64u:0u)|(prompt_device_lock::Suspended()?128u:0u);
            e.v[0]=float(f.pointer_menu_kind);e.v[1]=f.left.trigger;e.v[2]=f.right.trigger;e.v[3]=f.left.grip;e.v[4]=f.right.grip;e.v[5]=float(f.ray_hand);e.v[6]=float(d.owner);
            e.v[7]=prompt_device_lock::Installed()?1.0f:0.0f;e.v[8]=float(prompt_device_lock::ForcedDevice());e.v[9]=mixed_cursor_ready?1.0f:0.0f; // R25: existing F2 receipt; no native readback
            kcd2vr::diagnostics::ownership48::name(e,"UI_CLICK_ADMISSION_BLOCK_POINTER_FOREGROUND_ARMED_FOCUS_HIT_PAD_KBM");(void)kcd2vr::diagnostics::ownership48::push(e);
        }
    }

    if(!blocked&&!f.native_gamepad)Keyboard(f,p,acceptNew,now);
    else if(f.native_gamepad){ReleaseKeys();g_b_prev=(p.buttons&0x2000)!=0;g_menu_prev=(p.buttons&0x10000)!=0;g_b_eligible=false;g_b_long=false;}
    if(g_pulse_key&&now>=g_pulse_until){Key(g_pulse_key,false);g_pulse_key=0;}
    const bool focusGesture=!blocked&&(f.native_gamepad||acceptNew)&&NativeFocusGesture175(p);
    if(focusGesture&&!g_mouse){
        if(!g_navigation_was)prompt_device_lock::NotifyNavigation();
        g_focus_owner=true;if(f.ray_hit){g_focus_uv=true;g_focus_u=f.u;g_focus_v=f.v;}
    }
    g_navigation_was=focusGesture;
    bool hit=false;
    if(pointer&&p.foreground){
        RECT r{};hit=f.ray_hand==d.hand&&f.ray_hit&&std::isfinite(f.u)&&std::isfinite(f.v)&&f.u>=0&&f.u<=1&&f.v>=0&&f.v<=1&&GetClientRect(f.window,&r)&&r.right-r.left>1&&r.bottom-r.top>1;
        const float trigger=p.hands[d.hand].trigger;
        if(focusGesture&&!g_mouse&&trigger>=.55f)g_pointer_armed=false;
        if(trigger<=.35f){Mouse(false);g_pointer_armed=true;}
        if(!hit){g_pointer_armed=false;g_pixel=false;ClearVirtualCursorR16();} // R16b: no hit = no virtual cursor
        else {
            // Keep the VR ray/dot visible even while native selection has focus.
            // Only mouse-event delivery is suspended, not the visual pointer.
            out.cursor=true;out.u=f.u;out.v=f.v;
            // A stationary/jittering hand cannot steal a newly selected list row.
            // An intentional new ray movement or a fresh click returns mouse focus.
            if(g_focus_owner&&!g_focus_uv){g_focus_uv=true;g_focus_u=f.u;g_focus_v=f.v;}
            const float du=f.u-g_focus_u,dv=f.v-g_focus_v;
            if(!focusGesture&&((g_focus_uv&&du*du+dv*dv>.0001f)||(g_pointer_armed&&trigger>=.55f)))g_focus_owner=false;
            if(g_focus_owner&&!g_mouse)ClearVirtualCursorR16(); // R16b: native focus = real cursor
            if(!g_focus_owner||g_mouse){
                POINT q{static_cast<LONG>(std::lround(f.u*(r.right-r.left-1))),static_cast<LONG>(std::lround(f.v*(r.bottom-r.top-1)))};
                {bool virtualR16=false;long imgW=0,imgH=0;const long cw=r.right-r.left,ch=r.bottom-r.top; // R16b
                 if(g_vc_mode_r16.load(std::memory_order_acquire)&&PresentedImageR16(imgW,imgH)&&(std::labs(imgW-cw)>2||std::labs(imgH-ch)>2)&&EnsureVirtualCursorR16()){
                    POINT image{static_cast<LONG>(std::lround(f.u*(imgW-1))),static_cast<LONG>(std::lround(f.v*(imgH-1)))};POINT virt=image;
                    if(ClientToScreen(f.window,&virt)&&std::labs(virt.x)<1000000&&std::labs(virt.y)<1000000){
                        SetVirtualCursorR16(virt.x,virt.y);virtualR16=true;
                        // q remains real CLIENT pixels; only the qualified game reader sees image pixels.
                    }
                 }
                 if(!virtualR16)ClearVirtualCursorR16();}
                // Production uses the same client->screen contract as the F2
                // comparison. A stationary ray must follow a moved window or a
                // native cursor recenter; client pixels alone cannot cache it.
                if(kcd2vr::diagnostics::ownership48::active(now)){
                    static std::uint64_t last48=0;if(!last48||now<last48||now-last48>=100){last48=now;kcd2vr::diagnostics::ownership48::Event e{};e.stage=8;e.tick=now;e.thread=GetCurrentThreadId();e.code=1;e.actor=reinterpret_cast<std::uintptr_t>(f.window);e.item=f.context;e.side=d.hand;
                        long iw=0,ih=0;(void)PresentedImageR16(iw,ih);e.v[0]=f.u;e.v[1]=f.v;e.v[2]=float(r.right-r.left);e.v[3]=float(r.bottom-r.top);e.v[4]=float(iw);e.v[5]=float(ih);e.v[6]=float(q.x);e.v[7]=float(q.y);e.v[8]=trigger;e.flags=g_vc_until_r16.load()?1u:0u;
                        kcd2vr::diagnostics::ownership48::name(e,"NATIVE_MENU_UV_CLIENT_IMAGE_CURSOR_BEFORE_MOVE");(void)kcd2vr::diagnostics::ownership48::push(e);
                    }
                }
                POINT screen=q,actual{};
                bool moved=ClientToScreen(f.window,&screen)!=0;
                const bool actualValid=RealCursorPosR16(actual); // R16b: the real OS cursor
                if(moved&&ui_hud_experiment36c5::NeedsMove(screen.x,screen.y,g_pixel,g_x,g_y,actualValid,actual.x,actual.y)){
                    moved=SetCursorPos(screen.x,screen.y)!=0;
                    if(moved){g_x=screen.x;g_y=screen.y;g_pixel=true;++g_mouse_moves;}
                }
                if(!moved){g_pixel=false;++g_errors;}
                if(flashui_probe::HudExperiment36Enabled()){
                static RECT lastClient36{};static POINT lastOrigin36{};
                POINT origin{};
                if(ClientToScreen(f.window,&origin)&&(origin.x!=lastOrigin36.x||origin.y!=lastOrigin36.y||std::memcmp(&r,&lastClient36,sizeof(r))!=0)){
                    lastOrigin36=origin;lastClient36=r;char geometry[240]{};
                    std::snprintf(geometry,sizeof(geometry),"V66.178R36C5 HUD_POINTER_CLIENT origin=(%ld,%ld) size=(%ld,%ld) uv=(%.4f,%.4f) target=(%ld,%ld)",origin.x,origin.y,r.right-r.left,r.bottom-r.top,double(f.u),double(f.v),screen.x,screen.y);diagnostic_trace::Write(geometry);
                }
                }
                if(moved){out.cursor=true;out.u=f.u;out.v=f.v;if(g_pointer_armed&&trigger>=.55f)Mouse(true);}
            }
        }
    }else if(!pointer)ResetPointer();
    if(!pointer||!p.foreground)ClearVirtualCursorR16(); // R16b: pointer off or game not in front
    // Functional scroll events, not selection. Never scroll at a stale ray hit.
    if(!f.native_gamepad&&acceptNew&&!f.passive_back_only&&(!pointer||hit)&&std::fabs(p.axes[3])>.55f&&now-g_wheel_at>=100){
        INPUT e{};e.type=INPUT_MOUSE;e.mi.dwFlags=MOUSEEVENTF_WHEEL;e.mi.mouseData=static_cast<DWORD>(p.axes[3]>0?WHEEL_DELTA:-WHEEL_DELTA);
        if(SendInput(1,&e,sizeof(e))!=1)++g_errors;g_wheel_at=now;
    }
    if(!captured&&OwnerState::Neutral(p))g_neutral_exit=false;
    out.blocks_gameplay=captured||g_neutral_exit;
    if(int(d.owner)!=g_last_owner){char m[240]{};std::snprintf(m,sizeof(m),"V66.96-UI-INPUT owner=%d hand=%d nativeXI=%d promptLock=%d mixedCursor=%d errors=%llu",int(d.owner),out.hand,f.native_gamepad?1:0,prompt_device_lock::Suspended()?0:1,prompt_device_lock::MixedCursorReady()?1:0,(unsigned long long)g_errors);diagnostic_trace::Write(m);g_last_owner=int(d.owner);}
    // Four structural changes per qualified native menu lifetime, not a timer,
    // ray-coordinate stream or a native UI query. Records survive capture OFF.
    if(f.pointer_menu_kind==1||f.pointer_menu_kind==2){
        if(g_laser_receipt_context!=f.context){g_laser_receipt_context=f.context;g_laser_receipt_state=~std::uint32_t{0};g_laser_receipts=0;}
        const auto state=unsigned(f.menu)|(unsigned(f.allow_pointer)<<1)|(unsigned(blocked)<<2)|
            (unsigned(p.foreground)<<3)|(unsigned(f.native_gamepad)<<4)|(unsigned(mixed_cursor_ready)<<5)|
            (unsigned(f.right.quick_access_touch)<<6)|(unsigned(hands[0].tracked)<<7)|(unsigned(hands[1].tracked)<<8)|
            (unsigned(hands[0].grip>=.60f)<<9)|(unsigned(hands[1].grip>=.60f)<<10)|
            (unsigned(out.hand+1)<<11)|(unsigned(f.ray_hand+1)<<13)|(unsigned(f.ray_hit)<<15)|
            (unsigned(out.cursor)<<16)|(unsigned(f.pointer_menu_kind)<<17);
        if(state!=g_laser_receipt_state&&g_laser_receipts<4){
            g_laser_receipt_state=state;++g_laser_receipts;char m[440]{};
            std::snprintf(m,sizeof(m),"M1 UI_LASER_INPUT owner=%s context=%llu receipt=%u/4 menu=%d pointerAllowed=%d blocked=%d foreground=%d nativeXI=%d mixedCursor=%d thumbrestModifier=%d tracked=%d,%d grips=%.2f,%.2f selectedHand=%d rayHand=%d rayHit=%d cursor=%d",
                f.pointer_menu_kind==1?"INVENTORY_MAP":"SHOP",(unsigned long long)f.context,g_laser_receipts,f.menu,f.allow_pointer,blocked,p.foreground,f.native_gamepad,mixed_cursor_ready,
                f.right.quick_access_touch,hands[0].tracked,hands[1].tracked,double(hands[0].grip),double(hands[1].grip),out.hand,f.ray_hand,f.ray_hit,out.cursor);
            diagnostic_trace::Write(m);
        }
    }
    g_context=f.context;g_native_pad=f.native_gamepad;out.native_pad=f.native_gamepad;return out;
}
int PointerHand() noexcept{return g_hand.load();}
std::uint8_t WorldGripBlockedMask() noexcept{return g_grips.load();}
void WriteSnapshot() noexcept {
    char m[256]{};std::snprintf(m,sizeof(m),"V66.96-UI-INPUT-SNAPSHOT transport=%s context=%llu hand=%d keys=%llu mouseMoves=%llu focusOwns=%d errors=%llu gripBlock=0x%X",
        g_native_pad?"NATIVE_XI":"STOCK_SCAN_FALLBACK",(unsigned long long)g_context,g_hand.load(),(unsigned long long)g_keys_sent,(unsigned long long)g_mouse_moves,g_focus_owner?1:0,(unsigned long long)g_errors,unsigned(g_grips.load()));diagnostic_trace::Write(m);prompt_device_lock::WriteCursorSnapshot();
}
} // namespace
namespace kcd2vr::ui_menu_input {
void SetWindowedLaserImageSpaceR16(bool on) noexcept {g_vc_mode_r16.store(on,std::memory_order_release);if(!on)ClearVirtualCursorR16();}
}
