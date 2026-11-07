#include "vrik/tracked_frame51.h"
#include "activity_eye59c19.h"
#include "vrik/workstation178r20c1.h"
#include "vrik/presentation_context55.h"
#include "tracking_sample59c6.h"
#include "core/native_grasp13.h"
#include "core/snapshot_mailbox.h"
#include "diagnostics/capture168.h"
#include "vrik/visible_failure.h"
#include "diagnostics/interaction_cycle_h4.h"
#include <atomic>
#include <mutex>
#include <cstdio>
#include <cstring>

namespace kcd2vr::frik_pose_probe {
namespace f=kcd2vr::tracked_frame51;namespace m=kcd2vr::pose_core;
struct CommittedFrame6695 {m::Stamp stamp{};std::uint64_t publicationR24=0;float world[8]{};bool headEvidence122=false;};
static int census51=3;static std::uint64_t now51=1000;
static std::uint64_t GetTickCount64(){return now51;}
static unsigned GetCurrentThreadId(){return 1;}
static int CombatCensusMode(){return census51;}
static std::atomic<int> g_cal_state{2};
static std::atomic<bool> g_gameplay_allowed{true},g_head_valid{true},g_ctx_dialogue{false},g_ctx_menu{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_ctx_reading{false};
static kcd2vr::grasp13::Acquisition54 pickupCamera54{};
static bool pickupCameraReady54=false;
static bool ReadPickupCamera54(kcd2vr::grasp13::Acquisition54& p) noexcept {p=pickupCamera54;return pickupCameraReady54;}
static bool (*g_graspAcquisition54)(kcd2vr::grasp13::Acquisition54&) noexcept=nullptr;
static constexpr std::uint64_t kNativeBodyHeightBit54=std::uint64_t(1)<<63;
static std::atomic<std::uint64_t> g_frame_control51{2},g_frame_publication51{0},g_pose_epoch6689{1};
static std::uint64_t ReadCompletedPublicationT23() noexcept{return g_frame_publication51.load(std::memory_order_acquire);}
static std::atomic<double> g_cal_native_head_x{0},g_cal_native_head_y{0},g_cal_native_head_z{1.7};
static std::atomic<void*> g_henry_char_attachment_manager{reinterpret_cast<void*>(0x108)};
namespace diagnostics::lab {static bool reference=false;static bool NativeReference(){return reference;}}
namespace diagnostics {namespace ownership48=::kcd2vr::diagnostics::ownership48; namespace interaction_cycle_h4=::kcd2vr::diagnostics::interaction_cycle_h4; namespace capture168=::kcd2vr::diagnostics::capture168;}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};static bool kick=false;static bool NativeKickActive175(){return kick;}}
namespace diagnostic_trace {static unsigned bowReceipts55=0;static void Write(const char* s){if(std::strstr(s,"C55 BOW_CAMERA "))++bowReceipts55;}}
static bool packetReady51=true;static CommittedFrame6695 packet51{};
// The mailbox is the synthetic leaf. The production packet admission/freshness
// reader is extracted below so it cannot silently bypass gameplay ownership.
static std::atomic<bool> g_committed_pose_valid6689{true},g_pose_restore_fault6689{false},g_dialogue_body178r9{false};
static std::atomic<std::uint64_t> g_tool_context19c4r2{1000<<8};
static std::atomic<void*> g_committed_character6689{reinterpret_cast<void*>(0x100)};
static bool TrackingHeadUsable111(){return g_head_valid.load();}
static unsigned entryKindH51=kcd2vr::minigame_hands45::Alchemy;
static void BeginAlchemyEntryH51(){
    auto& w=kcd2vr::activity_input163::witness175;w.ready=true;w.transitionDepth175=0;
    w.Begin(entryKindH51==kcd2vr::minigame_hands45::Alchemy?kcd2vr::activity_input163::Kind175::Alchemy:kcd2vr::activity_input163::Kind175::Smithing,0xA000,now51);
    kcd2vr::minigame_hands45::setMode(entryKindH51,kcd2vr::minigame_hands45::Gesture);
    kcd2vr::minigame_hands45::publish(entryKindH51,now51);
}
static void BeginAlchemyNativeOnlyH51(){
    auto& w=kcd2vr::activity_input163::witness175;w.ready=true;w.transitionDepth175=0;
    w.Begin(entryKindH51==kcd2vr::minigame_hands45::Alchemy?kcd2vr::activity_input163::Kind175::Alchemy:kcd2vr::activity_input163::Kind175::Smithing,0xA000,now51);
}
static unsigned beginAlchemyOnPacketReadH51=0,packetNativeMutationH51=0;
static unsigned packetReadsD62=0,packetRaceD62=0;
static void AdvancePacketD62();
static struct PacketMailbox55 {bool read(CommittedFrame6695& p){p=packet51;
    ++packetReadsD62;if(packetRaceD62){const unsigned mode=packetRaceD62;if(mode==1)packetRaceD62=0;AdvancePacketD62();
        if(mode==3)g_ctx_cutscene=true;}
    if(beginAlchemyOnPacketReadH51&&!--beginAlchemyOnPacketReadH51){
        if(packetNativeMutationH51==1)BeginAlchemyNativeOnlyH51();
        else if(packetNativeMutationH51==2)kcd2vr::activity_input163::witness175.transitionDepth175=1;
        else BeginAlchemyEntryH51();}
    return packetReady51;}} g_committed_frame6695;
