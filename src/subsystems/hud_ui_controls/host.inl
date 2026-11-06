// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 1
// src/mod.cpp: fillmenustate_01.inl
// M2_SLOT_BEGIN 1
    out.native_ui_scale = vr_native_ui_scale_.load(std::memory_order_acquire);
// M2_SLOT_END 1
#elif KCD2VR_M2_HOST_SLOT == 3
// src/mod.cpp: fillmenustate_02.inl
// M2_SLOT_BEGIN 3
    out.hud_live_observed = flashui_probe::HasRecentHud();
    out.ui_runtime_build_supported = IsSupportedWhGameBuildV6143();
// M2_SLOT_END 3
#elif KCD2VR_M2_HOST_SLOT == 4
// src/mod.cpp: fillmenustate_03.inl
// M2_SLOT_BEGIN 4
    out.hud_xml_scale = hud_xml_scale_.load(std::memory_order_acquire);
    out.hud_xml_offset_x = hud_xml_offset_x_.load(std::memory_order_acquire);
    out.hud_xml_offset_y = hud_xml_offset_y_.load(std::memory_order_acquire);
    out.flat_ui_ipd_mm = vr_flat_ui_ipd_mm_.load(std::memory_order_acquire);
// M2_SLOT_END 4
#elif KCD2VR_M2_HOST_SLOT == 5
// src/mod.cpp: fillmenustate_04.inl
// M2_SLOT_BEGIN 5
    out.hud_native_canvas_fit = vr_hud_native_canvas_fit_.load(std::memory_order_acquire);
    out.hud_native_canvas_size = vr_hud_native_canvas_size_.load(std::memory_order_acquire);
    out.hud_native_canvas_spread_x = vr_hud_native_canvas_spread_x_.load(std::memory_order_acquire);
    out.hud_native_canvas_spread_y = vr_hud_native_canvas_spread_y_.load(std::memory_order_acquire);
    out.auto_correct_hud = vr_auto_correct_hud_.load(std::memory_order_acquire);
    out.hud_lag_on = hud_lag_on22_.load(std::memory_order_acquire); // C22b
// M2_SLOT_END 5
#elif KCD2VR_M2_HOST_SLOT == 11
// src/mod.cpp: fillmenustate_05.inl
// M2_SLOT_BEGIN 11
    out.vr_ui_layout = enable_vr_ui_layout_.load(std::memory_order_acquire);
    out.vr_ui_auto_layout = vr_ui_auto_layout_.load(std::memory_order_acquire);
    out.hud_corrected_active = true; // C33: the corrected HUD is always on
    out.auto_hud_icon_scale = auto_hud_icon_scale_.load(std::memory_order_acquire);
    out.vr_ui_compactness = vr_ui_compactness_.load(std::memory_order_acquire);
    out.vr_ui_safe_width = vr_ui_safe_width_.load(std::memory_order_acquire);
    out.vr_ui_safe_height = vr_ui_safe_height_.load(std::memory_order_acquire);
    out.vr_ui_top_boundary = vr_ui_top_boundary_.load(std::memory_order_acquire);
    out.vr_ui_bottom_boundary = vr_ui_bottom_boundary_.load(std::memory_order_acquire);
    out.vr_ui_center_offset_x = vr_ui_center_offset_x_.load(std::memory_order_acquire);
    out.vr_ui_center_offset_y = vr_ui_center_offset_y_.load(std::memory_order_acquire);
// M2_SLOT_END 11
#elif KCD2VR_M2_HOST_SLOT == 14
// src/mod.cpp: fillmenustate_06.inl
// M2_SLOT_BEGIN 14
    out.vr_ui_subtitle_scale = vr_ui_subtitle_scale_.load(std::memory_order_acquire);
    out.vr_ui_subtitle_y = vr_ui_subtitle_y_.load(std::memory_order_acquire);
// M2_SLOT_END 14
#elif KCD2VR_M2_HOST_SLOT == 16
// src/mod.cpp: fillmenustate_07.inl
// M2_SLOT_BEGIN 16
    out.vr_ui_item_panel_scale = vr_ui_item_panel_scale_.load(std::memory_order_acquire);
    out.vr_ui_item_panel_x = vr_ui_item_panel_x_.load(std::memory_order_acquire);
    out.vr_ui_item_panel_y = vr_ui_item_panel_y_.load(std::memory_order_acquire);
// M2_SLOT_END 16
#elif KCD2VR_M2_HOST_SLOT == 20
// src/mod.cpp: fillmenustate_08.inl
// M2_SLOT_BEGIN 20
    out.vr_ui_hints_scale = vr_ui_hints_scale_.load(std::memory_order_acquire);
    out.vr_ui_hints_inset_pct = vr_ui_hints_inset_pct_.load(std::memory_order_acquire);
    out.vr_ui_hints_y = vr_ui_hints_y_.load(std::memory_order_acquire);
    out.vr_ui_control_prompt_scale = vr_ui_control_prompt_scale_.load(std::memory_order_acquire);
    out.vr_ui_control_prompt_x = vr_ui_control_prompt_x_.load(std::memory_order_acquire);
    out.vr_ui_control_prompt_y = vr_ui_control_prompt_y_.load(std::memory_order_acquire);
    out.vr_ui_compass_scale = vr_ui_compass_scale_.load(std::memory_order_acquire);
    out.vr_ui_compass_y = vr_ui_compass_y_.load(std::memory_order_acquire);
    out.vr_ui_stats_scale = vr_ui_stats_scale_.load(std::memory_order_acquire);
    out.vr_ui_stats_x = vr_ui_stats_x_.load(std::memory_order_acquire);
    out.vr_ui_stats_y = vr_ui_stats_y_.load(std::memory_order_acquire);
    out.vr_ui_lower_scale = vr_ui_lower_scale_.load(std::memory_order_acquire);
    out.vr_ui_lower_y = vr_ui_lower_y_.load(std::memory_order_acquire);
    out.vr_ui_lower_spread = vr_ui_lower_spread_.load(std::memory_order_acquire);
    out.vr_ui_hide_letterbox = vr_ui_hide_letterbox_.load(std::memory_order_acquire);
// M2_SLOT_END 20
#elif KCD2VR_M2_HOST_SLOT == 21
// src/mod.cpp: fillmenustate_09.inl
// M2_SLOT_BEGIN 21
    out.hud_crosshair_visible = hud_crosshair_visible_.load(std::memory_order_acquire);
    out.hud_lower_icons_visible = hud_lower_icons_visible_.load(std::memory_order_acquire);
    out.hud_compass_visible = hud_compass_visible_.load(std::memory_order_acquire);
    out.hud_status_icons_visible=hud_status_icons_visible_.load();
    out.contextual_hud=vr_contextual_hud_.load(std::memory_order_acquire);
// M2_SLOT_END 21
#elif KCD2VR_M2_HOST_SLOT == 25
// src/mod.cpp: fillmenustate_10.inl
// M2_SLOT_BEGIN 25
    out.touch_bindings = enable_touch_bindings_.load(std::memory_order_acquire);
// M2_SLOT_END 25
#elif KCD2VR_M2_HOST_SLOT == 27
// src/mod.cpp: fillmenustate_11.inl
// M2_SLOT_BEGIN 27
    out.vr_laser_visible = vr_laser_visible_.load(std::memory_order_acquire);
// M2_SLOT_END 27
#elif KCD2VR_M2_HOST_SLOT == 37
// src/mod.cpp: fillmenustate_12.inl
// M2_SLOT_BEGIN 37
    out.vr_display_mode = 0;
    out.vr_pad_glyph_style = vr_pad_glyph_style_.load(std::memory_order_acquire)==4?4:2;
    out.vr_prompt_identity_lab = vr_prompt_identity_lab_.load(std::memory_order_acquire);
// M2_SLOT_END 37
#elif KCD2VR_M2_HOST_SLOT == 39
// src/mod.cpp: applymenustate_01.inl
// M2_SLOT_BEGIN 39
    const float native_ui_scale = std::clamp(state.native_ui_scale, 0.50f, 1.20f);
// M2_SLOT_END 39
#elif KCD2VR_M2_HOST_SLOT == 41
// src/mod.cpp: applymenustate_02.inl
// M2_SLOT_BEGIN 41
    const float hud_xml_scale = std::clamp(state.hud_xml_scale, 0.50f, 1.10f);
    const int hud_xml_x = std::clamp(state.hud_xml_offset_x, -700, 700);
    const int hud_xml_y = std::clamp(state.hud_xml_offset_y, -700, 700);
    const float flat_ui_ipd_mm = std::clamp(state.flat_ui_ipd_mm, -80.0f, 80.0f);
// M2_SLOT_END 41
#elif KCD2VR_M2_HOST_SLOT == 43
// src/mod.cpp: applymenustate_03.inl
// M2_SLOT_BEGIN 43
    const float ui_sub_scale = std::clamp(state.vr_ui_subtitle_scale,0.45f,0.90f);
    const float ui_sub_y = std::clamp(state.vr_ui_subtitle_y,-600.0f,600.0f);
// M2_SLOT_END 43
#elif KCD2VR_M2_HOST_SLOT == 45
// src/mod.cpp: applymenustate_04.inl
// M2_SLOT_BEGIN 45
    const float ui_item_panel_scale = std::clamp(state.vr_ui_item_panel_scale,0.45f,1.25f);
    const float ui_item_panel_x = std::clamp(state.vr_ui_item_panel_x,-700.0f,700.0f);
    const float ui_item_panel_y = std::clamp(state.vr_ui_item_panel_y,-450.0f,450.0f);
// M2_SLOT_END 45
#elif KCD2VR_M2_HOST_SLOT == 47
// src/mod.cpp: applymenustate_05.inl
// M2_SLOT_BEGIN 47
    const float ui_hints_scale = std::clamp(state.vr_ui_hints_scale,0.25f,1.50f);
    const float ui_hints_inset = std::clamp(state.vr_ui_hints_inset_pct,0.0f,75.0f);
    const float ui_hints_y = std::clamp(state.vr_ui_hints_y,-600.0f,600.0f);
    const float ui_control_prompt_scale = std::clamp(state.vr_ui_control_prompt_scale,0.30f,1.50f);
    const float ui_control_prompt_x = std::clamp(state.vr_ui_control_prompt_x,-700.0f,700.0f);
    const float ui_control_prompt_y = std::clamp(state.vr_ui_control_prompt_y,-600.0f,600.0f);
// M2_SLOT_END 47
#elif KCD2VR_M2_HOST_SLOT == 48
// src/mod.cpp: applymenustate_06.inl
// M2_SLOT_BEGIN 48
    if(auto_hud_icon_scale_.exchange(iconScale51c3)!=iconScale51c3)vr_hud_reapply_requested_.store(true,std::memory_order_release);
    const float ui_compactness = std::clamp(state.vr_ui_compactness,0.25f,1.20f);
    const float ui_safe_width = std::clamp(state.vr_ui_safe_width,0.60f,1.40f);
    const float ui_safe_height = std::clamp(state.vr_ui_safe_height,0.60f,1.40f);
    const float ui_top_boundary = std::clamp(state.vr_ui_top_boundary,0.55f,3.00f);
    const float ui_bottom_boundary = std::clamp(state.vr_ui_bottom_boundary,0.55f,3.00f);
    const float ui_center_x = std::clamp(state.vr_ui_center_offset_x,-500.0f,500.0f);
    const float ui_center_y = std::clamp(state.vr_ui_center_offset_y,-500.0f,500.0f);
    const float ui_compass_scale=std::clamp(state.vr_ui_compass_scale,0.15f,1.50f);
    const float ui_compass_y=std::clamp(state.vr_ui_compass_y,-20.0f,100.0f);
    const float ui_stats_scale=std::clamp(state.vr_ui_stats_scale,0.15f,1.50f);
    const float ui_stats_x=std::clamp(state.vr_ui_stats_x,-700.0f,700.0f);
    const float ui_stats_y=std::clamp(state.vr_ui_stats_y,-20.0f,100.0f);
    const float ui_lower_scale=std::clamp(state.vr_ui_lower_scale,0.15f,2.00f);
    const float ui_lower_y=std::clamp(state.vr_ui_lower_y,-500.0f,500.0f);
    const float ui_lower_spread=std::clamp(state.vr_ui_lower_spread,-400.0f,400.0f);
