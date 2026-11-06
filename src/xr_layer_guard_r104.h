#pragma once
// R104/R106 (SteamVR: "the VR menu shows, but no HUD, no laser, nothing"):
// VDXR accepts composition layers that SteamVR refuses. SteamVR validates every layer of xrEndFrame (pose quaternion
// unit length, finite position, size > 0, sub-image rect inside the swapchain, swapchain handle, space) and refuses
// the WHOLE frame when one layer is bad - the compositor then keeps showing the last frame it accepted, which is a
// frame without that overlay. So the overlays never appear and the picture runs slower.
// This header is runtime independent (pure math + a small state machine, host-testable):
//  1. fixQuad/fixCylinder: repair what can be repaired (normalize the quaternion, clamp the rect into the swapchain,
//     give a degenerate size a minimum) and drop only what can't (NaN pose/size, null swapchain/space).
//  2. Quarantine (R106: only for LAYER error codes of xrEndFrame - layer invalid, layer limit, swapchain rect, pose
//     invalid, validation failure; time/session errors blame nobody; the frame is never sent twice): the overlay
//     kinds of a refused frame sit out shortly and come back one at a time; the kind whose return makes the next
//     frame fail sits out longer (10 s, 20 s, .. max 60 s) and is then tried again. Nothing is off for the session.
#include <cmath>
#include <cstdint>

namespace xr_layer_guard_r104 {

enum Kind : unsigned { FlatGame=0, FlatLaser, FlatCursor, Dice, TestLaser, HandLaser, Hud, Menu, MenuLaser, Count };

inline const char* name(unsigned k) noexcept {
    static const char* const n[Count]={"FLAT_GAME","FLAT_LASER","FLAT_CURSOR","DICE","TEST_LASER","HAND_LASER","HUD","MENU","MENU_LASER"};
    return k<Count?n[k]:"?";
}

enum Fix : unsigned { FixNone=0, FixQuat=1u, FixRect=2u, FixSize=4u };
enum Drop : unsigned { DropNone=0, DropNoSwapchain, DropEmptySwapchain, DropPose, DropSize, DropNoSpace };

inline const char* dropName(unsigned d) noexcept {
    switch(d){case DropNoSwapchain:return "NO_SWAPCHAIN";case DropEmptySwapchain:return "EMPTY_SWAPCHAIN";
              case DropNoSpace:return "NO_SPACE";case DropPose:return "POSE_INVALID";case DropSize:return "SIZE_NOT_FINITE";default:return "NONE";}
}

inline bool finite3(float a,float b,float c) noexcept { return std::isfinite(a)&&std::isfinite(b)&&std::isfinite(c); }

// Returns DropNone when usable; *fixes gets the repairs made.
template<class Pose>
inline unsigned fixPose(Pose& p,unsigned* fixes) noexcept {
    auto& q=p.orientation;
    if(!finite3(q.x,q.y,q.z)||!std::isfinite(q.w)||!finite3(p.position.x,p.position.y,p.position.z))return DropPose;
    const double n2=double(q.x)*q.x+double(q.y)*q.y+double(q.z)*q.z+double(q.w)*q.w;
    if(!(n2>1e-8f))return DropPose;
    // SteamVR checks unit length (spec: XR_ERROR_POSE_INVALID). Always renormalize; only count a visible change.
    const double inv=1.0/std::sqrt(n2);
    if(std::fabs(n2-1.0f)>1e-5f&&fixes)*fixes|=FixQuat;
    q.x=float(q.x*inv);q.y=float(q.y*inv);q.z=float(q.z*inv);q.w=float(q.w*inv);
    return DropNone;
}

template<class SubImage>
inline unsigned fixRect(SubImage& s,std::int32_t swW,std::int32_t swH,unsigned* fixes) noexcept {
    if(!s.swapchain)return DropNoSwapchain;
    if(swW<=0||swH<=0)return DropEmptySwapchain;
    auto& o=s.imageRect.offset;auto& e=s.imageRect.extent;
    const std::int32_t ox=o.x<0?0:(o.x>=swW?swW-1:o.x),oy=o.y<0?0:(o.y>=swH?swH-1:o.y);
    std::int32_t ex=e.width,ey=e.height;
    if(ex<1)ex=1;
    if(ey<1)ey=1;
    if(ex>swW-ox)ex=swW-ox;
    if(ey>swH-oy)ey=swH-oy;
    if(ox!=o.x||oy!=o.y||ex!=e.width||ey!=e.height){if(fixes)*fixes|=FixRect;o.x=ox;o.y=oy;e.width=ex;e.height=ey;}
    if(s.imageArrayIndex!=0){if(fixes)*fixes|=FixRect;s.imageArrayIndex=0;}
    return DropNone;
}

// Quad: XrCompositionLayerQuad (pose, size, subImage). minSize keeps a 1.5 mm laser a laser.
template<class Quad>
inline unsigned fixQuad(Quad& l,std::int32_t swW,std::int32_t swH,unsigned* fixes) noexcept {
    if(!l.space)return DropNoSpace;
    if(unsigned d=fixRect(l.subImage,swW,swH,fixes))return d;
    if(unsigned d=fixPose(l.pose,fixes))return d;
    if(!std::isfinite(l.size.width)||!std::isfinite(l.size.height))return DropSize;
    constexpr float minSize=0.0005f;
    if(l.size.width<minSize){l.size.width=minSize;if(fixes)*fixes|=FixSize;}
    if(l.size.height<minSize){l.size.height=minSize;if(fixes)*fixes|=FixSize;}
    return DropNone;
}

template<class Cyl>
inline unsigned fixCylinder(Cyl& l,std::int32_t swW,std::int32_t swH,unsigned* fixes) noexcept {
    if(!l.space)return DropNoSpace;
    if(unsigned d=fixRect(l.subImage,swW,swH,fixes))return d;
    if(unsigned d=fixPose(l.pose,fixes))return d;
    if(!std::isfinite(l.radius)||!std::isfinite(l.centralAngle)||!std::isfinite(l.aspectRatio))return DropSize;
    if(l.radius<0.05f){l.radius=0.05f;if(fixes)*fixes|=FixSize;}
    if(l.centralAngle<0.01f){l.centralAngle=0.01f;if(fixes)*fixes|=FixSize;}
    if(l.centralAngle>6.2831853f){l.centralAngle=6.2831853f;if(fixes)*fixes|=FixSize;}
    if(l.aspectRatio<0.01f){l.aspectRatio=0.01f;if(fixes)*fixes|=FixSize;}
    return DropNone;
}

// Only these xrEndFrame results say "a layer is wrong" (openxr.h: XR_ERROR_VALIDATION_FAILURE -1,
// XR_ERROR_LAYER_INVALID -23, XR_ERROR_LAYER_LIMIT_EXCEEDED -24, XR_ERROR_SWAPCHAIN_RECT_INVALID -25,
// XR_ERROR_POSE_INVALID -39). R108: R106 had -42/-43/-44 here (wrong numbers, copied from memory) - a real
// LAYER_INVALID (-23) would never have started the sit-out. -1 stays: SteamVR reports some bad layer fields that way.
inline bool layerError(int code) noexcept { return code==-1||code==-23||code==-24||code==-25||code==-39; }

struct Quarantine {
    std::uint64_t until[Count]{};      // kind sits out while now < until
    std::uint64_t admittedAt[Count]{}; // when the kind last came back from a sit-out
    std::uint32_t strikes[Count]{};
    std::uint32_t lastBlamed=Count;

