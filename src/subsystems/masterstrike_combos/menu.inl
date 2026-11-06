// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_DRAW_COMBATFLOW59C17)
 // R59.16C10R17
        pageTitle(L"COMBAT FLOW",L"Changes apply immediately and are saved"); vrNav(); DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        DrawButton(dc,font_small_,kCfGuardStrict59c17,s.exp_guard_strict?L"[X] FRONTAL BLADE GUARD: STRICTER MOTION ENTRY ON":L"[ ] FRONTAL BLADE GUARD: STRICTER MOTION ENTRY OFF",s.exp_guard_strict?kAccent:kPanel2);
        DrawButton(dc,font_small_,kCfNoCooldown59c17,s.exp_no_block_cooldown?L"[X] PERFECT BLOCK: MINIMAL NATIVE COOLDOWN (1 ms, reliable release)":L"[ ] PERFECT BLOCK: SAVED NATIVE BLOCK COOLDOWN",s.exp_no_block_cooldown?kAccent:kPanel2);
        DrawButton(dc,font_small_,kCfMsStance59c17,s.exp_ms_keep_stance?L"[X] MASTER STRIKE: KEEP THE STANCE THAT OPENED THE WINDOW":L"[ ] MASTER STRIKE: THE STROKE REWRITES THE STANCE (R16)",s.exp_ms_keep_stance?kAccent:kPanel2);
        std::swprintf(v,128,s.exp_native_hit_time<.10f?L"native":L"%.2f s",double(s.exp_native_hit_time)); DrawSlider(dc,font_small_,kCfHitTime59c17,L"NPC reaction: the hidden attack lands this long after your swing commits",s.exp_native_hit_time/.60f,v);
        DrawButton(dc,font_small_,kCfComboExtend59c17,s.exp_combo_window_extend?L"[X] EXTEND":L"[ ] EXTEND",s.exp_combo_window_extend?kAccent:kPanel2);
        std::swprintf(v,128,s.exp_combo_window_ms<=0?L"native":L"%.2f s",double(s.exp_combo_window_ms)*.001); DrawSlider(dc,font_small_,kCfComboWindow59c17,L"Combo time (s)",s.exp_combo_window_ms/3000.f,v);
        DrawButton(dc,font_small_,kCfComboZoneCZ,s.exp_combo_zone_assist?L"[X] COMBO HELP: ZONE":L"[ ] COMBO HELP: ZONE",s.exp_combo_zone_assist?kAccent:kPanel2); // I12
        DrawButton(dc,font_small_,kCfComboHoldN2,s.exp_combo_recovery_hold?L"[X] COMBO HELP: HOLD":L"[ ] COMBO HELP: HOLD",s.exp_combo_recovery_hold?kAccent:kPanel2); // I12/N2
        if(s.exp_ms_handoff23<0.f)std::swprintf(v,128,L"= guard exit");else std::swprintf(v,128,L"%.2f m/s",double(s.exp_ms_handoff23)); // C24b
        DrawSlider(dc,font_small_,kCfMsHandoff23,L"Master strike: strike out of guard in the native window at this speed",s.exp_ms_handoff23<0.f?0.f:(s.exp_ms_handoff23-1.5f)/3.f,v);
        DrawButton(dc,font_small_,kCfInstantFree38,s.exp_instant_free_swing38?L"[X] SYNC: FREE SWING STARTS AT ONCE (no native 0.3 s wait without a lock)":L"[ ] SYNC: NATIVE 0.3 s FREE-ATTACK WAIT (swing without a lock)",s.exp_instant_free_swing38?kAccent:kPanel2); // C38
        DrawButton(dc,font_small_,kCfHideLean38,s.exp_hide_swing_lean38?L"[X] SYNC: NO NATIVE UPPER-BODY LEAN IN THE FREE SWING":L"[ ] SYNC: NATIVE FREE-SWING LEAN VISIBLE",s.exp_hide_swing_lean38?kAccent:kPanel2); // C38
        DrawButton(dc,font_small_,kCfMsTestForce,s.exp_ms_test_force?L"[X] MS TEST: LOG PRESSES":L"[ ] MS TEST: LOG PRESSES",s.exp_ms_test_force?kAccent:kPanel2); // DS-F5
        DrawButton(dc,font_small_,kCfComboTestForce,!s.exp_combo_window_extend?L"COMBO TEST: EXTEND OFF":s.exp_combo_test_force?L"[X] COMBO TEST: HOLD 5s":L"[ ] COMBO TEST: HOLD 5s",s.exp_combo_window_extend&&s.exp_combo_test_force?kAccent:kPanel2); // DS-F6
        DrawButton(dc,font_small_,kCfKicks26,L"KICKS  >  (game kick or VR leg, kick off an impaled enemy)",RGB(176,148,98)); // C26

#elif defined(KCD2VR_MENU_CLICK_COMBATFLOW59C17)
 // R59.16C10R17
        if(begin_drag(kCfHitTime59c17,DragControl::ExpNativeHitTime59c17)||begin_drag(kCfComboWindow59c17,DragControl::ExpComboWindow59c17)||
           begin_drag(kCfMsHandoff23,DragControl::ExpMsHandoff23)) return;
        auto flip=[&](const UiRect& r,bool& v,const char* name)->bool{ if(!PtIn(r,x,y))return false; v=!v; ApplyState(s); save(); char m[96]{}; std::snprintf(m,sizeof(m),"R59.16C10R17-COMBAT-FLOW %s=%s",name,v?"ON":"OFF"); diagnostic_trace::Write(m); MarkDirty(); return true; };
        if(flip(kCfComboExtend59c17,s.exp_combo_window_extend,"VrComboWindowExtend")||flip(kCfGuardStrict59c17,s.exp_guard_strict,"VrGuardStrictEntry")||flip(kCfNoCooldown59c17,s.exp_no_block_cooldown,"VrNoBlockCooldownF4")||
           flip(kCfMsStance59c17,s.exp_ms_keep_stance,"VrMasterStrikeKeepStance"))return;
        if(flip(kCfInstantFree38,s.exp_instant_free_swing38,"VrInstantFreeSwing")||flip(kCfHideLean38,s.exp_hide_swing_lean38,"VrHideSwingLean")) return; // C38
        if(flip(kCfMsTestForce,s.exp_ms_test_force,"VrMasterStrikeTestForce")) return; // DS-F5
        if(flip(kCfComboZoneCZ,s.exp_combo_zone_assist,"VrComboZoneAssist")||flip(kCfComboHoldN2,s.exp_combo_recovery_hold,"VrComboRecoveryHold")) return; // I12
        if(s.exp_combo_window_extend&&flip(kCfComboTestForce,s.exp_combo_test_force,"VrComboTestForce")) return; // DS-F6
        if(PtIn(kCfKicks26,x,y)){ GoToPage(MenuPage::Kicks26,+1); return; } // C26
        return;

#elif defined(KCD2VR_MENU_DRAW_COUNTERSTAB40)
        // Retired direct-contact page: navigation is redirected to shared combat settings.
#elif defined(KCD2VR_MENU_CLICK_COUNTERSTAB40)
        GoToPage(MenuPage::Experiments,+1);return;
#endif
