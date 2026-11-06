// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_DRAW_NPC_TUNING)
        pageTitle(L"NPC COMBAT SETTINGS",L"Native enemy tuning | Independent controls");
        DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        std::swprintf(v,128,L"%.0f%%",double(s.npc_difficulty_percent));
        DrawSlider(dc,font_small_,kNpcDifficulty,L"NPC difficulty: native to C8",s.npc_difficulty_percent*.01f,v);
        Text(dc,font_small_,L"0% keeps current native AI values. 100% uses the earlier C8 harder-AI reaction and guard-switch settings.",620,202,418,48,kMuted,DT_LEFT|DT_WORDBREAK);
        Text(dc,font_small_,s.npc_difficulty_percent<=0.f?L"Difficulty: native":s.npc_difficulty_ready?L"Difficulty: control ready":L"Difficulty: not active; native retained",620,256,418,30,kMuted,DT_LEFT|DT_WORDBREAK);
        std::swprintf(v,128,L"x%.2f",double(s.npc_defense_speed));
        DrawSlider(dc,font_small_,kNpcDefenseSpeed,L"NPC block / parry / dodge speed",(s.npc_defense_speed-1.f)*.5f,v);
        Text(dc,font_small_,L"1.00 retains current defense speed. Higher values accelerate selected NPC defenses. Henry's windows are unchanged.",620,351,418,54,kMuted,DT_LEFT|DT_WORDBREAK);
        Text(dc,font_small_,s.npc_defense_speed<=1.f?L"Defense speed: current":s.npc_defense_ready?L"Defense speed: control ready":L"Defense speed: not active; current speed retained",620,413,418,30,kMuted,DT_LEFT|DT_WORDBREAK);
        std::swprintf(v,128,L"%.0f%%",double(s.npc_unarmed_spacing_percent));
        DrawSlider(dc,font_small_,kNpcUnarmedSpacing,L"NPC unarmed preferred-distance scale",(s.npc_unarmed_spacing_percent-25.f)/75.f,v);
        Text(dc,font_small_,L"60% is the boxing default; 100% restores native distance. Native tactical distance overrides and attack reach stay unchanged.",620,516,418,54,kMuted,DT_LEFT|DT_WORDBREAK);
        if(s.npc_unarmed_spacing_percent>=100.f)std::swprintf(v,128,L"Unarmed distance: native");
        else if(s.npc_unarmed_spacing_ready)std::swprintf(v,128,L"Unarmed distance: control ready");
        else std::swprintf(v,128,L"Unarmed distance unavailable: %hs",s.npc_unarmed_spacing_failure);
        Text(dc,font_small_,v,620,576,418,48,kMuted,DT_LEFT|DT_WORDBREAK);

#elif defined(KCD2VR_MENU_CLICK_NPC_TUNING)
        if(begin_drag(kNpcDifficulty,DragControl::NpcDifficulty)||begin_drag(kNpcDefenseSpeed,DragControl::NpcDefenseSpeed)||begin_drag(kNpcUnarmedSpacing,DragControl::NpcUnarmedSpacing))return;
        return;

#elif defined(KCD2VR_MENU_DRAW_EXPERIMENTS2)
        pageTitle(L"COMBAT TUNING 2",L"Native combat settings");vrNav();
        DrawButton(dc,font_small_,kExpWallSlide,s.exp_wall_slide?L"[X] SLIDE ALONG WALLS":L"[ ] SLIDE ALONG WALLS",s.exp_wall_slide?kAccent:kPanel2);
        DrawButton(dc,font_small_,kExpAttackGate,s.exp_attack_require_opponent?L"[X] REQUIRE AN OPPONENT":L"[ ] REQUIRE AN OPPONENT",s.exp_attack_require_opponent?kAccent:kPanel2);
        std::swprintf(v,128,L"%.0f cm",double(s.exp_npc_proximity_radius36)*100.);DrawSlider(dc,font_small_,kExpNpcProximity36,L"Blade proximity observation",(s.exp_npc_proximity_radius36-.20f)/.40f,v);
        std::swprintf(v,128,L"%.1f m",double(s.exp_attack_reach_m));DrawSlider(dc,font_small_,kExpAttackReach,L"Opponent distance",(s.exp_attack_reach_m-1.f)/4.f,v);
        DrawButton(dc,font_small_,kExpImpale40,s.exp_impale40==1?L"IMPALE DISPLAY: ENERGY + ARMOUR":s.exp_impale40==2?L"IMPALE DISPLAY: REDUCED ARMOUR":L"IMPALE DISPLAY: OFF",s.exp_impale40?kAccent:kPanel2);
        DrawButton(dc,font_small_,kExpWeaponFeelMore59c14,L"WEAPON FEEL >",RGB(176,148,98));
#elif defined(KCD2VR_MENU_CLICK_EXPERIMENTS2)
        if(begin_drag(kExpNpcProximity36,DragControl::ExpNpcProximity36)||begin_drag(kExpAttackReach,DragControl::ExpAttackReach59c10))return;
        if(PtIn(kExpWallSlide,x,y)){s.exp_wall_slide=!s.exp_wall_slide;ApplyState(s);save();MarkDirty();return;}
        if(PtIn(kExpAttackGate,x,y)){s.exp_attack_require_opponent=!s.exp_attack_require_opponent;ApplyState(s);save();MarkDirty();return;}
        if(PtIn(kExpImpale40,x,y)){s.exp_impale40=(std::clamp(s.exp_impale40,0,2)+1)%3;ApplyState(s);save();MarkDirty();return;}
        if(PtIn(kExpWeaponFeelMore59c14,x,y)){GoToPage(MenuPage::WeaponFeel59c14,+1);return;}
        return;
#endif
