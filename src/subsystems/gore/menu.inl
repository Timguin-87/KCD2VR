// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_CLICK_KDECAPITATION59C7E)
if(PtIn(kDecapitation59c7e,x,y)){ CycleDecap44(s); ApplyState(s); save(); diagnostic_trace::Write(DecapTrace46(s,"C46-IMMERSION")); return; }
#elif defined(KCD2VR_MENU_DRAW_KDECAPITATION59C7E)
        DrawButton(dc,font_small_,kDecapitation59c7e,DecapLabel44(s),(s.decapitation||s.dismemberment_d80)?kAccent:kPanel2); // C44
#elif defined(KCD2VR_MENU_CLICK_KBLOODYFIGHTS49)
if(PtIn(kBloodyFights49,x,y)){s.bloody_fights49=!s.bloody_fights49;ApplyState(s);save();diagnostic_trace::Write(s.bloody_fights49?"C49-IMMERSION VrBloodyFights=ON":"C49-IMMERSION VrBloodyFights=OFF");MarkDirty();return;}
#elif defined(KCD2VR_MENU_DRAW_KBLOODYFIGHTS49)
        DrawButton(dc,font_small_,kBloodyFights49,s.bloody_fights49?L"[X] BLOODY FIGHTS (2x + GUSH)":L"[ ] BLOODY FIGHTS (2x + GUSH)",s.bloody_fights49?kAccent:kPanel2); // C49
#elif defined(KCD2VR_MENU_CLICK_KIXDECAP)
if(PtIn(kIxDecap,x,y)){ CycleDecap44(s); done(DecapTrace46(s,"C46-EXPERIMENTS")); return; }
#elif defined(KCD2VR_MENU_DRAW_KIXDECAP)
        DrawButton(dc,font_small_,kIxDecap,DecapLabel44(s),(s.decapitation||s.dismemberment_d80)?kAccent:kPanel2); // C44
#elif defined(KCD2VR_MENU_CLICK_KIXIMPALE32)
if(PtIn(kIxImpale32,x,y)){ s.impale_hang=(s.impale_hang+1)%4; char m[96]{}; std::snprintf(m,sizeof(m),"C32-EXPERIMENTS VrImpaleHang=%d",s.impale_hang); done(m); return; }
#elif defined(KCD2VR_MENU_DRAW_KIXIMPALE32)
        DrawButton(dc,font_small_,kIxImpale32,s.impale_hang==0?L"IMPALE: LEGACY LATCH (no hanging body)":L"IMPALE: THE DEAD BODY HANGS ON THE BLADE (weight: COUNTER STAB page)",s.impale_hang?kAccent:kPanel2); // C32
#elif defined(KCD2VR_MENU_CLICK_KCM49IMPALEKILL)
if(PtIn(kCm49ImpaleKill,x,y)){ s.impale_hang=combat_menu49::impaleToggle(s.impale_hang,s.impale_hang_on45); ApplyState(s); save(); char m[96]{}; std::snprintf(m,sizeof(m),"C45-EXPERIMENTS VrImpaleHang=%d (IMPALE ON KILL STAB %s)",s.impale_hang,s.impale_hang?"ON":"OFF"); diagnostic_trace::Write(m); MarkDirty(); return; }
#elif defined(KCD2VR_MENU_DRAW_KCM49IMPALEKILL)
        DrawButton(dc,font_small_,kCm49ImpaleKill,s.impale_hang?L"IMPALE ON KILL STAB: ON":L"IMPALE ON KILL STAB: OFF",s.impale_hang?kAccent:kPanel2); // C45: VrImpaleHang 0 / last non-zero
#elif defined(KCD2VR_MENU_CLICK_KCM49DECAPKILL)
if(PtIn(kCm49DecapKill,x,y)){ CycleDecap44(s); ApplyState(s); save(); diagnostic_trace::Write(DecapTrace46(s,"C46-COMBAT")); MarkDirty(); return; }
#elif defined(KCD2VR_MENU_DRAW_KCM49DECAPKILL)
        DrawButton(dc,font_small_,kCm49DecapKill,DecapLabel44(s),(s.decapitation||s.dismemberment_d80)?kAccent:kPanel2); // C45/C46: same cycle as IMMERSION
#endif
