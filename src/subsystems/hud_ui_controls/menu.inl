// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_CLICK_KIXGESTURES)
if(PtIn(kIxGestures,x,y)){ s.gesture_talk=!s.gesture_talk; done(s.gesture_talk?"R59.16C10R8-EXPERIMENTS gestureTalk=ON":"R59.16C10R8-EXPERIMENTS gestureTalk=OFF"); return; }
#elif defined(KCD2VR_MENU_DRAW_KIXGESTURES)
        DrawButton(dc,font_small_,kIxGestures,s.gesture_talk?L"[X] INDEX-FINGER POINTING QUICK TALK":L"[ ] INDEX-FINGER POINTING QUICK TALK",s.gesture_talk?kAccent:kPanel2);
#elif defined(KCD2VR_MENU_DRAW_VRHUD)

        if(!developer_mode::Enabled()){ // R160 UI: one simple, readable page (developer mode keeps the pages below)
        pageTitle(L"VR HUD",L"Icon size, HUD mode and what the HUD shows"); vrNav();
        HFONT bigR160=font_normal_?font_normal_:font_small_;
        std::swprintf(v,128,L"%.0f%%",double(kcd2vr::ui_hud_r160::ClampUnified(s.vr_ui_unified_scale_r160)*100.f));
        DrawSlider(dc,bigR160,kSimpleIconR160,L"Icon size (whole HUD)",(kcd2vr::ui_hud_r160::ClampUnified(s.vr_ui_unified_scale_r160)-.50f)/1.00f,v);
        {const float craftR160=prompt_policy_d80::minigameScale.load(std::memory_order_acquire); // VrMinigamePromptScale (R145)
         std::swprintf(v,128,L"%.0f%%",double(craftR160*100.f));DrawSlider(dc,bigR160,kSimpleCraftR160,L"Crafting / minigame icon size",(craftR160-.25f)/.75f,v);}
        Text(dc,bigR160,L"HUD mode",620,207,418,20,kAccent,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
        {const int modeR160=HudModeR160(s);
         DrawButton(dc,font_small_,kModeImmersiveR160,modeR160==0?L"[X] IMMERSIVE":L"IMMERSIVE",modeR160==0?kAccent:kPanel2);
         DrawButton(dc,font_small_,kModeMinimalR160,modeR160==1?L"[X] MINIMAL IMMERSIVE":L"MINIMAL IMMERSIVE",modeR160==1?kAccent:kPanel2);
         DrawButton(dc,font_small_,kModeNormalR160,modeR160==2?L"[X] NORMAL":L"NORMAL",modeR160==2?kAccent:kPanel2);}
        for(int i=0;i<kSimpleTicksR160;++i){bool inverted=false;const wchar_t* label=L"";const bool* field=SimpleTickFieldR160(s,i,inverted,label);
            if(!field)continue;const bool on=inverted?!*field:*field;wchar_t tick[48]{};std::swprintf(tick,48,L"%ls %ls",on?L"[X]":L"[ ]",label);
            DrawButton(dc,bigR160,SimpleTickR160(i),tick,on?kAccent:kPanel2);}
        // same value and log scale as INTERACTION / PROMPTS: 0.75..10 m, slider end = INF (VrHudInfinityR125)
        if(s.live_d39.hudInfinityR125)std::swprintf(v,128,L"INF");else std::swprintf(v,128,s.hud_distance_r110<2.0f?L"%.2f m":L"%.1f m",double(s.hud_distance_r110));
        DrawSlider(dc,bigR160,kSimpleHudDistR160,L"HUD distance (size stays)",s.live_d39.hudInfinityR125?1.0f:std::log(std::clamp(s.hud_distance_r110,kcd2vr::ui_surface_geometry::kHudConvergenceMinR127,kcd2vr::ui_surface_geometry::kHudConvergenceMaxR127)/kcd2vr::ui_surface_geometry::kHudConvergenceMinR127)/std::log(kcd2vr::ui_surface_geometry::kHudConvergenceMaxR127/kcd2vr::ui_surface_geometry::kHudConvergenceMinR127)*0.90f,v);
        } else {
        const bool correctedUi=false;
        pageTitle(L"VR CONFIG",L"Manual HUD placement and visibility"); vrNav();
        HFONT denseFont=font_tiny_?font_tiny_:font_small_;
        Text(dc,denseFont,L"HUD layout",620,112,202,24,kAccent,DT_LEFT);
        DrawCheck(dc,denseFont,kV4UiAuto,s.auto_correct_hud,L"Keep AutoHUD after F2 off");
        if(s.auto_correct_hud||s.hud_corrected_active){
            std::swprintf(v,128,L"%.0f%%",double(s.auto_hud_icon_scale*100.f));
            DrawSlider(dc,denseFont,kV4UiCompact,L"HUD icon / text size",(s.auto_hud_icon_scale-.50f)/5.50f,v);
            std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_safe_width*100.f));
            DrawSlider(dc,denseFont,kV4UiSafeWidth,L"Horizontal HUD size",(s.vr_ui_safe_width-.60f)/.80f,v);
            std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_safe_height*100.f));
            DrawSlider(dc,denseFont,kV4UiSafeHeight,L"Vertical HUD size",(s.vr_ui_safe_height-.60f)/.80f,v);
            std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_top_boundary*100.f));
            DrawSlider(dc,denseFont,kAutoUpper49,L"Upper HUD boundary",(s.vr_ui_top_boundary-.55f)/2.45f,v);
            std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_bottom_boundary*100.f));
            DrawSlider(dc,denseFont,kAutoLower49,L"Lower HUD boundary",(s.vr_ui_bottom_boundary-.55f)/2.45f,v);
            DrawButton(dc,denseFont,kV4UiElementTab,L"ELEMENT SIZES",kPanel2);
            DrawButton(dc,denseFont,kV4UiPromptTab,L"PROMPT SIZES",kPanel2);
            DrawButton(dc,denseFont,kV4UiReset,L"RESET CENTERED",kPanel2);
            DrawButton(dc,denseFont,kV4UiApply,L"REAPPLY + SAVE",kAccent);
        }else if(s.hud_native_canvas_fit){
            std::swprintf(v,128,L"%.0f%%",double(s.hud_native_canvas_size*100.f));
            DrawSlider(dc,denseFont,kV4UiCompact,L"HUD icon / text size",(s.hud_native_canvas_size-.70f)/.80f,v);
            std::swprintf(v,128,L"%.0f%%",double(s.hud_native_canvas_spread_x*100.f));
            DrawSlider(dc,denseFont,kV4UiSafeWidth,L"HUD horizontal anchor spread",(s.hud_native_canvas_spread_x-.45f)/.65f,v);
            std::swprintf(v,128,L"%.0f%%",double(s.hud_native_canvas_spread_y*100.f));
            DrawSlider(dc,denseFont,kV4UiSafeHeight,L"HUD vertical anchor spread",(s.hud_native_canvas_spread_y-.45f)/.65f,v);
            Text(dc,denseFont,L"V66.98 keeps elements at native raster scale and moves only their anchor positions inward. Size no longer pushes HUD groups back toward the edges. Fades, vignette and fullscreen effects stay untouched.",620,300,418,100,kText,DT_LEFT|DT_WORDBREAK);
            Text(dc,denseFont,L"Inventory / Shop Theater size is on VR CONFIG > VIEW. Grip remains laser-only in Theater. Ctrl+Alt+F9 reports the native anchor transaction.",620,410,418,70,kMuted,DT_LEFT|DT_WORDBREAK);
            DrawButton(dc,denseFont,kV4UiReset,L"RESET FITS",kPanel2);
            DrawButton(dc,denseFont,kV4UiApply,L"REAPPLY + SAVE",kAccent);
        }else if(correctedUi){
            Text(dc,denseFont,L"AUTO HUD ACTIVE — global scale/position now affects the whole projected HUD. Element pages stay available for compass, quick slots and prompt-specific tuning.",620,132,418,48,kAccent,DT_CENTER|DT_WORDBREAK);
            std::swprintf(v,128,L"%.0f%%",static_cast<double>(s.vr_ui_compactness*100.0f)); DrawSlider(dc,denseFont,kV4UiCompact,L"Global HUD scale",(s.vr_ui_compactness-0.25f)/0.95f,v);
            std::swprintf(v,128,L"%.0f%%",static_cast<double>(s.vr_ui_safe_width*100.0f)); DrawSlider(dc,denseFont,kV4UiSafeWidth,L"Auto HUD horizontal fit",(s.vr_ui_safe_width-0.60f)/0.80f,v);
            std::swprintf(v,128,L"X %+.0f px",static_cast<double>(s.vr_ui_center_offset_x)); DrawSlider(dc,denseFont,kV451HudOffsetX,L"Auto HUD horizontal offset",(s.vr_ui_center_offset_x+250.0f)/500.0f,v);
            std::swprintf(v,128,L"Y %+.0f px",static_cast<double>(s.vr_ui_center_offset_y)); DrawSlider(dc,denseFont,kV451HudOffsetY,L"Auto HUD vertical offset",(s.vr_ui_center_offset_y+250.0f)/500.0f,v);
            DrawButton(dc,denseFont,kV4UiElementTab,L"ELEMENT CONTROLS",kPanel2);
            DrawButton(dc,denseFont,kV4UiPromptTab,L"INTERACTION / PROMPTS",kPanel2);
        } else {
            std::swprintf(v,128,L"%.0f%%",static_cast<double>(s.vr_ui_compactness*100.0f)); DrawSlider(dc,denseFont,kV4UiCompact,L"HUD element scale",(s.vr_ui_compactness-0.25f)/0.95f,v);
            std::swprintf(v,128,L"%.0f%%",static_cast<double>(s.vr_ui_safe_width*100.0f)); DrawSlider(dc,denseFont,kV4UiSafeWidth,L"Safe zone horizontal size",(s.vr_ui_safe_width-0.60f)/0.80f,v);
            std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_top_boundary*100.f)); DrawSlider(dc,denseFont,kV4UiTopBoundary,L"Upper HUD boundary",(s.vr_ui_top_boundary-.55f)/2.45f,v);
            std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_bottom_boundary*100.f)); DrawSlider(dc,denseFont,kV4UiBottomBoundary,L"Lower HUD boundary",(s.vr_ui_bottom_boundary-.55f)/2.45f,v);
            std::swprintf(v,128,L"X %+.0f px",static_cast<double>(s.vr_ui_center_offset_x)); DrawSlider(dc,denseFont,kV4ManualOffsetX30c1,L"X offset",(s.vr_ui_center_offset_x+250.0f)/500.0f,v);
            std::swprintf(v,128,L"Y %+.0f px",static_cast<double>(s.vr_ui_center_offset_y)); DrawSlider(dc,denseFont,kV4ManualOffsetY30c1,L"Y offset",(s.vr_ui_center_offset_y+250.0f)/500.0f,v);
            DrawButton(dc,denseFont,kV4UiElementTab,L"ELEMENT CONTROLS",kPanel2);
            DrawButton(dc,denseFont,kV4UiPromptTab,L"INTERACTION / PROMPTS",kPanel2);
        }
        { // manual visibility controls are available for every layout
            DrawCheck(dc,denseFont,kV4Crosshair,s.hud_crosshair_visible,L"Center point");
            DrawCheck(dc,denseFont,kV4LowerIcons,s.hud_lower_icons_visible,L"Vitals / lower status");
            DrawCheck(dc,denseFont,kV4Compass,s.hud_compass_visible,L"Compass");
            DrawCheck(dc,denseFont,kHudStatus170,s.hud_status_icons_visible,L"Status icons");
        }
        if(!s.hud_native_canvas_fit&&!(s.auto_correct_hud||s.hud_corrected_active)){DrawButton(dc,font_small_,kV4UiReset,L"RESET LAYOUT",RGB(176,148,98)); DrawButton(dc,font_small_,kV4UiApply,L"APPLY + SAVE",kAccent);}
        DrawCheck(dc,denseFont,kContextualHud162r1,s.contextual_hud,L"Immersive HUD (palm look)");
        DrawCheck(dc,denseFont,kMinimalHudR141,s.live_d39.minimalHudR141,L"Minimal HUD"); // R141 VrMinimalHudR141
        DrawCheck(dc,denseFont,kHudLag22,s.hud_lag_on,L"HUD follows your head a little slower"); // C22b
        DrawCheck(dc,denseFont,kHudQuickSlotsR115,s.quick_slots_on_selection_r115,L"Weapon / potion slots only while selecting (3 s)"); // R115
        DrawCheck(dc,denseFont,kHudBlockMsgR124,s.show_block_indicator_h24,L"BLOCK message"); // R124 (R141: half row)
        DrawCheck(dc,denseFont,kHudTutorialsR141,!s.live_d39.hideTutorialsR141,L"Tutorial messages"); // R141: ticked = shown (VrHideTutorialsR141=false)
        } // R160 UI: developer mode page

