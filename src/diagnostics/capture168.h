#pragma once
// Finite diagnostic leases, independent of gameplay/physics ownership.
// Control thread arms/stops. Producers do not refresh a lease by logging.
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <cstdlib>
namespace kcd2vr::diagnostics::capture168 {
enum class Domain : unsigned { Combat, Interaction, Hud, Reload, Hands, Vrik, HandReference, Bug, Physics };
inline constexpr unsigned DomainCount=9;
inline constexpr std::uint64_t Duration=180000;
// D84: independent F9 physics recorder; existing domain IDs remain stable.
// D48: legacy reader domains are aliases of the requested
// keys, never independent switches: HUD -> F2, hands -> F5, references -> F7.
inline constexpr Domain canonical(Domain d) noexcept {
    return d==Domain::Hud?Domain::Bug:d==Domain::Hands?Domain::Interaction:
        d==Domain::HandReference?Domain::Vrik:d;
}
inline constexpr unsigned members(Domain d) noexcept {
    d=canonical(d);return (1u<<unsigned(d))|
        (d==Domain::Bug?(1u<<unsigned(Domain::Hud)):0u)|
        (d==Domain::Interaction?(1u<<unsigned(Domain::Hands)):0u)|
        (d==Domain::Vrik?(1u<<unsigned(Domain::HandReference)):0u);
}
struct Gate {
    std::array<std::atomic<std::uint64_t>,DomainCount> until{},session{},closedAt{};
    std::atomic<unsigned> mask{0};
    // D67: 1 diagnostics, 2 fresh-publication comparison, 3 elbow/solve comparison.
    // This finite recorder owns only the selected comparison session.
    std::atomic<std::uint64_t> vrikFistSession{0}; // retained compatibility field; never enables a fist recipe
    std::atomic<unsigned> vrikStageD67{1};
    void select(Domain d,bool on,std::uint64_t now) noexcept {
        d=canonical(d);const auto i=unsigned(d),bits=members(d);
        if(d==Domain::Vrik){vrikFistSession.store(0,std::memory_order_release);vrikStageD67.store(1,std::memory_order_release);}
        const bool begin=on&&now&&now<=UINT64_MAX-Duration;
        const auto serial=begin?session[i].load(std::memory_order_acquire)+1:session[i].load(std::memory_order_acquire);
        const bool closing=!begin&&until[i].load(std::memory_order_acquire)!=0&&serial!=0;
        for(unsigned j=0;j<DomainCount;++j)if(bits&(1u<<j)){
            session[j].store(serial,std::memory_order_release);
            until[j].store(begin?now+Duration:0,std::memory_order_release);
            if(begin||closing)closedAt[j].store(closing?now:0,std::memory_order_release);
        }
        if(begin)mask.fetch_or(bits,std::memory_order_release);
        else mask.fetch_and(~bits,std::memory_order_release);
    }
    bool active(Domain d,std::uint64_t now) const noexcept {
        const auto i=unsigned(canonical(d));if(!(mask.load(std::memory_order_acquire)&(1u<<i)))return false;
        const auto u=until[i].load(std::memory_order_acquire);
        return now&&u&&now<u&&u-now<=Duration;
    }
    // Only already-captured records and finite end receipts may drain briefly
    // after a stop. Ordinary producers must still hold active(). A zero/older
    // session is never a ticket; this is not an always-on fault exception.
    bool receipt(Domain d,std::uint64_t serial,std::uint64_t now) const noexcept {
        const auto i=unsigned(canonical(d));
        if(!serial||session[i].load(std::memory_order_acquire)!=serial)return false;
        if(active(d,now))return true;
        const auto closed=closedAt[i].load(std::memory_order_acquire);
        return closed&&now>=closed&&now-closed<=5000;
    }
    unsigned vrikRung(std::uint64_t now) const noexcept {
        const auto serial=session[unsigned(Domain::Vrik)].load(std::memory_order_acquire);
        if(!active(Domain::Vrik,now))return 0;
        const auto stage=vrikStageD67.load(std::memory_order_acquire);
        return serial&&stage>=1&&stage<=3&&serial==session[unsigned(Domain::Vrik)].load(std::memory_order_acquire)?stage:1u;
    }
    unsigned cycleVrik(std::uint64_t now) noexcept {
        const auto rung=vrikRung(now);
        if(rung==1||rung==2){vrikStageD67.store(rung+1,std::memory_order_release);return rung+1;}
        select(Domain::Vrik,rung==0,now);return rung==0&&active(Domain::Vrik,now)?1u:0u;
    }
    bool any(std::uint64_t now) const noexcept {
        return active(Domain::Bug,now)||active(Domain::Combat,now)||active(Domain::Interaction,now)||active(Domain::Reload,now)||active(Domain::Vrik,now)||active(Domain::Physics,now);
    }
    unsigned poll(std::uint64_t now) noexcept {
        unsigned ended=0;
        for(auto d:{Domain::Bug,Domain::Combat,Domain::Interaction,Domain::Vrik,Domain::Reload,Domain::Physics}){
            const auto i=unsigned(d);auto u=until[i].load(std::memory_order_acquire);
            if(u&&(now>=u||u-now>Duration)&&until[i].compare_exchange_strong(u,0)){
                const auto bits=members(d);mask.fetch_and(~bits,std::memory_order_release);
                for(unsigned j=0;j<DomainCount;++j)if(bits&(1u<<j)){
                    until[j].store(0,std::memory_order_release);closedAt[j].store(now,std::memory_order_release);
                }
                if(d==Domain::Vrik){vrikFistSession.store(0,std::memory_order_release);vrikStageD67.store(1,std::memory_order_release);}
                ended|=1u<<i;
            }
        }
        return ended;
    }
};
inline Gate gate;
inline const char* name(Domain d) noexcept {return d==Domain::Physics?"PHYSICS":d==Domain::Bug?"BUG":d==Domain::HandReference?"HAND_REFERENCE":d==Domain::Vrik?"VRIK":d==Domain::Hands?"HANDS":d==Domain::Combat?"COMBAT":d==Domain::Interaction?"INTERACTION":d==Domain::Reload?"RELOAD":"HUD";}
inline bool active(Domain d,std::uint64_t now) noexcept {return gate.active(d,now);}
inline bool any(std::uint64_t now) noexcept {return gate.any(now);}
inline bool handCapture(std::uint64_t now) noexcept {return active(Domain::Vrik,now)||active(Domain::Combat,now)||active(Domain::Interaction,now);}
inline bool contains(const char* s,const char* token) noexcept {return s&&std::strstr(s,token)!=nullptr;}
// D84: F9 owns a finite independent physics recorder. Historical F2/F3
// consumers still accept these bounded status families for compatibility.
inline bool f9FamilyD56(const char* s) noexcept {
    if(!s)return false;
    for(const char* p:{"T4 F9 ","B2 F9_INSTALL ","B3 F9 requested=","H21 F9 ",
            "H26 F9 ","C61 F9 ","C57R1 F9 ","C4R4 F9 refused:",
            "C4R4 RAGDOLL_F9 ","C58 LAB RUNG_NOW="})
        if(std::strncmp(s,p,std::strlen(p))==0)return true;
    return false;
}
// C54R3: payload text names other systems and may contain FAULT/INPUT_OWNER.
// A structured family keeps its explicit recorder; free-form reason text cannot
// accidentally enable details under another key or bypass quiet mode.
inline int exactDomainC54R3(const char* s) noexcept {
    if(!s)return -1;
    if(std::strncmp(s,"D48_F11 ",8)==0)return int(Domain::Reload);
    if(std::strncmp(s,"D42_F2 ",7)==0)return int(Domain::Bug);
    if(std::strncmp(s,"D42_F3 ",7)==0)return int(Domain::Combat);
    if(std::strncmp(s,"D42_F5 ",7)==0)return int(Domain::Interaction);
    if(std::strncmp(s,"D42_F7 ",7)==0)return int(Domain::Vrik);
    if(std::strncmp(s,"D84_F9 ",7)==0)return int(Domain::Physics);
    // A34 already observes the installed native soup/wash action. Its old
    // prefix fell through to Combat, dropping it in an F5-only recording.
    if(std::strncmp(s,"A34 NATIVE_PASSENGER ",21)==0)return int(Domain::Interaction);
    if(std::strncmp(s,"HAND_F10 ",9)==0)return int(Domain::HandReference);
    if(std::strncmp(s,"H29R4 F5_",9)==0)return int(Domain::Interaction);
    if(std::strncmp(s,"H29 STRIKE",10)==0||std::strncmp(s,"H29 FIST_BLOCK",14)==0)return int(Domain::Combat);
    if(std::strncmp(s,"VRIK_F7 ",8)==0)return int(Domain::Vrik);
    // These legacy pose labels are produced only by the finite F7 recorder.
    // Match the producer family before inspecting payload words (HUD/FAULT/etc.).
    for(const char* p:{"V66.89-POSE TEMP_WRITE ","V66.115R1 HEAD_BODY ","V66.128-BODY ",
        "V66.137 COMPOSE_SET ","V66.89-ANATOMY ","V66.92-HIERARCHY ","V66.128-LEGS ",
        "V66.89-FINGER-","V66.94-FINGER ","V66.105-FINGER-","V66.92R1-FINGER-DATA ",
        "W1 HAND_ANATOMY ","R59.16C8 PALM_AUTO ","WS TORSO "})
        if(std::strncmp(s,p,std::strlen(p))==0)return int(Domain::Vrik);
    for(const char* p:{"C54R3 HIT_CHAIN ","C54R3 HIT_NOMINATION ","C54R3 HIT_QUEUE ","C54R3 PUNCH_PATH ","C54R3 PUNCH_HEALTH "})
        if(std::strncmp(s,p,std::strlen(p))==0)return int(Domain::Combat);
    if(std::strncmp(s,"C54R3 F5_DOOR_",14)==0)return int(Domain::Interaction);
    return -1;
}
// Retained for source compatibility with older fixtures. D48 has no
// unconditional textual exception, including FAULT, startup and mode changes.
inline bool essential(const char*) noexcept {return false;}
inline int receiptDomain(const char* s) noexcept {
    if(!s)return -1;
    for(const char* p:{"D42_F2_END ","F2_CAUSAL "})if(std::strncmp(s,p,std::strlen(p))==0)return int(Domain::Bug);
    if(std::strncmp(s,"D42_F3_END ",11)==0)return int(Domain::Combat);
    for(const char* p:{"D42_F5_END ","H29R4 F5_END ","C54R3 F5_DOOR_CAPTURE_END "})
        if(std::strncmp(s,p,std::strlen(p))==0)return int(Domain::Interaction);
    if(std::strncmp(s,"D42_F7_END ",11)==0)return int(Domain::Vrik);
    if(std::strncmp(s,"D48_F11 ",8)==0||contains(s,"RELOAD_NATIVE_END "))return int(Domain::Reload);
    if(contains(s,"F8_LIFECYCLE_END "))return int(Domain::Interaction);
    if(std::strncmp(s,"V66.168 CAPTURE ",16)==0||std::strncmp(s,"D42_COVERAGE ",13)==0){
        if(contains(s,"domain=BUG")||contains(s,"domain=HUD"))return int(Domain::Bug);
        if(contains(s,"domain=COMBAT"))return int(Domain::Combat);
        if(contains(s,"domain=PHYSICS"))return int(Domain::Physics);
        if(contains(s,"domain=INTERACTION")||contains(s,"domain=HANDS"))return int(Domain::Interaction);
        if(contains(s,"domain=VRIK")||contains(s,"domain=HAND_REFERENCE"))return int(Domain::Vrik);
        if(contains(s,"domain=RELOAD"))return int(Domain::Reload);
    }
    return -1;
}
inline bool capturedReceipt(const char* s,std::uint64_t now) noexcept {
    const int d=receiptDomain(s);if(d<0)return false;
    const char* p=std::strstr(s,"session=");
    if(!p)return active(Domain(d),now);
    p+=8;if(*p<'0'||*p>'9')return false;
    char* end=nullptr;const auto serial=std::strtoull(p,&end,10);
    if(!end||(*end&&*end!=' '))return false;
    return gate.receipt(Domain(d),serial,now);
}
inline Domain domain(const char* s) noexcept {
    if(f9FamilyD56(s))return Domain::Physics;
    const int exact=exactDomainC54R3(s);if(exact>=0)return Domain(exact);
    if(contains(s,"H21 MS_ELIGIBILITY "))return Domain::Combat;
    // D64: these producers already run only for F3. Route their bounded
    // receipts before incidental payload words or the default F2 domain.
    for(const char* p:{"V66.120 HANDLING ","V66.124 WEAPON_ACQUISITION ",
            "R59.16C10 SUPPORT_LATCH ","C39 SUPPORT_GRIP "})
        if(std::strncmp(s,p,std::strlen(p))==0)return Domain::Combat;
    // D56: bounded gore admission/material families belong to the F3 capture.
    for(const char* p:{"C43T DECAP_CORPSE ","C60 LIMB_","C60 DECAP_GATE ","C61 CORPSE_CUT_",
            "C39 DECAP ","D43 LIMB_","D54 GORE_","D56 GORE_"})
        if(std::strncmp(s,p,std::strlen(p))==0)return Domain::Combat;
    // H14: this native combo record describes recipe CURSORS, not a HUD
    // cursor. Its actual formatter ends with CURSORS_AND_CANDIDATES_ONLY;
    // the broad HUD matcher below used to silently discard it in F3-only runs.
    if(contains(s,"V66.91-F3-COMBO_EVENT "))return Domain::Combat;
    if(contains(s,"D52 F5_DIALOGUE_"))return Domain::Interaction;
    if(contains(s,"D52 DIALOGUE_GAZE_"))return Domain::Bug;
    if(contains(s,"C63 DIALOGUE_HMD_LOOK "))return Domain::Hud; // finite F2 receipt, never quiet per-frame logging
    if(contains(s,"C62 IMPALE_NAMESPACE "))return Domain::Combat; // bounded pre-submit witness, never essential
    if(contains(s,"C32 GRAPHICS_STATE ")||contains(s,"C26 EYE_ADAPT ")||contains(s,"C26 VR_SHADOWS "))return Domain::Hud;
    // Native-grasp version prefixes changed independently of the F5 recorder.
    // In C19C2 the C10R18 refusal accidentally defaulted to Combat, hiding the
    // acquisition failure precisely when the user ran F5 with F3 disabled.
    if(contains(s,"NATIVE_GRASP")||contains(s,"GRAB_SOCKET")||contains(s,"C10 HAND_HOLD_INPUT")||contains(s,"C10 HAND_USE hand="))return Domain::Interaction;
    if(contains(s,"R59.16C10R11 ACTION_OWNER")||contains(s,"R59.16C10R11 MANNEQUIN_QUEUE")||contains(s,"R59.16C10R11 MANNEQUIN_SCOPE")||
       contains(s,"R59.16C10R11 HENRY_CONTROLLER")||contains(s,"R59.16C10R11 ANIM_EVENT"))return Domain::Interaction;
    if(contains(s," F11_STAGE ")||contains(s,"F11-RELOAD")||contains(s,"RELOAD_NATIVE")||contains(s,"RELOAD_MILESTONE")||contains(s,"WEAPON_LIFECYCLE")||contains(s,"MISSILE_HELPER")||contains(s,"ARROW ATTACH"))return Domain::Reload;
    if(contains(s,"F8_LIFECYCLE")||contains(s,"LIFECYCLE168")||contains(s,"HIGGS")||
       contains(s,"-F5-")||contains(s," F5_")||contains(s,"ITEM_CYCLE")||contains(s,"HAND-USE")||contains(s,"HAND-HOVER")|| // V66.178R16: R13/R14 F5 families
       contains(s,"ITEM_CENSUS")|| // V66.178R19 (also accepted under F11-RELOAD, see accept())
       contains(s,"INTERACT")||contains(s,"ACTIVITY")||contains(s,"ALCHEMY")||contains(s,"BOOK_FOCUS_ZOOM")||
       contains(s,"SMITH")||contains(s,"MINIGAME")||contains(s,"UI-INPUT")||contains(s,"DOOR_")||contains(s,"PAGE_INPUT")||
       contains(s,"GESTURE_TALK")||contains(s,"ROOMSCALE_DODGE")|| // R59.16C7E-DECAP1: gestures/dodge are F5 material
       contains(s,"PREMAP")||contains(s,"INPUT_OWNER"))return Domain::Interaction;
    if(contains(s,"HUD")||contains(s,"PALM_ALPHA")||contains(s,"PALM_GAZE")||contains(s,"DIALOGUE_VIEW")||contains(s,"DIALOGUE_GATE")||contains(s,"-DIALOGUE-GUARD")||contains(s,"CAMERA_BODY")||contains(s,"-UI")||contains(s,"THEATER")||contains(s,"PAUSE")||
       contains(s,"DEATH-FIT")||contains(s,"CURSOR")||contains(s,"PROMPT")||
       contains(s,"COMPOSITION")||contains(s,"PRESENTATION")||contains(s,"XR_TIMING")||
       contains(s,"HOOK_COST")||contains(s,"FRAME_BUDGET")||contains(s,"FRAME_STALL")||contains(s,"POSE-REJECT")||contains(s," PERF ")|| // R59.16C7E-DECAP1: performance = F2
       contains(s,"XR-TIMING")||contains(s,"EYE-")||contains(s,"-POSE-TOKEN")||
       contains(s,"-LIVE-PUBLISH")||contains(s,"-LIVE-COPY")||contains(s,"-XR-CONSUME")||
       contains(s,"-CONTINUOUS")||contains(s,"-TWO-SLOT")||contains(s,"-LIVE-PAIR")||
       contains(s,"CLOUD")||contains(s,"WATER")||contains(s,"SVO")||contains(s,"-PAIR-POSE"))return Domain::Hud;
    if(contains(s,"COMBAT")||contains(s,"HIT_")||contains(s,"STRIKE")||contains(s,"PARRY")||contains(s,"COMBO")||contains(s,"CLINCH"))return Domain::Combat;
    if(contains(s,"VRIK")||contains(s,"FRIK")||contains(s,"ELBOW")||contains(s,"HAND_SOURCE"))return Domain::Vrik;
    return Domain::Bug;
}
inline bool accept(const char* s,std::uint64_t now) noexcept {
    if(!s)return false;
    if(receiptDomain(s)>=0)return capturedReceipt(s,now);
    if(!gate.mask.load(std::memory_order_relaxed))return false;
    // Retain existing F2/F3 readers alongside the explicit F9 recorder.
    // Payload text cannot create or extend a capture.
    if(f9FamilyD56(s))return active(Domain::Physics,now)||active(Domain::Combat,now)||active(Domain::Bug,now);
    // D50: a body owner is shared F2/F7/F3 evidence. Its alias/reason payload
    // must not select a different recorder, and its optional native hash read
    // must be gated by this same policy before formatting.
    if(std::strncmp(s,"V66.178R30C1 BODY_GATE ",sizeof("V66.178R30C1 BODY_GATE ")-1)==0)
        return active(Domain::Bug,now)||active(Domain::Vrik,now)||active(Domain::Combat,now);
    if(std::strncmp(s,"T18 AMMO ",9)==0)
        return active(Domain::Interaction,now)||active(Domain::Reload,now)||active(Domain::Bug,now);
    // Read-only native ownership witnesses serve the F7 and F11 producers.
    for(const char* p:{"V65.09-F10-ORDER ","V65.02-SETIK ","V65.02-OWNER ","V65.47-WEAPON-OWNER LIGHT "})
        if(std::strncmp(s,p,std::strlen(p))==0)
            return active(Domain::Vrik,now)||active(Domain::Reload,now);
    for(const char* p:{"D49 HOLSTER_DOOR_ROUTE ","H40 F5_HAND_ONLY_DOOR ","V66.90R1-CURSOR ",
        "D48 HOLSTER_REQUEST ","D48 HOLSTER_NOT_ENTERED ","D48 HOLSTER_PRESENTATION ",
        "D48 HOLSTER_REVEAL ","D48 FRONT_HOLSTER ","HS2 HOLSTER_SAY ","HS2 WEAPON_SWITCH ",
        "H24 HOLSTER_FENCE ","V66.116 HOLSTER ","V66.126 EQUIPMENT_REJECT ","C62 EQUIPMENT_REJECT "})
        if(std::strncmp(s,p,std::strlen(p))==0)
            return active(Domain::Interaction,now)||active(Domain::Bug,now);
    if(std::strncmp(s,"R25 ALCHEMY_RESOURCE_PATH ",sizeof("R25 ALCHEMY_RESOURCE_PATH ")-1)==0)
        return active(Domain::Interaction,now)||active(Domain::Bug,now);
    const int exact=exactDomainC54R3(s);if(exact>=0)return active(Domain(exact),now);
    // Route cost families before payload words such as INTERACTION_BODY/FIST.
    // F2 uses Bug, while HUD remains a separate recorder.
    if(std::strncmp(s,"V66.122 PERF ",13)==0||std::strncmp(s,"V66.178R22 HOOK_COST ",19)==0||
       std::strncmp(s,"V66.178R6 FRAME_STALL ",21)==0)
        return active(Domain::Bug,now)||active(Domain::Hud,now);
    // The same read-only FIFO producer serves F3, F5 and reload captures.
    // Previously F5-only snapshots were formatted, then silently classified as combat.
    if(contains(s,"V66.90-PLAYBACK "))return active(Domain::Combat,now)||active(Domain::Interaction,now)||active(Domain::Reload,now);
    // The existing dialogue reporter runs under either camera or interaction capture.
    // Preserve both finite leases; neither record is essential in quiet mode.
    if(contains(s,"HUD_DIALOGUE_STATE")||contains(s,"HUD_DIALOGUE_PRESENTATION")||contains(s,"HUD_DIALOGUE_BODY"))
        return active(Domain::Hud,now)||active(Domain::Interaction,now);
    // The producer explicitly serves F3 and F2; do not silently discard its
    // combat-side publication by classifying the CAMERA prefix as HUD only.
    if(contains(s,"CAMERA_BODY"))return active(Domain::Combat,now)||active(Domain::Hud,now);
    if(contains(s,"GRIP_SOCKET"))return active(Domain::Combat,now)||active(Domain::Reload,now);
    if(contains(s,"-COMBAT_CAPTURE")||contains(s,"POSE_VALIDITY")||contains(s,"INPUT_OWNER")||contains(s,"UI-INPUT")||
       contains(s,"UI-STATE")||contains(s,"PREMAP")||contains(s,"CAPTURE_HEALTH"))return any(now);
    if(contains(s,"F10_HANDS")||contains(s,"HAND_CONTACT")||contains(s,"SELF_CONTACT")||contains(s," AUTHORITY")||contains(s,"HAND_USE_GATE")||contains(s,"FIST")||contains(s,"BOXING")) // R59.16C7E-DECAP1: hand-to-hand under F3 or F5
        return handCapture(now);
    // Bow pose/state is needed during either a combat or reload capture.
    // Keep the existing producer throttles; no extra observer or native reads.
    if(contains(s,"BOW_STRING")||contains(s,"BOW_STATE")||contains(s,"BOW_GEOMETRY")||
       contains(s," BOW_")||contains(s,"BOW_RELEASE"))
        return active(Domain::Reload,now)||active(Domain::Combat,now);
    // The existing fader recorder serves F2 Bug and explicit HUD captures.
    // ACTIVITY must not send its already-gated receipt to F5-only routing.
    if(contains(s,"R47C1 ACTIVITY_FADER "))
        return active(Domain::Bug,now)||active(Domain::Hud,now);
    if(contains(s,"C71 F5_CRAFT_")||contains(s,"F5_ATTEMPT ")||contains(s,"D52 CONTACT_OCCUPANCY "))
        return active(Domain::Interaction,now)||active(Domain::Bug,now);
    if(contains(s,"ITEM_CENSUS"))return active(Domain::Reload,now)||active(Domain::Interaction,now); // V66.178R19
    // D38: R8D holster attempts are produced ONLY under F2 BUG but fell through to Combat (F3) = never written.
    if(contains(s,"R8D HOLSTER_")||contains(s,"HOLSTER_ZONE ")||contains(s,"HOLSTER_REQUEST "))return active(Domain::Interaction,now)||active(Domain::Bug,now);
    // D30 explicit F-bind routes (formerly quiet-mode lines or new D30 summaries).
    if(std::strncmp(s,"F2 FRAME ",9)==0||std::strncmp(s,"J26 PACING ",11)==0||contains(s,"XR_TIMING"))return active(Domain::Bug,now)||active(Domain::Hud,now);
    if(std::strncmp(s,"F7 SUMMARY ",11)==0)return active(Domain::Vrik,now);
    if(contains(s,"D52 DIALOGUE_GAZE_STATE ")||contains(s,"D53 PROMPT_CONSUMER ")||contains(s,"D56 PROMPT_")||contains(s,"D56 GLYPH_ONLY "))return active(Domain::Bug,now)||active(Domain::Interaction,now);
    // D53: the bounded dodge receipt belongs to boxing (F3) as well as F5.
    if(contains(s,"R59.16C5 ROOMSCALE_DODGE "))return active(Domain::Combat,now)||active(Domain::Interaction,now);
    if(std::strncmp(s,"F5 HELD ",8)==0||std::strncmp(s,"F5 ANIM ",8)==0)return active(Domain::Interaction,now);
    if(std::strncmp(s,"F11 RANGED",10)==0||contains(s,"XR32 CROSSBOW_ZOOM ")||contains(s,"T27 LEFT_CHEST preflight="))return active(Domain::Reload,now)||(contains(s,"T27 ")&&active(Domain::Combat,now));
    if(contains(s,"STOWED_BOW"))return active(Domain::Interaction,now)||active(Domain::Reload,now);
    if(contains(s,"T8 INVENTORY ")||contains(s,"T1 WEAPON_RELEASE ")||contains(s,"T1 WEAPON_DROP_RESULT ")||contains(s,"T5 SUPPLY_STATUS "))
        return active(Domain::Reload,now)||active(Domain::Combat,now)||active(Domain::Interaction,now);
    if(contains(s,"M31 MAP_PRESENT ")||contains(s,"H24 MAP_INPUT ")||contains(s,"H25 MAP_GESTURE ")||contains(s,"T5 MAP_READINESS "))
        return active(Domain::Interaction,now)||active(Domain::Hud,now);
    if(contains(s,"R47C1 DIALOGUE_POSE")||contains(s,"BOOK_FOCUS_ZOOM"))return active(Domain::Interaction,now)||active(Domain::Hud,now);
    if(contains(s,"D52 GORE_")||contains(s,"C47 IMPALE stage=")||contains(s,"C32 IMPALE")||contains(s,"C50 GORE_")||contains(s,"C50 DISMEMBER")||contains(s,"C43D STUMP_HEAD")||
       contains(s,"C32 GORE")||contains(s,"C47 GORE ")||contains(s,"C32 DECAP")||contains(s,"C41 DECAP")||contains(s,"C44 DECAP")||contains(s,"C44 GORE")||
       contains(s,"C26 KICK")||contains(s,"C37 KICK_")||contains(s,"R59C5 NATIVE_SERVICE"))return active(Domain::Combat,now);
    return active(domain(s),now);
}
}

