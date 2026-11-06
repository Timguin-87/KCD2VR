// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_CLICK_KHMDHEIGHT54)
if(PtIn(kHmdHeight54,x,y)){kcd2vr::native_crouch_r93::select(s);ApplyState(s);save();MarkDirty();return;}
#elif defined(KCD2VR_MENU_DRAW_KHMDHEIGHT54)
        DrawButton(dc,font_small_,kHmdHeight54,kcd2vr::native_crouch_r93::label(s),s.body_height_mode98!=1?kAccent:kPanel2);
#elif defined(KCD2VR_MENU_CLICK_KIXBODYFOLLOW)
if(PtIn(kIxBodyFollow,x,y)){ s.vrik_body_follow=!s.vrik_body_follow; done(s.vrik_body_follow?"R59.16C10R8-EXPERIMENTS 6dofBody=ON":"R59.16C10R8-EXPERIMENTS 6dofBody=OFF"); return; }
#elif defined(KCD2VR_MENU_DRAW_KIXBODYFOLLOW)
        DrawButton(dc,font_small_,kIxBodyFollow,s.vrik_body_follow?L"[X] 6DOF BODY (F1)":L"[ ] 6DOF BODY (F1)",s.vrik_body_follow?kAccent:kPanel2);
#elif defined(KCD2VR_MENU_CLICK_KIXROOMSCALE)
if(PtIn(kIxRoomscale,x,y)){ const bool walkingD81=s.roomscale_mode==1||s.roomscale_mode==2,dodgeD81=s.roomscale_mode==2||s.roomscale_mode==3; s.roomscale_mode=walkingD81?(dodgeD81?3:0):(dodgeD81?2:1); done("D81 physical walking toggled; headset dodge preserved"); return; }
#elif defined(KCD2VR_MENU_DRAW_KIXROOMSCALE)
        { const bool walkingD81=s.roomscale_mode==1||s.roomscale_mode==2;DrawButton(dc,font_small_,kIxRoomscale,walkingD81?L"PHYSICAL WALKING: ON":L"PHYSICAL WALKING: OFF",walkingD81?kAccent:kPanel2); }
#elif defined(KCD2VR_MENU_DRAW_FINGERPOSITIONS40)

        pageTitle(L"FINGER POSITIONS",L"Free hands only; native held-object grips stay native");DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        const int h=fingerSide40_.load(),f=fingerDigit40_.load();const auto* a=s.fingers40[h][f];
        DrawButton(dc,font_small_,kFingerSide40,h?L"LEFT HAND (click to switch)":L"RIGHT HAND (click to switch)",kAccent);
        const wchar_t* names[]{L"THUMB",L"INDEX",L"MIDDLE",L"RING",L"LITTLE"};for(int i=0;i<5;++i)DrawButton(dc,font_tiny_?font_tiny_:font_small_,FingerButton40(i),names[i],i==f?kAccent:kPanel2);
        std::swprintf(v,128,L"%.0f%%",double(a[0])*100);DrawSlider(dc,font_small_,kFingerOpen40,L"Open-hand curl",a[0],v);
        std::swprintf(v,128,L"%.0f%%",double(a[1])*100);DrawSlider(dc,font_small_,kFingerClosed40,L"Closed-fist curl (reduce to avoid over-closing)",a[1],v);
        std::swprintf(v,128,L"%+.1f deg",double(a[2]));DrawSlider(dc,font_small_,kFingerLateral40,L"Lateral spread at finger base",(a[2]+20)/40,v);
        std::swprintf(v,128,L"%+.1f deg",double(a[3]));DrawSlider(dc,font_small_,kFingerProximal55,L"Closed-fist base-joint curl",(a[3]+30)/60,v);
        if(f==2){
            const float shift=s.finger_middle_shift_mm_h31[h];std::swprintf(v,128,L"%.1f mm",double(shift));DrawSlider(dc,font_small_,kFingerMiddleC54,L"Middle finger: sideways base movement",shift/20.f,v);
            const float medial=s.finger_middle_distal_medial_h31[h];std::swprintf(v,128,L"%+.1f deg",double(medial));DrawSlider(dc,font_small_,kFingerMiddleMedialH31,L"Last two joints: inward correction",(medial+20.f)/40.f,v);
        }else{
            const float middle=s.finger_middle_lateral_c54[h][f];std::swprintf(v,128,L"%+.1f deg",double(middle));DrawSlider(dc,font_small_,kFingerMiddleC54,L"Closed-fist middle-joint rotation",(middle+20)/40,v);
        }
        DrawButton(dc,font_small_,kFingerReset40,L"RESET THIS FINGER",kPanel2);
        Text(dc,font_tiny_?font_tiny_:font_small_,L"Closing only. Open hands and held-object grips stay unchanged.",620,650,418,18,kMuted,DT_LEFT);

#elif defined(KCD2VR_MENU_CLICK_FINGERPOSITIONS40)

        if(PtIn(kFingerSide40,x,y)){fingerSide40_.store(1-fingerSide40_.load());MarkDirty();return;}
        for(int i=0;i<5;++i)if(PtIn(FingerButton40(i),x,y)){fingerDigit40_.store(i);MarkDirty();return;}
        if(begin_drag(kFingerOpen40,DragControl::FingerOpen40)||begin_drag(kFingerClosed40,DragControl::FingerClosed40)||begin_drag(kFingerLateral40,DragControl::FingerLateral40)||begin_drag(kFingerProximal55,DragControl::FingerProximal55))return;
        if(fingerDigit40_.load()==2){if(begin_drag(kFingerMiddleC54,DragControl::FingerMiddleShiftH31)||begin_drag(kFingerMiddleMedialH31,DragControl::FingerMiddleMedialH31))return;}
        else if(begin_drag(kFingerMiddleC54,DragControl::FingerMiddleLateralC54))return;
        if(PtIn(kFingerReset40,x,y)){const int h=fingerSide40_.load(),f=fingerDigit40_.load();auto& v=s.fingers40[h][f];v[0]=0;v[1]=1;v[2]=0;v[3]=0;s.finger_middle_lateral_c54[h][f]=0;if(f==2){s.finger_middle_shift_mm_h31[h]=0;s.finger_middle_distal_medial_h31[h]=0;}ApplyState(s);save();MarkDirty();}return;

#endif
