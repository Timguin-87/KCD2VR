// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 2
// src/mod.cpp: fillmenustate_01.inl
// M2_SLOT_BEGIN 2
    out.dialogue_text_scale = vr_dialogue_text_scale_.load(std::memory_order_acquire);
// M2_SLOT_END 2
#elif KCD2VR_M2_HOST_SLOT == 7
// src/mod.cpp: fillmenustate_02.inl
// M2_SLOT_BEGIN 7
    out.dialogue_hands59c7f=vrik_in_dialogue_;
// M2_SLOT_END 7
#elif KCD2VR_M2_HOST_SLOT == 9
// src/mod.cpp: fillmenustate_03.inl
// M2_SLOT_BEGIN 9
    out.dialogue_smooth38=CameraHook::DialogueSmooth38(); // C38
// M2_SLOT_END 9
#elif KCD2VR_M2_HOST_SLOT == 17
// src/mod.cpp: fillmenustate_04.inl
// M2_SLOT_BEGIN 17
    out.vr_ui_dialogue_scale = vr_ui_dialogue_scale_.load(std::memory_order_acquire);
// M2_SLOT_END 17
#elif KCD2VR_M2_HOST_SLOT == 19
// src/mod.cpp: fillmenustate_05.inl
// M2_SLOT_BEGIN 19
    out.vr_ui_dialogue_y = vr_ui_dialogue_y_.load(std::memory_order_acquire);
    out.vr_ui_dialogue_inset_pct = vr_ui_dialogue_inset_pct_.load(std::memory_order_acquire);
// M2_SLOT_END 19
#elif KCD2VR_M2_HOST_SLOT == 22
// src/mod.cpp: fillmenustate_06.inl
// M2_SLOT_BEGIN 22
    out.dialogue_camera_mode = vr_dialogue_camera_mode_.load(std::memory_order_acquire);
// M2_SLOT_END 22
#elif KCD2VR_M2_HOST_SLOT == 24
// src/mod.cpp: fillmenustate_07.inl
// M2_SLOT_BEGIN 24
    out.dialogue_zoom_compensation = vr_dialogue_zoom_compensation_.load(std::memory_order_acquire);
    out.dialogue_zoom_override = vr_dialogue_zoom_override_.load(std::memory_order_acquire);
    out.dialogue_fov_override = vr_dialogue_fov_override_.load(std::memory_order_acquire);
    out.dialogue_force_preview = vr_dialogue_force_preview_.load(std::memory_order_acquire);
    out.dialogue_detected = vr_dialogue_detected_.load(std::memory_order_acquire);
    out.dialogue_fov_deg = vr_dialogue_fov_deg_.load(std::memory_order_acquire);
// M2_SLOT_END 24
#elif KCD2VR_M2_HOST_SLOT == 40
// src/mod.cpp: applymenustate_01.inl
// M2_SLOT_BEGIN 40
    const float dialogue_text_scale = std::clamp(state.dialogue_text_scale, 0.50f, 1.50f);
// M2_SLOT_END 40
#elif KCD2VR_M2_HOST_SLOT == 42
// src/mod.cpp: applymenustate_02.inl
// M2_SLOT_BEGIN 42
    const int dialogue_mode = std::clamp(state.dialogue_camera_mode, 0, 2);
    const float dialogue_zoom_comp = std::clamp(state.dialogue_zoom_compensation,0.05f,2.50f);
    const float dialogue_fov = std::clamp(state.dialogue_fov_deg,70.0f,140.0f);
// M2_SLOT_END 42
#elif KCD2VR_M2_HOST_SLOT == 46
// src/mod.cpp: applymenustate_03.inl
// M2_SLOT_BEGIN 46
    const float ui_dialog_scale = std::clamp(state.vr_ui_dialogue_scale,0.30f,1.50f);
    const float ui_dialog_y = std::clamp(state.vr_ui_dialogue_y,-720.0f,720.0f);
    const float ui_dialog_inset = std::clamp(state.vr_ui_dialogue_inset_pct,0.0f,80.0f);
// M2_SLOT_END 46
#elif KCD2VR_M2_HOST_SLOT == 50
// src/mod.cpp: applymenustate_04.inl
// M2_SLOT_BEGIN 50
    const float old_ui_dialogue_scale=vr_ui_dialogue_scale_.load(std::memory_order_acquire);