    // Call for every kind that wants to be submitted this frame.
    bool admit(unsigned k,std::uint64_t now) noexcept {
        if(k>=Count)return false;
        if(until[k]==0)return true;
        if(now<until[k])return false;
        until[k]=0;admittedAt[k]=now;return true;
    }
    // Call after xrEndFrame with the overlay kinds that were in the frame (mask) and whether the frame was
    // accepted. A refusal that is not a layer error (code) blames nobody. Returns the sit-out (ms) given to the
    // blamed kind, or 0.
    std::uint32_t onResult(unsigned mask,bool ok,std::uint64_t now,int code=-23) noexcept {
        lastBlamed=Count;
        if(ok){
            for(unsigned k=0;k<Count;++k)if((mask>>k)&1u){
                if(strikes[k]&&admittedAt[k]&&now>admittedAt[k]+15000u)strikes[k]=0; // stayed good 15 s: forgiven
            }
            return 0;
        }
        if(!mask||!layerError(code))return 0;
        // The kind that came back most recently (inside 600 ms) is the suspect.
        unsigned suspect=Count;std::uint64_t newest=0;
        for(unsigned k=0;k<Count;++k)if(((mask>>k)&1u)&&admittedAt[k]&&now>=admittedAt[k]&&now-admittedAt[k]<=600u&&admittedAt[k]>newest){newest=admittedAt[k];suspect=k;}
        unsigned single=Count,n=0;for(unsigned k=0;k<Count;++k)if((mask>>k)&1u){single=k;++n;}
        if(suspect==Count&&n==1)suspect=single;
        if(suspect!=Count){
            const std::uint32_t s=strikes[suspect]<3u?strikes[suspect]:3u;
            std::uint32_t ms=10000u<<s;if(ms>60000u)ms=60000u;
            ++strikes[suspect];until[suspect]=now+ms;admittedAt[suspect]=0;lastBlamed=suspect;
            return ms;
        }
        // Unknown culprit: everyone in the frame sits out, then they return one by one 300 ms apart.
        std::uint64_t at=now+1000u;
        for(unsigned k=0;k<Count;++k)if((mask>>k)&1u){until[k]=at;admittedAt[k]=0;at+=300u;}
        return 0;
    }
};

} // namespace xr_layer_guard_r104
