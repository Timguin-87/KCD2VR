#include "../src/subsystems/experimental/live_settings_d39.h"
#include <cassert>
#include <cstdio>
#include <cwchar>
#include <limits>
#include <set>

// Only the portable view of VrMenuState is needed. The reducer below is the
// exact production helper used by VrMenu::HandleClick and DrawPanel.
struct MenuState {
    kcd2vr::live_settings_d39::Settings live_d39{};
    bool vrik_body_follow=true,hmd_body_height54=true,smooth_move51=false;
    int roomscale_mode=1,vr_turn_mode=1;
    bool precise_contacts44=true,exp_wall_slide=false,grab_hand_stays43=true;
    bool activity_hands59c7f[4]{true,true,true,false};
    int alchemy_mode67=1,smithing_mode67=1,smith_anvil_mode_a3=1;
    bool craft_trial_h41=false,smith_auto_hold36=true,user_shoulder_holsters=true,chest_dagger54=true;
    int weapon_release_mode_t1=0;
    bool throwing_supply_t2=false,gesture_talk=false,native_kill_nf=false;
    int held_map_region32=0,exp_hide_stowed_bow=0,physical_f9_rung61=0,physical_f9_next_h6=1;
    float clinch_distance32=.55f;bool npc_touch43=false;
};
namespace live=kcd2vr::live_settings_d39;
int main(){
    unsigned exercised=0,persistent=0,session=0,readOnly=0;
    std::set<unsigned> seen;
    for(unsigned selected=0;selected<=live::kBodyPage2D76;++selected){
        for(unsigned n=0;n<live::rows(selected).size();++n){
            const auto& row=live::rows(selected)[n];MenuState owner{},requested=owner;
            unsigned selectedCopy=selected;int applied=0,saved=0;
            const auto rect=live::rowRect(n);
            wchar_t before[160]{},after[160]{};live::label(owner,row,before,160);
            const auto result=live::dispatch(selectedCopy,rect.l+4,rect.t+4,requested,
                [&](const MenuState& next){++applied;owner=next;return true;},[&](){++saved;return true;});
            if(row.id==live::Id::None){assert(result==live::Click::None&&applied==0&&saved==0);continue;}
            if(row.readOnly){assert(result==live::Click::None&&applied==0&&saved==0);++readOnly;continue;}
            const bool first=seen.insert(unsigned(row.id)).second;
            if(!first){assert(selected==unsigned(live::Group::CraftD67)||selected==live::kBodyPage2D76);
                assert(row.id==live::Id::LeanHeightD70||row.id==live::Id::AlchemySpeedD65||row.id==live::Id::AlchemyToolContactD55||row.id==live::Id::AlchemyTransferD59||row.id==live::Id::SmithCollision||row.id==live::Id::AnvilMode||row.id==live::Id::SmithAutoHold);}
            ++exercised;
            assert(result==live::Click::Changed&&applied==1&&selectedCopy==selected);
            assert(saved==(row.session?0:1));row.session?++session:++persistent;
            live::label(owner,row,after,160);assert(std::wcscmp(before,after)!=0);
            assert(std::wcslen(after)<100); // 418px row at the actual small menu font
            if(n+1<8)assert(live::detailRect(n).b<live::rowRect(n+1).t);
        }
    }
    // The actual table now includes the D64 timing/map controls and six D65 controls.
    // Every visible row above was clicked, dispatched, re-read and labelled.
    std::fprintf(stderr,"row counts %u %u %u %u\n",exercised,session,persistent,readOnly);
    assert(exercised==115&&session==2&&persistent==113&&readOnly==4&&seen.size()==108); // D84 blade zone, up cut, stab start rows
    // D81 retires both knife controls from every visible menu page.
    assert(!seen.count(unsigned(live::Id::KnifeRightHipD66))&&seen.count(unsigned(live::Id::ThrowStock)));
    {MenuState s{};assert(!s.live_d39.knifeRightHipD66);
     assert(!s.throwing_supply_t2);
     assert(s.live_d39.twoHandBiasD65&&s.live_d39.twoHandPrimaryBiasD65==.20f&&s.live_d39.bowAimD65&&s.live_d39.crossbowReloadD65&&!s.live_d39.pickupClockD65&&s.live_d39.alchemySpeedD65==4);
     assert(live::advance(s,live::Id::PickupCameraBridgeD55)&&!s.live_d39.pickupCameraBridgeD55);
     assert(live::advance(s,live::Id::TwoHandBiasD65)&&!s.live_d39.twoHandBiasD65);
     assert(s.live_d39.bowAimD65&&s.live_d39.crossbowReloadD65&&!s.live_d39.pickupClockD65);
     for(int i=0;i<6;++i)assert(live::advance(s,live::Id::AlchemySpeedD65));
     assert(s.live_d39.alchemySpeedD65==4);}

    // D69 navigation shares the existing VRIK tab but not row slots or values.
    {MenuState s{};unsigned selected=unsigned(live::Group::MotionD67);int calls=0;
     auto click=[&](live::Rect r){return live::dispatch(selected,r.l+1,r.t+1,s,[&](const auto&){++calls;return true;},[&](){++calls;return true;});};
     assert(&live::rows(selected)==&live::kRows[unsigned(live::Group::MotionD67)]);
     assert(click(live::kMotionNextD69)==live::Click::Changed&&selected==live::kMotionPage2D69&&calls==0);
     assert(live::group(selected)==live::Group::MotionD67&&&live::rows(selected)==&live::kMotionTrialsD69);
     assert(!s.live_d39.handLeadD69&&!s.live_d39.weaponSameFrameD69&&!s.live_d39.rigidGripD69&&!s.live_d39.elbowHysteresisD69);
     assert(click(live::rowRect(1))==live::Click::Changed&&s.live_d39.handLeadMsD69==16.f&&!s.live_d39.handLeadD69&&calls==2);
     assert(click(live::rowRect(0))==live::Click::Changed&&s.live_d39.handLeadD69&&calls==4);
     assert(!s.live_d39.weaponSameFrameD69&&!s.live_d39.rigidGripD69&&!s.live_d39.elbowHysteresisD69);
     assert(s.live_d39.gripAnchorD67&&s.live_d39.supportReleaseD67&&s.live_d39.elbowContinuityD67&&s.live_d39.motionTrialD67==2);
     assert(click(live::kMotionPrevD69)==live::Click::Changed&&selected==unsigned(live::Group::MotionD67)&&calls==4);
     assert(click(live::kMotionNextD69)==live::Click::Changed);
     assert(click(live::kTabs[unsigned(live::Group::CraftD67)])==live::Click::Changed&&selected==unsigned(live::Group::CraftD67));
     assert(click(live::kMotionNextD69)==live::Click::None); // hidden page navigation cannot act on another tab
     assert(click(live::kTabs[unsigned(live::Group::MotionD67)])==live::Click::Changed&&selected==unsigned(live::Group::MotionD67));
     assert(live::group(999)==live::Group::Body&&&live::rows(999)==&live::kRows[0]);
     for(int n=0;n<8;++n)assert(live::advance(s,live::Id::HandLeadMsD69));
     assert(s.live_d39.handLeadMsD69==16.f);
     s.live_d39.handLeadMsD69=33;live::advance(s,live::Id::HandLeadMsD69);assert(s.live_d39.handLeadMsD69==0);
     s.live_d39.handLeadMsD69=std::numeric_limits<float>::quiet_NaN();live::advance(s,live::Id::HandLeadMsD69);assert(s.live_d39.handLeadMsD69==0);
    }

    {MenuState s{};unsigned selected=unsigned(live::Group::Combat);int calls=0;
     const auto n=live::kMotionNextD69;
     assert(live::dispatch(selected,n.l+1,n.t+1,s,[&](const auto&){++calls;return true;},[&](){++calls;return true;})==live::Click::Changed);
     assert(selected==live::kCombatPage2D70&&live::group(selected)==live::Group::Combat&&calls==0);
     assert(!s.live_d39.stabFixD77&&!s.live_d39.unarmedFixD77&&!s.live_d39.masterstrikeFixD77&&!s.live_d39.hudWalkoffD70);
     assert(live::advance(s,live::Id::StabFixD77)&&s.live_d39.stabFixD77&&!s.live_d39.unarmedFixD77&&!s.live_d39.masterstrikeFixD77&&!s.live_d39.hudWalkoffD70);
     assert(live::advance(s,live::Id::UnarmedFixD77)&&s.live_d39.unarmedFixD77&&s.live_d39.stabFixD77&&!s.live_d39.masterstrikeFixD77);
     assert(live::advance(s,live::Id::MasterstrikeFixD77)&&s.live_d39.masterstrikeFixD77&&s.live_d39.unarmedFixD77&&s.live_d39.stabFixD77);
     assert(s.live_d39.finisherUpperPlayerD83&&s.live_d39.finisherUpperNpcD83);
     assert(!s.live_d39.bladeZoneD84&&live::advance(s,live::Id::BladeZoneD84)&&s.live_d39.bladeZoneD84); // D84
     assert(!s.live_d39.upCutLowD84&&live::advance(s,live::Id::UpCutLowD84)&&s.live_d39.upCutLowD84);
     assert(!s.live_d39.stabStartD84&&live::advance(s,live::Id::StabStartD84)&&s.live_d39.stabStartD84);
     assert(live::advance(s,live::Id::FinisherUpperPlayerD83)&&!s.live_d39.finisherUpperPlayerD83&&s.live_d39.finisherUpperNpcD83);
     assert(live::advance(s,live::Id::FinisherUpperNpcD83)&&!s.live_d39.finisherUpperPlayerD83&&!s.live_d39.finisherUpperNpcD83);
     assert(live::advance(s,live::Id::HudWalkoffD70)&&s.live_d39.hudWalkoffD70);
    }
    {MenuState s{};unsigned selected=unsigned(live::Group::Hands);int calls=0;
     auto click=[&](live::Rect r){return live::dispatch(selected,r.l+1,r.t+1,s,[&](const auto&){++calls;return true;},[&](){++calls;return true;});};
     assert(click(live::kMotionNextD69)==live::Click::Changed&&selected==live::kHandsPage2D71&&calls==0);
     assert(!s.live_d39.contactContinuityD71&&!s.live_d39.itemHandoverD71);
     assert(click(live::rowRect(0))==live::Click::Changed&&s.live_d39.contactContinuityD71&&!s.live_d39.itemHandoverD71);
     assert(click(live::rowRect(1))==live::Click::Changed&&s.live_d39.itemHandoverD71&&calls==4);
     assert(click(live::kMotionPrevD69)==live::Click::Changed&&selected==unsigned(live::Group::Hands));
     assert(s.live_d39.motionTrialD67==2&&!s.live_d39.handSmoothing);}
    // D72: every independent instant preference survives its master being off.
    {MenuState s{};unsigned selected=unsigned(live::Group::Objects);int calls=0;
     auto click=[&](live::Rect r){return live::dispatch(selected,r.l+1,r.t+1,s,[&](const auto&){++calls;return true;},[&](){++calls;return true;});};
     assert(click(live::kMotionNextD69)==live::Click::Changed&&selected==live::kObjectsPage2D72&&calls==0);
     assert(&live::rows(selected)==&live::kInstantActivitiesD72);
     assert(click(live::rowRect(0))==live::Click::Changed&&!s.live_d39.instantActivitiesD72);
     assert(s.live_d39.instantInteractionsD72&&s.live_d39.instantCraftingD72&&s.live_d39.instantReloadsD72&&s.live_d39.instantCombatD72);
     wchar_t label[160]{};live::label(s,live::kInstantActivitiesD72[3],label,160);assert(std::wcsstr(label,L"MASTER OFF"));
     assert(click(live::rowRect(2))==live::Click::Changed&&!s.live_d39.instantCraftingD72&&s.live_d39.instantReloadsD72);
     assert(click(live::rowRect(0))==live::Click::Changed&&s.live_d39.instantActivitiesD72&&!s.live_d39.instantCraftingD72);
     s.live_d39.instantStateD72[2]=unsigned(kcd2vr::instant_d72::State::Active);
     live::label(s,live::kInstantActivitiesD72[3],label,160);assert(std::wcsstr(label,L"ACTIVE"));
     s.live_d39.instantStateD72[2]=unsigned(kcd2vr::instant_d72::State::Unsupported);
     live::label(s,live::kInstantActivitiesD72[3],label,160);assert(std::wcsstr(label,L"NATIVE FOR THIS ACTION"));
     assert(click(live::kMotionPrevD69)==live::Click::Changed&&selected==unsigned(live::Group::Objects));
    }
    // Accepted petting starts ON; washing and transfer trials remain OFF. A click must not couple them
    // to one another or alter the accepted hand/weapon/camera preferences.
    {MenuState s{};assert(s.live_d39.petGestureD59&&!s.live_d39.washGestureD59&&!s.live_d39.alchemyTransferD59);
        const bool follow=s.vrik_body_follow;const int hands=s.live_d39.handsMode;
        assert(live::advance(s,live::Id::PetGestureD59)&&!s.live_d39.petGestureD59);
        assert(!s.live_d39.washGestureD59&&!s.live_d39.alchemyTransferD59);
        assert(live::advance(s,live::Id::WashGestureD59)&&s.live_d39.washGestureD59);
        assert(live::advance(s,live::Id::AlchemyTransferD59)&&s.live_d39.alchemyTransferD59);
        assert(s.vrik_body_follow==follow&&s.live_d39.handsMode==hands);}
    {MenuState s{};s.alchemy_mode67=3;s.live_d39.alchemyPickupReach=1;wchar_t value[160]{};
        assert(!live::advance(s,live::Id::AlchemyPickupReach)&&s.live_d39.alchemyPickupReach==1);
        live::label(s,live::kRows[unsigned(live::Group::Alchemy)][2],value,160);assert(std::wcsstr(value,L"PALM RAY / 5 CM SIDES"));
        assert(s.live_d39.pickupCameraBridgeD55&&!s.live_d39.alchemyToolContactD55); // D64 camera default; contact stays opt-in
        assert(live::advance(s,live::Id::PickupCameraBridgeD55)&&!s.live_d39.pickupCameraBridgeD55&&!s.live_d39.alchemyToolContactD55);
        assert(live::advance(s,live::Id::AlchemyToolContactD55)&&s.live_d39.alchemyToolContactD55);}
    // Merely visiting tabs never mutates settings, saves or installs an owner.
    for(unsigned n=0;n<live::kTabs.size();++n){MenuState s{};unsigned selected=0;int calls=0;
        const auto r=live::kTabs[n];
        assert(live::dispatch(selected,r.l+1,r.t+1,s,[&](const auto&){++calls;return true;},[&](){++calls;return true;})==live::Click::Changed);
        assert(selected==n&&calls==0);
    }
    // The canonical hand mode retains BALANCED and keeps its legacy bool in
    // agreement. No other menu choice is reset when the old button shares it.
    {MenuState s{};s.alchemy_mode67=3;
        assert(live::advance(s,live::Id::HandsMode));assert(s.live_d39.handsMode==2&&!s.precise_contacts44);
        assert(live::advance(s,live::Id::HandsMode));assert(s.live_d39.handsMode==0&&!s.precise_contacts44);
        assert(live::advance(s,live::Id::HandsMode));assert(s.live_d39.handsMode==1&&s.precise_contacts44&&s.alchemy_mode67==3);
    }
    // Refused application cannot save an unaccepted preference; actual owner
    // values remain what the next FillMenuState snapshot will display.
    {MenuState owner{},request=owner;unsigned selected=0;int saved=0;auto r=live::rowRect(0);
        assert(live::dispatch(selected,r.l+1,r.t+1,request,[](const auto&){return false;},[&](){++saved;return true;})==live::Click::ApplyFailed);
        assert(owner.vrik_body_follow&&saved==0);
    }
    {MenuState owner{},request=owner;unsigned selected=0;auto r=live::rowRect(0);
        assert(live::dispatch(selected,r.l+1,r.t+1,request,[&](const auto& value){owner=value;return true;},[](){return false;})==live::Click::SaveFailed);
        assert(!owner.vrik_body_follow);
    }
    // F9 follows its native accepted/refused next cursor, not a parallel mode
    // counter. Session changes cannot invoke the persistent save callback.
    {MenuState s{};s.physical_f9_next_h6=3;unsigned selected=unsigned(live::Group::Objects);auto r=live::rowRect(1);int saved=0;
        assert(live::dispatch(selected,r.l+1,r.t+1,s,[](const auto&){return true;},[&](){++saved;return true;})==live::Click::Changed);
        assert(s.physical_f9_rung61==3&&saved==0);
    }
    // Torch and visor remain independent of QuickTalk and of each other.
    {MenuState s{};assert(!s.gesture_talk);
        live::advance(s,live::Id::TorchGesture);assert(!s.live_d39.torchGesture&&s.live_d39.visorGesture&&!s.gesture_talk);
        live::advance(s,live::Id::QuickTalk);assert(s.gesture_talk&&!s.live_d39.torchGesture&&s.live_d39.visorGesture);
    }
    // Saved object stow exposes all real button mappings and OFF.
    {MenuState s{};for(int button=0;button<3;++button){live::advance(s,live::Id::GripStowTrial);assert(s.live_d39.gripStowTrial&&s.live_d39.gripStowButton==button);}
        live::advance(s,live::Id::GripStowTrial);assert(!s.live_d39.gripStowTrial);
    }
    // Saved smoothing preference is visibly distinct from an F2 override.
    {MenuState s{};s.live_d39.handSmoothing=true;s.live_d39.handSmoothingPaused=true;wchar_t value[160]{};
        live::label(s,live::kRows[1][5],value,160);assert(std::wcsstr(value,L"ON / F2 PAUSED"));}
    // F2 mode 3 temporarily selects its own elbow/shoulder policy. Changing
    // the saved preference must not conceal that override or switch F2 modes.
    {MenuState s{};wchar_t value[160]{};const auto& row=live::kRows[1][7];
        assert(row.id==live::Id::ElbowSolverD48);
        live::label(s,row,value,160);assert(std::wcsstr(value,L"CONTINUOUS")&&!std::wcsstr(value,L"OVERRIDE"));
        s.live_d39.elbowF2OverrideD48=true;live::label(s,row,value,160);
        assert(std::wcsstr(value,L"CONTINUOUS / F2 3 OVERRIDE"));
        assert(live::advance(s,row.id)&&s.live_d39.elbowF2OverrideD48);
        live::label(s,row,value,160);assert(std::wcsstr(value,L"FRIK TRIAL / F2 3 OVERRIDE"));
        s.live_d39.elbowF2OverrideD48=false;live::label(s,row,value,160);
        assert(std::wcsstr(value,L"FRIK TRIAL")&&!std::wcsstr(value,L"OVERRIDE"));
        assert(std::wcsstr(row.detail,L"bow/native actions")&&std::wcslen(value)<100);
    }
    // Chest supply is explicit opt-in and reversible; right hip stays retired.
    {MenuState s{};assert(!s.throwing_supply_t2&&!s.live_d39.knifeRightHipD66);
        assert(live::advance(s,live::Id::ThrowStock)&&s.throwing_supply_t2);
        assert(live::advance(s,live::Id::ThrowStock)&&!s.throwing_supply_t2);
        assert(!live::advance(s,live::Id::KnifeRightHipD66));
        assert(!s.throwing_supply_t2&&!s.live_d39.knifeRightHipD66&&s.weapon_release_mode_t1==0);}
    // Runtime unavailable is distinct from the requested ON preference.
    {MenuState s{};wchar_t value[160]{};live::label(s,live::kRows[5][0],value,160);assert(std::wcsstr(value,L"NOT READY"));
        s.live_d39.parchmentReady=true;live::label(s,live::kRows[5][0],value,160);assert(!std::wcsstr(value,L"NOT READY"));
    }
    {MenuState s{};s.live_d39.shoulderWarningM=std::numeric_limits<float>::quiet_NaN();live::advance(s,live::Id::ShoulderWarning);assert(s.live_d39.shoulderWarningM==.02f);
        s.live_d39.cameraHoldMs=5000;live::advance(s,live::Id::CameraHold);assert(s.live_d39.cameraHoldMs==0);
        s.alchemy_mode67=3;live::advance(s,live::Id::AlchemyMode);assert(s.alchemy_mode67==0);
    }
    // D40 lab preferences visibly distinguish a saved ON from the selected mode.
    {MenuState s{};s.live_d39.labCorpseDragD40=true;wchar_t value[160]{};
        live::label(s,live::kRows[unsigned(live::Group::Npc)][5],value,160);assert(std::wcsstr(value,L"EXPERIMENTAL"));
        s.physical_f9_rung61=3;live::label(s,live::kRows[unsigned(live::Group::Npc)][5],value,160);assert(std::wcsstr(value,L"EXPERIMENTAL"));}
    {MenuState s{};assert(s.live_d39.clinchModeD40==2);live::advance(s,live::Id::ClinchModeD40);assert(s.live_d39.clinchModeD40==0);
        live::advance(s,live::Id::ClinchModeD40);assert(s.live_d39.clinchModeD40==1);
        live::advance(s,live::Id::ClinchModeD40);assert(s.live_d39.clinchModeD40==2);}
    {MenuState s{};s.live_d39.npcContactReactionD41=false;wchar_t value[160]{};
        live::label(s,live::kRows[unsigned(live::Group::Combat)][4],value,160);
        assert(std::wcsstr(value,L"ON / REACTIONS OFF"));
        assert(s.live_d39.npcStaminaReactionD41&&s.live_d39.finisherCandidateD41);}
    {MenuState s{};wchar_t value[160]{};const auto& row=live::kRows[unsigned(live::Group::Combat)][6];
        assert(row.id==live::Id::FreeTargetingD42&&s.live_d39.freeTargetingD42);
        live::label(s,row,value,160);assert(std::wcsstr(value,L"PAUSED"));
        live::advance(s,row.id);live::label(s,row,value,160);assert(std::wcsstr(value,L"OFF"));
        live::advance(s,row.id);live::label(s,row,value,160);assert(std::wcsstr(value,L"PAUSED"));
        s.live_d39.freeTargetingContextD42=true;live::label(s,row,value,160);assert(std::wcsstr(value,L"NOT READY"));
        s.live_d39.freeTargetingLeaseD42=true;live::label(s,row,value,160);assert(std::wcsstr(value,L"CAMERA LEASED"));}
    // D51 reuses two independent existing runtime owners. A directions-only
    // recipe is a backup to the physical finisher condition, not forced success.
    {MenuState s{};wchar_t value[160]{};const auto& native=live::kRows[unsigned(live::Group::Combat)][2];
        const auto& combo=live::kRows[unsigned(live::Group::Combat)][5];
        assert(native.id==live::Id::NativePairedBackupD51&&!s.native_kill_nf&&s.live_d39.finisherCandidateD41);
        assert(live::advance(s,native.id)&&s.native_kill_nf&&s.live_d39.finisherCandidateD41);
        live::label(s,native,value,160);assert(std::wcsstr(value,L": ON"));
        assert(live::advance(s,combo.id)&&!s.live_d39.finisherCandidateD41&&s.native_kill_nf);
        live::label(s,combo,value,160);assert(std::wcsstr(value,L"DIRECTIONS (BACKUP)"));
        assert(live::advance(s,native.id)&&!s.native_kill_nf&&!s.live_d39.finisherCandidateD41);
        live::label(s,native,value,160);assert(std::wcsstr(value,L"OFF (DEFAULT)"));
        assert(std::wcsstr(combo.detail,L"RPG"));}
    // The removed living-body writers cannot be re-enabled through another
    // caller of advance, even if an old INI still holds their requested value.
    {MenuState s{};s.npc_touch43=true;s.live_d39.labLivingTouchD40=true;s.live_d39.labLivingImpactD40=true;
        assert(!live::advance(s,live::Id::NpcTouchD40));
        for(unsigned row:{4u}){const auto& item=live::kRows[unsigned(live::Group::Npc)][row];
            wchar_t value[160]{};assert(item.readOnly&&!live::advance(s,item.id));
            live::label(s,item,value,160);assert(std::wcsstr(value,L"PAUSED / NATIVE REACTIONS"));}}
    {MenuState s{};const auto& item=live::kRows[unsigned(live::Group::Npc)][3];
     assert(s.live_d39.npcLifeD73&&s.live_d39.npcListeningD76&&!item.readOnly);
     assert(live::advance(s,item.id)&&!s.live_d39.npcLifeD73&&s.live_d39.npcListeningD76);
     assert(live::advance(s,item.id)&&s.live_d39.npcLifeD73&&s.live_d39.npcListeningD76);}
    {MenuState s{};unsigned selected=unsigned(live::Group::Body);int applied=0;
     assert(!s.live_d39.nativeRoomscaleD76&&s.live_d39.ridingVrikD76&&s.live_d39.npcListeningD76);
     auto click=[&](live::Rect r){return live::dispatch(selected,r.l+1,r.t+1,s,[&](const auto&){++applied;return true;},[]{return true;});};
     assert(click(live::kMotionNextD69)==live::Click::Changed&&selected==live::kBodyPage2D76&&applied==0);
     assert(click(live::rowRect(0))==live::Click::Changed&&s.live_d39.nativeRoomscaleD76&&s.live_d39.ridingVrikD76);
     assert(click(live::rowRect(2))==live::Click::Changed&&!s.live_d39.ridingVrikD76);
     assert(click(live::kMotionPrevD69)==live::Click::Changed&&selected==unsigned(live::Group::Body));
     selected=unsigned(live::Group::Npc);assert(click(live::rowRect(2))==live::Click::Changed&&!s.live_d39.npcListeningD76);
     assert(s.live_d39.npcLifeD73); // listening OFF does not disable gaze/face idles
     s.live_d39.npcLifeD73=false;
     assert(click(live::rowRect(2))==live::Click::Changed&&s.live_d39.npcListeningD76&&!s.live_d39.npcLifeD73);
    }
    std::printf("D81 live dispatch: %u production controls (%u persistent, %u session), %u read-only states, twelve tabs and VRIK page 2 PASS\n",exercised,persistent,session,readOnly);
}
