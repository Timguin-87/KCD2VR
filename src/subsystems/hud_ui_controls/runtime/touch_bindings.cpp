#include "touch_bindings.h"
#include "quick_access_r112.h"
#include "controller_layout_r118.h" // R118: X hold = inventory on the STEAMVR / PS VR2 layout
#include "held_map_m1.h"
#include "subsystems/vrmap/runtime/native_map_open_h25.h"
#include "debug_trace.h"
#include "diagnostic_trace.h" // V66.178R21: R3_EDGE goes to the diagnostics log
#include "native_input_look_lab.h"

#include <windows.h>
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace kcd2vr {

TouchBindings::Dir TouchBindings::StickDir(float x, float y, float threshold)
{
    const float ax=std::fabs(x), ay=std::fabs(y);
    if (std::max(ax,ay) < threshold) return Dir::None;
    if (ay >= ax) return y >= 0.0f ? Dir::Up : Dir::Down;
    return x >= 0.0f ? Dir::Right : Dir::Left;
}

namespace {
UINT SendScanKey(unsigned short vk, bool down)
{
    INPUT i{}; i.type=INPUT_KEYBOARD;
    const UINT scan=MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
    if(scan){ i.ki.wVk=0; i.ki.wScan=static_cast<WORD>(scan); i.ki.dwFlags=KEYEVENTF_SCANCODE | (down?0:KEYEVENTF_KEYUP); }
    else { i.ki.wVk=vk; i.ki.dwFlags=down?0:KEYEVENTF_KEYUP; }
    const UINT sent=SendInput(1,&i,sizeof(i));
    if(sent!=1){ char m[160]{}; std::snprintf(m,sizeof(m),"V58.6-INJECT key vk=0x%02X %s FAILED sent=%u err=%lu",vk,down?"down":"up",sent,GetLastError()); debug_trace::Write(m); }
    return sent;
}

float LinearAxis(float v, float dead)
{
    const float av=std::fabs(v);
    if(av<=dead) return 0.0f;
    const float n=std::clamp((av-dead)/(1.0f-dead),0.0f,1.0f);
    return std::copysign(n,v);
}

float CurvedAxis(float v, float dead)
{
    const float av=std::fabs(v);
    if(av<=dead) return 0.0f;
    const float n=std::clamp((av-dead)/(1.0f-dead),0.0f,1.0f);
    // Gentle curve around centre, full authority near the edge.
    const float c=n*n*(3.0f-2.0f*n);
    return std::copysign(c,v);
}
}

void TouchBindings::KeyDown(unsigned short vk)
{
    (void)SendScanKey(vk,true);
}
void TouchBindings::KeyUp(unsigned short vk)
{
    (void)SendScanKey(vk,false);
}
void TouchBindings::KeyTap(unsigned short vk)
{
    const UINT a=SendScanKey(vk,true), b=SendScanKey(vk,false);
    if(a==1 && b==1){ char m[128]{}; std::snprintf(m,sizeof(m),"V58.6-INJECT keytap vk=0x%02X accepted-by-Windows",vk); debug_trace::Write(m); }
}
void TouchBindings::MouseButton(bool left, bool down)
{
    INPUT i{}; i.type=INPUT_MOUSE;
    if(left) i.mi.dwFlags=down?MOUSEEVENTF_LEFTDOWN:MOUSEEVENTF_LEFTUP;
    else i.mi.dwFlags=down?MOUSEEVENTF_RIGHTDOWN:MOUSEEVENTF_RIGHTUP;
    const UINT sent=SendInput(1,&i,sizeof(i));
    char m[192]{};
    if(sent==1) {
        std::snprintf(m,sizeof(m),"V58.6-INJECT mouse=%s state=%s accepted-by-Windows",left?"left":"right",down?"down":"up");
    } else {
        std::snprintf(m,sizeof(m),"V58.6-INJECT mouse=%s state=%s FAILED sent=%u err=%lu",left?"left":"right",down?"down":"up",sent,GetLastError());
    }
    debug_trace::Write(m);
}
void TouchBindings::MouseMoveRelative(long dx,long dy)
{
    if(dx==0 && dy==0) return;
    INPUT i{}; i.type=INPUT_MOUSE; i.mi.dx=dx; i.mi.dy=dy; i.mi.dwFlags=MOUSEEVENTF_MOVE;
    const UINT sent=SendInput(1,&i,sizeof(i));
    if(sent!=1) {
        char m[192]{}; std::snprintf(m,sizeof(m),"V58.6-INJECT mouse-move dx=%ld dy=%ld FAILED sent=%u err=%lu",dx,dy,sent,GetLastError());
        debug_trace::Write(m);
    }
}

void TouchBindings::MouseWheel(int delta)
{
    if(!delta) return;
    INPUT i{}; i.type=INPUT_MOUSE; i.mi.dwFlags=MOUSEEVENTF_WHEEL; i.mi.mouseData=static_cast<DWORD>(delta);
    const UINT sent=SendInput(1,&i,sizeof(i));
    if(sent!=1){ char m[192]{}; std::snprintf(m,sizeof(m),"V65.02b-INJECT mouse-wheel delta=%d FAILED sent=%u err=%lu",delta,sent,GetLastError()); debug_trace::Write(m); }
}
void TouchBindings::SetKeyHeld(unsigned short vk,bool want,bool& held)
{
    if(want==held) return;
    held=want; if(want) KeyDown(vk); else KeyUp(vk);
    char m[128]{}; std::snprintf(m,sizeof(m),"V58.6-INJECT held vk=0x%02X state=%s",vk,want?"down":"up"); debug_trace::Write(m);
}

void TouchBindings::SetEnabled(bool enabled)
{
    if(enabled_==enabled) return;
    if(!enabled){back_escape_m1_.Cancel();Reset();}
    enabled_=enabled;
    debug_trace::Write(enabled ? "V58.6-TOUCH bindings ENABLED" : "V58.6-TOUCH bindings DISABLED; injected keys released");
}



bool TouchBindings::RequestPhysicalInteractionUse(std::uint64_t originalPressD48)
{
    if(!enabled_ || interact_held_) return false;
    if(native_gamepad_actions_){
        physical_interact_pulse59c10_=static_cast<std::uint64_t>(GetTickCount64())+80;
        physical_interact_press_d48_=originalPressD48;
        return true;
    }
    const UINT down=SendScanKey(config_.interact,true),up=SendScanKey(config_.interact,false);
    return down==1&&up==1;
}

void TouchBindings::ValidatePhysicalInteractionPressD48(std::uint64_t originalPress,bool held) noexcept
{
    if(physical_interact_pulse59c10_&&physical_interact_press_d48_&&
       (!held||originalPress!=physical_interact_press_d48_)){
        physical_interact_pulse59c10_=0;physical_interact_press_d48_=0;
    }
}

bool TouchBindings::SetPhysicalInteractionHeld114(bool held)
{
    physical_interact_xi59c10_=enabled_&&held&&native_gamepad_actions_;
    physical_interact114_=enabled_&&held&&!native_gamepad_actions_;
    SetKeyHeld(config_.interact,manual_interact114_||physical_interact114_,interact_held_);
    return physical_interact114_||physical_interact_xi59c10_;
}
void TouchBindings::SetNativeGamepadActions(bool enabled)
{
    if(native_gamepad_actions_==enabled)return;
    // A device-family change ends this synthetic request. Do not replay it in
    // the new family, or release a separately held manual keyboard interaction.
    physical_interact114_=physical_interact_xi59c10_=false;
    physical_interact_pulse59c10_=0;
    SetKeyHeld(config_.interact,manual_interact114_,interact_held_);
    native_gamepad_actions_=enabled;
}
bool TouchBindings::PhysicalInteractionGamepad59c10(std::uint64_t now,bool allowed) noexcept
{
    if(!enabled_||!native_gamepad_actions_||!allowed){
        physical_interact_xi59c10_=false;physical_interact_pulse59c10_=0;return false;
    }
    if(physical_interact_pulse59c10_&&
       (now>=physical_interact_pulse59c10_||physical_interact_pulse59c10_-now>80))physical_interact_pulse59c10_=0;
    return physical_interact_xi59c10_||physical_interact_pulse59c10_!=0;
}
void TouchBindings::SetManualInteractionHeld114(bool held)
{
    manual_interact114_=held;
    SetKeyHeld(config_.interact,manual_interact114_||physical_interact114_,interact_held_);
}
void TouchBindings::ReleaseInteractionOwners114()
{
    manual_interact114_=physical_interact114_=false;
    physical_interact_xi59c10_=false;physical_interact_pulse59c10_=0;
    SetKeyHeld(config_.interact,false,interact_held_);
}


void TouchBindings::SetVrikInputLabStage(int stage)
{
    stage = std::clamp(stage, 0, 3);
    if(stage==vrik_input_lab_stage_) return;
    vrik_input_lab_stage_=stage;
    // Never leave a synthetic combat button held when ownership is removed.
    if(stage>=1 && lmb_){ MouseButton(true,false); lmb_=false; }
    if(stage>=2 && rmb_){ MouseButton(false,false); rmb_=false; }
    char m[256]{};
    std::snprintf(m,sizeof(m),
        "V65.43-F9-INPUT stage=%d RTattack=%s LTblock=%s quickWeaponToggle=%s scope=GAMEPLAY-ONLY",
        stage, stage>=1?"SUPPRESSED":"NATIVE", stage>=2?"SUPPRESSED":"NATIVE", stage>=3?"SUPPRESSED":"NATIVE");
    debug_trace::Write(m);
}

void TouchBindings::Reset()
{
    SetKeyHeld(config_.move_forward,false,w_); SetKeyHeld(config_.move_left,false,a_); SetKeyHeld(config_.move_back,false,s_); SetKeyHeld(config_.move_right,false,d_);
    SetKeyHeld(config_.sprint,false,shift_); ReleaseInteractionOwners114();
    if(lmb_){ MouseButton(true,false); lmb_=false; }
    if(rmb_){ MouseButton(false,false); rmb_=false; }
    if(b_x_held_){ KeyUp(config_.horse_dog); b_x_held_=false; }
    menu_prev_=y_prev_=x_prev_=false; x_prev_down_=false; x_down_since_=0; x_long_sent_=false;
    b_prev_down_=false; b_down_since_=0; b_long_sent_=false;b_quick_reply_eligible175_=false;
    right_grip_prev_down_=false; right_grip_since_=0; right_grip_used_as_modifier_=false;
    torch_chord_prev_=false; vr_menu_chord_prev_=false; vr_menu_chord_since_=0; sticks_menu_chord_prev_r112_=false; sticks_menu_chord_since_r112_=0; map_chord_latched_=false; map_chord_since_=0;
    y_prev_down_=false; y_down_since_=0; y_long_sent_=false;
    left_menu_prev_down_=false; left_menu_since_=0; left_menu_long_sent_=false; inventory_request118_=false;
    quick_dir_prev_=Dir::None; utility_dir_since_=0; utility_long_sent_=false; quick_consumable_dir_=Dir::None; attack_dir_prev_=Dir::None;
    if(quick_consumable_key_){ KeyUp(quick_consumable_key_); quick_consumable_key_=0; }
    quick_consumable_pending_key_=0; quick_consumable_hold_at_ms_=0;
    jump_dir_prev_=Dir::None; crouch_dir_prev_=Dir::None; target_switch_latched_=false; snap_turn_latched_=true; last_combat_input_ms_=0;
    menu_nav_l_=menu_nav_r_=Dir::None; menu_nav_l_next_repeat_ms_=menu_nav_r_next_repeat_ms_=0;
    quick_selected_key_=0;
    if(b_context_back_held_){ KeyUp(config_.pause); b_context_back_held_=false; }
    if(minigame_rt_held_){KeyUp('F');minigame_rt_held_=false;}
    if(minigame_x_held_){KeyUp('R');minigame_x_held_=false;}
    if(minigame_y_held_){KeyUp('Q');minigame_y_held_=false;}
    minigame_lt_held_=false;minigame_dpad_prev_=Dir::None;minigame_page_prev_=Dir::None;alchemy_context_=false;alchemy_reading_context_=false;
    turn_last_ms_=0; turn_fraction_=0.0f; bow_fraction_y_=0.0f; turn_last_log_ms_=0; bow_last_log_ms_=0;
}

bool TouchBindings::UpdatePauseChord58c2(ControllerState& left,const ControllerState& right,bool allowed)
{
    const auto d=pause_chord58c2_.update(enabled_&&left.active&&right.active,allowed,left.menu_down,right.quick_access_touch);
    if(d.consume){ConsumeMenuUntilRelease174r2();left.menu_down=false;}
    if(d.fire){KeyTap(config_.pause);debug_trace::Write("R58C2 R_THUMBREST_MENU pause edge native=ESCAPE inventory=CONSUMED");}
    return d.fire;
}

bool TouchBindings::UpdateBackEscapeM1(const ControllerState& right,bool allowed,std::uint64_t input_generation)
{
    if(!back_escape_m1_.Update(enabled_&&right.active&&right.b_valid,allowed,right.b_down,GetTickCount64(),input_generation))return false;
    // End only this transport's own held Escape before the explicit down/up
    // pair. Native XI B and physical Grip state remain unchanged.
    if(config_.pause==VK_ESCAPE)SetKeyHeld(config_.pause,false,b_context_back_held_);
    KeyTap(VK_ESCAPE);
    diagnostic_trace::Write("M1 INPUT B_HOLD_ESCAPE heldMs=2000 key=ESCAPE pair=DOWN_UP oncePerPress=1 nativeB=UNCHANGED acceptance=REQUEST_ONLY");
    return true;
}

TouchBindings::Events TouchBindings::Update(const ControllerState& left,const ControllerState& right,bool mis_menu_visible,bool theater_ui_visible,bool contextual_ui_visible,bool back_context_visible)
{
    Events ev{};
    const bool inventoryRequest118=inventory_request118_; inventory_request118_=false; // R118: one frame only, never replayed later
    if(!enabled_ || (!left.active && !right.active)) { Reset(); r3_prev_down_=true; return ev; } // R20C1: recover only after a known neutral R3

    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const bool backRecoveryConsumedM1=back_escape_m1_.Consumed();
    if(menu_consumed174r2_){
        left_menu_prev_down_=left.menu_down;
        left_menu_long_sent_=left.menu_down;
        left_menu_since_=0;
        if(!left.menu_down)menu_consumed174r2_=false;
    }
    // Consume the click even in a menu/minigame. A held click cannot become
    // a new special attack after an ownership/transport/Grip transition.
    // V66.178R20: `thumbstick_pressed` is OpenXR's changedSinceLastSync, true for ONE XR sync (72-90 Hz); this
    // update runs at game rate (18-30 fps in the 2026-10-09 run), so most clicks were never seen (R3_SPECIAL 0
    // for the whole session). The edge is taken on the LEVEL as this update sees it.
    const bool r3Edge162=right.active&&(right.thumbstick_down||right.thumbstick_pressed)&&!r3_prev_down_;
    r3_prev_down_=!right.active||right.thumbstick_down||right.thumbstick_pressed; // inactive input is quarantined until a tracked neutral
    const bool authored_minigame=alchemy_context_||alchemy_reading_context_||native_minigame162r1_;
    // A menu, authored activity or lost right controller cannot donate a held
    // deflection to gameplay. Require neutral before the next snap.
    const bool snap_gameplay_d37=right.active&&!mis_menu_visible&&!theater_ui_visible&&
        !contextual_ui_visible&&!back_context_visible&&!authored_minigame;
    if(!snap_gameplay_d37)
        snap_turn_latched_=true;
    if(!authored_minigame){
        if(minigame_rt_held_){KeyUp('F');minigame_rt_held_=false;}
        if(minigame_x_held_){KeyUp('R');minigame_x_held_=false;}
        if(minigame_y_held_){KeyUp('Q');minigame_y_held_=false;}
        if(minigame_dpad_prev_==Dir::Up)KeyUp('X');
        minigame_dpad_prev_=Dir::None;minigame_page_prev_=Dir::None;
        minigame_lt_held_=false;
    }
    static std::uint64_t last_state_log_ms=0;
    if(now-last_state_log_ms>=5000ull) {
        last_state_log_ms=now;
        char m[704]{};
        std::snprintf(m,sizeof(m),
            "V58.9-TOUCH-STATE menu=%s L={active:%s stick:(%.3f,%.3f) click:%s trig:%.3f grip:%.3f X:%s Y:%s Menu:%s} R={active:%s stick:(%.3f,%.3f) click:%s thumbrest:%s trig:%.3f grip:%.3f[RESERVED] A:%s B:%s}",
            mis_menu_visible?"true":"false",
            left.active?"true":"false",left.thumb_x,left.thumb_y,left.thumbstick_down?"down":"up",left.trigger,left.grip,left.x_down?"down":"up",left.y_down?"down":"up",left.menu_down?"down":"up",
            right.active?"true":"false",right.thumb_x,right.thumb_y,right.thumbstick_down?"down":"up",right.quick_access_touch?"touch":"off",right.trigger,right.grip,right.a_down?"down":"up",right.b_down?"down":"up");
        debug_trace::Write(m);
    }

    // V65.03b utility pairing: X tap = Torch, X hold = Visor in gameplay.
    // Menus/dialogue do not leak these equipment actions. Release after a hold
    // never also toggles Torch.
    const bool quick_touch=right.quick_access_touch;
    const bool x_utility_context=!native_gamepad_actions_ && !authored_minigame && !quick_touch && !mis_menu_visible && !theater_ui_visible && !contextual_ui_visible && !back_context_visible;
    if(left.x_down && !x_prev_down_) { x_down_since_=now; x_long_sent_=false; }
    if(x_utility_context && left.x_down && !x_long_sent_ && x_down_since_ && now-x_down_since_>=800ull) {
        // R118: keyboard fallback on the STEAMVR / PS VR2 layout: X hold = Inventory (same request as Left Menu).
        if(kcd2vr::controller_layout_r118::XHoldOpensInventory(kcd2vr::controller_layout_r118::Get())){
            KeyTap(config_.inventory); ev.toggle_inventory_comfort=true; debug_trace::Write("R118-TOUCH X HOLD -> native Inventory toggle request (layout STEAMVR/PSVR2)");
        } else { KeyTap(config_.visor); debug_trace::Write("V65.03b-TOUCH X HOLD -> Z visor toggle"); }
        x_long_sent_=true;
    }
    if(!left.x_down && x_prev_down_) {
        if(x_utility_context && !x_long_sent_){ KeyTap(config_.torch); debug_trace::Write("V65.03b-TOUCH X TAP -> R torch toggle"); }
        x_down_since_=0; x_long_sent_=false;
    }
    x_prev_down_=left.x_down;

    // V66.108: capacitive quick-access layer. Left Grip is now entirely physical.
    // Quest/Touch binds quick_access_touch to /user/hand/right/input/thumbrest/touch.
    // The touch alone does nothing: a deliberate digital left-stick direction activates the chord.
    const Dir meta_left_dir = StickDir(left.thumb_x,left.thumb_y);
    native_map_open_h25::ObserveChord(quick_touch&&meta_left_dir==Dir::Up); // physical chord rearm survives TouchBindings::Reset
    // V66.109 keeps the successful V66.108 RIGHT thumbrest modifier. Both
    // physical Grips remain completely free for interaction / support / holsters.
    const bool quick_chord_active = quick_touch && meta_left_dir!=Dir::None;
    const bool vr_menu_chord = !authored_minigame && quick_touch && meta_left_dir==Dir::Down;
    const bool map_chord = !authored_minigame && !mis_menu_visible && quick_touch && meta_left_dir==Dir::Up && !vr_menu_chord;
    if(map_chord) {
        if(map_chord_since_==0) map_chord_since_=now;
        if(!map_chord_latched_ && now-map_chord_since_>=500ull) {
            bool accepted=false;
            if(held_map_m1::Selected()){
                accepted=!native_map_open_h25::chordClaimed.exchange(true)&&(held_map_m1::Requested()||held_map_m1::Allowed(now));
                if(accepted)held_map_m1::SetRequested31(!held_map_m1::Requested(),now);
            }else{KeyTap(config_.map);ev.toggle_map_comfort=true;accepted=true;}
            map_chord_latched_=true;
            char receipt[224]{};
            std::snprintf(receipt,sizeof(receipt),"H25 MAP_GESTURE route=%s accepted=%d holdMs=%llu presentationOwner=STATIC_PAPER_M41",
                held_map_m1::Selected()?"STATIC_PAPER_M41":"NATIVE_MAP_MENU",accepted,
                (unsigned long long)(now-map_chord_since_));diagnostic_trace::Write(receipt);
        }
    } else {
        map_chord_since_=0; map_chord_latched_=false;
    }
    // R112 (Miki: "could not find thumb rest equivalent on playstation vr2"): controllers without a thumbrest (PS VR2
    // Sense, Index, Vive, WMR - quick_access_touch is only bound on the Touch profile) get a second VR menu chord:
    // BOTH stick clicks held 0.6 s (L3 first: a held L3 already keeps R3 from requesting a kick, r3SpecialEligible
    // below). works on every controller, in and out of the menu; the thumbrest chord is unchanged.
    const bool sticks_menu_chord_r112 = !authored_minigame && left.active && right.active && left.thumbstick_down && right.thumbstick_down;
    if(sticks_menu_chord_r112) {
        if(sticks_menu_chord_since_r112_==0) sticks_menu_chord_since_r112_=now;
        if(!sticks_menu_chord_prev_r112_ && now-sticks_menu_chord_since_r112_>=600ull) {
            ev.toggle_mis_menu=true; sticks_menu_chord_prev_r112_=true;
            debug_trace::Write("R112-TOUCH L3+R3 HOLD 0.6s -> VR menu toggle (controllers without a thumbrest)");
        }
    } else {
        sticks_menu_chord_since_r112_=0; sticks_menu_chord_prev_r112_=false;
    }
    if(vr_menu_chord) {
        if(vr_menu_chord_since_==0) vr_menu_chord_since_=now;
        if(!vr_menu_chord_prev_ && now-vr_menu_chord_since_>=500ull) {
            ev.toggle_mis_menu=true; vr_menu_chord_prev_=true;
            debug_trace::Write("V66.108-TOUCH R-THUMBREST+LS-DOWN HOLD 0.5s -> VR menu toggle");
        }
    } else {
        vr_menu_chord_since_=0; vr_menu_chord_prev_=false;
    }

    // V66.109 gameplay-only meta chord: RIGHT thumbrest + X hold opens
    // KCD2's proven Skip-Time/Wait keyboard superaction. Authored minigames
    // keep X for the action shown by KCD2's own controller glyph.
    if(!authored_minigame && !mis_menu_visible && !theater_ui_visible && !contextual_ui_visible && quick_touch &&
       left.x_down && !x_long_sent_ && x_down_since_ && now-x_down_since_>=800ull) {
        KeyTap(config_.skip_time);
        x_long_sent_=true;
        debug_trace::Write("V66.109-TOUCH R-THUMBREST+X HOLD -> native Skip Time / Wait request");
    }

    if(mis_menu_visible) {
        SetKeyHeld(config_.move_forward,false,w_); SetKeyHeld(config_.move_left,false,a_); SetKeyHeld(config_.move_back,false,s_); SetKeyHeld(config_.move_right,false,d_);
        SetKeyHeld(config_.sprint,false,shift_); ReleaseInteractionOwners114();
        if(lmb_){MouseButton(true,false);lmb_=false;} if(rmb_){MouseButton(false,false);rmb_=false;}
        attack_dir_prev_=Dir::None; quick_dir_prev_=Dir::None; quick_consumable_dir_=Dir::None; jump_dir_prev_=crouch_dir_prev_=Dir::None;
        if(quick_consumable_key_){ KeyUp(quick_consumable_key_); quick_consumable_key_=0; }
        quick_consumable_pending_key_=0; quick_consumable_hold_at_ms_=0;
        turn_last_ms_=now; turn_fraction_=0.0f;
        // Consume B while the mod menu owns input; holding it through close is not
        // a fresh world Quick Speak press.
        b_prev_down_=right.b_down;b_down_since_=0;b_long_sent_=true;b_quick_reply_eligible175_=false;
        return ev;
    }

    // V66.98 AUTHORED MINIGAME CONTROLLER SEMANTICS.  Alchemy/recipe-book
    // contexts keep the exact KCD2 Xbox meanings even when native XI is unavailable.
    // The keyboard keys below are not invented aliases: they are the retail
    // keybindSuperactions equivalents for the same action-map entries.
    if(authored_minigame) {
        SetKeyHeld(config_.move_forward,false,w_); SetKeyHeld(config_.move_left,false,a_); SetKeyHeld(config_.move_back,false,s_); SetKeyHeld(config_.move_right,false,d_);
        SetKeyHeld(config_.sprint,false,shift_);
        if(lmb_){MouseButton(true,false);lmb_=false;} if(rmb_){MouseButton(false,false);rmb_=false;}
        menu_nav_l_=menu_nav_r_=Dir::None;menu_nav_l_next_repeat_ms_=menu_nav_r_next_repeat_ms_=0;
        turn_fraction_=bow_fraction_y_=0.0f;
        const Dir md=quick_touch?StickDir(left.thumb_x,left.thumb_y):Dir::None;

        SetManualInteractionHeld114(!native_gamepad_actions_&&right.a_down);
        if(!native_gamepad_actions_) {
            // A / Cross = primary minigame action (E/Enter superaction).
            if(alchemy_context_) {
                // LT = bag/bellows (Q), RT = sandglass/secondary (F), X = reset/back superaction (R).
                const bool lt=left.trigger>0.55f, rt=right.trigger>0.55f;
                if(lt&&!minigame_lt_held_) KeyTap('Q');
                minigame_lt_held_=lt;
                if(rt!=minigame_rt_held_){minigame_rt_held_=rt; if(rt)KeyDown('F');else KeyUp('F');}
                if(left.x_down!=minigame_x_held_){minigame_x_held_=left.x_down;if(minigame_x_held_)KeyDown('R');else KeyUp('R');}
                // RIGHT thumbrest+LS is the FULL authored D-pad in minigames: Up=move pot (X), Down=ingredients/inventory (I).
                if(md!=minigame_dpad_prev_){
                    if(minigame_dpad_prev_==Dir::Up)KeyUp('X');
                    if(md==Dir::Up)KeyDown('X');
                    if(md==Dir::Down)KeyTap(config_.inventory);
                    minigame_dpad_prev_=md;
                }
            } else if(alchemy_reading_context_) {
                // Recipe book: A=select (E), Y=toggle (Q), B=back (R).
                // Grips stay physical; RS-X is the spare VR page axis and maps
                // to the retail keyboard read_prev/read_next superactions.
                if(left.y_down!=minigame_y_held_){minigame_y_held_=left.y_down;if(minigame_y_held_)KeyDown('Q');else KeyUp('Q');}
                const Dir page=StickDir(right.thumb_x,0.0f);
                if(page!=minigame_page_prev_){
                    if(page==Dir::Left)KeyTap(VK_LEFT);
                    if(page==Dir::Right)KeyTap(VK_RIGHT);
                    minigame_page_prev_=page;
                }
            }

            // Native controller B is overloaded by KCD2 itself: short Back vs >=0.25s minigame exit.
            // Defer the keyboard pulse so a hold cannot first fire the short-back action.
            if(right.b_down&&!b_prev_down_){b_down_since_=now;b_long_sent_=false;}
            if(right.b_down&&!backRecoveryConsumedM1&&!b_long_sent_&&b_down_since_&&now-b_down_since_>=250ull){SetKeyHeld(config_.pause,true,b_context_back_held_);b_long_sent_=true;}
            if(!right.b_down&&b_prev_down_){SetKeyHeld(config_.pause,false,b_context_back_held_);if(!b_long_sent_&&!backRecoveryConsumedM1)KeyTap('R');b_down_since_=0;b_long_sent_=false;}

            // Quest Menu stands in for Xbox View/Back = Help while the minigame owns input.
            if(left.menu_down&&!left_menu_prev_down_)left_menu_since_=now;
            if(!left.menu_down&&left_menu_prev_down_){KeyTap(config_.skip_time);left_menu_since_=0;}
        }
        // Help/View is owned by this activity. Holding/releasing it through
        // activity exit must not also open Inventory in the gameplay adapter.
        if(left.menu_down){menu_consumed174r2_=true;left_menu_long_sent_=true;}
        b_quick_reply_eligible175_=false;
        b_prev_down_=right.b_down;left_menu_prev_down_=left.menu_down;
        return ev;
    }

    // V65.72 THEATER INPUT: retail APSE-style keyboard/mouse transport:
    // right-controller ray -> OS mouse (owned by FlatUiPointerInjector in mod.cpp),
    // trigger/A -> mouse click there, both sticks -> arrow keys, right-stick Y -> mouse wheel,
    // B -> contextual Escape/Back, left Menu tap -> native Inventory toggle.
    // No APSE alias chords, no native-XI switching, no immersive pointer fallback.
    if(theater_ui_visible) {
        SetKeyHeld(config_.move_forward,false,w_); SetKeyHeld(config_.move_left,false,a_); SetKeyHeld(config_.move_back,false,s_); SetKeyHeld(config_.move_right,false,d_);
        SetKeyHeld(config_.sprint,false,shift_); ReleaseInteractionOwners114();
        if(lmb_){MouseButton(true,false);lmb_=false;} if(rmb_){MouseButton(false,false);rmb_=false;}
        if(quick_consumable_key_){ KeyUp(quick_consumable_key_); quick_consumable_key_=0; }
        quick_consumable_pending_key_=0; quick_consumable_hold_at_ms_=0;
        quick_dir_prev_=quick_consumable_dir_=attack_dir_prev_=jump_dir_prev_=crouch_dir_prev_=Dir::None;
        turn_last_ms_=now; turn_fraction_=bow_fraction_y_=0.0f;

        // V65.72 restores the proven V64.14 Theater navigation contract and matches
        // retail defaultProfile.xml: left stick focus + right stick wheel/slider are accepted for left/right/up/down.
        // Keep the old hysteresis/repeat behavior so ray-pointer ownership and stick
        // navigation remain independent instead of fighting each other.
        auto nav_one=[&](float x,float y,Dir& held,std::uint64_t& nextRepeat){
            const float ax=std::fabs(x), ay=std::fabs(y);
            Dir d=Dir::None;
            const float threshold=(held==Dir::None)?0.25f:0.18f;
            if(std::max(ax,ay)>=threshold) d=(ay>=ax)?(y>=0.0f?Dir::Up:Dir::Down):(x>=0.0f?Dir::Right:Dir::Left);
            const auto vk_for=[](Dir q)->unsigned short { switch(q){ case Dir::Up:return VK_UP; case Dir::Down:return VK_DOWN; case Dir::Left:return VK_LEFT; case Dir::Right:return VK_RIGHT; default:return 0; } };
            if(d==Dir::None){ held=Dir::None; nextRepeat=0; return; }
            if(d!=held){ held=d; nextRepeat=now+350ull; if(const auto vk=vk_for(d)){ KeyTap(vk); ev.theater_ui_input=true; } }
            else if(nextRepeat && now>=nextRepeat){ nextRepeat=now+100ull; if(const auto vk=vk_for(d)){ KeyTap(vk); ev.theater_ui_input=true; } }
        };
        // Left stick owns keyboard focus navigation. Right-stick vertical is scroll
        // only (do not also emit Up/Down); right-stick horizontal remains useful for
        // slider/rotate/sort-style UI. This avoids the old wheel+arrow double action.
        nav_one(left.thumb_x,left.thumb_y,menu_nav_l_,menu_nav_l_next_repeat_ms_);
        nav_one(right.thumb_x,0.0f,menu_nav_r_,menu_nav_r_next_repeat_ms_);

        // Retail superaction keyboard equivalents that actually exist:
        // X -> Tab/cycle_tabs (tab_next), Y -> Q/toggle_info. Prompt classifier stays Xbox.
        static bool theater_x_prev=false,theater_y_prev=false;
        if(left.x_down && !theater_x_prev){ KeyTap(VK_TAB); ev.theater_ui_input=true; debug_trace::Write("V66.69-THEATER X -> retail cycle_tabs/Tab"); }
        if(left.y_down && !theater_y_prev){ KeyTap('Q'); ev.theater_ui_input=true; debug_trace::Write("V66.69-THEATER Y -> retail toggle_info/Q"); }
        theater_x_prev=left.x_down; theater_y_prev=left.y_down;

        static int wheelHeld=0; static std::uint64_t wheelNext=0;
        const int wantWheel=right.thumb_y>0.30f?1:(right.thumb_y<-0.30f?-1:0);
        if(!wantWheel){ wheelHeld=0; wheelNext=0; }
        else if(wantWheel!=wheelHeld){
            wheelHeld=wantWheel; wheelNext=now+250ull; MouseWheel(wantWheel>0?WHEEL_DELTA:-WHEEL_DELTA); ev.theater_ui_input=true;
        } else if(wheelNext && now>=wheelNext){
            wheelNext=now+85ull; MouseWheel(wantWheel>0?WHEEL_DELTA:-WHEEL_DELTA); ev.theater_ui_input=true;
        }

        // A and trigger are consumed by the Theater pointer injector as left-click.
        ReleaseInteractionOwners114();

        // V66.86: Theater Back must behave like a real controller Back/Escape. A tap
        // sends one native pulse on release; a 700 ms hold sends the same pulse as a
        // failsafe, never a held key, so no Escape state can leak into gameplay.
        if(right.b_down && !b_prev_down_){ b_down_since_=now; b_long_sent_=false; }
        if(right.b_down && !backRecoveryConsumedM1 && !b_long_sent_ && b_down_since_ && now-b_down_since_>=700ull){
            KeyTap(config_.pause); b_long_sent_=true; ev.theater_ui_input=true; ev.back_cancel=true;
            debug_trace::Write("V66.86-THEATER B HOLD 700ms -> native Escape/Back pulse");
        }
        if(!right.b_down && b_prev_down_){
            if(!b_long_sent_&&!backRecoveryConsumedM1){KeyTap(config_.pause);ev.theater_ui_input=true;ev.back_cancel=true;debug_trace::Write("V66.86-THEATER B TAP -> native Escape/Back pulse");}
            b_down_since_=0; b_long_sent_=false;
        }
        b_prev_down_=right.b_down;
        if(b_context_back_held_){ KeyUp(config_.pause); b_context_back_held_=false; }

        // Left Menu is the authoritative Inventory Theater switch. Send the same
        // native keyboard Inventory action on every short tap; mod.cpp treats the edge as
        // the request and keeps Theater until native close ACK. No hold-Escape meaning remains here.
        if(left.menu_down && !left_menu_prev_down_) { left_menu_since_=now; left_menu_long_sent_=false; }
        if(!left.menu_down && left_menu_prev_down_) {
            if(!native_gamepad_actions_) KeyTap(config_.inventory);
            ev.toggle_inventory_comfort=true;
            ev.theater_ui_input=true;
            debug_trace::Write("V65.72-THEATER Menu TAP -> native Inventory toggle request; presentation waits for native close ACK");
            left_menu_since_=0; left_menu_long_sent_=false;
        }
        left_menu_prev_down_=left.menu_down;
        return ev;
    } else if(contextual_ui_visible) {
        // V65.69 immersive ItemTransfer/ItemSelection helper: navigation only.
        // No VR pointer, no mouse motion, no wheel, no APSE aliases. Preserve only
        // keyboard-style selection/back so the existing immersive UI remains usable.
        SetKeyHeld(config_.move_forward,false,w_); SetKeyHeld(config_.move_left,false,a_); SetKeyHeld(config_.move_back,false,s_); SetKeyHeld(config_.move_right,false,d_);
        SetKeyHeld(config_.sprint,false,shift_);
        if(lmb_){MouseButton(true,false);lmb_=false;} if(rmb_){MouseButton(false,false);rmb_=false;}
        const Dir d=StickDir(left.thumb_x,left.thumb_y,0.35f);
        const auto vk_for=[](Dir q)->unsigned short { switch(q){ case Dir::Up:return VK_UP; case Dir::Down:return VK_DOWN; case Dir::Left:return VK_LEFT; case Dir::Right:return VK_RIGHT; default:return 0; } };
        if(d==Dir::None){ menu_nav_l_=Dir::None; menu_nav_l_next_repeat_ms_=0; }
        else if(d!=menu_nav_l_){ menu_nav_l_=d; menu_nav_l_next_repeat_ms_=now+350ull; if(const auto vk=vk_for(d)){ KeyTap(vk); ev.theater_ui_input=true; } }
        else if(menu_nav_l_next_repeat_ms_ && now>=menu_nav_l_next_repeat_ms_){ menu_nav_l_next_repeat_ms_=now+100ull; if(const auto vk=vk_for(d)){ KeyTap(vk); ev.theater_ui_input=true; } }
        menu_nav_r_=Dir::None; menu_nav_r_next_repeat_ms_=0;
        SetManualInteractionHeld114(right.a_down);
        // V66.115R2: contextual inspection/transfer UIs consume Back as an edge,
        // not as a continuously held Escape key. Horse inspection in particular
        // can ignore a held keyboard state. Mirror Theater's proven pulse policy:
        // tap -> one Back pulse on release; long hold -> one 700 ms failsafe pulse.
        if(right.b_down && !b_prev_down_){ b_down_since_=now; b_long_sent_=false; }
        if(right.b_down && !backRecoveryConsumedM1 && !b_long_sent_ && b_down_since_ && now-b_down_since_>=700ull){
            KeyTap(config_.pause); b_long_sent_=true; ev.theater_ui_input=true; ev.back_cancel=true;
            debug_trace::Write("V66.115R2-CONTEXT B HOLD 700ms -> native Escape/Back pulse");
        }
        if(!right.b_down && b_prev_down_){
            if(!b_long_sent_&&!backRecoveryConsumedM1){
                KeyTap(config_.pause); ev.theater_ui_input=true; ev.back_cancel=true;
                debug_trace::Write("V66.115R2-CONTEXT B TAP -> native Escape/Back pulse");
            }
            b_down_since_=0; b_long_sent_=false;
        }
        b_prev_down_=right.b_down;
        if(b_context_back_held_){ KeyUp(config_.pause); b_context_back_held_=false; }
        return ev;
    } else {
        menu_nav_l_=menu_nav_r_=Dir::None; menu_nav_l_next_repeat_ms_=menu_nav_r_next_repeat_ms_=0;
        if(b_context_back_held_&&(!back_context_visible||native_gamepad_actions_)){ KeyUp(config_.pause); b_context_back_held_=false; }
    }

    if(back_context_visible&&!native_gamepad_actions_){
        SetKeyHeld(config_.pause,right.b_down&&!backRecoveryConsumedM1,b_context_back_held_);
    }
    // V65.61: gameplay B keeps Quick Reply; contextual menus above own B hold/release. In a proven back context it is Back;
    // in ordinary gameplay it becomes Quick Reply/contextual talk below.
    // B's previous state must survive until the gameplay press-edge test below.
    // Back-only contexts consume that edge at the end without issuing Quick Speak.

    // V65.03b native-inspired VR layout:
    // LEFT stick = movement; L3 = sprint. RIGHT stick L/R = horizontal turn,
    // Up = jump, Down = crouch. R3 = Skip Time. RIGHT GRIP + RIGHT stick
    // temporarily owns smooth two-axis precision turn/bow aim.
    // LEFT TRIGGER = block; RIGHT TRIGGER = native attack/bow hold-release timing.
    // RIGHT THUMBREST TOUCH + LEFT stick is the utility layer: Left=primary weapon,
    // Right=secondary weapon, Down=VR menu, Up=Map hold. The touch itself has no action.
    // LEFT GRIP is fully physical (F5/grabs/support/holsters). X tap=Torch / hold=Visor.
    constexpr float move_dead=0.24f;
    constexpr float turn_dead=0.16f;
    // V66.108: only a touch+digital-stick chord suppresses locomotion. Resting on
    // the capacitive surface with a centered stick has no gameplay effect.
    const bool modifier=quick_chord_active;
    // Right Grip owns directional weapon/bow control only. Quick Reply moved to
    // B tap; grips are no longer overloaded with tap actions.
    const bool right_grip_down = right.grip > 0.55f;
    if(right_grip_down && !right_grip_prev_down_) {
        right_grip_since_=now; right_grip_used_as_modifier_=false;
    }
    if(right_grip_down && (std::fabs(right.thumb_x)>0.22f || std::fabs(right.thumb_y)>0.22f))
        right_grip_used_as_modifier_=true;
    if(!right_grip_down && right_grip_prev_down_) {
        right_grip_since_=0; right_grip_used_as_modifier_=false;
    }
    right_grip_prev_down_=right_grip_down;
    // V66.138: do not infer free-look ownership from a 550 ms hold/modifier
    // heuristic. The caller publishes the actual VRIK ownership every frame.
    // When VRIK is OFF, Right Grip immediately restores the proven two-axis
    // mouse-look path; when VRIK is active, Grip remains purely physical.
    const bool directional_mode = attack_direction_mouse_ && right_grip_free_look_enabled_ && right_grip_down;
    // V64.17 conservative combat-activity gate for the display-mode chord.
    // This is intentionally fail-safe rather than claiming a native KCD2 combat-state hook:
    // attack/block or weapon-direction input blocks LG+R3 for 3 seconds.
    if(right.trigger>0.20f || left.trigger>0.20f || directional_mode) last_combat_input_ms_=now;
    const Dir ld=StickDir(left.thumb_x,left.thumb_y);
    const Dir rd=StickDir(right.thumb_x,right.thumb_y);

    if(!modifier && !native_left_stick_) {
        float move_x=left.thumb_x,move_y=left.thumb_y;
        if(hmd_walking_direction_&&hmd_walking_yaw_valid_&&std::isfinite(hmd_walking_yaw_rad_)){
            const float c=std::cos(hmd_walking_yaw_rad_),s=std::sin(hmd_walking_yaw_rad_);
            const float x=c*move_x-s*move_y,y=s*move_x+c*move_y;
            move_x=std::clamp(x,-1.0f,1.0f);move_y=std::clamp(y,-1.0f,1.0f);
        }
        SetKeyHeld(config_.move_forward, move_y >  move_dead, w_);
        SetKeyHeld(config_.move_back, move_y < -move_dead, s_);
        SetKeyHeld(config_.move_left, move_x < -move_dead, a_);
        SetKeyHeld(config_.move_right, move_x >  move_dead, d_);
    } else {
        SetKeyHeld(config_.move_forward,false,w_); SetKeyHeld(config_.move_left,false,a_); SetKeyHeld(config_.move_back,false,s_); SetKeyHeld(config_.move_right,false,d_);
    }
    SetKeyHeld(config_.sprint, native_gamepad_actions_ ? false : left.thumbstick_down, shift_);
    // V66.162: XI has no physical R3 special-attack mapping (its r3 field
    // is RS-down crouch). Use the established F action-map transport exactly
    // once in either mode; Grip remains available for holding the weapon.
    // This requests the native secondary action; it does NOT force an armed kick.
    // V66.178R21: a back-context map being enabled during combat is not a reason to swallow the click (menus and
    // minigames are still excluded); every click is logged with its gating so the next log can say why.
    const bool r3SpecialEligible=!authored_minigame&&r3Edge162&&!modifier&&!left.thumbstick_down;
    if(r3Edge162){char m[200]{};std::snprintf(m,sizeof(m),"V66.178R21 R3_EDGE eligible=%d authoredMinigame=%d modifier=%d l3=%d backContext=%d menu=%d",r3SpecialEligible?1:0,authored_minigame?1:0,modifier?1:0,left.thumbstick_down?1:0,back_context_visible?1:0,mis_menu_visible?1:0);diagnostic_trace::Write(m);}
    if(r3SpecialEligible) {
        if(!native_special168_)KeyTap(0x46 /* explicit legacy default-F transport */);
        ev.special_attack_requested=true;
        debug_trace::Write("V66.176-TOUCH R3 -> mode-owned special request; native or F4 procedural route; no accepted-hit claim");
    }

    // V66.108 capacitive utility layer. Left/Right preserve the existing semantic
    // quick-weapon actions. Up/Down stay reserved for Map/VR-menu and are never
    // emitted as gameplay D-pad directions by this keyboard/mouse bridge.
    const Dir utility_dir = modifier ? ld : Dir::None;
    if(utility_dir != quick_dir_prev_) {
        if(utility_dir==Dir::Up) {
            // The actual map action is emitted only by the 0.5 s hold gate above.
        } else if(utility_dir==Dir::Left) {
            if(vrik_input_lab_stage_<3 && !native_gamepad_actions_) {
                weapon_slot_toggle_=1; KeyTap(config_.weapon_slot1);
                debug_trace::Write("V66.108-TOUCH R-THUMBREST+LS-LEFT -> primary weapon/D-pad-left semantic");
            } else debug_trace::Write("V66.108-F9-INPUT suppressed R-THUMBREST+LS-LEFT primary weapon semantic");
        } else if(utility_dir==Dir::Right) {
            if(vrik_input_lab_stage_<3 && !native_gamepad_actions_) {
                weapon_slot_toggle_=2; KeyTap(config_.weapon_slot2);
                debug_trace::Write("V66.108-TOUCH R-THUMBREST+LS-RIGHT -> secondary weapon/D-pad-right semantic");
            } else debug_trace::Write("V66.108-F9-INPUT suppressed R-THUMBREST+LS-RIGHT secondary weapon semantic");
        }
        quick_dir_prev_=utility_dir;
    }

    // While RIGHT GRIP is held, RIGHT stick becomes the same smooth two-axis
    // directional path previously proven comfortable for bow aim (50% rate).
    if(turn_last_ms_==0) turn_last_ms_=now;
    const std::uint64_t elapsed_ms=std::min<std::uint64_t>(now-turn_last_ms_,50ull);
    turn_last_ms_=now;
    constexpr float full_turn_counts_per_sec=1320.0f;
    // V65.03: HMD-driven head aim is retired. This mouse path is only the
    // user's explicit Right-Grip + stick precision turn/bow-aim control; HMD
    // free-look never injects gameplay look motion here.
    if (directional_mode && elapsed_ms>0) {
        const float ax=CurvedAxis(right.thumb_x,0.10f);
        const float ay=CurvedAxis(right.thumb_y,0.10f);
        constexpr float bow_rate = full_turn_counts_per_sec * 0.50f;
        turn_fraction_ += ax * bow_rate * (static_cast<float>(elapsed_ms)/1000.0f);
        bow_fraction_y_ += (-ay) * bow_rate * (static_cast<float>(elapsed_ms)/1000.0f);
        const long dx=static_cast<long>(std::trunc(turn_fraction_));
        const long dy=static_cast<long>(std::trunc(bow_fraction_y_));
        turn_fraction_ -= static_cast<float>(dx);
        bow_fraction_y_ -= static_cast<float>(dy);
        MouseMoveRelative(dx,dy);
        if(now-bow_last_log_ms_>=2000ull) {
            bow_last_log_ms_=now;
            char m[224]{}; std::snprintf(m,sizeof(m),
                "V61.32-DIRECTIONAL-AIM grip=%.2f stick=(%.3f,%.3f) curve=(%.3f,%.3f) mouse=(%ld,%ld) rate=%.0fcounts/s",
                right.grip,right.thumb_x,right.thumb_y,ax,ay,dx,dy,bow_rate);
            debug_trace::Write(m);
        }
    } else {
        bow_fraction_y_=0.0f;
        const float ax=right.thumb_x;
        // Outer edge: one deliberate horizontal pulse per deflection. KCD2's
        // current mouse bridge remains the fallback until a maintained native
        // XInput/action injector is proven; this is intentionally latched.
        // D37: XI readiness only owns smooth turning. Snap still emits exactly
        // one existing mouse pulse, while the gameplay XI yaw axis is withheld.
        if(native_right_stick_&&turn_mode_!=0) { turn_fraction_=0.0f; target_switch_latched_=false; } else if(turn_mode_==0){
            // V64.17 provisional snap-turn through the proven mouse bridge. Pulse size is
            // deliberately fixed for A/B comfort testing; native gamepad/camera action is next.
            if(snap_gameplay_d37 && std::fabs(ax)>=0.72f && !snap_turn_latched_){
                snap_turn_latched_=true;
                const long pulse=ax>0.0f?360:-360;
                MouseMoveRelative(pulse,0);
                char m[192]{}; std::snprintf(m,sizeof(m),"V64.17-SNAP-TURN side=%s mousePulse=%ld provisional",ax>0.0f?"right":"left",pulse); debug_trace::Write(m);
            } else if(snap_gameplay_d37 && std::fabs(ax)<=0.35f) snap_turn_latched_=false;
            turn_fraction_=0.0f; target_switch_latched_=false;
        } else {
            // V64.45: smooth turn is fully analog across the whole stick range.
            // The old >=0.88 outer-edge opponent-switch pulse stole the highest
            // part of the axis and made turn speed non-monotonic.
            target_switch_latched_=false;
            const float turn_axis=LinearAxis(ax,turn_dead);
            if(turn_axis!=0.0f && elapsed_ms>0) {
                constexpr float vr_turn_rate=full_turn_counts_per_sec*0.55f;
                turn_fraction_ += turn_axis * vr_turn_rate * (static_cast<float>(elapsed_ms)/1000.0f);
                const long dx=static_cast<long>(std::trunc(turn_fraction_));
                turn_fraction_ -= static_cast<float>(dx);
                MouseMoveRelative(dx,0);
                if(now-turn_last_log_ms_>=500ull) {
                    turn_last_log_ms_=now;
                    char m[208]{}; std::snprintf(m,sizeof(m),"V64.45-SMOOTH-TURN-LINEAR rightX=%.3f linear=%.3f dx=%ld rate=%.0fcounts/s",right.thumb_x,turn_axis,dx,vr_turn_rate);
                    debug_trace::Write(m);
                }
            } else if(turn_axis==0.0f) turn_fraction_=0.0f;
        }
    }

    // Directional selection is continuous under Right Grip above; the old
    // Right-Trigger + discrete mouse-pulse attack-direction branch is retired.
    attack_dir_prev_=Dir::None;

    // Combat buttons: RT is held natively as LMB so KCD2 owns tap/hold/heavy
    // timing. LT is block/guard through RMB; grips remain modifiers.
    const bool want_lmb=(native_gamepad_actions_ || vrik_input_lab_stage_>=1) ? false : (right.trigger>0.55f);
    if(want_lmb!=lmb_){ lmb_=want_lmb; MouseButton(true,lmb_); }
    const bool want_rmb=(native_gamepad_actions_ || vrik_input_lab_stage_>=2) ? false : (left.trigger>0.55f);
    if(want_rmb!=rmb_){ rmb_=want_rmb; MouseButton(false,rmb_); }

    SetManualInteractionHeld114(!native_gamepad_actions_&&right.a_down);
    if(!native_gamepad_actions_) {
        if(right.a_pressed) debug_trace::Write("V64.83-TOUCH A -> E primary action DOWN; hold-A remains held-E for contextual use/rob/pickpocket");
    }

    // V65.03b VR vertical-stick actions. Normal VR never uses the stick for
    // camera pitch: Up=Jump, Down=Crouch. Holding Right Grip transfers the whole
    // stick to smooth two-axis precision aim, so neither action can fire there.
    const Dir vertical_action_dir = directional_mode ? Dir::None : rd;
    if(!native_gamepad_actions_ && vertical_action_dir==Dir::Up && jump_dir_prev_!=Dir::Up) {
        KeyTap(config_.jump);
        debug_trace::Write("V65.03b-TOUCH RS-UP -> SPACE jump");
    }
    if(!native_gamepad_actions_ && vertical_action_dir==Dir::Down && crouch_dir_prev_!=Dir::Down) {
        KeyTap(config_.crouch);
        debug_trace::Write("V65.03b-TOUCH RS-DOWN -> C crouch");
    }
    jump_dir_prev_=vertical_action_dir;
    crouch_dir_prev_=vertical_action_dir;

    // V61.32 LEFT Y = horse/companion family (swapped from B to match headset instinct).
    if(left.y_down && !y_prev_down_) { y_down_since_=now; y_long_sent_=false; }
    if(left.y_down && !y_long_sent_ && y_down_since_ && now-y_down_since_>=500ull) {
        if(!native_gamepad_actions_){ KeyDown(config_.horse_dog); b_x_held_=true; debug_trace::Write("V61.32-TOUCH Y HOLD -> hold X companion/dog"); }
        else debug_trace::Write("V64.80-TOUCH Y HOLD -> native XI_Y owns companion/dog");
        y_long_sent_=true;
    }
    if(!left.y_down && y_prev_down_) {
        if(b_x_held_){ KeyUp(config_.horse_dog); b_x_held_=false; }
        else if(!y_long_sent_ && !native_gamepad_actions_){ KeyTap(config_.horse_dog); debug_trace::Write("V61.32-TOUCH Y SHORT -> X horse/companion context"); }
        y_down_since_=0; y_long_sent_=false;
    }
    y_prev_down_=left.y_down;

    // V66.109: plain Left Menu = Inventory. RIGHT thumbrest + Left Menu =
    // Pause/Exit. The chord is consumed so releasing it can never also open
    // Inventory. Use the proven KCD2 keyboard superaction regardless of XI state.
    if(left.menu_down && !left_menu_prev_down_) {
        left_menu_since_=now; left_menu_long_sent_=false;
    } // R58C2 pause chord was consumed before XI and UI arbitration.
    if(!left.menu_down && left_menu_prev_down_) {
        if(!left_menu_long_sent_){
            KeyTap(config_.inventory); ev.toggle_inventory_comfort=true;
            debug_trace::Write("V66.109-TOUCH Menu TAP -> native Inventory toggle request");
        }
        left_menu_since_=0; left_menu_long_sent_=false;
    }
    left_menu_prev_down_=left.menu_down;
    // R118: X/Square hold on the STEAMVR / PS VR2 layout (native pad, queued by mod.cpp) = the same native
    // Inventory request as the Left Menu tap above (SteamVR keeps the left Menu button for itself).
    if(inventoryRequest118&&!ev.toggle_inventory_comfort){
        KeyTap(config_.inventory); ev.toggle_inventory_comfort=true;
        debug_trace::Write("R118-TOUCH X HOLD -> native Inventory toggle request (layout STEAMVR/PSVR2)");
    }

    // B tap remains Quick Speak. The explicit 2s recovery owns that long press;
    // RIGHT thumbrest + Left Menu keeps its independent Pause/Exit shortcut.
    if(native_gamepad_actions_||back_context_visible)b_quick_reply_eligible175_=false;
    if(right.b_down&&!b_prev_down_){
        b_down_since_=now;b_long_sent_=false;
        b_quick_reply_eligible175_=!native_gamepad_actions_&&!back_context_visible&&!quick_touch;
    }
    if(!right.b_down&&b_prev_down_){
        // R113 (chatgpt R112): with FACE TOUCH the B touch is the modifier; releasing a plain B click still
        // sends Quick Speak unless a utility chord was active.
        if(b_quick_reply_eligible175_&&!backRecoveryConsumedM1&&
           (!quick_touch||(quick_access_r112::FaceTouch()&&!quick_chord_active))) {
            KeyTap(config_.quick_reply);debug_trace::Write("V66.175-TOUCH B TAP -> fallback Quick Speak; native XI unavailable");
        }
        b_quick_reply_eligible175_=false;b_down_since_=0;b_long_sent_=false;
    }
    b_prev_down_=right.b_down;

    return ev;
}

} // namespace kcd2vr
