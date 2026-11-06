void SetBowStringTrigger160(bool on) noexcept {g_bow_string_trigger160.store(on);diagnostic_trace::Write(on?"V66.160 BOW_STRING control=TRIGGER (Grip takes the arrow)":"V66.160 BOW_STRING control=GRIP");}
void SetBowReload146(bool manual,bool nativeGrab) noexcept {g_bow_manual_reload146.store(manual);g_bow_native_grab146.store(nativeGrab);
    char m[200]{};std::snprintf(m,sizeof(m),"V66.146R1 BOW_RELOAD manual=%d nativeGrabAtQuiver=%d quiver=RIGHT_SHOULDER rightGrip=TAKE_ARROW/DRAW releaseGrip=SHOOT",manual?1:0,nativeGrab?1:0);diagnostic_trace::Write(m);}
