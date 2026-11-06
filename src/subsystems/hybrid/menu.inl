// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_DRAW_EXPERIMENTS)
 // C49: only the main sliders (Miki 17 Oct); everything else is on MORE COMBAT TUNING
        namespace cm=combat_menu49;
        pageTitle(L"EXPERIMENTS",L"Veto bypass: mod opponent / rhythm only; native rules remain"); vrNav();
        std::swprintf(v,128,L"%.2f m",double(s.exp_npc_proximity_radius36)); DrawSlider(dc,font_small_,kCm49Proximity,L"Proximity distance (blade near his body, F3)",cm::proximityT(s.exp_npc_proximity_radius36),v);
        DrawButton(dc,font_small_,kCm49BlockSpeed,L"NPC COMBAT SETTINGS >",kPanel2);
        std::swprintf(v,128,L"x%.2f",double(s.exp_weight_feel)); DrawSlider(dc,font_small_,kCm49Weight,L"Weapon weight and drag (lower = heavier)",cm::weightT(s.exp_weight_feel),v);
        DrawSlider(dc,font_small_,kCm49Skill,L"Skill feel: real / low / medium / high",cm::skillT(s.stat_sim26),s.stat_sim26==1?L"LOW":s.stat_sim26==2?L"MEDIUM":s.stat_sim26==3?L"HIGH":L"REAL");
        DrawButton(dc,font_small_,kCm49InstantFree,s.exp_instant_free_swing38?L"[X] COMBAT FLOW: FREE SWING STARTS AT ONCE (no native 0.3 s wait)":L"[ ] COMBAT FLOW: FREE SWING STARTS AT ONCE (native 0.3 s wait)",s.exp_instant_free_swing38?kAccent:kPanel2);

#define KCD2VR_MENU_DRAW_KCM49IMPALEKILL
#include "subsystems/gore/menu.inl"
#undef KCD2VR_MENU_DRAW_KCM49IMPALEKILL


#define KCD2VR_MENU_DRAW_KCM49DECAPKILL
#include "subsystems/gore/menu.inl"
#undef KCD2VR_MENU_DRAW_KCM49DECAPKILL

        DrawButton(dc,font_small_,U49(cm::kDefaults54),L"COMBAT DEFAULTS",RGB(176,148,98));
        DrawButton(dc,font_small_,U49(cm::kShowBlockH24),s.show_block_indicator_h24?L"[X] SHOW BLOCK INDICATOR":L"[ ] SHOW BLOCK INDICATOR",s.show_block_indicator_h24?kAccent:kPanel2);
        DrawButton(dc,font_small_,kWindowNavH41,L"NATIVE WINDOWS / TRIALS >",kPanel2);
        DrawButton(dc,font_small_,U49(cm::kNote),s.bypass_mod_vetoes_h41?L"[X] Disable extra mod attack vetoes":L"[ ] Disable extra mod attack vetoes",s.bypass_mod_vetoes_h41?kAccent:kPanel2);
        DrawButton(dc,font_small_,U49(claude_h8::kOpenCombat),L"POMMEL / KICK TESTS >",kPanel2);
        DrawButton(dc,font_small_,kCm49More,L"MORE COMBAT TUNING >",RGB(176,148,98));
        DrawButton(dc,font_small_,kCm49Flow,L"COMBAT FLOW  >  (perfect block, master strike, combos, free swing)",RGB(176,148,98));

#elif defined(KCD2VR_MENU_CLICK_EXPERIMENTS)
        if(PtIn(U49(combat_menu49::kShowBlockH24),x,y)){s.show_block_indicator_h24=!s.show_block_indicator_h24;ApplyState(s);save();MarkDirty();return;}
 // C49: first fight page - the 4 main sliders + FREE SWING STARTS AT ONCE
        if(PtIn(kCm54Defaults,x,y)){ combat_menu49::defaults54(s);ApplyState(s);save();MarkDirty();diagnostic_trace::Write("C54 COMBAT_DEFAULTS mainSlidersAndGuard=RESET HUD_BOW_CAMERA_CALIBRATION=UNCHANGED");return; }
        if(PtIn(kCm49BlockSpeed,x,y)){GoToPage(MenuPage::NpcTuning,+1);return;}
        if(begin_drag(kCm49Proximity,DragControl::ExpProximity49)||
           begin_drag(kCm49Weight,DragControl::ExpWeight49)||begin_drag(kCm49Skill,DragControl::ExpSkill49)) return;
        if(PtIn(kCm49InstantFree,x,y)){ s.exp_instant_free_swing38=!s.exp_instant_free_swing38; ApplyState(s); save(); diagnostic_trace::Write(s.exp_instant_free_swing38?"C49-EXPERIMENTS VrInstantFreeSwing=ON":"C49-EXPERIMENTS VrInstantFreeSwing=OFF"); MarkDirty(); return; } // same state as COMBAT FLOW

