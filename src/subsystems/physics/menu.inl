// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_CLICK_KPRECISECONTACTS44)
if(PtIn(kPreciseContacts44,x,y)){const int m=(frik_pose_probe::HandsModeDs2()+1)%3; // DS2: NORMAL->PRECISE->PERFORMANCE
 s.live_d39.handsMode=m;s.precise_contacts44=(m==1);ApplyState(s);save();MarkDirty();
 diagnostic_trace::Write(m==0?"DS2 HANDS MODE: NORMAL":m==1?"DS2 HANDS MODE: PRECISE":"DS2 HANDS MODE: PERFORMANCE");return;}
#elif defined(KCD2VR_MENU_DRAW_KPRECISECONTACTS44)
        {const int hm=frik_pose_probe::HandsModeDs2(); // DS2: tri-state
        DrawButton(dc,font_small_,kPreciseContacts44,hm==0?L"[ ] HANDS: NORMAL (COARSE)":hm==1?L"[X] HANDS: PRECISE FINGERS":L"[X] HANDS: PERFORMANCE",hm?kAccent:kPanel2);} // C49: half row
#elif defined(KCD2VR_MENU_CLICK_KRAGDOLLPHYSICS59C10R7)
if(PtIn(kRagdollPhysics59c10r7,x,y)){ s.physical_f9_rung61=s.physical_f9_next_h6; ApplyState(s); save(); diagnostic_trace::Write("C61-IMMERSION F9 physical weapon mode changed"); MarkDirty(); return; }
#elif defined(KCD2VR_MENU_DRAW_KRAGDOLLPHYSICS59C10R7)
        DrawButton(dc,font_small_,kRagdollPhysics59c10r7,s.physical_f9_rung61==3?L"F9: CORPSE PUSH / TRACKED WEAPON":s.physical_f9_rung61==2?L"F9: PHYSICAL WEAPON / PROPS":s.physical_f9_rung61==1?L"F9: OBSERVE ONLY":L"F9: OFF",s.physical_f9_rung61?kAccent:kPanel2); // R59.16C10R7
#elif defined(KCD2VR_MENU_CLICK_KIXPHYSBODIES)
if(PtIn(kIxPhysBodies,x,y)){ s.phys_bodies=(s.phys_bodies+1)%3; done(s.phys_bodies==0?"R59.16C10R8-EXPERIMENTS physBodies=OFF":s.phys_bodies==1?"R59.16C10R8-EXPERIMENTS physBodies=FOLLOW":"R59.16C10R8-EXPERIMENTS physBodies=COLLIDE"); return; }
#elif defined(KCD2VR_MENU_DRAW_KIXPHYSBODIES)
        DrawButton(dc,font_small_,kIxPhysBodies,s.phys_bodies==0?L"PHYSICS PROXIES: OFF":s.phys_bodies==1?L"PHYSICS PROXIES: FOLLOW ONLY":L"PHYSICS PROXIES: COLLIDE UNAVAILABLE",s.phys_bodies==1?kAccent:kPanel2);
#elif defined(KCD2VR_MENU_CLICK_KIXRAGDOLL)
if(PtIn(kIxRagdoll,x,y)){ s.physical_f9_rung61=s.physical_f9_next_h6; done("C61 F9 PHYSICAL MODE change requested"); return; }
#elif defined(KCD2VR_MENU_DRAW_KIXRAGDOLL)
        DrawButton(dc,font_small_,kIxRagdoll,s.physical_f9_rung61==3?L"F9: CORPSE PUSH / TRACKED WEAPON":s.physical_f9_rung61==2?L"F9: PHYSICAL WEAPON / PROPS":s.physical_f9_rung61==1?L"F9: OBSERVE ONLY":L"F9: OFF",s.physical_f9_rung61?kAccent:kPanel2);
