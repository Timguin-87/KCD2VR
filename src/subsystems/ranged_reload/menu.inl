// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_CLICK_KV149MANUALBOWRELOAD)
if(PtIn(kV149ManualBowReload,x,y)){ GoToPage(MenuPage::RangedOptions32,+1); return; }
#elif defined(KCD2VR_MENU_DRAW_KV149MANUALBOWRELOAD)
        DrawButton(dc,font_small_,kV149ManualBowReload,L"RANGED RELOAD / ZOOM",kPanel2); // effective saved loading choice
#endif
