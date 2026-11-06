// Generated exact Mod statements. Native activity/context and copied world input are fixture leaves.
struct PublicAlchemyGatesH5 {bool gesture=false,head=false,legacyPointer=false;};
static PublicAlchemyGatesH5 PublicAlchemySelectionH5(bool alchemyAvailable63) {
using namespace kcd2vr;
        const bool alchemyPalmExperiment63=combat_rules178r10::f5Rung.load(std::memory_order_acquire)==2&&
            workstation178r20c1::experimentArmed36.load(std::memory_order_acquire);
        const bool alchemyGesture67=alchemyAvailable63&&minigame_hands45::modeFor(minigame_hands45::Alchemy)!=minigame_hands45::Native&&
            (minigame_hands45::freeTrackedMode(minigame_hands45::modeFor(minigame_hands45::Alchemy))||alchemyPalmExperiment63);
        // H18: the obsolete whole-hand ray branch was reachable only in NATIVE
        // mode while F5=2, stealing gaze from the native fallback. Free Alchemy
        // already nominates ONE qualified nearby hand in its OnScreen bridge;
        // everywhere else (including Native/Semi and book) keep HMD gaze.
        const bool alchemyPointer59c2=false;
        const bool alchemyHeadSelection63=alchemyAvailable63;
        subsystems::alchemy::bridge67::enabled.store(alchemyGesture67,std::memory_order_release);
return {alchemyGesture67,alchemyHeadSelection63,alchemyPointer59c2};
}
static void PublicAlchemyContextH5(kcd2vr::semi_workstations46::Context& c,std::uint64_t now46) {
using namespace kcd2vr;
                const bool oneCraft67=bool(c.alchemy)!=bool(c.smith);
                const auto kind67=c.alchemy?minigame_hands45::Alchemy:minigame_hands45::Smithing;
                c.guided=guided_crafting55::active(now46);
                c.gestureAlchemy67=c.alchemy&&workstation178r20c1::fullVrAlchemy63(now46);
                c.gestureSmith67=c.smith&&workstation178r20c1::gestureSmith67(now46);
                c.alchemyGaze63=c.guided&&c.alchemy&&!c.smith;
                c.enabled=oneCraft67&&(c.guided||minigame_hands45::modeFor(kind67)!=minigame_hands45::Native)&&
                    combat_rules178r10::workstationEnabled.load(std::memory_order_acquire);
}
