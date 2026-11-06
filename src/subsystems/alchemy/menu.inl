// Owned crafting menu: saved modes0/1/2 plus the distinct6DOF mode3, shown in both places.
#if defined(KCD2VR_MENU_DRAW_IMMERSION_CRAFTING_H18)
        {const int chosen=kcd2vr::crafting_menu_h18::selected(s,kcd2vr::minigame_hands45::Alchemy);
         std::swprintf(v,128,L"ALCHEMY: %ls",kcd2vr::crafting_menu_h18::label(chosen));
         DrawButton(dc,font_small_,kMinigameHands45,v,chosen?kAccent:kPanel2);}
#define KCD2VR_MENU_DRAW_IMMERSION_SMITH_H18
#include "subsystems/smithing/menu.inl"
#undef KCD2VR_MENU_DRAW_IMMERSION_SMITH_H18
#elif defined(KCD2VR_MENU_CLICK_IMMERSION_CRAFTING_H18)
        if(PtIn(kMinigameHands45,x,y)){
            kcd2vr::crafting_menu_h18::next(s,kcd2vr::minigame_hands45::Alchemy);
            ApplyState(s);save();MarkDirty();return;
        }
#define KCD2VR_MENU_CLICK_IMMERSION_SMITH_H18
#include "subsystems/smithing/menu.inl"
#undef KCD2VR_MENU_CLICK_IMMERSION_SMITH_H18
#elif defined(KCD2VR_MENU_DRAW_CRAFTING67)
        pageTitle(L"ALCHEMY / SMITHING",L"Independent saved modes; F5 remains a separate test cycle");vrNav();
        Text(dc,font_small_,L"ALCHEMY",620,120,418,28,kText,DT_SINGLELINE);
        for(int mode=0;mode<kcd2vr::crafting_menu_h18::kModeCount;++mode)DrawButton(dc,font_small_,U49(kcd2vr::crafting_menu_h18::kChoices[0][mode]),
            kcd2vr::crafting_menu_h18::label(mode),mode==kcd2vr::crafting_menu_h18::selected(s,kcd2vr::minigame_hands45::Alchemy)?kAccent:kPanel2);
        Text(dc,font_small_,L"NATIVE: authored hands and A. SEMI: tracked hands between native actions. FREE VRIK: tracked tool gestures. 6DOF VRIK: head, hands and visible body follow station movement. Native station placement, recipes and A fallback stay active.",620,204,418,88,kMuted,DT_WORDBREAK);
#define KCD2VR_MENU_DRAW_CRAFTING67_SMITH
#include "subsystems/smithing/menu.inl"
#undef KCD2VR_MENU_DRAW_CRAFTING67_SMITH
        DrawButton(dc,font_small_,UiRect{620,604,1038,636},s.craft_trial_h41?L"[X] CRAFT1 near-selection / smith fore-aft trial":L"[ ] CRAFT1 near-selection / smith fore-aft trial",s.craft_trial_h41?kAccent:kPanel2);
        Text(dc,font_tiny_?font_tiny_:font_small_,L"F5 rung1 records only. Saved FREE / 6DOF VRIK needs no F5 press. Rung2 and guided rung3 remain separate experiments. Switching a mode retires that hand-control generation; unsupported native boundaries stay native.",620,490,418,72,kMuted,DT_WORDBREAK);
#elif defined(KCD2VR_MENU_CLICK_CRAFTING67)
        if(PtIn(UiRect{620,604,1038,636},x,y)){s.craft_trial_h41=!s.craft_trial_h41;ApplyState(s);save();MarkDirty();return;}
        for(int mode=0;mode<kcd2vr::crafting_menu_h18::kModeCount;++mode)if(PtIn(U49(kcd2vr::crafting_menu_h18::kChoices[0][mode]),x,y)){
            kcd2vr::crafting_menu_h18::select(s,kcd2vr::minigame_hands45::Alchemy,mode);
            ApplyState(s);save();MarkDirty();return;
        }
#define KCD2VR_MENU_CLICK_CRAFTING67_SMITH
#include "subsystems/smithing/menu.inl"
#undef KCD2VR_MENU_CLICK_CRAFTING67_SMITH
#endif