// M2_SLOT_END 48
#elif KCD2VR_M2_HOST_SLOT == 49
// src/mod.cpp: applymenustate_07.inl
// M2_SLOT_BEGIN 49
    enable_vr_ui_layout_.store(state.vr_ui_layout,std::memory_order_release);
    const float old_ui_safe_width=vr_ui_safe_width_.load(std::memory_order_acquire);
    const float old_ui_safe_height=vr_ui_safe_height_.load(std::memory_order_acquire);
    const float old_ui_top_boundary=vr_ui_top_boundary_.load(std::memory_order_acquire);
    const float old_ui_bottom_boundary=vr_ui_bottom_boundary_.load(std::memory_order_acquire);
    const float old_ui_compactness=vr_ui_compactness_.load(std::memory_order_acquire);
    const float old_ui_center_x=vr_ui_center_offset_x_.load(std::memory_order_acquire);
    const float old_ui_center_y=vr_ui_center_offset_y_.load(std::memory_order_acquire);
// M2_SLOT_END 49
#elif KCD2VR_M2_HOST_SLOT == 51
// src/mod.cpp: applymenustate_08.inl
// M2_SLOT_BEGIN 51
    const bool changed_canvas_mode=vr_hud_native_canvas_fit_.exchange(false,std::memory_order_acq_rel);
    const float canvas_size=std::isfinite(state.hud_native_canvas_size)?std::clamp(state.hud_native_canvas_size,.70f,1.50f):1.00f;
    const float canvas_spread_x=std::isfinite(state.hud_native_canvas_spread_x)?std::clamp(state.hud_native_canvas_spread_x,.45f,1.10f):.78f;
    const float canvas_spread_y=std::isfinite(state.hud_native_canvas_spread_y)?std::clamp(state.hud_native_canvas_spread_y,.45f,1.10f):.78f;
    const float old_canvas_size=vr_hud_native_canvas_size_.exchange(canvas_size,std::memory_order_acq_rel);
    const float old_canvas_spread_x=vr_hud_native_canvas_spread_x_.exchange(canvas_spread_x,std::memory_order_acq_rel);
    const float old_canvas_spread_y=vr_hud_native_canvas_spread_y_.exchange(canvas_spread_y,std::memory_order_acq_rel);
    if(changed_canvas_mode||std::fabs(old_canvas_size-canvas_size)>.0001f||std::fabs(old_canvas_spread_x-canvas_spread_x)>.0001f||std::fabs(old_canvas_spread_y-canvas_spread_y)>.0001f)
        vr_hud_reapply_requested_.store(true,std::memory_order_release);
// M2_SLOT_END 51
#elif KCD2VR_M2_HOST_SLOT == 52
// src/mod.cpp: applymenustate_09.inl
// M2_SLOT_BEGIN 52
    hud_lag_on22_.store(state.hud_lag_on,std::memory_order_release); // C22b
    if(xr_manager_)xr_manager_->SetHudLag(state.hud_lag_on?hud_lag22_.load(std::memory_order_relaxed):0.0f);
    const bool requested_auto_hud=state.auto_correct_hud; // R49C1: same readable-parent owner as F2, explicitly selected.
    const bool old_auto_correct_hud=vr_auto_correct_hud_.exchange(requested_auto_hud,std::memory_order_acq_rel);
    if(old_auto_correct_hud!=requested_auto_hud ||
       (requested_auto_hud &&
        (std::fabs(old_ui_compactness-ui_compactness)>0.0001f ||
         std::fabs(old_ui_safe_width-ui_safe_width)>0.0001f ||
         std::fabs(old_ui_safe_height-ui_safe_height)>0.0001f ||
         std::fabs(old_ui_top_boundary-ui_top_boundary)>0.0001f ||
         std::fabs(old_ui_bottom_boundary-ui_bottom_boundary)>0.0001f ||
         std::fabs(old_ui_center_x-ui_center_x)>0.01f ||
         std::fabs(old_ui_center_y-ui_center_y)>0.01f ||
         std::fabs(old_ui_dialogue_scale-ui_dialog_scale)>0.0001f)))
        vr_hud_reapply_requested_.store(true,std::memory_order_release);
    if(old_auto_correct_hud ||
       (old_cyclopean!=cyclopean ||
        std::fabs(old_ui_safe_width-ui_safe_width)>0.0001f ||
        std::fabs(old_ui_safe_height-ui_safe_height)>0.0001f ||
        std::fabs(old_source_fov-source_fov)>0.0001f))
        vr_corrected_hud_dirty_.store(true,std::memory_order_release);
// M2_SLOT_END 52
#elif KCD2VR_M2_HOST_SLOT == 53
// src/mod.cpp: applymenustate_10.inl
// M2_SLOT_BEGIN 53
    vr_ui_auto_layout_.store(false,std::memory_order_release); // legacy per-element owner stays off; Auto HUD owns observed HUD/MasterSwitch root
    vr_ui_compactness_.store(ui_compactness,std::memory_order_release);
    vr_ui_safe_width_.store(ui_safe_width,std::memory_order_release);
    vr_ui_safe_height_.store(ui_safe_height,std::memory_order_release);
    vr_ui_top_boundary_.store(ui_top_boundary,std::memory_order_release);
    vr_ui_bottom_boundary_.store(ui_bottom_boundary,std::memory_order_release);
    vr_ui_center_offset_x_.store(ui_center_x,std::memory_order_release);
    vr_ui_center_offset_y_.store(ui_center_y,std::memory_order_release);
// M2_SLOT_END 53
#elif KCD2VR_M2_HOST_SLOT == 55
// src/mod.cpp: applymenustate_11.inl
// M2_SLOT_BEGIN 55
    vr_ui_subtitle_scale_.store(ui_sub_scale,std::memory_order_release);
    vr_ui_subtitle_y_.store(ui_sub_y,std::memory_order_release);
// M2_SLOT_END 55
#elif KCD2VR_M2_HOST_SLOT == 57
// src/mod.cpp: applymenustate_12.inl
// M2_SLOT_BEGIN 57
    vr_ui_item_panel_scale_.store(ui_item_panel_scale,std::memory_order_release);
    vr_ui_item_panel_x_.store(ui_item_panel_x,std::memory_order_release);
    vr_ui_item_panel_y_.store(ui_item_panel_y,std::memory_order_release);
// M2_SLOT_END 57
#elif KCD2VR_M2_HOST_SLOT == 60
// src/mod.cpp: applymenustate_13.inl
// M2_SLOT_BEGIN 60
    vr_ui_hints_scale_.store(ui_hints_scale,std::memory_order_release);
    vr_ui_hints_inset_pct_.store(ui_hints_inset,std::memory_order_release);
    vr_ui_hints_y_.store(ui_hints_y,std::memory_order_release);
    vr_ui_control_prompt_scale_.store(ui_control_prompt_scale,std::memory_order_release);
    vr_ui_control_prompt_x_.store(ui_control_prompt_x,std::memory_order_release);
    vr_ui_control_prompt_y_.store(ui_control_prompt_y,std::memory_order_release);
    vr_ui_compass_scale_.store(ui_compass_scale,std::memory_order_release);
    vr_ui_compass_y_.store(ui_compass_y,std::memory_order_release);
    vr_ui_stats_scale_.store(ui_stats_scale,std::memory_order_release);
    vr_ui_stats_x_.store(ui_stats_x,std::memory_order_release);
    vr_ui_stats_y_.store(ui_stats_y,std::memory_order_release);
    vr_ui_lower_scale_.store(ui_lower_scale,std::memory_order_release);
    vr_ui_lower_y_.store(ui_lower_y,std::memory_order_release);
    vr_ui_lower_spread_.store(ui_lower_spread,std::memory_order_release);
    vr_ui_hide_letterbox_.store(state.vr_ui_hide_letterbox,std::memory_order_release);
// M2_SLOT_END 60
#elif KCD2VR_M2_HOST_SLOT == 61
// src/mod.cpp: applymenustate_14.inl
// M2_SLOT_BEGIN 61
    vr_native_ui_scale_.store(native_ui_scale, std::memory_order_release);
// M2_SLOT_END 61
#elif KCD2VR_M2_HOST_SLOT == 63
// src/mod.cpp: applymenustate_15.inl
// M2_SLOT_BEGIN 63
    hud_xml_scale_.store(hud_xml_scale, std::memory_order_release);
    hud_xml_offset_x_.store(hud_xml_x, std::memory_order_release);
    hud_xml_offset_y_.store(hud_xml_y, std::memory_order_release);
    vr_flat_ui_ipd_mm_.store(flat_ui_ipd_mm, std::memory_order_release);
// M2_SLOT_END 63
#elif KCD2VR_M2_HOST_SLOT == 64
// src/mod.cpp: applymenustate_16.inl
// M2_SLOT_BEGIN 64
    hud_crosshair_visible_.store(state.hud_crosshair_visible,std::memory_order_release);
    hud_lower_icons_visible_.store(state.hud_lower_icons_visible,std::memory_order_release);
    hud_compass_visible_.store(state.hud_compass_visible,std::memory_order_release);
    hud_status_icons_visible_.store(state.hud_status_icons_visible);
    vr_contextual_hud_.store(state.contextual_hud,std::memory_order_release); // explicit checkbox; VRIK gate evaluated per sample
// M2_SLOT_END 64
#elif KCD2VR_M2_HOST_SLOT == 68
// src/mod.cpp: applymenustate_17.inl
// M2_SLOT_BEGIN 68
    enable_touch_bindings_.store(state.touch_bindings, std::memory_order_release);
// M2_SLOT_END 68
#elif KCD2VR_M2_HOST_SLOT == 74
// src/mod.cpp: applymenustate_18.inl
// M2_SLOT_BEGIN 74
    vr_laser_visible_.store(state.vr_laser_visible,std::memory_order_release);
// M2_SLOT_END 74
#elif KCD2VR_M2_HOST_SLOT == 87
// src/mod.cpp: applymenustate_19.inl
// M2_SLOT_BEGIN 87
    vr_display_mode_override_.store(0,std::memory_order_release); // V65.03 persistent presentation policy is AUTO-only
    vr_pad_glyph_style_.store(state.vr_pad_glyph_style==4?4:2,std::memory_order_release);
    vr_prompt_identity_lab_.store(state.vr_prompt_identity_lab,std::memory_order_release);
    prompt_device_lock::SetForcedDevice(state.vr_pad_glyph_style==4?4:2);
    (void)RestyleF7PromptPresentationD33(state.vr_pad_glyph_style==4?4:2); // D33: native prompts follow the menu choice now
    touch_bindings_.SetEnabled(state.touch_bindings);
