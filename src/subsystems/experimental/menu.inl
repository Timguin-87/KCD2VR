// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_CLICK_KHEADDODGE54)
if(PtIn(kHeadDodge54,x,y)){s.roomscale_mode=combat_menu49::dodgeMode54(s.roomscale_mode);ApplyState(s);save();MarkDirty();return;}
#elif defined(KCD2VR_MENU_DRAW_KHEADDODGE54)
        DrawButton(dc,font_small_,kHeadDodge54,headDodge54?L"HEADSET DODGE: ON":L"HEADSET DODGE: OFF",headDodge54?kAccent:kPanel2);
#elif defined(KCD2VR_MENU_CLICK_KIXCLINCH32)
if(PtIn(kIxClinch32,x,y)){
            s.clinch_distance32=live_settings_d39::next(s.clinch_distance32,std::array<float,5>{0,.40f,.45f,.55f,.70f});
            done("C32 CONFIG current-pair clinch initiation limit changed (0=native)");return;}
#elif defined(KCD2VR_MENU_DRAW_KIXCLINCH32)
        DrawButton(dc,font_small_,kIxClinch32,v,RGB(176,148,98));
#elif defined(KCD2VR_MENU_DRAW_KICKS26)
 // C26
        pageTitle(L"KICKS",L"R3 kick - changes apply immediately and are saved"); vrNav(); DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        DrawButton(dc,font_small_,kKkStyle26,s.kick_style26==1?L"KICK LOOK: VR LEG, GAME RULES (our leg animation, the game's kick decides)":s.kick_style26==2?L"KICK LOOK: VR LEG CONTACT PROBE (no damage)":L"KICK LOOK: GAME KICK (the game's own kick animation and rules)",s.kick_style26==2?kPanel2:kAccent);
        DrawButton(dc,font_small_,kKkLeg26,s.kick_leg26==1?L"VR LEG: LEFT":s.kick_leg26==2?L"VR LEG: ALTERNATE":L"VR LEG: RIGHT",kPanel2);
        std::swprintf(v,128,L"%.2f s",double(s.kick_reach_ms26)/1000.0); DrawSlider(dc,font_small_,kKkReach26,L"Foot reaches the enemy this long after the game kick starts",(s.kick_reach_ms26-300)/900.f,v);
        DrawButton(dc,font_small_,kKkImpale26,s.kick_frees_impale26?L"[X] A KICK FREES YOUR BLADE FROM AN IMPALED ENEMY":L"[ ] A KICK FREES YOUR BLADE FROM AN IMPALED ENEMY (pull it out by hand)",s.kick_frees_impale26?kAccent:kPanel2);
        DrawButton(dc,font_small_,kKkRoute33,s.kick_special33?L"R3 WITH A WEAPON: GAME SPECIAL ATTACK (kick at zone 5)":L"R3 WITH A WEAPON: OLD ZONE-3 ROUTE (no game kick)",s.kick_special33?kAccent:kPanel2); // C33
        DrawButton(dc,font_small_,kKkRealHit37c2,L"VR LEG CONTACT PROBE: LOG ONLY (no damage)",kPanel2); // C37C2
        DrawButton(dc,font_small_,kKkFallback33,s.kick_fallback33?L"[X] VR LEG MODE ONLY: FALLBACK AFTER 0.25 s":L"[ ] VR LEG MODE ONLY: FALLBACK AFTER 0.25 s",s.kick_fallback33?kAccent:kPanel2); // GAME KICK always uses native clip
        DrawButton(dc,font_small_,kKkProfile52,s.kick_profile52==1?L"KICK STYLE: OLD (knee 0.11 s, foot out 0.2-0.3 s, back by 0.52 s)":L"LEG PROFILE: NORMAL-KICK REFERENCE (0.73 s extension; native rules unchanged)",s.kick_profile52==1?kPanel2:kAccent); // C52
        std::swprintf(v,128,L"%d %%",s.kick_reach52); DrawSlider(dc,font_small_,kKkReach52,L"VR leg reach (straight leg = 100 %)",(s.kick_reach52-60)/40.f,v); // C52
        Text(dc,font_small_,L"Kicks follow the game's rules: a kick can break a block (best against shields) or push the enemy back, and he can still dodge it. Damage, stamina and the dice stay native. GAME KICK and VR LEG use the same native kick - only the leg you see differs. VR LEG CONTACT PROBE draws the leg and records foot contact under F3. The contact probe cannot apply damage or spend stamina. Use GAME KICK or VR LEG, GAME RULES for native kicks. R3 at an impaled enemy frees the blade.",620,394,418,150,kMuted,DT_LEFT|DT_WORDBREAK);

