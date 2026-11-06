void SetSmoothBody51(bool on) noexcept {
    auto previous=g_frame_control51.load(std::memory_order_acquire);
    while(bool(previous&1)!=on){
        const auto next=((previous>>1)+1)*2+unsigned(on);
        if(g_frame_control51.compare_exchange_weak(previous,next,std::memory_order_acq_rel))break;
    }
    diagnostic_trace::Write(on?"C51R1 SMOOTH_MOVEMENT ON shared native frame; tracked head/hands direct":"C51R1 SMOOTH_MOVEMENT OFF native gait/crouch frame; tracked head/hands direct");
}
bool SmoothBody51() noexcept {return (g_frame_control51.load(std::memory_order_acquire)&1)!=0;}
// R98: 0 HMD HEIGHT, 1 NATIVE HEIGHT, 2 HMD + NATIVE (RIDE/CROUCH). One
// atomic token: a completed pose can never mix two modes.
static constexpr std::uint64_t kBodyHeightBitsR98=kNativeBodyHeightBit54|kCombinedBodyHeightBitR98;
static int BodyHeightModeFromControlR98(std::uint64_t c) noexcept {
    return (c&kNativeBodyHeightBit54)?1:(c&kCombinedBodyHeightBitR98)?2:0;
}
void SetBodyHeightModeR98(int mode) noexcept {
    mode=mode==1||mode==2?mode:0;
    const auto bits=mode==1?kNativeBodyHeightBit54:mode==2?kCombinedBodyHeightBitR98:std::uint64_t(0);
    auto old=g_frame_control51.load(std::memory_order_acquire);
    for(;;){
        if((old&kBodyHeightBitsR98)==bits)return;
        const auto generation=(old&~kBodyHeightBitsR98)+2;
        if(g_frame_control51.compare_exchange_weak(old,generation|bits,std::memory_order_acq_rel))break;
    }
    diagnostic_trace::Write(mode==1?"C54 HMD_BODY_HEIGHT OFF native pelvis Z including button crouch (R98 mode=NATIVE HEIGHT)":
        mode==2?"R98 BODY_HEIGHT mode=HMD+NATIVE HMD height and hands; native height while crouched or riding (blend 0.25 s)":
        "C54 HMD_BODY_HEIGHT ON shared root-relative body/head/hands; physical crouch (R98 mode=HMD HEIGHT)");
}
int BodyHeightModeR98() noexcept {return BodyHeightModeFromControlR98(g_frame_control51.load(std::memory_order_acquire));}
void SetHmdBodyHeight54(bool on) noexcept {SetBodyHeightModeR98(on?0:1);}
bool HmdBodyHeight54() noexcept {return (g_frame_control51.load(std::memory_order_acquire)&kNativeBodyHeightBit54)==0;}
void SetNativeCrouchR93(bool on) noexcept {
    if(kcd2vr::native_crouch_r93::enabled.exchange(on,std::memory_order_acq_rel)==on)return;
    kcd2vr::native_crouch_r93::invalidate();
    g_frame_control51.fetch_add(2,std::memory_order_acq_rel);
}
bool NativeCrouchR93() noexcept {return kcd2vr::native_crouch_r93::enabled.load(std::memory_order_acquire);}
void SetHeadRaiseMax59c9(float cm) noexcept {g_head_raise_max59c9.store(std::isfinite(cm)?std::clamp(cm,0.f,40.f)/100.f:.18f,std::memory_order_relaxed);} // R59.16C9C
void SetAutoPalm59c8(bool on) noexcept {g_auto_palm59c8.store(on,std::memory_order_relaxed);} // R59.16C8
void SetWristFromGrip59c7f(float cm) noexcept {g_wrist_from_grip59c7f.store(std::isfinite(cm)?std::clamp(cm,-15.f,15.f)/100.f:0.f,std::memory_order_relaxed);}