// M2_SLOT_END 87
#elif KCD2VR_M2_HOST_SLOT == 89
// src/mod.cpp: applymenustate_20.inl
// M2_SLOT_BEGIN 89
    if (xr_manager_) {
        xr_manager_->SetDiagnosticRendering(state.diagnostic_rendering);
        xr_manager_->SetUiLaserVisible(state.vr_laser_visible);
        xr_manager_->SetFlatUiScale(flat_screen_scale);
        xr_manager_->SetFlatUiIpdMm(flat_ui_ipd_mm);
        xr_manager_->SetActiveVrBridge(state.bridge_enabled);
        // V61.0: pseudo presets remain available when stereo_mode==0.
        // Explicit AFR/DUAL tests bypass pseudo UV disparity and use the matched-B tangent map.
        const V603ViewPreset preset = ViewPresetForMode(cyclopean);
        int active_presentation = preset.presentation_mode;
        float active_uv = preset.uv_binocular;
        float active_asym = preset.fov_asymmetry;
        int active_map_mode = MonoMapModeForCleanMode(cyclopean);
        if (native_production || stereo == 1 || stereo == 2) {
            active_presentation = 3;
            active_uv = 0.0f;
            active_asym = 1.0f;
            // V66.30: genuine native L/R still needs intrinsics mapping because KCD2
            // renders both shifted eye cameras through the game's symmetric source
            // frustum. Correct FOV (cyclopean==1) must use map 5: game_fov_target
            // describes the captured KCD source frustum and source_fov is the
            // requested VR view FOV. The old forced map 4 caused the scale/warp
            // regression by treating the two domains as one.
            active_map_mode = (native_production && cyclopean==1) ? 5 : 4;
        }
        vr_presentation_mode_.store(active_presentation, std::memory_order_release);
        // V65.17: source/zoom edits are persisted in ordinary mono. PROJECTIVE REMAP
        // consumes the source FOV last synchronized by the explicit REMAP button.
        vr_zoom_.store(zoom, std::memory_order_release);
        const float mapped_source_fov = effective_source_fov;
        const float mapped_zoom = cyclopean==1 ? 1.0f : effective_zoom;
        if(cyclopean==1) {
            vr_source_vertical_fov_deg_.store(source_fov,std::memory_order_release); // user VR FOV
            vr_corrected_source_vfov_deg_.store(game_fov_target,std::memory_order_release); // actual KCD geometry
            vr_corrected_view_vfov_deg_.store(source_fov,std::memory_order_release);
            vr_corrected_zoom_.store(1.0f,std::memory_order_release);
            xr_manager_->SetCorrectedViewVerticalFov(source_fov);
        }
        xr_manager_->SetPresentationSettings(
            render_scale, active_presentation, mapped_zoom,
            0.0f, 0.0f, 0.0f, 0.0f, mapped_source_fov);
        xr_manager_->SetAngularCorrectMono(native_production ? true : (stereo == 0 ? preset.angular_correct : true));
        xr_manager_->SetPoseHonestyOverride(native_production ? 1 : (stereo == 0 ? preset.pose_honesty : 1));
        xr_manager_->SetMonoFovAsymmetryScale(active_asym);
        xr_manager_->SetMonoUvReprojectionMode(active_map_mode);
        xr_manager_->SetMonoUvBinocularOffset(active_uv);
        // V64.35 hybrid: V64.22C headset-positive pseudo semantics.
        xr_manager_->SetDepthPushWeight(depth_stereo_strength);
        const bool pseudo_depth = !native_production && cyclopean == 3;
        xr_manager_->SetDepthPushEnabled(pseudo_depth);
        if (DeviceHook::GenericDepthProbeEnabled() != pseudo_depth) DeviceHook::SetGenericDepthProbe(pseudo_depth);
        // V66.24: native stereo uses angular projection centering, not legacy planar UV offsets.
        const float applied_center_x = native_production ? 0.0f : (cyclopean <= 1 ? 0.0f : center_x);
        const float applied_center_y = native_production ? 0.0f : (cyclopean == 0 ? 0.0f : center_y);
        vr_presentation_center_x_.store(applied_center_x, std::memory_order_release);
        vr_presentation_center_y_.store(applied_center_y, std::memory_order_release);
        xr_manager_->SetPresentationCenterX(applied_center_x);
        xr_manager_->SetPresentationCenterY(applied_center_y);
        xr_manager_->SetSwapEyes(state.swap_eyes);
        if (native_production && !native_stereo_running_r89_.load(std::memory_order_acquire)) {
            // R89 dice: STEREO is wanted but no stereo run is producing pairs (start
            // refused, DLSS/size fault, between runs). XR mode 2 here re-presented the
            // last old pair for good: D85R5 KCD2VR1 23:53:07, VR zoom slider after the
            // dice game -> freshPerS=0 reusePct=100 -> frozen dice image until restart.
            // Live image now; Run's autostart sets mode 2 itself when a run starts.
            xr_manager_->SetStereoExperiment(0, 0.0f);
        } else if (native_production) {
            // V66.24: our VR Settings quad is additive. Applying any setting must
            // preserve the certified native L/R projection underneath it.
            xr_manager_->SetStereoExperiment(2, 0.0f);
            debug_trace::Write("V66.24-VR-MENU-STEREO PRESERVED production=3 XR=MODE2 stage4=AUTHORITATIVE overlay=ADDITIVE monoReset=NO");
            diagnostic_trace::Write("V66.24-VR-MENU-STEREO PRESERVED production=3 XR=MODE2 stage4=AUTHORITATIVE overlay=ADDITIVE monoReset=NO");
        } else if (stereo == 1) {
            xr_manager_->SetStereoExperiment(1, ipd_scale);
        } else {
            xr_manager_->SetStereoExperiment(0, 0.0f);
        }
        if (!native_production && stereo == 0 && cyclopean == 0)
            debug_trace::Write("V61.0-MONO invariant requested: stereo/AFR/native-dual off; matched pseudo shifts cleared");
        xr_manager_->SetSharpening(engine_clarity);
    }
// M2_SLOT_END 89
#elif KCD2VR_M2_HOST_SLOT == 90
// src/mod.cpp: applymenustate_21.inl
// M2_SLOT_BEGIN 90
    std::snprintf(m, sizeof(m),
        "V62.4-MENU-APPLY bridge=%s tracking=%s/%s camSide=%.3fm camBack=%.3fm camHeight=%.3fm present=%d zoom=%.2f sourceVFov=%.1f cullScale=%.2f legacyHudScale=%.2f flatScreenScale=%.2f zoomRetain=%.2f stereo=%d cyclo=%d asym=%.3f afrIpd=%.2f swap=%s balanced=%s clarity=%.2f renderScale=%.2f dialogueMode=%d touch=%s",
        state.bridge_enabled?"true":"false", state.head_rotation?"rot":"-", state.head_position?"pos":"-",
        static_cast<double>(camera_side), static_cast<double>(camera_back), static_cast<double>(camera_height), presentation, static_cast<double>(effective_zoom), static_cast<double>(effective_source_fov), static_cast<double>(cull_scale), static_cast<double>(native_ui_scale), static_cast<double>(flat_screen_scale), static_cast<double>(transient_zoom_retain), stereo, cyclopean, static_cast<double>(CyclopeanAsymmetryForMode(cyclopean)),
        static_cast<double>(ipd_scale), state.swap_eyes?"true":"false", state.balanced_graphics?"true":"false",
        static_cast<double>(engine_clarity), static_cast<double>(render_scale), dialogue_mode, state.touch_bindings?"true":"false");
