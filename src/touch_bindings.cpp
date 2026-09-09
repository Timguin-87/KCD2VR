#include "touch_bindings.h"
#include "debug_trace.h"

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
void TouchBindings::SetKeyHeld(unsigned short vk,bool want,bool& held)
{
    if(want==held) return;
    held=want; if(want) KeyDown(vk); else KeyUp(vk);
    char m[128]{}; std::snprintf(m,sizeof(m),"V58.6-INJECT held vk=0x%02X state=%s",vk,want?"down":"up"); debug_trace::Write(m);
}

void TouchBindings::SetEnabled(bool enabled)
{
    if(enabled_==enabled) return;
    if(!enabled) Reset();
    enabled_=enabled;
    debug_trace::Write(enabled ? "V58.6-TOUCH bindings ENABLED" : "V58.6-TOUCH bindings DISABLED; injected keys released");
}

void TouchBindings::Reset()
{
    SetKeyHeld(config_.move_forward,false,w_); SetKeyHeld(config_.move_left,false,a_); SetKeyHeld(config_.move_back,false,s_); SetKeyHeld(config_.move_right,false,d_);
    SetKeyHeld(config_.sprint,false,shift_);
    if(lmb_){ MouseButton(true,false); lmb_=false; }
    if(rmb_){ MouseButton(false,false); rmb_=false; }
    if(b_x_held_){ KeyUp(config_.horse_dog); b_x_held_=false; }
    menu_prev_=y_prev_=x_prev_=false;
    b_prev_down_=false; b_down_since_=0; b_long_sent_=false;
    y_prev_down_=false; y_down_since_=0; y_long_sent_=false;
    left_menu_prev_down_=false; left_menu_since_=0; left_menu_long_sent_=false;
    quick_dir_prev_=Dir::None; quick_consumable_dir_=Dir::None; attack_dir_prev_=Dir::None;
    if(quick_consumable_key_){ KeyUp(quick_consumable_key_); quick_consumable_key_=0; }
    jump_dir_prev_=Dir::None; target_switch_latched_=false;
    turn_last_ms_=0; turn_fraction_=0.0f; bow_fraction_y_=0.0f; turn_last_log_ms_=0; bow_last_log_ms_=0;
}

