// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 6
// src/mod.cpp: fillmenustate_01.inl
// M2_SLOT_BEGIN 6
    out.native_head_look = enable_native_head_aim_.load(std::memory_order_acquire);
// M2_SLOT_END 6
#elif KCD2VR_M2_HOST_SLOT == 12
// src/mod.cpp: fillmenustate_02.inl
// M2_SLOT_BEGIN 12
    out.interaction_center_test = vr_interaction_center_test_.load(std::memory_order_acquire);
// M2_SLOT_END 12
#elif KCD2VR_M2_HOST_SLOT == 15
// src/mod.cpp: fillmenustate_03.inl
// M2_SLOT_BEGIN 15
    out.vr_ui_interaction_menu_scale = vr_ui_interaction_menu_scale_.load(std::memory_order_acquire);
    out.vr_ui_interaction_menu_x = vr_ui_interaction_menu_x_.load(std::memory_order_acquire);
    out.vr_ui_interaction_menu_y = vr_ui_interaction_menu_y_.load(std::memory_order_acquire);
// M2_SLOT_END 15
#elif KCD2VR_M2_HOST_SLOT == 23
// src/mod.cpp: fillmenustate_04.inl
// M2_SLOT_BEGIN 23
    out.first_person_activities = first_person_activities_.load(std::memory_order_acquire);
// M2_SLOT_END 23
#elif KCD2VR_M2_HOST_SLOT == 44
// src/mod.cpp: applymenustate_01.inl
// M2_SLOT_BEGIN 44
    const float ui_interaction_scale = std::clamp(state.vr_ui_interaction_menu_scale,0.45f,1.10f);
    const float ui_interaction_x = std::clamp(state.vr_ui_interaction_menu_x,-700.0f,700.0f);
    const float ui_interaction_y = std::clamp(state.vr_ui_interaction_menu_y,-450.0f,450.0f);
// M2_SLOT_END 44
#elif KCD2VR_M2_HOST_SLOT == 54
// src/mod.cpp: applymenustate_02.inl
// M2_SLOT_BEGIN 54
    vr_interaction_center_test_.store(state.interaction_center_test,std::memory_order_release);
// M2_SLOT_END 54
#elif KCD2VR_M2_HOST_SLOT == 56
// src/mod.cpp: applymenustate_03.inl
// M2_SLOT_BEGIN 56
    vr_ui_interaction_menu_scale_.store(ui_interaction_scale,std::memory_order_release);
    vr_ui_interaction_menu_x_.store(ui_interaction_x,std::memory_order_release);
    vr_ui_interaction_menu_y_.store(ui_interaction_y,std::memory_order_release);
// M2_SLOT_END 56
#elif KCD2VR_M2_HOST_SLOT == 66
// src/mod.cpp: applymenustate_04.inl
// M2_SLOT_BEGIN 66
    first_person_activities_.store(state.first_person_activities, std::memory_order_release);
// M2_SLOT_END 66
#elif KCD2VR_M2_HOST_SLOT == 69
// src/mod.cpp: applymenustate_05.inl
// M2_SLOT_BEGIN 69
    enable_native_head_aim_.store(false, std::memory_order_release); // V66.163 retired, including stale menu state