// M2_SLOT_END 50
#elif KCD2VR_M2_HOST_SLOT == 58
// src/mod.cpp: applymenustate_05.inl
// M2_SLOT_BEGIN 58
    vr_ui_dialogue_scale_.store(ui_dialog_scale,std::memory_order_release);
// M2_SLOT_END 58
#elif KCD2VR_M2_HOST_SLOT == 59
// src/mod.cpp: applymenustate_06.inl
// M2_SLOT_BEGIN 59
    vr_ui_dialogue_y_.store(ui_dialog_y,std::memory_order_release);
    vr_ui_dialogue_inset_pct_.store(ui_dialog_inset,std::memory_order_release);
// M2_SLOT_END 59
#elif KCD2VR_M2_HOST_SLOT == 62
// src/mod.cpp: applymenustate_07.inl
// M2_SLOT_BEGIN 62
    vr_dialogue_text_scale_.store(dialogue_text_scale, std::memory_order_release);
// M2_SLOT_END 62
#elif KCD2VR_M2_HOST_SLOT == 65
// src/mod.cpp: applymenustate_08.inl
// M2_SLOT_BEGIN 65
    vr_dialogue_camera_mode_.store(dialogue_mode, std::memory_order_release);
// M2_SLOT_END 65
#elif KCD2VR_M2_HOST_SLOT == 67
// src/mod.cpp: applymenustate_09.inl
// M2_SLOT_BEGIN 67
    vr_dialogue_zoom_compensation_.store(dialogue_zoom_comp,std::memory_order_release);
    vr_dialogue_fov_deg_.store(dialogue_fov,std::memory_order_release);
    vr_dialogue_zoom_override_.store(state.dialogue_zoom_override,std::memory_order_release);
    vr_dialogue_fov_override_.store(state.dialogue_fov_override,std::memory_order_release);
    vr_dialogue_force_preview_.store(state.dialogue_force_preview,std::memory_order_release);
// M2_SLOT_END 67
#elif KCD2VR_M2_HOST_SLOT == 70
// src/mod.cpp: applymenustate_10.inl
// M2_SLOT_BEGIN 70
    vrik_in_dialogue_=state.dialogue_hands59c7f;
// M2_SLOT_END 70
#elif KCD2VR_M2_HOST_SLOT == 72
// src/mod.cpp: applymenustate_11.inl
// M2_SLOT_BEGIN 72
    CameraHook::SetDialogueSmooth38(state.dialogue_smooth38); // C38
