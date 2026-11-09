#pragma once
// R158 (Miki 8 Nov: "the 'pickable' things (instant pickup items) now sometimes float in the air if they do not
// 'insert' into my chest... so they need the same gravity rules as the 'non pickable' objects (scenery props)").
// why the two differ: a scenery prop (static_grab_r89.inl) is carried by the native velocity servo the whole hold,
// so its rigid body is simulated (awake) every frame and the release only writes the hand velocity (R116). the
// instant-held pickup item (staged_pickup596c2.inl, VrInstantInteractionsD72) is carried KINEMATIC since R153 when
// VrPropFirmHoldR116 is ON (R96 after 100 ms when OFF): SetPosRotScale every frame + a ZERO velocity write. its body
// never moves by itself, so the physics lets it fall asleep; the release writes the trail velocity only when the
// hand moved (a zero velocity never wakes a sleeping body) -> a gentle let-go stays in the air = "sometimes".
// on top, the native immediate pickup can leave the item hanging on a parent (R153 readback: the item placed at 2x
// its world position; CEntity+0x98 -> +0x18 is the audited GetParent 83A8F4 layout); a child entity stays where
// its parent keeps it.
// fix (once per drop, game thread, next native rhythm frame, static_grab_r89n::Tick): if the item hangs on a parent
// -> Lua DetachThis(1) (keep the world transform; the pose is put back if the readback moved), then Lua
// AwakePhysics(1) (the same call PushPropR94 already makes on PickableItems), then the hand velocity again (the
// scenery release writes the same). a stowed item is untouched (it is in the inventory). no latch: each drop is
// its own one-shot request, a request older than 0.5 s is skipped.
// pure parts here (host test tests/instant_drop_r158_test.cpp).
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cmath>
namespace kcd2vr::instant_drop_r158 {
inline constexpr std::uint64_t kMaxAgeMs=500; // a drop not serviced within 0.5 s is skipped (never retried)
inline constexpr double kHandSpeed=.05;       // m/s: below this the release carried no hand speed (gravity alone)
struct Request {std::uint32_t id=0;std::uintptr_t entity=0;double v[3]{};bool handVelocity=false;std::uint64_t tick=0;};
inline double speed(const Request& r) noexcept {return std::sqrt(r.v[0]*r.v[0]+r.v[1]*r.v[1]+r.v[2]*r.v[2]);}
// write the hand velocity again after the wake? only when the release had one (a still hand = gravity alone)
inline bool rewriteVelocity(const Request& r) noexcept {
    const double s=speed(r);return r.handVelocity&&std::isfinite(s)&&s>=kHandSpeed;
}
// one slot: there is only one instant-held item at a time, the latest drop wins. the caller holds PropLock596.
struct Slot {
    Request r{};bool pending=false;
    void post(const Request& x) noexcept {if(!x.id||!x.entity||!x.tick)return;r=x;pending=true;}
    // true = service it now; a request older than kMaxAgeMs (or from the future) is consumed and skipped
    bool take(std::uint64_t now,Request& out) noexcept {
        out={};if(!pending)return false;pending=false;out=r;r={};
        return now>=out.tick&&now-out.tick<=kMaxAgeMs;
    }
};
// Lua chunks: assert = the chunk answers false to the ASI (same style as prop_impact_sound_r124.h)
inline int detachChunk(char* out,std::size_t n,std::uint32_t id) noexcept {
    return std::snprintf(out,n,"local e=System and System.GetEntity and System.GetEntity(%u) "
        "assert(e and type(e.DetachThis)=='function' and pcall(e.DetachThis,e,1),'KCD2VR instant drop: DetachThis refused')",id);
}
inline int wakeChunk(char* out,std::size_t n,std::uint32_t id) noexcept {
    return std::snprintf(out,n,"local e=System and System.GetEntity and System.GetEntity(%u) "
        "assert(e and type(e.AwakePhysics)=='function' and pcall(e.AwakePhysics,e,1),'KCD2VR instant drop: AwakePhysics refused')",id);
}
}