#elif defined(KCD2VR_MENU_CLICK_VRHUD)

        if(!developer_mode::Enabled()){ // R160 UI simple page: every control saves at once (sliders on release)
            if(begin_drag(kSimpleIconR160,DragControl::UnifiedScaleR160))return;
            if(begin_drag(kSimpleCraftR160,DragControl::CraftUiSizeR145))return;
            if(begin_drag(kSimpleHudDistR160,DragControl::HudDistanceR110))return;
            const UiRect modesR160[3]{kModeImmersiveR160,kModeMinimalR160,kModeNormalR160};
            for(int m=0;m<3;++m)if(PtIn(modesR160[m],x,y)){SetHudModeR160(s,m);ApplyState(s);save();MarkDirty();return;}
            for(int i=0;i<kSimpleTicksR160;++i){if(!PtIn(SimpleTickR160(i),x,y))continue;
                bool inverted=false;const wchar_t* label=L"";bool* field=SimpleTickFieldR160(s,i,inverted,label);if(!field)return;
                *field=!*field;ApplyState(s); // never refused: ON = the feature runs
                if((i==2||i==4||i==7)&&callbacks_.apply_hud_visibility)callbacks_.apply_hud_visibility(callbacks_.context,s.hud_crosshair_visible,s.hud_lower_icons_visible,s.hud_compass_visible);
                save();MarkDirty();return;}
            return;
        }
        if(PtIn(kV4UiAuto,x,y)){s.auto_correct_hud=!s.auto_correct_hud;ApplyState(s);save();MarkDirty();return;}
        // Session opt-in; native mode ignores palm gates.
        if(PtIn(kContextualHud162r1,x,y)){s.contextual_hud=!s.contextual_hud;ApplyState(s);save();MarkDirty();return;}
        if(PtIn(kMinimalHudR141,x,y)){s.live_d39.minimalHudR141=!s.live_d39.minimalHudR141;ApplyState(s);save();MarkDirty();return;} // R141 VrMinimalHudR141
        if(PtIn(kHudLag22,x,y)){s.hud_lag_on=!s.hud_lag_on;ApplyState(s);save();MarkDirty();return;} // C22b
        if(PtIn(kHudQuickSlotsR115,x,y)){s.quick_slots_on_selection_r115=!s.quick_slots_on_selection_r115;ApplyState(s);save();MarkDirty();return;} // R115
        if(PtIn(kHudBlockMsgR124,x,y)){s.show_block_indicator_h24=!s.show_block_indicator_h24;ApplyState(s);save();MarkDirty();return;} // R124 VrShowBlockIndicator
        if(PtIn(kHudTutorialsR141,x,y)){s.live_d39.hideTutorialsR141=!s.live_d39.hideTutorialsR141;ApplyState(s);save();MarkDirty();return;} // R141 VrHideTutorialsR141
        if(PtIn(kV4Crosshair,x,y)){ s.hud_crosshair_visible=!s.hud_crosshair_visible; ApplyState(s); if(callbacks_.apply_hud_visibility) callbacks_.apply_hud_visibility(callbacks_.context,s.hud_crosshair_visible,s.hud_lower_icons_visible,s.hud_compass_visible); save(); return; }
        if(PtIn(kV4LowerIcons,x,y)){ s.hud_lower_icons_visible=!s.hud_lower_icons_visible; ApplyState(s); if(callbacks_.apply_hud_visibility) callbacks_.apply_hud_visibility(callbacks_.context,s.hud_crosshair_visible,s.hud_lower_icons_visible,s.hud_compass_visible); save(); return; }
        if(PtIn(kV4Compass,x,y)){ s.hud_compass_visible=!s.hud_compass_visible; ApplyState(s); if(callbacks_.apply_hud_visibility) callbacks_.apply_hud_visibility(callbacks_.context,s.hud_crosshair_visible,s.hud_lower_icons_visible,s.hud_compass_visible); save(); return; }
        if(PtIn(kHudStatus170,x,y)){s.hud_status_icons_visible=!s.hud_status_icons_visible;ApplyState(s);save();MarkDirty();return;}
        if(s.auto_correct_hud||s.hud_corrected_active){
            if(PtIn(kV4UiElementTab,x,y)){GoToPage(MenuPage::VrHudElementControls,+1);return;}
            if(PtIn(kV4UiPromptTab,x,y)){ GoToPage(MenuPage::VrHudElements,+1); return; }
            if(begin_drag(kV4UiCompact,DragControl::UiCompactness))return;
            if(begin_drag(kV4UiSafeWidth,DragControl::UiSafeWidth))return;
            if(begin_drag(kV4UiSafeHeight,DragControl::UiSafeHeight))return;
            if(begin_drag(kAutoUpper49,DragControl::UiTopBoundary))return;
            if(begin_drag(kAutoLower49,DragControl::UiBottomBoundary))return;

            if(PtIn(kV4UiReset,x,y)){
                s.auto_hud_icon_scale=1.67f;s.vr_ui_compactness=1.00f; // R160 UI: RESET = the new shipped defaults (his 8 Nov INI)
                s.vr_ui_safe_width=0.60f;
                s.vr_ui_top_boundary=1.29f;
                s.vr_ui_bottom_boundary=2.08f;
                s.vr_ui_safe_height=0.92f; // C57 RESET = Miki default (R160 values)
                s.vr_ui_center_offset_x=0.0f;
                s.vr_ui_center_offset_y=0.0f;
                ApplyState(s);
                if(callbacks_.apply_vr_ui_layout)callbacks_.apply_vr_ui_layout(callbacks_.context,&s);
                save();return;
            }
            if(PtIn(kV4UiApply,x,y)){
                if(callbacks_.apply_vr_ui_layout)callbacks_.apply_vr_ui_layout(callbacks_.context,&s);
                save();return;
            }
            return;
        }
        if(s.hud_native_canvas_fit){
            if(begin_drag(kV4UiCompact,DragControl::UiCompactness))return;
            if(begin_drag(kV4UiSafeWidth,DragControl::UiSafeWidth))return;
            if(begin_drag(kV4UiSafeHeight,DragControl::UiSafeHeight))return;
            if(PtIn(kV4UiReset,x,y)){s.hud_native_canvas_size=1.00f;s.hud_native_canvas_spread_x=.78f;s.hud_native_canvas_spread_y=.78f;ApplyState(s);if(callbacks_.apply_vr_ui_layout)callbacks_.apply_vr_ui_layout(callbacks_.context,&s);save();return;}
            if(PtIn(kV4UiApply,x,y)){if(callbacks_.apply_vr_ui_layout)callbacks_.apply_vr_ui_layout(callbacks_.context,&s);save();return;}
            return;
        }
        const bool correctedUi=false;
        if(correctedUi){
            if(begin_drag(kV4UiCompact,DragControl::UiCompactness)) return;
            if(begin_drag(kV4UiSafeWidth,DragControl::UiSafeWidth)) return;
            if(begin_drag(kV451HudOffsetX,DragControl::UiHudOffsetX)) return;
            if(begin_drag(kV451HudOffsetY,DragControl::UiHudOffsetY)) return;
            if(PtIn(kV4UiElementTab,x,y)){ GoToPage(MenuPage::VrHudElementControls,+1); return; }
            if(PtIn(kV4UiPromptTab,x,y)){ GoToPage(MenuPage::VrHudElements,+1); return; }
            if(PtIn(kV4UiReset,x,y)){ s.vr_ui_compactness=0.59f; s.vr_ui_safe_width=0.65f; s.vr_ui_safe_height=1.01f; s.vr_ui_top_boundary=1.17f; s.vr_ui_bottom_boundary=1.63f; /*C57R1 RESET = current manual boundaries too*/ s.vr_ui_center_offset_x=0.0f; s.vr_ui_center_offset_y=-115.0f; ApplyState(s); if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s); save(); return; }
            if(PtIn(kV4UiApply,x,y)){ if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s); if(callbacks_.save_settings) (void)callbacks_.save_settings(callbacks_.context); return; }
            return;
        }
        if(begin_drag(kV4UiCompact,DragControl::UiCompactness)) return;
        if(begin_drag(kV4UiSafeWidth,DragControl::UiSafeWidth)) return;
        if(begin_drag(kV4UiTopBoundary,DragControl::UiTopBoundary)) return;
        if(begin_drag(kV4UiBottomBoundary,DragControl::UiBottomBoundary)) return;
        if(begin_drag(kV4ManualOffsetX30c1,DragControl::UiHudOffsetX)) return;
        if(begin_drag(kV4ManualOffsetY30c1,DragControl::UiHudOffsetY)) return;
        if(PtIn(kV4UiElementTab,x,y)){ GoToPage(MenuPage::VrHudElementControls,+1); return; }
        if(PtIn(kV4UiPromptTab,x,y)){ GoToPage(MenuPage::VrHudElements,+1); return; }
        if(PtIn(kV4UiReset,x,y)){
            s.vr_ui_layout=true; s.vr_ui_auto_layout=false; s.vr_ui_compactness=0.73f; s.vr_ui_safe_width=0.65f; s.vr_ui_safe_height=1.01f; s.vr_ui_top_boundary=1.17f; s.vr_ui_bottom_boundary=1.63f; /*C57 RESET = Miki defaults (was 1.0/1.0/1.0)*/ s.vr_ui_center_offset_x=0.0f; s.vr_ui_center_offset_y=0.0f; s.vr_ui_subtitle_scale=0.54f; s.vr_ui_subtitle_y=0.0f;
            // Retired Alchemy/Menu page now follows the proven Item Panels defaults.
            s.vr_ui_item_panel_scale=0.73f; s.vr_ui_item_panel_x=-380.0f; s.vr_ui_item_panel_y=-300.0f;
            s.vr_ui_interaction_menu_scale=s.vr_ui_item_panel_scale; s.vr_ui_interaction_menu_x=s.vr_ui_item_panel_x; s.vr_ui_interaction_menu_y=s.vr_ui_item_panel_y;
            s.vr_ui_hints_scale=0.64f; s.vr_ui_hints_inset_pct=33.0f; s.vr_ui_hints_y=-185.0f;
            s.vr_ui_control_prompt_scale=0.70f; s.vr_ui_control_prompt_x=0.0f; s.vr_ui_control_prompt_y=0.0f;
            // R113 (chatgpt R112): a layout reset keeps every visibility choice.
            ApplyState(s);
            if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s);
            save();
            return;
        }
        // V64.20 old per-element free-position page retired; centered HUD is the single layout authority.
        if(PtIn(kV4UiApply,x,y)){ if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s); if(callbacks_.apply_hud_visibility) callbacks_.apply_hud_visibility(callbacks_.context,s.hud_crosshair_visible,s.hud_lower_icons_visible,s.hud_compass_visible); if(callbacks_.save_settings) (void)callbacks_.save_settings(callbacks_.context); return; }
        return;