#define KCD2VR_MENU_CLICK_KCM49IMPALEKILL
#include "subsystems/gore/menu.inl"
#undef KCD2VR_MENU_CLICK_KCM49IMPALEKILL
 // C45: same state as IMMERSION > EXPERIMENTS

#define KCD2VR_MENU_CLICK_KCM49DECAPKILL
#include "subsystems/gore/menu.inl"
#undef KCD2VR_MENU_CLICK_KCM49DECAPKILL
 // C45/C46: same state as IMMERSION
        if(PtIn(kCm49More,x,y)){ GoToPage(MenuPage::CombatTuning49,+1); return; }
        if(PtIn(kCm49Flow,x,y)){ GoToPage(MenuPage::CombatFlow59c17,+1); return; }
        return;

#elif defined(KCD2VR_MENU_DRAW_COMBATTUNING49)
 // R59.16C10d page 1, moved behind EXPERIMENTS in C49 (rects unchanged)
        pageTitle(L"MORE COMBAT TUNING",L"Combat tuning - changes apply immediately and are saved"); vrNav();
        DrawButton(dc,font_small_,kExpTelegraphWindup,s.exp_telegraph_windup?L"[X] HYBRID: WIND-UP CUE":L"[ ] HYBRID: WIND-UP CUE",s.exp_telegraph_windup?kAccent:kPanel2);
        DrawButton(dc,font_small_,kExpTelegraphStrike,s.exp_telegraph_strike?L"[X] ANNOUNCE ATTACK EARLY":L"[ ] ANNOUNCE ATTACK EARLY",s.exp_telegraph_strike?kAccent:kPanel2);
        DrawButton(dc,font_small_,kExpWindupHold,s.exp_windup_hold?L"[X] HYBRID: HOLD TO PREPARE":L"[ ] HYBRID: HOLD TO PREPARE",s.exp_windup_hold?kAccent:kPanel2);
        DrawButton(dc,font_small_,kExpGuardCounter,s.exp_guard_counter_motion?L"[X] PARRY INPUT: COUNTER-MOTION":L"[ ] PARRY INPUT: WINDOW ASSIST",s.exp_guard_counter_motion?kAccent:kPanel2); // R59.16C10R17: all F4 modes
        DrawButton(dc,font_small_,kExpContextualHud,s.contextual_hud?L"[X] PALM HUD":L"[ ] PALM HUD",s.contextual_hud?kAccent:kPanel2);
        DrawButton(dc,font_small_,kExpBaseline35,L"COMBAT BASELINE",RGB(176,148,98));
        std::swprintf(v,128,s.exp_guard_enter_speed<=0.f?L"off":L"%.1f m/s",double(s.exp_guard_enter_speed)); DrawSlider(dc,font_small_,kExpGuardEnterSpeed,L"Guard ENTER: blade must be slower than",s.exp_guard_enter_speed/5.f,v);
        std::swprintf(v,128,L"%d ms",s.exp_guard_enter_dwell_ms); DrawSlider(dc,font_small_,kExpGuardEnterDwell,L"Guard ENTER: hold the posture this long first",s.exp_guard_enter_dwell_ms/600.f,v);
        std::swprintf(v,128,s.exp_guard_exit_speed<=0.f?L"position only":L"%.1f m/s",double(s.exp_guard_exit_speed)); DrawSlider(dc,font_small_,kExpGuardExitSpeed,L"Block release speed: faster forward motion leaves guard",s.exp_guard_exit_speed/6.f,v);
        std::swprintf(v,128,L"%d ms",s.exp_guard_after_attack_ms); DrawSlider(dc,font_small_,kExpGuardAfterAttack,L"No guard this long after a committed stroke",s.exp_guard_after_attack_ms/2000.f,v);
        std::swprintf(v,128,L"%.0f J",double(s.exp_strike_reference_j)); DrawSlider(dc,font_small_,kExpStrikeRefJ,L"Strike power: energy of YOUR full stroke (= 100%)",(s.exp_strike_reference_j-10.f)/190.f,v);
        std::swprintf(v,128,L"%.0f%%",double(s.exp_strike_max_power*100.f)); DrawSlider(dc,font_small_,kExpStrikeMaxPower,L"Strike power cap (of a normal native swing)",(s.exp_strike_max_power-.5f)/1.5f,v);
        std::swprintf(v,128,L"x%.2f",double(s.exp_strike_two_handed)); DrawSlider(dc,font_small_,kExpStrikeTwoHanded,L"Two-handed bonus (inside the cap)",(s.exp_strike_two_handed-1.f)/1.f,v);
        std::swprintf(v,128,L"%.0f cm",double(s.exp_head_raise_cm)); DrawSlider(dc,font_small_,kExpHeadRaise,L"Head may rise above the calibrated height by",s.exp_head_raise_cm/60.f,v);
        DrawButton(dc,font_small_,kExpAutoWindup19c4r1,s.exp_guard_auto_windup?L"[X] AUTO GUARD REFRESH ON ENEMY WIND-UP (native timing)":L"[ ] AUTO GUARD REFRESH ON ENEMY WIND-UP (native timing)",s.exp_guard_auto_windup?kAccent:kPanel2);
        DrawButton(dc,font_small_,kExpMore59c12,L"COMBAT TUNING 2  >  (NPC block timing, attack gate, R12 switches)",RGB(176,148,98)); // R59.16C10R12
        DrawButton(dc,font_small_,kCm43Touch,L"NPC TOUCH: PAUSED",kPanel2); // C43T (C46: bottom row of this page)
        DrawButton(dc,font_small_,kCm43Fall,L"FALL / PLAY: PAUSED",kPanel2); // C43T