// F2 owns general/HUD diagnostics. Gameplay comparisons remain separate preferences.
// Producers copy existing values into a bounded try-lock queue. Formatting/I/O is on the control worker.
namespace kcd2vr::diagnostics::ownership48 {
enum Stage : unsigned { Body=1, Pose=2, Attachment=3, CameraBase=4, CameraFinal=5, PoseSkip=6 };
struct Event {
 std::uint64_t order=0,session=0,tick=0,cpuNs=0,sample=0,publication=0,epoch=0,sourceEpoch=0,sourceNs=0;
 std::int64_t poseTime=0;std::uint64_t item=0,generation=0,actor=0,object=0;
 unsigned stage=0,thread=0,flags=0;int side=-1,code=0;char reason[128]{};float v[32]{};
};
inline thread_local std::uint64_t cameraSample48=0,cameraPublication48=0,cameraEpoch48=0,cameraActor48=0;
inline std::array<Event,2048> queue{};inline unsigned head=0,tail=0;
inline std::atomic_flag lock=ATOMIC_FLAG_INIT;inline std::atomic<std::uint64_t> dropped{0},order{0};
inline bool active(std::uint64_t tick) noexcept {return capture168::active(capture168::Domain::Bug,tick);}
inline void name(Event& e,const char* s) noexcept {if(!s)return;std::size_t i=0;for(;i+1<sizeof(e.reason)&&s[i];++i)e.reason[i]=s[i];e.reason[i]=0;}
inline bool push(Event e) noexcept {
 if(!active(e.tick))return false;
 if(lock.test_and_set(std::memory_order_acquire)){dropped.fetch_add(1,std::memory_order_relaxed);return false;}
 const unsigned next=(head+1)%queue.size();
 if(next==tail){lock.clear(std::memory_order_release);dropped.fetch_add(1,std::memory_order_relaxed);return false;}
 e.session=capture168::gate.session[unsigned(capture168::Domain::Bug)].load(std::memory_order_acquire);
 e.order=order.fetch_add(1,std::memory_order_relaxed)+1;
 e.cpuNs=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
 queue[head]=e;head=next;lock.clear(std::memory_order_release);return true;
}
inline bool pop(Event& e) noexcept {
 if(lock.test_and_set(std::memory_order_acquire))return false;
 const bool have=tail!=head;if(have){e=queue[tail];tail=(tail+1)%queue.size();}
 lock.clear(std::memory_order_release);return have;
}
}
