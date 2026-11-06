// Repeated lexical contribution. No native calls; existing state/apply/save owner.
#if defined(KCD2VR_H8_MENU_CLICK)
 if(PtIn(kPageBack,x,y)){GoToPage(MenuPage::Experiments,-1);return;}
 if(PtIn(U49(claude_h8::kPommel),x,y)){s.pommel_strike_h8=!s.pommel_strike_h8;ApplyState(s);save();MarkDirty();return;}
 if(PtIn(U49(claude_h8::kKickDamage),x,y)){MarkDirty();return;} // Contact probe has no damage toggle.
 if(PtIn(U49(claude_h8::kKickSetup),x,y)){GoToPage(MenuPage::Kicks26,+1);return;}
 if(PtIn(U49(claude_h8::kImpale),x,y)){s.impale_main_probe_h8=!s.impale_main_probe_h8;ApplyState(s);save();MarkDirty();return;}
 if(begin_drag(U49(claude_h8::kAssist),DragControl::KickAssistH8))return;
#elif defined(KCD2VR_H8_MENU_DRAW)
 pageTitle(L"EXPERIMENTS",L"Pommel, kick and impalement tests - changes saved immediately");vrNav();
 DrawButton(dc,font_small_,kPageBack,L"< BACK",kPanel2);
 DrawButton(dc,font_small_,U49(claude_h8::kPommel),s.pommel_strike_h8?L"[X] FORWARD POMMEL GESTURE":L"[ ] FORWARD POMMEL GESTURE",s.pommel_strike_h8?kAccent:kPanel2);
 DrawButton(dc,font_small_,U49(claude_h8::kKickDamage),L"FOOT CONTACT PROBE: LOG ONLY (no damage)",kPanel2);
 std::swprintf(v,128,s.kick_assist_h8?L"%d cm":L"OFF",s.kick_assist_h8);
 DrawSlider(dc,font_small_,U49(claude_h8::kAssist),L"Extra kick reach beyond visible foot",s.kick_assist_h8/30.f,v);
 DrawButton(dc,font_small_,U49(claude_h8::kKickSetup),L"KICK STYLE / LEG SETTINGS >",kPanel2);
 DrawButton(dc,font_small_,U49(claude_h8::kImpale),s.impale_main_probe_h8?L"[X] SAME-BODY IMPALE DIAGNOSTIC":L"[ ] SAME-BODY IMPALE DIAGNOSTIC",s.impale_main_probe_h8?kAccent:kPanel2);
 Text(dc,font_small_,L"Pommel: deliberate forward, hilt-first push; the game decides its contextual attack. GAME KICK and VR LEG, GAME RULES use the native kick action. The foot-contact probe is observation only; extra reach changes only its diagnostic query. Impale diagnostic reads the exact current hit body. It does not attach or drive a new corpse part. None of these switches enables full physics combat or thrown-weapon damage.",claude_h8::kHelp.l,claude_h8::kHelp.t,claude_h8::kHelp.r-claude_h8::kHelp.l,claude_h8::kHelp.b-claude_h8::kHelp.t,kMuted,DT_LEFT|DT_WORDBREAK);
#endif