#elif defined(KCD2VR_MENU_CLICK_COMBATTUNING49)
 // R59.16C10d page 1, moved unchanged behind EXPERIMENTS in C49
        if(PtIn(kExpBaseline35,x,y)){
            // Explicit comparison preset only; never loaded automatically.
            // Preserve mode, RPG outcomes, HUD, bow, camera and calibration.
            combat_menu49::defaults54(s);
            ApplyState(s);save();MarkDirty();
            diagnostic_trace::Write("C35 COMBAT_BASELINE forwardGuard=APPLIED windupPrepare=OFF guardEnter=2.5 dwellMs=0 guardExit=3.5 counterMotion=ON automaticPerfectBlock=OFF mode=UNCHANGED");
            return;
        }
        if(begin_drag(kExpGuardEnterSpeed,DragControl::ExpGuardEnterSpeed59c10)||begin_drag(kExpGuardEnterDwell,DragControl::ExpGuardEnterDwell59c10)||begin_drag(kExpGuardExitSpeed,DragControl::ExpGuardExitSpeed59c10)||begin_drag(kExpGuardAfterAttack,DragControl::ExpGuardAfterAttack59c10)) return;
        if(begin_drag(kExpStrikeRefJ,DragControl::ExpStrikeRefJ59c10)||begin_drag(kExpStrikeMaxPower,DragControl::ExpStrikeMaxPower59c10)||begin_drag(kExpStrikeTwoHanded,DragControl::ExpStrikeTwoHanded59c10)||begin_drag(kExpHeadRaise,DragControl::ExpHeadRaise59c10)) return;
        if(PtIn(kExpMore59c12,x,y)){ GoToPage(MenuPage::Experiments2,+1); return; } // R59.16C10R12
        if(PtIn(kCm43Touch,x,y)||PtIn(kCm43Fall,x,y))return; // D51 unavailable living actuation
        auto flip=[&](const UiRect& r,bool& v,const char* name)->bool{ if(!PtIn(r,x,y))return false; v=!v; ApplyState(s); save(); char m[96]{}; std::snprintf(m,sizeof(m),"R59.16C10d-EXPERIMENTS %s=%s",name,v?"ON":"OFF"); diagnostic_trace::Write(m); MarkDirty(); return true; };
        if(flip(kExpTelegraphWindup,s.exp_telegraph_windup,"VrTelegraphWindup")||flip(kExpTelegraphStrike,s.exp_telegraph_strike,"VrTelegraphStrike")||flip(kExpWindupHold,s.exp_windup_hold,"VrWindupHold")||
           flip(kExpGuardCounter,s.exp_guard_counter_motion,"VrGuardRequireCounterMotion")||flip(kExpContextualHud,s.contextual_hud,"VrContextualHud")||
           flip(kExpAutoWindup19c4r1,s.exp_guard_auto_windup,"VrGuardAutoWindupRefresh")) return;
        return;

#endif