#elif defined(KCD2VR_MENU_DRAW_VRHUDELEMENTCONTROLS)

        pageTitle(L"ELEMENT CONTROLS",L"Large Touch-friendly sliders for the HUD elements that need separate placement"); vrNav();
        HFONT dense=font_tiny_?font_tiny_:font_small_;
        Text(dc,dense,L"COMPASS",620,132,418,20,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_compass_scale*100.0f)); DrawSlider(dc,dense,kV4ElemCompassScale,L"Compass size",(s.vr_ui_compass_scale-0.15f)/1.35f,v);
        std::swprintf(v,128,L"%+.0f px",double(s.vr_ui_compass_y)); DrawSlider(dc,dense,kV4ElemCompassY,L"Compass vertical",(s.vr_ui_compass_y+300.0f)/600.0f,v);
        std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_stats_scale*100.0f)); DrawSlider(dc,dense,kV4ElemStatsScale,L"Health size",(s.vr_ui_stats_scale-.15f)/1.35f,v);
        Text(dc,dense,L"HEALTH / QUICK SLOTS (Stats + QAMWeapon/QAMFood)",620,292,418,20,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_lower_scale*100.0f)); DrawSlider(dc,dense,kV4ElemLowerScale,L"Quick-slot size",(s.vr_ui_lower_scale-0.15f)/1.85f,v);
        std::swprintf(v,128,L"%+.0f px",double(s.vr_ui_lower_y)); DrawSlider(dc,dense,kV4ElemLowerY,L"Quick-slot vertical",(s.vr_ui_lower_y+250.0f)/500.0f,v);
        std::swprintf(v,128,L"%+.0f px",double(s.vr_ui_lower_spread)); DrawSlider(dc,dense,kV4ElemLowerSpread,L"Quick-slot spread",(s.vr_ui_lower_spread+200.0f)/400.0f,v);
        std::swprintf(v,128,L"%.0f%%",double(s.combat_crosshair_scale_r112*100.0f)); DrawSlider(dc,dense,kCombatCrosshairR112,L"Combat crosshair size (fight directions)",(s.combat_crosshair_scale_r112-0.25f)/1.25f,v); // R112
        DrawCheck(dc,dense,kCombatCrossR125,s.live_d39.combatCrossR125,L"Combat direction cross (OFF: only lock / block / MS icons)"); // R125
        DrawButton(dc,dense,kV4ElemBack,L"CORE UI",RGB(176,148,98)); DrawButton(dc,dense,kV4ElemSave,L"APPLY + SAVE",kAccent);

