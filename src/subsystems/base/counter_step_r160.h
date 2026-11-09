#pragma once
#include <cmath>
#include <cstdint>

// R160 (Miki R158 log 8 Nov 18:19-18:22, VrCombatNoStep=true: "henry is still dancing forward with the strikes"):
// the chained follow-ups (combat_*_fz*_az*_attackspecial_lngsw) carry Henry +41..+70 cm toward the enemy, and the
// R125 root-motion exit has never owned one frame in any recorded fight (0 R125 rows with F2+F3 on, while the R112
// alignment veto vetoed every frame of the same attacks). this is the pure decision of the counter-step that keeps
// Henry where his attack sequence started: while his own ordinary slash/thrust (lease = the R112 veto of this frame)
// is in phase 4/8/16, and through its phase 32 after-strike, the FORWARD part of his entity travel beyond a small
// budget is removed. sideways and backward travel (shoves, clinch back-off) stay native, the stick / roomscale
// (pushed stick) moves him freely and moves the anchor with him, and the moment the sequence ends (any other phase,
// kick, clinch, refusal, menu) nothing is written any more - no snap back, no latch, next attack arms again.
namespace kcd2vr::counter_step160 {
inline constexpr float kForwardBudgetM=0.05f;   // natural foot settling of an ordinary natk (0..2 cm) stays untouched
inline constexpr float kReleaseJumpM=1.5f;      // a jump this far from the anchor is a teleport/load, never corrected
inline constexpr std::uint64_t kLeaseMs=100;    // the R112 query runs every frame of phase 4/8/16 (C24 ~85 calls/s)
inline constexpr std::uint64_t kMaxWindowMs=6000; // safety: one sequence never holds longer (re-arms at once if still attacking)
inline constexpr float kStickActive=0.20f;      // left stick magnitude that counts as the player's own walking
// R164 (Miki R163 log 8 Nov 23:38-23:44: "we need enemies a little bit closer during the fights, I can't reach them" +
// "henry glitching, one frame visible, if he tries to do the step"): the native attack step is how the game closes the
// distance (C38 ATTACK_STEP forwardCm median 12, up to 76 per attack; Capon stood 1.3-2.0 m away) and R160 removed ALL
// of it (removedCm 35-100 per attack), so the blade fell short, and every clamped frame first showed the stepped pose
// for one frame (maxForwardCm 8-10 = the one-frame jump). now the step is native while it only brings Henry toward his
// current opponent: forward travel is removed only past the point where he would come closer than kStandOffM (centre to
// centre) to that opponent, so he still never runs into / past him ("dancing forward"). no known opponent position this
// frame = the R160 rule (5 cm) unchanged.
inline constexpr float kStandOffM=1.10f;

inline bool leasePhase(int p) noexcept {return p==4||p==8||p==16;}
inline bool holdPhase(int p) noexcept {return leasePhase(p)||p==32;}
inline bool leaseFresh(std::uint64_t lease,std::uint64_t now) noexcept {return lease&&now>=lease&&now-lease<=kLeaseMs;}
inline bool stickActive(float lx,float ly) noexcept {
    return std::isfinite(lx)&&std::isfinite(ly)&&lx*lx+ly*ly>kStickActive*kStickActive;
}

struct Frame {
    bool enabled=false;   // VrCombatNoStep ("Attack step veto") + the R112 hooks armed
    bool context=false;   // gameplay, no dialogue/menu/cutscene/scripted, no protected/paired body, no clinch input
    bool kick=false;      // a native kick is running (kicks may step, never touched)
    bool ordinary=false;  // Henry's live main action is an ordinary slash/thrust (kind 0/1)
    bool stick=false;     // the player is walking with the left stick (or roomscale follow / dodge injected it)
    int phase=0;          // Henry's combat state +0x38
    std::uint64_t now=0,lease=0;
    std::uintptr_t entity=0;
    float x=0,y=0;        // entity world position (planar)
    float fx=0,fy=0;      // entity forward (+Y column)
    bool target=false;    // R164: Henry's current opponent's planar position is known this frame
    float tx=0,ty=0;      // R164: that opponent (world, planar)
};
struct State {bool armed=false;std::uintptr_t entity=0;float ax=0,ay=0;std::uint64_t since=0;};
enum class Why : unsigned char {Idle,Armed,Held,Clamped,Stick,Released};
struct Result {Why why=Why::Idle;bool write=false;float x=0,y=0,removed=0,forward=0;};

// R164: how far forward from the anchor (along the unit facing ux,uy) Henry may travel: the 5 cm budget, or the native
// step up to kStandOffM from the opponent (the first point of the forward line that is kStandOffM from him). an
// opponent behind / beside the line (it never comes that close) = no limit from him: the budget only if he is behind,
// the whole step if the line passes him wider than kStandOffM.
inline float allowedForward(float ax,float ay,float ux,float uy,const Frame& f) noexcept {
    if(!f.target||!std::isfinite(f.tx)||!std::isfinite(f.ty))return kForwardBudgetM;
    const float ex=f.tx-ax,ey=f.ty-ay,along=ex*ux+ey*uy,perp2=ex*ex+ey*ey-along*along;
    if(!(along>0))return kForwardBudgetM;                 // opponent behind Henry's facing: nothing to close
    const float so2=kStandOffM*kStandOffM;
    if(!(perp2<so2))return 1e6f;                            // the forward line never comes within kStandOffM of him
    const float t=along-std::sqrt(so2-perp2);
    return t>kForwardBudgetM?t:kForwardBudgetM;
}
inline Result step(State& s,const Frame& f) noexcept {
    Result r{};
    const bool was=s.armed;
    const auto release=[&]() noexcept {s={};r.why=was?Why::Released:Why::Idle;return r;};
    if(!f.enabled||!f.context||f.kick||!f.ordinary||!f.entity||!holdPhase(f.phase)||
       !std::isfinite(f.x)||!std::isfinite(f.y))return release();
    const bool fresh=leaseFresh(f.lease,f.now);
    if(!s.armed||s.entity!=f.entity){
        // a sequence only starts inside a vetoed attack frame (phase 4/8/16 + this frame's R112 decision)
        if(!leasePhase(f.phase)||!fresh)return release();
        s.armed=true;s.entity=f.entity;s.ax=f.x;s.ay=f.y;s.since=f.now;r.why=Why::Armed;return r;
    }
    if(leasePhase(f.phase)&&!fresh)return release(); // the R112 veto refused this attack frame (kick/clinch/guard/hit/OFF)
    if(f.now<s.since||f.now-s.since>kMaxWindowMs)return release();
    if(f.stick){s.ax=f.x;s.ay=f.y;r.why=Why::Stick;return r;} // own walking: free, and the anchor walks with him
    const float dx=f.x-s.ax,dy=f.y-s.ay;
    if(!(dx*dx+dy*dy<=kReleaseJumpM*kReleaseJumpM))return release();
    const float n=std::sqrt(f.fx*f.fx+f.fy*f.fy);
    if(!std::isfinite(n)||n<.1f){r.why=Why::Held;return r;}
    const float ux=f.fx/n,uy=f.fy/n,forward=dx*ux+dy*uy;
    r.forward=forward;
    const float allowed=allowedForward(s.ax,s.ay,ux,uy,f);
    if(!(forward>allowed)){r.why=Why::Held;return r;}
    const float c=forward-allowed;
    r.why=Why::Clamped;r.write=true;r.x=f.x-c*ux;r.y=f.y-c*uy;r.removed=c;return r;
}
}