// M2_SLOT_END 90
#elif KCD2VR_M2_HOST_SLOT == 91
// src/mod.cpp: savemenusettings_01.inl
// M2_SLOT_BEGIN 91
    ok = wb("EnableVrUiLayout", enable_vr_ui_layout_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrUiAutoLayout", vr_ui_auto_layout_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrHudNativeCanvasFit", vr_hud_native_canvas_fit_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrHudNativeCanvasSize", vr_hud_native_canvas_size_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrHudNativeCanvasSpreadX", vr_hud_native_canvas_spread_x_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrHudNativeCanvasSpreadY", vr_hud_native_canvas_spread_y_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrAutoCorrectHud", vr_auto_correct_hud_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrHudFollowSlow", hud_lag_on22_.load(std::memory_order_acquire)) && ok; // C22b
// M2_SLOT_END 91
#elif KCD2VR_M2_HOST_SLOT == 92
// src/mod.cpp: savemenusettings_02.inl
// M2_SLOT_BEGIN 92
    ok = wf("VrAutoHudIconScale",auto_hud_icon_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiCompactness", vr_ui_compactness_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiSafeWidth", vr_ui_safe_width_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiSafeHeight", vr_ui_safe_height_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrManualHudUpperBoundary", vr_ui_top_boundary_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrManualHudLowerBoundary", vr_ui_bottom_boundary_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiCenterOffsetX", vr_ui_center_offset_x_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiCenterOffsetY", vr_ui_center_offset_y_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 92
#elif KCD2VR_M2_HOST_SLOT == 94
// src/mod.cpp: savemenusettings_03.inl
// M2_SLOT_BEGIN 94
    ok = wf("VrUiSubtitleScale", vr_ui_subtitle_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiSubtitleY", vr_ui_subtitle_y_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 94
#elif KCD2VR_M2_HOST_SLOT == 96
// src/mod.cpp: savemenusettings_04.inl
// M2_SLOT_BEGIN 96
    ok = wf("VrUiItemPanelScale", vr_ui_item_panel_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiItemPanelX", vr_ui_item_panel_x_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiItemPanelY", vr_ui_item_panel_y_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 96
#elif KCD2VR_M2_HOST_SLOT == 100
// src/mod.cpp: savemenusettings_05.inl
// M2_SLOT_BEGIN 100
    ok = wf("VrUiHintsScale", vr_ui_hints_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiHintsInsetPct", vr_ui_hints_inset_pct_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiHintsY", vr_ui_hints_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiControlPromptScale", vr_ui_control_prompt_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiControlPromptX", vr_ui_control_prompt_x_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiControlPromptY", vr_ui_control_prompt_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiCompassScale", vr_ui_compass_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiCompassY", vr_ui_compass_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiStatsScale", vr_ui_stats_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiStatsX", vr_ui_stats_x_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiStatsY", vr_ui_stats_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiLowerScale", vr_ui_lower_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiLowerY", vr_ui_lower_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiLowerSpread", vr_ui_lower_spread_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrUiHideLetterbox", vr_ui_hide_letterbox_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrHudCrosshairVisible", hud_crosshair_visible_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrHudLowerIconsVisible", hud_lower_icons_visible_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrHudCompassVisible", hud_compass_visible_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrHudStatusIconsVisible", hud_status_icons_visible_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrContextualHud",vr_contextual_hud_.load()) && ok;
// M2_SLOT_END 100
#elif KCD2VR_M2_HOST_SLOT == 106
// src/mod.cpp: savemenusettings_06.inl
// M2_SLOT_BEGIN 106
    ok = wb("EnableTouchBindings", enable_touch_bindings_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 106
#elif KCD2VR_M2_HOST_SLOT == 109
// src/mod.cpp: savemenusettings_07.inl
// M2_SLOT_BEGIN 109
    ok = wb("VrLaserVisible", vr_laser_visible_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 109
#elif KCD2VR_M2_HOST_SLOT == 119
// src/mod.cpp: savemenusettings_08.inl
// M2_SLOT_BEGIN 119
    ok = wi("VrDisplayMode", vr_display_mode_override_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrPadGlyphStyle", vr_pad_glyph_style_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrPromptIdentityLab", vr_prompt_identity_lab_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 119
#elif KCD2VR_M2_HOST_SLOT == 121
// src/mod.cpp: savemenusettings_09.inl
// M2_SLOT_BEGIN 121
    ok = wf("VrNativeUiScale", vr_native_ui_scale_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 121
#elif KCD2VR_M2_HOST_SLOT == 123
// src/mod.cpp: savemenusettings_10.inl
// M2_SLOT_BEGIN 123
    ok = wf("HudXmlScale", hud_xml_scale_.load(std::memory_order_acquire)) && ok;
    ok = wi("HudXmlOffsetX", hud_xml_offset_x_.load(std::memory_order_acquire)) && ok;
    ok = wi("HudXmlOffsetY", hud_xml_offset_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrFlatUiIpdMm", vr_flat_ui_ipd_mm_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 123
#elif KCD2VR_M2_HOST_SLOT == 124
// src/mod.cpp: loadconfig_01.inl
// M2_SLOT_BEGIN 124
    settings.bind("EnableVrUiLayout", enable_vr_ui_layout_, true);
    settings.bind("VrUiAutoLayout", vr_ui_auto_layout_, false);
    settings.bind("VrHudNativeCanvasFit", vr_hud_native_canvas_fit_, false);
    settings.bind("VrHudNativeCanvasSize", vr_hud_native_canvas_size_, 1.00f);
    settings.bind("VrHudNativeCanvasSpreadX", vr_hud_native_canvas_spread_x_, 0.78f);
    settings.bind("VrHudNativeCanvasSpreadY", vr_hud_native_canvas_spread_y_, 0.78f);
    settings.bind("VrAutoCorrectHud", vr_auto_correct_hud_, false);
// M2_SLOT_END 124
#elif KCD2VR_M2_HOST_SLOT == 125
// src/mod.cpp: loadconfig_02.inl
// M2_SLOT_BEGIN 125
    settings.bind("VrAutoHudIconScale",auto_hud_icon_scale_,1.75f);
    settings.bind("VrUiCompactness", vr_ui_compactness_, 1.00f);
    settings.bind("VrUiSafeWidth", vr_ui_safe_width_, 0.65f); // C57: same default as the explicit read
    settings.bind("VrUiSafeHeight", vr_ui_safe_height_, 1.01f); // C57: same default as the explicit read
    settings.bind("VrUiTopBoundary", vr_ui_top_boundary_, 0.90f);
    settings.bind("VrUiBottomBoundary", vr_ui_bottom_boundary_, 0.90f);
    settings.bind("VrUiCenterOffsetX", vr_ui_center_offset_x_, 0.0f);
    settings.bind("VrUiCenterOffsetY", vr_ui_center_offset_y_, -115.0f);
// M2_SLOT_END 125
#elif KCD2VR_M2_HOST_SLOT == 127
// src/mod.cpp: loadconfig_03.inl
// M2_SLOT_BEGIN 127
    settings.bind("VrUiSubtitleScale", vr_ui_subtitle_scale_, 0.54f);
    settings.bind("VrUiSubtitleY", vr_ui_subtitle_y_, -375.0f);
// M2_SLOT_END 127
#elif KCD2VR_M2_HOST_SLOT == 129
// src/mod.cpp: loadconfig_04.inl
// M2_SLOT_BEGIN 129
    settings.bind("VrUiItemPanelScale", vr_ui_item_panel_scale_, 0.73f);
    settings.bind("VrUiItemPanelX", vr_ui_item_panel_x_, -380.0f);
    settings.bind("VrUiItemPanelY", vr_ui_item_panel_y_, -300.0f);
// M2_SLOT_END 129
#elif KCD2VR_M2_HOST_SLOT == 131
// src/mod.cpp: loadconfig_05.inl
// M2_SLOT_BEGIN 131
    settings.bind("VrUiHintsScale", vr_ui_hints_scale_, 0.64f);
    settings.bind("VrUiHintsInsetPct", vr_ui_hints_inset_pct_, 33.0f);
    settings.bind("VrUiHintsY", vr_ui_hints_y_, -185.0f);
    settings.bind("VrUiControlPromptScale", vr_ui_control_prompt_scale_, 0.70f);
    settings.bind("VrUiControlPromptX", vr_ui_control_prompt_x_, 0.0f);
    settings.bind("VrUiControlPromptY", vr_ui_control_prompt_y_, 0.0f);
    settings.bind("VrUiCompassScale", vr_ui_compass_scale_, 1.0f);
    settings.bind("VrUiCompassY", vr_ui_compass_y_, 0.0f);
    settings.bind("VrUiStatsScale", vr_ui_stats_scale_, 1.0f);
    settings.bind("VrUiStatsX", vr_ui_stats_x_, 0.0f);
    settings.bind("VrUiStatsY", vr_ui_stats_y_, 0.0f);
    settings.bind("VrUiLowerScale", vr_ui_lower_scale_, 1.0f);
    settings.bind("VrUiLowerY", vr_ui_lower_y_, 0.0f);
    settings.bind("VrUiLowerSpread", vr_ui_lower_spread_, 0.0f);
    settings.bind("VrUiHideLetterbox", vr_ui_hide_letterbox_, false);
    settings.bind("VrHudCrosshairVisible", hud_crosshair_visible_, false);
    settings.bind("VrHudLowerIconsVisible", hud_lower_icons_visible_, true);
    settings.bind("VrHudCompassVisible", hud_compass_visible_, true);
    settings.bind("VrHudStatusIconsVisible", hud_status_icons_visible_, false);
    settings.bind("VrContextualHud",vr_contextual_hud_,true); // C41: same default as the INI read (Miki)
// M2_SLOT_END 131
#elif KCD2VR_M2_HOST_SLOT == 135
// src/mod.cpp: loadconfig_06.inl
// M2_SLOT_BEGIN 135
    settings.bind("EnableTouchBindings", enable_touch_bindings_, true);
// M2_SLOT_END 135
#elif KCD2VR_M2_HOST_SLOT == 137
// src/mod.cpp: loadconfig_07.inl
// M2_SLOT_BEGIN 137
    settings.bind("VrLaserVisible", vr_laser_visible_, true);
// M2_SLOT_END 137
#elif KCD2VR_M2_HOST_SLOT == 139
// src/mod.cpp: loadconfig_08.inl
// M2_SLOT_BEGIN 139
    settings.bind("VrDisplayMode", vr_display_mode_override_, 0);
    settings.bind("VrPadGlyphStyle", vr_pad_glyph_style_, 2);
    settings.bind("VrPromptIdentityLab", vr_prompt_identity_lab_, false);
// M2_SLOT_END 139
#elif KCD2VR_M2_HOST_SLOT == 141
// src/mod.cpp: loadconfig_09.inl
// M2_SLOT_BEGIN 141
    settings.bind("VrNativeUiScale", vr_native_ui_scale_, 1.00f);
// M2_SLOT_END 141
#elif KCD2VR_M2_HOST_SLOT == 143
// src/mod.cpp: loadconfig_10.inl
// M2_SLOT_BEGIN 143
    settings.bind("HudXmlScale", hud_xml_scale_, 0.85f);
    settings.bind("HudXmlOffsetX", hud_xml_offset_x_, 0);
    settings.bind("HudXmlOffsetY", hud_xml_offset_y_, 0);
    settings.bind("VrFlatUiIpdMm", vr_flat_ui_ipd_mm_, 0.0f);
// M2_SLOT_END 143
#elif KCD2VR_M2_HOST_SLOT == 147
// src/mod.cpp: loadconfig_11.inl
// M2_SLOT_BEGIN 147
    future.bind("HideCenterCursor", hide_center_cursor_, false);
    future.bind("EnableWristBodyUI", enable_wrist_body_ui_, false);
// M2_SLOT_END 147
#elif KCD2VR_M2_HOST_SLOT == 149
// src/mod.cpp: loadconfig_12.inl
// M2_SLOT_BEGIN 149
    enable_vr_ui_layout_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableVrUiLayout", true), std::memory_order_relaxed);
// M2_SLOT_END 149
#elif KCD2VR_M2_HOST_SLOT == 150
// src/mod.cpp: loadconfig_13.inl
// M2_SLOT_BEGIN 150
    kcd2vr::ui_menu_input::SetWindowedLaserImageSpaceR16(ReadIniBoolExplicit(explicit_ini,"Settings","VrWindowedLaserImageSpace",false)); // R16b A/B. R8: retired by default (Miki 25 Oct: "scrap that for now")
    hud_lag_on22_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrHudFollowSlow",true)); // C22b: HUD follows the head a little slower (default ON)
    hud_lag22_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrHudLag",0.4f),0.0f,1.0f),std::memory_order_relaxed);
    vr_auto_correct_hud_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrAutoCorrectHud",false));
    vr_hud_native_canvas_fit_.store(false);
    vr_contextual_hud_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrContextualHud",true) /*C37C3 default = Miki ini*/);
// M2_SLOT_END 150
#elif KCD2VR_M2_HOST_SLOT == 151
// src/mod.cpp: loadconfig_14.inl
// M2_SLOT_BEGIN 151
    vr_hud_native_canvas_size_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrHudNativeCanvasSize",1.00f),0.70f,1.50f),std::memory_order_relaxed);
    vr_hud_native_canvas_spread_x_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrHudNativeCanvasSpreadX",0.78f),0.45f,1.10f),std::memory_order_relaxed);
    vr_hud_native_canvas_spread_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrHudNativeCanvasSpreadY",0.78f),0.45f,1.10f),std::memory_order_relaxed);
// M2_SLOT_END 151
#elif KCD2VR_M2_HOST_SLOT == 152
// src/mod.cpp: loadconfig_15.inl
// M2_SLOT_BEGIN 152
    const auto iconScale51c3=ReadIniFloatExplicit(explicit_ini,"Settings","VrAutoHudIconScale",1.75f);
// M2_SLOT_END 152
#elif KCD2VR_M2_HOST_SLOT == 162
// src/mod.cpp: loadconfig_16.inl
// M2_SLOT_BEGIN 162
    vr_ui_compactness_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiCompactness",1.00f),0.25f,1.20f),std::memory_order_relaxed);
// M2_SLOT_END 162
#elif KCD2VR_M2_HOST_SLOT == 164
// src/mod.cpp: loadconfig_17.inl
// M2_SLOT_BEGIN 164
    vr_ui_center_offset_x_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiCenterOffsetX", 0.0f),-500.0f,500.0f),std::memory_order_relaxed);
    vr_ui_center_offset_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiCenterOffsetY", -115.0f),-500.0f,500.0f),std::memory_order_relaxed);
    vr_ui_safe_width_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiSafeWidth", 0.65f) /*C37C3*/,0.60f,1.40f),std::memory_order_relaxed);
    vr_ui_safe_height_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiSafeHeight", 1.01f) /*C37C3*/,0.60f,1.40f),std::memory_order_relaxed);
// M2_SLOT_END 164
#elif KCD2VR_M2_HOST_SLOT == 165
// src/mod.cpp: loadconfig_18.inl
// M2_SLOT_BEGIN 165
    vr_ui_subtitle_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiSubtitleScale", 0.54f) /*C37C3*/,0.30f,1.25f),std::memory_order_relaxed);
    vr_ui_subtitle_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiSubtitleY", -375.0f),-600.0f,600.0f),std::memory_order_relaxed);
// M2_SLOT_END 165
#elif KCD2VR_M2_HOST_SLOT == 167
// src/mod.cpp: loadconfig_19.inl
// M2_SLOT_BEGIN 167
    vr_ui_item_panel_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiItemPanelScale",0.73f),0.45f,1.25f),std::memory_order_relaxed);
    vr_ui_item_panel_x_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiItemPanelX",-380.0f),-700.0f,700.0f),std::memory_order_relaxed);
    vr_ui_item_panel_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiItemPanelY",-300.0f),-450.0f,450.0f),std::memory_order_relaxed);
// M2_SLOT_END 167
#elif KCD2VR_M2_HOST_SLOT == 169
// src/mod.cpp: loadconfig_20.inl
// M2_SLOT_BEGIN 169
    vr_ui_hints_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiHintsScale", 0.64f),0.25f,1.50f),std::memory_order_relaxed);
    vr_ui_hints_inset_pct_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiHintsInsetPct", 33.0f),0.0f,75.0f),std::memory_order_relaxed);
    vr_ui_hints_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiHintsY", -185.0f),-600.0f,600.0f),std::memory_order_relaxed);
    vr_ui_control_prompt_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiControlPromptScale",0.70f),0.30f,1.50f),std::memory_order_relaxed);
    vr_ui_control_prompt_x_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiControlPromptX",0.0f),-700.0f,700.0f),std::memory_order_relaxed);
    vr_ui_control_prompt_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiControlPromptY",0.0f),-600.0f,600.0f),std::memory_order_relaxed);
    vr_ui_compass_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiCompassScale",1.0f),0.15f,1.50f),std::memory_order_relaxed);
    vr_ui_compass_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiCompassY",0.0f),-20.0f,100.0f),std::memory_order_relaxed);
    vr_ui_stats_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiStatsScale",1.0f),0.15f,1.50f),std::memory_order_relaxed);
    vr_ui_stats_x_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiStatsX",0.0f),-700.0f,700.0f),std::memory_order_relaxed);
    vr_ui_stats_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiStatsY",0.0f),-20.0f,100.0f),std::memory_order_relaxed);
    vr_ui_lower_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiLowerScale",1.0f),0.15f,2.00f),std::memory_order_relaxed);
    vr_ui_lower_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiLowerY",0.0f),-500.0f,500.0f),std::memory_order_relaxed);
    vr_ui_lower_spread_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiLowerSpread",0.0f),-400.0f,400.0f),std::memory_order_relaxed);
    vr_ui_hide_letterbox_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrUiHideLetterbox",false),std::memory_order_relaxed);
// M2_SLOT_END 169
#elif KCD2VR_M2_HOST_SLOT == 173
// src/mod.cpp: loadconfig_21.inl
// M2_SLOT_BEGIN 173
    enable_touch_bindings_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableTouchBindings", true), std::memory_order_relaxed);
// M2_SLOT_END 173
#elif KCD2VR_M2_HOST_SLOT == 176
// src/mod.cpp: loadconfig_22.inl
// M2_SLOT_BEGIN 176
    vr_pad_glyph_style_.store(ReadIniIntExplicit(explicit_ini, "Settings", "VrPadGlyphStyle", 2)==4?4:2,std::memory_order_relaxed);
    vr_prompt_identity_lab_.store(kcd2vr::developer_mode::Enabled()&&ReadIniBoolExplicit(explicit_ini,"Settings","VrPromptIdentityLab",false),std::memory_order_relaxed);
    vr_laser_visible_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "VrLaserVisible", true),std::memory_order_relaxed);
// M2_SLOT_END 176
#elif KCD2VR_M2_HOST_SLOT == 189
// src/mod.cpp: loadconfig_23.inl
// M2_SLOT_BEGIN 189
    hud_crosshair_visible_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrHudCrosshairVisible",false));
    hud_compass_visible_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrHudCompassVisible",true));
    hud_status_icons_visible_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrHudStatusIconsVisible",false));
// M2_SLOT_END 189
#elif KCD2VR_M2_HOST_SLOT == 190
// src/mod.cpp: loadconfig_24.inl
// M2_SLOT_BEGIN 190
    vr_contextual_hud_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrContextualHud",true) /*C37C3 default = Miki ini*/);
// M2_SLOT_END 190
#elif KCD2VR_M2_HOST_SLOT == 214
// src/mod.cpp: loadconfig_25.inl
// M2_SLOT_BEGIN 214
    vr_display_mode_override_.store(0,std::memory_order_relaxed); // ignore retired persistent Theater setting
// M2_SLOT_END 214
#elif KCD2VR_M2_HOST_SLOT == 216
// src/mod.cpp: loadconfig_26.inl
// M2_SLOT_BEGIN 216
    vr_native_ui_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrNativeUiScale", 1.00f),0.50f,1.20f), std::memory_order_relaxed);
// M2_SLOT_END 216
#elif KCD2VR_M2_HOST_SLOT == 218
// src/mod.cpp: loadconfig_27.inl
// M2_SLOT_BEGIN 218
    hud_xml_scale_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "HudXmlScale", 0.85f), std::memory_order_relaxed);
    hud_xml_offset_x_.store(ReadIniIntExplicit(explicit_ini, "Settings", "HudXmlOffsetX", 0), std::memory_order_relaxed);
    hud_xml_offset_y_.store(ReadIniIntExplicit(explicit_ini, "Settings", "HudXmlOffsetY", 0), std::memory_order_relaxed);
    vr_flat_ui_ipd_mm_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrFlatUiIpdMm", 0.0f), std::memory_order_relaxed);
// M2_SLOT_END 218
#elif KCD2VR_M2_HOST_SLOT == 221
// src/mod.h: mod_state_01.inl
// M2_SLOT_BEGIN 221
    std::atomic<bool> enable_vr_ui_layout_{true};
    std::atomic<bool> vr_ui_auto_layout_{false};      // V64.0 F4 reversible MasterSwitch auto-layout lab
    std::atomic<float> auto_hud_icon_scale_{1.75f};
    std::atomic<unsigned> hud_session_rung57c1_{0}; // retired presentation rung; F2 capture never owns HUD
    std::atomic<float> vr_ui_compactness_{1.00f};     // semantic HUD scale
    std::atomic<float> vr_ui_safe_width_{1.03f};       // V64.83 safe-zone horizontal size multiplier
    std::atomic<float> vr_ui_safe_height_{1.03f};      // V64.83 legacy/semantic vertical size multiplier
    std::atomic<float> vr_ui_top_boundary_{0.90f};     // V66.117R1 uniform HUD upper boundary factor
    std::atomic<float> vr_ui_bottom_boundary_{0.90f};  // V66.117R1 uniform HUD lower boundary factor
    std::atomic<float> vr_ui_center_offset_x_{0.0f};   // V64.51 centered HUD live global X offset in Flash design px
    std::atomic<float> vr_ui_center_offset_y_{-115.0f};   // V64.51 centered HUD live global Y offset in Flash design px
    std::atomic<bool> vr_hud_reapply_requested_{false}; // V64.51 one-shot live centered-HUD recompute
    std::atomic<bool> vr_hud_native_canvas_fit_{false};
    std::atomic<float> vr_hud_native_canvas_size_{1.00f}; // V66.98 element/icon scale only; no longer controls anchor distance
    std::atomic<float> vr_hud_native_canvas_spread_x_{0.78f}; // V66.98 inward anchor spacing, independent from icon scale
    std::atomic<float> vr_hud_native_canvas_spread_y_{0.78f};
    std::atomic<bool> hud_lag_on22_{true}; std::atomic<float> hud_lag22_{0.4f}; // C22b VrHudFollowSlow (on/off, menu) + VrHudLag strength (INI)
    std::atomic<bool> vr_auto_correct_hud_{false};       // V65.04 cached corrected-HUD experiment, all vision modes
    std::atomic<bool> vr_corrected_hud_dirty_{false};    // event-driven apply only; never a polling retry loop
// M2_SLOT_END 221
#elif KCD2VR_M2_HOST_SLOT == 223
// src/mod.h: mod_state_02.inl
// M2_SLOT_BEGIN 223
    std::atomic<float> vr_ui_subtitle_scale_{0.54f};
    std::atomic<float> vr_ui_subtitle_y_{0.0f};          // design pixels relative to vanilla bc.subtitles
// M2_SLOT_END 223
#elif KCD2VR_M2_HOST_SLOT == 228
// src/mod.h: mod_state_03.inl
// M2_SLOT_BEGIN 228
    std::atomic<float> vr_ui_item_panel_scale_{1.00f};
    std::atomic<float> vr_ui_item_panel_x_{0.0f};
    std::atomic<float> vr_ui_item_panel_y_{0.0f};
    std::atomic<float> vr_ui_hints_scale_{0.64f};
    std::atomic<float> vr_ui_hints_inset_pct_{33.0f};    // percent of half-screen, moves br.actionHints left
    std::atomic<float> vr_ui_hints_y_{-185.0f};             // design pixels relative to vanilla actionHints
    std::atomic<float> vr_ui_control_prompt_scale_{0.70f};   // TutorialMessage (tl.tutorial)
    std::atomic<float> vr_ui_control_prompt_x_{0.0f};
    std::atomic<float> vr_ui_control_prompt_y_{0.0f};
// M2_SLOT_END 228
#elif KCD2VR_M2_HOST_SLOT == 229
// src/mod.h: mod_state_04.inl
// M2_SLOT_BEGIN 229
    std::atomic<float> vr_ui_compass_scale_{1.00f};
    std::atomic<float> vr_ui_compass_y_{0.0f};
    std::atomic<float> vr_ui_stats_scale_{1.00f};
    std::atomic<float> vr_ui_stats_x_{0.0f};
    std::atomic<float> vr_ui_stats_y_{0.0f};
    std::atomic<float> vr_ui_lower_scale_{1.00f};
    std::atomic<float> vr_ui_lower_y_{0.0f};
    std::atomic<float> vr_ui_lower_spread_{0.0f};
    std::atomic<bool> vr_ui_hide_letterbox_{false};
// M2_SLOT_END 229
#elif KCD2VR_M2_HOST_SLOT == 230
// src/mod.h: mod_state_05.inl
// M2_SLOT_BEGIN 230
    std::atomic<bool> hud_crosshair_visible_{false};
    std::atomic<bool> hud_lower_icons_visible_{true};
    std::atomic<bool> hud_compass_visible_{true};
    std::atomic<bool> hud_status_icons_visible_{false};
    std::atomic<bool> vr_contextual_hud_{false}; // compatibility setting; no automatic native visibility suppression
// M2_SLOT_END 230
#elif KCD2VR_M2_HOST_SLOT == 234
// src/mod.h: mod_state_06.inl
// M2_SLOT_BEGIN 234
    std::atomic<bool> enable_touch_bindings_{true};
// M2_SLOT_END 234
#elif KCD2VR_M2_HOST_SLOT == 237
// src/mod.h: mod_state_07.inl
// M2_SLOT_BEGIN 237
    std::atomic<bool> vr_laser_visible_{true};
// M2_SLOT_END 237
#elif KCD2VR_M2_HOST_SLOT == 240
// src/mod.h: mod_state_08.inl
// M2_SLOT_BEGIN 240
    std::atomic<int> vr_display_mode_override_{0}; // 0=AUTO, 1=FULL VR, 2=THEATER
    std::atomic<int> vr_pad_glyph_style_{2}; // prompt presentation only
    std::atomic<bool> vr_prompt_identity_lab_{false}; // F7 Controls-page gate; never owns gameplay transport
// M2_SLOT_END 240
#elif KCD2VR_M2_HOST_SLOT == 241
// src/mod.h: mod_state_09.inl
// M2_SLOT_BEGIN 241
    std::atomic<float> vr_native_ui_scale_{1.00f}; // V61.38 live HUD root scale request, 0.50..1.20; no startup write
// M2_SLOT_END 241
#elif KCD2VR_M2_HOST_SLOT == 243
// src/mod.h: mod_state_10.inl
// M2_SLOT_BEGIN 243
    std::atomic<float> hud_xml_scale_{0.85f};
    std::atomic<int> hud_xml_offset_x_{0};
    std::atomic<int> hud_xml_offset_y_{0};
    std::atomic<float> vr_flat_ui_ipd_mm_{0.0f};
// M2_SLOT_END 243
#elif KCD2VR_M2_HOST_SLOT == 248
// src/mod.h: mod_state_11.inl
// M2_SLOT_BEGIN 248
    std::atomic<bool> hide_center_cursor_{false};
    std::atomic<bool> enable_wrist_body_ui_{false};
// M2_SLOT_END 248
#elif KCD2VR_M2_HOST_SLOT == 250
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 250
    float native_ui_scale = 1.00f;          // V61.38 live HUD root-scale request, 0.50..1.20
// M2_SLOT_END 250
#elif KCD2VR_M2_HOST_SLOT == 252
// src/vr_menu.h: menu_state_02.inl
// M2_SLOT_BEGIN 252
    bool hud_live_observed = false;
    bool ui_runtime_build_supported = false;
// M2_SLOT_END 252
#elif KCD2VR_M2_HOST_SLOT == 253
// src/vr_menu.h: menu_state_03.inl
// M2_SLOT_BEGIN 253
    float hud_xml_scale = 0.85f;             // V61.20 generated HUD.xml fixed-rectangle scale
    int hud_xml_offset_x = 0;                 // pixels in 1872x2016 HUD canvas coordinates
    int hud_xml_offset_y = 0;                 // negative moves HUD upward
    float flat_ui_ipd_mm = 0.0f;            // F6 projection-flat convergence/IPD diagnostic; 0 = identical eye geometry
// M2_SLOT_END 253
#elif KCD2VR_M2_HOST_SLOT == 254
// src/vr_menu.h: menu_state_04.inl
// M2_SLOT_BEGIN 254
    bool contextual_hud = false; // VRIK-only palm gaze; native/reference remains full HUD
    bool hud_native_canvas_fit = false;
    float hud_native_canvas_size = 1.00f; // V66.98 icon/text scale only
    float hud_native_canvas_spread_x = 0.78f;
    float hud_native_canvas_spread_y = 0.78f;
    bool hud_lag_on = true;                 // C22b VrHudFollowSlow: HUD follows the head a little slower
    bool auto_correct_hud = false;          // V66.148 V65.22/V66.118R1 whole-HUD AutoHUD owner; suppresses centered fallback while ON
    bool vr_ui_layout = true;              // V62.4 native semantic HUD master
    bool vr_ui_auto_layout = false;        // V64.0 F4: root HUD follows final VR visual center
    bool hud_corrected_active = false; // current F2 session or AutoHUD owner, read-only
    float auto_hud_icon_scale = 1.75f; // independent readable-parent scale, AutoHUD/F2 only
    float vr_ui_compactness = 1.00f;       // centered HUD semantic element scale
    float vr_ui_safe_width = 1.03f;         // V64.83 safe-zone horizontal size multiplier
    float vr_ui_safe_height = 1.03f;        // V64.83 legacy/semantic vertical size multiplier
    float vr_ui_top_boundary = 0.90f;       // V66.115R3 uniform HUD: upper anchor boundary about authored center
    float vr_ui_bottom_boundary = 0.90f;    // V66.115R3 uniform HUD: lower anchor boundary about authored center
    float vr_ui_center_offset_x = 0.0f;    // V64.51 centered HUD global horizontal offset, design px
    float vr_ui_center_offset_y = -115.0f;    // V64.51 centered HUD global vertical offset, design px
// M2_SLOT_END 254
#elif KCD2VR_M2_HOST_SLOT == 257
// src/vr_menu.h: menu_state_05.inl
// M2_SLOT_BEGIN 257
    float vr_ui_subtitle_scale = 0.54f; // V64.20 centered HUD comfort-zone fraction (legacy field repurposed)
    float vr_ui_subtitle_y = 0.0f;
// M2_SLOT_END 257
#elif KCD2VR_M2_HOST_SLOT == 260
// src/vr_menu.h: menu_state_06.inl
// M2_SLOT_BEGIN 260
    float vr_ui_item_panel_scale = 0.73f;
    float vr_ui_item_panel_x = -380.0f;
    float vr_ui_item_panel_y = -300.0f;
    float vr_ui_hints_scale = 0.64f;
    float vr_ui_hints_inset_pct = 33.0f;
    float vr_ui_hints_y = -185.0f;
    float vr_ui_control_prompt_scale = 0.70f; // tl.tutorial / general tutorial-control prompt layer
    float vr_ui_control_prompt_x = 0.0f;
    float vr_ui_control_prompt_y = 0.0f;
// M2_SLOT_END 260
#elif KCD2VR_M2_HOST_SLOT == 261
// src/vr_menu.h: menu_state_07.inl
// M2_SLOT_BEGIN 261
    float vr_ui_compass_scale = 1.00f;
    float vr_ui_compass_y = 0.0f;           // percent of screen height, positive upward
    float vr_ui_stats_scale = 1.00f;
    float vr_ui_stats_x = 0.0f;             // stage px from screen centre
    float vr_ui_stats_y = 0.0f;             // percent of screen height, positive upward
    float vr_ui_lower_scale = 1.00f;        // quick weapon/food slots
    float vr_ui_lower_y = 0.0f;             // percent of screen height
    float vr_ui_lower_spread = 0.0f;        // percent, symmetric horizontal spread
    bool vr_ui_hide_letterbox = false;       // experimental Dynamic-HUD-style HUD strip suppression
// M2_SLOT_END 261
#elif KCD2VR_M2_HOST_SLOT == 262
// src/vr_menu.h: menu_state_08.inl
// M2_SLOT_BEGIN 262
    bool hud_status_icons_visible = false;
    bool hud_crosshair_visible = false;
    bool hud_lower_icons_visible = true;   // Stats container
    bool hud_compass_visible = true;
// M2_SLOT_END 262
#elif KCD2VR_M2_HOST_SLOT == 266
// src/vr_menu.h: menu_state_09.inl
// M2_SLOT_BEGIN 266
    bool touch_bindings = true;
// M2_SLOT_END 266
#elif KCD2VR_M2_HOST_SLOT == 313
// src/vr_menu.h: menu_state_10.inl
// M2_SLOT_BEGIN 313
    bool vr_laser_visible = true;          // V64.17: visual only; aim hit-test remains active
    int vr_display_mode = 0;               // V64.89 menu policy: 0 auto context rules, 2 theater for menus/scenes; manual F6 is separate
    int vr_pad_glyph_style = 2;             // 2 Xbox, 4 PlayStation prompt family
    bool vr_prompt_identity_lab = false;      // V65.10b F7-gated UI-only controller identity lab
// M2_SLOT_END 313
#elif KCD2VR_M2_HOST_SLOT == 5001
// src/mod.cpp: native_ui_layout
// M2_SLOT_BEGIN 5001

// M2_SLOT_END 5001
#elif KCD2VR_M2_HOST_SLOT == 5002
// src/mod.cpp: hud_semantic_scale
// M2_SLOT_BEGIN 5002

// M2_SLOT_END 5002
#elif KCD2VR_M2_HOST_SLOT == 5004
// src/mod.cpp: glyph_presentation
// M2_SLOT_BEGIN 5004
bool ApplyControllerGlyphsV6476(int inputId) noexcept
{
    // Retail rejects console Lua. Queue the same presentation-only request for
    // the qualified game VM instead; TouchBindings remains the input owner.
    return prompt_device_lock::RequestFlashStyleD56(inputId);
}

bool ApplyControllerPromptPresentationV6477(int inputId) noexcept
{
    // V64.77 restores the V64.22B rule: presentation only.  Do not touch the
    // global input-device classifier because the golden keyboard bridge is the
    // actual controller transport.
    return ApplyControllerGlyphsV6476(inputId==4?4:2);
}

// M2_SLOT_END 5004
#elif KCD2VR_M2_HOST_SLOT == 5005
// src/mod.cpp: prompt_lease
// M2_SLOT_BEGIN 5005
static bool g_f7_prompt_active_d33=false;static int g_f7_prompt_style_d33=2; // D33: shared with RestyleF7PromptPresentationD33
bool ApplyF7PromptPresentation(bool enabled, int flashStyle) noexcept
{
    bool& active=g_f7_prompt_active_d33;
    static bool have_restore=false;
    static int restore_preferred=0;
    if(enabled==active) return true;

    void* console=SafeGetCryConsole();
    void* preferred=console?SafeConsoleGetCVar(console,"wh_ui_PreferredInputDevice"):nullptr;
    if(!preferred) {
        diagnostic_trace::Write("V65.11-F11 rung2 prompt CVar unavailable; fail-closed");
        return false;
    }

    if(enabled) {
        restore_preferred=SafeCVarGetInt(preferred,0);
        have_restore=true;
        const int preferred_id=2; // D68 Xbox semantics; PS is art only in the shared button movie.
        const bool cvar_ok=SafeCVarSetInt(preferred,preferred_id);
        const bool flash_ok=ApplyControllerPromptPresentationV6477(flashStyle==4?4:2);
        if(!cvar_ok) { have_restore=false; return false; }
        active=true;g_f7_prompt_style_d33=flash_ok?(flashStyle==4?4:2):0;
        char m[320]{};
        std::snprintf(m,sizeof(m),
            "D56 GLYPH_ONLY preferred=%d flashId=%d flashQueued=%d transport=KEYBOARD_CONTEXTUAL edgeTriggered=1",
            preferred_id,flashStyle==4?4:2,flash_ok?1:0);
        diagnostic_trace::Write(m);
        return flash_ok;
    }

    bool ok=true;
    if(have_restore) ok=SafeCVarSetInt(preferred,restore_preferred);
    active=false;
    have_restore=false;
    char m[256]{};
    std::snprintf(m,sizeof(m),
        "V65.11-F11 GLYPH-ONLY OFF restoredPreferred=%d ok=%d edgeTriggered=1",
        restore_preferred,ok?1:0);
    diagnostic_trace::Write(m);
    return ok;
}
// D68: preferred device stays Xbox2 for both art choices. Only the separate
// Flash glyph callback selects PS art; native action labels/layout stay Xbox.
// Startup/shutdown retain the existing prompt lease and saved style.
bool RestyleF7PromptPresentationD33(int flashStyle) noexcept
{
    const int style=flashStyle==4?4:2;
    // A startup CVar/VM miss must not permanently disable later menu changes.
    // Retry on this explicit style/UI event; never poll for presentation.
    if(!g_f7_prompt_active_d33)return ApplyF7PromptPresentation(true,style);
    if(style==g_f7_prompt_style_d33)return true;
    void* console=SafeGetCryConsole();
    void* preferred=console?SafeConsoleGetCVar(console,"wh_ui_PreferredInputDevice"):nullptr;
    if(!preferred)return false;
    const bool cvar_ok=SafeCVarSetInt(preferred,2);
    const bool flash_ok=ApplyControllerPromptPresentationV6477(style);
    // The style cache means queued successfully, not just native family written.
    // Keep a failed queue pending so a later actual menu/UI event can retry it.
    if(cvar_ok&&flash_ok)g_f7_prompt_style_d33=style;
    char m[200]{};std::snprintf(m,sizeof(m),"D56 PROMPT_STYLE style=%s preferred=%d cvar=%d flashQueued=%d",style==4?"PLAYSTATION":"XBOX",2,cvar_ok?1:0,flash_ok?1:0);
    diagnostic_trace::Write(m);
    return cvar_ok&&flash_ok;
}

// M2_SLOT_END 5005
#elif KCD2VR_M2_HOST_SLOT == 5006
// src/mod.cpp: adaptive_hud_lease
// M2_SLOT_BEGIN 5006
struct AdaptiveHudBackend57c1 {
    void* Resolve() noexcept {
        const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
        if(!retail_build112::Ready(b))return nullptr;
        return SafeConsoleGetCVar(SafeGetCryConsole(),"wh_ui_EnableAdaptiveHud");
    }
    bool Read(int& value) noexcept {auto* v=Resolve();value=SafeCVarGetInt(v,INT_MIN);return value!=INT_MIN;}
    bool Write(int value) noexcept {auto* v=Resolve();return v&&SafeCVarSetInt(v,value);}
};
static ui_adaptive_hud57c1::Lease g_adaptive_hud57c1;
// R59.16C10R16: the bow's centre point is the game's "Show cursor while missile weapon is in hand" (wh_pl_ShowFireCursor). With the
// crosshair hidden in the VR menu it is leased to 0 the same way (restored when the crosshair is shown again or the mod unloads).
struct CVarLeaseBackend59c16 {
    const char* name=nullptr;
    void* Resolve() noexcept {
        const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
        if(!retail_build112::Ready(b))return nullptr;
        return SafeConsoleGetCVar(SafeGetCryConsole(),name);
    }
    bool Read(int& value) noexcept {auto* v=Resolve();value=SafeCVarGetInt(v,INT_MIN);return value!=INT_MIN;}
    bool Write(int value) noexcept {auto* v=Resolve();return v&&SafeCVarSetInt(v,value);}
};
static ui_adaptive_hud57c1::Lease g_fire_cursor59c16;
static prompt_policy_d80::HintsLease g_show_hints_d80;
static std::atomic<std::uint64_t> g_hud_rehidden59c16{0};
// M2_SLOT_END 5006
#elif KCD2VR_M2_HOST_SLOT == 5012
// src/mod.cpp: pause_blur_backend
// M2_SLOT_BEGIN 5012
struct PauseBlurConsole124R1 {
    bool Get(const char* name,int& value) noexcept {void* c=SafeGetCryConsole();void* v=c?SafeConsoleGetCVar(c,name):nullptr;if(!v)return false;value=SafeCVarGetInt(v,-99);return value!=-99;}
    bool Set(const char* name,int value) noexcept {void* c=SafeGetCryConsole();void* v=c?SafeConsoleGetCVar(c,name):nullptr;return v&&SafeCVarSetInt(v,value);}
};
// M2_SLOT_END 5012
#elif KCD2VR_M2_HOST_SLOT == 5016
// src/mod.cpp: utility_talk_gestures
// M2_SLOT_BEGIN 5016
struct GestureFrame59c5 {bool valid=false;XrPose head{};ControllerState hands[2]{};}; // hands[0]=right, [1]=left
static GestureFrame59c5 g_gesture_frame59c5{};
static void ApplyUtilityGestures54(native_vr_pad_premap::State& pad,const native_vr_pad_premap::Context& c,bool trackedGameplay) noexcept
{
    namespace ug=kcd2vr::utility54;static ug::State state{};
    const auto now=std::uint64_t(GetTickCount64());ug::Input i{};i.tick=now;i.trigger=g_gesture_frame59c5.hands[1].trigger;i.grip=g_gesture_frame59c5.hands[1].grip;
    i.torchEnabled=ug::torchGestureD39.load(std::memory_order_acquire);i.visorEnabled=ug::visorGestureD39.load(std::memory_order_acquire);
    const bool semantic=!c.vr_menu&&!c.theater&&!c.system_menu&&!c.dialogue_navigation&&!c.semantic_minigame&&!c.apse_ui&&!c.back_context;
    // VrGestureTalk owns optional index-point Quick Talk only. Its preference
    // must not also disable the independent torch or visor gestures.
    i.enabled=semantic&&trackedGameplay&&frik_pose_probe::CalibrationReady()&&
        !animation_filter::nativeBodyOwned112.load();
    if(g_gesture_frame59c5.valid){
        const auto& h=g_gesture_frame59c5.head;const auto& l=g_gesture_frame59c5.hands[1];
        float fx=-2.f*(h.qw*h.qy+h.qx*h.qz),fz=-1.f+2.f*(h.qx*h.qx+h.qy*h.qy);
        const float len=std::sqrt(fx*fx+fz*fz);
        i.valid=l.active&&std::isfinite(len)&&len>1e-4f;
        if(i.valid){fx/=len;fz/=len;const double x=l.pose.pos_x-h.pos_x,z=l.pose.pos_z-h.pos_z;
            i.right=x*(-fz)+z*fx;i.forward=x*fx+z*fz;i.up=l.pose.pos_y-h.pos_y;i.trigger=l.trigger;
            const int held=frik_pose_probe::UtilityLeftItemR114(now); // R114: torch stow survives a late equipment copy
            const int physicalHeld55=frik_pose_probe::ReadPhysicalHandHeld55(1,now);
            i.empty=held==0&&physicalHeld55==0;i.torch=held==11;
        }
    }
    const bool sameClock55=state.last==i.tick;
    const auto r=ug::update(state,i);
    // One input-edge receipt during explicit discovery captures a refused
    // gesture as well as a successful request. No idle/per-frame logging.
    static bool triggerDown55=false;
    const bool pressed55=i.trigger>=.70&&!triggerDown55;
    if(i.trigger>=.70)triggerDown55=true;else if(i.trigger<.25)triggerDown55=false;
    if(pressed55&&
       (diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)||
        diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))){
        char m[350]{};std::snprintf(m,sizeof(m),
            "C55 UTILITY_INPUT enabled=%d semantic=%d tracked=%d valid=%d empty=%d torch=%d relative=(%.3f,%.3f,%.3f) trigger=%.2f sameClock=%d armedZone=%d blocked=%d nativeBody=%d",
            i.enabled,semantic,trackedGameplay,i.valid,i.empty,i.torch,i.right,i.forward,i.up,i.trigger,sameClock55,int(state.zone),state.blocked,
            animation_filter::nativeBodyOwned112.load()?1:0);diagnostic_trace::Write(m);
    }
    // Torch requests retain the immutable gesture time; no repeated XI pulse.
    if(semantic&&r.consumesTrigger)pad.lt=0; // quarantine persists through HMD loss, never overrides native UI; VR guard may still own LT later
    if(r.action==ug::Action::Torch)(void)frik_pose_probe::RequestTorch62(true,now);
    else if(r.action==ug::Action::VisorOpen||r.action==ug::Action::VisorClose)
        (void)frik_pose_probe::RequestVisor54(r.action==ug::Action::VisorClose,now);
    // Native callback performs both draw/stow and all native equipment checks.
    if(r.action!=ug::Action::None){char m[210]{};std::snprintf(m,sizeof(m),
        "C54 UTILITY_GESTURE action=%s leftTrigger=1 transport=%s nativeOutcome=UNOBSERVED",
        r.action==ug::Action::Torch?"TORCH":r.action==ug::Action::VisorClose?"VISOR_CLOSE":"VISOR_OPEN",
        r.action==ug::Action::Torch?"PINNED_NATIVE_FRAME_TORCH":"PINNED_NATIVE_FRAME_VISOR");diagnostic_trace::Write(m);}
}

static kcd2vr::gesture_talk59c5::Output PrepareGestureTalkH4(const native_vr_pad_premap::Context& padCtx,bool trackedGameplay) noexcept
{
                namespace gt=kcd2vr::gesture_talk59c5;static gt::State gesture59c5{};
                const auto gNow=std::uint64_t(GetTickCount64());
                const bool chatOpen=padCtx.quick_talk_open54;
                const bool gameplay=!padCtx.vr_menu&&!padCtx.theater&&!padCtx.system_menu&&!padCtx.dialogue_navigation&&
                    !padCtx.semantic_minigame&&!padCtx.apse_ui&&(!padCtx.back_context||chatOpen)&&trackedGameplay;
                gt::Config gestureConfig47{};gestureConfig47.holdPoint=true;gestureConfig47.manualPoint54=true;gestureConfig47.requireIndexOffTrigger=true;gestureConfig47.pointHoldMs=0;gestureConfig47.pointMinUp=-.90;gestureConfig47.automaticVoice=false; // R93R1: optional immediate index-point chooser; native choices only, original cooldown retained
                kcd2vr::grasp13::PendingUse55 pendingPickupD78{};
                const bool hasPendingPickupD78=kcd2vr::action_time_owner59c11::ReadPendingUse55(pendingPickupD78)&&
                    pendingPickupD78.serial&&pendingPickupD78.character&&pendingPickupD78.epoch&&
                    pendingPickupD78.requestedAt&&gNow>=pendingPickupD78.requestedAt&&gNow-pendingPickupD78.requestedAt<=1500;
                kcd2vr::hand_only_door_h40::Admission doorTalkAdmissionH40[2];
                gt::Hand hands[2]{};
                if(trackedGameplay&&g_gesture_talk59c5.load(std::memory_order_relaxed)&&g_gesture_frame59c5.valid){const auto& head_pose=g_gesture_frame59c5.head;
                    // OpenXR look direction = head quaternion applied to (0,0,-1); keep its horizontal part (Y is up).
                    const float qx=head_pose.qx,qy=head_pose.qy,qz=head_pose.qz,qw=head_pose.qw;
                    float fx=-2.f*(qw*qy+qx*qz),fz=-1.f+2.f*(qx*qx+qy*qy);
                    const float fl=std::sqrt(fx*fx+fz*fz);if(fl>1e-4f){fx/=fl;fz/=fl;}else{fx=0.f;fz=-1.f;} // horizontal look direction
                    for(int side=0;side<2;++side){const auto& c=g_gesture_frame59c5.hands[side];
                        auto& h=hands[side];h.valid=c.active&&c.aim_active&&
                            !(kcd2vr::corpse_grip_cg1::claims.reserved()&(1u<<side))&&
                            !(hasPendingPickupD78&&pendingPickupD78.side==side)&&doorTalkAdmissionH40[side].take(1u<<side);
                        // DS-2026-10-23 (Miki: near someone the GRAB owns the hand, not the quick talk).
                        // Only a HELD grip on a living NPC suppresses the talk - a mere brush keeps it
                        // (the Codex T27 finding 6: no incidental-contact invalidation).
                        if(h.valid&&c.grip>=.60f&&kcd2vr::grab_over_talk_ds::LivingNear(side,gNow,250))h.valid=false;
                        // A support hand has no native held-item WUID. It is
                        // still occupied by the committed two-handed solve.
                        h.empty=frik_pose_probe::ExactHandItemState(side)==0&&
                            frik_pose_probe::ReadPhysicalHandHeld55(side,gNow)==0;
                        // An already committed holster grasp keeps this Grip
                        // while withdrawing; it is not a new QuickTalk point.
                        if(c.grip>.55f&&frik_pose_probe::ReadHolsterGripOwnerR15(side,gNow))h.valid=false;
                        h.grip=c.grip;h.trigger=c.trigger;h.triggerTouch=c.trigger_touch;
                        const float rx=c.aim_pose.pos_x-head_pose.pos_x,ry=c.aim_pose.pos_y-head_pose.pos_y,rz=c.aim_pose.pos_z-head_pose.pos_z;
                        const auto& aq=c.aim_pose;
                        const double ax=-2.0*(aq.qw*aq.qy+aq.qx*aq.qz),ay=2.0*(aq.qw*aq.qx-aq.qy*aq.qz),az=-1.0+2.0*(aq.qx*aq.qx+aq.qy*aq.qy);
                        h.outwardR26=gt::OutwardPointR26(rx,ry,rz,ax,ay,az,fx,fz);
                        h.forward=double(rx*fx+rz*fz);h.up=double(ry);}
                }
                // Observe/query an NPC only while a current free hand is in
                // the existing pointing envelope. Idle gameplay adds no script
                // calls; pending pickup/door/holster owners cannot request one.
                const bool pointRequestedD78=g_gesture_talk59c5.load(std::memory_order_acquire)&&gameplay&&!chatOpen&&
                    (gt::retainedPointH4(hands[0],gestureConfig47)||gt::retainedPointH4(hands[1],gestureConfig47));
                const bool pointTargetD78=kcd2vr::quick_talk_target_d78::Prepare(
                    pointRequestedD78&&kcd2vr::misc_gestures_d59::Ensure(gNow),gNow);
                // The native-open chooser keeps its original owner. A new
                // point needs this NPC's enabled descriptor; raised battle cry
                // retains its existing native route and does not need a target.
                for(auto& hand:hands)hand.outwardR26=hand.outwardR26&&(chatOpen||pointTargetD78);
                const bool inCombat=combat_diag_probe::HenryInCombat178r31(gNow)==1;
                static unsigned previousChoices54=0;
                const unsigned choices54=(g_gesture_frame59c5.hands[0].a_down?1u:0u)|(g_gesture_frame59c5.hands[0].b_down?2u:0u)|
                    (g_gesture_frame59c5.hands[1].x_down?4u:0u)|(g_gesture_frame59c5.hands[1].y_down?8u:0u);
                const bool selected54=chatOpen&&(choices54&~previousChoices54)!=0;previousChoices54=choices54;
                static std::uint64_t selfPatSeen54=0,selfPatTick54=0;
                kcd2vr::self_contact54::Event selfPat54{};
                bool patFired54=false;
                if(gestureConfig47.automaticVoice&&frik_pose_probe::ReadSelfPat54(selfPat54,gNow)&&
                   (selfPat54.serial!=selfPatSeen54||selfPat54.tick!=selfPatTick54)){
                    selfPatSeen54=selfPat54.serial;selfPatTick54=selfPat54.tick; // context refusal cannot defer a gesture into a later menu/fight
                    patFired54=gt::battleCryContact54(gesture59c5,gNow,true,
                        g_gesture_talk59c5.load(std::memory_order_relaxed)&&gameplay&&!gt::combatChord(hands),inCombat,gestureConfig47);
                }
                auto g=gt::update(gesture59c5,gNow,hands,g_gesture_talk59c5.load(std::memory_order_relaxed)&&gameplay,inCombat,chatOpen,gestureConfig47,selected54);
                if(patFired54)g.fired=gt::Kind::BattleCry;
                frik_pose_probe::PublishQuickTalkOwnerH4(g.claimMaskH4,gNow);
                if(g.fired!=gt::Kind::None||g.chatTimeout){char m[200]{};std::snprintf(m,sizeof(m),"R59.16C5 GESTURE_TALK %s inCombat=%d chatOpen=%d route=NATIVE_QUICK_TALK option=%s",
                    g.chatTimeout?"CHAT_DID_NOT_OPEN":g.fired==gt::Kind::Talk?"POINT_TALK":g.fired==gt::Kind::Taunt?"POINT_TAUNT":patFired54?"CONTACT_BATTLE_CRY":"RAISE_BATTLE_CRY",inCombat?1:0,chatOpen?1:0,
                    g.chatTimeout?"NONE":g.fired==gt::Kind::Taunt?"3_X":g.fired==gt::Kind::BattleCry?"2_B":"MENU_ONLY");diagnostic_trace::Write(m);}
                return g;
            }

// M2_SLOT_END 5016
#elif KCD2VR_M2_HOST_SLOT == 5018
// src/mod.cpp: palm_hud_stage
// M2_SLOT_BEGIN 5018
        // V66.69 F4 IMMERSIVE UI runtime. Stats/Compass are the only Flash clips moved;
        // fullscreen fader/vignette/ratio-strip ownership stays untouched. Flash writes are
        // capped at 10 Hz and only occur after a material wrist-position/visibility change.
        if(xr_manager_ && f4_immersive_ui_rung>0){
            XrManager::RenderFrameState rf{};
            const bool frameOk=xr_manager_->CopyActiveRenderFrameState(rf)&&rf.stereo_views_valid;
            auto qrot=[](const XrQuaternionf& q,const XrVector3f& v)->XrVector3f{
                const float tx=2.0f*(q.y*v.z-q.z*v.y),ty=2.0f*(q.z*v.x-q.x*v.z),tz=2.0f*(q.x*v.y-q.y*v.x);
                return {v.x+q.w*tx+(q.y*tz-q.z*ty),v.y+q.w*ty+(q.z*tx-q.x*tz),v.z+q.w*tz+(q.x*ty-q.y*tx)};
            };
            auto qconj=[](XrQuaternionf q)->XrQuaternionf{return {-q.x,-q.y,-q.z,q.w};};
            XrVector3f headPos{}; XrQuaternionf headQ{0,0,0,1};
            if(frameOk){
                headPos={0.5f*(rf.eye_pose[0].position.x+rf.eye_pose[1].position.x),0.5f*(rf.eye_pose[0].position.y+rf.eye_pose[1].position.y),0.5f*(rf.eye_pose[0].position.z+rf.eye_pose[1].position.z)};
                headQ=rf.eye_pose[0].orientation;
            }
            auto projectHud=[&](const XrPose& p,float& x,float& y,bool& looked)->bool{
                looked=false;if(!frameOk)return false;
                const XrVector3f rel{p.pos_x-headPos.x,p.pos_y-headPos.y,p.pos_z-headPos.z};
                const auto hv=qrot(qconj(headQ),rel);if(hv.z>-0.06f)return false;
                const float tx=hv.x/(-hv.z),ty=hv.y/(-hv.z);
                const float l=std::min(std::tan(rf.eye_fov[0].angleLeft),std::tan(rf.eye_fov[1].angleLeft));
                const float r=std::max(std::tan(rf.eye_fov[0].angleRight),std::tan(rf.eye_fov[1].angleRight));
                const float d=std::min(std::tan(rf.eye_fov[0].angleDown),std::tan(rf.eye_fov[1].angleDown));
                const float u=std::max(std::tan(rf.eye_fov[0].angleUp),std::tan(rf.eye_fov[1].angleUp));
                if(!(r-l>0.1f&&u-d>0.1f))return false;
                const float uu=(tx-l)/(r-l),vv=(u-ty)/(u-d);x=uu*1920.0f;y=vv*1080.0f;
                looked=uu>0.12f&&uu<0.88f&&vv>0.10f&&vv<0.90f&&(-hv.z)<1.20f;
                return true;
            };
            const bool wristMode=false; // V66.170: palm gaze reveals fixed HUD; no moving wrist HUD writer
            static float f4LastSx=-9999,f4LastSy=-9999,f4LastCx=-9999,f4LastCy=-9999; static bool f4LastSl=false,f4LastCl=false;
            if(wristMode && ladder_now_ms>=f4_wrist_next_ms){
                f4_wrist_next_ms=ladder_now_ms+100ull;
                float sx=960,sy=700,cx=960,cy=180;bool sl=false,cl=false;
                const auto& lc=xr_manager_->LeftController();const auto& rc=xr_manager_->RightController();
                if(lc.active)(void)projectHud(lc.pose,sx,sy,sl);if(rc.active)(void)projectHud(rc.pose,cx,cy,cl);
                const auto moved=[](float a,float b){return std::fabs(a-b)>=4.0f;};
                if(!f4WristApplied||moved(sx,f4LastSx)||moved(sy,f4LastSy)||moved(cx,f4LastCx)||moved(cy,f4LastCy)||sl!=f4LastSl||cl!=f4LastCl){
                    (void)flashui_probe::ApplyWristHudLayout(true,sx,sy,cx,cy,0.72f,sl,cl);
                    f4WristApplied=true; // retain restoration duty even after a partial legacy write
                    f4LastSx=sx;f4LastSy=sy;f4LastCx=cx;f4LastCy=cy;f4LastSl=sl;f4LastCl=cl;
                }
            } else if(!wristMode && !vr_hud_native_canvas_fit_.load(std::memory_order_acquire) && !vr_auto_correct_hud_.load(std::memory_order_acquire)) {
                if(f4WristApplied && flashui_probe::ApplyWristHudLayout(false,0,0,0,0,1.0f,false,false))f4WristApplied=false;
            } // native-canvas handover owns its bounded restoration above

            const auto& rc=xr_manager_->RightController();
            const int rightItemState=ensure_vrik_lab()?frik_pose_probe::ExactHandItemState(0):-1;
            const bool laserRung=(f4_immersive_ui_rung==2||f4_immersive_ui_rung>=4);
            const bool handLaser=laserRung && !activity163.nativeControls && !ui89_policy.BlocksWorldInteraction() && !mis_menu_after_frame && !(ui_menu_input::WorldGripBlockedMask()&2u) && !safe_ui_fit && frameOk && rc.aim_active && rc.grip>0.55f && rightItemState==0;
            static int lastF4ItemState=-99;
            if(laserRung && rightItemState!=lastF4ItemState){
                lastF4ItemState=rightItemState;
                char sm[192]{};std::snprintf(sm,sizeof(sm),"V66.69-F4-HAND-STATE right=%s laserPolicy=EMPTY-ONLY",rightItemState==0?"EMPTY":(rightItemState==1?"OCCUPIED":"UNKNOWN"));diagnostic_trace::Write(sm);
            }
            if(handLaser){
                CameraRotationSample cam{};float wx=0,wy=0,wz=0;
                if(CameraHook::GetLatestFrustumRotationSample(cam)&&CameraHook::GetLatestVisualPositionWorld(wx,wy,wz,100)){
                    const XrVector3f relRef{rc.aim_pose.pos_x-headPos.x,rc.aim_pose.pos_y-headPos.y,rc.aim_pose.pos_z-headPos.z};
                    const XrVector3f relHead=qrot(qconj(headQ),relRef);
                    const XrQuaternionf aimQ{rc.aim_pose.qx,rc.aim_pose.qy,rc.aim_pose.qz,rc.aim_pose.qw};
                    const XrVector3f aimRef=qrot(aimQ,{0,0,-1});const XrVector3f aimHead=qrot(qconj(headQ),aimRef);
                    const float rx=cam.m[0],ry=cam.m[3],rz=cam.m[6], fx=cam.m[1],fy=cam.m[4],fz=cam.m[7], ux=cam.m[2],uy=cam.m[5],uz=cam.m[8];
                    float ox=wx+rx*relHead.x+ux*relHead.y+fx*(-relHead.z);float oy=wy+ry*relHead.x+uy*relHead.y+fy*(-relHead.z);float oz=wz+rz*relHead.x+uz*relHead.y+fz*(-relHead.z);
                    float dx=rx*aimHead.x+ux*aimHead.y+fx*(-aimHead.z),dy=ry*aimHead.x+uy*aimHead.y+fy*(-aimHead.z),dz=rz*aimHead.x+uz*aimHead.y+fz*(-aimHead.z);
                    const float dl=std::sqrt(dx*dx+dy*dy+dz*dz);
                    if(dl>1e-4f){
                        dx/=dl;dy/=dl;dz/=dl;ox+=dx*0.055f;oy+=dy*0.055f;oz+=dz*0.055f;
                        interaction_alignment::SetHandRayWorld(true,ox,oy,oz,dx,dy,dz);
                        const float al=std::sqrt(aimRef.x*aimRef.x+aimRef.y*aimRef.y+aimRef.z*aimRef.z);
                        if(al>1e-4f){const float adx=aimRef.x/al,ady=aimRef.y/al,adz=aimRef.z/al;xr_manager_->SetHandInteractionLaserLocal(true,rc.aim_pose.pos_x+adx*0.055f,rc.aim_pose.pos_y+ady*0.055f,rc.aim_pose.pos_z+adz*0.055f,adx,ady,adz,3.0f);}
                        // Combat cursor remains game-owned; center point has its own preference.
                    }
                }
            } else {
                if(frik_pose_probe::MiscInteractionDiagMode()<2) interaction_alignment::ClearLegacyHandRayWorldH5();
                xr_manager_->SetHandInteractionLaserLocal(false,0,0,0,0,0,-1,0.0f);
                // Never interpret the center-point preference as combat-cursor visibility.
            }
        } else {
            if(frik_pose_probe::MiscInteractionDiagMode()<2) interaction_alignment::ClearLegacyHandRayWorldH5();
            if(xr_manager_) xr_manager_->SetHandInteractionLaserLocal(false,0,0,0,0,0,-1,0.0f);
        }

// M2_SLOT_END 5018
#else
#error Unknown M2 host contribution slot
#endif