#elif defined(KCD2VR_MENU_CLICK_KIXRAGDOLLPUSH33)
if(PtIn(kIxRagdollPush33,x,y)){ s.ragdoll_push=s.ragdoll_push%3+1; char m[96]{}; std::snprintf(m,sizeof(m),"C33-EXPERIMENTS VrRagdollPush=%d",s.ragdoll_push); done(m); return; }
#elif defined(KCD2VR_MENU_DRAW_KIXRAGDOLLPUSH33)
        DrawButton(dc,font_small_,kIxRagdollPush33,s.ragdoll_push==1?L"RAGDOLL PUSH: LOW":s.ragdoll_push==3?L"RAGDOLL PUSH: STRONG":L"RAGDOLL PUSH: NORMAL",s.ragdoll_push!=2?kAccent:kPanel2); // C33
#elif defined(KCD2VR_MENU_DRAW_WEAPONFEEL59C14)
 // R59.16C10R14
        pageTitle(L"WEAPON FEEL",L"Changes apply immediately and are saved"); vrNav(); DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        std::swprintf(v,128,L"x%.2f",double(s.exp_weight_feel)); DrawSlider(dc,font_small_,kWfWeight59c14,L"Arm authority over the weapon (lower = heavier, more drag)",(s.exp_weight_feel-.2f)/1.8f,v);
        std::swprintf(v,128,L"%.2f",double(s.exp_fatigue_ramp)); DrawSlider(dc,font_small_,kWfFatigue59c14,L"Drag grows as stamina falls (0 = only below 15 %, 1 = x4 at empty)",s.exp_fatigue_ramp,v);
        std::swprintf(v,128,L"x%.2f",double(s.exp_pole_one_hand)); DrawSlider(dc,font_small_,kWfPole59c14,L"Halberd with ONE hand: authority fraction (two hands restore it)",(s.exp_pole_one_hand-.15f)/.85f,v);
        std::swprintf(v,128,L"x%.1f",double(s.exp_swing_stamina_scale)); DrawSlider(dc,font_small_,kWfStamina59c14,L"Stamina per stroke: x the native attack cost, scaled by your stroke's energy (0 = free)",s.exp_swing_stamina_scale/2.f,v);
        std::swprintf(v,128,L"%.2f",double(s.exp_strike_min_scale)); DrawSlider(dc,font_small_,kWfMinScale59c14,L"Rung 2: minimum stroke power to attack (energy scale 0..1)",s.exp_strike_min_scale,v);
        std::swprintf(v,128,L"%d ms",s.exp_strike_min_interval_ms); DrawSlider(dc,font_small_,kWfMinInterval59c14,L"Rung 2: minimum time between two native attacks",s.exp_strike_min_interval_ms/1000.f,v);
        DrawButton(dc,font_small_,kWfMinGate59c14,s.exp_strike_min_gate?L"[X] A STROKE UNDER THE MINIMUM POWER DOES NOT ATTACK":L"[ ] A STROKE UNDER THE MINIMUM POWER DOES NOT ATTACK (marked WEAK, still attacks)",s.exp_strike_min_gate?kAccent:kPanel2);
        DrawButton(dc,font_small_,kWfMasterGame59c14,s.exp_master_strike_game?L"[X] MASTER STRIKES: THE GAME DECIDES (no VR veto)":L"[ ] MASTER STRIKES: ONLY A THRUST FROM THE OPPOSITE SIDE (VR veto)",s.exp_master_strike_game?kAccent:kPanel2);
        DrawButton(dc,font_small_,kWfBowHide59c14,s.live_d39.hideBackWeaponsR121?L"STOWED BOW: HIDDEN (EXPERIMENTS GEAR 2: BACK WEAPONS ON)":s.exp_hide_stowed_bow==1?L"STOWED BOW: HIDDEN WHEN YOU LOOK UP":s.exp_hide_stowed_bow==2?L"STOWED BOW: ALWAYS HIDDEN ON THE BACK":L"STOWED BOW: NATIVE (visible, may clip when you look up)",s.exp_hide_stowed_bow?kAccent:kPanel2);
        DrawButton(dc,font_small_,kWfTwoHandAxis59c16,s.exp_two_hand_axis?L"[X] TWO HANDS: LATCH HILT / SHAFT WHILE GRIP HELD":L"[ ] TWO HANDS: FIXED SOCKET (old, right hand steers)",s.exp_two_hand_axis?kAccent:kPanel2); // R59.16C10R16
        Text(dc,font_small_,L"Weight: the game's own grab pose holds the weapon, the arm drives it with limited force/torque and it may now trail the hand up to 30 cm when heavy or tired. A halberd in one hand is a 10-pole stick; the left hand on the shaft gives the authority back. Stamina: the game's own weapon-raise cost, charged 1-4 times by energy and mass; live strength/agility/stamina now reach the weapon in every rung. Two hands: close the left grip at the hilt or shaft. The chosen local grasp stays latched while Grip is held; tracking and native paired actions retain their release rules.",620,552,418,80,kMuted,DT_CENTER|DT_WORDBREAK);
        std::swprintf(v,128,s.exp_weapon_stat_effect<=0.f?L"off":L"x%.2f",double(s.exp_weapon_stat_effect)); DrawSlider(dc,font_small_,kWfStatEffect59c17,L"RPG stats: strength = drive force, agility = follow speed (vs the weapon's requirements)",s.exp_weapon_stat_effect/1.5f,v); // R59.16C10R17
        DrawButton(dc,font_small_,kWfCombatFlow59c17,L"COMBAT FLOW  >  (guard, perfect block, master strike, combos, NPC reaction time)",RGB(176,148,98));
        DrawButton(dc,font_small_,kWfSim26,s.stat_sim26==1?L"SIMULATE HENRY: LOW LEVEL (str 6, agi 6, weapon skill 2) - feel only":s.stat_sim26==2?L"SIMULATE HENRY: MEDIUM (str 14, agi 14, skill 12) - feel only":s.stat_sim26==3?L"SIMULATE HENRY: HIGH (str 25, agi 25, skill 25) - feel only":L"SIMULATE HENRY: OFF (Henry's real stats and weapon skill)",s.stat_sim26?kAccent:kPanel2); // C26c