TouchBindings::Events TouchBindings::Update(const ControllerState& left,const ControllerState& right,bool mis_menu_visible,bool theater_ui_visible)
{
    Events ev{};
    if(!enabled_ || (!left.active && !right.active)) { Reset(); return ev; }

    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    static std::uint64_t last_state_log_ms=0;
    if(now-last_state_log_ms>=500ull) {
        last_state_log_ms=now;
        char m[704]{};
        std::snprintf(m,sizeof(m),
            "V58.9-TOUCH-STATE menu=%s L={active:%s stick:(%.3f,%.3f) click:%s trig:%.3f grip:%.3f X:%s Y:%s Menu:%s} R={active:%s stick:(%.3f,%.3f) click:%s trig:%.3f grip:%.3f[RESERVED] A:%s B:%s}",
            mis_menu_visible?"true":"false",
            left.active?"true":"false",left.thumb_x,left.thumb_y,left.thumbstick_down?"down":"up",left.trigger,left.grip,left.x_down?"down":"up",left.y_down?"down":"up",left.menu_down?"down":"up",
            right.active?"true":"false",right.thumb_x,right.thumb_y,right.thumbstick_down?"down":"up",right.trigger,right.grip,right.a_down?"down":"up",right.b_down?"down":"up");
        debug_trace::Write(m);
    }

    // X remains dedicated to the MIS book.
    if(left.x_pressed) {
        ev.toggle_mis_menu=true;
        debug_trace::Write("V58.6-TOUCH X -> toggle MIS menu");
    }

    if(mis_menu_visible) {
        SetKeyHeld(config_.move_forward,false,w_); SetKeyHeld(config_.move_left,false,a_); SetKeyHeld(config_.move_back,false,s_); SetKeyHeld(config_.move_right,false,d_);
        SetKeyHeld(config_.sprint,false,shift_);
        if(lmb_){MouseButton(true,false);lmb_=false;} if(rmb_){MouseButton(false,false);rmb_=false;}
        attack_dir_prev_=Dir::None; quick_dir_prev_=Dir::None; quick_consumable_dir_=Dir::None; jump_dir_prev_=Dir::None;
        if(quick_consumable_key_){ KeyUp(quick_consumable_key_); quick_consumable_key_=0; }
        turn_last_ms_=now; turn_fraction_=0.0f;
        return ev;
    }

    // V62.9: F6/theater UI owns the pointer and right trigger. Suppress all
    // gameplay movement/combat injection, but preserve the left Menu short/hold
    // semantics so Inventory/Pause can still be closed naturally.
    if(theater_ui_visible) {
        SetKeyHeld(config_.move_forward,false,w_); SetKeyHeld(config_.move_left,false,a_); SetKeyHeld(config_.move_back,false,s_); SetKeyHeld(config_.move_right,false,d_);
        SetKeyHeld(config_.sprint,false,shift_);
        if(lmb_){MouseButton(true,false);lmb_=false;} if(rmb_){MouseButton(false,false);rmb_=false;}
        if(quick_consumable_key_){ KeyUp(quick_consumable_key_); quick_consumable_key_=0; }
        quick_dir_prev_=quick_consumable_dir_=attack_dir_prev_=jump_dir_prev_=Dir::None;
        turn_last_ms_=now; turn_fraction_=bow_fraction_y_=0.0f;
        if(left.menu_down && !left_menu_prev_down_) { left_menu_since_=now; left_menu_long_sent_=false; }
        if(left.menu_down && !left_menu_long_sent_ && left_menu_since_ && now-left_menu_since_>=500ull) {
            KeyTap(config_.pause); left_menu_long_sent_=true; ev.toggle_pause_comfort=true;
        }
        if(!left.menu_down && left_menu_prev_down_) {
            if(!left_menu_long_sent_) { KeyTap(config_.inventory); ev.toggle_inventory_comfort=true; }
            left_menu_since_=0; left_menu_long_sent_=false;
        }
        left_menu_prev_down_=left.menu_down;
        return ev;
    }

    // V61.32 combat-oriented locomotion layout:
    // LEFT stick = forward/back + strafe; left stick click = sprint.
    // RIGHT stick inner range = reduced smooth turn, outer horizontal range = one
    // latched opponent-switch gesture; right stick click = crouch, Up = jump.
    // RIGHT GRIP + RIGHT stick = smooth two-axis weapon/bow direction.
    // LEFT TRIGGER = block; RIGHT TRIGGER = held native attack/heavy timing.
    // LEFT GRIP is the quick-action modifier: LEFT stick Up/Down = torch/visor;
    // RIGHT stick Left/Right = consumable slots 5/6, held to consume.
    constexpr float move_dead=0.24f;
    constexpr float turn_dead=0.16f;
    // V61.32: LT is dedicated block. Left grip becomes the optional quick-action
    // modifier so locomotion/block no longer compete for the same analog input.
    const bool modifier=left.grip>0.55f;
    // Right grip owns directional weapon/bow control. The same smooth mouse path
    // preserves the good bow aim while giving melee a gamepad-like direction modifier.
    const bool directional_mode = attack_direction_mouse_ && right.grip > 0.55f;
    const Dir ld=StickDir(left.thumb_x,left.thumb_y);
    const Dir rd=StickDir(right.thumb_x,right.thumb_y);

    if(!modifier) {
        SetKeyHeld(config_.move_forward, left.thumb_y >  move_dead, w_);
        SetKeyHeld(config_.move_back, left.thumb_y < -move_dead, s_);
        SetKeyHeld(config_.move_left, left.thumb_x < -move_dead, a_);
        SetKeyHeld(config_.move_right, left.thumb_x >  move_dead, d_);
    } else {
        SetKeyHeld(config_.move_forward,false,w_); SetKeyHeld(config_.move_left,false,a_); SetKeyHeld(config_.move_back,false,s_); SetKeyHeld(config_.move_right,false,d_);
    }
    SetKeyHeld(config_.sprint, left.thumbstick_down, shift_);
    if(right.thumbstick_pressed) { KeyTap(config_.crouch); debug_trace::Write("V58.9-TOUCH right-stick click -> C crouch"); }

    // V62.6: while LEFT GRIP is held, reserve horizontal RIGHT stick for the
    // two primary quick-consumable slots. KCD2 uses keyboard 5/6 for the first
    // two consumable slots, and consuming is natively a hold action. Keep the
    // selected key held for as long as the chord is held; release on centre,
    // vertical motion, direction change, or modifier release.
    Dir consume_dir=Dir::None;
    if(modifier) {
        const float ax=std::fabs(right.thumb_x), ay=std::fabs(right.thumb_y);
        constexpr float enter=0.62f;
        constexpr float release=0.42f;
        if(quick_consumable_dir_==Dir::None) {
            if(ax>=enter && ax>=ay) consume_dir=right.thumb_x<0.0f?Dir::Left:Dir::Right;
        } else {
            if(ax>=release && ax>=ay*0.90f) consume_dir=right.thumb_x<0.0f?Dir::Left:Dir::Right;
        }
    }
    const unsigned short want_consume_key = consume_dir==Dir::Left ? config_.quick_left :
                                             consume_dir==Dir::Right ? config_.quick_right : 0;
    if(want_consume_key!=quick_consumable_key_) {
        if(quick_consumable_key_) KeyUp(quick_consumable_key_);
        quick_consumable_key_=want_consume_key;
        if(quick_consumable_key_) {
            KeyDown(quick_consumable_key_);
            debug_trace::Write(quick_consumable_key_==config_.quick_left
                ? "V62.6-TOUCH LGRIP+R-LEFT -> hold 5 first/left quick consumable"
                : "V62.6-TOUCH LGRIP+R-RIGHT -> hold 6 second/right quick consumable");
        } else if(quick_consumable_dir_!=Dir::None) {
            debug_trace::Write("V62.6-TOUCH quick consumable chord released");
        }
    }
    quick_consumable_dir_=consume_dir;

    // Continuous camera/body yaw is on RIGHT X, deliberately reduced in VR.
    // While RIGHT GRIP is held, RIGHT stick becomes the same smooth two-axis
    // directional path previously proven comfortable for bow aim (50% rate).
    if(turn_last_ms_==0) turn_last_ms_=now;
    const std::uint64_t elapsed_ms=std::min<std::uint64_t>(now-turn_last_ms_,50ull);
    turn_last_ms_=now;
    constexpr float full_turn_counts_per_sec=1320.0f;
    if (modifier) {
        // Left Grip owns the quick-action chord; do not turn, target-switch,
        // jump, or feed attack-direction mouse motion from the right stick.
        turn_fraction_=0.0f; bow_fraction_y_=0.0f; target_switch_latched_=false;
    } else if (directional_mode && elapsed_ms>0) {
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
        if(now-bow_last_log_ms_>=500ull) {
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
        if(std::fabs(ax)>=0.88f) {
            if(!target_switch_latched_) {
                target_switch_latched_=true;
                const long pulse=ax>0.0f?96:-96;
                MouseMoveRelative(pulse,0);
                char m[192]{}; std::snprintf(m,sizeof(m),"V61.32-OPPONENT-SWITCH-GESTURE side=%s mousePulse=%ld latch=true",ax>0.0f?"right":"left",pulse); debug_trace::Write(m);
            }
            turn_fraction_=0.0f;
        } else {
            if(std::fabs(ax)<=0.58f) target_switch_latched_=false;
            const float turn_axis=CurvedAxis(ax,turn_dead);
            if(turn_axis!=0.0f && elapsed_ms>0) {
                constexpr float vr_turn_rate=full_turn_counts_per_sec*0.55f;
                turn_fraction_ += turn_axis * vr_turn_rate * (static_cast<float>(elapsed_ms)/1000.0f);
                const long dx=static_cast<long>(std::trunc(turn_fraction_));
                turn_fraction_ -= static_cast<float>(dx);
                MouseMoveRelative(dx,0);
                if(now-turn_last_log_ms_>=500ull) {
                    turn_last_log_ms_=now;
                    char m[208]{}; std::snprintf(m,sizeof(m),"V61.32-TURN rightX=%.3f curve=%.3f dx=%ld rate=%.0fcounts/s outerSwitch=.88",right.thumb_x,turn_axis,dx,vr_turn_rate);
                    debug_trace::Write(m);
                }
            } else if(turn_axis==0.0f) turn_fraction_=0.0f;
        }
    }

    // LEFT grip + LEFT stick: only torch / visor. Quick weapons are removed;
    // consumables now use LEFT grip + RIGHT stick Left/Right above. Locomotion
    // is released while the modifier is held, making these commands deterministic.
    if(modifier && ld!=quick_dir_prev_ && ld!=Dir::None) {
        switch(ld){
        case Dir::Up: KeyTap(config_.torch); debug_trace::Write("V61.32-TOUCH GRIP+L-UP -> R torch"); break;
        case Dir::Down: KeyTap(config_.visor); debug_trace::Write("V61.32-TOUCH GRIP+L-DOWN -> Z visor"); break;
        case Dir::Left: break;
        case Dir::Right: break;
        default: break;
        }
    }
    quick_dir_prev_=modifier ? ld : Dir::None;

    // V61.32 directional selection is continuous under Right Grip above; the
    // old Right-Trigger + discrete mouse-pulse attack-direction branch is retired.
    attack_dir_prev_=Dir::None;

    // V61.32 combat buttons: RT is held natively as LMB so KCD2 itself owns
    // tap/hold/heavy timing. LT is block/guard through RMB; grips are modifiers.
    const bool want_lmb=right.trigger>0.55f;
    if(want_lmb!=lmb_){ lmb_=want_lmb; MouseButton(true,lmb_); }
    const bool want_rmb=left.trigger>0.55f;
    if(want_rmb!=rmb_){ rmb_=want_rmb; MouseButton(false,rmb_); }

    if(right.a_pressed) { KeyTap(config_.interact); debug_trace::Write("V58.6-TOUCH A -> E interact/select"); }

    // V59.3: jump returns to the right-stick Up gesture. This preserves Left Y
    // for weapon slot/sheath control. Edge-trigger the dominant Up direction so
    // holding the stick does not spam jump. Attack-direction mode owns the stick
    // while the right trigger is active.
    const Dir jump_dir = (directional_mode || modifier) ? Dir::None : rd;
    if (jump_dir == Dir::Up && jump_dir_prev_ != Dir::Up) {
        KeyTap(config_.jump);
        debug_trace::Write("V59.3-TOUCH right-stick UP -> SPACE jump");
    }
    jump_dir_prev_ = jump_dir;

    // V61.32 LEFT Y = horse/companion family (swapped from B to match headset instinct).
    if(left.y_down && !y_prev_down_) { y_down_since_=now; y_long_sent_=false; }
    if(left.y_down && !y_long_sent_ && y_down_since_ && now-y_down_since_>=500ull) {
        KeyDown(config_.horse_dog); b_x_held_=true; y_long_sent_=true; debug_trace::Write("V61.32-TOUCH Y HOLD -> hold X companion/dog");
    }
    if(!left.y_down && y_prev_down_) {
        if(b_x_held_){ KeyUp(config_.horse_dog); b_x_held_=false; }
        else if(!y_long_sent_){ KeyTap(config_.horse_dog); debug_trace::Write("V61.32-TOUCH Y SHORT -> X horse/companion context"); }
        y_down_since_=0; y_long_sent_=false;
    }
    y_prev_down_=left.y_down;

    // Left Menu: short = inventory, hold >=0.5 s = pause/Escape.
    if(left.menu_down && !left_menu_prev_down_) { left_menu_since_=now; left_menu_long_sent_=false; }
    if(left.menu_down && !left_menu_long_sent_ && left_menu_since_ && now-left_menu_since_>=500ull) {
        KeyTap(config_.pause); left_menu_long_sent_=true; ev.toggle_pause_comfort=true; debug_trace::Write("V61.33-TOUCH Menu HOLD -> Escape/pause; immersive view retained (F6 manual only)");
    }
    if(!left.menu_down && left_menu_prev_down_) {
        if(!left_menu_long_sent_) { KeyTap(config_.inventory); ev.toggle_inventory_comfort=true; debug_trace::Write("V61.33-TOUCH Menu SHORT -> I inventory; immersive view retained (F6 manual only)"); }
        left_menu_since_=0; left_menu_long_sent_=false;
    }
    left_menu_prev_down_=left.menu_down;

    // V61.32 RIGHT B = weapon family. Short swaps slots 1/2; hold taps the
    // current slot again for the existing provisional draw/holster behavior.
    if(right.b_down && !b_prev_down_) { b_down_since_=now; b_long_sent_=false; }
    if(right.b_down && !b_long_sent_ && b_down_since_ && now-b_down_since_>=500ull) {
        KeyTap(weapon_slot_toggle_==1?config_.weapon_slot1:config_.weapon_slot2);
        b_long_sent_=true;
        char m[144]{}; std::snprintf(m,sizeof(m),"V61.32-TOUCH B HOLD -> draw/holster current weapon slot %d",weapon_slot_toggle_); debug_trace::Write(m);
    }
    if(!right.b_down && b_prev_down_) {
        if(!b_long_sent_) {
            weapon_slot_toggle_=weapon_slot_toggle_==1?2:1;
            KeyTap(weapon_slot_toggle_==1?config_.weapon_slot1:config_.weapon_slot2);
            char m[128]{}; std::snprintf(m,sizeof(m),"V61.32-TOUCH B SHORT -> weapon swap slot %d",weapon_slot_toggle_); debug_trace::Write(m);
        }
        b_down_since_=0; b_long_sent_=false;
    }
    b_prev_down_=right.b_down;

    return ev;
}

} // namespace kcd2vr