#elif defined(KCD2VR_MENU_CLICK_KICKS26)
 // C26 KICKS page
        if(begin_drag(kKkReach26,DragControl::KickReach26)) return;
        if(PtIn(kKkProfile52,x,y)){ s.kick_profile52=(s.kick_profile52+1)%2; ApplyState(s); save(); char m[96]{}; std::snprintf(m,sizeof(m),"C52-KICKS VrKickProfile=%d",s.kick_profile52); diagnostic_trace::Write(m); MarkDirty(); return; } // C52
        if(begin_drag(kKkReach52,DragControl::KickReach52)) return; // C52
        if(PtIn(kKkStyle26,x,y)){ s.kick_style26=(s.kick_style26+1)%3; ApplyState(s); save(); char m[96]{}; std::snprintf(m,sizeof(m),"C26-KICKS VrKickStyle=%d",s.kick_style26); diagnostic_trace::Write(m); MarkDirty(); return; }
        if(PtIn(kKkLeg26,x,y)){ s.kick_leg26=(s.kick_leg26+1)%3; ApplyState(s); save(); char m[96]{}; std::snprintf(m,sizeof(m),"C26-KICKS VrKickLeg=%d",s.kick_leg26); diagnostic_trace::Write(m); MarkDirty(); return; }
        if(PtIn(kKkRoute33,x,y)){ s.kick_special33=!s.kick_special33; ApplyState(s); save(); diagnostic_trace::Write(s.kick_special33?"C33-KICKS VrKickSpecialInput=ON":"C33-KICKS VrKickSpecialInput=OFF"); MarkDirty(); return; }
        if(PtIn(kKkRealHit37c2,x,y)){ MarkDirty(); return; } // Contact probe has no damage toggle.
        if(PtIn(kKkFallback33,x,y)){ s.kick_fallback33=!s.kick_fallback33; ApplyState(s); save(); diagnostic_trace::Write(s.kick_fallback33?"C33-KICKS VrKickFallbackLeg=ON":"C33-KICKS VrKickFallbackLeg=OFF"); MarkDirty(); return; }
        if(PtIn(kKkImpale26,x,y)){ s.kick_frees_impale26=!s.kick_frees_impale26; ApplyState(s); save(); diagnostic_trace::Write(s.kick_frees_impale26?"C26-KICKS VrKickFreesImpale=ON":"C26-KICKS VrKickFreesImpale=OFF"); MarkDirty(); return; }
        return;

#elif defined(KCD2VR_MENU_DRAW_IMMERSIONEXPERIMENTS)
 // R59.16C10R8
        DrawButton(dc,font_small_,U49(claude_h8::kOpenImmersion),L"POMMEL / KICK / IMPALE TESTS >",kPanel2);
        pageTitle(L"EXPERIMENTS",L"Switch experimental features on/off - saved immediately");
        DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));

#define KCD2VR_MENU_DRAW_KIXPHYSBODIES
#include "subsystems/physics/menu.inl"
#undef KCD2VR_MENU_DRAW_KIXPHYSBODIES


#define KCD2VR_MENU_DRAW_KIXBODYFOLLOW
#include "subsystems/vrik/menu.inl"
#undef KCD2VR_MENU_DRAW_KIXBODYFOLLOW


#define KCD2VR_MENU_DRAW_KIXROOMSCALE
#include "subsystems/vrik/menu.inl"
#undef KCD2VR_MENU_DRAW_KIXROOMSCALE


#define KCD2VR_MENU_DRAW_KIXGESTURES
#include "subsystems/hud_ui_controls/menu.inl"
#undef KCD2VR_MENU_DRAW_KIXGESTURES


#define KCD2VR_MENU_DRAW_KIXRAGDOLL
#include "subsystems/physics/menu.inl"
#undef KCD2VR_MENU_DRAW_KIXRAGDOLL


#define KCD2VR_MENU_DRAW_KIXDECAP
#include "subsystems/gore/menu.inl"
#undef KCD2VR_MENU_DRAW_KIXDECAP


#define KCD2VR_MENU_DRAW_KIXRAGDOLLPUSH33
#include "subsystems/physics/menu.inl"
#undef KCD2VR_MENU_DRAW_KIXRAGDOLLPUSH33


#define KCD2VR_MENU_DRAW_KIXIMPALE32
#include "subsystems/gore/menu.inl"
#undef KCD2VR_MENU_DRAW_KIXIMPALE32

        DrawButton(dc,font_small_,kIxCombat,L"COMBAT TUNING  >",RGB(176,148,98));

#define KCD2VR_MENU_DRAW_KIXCHESTDAGGERT3
#include "subsystems/throw_drop/menu.inl"
#undef KCD2VR_MENU_DRAW_KIXCHESTDAGGERT3


#define KCD2VR_MENU_DRAW_KIXMAPREGION32
#include "subsystems/vrmap/menu.inl"
#undef KCD2VR_MENU_DRAW_KIXMAPREGION32

        if(s.clinch_distance32<=0)std::swprintf(v,128,L"CLINCH INITIATION: NATIVE");
        else std::swprintf(v,128,L"CLINCH INITIATION: %.2f m (current native pair)",double(s.clinch_distance32));