// M2_SLOT_END 72
#elif KCD2VR_M2_HOST_SLOT == 97
// src/mod.cpp: savemenusettings_01.inl
// M2_SLOT_BEGIN 97
    ok = wf("VrUiDialogueScale", vr_ui_dialogue_scale_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 97
#elif KCD2VR_M2_HOST_SLOT == 99
// src/mod.cpp: savemenusettings_02.inl
// M2_SLOT_BEGIN 99
    ok = wf("VrUiDialogueY", vr_ui_dialogue_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiDialogueInsetPct", vr_ui_dialogue_inset_pct_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 99
#elif KCD2VR_M2_HOST_SLOT == 103
// src/mod.cpp: savemenusettings_03.inl
// M2_SLOT_BEGIN 103
    ok = wi("VrDialogueCameraMode", vr_dialogue_camera_mode_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 103
#elif KCD2VR_M2_HOST_SLOT == 105
// src/mod.cpp: savemenusettings_04.inl
// M2_SLOT_BEGIN 105
    ok = wf("VrDialogueZoomCompensation", vr_dialogue_zoom_compensation_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrDialogueFovDegrees", vr_dialogue_fov_deg_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrDialogueZoomOverride", vr_dialogue_zoom_override_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrDialogueFovOverride", vr_dialogue_fov_override_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 105
#elif KCD2VR_M2_HOST_SLOT == 111
// src/mod.cpp: savemenusettings_05.inl
// M2_SLOT_BEGIN 111
    ok = wb("VrikInDialogue",vrik_in_dialogue_) && ok;
// M2_SLOT_END 111
#elif KCD2VR_M2_HOST_SLOT == 122
// src/mod.cpp: savemenusettings_06.inl
// M2_SLOT_BEGIN 122
    ok = wf("VrDialogueTextScale", vr_dialogue_text_scale_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 122
#elif KCD2VR_M2_HOST_SLOT == 130
// src/mod.cpp: loadconfig_01.inl
// M2_SLOT_BEGIN 130
    settings.bind("VrUiDialogueScale", vr_ui_dialogue_scale_, 0.71f);
    settings.bind("VrUiDialogueY", vr_ui_dialogue_y_, 420.0f);
    settings.bind("VrUiDialogueInsetPct", vr_ui_dialogue_inset_pct_, 52.0f);
// M2_SLOT_END 130
#elif KCD2VR_M2_HOST_SLOT == 132
// src/mod.cpp: loadconfig_02.inl
// M2_SLOT_BEGIN 132
    settings.bind("VrDialogueCameraMode", vr_dialogue_camera_mode_, 2);
// M2_SLOT_END 132
#elif KCD2VR_M2_HOST_SLOT == 134
// src/mod.cpp: loadconfig_03.inl
// M2_SLOT_BEGIN 134
    settings.bind("VrDialogueZoomCompensation", vr_dialogue_zoom_compensation_, 0.50f);
    settings.bind("VrDialogueFovDegrees", vr_dialogue_fov_deg_, 95.0f);
    settings.bind("VrDialogueZoomOverride", vr_dialogue_zoom_override_, true);
    settings.bind("VrDialogueFovOverride", vr_dialogue_fov_override_, false);
// M2_SLOT_END 134
#elif KCD2VR_M2_HOST_SLOT == 142
// src/mod.cpp: loadconfig_04.inl
// M2_SLOT_BEGIN 142
    settings.bind("VrDialogueTextScale", vr_dialogue_text_scale_, 1.00f);
// M2_SLOT_END 142
#elif KCD2VR_M2_HOST_SLOT == 168
// src/mod.cpp: loadconfig_05.inl
// M2_SLOT_BEGIN 168
    vr_ui_dialogue_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiDialogueScale", 0.71f),0.30f,1.50f),std::memory_order_relaxed);
    vr_ui_dialogue_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiDialogueY", 420.0f),-720.0f,720.0f),std::memory_order_relaxed);
    vr_ui_dialogue_inset_pct_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiDialogueInsetPct", 52.0f),0.0f,80.0f),std::memory_order_relaxed);
// M2_SLOT_END 168
#elif KCD2VR_M2_HOST_SLOT == 170
// src/mod.cpp: loadconfig_06.inl
// M2_SLOT_BEGIN 170
    vr_dialogue_camera_mode_.store(std::clamp(ReadIniIntExplicit(explicit_ini, "Settings", "VrDialogueCameraMode", 2),0,2), std::memory_order_relaxed);
// M2_SLOT_END 170
#elif KCD2VR_M2_HOST_SLOT == 172
// src/mod.cpp: loadconfig_07.inl
// M2_SLOT_BEGIN 172
    vr_dialogue_zoom_compensation_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrDialogueZoomCompensation",0.50f),0.05f,2.50f),std::memory_order_relaxed);
    vr_dialogue_fov_deg_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrDialogueFovDegrees",95.0f),70.0f,140.0f),std::memory_order_relaxed);
    vr_dialogue_zoom_override_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDialogueZoomOverride",true),std::memory_order_relaxed);
    vr_dialogue_fov_override_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDialogueFovOverride",false),std::memory_order_relaxed);
// M2_SLOT_END 172
#elif KCD2VR_M2_HOST_SLOT == 207
// src/mod.cpp: loadconfig_08.inl
// M2_SLOT_BEGIN 207
    vrik_in_dialogue_=ReadIniBoolExplicit(explicit_ini,"Settings","VrikInDialogue",true);
// M2_SLOT_END 207
#elif KCD2VR_M2_HOST_SLOT == 217
// src/mod.cpp: loadconfig_09.inl
// M2_SLOT_BEGIN 217
    vr_dialogue_text_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrDialogueTextScale", 1.00f),0.50f,1.50f), std::memory_order_relaxed);
// M2_SLOT_END 217
#elif KCD2VR_M2_HOST_SLOT == 224
// src/mod.h: mod_state_01.inl
// M2_SLOT_BEGIN 224
    std::atomic<float> vr_ui_dialogue_scale_{0.71f};
// M2_SLOT_END 224
#elif KCD2VR_M2_HOST_SLOT == 226
// src/mod.h: mod_state_02.inl
// M2_SLOT_BEGIN 226
    std::atomic<float> vr_ui_dialogue_y_{420.0f};          // last runtime-tuned dialogue text baseline
    std::atomic<float> vr_ui_dialogue_inset_pct_{52.0f};
// M2_SLOT_END 226
#elif KCD2VR_M2_HOST_SLOT == 231
// src/mod.h: mod_state_03.inl
// M2_SLOT_BEGIN 231
    std::atomic<bool> vrik_in_dialogue_{true}; // V66.178R9 INI VrikInDialogue
    std::atomic<int> vr_dialogue_camera_mode_{2}; // V65.15 default: 2 immersive 1P + NoCameras; native NPC gaze
// M2_SLOT_END 231
#elif KCD2VR_M2_HOST_SLOT == 233
// src/mod.h: mod_state_04.inl
// M2_SLOT_BEGIN 233
    std::atomic<float> vr_dialogue_zoom_compensation_{0.50f}; // V63.6 dialogue framing; low values zoom out
    std::atomic<float> vr_dialogue_fov_deg_{95.0f}; // optional dialogue compositor source-FOV override
    std::atomic<bool> vr_dialogue_zoom_override_{true};
    std::atomic<bool> vr_dialogue_fov_override_{false};
    std::atomic<bool> vr_dialogue_force_preview_{false}; // transient manual preview; never persisted
    std::atomic<bool> vr_dialogue_detected_{false};      // V63.5 live diagnostic only
// M2_SLOT_END 233
#elif KCD2VR_M2_HOST_SLOT == 242
// src/mod.h: mod_state_05.inl
// M2_SLOT_BEGIN 242
    std::atomic<float> vr_dialogue_text_scale_{1.00f}; // V61.37 subtitle size multiplier test, 0.50..1.50
// M2_SLOT_END 242
#elif KCD2VR_M2_HOST_SLOT == 251
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 251
    float dialogue_text_scale = 1.00f;       // V61.37 live subtitle/dialogue text scale test, 0.50..1.50
// M2_SLOT_END 251
#elif KCD2VR_M2_HOST_SLOT == 258
// src/vr_menu.h: menu_state_02.inl
// M2_SLOT_BEGIN 258
    float vr_ui_dialogue_scale = 0.71f;
    float vr_ui_dialogue_y = 420.0f;          // V62.7: design pixels relative to vanilla dialogue anchors
    float vr_ui_dialogue_inset_pct = 52.0f;
// M2_SLOT_END 258
#elif KCD2VR_M2_HOST_SLOT == 263
// src/vr_menu.h: menu_state_03.inl
// M2_SLOT_BEGIN 263
    int dialogue_camera_mode = 2;          // V65.15 default: 1P + NoCameras; native NPC gaze; 0 native/theater, 1 ForcePOV
// M2_SLOT_END 263
#elif KCD2VR_M2_HOST_SLOT == 265
// src/vr_menu.h: menu_state_04.inl
// M2_SLOT_BEGIN 265
    float dialogue_zoom_compensation = 0.50f; // V63.6 compositor framing multiplier; low values zoom out
    float dialogue_fov_deg = 95.0f;          // optional compositor source-FOV override
    bool dialogue_zoom_override = true;       // independent dialogue framing override
    bool dialogue_fov_override = false;       // independent FOV override; off by default after V63.5 test
    bool dialogue_force_preview = false;       // transient FORCE ON diagnostic; intentionally not persisted
    bool dialogue_detected = false;            // live detector status, read-only UI
// M2_SLOT_END 265
#elif KCD2VR_M2_HOST_SLOT == 267
// src/vr_menu.h: menu_state_05.inl
// M2_SLOT_BEGIN 267
    bool dialogue_hands59c7f=true;
    bool dialogue_smooth38=true; // C38 VrDialogueSmoothCamera
// M2_SLOT_END 267
#elif KCD2VR_M2_HOST_SLOT == 5003
// src/mod.cpp: dialogue_text_scale
// M2_SLOT_BEGIN 5003

// M2_SLOT_END 5003
#elif KCD2VR_M2_HOST_SLOT == 5015
// src/mod.cpp: dialogue_camera_gaze_leases
// M2_SLOT_BEGIN 5015
struct DialogueCVarSnapshot
{
    bool valid=false;
    int force_pov=0;
    int no_cameras=0;
};

bool CaptureDialogueCVars(DialogueCVarSnapshot& out) noexcept
{
    void* c=SafeGetCryConsole(); if(!c) return false;
    void* p=SafeConsoleGetCVar(c,"wh_dlg_ForcePOV");
    void* n=SafeConsoleGetCVar(c,"wh_dlg_NoCameras");
    if(!p || !n) return false;
    out.force_pov=SafeCVarGetInt(p,0);
    out.no_cameras=SafeCVarGetInt(n,0);
    out.valid=true;
    return true;
}

bool ApplyDialogueMode(const DialogueCVarSnapshot& original,int mode) noexcept
{
    if(!original.valid) return false;
    void* c=SafeGetCryConsole(); if(!c) return false;
    void* p=SafeConsoleGetCVar(c,"wh_dlg_ForcePOV");
    void* n=SafeConsoleGetCVar(c,"wh_dlg_NoCameras");
    if(!p || !n) return false;
    int target_p=original.force_pov, target_n=original.no_cameras;
    if(mode==1){ target_p=1; target_n=0; }
    else if(mode==2){ target_p=1; target_n=1; } // V65.15: immersive 1P + remove authored dialogue cameras
    bool ok=true; ok=SafeCVarSetInt(p,target_p)&&ok; ok=CheatCVarSetInt143(n,target_n)&&ok; // R143: NoCameras is a cheat cvar (ForcePOV is not)
    const int got_p=SafeCVarGetInt(p,-999), got_n=SafeCVarGetInt(n,-999);
    char m[256]{}; std::snprintf(m,sizeof(m),
        "V58-DIALOGUE mode=%d target={ForcePOV:%d NoCameras:%d} readback={%d,%d} ok=%s",
        mode,target_p,target_n,got_p,got_n,ok?"true":"false"); debug_trace::Write(m);
    return ok && got_p==target_p && got_n==target_n;
}


struct DialogueLookCVarSnapshot {
    bool captured=false;
    void* npc_target=nullptr; int npc_target_value=0;
    void* force_look=nullptr; int force_look_value=0;
    void* disable_variations=nullptr; int disable_variations_value=0;
    void* eye_offset_mode=nullptr; int eye_offset_mode_value=0;
};
void ApplyDialogueLookAssist(const DialogueLookCVarSnapshot& o,bool enabled) noexcept
{
    if(!o.captured)return; bool ok=true;
    if(o.npc_target) ok=SafeCVarSetInt(o.npc_target,enabled?1:o.npc_target_value)&&ok;
    if(o.force_look) ok=SafeCVarSetInt(o.force_look,enabled?1:o.force_look_value)&&ok;
    // These two current-retail CVars are self-describing in WHGame.dll: mode 2
    // always uses the lookee's bone position, and variation-disable=1 applies
    // when the player is the selected look target. Restore both exactly on OFF.
    if(o.disable_variations) ok=SafeCVarSetInt(o.disable_variations,enabled?1:o.disable_variations_value)&&ok;
    if(o.eye_offset_mode) ok=SafeCVarSetInt(o.eye_offset_mode,enabled?2:o.eye_offset_mode_value)&&ok;
    char m[512]{}; std::snprintf(m,sizeof(m),
        "V65.09-F5 LOOK-CVAR-ASSIST assist=%d npcLook=%d forceLook=%d disableVariations=%d eyeOffsetMode=%d ok=%d targetPointOwner=EXACT-HMD-HOOK",enabled?1:0,
        o.npc_target?SafeCVarGetInt(o.npc_target,-999):-999,o.force_look?SafeCVarGetInt(o.force_look,-999):-999,
        o.disable_variations?SafeCVarGetInt(o.disable_variations,-999):-999,o.eye_offset_mode?SafeCVarGetInt(o.eye_offset_mode,-999):-999,ok?1:0);
    diagnostic_trace::Write(m);
}


// M2_SLOT_END 5015
#else
#error Unknown M2 host contribution slot
#endif
