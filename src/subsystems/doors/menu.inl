// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_CLICK_KDOORGESTURES46)
if(PtIn(kDoorGestures46,x,y)){return;} // CL-D: no switch - hand doors are always on
#elif defined(KCD2VR_MENU_DRAW_KDOORGESTURES46)
        DrawButton(dc,font_small_,kDoorGestures46,L"DOORS: YOUR HAND (ALWAYS ON)",kAccent);
#endif
