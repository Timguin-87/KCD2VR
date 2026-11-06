#pragma once
// Pure presentation/intent math for the exact XR34 owner. No engine access,
// clock mutation, inventory state or model-axis guesses live in this file.
// Included after Passenger by crossbow_native_xr34.h, inside its namespace.
inline bool wristValidD78(const Wrist& w) noexcept {
    double n=0;for(float q:w.q){if(!std::isfinite(q))return false;n+=double(q)*q;}
    for(float p:w.p)if(!std::isfinite(p))return false;
    return n>.99&&n<1.01;
}
inline Wrist composeWristD78(const Wrist& a,const Wrist& b) noexcept {
    const double ax=a.q[0],ay=a.q[1],az=a.q[2],aw=a.q[3];
    const double bx=b.q[0],by=b.q[1],bz=b.q[2],bw=b.q[3];
    const float inverse[4]{-a.q[0],-a.q[1],-a.q[2],a.q[3]};
    const auto p=rotateInverse(inverse,{b.p[0],b.p[1],b.p[2]});
    return {{float(aw*bx+ax*bw+ay*bz-az*by),float(aw*by-ax*bz+ay*bw+az*bx),
        float(aw*bz+ax*by-ay*bx+az*bw),float(aw*bw-ax*bx-ay*by-az*bz)},
        {float(a.p[0]+p.x),float(a.p[1]+p.y),float(a.p[2]+p.z)}};
}
inline Wrist inverseWristD78(const Wrist& w) noexcept {
    const auto p=rotateInverse(w.q,{-w.p[0],-w.p[1],-w.p[2]});
    return {{-w.q[0],-w.q[1],-w.q[2],w.q[3]},{float(p.x),float(p.y),float(p.z)}};
}
inline bool sameMechanismClockD78(const Clock& a,const Clock& b) noexcept {
    return a.qualified&&b.qualified&&a.entry&&a.entry==b.entry&&a.segment==b.segment&&
        a.normalized==b.normalized&&a.activeTime==b.activeTime&&a.playbackScale==b.playbackScale&&
        a.controllerScale==b.controllerScale&&a.characterScale==b.characterScale;
}
// R98 xbow: the class 14 string witness is read at the weapon skinning, after
// the native update. On every frame the hand moved the clock, the live clock is
// already past the passenger clock there (D86 NATIVE_CLOCK_OR_OWNER, e.g.
// 00:33:54.481 speed=92.7), so there was a string pose only every second frame
// and the hand/lever jumped between the string and the free hand (flicker).
// The previous frame's witness of the SAME clip entry/segment/scales, never a
// newer one, keeps the hand on the string for presentation. Stroke credit and
// native events still need the exact clock.
inline bool laggingStringClockR98(const Clock& witness,const Clock& now) noexcept {
    return witness.qualified&&now.qualified&&witness.entry&&witness.entry==now.entry&&witness.segment==now.segment&&
        witness.playbackScale==now.playbackScale&&witness.controllerScale==now.controllerScale&&
        witness.characterScale==now.characterScale&&std::isfinite(witness.normalized)&&std::isfinite(now.normalized)&&
        witness.normalized<=now.normalized&&now.normalized-witness.normalized<=.05f;
}
inline Vec intentHandD78(const Source& s,int side) noexcept {
    // Legacy non-D78 producers retain their former contract. Both production
    // instant producers refuse missing raw intent before publishing a Source.
    return s.intentKnownD78?s.intentHandD78[side]:s.hand[side];
}
inline bool reloadContactDomainD78(const HandConstraintD78& c,const Spatial& binding,Vec intent) noexcept {
    if(!finite(intent)||!finite(c.start)||!finite(c.axis)||std::abs(length(c.axis)-1.)>1e-4||
       !std::isfinite(c.travel)||c.travel<.03||c.travel>.5)return false;
    const Vec delta=intent-c.start;const double axial=dot(delta,c.axis);
    if(length(delta-scale(c.axis,axial))>.12||axial<-.12||axial>c.travel+.12)return false;
    if(c.mounted){
        const auto& handle=binding.toolWristD78;
        if(!binding.toolMechanismD72||!wristValidD78(handle)||
           length(intent-Vec{handle.p[0],handle.p[1],handle.p[2]})>.35)return false;
    }else{
        if(!binding.stringMechanismD78||!finite(binding.stringCurrentD78)||!finite(c.stringGripD78))return false;
        const Vec pin{binding.stringCurrentD78.x+c.stringGripD78.x,binding.stringCurrentD78.y+c.stringGripD78.y,
            binding.stringCurrentD78.z+c.stringGripD78.z};
        if(length(intent-pin)>.35)return false;
    }
    return true;
}
inline bool boltFrameAlignedD78(const Vec (&actual)[3],const Vec (&target)[3],bool& aligned) noexcept {
    aligned=false;
    for(const auto* frame:{actual,target}){
        for(int i=0;i<3;++i)if(!finite(frame[i])||std::abs(length(frame[i])-1.)>1e-4)return false;
        if(std::abs(dot(frame[0],frame[1]))>1e-4||std::abs(dot(frame[0],frame[2]))>1e-4||
           std::abs(dot(frame[1],frame[2]))>1e-4)return false;
        const Vec cross{frame[0].y*frame[1].z-frame[0].z*frame[1].y,
            frame[0].z*frame[1].x-frame[0].x*frame[1].z,frame[0].x*frame[1].y-frame[0].y*frame[1].x};
        if(dot(cross,frame[2])<.9999)return false;
    }
    // Compare complete native helper frames, not an assumed bolt mesh axis.
    // 35 degrees is an explicit placement tolerance; it is not asset geometry.
    const double cosine=(dot(actual[0],target[0])+dot(actual[1],target[1])+dot(actual[2],target[2])-1.)*.5;
    aligned=std::clamp(cosine,-1.,1.)>=.8191520442889918;return true;
}
inline bool projectReloadWristD78(const Passenger& p,const Wrist (&raw)[2],const bool (&valid)[2],
    std::uint64_t sample,std::uint64_t tick,std::uint64_t tracking,double grip,Wrist& out) noexcept {
    out={};const int hand=p.workingHand,primary=p.frame.primary;const auto& c=p.constraintD78;
    if(!p.valid||p.bootstrap||!handDrivenD81(p.frame)||!p.frame.intentKnownD78||!c.active||
       hand<0||hand>1||primary!=1-hand||!valid[0]||!valid[1]||!wristValidD78(raw[0])||!wristValidD78(raw[1])||
       !wristValidD78(p.frame.primaryGripD72)||!sample||sample<p.frame.sample||tick<p.frame.tick||
       tick-p.frame.tick>80||tracking!=p.frame.tracking||!std::isfinite(grip)||grip<.25||
       !finite(c.start)||!finite(c.axis)||std::abs(length(c.axis)-1.)>1e-4||
       !std::isfinite(c.travel)||c.travel<.03||c.travel>.5||!std::isfinite(c.progress)||c.progress<0||c.progress>1)return false;
    const Wrist weapon=composeWristD78(raw[primary],inverseWristD78(p.frame.primaryGripD72));
    if(!wristValidD78(weapon))return false;
    Wrist local=composeWristD78(inverseWristD78(weapon),raw[hand]);
    const Vec intent{local.p[0],local.p[1],local.p[2]};
    if(!reloadContactDomainD78(c,p.binding,intent))return false;
    if(c.mounted){
        // This pose is read from the untouched native weapon skinning input,
        // joined to THIS clock, binding and lease. It is never the placed prop.
        if(p.frame.classId!=15||!p.binding.toolMechanismD72||!p.binding.toolMountedPoseD78||
           !sameMechanismClockD78(p.binding.mechanismClockD78,p.clock)||
           !p.binding.mechanismTickD78||tick<p.binding.mechanismTickD78||tick-p.binding.mechanismTickD78>80||
           !wristValidD78(p.binding.toolWristD78))return false;
        local=p.binding.toolWristD78;
    }else{
        // Follow the actual current native string joint. Native animation time
        // need not map linearly to string distance, so a linear wrist target
        // alone cannot prove that the hand remains attached to the string.
        if(!p.binding.stringMechanismD78||!finite(p.binding.stringCurrentD78)||!finite(c.stringGripD78)||
           !(sameMechanismClockD78(p.binding.stringClockD78,p.clock)||laggingStringClockR98(p.binding.stringClockD78,p.clock))||!p.binding.stringTickD78||
           tick<p.binding.stringTickD78||tick-p.binding.stringTickD78>80)return false;
        const Vec pin{p.binding.stringCurrentD78.x+c.stringGripD78.x,p.binding.stringCurrentD78.y+c.stringGripD78.y,
            p.binding.stringCurrentD78.z+c.stringGripD78.z};
        local.p[0]=float(pin.x);local.p[1]=float(pin.y);local.p[2]=float(pin.z);
    }
    out=composeWristD78(weapon,local);return wristValidD78(out);
}
