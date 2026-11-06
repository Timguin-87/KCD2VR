// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_DRAW_VRACTIONS)

        pageTitle(L"ACTIONS",L"Authored interaction animation policy"); vrNav();
        DrawButton(dc,font_small_,kV4ActionNative,L"NATIVE ACTIVITIES",!s.first_person_activities?kAccent:kPanel2);
        DrawButton(dc,font_small_,kV4ActionFirst,L"1ST PERSON  [RESTART]",s.first_person_activities?kAccent:kPanel2);
        DrawButton(dc,font_small_,kActionOwner59c11,s.exp_action_owner==2?L"NATIVE GRASP: ON":s.exp_action_owner==1?L"NATIVE GRASP: OBSERVE":L"NATIVE GRASP: OFF",s.exp_action_owner==2?kAccent:kPanel2); // R59.16C10R11
        DrawButton(dc,font_small_,kActionHoldAt59c11,L"HOLD AT NATIVE ATTACH",kPanel2);
        Text(dc,font_small_,L"Grip holds; release drops; chest stows.\nNative grasp, VR wrist. F5 is not required.",620,166,418,50,kMuted,DT_CENTER|DT_WORDBREAK);
        static const wchar_t* activityNames59c7f[]{L"ALCHEMY HANDS",L"SMITHING HANDS",L"SHARPENING HANDS",L"READING HANDS",L"DIALOGUE HANDS"};
        for(unsigned i=0;i<5;++i){const bool enabled=i<4?s.activity_hands59c7f[i]:s.dialogue_hands59c7f;
            std::swprintf(v,128,L"%ls: %ls",activityNames59c7f[i],enabled?(i<2?L"SEMI WHEN SELECTED":L"TRACKED"):L"NATIVE");
            DrawButton(dc,font_small_,kActivityHands59c7f[i],v,enabled?kAccent:kPanel2);}

#define KCD2VR_MENU_DRAW_KSMITHAUTOHOLD36
#include "subsystems/smithing/menu.inl"
#undef KCD2VR_MENU_DRAW_KSMITHAUTOHOLD36

        DrawButton(dc,font_small_,kGrabMode61,s.grab_mode60==2?L"PICKUP: STAGED HOLD":L"PICKUP: NATIVE",s.grab_mode60?kAccent:kPanel2);
        DrawButton(dc,font_small_,kGrabReach43,s.grab_hand_stays43?L"GRAB: HAND STAYS ON CONTROLLER":L"GRAB: HAND FOLLOWS GAME REACH",s.grab_hand_stays43?kAccent:kPanel2); // C43

#define KCD2VR_MENU_DRAW_KDOORGESTURES46
#include "subsystems/doors/menu.inl"
#undef KCD2VR_MENU_DRAW_KDOORGESTURES46

        Text(dc,font_small_,L"Choose SEMI-PASSENGER in IMMERSION. Small hand motions pace work; Henry handles reaching. Stick and trigger controls remain available. Doors: squeeze, wait for contact, then move.",620,520,418,70,kMuted,DT_CENTER|DT_WORDBREAK);

#elif defined(KCD2VR_MENU_CLICK_VRACTIONS)

        for(unsigned i=0;i<5;++i)if(PtIn(kActivityHands59c7f[i],x,y)){
            if(i<4)s.activity_hands59c7f[i]=!s.activity_hands59c7f[i];else s.dialogue_hands59c7f=!s.dialogue_hands59c7f;
            ApplyState(s);save();MarkDirty();return;
        }

#define KCD2VR_MENU_CLICK_KSMITHAUTOHOLD36
#include "subsystems/smithing/menu.inl"
#undef KCD2VR_MENU_CLICK_KSMITHAUTOHOLD36


#define KCD2VR_MENU_CLICK_KDOORGESTURES46
#include "subsystems/doors/menu.inl"
#undef KCD2VR_MENU_CLICK_KDOORGESTURES46

        if(PtIn(kGrabMode61,x,y)){s.grab_mode60=s.grab_mode60==2?0:2;ApplyState(s);save();diagnostic_trace::Write(s.grab_mode60==2?"C62 PICKUP mode=STAGED_NATIVE_HOLD attach=NATIVE_EVENT":"C62 PICKUP mode=NATIVE f5PhysicalOverride=EXPLICIT_ARMED_ONLY");MarkDirty();return;}
        if(PtIn(kGrabReach43,x,y)){ s.grab_hand_stays43=!s.grab_hand_stays43; ApplyState(s); save(); diagnostic_trace::Write(s.grab_hand_stays43?"C43-ACTIONS VrGrabHandStaysOnController=ON (no reach blend toward the game's pick animation; doors unchanged)":"C43-ACTIONS VrGrabHandStaysOnController=OFF (R14 handover blend during the pick approach)"); MarkDirty(); return; }
        if(PtIn(kV4ActionNative,x,y)){ s.first_person_activities=false; ApplyState(s); save(); diagnostic_trace::Write("V64.3-ACTIONS Native selected; companion first-person activity PAK should be disabled on next restart"); return; }
        if(PtIn(kV4ActionFirst,x,y)){ s.first_person_activities=true; ApplyState(s); save(); diagnostic_trace::Write("V64.3-ACTIONS First Person selected; companion Mannequin PAK is restart-required"); return; }
        if(PtIn(kActionOwner59c11,x,y)){ s.exp_action_owner=(s.exp_action_owner+1)%3; ApplyState(s); save(); char m[120]{}; std::snprintf(m,sizeof(m),"R59.16C10R11-ACTIONS VrActionTimeOwner=%d",s.exp_action_owner); diagnostic_trace::Write(m); MarkDirty(); return; }
        if(PtIn(kActionHoldAt59c11,x,y)){ return; } // C10R13: native Attach owns this milestone, not a percentage slider
        return;

#endif