static kcd2vr::core::SnapshotMailbox<f::Frame> g_tracked_frame51;
struct ModelWorld6692 {m::Quat q{};m::Vec3 t{};double scale=1;};
static ModelWorld6692 world51{};static bool worldReady51=true;static unsigned worldReads51=0,worldMutation51=0;
static std::uintptr_t worldCharacter51=0;
static void publish51(f::Frame f);
static bool ReadModelWorld6692(void* character,ModelWorld6692& out){
    ++worldReads51;worldCharacter51=reinterpret_cast<std::uintptr_t>(character);out=world51;
    switch(worldMutation51){case 1:g_henry_char_attachment_manager=reinterpret_cast<void*>(0x888);break;
        case 2:++g_pose_epoch6689;break;case 3:g_frame_control51.fetch_add(2);break;
        case 4:packetReady51=false;break;case 5:g_frame_publication51.fetch_add(2);break;
        // Workstations themselves may set the broad menu bit; real UI is
        // independently published in the production context word.
        case 6:g_ctx_menu=true;g_tool_context19c4r2=(now51<<8)|16;break;
        case 7:tracking59c6::Reset();break;
        case 8:{auto& w=kcd2vr::activity_input163::witness175;const auto instance=w.active[0].load();w.End(instance);
            w.Begin(kcd2vr::activity_input163::Kind175::Alchemy,instance,now51);break;}
        case 9:kcd2vr::minigame_hands45::setMode(entryKindH51,kcd2vr::minigame_hands45::SemiPassenger);break;
        case 10:g_dialogue_body178r9=false;break;
        case 11:kcd2vr::minigame_hands45::setMode(entryKindH51,kcd2vr::minigame_hands45::Native);break;
        case 12:kcd2vr::minigame_hands45::witness=0;break;
        case 13:kcd2vr::activity_input163::witness175.transitionDepth175=1;break;
        case 14:BeginAlchemyEntryH51();worldMutation51=0;break;
        case 15:kcd2vr::activity_input163::witness175.End(0xA000);kcd2vr::minigame_hands45::publish(kcd2vr::minigame_hands45::None,now51);break;
        case 16:BeginAlchemyNativeOnlyH51();worldMutation51=0;break;
        case 17:kcd2vr::activity_input163::witness175.transitionDepth175=1;worldMutation51=0;break;
        case 18:++pickupCamera54.nativeToken;break;
        case 19:kcd2vr::camera_hold_e1::pickupBridgeD55=false;break;
        case 20:case 21:{f::Frame next{};g_tracked_frame51.read(next);++next.stamp.sample;next.stamp.tick=now51;
            publish51(next);if(worldMutation51==20)worldMutation51=0;break;}
        case 22:case 23:{const auto change=worldMutation51;worldMutation51=0;
            g_graspAcquisition54=nullptr;pickupCameraReady54=false;g_gameplay_allowed=true;g_ctx_scripted=false;
            if(change==23){f::Frame next{};g_tracked_frame51.read(next);++next.stamp.sample;next.stamp.tick=now51;
                next.acquisitionSerial=0;next.acquisitionAction=next.acquisitionScope=0;next.acquisitionToken=0;
                publish51(next);}break;}
        case 24:g_graspAcquisition54=ReadPickupCamera54;pickupCameraReady54=true;g_gameplay_allowed=false;g_ctx_scripted=true;worldMutation51=0;break;
        case 25:++pickupCamera54.serial;worldMutation51=0;break;}


    return worldReady51;
}
#include "generated_tracked_camera_pickup_d55.inc"
static unsigned checks=0,failures=0;
static void ck(bool yes,const char* label){++checks;if(!yes)++failures;std::printf("%s %s\n",yes?"PASS":"FAIL",label);}
static bool near(float a,float b){return std::fabs(a-b)<1e-5f;}
static f::Frame frame51(){f::Frame f{};f.stamp={1,2,1000,0x100};f.control=2;f.valid=f.neutral=true;
    f.root={10,20,5};f.nativeHead=f.anchor=f.rawAnchor=f.calibratedHead={0,0,1.7};f.restOffset={0,0,.8};return f;}
static void publish51(f::Frame f){const auto previous=g_frame_publication51.load()&~std::uint64_t(1);g_frame_publication51=previous+1;f.publication=previous+2;
    g_tool_context19c4r2=now51<<8;
    g_committed_character6689=reinterpret_cast<void*>(f.stamp.actor);
    packet51.stamp=f.stamp;packet51.publicationR24=f.publication;packet51.headEvidence122=true;
    packet51.world[0]=float(f.rotation.x);packet51.world[1]=float(f.rotation.y);packet51.world[2]=float(f.rotation.z);packet51.world[3]=float(f.rotation.w);
    packet51.world[4]=float(f.root.x);packet51.world[5]=float(f.root.y);packet51.world[6]=float(f.root.z);packet51.world[7]=float(f.scale);
    g_tracked_frame51.publish(f);g_frame_publication51=f.publication;world51={f.rotation,f.root,f.scale};}
