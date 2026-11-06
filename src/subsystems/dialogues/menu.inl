// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_DRAW_DIALOGUE)

        pageTitle(L"IMMERSION",L"Dialogue and crafting body ownership"); vrNav();
        DrawButton(dc,font_normal_,kV4DialogNative,L"NATIVE / THEATER",s.dialogue_camera_mode==0?kAccent:kPanel2);
        DrawButton(dc,font_normal_,kV4DialogNoCamera,L"IMMERSIVE FIRST PERSON",s.dialogue_camera_mode==2?kAccent:kPanel2);
#define KCD2VR_MENU_DRAW_STOCK_T8
#include "subsystems/throw_drop/menu.inl" // T8 owned menu contribution, existing note rectangle
#undef KCD2VR_MENU_DRAW_STOCK_T8
        DrawButton(dc,font_small_,U49(immersion_menu45::kShowHintsD80),prompt_policy_d80::showHints.load()?L"SHOW HINTS: ON":L"SHOW HINTS: OFF",prompt_policy_d80::showHints.load()?kAccent:kPanel2);
#define KCD2VR_MENU_DRAW_IMMERSION_CRAFTING_H18
#include "subsystems/alchemy/menu.inl"
#undef KCD2VR_MENU_DRAW_IMMERSION_CRAFTING_H18
        DrawButton(dc,font_small_,U49(immersion_menu45::kCraftingDetailsH18),L"CRAFTING DETAILS >",kPanel2);

#define KCD2VR_MENU_DRAW_KPRECISECONTACTS44
#include "subsystems/physics/menu.inl"
#undef KCD2VR_MENU_DRAW_KPRECISECONTACTS44


#define KCD2VR_MENU_DRAW_KBLOODYFIGHTS49
#include "subsystems/gore/menu.inl"
#undef KCD2VR_MENU_DRAW_KBLOODYFIGHTS49

        DrawButton(dc,font_small_,kDialogueSmooth38,s.dialogue_smooth38?L"[X] SMOOTH DIALOGUE":L"[ ] SMOOTH DIALOGUE",s.dialogue_smooth38?kAccent:kPanel2);
        const bool headDodge54=s.roomscale_mode==2||s.roomscale_mode==3;

#define KCD2VR_MENU_DRAW_KHEADDODGE54
#include "subsystems/experimental/menu.inl"
#undef KCD2VR_MENU_DRAW_KHEADDODGE54


#define KCD2VR_MENU_DRAW_KV149USERHOLSTERS
#include "subsystems/throw_drop/menu.inl"
#undef KCD2VR_MENU_DRAW_KV149USERHOLSTERS


#define KCD2VR_MENU_DRAW_KV149MANUALBOWRELOAD
#include "subsystems/ranged_reload/menu.inl"
#undef KCD2VR_MENU_DRAW_KV149MANUALBOWRELOAD


#define KCD2VR_MENU_DRAW_KDECAPITATION59C7E
#include "subsystems/gore/menu.inl"
#undef KCD2VR_MENU_DRAW_KDECAPITATION59C7E


#define KCD2VR_MENU_DRAW_KRAGDOLLPHYSICS59C10R7
#include "subsystems/physics/menu.inl"
#undef KCD2VR_MENU_DRAW_KRAGDOLLPHYSICS59C10R7

        if(developer_mode::Enabled())DrawButton(dc,font_small_,kImmersionExperiments59c10r8,L"PHYSICS / 6DOF >",RGB(176,148,98));

#define KCD2VR_MENU_DRAW_KHMDHEIGHT54
#include "subsystems/vrik/menu.inl"
#undef KCD2VR_MENU_DRAW_KHMDHEIGHT54

        std::swprintf(v,128,L"%+.0f cm",double(s.weapon_grip_cm)); DrawSlider(dc,font_small_,kWeaponGrip59c7f,L"Weapon in hand: wrist <-> palm",(s.weapon_grip_cm+15.f)/30.f,v); // R59.16C7F
        DrawButton(dc,font_small_,kSupportSlot34,s.support_slot34?L"L-FALLBACK: WEAPON SLOT":L"L-FALLBACK: SLIDERS",s.support_slot34?kAccent:kPanel2); // C37: a captured native left frame always wins
        std::swprintf(v,128,L"%.0f cm",double(s.support_sword_cm)); DrawSlider(dc,font_small_,kSupportSword59c7f,L"Sword synthetic fallback",(s.support_sword_cm-6.f)/8.f,v);
        std::swprintf(v,128,L"%.0f cm",double(s.support_pole_cm)); DrawSlider(dc,font_small_,kSupportPole59c7f,L"Pole synthetic fallback (no native/slot frame)",(s.support_pole_cm-6.f)/54.f,v);
        DrawButton(dc,font_small_,kAutoPalm59c8,s.auto_palm?L"[X] PALM AUTO-ALIGN":L"[ ] PALM AUTO-ALIGN",s.auto_palm?kAccent:kPanel2); // R59.16C8

