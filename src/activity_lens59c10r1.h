#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include "dialogue_state59c10r2.h"
#include "activity_eye59c19.h"

namespace kcd2vr::activity_lens59c10r1 {
// Exact retail C_Player::UpdateView output. Not a CCamera/frustum field write.
struct Policy { std::uint64_t tick=0; float radians=0; bool immersive=false; bool eye=false; };
enum class Owner : unsigned { None, FirstPerson, Minigame };
enum class Refusal : unsigned { None, WitnessUnavailable, PlayerIdentity, ActivityIdentity,
    ActivityUnsupported, ActivityInactive, NoActivity, CameraManager, DebugCamera, CameraUnknown };
inline const char* name(Refusal r) noexcept {
    constexpr const char* names[]{"NONE","WITNESS_UNAVAILABLE","PLAYER_IDENTITY","ACTIVITY_IDENTITY",
        "ACTIVITY_UNSUPPORTED","ACTIVITY_INACTIVE","NO_ACTIVITY","CAMERA_MANAGER","DEBUG_OR_EXTERNAL_CAMERA","CAMERA_UNKNOWN"};
    return names[unsigned(r)];
}
struct Evidence { Owner camera=Owner::None; unsigned kindMask=0;
    Refusal refusal=Refusal::None;std::uintptr_t instance=0,cameraRva=0; };
struct ForgeCamera {std::uint64_t tick=0,lifetime=0;std::uintptr_t instance=0,player=0;Owner camera=Owner::None;};
inline bool forgeCurrent(const ForgeCamera& observed,std::uint64_t now,std::uintptr_t instance,
                         std::uint64_t lifetime,bool ready,bool transition) noexcept {
    return ready&&!transition&&instance&&observed.instance==instance&&observed.player&&
        observed.lifetime==lifetime&&dialogue_state59c10r2::age(observed.tick,now,80)&&
        (observed.camera==Owner::FirstPerson||observed.camera==Owner::Minigame);
}
// Positive copied native-camera observation, independent of requested Theater
// policy; current lifecycle and <=80ms age are rejoined on each host read.
bool ForgeCameraCurrent(std::uint64_t now) noexcept;
bool DiceCameraCurrentD80(std::uint64_t now) noexcept;
inline bool fresh(const Policy& p,std::uint64_t now) noexcept {
    return p.immersive&&p.tick&&now>=p.tick&&now-p.tick<=80&&
        std::isfinite(p.radians)&&p.radians>=1.0f&&p.radians<=2.45f;
}
inline float select(const Policy& p,std::uint64_t now,float native,const Evidence& e) noexcept {
    // Alchemy=1, Reading=4, Sharpening=8, ForgeBuilder=16, Dice=32. The native
    // construction plan remains a world texture; its live minigame owns lens.
    if(!fresh(p,now)||e.camera==Owner::None||!(e.kindMask&61)||
       (e.kindMask&~61u)||!std::isfinite(native)||native<=.1f||native>=3.1f)return native;
    return p.radians;
}
// Reader is a fault-contained native scalar read; tests exercise this same
// pointer/owner chain with explicit memory. No engine calls or SDK virtual slots.
template<class Read> Evidence qualify(std::uintptr_t base,std::uintptr_t viewSelf,
    const std::array<std::uintptr_t,4>& active,Read&& read,std::uintptr_t forgeBuilder=0,std::uintptr_t dice=0) noexcept {
    Evidence out{};std::uintptr_t vf=0,player=0,back=0,entity=0;
    const auto refuse=[&](Refusal why){out.refusal=why;return out;};
    if(!base||viewSelf<0x40||!read(viewSelf,vf)||vf!=base+0x3A81028)return refuse(Refusal::PlayerIdentity);
    // C10R18B: 0x3A81028 is C_Player's SECONDARY vtable at +0x40 (RTTI: C_Player@entitymodule, COL offset 0x40; ctor 0xDA7772
    // writes 0x3AA4B98 at +0 and 0x3A81028 at +0x40; 7F2335 itself uses this-0x40 as the object). So this-0x40 is the C_Player and
    // the C_CombatPlayer (0x3A80610) is its +0x278 back-link. C10R2..R18 read this-0x40 as the combat player: every receipt refused
    // PLAYER_IDENTITY (R10 + R14B logs, 431 of 431), so the dialogue witness and the activity lens never ran.
    player=viewSelf-0x40;std::uintptr_t actor=0;
    if(!read(player,vf)||vf!=base+0x3AA4B98||!read(player+0x278,actor)||!actor||
       !read(actor,vf)||vf!=base+0x3A80610||!read(actor+0x1F8,back)||back!=player||
       !read(player+0x38,entity)||!entity)return refuse(Refusal::PlayerIdentity);
    constexpr std::uintptr_t types[]{0x3F60B08,0x3A6A390,0x3F749E0,0x3F74BA0};
    for(unsigned i=0;i<active.size();++i){
        if(!active[i])continue;
        unsigned char running=0;
        out.instance=active[i];
        if(i==1)return refuse(Refusal::ActivityUnsupported);
        if(!read(active[i],vf)||vf!=base+types[i]||
           !read(active[i]+0x18,back)||back!=player)return refuse(Refusal::ActivityIdentity);
        if(!read(active[i]+0x2C,running)||running!=1)return refuse(Refusal::ActivityInactive);
        out.kindMask|=1u<<i;
    }
    if(forgeBuilder){
        unsigned char running=0;out.instance=forgeBuilder;
        if(!read(forgeBuilder,vf)||vf!=base+0x3F6FA30||
           !read(forgeBuilder+0x18,back)||back!=player)return refuse(Refusal::ActivityIdentity);
        if(!read(forgeBuilder+0x2C,running)||running!=1)return refuse(Refusal::ActivityInactive);
        out.kindMask|=16u;
    }
    if(dice){
        unsigned char running=0;out.instance=dice;
        if(!read(dice,vf)||vf!=base+0x3F72098||!read(dice+0x18,back)||back!=player)return refuse(Refusal::ActivityIdentity);
        if(!read(dice+0x2C,running)||running!=1)return refuse(Refusal::ActivityInactive);
        out.kindMask|=32u;
    }
    if(!out.kindMask)return refuse(Refusal::NoActivity);
    std::uintptr_t game=0,manager=0,camera=0;unsigned char debug=0,external=0;
    // 9155F1 reads this global; 7F2314 consumes game+38 as the camera manager.
    if(!read(base+0x549D388,game)||!game||!read(game+0x38,manager)||!manager||
       !read(manager,vf)||vf!=base+0x408E560)return refuse(Refusal::CameraManager);
    if(!read(manager+0x38,debug)||!read(manager+0x3B,external))return refuse(Refusal::CameraManager);
    if(debug||external)return refuse(Refusal::DebugCamera);
    if(!read(manager+0x30,camera)||!camera||!read(camera,vf))return refuse(Refusal::CameraManager);
    out.cameraRva=vf>=base&&vf-base<0x6000000?vf-base:0;
    if(camera==manager+0x1E8&&vf==base+0x3A5A0F8)out.camera=Owner::FirstPerson;
    else if(camera==manager+0x338&&vf==base+0x408EB60)out.camera=Owner::Minigame;
    else return refuse(Refusal::CameraUnknown); // UI, dialogue, bone-animated, external and unknown owners.
    return out;
}
// R89 dice: the game can keep its Minigame camera (manager+0x338, dice = 0.6 rad =
// 34 deg = 2.8x zoom) as the player view after the dice/activity witness cleared
// or while the activity is already stopping (+0x2C running!=1). The lens stopped
// at that edge, so nothing wrote the FOV any more and the zoom stayed after the
// dice game (D85R5 KCD2VR1 23:50-23:52 cameraOwner=2 nativeDeg=34.377).
// Same camera chain as qualify(), without needing a live activity.
template<class Read> Owner playerCamera(std::uintptr_t base,Read&& read,bool* observedOtherR95=nullptr) noexcept {
    if(observedOtherR95)*observedOtherR95=false;
    std::uintptr_t game=0,manager=0,camera=0,vf=0;unsigned char debug=0,external=0;
    if(!base||!read(base+0x549D388,game)||!game||!read(game+0x38,manager)||!manager||
       !read(manager,vf)||vf!=base+0x408E560)return Owner::None;
    if(!read(manager+0x38,debug)||!read(manager+0x3B,external))return Owner::None;
    if(debug||external){if(observedOtherR95)*observedOtherR95=true;return Owner::None;}
    if(!read(manager+0x30,camera)||!camera||!read(camera,vf))return Owner::None;
    if(camera==manager+0x1E8&&vf==base+0x3A5A0F8)return Owner::FirstPerson;
    if(camera==manager+0x338&&vf==base+0x408EB60)return Owner::Minigame;
    // A successfully read different native camera is positive takeover evidence.
    // Unreadable memory or a malformed expected camera remains unavailable.
    if(observedOtherR95&&vf&&camera!=manager+0x1E8&&camera!=manager+0x338)*observedOtherR95=true;
    return Owner::None;
}
// Only a qualified dice lifecycle may retain the lens after its own exit. The
// shared Minigame camera also serves alchemy/reading/etc.; owning that camera is
// not a dice token. Witness175::End increments lifetime once when dice clears.
struct ExitHoldR89 {
    bool armed=false;std::uintptr_t dice=0,player=0;std::uint64_t lifetime=0;
};
struct DiceExitFrameR91 {
    bool known=false,wanted=false;std::uintptr_t dice=0,player=0;
    std::uint64_t lifetime=0;unsigned kindMask=0;Evidence evidence{};Owner camera=Owner::None;
    // R95: intent and observation are independent. Unknown observations suspend
    // writes; the retained identity grants no lens authority on its own.
    bool policyKnown=true,policyFresh=true,protectedContext=false;
};
inline bool exitHoldStep(ExitHoldR89& h,const DiceExitFrameR91& f) noexcept {
    if((f.policyKnown&&!f.wanted)||f.protectedContext){h={};return false;}
    if(!f.known)return false;
    if(!f.player||(f.kindMask!=0&&f.kindMask!=32u)||(h.armed&&f.player!=h.player)){h={};return false;}
    // Positive replacement/end evidence revokes even while policy is stale.
    if(h.armed&&((f.dice&&(f.dice!=h.dice||f.lifetime!=h.lifetime))||
       (!f.dice&&(h.lifetime==UINT64_MAX||f.lifetime!=h.lifetime+1)))){h={};return false;}
    if(!f.policyKnown||!f.policyFresh)return false;
    if(f.dice&&f.kindMask==32u&&f.evidence.kindMask==32u&&
       f.evidence.refusal==Refusal::None&&f.evidence.camera!=Owner::None){
        h={true,f.dice,f.player,f.lifetime};return false; // the ordinary dice lens owns this frame
    }
    if(h.armed&&f.kindMask==32u&&f.dice==h.dice&&f.lifetime==h.lifetime&&
       f.evidence.refusal!=Refusal::ActivityInactive)return false; // same visit, inconclusive qualifier
    if(!h.armed||f.player!=h.player){h={};return false;}
    const bool stopping=f.kindMask==32u&&f.dice==h.dice&&f.lifetime==h.lifetime&&
        f.evidence.refusal==Refusal::ActivityInactive;
    const bool justEnded=!f.kindMask&&!f.dice&&h.lifetime!=UINT64_MAX&&f.lifetime==h.lifetime+1;
    if(!stopping&&!justEnded){h={};return false;}
    if(f.camera==Owner::None)return false; // native camera observation unavailable; no write
    if(f.camera!=Owner::Minigame){h={};return false;} // positively observed normal camera completes the debt
    return true;
}
// C10R19: same exact native callback, copied POD only; no render-side native polling.
activity_eye59c19::State EyeState(std::uint64_t now) noexcept;
struct EyeReceipt59c19 { std::uint64_t tick=0;activity_eye59c19::State state{};
    activity_eye59c19::Reason reason=activity_eye59c19::Reason::OwnerUnavailable;
    dialogue_state59c10r2::V authored{},anchored{},head{};bool haveHead=false,neutral=false,fromCull=false; }; // Composition attempt, not render acceptance.
void NoteEye59c19(const EyeReceipt59c19&) noexcept;
// Request only: native selector still verifies current Henry dialogue membership.
void RequestDialogueFirstPerson19c2(bool wanted,std::uint64_t now) noexcept;
bool Install() noexcept;
void Uninstall() noexcept;
void Update(bool immersive,float degrees,std::uint64_t now) noexcept;
void Report(std::uint64_t now) noexcept; // bounded control-thread diagnostics only
dialogue_state59c10r2::State Dialogue(std::uint64_t now) noexcept;
}