static void AdvancePacketD62(){f::Frame f{};g_tracked_frame51.read(f);++f.stamp.sample;f.stamp.tick=now51;publish51(f);}
int tests51(){
    // Existing tests isolate fresh completed-frame admission. D53's actual
    // fallback integration is exercised separately with its hold enabled.
    kcd2vr::camera_hold_e1::maxHoldMs=0;
    float x=0,y=0,z=0;auto frame=frame51();publish51(frame);
    worldMutation51=5;
    ck(!TrackedFrameCamera51(10,20,6.7f,x,y,z),"refused first neutral read cannot commit its staged anatomical eye offset");
    worldMutation51=0;publish51(frame);
    ck(TrackedFrameCamera51(10,20,6.756f,x,y,z)&&near(z,6.756f),"actual reader learns exact neutral original-body eye offset");
    {const auto reads=worldReads51;worldMutation51=20;
     ck(TrackedFrameCamera51(10,20,6.756f,x,y,z)&&near(z,6.756f)&&worldReads51==reads+2,
        "D56 completed publication race reconstructs a fresh pair once instead of native fallback");
     worldMutation51=21;const auto before=worldReads51;
     ck(!TrackedFrameCamera51(10,20,6.756f,x,y,z)&&worldReads51==before+2,
        "D56 retry is bounded to two attempts; continuously racing publication never accepts a torn pair");
     worldMutation51=0;publish51(frame);}

    {const auto reads=packetReadsD62;const auto roots=worldReads51;packetRaceD62=1;
     ck(TrackedFrameCamera51(10,20,6.756f,x,y,z)&&near(z,6.756f)&&packetReadsD62==reads+3&&worldReads51==roots+1,
        "D62 initial packet publication race retries one complete pair before reading current root");
     packetRaceD62=2;const auto churnReads=packetReadsD62;const auto churnRoots=worldReads51;
     float churnX=11,churnY=22,churnZ=33;
     const bool churnAcceptedD62=TrackedFrameCamera51(10,20,6.756f,churnX,churnY,churnZ);
     // The preexisting hold wrapper performs one current-root preflight before
     // its zero-duration hold refuses. Neither failed fresh-pair attempt reads
     // a root or publishes an eye, and the rejected wrapper preserves outputs.
     ck(!churnAcceptedD62&&packetReadsD62==churnReads+2&&worldReads51==churnRoots+1&&
        near(churnX,11)&&near(churnY,22)&&near(churnZ,33),
        "D62 continuous initial packet churn is bounded to two attempts and never publishes a torn pair or held eye");
     packetRaceD62=3;const auto ownerReads=packetReadsD62;
     ck(!TrackedFrameCamera51(10,20,6.756f,x,y,z)&&packetReadsD62==ownerReads+1,
        "D62 a real native camera owner during packet churn is never retried into gameplay");
     packetRaceD62=0;g_ctx_cutscene=false;publish51(frame);}

    frame.neutral=false;frame.bow=true;frame.nativeHead={.1,.2,1.45};++frame.stamp.sample;frame.stamp.tick=++now51;publish51(frame);
    ck(TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z)&&near(x,10)&&near(y,20)&&near(z,6.756f),"actual reader removes authored bow eye motion using current committed anchor");
    world51.t={10.1,20.05,5.02};
    ck(TrackedFrameCamera51(10.2f,20.25f,6.526f,x,y,z)&&near(x,10.1f)&&near(y,20.05f)&&near(z,6.776f)&&
       worldReads51>=3&&worldCharacter51==frame.stamp.actor,
       "actual reader preserves current root travel between completed poses with one bounded native transform read");
    const double q=std::sqrt(.5);world51.q={0,0,q,q};
    ck(TrackedFrameCamera51(9.9f,20.15f,6.526f,x,y,z)&&near(x,10.1f)&&near(y,20.05f)&&near(z,6.776f),
       "actual reader projects the completed model-local anchor through current root rotation");
    world51.scale=.9;
    ck(!TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z),"current scale cannot borrow a completed frame at another scale");
    publish51(frame);worldReady51=false;
    ck(!TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z),"failed current model-world read cannot replay a cached world root");
    worldReady51=true;
    for(unsigned mutation=1;mutation<=6;++mutation){
        const auto token=g_frame_control51.load();worldMutation51=mutation;
        ck(!TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z),"owner/epoch/control/pose/publication/context change during world read retires result");
        worldMutation51=0;g_henry_char_attachment_manager=reinterpret_cast<void*>(0x108);g_pose_epoch6689=1;
        g_frame_control51=token;packetReady51=true;g_ctx_menu=false;publish51(frame);
    }
    const auto published=g_frame_publication51.load();g_frame_publication51=published+1;
    ck(!TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z),"equal-XR-sample body replacement remains unavailable while paired publication is odd");
    g_frame_publication51=published+2;
    ck(!TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z),"equal XR stamp/world cannot pair a new body publication with an old camera frame");
    publish51(frame);
    for(unsigned gate=0;gate<10;++gate){
        switch(gate){case 0:g_ctx_menu=true;break;case 1:g_ctx_dialogue=true;break;case 2:g_ctx_scripted=true;break;
        case 3:g_ctx_cutscene=true;break;case 4:g_ctx_reading=true;break;case 5:animation_filter::nativeBodyOwned112=true;break;
        case 6:animation_filter::kick=true;break;case 7:packetReady51=false;break;case 8:g_head_valid=false;break;case 9:diagnostics::lab::reference=true;break;}
        ck(!TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z),"actual forbidden/unavailable owner cannot apply frame correction");
        g_ctx_menu=g_ctx_dialogue=g_ctx_scripted=g_ctx_cutscene=g_ctx_reading=false;animation_filter::nativeBodyOwned112=false;
        animation_filter::kick=false;packetReady51=true;g_head_valid=true;diagnostics::lab::reference=false;
        ck(TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z)&&near(z,6.756f),"return while bow remains drawn reuses only same-body anatomical eye offset");
    }
    now51=frame.stamp.tick+81;
    ck(!TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z),"stale actual reader frame is refused");
    frame.stamp.tick=now51;++frame.stamp.sample;publish51(frame);
    ck(TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z)&&near(z,6.756f),"fresh same-body drawn frame resumes after stale interval without recapturing aim posture");
    SetSmoothBody51(true);const auto token=g_frame_control51.load();
    ck(SmoothBody51()&&token==5&&!TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z),"one atomic token changes mode and immediately retires prior frame generation");
    frame.control=token;frame.smooth=true;publish51(frame);
    ck(TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z)&&near(z,6.756f),"toggle during draw keeps immutable same-body anatomy after current-frame join");
    SetSmoothBody51(false);ck(!SmoothBody51()&&g_frame_control51.load()==6,"OFF mode and its generation change atomically");
    frame.control=6;frame.smooth=false;publish51(frame);
    packet51.stamp.sample++;
    ck(!TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z),"actual reader rejects mismatched pose and frame samples");
    publish51(frame);packet51.world[4]+=.05f;
    ck(!TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z),"actual reader rejects a different native model transform");
    frame.stamp.actor=0x200;frame.stamp.epoch=2;g_pose_epoch6689=2;g_henry_char_attachment_manager=reinterpret_cast<void*>(0x208);publish51(frame);
    ck(!TrackedFrameCamera51(10.1f,20.2f,6.506f,x,y,z),"replacement body cannot borrow previous character's eye offset while drawn");
    frame.neutral=true;frame.nativeHead={0,0,1.7};publish51(frame);
    ck(TrackedFrameCamera51(10,20,6.756f,x,y,z),"replacement body needs its own positively neutral measurement");
    // A matching current accepted acquisition may own presentation through a
    // scripted pickup, but no other queued/expired/replaced native action can.
    frame=frame51();frame.control=g_frame_control51.load();frame.stamp.tick=now51;
    g_henry_char_attachment_manager=reinterpret_cast<void*>(0x108);g_pose_epoch6689=1;
    publish51(frame);ck(TrackedFrameCamera51(10,20,6.756f,x,y,z),"pickup camera gets current body's neutral anatomy first");
    pickupCamera54={frame.stamp.actor,0x2000,0x3000,23,frame.stamp.epoch,now51,now51,17,0,true};
    pickupCamera54.trackingEpoch55=tracking59c6::epoch.load();
    g_graspAcquisition54=ReadPickupCamera54;pickupCameraReady54=true;g_gameplay_allowed=false;g_ctx_scripted=true;
    ck(!TrackedFrameCamera51(10,20,6.756f,x,y,z),"old ordinary frame cannot borrow newly accepted pickup witness");
    frame.neutral=false;frame.acquisitionSerial=23;frame.acquisitionAction=0x2000;frame.acquisitionScope=0x3000;frame.acquisitionToken=17;frame.nativeHead.y+=.20f;publish51(frame);
    ck(TrackedFrameCamera51(10,20.2f,6.756f,x,y,z)&&near(y,20),"joined pickup frame removes authored lunge without gameplay promotion");
    g_tool_context19c4r2=(now51<<8)|16;
    ck(!g_ctx_menu.load()&&!TrackedFrameCamera51(10,20.2f,6.756f,x,y,z),"safe-ui/matrix context wins immediately even before generic menu/input flags update");
    g_tool_context19c4r2=(now51-81)<<8;
    ck(!TrackedFrameCamera51(10,20.2f,6.756f,x,y,z),"stale copied modal context cannot authorize pickup camera");
    g_tool_context19c4r2=now51<<8;
    tracking59c6::Reset();
    ck(!TrackedFrameCamera51(10,20.2f,6.756f,x,y,z),"XR source reset immediately refuses cached old Picking before next raw input publication");
    tracking59c6::epoch=pickupCamera54.trackingEpoch55;worldMutation51=7;
    ck(!TrackedFrameCamera51(10,20.2f,6.756f,x,y,z),"XR source reset during current model-world read cannot commit old Picking camera");
    worldMutation51=0;tracking59c6::epoch=pickupCamera54.trackingEpoch55;
    ++pickupCamera54.nativeToken;
    ck(!TrackedFrameCamera51(10,20.2f,6.756f,x,y,z),"new native action token cannot borrow old pickup camera frame");
    --pickupCamera54.nativeToken;pickupCamera54.scope=0x4000;
    ck(!TrackedFrameCamera51(10,20.2f,6.756f,x,y,z),"new native scope cannot borrow old pickup camera frame");
    pickupCamera54.scope=0x3000;animation_filter::nativeBodyOwned112=true;
    ck(!TrackedFrameCamera51(10,20.2f,6.756f,x,y,z),"protected native hit wins over an otherwise matching pickup witness");
    animation_filter::nativeBodyOwned112=false;
    ++pickupCamera54.serial;
    ck(!TrackedFrameCamera51(10,20.2f,6.756f,x,y,z),"different acquisition cannot use previous presentation frame");
    --pickupCamera54.serial;pickupCameraReady54=false;
    ck(!TrackedFrameCamera51(10,20.2f,6.756f,x,y,z),"release immediately retires pickup camera ownership");
    g_gameplay_allowed=true;g_ctx_scripted=false;g_graspAcquisition54=nullptr;
    // Exercise the actual reader's joined bow heading and exact exit behavior.
    frame=frame51();frame.stamp.actor=0x300;frame.stamp.epoch=3;frame.control=g_frame_control51.load();
    frame.stamp.tick=now51;g_pose_epoch6689=3;g_henry_char_attachment_manager=reinterpret_cast<void*>(0x308);publish51(frame);
    float hx=0,hy=0;
    ck(TrackedFrameCamera55(10,20,6.756f,0,1,0,x,y,z,hx,hy)&&near(hx,0)&&near(hy,0),
       "actual reader learns neutral local yaw without acquiring ordinary heading");
    frame.bow=true;frame.neutral=false;++frame.stamp.sample;frame.stamp.tick=++now51;publish51(frame);
    ck(diagnostic_trace::bowReceipts55==0,"quiet bow camera path emits no transaction receipt");
    kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Reload,true,now51);
    ck(TrackedFrameCamera55(10.04f,20,6.70f,.12f,.9928f,.02f,x,y,z,hx,hy)&&near(hx,0)&&near(hy,1)&&near(z,6.756f),
       "actual first bow draw excludes authored yaw and position sway");
    world51.q={0,0,q,q};world51.t={11,22,5.2};
    ck(TrackedFrameCamera55(11,22,6.90f,-.9928f,.12f,0,x,y,z,hx,hy)&&near(hx,-1)&&near(hy,0)&&near(z,6.956f),
       "actual current root turn and travel remain authoritative during bow draw");
    ck(diagnostic_trace::bowReceipts55==1,"finite F11 capture records current joined bow transaction once per500ms");
    kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Reload,false,now51);
    g_ctx_scripted=true;g_gameplay_allowed=false;
    ck(!TrackedFrameCamera55(11,22,6.45f,0,1,0,x,y,z,hx,hy)&&near(hx,0)&&near(hy,0),
       "native workstation ownership cannot apply a bow heading or frame");
    g_ctx_scripted=false;g_gameplay_allowed=true;
    frame.bow=false;frame.neutral=true;++frame.stamp.sample;frame.stamp.tick=++now51;publish51(frame);
    ck(TrackedFrameCamera55(10,20,6.45f,0,1,0,x,y,z,hx,hy)&&near(z,6.756f)&&near(hx,0)&&near(hy,0),
       "same-body workstation exit cannot retrain anatomy from its returning low native camera");
    const auto heightBefore54=g_frame_control51.load();SetHmdBodyHeight54(false);
    ck(!HmdBodyHeight54()&&g_frame_control51.load()!=heightBefore54&&
       !TrackedFrameCamera51(10,20,6.756f,x,y,z),"height ownership change atomically retires previous completed pose");
    SetSmoothBody51(true);ck(SmoothBody51()&&!HmdBodyHeight54(),"smooth toggle preserves explicit native-height policy");
    SetHmdBodyHeight54(true);ck(HmdBodyHeight54()&&SmoothBody51(),"HMD-height toggle preserves smoothing and retires its own generation");
    std::printf("tracked_frame51_camera=%u/%u; actual reader/settings; native/headset=NOT_EXECUTED\n",checks-failures,checks);
    return failures?1:0;
}
int alchemyTestsH51(){
    namespace ws=kcd2vr::workstation178r20c1;namespace mh=kcd2vr::minigame_hands45;
    namespace ai=kcd2vr::activity_input163;
    checks=failures=0;float x=0,y=0,z=0,hx=0,hy=0;
    const auto restore=[](){g_cal_state=2;census51=3;g_head_valid=true;g_committed_pose_valid6689=true;g_pose_restore_fault6689=false;
        g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_ctx_reading=false;
        animation_filter::nativeBodyOwned112=false;animation_filter::kick=false;diagnostics::lab::reference=false;
        g_frame_control51=2;g_pose_epoch6689=55;g_henry_char_attachment_manager=reinterpret_cast<void*>(0x508);
        packetReady51=worldReady51=true;worldMutation51=0;g_graspAcquisition54=nullptr;pickupCameraReady54=false;
        ai::witness175.ready=true;ai::witness175.transitionDepth175=0;};
    restore();now51=10000;g_frame_publication51=0;g_gameplay_allowed=true;g_dialogue_body178r9=false;
    kcd2vr::alchemy_own_head_h51::enabled=false;
    auto frame=frame51();frame.stamp={55,2,now51,0x500};publish51(frame);
    ck(TrackedFrameCamera55(10,20,6.756f,0,1,0,x,y,z,hx,hy),"H51 actual neutral body primes its anatomical eye and heading");
    ai::witness175.Clear();ai::witness175.Begin(ai::Kind175::Alchemy,0xA000,now51);
    ws::experimentArmed36=false;kcd2vr::combat_rules178r10::f5Rung=0;kcd2vr::combat_rules178r10::workstationEnabled=true;
    mh::setMode(mh::Alchemy,mh::Gesture);
    const auto publishAlchemy=[&](){++now51;mh::publish(mh::Alchemy,now51);frame.neutral=false;frame.workstationKindH51=mh::Alchemy;
        frame.nativeHead={.3,.2,1.4};++frame.stamp.sample;frame.stamp.tick=now51;
        ws::WorkstationOwnerH51 owner{};ck(ws::workstationOwnerH51(now51,owner),"H51 current native lifecycle supplies frame identity");
        frame.workstationModeEpochH51=owner.modeEpoch;frame.workstationLifetimeH51=owner.lifetime;frame.workstationInstanceH51=owner.instance;
        publish51(frame);};
    publishAlchemy();g_gameplay_allowed=false;g_dialogue_body178r9=true;g_ctx_scripted=true;
    ck(TrackedFrameCamera55(10.3f,20.2f,6.456f,.7f,.7f,0,x,y,z,hx,hy),"saved FREE VRIK alchemy admits its exact body and camera without the legacy H51 test switch");
    kcd2vr::alchemy_own_head_h51::enabled=true;
    ck(TrackedFrameCamera55(10.3f,20.2f,6.456f,.7f,.7f,0,x,y,z,hx,hy)&&near(x,10)&&near(y,20)&&near(z,6.756f)&&near(hx,0)&&near(hy,1),
       "H51 current committed alchemy body and camera share anchor while native gameplay is held OFF");
    auto ordinary=frame;ordinary.workstationKindH51=false;ordinary.workstationModeEpochH51=ordinary.workstationLifetimeH51=0;ordinary.workstationInstanceH51=0;publish51(ordinary);
    ck(!TrackedFrameCamera55(10.3f,20.2f,6.456f,.7f,.7f,0,x,y,z,hx,hy),"H51 switch and activity alone cannot enable an ordinary unowned frame");
    publish51(frame);
    for(unsigned mutation:{1u,2u,3u,4u,5u,6u,8u,9u,10u,11u,12u,13u}){
        worldMutation51=mutation;
        char labelD53[160]{};std::snprintf(labelD53,sizeof(labelD53),"H51 owner/context/native-lifetime mutation=%u during actual world read retires the camera result",mutation);
        ck(!TrackedFrameCamera55(10.3f,20.2f,6.456f,.7f,.7f,0,x,y,z,hx,hy),labelD53);
        restore();g_gameplay_allowed=false;g_dialogue_body178r9=true;g_ctx_scripted=true;kcd2vr::alchemy_own_head_h51::enabled=true;
        mh::setMode(mh::Alchemy,mh::Gesture);publishAlchemy();
        ck(TrackedFrameCamera55(10.3f,20.2f,6.456f,.7f,.7f,0,x,y,z,hx,hy),"H51 fresh committed owner resumes after its explicit replacement");
    }
    mh::setMode(mh::Alchemy,mh::SemiPassenger);mh::setMode(mh::Alchemy,mh::Gesture);
    ck(!TrackedFrameCamera55(10.3f,20.2f,6.456f,.7f,.7f,0,x,y,z,hx,hy),"H51 mode ABA cannot reuse the preceding mode's completed frame");
    publishAlchemy();ai::witness175.End(0xA000);ai::witness175.Begin(ai::Kind175::Alchemy,0xA000,now51);
    ck(!TrackedFrameCamera55(10.3f,20.2f,6.456f,.7f,.7f,0,x,y,z,hx,hy),"H51 same-address native End/Begin cannot borrow a preceding lifetime's frame");
    publishAlchemy();ai::witness175.End(0xA000);
    ck(!TrackedFrameCamera55(10.3f,20.2f,6.456f,.7f,.7f,0,x,y,z,hx,hy),"H51 ended native activity cannot borrow a still-fresh minigame-kind timestamp");
    g_gameplay_allowed=true;g_ctx_scripted=false;publish51(ordinary);
    ck(TrackedFrameCamera55(10.3f,20.2f,6.456f,.7f,.7f,0,x,y,z,hx,hy)&&near(hx,0)&&near(hy,0),"H51 ordinary exit keeps existing camera behavior without alchemy heading");
    kcd2vr::alchemy_own_head_h51::enabled=false;ai::witness175.Clear();ai::witness175.ready=false;mh::publish(mh::None,now51);
    std::printf("tracked_frame51_alchemy_camera=%u/%u; actual reader/service and cached native lifecycle; engine/headset=NOT_EXECUTED\n",checks-failures,checks);
    return failures?1:0;
}
int alchemyEntryMeasurementH51(){
    namespace ai=kcd2vr::activity_input163;namespace mh=kcd2vr::minigame_hands45;namespace ws=kcd2vr::workstation178r20c1;
    checks=failures=0;float x=0,y=0,z=0,hx=0,hy=0;unsigned identity=70;
    for(unsigned kind:{mh::Alchemy,mh::Smithing})for(bool primed:{false,true})for(unsigned beginAt:{0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,11u,12u}){
        entryKindH51=kind;mh::setMode(kind,mh::Gesture);++identity;now51+=10;ai::witness175.Clear();ai::witness175.ready=true;ai::witness175.transitionDepth175=0;
        mh::publish(mh::None,now51);g_cal_state=2;census51=3;g_head_valid=true;g_committed_pose_valid6689=true;
        g_pose_restore_fault6689=false;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_ctx_reading=false;
        g_gameplay_allowed=g_dialogue_body178r9=true;animation_filter::nativeBodyOwned112=false;animation_filter::kick=false;
        diagnostics::lab::reference=false;g_frame_control51=2;g_pose_epoch6689=identity;
        const auto actor=std::uintptr_t(0x1000+identity*0x100);g_henry_char_attachment_manager=reinterpret_cast<void*>(actor+8);
        packetReady51=worldReady51=true;worldMutation51=0;beginAlchemyOnPacketReadH51=packetNativeMutationH51=0;g_graspAcquisition54=nullptr;
        ws::experimentArmed36=false;kcd2vr::combat_rules178r10::f5Rung=0;kcd2vr::combat_rules178r10::workstationEnabled=true;
        kcd2vr::alchemy_own_head_h51::enabled=true;
        auto f=frame51();f.stamp={identity,2,now51,actor};publish51(f);
        if(primed)ck(TrackedFrameCamera55(10,20,6.756f,0,1,0,x,y,z,hx,hy),"H51 entry control learns genuine ordinary eye before native Begin");
        if(beginAt==0)BeginAlchemyEntryH51();else if(beginAt==1)worldMutation51=14;else if(beginAt==2)beginAlchemyOnPacketReadH51=2;
        else if(beginAt<6){BeginAlchemyEntryH51();worldMutation51=beginAt==3?15u:beginAt==4?9u:11u;}
        else if(beginAt==6)BeginAlchemyNativeOnlyH51();else if(beginAt==7)worldMutation51=16;
        else if(beginAt==8){BeginAlchemyNativeOnlyH51();worldMutation51=15;}
        else if(beginAt==9)ai::witness175.transitionDepth175=1;else if(beginAt==10)worldMutation51=17;
        else {beginAlchemyOnPacketReadH51=2;packetNativeMutationH51=beginAt==11?2u:1u;}
        const bool early=TrackedFrameCamera55(10.2f,20.1f,6.606f,.6f,.8f,0,x,y,z,hx,hy);
        ck(early==primed&&(!early||(near(x,10)&&near(y,20)&&near(z,6.756f))),
           "H51 native Begin before/read/final-commit cannot turn previous ordinary neutral stamp into authored-eye calibration");
        ws::WorkstationOwnerH51 owner{};
        if(beginAt<3||(beginAt>=6&&beginAt!=8)){
        if(beginAt>=9&&beginAt<=11){ai::witness175.transitionDepth175=0;BeginAlchemyNativeOnlyH51();}
        if(beginAt>=6){mh::setMode(kind,mh::Gesture);mh::publish(kind,now51);}
        ck(ws::workstationOwnerH51(now51,owner),"H51 entry schedule reaches actual existing native alchemy owner");
        f.workstationKindH51=kind;f.neutral=false;f.nativeHead={.2,.1,1.55};
        f.workstationModeEpochH51=owner.modeEpoch;f.workstationLifetimeH51=owner.lifetime;f.workstationInstanceH51=owner.instance;
        ++f.stamp.sample;f.stamp.tick=++now51;mh::publish(kind,now51);publish51(f);
        const bool after=TrackedFrameCamera55(10.2f,20.1f,6.606f,.6f,.8f,0,x,y,z,hx,hy);
        ck(after==primed&&(!after||(near(x,10)&&near(y,20)&&near(z,6.756f)&&near(hx,0)&&near(hy,1))),
           "H51 committed activity uses only established neutral anatomy; unprimed native view stays unmodified");
        }else ck(!ws::workstationOwnerH51(now51,owner)||mh::modeFor(kind)!=mh::Gesture,
                 "H51 authored input then native End/policy exit during world read retires activity presentation");
        ai::witness175.End(0xA000);mh::publish(mh::None,++now51);f.workstationKindH51=false;f.neutral=true;
        f.workstationModeEpochH51=f.workstationLifetimeH51=0;f.workstationInstanceH51=0;f.nativeHead={0,0,1.7};
        ++f.stamp.sample;f.stamp.tick=now51;publish51(f);
        ck(TrackedFrameCamera55(10,20,6.756f,0,1,0,x,y,z,hx,hy)&&near(x,10)&&near(y,20)&&near(z,6.756f),
           "H51 entry without prior measurement can learn again from a genuine ordinary exit frame");
    }
    entryKindH51=mh::Alchemy;kcd2vr::alchemy_own_head_h51::enabled=false;ai::witness175.Clear();ai::witness175.ready=false;
    mh::publish(mh::None,now51);std::printf("tracked_frame51_alchemy_entry=%u/%u; actual reader/service; engine/headset=NOT_EXECUTED\n",checks-failures,checks);
    return failures?1:0;
}
int pickupGapD53(){
    namespace ch=kcd2vr::camera_hold_e1;checks=failures=0;float x=0,y=0,z=0;
    ch::pickupBridgeD55=true;
    f::Frame frame{};
    const auto seed=[&](bool activePickup){
        ch::maxHoldMs=1500;now51+=2000;
        census51=3;g_cal_state=2;g_head_valid=true;g_pose_restore_fault6689=false;g_committed_pose_valid6689=true;
        g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_ctx_reading=false;
        animation_filter::nativeBodyOwned112=false;animation_filter::kick=false;diagnostics::lab::reference=false;
        g_gameplay_allowed=true;g_graspAcquisition54=nullptr;pickupCameraReady54=false;worldMutation51=0;
        packetReady51=worldReady51=true;g_frame_control51=2;g_pose_epoch6689=71;
        g_henry_char_attachment_manager=reinterpret_cast<void*>(0x708);
        kcd2vr::activity_input163::witness175.Clear();kcd2vr::activity_input163::witness175.ready=false;
        kcd2vr::minigame_hands45::publish(kcd2vr::minigame_hands45::None,now51);
        frame=frame51();frame.stamp={71,now51,now51,0x700};publish51(frame);
        ck(TrackedFrameCamera51(10,20,6.756f,x,y,z),"D53 establish neutral plain base in actual reader");
        pickupCamera54={};pickupCamera54.character=0x700;pickupCamera54.action=0x2000;pickupCamera54.scope=0x3000;
        pickupCamera54.serial=99;pickupCamera54.epoch=71;pickupCamera54.startedAt=pickupCamera54.tick=now51;
        pickupCamera54.nativeToken=17;pickupCamera54.side=0;pickupCamera54.active=true;pickupCamera54.generation55=42;
        pickupCamera54.trackingEpoch55=tracking59c6::epoch.load();
        g_graspAcquisition54=ReadPickupCamera54;pickupCameraReady54=true;g_gameplay_allowed=false;g_ctx_scripted=true;
        if(activePickup){frame.neutral=false;frame.acquisitionSerial=99;frame.acquisitionAction=0x2000;
            frame.acquisitionScope=0x3000;frame.acquisitionToken=17;frame.nativeHead={0,.2,1.5};publish51(frame);
            ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z)&&near(y,20)&&near(z,6.756f),"D53 completed exact pickup keeps neutral camera base");}
    };
    seed(false);packetReady51=false;
    ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z)&&near(y,20)&&near(z,6.756f),
       "D53 active pickup may bridge missing first completed pose using prior plain base");
    seed(true);packetReady51=false;world51.t={10.05,20.1,5.02};
    ck(TrackedFrameCamera51(10.05f,20.3f,6.576f,x,y,z)&&near(x,10.05f)&&near(y,20.1f)&&near(z,6.776f),
       "D53 pickup packet gap follows current root without native forward/down snap");
    seed(true);g_frame_publication51.fetch_add(1);
    ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z)&&near(y,20)&&near(z,6.756f),
       "D53 same pickup retains base during odd body publication");
    for(unsigned race:{4u,5u}){seed(true);worldMutation51=race;
        ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z)&&near(y,20)&&near(z,6.756f),
           "D53 final packet/publication churn is a same-owner gap, not native camera takeover");}
    for(unsigned change=0;change<10;++change){seed(true);packetReady51=false;
        switch(change){case 0:++pickupCamera54.serial;break;case 1:++pickupCamera54.scope;break;
          case 2:++pickupCamera54.nativeToken;break;case 3:++pickupCamera54.generation55;break;
          case 4:++pickupCamera54.action;break;case 5:pickupCameraReady54=false;break;
          case 6:tracking59c6::Reset();break;case 7:g_pose_restore_fault6689=true;break;
          case 8:g_ctx_menu=true;break;case 9:g_tool_context19c4r2=(now51-81)<<8;break;}
        ck(!TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),"D53 replaced/retired/stale/native pickup owner cannot borrow held base");}
    for(unsigned race:{1u,2u,3u,6u,7u,18u}){seed(true);packetReady51=false;worldMutation51=race;
        ck(!TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),"D53 owner/action/tracking change during root read rejects held result");}
    seed(true);packetReady51=false;now51+=1501;pickupCamera54.tick=now51;g_tool_context19c4r2=now51<<8;
    ck(!TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),"D53 fresh action cannot renew expired camera sample");
    // Default path and experiment handoff regressions. No pickup bridge while off.
    seed(true);ch::pickupBridgeD55=false;packetReady51=false;
    ck(!TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),"D55 disabling bridge retires pickup-specific base immediately");
    g_graspAcquisition54=nullptr;g_gameplay_allowed=true;g_ctx_scripted=false;
    ck(!TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),"D55 disabled pickup base cannot leak into later plain gameplay");
    ch::pickupBridgeD55=true;seed(false);packetReady51=false;worldMutation51=19;
    ck(!TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),"D55 disabling during current-root read refuses experimental result");
    ch::pickupBridgeD55=false;seed(false);packetReady51=false;
    ck(!TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),"D55 default OFF preserves refusal across first pickup packet gap");
    ch::pickupBridgeD55=true;seed(true);packetReady51=false;
    g_graspAcquisition54=nullptr;pickupCameraReady54=false;g_gameplay_allowed=true;g_ctx_scripted=false;
    ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z)&&near(y,20)&&near(z,6.756f),
       "D64 completed exact pickup bridges a missing plain successor without native eye snap");
    for(unsigned age:{60u,61u}){seed(true);packetReady51=false;now51+=age;g_tool_context19c4r2=now51<<8;
        g_graspAcquisition54=nullptr;pickupCameraReady54=false;g_gameplay_allowed=true;g_ctx_scripted=false;
        ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z)==(age<=60),
           "D64 retired pickup anatomical bridge expires after 60 ms without renewing age");}
    for(unsigned age:{60u,61u}){seed(false);packetReady51=false;now51+=age;pickupCamera54.tick=now51;g_tool_context19c4r2=now51<<8;
        ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z)==(age<=60),
           "D64 first pickup cannot borrow a plain base older than 60 ms");}
    seed(true);worldMutation51=22;
    ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z)&&near(y,20)&&near(z,6.756f),
       "D64 Picking retires during current-root read and bridges the same body into plain gameplay");
    seed(true);packetReady51=false;worldMutation51=22;
    ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z)&&near(y,20)&&near(z,6.756f),
       "D64 Picking retires during bridge root read with current HMD admission preserved");
    seed(true);g_graspAcquisition54=nullptr;pickupCameraReady54=false;g_gameplay_allowed=true;g_ctx_scripted=false;
    ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z)&&near(y,20)&&near(z,6.756f),
       "D64 plain entry plus still-Picking completed frame covers recorded zero-action final refusal");
    seed(true);worldMutation51=23;worldReads51=0;
    ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z)&&worldReads51==2&&near(y,20)&&near(z,6.756f),
       "D64 completed plain publication wins one fresh retry after Picking retirement");
    seed(false);g_graspAcquisition54=nullptr;pickupCameraReady54=false;g_gameplay_allowed=true;g_ctx_scripted=false;worldMutation51=24;
    ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z)&&near(y,20)&&near(z,6.756f),
       "D64 exact Picking begins during plain root read without losing VR camera admission");
    seed(false);packetReady51=false;g_graspAcquisition54=nullptr;pickupCameraReady54=false;g_gameplay_allowed=true;g_ctx_scripted=false;
    now51+=61;pickupCamera54.tick=now51;g_tool_context19c4r2=now51<<8;worldMutation51=24;
    ck(!TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),
       "D64 pickup beginning during bridge root read reapplies 60 ms cap to old plain base");
    seed(true);worldMutation51=25;
    ck(!TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),
       "D64 replaced Picking action during root read cannot inherit another action bridge");
    ch::pickupBridgeD55=true;seed(false);g_graspAcquisition54=nullptr;pickupCameraReady54=false;
    g_gameplay_allowed=true;g_ctx_scripted=false;worldMutation51=5;
    ck(!TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),"D55 final plain publication mismatch keeps old owner-loss semantics even with trial on");
    ch::pickupBridgeD55=true;seed(true);packetReady51=false;g_head_valid=false;
    ck(!TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),"D55 current HMD loss never becomes long held tracking");
    ch::pickupBridgeD55=true;seed(true);packetReady51=false;g_ctx_cutscene=true;
    ck(!TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),"D55 native cutscene camera yields and retires cache");
    ch::pickupBridgeD55=false;seed(false);packetReady51=false;g_graspAcquisition54=nullptr;
    g_gameplay_allowed=true;g_ctx_scripted=false;world51.t={10.05,20.1,5.02};
    ck(TrackedFrameCamera51(10.05f,20.3f,6.576f,x,y,z)&&near(x,10.05f)&&near(y,20.1f)&&near(z,6.776f),
       "D55 default OFF retains existing plain anatomical hold and CURRENT root travel");
    // Existing F5 core evidence survives integration; bridge receipts are
    // separately identified and do not pretend to be final rendered pixels.
    namespace cap=kcd2vr::diagnostics::capture168;namespace ic=kcd2vr::diagnostics::interaction_cycle_h4;
    ch::pickupBridgeD55=true;seed(true);cap::gate.select(cap::Domain::Interaction,true,now51);
    ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),"D55 diagnostic accepted pickup invokes actual camera service");
    ic::Event event{};bool acceptedReceipt=false;
    while(ic::recorder.pop(event))if(!std::strcmp(event.label,"D53_CAMERA_CORE_NOT_FINAL_RENDER")&&event.code==1)
        acceptedReceipt=event.actor==0x700&&event.poseEpoch==71&&event.action==0x2000&&event.token==99&&
            event.sourceTick==packet51.stamp.tick&&event.generation==packet51.stamp.sample&&
            event.collector==g_frame_publication51.load()&&near(event.point[2],6.756f);
    ck(acceptedReceipt,"D55 existing F5 core actor/sample/publication/action/solved-eye provenance retained");
    packetReady51=false;ck(TrackedFrameCamera51(10,20.2f,6.556f,x,y,z),"D55 F5 captures held pickup gap");
    bool refusedCore=false,heldReceipt=false;
    while(ic::recorder.pop(event)){
        if(!std::strcmp(event.label,"D53_CAMERA_CORE_NOT_FINAL_RENDER"))
            refusedCore=!event.code&&!std::strcmp(event.reason,"TRACKED_CAMERA_PACKET_JOIN_UNAVAILABLE");
        if(!std::strcmp(event.label,"D55_CAMERA_BRIDGE_NOT_FINAL_RENDER"))
            heldReceipt=event.code==int(ch::Why::Held)&&event.actor==0x700&&event.action==0x2000&&event.token==99&&
                event.trackingEpoch==tracking59c6::epoch.load()&&event.entity==0x3000&&event.target==17&&event.collector==2;
    }
    ck(refusedCore&&heldReceipt,"D55 precise core refusal and separately labelled bridge receipt both recorded");
    cap::gate.select(cap::Domain::Interaction,false,now51);const auto pushed=ic::recorder.pushed.load();
    (void)TrackedFrameCamera51(10,20.2f,6.556f,x,y,z);
    ck(ic::recorder.pushed.load()==pushed,"D55 F5 off produces no core or bridge recorder traffic");
    ch::pickupBridgeD55=false;
    std::printf("tracked_camera_pickup_d55=%u/%u; actual production reader; native/headset=NOT_EXECUTED\n",checks-failures,checks);
    return failures?1:0;
}
}
int main(){const int baseline=kcd2vr::frik_pose_probe::tests51();const int alchemy=kcd2vr::frik_pose_probe::alchemyTestsH51();
    const int entry=kcd2vr::frik_pose_probe::alchemyEntryMeasurementH51();const int pickup=kcd2vr::frik_pose_probe::pickupGapD53();
    return entry||alchemy||baseline||pickup;}