#elif defined(KCD2VR_MENU_CLICK_WEAPONFEEL59C14)
 // R59.16C10R14
        if(begin_drag(kWfWeight59c14,DragControl::ExpWeightFeel59c14)||begin_drag(kWfFatigue59c14,DragControl::ExpFatigueRamp59c14)||begin_drag(kWfPole59c14,DragControl::ExpPoleOneHand59c14)||
           begin_drag(kWfStamina59c14,DragControl::ExpSwingStamina59c14)||begin_drag(kWfMinScale59c14,DragControl::ExpStrikeMinScale59c14)||begin_drag(kWfMinInterval59c14,DragControl::ExpStrikeMinInterval59c14)) return;
        auto flip=[&](const UiRect& r,bool& v,const char* name)->bool{ if(!PtIn(r,x,y))return false; v=!v; ApplyState(s); save(); char m[96]{}; std::snprintf(m,sizeof(m),"R59.16C10R14-WEAPON-FEEL %s=%s",name,v?"ON":"OFF"); diagnostic_trace::Write(m); MarkDirty(); return true; };
        if(flip(kWfMinGate59c14,s.exp_strike_min_gate,"VrStrikeMinScaleGate")||flip(kWfMasterGame59c14,s.exp_master_strike_game,"VrMasterStrikeGameDecides")||
           flip(kWfTwoHandAxis59c16,s.exp_two_hand_axis,"VrTwoHandAxisGrip")) return;
        if(PtIn(kWfBowHide59c14,x,y)){ s.exp_hide_stowed_bow=(s.exp_hide_stowed_bow+1)%3; ApplyState(s); save(); char m[96]{}; std::snprintf(m,sizeof(m),"R59.16C10R14-WEAPON-FEEL VrHideStowedBow=%d",s.exp_hide_stowed_bow); diagnostic_trace::Write(m); MarkDirty(); return; }
        if(begin_drag(kWfStatEffect59c17,DragControl::ExpWeaponStatEffect59c17)) return; // R59.16C10R17
        if(PtIn(kWfCombatFlow59c17,x,y)){ GoToPage(MenuPage::CombatFlow59c17,+1); return; }
        if(PtIn(kWfSim26,x,y)){ s.stat_sim26=(s.stat_sim26+1)%4; ApplyState(s); save(); char m[96]{}; std::snprintf(m,sizeof(m),"C26-WEAPON-FEEL VrSimulateHenry=%d",s.stat_sim26); diagnostic_trace::Write(m); MarkDirty(); return; } // C26c
        return;

#endif
