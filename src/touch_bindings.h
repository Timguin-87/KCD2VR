#pragma once

#include "xr_input.h"
#include <cstdint>

namespace kcd2vr {

// V58.6 provisional Quest/Touch -> vanilla KCD2 keyboard/mouse bridge.
// Layout follows the requested test scheme while the native/ViGEm path is researched.
class TouchBindings {
public:
    struct Config {
        unsigned short move_forward='W', move_left='A', move_back='S', move_right='D';
        unsigned short sprint=160, crouch='C', jump=32, interact='E';
        unsigned short torch='R', visor='Z', horse_dog='X', inventory='I', pause=27;
        unsigned short quick_left='5', quick_right='6', weapon_slot1='1', weapon_slot2='2';
    };
    struct Events {
        bool toggle_mis_menu = false;
        bool toggle_inventory_comfort = false;
        bool toggle_pause_comfort = false;
    };

    void SetEnabled(bool enabled);
    bool Enabled() const { return enabled_; }
    void SetAttackDirectionMouseEmulation(bool enabled) { attack_direction_mouse_ = enabled; }
    Events Update(const ControllerState& left,const ControllerState& right,bool mis_menu_visible,bool theater_ui_visible=false);
    void SetConfig(const Config& config) { config_=config; }
    void Reset();

private:
    enum class Dir : int { None=0, Up=1, Down=2, Left=3, Right=4 };

    static Dir StickDir(float x, float y, float threshold = 0.62f);
    static void KeyDown(unsigned short vk);
    static void KeyUp(unsigned short vk);
    static void KeyTap(unsigned short vk);
    static void MouseButton(bool left, bool down);
    static void MouseMoveRelative(long dx, long dy);
    static void SetKeyHeld(unsigned short vk, bool want, bool& held);

    Config config_{};
    bool enabled_ = true;
    bool attack_direction_mouse_ = true;
    bool w_=false,a_=false,s_=false,d_=false,shift_=false,lmb_=false,rmb_=false;
    bool menu_prev_=false, y_prev_=false, x_prev_=false;
    bool y_prev_down_=false;
    std::uint64_t y_down_since_=0;
    bool y_long_sent_=false;
    bool b_prev_down_=false;
    std::uint64_t b_down_since_=0;
    bool b_long_sent_=false;
    bool b_x_held_=false; // reused for Y-hold companion X after V61.32 B/Y family swap
    bool target_switch_latched_=false;
    bool left_menu_prev_down_=false;
    std::uint64_t left_menu_since_=0;
    bool left_menu_long_sent_=false;
    Dir quick_dir_prev_=Dir::None;
    Dir quick_consumable_dir_=Dir::None;
    unsigned short quick_consumable_key_=0;
    Dir attack_dir_prev_=Dir::None;
    Dir jump_dir_prev_=Dir::None;
    std::uint64_t turn_last_ms_=0;
    std::uint64_t turn_last_log_ms_=0;
    float turn_fraction_=0.0f;
    float bow_fraction_y_=0.0f;
    std::uint64_t bow_last_log_ms_=0;
    int weapon_slot_toggle_=1;
};

} // namespace kcd2vr
