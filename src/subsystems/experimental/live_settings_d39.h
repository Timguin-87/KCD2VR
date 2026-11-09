#pragma once
// D39: the production menu's value reducer and click dispatch. No Win32/game
// types, so the same path can be exercised by the portable regression runner.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cwchar>
#include <type_traits>
#include <utility>
#include "../../vrik/instant_activities_d72.h"
#include "../dice/hand_select_d85.h" // D85 dice
#include "../../vrik/static_grab_r89.h" // R90 (R89 pickup): small props in the hand
#include "../../vrik/two_hand_weight_r90.h" // R90 two-hand weight feel
#include "../../vrik/native_crouch_r93.h" // R98 body height labels/cycle
#include "../../controller_layout_r118.h" // R118 controller layout (GEAR page 2)

namespace kcd2vr::live_settings_d39 {
struct Settings {
    bool instantActivitiesD72=true,instantInteractionsD72=false,instantCraftingD72=false,instantReloadsD72=false,instantCombatD72=true; // R124 defaults = Miki's INI
    unsigned instantStateD72[4]{};
    bool contactContinuityD71=true,itemHandoverD71=true;
    bool handLeadD69=false,weaponSameFrameD69=false,rigidGripD69=true,elbowHysteresisD69=false;
    float handLeadMsD69=11.f;
    bool finisherUpperPlayerD83=false,finisherUpperNpcD83=false;bool bladeZoneD84=true,upCutLowD84=true,stabStartD84=true; // D84 blade zone / up cut / stab start
    // D83R1: native action + limited tracked head turns
    bool stabFixD77=true,unarmedFixD77=true,masterstrikeFixD77=true,hudWalkoffD70=false;float leanHeightD70=.12f,dodgeWindowD70=1.25f;
    bool gripAnchorD67=true,arcCoverageD67=true,supportReleaseD67=false,contactFairnessD67=false,elbowContinuityD67=true;
    bool smithRecoveryD67=true,smithSemiD67=true;
    int motionTrialD67=4;float elbowOutwardD67=.20f,elbowBackD67=.25f;
    float shoulderWarningM=.15f;
    int handsMode=2;
    bool handSeparation=false,nativeHandPhysics=false,handSmoothing=false,handSmoothingPaused=false;
    bool workstationGestures=true;
    bool petGestureD59=true,washGestureD59=false,alchemyTransferD59=true;
    int alchemyPickupReach=0;
    bool smithToolCollision=true,stationTorso=true;
    float stationClearCm=12.f;
    bool torchGesture=true,visorGesture=true;
    bool parchmentMap=true,parchmentReady=false;
    int cameraHoldMs=1500;
    int timingLocationD64=1;
    float mapSizeD64=.80f;
    bool mapSupportD64=true;
    bool mapNativePaperM43=false;
    bool twoHandBiasD65=true;
    bool knifeRightHipD66=false;
    bool eyeSrgbR114=true; // R114: VrEyeSrgbR114, eye swapchain prefers sRGB (VDXR colours like SteamVR); next game start
    bool knifeChestZoneR109=false; // R109: VrKnifeChestZoneR109, the empty left hand also draws a knife at the chest pouch (where the dagger shows)
    bool vrikOnStartR111=true,startHenryEyesR111=true; // R111: VrVrikOnStartR111 (VRIK at start/load, R149: also without a saved T-pose), VrStartAtHenryEyesR111 (recenter = Henry's eyes, R148: + start facing)
    int controllerLayoutR118=0; // R118: VrControllerLayoutR118, 0 OPENXR / 1 STEAMVR / 2 PS VR2 (X hold = inventory on 1/2)
    bool quickSlotFadeR112=true; // R112: VrHudQuickSlotFadeR112, quick weapon/potion selectors fade 3 s after the last change
    bool alchemyViewSelectR116=true; // R116: VrAlchemyViewSelectR116, alchemy selection = what the headset looks at / the hand reaches
    bool swingWhooshR116=true; // R116: VrSwingWhooshR116, weapon whoosh at your physical release, the clip's later one skipped
    bool bowAimD65=true,crossbowReloadD65=true,pickupClockD65=true;
    float alchemySpeedD65=8.f,crankTravelD66=.08f;
    float twoHandPrimaryBiasD65=.4f;
    bool pickupCameraBridgeD55=true,alchemyToolContactD55=true;
    bool bowArmAim=true,labPropWake=false,arrowNativeTurnD48=true,frikElbowsD48=true;
    bool elbowSolverR89=false,elbowPreferenceR92=false; // R90: R89 hand-guided elbow option (VrElbowSolverR89); same slot as frikElbowsD48
    bool elbowF2OverrideD48=false; // Read-only comparison status; never applied or saved.
    bool nativeRoomscaleD76=false,npcListeningD76=true,ridingVrikD76=true;
    bool diceHandsD85=false;unsigned diceHandsStatusD85=0; // D85 dice: point to choose (experiment, OFF)
    bool staticGrabR89=true;  // R90 (R89 pickup): small scenery props in the hand (VrStaticPropGrabR89)
    bool crossbowHandOnlyR90=true,corpseSqueezeSearchR90=false; // R96: crossbow reload route (true HAND ONLY), corpse search route
    bool heavyCrossbowHandPullR111=true; // R111: VrHeavyCrossbowHandPullR111, heavy crossbow string pulled by hand like the light one (no cranequin)
    bool mediumCrossbowHandPullR117=true; // R117: VrMediumCrossbowHandPullR117, goat's-foot crossbow string pulled by hand like the light one (no spanner)
    bool twoHandWeightR90=false; // R90: VrWeaponWeightFeel also scales the two-hand rotation drive (OFF = R2)
    bool weaponKickR96=true; // R96: VrWeaponKickR96, R3 kick with a melee weapon against an enemy
    bool shieldBlockBoxR120=true; // R120: VrShieldBlockBoxR120, shield presses block only inside its own box
    bool hideBackWeaponsR121=true; // R121: VrHideBackWeapons, bow/crossbow/shield/quiver on the back hidden in normal play (was INI only)
    bool hideAtStationsR121=true; // R121: VrHideStowedAtStations, every stowed weapon hidden at alchemy/smithing (was INI only)
    bool eyeRayR121=true; // R121: VrInteractionEyeRayR121, the look/selection ray starts at the eye you see from
    bool meleeGripR123=true; // R123: VrMeleeAuthoredGripR123, sword/axe/mace held at the weapon's own hand socket (no learned clip pose)
    bool hudInfinityR125=false; // R125: VrHudInfinityR125, HUD at optical infinity (one quad per eye, zero disparity)
    bool combatCrossR125=true; // R160 UI: default true = his INI (was false). R125: VrCombatCrossR125, false = combat direction cross art hidden, lock/perfect-block/master-strike icons stay
    bool hideTutorialsR141=false; // R141: VrHideTutorialsR141, true = tutorial popups off (the game's wh_ui_TutorialsEnabled held 0)
    bool minimalHudR141=false; // R141: VrMinimalHudR141, only prompts, dialogue/subtitles, quest text (R153) and the palm compass/health; the rest stays hidden
    bool propImpactSoundR124=true; // R124: VrPropImpactSoundR124, our loose props play the game's put-down sound when they hit something
    bool sceneryPropSwapR100=true; // R100: VrSceneryPropSwapR100, level-brush props become loose copies on Grip
    bool propFirmHoldR116=true; // R116: VrPropFirmHoldR116, held small props follow the hand every frame (no weight/drag budget)
    bool sceneryBigPropsR158=false; // R158: VrSceneryBigPropsR158, scenery props up to 1 m across (false = 65 cm, R89/R100)
    bool cinematicBarsR107=true; // R107: VrCinematicBarsR107, cutscene/dialogue screen shows only the 16:9 band
    bool lockpickOpaqueR141=true; // R141: VrLockpickOpaqueR141, lockpicking overlay shown solid in the headset (HUD layer alpha pass, only while the lock is open)
    bool eyeHoldR141=true; // R141: VrEyeHoldR141, a frame without a new eye image re-shows the last one (not black)
    bool rideFollowXR147=true; // R147: VrRideFollowXR147, a short X tap (PS VR2 Square) takes the follow prompt (LB); R155: on foot too
    int tutorialNativeHitsR159=0; // R162: default 0 OFF (GAME TIMING gave 0 chained follow-ups in VR, REVIEW162_tut). 1 NATIVE HITS (old R159), 2 GAME TIMING. R159: VrTutorialNativeHitsR159, practice / quest weapon in hand (tutorial fight) = no mod direct hit, native route only
    bool trainingVanillaNpcR163=true; // R163: VrTrainingVanillaNpcR163, practice / quest weapon in hand (training fight) = no mod write to any NPC (AI, animation, action timing, global NPC cvars)
    bool impaleFullDepthR128=true; // R128: VrImpaleFullDepthR128, killing-stab hang goes in to the guard (plate on the zone = shallow) + own blade passes the hung corpse
    bool impaleFastDeathR106=true; // R106: VrImpaleFastDeathR106, the impaled man's death clip plays 3x until the ragdoll pin
    bool npcLifeD73=true;
    bool npcGazeHmdD52=true,npcGazeReadyD52=false,npcGazeSceneD52=false;
    bool fistStrokeD49=true; // Claude peak-stroke comparison; R124 default ON (Miki's INI).
    bool fingerTrial=false,alchemyEyeTrial=false,gripStowTrial=false;
    int gripStowButton=0;
    bool pickupNearOnlyD40=false;
    bool combatNoStepD40=true,combatNoTurnD40=true,combatNoTurnReadyD40=false,clinchReadyD40=false;
    bool freeTargetingD42=true,freeTargetingContextD42=false,freeTargetingLeaseD42=false;
    int freeTargetingModeR133=1; // R133: 0 NATIVE, 1 FREE, 2 FREE PLUS (VrFreeTargetingModeR133); freeTargetingD42 = mode!=0
    bool freeTargetingHoldR133=false; // R133 status: FREE PLUS view offset is live right now
    int clinchModeD40=2;
    bool labLivingTouchD40=false,labLivingImpactD40=false,labCorpseDragD40=false;
    bool npcContactReactionD41=true,npcStaminaReactionD41=true,finisherCandidateD41=true;
    bool propStatusKnownD40[2]{};
    unsigned propStateD40[2]{};
};
enum class Group:unsigned {Body,Hands,Alchemy,Smith,Gear,View,Objects,Npc,Combat,RepairsD65,MotionD67,CraftD67,Count};
enum class Id:unsigned {
    None,BodyFollow,Roomscale,HeadHeight,SmoothMove,Turn,ShoulderWarning,
    HandsMode,HandSeparation,NativeHandPhysics,WallSlide,GrabHand,HandSmoothing,FingerTrial,
    AlchemyHands,AlchemyMode,AlchemyPickupReach,WorkstationGestures,CraftReach,AlchemyEyeTrial,
    SmithHands,SmithMode,SmithCollision,AnvilMode,SmithAutoHold,StationTorso,StationClearance,
    ShoulderLayout,ChestDagger,WeaponRelease,ThrowStock,QuickTalk,TorchGesture,VisorGesture,GripStowTrial,
    ParchmentMap,MapRegion,CameraHold,BowArmAim,HideBow,F9,PropWake,
    PickupReachD40,PropRightD40,PropLeftD40,CombatNoStepD40,CombatNoTurnD40,CombatTurnStatusD40,
    ClinchModeD40,ClinchDistanceD40,NpcTouchD40,LivingTouchD40,LivingImpactD40,CorpseDragD40,ClinchStatusD40,
    NpcContactReactionD41,NpcStaminaReactionD41,FinisherCandidateD41,FreeTargetingD42,ArrowHoldD48,ElbowSolverD48,FistStrokeD49,NativePairedBackupD51,NpcGazeD52,PickupCameraBridgeD55,AlchemyToolContactD55,
    PetGestureD59,WashGestureD59,AlchemyTransferD59,TimingLocationD64,MapSizeD64,MapSupportD64,TwoHandBiasD65,TwoHandPrimaryBiasD65,BowAimD65,CrossbowReloadD65,AlchemySpeedD65,PickupClockD65,CrankTravelD66,KnifeRightHipD66,GripAnchorD67,ArcCoverageD67,SupportReleaseD67,ContactFairnessD67,ElbowContinuityD67,MotionTrialD67,ElbowOutwardD67,ElbowBackD67,SmithRecoveryD67,SmithSemiD67,HandLeadD69,HandLeadMsD69,WeaponSameFrameD69,RigidGripD69,ElbowHysteresisD69,StabFixD77,UnarmedFixD77,MasterstrikeFixD77,FinisherUpperPlayerD83,FinisherUpperNpcD83,BladeZoneD84,UpCutLowD84,StabStartD84,HudWalkoffD70,LeanHeightD70,DodgeWindowD70,ContactContinuityD71,ItemHandoverD71,InstantActivitiesD72,InstantInteractionsD72,InstantCraftingD72,InstantReloadsD72,InstantCombatD72,MapNativePaperM43,NpcLifeD73,NativeRoomscaleD76,NpcListeningD76,RidingVrikD76,DiceHandsD85,StaticGrabR89,CrossbowHandOnlyR90,CorpseSqueezeSearchR90,TwoHandWeightR90,WeaponKickR96,SceneryPropSwapR100,ImpaleFastDeathR106,CinematicBarsR107,EyeSrgbR114,KnifeChestZoneR109,HeavyCrossbowHandPullR111,VrikOnStartR111,StartHenryEyesR111,QuickSlotFadeR112,AlchemyViewSelectR116,PropFirmHoldR116,SwingWhooshR116,MediumCrossbowHandPullR117,ControllerLayoutR118,QuickAccessFaceTouchR118,ShieldBlockBoxR120,HideBackWeaponsR121,HideAtStationsR121,EyeRayR121,MeleeGripR123,PropImpactSoundR124,HudInfinityR125,CombatCrossR125,ImpaleFullDepthR128,MinimalHudR141,HideTutorialsR141,LockpickOpaqueR141,EyeHoldR141,RideFollowXR147,SceneryBigPropsR158,TutorialNativeHitsR159,TrainingVanillaNpcR163
};
struct Row {Id id;const wchar_t* title;const wchar_t* detail;bool session=false,readOnly=false;};
inline constexpr std::array<std::array<Row,8>,12> kRows{{
    {{{Id::BodyFollow,L"6DOF body follows head",L"Your torso follows your head; wrists stay tracked."},
      {Id::Roomscale,L"Roomscale movement",L"Physical steps, left/right/back headset dodges, both, or neither."},
      {Id::HeadHeight,L"Body height",L"HMD, native, or HMD with native height on horse and in crouch."},
      {Id::SmoothMove,L"Body movement smoothing",L"Smooth body movement without filtering your view."},
      {Id::Turn,L"Right-stick turn",L"Right stick turns in steps or continuously."},
      {Id::ShoulderWarning,L"Shoulder travel warning",L"Warning distance only; does not limit your reach."},
      {Id::CinematicBarsR107,L"Cutscene 16:9 bars",L"Cutscenes and native dialogues show only the 16:9 band: nothing above/below the bars. OFF: whole frame."}, // R107
      {Id::KnifeChestZoneR109,L"Knives: chest pouch too",L"Empty left hand + Grip at the chest (where the dagger shows) draws a knife, same as at the left hip. OFF: left hip only."}}}, // R109
    {{{Id::HandsMode,L"Free-hand contact detail",L"Balanced: palm and two tips, without finger bending."},
      {Id::HandSeparation,L"Hand-to-hand collision",L"Prevent empty hands from passing through each other."},
      {Id::NativeHandPhysics,L"Native hand bodies",L"Trial: native hand bodies at verified physics hooks."},
      {Id::WallSlide,L"Held-weapon wall slide",L"Allow held weapons to slide along world surfaces."},
      {Id::GrabHand,L"Grab hand stays on controller",L"Keep your wrist tracked while grabbing an object."},
      {Id::MotionTrialD67,L"Hand presentation timing",L"Baseline, fresh pose capped, or fresh pose uncapped."},
      {Id::FingerTrial,L"Finger contact trial",L"Temporary finger curl on contact; resets on restart.",true},
      {Id::ElbowSolverD48,L"Elbow preference",L"Default, hand-guided, FRIK, or elbow reach preference."}}},
    {{{Id::AlchemyHands,L"Tracked alchemy hands",L"Use tracked hands at the alchemy bench."},
      {Id::AlchemyMode,L"Alchemy work mode",L"Native, guided, gestures, or free 6DOF crafting."},
      {Id::AlchemyPickupReach,L"Alchemy pickup reach",L"Free mode uses the palm ray; other modes assist."},
      {Id::WorkstationGestures,L"Legacy work gestures",L"Older gesture route outside explicit crafting modes."},
      {Id::CraftReach,L"Craft reach trial",L"Optional reach check; recipe rules still apply."},
      {Id::AlchemyEyeTrial,L"Legacy alchemy head trial",L"Compare the older head-selection experiment."},
      {Id::AlchemyToolContactD55,L"Alchemy tool contact trial",L"World contact for tools in free 6DOF alchemy."},
      {Id::AlchemyTransferD59,L"Alchemy release handoff trial",L"Keep a valid drop until the game can accept it."}}},
    {{{Id::SmithHands,L"Tracked smithing hands",L"Use tracked hands at the smithing station."},
      {Id::SmithMode,L"Smithing work mode",L"Native, guided, gestures, or free 6DOF crafting."},
      {Id::SmithCollision,L"Smith world tool collision",L"Hammer, tongs and held workpiece touch the world."},
      {Id::AnvilMode,L"Anvil support",L"Support from the anvil bounds or learned surface."},
      {Id::SmithAutoHold,L"Smith tools follow hands",L"Keep held tools attached to your tracked hands."},
      {Id::StationTorso,L"Station torso follows head",L"Torso follows upright steps while allowing a lean."},
      {Id::StationClearance,L"Station chest clearance",L"Move your chest back; your view stays unchanged."},
      {Id::AlchemyViewSelectR116,L"Alchemy: select what you look at",L"Picks the item your headset looks at, or the one your hand reaches. OFF: R115 selection."}}}, // R116
    {{{Id::ShoulderLayout,L"Shoulder holster layout",L"Bow: right shoulder, drawn with the left hand."},
      {Id::ChestDagger,L"Carried chest dagger display",L"Show the carried dagger, separate from thrown knives."},
      {Id::WeaponRelease,L"Weapon grip release",L"Allow drops or throws when the game permits them."},
      {Id::ThrowStock,L"Knives: hip / chest pouch",L"Empty left hand, Grip at your left hip or chest draws an inventory knife; release away from the pouch throws it."},
      {Id::QuickTalk,L"Index-finger pointing Quick Talk",L"Point your index finger while looking at a person or dog; keep pointing to choose a command."},
      {Id::TorchGesture,L"Torch shoulder gesture",L"Left shoulder + Trigger; Grip may stay closed."},
      {Id::VisorGesture,L"Visor temple gesture",L"Empty left hand at temple: Trigger, then up or down."},
      {Id::TwoHandWeightR90,L"Two-hand weight feel",L"Weapon weight slider also slows two-hand turns. OFF: fixed two-hand heft."}}}, // R90
    {{{Id::ParchmentMap,L"Static paper map",L"Right thumbrest + left stick up opens; B closes."},
      {Id::MapSizeD64,L"Paper map size",L"Resize the paper without stretching the artwork."},
      {Id::CameraHold,L"Brief missing-body-pose hold",L"Brief missing body-base hold; zero disables it."},
      {Id::BowArmAim,L"Bow arm-line aim",L"Legacy saved option; no active aim writer uses it."},
      {Id::HideBow,L"Stowed bow visibility",L"Stowed bow only; used when GEAR page 2 \"Hide weapons on your back\" is OFF."},
      {Id::ArrowHoldD48,L"Held arrow orientation",L"Choose wrist grip or the native arrow rotation."},
      {Id::MapNativePaperM43,L"Map performance trial",L"Try the native paper renderer; reopen the map."},
      {Id::MapSupportD64,L"Map second-hand support",L"Hold the opposite edge with an empty left hand."}}},
    {{{Id::PickupReachD40,L"Item pickup reach",L"Direct pickup: 20 cm; assisted pickup: 25 cm."},
      {Id::F9,L"Physical interaction session",L"Optional weapon contact or NPC physics lab for this session. Starts OFF.",true},
      {Id::PropWake,L"Small-object wake lab",L"Lab: wake selected loose objects, not scenery."},
      {Id::GripStowTrial,L"Held-object stow button",L"While holding a prop, press this button to stow it."},
      {Id::PropRightD40,L"Right physical prop",L"Shows the current right-hand physical grab state.",false,true},
      {Id::PropLeftD40,L"Left physical prop",L"Shows the current left-hand physical grab state.",false,true},
      {Id::PetGestureD59,L"Dog petting",L"Stroke the dog with an open, empty hand."},
      {Id::WashGestureD59,L"Face washing gesture trial",L"Trial: dip open hands in a tub, then raise to face."}}},
    {{{Id::ClinchModeD40,L"Player clinch initiation",L"Grip with Trigger released; NPC clinches stay native."},
      {Id::ClinchDistanceD40,L"Player clinch distance",L"Your clinch range; zero uses the game default."},
      {Id::NpcListeningD76,L"NPC listening gestures",L"Native listening idles while Henry speaks or chooses a reply."},
      {Id::NpcLifeD73,L"NPC gaze and face idles",L"Saved preference; conversation NPCs follow the headset and play native idles."},
      {Id::LivingImpactD40,L"Accepted-hit reaction lab",L"Unavailable: no extra skeletal impact is added.",false,true},
      {Id::CorpseDragD40,L"Physical corpse dragging",L"Try grabbing confirmed dead bodies with an empty hand. F9 records failures."},
      {Id::ClinchStatusD40,L"Clinch control",L"Shows whether player clinch control is available.",false,true},
      {Id::NpcGazeD52,L"Dialogue NPC gaze",L"Follow your HMD during an active conversation."}}},
    {{{Id::CombatNoStepD40,L"Attack step veto",L"Stop automatic attack steps; normal walking stays."},
      {Id::CombatNoTurnD40,L"Attack auto-turn veto",L"Stop ordinary attack turns; paired scenes stay native."},
      {Id::NativePairedBackupD51,L"Native paired backup",L"Allow native finisher and masterstrike animations."},
      {Id::NpcContactReactionD41,L"Hit reaction by body region",L"Choose native reaction from the accepted hit region."},
      {Id::NpcStaminaReactionD41,L"Include stamina-only hits",L"Include armour hits, but never blocked hits."},
      {Id::FinisherCandidateD41,L"Combo finish requirement",L"Final contact rules; native combo/RPG checks remain."},
      {Id::FreeTargetingD42,L"Free targeting",L"GAME LOCK / FREE / FREE PLUS: plus = the game never turns your view; stick walks where you look."},
      {Id::FistStrokeD49,L"Punch stroke trial",L"Trial: punch peak speed and reversal recognition."}}},
    {{{Id::PickupCameraBridgeD55,L"Pickup camera handover trial",L"Keep the tracked view during brief pickup handover."},
      {Id::TwoHandBiasD65,L"Two-hand primary influence trial",L"Let the primary hand influence two-hand direction."},
      {Id::TwoHandPrimaryBiasD65,L"Additional primary-hand influence",L"More primary-hand steering of the weapon shaft."},
      {Id::BowAimD65,L"Physical bow shot direction trial",L"Use the released bow pose for the matching shot."},
      {Id::CrossbowReloadD65,L"Hand-driven crossbow reload",L"Pull the string or tool, take a bolt from the quiver, then place it on the rail."},
      {Id::HeavyCrossbowHandPullR111,L"Heavy crossbow: pull string by hand",L"Grip the string at the front, pull it back: cocked. No crank. OFF: old crank route."}, // R111
      {Id::MediumCrossbowHandPullR117,L"Medium crossbow: pull string by hand",L"Goat's-foot crossbow: grip the string at the front, pull it back. No spanner. OFF: spanner from chest."}, // R117
      {Id::CrankTravelD66,L"Heavy crossbow crank travel",L"Old crank route only (hand pull OFF): forward travel to cock."}}},
    {{{Id::SwingWhooshR116,L"Swing sound with your swing",L"Whoosh plays when you release the cut/stab, the game's late one is skipped. OFF: game whoosh only."}, // R116
      {Id::ElbowContinuityD67,L"Continuous elbow target",L"Avoid torso-search elbow jumps; keep tracked wrists."},
      {Id::ElbowOutwardD67,L"Elbow trial: outward preference",L"Elbow trial: prefer elbows farther from your sides."},
      {Id::ElbowBackD67,L"Elbow trial: backward preference",L"Elbow trial: prefer elbows farther behind your hands."},
      {Id::GripAnchorD67,L"Grip anchored in free space",L"Keep the grip fixed in air; yield on real contact."},
      {Id::SupportReleaseD67,L"Two-hand release continuity",L"Prevent angle snaps when the support hand lets go."},
      {Id::ArcCoverageD67,L"Current weapon arc coverage",L"Use the existing sweep budget for the current arc."},
      {Id::ContactFairnessD67,L"Free-hand contact budget fairness",L"Give the older hand first use of the contact budget."}}},
    {{{Id::AlchemySpeedD65,L"Alchemy transfer speed",L"Faster pickup and return. Book, pouring and station movement keep native speed."},
      {Id::AlchemyToolContactD55,L"Alchemy tool contact",L"World contact for tools in free 6DOF alchemy."},
      {Id::AlchemyTransferD59,L"Alchemy release handoff",L"Keep a valid drop until the game can accept it."},
      {Id::SmithRecoveryD67,L"Smith stroke recovery",L"Keep a deliberate hammer lift through stroke recovery."},
      {Id::SmithSemiD67,L"Smith LT anvil assist",L"Hold left Trigger: native anvil pose. Release: free hand."},
      {Id::PickupClockD65,L"Faster native pickup handover trial",L"Speed accepted pickup clips; keep native item rules."}, // R117: moved from the crossbow page; it replaces the DUPLICATE smith collision row (still on the SMITH page)
      {Id::AnvilMode,L"Anvil support mode",L"Support from the anvil bounds or learned surface."},
      {Id::SmithAutoHold,L"Smith tools follow hands",L"Keep held tools attached to your tracked hands."}}}
}};
// Second VRIK page: explicit comparisons, independent of the accepted D67 controls.
inline constexpr std::array<Row,8> kMotionTrialsD69{{
    {Id::HandLeadD69,L"Extra hand prediction trial",L"Extra prediction beyond OpenXR; optional comparison."},
    {Id::HandLeadMsD69,L"Extra prediction time",L"Extra prediction time; requires the switch above."},
    {Id::WeaponSameFrameD69,L"Same-cycle weapon placement trial",L"Place weapon from the completed hand-pose cycle."},
    {Id::RigidGripD69,L"Rigid primary grip trial",L"Trial: fixed melee grip with tip weight and resistance."},
    {Id::ElbowHysteresisD69,L"Elbow pole hysteresis trial",L"Keep elbow bend direction stable across small moves."},
    {Id::HudWalkoffD70,L"HUD event-only updates",L"Update HUD on changes instead of unchanged work."},
    {Id::TimingLocationD64,L"F2 comparison boundary",L"Only the F2 timing comparison uses this location."}, // R163: moved here from VRIK page 2 row 4; replaces the REPEATED Forward lean height row (it stays on BODY page 2 row 2)
    {Id::DodgeWindowD70,L"Global native dodge window",L"Native dodge window for ALL actors; zero is default."}
}};
inline constexpr std::array<Row,8> kCombatTrialsD70{{
    {Id::StabFixD77,L"Stab follow-through trial",L"Keep a committed stab while both tips advance."},
    {Id::UnarmedFixD77,L"Unarmed clinch trial",L"Limit unarmed clinch initiation to 30 cm."},
    {Id::MasterstrikeFixD77,L"Masterstrike reaction trial",L"Try stab-reaction families; other attacks stay normal."},
    {Id::FinisherUpperPlayerD83,L"Your finishers / masterstrikes",L"Turn your head during the native move; arms and weapon stay native."}, // D83 finisher
    {Id::FinisherUpperNpcD83,L"NPC finishers / combos on you",L"Turn your head during the native move; arms and weapon stay native."}, // D83 finisher
    {Id::BladeZoneD84,L"Attack direction",L"Native attack side follows where the blade swings, not where the hand was held."}, // D84 blade zone
    {Id::UpCutLowD84,L"Cut from below",L"An upward cut counts as the low attack, for combos."}, // D84 up cut
    {Id::StabStartD84,L"Stab direction",L"Side from where the stab starts: above, left, right, chest/below."} // D84 stab start
}};
inline constexpr std::array<Row,8> kHandTrialsD71{{
    {Id::ContactContinuityD71,L"Contact stability trial",L"Keep contact surfaces stable; no added hand filter."},
    {Id::ItemHandoverD71,L"First object placement trial",L"Use the current grip for first supported attachment."},
    {Id::HandSmoothing,L"Extra hand filtering",L"Legacy low-pass filter; separate from fresh-pose timing."},
    {Id::TrainingVanillaNpcR163,L"Training fights: vanilla NPCs",L"Wooden / quest sword in hand: no mod feature touches any NPC (AI, animations, timing, look). Your own VR stays."}, // R163: next to the R159 row; its old row "F2 comparison boundary" moved to VRIK MOTION page 2 (replaces the REPEATED Forward lean height row, still on BODY page 2)
    {Id::TutorialNativeHitsR159,L"Tutorials (training fights)",L"Wooden / quest sword in hand: GAME TIMING = the game's own attack speed, so the partner can block; NATIVE HITS = no mod direct hit."}, // R159: replaces the REPEATED Loose scenery props row (it stays on OBJECTS page 3)
    {Id::ImpaleFastDeathR106,L"Impale: fast death",L"Killing stab: his death animation runs 3x so the ragdoll (and the hang on your blade) comes at once."}, // R106
    {Id::WeaponKickR96,L"Weapon kicks vs enemies",L"R3 with a sword/axe/mace/polearm in a fight kicks. OFF: old heavy R3 press (often a stab)."}, // R96
    {Id::CorpseSqueezeSearchR90,L"Corpse search",L"Always: catches fast grabs. On squeeze: no idle FPS cost, a fast grab can miss."} // R90
}};
inline constexpr std::array<Row,8> kInstantActivitiesD72{{
    {Id::InstantActivitiesD72,L"INSTANT activities",L"Enable the instant action groups selected below."},
    {Id::InstantInteractionsD72,L"INSTANT interactions",L"Physical pickup and release for supported objects."},
    {Id::InstantCraftingD72,L"INSTANT crafting",L"Supported crafting actions follow your gestures."},
    {Id::InstantReloadsD72,L"Fast reload transitions",L"Native transition speed only; hands still own the string and bolt."},
    {Id::InstantCombatD72,L"INSTANT masterstrikes / combos",L"Game grants the master strike, your blade hits (no paired clip). Physical finishers; native defenses and costs stay."},
    {Id::DiceHandsD85,L"Dice: point to choose (trial)",L"Point the right hand at a die, A keeps it. Cup stays native."}, // D85 dice
    {Id::StaticGrabR89,L"Small props in the hand",L"Touch a mug/plate/candle, close Grip: carry, place, throw."}, // R90 (R89 pickup)
    {Id::CrossbowHandOnlyR90,L"Crossbow reload route",L"Hand only: your hands cock and load (bow pattern). Native: game reload, hands ride it."} // R96 restored
}};
struct Rect {int l,t,r,b;};
constexpr bool inside(Rect r,int x,int y) noexcept {return x>=r.l&&x<r.r&&y>=r.t&&y<r.b;}
inline constexpr std::array<Rect,12> kTabs{{{620,116,686,142},{690,116,756,142},{760,116,826,142},{830,116,896,142},{900,116,966,142},{970,116,1038,142},{620,146,686,172},{690,146,756,172},{760,146,826,172},{830,146,896,172},{900,146,966,172},{970,146,1038,172}}};
constexpr Rect rowRect(unsigned n) noexcept {return {620,182+48*int(n),1038,208+48*int(n)};}
constexpr Rect detailRect(unsigned n) noexcept {return {620,209+48*int(n),1038,229+48*int(n)};}
inline constexpr Rect kTrials{620,575,754,605},kCombat{762,575,896,605},kCrafting{904,575,1038,605};
inline constexpr std::array<Row,8> kBodyTrialsD76{{
    {Id::NativeRoomscaleD76,L"Physical walking 1:1 trial",L"Same walking choice as Roomscale movement; keep 6DOF body on."},
    {Id::LeanHeightD70,L"Leaning height threshold",L"Lower your head this much to lean without walking."},
    {Id::RidingVrikD76,L"Horse riding VRIK",L"Native mount/dismount; seated: upper-body VRIK at once."},
    {Id::VrikOnStartR111,L"Start in VRIK",L"VRIK is on at every game start and save load. Without a saved T-pose it uses default arms (F8 for exact arms). You can still switch it off."}, // R111, R149: also without a saved T-pose
    {Id::StartHenryEyesR111,L"Start at Henry's eye height",L"Start/load/recenter puts your view at Henry's eyes, sitting or standing. Menu screen + load face you (SteamVR)."}, // R111 + R148
    {Id::EyeSrgbR114,L"Eye colours: sRGB swapchain",L"VDXR/Quest Link colours like SteamVR (not overblown). Takes effect at the next game start."}, // R114
    {Id::PropFirmHoldR116,L"Held small props: firm grip",L"Mug/plate/jug stays locked in the hand, no weight or drag. OFF: old heavy servo."}, // R116
    {Id::QuickSlotFadeR112,L"HUD: weapon/potion slots only while selecting",L"Hidden; thumbrest + left stick left/right shows the slot 3 s, then it fades. OFF: always shown. Also on VR HUD page."} // R112
}};
// R118: GEAR page 2 = controller layout (every INI key has its Experiments switch; page 1 was full).
inline constexpr std::array<Row,8> kGearTrialsR118{{
    {Id::ControllerLayoutR118,L"Controller layout",L"OPENXR: Menu = inventory, X hold = torch. STEAMVR / PS VR2: X hold = inventory."}, // R118
    {Id::QuickAccessFaceTouchR118,L"Quick access modifier",L"Thumbrest, or touch Y/B (Triangle/Circle) on controllers without one. PS VR2 sets face touch."}, // R118 (R113 key)
    {Id::HideBackWeaponsR121,L"Hide weapons on your back",L"Bow, crossbow, shield and quiver on the back are hidden. OFF: the stowed bow row decides for the bow, the rest show."}, // R121
    {Id::HideAtStationsR121,L"Hide all weapons at crafting tables",L"At alchemy and smithing every stowed weapon is hidden, hip sword and dagger too. OFF: normal rules."}, // R121
    {Id::MeleeGripR123,L"Fixed sword grip",L"Sword, axe, mace and dagger sit in the hand at their own handle socket. OFF: learned from Henry's animated hand (old)."}, // R123
    {Id::PropImpactSoundR124,L"Prop impact sounds",L"A mug/plate/jug you drop, throw or knock makes the game's clay/wood/metal sound when it lands. OFF: silent."}, // R124
    {Id::EyeRayR121,L"Look selection from your eye",L"The centre of your view or an index-finger point selects, also in VRIK and after leaning at a table. OFF: hands only, Henry's eye."}, // R121 + R137
    {Id::ShieldBlockBoxR120,L"Shield block box",L"Shield blocks only inside its box (COMBAT > BLOCK > SHIELD BOX). OFF: old wide rule, block almost always on."} // R120
}};
// R125: GEAR page 3 = HUD look (pages 1 and 2 are full).
inline constexpr std::array<Row,8> kGearTrialsR125{{
    {Id::HudInfinityR125,L"HUD at infinity (flat, never double)",L"Each eye gets its own HUD copy: no depth, eyes stay parallel. OFF: one HUD at the VR HUD distance."}, // R125
    {Id::CombatCrossR125,L"Combat direction cross",L"OFF: the big direction arms are hidden, only lock / perfect block / master strike icons show."}, // R125
    {Id::ImpaleFullDepthR128,L"Impale: blade goes in fully",L"Killing stab: pinned only where your blade touches him, then he slides on up to the guard. Plate: shallow. OFF: old."}, // R128, R153
    {Id::LockpickOpaqueR141,L"Lockpicking lock: solid",L"While picking a lock its rings show as solid as on the monitor. OFF: old see-through look. Other HUD never changes."}, // R141
    {Id::EyeHoldR141,L"No black frames: hold last picture",L"When the game stalls or a DLSS/FSR pair is missing, the last picture stays (max 2 s) instead of a black frame. OFF: black (old)."}, // R141
    {Id::MinimalHudR141,L"Minimal HUD",L"Only prompts, dialogue, subtitles, quest text + palm compass/health. Status icons, popups, cursor hidden."}, // R141 row 6, R153 quest text kept
    {Id::HideTutorialsR141,L"Tutorial messages",L"HIDDEN: no tutorial popups (the game's own Tutorials option, held while you play). Also hidden by Minimal HUD."}, // R141 row 7
    {Id::RideFollowXR147,L"X tap = Follow",L"A short X tap (PS VR2: Square) follows the NPC when the game shows Follow (LB), on foot and on the horse (R155). OFF: X tap = talk."} // R147 row 8
}};
// R158: OBJECTS page 3 = everything about props held in the hand on one page (Miki: the size option "in 'items'").
// row 1 is the new size switch; rows 2-8 are the SAME switches as on their own pages (same Id = same setting).
// R159: row 3 (Loose scenery props) is now its only row - its VRIK page 2 slot holds "Tutorials: native hits only".
inline constexpr std::array<Row,8> kObjectsTrialsR158{{
    {Id::SceneryBigPropsR158,L"Scenery props: size",L"SMALL: mugs, plates, candles (up to 65 cm). UP TO 1 M: also stools, baskets, small crates (carried like a light box)."}, // R158
    {Id::StaticGrabR89,L"Small props in the hand",L"Touch a mug/plate/candle, close Grip: carry, place, throw."}, // same as OBJECTS page 2
    {Id::SceneryPropSwapR100,L"Loose scenery props (swap)",L"Grip a table mug/plate/candle that is level mesh: it becomes a loose prop you carry."}, // R159: its only row now (VRIK page 2 row 5 = Tutorials: native hits only)
    {Id::PropFirmHoldR116,L"Held small props: firm grip",L"Mug/plate/jug stays locked in the hand, no weight or drag. OFF: old heavy servo."}, // same as BODY page 2
    {Id::PropImpactSoundR124,L"Prop impact sounds",L"A mug/plate/jug you drop, throw or knock makes the game's clay/wood/metal sound when it lands. OFF: silent."}, // same as GEAR page 2
    {Id::InstantInteractionsD72,L"INSTANT interactions",L"Physical pickup and release for supported objects."}, // same as OBJECTS page 2
    {Id::PickupReachD40,L"Item pickup reach",L"Direct pickup: 20 cm; assisted pickup: 25 cm."}, // same as OBJECTS page 1
    {Id::GripStowTrial,L"Held-object stow button",L"While holding a prop, press this button to stow it."} // same as OBJECTS page 1
}};
inline constexpr unsigned kMotionPage2D69=static_cast<unsigned>(Group::Count),kCombatPage2D70=kMotionPage2D69+1,kHandsPage2D71=kCombatPage2D70+1,kObjectsPage2D72=kHandsPage2D71+1,kBodyPage2D76=kObjectsPage2D72+1,kGearPage2R118=kBodyPage2D76+1,kGearPage3R125=kGearPage2R118+1,kObjectsPage3R158=kGearPage3R125+1;
inline constexpr Rect kMotionPrevD69{694,88,782,112},kMotionNextD69{946,88,1038,112};
constexpr Group group(unsigned n) noexcept {return n==kObjectsPage3R158?Group::Objects:n==kGearPage3R125||n==kGearPage2R118?Group::Gear:n==kBodyPage2D76?Group::Body:n==kObjectsPage2D72?Group::Objects:n==kHandsPage2D71?Group::Hands:n==kCombatPage2D70?Group::Combat:n==kMotionPage2D69?Group::MotionD67:n<static_cast<unsigned>(Group::Count)?Group(n):Group::Body;}
constexpr bool motionPage2D69(unsigned n) noexcept {return n==kMotionPage2D69;}
constexpr const std::array<Row,8>& rows(unsigned n) noexcept {return n==kObjectsPage3R158?kObjectsTrialsR158:n==kGearPage3R125?kGearTrialsR125:n==kGearPage2R118?kGearTrialsR118:n==kBodyPage2D76?kBodyTrialsD76:n==kObjectsPage2D72?kInstantActivitiesD72:n==kHandsPage2D71?kHandTrialsD71:n==kCombatPage2D70?kCombatTrialsD70:motionPage2D69(n)?kMotionTrialsD69:kRows[unsigned(group(n))];}
inline constexpr const wchar_t* kTabsText[]{L"BODY",L"VRIK",L"ALCHEMY",L"SMITH",L"GEAR",L"VIEW",L"OBJECTS",L"NPCS",L"COMBAT",L"REPAIRS",L"VRIK+",L"CRAFT+"};
constexpr bool layoutValid() noexcept {
    for(unsigned i=0;i<kTabs.size();++i){
        if(kTabs[i].l<620||kTabs[i].r>1038)return false;
        for(unsigned j=0;j<i;++j)if(kTabs[i].l<kTabs[j].r&&kTabs[j].l<kTabs[i].r&&kTabs[i].t<kTabs[j].b&&kTabs[j].t<kTabs[i].b)return false;
    }
    return kMotionPrevD69.l>682&&kMotionPrevD69.b<kTabs.front().t&&kMotionNextD69.l>kMotionPrevD69.r&&kMotionNextD69.b<kTabs.front().t&&kMotionNextD69.r<=1038&&kTabs.back().b<rowRect(0).t&&detailRect(7).b<kTrials.t&&kTrials.b<672;
}
static_assert(layoutValid(),"D39 controls must fit the menu clip without overlap");
constexpr int cycle(int value,int count) noexcept {return value>=0&&value<count?(value+1)%count:0;}
// R118: the real VrMenuState has the R113 modifier field; portable test views may not (C++17, no requires).
template<class S,class=void> struct HasFaceTouchR118:std::false_type{};
template<class S> struct HasFaceTouchR118<S,std::void_t<decltype(std::declval<S&>().quick_access_face_touch_r112)>>:std::true_type{};
template<class S,class=void> struct HasLeftThumbrestR143:std::false_type{};
template<class S> struct HasLeftThumbrestR143<S,std::void_t<decltype(std::declval<S&>().quick_access_left_thumbrest_r143)>>:std::true_type{};
template<class S,class=void> struct HasMsModeR120:std::false_type{};
template<class S> struct HasMsModeR120<S,std::void_t<decltype(std::declval<S&>().exp_ms_stab_only47)>>:std::true_type{};
template<std::size_t N> inline float next(float value,const std::array<float,N>& values) noexcept {
    if(!std::isfinite(value))return values.front();
    for(float candidate:values)if(candidate>value+.0001f)return candidate;
    return values.front();
}
// Existing menu fields are reused, rather than a second settings shadow that
// overwrites changes made on Controls, Immersion or Crafting pages.
template<class S> bool advance(S& s,Id id) noexcept {
    auto& d=s.live_d39;
    switch(id){
    case Id::InstantActivitiesD72:d.instantActivitiesD72=!d.instantActivitiesD72;break;
    case Id::InstantInteractionsD72:d.instantInteractionsD72=!d.instantInteractionsD72;break;
    case Id::InstantCraftingD72:d.instantCraftingD72=!d.instantCraftingD72;break;
    case Id::InstantReloadsD72:d.instantReloadsD72=!d.instantReloadsD72;break;
    case Id::InstantCombatD72:d.instantCombatD72=!d.instantCombatD72;
        // R120: the switch says "INSTANT masterstrikes", so it now picks the INSTANT master strike (gate mode 3, MISC 2 row);
        // OFF from INSTANT goes back to the native cut/stab gate (1). the MISC row can still choose any of the 4 by hand.
        if constexpr(HasMsModeR120<S>::value){if(d.instantCombatD72)s.exp_ms_stab_only47=3;else if(s.exp_ms_stab_only47==3)s.exp_ms_stab_only47=1;}
        break;
    case Id::DodgeWindowD70:d.dodgeWindowD70=next(d.dodgeWindowD70,std::array<float,10>{0.f,1.f,1.25f,1.5f,1.75f,2.f,2.25f,2.5f,2.75f,3.f});break;
    case Id::ContactContinuityD71:d.contactContinuityD71=!d.contactContinuityD71;break;
    case Id::ItemHandoverD71:d.itemHandoverD71=!d.itemHandoverD71;break;
    case Id::StabFixD77:d.stabFixD77=!d.stabFixD77;break;
    case Id::UnarmedFixD77:d.unarmedFixD77=!d.unarmedFixD77;break;
    case Id::MasterstrikeFixD77:d.masterstrikeFixD77=!d.masterstrikeFixD77;break;
    case Id::FinisherUpperPlayerD83:d.finisherUpperPlayerD83=!d.finisherUpperPlayerD83;break;
    case Id::FinisherUpperNpcD83:d.finisherUpperNpcD83=!d.finisherUpperNpcD83;break;
    case Id::BladeZoneD84:d.bladeZoneD84=!d.bladeZoneD84;break; // D84 blade zone
    case Id::UpCutLowD84:d.upCutLowD84=!d.upCutLowD84;break;
    case Id::StabStartD84:d.stabStartD84=!d.stabStartD84;break;
    case Id::HudWalkoffD70:d.hudWalkoffD70=!d.hudWalkoffD70;break;
    case Id::LeanHeightD70:d.leanHeightD70=next(d.leanHeightD70,std::array<float,7>{0.f,.02f,.04f,.06f,.08f,.10f,.12f});break;
    case Id::HandLeadD69:d.handLeadD69=!d.handLeadD69;break;
    case Id::HandLeadMsD69:d.handLeadMsD69=next(d.handLeadMsD69,std::array<float,8>{0.f,3.f,6.f,8.f,11.f,16.f,22.f,33.f});break;
    case Id::WeaponSameFrameD69:d.weaponSameFrameD69=!d.weaponSameFrameD69;break;
    case Id::RigidGripD69:d.rigidGripD69=!d.rigidGripD69;break;
    case Id::ElbowHysteresisD69:d.elbowHysteresisD69=!d.elbowHysteresisD69;break;
    case Id::GripAnchorD67:d.gripAnchorD67=!d.gripAnchorD67;break;
    case Id::ArcCoverageD67:d.arcCoverageD67=!d.arcCoverageD67;break;
    case Id::SupportReleaseD67:d.supportReleaseD67=!d.supportReleaseD67;break;
    case Id::ContactFairnessD67:d.contactFairnessD67=!d.contactFairnessD67;break;
    case Id::ElbowContinuityD67:d.elbowContinuityD67=!d.elbowContinuityD67;break;
    case Id::MotionTrialD67:d.motionTrialD67=d.motionTrialD67==0?2:d.motionTrialD67==2?4:0;break;
    case Id::ElbowOutwardD67:d.elbowOutwardD67=next(d.elbowOutwardD67,std::array<float,6>{.1f,.2f,.3f,.35f,.45f,.6f});break;
    case Id::ElbowBackD67:d.elbowBackD67=next(d.elbowBackD67,std::array<float,6>{.1f,.2f,.25f,.35f,.45f,.6f});break;
    case Id::SmithRecoveryD67:d.smithRecoveryD67=!d.smithRecoveryD67;break;
    case Id::SmithSemiD67:d.smithSemiD67=!d.smithSemiD67;break;
    case Id::BodyFollow:s.vrik_body_follow=!s.vrik_body_follow;break;
    case Id::Roomscale:s.roomscale_mode=cycle(s.roomscale_mode,4);d.nativeRoomscaleD76=s.roomscale_mode==1||s.roomscale_mode==2;break;
    case Id::HeadHeight:s.body_height_mode98=native_crouch_r93::next_body_height_r98(s.body_height_mode98);break;
    case Id::SmoothMove:s.smooth_move51=!s.smooth_move51;break;
    case Id::Turn:s.vr_turn_mode=cycle(s.vr_turn_mode,2);break;
    case Id::ShoulderWarning:d.shoulderWarningM=next(d.shoulderWarningM,std::array<float,7>{.02f,.05f,.10f,.15f,.20f,.30f,.40f});break;
    case Id::HandsMode:d.handsMode=cycle(d.handsMode,3);s.precise_contacts44=d.handsMode==1;break;
    case Id::HandSeparation:d.handSeparation=!d.handSeparation;break;
    case Id::NativeHandPhysics:d.nativeHandPhysics=!d.nativeHandPhysics;break;
    case Id::WallSlide:s.exp_wall_slide=!s.exp_wall_slide;break;
    case Id::GrabHand:s.grab_hand_stays43=!s.grab_hand_stays43;break;
    case Id::HandSmoothing:d.handSmoothing=!d.handSmoothing;break;
    case Id::FingerTrial:d.fingerTrial=!d.fingerTrial;break;
    case Id::AlchemyHands:s.activity_hands59c7f[0]=!s.activity_hands59c7f[0];break;
    case Id::AlchemyMode:s.alchemy_mode67=cycle(s.alchemy_mode67,4);break;
    case Id::AlchemyPickupReach:if(s.alchemy_mode67==3)return false;d.alchemyPickupReach=cycle(d.alchemyPickupReach,2);break;
    case Id::WorkstationGestures:d.workstationGestures=!d.workstationGestures;break;
    case Id::CraftReach:s.craft_trial_h41=!s.craft_trial_h41;break;
    case Id::AlchemyEyeTrial:d.alchemyEyeTrial=!d.alchemyEyeTrial;break;
    case Id::SmithHands:s.activity_hands59c7f[1]=!s.activity_hands59c7f[1];break;
    case Id::SmithMode:s.smithing_mode67=cycle(s.smithing_mode67,4);break;
    case Id::SmithCollision:d.smithToolCollision=!d.smithToolCollision;break;
    case Id::AnvilMode:s.smith_anvil_mode_a3=cycle(s.smith_anvil_mode_a3,3);break;
    case Id::SmithAutoHold:s.smith_auto_hold36=!s.smith_auto_hold36;break;
    case Id::StationTorso:d.stationTorso=!d.stationTorso;break;
    case Id::StationClearance:d.stationClearCm=next(d.stationClearCm,std::array<float,8>{0,5,10,12,15,20,25,30});break;
    case Id::ShoulderLayout:s.user_shoulder_holsters=!s.user_shoulder_holsters;break;
    case Id::ChestDagger:s.chest_dagger54=!s.chest_dagger54;break;
    case Id::WeaponRelease:s.weapon_release_mode_t1=cycle(s.weapon_release_mode_t1,2);break;
    case Id::ThrowStock:s.throwing_supply_t2=!s.throwing_supply_t2;break;
    case Id::KnifeRightHipD66:return false; // right hip remains retired
    case Id::QuickTalk:s.gesture_talk=!s.gesture_talk;break;
    case Id::TorchGesture:d.torchGesture=!d.torchGesture;break;
    case Id::VisorGesture:d.visorGesture=!d.visorGesture;break;
    case Id::GripStowTrial:if(!d.gripStowTrial){d.gripStowTrial=true;d.gripStowButton=0;}else if(d.gripStowButton<2)++d.gripStowButton;else d.gripStowTrial=false;break;
    case Id::ParchmentMap:d.parchmentMap=!d.parchmentMap;break;
    case Id::MapRegion:return false; // Static paper selects current native location.
    case Id::CameraHold:d.cameraHoldMs=int(next(float(d.cameraHoldMs),std::array<float,7>{0,100,250,500,1000,1500,5000}));break;
    case Id::BowArmAim:d.bowArmAim=!d.bowArmAim;break;
    case Id::ArrowHoldD48:d.arrowNativeTurnD48=!d.arrowNativeTurnD48;break;
    // R90: one slot, four choices: D86 DEFAULT -> R89 HAND-GUIDED -> FRIK TRIAL -> ELBOW REACH PREFERENCE -> D86 DEFAULT ...
    // R154: same order as the arm solver (pose_arm_solver.inl: R89 > reach preference > FRIK > default), so an INI with
    // VrFrikElbows=true AND VrElbowPreferenceD67=true (menu never writes that) shows and leaves the one that really runs.
    case Id::ElbowSolverD48:
        if(d.elbowSolverR89){d.elbowSolverR89=false;d.frikElbowsD48=true;d.elbowPreferenceR92=false;}
        else if(d.elbowPreferenceR92){d.elbowPreferenceR92=false;d.frikElbowsD48=false;}
        else if(d.frikElbowsD48){d.frikElbowsD48=false;d.elbowPreferenceR92=true;}
        else d.elbowSolverR89=true;break;
    case Id::TimingLocationD64:d.timingLocationD64=1-d.timingLocationD64;break;
    case Id::MapSizeD64:d.mapSizeD64=d.mapSizeD64>=1.49f?.40f:d.mapSizeD64+.05f;break;
    case Id::MapSupportD64:d.mapSupportD64=!d.mapSupportD64;break;
    case Id::MapNativePaperM43:d.mapNativePaperM43=!d.mapNativePaperM43;break;
    case Id::TwoHandBiasD65:d.twoHandBiasD65=!d.twoHandBiasD65;break;
    case Id::BowAimD65:d.bowAimD65=!d.bowAimD65;break;
    case Id::CrossbowReloadD65:d.crossbowReloadD65=!d.crossbowReloadD65;break;
    case Id::CrankTravelD66:d.crankTravelD66=next(d.crankTravelD66,std::array<float,9>{.04f,.06f,.08f,.10f,.12f,.14f,.16f,.18f,.20f});break;
    case Id::PickupClockD65:d.pickupClockD65=!d.pickupClockD65;break;
    case Id::AlchemySpeedD65:d.alchemySpeedD65=next(d.alchemySpeedD65,std::array<float,6>{1,2,3,4,6,8});break;
    case Id::TwoHandPrimaryBiasD65:d.twoHandPrimaryBiasD65=next(d.twoHandPrimaryBiasD65,std::array<float,9>{0,.05f,.10f,.15f,.20f,.25f,.30f,.35f,.40f});break;
    case Id::NativeRoomscaleD76:{const bool dodge=s.roomscale_mode==2||s.roomscale_mode==3;d.nativeRoomscaleD76=!d.nativeRoomscaleD76;s.roomscale_mode=d.nativeRoomscaleD76?(dodge?2:1):(dodge?3:0);break;}
    case Id::NpcListeningD76:d.npcListeningD76=!d.npcListeningD76;break;
    case Id::RidingVrikD76:d.ridingVrikD76=!d.ridingVrikD76;break;
    case Id::DiceHandsD85:d.diceHandsD85=!d.diceHandsD85;break; // D85 dice
    case Id::StaticGrabR89:d.staticGrabR89=!d.staticGrabR89;break; // R90 (R89 pickup)
    case Id::SceneryBigPropsR158:d.sceneryBigPropsR158=!d.sceneryBigPropsR158;break; // R158: never refused
    case Id::CrossbowHandOnlyR90:d.crossbowHandOnlyR90=!d.crossbowHandOnlyR90;break; // R96: never refused
    case Id::CorpseSqueezeSearchR90:d.corpseSqueezeSearchR90=!d.corpseSqueezeSearchR90;break; // R90
    case Id::TwoHandWeightR90:d.twoHandWeightR90=!d.twoHandWeightR90;break; // R90
    case Id::WeaponKickR96:d.weaponKickR96=!d.weaponKickR96;break; // R96
    case Id::SceneryPropSwapR100:d.sceneryPropSwapR100=!d.sceneryPropSwapR100;break; // R100: never refused
    case Id::ImpaleFastDeathR106:d.impaleFastDeathR106=!d.impaleFastDeathR106;break; // R106: never refused
    case Id::CinematicBarsR107:d.cinematicBarsR107=!d.cinematicBarsR107;break; // R107: never refused
    case Id::KnifeChestZoneR109:d.knifeChestZoneR109=!d.knifeChestZoneR109;break; // R109: never refused
    case Id::EyeSrgbR114:d.eyeSrgbR114=!d.eyeSrgbR114;break; // R114: never refused (applies at the next session)
    case Id::HeavyCrossbowHandPullR111:d.heavyCrossbowHandPullR111=!d.heavyCrossbowHandPullR111;break; // R111: never refused
    case Id::MediumCrossbowHandPullR117:d.mediumCrossbowHandPullR117=!d.mediumCrossbowHandPullR117;break; // R117: never refused
    case Id::VrikOnStartR111:d.vrikOnStartR111=!d.vrikOnStartR111;break; // R111: never refused
    case Id::StartHenryEyesR111:d.startHenryEyesR111=!d.startHenryEyesR111;break; // R111: never refused
    case Id::QuickSlotFadeR112:d.quickSlotFadeR112=!d.quickSlotFadeR112;break; // R112: never refused
    case Id::ControllerLayoutR118: // R118: never refused; choosing a layout applies its modifier preset (PS VR2 = face touch)
        d.controllerLayoutR118=controller_layout_r118::Next(d.controllerLayoutR118);
        if constexpr(HasFaceTouchR118<S>::value)s.quick_access_face_touch_r112=controller_layout_r118::FaceTouchFor(d.controllerLayoutR118);
        if constexpr(HasLeftThumbrestR143<S>::value)s.quick_access_left_thumbrest_r143=false;
        break;
    case Id::QuickAccessFaceTouchR118: // R118: the R113 VrQuickAccessFaceTouchR112 key, never refused
        if constexpr(HasFaceTouchR118<S>::value){
            if constexpr(HasLeftThumbrestR143<S>::value){
                if(s.quick_access_face_touch_r112){s.quick_access_face_touch_r112=false;s.quick_access_left_thumbrest_r143=false;}
                else if(s.quick_access_left_thumbrest_r143){s.quick_access_left_thumbrest_r143=false;s.quick_access_face_touch_r112=true;}
                else s.quick_access_left_thumbrest_r143=true;
            }else s.quick_access_face_touch_r112=!s.quick_access_face_touch_r112;
            break;
        }
        else return false;
    case Id::AlchemyViewSelectR116:d.alchemyViewSelectR116=!d.alchemyViewSelectR116;break; // R116: never refused
    case Id::PropFirmHoldR116:d.propFirmHoldR116=!d.propFirmHoldR116;break; // R116: never refused
    case Id::ShieldBlockBoxR120:d.shieldBlockBoxR120=!d.shieldBlockBoxR120;break; // R120: never refused
    case Id::HideBackWeaponsR121:d.hideBackWeaponsR121=!d.hideBackWeaponsR121;break; // R121: never refused
    case Id::HideAtStationsR121:d.hideAtStationsR121=!d.hideAtStationsR121;break; // R121: never refused
    case Id::EyeRayR121:d.eyeRayR121=!d.eyeRayR121;break; // R121: never refused
    case Id::MeleeGripR123:d.meleeGripR123=!d.meleeGripR123;break; // R123: never refused
    case Id::PropImpactSoundR124:d.propImpactSoundR124=!d.propImpactSoundR124;break; // R124: never refused
    case Id::HudInfinityR125:d.hudInfinityR125=!d.hudInfinityR125;break; // R125: never refused
    case Id::CombatCrossR125:d.combatCrossR125=!d.combatCrossR125;break; // R125: never refused
    case Id::ImpaleFullDepthR128:d.impaleFullDepthR128=!d.impaleFullDepthR128;break; // R128: never refused
    case Id::MinimalHudR141:d.minimalHudR141=!d.minimalHudR141;break; // R141: never refused
    case Id::HideTutorialsR141:d.hideTutorialsR141=!d.hideTutorialsR141;break; // R141: never refused
    case Id::LockpickOpaqueR141:d.lockpickOpaqueR141=!d.lockpickOpaqueR141;break; // R141: never refused
    case Id::EyeHoldR141:d.eyeHoldR141=!d.eyeHoldR141;break; // R141: never refused, live (next XR frame)
    case Id::RideFollowXR147:d.rideFollowXR147=!d.rideFollowXR147;break; // R147: never refused, live (next X press)
    case Id::TrainingVanillaNpcR163:d.trainingVanillaNpcR163=!d.trainingVanillaNpcR163;break; // R163: never refused, live (next control tick)
    case Id::TutorialNativeHitsR159:d.tutorialNativeHitsR159=(std::clamp(d.tutorialNativeHitsR159,0,2)+1)%3;break; // R161: cycles OFF / NATIVE HITS / GAME TIMING, never refused, live (next swing)
    case Id::SwingWhooshR116:d.swingWhooshR116=!d.swingWhooshR116;break; // R116: never refused
    case Id::NpcLifeD73:d.npcLifeD73=!d.npcLifeD73;break;
    case Id::NpcGazeD52:d.npcGazeHmdD52=!d.npcGazeHmdD52;break;
    case Id::FistStrokeD49:d.fistStrokeD49=!d.fistStrokeD49;break;
    case Id::PickupCameraBridgeD55:d.pickupCameraBridgeD55=!d.pickupCameraBridgeD55;break;
    case Id::AlchemyToolContactD55:d.alchemyToolContactD55=!d.alchemyToolContactD55;break;
    case Id::PetGestureD59:d.petGestureD59=!d.petGestureD59;break;
    case Id::WashGestureD59:d.washGestureD59=!d.washGestureD59;break;
    case Id::AlchemyTransferD59:d.alchemyTransferD59=!d.alchemyTransferD59;break;
    case Id::HideBow:s.exp_hide_stowed_bow=cycle(s.exp_hide_stowed_bow,3);break;
    case Id::F9:s.physical_f9_rung61=std::clamp(s.physical_f9_next_h6,0,3);break;
    case Id::PropWake:d.labPropWake=!d.labPropWake;break;
    case Id::PickupReachD40:d.pickupNearOnlyD40=!d.pickupNearOnlyD40;break;
    case Id::CombatNoStepD40:d.combatNoStepD40=!d.combatNoStepD40;break;
    case Id::CombatNoTurnD40:d.combatNoTurnD40=!d.combatNoTurnD40;break;
    case Id::NativePairedBackupD51:s.native_kill_nf=!s.native_kill_nf;break;
    case Id::ClinchModeD40:d.clinchModeD40=cycle(d.clinchModeD40,3);break;
    case Id::ClinchDistanceD40:s.clinch_distance32=next(s.clinch_distance32,std::array<float,5>{0,.40f,.45f,.55f,.70f});break;
    case Id::NpcTouchD40:return false; // D51 living reaction actuation suspended
    case Id::LivingTouchD40:case Id::LivingImpactD40:return false; // D51 suspended
    case Id::CorpseDragD40:d.labCorpseDragD40=!d.labCorpseDragD40;break;
    case Id::NpcContactReactionD41:d.npcContactReactionD41=!d.npcContactReactionD41;break;
    case Id::NpcStaminaReactionD41:d.npcStaminaReactionD41=!d.npcStaminaReactionD41;break;
    case Id::FinisherCandidateD41:d.finisherCandidateD41=!d.finisherCandidateD41;break;
    case Id::FreeTargetingD42:d.freeTargetingModeR133=d.freeTargetingModeR133>=2?0:d.freeTargetingModeR133<0?1:d.freeTargetingModeR133+1; // R133: 0 -> 1 -> 2 -> 0
      d.freeTargetingD42=d.freeTargetingModeR133!=0;break;
    default:return false;
    }
    return true;
}
inline const wchar_t* on(bool value) noexcept {return value?L"ON":L"OFF";}
inline const wchar_t* workMode(int value) noexcept {return value==3?L"6DOF":value==2?L"GESTURES":value==1?L"ASSISTED NATIVE":L"NATIVE";}
inline const wchar_t* propStateD40(unsigned value) noexcept {
    switch(value){case 0:return L"IDLE";case 1:return L"PREPARING";case 2:return L"WAITING FOR CONSTRAINT";
    case 3:return L"HELD";case 4:return L"RELEASING";case 5:return L"RETIRING";case 6:return L"QUARANTINED";case 7:return L"FAULT";default:return L"UNKNOWN";}
}
template<class S> void label(const S& s,const Row& row,wchar_t* out,std::size_t capacity) noexcept {
    const auto& d=s.live_d39;const wchar_t* value=L"";wchar_t number[48]{};
    switch(row.id){
    case Id::InstantActivitiesD72:value=on(d.instantActivitiesD72);break;
    case Id::InstantInteractionsD72:case Id::InstantCraftingD72:case Id::InstantReloadsD72:case Id::InstantCombatD72:{
        const unsigned family=row.id==Id::InstantInteractionsD72?0:row.id==Id::InstantCraftingD72?1:row.id==Id::InstantReloadsD72?2:3;
        const bool requested=family==0?d.instantInteractionsD72:family==1?d.instantCraftingD72:family==2?d.instantReloadsD72:d.instantCombatD72;
        const unsigned st=d.instantStateD72[family];
        value=!requested?L"OFF":!d.instantActivitiesD72?L"ON / MASTER OFF":st==2?L"ACTIVE":st==3?L"FINISHING":st==1?L"READY":st==4?L"NATIVE FOR THIS ACTION":L"ON / WAITING";
        // R120: the master strike part is the gate mode (MISC 2 row), say which one runs
        if constexpr(HasMsModeR120<S>::value)if(family==3)value=s.exp_ms_stab_only47==3?L"ON / INSTANT MASTER STRIKE":requested?L"ON / MASTER STRIKE PAIRED (MISC 2)":L"OFF";
        break;
    }
    case Id::DodgeWindowD70:if(d.dodgeWindowD70<=0)value=L"NATIVE";else{std::swprintf(number,48,L"%.2fx",double(d.dodgeWindowD70));value=number;}break;
    case Id::ContactContinuityD71:value=on(d.contactContinuityD71);break;
    case Id::ItemHandoverD71:value=on(d.itemHandoverD71);break;
    case Id::StabFixD77:value=on(d.stabFixD77);break;
    case Id::UnarmedFixD77:value=on(d.unarmedFixD77);break;
    case Id::MasterstrikeFixD77:value=on(d.masterstrikeFixD77);break;
    case Id::FinisherUpperPlayerD83:value=d.finisherUpperPlayerD83?L"NATIVE + HEAD TURNS":L"NATIVE";break;
    case Id::FinisherUpperNpcD83:value=d.finisherUpperNpcD83?L"NATIVE + HEAD TURNS":L"NATIVE";break;
    case Id::BladeZoneD84:value=d.bladeZoneD84?L"BLADE":L"HAND (OLD)";break; // D84 blade zone
    case Id::UpCutLowD84:value=d.upCutLowD84?L"LOW ATTACK":L"LEFT / RIGHT (OLD)";break;
    case Id::StabStartD84:value=d.stabStartD84?L"FROM START":L"OLD";break;
    case Id::HudWalkoffD70:value=on(d.hudWalkoffD70);break;
    case Id::LeanHeightD70:std::swprintf(number,48,L"%.0f cm",double(d.leanHeightD70*100));value=number;break;
    case Id::HandLeadD69:value=on(d.handLeadD69);break;
    case Id::HandLeadMsD69:std::swprintf(number,48,L"%.0f ms",double(d.handLeadMsD69));value=number;break;
    case Id::WeaponSameFrameD69:value=on(d.weaponSameFrameD69);break;
    case Id::RigidGripD69:value=on(d.rigidGripD69);break;
    case Id::ElbowHysteresisD69:value=on(d.elbowHysteresisD69);break;
    case Id::GripAnchorD67:value=on(d.gripAnchorD67);break;
    case Id::ArcCoverageD67:value=on(d.arcCoverageD67);break;
    case Id::SupportReleaseD67:value=on(d.supportReleaseD67);break;
    case Id::ContactFairnessD67:value=on(d.contactFairnessD67);break;
    case Id::ElbowContinuityD67:value=on(d.elbowContinuityD67);break;
    case Id::SmithRecoveryD67:value=on(d.smithRecoveryD67);break;
    case Id::SmithSemiD67:value=on(d.smithSemiD67);break;
    case Id::MotionTrialD67:value=d.motionTrialD67==2||d.motionTrialD67==3?L"FRESH POSE CAPPED":d.motionTrialD67==4?L"FRESH POSE UNCAPPED":L"BASELINE";break;
    case Id::ElbowOutwardD67:std::swprintf(number,48,L"%.2f",double(d.elbowOutwardD67));value=number;break;
    case Id::ElbowBackD67:std::swprintf(number,48,L"%.2f",double(d.elbowBackD67));value=number;break;
    case Id::BodyFollow:value=on(s.vrik_body_follow);break;
    case Id::Roomscale:value=s.roomscale_mode==1?(!s.vrik_body_follow?L"WALK OFF: 6DOF BODY OFF":d.nativeRoomscaleD76?L"1:1 WALK":L"LEGACY FOLLOW"):s.roomscale_mode==2?(!s.vrik_body_follow?L"DODGE (WALK: 6DOF BODY OFF)":d.nativeRoomscaleD76?L"1:1 WALK + DODGE":L"LEGACY FOLLOW + DODGE"):s.roomscale_mode==3?L"DODGE":L"OFF";break; // R121: walking needs 6DOF body (CameraHook::SetRoomscaleFollow)
    case Id::HeadHeight:value=native_crouch_r93::body_height_label_r98(s.body_height_mode98);break;
    case Id::SmoothMove:value=on(s.smooth_move51);break;
    case Id::Turn:value=s.vr_turn_mode==0?L"SNAP":L"SMOOTH";break;
    case Id::ShoulderWarning:std::swprintf(number,48,L"%.0f cm",double(d.shoulderWarningM)*100);value=number;break;
    case Id::HandsMode:value=d.handsMode==2?L"BALANCED":d.handsMode==1?L"PRECISE":L"NORMAL";break;
    case Id::HandSeparation:value=on(d.handSeparation);break;
    case Id::NativeHandPhysics:value=on(d.nativeHandPhysics);break;
    case Id::WallSlide:value=on(s.exp_wall_slide);break;
    case Id::GrabHand:value=on(s.grab_hand_stays43);break;
    case Id::TimingLocationD64:value=d.timingLocationD64?L"AT CAMERA PUBLICATION":L"BEFORE NATIVE UPDATE";break;
    case Id::MapSizeD64:std::swprintf(number,48,L"%.0f%%",double(d.mapSizeD64*100.f));value=number;break;
    case Id::MapSupportD64:value=on(d.mapSupportD64);break;
    case Id::MapNativePaperM43:value=on(d.mapNativePaperM43);break;
    case Id::HandSmoothing:value=!d.handSmoothing?L"OFF":d.handSmoothingPaused?L"ON / F2 PAUSED":L"ON";break;
    case Id::FingerTrial:value=on(d.fingerTrial);break;
    case Id::AlchemyHands:value=on(s.activity_hands59c7f[0]);break;
    case Id::AlchemyMode:value=workMode(s.alchemy_mode67);break;
    case Id::AlchemyPickupReach:value=s.alchemy_mode67==3?L"PALM RAY / 5 CM SIDES":d.alchemyPickupReach==1?L"ASSISTED":L"CLOSE BOUNDS";break;
    case Id::WorkstationGestures:value=on(d.workstationGestures);break;
    case Id::CraftReach:value=on(s.craft_trial_h41);break;
    case Id::AlchemyEyeTrial:value=on(d.alchemyEyeTrial);break;
    case Id::SmithHands:value=on(s.activity_hands59c7f[1]);break;
    case Id::SmithMode:value=workMode(s.smithing_mode67);break;
    case Id::SmithCollision:value=on(d.smithToolCollision);break;
    case Id::AnvilMode:value=s.smith_anvil_mode_a3==1?L"REAL BOUNDS":s.smith_anvil_mode_a3==2?L"LEARNED":L"OFF";break;
    case Id::SmithAutoHold:value=on(s.smith_auto_hold36);break;
    case Id::StationTorso:value=on(d.stationTorso);break;
    case Id::StationClearance:std::swprintf(number,48,L"%.0f cm",double(d.stationClearCm));value=number;break;
    case Id::ShoulderLayout:value=s.user_shoulder_holsters?L"USER":L"ALTERNATE";break;
    case Id::ChestDagger:value=on(s.chest_dagger54);break;
    case Id::WeaponRelease:value=s.weapon_release_mode_t1==1?L"DROP / THROW":L"AUTO STOW";break;
    case Id::ThrowStock:value=on(s.throwing_supply_t2);break;
    case Id::KnifeRightHipD66:value=on(d.knifeRightHipD66);break;
    case Id::QuickTalk:value=on(s.gesture_talk);break;
    case Id::TorchGesture:value=on(d.torchGesture);break;
    case Id::VisorGesture:value=on(d.visorGesture);break;
    case Id::GripStowTrial:value=!d.gripStowTrial?L"OFF":d.gripStowButton==1?L"B / X":d.gripStowButton==2?L"STICK CLICK":L"A / Y";break;
    case Id::ParchmentMap:value=!d.parchmentMap?L"OFF":d.parchmentReady?L"ON":L"ON / NOT READY";break;
    case Id::MapRegion:value=L"AUTO LOCATION";break;
    case Id::CameraHold:std::swprintf(number,48,L"%d ms",d.cameraHoldMs);value=number;break;
    case Id::BowArmAim:value=on(d.bowArmAim);break;
    case Id::ArrowHoldD48:value=d.arrowNativeTurnD48?L"NATIVE TURN":L"STABLE GRIP";break;
    case Id::FistStrokeD49:value=on(d.fistStrokeD49);break;
    case Id::PickupCameraBridgeD55:value=on(d.pickupCameraBridgeD55);break;
    case Id::TwoHandBiasD65:value=on(d.twoHandBiasD65);break;
    case Id::BowAimD65:value=on(d.bowAimD65);break;
    case Id::CrossbowReloadD65:value=on(d.crossbowReloadD65);break;
    case Id::CrankTravelD66:std::swprintf(number,48,L"%.0f cm",double(d.crankTravelD66)*100);value=number;break;
    case Id::PickupClockD65:value=on(d.pickupClockD65);break;
    case Id::AlchemySpeedD65:if(d.alchemySpeedD65<=1.f)value=L"OFF / NATIVE SPEED";else{std::swprintf(number,48,L"%.0fx",double(d.alchemySpeedD65));value=number;}break;
    case Id::TwoHandPrimaryBiasD65:std::swprintf(number,48,L"%.0f%%",double(d.twoHandPrimaryBiasD65)*100);value=number;break;
    case Id::AlchemyToolContactD55:value=on(d.alchemyToolContactD55);break;
    case Id::PetGestureD59:value=on(d.petGestureD59);break;
    case Id::WashGestureD59:value=on(d.washGestureD59);break;
    case Id::AlchemyTransferD59:value=on(d.alchemyTransferD59);break;
    case Id::NativeRoomscaleD76:value=on(d.nativeRoomscaleD76);break;
    case Id::NpcListeningD76:value=on(d.npcListeningD76);break;
    case Id::RidingVrikD76:value=on(d.ridingVrikD76);break;
    case Id::DiceHandsD85:value=d.diceHandsD85?dice_hands_d85::statusText(d.diceHandsStatusD85):L"OFF (gamepad)";break; // D85 dice
    case Id::StaticGrabR89:value=on(d.staticGrabR89);break; // R90 (R89 pickup)
    case Id::SceneryBigPropsR158:value=d.sceneryBigPropsR158?L"UP TO 1 M":L"SMALL";break; // R158
    case Id::CrossbowHandOnlyR90:value=d.crossbowHandOnlyR90?L"HAND ONLY":L"NATIVE-DRIVEN (XR34)";break; // R96
    case Id::CorpseSqueezeSearchR90:value=d.corpseSqueezeSearchR90?L"ONLY ON SQUEEZE (R89)":L"ALWAYS (R2 BUDGETED)";break; // R90
    case Id::TwoHandWeightR90:value=d.twoHandWeightR90?L"ON (weight slider)":L"OFF (R2 heft)";break; // R90
    case Id::WeaponKickR96:value=on(d.weaponKickR96);break; // R96
    case Id::SceneryPropSwapR100:value=on(d.sceneryPropSwapR100);break; // R100
    case Id::ImpaleFastDeathR106:value=on(d.impaleFastDeathR106);break; // R106
    case Id::CinematicBarsR107:value=on(d.cinematicBarsR107);break; // R107
    case Id::KnifeChestZoneR109:value=on(d.knifeChestZoneR109);break; // R109
    case Id::EyeSrgbR114:value=on(d.eyeSrgbR114);break; // R114
    // R121: hand pull runs only with Hand-driven crossbow reload ON and route HAND ONLY (crossbow_hand_d89.inl:113/307).
    case Id::HeavyCrossbowHandPullR111:value=!d.heavyCrossbowHandPullR111?L"OFF (crank)":!d.crossbowReloadD65?L"ON / HAND RELOAD OFF":!d.crossbowHandOnlyR90?L"ON / ROUTE NATIVE":L"ON (hand pull)";break; // R111
    case Id::MediumCrossbowHandPullR117:value=!d.mediumCrossbowHandPullR117?L"OFF (spanner)":!d.crossbowReloadD65?L"ON / HAND RELOAD OFF":!d.crossbowHandOnlyR90?L"ON / ROUTE NATIVE":L"ON (hand pull)";break; // R117
    case Id::VrikOnStartR111:value=on(d.vrikOnStartR111);break; // R111
    case Id::StartHenryEyesR111:value=on(d.startHenryEyesR111);break; // R111
    case Id::QuickSlotFadeR112:value=on(d.quickSlotFadeR112);break; // R112
    case Id::ControllerLayoutR118:value=controller_layout_r118::Name(d.controllerLayoutR118);break; // R118
    case Id::QuickAccessFaceTouchR118:if constexpr(HasFaceTouchR118<S>::value)value=s.quick_access_face_touch_r112?L"FACE TOUCH":[] (const S& v){if constexpr(HasLeftThumbrestR143<S>::value)return v.quick_access_left_thumbrest_r143?L"LEFT THUMBREST":L"RIGHT THUMBREST";else return L"THUMBREST";}(s);break; // R143
    case Id::AlchemyViewSelectR116:value=on(d.alchemyViewSelectR116);break; // R116
    case Id::PropFirmHoldR116:value=on(d.propFirmHoldR116);break; // R116
    case Id::ShieldBlockBoxR120:value=on(d.shieldBlockBoxR120);break; // R120
    case Id::HideBackWeaponsR121:value=on(d.hideBackWeaponsR121);break; // R121
    case Id::HideAtStationsR121:value=on(d.hideAtStationsR121);break; // R121
    case Id::EyeRayR121:value=on(d.eyeRayR121);break; // R121
    case Id::MeleeGripR123:value=on(d.meleeGripR123);break; // R123
    case Id::PropImpactSoundR124:value=on(d.propImpactSoundR124);break; // R124
    case Id::HudInfinityR125:value=d.hudInfinityR125?L"ON (INF)":L"OFF (distance)";break; // R125
    case Id::CombatCrossR125:value=d.combatCrossR125?L"SHOWN":L"HIDDEN (icons only)";break; // R125
    case Id::ImpaleFullDepthR128:value=d.impaleFullDepthR128?L"ON (to the guard)":L"OFF (shallow)";break; // R128
    case Id::MinimalHudR141:value=d.minimalHudR141?L"ON (minimal)":L"OFF (normal HUD)";break; // R141
    case Id::HideTutorialsR141:value=d.hideTutorialsR141?L"HIDDEN":(d.minimalHudR141?L"SHOWN (Minimal HUD hides them)":L"SHOWN");break; // R141
    case Id::LockpickOpaqueR141:value=d.lockpickOpaqueR141?L"ON (solid)":L"OFF (see-through)";break; // R141
    case Id::EyeHoldR141:value=d.eyeHoldR141?L"ON (hold, no black)":L"OFF (black frame)";break; // R141
    case Id::RideFollowXR147:value=d.rideFollowXR147?L"ON (X tap = LB when Follow is shown)":L"OFF (X tap = talk)";break; // R147, R155 on foot too
    case Id::TrainingVanillaNpcR163:value=d.trainingVanillaNpcR163?L"ON (NPCs vanilla in training fights)":L"OFF (all NPC features run)";break; // R163
    case Id::TutorialNativeHitsR159:value=d.tutorialNativeHitsR159==1?L"NATIVE HITS (no mod direct hit)":d.tutorialNativeHitsR159==2?L"GAME TIMING (game's own attack speed)":L"OFF";break; // R161
    case Id::SwingWhooshR116:value=on(d.swingWhooshR116);break; // R116
    case Id::NpcLifeD73:value=on(d.npcLifeD73);break;
    case Id::NpcGazeD52:value=!d.npcGazeHmdD52?L"NATIVE":!d.npcGazeReadyD52?L"FOLLOW HMD / UNAVAILABLE":d.npcGazeSceneD52?L"FOLLOW HMD / ADMITTED":L"FOLLOW HMD / WAITING";break;
    case Id::ElbowSolverD48:value=d.elbowSolverR89?L"HAND-GUIDED":d.elbowPreferenceR92?L"ELBOW REACH PREFERENCE":d.frikElbowsD48?L"FRIK TRIAL":L"DEFAULT";break; // R154: solver order
    case Id::HideBow:value=d.hideBackWeaponsR121?L"HIDDEN (GEAR 2: BACK WEAPONS ON)":s.exp_hide_stowed_bow==2?L"ALWAYS HIDDEN":s.exp_hide_stowed_bow==1?L"HIDE LOOKING UP":L"NATIVE";break; // R121: back rule wins
    case Id::F9:value=s.physical_f9_rung61==3?L"TRACKED WEAPON + LAB":s.physical_f9_rung61==2?L"PHYSICAL WEAPON":s.physical_f9_rung61==1?L"OBSERVE":L"OFF";break;
    case Id::PropWake:value=on(d.labPropWake);break;
    case Id::PickupReachD40:value=d.pickupNearOnlyD40?L"DIRECT (20 cm)":L"ASSISTED (25 cm)";break;
    case Id::CombatNoStepD40:value=on(d.combatNoStepD40);break;
    case Id::CombatNoTurnD40:value=!d.combatNoTurnD40?L"OFF":d.combatNoTurnReadyD40?L"ON":L"ON / HOOK NOT READY";break;
    case Id::NativePairedBackupD51:value=s.native_kill_nf?L"ON":L"OFF (DEFAULT)";break;
    case Id::CombatTurnStatusD40:value=d.combatNoTurnReadyD40?L"AVAILABLE":L"NOT READY";break;
    case Id::ClinchModeD40:value=d.clinchModeD40==1?L"CONTACT + GRIP":d.clinchModeD40==2?L"OFF":L"DISTANCE / LEGACY";break;
    case Id::ClinchDistanceD40:if(s.clinch_distance32<=0)value=L"NATIVE";else{std::swprintf(number,48,L"%.2f m",double(s.clinch_distance32));value=number;}break;
    case Id::NpcTouchD40:value=L"PAUSED / NATIVE REACTIONS";break;
    case Id::LivingTouchD40:case Id::LivingImpactD40:value=L"PAUSED / NATIVE REACTIONS";break;
    case Id::CorpseDragD40:value=d.labCorpseDragD40?L"ON / EXPERIMENTAL":L"OFF";break;
    case Id::NpcContactReactionD41:value=on(d.npcContactReactionD41);break;
    case Id::NpcStaminaReactionD41:value=!d.npcStaminaReactionD41?L"OFF":d.npcContactReactionD41?L"ON":L"ON / REACTIONS OFF";break;
    case Id::FinisherCandidateD41:value=d.finisherCandidateD41?L"PHYSICAL":L"DIRECTIONS (BACKUP)";break;
    case Id::FreeTargetingD42:// R103: context is false whenever this menu is open, so "PAUSED" was shown every time he looked.
      // R133: 3-way. the status part stays the R103 one (context false while this menu is open).
      if(d.freeTargetingModeR133<=0)value=L"OFF (GAME LOCK + CAMERA)";
      else if(d.freeTargetingModeR133==1)value=!d.freeTargetingContextD42?L"FREE (WORKS IN COMBAT)":d.freeTargetingLeaseD42?L"FREE / ACTIVE":L"FREE / NOT READY";
      else value=!d.freeTargetingContextD42?L"FREE PLUS (WORKS IN COMBAT)":d.freeTargetingHoldR133?L"FREE PLUS / VIEW HELD":d.freeTargetingLeaseD42?L"FREE PLUS / ACTIVE":L"FREE PLUS / NOT READY";
      break;
    case Id::ClinchStatusD40:value=d.clinchReadyD40?L"AVAILABLE":L"NOT READY";break;
    case Id::PropRightD40:case Id::PropLeftD40:{const int side=row.id==Id::PropLeftD40?1:0;value=d.propStatusKnownD40[side]?propStateD40(d.propStateD40[side]):L"NO CURRENT STATUS";break;}
    default:break;
    }
    if(capacity)std::swprintf(out,capacity,L"%ls: %ls",row.title,value);
}
enum class Click {None,Changed,ApplyFailed,SaveFailed,Trials,Combat,Crafting};
template<class S,class Apply,class Save> Click dispatch(unsigned& selected,int x,int y,S& state,Apply apply,Save save) {
    for(unsigned n=0;n<kTabs.size();++n)if(inside(kTabs[n],x,y)){selected=n;return Click::Changed;}
    if(group(selected)==Group::Body||group(selected)==Group::MotionD67||group(selected)==Group::Combat||group(selected)==Group::Hands||group(selected)==Group::Objects||group(selected)==Group::Gear){
        const bool body=group(selected)==Group::Body,combat=group(selected)==Group::Combat,hands=group(selected)==Group::Hands,objects=group(selected)==Group::Objects,gear=group(selected)==Group::Gear;
        if(gear&&selected==kGearPage3R125&&inside(kMotionPrevD69,x,y)){selected=kGearPage2R118;return Click::Changed;} // R125: GEAR 3 < 2 < 1
        if(gear&&(selected==kGearPage2R118||selected==kGearPage3R125)&&inside(kMotionNextD69,x,y)){selected=kGearPage3R125;return Click::Changed;} // R125: GEAR 2 > 3
        if(objects&&selected==kObjectsPage3R158&&inside(kMotionPrevD69,x,y)){selected=kObjectsPage2D72;return Click::Changed;} // R158: OBJECTS 3 < 2 < 1
        if(objects&&(selected==kObjectsPage2D72||selected==kObjectsPage3R158)&&inside(kMotionNextD69,x,y)){selected=kObjectsPage3R158;return Click::Changed;} // R158: OBJECTS 2 > 3
        if(inside(kMotionPrevD69,x,y)){selected=unsigned(gear?Group::Gear:body?Group::Body:objects?Group::Objects:hands?Group::Hands:combat?Group::Combat:Group::MotionD67);return Click::Changed;}
        if(inside(kMotionNextD69,x,y)){selected=gear?kGearPage2R118:body?kBodyPage2D76:objects?kObjectsPage2D72:hands?kHandsPage2D71:combat?kCombatPage2D70:kMotionPage2D69;return Click::Changed;}
    }
    if(inside(kTrials,x,y))return Click::Trials;
    if(inside(kCombat,x,y))return Click::Combat;
    if(inside(kCrafting,x,y))return Click::Crafting;
    const auto& rows=live_settings_d39::rows(selected);
    for(unsigned n=0;n<rows.size();++n)if(!rows[n].readOnly&&inside(rowRect(n),x,y)&&advance(state,rows[n].id)){
        if(!apply(state))return Click::ApplyFailed;
        if(!rows[n].session&&!save())return Click::SaveFailed;
        return Click::Changed;
    }
    return Click::None;
}
} // namespace kcd2vr::live_settings_d39
