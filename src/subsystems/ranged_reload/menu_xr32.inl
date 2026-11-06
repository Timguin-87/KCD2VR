// Included only at the host menu scopes. Native renderer and input remain host-owned.
#if defined(KCD2VR_RANGED32_CLICK)
if(PtIn(UiRect{620,510,1038,548},x,y)){s.live_d39.arrowNativeTurnD48=!s.live_d39.arrowNativeTurnD48;ApplyState(s);save();MarkDirty();return;}
if(PtIn(UiRect{620,264,1038,302},x,y)){s.archer_trial_h41=!s.archer_trial_h41;ApplyState(s);save();MarkDirty();return;}
if(PtIn(kRangedBow32,x,y)){s.bow_manual_reload=!s.bow_manual_reload;ApplyState(s);save();MarkDirty();return;}
if(PtIn(kRangedZoom32,x,y)){s.crossbow_no_zoom32=!s.crossbow_no_zoom32;ApplyState(s);save();MarkDirty();return;}
#elif defined(KCD2VR_RANGED32_DRAW)
pageTitle(L"RANGED RELOAD / ZOOM",L"");
DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
Text(dc,font_small_,L"Aim-camera choice; reload and ammunition stay native",690,76,348,24,kMuted,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
DrawButton(dc,font_small_,kRangedBow32,s.bow_manual_reload?L"BOW RELOAD: QUIVER":L"BOW RELOAD: AUTO",s.bow_manual_reload?kAccent:kPanel2);
DrawButton(dc,font_small_,kRangedZoom32,s.crossbow_no_zoom32?L"CROSSBOW AIM ZOOM: OFF":L"CROSSBOW AIM ZOOM: NATIVE",s.crossbow_no_zoom32?kAccent:kPanel2);
DrawButton(dc,font_small_,UiRect{620,264,1038,302},s.archer_trial_h41?L"ARCHER STANCE: ON":L"ARCHER STANCE: OFF",s.archer_trial_h41?kAccent:kPanel2);
Text(dc,font_small_,L"Archer stance turns the chest toward the bow while drawing. Reload and weapon grip keep their current settings.",620,326,418,65,kMuted,DT_WORDBREAK);
Text(dc,font_small_,L"Use your right hand for the string/tool and shoulder bolt. Instant Reloads changes the native tail speed.",620,410,418,78,kMuted,DT_WORDBREAK);
DrawButton(dc,font_small_,UiRect{620,510,1038,548},s.live_d39.arrowNativeTurnD48?L"ARROW ORIENTATION: NATIVE":L"ARROW ORIENTATION: STABLE GRIP",s.live_d39.arrowNativeTurnD48?kAccent:kPanel2);
#endif