#define KCD2VR_MENU_DRAW_KWEAPONRELEASET1
#include "subsystems/throw_drop/menu.inl"
#undef KCD2VR_MENU_DRAW_KWEAPONRELEASET1

        std::swprintf(v,128,L"%.2f s",double(s.target_dwell_s)); DrawSlider(dc,font_small_,kTargetDwell59c8,L"Target switch: look away this long before the lock moves",s.target_dwell_s/1.5f,v); // R59.16C8
        std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_dialogue_scale*100.f)); DrawSlider(dc,font_small_,kV4DialogUiScale,L"Dialogue text size",(s.vr_ui_dialogue_scale-.30f)/1.20f,v);


#elif defined(KCD2VR_MENU_CLICK_DIALOGUE)
        if(PtIn(U49(immersion_menu45::kShowHintsD80),x,y)){prompt_policy_d80::showHints.store(!prompt_policy_d80::showHints.load());prompt_policy_d80::Refresh();save();MarkDirty();return;}

        if(begin_drag(kV4DialogUiScale,DragControl::UiDialogueScale)) return;
        if(PtIn(kSupportSlot34,x,y)){ s.support_slot34=!s.support_slot34; ApplyState(s); save(); MarkDirty(); return; } // C34b
        if(begin_drag(kWeaponGrip59c7f,DragControl::WeaponGrip59c7f)||begin_drag(kSupportSword59c7f,DragControl::SupportSword59c7f)||begin_drag(kSupportPole59c7f,DragControl::SupportPole59c7f)) return; // R59.16C7F
        if(begin_drag(kTargetDwell59c8,DragControl::TargetDwell59c8)) return; // R59.16C8
        if(PtIn(kAutoPalm59c8,x,y)){ s.auto_palm=!s.auto_palm; ApplyState(s); save(); diagnostic_trace::Write(s.auto_palm?"R59.16C8-IMMERSION autoPalm=ON":"R59.16C8-IMMERSION autoPalm=OFF"); MarkDirty(); return; }

#define KCD2VR_MENU_CLICK_KWEAPONRELEASET1
#include "subsystems/throw_drop/menu.inl"
#undef KCD2VR_MENU_CLICK_KWEAPONRELEASET1

        // V66.69 Immersion page: intentionally only the two production dialogue policies.
        if(PtIn(kV4DialogNative,x,y)){ s.dialogue_camera_mode=0; s.cyclopean_mode=0; ApplyState(s); save(); diagnostic_trace::Write("V66.69-IMMERSION dialogue=NATIVE_KCD2_CAMERAS_THEATER"); return; }
        if(PtIn(kV4DialogNoCamera,x,y)){ s.dialogue_camera_mode=2; s.cyclopean_mode=0; ApplyState(s); save(); diagnostic_trace::Write("V66.69-IMMERSION dialogue=FIRST_PERSON_NOCAMERAS_HMD"); return; }

#define KCD2VR_MENU_CLICK_KPRECISECONTACTS44
#include "subsystems/physics/menu.inl"
#undef KCD2VR_MENU_CLICK_KPRECISECONTACTS44


#define KCD2VR_MENU_CLICK_KBLOODYFIGHTS49
#include "subsystems/gore/menu.inl"
#undef KCD2VR_MENU_CLICK_KBLOODYFIGHTS49

        if(PtIn(kDialogueSmooth38,x,y)){ s.dialogue_smooth38=!s.dialogue_smooth38; ApplyState(s); save(); diagnostic_trace::Write(s.dialogue_smooth38?"C38-IMMERSION VrDialogueSmoothCamera=ON":"C38-IMMERSION VrDialogueSmoothCamera=OFF"); MarkDirty(); return; }

#define KCD2VR_MENU_CLICK_KV149USERHOLSTERS
#include "subsystems/throw_drop/menu.inl"
#undef KCD2VR_MENU_CLICK_KV149USERHOLSTERS

        if(developer_mode::Enabled()&&PtIn(kImmersionExperiments59c10r8,x,y)){ GoToPage(MenuPage::ImmersionExperiments,+1); return; } // R59.16C10R8

#define KCD2VR_MENU_CLICK_KHMDHEIGHT54
#include "subsystems/vrik/menu.inl"
#undef KCD2VR_MENU_CLICK_KHMDHEIGHT54


#define KCD2VR_MENU_CLICK_KHEADDODGE54
#include "subsystems/experimental/menu.inl"
#undef KCD2VR_MENU_CLICK_KHEADDODGE54

#define KCD2VR_MENU_CLICK_IMMERSION_CRAFTING_H18
#include "subsystems/alchemy/menu.inl"
#undef KCD2VR_MENU_CLICK_IMMERSION_CRAFTING_H18
        if(PtIn(U49(immersion_menu45::kCraftingDetailsH18),x,y)){GoToPage(MenuPage::Crafting67,+1);return;}

#define KCD2VR_MENU_CLICK_KRAGDOLLPHYSICS59C10R7
#include "subsystems/physics/menu.inl"
#undef KCD2VR_MENU_CLICK_KRAGDOLLPHYSICS59C10R7


#define KCD2VR_MENU_CLICK_KDECAPITATION59C7E
#include "subsystems/gore/menu.inl"
#undef KCD2VR_MENU_CLICK_KDECAPITATION59C7E
 // C44/C46

#define KCD2VR_MENU_CLICK_KV149MANUALBOWRELOAD
#include "subsystems/ranged_reload/menu.inl"
#undef KCD2VR_MENU_CLICK_KV149MANUALBOWRELOAD

        return;

#endif
