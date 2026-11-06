// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_CLICK_KIXMAPREGION32)
if(PtIn(kIxMapRegion32,x,y)){done("M41 STATIC PAPER artwork follows current native location automatically");return;}
#elif defined(KCD2VR_MENU_DRAW_KIXMAPREGION32)
        DrawButton(dc,font_small_,kIxMapRegion32,L"STATIC PAPER: AUTO LOCATION",RGB(176,148,98));
#endif
