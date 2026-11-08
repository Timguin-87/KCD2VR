#include "vrik/native_bridges168.h" // R96: VrWeaponKickR96 switch
#include "xr_eye_srgb_r114.h" // R114: VrEyeSrgbR114 (live_runtime_d40.inl)
#include "subsystems/alchemy/head_select_r116.h" // R116: VrAlchemyViewSelectR116 (live_runtime_d40.inl)
#include "vrik/eye_ray_r121.h" // R121: VrInteractionEyeRayR121 (live_runtime_d40.inl)
#include "vrik/view_select_r137.h" // R137: index-finger point ray for object selection
#include "vrik/melee_grip_r123.h" // R123: VrMeleeAuthoredGripR123 (live_runtime_d40.inl)
#include "vrik/prop_impact_sound_r124.h" // R124: VrPropImpactSoundR124 (live_runtime_d40.inl)
#include "vrik/swing_whoosh_r116.h" // R116: VrSwingWhooshR116 (live_runtime_d40.inl)
#include "subsystems/ranged_reload/stowed_hide_hw.h" // R121: VrHideBackWeapons / VrHideStowedAtStations (live_runtime_d40.inl)
#include "vrik/impale_death_speed_r106.h" // R106: VrImpaleFastDeathR106 switch
#include "core/cinematic_bars_r107.h" // R107: VrCinematicBarsR107 switch
#include "core/cutscene_signal_r132.h" // R132: view-system cutscene count (prologue cutscenes)
#include "vrik/release_combat_r92.h"
#include "subsystems/base/startup_graphics_d80.h"
#include "diagnostics/capture_ticket_d84.h"
#include "diagnostics/activity_links_d77.h"
#include "diagnostics/log_sink_d48.h"
#include "diagnostics/native_palette_d67r1.h"
#include "diagnostics/native_palette_view_d67r1.h"
#include "vrik/knife_pouch_d66.h"
#include "vrik/start_r111.h" // R111: calibrated = VRIK at start/load, start at Henry's eyes
#include "vrik/agency_scripted_r132.h" // R132: story-only action filters (prologue ride to Trosky) are not "scripted"
#include "quick_access_r112.h" // R113: quick-access modifier THUMBREST / FACE TOUCH
#include "controller_layout_r118.h" // R118: controller layout OPENXR / STEAMVR / PS VR2
#include "core/observed_load_recovery_r96.h" // R111: load episode for the start decision
#include "subsystems/masterstrike_combos/paired_signals_d62c.h" // D62C paired hit timing settings
#include "subsystems/masterstrike_combos/native_kill_anim_nf.h" // I10/NF
#include "subsystems/alchemy/reach_policy_craft1.h"
#include "core/player_window_h26.h"
#include "diagnostics/hmd_dodge_context_h24.h"
#include "subsystems/base/view_defaults_b2.h"
#include "subsystems/base/linked_fov_h26.h"
#include "subsystems/catalog.h"
#include "vrik/lab58.h"
#include "vrik/station_ws_j27.h" // J27 station torso + smith tool contact
#include "subsystems/smithing/anvil_a2.h" // default-off approximate anvil experiment
#include "vrik/finger_rig_w1.h" // W1 captured fist default
#include "vrik/elbow_pole_w1.h" // W1 relaxed elbow pole
#include "vrik/motion_trials_d67.h"
#include "vrik/claude_motion_d69.h"
#include "vrik/weapon_trials_d69.h"
#include "experiments/claude_bundle_d70.h"
#include "experiments/contact_handover_d71.h"
#include "vrik/dodge_window_d70.h"
#include "vrik/held_motion_d67.h"
#include "subsystems/crafting/options_d67.h"
#include "developer_mode.h"
#include "graphics_menu_options.h"
#include "stereo_upscaler_d81r2.h"
#include "native_upscaler_stereo_d82.h"
#include "native_dlss_stereo_d82.h"
#include "diagnostics/presentation_receipt_d86.h"
#include "upscaler_live_switch_d85.h" // D85 UPSCALERS
#include "vrik/frik_elbow_lab.h" // live Experiments elbow comparison
#include "vrik/alchemy_tool_contact_d55.h"
#include "vrik/camera_hold_e1.h" // E1 camera base hold (VrCameraBodyHoldMs)
#include "vrik/bow_arm_aim_r9.h" // R9 lane A: bow follows the chest->bow hand line (VrBowArmAim)
#include "vrik/combat_r9.h" // R9 fist reach / combo slot phase 2 / raised-hilt guard
#include "vrik/roomscale58.h"
#include "vrik/self_contact54.h"
#include "vrik/utility_gestures54.h"
#include "vrik/gore32.h" // C32 impale hang + decapitation settings
#include "vrik/visual_selection59c7f.h" // H50R2c free-hand smoothing settings
#include "vrik/smooth_lab_f2.h" // D39 shared saved baseline preference
#include "vrik/blood49.h" // C49 BLOODY FIGHTS setting
#include "vrik/gorepak50.h" // C50 VrDismemberment (gore pak)
#include "vrik/impale40.h" // C40 VrImpaleByArmour + INI depth tuning
#include "vrik/kick_style26.h" // C33 kick route + fallback leg
#include "build_identity.h"
#include "subsystems/base/startup_setup_s15.h"
#include "vrik/finger_tuning40.h"
#include "vrik/melee_tuning_c54.h"
#include "vrik/punch_travel_d65.h"
#include "vrik/two_hand_bias_d65.h"
#include "vrik/instant_activities_d72.h"
#include "vrik/candidate_defaults_d72.h"
#include "subsystems/ranged_reload/options_d65c.h"
#include "held_map_m1.h"
#include "held_map_feed_t2.h"
#include "native_script_t2.h"
#include "subsystems/doors/hand_only_h40.h"
#include "native_weapon_body_t4.h"
#include "native_physics_phase_t4.h"
#include "vrik/native_held_owner_t4_status.h"
#include "immersive_fov59c7g.h"
#include "activity_lens59c10r1.h"
#include "action_time_owner59c11.h"
#include "subsystems/experimental/claude_options_h8.h"
#include "subsystems/experimental/deepseek_options_h29.h"
#include "subsystems/masterstrike_combos/body_recipe_policy.h" // NR1 VrNpcReactsToBlade
#include "subsystems/masterstrike_combos/physical_finisher_cb1.h" // CB1 VrPhysicalComboFinisher
#include "vrik/hand_present_d61.h"
#include "diagnostics/performance_metrics_d61.h"
#include "diagnostics/motion_service_d68.h"
#include "subsystems/throw_drop/inventory_lifecycle_t8.h"
#include "vrik/swing_effort59c14.h" // R59.16C10R14
#include "vrik/visual_selection59c7f.h"
#include "vrik/gesture_talk59c5.h"
#include "subsystems/hud_ui_controls/runtime/quick_talk_native_h19.h"
#include "vrik/prop_drive5916c1.h"
#include "vrik/weapon_release_t1.h"
#include "vrik/live_attributes5912.h"
#include "vrik/motion_owned59c4.h"
#include "subsystems/direct/combat_router_h3.h"
#include "subsystems/direct/npc_tuning.h"
#include "subsystems/direct/npc_defense.h"
#include <climits>
#include "diagnostics/hotkeys168r1.h"
#include "diagnostics/hook_cost178r22.h" // V66.178R22c
#include "diagnostics/capture_control168.h"
#include "theater_exit113.h"
#include "theater_toggle_r124.h" // R124 F6 Theater <-> Immersive toggle
#include "ui_native_state.h"
#include "ui_menu_camera59c2.h"
#include "ui_hud_geometry.h"
#include "ui_hud_r112.h" // R112 combat crosshair size + quick-slot fade
#include "ui_menu_router.h"
#include "ui_scene_presentation.h"
#include "ui_native_catalogue.h"
#include "ui_menu_input.h"
// mod.cpp

#include "mod.h"
#include "diagnostics/combat_flow_h12.h"
#include "ui_pause_blur124r1.h"
#include "ui_pause_schedule59c10r2.h"
#include "debug_trace.h"
#include "diagnostic_trace.h"
#include "diagnostics/xr_timing165.h"
#include "diagnostics/frame_summary_d30.h" // D30 F2 FRAME / F7 SUMMARY
#include "diagnostics/physics_stage_d42.h"
#include "diagnostics/interaction_snapshot_d42.h"
#include "subsystems/experimental/free_targeting_d42.h"
#include "subsystems/ranged_reload/arrow_hold_d48.h"
#include "vrik/fist_stroke_d49.h"
#include "subsystems/ranged_reload/reload_diagnostics_d48.h"
#include "vrik/align_veto_receipts24.h"
#include "diagnostics/hook_cost178r22.h"
#include "stereo_archaeology.h"
#include "native_stereo_outer_lab.h"
#include "native_postaa_repair_v6632.h"
#include "native_cloud_right_v6641.h"
#include "native_stage_bisect_v6644.h"
#include "native_stereo_preflight_v6602.h"
#include "native_svo_prev_per_eye_v6649.h"
#include "native_svo_right_v6654.h"
#include "native_taa_probe_v6657.h"
#include "native_taa_pair_epoch_v6661.h"
#include "native_water_reflection_right_v6663.h"
#include <limits>
#include <cmath>
#include "stereo_design_capture_v6606.h"
#include "native_vr_control_plane_lab.h"
#include "frame_sync.h"
#include "recursive_view_probe.h"
#include "vrik_bridge.h"
#include "frik_pose_probe.h"
#include "subsystems/physics/native_grab_input60.h"
#include "subsystems/physics/grab_over_talk_ds.h" // DS-2026-10-23: the grab owns the hand near a living NPC // DS-F88: grip-evidence resolver (header-only, inline-shared)
#include "vrik/physical_interaction.h"
#include "vrik/interaction_cone57c1.h"
#include "vrik/held_interaction114.h"
#include "vrik/pickup_use19c3.h"
#include "subsystems/physics/runtime/native_grab_producer60.h"
#include "vrik/interaction_ownership111.h"
#include "vrik/default_migration111.h"
#include "vrik/shortcut_cycle167r1.h"
#include "vrik/combat_rules178r10.h"
#include "vrik/vr_combat_ownership111.h" // V66.178R25 hideNativeClips
#include "vrik/direct_counter39.h" // C39 VrDirectGuardOutOfStagger
#include "vrik/counter_stab40.h" // C40 VrDirectCounterStab*
#include "vrik/master_gate47.h" // C47 VrMasterStrikeStabOnly / VrMasterStrikeStabEnergy
#include "vrik/dirblock39.h" // C39 VrDirectNpcDirectionalBlock
#include "vrik/feint_hybrid_fe.h" // D34 FE VrFeintHybrid
#include "vrik/workstation178r20c1.h"
#include "vrik/semi_workstations46.h"
#include "core/book_gesture46.h"
#include "vrik/book_input46.h"
#include "diagnostics/interaction_receipt59c10.h"
#include "diagnostics/interaction_cycle_h4.h"
#include "animation_filter.h"
#include "interaction_alignment.h"
#include "retail_build112.h"
#include "head_aim120r3.h"
#include "activity_input163.h"
#include "vrik/hand_point123r1.h"
#include "diagnostics/lab_state.h"
#include "flashui_probe.h"
#include "ui_contextual_hud162r1.h"
#include "ui_hud_transition170.h"
#include "ui_palm171.h"
#include "subsystems/hud_ui_controls/prompt_policy_d80.h"
#include "ui_gameplay_hud54.h"
#include "ui_hud_work59c7.h"
#include "ui_hud_experiment36c5.h"
#include "ui_adaptive_hud57c1.h"
#include "ragdoll_lease59.h"
#include "dialogue_camera_override.h"
#include "npc_lookat_hmd.h"
#include "subsystems/dialogues/lively_npc_d73.h"
#include "subsystems/roomscale/settings_d76.h"
#include "vrik/riding_vrik_d76.h"
#include "vrik/finisher_upper_d83.h" // D83R1 tracked head turns during native paired actions
#include "vrik/blade_zone_d84.h" // D84 blade zone (attack side from the blade stroke)
#include "subsystems/dice/hand_select_d85.h" // D85 dice: point to choose
#include "vrik/static_grab_r89.h" // R90 (R89 pickup): small props in the hand
#include "vrik/pickup_use_fallback_r120.h" // R120: run the item Use when the game interactor did not
#include "vrik/gesture_talk_policy_d33.h" // D33 gesture talk off + headset glyphs
#include "subsystems/physics/f9_status_fd.h" // D30 F9 visible status
#include "npc_look_policy63.h" // D52 Native/FollowHMD live preference
#include "subsystems/masterstrike_combos/combo_zone_assist_cz.h" // I12 VrComboZoneAssist
#include "subsystems/masterstrike_combos/combo_recovery_hold_r14.h" // I12/N2 VrComboRecoveryHold
#include "subsystems/masterstrike_combos/combo_carrier_cb.h" // D30-CMB combo carrier keep-alive
#include "dialogue_pose47c1.h"
#include "subsystems/dialogues/ratio_strips_d79.h"
#include "vrik/reading_pose32.h"
#include "dialogue_presentation19c2.h"
#include "activity_fader47c1.h"
#include "focus_zoom58c2.h"
#include "core/reading_pages42.h"
#include "core/book_pages45.h"
#include "vrik/alchemy_input19c4.h"
#include "vrik/alchemy_palm_lock27.h"
#include "hud_xml_mod.h"
#include "depth_probe.h"
#include "prompt_device_lock.h"
#include "subsystems/hud_ui_controls/runtime/glyph_apply_r129.h" // R129 PS/Xbox button art: never behind the CVar lease, re-sent on every UI change
#include "subsystems/alchemy/finish_observer_cb1.h" // CB1
#include "subsystems/throw_drop/grip_stow_cb1.h" // CB1
#include "subsystems/throw_drop/stow_button_h41.h"
#include "native_input_look_lab.h"
#include "vrik/attack_xi178r8.h"
#include "native_vr_pad_premap.h"
#include "shop_state_probe.h"
#include "action_map_probe.h"
#include "hud_layer_v6664.h"
#include "combat_diag_probe.h"

#include <DetourModKit/logger.hpp>
#include <DetourModKit/config.hpp>

#include <chrono>
#include <thread>
#include "core/async_save.h" // C25: coalescing background INI writer
#include "vr_exposure26.h" // C26d head-steady eye adaptation
#include "graphics_owner31.h" // C31 scoped, reversible native-CVar leases
#include "diagnostics/graphics_receipts32.h"
#ifdef KCD2VR_PROXY_DINPUT8
#include "proxy_dinput8_29.h"
#include "diagnostics/startup_batch32.h"
#endif
#include "ui_inventory_early_exit25.h" // C25: Inventory Theater ends on the first native close reaction
#include <windows.h>
#include "core/config_file_guard_d62.h"
#include "core/default_ini_r118.h" // R118: built-in default KCD2VR.ini
#include "diagnostics/unified_log_d81.h"
#include "subsystems/interactions/misc_gestures_d59.h"
#include "subsystems/doors/hand_only_h40_service.inl"
#include <mmsystem.h>
static bool g_timer_period_1ms_59c7a = false; // R59.16C7A
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <utility>
#include <algorithm>
#include <atomic>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>

namespace kcd2vr {
namespace { std::atomic<void*> nativeUpscalerCvarD82{nullptr}; }

using DetourModKit::log;
namespace config = DetourModKit::config;


namespace {

// Debug marker: KCD2VR-V65.38b-SAME-FRAME-DUAL-PASS-COMPILE-FIX.
// Built from V65.02b; retired hypotheses remain provenance-only unless a gated ladder explicitly enables them.
bool GetLoadedModuleSiblingPath(const char* filename, char* out, DWORD capacity) noexcept
{
    if (!filename || !out || capacity == 0)
        return false;

    HMODULE self = nullptr;
    if (!GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(&GetLoadedModuleSiblingPath),
            &self))
        return false;

    DWORD len = GetModuleFileNameA(self, out, capacity);
    if (len == 0 || len >= capacity)
        return false;

    char* slash = std::strrchr(out, '\\');
    if (!slash)
        slash = std::strrchr(out, '/');
    if (!slash)
        return false;

    *(slash + 1) = '\0';
    const size_t used = std::strlen(out);
    const size_t name_len = std::strlen(filename);
    if (used + name_len + 1 > capacity)
        return false;

    std::memcpy(out + used, filename, name_len + 1);
    return true;
}


bool GetGameRootUserCfgPath(char* out, DWORD capacity) noexcept
{
    if (!out || capacity == 0) return false;
    HMODULE self = nullptr;
    if (!GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(&GetGameRootUserCfgPath),
            &self)) return false;
    char module_path[MAX_PATH * 2]{};
    const DWORD len = GetModuleFileNameA(self, module_path, static_cast<DWORD>(sizeof(module_path)));
    if (len == 0 || len >= sizeof(module_path)) return false;
    std::string path(module_path, len);
    std::string lower = path;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    const std::string marker = "\\bin\\";
    const size_t pos = lower.rfind(marker);
    if (pos == std::string::npos) return false;
    const std::string cfg = path.substr(0, pos + 1) + "user.cfg";
    if (cfg.size() + 1 > capacity) return false;
    std::memcpy(out, cfg.c_str(), cfg.size() + 1);
    return true;
}

bool SaveGraphicsSettingsToUserCfgR27(const std::vector<std::pair<std::string,int>>& values) noexcept
{
    char path[MAX_PATH * 2]{};
    if (!GetGameRootUserCfgPath(path, static_cast<DWORD>(sizeof(path)))) return false;

    std::vector<std::string> lines;
    {
        std::ifstream in(path, std::ios::binary);
        const DWORD attrs=GetFileAttributesA(path);
        if(attrs!=INVALID_FILE_ATTRIBUTES&&!in)return false;
        if(attrs==INVALID_FILE_ATTRIBUTES&&GetLastError()!=ERROR_FILE_NOT_FOUND&&GetLastError()!=ERROR_PATH_NOT_FOUND)return false;
        std::string line;
        while (std::getline(in, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            lines.push_back(line);
        }
        if(in.bad())return false;
    }

    lines=kcd2vr::graphics_owner31::updateUserCfgR27(lines,values);

    const std::string tmp = std::string(path) + ".kcd2vr.tmp";
    {
        std::ofstream out_file(tmp, std::ios::binary | std::ios::trunc);
        if (!out_file) return false;
        for (const auto& line : lines) {
            out_file << line << "\r\n";
        }
        out_file.flush();
        if (!out_file.good()) return false;
    }
    if (!MoveFileExA(tmp.c_str(), path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileA(tmp.c_str());
        return false;
    }
    return true;
}

bool SaveSourceResolutionToUserCfg(int width,int height,char* log_buf,std::size_t log_cap) noexcept
{
    if(width<640||height<640||width>4096||height>4096)return false;
    const bool ok=SaveGraphicsSettingsToUserCfgR27({{"r_Width",width},{"r_Height",height}});
    if(ok&&log_buf&&log_cap)std::snprintf(log_buf,log_cap,"V63.1-SOURCE-RES saved %dx%d to game-root user.cfg; applies on next KCD2 launch; no live r_Width/r_Height write",width,height);
    return ok;
}

bool ReadIniBoolExplicit(const char* path, const char* section, const char* key, bool fallback) noexcept
{
    if (!path || !section || !key)
        return fallback;

    char raw[64]{};
    const char* fallback_text = fallback ? "true" : "false";
    GetPrivateProfileStringA(section, key, fallback_text, raw, static_cast<DWORD>(sizeof(raw)), path);

    // Trim leading/trailing ASCII whitespace.
    char* begin = raw;
    while (*begin == ' ' || *begin == '\t' || *begin == '\r' || *begin == '\n') ++begin;
    char* end = begin + std::strlen(begin);
    while (end > begin && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\r' || end[-1] == '\n')) --end;
    *end = '\0';

    for (char* p = begin; *p; ++p)
        if (*p >= 'A' && *p <= 'Z') *p = static_cast<char>(*p - 'A' + 'a');

    const bool value =
        std::strcmp(begin, "true") == 0 ||
        std::strcmp(begin, "1") == 0 ||
        std::strcmp(begin, "yes") == 0 ||
        std::strcmp(begin, "on") == 0;

    char trace[256]{};
    std::snprintf(trace, sizeof(trace),
        "explicit INI bool: [%s] %s raw='%s' parsed=%s",
        section, key, begin, value ? "true" : "false");
    debug_trace::Write(trace);
    return value;
}


int ReadIniIntExplicit(const char* path, const char* section, const char* key, int fallback) noexcept
{
    if (!path || !section || !key)
        return fallback;
    char fb[64]{};
    std::snprintf(fb, sizeof(fb), "%d", fallback);
    char raw[64]{};
    GetPrivateProfileStringA(section, key, fb, raw, static_cast<DWORD>(sizeof(raw)), path);
    char* end = nullptr;
    const long parsed = std::strtol(raw, &end, 10);
    const int value = (end && end != raw) ? static_cast<int>(parsed) : fallback;
    char trace[256]{};
    std::snprintf(trace, sizeof(trace), "explicit INI int: [%s] %s raw='%s' parsed=%d", section, key, raw, value);
    debug_trace::Write(trace);
    return value;
}

// V66.161 VRIK hand aim pitch: latest published interaction hand ray pitch (radians, up+).

float ReadIniFloatExplicit(const char* path, const char* section, const char* key, float fallback) noexcept
{
    if (!path || !section || !key)
        return fallback;
    char fb[64]{};
    std::snprintf(fb, sizeof(fb), "%.9g", static_cast<double>(fallback));
    char raw[64]{};
    GetPrivateProfileStringA(section, key, fb, raw, static_cast<DWORD>(sizeof(raw)), path);
    char* end = nullptr;
    const float parsed = std::strtof(raw, &end);
    const float value = (end && end != raw) ? parsed : fallback;
    char trace[256]{};
    std::snprintf(trace, sizeof(trace), "explicit INI float: [%s] %s raw='%s' parsed=%.6f", section, key, raw, value);
    debug_trace::Write(trace);
    return value;
}


// V46.1 direct CryEngine CVar access, recovered from WHGame.dll itself.
// WHGame code at 0x245F33D obtains r_Width/r_Height through IConsole::GetCVar
// (vslot +0xB8) and sets the integer value through ICVar vslot +0x38.
// Float readers/setters are independently observed at +0x20/+0x40.
// Keeping all SEH pointer touches in tiny leaf helpers avoids MSVC C2712.
constexpr std::uintptr_t kIConsoleGlobalRva = 0x492D8A8ull;
constexpr std::uint32_t kSupportedWhTimestampV6143 = 0x6A350E20u;

bool IsSupportedWhGameBuildV6143() noexcept
{
    const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
    if (!base) return false;
    __try {
        const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE) return false;
        const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
        return nt->Signature == IMAGE_NT_SIGNATURE && nt->FileHeader.TimeDateStamp == kSupportedWhTimestampV6143;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

void* SafeGetCryConsole() noexcept
{
    const auto wh = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
    if (!wh) return nullptr;
    __try { return *reinterpret_cast<void**>(wh + kIConsoleGlobalRva); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

void* SafeConsoleGetCVar(void* console, const char* name) noexcept
{
    if (!console || !name) return nullptr;
    __try {
        auto** vt = *reinterpret_cast<void***>(console);
        if (!vt || !vt[0xB8 / sizeof(void*)]) return nullptr;
        using Fn = void*(__fastcall*)(void*, const char*);
        return reinterpret_cast<Fn>(vt[0xB8 / sizeof(void*)])(console, name);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

// V64.19C native Dynamic Detector agency bridge, recovered from this exact
// WHGame.dll build. Chain: *(WHGame+0x549D328) -> CCryAction;
// CCryAction+0x530 -> CActionMapManager; vtable+0x108 ->
// IsFilterEnabled(const char*) returning bool in AL. Every hop is validated.
constexpr std::uintptr_t kCryActionGlobalRva = 0x549D328ull;
constexpr std::uintptr_t kCryActionVftRva = 0x40472D0ull;
constexpr std::uintptr_t kActionMapManagerVftRva = 0x4048018ull;
constexpr std::uintptr_t kActionMapManagerOffset = 0x530ull;
constexpr std::uintptr_t kActionMapIsFilterEnabledSlot = 0x108ull;
constexpr std::uintptr_t kActionMapIsFilterEnabledRva = 0x35742A4ull;

void* SafeResolveActionMapManagerV6419C() noexcept
{
    if (!IsSupportedWhGameBuildV6143()) return nullptr;
    const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
    if (!base) return nullptr;
    __try {
        void* cry = *reinterpret_cast<void**>(base + kCryActionGlobalRva);
        if (!cry) return nullptr;
        if (*reinterpret_cast<std::uintptr_t*>(cry) != base + kCryActionVftRva) return nullptr;
        void* mgr = *reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(cry) + kActionMapManagerOffset);
        if (!mgr) return nullptr;
        auto** vt = *reinterpret_cast<void***>(mgr);
        if (!vt || reinterpret_cast<std::uintptr_t>(vt) != base + kActionMapManagerVftRva) return nullptr;
        void* fn = vt[kActionMapIsFilterEnabledSlot / sizeof(void*)];
        if (reinterpret_cast<std::uintptr_t>(fn) != base + kActionMapIsFilterEnabledRva) return nullptr;
        return mgr;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

bool SafeActionMapIsFilterEnabledV6419C(void* mgr, const char* name, bool* enabled) noexcept
{
    if (enabled) *enabled = false;
    if (!mgr || !name || !enabled || !IsSupportedWhGameBuildV6143()) return false;
    const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
    if (!base) return false;
    __try {
        auto** vt = *reinterpret_cast<void***>(mgr);
        if (!vt || reinterpret_cast<std::uintptr_t>(vt) != base + kActionMapManagerVftRva) return false;
        void* target = vt[kActionMapIsFilterEnabledSlot / sizeof(void*)];
        if (reinterpret_cast<std::uintptr_t>(target) != base + kActionMapIsFilterEnabledRva) return false;
        using Fn = bool(__fastcall*)(void*, const char*);
        *enabled = reinterpret_cast<Fn>(target)(mgr, name);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

struct NativeAgencyStateV6419C { bool known=false; int count=-1; std::uint64_t mask=0; };

static constexpr const char* kAgencyFiltersV6419C[] = {
    "no_menu", "no_inventory", "no_interaction", "no_apse_controller",
    "no_attack", "no_camera", "no_horizontal_camera", "no_focus",
    "no_jump", "no_crouch", "no_move", "no_horse",
    "no_horse_dismount", "no_horse_mount", "no_qam_weapons",
    "no_open_skiptime", "no_stealth_kill", "no_dog", "no_fast_travel",
    "no_torch", "no_drop_item", "no_standUp", "no_follow", "no_distract",
    "no_grab_body", "no_knock_out", "no_carryitem_put", "no_cart_dismount",
    "no_companion_bond", "no_horse_inspect", "no_horse_saddle_bond"
};

int SafeCVarGetInt(void* cvar, int fallback) noexcept;
bool SafeCVarSetInt(void* cvar, int value) noexcept;

// V64.28 native "mouse-look from head" lab. Same exact-build look-controller
// chain as V64.22B, but WRITE mode gives native KCD2 look sole ROTATION
// authority while CameraHook keeps OpenXR POSITION enabled. No residual HMD
// rotation and no smoothing are used in the view path.
constexpr std::uintptr_t kCryActionActionGameOffsetV6422B = 0x88ull;
constexpr std::uintptr_t kActionGameLocalActorOffsetV6422B = 0xA40ull;
constexpr std::uintptr_t kPlayerLookControllerOffsetV6422B = 0x238ull;
constexpr std::uintptr_t kLookPitchOffsetV6422B = 0x08ull;
constexpr std::uintptr_t kLookYawOffsetV6422B = 0x10ull;
constexpr std::uintptr_t kLookQuatOffsetV6422B = 0x24ull;
constexpr std::uintptr_t kLookYaw2OffsetV6422B = 0x44ull;
constexpr std::uintptr_t kLookPitch2OffsetV6422B = 0x48ull;

struct LookQuatV6422B { float x,y,z,w; };





struct HeadLookLabResultV6428 { bool headValid=false; bool stateOk=false; bool wrote=false; bool ownsRotation=false; void* controller=nullptr; };



NativeAgencyStateV6419C ReadNativeAgencyStateV6419C() noexcept
{
    NativeAgencyStateV6419C out{};
    void* mgr=SafeResolveActionMapManagerV6419C();
    if (!mgr) return out;
    int count=0; std::uint64_t mask=0;
    for (std::size_t i=0;i<sizeof(kAgencyFiltersV6419C)/sizeof(kAgencyFiltersV6419C[0]);++i) {
        bool enabled=false;
        if (!SafeActionMapIsFilterEnabledV6419C(mgr,kAgencyFiltersV6419C[i],&enabled)) return out;
        if (enabled) { ++count; mask|=(1ull<<i); }
    }
    out.known=true; out.count=count; out.mask=mask; return out;
}

// R132: CViewSystem::m_cutsceneCount, -1 = not readable (wrong build / not created yet). see core/cutscene_signal_r132.h
int ReadViewSystemCutsceneCountR132() noexcept
{
    if (!IsSupportedWhGameBuildV6143()) return -1;
    const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if (!base) return -1;
    namespace cs = kcd2vr::cutscene_signal_r132;
    __try {
        void* cry = *reinterpret_cast<void**>(base + kCryActionGlobalRva);
        if (!cry || *reinterpret_cast<std::uintptr_t*>(cry) != base + kCryActionVftRva) return -1;
        const auto view = *reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(cry) + cs::kViewSystemOffset);
        if (!view) return -1;
        const auto vt = *reinterpret_cast<std::uintptr_t*>(view);
        if (vt != base + cs::kViewSystemVft) return -1;
        if (*reinterpret_cast<std::uintptr_t*>(vt + cs::kIsPlayingCutSceneSlot) != base + cs::kIsPlayingCutSceneRva) return -1;
        return *reinterpret_cast<volatile int*>(view + cs::kCutsceneCountOffset);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

void FormatAgencyMaskV6419C(std::uint64_t mask, char* out, std::size_t cap) noexcept
{
    if (!out || cap==0) return; out[0]='\0'; std::size_t used=0;
    for (std::size_t i=0;i<sizeof(kAgencyFiltersV6419C)/sizeof(kAgencyFiltersV6419C[0]);++i) {
        if (!(mask&(1ull<<i))) continue;
        const int n=std::snprintf(out+used,cap-used,"%s%s",used?"|":"",kAgencyFiltersV6419C[i]);
        if (n<=0) break; const auto nn=static_cast<std::size_t>(n);
        if (nn>=cap-used) { out[cap-1]='\0'; break; } used+=nn;
    }
    if (used==0) std::snprintf(out,cap,"none");
}

// C57R1: exact-retail command entry, never the retired +0xD0 print slot.
// Console Lua remains dev-gated in retail even at the correct ExecuteString.
// Reject script commands explicitly instead of claiming glyph/layout execution.
// Existing native FlashUI ownership remains the HUD path; do not revive whole-
// root MasterSwitch transforms or run UI Lua on an arbitrary calling thread.
// Legacy CSV remains readable; exact Picking qualification is fixed in the
// native owner. NATIVE/STAGED HOLD uses explicit preference wiring, never a
// fake CSV exclusion or the unsafe C60 INSTANT AddItem route.
static std::string g_owned_fragments60;
static void ApplyOwnedFragments60() noexcept {
    action_time_owner59c11::SetOwnedFragments(g_owned_fragments60.c_str());
}
static std::atomic<bool> g_console_execute57{true};
bool SafeConsoleExecuteStringCandidate(void* console,const char* command) noexcept {
    if(!IsSupportedWhGameBuildV6143()||!console||!command||!*command||
       !g_console_execute57.load(std::memory_order_relaxed))return false;
    const char* text=command;
    while(*text==' '||*text=='\t'||*text=='\r'||*text=='\n')++text;
    if(!*text||*text=='#'||*text=='@')return false;
    __try {
        const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
        auto** vt=*reinterpret_cast<void***>(console);
        if(!base||!retail_build112::Ready(base)||reinterpret_cast<std::uintptr_t>(vt)!=base+0x3DCF850||
           reinterpret_cast<std::uintptr_t>(vt[0x118/sizeof(void*)])!=base+0x79EB88)return false;
        using Exec=void(__fastcall*)(void*,const char*,bool,bool);
        reinterpret_cast<Exec>(vt[0x118/sizeof(void*)])(console,command,true,true);
        return true; // Native queue accepted, not proof of command execution.
    } __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}

// C43D: the REAL IConsole::ExecuteString. checked 17 oct against WHGame 6A350E20: CXConsole (RTTI) vtable +3DCF850,
// slot +0x118 = +79EB88: (this, const char* cmd, bool silent, bool defer) - if defer it queues the command for the main
// thread, else ExecuteStringInternal, which tests cmd[0]=='#' (lua) and '@'. NOTE for codex: the old +0xD0 'candidate'
// above is NOT ExecuteString - its body copies the text and handles '\n' (a print/add-line); commands sent through it
// were printed, not executed. left unchanged here, only flagged.

// C47 (C46 log: the style 2 head never flew): the console route above can never run the Lua in retail. Static proof
// (WHGame 6A350E20): ExecuteString +79EB88 -> ExecuteStringInternal +79EBDC; for '#'/'@' it goes to +79F109, which
// prints the line (AddLine) and runs the script ONLY if m_pSystem(+0x200)->IsDevMode() (vt +0x3F8 = +79E238: the dev
// flag CSystem+0x5D2 && !IsMODValid) - false in a normal Steam start, so every C43D STUMP/HEAD line was only printed.
// The route below is the same script call that branch makes, without the dev-mode gate: CSystem (vt +3DC2858)
// +0x278 GetIScriptSystem (+1A73480: [[this+0x20]+0x28]) -> CScriptSystem (vt +3B8C610) +0x30 ExecuteBuffer (+4D4484)
// (this, buffer, size, description, env=null). Called only from the native game frame (gore32n::Tick under
// PhysicalNativeFrameTick6698 = the outer CSystem frame, the Lua thread). Every hop is pinned; SEH fails closed.
namespace c47lua {
constexpr std::uintptr_t kConsoleVt=0x3DCF850,kExecString=0x79EB88,kSystemVt=0x3DC2858,kGetScript=0x1A73480,kIsDevMode=0x79E238,kScriptVt=0x3B8C610,kExecBuffer=0x4D4484;
inline std::atomic<bool> g_first{true};
struct Probe {bool build=false,consoleExec=false,devGateBytes=false,sysGetScript=false,sysIsDev=false,scriptExec=false,console=false,system=false,script=false;int devMode=-1;};
inline Probe probe(void*& scriptOut) noexcept {
    Probe p{};scriptOut=nullptr;p.build=IsSupportedWhGameBuildV6143();if(!p.build)return p;
    const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));if(!base)return p;
    static const unsigned char gate[]{0x48,0x8b,0x8f,0x00,0x02,0x00,0x00,0x48,0x8b,0x01,0xff,0x90,0xf8,0x03,0x00,0x00};
    __try{
        p.consoleExec=*reinterpret_cast<std::uintptr_t*>(base+kConsoleVt+0x118)==base+kExecString;
        p.devGateBytes=std::memcmp(reinterpret_cast<const void*>(base+0x79F10E),gate,sizeof(gate))==0;
        p.sysGetScript=*reinterpret_cast<std::uintptr_t*>(base+kSystemVt+0x278)==base+kGetScript;
        p.sysIsDev=*reinterpret_cast<std::uintptr_t*>(base+kSystemVt+0x3F8)==base+kIsDevMode;
        p.scriptExec=*reinterpret_cast<std::uintptr_t*>(base+kScriptVt+0x30)==base+kExecBuffer;
        void* console=SafeGetCryConsole();
        if(console&&*reinterpret_cast<std::uintptr_t*>(console)==base+kConsoleVt){p.console=true;
            void* sys=*reinterpret_cast<void**>(static_cast<unsigned char*>(console)+0x200);
            if(sys&&*reinterpret_cast<std::uintptr_t*>(sys)==base+kSystemVt){p.system=true;
                p.devMode=*(static_cast<unsigned char*>(sys)+0x5D2)?1:0;
                if(p.sysGetScript){void* ss=reinterpret_cast<void*(__fastcall*)(void*)>(base+kGetScript)(sys);
                    if(ss&&*reinterpret_cast<std::uintptr_t*>(ss)==base+kScriptVt){p.script=true;scriptOut=ss;}}}}
    }__except(EXCEPTION_EXECUTE_HANDLER){scriptOut=nullptr;p.script=false;}
    return p;
}
inline void receipt(const char* when,const Probe& p) noexcept {
    char m[420]{};std::snprintf(m,sizeof(m),"C47 GORE LUA_ROUTE when=%s build=%d consoleExecuteString=%s consoleLuaNeedsDevMode=%s systemGetScriptSystem=%s systemIsDevMode=%s scriptExecuteBuffer=%s console=%s system=%s script=%s devMode=%s route=%s",
        when,p.build?1:0,p.consoleExec?"PIN_OK(79EB88)":"MISMATCH",p.devGateBytes?"YES(79F118)":"UNPROVEN",p.sysGetScript?"PIN_OK(1A73480)":"MISMATCH",p.sysIsDev?"PIN_OK(79E238)":"MISMATCH",
        p.scriptExec?"PIN_OK(4D4484)":"MISMATCH",p.console?"OK":"NOT_YET",p.system?"OK":"NOT_YET",p.script?"OK":"NOT_YET",p.devMode<0?"UNKNOWN":p.devMode?"ON":"OFF",
        p.build&&p.sysGetScript&&p.scriptExec?"SCRIPT_EXECUTEBUFFER_DIRECT":"NONE(style2 falls back to HIDE)");
    diagnostic_trace::Write(m);
}
}
bool ExecuteLuaDirect47(const char* lua) noexcept {
    if(!lua||!*lua)return false;
    void* ss=nullptr;const auto p=c47lua::probe(ss);
    const bool first=c47lua::g_first.exchange(false);if(first)c47lua::receipt("FIRST_USE",p);
    if(!ss||!p.scriptExec)return false;
    const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));if(!base)return false;
    bool ok=false;
    __try{ok=reinterpret_cast<bool(__fastcall*)(void*,const char*,std::size_t,const char*,void*)>(base+c47lua::kExecBuffer)(ss,lua,std::strlen(lua),"kcd2vr_c47",nullptr);}
    __except(EXCEPTION_EXECUTE_HANDLER){ok=false;diagnostic_trace::Write("C47 GORE LUA_FAULT (ExecuteBuffer exception)");}
    return ok;
}


#define KCD2VR_M2_HOST_SLOT 5001 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=5001
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
// V63.0: Lua HUD companion retired after headset testing showed periodic frame-pacing
// regressions. Native FlashUI + event-driven bounded reapply is the only active HUD path.

#define KCD2VR_M2_HOST_SLOT 5002 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=5002
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 5003 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=5003
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 5004 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=5004
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
int SafeCVarGetInt(void* cvar, int fallback = 0) noexcept
{
    if (!cvar) return fallback;
    __try {
        auto** vt = *reinterpret_cast<void***>(cvar);
        if (!vt || !vt[0x10 / sizeof(void*)]) return fallback;
        using Fn = int(__fastcall*)(void*);
        return reinterpret_cast<Fn>(vt[0x10 / sizeof(void*)])(cvar);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return fallback; }
}

float SafeCVarGetFloat(void* cvar, float fallback = 0.0f) noexcept
{
    if (!cvar) return fallback;
    __try {
        auto** vt = *reinterpret_cast<void***>(cvar);
        if (!vt || !vt[0x20 / sizeof(void*)]) return fallback;
        using Fn = float(__fastcall*)(void*);
        return reinterpret_cast<Fn>(vt[0x20 / sizeof(void*)])(cvar);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return fallback; }
}

bool SafeCVarSetInt(void* cvar, int value) noexcept
{
    if (!cvar) return false;
    __try {
        auto** vt = *reinterpret_cast<void***>(cvar);
        if (!vt || !vt[0x38 / sizeof(void*)]) return false;
        using Fn = void(__fastcall*)(void*, int);
        reinterpret_cast<Fn>(vt[0x38 / sizeof(void*)])(cvar, value);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

// V65.11 F11 rung 2: UI presentation only. This is intentionally edge-triggered
// and never owns gameplay transport. It preserves/restores KCD2's previous
// PreferredInputDevice value and performs at most one Flash SetInputId on entry.
#define KCD2VR_M2_HOST_SLOT 5005 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=5005
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
// V66.60: ICVar string access, proven on this WHGame.dll from the RTTI vtables of
// CXConsoleVariableString (0x183A7F1A0) and CXConsoleVariableStringRef (0x183DCFD60):
// +0x28 GetString() = return [this+0x48]; +0x38 Set(int); +0x40 Set(float); +0x48 Set(const char*)
// (string compare against [this+0x48]). e_GsmSizesWH's change callback 0x1819C5B70 re-parses
// the list via +0x28, so a live Set takes effect. Only use these on STRING CVars.
const char* SafeCVarGetString(void* cvar) noexcept
{
    if (!cvar) return nullptr;
    __try {
        auto** vt = *reinterpret_cast<void***>(cvar);
        if (!vt || !vt[0x28 / sizeof(void*)]) return nullptr;
        using Fn = const char*(__fastcall*)(void*);
        return reinterpret_cast<Fn>(vt[0x28 / sizeof(void*)])(cvar);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}
bool SafeCVarSetString(void* cvar, const char* value) noexcept
{
    if (!cvar || !value) return false;
    __try {
        auto** vt = *reinterpret_cast<void***>(cvar);
        if (!vt || !vt[0x48 / sizeof(void*)]) return false;
        using Fn = void(__fastcall*)(void*, const char*);
        reinterpret_cast<Fn>(vt[0x48 / sizeof(void*)])(cvar, value);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool SafeCVarSetFloat(void* cvar, float value) noexcept
{
    if (!cvar) return false;
    __try {
        auto** vt = *reinterpret_cast<void***>(cvar);
        if (!vt || !vt[0x40 / sizeof(void*)]) return false;
        using Fn = void(__fastcall*)(void*, float);
        reinterpret_cast<Fn>(vt[0x40 / sizeof(void*)])(cvar, value);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

// C31: keep the C29 graphics options, but own only verified edge writes.
// Each API leaf resolves its native CVar afresh; no unguarded pointer is cached.
std::atomic<int> g_vr_shadow_quality49{0};std::atomic<bool> g_eye_head26{true}; // C49 VrShadowQuality 0 LOW/1 MEDIUM/2 HIGH (was C26d/C32 bool VrShadowTweaks, opt-in)
struct GraphicsIO31 {
    bool Ready() const noexcept {
        return kcd2vr::retail_build112::Ready(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")));
    }
    bool Read(const char* name,bool floating,float& out) const noexcept {
        void* console=SafeGetCryConsole();void* value=console?SafeConsoleGetCVar(console,name):nullptr;
        if(!value)return false;
        if(floating){const float native=SafeCVarGetFloat(value,std::numeric_limits<float>::quiet_NaN());
            if(!std::isfinite(native))return false;out=native;return true;}
        const int native=SafeCVarGetInt(value,INT_MIN);if(native==INT_MIN||double(float(native))!=double(native))return false;out=float(native);return true;
    }
    bool Write(const char* name,bool floating,float wanted) const noexcept {
        if(!std::isfinite(wanted)||(!floating&&(double(wanted)<double(INT_MIN)||double(wanted)>double(INT_MAX))))return false;
        void* console=SafeGetCryConsole();void* value=console?SafeConsoleGetCVar(console,name):nullptr;
        return value&&(floating?SafeCVarSetFloat(value,wanted):SafeCVarSetInt(value,int(wanted)));
    }
};
static GraphicsIO31 g_graphics_io31;
static kcd2vr::graphics_owner31::Shadows g_vr_shadows_state26;
static kcd2vr::graphics_owner31::Eye g_eye_adapt26;
static kcd2vr::diagnostics::GraphicsReceipts32 g_graphics_receipts32;

// V66.178R13: reversible combat camera-lock lease (review repair of the R10 block).
struct CombatLockLease178r13 {
    static constexpr const char* names[4]{"wh_cs_PlayerInputLockAreaWidth","wh_cs_PlayerInputCombatUnlockDelay","wh_cs_PlayerInputLockingTolerance","wh_cs_PlayerInputCursorUnlockDistance"};
    static constexpr float off[4]{0.f,0.f,0.f,300.f};
    bool applied=false,disabledApplied=false;float original[4]{};int disabledOriginal=0;
    // Returns true when a message was produced.
    bool Apply(bool wantOff,bool wantDisabled,int rung,int mode,char* msg,std::size_t cap) noexcept {
        if(wantOff==applied&&wantDisabled==disabledApplied)return false;
        void* con=SafeGetCryConsole();int written=0,missing=0,verified=0;
        if(wantOff!=applied){
            void* v[4]{};bool all=con!=nullptr;
            for(int i=0;i<4;++i){v[i]=con?SafeConsoleGetCVar(con,names[i]):nullptr;if(!v[i]){all=false;++missing;}}
            if(wantOff){
                if(all){for(int i=0;i<4;++i)original[i]=SafeCVarGetFloat(v[i],off[i]);
                    for(int i=0;i<4;++i){if(SafeCVarSetFloat(v[i],off[i]))++written;if(std::fabs(SafeCVarGetFloat(v[i],-1.f)-off[i])<1e-4f)++verified;}
                    if(verified!=4){for(int i=0;i<4;++i)(void)SafeCVarSetFloat(v[i],original[i]);applied=false;} // partial: roll back
                    else applied=true;}
            }else{
                for(int i=0;i<4;++i)if(v[i]){const float cur=SafeCVarGetFloat(v[i],off[i]);
                    if(std::fabs(cur-off[i])<1e-4f){if(SafeCVarSetFloat(v[i],original[i]))++written;} // only restore a value still ours
                    ++verified;}
                applied=false;
            }
        }
        if(wantDisabled!=disabledApplied){
            void* v=con?SafeConsoleGetCVar(con,"wh_cs_PlayerLockDisabled"):nullptr;
            if(v){if(wantDisabled){disabledOriginal=SafeCVarGetInt(v,0);if(SafeCVarSetInt(v,1))++written;disabledApplied=SafeCVarGetInt(v,0)==1;}
                  else{if(SafeCVarGetInt(v,0)==1&&SafeCVarSetInt(v,disabledOriginal))++written;disabledApplied=false;}}
            else{++missing;disabledApplied=false;}
        }
        std::snprintf(msg,cap,"V66.178R13 CAMERA_LOCK autoLock=%s hardDisabled=%d rung=%d mode=%d written=%d verified=%d missing=%d originals=%.3g/%.3g/%.3g/%.3g",
            applied?"OFF":"NATIVE",disabledApplied?1:0,rung,mode,written,verified,missing,double(original[0]),double(original[1]),double(original[2]),double(original[3]));
        return true;
    }
    void Restore() noexcept {char m[320]{};if(Apply(false,false,0,0,m,sizeof(m)))diagnostic_trace::Write(m);}
};
static CombatLockLease178r13 g_combat_lock178r13;
// R50C1: preserve opponent-selection rules and lease only the combat-camera
// switch. Missing/failed CVars are attempted once per mode edge, never per frame.
static std::atomic<bool> g_combat_camera_only50{true};
struct CombatCameraLease50 {
    bool applied=false,requested=false,attempted=false,restoreOwed=false,pending=false;
    int original=0;unsigned retries=0;std::uint64_t retryTick=0;
    bool Apply(bool want,char* msg,std::size_t cap) noexcept {
        const auto now=std::uint64_t(GetTickCount64());
        const bool edge=!attempted||want!=requested;
        if(!edge){
            // Only uncertain writes/restores retry, at most three times per
            // selection edge. A settled lease never polls native CVars.
            if(!pending||retries>=3||now<retryTick||now-retryTick<100)return false;
            ++retries;
        }else retries=0;
        requested=want;attempted=true;pending=false;retryTick=now;
        if(!want&&!restoreOwed&&!applied)return false;
        if(!retail_build112::Ready(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")))){
            pending=restoreOwed;
            std::snprintf(msg,cap,"V66.178R50C1 COMBAT_CAMERA wanted=%d retail=UNVERIFIED writes=NONE pending=%d retry=%u",want,pending,retries);return true;}
        void* con=SafeGetCryConsole();void* v=con?SafeConsoleGetCVar(con,"wh_cs_DisableCombatCamera"):nullptr;
        const int current=v?SafeCVarGetInt(v,INT_MIN):INT_MIN;
        if(current==INT_MIN){pending=restoreOwed;std::snprintf(msg,cap,"V66.178R50C1 COMBAT_CAMERA wanted=%d read=UNAVAILABLE owned=%d pending=%d retry=%u",want,applied,pending,retries);return true;}
        bool verified=false;
        if(want){
            if(restoreOwed&&current==1){applied=true;verified=true;}
            else if(restoreOwed&&(!edge||current!=original)){
                applied=false;restoreOwed=false; // A foreign writer won; do not overwrite it.
            }else{
                if(!restoreOwed)original=current;
                // Retain the original even if the successful native setter's
                // readback disappears. OFF must still be able to restore it.
                restoreOwed=true;(void)SafeCVarSetInt(v,1);
                const int after=SafeCVarGetInt(v,INT_MIN);
                applied=verified=after==1;
                pending=after==INT_MIN;
                if(after!=INT_MIN&&after!=1)restoreOwed=false;
            }
        }else if(current==1){
            (void)SafeCVarSetInt(v,original);
            const int after=SafeCVarGetInt(v,INT_MIN);
            verified=after==original;
            if(verified||(after!=INT_MIN&&after!=1)){applied=false;restoreOwed=false;}
            else pending=restoreOwed=true;
        }else{applied=false;restoreOwed=false;} // Preserve another owner's value.
        std::snprintf(msg,cap,"V66.178R50C1 COMBAT_CAMERA wanted=%d owned=%d verified=%d original=%d pending=%d retry=%u opponentSelection=NATIVE",want,applied,verified,original,pending,retries);return true;
    }
    void Restore() noexcept {attempted=false;char m[256]{};if(Apply(false,m,sizeof(m)))diagnostic_trace::Write(m);}
};
static CombatCameraLease50 g_combat_camera50;
#define KCD2VR_M2_HOST_SLOT 5006 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=5006
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
// F9 selection/status only. The legacy ragdoll multiplier is read-only;
// native living hit reactions retain their own animation/physics ownership.
#define KCD2VR_M2_HOST_SLOT 5007 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=5007
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
// C39 VR DIRECT NPC BLOCK (Miki: "force the npc finally to block in VR direct mode").
// The game has its own switch for this: wh_cs_ForceBlock (registered at +10AA2xx, help text: "Force AI to block by:
// 0 - disabled, 1 - normal block, 2 - perfect block, 3 - no block, 4 - broken block, 5 - master strike"). It acts on the
// native block OFFER the NPC already gets when VR DIRECT publishes Henry's phase 4 (914204) - so the block is the game's own
// animation, timing and outcome, just never declined. Codex's C37C2 review: 36 of 47 direct contacts got no defense at all.
// Modes (COMBAT FLOW page, INI VrDirectNpcBlock): 0 native, 1 often (per swing 65 %), 2 always block, 3 always perfect block.
// Only while F4 VR DIRECT is on; the original value is restored the moment it is off (the CVar is global, so other
// fights are affected only while you fight in VR DIRECT). One roll per swing onset, never per frame.
#define KCD2VR_M2_HOST_SLOT 5008 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=5008
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
// D51: legacy living-hit CVar leases may retire their own prior writes only.
// F9 must not force global impact, recoil or fall-and-play settings for every
// arrow/NPC. Per-actor animation/physics handoff for those trials is unverified.
#define KCD2VR_M2_HOST_SLOT 5009 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=5009
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
// R51C1: no global NPC/combat CVar preset. Only the exact-Henry native
// angular predicate is widened during a deliberate F4 gesture.
static std::atomic<bool> g_combat_preset51{true};
struct CombatCvarLease51 {
    bool enabled=true,applied=false;
    void Load(const char*,bool on) noexcept {enabled=on;}
    bool Apply(bool want,char* msg,std::size_t cap) noexcept {
        want=want&&enabled;if(want==applied)return false;applied=want;
        frik_pose_probe::SetCombatAdmissionAssist51(want);
        std::snprintf(msg,cap,"V66.178R51C1 ADMISSION_MODE enabled=%d globalCvarWrites=NONE scope=HENRY_DELIBERATE_ANGLE",want);return true;
    }
    void Restore() noexcept {char m[256]{};if(Apply(false,m,sizeof(m)))diagnostic_trace::Write(m);}
};
static CombatCvarLease51 g_combat_cvars51;
// V66.178R52 (Claude, on R51C1): R51C1 kept the Henry-only angle predicate and retired the cvar preset because some
// of those rules are shared with NPCs. Agreed for the shared ones - but four of them are PLAYER rules by the game's
// own naming (wh_cs_Player*), and two of them are the user's open complaints: the opponent is locked only within
// 45 deg of the VIEW (VR head and body turn independently -> Henry had an opponent in 6 of 26 summaries), and a free
// stab is refused inside PlayerFreeAttackStabMinDistance ("stabs: no sound"). Those four stay a reversible lease
// (VrCombatPlayerRules, default on). The shared ones (perfect-block volume 0.1 m, attack move offset, block cooldown)
// are opt-in: VrCombatSharedRules=true applies them to everyone in the fight - the user's choice, logged as such.
static std::atomic<bool> g_dialogue_stable5915{false}; // R59.15: INI VrDialogueStableCamera. R59.16C7E-DECAP1B: default OFF - the frozen pre-dialogue base left the view outside Henry's head when the dialogue moved him (13 Oct report)
static std::atomic<bool> g_player_rules52{true},g_shared_rules52{false},g_no_lunge594{false};
static float g_decapitation_min_energy59c7e=0.6f;
static bool g_support_socket_synthetic59c7f=true; // R59.16C7F
static std::atomic<bool> g_pause_frozen_pose59c14{true}; // R59.16C10R14: INI VrPauseFrozenPose
#define KCD2VR_M2_HOST_SLOT 5010 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=5010
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
static std::atomic<bool> g_immersive_fov59c7g{false}; // R59.16C9A: C7G SetFrustum hook OFF by default (13 Oct 18:57 log: every other XR frame black, freshPct=50 while it ran)
static std::atomic<bool> g_activity_lens59c10r1{true}; // scoped native activity view output, no frustum hook
static std::atomic<bool> g_auto_palm59c8{true},g_riposte_native59c8{false}; // R59.16C8: Immersion page (INI VrAutoPalmAlign, VrRiposteNative)
// R59.16C10d EXPERIMENTS page shadows: the INI read stores here, the page applies through the same setters and the INI write saves them.
struct Exp59c10 {std::atomic<bool> showBlockIndicatorH24{false};std::atomic<bool> wallSlide{true},telegraphWindup{true},telegraphStrike{false},windupHold{true},guardCounter{true},guardAutoWindup{false};std::atomic<float> guardEnterSpeed{3.f},guardExitSpeed{4.f},strikeRefJ{75.f},strikeMaxPower{1.10f},strikeTwoHanded{1.25f},strikeMinPower{.30f},headRaiseCm{18.f};std::atomic<int> guardEnterDwellMs{0},guardAfterAttackMs{400},strikeRepeatMs{2000},riposteWindowMs{700};std::atomic<bool> strikePower{true};
    std::atomic<float> npcEtaPrepare{0.25f},npcEtaStrike{0.12f},attackReachM{2.6f};std::atomic<bool> attackRequireOpponent{true};
    std::atomic<bool> directBlock39{true};std::atomic<float> directBlockLo39{2.f},directBlockHi39{3.f}; // C39 VR DIRECT NPC block speed by skill
    std::atomic<int> actionOwner{2};std::atomic<float> actionHoldAt{0.50f},actionResumeSpeed{2.0f}; // R59.16C10R15: OWN by default
    std::atomic<bool> syncOverdueAny{true},recoveryOverride{true},noRowFallback{true};
    std::atomic<bool> reclaimGuard{true},strikeDuringStagger{false}; // R59.16C10R16 rung 4 (F4 press 4); C44: derived from staggerRule44 (!=1)
    std::atomic<int> staggerRule44{1}; // C44 VrDirectStaggerRule 0 strike out of stagger / 1 no DIRECT hit while staggered / 2 half strength
    // R59.16C10R17 COMBAT FLOW page
    std::atomic<bool> guardStrict{false},noBlockCooldown{true},msKeepStance{true},intentLive{true};
    std::atomic<bool> instantFreeSwing38{true},hideSwingLean38{true}; // C38: VrInstantFreeSwing / VrHideSwingLean (COMBAT FLOW page)
    std::atomic<bool> staggerGuard39{false}; // C39: VrDirectGuardOutOfStagger (COMBAT FLOW page, F4 DIRECT only)
    std::atomic<bool> dirBlock39{true}; // C39: VrDirectNpcDirectionalBlock (F4 DIRECT only)
    std::atomic<float> nativeHitTime{0.30f},weaponStatEffect{1.0f};std::atomic<int> comboWindowMs{1500},intentLiveMs{60};
    std::atomic<bool> ownHits18{true};std::atomic<float> npcBlockSpeed18{1.0f};std::atomic<bool> msHandoffOn23{true};std::atomic<float> msHandoff23{3.5f};std::atomic<int> kickStyle26{0},kickLeg26{0},kickReachMs26{730},statSim26{0};std::atomic<bool> kickFrees26{true}; // C10R18 F4 press 4 (VR DIRECT); C10R19 native timing default
    // R59.16C10R14 WEAPON FEEL page
    std::atomic<float> weightFeel{0.6f},fatigueRamp{0.6f},poleOneHand{0.40f},swingStaminaScale{1.0f},strikeMinScale{0.35f};std::atomic<int> strikeMinIntervalMs{0},hideStowedBow{2};std::atomic<bool> twoHandAxis{true}; // C54 virtual back holster: hidden in immersive view
    std::atomic<bool> strikeMinGate{false},masterStrikeGame{true},strikeWaitNative{true};std::atomic<float> hideBowPitchDeg{20.f};} g_exp59c10; // R59.16C10R11: action time owner (0 off, 1 observe, 2 own); R12: the three 19:43-log switches // R59.16C10R5: NPC block ETA (S33) + opponent-in-reach gate
static std::atomic<float> g_target_dwell59c8{0.4f}; // R59.16C8: seconds the head must look away before the opponent lock switches (INI VrTargetSwitchDwell; Immersion slider)
static bool g_targeting_lease59c8=true;
static std::atomic<bool> g_hybrid_default59{true}; // C59 normal calibrated gameplay uses the headset-preferred hybrid backend
static std::atomic<bool> g_held_map_apply_d39{false}; // Run owns live pin installation; menu only queues intent.
static std::atomic<bool> g_held_map59c10r10{true}; // R38: honor existing VrHeldMap; explicit Ds7 value takes precedence
#define KCD2VR_M2_HOST_SLOT 5011 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=5011
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
static std::atomic<bool> g_inventory_early_exit25{true}; // C25: INI VrInventoryEarlyExit (default true; INI-only, read at startup)
static std::atomic<std::uint64_t> g_apse_close_press_ms{0}; // C25: last owned pad B/menu-close press inside APSE UI (loop thread)
static std::atomic<bool> g_gesture_talk59c5{false}; // Optional index-finger point holds native Quick Talk choices (INI VrGestureTalk, VR menu)
static std::atomic<bool> g_alchemy_finish_observer_cb1{false}; // CB1: INI VrAlchemyFinishObserver, read-only native brew result observer
static std::atomic<bool> g_smooth_move51{false}; // C51: smooth the shared body anchor; tracked head/hands remain owned in either mode.
static std::atomic<bool> g_hmd_walking_direction134{true}; // R134: rotate gameplay walking axes by recentered HMD yaw.
static std::atomic<int> g_comfort_vignette51{0}; // C51: 0 off, 1 low, 2 medium, 3 strong.
static std::atomic<bool> g_roomscale_follow58{false},g_roomscale_dodge58{false}; // V66.178R58 (INI VrRoomscaleFollow / VrRoomscaleDodge; active while 6DOF BODY is on)
struct CombatCvarLease52 {
    static constexpr int kMax=24;const char* names[kMax]{};float values[kMax]{};float originals[kMax]{};bool had[kMax]{};int count=0;bool applied=false;
    void Add(const char* n,float v) noexcept {if(count<kMax){names[count]=n;values[count]=v;++count;}}
    int freeModeR133=0;
    void Load(bool player,bool shared,bool targeting=false,bool noLunge=false,int freeMode=0) noexcept {count=0;freeModeR133=freeMode;
        // R133: the target cone follows the free-targeting mode (free_targeting_plus_r133.h cone()). mode 0 = the
        // R52/R57 values below exactly as before.
        const auto coneR133=kcd2vr::free_targeting_r133::cone(freeMode);
        if(player){Add("wh_cs_PlayerMaxOpponentAngleToLock",coneR133.lockAngle);Add("wh_cs_PlayerMaxOpponentAngleToUnlock",coneR133.unlockAngle);
            Add("wh_cs_PlayerFreeAttackMinDistance",0.f);Add("wh_cs_PlayerFreeAttackStabMinDistance",0.f);
            // Shared distance correction belongs exclusively to explicit shared-rule opt-in.
            // R55C1: DisabledAttackTimeout was already zero in the supplied R55
            // run. Its global write cannot explain the improvement and is removed.
        }
        if(player&&targeting){
            // V66.178R57 (the user's "Responsive Targeting" cfg pak, ModConfigs/ResponsiveTargeting.cfg): the same wh_cs_Player*
            // values applied as a reversible lease instead of a pak. Lock/unlock distance 8 m, no unlock timeout/delays, a 2x
            // angle bias toward the current opponent, locking tolerance 20 (was 60) and a 0.6 lock area (was 0.4). The cfg's
            // 0.8 rad lock/unlock angles are NOT taken: the R52 90/137 deg values exist because the VR head and body turn apart.
            Add("wh_cs_PlayerMaxOpponentDistanceToLock",8.f);Add("wh_cs_PlayerMaxOpponentDistanceToUnlock",8.f);
            Add("wh_cs_PlayerUnlockTimeout",0.f);Add("wh_cs_PlayerVerticalUnlockDelay",0.f);Add("wh_cs_PlayerHorizontalUnlockDelay",0.f);
            Add("wh_cs_PlayerLockOpponentAngleBias",1.f); // C44: remove the doubled bias retaining the old opponent
            // R59.16C8 (Miki: "looser targeting - the head view picks a target after looking at it for longer"): the look-away
            // time before the lock switches is the Immersion slider VrTargetSwitchDwell (seconds), live.
            {const float dwell=std::clamp(g_target_dwell59c8.load(std::memory_order_relaxed),0.f,2.f);
             Add("wh_cs_PlayerInputCombatUnlockDelay",dwell);Add("wh_cs_PlayerInputMouseUnlockMinTime",dwell);Add("wh_cs_PlayerInputMouseUnlockReturnTime",std::max(0.2f,dwell));}
            Add("wh_cs_PlayerInputLockingTolerance",20.f);Add("wh_cs_PlayerInputLockAreaWidth",coneR133.lockArea);
            if(coneR133.hyst)Add("wh_cs_PlayerInputUnlockAreaWidthHyst",coneR133.unlockHyst); // R133: native 0.2
        }
        if(player&&!targeting&&coneR133.hyst){ // R133: the wider cone is part of free targeting 1/2 even without R57
            Add("wh_cs_PlayerInputLockAreaWidth",coneR133.lockArea);Add("wh_cs_PlayerInputUnlockAreaWidthHyst",coneR133.unlockHyst);}
        if(shared){Add("wh_cs_PerfectBlock_SpatialVolumeRadius",0.6f);Add("wh_cs_MaxAttackDistanceMoveOffset",0.f);Add("wh_cs_BlockCooldown",0.05f);}}
    bool Apply(bool want,char* msg,std::size_t cap) noexcept {
        if(want==applied||!count)return false;
        void* con=SafeGetCryConsole();int written=0,missing=0;std::size_t at=std::size_t(std::snprintf(msg,cap,"V66.178R52 CVAR_LEASE %s scope=%s",want?"APPLY":"RESTORE",g_shared_rules52.load()?"PLAYER+SHARED(user opt-in)":"PLAYER_NAMED_CVARS_SCOPE_NOT_PROVEN"));
        for(int i=0;i<count;++i){void* v=con?SafeConsoleGetCVar(con,names[i]):nullptr;
            if(!v){++missing;had[i]=false;if(at<cap)at+=std::size_t(std::snprintf(msg+at,cap-at," %s=MISSING",names[i]+6));continue;}
            if(want){originals[i]=SafeCVarGetFloat(v,0.f);had[i]=true;if(SafeCVarSetFloat(v,values[i]))++written;
                if(at<cap)at+=std::size_t(std::snprintf(msg+at,cap-at," %s=%.4g(was %.4g)",names[i]+6,double(SafeCVarGetFloat(v,0.f)),double(originals[i])));}
            else if(had[i]&&std::fabs(SafeCVarGetFloat(v,0.f)-values[i])<1e-4f){if(SafeCVarSetFloat(v,originals[i]))++written;}}
        applied=want;if(at<cap)std::snprintf(msg+at,cap-at," written=%d missing=%d",written,missing);return true;}
    void Restore() noexcept {char m[1024]{};if(Apply(false,m,sizeof(m)))diagnostic_trace::Write(m);}
};
static CombatCvarLease52 g_combat_cvars52;
// H24: retain a tiny POSITIVE native delay so original LT-up initializes its
// timer. Zero skips that initialization and an invalid timer never ACKs release.
#include "subsystems/masterstrike_combos/block_cooldown_lease_h24.inl"
// C38 INSTANT FREE SWING (17 Oct log, F4 SYNC: 21 of 39 VR swings were FREE attacks - no opponent lock - and each one waited
// 286-322 ms in phase 4 before the native swing began; the locked ones 43-100 ms). WHGame 6A350E20: wh_cs_PlayerFreeAttackPrepareTime
// ("Time in sec for prepare attack during free attack. The attack action is started after that time.", default 0.3) is stored at
// config +0x134 (registration 10A9815) and read at exactly ONE place, 9D40B5, which arms the timer 9D6A34 inside 9D3F80. 9D3F80 is
// only reached from 9D4244 <- 181002DD4, a slot of the C_CombatPlayer vtable (3A80938 in 3A80610; NPCs use 3E3FAC8), so the value
// is a PLAYER rule. 0 while an F4 mode is on: the timer expires on the next update and the native free attack starts there (phase
// 4/256 check at 9D40E8 unchanged). Every other rule (zone, damage, NPC defence, stamina) is the game's. Restored on exit if still ours.
struct FreeAttackPrepareLease38 {
    bool applied=false,had=false;float original=0.3f;
    bool Apply(bool want,char* msg,std::size_t cap) noexcept {
        if(want==applied)return false;
        void* con=SafeGetCryConsole();void* v=con?SafeConsoleGetCVar(con,"wh_cs_PlayerFreeAttackPrepareTime"):nullptr;
        if(want){applied=true;if(!v){had=false;std::snprintf(msg,cap,"C38 FREE_SWING_LEASE APPLY cvar=MISSING");return true;}
            original=SafeCVarGetFloat(v,.3f);had=true;const bool ok=SafeCVarSetFloat(v,0.f);
            std::snprintf(msg,cap,"C38 FREE_SWING_LEASE APPLY wh_cs_PlayerFreeAttackPrepareTime=%.3g(was %.3g) written=%d scope=PLAYER_FREE_ATTACK_ONLY reader=9D40B5",double(SafeCVarGetFloat(v,0.f)),double(original),ok?1:0);}
        else{bool ok=false;if(v&&had&&std::fabs(SafeCVarGetFloat(v,-1.f))<1e-4f)ok=SafeCVarSetFloat(v,original);applied=false;
            std::snprintf(msg,cap,"C38 FREE_SWING_LEASE RESTORE wh_cs_PlayerFreeAttackPrepareTime=%.3g restored=%d",double(v?SafeCVarGetFloat(v,0.f):-1.f),ok?1:0);}
        return true;}
    void Restore() noexcept {char m[220]{};if(Apply(false,m,sizeof(m)))diagnostic_trace::Write(m);}
};
static FreeAttackPrepareLease38 g_free_swing38;
// V66.178R50: once per session when a combat rung starts, log the live values of the native block/lock rules so the
// next log shows what the game is actually using (nothing is written here).
static void ProbeCombatCvars50() noexcept {
    static bool done=false;if(done)return;done=true;
    if(!retail_build112::Ready(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"))))return;
    void* con=SafeGetCryConsole();if(!con)return;
    static constexpr const char* names[]{"wh_cs_DisableCombatCamera","wh_cs_DisablePlayerOpponentSelection","wh_cs_PlayerLockDisabled",
        "wh_cs_PlayerInputLockAreaWidth","wh_cs_PlayerInputLockingTolerance","wh_cs_PlayerMaxOpponentDistanceToLock","wh_cs_PlayerMaxOpponentAngleToLock",
        "wh_cs_DelayedPerfectBlockInSlot","wh_cs_EnableRipostePerfectBlock","wh_cs_ForceSpecialPerfectBlock","wh_cs_ForceBlock","wh_cs_DelayedBlock",
        "wh_cs_BlockCooldown","wh_cs_FreeBlockPreparingDuration","wh_cs_DisableWeakBlock","wh_cs_LogicalBlockMaxAngle","wh_cs_PerfectBlock_SpatialVolumeRadius",
        "wh_cs_PerfectBlock_RetargetAngleFOVBias","wh_cs_RipostePerfectBlockZoneDistance","wh_cs_AIForceBlockMode","wh_cs_AIReblockMode","wh_cs_StealthDisableAIPerfectBlock",
        "wh_cs_LockedAttackMaxAngle","wh_cs_LockedAttackDistanceTolerance","wh_cs_LockedAttackZAngle","wh_cs_TimeAlignment_BlockSpeedMod","wh_cs_CombatCameraStyle",
        "wh_cs_PlayerMaxOpponentAngleToUnlock","wh_cs_MaxAttackDistanceMoveOffset","wh_cs_MaxAttackDistanceMoveOffsetStiffness","wh_cs_PlayerFreeAttackMinDistance","wh_cs_PlayerFreeAttackStabMinDistance","wh_cs_PlayerDistanceTolerance","wh_cs_PlayerMaxThreatDistance", // R51
        "wh_cs_DisabledAttackTimeout","wh_cs_DisabledAttackDebug","wh_cs_WeaponSolverNearColDist","wh_cs_WeaponSolverNearColDistVisualFilter","wh_cs_WeaponProcSolverNearColDist","wh_cs_MaxFakeCollisionDistance","wh_cs_BehindCollisionIgnoreThreshold","wh_cs_EnableCollisionResizing","wh_cs_WeaponSolverEnableMultipleCollisions",
        "wh_cs_ComboSlotDuration","wh_cs_RiposteSlotDuration","wh_cs_DelayedMasterStrikeInSlot","wh_cs_DelayedRiposteInSlot",
        "wh_cs_EnableRiposteChains","wh_cs_CombosEnabled","wh_cs_MasterStrikeOnRiposteTimeOffset","wh_cs_ShowRiposteUI","wh_cs_ShowComboZonesUI",
        "wh_cs_PlayerInputUnlockAreaWidthHyst","wh_cs_PlayerInputLockAreaWidthNoCombat","wh_cs_PlayerInputGamepadUnlockMinDistanceToUnlock","wh_cs_PlayerInputGamepadUnlockMinTime","wh_cs_PlayerInputGamepadUnlockMinTimeSameZone","wh_cs_PlayerInputGamepadUnlockReturnTime","wh_cs_PlayerLockOpponentAngleBias","wh_cs_PlayerUnlockTimeout","wh_cs_PlayerMaxOpponentDistanceToUnlock",
        "wh_HitReaction_Enabled","wh_HitReaction_CollisionsEnabled","wh_HitReaction_PhysicalHitCoef","wh_HitReaction_EnvironmentCollisionScale","wh_cs_PhysHitReaction_ImpactCoef","wh_cs_PhysWeaponHitReaction_ImpactCoef","wh_cs_HitReactions","g_hitDeathReactions_disableRagdoll","p_joint_damage_accum","p_joint_damage_accum_threshold","ca_DeathBlendTime"}; // R57: the native physical hit-reaction / ragdoll knobs (learned from the KCD1 ragdoll mod's technique: hit_reaction_type -> Ragdollize + physical_coef): targeting cfg values (nexus 2497) probed before any lease // R55: the "X" (disabled attack) rule and the weapon solver's near-collision distances
    char line[1200]{};unsigned batch=0;std::size_t at=0;
    const auto begin=[&](){at=std::size_t(std::snprintf(line,sizeof(line),"V66.178R55C2 CVAR_PROBE batch=%u",++batch));};
    const auto flush=[&](){diagnostic_trace::Write(line);KCD2VR_DIAG_LOG(Bug,warning,"{}",line);};begin();
    for(const char* n:names){void* v=SafeConsoleGetCVar(con,n);char entry[192]{};
        const char* shown=std::strncmp(n,"wh_cs_",6)==0?n+6:n; // R57: non-wh_cs_ names keep their prefix
        const int length=v?std::snprintf(entry,sizeof(entry)," %s=%.4g",shown,double(SafeCVarGetFloat(v,-999.f))):
            std::snprintf(entry,sizeof(entry)," %s=MISSING",shown);
        if(length<=0||std::size_t(length)>=sizeof(entry))continue;
        if(at+std::size_t(length)>=sizeof(line)){flush();begin();}
        std::memcpy(line+at,entry,std::size_t(length)+1);at+=std::size_t(length);
    }
    flush();}


// V58.3 regression repair: KCD2 can expose the saved cl_fov value while the
// active ViewCamera is still using the old/default cached projection until the
// graphics slider is touched. A same-value Set() may be short-circuited by the
// CVar implementation, so reproduce the user's proven slider workaround with a
// tiny reversible pulse and immediately restore the exact original value.
// This preserves the user's chosen FOV; it does not tie the MIS mono-remap
// calibration back to cl_fov.
float ReadCurrentGameFovValue() noexcept
{
    void* console = SafeGetCryConsole();
    if (!console) return 0.0f;
    void* f = SafeConsoleGetCVar(console, "cl_fov");
    if (!f) return 0.0f;
    const float v = SafeCVarGetFloat(f, 0.0f);
    return (v > 1.0f && v < 200.0f) ? v : 0.0f;
}

float ReadCurrentHorizontalFovValue() noexcept
{
    void* console = SafeGetCryConsole();
    if (!console) return 0.0f;
    void* f = SafeConsoleGetCVar(console, "cl_hfov");
    if (!f) return 0.0f;
    const float v = SafeCVarGetFloat(f, 0.0f);
    return (v > 1.0f && v < 220.0f) ? v : 0.0f;
}

bool ApplyNativeHorizontalFov(float value) noexcept
{
    void* console = SafeGetCryConsole();
    void* hf = console ? SafeConsoleGetCVar(console, "cl_hfov") : nullptr;
    void* vf = console ? SafeConsoleGetCVar(console, "cl_fov") : nullptr;
    void* rw = console ? SafeConsoleGetCVar(console, "r_CustomResWidth") : nullptr;
    void* rh = console ? SafeConsoleGetCVar(console, "r_CustomResHeight") : nullptr;
    if (!hf) { debug_trace::Write("V60.9-HFOV cl_hfov CVar not found"); return false; }
    const float target = std::clamp(value, 70.0f, 170.0f);
    const float before_v = vf ? SafeCVarGetFloat(vf, -1.0f) : -1.0f;
    const int before_w = rw ? SafeCVarGetInt(rw, -1) : -1;
    const int before_h = rh ? SafeCVarGetInt(rh, -1) : -1;
    const bool ok = SafeCVarSetFloat(hf, target);
    const float got_h = SafeCVarGetFloat(hf, -1.0f);
    const float got_v = vf ? SafeCVarGetFloat(vf, -1.0f) : -1.0f;
    const int got_w = rw ? SafeCVarGetInt(rw, -1) : -1;
    const int got_hres = rh ? SafeCVarGetInt(rh, -1) : -1;
    char m[384]{};
    std::snprintf(m,sizeof(m),
        "V60.9-HFOV APPLY cl_hfov target=%.1f readback=%.3f cl_fov before/after=%.3f/%.3f customRes before=%dx%d after=%dx%d ok=%s",
        static_cast<double>(target), static_cast<double>(got_h),
        static_cast<double>(before_v), static_cast<double>(got_v),
        before_w,before_h,got_w,got_hres,ok?"true":"false");
    debug_trace::Write(m);
    return ok && std::fabs(got_h-target) < 0.25f;
}

float ReadCurrentGameFpsCap() noexcept
{
    void* console = SafeGetCryConsole();
    if (!console) return 0.0f;
    void* f = SafeConsoleGetCVar(console, "sys_MaxFPS");
    return f ? SafeCVarGetFloat(f, 0.0f) : 0.0f;
}

bool ApplyNativeGameFov(float value) noexcept
{
    void* console = SafeGetCryConsole();
    void* f = console ? SafeConsoleGetCVar(console, "cl_fov") : nullptr;
    if (!f) return false;
    const float target = std::clamp(value, 70.0f, 130.0f);
    const bool ok = SafeCVarSetFloat(f, target);
    const float got = SafeCVarGetFloat(f, -1.0f);
    char m[256]{}; std::snprintf(m,sizeof(m),"V59.7-NATIVE-APPLY cl_fov target=%.1f readback=%.3f ok=%s; no pulse",target,got,ok?"true":"false"); debug_trace::Write(m);
    return ok && std::fabs(got-target) < 0.25f;
}

bool ApplyNativeGameFovV6128Test(float value) noexcept
{
    void* console = SafeGetCryConsole();
    void* f = console ? SafeConsoleGetCVar(console, "cl_fov") : nullptr;
    if (!f) return false;
    // Test-only ceiling. If the CVar itself has a lower engine clamp, readback exposes it
    // and the closed loop stops rather than forcing memory.
    const float target = std::clamp(value, 70.0f, 150.0f);
    const bool ok = SafeCVarSetFloat(f, target);
    const float got = SafeCVarGetFloat(f, -1.0f);
    char m[288]{}; std::snprintf(m,sizeof(m),
        "V61.32-GAME-FOV request=%.2f readback=%.3f ok=%s ceiling=150 ordinary-CVar-only",
        static_cast<double>(target),static_cast<double>(got),ok?"true":"false"); debug_trace::Write(m);
    return ok && got > 1.0f;
}

bool ApplyNativeGameFps(float value) noexcept
{
    void* console = SafeGetCryConsole();
    void* f = console ? SafeConsoleGetCVar(console, "sys_MaxFPS") : nullptr;
    if (!f) return false;
    const float target = std::clamp(value, 60.0f, 120.0f);
    const bool ok = SafeCVarSetFloat(f, target);
    const float got = SafeCVarGetFloat(f, -1.0f);
    char m[256]{}; std::snprintf(m,sizeof(m),"V59.7-NATIVE-APPLY sys_MaxFPS target=%.1f readback=%.3f ok=%s",target,got,ok?"true":"false"); debug_trace::Write(m);
    return ok && std::fabs(got-target) < 0.25f;
}

bool DisableNativeGameFpsCap() noexcept
{
    void* console = SafeGetCryConsole();
    void* f = console ? SafeConsoleGetCVar(console, "sys_MaxFPS") : nullptr;
    if (!f) return false;
    const bool ok = SafeCVarSetFloat(f, 0.0f);
    const float got = SafeCVarGetFloat(f, -1.0f);
    char m[256]{}; std::snprintf(m,sizeof(m),"V65.37-FPS startup/off sys_MaxFPS=0 readback=%.3f ok=%s",got,ok?"true":"false"); debug_trace::Write(m);
    return ok && std::fabs(got) < 0.25f;
}

int ReadCurrentGameVsync() noexcept
{
    void* console=SafeGetCryConsole();
    void* v=console?SafeConsoleGetCVar(console,"r_Vsync"):nullptr;
    return v?SafeCVarGetInt(v,-1):-1;
}

bool DisableNativeGameVsync() noexcept
{
    void* console=SafeGetCryConsole();
    void* v=console?SafeConsoleGetCVar(console,"r_Vsync"):nullptr;
    if(!v) return false;
    const bool ok=SafeCVarSetInt(v,0);
    const int got=SafeCVarGetInt(v,-1);
    char m[192]{}; std::snprintf(m,sizeof(m),"V65.44-FPS r_Vsync=0 readback=%d ok=%s",got,ok?"true":"false"); debug_trace::Write(m);
    return ok && got==0;
}

float ReadMergedMeshesDistance() noexcept
{
    void* console = SafeGetCryConsole();
    void* f = console ? SafeConsoleGetCVar(console, "e_MergedMeshesViewDistRatio") : nullptr;
    return f ? SafeCVarGetFloat(f, 0.0f) : 0.0f;
}

bool ApplyMergedMeshesDistance(float value) noexcept
{
    // V61.32 release-facing "View Distance" control: keep the three ordinary
    // Warhorse distance ratios in lockstep so the slider is predictable.
    void* console = SafeGetCryConsole();
    void* view = console ? SafeConsoleGetCVar(console, "e_ViewDistRatio") : nullptr;
    void* veg = console ? SafeConsoleGetCVar(console, "e_ViewDistRatioVegetation") : nullptr;
    void* merged = console ? SafeConsoleGetCVar(console, "e_MergedMeshesViewDistRatio") : nullptr;
    if (!view || !veg || !merged) return false;
    const float target = std::clamp(value, 30.0f, 300.0f);
    bool ok=true;
    ok=SafeCVarSetFloat(view,target)&&ok;
    ok=SafeCVarSetFloat(veg,target)&&ok;
    ok=SafeCVarSetFloat(merged,target)&&ok;
    const float gv=SafeCVarGetFloat(view,-1.0f), gg=SafeCVarGetFloat(veg,-1.0f), gm=SafeCVarGetFloat(merged,-1.0f);
    ok=ok && std::fabs(gv-target)<0.25f && std::fabs(gg-target)<0.25f && std::fabs(gm-target)<0.25f;
    char m[360]{};
    std::snprintf(m,sizeof(m),"V61.32-VIEW-DIST APPLY target=%.1f readback={view:%.2f veg:%.2f merged:%.2f} ok=%s",
        static_cast<double>(target),static_cast<double>(gv),static_cast<double>(gg),static_cast<double>(gm),ok?"true":"false");
    debug_trace::Write(m);
    return ok;
}




// V61.12: KCD2 retail WHGame contains the DOF parameter names
// Dof_User_FocusDistance / Dof_FocusDistance / FocusDistance. Try them only
// through the existing CryConsole read path; never write engine DOF state.



// V58 dialogue camera controls discovered as native WHGame CVars. Keep this
// reversible: capture the user's/game's original values once, apply only the
// two documented camera switches, and restore on mode Native/shutdown.
#define KCD2VR_M2_HOST_SLOT 5012 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=5012
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 5013 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=5013
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 5014 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=5014
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 5015 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=5015
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
void V54PhaseBeeps(int phase) noexcept
{
    // Keep the test/game thread unblocked: emit the phase ID on a tiny detached sound thread.
    std::thread([phase](){ for(int i=0;i<phase;i++){ Beep(880,90); if(i+1<phase) Sleep(80); } }).detach();
}
void V54DoneBeep() noexcept { std::thread([](){ Beep(620,180); Beep(1040,260); }).detach(); }
static std::uint64_t g_stallCp178r30[5]{}; // V66.178R30: coarse loop checkpoints for FRAME_STALL_LOOP
void LabLadderCue(int mode) noexcept
{
    // V65.03: one unambiguous cue scheme for every diagnostic ladder.
    // OFF/wrap = one long low tone. Rung N = N short high beeps.
    std::thread([mode](){
        if(mode<=0){ Beep(280,500); return; }
        for(int i=0;i<mode;++i){ Beep(920,95); if(i+1<mode) Sleep(75); }
    }).detach();
}

struct V54StereoCVarSnapshot { bool valid=false; float strength=0, eye=0, screen=0, scale=0; int flip=0; };
V54StereoCVarSnapshot CaptureV54StereoCVars() noexcept {
    V54StereoCVarSnapshot o{}; void* c=SafeGetCryConsole(); if(!c) return o;
    auto F=[&](const char*n){return SafeCVarGetFloat(SafeConsoleGetCVar(c,n),0.0f);};
    auto I=[&](const char*n){return SafeCVarGetInt(SafeConsoleGetCVar(c,n),0);};
    o.strength=F("r_StereoStrength"); o.eye=F("r_StereoEyeDist"); o.screen=F("r_StereoScreenDist"); o.scale=F("r_stereoScaleCoefficient"); o.flip=I("r_StereoFlipEyes"); o.valid=true; return o;
}
bool ApplyV54StereoCVars(float ipd, bool flip) noexcept {
    void* c=SafeGetCryConsole(); if(!c) return false; bool ok=true;
    ok=SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_StereoStrength"),1.0f)&&ok;
    ok=SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_StereoEyeDist"),ipd)&&ok;
    ok=SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_stereoScaleCoefficient"),1.0f)&&ok;
    ok=SafeCVarSetInt(SafeConsoleGetCVar(c,"r_StereoFlipEyes"),flip?1:0)&&ok;
    char m[320]{};std::snprintf(m,sizeof(m),"V54-STEREO-CVARS target={strength:1 eye:%.6f scale:1 flip:%d} ok=%s",(double)ipd,flip?1:0,ok?"true":"false");debug_trace::Write(m);return ok;
}
void RestoreV54StereoCVars(const V54StereoCVarSnapshot&o) noexcept { if(!o.valid)return; void*c=SafeGetCryConsole(); if(!c)return; SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_StereoStrength"),o.strength); SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_StereoEyeDist"),o.eye); SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_StereoScreenDist"),o.screen); SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_stereoScaleCoefficient"),o.scale); SafeCVarSetInt(SafeConsoleGetCVar(c,"r_StereoFlipEyes"),o.flip); }
static V54StereoCVarSnapshot g_v610_manual_stereo_original{};
static bool g_v610_manual_dual_active=false;

struct EngineCVarSnapshot
{
    bool valid = false;
    int custom_width = 0;
    int custom_height = 0;
    int custom_preview = 0;
    int custom_max_size = 0;
    float cl_fov = 0.0f;
};



bool ApplyMenuCustomResolution(int width, int height) noexcept
{
    // V63.1 safety fix: source resolution is STARTUP-ONLY. Never mutate
    // r_Width/r_Height after the D3D12/OpenXR bridge is attached. The Graphics
    // menu writes game-root user.cfg so KCD2 owns renderer creation next launch.
    char msg[768]{};
    const bool ok = SaveSourceResolutionToUserCfg(width, height, msg, sizeof(msg));
    if (msg[0]) { debug_trace::Write(msg); diagnostic_trace::Write(msg); }
    else {
        std::snprintf(msg,sizeof(msg),"V63.1-SOURCE-RES failed saving %dx%d to game-root user.cfg",width,height);
        debug_trace::Write(msg); diagnostic_trace::Write(msg);
    }
    return ok;
}


struct FpsCVarSnapshot
{
    bool valid = false;
    float sys_max_fps = 0.0f;
    int r_vsync = -999;
    int duplicate_last_frame = -999;
};


bool ApplyFpsCap(const FpsCVarSnapshot& original, float cap, char* log_buf, std::size_t log_cap) noexcept
{
    if (!original.valid) return false;
    void* console = SafeGetCryConsole();
    void* maxfps = SafeConsoleGetCVar(console, "sys_MaxFPS");
    if (!console || !maxfps) return false;
    const float target = cap > 1.0f ? cap : original.sys_max_fps;
    const bool ok = SafeCVarSetFloat(maxfps, target);
    const float got = SafeCVarGetFloat(maxfps, -1.0f);
    if (log_buf && log_cap) std::snprintf(log_buf, log_cap,
        "V49-FPS-CAP target=%.3f readback=%.3f original=%.3f ok=%s",
        static_cast<double>(target), static_cast<double>(got), static_cast<double>(original.sys_max_fps), ok ? "true" : "false");
    return ok;
}



bool ApplyDuplicateLastFrame(int value, char* log_buf, std::size_t log_cap) noexcept
{
    void* console = SafeGetCryConsole();
    void* dup = console ? SafeConsoleGetCVar(console, "r_DuplicateLastFrameWH") : nullptr;
    if (!dup) { if(log_buf&&log_cap) std::snprintf(log_buf,log_cap,"V50-DUPLICATE-LAST unavailable"); return false; }
    const bool ok = SafeCVarSetInt(dup, value);
    const int got = SafeCVarGetInt(dup, -999);
    if(log_buf&&log_cap) std::snprintf(log_buf,log_cap,"V50-DUPLICATE-LAST target=%d readback=%d ok=%s",value,got,ok?"true":"false");
    return ok && got==value;
}

// V46.3 safe VR graphics profile. This is intentionally conservative: it only
// applies ordinary CVars already exercised by Warhorse's shipped quality
// presets. It does NOT force stereo-frustum bypass, occlusion, shadow cascade,
// TAA/history, screen-space effect, or HLOD internals. Those remain diagnosis
// knobs until the native eye path is proven.
struct VrGraphicsSnapshot
{
    bool valid = false;
    float view_dist = 0.0f;
    float vegetation_dist = 0.0f;
    float merged_mesh_dist = 0.0f;
    float sharpening = 0.0f;
    int motion_blur = 0;
    int antialiasing_mode = 0;
    int volcloud_temporal = 0;
    int volcloud_stereo = 0;
    float volfog_reprojection_blend = 0.0f;
};

bool CaptureVrGraphicsSnapshot(VrGraphicsSnapshot& out) noexcept
{
    void* console = SafeGetCryConsole();
    if (!console) return false;
    void* view = SafeConsoleGetCVar(console, "e_ViewDistRatio");
    void* veg = SafeConsoleGetCVar(console, "e_ViewDistRatioVegetation");
    void* merged = SafeConsoleGetCVar(console, "e_MergedMeshesViewDistRatio");
    void* sharp = SafeConsoleGetCVar(console, "r_Sharpening");
    void* blur = SafeConsoleGetCVar(console, "r_MotionBlur");
    void* aa = SafeConsoleGetCVar(console, "r_AntialiasingMode");
    void* cloud_t = SafeConsoleGetCVar(console, "r_VolumetricCloudsTemporalReprojection");
    void* cloud_s = SafeConsoleGetCVar(console, "r_VolumetricCloudsStereoReprojection");
    void* fog_blend = SafeConsoleGetCVar(console, "r_VolumetricFogReprojectionBlendFactor");
    if (!view || !veg || !merged || !sharp || !blur || !aa || !cloud_t || !cloud_s || !fog_blend) return false;
    out.view_dist = SafeCVarGetFloat(view, 0.0f);
    out.vegetation_dist = SafeCVarGetFloat(veg, 0.0f);
    out.merged_mesh_dist = SafeCVarGetFloat(merged, 0.0f);
    out.sharpening = SafeCVarGetFloat(sharp, 0.0f);
    out.motion_blur = SafeCVarGetInt(blur, 0);
    out.antialiasing_mode = SafeCVarGetInt(aa, 0);
    out.volcloud_temporal = SafeCVarGetInt(cloud_t, 0);
    out.volcloud_stereo = SafeCVarGetInt(cloud_s, 0);
    out.volfog_reprojection_blend = SafeCVarGetFloat(fog_blend, 0.0f);
    // Reject a bad/exception fallback snapshot rather than ever replacing an
    // unknown renderer state with our defaults.
    if (out.view_dist <= 0.0f || out.vegetation_dist <= 0.0f || out.merged_mesh_dist <= 0.0f)
        return false;
    out.valid = true;
    return true;
}

bool ApplyVrBalancedGraphics(const VrGraphicsSnapshot& original, float clarity,
                             char* log_buf, std::size_t log_cap) noexcept
{
    if (!original.valid) return false;
    void* console = SafeGetCryConsole();
    if (!console) return false;
    void* view = SafeConsoleGetCVar(console, "e_ViewDistRatio");
    void* veg = SafeConsoleGetCVar(console, "e_ViewDistRatioVegetation");
    void* merged = SafeConsoleGetCVar(console, "e_MergedMeshesViewDistRatio");
    void* sharp = SafeConsoleGetCVar(console, "r_Sharpening");
    void* blur = SafeConsoleGetCVar(console, "r_MotionBlur");
    void* aa = SafeConsoleGetCVar(console, "r_AntialiasingMode");
    void* cloud_t = SafeConsoleGetCVar(console, "r_VolumetricCloudsTemporalReprojection");
    void* cloud_s = SafeConsoleGetCVar(console, "r_VolumetricCloudsStereoReprojection");
    void* fog_blend = SafeConsoleGetCVar(console, "r_VolumetricFogReprojectionBlendFactor");
    if (!view || !veg || !merged || !sharp || !blur || !aa || !cloud_t || !cloud_s || !fog_blend) return false;

    // V66.23: this was accidentally std::max(), so the nominal 120 baseline never
    // reduced KCD2's normal 140 view distances. Preserve lower user values but cap
    // higher values at 120 for the double-render VR path.
    const float target_view = std::min(original.view_dist, 120.0f);
    const float target_veg = std::min(original.vegetation_dist, 120.0f);
    const float target_merged = std::min(original.merged_mesh_dist, 120.0f);
    const float requested_clarity = std::clamp(clarity, 0.0f, 0.75f);
    const float target_sharp = std::max(original.sharpening, requested_clarity);

    bool ok = true;
    ok = SafeCVarSetFloat(view, target_view) && ok;
    ok = SafeCVarSetFloat(veg, target_veg) && ok;
    ok = SafeCVarSetFloat(merged, target_merged) && ok;
    ok = SafeCVarSetInt(blur, 0) && ok; // no camera/object motion blur in VR baseline
    ok = SafeCVarSetFloat(sharp, target_sharp) && ok;
    // KCD2 default AA=3 is temporal SMAA 2TX. Native L/R currently share the
    // ordinary temporal history path, producing the translucent/smear image seen
    // in the headset. Mode 1 is spatial SMAA 1X: antialiasing without temporal history.
    // V66.85: AA/upscaler mode is player-owned; no r_AntialiasingMode write.
    // V66.79a SKYFIX: NEVER write cloud/fog reprojection CVars. Headset testing
    // proved that even same-value writes can blow out the sky. KCD2/user config owns
    // these three values; the VR mod only observes them.

    const float got_view = SafeCVarGetFloat(view, -1.0f);
    const float got_veg = SafeCVarGetFloat(veg, -1.0f);
    const float got_merged = SafeCVarGetFloat(merged, -1.0f);
    const int got_blur = SafeCVarGetInt(blur, -1);
    const float got_sharp = SafeCVarGetFloat(sharp, -1.0f);
    const int got_aa = SafeCVarGetInt(aa, -1);
    const int got_cloud_t = SafeCVarGetInt(cloud_t, -1);
    const int got_cloud_s = SafeCVarGetInt(cloud_s, -1);
    const float got_fog_blend = SafeCVarGetFloat(fog_blend, -1.0f);
    if (log_buf && log_cap) {
        std::snprintf(log_buf, log_cap,
            "V66.79a-VR-GRAPHICS original={view:%.1f veg:%.1f merged:%.1f blur:%d aa:%d cloudT:%d cloudS:%d fogBlend:%.3f} target={view:%.1f veg:%.1f merged:%.1f blur:0 aa:PLAYER-OWNED skyReproj:UNTOUCHED} readback={view:%.1f veg:%.1f merged:%.1f blur:%d aa:%d cloudT:%d cloudS:%d fogBlend:%.3f sharp:%.3f} ok=%s",
            double(original.view_dist),double(original.vegetation_dist),double(original.merged_mesh_dist),original.motion_blur,
            original.antialiasing_mode,original.volcloud_temporal,original.volcloud_stereo,double(original.volfog_reprojection_blend),
            double(target_view),double(target_veg),double(target_merged),double(got_view),double(got_veg),double(got_merged),
            got_blur,got_aa,got_cloud_t,got_cloud_s,double(got_fog_blend),double(got_sharp),ok?"true":"false");
    }
    return ok;
}

bool RestoreVrGraphics(const VrGraphicsSnapshot& original, char* log_buf, std::size_t log_cap) noexcept
{
    if (!original.valid) return false;
    void* console = SafeGetCryConsole();
    if (!console) return false;
    void* view = SafeConsoleGetCVar(console, "e_ViewDistRatio");
    void* veg = SafeConsoleGetCVar(console, "e_ViewDistRatioVegetation");
    void* merged = SafeConsoleGetCVar(console, "e_MergedMeshesViewDistRatio");
    void* sharp = SafeConsoleGetCVar(console, "r_Sharpening");
    void* blur = SafeConsoleGetCVar(console, "r_MotionBlur");
    void* aa = SafeConsoleGetCVar(console, "r_AntialiasingMode");
    void* cloud_t = SafeConsoleGetCVar(console, "r_VolumetricCloudsTemporalReprojection");
    void* cloud_s = SafeConsoleGetCVar(console, "r_VolumetricCloudsStereoReprojection");
    void* fog_blend = SafeConsoleGetCVar(console, "r_VolumetricFogReprojectionBlendFactor");
    if (!view || !veg || !merged || !sharp || !blur || !aa || !cloud_t || !cloud_s || !fog_blend) return false;
    bool ok = true;
    ok = SafeCVarSetFloat(view, original.view_dist) && ok;
    ok = SafeCVarSetFloat(veg, original.vegetation_dist) && ok;
    ok = SafeCVarSetFloat(merged, original.merged_mesh_dist) && ok;
    ok = SafeCVarSetInt(blur, original.motion_blur) && ok;
    ok = SafeCVarSetFloat(sharp, original.sharpening) && ok;
    // V66.85: AA/upscaler mode is player-owned; no restore write.
    // V66.79a SKYFIX: cloud/fog reprojection CVars are game-owned and never restored/written by the mod.
    if (log_buf && log_cap) {
        std::snprintf(log_buf, log_cap,
            "V46.3-GRAPHICS-RESTORE view=%.2f veg=%.2f merged=%.2f blur=%d sharp=%.3f ok=%s",
            static_cast<double>(original.view_dist), static_cast<double>(original.vegetation_dist),
            static_cast<double>(original.merged_mesh_dist), original.motion_blur,
            static_cast<double>(original.sharpening), ok ? "true" : "false");
    }
    return ok;
}



struct V6626EffectSnapshot {
    bool valid=false;
    int shadow_update=1;
    int shadows=1;
    int shadows_screen=1;
    int shadows_mask=1;
    int shadow_cascade_reproj_wh=0;
    float shadow_jitter_multiplier_wh=1.0f;
    int hdr_bloom=1;
    int hdr_eye=1;
    int hdr_range=0;
    int clouds=1;
    int vol_clouds=2;
    int cloud_temporal=1;
    int cloud_stereo=1;
    float fog_blend=-1.0f; // V66.33: captured r_VolumetricFogReprojectionBlendFactor (-1 = unreadable)
    int aa=3;
    // V66.37: retail KCD2 exposes native TAA tuning controls. Capture the user's/game's
    // originals so every diagnostic rung is reversible rather than baking in guessed defaults.
    int taa_pattern=1;
    float tsaa_subpixel=0.0f;
    float tsaa_smoothness=0.0f;
    int taa_sharpening=0;
    float taa_falloff_hi=10.0f;
    float taa_falloff_low=2.0f;
    float tsaa_mip_bias=0.0f;
};




// V66.40 simple-fix lab. These are ordinary live CVars, but every rung captures the
// user's current values first and verifies exact readback. Rung 0 restores that capture.
// No guessed renderer pointers or unverified native calls are involved.
static std::atomic<bool> g_v6652_owns_contact_shadows{false}; // set while an F8 rung turns contact shadows off
// V66.52 F8 screen-space / depth-based effect ladder (live CVars, captured and restored).
// Artifacts sit at depth discontinuities (plank gaps, stair edges, water edges), gone at zero IPD.
static const char* const kV6652Cvars[]={
    "r_ShadowsScreenSpace",      // 0 contact shadows (screen-space, depth traced)
    "r_Refraction",              // 1
    "r_WaterReflections",        // 2 cached planar water reflections
    "r_WaterCaustics",           // 3
    "r_WaterVolumeCaustics",     // 4
    "r_WaterCausticsDeferred",   // 5
    "r_Reflections",             // 6
    "r_DeferredDecals",          // 7
    "r_DeferredShadingSSS",      // 8
    "e_ParticlesSoftIntersect",  // 9
    "r_ParticlesRefraction",     // 10
    "e_svoTI_Apply",             // 11 V66.53: the CVar the SVOGI stage gate actually reads (0x180575644)
};
constexpr int kV6652CvarCount=int(sizeof(kV6652Cvars)/sizeof(kV6652Cvars[0]));
struct V6652Rung { const char* name; std::uint32_t mask; };
static const V6652Rung kV6652Rungs[]={
    {"CAPTURED",0u},
    {"SVO-APPLY-OFF(e_svoTI_Apply)",1u<<11},
    {"CONTACT-SHADOWS-OFF(r_ShadowsScreenSpace)",1u<<0},
    {"REFRACTION-OFF(r_Refraction)",1u<<1},
    {"WATER-REFLECTIONS-OFF",1u<<2},
    {"CAUSTICS-OFF(water+volume+deferred)",(1u<<3)|(1u<<4)|(1u<<5)},
    {"REFLECTIONS-OFF(r_Reflections)",1u<<6},
    {"DEFERRED-DECALS-OFF",1u<<7},
    {"SSS-OFF(r_DeferredShadingSSS)",1u<<8},
    {"PARTICLE-SOFT+REFRACTION-OFF",(1u<<9)|(1u<<10)},
    {"ALL-OFF",(1u<<kV6652CvarCount)-1u},
};
constexpr int kV6652RungCount=int(sizeof(kV6652Rungs)/sizeof(kV6652Rungs[0]));
struct V6652Snapshot { bool valid=false; bool present[16]{}; float value[16]{}; };

// V66.52: log the LIVE values of settings that user.cfg may not control (menu can override).
void V6652LogAuthority(const char* when) noexcept {
    void* c=SafeGetCryConsole(); if(!c) return;
    static const char* const names[]={"r_SuperResolution_Mode","r_AntialiasingMode","r_SuperResolution_NVIDIA_DLSS_QualityMode",
        "r_SuperResolution_AMD_FSR_QualityMode","r_SuperResolution_INTEL_XeSS_QualityMode","r_DuplicateLastFrameWH",
        "r_DuplicateLastFrameOnCameraChangeWH","r_SSReflections","e_svoTI_Active","e_svoTI_Apply","r_Width","r_Height"};
    char m[900]{}; int n=std::snprintf(m,sizeof(m),"V66.52-LIVE-CVARS when=%s",when?when:"");
    for(const char* nm:names){ if(n<=0||n>=int(sizeof(m))-64) break;
        void* v=SafeConsoleGetCVar(c,nm);
        n+= v?std::snprintf(m+n,sizeof(m)-n," %s=%.2f",nm,double(SafeCVarGetFloat(v,-999.0f))):std::snprintf(m+n,sizeof(m)-n," %s=MISSING",nm); }
    diagnostic_trace::Write(m);
}


// V66.44 F8 RIGHT-pass stage elimination ladder. Rungs: 0 OFF, 1 ALL, 2-4 groups,
// 5.. single stages (table order). Only the RIGHT pass is affected; judge the LEFT eye.
struct V6644BisectRung { const char* name; std::uint32_t mask; };
static const V6644BisectRung kV6644Rungs[] = {
    // V66.45: V66.44 crashed on ALL-15 (WHGame+0x1DDB180, right after the press). Groups now
    // use ONLY gate-level skips (IsStageActive=false, the engine's own inactive path).
    // The four Execute-level skips come last, one at a time, marked RISKY (save first).
    {"OFF",0u},
    {"ALL-GATES-11",0x7E76u},
    {"GROUP-LIGHT-GATES(ssoAO,heightmapAO,decals)",0x206u},
    {"GROUP-ATMOS-WATER(volFog,fog,water,sunShafts,rain)",0x1870u},
    {"GROUP-POST(sss,bloom,motionBlurPrep)",0x6400u},
    {"SSO-AO",1u<<1},{"HEIGHTMAP-AO",1u<<2},{"VOLUMETRIC-FOG",1u<<4},{"FOG",1u<<5},{"WATER",1u<<6},
    {"DEFERRED-DECALS",1u<<9},{"SCREEN-SPACE-SSS",1u<<10},{"SUN-SHAFTS",1u<<11},{"RAIN",1u<<12},
    {"BLOOM",1u<<13},{"MOTIONBLUR-PREP",1u<<14},
    {"RISKY-EXEC-SHADOW-MASK",1u<<0},{"RISKY-EXEC-SVOGI",1u<<3},
    {"RISKY-EXEC-SCENE-FORWARD",1u<<7},{"RISKY-EXEC-TILED-LIGHT-VOLUMES",1u<<8},
};
constexpr int kV6644RungCount=int(sizeof(kV6644Rungs)/sizeof(kV6644Rungs[0]));

// V66.43 F8 temporal-shadow isolation ladder. V66.42 test: SVOGI rungs changed nothing,
// r_SSReflections already 0, local-light distance/budget (V66.40) changed nothing.
// Remaining shadow systems that depend on frame-to-frame history (and now see two eye
// passes per frame): shadow jitter, alternating-frame cascade reprojection, motion bias,
// and the time-sliced local-light shadow pool. Values are read/written as float
// (ICVar Set/GetFVal work for int CVars too); rung 0 restores the capture.
// Set while an F8 shadow rung (1..5) is active, so the 2 s post-FX re-apply
// (ApplyV6626EffectLadder) does not silently restore the two WH shadow CVars.
static std::atomic<bool> g_v6643_shadow_ladder_owns{false};

struct V6643ShadowTemporalSnapshot {
    bool valid=false;
    float jitter=0, jitter_mult=0, cascade_reproj=0, motion_bias=0, pool_budget=0;
};
static const char* const kV6643Names[5]={
    "r_ShadowJittering","r_ShadowJitteringMultiplierWH","r_ShadowCascadesReprojectionWH",
    "r_ShadowsMotionBiasWH","r_ShadowPoolBudgetPerFrameWH"};




// V66.41 F8 reflection/GI isolation ladder. User reports shimmer/flicker on shiny
// surfaces (water, stairs) with r_SSReflections already 0, and missing clouds in LEFT
// reflections. Retail keeps per-eye history only for AA and clouds; SVOGI specular and
// its temporal filter are shared between the eyes. Every rung captures live values first,
// writes documented CVars only, verifies readback; rung 0 restores the capture.
struct V6641ReflGiSnapshot {
    bool valid=false;
    int svo_active=1;
    float svo_specular=1.0f;
    float svo_temporal_base=0.0f;
    int ssr=-1; // logged only
};




struct V6640LightShadowSnapshot {
    bool valid=false;
    float view_dist_lights=0.0f;
    float shadow_cast_view_dist_lights=0.0f;
    int vertical_light_culling=0;
    int shadow_casting_budget=0;
    int shadow_casting_max_count=0;
    int shadow_pool_base_cost=0;
    int shadow_pool_budget_per_frame=0;
};


const char* V6640LightShadowStepName(int step) noexcept
{
    switch(step){
        case 0:return "CAPTURED";
        case 1:return "LIGHT-DIST-30";
        case 2:return "SHADOW-LIGHT-DIST-0.15";
        case 3:return "VERTICAL-CULL-3";
        case 4:return "RETAIL-LIKE-30/0.15/3/B12";
        case 5:return "CONSERVATIVE-24/0.10/3/B12";
        default:return "UNKNOWN";
    }
}

bool ApplyV6640LightShadowStep(const V6640LightShadowSnapshot& orig,int step,char* log_buf,std::size_t log_cap) noexcept
{
    if(!orig.valid) return false;
    step=((step%6)+6)%6;
    void* c=SafeGetCryConsole(); if(!c) return false;
    auto cv=[&](const char* n)->void*{ return SafeConsoleGetCVar(c,n); };
    void* view=cv("e_ViewDistRatioLights");
    void* cast=cv("e_ShadowsCastViewDistRatioLights");
    void* vert=cv("wh_e_VerticalLightCulling");
    void* budget=cv("r_ShadowCastingLightsBudgetWH");
    void* maxcount=cv("r_ShadowCastingLightsMaxCount");
    if(!view||!cast||!vert||!budget||!maxcount) return false;

    float tv=orig.view_dist_lights, tc=orig.shadow_cast_view_dist_lights;
    int tvert=orig.vertical_light_culling, tb=orig.shadow_casting_budget, tmax=orig.shadow_casting_max_count;
    if(step==1) tv=30.0f;
    else if(step==2) tc=0.15f;
    else if(step==3) tvert=3;
    else if(step==4) {tv=30.0f;tc=0.15f;tvert=3;tb=12;tmax=32;}
    else if(step==5) {tv=24.0f;tc=0.10f;tvert=3;tb=12;tmax=32;}

    bool ok=true;
    ok=SafeCVarSetFloat(view,tv)&&ok;
    ok=SafeCVarSetFloat(cast,tc)&&ok;
    ok=SafeCVarSetInt(vert,tvert)&&ok;
    ok=SafeCVarSetInt(budget,tb)&&ok;
    ok=SafeCVarSetInt(maxcount,tmax)&&ok;
    const float gv=SafeCVarGetFloat(view,-999.0f), gc=SafeCVarGetFloat(cast,-999.0f);
    const int gvert=SafeCVarGetInt(vert,-999), gb=SafeCVarGetInt(budget,-999), maxc=SafeCVarGetInt(maxcount,-999);
    ok=ok && std::fabs(gv-tv)<0.011f && std::fabs(gc-tc)<0.0011f && gvert==tvert && gb==tb && maxc==tmax;
    const int poolbase = cv("r_ShadowPoolBaseCostWH")?SafeCVarGetInt(cv("r_ShadowPoolBaseCostWH"),-999):-999;
    const int poolframe = cv("r_ShadowPoolBudgetPerFrameWH")?SafeCVarGetInt(cv("r_ShadowPoolBudgetPerFrameWH"),-999):-999;
    if(log_buf&&log_cap) std::snprintf(log_buf,log_cap,
        "V66.40-F8-LIGHT-SHADOW step=%d(%s) target={view:%.3f shadowDist:%.3f vertical:%d budget:%d maxCount:%d} readback={view:%.3f shadowDist:%.3f vertical:%d budget:%d maxCount:%d} untouched={poolBase:%d poolFrame:%d} captured={view:%.3f shadowDist:%.3f vertical:%d budget:%d maxCount:%d} result=%s",
        step,V6640LightShadowStepName(step),double(tv),double(tc),tvert,tb,tmax,double(gv),double(gc),gvert,gb,maxc,poolbase,poolframe,
        double(orig.view_dist_lights),double(orig.shadow_cast_view_dist_lights),orig.vertical_light_culling,orig.shadow_casting_budget,orig.shadow_casting_max_count,ok?"ACTIVE":"NOT-ACTIVE");
    return ok;
}

struct V6640AoSnapshot {
    bool valid=false;
    int ssdo=0;
    int height_map_ao=0;
    float svo_ssao_amount=0.0f;
};


const char* V6640AoStepName(int step) noexcept
{
    switch(step){
        case 0:return "CAPTURED";
        case 1:return "SSDO-OFF";
        case 2:return "HEIGHTMAP-AO-OFF";
        case 3:return "SVO-SSAO-OFF";
        case 4:return "ALL-AO-OFF";
        default:return "UNKNOWN";
    }
}

bool ApplyV6640AoStep(const V6640AoSnapshot& orig,int step,char* log_buf,std::size_t log_cap) noexcept
{
    if(!orig.valid) return false;
    step=((step%5)+5)%5;
    void* c=SafeGetCryConsole(); if(!c) return false;
    void* ssdo=SafeConsoleGetCVar(c,"r_ssdo");
    void* hmao=SafeConsoleGetCVar(c,"r_HeightMapAO");
    void* svo=SafeConsoleGetCVar(c,"e_svoTI_SSAOAmount");
    if(!ssdo||!hmao||!svo) return false;
    int ts=orig.ssdo, th=orig.height_map_ao; float ta=orig.svo_ssao_amount;
    if(step==1) ts=0;
    else if(step==2) th=0;
    else if(step==3) ta=0.0f;
    else if(step==4) {ts=0;th=0;ta=0.0f;}
    bool ok=true;
    ok=SafeCVarSetInt(ssdo,ts)&&ok;
    ok=SafeCVarSetInt(hmao,th)&&ok;
    ok=SafeCVarSetFloat(svo,ta)&&ok;
    const int gs=SafeCVarGetInt(ssdo,-999), gh=SafeCVarGetInt(hmao,-999);
    const float ga=SafeCVarGetFloat(svo,-999.0f);
    ok=ok && gs==ts && gh==th && std::fabs(ga-ta)<0.0011f;
    if(log_buf&&log_cap) std::snprintf(log_buf,log_cap,
        "V66.40-F5-AO step=%d(%s) target={ssdo:%d heightMapAO:%d svoSSAO:%.3f} readback={ssdo:%d heightMapAO:%d svoSSAO:%.3f} captured={ssdo:%d heightMapAO:%d svoSSAO:%.3f} result=%s",
        step,V6640AoStepName(step),ts,th,double(ta),gs,gh,double(ga),orig.ssdo,orig.height_map_ao,double(orig.svo_ssao_amount),ok?"ACTIVE":"NOT-ACTIVE");
    return ok;
}



// C51R1: shared camera/body pose ownership controls optional movement smoothing.
// Do not zero global camera CVars: OFF preserves native gait policy, and enemy
// hit/stagger/cinematic shake remains the game's property in either mode.

// V66.34 performance: skip per-draw / per-descriptor diagnostic tracing (PSO/mode probe,
// SRV/descriptor maps, draw census, V66.24 post-FX resource census). None of it feeds the
// live L/R pair detector (that uses OMSetRenderTargets/ResourceBarrier + the RTV map,
// which stay active). Set to false and rebuild if a post-FX/UI-descriptor census is needed.
constexpr bool kV6634ProductionPerfQuiet = true;
std::atomic<int> g_vrik_compare_requested_d61{-1}; // UI publishes; Run owns comparison state





struct WindowSearchContext
{
    DWORD pid = 0;
    HWND best = nullptr;
    long long best_area = 0;
};

BOOL CALLBACK FindProcessWindowProc(HWND hwnd, LPARAM param) noexcept
{
    auto* ctx = reinterpret_cast<WindowSearchContext*>(param);
    if (!ctx || !IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER) != nullptr) return TRUE;
    DWORD pid = 0; GetWindowThreadProcessId(hwnd, &pid);
    if (pid != ctx->pid) return TRUE;
    RECT r{}; if (!GetClientRect(hwnd, &r)) return TRUE;
    const long long w = static_cast<long long>(r.right - r.left);
    const long long h = static_cast<long long>(r.bottom - r.top);
    const long long area = w * h;
    if (w >= 640 && h >= 480 && area > ctx->best_area) {
        ctx->best = hwnd;
        ctx->best_area = area;
    }
    return TRUE;
}

HWND FindMainGameWindow() noexcept
{
    static HWND cached=nullptr;
    static std::uint64_t last_scan_ms=0;
    const std::uint64_t now=static_cast<std::uint64_t>(GetTickCount64());
    if(cached && IsWindow(cached) && now-last_scan_ms<2000ull) return cached;
    WindowSearchContext ctx{};
    ctx.pid = GetCurrentProcessId();
    EnumWindows(&FindProcessWindowProc, reinterpret_cast<LPARAM>(&ctx));
    cached=ctx.best; last_scan_ms=now;
    return cached;
}

bool IsGameUiCursorVisible(HWND hwnd) noexcept
{
    if (hwnd == nullptr) {
        return false;
    }
    if (GetForegroundWindow() != hwnd) {
        return false;
    }

    CURSORINFO ci{};
    ci.cbSize = sizeof(CURSORINFO);
    if (!GetCursorInfo(&ci)) {
        return false;
    }
    if ((ci.flags & CURSOR_SHOWING) == 0) {
        return false;
    }

    POINT clientPoint = ci.ptScreenPos;
    if (!ScreenToClient(hwnd, &clientPoint)) {
        return false;
    }

    RECT clientRect{};
    if (!GetClientRect(hwnd, &clientRect)) {
        return false;
    }

    const bool insideX = clientPoint.x >= clientRect.left && clientPoint.x < clientRect.right;
    const bool insideY = clientPoint.y >= clientRect.top && clientPoint.y < clientRect.bottom;
    return insideX && insideY;
}

} // namespace

constexpr float kV6504DefaultGameFov = 95.0f; // V66.31: historical neutral default; user selects Game FOV explicitly
constexpr float kV6504DefaultSourceFov = 95.0f; // V66.31: historical neutral default; CORRECT FOV recalculates any chosen Game/VR pair
constexpr float kV6504DefaultZoom = 0.95f;

// Mode 1 is V65.17 PROJECTIVE FOV REMAP. Legacy INI field names are retained
// for backwards compatibility, but there is no fixed 120/95 Corrected-Mono pair.
constexpr float kV6511CorrectedCenterScaleFov = 95.0f; // legacy storage only
constexpr float kV6508CorrectedZoom = 1.0f;            // REMAP must not double-scale

static float EffectiveSourceFovForMode(int mode, float configured) noexcept
{
    (void)mode;
    return std::clamp(configured,70.0f,140.0f);
}

static float EffectiveZoomForMode(int mode, float configured) noexcept
{
    if(mode==1) return 1.0f;
    return std::clamp(configured,0.85f,1.20f);
}

struct V603ViewPreset {
    int presentation_mode;
    float fov_asymmetry;
    float uv_binocular;
    bool angular_correct;
    int pose_honesty;
    const char* name;
};

static V603ViewPreset ViewPresetForMode(int mode) noexcept
{
    // V65.17: Normal Mono/Pseudostereo retain the V65.01 mapping. Mode 1 is a
    // mathematically projective tangent-space FOV REMAP; no peripheral easing.
    switch (mode) {
    case 1: return {3, 0.00f, 0.000f, true, 1, "CORRECT_FOV_USER_GAME_TO_USER_VR"};
    case 2: return {3, 0.90f, 0.000f, true, 1, "PSEUDO_090"};
    case 3: return {3, 1.00f, 0.000f, true, 1, "PSEUDO_DEPTH_070_055"};
    default:return {3, 0.00f, 0.000f, true, 1, "MONO_OLD"};
    }
}

static int MonoMapModeForCleanMode(int mode) noexcept
{
    // V65.27: Mono=old map0, Pseudostereo=old depth map4. Correct FOV is
    // isolated in map5 and does not alter either production fallback path.
    if (mode == 1) return 5;
    return mode >= 2 ? 4 : 0;
}

static float CyclopeanAsymmetryForMode(int mode) noexcept
{
    return ViewPresetForMode(mode).fov_asymmetry;
}

Mod& Mod::Instance()
{
    static Mod instance;
    return instance;
}

void Mod::FillMenuState(VrMenuState& out) const
{
    out.fast_block_r92=release_combat_r92::fastBlock.load();
#define KCD2VR_D40_LIVE_READ
#include "subsystems/experimental/live_runtime_d40.inl"
#undef KCD2VR_D40_LIVE_READ
    // D39: read the actual owners on every menu snapshot. No stale INI mirrors.
    auto& live=out.live_d39;
    live.shoulderWarningM=frik_pose_probe::GetShoulderReachD38();
    live.handsMode=frik_pose_probe::HandsModeDs2();
    live.handSeparation=frik_pose_probe::HandSeparationDs1();
    live.nativeHandPhysics=frik_pose_probe::NativeHandPhysicsH24();
    live.handSmoothing=smooth_lab::BaselineSmoothingD39();live.handSmoothingPaused=smooth_lab::Mode()>=2;
    live.workstationGestures=workstation178r20c1::gesturesEverywhere59c2.load();
    live.petGestureD59=petting_d59::Enabled();live.washGestureD59=washing_d59::Enabled();
    live.alchemyTransferD59=misc_gestures_d59::alchemyTransfer.load();
    live.alchemyPickupReach=subsystems::alchemy::reach_craft1::getPickupReachD39();
    live.smithToolCollision=station_ws_j27::toolCollision.load();
    live.stationTorso=station_ws_j27::torsoEnabled.load();
    live.stationClearCm=station_ws_j27::clearCm.load();
    live.torchGesture=utility54::torchGestureD39.load();
    live.visorGesture=utility54::visorGestureD39.load();
    live.parchmentMap=g_held_map59c10r10.load();live.parchmentReady=held_map_m1::Selected();
    live.cameraHoldMs=int(camera_hold_e1::maxHoldMs.load());
    live.bowArmAim=bow_arm_aim_r9::enabled.load();live.labPropWake=lab58::propWake.load();
    live.fingerTrial=deepseek_h29::fingerContactSpringH31.load();
    live.alchemyEyeTrial=alchemy_own_head_h51::enabled.load();
    live.gripStowTrial=grip_stow_cb1::Enabled();live.gripStowButton=grip_stow_cb1::Button();
    out.enabled = enabled_.load(std::memory_order_acquire);
    out.bridge_enabled = enable_vr_output_bridge_.load(std::memory_order_acquire);
    out.head_rotation = apply_head_rotation_.load(std::memory_order_acquire);
    out.head_position = apply_head_position_.load(std::memory_order_acquire);
    out.diagnostic_rendering = diagnostic_rendering_.load(std::memory_order_acquire);
    out.swap_eyes = vr_swap_eyes_.load(std::memory_order_acquire);
    out.rotation_order = vr_rotation_order_.load(std::memory_order_acquire);
    out.fake_dof_active = fake_dof_active_.load(std::memory_order_acquire);
    out.fake_dof_scale = fake_dof_scale_.load(std::memory_order_acquire);
    out.engine_dof_active = engine_dof_active_.load(std::memory_order_acquire);
    out.engine_dof_available = engine_dof_available_.load(std::memory_order_acquire);
    out.engine_dof_focus_m = engine_dof_focus_m_.load(std::memory_order_acquire);
    out.engine_dof_scale = engine_dof_scale_.load(std::memory_order_acquire);
    out.depth_stereo_strength = vr_depth_stereo_strength_.load(std::memory_order_acquire);
    out.head_pose_scale = head_pose_scale_.load(std::memory_order_acquire);
    out.camera_side_offset_m = camera_side_offset_m_.load(std::memory_order_acquire);
    out.camera_back_offset_m = camera_back_offset_m_.load(std::memory_order_acquire);
    out.camera_height_offset_m = camera_height_offset_m_.load(std::memory_order_acquire);
    out.game_fov_target = menu_game_fov_target_.load(std::memory_order_acquire);
    out.horizontal_fov_target = menu_horizontal_fov_target_.load(std::memory_order_acquire);
    out.fps_cap_enabled = menu_fps_cap_enabled_.load(std::memory_order_acquire);
    out.fps_cap_target = menu_fps_cap_target_.load(std::memory_order_acquire);
    out.merged_mesh_target = menu_merged_mesh_target_.load(std::memory_order_acquire);
    out.auto_projection_active = vr_auto_projection_active_.load(std::memory_order_acquire);
    out.auto_fov_trim_deg = vr_auto_fov_trim_deg_.load(std::memory_order_acquire);
    out.auto_center_x_trim = vr_auto_center_x_trim_.load(std::memory_order_acquire);
    out.auto_center_y_trim = vr_auto_center_y_trim_.load(std::memory_order_acquire);
    out.auto_projection_center = vr_auto_projection_center_.load(std::memory_order_acquire);
    out.projection_pitch_trim_deg = vr_pitch_correction_deg_.load(std::memory_order_acquire);
    out.projection_pitch_effective_deg = xr_manager_ ? xr_manager_->GetEffectivePitchCorrectionDeg() : out.projection_pitch_trim_deg;
    out.presentation_mode = vr_presentation_mode_.load(std::memory_order_acquire);
    out.presentation_zoom = vr_zoom_.load(std::memory_order_acquire);
    out.source_vertical_fov_deg = vr_source_vertical_fov_deg_.load(std::memory_order_acquire);
    out.corrected_center_scale_fov_deg = vr_corrected_view_vfov_deg_.load(std::memory_order_acquire);
    out.presentation_center_x = vr_presentation_center_x_.load(std::memory_order_acquire);
    out.presentation_center_y = vr_presentation_center_y_.load(std::memory_order_acquire);
    out.cull_frustum_scale = vr_cull_frustum_scale_.load(std::memory_order_acquire);
#define KCD2VR_M2_HOST_SLOT 1 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=1
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 2 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=2
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 3 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=3
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.flat_screen_scale = vr_flat_screen_scale_.load(std::memory_order_acquire);
    out.passive_full_frame_scale = vr_passive_full_frame_scale_.load(std::memory_order_acquire);
    out.hud_distance_r110 = kcd2vr::ui_surface_geometry::HudDistanceR110(); // R110
    out.combat_crosshair_scale_r112 = kcd2vr::ui_hud_r112::CombatCrosshairScale(); // R112
    out.pause_overlay_size = vr_pause_overlay_size_.load(std::memory_order_acquire);
    out.transient_zoom_retain = vr_transient_zoom_retain_.load(std::memory_order_acquire);
#define KCD2VR_M2_HOST_SLOT 4 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=4
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.stereo_mode = vr_production_stereo_mode_.load(std::memory_order_acquire);
    out.cyclopean_mode = vr_cyclopean_mode_.load(std::memory_order_acquire);
    out.afr_ipd_scale = vr_afr_ipd_scale_.load(std::memory_order_acquire);
    out.balanced_graphics = enable_vr_balanced_graphics_.load(std::memory_order_acquire);
    out.fix_head_sync = (vr_pose_source_mode_.load(std::memory_order_acquire) == 2);
    out.f2_ownership_fix = f2_ownership_fix_.load(std::memory_order_acquire);
#define KCD2VR_M2_HOST_SLOT 5 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=5
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 6 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=6
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.activity_hands59c7f[0]=activity_input163::trackedHands59c7f[0].load();
    out.activity_hands59c7f[1]=activity_input163::trackedHands59c7f[1].load();
    out.activity_hands59c7f[2]=activity_input163::trackedHands59c7f[3].load();
    out.activity_hands59c7f[3]=activity_input163::trackedHands59c7f[2].load();
#define KCD2VR_M2_HOST_SLOT 7 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=7
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.precise_contacts44=frik_pose_probe::PreciseContacts44();
#define KCD2VR_M2_HOST_SLOT 8 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=8
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 9 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=9
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.calibration_auto_restore43=frik_pose_probe::CalibrationAutoRestore43(); // C43
    out.smith_auto_hold36=workstation178r20c1::smithAutoHold36.load(std::memory_order_acquire); // C37
    out.smith_anvil_mode_a3=subsystems::smithing::anvil_a2::mode.load(std::memory_order_acquire); // A3
    out.minigame_hands45=minigame_hands45::clampMode(minigame_hands45::mode.load());
    out.alchemy_mode67=minigame_hands45::modeFor(minigame_hands45::Alchemy);
    out.smithing_mode67=minigame_hands45::modeFor(minigame_hands45::Smithing);
#define KCD2VR_M2_HOST_SLOT 10 // M2_SOURCE_SELECTOR
#include "subsystems/doors/host.inl" // M2_OWNED_SOURCE slot=10
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.grab_hand_stays43=frik_pose_probe::GrabHandStays43();
    out.grab_mode60=frik_pose_probe::GrabMode60(); // C43
    out.native_activity_controls163 = native_activity_controls163_.load();
#define KCD2VR_M2_HOST_SLOT 11 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=11
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 12 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=12
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 13 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=13
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 14 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=14
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 15 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=15
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 16 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=16
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 17 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=17
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 18 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=18
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 19 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=19
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 20 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=20
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.graphics_profile_applied = graphics_profile_applied_for_ui_.load(std::memory_order_acquire);
    out.render_scale = vr_render_scale_.load(std::memory_order_acquire);
    out.source_width = vr_source_width_.load(std::memory_order_acquire);
    out.source_height = vr_source_height_.load(std::memory_order_acquire);
    out.engine_clarity = vr_engine_clarity_.load(std::memory_order_acquire);
    out.gamma = vr_gamma_.load(std::memory_order_acquire);
    out.brightness = vr_brightness_.load(std::memory_order_acquire);
    out.contrast = vr_contrast_.load(std::memory_order_acquire);
#define KCD2VR_M2_HOST_SLOT 21 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=21
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 22 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=22
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 23 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=23
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 24 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=24
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 25 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=25
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 26 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=26
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.vr_turn_mode = vr_turn_mode_.load(std::memory_order_acquire);
#define KCD2VR_M2_HOST_SLOT 27 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=27
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 28 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=28
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.free_hand_present_d61=hand_present_d61::Enabled();
    out.native_finisher_test_d61=finisher_cb1::nativeValidationTestD61.load(std::memory_order_acquire);
    out.vrik_comparison_d61=int(motion_trials_d67::MenuChoiceForMode(motion_trials_d67::MenuMode()));
    for(int h=0;h<2;++h)for(int f=0;f<5;++f)for(int k=0;k<4;++k)out.fingers40[h][f][k]=finger40::get(h,f,k);
    for(unsigned c=0;c<melee_tuning_c54::classCount;++c)out.melee_types_c54[c]=melee_tuning_c54::read(int(c));
    out.independent_thrust_d67=thrust_tuning_d67::enabled.load();for(int c=0;c<17;++c)out.thrust_d67[c]=thrust_tuning_d67::read(c);
    out.blade_axis_cosine_h41=melee_tuning_c54::bladeAxisSettingH41(melee_tuning_c54::bladeAxisCosineH41.load());
    out.punch_minimum_m_d65=punch_travel_d65::value();
    out.punch_sensitivity_c54=melee_tuning_c54::punch();out.block_sensitivity_h29=melee_tuning_c54::blockH29();out.craft_trial_h41=subsystems::alchemy::reach_craft1::enabled.load();out.bypass_mod_vetoes_h41=melee_tuning_c54::bypassExtraAttackVetoesH41.load();out.chest_trial_h41=frik_pose_probe::ChestPitchShareCB();out.archer_trial_h41=frik_pose_probe::ArcherStanceCB();out.punch_travel_h41=melee_tuning_c54::punchTravel.load();out.guard_width_h41=melee_tuning_c54::guardHalfWidth.load();out.guard_height_h41=melee_tuning_c54::guardHalfHeight.load();out.guard_depth_d61=melee_tuning_c54::guardBoxDepthD61.load();out.guard_body_distance_d56=melee_tuning_c54::guardBodyDistanceD56.load();out.pb_window_h41=player_window_h26::perfectBlockMultiplier.load();out.ms_window_h41=player_window_h26::multiplier.load();
    for(unsigned i=0;i<shield_block_r120::Count;++i)out.shield_box_r120[i]=shield_block_r120::get(i); // R120 shield block box sliders
    for(int h=0;h<2;++h)for(int f=0;f<5;++f)out.finger_middle_lateral_c54[h][f]=finger40::getMiddleLateralC54(h,f);
    for(int h=0;h<2;++h){out.finger_middle_shift_mm_h31[h]=finger40::getShiftMmH31(h);out.finger_middle_distal_medial_h31[h]=finger40::getDistalMedialDegH31(h);} // H31_FINGER_FILL

    out.roomscale_mode = (roomscale_d76::Enabled()||g_roomscale_follow58.load(std::memory_order_acquire))?(g_roomscale_dodge58.load(std::memory_order_acquire)?2:1):
        (g_roomscale_dodge58.load(std::memory_order_acquire)?3:0); // R59; R59.16C5: 3 = HMD dodge only (no walking, no 6DOF BODY needed)
    out.gesture_talk = g_gesture_talk59c5.load(std::memory_order_acquire); // R59.16C5
    out.smooth_move51 = g_smooth_move51.load(std::memory_order_acquire);
    out.hmd_walking_direction134 = g_hmd_walking_direction134.load(std::memory_order_acquire);
    out.native_crouch_r93 = frik_pose_probe::NativeCrouchR93();
    out.body_height_mode98 = frik_pose_probe::BodyHeightModeR98(); // R98
    out.chest_dagger54 = frik_pose_probe::ChestDagger54();
#define KCD2VR_M2_HOST_SLOT 29 // M2_SOURCE_SELECTOR
#include "subsystems/throw_drop/host.inl" // M2_OWNED_SOURCE slot=29
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.comfort_vignette51 = g_comfort_vignette51.load(std::memory_order_acquire);
    out.reading_pages45 = kcd2vr::book_pages45::clampMode(kcd2vr::book_pages45::mode.load(std::memory_order_acquire)); // C45
    out.quick_slots_on_selection_r115 = kcd2vr::ui_hud_r112::quickSlotFade.load(std::memory_order_acquire); // R115
    out.quick_access_face_touch_r112 = kcd2vr::quick_access_r112::FaceTouch();
    out.quick_access_available_hands_r112 = kcd2vr::quick_access_r112::AvailableHands();
#define KCD2VR_M2_HOST_SLOT 30 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=30
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 31 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=31
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.physical_f9_rung61 = g_physical_held_f9_t4.load(std::memory_order_acquire);
    out.physical_f9_next_h6 = NextPhysicalHeldF9H6();
#define KCD2VR_M2_HOST_SLOT 32 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=32
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.phys_bodies = g_phys_bodies59c10r8.load(std::memory_order_acquire); // R59.16C10R8
#define KCD2VR_M2_HOST_SLOT 33 // M2_SOURCE_SELECTOR
#include "subsystems/vrmap/host.inl" // M2_OWNED_SOURCE slot=33
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 34 // M2_SOURCE_SELECTOR
#include "subsystems/experimental/host.inl" // M2_OWNED_SOURCE slot=34
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    {const auto& e=g_exp59c10;out.exp_wall_slide=e.wallSlide.load();out.exp_telegraph_windup=e.telegraphWindup.load();out.exp_telegraph_strike=e.telegraphStrike.load();out.exp_windup_hold=e.windupHold.load();out.exp_guard_counter_motion=e.guardCounter.load();out.exp_guard_auto_windup=e.guardAutoWindup.load();
     out.exp_guard_enter_speed=e.guardEnterSpeed.load();out.exp_guard_enter_dwell_ms=e.guardEnterDwellMs.load();out.exp_guard_exit_speed=e.guardExitSpeed.load();out.exp_guard_after_attack_ms=e.guardAfterAttackMs.load();
     out.exp_strike_reference_j=e.strikeRefJ.load();out.exp_strike_max_power=e.strikeMaxPower.load();out.exp_strike_two_handed=e.strikeTwoHanded.load();out.exp_head_raise_cm=e.headRaiseCm.load();
     out.npc_block39=g_npc_block39.load();out.npc_touch43=g_touch43.load();out.npc_fallplay43=g_fallplay43.load();out.exp_npc_proximity_radius36=frik_pose_probe::NpcProximityRadius36();out.exp_npc_eta_prepare=e.npcEtaPrepare.load();out.exp_npc_eta_strike=e.npcEtaStrike.load();out.exp_attack_require_opponent=e.attackRequireOpponent.load();out.exp_attack_reach_m=e.attackReachM.load();
     out.exp_direct_block_timing39=e.directBlock39.load();out.exp_direct_block_lo39=e.directBlockLo39.load();out.exp_direct_block_hi39=e.directBlockHi39.load(); // C39
     out.exp_npc_eta_prepare=e.npcEtaPrepare.load();out.exp_npc_eta_strike=e.npcEtaStrike.load();out.exp_attack_require_opponent=e.attackRequireOpponent.load();out.exp_attack_reach_m=e.attackReachM.load();
     out.exp_action_owner=e.actionOwner.load();out.exp_action_hold_at=e.actionHoldAt.load();
     out.exp_sync_overdue_direct=e.syncOverdueAny.load();out.exp_strike_in_recovery=e.recoveryOverride.load();out.exp_hit_without_row=e.noRowFallback.load();
     out.exp_reclaim_guard=e.reclaimGuard.load();out.exp_stagger_rule44=e.staggerRule44.load(); // R59.16C10R16 / C44
     out.exp_guard_strict=e.guardStrict.load();out.show_block_indicator_h24=e.showBlockIndicatorH24.load();out.exp_no_block_cooldown=e.noBlockCooldown.load();out.exp_ms_keep_stance=e.msKeepStance.load();out.exp_intent_live=e.intentLive.load(); // R59.16C10R17
     out.exp_instant_free_swing38=e.instantFreeSwing38.load();out.exp_hide_swing_lean38=e.hideSwingLean38.load(); // C38
     out.exp_direct_stagger_guard39=e.staggerGuard39.load(); // C39
     {namespace cs=kcd2vr::counter_stab40;out.exp_counter_stab40=cs::enabled.load();out.exp_counter_stab_energy40=cs::minEnergy.load();out.exp_counter_stab_mul40=cs::strengthMul.load();out.exp_counter_stab_window40=int(cs::windowMs.load());out.exp_counter_stab_perk44=cs::needsPerk.load();} // C40 / C44
     {namespace mg=kcd2vr::master_gate47;out.exp_ms_stab_only47=mg::clampMode(mg::mode.load());out.exp_ms_stab_energy47=mg::clampEnergy(mg::minEnergy.load());out.exp_ms_test_force=mg::testForce.load();} // C47 / DS-F5
     out.impale_hang_mass44=kcd2vr::gore32::hangMassKg.load();out.impale_kill_energy44=kcd2vr::contact58c1::impaleKillEnergy44.load(); // C44
     out.exp_direct_dirblock39=e.dirBlock39.load(); // C39 DIRBLOCK
     out.exp_native_hit_time=e.nativeHitTime.load();out.exp_weapon_stat_effect=e.weaponStatEffect.load();out.exp_combo_window_extend=combat_diag_probe::ComboWindowExtension();out.exp_combo_zone_assist=kcd2vr::combo_zone_assist_cz::enabled.load();out.exp_combo_recovery_hold=kcd2vr::combo_hold_r14::enabled.load();out.exp_combo_window_ms=e.comboWindowMs.load();out.exp_intent_live_ms=e.intentLiveMs.load();out.exp_combo_test_force=combat_diag_probe::ComboTestForce(); // DS-F6
     out.exp_own_hits18=e.ownHits18.load();out.exp_npc_block_speed18=e.npcBlockSpeed18.load(); // C10R18
     out.npc_block_anim_speed109=npc_defense::blockSpeed.load();out.npc_parry_anim_speed109=npc_defense::parrySpeed.load();out.npc_dodge_anim_speed109=npc_defense::dodgeSpeed.load();out.npc_defense_raise109=npc_defense::raiseExperiment.load(); // R109 MISC 1
     {namespace rh=kcd2vr::routerH3;out.win_router_mode109=rh::mode.load();out.win_combo_s109=rh::comboSeconds.load();out.win_riposte_s109=rh::riposteSeconds.load();out.win_standard_s109=rh::standardSeconds.load();out.win_ai_mod109=rh::aiSlotModifier.load();out.win_pl_mod109=rh::playerSlotModifier.load();out.win_block_speed_mod109=rh::blockSpeedModifier.load();} // R109 MISC 3
     out.riposte_window_ms109=e.riposteWindowMs.load();out.strike_power_on109=e.strikePower.load();out.strike_min_power109=e.strikeMinPower.load();out.strike_repeat_ms109=e.strikeRepeatMs.load(); // R109 MISC 3
     out.exp_ms_handoff23=e.msHandoff23.load(); // C24b
     out.kick_style26=e.kickStyle26.load();out.kick_leg26=e.kickLeg26.load();out.kick_reach_ms26=e.kickReachMs26.load();out.kick_frees_impale26=e.kickFrees26.load();out.kick_special33=kcd2vr::kick26::nativeSpecial33.load();out.kick_fallback33=kcd2vr::kick26::fallbackLeg.load();out.kick_real_hit37c2=kcd2vr::kick26::realHitTest37c2.load();out.kick_profile52=kcd2vr::kick26::profile52.load();out.kick_reach52=kcd2vr::kick26::reachPercent52.load();out.stat_sim26=e.statSim26.load(); // C26 / C26c
     out.vr_shadow_quality49=g_vr_shadow_quality49.load();out.eye_head26=g_eye_head26.load(); // C26d / C49
     out.exp_weight_feel=e.weightFeel.load();out.exp_fatigue_ramp=e.fatigueRamp.load();out.exp_pole_one_hand=e.poleOneHand.load();out.exp_swing_stamina_scale=e.swingStaminaScale.load();
     out.exp_strike_min_scale=e.strikeMinScale.load();out.exp_strike_min_interval_ms=e.strikeMinIntervalMs.load();out.exp_strike_min_gate=e.strikeMinGate.load();out.exp_master_strike_game=e.masterStrikeGame.load();out.exp_hide_stowed_bow=e.hideStowedBow.load();out.exp_two_hand_axis=e.twoHandAxis.load();} // R59.16C10d / R5 / R11 / R12 / R14 / R16
    out.auto_palm=g_auto_palm59c8.load(std::memory_order_acquire);out.riposte_native=g_riposte_native59c8.load(std::memory_order_acquire);out.target_dwell_s=g_target_dwell59c8.load(std::memory_order_acquire); // R59.16C8
#define KCD2VR_M2_HOST_SLOT 35 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=35
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 36 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=36
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 37 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=37
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.matrix_running = matrix_running_for_ui_.load(std::memory_order_acquire);

    XrManager* xr = xr_manager_;
    if (xr) {
        out.xr_running = xr->IsRunning();
        xr->GetEyeSize(out.eye_width, out.eye_height);
        // V61.43 PERF: this used to lock/copy the render frame and run 8x tan +
        // atan every menu refresh. The XR coverage value is now calculated by
        // the existing 5 Hz auto-projection update and published atomically.
        out.required_source_vfov_deg = vr_required_source_vfov_deg_.load(std::memory_order_acquire);
        XrManager::RenderFrameState frame{};
        if (xr->CopyActiveRenderFrameState(frame)) out.raw_ipd_m = frame.raw_ipd;
        const auto tc = xr->GetTestCounters();
        out.xr_frames = tc.xr_frames;
        out.valid_snapshots = tc.valid_frame_snapshots;
    }
#define KCD2VR_M2_HOST_SLOT 38 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=38
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    out.present_captures = DeviceHook::PresentCaptureCount();
    out.recursive_requests = RecursiveViewProbe::RecursiveRequestCount();
    out.recursive_passes = RecursiveViewProbe::RecursivePassCount();
    out.recursive_pipeline = FrameSync::PipelineCount(1);
    out.recursive_rt = DeviceHook::RecursiveRenderTargetBarrierCount();
}

bool Mod::RecalculateCorrectedProjection()
{
    if(!xr_manager_) return false;
    // H26: Game and VR use the same selected vertical FOV.
    // Game FOV defines what KCD renders. VR FOV defines the virtual angular
    // image plane used by the compositor. The shader uses each eye's REAL
    // OpenXR ray, so the same physical direction samples the same mono pixel
    // in both eyes. No resolution constants and no common-eye frustum.
    const float requested_game=std::clamp(menu_game_fov_target_.load(std::memory_order_acquire),70.0f,130.0f);
    const float requested_vr=requested_game; // shared Game + VR FOV
    const bool game_fov_ok=ApplyNativeGameFov(requested_game);
    float actual_game=ReadCurrentGameFovValue();
    if(!(actual_game>1.0f)) actual_game=requested_game;
    actual_game=std::clamp(actual_game,70.0f,140.0f);

    vr_cyclopean_mode_.store(1,std::memory_order_release);
    vr_corrected_source_vfov_deg_.store(actual_game,std::memory_order_release);
    vr_corrected_view_vfov_deg_.store(requested_vr,std::memory_order_release);
    vr_corrected_zoom_.store(1.0f,std::memory_order_release);
    vr_zoom_.store(1.0f,std::memory_order_release);
    xr_manager_->SetCorrectedViewVerticalFov(requested_vr);
    xr_manager_->SetPresentationSettings(
        vr_render_scale_.load(std::memory_order_acquire),3,1.0f,
        0.0f,0.0f,0.0f,0.0f,actual_game);
    xr_manager_->SetMonoUvReprojectionMode(5);
    xr_manager_->SetMonoUvBinocularOffset(0.0f);
    xr_manager_->SetPresentationCenterX(std::clamp(vr_presentation_center_x_.load(std::memory_order_acquire),-0.20f,0.20f));
    xr_manager_->SetPresentationCenterY(std::clamp(vr_presentation_center_y_.load(std::memory_order_acquire),-0.20f,0.20f));

    UINT sw=0,sh=0; xr_manager_->GetLastCapturedSourceResolution(sw,sh);
    const float aspect=(sw&&sh)?float(sw)/float(sh):1.0f;
    const float srcTanY=std::tan(actual_game*0.5f*0.01745329251994329577f);
    const float targetTanY=std::tan(requested_vr*0.5f*0.01745329251994329577f);
    const float planeScale=targetTanY>1e-4f?srcTanY/targetTanY:1.0f;
    char m[640]{};
    std::snprintf(m,sizeof(m),
        "V66.31-CORRECT-FOV selectedGameFov=%.2f selectedVrFov=%.2f source=%ux%u aspect=%.5f tangentPlaneScale=%.5f mapper=PER_EYE_REAL_OPENXR_RAY selectedPairOnly=1 gameFovWrite=%d",
        double(actual_game),double(requested_vr),unsigned(sw),unsigned(sh),double(aspect),double(planeScale),game_fov_ok?1:0);
    diagnostic_trace::Write(m); debug_trace::Write(m);
    vr_corrected_hud_dirty_.store(true,std::memory_order_release);
    return true;
}

bool Mod::CenterCorrectedProjection()
{
    if(!xr_manager_) return false;
    XrManager::StereoViewSnapshot sv{};
    if(!xr_manager_->CopyLastGoodStereoViews(sv) || !sv.valid || sv.view_count<2) {
        diagnostic_trace::Write("V65.08-PROJECTION CENTER unavailable: no valid cached OpenXR stereo views; no change");
        return false;
    }
    const float top=std::max(sv.eye_fov[0].angleUp,sv.eye_fov[1].angleUp);
    const float bottom=std::min(sv.eye_fov[0].angleDown,sv.eye_fov[1].angleDown);
    const float left=std::min(sv.eye_fov[0].angleLeft,sv.eye_fov[1].angleLeft);
    const float right=std::max(sv.eye_fov[0].angleRight,sv.eye_fov[1].angleRight);
    const float center_rad=0.5f*(top+bottom);
    const float residual_deg=std::clamp(-center_rad*57.29577951308232f,-15.0f,15.0f);
    const float tanL=std::tan(left),tanR=std::tan(right),tanD=std::tan(bottom),tanU=std::tan(top);
    const float planarX=std::clamp(-(tanL+tanR)*0.25f,-0.20f,0.20f);
    const float planarY=std::clamp( (tanD+tanU)*0.25f,-0.20f,0.20f);
    vr_pitch_correction_deg_.store(residual_deg,std::memory_order_release);
    vr_presentation_center_x_.store(planarX,std::memory_order_release);
    vr_presentation_center_y_.store(planarY,std::memory_order_release);
    vr_auto_projection_center_.store(false,std::memory_order_release);
    xr_manager_->SetAutoProjectionCenter(false);
    xr_manager_->SetPitchCorrectionDeg(residual_deg);
    xr_manager_->SetPresentationCenterX(planarX);
    xr_manager_->SetPresentationCenterY(planarY);
    const float src=vr_corrected_source_vfov_deg_.load(std::memory_order_acquire);
    const float half_src=0.5f*src*0.017453292519943295f;
    const float top_after=top + residual_deg*0.017453292519943295f;
    const float bottom_after=bottom + residual_deg*0.017453292519943295f;
    const float margin_top=(half_src-top_after)*57.29577951308232f;
    const float margin_bottom=(bottom_after+half_src)*57.29577951308232f;
    char m[512]{}; std::snprintf(m,sizeof(m),
        "V66.85-PROJECTION CENTER sourceFov=%.2f xrUnion={L:%+.3f R:%+.3f D:%+.3f U:%+.3f} residualPitch=%+.3fdeg planar=(%+.4f,%+.4f) coverageMarginsDeg=(bottom:%+.3f top:%+.3f)",
        double(src),double(left*57.29577951308232f),double(right*57.29577951308232f),double(bottom*57.29577951308232f),double(top*57.29577951308232f),double(residual_deg),
        double(planarX),double(planarY),double(margin_bottom),double(margin_top));
    diagnostic_trace::Write(m); debug_trace::Write(m);
    return true;
}

bool Mod::ApplyMenuState(const VrMenuState& requestedState)
{
    auto state=requestedState;
    if(!developer_mode::Enabled())state.diagnostic_rendering=false;
    if (matrix_running_for_ui_.load(std::memory_order_acquire)) return false;

    const int presentation = std::clamp(state.presentation_mode, 0, 3);
    const int stereo = 0; // legacy AFR experiment remains retired
    const int production_stereo = (state.stereo_mode==3) ? 3 : 0;
    const bool native_production = production_stereo==3; // V66.24 authoritative stage4/XR mode2 owner
    // D82: this atomic is the requested/persisted mode. Accept a supported
    // Stereo request even before vendor twins exist so the existing save worker
    // enables early capture next launch. Actual stage4 entry remains separately
    // guarded by CanEnterStereo in Run; no renderer is started here.
    if(native_production&&vr_production_stereo_mode_.load(std::memory_order_acquire)!=3){
        void* con=SafeGetCryConsole();
        void* sr=con?SafeConsoleGetCVar(con,"r_SuperResolution_Mode"):nullptr;
        const int actual=sr?SafeCVarGetInt(sr,-1):-1;
        if(!stereo_upscaler_d81r2::canKeep(3,actual>=0,actual))return false;
    }
    release_combat_r92::fastBlock.store(state.fast_block_r92);
    const int old_cyclopean = std::clamp(vr_cyclopean_mode_.load(std::memory_order_acquire),0,3);
    const int cyclopean = (state.cyclopean_mode==1) ? 1 : 0; // V66.69: pseudostereo retired; mode1 survives only as internal projective remap
    const float head_scale = std::clamp(state.head_pose_scale, 0.80f, 1.20f);
    const float camera_side = std::clamp(state.camera_side_offset_m, -0.30f, 0.30f);
    const float camera_back = std::clamp(state.camera_back_offset_m, -0.30f, 0.30f);
    const float camera_height = std::clamp(state.camera_height_offset_m, -0.30f, 0.30f);
    const float zoom = cyclopean==1 ? kV6508CorrectedZoom : std::clamp(state.presentation_zoom, 0.85f, 1.20f);
    const float previous_game_fov=menu_game_fov_target_.load(std::memory_order_acquire);
    const float previous_zoom_d86=vr_zoom_.load(std::memory_order_acquire);
    const float game_fov_target = kcd2vr::linked_fov_h26::Value(state.game_fov_target);
    const float horizontal_fov_target = std::clamp(state.horizontal_fov_target,70.0f,170.0f);
    const float fps_cap_target = std::clamp(state.fps_cap_target,60.0f,120.0f);
    const float merged_mesh_target = std::clamp(state.merged_mesh_target,30.0f,300.0f);
    const float auto_fov_trim = std::clamp(state.auto_fov_trim_deg,-10.0f,10.0f);
    const float auto_center_x_trim = std::clamp(state.auto_center_x_trim,-0.12f,0.12f);
    const float auto_center_trim = std::clamp(state.auto_center_y_trim,-0.12f,0.12f);
    const float pitch_trim_deg = std::clamp(state.projection_pitch_trim_deg,-15.0f,15.0f);
    const float source_fov = game_fov_target; // H26 one shared Game + VR setting
    const float compositor_source_fov = cyclopean==1 ? game_fov_target : source_fov; // actual captured KCD geometry
    const float effective_source_fov = cyclopean==1 ? compositor_source_fov : EffectiveSourceFovForMode(cyclopean, source_fov);
    const float effective_zoom = EffectiveZoomForMode(cyclopean, zoom);
    const float center_x = std::clamp(state.presentation_center_x, -0.20f, 0.20f);
    const float center_y = std::clamp(state.presentation_center_y, -0.20f, 0.20f);
    const float cull_scale = std::clamp(state.cull_frustum_scale, 1.0f, 1.60f);
#define KCD2VR_M2_HOST_SLOT 39 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=39
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 40 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=40
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    const float flat_screen_scale = std::clamp(state.flat_screen_scale, 0.45f, 3.00f);
    const float passive_full_frame_scale = std::clamp(state.passive_full_frame_scale, 1.00f, 2.00f); // R107
    const float pause_overlay_size = std::clamp(state.pause_overlay_size, 0.45f, 1.15f);
    const float transient_zoom_retain = std::clamp(state.transient_zoom_retain, 0.0f, 1.0f);
#define KCD2VR_M2_HOST_SLOT 41 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=41
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    const float ipd_scale = 0.0f; // V61.39: retired AFR/DUAL path cannot be re-armed by stale menu state
    const float render_scale = std::clamp(state.render_scale, 0.50f, 2.0f);
    const float engine_clarity = std::clamp(state.engine_clarity, 0.0f, 0.75f);
    const float gamma = std::clamp(state.gamma,0.50f,2.00f);
    const float brightness = std::clamp(state.brightness,-0.50f,0.50f);
    const float contrast = std::clamp(state.contrast,0.50f,2.00f);
    const float depth_stereo_strength = std::clamp(state.depth_stereo_strength, 0.0f, 1.0f);
#define KCD2VR_M2_HOST_SLOT 42 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=42
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    const int rotation_order = state.rotation_order ? 1 : 0;
    const int pose_source_mode = state.fix_head_sync ? 2 : 0; // V62.4 checkbox owns only the proven A/B modes
#define KCD2VR_M2_HOST_SLOT 43 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=43
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 44 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=44
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 45 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=45
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 46 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=46
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 47 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=47
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    const float iconScale51c3=std::isfinite(state.auto_hud_icon_scale)?std::clamp(state.auto_hud_icon_scale,.50f,6.f):1.75f;
#define KCD2VR_M2_HOST_SLOT 48 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=48
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR

    // Enabled is persisted by the menu but remains a restart-only master gate.
    enabled_.store(state.enabled, std::memory_order_release);
    enable_vr_output_bridge_.store(state.bridge_enabled, std::memory_order_release);
    apply_head_rotation_.store(state.head_rotation, std::memory_order_release);
    apply_head_position_.store(state.head_position, std::memory_order_release);
    diagnostic_rendering_.store(state.diagnostic_rendering, std::memory_order_release);
    vr_swap_eyes_.store(state.swap_eyes, std::memory_order_release);
    vr_rotation_order_.store(rotation_order, std::memory_order_release);
    vr_pose_source_mode_.store(pose_source_mode, std::memory_order_release);
#define KCD2VR_M2_HOST_SLOT 49 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=49
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 50 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=50
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    const float old_source_fov=vr_source_vertical_fov_deg_.load(std::memory_order_acquire);
#define KCD2VR_M2_HOST_SLOT 51 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=51
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // V66.114R1: this compatibility field now selects SAFE-UNIFORM readable-HUD
    // fitting. It does NOT revive MasterSwitch/whole-root AutoHUD.
#define KCD2VR_M2_HOST_SLOT 52 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=52
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    const bool old_f2_fix=f2_ownership_fix_.exchange(true,std::memory_order_acq_rel);
#define KCD2VR_M2_HOST_SLOT 53 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=53
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 54 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=54
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 55 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=55
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 56 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=56
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 57 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=57
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 58 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=58
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    {   // R59.16C7F: live hand/weapon placement from the Immersion sliders (applied immediately, saved on release)
        const float wg=std::clamp(state.weapon_grip_cm,-15.f,15.f),ss=std::clamp(state.support_sword_cm,6.f,60.f),sp=std::clamp(state.support_pole_cm,6.f,60.f);
        if(weapon_grip_cm_.exchange(wg,std::memory_order_acq_rel)!=wg)frik_pose_probe::SetWristFromGrip59c7f(wg);
        if(support_sword_cm_.exchange(ss,std::memory_order_acq_rel)!=ss||support_pole_cm_.exchange(sp,std::memory_order_acq_rel)!=sp)
            frik_pose_probe::SetSupportSocket178r9(g_support_socket_synthetic59c7f,ss,sp);
        if(frik_pose_probe::SupportSlot34()!=state.support_slot34)frik_pose_probe::SetSupportSlot34(state.support_slot34); // C34b
    }
#define KCD2VR_M2_HOST_SLOT 59 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=59
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 60 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=60
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    head_pose_scale_.store(head_scale, std::memory_order_release);
    camera_side_offset_m_.store(camera_side, std::memory_order_release);
    camera_back_offset_m_.store(camera_back, std::memory_order_release);
    camera_height_offset_m_.store(camera_height, std::memory_order_release);
    menu_game_fov_target_.store(game_fov_target,std::memory_order_release);
    menu_horizontal_fov_target_.store(horizontal_fov_target,std::memory_order_release);
    menu_fps_cap_enabled_.store(state.fps_cap_enabled,std::memory_order_release);
    menu_fps_cap_target_.store(fps_cap_target,std::memory_order_release);
    menu_merged_mesh_target_.store(merged_mesh_target,std::memory_order_release);
    vr_auto_fov_trim_deg_.store(auto_fov_trim,std::memory_order_release);
    vr_auto_center_x_trim_.store(auto_center_x_trim,std::memory_order_release);
    vr_auto_center_y_trim_.store(auto_center_trim,std::memory_order_release);
    vr_auto_projection_center_.store(state.auto_projection_center,std::memory_order_release);
    vr_pitch_correction_deg_.store(pitch_trim_deg,std::memory_order_release);
    if (xr_manager_) { xr_manager_->SetAutoProjectionCenter(state.auto_projection_center); xr_manager_->SetPitchCorrectionDeg(pitch_trim_deg); }
    vr_presentation_mode_.store(presentation, std::memory_order_release);
    vr_zoom_.store(zoom, std::memory_order_release);
    vr_source_vertical_fov_deg_.store(source_fov, std::memory_order_release);
    vr_presentation_center_x_.store(center_x, std::memory_order_release);
    vr_presentation_center_y_.store(center_y, std::memory_order_release);
    vr_cull_frustum_scale_.store(cull_scale, std::memory_order_release);
    // V66.59: cull scale is owned by F3 now; a settings apply must not reset it.
#define KCD2VR_M2_HOST_SLOT 61 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=61
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 62 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=62
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    vr_flat_screen_scale_.store(flat_screen_scale, std::memory_order_release);
    vr_passive_full_frame_scale_.store(passive_full_frame_scale, std::memory_order_release);
    kcd2vr::ui_surface_geometry::hudDistanceR110.store(std::isfinite(state.hud_distance_r110)?std::clamp(state.hud_distance_r110,kcd2vr::ui_surface_geometry::kHudConvergenceMinR127,kcd2vr::ui_surface_geometry::kHudConvergenceMaxR127):3.0f,std::memory_order_release); // R110
    kcd2vr::ui_hud_r112::combatCrosshairScale.store(std::isfinite(state.combat_crosshair_scale_r112)?std::clamp(state.combat_crosshair_scale_r112,kcd2vr::ui_hud_r112::kCombatCrosshairMin,kcd2vr::ui_hud_r112::kCombatCrosshairMax):kcd2vr::ui_hud_r112::kCombatCrosshairDefault,std::memory_order_release); // R112
    vr_pause_overlay_size_.store(pause_overlay_size, std::memory_order_release);
    vr_transient_zoom_retain_.store(transient_zoom_retain, std::memory_order_release);
#define KCD2VR_M2_HOST_SLOT 63 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=63
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // H26: FOV changes are committed below through the existing native setter.
    vr_stereo_experiment_.store(stereo, std::memory_order_release);
    vr_production_stereo_mode_.store(production_stereo,std::memory_order_release);
    vr_cyclopean_mode_.store(cyclopean, std::memory_order_release);
    vr_afr_ipd_scale_.store(ipd_scale, std::memory_order_release);
    const bool old_balanced = enable_vr_balanced_graphics_.exchange(state.balanced_graphics, std::memory_order_acq_rel);
    const float old_clarity = vr_engine_clarity_.exchange(engine_clarity, std::memory_order_acq_rel);
    vr_render_scale_.store(render_scale, std::memory_order_release);
    vr_gamma_.store(gamma,std::memory_order_release);
    vr_brightness_.store(brightness,std::memory_order_release);
    vr_contrast_.store(contrast,std::memory_order_release);
#define KCD2VR_M2_HOST_SLOT 64 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=64
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    vr_depth_stereo_strength_.store(depth_stereo_strength, std::memory_order_release);
#define KCD2VR_M2_HOST_SLOT 65 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=65
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 66 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=66
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 67 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=67
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 68 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=68
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 69 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=69
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    const bool alchemyHandsChangedD39=activity_input163::trackedHands59c7f[0].exchange(state.activity_hands59c7f[0])!=state.activity_hands59c7f[0];
    const bool smithHandsChangedD39=activity_input163::trackedHands59c7f[1].exchange(state.activity_hands59c7f[1])!=state.activity_hands59c7f[1];
    if(alchemyHandsChangedD39||smithHandsChangedD39)minigame_hands45::modeEpoch67.fetch_add(1,std::memory_order_acq_rel);
    activity_input163::trackedHands59c7f[3]=state.activity_hands59c7f[2];
    activity_input163::trackedHands59c7f[2]=state.activity_hands59c7f[3];
#define KCD2VR_M2_HOST_SLOT 70 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=70
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    const bool nativeRoomscaleWasEnabledD84=roomscale_d76::Enabled();
#define KCD2VR_D40_LIVE_APPLY
#include "subsystems/experimental/live_runtime_d40.inl"
#undef KCD2VR_D40_LIVE_APPLY
    // D39: all three hand modes share one owner, including the legacy button.
    frik_pose_probe::SetHandsModeDs2(state.live_d39.handsMode);
    frik_pose_probe::SetHandSeparationDs1(state.live_d39.handSeparation);
    if(frik_pose_probe::NativeHandPhysicsH24()!=state.live_d39.nativeHandPhysics)
        frik_pose_probe::SetNativeHandPhysicsH24(state.live_d39.nativeHandPhysics);
    frik_pose_probe::SetShoulderReachD38(state.live_d39.shoulderWarningM);
    smooth_lab::SetBaselineSmoothingD39(state.live_d39.handSmoothing);
    workstation178r20c1::gesturesEverywhere59c2.store(state.live_d39.workstationGestures);
    misc_gestures_d59::SetPet(state.live_d39.petGestureD59);misc_gestures_d59::SetWash(state.live_d39.washGestureD59);
    misc_gestures_d59::alchemyTransfer.store(state.live_d39.alchemyTransferD59);
    subsystems::alchemy::reach_craft1::setPickupReachD39(state.live_d39.alchemyPickupReach);
    station_ws_j27::setToolCollision(state.live_d39.smithToolCollision);
    station_ws_j27::torsoEnabled.store(state.live_d39.stationTorso);
    station_ws_j27::clearCm.store(std::isfinite(state.live_d39.stationClearCm)?std::clamp(state.live_d39.stationClearCm,0.f,30.f):12.f);
    utility54::SetTorchGestureD39(state.live_d39.torchGesture);
    utility54::SetVisorGestureD39(state.live_d39.visorGesture);
    if(g_held_map59c10r10.exchange(state.live_d39.parchmentMap)!=state.live_d39.parchmentMap)
        g_held_map_apply_d39.store(true,std::memory_order_release);
    camera_hold_e1::maxHoldMs.store(std::uint32_t(std::clamp(state.live_d39.cameraHoldMs,0,5000)));
    bow_arm_aim_r9::enabled.store(state.live_d39.bowArmAim);lab58::propWake.store(state.live_d39.labPropWake);
    deepseek_h29::fingerContactSpringH31.store(state.live_d39.fingerTrial);
    alchemy_own_head_h51::enabled.store(state.live_d39.alchemyEyeTrial);
    grip_stow_cb1::SetEnabled(state.live_d39.gripStowTrial);grip_stow_cb1::SetButton(state.live_d39.gripStowButton);
#define KCD2VR_M2_HOST_SLOT 71 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=71
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 72 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=72
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    frik_pose_probe::SetCalibrationAutoRestore43(state.calibration_auto_restore43); // C43
    workstation178r20c1::setSmithAutoHold36(state.smith_auto_hold36); // C37: a change revokes current smithing holds
    subsystems::smithing::anvil_a2::setMode(state.smith_anvil_mode_a3); // A3
    const int nextMinigame46=minigame_hands45::clampMode(state.minigame_hands45);
    if(minigame_hands45::mode.exchange(nextMinigame46)!=nextMinigame46&&nextMinigame46!=minigame_hands45::Native)
        combat_rules178r10::workstationEnabled.store(true,std::memory_order_release);
    minigame_hands45::setMode(minigame_hands45::Alchemy,state.alchemy_mode67);
    minigame_hands45::setMode(minigame_hands45::Smithing,state.smithing_mode67);
    if(state.alchemy_mode67||state.smithing_mode67)combat_rules178r10::workstationEnabled.store(true,std::memory_order_release);
    workstation178r20c1::alchemyPassenger42.store(true); // Both policies retain Henry's authored reach/held-tool path.

#define KCD2VR_CONFIG_APPLY_DOORS0
#include "subsystems/doors/config.inl"
#undef KCD2VR_CONFIG_APPLY_DOORS0

    frik_pose_probe::SetGrabHandStays43(state.grab_hand_stays43);
    if(frik_pose_probe::GrabMode60()!=state.grab_mode60){frik_pose_probe::SetGrabMode60(state.grab_mode60);ApplyOwnedFragments60();} // C43
    native_activity_controls163_.store(state.native_activity_controls163);
#define KCD2VR_M2_HOST_SLOT 73 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=73
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    vr_turn_mode_.store(state.vr_turn_mode==0?0:1,std::memory_order_release);
#define KCD2VR_M2_HOST_SLOT 74 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=74
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 75 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=75
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    g_gesture_talk59c5.store(state.gesture_talk,std::memory_order_release); // R59.16C5
    if(g_smooth_move51.exchange(state.smooth_move51)!=state.smooth_move51)frik_pose_probe::SetSmoothBody51(state.smooth_move51);
    g_hmd_walking_direction134.store(state.hmd_walking_direction134,std::memory_order_release);
    if(frik_pose_probe::NativeCrouchR93()!=state.native_crouch_r93)frik_pose_probe::SetNativeCrouchR93(state.native_crouch_r93);
    if(frik_pose_probe::BodyHeightModeR98()!=state.body_height_mode98)frik_pose_probe::SetBodyHeightModeR98(state.body_height_mode98); // R98
    if(frik_pose_probe::ChestDagger54()!=state.chest_dagger54)frik_pose_probe::SetChestDagger54(state.chest_dagger54);
#define KCD2VR_M2_HOST_SLOT 76 // M2_SOURCE_SELECTOR
#include "subsystems/throw_drop/host.inl" // M2_OWNED_SOURCE slot=76
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    g_comfort_vignette51.store(std::clamp(state.comfort_vignette51,0,3),std::memory_order_release);
    if(xr_manager_)xr_manager_->SetComfortVignette(g_comfort_vignette51.load(std::memory_order_acquire));
    kcd2vr::book_pages45::mode.store(kcd2vr::book_pages45::clampMode(state.reading_pages45),std::memory_order_release); // C45
    kcd2vr::quick_access_r112::SetFaceTouch(state.quick_access_face_touch_r112);
    kcd2vr::ui_hud_r112::quickSlotFade.store(state.quick_slots_on_selection_r115,std::memory_order_release); // R115 (saved as VrHudQuickSlotFadeR112)
    if(g_ragdoll_push33.exchange(std::clamp(state.ragdoll_push,1,3))!=std::clamp(state.ragdoll_push,1,3)&&g_ragdoll_rung19c4r4.load()==2) // C33: live strength change
        combat_diag_probe::SetContactImpulse596(g_physical_held_f9_t4.load()==3&&g_ragdoll59.On()&&!g_legacy_phys_suspended_t4.load()&&g_contact_impulse596.load(),g_contact_impulse_scale596.load()*RagdollPushScale33(),RagdollPushCap33(),4.f);
    if(g_physical_held_f9_t4.load()!=std::clamp(state.physical_f9_rung61,0,3))ApplyPhysicalHeldF9T4(state.physical_f9_rung61);
#define KCD2VR_M2_HOST_SLOT 77 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=77
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 78 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=78
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 79 // M2_SOURCE_SELECTOR
#include "subsystems/vrmap/host.inl" // M2_OWNED_SOURCE slot=79
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 80 // M2_SOURCE_SELECTOR
#include "subsystems/experimental/host.inl" // M2_OWNED_SOURCE slot=80
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 81 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=81
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    if(g_phys_bodies59c10r8.exchange(std::clamp(state.phys_bodies,0,2))!=std::clamp(state.phys_bodies,0,2))
        ApplyLegacyPhysBodiesT4(); // Preserve saved choice while F9 owns native physics.
#define KCD2VR_M2_HOST_SLOT 82 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=82
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    {   // R59.16C10d EXPERIMENTS: apply only what changed, through the INI setters
        auto& e=g_exp59c10;
        if(e.wallSlide.exchange(state.exp_wall_slide)!=state.exp_wall_slide)frik_pose_probe::SetWallSlide59c10(state.exp_wall_slide);
        if(e.telegraphWindup.exchange(state.exp_telegraph_windup)!=state.exp_telegraph_windup)frik_pose_probe::SetTelegraphWindup59c10(state.exp_telegraph_windup);
        if(e.telegraphStrike.exchange(state.exp_telegraph_strike)!=state.exp_telegraph_strike)combat_diag_probe::SetTelegraphStrike59c10(state.exp_telegraph_strike);
        if(e.windupHold.exchange(state.exp_windup_hold)!=state.exp_windup_hold)frik_pose_probe::SetWindupHold57(state.exp_windup_hold);
        if(e.guardCounter.exchange(state.exp_guard_counter_motion)!=state.exp_guard_counter_motion)frik_pose_probe::SetGuardCounterMotion57C1(state.exp_guard_counter_motion);
        if(e.guardAutoWindup.exchange(state.exp_guard_auto_windup)!=state.exp_guard_auto_windup)frik_pose_probe::SetGuardAutoWindup19c4r1(state.exp_guard_auto_windup);
        const float ges=std::clamp(state.exp_guard_enter_speed,0.f,5.f);const int ged=std::clamp(state.exp_guard_enter_dwell_ms,0,600);
        const bool speedChanged=std::fabs(e.guardEnterSpeed.exchange(ges)-ges)>1e-4f;
        const bool dwellChanged=e.guardEnterDwellMs.exchange(ged)!=ged;
        if(speedChanged||dwellChanged)frik_pose_probe::SetGuardEntry178r29(ges,unsigned(ged));
        const float gxs=std::clamp(state.exp_guard_exit_speed,0.f,6.f);if(std::fabs(e.guardExitSpeed.exchange(gxs)-gxs)>1e-4f)frik_pose_probe::SetGuardExitSpeed178r45(gxs);
        const int gaa=std::clamp(state.exp_guard_after_attack_ms,0,2000);if(e.guardAfterAttackMs.exchange(gaa)!=gaa)frik_pose_probe::SetGuardAfterAttackMs49(unsigned(gaa));
        const float srj=std::clamp(state.exp_strike_reference_j,10.f,200.f);if(std::fabs(e.strikeRefJ.exchange(srj)-srj)>1e-4f)frik_pose_probe::SetStrikeReferenceJ59c9(srj);
        const float smp=std::clamp(state.exp_strike_max_power,.5f,2.f),sth=std::clamp(state.exp_strike_two_handed,1.f,2.f);
        const bool powerChanged=std::fabs(e.strikeMaxPower.exchange(smp)-smp)>1e-4f;
        const bool supportChanged=std::fabs(e.strikeTwoHanded.exchange(sth)-sth)>1e-4f;
        const float smn=std::clamp(state.strike_min_power109,.05f,1.f);const int srm=std::clamp(state.strike_repeat_ms109,200,10000),rwm=std::clamp(state.riposte_window_ms109,100,3000); // R109 MISC 3
        const bool miscPowerChanged=(e.strikePower.exchange(state.strike_power_on109)!=state.strike_power_on109)|(std::fabs(e.strikeMinPower.exchange(smn)-smn)>1e-4f)|(e.strikeRepeatMs.exchange(srm)!=srm)|(e.riposteWindowMs.exchange(rwm)!=rwm);
        if(powerChanged||supportChanged||miscPowerChanged)
            combat_diag_probe::SetStrikePower59c8(e.strikePower.load(),sth,e.strikeMinPower.load(),smp,unsigned(e.strikeRepeatMs.load()),g_riposte_native59c8.load(),unsigned(e.riposteWindowMs.load()));
        const float hr=std::clamp(state.exp_head_raise_cm,0.f,60.f);if(std::fabs(e.headRaiseCm.exchange(hr)-hr)>1e-4f)frik_pose_probe::SetHeadRaiseMax59c9(hr);
        for(int h=0;h<2;++h)for(int f=0;f<5;++f)for(int k=0;k<4;++k)finger40::set(h,f,k,state.fingers40[h][f][k]);
        hand_present_d61::SetEnabled(state.free_hand_present_d61);
        finisher_cb1::nativeValidationTestD61.store(state.native_finisher_test_d61,std::memory_order_release);
        // D67 live contribution applies the single canonical motion-menu choice.
        for(unsigned c=0;c<melee_tuning_c54::classCount;++c)melee_tuning_c54::set(int(c),state.melee_types_c54[c]);
        thrust_tuning_d67::enabled.store(state.independent_thrust_d67);for(int c=0;c<17;++c)thrust_tuning_d67::set(c,state.thrust_d67[c]);
        melee_tuning_c54::bladeAxisCosineH41.store(melee_tuning_c54::bladeAxisSettingH41(state.blade_axis_cosine_h41));
        punch_travel_d65::minimumMeters.store(punch_travel_d65::setting(state.punch_minimum_m_d65));
        melee_tuning_c54::punchSensitivity.store(melee_tuning_c54::bound(state.punch_sensitivity_c54,.5f,2.f,1.f));
        melee_tuning_c54::blockSensitivityH29.store(melee_tuning_c54::bound(state.block_sensitivity_h29,.5f,1.5f,1.f));subsystems::alchemy::reach_craft1::enabled.store(state.craft_trial_h41);melee_tuning_c54::bypassExtraAttackVetoesH41.store(state.bypass_mod_vetoes_h41);frik_pose_probe::SetChestPitchShareCB(state.chest_trial_h41);frik_pose_probe::SetArcherStanceCB(state.archer_trial_h41);melee_tuning_c54::punchTravel.store(melee_tuning_c54::bound(state.punch_travel_h41,.4f,2.f,1.f));melee_tuning_c54::guardHalfWidth.store(melee_tuning_c54::bound(state.guard_width_h41,.25f,.75f,.5f));melee_tuning_c54::guardHalfHeight.store(melee_tuning_c54::bound(state.guard_height_h41,.1f,.35f,.2f));melee_tuning_c54::guardBoxDepthD61.store(melee_tuning_c54::bound(state.guard_depth_d61,.10f,.80f,.50f));melee_tuning_c54::guardBodyDistanceD56.store(melee_tuning_c54::bound(state.guard_body_distance_d56,.35f,1.05f,.65f));player_window_h26::perfectBlockMultiplier.store(melee_tuning_c54::bound(state.pb_window_h41,1.f,2.f,2.f));player_window_h26::multiplier.store(melee_tuning_c54::bound(state.ms_window_h41,1.f,2.f,1.5f));
        for(unsigned i=0;i<shield_block_r120::Count;++i)shield_block_r120::set(i,state.shield_box_r120[i]); // R120 shield block box sliders
        for(int h=0;h<2;++h)for(int f=0;f<5;++f)finger40::setMiddleLateralC54(h,f,state.finger_middle_lateral_c54[h][f]);
        for(int h=0;h<2;++h){finger40::setShiftMmH31(h,state.finger_middle_shift_mm_h31[h]);finger40::setDistalMedialDegH31(h,state.finger_middle_distal_medial_h31[h]);} // H31_FINGER_APPLY
        frik_pose_probe::SetNpcProximityRadius36(state.exp_npc_proximity_radius36); // C38P: same owner/clamp and existing INI persistence
        g_npc_block39.store(std::clamp(state.npc_block39,0,3)); // C39
        if(g_touch43.exchange(state.npc_touch43)!=state.npc_touch43)frik_pose_probe::SetTouchReact43(state.npc_touch43,g_touch_mass43.load()); // C43T
        g_fallplay43.store(state.npc_fallplay43);
        {   // R59.16C10R5: NPC block ETA (S33) and the opponent-in-reach gate
            const float ep=std::clamp(state.exp_npc_eta_prepare,.10f,.6f),es=std::clamp(state.exp_npc_eta_strike,.10f,.6f); // R59.16C10R16: >= 0.10 s
            const bool etaChanged=std::fabs(e.npcEtaPrepare.exchange(ep)-ep)>1e-4f|std::fabs(e.npcEtaStrike.exchange(es)-es)>1e-4f;
            if(etaChanged)combat_diag_probe::SetNpcBlockEta59c10(ep,es);
            const float blo=std::clamp(state.exp_direct_block_lo39,1.f,3.f),bhi=std::clamp(state.exp_direct_block_hi39,1.f,3.f); // C39
            const bool blockChanged=(e.directBlock39.exchange(state.exp_direct_block_timing39)!=state.exp_direct_block_timing39)|(std::fabs(e.directBlockLo39.exchange(blo)-blo)>1e-4f)|(std::fabs(e.directBlockHi39.exchange(bhi)-bhi)>1e-4f);
            if(blockChanged)combat_diag_probe::SetDirectBlockTiming39(state.exp_direct_block_timing39,blo,bhi);
            const float reach=std::clamp(state.exp_attack_reach_m,1.f,5.f);
            const bool gateChanged=(e.attackRequireOpponent.exchange(state.exp_attack_require_opponent)!=state.exp_attack_require_opponent)|(std::fabs(e.attackReachM.exchange(reach)-reach)>1e-4f);
            if(gateChanged)frik_pose_probe::SetAttackOpponentGate59c10(state.exp_attack_require_opponent,reach);
        }
        {   // R59.16C10R11: action time owner mode + hold point (ACTIONS page); the fragment list and the rest stay INI-only
            const int mode=std::clamp(state.exp_action_owner,0,2);const float hold=std::clamp(state.exp_action_hold_at,.05f,.90f);
            if(e.actionOwner.exchange(mode)!=mode)action_time_owner59c11::SetMode(mode);
            if(std::fabs(e.actionHoldAt.exchange(hold)-hold)>1e-4f){auto t=action_time_owner59c11::GetTuning();t.holdAt=hold;action_time_owner59c11::SetTuning(t);}
        }
        {   // R59.16C10R12: the three 19:43-log switches (COMBAT TUNING 2 page)
            const bool c1=e.syncOverdueAny.exchange(state.exp_sync_overdue_direct)!=state.exp_sync_overdue_direct,c2=e.recoveryOverride.exchange(state.exp_strike_in_recovery)!=state.exp_strike_in_recovery,c3=e.noRowFallback.exchange(state.exp_hit_without_row)!=state.exp_hit_without_row;
            if(c1||c2||c3)combat_diag_probe::SetRung3Switches59c12(state.exp_sync_overdue_direct,state.exp_strike_in_recovery,state.exp_hit_without_row);
            const int sr44=kcd2vr::direct_counter39::clampStaggerRule44(state.exp_stagger_rule44);const bool sds44=kcd2vr::direct_counter39::staggerStrikeAllowed44(sr44); // C44
            e.staggerRule44.store(sr44);kcd2vr::direct_counter39::staggerRule44.store(sr44,std::memory_order_relaxed);
            const bool c4=e.reclaimGuard.exchange(state.exp_reclaim_guard)!=state.exp_reclaim_guard,c5=e.strikeDuringStagger.exchange(sds44)!=sds44;
            if(c4||c5)combat_diag_probe::SetRung4Switches59c16(state.exp_reclaim_guard,sds44); // R59.16C10R16
        }
        {   // R59.16C10R14 WEAPON FEEL page
            const float wf=std::clamp(state.exp_weight_feel,.2f,2.f);if(std::fabs(e.weightFeel.exchange(wf)-wf)>1e-4f)frik_pose_probe::SetWeaponAuthorityScale178r44(wf);
            const float fr=std::clamp(state.exp_fatigue_ramp,0.f,1.f),po=std::clamp(state.exp_pole_one_hand,.15f,1.f);
            const bool feelChanged=(std::fabs(e.fatigueRamp.exchange(fr)-fr)>1e-4f)|(std::fabs(e.poleOneHand.exchange(po)-po)>1e-4f)|(e.strikeMinGate.exchange(state.exp_strike_min_gate)!=state.exp_strike_min_gate);
            if(feelChanged)frik_pose_probe::SetWeaponFeel59c14(fr,po,state.exp_strike_min_gate,true);
            const float ss=std::clamp(state.exp_swing_stamina_scale,0.f,2.f);if(std::fabs(e.swingStaminaScale.exchange(ss)-ss)>1e-4f)kcd2vr::swing_effort59c14::scale.store(ss);
            const float ms=std::clamp(state.exp_strike_min_scale,0.f,1.f);const int mi=std::clamp(state.exp_strike_min_interval_ms,0,3000);
            if((std::fabs(e.strikeMinScale.exchange(ms)-ms)>1e-4f)|(e.strikeMinIntervalMs.exchange(mi)!=mi))frik_pose_probe::SetVrCombatCore178(ms,unsigned(mi),e.strikeWaitNative.load());
            if(e.masterStrikeGame.exchange(state.exp_master_strike_game)!=state.exp_master_strike_game)frik_pose_probe::SetMasterStrikeVeto59c14(!state.exp_master_strike_game);
            const int hb=std::clamp(state.exp_hide_stowed_bow,0,2);if(e.hideStowedBow.exchange(hb)!=hb)frik_pose_probe::SetHideStowedBow59c14(hb,e.hideBowPitchDeg.load());
            if(e.twoHandAxis.exchange(state.exp_two_hand_axis)!=state.exp_two_hand_axis)frik_pose_probe::SetTwoHandAxis59c16(state.exp_two_hand_axis); // R59.16C10R16
        }
        {   // R59.16C10R17 COMBAT FLOW page
            if(e.guardStrict.exchange(state.exp_guard_strict)!=state.exp_guard_strict)frik_pose_probe::SetGuardStrict59c17(state.exp_guard_strict);
            e.showBlockIndicatorH24.store(state.show_block_indicator_h24);e.noBlockCooldown.store(state.exp_no_block_cooldown); // the lease below follows it on the next frame
            e.instantFreeSwing38.store(state.exp_instant_free_swing38); // C38: the lease follows on the next frame
            e.hideSwingLean38.store(state.exp_hide_swing_lean38);kcd2vr::vr_combat111::hideSwingLean38.store(state.exp_hide_swing_lean38,std::memory_order_relaxed); // C38
            e.staggerGuard39.store(state.exp_direct_stagger_guard39);kcd2vr::direct_counter39::staggerGuard.store(state.exp_direct_stagger_guard39,std::memory_order_relaxed); // C39
            {namespace cs=kcd2vr::counter_stab40;cs::enabled.store(state.exp_counter_stab40);cs::minEnergy.store(cs::clampEnergy(state.exp_counter_stab_energy40));cs::strengthMul.store(cs::clampMul(state.exp_counter_stab_mul40));cs::windowMs.store(cs::clampWindow(state.exp_counter_stab_window40));cs::needsPerk.store(state.exp_counter_stab_perk44);} // C40 / C44
            {namespace mg=kcd2vr::master_gate47;mg::mode.store(mg::clampMode(state.exp_ms_stab_only47));mg::minEnergy.store(mg::clampEnergy(state.exp_ms_stab_energy47));mg::testForce.store(state.exp_ms_test_force);} // C47 / DS-F5
            kcd2vr::gore32::hangMassKg.store(float(kcd2vr::gore32::clampHangMass44(state.impale_hang_mass44)));kcd2vr::contact58c1::impaleKillEnergy44.store(std::clamp(state.impale_kill_energy44,0.f,1.f)); // C44
            e.dirBlock39.store(state.exp_direct_dirblock39);kcd2vr::dirblock39::enabled.store(state.exp_direct_dirblock39,std::memory_order_relaxed); // C39 DIRBLOCK
            const float nh=state.exp_native_hit_time<.10f?0.f:std::min(state.exp_native_hit_time,.60f),ws=std::clamp(state.exp_weapon_stat_effect,0.f,1.5f);
            const bool flowChanged=(std::fabs(e.nativeHitTime.exchange(nh)-nh)>1e-4f)|(std::fabs(e.weaponStatEffect.exchange(ws)-ws)>1e-4f)|(e.msKeepStance.exchange(state.exp_ms_keep_stance)!=state.exp_ms_keep_stance);
            if(flowChanged)frik_pose_probe::SetCombatFlow59c17(nh,state.exp_ms_keep_stance,ws);
            combat_diag_probe::SetComboWindowExtension(state.exp_combo_window_extend);
            kcd2vr::combo_zone_assist_cz::enabled.store(state.exp_combo_zone_assist,std::memory_order_relaxed); // I12
            kcd2vr::combo_hold_r14::enabled.store(state.exp_combo_recovery_hold,std::memory_order_relaxed); // I12/N2
            kcd2vr::combo_carrier_cb::enabled.store(state.exp_combo_recovery_hold,std::memory_order_relaxed); // D30-CMB carrier follows HOLD
            const int cw=std::clamp(state.exp_combo_window_ms,0,3000),lm=std::clamp(state.exp_intent_live_ms,0,120);
            const bool routeChanged=(e.intentLive.exchange(state.exp_intent_live)!=state.exp_intent_live)|(e.comboWindowMs.exchange(cw)!=cw)|(e.intentLiveMs.exchange(lm)!=lm);
            if(routeChanged)combat_diag_probe::SetCombatFlow59c17(state.exp_intent_live,unsigned(lm),unsigned(cw));
            combat_diag_probe::SetComboTestForce(state.exp_combo_test_force); // DS-F6 (no-op unless changed)
            const float bs=state.exp_npc_block_speed18<=0.f?0.f:std::clamp(state.exp_npc_block_speed18,1.f,3.f); // C10R18 (0 = legacy ETA values, INI only)
            const bool directChanged=(e.ownHits18.exchange(state.exp_own_hits18)!=state.exp_own_hits18)|(std::fabs(e.npcBlockSpeed18.exchange(bs)-bs)>1e-4f);
            if(directChanged)combat_diag_probe::SetVrDirect18(state.exp_own_hits18,bs);
            {namespace rh=kcd2vr::routerH3; // R109 MISC 3: the F12 router mode and the six wh_cs_* window values (Windows::service writes them only in mode 2)
             rh::mode.store(std::clamp(state.win_router_mode109,0,2),std::memory_order_release);
             rh::comboSeconds.store(rh::setting(state.win_combo_s109,.5f,3.f,3.f));rh::riposteSeconds.store(rh::setting(state.win_riposte_s109,.1f,1.5f,1.f));rh::standardSeconds.store(rh::setting(state.win_standard_s109,.1f,2.f,1.5f));
             rh::aiSlotModifier.store(rh::setting(state.win_ai_mod109,0.f,1.f,.2f));rh::playerSlotModifier.store(rh::setting(state.win_pl_mod109,0.f,1.f,.2f));rh::blockSpeedModifier.store(rh::setting(state.win_block_speed_mod109,.8f,2.4f,1.4f));}
            {const float mh=state.exp_ms_handoff23<0.f?-1.f:std::clamp(state.exp_ms_handoff23,1.5f,4.5f); // C24b menu slider
             if(std::fabs(e.msHandoff23.exchange(mh)-mh)>1e-4f)frik_pose_probe::SetMasterHandoff23(e.msHandoffOn23.load(),mh);}
            {const int ks=std::clamp(state.kick_style26,0,2),kl=std::clamp(state.kick_leg26,0,2),kr=std::clamp(state.kick_reach_ms26,300,1200); // C26 KICKS page
             e.kickStyle26.store(ks);e.kickLeg26.store(kl);e.kickReachMs26.store(kr);e.kickFrees26.store(state.kick_frees_impale26);
             frik_pose_probe::SetKick26(ks,kl,kr,state.kick_frees_impale26);
             kcd2vr::kick26::nativeSpecial33.store(state.kick_special33);kcd2vr::kick26::fallbackLeg.store(state.kick_fallback33);kcd2vr::kick26::realHitTest37c2.store(state.kick_real_hit37c2);kcd2vr::kick26::profile52.store(std::clamp(state.kick_profile52,0,1));kcd2vr::kick26::reachPercent52.store(std::clamp(state.kick_reach52,60,100));} // C33
            {const int sm=std::clamp(state.stat_sim26,0,3);e.statSim26.store(sm);frik_pose_probe::SetStatSim26(sm);} // C26c
            g_vr_shadow_quality49.store(kcd2vr::graphics_owner31::clampShadowLevel(state.vr_shadow_quality49)); // C31/C49: Run owns the native CVar edge, menu only publishes preference
            g_eye_head26.store(state.eye_head26); // C26d
        }
    }
#define KCD2VR_M2_HOST_SLOT 83 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=83
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 84 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=84
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // One visible reload preference. The retired boolean-only quiver path cannot compete.
#define KCD2VR_M2_HOST_SLOT 85 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=85
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR

#define KCD2VR_CONFIG_APPLY_RANGED_RELOAD0
#include "subsystems/ranged_reload/config.inl"
#undef KCD2VR_CONFIG_APPLY_RANGED_RELOAD0

#define KCD2VR_M2_HOST_SLOT 86 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=86
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 87 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=87
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 88 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=88
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    touch_bindings_.SetTurnMode(state.vr_turn_mode);
    if (old_balanced != state.balanced_graphics || std::fabs(old_clarity - engine_clarity) > 0.0001f)
        graphics_profile_refresh_requested_.store(true, std::memory_order_release);

    CameraHook::SetTrackingOptions(state.head_rotation, state.head_position);
    CameraHook::SetPoseSourceMode(pose_source_mode, 0.0f);
    CameraHook::SetRotationOrder(rotation_order);
    CameraHook::SetCameraCalibrationOffset(camera_side, camera_back, camera_height);
    CameraHook::SetAfrIpd(0.064f, ipd_scale);
#define KCD2VR_M2_HOST_SLOT 89 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=89
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR

    // H26: explicit FOV edits and mode changes commit the same shared FOV.
    if(old_cyclopean!=cyclopean || std::fabs(previous_game_fov-game_fov_target)>.001f) {
        // V66.30: never force the historical 120-degree KCD camera when entering
        // Correct FOV. Render geometry at the requested Game FOV and map that known
        // source frustum to the requested VR FOV in map 5.
        const float transition_fov = game_fov_target;
        const bool fov_ok = ApplyNativeGameFov(transition_fov);
        char fm[448]{}; std::snprintf(fm,sizeof(fm),
            "V66.31-VISION modeTransition %d->%d selectedGameFov=%.1f selectedVrFov=%.1f map=%d zoom=%.2f gameFovWrite=SELECTED-ONLY result=%d",
            old_cyclopean,cyclopean,double(transition_fov),double(source_fov),
            (native_production&&cyclopean==1)?5:MonoMapModeForCleanMode(cyclopean),double(cyclopean==1?1.0f:effective_zoom),fov_ok?1:0);
        diagnostic_trace::Write(fm);
    }

    // V65.04: the F2 checkbox is the first write-side ownership test. It keeps
    // the trusted fixed8ms/adaptive35 baseline and enables only the rung-3
    // renderer matcher plus renderer-owned PRESENT token selection.
    if(!old_f2_fix || !FrameSync::HeadOwnershipFixEnabled()) {
        f2_ownership_fix_.store(true,std::memory_order_release);
        FrameSync::SetHeadOwnershipFixEnabled(true);
        FrameSync::SetHeadOwnershipProbeLevel(3);
        (void)FrameSync::SetPipelineProbeEnabled(true);
        CameraHook::SetAngularDiagnosticsEnabled(false);
        CameraHook::SetRotationFilterMode(5);
        if(xr_manager_) xr_manager_->SetPresentPacingMode(0);
    }

    // V60.5: MONO is a hard state gate, not just a one-shot reset.  V60.4
    // reset the XR manager above and then immediately re-applied the stale
    // persisted stereo mode here, which could bring the doubled view back.
    // V61.0 explicit renderer-test modes. AFR is frame-locked at ViewCamera publication.
    // DUAL is a diagnostic CryEngine/native CRenderView path and leaves the PRESENT bridge mono.
    if (stereo == 1) {
        if (g_v610_manual_dual_active) { RestoreV54StereoCVars(g_v610_manual_stereo_original); g_v610_manual_dual_active=false; }
        FrameSync::SetNativeStereoReentryMode(0, 0.064f);
        DeviceHook::SetAfrStereoCapture(true);
        FrameSync::SetStereoMode(1);
    } else if (stereo == 2) {
        DeviceHook::SetAfrStereoCapture(false);
        FrameSync::SetStereoMode(0);
        float runtime_ipd=0.064f; XrManager::StereoViewSnapshot sv{};
        if (xr_manager_ && xr_manager_->CopyLatestStereoViews(sv) && sv.raw_ipd>0.03f && sv.raw_ipd<0.10f) runtime_ipd=sv.raw_ipd;
        const float test_ipd=std::clamp(runtime_ipd*std::max(0.05f,std::fabs(ipd_scale)),0.004f,0.095f);
        if (!g_v610_manual_stereo_original.valid) g_v610_manual_stereo_original=CaptureV54StereoCVars();
        ApplyV54StereoCVars(test_ipd, state.swap_eyes);
        g_v610_manual_dual_active=true;
        FrameSync::SetNativeStereoReentryMode(2, test_ipd);
        char ds[256]{}; std::snprintf(ds,sizeof(ds),"V61.0-DUAL-MENU engineStereo=ON reentry=selector01 testIpd=%.4fm scale=%.2f",(double)test_ipd,(double)ipd_scale); debug_trace::Write(ds);
    } else {
        DeviceHook::SetAfrStereoCapture(false);
        FrameSync::SetStereoMode(0);
        FrameSync::SetNativeStereoReentryMode(0, 0.064f);
        CameraHook::SetAfrIpd(0.064f, 0.0f);
        if (g_v610_manual_dual_active) { RestoreV54StereoCVars(g_v610_manual_stereo_original); g_v610_manual_dual_active=false; }
    }

    char m[320]{};
#define KCD2VR_M2_HOST_SLOT 90 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=90
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    debug_trace::Write(m);
    // D86: 0.95x framing and 95-degree FOV are different controls. Preserve
    // the user's actual apply event alongside the current queue identity; a
    // settings write does not prove that a fresh scene reached the headset.
    if(diagnostics::presentation_d86::selected()&&
       (std::fabs(previous_zoom_d86-zoom)>1e-5f||std::fabs(state.presentation_zoom-zoom)>1e-5f||old_cyclopean!=cyclopean)){
        diagnostics::presentation_d86::apply(std::uint64_t(GetTickCount64()),previous_zoom_d86,state.presentation_zoom,zoom,
            cyclopean,production_stereo,[&]() noexcept {
                diagnostics::presentation_d86::Snapshot observation{};
                if(xr_manager_){const auto view=xr_manager_->ObservePresentationD86();observation.xrKnown=true;
                    observation.xrZoom=view.zoom;observation.dialogueMultiplier=view.dialogueMultiplier;
                    observation.sourceFov=view.sourceFov;observation.presentation=view.presentation;observation.mapping=view.mapping;}
                const auto queued=native_upscaler_stereo_d82::LastPublication();
                observation.publicationRevision=queued.serial;observation.publishedEpoch=queued.epoch;observation.publishedFrame=queued.frame;
                observation.publishedTechnology=queued.technology;observation.producedPairs=DeviceHook::BridgeCompletePairCount();
                observation.consumedPairSerial=DeviceHook::V6634LastConsumedPairSerial();observation.presents=DeviceHook::PresentCaptureCount();
                const auto dlss=native_dlss_stereo_d82::GetStatus();observation.dlssEnabled=dlss.enabled;observation.dlssFault=unsigned(dlss.fault);
                return observation;
            },diagnostic_trace::Write);
    }
    return true;
}

bool Mod::SaveMenuSettings() const
{
    char path[MAX_PATH * 2]{};
    if (!GetLoadedModuleSiblingPath("KCD2VR.ini", path, static_cast<DWORD>(sizeof(path)))) return false;
    // D62: never reconstruct a missing/unreadable INI from current menu
    // defaults. Serialize with calibration and checkpoint the CURRENT file.
    core::config_file_guard_d62::Lease iniLeaseD62(path);
    if(!iniLeaseD62.valid()){
        char message[192]{};std::snprintf(message,sizeof(message),"D62 CONFIG_SAVE_REFUSED reason=%s existingProfile=UNTOUCHED restore=NOT_ATTEMPTED",
            core::config_file_guard_d62::name(iniLeaseD62.status()));diagnostic_trace::Write(message);return false;
    }
    if(!iniLeaseD62.checkpointed()&&!core::config_file_guard_d62::checkpointFailureShown.exchange(true))
        diagnostic_trace::Write("D62 CONFIG_CHECKPOINT_UNAVAILABLE existingReadableProfile=RETAINED writes=PER_KEY_MERGE");
    auto wb = [&](const char* key, bool v) { return WritePrivateProfileStringA("Settings", key, v?"true":"false", path) != FALSE; };
    auto wi = [&](const char* key, int v) { char b[64]{}; std::snprintf(b,sizeof(b),"%d",v); return WritePrivateProfileStringA("Settings",key,b,path) != FALSE; };
    auto wf = [&](const char* key, float v) { char b[64]{}; std::snprintf(b,sizeof(b),"%.6g",static_cast<double>(v)); return WritePrivateProfileStringA("Settings",key,b,path) != FALSE; };
    bool ok = true;
    ok=wb("VrIndependentThrustD67",thrust_tuning_d67::enabled.load())&&ok;
    ok=wi("VrMotionTrialD67",int(motion_trials_d67::MenuMode()))&&ok;
    ok=wb("VrElbowPreferenceD67",motion_trials_d67::ElbowPreferenceR92())&&ok;
    for(int c=0;c<17;++c){const auto v=thrust_tuning_d67::read(c);char key[80]{};std::snprintf(key,sizeof(key),"VrThrustSpeedD67_%d",c);ok=wf(key,v.speed)&&ok;std::snprintf(key,sizeof(key),"VrThrustTravelD67_%d",c);ok=wf(key,v.travel)&&ok;}
    ok=wb("VrFreeHandPresentLeadD61",hand_present_d61::Enabled())&&ok;
    for(unsigned c=0;c<17;++c){const auto v=melee_tuning_c54::read(int(c));char key[64]{};std::snprintf(key,sizeof(key),"VrMeleeTypeC54_%u_Cut",c);ok=wf(key,v.cut)&&ok;std::snprintf(key,sizeof(key),"VrMeleeTypeC54_%u_Stab",c);ok=wf(key,v.stab)&&ok;std::snprintf(key,sizeof(key),"VrMeleeTypeH29_%u_Intent",c);ok=wf(key,v.intentH29)&&ok;std::snprintf(key,sizeof(key),"VrMeleeTypeH29_%u_Sensitivity",c);ok=wf(key,v.sensitivityH29)&&ok;std::snprintf(key,sizeof(key),"VrMeleeTypeR8_%u_MinTravel",c);ok=wf(key,v.travelR8)&&ok;}
    ok=wf("VrBladeAxisAlignmentH41",melee_tuning_c54::bladeAxisSettingH41(melee_tuning_c54::bladeAxisCosineH41.load()))&&ok;
    ok=wf("VrPunchMinimumTravelMetersD65",punch_travel_d65::value())&&ok;
    ok=wf("VrPunchSensitivityC54",melee_tuning_c54::punch())&&ok;ok=wf("VrFistBlockSensitivityH29",melee_tuning_c54::blockH29())&&ok;ok=wb("VrCraftProximityTrialH41",subsystems::alchemy::reach_craft1::enabled.load())&&ok;ok=wb("VrDisableExtraModAttackVetoesH41",melee_tuning_c54::bypassExtraAttackVetoesH41.load())&&ok;ok=wf("VrChestPitchShareH41Trial",frik_pose_probe::ChestPitchShareCB())&&ok;ok=wb("VrArcherStanceH41Trial",frik_pose_probe::ArcherStanceCB())&&ok;ok=wf("VrPunchTravelH41",melee_tuning_c54::punchTravel.load())&&ok;ok=wf("VrGuardPlaneHalfWidthH41",melee_tuning_c54::guardHalfWidth.load())&&ok;ok=wf("VrGuardPlaneHalfHeightH41",melee_tuning_c54::guardHalfHeight.load())&&ok;ok=wf("VrGuardBoxDepthD61",melee_tuning_c54::guardBoxDepthD61.load())&&ok;ok=wf("VrGuardMaxBodyDistanceD56",melee_tuning_c54::guardBodyDistanceD56.load())&&ok;ok=wf("VrPerfectBlockScaleH41",player_window_h26::perfectBlockMultiplier.load())&&ok;ok=wf("VrMasterstrikeScaleH41",player_window_h26::multiplier.load())&&ok;
    for(unsigned i=0;i<shield_block_r120::Count;++i)ok=wf(shield_block_r120::kSpecs[i].key,shield_block_r120::get(i))&&ok; // R120 shield block box
    for(int h=0;h<2;++h)for(int f=0;f<5;++f){char key[64]{};std::snprintf(key,sizeof(key),"VrFingerMiddleLateralC54_%c_%d",h?'L':'R',f);ok=wf(key,finger40::getMiddleLateralC54(h,f))&&ok;}
    for(int h=0;h<2;++h){char key[80]{};std::snprintf(key,sizeof(key),"VrFingerMiddleBaseShiftMmH31_%c",h?'L':'R');ok=wf(key,finger40::getShiftMmH31(h))&&ok;std::snprintf(key,sizeof(key),"VrFingerMiddleDistalMedialDegH31_%c",h?'L':'R');ok=wf(key,finger40::getDistalMedialDegH31(h))&&ok;} // H31_FINGER_SAVE

    ok = wb("VrDeveloperMode", developer_mode::Enabled()) && ok;
    ok = wb("Enabled", enabled_.load(std::memory_order_acquire)) && ok;
    ok = wf("HeadPoseScale", head_pose_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrCameraSideOffsetMeters", camera_side_offset_m_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrCameraBackOffsetMeters", camera_back_offset_m_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrCameraHeightOffsetMeters", camera_height_offset_m_.load(std::memory_order_acquire)) && ok;
    ok = wb("ApplyHeadRotation", apply_head_rotation_.load(std::memory_order_acquire)) && ok;
    ok = wb("ApplyHeadPosition", apply_head_position_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrPoseSourceMode", vr_pose_source_mode_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrPosePredictionOffsetMs", vr_pose_prediction_offset_ms_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrGameFov", menu_game_fov_target_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrHorizontalFov", menu_horizontal_fov_target_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrFpsCapEnabled", menu_fps_cap_enabled_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrFpsCap", menu_fps_cap_target_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrMergedMeshDistance", menu_merged_mesh_target_.load(std::memory_order_acquire)) && ok;
#define KCD2VR_M2_HOST_SLOT 91 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=91
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wi("VrHudLayoutVersion",1202) && ok;
    ok = wi("VrHeadSmoothPreferenceVersion",3) && ok;
    ok = wb("VrF2OwnershipFix", true) && ok; // V65.10 compatibility key; F2 ownership is always ON
#define KCD2VR_M2_HOST_SLOT 92 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=92
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 93 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=93
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 94 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=94
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 95 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=95
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 96 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=96
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 97 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=97
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 98 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=98
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 99 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=99
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 100 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=100
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 101 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=101
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wb("BowManualReload", false) && ok; // retired comparison path, never a second loader
#define KCD2VR_M2_HOST_SLOT 102 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=102
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wb("DiagnosticRendering", diagnostic_rendering_.load(std::memory_order_acquire)) && ok;
    ok = wb("EnableVrOutputBridge", enable_vr_output_bridge_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrRenderScale", vr_render_scale_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrSourceWidth", vr_source_width_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrSourceHeight", vr_source_height_.load(std::memory_order_acquire)) && ok;
    ok = wb("EnableVrBalancedGraphics", enable_vr_balanced_graphics_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrEngineClarity", vr_engine_clarity_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrGamma", vr_gamma_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrBrightness", vr_brightness_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrContrast", vr_contrast_.load(std::memory_order_acquire)) && ok;
#define KCD2VR_M2_HOST_SLOT 103 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=103
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 104 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=104
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wi("VrFirstPersonActivityPreferenceVersion",1) && ok;
#define KCD2VR_M2_HOST_SLOT 105 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=105
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 106 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=106
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 107 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=107
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wi("VrHeadAimPreferenceVersion",4) && ok;
#define KCD2VR_M2_HOST_SLOT 108 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=108
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wi("VrTurnMode", vr_turn_mode_.load(std::memory_order_acquire)) && ok;
#define KCD2VR_M2_HOST_SLOT 109 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=109
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 110 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=110
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wb("VrRoomscaleFollow", g_roomscale_follow58.load(std::memory_order_acquire)) && ok; // R59: menu-owned
    ok = wb("VrRoomscaleDodge", g_roomscale_dodge58.load(std::memory_order_acquire)) && ok;
    ok = wb("VrNativeHandPhysics", frik_pose_probe::NativeHandPhysicsH24()) && ok;
    ok = wb("VrPreciseSurfaceContacts", frik_pose_probe::PreciseContacts44()) && ok;
    ok = wi("VrHandsModeDs2",frik_pose_probe::HandsModeDs2()) && ok;
    ok = wb("VrHandSeparationDs1",frik_pose_probe::HandSeparationDs1()) && ok;
    ok = wf("VrShoulderReachWarningM",frik_pose_probe::GetShoulderReachD38()) && ok;
    ok = wi("VrCameraBodyHoldMs",int(camera_hold_e1::maxHoldMs.load())) && ok;
    ok = wb("VrBowArmAim",bow_arm_aim_r9::enabled.load()) && ok;
    ok = wb("VrHandSmoothH50",smooth_lab::BaselineSmoothingD39()) && ok;
    ok = wf("VrHandSmoothMinHzH50",hand_smooth_h50::minCutoffHz.load()) && ok;
    ok = wf("VrHandSmoothBetaH50",hand_smooth_h50::beta.load()) && ok;
    ok = wb("VrAlchemyOwnHeadH51",alchemy_own_head_h51::enabled.load()) && ok;
    ok = wb("VrWorkstationGestures",workstation178r20c1::gesturesEverywhere59c2.load()) && ok;
    ok = wb("VrPetGestureD59",petting_d59::Enabled()) && ok;
    ok = wb("VrWashGestureD59",washing_d59::Enabled()) && ok;
    ok = wb("VrAlchemyTransferD59",misc_gestures_d59::alchemyTransfer.load()) && ok;
    ok = wi("VrAlchemyPickupReachD39",subsystems::alchemy::reach_craft1::getPickupReachD39()) && ok;
    ok = wb("VrTorchGestureD39",utility54::torchGestureD39.load()) && ok;
    ok = wb("VrVisorGestureD39",utility54::visorGestureD39.load()) && ok;
#define KCD2VR_D40_LIVE_SAVE
#include "subsystems/experimental/live_runtime_d40.inl"
#undef KCD2VR_D40_LIVE_SAVE

#define KCD2VR_CONFIG_SAVE_GORE0
#include "subsystems/gore/config.inl"
#undef KCD2VR_CONFIG_SAVE_GORE0

    ok = wb("VrKickFistLane", frik_pose_probe::FistKickEnabledH35()) && ok;
    ok = wb("VrGripStow", kcd2vr::grip_stow_cb1::Enabled()) && ok;
    ok = wi("VrGripStowButton", kcd2vr::grip_stow_cb1::Button()) && ok;
    ok = wb("VrGestureTalk", g_gesture_talk59c5.load(std::memory_order_acquire)) && ok; // R59.16C5: menu-owned
    ok = wb("VrSmoothMovement", g_smooth_move51.load(std::memory_order_acquire)) && ok;
    ok = wb("VrHmdWalkingDirectionR134", g_hmd_walking_direction134.load(std::memory_order_acquire)) && ok;
    ok = wb("VrNativeCrouchR93", frik_pose_probe::NativeCrouchR93()) && ok;
    ok = wi("VrBodyHeightModeR98", frik_pose_probe::BodyHeightModeR98()) && ok; // R98: 0 HMD, 1 NATIVE, 2 HMD + NATIVE (ride/crouch)
    ok = wb("VrHmdBodyHeight", frik_pose_probe::HmdBodyHeight54()) && ok; // old bool, kept for older builds
    ok = wb("VrChestDagger", frik_pose_probe::ChestDagger54()) && ok;

#define KCD2VR_CONFIG_SAVE_THROW_DROP0
#include "subsystems/throw_drop/config.inl"
#undef KCD2VR_CONFIG_SAVE_THROW_DROP0


#define KCD2VR_CONFIG_SAVE_THROW_DROP1
#include "subsystems/throw_drop/config.inl"
#undef KCD2VR_CONFIG_SAVE_THROW_DROP1

    ok = wi("VrComfortVignette", g_comfort_vignette51.load(std::memory_order_acquire)) && ok;
    ok = wi("VrReadingPages", kcd2vr::book_pages45::clampMode(kcd2vr::book_pages45::mode.load())) && ok; // C45: 0 modifier+flick, 1 modifier only, 2 native (C42 stick + grips)
    ok = wb("VrQuickAccessFaceTouchR112", kcd2vr::quick_access_r112::FaceTouch()) && ok; // R113
    ok = wb("VrAlchemyHands",activity_input163::trackedHands59c7f[0].load()) && ok;
    ok = wb("VrSmithingHands",activity_input163::trackedHands59c7f[1].load()) && ok;
    ok = wb("VrSharpeningHands",activity_input163::trackedHands59c7f[3].load()) && ok;
    ok = wb("VrReadingHands",activity_input163::trackedHands59c7f[2].load()) && ok;
#define KCD2VR_M2_HOST_SLOT 111 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=111
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR

#define KCD2VR_CONFIG_SAVE_DIALOGUES0
#include "subsystems/dialogues/config.inl"
#undef KCD2VR_CONFIG_SAVE_DIALOGUES0

    ok = wb(kcd2vr::calibration_restore43::kIniKey,frik_pose_probe::CalibrationAutoRestore43()) && ok; // C43: menu-owned

#define KCD2VR_CONFIG_SAVE_SMITHING0
#include "subsystems/smithing/config.inl"
#undef KCD2VR_CONFIG_SAVE_SMITHING0

    ok = wi("VrMinigameHands",minigame_hands45::clampMode(minigame_hands45::mode.load())) && ok;
    ok = wi("VrAlchemyMode",minigame_hands45::modeFor(minigame_hands45::Alchemy)) && ok;
    ok = wi("VrSmithingMode",minigame_hands45::modeFor(minigame_hands45::Smithing)) && ok;
    ok = wb("VrWorkstationHands",combat_rules178r10::workstationEnabled.load()) && ok;

#define KCD2VR_CONFIG_SAVE_DOORS0
#include "subsystems/doors/config.inl"
#undef KCD2VR_CONFIG_SAVE_DOORS0

    ok = wb("VrGrabHandStaysOnController",frik_pose_probe::GrabHandStays43()) && ok; // C43: menu-owned
#define KCD2VR_M2_HOST_SLOT 112 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=112
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wb("VrShowInteractionHints",prompt_policy_d80::showHints.load()) && ok;
    ok = wb("VrTimedInteractionPrompts",prompt_policy_d80::timed.load()) && ok;
    ok = wf("VrMinigamePromptScale",prompt_policy_d80::minigameScale.load()) && ok;
    ok = wb("VrDismemberment", kcd2vr::gorepak50::dismember.load(std::memory_order_acquire)) && ok; // C50 (gore pak only)
    ok = wi("VrDismembermentVersion",2) && ok; // D56: qualified male-human full-swing corpse/kill limb path
#define KCD2VR_M2_HOST_SLOT 113 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=113
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 114 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=114
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wb("VrLabLivingHitImpact58", false) && ok;
    ok = wb("VrLabCorpseDrag58", kcd2vr::lab58::corpseDrag.load(std::memory_order_acquire)) && ok;
    ok = wb("VrLabPropWake58", false) && ok;
    ok = wf("VrLabLivingHitK58", kcd2vr::lab58::livingHitK.load()) && ok;
    ok = wi("VrGrabMode",frik_pose_probe::GrabMode60()) && ok; // 0 native, 2 staged hold; 1 unsafe/unsupported
#define KCD2VR_M2_HOST_SLOT 115 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=115
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wi("VrPhysBodies", g_phys_bodies59c10r8.load()) && ok; // R59.16C10R8
    ok = wb("VrHeldMap",g_held_map59c10r10.load()) && ok;
    ok = wb("VrHeldParchmentMapDs7",g_held_map59c10r10.load()) && ok;
    ok = wi("VrHeldMapRegion",int(held_map_m1::GetPrintedRegion())) && ok;
#define KCD2VR_M2_HOST_SLOT 116 // M2_SOURCE_SELECTOR
#include "subsystems/experimental/host.inl" // M2_OWNED_SOURCE slot=116
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 117 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=117
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok=wb("VrFinger42OpenNativeMigrated",true)&&ok;
    ok=wb("VrFinger47BothHandsMigrated",true)&&ok;
    ok=wb("VrGesture47PreferencesMigrated",true)&&ok;
    ok=wi("VrGestureTalkVersion",kcd2vr::gesture_talk_policy_d33::kVersion)&&ok; // D33
    // VrAlchemyPassenger42 is retired; do not resave an ignored switch.
    ok = wf("VrNpcProximityRadius36",frik_pose_probe::NpcProximityRadius36()) && ok;
#define KCD2VR_M2_HOST_SLOT 118 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=118
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wb("VrNpcTouchReact",g_touch43.load()) && ok; ok = wb("VrNpcFallAndPlay",g_fallplay43.load()) && ok; // C43T
    ok = wf("VrHeldPropClearanceMeters35",kcd2vr::grasp13::heldClearance35.load(std::memory_order_acquire)) && ok;
    ok = wf("VrPropStowRadiusMeters35",kcd2vr::grasp13::stowRadius35.load(std::memory_order_acquire)) && ok;
    {const auto& e=g_exp59c10; // R59.16C10d
     ok = wb("VrWallSlide",e.wallSlide.load()) && ok; ok = wb("VrWallSlideOptInH18",e.wallSlide.load()) && ok; ok = wb("VrTelegraphWindup",e.telegraphWindup.load()) && ok; ok = wb("VrTelegraphStrike",e.telegraphStrike.load()) && ok; ok = wb("VrWindupHold",e.windupHold.load()) && ok; ok = wb("VrGuardRequireCounterMotion",e.guardCounter.load()) && ok; ok = wb("VrGuardAutoWindupRefresh",e.guardAutoWindup.load()) && ok;
     ok = wb("VrFastBlockR92",release_combat_r92::fastBlock.load()) && ok;
     ok = wf("VrGuardEnterMaxSpeed",e.guardEnterSpeed.load()) && ok; ok = wi("VrGuardEnterDwellMs",e.guardEnterDwellMs.load()) && ok; ok = wf("VrGuardExitSpeed",e.guardExitSpeed.load()) && ok; ok = wi("VrGuardAfterAttackMs",e.guardAfterAttackMs.load()) && ok;
     ok = wf("VrStrikeReferenceJ",e.strikeRefJ.load()) && ok; ok = wf("VrStrikeMaxPower",e.strikeMaxPower.load()) && ok; ok = wf("VrStrikeTwoHandedBonus",e.strikeTwoHanded.load()) && ok; ok = wf("VrHeadRaiseMaxCm",e.headRaiseCm.load()) && ok;
     ok = wb("VrStrikePower",e.strikePower.load()) && ok; ok = wf("VrStrikeMinPower",e.strikeMinPower.load()) && ok; ok = wi("VrStrikeRepeatMs",e.strikeRepeatMs.load()) && ok; ok = wi("VrRiposteWindowMs",e.riposteWindowMs.load()) && ok; // R109 MISC 3 (were read-only INI keys)
     ok = wb("VrDirectNpcBlockBySkill",e.directBlock39.load()) && ok; ok = wf("VrDirectNpcBlockSpeedLow",e.directBlockLo39.load()) && ok; ok = wf("VrDirectNpcBlockSpeedHigh",e.directBlockHi39.load()) && ok; // C39
     ok = wf("VrNpcBlockEtaPrepare",e.npcEtaPrepare.load()) && ok; ok = wf("VrNpcBlockEtaStrike",e.npcEtaStrike.load()) && ok; ok = wb("VrAttackRequireOpponent",e.attackRequireOpponent.load()) && ok; ok = wf("VrAttackReachMeters",e.attackReachM.load()) && ok;
     ok = wi("VrActionTimeOwner",e.actionOwner.load()) && ok; ok = wf("VrActionHoldAt",e.actionHoldAt.load()) && ok; ok = wf("VrActionResumeSpeed",e.actionResumeSpeed.load()) && ok;
     ok = wb("VrSyncOverdueDirect",e.syncOverdueAny.load()) && ok; ok = wb("VrDirectStrikeInRecovery",e.recoveryOverride.load()) && ok; ok = wb("VrDirectHitWithoutDefenseRow",e.noRowFallback.load()) && ok;
     ok = wb("VrReclaimPhaseFromOwnGuard",e.reclaimGuard.load()) && ok; ok = wi("VrDirectStaggerRule",e.staggerRule44.load()) && ok; // R59.16C10R16 / C44 (replaces VrStrikeDuringStagger)
     ok = wf("VrWeaponWeightFeel",e.weightFeel.load()) && ok; ok = wf("VrWeaponFatigueRamp",e.fatigueRamp.load()) && ok; ok = wf("VrPoleOneHandFeel",e.poleOneHand.load()) && ok; ok = wf("VrSwingStaminaScale",e.swingStaminaScale.load()) && ok;
     ok = wf("VrStrikeMinScale",e.strikeMinScale.load()) && ok; ok = wi("VrStrikeMinIntervalMs",e.strikeMinIntervalMs.load()) && ok; ok = wb("VrStrikeMinScaleGate",e.strikeMinGate.load()) && ok;
     ok = wi("VrMasterStrikeVeto",e.masterStrikeGame.load()?0:1) && ok; ok = wi("VrHideStowedBow",e.hideStowedBow.load()) && ok; ok = wb("VrTwoHandAxisGrip",e.twoHandAxis.load()) && ok; // R59.16C10R5 / R11 / R12 / R14
     ok = wb(block_indicator_h24::iniKey,e.showBlockIndicatorH24.load()) && ok; ok = wb("VrGuardStrictEntry",e.guardStrict.load()) && ok; ok = wb("VrNoBlockCooldownF4",e.noBlockCooldown.load()) && ok; ok = wb("VrMasterStrikeKeepStance",e.msKeepStance.load()) && ok; // R59.16C10R17
     ok = wb("VrInstantFreeSwing",e.instantFreeSwing38.load()) && ok; ok = wb("VrHideSwingLean",e.hideSwingLean38.load()) && ok; // C38
     ok = wb("VrDirectGuardOutOfStagger",e.staggerGuard39.load()) && ok; // C39
     {namespace cs=kcd2vr::counter_stab40;ok = wb("VrDirectCounterStab",cs::enabled.load()) && ok;ok = wf("VrDirectCounterStabEnergy",cs::minEnergy.load()) && ok;ok = wf("VrDirectCounterStabStrength",cs::strengthMul.load()) && ok;ok = wi("VrDirectCounterStabWindowMs",int(cs::windowMs.load())) && ok;
      ok = wb("VrDirectCounterStabNeedsPerk",cs::needsPerk.load()) && ok;} // C40 / C44
     {namespace mg=kcd2vr::master_gate47;const int m120=mg::clampMode(mg::mode.load());ok = wi("VrMasterStrikeModeR120",m120) && ok;ok = wi("VrMasterStrikeStabOnly",m120==mg::kModeInstant?1:m120) && ok;ok = wf("VrMasterStrikeStabEnergy",mg::clampEnergy(mg::minEnergy.load())) && ok;} // C47 (DS-F5 VrMasterStrikeTestForce is session-only and NOT saved)
     ok = wb("VrDirectNpcDirectionalBlock",e.dirBlock39.load()) && ok; // C39 DIRBLOCK
     ok = wb("VrIntentLiveRoute",e.intentLive.load()) && ok; ok = wi("VrIntentLiveWaitMs",e.intentLiveMs.load()) && ok; ok = wi("VrComboWindowMs",e.comboWindowMs.load()) && ok; ok = wb("VrComboWindowExtend",combat_diag_probe::ComboWindowExtension()) && ok; ok = wb("VrComboZoneAssist",kcd2vr::combo_zone_assist_cz::enabled.load()) && ok; ok = wb("VrFeintHybrid",kcd2vr::feint_fe::enabled.load()) && ok; ok = wb("VrComboRecoveryHold",kcd2vr::combo_hold_r14::enabled.load()) && ok; ok = wi("VrComboRecoveryHoldMs",int(kcd2vr::combo_hold_r14::maxHoldMs.load())) && ok; ok = wf("VrComboWindowSeconds",float(e.comboWindowMs.load())*.001f) && ok; // DS-F6 VrComboTestForce is session-only and NOT saved
     ok = wi("VrF2Rung",kcd2vr::smooth_lab::BugRecordingRung(kcd2vr::smooth_lab::Mode())?0:int(kcd2vr::smooth_lab::Mode())) && ok; // J26 (C29: recording rungs are not remembered)
     ok = wb("VrStationTorsoFollowHead",kcd2vr::station_ws_j27::torsoEnabled.load()) && ok; // J27
     ok = wb("VrSmithToolCollision",kcd2vr::station_ws_j27::toolCollision.load()) && ok; // J27
     ok = wi("VrSmithAnvilContact",subsystems::smithing::anvil_a2::mode.load()) && ok; // A3 0 off 1 real 2 learned
     ok = wf("VrStationChestClearCm",kcd2vr::station_ws_j27::clearCm.load()) && ok; // J27
     ok = wf("VrNativeHitTime",e.nativeHitTime.load()) && ok; ok = wf("VrWeaponStatEffect",e.weaponStatEffect.load()) && ok;
     ok = wb("VrDirectOwnsHits",e.ownHits18.load()) && ok; ok = wf("VrNpcBlockSpeed",e.npcBlockSpeed18.load()) && ok; // C10R18
     ok = wf("VrMasterHandoffForwardSpeed",e.msHandoff23.load()) && ok; // C24b
     ok = wi("VrKickStyle",e.kickStyle26.load()) && ok; ok = wi("VrKickLeg",e.kickLeg26.load()) && ok; ok = wi("VrKickReachMs",e.kickReachMs26.load()) && ok; ok = wb("VrKickFreesImpale",e.kickFrees26.load()) && ok; ok = wb("VrKickSpecialInput",kcd2vr::kick26::nativeSpecial33.load()) && ok; ok = wb("VrKickFallbackLeg",kcd2vr::kick26::fallbackLeg.load()) && ok; ok = wb("VrKickRealHitTest",kcd2vr::kick26::realHitTest37c2.load()) && ok; ok = wi("VrKickProfile",kcd2vr::kick26::profile52.load()) && ok; ok = wi("VrKickReach",kcd2vr::kick26::reachPercent52.load()) && ok; ok = wi("VrSimulateHenry",e.statSim26.load()) && ok; ok = wi("VrShadowQuality",g_vr_shadow_quality49.load()) && ok; ok = wb("VrShadowTweaks",g_vr_shadow_quality49.load()==2) && ok; ok = wb("VrEyeAdaptHeadSteady",g_eye_head26.load()) && ok;} // C26 / C26c / C26d
    ok = wb("VrAutoPalmAlign", g_auto_palm59c8.load(std::memory_order_acquire)) && ok; ok = wf("VrTargetSwitchDwell", g_target_dwell59c8.load(std::memory_order_acquire)) && ok; // R59.16C8
#define KCD2VR_M2_HOST_SLOT 119 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=119
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wi("VrPresentationMode", vr_presentation_mode_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrPresentationZoom", vr_zoom_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrAutoFovTrimDegrees", vr_auto_fov_trim_deg_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrAutoCenterXTrim", vr_auto_center_x_trim_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrAutoCenterYTrim", vr_auto_center_y_trim_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrAutoProjectionCenter", vr_auto_projection_center_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrPitchCorrectionDeg", vr_pitch_correction_deg_.load(std::memory_order_acquire)) && ok;
#define KCD2VR_M2_HOST_SLOT 120 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=120
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wf("VrSourceVerticalFovDegrees", vr_source_vertical_fov_deg_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrCorrectedCenterScaleFovDegrees", vr_corrected_view_vfov_deg_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrCullFrustumScale", vr_cull_frustum_scale_.load(std::memory_order_acquire)) && ok;
#define KCD2VR_M2_HOST_SLOT 121 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=121
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 122 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=122
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wf("VrFlatScreenScale", vr_flat_screen_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrPassiveFullFrameScale", vr_passive_full_frame_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrHudDistanceR110", kcd2vr::ui_surface_geometry::HudDistanceR110()) && ok; // R110
    ok = wf("VrCombatCrosshairScaleR112", kcd2vr::ui_hud_r112::CombatCrosshairScale()) && ok; // R112
    ok = wf("VrPauseOverlaySize", vr_pause_overlay_size_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrTransientZoomRetain", vr_transient_zoom_retain_.load(std::memory_order_acquire)) && ok;
#define KCD2VR_M2_HOST_SLOT 123 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=123
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ok = wf("VrLeftOffsetX", vr_left_offset_x_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrLeftOffsetY", vr_left_offset_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrRightOffsetX", vr_right_offset_x_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrRightOffsetY", vr_right_offset_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrDepthStereoStrength", vr_depth_stereo_strength_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrRotationOrder", vr_rotation_order_.load(std::memory_order_acquire)) && ok;
    ok = wb("MirrorMisMenuToDesktop", mirror_mis_menu_to_desktop_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrStereoExperiment", vr_production_stereo_mode_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrCleanPresentationMode", vr_cyclopean_mode_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrAfrIpdScale", vr_afr_ipd_scale_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrSwapEyes", vr_swap_eyes_.load(std::memory_order_acquire)) && ok;
    return ok;
}

// C25: the VR-menu/F1 save used to run SaveMenuSettings() inline on the XR/tracking
// loop: ~200 WritePrivateProfileStringA calls, each rewriting the ~840-line INI,
// blocked the loop ~1.5 s (FRAME_STALL_SPLIT watchdogToPolicy) -> headset flicker.
// Now the loop only bumps a counter; ONE detached writer saves the latest values and
// re-runs while newer requests arrived (coalesced). SaveMenuSettings() reads atomics
// only, so the writer needs no snapshot. No timers/polling; at most one writer.
namespace {
core::AsyncSaveCoalescer g_menu_save25;
std::atomic<bool> g_menu_save_beep25{false},g_menu_save_detached26{false};
// Detached workers and native trampolines are process-lifetime owners. A bounded
// flush is not module lifetime proof; pin before spawning, outside DllMain.
bool PinMenuSaveModule26c1() noexcept {
    static const bool pinned=[]() noexcept {
        HMODULE module=nullptr;
        return GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
            reinterpret_cast<LPCWSTR>(&PinMenuSaveModule26c1),&module)!=FALSE&&module;
    }();
    return pinned;
}
}
void Mod::RequestMenuSave(bool beep) const
{
    if(g_menu_save_detached26.load(std::memory_order_acquire)) return; // C26: no INI write during DLL detach
    if(!PinMenuSaveModule26c1()) {
        diagnostic_trace::Write("C26C1 CONFIG_SAVE_REFUSED reason=MODULE_LIFETIME_UNPROVEN");return;
    }
    if(beep) g_menu_save_beep25.store(true,std::memory_order_release);
    const Mod* self=this;
    const auto r=g_menu_save25.Request([](auto worker){ std::thread(std::move(worker)).detach(); },
        [self](std::uint64_t gen)->bool{
            const std::uint64_t t0=GetTickCount64();
            const bool ok=self->SaveMenuSettings();
            const std::uint64_t ms=GetTickCount64()-t0;
            debug_trace::Write(ok?"V63.1-CONFIG saved KCD2VR.ini beside ASI":"V63.1-CONFIG SAVE FAILED");
            char m[160]{};std::snprintf(m,sizeof(m),"C25 CONFIG_SAVE %s gen=%llu ms=%llu thread=WORKER",ok?"OK":"FAILED",(unsigned long long)gen,(unsigned long long)ms);
            diagnostic_trace::Write(m);
            if(g_menu_save_beep25.exchange(false,std::memory_order_acq_rel)&&ok) MessageBeep(MB_OK);
            return ok;});
    if(r==core::AsyncSaveCoalescer::Requested::Inline) diagnostic_trace::Write("C25 CONFIG_SAVE_INLINE reason=NO_THREAD_OR_SHUTDOWN");
}
void Mod::FlushMenuSave(const char* where) const
{
    g_menu_save25.Close();
    const bool idle=g_menu_save25.WaitIdle(std::chrono::milliseconds(3000));
    char m[128]{};std::snprintf(m,sizeof(m),"C25 CONFIG_SAVE_FLUSH where=%s idle=%d",where?where:"?",idle?1:0);
    diagnostic_trace::Write(m);
}

void Mod::OnRenderTick(ID3D12CommandQueue* queue)
{
    Mod& mod = Instance();
    if(mod.vr_menu_)mod.vr_menu_->SetBlockIndicatorH24(g_exp59c10.showBlockIndicatorH24.load(std::memory_order_acquire)&&mod.xr_manager_&&!mod.xr_manager_->GetFlatUiMode()&&
        combat_diag_probe::NativeBlockIndicatorH24(GetTickCount64()));
    // V58 hybrid ordering: refresh the dedicated menu texture first, let XR
    // copy the CLEAN game backbuffer + separate menu layer, then composite the
    // same book onto the desktop/monitor after the XR capture is complete.
    if (mod.vr_menu_) mod.vr_menu_->OnRenderThreadTick(queue);
    XrManager* xr = mod.xr_manager_;
    if (xr) {
        ID3D12Resource* menu_tex = mod.vr_menu_ ? mod.vr_menu_->GetQuadTexture() : nullptr;
        xr->SetMenuQuadSource(menu_tex, mod.vr_menu_ && mod.vr_menu_->IsLayerVisible(), mod.vr_menu_ && mod.vr_menu_->IsDiagnosticOnlyH24());
        xr->OnRenderThreadTick(queue);
    }
    // V58.3: hybrid desktop mirroring is OFF by default. V58.1 could feed a
    // previously composited desktop book back into a later captured backbuffer,
    // producing the user's "monitor view flashing behind the XR menu" symptom.
    if (mod.vr_menu_ && mod.mirror_mis_menu_to_desktop_.load(std::memory_order_acquire))
        mod.vr_menu_->OnDesktopOverlayTick(queue);
}

bool Mod::LoadConfig()
{
    // Logger::configure() sets up the process-default sink; log() returns it.
    // File name resolves relative to the runtime (game binary) directory.
    (void)diagnostics::unified_log_d81::Initialize();
    debug_trace::Write("logger configured; fallback trace active");
    // R118: the default-INI result is logged further down, once the diagnostic sink is on (else it would be dropped).

    char ini_path[MAX_PATH * 2]{};
    const bool have_ini_path =
        GetLoadedModuleSiblingPath("KCD2VR.ini", ini_path, static_cast<DWORD>(sizeof(ini_path)));

    // D62: complete-read proof precedes calibration reads, binding defaults and
    // every one-time migration. DetourModKit load/reload has no success result;
    // neither a returned void nor reload()==true proves a readable file.
    core::config_file_guard_d62::Lease iniLeaseD62(have_ini_path?ini_path:"KCD2VR.ini");
    if(!iniLeaseD62.valid()){
        char message[1024]{};std::snprintf(message,sizeof(message),
            "KCD2VR could not read the complete configuration file (%s):\n%s\n%s\nRestore KCD2VR.ini beside the mod DLL from your saved profile or the full mod package, then restart the game. Configuration migrations and calibration saves were skipped.",
            core::config_file_guard_d62::name(iniLeaseD62.status()),have_ini_path?ini_path:"KCD2VR.ini",
            core::default_ini_r118::lastResult==int(core::default_ini_r118::Result::Failed)?"(R118: the built-in default KCD2VR.ini could not be written - the game folder is not writable)\n":
            core::default_ini_r118::lastResult==int(core::default_ini_r118::Result::Exists)?"(R118: the existing file is kept as it is, it is never replaced by the default)\n":"");
        debug_trace::Write(message);diagnostic_trace::Write("D62 CONFIG_LOAD_REFUSED migrations=SKIPPED defaults=NOT_PERSISTED calibration=UNTOUCHED");
        if(!core::config_file_guard_d62::loadFailureShown.exchange(true))
            MessageBoxA(nullptr,message,"KCD2VR configuration unavailable",MB_OK|MB_ICONERROR);
        return false;
    }
    if(!iniLeaseD62.checkpointed()&&!core::config_file_guard_d62::checkpointFailureShown.exchange(true))
        diagnostic_trace::Write("D62 CONFIG_CHECKPOINT_UNAVAILABLE existingReadableProfile=RETAINED startup=CONTINUES");
    // Keep DetourModKit and explicit Win32 reads/writes on the same absolute
    // file, including the rare unresolved-module fallback.
    const auto configPathD62=iniLeaseD62.path().string();
    if(have_ini_path) frik_pose_probe::ConfigureCalibrationPersistence(configPathD62.c_str());

    config::Ini ini;
    if (have_ini_path)
    {
        ini.load(configPathD62.c_str());
        char trace[768]{};
        std::snprintf(trace, sizeof(trace), "loading config from ASI directory: %s", ini_path);
        debug_trace::Write(trace);
    }
    else
    {
        ini.load(configPathD62.c_str());
        debug_trace::Write("WARNING: could not resolve ASI-relative INI path; using verified absolute process-relative KCD2VR.ini");
    }

    auto settings = ini.section("Settings");
    settings.bind_log_level("LogLevel", "INFO"); // D81: sparse producers control capture volume; OFF was never a supported DMK level
    settings.bind("Enabled", enabled_, true);
    settings.bind("HeadPoseScale", head_pose_scale_, kcd2vr::view_defaults_b2::Get("HeadPoseScale", 1.0f));
    settings.bind("VrCameraSideOffsetMeters", camera_side_offset_m_, 0.0f);
    settings.bind("VrCameraBackOffsetMeters", camera_back_offset_m_, kcd2vr::view_defaults_b2::Get("VrCameraBackOffsetMeters", 0.0f));
    settings.bind("VrCameraHeightOffsetMeters", camera_height_offset_m_, kcd2vr::view_defaults_b2::Get("VrCameraHeightOffsetMeters", 0.035f));
    settings.bind("ApplyHeadRotation", apply_head_rotation_, kcd2vr::view_defaults_b2::Get("ApplyHeadRotation", true));
    settings.bind("ApplyHeadPosition", apply_head_position_, kcd2vr::view_defaults_b2::Get("ApplyHeadPosition", true));
    settings.bind("VrPoseSourceMode", vr_pose_source_mode_, kcd2vr::view_defaults_b2::Get("VrPoseSourceMode", 2)); // C26C1: preserve fresh sampling default
    settings.bind("VrGameFov", menu_game_fov_target_, kcd2vr::view_defaults_b2::Get("VrGameFov", kV6504DefaultGameFov));
    settings.bind("VrHorizontalFov", menu_horizontal_fov_target_, kcd2vr::view_defaults_b2::Get("VrHorizontalFov", 95.0f));
    settings.bind("VrFpsCapEnabled", menu_fps_cap_enabled_, kcd2vr::view_defaults_b2::Get("VrFpsCapEnabled", false));
    settings.bind("VrFpsCap", menu_fps_cap_target_, kcd2vr::view_defaults_b2::Get("VrFpsCap", 90.0f));
    settings.bind("VrMergedMeshDistance", menu_merged_mesh_target_, kcd2vr::view_defaults_b2::Get("VrMergedMeshDistance", 120.0f));
#define KCD2VR_M2_HOST_SLOT 124 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=124
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    settings.bind("VrF2OwnershipFix", f2_ownership_fix_, true); // V65.10 deprecated toggle; forced ON below
#define KCD2VR_M2_HOST_SLOT 125 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=125
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 126 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=126
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 127 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=127
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 128 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=128
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 129 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=129
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 130 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=130
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 131 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=131
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    settings.bind("VrPosePredictionOffsetMs", vr_pose_prediction_offset_ms_, 0.0f);
    settings.bind("DiagnosticRendering", diagnostic_rendering_, kcd2vr::view_defaults_b2::Get("DiagnosticRendering", false));
    settings.bind("EnableVrOutputBridge", enable_vr_output_bridge_, kcd2vr::view_defaults_b2::Get("EnableVrOutputBridge", true));
    settings.bind("VrRenderScale", vr_render_scale_, kcd2vr::view_defaults_b2::Get("VrRenderScale", 1.0f));
    settings.bind("VrSourceWidth", vr_source_width_, kcd2vr::view_defaults_b2::Get("VrSourceWidth", 1920));
    settings.bind("VrSourceHeight", vr_source_height_, kcd2vr::view_defaults_b2::Get("VrSourceHeight", 1800));
    settings.bind("EnableVrBalancedGraphics", enable_vr_balanced_graphics_, kcd2vr::view_defaults_b2::Get("EnableVrBalancedGraphics", true));
    settings.bind("VrEngineClarity", vr_engine_clarity_, kcd2vr::view_defaults_b2::Get("VrEngineClarity", 0.18f));
    settings.bind("VrGamma", vr_gamma_, kcd2vr::view_defaults_b2::Get("VrGamma", 0.85f));
    settings.bind("VrBrightness", vr_brightness_, kcd2vr::view_defaults_b2::Get("VrBrightness", -0.06f));
    settings.bind("VrContrast", vr_contrast_, kcd2vr::view_defaults_b2::Get("VrContrast", 1.0f));
#define KCD2VR_M2_HOST_SLOT 132 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=132
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 133 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=133
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 134 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=134
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 135 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=135
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // R3: head aim uses explicit load/save and its one-time preference migration below.
#define KCD2VR_M2_HOST_SLOT 136 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=136
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    settings.bind("VrTurnMode", vr_turn_mode_, 1);
#define KCD2VR_M2_HOST_SLOT 137 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=137
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 138 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=138
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 139 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=139
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    settings.bind("VrPresentationMode", vr_presentation_mode_, kcd2vr::view_defaults_b2::Get("VrPresentationMode", 3));
    settings.bind("VrPresentationZoom", vr_zoom_, kcd2vr::view_defaults_b2::Get("VrPresentationZoom", kV6504DefaultZoom));
    settings.bind("VrAutoFovTrimDegrees", vr_auto_fov_trim_deg_, kcd2vr::view_defaults_b2::Get("VrAutoFovTrimDegrees", 0.0f));
    settings.bind("VrAutoCenterXTrim", vr_auto_center_x_trim_, kcd2vr::view_defaults_b2::Get("VrAutoCenterXTrim", 0.00f));
    settings.bind("VrAutoCenterYTrim", vr_auto_center_y_trim_, kcd2vr::view_defaults_b2::Get("VrAutoCenterYTrim", 0.00f));
    settings.bind("VrAutoProjectionCenter", vr_auto_projection_center_, false);
    settings.bind("VrPitchCorrectionDeg", vr_pitch_correction_deg_, 0.0f);
#define KCD2VR_M2_HOST_SLOT 140 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=140
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    settings.bind("VrSourceVerticalFovDegrees", vr_source_vertical_fov_deg_, kcd2vr::view_defaults_b2::Get("VrSourceVerticalFovDegrees", kV6504DefaultSourceFov));
    settings.bind("VrCorrectedCenterScaleFovDegrees", vr_corrected_view_vfov_deg_, kcd2vr::view_defaults_b2::Get("VrCorrectedCenterScaleFovDegrees", kV6511CorrectedCenterScaleFov));
    settings.bind("VrCullFrustumScale", vr_cull_frustum_scale_, kcd2vr::view_defaults_b2::Get("VrCullFrustumScale", 1.0f));
#define KCD2VR_M2_HOST_SLOT 141 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=141
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 142 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=142
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    settings.bind("VrFlatScreenScale", vr_flat_screen_scale_, kcd2vr::view_defaults_b2::Get("VrFlatScreenScale", 1.70f));
    settings.bind("VrPassiveFullFrameScale", vr_passive_full_frame_scale_, kcd2vr::view_defaults_b2::Get("VrPassiveFullFrameScale", 1.20f));
    settings.bind("VrPauseOverlaySize", vr_pause_overlay_size_, 0.98f); // R59.16C10R14: 0.65 was tuned before C10R2 scaled the button containers ("menu too small, 50 % bigger")
    settings.bind("VrTransientZoomRetain", vr_transient_zoom_retain_, 0.00f);
    settings.bind("VrDepthStereoStrength", vr_depth_stereo_strength_, kcd2vr::view_defaults_b2::Get("VrDepthStereoStrength", 0.30f));
#define KCD2VR_M2_HOST_SLOT 143 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=143
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    settings.bind("MirrorMisMenuToDesktop", mirror_mis_menu_to_desktop_, false);
    settings.bind("VrLeftOffsetX", vr_left_offset_x_, kcd2vr::view_defaults_b2::Get("VrLeftOffsetX", 0.0f));
    settings.bind("VrLeftOffsetY", vr_left_offset_y_, kcd2vr::view_defaults_b2::Get("VrLeftOffsetY", 0.0f));
    settings.bind("VrRightOffsetX", vr_right_offset_x_, kcd2vr::view_defaults_b2::Get("VrRightOffsetX", 0.0f));
    settings.bind("VrRightOffsetY", vr_right_offset_y_, kcd2vr::view_defaults_b2::Get("VrRightOffsetY", 0.0f));
    settings.bind("VrStereoExperiment", vr_production_stereo_mode_, kcd2vr::view_defaults_b2::Get("VrStereoExperiment", 3));
    settings.bind("VrCleanPresentationMode", vr_cyclopean_mode_, kcd2vr::view_defaults_b2::Get("VrCleanPresentationMode", 0)); // V66.69: 0 production mono/stereo source mapping; 1 internal projective remap; pseudostereo retired
    settings.bind("VrRotationOrder", vr_rotation_order_, 0);
    settings.bind("VrAfrIpdScale", vr_afr_ipd_scale_, 0.0f);
    settings.bind("VrSwapEyes", vr_swap_eyes_, false);
    settings.bind("EnableLegacyStereoProbes", enable_legacy_probes_, false);
    settings.bind("EnableNativeEyeWrite", enable_native_eye_write_, false);


    auto future = ini.section("FutureFeatures");
#define KCD2VR_M2_HOST_SLOT 144 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=144
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 145 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=145
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 146 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=146
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    future.bind("EnableAlchemyVR", enable_alchemy_vr_, false);
    future.bind("EnableSmithingVR", enable_smithing_vr_, false);
    future.bind("DisableTargetLockSteering", disable_target_lock_steering_, false);
#define KCD2VR_M2_HOST_SLOT 147 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=147
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR

    auto experimental = ini.section("Experimental");
#define KCD2VR_M2_HOST_SLOT 148 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=148
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR

    const char* explicit_ini = configPathD62.c_str();
    // R107: undo the Codex R105 "minimal HUD" rewrite of the INI (marker VrMinimalHudDefaultsR105=1): it set
    // VrShowInteractionHints=false, so the contextual "A grab / open" hints were gone (wh_ui_ShowHints lease
    // writes 0 from that key). Once: hints back ON; the other R105 keys are ignored by this build anyway.
    // Done only when the R105 marker is there and this undo marker is not, so a later choice stays saved.
    if(ReadIniIntExplicit(explicit_ini,"Settings","VrMinimalHudDefaultsR105",0)>=1&&
       ReadIniIntExplicit(explicit_ini,"Settings","VrHudDefaultsRestoredR107",0)<1){
        const bool ok=WritePrivateProfileStringA("Settings","VrShowInteractionHints","true",explicit_ini)!=FALSE;
        if(ok)(void)WritePrivateProfileStringA("Settings","VrHudDefaultsRestoredR107","1",explicit_ini);
        diagnostic_trace::WriteStatus(ok?"R107 HUD_DEFAULTS R105 minimal-HUD rewrite undone: VrShowInteractionHints=true (hints back)":
            "R107 HUD_DEFAULTS could not write VrShowInteractionHints=true - set it in the INI by hand");
    }
    // D81: apply the requested repairs once to the existing profile too.
    // Keep every other setting and all calibration sections exactly as saved.
    if(ReadIniIntExplicit(explicit_ini,"Settings","VrRepairDefaultsD81",0)<1){
        const struct {const char* key;const char* value;} changes[]{
            {"VrMinigamePromptScale","0.45"},{"VrSmithSemiAnchorsD67","true"},
            {"VrLivelyNpcD73","true"},{"VrNpcListeningGesturesD76","true"}};
        bool saved=true;
        for(const auto& change:changes)
            saved=(WritePrivateProfileStringA("Settings",change.key,change.value,explicit_ini)!=FALSE)&&saved;
        if(saved)(void)WritePrivateProfileStringA("Settings","VrRepairDefaultsD81","1",explicit_ini);
    }
    // D85R5R2 correction: migrate only the expressly requested station size.
    // An exit-saved INI cannot establish earlier live roomscale state. Preserve
    // all movement choices instead of presenting a forced option as a repair.
    // Keep the existing marker so an R1 user's later prompt choice survives.
    const bool applyRepairD85R5R1=ReadIniIntExplicit(explicit_ini,"Settings","VrRepairDefaultsD85R5R1",0)<1;
    if(applyRepairD85R5R1){
        const struct {const char* key;const char* value;} requested[]{
            {"VrMinigamePromptScale","0.45"}};
        bool saved=true;
        for(const auto& change:requested)
            saved=(WritePrivateProfileStringA("Settings",change.key,change.value,explicit_ini)!=FALSE)&&saved;
        if(saved)(void)WritePrivateProfileStringA("Settings","VrRepairDefaultsD85R5R1","1",explicit_ini);
    }
    // Launcher selection is startup-only. Reloading settings cannot arm diagnostics.
    developer_mode::ConfigureStartup(ReadIniBoolExplicit(explicit_ini,"Settings","VrDeveloperMode",false));
    // D72 adopts only the requested candidate controls once, preserving the
    // user's calibration, graphics and unrelated setting values.
    if(ReadIniIntExplicit(explicit_ini,"Settings","VrCandidateDefaultsD72",0)==0){
        // The shared INI lease already retains the one recovery checkpoint.
        {
            bool okD72=true;
            for(const auto& settingD72:defaults_d72::settings)
                okD72=(WritePrivateProfileStringA("Settings",settingD72.key,settingD72.value,explicit_ini)!=FALSE)&&okD72;
            if(okD72)(void)WritePrivateProfileStringA("Settings","VrCandidateDefaultsD72","1",explicit_ini);
        }
    }
    thrust_tuning_d67::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrIndependentThrustD67",true));
    const int storedMotionR92=ReadIniIntExplicit(explicit_ini,"Settings","VrMotionTrialD67",4) /* R124: missing = 4 FRESH POSE UNCAPPED (Miki's INI) */;
    motion_trials_d67::SetMenuMode(motion_trials_d67::MenuModeFromIni(storedMotionR92));
    motion_trials_d67::SetElbowPreferenceR92(ReadIniBoolExplicit(explicit_ini,"Settings","VrElbowPreferenceD67",storedMotionR92==3));
    for(int c=0;c<17;++c){auto v=thrust_tuning_d67::defaults(c);char key[80]{};std::snprintf(key,sizeof(key),"VrThrustSpeedD67_%d",c);v.speed=ReadIniFloatExplicit(explicit_ini,"Settings",key,v.speed);std::snprintf(key,sizeof(key),"VrThrustTravelD67_%d",c);v.travel=ReadIniFloatExplicit(explicit_ini,"Settings",key,v.travel);thrust_tuning_d67::set(c,v);}
    for(unsigned c=0;c<17;++c){auto v=melee_tuning_c54::read(int(c));char key[64]{};std::snprintf(key,sizeof(key),"VrMeleeTypeC54_%u_Cut",c);v.cut=ReadIniFloatExplicit(explicit_ini,"Settings",key,1.f);std::snprintf(key,sizeof(key),"VrMeleeTypeC54_%u_Stab",c);v.stab=ReadIniFloatExplicit(explicit_ini,"Settings",key,1.f);std::snprintf(key,sizeof(key),"VrMeleeTypeH29_%u_Intent",c);v.intentH29=ReadIniFloatExplicit(explicit_ini,"Settings",key,1.f);std::snprintf(key,sizeof(key),"VrMeleeTypeH29_%u_Sensitivity",c);v.sensitivityH29=ReadIniFloatExplicit(explicit_ini,"Settings",key,1.f);std::snprintf(key,sizeof(key),"VrMeleeTypeR8_%u_MinTravel",c);v.travelR8=ReadIniFloatExplicit(explicit_ini,"Settings",key,1.f);melee_tuning_c54::set(int(c),v);}
    melee_tuning_c54::bladeAxisCosineH41.store(melee_tuning_c54::bladeAxisSettingH41(ReadIniFloatExplicit(explicit_ini,"Settings","VrBladeAxisAlignmentH41",.80f)));
    punch_travel_d65::minimumMeters.store(punch_travel_d65::setting(ReadIniFloatExplicit(explicit_ini,"Settings","VrPunchMinimumTravelMetersD65",.10f)));
    melee_tuning_c54::punchSensitivity.store(melee_tuning_c54::bound(ReadIniFloatExplicit(explicit_ini,"Settings","VrPunchSensitivityC54",1.f),.5f,2.f,1.f));
    melee_tuning_c54::blockSensitivityH29.store(melee_tuning_c54::bound(ReadIniFloatExplicit(explicit_ini,"Settings","VrFistBlockSensitivityH29",1.25f),.5f,1.5f,1.f));
    for(int h=0;h<2;++h)for(int f=0;f<5;++f){char key[64]{};std::snprintf(key,sizeof(key),"VrFingerMiddleLateralC54_%c_%d",h?'L':'R',f);finger40::setMiddleLateralC54(h,f,ReadIniFloatExplicit(explicit_ini,"Settings",key,0.f));}
    for(int h=0;h<2;++h){char key[80]{};std::snprintf(key,sizeof(key),"VrFingerMiddleBaseShiftMmH31_%c",h?'L':'R');finger40::setShiftMmH31(h,ReadIniFloatExplicit(explicit_ini,"Settings",key,15.f));std::snprintf(key,sizeof(key),"VrFingerMiddleDistalMedialDegH31_%c",h?'L':'R');finger40::setDistalMedialDegH31(h,ReadIniFloatExplicit(explicit_ini,"Settings",key,10.f));} // H31_FINGER_LOAD


    // V66.111: preserve user calibration but migrate only the two exact inherited defaults
    // falsified by the V66.109 headset run. build_release.bat intentionally preserves the
    // runtime INI, so changing C++ defaults alone cannot repair an existing installation.
    bool forceHeight111=false,forceSemanticHud111=false;
    {
        g_dialogue_stable5915.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDialogueStableCamera",false));
        g_alchemy_finish_observer_cb1.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrAlchemyFinishObserver",false)); // CB1
        kcd2vr::grip_stow_cb1::SetEnabled(ReadIniBoolExplicit(explicit_ini,"Settings","VrGripStow",false)); // CB1: face button while holding a prop = stow (AddItem)
        kcd2vr::grip_stow_cb1::SetButton(ReadIniIntExplicit(explicit_ini,"Settings","VrGripStowButton",0)); // CB1: 0 A/Y, 1 B/X, 2 stick click
        g_inventory_early_exit25.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrInventoryEarlyExit",true)); // C25
        CameraHook::SetDialogueDefaultEye59c13(ReadIniBoolExplicit(explicit_ini,"Settings","VrDialogueDefaultEye",true)); // R59.16C10R13
        combat_diag_probe::SetSwingStamina5916(ReadIniBoolExplicit(explicit_ini,"Settings","VrSwingStamina",true));
        kcd2vr::prop5916c1::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrPropPhysicsDrive",true));
        const int migrated=ReadIniIntExplicit(explicit_ini,"Settings","V66111DefaultsMigrated",0);
        if(!migrated){
            const float oldHeight=ReadIniFloatExplicit(explicit_ini,"Settings","VrCameraHeightOffsetMeters",kcd2vr::view_defaults_b2::Get("VrCameraHeightOffsetMeters", 0.035f));
            const bool oldCanvas=ReadIniBoolExplicit(explicit_ini,"Settings","VrHudNativeCanvasFit",false);
            const float oldSize=ReadIniFloatExplicit(explicit_ini,"Settings","VrHudNativeCanvasSize",1.00f);
            const float oldSx=ReadIniFloatExplicit(explicit_ini,"Settings","VrHudNativeCanvasSpreadX",0.78f);
            const float oldSy=ReadIniFloatExplicit(explicit_ini,"Settings","VrHudNativeCanvasSpreadY",0.78f);
            const bool moveHeight=defaults_migration111::LegacyCameraHeight(oldHeight);
            const bool retireCanvas=defaults_migration111::LegacyMergedCanvas(oldCanvas,oldSize,oldSx,oldSy);
            forceHeight111=moveHeight;forceSemanticHud111=retireCanvas;
            bool writesOk=true;
            if(moveHeight)writesOk=WritePrivateProfileStringA("Settings","VrCameraHeightOffsetMeters","0.035",explicit_ini)!=FALSE&&writesOk;
            if(retireCanvas)writesOk=WritePrivateProfileStringA("Settings","VrHudNativeCanvasFit","false",explicit_ini)!=FALSE&&writesOk;
            if(writesOk)WritePrivateProfileStringA("Settings","V66111DefaultsMigrated","1",explicit_ini);
            char m[320]{};std::snprintf(m,sizeof(m),"V66.111-CONFIG-MIGRATION legacyHeight=%d legacyMergedCanvas=%d write=%s calibration=UNCHANGED",moveHeight?1:0,retireCanvas?1:0,writesOk?"OK":"FAILED_RUNTIME_FALLBACK");diagnostic_trace::Write(m);
            // The force flags below also apply this migration for the current process if
            // the game directory is read-only and the profile write cannot persist.
        }
    }

    // V66.08 is a gated stereo-design source branch: always retain the startup GPU census
    // and the F11 preflight log, even when a previously saved INI has LogLevel=OFF.
    const bool diagnostics_enabled = true;
    diagnostic_trace::SetEnabled(diagnostics_enabled);
    if(core::default_ini_r118::lastResult==int(core::default_ini_r118::Result::Created))
        diagnostic_trace::WriteStatus("R118 DEFAULT_INI created: no KCD2VR.ini was beside the mod, the built-in default was written (an existing file is never replaced)");
    else if(core::default_ini_r118::lastResult==int(core::default_ini_r118::Result::Failed)||core::default_ini_r118::lastResult==int(core::default_ini_r118::Result::NoDefault))
        diagnostic_trace::WriteStatus(core::default_ini_r118::lastResult==int(core::default_ini_r118::Result::Failed)?
            "R118 DEFAULT_INI not written: the game folder is not writable":"R118 DEFAULT_INI not written: this DLL has no built-in default");
#ifdef KCD2VR_PROXY_DINPUT8
    proxy29::FlushStartupRecords32(); // after successful logger admission, never a plugin rescan
#endif
    KCD2VR_DIAG_LOG(Bug,info,"D81 diagnostics {} -> KCD2VR.log (sparse/state-change output)", diagnostics_enabled ? "ENABLED" : "DISABLED");

    // V40.2: DetourModKit config can report Loaded 0 items for this host build.
    // Explicitly parse every active VR setting through Win32 profile APIs so the
    // ASI-side KCD2VR.ini is authoritative and runtime logs show the real values.
    enabled_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "Enabled", true), std::memory_order_relaxed);
    head_pose_scale_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "HeadPoseScale", kcd2vr::view_defaults_b2::Get("HeadPoseScale", 1.0f)), std::memory_order_relaxed);
    camera_side_offset_m_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrCameraSideOffsetMeters", 0.0f), -0.30f, 0.30f), std::memory_order_relaxed);
    camera_back_offset_m_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrCameraBackOffsetMeters", kcd2vr::view_defaults_b2::Get("VrCameraBackOffsetMeters", 0.0f)), -0.30f, 0.30f), std::memory_order_relaxed);
    camera_height_offset_m_.store(forceHeight111?0.035f:std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrCameraHeightOffsetMeters", kcd2vr::view_defaults_b2::Get("VrCameraHeightOffsetMeters", 0.035f)), -0.30f, 0.30f), std::memory_order_relaxed);
    apply_head_rotation_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "ApplyHeadRotation", kcd2vr::view_defaults_b2::Get("ApplyHeadRotation", true)), std::memory_order_relaxed);
    apply_head_position_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "ApplyHeadPosition", kcd2vr::view_defaults_b2::Get("ApplyHeadPosition", true)), std::memory_order_relaxed);
    vr_pose_source_mode_.store(std::clamp(ReadIniIntExplicit(explicit_ini, "Settings", "VrPoseSourceMode", kcd2vr::view_defaults_b2::Get("VrPoseSourceMode", 2)),0,3), std::memory_order_relaxed);
    tracking59c6::Configure(ReadIniBoolExplicit(explicit_ini,"Settings","VrSynchronousHands",true));
#define KCD2VR_M2_HOST_SLOT 149 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=149
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // R22C2: user selected the old/manual HUD. Ignore retired owner flags without
    // changing saved placement, visibility preferences or calibration on disk.
#define KCD2VR_M2_HOST_SLOT 150 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=150
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // C26C1: do not migrate/reset an explicitly saved head-pose preference.
    // Missing settings use the accepted fresh SYNC_NOW default above. Cached
    // sampling remains an explicit user choice, not a new startup rewrite.
#define KCD2VR_M2_HOST_SLOT 151 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=151
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    if(ReadIniIntExplicit(explicit_ini,"Settings","VrHudLayoutVersion",0)<1202){
        const bool top=WritePrivateProfileStringA("Settings","VrUiTopBoundary","1.10",explicit_ini)!=FALSE;
        const bool bottom=WritePrivateProfileStringA("Settings","VrUiBottomBoundary","1.80",explicit_ini)!=FALSE;
        const bool marked=top&&bottom&&WritePrivateProfileStringA("Settings","VrHudLayoutVersion","1202",explicit_ini)!=FALSE;
        diagnostic_trace::Write(marked?"V66.120R2-HUD expanded border defaults upper=110% lower=180%; size/calibration retained":"V66.120R2-HUD border preference save failed");
    }
#define KCD2VR_M2_HOST_SLOT 152 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=152
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    g_ragdoll_scale59.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrRagdollImpactScale",3.f),1.f,10.f)); // R59 F9
#define KCD2VR_M2_HOST_SLOT 153 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=153
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 154 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=154
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 155 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=155
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 156 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=156
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    frik_pose_probe::SetTouchReact43(g_touch43.load(),g_touch_mass43.load()); // C43T
    {void* ss47=nullptr;const auto p47=c47lua::probe(ss47);c47lua::receipt("STARTUP",p47); // C47: one-time startup receipt of the Lua route pins
     // C47: the console '#' route is dev-mode gated (see c47lua); the direct ExecuteBuffer route replaces it when its pins hold
     frik_pose_probe::SetLuaExecutor43(p47.build&&p47.sysGetScript&&p47.scriptExec?&ExecuteLuaDirect47:nullptr);}
    g_phys_bodies59c10r8.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrPhysBodies",0),0,2)); // R59.16C10R8
#define KCD2VR_M2_HOST_SLOT 157 // M2_SOURCE_SELECTOR
#include "subsystems/hybrid/host.inl" // M2_OWNED_SOURCE slot=157
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // R38: a saved VrHeldMap preference remains authoritative after the DS7 key
    // rename. An explicitly saved DS7 choice still overrides the legacy value.
    const bool heldMapLegacyR38=ReadIniBoolExplicit(explicit_ini,"Settings","VrHeldMap",true);
    g_held_map59c10r10.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrHeldParchmentMapDs7",heldMapLegacyR38));
    diagnostic_trace::Write(g_held_map59c10r10.load()?
        "R38 MAP_SETTING HELD_PARCHMENT_ENABLED chord=RIGHT_THUMBREST_LEFT_STICK_UP":
        "R38 MAP_SETTING NATIVE_MENU_SELECTED explicitSetting=OFF");
    const int printedMapRegion32=ReadIniIntExplicit(explicit_ini,"Settings","VrHeldMapRegion",0);
    held_map_m1::SetPrintedRegion(printedMapRegion32==1?held_map_m1::PrintedRegion::Kuttenberg:held_map_m1::PrintedRegion::Trosky);
#define KCD2VR_M2_HOST_SLOT 158 // M2_SOURCE_SELECTOR
#include "subsystems/experimental/host.inl" // M2_OWNED_SOURCE slot=158
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 159 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=159
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    frik_pose_probe::SetNpcProximityRadius36(ReadIniFloatExplicit(explicit_ini,"Settings","VrNpcProximityRadius36",.4f));
    action_time_owner59c11::SetPropPresentation35(
        ReadIniFloatExplicit(explicit_ini,"Settings","VrHeldPropClearanceMeters35",.02f), // C34: held items clipped through the palm -> outward clearance; R124 default 2 cm (Miki's INI)
        ReadIniFloatExplicit(explicit_ini,"Settings","VrPropStowRadiusMeters35",.28f)); // C34: chest stow zone; R124 default 28 cm (Miki's INI)
#define KCD2VR_M2_HOST_SLOT 160 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=160
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    if(g_held_map59c10r10.load()){const bool pins=held_map_m1::Install();held_map_m1::Enable(pins);
        diagnostic_trace::Write(pins?"C32 HELD_MAP selected: merged current companion prints retail map artwork; open = right thumbrest + left stick up 1 s, close = B":
            "C10R10 HELD_MAP disabled: retail pins did not match (no placement calls)");}
#define KCD2VR_M2_HOST_SLOT 161 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=161
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    ApplyLegacyPhysBodiesT4();
    auto_hud_icon_scale_.store(std::isfinite(iconScale51c3)?std::clamp(iconScale51c3,.5f,6.f):1.75f);
#define KCD2VR_M2_HOST_SLOT 162 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=162
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    f2_ownership_fix_.store(true, std::memory_order_relaxed); // V65.10 always ON; ignore stale INI false
    // V66.146: do not force the old head-ray assist ON; persisted/default setting owns it.
#define KCD2VR_M2_HOST_SLOT 163 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=163
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 164 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=164
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // R30C1: dormant AutoHUD boundaries are not the user's manual placement.
    // New keys fall back to the effective old manual height; keep saved offsets.
    (void)0; // C37C3: manualHeight30c1 fallback replaced by the fixed defaults below
    vr_ui_top_boundary_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrManualHudUpperBoundary",1.17f) /*C37C3 was manualHeight30c1*/,.55f,3.f));
    vr_ui_bottom_boundary_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrManualHudLowerBoundary",1.63f) /*C37C3 was manualHeight30c1*/,.55f,3.f));
#define KCD2VR_M2_HOST_SLOT 165 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=165
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 166 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=166
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 167 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=167
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 168 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=168
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 169 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=169
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    vr_pose_prediction_offset_ms_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrPosePredictionOffsetMs", 0.0f),-50.0f,50.0f), std::memory_order_relaxed);
    menu_game_fov_target_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrGameFov", kcd2vr::view_defaults_b2::Get("VrGameFov", kV6504DefaultGameFov)),70.0f,130.0f), std::memory_order_relaxed);
    menu_horizontal_fov_target_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrHorizontalFov", kcd2vr::view_defaults_b2::Get("VrHorizontalFov", 95.0f)),70.0f,170.0f), std::memory_order_relaxed);
    menu_fps_cap_enabled_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "VrFpsCapEnabled", kcd2vr::view_defaults_b2::Get("VrFpsCapEnabled", false)), std::memory_order_relaxed);
    menu_fps_cap_target_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrFpsCap", kcd2vr::view_defaults_b2::Get("VrFpsCap", 90.0f)),60.0f,120.0f), std::memory_order_relaxed);
    menu_merged_mesh_target_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrMergedMeshDistance", kcd2vr::view_defaults_b2::Get("VrMergedMeshDistance", 120.0f)),30.0f,300.0f), std::memory_order_relaxed);
    diagnostic_rendering_.store(developer_mode::Enabled()&&ReadIniBoolExplicit(explicit_ini, "Settings", "DiagnosticRendering", kcd2vr::view_defaults_b2::Get("DiagnosticRendering", false)), std::memory_order_relaxed);
    enable_vr_output_bridge_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableVrOutputBridge", kcd2vr::view_defaults_b2::Get("EnableVrOutputBridge", true)), std::memory_order_relaxed);
    vr_render_scale_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrRenderScale", kcd2vr::view_defaults_b2::Get("VrRenderScale", 1.0f)), std::memory_order_relaxed);
    vr_source_width_.store(std::clamp(ReadIniIntExplicit(explicit_ini, "Settings", "VrSourceWidth", kcd2vr::view_defaults_b2::Get("VrSourceWidth", 1920)), 640, 4096), std::memory_order_relaxed);
    vr_source_height_.store(std::clamp(ReadIniIntExplicit(explicit_ini, "Settings", "VrSourceHeight", kcd2vr::view_defaults_b2::Get("VrSourceHeight", 1800)), 640, 4096), std::memory_order_relaxed);
    enable_vr_balanced_graphics_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableVrBalancedGraphics", kcd2vr::view_defaults_b2::Get("EnableVrBalancedGraphics", true)), std::memory_order_relaxed);
    vr_engine_clarity_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrEngineClarity", kcd2vr::view_defaults_b2::Get("VrEngineClarity", 0.18f)), std::memory_order_relaxed);
    vr_gamma_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrGamma",kcd2vr::view_defaults_b2::Get("VrGamma", 0.85f)),0.50f,2.00f),std::memory_order_relaxed);
    vr_brightness_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrBrightness",kcd2vr::view_defaults_b2::Get("VrBrightness", -0.06f)),-0.50f,0.50f),std::memory_order_relaxed);
    vr_contrast_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrContrast",kcd2vr::view_defaults_b2::Get("VrContrast", 1.0f)),0.50f,2.00f),std::memory_order_relaxed);
#define KCD2VR_M2_HOST_SLOT 170 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=170
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 171 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=171
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 172 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=172
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 173 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=173
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 174 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=174
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // Preserve calibration and other preferences; retire only this camera writer.
    WritePrivateProfileStringA("Settings","EnableNativeHeadAim","false",explicit_ini);
    WritePrivateProfileStringA("Settings","VrHeadAimPreferenceVersion","4",explicit_ini);
#define KCD2VR_M2_HOST_SLOT 175 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=175
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    vr_turn_mode_.store(std::clamp(ReadIniIntExplicit(explicit_ini, "Settings", "VrTurnMode", 1),0,1),std::memory_order_relaxed);
#define KCD2VR_M2_HOST_SLOT 176 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=176
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // V66.128: 6DOF body follow is user-toggled (F1 / VR menu CONTROLS) and persisted;
    // chest twist and bow string follow are INI-only switches (default ON).
#define KCD2VR_M2_HOST_SLOT 177 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=177
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    g_roomscale_follow58.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrRoomscaleFollow",false)); // V66.178R58: acts only while 6DOF BODY is on
    g_roomscale_dodge58.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrRoomscaleDodge",true));
    g_smooth_move51.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrSmoothMovement",false));
    g_hmd_walking_direction134.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrHmdWalkingDirectionR134",true));
    subsystems::alchemy::reach_craft1::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrCraftProximityTrialH41",false));
    frik_pose_probe::SetChestPitchShareCB(ReadIniFloatExplicit(explicit_ini,"Settings","VrChestPitchShareH41Trial",0.f));
    frik_pose_probe::SetArcherStanceCB(ReadIniBoolExplicit(explicit_ini,"Settings","VrArcherStanceH41Trial",false));
    melee_tuning_c54::bypassExtraAttackVetoesH41.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDisableExtraModAttackVetoesH41",false));
    frik_pose_probe::SetSmoothBody51(g_smooth_move51.load(std::memory_order_acquire));
    // H4: Miki explicitly requested physical crouch enabled for this recovery.
    // Adopt that choice once; subsequent changes through the menu remain saved.
    frik_pose_probe::SetNativeCrouchR93(ReadIniBoolExplicit(explicit_ini,"Settings","VrNativeCrouchR93",false));
    const bool crouchChoiceH4=ReadIniIntExplicit(explicit_ini,"Settings","VrPhysicalCrouchH4Adopted",0)>=1;
    if(!crouchChoiceH4){
        const bool savedH4=WritePrivateProfileStringA("Settings","VrHmdBodyHeight","true",explicit_ini)!=FALSE;
        if(savedH4)(void)WritePrivateProfileStringA("Settings","VrPhysicalCrouchH4Adopted","1",explicit_ini);
        diagnostic_trace::Write("H4 PHYSICAL_CROUCH requestedDefault=ON standingEye=REST_SKELETON HMD=AUTHORITATIVE laterMenuChoice=PRESERVED");
    }
    // R98 body height: 0 HMD HEIGHT, 1 NATIVE HEIGHT, 2 HMD + NATIVE (RIDE/CROUCH).
    // The new key wins. Without it (first R98 start) the old bool had no
    // combined choice, so the requested combined mode is used; the old value
    // only goes to the log line.
    {
        int bodyHeightR98=ReadIniIntExplicit(explicit_ini,"Settings","VrBodyHeightModeR98",-1);
        if(bodyHeightR98<0||bodyHeightR98>2){
            const bool oldHmdR98=ReadIniBoolExplicit(explicit_ini,"Settings","VrHmdBodyHeight",true);
            bodyHeightR98=2;
            char lineR98[200]{};std::snprintf(lineR98,sizeof(lineR98),"R98 BODY_HEIGHT key VrBodyHeightModeR98 missing; old VrHmdBodyHeight=%s -> mode=2 HMD+NATIVE (ride/crouch)",oldHmdR98?"true":"false");
            diagnostic_trace::Write(lineR98);
        }
        frik_pose_probe::SetBodyHeightModeR98(bodyHeightR98);
    }
    frik_pose_probe::SetChestDagger54(ReadIniBoolExplicit(explicit_ini,"Settings","VrChestDagger",true));

#define KCD2VR_CONFIG_LOAD_THROW_DROP0
#include "subsystems/throw_drop/config.inl"
#undef KCD2VR_CONFIG_LOAD_THROW_DROP0


#define KCD2VR_CONFIG_LOAD_THROW_DROP1
#include "subsystems/throw_drop/config.inl"
#undef KCD2VR_CONFIG_LOAD_THROW_DROP1

    g_console_execute57.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrConsoleCommandsExecute",true)); // C57
    g_comfort_vignette51.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrComfortVignette",0),0,3));
    g_gesture_talk59c5.store(kcd2vr::gesture_talk_policy_d33::loadTalk(ReadIniIntExplicit(explicit_ini,"Settings","VrGestureTalkVersion",0),
        ReadIniBoolExplicit(explicit_ini,"Settings","VrGestureTalk",true))); // R124: pointing ON by default (Miki's INI); a saved OFF is kept; X stays
    frik_pose_probe::SetFistKickEnabledH35(ReadIniBoolExplicit(explicit_ini,"Settings","VrKickFistLane",false)); // CB1: unarmed R3 kick through the fist lane (1 = on)
    kcd2vr::book_pages45::mode.store(kcd2vr::book_pages45::clampMode(ReadIniIntExplicit(explicit_ini,"Settings","VrReadingPages",0))); // C45
    kcd2vr::quick_access_r112::SetFaceTouch(ReadIniBoolExplicit(explicit_ini,"Settings","VrQuickAccessFaceTouchR112",false)); // R113: VR CONFIG > CONTROLS
    diagnostic_trace::Write(g_gesture_talk59c5.load()?
        "KCD2VR BUILD GESTURE_CAPABILITY talkSetting=ON route=CONTEXTUAL_NATIVE acceptedAction=UNPROVEN combatTaunt=NOT_EQUIVALENT":
        "KCD2VR BUILD GESTURE_CAPABILITY talkSetting=OFF reason=SAVED_SETTING combatTaunt=NOT_EQUIVALENT");
#define KCD2VR_M2_HOST_SLOT 178 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=178
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    g_decapitation_min_energy59c7e=std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrDecapitationMinEnergy",0.6f),0.05f,1.f);
#define KCD2VR_M2_HOST_SLOT 179 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=179
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    kcd2vr::gore32::decap.store(g_decapitation59c7e.load()); // C32: the same switch now really takes the head off
    // R124: missing key = ON (Miki's INI); an explicit saved OFF stays OFF.
    const bool dismemberD56=ReadIniBoolExplicit(explicit_ini,"Settings","VrDismemberment",true);
    prompt_policy_d80::showHints.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrShowInteractionHints",true));
    prompt_policy_d80::timed.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrTimedInteractionPrompts",true));
    // The previous release doubled station labels to 90%. Restore the requested
    // half-size once; preserve other tuned sizes and all later menu choices.
    const float savedPromptScaleR92R1=ReadIniFloatExplicit(explicit_ini,"Settings","VrMinigamePromptScale",.45f);
    const bool firstPromptSizeR92R1=ReadIniIntExplicit(explicit_ini,"Settings","VrPromptSizeR92R1",0)<1;
    const bool repairPromptSizeR92R1=firstPromptSizeR92R1 &&
        std::isfinite(savedPromptScaleR92R1) && std::fabs(savedPromptScaleR92R1-.90f)<.001f;
    if(firstPromptSizeR92R1){
        const bool saved=!repairPromptSizeR92R1 || WritePrivateProfileStringA(
            "Settings","VrMinigamePromptScale","0.45",explicit_ini)!=FALSE;
        if(saved)(void)WritePrivateProfileStringA("Settings","VrPromptSizeR92R1","1",explicit_ini);
    }
    const float promptScaleD80=repairPromptSizeR92R1?.45f:savedPromptScaleR92R1;
    prompt_policy_d80::minigameScale.store(std::isfinite(promptScaleD80)?std::clamp(promptScaleD80,.25f,1.f):.45f);
    kcd2vr::gorepak50::dismember.store(dismemberD56);
#define KCD2VR_M2_HOST_SLOT 180 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=180
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    kcd2vr::gore32::decapCorpseMinSpeed.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrDecapitationCorpseMinSpeed",2.5f),0.5f,12.f)); // C41
#define KCD2VR_M2_HOST_SLOT 181 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=181
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    kcd2vr::gore32::decapHeadImpulse44.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrDecapitationHeadImpulse",20.f),0.f,200.f));
#define KCD2VR_M2_HOST_SLOT 182 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=182
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    kcd2vr::gore32::hangMassKg.store(float(kcd2vr::gore32::clampHangMass44(ReadIniFloatExplicit(explicit_ini,"Settings","VrImpaleHangMassKg",40.f)))); // C44
    kcd2vr::contact58c1::impaleKillEnergy44.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrImpaleKillEnergy",.3f),0.f,1.f)); // C44
    kcd2vr::impale47::entityRoot.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrImpaleEntityRoot",true)); // I10: impale root type 2
    kcd2vr::impale_corpse_r15::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrImpaleCorpse",true)); // I10/N2: stab into a game-killed corpse impales (hang mode)
    {namespace i40=kcd2vr::impale40; // C40 IMPALE BY ARMOUR (default OFF)
     i40::mode.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrImpaleByArmour",0),0,2));
     const float lo=std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrImpaleLightArmour",15.f),0.f,500.f);
     i40::lightArmour.store(lo);i40::hardArmour.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrImpaleHardArmour",60.f),lo+1.f,1000.f));
     i40::minEnergy.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrImpaleMinEnergy",.30f),0.f,1.f));
     i40::throughEnergy.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrImpaleThroughEnergy",.75f),0.f,1.5f));
     i40::partialCm.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrImpalePartialCm",30.f),5.f,60.f));
     i40::throughCm.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrImpaleThroughCm",55.f),20.f,90.f));}
#define KCD2VR_M2_HOST_SLOT 183 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=183
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // R59.16C8: strike power, riposte window, auto palm, target dwell (all menu/INI; see KCD2VR.ini)
    g_immersive_fov59c7g.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrImmersiveFov",kcd2vr::view_defaults_b2::Get("VrImmersiveFov", false))); // R59.16C9A
    g_activity_lens59c10r1.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrActivityFovLock",kcd2vr::view_defaults_b2::Get("VrActivityFovLock", true)));
    combat_diag_probe::SetHitPairFlag59c9(ReadIniBoolExplicit(explicit_ini,"Settings","VrHitPairFlag",false)); // default off since Codex x review // R59.16C9C
#define KCD2VR_M2_HOST_SLOT 184 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=184
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    g_auto_palm59c8.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrAutoPalmAlign",true));frik_pose_probe::SetAutoPalm59c8(g_auto_palm59c8.load());
    g_riposte_native59c8.store(false); // R59.16C8 final: VrRiposteNative retired (no native attack press)
#define KCD2VR_M2_HOST_SLOT 185 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=185
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    {auto& e=g_exp59c10; // R59.16C10d: shadows for the EXPERIMENTS page
     e.strikePower.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrStrikePower",true));e.strikeTwoHanded.store(ReadIniFloatExplicit(explicit_ini,"Settings","VrStrikeTwoHandedBonus",1.25f));
     e.strikeMinPower.store(ReadIniFloatExplicit(explicit_ini,"Settings","VrStrikeMinPower",0.30f));e.strikeMaxPower.store(ReadIniFloatExplicit(explicit_ini,"Settings","VrStrikeMaxPower",1.10f));
     e.strikeRepeatMs.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrStrikeRepeatMs",2000),200,10000));e.riposteWindowMs.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrRiposteWindowMs",700),100,3000));
     combat_diag_probe::SetStrikePower59c8(e.strikePower.load(),e.strikeTwoHanded.load(),e.strikeMinPower.load(),e.strikeMaxPower.load(),unsigned(e.strikeRepeatMs.load()),g_riposte_native59c8.load(),unsigned(e.riposteWindowMs.load()));}
    native_vr_pad_premap::SetXQuickTalk58(ReadIniBoolExplicit(explicit_ini,"Settings","VrXQuickTalk",true)); // V66.178R58: companion pak controls
    native_vr_pad_premap::SetBQuickTalk45(ReadIniBoolExplicit(explicit_ini,"Settings","VrBQuickTalk",false)); // C45: B is not a second talk button
#define KCD2VR_M2_HOST_SLOT 186 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=186
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 187 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=187
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    frik_pose_probe::SetWeaponConstraints134(ReadIniBoolExplicit(explicit_ini,"Settings","WeaponConstraintDrive",true),ReadIniBoolExplicit(explicit_ini,"Settings","WeaponTwoHandConstraint",true));
    // One-time migration of the retired zone-only default to the user's explicit
    // release-to-holster preference. The shared last-good checkpoint protects this write.
    const bool migrateHolster146=ReadIniIntExplicit(explicit_ini,"Settings","VrGripReleasePreferenceVersion",0)<1;
    if(migrateHolster146){
        const bool saved=WritePrivateProfileStringA("Settings","HolsterOnGripRelease","1",explicit_ini)!=FALSE;
        if(saved)WritePrivateProfileStringA("Settings","VrGripReleasePreferenceVersion","1",explicit_ini);
        diagnostic_trace::Write(saved?"V66.146R2 CONFIG release-to-holster migrated; calibration preserved; single INI recovery checkpoint retained":
            "V66.146R2 CONFIG release-to-holster runtime-only; could not persist INI");
    }
    const bool userHolsters149=ReadIniBoolExplicit(explicit_ini,"Settings","UserShoulderHolsters",
        ReadIniBoolExplicit(explicit_ini,"Settings","LegacyShoulderHolsters",true));
#define KCD2VR_M2_HOST_SLOT 188 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=188
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 189 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=189
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // C10R2 / Claude candidate C: preserve the user's saved immersive HUD choice.
#define KCD2VR_M2_HOST_SLOT 190 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=190
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // Optional manual quiver reload comparison; now live-toggleable from IMMERSION.
#define KCD2VR_M2_HOST_SLOT 191 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=191
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // V66.162: retire the native camera-pitch servo, including existing V161 INIs.
    // Hand targeting keeps its scoped interactor ray; it must not steer camera input.
#define KCD2VR_M2_HOST_SLOT 192 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=192
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // V66.178 F4 rung 2 VR combat core: minimum strike quality (speed ratio of a full stroke) and design rhythm.
#define KCD2VR_M2_HOST_SLOT 193 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=193
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    g_exp59c10.strikeMinIntervalMs.store(int(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrStrikeMinIntervalMs",0.0f),0.0f,3000.0f)));
    g_exp59c10.strikeWaitNative.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrStrikeWaitForNativeAttack",true));
    frik_pose_probe::SetVrCombatCore178(g_exp59c10.strikeMinScale.load(),unsigned(g_exp59c10.strikeMinIntervalMs.load()),g_exp59c10.strikeWaitNative.load()); // R14: menu-owned
    // V66.178R6: digital stick-direction pseudo-keys for KCD2's menu navigation actionmaps.
#define KCD2VR_M2_HOST_SLOT 194 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=194
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // V66.178R7: page-memo window for guarded native reads (0 = query every page, V169 default 150 ms).
    frik_pose_probe::SetReadPageMemoMs(unsigned(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrReadPageMemoMs",150.0f),0.0f,150.0f)));
    // V66.178R8: 1 = attack/guard edges as native XI gamepad events (no synchronous handler call), 0 = direct call.
    attack_xi178r8::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrAttackTransport",false)); // V66.178R26: default DIRECT again (the V174 timing); the 69 ms "direct cost" was the combo capture, fixed in R23
    frik_pose_probe::SetF4ContactGate178r26(ReadIniIntExplicit(explicit_ini,"Settings","VrF4ContactGate",0)); // 0 = pass-through (V174 behaviour), 1 = release-ticket gate (V176)
    frik_pose_probe::SetSupportHiltRegion178r26(ReadIniBoolExplicit(explicit_ini,"Settings","VrSupportHiltRegion",false)); // false = V164 fixed socket
    frik_pose_probe::SetTwoHandV120_178r27(ReadIniBoolExplicit(explicit_ini,"Settings","VrTwoHandV120",true)); // V66.178R27: the V66.120/123 two-hand grip (socket toward the blade, authored hand frame, 8 deg constraint)
    combat_diag_probe::SetVrHitPolicy5913(unsigned(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrTargetRepeatMs",150),50,3000)),
        ReadIniBoolExplicit(explicit_ini,"Settings","VrHitHostileOnly",false)); // R59.13b
    combat_diag_probe::SetVrNeutralHitMinEnergy59c4(ReadIniFloatExplicit(explicit_ini,"Settings","VrNeutralHitMinEnergy",.10f)); // R59.16C4
    combat_diag_probe::SetGeometryAttackTypes178r27(ReadIniIntExplicit(explicit_ini,"Settings","VrGeometryAttackTypeCut",1),ReadIniIntExplicit(explicit_ini,"Settings","VrGeometryAttackTypeThrust",0)); // V66.178R27 rung 6: E_CombatAttackType passed to BuildAndSubmitMeleeHit

#define KCD2VR_CONFIG_LOAD_HYBRID0
#include "subsystems/hybrid/config.inl"
#undef KCD2VR_CONFIG_LOAD_HYBRID0

    frik_pose_probe::SetGeometryAutoLock178r29(ReadIniBoolExplicit(explicit_ini,"Settings","VrGeometryAutoLock",false)); // V66.178R29; R31 default off (out of combat R3 = toggle_crouch)
    g_combat_camera_only50.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrCombatCameraOnly",true),std::memory_order_relaxed); // V66.178R50
    g_combat_preset51.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrCombatAdmissionAssist",true),std::memory_order_relaxed);g_combat_cvars51.Load(explicit_ini,g_combat_preset51.load()); // V66.178R51
    g_player_rules52.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrCombatPlayerRules",true));g_shared_rules52.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrCombatSharedRules",false));
    g_no_lunge594.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrNoAttackLunge",true)); // R59.6: Miki's decision restored (C4 had flipped it to false)
    g_targeting_lease59c8=ReadIniBoolExplicit(explicit_ini,"Settings","VrCombatResponsiveTargeting",true);
    g_combat_cvars52.Load(g_player_rules52.load(),g_shared_rules52.load(),g_targeting_lease59c8,g_no_lunge594.load(),
        kcd2vr::free_targeting_d42::enabled.load()?kcd2vr::free_targeting_d42::mode():0); // V66.178R52 / R57 / R59.4 / R59.16C8 dwell / R133 cone (main loop re-leases on a mode change)
    combat_diag_probe::SetGeometryEngageNative178r31(ReadIniBoolExplicit(explicit_ini,"Settings","VrGeometryEngageNative",true)); // V66.178R31
    combat_diag_probe::SetGeometryOuter178r33(ReadIniBoolExplicit(explicit_ini,"Settings","VrGeometryOuterContinuation",true)); // V66.178R33
#define KCD2VR_M2_HOST_SLOT 195 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=195
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    combat_diag_probe::SetGeometryLiveAction178r35(ReadIniBoolExplicit(explicit_ini,"Settings","VrGeometryLiveAction",true)); // V66.178R35
#define KCD2VR_M2_HOST_SLOT 196 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=196
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    combat_diag_probe::SetContactNeverLost595(false); // retired unsafe fallback
    motion59c4::previewRequested.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrMotionOwnedTimingPreview",false));
#define KCD2VR_M2_HOST_SLOT 197 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=197
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    frik_pose_probe::SetPerfectBlockWindow178r42(ReadIniIntExplicit(explicit_ini,"Settings","VrPerfectBlockWindow",-1)); // V66.178R42
    frik_pose_probe::SetPlayerCombatWindowScaleH26(ReadIniFloatExplicit(explicit_ini,"Settings","VrPlayerCombatWindowScaleH26",1.5f));
    melee_tuning_c54::punchTravel.store(melee_tuning_c54::bound(ReadIniFloatExplicit(explicit_ini,"Settings","VrPunchTravelH41",1.0f),0.4f,2.0f,1.0f));melee_tuning_c54::guardHalfWidth.store(melee_tuning_c54::bound(ReadIniFloatExplicit(explicit_ini,"Settings","VrGuardPlaneHalfWidthH41",0.435629f),0.25f,0.75f,0.5f));melee_tuning_c54::guardHalfHeight.store(melee_tuning_c54::bound(ReadIniFloatExplicit(explicit_ini,"Settings","VrGuardPlaneHalfHeightH41",0.177844f),0.1f,0.35f,0.2f));melee_tuning_c54::guardBoxDepthD61.store(melee_tuning_c54::bound(ReadIniFloatExplicit(explicit_ini,"Settings","VrGuardBoxDepthD61",.50f),.10f,.80f,.50f));melee_tuning_c54::guardBodyDistanceD56.store(melee_tuning_c54::bound(ReadIniFloatExplicit(explicit_ini,"Settings","VrGuardMaxBodyDistanceD56",.57f),.35f,1.05f,.65f));player_window_h26::perfectBlockMultiplier.store(melee_tuning_c54::bound(ReadIniFloatExplicit(explicit_ini,"Settings","VrPerfectBlockScaleH41",2.0f),1.0f,2.0f,2.0f));player_window_h26::multiplier.store(melee_tuning_c54::bound(ReadIniFloatExplicit(explicit_ini,"Settings","VrMasterstrikeScaleH41",1.5f),1.0f,2.0f,1.5f)); // H26 exact Henry PB/MS native allowance only
    for(unsigned i=0;i<shield_block_r120::Count;++i)shield_block_r120::set(i,ReadIniFloatExplicit(explicit_ini,"Settings",shield_block_r120::kSpecs[i].key,shield_block_r120::kSpecs[i].def)); // R120 shield block box
    hand_present_d61::SetEnabled(ReadIniBoolExplicit(explicit_ini,"Settings","VrFreeHandPresentLeadD61",false));
    finisher_cb1::nativeValidationTestD61.store(false,std::memory_order_release); // session-only native comparator
    // D39_SMOOTH_PREFERENCE_LOAD_BEGIN: canonical OFF must beat an old legacy ON.
    const bool legacySmoothD39=ReadIniBoolExplicit(explicit_ini,"Settings","VrSmoothHands",false);
    hand_smooth_h50::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrHandSmoothH50",legacySmoothD39));
    hand_smooth_h50::minCutoffHz.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrHandSmoothMinHzH50",
        legacySmoothD39?smooth_lab::CutoffHzForRetention(smooth_lab::kFrikRetention90):1.5f),.2f,10.f));
    hand_smooth_h50::beta.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrHandSmoothBetaH50",20.f),0.f,100.f));
    smooth_lab::ApplyDefaults(false); // capture the explicit preference, then apply the current F2 override
    // D39_SMOOTH_PREFERENCE_LOAD_END
    {// J26: VrF2Rung (0..3, default 0). D45: active rungs record; startup remains baseline. first load
     // only; later reloads/saves never fight an F2 press. a recording rung starts its normal finite Bug lease.
     static bool f2LoadedJ26=false;
     if(!f2LoadedJ26){f2LoadedJ26=true;
      const unsigned savedJ26=unsigned(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrF2Rung",0),0,3));
      // C29: never start in a RECORDING rung (Miki's INI had 1 = bug recording every frame since J26, costs fps).
      const unsigned rungJ26=kcd2vr::smooth_lab::BugRecordingRung(savedJ26)?0u:savedJ26;
      if(rungJ26!=savedJ26){char m[160]{};std::snprintf(m,sizeof(m),"C29 F2_START saved=%u is a recording rung - starting in 0 (press F2 to record)",savedJ26);diagnostic_trace::Write(m);}
      if(kcd2vr::smooth_lab::SetMode(rungJ26)){const auto nowJ26=std::uint64_t(GetTickCount64());
          // D48: saved comparison preference never starts a recorder at game launch.
          char m[160]{};std::snprintf(m,sizeof(m),"J26 F2_START rung=%u from=VrF2Rung bug=%d",rungJ26,kcd2vr::smooth_lab::BugRecordingRung(rungJ26)?1:0);diagnostic_trace::Write(m);}}}
    kcd2vr::finger_rig_w1::capturedFistDefault.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrCapturedFistDefault",true)); // W1: F7 rung 2 native punch fist = the normal fist
    kcd2vr::elbow_pole_w1::outward.store(kcd2vr::elbow_pole_w1::clampOut(ReadIniFloatExplicit(explicit_ini,"Settings","VrElbowPoleOutW1",.20f))); // W1 relaxed elbow pole
    kcd2vr::elbow_pole_w1::backward.store(kcd2vr::elbow_pole_w1::clampBack(ReadIniFloatExplicit(explicit_ini,"Settings","VrElbowPoleBackW1",.45f)));
    kcd2vr::frik_elbow_lab::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrFrikElbows",true)); // D48: live Experiments preference; recording never changes this. R124: default ON (Miki's INI)
    kcd2vr::elbow_r89::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrElbowSolverR89",false)); // R90: R89 hand-guided elbow option, default OFF (D86 default); wins over VrFrikElbows
    kcd2vr::camera_hold_e1::maxHoldMs.store(std::uint32_t(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrCameraBodyHoldMs",1500),0,5000))); // E1: 0 = native eye at once
    kcd2vr::bow_arm_aim_r9::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrBowArmAim",true)); // R9 lane A
    kcd2vr::alchemy_own_head_h51::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrAlchemyOwnHeadH51",false)); // H51 GESTURE alchemy own-head experiment, default OFF
    g_exp59c10.weightFeel.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrWeaponWeightFeel",0.85f),0.2f,2.0f));
    frik_pose_probe::SetWeaponAuthorityScale178r44(g_exp59c10.weightFeel.load()); // V66.178R44 (R14: WEAPON FEEL page)
    g_pause_frozen_pose59c14.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrPauseFrozenPose",true)); // R59.16C10R14
    {   // R59.16C10R14 WEAPON FEEL: fatigue ramp, one-handed polearm, energy stamina, rung-2 min-power gate, master-strike veto, stowed bow hide
        g_exp59c10.fatigueRamp.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrWeaponFatigueRamp",0.6f),0.f,1.f));
        g_exp59c10.poleOneHand.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrPoleOneHandFeel",0.70f),.15f,1.f));
        g_exp59c10.strikeMinGate.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrStrikeMinScaleGate",false));
        // R59.16C10R15 (14 Oct 23:47 log: 19 strokes thrown away as TOO_WEAK; with the R14 drag the measured energy is lower,
        // the gate turned real swings into nothing - "weapon swings are so unreliable"): the gate is OFF again, once.
        if(ReadIniIntExplicit(explicit_ini,"Settings","VrStrikeGatePreferenceVersion",0)<1){
            g_exp59c10.strikeMinGate.store(false);(void)WritePrivateProfileStringA("Settings","VrStrikeMinScaleGate","false",explicit_ini);
            (void)WritePrivateProfileStringA("Settings","VrStrikeGatePreferenceVersion","1",explicit_ini);}
        frik_pose_probe::SetWeaponFeel59c14(g_exp59c10.fatigueRamp.load(),g_exp59c10.poleOneHand.load(),g_exp59c10.strikeMinGate.load(),ReadIniBoolExplicit(explicit_ini,"Settings","VrLiveAttributesAllRungs",true));
        g_exp59c10.swingStaminaScale.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrSwingStaminaScale",1.0f),0.f,2.f));kcd2vr::swing_effort59c14::scale.store(g_exp59c10.swingStaminaScale.load());
        g_exp59c10.masterStrikeGame.store(ReadIniIntExplicit(explicit_ini,"Settings","VrMasterStrikeVeto",0)==0);frik_pose_probe::SetMasterStrikeVeto59c14(!g_exp59c10.masterStrikeGame.load());
        int hiddenBow54=std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrHideStowedBow",2),0,2);
        if(ReadIniIntExplicit(explicit_ini,"Settings","VrStowedBowPreference54",0)<1){
            // Explicit user request: retire the inherited look-up-only mode;
            // later menu choices (including OFF) remain authoritative.
            const bool migrate54=hiddenBow54==1;
            if(migrate54)hiddenBow54=2;
            bool saved54=true;
            if(migrate54){
                saved54=WritePrivateProfileStringA("Settings","VrHideStowedBow","2",explicit_ini)!=FALSE;
            }
            if(saved54)(void)WritePrivateProfileStringA("Settings","VrStowedBowPreference54","1",explicit_ini);
            if(migrate54)diagnostic_trace::Write(saved54?"C54 STOWED_BOW mode=ALWAYS_HIDDEN inherited-look-up-only migrated; other INI settings preserved":"C54 STOWED_BOW mode=ALWAYS_HIDDEN runtime only; INI persist unavailable");
        }
        g_exp59c10.hideStowedBow.store(hiddenBow54);
        g_exp59c10.hideBowPitchDeg.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrHideStowedBowPitchDeg",20.f),0.f,80.f));
        {char names[160]{};(void)GetPrivateProfileStringA("Settings","VrHideStowedBowAttachments","weapon_bow,weapon_crossbow",names,sizeof(names),explicit_ini);frik_pose_probe::SetStowedBowAttachments59c15(names);} // R59.16C10R15
        frik_pose_probe::SetHideStowedBow59c14(g_exp59c10.hideStowedBow.load(),g_exp59c10.hideBowPitchDeg.load());
        // R121: VrHideStowedAtStations / VrHideBackWeapons are read in live_runtime_d40.inl (KCD2VR_D40_LIVE_LOAD), Experiments GEAR page 2.
        g_exp59c10.twoHandAxis.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrTwoHandAxisGrip",true));frik_pose_probe::SetTwoHandAxis59c16(g_exp59c10.twoHandAxis.load()); // R59.16C10R16
    }
    {   // R59.16C10R17 COMBAT FLOW (VR menu: WEAPON FEEL > COMBAT FLOW): every value is a menu control; the INI only stores it.
        auto& e=g_exp59c10;
        e.guardStrict.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrGuardStrictEntry",false));frik_pose_probe::SetGuardStrict59c17(e.guardStrict.load());
        e.showBlockIndicatorH24.store(ReadIniBoolExplicit(explicit_ini,"Settings",block_indicator_h24::iniKey,block_indicator_h24::defaultEnabled));
        e.noBlockCooldown.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrNoBlockCooldownF4",true));
        e.instantFreeSwing38.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrInstantFreeSwing",true)); // C38
        e.hideSwingLean38.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrHideSwingLean",true));kcd2vr::vr_combat111::hideSwingLean38.store(e.hideSwingLean38.load(),std::memory_order_relaxed); // C38
        e.staggerGuard39.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDirectGuardOutOfStagger",false));kcd2vr::direct_counter39::staggerGuard.store(e.staggerGuard39.load(),std::memory_order_relaxed); // C39
        {namespace cs=kcd2vr::counter_stab40;cs::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDirectCounterStab",true));cs::minEnergy.store(cs::clampEnergy(ReadIniFloatExplicit(explicit_ini,"Settings","VrDirectCounterStabEnergy",.6f)));
         cs::strengthMul.store(cs::clampMul(ReadIniFloatExplicit(explicit_ini,"Settings","VrDirectCounterStabStrength",1.5f)));cs::windowMs.store(cs::clampWindow(ReadIniIntExplicit(explicit_ini,"Settings","VrDirectCounterStabWindowMs",500)));
         cs::needsPerk.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDirectCounterStabNeedsPerk",true));} // C40 (C44: default ON, gated by the game's Master strike perk)
        {namespace mg=kcd2vr::master_gate47;const int legacy=ReadIniIntExplicit(explicit_ini,"Settings","VrMasterStrikeStabOnly",1);int m120=ReadIniIntExplicit(explicit_ini,"Settings","VrMasterStrikeModeR120",-1);
         // R120: VrMasterStrikeModeR120 (0..3) is the saved menu choice. not saved yet = old INI: "INSTANT masterstrikes" ON (VrInstantCombatD72) with the old
         // default gate 1 starts in INSTANT (3), everything else keeps its old mode. after the first save his menu choice is kept as it is.
         if(m120<0)m120=(legacy==1&&ReadIniBoolExplicit(explicit_ini,"Settings","VrInstantCombatD72",true))?mg::kModeInstant:legacy;
         mg::mode.store(mg::clampMode(m120));mg::minEnergy.store(mg::clampEnergy(ReadIniFloatExplicit(explicit_ini,"Settings","VrMasterStrikeStabEnergy",.30f)));mg::testForce.store(false);} // C47 (default 1: only a hard opposite-side thrust); DS-F5 TEST toggle is session-only, always OFF at startup
        e.dirBlock39.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDirectNpcDirectionalBlock",true));kcd2vr::dirblock39::enabled.store(e.dirBlock39.load(),std::memory_order_relaxed); // C39 DIRBLOCK
        e.msKeepStance.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrMasterStrikeKeepStance",true));
        {const float nh=ReadIniFloatExplicit(explicit_ini,"Settings","VrNativeHitTime",0.30f);e.nativeHitTime.store(nh<.10f?0.f:std::min(nh,.60f));}
        e.weaponStatEffect.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrWeaponStatEffect",1.0f),0.f,1.5f));
        frik_pose_probe::SetCombatFlow59c17(e.nativeHitTime.load(),e.msKeepStance.load(),e.weaponStatEffect.load());
        e.intentLive.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrIntentLiveRoute",true));
        e.intentLiveMs.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrIntentLiveWaitMs",60),0,120)); // contacts expire at 160 ms
        const int legacyComboMs=std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrComboWindowMs",1500),0,3000);
        const float comboSeconds=ReadIniFloatExplicit(explicit_ini,"Settings","VrComboWindowSeconds",float(legacyComboMs)*.001f);
        e.comboWindowMs.store(std::isfinite(comboSeconds)?int(std::lround(std::clamp(comboSeconds,0.f,3.f)*1000.f)):legacyComboMs);
        combat_diag_probe::SetComboWindowExtension(ReadIniBoolExplicit(explicit_ini,"Settings","VrComboWindowExtend",true));
        kcd2vr::combo_zone_assist_cz::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrComboZoneAssist",true)); // I12 (C29 default on)
        kcd2vr::feint_fe::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrFeintHybrid",true)); // D34 FE: post-commit HYBRID native feint (default on)
        kcd2vr::combo_hold_r14::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrComboRecoveryHold",false)); // I12/N2: hold Henry's attack end while the combo slot is open
        kcd2vr::combo_carrier_cb::enabled.store(kcd2vr::combo_hold_r14::enabled.load()); // D30-CMB: carrier keep-alive follows COMBO HELP: HOLD
        kcd2vr::combo_hold_r14::maxHoldMs.store(std::uint32_t(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrComboRecoveryHoldMs",1000),0,1500))); // 0 = off
        combat_diag_probe::SetComboTestForce(false); // DS-F6 TEST toggle is session-only, always OFF at startup
        combat_diag_probe::SetCombatFlow59c17(e.intentLive.load(),unsigned(e.intentLiveMs.load()),unsigned(e.comboWindowMs.load()));
        // C10R18 F4 press 4 (VR DIRECT): the blade owns Henry's hits; the enemy blocks at VrNpcBlockSpeed x his native block speed.
        e.ownHits18.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDirectOwnsHits",true));
        {const float bs=ReadIniFloatExplicit(explicit_ini,"Settings","VrNpcBlockSpeed",1.0f);e.npcBlockSpeed18.store(bs<=0.f?0.f:std::clamp(bs,1.f,3.f));}
        // C10R19: the NPC re-aims his block at Henry's +0x450 every frame (9E3B80), so R18's constant 1.5 only made his guard COUNT as up
        // earlier while the animation lagged. Native timing (= the C10R3 run) once; the slider stays the user's choice afterwards.
        if(ReadIniIntExplicit(explicit_ini,"Settings","VrRung4PreferenceVersion",0)<3&&e.npcBlockSpeed18.load()>0.f){
            e.npcBlockSpeed18.store(1.f);(void)WritePrivateProfileStringA("Settings","VrNpcBlockSpeed","1.00",explicit_ini);}
        combat_diag_probe::SetVrDirect18(e.ownHits18.load(),e.npcBlockSpeed18.load());
    }
    frik_pose_probe::SetAttackAtPrepare178r42(ReadIniBoolExplicit(explicit_ini,"Settings","VrAttackAtPrepare",true)); // V66.178R42
    frik_pose_probe::SetContactStartsAttack178r41(ReadIniBoolExplicit(explicit_ini,"Settings","VrContactStartsAttack",false)); // V66.178R41; R124 default OFF (Miki's INI)
    combat_diag_probe::SetGeometryRejectNativeSweep178r40(ReadIniBoolExplicit(explicit_ini,"Settings","VrGeometryRejectNativeSweep",true)); // V66.178R40
    frik_pose_probe::SetMotionClock50C2(ReadIniBoolExplicit(explicit_ini,"Settings","VrMotionAttackClock",true)); // R50C2: F4 rung 2 only
    combat_diag_probe::SetGeometryContactTime178r46(ReadIniBoolExplicit(explicit_ini,"Settings","VrGeometryContactTime",false),std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrGeometryContactTimePhaseMask",20),0,0x7ff)); // V66.178R46
    combat_diag_probe::SetGeometryHold178r39(unsigned(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrGeometryHoldMs",600),0,1500)),ReadIniIntExplicit(explicit_ini,"Settings","VrGeometryStrikePhase",8)); // V66.178R39
    combat_diag_probe::SetGeometryApply178r30(ReadIniBoolExplicit(explicit_ini,"Settings","VrGeometryApply",true)); // V66.178R30 (R28 review F01)
    frik_pose_probe::SetTwoHandConstraintV120_178r29(ReadIniBoolExplicit(explicit_ini,"Settings","VrTwoHandConstraintV120",false)); // V66.178R29: V130 constraint by default (the grip holds)
    // C19C4R1: recover easy normal-guard entry without the historical automatic
    // perfect-block exemption. Never silently rewrite an existing guard profile.
    // In particular, retire R17's one-shot 2.5/0 -> 1.5/60 INI migration.
    release_combat_r92::fastBlock.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrFastBlockR92",false));
    g_exp59c10.guardEnterSpeed.store(ReadIniFloatExplicit(explicit_ini,"Settings","VrGuardEnterMaxSpeed",2.5f));g_exp59c10.guardEnterDwellMs.store(std::max(0,ReadIniIntExplicit(explicit_ini,"Settings","VrGuardEnterDwellMs",0)));
    frik_pose_probe::SetGuardEntry178r29(g_exp59c10.guardEnterSpeed.load(),unsigned(g_exp59c10.guardEnterDwellMs.load())); // V66.178R29 (R45: no entry dwell)
    g_exp59c10.guardAfterAttackMs.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrGuardAfterAttackMs",400),0,2000));frik_pose_probe::SetGuardAfterAttackMs49(unsigned(g_exp59c10.guardAfterAttackMs.load())); // V66.178R49
    kcd2vr::master_gate47::requireOpposite.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrMasterStrikeOppositeSide",false)); // I10: false = the native window decides the side
    kcd2vr::master_gate47::cue.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrMasterStrikeCue",true)); // D30-CMB: weapon-hand pulse when the native MS window opens
    kcd2vr::fist_reach_r10::vrOnlyHits.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrFistVrOnlyHits",true)); // R10: no animated-fist hit while the VR fist is valid
    kcd2vr::fist_reach_r10::peakReplay.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrFistPeakReplay",true)); // R10: the punch's farthest real segment into the native window (R9 VrFistReachAssistM retired)
    kcd2vr::fist_clock_r11::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrFistClock",true));
    kcd2vr::fist_clock_r11::maxHoldMs.store(std::uint32_t(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrFistHoldMs",500),0,1000)));
    kcd2vr::combat_r9::keepSlotPhase2.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrComboKeepSlotPhase2",false)); // R9
    {const float h=ReadIniFloatExplicit(explicit_ini,"Settings","VrGuardHiltBelowHeadM",.40f);kcd2vr::combat_r9::guardHiltBelowHeadM.store(h<=0.f?0.f:std::clamp(h,.20f,.90f));} // R9: 0 = off
    g_exp59c10.guardExitSpeed.store(ReadIniFloatExplicit(explicit_ini,"Settings","VrGuardExitSpeed",3.5f));frik_pose_probe::SetGuardExitSpeed178r45(g_exp59c10.guardExitSpeed.load()); // V66.178R45: a strike from guard releases the block
    g_exp59c10.msHandoffOn23.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrMasterWindowHandoff",true)); // C24b: slider on COMBAT FLOW
#define KCD2VR_M2_HOST_SLOT 198 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=198
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    frik_pose_probe::SetMasterHandoff23(g_exp59c10.msHandoffOn23.load(),g_exp59c10.msHandoff23.load()); // C23: only inside the live native +7D8 window; <0 = follow VrGuardExitSpeed
    g_exp59c10.guardCounter.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrGuardRequireCounterMotion",true));frik_pose_probe::SetGuardCounterMotion57C1(g_exp59c10.guardCounter.load());
    g_exp59c10.guardAutoWindup.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrGuardAutoWindupRefresh",false));frik_pose_probe::SetGuardAutoWindup19c4r1(g_exp59c10.guardAutoWindup.load());
    frik_pose_probe::SetGuardIncoming178r45(ReadIniBoolExplicit(explicit_ini,"Settings","VrGuardIncomingRepress",true),
        std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrGuardIncomingPhaseMask",12),0,0x7ff),
        unsigned(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrGuardIncomingCooldownMs",120),0,2000))); // V66.178R45
#define KCD2VR_M2_HOST_SLOT 199 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=199
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    frik_pose_probe::SetThrustAsSwing55(ReadIniBoolExplicit(explicit_ini,"Settings","VrThrustAsSwing",true));
#define KCD2VR_M2_HOST_SLOT 200 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=200
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    g_exp59c10.windupHold.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrWindupHold",true));frik_pose_probe::SetWindupHold57(g_exp59c10.windupHold.load());
    g_exp59c10.wallSlide.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrWallSlideOptInH18",false));frik_pose_probe::SetWallSlide59c10(g_exp59c10.wallSlide.load());
    frik_pose_probe::SetWallFriction57(ReadIniFloatExplicit(explicit_ini,"Settings","VrWallFriction",.35f)); // R59.16C10d
    g_exp59c10.attackRequireOpponent.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrAttackRequireOpponent",true));g_exp59c10.attackReachM.store(ReadIniFloatExplicit(explicit_ini,"Settings","VrAttackReachMeters",2.6f));
    frik_pose_probe::SetAttackOpponentGate59c10(g_exp59c10.attackRequireOpponent.load(),g_exp59c10.attackReachM.load()); // R59.16C10R4F
    {   // R59.16C10R11 action time owner: VrActionTimeOwner 0 off / 1 observe (receipts) / 2 own; the hold point, resume speed, watchdog,
        // commit pull, cancel grip and cancel mode; VrActionOwnFragments = Mannequin fragment names the owner may hold (see KCD2VR.ini).
        g_exp59c10.actionOwner.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrActionTimeOwner",2),0,2));
        // R59.16C10R15: R11-R14 shipped OBSERVE (1) - the hold never ran ("no change in the interactions"). OWN once, then the user's choice.
        if(ReadIniIntExplicit(explicit_ini,"Settings","VrActionOwnerPreferenceVersion",0)<1){
            if(g_exp59c10.actionOwner.load()==1){g_exp59c10.actionOwner.store(2);(void)WritePrivateProfileStringA("Settings","VrActionTimeOwner","2",explicit_ini);}
            (void)WritePrivateProfileStringA("Settings","VrActionOwnerPreferenceVersion","1",explicit_ini);}
        action_time_owner59c11::Tuning t{};
        t.holdAt=std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrActionHoldAt",0.50f),.05f,.90f);g_exp59c10.actionHoldAt.store(t.holdAt);
        t.resumeSpeed=std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrActionResumeSpeed",2.0f),.25f,8.f);g_exp59c10.actionResumeSpeed.store(t.resumeSpeed);
        t.holdTimeoutMs=static_cast<unsigned>(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrActionHoldTimeoutMs",2500),300,15000));
        t.commitPullM=std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrActionCommitPullCm",10.f),2.f,60.f)/100.f;
        t.cancelGrip=std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrActionCancelGrip",0.35f),0.f,.9f);
        t.cancelMode=std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrActionCancelMode",1),0,1);
        action_time_owner59c11::SetTuning(t);
        char owned[512]{};GetPrivateProfileStringA("Settings","VrActionOwnFragments","Picking,Placing,AlchemyTake,AlchemyReturn,AlchemyPutHerb,AlchemyPutBowl,AlchemyPutMortar,DogPetting,CarryItemPickup,CarryItemPlace",owned,DWORD(sizeof(owned)),explicit_ini);
        g_owned_fragments60=owned;ApplyOwnedFragments60();
        action_time_owner59c11::SetMode(g_exp59c10.actionOwner.load());
        frik_pose_probe::SetGrabMode60(ReadIniIntExplicit(explicit_ini,"Settings","VrGrabMode",0));ApplyOwnedFragments60(); // C61 safe mode 0/2
    }
#define KCD2VR_M2_HOST_SLOT 201 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=201
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    g_exp59c10.npcEtaPrepare.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrNpcBlockEtaPrepare",0.25f),.10f,.60f));g_exp59c10.npcEtaStrike.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrNpcBlockEtaStrike",0.12f),.10f,.60f));
    // R59.16C10R16 (rung-4 analysis): phase 8 at the COMMIT, not at the first forward sample (22 of 43 R14B early offers went to
    // swings that never happened); NPC block times below 0.10 s put the block's sync point before the blade. Applied once.
#define KCD2VR_M2_HOST_SLOT 202 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=202
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // C10R18 (Miki, 15 Oct: the C10R3 run where press 4 felt best - the enemy BEGAN to block): the strike phase at the swing START
    // again, now with a natural NPC block speed and no native parry of a swing without a contact. Applied once; then the user's choice.
#define KCD2VR_M2_HOST_SLOT 203 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=203
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    if(ReadIniIntExplicit(explicit_ini,"Settings","VrRung4PreferenceVersion",0)<3) // C10R19: VrNpcBlockSpeed native (applied above)
        (void)WritePrivateProfileStringA("Settings","VrRung4PreferenceVersion","3",explicit_ini);
#define KCD2VR_M2_HOST_SLOT 204 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=204
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    g_exp59c10.directBlockLo39.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrDirectNpcBlockSpeedLow",2.f),1.f,3.f));
    g_exp59c10.directBlockHi39.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrDirectNpcBlockSpeedHigh",3.f),1.f,3.f));
#define KCD2VR_M2_HOST_SLOT 205 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=205
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    combat_diag_probe::SetNpcBlockEta59c10(g_exp59c10.npcEtaPrepare.load(),g_exp59c10.npcEtaStrike.load()); // R59.16C10R4B (S33): seconds to contact announced to the NPC block aligner // R59.16C10 // V66.178R57 // V66.178R55: thrust = ordinary attack from its origin side
    frik_pose_probe::SetCombatNoStep55(ReadIniBoolExplicit(explicit_ini,"Settings","VrCombatNoStep",true));    // V66.178R55: no native attack alignment step/turn while the VR weapon owns the attack
#define KCD2VR_M2_HOST_SLOT 206 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=206
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 207 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=207
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    frik_pose_probe::SetNativeHandPhysicsH24(ReadIniBoolExplicit(explicit_ini,"Settings","VrNativeHandPhysics",false));
    // D39: persist the complete NORMAL/PRECISE/BALANCED choice; old INIs
    // keep their boolean preference until a tri-state choice is explicitly saved.
    // R124: neither key in the INI = 2 BALANCED (Miki's INI). an old INI with only the bool keeps it (true 1 / false 0).
    {const bool preciseT=ReadIniBoolExplicit(explicit_ini,"Settings","VrPreciseSurfaceContacts",true),
                preciseF=ReadIniBoolExplicit(explicit_ini,"Settings","VrPreciseSurfaceContacts",false);
     frik_pose_probe::SetHandsModeDs2(ReadIniIntExplicit(explicit_ini,"Settings","VrHandsModeDs2",preciseT!=preciseF?2:(preciseT?1:0)));}
    frik_pose_probe::SetHandSeparationDs1(ReadIniBoolExplicit(explicit_ini,"Settings","VrHandSeparationDs1",false)); // DS1; R124 default OFF (Miki's INI)
    frik_pose_probe::SetShoulderReachD38(ReadIniFloatExplicit(explicit_ini,"Settings","VrShoulderReachWarningM",
        ReadIniFloatExplicit(explicit_ini,"Settings","VrShoulderReachMaxM",.15f))); // D39 diagnostic warning only; never a reach cap
    utility54::SetTorchGestureD39(ReadIniBoolExplicit(explicit_ini,"Settings","VrTorchGestureD39",true));
    utility54::SetVisorGestureD39(ReadIniBoolExplicit(explicit_ini,"Settings","VrVisorGestureD39",true));
#define KCD2VR_D40_LIVE_LOAD
#include "subsystems/experimental/live_runtime_d40.inl"
#undef KCD2VR_D40_LIVE_LOAD
    {   // R118: the chosen controller layout always goes to KCD2VR.log (one line per config load, not F2-gated).
        namespace cl=kcd2vr::controller_layout_r118;char m[200]{};
        std::snprintf(m,sizeof(m),"R118 CONTROLLER_LAYOUT layout=%s xHold=%s leftMenu=INVENTORY modifier=%s (VrControllerLayoutR118, VR CONFIG > CONTROLS)",
            cl::NameA(cl::Get()),cl::XHoldOpensInventory(cl::Get())?"INVENTORY":"TORCH",kcd2vr::quick_access_r112::FaceTouch()?"FACE_TOUCH":"THUMBREST");
        diagnostic_trace::WriteStatus(m);
    }
    if(ReadIniIntExplicit(explicit_ini,"Settings","VrArrowHoldNativeTurnD56Adopted",0)<1){
        const bool savedD56=WritePrivateProfileStringA("Settings","VrArrowHoldNativeTurn","true",explicit_ini)!=FALSE;
        if(savedD56)(void)WritePrivateProfileStringA("Settings","VrArrowHoldNativeTurnD56Adopted","1",explicit_ini);
        diagnostic_trace::Write("D56 ARROW_ORIENTATION requestedDefault=NATIVE laterMenuChoice=PRESERVED menu=RANGED");
    }

#define KCD2VR_CONFIG_LOAD_GORE0
#include "subsystems/gore/config.inl"
#undef KCD2VR_CONFIG_LOAD_GORE0


#define KCD2VR_CONFIG_LOAD_DIALOGUES0
#include "subsystems/dialogues/config.inl"
#undef KCD2VR_CONFIG_LOAD_DIALOGUES0

    frik_pose_probe::SetCalibrationAutoRestore43(ReadIniBoolExplicit(explicit_ini,"Settings",kcd2vr::calibration_restore43::kIniKey,true)); // C43: missing key = ON
    activity_input163::trackedHands59c7f[0]=ReadIniBoolExplicit(explicit_ini,"Settings","VrAlchemyHands",true);
    activity_input163::trackedHands59c7f[1]=ReadIniBoolExplicit(explicit_ini,"Settings","VrSmithingHands",true);
    activity_input163::trackedHands59c7f[3]=ReadIniBoolExplicit(explicit_ini,"Settings","VrSharpeningHands",true);
    activity_input163::trackedHands59c7f[2]=ReadIniBoolExplicit(explicit_ini,"Settings","VrReadingHands",false);

#define KCD2VR_CONFIG_LOAD_SMITHING0
#include "subsystems/smithing/config.inl"
#undef KCD2VR_CONFIG_LOAD_SMITHING0

    frik_pose_probe::SetGrabHandStays43(ReadIniBoolExplicit(explicit_ini,"Settings","VrGrabHandStaysOnController",true)); // C43
    // V66.178R9: off-hand socket on two-handed melee weapons.
    weapon_grip_cm_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrWristFromGripCm",0.f),-15.f,15.f),std::memory_order_relaxed); // R59.16C7F (Immersion sliders)
    support_sword_cm_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrSupportGripOffsetCm",10.0f),6.f,60.f),std::memory_order_relaxed);
    support_pole_cm_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrSupportGripOffsetPoleCm",32.0f),6.f,60.f),std::memory_order_relaxed);
    g_support_socket_synthetic59c7f=ReadIniBoolExplicit(explicit_ini,"Settings","VrSupportSocketSynthetic",true);
#define KCD2VR_M2_HOST_SLOT 208 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=208
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    frik_pose_probe::SetSupportSlot34(ReadIniBoolExplicit(explicit_ini,"Settings","VrSupportFromWeaponSlot",true)); // C34b
    attack_xi178r8::reset();
    // V66.178R10: F4 rung 3 rules, kick transport, F11 workstation hands.
    // R49C1: these legacy overrides disabled opponent acquisition as well as camera steering.
    // Ignore stale INI values while the native combat adapters require that relationship.
    combat_rules178r10::autoLockMode.store(0);
    combat_rules178r10::lockDisabled.store(false);
    diagnostic_trace::Write("V66.178R49C1 COMBAT_TARGET_POLICY nativeAcquisition=PRESERVED legacyAutoLockOverrides=IGNORED hmdCamera=UNCHANGED");
    kcd2vr::vr_combat111::hideNativeClips178r25.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrCombatHideNativeClips",true)); // V66.178R25
    kcd2vr::vr_combat111::hideEngagedClips50.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrCombatHideEngagedClips",true)); // V66.178R50
#define KCD2VR_M2_HOST_SLOT 209 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=209
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 210 // M2_SOURCE_SELECTOR
#include "subsystems/experimental/host.inl" // M2_OWNED_SOURCE slot=210
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    {const int iniT=combat_rules178r10::kickTransport.load();combat_rules178r10::kickTransportIni26.store(iniT==0?3:iniT); // C26 KICKS page (style 2 = old transport 0)
     auto& e=g_exp59c10;e.kickStyle26.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrKickStyle",0),0,2));
     e.kickLeg26.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrKickLeg",2),0,2));e.kickReachMs26.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrKickReachMs",730),300,1200));
     e.kickFrees26.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrKickFreesImpale",true));
     kcd2vr::kick26::nativeSpecial33.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrKickSpecialInput",true));kcd2vr::kick26::fallbackLeg.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrKickFallbackLeg",true)); // H26; R124 default ON (Miki's INI)
      kcd2vr::kick26::realHitTest37c2.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrKickRealHitTest",false)); // Legacy preference only: direct foot-contact damage is retired until native action admission is joined.
    kcd2vr::kick26::profile52.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrKickProfile",0),0,1));
     kcd2vr::kick26::reachPercent52.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrKickReach",92),60,100));
     frik_pose_probe::SetKick26(e.kickStyle26.load(),e.kickLeg26.load(),e.kickReachMs26.load(),e.kickFrees26.load());
     e.statSim26.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrSimulateHenry",0),0,3));frik_pose_probe::SetStatSim26(e.statSim26.load()); // C26c
     g_vr_shadow_quality49.store(kcd2vr::graphics_owner31::migrateShadowLevel(ReadIniIntExplicit(explicit_ini,"Settings","VrShadowQuality",kcd2vr::view_defaults_b2::Get("VrShadowQuality", -1)),ReadIniBoolExplicit(explicit_ini,"Settings","VrShadowTweaks",kcd2vr::view_defaults_b2::Get("VrShadowTweaks", false))));g_eye_head26.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrEyeAdaptHeadSteady",kcd2vr::view_defaults_b2::Get("VrEyeAdaptHeadSteady", true)));} // C26d / C49 VrShadowQuality (old VrShadowTweaks migrates)
    attack_xi178r8::specialViaLb.store(combat_rules178r10::kickTransport.load()==2); // V66.178R17: LB press needs the KCD2VR_Input mod
    combat_rules178r10::workstationEnabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrWorkstationHands",true));
    workstation178r20c1::gesturesEverywhere59c2.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrWorkstationGestures",true)); // R59.16C2
    misc_gestures_d59::SetPet(ReadIniBoolExplicit(explicit_ini,"Settings","VrPetGestureD59",true));
    misc_gestures_d59::SetWash(ReadIniBoolExplicit(explicit_ini,"Settings","VrWashGestureD59",false));
    misc_gestures_d59::alchemyTransfer.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrAlchemyTransferD59",true)) /* R124: default ON (Miki's INI) */;
    workstation178r20c1::alchemyHandRay59c2.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrAlchemyHandRay",true)); // Retired compatibility value; no live consumer.
    subsystems::alchemy::reach_craft1::setPickupReachD39(ReadIniIntExplicit(explicit_ini,"Settings","VrAlchemyPickupReachD39",0));
    workstation178r20c1::alchemyPassenger42.store(true); // C46R1 retires the incompatible free-placement alchemy experiment.
    minigame_hands45::mode.store(minigame_hands45::clampMode(ReadIniIntExplicit(explicit_ini,"Settings","VrMinigameHands",minigame_hands45::SemiPassenger))); // C45
    minigame_hands45::setMode(minigame_hands45::Alchemy,ReadIniIntExplicit(explicit_ini,"Settings","VrAlchemyMode",minigame_hands45::SixDofVrik) /* R124: missing = 3 FREE 6DOF like smithing (Miki's INI) */);
    minigame_hands45::setMode(minigame_hands45::Smithing,ReadIniIntExplicit(explicit_ini,"Settings","VrSmithingMode",minigame_hands45::SixDofVrik));
    kcd2vr::station_body_h41::fallbackEnabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrStation6DofFallback",true)); // R9C: refused 6DOF station visit -> FREE body
    kcd2vr::station_ws_j27::torsoEnabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrStationTorsoFollowHead",true)); // J27: hips behind the head at stations
    kcd2vr::station_ws_j27::setToolCollision(ReadIniBoolExplicit(explicit_ini,"Settings","VrSmithToolCollision",true)); // J27: hammer/tongs/iron stop at the world
    {// A3 VrSmithAnvilContact: 0 off, 1 REAL ANVIL (default), 2 LEARNED. an INI that only has the old A2 key keeps its choice.
     const bool legacyA2=ReadIniBoolExplicit(explicit_ini,"Settings","VrSmithAnvilCollisionA2",false);
     kcd2vr::subsystems::smithing::anvil_a2::setMode(ReadIniIntExplicit(explicit_ini,"Settings","VrSmithAnvilContact",legacyA2?2:1));}
    {const float c=ReadIniFloatExplicit(explicit_ini,"Settings","VrStationChestClearCm",12.f);kcd2vr::station_ws_j27::clearCm.store(std::isfinite(c)?std::clamp(c,0.f,30.f):12.f);} // J27
    workstation178r20c1::alchemy_book_as1::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrWorkstationHenryEyeAS1",true));

#define KCD2VR_CONFIG_LOAD_DOORS0
#include "subsystems/doors/config.inl"
#undef KCD2VR_CONFIG_LOAD_DOORS0

    frik_pose_probe::SetNpcFlinch178r11(ReadIniBoolExplicit(explicit_ini,"Settings","VrNpcFlinch",true)); // V66.178R11
    frik_pose_probe::SetContactEffects178r12(ReadIniBoolExplicit(explicit_ini,"Settings","VrContactEffects",true)); // V66.178R12
    frik_pose_probe::SetNativeHandover178r14(ReadIniBoolExplicit(explicit_ini,"Settings","VrNativeHandover",true)); // V66.178R14
    frik_pose_probe::SetFistNativeArm178r15(ReadIniBoolExplicit(explicit_ini,"Settings","VrFistNativeArm",false)); // V66.178R15
#define KCD2VR_M2_HOST_SLOT 211 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=211
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    combat_rules178r10::reset();
#define KCD2VR_M2_HOST_SLOT 212 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=212
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR

#define KCD2VR_CONFIG_LOAD_RANGED_RELOAD
#include "subsystems/ranged_reload/config.inl"
#undef KCD2VR_CONFIG_LOAD_RANGED_RELOAD

#define KCD2VR_M2_HOST_SLOT 213 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=213
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 214 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=214
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // V62.9: release-facing PC key targets for the fixed Touch action layout.
    // These are ordinary Win32 virtual-key codes; changing them never patches KCD2 input objects.
    {
        TouchBindings::Config tc{};
        tc.move_forward=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","MoveForwardVk",'W'),1,255));
        tc.move_left=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","MoveLeftVk",'A'),1,255));
        tc.move_back=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","MoveBackVk",'S'),1,255));
        tc.move_right=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","MoveRightVk",'D'),1,255));
        tc.sprint=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","SprintVk",VK_LSHIFT),1,255));
        tc.crouch=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","CrouchVk",'C'),1,255));
        tc.jump=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","JumpVk",VK_SPACE),1,255));
        tc.interact=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","InteractVk",'E'),1,255));
        tc.torch=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","TorchVk",'R'),1,255));
        tc.visor=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","VisorVk",'Z'),1,255));
        tc.horse_dog=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","HorseDogVk",'X'),1,255));
        tc.inventory=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","InventoryVk",'I'),1,255));
        tc.pause=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","PauseVk",VK_ESCAPE),1,255));
        tc.map=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","MapVk",'M'),1,255));
        tc.quick_reply=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","QuickReplyVk",'Q'),1,255));
        tc.skip_time=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","SkipTimeVk",'T'),1,255));
        tc.quick_left=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","QuickLeftVk",'5'),1,255));
        tc.quick_right=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","QuickRightVk",'6'),1,255));
        tc.weapon_slot1=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","WeaponSlot1Vk",'1'),1,255));
        tc.weapon_slot2=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","WeaponSlot2Vk",'2'),1,255));
        touch_bindings_.SetConfig(tc);
    }
    vr_presentation_mode_.store(ReadIniIntExplicit(explicit_ini, "Settings", "VrPresentationMode", kcd2vr::view_defaults_b2::Get("VrPresentationMode", 3)), std::memory_order_relaxed);
    vr_zoom_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrPresentationZoom", kcd2vr::view_defaults_b2::Get("VrPresentationZoom", kV6504DefaultZoom)), std::memory_order_relaxed);
    vr_auto_fov_trim_deg_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrAutoFovTrimDegrees", kcd2vr::view_defaults_b2::Get("VrAutoFovTrimDegrees", 0.0f)),-10.0f,10.0f), std::memory_order_relaxed);
    vr_auto_center_x_trim_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrAutoCenterXTrim", kcd2vr::view_defaults_b2::Get("VrAutoCenterXTrim", 0.00f)),-0.12f,0.12f), std::memory_order_relaxed);
    vr_auto_center_y_trim_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrAutoCenterYTrim", kcd2vr::view_defaults_b2::Get("VrAutoCenterYTrim", 0.00f)),-0.12f,0.12f), std::memory_order_relaxed);
    vr_auto_projection_center_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "VrAutoProjectionCenter", false), std::memory_order_relaxed);
    vr_pitch_correction_deg_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrPitchCorrectionDeg", 0.0f),-15.0f,15.0f), std::memory_order_relaxed);
#define KCD2VR_M2_HOST_SLOT 215 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=215
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    vr_source_vertical_fov_deg_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrSourceVerticalFovDegrees", kcd2vr::view_defaults_b2::Get("VrSourceVerticalFovDegrees", kV6504DefaultSourceFov)), std::memory_order_relaxed);
    vr_corrected_view_vfov_deg_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrCorrectedCenterScaleFovDegrees", kcd2vr::view_defaults_b2::Get("VrCorrectedCenterScaleFovDegrees", kV6511CorrectedCenterScaleFov)),85.0f,115.0f), std::memory_order_relaxed);
    vr_cull_frustum_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrCullFrustumScale", kcd2vr::view_defaults_b2::Get("VrCullFrustumScale", 1.0f)),1.0f,1.60f), std::memory_order_relaxed);
#define KCD2VR_M2_HOST_SLOT 216 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=216
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 217 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=217
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    vr_flat_screen_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrFlatScreenScale", kcd2vr::view_defaults_b2::Get("VrFlatScreenScale", 1.70f)),0.45f,3.00f), std::memory_order_relaxed);
    vr_passive_full_frame_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrPassiveFullFrameScale", kcd2vr::view_defaults_b2::Get("VrPassiveFullFrameScale", 1.20f)),1.00f,2.00f), std::memory_order_relaxed);
    kcd2vr::ui_surface_geometry::hudDistanceR110.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrHudDistanceR110",3.0f),kcd2vr::ui_surface_geometry::kHudConvergenceMinR127,kcd2vr::ui_surface_geometry::kHudConvergenceMaxR127),std::memory_order_relaxed); // R110: HUD far away, same size
    // R112: size of the big directional combat crosshair (CombatCursorCross), 0.25..1.50, default 0.50 (native was 1.00 = far too big in VR).
    kcd2vr::ui_hud_r112::combatCrosshairScale.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrCombatCrosshairScaleR112",kcd2vr::ui_hud_r112::kCombatCrosshairDefault),kcd2vr::ui_hud_r112::kCombatCrosshairMin,kcd2vr::ui_hud_r112::kCombatCrosshairMax),std::memory_order_relaxed);
    vr_pause_overlay_size_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrPauseOverlaySize", 0.98f),0.45f,1.15f), std::memory_order_relaxed);
    // V66.160 migration: 1.00 filled the whole VR canvas, so the right-anchored pause
    // list sat far right and oversized (also the death screen, which shares it). Move an
    // untouched 1.00 to 0.65 once; any later user choice is preserved.
    if(ReadIniIntExplicit(explicit_ini,"Settings","VrPauseOverlayPreferenceVersion",0)<1){
        if(std::abs(vr_pause_overlay_size_.load(std::memory_order_relaxed)-1.0f)<=0.0001f){
            vr_pause_overlay_size_.store(0.65f,std::memory_order_relaxed);
            (void)WritePrivateProfileStringA("Settings","VrPauseOverlaySize","0.65",explicit_ini);}
        (void)WritePrivateProfileStringA("Settings","VrPauseOverlayPreferenceVersion","1",explicit_ini);
    }
    if(ReadIniIntExplicit(explicit_ini,"Settings","VrPauseOverlayPreferenceVersion",0)<2){ // R59.16C10R14: the V66.160 0.65 -> 0.98 (buttons are scaled since C10R2)
        if(std::abs(vr_pause_overlay_size_.load(std::memory_order_relaxed)-0.65f)<=0.0001f){
            vr_pause_overlay_size_.store(0.98f,std::memory_order_relaxed);
            (void)WritePrivateProfileStringA("Settings","VrPauseOverlaySize","0.98",explicit_ini);}
        (void)WritePrivateProfileStringA("Settings","VrPauseOverlayPreferenceVersion","2",explicit_ini);
    }
    vr_transient_zoom_retain_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrTransientZoomRetain", 0.00f),0.0f,1.0f), std::memory_order_relaxed);
    vr_depth_stereo_strength_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrDepthStereoStrength", kcd2vr::view_defaults_b2::Get("VrDepthStereoStrength", 0.30f)),0.0f,1.0f), std::memory_order_relaxed);
#define KCD2VR_M2_HOST_SLOT 218 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=218
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    mirror_mis_menu_to_desktop_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "MirrorMisMenuToDesktop", false), std::memory_order_relaxed);
    vr_left_offset_x_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrLeftOffsetX", kcd2vr::view_defaults_b2::Get("VrLeftOffsetX", 0.0f)), std::memory_order_relaxed);
    vr_left_offset_y_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrLeftOffsetY", kcd2vr::view_defaults_b2::Get("VrLeftOffsetY", 0.0f)), std::memory_order_relaxed);
    vr_right_offset_x_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrRightOffsetX", kcd2vr::view_defaults_b2::Get("VrRightOffsetX", 0.0f)), std::memory_order_relaxed);
    vr_right_offset_y_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrRightOffsetY", kcd2vr::view_defaults_b2::Get("VrRightOffsetY", 0.0f)), std::memory_order_relaxed);
    vr_stereo_experiment_.store(0, std::memory_order_relaxed); // V61.39: stale AFR/DUAL INI values ignored
    { const int saved=ReadIniIntExplicit(explicit_ini, "Settings", "VrCleanPresentationMode", kcd2vr::view_defaults_b2::Get("VrCleanPresentationMode", 0)); vr_cyclopean_mode_.store(saved==1?1:0, std::memory_order_relaxed); }
    vr_rotation_order_.store(0, std::memory_order_relaxed); // V61.20 D*B diagnostic retired
    vr_afr_ipd_scale_.store(0.0f, std::memory_order_relaxed); // V61.39: retired
    vr_swap_eyes_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "VrSwapEyes", false), std::memory_order_relaxed);
    enable_legacy_probes_.store(developer_mode::Enabled()&&ReadIniBoolExplicit(explicit_ini, "Settings", "EnableLegacyStereoProbes", false), std::memory_order_relaxed);
    enable_native_eye_write_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableNativeEyeWrite", false), std::memory_order_relaxed);
#define KCD2VR_M2_HOST_SLOT 219 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=219
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR

    // Apply for this run even if the INI could not be written. This is after
    // every settings bind; reloading after the recorded migration keeps the
    // user's later menu choices. No movement option is overridden here.
    if(applyRepairD85R5R1 || repairPromptSizeR92R1){
        prompt_policy_d80::minigameScale.store(.45f,std::memory_order_release);
    }

    {
        char trace[320]{};
        std::snprintf(
            trace, sizeof(trace),
            "config parsed: Enabled=%s ApplyHeadRotation=%s ApplyHeadPosition=%s DiagnosticRendering=%s EnableVrOutputBridge=%s EnableNativeEyeWrite=%s HeadPoseScale=%.3f BalancedGraphics=%s Clarity=%.2f",
            enabled_.load(std::memory_order_relaxed) ? "true" : "false",
            apply_head_rotation_.load(std::memory_order_relaxed) ? "true" : "false",
            apply_head_position_.load(std::memory_order_relaxed) ? "true" : "false",
            diagnostic_rendering_.load(std::memory_order_relaxed) ? "true" : "false",
            enable_vr_output_bridge_.load(std::memory_order_relaxed) ? "true" : "false",
            enable_native_eye_write_.load(std::memory_order_relaxed) ? "true" : "false",
            head_pose_scale_.load(std::memory_order_relaxed),
            enable_vr_balanced_graphics_.load(std::memory_order_relaxed) ? "true" : "false",
            vr_engine_clarity_.load(std::memory_order_relaxed));
        debug_trace::Write(trace);
    }

    {
        char trace[768]{};
        std::snprintf(trace, sizeof(trace),
            "future gates (inert until implemented): bodyIK=%s holsters=%s motion=%s melee=%s staminaHandling=%s grabbing=%s npcPhysical=%s bow=%s alchemy=%s smithing=%s noLockSteering=%s hideCursor=%s wristUI=%s",
            enable_body_ik_.load() ? "true" : "false",
            enable_virtual_holsters_.load() ? "true" : "false",
            enable_motion_controls_.load() ? "true" : "false",
            enable_native_melee_.load() ? "true" : "false",
            enable_stamina_weapon_handling_.load() ? "true" : "false",
            enable_physical_grabbing_.load() ? "true" : "false",
            enable_npc_physical_reaction_.load() ? "true" : "false",
            enable_physical_bow_.load() ? "true" : "false",
            enable_alchemy_vr_.load() ? "true" : "false",
            enable_smithing_vr_.load() ? "true" : "false",
            disable_target_lock_steering_.load() ? "true" : "false",
            hide_center_cursor_.load() ? "true" : "false",
            enable_wrist_body_ui_.load() ? "true" : "false");
        debug_trace::Write(trace);
    }

    return true;
}

// V66.178R58 ROOMSCALE FOLLOW + COMBAT DODGE (vrik/roomscale58.h). Outside menus/dialogue/theater, with the 6DOF
// body on and the option set, the remaining HMD offset drives the native movement stick toward the player; in combat
// with a locked opponent a quick sideways/backward HMD move sends the native dodge (jump + direction while locked)
// once. The stick values (and B for the dodge) are the only things added to the pad.
// R59.16C5 GESTURE QUICK TALK: pointing / raised hand -> the game's own quick-talk menu (perks, buffs, morale and NPC
// reactions stay native). Fed by a per-loop snapshot of the head and both controllers (same XR frame).
#define KCD2VR_M2_HOST_SLOT 5016 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=5016
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 5017 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=5017
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
void Mod::Run()
{
    diagnostics::perf_d61::gate.select(false,diagnostics::perf_d61::nowMs()); // close any prior run lease
    diagnostics::map_d72::stopD84();
    subsystems::HostServices moduleHost{};
    moduleHost.log=[](std::uintptr_t,subsystems::ModuleId,subsystems::LogLevel,const char* text) noexcept {diagnostic_trace::Write(text);};
    const auto modulesReady=subsystems::StartCatalog(moduleHost);
    if(!modulesReady){char message[180]{};std::snprintf(message,sizeof(message),"M2 SUBSYSTEM_CONTRACT refused=%s module=%u other=%u",subsystems::ErrorName(modulesReady.error),modulesReady.module,modulesReady.other);debug_trace::Write(message);return;}
    // R118: no KCD2VR.ini beside the mod = write the built-in default first (never over an existing file), then the
    // normal pending-setup merge and LoadConfig run on it. Result goes to KCD2VR.log from LoadConfig.
    core::default_ini_r118::lastResult=startup_s15::EnsureDefaultIniR118();
    if(!startup_s15::ApplyPendingBeforeConfig())debug_trace::Write("H21 STARTUP_SETUP pending=NOT_APPLIED reason=VALIDATION_OR_FILE_ACCESS currentSettings=PRESERVED");
    if (!LoadConfig()){(void)subsystems::StopCatalog();return;}
    native_upscaler_stereo_d82::InstallEarly(enabled_.load(std::memory_order_acquire)&&vr_production_stereo_mode_.load(std::memory_order_acquire)==3);
    native_upscaler_stereo_d82::SetTechnologyReader(+[]() noexcept {
        void* value=nativeUpscalerCvarD82.load(std::memory_order_acquire);
        return value?SafeCVarGetInt(value,-1):-1;
    });
    const float linkedStartupFov=kcd2vr::linked_fov_h26::Value(menu_game_fov_target_.load());
    menu_game_fov_target_.store(linkedStartupFov);
    vr_source_vertical_fov_deg_.store(linkedStartupFov);
    vr_corrected_view_vfov_deg_.store(linkedStartupFov);
    startup_s15::setupLogSinkR131=+[](const char* line) noexcept {diagnostic_trace::Write(line);}; // R131
    startup_s15::FirstRun(); // desktop offer; next-launch settings only, bootstrap never waits

    KCD2VR_DIAG_LOG(Bug,info,"KCD2VR starting up");
    debug_trace::Write(KCD2VR_BUILD_MARKER);
    KCD2VR_DIAG_LOG(Bug,warning,"KCD2VR BUILD " KCD2VR_BUILD_MARKER);
    diagnostic_trace::Write("KCD2VR BUILD " KCD2VR_BUILD_MARKER);
    diagnostic_trace::Write("M2 SUBSYSTEM_CONTRACT base=KCD2VR-M2-C66-STATIC-2 modules=17 nativeLifecycles=HOST_OWNED contextPolling=NONE runtime=ONE_STATIC_ASI");
    {char identity32[176]{};std::snprintf(identity32,sizeof(identity32),
        "C32 PROCESS pid=%lu workerTid=%lu (separate starts may append to the same log)",GetCurrentProcessId(),GetCurrentThreadId());
        KCD2VR_DIAG_LOG(Bug,info,"{}",identity32);diagnostic_trace::Write(identity32);}
    // R59.16C7A: 1 ms system timer for this process (Win10 2004+ scopes it per process).
    // Without it every Sleep / condition-variable timeout rounds up to ~15.6 ms.
    g_timer_period_1ms_59c7a = timeBeginPeriod(1) == TIMERR_NOERROR;
    diagnostic_trace::Write(g_timer_period_1ms_59c7a ? "R59.16C7A FRAME_BUDGET timerResolution=1ms ok=1" : "R59.16C7A FRAME_BUDGET timerResolution=default ok=0");
    diagnostic_trace::Write("D48 HOTKEYS: F2=BUG_AND_HUD(1)/SELECTED_TIMING_COMPARISON(2)/ELBOW_SHOULDER_COMPARISON(3)/OFF; Shift+F2=PERFORMANCE_AND_MOTION_BURSTS; F3=COMBAT_CAPTURE; F4=RETIRED_HYBRID_IS_GAMEPLAY; F5=INTERACTION_CAPTURE(1)/PHYSICAL_GRIP_AND_SPATIAL_DOOR(2)/GUIDED_CRAFTING_AND_LEGACY_DOOR(3)/OFF; Shift+F7=VRIK_BASELINE(1)/FRESH_POSE_TIMING(2)/ELBOW_PREFERENCE(3)/OFF; F7=VR_MENU; F6=THEATER_IMMERSIVE_TOGGLE; F9=PHYSICS_CAPTURE_GAMEPLAY_UNCHANGED; F10=HAND_MODE_NO_AUTO_CAPTURE; F11=RELOAD_CAPTURE(1)/STAGED_RELOAD(2)/OFF; Shift+F8=CALIBRATION; experimentPreferences=LIVE_MENU; captureDurationMs=180000");
    diagnostic_trace::Write("V66.176 SCOPE: coherent physical bow/weapon/contact source repair; bow pull=CONFIGURED_PHYSICAL_GRIP_OR_TRIGGER autoLoad=NATIVE support=QUALIFIED_HILT completedPublication=REQUIRED freshnessLimits=UNCHANGED nativeRules=AUTHORITATIVE");
    diagnostic_trace::Write("V66.175 SCOPE: native XI and manual activity controls preserved; F4 third mode=VR_ATTEMPTS_NATIVE_CONSEQUENCES; F2/HUD selection independent");
    diagnostic_trace::Write("V66.175 SCOPE: F10=native boxing transport with tracked hands; F4_R3=EXPLICIT_COMPANION_ZONE3_NATIVE_EXCEPTION selectedKick=UNVERIFIED; F9=PHYSICAL_HELD_PROTOTYPE(defaultOFF,noSolverRpgAdapter); F5=BODY_VELOCITY_HOLD constraint=NONE");
    diagnostic_trace::Write("D57 RELOAD_SCOPE: F11=1 native reload recording; F11=2 existing bow experiment + qualified light/medium crossbow native-child gesture drive + staged rifle request; heavy crossbow crank binding=UNPROVEN; qualified bow/quiver default=VrManualBowDefault59; capture alone grants no gameplay ownership; universal manual reload success=NOT_CLAIMED");
    diagnostic_trace::Write("V66.163-HEAD-AIM RETIRED: immersive rotation=OPENXR; ranged RT never changes camera owner; native XI input transport retained");
    HMODULE loadedSelf124=nullptr;char loadedPath124[MAX_PATH*2]{};
    if(GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCSTR>(&GetLoadedModuleSiblingPath),&loadedSelf124)){
        const DWORD size=GetModuleFileNameA(loadedSelf124,loadedPath124,DWORD(sizeof(loadedPath124)));
        if(size&&size<sizeof(loadedPath124)){char line[MAX_PATH*2+48]{};std::snprintf(line,sizeof(line),"V66.124 LOADED_ASI path=%s",loadedPath124);KCD2VR_DIAG_LOG(Bug,info,"V66.124 LOADED_ASI path={}",loadedPath124);diagnostic_trace::Write(line);}
    }

    debug_trace::Write("KCD2VR starting up; debug path anchored to loaded ASI directory");

    // V61.18 retired diagnostics: fixed composition order and fixed 1:1 head translation.
    vr_rotation_order_.store(0, std::memory_order_relaxed);
    head_pose_scale_.store(1.0f, std::memory_order_relaxed);
    debug_trace::Write("KCD2VR starting up; debug path anchored to loaded ASI directory");

    if (!enabled_.load(std::memory_order_relaxed))
    {
        KCD2VR_DIAG_LOG(Bug,info,"KCD2VR disabled via config, exiting");
        return;
    }

    running_ = true;

    camera_hook_ = std::make_unique<CameraHook>();
    CameraHook::SetTrackingOptions(apply_head_rotation_.load(std::memory_order_relaxed),
                                   apply_head_position_.load(std::memory_order_relaxed));
    CameraHook::SetCameraCalibrationOffset(camera_side_offset_m_.load(std::memory_order_relaxed),
                                           camera_back_offset_m_.load(std::memory_order_relaxed),
                                           camera_height_offset_m_.load(std::memory_order_relaxed));
    debug_trace::Write("installing camera hook");
    if (!camera_hook_->Install())
    {
        KCD2VR_DIAG_LOG(Bug,error,"Camera hook failed to install - candidate ladder not resolved yet. See ROADMAP.md.");
        running_ = false;
        return;
    }

    debug_trace::Write("camera hook installed");
    const bool native_input_look_lab_installed = native_input_look_lab::Install();
    const bool head_aim_retail_verified=IsSupportedWhGameBuildV6143()&&retail_build112::Verify(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")));
    ActivityFade123R1 activityFade123{};
    focus_zoom58c2::Lease focusZoom58c2{};FocusZoomConsole58c2 focusZoomIO58c2{};
    ui_pause_blur124r1::Lease pauseBlur124;PauseBlurConsole124R1 pauseBlurIo124;
    bool pauseBlurWanted124=false;unsigned pauseBlurAttempts124=0;std::uint64_t pauseBlurNext124=0;
    int lastActivityFade123=-1;std::uint64_t retryActivityFade123=0;
    diagnostic_trace::Write(native_input_look_lab_installed ? "V64.33-NATIVE-INPUT hook retained for diagnostics; V66.38 F4 now owns stereo pair-pose timing" : "V64.33-NATIVE-INPUT hook unavailable; unrelated to V66.38 F4 pair-pose timing");
    if(native_input_look_lab_installed) {
        native_input_look_lab::SetGamepadMode(3);
        diagnostic_trace::Write("V66.96-INPUT native XI requested; Touch remains physical owner and VR chords are resolved before gamepad publication");
    }
    diagnostic_trace::Write("V66.96-INPUT target: Touch -> KCD2VR premap -> native XI; scan-code transport is fail-closed fallback only; right-grip weapon/bow free-look remains KCD2VR-owned");
    diagnostic_trace::Write("V65.04a FIXED VISION + F8 FRAME NEXUS: V65.04 projection/HUD/F2/F5/F9/controls retained unchanged; F8 broad DoFrame/GetView/recursive census replaced by focused read-only CCryAction frame-nexus -> camera handoff -> renderer correlation -> engine phase ladder; no second-render write in this build.");
    CameraHook::SetCullFrustumScale(1.0f); // V61.5: ignore stale INI cull scale
    device_hook_ = std::make_unique<DeviceHook>();
    const bool v6632_postaa_repair_installed=native_postaa_repair_v6632::Install();
    // V66.41: RIGHT-eye volumetric cloud histories get real GPU memory (retail allocates LEFT only).
    const bool v6641_cloud_right_installed=native_cloud_right_v6641::Install();
    // V66.48: V66.44/45 stage-skip hooks retired (skipping stages crashed); not installed.
    (void)native_svo_prev_per_eye_v6649::Install(); // V66.49 per-eye SVO previous camera (default ON)
    // V66.54: RIGHT eye gets its own SVO history + reprojection (needs V66.49 ON). F8 = A/B.
    (void)native_svo_right_v6654::Install();
    // V66.77: always-on TAA jitter probe removed (diagnostic only).
    (void)native_taa_pair_epoch_v6661::Install(); // V66.68: mode-owned; STEREO=ON, MONO=OFF; no hotkey
    (void)native_water_reflection_right_v6663::Install(); // V66.63 cheap per-eye water/cloud-reflection A/B history
    diagnostic_trace::Write(v6641_cloud_right_installed ?
        "V66.41-CLOUD-RIGHT hook armed (provisions on the next cloud update)" :
        "V66.41-CLOUD-RIGHT hook unavailable; RIGHT eye clouds stay retail (missing)");
    diagnostic_trace::Write(v6632_postaa_repair_installed ?
        "V66.33-POSTAA-RIGHT hook armed (if the game already resized, the post-FX policy kicks one Resize)" :
        "V66.33-POSTAA-RIGHT hook unavailable; production AA will stay SMAA1X fail-safe");
    debug_trace::Write("installing D3D12 device hook");
    if (!device_hook_->Install())
    {
        KCD2VR_DIAG_LOG(Bug,error,"Device hook failed to install - cannot capture the game's D3D12 device");
        native_postaa_repair_v6632::Uninstall();
        camera_hook_->Uninstall();
        camera_hook_.reset();
        running_ = false;
        return;
    }

    KCD2VR_DIAG_LOG(Bug,info,"Waiting for the game's D3D12 device/queue to be captured (Proceed to main menu or load a save)...");
    debug_trace::Write("waiting for D3D12 device + DIRECT queue capture");
    ID3D12Device* game_device = nullptr;
    ID3D12CommandQueue* game_queue = nullptr;
    bool captured = false;

    // Polling window extended up to 60 seconds to allow time to reach the main menu / load game
    for (int i = 0; i < 600 && running_; ++i)
    {
        if (device_hook_->TryGetDevice(&game_device, &game_queue))
        {
            captured = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!running_)
    {
        KCD2VR_DIAG_LOG(Bug,info,"Shutdown requested while waiting for device capture");
        device_hook_->Uninstall();
        device_hook_.reset();
        native_postaa_repair_v6632::Uninstall();
        native_input_look_lab::Uninstall();
        camera_hook_->Uninstall();
        camera_hook_.reset();
        return;
    }

    if (!captured)
    {
        KCD2VR_DIAG_LOG(Bug,error,"Timed out waiting for the game's D3D12 device to be captured. Make sure you reach the main menu.");
        device_hook_->Uninstall();
        device_hook_.reset();
        native_postaa_repair_v6632::Uninstall();
        camera_hook_->Uninstall();
        camera_hook_.reset();
        running_ = false;
        return;
    }

    debug_trace::Write("D3D12 device + DIRECT queue captured");
    if (enable_legacy_probes_.load(std::memory_order_relaxed)) {
        debug_trace::Write("V45-RECURSIVE legacy probes requested but suppressed: they overlap the ViewCamera publish hook used for frame identity");
    } else {
        debug_trace::Write("V45-LEAN legacy V29-V44 archaeology probes disabled; recursive-view proof only");
    }
    stereo_archaeology::detail::g_native_eye_write_enabled.store(
        enable_native_eye_write_.load(std::memory_order_relaxed), std::memory_order_release);

    if (!FrameSync::Install()) {
        debug_trace::Write("V45-PIPELINE install FAILED; forcing mono bridge for safety");
        vr_stereo_experiment_.store(0, std::memory_order_relaxed);
    }
    FrameSync::SetStereoMode(vr_stereo_experiment_.load(std::memory_order_relaxed));

    // V65.04a: old recursive-view F8 archaeology is retired; normal play and new F8 never install that broad observer.

    xr_manager_ = &XrManager::Instance();
    xr_manager_->SetDiagnosticRendering(diagnostic_rendering_.load(std::memory_order_relaxed));
    xr_manager_->SetActiveVrBridge(enable_vr_output_bridge_.load(std::memory_order_relaxed));

    // V65.17 startup presentation: preserve saved source geometry. Projective REMAP
    // (mode 1) uses that saved source FOV with zoom=1; pressing REMAP resyncs it to live cl_fov.
    const int startup_preset = std::clamp(vr_cyclopean_mode_.load(std::memory_order_relaxed),0,3);
    const V603ViewPreset startup_view = ViewPresetForMode(startup_preset);
    const int startup_present = startup_view.presentation_mode;
    const float startup_saved_source_fov = std::clamp(vr_source_vertical_fov_deg_.load(std::memory_order_relaxed),70.0f,140.0f);
    const float startup_saved_zoom = std::clamp(vr_zoom_.load(std::memory_order_relaxed),0.85f,1.20f);
    const float startup_vr_fov = std::clamp(startup_saved_source_fov,60.0f,140.0f); // user VR FOV
    const float startup_framing = EffectiveZoomForMode(startup_preset,startup_saved_zoom);
    const float startup_game_geometry = std::clamp(menu_game_fov_target_.load(std::memory_order_relaxed),70.0f,130.0f);
    const float startup_compositor_source_fov = startup_preset==1 ? startup_game_geometry : startup_vr_fov;
    const float startup_corrected_view_fov=startup_vr_fov;
    vr_corrected_source_vfov_deg_.store(startup_compositor_source_fov,std::memory_order_relaxed);
    vr_corrected_view_vfov_deg_.store(startup_corrected_view_fov,std::memory_order_relaxed);
    vr_corrected_zoom_.store(startup_preset==1?1.0f:kV6508CorrectedZoom,std::memory_order_relaxed);
    xr_manager_->SetCorrectedViewVerticalFov(startup_corrected_view_fov);
    const float startup_uv = startup_view.uv_binocular;

    vr_stereo_experiment_.store(0, std::memory_order_relaxed);
    vr_afr_ipd_scale_.store(0.0f, std::memory_order_relaxed);
    vr_presentation_mode_.store(startup_present, std::memory_order_relaxed);
    vr_zoom_.store(startup_saved_zoom, std::memory_order_relaxed);
    vr_source_vertical_fov_deg_.store(startup_saved_source_fov, std::memory_order_relaxed);
    vr_swap_eyes_.store(false, std::memory_order_relaxed);
    xr_manager_->SetPresentationSettings(
        vr_render_scale_.load(std::memory_order_relaxed), startup_present, startup_framing,
        0.0f,0.0f,0.0f,0.0f,startup_compositor_source_fov);
    xr_manager_->SetStereoExperiment(0, 0.0f);
    xr_manager_->SetAngularCorrectMono(startup_view.angular_correct);
    xr_manager_->SetPoseHonestyOverride(startup_view.pose_honesty);
    xr_manager_->SetMonoFovAsymmetryScale(startup_view.fov_asymmetry);
    xr_manager_->SetMonoUvReprojectionMode(MonoMapModeForCleanMode(startup_preset));
    xr_manager_->SetMonoUvBinocularOffset(startup_uv);
    xr_manager_->SetPresentationCenterX(startup_preset<=1 ? 0.0f : vr_presentation_center_x_.load(std::memory_order_relaxed));
    xr_manager_->SetPresentationCenterY(startup_preset==0 ? 0.0f : vr_presentation_center_y_.load(std::memory_order_relaxed));
    xr_manager_->SetAutoProjectionCenter(vr_auto_projection_center_.load(std::memory_order_relaxed));
    xr_manager_->SetPitchCorrectionDeg(vr_pitch_correction_deg_.load(std::memory_order_relaxed));
    xr_manager_->SetHudLag(hud_lag_on22_.load(std::memory_order_relaxed)?hud_lag22_.load(std::memory_order_relaxed):0.0f); // C22b
    xr_manager_->SetSwapEyes(false);
    xr_manager_->SetFlatUiScale(vr_flat_screen_scale_.load(std::memory_order_relaxed));
    vr_flat_ui_ipd_mm_.store(0.0f, std::memory_order_relaxed);
    xr_manager_->SetFlatUiIpdMm(0.0f);
    xr_manager_->SetPresentPosePairingMode(1);
    xr_manager_->SetStereoStabilityExperiment(false); // F2 rung0 starts from exact V66.26 presentation behavior
    (void)native_stereo_outer_lab::SetV6627RightPreparationExperiment(false);
    DeviceHook::SetV6627PerformanceQuiet(kV6634ProductionPerfQuiet);
    DeviceHook::SetAfrStereoCapture(false);
    FrameSync::SetStereoMode(0);
    CameraHook::SetAfrIpd(0.064f, 0.0f);
    CameraHook::SetRotationFilterMode(5); // V65.03b F2-OFF: exact V65.02b trusted fixed-8ms head baseline
    CameraHook::SetRotationOrder(vr_rotation_order_.load(std::memory_order_relaxed));
    // V61.30 baseline: DOF-focus pseudo is no longer a startup mode. It is a
    // deliberate F4 hypothesis test so GOOD MONO remains the uncontaminated baseline.
    fake_dof_active_.store(false, std::memory_order_release);
    engine_dof_active_.store(false, std::memory_order_release);
    engine_dof_available_.store(false, std::memory_order_release);
    engine_dof_focus_m_.store(0.0f, std::memory_order_release);
    fake_dof_started_ms_.store(0, std::memory_order_release);
    fake_dof_scale_.store(0.95f, std::memory_order_release);
    engine_dof_scale_.store(0.95f, std::memory_order_release);
    debug_trace::Write("V65.04-HEAD baseline=fixed8ms+adaptive35; F2 ownership fix never switches RAW/IMMEDIATE");
    { char msg[256]{}; std::snprintf(msg, sizeof(msg),
        "V40-STEREO config experiment=%d afrIpdScale=%.3f swapEyes=%s hotkeys=Insert(mode)/Home(swap)",
        vr_stereo_experiment_.load(), vr_afr_ipd_scale_.load(), vr_swap_eyes_.load()?"true":"false"); debug_trace::Write(msg); }
    { char msg[320]{}; std::snprintf(msg, sizeof(msg),
        "V44-PRESENT config renderScale=%.3f mode=%d zoom=%.3f sourceVFov=%.2f Loff=(%.3f,%.3f) Roff=(%.3f,%.3f)",
        vr_render_scale_.load(), vr_presentation_mode_.load(), vr_zoom_.load(), vr_source_vertical_fov_deg_.load(),
        vr_left_offset_x_.load(), vr_left_offset_y_.load(), vr_right_offset_x_.load(), vr_right_offset_y_.load());
      debug_trace::Write(msg); }

    debug_trace::Write("XR-OWNER-V20 single-source=XrManager::Instance; last-good stereo cache isolated from raw locate failures");
    debug_trace::Write("starting OpenXR initialization");
    if (!xr_manager_->EnsureInitialized(game_device, game_queue, &running_))
    {
        KCD2VR_DIAG_LOG(Bug,error,"OpenXR session initialization failed");
        // Stereo observers were installed immediately before OpenXR init.
        // Tear them down on this failure path as well, otherwise an XR init
        // failure leaves WHGame detours active after the worker exits.
        RecursiveViewProbe::Uninstall();
    FrameSync::Uninstall();
        stereo_archaeology::Shutdown();
        xr_manager_->Shutdown();
        xr_manager_ = nullptr; // singleton - clear the pointer, do not delete
        device_hook_->Uninstall();
        device_hook_.reset();
        camera_hook_->Uninstall();
        camera_hook_.reset();
        running_ = false;
        return;
    }

    // V65.40 clean bridge branch: wide stereo archaeology is intentionally NOT installed.
    // Prior static/runtime archaeology is already proven; keep only bridge-critical hooks.
    stereo_archaeology::detail::g_native_eye_write_enabled.store(false,std::memory_order_release);
    stereo_archaeology::SetWideCollectionArmed(false);
    diagnostic_trace::Write("D48 HOTKEYS: F2=BUG_AND_HUD(1)/SELECTED_TIMING_COMPARISON(2)/ELBOW_SHOULDER_COMPARISON(3)/OFF; Shift+F2=PERFORMANCE_AND_MOTION_BURSTS; F3=COMBAT_CAPTURE; F4=RETIRED_HYBRID_IS_GAMEPLAY; F5=INTERACTION_CAPTURE(1)/PHYSICAL_GRIP_AND_SPATIAL_DOOR(2)/GUIDED_CRAFTING_AND_LEGACY_DOOR(3)/OFF; Shift+F7=VRIK_BASELINE(1)/FRESH_POSE_TIMING(2)/ELBOW_PREFERENCE(3)/OFF; F7=VR_MENU; F6=THEATER_IMMERSIVE_TOGGLE; F9=PHYSICS_CAPTURE_GAMEPLAY_UNCHANGED; F10=HAND_MODE_NO_AUTO_CAPTURE; F11=RELOAD_CAPTURE(1)/STAGED_RELOAD(2)/OFF; Shift+F8=CALIBRATION; experimentPreferences=LIVE_MENU; captureDurationMs=180000");

    // V65.11: V65.10's XINPUT9_1_0 IAT detour is RETIRED. The Sept-13
    // headset-proven transport is KCD2's internal XI dispatcher and is reachable
    // only behind the explicit F11 rung 1 gate. Production starts on the proven
    // keyboard/contextual TouchBindings path.
    diagnostic_trace::Write("V66.89 INPUT transport preserved; F3/F5/F10/F12 own body/weapon diagnostics, not renderer effects");

    // V63.1: DO NOT write r_Width/r_Height at startup. Source resolution is
    // written to game-root user.cfg from the Graphics page and takes effect on
    // the next KCD2 launch before the D3D12/OpenXR bridge exists.
    diagnostic_trace::Write("V63.1-SOURCE-RES startup live apply disabled; KCD2/user.cfg owns renderer creation");

    // V65.43a: startup FPS retry state must outlive the startup configuration block.
    // Keep it function-local to Mod::Run(), but in the enclosing scope used by the main loop.
    bool startup_fps_uncap_confirmed=false;
    std::uint64_t startup_fps_retry_after_ms=0;
    std::uint64_t startup_fps_watchdog_after_ms=0;

    // H26: apply the shared selection at startup; bounded confirmation below
    // handles late native profile loading without a permanent FOV writer.
    {
      const int startup_mode=std::clamp(vr_cyclopean_mode_.load(std::memory_order_acquire),0,3);
      const float startup_game_fov=std::clamp(menu_game_fov_target_.load(std::memory_order_acquire),70.0f,130.0f);
      const float startup_source_fov=EffectiveSourceFovForMode(startup_mode,
          std::clamp(vr_source_vertical_fov_deg_.load(std::memory_order_acquire),70.0f,140.0f));
      const float startup_zoom=EffectiveZoomForMode(startup_mode,
          std::clamp(vr_zoom_.load(std::memory_order_acquire),0.85f,1.20f));
      const bool startup_fov_write_ok=ApplyNativeGameFov(startup_game_fov);
      if(startup_mode==1) xr_manager_->SetCorrectedViewVerticalFov(vr_corrected_view_vfov_deg_.load(std::memory_order_acquire));
      xr_manager_->SetPresentationSettings(
          vr_render_scale_.load(std::memory_order_relaxed),
          vr_presentation_mode_.load(std::memory_order_relaxed),
          startup_zoom,
          0.0f,0.0f,0.0f,0.0f,startup_source_fov);

      // Saved F2 state is applied only after XR is live. It enables the renderer
      // matcher/write-side token pairing but preserves fixed8ms + adaptive35.
      // V65.14: preserve the persisted Auto HUD toggle; never force it back ON at XR startup.
      vr_corrected_hud_dirty_.store(true,std::memory_order_release);
      f2_ownership_fix_.store(true,std::memory_order_release);
      FrameSync::SetHeadOwnershipFixEnabled(true);
      FrameSync::SetHeadOwnershipProbeLevel(3);
      (void)FrameSync::SetPipelineProbeEnabled(true);
      CameraHook::SetAngularDiagnosticsEnabled(false);
      CameraHook::SetRotationFilterMode(5);
      xr_manager_->SetPresentPacingMode(0);

      const float hf=ReadCurrentHorizontalFovValue();
      if(hf>1.0f) menu_horizontal_fov_target_.store(std::clamp(hf,70.0f,170.0f),std::memory_order_release);

      // V65.37: KCD2's startup cap is frequently 60 and is disruptive in VR.
      // Always start uncapped (sys_MaxFPS=0). The in-VR menu can still opt into
      // the existing simple manual limiter afterward.
      const float current_fps=ReadCurrentGameFpsCap();
      startup_original_fps_cap_.store(current_fps,std::memory_order_release);
      menu_fps_cap_enabled_.store(false,std::memory_order_release);
      startup_fps_uncap_confirmed=DisableNativeGameFpsCap();
      (void)DisableNativeGameVsync();
      startup_fps_retry_after_ms=0;

      const float startup_merged=std::clamp(menu_merged_mesh_target_.load(std::memory_order_acquire),30.0f,300.0f);
      if(ApplyMergedMeshesDistance(startup_merged)) {
          const float merged=ReadMergedMeshesDistance();
          if(merged>1.0f) menu_merged_mesh_target_.store(std::clamp(merged,30.0f,300.0f),std::memory_order_release);
      }
      // V63.0: compositor-side color controls persist in KCD2VR.ini. Never read/write
      // the crash-prone retail r_Gamma/r_Brightness/r_Contrast setter path.
      const float startup_gamma=std::clamp(vr_gamma_.load(std::memory_order_acquire),0.50f,2.00f);
      const float startup_brightness=std::clamp(vr_brightness_.load(std::memory_order_acquire),-0.50f,0.50f);
      const float startup_contrast=std::clamp(vr_contrast_.load(std::memory_order_acquire),0.50f,2.00f);
      if(xr_manager_) xr_manager_->SetColorAdjustment(startup_gamma,startup_brightness,startup_contrast);
      char color_start[192]{}; std::snprintf(color_start,sizeof(color_start),
          "V63.0-XR-COLOR startup gamma=%.2f brightness=%+.2f contrast=%.2f persisted=true",
          double(startup_gamma),double(startup_brightness),double(startup_contrast));
      debug_trace::Write(color_start);

      char m[640]{};
      std::snprintf(m,sizeof(m),
          "H28R1 FOV_START stage=INITIAL selectedGame=%.3f selectedVR=%.3f nativeReadback=%.3f compositorSource=%.3f correctedView=%.3f zoom=%.3f writeReturned=%d cyclo=%d measurement=CVAR_AND_COMPOSITOR_SETTINGS renderedLens=UNVERIFIED",
          double(startup_game_fov),double(vr_source_vertical_fov_deg_.load()),double(ReadCurrentGameFovValue()),
          double(xr_manager_->GetSourceVerticalFov()),double(xr_manager_->GetCorrectedViewVerticalFov()),
          double(startup_zoom),int(startup_fov_write_ok),startup_mode);
      KCD2VR_DIAG_LOG(Bug,info,"{}",m);
      diagnostic_trace::Write(m);
    }

    // V46.2 in-backbuffer menu. It shares the game's captured D3D12 device and
    // never creates a second graphics device. Input is chained through the
    // game's HWND using SetWindowSubclass so other overlays can coexist.
    if (HWND game_window = FindMainGameWindow()) {
        vr_menu_ = std::make_unique<VrMenu>();
        VrMenu::Callbacks menu_callbacks{};
        menu_callbacks.context = this;
        menu_callbacks.read_state = [](void* ctx, VrMenuState* out) {
            if (ctx && out) static_cast<Mod*>(ctx)->FillMenuState(*out);
        };
        menu_callbacks.apply_state = [](void* ctx, const VrMenuState* state) -> bool {
            return ctx && state && static_cast<Mod*>(ctx)->ApplyMenuState(*state);
        };
        menu_callbacks.request_matrix = [](void* ctx) {
            if (ctx) static_cast<Mod*>(ctx)->matrix_test_requested_.store(true, std::memory_order_release);
        };
        menu_callbacks.request_safe_mono = [](void* ctx) {
            if (ctx) static_cast<Mod*>(ctx)->safe_mono_requested_.store(true, std::memory_order_release);
        };
        menu_callbacks.save_settings = [](void* ctx) -> bool {
            auto* self=static_cast<Mod*>(ctx); if(!self) return false;
            self->RequestMenuSave(true); // C25: off the XR loop; worker logs V63.1-CONFIG + beeps on success
            return true;
        };

        menu_callbacks.request_record = [](void* ctx, int category) {
            if (ctx) static_cast<Mod*>(ctx)->record_request_.store(category, std::memory_order_release);
        };
        menu_callbacks.request_resolution_recreate = [](void* ctx, float render_scale) {
            auto* self = static_cast<Mod*>(ctx);
            if (self && self->xr_manager_) self->xr_manager_->RequestSwapchainRecreate(render_scale);
        };
        menu_callbacks.apply_custom_resolution = [](void* ctx, int width, int height) -> bool {
            auto* self=static_cast<Mod*>(ctx); if(!self) return false;
            const bool cfg_ok=ApplyMenuCustomResolution(width,height);
            if(cfg_ok){
                self->vr_source_width_.store(width,std::memory_order_release);
                self->vr_source_height_.store(height,std::memory_order_release);
                self->RequestMenuSave(false); // C25: INI result is logged by the worker (C25 CONFIG_SAVE)
                char m[384]{}; std::snprintf(m,sizeof(m),
                    "V63.1-SOURCE-RES selected=%dx%d user.cfg=ok KCD2VR.ini=QUEUED restartRequired=true liveRendererWrite=false",
                    width,height);
                debug_trace::Write(m);
      diagnostic_trace::Write(m);
                diagnostic_trace::Write(m);
                return true;
            }
            return false;
        };
        // V62.4: old generic MasterSwitch/SetSubtitleSize experiments stay retired.
        // The visible UI page drives declared KCD2 MovieClips directly.
        menu_callbacks.apply_hud_scale = nullptr;
        menu_callbacks.apply_dialogue_text_scale = nullptr;
        menu_callbacks.apply_vr_ui_layout = [](void* ctx, const VrMenuState* state) -> bool {
            if(!state) return false;
            // V64.20: centered angular HUD is the single HUD positioning authority.
            // Old per-element free positioning is retired. The only direct menu
            // transform retained here is the targeted oversized cooking/ingredient
            // /inventory-list panel experiment.
            // V64.86: separate Alchemy/Menu tuning retired; if this legacy target appears,
            // keep it aligned with the proven Item Panels defaults/settings.
            auto* self=static_cast<Mod*>(ctx);
            // UI1 preflight: a HUD setting must not move APSE, AlchemyBook or
            // ItemSelection/ItemTransfer using the retired large-panel experiment.
            // Preserve settings, but let the final-flat menu keep its native layout.
            const bool large=false;
            const bool items=false;
            if(self) self->vr_hud_reapply_requested_.store(true,std::memory_order_release);
            char ui_diag[640]{};
            std::snprintf(ui_diag,sizeof(ui_diag),
                "V64.51-UI LIVE centeredSize=%.2f comfort=%.2f offset=(%+.0f,%+.0f) reapply=1 largeMenu={s:%.2f x:%+.0f y:%+.0f applied:%d} itemPanels={s:%.2f x:%+.0f y:%+.0f applied:%d}",
                state->vr_ui_compactness,state->vr_ui_subtitle_scale,state->vr_ui_center_offset_x,state->vr_ui_center_offset_y,
                state->vr_ui_item_panel_scale,state->vr_ui_item_panel_x,state->vr_ui_item_panel_y,large?1:0,
                state->vr_ui_item_panel_scale,state->vr_ui_item_panel_x,state->vr_ui_item_panel_y,items?1:0);
            diagnostic_trace::Write(ui_diag);
            return true;
        };

        menu_callbacks.request_vrik_cycle = [](void* ctx) {
            auto* self=static_cast<Mod*>(ctx);
            if(self) self->vrik_cycle_requested_.store(true,std::memory_order_release);
        };
        menu_callbacks.set_vrik_mode = [](void* ctx, bool enabled) {
            // V64.16: defer the request to the main loop. This keeps the VRIK
            // observer hooks physically uninstalled until an explicit lab action.
            auto* self=static_cast<Mod*>(ctx);
            if(self) self->vrik_menu_mode_requested_.store(enabled ? 2 : 0,std::memory_order_release);
        };
        menu_callbacks.request_vrik_recalibrate = [](void* ctx) {
            auto* self=static_cast<Mod*>(ctx);
            if(self) self->vrik_recalibrate_requested_.store(true,std::memory_order_release);
        };
        frik_pose_probe::SetTrackingOriginProbeH46(&CameraHook::ReadTrackingOrigin125);
        frik_pose_probe::SetViewPositionProbe43(&CameraHook::GetLatestVisualPositionWorld); // C43 receipt: view vs Henry's head at owner exits
        menu_callbacks.request_vrik_restore = [](void* ctx) { // C43 RESTORE CALIBRATION
            auto* self=static_cast<Mod*>(ctx);
            if(self) self->vrik_restore_requested_.store(true,std::memory_order_release);
        };

        menu_callbacks.apply_game_fov = [](void* ctx, float value) {
            auto* self=static_cast<Mod*>(ctx); if(!self) return;
            VrMenuState linked{};self->FillMenuState(linked);
            linked.game_fov_target=kcd2vr::linked_fov_h26::Value(value);
            linked.source_vertical_fov_deg=linked.game_fov_target;
            (void)self->ApplyMenuState(linked);
            (void)ApplyNativeGameFov(linked.game_fov_target);
        };
        menu_callbacks.recalculate_projection = [](void* ctx) -> bool {
            auto* self=static_cast<Mod*>(ctx); return self && self->RecalculateCorrectedProjection();
        };
        menu_callbacks.center_projection = [](void* ctx) -> bool {
            auto* self=static_cast<Mod*>(ctx); return self && self->CenterCorrectedProjection();
        };
        menu_callbacks.apply_horizontal_fov = [](void* ctx, float value) {
            auto* self=static_cast<Mod*>(ctx); if(!self) return;
            if(ApplyNativeHorizontalFov(value)) {
                const float confirmed=ReadCurrentHorizontalFovValue();
                if(confirmed>1.0f) self->menu_horizontal_fov_target_.store(confirmed,std::memory_order_release);
            }
        };
        menu_callbacks.apply_fps_cap = [](void* ctx, float value) {
            auto* self=static_cast<Mod*>(ctx); if(!self) return;
            if(value>1.0f) {
                const float target=std::clamp(value,60.0f,120.0f);
                if(ApplyNativeGameFps(target)) {
                    self->menu_fps_cap_enabled_.store(true,std::memory_order_release);
                    self->menu_fps_cap_target_.store(target,std::memory_order_release);
                    debug_trace::Write("V65.03b-FPS manual limiter ON; simple sys_MaxFPS only");
                }
            } else {
                const bool ok=DisableNativeGameFpsCap();
                self->menu_fps_cap_enabled_.store(false,std::memory_order_release);
                char m[192]{}; std::snprintf(m,sizeof(m),"V65.37-FPS manual limiter OFF -> uncapped sys_MaxFPS=0 ok=%s",ok?"true":"false"); debug_trace::Write(m);
            }
        };
        menu_callbacks.apply_merged_mesh = [](void* ctx, float value) {
            auto* self=static_cast<Mod*>(ctx); if(!self) return;
            if(ApplyMergedMeshesDistance(value)) self->menu_merged_mesh_target_.store(ReadMergedMeshesDistance(),std::memory_order_release);
        };
        // R27 Graphics: native KCD2 upscaler selection, saved only on explicit Save for Restart.
        menu_callbacks.read_graphics_preset=[](void*) -> int {
            void* c=SafeGetCryConsole();void* v=c?SafeConsoleGetCVar(c,"sys_spec_Full"):nullptr;
            const int preset=v?SafeCVarGetInt(v,-1):-1;
            std::vector<std::pair<std::string,int>> values;
            if(preset<1||preset>5||!startup_graphics_d80::settings(-1,0,preset,values))return -1;
            for(const auto& group:values){void* cv=SafeConsoleGetCVar(c,group.first.c_str());if(!cv||SafeCVarGetInt(cv,-1)!=preset)return -1;}
            return preset;
        };
        menu_callbacks.apply_graphics_preset=[](void*,int preset) -> bool {
            std::vector<std::pair<std::string,int>> values;
            if(preset<1||preset>5||!startup_graphics_d80::settings(-1,0,preset,values))return false;
            void* c=SafeGetCryConsole();if(!c)return false;
            // Resolve all groups first. Never silently claim a partial preset.
            std::vector<void*> vars;std::vector<int> previous;
            for(const auto& v:values){void* cv=SafeConsoleGetCVar(c,v.first.c_str());if(!cv)return false;vars.push_back(cv);previous.push_back(SafeCVarGetInt(cv,-1));}
            bool ok=true;
            for(std::size_t i=0;i<vars.size();++i)ok=SafeCVarSetInt(vars[i],values[i].second)&&ok;
            for(std::size_t i=0;i<vars.size();++i)ok=SafeCVarGetInt(vars[i],-1)==values[i].second&&ok;
            if(ok)ok=SaveGraphicsSettingsToUserCfgR27(values);
            if(!ok)for(std::size_t i=0;i<vars.size();++i)(void)SafeCVarSetInt(vars[i],previous[i]);
            return ok;
        };
        menu_callbacks.read_upscaler = [](void*, int* tech, int* quality, int* preset) -> bool {
            void* c=SafeGetCryConsole(); if(!c||!tech||!quality||!preset) return false;
            void* m=SafeConsoleGetCVar(c,"r_SuperResolution_Mode"); if(!m) return false;
            const int mode=SafeCVarGetInt(m,-1);if(!graphics_menu_options::Supported(mode))return false;
            *tech=mode;const int fallback=graphics_menu_options::DefaultQuality(mode);
            const char* qualityKey=graphics_menu_options::QualityKey(mode);
            void* q=qualityKey?SafeConsoleGetCVar(c,qualityKey):nullptr;
            *quality=q?SafeCVarGetInt(q,fallback):fallback;
            void* pr=SafeConsoleGetCVar(c,"r_SuperResolution_NVIDIA_DLSS_Preset"); *preset=pr?SafeCVarGetInt(pr,0):0;
            return true;
        };
        menu_callbacks.stereo_ready = [](void* ctx) -> bool {
            if(!ctx)return false;
            void* con=SafeGetCryConsole();
            void* sr=con?SafeConsoleGetCVar(con,"r_SuperResolution_Mode"):nullptr;
            const int actual=sr?SafeCVarGetInt(sr,-1):-1;
            return stereo_upscaler_d81r2::canKeep(3,actual>=0,actual)&&native_upscaler_stereo_d82::CanEnterStereo(actual);
        };
        menu_callbacks.apply_upscaler = [](void* ctx, int tech, int quality, int preset) -> bool {
            const auto* self=static_cast<const Mod*>(ctx);
            if(!self||!stereo_upscaler_d81r2::canSelect(
                self->vr_production_stereo_mode_.load(std::memory_order_acquire),tech))return false;
            // R27: the renderer creates its temporal resources on restart. The old
            // live CVar-only switch neither recreated a qualified per-eye context
            // nor persisted past an older user.cfg mode=0. Save the explicit choice
            // atomically, just like source resolution; leave this session intact.
            std::vector<std::pair<std::string,int>> values;
            const bool valid=kcd2vr::graphics_owner31::upscalerSettingsR27({tech,quality,preset},values);
            const bool ok=valid&&SaveGraphicsSettingsToUserCfgR27(values);
            // D85 UPSCALERS: also switch live. The worker loop stops stereo, writes
            // the same values and waits for the game's own context creation.
            if(ok)upscaler_live_switch_d85::Instance().Request({tech,quality,preset});
            char msg[320]{};std::snprintf(msg,sizeof(msg),"R27 UPSCALER_SAVE tech=%d quality=%d preset=%d result=%s savedForRestart=1 liveSwitch=%s",tech,quality,preset,ok?"SAVED":"FAILED",ok?"REQUESTED":"NONE");diagnostic_trace::Write(msg);
            return ok;
        };
        menu_callbacks.apply_graphics_color = [](void* ctx, float gamma, float brightness, float contrast) -> bool {
            auto* self=static_cast<Mod*>(ctx); if(!self || !self->xr_manager_) return false;
            gamma=std::clamp(gamma,0.50f,2.00f);
            brightness=std::clamp(brightness,-0.50f,0.50f);
            contrast=std::clamp(contrast,0.50f,2.00f);
            self->vr_gamma_.store(gamma,std::memory_order_release);
            self->vr_brightness_.store(brightness,std::memory_order_release);
            self->vr_contrast_.store(contrast,std::memory_order_release);
            self->xr_manager_->SetColorAdjustment(gamma,brightness,contrast);
            char m[256]{}; std::snprintf(m,sizeof(m),
                "V63.0-XR-COLOR gamma=%.2f brightness=%+.2f contrast=%.2f compositorOnly=true",
                double(gamma),double(brightness),double(contrast));
            diagnostic_trace::Write(m);
            return true;
        };
        menu_callbacks.apply_hud_visibility = [](void* ctx, bool crosshair, bool lower_icons, bool compass) -> bool {
            auto* self=static_cast<Mod*>(ctx); if(!self) return false;
            self->hud_crosshair_visible_.store(crosshair,std::memory_order_release);
            self->hud_lower_icons_visible_.store(lower_icons,std::memory_order_release);
            self->hud_compass_visible_.store(compass,std::memory_order_release);
            return true; // worker applies the requested transition once
        };
        menu_callbacks.request_recenter = [](void* ctx) {
            (void)ctx;
            CameraHook::RequestRecenter();
            debug_trace::Write("V66.100-RECENTER origin only; saved arm reach and finger profiles preserved; no T-pose request");
        };
        menu_callbacks.toggle_fake_dof = [](void* ctx) {
            auto* self=static_cast<Mod*>(ctx); if(!self) return;
            const bool on=!self->fake_dof_active_.load(std::memory_order_acquire);
            if(on) self->engine_dof_active_.store(false,std::memory_order_release);
            self->fake_dof_active_.store(on,std::memory_order_release);
            self->fake_dof_started_ms_.store(on?static_cast<std::uint64_t>(GetTickCount64()):0ull,std::memory_order_release);
            self->fake_dof_scale_.store(on?0.80f:CyclopeanAsymmetryForMode(self->vr_cyclopean_mode_.load(std::memory_order_acquire)),std::memory_order_release);
            if(on) self->vr_cyclopean_mode_.store(3,std::memory_order_release);
            debug_trace::Write(on?"V61.16-FAKE-DOF ON exaggerated visibility test .70 -> .85 -> 1.00":"V61.16-FAKE-DOF OFF restored selected pseudo preset");
        };
        menu_callbacks.toggle_engine_dof = [](void* ctx) {
            auto* self=static_cast<Mod*>(ctx); if(!self) return;
            const bool on=!self->engine_dof_active_.load(std::memory_order_acquire);
            if(on) {
                self->fake_dof_active_.store(false,std::memory_order_release);
                self->fake_dof_started_ms_.store(0,std::memory_order_release);
                self->vr_cyclopean_mode_.store(3,std::memory_order_release);
            }
            self->engine_dof_active_.store(on,std::memory_order_release);
            debug_trace::Write(on?"V61.12-ENGINE-DOF ON; read-only KCD2 focus-distance adaptive pseudo requested":"V61.12-ENGINE-DOF OFF; restored selected pseudo preset");
        };
        menu_callbacks.write_hud_xml_mod = [](void* ctx) -> bool {
            auto* self=static_cast<Mod*>(ctx); if(!self) return false;
            hud_xml_mod::Settings hs{};
            hs.scale=self->hud_xml_scale_.load(std::memory_order_acquire);
            hs.offset_x=self->hud_xml_offset_x_.load(std::memory_order_acquire);
            hs.offset_y=self->hud_xml_offset_y_.load(std::memory_order_acquire);
            return hud_xml_mod::WriteRuntimeHudMod(hs);
        };
        menu_callbacks.reset_hud_xml_mod = [](void* ctx) -> bool {
            auto* self=static_cast<Mod*>(ctx); if(!self) return false;
            self->hud_xml_scale_.store(1.0f,std::memory_order_release);
            self->hud_xml_offset_x_.store(0,std::memory_order_release);
            self->hud_xml_offset_y_.store(0,std::memory_order_release);
            return hud_xml_mod::ResetRuntimeHudMod();
        };
        if (!vr_menu_->Initialize(game_device, game_window, menu_callbacks)) {
            debug_trace::Write("V46.2-MENU initialization failed; VR continues with hotkeys only");
            vr_menu_.reset();
        }
    } else {
        debug_trace::Write("V46.2-MENU game HWND not found; VR continues with hotkeys only");
    }

    // Install the callback only after XrManager and the menu are initialized.
    // Menu rendering deliberately runs before the XR blit in OnRenderTick.
    DeviceHook::SetRenderTickCallback(&Mod::OnRenderTick);

    KCD2VR_DIAG_LOG(Bug,info,"KCD2VR running - head tracking active; render-thread handoff armed");
    debug_trace::Write("OpenXR initialized; render-thread handoff armed; entering tracking loop");
    debug_trace::Write("V61.38 BUILD ID RELEASE-RECOVERY | FPS=USER | FOV=95/95 MATCHED START | 0=OLD-MONO | 1=CORRECTED-MONO | 2=PSEUDO-.90 | 3=DEPTH-PSEUDO-near-.70->.55-far-1.00 | DIALOG-ZOOM-RETAIN=.30 | HUD/SUBTITLE=SESSION-PROBE | CULL=V61.12");
    debug_trace::Write("V61.27 combined: Touch-ray menu cursor fixed; menu 20% smaller; HUD XML viewport sliders retired; final-skinning probe read-only; no native stereo writes");

    struct MatrixPhase {
        const char* name; int kind; int pose_source; int rotation_order; int filter_mode; float prediction_offset_ms; std::uint64_t duration_ms;
        // V58.9 mono-focused F8 ladder. We intentionally retired the long
        // tracking/native-stereo ladder after V58.8 showed no useful visual
        // improvement and proved the tagged PRESENT path collapses to one eye.
        int crop_mode = -1;      // -1 unchanged, 0 legacy safety clamp, 1 angular/no-clamp
        int pose_mode = -1;      // -1 unchanged, 0 old fake-separated, 1 honest common origin
        int presentation_mode = -1; // -1 unchanged, 3 OpenXR tangent remap, 2 direct aspect-fill copy
        float source_vfov = 0.0f;   // >0 fixed test VFOV; 0=current published/game source
        float zoom = 1.0f;
        float mono_fov_asym = 1.0f; // submitted XrFovf asymmetry scale
        int pairing_mode = 0;      // 0=current XR pose, 1=PRESENT/render-paired pose
        int uv_reproj_mode = 0;    // 0=direct copy, 1=rotation-only ray reprojection
        float uv_binocular = 0.0f; // constant opposite-sign source UV x shift
        int bottom_edge_mode = 0; // V59.2: 0 black ref, 1 shift, 2 Y compress, 3/4 tail remap, 5 edge hold
        int stereo_test = 0;        // V61.0: 0 mono baseline, 1 AFR, 2 native dual-renderer diagnostic
        float stereo_ipd_scale = 0.0f;
    };
    const MatrixPhase matrix_phases[] = {
        // V61.0 stereo ladder at the current game/source FOV and current high-quality source raster.
        // A-D: frame-locked AFR with one shared base head pose and only +/- eye translation.
        // E-F: CryEngine stereo CVars + Normal CRenderView dual-pass selector diagnostic.
        // All phases use matched-B tangent mapping, no pseudo UV disparity, no temporal reprojection.
        { "A610_AFR_IPD025", 0,0,0,5,0.0f,8000ull, 1,1,3,0.0f, 1.00f,1.00f,1,4,0.0f,0, 1,0.25f },
        { "B610_AFR_IPD050", 0,0,0,5,0.0f,8000ull, 1,1,3,0.0f, 1.00f,1.00f,1,4,0.0f,0, 1,0.50f },
        { "C610_AFR_IPD075", 0,0,0,5,0.0f,8000ull, 1,1,3,0.0f, 1.00f,1.00f,1,4,0.0f,0, 1,0.75f },
        { "D610_AFR_IPD100", 0,0,0,5,0.0f,8000ull, 1,1,3,0.0f, 1.00f,1.00f,1,4,0.0f,0, 1,1.00f },
        { "E610_DUAL_IPD050",0,0,0,5,0.0f,8000ull, 1,1,3,0.0f, 1.00f,1.00f,1,4,0.0f,0, 2,0.50f },
        { "F610_DUAL_IPD100",0,0,0,5,0.0f,8000ull, 1,1,3,0.0f, 1.00f,1.00f,1,4,0.0f,0, 2,1.00f }
    };
    constexpr int kMatrixPhaseCount = static_cast<int>(sizeof(matrix_phases) / sizeof(matrix_phases[0]));

    struct MatrixCounters {
        XrManager::TestCounters xr{};
        std::uint64_t present = 0;
        std::uint64_t recursive_requests = 0;
        std::uint64_t recursive_passes = 0;
        std::uint64_t normal_pipeline = 0;
        std::uint64_t recursive_pipeline = 0;
        std::uint64_t recursive_barriers = 0;
        std::uint64_t recursive_rt = 0;
        std::uint64_t customres_barriers = 0;
        std::uint64_t customres_rt = 0;
        std::uint64_t published_fov_samples = 0;
        std::uint64_t scene_rt = 0;
        std::uint64_t scene_left = 0;
        std::uint64_t scene_right = 0;
    };
    auto capture_matrix_counters = [&]() -> MatrixCounters {
        MatrixCounters c{};
        c.xr = xr_manager_->GetTestCounters();
        c.present = DeviceHook::PresentCaptureCount();
        c.recursive_requests = RecursiveViewProbe::RecursiveRequestCount();
        c.recursive_passes = RecursiveViewProbe::RecursivePassCount();
        c.normal_pipeline = FrameSync::PipelineCount(0);
        c.recursive_pipeline = FrameSync::PipelineCount(1);
        c.recursive_barriers = DeviceHook::RecursiveBarrierCount();
        c.recursive_rt = DeviceHook::RecursiveRenderTargetBarrierCount();
        c.customres_barriers = DeviceHook::CustomResolutionBarrierCount();
        c.customres_rt = DeviceHook::CustomResolutionRenderTargetBarrierCount();
        c.published_fov_samples = FrameSync::PublishedFovSampleCount();
        c.scene_rt = DeviceHook::SceneColorProbeBarrierCount();
        c.scene_left = DeviceHook::SceneColorProbeLeftCount();
        c.scene_right = DeviceHook::SceneColorProbeRightCount();
        return c;
    };
    auto delta_u64 = [](std::uint64_t a, std::uint64_t b) -> std::uint64_t { return a >= b ? (a - b) : 0; };

    auto latest_present_size = [&]() -> std::pair<std::uint64_t, std::uint32_t> {
        ID3D12Resource* r = nullptr;
        if (!DeviceHook::TryGetLatestPresentResource(&r) || !r) return {0, 0};
        const auto d = r->GetDesc();
        r->Release();
        return { static_cast<std::uint64_t>(d.Width), static_cast<std::uint32_t>(d.Height) };
    };

    bool matrix_active = false;
    int matrix_phase = -1;
    std::uint64_t matrix_phase_start_ms = 0;
    MatrixCounters matrix_phase_start{};
    MatrixCounters matrix_cycle_start{};
    EngineCVarSnapshot matrix_original_cvars{};
    FpsCVarSnapshot matrix_original_fps{};
    bool matrix_phase_cvar_ok = false;
    V54StereoCVarSnapshot v54_stereo_original{};
    std::uint64_t v54_reentry_start=0;
    bool matrix_phase_fps_ok = false;
    float matrix_phase_target_fps = 0.0f;
    float matrix_restore_fov = 100.0f;
    float matrix_restore_framing = 1.00f;
    float matrix_restore_cull_scale = 1.0f;
    std::uint64_t boxing_last_sample_ms = 0;
    std::uint32_t boxing_sample_index = 0;
    int boxing_countdown_last_second = -1;
    bool boxing_mouse_l_prev = false;
    bool boxing_mouse_r_prev = false;
    std::uint64_t vrik_last_status_ms = 0; // V61.50: control-thread-only sparse ownership telemetry

    auto apply_matrix_phase = [&](int index) {
        const MatrixPhase& ph = matrix_phases[index];
        matrix_phase_start = capture_matrix_counters();
        CameraHook::ResetWorstCaseAngular();
        xr_manager_->SetFlatUiMode(false); xr_manager_->SetLatePoseEnabled(false);
        CameraHook::SetInjectionMode(1); CameraHook::SetTrackingOptions(true,true);
        CameraHook::SetPoseSourceMode(ph.pose_source,ph.prediction_offset_ms); CameraHook::SetRotationOrder(ph.rotation_order); CameraHook::SetRotationFilterMode(ph.filter_mode);
        xr_manager_->SetPresentPosePairingMode(ph.pairing_mode); xr_manager_->SetProjectionPoseMode(0); xr_manager_->SetSwapEyes(false); xr_manager_->SetPresentationTestMarker(false);
        xr_manager_->SetStereoExperiment(0,0.0f); DeviceHook::SetAfrStereoCapture(false); FrameSync::SetStereoMode(0); CameraHook::SetAfrIpd(0.064f,0.0f);
        FrameSync::SetNativeStereoReentryMode(0,0.064f); FrameSync::SetDenseRotationProbe(false);
        DeviceHook::SetSceneColorProbe(false,0,0);
        // F8 ladder addition: crop_mode/pose_mode, -1 = leave whatever was
        // already configured (so the existing 6 tracking-only phases above
        // are completely unaffected by this addition).
        if (ph.crop_mode >= 0) xr_manager_->SetAngularCorrectMono(ph.crop_mode == 1);
        xr_manager_->SetPoseHonestyOverride(ph.pose_mode);
        xr_manager_->SetMonoFovAsymmetryScale(ph.mono_fov_asym);
        xr_manager_->SetMonoUvReprojectionMode(ph.uv_reproj_mode);
        xr_manager_->SetMonoUvBinocularOffset(ph.uv_binocular);
        xr_manager_->SetMonoBottomEdgeMode(ph.bottom_edge_mode);
        xr_manager_->ResetClampStats();
        // V58.9 phase-local presentation override. Mode 3 is the historical
        // tangent remap; mode 2 is an identical aspect-fill mono copy.
        if (ph.presentation_mode >= 0) {
            float phase_vfov = ph.source_vfov;
            if (ph.source_vfov > 0.0f) {
                const bool fov_ok = ApplyNativeGameFov(ph.source_vfov);
                const float confirmed = ReadCurrentGameFovValue();
                if (confirmed > 1.0f) {
                    phase_vfov = confirmed;
                    vr_source_vertical_fov_deg_.store(confirmed, std::memory_order_release);
                    menu_game_fov_target_.store(confirmed, std::memory_order_release);
                }
                char fm[320]{}; std::snprintf(fm,sizeof(fm),
                    "V60.7-FIXED-FOV target=%.1f readback=%.3f source=%.3f ok=%s XR_FOV=UNCHANGED",
                    ph.source_vfov, confirmed, phase_vfov, fov_ok?"true":"false");
                debug_trace::Write(fm);
            }
            if (phase_vfov <= 0.0f) {
                // V59.8: F8 deliberately uses the confirmed GAME_FOV/SOURCE_FOV
                // lock, not a transient published camera FOV (dialogue/inventory
                // can publish their own temporary values).
                phase_vfov = vr_source_vertical_fov_deg_.load(std::memory_order_relaxed);
            }
            xr_manager_->SetPresentationSettings(
                vr_render_scale_.load(std::memory_order_relaxed), ph.presentation_mode, ph.zoom,
                0.0f,0.0f,0.0f,0.0f,phase_vfov);
        }
        float runtime_ipd=0.064f; XrManager::StereoViewSnapshot sv{}; if(xr_manager_->CopyLatestStereoViews(sv)&&sv.raw_ipd>0.03f&&sv.raw_ipd<0.10f) runtime_ipd=sv.raw_ipd;
        if(index==0 && !v54_stereo_original.valid) v54_stereo_original=CaptureV54StereoCVars();
        // V61.0 phase-local stereo mode. Use current source raster/resolution; no custom-res rebuild.
        if (ph.stereo_test == 1) {
            RestoreV54StereoCVars(v54_stereo_original);
            FrameSync::SetNativeStereoReentryMode(0,0.064f);
            CameraHook::SetAfrIpd(runtime_ipd, ph.stereo_ipd_scale);
            FrameSync::SetStereoMode(1);
            DeviceHook::SetAfrStereoCapture(true);
            xr_manager_->SetStereoExperiment(1, ph.stereo_ipd_scale);
        } else if (ph.stereo_test == 2) {
            DeviceHook::SetAfrStereoCapture(false); FrameSync::SetStereoMode(0); xr_manager_->SetStereoExperiment(0,0.0f);
            const float test_ipd=std::clamp(runtime_ipd*ph.stereo_ipd_scale,0.004f,0.095f);
            ApplyV54StereoCVars(test_ipd,false);
            FrameSync::SetNativeStereoReentryMode(2,test_ipd);
            CameraHook::SetAfrIpd(runtime_ipd,0.0f);
        }
        const auto probe_size = latest_present_size();
        const std::uint32_t probe_w = probe_size.first ? static_cast<std::uint32_t>(probe_size.first) : 1920u;
        const std::uint32_t probe_h = probe_size.second ? probe_size.second : 1920u;
        if(ph.stereo_test==2) {
            DeviceHook::SetSceneColorProbe(true,probe_w,probe_h);
            char sp[320]{}; std::snprintf(sp,sizeof(sp),
                "V61.0-DUAL-PROBE phase=%s reentryMode=%d ipdScale=%.2f rawIpd=%.4f target=%ux%u PRESENT_BRIDGE=MONO",
                ph.name,FrameSync::NativeStereoReentryMode(),(double)ph.stereo_ipd_scale,(double)runtime_ipd,probe_w,probe_h);
            debug_trace::Write(sp);
        }
        v54_reentry_start=FrameSync::NativeStereoReentryPasses();
        matrix_phase=index; matrix_phase_start_ms=static_cast<std::uint64_t>(GetTickCount64());
        V54PhaseBeeps(index+1);
        char m[720]{}; std::snprintf(m,sizeof(m),
          "V61.0-F8-BEGIN BUILD=AFR-DUAL-TPV-MENU-20260906-N phase=%d/%d name=%s durationMs=%llu GAME_FOV=%.3f SOURCE_FOV=%.3f MATCHED=%s kind=%d presentMode=%d fovAsym=%.3f zoom=%.2f mapMode=%d uvBinoc=%.4f pairing=%d stereoTest=%d ipdScale=%.2f",
          index+1,kMatrixPhaseCount,ph.name,(unsigned long long)ph.duration_ms,
          (double)ReadCurrentGameFovValue(), (double)vr_source_vertical_fov_deg_.load(std::memory_order_relaxed),
          (std::fabs(ReadCurrentGameFovValue()-vr_source_vertical_fov_deg_.load(std::memory_order_relaxed))<0.01f)?"true":"false",
          ph.kind, ph.presentation_mode, (double)ph.mono_fov_asym,(double)ph.zoom,ph.uv_reproj_mode,(double)ph.uv_binocular,ph.pairing_mode,ph.stereo_test,(double)ph.stereo_ipd_scale);
        debug_trace::Write(m);
    };

    auto finish_matrix_phase = [&]() {
        if(matrix_phase<0 || matrix_phase>=kMatrixPhaseCount) return;
        const MatrixPhase& ph=matrix_phases[matrix_phase]; const MatrixCounters endc=capture_matrix_counters();
        const auto psz=latest_present_size();
        const char* src = ph.pose_source==0?"CACHED_XR":ph.pose_source==1?"SYNC_PREDICTED":ph.pose_source==2?"SYNC_NOW":"SYNC_PREDICTED_OFFSET";
        char m[800]{};
        std::snprintf(m,sizeof(m),
            "V56-F8-END phase=%d/%d name=%s xrFrames=%llu present=%llu presentSize=%llux%u lastPoseFrame=%llu cameraPoseApplyAgeMs=%llu poseSource=%s rotationOrder=%s filterMode=%d offsetMs=%.1f monoBlits=%llu afrBlits=%llu",
            matrix_phase+1,kMatrixPhaseCount,ph.name,
            static_cast<unsigned long long>(delta_u64(endc.xr.xr_frames,matrix_phase_start.xr.xr_frames)),
            static_cast<unsigned long long>(delta_u64(endc.present,matrix_phase_start.present)),
            static_cast<unsigned long long>(psz.first),psz.second,
            static_cast<unsigned long long>(CameraHook::LastPoseFrameIndex()),
            static_cast<unsigned long long>(CameraHook::LastPoseApplyTickMs()?GetTickCount64()-CameraHook::LastPoseApplyTickMs():0),
            src,ph.rotation_order==0?"BASE_X_HMD_LOCAL":"HMD_X_BASE_DIAGNOSTIC",ph.filter_mode,ph.prediction_offset_ms,
            static_cast<unsigned long long>(delta_u64(endc.xr.mono_blits,matrix_phase_start.xr.mono_blits)),
            static_cast<unsigned long long>(delta_u64(endc.xr.afr_blits,matrix_phase_start.xr.afr_blits)));
        debug_trace::Write(m);
        char x[320]{}; std::snprintf(x,sizeof(x),"V56-REENTRY-DIAG phase=%d reentryMode=%d dualPassExecutions=%llu",matrix_phase+1,FrameSync::NativeStereoReentryMode(),(unsigned long long)(FrameSync::NativeStereoReentryPasses()-v54_reentry_start)); debug_trace::Write(x);
        { char sc[384]{}; std::snprintf(sc,sizeof(sc),
            "V59.6-SCENE-SUMMARY phase=%d name=%s rtTouches=%llu left=%llu right=%llu",
            matrix_phase+1,ph.name,
            (unsigned long long)DeviceHook::SceneColorProbeBarrierCount(),
            (unsigned long long)DeviceHook::SceneColorProbeLeftCount(),
            (unsigned long long)DeviceHook::SceneColorProbeRightCount()); debug_trace::Write(sc); }
        float wg=0,wh=0,wf=0; std::uint64_t wn=0; CameraHook::GetWorstCaseAngular(wg,wh,wf,wn); char a[320]{}; std::snprintf(a,sizeof(a),"V56-ANGULAR-WORST phase=%d name=%s worstGapDeg=%.4f hmdDeg=%.4f finalDeg=%.4f sample=%llu",matrix_phase+1,ph.name,wg,wh,wf,(unsigned long long)wn); debug_trace::Write(a);
        // F8 ladder addition: directly reports what fraction of blits this
        // phase had the legacy coverage-clamp actively reducing scale below
        // what was requested - the exact, empirical measurement of the
        // musings doc's 0.697-vs-0.995 finding, per phase.
        {
            std::uint64_t clamp_active=0, clamp_total=0; xr_manager_->GetClampStats(clamp_active, clamp_total);
            const double clamp_pct = clamp_total > 0 ? (100.0 * static_cast<double>(clamp_active) / static_cast<double>(clamp_total)) : 0.0;
            char c[256]{}; std::snprintf(c,sizeof(c),"V56-CLAMP-STATS phase=%d name=%s clampActive=%llu clampTotal=%llu clampActivePct=%.1f",
                matrix_phase+1,ph.name,(unsigned long long)clamp_active,(unsigned long long)clamp_total,clamp_pct);
            debug_trace::Write(c);
        }
    };

    auto log_boxing_sample = [&]() {
        if (!xr_manager_) return;
        const auto& l = xr_manager_->LeftController();
        const auto& r = xr_manager_->RightController();
        void* console = SafeGetCryConsole();
        auto cvar_i = [&](const char* name, int fallback) -> int {
            return SafeCVarGetInt(SafeConsoleGetCVar(console, name), fallback);
        };
        const int player_no_ik = cvar_i("player_NoIK", -999);
        const int player_draw_ik = cvar_i("player_DrawIK", -999);
        const int debug_limbs = cvar_i("wh_pl_DebugIKLimbs", -999);
        const int adik_targets = cvar_i("ca_useADIKTargets", -999);
        const int show_head = cvar_i("wh_pl_ShowHead", -999);
        const bool mouse_l = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        const bool mouse_r = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
        const bool key_w = (GetAsyncKeyState('W') & 0x8000) != 0;
        const bool key_a = (GetAsyncKeyState('A') & 0x8000) != 0;
        const bool key_s = (GetAsyncKeyState('S') & 0x8000) != 0;
        const bool key_d = (GetAsyncKeyState('D') & 0x8000) != 0;
        const bool key_f = (GetAsyncKeyState('F') & 0x8000) != 0;
        const bool key_e = (GetAsyncKeyState('E') & 0x8000) != 0;
        const bool key_q = (GetAsyncKeyState('Q') & 0x8000) != 0;
        const bool mouse_m = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
        char m[1400]{};
        std::snprintf(m, sizeof(m),
            "V47.0-COMBAT sample=%u input={LMB:%s RMB:%s MMB:%s F:%s E:%s Q:%s W:%s A:%s S:%s D:%s} Lctrl={active:%s pos:(%.4f,%.4f,%.4f)} Rctrl={active:%s pos:(%.4f,%.4f,%.4f)} cvars={player_NoIK:%d player_DrawIK:%d wh_pl_DebugIKLimbs:%d ca_useADIKTargets:%d wh_pl_ShowHead:%d} counters={normalPipe:%llu recursivePipe:%llu present:%llu}",
            ++boxing_sample_index, mouse_l?"down":"up", mouse_r?"down":"up", mouse_m?"down":"up", key_f?"down":"up", key_e?"down":"up", key_q?"down":"up", key_w?"down":"up", key_a?"down":"up", key_s?"down":"up", key_d?"down":"up",
            l.active?"true":"false", l.pose.pos_x,l.pose.pos_y,l.pose.pos_z,
            r.active?"true":"false", r.pose.pos_x,r.pose.pos_y,r.pose.pos_z,
            player_no_ik,player_draw_ik,debug_limbs,adik_targets,show_head,
            static_cast<unsigned long long>(FrameSync::PipelineCount(0)),
            static_cast<unsigned long long>(FrameSync::PipelineCount(1)),
            static_cast<unsigned long long>(DeviceHook::PresentCaptureCount()));
        debug_trace::Write(m);
    };

    auto restore_safe_mono = [&]() {
        if (vr_menu_) vr_menu_->SetCaptureCountdown(false, 0);
        DeviceHook::SetCustomResolutionProbe(false, 0, 0);
        const float safe_fov = std::clamp(matrix_active ? matrix_restore_fov : vr_source_vertical_fov_deg_.load(std::memory_order_relaxed), 70.0f, 140.0f);
        const float safe_framing = std::clamp(matrix_active ? matrix_restore_framing : vr_zoom_.load(std::memory_order_relaxed), 0.80f, 2.00f);
        // V58 safe state is Mono + OpenXR-FOV fallback calibration. Do not
        // touch KCD2 cl_fov or desktop/custom resolution here.
        vr_stereo_experiment_.store(0, std::memory_order_relaxed);
        vr_afr_ipd_scale_.store(0.0f, std::memory_order_relaxed);
        const int restore_preset = std::clamp(vr_cyclopean_mode_.load(std::memory_order_relaxed),0,5);
        const V603ViewPreset restore_view = ViewPresetForMode(restore_preset);
        const int restore_present = restore_view.presentation_mode;
        const float restore_zoom = safe_framing;
        const float restore_uv = restore_view.uv_binocular;
        vr_presentation_mode_.store(restore_present, std::memory_order_relaxed);
        vr_source_vertical_fov_deg_.store(safe_fov, std::memory_order_relaxed);
        vr_zoom_.store(restore_zoom, std::memory_order_relaxed);
        vr_left_offset_x_.store(0.0f, std::memory_order_relaxed);
        vr_left_offset_y_.store(0.0f, std::memory_order_relaxed);
        vr_right_offset_x_.store(0.0f, std::memory_order_relaxed);
        vr_right_offset_y_.store(0.0f, std::memory_order_relaxed);
        vr_swap_eyes_.store(false, std::memory_order_relaxed);
        xr_manager_->SetStereoExperiment(0, 0.0f);
        xr_manager_->SetPresentationSettings(
            vr_render_scale_.load(std::memory_order_relaxed), restore_present, restore_zoom,
            0.0f, 0.0f, 0.0f, 0.0f, safe_fov);
        xr_manager_->SetAngularCorrectMono(restore_view.angular_correct);
        xr_manager_->SetPoseHonestyOverride(restore_view.pose_honesty);
        xr_manager_->SetMonoUvReprojectionMode(0);
        xr_manager_->SetMonoUvBinocularOffset(restore_uv);
        xr_manager_->SetSwapEyes(false);
        xr_manager_->SetMonoFovAsymmetryScale(restore_view.fov_asymmetry);
        xr_manager_->SetPresentationTestMarker(false);
        xr_manager_->SetPresentPosePairingMode(1);
        DeviceHook::SetAfrStereoCapture(false);
        FrameSync::SetStereoMode(0);
        CameraHook::SetAfrIpd(0.064f, 0.0f);
    };

    bool vr_projection_logged = false;
    auto ensure_good_mono_projection = [&]() {
        if(vr_projection_logged) return;
        vr_projection_logged=true;
        debug_trace::Write("V58-GOOD-MONO: OpenXR tangent remap active; mono coverage/framing are compositor-only; KCD2 cl_fov and desktop resolution untouched");
    debug_trace::Write("V61.26-RESOLUTION preferred source baseline=1920x1800; KCD2 source/desktop resolution is NOT overridden by this build");
    };

    VrGraphicsSnapshot vr_graphics_original{};
    bool vr_graphics_applied = false;
    DialogueCVarSnapshot dialogue_original{};
    int dialogue_mode_applied = -1;
    std::uint64_t vr_graphics_last_try_ms = 0;
    graphics_profile_refresh_requested_.store(true, std::memory_order_release);
    graphics_profile_applied_for_ui_.store(false, std::memory_order_release);

    auto refresh_vr_graphics_profile = [&](bool force) {
        const bool wanted = enable_vr_balanced_graphics_.load(std::memory_order_acquire);
        const std::uint64_t now = static_cast<std::uint64_t>(GetTickCount64());
        if (wanted) {
            if (vr_graphics_applied && !force) return;
            if (!vr_graphics_original.valid) {
                if (!force && now - vr_graphics_last_try_ms < 1000) return;
                vr_graphics_last_try_ms = now;
                if (!CaptureVrGraphicsSnapshot(vr_graphics_original)) return;
                char msg[384]{};
                std::snprintf(msg, sizeof(msg),
                    "V46.3-GRAPHICS-SNAPSHOT view=%.2f veg=%.2f merged=%.2f blur=%d sharp=%.3f",
                    static_cast<double>(vr_graphics_original.view_dist),
                    static_cast<double>(vr_graphics_original.vegetation_dist),
                    static_cast<double>(vr_graphics_original.merged_mesh_dist),
                    vr_graphics_original.motion_blur, static_cast<double>(vr_graphics_original.sharpening));
                debug_trace::Write(msg);
            } else if (!force && now - vr_graphics_last_try_ms < 1000) {
                return;
            }
            vr_graphics_last_try_ms = now;
            char msg[768]{};
            const bool ok = ApplyVrBalancedGraphics(vr_graphics_original,
                vr_engine_clarity_.load(std::memory_order_acquire), msg, sizeof(msg));
            if (msg[0]) debug_trace::Write(msg);
            if (!ok) {
                // A setter can fail after earlier setters succeeded. Roll back
                // immediately so a partial profile can never be left behind.
                char rollback[512]{};
                (void)RestoreVrGraphics(vr_graphics_original, rollback, sizeof(rollback));
                if (rollback[0]) debug_trace::Write(rollback);
            }
            vr_graphics_applied = ok;
            graphics_profile_applied_for_ui_.store(ok, std::memory_order_release);
        } else if (vr_graphics_applied && vr_graphics_original.valid) {
            char msg[512]{};
            const bool ok = RestoreVrGraphics(vr_graphics_original, msg, sizeof(msg));
            if (msg[0]) debug_trace::Write(msg);
            if (ok) {
                vr_graphics_applied = false;
                graphics_profile_applied_for_ui_.store(false, std::memory_order_release);
            }
        }
    };

    kcd2vr::linked_fov_h26::Startup startupFov;
    bool startupFovReceipt=false,startupGameplayFovReceipt=false;
    auto refresh_game_fov_once = [&](bool gameplay=false) {
        if(!startupFov.Poll(GetTickCount64(),FrameSync::PublishedFovSampleCount(),gameplay))return;
        const float wanted=kcd2vr::linked_fov_h26::Value(menu_game_fov_target_.load(std::memory_order_acquire));
        const float before=ReadCurrentGameFovValue();
        const bool attempted=!(std::isfinite(before)&&before>1.f&&std::fabs(before-wanted)<.25f);
        const bool ok=attempted?ApplyNativeGameFov(wanted):true;
        const float after=attempted?ReadCurrentGameFovValue():before;
        const bool first=!startupFovReceipt,firstGameplay=gameplay&&!startupGameplayFovReceipt;
        startupFovReceipt=true;if(gameplay)startupGameplayFovReceipt=true;
        if(!first&&!firstGameplay&&!attempted)return;
        const bool confirmed=std::isfinite(after)&&after>1.f&&std::fabs(after-wanted)<.25f;
        char m[640]{};std::snprintf(m,sizeof(m),
          "H28R1 FOV_START stage=%s selectedGame=%.3f selectedVR=%.3f nativeBefore=%.3f nativeAfter=%.3f compositorSource=%.3f correctedView=%.3f zoom=%.3f attempted=%d writeReturned=%d confirmed=%d gameplay=%d savedTargetPreserved=1 measurement=CVAR_AND_COMPOSITOR_SETTINGS renderedLens=UNVERIFIED",
          firstGameplay?"FIRST_GAMEPLAY":first?"FIRST_RENDER":"CORRECTION",double(wanted),
          double(vr_source_vertical_fov_deg_.load()),double(before),double(after),
          double(xr_manager_->GetSourceVerticalFov()),double(xr_manager_->GetCorrectedViewVerticalFov()),
          double(vr_zoom_.load()),int(attempted),int(ok),int(confirmed),int(gameplay));
        KCD2VR_DIAG_LOG(Bug,info,"{}",m);
        diagnostic_trace::Write(m);
    };

    int f5_dialogue_lab_mode=0; // V65.03: 0 off, 1 camera suppression, 2 + NPC look-at assist
    DialogueLookCVarSnapshot f5_look_original{};
    auto refresh_dialogue_camera_mode = [&]() {
        const int configured=std::clamp(vr_dialogue_camera_mode_.load(std::memory_order_relaxed),0,2);
        const int wanted=configured; // V65.15: F5 dialogue ladder retired; menu setting owns dialogue mode
        if(!dialogue_original.valid) {
            if(!CaptureDialogueCVars(dialogue_original)) return;
            char m[192]{}; std::snprintf(m,sizeof(m),"V64.3-DIALOGUE original={ForcePOV:%d NoCameras:%d}",dialogue_original.force_pov,dialogue_original.no_cameras); diagnostic_trace::Write(m);
        }
        // V64.20: while F5 lab is active, enable ForcePOV first, then test final camera publication on top of that stable native base.
        // Outside the lab preserve the V64.13 Native/First Person setting. Native leaves KCD2's authored dialogue
        // camera choice untouched; First Person uses ForcePOV. The parser hook stays installed
        // for observation/compatibility but no longer forces Medium/Close/Closeup/Master categories.
        if(wanted!=dialogue_mode_applied && ApplyDialogueMode(dialogue_original,wanted)) dialogue_mode_applied=wanted;
        dialogue_camera_override::SetOverrideType(0);
        dialogue_camera_override::SetForceNoOverride(configured==2);
    };
    refresh_game_fov_once();
    refresh_dialogue_camera_mode();

    // V51 default runtime tracking: immutable predictedDisplayTime pose, applied once
    // at PublishViewCamera instead of repeatedly mutating frustum cameras.
    debug_trace::Write(mirror_mis_menu_to_desktop_.load(std::memory_order_relaxed)
        ? "V58.3-MENU desktop mirror ENABLED by INI (diagnostic; may reintroduce hybrid leakage)"
        : "V58.3-MENU desktop mirror DISABLED; dedicated XR menu layer only");
    touch_bindings_.SetEnabled(enable_touch_bindings_.load(std::memory_order_relaxed));
    touch_bindings_.SetAttackDirectionMouseEmulation(enable_touch_attack_direction_.load(std::memory_order_relaxed));
    touch_bindings_.SetTurnMode(vr_turn_mode_.load(std::memory_order_relaxed));
    xr_manager_->SetUiLaserVisible(vr_laser_visible_.load(std::memory_order_relaxed));
    xr_manager_->SetSharpening(vr_engine_clarity_.load(std::memory_order_relaxed));
    xr_manager_->SetLatePoseEnabled(false);
    xr_manager_->SetPresentPosePairingMode(1);
    xr_manager_->SetPresentPacingMode(0); // V65.04: adaptive35 stays authoritative with F2 ownership fix ON or OFF
    xr_manager_->SetXrRenderGateMode(0);  // legacy gate off; saved Fresh Pose pacing is independent (mode4 bypasses both)
    CameraHook::SetInjectionMode(1);
    CameraHook::SetPoseSourceMode(vr_pose_source_mode_.load(std::memory_order_relaxed),
                                  vr_pose_prediction_offset_ms_.load(std::memory_order_relaxed));
    {
        char pm[192]{};
        std::snprintf(pm,sizeof(pm),"V61.45-POSE-SOURCE normal mode=%d offsetMs=%.2f (0=cached 1=predicted 2=now 3=predicted+offset)",
            vr_pose_source_mode_.load(std::memory_order_relaxed),
            double(vr_pose_prediction_offset_ms_.load(std::memory_order_relaxed)));
        debug_trace::Write(pm);
    }

    // V61.32 presentation architecture: start in proven GOOD MONO immersive mode,
    // but restore F6 as the manual world-locked virtual screen. F8 now owns the
    // automatic cyclopean/source-FOV + optical-center correction experiment.
    bool crysis_crop_mono = true;
    bool adaptive_rotation_filter = false;
    debug_trace::Write("V65.04-HEAD trusted fixed8ms/adaptive35 baseline active; F2 toggles renderer-owned PRESENT pose pairing only");
    std::uint64_t angular_sync_last_log_ms = 0;
    xr_manager_->SetAngularCorrectMono(true);
    debug_trace::Write("V63.1-MONO corrected projection baseline retained; V60.1 dialog/inventory guard; Fix head sync=mode2; native event-driven HUD; VRIK ladder starts OFF");

    bool f6_down_prev = false;
    int v6627_f2_stage = 1; // V66.29: 0=baseline pose publication, 1=renderer-owned PRESENT pose pairing (corrected head following)
    int v6640_ao_step=0;
    V6640AoSnapshot v6640_ao_original{};
    bool v6640_ao_snapshot_ready=false;
    int single_run_last_phase=0; // compile fix: declaration was lost in V66.69
    constexpr int f4_immersive_ui_rung=0; // V66.92R1: experimental F4 is retired; retained branches cannot activate.
    bool physical_map_active=false;
    std::uint64_t physical_map_open_ms=0;
    std::uint64_t physical_map_read_start_base=0;
    bool physical_map_loop_requested=false;
    std::uint64_t f4_wrist_next_ms=0;
    int v6638_pair_pose_step=0; // historical source-only stereo pair pose step; no production hotkey owner
    int f5_hdr_pair_step = 0; // V65.93: 0 OFF, 1 read-only census, 2 A->L/B->R, 3 swapped, 4 A-both, 5 B-both
    bool f7_down_prev = false;
    int v6652_rung=0;
    V6652Snapshot v6652_original{};
    int v6644_bisect_rung=0;
    int v6643_shadow_step=0;
    V6643ShadowTemporalSnapshot v6643_shadow_original{};
    bool v6643_shadow_snapshot_ready=false;
    (void)v6643_shadow_step; (void)v6643_shadow_snapshot_ready; (void)v6643_shadow_original;
    int v6641_refl_gi_step=0;
    V6641ReflGiSnapshot v6641_refl_gi_original{};
    bool v6641_refl_gi_snapshot_ready=false;
    (void)v6641_refl_gi_step; (void)v6641_refl_gi_original; (void)v6641_refl_gi_snapshot_ready;
    int v6640_light_shadow_step=0;
    V6640LightShadowSnapshot v6640_light_shadow_original{};
    bool v6640_light_shadow_snapshot_ready=false;
    (void)v6640_light_shadow_step; (void)v6640_light_shadow_snapshot_ready; (void)v6640_light_shadow_original;
    bool f9_down_prev = false;
    bool f10_down_prev = false;
    bool f11_down_prev = false;
    bool f12_down_prev = false;
    int f7_input_mode = 0; // V65.19: native XI production transport retired; F12 only reasserts prompt family
    bool f8_research_override=false;
    bool f8_archaeology_session=false; // once F8 archaeology is entered, legacy stereo writer stays quarantined until restart
    bool f8_wide_archaeology_active=false;
    stereo_archaeology::WideCounterSnapshot f8_wide_baseline{}; // V65.34 rung6 dormant broad CryEngine stereo observer suite
    int f8_research_step=0;
    int f9_postfx_step=0; // V66.18: 0 idle, 4 continuous native stereo active/pending
    std::uint64_t v6619_next_autostart_ms=0; // V66.19R1: autostart retry/re-arm backoff
    std::uint64_t v6623_last_render_policy_ms=0; // vr.cfg-style sparse reapply/watchdog
    int v6624_taa_probe_state=2; // V66.26: automatic TAA window retired; F12 owns AA explicitly
    std::uint64_t v6624_taa_probe_start_pair=0;
    V6626EffectSnapshot v6626_effect_original{};
    bool v6626_effect_snapshot_ready=false;
    int v6626_shadow_step=0; // F9 V66.39R1: census, center shadow prep, exact SSR source census, combined
    int v6626_hdr_step=0;    // diagnostic override only; normal V66.32 path does not apply the ladder
    int v6626_cloud_step=0;  // F11 starts from captured retail policy, then temporal/stereo reprojection A/B
    int v6635_secondary_step=0; // retained source-only; V66.38 F11 no longer uses cloud-state block copying
    int v6626_aa_step=7;     // V66.37: first F12 press lands on rung 0 (clean AA0 baseline)
    bool v6632_effect_override_active=false; // normal path never reapplies F9-F12 ladder
    bool v6626_native_census_started=false;
    int v6619_xr_stereo_applied=-1;          // last XR presentation mode applied while stage 4 runs
    bool v6619_theater_capture_paused=false; // theater keeps native stage4 alive but disables pair capture/publication
    bool v6619_waiting_fresh_pair=false;     // immersive resume stays mono until a new post-theater pair is published
    std::uint64_t v6619_resume_pair_count=0; // pair-count baseline captured before resume census is rearmed
    bool v6622_default_center_applied=false;   // derive vertical optical-center pitch once from live XR eye FOV
    bool v6618_auto_stereo_enabled=vr_production_stereo_mode_.load(std::memory_order_acquire)==3; // V66.68 menu Mono/Stereo is authoritative
    bool v6668_mode_policy_initialized=false;
    int v6668_gsm_policy_pending=-1;
    std::uint64_t v6668_gsm_policy_retry_ms=0;
    bool v6668_taa_policy_pending=true; // V66.85 observe player AA only; pair-epoch/retail history wiring is mode-owned separately
    std::uint64_t v6668_taa_policy_retry_ms=0;
    int f10_bridge_step=0; // V65.90: 0 OFF, 1 exact first-F9 repro, 2 swapped, 3 frozen-both control, 4 live-both control
    int f10_bridge_completed_frames=0;
    std::uint64_t f10_bridge_last_pair_serial=0;
    bool f10_bridge_producer_active=false;
    int f2_head_probe_level=3; // V65.10 always-on ownership matcher
    int f8_stereo_probe_level=0;
    bool f8_lab_initialized=false;
    bool f8_native_vr_initialized=false;
    bool f8_cal_key_prev=false;
    int finger_capture_status_prev=0;
    int f8_cal_stage=0; // V66.68 menu-driven guided T-pose: 0 idle, 1 countdown, 2 capture
    std::uint64_t f8_cal_stage_ms=0;
    int f8_cal_beep_index=-1;
    int vrik_auto_enable_after_cal=0;
    kcd2vr::start_r111::Episode start_episode_r111{}; // R111: one start decision per game start / save load
    kcd2vr::start_r111::Steady start_head_r112{};     // R112: the automatic recenter waits for a steady head at the runtime center
    std::uint64_t vrik_cal_complete_until_ms=0;
    int f9_vrik_input_step=0;
    int v6559_anim_lab_step=0;
    std::uint64_t f2_next_status_ms=0,f8_next_status_ms=0;
    bool key_i_prev = false, key_m_prev = false, key_esc_prev = false;
    // V65.23: presentation overrides are retired. The game-reported scene is the sole
    // V65.29 clean router owns Theater. R124: F6 toggles Theater <-> Immersive, an L3+R3 click releases to Immersive.
    int scene_emergency_override = -1; // V65.23 retired sentinel; never used to choose production presentation
    int presentation_scene_key_prev = -1;
    bool emergency_toggle_requested = false; // V65.29 panic release to Immersive (R124: the release half of the F6 toggle)
    kcd2vr::theater_toggle_r124::State theater_toggle_r124{}; // R124 F6 toggle: ends on F6 or the next scene-key change
    bool theater_shown_prev_r124 = false; // what the headset showed last tick (F6 decides the direction from it)
    int applied_display_override=-1; // persisted menu policy only: 0 AUTO, 2 THEATER
    bool inventory_flat_ui = false;
    bool inventory_close_pending = false;
    std::uint64_t inventory_close_absent_since_ms = 0;
    bool map_flat_ui = false;
    bool cursor_menu_apse_hub = false; // V64.99 transition-classified APSE inventory/player/map hub
    bool cursor_menu_shop_positive = false; // V65.08 positive shop child classification; never infer from generic cursor alone
    bool pause_flat_ui = false;
    bool cursor_menu_flat_ui = false; // game UI scene lifetime; cursor is entry hint only
    using CleanMenuOwner = ui_menu_router::Owner;
    ui_menu_router::Router ui88_router;
    CleanMenuOwner clean_menu_owner = CleanMenuOwner::Frontend;
    std::uint64_t contextual_fullscreen_absent_since_ms=0;
    std::uint32_t cursor_menu_panel_mask=0;
    // V65.00: never use cursor disappearance as menu-close authority.  Once a game UI
    // scene opens, track the actual KCD2 Flash root for that scene until the root is gone.
    // This is bounded to the active menu only and sampled at 4 Hz, so normal gameplay has
    // zero FlashUI lifetime polling.  It mirrors the native C_UIApse/C_UIShop/
    // C_UIItemTransfer lifetime exposed by WHGame without guessing an unsafe C++ ABI.
    enum class MenuLifetimeKind : unsigned char { None, Apse, Crafting, Transfer };
    MenuLifetimeKind menu_lifetime_kind = MenuLifetimeKind::None;
    std::uint64_t menu_lifetime_next_ms = 0;
    std::uint64_t cursor_menu_probe_until_ms = 0;
    std::uint64_t cursor_menu_probe_next_ms = 0;
    std::uint64_t inventory_theater_hold_until_ms = 0; // V65.08 bounded APSE tab-rebuild grace; cleared on proven gameplay return
    bool startup_flat_ui_auto = true; // V65.03: title/start stays Theater. Continue/Load
    // releases this owner immediately when the frontend closes; a real cutscene can
    // independently re-enter Theater. No gameplay-FOV startup heuristic is used.
    std::uint64_t startup_ready_since_ms = 0;
    bool startup_frontend_cursor_seen = false;
    bool startup_last_raw_cursor_visible = false;
    std::uint64_t native_menu_no_cursor_since_ms = 0; // V66.20: game-state exit debounce for large menu plane
    bool fov_safety_logged = false;
    bool tracking_suspended_for_flat_ui = true;
    void* flat_state_console = SafeGetCryConsole();
    void* cutscene_context_cvar = flat_state_console ? SafeConsoleGetCVar(flat_state_console, "e_SkipInFrustumOptimizationWH") : nullptr;
    // C10R2: iDialogueContextId belongs to dialogue camera prefab properties;
    // it is not a console variable. The retained player-view hook observes the
    // exact native GetPlayerDialog chain and publishes a fresh, bounded witness.
    // V64.19C Dynamic Detector-inspired correlation. wh_ui_ShowHud is safe to
    // observe, but is NOT by itself a VRIK write gate because it also covers the
    // user's manual HUD-off preference. Scripted loss-of-agency is now read through
    // the exact-build CCryAction -> CActionMapManager -> IsFilterEnabled path.
    void* show_hud_cvar = flat_state_console ? SafeConsoleGetCVar(flat_state_console, "wh_ui_ShowHud") : nullptr;
    dialogue_ratio_d79::Lease dialogueRatioD79;
    // V62.6: Inventory can recreate the HUD MovieClips. Reapply the desired
    // semantic layout with a short bounded retry sequence after close.
    bool inventory_was_open_for_hud = false;
    int hud_reapply_attempts = 0;
    std::uint64_t hud_reapply_next_ms = 0;
    // V63.0: Lua HUD lifecycle companion retired. No periodic script re-push.
    CameraHook::SetTrackingOptions(false,false);
    xr_manager_->SetNativeMenuPlaneMode(true);
    debug_trace::Write("V63.1-STARTUP corrected culling retained; head-sync cached<->SYNC_NOW; F6 AUTO/override restored; Lua retired; native event-driven HUD; VRIK ladder starts OFF");
    int matrix_restore_scene_emergency_override = -1;
    bool matrix_restore_inventory_flat_ui = false;
    // V61.18: run HUD FlashUI RTTI discovery directly; V61.17 accidentally only reached this
    // through the optional stereo graphics-pipeline observer, so normal runs never scanned.
    stereo_archaeology::DiscoverHudFlashUiOnce();
    // V65.03: native-stereo archaeology is F8-gated only; normal gameplay installs no stereo chase.
    // V62.4: restore the low-frequency FlashUI GetElement observer. The native
    // HUD sliders reuse the same game-returned HUD object path that previously
    // proved CombatCursorCross visibility, avoiding retail console-Lua entirely.
    bool flashui_live_installed=flashui_probe::Install();
    if(flashui_live_installed)(void)flashui_probe::SetHudRepair175(true);
    (void)activity_input163::InstallNative175();
    if(flashui_live_installed) KCD2VR_DIAG_LOG(Bug,info,"V65.16 native FlashUI HUD observer installed");
    else KCD2VR_DIAG_LOG(Bug,warning,"V65.16 FlashUI HUD observer not ready at startup; event-driven retry armed");
    std::uint64_t last_hud_generation=flashui_probe::HudGeneration();
    bool last_dialogue_for_hud=false;
    bool last_effective_flat_for_hud=true;
    // V65.16 VRIK identity primer: install the exact-build hooks read-only at startup
    // so Henry's player/SetIK ownership can be learned BEFORE the user presses F10.
    // The skinning hook fast-paths immediately while all labs are OFF; no pose writes
    // are authorized until an explicit F10 rung is selected.
    enable_vrik_ownership_test_.store(false,std::memory_order_release);
    bool animation_filter_installed=false;
    bool vrik_probe_installed=frik_pose_probe::Install();
    bool vrik_probe_install_attempted=true;
    if(vrik_probe_installed) {
        if(frik_pose_probe::NativeHandPhysicsH24())(void)frik_pose_probe::PrepareNativeHandPhysicsH24();
        frik_pose_probe::SetPlayerIdentityObservationEnabled(true);
        enable_vrik_ownership_test_.store(true,std::memory_order_release);
        diagnostic_trace::Write("V66.68-VRIK identity primer installed from V65.81 integration + V65.83 FRIK donor; writes remain OFF until F10 rung>0");
    } else {
        diagnostic_trace::Write("V66.68-VRIK identity primer FAILED-CLOSED; explicit VRIK action may retry");
        vrik_probe_install_attempted=false;
    }
    bool animation_filter_install_attempted=false;
    auto ensure_animation_filter=[&]()->bool {
        if(!animation_filter_installed && !animation_filter_install_attempted){
            animation_filter_install_attempted=true; animation_filter_installed=animation_filter::Install();
            diagnostic_trace::Write(animation_filter_installed?"V66.69 animation observer lazy install OK":"V66.69 animation observer install FAILED-CLOSED");
        }
        return animation_filter_installed;
    };
    auto ensure_vrik_lab=[&]()->bool {
        (void)ensure_animation_filter();
        if(!vrik_probe_installed && !vrik_probe_install_attempted){
            vrik_probe_install_attempted=true; enable_vrik_ownership_test_.store(true,std::memory_order_release);
            vrik_probe_installed=frik_pose_probe::Install();
            if(vrik_probe_installed){if(frik_pose_probe::NativeHandPhysicsH24())(void)frik_pose_probe::PrepareNativeHandPhysicsH24();frik_pose_probe::SetPlayerIdentityObservationEnabled(true);}
            diagnostic_trace::Write(vrik_probe_installed?"V66.68-VRIK hooks retry OK":"V66.68-VRIK hooks retry FAILED-CLOSED");
        }
        return vrik_probe_installed;
    };
    diagnostic_trace::Write("V66.68-GOLDEN ownership: V66.63 stereo/HUD/glyph core + V65.81 integration/V65.83 FRIK selective donor; no stereo effect ladders own VRIK keys");

    // V64.82: keyboard/mouse semantics are intentionally authoritative again.
    // The exact-build prompt hook changes only KCD2's UI device classification,
    // so contextual keyboard actions remain intact while prompts stay controller-shaped.
    // V65.08 restores the headset-positive V64.22B ownership rule: do NOT hook
    // KCD2's global input-device classifier. Keyboard/mouse semantics remain the
    // transport; Flash SetInputId owns glyph presentation independently.
    const int savedGlyphStyleD33=vr_pad_glyph_style_.load(std::memory_order_acquire)==4?4:2; // D33: saved / headset-derived glyph family, was hardcoded Xbox
    prompt_device_lock::SetForcedDevice(savedGlyphStyleD33);
    const bool prompt_device_lock_installed=prompt_device_lock::Install();
    // CB1: native alchemy completion observer (read-only entry detour on C_RPGMinigames::OnAlchemyRecipeFinish). INI VrAlchemyFinishObserver (1).
    kcd2vr::alchemy_finish_cb1::SetEnabled(g_alchemy_finish_observer_cb1.load(std::memory_order_acquire));
    if(kcd2vr::alchemy_finish_cb1::Enabled()){
        diagnostic_trace::Write(kcd2vr::alchemy_finish_cb1::Install()?"CB1 alchemy finish observer installed":"CB1 alchemy finish observer NOT installed (see line above)");
    } else diagnostic_trace::Write("CB1 alchemy finish observer OFF by INI");
    (void)kcd2vr::grip_stow_cb1::Install(); // CB1: script client for the grip-stow route (ON/OFF by VrGripStow at request time)
    diagnostic_trace::Write(prompt_device_lock_installed
        ? (savedGlyphStyleD33==4?"D68-PROMPTS PlayStation art; Xbox classifier/layout retained; keyboard/mouse Touch execution unchanged":"D68-PROMPTS Xbox art and classifier/layout; keyboard/mouse Touch execution unchanged")
        : "V65.19-PROMPTS classifier hook unavailable; keyboard/mouse Touch execution remains safe, Xbox display family may fall back");
    (void)ApplyF7PromptPresentation(true,savedGlyphStyleD33);
    diagnostic_trace::Write("V65.19-SUPERACTIONS presentation authority=CURRENT RETAIL defaultProfile.xml + defaultActionHelp.xml; tap/hold/context semantics are not hardcoded by KCD2VR");

    const bool dialogue_camera_override_installed=dialogue_camera_override::Install();
    if(!dialogue_camera_override_installed) KCD2VR_DIAG_LOG(Bug,warning,"V64.3 dialogue native camera-type override unavailable; Native/First-Person still work, category forcing stays fail-closed");
    // C63: native target resolution stays original-first. Only an already
    // Henry-targeting LookAt in a fresh embodied FirstPerson dialogue may use
    // the current HMD point; no AI task/participant controller is replaced.
    npc_lookat_hmd::SetEnabled(npc_lookat_hmd::Install());
    const bool shop_state_probe_installed=false; // Query-wrapper cache is not a lifecycle owner.
    diagnostic_trace::Write("V66.176 SCOPE: authored minigames retain native gamepad semantics; semantic HUD elements preserve native content and full-field effects");
    const bool ui88_lifecycle_installed=ui_native_state::Install();
    if(!ui_menu_camera59c2::Install())diagnostic_trace::Write("R59C2 MENU_CAMERA unavailable; presentation retains existing UI policy");
    if(!activity_lens59c10r1::Install())diagnostic_trace::Write("KCD2VR BUILD ACTIVITY_LENS unavailable; native activity lens retained; exact dialogue witness unavailable");
    if(!action_time_owner59c11::Install())diagnostic_trace::Write("R59.16C10R11 ACTION_OWNER unavailable (pins/hooks); Mannequin receipts and the time owner stay OFF"); // R59.16C10R11
    if(!g_immersive_fov59c7g.load())diagnostic_trace::Write("KCD2VR BUILD note: C7G IMMERSIVE_FOV disabled (VrImmersiveFov=0; the frustum hook broke stereo pairing on 13 Oct)");
    else if(!immersive_fov59c7g::Install())diagnostic_trace::Write("C7G IMMERSIVE_FOV unavailable; native frustum input retained");
    const bool ui88_pad_installed=ui_menu_input::Install();
    if(!ui88_lifecycle_installed) diagnostic_trace::Write("V66.88-UI-LIFECYCLE unavailable: state UNKNOWN; explicit requests keep safe full-frame presentation, F6 remains panic release");
    (void)ui88_pad_installed;
    // V66.77: the CActionMap::Enable observer is RETIRED. Headset proof (V66.72/74/75): installed at
    // startup it stalls save loading; installed later it freezes the game on the next action-map
    // change (black screen). V66.88 keeps it disabled and observes exact native UI
    // visibility/lifecycle instead; neither cursor state nor shop query results close menus.
    const bool action_map_probe_installed=false;
    if(!action_map_probe_installed) diagnostic_trace::Write("V66.88-UI old action-map hook and IsShopOpened ownership remain retired");
    bool combat_diag_probe_installed=false; // lazy F4 only; never install during loading/startup

    (void)kcd2vr::hand_only_door_h40::Install(); // exact existing original-first script callback; no extra hook
    const bool interaction_alignment_installed=interaction_alignment::Install();
    interaction_alignment::SetEnabled(false); // V66.146 per-frame/F5 ownership enables only when explicitly requested
    if(interaction_alignment_installed)
        KCD2VR_DIAG_LOG(Bug,info,"Interaction alignment hooks installed");
    else
        KCD2VR_DIAG_LOG(Bug,warning,"Interaction alignment hooks unavailable; pickup/use redirect fails closed");

    // V61.32 combined-test state. F6 is no longer a hypothesis key; it is the
    // restored LOCAL virtual screen. F8 owns the corrected projection proof.
    bool f8_auto_projection_active=false;
    vr_auto_projection_active_.store(false,std::memory_order_release);
    float f8_restore_center_x=vr_presentation_center_x_.load(std::memory_order_acquire);
    float f8_restore_center_y=vr_presentation_center_y_.load(std::memory_order_acquire);
    std::uint64_t f8_last_adjust_ms=0, f8_last_status_ms=0;
    std::uint64_t ui_auto_last_apply_ms=0;

    // V61.38: restore the successful V61.33 transient-zoom concept, but drive
    // it from the exact gameplay CView FOV recovered by the V61.12 identity gate.
    // This keeps dialogue/inventory/bow camera zoom from contaminating the
    // compositor while retaining a user-selectable fraction of native zoom.
    // V61.45: V61.38 transient dialogue/inventory cl_fov pumping is RETIRED.
    // The solved V60.1/V61.36 behavior is the fixed source-FOV guard below: gameplay
    // FOV remains untouched and transient dialogue/inventory camera publications are
    // never allowed to rescale the VR compositor source model.

    auto compute_auto_projection=[&](float actual_vfov_deg,float& out_center_x,float& out_center_y,float& out_required_centered,float& out_required_no_shift)->bool {
        out_center_x=0.0f; out_center_y=0.0f; out_required_centered=0.0f; out_required_no_shift=0.0f;
        if(!xr_manager_ || !(actual_vfov_deg>30.0f && actual_vfov_deg<150.0f)) return false;
        XrManager::RenderFrameState rf{}; if(!xr_manager_->CopyActiveRenderFrameState(rf)||!rf.stereo_views_valid) return false;
        std::uint32_t sw=1920,sh=1800; xr_manager_->GetLastCapturedSourceResolution(sw,sh);
        const float aspect=sh?static_cast<float>(sw)/static_cast<float>(sh):(1920.0f/1800.0f);
        float maxx=0.0f, left=0.0f, right=0.0f, top=0.0f, down=0.0f; bool first=true;
        for(int e=0;e<2;++e){
            const auto& f=rf.eye_fov[e];
            const float l=std::tan(f.angleLeft),r=std::tan(f.angleRight),u=std::tan(f.angleUp),d=std::tan(f.angleDown);
            maxx=std::max(maxx,std::max(std::fabs(l),std::fabs(r)));
            if(first||l<left) left=l; if(first||r>right) right=r;
            top=std::max(top,u); if(first||d<down) down=d; first=false;
        }
        const float vertical_half=0.5f*(top-down);
        const float needed_tan=std::max(vertical_half,maxx/std::max(aspect,0.5f));
        out_required_centered=2.0f*std::atan(needed_tan)*57.29577951308232f;
        // Same no-shift coverage diagnostic formerly calculated separately by
        // GetRequiredMonoSourceVFovDeg() on every frame.
        const float maxy=std::max(std::fabs(top),std::fabs(down));
        const float no_shift_tan=std::max(maxy,maxx/std::max(aspect,0.5f));
        out_required_no_shift=2.0f*std::atan(no_shift_tan)*57.29577951308232f;
        const float source_tan=std::tan(actual_vfov_deg*0.5f*0.01745329251994329577f);
        const float source_tan_x=source_tan*std::max(aspect,0.5f);
        if(!(source_tan>0.05f) || !(source_tan_x>0.05f)) return false;
        // Common X/Y center of the binocular outer frustum in symmetric source UV.
        out_center_x=std::clamp((right+left)/(4.0f*source_tan_x),-0.20f,0.20f);
        out_center_y=std::clamp((top+down)/(4.0f*source_tan),-0.20f,0.20f);
        return true;
    };

    struct VrHudSafeArea {
        float left=0.0f, top=0.0f, right=1920.0f, bottom=1080.0f;
        float common_left=0.0f, common_right=0.0f, common_up=0.0f, common_down=0.0f;
        float safe_left_angle=0.0f, safe_right_angle=0.0f, safe_up_angle=0.0f, safe_down_angle=0.0f;
        float source_tan_x=0.0f, source_tan_y=0.0f, zoom=1.0f, center_x=0.0f, center_y=0.0f;
    };
    bool f4_vrhud_safe_active=false;
    bool f4_vrhud_safe_desired=true; // V64.20 centered HUD default.
    std::uint64_t f4_vrhud_last_apply_attempt_ms=0;
    std::uint64_t f4_vrhud_last_readback_ms=0;
    auto compute_legacy_world_hud_safe_area=[&](VrHudSafeArea& out)->bool {
        out=VrHudSafeArea{};
        if(!xr_manager_) return false;
        XrManager::RenderFrameState rf{};
        if(!xr_manager_->CopyActiveRenderFrameState(rf) || !rf.stereo_views_valid) return false;

        // Cyclopean/common visible angular region: only use rays visible to both
        // eyes so a monoscopic Flash HUD never depends on one eye's outer fringe.
        const float commonLeft=std::max(rf.eye_fov[0].angleLeft,rf.eye_fov[1].angleLeft);
        const float commonRight=std::min(rf.eye_fov[0].angleRight,rf.eye_fov[1].angleRight);
        const float commonUp=std::min(rf.eye_fov[0].angleUp,rf.eye_fov[1].angleUp);
        const float commonDown=std::max(rf.eye_fov[0].angleDown,rf.eye_fov[1].angleDown);
        if(!(commonLeft<0.0f && commonRight>0.0f && commonDown<0.0f && commonUp>0.0f)) return false;

        const float leftTan=std::tan(commonLeft), rightTan=std::tan(commonRight);
        const float downTan=std::tan(commonDown), upTan=std::tan(commonUp);
        if(!(rightTan-leftTan>0.20f) || !(upTan-downTan>0.20f)) return false;

        // Test constants, intentionally conservative. They shrink the actual
        // common OpenXR angular field rather than assuming Quest pixels/FOV.
        const float kComfortHorizontal=std::clamp(vr_ui_subtitle_scale_.load(std::memory_order_acquire),0.45f,0.90f);
        const float kComfortVertical=std::clamp(kComfortHorizontal*0.83f,0.38f,0.75f);
        const float cxTan=0.5f*(leftTan+rightTan);
        const float cyTan=0.5f*(downTan+upTan);
        const float hxTan=0.5f*(rightTan-leftTan)*kComfortHorizontal;
        const float hyTan=0.5f*(upTan-downTan)*kComfortVertical;
        const float safeLeftTan=cxTan-hxTan, safeRightTan=cxTan+hxTan;
        const float safeDownTan=cyTan-hyTan, safeUpTan=cyTan+hyTan;

        std::uint32_t sw=1920,sh=1800; xr_manager_->GetLastCapturedSourceResolution(sw,sh);
        const float aspect=sh?static_cast<float>(sw)/static_cast<float>(sh):(1920.0f/1800.0f);
        const float sourceVfov=std::clamp(vr_source_vertical_fov_deg_.load(std::memory_order_acquire),20.0f,160.0f);
        const float sourceTanY=std::tan(sourceVfov*0.008726646259971648f);
        const float sourceTanX=sourceTanY*std::max(aspect,0.5f);
        if(!(sourceTanX>0.05f && sourceTanY>0.05f)) return false;
        const float zoom=std::clamp(vr_zoom_.load(std::memory_order_acquire),0.25f,4.0f);
        const float invZoom=1.0f/zoom;
        const float centerX=std::clamp(vr_presentation_center_x_.load(std::memory_order_acquire),-0.20f,0.20f);
        const float centerY=std::clamp(vr_presentation_center_y_.load(std::memory_order_acquire),-0.20f,0.20f);

        // V64.77: restore the V64.47 HUD reference mapping.  The headset safe
        // rectangle is derived from the source FOV/UV center only.  Automatic
        // projection-center pitch correction belongs to world rendering and must
        // not tilt/re-anchor Flash semantic HUD coordinates.
        auto flashX=[&](float tanX){ return (0.5f + centerX + (tanX/(2.0f*sourceTanX))*invZoom)*1920.0f; };
        auto flashY=[&](float tanY){ return (0.5f + centerY - (tanY/(2.0f*sourceTanY))*invZoom)*1080.0f; };
        float l=flashX(safeLeftTan), r=flashX(safeRightTan);
        float t=flashY(safeUpTan), b=flashY(safeDownTan);
        l=std::clamp(l,80.0f,880.0f); r=std::clamp(r,1040.0f,1840.0f);
        t=std::clamp(t,45.0f,480.0f); b=std::clamp(b,600.0f,1035.0f);
        if(!(r-l>300.0f && b-t>220.0f)) return false;
        // V64.57: IMPORTANT invariant: the XR-derived safe rectangle stays fixed.
        // User X/Y controls are applied later to each semantic HUD anchor, not here.
        // Moving this rectangle (V64.51 behavior) changed KCD2's anchor reference
        // frame and regressed lower/context prompts while the compass still looked OK.

        out.left=l; out.top=t; out.right=r; out.bottom=b;
        out.common_left=commonLeft; out.common_right=commonRight; out.common_up=commonUp; out.common_down=commonDown;
        out.safe_left_angle=std::atan(safeLeftTan); out.safe_right_angle=std::atan(safeRightTan);
        out.safe_up_angle=std::atan(safeUpTan); out.safe_down_angle=std::atan(safeDownTan);
        out.source_tan_x=sourceTanX; out.source_tan_y=sourceTanY; out.zoom=zoom; out.center_x=centerX; out.center_y=centerY;
        return true;
    };

    auto compute_vrhud_safe_area=[&](VrHudSafeArea& out)->bool {
        if(!hud_layer_v6664::Enabled())return compute_legacy_world_hud_safe_area(out);
        ui_surface_geometry::Hud geometry{};std::uint64_t epoch=0;
        if(!xr_manager_||!xr_manager_->GetHudSurfaceGeometry(geometry,epoch))return false;
        out=VrHudSafeArea{};out.left=geometry.safe.left;out.top=geometry.safe.top;
        out.right=geometry.safe.right;out.bottom=geometry.safe.bottom;
        out.source_tan_x=geometry.surface.width/(2*geometry.surface.distance);
        out.source_tan_y=geometry.surface.height/(2*geometry.surface.distance);
        return true;
    };

    auto apply_legacy_vrhud_safe=[&](bool enable)->bool {
        if(!enable) {
            const bool compassOk=flashui_probe::ApplyVrAngularSafeLayout(
                false,0.0f,0.0f,1920.0f,1080.0f,1.0f,0.0f,0.0f,
                0.0f,                 // subtitleY
                1.0f,0.0f,0.0f,      // dialogue
                1.0f,0.0f,0.0f,      // interaction/action hints
                1.0f,0.0f,0.0f,      // control/tutorial prompts
                1.0f,0.0f,            // compass
                1.0f,0.0f,0.0f,       // stats
                1.0f,0.0f,0.0f);      // lower quick slots
            diagnostic_trace::Write(compassOk?"V64.20-CENTERED-HUD OFF restored vanilla semantic baselines":"V64.20-CENTERED-HUD OFF restore partial/failed");
            return compassOk;
        }
        VrHudSafeArea a{};
        if(!compute_vrhud_safe_area(a)) {
            diagnostic_trace::Write("V64.15-F4-VRHUD WAIT no valid actual HUD surface yet; no HUD write performed");
            return false;
        }
        const float hudScale=std::clamp(vr_ui_compactness_.load(std::memory_order_acquire),0.25f,1.20f);
        const float safeW=std::clamp(vr_ui_safe_width_.load(std::memory_order_acquire),0.60f,1.00f);
        const auto vertical30c1=ui_hud_geometry::ManualBorders30c1(a.top,a.bottom,vr_ui_top_boundary_.load(),vr_ui_bottom_boundary_.load());
        const float baseCx=0.5f*(a.left+a.right),halfW=0.5f*(a.right-a.left)*safeW;
        const float safeLeft=baseCx-halfW,safeRight=baseCx+halfW,safeTop=vertical30c1.top,safeBottom=vertical30c1.bottom;
        const float hudOffsetX=std::clamp(vr_ui_center_offset_x_.load(std::memory_order_acquire),-500.0f,500.0f);
        const float hudOffsetY=std::clamp(vr_ui_center_offset_y_.load(std::memory_order_acquire),-500.0f,500.0f);
        const bool ok=flashui_probe::ApplyVrAngularSafeLayout(
            true,safeLeft,safeTop,safeRight,safeBottom,hudScale,hudOffsetX,hudOffsetY,
            vr_ui_subtitle_y_.load(std::memory_order_acquire),
            vr_ui_dialogue_scale_.load(std::memory_order_acquire),
            vr_ui_dialogue_y_.load(std::memory_order_acquire),
            vr_ui_dialogue_inset_pct_.load(std::memory_order_acquire),
            vr_ui_hints_scale_.load(std::memory_order_acquire),
            vr_ui_hints_inset_pct_.load(std::memory_order_acquire),
            vr_ui_hints_y_.load(std::memory_order_acquire),
            vr_ui_control_prompt_scale_.load(std::memory_order_acquire),
            vr_ui_control_prompt_x_.load(std::memory_order_acquire),
            vr_ui_control_prompt_y_.load(std::memory_order_acquire),
            vr_ui_compass_scale_.load(std::memory_order_acquire),
            vr_ui_compass_y_.load(std::memory_order_acquire),
            vr_ui_stats_scale_.load(std::memory_order_acquire),
            vr_ui_stats_x_.load(std::memory_order_acquire),
            vr_ui_stats_y_.load(std::memory_order_acquire),
            vr_ui_lower_scale_.load(std::memory_order_acquire),
            vr_ui_lower_y_.load(std::memory_order_acquire),
            vr_ui_lower_spread_.load(std::memory_order_acquire),vertical30c1.center);
        constexpr float kRadToDeg=57.29577951308232f;
        char m[768]{};
        std::snprintf(m,sizeof(m),
            "V64.57-CENTERED-HUD %s commonDeg=[L%.2f R%.2f U%.2f D%.2f] safeDeg=[L%.2f R%.2f U%.2f D%.2f] flashSafe=[%.1f,%.1f..%.1f,%.1f] sourceTan=(%.4f,%.4f) zoom=%.3f uvCenter=(%+.4f,%+.4f) size=%.2f comfort=%.2f hudOffset=(%+.0f,%+.0f)",
            ok?"ON":"APPLY-FAILED",double(a.common_left*kRadToDeg),double(a.common_right*kRadToDeg),double(a.common_up*kRadToDeg),double(a.common_down*kRadToDeg),
            double(a.safe_left_angle*kRadToDeg),double(a.safe_right_angle*kRadToDeg),double(a.safe_up_angle*kRadToDeg),double(a.safe_down_angle*kRadToDeg),
            double(safeLeft),double(safeTop),double(safeRight),double(safeBottom),double(a.source_tan_x),double(a.source_tan_y),double(a.zoom),double(a.center_x),double(a.center_y),
            double(hudScale),double(std::clamp(vr_ui_subtitle_scale_.load(std::memory_order_acquire),0.45f,0.90f)),
            double(vr_ui_center_offset_x_.load(std::memory_order_acquire)),double(vr_ui_center_offset_y_.load(std::memory_order_acquire)));
        diagnostic_trace::Write(m); debug_trace::Write(m);
        return ok;
    };
    // Corrected HUD remains the established baseline. Palm HUD is the persistent VR-menu setting.
    // F2 has no presentation rung and cannot change HUD/map state.
    hud_session_rung57c1_.store(0);
    flashui_probe::SetHudExperiment36(true,false);
    hud_layer_v6664::SetDesktopMirror36(true);
    (void)activity_fader47c1::Install();
    vr_hud_reapply_requested_.store(true);
    ui_hud_experiment36c5::PalmReveal palmReveal36;
    enum class HudLayoutOwner {None,Legacy,NativeCanvas,UniformReadable,WholeReadable};
    HudLayoutOwner hud_layout_owner=HudLayoutOwner::None;
    auto whole_readable_wanted44c2=[&](){return vr_auto_correct_hud_.load()||true||(vr_contextual_hud_.load()&&visual_selection59c7f::Selected(frik_pose_probe::CalibrationReady(),frik_pose_probe::CombatCensusMode(),frik_pose_probe::FinalSkinTestMode()));};
    bool f4WristApplied=false; // shared ownership: restore children BEFORE fitting native parents
    auto apply_f4_vrhud_safe=[&](bool enable)->bool {
        const bool uniform=vr_auto_correct_hud_.load(std::memory_order_acquire);
        const bool anchored=vr_hud_native_canvas_fit_.load(std::memory_order_acquire);
        const auto desired=!enable?HudLayoutOwner::None:
            (whole_readable_wanted44c2()?HudLayoutOwner::WholeReadable:(uniform?HudLayoutOwner::UniformReadable:(anchored?HudLayoutOwner::NativeCanvas:HudLayoutOwner::Legacy))); // fixed-center readable parents only
        // Restore the previous owner first; NativeCanvas and UniformReadable share
        // one transaction, so either restore path returns only readable parents.
        if(hud_layout_owner!=desired){
            bool restored=true;
            if(hud_layout_owner==HudLayoutOwner::NativeCanvas||hud_layout_owner==HudLayoutOwner::UniformReadable||hud_layout_owner==HudLayoutOwner::WholeReadable)
                restored=flashui_probe::ApplyNativeCanvasLayout(false,{},1.f);
            else if(hud_layout_owner==HudLayoutOwner::Legacy)
                restored=apply_legacy_vrhud_safe(false);
            if(!restored)return false;
            hud_layout_owner=HudLayoutOwner::None;
        }
        if(desired==HudLayoutOwner::None)return flashui_probe::ApplyNativeCanvasLayout(false,{},1.f);
        if(desired==HudLayoutOwner::Legacy){
            if(!flashui_probe::ApplyNativeCanvasLayout(false,{},1.f))return false;
            const bool ok=apply_legacy_vrhud_safe(true);
            hud_layout_owner=HudLayoutOwner::Legacy;return ok;
        }
        if(f4WristApplied){
            if(!flashui_probe::ApplyWristHudLayout(false,0,0,0,0,1.f,false,false))return false;
            f4WristApplied=false;
        }
        ui_surface_geometry::Hud geometry{};std::uint64_t epoch=0;
        if(!hud_layer_v6664::Enabled()||!xr_manager_||!xr_manager_->GetHudSurfaceGeometry(geometry,epoch))return false;
        bool ok=false;
        if(desired==HudLayoutOwner::WholeReadable){
            ok=flashui_probe::ApplyWholeReadableHud44c2(true,geometry,vr_ui_compactness_.load(),vr_ui_safe_width_.load(),vr_ui_top_boundary_.load(),vr_ui_bottom_boundary_.load(),vr_ui_safe_height_.load(),auto_hud_icon_scale_.load());
            // The corrected F2 owner also honours the health/compass/quick-slot
            // controls. Previously these sliders only affected the other layout.
            if(ok)(void)flashui_probe::ApplyUniformHudElements171(vr_ui_compass_scale_.load(),vr_ui_compass_y_.load(),
                vr_ui_stats_scale_.load(),vr_ui_lower_scale_.load(),vr_ui_lower_y_.load(),vr_ui_lower_spread_.load(),vr_ui_stats_x_.load(),vr_ui_stats_y_.load());
        }else if(desired==HudLayoutOwner::UniformReadable){
            ok=flashui_probe::ApplyUniformReadableHudLayout(true,geometry,
                vr_ui_compactness_.load(std::memory_order_acquire),
                vr_ui_safe_width_.load(std::memory_order_acquire),
                vr_ui_top_boundary_.load(std::memory_order_acquire),
                vr_ui_bottom_boundary_.load(std::memory_order_acquire),
                vr_ui_center_offset_x_.load(std::memory_order_acquire),
                0.0f,vr_ui_dialogue_scale_.load(),vr_ui_hints_scale_.load(),vr_ui_control_prompt_scale_.load(),vr_ui_subtitle_scale_.load()); // authored vertical center fixed; top/bottom boundaries own Y
            if(ok)(void)flashui_probe::ApplyUniformHudElements171(vr_ui_compass_scale_.load(),vr_ui_compass_y_.load(),
                vr_ui_stats_scale_.load(),vr_ui_lower_scale_.load(),vr_ui_lower_y_.load(),vr_ui_lower_spread_.load(),vr_ui_stats_x_.load(),vr_ui_stats_y_.load());
            // Dialogue cl/cr and subtitle bc share this same captured-parent owner.
            // Do not overwrite the fit through their semantic aliases.
        }else{
            ok=flashui_probe::ApplyNativeCanvasLayout(true,geometry,
                vr_hud_native_canvas_size_.load(std::memory_order_acquire),
                vr_hud_native_canvas_spread_x_.load(std::memory_order_acquire),
                vr_hud_native_canvas_spread_y_.load(std::memory_order_acquire));
        }
        if(ok||flashui_probe::NativeCanvasActive())hud_layout_owner=desired;
        return ok;
    };


    const int f3_view_marriage_mode=0; // Release baseline
    const int f4_headlook_mode=0; // Release baseline
    bool native_head_rotation_owned=false;
    bool view_marriage_visual_active=false;
    bool pitch_pgup_prev=false, pitch_pgdn_prev=false;
    // V64.76: one controller presentation chord. Left Grip + R3 mirrors F6.
    bool lg_r3_prev=false;
    bool f2_ui_layers_test=false;
    std::uint64_t native_xi_heartbeat_ms=0;
    bool native_xi_active_prev=false;
    bool native_xi_degraded_logged=false;
    int native_xi_discovery_attempts=0;
    std::uint64_t native_xi_next_discovery_ms=0;
    float npcSpacingAttempt=1.f;
    std::uintptr_t npcSpacingModule=0; // Per-Run activation; a restarted Run must rearm its saved setting.
#ifdef KCD2VR_PROXY_DINPUT8
    diagnostics::StartupRetry32 startupRetry32;
#endif
    // Same worker-owned static state; diagnostics must also close on Run exit.
    static semi_workstations46::State semiWorkstations46;
    while (running_)
    {
#ifdef KCD2VR_PROXY_DINPUT8
        startupRetry32.Tick(GetTickCount64(),proxy29::StartupRecordsPending32(),
            []{return diagnostic_trace::Enabled();},[]{diagnostic_trace::SetEnabled(true);},[]{proxy29::FlushStartupRecords32();});
#endif
        diagnostics::xr165::Report(GetTickCount64());
        if(diagnostics::perf_d61::requested()){
            diagnostics::perf_d61::External p{};p.pairs=DeviceHook::BridgeCompletePairCount();p.presents=DeviceHook::PresentCaptureCount();
            p.xrFrames=diagnostics::xr165::frameCount.load(std::memory_order_relaxed);
            for(unsigned source=0;source<2;++source){p.fresh+=pacing59c6::counts[source][0].load(std::memory_order_relaxed);p.reused+=pacing59c6::counts[source][1].load(std::memory_order_relaxed);}
            p.logLines=diagnostic_trace::Accepted();p.f2Rung=smooth_lab::Mode();p.otherCaptureMask=diagnostics::capture168::gate.mask.load(std::memory_order_relaxed);
            diagnostics::perf_d61::service(diagnostics::perf_d61::nowMs(),p,diagnostic_trace::WritePerformanceD61);
        }
        diagnostics::map_d72::workerD84(diagnostics::perf_d61::stageNames,[](diagnostics::map_d72::Map& map,diagnostics::map_d72::External& p){ // D84: no map snapshots or clock calls when capture is OFF.
            const auto intent=held_map_m1::Intent31();const auto c=held_map_m1::Snapshot();
            map={intent.open,held_map_m1::VisibleD72(),intent.serial,c.placements,c.shows,c.hides,c.missing,c.stale,c.poseRefusal,c.lastHideReasonM4};
            if(intent.open||diagnostics::map_d72::active.load(std::memory_order_relaxed)){
                p.pairs=DeviceHook::BridgeCompletePairCount();p.presents=DeviceHook::PresentCaptureCount();p.xrFrames=diagnostics::xr165::frameCount.load(std::memory_order_relaxed);
                for(unsigned source=0;source<2;++source){p.fresh+=pacing59c6::counts[source][0].load(std::memory_order_relaxed);p.reused+=pacing59c6::counts[source][1].load(std::memory_order_relaxed);}
                p.logLines=diagnostic_trace::Accepted();p.f2Rung=smooth_lab::Mode();p.otherCaptureMask=diagnostics::capture168::gate.mask.load(std::memory_order_relaxed);
            }
        },diagnostic_trace::WriteMapPerformanceD72);
        if(diagnostics::frame_d30::anyOn()){ // D30: 1 s performance/jitter (F2) and VRIK (F7) summaries, nothing while both are off
            diagnostics::frame_d30::External ext{};ext.xrFrames=diagnostics::xr165::frameCount.load(std::memory_order_relaxed);
            for(unsigned source=0;source<2;++source){ext.fresh+=pacing59c6::counts[source][0].load(std::memory_order_relaxed);ext.reused+=pacing59c6::counts[source][1].load(std::memory_order_relaxed);}
            ext.inclusiveUs=hookcost178r22::topLevelUs.load(std::memory_order_relaxed);ext.ourUs=hookcost178r22::measuredOwnUs.load(std::memory_order_relaxed);ext.logLines=diagnostic_trace::Accepted();
            ext.handoffNs=diagnostics::xr165::metrics[diagnostics::xr165::HandoffPickup].ns.load(std::memory_order_relaxed);
            ext.handoffCalls=diagnostics::xr165::metrics[diagnostics::xr165::HandoffPickup].calls.load(std::memory_order_relaxed);
            diagnostics::frame_d30::report(GetTickCount64(),ext,[](const char* line){diagnostic_trace::Write(line);});
        }
        combat_diag_probe::DrainNativeServices59C5();
        diagnostics::interaction_cycle_h4::service(GetTickCount64(),diagnostic_trace::Write);
        frik_pose_probe::DrainCopiedDiagnosticsH47(std::uint64_t(GetTickCount64()));
        frik_pose_probe::ServiceDrawDiagnosticsD77();
        diagnostics::physics_d42::drain(std::uint64_t(GetTickCount64()),[](auto domain,std::uint64_t session,const char* line){diagnostic_trace::WriteReceiptFor(domain,session,line);});
        diagnostics::motion_d68::service(diagnostic_trace::WritePerformanceD68,
            smooth_lab::Mode(),hand_smooth_h50::enabled.load(),hand_present_d61::Enabled(),hj1::enabled.load(),tracking59c6::cameraSource.load());
        diagnostic_trace::Drain(); // copied F3/F7/Shift+F2 formatting and batched disk I/O on mod worker
        // V64.42 live angular projection trim. Auto mode adds the FOV-derived
        // angular center; PageUp/PageDown always adjust only the manual trim.
        const bool pitchPgUp=(GetAsyncKeyState(VK_PRIOR)&0x8000)!=0;
        const bool pitchPgDn=(GetAsyncKeyState(VK_NEXT)&0x8000)!=0;
        if ((pitchPgUp && !pitch_pgup_prev) || (pitchPgDn && !pitch_pgdn_prev)) {
            float cur=vr_pitch_correction_deg_.load(std::memory_order_acquire);
            cur=std::clamp(cur + (pitchPgUp ? 0.1f : -0.1f), -15.0f, 15.0f);
            vr_pitch_correction_deg_.store(cur,std::memory_order_release);
            if(xr_manager_) xr_manager_->SetPitchCorrectionDeg(cur);
            char pm[192]{}; std::snprintf(pm,sizeof(pm),"V64.42-PROJECTION-CENTER auto=%d trim=%+.1f effective=%+.2f deg",vr_auto_projection_center_.load(std::memory_order_acquire)?1:0,double(cur),double(xr_manager_?xr_manager_->GetEffectivePitchCorrectionDeg():cur)); diagnostic_trace::Write(pm);
        }
        pitch_pgup_prev=pitchPgUp; pitch_pgdn_prev=pitchPgDn;
        const std::uint64_t ladder_now_ms=static_cast<std::uint64_t>(GetTickCount64());
        {   // V66.178R22b FRAME_BUDGET: the mod's main-loop cadence (what the player feels), every 5 s, always logged.
            static std::uint64_t fbPrev=0,fbWindow=0,fbN=0,fbSum=0,fbMax=0,fbOver50=0;
            if(fbPrev){const auto dt=ladder_now_ms-fbPrev;++fbN;fbSum+=dt;if(dt>fbMax)fbMax=dt;if(dt>50)++fbOver50;}
            // V66.178R30 (R28 review): a loop iteration over 150 ms flushes ONE bounded record of where the previous iteration's
            // time went, from coarse checkpoints (top, after HUD apply, after XR RunFrame, after tracking publish, after pad publish).
            if(fbPrev){const auto dt=ladder_now_ms-fbPrev;if(dt>150){char sm[300]{};std::snprintf(sm,sizeof(sm),"V66.178R30 FRAME_STALL_LOOP ms=%llu topToHud=%lld hudToXrFrame=%lld xrFrameToTracking=%lld trackingToPad=%lld padToNextTop=%lld f4=%d (coarse 16 ms ticks; the largest segment is where the stall lived)",
                (unsigned long long)dt,(long long)(g_stallCp178r30[1]-g_stallCp178r30[0]),(long long)(g_stallCp178r30[2]-g_stallCp178r30[1]),(long long)(g_stallCp178r30[3]-g_stallCp178r30[2]),(long long)(g_stallCp178r30[4]-g_stallCp178r30[3]),(long long)(ladder_now_ms-g_stallCp178r30[4]),frik_pose_probe::CombatExperiment175Enabled()?1:0);diagnostic_trace::Write(sm);}}
            g_stallCp178r30[0]=ladder_now_ms;
            fbPrev=ladder_now_ms;if(!fbWindow)fbWindow=ladder_now_ms;
            if(ladder_now_ms-fbWindow>=5000&&fbN){char m[320]{};static std::uint64_t fbHudGen=0,fbPairs=0,fbPresents=0;const auto hudGenNow=flashui_probe::HudGeneration();
                // V66.178R31: the GAME's render rate (completed stereo pairs and PRESENT captures per second); the XR loop above runs at its own 64 Hz regardless.
                const auto pairsNow=DeviceHook::BridgeCompletePairCount(),presentsNow=DeviceHook::PresentCaptureCount();const double win=double(ladder_now_ms-fbWindow)/1000.0;
                const double gameFps=fbPairs?double(pairsNow-fbPairs)/win:0.0,presentFps=fbPresents?double(presentsNow-fbPresents)/win:0.0;fbPairs=pairsNow;fbPresents=presentsNow;
                std::snprintf(m,sizeof(m),"V66.178R22 FRAME_BUDGET loops=%llu avgMs=%.1f maxMs=%llu over50ms=%llu gameStereoPairsPerSec=%.1f presentsPerSec=%.1f hudGenerationChanges=%llu calibrated=%d f4=%d f3=%d",(unsigned long long)fbN,double(fbSum)/double(fbN),(unsigned long long)fbMax,(unsigned long long)fbOver50,gameFps,presentFps,(unsigned long long)(hudGenNow-fbHudGen),frik_pose_probe::CalibrationReady()?1:0,frik_pose_probe::CombatExperiment175Enabled()?1:0,diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,ladder_now_ms)?1:0);diagnostic_trace::Write(m);fbHudGen=hudGenNow;fbWindow=ladder_now_ms;fbN=fbSum=fbMax=fbOver50=0;}
        }
        // V65.44: startup can set sys_MaxFPS=0 successfully and KCD2 may later reassert 60
        // during frontend/gameplay initialization. Retry until available, then watchdog once/sec
        // while the user-facing manual limiter is OFF. Never fight an explicitly enabled limiter.
        if(!startup_fps_uncap_confirmed && ladder_now_ms>=startup_fps_retry_after_ms) {
            startup_fps_retry_after_ms=ladder_now_ms+250ull;
            startup_fps_uncap_confirmed=DisableNativeGameFpsCap();
            if(startup_fps_uncap_confirmed) diagnostic_trace::Write("V65.44-FPS startup retry CONFIRMED sys_MaxFPS=0");
        }
        if(!menu_fps_cap_enabled_.load(std::memory_order_acquire) && ladder_now_ms>=startup_fps_watchdog_after_ms) {
            startup_fps_watchdog_after_ms=ladder_now_ms+1000ull;
            const float live_cap=ReadCurrentGameFpsCap();
            const int live_vsync=ReadCurrentGameVsync();
            bool cap_fixed=true,vsync_fixed=true;
            if(live_cap>1.0f) cap_fixed=DisableNativeGameFpsCap();
            if(live_vsync>0) vsync_fixed=DisableNativeGameVsync();
            if(live_cap>1.0f || live_vsync>0) {
                char fm[256]{}; std::snprintf(fm,sizeof(fm),
                    "V65.44-FPS WATCHDOG observed sys_MaxFPS=%.3f r_Vsync=%d corrected={cap:%d vsync:%d}",
                    double(live_cap),live_vsync,cap_fixed?1:0,vsync_fixed?1:0); diagnostic_trace::Write(fm);
            }
            startup_fps_uncap_confirmed=(live_cap<=1.0f || cap_fixed);
        }
        const bool developerMode=developer_mode::Enabled();
        const bool f1_down = (GetAsyncKeyState(VK_F1) & 0x8000) != 0; // V66.128 body follow
        const bool f2_down = (GetAsyncKeyState(VK_F2) & 0x8000) != 0;
        const bool f5_down = (GetAsyncKeyState(VK_F5) & 0x8000) != 0;
        const bool f6_down = (GetAsyncKeyState(VK_F6) & 0x8000) != 0;
        const bool f7_down = (GetAsyncKeyState(VK_F7) & 0x8000) != 0;
        const bool f8_down = (GetAsyncKeyState(VK_F8) & 0x8000) != 0;
        const bool f8_pressed = f8_down && !f8_cal_key_prev;
        const bool shift_down = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        const bool ui88_diag_down=developerMode&&(GetAsyncKeyState(VK_CONTROL)&0x8000)&&(GetAsyncKeyState(VK_MENU)&0x8000)&&(GetAsyncKeyState(VK_F9)&0x8000);
        const bool f9_down = (GetAsyncKeyState(VK_F9) & 0x8000) != 0 && !ui88_diag_down;

        const bool f10_down = (GetAsyncKeyState(VK_F10) & 0x8000) != 0;
        const bool f3_down = (GetAsyncKeyState(VK_F3) & 0x8000) != 0;
        const bool f11_down = (GetAsyncKeyState(VK_F11) & 0x8000) != 0;
        const bool f12_down = (GetAsyncKeyState(VK_F12) & 0x8000) != 0;

        // V64.17 synchronize menu/controller display override with the existing F6 authority.
        // V65.03 presentation has one persistent policy: AUTO. F6/L3+R3 is the
        // temporary scene-local escape hatch and can never rewrite this policy.
        const int wanted_display_override=0;
        if(vr_display_mode_override_.load(std::memory_order_acquire)!=0)
            vr_display_mode_override_.store(0,std::memory_order_release);
        if(wanted_display_override!=applied_display_override){
            applied_display_override=wanted_display_override; scene_emergency_override=-1;
            diagnostic_trace::Write("V65.23-MENU-POLICY game-state-only; manual scene override retired");
            startup_ready_since_ms=0;
        }

        // D48: five finite recorder groups. F2/F5 retain their explicit D46 gameplay comparison rungs.
        static diagnostics::keys168r1::Edges captureKeys;
        const auto capturePress=captureKeys.poll({f2_down,f3_down,f5_down},matrix_active||!developerMode);
        diagnostics::capture168::poll(GetTickCount64());
        diagnostics::native_palette_d67r1::drain(diagnostics::perf_d61::nowMs(),diagnostic_trace::WritePerformanceD68);
        diagnostics::native_palette_d67r1::view::drain(diagnostics::perf_d61::nowMs(),diagnostic_trace::WritePerformanceD68);
        kcd2vr::reload_diagnostics_d48::drain(GetTickCount64(),[](auto domain,std::uint64_t session,const char* line){diagnostic_trace::WriteReceiptFor(domain,session,line);});
        kcd2vr::diagnostics::activity_links_d77::drain(GetTickCount64(), [](auto domain,std::uint64_t session,const char* line){diagnostic_trace::WriteReceiptFor(domain,session,line);});
        {namespace oc=diagnostics::ownership48;oc::Event e{};unsigned budget=256;
         while(budget--&&oc::pop(e)){char line[2048]{};int n=std::snprintf(line,sizeof(line),
          "F2_CAUSAL stage=%u order=%llu session=%llu tick=%llu cpuNs=%llu thread=%u sample=%llu publication=%llu epoch=%llu sourceEpoch=%llu sourceNs=%llu poseTime=%lld actor=%llX object=%llX item=%llX generation=%llu side=%d code=%d flags=%u reason=%s data=",
          e.stage,(unsigned long long)e.order,(unsigned long long)e.session,(unsigned long long)e.tick,(unsigned long long)e.cpuNs,e.thread,
          (unsigned long long)e.sample,(unsigned long long)e.publication,(unsigned long long)e.epoch,(unsigned long long)e.sourceEpoch,
          (unsigned long long)e.sourceNs,(long long)e.poseTime,(unsigned long long)e.actor,(unsigned long long)e.object,(unsigned long long)e.item,
          (unsigned long long)e.generation,e.side,e.code,e.flags,e.reason);
          for(unsigned j=0;j<32&&n>0&&n<int(sizeof(line))-32;++j)n+=std::snprintf(line+n,sizeof(line)-n,"%s%.7g",j?",":"",double(e.v[j]));
          diagnostic_trace::Write(line);
         }
         static std::uint64_t seenDrops=0;const auto drops=oc::dropped.load(std::memory_order_relaxed);
         if(drops!=seenDrops){char line[128]{};std::snprintf(line,sizeof(line),"F2_CAUSAL QUEUE_DROPPED total=%llu delta=%llu",(unsigned long long)drops,(unsigned long long)(drops-seenDrops));diagnostic_trace::Write(line);seenDrops=drops;}
        }

        static bool f7AudibleActiveH29=false;
        {const bool activeH29=diagnostics::capture168::active(diagnostics::capture168::Domain::Vrik,GetTickCount64());
         if(f7AudibleActiveH29&&!activeH29)LabLadderCue(0); // automatic expiry is audible too
         f7AudibleActiveH29=activeH29;
         // D48: recording cannot change the solver. The saved live menu
         // preference owns the optional FRIK algorithm in every recorder rung.
         kcd2vr::frik_elbow_lab::rung2.store(false,std::memory_order_relaxed);}


        {const auto now=std::uint64_t(GetTickCount64());const bool active=diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now);
         const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();static bool was=false;static std::uint64_t seen=0;
         if(active!=was||(active&&session!=seen)){interaction_alignment::SetDoorDiagnosticsCaptureC54R3(active);was=active;seen=session;}}

        if(const int requested=g_vrik_compare_requested_d61.exchange(-1,std::memory_order_acq_rel);requested>=0){
            const unsigned mode=requested==1?2u:requested==2?3u:0u;
            kcd2vr::smooth_lab::SetMode(mode); // sole Run owner; recording mask unchanged
        }
        kcd2vr::smooth_lab::RefreshBaselineSmoothingD39(); // D39: apply live baseline preference on the F2 policy owner
        if(capturePress.hud){
            const auto now=GetTickCount64();
            // D68: Shift+F2 measures performance and bounded motion bursts; no comparison policy changes.
            if(shift_down){
                const auto perfNow=diagnostics::perf_d61::nowMs();
                if(!diagnostics::perf_d61::gate.active(perfNow)){
                    diagnostics::capture168::select(diagnostics::capture168::Domain::Bug,false,now,"SHIFT_F2_PERF_ONLY");
                    diagnostics::capture168::select(diagnostics::capture168::Domain::Hud,false,now,"SHIFT_F2_PERF_ONLY");
                }
                const bool on=diagnostics::perf_d61::toggle(perfNow,diagnostic_trace::WritePerformanceD61);
                LabLadderCue(on?1:0);
                (void)visible_failure::publish(on?"Shift+F2: performance + motion trace (180s)":"Shift+F2: performance metrics off",now);
            }else{
                const unsigned rung=kcd2vr::smooth_lab::Next(kcd2vr::smooth_lab::Mode());
                // D45 comparison ladder is retained: rungs 1/2/3 record F2; 2 changes timing and 3 changes the elbow/shoulder policy.
                diagnostics::capture168::select(diagnostics::capture168::Domain::Bug,kcd2vr::smooth_lab::BugRecordingRung(rung),now,"F2_BUG_DIAGNOSTICS");
                kcd2vr::smooth_lab::SetMode(rung);
                RequestMenuSave(false); // J26: VrF2Rung, so the next start comes up in the same rung
                char mode[288]{};std::snprintf(mode,sizeof(mode),"D64 HOTKEYS: F2_SELECTED=%u bug=%d earlyFrameTiming=%d latePublishTiming=%d elbowShoulder=%d otherModes=UNCHANGED",
                    rung,kcd2vr::smooth_lab::BugRecordingRung(rung)?1:0,kcd2vr::smooth_lab::EarlyPacingRungD45(rung)?1:0,kcd2vr::smooth_lab::PacingRung(rung)?1:0,kcd2vr::smooth_lab::ElbowShoulderSelectedD45()?1:0);
                diagnostic_trace::Write(mode);
                LabLadderCue(rung);
                (void)kcd2vr::visible_failure::publish(kcd2vr::smooth_lab::RungText(rung),now);
            }
        }
        // V66.128 F1: 6DOF body follow (pelvis follows real steps/crouch, leg IK). Plain F-key.
        {static bool f1_prev=false;
         if(!matrix_active&&f1_down&&!f1_prev){const bool on=!vrik_body_follow_.load(std::memory_order_acquire);
             vrik_body_follow_.store(on,std::memory_order_release);frik_pose_probe::SetBodyFollow128(on);CameraHook::SetRoomscaleFollow(((roomscale_d76::Enabled()||g_roomscale_follow58.load())&&on)||g_roomscale_dodge58.load()||frik_pose_probe::CombatExperiment175Enabled()); // R58; R59.16C5
             RequestMenuSave(false);LabLadderCue(on?1:0);} // C25: async INI write
         f1_prev=f1_down;}

        // V66.30: the V65.04/V65.10 renderer-matched pose-to-pixels ownership fix
        // is production behavior again, not an A/B. Keep the trusted fixed-8 ms
        // rotation filter + adaptive-35 pickup, pipeline probe rung 3, and reference
        // XR submission. F2 changes recording only; a key press cannot regress
        // native stereo back to legacy PRESENT-pose ownership.
        v6627_f2_stage=1;
        f2_ownership_fix_.store(true,std::memory_order_release);
        f2_head_probe_level=3;
        if(!FrameSync::HeadOwnershipFixEnabled()) FrameSync::SetHeadOwnershipFixEnabled(true);
        if(FrameSync::HeadOwnershipProbeLevel()!=3) FrameSync::SetHeadOwnershipProbeLevel(3);
        if(xr_manager_ && xr_manager_->StereoStabilityExperimentEnabled()) xr_manager_->SetStereoStabilityExperiment(false);
        if(DeviceHook::V6627PerformanceQuiet()!=kV6634ProductionPerfQuiet) DeviceHook::SetV6627PerformanceQuiet(kV6634ProductionPerfQuiet);
        if(native_stereo_outer_lab::V6627RightPreparationEnabled()) (void)native_stereo_outer_lab::SetV6627RightPreparationExperiment(false);
        // TAA pair-epoch ownership follows MONO/STEREO only, not the F2 recorder.


        // One explicit gameplay choice. Recorders never restore or overwrite it.
        auto read_shortcut167r1=[]() noexcept -> kcd2vr::user175::Configuration {
            return {frik_pose_probe::HolsterCombatDiagMode(),frik_pose_probe::MiscInteractionDiagMode(),
                frik_pose_probe::CombatCensusMode(),frik_pose_probe::WeaponControlMode(),frik_pose_probe::CombatExperiment175Enabled(),frik_pose_probe::PhysicalCombat177Enabled(),frik_pose_probe::CombatFull178r10Enabled()};
        };
        auto apply_user_mode175=[&](kcd2vr::user175::Mode requested) noexcept {
            const bool combatOff44=frik_pose_probe::CombatExperiment175Enabled()&&requested==kcd2vr::user175::Mode::Weapons;
            // Exiting an experiment must work even if native prerequisites disappeared.
            // Keep normal handling if still qualified; otherwise revoke rather than leave combat armed.
            auto resolved44=(combatOff44&&!frik_pose_probe::CalibrationReady())?kcd2vr::user175::Mode::Off:requested;
            resolved44=kcd2vr::user175::default59(resolved44,g_hybrid_default59.load());
            bool ok=(combatOff44||resolved44==kcd2vr::user175::Mode::Off||ensure_vrik_lab())&&
                frik_pose_probe::ApplyUserMode175(resolved44);
            if(!ok&&combatOff44){resolved44=kcd2vr::user175::Mode::Off;ok=frik_pose_probe::ApplyUserMode175(resolved44);}
            if(combatOff44&&resolved44==kcd2vr::user175::Mode::Off)
                diagnostic_trace::Write("C44 F4_OFF combat=REVOKED handling=UNAVAILABLE(native prerequisite or calibration lost)");
            if((requested==kcd2vr::user175::Mode::Combat||requested==kcd2vr::user175::Mode::CombatPhysical)&&(combat_diag_probe::ReadinessMask168r1()&1u))
                combat_diag_probe_installed=true;
            if(ok){CameraHook::SetRoomscaleFollow(((roomscale_d76::Enabled()||g_roomscale_follow58.load())&&frik_pose_probe::BodyFollow128())||g_roomscale_dodge58.load()||frik_pose_probe::CombatExperiment175Enabled());
                KCD2VR_DIAG_LOG(Bug,warning,"V66.176 {} (diagnostics unchanged)",kcd2vr::user175::name(requested));
                LabLadderCue(combatOff44?0:kcd2vr::user175::pressCurrent19c4r2(requested)?kcd2vr::user175::pressCurrent19c4r2(requested): // C4R2: public backend beeps
                    requested==kcd2vr::user175::Mode::Off?0:requested==kcd2vr::user175::Mode::CombatIntegrated?1:requested==kcd2vr::user175::Mode::CombatGeometry?2:requested==kcd2vr::user175::Mode::CombatSynchronized?3:requested==kcd2vr::user175::Mode::CombatContactLab?4:requested==kcd2vr::user175::Mode::CombatImpale?5:requested==kcd2vr::user175::Mode::CombatCombo?4:requested==kcd2vr::user175::Mode::CombatFull?3:requested==kcd2vr::user175::Mode::CombatPhysical?2:1);}
            else MessageBeep(MB_ICONASTERISK);
            return ok;
        };
        // Shift+F8 keeps the established calibration shortcut; plain F8 is reserved.
        auto current_runtime_ipd=[&]() noexcept {
            float ipd=0.064f; XrManager::StereoViewSnapshot sv{};
            if(xr_manager_ && xr_manager_->CopyLatestStereoViews(sv) && sv.raw_ipd>0.03f && sv.raw_ipd<0.10f) ipd=sv.raw_ipd;
            return ipd;
        };

        // Capture the user's original GSM cascade string once. Stereo uses the proven
        // VR2 layout; Mono restores this exact captured string. GSM follows the
        // established render-mode policy, not the F3 capture key.
        static bool golden_gsm_captured=false;
        static char golden_gsm_original[128]{};
        static int golden_gsm_step=2; // Stereo production default is VR2.
        auto golden_apply_gsm=[&](int step,const char* why) noexcept -> bool {
            void* con=SafeGetCryConsole(); void* v=con?SafeConsoleGetCVar(con,"e_GsmSizesWH"):nullptr;
            if(!v) return false;
            if(!golden_gsm_captured){ const char* o=SafeCVarGetString(v); std::snprintf(golden_gsm_original,sizeof(golden_gsm_original),"%s",o?o:"2,8,32,200"); golden_gsm_captured=true; }
            static const char* const kSizes[3]={nullptr,"4,12,40,120,600","6,20,50,150,600"};
            step=std::clamp(step,0,2); const char* want=kSizes[step]?kSizes[step]:golden_gsm_original;
            const bool set=SafeCVarSetString(v,want); const char* got=SafeCVarGetString(v);
            const bool ok=set&&got&&std::strcmp(got,want)==0;
            char m[384]{};std::snprintf(m,sizeof(m),"V66.68-GSM reason=%s step=%d original=\"%s\" target=\"%s\" readback=\"%s\" result=%s",why?why:"?",step,golden_gsm_original,want,got?got:"(null)",ok?"ACTIVE":"REJECTED");
            diagnostic_trace::Write(m); return ok;
        };
        if(capturePress.combat){
            const auto now=GetTickCount64();
            const bool on=!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now);
            unsigned armed=0;
            if(on){
                diagnostics::capture168::select(diagnostics::capture168::Domain::Combat,true,now,"F3");
                if(ensure_vrik_lab()){
                    if(!combat_diag_probe_installed)combat_diag_probe_installed=combat_diag_probe::Install();
                    armed=frik_pose_probe::SetCombatObservation168r1(true);
                }
            const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
            if(base&&retail_build112::Verify(base)){
                static const char* const names[]{"wh_cs_playerinputlockareawidth","wh_cs_playerinputcombatunlockdelay",
                    "wh_cs_playerinputlockingtolerance","wh_cs_playerinputcursorunlockdistance"};
                void* console=SafeGetCryConsole();
                for(const auto* name:names){void* var=console?SafeConsoleGetCVar(console,name):nullptr;
                    const float value=SafeCVarGetFloat(var,std::numeric_limits<float>::quiet_NaN());char line[384]{};
                    std::snprintf(line,sizeof(line),"V66.168 CAMERA_LOCK_CVAR name=%s found=%d value=%.9g readOk=%d mutation=NONE targetIdentity=UNCHANGED",name,var!=nullptr,double(value),std::isfinite(value));
                    diagnostic_trace::Write(line);
                }
            }else diagnostic_trace::Write("V66.168 CAMERA_LOCK_CVAR retail=UNVERIFIED reads=SKIPPED mutation=NONE");
            }else {
                (void)frik_pose_probe::SetCombatObservation168r1(false);
                diagnostics::capture168::select(diagnostics::capture168::Domain::Combat,false,now,"F3");
            }
            const auto modes=read_shortcut167r1();char coverage[640]{};
            std::snprintf(coverage,sizeof(coverage),
                "V66.168R1 COVERAGE domain=COMBAT state=%s armMask=%u installedMask=%u F3mode=%d F5mode=%d F10=%d F12=%d modeWrites=NONE query=CRITERIA_NOT_SELECTED_ACTION npcShapeMapping=NOT_IMPLEMENTED partHitJoin=NOT_IMPLEMENTED",
                on?"BEGIN":"END",armed,combat_diag_probe::ReadinessMask168r1(),modes.combat,modes.interaction,modes.pose,modes.weapon);
            diagnostic_trace::Write(coverage);
            KCD2VR_DIAG_LOG(Bug,warning,"V66.168R1 F3 combat recording {} armMask={} (gameplay modes unchanged)",on?"ON":"OFF",armed);
            LabLadderCue(on?1:0);
        }

        // V66.85 AA OWNERSHIP: the player/game owns DLSS/FSR/DLAA/TAA selection.
        // Mono/Stereo toggles only temporal-history wiring, never r_AntialiasingMode.
        auto golden_observe_native_aa=[&](const char* why) noexcept -> bool {
            void* con=SafeGetCryConsole(); if(!con) return false;
            void* sr=SafeConsoleGetCVar(con,"r_SuperResolution_Mode");
            void* aa=SafeConsoleGetCVar(con,"r_AntialiasingMode");
            if(!sr||!aa) return false;
            nativeUpscalerCvarD82.store(sr,std::memory_order_release);
            const int sr_mode=SafeCVarGetInt(sr,-1), aa_mode=SafeCVarGetInt(aa,-1);
            if(sr_mode<0||aa_mode<0) return false;
            native_upscaler_stereo_d82::SetMode(vr_production_stereo_mode_.load(std::memory_order_acquire)==3,sr_mode);
            native_upscaler_stereo_d82::LogStatus(why);
            char m[320]{}; std::snprintf(m,sizeof(m),
                "V66.85-AA reason=%s superResolution=%d antialiasingMode=%d writes=NONE history=%s",
                why?why:"?",sr_mode,aa_mode,vr_production_stereo_mode_.load(std::memory_order_acquire)==3?"PAIR-EPOCH":"RETAIL-PER-PASS");
            diagnostic_trace::Write(m); return true;
        };

        auto start_guided_vrik_calibration=[&]() noexcept {
            if(!ensure_vrik_lab()){ MessageBeep(MB_ICONASTERISK); return; }
            (void)frik_pose_probe::ApplyUserMode175(kcd2vr::user175::Mode::Off);
            frik_pose_probe::BeginArmLengthCalibration();
            f8_cal_stage=1; f8_cal_stage_ms=ladder_now_ms; f8_cal_beep_index=-1;
            vrik_cal_complete_until_ms=0;
            vrik_auto_enable_after_cal=0; // Calibration enters the explicit upper-body/physical-handling baseline with saved fingers; no deferred override.
            vrik_calibration_ui_state_.store(3,std::memory_order_release);
            if(vr_menu_){ vr_menu_->SetVisible(false); vr_menu_->SetCaptureCountdown(true,3); }
            diagnostic_trace::Write("V66.175 VRIK-CAL MAKE A T-POSE -> 3-2-1 -> capture 2s -> calibrated upper body + physical handling; saved fingers retained; recorders unchanged");
            KCD2VR_DIAG_LOG(Bug,warning,"V66.175 VRIK arm calibration: MAKE A T-POSE; saved fingers retained; success enables calibrated upper body and physical handling");
        };

        if(!matrix_active&&f8_down&&!f8_cal_key_prev&&shift_down){
            if(f8_cal_stage){frik_pose_probe::CancelArmLengthCalibration();f8_cal_stage=0;vrik_auto_enable_after_cal=0;vrik_calibration_ui_state_=0;if(vr_menu_)vr_menu_->SetCaptureCountdown(false,0);}
            else start_guided_vrik_calibration();
        }
        f8_cal_key_prev=f8_down;
        const bool recalibrate_req=vrik_recalibrate_requested_.exchange(false,std::memory_order_acq_rel);
        if(recalibrate_req){start_guided_vrik_calibration();}
        if(vrik_cycle_requested_.exchange(false,std::memory_order_acq_rel)) {
            kcd2vr::start_r111::manual(start_episode_r111,inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire)); // R111: his choice wins this episode
            if(f8_cal_stage){frik_pose_probe::CancelArmLengthCalibration();f8_cal_stage=0;vrik_auto_enable_after_cal=0;vrik_calibration_ui_state_=0;if(vr_menu_)vr_menu_->SetCaptureCountdown(false,0);}
            (void)apply_user_mode175(kcd2vr::user175::hands(frik_pose_probe::CombatCensusMode(),frik_pose_probe::WeaponControlMode()));
        }
        const int requestedF10=vrik_menu_mode_requested_.exchange(-1,std::memory_order_acq_rel);
        if(requestedF10>=0)kcd2vr::start_r111::manual(start_episode_r111,inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire)); // R111: menu VRIK/NATIVE = his choice until the next load
        if(requestedF10==0){
            if(f8_cal_stage>0) frik_pose_probe::CancelArmLengthCalibration();
            f8_cal_stage=0; vrik_auto_enable_after_cal=0; vrik_calibration_ui_state_.store(0,std::memory_order_release);
            if(vr_menu_) vr_menu_->SetCaptureCountdown(false,0);
            (void)apply_user_mode175(kcd2vr::user175::Mode::Off);
            diagnostic_trace::Write("V66.68-VRIK MENU -> NATIVE TOUCH / F10 rung0");
        } else if(requestedF10>0){
            if(requestedF10==5)(void)apply_user_mode175(kcd2vr::user175::Mode::Hands);
            else if(!kcd2vr::calibration_restore43::needTpose(frik_pose_probe::CalibrationStored43())&&ensure_vrik_lab()){
                // C43 (Miki: "I don't need to recalibrate every time - it loads the calibration from the .ini"): the stored
                // F8 profile is applied as is; the session base is re-seated under the headset (same recenter as after F8).
                const bool active=apply_user_mode175(kcd2vr::user175::Mode::Calibrated);
                if(active){CameraHook::RequestRecenter();(void)frik_pose_probe::RestoreCalibration43("MENU_VRIK_ON");}
                diagnostic_trace::Write(active?"C43 CALIBRATION MENU_VRIK_ON storedCalibration=APPLIED tPose=NOT_REQUIRED (RECALIBRATE for a new T-pose)":"C43 CALIBRATION MENU_VRIK_ON storedCalibration=PRESENT modeApply=FAILED");
                if(!active)start_guided_vrik_calibration();
            }
            else start_guided_vrik_calibration(); // explicit menu VRIK selection; F10 hotkey remains separate
        }
        if(vrik_restore_requested_.exchange(false,std::memory_order_acq_rel)){ // C43 menu RESTORE CALIBRATION
            if(frik_pose_probe::CalibrationStored43()){frik_pose_probe::ForceCalibrationRestore43();CameraHook::RequestRecenter();(void)frik_pose_probe::RestoreCalibration43("MENU_RESTORE");
                diagnostic_trace::Write("C43 CALIBRATION MENU_RESTORE recenter=1 verticalReference=STANDING_EYE headAnchor=CALIBRATED");MessageBeep(MB_OK);}
            else {diagnostic_trace::Write("C43 CALIBRATION MENU_RESTORE storedCalibration=NONE (do a T-pose once)");MessageBeep(MB_ICONASTERISK);}
        }

        { // R111 (Miki 5 Nov): "the game should always start in VRIK mode once calibrated once" + "start at Henry's eyes".
          // before R111 nothing switched VRIK on at start: the mode is not saved (g_census_mode starts 0), only the menu
          // VRIK button (C43 MENU_VRIK_ON) did it. now: at every game start and after every save load, once the world
          // has been up 1.5 s, a saved F8 calibration switches VRIK on exactly like that button (stored calibration,
          // recenter, no T-pose). a refused start retries every 2 s (no latch). switching it off yourself is kept until
          // the next load. with VrStartAtHenryEyesR111 the load also recenters once, so the view starts at Henry's eyes.
          namespace sr=kcd2vr::start_r111;
          const auto loadR111=kcd2vr::observed_load_recovery_r96::observe();
          const bool calibratedR111=frik_pose_probe::CalibrationStored43();
          const bool vrikOnR111=vrik_probe_installed&&frik_pose_probe::CombatCensusMode()>=2;
          // R112: the automatic recenter takes the headset as Henry's eyes only while it is steady where the player put
          // his runtime center (OpenXR LOCAL origin = the Oculus/SteamVR recenter). R111 took it wherever it was 1.5 s
          // after the world came up: 42 cm forward (21:36:26) and 18 cm sideways (21:41:09) in the R111 log, so the
          // camera sat outside Henry's head for the rest of the session. see start_r111.h.
          XrPose headR112{};bool headReadyR112=false;float headYawR112=0.f;
          if(CameraHook::CopyHeadPoseR112(headR112)){
              headYawR112=sr::yawDeg(headR112.qx,headR112.qy,headR112.qz,headR112.qw);
              const bool still=sr::steady(start_head_r112,headR112.pos_x,headR112.pos_y,headR112.pos_z,headYawR112,ladder_now_ms);
              headReadyR112=still&&sr::atRuntimeCenter(headR112.pos_x,headR112.pos_y,headR112.pos_z,headYawR112);
          }else start_head_r112.have=false;
          const bool startedRecenterR112=start_episode_r111.recenterDue;
          const auto act=sr::decide(start_episode_r111,loadR111.episode,loadR111.loading,sr::worldReady.load(std::memory_order_acquire),
              ladder_now_ms,sr::vrikOnStart.load(std::memory_order_acquire),sr::henryEyes.load(std::memory_order_acquire),
              calibratedR111,vrikOnR111,f8_cal_stage!=0||frik_pose_probe::FingerCaptureActive(),headReadyR112);
          if(act==sr::Act::StartVrik){
              const auto resolved=kcd2vr::user175::default59(kcd2vr::user175::Mode::Calibrated,g_hybrid_default59.load());
              const bool ok=ensure_vrik_lab()&&frik_pose_probe::ApplyUserMode175(resolved);
              char m[256]{};
              if(ok){
                  CameraHook::SetRoomscaleFollow(((roomscale_d76::Enabled()||g_roomscale_follow58.load())&&frik_pose_probe::BodyFollow128())||g_roomscale_dodge58.load()||frik_pose_probe::CombatExperiment175Enabled());
                  (void)frik_pose_probe::RestoreCalibration43("R111_START");
                  // R112: VRIK is on now; its recenter waits for the head at the runtime center (next ticks, Recenter below)
                  std::snprintf(m,sizeof(m),"R111 VRIK_START on=1 load=%llu tries=%u (saved calibration, recenter at %s when the headset is steady at your runtime center)",(unsigned long long)loadR111.episode,start_episode_r111.tries,
                      sr::henryEyes.load()?"Henry's eyes":"standing eye height");
                  sr::started(start_episode_r111);
              }else std::snprintf(m,sizeof(m),"R111 VRIK_START on=0 load=%llu try=%u reason=%s - retry in 2 s",(unsigned long long)loadR111.episode,start_episode_r111.tries,
                  !vrik_probe_installed?"VRIK hooks not installed":"Henry not ready for the calibrated body (see V66.175 MODE row)");
              if(ok||start_episode_r111.tries==1||start_episode_r111.tries%15==0)diagnostic_trace::WriteStatus(m); // KCD2VR.log only
          }else if(act==sr::Act::Recenter){
              CameraHook::RequestRecenter(); // R111: a save load starts at Henry's eyes too (vertical shift 0, see StandingEyeShift43)
              if(startedRecenterR112)(void)frik_pose_probe::RestoreCalibration43("R112_START_RECENTER");
              char m[256]{};std::snprintf(m,sizeof(m),"R112 AUTO_RECENTER load=%llu by=%s vrik=%d calibrated=%d start=%s head=(%.3f,%.3f,%.3f) yawDeg=%.1f (steady at your runtime center)",
                  (unsigned long long)loadR111.episode,startedRecenterR112?"VRIK_START":"LOAD",vrikOnR111?1:0,calibratedR111?1:0,sr::henryEyes.load()?"HENRY_EYES":"STANDING_EYE",
                  double(headR112.pos_x),double(headR112.pos_y),double(headR112.pos_z),double(headYawR112));
              diagnostic_trace::WriteStatus(m);sr::finished(start_episode_r111,loadR111.episode);
          }else if(act==sr::Act::KeepOrigin){
              // R112: the headset was never steady at the runtime center within 10 s: no jump, the origin you have stays.
              // the menu VRIK button / RESTORE / headset recenter still recenter at once; the next load decides again.
              char m[384]{};std::snprintf(m,sizeof(m),"R112 AUTO_RECENTER skipped load=%llu by=%s head=(%.3f,%.3f,%.3f) yawDeg=%.1f - headset not steady at your runtime center for 10 s, current origin kept (recenter the headset or use the menu)",
                  (unsigned long long)loadR111.episode,startedRecenterR112?"VRIK_START":"LOAD",double(headR112.pos_x),double(headR112.pos_y),double(headR112.pos_z),double(headYawR112));
              diagnostic_trace::WriteStatus(m);sr::finished(start_episode_r111,loadR111.episode);
          }
        }
        if(f8_cal_stage>0){
            const auto elapsed=ladder_now_ms-f8_cal_stage_ms;
            if(f8_cal_stage==1){
                const int sec=int(elapsed/1000ull);
                const int shown=std::clamp(3-sec,1,3);
                vrik_calibration_ui_state_.store(shown,std::memory_order_release);
                if(vr_menu_) vr_menu_->SetCaptureCountdown(true,shown);
                if(sec>=0&&sec<3&&sec!=f8_cal_beep_index){ f8_cal_beep_index=sec; MessageBeep(MB_OK); }
                if(elapsed>=3000ull){
                    f8_cal_stage=2; f8_cal_stage_ms=ladder_now_ms; f8_cal_beep_index=-1;
                    frik_pose_probe::BeginGuidedCalibrationPose(1);
                    frik_pose_probe::RequestCalibrationFromRecenter();
                    vrik_calibration_ui_state_.store(4,std::memory_order_release);
                    if(vr_menu_) vr_menu_->SetCaptureCountdown(true,0);
                    diagnostic_trace::Write("V66.69-VRIK-CAL NOW capture=2s pose=BODY-T-POSE");
                }
            } else if(f8_cal_stage==2 && elapsed>=2000ull){
                const bool ready=frik_pose_probe::CompleteArmLengthCalibration();f8_cal_stage=0;vrik_auto_enable_after_cal=0;
                const bool active=ready&&apply_user_mode175(kcd2vr::user175::Mode::Calibrated);
                vrik_calibration_ui_state_.store(active?5:6,std::memory_order_release);vrik_cal_complete_until_ms=ladder_now_ms+(active?1500ull:4000ull);
                if(vr_menu_){vr_menu_->SetVisible(false);vr_menu_->SetCaptureCountdown(true,active?-1:-2);}
                if(active){CameraHook::RequestRecenter();MessageBeep(MB_OK);}
                else {(void)apply_user_mode175(kcd2vr::user175::Mode::Off);MessageBeep(MB_ICONASTERISK);}
                char status[256]{};std::snprintf(status,sizeof(status),"R59.10C1 VRIK_CAL_RESULT profileReady=%d modeActive=%d automaticEnable=%s",ready,active,active?"SUCCESS":"FAILED");diagnostic_trace::Write(status);
            }
        }
        if(vrik_cal_complete_until_ms && ladder_now_ms>=vrik_cal_complete_until_ms){vrik_cal_complete_until_ms=0;vrik_calibration_ui_state_.store(0,std::memory_order_release);if(vr_menu_)vr_menu_->SetCaptureCountdown(false,0);}
        if(vrik_probe_installed){
            const int status=frik_pose_probe::FingerCaptureStatus();
            if(status!=finger_capture_status_prev){finger_capture_status_prev=status;
                if(status){KCD2VR_DIAG_LOG(Bug,warning,"V66.100 FINGERS: {}",frik_pose_probe::FingerCaptureStatusName(status));
                    if(status==5)MessageBeep(MB_OK);}
            }
        }

        // F4 combat-route switching is retired; calibrated gameplay uses hybrid.


        // R36C4: expiry revokes the experiment too; it cannot remain invisibly armed.
        frik_pose_probe::ServiceInteractionExperiment36();
        // F5 observes the actual failing configuration, with VRIK ON or OFF.
        if(capturePress.interaction){
            // OFF -> diagnostics -> physical Grip -> guided crafting -> OFF.
            // Requested mode remains explicit even when admission fails.
            const bool wasActive=frik_pose_probe::InteractionLifecycleActive168();
            const int current=combat_rules178r10::f5Rung.load(std::memory_order_relaxed);
            const int rung=(current+1)%4;
            combat_rules178r10::f5Rung.store(rung,std::memory_order_relaxed);
            const bool on=rung!=0;
            // Install observers only. This does not select a VRIK mode or calibrate.
            if(on&&!wasActive)(void)ensure_vrik_lab();
            const bool ok=on?(wasActive?frik_pose_probe::InteractionLifecycleReady36():frik_pose_probe::SetInteractionLifecycle165(true)):frik_pose_probe::SetInteractionLifecycle165(false);
            const bool admitted=frik_pose_probe::SetInteractionExperiment36(rung==2||rung==3);
            const auto modes=read_shortcut167r1();char coverage[640]{};
            std::snprintf(coverage,sizeof(coverage),
                "V66.168R1 COVERAGE domain=INTERACTION state=%s lifecycleReady=%d F3mode=%d F5mode=%d F10=%d F12=%d modeWrites=NONE capabilityInventory=D42_COVERAGE nativeGrabStatus=D42_F5_PROP_OWNER stationStages=D42_F5",
                on?"BEGIN":"END",ok?1:0,modes.combat,modes.interaction,modes.pose,modes.weapon);
            diagnostic_trace::Write(coverage);
            KCD2VR_DIAG_LOG(Bug,warning,"C55 F5 rung={} mode={} recording={} experiment={} coverageMask={} (VRIK selection unchanged)",rung,
                rung==3?"GUIDED_CRAFTING+LEGACY_DOOR":rung==2?"PHYSICAL_GRIP+SPATIAL_DOOR":rung==1?"DIAGNOSTICS":"OFF",
                on?"ON":"OFF",rung>=2&&admitted?"ARMED":"OFF",frik_pose_probe::InteractionCoverage175());
            LabLadderCue(rung);
            if(!ok||!admitted)MessageBeep(MB_ICONASTERISK);
        }
        {const auto nowD42=std::uint64_t(GetTickCount64());
            alchemy_finish_cb1::SetCaptureSessionD42(
                diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,nowD42)?
                diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load(std::memory_order_acquire):0);}
        frik_pose_probe::MiscInteractionDiagTick();

        // D84 F9 records physics without selecting or retiring a gameplay mode.
        // Existing physical-weapon/lab choices remain under their menu controls.
        if(developerMode&&!matrix_active && f9_down && !f9_down_prev && !shift_down){
            const auto now=std::uint64_t(GetTickCount64());
            const bool on=!diagnostics::capture168::active(diagnostics::capture168::Domain::Physics,now);
            if(on&&ensure_vrik_lab())combat_diag_probe_installed=combat_diag_probe::Install();
            diagnostics::capture168::select(diagnostics::capture168::Domain::Physics,on,now,"F9_PHYSICS_DIAGNOSTICS");
            if(!on)frik_pose_probe::SetWeaponUpstreamDiagMode(0);
            LabLadderCue(on?1:0);
        }
        if(developerMode&&!matrix_active && f9_down && !f9_down_prev && shift_down){
            if(ensure_vrik_lab()){
                const auto now=std::uint64_t(GetTickCount64());
                if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Physics,now)){
                    frik_pose_probe::SetWeaponUpstreamDiagMode(0);
                    diagnostics::capture168::select(diagnostics::capture168::Domain::Physics,true,now,"SHIFT_F9_UPSTREAM_DIAGNOSTICS");
                }
                frik_pose_probe::CycleWeaponUpstreamDiagMode(); const int rung=frik_pose_probe::WeaponUpstreamDiagMode();
                char m[384]{};std::snprintf(m,sizeof(m),"D84_F9 UPSTREAM rung=%d %s input=NATIVE writes=NONE",rung,frik_pose_probe::WeaponUpstreamDiagModeName(rung));
                diagnostic_trace::WriteFor(diagnostics::capture168::Domain::Physics,m);LabLadderCue(rung);
            } else MessageBeep(MB_ICONASTERISK);
        }

        if(!matrix_active && f10_down && !f10_down_prev&&developer_mode::FunctionKeyAvailable(10,shift_down,(GetAsyncKeyState(VK_CONTROL)&0x8000)!=0)){
            const auto route=diagnostics::keys168r1::f10Route(shift_down,(GetAsyncKeyState(VK_CONTROL)&0x8000)!=0);
            if(route==diagnostics::keys168r1::F10Route::HandsRecorder){
                // Retired extra recorder key: hand references are part of Shift+F7 (R124).
                (void)kcd2vr::visible_failure::publish("Use Shift+F7 to record VRIK and hand references",GetTickCount64());
                LabLadderCue(int(diagnostics::capture168::gate.vrikRung(GetTickCount64())));
            }else{
            if(f8_cal_stage){frik_pose_probe::CancelArmLengthCalibration();f8_cal_stage=0;vrik_auto_enable_after_cal=0;vrik_calibration_ui_state_=0;if(vr_menu_)vr_menu_->SetCaptureCountdown(false,0);}
            kcd2vr::start_r111::manual(start_episode_r111,inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire)); // R111
            if(route==diagnostics::keys168r1::F10Route::NativeCapture&&ensure_vrik_lab()){
                (void)frik_pose_probe::ApplyUserMode175(kcd2vr::user175::Mode::Off);
                frik_pose_probe::SetCombatCensusMode(1);frik_pose_probe::BeginFingerCapture();
            }else {
                const int next=(frik_pose_probe::HandsMode59c7f()+1)%3;
                if(next)(void)ensure_vrik_lab();
                (void)frik_pose_probe::SetHandsMode59c7f(next);
                LabLadderCue(next);
            }
            }
        }
        touch_bindings_.SetVrikInputLabStage(0); // fingers read XR analogs; combat input remains native bridge.
        // V66.138 explicit Right-Grip ownership: old free-look only when VRIK is OFF.
        // F10=3/4 reserve Grip for tracked hands/weapons, so no mouse-look is generated.
        touch_bindings_.SetRightGripFreeLookEnabled(frik_pose_probe::CombatCensusMode()==0);

        if(developerMode&&!matrix_active && f11_down && !f11_down_prev){
            (void)ensure_vrik_lab();
            const int next=(frik_pose_probe::ReloadMode47C2()+1)%3;
            (void)frik_pose_probe::SetReloadMode47C2(next);
            KCD2VR_DIAG_LOG(Bug,warning,"H21 F11 rung {} (1 RELOAD DIAGNOSTICS; 2 RELOAD TESTS: crossbow/rifle staged native request + existing bow experiment; 0 native; saved bow policy preserved)",frik_pose_probe::ReloadMode47C2());
            LabLadderCue(frik_pose_probe::ReloadMode47C2());
        }
        if(developerMode&&!matrix_active && f12_down && !f12_down_prev){
            if(GetAsyncKeyState(VK_SHIFT)&0x8000){
                if(vr_menu_)vr_menu_->ToggleDeepSeekLabH29();
            }else{
                const int test=(kcd2vr::routerH3::mode.load()+1)%3;
                kcd2vr::routerH3::mode.store(test,std::memory_order_release);
                char line[256]{};std::snprintf(line,sizeof(line),
                    "H3 F12 test=%d state=%s liveRouter=UNQUALIFIED nativeRpgDecisions=PRESERVED forcedOutcomes=NONE F4=UNCHANGED deepseekLab=SHIFT_F12",
                    test,test==0?"OFF":test==1?"ROUTER_OBSERVATION":"NATIVE_WINDOWS_ONLY");
                diagnostic_trace::Write(line);KCD2VR_DIAG_LOG(Bug,warning,"{}",line);LabLadderCue(test);
            }
        }
        // Native observation may remain installed for physical feedback. Its log
        // switch follows the F3 lease only, including expiry, never F4/F10/F12.
        if(combat_diag_probe_installed){
            const bool capture=diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64());
            combat_diag_probe::SetEnabled(capture);
            combat_diag_probe::SetVrWeaponHitLogging136(capture);
        }
        // V66.178R10/R13 camera lock policy (KCD2 cvars; the KCD1 "RemoveAutoCameraLockInCombat" values),
        // as a reversible lease: originals read before any write, readback verified, restored on exit/shutdown.
        {
            const int rung=frik_pose_probe::F4Rung178r10();
            {   // R59.16C10R17: one COMBAT_FLOW line every 5 s in the NORMAL log while an F4 mode is on (cumulative receipts; no F3 needed)
                static std::uint64_t flowLog59c17=0;const auto t59c17=std::uint64_t(GetTickCount64());
                if(rung>0&&t59c17-flowLog59c17>=5000){flowLog59c17=t59c17;char a[960]{},b[960]{},c[1536]{},n[2048]{}; // H12 copied event counters; no additional per-frame native reads
                    frik_pose_probe::CombatFlowReceipts59c17(a,sizeof(a));combat_diag_probe::CombatFlowReceipts59c17(b,sizeof(b));
                    kcd2vr::diagnostics::flowH12::format(kcd2vr::diagnostics::flowH12::audit,c,sizeof(c));
                    std::snprintf(n,sizeof(n),"R59.16C10R17 COMBAT_FLOW rung=%d %s %s",rung,a,b);KCD2VR_DIAG_LOG(Bug,warning,"{}",n);diagnostic_trace::Write(n);
                    // The diagnostic queue has a 2048-byte record. Keep this
                    // bounded eligibility receipt separate so it cannot lose
                    // its tail behind the two historical combat summaries.
                    std::snprintf(n,sizeof(n),"H12 COMBAT_ELIGIBILITY rung=%d %s",rung,c);KCD2VR_DIAG_LOG(Bug,warning,"{}",n);diagnostic_trace::Write(n);}
            }
            const int mode=combat_rules178r10::autoLockMode.load(std::memory_order_relaxed);
            const bool cameraOnly50=g_combat_camera_only50.load(std::memory_order_relaxed);
            char m[320]{};if(g_combat_lock178r13.Apply(false,false,rung,mode,m,sizeof(m))){diagnostic_trace::Write(m);KCD2VR_DIAG_LOG(Bug,warning,"{}",m);}
            #include "subsystems/experimental/free_targeting_service_d42.inl"
            {   // R133: free-targeting mode. a mode change re-leases the target cone (restore the old values first,
                // the lease only gives back a value that is still ours) and writes ONE row. a fight that FREE PLUS
                // held writes one row when it ends. per-second rows only while F3 records.
                namespace r=kcd2vr::free_targeting_r133;
                const int modeR133=kcd2vr::free_targeting_d42::enabled.load(std::memory_order_acquire)?kcd2vr::free_targeting_d42::mode():0;
                if(modeR133!=g_combat_cvars52.freeModeR133){
                    char c[1024]{};if(g_combat_cvars52.Apply(false,c,sizeof(c)))diagnostic_trace::Write(c);
                    g_combat_cvars52.Load(g_player_rules52.load(),g_shared_rules52.load(),g_targeting_lease59c8,g_no_lunge594.load(),modeR133);
                }
                static int loggedModeR133=-1;
                if(modeR133!=loggedModeR133){
                    loggedModeR133=modeR133;const auto k=r::cone(modeR133);char m[420]{};
                    std::snprintf(m,sizeof(m),"R133 FREE_TARGETING mode=%d(%s) combatCamera=%s queuedTurnVeto=%s viewHold=%s stickTurn=%s lockDeg=%.0f unlockDeg=%.0f lockAreaM=%.2f unlockHystM=%.2f",
                        modeR133,r::name(modeR133),modeR133?"LEASED_OFF":"NATIVE",modeR133?"ORDINARY_PHASES":"NATIVE",
                        modeR133==r::FreePlus?"ON":"OFF",modeR133==r::FreePlus?"ON":"OFF",
                        double(k.lockAngle)*57.29578,double(k.unlockAngle)*57.29578,double(k.lockArea),double(k.hyst?k.unlockHyst:.2f));
                    diagnostic_trace::Write(m);KCD2VR_DIAG_LOG(Bug,warning,"{}",m);
                }
                const auto nowR133=std::uint64_t(GetTickCount64());
                const bool engagedR133=r::engaged(nowR133);
                static bool wasEngagedR133=false;
                if(engagedR133&&!wasEngagedR133){r::absorbFrames.store(0);r::stickFrames.store(0);r::bleedFrames.store(0);r::absorbedDeg.store(0.f);r::maxOffsetDeg.store(0.f);}
                if(!engagedR133&&wasEngagedR133&&modeR133==r::FreePlus&&r::absorbFrames.load()){
                    char m[360]{};std::snprintf(m,sizeof(m),
                        "R133 FREE_PLUS fight_end gameTurnAbsorbedDeg=%.0f maxViewOffsetDeg=%.0f absorbFrames=%llu stickTurnedFrames=%llu offsetNowDeg=%.0f/%.0f (view/body; runs out while Henry turns back)",
                        double(r::absorbedDeg.load()),double(r::maxOffsetDeg.load()),(unsigned long long)r::absorbFrames.load(),(unsigned long long)r::stickFrames.load(),
                        double(r::offsetView.load())*57.29578,double(r::offsetBody.load())*57.29578);
                    diagnostic_trace::Write(m);KCD2VR_DIAG_LOG(Bug,warning,"{}",m);
                }
                wasEngagedR133=engagedR133;
                static std::uint32_t loggedClearsR133=0;const auto clearsR133=r::clears.load();
                if(clearsR133!=loggedClearsR133){loggedClearsR133=clearsR133;
                    diagnostic_trace::Write("R133 FREE_PLUS view offset cleared: Henry did not turn back under the view within 4 s (one view snap)");}
                static std::uint64_t lastF3R133=0;
                if(modeR133==r::FreePlus&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,nowR133)&&nowR133-lastF3R133>=1000){
                    lastF3R133=nowR133;char m[360]{};std::snprintf(m,sizeof(m),
                        "R133_F3 FREE_PLUS engaged=%d context=%d offsetViewDeg=%.1f offsetBodyDeg=%.1f absorbFrames=%llu absorbedDeg=%.0f stickTurnedFrames=%llu bleedFrames=%llu bleed=%d userTurn=%d",
                        engagedR133?1:0,kcd2vr::free_targeting_d42::contextReady.load()?1:0,double(r::offsetView.load())*57.29578,double(r::offsetBody.load())*57.29578,
                        (unsigned long long)r::absorbFrames.load(),double(r::absorbedDeg.load()),(unsigned long long)r::stickFrames.load(),(unsigned long long)r::bleedFrames.load(),
                        r::bleedSign.load(),r::userTurning(nowR133)?1:0);
                    diagnostic_trace::Write(m);
                }
            }
            if(rung>0)ProbeCombatCvars50();
            {char c[2048]{};if(g_combat_cvars51.Apply(rung>0,c,sizeof(c))){diagnostic_trace::Write(c);KCD2VR_DIAG_LOG(Bug,warning,"{}",c);}} // V66.178R51
            {char c[1024]{};if(g_combat_cvars52.Apply(rung>0,c,sizeof(c))){diagnostic_trace::Write(c);KCD2VR_DIAG_LOG(Bug,warning,"{}",c);}} // V66.178R52
            {char c[220]{};if(g_block_cooldown59c17.Apply(rung>0&&g_exp59c10.noBlockCooldown.load()&&!g_shared_rules52.load(),c,sizeof(c))){diagnostic_trace::Write(c);KCD2VR_DIAG_LOG(Bug,warning,"{}",c);}} // R59.16C10R17
            {char c[220]{};if(g_free_swing38.Apply(rung>0&&g_exp59c10.instantFreeSwing38.load(),c,sizeof(c))){diagnostic_trace::Write(c);KCD2VR_DIAG_LOG(Bug,warning,"{}",c);}} // C38
        }

        flashui_probe::ServiceDiceMarkerR92(); // scoped native cursor lease, no scene/camera writes
        native_stereo_outer_lab::PollSingleRun();
        const int singleRunPhase=native_stereo_outer_lab::SingleRunPhase();
        if(singleRunPhase!=single_run_last_phase) {
            // V66.12: completion is the only verdict sound. No high tone is played
            // merely because a dangerous stage was armed or entered.
            if(singleRunPhase==0 && single_run_last_phase!=0) {
                if(native_stereo_outer_lab::LastRunPassed())V54DoneBeep(); else LabLadderCue(0);
            } else if(singleRunPhase==-2)LabLadderCue(0);
            single_run_last_phase=singleRunPhase;
        }
        if(f9_postfx_step==4 && !native_stereo_outer_lab::SingleRunBusy() && native_stereo_outer_lab::LastRunKind()==5) {
            stereo_design_capture::SetOutputCensusEye(-1,0);
            stereo_design_capture::DumpOutputCensus();
            const bool pass=native_stereo_outer_lab::LastRunPassed() && native_stereo_outer_lab::LastDesignStagePassed()>=4;
            if(xr_manager_) xr_manager_->SetStereoExperiment(0,0.0f);
            stereo_design_capture::SetOutputCensus(false);
            char m[384]{};std::snprintf(m,sizeof(m),
                "V66.18-CONTINUOUS COMPLETE nativeLR=%s action=AUTO-REARM-IF-IMMERSIVE completedFrames=%u lastPairSerial=%llu",
                pass?"PASS":"FAIL",native_stereo_outer_lab::LastDesignStagePassed(),
                (unsigned long long)native_stereo_outer_lab::LastDesignPairSerial());
            diagnostic_trace::Write(m);KCD2VR_DIAG_LOG(Bug,warning,"{}",m);
            f9_postfx_step=0;
            v6619_xr_stereo_applied=0;
            v6619_theater_capture_paused=false;
            v6619_waiting_fresh_pair=false;
            v6619_next_autostart_ms=static_cast<std::uint64_t>(GetTickCount64())+(pass?750ull:2000ull); // no instant re-arm churn
        }

        // V66.12 F10 control: once the final-PRESENT snapshot exists, freeze it and
        // keep presenting that SAME owned COMMON-state image to both XR eyes. This
        // isolates final-backbuffer cache + D3D12 copy + XR blit plumbing.
        if(f10_bridge_step!=0 && xr_manager_ && xr_manager_->V6589PackedFrameSerial()!=f10_bridge_last_pair_serial) {
            f10_bridge_last_pair_serial=xr_manager_->V6589PackedFrameSerial();
            ++f10_bridge_completed_frames;
            xr_manager_->SetV6589PackedCaptureEnabled(false);
            char fm[448]{}; std::snprintf(fm,sizeof(fm),
                "V66.12-F10 FINAL-PRESENT-CACHE READY frameSerial=%llu mode=FROZEN-BOTH capture=OFF source=real-submitted-PRESENT expectedVisual=NORMAL-GAME-IN-BOTH-EYES",
                (unsigned long long)f10_bridge_last_pair_serial); diagnostic_trace::Write(fm); KCD2VR_DIAG_LOG(Bug,warning,"{}",fm);
            LabLadderCue(f10_bridge_step);
        }

        // V66.31: F9-F12 ownership is handled above by the restored V66.26 effect ladder.
        v6590_f10_done: ;
        // R124 (Miki): plain F7 = VR menu, Shift+F7 = the VRIK recorder ladder (was the other way round).
        if(!matrix_active&&f7_down&&!f7_down_prev&&developer_mode::FunctionKeyAvailable(7,shift_down)){
            if(!shift_down){if(vr_menu_){vr_menu_->Toggle();diagnostic_trace::Write(vr_menu_->IsVisible()?"R124-F7 VR menu opened":"R124-F7 VR menu closed");}}
            else {const auto now=std::uint64_t(GetTickCount64());const auto rung=diagnostics::capture168::cycleVrik(now,"Shift+F7");motion_trials_d67::SetMode(rung);f7AudibleActiveH29=rung!=0;LabLadderCue(int(rung));
                kcd2vr::frik_elbow_lab::rung2.store(false,std::memory_order_relaxed); // solver is a menu preference
                diagnostic_trace::WriteFor(diagnostics::capture168::Domain::Vrik,rung==3?"D42_F7 D67 rung=3 ELBOW_PREFERENCE":rung==2?"D42_F7 D67 rung=2 FRESH_PUBLICATION_TIMING":rung==1?"D42_F7 D67 rung=1 BASELINE_DIAGNOSTICS":"D42_F7 D67 rung=0 OFF");}

        }
        // V66.68: VR-menu VRIK requests were consumed by the guided-calibration block above.
        if(f8_research_override && f8_stereo_probe_level>0 && ladder_now_ms>=f8_next_status_ms){
            f8_next_status_ms=ladder_now_ms+1000ull; const auto fs=native_vr_control_plane_lab::GetStatus();
            char m[1024]{}; std::snprintf(m,sizeof(m),
                "V65.30-F8-SUM rung=%d name=%s supported=%d runtimeHooks=%d oldReadPE={calls:%llu} oldStereoPE={calls:%llu} nativeEye=%.6f policy=READ-ONLY startupTheory=ACTIVE duplicateRenders=0",
                fs.mode,native_vr_control_plane_lab::ModeName(fs.mode),fs.supported_build?1:0,fs.flow_process_hooks_installed?1:0,
                (unsigned long long)fs.read_process_calls,
                (unsigned long long)fs.stereo_process_calls,double(fs.bridge_written_eye_dist));
            diagnostic_trace::Write(m);
        }

        if (!matrix_active && f6_down && !f6_down_prev) {
            // R124: F6 toggles. Theater on screen -> Immersive (the V65.29 panic + an Immersive override for an
            // authored cutscene/dialogue the panic alone never released); Immersive -> flat Theater until F6 again
            // or the next native menu/scene change (theater_toggle_r124.h).
            if(kcd2vr::theater_toggle_r124::Press(theater_toggle_r124,theater_shown_prev_r124,true)) emergency_toggle_requested=true;
            else std::thread([](){ Beep(760,70); Beep(520,90); }).detach();
            startup_ready_since_ms=0;
            char m[160]{};std::snprintf(m,sizeof(m),"R124-F6 TOGGLE shown=%s force=%s panic=%d",theater_shown_prev_r124?"THEATER":"IMMERSIVE",
                kcd2vr::theater_toggle_r124::Name(theater_toggle_r124.force),emergency_toggle_requested?1:0);diagnostic_trace::Write(m);
        }
        if (vr_menu_ && vr_menu_->IsVisible()) {
            vr_menu_->PumpInput();
            // V58.2: corrected user layout. The LEFT stick is the menu D-pad,
            // while the right controller keeps the pointer ray, trigger, A and B.
            // Either controller may keep navigation alive if the other hand is
            // temporarily untracked; right-hand selection simply remains idle.
            const auto& lc=xr_manager_->LeftController();
            const auto& rc=xr_manager_->RightController();
            int px=0,py=0; const bool pv=xr_manager_->GetMenuPointer(px,py);
            const float lmag=lc.thumb_x*lc.thumb_x+lc.thumb_y*lc.thumb_y;
            const float rmag=rc.thumb_x*rc.thumb_x+rc.thumb_y*rc.thumb_y;
            const float navx=(rmag>lmag)?rc.thumb_x:lc.thumb_x;
            const float navy=(rmag>lmag)?rc.thumb_y:lc.thumb_y;
            vr_menu_->PumpVrInput(lc.active || rc.active,px,py,pv,rc.trigger,navx,navy,rc.a_pressed,rc.b_pressed);
        }
        // V62.4 AUTO F6 default-key fallback for keyboard-opened APSE screens.
        // These are intentionally only fallbacks (custom key remaps are not inferred).
        const bool key_i=(GetAsyncKeyState('I')&0x8000)!=0;
        const bool key_m=(GetAsyncKeyState('M')&0x8000)!=0;
        const bool key_esc=(GetAsyncKeyState(VK_ESCAPE)&0x8000)!=0;
        static std::uint64_t suppress_inventory_key_fallback_until_ms=0;
        static std::uint64_t suppress_map_key_fallback_until_ms=0;
        const std::uint64_t key_fallback_now_ms = static_cast<std::uint64_t>(GetTickCount64());
        if(!(vr_menu_&&vr_menu_->IsVisible())) {
            // V66.68: keyboard I/M are game input only; they never own VR presentation.
            // Left Menu is the explicit Inventory presentation request and native lifecycle closes it.
            if(key_i&&!key_i_prev && key_fallback_now_ms>=suppress_inventory_key_fallback_until_ms)
                diagnostic_trace::Write("V66.68-MENU keyboard I observed; presentation ownership unchanged");
            if(key_m&&!key_m_prev && key_fallback_now_ms>=suppress_map_key_fallback_until_ms)
                diagnostic_trace::Write("V66.68-MENU keyboard M observed; presentation ownership unchanged");
            if(key_esc&&!key_esc_prev) {
                diagnostic_trace::Write("V66.120R3-MENU Escape observed; native visibility owns pause/close, no speculative Pause request");
            }
        }
        key_i_prev=key_i; key_m_prev=key_m; key_esc_prev=key_esc;

        ensure_good_mono_projection();
        xr_manager_->SetFlatUiScale(vr_flat_screen_scale_.load(std::memory_order_relaxed));
        xr_manager_->SetFlatUiIpdMm(0.0f);

        const float published_fov_rad = FrameSync::LatestPublishedFovRadians();
        const float published_fov_deg = published_fov_rad * 57.29577951308232f;
        const std::uint64_t published_fov_samples = FrameSync::PublishedFovSampleCount();
        const bool have_published_fov = published_fov_samples > 0 && published_fov_rad > 0.50f && published_fov_rad < 2.70f;
        const std::uint64_t now_ms = static_cast<std::uint64_t>(GetTickCount64());
        // MENUFIX: transition/request driven only. Never rewrite centered HUD every 250 ms while VR menu is open.
        // V63.6 generic APSE/item-menu owner. Trading/cooking/item-selection screens show the
        // game cursor even when they are not opened by I/M/ESC and do not set dialogue state.
        const HWND current_game_hwnd=FindMainGameWindow();
        const bool raw_game_cursor_visible=IsGameUiCursorVisible(current_game_hwnd);
        const bool game_cursor_visible=!(vr_menu_&&vr_menu_->IsVisible()) && raw_game_cursor_visible;

        // D39 live map preference. Installation stays on the existing Run owner,
        // never in the menu's Win32/render callback. A changed intent wins over
        // an in-flight installation result and is serviced on the next iteration.
        if(g_held_map_apply_d39.exchange(false,std::memory_order_acq_rel)){
            const bool wanted=g_held_map59c10r10.load(std::memory_order_acquire);
            const bool ready=wanted&&held_map_m1::Install();
            if(g_held_map59c10r10.load(std::memory_order_acquire)==wanted){
                held_map_m1::Enable(ready);
                diagnostic_trace::Write(!wanted?"D39 HELD_MAP live=OFF":ready?"D39 HELD_MAP live=ON":"D39 HELD_MAP requested=ON qualifiedPins=UNAVAILABLE");
            }else g_held_map_apply_d39.store(true,std::memory_order_release);
        }
        const auto native_ui_snapshot=ui_native_state::Read();
        { // D56: native UI recreation/visibility edge, never a periodic refresh.
            // R129: no longer gated on the CVar lease (g_f7_prompt_active_d33). That lease failed at mod start, so after a
            // restart / save load the saved PlayStation art was never re-sent until a VR menu click. The first pass here
            // (statics start at ~0) is also the start-up retry.
            static kcd2vr::glyph_apply_r129::Edge promptEdge;
            if(promptEdge.changed(flashui_probe::HudGeneration(),native_ui_snapshot.epoch,native_ui_snapshot.visible))
                ResendGlyphStyleOnUiChangeR129(vr_pad_glyph_style_.load(std::memory_order_acquire));
        }
        // C59: semantic native load UI edge requests cleanup; attachment setters
        // run only on the existing native frame owner, never this policy thread.
        static bool goreLoading59=false;
        const bool loading59=native_ui_snapshot.Open(ui_native_state::Loading)||native_ui_snapshot.Open(ui_native_state::Progress);
        if(loading59&&!goreLoading59)frik_pose_probe::RequestGoreVisualRestore59();
        // R97: the session-latch re-arm (core/load_rearm_r96.h) reads the T8 observed load episode published just below;
        // it has no separate counter here any more (one load signal, no double reset).
        goreLoading59=loading59;
        // T8: native load observations rearm finite stock after every save load.
        constexpr auto inventoryLoadMaskT8=ui_native_state::Bit(ui_native_state::Loading)|ui_native_state::Bit(ui_native_state::Progress);
        inventory_lifecycle_t8::PublishUi(native_ui_snapshot.known,native_ui_snapshot.visible,native_ui_snapshot.installed!=0,inventoryLoadMaskT8);
        {static int priorGateT8=-1;const int gateT8=(native_ui_snapshot.known&inventoryLoadMaskT8)!=inventoryLoadMaskT8?0:loading59?1:2;
         if(gateT8!=priorGateT8){priorGateT8=gateT8;char lineT8[192]{};std::snprintf(lineT8,sizeof(lineT8),
             "T8 INVENTORY gate=%s known=%08X loadingMask=%08X gameplayWrites=SCRIPT_OWNER_ONLY",
             gateT8==0?"WAIT_NATIVE_LOAD_OBSERVATION":gateT8==1?"LOADING_BLOCKED":"POST_LOAD_SETTLE",native_ui_snapshot.known,inventoryLoadMaskT8);diagnostic_trace::Write(lineT8);}}
        static bool ui88_diag_prev=false;
        const bool ui89_mark_requested=ui88_diag_down&&!ui88_diag_prev;
        ui88_diag_prev=ui88_diag_down;
        const std::uint32_t passive_menu_mask=native_ui_snapshot.visible; // diagnostics only; no lookup/cursor inference
        // V65.03 startup policy: frontend-close is the explicit Continue/Load handoff.
        // The title/start owner releases immediately; real cutscene/dialogue detection is
        // independent and may re-enter Theater on the very next scene. The agency state
        // below remains useful for scripted/cutscene classification, not startup release.
        static NativeAgencyStateV6419C presentation_agency{};
        static std::uint64_t presentation_agency_next_ms=0;
        if(now_ms>=presentation_agency_next_ms) {
            presentation_agency_next_ms=now_ms+200ull; // V66.89: no cursor-triggered high-frequency agency polling
            presentation_agency=ReadNativeAgencyStateV6419C();
        }
        const bool scripted_agency_flat_ui = presentation_agency.known && presentation_agency.count>=4;
        const bool startup_cutscene_active = cutscene_context_cvar && SafeCVarGetInt(cutscene_context_cvar,0)==1;
        const auto dialogueState59c10r2=activity_lens59c10r1::Dialogue(now_ms);
        const bool startup_dialogue_active = dialogueState59c10r2.active;
        // V65.29 CLEAN MENU ROUTER.
        // Cursor/APSE is INPUT CONTEXT ONLY. It never owns Theater and we never scan
        // Flash panel masks/lifetimes to decide presentation. This removes the old
        // 2-second ProbeInteractionMenuPanels loop and stale APSE Theater latch.
        cursor_menu_flat_ui = game_cursor_visible;
        cursor_menu_panel_mask = 0;
        cursor_menu_apse_hub = false;
        menu_lifetime_kind = MenuLifetimeKind::None;
        cursor_menu_probe_until_ms = 0;
        cursor_menu_probe_next_ms = 0;
        { // R132: the CVar pointer was looked up once at loop start; if the console/CVar was not there yet it stayed null
          // for the whole session. retry every 2 s until found (no latch).
            static std::uint64_t cvarRetryR132=0;
            if(!cutscene_context_cvar&&now_ms>=cvarRetryR132){cvarRetryR132=now_ms+2000ull;
                if(!flat_state_console)flat_state_console=SafeGetCryConsole();
                if(flat_state_console)cutscene_context_cvar=SafeConsoleGetCVar(flat_state_console,"e_SkipInFrustumOptimizationWH");}
        }
        // R132: authored cutscene = the old WH CVar OR the engine's own cut-scene sequence count (CViewSystem). the
        // new-game prologue plays its cutscenes as Track View cut-scene sequences without the CVar (R131 log 7 Nov).
        static kcd2vr::cutscene_signal_r132::Gate viewCutsceneGateR132;
        const int view_cutscene_count_r132=ReadViewSystemCutsceneCountR132();
        const bool cutscene_cvar_r132 = cutscene_context_cvar && SafeCVarGetInt(cutscene_context_cvar,0)==1;
        const bool cutscene_view_r132 = viewCutsceneGateR132.update(view_cutscene_count_r132>0,now_ms);
        const bool cutscene_cvar_flat_ui = cutscene_cvar_r132 || cutscene_view_r132;
        // A count of disabled controls also rises during smithing/alchemy. It
        // protects native body actions, but does not prove a cinematic camera.
        const bool smithingImmersive43=ui_scene_presentation::SmithingImmersive43(
            first_person_activities_.load(std::memory_order_relaxed),
            activity_input163::witness175.ready.load(std::memory_order_acquire),
            activity_input163::witness175.active[unsigned(activity_input163::Kind175::Smithing)].load(std::memory_order_acquire)!=0,
            dialogueState59c10r2,now_ms);
        // A positively observed current ForgeBuilder FirstPerson/Minigame provider
        // is a native activity camera, not a cinematic CVar's theater request.
        const bool forgeImmersiveM1=first_person_activities_.load(std::memory_order_relaxed)&&activity_lens59c10r1::ForgeCameraCurrent(now_ms);
        const bool diceImmersiveD80=first_person_activities_.load(std::memory_order_relaxed)&&activity_lens59c10r1::DiceCameraCurrentD80(now_ms);
        const bool cutscene_flat_ui = cutscene_cvar_flat_ui&&!smithingImmersive43&&!forgeImmersiveM1&&!diceImmersiveD80;
        const auto activity163=activity_input163::Read(native_ui_snapshot,native_activity_controls163_.load());
        const auto& promptStationD74=activity_input163::witness175;
        const bool stationPromptsD74=promptStationD74.ready.load(std::memory_order_acquire)&&
            !promptStationD74.transitionDepth175.load(std::memory_order_acquire)&&
            (promptStationD74.active[unsigned(activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire)||
             promptStationD74.active[unsigned(activity_input163::Kind175::Smithing)].load(std::memory_order_acquire));
        prompt_policy_d80::movieLease.Prepare(stationPromptsD74,flashui_probe::HudGeneration());
        const bool dialogue_active = dialogueState59c10r2.active;
        vr_dialogue_detected_.store(dialogue_active,std::memory_order_release);
        // Native NPC target/animation ownership stays intact. Henry's camera
        // and original-body VRIK ownership are separate below.
        const int dialogue_mode_now = std::clamp(vr_dialogue_camera_mode_.load(std::memory_order_relaxed),0,2);
        dialogue_camera_override::SetDialogueActive(dialogue_active);
        const int native_dialogue_camera_type=dialogue_camera_override::LastNativeType();
        // Mode 2 uses the measured current Henry eye/body anchor below. Never
        // replay a frozen pre-dialogue world position when dialogue moves Henry.
        // Retain the explicitly selected legacy stable option for mode 1 only.
        CameraHook::SetDialogueLabState((g_dialogue_stable5915.load()&&dialogue_mode_now==1)?1:-1,dialogue_active,native_dialogue_camera_type);
        CameraHook::ReportDialogueEmbodiment59c10r2(now_ms);
        { // C43: calibration restore on the EXIT edge of every temporary owner (dialogue, activity/minigame, native body =
          // knockdown/hit reaction/ragdoll, native hand use). Three atomic reads per loop; the restore itself is edge-only.
            static kcd2vr::calibration_restore43::OwnerExit ownerExit43{};
            const bool activityOwner43=(g_activity_lens59c10r1.load()&&activity_lens59c10r1::EyeState(now_ms).active)||activity_input163::witness175.Active();
            const unsigned owners43=(dialogue_active?1u:0u)|(activityOwner43?2u:0u)|(animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)?4u:0u);
            const auto exit43=kcd2vr::calibration_restore43::ownerExit(ownerExit43,owners43);
            if(exit43.restore)(void)frik_pose_probe::RestoreCalibration43(kcd2vr::calibration_restore43::exitOwner(exit43.fell));
        }
        const bool native_dialogue_flat_ui = dialogue_active && dialogue_mode_now==0; // V66.69 Native dialogue uses authored KCD2 cameras in Theater
        clean_menu_owner=ui88_router.Resolve(native_ui_snapshot);
        startup_flat_ui_auto=clean_menu_owner==CleanMenuOwner::Frontend;
        cursor_menu_shop_positive=native_ui_snapshot.Open(ui_native_state::Shop);
        const bool shop_cursor_theater=clean_menu_owner==CleanMenuOwner::Shop;
        if(clean_menu_owner!=CleanMenuOwner::InventoryMap){inventory_close_pending=false;inventory_close_absent_since_ms=0;}
        static std::uint32_t ui88_logged_epoch=~0u;
        static bool ui88_logged_loss=false;
        if(ui88_logged_epoch!=native_ui_snapshot.epoch || (!ui88_logged_loss&&native_ui_snapshot.losses)){
            ui88_logged_epoch=native_ui_snapshot.epoch;ui88_logged_loss=native_ui_snapshot.losses!=0;
            char m[384]{};std::snprintf(m,sizeof(m),
                "V66.89-UI-STATE known=0x%08X visible=0x%08X epoch=%u owner=%d installed=0x%X observations=%llu losses=%llu cursorInformational=%d",
                native_ui_snapshot.known,native_ui_snapshot.visible,native_ui_snapshot.epoch,int(clean_menu_owner),native_ui_snapshot.installed,
                (unsigned long long)native_ui_snapshot.observations,(unsigned long long)native_ui_snapshot.losses,raw_game_cursor_visible?1:0);
            diagnostic_trace::Write(m);
        }
        // Keep old flags as mirrors only; they no longer discover presentation.
        if(clean_menu_owner!=CleanMenuOwner::InventoryMap){ inventory_flat_ui=false; map_flat_ui=false; }
        pause_flat_ui = clean_menu_owner==CleanMenuOwner::Pause;
        // Cursor visibility remains telemetry only. Native catalogue focus is the
        // shared owner; hiding a cursor for gamepad must not close or alter a menu.
        const bool immersive_cursor_menu = false;
        const bool auto_cutscene_theater = cutscene_flat_ui && !dialogue_active; // V66.97 authored cutscene/video Theater; ordinary dialogue stays immersive for HMD gaze
        const bool authored_scene_theater=ui_scene_presentation::AuthoredScene(cutscene_flat_ui,dialogue_active,dialogue_mode_now);
        // Resolve panic before deriving the shared menu presentation decision.
        if(emergency_toggle_requested) {
            emergency_toggle_requested=false;
            ui88_router.Panic(native_ui_snapshot);
            clean_menu_owner=CleanMenuOwner::None;
            startup_flat_ui_auto=false;
            inventory_flat_ui=false; map_flat_ui=false; pause_flat_ui=false;
            diagnostic_trace::Write("V65.29-PRESENTATION PANIC RELEASE -> IMMERSIVE");
            std::thread([](){ Beep(520,70); Beep(760,90); }).detach();
        }

        // One immutable policy snapshot for this iteration. Neither grip/device
        // switching nor HUD/effect visibility can redefine menu presentation.
        const auto ui89_policy=ui88_router.Read();
        clean_menu_owner=ui89_policy.owner;
        const auto ui97_scene=ui_scene_presentation::Resolve(ui89_policy,authored_scene_theater);
        const auto menuCamera59c2=ui_menu_camera59c2::Read();
        static ui_menu_transition59c2::Gate menuTransition59c2;
        bool policy_theater_scene=menuTransition59c2.Theater(ui89_policy,ui97_scene.theater,menuCamera59c2);
        {   // C25: end Inventory Theater on the first native close reaction (ApseModalDialog bit drop) instead of
            // waiting ~0.5 s for the last APSE panel bit. Needs a close press <=1 s ago; restored if the modal
            // returns or the inventory is still up after 1 s. Shop/Map/other owners: unchanged.
            using namespace ui_native_catalogue;
            static ui_inventory_early_exit25::Gate invEarly25;
            const std::uint64_t press25=g_apse_close_press_ms.load(std::memory_order_relaxed);
            ui_inventory_early_exit25::Input in25{};
            in25.enabled=g_inventory_early_exit25.load(std::memory_order_relaxed);
            const std::uint32_t mapBit25=ui_native_state::Bit(ui_native_state::Map);
            // Inventory/Player/Codex/Quest only: the Map (incl. the physical map) keeps its existing exit.
            in25.inventoryOwner=ui89_policy.owner==CleanMenuOwner::InventoryMap&&!physical_map_active&&(native_ui_snapshot.visible&mapBit25)==0;
            in25.modalVisible=(native_ui_snapshot.visible&native_ui_snapshot.known&ModalMask)!=0;
            in25.modalKnownClosed=(native_ui_snapshot.known&ModalMask)!=0&&(native_ui_snapshot.visible&ModalMask)==0;
            in25.rootsVisible=(native_ui_snapshot.visible&native_ui_snapshot.known&(ApseMask&~mapBit25))!=0;
            const auto inventoryBit26c1=ui_native_state::Bit(ui_native_state::Inventory);
            in25.primaryRootVisible=(native_ui_snapshot.visible&native_ui_snapshot.known&inventoryBit26c1)!=0;
            in25.primaryRootKnownClosed=(native_ui_snapshot.known&inventoryBit26c1)!=0&&(native_ui_snapshot.visible&inventoryBit26c1)==0;
            in25.otherPrimaryRootVisible=(native_ui_snapshot.known&native_ui_snapshot.visible&(Mask(Role::ApseRoot)&~inventoryBit26c1))!=0;
            in25.uiUncertain=ui89_policy.uncertain!=0;
            in25.closePressRecent=inventory_close_pending||(press25!=0&&now_ms>=press25&&now_ms-press25<=ui_inventory_early_exit25::kWindowMs);
            in25.nowMs=now_ms;
            const auto edge25=invEarly25.Update(in25);
            if(edge25!=ui_inventory_early_exit25::Edge::None){
                char m[256]{};std::snprintf(m,sizeof(m),"C24 THEATER_EARLY_EXIT %s reason=%s sinceMs=%llu closePending=%d pressAgeMs=%lld nativeVisible=%08X",
                    edge25==ui_inventory_early_exit25::Edge::Begin?"BEGIN":edge25==ui_inventory_early_exit25::Edge::Complete?"COMPLETE":"CANCEL",
                    ui_inventory_early_exit25::Name(edge25),(unsigned long long)(now_ms-invEarly25.SinceMs()),inventory_close_pending?1:0,
                    press25?(long long)(now_ms-press25):-1ll,native_ui_snapshot.visible);
                diagnostic_trace::Write(m);
            }
            if(invEarly25.Active())policy_theater_scene=false; // Active() implies inventoryOwner this tick
        }
        const bool pause_overlay_scene=ui97_scene.pause_overlay;
        const bool death_overlay_scene=ui97_scene.death_overlay;
        const bool immersive_system_overlay=pause_overlay_scene||death_overlay_scene;
        const bool passive_fullscreen_scene=ui97_scene.passive_fullscreen;

        // V66.97: presentation and input stay independent. Pause is a native
        // simulation pause + immersive stereo overlay. Authored cutscenes become
        // passive Theater even when no interactive UI owner is visible.
        const int presentation_scene_key=(authored_scene_theater&&ui89_policy.owner==CleanMenuOwner::None)
            ? 100 : static_cast<int>(ui89_policy.owner);
        if(presentation_scene_key_prev<0) presentation_scene_key_prev=presentation_scene_key;
        else if(presentation_scene_key!=presentation_scene_key_prev) {
            // R132: one row per scene decision, always in KCD2VR.log (not only with F2): which signal made it a cutscene.
            char sm[320]{}; std::snprintf(sm,sizeof(sm),
                "V65.23-SCENE game-state transition %d->%d (%s) cutscene=%s viewCutsceneCount=%d cvar=%d dialogue=%d dialogueMode=%d theater=%d; presentation follows game only",
                presentation_scene_key_prev,presentation_scene_key,presentation_scene_key==100?"AUTHORED_CUTSCENE":ui_menu_router::Name(ui89_policy.owner),
                kcd2vr::cutscene_signal_r132::Source(cutscene_cvar_r132,cutscene_view_r132),view_cutscene_count_r132,cutscene_cvar_r132?1:0,
                dialogue_active?1:0,dialogue_mode_now,policy_theater_scene?1:0);
            diagnostic_trace::WriteStatus(sm);
            presentation_scene_key_prev=presentation_scene_key;
            vr_hud_reapply_requested_.store(true,std::memory_order_release);
            // Visibility snapshot above is passive. Never call GetElement/IsElementPresent
            // from a transition diagnostic: registered does not mean currently visible.

        }
        {   // R124 F6 override: applied after the panic and the scene key of this tick; any scene change ends it.
            const auto forceBefore=theater_toggle_r124.force;
            policy_theater_scene=kcd2vr::theater_toggle_r124::Apply(theater_toggle_r124,presentation_scene_key,policy_theater_scene);
            if(forceBefore!=theater_toggle_r124.force){char m[128]{};std::snprintf(m,sizeof(m),"R124-F6 override ended (scene %d); presentation follows the game again",presentation_scene_key);diagnostic_trace::Write(m);}
        }
        // UI1 preflight: every consumer agrees that a confirmed full-menu owner is Theater.
        // F2 may change HUD experiments, but must not negate only this branch.
        const bool theater_expected = policy_theater_scene;
        inventory_theater_hold_until_ms=0; // V65.23 retired heuristic hold

        // The V118 exclusive-layer policy remains: no gameplay projection behind
        // Theater. Exit follows the observed owner without a fixed delay.
        const bool wasWaiting=v6619_waiting_fresh_pair;
        const bool theater_presented=theater_exit113::update(
            f9_postfx_step==4 && xr_manager_ && !matrix_active, theater_expected,
            v6619_xr_stereo_applied, DeviceHook::BridgeCompletePairCount(),
            v6619_waiting_fresh_pair,v6619_resume_pair_count);
        theater_shown_prev_r124=theater_presented; // R124 F6 direction for the next key press
        // D79: earliest worker boundary with this iteration's native dialogue
        // and final Theater policy. Generation changes and initial Flash resets
        // get bounded retries; no render hook or persistent HUD polling is added.
        const bool ratioWantedD79=dialogue_ratio_d79::Wanted(dialogueState59c10r2,
            dialogue_mode_now,theater_presented,matrix_active,diagnostics::lab::NativeReference(),
            activity_input163::witness175.Active(),ui89_policy,now_ms);
        dialogueRatioD79.Update(ratioWantedD79,dialogueState59c10r2.instance,
            flashui_probe::HudGeneration(),now_ms,[](bool visible,std::uint64_t expectedGeneration){
                dialogue_ratio_d79::Result result{};
                result.applied=flashui_probe::SetHudMovieClipVisible("RatioStrips",visible,
                    expectedGeneration,&result.generation);
                return result;
            });
        // Commit the plane/HUD owner before stereo switches, not later in this tick.
        xr_manager_->SetNativeMenuPlaneMode(theater_presented);
        xr_manager_->SetProductionPresentation59c2(f9_postfx_step==4&&!matrix_active,theater_presented);
        // Edge-only F2 evidence: visibility ownership is not proof that the
        // first menu texture has rendered. The selected native camera now gates
        // APSE/Shop presentation; its output is observed, never changed.
        static int presentation59c1=-1;
        const int nextPresentation59c1=int(ui89_policy.owner)*2+int(theater_presented);
        if(nextPresentation59c1!=presentation59c1){
            presentation59c1=nextPresentation59c1;
            char m[400]{};std::snprintf(m,sizeof(m),
                "R59C2 UI_PRESENTATION_EDGE tick=%llu owner=%s theater=%d nativeKnown=%08X nativeVisible=%08X nativeEpoch=%u cutsceneBit=%d projectionPairs=%llu cameraObserver=%d cameraKnown=%d selectedCamera=%u cameraSerial=%llu delayTimer=NONE",
                (unsigned long long)now_ms,ui_menu_router::Name(ui89_policy.owner),theater_presented,
                native_ui_snapshot.known,native_ui_snapshot.visible,native_ui_snapshot.epoch,cutscene_flat_ui,
                (unsigned long long)DeviceHook::BridgeCompletePairCount(),menuCamera59c2.installed,menuCamera59c2.known,unsigned(menuCamera59c2.camera),(unsigned long long)menuCamera59c2.serial);diagnostic_trace::Write(m);
        }
        if(theater_presented||immersive_system_overlay) CameraHook::SetNativeHeadRotationOwner(false);
        hud_layer_v6664::SetEnabled((vr_production_stereo_mode_.load(std::memory_order_acquire)==3||f2_ui_layers_test)&&!theater_presented);
        if(wasWaiting && !v6619_waiting_fresh_pair)
            diagnostic_trace::Write("R59C2-THEATER-EXIT IMMEDIATE projection=EXCLUSIVE flatLayer=OFF");

        // V66.68 GOLDEN render ownership: exactly two modes, with the narrowest
        // proven deltas. Do NOT capture/restore/write cloud temporal/stereo/fog CVars.
        // KCD2 owns those states. Stereo only owns per-eye history/backing modules,
        // the pair-epoch TAA history selector, VR2 shadow cascades, and stage4 L/R.
        const bool v6668_stereo_requested=vr_production_stereo_mode_.load(std::memory_order_acquire)==3;
        if(!v6668_mode_policy_initialized || v6668_stereo_requested!=v6618_auto_stereo_enabled){
            v6668_mode_policy_initialized=true;
            v6618_auto_stereo_enabled=v6668_stereo_requested;
            native_upscaler_stereo_d82::SetMode(v6668_stereo_requested,
                SafeCVarGetInt(nativeUpscalerCvarD82.load(std::memory_order_acquire),-1));
            native_taa_pair_epoch_v6661::SetEnabled(v6668_stereo_requested);
            native_cloud_right_v6641::SetRightCloudsEnabled(v6668_stereo_requested);
            native_svo_right_v6654::SetEnabled(v6668_stereo_requested);
            hud_layer_v6664::SetEnabled(v6668_stereo_requested); // V66.77: HUD layer = STEREO only
            v6668_gsm_policy_pending=v6668_stereo_requested?2:0;
            v6668_gsm_policy_retry_ms=0;
            v6668_taa_policy_pending=true;
            v6668_taa_policy_retry_ms=0;
            (void)native_stereo_outer_lab::SetV6626NativeStageCensus(false);
            v6626_native_census_started=false;
            if(!v6668_stereo_requested){
                if(f9_postfx_step==4 || native_stereo_outer_lab::SingleRunBusy()){
                    native_stereo_outer_lab::StopSingleRun();
                    native_stereo_outer_lab::SetBridgeStage(0,current_runtime_ipd());
                }
                stereo_design_capture::SetOutputCensus(false);
                if(xr_manager_){ xr_manager_->SetTheaterDrain(false); xr_manager_->SetStereoExperiment(0,0.0f); }
                f9_postfx_step=0; v6619_xr_stereo_applied=0;
                v6619_theater_capture_paused=false; v6619_waiting_fresh_pair=false;
                diagnostic_trace::Write("V66.68-MODE MONO: native single-view + retail/per-pass TAA history + original GSM; stereo per-eye histories OFF; cloud/fog CVars UNTOUCHED");
            } else {
                v6619_next_autostart_ms=0;
                diagnostic_trace::Write("V66.68-MODE STEREO: stage4 requested + pair-epoch TAA + RIGHT cloud/SVO histories + VR2 GSM; cloud/fog CVars UNTOUCHED");
            }
        }
        if(v6668_gsm_policy_pending>=0 && now_ms>=v6668_gsm_policy_retry_ms){
            v6668_gsm_policy_retry_ms=now_ms+500ull;
            if(golden_apply_gsm(v6668_gsm_policy_pending,v6668_gsm_policy_pending==2?"STEREO-PROFILE":"MONO-PROFILE")){
                v6668_gsm_policy_pending=-1;}
        }
        if(v6668_taa_policy_pending && now_ms>=v6668_taa_policy_retry_ms){
            v6668_taa_policy_retry_ms=now_ms+500ull;
            if(golden_observe_native_aa(v6668_stereo_requested?"STEREO":"MONO")) v6668_taa_policy_pending=false;
        }

        // D85 UPSCALERS: restart-free upscaler switch. Runs before the autostart
        // on this same thread; the autostart stays held until it finishes.
        upscaler_live_switch_d85::Instance().Step(now_ms,v6618_auto_stereo_enabled,
            [&]() noexcept {return f9_postfx_step==4||native_stereo_outer_lab::SingleRunBusy();},
            [&]() noexcept {native_stereo_outer_lab::StopSingleRun();},
            [&](const upscaler_live_switch_d85::Selection& sel) noexcept {
                std::vector<std::pair<std::string,int>> values;void* con=SafeGetCryConsole();
                if(!con||!kcd2vr::graphics_owner31::upscalerSettingsR27({sel.tech,sel.quality,sel.preset},values))return false;
                for(const auto& v:values){ // R27 order: quality/preset first, mode last
                    void* cvar=SafeConsoleGetCVar(con,v.first.c_str());
                    if(!cvar||!SafeCVarSetInt(cvar,v.second)||SafeCVarGetInt(cvar,-99)!=v.second)return false;
                }
                return true;},
            [&](int tech) noexcept {return native_upscaler_stereo_d82::CanEnterStereo(tech);},
            [&]() noexcept {
                const auto p=native_upscaler_stereo_d82::LastPublication();
                return upscaler_live_switch_d85::Publication{p.serial,p.epoch,
                    native_upscaler_stereo_d82::CurrentEpoch(),p.technology};
            },
            [&](const char* line) noexcept {diagnostic_trace::Write(line);});
        // V66.18 production entry: no F11/F9 ceremony.  Arm continuous stereo on
        // the first real immersive gameplay handoff, and re-arm automatically after
        // a deliberate theater/menu interval or an unexpected stage-4 stop.  Theater
        // keeps the existing mono/flat presentation owner; we do not start new stereo
        // work while it owns presentation.
        // V66.19: retry backoff. A failed start used to be retried EVERY frame, and each
        // attempt flipped XR stereo mode 2->0 and reset/re-armed the whole capture census
        // (with an unthrottled log line each way). Now: at most one attempt per 750 ms.
        if(v6618_auto_stereo_enabled && !matrix_active && !theater_presented && xr_manager_ &&
           !upscaler_live_switch_d85::Instance().HoldsStereo() && // D85 UPSCALERS
           f9_postfx_step==0 && !native_stereo_outer_lab::SingleRunBusy() &&
           now_ms>=v6619_next_autostart_ms &&
           native_upscaler_stereo_d82::CanEnterStereo(SafeCVarGetInt(nativeUpscalerCvarD82.load(std::memory_order_acquire),-1))) {
            v6619_next_autostart_ms=now_ms+750ull;
            xr_manager_->SetStereoExperiment(2,0.0f);
            stereo_design_capture::SetOutputCensus(true);
            if(native_stereo_outer_lab::StartStereoDesign(4,current_runtime_ipd())) {
                f9_postfx_step=4;
                // V66.24: production native stereo is the authoritative menu-visible state.
                // Without this, FillMenuState reported mono and any VR Settings edit
                // routed ApplyMenuState back through the retired mono experiment.
                vr_production_stereo_mode_.store(3,std::memory_order_release);
                v6619_xr_stereo_applied=2; // initial arm: mode 2 falls back to mono until first certified pair
                v6619_theater_capture_paused=false;
                v6619_waiting_fresh_pair=false;
                vr_hud_reapply_requested_.store(true,std::memory_order_release);
                vr_corrected_hud_dirty_.store(true,std::memory_order_release);
                // V66.68 GOLDEN: no generic post-FX snapshot/rewrite on Stereo entry.
                // KCD2 owns AA/cloud/fog CVars. Native AA uses the existing
                // per-eye histories; DLSS/FSR use their separate vendor instances.
                if(v6640_light_shadow_snapshot_ready && v6640_light_shadow_step!=0) { char lm[16]{}; (void)ApplyV6640LightShadowStep(v6640_light_shadow_original,v6640_light_shadow_step,lm,sizeof(lm)); }
                if(v6640_ao_snapshot_ready && v6640_ao_step!=0) { char am[16]{}; (void)ApplyV6640AoStep(v6640_ao_original,v6640_ao_step,am,sizeof(am)); }
                DeviceHook::SetV6638PairPoseMode(v6638_pair_pose_step);
                v6623_last_render_policy_ms=now_ms;
                // V66.68: expensive native-stage census stays OFF in production.
                char m[640]{}; std::snprintf(m,sizeof(m),
                    "V66.19R1-AUTOSTART ARMED scene=IMMERSIVE ipd=%.6f XR=MONO-UNTIL-FIRST-CERTIFIED-PAIR then=LIVE-NATIVE-LR HUD=REAPPLY-REQUESTED publication=PER-CONSUMED-PAIR",
                    double(current_runtime_ipd())); diagnostic_trace::Write(m); KCD2VR_DIAG_LOG(Bug,warning,"{}",m);
                V6652LogAuthority("stereo-start");
            } else {
                stereo_design_capture::SetOutputCensus(false);
                xr_manager_->SetStereoExperiment(0,0.0f);
                static std::uint64_t last_auto_refuse_log=0;
                if(now_ms-last_auto_refuse_log>=1000ull){ last_auto_refuse_log=now_ms; diagnostic_trace::Write("V66.19R1-AUTOSTART WAIT reason=native-stage-not-ready-yet retry=IMMERSIVE-NEXT-TICK"); }
            }
        }

        // V66.29: per-eye work summary every 5 s while native stereo runs.
        // modeL/modeR = native shader-mode resolutions per eye (V66.26 found RIGHT=0: the
        // particle render element only prepares for LEFT). If F2 stage 2 helps, modeR > 0.
        if(f9_postfx_step==4) {
            static std::uint64_t v6628_next_ms=0;
            static native_stereo_outer_lab::V6628EyeWork v6628_prev{};
            if(now_ms>=v6628_next_ms) {
                native_stereo_outer_lab::V6628EyeWork w{}; native_stereo_outer_lab::GetV6628EyeWork(w);
                if(v6628_next_ms!=0) {
                    char em[400]{}; std::snprintf(em,sizeof(em),
                        "V66.31-EYE-WORK 5s headFollow=%s particleModeL=%llu particleModeR=%llu shadowGateL=%llu shadowGateR=%llu retiredPrepCalls=%llu retiredPrepSkips=%llu",
                        v6627_f2_stage==1?"CORRECTED":"BASELINE",
                        (unsigned long long)(w.modeLeft-v6628_prev.modeLeft),(unsigned long long)(w.modeRight-v6628_prev.modeRight),
                        (unsigned long long)(w.gateLeft-v6628_prev.gateLeft),(unsigned long long)(w.gateRight-v6628_prev.gateRight),
                        (unsigned long long)(w.prepCalls-v6628_prev.prepCalls),(unsigned long long)(w.prepSkips-v6628_prev.prepSkips));
                    diagnostic_trace::Write(em);
                }
                v6628_prev=w; v6628_next_ms=now_ms+5000ull;
            }
        }

        // V66.32: restore the old/native flat theater presentation for large KCD2
        // menus WITHOUT restoring V66.22's broken producer teardown/rearm lifecycle.
        // Stage4/capture stay alive. While the flat menu owns XR, completed native
        // L/R pairs are consumed without presentation so published!=consumed cannot
        // wedge. On gameplay return XR switches directly back to native L/R.
        native_stereo_running_r89_.store(f9_postfx_step==4,std::memory_order_release); // R89 dice: read by ApplyMenuState
        if(f9_postfx_step==4 && xr_manager_ && !matrix_active) {
            // V66.68: no 2-second CVar/post-FX watchdog and no production native-stage census.
            // Those broad owners repeatedly regressed sky state and performance.
            v6623_last_render_policy_ms=now_ms;

            if(theater_presented) {
                // A reopened native menu must drain even if XR is already flat.
                // During exit, drain any older pending pair so the producer can
                // progress beyond the baseline instead of waiting on that pair.
                xr_manager_->SetTheaterDrain(theater_exit113::drain(theater_expected,v6619_waiting_fresh_pair));
                // Use KCD2's completed flat Present on the fixed LOCAL theater plane.
                if(v6619_xr_stereo_applied!=0) {
                    xr_manager_->SetStereoExperiment(0,0.0f);
                    v6619_xr_stereo_applied=0;
                    diagnostic_trace::Write("V66.33-NATIVE-THEATER ENTER XR=FLAT-FINAL-OUTPUT stage4=RUNNING capture=LIVE producer=KEEP-ALIVE drain=ON(render-thread)");
                }
                v6619_theater_capture_paused=false;
            } else {
                // V66.33: the producer never stopped, so the newest published pair is a
                // live render of the current frame; XR can present it directly.
                if(v6619_xr_stereo_applied!=2) {
                    xr_manager_->SetStereoExperiment(2,0.0f);
                    xr_manager_->SetTheaterDrain(false);
                    v6619_xr_stereo_applied=2;
                    diagnostic_trace::Write("V66.113-NATIVE-THEATER EXIT XR=LIVE-NATIVE-LR stage4=RUNNING capture=LIVE drain=OFF presentation=UNVERIFIED");
                }
                v6619_theater_capture_paused=false;
                v6619_waiting_fresh_pair=false;
            }
        } else if(f9_postfx_step!=4) {
            if(xr_manager_) xr_manager_->SetTheaterDrain(false);
            // R89 dice: no stereo run -> XR must not stay in pair mode 2. Mode 2
            // without a producer shows the last old pair forever (D85R5 KCD2VR1
            // 23:53:07-23:54 frozen dice image after a VR-menu zoom edit). Labs
            // (single run / matrix) keep their own modes.
            if(xr_manager_ && !matrix_active && !native_stereo_outer_lab::SingleRunBusy() &&
               xr_manager_->StereoExperimentR89()==2) {
                xr_manager_->SetStereoExperiment(0,0.0f);
                diagnostic_trace::Write("R89 STEREO_IDLE_RESET xr=MODE2_WITHOUT_RUN->MONO_LIVE reason=no-stereo-run");
            }
            v6619_xr_stereo_applied=-1;
            v6619_theater_capture_paused=false;
            v6619_waiting_fresh_pair=false;
        }

        // V65.20 historical recovery: restore the V65.04/V65.06 AUTO-HUD owner exactly in spirit:
        // one shared tangent-derived global UI transform, event-driven only, with no transactional
        // centered-HUD fallback fighting ownership. Toggle/settings/reapply/UI-generation are the only
        // retry events. A failed apply waits for the next real event.
        // V66.22 default vertical centering. The Quest/VDXR stereo frustum is
        // vertically asymmetric; use the same angular residual as CENTER PROJECTION
        // automatically once valid stereo views exist. This is a camera-ray rotation,
        // not a planar UV shift, and it never changes/tears down native stereo mode.
        if(f9_postfx_step==4 && !theater_presented && xr_manager_ && !v6622_default_center_applied) {
            XrManager::StereoViewSnapshot sv{};
            if(xr_manager_->CopyLastGoodStereoViews(sv) && sv.valid && sv.view_count>=2) {
                const float top=std::max(sv.eye_fov[0].angleUp,sv.eye_fov[1].angleUp);
                const float bottom=std::min(sv.eye_fov[0].angleDown,sv.eye_fov[1].angleDown);
                const float left=std::min(sv.eye_fov[0].angleLeft,sv.eye_fov[1].angleLeft);
                const float right=std::max(sv.eye_fov[0].angleRight,sv.eye_fov[1].angleRight);
                const float residual_deg=std::clamp(-0.5f*(top+bottom)*57.29577951308232f,-15.0f,15.0f);
                const float planarX=std::clamp(-(std::tan(left)+std::tan(right))*0.25f,-0.20f,0.20f);
                const float planarY=std::clamp( (std::tan(bottom)+std::tan(top))*0.25f,-0.20f,0.20f);
                vr_pitch_correction_deg_.store(residual_deg,std::memory_order_release);
                vr_auto_projection_center_.store(false,std::memory_order_release);
                vr_presentation_center_x_.store(planarX,std::memory_order_release);
                vr_presentation_center_y_.store(planarY,std::memory_order_release);
                xr_manager_->SetAutoProjectionCenter(false);
                xr_manager_->SetPresentationCenterX(planarX);
                xr_manager_->SetPresentationCenterY(planarY);
                xr_manager_->SetPitchCorrectionDeg(residual_deg);
                v6622_default_center_applied=true;
                char cm[448]{}; std::snprintf(cm,sizeof(cm),
                    "V66.85-STEREO-CENTER default=ANGULAR+PLANAR residualPitch=%+.3fdeg planar=(%+.4f,%+.4f) xrUnion={L:%+.3f R:%+.3f D:%+.3f U:%+.3f} stereoMode=UNCHANGED",
                    double(residual_deg),double(planarX),double(planarY),double(left*57.29577951308232f),double(right*57.29577951308232f),double(bottom*57.29577951308232f),double(top*57.29577951308232f));
                diagnostic_trace::Write(cm); debug_trace::Write(cm);
            }
        }

        static std::uint64_t ui88_hud_epoch=0,ui88_hud_native_generation=~0ull;
        ui_surface_geometry::Hud ui88_hud_geometry{};std::uint64_t measured_hud_epoch=0;
        const auto measured_native_generation=flashui_probe::HudGeneration();
        static bool previousWhole44c2=false;
        const bool wholeWanted44c2=whole_readable_wanted44c2();
        if(previousWhole44c2!=wholeWanted44c2){previousWhole44c2=wholeWanted44c2;vr_hud_reapply_requested_.store(true);}
        // Dynamic child identity does not dirty parent geometry. The child controller
        // below has its own epoch, per-element cache and three-attempt budget.
        if((xr_manager_->GetHudSurfaceGeometry(ui88_hud_geometry,measured_hud_epoch)&&measured_hud_epoch!=ui88_hud_epoch)||measured_native_generation!=ui88_hud_native_generation){
            ui88_hud_epoch=measured_hud_epoch;ui88_hud_native_generation=measured_native_generation;
            vr_hud_reapply_requested_.store(true,std::memory_order_release);
        }
        const bool auto_correct_hud_enabled=vr_auto_correct_hud_.load(std::memory_order_acquire);
        static bool canvas_prev_dialogue=false;
        if(canvas_prev_dialogue!=dialogue_active){canvas_prev_dialogue=dialogue_active;vr_hud_reapply_requested_.store(true);}
        const bool hudReapplyEpoch23=vr_hud_reapply_requested_.load(std::memory_order_acquire),hudDirty23=vr_corrected_hud_dirty_.load(std::memory_order_acquire); // V66.178R23: trigger attribution
        const bool hud_reapply=vr_hud_reapply_requested_.exchange(false,std::memory_order_acq_rel) |
            vr_corrected_hud_dirty_.exchange(false,std::memory_order_acq_rel);
        // V66.162R1: measured XR surface -> ONE readable-parent coordinator.
        // No MasterSwitch/world-FOV writer and no inverse fullscreen-effect writes.
        // Auto-centered/native-anchor/legacy are explicit mutually exclusive modes.
        // Auto-centered uses the measured readable parents; no root fallback.
        const bool manualBaselineWanted36 = f4_vrhud_safe_desired && !theater_presented;
        const bool centered_safe_should_be_on = manualBaselineWanted36||(wholeWanted44c2&&!theater_presented); // one selected readable layout owner
        static bool ui88_hud_wanted_prev=false;
        static unsigned ui88_hud_attempts=0;
        static bool ui88_hud_pending=false;
        if(hud_reapply||centered_safe_should_be_on!=ui88_hud_wanted_prev){ui88_hud_attempts=0;f4_vrhud_last_apply_attempt_ms=0;ui88_hud_pending=true;}
        ui88_hud_wanted_prev=centered_safe_should_be_on;
        const bool ui88_hud_work=ui88_hud_pending||f4_vrhud_safe_active!=centered_safe_should_be_on;
        static std::uint64_t hudLastApplyOk23=0;static bool hudLastWant23=false; // V66.178R25: a 300 ms Flash transaction at most every 1.5 s unless the wanted state flips
        // V66.178R27: an apply with the SAME inputs as the last successful one (geometry epoch, HUD generation,
        // dialogue, all layout parameters) is a no-op by construction - skip it entirely (CORRECTED_DIRTY re-armed
        // applies with identical values several times per minute).
        static std::uint64_t hudFingerprint27=0;
        std::uint64_t fp27=1469598103934665603ull;auto mix27=[&](double v){std::uint64_t b=0;std::memcpy(&b,&v,sizeof(b));fp27^=b;fp27*=1099511628211ull;};
        mix27(double(wholeWanted44c2));mix27(double(true));mix27(double(centered_safe_should_be_on));mix27(double(ui88_hud_epoch));mix27(double(ui88_hud_native_generation));mix27(double(dialogue_active));mix27(double(vr_auto_correct_hud_.load()));mix27(double(vr_hud_native_canvas_fit_.load()));mix27(double(enable_vr_ui_layout_.load()));
        // V66.178R30 (R28 review F09): EVERY consumed placement setting is part of the change key (R27 left out safe_height,
        // center_offset_y and the per-element X/Y/inset values, so a real vertical slider change could be skipped as a no-op).
        mix27(double(vr_ui_bottom_boundary_.load()));mix27(double(vr_ui_center_offset_x_.load()));mix27(double(vr_ui_center_offset_y_.load()));mix27(double(vr_ui_compactness_.load()));mix27(double(auto_hud_icon_scale_.load()));mix27(double(vr_ui_compass_scale_.load()));mix27(double(vr_ui_compass_y_.load()));mix27(double(vr_ui_control_prompt_scale_.load()));mix27(double(vr_ui_control_prompt_x_.load()));mix27(double(vr_ui_control_prompt_y_.load()));mix27(double(vr_ui_dialogue_inset_pct_.load()));mix27(double(vr_ui_dialogue_scale_.load()));mix27(double(vr_ui_dialogue_y_.load()));mix27(double(vr_ui_hints_inset_pct_.load()));mix27(double(vr_ui_hints_scale_.load()));mix27(double(vr_ui_hints_y_.load()));mix27(double(vr_ui_interaction_menu_scale_.load()));mix27(double(vr_ui_interaction_menu_x_.load()));mix27(double(vr_ui_interaction_menu_y_.load()));mix27(double(vr_ui_item_panel_scale_.load()));mix27(double(vr_ui_item_panel_x_.load()));mix27(double(vr_ui_item_panel_y_.load()));mix27(double(vr_ui_lower_scale_.load()));mix27(double(vr_ui_lower_spread_.load()));mix27(double(vr_ui_lower_y_.load()));mix27(double(vr_ui_safe_height_.load()));mix27(double(vr_ui_safe_width_.load()));mix27(double(vr_ui_stats_scale_.load()));mix27(double(vr_ui_stats_x_.load()));mix27(double(vr_ui_stats_y_.load()));mix27(double(vr_ui_subtitle_scale_.load()));mix27(double(vr_ui_subtitle_y_.load()));mix27(double(vr_ui_top_boundary_.load()));mix27(double(vr_hud_native_canvas_size_.load()));mix27(double(vr_hud_native_canvas_spread_x_.load()));mix27(double(vr_hud_native_canvas_spread_y_.load()));
        const bool hudSameInputs27=hudFingerprint27==fp27&&f4_vrhud_safe_active==centered_safe_should_be_on;
        if(hudSameInputs27&&ui88_hud_pending){ui88_hud_pending=false;ui88_hud_attempts=0;} // identical request: nothing to apply
        const bool hudDebounced25=(hudLastApplyOk23&&hudLastWant23==centered_safe_should_be_on&&now_ms-hudLastApplyOk23<300)||hudSameInputs27; // V66.178R30: with the complete change key the time debounce only coalesces slider drags (300 ms, was 1.5 s)
        if(!hudDebounced25&&!matrix_active&&!native_ui_snapshot.Open(ui_native_state::Loading)&&!native_ui_snapshot.Open(ui_native_state::Progress)&&ui88_hud_work&&ui88_hud_attempts<3&&
           (!f4_vrhud_last_apply_attempt_ms||now_ms-f4_vrhud_last_apply_attempt_ms>=200)){
            ++ui88_hud_attempts;f4_vrhud_last_apply_attempt_ms=now_ms;
            bool appliedHud22=false;const auto hudT0_23=std::chrono::steady_clock::now();{diagnostics::perf_d61::Scope perfHudD61{diagnostics::perf_d61::HudApply};kcd2vr::hookcost178r22::Scope hcApply22{kcd2vr::hookcost178r22::HudApply};appliedHud22=apply_f4_vrhud_safe(centered_safe_should_be_on);} // V66.178R22c
            {double rMs=0,gMs=0,sMs=0;std::uint64_t hits=0,misses=0;flashui_probe::CanvasPhaseCost178r27(rMs,gMs,sMs,hits,misses);
             char hm[360]{};std::snprintf(hm,sizeof(hm),"V66.178R23 HUD_APPLY want=%d parentOk=%d attempt=%u ms=%.1f resolveMs=%.1f(hits=%llu misses=%llu) getMs=%.1f setMs=%.1f trigger=%s%s%s phaseScope=CAPTURED_SINCE_LAST_DRAIN caller=MOD_XR_WORKER",centered_safe_should_be_on?1:0,appliedHud22?1:0,ui88_hud_attempts,double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-hudT0_23).count())/1000.0,rMs,(unsigned long long)hits,(unsigned long long)misses,gMs,sMs,hudReapplyEpoch23?"SURFACE_EPOCH_OR_GENERATION ":"",hudDirty23?"CORRECTED_DIRTY ":"",(!hudReapplyEpoch23&&!hudDirty23)?"OWNER_TOGGLE_OR_RETRY ":"");diagnostic_trace::Write(hm);}
            if(appliedHud22){hudLastApplyOk23=now_ms;hudLastWant23=centered_safe_should_be_on;hudFingerprint27=fp27;
                f4_vrhud_safe_active=centered_safe_should_be_on;ui88_hud_pending=false;
                diagnostic_trace::Write(!centered_safe_should_be_on?"V66.90-HUD native geometry restored for complete menu":
                    (vr_auto_correct_hud_.load()?"V66.162R1-HUD OWNER=READABLE_PARENTS XR_SURFACE upper/lower=55..300% size=25..120% center=FIXED effects=NATIVE":
                    (vr_hud_native_canvas_fit_.load()?"V66.90-HUD native-canvas parents fitted; field effects unchanged":"V66.90-HUD legacy semantic mode active")));
            }else if(ui88_hud_attempts==3){
                const auto cr=flashui_probe::NativeCanvasReport();char message[256]{};
                std::snprintf(message,sizeof(message),"V66.90-HUD deferred after 3 event attempts; status=%d resolved=0x%03X failed=0x%03X; latched-until-layout-setting-or-native-lifetime-change",int(cr.status),cr.resolved,cr.failed);
                diagnostic_trace::Write(message);
            }
        }
        if(centered_safe_should_be_on&&hud_layout_owner==HudLayoutOwner::WholeReadable&&
           !matrix_active&&!native_ui_snapshot.Open(ui_native_state::Loading)&&!native_ui_snapshot.Open(ui_native_state::Progress))
            (void)flashui_probe::ApplyUniformHudElements171(vr_ui_compass_scale_.load(),vr_ui_compass_y_.load(),
                vr_ui_stats_scale_.load(),vr_ui_lower_scale_.load(),vr_ui_lower_y_.load(),vr_ui_lower_spread_.load(),vr_ui_stats_x_.load(),vr_ui_stats_y_.load());
        // D74: prompts were previously wired only to the unreachable UniformReadable
        // branch. Existing station lifecycle evidence selects local 60%; no head or
        // hand target is changed, and leaving the station restores saved user sizes.
        flashui_probe::UpdateReadablePromptsD74(centered_safe_should_be_on&&(hud_layout_owner==HudLayoutOwner::WholeReadable||hud_layout_owner==HudLayoutOwner::UniformReadable)&&
            !matrix_active&&!native_ui_snapshot.Open(ui_native_state::Loading)&&!native_ui_snapshot.Open(ui_native_state::Progress),
            false,vr_ui_hints_scale_.load(),vr_ui_control_prompt_scale_.load(),now_ms); // station children have one movie-local owner below
        // R112: the big directional combat crosshair gets its own size (VR HUD > ELEMENT CONTROLS). Same owner window as
        // the prompts above; outside it (menus, loading, Theater) the native size is given back.
        flashui_probe::UpdateCombatCrosshairR112(centered_safe_should_be_on&&(hud_layout_owner==HudLayoutOwner::WholeReadable||hud_layout_owner==HudLayoutOwner::UniformReadable)&&
            !matrix_active&&!native_ui_snapshot.Open(ui_native_state::Loading)&&!native_ui_snapshot.Open(ui_native_state::Progress),now_ms);
        // R125: the combat direction cross art hidden (VrCombatCrossR125=false, default) in every HUD mode (immersive palm HUD
        // or the normal one), lock + perfect block / master strike icons stay native. Re-asserted every 0.5 s, no latch.
        flashui_probe::UpdateCombatCrossArtR125(!kcd2vr::ui_hud_r112::combatCrossR125.load(std::memory_order_acquire),now_ms);
        g_stallCp178r30[1]=std::uint64_t(GetTickCount64()); // R30 checkpoint: HUD apply done
        // V66.162R1: native visibility/animations own the ordinary full HUD.
        // Contextual visibility changes only at palm/settings/lifecycle transitions.
        // V63.4 forced-1P dialogue lens guard. Both controls live entirely in compositor/source mapping space.
        // Never writes cl_fov and never mutates the KCD2 camera. This makes zoom-vs-FOV diagnosis independent.
        static float last_dialogue_zoom_mult=-1.0f;
        static float last_dialogue_guard_fov=-1.0f;
        static bool last_dialogue_guard_active=false;
        static bool dialogue_fov_session_active=false;
        static bool dialogue_fov_override_prev=false;
        static bool dialogue_fov_latched=false;
        static float dialogue_fov_latched_value=0.0f;
        static std::uint64_t dialogue_fov_latch_after_sample=0;
        const bool dialogue_force_preview=vr_dialogue_force_preview_.load(std::memory_order_acquire);
        const bool dialogue_guard_active=((dialogue_active && dialogue_mode_now>=1) || dialogue_force_preview);
        const bool dialogue_zoom_override=vr_dialogue_zoom_override_.load(std::memory_order_acquire);
        const bool dialogue_fov_override=vr_dialogue_fov_override_.load(std::memory_order_acquire);

        if(dialogue_guard_active!=dialogue_fov_session_active){
            dialogue_fov_session_active=dialogue_guard_active;
            dialogue_fov_latched=false;
            dialogue_fov_latch_after_sample=published_fov_samples;
            if(!dialogue_guard_active) dialogue_fov_latched_value=0.0f;
        }
        if(dialogue_guard_active && dialogue_fov_override && !dialogue_fov_override_prev){
            dialogue_fov_latched=false;
            dialogue_fov_latch_after_sample=published_fov_samples;
        }
        if(dialogue_guard_active && dialogue_fov_override && !dialogue_fov_latched &&
           have_published_fov && published_fov_samples>dialogue_fov_latch_after_sample){
            dialogue_fov_latched_value=std::clamp(published_fov_deg,70.0f,140.0f);
            dialogue_fov_latched=true;
            char fl[224]{};
            std::snprintf(fl,sizeof(fl),"V64.0-DIALOGUE-FOV captured=%.2f sample=%llu source=fresh-published hold-until-exit=true",
                double(dialogue_fov_latched_value),(unsigned long long)published_fov_samples);
            diagnostic_trace::Write(fl);
        }
        dialogue_fov_override_prev=dialogue_fov_override;

        // C19C2: requested immersive policy owns full-size framing, even on a
        // frame without an applied-eye receipt. Legacy mode1 keeps its explicit
        // compensation. A transient receipt must never halve the picture.
        const float dialogue_zoom_mult=dialogue_presentation19c2::zoom(dialogue_guard_active,dialogue_zoom_override,
            dialogue_mode_now,vr_dialogue_zoom_compensation_.load(std::memory_order_acquire));
        const float dialogue_guard_fov=(dialogue_guard_active && dialogue_fov_override)
            ? (dialogue_fov_latched ? dialogue_fov_latched_value : std::clamp(vr_dialogue_fov_deg_.load(std::memory_order_acquire),70.0f,140.0f))
            : std::clamp(vr_source_vertical_fov_deg_.load(std::memory_order_acquire),70.0f,140.0f);
        if(xr_manager_ && (std::fabs(dialogue_zoom_mult-last_dialogue_zoom_mult)>0.0001f ||
                           std::fabs(dialogue_guard_fov-last_dialogue_guard_fov)>0.01f ||
                           dialogue_guard_active!=last_dialogue_guard_active)) {
            xr_manager_->SetDialogueZoomMultiplier(dialogue_zoom_mult);
            xr_manager_->SetSourceVerticalFov(dialogue_guard_fov);
            last_dialogue_zoom_mult=dialogue_zoom_mult;
            last_dialogue_guard_fov=dialogue_guard_fov;
            last_dialogue_guard_active=dialogue_guard_active;
            const char* fsrc=!(dialogue_guard_active&&dialogue_fov_override)?"normal":(dialogue_fov_latched?"latched":"manual-fallback");
            char dz[320]{}; std::snprintf(dz,sizeof(dz),"V64.0-DIALOGUE-GUARD active=%d detected=%d forced=%d zoomOverride=%d zoom=%.2f fovOverride=%d fov=%.1f fovSource=%s normalFov=%.1f compositorOnly=true cl_fovWrite=false",
                dialogue_guard_active?1:0,dialogue_active?1:0,dialogue_force_preview?1:0,dialogue_zoom_override?1:0,double(dialogue_zoom_mult),dialogue_fov_override?1:0,double(dialogue_guard_fov),fsrc,double(vr_source_vertical_fov_deg_.load(std::memory_order_acquire))); diagnostic_trace::Write(dz);
        }

        // THEATERHUD: no delayed inventory-specific safe-zone switch. Presentation state
        // above changes the layout immediately. If FlashUI recreates its HUD object, this
        // bounded event-driven repair reapplies the layout appropriate to the CURRENT mode.
        if(hud_reapply_attempts>0 && now_ms>=hud_reapply_next_ms) {
            // V65.23: consume legacy repair requests without activating SAFE/CENTERED HUD.
            // Whole-root AUTO-HUD retries are driven by HUD generation / explicit reapply above.
            hud_reapply_attempts=0;
            hud_reapply_next_ms=0;
        }
        // V63.0: NO periodic HUD keepalive. The 1 Hz reacquire/reapply path matched the
        // headset-visible one-second hitch. Persistence is event-driven only.

        const bool ui_auto_immersive = vr_ui_auto_layout_.load(std::memory_order_acquire);
        const bool effective_flat_ui_pre = theater_presented;
        // V63.0 native persistence: reapply only when the HUD object changes or a
        // major UI/dialogue presentation context transitions. No time-based keepalive.
        const std::uint64_t hud_generation=flashui_probe::HudGeneration();
        const bool hud_recreated=(hud_generation!=last_hud_generation);
        const bool hud_context_changed=(dialogue_active!=last_dialogue_for_hud) || (effective_flat_ui_pre!=last_effective_flat_for_hud);
        if((hud_recreated || hud_context_changed) && hud_reapply_attempts==0) {
            last_hud_generation=hud_generation;
            last_dialogue_for_hud=dialogue_active;
            last_effective_flat_for_hud=effective_flat_ui_pre;
            hud_reapply_attempts=1;
            hud_reapply_next_ms=now_ms+80ull;
            (void)hud_recreated; (void)hud_context_changed;
        }
        // V61.43 PERF: coverage trig/mutex work is no longer done every XR
        // frame. It is refreshed inside the 200 ms auto-projection block below.
        float required_crop_vfov_deg = vr_required_source_vfov_deg_.load(std::memory_order_relaxed);

        // V61.45: transient dialogue/inventory zoom writer retired.
        // Keep gameplay cl_fov and the compositor source-FOV model unchanged here.
        // The V60.1 UI/DIALOG FOV GUARD below intentionally ignores transient
        // dialogue/inventory camera publications for VR source mapping.

        // V61.38: depth is live only while the DEPTH PSEUDO mode is selected.
        // Leaving that button disables candidate scoring and shader depth use
        // immediately; the installed descriptor hooks remain cheap pass-throughs.
        // V65.17: projective FOV REMAP (mode 1) owns a cached live-game-FOV-synchronized
        // symmetric-source tangent remap and must NOT run the historical 200 ms
        // optical-center updater. Keep the V65.01 auto-projection behavior only
        // for the pseudo modes (2/3), where it was part of their working path.
        const int live_projection_mode=std::clamp(vr_cyclopean_mode_.load(std::memory_order_acquire),0,3);
        f8_auto_projection_active = live_projection_mode >= 2;
        vr_auto_projection_active_.store(f8_auto_projection_active, std::memory_order_release);
        if (xr_manager_) {
            const int clean_mode = std::clamp(vr_cyclopean_mode_.load(std::memory_order_acquire), 0, 3);
            const bool depth_mode = clean_mode == 3;
            // V64.35 hybrid: keep V64.22C direct strength and do not couple pseudo to KCD2 DOF.
            xr_manager_->SetDepthPushWeight(vr_depth_stereo_strength_.load(std::memory_order_acquire));
            xr_manager_->SetDepthPushEnabled(depth_mode);
            if (DeviceHook::GenericDepthProbeEnabled() != depth_mode) DeviceHook::SetGenericDepthProbe(depth_mode);
        }
        if (!f8_auto_projection_active && xr_manager_) {
            const int center_mode=std::clamp(vr_cyclopean_mode_.load(std::memory_order_acquire),0,3);
            if(center_mode==0) {
                vr_presentation_center_x_.store(0.0f, std::memory_order_release);
                vr_presentation_center_y_.store(0.0f, std::memory_order_release);
                xr_manager_->SetPresentationCenterX(0.0f);
                xr_manager_->SetPresentationCenterY(0.0f);
            } else if(center_mode==1) {
                vr_presentation_center_x_.store(0.0f, std::memory_order_release);
                xr_manager_->SetPresentationCenterX(0.0f);
                xr_manager_->SetPresentationCenterY(std::clamp(vr_presentation_center_y_.load(std::memory_order_acquire),-0.20f,0.20f));
            }
        }
        if(!matrix_active && f8_auto_projection_active && xr_manager_ && !effective_flat_ui_pre
           && (now_ms - f8_last_adjust_ms >= 200ull)) {
            // V61.40/V61.43: this whole block - eye-FOV trig/coverage math plus
            // presentation setter calls - was running
            // unconditionally every single main-loop tick for any mode other than
            // OLD MONO, unthrottled, even though its inputs (game FOV, source
            // mapper FOV, trims, zoom) only actually change at menu-slider speed.
            // The status log right below was already throttled to 1/sec; this
            // reuses the same pattern (f8_last_adjust_ms, declared but never wired
            // to anything) at 200ms - still feels instant on a slider drag, but
            // cuts the real per-tick cost this introduced versus OLD MONO by ~18x
            // at a typical 90Hz tick rate. No visual/behavioral change intended.
            f8_last_adjust_ms=now_ms;
            const float actual=CameraHook::LatestGameplayFovDegrees();
            const auto samples=CameraHook::GameplayFovSampleCount();
            if(samples>0 && actual>30.0f && actual<150.0f){
                // V61.34 calibration: game FOV and compositor/view FOV are deliberately
                // independent again. The old menu exposes both. Automatic projection derives the
                // headset optical-center correction from the *selected view mapper FOV*; F8 is now stereo archaeology.
                const float mapperFov=std::clamp(vr_source_vertical_fov_deg_.load(std::memory_order_acquire),70.0f,140.0f);
                float autoCenterX=0.0f,autoCenterY=0.0f,requiredCentered=0.0f,requiredNoShift=0.0f;
                if(compute_auto_projection(mapperFov,autoCenterX,autoCenterY,requiredCentered,requiredNoShift)){
                    required_crop_vfov_deg=requiredNoShift;
                    vr_required_source_vfov_deg_.store(requiredNoShift,std::memory_order_release);
                    const float fovTrim=vr_auto_fov_trim_deg_.load(std::memory_order_acquire);
                    const float centerXTrim=vr_auto_center_x_trim_.load(std::memory_order_acquire);
                    const float centerYTrim=vr_auto_center_y_trim_.load(std::memory_order_acquire);
                    const float target=std::clamp(requiredCentered + 1.0f + fovTrim,70.0f,145.0f); // diagnostic suggestion only
                    const int activeCyclo=std::clamp(vr_cyclopean_mode_.load(std::memory_order_acquire),0,3);
                    // V65.02a corrected mono derives the binocular optical center
                    // directly from the OpenXR tangent bounds.  Keep only the
                    // user's explicit center trims as UV residuals so we do not
                    // apply the asymmetry correction twice.
                    const float effectiveCenterX=std::clamp((activeCyclo==1 ? 0.0f : autoCenterX) + centerXTrim,-0.20f,0.20f);
                    const float effectiveCenterY=std::clamp((activeCyclo==1 ? 0.0f : autoCenterY) + centerYTrim,-0.20f,0.20f);
                    vr_presentation_center_x_.store(effectiveCenterX,std::memory_order_release);
                    vr_presentation_center_y_.store(effectiveCenterY,std::memory_order_release);
                    xr_manager_->SetPresentationCenterX(effectiveCenterX);
                    xr_manager_->SetPresentationCenterY(effectiveCenterY);
                    const V603ViewPreset activePreset=ViewPresetForMode(activeCyclo);
                    const float compositorFov=(activeCyclo==1)
                        ? std::clamp(ReadCurrentGameFovValue()>1.0f?ReadCurrentGameFovValue():menu_game_fov_target_.load(std::memory_order_relaxed),70.0f,140.0f)
                        : mapperFov;
                    if(activeCyclo==1) xr_manager_->SetCorrectedViewVerticalFov(mapperFov);
                    xr_manager_->SetPresentationSettings(
                        vr_render_scale_.load(std::memory_order_relaxed), activePreset.presentation_mode,
                        activeCyclo==1?1.0f:vr_zoom_.load(std::memory_order_relaxed), 0.0f,0.0f,0.0f,0.0f, compositorFov);
                    xr_manager_->SetAngularCorrectMono(activePreset.angular_correct);
                    xr_manager_->SetPoseHonestyOverride(activePreset.pose_honesty);
                    xr_manager_->SetMonoFovAsymmetryScale(activePreset.fov_asymmetry);
                    xr_manager_->SetMonoUvReprojectionMode(MonoMapModeForCleanMode(activeCyclo));
                    xr_manager_->SetMonoUvBinocularOffset(activePreset.uv_binocular);
                    // V61.34 calibration build: do not chase/overwrite cl_fov here.
                    // The old menu Apply button owns game FOV so headset tuning is possible.
                    if(now_ms-f8_last_status_ms>=1000ull){
                        f8_last_status_ms=now_ms;
                        char fm[640]{}; std::snprintf(fm,sizeof(fm),
                            "V64.1-AUTO-PROJECTION STATUS actualGameplayVFOV=%.3f suggestedCoverageVFOV=%.3f requiredCentered=%.3f requiredNoShift=%.3f gameCVar=%.3f sourceMapper=%.3f centerX={auto:%.5f trim:%+.5f effective:%.5f} centerY={auto:%.5f trim:%+.5f effective:%.5f} fovTrim=%+.2f zoom=%.3f samples=%llu coverage=%s",
                            double(actual),double(target),double(requiredCentered),double(required_crop_vfov_deg),double(ReadCurrentGameFovValue()),double(mapperFov),
                            double(autoCenterX),double(centerXTrim),double(effectiveCenterX),double(autoCenterY),double(centerYTrim),double(effectiveCenterY),double(fovTrim),double(vr_zoom_.load(std::memory_order_relaxed)),
                            (unsigned long long)samples,mapperFov+0.15f>=requiredCentered?"YES":"NO"); debug_trace::Write(fm);
                    }
                }
            } else if(now_ms-f8_last_status_ms>=1000ull){
                f8_last_status_ms=now_ms; debug_trace::Write("V64.1-AUTO-PROJECTION WAIT exact gameplay CView FOV sample not available yet");
            }
        }

        // V64.15: legacy MasterSwitch AUTO remains retired. F4 angular-safe mode
        // is local/non-persistent and uses semantic clips plus OpenXR-derived anchors.
        if(vr_ui_auto_layout_.exchange(false,std::memory_order_acq_rel))
            diagnostic_trace::Write("V64.15-F4 legacy MasterSwitch AUTO forced OFF; angular-safe semantic A/B owns F4");
        // V64.12 default interaction assist: visual-center ray is used only by the forgiving candidate cone; native ray direction is never steered.
        (void)interaction_alignment_installed; // V64.31 disabled in camera-focus build

        // MENUFIX/V64.39: legacy startup release via published gameplay FOV is retired.
        // The frontend cursor close transition above owns the Continue/Load handoff.

        // V66.86 F2 recovery: until a named Inventory/Map/Shop texture is isolated,
        // large native menus use KCD2's COMPLETED FINAL FLAT OUTPUT as one fixed
        // Theater/menu layer. F2 owns the gameplay HUD only.
        const bool auto_theater = theater_presented;
        const bool f2_ui_menu_active=false; // future isolated named MENU texture only
        const bool safe_ui_fit=theater_presented;
        const char* presentation_owner=authored_scene_theater&&ui89_policy.owner==CleanMenuOwner::None
            ? (native_dialogue_flat_ui?"DIALOGUE_NATIVE_PASSIVE":"CUTSCENE_PASSIVE") : ui_menu_router::Name(ui89_policy.owner);
        xr_manager_->SetNativeMenuPlaneMode(safe_ui_fit);
        xr_manager_->SetFullMenuFit(!immersive_system_overlay&&(ui89_policy.FitCompleteFrame()||(authored_scene_theater&&ui89_policy.owner==CleanMenuOwner::None)));
        xr_manager_->SetPassiveFullFrameScale(passive_fullscreen_scene
            ? ui_scene_presentation::PassiveScale(vr_passive_full_frame_scale_.load(std::memory_order_acquire)) : 1.0f);
        // R107: cutscene / native dialogue = only the 16:9 band of the frame on the screen (the game's own bars
        // sit at the band edges; outside the band the world was visible). Not for loading screens or menus.
        xr_manager_->SetFlatCropAspectR107(kcd2vr::cinematic_bars_r107::enabled.load(std::memory_order_acquire)&&
            authored_scene_theater&&ui89_policy.owner==CleanMenuOwner::None?kcd2vr::cinematic_bars_r107::kAspect:0.0f);
        // Standalone transfer/selection remains immersive. Its two content parents
        // get their own reversible fit; fullscreen dimmers remain untouched.
        const auto itemMask=ui_native_state::Bit(ui_native_state::ItemSelection)|ui_native_state::Bit(ui_native_state::ItemTransfer);
        const auto itemLive=ui89_policy.supported&itemMask;
        const bool itemFitWanted=itemLive&&!ui89_policy.FullFrame()&&!matrix_active;
        xr_manager_->SetImmersiveMenuPointer(itemFitWanted);
        ui_surface_geometry::Hud itemGeometry{};std::uint64_t itemSurfaceEpoch=0;
        const bool itemSurfaceReady=xr_manager_->GetHudSurfaceGeometry(itemGeometry,itemSurfaceEpoch);
        const auto itemGeneration=flashui_probe::ContextPanelGeneration();
        const float itemSize=vr_hud_native_canvas_size_.load(std::memory_order_acquire);
        static std::uint32_t lastItemMask=~0u;static std::uint64_t lastItemSurface=~0ull,lastItemGeneration=~0ull;
        static float lastItemSize=-1;static bool lastItemFit=false;static unsigned itemAttempts=0;static std::uint64_t itemNext=0;
        if(itemLive!=lastItemMask||itemSurfaceEpoch!=lastItemSurface||itemGeneration!=lastItemGeneration||itemSize!=lastItemSize||itemFitWanted!=lastItemFit){
            lastItemMask=itemLive;lastItemSurface=itemSurfaceEpoch;lastItemGeneration=itemGeneration;lastItemSize=itemSize;lastItemFit=itemFitWanted;itemAttempts=3;itemNext=now_ms;
        }
        if(itemAttempts&&now_ms>=itemNext&&(!itemFitWanted||itemSurfaceReady)){
            --itemAttempts;bool fitOk=true;
            for(unsigned pi=0;pi<2;++pi){const auto bit=ui_native_state::Bit(pi==0?ui_native_state::ItemSelection:ui_native_state::ItemTransfer);
                fitOk=flashui_probe::ApplyContextPanelLayout(pi,itemFitWanted&&(itemLive&bit),itemGeometry,itemSize)&&fitOk;}
            if(fitOk)itemAttempts=0;else itemNext=now_ms+350;
            if(fitOk||!itemAttempts){char im[192]{};std::snprintf(im,sizeof(im),"V66.90R1-ITEM-FIT wanted=%d mask=0x%X result=%s fullFrame=%d",itemFitWanted?1:0,itemLive,fitOk?"GEOMETRY_VERIFIED":"DEFERRED",ui89_policy.FullFrame()?1:0);diagnostic_trace::Write(im);}
        }

        // C63: helmet-only placement on Overlay visibility/lifetime or optical
        // surface changes. At most8attempts; no steady Flash queries.
        { static std::uint64_t sample63=~0ull,surface63=~0ull,next63=0;
          static bool wanted63=false;static unsigned attempts63=0;
          const bool wanted=native_ui_snapshot.Open(ui_native_state::Overlay)&&!safe_ui_fit&&!matrix_active;
          const auto sample=flashui_probe::HelmetGeneration63();
          if(wanted!=wanted63||sample!=sample63||itemSurfaceEpoch!=surface63){
            wanted63=wanted;sample63=sample;surface63=itemSurfaceEpoch;attempts63=8;next63=now_ms;
          }
          if(attempts63&&now_ms>=next63&&itemSurfaceReady){
            --attempts63;next63=now_ms+150;
            const bool ok=flashui_probe::ApplyHelmetOverlay63(wanted,itemGeometry.surface.width);
            if(ok)attempts63=0;
            if(ok||!attempts63){char line63[200]{};std::snprintf(line63,sizeof(line63),
              "H15 HELMET_FIT enabled=%d result=%s relativeX=0.9504 relativeY=0.792 widthIncrease=20percent center=NATIVE target=Overlay.overlay0 writes=XY_SCALE",
              wanted,ok?"READBACK_VERIFIED":"DEFERRED");diagnostic_trace::Write(line63);}
          }
        }

        // B2: independent wait/sleep/read card; only real lifetime/surface edges
        // schedule bounded retries. No steady Flash polling or whole-root fit.
        { static std::uint64_t seen=~0ull,surface=~0ull,next=0;static bool previous=false;static unsigned attempts=0;
          const bool wanted=native_ui_snapshot.Open(ui_native_state::SkipTime)&&!safe_ui_fit&&!matrix_active;
          const auto generation=flashui_probe::WaitGenerationB2();
          if(wanted!=previous||generation!=seen||itemSurfaceEpoch!=surface){previous=wanted;seen=generation;surface=itemSurfaceEpoch;attempts=8;next=now_ms;}
          if(attempts&&now_ms>=next&&(!wanted||itemSurfaceReady)){
            --attempts;next=now_ms+150;const bool ok=flashui_probe::ApplyWaitOverlayB2(wanted,itemGeometry);
            if(ok)attempts=0;
            if(ok||!attempts){char line[192]{};std::snprintf(line,sizeof(line),"B2 WAIT_FIT enabled=%d result=%s size=0.70 target=SkipTime.mc_Dialog background=NATIVE",wanted,ok?"READBACK_VERIFIED":"DEFERRED");diagnostic_trace::Write(line);}
          }
        }

        // S1 base bridge: native minigame parents use the same optical safe fit;
        // input ownership and the immersive world remain native.
        flashui_probe::UpdateActivityOverlaysS1(native_ui_snapshot,!safe_ui_fit&&!matrix_active&&!ui89_policy.FullFrame(),
            itemSurfaceReady,itemGeometry,itemSurfaceEpoch,itemSize,now_ms);

        // C43: fit the separate Yes/No question movie only on immersive surfaces.
        // Three bounded attempts per actual context/surface/settings change.
        const bool modalFit43=(ui89_policy.supported&ui_native_catalogue::ModalMask)&&
            !ui89_policy.FullFrame()&&!matrix_active;
        static bool lastModal43=false;static std::uint64_t modalContext43=0,modalSurface43=0;
        static float modalSize43=-1;static unsigned modalAttempts43=0;static std::uint64_t modalNext43=0;
        const float modalSizeNow43=vr_pause_overlay_size_.load(std::memory_order_relaxed);
        if(modalFit43!=lastModal43||ui89_policy.context!=modalContext43||itemSurfaceEpoch!=modalSurface43||modalSizeNow43!=modalSize43){
            lastModal43=modalFit43;modalContext43=ui89_policy.context;modalSurface43=itemSurfaceEpoch;
            modalSize43=modalSizeNow43;modalAttempts43=3;modalNext43=now_ms;
        }
        if(modalAttempts43&&now_ms>=modalNext43&&(!modalFit43||itemSurfaceReady)){
            --modalAttempts43;const bool ok=flashui_probe::ApplyModalOverlayLayout43(modalFit43,itemGeometry,modalSizeNow43);
            if(ok)modalAttempts43=0;else modalNext43=now_ms+350;
            if(modalFit43&&(ok||!modalAttempts43))diagnostic_trace::Write(ok?
                "C43 MODAL_FIT questionAnchor=VERIFIED parentPause=UNCHANGED steadyCalls=0":
                "C43 MODAL_FIT questionAnchor=UNAVAILABLE parentPause=UNCHANGED steadyCalls=0");
        }

        // C10R2: opening a menu must not enable additional rendering effects.
        // Keep native/user DoF settings; never force r_DepthOfField=2 here.
        const bool blurWanted124=false;
        if(blurWanted124!=pauseBlurWanted124){pauseBlurWanted124=blurWanted124;pauseBlurAttempts124=3;pauseBlurNext124=now_ms;}
        if(pauseBlurAttempts124&&now_ms>=pauseBlurNext124){
            --pauseBlurAttempts124;const bool blurOk124=pauseBlur124.Apply(pauseBlurIo124,blurWanted124);
            if(blurOk124)pauseBlurAttempts124=0;else pauseBlurNext124=now_ms+350;
            if(blurOk124||!pauseBlurAttempts124){char m[192]{};std::snprintf(m,sizeof(m),"V66.124R1-PAUSE-BLUR wanted=%d result=%s renderer=2 nearDof=1 mode=1 restore=OWNED_ORIGINAL",blurWanted124,blurOk124?"VERIFIED":"UNAVAILABLE");diagnostic_trace::Write(m);}
        }
        // V66.97 Pause overlay: keep KCD2's actual paused simulation and native
        // full-field DoF/dimming, but fit only Menu.gfx readable root children.
        // No polling: reapply only on menu context, HUD surface, or setting change.
        ui_surface_geometry::Hud pauseGeometry{};std::uint64_t pauseSurfaceEpoch=0;
        const bool pauseSurfaceReady=xr_manager_->GetHudSurfaceGeometry(pauseGeometry,pauseSurfaceEpoch);
        const float pauseSize=vr_pause_overlay_size_.load(std::memory_order_acquire);
        static bool lastPauseWanted=false;static std::uint64_t lastPauseContext=~0ull,lastPauseSurface=~0ull;static float lastPauseSize=-1.0f;
        static ui_pause_schedule59c10r2::Settling pauseSettle;static float lastPauseHints=-1.f;
        static bool lastPauseExperimental36=false;
        if(lastPauseExperimental36!=true){lastPauseExperimental36=true;lastPauseSize=-1.f;}
        static std::uint64_t lastPauseGeneration=~0ull;const auto pauseGeneration=flashui_probe::PauseGeneration();
        if(pause_overlay_scene!=lastPauseWanted||ui89_policy.context!=lastPauseContext||pauseSurfaceEpoch!=lastPauseSurface||pauseSize!=lastPauseSize||lastPauseHints!=vr_ui_control_prompt_scale_.load()||lastPauseGeneration!=pauseGeneration){
            lastPauseHints=vr_ui_control_prompt_scale_.load();lastPauseGeneration=pauseGeneration;
            lastPauseWanted=pause_overlay_scene;lastPauseContext=ui89_policy.context;lastPauseSurface=pauseSurfaceEpoch;lastPauseSize=pauseSize;pauseSettle.Begin(pause_overlay_scene,now_ms);
        }
        {   // The exact native ShowPage completion is a layout event. Ordinary
            // GetElement("Menu") lookups can run every frame and must not rearm
            // the expensive fit. A new page restarts only its bounded settling.
            static std::uint64_t lastMenuPage59c19=~0ull;
            const auto menuPage=flashui_probe::MenuPageEventSerial59c19();
            if(pause_overlay_scene&&menuPage!=lastMenuPage59c19){
                pauseSettle.Begin(true,now_ms);
                static unsigned refits=0;if(++refits<=5||refits%20==0){char rm[160]{};std::snprintf(rm,sizeof(rm),"C10R19 PAUSE_REFIT pageEvents=%llu refits=%u source=NATIVE_SHOW_PAGE",(unsigned long long)menuPage,refits);diagnostic_trace::Write(rm);}
            }
            lastMenuPage59c19=menuPage;
        }
        // Event / generation transitions only: do not poll or refit every 300 ms.
        if(pauseSettle.Take(now_ms,!pause_overlay_scene||pauseSurfaceReady)){
            // Pause prompts belong to the menu fit. Applying the smaller gameplay
            // HUD hint/prompt multiplier again made its button legend unreadable.
            const auto pauseBegin=std::chrono::steady_clock::now();
            const bool ok=flashui_probe::ApplyPauseOverlayLayout(pause_overlay_scene,pauseGeometry,pauseSize,1.0f);
            pauseSettle.Complete(ok);
            const auto pauseUs=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-pauseBegin).count();
            // Acquiring the menu may establish its lifetime. Do not mistake our
            // own acquisition for another open event and restart settling.
            lastPauseGeneration=flashui_probe::PauseGeneration();
            if(ok||!pauseSettle.Pending()){char pm[320]{};std::snprintf(pm,sizeof(pm),
                "V66.97-PAUSE-FIT wanted=%d result=%s size=%.2f theater=%d effects=NATIVE layoutUs=%lld settlePending=%d writes=EVENT_ONLY",
                pause_overlay_scene?1:0,ok?"READBACK_VERIFIED":"DEFERRED",double(pauseSize),safe_ui_fit?1:0,(long long)pauseUs,pauseSettle.Pending()?1:0);diagnostic_trace::Write(pm);}
        }
        static bool lastDeathWanted=false;static std::uint64_t lastDeathContext=~0ull,lastDeathSurface=~0ull,lastDeathGeneration=~0ull;
        static float lastDeathSize=-1;static unsigned deathAttempts=0;static std::uint64_t deathNext=0;
        const auto deathGeneration=flashui_probe::DeathGeneration();
        if(death_overlay_scene!=lastDeathWanted||ui89_policy.context!=lastDeathContext||pauseSurfaceEpoch!=lastDeathSurface||pauseSize!=lastDeathSize||deathGeneration!=lastDeathGeneration){
            lastDeathWanted=death_overlay_scene;lastDeathContext=ui89_policy.context;lastDeathSurface=pauseSurfaceEpoch;lastDeathSize=pauseSize;lastDeathGeneration=deathGeneration;
            deathAttempts=3;deathNext=now_ms;
        }
        bool deathMaintainRun160=false;
        if(deathAttempts&&now_ms>=deathNext&&(!death_overlay_scene||pauseSurfaceReady)){
            --deathAttempts;const bool ok=flashui_probe::ApplyDeathOverlayLayout(death_overlay_scene,pauseGeometry,pauseSize);
            if(ok)deathAttempts=0;else deathNext=now_ms+350;
            const bool quietDeath160=deathMaintainRun160&&ok;deathMaintainRun160=false;
            if(!quietDeath160&&(ok||!deathAttempts)){char m[192]{};std::snprintf(m,sizeof(m),"V66.135R2-DEATH-FIT wanted=%d result=%s size=%.2f theater=OFF background=FULL_FIELD",death_overlay_scene,ok?"VERIFIED":"DEFERRED",double(pauseSize));diagnostic_trace::Write(m);}
        }


        xr_manager_->SetNativeMenuPointerHand(ui_menu_input::PointerHand());
        xr_manager_->SetUiLayerTest(f2_ui_layers_test,f2_ui_menu_active);
        static bool v6686_ui_was_flat=false;
        if(v6686_ui_was_flat && !safe_ui_fit){
            touch_bindings_.Reset();
            xr_manager_->SetFlatUiCursorVisual(0.5f,0.5f,false);
            diagnostic_trace::Write("V66.86-UI-EXIT HARD-RELEASE touchBindings=RESET laser=OFF menuPlane=OFF");
        }
        v6686_ui_was_flat=safe_ui_fit;
        // V66.78 ROOT CAUSE of the dead Theater Inventory/Shop since V66.64: the HUD layer redirected
        // KCD2's late UI draws (which in Theater ARE the Inventory/Shop menu) away from the backbuffer
        // that Theater shows. The redirect only runs in immersive stereo now; in Theater KCD2's own
        // menu image is shown untouched (V65.77b behaviour).
        hud_layer_v6664::SetEnabled((vr_production_stereo_mode_.load(std::memory_order_acquire)==3 || f2_ui_layers_test) && !safe_ui_fit);
        xr_manager_->SetFlatUiScale(vr_flat_screen_scale_.load(std::memory_order_relaxed));
        xr_manager_->SetFlatUiDistance(1.50f);
        // UI input below owns device family and execution together. No worker-thread
        // SetInputId hammering and no global fake-XI gameplay transport.
        // V64.85 release default: keep gameplay interaction targeting aligned with the
        // visible OpenXR head direction without handing camera ownership to native mouse
        // look.  This is the safe "shadow interaction aim" layer: eye/reach stay native,
        // only the pickup/use ray + on-screen gate follow final visual forward. Theater
        // owns mouse/keyboard UI, so the interaction redirect is disabled there.
        {   // C7G: cover entry/exit as well as the recognized book/activity stage.
            // Native third-person cutscenes and full-menu cameras retain their FOV.
            const bool readingHead32=reading_pose32::nativeContext(
                vrik_probe_installed&&visual_selection59c7f::Selected(frik_pose_probe::CalibrationReady(),frik_pose_probe::CombatCensusMode(),frik_pose_probe::FinalSkinTestMode()),
                cutscene_flat_ui,GetTickCount64(),ui89_policy);
            const bool wanted=head_aim_retail_verified&&!policy_theater_scene&&(!auto_cutscene_theater||readingHead32)&&
                !native_dialogue_flat_ui&&!matrix_active&&!diagnostics::lab::NativeReference()&&
                (apply_head_rotation_.load()||apply_head_position_.load());
            if(g_immersive_fov59c7g.load())immersive_fov59c7g::Update(wanted,menu_game_fov_target_.load(),GetTickCount64()); // R59.16C9A
            if(g_activity_lens59c10r1.load()){
                activity_lens59c10r1::Update(wanted,menu_game_fov_target_.load(),now_ms);
                activity_lens59c10r1::Report(now_ms);
            }
            const auto result=focusZoom58c2.step(wanted,GetTickCount64(),focusZoomIO58c2);
            if(result.attempted){char m[240]{};std::snprintf(m,sizeof(m),"R58C2 BOOK_FOCUS_ZOOM scope=%d applied=%d owned=%d nativeBaseline=%.3f target=0 setting=wh_pl_FocusFOVMaxDecrease steadyCalls=0",wanted,result.ok,focusZoom58c2.owned,double(focusZoom58c2.baseline));diagnostic_trace::Write(m);}
        }
        const bool activityFadeWanted123=first_person_activities_.load(std::memory_order_acquire)&&head_aim_retail_verified&&!diagnostics::lab::NativeReference();
        const auto fadeNow123=std::uint64_t(GetTickCount64());
        if(lastActivityFade123!=int(activityFadeWanted123)||(retryActivityFade123&&fadeNow123>=retryActivityFade123)){
            const bool applied123=activityFade123.Apply(activityFadeWanted123);
            lastActivityFade123=int(activityFadeWanted123);retryActivityFade123=applied123?0:fadeNow123+2000;
            char m[160]{};std::snprintf(m,sizeof(m),"V66.123R1-ACTIVITY-FADE requested=%d applied=%d scope=HERBS_DIGGING_BUTCHERING_ALCHEMY globalFader=UNTOUCHED",activityFadeWanted123,applied123);diagnostic_trace::Write(m);
        }
        // V66.155: outside VRIK, interaction follows the final OpenXR visual view by
        // default. This is NOT native head-aim: body/camera ownership is unchanged; only
        // KCD2's pickup/use ray and on-screen interaction gate use the visual-forward ray.
        // Inside VRIK the head ray is disabled so hand/finger interaction can be the sole
        // interaction authority.
        const int viewFollowVrikMode=frik_pose_probe::CombatCensusMode();
        const bool nonVrikVisualInteraction=(viewFollowVrikMode==0);
        const bool head_interaction_aim = interaction_alignment_installed && nonVrikVisualInteraction &&
            !safe_ui_fit && !f2_ui_menu_active && !matrix_active &&
            !diagnostics::lab::NativeReference() && !ui89_policy.BlocksWorldInteraction() && !activity163.nativeControls;
        const bool vrikHandInteraction = interaction_alignment_installed && viewFollowVrikMode>=2 &&
            !safe_ui_fit && !f2_ui_menu_active && !matrix_active &&
            !diagnostics::lab::NativeReference() && !ui89_policy.BlocksWorldInteraction() && !activity163.nativeControls;
        // R59.16C2: the alchemy minigame keeps the interactor backend so a pointing hand can aim KCD2's own
        // interactor at the table's tools. Native minigame input (PreserveNative) is allowed; a modal/menu is not.
        const auto& alchemyWitness59c2=activity_input163::witness175;
        const bool alchemyAvailable63=interaction_alignment_installed&&
            activity_input163::trackedHands59c7f[unsigned(activity_input163::Kind175::Alchemy)].load()&&
            activity163.nativeControls&&alchemyWitness59c2.ready.load(std::memory_order_acquire)&&
            !alchemyWitness59c2.transitionDepth175.load(std::memory_order_acquire)&&
            alchemyWitness59c2.active[unsigned(activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire)&&
            !safe_ui_fit&&!f2_ui_menu_active&&!matrix_active&&!diagnostics::lab::NativeReference()&&
            (!ui89_policy.BlocksWorldInteraction()||ui89_policy.NativeContextControls());
        const bool alchemyPalmExperiment63=combat_rules178r10::f5Rung.load(std::memory_order_acquire)==2&&
            workstation178r20c1::experimentArmed36.load(std::memory_order_acquire);
        const bool alchemyGesture67=alchemyAvailable63&&minigame_hands45::modeFor(minigame_hands45::Alchemy)!=minigame_hands45::Native&&
            (minigame_hands45::freeTrackedMode(minigame_hands45::modeFor(minigame_hands45::Alchemy))||alchemyPalmExperiment63);
        // H18: the obsolete whole-hand ray branch was reachable only in NATIVE
        // mode while F5=2, stealing gaze from the native fallback. Free Alchemy
        // already nominates ONE qualified nearby hand in its OnScreen bridge;
        // everywhere else (including Native/Semi and book) keep HMD gaze.
        const bool alchemyPointer59c2=false;
        const bool alchemyHeadSelection63=alchemyAvailable63;
        subsystems::alchemy::bridge67::enabled.store(alchemyGesture67,std::memory_order_release);
        const bool interactionBackendEnabled=head_interaction_aim||vrikHandInteraction||alchemyPointer59c2||alchemyHeadSelection63;
        // One selector at a time: normal/guided passenger is HMD gaze; only
        // the qualified Free bridge may nominate a nearby hand. Retire old rays on the gaze edge.
        static bool alchemyHeadWas63=false;
        if(alchemyHeadSelection63&&!alchemyHeadWas63){
            interaction_alignment::SetPalmRays59c9({});
            interaction_alignment::SetHandRayWorld(false,0,0,0,0,1,0);
        }
        alchemyHeadWas63=alchemyHeadSelection63;
        interaction_alignment::SetNativeActivityFallback5916c3(false);
        interaction_alignment::SetVisualFallbackEnabled(head_interaction_aim||alchemyHeadSelection63);
        interaction_alignment::SetEnabled(interactionBackendEnabled);
        // V66.170: prompt geometry changes only with settings/HUD lifecycle.
        {
            static int last_head_interaction=-1;
            const int now_head_interaction=head_interaction_aim?1:0;
            if(now_head_interaction!=last_head_interaction){
                diagnostic_trace::Write("V66.155 VIEW_FOLLOW headFollow=RENDERER_MATCHED_PRESENT; interaction owner transition; camera unchanged");
                diagnostic_trace::Write(head_interaction_aim
                    ? "V66.155-VIEW-FOLLOW NONVRIK visual-forward owns pickup/use ray; camera/body remain OpenXR/native unchanged"
                    : "V64.85-HEAD-INTERACT OFF theater/lab owns presentation");
                last_head_interaction=now_head_interaction;
            }
        }
        {
            static bool last_valid=false,last_effective=false,last_auto=false; static int last_scene=-1,last_input=-1;
            if(!last_valid || safe_ui_fit!=last_effective || auto_theater!=last_auto || presentation_scene_key!=last_scene || int(ui89_policy.input)!=last_input){
                char tm[320]{}; std::snprintf(tm,sizeof(tm),
                    "V66.97-PRESENTATION scene=%d owner=%s finalFrame=%d fitWhole=%d pauseOverlay=%d passiveFull=%d passiveScale=%.2f input=%d pointerAllowed=%d context=%llu",
                    presentation_scene_key,presentation_owner,safe_ui_fit?1:0,(ui89_policy.FitCompleteFrame()||(authored_scene_theater&&ui89_policy.owner==CleanMenuOwner::None))?1:0,pause_overlay_scene?1:0,passive_fullscreen_scene?1:0,
                    double(passive_fullscreen_scene?ui_scene_presentation::PassiveScale(vr_passive_full_frame_scale_.load(std::memory_order_acquire)):1.0f),
                    int(ui89_policy.input),ui89_policy.PointerAllowed()?1:0,(unsigned long long)ui89_policy.context);
                diagnostic_trace::Write(tm);
                last_valid=true; last_effective=safe_ui_fit; last_auto=auto_theater; last_scene=presentation_scene_key; last_input=int(ui89_policy.input);
            }
        }
        xr_manager_->SetFlatUiCurved(false); // one original large flat theater screen
        {   // R59.16C10R14 (Miki: "the game behind the pause menu runs very slowly, stuttering"): the paused native picture is
            // frozen (no HMD injection) but was submitted with the LIVE per-frame eye pose, so every head movement dragged
            // and juddered the still image. While the pause overlay is up the world layer uses a frozen render pose
            // (world-fixed still picture; the head-locked menu quad is unaffected). The F8 ladder owns the mode while it runs.
            static bool pausePoseFrozen59c14=false;
            const bool wantFrozen=pause_overlay_scene&&!matrix_active&&g_pause_frozen_pose59c14.load(std::memory_order_relaxed);
            if(wantFrozen!=pausePoseFrozen59c14){pausePoseFrozen59c14=wantFrozen;xr_manager_->SetProjectionPoseMode(wantFrozen?1:0);
                diagnostic_trace::Write(wantFrozen?"R59.16C10R14 PAUSE_POSE frozen render pose ON (world-fixed still background behind the pause menu)":"R59.16C10R14 PAUSE_POSE frozen render pose OFF (live eye pose)");}
        }
        const bool suspend_game_camera_for_ui=safe_ui_fit||f2_ui_menu_active||immersive_system_overlay;
        if (suspend_game_camera_for_ui != tracking_suspended_for_flat_ui) {
            tracking_suspended_for_flat_ui = suspend_game_camera_for_ui;
            CameraHook::SetTrackingOptions(
                suspend_game_camera_for_ui ? false : apply_head_rotation_.load(std::memory_order_relaxed),
                suspend_game_camera_for_ui ? false : apply_head_position_.load(std::memory_order_relaxed));
            debug_trace::Write(suspend_game_camera_for_ui
                ? (pause_overlay_scene?"V66.124R1-PAUSE-CAMERA frozen native background; scaled overlay; theater=OFF":f2_ui_menu_active?"V66.85-F2-UI game-camera HMD injection suspended; stereo world remains behind fixed XR menu":"V66.20-MENU-PLANE game-camera HMD injection suspended while world projection hidden")
                : "V66.20-MENU-PLANE game-camera HMD injection restored for immersive stereo");
        }

        // V62.9 dialogue/source-FOV restore. The known-good V60.1/V61.36 rule was
        // that transient dialogue/inventory camera publications must never become
        // the compositor's source-FOV calibration. Keep the user-confirmed source
        // VFOV authoritative while immersive; F6 theater has its own flat geometry.
        if(!matrix_active && !safe_ui_fit && xr_manager_) {
            static std::uint64_t last_source_guard_ms=0;
            static float last_source_guard_value=-1.0f;
            if(dialogue_guard_active && dialogue_fov_override){
                if(std::fabs(last_source_guard_value-dialogue_guard_fov)>0.01f){
                    xr_manager_->SetSourceVerticalFov(dialogue_guard_fov);
                    last_source_guard_value=dialogue_guard_fov;
                }
                last_source_guard_ms=now_ms;
            } else if(now_ms-last_source_guard_ms>=250ull) {
                last_source_guard_ms=now_ms;
                const int source_mode=std::clamp(vr_cyclopean_mode_.load(std::memory_order_acquire),0,3);
                // V65.07: corrected mode must keep the last explicit RECALCULATE
                // source geometry. The editable VR View FOV is a separate framing
                // value and must never leak back into source_vertical_fov_deg_.
                const float wanted_source = source_mode==1
                    ? std::clamp(vr_corrected_source_vfov_deg_.load(std::memory_order_acquire),70.0f,130.0f)
                    : EffectiveSourceFovForMode(source_mode,vr_source_vertical_fov_deg_.load(std::memory_order_acquire));
                if(std::fabs(last_source_guard_value-wanted_source)>0.01f){
                    xr_manager_->SetSourceVerticalFov(wanted_source);
                    last_source_guard_value=wanted_source;
                }
            }
        }

        // V61.34 F4 is observation-only generic depth discovery. It never
        // changes pseudo strength, DOF state, presentation, or resource contents.

        const bool graphics_refresh = graphics_profile_refresh_requested_.exchange(false, std::memory_order_acq_rel);
        refresh_vr_graphics_profile(graphics_refresh);
        refresh_game_fov_once(ui89_policy.owner==CleanMenuOwner::None && ui89_policy.uncertain==0 &&
            native_ui_snapshot.Open(ui_native_state::Hud) && !dialogue_active && !cutscene_flat_ui);
        refresh_dialogue_camera_mode();

        // V62.4: generic MasterSwitch/percentage text controls remain retired.
        // Native semantic UI is issued directly from the menu/control thread against
        // declared HUD MovieClips. The old passive FlashUI observer/retry loop is
        // intentionally gone; do not retain dead references here.

        const bool matrix_requested = matrix_test_requested_.exchange(false, std::memory_order_acq_rel);
        if (matrix_requested) { // V61.15: keyboard F8 path retired
            if(fake_dof_active_.exchange(false,std::memory_order_acq_rel)) {
                fake_dof_started_ms_.store(0,std::memory_order_release);
                debug_trace::Write("V61.12-FAKE-DOF cancelled by manual F8 pseudo preset selection");
            }
            if(engine_dof_active_.exchange(false,std::memory_order_acq_rel))
                debug_trace::Write("V61.12-ENGINE-DOF cancelled by manual F8 pseudo preset selection");
            // V61.5: native stereo branch retired. F8/menu Stereo Test is now a manual
            // single-image pseudo-stereo preset selector. MONO remains the V61.4
            // LEGACY_GOOD_MONO hard gate and is never routed through cyclopean processing.
            static constexpr int kPseudoCycle[4] = {1, 2, 3, 0}; // V61.10: .80 -> .85 -> .95 -> MONO
            const int cur = std::clamp(vr_cyclopean_mode_.load(std::memory_order_relaxed), 0, 3);
            int next = 1;
            for (int i = 0; i < 4; ++i) {
                if (kPseudoCycle[i] == cur) { next = kPseudoCycle[(i + 1) % 4]; break; }
            }
            // From MONO, start at PS1 rather than skipping to PS2.
            if (cur == 0) next = 1;

            vr_stereo_experiment_.store(0, std::memory_order_release);
            vr_cyclopean_mode_.store(next, std::memory_order_release);
            vr_afr_ipd_scale_.store(0.0f, std::memory_order_release);
            DeviceHook::SetAfrStereoCapture(false);
            FrameSync::SetStereoMode(0);
            FrameSync::SetNativeStereoReentryMode(0, 0.064f);

            if (xr_manager_) {
                const V603ViewPreset preset = ViewPresetForMode(next);
                xr_manager_->SetStereoExperiment(0, 0.0f);
                xr_manager_->SetPresentationSettings(
                    vr_render_scale_.load(std::memory_order_relaxed),
                    preset.presentation_mode,
                    vr_zoom_.load(std::memory_order_relaxed),
                    0.0f, 0.0f, 0.0f, 0.0f,
                    vr_source_vertical_fov_deg_.load(std::memory_order_relaxed));
                xr_manager_->SetAngularCorrectMono(preset.angular_correct);
                xr_manager_->SetPoseHonestyOverride(preset.pose_honesty);
                xr_manager_->SetMonoFovAsymmetryScale(preset.fov_asymmetry);
                xr_manager_->SetMonoUvReprojectionMode(MonoMapModeForCleanMode(next));
                xr_manager_->SetMonoUvBinocularOffset(preset.uv_binocular);
            }

            const char* name = "MONO_OLD";
            switch (next) {
                case 1: name = "MONO_CORRECTED_TANGENT110"; break;
                case 2: name = "PSEUDO_090"; break;
                case 3: name = "PSEUDO_DEPTH_070_055"; break;
                default: break;
            }
            char msg[256]{};
            std::snprintf(msg, sizeof(msg),
                "V61.9-PSEUDO-SELECT mode=%d name=%s asym=%.3f uv=%.4f; 0.95 preferred; native/AFR/DUAL off",
                next, name, static_cast<double>(CyclopeanAsymmetryForMode(next)),
                static_cast<double>(ViewPresetForMode(next).uv_binocular));
            debug_trace::Write(msg);
            MessageBeep(MB_OK);
        }
        // Retired automated AFR/DUAL ladder remains unreachable.

        if (safe_mono_requested_.exchange(false, std::memory_order_acq_rel))
        {
            vr_cyclopean_mode_.store(0, std::memory_order_relaxed);
            if (matrix_active) {
                debug_trace::Write("V49-MENU Good Mono requested: cancelling active F8 ladder");
                char fps_restore_msg[256]{};
                (void)ApplyFpsCap(matrix_original_fps, 0.0f, fps_restore_msg, sizeof(fps_restore_msg)); char dup_restore[192]{}; (void)ApplyDuplicateLastFrame(matrix_original_fps.duplicate_last_frame,dup_restore,sizeof(dup_restore)); if(dup_restore[0]) debug_trace::Write(dup_restore); FrameSync::SetDenseRotationProbe(false);
                if (fps_restore_msg[0]) debug_trace::Write(fps_restore_msg);
                // Restore while matrix_active is still true so restore_safe_mono
                // uses the pre-test FOV/framing snapshot rather than the current phase.
                DeviceHook::SetSceneColorProbe(false,0,0); FrameSync::SetNativeStereoReentryMode(0,0.064f); RestoreV54StereoCVars(v54_stereo_original);
                xr_manager_->SetLatePoseEnabled(false); xr_manager_->SetPresentPosePairingMode(1); xr_manager_->SetProjectionPoseMode(0);
                xr_manager_->SetStereoExperiment(0,0.0f); DeviceHook::SetAfrStereoCapture(false); FrameSync::SetStereoMode(0); CameraHook::SetAfrIpd(0.064f,0.0f);
                CameraHook::SetInjectionMode(1); CameraHook::SetTrackingOptions(true,true); CameraHook::SetPoseSourceMode(vr_pose_source_mode_.load(std::memory_order_relaxed),vr_pose_prediction_offset_ms_.load(std::memory_order_relaxed)); CameraHook::SetRotationOrder(vr_rotation_order_.load(std::memory_order_relaxed)); CameraHook::SetRotationFilterMode(5); adaptive_rotation_filter=false;
                // F8 ladder addition: same restore as normal completion -
                // without this, cancelling mid-test would leave these stuck
                // at whatever the interrupted phase had set.
                xr_manager_->SetAngularCorrectMono(crysis_crop_mono);
                xr_manager_->SetPoseHonestyOverride(-1);
                xr_manager_->SetMonoUvReprojectionMode(0); xr_manager_->SetMonoUvBinocularOffset(0.0f);
                xr_manager_->SetMonoBottomEdgeMode(0); xr_manager_->SetMonoFovAsymmetryScale(CyclopeanAsymmetryForMode(vr_cyclopean_mode_.load(std::memory_order_relaxed)));
                restore_safe_mono();
                scene_emergency_override = matrix_restore_scene_emergency_override;
                inventory_flat_ui = matrix_restore_inventory_flat_ui;
                CameraHook::SetCullFrustumScale(matrix_restore_cull_scale);
                matrix_active = false;
                matrix_phase = -1;
                matrix_running_for_ui_.store(false, std::memory_order_release);
            } else {
                restore_safe_mono();
            }
            debug_trace::Write("V47.0-GOOD-MONO applied: Mono + OpenXR FOV + safe capture FOV + separate framing + swap off");
        }

        if (matrix_active)
        {
            const auto now_ms = static_cast<std::uint64_t>(GetTickCount64());
            const std::uint64_t phase_duration_ms = (matrix_phase >= 0 && matrix_phase < kMatrixPhaseCount)
                ? matrix_phases[matrix_phase].duration_ms : 10000ull;
            if (now_ms - matrix_phase_start_ms >= phase_duration_ms)
            {
                finish_matrix_phase();
                if (matrix_phase + 1 < kMatrixPhaseCount)
                {
                    apply_matrix_phase(matrix_phase + 1);
                }
                else
                {
                    const MatrixCounters total = capture_matrix_counters();
                    const auto total_req = delta_u64(total.recursive_requests, matrix_cycle_start.recursive_requests);
                    const auto total_pass = delta_u64(total.recursive_passes, matrix_cycle_start.recursive_passes);
                    const auto total_pipe = delta_u64(total.recursive_pipeline, matrix_cycle_start.recursive_pipeline);
                    const auto total_rt = delta_u64(total.recursive_rt, matrix_cycle_start.recursive_rt);
                    char fps_restore_msg[256]{};
                    (void)ApplyFpsCap(matrix_original_fps, 0.0f, fps_restore_msg, sizeof(fps_restore_msg)); char dup_restore[192]{}; (void)ApplyDuplicateLastFrame(matrix_original_fps.duplicate_last_frame,dup_restore,sizeof(dup_restore)); if(dup_restore[0]) debug_trace::Write(dup_restore); FrameSync::SetDenseRotationProbe(false);
                    if (fps_restore_msg[0]) debug_trace::Write(fps_restore_msg);
                    xr_manager_->SetLatePoseEnabled(false);
                    xr_manager_->SetPresentPosePairingMode(1);
                    xr_manager_->SetProjectionPoseMode(0);
                    xr_manager_->SetStereoExperiment(0,0.0f);
                    DeviceHook::SetAfrStereoCapture(false);
                    FrameSync::SetStereoMode(0);
                    CameraHook::SetAfrIpd(0.064f,0.0f);
                    CameraHook::SetInjectionMode(1);
                    CameraHook::SetTrackingOptions(true,true);
                    CameraHook::SetPoseSourceMode(vr_pose_source_mode_.load(std::memory_order_relaxed),
                                                  vr_pose_prediction_offset_ms_.load(std::memory_order_relaxed));
                    CameraHook::SetRotationOrder(vr_rotation_order_.load(std::memory_order_relaxed));
                    CameraHook::SetRotationFilterMode(5); adaptive_rotation_filter=false;
                    FrameSync::SetNativeStereoReentryMode(0,0.064f);
                    DeviceHook::SetSceneColorProbe(false,0,0);
                    RestoreV54StereoCVars(v54_stereo_original);
                    // F8 ladder addition: restore both overrides back to
                    // normal-gameplay state - crysis_crop_mono is the user's
                    // own toggled preference (same variable normal gameplay
                    // uses), pose_honesty_override_=-1 returns to the default
                    // stereo_experiment_==0 gating. Without this, gameplay
                    // after the F8 test would be stuck on whatever the LAST
                    // ladder phase happened to set.
                    xr_manager_->SetAngularCorrectMono(crysis_crop_mono);
                    xr_manager_->SetPoseHonestyOverride(-1);
                    xr_manager_->SetMonoUvReprojectionMode(0); xr_manager_->SetMonoUvBinocularOffset(0.0f);
                xr_manager_->SetMonoBottomEdgeMode(0); xr_manager_->SetMonoFovAsymmetryScale(CyclopeanAsymmetryForMode(vr_cyclopean_mode_.load(std::memory_order_relaxed)));
                    V54DoneBeep();
                    (void)ApplyNativeGameFov(matrix_restore_fov);
                    vr_source_vertical_fov_deg_.store(matrix_restore_fov, std::memory_order_release);
                    menu_game_fov_target_.store(matrix_restore_fov, std::memory_order_release);
                    restore_safe_mono();
                    scene_emergency_override = matrix_restore_scene_emergency_override;
                    inventory_flat_ui = matrix_restore_inventory_flat_ui;
                    CameraHook::SetCullFrustumScale(matrix_restore_cull_scale);
                    matrix_active = false;
                    matrix_running_for_ui_.store(false, std::memory_order_release);
                    matrix_phase = -1;
                    char done[640]{};
                    std::snprintf(done, sizeof(done),
                        "V61.0-F8-COMPLETE BUILD=AFR-DUAL-TPV-MENU-20260906-N restored pre-test FOV/preset; recursiveTotals={requests:%llu passes:%llu pipeline:%llu rt:%llu}",
                        static_cast<unsigned long long>(total_req), static_cast<unsigned long long>(total_pass),
                        static_cast<unsigned long long>(total_pipe), static_cast<unsigned long long>(total_rt));
                    debug_trace::Write(done);
                }
            }
        }

        // V66.69: legacy stereo/eye-swap and timed native-stereo archaeology hotkeys remain retired.
        // F2/F8 do not own production behavior; VRIK calibration is menu-driven.
        const bool menu_open = vr_menu_ && vr_menu_->IsVisible();
        (void)menu_open;
        f6_down_prev=f6_down; f7_down_prev=f7_down; f9_down_prev=f9_down; f10_down_prev=f10_down; f11_down_prev=f11_down; f12_down_prev=f12_down;
        // V64.31 CRITICAL: do not re-enable the old additive HMD rotation later
        // in the frame after F4 native-look has already taken rotation authority.
        // V64.28-V64.30 accidentally overwrote the F4 decision here every loop.
        CameraHook::SetTrackingOptions(
            suspend_game_camera_for_ui ? false : (view_marriage_visual_active ? true : (native_head_rotation_owned ? false : apply_head_rotation_.load(std::memory_order_relaxed))),
            suspend_game_camera_for_ui ? false : apply_head_position_.load(std::memory_order_relaxed));
        if(safe_ui_fit) {
            static std::uint64_t lastTheaterCamLog=0; const auto tnow=static_cast<std::uint64_t>(GetTickCount64());
            if(tnow-lastTheaterCamLog>=2000ull){ lastTheaterCamLog=tnow; diagnostic_trace::Write("V64.84-THEATER-CAMERA final-owner rot=0 pos=0 after head-look block"); }
        }
        XrPose head_pose{};
        // V46: RunFrame blocks on xrWaitFrame(), samples pose/views once at
        // predictedDisplayTime, immediately publishes that head pose to
        // CameraHook, then uses the same cached RenderFrameState for XR
        // composition. No post-xrEndFrame camera update.
        xr_manager_->SetComfortVignette(g_comfort_vignette51.load(std::memory_order_acquire));
        xr_manager_->SetComfortGameplay51(!menu_open&&!matrix_active&&!theater_presented&&!safe_ui_fit&&
            !ui89_policy.BlocksWorldInteraction()&&!activity163.nativeControls&&!dialogue_active&&
            !pause_overlay_scene&&!death_overlay_scene&&!diagnostics::lab::NativeReference(),
            vr_turn_mode_.load(std::memory_order_acquire)==1);
        const bool xr_head_valid=xr_manager_->RunFrame(head_pose, head_pose_scale_.load(std::memory_order_relaxed));
        g_gesture_frame59c5.valid=xr_head_valid;g_gesture_frame59c5.head=head_pose; // R59.16C5
        if(xr_head_valid){g_gesture_frame59c5.hands[0]=xr_manager_->RightController();g_gesture_frame59c5.hands[1]=xr_manager_->LeftController();}
        g_stallCp178r30[2]=std::uint64_t(GetTickCount64()); // R30 checkpoint: XR frame done
        const bool mis_menu_after_frame = vr_menu_ && vr_menu_->IsVisible();
        auto ui88_left=xr_manager_->LeftController();const auto& ui88_right=xr_manager_->RightController();
        const bool pauseChord58c2=touch_bindings_.UpdatePauseChord58c2(ui88_left,ui88_right,!mis_menu_after_frame);
        const bool pauseConsumed58c2=xr_manager_->LeftController().menu_down&&!ui88_left.menu_down;
        // Pure pose math each sample; Flash work only on mask/lifetime transitions.
        using namespace ui_contextual_hud162r1;
        Input palmInput36{};palmInput36.now=GetTickCount64();
        palmInput36.enabled=vr_contextual_hud_.load();
        palmInput36.vrik=visual_selection59c7f::Selected(frik_pose_probe::CalibrationReady(),frik_pose_probe::CombatCensusMode(),frik_pose_probe::FinalSkinTestMode());
        palmInput36.gameplay=!ui89_policy.BlocksWorldInteraction()&&!mis_menu_after_frame&&!pause_overlay_scene&&!death_overlay_scene;
        if(xr_manager_)xr_manager_->SetHudLagGameplay(palmInput36.gameplay&&!theater_presented&&!dialogue_active); // C22b: lag only in free gameplay
        palmInput36.headValid=xr_head_valid;
        palmInput36.head={head_pose.pos_x,head_pose.pos_y,head_pose.pos_z};
        palmInput36.headOrientation={head_pose.qx,head_pose.qy,head_pose.qz,head_pose.qw};
        const auto graphicsCamera31=activity_lens59c10r1::Dialogue(palmInput36.now); // copied native View observation only
        const bool graphicsStereo31=vr_production_stereo_mode_.load(std::memory_order_acquire)==3&&
            !matrix_active&&!diagnostics::lab::NativeReference();
        const bool graphicsEyeScope31=graphicsStereo31&&palmInput36.gameplay&&!theater_presented&&!dialogue_active&&
            !activity163.nativeControls&&dialogue_state59c10r2::fresh(graphicsCamera31,palmInput36.now)&&
            graphicsCamera31.camera==dialogue_state59c10r2::Camera::FirstPerson;
        g_vr_shadows_state26.ServiceLevel(graphicsStereo31?g_vr_shadow_quality49.load(std::memory_order_relaxed):0,palmInput36.now,g_graphics_io31); // C49 LOW/MEDIUM/HIGH
        tracking59c6::Stamp graphicsSource32{};
        const bool graphicsStamped32=xr_manager_->CopyInputSampleStamp59c6(graphicsSource32)&&graphicsSource32.tick&&
            graphicsSource32.epoch==tracking59c6::epoch.load(std::memory_order_acquire);
        const bool backEscapeM1=touch_bindings_.UpdateBackEscapeM1(ui88_right,
            xr_head_valid&&graphicsStamped32&&palmInput36.now>=graphicsSource32.tick&&palmInput36.now-graphicsSource32.tick<=80&&
            !matrix_active&&current_game_hwnd&&GetForegroundWindow()==current_game_hwnd,graphicsSource32.epoch);
        g_eye_adapt26.Service(g_eye_head26.load(std::memory_order_relaxed),graphicsEyeScope31,xr_head_valid&&graphicsStamped32,
            {head_pose.qx,head_pose.qy,head_pose.qz,head_pose.qw},palmInput36.now,g_graphics_io31,graphicsSource32.tick);
        g_graphics_receipts32.Observe(g_vr_shadow_quality49.load(std::memory_order_relaxed),g_eye_head26.load(std::memory_order_relaxed),
            diagnostics::capture168::active(diagnostics::capture168::Domain::Hud,palmInput36.now),
            g_vr_shadows_state26,g_eye_adapt26,[](const char* line){diagnostic_trace::Write(line);});
        const std::array<const ControllerState*,2> palmHands36{&ui88_left,&ui88_right};
        std::array<bool,2> open36{};
        for(unsigned h=0;h<2;++h){const auto&c=*palmHands36[h];
            palmInput36.palm[h]={{c.pose.pos_x,c.pose.pos_y,c.pose.pos_z},{c.pose.qx,c.pose.qy,c.pose.qz,c.pose.qw},c.active};
            const bool heldPalmD85R1=frik_pose_probe::ReadPhysicalHandHeld55(h==0?1:0,palmInput36.now)==1;
            open36[h]=ui_palm171::OpenCurrent(c.grip,c.trigger,
                c.a_down||c.b_down||c.x_down||c.y_down||c.menu_down||c.thumbstick_down,
                c.grip_valid,c.trigger_valid,c.trigger_touch,c.trigger_touch_valid,heldPalmD85R1);
        }
        // Use already copied native station/step and tracked-palm inputs. This
        // only queues presentation changes; no native poll, pose or input write.
        const auto alchemyPromptD80=promptStationD74.active[unsigned(activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire);
        const auto promptLifeD80=promptStationD74.lifetime175.load(std::memory_order_acquire);
        // The old currentStageCode37 sampler is F5-only. Use the production
        // copied work witness so timed prompts do not depend on diagnostics.
        static std::uint64_t promptPhaseLifeD80=0;static unsigned alchemyPromptPhaseD80=0;
        if(promptPhaseLifeD80!=promptLifeD80){promptPhaseLifeD80=promptLifeD80;alchemyPromptPhaseD80=0;}
        alchemy_semi46::WorkWitness promptWorkD80{};
        if(stationPromptsD74&&alchemyPromptD80&&alchemy_semi46::work.read(promptWorkD80)&&
           promptWorkD80.instance==alchemyPromptD80&&promptWorkD80.lifetime==promptLifeD80&&
           alchemy_semi46::fresh(promptWorkD80.tick,now_ms)){
            const auto identity=std::uint64_t(promptWorkD80.parent)^std::uint64_t(promptWorkD80.token)*0x9E3779B97F4A7C15ull;
            alchemyPromptPhaseD80=unsigned(identity^(identity>>32))^(promptWorkD80.kind<<24)^unsigned(promptWorkD80.phase+1);
        }
        const unsigned promptPhaseD80=alchemyPromptD80?alchemyPromptPhaseD80:unsigned(promptStationD74.SmithPhaseC1()+1);
        const bool promptPalmD80=palmInput36.gameplay&&palmInput36.headValid&&
            ((open36[0]&&Looking(palmInput36,0,false))||(open36[1]&&Looking(palmInput36,1,false)));
        prompt_device_lock::PublishMinigamePromptsD80(stationPromptsD74,promptLifeD80,promptPhaseD80,promptPalmD80,now_ms,alchemyPromptD80!=0);
        const auto reveal36=palmReveal36.Update(palmInput36,open36);
        const bool contextual36=reveal36.active;
        static std::uint64_t palmLog44c2=0;
        if(palmInput36.enabled&&diagnostics::capture168::active(diagnostics::capture168::Domain::Hud,palmInput36.now)&&
           palmInput36.now-palmLog44c2>=5000){palmLog44c2=palmInput36.now;
            char m[288]{};std::snprintf(m,sizeof(m),"R44C2 PALM_GAZE vrik=%d gameplay=%d head=%d valid=%d/%d open=%d/%d looking=%d/%d reveal=%d/%d thumbTouch=IGNORED cone=40deg",palmInput36.vrik,palmInput36.gameplay,palmInput36.headValid,palmInput36.palm[0].valid,palmInput36.palm[1].valid,open36[0],open36[1],Looking(palmInput36,0,false),Looking(palmInput36,1,false),reveal36.left,reveal36.right);diagnostic_trace::Write(m);
            // F2 only; both receipts use the existing KCD2VR.log diagnostic gate.
            char n[320]{};std::snprintf(n,sizeof(n),"R59.16C10R16 IMMERSIVE_HUD active=%d gameplay=%d vrik=%d(not required) reveal=L%d/R%d adaptiveOff=%d fireCursorOff=%d rehiddenTotal=%llu crosshairShown=%d",
                reveal36.active?1:0,palmInput36.gameplay?1:0,palmInput36.vrik?1:0,reveal36.left?1:0,reveal36.right?1:0,g_adaptive_hud57c1.Owned()?1:0,g_fire_cursor59c16.Owned()?1:0,
                (unsigned long long)g_hud_rehidden59c16.load(std::memory_order_relaxed),hud_crosshair_visible_.load()?1:0);
            KCD2VR_DIAG_LOG(Bug,warning,"{}",n);}

        flashui_probe::SetHudExperiment36(true,false); // alpha gate owns contextual visibility; native writer remains passive
        const bool gazeAlchemy54=alchemyHeadSelection63&&
            !mis_menu_after_frame&&!pause_overlay_scene&&!death_overlay_scene&&!dialogue_active&&!theater_presented;
        flashui_probe::SetHudSemanticOwner54(contextual36,reveal36.left,reveal36.right,gazeAlchemy54);
        const unsigned savedHidden54=(hud_crosshair_visible_.load()?0u:HideCrosshair163)|
            (hud_compass_visible_.load()?0u:HideCompass163)|(hud_lower_icons_visible_.load()?0u:HideLower163)|
            (hud_status_icons_visible_.load()?0u:HideStatus163);
        const unsigned userHidden170=ui_gameplay_hud54::Preferences(savedHidden54,palmInput36.gameplay,gazeAlchemy54);
        std::uint64_t hudLeaseEpoch54=1469598103934665603ull;
        ui_hud_work59c7::Mix(hudLeaseEpoch54,flashui_probe::HudGeneration());
        ui_hud_work59c7::Mix(hudLeaseEpoch54,native_ui_snapshot.epoch);
        {CVarLeaseBackend59c16 hintsIO{"wh_ui_ShowHints"};
         g_show_hints_d80.Update(hintsIO,true,prompt_policy_d80::showHints.load(std::memory_order_acquire),hudLeaseEpoch54);}

        // Palm visibility and alpha share the same reveal. Saved ordinary hide
        // preferences return on exit; quick selectors remain native and the
        // crosshair keeps its separate preference. This works without F2.
        // C32 user recovery: native control/interaction prompts remain available
        // in fights too. Palm reveal owns only its existing semantic groups.
        const unsigned hidden36=ui_hud_experiment36c5::VisibilityMask(userHidden170,reveal36);
        // Exact retail callback 1F79460 -> 1F79DC0 -> 1F79E20 releases native
        // adaptive fading and restores alpha to 100. The native menu/visibility
        // rules still run. No timer-driven CVar reassertion or Flash show loop.
        // C4R4: baseline is read-only; no global hit/ragdoll CVar amplification.
        {
            // Opt-in NPC controls do not depend on F4 mode or recording.
            // Activation is on this existing control thread, once per requested
            // value after failure. Native defaults install no extra hooks.
            static float npcDifficultyAttempt=-1.f;static int npcDefenseAttempt=-1;
            const float difficulty=npc_difficulty::difficulty.load(std::memory_order_relaxed);
            const int defense=npc_defense::Wanted()?1:0; // D34: dodge default 1.25 installs by default
            if(difficulty!=npcDifficultyAttempt){npcDifficultyAttempt=difficulty;
                if(difficulty>0.f&&!npc_difficulty::Ready()){
                    const bool ok=npc_difficulty::Install(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")));
                    diagnostic_trace::Write(ok?"NPC difficulty control ready (native/C8 AI inputs)":"NPC difficulty control unavailable; native AI retained");}}
            if(defense!=npcDefenseAttempt){npcDefenseAttempt=defense;
                if(defense&&!npc_defense::Ready()){
                    const bool ok=npc_defense::Install(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")));
                    if(ok)diagnostic_trace::Write("NPC defensive speed control ready");
                    else {char line[180]{};std::snprintf(line,sizeof(line),"NPC defensive speed unavailable reason=%s; current speed retained",npc_defense::Failure());diagnostic_trace::Write(line);}}}
            const bool consoleReady=retail_build112::Ready(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")))&&SafeConsoleGetCVar(SafeGetCryConsole(),"wh_HitReaction_Enabled");
            frik_pose_probe::TickPhysBodies59c10r8(); // R59.16C10R8: no-op while OFF; destroys bodies when switched off
            ServicePhysicalHeldF9T4();
#define KCD2VR_M2_HOST_SLOT 9010
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
            {RagdollConsole59 io43;
             // D51: no F9/global living-hit CVar forcing. Retire only values still owned by us.
             if(consoleReady){
                 (void)g_lease_impact43.Retire(io43);
                 (void)g_lease_recoil43.Retire(io43);
                 (void)g_lease_fall43.Retire(io43);
             }
             kcd2vr::lab58::impactLeaseOk.store(false,std::memory_order_release);}
            {RagdollConsole59 io39;g_npc_block_state39.Service(consoleReady&&frik_pose_probe::CombatExperiment175Enabled()&&frik_pose_probe::F4Rung178r10()==3,
                g_npc_block39.load(std::memory_order_relaxed),0,io39);} // C39
            {CombatWindowsConsoleH3 ioH3;
             const bool activeH3=consoleReady&&frik_pose_probe::CombatExperiment175Enabled()&&!matrix_active&&
                 !dialogue_active&&!theater_presented&&!mis_menu_after_frame&&!pause_overlay_scene&&!death_overlay_scene&&!cutscene_flat_ui;
             g_router_windows_h3.service(activeH3,GetTickCount64(),ioH3,[](const char* name,const char* event,float before,float after){
                 char line[256]{};std::snprintf(line,sizeof(line),"H3 WINDOW name=%s event=%s before=%.3f after=%.3f scope=GLOBAL_NATIVE_TIMING rpg=NATIVE ForceBlock=UNTOUCHED",name,event,double(before),double(after));diagnostic_trace::Write(line);});}
            {CombatWindowsConsoleH3 ioD70;
             const bool activeD70=consoleReady&&g_roomscale_dodge58.load(std::memory_order_acquire)&&!matrix_active&&
                 !dialogue_active&&!theater_presented&&!mis_menu_after_frame&&!pause_overlay_scene&&!death_overlay_scene&&!cutscene_flat_ui;
             g_dodge_window_d70.service(activeD70,GetTickCount64(),ioD70,[](const char* name,const char* event,float before,float after){
                 if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64()))return;
                 char line[256]{};std::snprintf(line,sizeof(line),"D70 DODGE_WINDOW name=%s event=%s before=%.3f after=%.3f scope=GLOBAL_ALL_ACTORS result=NATIVE",name,event,double(before),double(after));diagnostic_trace::Write(line);});}
            static bool baseline59c10r7=false;if(consoleReady&&!baseline59c10r7&&!g_ragdoll59.On()){baseline59c10r7=LogPhysicsBaseline59c10r7();}
            ServiceRagdollPushH6(consoleReady);
            // Atomic prerequisite reads only; log a transition, never each frame.
            static int f9Prerequisite57=-1;
            const int f9Now57=!g_ragdoll_wanted59.load()?0:frik_pose_probe::PhysicalCombat177Enabled()?2:1;
            if(f9Now57!=f9Prerequisite57){f9Prerequisite57=f9Now57;
                diagnostic_trace::Write(f9Now57==1?"C57R1 F9 status=WAITING_FOR_F4_PHYSICAL_COMBAT enabled=1 delivered=0":
                    f9Now57==2?"C57R1 F9 status=F4_PREREQUISITE_MET nativeDeathAndExactContactStillRequired=1":"C57R1 F9 status=OFF");}

        }
        AdaptiveHudBackend57c1 adaptiveIo57c1;
        // R59.16C10R16 (one HUD owner): while the immersive palm HUD owns the elements the game's adaptive fader is OFF (it faded hidden
        // elements back in); its one-time alpha reset is answered by re-hiding below, in the same frame.
        if(g_adaptive_hud57c1.Update(adaptiveIo57c1,whole_readable_wanted44c2()||contextual36,hudLeaseEpoch54)){
            char m[208]{};std::snprintf(m,sizeof(m),"R57C1 HUD_ADAPTIVE wantedOff=%d owned=%d nativeCallback=ALPHA_RESET visible=UNCHANGED steadyCalls=0",whole_readable_wanted44c2()||contextual36,g_adaptive_hud57c1.Owned());diagnostic_trace::Write(m);
        }
        {CVarLeaseBackend59c16 fireCursor{"wh_pl_ShowFireCursor"};
         // Held only during gameplay: a game options screen (menus) sees and may save the player's own value, never our 0.
         const bool fireOff59c16=palmInput36.gameplay&&!gazeAlchemy54;
         if(g_fire_cursor59c16.Update(fireCursor,fireOff59c16,hudLeaseEpoch54)){
            char m[180]{};std::snprintf(m,sizeof(m),"R59.16C10R16 HUD_FIRE_CURSOR wantedOff=%d owned=%d (bow centre point = wh_pl_ShowFireCursor)",fireOff59c16?1:0,g_fire_cursor59c16.Owned());
            diagnostic_trace::Write(m);KCD2VR_DIAG_LOG(Bug,warning,"{}",m);}}
        // Release palm alpha before ordinary visibility+alpha takes ownership.
        // Palm mode pins exact Stats ancestors only during a right reveal. The
        // semantic visibility mask and alpha owner share the same reveal edge.
        if(!contextual36)flashui_probe::UpdatePalmAlpha44c2(false,false,false,false,flashui_probe::HudAlphaGeneration54());
        const bool vitalsGameplay59c10=palmInput36.gameplay&&!dialogue_active&&!theater_presented&&
            !activity163.nativeControls&&!(hidden36&HideLower163);
        flashui_probe::UpdateHudVitals59c7(vitalsGameplay59c10&&!contextual36,vitalsGameplay59c10&&contextual36&&reveal36.right);
        flashui_probe::PublishHudVisibility163(!(hidden36&HideCrosshair163),!(hidden36&HideCompass163),!(hidden36&HideLower163),!(hidden36&HideStatus163));
        if(!native_ui_snapshot.Open(ui_native_state::Loading)&&!native_ui_snapshot.Open(ui_native_state::Progress))
            {
                const auto generation44c2=flashui_probe::HudGeneration();
                // Alpha and manual permission owners have separate event caches.
                if(!contextual36)flashui_probe::UpdateHudVisibility170(false,userHidden170,generation44c2);
                else flashui_probe::UpdateHudVisibility170(false,hidden36,generation44c2); // controller is event-only; preference changes must work while palm mode is active
                if(contextual36)flashui_probe::UpdatePalmAlpha44c2(true,reveal36.left,reveal36.right,action_map_probe::WeaponQamOpen171()||action_map_probe::FoodQamOpen171(),flashui_probe::HudAlphaGeneration54());
                // R112: quick weapon/potion selectors fade 3 s after the last quick-access D-pad input like the native
                // adaptive HUD did (that one stays off for the palm HUD, R57C1). Right palm vitals reveal shows them too.
                flashui_probe::UpdateQuickSlotFadeR112(ui_hud_r112::quickSlotFade.load(std::memory_order_acquire),contextual36&&reveal36.right,GetTickCount64());
                // C54 exact native alpha/visibility writers own reassertion;
                // no periodic Flash reads or 1-second hide loop.
            }
        // Retry the observer on real native lifecycle progress, not a timer or
        // a repeated manager lookup. Startup can precede the FlashUI singleton.
        static std::uint64_t flashRetryObservation163=~0ull;
        const auto flashEvent163=(std::uint64_t(native_ui_snapshot.epoch)<<32)|native_ui_snapshot.known;
        if(!flashui_live_installed && native_ui_snapshot.observations &&
           flashRetryObservation163!=flashEvent163){
            flashRetryObservation163=flashEvent163;
            flashui_live_installed=flashui_probe::Install();
            if(flashui_live_installed)(void)flashui_probe::SetHudRepair175(true);
            if(flashui_live_installed)diagnostic_trace::Write("V66.163-HUD observer READY after native lifecycle event");
        }
        const auto nativeXiNow=static_cast<std::uint64_t>(GetTickCount64());
        if(native_input_look_lab_installed && native_input_look_lab::GetGamepadMode()!=3)
            native_input_look_lab::SetGamepadMode(3);
        if(native_input_look_lab_installed && !native_input_look_lab::NativeGamepadFullReady() &&
           native_input_look_lab::GamepadDiscoveryContextReady() &&
           native_xi_discovery_attempts<2 && (ui88_left.active||ui88_right.active) && nativeXiNow>=native_xi_next_discovery_ms){
            ++native_xi_discovery_attempts;
            (void)native_input_look_lab::DiscoverNativeGamepadSymbolsDirect();
            native_xi_next_discovery_ms=nativeXiNow+(native_xi_discovery_attempts==1?2000ull:5000ull);
        }
        const bool native_xi_full=native_input_look_lab_installed&&native_input_look_lab::NativeGamepadFullReady();
        const bool ui88_panic=ui88_left.active&&ui88_right.active&&ui88_left.thumbstick_down&&ui88_right.thumbstick_down;
        ui_menu_input::Frame ui88_input{};
        ui88_input.window=current_game_hwnd;
        ui88_input.menu=ui89_policy.MenuAdapter()&&!mis_menu_after_frame&&!ui88_panic;
        ui88_input.allow_pointer=ui89_policy.GripPointerAllowed175();
        ui88_input.passive_back_only=ui89_policy.PassiveBackOnly();
        ui88_input.block_gameplay=ui89_policy.BlocksMappedGameplay()&&!mis_menu_after_frame;
        ui88_input.consume_grips=ui89_policy.BlocksWorldInteraction()||mis_menu_after_frame;
        ui88_input.system_menu=ui89_policy.SystemMenu();
        ui88_input.native_gamepad=native_xi_full;
        ui88_input.inventory_root=ui89_policy.owner==CleanMenuOwner::InventoryMap;
        ui88_input.pointer_menu_kind=ui89_policy.owner==CleanMenuOwner::InventoryMap?1:ui89_policy.owner==CleanMenuOwner::Shop?2:ui89_policy.owner==CleanMenuOwner::Frontend?3:0;
        ui88_input.modal=(ui89_policy.supported&ui_native_catalogue::ModalMask)!=0;
        // Left Menu uses the configured Inventory toggle only for its root;
        // other menus receive native Back, never an unrelated inventory opener.
        ui88_input.inventory_key=touch_bindings_.GetConfig().inventory;
        ui88_input.context=ui89_policy.context;
        ui88_input.left=ui88_left;ui88_input.right=ui88_right;
        if(!ui88_input.native_gamepad&&touch_bindings_.BackEscapeConsumedM1()){ui88_input.right.b_down=false;ui88_input.right.b_pressed=false;}
        ui88_input.ray_hand=xr_manager_->FlatUiPointerHand();
        ui88_input.ray_hit=xr_manager_->GetFlatUiPointer(ui88_input.u,ui88_input.v);
        static bool ui88_menu_prev=false;
        const bool ui89_captures_controls=ui88_input.menu||ui88_input.block_gameplay;
        if(ui88_menu_prev!=ui89_captures_controls){touch_bindings_.Reset();ui88_menu_prev=ui89_captures_controls;}
        const auto ui88_input_result=ui_menu_input::Update(ui88_input);
        xr_manager_->SetNativeMenuPointerHand(ui88_input_result.hand);
        xr_manager_->SetFlatUiCursorVisual(ui88_input_result.u,ui88_input_result.v,ui88_input_result.cursor);

        if(ui89_mark_requested){
            const auto canvas_report=flashui_probe::NativeCanvasReport();
            flashui_probe::WriteNativeCanvasSnapshot();
            char canvas_line[256]{};
            std::snprintf(canvas_line,sizeof(canvas_line),"V66.90-HUD canvas selected=%d active=%d status=%d complete=%d resolved=0x%03X written=0x%03X failed=0x%03X",
                vr_hud_native_canvas_fit_.load()?1:0,flashui_probe::NativeCanvasActive()?1:0,int(canvas_report.status),canvas_report.complete?1:0,
                canvas_report.resolved,canvas_report.written,canvas_report.failed);diagnostic_trace::Write(canvas_line);

            char marker[512]{};
            std::snprintf(marker,sizeof(marker),
                "V66.89-UI-MARK owner=%s presentation=%d input=%d pointerAllowed=%d context=%llu known=0x%08X visible=0x%08X supported=0x%08X uncertain=0x%08X hookMask=0x%X losses=%llu",
                ui_menu_router::Name(ui89_policy.owner),int(ui89_policy.presentation),int(ui89_policy.input),ui89_policy.PointerAllowed()?1:0,
                (unsigned long long)ui89_policy.context,native_ui_snapshot.known,native_ui_snapshot.visible,ui89_policy.supported,ui89_policy.uncertain,
                native_ui_snapshot.installed,(unsigned long long)native_ui_snapshot.losses);
            diagnostic_trace::Write(marker);
            // Explicit hotkey only: fixed 30 records, no manager calls, pixel
            // readbacks, file scanning, or continuous per-frame diagnostics.
            for(unsigned k=0;k<ui_native_state::Count;++k){
                const auto kind=static_cast<ui_native_state::Kind>(k);
                const auto* entry=ui_native_catalogue::Find(kind);
                std::snprintf(marker,sizeof(marker),
                    "V66.89-UI-KIND name=%s kind=%u known=%d visible=%d samples=%llu role=%d stage=%ux%u textureConstraint=%ux%u",
                    ui_native_catalogue::Name(kind),k,native_ui_snapshot.Known(kind)?1:0,native_ui_snapshot.Open(kind)?1:0,
                    (unsigned long long)native_ui_snapshot.samples[k],entry?int(entry->role):-1,
                    entry?entry->stage_width:0u,entry?entry->stage_height:0u,entry?entry->texture_width:0u,entry?entry->texture_height:0u);
                diagnostic_trace::Write(marker);
            }
            ui_menu_input::WriteSnapshot();hud_layer_v6664::WriteSnapshot();
        }

        // V66.109: trigger ownership survives a menu closing inside RunFrame.
        // UI pointer/menu input owns the raw trigger while visible; gameplay and
        // VRIK receive it only after a tracked release/re-arm.
        const bool trigger_ui_after=mis_menu_after_frame||ui89_policy.BlocksWorldInteraction()||dialogue_active;
        const bool trigger_vrik_live=vrik_probe_installed&&frik_pose_probe::CombatCensusMode()>=2;
        const bool trigger_cal_ready=vrik_probe_installed&&frik_pose_probe::CalibrationReady();
        const bool trigger_activation=(trigger_vrik_live&&!trigger_vrik_was_live_)||(trigger_cal_ready&&!trigger_calibration_was_ready_);
        trigger_vrik_was_live_=trigger_vrik_live;trigger_calibration_was_ready_=trigger_cal_ready;
        const auto& trigger_l=xr_manager_->LeftController();const auto& trigger_r=xr_manager_->RightController();
        const float gameplay_trigger_l=gameplay_trigger_[0].sample(trigger_ui_after||trigger_activation,trigger_l.active,trigger_l.trigger);
        const float gameplay_trigger_r=gameplay_trigger_[1].sample(trigger_ui_after||trigger_activation,trigger_r.active,trigger_r.trigger);

        #include "dialogue_policy28.inl"
        // R111: ordinary gameplay with the world up (no menu/UI owner, no dialogue/cutscene/theater/activity) - the
        // start/load decision (calibrated = VRIK on, start at Henry's eyes) waits for this. published even while the
        // VRIK hooks are not installed yet (they are installed lazily by that start).
        kcd2vr::start_r111::worldReady.store(!ui89_policy.BlocksWorldInteraction()&&!mis_menu_after_frame&&!matrix_active&&
            !dialogue_active&&!activity163.nativeControls&&!cutscene_flat_ui&&!safe_ui_fit,std::memory_order_release);
        if(vrik_probe_installed) {
            const auto& lc=xr_manager_->LeftController();
            const auto& rc=xr_manager_->RightController();
            const frik_pose_probe::TrackingPose hp{head_pose.pos_x,head_pose.pos_y,head_pose.pos_z,head_pose.qx,head_pose.qy,head_pose.qz,head_pose.qw};
            const frik_pose_probe::TrackingPose lp{lc.pose.pos_x,lc.pose.pos_y,lc.pose.pos_z,lc.pose.qx,lc.pose.qy,lc.pose.qz,lc.pose.qw};
            const frik_pose_probe::TrackingPose rp{rc.pose.pos_x,rc.pose.pos_y,rc.pose.pos_z,rc.pose.qx,rc.pose.qy,rc.pose.qz,rc.pose.qw};
            const bool vrik_menu_busy=ui89_policy.BlocksWorldInteraction()||mis_menu_after_frame||activity163.nativeControls;
            const bool hud_hidden57=show_hud_cvar && SafeCVarGetInt(show_hud_cvar,1)==0;
            frik_pose_probe::GameStateContext gs{};
#include "vrik/workstation_pose_context19c4r2_producer.inl"
            gs.native_reading=activity_input163::witness175.ready.load(std::memory_order_acquire)&&
                activity_input163::witness175.active[unsigned(activity_input163::Kind175::Reading)].load(std::memory_order_acquire)!=0;
            gs.hud_hidden=hud_hidden57; // Presentation only, never a native Reading owner.
            // V64.19C: the proven native action-filter census is polled at 10 Hz
            // only after a VRIK lab is explicitly installed.
            static NativeAgencyStateV6419C agency_cached{};
            static std::uint64_t agency_next_poll_ms=0;
            static std::uint64_t agency_last_logged_mask=~0ull;
            static bool agency_last_logged_known=false;
            const std::uint64_t agency_now=static_cast<std::uint64_t>(GetTickCount64());
            if (agency_now>=agency_next_poll_ms) {
                agency_next_poll_ms=agency_now+100ull;
                agency_cached=ReadNativeAgencyStateV6419C();
                if (agency_cached.known!=agency_last_logged_known ||
                    (agency_cached.known && agency_cached.mask!=agency_last_logged_mask)) {
                    char active[512]{}; FormatAgencyMaskV6419C(agency_cached.known?agency_cached.mask:0,active,sizeof(active));
                    char m[768]{};
                    std::snprintf(m,sizeof(m),
                        "V64.19C-AGENCY known=%d count=%d scripted=%d mask=0x%08llX active=%s source=CCryAction+0x530/vslot+0x108",
                        agency_cached.known?1:0,agency_cached.known?agency_cached.count:-1,
                        kcd2vr::agency_r132::scripted(agency_cached.known,agency_cached.count,agency_cached.mask)?1:0, // R132
                        static_cast<unsigned long long>(agency_cached.mask),active);
                    diagnostic_trace::Write(m);
                    agency_last_logged_known=agency_cached.known;
                    agency_last_logged_mask=agency_cached.known?agency_cached.mask:~0ull;
                }
            }
            gs.agency_known=agency_cached.known;
            gs.agency_count=agency_cached.known?agency_cached.count:-1;
            // R132: the old rule (any 4 filters) took VRIK away for the whole prologue ride to Trosky - the game keeps
            // 5 story filters on there (no_inventory|no_dog|no_distract|no_companion_bond|no_horse_saddle_bond) while you
            // ride yourself. scripted now also needs a real control filter (move/camera, or 2 body-control ones).
            gs.scripted=kcd2vr::agency_r132::scripted(agency_cached.known,agency_cached.count,agency_cached.mask);
            { // R132: one KCD2VR.log row when VRIK is selected and the game takes control / gives it back (no on-screen text)
              static int lastScriptedR132=-1;
              const bool vrikSelectedR132=frik_pose_probe::CombatCensusMode()>=2;
              const int nowScriptedR132=vrikSelectedR132?(gs.scripted?1:0):-1;
              if(nowScriptedR132!=lastScriptedR132){
                  if(nowScriptedR132>=0&&(nowScriptedR132==1||lastScriptedR132==1||kcd2vr::agency_r132::storyOnly(agency_cached.known,agency_cached.count,agency_cached.mask))){
                      char active[512]{};FormatAgencyMaskV6419C(agency_cached.known?agency_cached.mask:0,active,sizeof(active));
                      char m[700]{};std::snprintf(m,sizeof(m),nowScriptedR132?
                          "R132 VRIK_WAIT reason=game took control (action filters %s) - VRIK body comes back by itself when they go":
                          "R132 VRIK_ON game control filters gone or story-only (%s) - VRIK body follows you",active);
                      diagnostic_trace::WriteStatus(m); // KCD2VR.log only
                  }
                  lastScriptedR132=nowScriptedR132;
              }
            }
            frik_pose_probe::PublishGameStateContext(gs);
            // Dialogue/cutscene/scripted control-loss are explicit VRIK authority
            // blockers. Unknown agency state stays fail-closed as "unknown", not
            // as a fabricated scripted scene.
            const bool vrik_gameplay_allowed=!safe_ui_fit && !vrik_menu_busy && !matrix_active &&
                !dialogue_active && !activity163.nativeControls && !cutscene_flat_ui && !gs.scripted;
            // V66.178R9: in dialogue the VRIK body/hands keep following the headset (pose only; no
            // physical handling, attacks, fists, hand-use or haptics). The native idle body clipped the view.
            // V66.178R13: F5 rung 2 keeps the VRIK upper body/arms at alchemy/smithing workstations (native fingers/tools).
#include "vrik/workstation_pose_body19c4r2_producer.inl"
            const bool dialogueRequested19c2=dialogue_pose47c1::Allow(dialoguePolicy28);
            const bool dialogueBody47c1=dialogueRequested19c2&&
                dialogue_state59c10r2::embodiedOwner(dialogueState59c10r2,GetTickCount64());
#include "vrik/reading_pose32_producer.inl"
            const bool heldPropBody43=frik_pose_probe::HeldInteractionVisual43()&&
                !safe_ui_fit&&!matrix_active&&!mis_menu_after_frame&&!dialogue_active&&!cutscene_flat_ui&&
                !ui89_policy.BlocksWorldInteraction()&&visual_selection59c7f::Selected(
                    frik_pose_probe::CalibrationReady(),frik_pose_probe::CombatCensusMode(),frik_pose_probe::FinalSkinTestMode());
            frik_pose_probe::SetDialogueBody178r9(dialogueBody47c1||workstationBody178r13||heldPropBody43);
            frik_pose_probe::PublishDialogueHandPresenceH24(dialogueState59c10r2,dialogueBody47c1);
            CameraHook::SetDialogueEmbodiment59c10r2(dialogueBody47c1,GetTickCount64());
            npc_lookat_hmd::PublishDialogueOwner63(dialogueState59c10r2,dialogueBody47c1);
            lively_npc_d73::Publish(reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity()),
                dialogueState59c10r2.membership.sceneD52,
                vrik_gameplay_allowed&&!mis_menu_after_frame&&!ui89_policy.BlocksWorldInteraction()&&!diagnostics::lab::NativeReference(),
                lively_npc_d73::DialogueScene(dialogueState59c10r2,GetTickCount64())&&
                    !safe_ui_fit&&!matrix_active&&!mis_menu_after_frame&&!cutscene_flat_ui,
                GetTickCount64(),&dialogueState59c10r2);
            if(dialogue_active){ // R59.16C10R13: say which gate holds the embodiment back, once per change and every 2 s
                static std::uint64_t lastGate59c13=0;static int lastKey59c13=-1;const auto nowG=GetTickCount64();
                const int key=(dialogueBody47c1?1:0)|(vrik_in_dialogue_?2:0)|(safe_ui_fit?4:0)|(matrix_active?8:0)|(mis_menu_after_frame?16:0)|(activity_input163::witness175.Active()?32:0)|(int(ui89_policy.owner)<<6)|(ui89_policy.uncertain?4096:0);
                if(key!=lastKey59c13||nowG-lastGate59c13>=2000){lastKey59c13=key;lastGate59c13=nowG;
                    char m[300]{};std::snprintf(m,sizeof(m),"R59.16C10R13 DIALOGUE_GATE allowed=%d vrikInDialogue=%d vrikSelected=%d cameraMode=%d embodiedOwner=%d theater=%d matrix=%d vrMenu=%d nativeRef=%d activity=%d uiOwner=%d uiUncertain=%u",
                        dialogueBody47c1?1:0,vrik_in_dialogue_?1:0,visual_selection59c7f::Selected(frik_pose_probe::CalibrationReady(),frik_pose_probe::CombatCensusMode(),frik_pose_probe::FinalSkinTestMode())?1:0,dialogue_mode_now,
                        dialogue_state59c10r2::embodiedOwner(dialogueState59c10r2,nowG)?1:0,safe_ui_fit?1:0,matrix_active?1:0,mis_menu_after_frame?1:0,diagnostics::lab::NativeReference()?1:0,activity_input163::witness175.Active()?1:0,int(ui89_policy.owner),unsigned(ui89_policy.uncertain));
                    diagnostic_trace::Write(m);}}
            static int lastDialoguePose47c1=-1;
            const int dialoguePose47c1=dialogue_active?int(dialogueBody47c1):-1;
            if(dialoguePose47c1!=lastDialoguePose47c1){
                char m[288]{};std::snprintf(m,sizeof(m),"R47C1 DIALOGUE_POSE active=%d visual=%d camera=%d uiOwner=%d cutsceneFlag=%d agency=%d gameplay=%d protectedBody=NATIVE",
                    dialogue_active,dialogueBody47c1,dialogue_mode_now,int(ui89_policy.owner),cutscene_flat_ui,gs.agency_count,vrik_gameplay_allowed);
                diagnostic_trace::Write(m);lastDialoguePose47c1=dialoguePose47c1;
            }
            frik_pose_probe::ObserveR3Combat101(rc.thumbstick_down);
            tracking59c6::Stamp source59c6{};(void)xr_manager_->CopyInputSampleStamp59c6(source59c6);
            frik_pose_probe::PublishTrackingSnapshot(hp,xr_head_valid,lp,lc.active,rp,rc.active,lc.grip,rc.grip,gameplay_trigger_l,gameplay_trigger_r,vrik_gameplay_allowed,
                source59c6.tick,source59c6.ns,source59c6.epoch,source59c6.locateTime,dialogueBody47c1,lc.trigger,rc.trigger);
            g_stallCp178r30[3]=std::uint64_t(GetTickCount64()); // R30 checkpoint: tracking published
            static std::uint64_t vrik_track_log_ms=0;
            if((frik_pose_probe::FinalSkinTestMode()>0 || frik_pose_probe::CombatCensusMode()>0) && now_ms-vrik_track_log_ms>=1000ull) {
                vrik_track_log_ms=now_ms;
                char tm[384]{}; std::snprintf(tm,sizeof(tm),"V64.51-VRIK XR-LIVE head=%d left=%d right=%d allowed=%d H=(%.3f,%.3f,%.3f) L=(%.3f,%.3f,%.3f) R=(%.3f,%.3f,%.3f)",xr_head_valid?1:0,lc.active?1:0,rc.active?1:0,vrik_gameplay_allowed?1:0,hp.px,hp.py,hp.pz,lp.px,lp.py,lp.pz,rp.px,rp.py,rp.pz); diagnostic_trace::Write(tm);
            }
        }

#define KCD2VR_M2_HOST_SLOT 5018 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=5018
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
        // V66.107: feed the floor-relative eye height to the VRIK head reference.
        if(xr_manager_){float eye=0.f;const bool ok=xr_manager_->GetEyeHeightAboveFloor(eye);frik_pose_probe::SetEyeHeightAboveFloor(ok,eye);}

        // V66.111 PHYSICAL HAND USE. F5=2 remains the explicit live diagnostic rung,
        // but F5=0 now enables the same path in ordinary calibrated immersive gameplay.
        // F3 combat labs, UI ownership and occupied/unknown hands fail closed.
        // This does not call StartInteractiveActionByName.
        // A neutral free-hand grip edge scopes KCD2's existing interaction ray to that hand for
        // 350 ms and emits one ordinary E/Use pulse. Native target selection, reach filters,
        // lock/theft/quest checks and the resulting action remain KCD2-owned.
        touch_bindings_.SetNativeGamepadActions(native_xi_full); // select THIS frame's transport before F5 requests
        // D85 dice: right hand world ray for "point to choose" (same transform as the
        // alchemy pointer fallback below). Only while ON and a native dice game runs.
        if(dice_hands_d85::enabled.load(std::memory_order_acquire)&&xr_manager_&&activity_input163::witness175.dice.load(std::memory_order_acquire)){
            dice_hands_d85::Ray rayD85{};XrManager::RenderFrameState rfD85{};CameraRotationSample camD85{};float cxD85=0,cyD85=0,czD85=0;
            static std::uint64_t frameD85=0,frameAtD85=0;
            const auto& cD85=xr_manager_->RightController();
            if(cD85.active&&xr_manager_->CopyActiveRenderFrameState(rfD85)&&rfD85.stereo_views_valid&&rfD85.head_pose_valid){
                if(rfD85.frame_index!=frameD85){frameD85=rfD85.frame_index;frameAtD85=ladder_now_ms;}
                if(frameAtD85&&ladder_now_ms-frameAtD85<=60&&CameraHook::GetLatestFrustumRotationSample(camD85)&&CameraHook::GetLatestVisualPositionWorld(cxD85,cyD85,czD85,100)){
                    const auto& e0=rfD85.eye_pose[0];const auto& e1=rfD85.eye_pose[1];
                    const float eyeLD85[3]{e0.position.x,e0.position.y,e0.position.z},eyeRD85[3]{e1.position.x,e1.position.y,e1.position.z};
                    const float hq[4]{e0.orientation.x,e0.orientation.y,e0.orientation.z,e0.orientation.w};
                    const float cp[3]{cD85.pose.pos_x,cD85.pose.pos_y,cD85.pose.pos_z},cq[4]{cD85.pose.qx,cD85.pose.qy,cD85.pose.qz,cD85.pose.qw};
                    const float wp[3]{cxD85,cyD85,czD85};
                    (void)dice_hands_d85::buildRay(eyeLD85,eyeRD85,hq,cp,cq,camD85.m,wp,frameAtD85,rayD85);
                }
            }
            (void)dice_hands_d85::ray.publish(rayD85);
        }
        bool handUseAllowed59c10=false;
        {
            kcd2vr::hand_only_door_h40::Diagnostic(ladder_now_ms);
            static physical_interaction106::State f5Physical{};
            static held_interaction114::State heldUse114{};
            static pickup_use19c3::State pickupUse19c3{};
        static door_use46::Controller doorUse46;
            static hand_point123r1::State point123{};
            static alchemy_input19c4::PalmLock27 alchemyPalmLock27;
            static bool heldSent114=false;
            static bool f5OwnedRay=false;
            const unsigned f3Mode=unsigned(frik_pose_probe::HolsterCombatDiagMode());
            const unsigned f5Mode=unsigned(frik_pose_probe::MiscInteractionDiagMode());
            const bool uiBlocked=matrix_active||safe_ui_fit||(vr_menu_&&vr_menu_->IsVisible())||ui89_policy.BlocksWorldInteraction()||activity163.nativeControls||dialogue_active;
            action_time_owner59c11::SetContextAvailable(!uiBlocked&&frik_pose_probe::CalibrationReady()); // V66.178R9: no hand-use in dialogue
            // V66.116: the backend being enabled does not occupy every hand.
            // The existing reservation reader returns both bits on unknown/stale
            // state, fist ownership, bow, support acquisition or protected context.
            const unsigned reserved114=frik_pose_probe::PhysicalHandReservations114();
            const bool physicalCombatBusy=(reserved114&3u)==3u;
            const auto interactionStatus114=interaction_alignment::GetStatus();
            const bool comparison114=interaction_ownership111::Comparison114(f3Mode,f5Mode,uiBlocked,frik_pose_probe::CalibrationReady())&&
                retail_build112::Ready(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")))&&
                interactionStatus114.installed&&interactionStatus114.enabled;
            const bool live111=comparison114||interaction_ownership111::Live(f3Mode,f5Mode,uiBlocked,frik_pose_probe::CalibrationReady(),physicalCombatBusy);
            touch_bindings_.SetGripWeaponComparison114((f3Mode==3&&frik_pose_probe::WeaponControlMode()==6)||(frik_pose_probe::WeaponControlMode()==7&&f3Mode!=1));
            physical_interaction106::Input pi{};pi.enabled=live111&&xr_manager_&&interactionStatus114.enabled;pi.menu=uiBlocked;pi.now_ms=ladder_now_ms;
            int itemState116[2]{-1,-1};unsigned gripMask116=0;
            if(xr_manager_){
                const auto& r=xr_manager_->RightController();const auto& l=xr_manager_->LeftController();
                const int rs=ensure_vrik_lab()?frik_pose_probe::ExactHandItemState(0):-1;
                const int ls=ensure_vrik_lab()?frik_pose_probe::ExactHandItemState(1):-1;
                const unsigned gripMask=ui_menu_input::WorldGripBlockedMask();
                itemState116[0]=rs;itemState116[1]=ls;gripMask116=gripMask;
                const bool rn=std::fabs(r.thumb_x)<0.28f&&std::fabs(r.thumb_y)<0.28f;
                const bool ln=std::fabs(l.thumb_x)<0.28f&&std::fabs(l.thumb_y)<0.28f;
                pi.hand[0]={interaction_ownership111::HandEligible(r.active,rs,true,(gripMask&2u)!=0),rs==0,rn,r.grip,false};
                pi.hand[1]={interaction_ownership111::HandEligible(l.active,ls,true,(gripMask&1u)!=0),ls==0,ln,l.grip,false};
                if(reserved114&1u)pi.hand[0].tracked=false;
                if(reserved114&2u)pi.hand[1].tracked=false;
            }
// A timed-out corpse nomination may return only its original unspent press.
std::uint64_t corpseReturnedPressD78[2]{};
// CG1_CORPSE_INPUT_BEGIN
// BEFORE the ordinary pulse/held policies see LEFT Grip. This filters only the
// physical gesture input; the real controller A button is not modified.
static kcd2vr::corpse_grip_cg1::Gate corpseGripGateCg1;
{
 const auto nowCg1=std::uint64_t(GetTickCount64());
 kcd2vr::corpse_grip_cg1::Candidate candidateCg1{};bool inputCurrentCg1=false;
 const bool qualifiedCg1=frik_pose_probe::ReadCorpseGrabCandidateCg1(candidateCg1,nowCg1,inputCurrentCg1);
 tracking59c6::Stamp squeezeStampCg1{};
 const bool squeezeCurrentCg1=xr_manager_&&xr_manager_->LeftController().grip_valid&&
  xr_manager_->CopyInputSampleStamp59c6(squeezeStampCg1)&&
  kcd2vr::corpse_grip_cg1::fresh(squeezeStampCg1.tick,nowCg1)&&
  squeezeStampCg1.epoch==tracking59c6::epoch.load(std::memory_order_acquire);
 const auto outCg1=kcd2vr::corpse_grip_cg1::updateShared<kcd2vr::hand_only_door_h40::Admission>(corpseGripGateCg1,1,nowCg1,inputCurrentCg1&&squeezeCurrentCg1,
  qualifiedCg1&&!pi.menu&&pi.hand[1].tracked&&pi.hand[1].empty,
  xr_manager_?xr_manager_->LeftController().grip:0.f,candidateCg1,kcd2vr::corpse_grip_cg1::request,pi.enabled&&pi.hand[1].neutral);
 if(outCg1.returnedPressD78&&pi.enabled&&!pi.menu&&pi.hand[1].tracked&&pi.hand[1].empty&&pi.hand[1].neutral){
  corpseReturnedPressD78[1]=outCg1.returnedPressD78;
  f5Physical.hand[1].down=false;f5Physical.hand[1].rearm=true;heldUse114.neutral[1]=true;
 }
 if(outCg1.suppressLeftUse)pi.hand[1].tracked=false;
}
// CG1_CORPSE_INPUT_END
// W5_CG1_RIGHT_BEGIN: the same consumed-squeeze gate for the RIGHT hand (Miki: drag corpses with our HANDS). kept
// outside the CG1 markers so the extracted LEFT worker fixture stays byte identical.
static kcd2vr::corpse_grip_cg1::Gate corpseGripGateRightW5;
{
 const auto nowR=std::uint64_t(GetTickCount64());
 kcd2vr::corpse_grip_cg1::Candidate candidateR{};bool inputCurrentR=false;
 const bool qualifiedR=frik_pose_probe::ReadCorpseGrabCandidateSideCg1(0,candidateR,nowR,inputCurrentR);
 tracking59c6::Stamp stampR{};
 const bool squeezeCurrentR=xr_manager_&&xr_manager_->RightController().grip_valid&&
  xr_manager_->CopyInputSampleStamp59c6(stampR)&&kcd2vr::corpse_grip_cg1::fresh(stampR.tick,nowR)&&
  stampR.epoch==tracking59c6::epoch.load(std::memory_order_acquire);
 const auto outR=kcd2vr::corpse_grip_cg1::updateShared<kcd2vr::hand_only_door_h40::Admission>(corpseGripGateRightW5,0,nowR,inputCurrentR&&squeezeCurrentR,
  qualifiedR&&!pi.menu&&pi.hand[0].tracked&&pi.hand[0].empty,
  xr_manager_?xr_manager_->RightController().grip:0.f,candidateR,kcd2vr::corpse_grip_cg1::handRequests[0],pi.enabled&&pi.hand[0].neutral);
 if(outR.returnedPressD78&&pi.enabled&&!pi.menu&&pi.hand[0].tracked&&pi.hand[0].empty&&pi.hand[0].neutral){
  corpseReturnedPressD78[0]=outR.returnedPressD78;
  f5Physical.hand[0].down=false;f5Physical.hand[0].rearm=true;heldUse114.neutral[0]=true;
 }
 if(outR.suppressLeftUse)pi.hand[0].tracked=false;
}
// W5_CG1_RIGHT_END
            static hands59c7f::Arbitration handsOwner59c7f;
            interaction_alignment::HandCandidate candidate59c7f{};
            (void)interaction_alignment::LastHandCandidate(candidate59c7f);
            #include "subsystems/interactions/misc_gestures_d59_input.inl"
            #include "subsystems/doors/hand_only_h40_input.inl"
            // H40 already consumed the raw edge during the reservation. Do not
            // rearm its door query; preserve the original time for ordinary Use.
            for(int side=0;side<2;++side)if(!fallbackPressH40[side]&&corpseReturnedPressD78[side])
                fallbackPressH40[side]=corpseReturnedPressD78[side];
            hands59c7f::Hand handsInput59c7f[2]{};
            for(int side=0;side<2;++side)if(xr_manager_){const auto& h=pi.hand[side];const auto& c=side?xr_manager_->LeftController():xr_manager_->RightController();
                handsInput59c7f[side]={h.tracked&&c.active,h.empty,h.grip,c.trigger};}
            const auto grabMask59c7f=handsOwner59c7f.update(hands59c7f::active()&&hands59c7f::pickupReady.load()&&!uiBlocked,
                hands59c7f::epoch.load(),handNowH40,handsInput59c7f,
                {candidate59c7f.object,candidate59c7f.owner,candidate59c7f.tick,candidate59c7f.distance});
            hands59c7f::grabMask.store(grabMask59c7f,std::memory_order_release);
            hands59c7f::sampleTick.store(handNowH40,std::memory_order_release);
            // Keep passive object hover, but a fist away from a qualified nearby
            // object must never issue Use. The native selector still validates it.
            // V66.178R7: hover age feeds the Use settle time (see physical_interaction.h).
            static std::uint64_t hoverSince178r7[2]{};
            auto pulseInput114=pi;pulseInput114.enabled=pi.enabled&&!comparison114;
            for(int side=0;side<2;++side)pulseInput114.hover_age_ms[side]=(hoverSince178r7[side]&&ladder_now_ms>=hoverSince178r7[side])?ladder_now_ms-hoverSince178r7[side]:0;
            auto heldInput114=pi;heldInput114.enabled=pi.enabled&&(comparison114||f5Mode==0); // C63 ordinary Pickable Grip shares the qualified hold timer
            auto po=physical_interaction106::Step(f5Physical,pulseInput114);
            const auto held114=held_interaction114::update(heldUse114,heldInput114);
            if(held114.owner>=0&&fallbackPressH40[held114.owner])heldUse114.pressed=fallbackPressH40[held114.owner];
            if(comparison114){po.ray_active=held114.ray;po.ray_owner=held114.owner;po.simultaneous_reject=held114.simultaneous;}
            // V66.178R7: hand hover (the game's own prompt following the empty VRIK hand) runs in
            // every VRIK user mode. Since V66.175 all user modes set the interaction rung to 2 while
            // this required 0, so hover - and with it every hand-aimed Use - was unreachable
            // (2026-10-08 log: 206 HAND_USE_GATE samples, rayRequested=0 throughout).
            const bool ordinaryPoint123=(f5Mode==0||door_use46::inputMode(f5Mode))&&!diagnostics::lab::NativeReference()&&frik_pose_probe::CombatCensusMode()>=2;
            auto buildPalm59c9=[&](int side,interaction112::Ray& ray)->bool{
                ray={};ray.owner=side;ray.hand=true;ray.tick=ladder_now_ms;ray.limit=interaction57c1::HandReach;ray.radius=interaction57c1::HandRadius;
                if(frik_pose_probe::ReadInteractionPalm59c9(side,ray.ox,ray.oy,ray.oz,ray.x,ray.y,ray.z,ray.tick,&ray.sourceEpoch,&ray.sourcePlayer))return true;
                if(!alchemyPointer59c2)return false; // no uncommitted ordinary-gameplay fallback
                if(!xr_manager_||side<0||side>1)return false;
                XrManager::RenderFrameState rf{};if(!xr_manager_->CopyActiveRenderFrameState(rf)||!rf.stereo_views_valid)return false;
                static std::uint64_t rayFrame123=0,rayFrameAt123=0;const auto rayNow123=std::uint64_t(GetTickCount64());
                if(rf.frame_index!=rayFrame123){rayFrame123=rf.frame_index;rayFrameAt123=rayNow123;}
                if(!rf.head_pose_valid||!rayFrameAt123||rayNow123<rayFrameAt123||rayNow123-rayFrameAt123>60)return false;
                auto qrot=[](const XrQuaternionf& q,const XrVector3f& v)->XrVector3f{const float tx=2.0f*(q.y*v.z-q.z*v.y),ty=2.0f*(q.z*v.x-q.x*v.z),tz=2.0f*(q.x*v.y-q.y*v.x);return {v.x+q.w*tx+(q.y*tz-q.z*ty),v.y+q.w*ty+(q.z*tx-q.x*tz),v.z+q.w*tz+(q.x*ty-q.y*tx)};};
                auto qconj=[](XrQuaternionf q)->XrQuaternionf{return {-q.x,-q.y,-q.z,q.w};};
                const auto& c=side?xr_manager_->LeftController():xr_manager_->RightController();if(!c.active)return false;
                const XrVector3f headPos{0.5f*(rf.eye_pose[0].position.x+rf.eye_pose[1].position.x),0.5f*(rf.eye_pose[0].position.y+rf.eye_pose[1].position.y),0.5f*(rf.eye_pose[0].position.z+rf.eye_pose[1].position.z)};
                const XrQuaternionf headQ=rf.eye_pose[0].orientation;
                CameraRotationSample cam{};float wx=0,wy=0,wz=0;if(!CameraHook::GetLatestFrustumRotationSample(cam)||!CameraHook::GetLatestVisualPositionWorld(wx,wy,wz,100))return false;
                const XrVector3f relRef{c.pose.pos_x-headPos.x,c.pose.pos_y-headPos.y,c.pose.pos_z-headPos.z};const XrVector3f relHead=qrot(qconj(headQ),relRef);
                const XrQuaternionf aimQ{c.pose.qx,c.pose.qy,c.pose.qz,c.pose.qw};const XrVector3f aimRef=qrot(aimQ,{0,0,-1});const XrVector3f aimHead=qrot(qconj(headQ),aimRef);
                const float rx=cam.m[0],ry=cam.m[3],rz=cam.m[6],fx=cam.m[1],fy=cam.m[4],fz=cam.m[7],ux=cam.m[2],uy=cam.m[5],uz=cam.m[8];
                float ox=wx+rx*relHead.x+ux*relHead.y+fx*(-relHead.z),oy=wy+ry*relHead.x+uy*relHead.y+fy*(-relHead.z),oz=wz+rz*relHead.x+uz*relHead.y+fz*(-relHead.z);
                // Native workstation poses may pause the VRIK writer. Use the
                // tracked grip (palm) frame here, never the remote aiming pointer.
                float dx=rx*aimHead.x+ux*aimHead.y+fx*(-aimHead.z),dy=ry*aimHead.x+uy*aimHead.y+fy*(-aimHead.z),dz=rz*aimHead.x+uz*aimHead.y+fz*(-aimHead.z);const float dl=std::sqrt(dx*dx+dy*dy+dz*dz);if(!std::isfinite(dl)||dl<1e-4f)return false;
                ray.ox=ox;ray.oy=oy;ray.oz=oz;ray.x=dx/dl;ray.y=dy/dl;ray.z=dz/dl;ray.tick=rayFrameAt123;return true;
            };

            {float hy=0.f,hp=0.f;if(CameraHook::GetHeadAimLocalAngles(hy,hp))frik_pose_probe::SetHeadPitch59c14(hp*57.2957795f);} // R59.16C10R14: stowed bow hide reads the HMD pitch
            const bool pointerEnabled32=(ordinaryPoint123&&!uiBlocked||alchemyPointer59c2)&&interactionStatus114.enabled&&frik_pose_probe::CalibrationReady()&&
                retail_build112::Ready(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")));
            #include "vrik/interaction_point32.inl"
            {   // R137 (vrik/view_select_r137.h): index-finger point = grip closed, index lifted off the trigger, the
                // empty hand out in front of your head and pointing away from you, 120 ms. its finger ray (finger tip
                // along the hand, the same committed VRIK palm frame as the palm ray) selects objects instead of the
                // headset ray while you point. switch: VrInteractionEyeRayR121. no latch: decided every loop.
                static kcd2vr::view_select_r137::State point137{};
                namespace vs137=kcd2vr::view_select_r137;
                vs137::Hand hands137[2]{};vs137::Head head137{};
                visual_ray_d75::Sample eye137{};
                const bool on137=kcd2vr::eye_ray_r121::enabled.load(std::memory_order_acquire)&&pointerEnabled32&&!uiBlocked&&
                    !alchemyPointer59c2&&pi.enabled&&!pi.menu&&xr_manager_&&CameraHook::GetLatestVisualRayD75(eye137,80);
                if(on137){
                    head137.valid=true;
                    for(int i=0;i<3;++i){head137.eye[i]=eye137.origin[i];head137.fwd[i]=eye137.forward[i];}
                    for(int side=0;side<2;++side){
                        const auto& c=side?xr_manager_->LeftController():xr_manager_->RightController();
                        auto& h=hands137[side];
                        h.empty=pi.hand[side].empty&&itemState116[side]==0;h.grip=c.grip;h.trigger=c.trigger;
                        h.touchValid=c.trigger_touch_valid;h.touch=c.trigger_touch;
                        interaction112::Ray r{};
                        // the ray is built only for a hand that could point (cheap committed-packet read)
                        h.valid=c.active&&h.empty&&c.grip>=.60f&&c.trigger<.10f&&c.trigger_touch_valid&&!c.trigger_touch&&
                            !(reserved114&(1u<<side))&&buildPalm59c9(side,r);
                        if(h.valid){h.hand[0]=r.ox;h.hand[1]=r.oy;h.hand[2]=r.oz;h.dir[0]=r.x;h.dir[1]=r.y;h.dir[2]=r.z;}
                    }
                }
                const bool posed137[2]{on137&&vs137::pose(hands137[0],head137),on137&&vs137::pose(hands137[1],head137)};
                const int pointSide137=vs137::update(point137,posed137,ladder_now_ms);
                float po137[3]{},pd137[3]{};
                const bool ray137=pointSide137>=0&&vs137::fingerRay(hands137[pointSide137],po137,pd137);
                interaction_alignment::SetPointRayR137(ray137,po137[0],po137[1],po137[2],pd137[0],pd137[1],pd137[2],ray137?pointSide137:-1);
                static int pointLogged137=-2;
                if((ray137?pointSide137:-1)!=pointLogged137){
                    pointLogged137=ray137?pointSide137:-1;
                    char m137[160]{};std::snprintf(m137,sizeof(m137),"R137 POINT_RAY hand=%s (index-finger point selects objects; none = headset view)",
                        pointLogged137==0?"RIGHT":pointLogged137==1?"LEFT":"NONE");diagnostic_trace::Write(m137);
                }
            }
            interaction_alignment::SetPalmRays59c9(palms59c9);
            bool passiveHover156=false;
            static int hoverOwner57c1=-1;
            if(pointer123.enabled&&!alchemyPointer59c2&&!po.ray_active){
                bool eligible[2]{};float grips[2]{};
                for(int side=0;side<2;++side){
                    eligible[side]=palms59c9.eligible[side];
                    grips[side]=pi.hand[side].grip;
                }
                // Grip travel never removes an otherwise valid hover ray. Keep
                // the existing hand until it becomes invalid or the other hand requests it.
                if(candidate59c7f.object&&candidate59c7f.owner>=0&&candidate59c7f.owner<2&&
                   ladder_now_ms>=candidate59c7f.tick&&ladder_now_ms-candidate59c7f.tick<=80&&eligible[candidate59c7f.owner])
                    hoverOwner57c1=candidate59c7f.owner;
                hoverOwner57c1=palms59c9.locked>=0?palms59c9.locked:interaction57c1::HoverOwner(hoverOwner57c1,eligible,grips);
                if(hoverOwner57c1>=0){po.ray_active=true;po.ray_owner=hoverOwner57c1;po.fire_use=false;passiveHover156=true;}
            }else hoverOwner57c1=po.ray_active?po.ray_owner:-1;
            if(pointResult123.simultaneous)po.simultaneous_reject=true;
            // V66.178R14 natural hands: a usable candidate under the hand (the on-screen check we force for the
            // hand cone) opens the fingers a little and gives one short haptic tick per new target.
            {
                static std::uint32_t hoverEnt26[2]{};static std::uint64_t hoverLost26[2]{},hoverTick26[2]{};
                interaction_alignment::HandCandidate cand{};
                const bool recent=interaction_alignment::LastHandCandidate(cand)&&ladder_now_ms>=cand.tick&&ladder_now_ms-cand.tick<=120&&cand.distance<=.60f;
                for(int side=0;side<2;++side){
                    const bool hovering=po.ray_active&&po.ray_owner==side&&recent&&cand.owner==side&&pi.hand[side].empty;
                    // C32: left-palm viewing must not curl/open fingers or buzz
                    // from a nearby interaction candidate. Selection/Grip rays
                    // remain available to BOTH hands; only incidental feedback
                    // is disabled on the left, as requested.
                    const bool feedback32=side==0&&hovering;
                    frik_pose_probe::SetHoverAffordance178r14(side,feedback32);
                    const std::uint64_t key=hovering?(std::uint64_t(std::llround(cand.x*20.f))*73856093ull)^(std::uint64_t(std::llround(cand.y*20.f))*19349663ull)^(std::uint64_t(std::llround(cand.z*20.f))*83492791ull)|1ull:0ull;
                    // V66.178R20: one tick when a target comes under the hand (entry), at most every 700 ms - not on
                    // every 5 cm the aim point drifts (the 2026-10-09 run buzzed continuously).
                    // C22b (Miki: constant left-hand buzz): the hover candidate flickers ~4x/s (C21 log: a sword on the floor under
                    // the empty left hand); every re-acquire counted as a new entry. One tick per NEW entity, forgotten only after
                    // 800 ms with no candidate (exit hysteresis), never more than one tick per 1.5 s.
                    const std::uint32_t ent=feedback32?cand.entity:0u;
                    if(ent){hoverLost26[side]=0;
                        if(ent!=hoverEnt26[side]){hoverEnt26[side]=ent;
                            if(ladder_now_ms-hoverTick26[side]>=1500){hoverTick26[side]=ladder_now_ms;(void)frik_pose_probe::HoverHaptic178r14(side,key);}}}
                    else if(hoverEnt26[side]){if(!hoverLost26[side])hoverLost26[side]=ladder_now_ms;
                        else if(ladder_now_ms-hoverLost26[side]>=800){hoverEnt26[side]=0;hoverLost26[side]=0;}}
                }
                // R59.16C10R10 INTERACTOR_CANDIDATE: the first question of every "doors / pickups / dog" report - what did KCD2's own
                // interactor get offered under the hand (script class, distance, hand, grips) and was our Use transport armed. One line
                // per candidate change (entity id), at most one per 250 ms and 400 per session; no native read beyond the published record.
                {static std::uint32_t lastEntity59c10r10=~0u;static std::uint64_t lastAt59c10r10=0;static unsigned lines59c10r10=0;
                 const std::uint32_t ent=recent?cand.entity:0u;
                 if(ent!=lastEntity59c10r10&&ladder_now_ms-lastAt59c10r10>=250&&lines59c10r10<400){
                    lastEntity59c10r10=ent;lastAt59c10r10=ladder_now_ms;++lines59c10r10;
                    char m[320]{};std::snprintf(m,sizeof(m),"R59.16C10R10 INTERACTOR_CANDIDATE entity=%u class=%s side=%d distance=%.2f object=%d gripR=%.2f gripL=%.2f emptyR=%d emptyL=%d rayActive=%d rayOwner=%d uiBlocked=%d (native interactor target under the hand; Use = grip gesture, bond/R3 = companion_bond, Y = request_dog/use_horse)",
                        ent,ent&&cand.cls[0]?cand.cls:"-",cand.owner,double(cand.distance),cand.object?1:0,double(pi.hand[0].grip),double(pi.hand[1].grip),pi.hand[0].empty?1:0,pi.hand[1].empty?1:0,po.ray_active?1:0,po.ray_owner,uiBlocked?1:0);
                    diagnostic_trace::Write(m);}}
            }

            auto publishF5Ray=[&](int side,bool passiveHover,bool=false)->bool{
                if(side<0||side>1||!palms59c9.eligible[side])return false;
                const auto& ray=palms59c9.ray[side];
                const float ox=ray.ox,oy=ray.oy,oz=ray.oz,dx=ray.x,dy=ray.y,dz=ray.z,reach158=interaction57c1::HandReach;
                interaction_alignment::SetHandRayWorld(true,ox,oy,oz,dx,dy,dz,reach158,interaction57c1::HandRadius,side);
                const bool active=interaction_alignment::HandRayActive();
                frik_pose_probe::SetInteractionHandContext158(active?side:-1,passiveHover,ox,oy,oz,reach158);
                return active;
            };
            bool heldNow114=false,useSent116=false;
            // Both palms participate in native tool selection. The native
            // alchemy action, not handedness guessed from table coordinates,
            // decides which fingers/tool/superaction it owns.
            static int alchemyOwner59c2=-1,alchemyLoggedOwner59c3=-1;static bool alchemyLogged59c2=false;
            bool alchemyRay59c2=false;
            if(alchemyPointer59c2){
                if(palms59c9.locked>=0)alchemyOwner59c2=palms59c9.locked;
                else if(candidate59c7f.object&&candidate59c7f.owner>=0&&candidate59c7f.owner<2&&
                    ladder_now_ms>=candidate59c7f.tick&&ladder_now_ms-candidate59c7f.tick<=80&&palms59c9.eligible[candidate59c7f.owner])alchemyOwner59c2=candidate59c7f.owner;
                if(alchemyOwner59c2<0||!palms59c9.eligible[alchemyOwner59c2])alchemyOwner59c2=palms59c9.eligible[0]?0:(palms59c9.eligible[1]?1:-1);
                if(alchemyOwner59c2>=0)alchemyRay59c2=publishF5Ray(alchemyOwner59c2,true);
            }else alchemyOwner59c2=-1;
            if(alchemyRay59c2!=alchemyLogged59c2||(alchemyRay59c2&&alchemyOwner59c2!=alchemyLoggedOwner59c3)){
                alchemyLogged59c2=alchemyRay59c2;alchemyLoggedOwner59c3=alchemyRay59c2?alchemyOwner59c2:-1;char m[200]{};
                std::snprintf(m,sizeof(m),"R59.16C2 ALCHEMY_HAND_RAY active=%d hand=%s source=PALM_PAIR_OR_GRIP_FALLBACK target=NATIVE_INTERACTOR use=GRIP_GESTURE",
                    alchemyRay59c2?1:0,alchemyOwner59c2==1?"LEFT":alchemyOwner59c2==0?"RIGHT":"NONE");diagnostic_trace::Write(m);}
            const bool rayPublished116=alchemyRay59c2||(po.ray_active&&publishF5Ray(po.ray_owner,passiveHover156));
            handUseAllowed59c10=pi.enabled&&!pi.menu&&rayPublished116&&!alchemyPointer59c2;
            for(int side=0;side<2;++side){
                const bool hovering=rayPublished116&&po.ray_owner==side;
                if(hovering&&!hoverSince178r7[side])hoverSince178r7[side]=ladder_now_ms;
                if(!hovering)hoverSince178r7[side]=0;
            }
            if(hands59c7f::active()&&(!grabMask59c7f||po.ray_owner<0||!(grabMask59c7f&(1u<<po.ray_owner))))po.fire_use=false;
#include "vrik/pickup_use_producer19c3.inl"
            physical_interaction106::CompleteUse116(f5Physical,po,useSent116);
            static int lastPoint123=-2;const int pointOwner123=ordinaryPoint123&&rayPublished116?po.ray_owner:-1;
            if(pointOwner123!=lastPoint123){char m[224]{};std::snprintf(m,sizeof(m),"V66.156-HAND-HOVER owner=%d visibleVrikEndpoint=1 passive=%d direction=COMMITTED_PALM automaticUse=0 reach=%s",pointOwner123,passiveHover156?1:0,"2.00m");diagnostic_trace::Write(m);lastPoint123=pointOwner123;}
            if(heldSent114!=heldNow114){
                heldSent114=heldNow114;
                // Held request alone is not an acknowledged native hand animation.
                char heldTrace[256]{};std::snprintf(heldTrace,sizeof(heldTrace),
                    "C10 HAND_HOLD_INPUT held=%d hand=%d reservedMask=%u transport=%s targetAction=NATIVE_CONTEXT acceptance=SEE_NATIVE_ACTION_RECEIPT",
                    heldNow114?1:0,po.ray_owner,reserved114,native_xi_full?"XI_A":"KEY_E");diagnostic_trace::Write(heldTrace);
            }
            if(po.simultaneous_reject)diagnostic_trace::Write("V66.111-HAND-USE simultaneous grip edges rejected; release and retry one hand");
            if(diagnostics::capture168::active(diagnostics::capture168::Domain::Hands,ladder_now_ms)){
                static std::uint64_t lastHands59c7f=0;
                if(ladder_now_ms<lastHands59c7f||ladder_now_ms-lastHands59c7f>=500){lastHands59c7f=ladder_now_ms;char m[384]{};
                    std::snprintf(m,sizeof(m),"C7F F10_HANDS pose=%d weapon=%d contextBlocked=%d itemRL=%d/%d reserved=%u gripRL=%.2f/%.2f grabMask=%u candidateObject=%d candidateSide=%d candidateDistance=%.3f candidateAge=%llu",
                        frik_pose_probe::CombatCensusMode(),frik_pose_probe::WeaponControlMode(),uiBlocked,itemState116[0],itemState116[1],reserved114,
                        double(pi.hand[0].grip),double(pi.hand[1].grip),grabMask59c7f,candidate59c7f.object,candidate59c7f.owner,double(candidate59c7f.distance),
                        (unsigned long long)(candidate59c7f.tick&&ladder_now_ms>=candidate59c7f.tick?ladder_now_ms-candidate59c7f.tick:UINT64_MAX));diagnostic_trace::Write(m);}
            }
            // One status per second in the explicit F5 labs; no native polling.
            static std::uint64_t lastGate116=0;
            if(f5Mode>=2&&(ladder_now_ms<lastGate116||ladder_now_ms-lastGate116>=1000)){
                lastGate116=ladder_now_ms;char gate[640]{};
                std::snprintf(gate,sizeof(gate),"V66.116 HAND_USE_GATE F3=%u F5=%u F12=%d enabled=%d comparison=%d uiBlocked=%d reservedRL=%u itemRL=%d/%d eligibleRL=%d/%d gripRL=%.3f/%.3f uiGripMask=%u rayRequested=%d rayPublished=%d owner=%d hold=%d targetAcceptance=UNOBSERVED",
                    f3Mode,f5Mode,frik_pose_probe::WeaponControlMode(),pi.enabled,comparison114,uiBlocked,reserved114,
                    itemState116[0],itemState116[1],pi.hand[0].tracked,pi.hand[1].tracked,double(pi.hand[0].grip),double(pi.hand[1].grip),gripMask116,
                    po.ray_active,rayPublished116,po.ray_owner,heldNow114);diagnostic_trace::Write(gate);
            }
        }

#define KCD2VR_M2_HOST_SLOT 5019 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=5019
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
        // V64.33 camera focus: use OpenXR only as the source. In F4 WRITE,
        // convert absolute HMD yaw/pitch into relative angular deltas and feed
        // those deltas through KCD2's own CameraInputDispatch. The injector
        // runs on the engine's input-dispatch thread and calls the original
        // dispatcher with cloned native mouse-look SInputEvents.
        {
            const auto& rrAim=xr_manager_->RightController();
            const bool protectedHeadLookBlock = inventory_flat_ui || map_flat_ui || pause_flat_ui ||
                                                 safe_ui_fit || f2_ui_menu_active || mis_menu_after_frame ||
                                                 dialogue_active || cutscene_flat_ui || ui89_policy.BlocksWorldInteraction();
            float headYaw=0.0f, headPitch=0.0f;
            const bool headValid=head_aim120r3::kEnabled&&CameraHook::GetHeadAimLocalAngles(headYaw,headPitch);
            const bool f3CameraPhaseTest=(f3_view_marriage_mode>0);
            const float f4Gain=f4_headlook_mode==2?0.35f:f4_headlook_mode==3?0.50f:f4_headlook_mode==4?0.70f:1.00f;
            const int aimEquipment=head_aim120r3::kEnabled?frik_pose_probe::NativeHeadAimEquipment120R3():-1;
            const bool rangedHeadAim=head_aim120r3::RangedHeadAim(aimEquipment,rrAim.trigger);
            XrManager::RenderFrameState aimFrame{};
            const bool tracked=head_aim120r3::kEnabled&&xr_manager_->CopyActiveRenderFrameState(aimFrame)&&aimFrame.head_pose_valid;
            static std::uint64_t aimFrameIndex=0,aimFrameAt=0;
            const auto aimNow=std::uint64_t(GetTickCount64());
            if(aimFrame.frame_index!=aimFrameIndex){aimFrameIndex=aimFrame.frame_index;aimFrameAt=aimNow;}
            const bool trackingFresh=tracked&&aimFrameAt&&aimNow>=aimFrameAt&&aimNow-aimFrameAt<=60;
            const bool productionHeadAim=head_aim120r3::Allowed(enable_native_head_aim_.load(std::memory_order_acquire),
                head_aim_retail_verified,protectedHeadLookBlock,trackingFresh,headValid,f3_view_marriage_mode!=0||diagnostics::lab::NativeReference()||frik_pose_probe::CombatCensusMode()<2,aimEquipment,rrAim.trigger);
            const float inputGain=productionHeadAim?1.f:f4Gain;
            const bool inputWrite=head_aim120r3::kEnabled && !protectedHeadLookBlock && native_input_look_lab_installed &&
                                  headValid&&trackingFresh&&(productionHeadAim||(f3_view_marriage_mode==0 && f4_headlook_mode>=2));
            // V66.162: one existing head/native-look publisher. The removed hand
            // servo shared its angular anchor and could inject yaw on owner transfer.
            // Preserve scoped hand-ray targeting and explicit non-VRIK Grip free look.
            native_input_look_lab::SetInputGain(inputGain);
            native_input_look_lab::SetActive(inputWrite);
            native_input_look_lab::PublishHeadAngles(headYaw,headPitch,headValid);

            native_head_rotation_owned=inputWrite && native_input_look_lab::Ready();
            CameraHook::SetNativeHeadRotationOwner(productionHeadAim&&native_head_rotation_owned);
            static int lastAimState=-1;
            const int aimState=protectedHeadLookBlock?0:!rangedHeadAim?1:!trackingFresh?2:!head_aim_retail_verified?3:native_head_rotation_owned?4:5;
            if(aimState!=lastAimState){char m[256]{};std::snprintf(m,sizeof(m),"V66.150-HEAD-AIM state=%d equipment=%d rt=%.2f requested=%d ready=%d yawTemplate=%d pitchTemplate=%d",aimState,aimEquipment,double(rrAim.trigger),enable_native_head_aim_.load()?1:0,native_input_look_lab::Ready()?1:0,native_input_look_lab::HasYawTemplate()?1:0,native_input_look_lab::HasPitchTemplate()?1:0);diagnostic_trace::Write(m);lastAimState=aimState;}
            view_marriage_visual_active=false;
            CameraHook::SetViewMarriageMode(0,1.0f);

            // V64.40: keep all three camera-phase rungs pure OpenXR with no native
            // head-input participation. Only the injection phase differs. Mode 1
            // preserves the V64.39 fresh PublishViewCamera experiment. Modes 2/3
            // use the early persistent CCamera path; mode 3 additionally persists
            // the composed pose into canonical CView rotation/position history state.
            CameraHook::SetFreshPublishPose(f3_view_marriage_mode==1);
            CameraHook::SetInjectionMode(f3_view_marriage_mode>=2?0:1);
            CameraHook::SetHistoryPersistentPose(f3_view_marriage_mode==3);
            xr_manager_->SetLatePoseEnabled(false);
            if(f3CameraPhaseTest) native_head_rotation_owned=false;
            // V64.83 preserves the V64.81 ownership rule: theater is the final
            // camera authority, even after the native-head/F3/F4 block.
            CameraHook::SetTrackingOptions(
                suspend_game_camera_for_ui ? false : (f3CameraPhaseTest ? true : (native_head_rotation_owned ? false : apply_head_rotation_.load(std::memory_order_acquire))),
                suspend_game_camera_for_ui ? false : apply_head_position_.load(std::memory_order_acquire));
            // V65.03: interaction alignment has one owner earlier in the presentation
            // resolver. Retired F3/F4 labs may not disable the production HMD-gaze ray.
            touch_bindings_.SetHeadAim(false,0.0f,0.0f,false);

            static std::uint64_t nextNativeInputLog=0;
            const auto niNow=static_cast<std::uint64_t>(GetTickCount64());
            if((f4_headlook_mode!=0 || f3_view_marriage_mode!=0) && niNow>=nextNativeInputLog) {
                nextNativeInputLog=niNow+1000ull;
                char nm[384]{};
                std::snprintf(nm,sizeof(nm),
                    "V64.41-HEAD f3=%d f4=%d blocked=%d headValid=%d ready=%d gain=%.2f templates={yaw:%d pitch:%d} visual=%s interaction=%d injection=%d freshPublish=%d historyPose=%d latePose=0",
                    f3_view_marriage_mode,f4_headlook_mode,protectedHeadLookBlock?1:0,headValid?1:0,
                    native_input_look_lab::Ready()?1:0,double(inputGain),native_input_look_lab::HasYawTemplate()?1:0,native_input_look_lab::HasPitchTemplate()?1:0,
                    native_head_rotation_owned?"KCD2_NATIVE":"OPENXR",
                    head_interaction_aim?1:0,
                    f3_view_marriage_mode>=2?0:1,f3_view_marriage_mode==1?1:0,f3_view_marriage_mode==3?1:0);
                diagnostic_trace::Write(nm);
            }
        }
        const auto& gpL=xr_manager_->LeftController(); const auto& gpR=xr_manager_->RightController();

        // V66.96: KCD2VR retains physical Touch ownership and resolves VR-only
        // chords first. The remaining state is published through the existing
        // native XI dispatcher; keyboard/scancode behavior is fallback only.
        const bool l3_r3_chord = gpL.active && gpR.active && gpL.thumbstick_down && gpR.thumbstick_down;
        // R113: L3+R3 HELD 0.6 s is the VR menu for controllers without a thumbrest (touch_bindings.cpp R112 chord).
        // the V66.88 panic release therefore fires on a SHORT L3+R3 click (released before 0.6 s), not on the press -
        // before R113 holding both sticks for the menu also beeped and dropped any flat/theater UI.
        static std::uint64_t l3r3_since_r113=0;
        if(l3_r3_chord && !lg_r3_prev) l3r3_since_r113=GetTickCount64();
        if(!l3_r3_chord && lg_r3_prev && l3r3_since_r113 && GetTickCount64()-l3r3_since_r113<600ull && !matrix_active) {
            // Resolved before the NEXT frame's single presentation snapshot.
            // R124: same release as F6 (also leaves a cutscene/dialogue Theater), never forces Theater.
            (void)kcd2vr::theater_toggle_r124::Press(theater_toggle_r124,theater_shown_prev_r124,false);
            emergency_toggle_requested=true;
            diagnostic_trace::Write("V66.88-ROUTER L3+R3 short click: panic queued; no half-frame owner mutation");
        }
        if(!l3_r3_chord) l3r3_since_r113=0;
        lg_r3_prev=l3_r3_chord;

        // Keep VRIK/grip-rearm ownership above the game-facing transport.
        // C7FM1: no new scene/UI queries in the worker; publish context and diagnostics only.
#define KCD2VR_M2_HOST_SLOT 5020 // M2_SOURCE_SELECTOR
#include "subsystems/vrmap/host.inl" // M2_OWNED_SOURCE slot=5020
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
        auto routeL=gpL; auto routeR=gpR;
        { // H41: consume an accepted stow press through its physical release, on both transports.
            static kcd2vr::grip_stow_cb1::ButtonOwnerH41 stowButtonsH41{};
            const auto nowH41=std::uint64_t(GetTickCount64());
            kcd2vr::grip_stow_cb1::RouteH41(stowButtonsH41,gpR,gpL,routeR,routeL,kcd2vr::grip_stow_cb1::Button(),
                !ui88_input_result.blocks_gameplay,[&](int side){return kcd2vr::grip_stow_cb1::Request(side,nowH41);});
        }
#define KCD2VR_M2_HOST_SLOT 6001 // M2_SOURCE_SELECTOR
#include "subsystems/vrmap/host.inl" // M2_OWNED_SOURCE slot=6001
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
        if(pauseConsumed58c2)routeL.menu_down=false; // same consumed chord for XI and fallback
        if(!activity163.nativeControls&&!ui89_policy.BlocksWorldInteraction()&&!mis_menu_after_frame){
            if(ui88_input_result.grip_rearm_mask&1u)routeL.grip=0.0f;
            if(ui88_input_result.grip_rearm_mask&2u)routeR.grip=0.0f;
            routeL.trigger=gameplay_trigger_l;routeR.trigger=gameplay_trigger_r;
        }

        if(l3_r3_chord) {
            routeL.thumbstick_down=false; routeR.thumbstick_down=false;
            routeL.thumbstick_pressed=false; routeR.thumbstick_pressed=false;
        }

        // V66.139: resolve physical RT ownership before *either* game-facing
        // transport.  Previously native XI saw the raw trigger first while the
        // fallback mouse path was zeroed later, so RT semantics changed when XI
        // readiness changed during a session.  Only the physical bow owns RT;
        // melee, crossbow and rifle keep their stock/native trigger transport.
        const bool physical_rt_owned = !mis_menu_after_frame && !activity163.nativeControls && !ui89_policy.BlocksWorldInteraction() &&
            !immersive_cursor_menu && frik_pose_probe::PhysicalWeaponConsumesRightTrigger139();
        if(physical_rt_owned) routeR.trigger=0.0f;
        // Resolve F4 gesture-only melee and fist/physical ownership before BOTH transports.
        // Previously native XI saw raw RT/LT first, while only fallback saw this
        // filter: held grips/triggers could drive an extra stock attack/block.
        const bool fist_menu=mis_menu_after_frame||ui89_policy.BlocksWorldInteraction()||immersive_cursor_menu||activity163.nativeControls;
        routeR.trigger=frik_pose_probe::FistTransportTrigger101(0,routeR.trigger,routeR.active,fist_menu,routeR.grip);
        routeL.trigger=frik_pose_probe::FistTransportTrigger101(1,routeL.trigger,routeL.active,fist_menu,routeL.grip);
        static bool physical_rt_owned_prev=false;
        if(physical_rt_owned!=physical_rt_owned_prev){
            char m[224]{};std::snprintf(m,sizeof(m),
                "V66.139-INPUT RT owner=%s nativeXI=%d F12=%d F3=%u",
                physical_rt_owned?"PHYSICAL_BOW":"GAME_TRANSPORT",native_xi_full?1:0,
                frik_pose_probe::WeaponControlMode(),frik_pose_probe::HolsterCombatDiagMode());
            diagnostic_trace::Write(m);physical_rt_owned_prev=physical_rt_owned;
        }
        static int nativeMiniTransport162r1=-1;
        const int miniTransport162r1=activity163.nativeControls?(native_xi_full?1:2):0;
        if(nativeMiniTransport162r1!=miniTransport162r1){nativeMiniTransport162r1=miniTransport162r1;
            diagnostic_trace::Write(miniTransport162r1==1?"V66.162R1-MINIGAME transport=NATIVE_XI LT=leftTrigger RT=rightTrigger LB=leftGrip RB=rightGrip DPAD=thumbrest+LS":
                miniTransport162r1==2?"V66.162R1-MINIGAME transport=KEYBOARD_FALLBACK nativeParity=UNAVAILABLE":"V66.162R1-MINIGAME transport=GAMEPLAY");}
        // R20C1: public F5 rung 2. Native event witnesses own workstation/stage
        // eligibility; no dependence on the retired action-map-enable observer.
        static workstation178r20c1::Controller workstationC1;
        workstation_sample46::Frame semiFrame46{};
        (void)frik_pose_probe::WorkstationFrame46(semiFrame46);
        {
            using namespace workstation178r20c1;using activity_input163::Kind175;
            const bool wsOn=gesturesOn(GetTickCount64())&&native_xi_full&&activity163.nativeControls&&
                !mis_menu_after_frame&&!ui88_input_result.blocks_gameplay&&!dialogue_active&&xr_manager_&&
                !ui89_policy.MenuAdapter();
            const auto& witness=activity_input163::witness175;
            const bool ready=witness.ready.load(std::memory_order_acquire);
            const auto alch=ready&&activity_input163::trackedHands59c7f[unsigned(Kind175::Alchemy)].load()?witness.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire):0;
            const auto smith=ready&&activity_input163::trackedHands59c7f[unsigned(Kind175::Smithing)].load()?witness.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire):0;
            const int phase=wsOn&&smith?witness.SmithPhaseC1():-1;
            const auto instance=wsOn?(alch?alch:smith):0;
            const Kind kind=!instance?Kind::None:alch?Kind::Alchemy:
                activity_input163::AnvilPhaseC1(phase)?Kind::Anvil:Kind::SmithingOther;
            std::array<Sample,2> hands{};
            if(instance)for(int side=0;side<2;++side){auto& sample=hands[side];
                sample.valid=semiFrame46.allowed&&semiFrame46.hand[side].valid;
                sample.height=semiFrame46.hand[side].position[1];sample.grip=semiFrame46.hand[side].grip;
                sample.sequence=semiFrame46.sequence;sample.tick=semiFrame46.tick;sample.timeNs=semiFrame46.timeNs;}
            // Tracking is published later than ladder_now_ms in this same loop.
            // Consume against the current clock, never label a fresh sample "future".
            const auto wsNow=std::uint64_t(GetTickCount64());
            // C45 MINIGAME HANDS witness: the native alchemy / smithing minigame is live, its native controls own the input (no
            // gameplay / weapon authority), no VR menu, UI block or dialogue, and that activity's hands are TRACKED (alch/smith above).
            // Independent of gesturesOn (wsOn) so VrWorkstationGestures=false cannot starve it. Copied, never polled.
            {const bool miniLive45=activity163.nativeControls&&!mis_menu_after_frame&&!ui88_input_result.blocks_gameplay&&!dialogue_active&&xr_manager_&&!ui89_policy.MenuAdapter();
             const unsigned witness45=miniLive45?(alch?minigame_hands45::Alchemy:smith?minigame_hands45::Smithing:minigame_hands45::None):minigame_hands45::None;
             minigame_hands45::publish(witness45,wsNow);
             if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,wsNow)){ // F5 rung 1, edge-only
                 const char* gate45=minigame_hands45::reasonNow(combat_rules178r10::workstationEnabled.load(std::memory_order_relaxed),wsNow);
                 const int mode45=minigame_hands45::currentMode();
                 static int lastMode45=-1;static unsigned lastWitness45=99;static const char* lastGate45="";
                 if(mode45!=lastMode45||witness45!=lastWitness45||gate45!=lastGate45){lastMode45=mode45;lastWitness45=witness45;lastGate45=gate45;
                     char m[260]{};std::snprintf(m,sizeof(m),"C45 MINIGAME_HANDS mode=%s gate=%s witness=%s nativeControls=%d trackedAlchemy=%d trackedSmithing=%d smithAutoHold=%d alchemyPassenger=%d f5Rung=%d f10Rung=%d authority=SEMI_WORK_INPUT_AND_NATIVE_HANDOFF",
                         minigame_hands45::label(mode45),gate45?gate45:"OPEN",minigame_hands45::witnessName(witness45),activity163.nativeControls?1:0,
                         activity_input163::trackedHands59c7f[unsigned(Kind175::Alchemy)].load()?1:0,activity_input163::trackedHands59c7f[unsigned(Kind175::Smithing)].load()?1:0,
                         smithAutoHold36.load(std::memory_order_acquire)?1:0,alchemyPassenger42.load()?1:0,combat_rules178r10::f5Rung.load(),hands59c7f::rung.load());
                     diagnostic_trace::Write(m);}}}
            auto result=workstationC1.update(kind,instance,phase,hands,wsNow);
            #include "vrik/alchemy_input_producer19c4.inl"
            const bool previous=combat_rules178r10::useHeld.exchange(result.use,std::memory_order_relaxed);
            if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,wsNow)){
                static diagnostics::interaction_receipt59c10::Gate receipt;
                const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
                const int handState=(workstationC1.armed[0]?1:0)|(workstationC1.armed[1]?2:0)|
                    (workstationC1.held[0]?4:0)|(workstationC1.held[1]?8:0)|(wsOn?16:0)|(result.stroke?32:0);
                // R8b: alchemy has no smith phase; its stage = native action class (alchemy_input19c4::currentStage37)
                const char* alchemyStageR8b=alch?kcd2vr::alchemy_input19c4::currentStage37(alch,wsNow):"-";
                const int stageKeyR8b=alch?1000+kcd2vr::alchemy_input19c4::currentStageCode37(alch,wsNow):phase;
                if(receipt.observe(session,wsNow,{instance,unsigned(kind),stageKeyR8b,handState},100,768)){
                    char m[860]{};std::snprintf(m,sizeof(m),
                        "C10 F5_WORKSTATION session=%llu kind=%s instance=%llX phase=%d alchemyStage=%s nativeAlchemy=%llX nativeSmith=%llX controlsQualified=%d witnessReady=%d nativeControls=%d vrMenu=%d uiBlocked=%d dialogue=%d menuAdapter=%d armedR=%d armedL=%d heldR=%d heldL=%d validR=%d validL=%d gripR=%.3f gripL=%.3f ageR=%llu ageL=%llu use=%d useChanged=%d stroke=%d hammerFastestDownMps=%.2f observed=%llu changes=%llu coalesced=%llu source=NATIVE_STAGE_CALLBACK",
                        (unsigned long long)session,kind==Kind::Alchemy?"ALCHEMY":kind==Kind::Anvil?"ANVIL":kind==Kind::SmithingOther?"SMITHING_OTHER":"NONE",
                        (unsigned long long)instance,phase,alchemyStageR8b,(unsigned long long)alch,(unsigned long long)smith,wsOn,ready,activity163.nativeControls,
                        mis_menu_after_frame,ui88_input_result.blocks_gameplay,dialogue_active,ui89_policy.MenuAdapter(),
                        int(workstationC1.armed[0]),int(workstationC1.armed[1]),int(workstationC1.held[0]),int(workstationC1.held[1]),
                        hands[0].valid,hands[1].valid,double(hands[0].grip),double(hands[1].grip),
                        (unsigned long long)diagnostics::interaction_receipt59c10::age(wsNow,hands[0].tick),
                        (unsigned long long)diagnostics::interaction_receipt59c10::age(wsNow,hands[1].tick),result.use,previous!=result.use,result.stroke,double(-workstationC1.hammer.fastestDown),
                        (unsigned long long)receipt.seen,(unsigned long long)receipt.changes,(unsigned long long)receipt.coalesced);
                    diagnostic_trace::Write(m);workstationC1.hammer.fastestDown=0; // C37C4 receipt window
                }
            }
            combat_rules178r10::strokeUntil.store(workstationC1.strokeEnd,std::memory_order_relaxed);
        }
        // C71 outer gate is recorded even when no native XI/crafting adapter runs.
        {const auto now=std::uint64_t(GetTickCount64());if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){
            auto& w=activity_input163::witness175;const auto alch=w.active[unsigned(activity_input163::Kind175::Alchemy)].load(),smith=w.active[unsigned(activity_input163::Kind175::Smithing)].load();
            const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();static diagnostics::interaction_receipt59c10::Gate receipt;
            const int bits=(native_xi_full?1:0)|(activity163.nativeControls?2:0)|(ui88_input_result.blocks_gameplay?4:0)|(dialogue_active?8:0)|(mis_menu_after_frame?16:0)|(w.ready.load()?32:0)|(w.transitionDepth175.load()?64:0);
            if((alch||smith)&&receipt.observe(session,now,{alch?alch:smith,unsigned(w.lifetime175.load()),combat_rules178r10::f5Rung.load(),bits},100,4096,500)){
                char line[1100]{};std::snprintf(line,sizeof(line),"C71 F5_CRAFT_CONTEXT session=%llu tick=%llu instance=%llX life=%llu nativeXI=%d nativeControls=%d uiBlocked=%d dialogue=%d vrMenu=%d witnessReady=%d transitionDepth=%u f5Rung=%d alchemyMode=%d smithMode=%d gripR=%.4f gripL=%.4f triggerR=%.4f triggerL=%.4f trackingFrame=%llu trackingAgeMs=%llu frameAllowed=%d headXR=%.6f,%.6f,%.6f",
                    (unsigned long long)session,(unsigned long long)now,(unsigned long long)(alch?alch:smith),(unsigned long long)w.lifetime175.load(),native_xi_full,activity163.nativeControls,ui88_input_result.blocks_gameplay,dialogue_active,mis_menu_after_frame,w.ready.load(),unsigned(w.transitionDepth175.load()),combat_rules178r10::f5Rung.load(),int(minigame_hands45::modeFor(minigame_hands45::Alchemy)),int(minigame_hands45::modeFor(minigame_hands45::Smithing)),routeR.grip,routeL.grip,routeR.trigger,routeL.trigger,
                    (unsigned long long)semiFrame46.sequence,(unsigned long long)diagnostics::interaction_receipt59c10::age(now,semiFrame46.tick),semiFrame46.allowed,semiFrame46.head[0],semiFrame46.head[1],semiFrame46.head[2]);diagnostic_trace::Write(line);
            }
        }}
        // R58C2: copied camera/activity telemetry only, inside F5 recording. This
        // distinguishes source FOV changes from compositor scaling without polling Flash.
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64())){
            static std::uint64_t next58c2=0,priorSession58c2=0;static unsigned priorKind58c2=~0u;static float priorFov58c2=0;
            const auto now58c2=std::uint64_t(GetTickCount64());
            const auto session58c2=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
            if(now58c2>=next58c2||session58c2!=priorSession58c2){
                next58c2=now58c2+250;
                auto& w=activity_input163::witness175;unsigned kind58c2=0;
                if(w.ready.load())for(unsigned i=0;i<w.active.size();++i)if(w.active[i].load())kind58c2|=1u<<i;
                const float fov58c2=CameraHook::LatestGameplayFovDegrees();
                if(session58c2!=priorSession58c2||kind58c2!=priorKind58c2||std::fabs(fov58c2-priorFov58c2)>=.5f){
                    char m[480]{};std::snprintf(m,sizeof(m),"R58C2 F5_ACTIVITY_CAMERA session=%llu nativeKindMask=%u witnessReady=%d smithPhase=%d gameplayFov=%.3f fovSampleCount=%llu compositorSourceFov=%.3f userZoom=%.3f theater=%d cameraMutation=NONE nativeCameraTranslation=UNOBSERVED",
                        (unsigned long long)session58c2,kind58c2,w.ready.load(),w.SmithPhaseC1(),double(fov58c2),(unsigned long long)CameraHook::GameplayFovSampleCount(),double(vr_source_vertical_fov_deg_.load()),double(vr_zoom_.load()),int(policy_theater_scene));
                    diagnostic_trace::Write(m);priorSession58c2=session58c2;priorKind58c2=kind58c2;priorFov58c2=fov58c2;
                }
            }
        }
        {   // V66.178R19: hand item census for F11 (reload) and F5 (interaction/workstation), once per change.
            const auto cNow=std::uint64_t(GetTickCount64());
            // R59C1: native item/entity reads are in PhysicalNativeFrameTick6698.
            (void)cNow;
        }
        if(native_xi_full) {
            const bool apseUi=ui89_policy.owner==CleanMenuOwner::InventoryMap||ui89_policy.owner==CleanMenuOwner::Shop||
                ui89_policy.owner==CleanMenuOwner::CraftingMenu||ui89_policy.owner==CleanMenuOwner::ContextualMenu;
            const bool quickTalkOpenH19=quick_talk_native_h19::Open();
            const native_vr_pad_premap::Context padCtx{
                .vr_menu=mis_menu_after_frame,
                .theater=ui89_policy.MenuAdapter()&&!ui89_policy.SystemMenu(),
                .contextual_ui=ui89_policy.owner==CleanMenuOwner::ContextualMenu,
                .dialogue_navigation=dialogue_active,
                .back_context=(dialogue_active||quickTalkOpenH19||action_map_probe::BackContext()||ui89_policy.MenuAdapter()||ui89_policy.NativeContextControls())&&!mis_menu_after_frame,
                .apse_hub=ui89_policy.owner==CleanMenuOwner::InventoryMap,
                .apse_ui=apseUi,
                .system_menu=ui89_policy.SystemMenu(),
                .passive_back_only=ui89_policy.PassiveBackOnly(),
                .semantic_minigame=activity163.nativeControls,
                .back_epoch=(action_map_probe::Transitions()<<32)^ui89_policy.context^(activity_input163::StageEpoch59c10()*0x9E3779B97F4A7C15ull), // R59.16C10: real native stage epoch (witnessed activity instances), Transitions() is inert
                .pointer_hand=ui88_input_result.hand,
                // Same owner as the fallback path: VRIK disables Grip free-look,
                // so Grip must not also suppress native stick locomotion (F8).
                .grip_weapon_comparison114=!touch_bindings_.RightGripFreeLookEnabled(),
                .quick_talk_open54=quickTalkOpenH19,
                .hand_claim_blocked_t24={frik_pose_probe::HandClaimBlocksT24(0),frik_pose_probe::HandClaimBlocksT24(1)},
                .snap_turn_d37=vr_turn_mode_.load(std::memory_order_relaxed)==0
            };
            auto padLeft5916c3=routeL,padRight5916c3=routeR;
            if(held_map_m1::Selected()&&held_map_m1::Allowed(GetTickCount64())&&!activity163.nativeControls&&routeR.quick_access_touch&&routeL.thumb_y>.55f&&std::fabs(routeL.thumb_x)<routeL.thumb_y){padLeft5916c3.thumb_y=0;padLeft5916c3.thumb_x=0;}
            if(padCtx.semantic_minigame&&!padCtx.vr_menu&&!padCtx.theater&&!padCtx.system_menu&&!padCtx.dialogue_navigation){
                const auto gripNow=std::uint64_t(GetTickCount64());
                if(workstationC1.ownsGrip(1,gripNow))padLeft5916c3.grip=0;
                if(workstationC1.ownsGrip(0,gripNow))padRight5916c3.grip=0;
            }
            const auto gestureTalkH4=PrepareGestureTalkH4(padCtx,xr_head_valid&&palmInput36.gameplay);
            auto talkPadCtxH4=padCtx;talkPadCtxH4.quick_talk_claim_h4=gestureTalkH4.claimMaskH4||gestureTalkH4.openChat;
            talkPadCtxH4.carry_query_r25=native_input_look_lab::GetGamepadMode()==3?&native_input_look_lab::QueryCarryContext25:nullptr;
            auto pad=native_vr_pad_premap::Build(padLeft5916c3,padRight5916c3,talkPadCtxH4);
            if(pad.torchRequestTick62)(void)frik_pose_probe::RequestTorch62(false,pad.torchRequestTick62);
            if(pad.inventoryRequestR118)touch_bindings_.QueueInventoryR118(true); // R118: X hold on STEAMVR / PS VR2 layout -> Inventory (sent by TouchBindings below)
            if(pad.xUtilityPress62){char xm[200]{};std::snprintf(xm,sizeof(xm),
                "C62 TORCH_INPUT X_PRESS route=%u legend=1:talkTorch,2:visor,3:wait,4:nativeUI,5:quarantined,6:inactive",unsigned(pad.xUtilityPress62));diagnostic_trace::Write(xm);}
            if(pad.backOwned59c10&&padCtx.apse_ui&&(pad.backEvent59c10==1||pad.backEvent59c10==4))
                g_apse_close_press_ms.store(std::uint64_t(GetTickCount64()),std::memory_order_relaxed); // C25: arms the Inventory early Theater exit
            if(pad.backEvent59c10){ // R59.16C10: bounded B transaction receipt (edges only; F5 capture)
                static const char* const kEvt[]={"NONE","PRESS","RELEASE","REARM_PULSE","MENU_CLOSE_PRESS"};
                char bm[320]{};std::snprintf(bm,sizeof(bm),"R59.16C10 UI-INPUT B_TRANSACTION phase=%s owned=%d published=%s ownerCtx=%u backContext=%d theater=%d system=%d dialogue=%d minigame=%d apse=%d vrMenu=%d stageEpoch=%016llX",
                    kEvt[pad.backEvent59c10<5?pad.backEvent59c10:0],pad.backOwned59c10?1:0,pad.b?"B":(pad.rb?"RB":"NONE"),unsigned(ui89_policy.context),
                    padCtx.back_context?1:0,padCtx.theater?1:0,padCtx.system_menu?1:0,padCtx.dialogue_navigation?1:0,padCtx.semantic_minigame?1:0,padCtx.apse_ui?1:0,padCtx.vr_menu?1:0,
                    (unsigned long long)padCtx.back_epoch);
                diagnostic_trace::Write(bm);
            }
            ApplyUtilityGestures54(pad,padCtx,xr_head_valid&&palmInput36.gameplay);
            // V66.178R8: physical attack/guard edges ride the native XI pad (defaultProfile.xml combat_base).
            // V66.178R16: acknowledged edges + ownership carried with the pad (attack_xi178r8::apply); the
            // special LT hold is part of the same mask. Nothing is forced inside menus/dialogue/minigames.
            if(!padCtx.vr_menu&&!padCtx.theater&&!padCtx.system_menu&&!padCtx.dialogue_navigation&&!padCtx.semantic_minigame&&!padCtx.quick_talk_open54)
                attack_xi178r8::apply(std::uint64_t(GetTickCount64()),pad.rt,pad.lt,pad.a,&pad.lb);
            else attack_xi178r8::ownedMask.store(0,std::memory_order_release);
            // F5 shares the existing native Use superaction. Never emit E while
            // XI owns the input family; preserve a user-held A on release.
            const bool handUseXi59c10=touch_bindings_.PhysicalInteractionGamepad59c10(GetTickCount64(),handUseAllowed59c10&&
                !padCtx.vr_menu&&!padCtx.theater&&!padCtx.system_menu&&!padCtx.dialogue_navigation&&!padCtx.semantic_minigame&&!padCtx.quick_talk_open54);
            pad.a=pad.a||handUseXi59c10;
            { // C46R1: current native book + one stamped, stable reading frame.
                namespace bp=kcd2vr::book_pages45;static book_gesture46::Arbiter pages46;
                const auto& w42=activity_input163::witness175;const auto now45=std::uint64_t(GetTickCount64());
                const auto lifetime46=w42.lifetime175.load(std::memory_order_acquire);
                const bool ctx45=padCtx.semantic_minigame&&!padCtx.vr_menu&&!padCtx.theater&&!padCtx.system_menu&&
                    !padCtx.dialogue_navigation&&!ui88_input_result.blocks_gameplay&&!ui89_policy.MenuAdapter()&&
                    w42.ready.load()&&!w42.transitionDepth175.load();
                activity_input163::AlchemyBook45 abook45{};
                const auto reading46=w42.active[unsigned(activity_input163::Kind175::Reading)].load(std::memory_order_acquire);
                const int book45=!ctx45?bp::None:reading46?bp::Reading:
                    activity_input163::AlchemyBookOpen45(now45,abook45)?bp::Alchemy:bp::None;
                const int mode45=bp::clampMode(bp::mode.load(std::memory_order_relaxed));
                workstation178r20c1::WorkstationOwnerH51 pageStationD53{};
                const bool moveBookD53=workstation178r20c1::workstationOwnerH51(now45,pageStationD53)&&
                    pageStationD53.kind==minigame_hands45::Alchemy&&
                    minigame_hands45::modeFor(minigame_hands45::Alchemy)==minigame_hands45::SixDofVrik;
                const float x42=station_reposition_d53::pageStick(moveBookD53,routeL.thumb_x,routeR.thumb_x);
                const auto owner46=book45==bp::Alchemy?
                    abook45.lifetime^(abook45.action*0x9E3779B97F4A7C15ull)^(abook45.generation*0xD6E8FEB86659FD93ull):
                    lifetime46^(reading46*0x9E3779B97F4A7C15ull);
                const auto r45=pages46.step(book45,owner46,now45,mode45,pad.dpad_left,pad.dpad_right,semiFrame46,x42,
                    abook45.instance,abook45.action,abook45.lifetime);
                const book_input46::Identity identity46{book45,lifetime46,reading46,abook45};
                const bool applied46=book_input46::apply(identity46,mode45,r45,std::uint64_t(GetTickCount64()),pad);
                if(!applied46){pages46.native=bp::Arbiter{};pages46.lastSource=bp::NoSource;}
                if(r45.requested&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now45)){
                    char m[370]{};std::snprintf(m,sizeof(m),"C46 PAGE_INPUT source=%s dir=%+d book=%s accepted=%d mode=%s zone=%s busy=%d lifetime=%llu action=%llX generation=%llu sample=%llu published=%s nativeOutcome=UNOBSERVED",
                        bp::sourceName(r45.source),r45.dir,bp::bookName(book45),r45.accepted?1:0,bp::modeName(mode45),pages46.zoneName(),r45.busy?1:0,
                        (unsigned long long)lifetime46,(unsigned long long)abook45.action,(unsigned long long)abook45.generation,
                        (unsigned long long)semiFrame46.sequence,r45.accepted&&applied46?(r45.dir<0?"LB_PULSE":"RB_PULSE"):"NONE");
                    diagnostic_trace::Write(m);
                }
            }

            // V66.178R10/R13: F5 rung 2 workstation inputs.
            if(!padCtx.vr_menu&&!padCtx.theater&&!padCtx.system_menu&&!padCtx.dialogue_navigation){
                const auto r10Now=std::uint64_t(GetTickCount64());
                if(padCtx.semantic_minigame){
                    workstationC1.apply(r10Now,pad.a,pad.lb,pad.rb,pad.rt);
                }
                else if(combat_diag_probe::GeometryLockClick178r29(r10Now))pad.r3=true; // V66.178R29: rung 2 enters combat through the game's own opponent lock
            }
            { // C46R1: the only automatic alchemy/smith input producer, after native mapping.
                const auto& w=activity_input163::witness175;
                semi_workstations46::Context c{};c.lifetime=w.lifetime175.load(std::memory_order_acquire);
                c.alchemy=w.active[unsigned(activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire);
                c.smith=w.active[unsigned(activity_input163::Kind175::Smithing)].load(std::memory_order_acquire);
                c.smithPhase=w.SmithPhaseC1();
                const auto now46=std::uint64_t(GetTickCount64());
                const bool oneCraft67=bool(c.alchemy)!=bool(c.smith);
                const auto kind67=c.alchemy?minigame_hands45::Alchemy:minigame_hands45::Smithing;
                c.guided=guided_crafting55::active(now46);
                c.gestureAlchemy67=c.alchemy&&workstation178r20c1::fullVrAlchemy63(now46);
                c.alchemyTransferD59=misc_gestures_d59::alchemyTransfer.load(std::memory_order_acquire);
                c.gestureSmith67=c.smith&&workstation178r20c1::gestureSmith67(now46);
                c.alchemyGaze63=c.guided&&c.alchemy&&!c.smith;
                c.enabled=oneCraft67&&(c.guided||minigame_hands45::modeFor(kind67)!=minigame_hands45::Native)&&
                    combat_rules178r10::workstationEnabled.load(std::memory_order_acquire);
                if(c.guided&&!c.alchemyGaze63)(void)interaction_alignment::LastHandCandidate(c.touch);
                c.qualified=padCtx.semantic_minigame&&!padCtx.vr_menu&&!padCtx.theater&&!padCtx.system_menu&&
                    !padCtx.dialogue_navigation&&!ui88_input_result.blocks_gameplay&&!ui89_policy.MenuAdapter()&&
                    w.ready.load(std::memory_order_acquire)&&!w.transitionDepth175.load(std::memory_order_acquire)&&
                    c.lifetime==w.lifetime175.load(std::memory_order_acquire)&&oneCraft67&&
                    minigame_hands45::witnessCurrent67(kind67,now46);
                c.moveQualifiedD53=padCtx.semantic_minigame&&!padCtx.vr_menu&&!padCtx.theater&&!padCtx.system_menu&&
                    !padCtx.dialogue_navigation&&!ui88_input_result.blocks_gameplay&&!ui89_policy.MenuAdapter()&&
                    w.ready.load(std::memory_order_acquire)&&!w.transitionDepth175.load(std::memory_order_acquire)&&
                    c.lifetime==w.lifetime175.load(std::memory_order_acquire)&&oneCraft67&&routeL.active;
                c.moveXD53=routeR.quick_access_touch?0.f:routeL.thumb_x;
                c.moveYD53=routeR.quick_access_touch?0.f:routeL.thumb_y;
                c.alchemyTracked=activity_input163::trackedHands59c7f[0].load();
                c.smithTracked=activity_input163::trackedHands59c7f[1].load();
                c.smithGestures=workstation178r20c1::smithAutoHold36.load();
                c.whyVisibleW4=combat_rules178r10::f5Rung.load(std::memory_order_relaxed)==3; // W4: F5 rung 3 says WHY a squeeze failed
                if(c.gestureAlchemy67||(c.smith&&!c.alchemy&&
                   minigame_hands45::modeFor(minigame_hands45::Smithing)!=minigame_hands45::Native))for(int side=0;side<2;++side){auto& p=c.world67[side];
                    float directionH21[3]{};
                    const bool aimValidH21=frik_pose_probe::ReadWorkstationPalmAimH21(side,p.point[0],p.point[1],p.point[2],p.tick,p.poseEpoch,p.sourceEpoch,c.worldPlayer67[side],directionH21);
                    p.valid=aimValidH21||frik_pose_probe::ReadWorkstationPalm67(side,p.point[0],p.point[1],p.point[2],p.tick,p.poseEpoch,p.sourceEpoch,c.worldPlayer67[side]);
                    c.aimH21[side]={{directionH21[0],directionH21[1],directionH21[2]},p.tick,p.poseEpoch,p.sourceEpoch,
                        minigame_hands45::modeEpoch67.load(std::memory_order_acquire),side,aimValidH21};
                }
                semiWorkstations46.apply(c,semiFrame46,now46,pad);
                if(c.gestureAlchemy67&&semiWorkstations46.lastAlchemy67.nomination.decision.hapticEdge){
                    const auto& d=semiWorkstations46.lastAlchemy67.nomination.decision;
                    const int side=d.source==subsystems::alchemy::near67::Source::Left?1:0;
                    (void)frik_pose_probe::HoverHaptic178r14(side,(std::uint64_t(d.ref.entity)<<16)^minigame_hands45::modeEpoch67.load());
                }
                if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now46)){
                    static diagnostics::interaction_receipt59c10::Gate gate46;
                    const auto session46=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
                    const auto& r=semiWorkstations46.lastSmith;
                    // D42 actual-stage receipts replace redundant G2 CRAFT_INPUT.
                    if(c.guided){
                        static diagnostics::interaction_receipt59c10::Gate guidedGate55;
                        const auto state55=semiWorkstations46.lastGuided;
                        if(guidedGate55.observe(session46,now46,{c.alchemy?c.alchemy:c.smith,unsigned(state55),c.smithPhase,0},0,160)){
                            char line[380]{};std::snprintf(line,sizeof(line),
                                "C55 GUIDED_CRAFT state=%s activity=%s nativePhase=%d mode=F5_RUNG3 selection=%s toolPose=EXACT_NATIVE_JOIN_REQUIRED progress=DOCKED_CONTROLLER_CREDIT_ONLY nativeEffect=UNOBSERVED",
                                guided_crafting55::name(state55),c.alchemy?"ALCHEMY":"SMITHING",c.smithPhase,c.alchemyGaze63?"CURRENT_NATIVE_HMD_GAZE":"CURRENT_NATIVE_PALM");
                            diagnostic_trace::Write(line);
                        }
                    }
                    if(c.smith&&gate46.observe(session46,now46,{c.smith,unsigned(r.reason),c.smithPhase,int(r.clock.request)},500,160)){
                        char line[440]{};std::snprintf(line,sizeof(line),
                            "C46 SEMI_SMITH_INPUT phase=%d state=%s request=%llu sample=%llu axes=(%.3f,%.3f) nativeRT=%d raiseWork=%.3f finishWork=%.3f clockReady=%d nativeOutcome=UNOBSERVED",
                            c.smithPhase,semi_smith46::name(r.reason),(unsigned long long)r.clock.request,
                            (unsigned long long)semiFrame46.sequence,double(r.x),double(r.y),r.requestRT?1:0,
                            r.clock.raiseWork,r.clock.finishWork,semi_smith46::clockReady.load()?1:0);
                        diagnostic_trace::Write(line);
                    }
                }
            }
            kcd2vr::gesture_talk59c5::apply54(pad,gestureTalkH4);
            {   // R134: gameplay walking follows the recentered HMD yaw. FREE PLUS adds its absorbed camera turn
                // because that view offset is also part of the direction the player sees. Transform only the native
                // gameplay stick; UI/activity axes and roomscale follow/dodge contributions keep their owners.
                namespace r=kcd2vr::free_targeting_r133;
                const bool padGameplayR134=pad.gameplay_r133&&!padCtx.vr_menu&&!padCtx.theater&&!padCtx.system_menu&&
                    !padCtx.dialogue_navigation&&!padCtx.semantic_minigame&&!padCtx.apse_ui&&!padCtx.back_context&&
                    xr_head_valid&&palmInput36.gameplay;
                if(padGameplayR134){
                    double movementYawR134=0.0;
                    if(g_hmd_walking_direction134.load(std::memory_order_acquire)){
                        float hmdYawR134=0.f,hmdPitchR134=0.f;
                        if(CameraHook::GetHeadAimLocalAngles(hmdYawR134,hmdPitchR134)&&std::isfinite(hmdYawR134))
                            movementYawR134=double(hmdYawR134);
                    }
                    const bool freePlusR133=kcd2vr::free_targeting_d42::enabled.load(std::memory_order_acquire)&&
                        kcd2vr::free_targeting_d42::mode()==r::FreePlus;
                    const double offsetR133=freePlusR133?r::offsetView.load(std::memory_order_acquire):0.0;
                    if(std::fabs(offsetR133)>r::kDoneRad)movementYawR134+=offsetR133;
                    if(std::fabs(movementYawR134)>r::kDoneRad&&(std::fabs(pad.lx)>.01f||std::fabs(pad.ly)>.01f)){
                        r::rotateStick(pad.lx,pad.ly,movementYawR134);
                        if(freePlusR133&&std::fabs(offsetR133)>r::kDoneRad)r::stickFrames.fetch_add(1,std::memory_order_relaxed);}
                    const int signR133=freePlusR133?r::bleedSign.load(std::memory_order_acquire):0;
                    if(signR133&&std::fabs(pad.rx)<.05f&&!r::userTurning(GetTickCount64())){
                        pad.rx=r::bleedRx(offsetR133,signR133);r::bleedFrames.fetch_add(1,std::memory_order_relaxed);}
                }
            }
            ApplyRoomscalePad58(pad,padCtx,xr_head_valid&&palmInput36.gameplay); // Fresh XR validity and this frame's gameplay owner.
            {const auto quickNowR112=std::uint64_t(GetTickCount64()); // R112: quick-access D-pad (left weapon, right food) = selector change
             if(pad.dpad_left)ui_hud_r112::weaponQuickInputMs.store(quickNowR112,std::memory_order_release);
             if(pad.dpad_right)ui_hud_r112::foodQuickInputMs.store(quickNowR112,std::memory_order_release);}
            native_input_look_lab::PublishGamepadState(
                pad.lx,pad.ly,pad.rx,pad.ry,pad.lt,pad.rt,
                pad.a,pad.b,pad.x,pad.y,pad.lb,pad.rb,pad.l3,pad.r3,pad.start,pad.back,
                pad.dpad_up,pad.dpad_down,pad.dpad_left,pad.dpad_right);
            g_stallCp178r30[4]=std::uint64_t(GetTickCount64()); // R30 checkpoint: pad published
            {const auto now=std::uint64_t(GetTickCount64());if(activity163.nativeControls&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){
                auto& w=activity_input163::witness175;const auto alch=w.active[unsigned(activity_input163::Kind175::Alchemy)].load(),smith=w.active[unsigned(activity_input163::Kind175::Smithing)].load();
                const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();static diagnostics::interaction_receipt59c10::Gate receipt;
                if((alch||smith)&&receipt.observe(session,now,{alch?alch:smith,unsigned(w.lifetime175.load()),w.SmithPhaseC1(),(pad.a?1:0)|(pad.b?2:0)|(pad.x?4:0)|(pad.rt>.5f?8:0)},0,16384,500)){
                    char line[600]{};std::snprintf(line,sizeof(line),"C71 F5_CRAFT_PAD_PUBLISHED session=%llu tick=%llu instance=%llX life=%llu phase=%d a=%d b=%d x=%d rt=%.5f lx=%.5f ly=%.5f nativeXI=%d source=PublishGamepadState nativeActionAcceptance=UNOBSERVED",
                        (unsigned long long)session,(unsigned long long)now,(unsigned long long)(alch?alch:smith),(unsigned long long)w.lifetime175.load(),w.SmithPhaseC1(),pad.a,pad.b,pad.x,pad.rt,pad.lx,pad.ly,native_xi_full);diagnostic_trace::Write(line);
                }
            }}

            // The native dispatcher must flush XI on its own input thread. F24 is
            // consumed inside our detour and never reaches KCD2 as a keyboard key.
            const auto hbNow=static_cast<std::uint64_t>(GetTickCount64());
            if(hbNow-native_xi_heartbeat_ms>=16ull){
                native_xi_heartbeat_ms=hbNow;native_input_look_lab::ArmTransportHeartbeat();
                INPUT in[2]{};in[0].type=INPUT_KEYBOARD;in[0].ki.wVk=VK_F24;
                in[1].type=INPUT_KEYBOARD;in[1].ki.wVk=VK_F24;in[1].ki.dwFlags=KEYEVENTF_KEYUP;
                (void)SendInput(2,in,sizeof(INPUT));
            }
        } else {
            semiWorkstations46.revoke(std::uint64_t(GetTickCount64()));
            frik_pose_probe::PublishQuickTalkOwnerH4(0,GetTickCount64());
            native_xi_heartbeat_ms=0;
            native_vr_pad_premap::Reset();
        }
        touch_bindings_.SetNativeGamepadAxes(
            native_xi_full&&native_input_look_lab::NativeLeftStickReady(),
            native_xi_full&&native_input_look_lab::NativeRightStickReady());
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64())){
            static std::uint64_t previousSession168=0,previousKey168=~0ull;
            const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
            const auto key=std::uint64_t(native_xi_full)|(std::uint64_t(activity163.nativeControls)<<1)|
                (std::uint64_t(mis_menu_after_frame)<<2)|(std::uint64_t(ui89_policy.BlocksWorldInteraction())<<3)|
                (std::uint64_t(immersive_cursor_menu)<<4)|(std::uint64_t(ui88_input_result.blocks_gameplay)<<5)|
                (std::uint64_t(dialogue_active)<<6)|(std::uint64_t(ui88_input_result.hand+1)<<8)|
                (std::uint64_t(frik_pose_probe::CombatCensusMode())<<12)|(std::uint64_t(frik_pose_probe::WeaponControlMode())<<16);
            if(session!=previousSession168||key!=previousKey168){previousSession168=session;previousKey168=key;char m[512]{};
                std::snprintf(m,sizeof(m),"V66.168 INPUT_OWNER session=%llu XI=%d activityNative=%d vrMenu=%d nativeUIBlocksWorld=%d cursorMenu=%d menuAdapterBlocks=%d dialogue=%d pointerHand=%d F10=%d F12=%d rawAxes=UNCHANGED_BY_DIAGNOSTIC",
                    (unsigned long long)session,native_xi_full,activity163.nativeControls,mis_menu_after_frame,ui89_policy.BlocksWorldInteraction(),immersive_cursor_menu,ui88_input_result.blocks_gameplay,dialogue_active,ui88_input_result.hand,frik_pose_probe::CombatCensusMode(),frik_pose_probe::WeaponControlMode());diagnostic_trace::Write(m);
            }
        }
        touch_bindings_.SetNativeSpecialRoute168(true);
        frik_pose_probe::SetNativeSpecialContext168(!mis_menu_after_frame&&!activity163.nativeControls&&
            !ui89_policy.BlocksWorldInteraction()&&!immersive_cursor_menu&&!ui88_input_result.blocks_gameplay&&
            !dialogue_active&&!quick_talk_native_h19::Open()&&!action_map_probe::BackContext());
        // A world-texture book being visible does not prove reading mode.
        // The native XI map, when available, selects the actual activity actions.
        touch_bindings_.SetAuthoredMinigameContext(false,false);
        touch_bindings_.SetNativeMinigameOwnership162r1(activity163.nativeControls);

        if(native_xi_full&&!native_xi_active_prev){
            native_xi_active_prev=true;native_xi_degraded_logged=false;
            diagnostic_trace::Write("V66.96-INPUT ACTIVE transport=NATIVE_XI_GAMEPAD fallback=OFF companionPAK=NONE");
        } else if(!native_xi_full&&native_xi_discovery_attempts>=2&&!native_xi_degraded_logged){
            native_xi_active_prev=false;native_xi_degraded_logged=true;
            diagnostic_trace::Write("V66.96-INPUT DEGRADED transport=SCAN_CODE_FALLBACK nativeXI=NOT_FULL_READY after bounded discovery");
        }

        const bool game_flat_menu_context = policy_theater_scene;
        TouchBindings::Events touch_events{};
        {float movementHmdYaw134=0.f,movementHmdPitch134=0.f;
         const bool movementHmdYawValid134=xr_head_valid&&CameraHook::GetHeadAimLocalAngles(movementHmdYaw134,movementHmdPitch134);
         touch_bindings_.SetHmdWalkingDirection(g_hmd_walking_direction134.load(std::memory_order_acquire),movementHmdYaw134,movementHmdYawValid134);}
        if(ui88_input_result.blocks_gameplay&&routeL.menu_down)touch_bindings_.ConsumeMenuUntilRelease174r2();
        if(ui88_input_result.blocks_gameplay)touch_bindings_.QueueInventoryR118(false); // R118: no gameplay Update this frame = no late Inventory
        if(!ui88_input_result.blocks_gameplay){
            // V66.139: physical RT ownership was already resolved before both
            // native-XI and fallback transport.  Do not apply a second, broader
            // melee-wide trigger suppression here.
            touch_events=touch_bindings_.Update(routeL,routeR,mis_menu_after_frame,false,
                immersive_cursor_menu&&!mis_menu_after_frame,
                (dialogue_active||quick_talk_native_h19::Open()||action_map_probe::BackContext()||ui89_policy.NativeContextControls())&&!mis_menu_after_frame);
        }

        frik_pose_probe::ServiceKick26(std::uint64_t(GetTickCount64())); // C26: native kick clip seen? style 1 draws our leg (two atomic loads when idle)
        {   // R59.16C2: an R3 kick that could not be sent says why (once per expired request).
            static std::uint64_t lastKickDrop59c2=0;int reason=0;std::uint64_t tick=0;
            if(frik_pose_probe::KickDrop59C2(reason,tick)&&tick!=lastKickDrop59c2){lastKickDrop59c2=tick;
                static const char* const names[]{"NONE","HELD_WINDUP","STROKE_ACTIVE","RHYTHM_OR_NATIVE_ATTACK_BUSY","COMMAND_BUFFER_FULL","STALE_PRESS","OWNER_REPLACED"};
                char m[200]{};std::snprintf(m,sizeof(m),"R59.16C2 R3_KICK59 stage=DROPPED reason=%s pendingMs=600 guard=NOT_A_BLOCKER",
                    reason>=0&&reason<=6?names[reason]:"UNKNOWN");diagnostic_trace::Write(m);}
        }
        touch_events.toggle_pause_comfort|=pauseChord58c2;
        touch_events.back_cancel|=backEscapeM1;
        if(touch_events.special_attack_requested){
            // U16: honor the KICKS page. Exactly one semantic producer per R3.
            const bool procedural=frik_pose_probe::CombatExperiment175Enabled()&&
                kcd2vr::kick26::Current()==kcd2vr::kick26::Style::VrLegOnly;
            // H35: with EMPTY hands the kick is the fist lane's own prepare/release with zone 5 (the proven 14 Oct chain); the
            // special-attack ingress stays the route for everything else (armed R3 unchanged).
            const int kickHandH35=kcd2vr::kick26::ForcedSide()<0?0:kcd2vr::kick26::ForcedSide();
            const auto kickResultH35=procedural?frik_pose_probe::FistKickResultH35::NotThisLane:frik_pose_probe::RequestFistKickStatusH35(kickHandH35);
            const bool fistKickH35=kickResultH35==frik_pose_probe::FistKickResultH35::Queued;
            const bool queued=fistKickH35?true:kickResultH35==frik_pose_probe::FistKickResultH35::Refused?false:procedural?frik_pose_probe::RequestVrLegKick34():frik_pose_probe::RequestNativeSpecial168();
            if(queued&&!fistKickH35)frik_pose_probe::NoteKickRequest26(!procedural);
            diagnostic_trace::Write(fistKickH35?"U16 R3_KICK route=FIST_ZONE5_NATIVE (H35: empty hands, prepare/release lane, game selects the kick row)":procedural?(queued?
                "U16 R3_KICK route=VR_LEG_CONTACT applications<=1 cost=NATIVE_IF_TEST_ENABLED":
                "U16 R3_KICK route=VR_LEG_CONTACT state=REFUSED context_or_busy=1"):
                (queued?"U16 R3_KICK route=NATIVE_SPECIAL selection=NATIVE_NOT_INFERRED":"U16 R3_KICK route=NATIVE_SPECIAL state=REFUSED"));
        }

        if(touch_events.back_cancel) {
            if(clean_menu_owner==CleanMenuOwner::InventoryMap){inventory_close_pending=true;inventory_close_absent_since_ms=0;}
            diagnostic_trace::Write("V66.86-MENU B/BACK -> native Escape/Back; Inventory/Map waits for native close ACK then hard-releases Theater+laser");
        }
        if(touch_events.toggle_pause_comfort) {
            if(clean_menu_owner==CleanMenuOwner::None)ui88_router.Request(CleanMenuOwner::Pause);
            clean_menu_owner=ui88_router.Current();
            pause_flat_ui=clean_menu_owner==CleanMenuOwner::Pause; inventory_flat_ui=false; map_flat_ui=false;
            diagnostic_trace::Write(pause_flat_ui ? "V66.97-PAUSE REQUEST -> native pause + immersive overlay (no Theater)" : "V65.29-PAUSE -> IMMERSIVE");
        }
#define KCD2VR_M2_HOST_SLOT 5021 // M2_SOURCE_SELECTOR
#include "subsystems/vrmap/host.inl" // M2_OWNED_SOURCE slot=5021
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
        // UI health is transition-only; removed the 3-second global input spam.
        // V64.23 FIX: was (safe_ui_fit || cursor_menu_flat_ui) - cursor_menu_flat_ui was removed
        // from driving actual theater mode back in V64.17 (cooking/chest/loot/alchemy now
        // correctly stay in the normal immersive view), but this stick-navigation code still
        // treated it as "theater is showing," hijacking both sticks into arrow-key menu
        // navigation while the player was still looking at normal gameplay. Only safe_ui_fit
        // (real theater/flat-screen presentation) should redirect the sticks away from movement.
        if (touch_events.toggle_mis_menu && vr_menu_) {
            // R109 (ChatGPT review of the SteamVR log): when the menu swapchain does not exist (format refused) an
            // opened VR menu is invisible in the headset but still takes the sticks/buttons = "no controls". Don't
            // open it then (desktop mirror users still can), say so in the log; closing always works.
            if(!vr_menu_->IsVisible()&&xr_manager_&&!xr_manager_->MenuSurfaceReadyR109()&&!mirror_mis_menu_to_desktop_.load(std::memory_order_acquire)){
                static std::uint64_t nextR109=0;const auto nowR109=std::uint64_t(GetTickCount64());
                if(nowR109>=nextR109){nextR109=nowR109+5000;diagnostic_trace::WriteStatus("R109 VR_MENU not opened: the menu swapchain does not exist (see R108 XR_FORMATS) - it would be invisible and eat the controls");}
            }else{
                vr_menu_->Toggle();
                debug_trace::Write(vr_menu_->IsVisible() ? "R113 VR menu opened (quick-access modifier+LS-Down or L3+R3)" : "R113 VR menu closed (quick-access modifier+LS-Down or L3+R3)");
            }
        }
        if (!matrix_active && touch_events.toggle_inventory_comfort) {
            suppress_inventory_key_fallback_until_ms=static_cast<std::uint64_t>(GetTickCount64())+300ull;
            if(clean_menu_owner==CleanMenuOwner::InventoryMap){
                // TouchBindings already sent the native Inventory toggle. Keep Theater and
                // its pointer/input owner until native Inventory/Map absence is observed.
                inventory_close_pending=true;
                inventory_close_absent_since_ms=0;
                diagnostic_trace::Write("V66.68-INVENTORY Menu close REQUEST -> KEEP THEATER until native close ACK");
            } else {
                inventory_close_pending=false; inventory_close_absent_since_ms=0;
                ui88_router.Request(CleanMenuOwner::InventoryMap);clean_menu_owner=ui88_router.Current();
                inventory_flat_ui=true; map_flat_ui=false; pause_flat_ui=false; cursor_menu_apse_hub=false;
                diagnostic_trace::Write("V66.68-INVENTORY Menu open REQUEST -> INPUT_PENDING; Theater waits for native visibility");
            }
        }
    }

    // Restore automatic presentation center state on shutdown.
    if(f8_auto_projection_active){
        // V61.34+ calibration contract: F8 does not own or restore game/source FOV.
        // It restores the pre-F8 presentation Center-X/Y on shutdown.
        vr_presentation_center_x_.store(f8_restore_center_x,std::memory_order_release);
        vr_presentation_center_y_.store(f8_restore_center_y,std::memory_order_release);
        if(xr_manager_) { xr_manager_->SetPresentationCenterX(f8_restore_center_x); xr_manager_->SetPresentationCenterY(f8_restore_center_y); }
        vr_auto_projection_active_.store(false,std::memory_order_release);
    }


    diagnostics::perf_d61::gate.select(false,diagnostics::perf_d61::nowMs()); // stop timed producers before teardown

    // If shutdown interrupts the 40-second stereo ladder, restore temporary CryEngine
    // CVars before renderer/OpenXR teardown whenever the console is still alive.
    if (matrix_active || matrix_original_cvars.valid) {
        restore_safe_mono();
        matrix_active = false;
        matrix_running_for_ui_.store(false, std::memory_order_release);
    }

    CameraHook::SetNativeHeadRotationOwner(false);
    FrameSync::SetHeadOwnershipFixEnabled(false);
    FrameSync::SetHeadOwnershipProbeLevel(0);
    (void)FrameSync::SetPipelineProbeEnabled(false);
    CameraHook::SetAngularDiagnosticsEnabled(false);
    // C31: bounded native-owner restoration before console/hook teardown.
    g_vr_shadows_state26.RearmRestore();g_eye_adapt26.RearmRestore(); // one explicit shutdown retry budget
    for(unsigned attempt31=0;attempt31<3;++attempt31){const auto tick31=GetTickCount64()+attempt31*500ull;
        g_vr_shadows_state26.Service(false,tick31,g_graphics_io31);
        g_eye_adapt26.Service(false,false,false,{},tick31,g_graphics_io31);}
    g_combat_lock178r13.Restore(); // V66.178R13: give the camera-lock cvars back on exit
    g_combat_camera50.Restore();g_combat_cvars51.Restore();g_combat_cvars52.Restore();g_block_cooldown59c17.Restore();g_free_swing38.Restore(); // V66.178R51/R52 // V66.178R50C1 // R59.16C10R17
    {CombatWindowsConsoleH3 ioH3;g_router_windows_h3.rearmRestore();for(unsigned retry=0;retry<3&&g_router_windows_h3.owns();++retry)
        g_router_windows_h3.service(false,GetTickCount64()+retry*250,ioH3,[](const char*,const char*,float,float){});
     if(g_router_windows_h3.owns())diagnostic_trace::Write("H3 WINDOW shutdownRestore=UNCONFIRMED consoleUnavailableOrWriteFailed=1");}
    {CombatWindowsConsoleH3 ioD70;g_dodge_window_d70.rearmRestore();
     for(unsigned retry=0;retry<3&&g_dodge_window_d70.owns();++retry)
        g_dodge_window_d70.service(false,GetTickCount64()+retry*250,ioD70,[](const char*,const char*,float,float){});
     if(g_dodge_window_d70.owns())diagnostic_trace::Write("D70 DODGE_WINDOW shutdownRestore=UNCONFIRMED");}
    native_upscaler_stereo_d82::Stop();
    if(f8_native_vr_initialized) native_vr_control_plane_lab::Shutdown();
    if(f8_lab_initialized) native_stereo_outer_lab::Shutdown();
    animation_filter::SetDialogueCameraSuppression(false);
    dialogue_camera_override::SetForceNoOverride(false);
    ApplyDialogueLookAssist(f5_look_original,false);
    CameraHook::SetDialogueLabState(-1,false,0);
    npc_lookat_hmd::SetEnabled(false);
    dialogue_camera_override::SetDialogueActive(false);
    dialogue_camera_override::SetOverrideType(0);
    if(dialogue_original.valid) {
        (void)ApplyDialogueMode(dialogue_original,0);
        diagnostic_trace::Write("V64.3-DIALOGUE shutdown: original native dialogue camera CVars restored");
    }

    // V46.3 quality profile is runtime-only. Restore exactly what was present
    // before MIS touched the ordinary CryEngine graphics CVars.
    if (vr_graphics_applied && vr_graphics_original.valid) {
        char msg[512]{};
        const bool restored = RestoreVrGraphics(vr_graphics_original, msg, sizeof(msg));
        if (msg[0]) debug_trace::Write(msg);
        if (!restored) debug_trace::Write("V46.3-GRAPHICS restore failed during shutdown");
        graphics_profile_applied_for_ui_.store(false, std::memory_order_release);
    }

    touch_bindings_.Reset();

    // Stop new render-thread entries and wait for any callback already inside
    // XrManager before destroying OpenXR/D3D12 state.
    DeviceHook::SetRenderTickCallback(nullptr);
    DeviceHook::WaitForRenderCallbacksIdle();
    if (g_timer_period_1ms_59c7a) { timeEndPeriod(1); g_timer_period_1ms_59c7a = false; } // R59.16C7A
    if (vr_menu_) {
        vr_menu_->Shutdown();
        vr_menu_.reset();
    }

    // Order matters: XrManager must be fully torn down (it holds its own
    // AddRef'd reference to the game's device/queue) BEFORE DeviceHook
    // releases its own reference.
    xr_manager_->Shutdown();
    xr_manager_ = nullptr; // singleton - clear the pointer, do not delete


    (void)ApplyF7PromptPresentation(false,vr_pad_glyph_style_.load(std::memory_order_acquire));
    native_vr_pad_premap::Reset();
    native_input_look_lab::SetGamepadMode(0);
    if(combat_diag_probe_installed) combat_diag_probe::Uninstall();
    npc_unarmed_spacing::Shutdown();
    npc_defense::Shutdown();
    npc_difficulty::Shutdown();
    if(action_map_probe_installed) action_map_probe::Uninstall();
    ui_menu_input::Uninstall();
    ui_menu_camera59c2::Uninstall();
    immersive_fov59c7g::Uninstall();
    action_time_owner59c11::Uninstall(); // R59.16C10R11: restores Henry's anim-event callback and any held speed bias before the animation observer goes
    activity_lens59c10r1::Uninstall();
    ui_native_state::Uninstall();
    shop_state_probe::Uninstall();
    native_taa_pair_epoch_v6661::Uninstall();
    native_postaa_repair_v6632::Uninstall();
    kcd2vr::alchemy_finish_cb1::Uninstall();
    prompt_device_lock::Uninstall();
    activity_fader47c1::Uninstall();
    npc_lookat_hmd::Uninstall();
    dialogue_camera_override::Uninstall();
    // Bounded shutdown restoration, with foreign-writer protection.
    for(unsigned i=0;i<3&&focusZoom58c2.owned;++i)(void)focusZoom58c2.step(false,GetTickCount64()+i*500,focusZoomIO58c2);
    (void)activityFade123.Apply(false);
    (void)pauseBlur124.Apply(pauseBlurIo124,false);
    interaction_alignment::Uninstall();
    native_weapon_body_t4::SetEnabled(false);
    native_weapon_body_t4::Cancel();
    native_physics_phase_t4::RequestShutdown();
    if(vrik_probe_installed) frik_pose_probe::Uninstall();
    if(animation_filter_installed) animation_filter::Uninstall();
    flashui_probe::UpdateCombatCrosshairR112(false,GetTickCount64()); // R112: native crosshair size back
    flashui_probe::UpdateCombatCrossArtR125(false,GetTickCount64()); // R125: native cross art back
    flashui_probe::UpdateQuickSlotFadeR112(false,false,GetTickCount64()+1000); // R112: native selector alpha back
    (void)flashui_probe::SetHudRepair175(false);
    {AdaptiveHudBackend57c1 io;g_adaptive_hud57c1.Update(io,false,~std::uint64_t{0});}
    {CVarLeaseBackend59c16 io{"wh_ui_ShowHints"};g_show_hints_d80.Update(io,false,true,~std::uint64_t{0});}
    {CVarLeaseBackend59c16 io{"wh_pl_ShowFireCursor"};g_fire_cursor59c16.Update(io,false,~std::uint64_t{0});} // R59.16C10R16
    if(g_ragdoll59.On()||g_ragdoll_rung19c4r4.load())ApplyRagdoll59(false);
    flashui_probe::Uninstall();
    activity_input163::UninstallNative175();
    RecursiveViewProbe::Uninstall();
    FrameSync::Uninstall();
    stereo_archaeology::Shutdown();

    device_hook_->Uninstall();
    device_hook_.reset();

    native_input_look_lab::Uninstall();
    camera_hook_->Uninstall();
    camera_hook_.reset();

    FlushMenuSave("RUN_EXIT"); // C25: let an in-flight INI write finish (bounded 3 s) before the log closes
    (void)subsystems::StopCatalog();
    KCD2VR_DIAG_LOG(Bug,info,"KCD2VR shut down cleanly");
    debug_trace::Write("KCD2VR shut down cleanly");
    // Native producers are detached. Four bounded passes empty the copied F7
    // ring (31 pending / 8 per pass); each pass also drains up to 64 F3 events.
    for(unsigned pass=0;pass<4;++pass){
        frik_pose_probe::DrainCopiedDiagnosticsH47(std::uint64_t(GetTickCount64()));
        diagnostics::physics_d42::drain(std::uint64_t(GetTickCount64()),[](auto domain,std::uint64_t session,const char* line){diagnostic_trace::WriteReceiptFor(domain,session,line);});
    }
    diagnostics::physics_d42::drain(std::uint64_t(GetTickCount64()),[](auto domain,std::uint64_t session,const char* line){diagnostic_trace::WriteReceiptFor(domain,session,line);},true);
    diagnostics::interaction_d42::capture.end(diagnostic_trace::Write);
    {const auto endD42=std::uint64_t(GetTickCount64());
        semiWorkstations46.alchemyStagesD42.stop(endD42,"SHUTDOWN",diagnostic_trace::Write);
        char lineD42[512]{};
        if(semiWorkstations46.forgeStagesD42.close(endD42,"SMITH_BELLOWS","SHUTDOWN",lineD42,sizeof(lineD42)))diagnostic_trace::Write(lineD42);}

    diagnostic_trace::SetEnabled(false); // flush queued records only after native hooks are detached
    log().shutdown();
}

void Mod::Shutdown()
{
    // C26 (Codex C4R4C1 review): DllMain/loader lock - never wait for or run an INI write here.
    // RUN_EXIT (run thread) already waited; a request after it is dropped, not written inline.
    g_menu_save_detached26.store(true,std::memory_order_release);g_menu_save25.Close();
    held_map_m1::Shutdown();
    native_script_t2::Shutdown();
    running_ = false;
}

} // namespace kcd2vr