#elif defined(KCD2VR_MENU_CLICK_VRHUDELEMENTCONTROLS)

        if(begin_drag(kV4ElemCompassScale,DragControl::UiCompassScale)) return;
        if(begin_drag(kV4ElemStatsScale,DragControl::UiStatsScale)) return;
        if(begin_drag(kV4ElemCompassY,DragControl::UiCompassY)) return;
        if(begin_drag(kV4ElemLowerScale,DragControl::UiLowerScale)) return;
        if(begin_drag(kV4ElemLowerY,DragControl::UiLowerY)) return;
        if(begin_drag(kV4ElemLowerSpread,DragControl::UiLowerSpread)) return;
        if(begin_drag(kCombatCrosshairR112,DragControl::CombatCrosshairR112)) return; // R112
        if(PtIn(kCombatCrossR125,x,y)){s.live_d39.combatCrossR125=!s.live_d39.combatCrossR125;ApplyState(s);if(callbacks_.save_settings)(void)callbacks_.save_settings(callbacks_.context);MarkDirty();return;} // R125 VrCombatCrossR125
        if(PtIn(kV4ElemBack,x,y)){ GoToPage(MenuPage::VrHud,-1); return; }
        if(PtIn(kV4ElemSave,x,y)){ ApplyState(s); if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s); save(); return; }
        return;