#define KCD2VR_MENU_DRAW_KIXCLINCH32
#include "subsystems/experimental/menu.inl"
#undef KCD2VR_MENU_DRAW_KIXCLINCH32

        DrawButton(dc,font_small_,kTimedPromptsD80,prompt_policy_d80::timed.load()?L"TIMED HINTS: ON":L"TIMED HINTS: OFF",prompt_policy_d80::timed.load()?kAccent:kPanel2);
        std::swprintf(v,128,L"CRAFTING UI SIZE: %.0f%%",double(prompt_policy_d80::minigameScale.load()*100.f)); // R145: same value as the VR HUD slider
        DrawButton(dc,font_small_,kPromptScaleD80,v,kPanel2);
        Text(dc,font_small_,L"Station hints: ten seconds on entry or a new step. Look at an open palm to reveal again. Crafting UI size (prompts + icons over the station things): 30 / 40 / 50 / 60 / 90 / 100%, any 5% step on VR HUD > INTERACTION / PROMPTS. Position stays native.",620,566,418,54,kMuted,DT_LEFT|DT_WORDBREAK);

#elif defined(KCD2VR_MENU_CLICK_IMMERSIONEXPERIMENTS)
 // R59.16C10R8
        if(PtIn(U49(claude_h8::kOpenImmersion),x,y)){GoToPage(MenuPage::ClaudeExperimentsH8,+1);return;}
        if(PtIn(kPageBack,x,y)){ GoToPage(MenuPage::Dialogue,-1); return; }
        auto done=[&](const char* m){ ApplyState(s); save(); diagnostic_trace::Write(m); MarkDirty(); };
        if(PtIn(kTimedPromptsD80,x,y)){prompt_policy_d80::timed.store(!prompt_policy_d80::timed.load());prompt_policy_d80::Refresh();save();MarkDirty();return;}
        if(PtIn(kPromptScaleD80,x,y)){const float old=prompt_policy_d80::minigameScale.load();prompt_policy_d80::minigameScale.store(old<.35f?.4f:old<.45f?.5f:old<.55f?.6f:old<.85f?.9f:old<.99f?1.f:.3f);prompt_policy_d80::Refresh();save();MarkDirty();return;}

#define KCD2VR_MENU_CLICK_KIXPHYSBODIES
#include "subsystems/physics/menu.inl"
#undef KCD2VR_MENU_CLICK_KIXPHYSBODIES


#define KCD2VR_MENU_CLICK_KIXBODYFOLLOW
#include "subsystems/vrik/menu.inl"
#undef KCD2VR_MENU_CLICK_KIXBODYFOLLOW


#define KCD2VR_MENU_CLICK_KIXROOMSCALE
#include "subsystems/vrik/menu.inl"
#undef KCD2VR_MENU_CLICK_KIXROOMSCALE


#define KCD2VR_MENU_CLICK_KIXGESTURES
#include "subsystems/hud_ui_controls/menu.inl"
#undef KCD2VR_MENU_CLICK_KIXGESTURES


#define KCD2VR_MENU_CLICK_KIXRAGDOLL
#include "subsystems/physics/menu.inl"
#undef KCD2VR_MENU_CLICK_KIXRAGDOLL


#define KCD2VR_MENU_CLICK_KIXDECAP
#include "subsystems/gore/menu.inl"
#undef KCD2VR_MENU_CLICK_KIXDECAP
 // C44/C46: OFF -> HIDE -> LOOSE NECK -> STUMP+HEAD

#define KCD2VR_MENU_CLICK_KIXRAGDOLLPUSH33
#include "subsystems/physics/menu.inl"
#undef KCD2VR_MENU_CLICK_KIXRAGDOLLPUSH33


#define KCD2VR_MENU_CLICK_KIXIMPALE32
#include "subsystems/gore/menu.inl"
#undef KCD2VR_MENU_CLICK_KIXIMPALE32

        if(PtIn(kIxCombat,x,y)){ GoToPage(MenuPage::Experiments,+1); return; }

#define KCD2VR_MENU_CLICK_KIXCHESTDAGGERT3
#include "subsystems/throw_drop/menu.inl"
#undef KCD2VR_MENU_CLICK_KIXCHESTDAGGERT3


#define KCD2VR_MENU_CLICK_KIXMAPREGION32
#include "subsystems/vrmap/menu.inl"
#undef KCD2VR_MENU_CLICK_KIXMAPREGION32


#define KCD2VR_MENU_CLICK_KIXCLINCH32
#include "subsystems/experimental/menu.inl"
#undef KCD2VR_MENU_CLICK_KIXCLINCH32

        return;

#endif