// M2_SLOT_END 69
#elif KCD2VR_M2_HOST_SLOT == 93
// src/mod.cpp: savemenusettings_01.inl
// M2_SLOT_BEGIN 93
    ok = wb("VrInteractionCenterTest", vr_interaction_center_test_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrInteractionAimRadiusMeters", vr_interaction_radius_m_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrInteractionAimConeHalfAngleDegrees", vr_interaction_cone_half_angle_deg_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrInteractionCenterOffsetX", vr_interaction_center_offset_x_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrInteractionCenterOffsetY", vr_interaction_center_offset_y_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 93
#elif KCD2VR_M2_HOST_SLOT == 95
// src/mod.cpp: savemenusettings_02.inl
// M2_SLOT_BEGIN 95
    ok = wf("VrUiInteractionMenuScale", vr_ui_interaction_menu_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiInteractionMenuX", vr_ui_interaction_menu_x_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiInteractionMenuY", vr_ui_interaction_menu_y_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 95
#elif KCD2VR_M2_HOST_SLOT == 104
// src/mod.cpp: savemenusettings_03.inl
// M2_SLOT_BEGIN 104
    ok = wb("FirstPersonActivities", first_person_activities_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 104
#elif KCD2VR_M2_HOST_SLOT == 107
// src/mod.cpp: savemenusettings_04.inl
// M2_SLOT_BEGIN 107
    ok = wb("EnableNativeHeadAim", enable_native_head_aim_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 107
#elif KCD2VR_M2_HOST_SLOT == 126
// src/mod.cpp: loadconfig_01.inl
// M2_SLOT_BEGIN 126
    settings.bind("VrInteractionCenterTest", vr_interaction_center_test_, false);
    settings.bind("VrInteractionAimRadiusMeters", vr_interaction_radius_m_, 0.21f);
    settings.bind("VrInteractionAimConeHalfAngleDegrees", vr_interaction_cone_half_angle_deg_, 10.0f);
    settings.bind("VrInteractionCenterOffsetX", vr_interaction_center_offset_x_, 0.0f);
    settings.bind("VrInteractionCenterOffsetY", vr_interaction_center_offset_y_, 0.0f);
// M2_SLOT_END 126
#elif KCD2VR_M2_HOST_SLOT == 128
// src/mod.cpp: loadconfig_02.inl
// M2_SLOT_BEGIN 128
    settings.bind("VrUiInteractionMenuScale", vr_ui_interaction_menu_scale_, 0.73f);
    settings.bind("VrUiInteractionMenuX", vr_ui_interaction_menu_x_, -380.0f);
    settings.bind("VrUiInteractionMenuY", vr_ui_interaction_menu_y_, -300.0f);
// M2_SLOT_END 128
#elif KCD2VR_M2_HOST_SLOT == 133
// src/mod.cpp: loadconfig_03.inl
// M2_SLOT_BEGIN 133
    settings.bind("FirstPersonActivities", first_person_activities_, true);
// M2_SLOT_END 133
#elif KCD2VR_M2_HOST_SLOT == 163
// src/mod.cpp: loadconfig_04.inl
// M2_SLOT_BEGIN 163
    vr_interaction_radius_m_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrInteractionAimRadiusMeters",0.21f),0.03f,0.30f),std::memory_order_relaxed);
    vr_interaction_cone_half_angle_deg_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrInteractionAimConeHalfAngleDegrees",10.0f),0.0f,18.0f),std::memory_order_relaxed);
    vr_interaction_center_offset_x_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrInteractionCenterOffsetX",0.0f),-0.25f,0.25f),std::memory_order_relaxed);
    vr_interaction_center_offset_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrInteractionCenterOffsetY",0.0f),-0.25f,0.25f),std::memory_order_relaxed);
// M2_SLOT_END 163
#elif KCD2VR_M2_HOST_SLOT == 166
// src/mod.cpp: loadconfig_05.inl
// M2_SLOT_BEGIN 166
    vr_ui_interaction_menu_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiInteractionMenuScale",0.73f),0.45f,1.10f),std::memory_order_relaxed);
    vr_ui_interaction_menu_x_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiInteractionMenuX",-380.0f),-700.0f,700.0f),std::memory_order_relaxed);
    vr_ui_interaction_menu_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiInteractionMenuY",-300.0f),-450.0f,450.0f),std::memory_order_relaxed);
// M2_SLOT_END 166
#elif KCD2VR_M2_HOST_SLOT == 171
// src/mod.cpp: loadconfig_06.inl
// M2_SLOT_BEGIN 171
    first_person_activities_.store(ReadIniBoolExplicit(explicit_ini,"Settings","FirstPersonActivities",true),std::memory_order_relaxed);
    if(ReadIniIntExplicit(explicit_ini,"Settings","VrFirstPersonActivityPreferenceVersion",0)<1){
        first_person_activities_.store(true,std::memory_order_relaxed);
        if(WritePrivateProfileStringA("Settings","FirstPersonActivities","true",explicit_ini))
            WritePrivateProfileStringA("Settings","VrFirstPersonActivityPreferenceVersion","1",explicit_ini);
    }
// M2_SLOT_END 171
#elif KCD2VR_M2_HOST_SLOT == 174
// src/mod.cpp: loadconfig_07.inl
// M2_SLOT_BEGIN 174
    enable_native_head_aim_.store(false,std::memory_order_relaxed);
// M2_SLOT_END 174
#elif KCD2VR_M2_HOST_SLOT == 222
// src/mod.h: mod_state_01.inl
// M2_SLOT_BEGIN 222
    std::atomic<bool> vr_interaction_center_test_{false}; // V66.146 opt-in head-ray assist; native/free-look is default
    std::atomic<float> vr_interaction_radius_m_{0.21f};   // V64.3 base candidate radius; native reach preserved
    std::atomic<float> vr_interaction_cone_half_angle_deg_{10.0f}; // V64.3 angular expansion with distance
    std::atomic<float> vr_interaction_center_offset_x_{0.0f}; // F5-only source-center correction; does not move presentation
    std::atomic<float> vr_interaction_center_offset_y_{0.0f};
// M2_SLOT_END 222
#elif KCD2VR_M2_HOST_SLOT == 227
// src/mod.h: mod_state_02.inl
// M2_SLOT_BEGIN 227
    std::atomic<float> vr_ui_interaction_menu_scale_{0.73f}; // APSE cooking/loot/chest native immersive UI
    std::atomic<float> vr_ui_interaction_menu_x_{-380.0f};
    std::atomic<float> vr_ui_interaction_menu_y_{-300.0f}; // native APSE panel Y offset in design pixels
// M2_SLOT_END 227
#elif KCD2VR_M2_HOST_SLOT == 232
// src/mod.h: mod_state_03.inl
// M2_SLOT_BEGIN 232
    std::atomic<bool> first_person_activities_{true}; // companion current-retail Mannequin camera patch; restart required
// M2_SLOT_END 232
#elif KCD2VR_M2_HOST_SLOT == 235
// src/mod.h: mod_state_04.inl
// M2_SLOT_BEGIN 235
    std::atomic<bool> enable_native_head_aim_{false};
// M2_SLOT_END 235
#elif KCD2VR_M2_HOST_SLOT == 255
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 255
    bool interaction_center_test = false;  // V64.0 F5 read/write state shown in menu state
// M2_SLOT_END 255
#elif KCD2VR_M2_HOST_SLOT == 259
// src/vr_menu.h: menu_state_02.inl
// M2_SLOT_BEGIN 259
    float vr_ui_interaction_menu_scale = 0.73f;
    float vr_ui_interaction_menu_x = -380.0f;
    float vr_ui_interaction_menu_y = -300.0f;
// M2_SLOT_END 259
#elif KCD2VR_M2_HOST_SLOT == 264
// src/vr_menu.h: menu_state_03.inl
// M2_SLOT_BEGIN 264
    bool first_person_activities = false; // restart-required companion Mannequin camera patch
// M2_SLOT_END 264
#elif KCD2VR_M2_HOST_SLOT == 287
// src/vr_menu.h: menu_state_04.inl
// M2_SLOT_BEGIN 287
    int   exp_action_owner = 2;             // VrActionTimeOwner 0 off / 1 observe / 2 own (R59.16C10R11, ACTIONS page)
    float exp_action_hold_at = 0.50f;       // VrActionHoldAt: hold point of an owned action (normalized clip time; 0.50 = the apple is already in the hand)
// M2_SLOT_END 287
#elif KCD2VR_M2_HOST_SLOT == 5013
// src/mod.cpp: activity_focus_zoom_backend
// M2_SLOT_BEGIN 5013
struct FocusZoomConsole58c2 {
    bool get(float& value) noexcept {
        if(!retail_build112::Ready(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"))))return false;
        void* c=SafeGetCryConsole();void* v=c?SafeConsoleGetCVar(c,"wh_pl_FocusFOVMaxDecrease"):nullptr;
        if(!v)return false;value=SafeCVarGetFloat(v,-1);return std::isfinite(value)&&value>=0&&value<=90;
    }
    bool set(float value) noexcept {
        if(!retail_build112::Ready(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"))))return false;
        void* c=SafeGetCryConsole();void* v=c?SafeConsoleGetCVar(c,"wh_pl_FocusFOVMaxDecrease"):nullptr;
        return v&&SafeCVarSetFloat(v,value);
    }
};
// M2_SLOT_END 5013
#elif KCD2VR_M2_HOST_SLOT == 5014
// src/mod.cpp: activity_fade_backend
// M2_SLOT_BEGIN 5014
struct ActivityFade123R1 {
    struct Entry {const char* name;float base=0;bool owned=false;};
    Entry entries[6]={{"wh_pl_HerbGatheringFadeLength"},{"wh_pl_HerbGatheringFadeInLength"},
        {"wh_pl_HerbGatheringFadeOutLength"},{"wh_pl_DiggingFaderLength"},
        {"wh_pl_ButcheringFaderLength"},{"wh_pl_AlchemyFinishFaderLength"}};
    bool Apply(bool enable) noexcept {
        void* console=SafeGetCryConsole();if(!console)return false;bool complete=true;
        for(auto& e:entries){void* cv=SafeConsoleGetCVar(console,e.name);if(!cv){complete=false;continue;}
            const float current=SafeCVarGetFloat(cv,-1.f);
            if(!std::isfinite(current)||current<0||current>60){complete=false;continue;}
            if(enable){
                if(e.owned&&std::fabs(current)>.001f)e.owned=false; // respect external ownership
                if(!e.owned){e.base=current;e.owned=true;}
                if(current>.001f&&(!SafeCVarSetFloat(cv,0.f)||std::fabs(SafeCVarGetFloat(cv,-1.f))>.001f)){
                    (void)SafeCVarSetFloat(cv,e.base);complete=false; // retain restoration ownership on failure
                }
                char line[192]{};std::snprintf(line,sizeof(line),"R47C1 ACTIVITY_TIMER name=%s base=%.4f readback=%.4f owned=%d",e.name,double(e.base),double(SafeCVarGetFloat(cv,-1.f)),int(e.owned));
                diagnostic_trace::Write(line);
            }else if(e.owned){
                if(std::fabs(current)>.001f&&std::fabs(current-e.base)>.001f){e.owned=false;continue;}
                if(!SafeCVarSetFloat(cv,e.base)||std::fabs(SafeCVarGetFloat(cv,-1.f)-e.base)>.001f){complete=false;continue;}
                e.owned=false;
            }
        }
        return complete;
    }
};
// M2_SLOT_END 5014
#else
#error Unknown M2 host contribution slot
#endif
