// Owned smithing choices. Shared state/apply/save and coordinate transforms stay host-owned.
#if defined(KCD2VR_MENU_DRAW_IMMERSION_SMITH_H18)
        {const int chosen=kcd2vr::crafting_menu_h18::selected(s,kcd2vr::minigame_hands45::Smithing);
         std::swprintf(v,128,L"SMITHING: %ls",kcd2vr::crafting_menu_h18::label(chosen));
         DrawButton(dc,font_small_,U49(immersion_menu45::kSmithingModeH18),v,chosen?kAccent:kPanel2);}
#elif defined(KCD2VR_MENU_CLICK_IMMERSION_SMITH_H18)
        if(PtIn(U49(immersion_menu45::kSmithingModeH18),x,y)){
            kcd2vr::crafting_menu_h18::next(s,kcd2vr::minigame_hands45::Smithing);
            ApplyState(s);save();MarkDirty();return;
        }
#elif defined(KCD2VR_MENU_DRAW_CRAFTING67_SMITH)
        Text(dc,font_small_,L"SMITHING",620,296,418,28,kText,DT_SINGLELINE);
        for(int mode=0;mode<kcd2vr::crafting_menu_h18::kModeCount;++mode)DrawButton(dc,font_small_,U49(kcd2vr::crafting_menu_h18::kChoices[1][mode]),
            kcd2vr::crafting_menu_h18::label(mode),mode==kcd2vr::crafting_menu_h18::selected(s,kcd2vr::minigame_hands45::Smithing)?kAccent:kPanel2);
        Text(dc,font_small_,L"NATIVE: authored hands. SEMI: gestures pace native work. FREE VRIK: tracked hammer/native iron support. 6DOF VRIK: tracked head, hands and visible body. Native station placement, heat, charge, quality and A fallback stay active.",620,380,418,90,kMuted,DT_WORDBREAK);
        DrawButton(dc,font_small_,UiRect{620,570,1038,598},s.smith_anvil_mode_a3==1?L"ANVIL CONTACT: REAL ANVIL":s.smith_anvil_mode_a3==2?L"ANVIL CONTACT: LEARNED (EXPERIMENT)":L"ANVIL CONTACT: OFF",s.smith_anvil_mode_a3?kAccent:kPanel2);
#elif defined(KCD2VR_MENU_CLICK_CRAFTING67_SMITH)
        if(PtIn(UiRect{620,570,1038,598},x,y)){s.smith_anvil_mode_a3=s.smith_anvil_mode_a3==1?2:s.smith_anvil_mode_a3==2?0:1;ApplyState(s);save();MarkDirty();return;} // A3 REAL -> LEARNED -> OFF
        for(int mode=0;mode<kcd2vr::crafting_menu_h18::kModeCount;++mode)if(PtIn(U49(kcd2vr::crafting_menu_h18::kChoices[1][mode]),x,y)){
            kcd2vr::crafting_menu_h18::select(s,kcd2vr::minigame_hands45::Smithing,mode);
            ApplyState(s);save();MarkDirty();return;
        }
#elif defined(KCD2VR_MENU_CLICK_KSMITHAUTOHOLD36)
if(PtIn(kSmithAutoHold36,x,y)){s.smith_auto_hold36=!s.smith_auto_hold36;ApplyState(s);save();diagnostic_trace::Write(s.smith_auto_hold36?
            "C46-ACTIONS VrSmithingAutoHold=ON (smith hand gestures enabled in semi-passenger mode)":
            "C46-ACTIONS VrSmithingAutoHold=OFF (smith native button controls)");MarkDirty();return;}
#elif defined(KCD2VR_MENU_DRAW_KSMITHAUTOHOLD36)
        DrawButton(dc,font_small_,kSmithAutoHold36,s.smith_auto_hold36?L"SMITHING: HAND GESTURES IN SEMI MODE":L"SMITHING: NATIVE BUTTON CONTROLS",s.smith_auto_hold36?kAccent:kPanel2);
#endif