#elif defined(KCD2VR_MENU_DRAW_VRHUDELEMENTS)

        pageTitle(L"INTERACTION / PROMPTS",L"Independent sizing for contextual action icons and tutorial/control prompts"); vrNav();
        HFONT dense=font_tiny_?font_tiny_:font_small_;
        Text(dc,dense,L"INTERACTION ICONS  (Hints / br.actionHints)",620,122,418,22,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_hints_scale*100.0f)); DrawSlider(dc,dense,kUiPromptHintScale,L"Interaction / menu icons",(s.vr_ui_hints_scale-0.25f)/1.25f,v);
        if(!(s.auto_correct_hud||s.hud_corrected_active)){std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_hints_inset_pct)); DrawSlider(dc,dense,kUiPromptHintInset,L"Interaction inward",s.vr_ui_hints_inset_pct/75.0f,v);}
        else{const float craftR145=prompt_policy_d80::minigameScale.load(std::memory_order_acquire); // R145 VrMinigamePromptScale
            std::swprintf(v,128,L"%.0f%%",double(craftR145*100.0f)); DrawSlider(dc,dense,kCraftUiSizeR145,L"Crafting UI size (alchemy / smithing prompts + icons)",(craftR145-0.25f)/0.75f,v);}
        if(!(s.auto_correct_hud||s.hud_corrected_active)){std::swprintf(v,128,L"%+.0f px",double(s.vr_ui_hints_y)); DrawSlider(dc,dense,kUiPromptHintY,L"Interaction vertical",(s.vr_ui_hints_y+300.0f)/600.0f,v);}
        std::swprintf(v,128,L"%.2fx",double(s.passive_full_frame_scale)); DrawSlider(dc,dense,kCinematicSizeR107,L"Cutscene / dialogue / loading size",(s.passive_full_frame_scale-1.0f)/1.0f,v); // R107
        Text(dc,dense,L"CONTROL / TUTORIAL PROMPTS  (TutorialMessage / tl.tutorial)",620,314,418,16,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE); // R160 UI: 318..340 -> 314..330, the two half-row labels start at 332
        std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_control_prompt_scale*100.0f)); DrawSlider(dc,dense,kUiControlPromptScale,L"Control prompt size",(s.vr_ui_control_prompt_scale-0.30f)/1.20f,v);
        std::swprintf(v,128,L"%+.0f px",double(kcd2vr::ui_hud_r160::ClampTutorialY(s.vr_ui_tutorial_y_r160))); DrawSlider(dc,dense,kTutorialYR160,L"Tutorial position (+ = down)",(kcd2vr::ui_hud_r160::ClampTutorialY(s.vr_ui_tutorial_y_r160)+540.0f)/1080.0f,v); // R160 UI dev: VrUiTutorialYR160
        if(!(s.auto_correct_hud||s.hud_corrected_active)){std::swprintf(v,128,L"X %+.0f px",double(s.vr_ui_control_prompt_x)); DrawSlider(dc,dense,kUiControlPromptX,L"Control prompt horizontal",(s.vr_ui_control_prompt_x+350.0f)/700.0f,v);}
        if(!(s.auto_correct_hud||s.hud_corrected_active)){std::swprintf(v,128,L"Y %+.0f px",double(s.vr_ui_control_prompt_y)); DrawSlider(dc,dense,kUiControlPromptY,L"Control prompt vertical",(s.vr_ui_control_prompt_y+300.0f)/600.0f,v);}
        else{ // R160: dialogue (DialogLeft/DialogRight) size + vertical, in the slots the corrected HUD leaves free. 100% / +0 px = the old placement
            std::swprintf(v,128,L"%.0f%%",double(kcd2vr::ui_native_canvas::DialogueFactorR160(s.vr_ui_dialogue_scale)*100.0f)); DrawSlider(dc,dense,kDlgSizeR160,L"Dialogue text size",(s.vr_ui_dialogue_scale-0.30f)/1.20f,v);
            std::swprintf(v,128,L"%+.0f px",double(kcd2vr::ui_native_canvas::DialogueOffsetR160(s.vr_ui_dialogue_y))); DrawSlider(dc,dense,kDlgYR160,L"Dialogue vertical position (+ = down)",(s.vr_ui_dialogue_y-kcd2vr::ui_native_canvas::kDialogueYMinR160)/(kcd2vr::ui_native_canvas::kDialogueYMaxR160-kcd2vr::ui_native_canvas::kDialogueYMinR160),v);}
        // R125: slider end = INF (VrHudInfinityR125): one HUD copy per eye, zero disparity, never double
        if(s.live_d39.hudInfinityR125)std::swprintf(v,128,L"INF");else std::swprintf(v,128,s.hud_distance_r110<2.0f?L"%.2f m":L"%.1f m",double(s.hud_distance_r110));
        DrawSlider(dc,dense,kHudDistanceR110,L"HUD convergence (where the eyes meet, size stays)",s.live_d39.hudInfinityR125?1.0f:std::log(std::clamp(s.hud_distance_r110,kcd2vr::ui_surface_geometry::kHudConvergenceMinR127,kcd2vr::ui_surface_geometry::kHudConvergenceMaxR127)/kcd2vr::ui_surface_geometry::kHudConvergenceMinR127)/std::log(kcd2vr::ui_surface_geometry::kHudConvergenceMaxR127/kcd2vr::ui_surface_geometry::kHudConvergenceMinR127)*0.90f,v); // R110 + R125 + R127 log scale
        std::swprintf(v,128,L"%.0f%%",double(s.pause_overlay_size*100.0f)); DrawSlider(dc,dense,kPauseOverlay124,L"Pause / death overlay size",(s.pause_overlay_size-.45f)/.70f,v);
        std::swprintf(v,128,L"%.0f%%",double(kcd2vr::ui_hud_r160::ClampUnified(s.vr_ui_unified_scale_r160)*100.0f)); DrawSlider(dc,dense,kUnifiedDevR160,L"Icon size (whole HUD)",(kcd2vr::ui_hud_r160::ClampUnified(s.vr_ui_unified_scale_r160)-.50f)/1.00f,v); // R160 UI: same value as the simple page
        DrawButton(dc,dense,kUiPanelBack,L"CORE UI",RGB(176,148,98)); DrawButton(dc,dense,kUiPanelApply,L"APPLY + SAVE",kAccent);


