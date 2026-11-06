#pragma once
#include <cmath>
#include <cstdint>
#include "subsystems/roomscale/native_roomscale_d76.h"
#include "dialogue_membership19c2.h"
#include "dialogue_input_owner19c3.h"

namespace kcd2vr::dialogue_state59c10r2 {
enum class Camera : unsigned { Unknown, FirstPerson, Dialogue };
// Diagnostic evidence only. Native82EC48 combines selected-camera vf20 with
// manager+39, and only then updates Henry's FirstPersonView group via1351FEC.
// A selected FirstPerson enum alone therefore does not prove a hidden head.
// Do not promote these bytes into camera/body permission or write them.
struct HeadPresentation {
    bool known=false;
    std::uintptr_t manager=0,selected=0,selectedVft=0;
    unsigned char blendFromThirdPerson=0,forceVisible=0,bypassCompose=0,headHidden=0,groupFlags=0;
};
struct State {
    std::uint64_t tick=0;
    std::uintptr_t entity=0,instance=0;
    bool known=false,active=false;
    Camera camera=Camera::Unknown;
    dialogue_membership19c2::Result membership{};
    std::uintptr_t player=0;
    dialogue_input_owner19c3::Result inputOwner{};
    HeadPresentation presentation{};
};
inline bool age(std::uint64_t tick,std::uint64_t now,std::uint64_t limit) noexcept {
    return tick&&now>=tick&&now-tick<=limit;
}
inline bool fresh(const State& s,std::uint64_t now) noexcept {return s.known&&s.entity&&age(s.tick,now,80);}
template<class Read> HeadPresentation observeHeadPresentation(std::uintptr_t base,std::uintptr_t game,
    std::uintptr_t player,std::uintptr_t cameras,Read& read) noexcept {
    HeadPresentation p{};std::uintptr_t vf=0,current=0;
    if(!read(cameras,vf)||vf!=base+0x408E560||!read(player,vf)||vf!=base+0x3AA4B98||
       !read(cameras+0x38,p.blendFromThirdPerson)||p.blendFromThirdPerson>1||!read(cameras+0x39,p.forceVisible)||p.forceVisible>1||
       !read(cameras+0x3B,p.bypassCompose)||p.bypassCompose>1||!read(cameras+0x30,p.selected)||!p.selected||
       !read(p.selected,p.selectedVft)||!read(player+0xA38,p.headHidden)||p.headHidden>1||
       !read(player+0xA39,p.groupFlags))return {};
    unsigned char value=0;
    if(!read(game+0x38,current)||current!=cameras||!read(cameras,vf)||vf!=base+0x408E560||
       !read(player,vf)||vf!=base+0x3AA4B98||!read(cameras+0x30,current)||current!=p.selected||
       !read(p.selected,vf)||vf!=p.selectedVft||
       !read(cameras+0x38,value)||value!=p.blendFromThirdPerson||!read(cameras+0x39,value)||value!=p.forceVisible||
       !read(cameras+0x3B,value)||value!=p.bypassCompose||!read(player+0xA38,value)||value!=p.headHidden||
       !read(player+0xA39,value)||value!=p.groupFlags)return {};
    p.manager=cameras;p.known=true;return p;
}
// Mirrors the exact retail GetPlayerDialog getter (197CB94), reached through
// game+150/module+10 at 311C9F6. iDialogueContextId is a SCRIPT PROPERTY, not a CVar.
// Called only in exact C_Player::UpdateView and the owned native camera-selector
// callback; no native calls, extra hot hook or worker-thread lookup.
template<class Read> State observe(std::uintptr_t base,std::uintptr_t self,std::uint64_t now,Read&& read,
    bool capturePresentation=false) noexcept {
    State out{};out.tick=now;
    std::uintptr_t vf=0,player=0,back=0,entity=0,game=0,module=0,manager=0,instance=0;
    if(!base||self<0x40||!read(self,vf)||vf!=base+0x3A81028)return out;
    // C10R18B: 0x3A81028 is C_Player's SECONDARY vtable at +0x40 (RTTI: C_Player@entitymodule, COL offset 0x40; ctor 0xDA7772
    // writes 0x3AA4B98 at +0 and 0x3A81028 at +0x40; 7F2335 itself uses this-0x40 as the object). So this-0x40 is the C_Player and
    // the C_CombatPlayer (0x3A80610) is its +0x278 back-link. C10R2..R18 read this-0x40 as the combat player: every receipt refused
    // PLAYER_IDENTITY (R10 + R14B logs, 431 of 431), so the dialogue witness and the activity lens never ran.
    player=self-0x40;std::uintptr_t actor=0;
    if(!read(player,vf)||vf!=base+0x3AA4B98||!read(player+0x278,actor)||!actor||
       !read(actor,vf)||vf!=base+0x3A80610||!read(actor+0x1F8,back)||back!=player||
       !read(player+0x38,entity)||!entity||!read(base+0x549D388,game)||!game||
       !read(game+0x150,module)||!module||!read(module,vf)||vf!=base+0x3E64C38||
       !read(module+0x10,manager)||!manager||!read(manager,vf)||vf!=base+0x3E64C90||
       !read(manager+0x90,instance))return out;
    if(instance&&(!read(instance,vf)||vf!=base+0x3E63A40||
                  !read(instance+0x18,vf)||vf!=base+0x3E63B60))return out;
    out.entity=entity;out.instance=instance;out.player=player;
    out.membership=dialogue_membership19c2::inspect(base,player,manager,instance,read);
    out.known=out.membership.known;
    // A live speech/bark scene is not interactive conversation ownership.
    // Keep native membership as independent evidence, but only the enabled
    // exact native "dialog" input map can suspend gameplay for dialogue.
    if(out.membership.active)out.inputOwner=dialogue_input_owner19c3::inspect(base,read);
    out.active=out.membership.active&&out.inputOwner.known&&out.inputOwner.enabled;
    if(!out.known)return out;
    std::uintptr_t cameras=0,camera=0;unsigned char debug=0,external=0;
    if(!read(game+0x38,cameras)||!cameras||!read(cameras,vf)||vf!=base+0x408E560)return out;
    // Same existing native callback, only during confirmed active dialogue.
    // Missing diagnostic bytes do not alter the existing ownership decision.
    if(capturePresentation&&out.active)out.presentation=observeHeadPresentation(base,game,player,cameras,read);
    if(!read(cameras+0x38,debug)||debug||!read(cameras+0x3B,external)||external||
       !read(cameras+0x30,camera)||!camera||!read(camera,vf))return out;
    if(camera==cameras+0x1E8&&vf==base+0x3A5A0F8)out.camera=Camera::FirstPerson;
    else if(camera==cameras+0x318&&vf==base+0x3A5A210)out.camera=Camera::Dialogue;
    return out; // Bone/UI/rider/external cameras never authorize embodiment.
}
inline bool embodiedOwner(const State& s,std::uint64_t now) noexcept {
    return fresh(s,now)&&s.active&&(s.camera==Camera::FirstPerson||s.camera==Camera::Dialogue);
}
struct V {float x=0,y=0,z=0;};
struct Q {float x=0,y=0,z=0,w=1;};
inline V add(V a,V b) noexcept {return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline V sub(V a,V b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline V mul(V a,float f) noexcept {return {a.x*f,a.y*f,a.z*f};}
inline float dot(V a,V b) noexcept {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline V cross(V a,V b) noexcept {return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
inline bool finite(V a) noexcept {return std::isfinite(a.x)&&std::isfinite(a.y)&&std::isfinite(a.z);}
inline V rotate(Q q,V p) noexcept {const V v{q.x,q.y,q.z};return add(p,mul(add(mul(cross(v,p),q.w),cross(v,cross(v,p))),2));}
inline Q conjugate(Q q) noexcept {return {-q.x,-q.y,-q.z,q.w};}
struct NativeHead {
    std::uint64_t tick=0,epoch=0;
    std::uintptr_t entity=0,character=0;
    V world{};Q body{};float scale=1;
    // Exact character+0xA08 model transform copied with this same head sample.
    V modelOrigin{};bool modelOriginKnown=false;
    // Same character-root adoption as the copied native world transform.
    native_roomscale_d76::Origin roomOriginD76{};
};
inline bool fresh(const NativeHead& h,std::uint64_t now) noexcept {
    const float norm=h.body.x*h.body.x+h.body.y*h.body.y+h.body.z*h.body.z+h.body.w*h.body.w;
    return h.entity&&h.character&&h.epoch&&age(h.tick,now,60)&&finite(h.world)&&
        std::isfinite(norm)&&std::fabs(norm-1)<.01f&&std::isfinite(h.scale)&&h.scale>=.1f&&h.scale<=10;
}
struct Eye {V position{};V basis[3]{{1,0,0},{0,1,0},{0,0,1}};};
inline bool valid(const Eye& e) noexcept {
    if(!finite(e.position))return false;
    for(auto b:e.basis)if(!finite(b)||std::fabs(dot(b,b)-1)>.02f)return false;
    return std::fabs(dot(e.basis[0],e.basis[1]))<.02f&&
        std::fabs(dot(e.basis[1],e.basis[2]))<.02f&&
        dot(cross(e.basis[0],e.basis[1]),e.basis[2])>.98f;
}
struct EyeOffset {
    std::uintptr_t entity=0,character=0;
    V local{};V basis[3]{};bool ready=false;
    V modelLocal{};bool modelLocalReady=false;
};
inline bool learn(const State& s,const NativeHead& h,const Eye& native,std::uint64_t now,EyeOffset& out) noexcept {
    if(!fresh(s,now)||s.active||s.camera!=Camera::FirstPerson||!fresh(h,now)||
       s.entity!=h.entity||!valid(native))return false;
    const V offset=mul(rotate(conjugate(h.body),sub(native.position,h.world)),1/h.scale);
    // Measured native eye/head relationship only: never learn a remote cinematic camera.
    if(!finite(offset)||dot(offset,offset)>.25f)return false;
    EyeOffset next{};next.entity=h.entity;next.character=h.character;next.local=offset;next.ready=true;
    // A measured ordinary neutral eye belongs to Henry's model, not an
    // authored dialogue head/neck animation. Current model transform remains
    // live; the existing HMD translation is composed once by camera_hook.
    if(h.modelOriginKnown&&finite(h.modelOrigin)){
        next.modelLocal=mul(rotate(conjugate(h.body),sub(native.position,h.modelOrigin)),1/h.scale);
        next.modelLocalReady=finite(next.modelLocal);
    }
    for(unsigned i=0;i<3;++i)next.basis[i]=rotate(conjugate(h.body),native.basis[i]);
    out=next;return true;
}
struct Policy {std::uint64_t tick=0;bool enabled=false;};
inline bool reconstruct(const State& s,const NativeHead& h,const EyeOffset& learned,
    Policy policy,std::uint64_t now,Eye& out) noexcept {
    if(!policy.enabled||!age(policy.tick,now,80)||!embodiedOwner(s,now)||!fresh(h,now)||
       !learned.ready||s.entity!=h.entity||learned.entity!=h.entity||learned.character!=h.character)return false;
    Eye next{};
    if(learned.modelLocalReady&&h.modelOriginKnown&&finite(h.modelOrigin)&&finite(learned.modelLocal))
        next.position=add(h.modelOrigin,rotate(h.body,mul(learned.modelLocal,h.scale)));
    else next.position=add(h.world,rotate(h.body,mul(learned.local,h.scale)));
    for(unsigned i=0;i<3;++i)next.basis[i]=rotate(h.body,learned.basis[i]);
    if(!valid(next))return false;
    // This is a native-eye BASE. Existing camera code applies recentered HMD,
    // IPD and user offsets once afterward. No cached world position or neck offset.
    // Epoch need not equal the learned calibration epoch: recenter changes XR
    // origin, not this body-local anatomical relationship. The live head accessor
    // independently rejects old-epoch snapshots.
    out=next;return true;
}
}