#elif defined(KCD2VR_MENU_CLICK_VRHUDELEMENTS)

        if(begin_drag(kUiPromptHintScale,DragControl::UiHintsScale)) return;
        if((s.auto_correct_hud||s.hud_corrected_active)&&begin_drag(kCraftUiSizeR145,DragControl::CraftUiSizeR145)) return; // R145, same slot as the hidden inset row
        const bool legacyRowsR160=!(s.auto_correct_hud||s.hud_corrected_active); // R160: hidden rows are not clickable any more (they sit under other rows)
        if(!legacyRowsR160&&begin_drag(kDlgSizeR160,DragControl::UiDialogueScale)) return; // R160
        if(!legacyRowsR160&&begin_drag(kDlgYR160,DragControl::UiDialogueY)) return; // R160
        if(legacyRowsR160&&begin_drag(kUiPromptHintInset,DragControl::UiHintsInset)) return;
        if(legacyRowsR160&&begin_drag(kUiPromptHintY,DragControl::UiHintsY)) return;
        if(begin_drag(kCinematicSizeR107,DragControl::PassiveFullFrameScale)) return; // R107
        if(begin_drag(kHudDistanceR110,DragControl::HudDistanceR110)) return; // R110
        if(begin_drag(kUiControlPromptScale,DragControl::UiControlPromptScale)) return;
        if(begin_drag(kPauseOverlay124,DragControl::PauseOverlaySize)) return;
        if(begin_drag(kTutorialYR160,DragControl::TutorialYR160)) return; // R160 UI dev
        if(begin_drag(kUnifiedDevR160,DragControl::UnifiedScaleR160)) return; // R160 UI
        if(legacyRowsR160&&begin_drag(kUiControlPromptX,DragControl::UiControlPromptX)) return;
        if(legacyRowsR160&&begin_drag(kUiControlPromptY,DragControl::UiControlPromptY)) return;
        if(PtIn(kUiPanelBack,x,y)){ GoToPage(MenuPage::VrHud,-1); return; }
        if(PtIn(kUiPanelApply,x,y)){
            ApplyState(s);
            if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s);
            if(callbacks_.save_settings) (void)callbacks_.save_settings(callbacks_.context);
            return;
        }
        return;

#elif defined(KCD2VR_MENU_DRAW_VRCONTROLS)

        pageTitle(L"VR CONTROLS",L"Golden controller path / keyboard fallbacks / prompt style"); vrNav();
        HFONT dense=font_tiny_?font_tiny_:font_small_;
        Text(dc,dense,L"LEFT CONTROLLER",620,116,196,22,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        Text(dc,dense,L"RIGHT CONTROLLER",842,116,196,22,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        // R118: the left column follows the chosen controller layout (STEAMVR / PS VR2: X hold = inventory).
        const bool xInventoryR118=controller_layout_r118::XHoldOpensInventory(s.live_d39.controllerLayoutR118);
        Text(dc,dense,s.quick_access_left_thumbrest_r143?
            L"LS: move / focus; L3: sprint\nLT: native block\nY / Triangle: horse / dog / info\nX/Square: tap talk / hold inventory\nLeft thumbrest + right stick: D-pad\nRight stick down: VR menu; up: held map\nRight A: wait; right B: pause\nMenu: inventory\nLeft grip: grab / support":
            xInventoryR118?
            L"LS: move / focus; L3: sprint\nLT: native block\nY / Triangle: horse / dog / info\nX/Square: tap talk, hold inventory\nL modifier + X/Square hold: visor\nTorch: L trigger at L shoulder\nR modifier + Menu: pause\nR modifier + X/Square hold: wait\nR modifier + LS L/R: D-pad\nR modifier + LS down: VR menu\nR modifier + LS up: held map\nLeft grip: grab / support":
            L"LS: move / focus; L3: sprint\nLT: native block\nY / Triangle: horse / dog / info\nX / Square: tap talk, hold torch\nL modifier + X/Square hold: visor\nMenu: inventory\nR modifier + Menu: pause\nR modifier + X/Square hold: wait\nR modifier + LS L/R: D-pad\nR modifier + LS down: VR menu\nR modifier + LS up: held map\nLeft grip: grab / support",620,145,196,162,kText,DT_LEFT|DT_WORDBREAK);
        Text(dc,dense,s.quick_access_left_thumbrest_r143?
            L"RS L/R: turn; up: jump; down: crouch\nWith left modifier: RS D-pad / map / VR menu\nR3: kick / special\nAttack: swing / thrust; bow RT pull\nA / Cross: use; modifier + A: Wait\nB / Circle: contextual Back; modifier + B: Pause\nPages: left modifier + RS L/R\nRight grip: free view / grab\nL3+R3 hold 0.6 s / F7: VR menu":
            L"RS L/R: turn; up: jump; down: crouch\nR3: kick / special\nAttack: swing / thrust; bow RT pull\nA / Cross: use / talk / hold\nB / Circle: contextual Back\nHold B/Circle: recovery Escape\nPages: R modifier + LS L/R\nor flick over the book (PAGES)\nRight grip: free view / grab\nL3+R3 hold 0.6 s / F7: VR menu",842,145,196,162,kText,DT_LEFT|DT_WORDBREAK);
        if(developer_mode::Enabled())Text(dc,dense,L"F2 bugs | F3 combat | F5 interactions | Shift+F7 VRIK | F11 reload",620,312,418,18,kMuted,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        const unsigned modifierHandsR112=s.quick_access_available_hands_r112&3u;
        const wchar_t* modifierHelpR112=s.quick_access_face_touch_r112?
            (modifierHandsR112==3?L"Modifier: touch Y/Triangle (left) or B/Circle (right), without pressing. Clicks keep their normal actions.":
             modifierHandsR112==1?L"Left Y/Triangle touch ready; right B/Circle touch unavailable. Touch without pressing.":
             modifierHandsR112==2?L"Right B/Circle touch ready; left Y/Triangle touch unavailable. Touch without pressing.":
             L"Face touch unavailable. The runtime must expose Y/Triangle and B/Circle touch. Clicks are unchanged."):
            s.quick_access_left_thumbrest_r143?
            L"Modifier: left thumbrest. Right stick = D-pad, map and VR menu; A = Wait, B = Pause.":
            L"Modifier: right thumbrest + left stick. Choose FACE TOUCH for controllers without a thumbrest.";
        Text(dc,dense,modifierHelpR112,620,335,418,28,kMuted,DT_LEFT|DT_WORDBREAK);
        {wchar_t layoutR118[48]{};std::swprintf(layoutR118,std::size(layoutR118),L"LAYOUT: %ls",controller_layout_r118::Name(s.live_d39.controllerLayoutR118));
         DrawButton(dc,dense,kControllerLayoutR118,layoutR118,s.live_d39.controllerLayoutR118?kAccent:kPanel2);} // R118
        DrawButton(dc,dense,kQuickAccessTouchR112,s.quick_access_face_touch_r112?L"MODIFIER: FACE TOUCH":s.quick_access_left_thumbrest_r143?L"MODIFIER: LEFT THUMBREST":L"MODIFIER: RIGHT THUMBREST",(s.quick_access_face_touch_r112||s.quick_access_left_thumbrest_r143)?kAccent:kPanel2);
        DrawButton(dc,dense,kV4CtrlHeadLook,s.native_activity_controls163?L"ACTIVITY GAMEPAD: ON":L"ACTIVITY GAMEPAD: OFF",s.native_activity_controls163?kAccent:kPanel2);
        DrawButton(dc,dense,kV4CtrlSnap,L"SNAP TURN",s.vr_turn_mode==0?kAccent:kPanel2);
        DrawButton(dc,dense,kV4CtrlSmooth,L"SMOOTH TURN",s.vr_turn_mode==1?kAccent:kPanel2);
        Text(dc,dense,L"PROMPT ICONS ONLY - SAME CONTROLS",620,460,418,13,kMuted,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        DrawButton(dc,dense,kV476GlyphXbox,L"XBOX",s.vr_pad_glyph_style==2?kAccent:kPanel2);
        DrawButton(dc,dense,kV476GlyphPs,L"PLAYSTATION",s.vr_pad_glyph_style==4?kAccent:kPanel2);
        DrawButton(dc,dense,kV6510bPromptLab,s.reading_pages45==0?L"PAGES: FLICK / STICK":s.reading_pages45==1?L"PAGES: STICK ONLY":L"PAGES: NATIVE",s.reading_pages45!=2?kAccent:kPanel2); // C45
        DrawButton(dc,dense,kV4CtrlLaser,s.vr_laser_visible?L"LASER: ON":L"LASER: OFF",s.vr_laser_visible?kAccent:kPanel2);
        Text(dc,dense,L"PRESENTATION: AUTO  (F6 = Theater/Immersive switch, L3+R3 click = Immersive)",620,565,418,14,kMuted,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        DrawButton(dc,dense,kV4CtrlDisplayAuto,L"AUTO",s.vr_display_mode==0?kAccent:kPanel2);
        DrawButton(dc,dense,kV4CtrlDisplayTheater,s.vrik_body_follow?L"BODY + WALK FOLLOW: ON (F1)":L"BODY + WALK FOLLOW: OFF (F1)",s.vrik_body_follow?kAccent:kPanel2);
        DrawButton(dc,dense,kRoomscale59,s.roomscale_mode==0?L"ROOMSCALE: OFF":s.roomscale_mode==1?L"ROOMSCALE: PHYSICAL WALKING":s.roomscale_mode==2?L"ROOMSCALE: WALK + HEADSET DODGE":L"HEADSET DODGE ONLY (no walking)",s.roomscale_mode?kAccent:kPanel2); // R59.16C5
        DrawButton(dc,dense,kSmoothMove51,s.smooth_move51?L"[X] BODY SMOOTH":L"[ ] BODY SMOOTH",s.smooth_move51?kAccent:kPanel2);
        DrawButton(dc,dense,kHmdWalkingDirection134,s.hmd_walking_direction134?L"WALK: HMD":L"WALK: STICK",s.hmd_walking_direction134?kAccent:kPanel2);
        DrawButton(dc,dense,kComfortVignette51,s.comfort_vignette51==0?L"VIGNETTE: OFF":s.comfort_vignette51==1?L"VIGNETTE: LOW":s.comfort_vignette51==2?L"VIGNETTE: MEDIUM":L"VIGNETTE: STRONG",s.comfort_vignette51?kAccent:kPanel2);
        DrawButton(dc,dense,kGestureTalk59c5,s.gesture_talk?L"[X] INDEX-FINGER POINTING QUICK TALK":L"[ ] INDEX-FINGER POINTING QUICK TALK",s.gesture_talk?kAccent:kPanel2);

#elif defined(KCD2VR_MENU_CLICK_VRCONTROLS)

        if(PtIn(kQuickAccessTouchR112,x,y)){
            if(s.quick_access_face_touch_r112){s.quick_access_face_touch_r112=false;s.quick_access_left_thumbrest_r143=false;}
            else if(s.quick_access_left_thumbrest_r143){s.quick_access_left_thumbrest_r143=false;s.quick_access_face_touch_r112=true;}
            else s.quick_access_left_thumbrest_r143=true;
            ApplyState(s);save();MarkDirty();return;}
        // R118: OPENXR -> STEAMVR -> PS VR2; the choice applies its modifier preset (PS VR2 = face touch), MODIFIER still toggles after.
        if(PtIn(kControllerLayoutR118,x,y)){(void)live_settings_d39::advance(s,live_settings_d39::Id::ControllerLayoutR118);ApplyState(s);save();
            char m[128]{};std::snprintf(m,sizeof(m),"R143-CONTROLS layout=%s modifier=%s",controller_layout_r118::NameA(s.live_d39.controllerLayoutR118),s.quick_access_face_touch_r112?"FACE_TOUCH":s.quick_access_left_thumbrest_r143?"LEFT_THUMBREST":"RIGHT_THUMBREST");diagnostic_trace::Write(m);MarkDirty();return;}
        if(PtIn(kV4CtrlHeadLook,x,y)){ s.native_activity_controls163=!s.native_activity_controls163; ApplyState(s); diagnostic_trace::Write(s.native_activity_controls163?"V66.163-CONTROLS activity gamepad ON (session only)":"V66.163-CONTROLS activity gamepad OFF"); MarkDirty(); return; }
        if(PtIn(kV4CtrlSnap,x,y)){ s.vr_turn_mode=0; ApplyState(s); save(); diagnostic_trace::Write("V64.17-CONTROLS turn=SNAP"); return; }
        if(PtIn(kV4CtrlSmooth,x,y)){ s.vr_turn_mode=1; ApplyState(s); save(); diagnostic_trace::Write("V64.17-CONTROLS turn=SMOOTH"); return; }
        if(PtIn(kV6510bPromptLab,x,y)){ s.reading_pages45=(s.reading_pages45+1)%3; ApplyState(s); save(); diagnostic_trace::Write(s.reading_pages45==0?"R112-CONTROLS pages=MODIFIER+FLICK":s.reading_pages45==1?"R112-CONTROLS pages=MODIFIER_ONLY":"C45-CONTROLS pages=NATIVE"); MarkDirty(); return; } // C45: slot of the retired routing-help button
        if(PtIn(kV4CtrlLaser,x,y)){ s.vr_laser_visible=!s.vr_laser_visible; ApplyState(s); save(); diagnostic_trace::Write(s.vr_laser_visible?"V64.17-CONTROLS laser=ON":"V64.17-CONTROLS laser=OFF hit-test-active"); return; }
        if(PtIn(kV4CtrlDisplayAuto,x,y)){ s.vr_display_mode=0; ApplyState(s); save(); diagnostic_trace::Write("V64.89-DISPLAY AUTO"); return; }
        if(PtIn(kV4CtrlDisplayTheater,x,y)){ s.vrik_body_follow=!s.vrik_body_follow; ApplyState(s); save(); diagnostic_trace::Write(s.vrik_body_follow?"V66.128-CONTROLS 6DOF body=ON":"V66.128-CONTROLS 6DOF body=OFF"); MarkDirty(); return; } // V66.128: retired Theater slot reused
        if(PtIn(kSmoothMove51,x,y)){ s.smooth_move51=!s.smooth_move51; ApplyState(s); save(); diagnostic_trace::Write(s.smooth_move51?"C51-CONTROLS VrSmoothMovement=true":"C51-CONTROLS VrSmoothMovement=false"); MarkDirty(); return; }
        if(PtIn(kHmdWalkingDirection134,x,y)){ s.hmd_walking_direction134=!s.hmd_walking_direction134; ApplyState(s); save(); diagnostic_trace::Write(s.hmd_walking_direction134?"R134-CONTROLS VrHmdWalkingDirectionR134=true":"R134-CONTROLS VrHmdWalkingDirectionR134=false"); MarkDirty(); return; }
        if(PtIn(kComfortVignette51,x,y)){ s.comfort_vignette51=(std::clamp(s.comfort_vignette51,0,3)+1)%4; ApplyState(s); save(); char m[80]{}; std::snprintf(m,sizeof(m),"C51-CONTROLS VrComfortVignette=%d",s.comfort_vignette51); diagnostic_trace::Write(m); MarkDirty(); return; }
        if(PtIn(kGestureTalk59c5,x,y)){ s.gesture_talk=!s.gesture_talk; ApplyState(s); save(); diagnostic_trace::Write(s.gesture_talk?"R59.16C5-CONTROLS gestureTalk=ON":"R59.16C5-CONTROLS gestureTalk=OFF"); MarkDirty(); return; }
        if(PtIn(kRoomscale59,x,y)){ s.roomscale_mode=(s.roomscale_mode+1)%4; ApplyState(s); save(); diagnostic_trace::Write(s.roomscale_mode==0?"V66.178R59-CONTROLS roomscale=OFF":s.roomscale_mode==1?"V66.178R59-CONTROLS roomscale=FOLLOW":s.roomscale_mode==2?"V66.178R59-CONTROLS roomscale=FOLLOW+DODGE":"R59.16C5-CONTROLS roomscale=DODGE_ONLY"); MarkDirty(); return; } // R59.16C5: 4 states
        if(PtIn(kV476GlyphXbox,x,y)){ s.vr_pad_glyph_style=2; ApplyState(s); save(); diagnostic_trace::Write("V64.76-CONTROLS prompts=XBOX"); return; }
        if(PtIn(kV476GlyphPs,x,y)){ s.vr_pad_glyph_style=4; ApplyState(s); save(); diagnostic_trace::Write("V64.76-CONTROLS prompts=PLAYSTATION"); return; }
        return;

#endif
