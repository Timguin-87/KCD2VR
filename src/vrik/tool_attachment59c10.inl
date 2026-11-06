#include "alchemy_tool_contact_d55.h"
#include "../experiments/contact_handover_d71.h"
// F5=2: late placement of a tool which KCD2 has ALREADY put in Henry's hand.
// Reuses the exact retail CEntityAttachment / CAttachmentBONE placement contract.
// No invented attachment, inventory transfer, object lifetime or rigid constraint.
static bool ValidPose6689(const QuatT28&) noexcept;
static const char* WorkstationContextRefusal19c4r2(std::uint64_t now,bool requireFresh) noexcept {
    return kcd2vr::presentation_context55::refusal(g_tool_context19c4r2.load(std::memory_order_acquire),now,requireFresh);
}
static const char* WorkstationToolReason19c4r2(std::uint64_t now) noexcept {
    using namespace kcd2vr::activity_input163;
    if(!kcd2vr::workstation178r20c1::ownsActivityWrist45(now))return "WORKSTATION_WRIST_NOT_ADMITTED"; // C45: or MINIGAME HANDS semi-passenger
    if(const auto* reason=WorkstationContextRefusal19c4r2(now,true))return reason;
    if(!g_dialogue_body178r9.load(std::memory_order_acquire))return "WORKSTATION_BODY_NOT_ADMITTED";
    if(!TrackedHands59c7f())return "WORKSTATION_TRACKED_HANDS_NOT_SELECTED";
    const auto tick=kcd2vr::workstation178r20c1::heldToolActivityTick59c10.load(std::memory_order_acquire);
    if(!tick||now<tick||now-tick>80)return "WORKSTATION_STAGE_STALE";
    const int stage=kcd2vr::workstation178r20c1::heldToolActivity59c10.load();
    const bool current=(stage==int(kcd2vr::workstation178r20c1::Kind::Alchemy)&&witness175.active[unsigned(Kind175::Alchemy)].load())||
        ((stage==int(kcd2vr::workstation178r20c1::Kind::Anvil)||stage==int(kcd2vr::workstation178r20c1::Kind::SmithingOther))&&
         witness175.active[unsigned(Kind175::Smithing)].load());
    return current?nullptr:"WORKSTATION_NATIVE_ACTIVITY_CHANGED";
}
static bool WorkstationToolPose18(std::uint64_t now) noexcept {return WorkstationToolReason19c4r2(now)==nullptr;}
static bool ReadGraspPresentationD72(kcd2vr::grasp13::Binding& out) noexcept {
    out={};
    if(g_graspBinding13&&g_graspBinding13(out)&&((out.attached&&out.place)||out.socketOnly28))return true;
    // The existing reader copied the same POD even when semantic ownership is
    // false. Avoid a second mailbox/raw-input read on every unrelated attachment.
    // R94: native-acknowledged ordinary pickup presentation is production behavior.
    // The owner still proves the exact active request, Grip, lifetime and 80 ms freshness.
    if(!out.acceptedAttachTickD72){out={};return false;}
    return action_time_owner59c11::ReadPendingAttachmentPresentationD72(out);
}
// C55: an ordinary native Picking action can temporarily revoke gameplay.
// That is not a workstation. Permit ONLY the same already attached ordinary
// item (or socket observation) under its exact current acquisition witness.
// This never promotes PendingAttach to HELD and never survives native cleanup
// revoking ReadGraspBinding. All gameplay and native inventory owners stay off.
static bool GraspAcquisitionVisual55(const kcd2vr::grasp13::Binding& b,std::uint64_t now,
                                    kcd2vr::grasp13::Acquisition54* proof=nullptr) noexcept {
    if(proof)*proof={};
    if(!((b.attached&&b.place)||b.socketOnly28||b.acceptedAttachTickD72)||!kcd2vr::grasp13::sameHeldResource54(b,b)||
       b.side<0||b.side>1||b.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       !kcd2vr::visual_selection59c7f::Selected(frik_pose_probe::CalibrationReady(),frik_pose_probe::CombatCensusMode(),frik_pose_probe::FinalSkinTestMode())||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||
       animation_filter::nativeBodyOwned112.load()||animation_filter::NativeKickActive175()||
       kcd2vr::activity_input163::witness175.Active()||
       WorkstationContextRefusal19c4r2(now,true))return false;
    kcd2vr::grasp13::Acquisition54 acquisition{};
    bool haveAcquisition=false;
    if(b.acceptedAttachTickD72&&!b.attached&&!b.socketOnly28){
        kcd2vr::grasp13::Binding pending{};
        haveAcquisition=action_time_owner59c11::ReadPendingAttachmentPresentationD72(pending)&&
            kcd2vr::grasp13::same(b,pending)&&pending.acceptedAttachTickD72==b.acceptedAttachTickD72;
        if(haveAcquisition)acquisition=pending.acceptedAcquisitionD72;
    } else haveAcquisition=action_time_owner59c11::ReadAcquisition54(acquisition);
    const bool current=haveAcquisition&&acquisition.active&&
        kcd2vr::grasp13::freshAcquisition54(acquisition,now)&&
        acquisition.character==b.character&&acquisition.epoch==b.epoch&&acquisition.side==b.side&&
        acquisition.graspToken==b.token&&acquisition.graspKey==b.key&&
        acquisition.graspItem==b.item&&acquisition.graspEntity==b.entity;
    if(current&&proof)*proof=acquisition;
    return current;
}
// The action owner grants this small visual-only gap only AFTER native cleanup
// and every native callback have returned, before its next controller adoption.
// No action/logic pointer is retained or read by this presentation route.
static bool FinishingVisualContextR94() noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(diagnostics::lab::NativeReference()||NativePassengerCurrentA34(now)||g_pose_restore_fault6689.load()||
       !kcd2vr::visual_selection59c7f::Selected(frik_pose_probe::CalibrationReady(),frik_pose_probe::CombatCensusMode(),frik_pose_probe::FinalSkinTestMode())||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||
       animation_filter::nativeBodyOwned112.load()||animation_filter::NativeKickActive175()||
       kcd2vr::activity_input163::witness175.Active()||WorkstationContextRefusal19c4r2(now,true))return false;
    return true;
}
static bool ReadFinishingVisualR94(kcd2vr::grasp13::FinishingPresentationR94& out) noexcept {
    // With no finishing receipt the owner returns before any raw/native reads.
    return (action_time_owner59c11::ReadFinishingAttachmentPresentationR94(out)||
        action_time_owner59c11::ReadAdoptedAttachmentPresentationR95(out))&&FinishingVisualContextR94();
}
static bool CurrentFinishingVisualR94(const kcd2vr::grasp13::FinishingPresentationR94& receipt) noexcept {
    return action_time_owner59c11::FinishingOrHeldPresentationCurrentR94(receipt)&&FinishingVisualContextR94();
}
static bool FinishingNativeHandR94(const kcd2vr::grasp13::Binding& b) noexcept {
    std::uint64_t key=0,itemKey=0;void* link=nullptr;void* player=nullptr;std::uint32_t id=0,actor=0;
    auto* item=reinterpret_cast<unsigned char*>(b.item);auto* manager=reinterpret_cast<unsigned char*>(b.manager);
    if(!retail_build112::Ready(WhBase())||!b.token||!b.key||!b.entity||!b.actor||!item||!manager||b.side<0||b.side>1||
       !NativeHandKey6697(manager,b.side,key)||key!=b.key||!PhysVft6698(item,0x3A2DA88)||
       !Read(item+0x30,itemKey)||itemKey!=b.key||!Read(item+0x88,link)||!link||
       !Read(static_cast<unsigned char*>(link)+0x30,id)||id!=b.entity||
       !Read(manager+0x38,player)||!player||!Read(static_cast<unsigned char*>(player)+0x30,actor)||actor!=b.actor)return false;
    void* again=nullptr;return Read(item+0x88,again)&&again==link&&Read(item+0x30,itemKey)&&itemKey==b.key&&
        NativeHandKey6697(manager,b.side,key)&&key==b.key;
}
static const char* ToolExperimentReason59c10(std::uint64_t now) noexcept {
    if(NativePassengerCurrentA34(now))return "NATIVE_PASSENGER_A34";
    kcd2vr::grasp13::Binding grasp{};
    kcd2vr::grasp13::FinishingPresentationR94 finishing{};
    const bool owned=ReadGraspPresentationD72(grasp);
    const bool finishingOwned=(!owned||!g_gameplay_allowed.load(std::memory_order_acquire))&&
        ReadFinishingVisualR94(finishing)&&(!owned||kcd2vr::grasp13::sameHeldResource54(grasp,finishing.binding));
    if(!owned&&!finishingOwned){
        if(!kcd2vr::workstation178r20c1::fullVrAlchemy63(now)){
            if(kcd2vr::combat_rules178r10::f5Rung.load()!=2)return "EXPERIMENT_RUNG_OFF";
            if(!kcd2vr::workstation178r20c1::experimentArmed36.load())return "EXPERIMENT_NOT_ADMITTED";
            if(!kcd2vr::workstation178r20c1::ownsWrist(now))return "WRIST_OWNER_NOT_ADMITTED";
        }
    }
    if(diagnostics::lab::NativeReference())return "NATIVE_REFERENCE";
    // This coherent publication precedes the general gameplay flag update. A real
    // menu wins even if a render callback still observes the preceding gameplay flag.
    if(const auto* reason=WorkstationContextRefusal19c4r2(now,false))return reason;
    // Native minigame controls intentionally keep gameplay/weapon authority OFF.
    // Their separately admitted tracked body may still place its own held tool.
    if(!g_gameplay_allowed.load()&&!(owned&&GraspAcquisitionVisual55(grasp,now))&&!finishingOwned)
        if(const auto* reason=WorkstationToolReason19c4r2(now))return reason;
    if(!retail_build112::Ready(WhBase()))return "RETAIL_FINGERPRINT";
    if(g_pose_restore_fault6689.load())return "POSE_RESTORE_FAULT_LATCH";
    return nullptr;
}
static bool ToolExperiment59c10(std::uint64_t now) noexcept {return ToolExperimentReason59c10(now)==nullptr;}
// C37 SMITHING AUTO-HOLD: a distinct owner for the PINNED native smithing tools only
// (hammer hand_right, tongs hand_left, sword workpiece entity_left; vrik/smith_tool36.h).
// No Grip threshold and no F5 rung: setting on + smithing minigame + admitted tracked wrist.
// The controller places the native tool entity at the controller hand; the native wrist
// is never a passenger. Any change of minigame, phase, holder, hand slot or attachment
// object fails the exact binding and placement stops in that same callback (native pose).
static const char* SmithAutoHoldReason36(std::uint64_t now) noexcept {
    using namespace kcd2vr::activity_input163;
    if(!kcd2vr::workstation178r20c1::smithAutoHold36.load(std::memory_order_acquire))return "SMITH_AUTO_HOLD_OFF";
    if(!witness175.ready.load(std::memory_order_acquire)||!witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire))return "SMITH_MINIGAME_INACTIVE";
    if(!kcd2vr::workstation178r20c1::ownsActivityWrist45(now))return "WRIST_OWNER_NOT_ADMITTED"; // C45: F5/F10 rung 2 or MINIGAME HANDS semi-passenger
    if(diagnostics::lab::NativeReference())return "NATIVE_REFERENCE";
    if(const auto* reason=WorkstationContextRefusal19c4r2(now,false))return reason;
    if(!g_gameplay_allowed.load())if(const auto* reason=WorkstationToolReason19c4r2(now))return reason;
    if(!retail_build112::Ready(WhBase()))return "RETAIL_FINGERPRINT";
    if(g_pose_restore_fault6689.load())return "POSE_RESTORE_FAULT_LATCH";
    return nullptr;
}
static bool SmithNativeHeatingD56(const kcd2vr::smith_tool36::Binding& b) noexcept {
    if(kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Smithing)!=kcd2vr::minigame_hands45::SixDofVrik)return false;
    const auto rd=[](std::uintptr_t at,auto& value){return Read(reinterpret_cast<void*>(at),value);};
    return kcd2vr::smith_tool36::detachedForgeD56(rd,WhBase(),b);
}
#include "native_activity_hands46.inl"
static bool NativeAlchemyFingers18(std::uintptr_t character,std::uint64_t epoch,int side,std::uint64_t now) noexcept {
    if(NativeActivityHandOwned46(true,character,epoch,side,now))return true;
    if(side<0||side>1||!ToolExperiment59c10(now))return false;
    ToolSocket59c10 socket{};kcd2vr::alchemy_tool18::Binding live{};
    return g_tool_socket59c10[side].read(socket)&&bool(socket.alchemy18)&&socket.epoch==epoch&&
        socket.side==side&&socket.tick&&now>=socket.tick&&now-socket.tick<=80&&
        socket.request==kcd2vr::workstation178r20c1::experimentEpoch36.load()&&
        ReadAlchemyTool18(socket.attachment,socket.id,character,live)&&kcd2vr::alchemy_tool18::same(socket.alchemy18,live);
}
// C37: an auto-held smithing tool keeps the native finger grip of its own hand (no Grip needed).
static bool NativeSmithFingers36(std::uintptr_t character,std::uint64_t epoch,int side,std::uint64_t now) noexcept {
    // Exact native finger locals do not grant native wrist motion. Keep them
    // for the hammer and both 6DOF hands, including before first tool placement.
    if(NativeActivityHandOwned46(false,character,epoch,side,now,nullptr,false))return true;
    if(side<0||side>1||SmithAutoHoldReason36(now))return false;
    ToolSocket59c10 socket{};kcd2vr::smith_tool36::Binding live{};
    return g_tool_socket59c10[side].read(socket)&&bool(socket.smith36)&&!socket.nativeGrasp13&&socket.epoch==epoch&&
        socket.side==side&&socket.tick&&now>=socket.tick&&now-socket.tick<=80&&
        socket.request==kcd2vr::workstation178r20c1::smithHoldEpoch36.load(std::memory_order_acquire)&&
        socket.modeEpoch67==kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire)&&
        ReadSmithTool36(socket.attachment,socket.id,character,live)&&kcd2vr::smith_tool36::same(socket.smith36,live);
}
static bool ToolReceipt59c10(std::uint64_t now,void* self,std::uint32_t target,int side,
    const char* reason,std::uint64_t sample=0,int joint=-1) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return false;
    using namespace kcd2vr::diagnostics::interaction_receipt59c10;
    static thread_local Gate receipt[3];auto& gate=receipt[side>=0&&side<2?side:2];
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
    if(!gate.observe(session,now,{reinterpret_cast<std::uintptr_t>(self)^reinterpret_cast<std::uintptr_t>(reason),target,side,0},500,384))return false;
    const auto stageTick=kcd2vr::workstation178r20c1::heldToolActivityTick59c10.load();
    char line[640]{};std::snprintf(line,sizeof(line),
        "C10 F5_TOOL_STAGE session=%llu target=%u attachmentOwner=%p hand=%d reason=%s requestEpoch=%llu sample=%llu socketJoint=%d workstation=%d stageAge=%llu actionTargetR=%u actionTargetL=%u observed=%llu changes=%llu coalesced=%llu releaseOwner=NATIVE_ATTACHMENT",
        (unsigned long long)session,target,self,side,reason,
        (unsigned long long)kcd2vr::workstation178r20c1::experimentEpoch36.load(),(unsigned long long)sample,joint,
        kcd2vr::workstation178r20c1::heldToolActivity59c10.load(),(unsigned long long)age(now,stageTick),
        g_native_use_target59c10[0].load(),g_native_use_target59c10[1].load(),
        (unsigned long long)gate.seen,(unsigned long long)gate.changes,(unsigned long long)gate.coalesced);
    diagnostic_trace::Write(line);return true;
}
#ifdef KCD2VR_CRAFT36B
// C43 GRAB_SOCKET (F5 rung 1, one line per grasp token and side): proves which transform the held prop uses. The item is placed at
// the game's own hand_right/hand_left socket (+9C relative = native attach of the item's slt_0 helper with the pickup request's
// quaternion), re-based from the ORIGINAL native hand bone onto the VR wrist: gripT/gripQ is that native per-item offset. The only
// wrist itself is controller-owned; outward clearance is bootstrap-only until the native canonical grip is captured. slt_0 and the request helper/quat are
// read from the exact pickup logic (C_PickUpItemLogic request at +0x28: helper +0x18, quaternion +0x20).
static void GrabSocketReceipt43(std::uint64_t now,void* self,std::uint32_t bound,int side,int joint,const kcd2vr::grasp13::Binding& grasp,
    const ToolSocket59c10& socket,const CommittedFrame6695& body,const QuatT28& model,const ph::Pose& target,const float (&wanted)[8]) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return;
    static thread_local std::uint64_t seen[2]{};if(side<0||side>1||seen[side]==grasp.token)return;seen[side]=grasp.token;
    const auto grip=RelativeFromAbsolute(body.allowed[side],model); // == native socket relative to the native hand bone (BuildToolPoses59c10)
    QuatT28 h0{};bool ok0=false;void* entity=EntityById6694(bound);
    if(entity&&g_attachment_helper_transform_original){__try{ok0=g_attachment_helper_transform_original(entity,"slt_0",&h0,true);}__except(EXCEPTION_EXECUTE_HANDLER){ok0=false;}}
    char helper[40]{};const char* helperPtr=nullptr;float rq[4]{};bool haveRequest=false;
    if(grasp.logic&&PhysVft6698(reinterpret_cast<void*>(grasp.logic),0x3A5B670)&&Read(reinterpret_cast<void*>(grasp.logic+0x40),helperPtr)&&Read(reinterpret_cast<void*>(grasp.logic+0x48),rq)){haveRequest=true;
        if(helperPtr)for(unsigned i=0;i+1<sizeof(helper);++i){char c=0;if(!Read(helperPtr+i,c)||!c)break;helper[i]=(c>=32&&c<127)?c:'?';}}
    const float wristT[3]{body.allowed[side].t[0],body.allowed[side].t[1],body.allowed[side].t[2]};
    const double dWrist=std::sqrt(double(model.t[0]-wristT[0])*(model.t[0]-wristT[0])+double(model.t[1]-wristT[1])*(model.t[1]-wristT[1])+double(model.t[2]-wristT[2])*(model.t[2]-wristT[2]));
    double dPalm=-1;if(body.palmValid59c9[side]){const auto& pm=body.palm59c9[side];dPalm=std::sqrt(double(model.t[0]-pm.x)*(model.t[0]-pm.x)+double(model.t[1]-pm.y)*(model.t[1]-pm.y)+double(model.t[2]-pm.z)*(model.t[2]-pm.z));}
    char line[700]{};std::snprintf(line,sizeof(line),
        "C43 GRAB_SOCKET side=%d target=%u token=%llu joint=%d socketRelT=(%.3f,%.3f,%.3f) socketRelQ=(%.3f,%.3f,%.3f,%.3f) gripT=(%.3f,%.3f,%.3f) gripQ=(%.3f,%.3f,%.3f,%.3f) slt_0=%d(%.3f,%.3f,%.3f q=%.3f,%.3f,%.3f,%.3f) request=%d helper=%s reqQ=(%.3f,%.3f,%.3f,%.3f) itemToWristM=%.3f itemToPalmM=%.3f clearanceM=%.3f worldItem=(%.3f,%.3f,%.3f) placedScale=%.3f source=NATIVE_SOCKET_ON_VR_WRIST(+clearance) self=%p",
        side,bound,(unsigned long long)grasp.token,joint,socket.relative.t[0],socket.relative.t[1],socket.relative.t[2],socket.relative.q[0],socket.relative.q[1],socket.relative.q[2],socket.relative.q[3],
        grip.t[0],grip.t[1],grip.t[2],grip.q[0],grip.q[1],grip.q[2],grip.q[3],ok0?1:0,h0.t[0],h0.t[1],h0.t[2],h0.q[0],h0.q[1],h0.q[2],h0.q[3],
        haveRequest?1:0,helper[0]?helper:"-",rq[0],rq[1],rq[2],rq[3],dWrist,dPalm,0.,
        double(target.p.x),double(target.p.y),double(target.p.z),double(wanted[7]),self);
    diagnostic_trace::Write(line);
}
#endif
// D71: a new native attachment need not wait another character callback just
// to correlate a KNOWN wrist/item-slot bone. These immutable ORIGINAL poses
// travelled with the same completed body; live (already VR-moved) joints are
// never read here. Unmapped joints keep the original mailbox bootstrap.
static bool OriginalToolGripD71(const CommittedFrame6695& body,const ToolSocket59c10& socket,
                                QuatT28& grip) noexcept {
    using namespace kcd2vr::contact_handover_d71;
    const int side=socket.side;const int nativeSideR93=socket.alchemy18?socket.alchemy18.side:side;const auto nowD71=std::uint64_t(GetTickCount64());
    // D72: a native minigame has already proved this exact hand membership.
    // Its first current-original placement is normal tracked presentation, not
    // contingent on the separate ordinary pickup handover experiment.
    const bool trackedMinigameD72=(socket.alchemy18&&kcd2vr::minigame_hands45::freeTrackedMode(
        kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Alchemy)))||
        (socket.smith36&&kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Smithing)==kcd2vr::minigame_hands45::SixDofVrik);
    // R94: the callback proved this ordinary grasp against the current native
    // binding and rechecks it before writing. Do not wait a second body packet
    // just because the unrelated legacy-tool handover trial is disabled.
    if((!socket.nativeGrasp13&&!trackedMinigameD72&&(!ItemHandover()||!body.itemHandoverRevisionD71||
        body.itemHandoverRevisionD71!=ItemRevision()))||!body.publicationR24||
       body.publicationR24!=ReadCompletedPublicationT23()||side<0||side>1||
       !body.valid[side]||!body.stamp.sample||!body.stamp.epoch||!body.stamp.actor||
       !body.stamp.tick||nowD71<body.stamp.tick||nowD71-body.stamp.tick>80||
       !TrackingHeadUsable111(nowD71)||!TrackingHandUsable111(side,nowD71)||
       socket.epoch!=body.stamp.epoch||reinterpret_cast<std::uintptr_t>(socket.character)!=body.stamp.actor||
       !socket.self||!socket.attachment||!socket.entity||!socket.id||!socket.request||
       socket.joint<0||!body.fistJointCountH31||unsigned(socket.joint)>=body.fistJointCountH31||
       nativeSideR93<0||nativeSideR93>1||!ValidPose6689(body.native[nativeSideR93])||!ValidPose6689(socket.relative))return false;
    const QuatT28* original=nullptr;
    if(socket.joint==body.fistWristBoneH31[nativeSideR93])original=&body.native[nativeSideR93];
    else if(socket.joint==body.fistSlotBoneH31[nativeSideR93]&&body.originalToolSlotValidD71[nativeSideR93])
        original=&body.originalToolSlotD71[nativeSideR93];
    if(!original||!ValidPose6689(*original))return false;
    const auto tool=AbsoluteFromRelative(*original,socket.relative);
    grip=RelativeFromAbsolute(body.native[nativeSideR93],tool);
    return ValidPose6689(tool)&&ValidPose6689(grip)&&Len(FromT(grip))<=.8f;
}
// Re-read the exact socket immediately before the existing native attachment
// writer. A switch/replacement/new body publication invalidates this bootstrap;
// socketOnly is never enough by itself and still grants no semantic HELD state.
static bool OriginalToolSocketCurrentD71(const CommittedFrame6695& body,const ToolSocket59c10& socket) noexcept {
    QuatT28 unused{},relative{};void* self=nullptr;void* manager=nullptr;void* character=nullptr;
    std::int16_t joint=-1;unsigned char deferred=255;std::uint32_t id=0;
    return OriginalToolGripD71(body,socket,unused)&&
        Read(static_cast<unsigned char*>(socket.attachment)+0x20,self)&&self==socket.self&&
        Read(static_cast<unsigned char*>(socket.attachment)+0x28,manager)&&manager&&
        Read(static_cast<unsigned char*>(manager)+0x70,character)&&character==socket.character&&
        Read(static_cast<unsigned char*>(self)+8,id)&&id==socket.id&&
        Read(static_cast<unsigned char*>(socket.attachment)+0x78,joint)&&joint==socket.joint&&
        Read(static_cast<unsigned char*>(socket.attachment)+0x7A,deferred)&&deferred<=1&&
        Read(static_cast<unsigned char*>(socket.attachment)+0x9C,relative)&&
        !std::memcmp(&relative,&socket.relative,sizeof(relative));
}
// The unique Picking action may end while this attachment callback is in
// flight. Its controller can publish the SAME held resource with logic=0.
// That is a presentation continuation, not a new native action/transfer grant.
// Current ownership must still be read independently before the final write.
static bool SameGraspPresentationR93R3(const kcd2vr::grasp13::Binding& before,
                                      const kcd2vr::grasp13::Binding& after) noexcept {
    using namespace kcd2vr::grasp13;
    if(same(before,after))return true;
    return before.logic&&!after.logic&&before.attached&&before.place&&after.attached&&after.place&&
        before.gripReady55&&after.gripReady55&&sameHeldResource54(before,after)&&
        valid(before.grip55)&&valid(after.grip55)&&
        !std::memcmp(&before.grip55,&after.grip55,sizeof(before.grip55));
}
static bool ToolAttachment59c10(void* self,void* attachment) noexcept {
    auto now=std::uint64_t(GetTickCount64());
    const char* experiment59c10=ToolExperimentReason59c10(now);
    const char* smithGate36=SmithAutoHoldReason36(now); // C37: separate owner gate, never opens the F5 routes
    if(experiment59c10&&smithGate36){
        // This is an ownership receipt, not a claim that an arbitrary attachment
        // is Henry's tool. No native reads are added on this early refusal path.
        if(kcd2vr::workstation178r20c1::heldToolActivity59c10.load()){
            const bool smithing36=kcd2vr::activity_input163::witness175.active[unsigned(kcd2vr::activity_input163::Kind175::Smithing)].load()!=0;
            ToolReceipt59c10(now,nullptr,0,-1,smithing36&&kcd2vr::workstation178r20c1::smithAutoHold36.load()?smithGate36:experiment59c10);}
        return false;
    }
    if(experiment59c10){void* am=nullptr; // C37 review: only the smith gate is open -> Henry's own attachments only, before any further reads
        if(!Read(static_cast<unsigned char*>(attachment)+0x28,am)||!am||am!=g_henry_char_attachment_manager.load(std::memory_order_acquire))return false;}
    ReadScope101R2 pages;
    std::uint32_t bound=0;
    if(!Read(static_cast<unsigned char*>(self)+8,bound)||!bound)return false;
    // Native lookup may overlap a newer station/control publication. This is
    // the validation clock only; no source receipt is renewed.
    now=std::uint64_t(GetTickCount64());
    // A live scenery rigid-body hold is the sole entity writer for this claim.
    if(kcd2vr::static_grab_r89::ownsEntityR94(bound))return false;
    // F12's actual weapons keep their sole owner. A tool must also have current
    // native hand membership; proximity or an action request alone is insufficient.
    bool relevantWeapon37=false;
    for(auto& weapon:g_phys_relevant_id101)if(weapon.load()==bound)relevantWeapon37=true;
    // C37R1 (ChatGPT): a weapon-classified tool is only arbitrated while native activity controls have revoked gameplay.
    if(relevantWeapon37&&(smithGate36||g_gameplay_allowed.load(std::memory_order_acquire)))return false;
    kcd2vr::grasp13::Binding grasp{};
    kcd2vr::grasp13::FinishingPresentationR94 finishingR94{};
    const bool ordinaryGrasp=ReadGraspPresentationD72(grasp)&&grasp.entity==bound&&grasp.side>=0&&grasp.side<2;
    const bool finishingGraspR94=(!ordinaryGrasp||!g_gameplay_allowed.load(std::memory_order_acquire))&&
        ReadFinishingVisualR94(finishingR94)&&finishingR94.binding.entity==bound&&
        (!ordinaryGrasp||kcd2vr::grasp13::sameHeldResource54(grasp,finishingR94.binding))&&
        FinishingNativeHandR94(finishingR94.binding);
    if(finishingGraspR94)grasp=finishingR94.binding;
    const bool nativeGrasp=ordinaryGrasp||finishingGraspR94;
    const bool acceptedPendingD72=nativeGrasp&&grasp.acceptedAttachTickD72&&!grasp.attached&&!grasp.socketOnly28;
    const auto wristSerial107=nativeGrasp?g_native_use_serial107[grasp.side].load(std::memory_order_acquire):0;
    if(nativeGrasp&&experiment59c10)return false; // C37: only the smith gate is open; a grasp never rides it
    // An owned apple does not authorize a different held tool or the other
    // hand. Only its exact binding bypasses the legacy F5 experiment gate.
    const bool legacy59c10=!experiment59c10&&(kcd2vr::workstation178r20c1::fullVrAlchemy63(now)||
       (kcd2vr::combat_rules178r10::f5Rung.load()==2&&kcd2vr::workstation178r20c1::experimentArmed36.load()&&kcd2vr::workstation178r20c1::ownsWrist(now)));
    if(!nativeGrasp&&!legacy59c10&&smithGate36)return false;
    // C10R19 (review): during native minigame controls gameplay authority is OFF. Only the admitted workstation body reads its committed
    // packet, and then only for the exact native alchemy binding (alchemy stage) or the CItem tool route at the smithing stages; a native
    // grasp never becomes controller-driven inside a minigame. The body is read only when needed (this runs for every attachment).
    const bool minigame19=!g_gameplay_allowed.load();
    kcd2vr::grasp13::Acquisition54 acquisition55{};
    const bool acquisitionVisual55=ordinaryGrasp&&minigame19&&GraspAcquisitionVisual55(grasp,now,&acquisition55);
    if(minigame19&&nativeGrasp&&!acquisitionVisual55&&!finishingGraspR94)return false;
    CommittedFrame6695 body{};bool haveBody=false;
    const auto readBody=[&]() noexcept {return haveBody||(haveBody=ReadCommittedPacketFor6695(body,
        minigame19&&WorkstationToolPose18(now),acquisitionVisual55?&acquisition55:nullptr,
        finishingGraspR94?&finishingR94:nullptr));};
    // C37: only Henry's own character attachments are read for a smithing binding (cheap pre-filter, no NPC work).
    kcd2vr::smith_tool36::Binding smith36{};bool nativeSmith36=false;
    if(!nativeGrasp&&!smithGate36){void* am36=nullptr;
        if(Read(static_cast<unsigned char*>(attachment)+0x28,am36)&&am36&&am36==g_henry_char_attachment_manager.load(std::memory_order_acquire)){
            if(!readBody())return false;
            nativeSmith36=ReadSmithTool36(attachment,bound,body.stamp.actor,smith36)&&smith36.object==reinterpret_cast<std::uintptr_t>(self);}}
    // D72 retained: detached forge blank does not revoke the tracked tongs.
    // Exactly one owner per stage: an exact smithing tool belongs to the auto-hold;
    // with the F5 route closed nothing else may continue from here.
    if(nativeSmith36&&kcd2vr::subsystems::smithing::bridge67::fixtureOwnedH21(smith36)&&
       !kcd2vr::subsystems::smithing::bridge67::fixtureJoined(body.smithFixture[smith36.side],smith36,
           body.stamp.epoch,kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire),
           body.stamp.sample,now))return false;
    if(relevantWeapon37&&!nativeSmith36)return false; // only an exact native smith tool changes ownership
    if(!nativeSmith36&&!nativeGrasp&&!legacy59c10)return false;
    kcd2vr::alchemy_tool18::Binding alchemy18{};bool nativeAlchemy18=false;
    if(!nativeGrasp&&kcd2vr::activity_input163::witness175.active[unsigned(kcd2vr::activity_input163::Kind175::Alchemy)].load()){
        if(!readBody())return false;
        nativeAlchemy18=ReadAlchemyTool18(attachment,bound,body.stamp.actor,alchemy18)&&alchemy18.object==reinterpret_cast<std::uintptr_t>(self);}
    if(minigame19&&!nativeGrasp&&!nativeAlchemy18&&kcd2vr::workstation178r20c1::heldToolActivity59c10.load()==int(kcd2vr::workstation178r20c1::Kind::Alchemy)){
        // Only Henry's observed attachment earns a refusal receipt; never format
        // every NPC/environment attachment while a workstation is active.
        void* am=nullptr;void* ch=nullptr;
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)&&body.stamp.actor&&
           PhysVft6698(attachment,0x3A657C0)&&Read(static_cast<unsigned char*>(attachment)+0x28,am)&&am&&
           Read(static_cast<unsigned char*>(am)+0x70,ch)&&reinterpret_cast<std::uintptr_t>(ch)==body.stamp.actor)
            ToolReceipt59c10(now,self,bound,-1,"ALCHEMY_ACTION_OR_CURRENT_ATTACHMENT_UNQUALIFIED");
        return false;
    }
    int side=nativeGrasp?grasp.side:nativeAlchemy18?alchemy18.side:nativeSmith36?smith36.side:-1;bool candidateSeen=false;
    for(int s=0;!nativeGrasp&&!nativeAlchemy18&&!nativeSmith36&&s<2;++s){std::uint32_t candidate=0,held=0;
        if(NativeCandidateItemId6699(s,candidate)&&candidate==bound){candidateSeen=true;
            if(NativeHeldItemId6694(s,held)&&held==bound){side=s;break;}}}
    if(side<0){
        if(candidateSeen||bound==g_native_use_target59c10[0].load()||bound==g_native_use_target59c10[1].load())
            ToolReceipt59c10(now,self,bound,-1,"NATIVE_HAND_MEMBERSHIP_MISSING");
        else if(kcd2vr::workstation178r20c1::heldToolActivity59c10.load()==int(kcd2vr::workstation178r20c1::Kind::Alchemy)){
            void* am=nullptr;void* ch=nullptr;
            if(readBody()&&PhysVft6698(attachment,0x3A657C0)&&Read(static_cast<unsigned char*>(attachment)+0x28,am)&&am&&
               Read(static_cast<unsigned char*>(am)+0x70,ch)&&reinterpret_cast<std::uintptr_t>(ch)==body.stamp.actor)
                ToolReceipt59c10(now,self,bound,-1,"ALCHEMY_ENTITY_NOT_IN_CURRENT_NATIVE_HAND");
        }
        return false;
    }
    const auto refuse=[&](const char* reason){ToolReceipt59c10(now,self,bound,side,reason);return false;};
    kcd2vr::subsystems::alchemy::bridge67::Control presentationControlD49{};
    const bool trackedAlchemyPresentationD39=nativeAlchemy18&&kcd2vr::workstation178r20c1::fullVrAlchemy63(now)&&
        kcd2vr::subsystems::alchemy::bridge67::presentationOwnsCurrentR95(alchemy18,body.stamp.epoch,[]() noexcept {return std::uint64_t(GetTickCount64());},&presentationControlD49,&side);
    if(nativeAlchemy18&&kcd2vr::workstation178r20c1::fullVrAlchemy63(now)&&
       !trackedAlchemyPresentationD39)return refuse("GESTURE_NATIVE_TOOL_PRESENTATION_REQUIRED");
    // This is presentation of an independently re-read native attachment, not
    // acquisition by an open hand. Only saved Free/6DOF may retain it during
    // an accepted release/return or delayed Attach; Native/Semi stay unchanged.
    const bool freeAlchemyPresentationD39=trackedAlchemyPresentationD39&&
        kcd2vr::minigame_hands45::freeTrackedMode(kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Alchemy));
    if(!readBody())return refuse("COMMITTED_WRIST_UNAVAILABLE"); // C10R19: the R18 receipt again
    if(acquisitionVisual55&&(body.stamp.actor!=grasp.character||body.stamp.epoch!=grasp.epoch||
       !GraspAcquisitionVisual55(grasp,std::uint64_t(GetTickCount64()))))return refuse("PICKING_VISUAL_OWNER_CHANGED");
    const auto stageTick=kcd2vr::workstation178r20c1::heldToolActivityTick59c10.load(std::memory_order_acquire);
    const auto stageNowR95=std::uint64_t(GetTickCount64());
    const bool station=stageTick&&stageNowR95>=stageTick&&stageNowR95-stageTick<=80&&
        kcd2vr::workstation178r20c1::heldToolActivity59c10.load()!=0;
    const bool action=now<g_native_use_until107[side].load()&&g_native_use_target59c10[side].load()==bound;
    if(!nativeGrasp&&!station&&!action)return refuse("NATIVE_STAGE_OR_ACCEPTED_ACTION_STALE");
    // Reading keeps the game's real book binding; page-turn buttons do not
    // release it. No Grip invents or acquires a book: ReadAlchemyTool18 proved it.
    const bool nativeBook71=nativeAlchemy18&&alchemy18.type==19&&kcd2vr::workstation178r20c1::fullVrAlchemy63(now);
    const float grip=side?g_left_grip.load():g_right_grip.load();
    if(!std::isfinite(grip)||(!nativeGrasp&&!nativeSmith36&&!nativeBook71&&!freeAlchemyPresentationD39&&grip<.25f))return refuse("GRIP_RELEASED_NATIVE_ATTACHMENT_RETAINS_OWNERSHIP");
    if(!TrackingHeadUsable111(now)||!TrackingHandUsable111(side,now))return refuse("TRACKING_UNUSABLE");
    if(!body.valid[side])return refuse("COMMITTED_HAND_INVALID");
    const auto request=nativeGrasp?grasp.token:nativeSmith36?kcd2vr::workstation178r20c1::smithHoldEpoch36.load():
        kcd2vr::workstation178r20c1::experimentEpoch36.load();
    const auto& old=body.tools59c10[side];
    const auto smithModeEpoch67=kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
    const bool mortarGrip69=nativeAlchemy18&&kcd2vr::workstation178r20c1::fullVrAlchemy63(now)&&
        old.valid&&old.socket.gripReady67&&old.socket.epoch==body.stamp.epoch&&
        old.socket.request==request&&old.socket.modeEpoch67==smithModeEpoch67&&old.socket.side==side&&old.socket.id==bound&&
        old.socket.self==self&&old.socket.attachment==attachment&&
        kcd2vr::alchemy_tool18::sameMortarGrip69(old.socket.alchemy18,alchemy18);
    const bool smithGrip67=nativeSmith36&&kcd2vr::workstation178r20c1::ownsActivityWrist45(now)&&
        old.valid&&old.socket.gripReady67&&old.socket.modeEpoch67==smithModeEpoch67&&
        old.socket.self==self&&old.socket.attachment==attachment&&old.socket.id==bound&&
        old.socket.request==request&&!old.socket.nativeGrasp13&&old.socket.epoch==body.stamp.epoch&&
        reinterpret_cast<std::uintptr_t>(old.socket.character)==body.stamp.actor&&
        kcd2vr::smith_tool36::sameCanonicalGripD56(old.socket.smith36,smith36);
    const bool continuing=old.valid&&old.socket.self==self&&old.socket.attachment==attachment&&
        (!(nativeSmith36||nativeAlchemy18)||old.socket.modeEpoch67==smithModeEpoch67)&&
        old.socket.id==bound&&old.socket.request==request&&old.socket.nativeGrasp13==nativeGrasp&&
        bool(old.socket.alchemy18)==nativeAlchemy18&&(!nativeAlchemy18||kcd2vr::alchemy_tool18::same(old.socket.alchemy18,alchemy18))&&
        bool(old.socket.smith36)==nativeSmith36&&(!nativeSmith36||kcd2vr::smith_tool36::same(old.socket.smith36,smith36));
    if(!nativeGrasp&&!nativeSmith36&&!continuing&&!mortarGrip69&&!nativeBook71&&!freeAlchemyPresentationD39&&grip<.55f)return refuse("ACQUISITION_GRIP_BELOW_THRESHOLD");
    if(!PhysVft6698(attachment,0x3A657C0)||!PhysSocketContract101(WhBase()))return refuse("ATTACHMENT_CLASS_OR_SOCKET_PINS");
    ToolSocket59c10 socket{};void* manager=nullptr;std::int16_t joint=-1;unsigned char deferred=0;
    if(!Read(static_cast<unsigned char*>(attachment)+0x28,manager)||!manager||
       !Read(static_cast<unsigned char*>(manager)+0x70,socket.character)||
       reinterpret_cast<std::uintptr_t>(socket.character)!=body.stamp.actor||
       !PhysVft6698(socket.character,0x40602F8)||
       !Read(static_cast<unsigned char*>(attachment)+0x78,joint)||joint<0||
       !Read(static_cast<unsigned char*>(attachment)+0x7A,deferred)||deferred>1||
       !Read(static_cast<unsigned char*>(attachment)+0x9C,socket.relative)||!ValidPose6689(socket.relative))return refuse("NATIVE_SOCKET_IDENTITY_OR_POSE");
    socket.self=self;socket.attachment=attachment;socket.side=side;socket.joint=joint;
    socket.id=bound;socket.entity=EntityById6694(bound);socket.tick=now;
    socket.epoch=body.stamp.epoch;socket.request=request;socket.nativeGrasp13=nativeGrasp;
    socket.alchemy18=alchemy18;socket.smith36=smith36;socket.modeEpoch67=(nativeSmith36||nativeAlchemy18)?smithModeEpoch67:0;
    const bool gesture67=(nativeAlchemy18&&kcd2vr::workstation178r20c1::fullVrAlchemy63(now))||
        (nativeSmith36&&kcd2vr::workstation178r20c1::ownsActivityWrist45(now));
    if(gesture67&&(continuing||smithGrip67||mortarGrip69)&&old.socket.gripReady67){socket.grip67=old.socket.grip67;socket.gripReady67=true;}
    if(nativeGrasp&&(grasp.character!=body.stamp.actor||grasp.epoch!=body.stamp.epoch))return refuse("GRASP_BODY_EPOCH_CHANGED");
    if(!socket.entity)return refuse("BOUND_ENTITY_UNAVAILABLE");
    const bool stableGrip55=nativeGrasp&&grasp.gripReady55&&kcd2vr::grasp13::valid(grasp.grip55)&&
        std::sqrt(double(grasp.grip55.p[0])*grasp.grip55.p[0]+double(grasp.grip55.p[1])*grasp.grip55.p[1]+
                  double(grasp.grip55.p[2])*grasp.grip55.p[2])<=.8;
    if(nativeGrasp&&grasp.gripReady55&&!stableGrip55)return refuse("CANONICAL_GRIP_INVALID");
    const bool correlated=(continuing||smithGrip67||mortarGrip69)&&old.socket.entity==socket.entity&&old.socket.character==socket.character&&
        old.socket.epoch==socket.epoch&&(stableGrip55||smithGrip67||mortarGrip69||(old.socket.joint==socket.joint&&
        !std::memcmp(&old.socket.relative,&socket.relative,sizeof(socket.relative))));
    // +7A announces pending native socket transforms, not detachment. Retail
    // 20E8290 copies the pending relative+F4 to current+9C before clearing it;
    // CEntityAttachment::UpdateAttachment/423974 do not consume that flag.
    // Publish ONLY the already-current validated+9C socket for the next
    // original-joint pose transaction, even when another transform is pending.
    // Refusing publication while dirty can starve bootstrap indefinitely.
    // New grips still require the original-joint/relative transaction below.
    // A canonical Gesture smith grip already proved for this exact tool keeps
    // its geometry across native phase/relative changes; its live phase, item,
    // holder, hand, lifetime and mode epoch were independently revalidated.
    // All other changed sockets require a new original-pose transaction.
    // A contended next-frame mailbox does not revoke the exact current
    // completed wrist/socket proof below. Returning native here alternated the
    // item's transform writer for one draw despite unchanged real ownership.
    if(!g_tool_socket59c10[side].publish(socket))
        (void)ToolReceipt59c10(now,self,bound,side,"SOCKET_PUBLICATION_DEFERRED_CURRENT_PROOF_REQUIRED");
    if(nativeAlchemy18&&alchemy18.side==0&&alchemy18.type==3&&
       kcd2vr::workstation178r20c1::fullVrAlchemy63(now)&&grip<.35f&&
       kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Alchemy)!=kcd2vr::minigame_hands45::SixDofVrik){
        // D53: mode3 keeps this exact still-native attachment on its tracked
        // wrist. D56 lets this presentation earn work only with the exact native
        // pair, the held left mortar and measured pestle motion/contact.
        // Opening the right hand must not restart the native arm loop.
        // Actual native detach/replacement still revokes the binding below.
        if(!presentationControlD49.passenger&&!presentationControlD49.controllerMortar70){
            // Right-only release pauses grinding and returns visual placement
            // to the actual native attachment. Keep its proven canonical grip
            // fresh for re-grip; do not cancel/detach the held LEFT mortar or
            // borrow the resource-carafe Return ABI for an unproven pestle.
            return refuse("PESTLE_RELEASE_NATIVE_PRESENTATION");
        }
    }
    // INVESTIGATE_D67R1[PICKUP_BOOTSTRAP]: D66 target261783/token3 took
    // 22-27ms across pending-attach/held in the sampled pickups, then another
    // 11-13ms across native finish/cleanup. This original-joint bootstrap and
    // FinishPickingForHold19c4r2 are separate visual handover boundaries.
    // Establish a same-item, same-attachment presentation/visibility bridge
    // without retaining revoked native pointers or repeating Attach/inventory.
    // Join final placement to the same drawn palette before claiming the
    // reported one-frame item flash is fixed. Camera ownership is separate.
    // Wait for the pose transaction to correlate the socket with ORIGINAL joints.
    // Never derive a grip from a destination already moved by VRIK (double delta).
    QuatT28 originalGripD71{};
    const bool currentOriginalD71=((!correlated&&!stableGrip55)||(nativeGrasp&&grasp.socketOnly28)||acceptedPendingD72)&&
        OriginalToolGripD71(body,socket,originalGripD71);
    const bool originalBootstrapD71=!correlated&&!stableGrip55&&currentOriginalD71;
    if(!correlated&&!stableGrip55&&!originalBootstrapD71)
        return refuse("WAIT_ORIGINAL_JOINT_TRANSACTION");
    // D40: native cleanup temporarily revokes publication and can leave the
    // newest committed packet without tools59c10. After the owner independently
    // requalifies the SAME real attachment, its captured canonical grip needs
    // no second ORIGINAL-joint bootstrap. Compose it on this current wrist;
    // never reuse a stale tool model or grant placement during cleanup.
    auto currentModelD40=originalBootstrapD71?AbsoluteFromRelative(body.allowed[side],originalGripD71):old.model;
    if(originalBootstrapD71&&!ValidPose6689(currentModelD40))return refuse("D71_ORIGINAL_CONTROLLER_MODEL_INVALID");
    if(stableGrip55){QuatT28 canonicalD40{};std::memcpy(&canonicalD40,&grasp.grip55,sizeof(canonicalD40));
        currentModelD40=AbsoluteFromRelative(body.allowed[side],canonicalD40);
        if(!ValidPose6689(currentModelD40))return refuse("CANONICAL_CONTROLLER_MODEL_INVALID");
    }
    const bool originalFingerPairD71=old.fingers55.sample==body.stamp.sample&&old.fingers55.tick==body.stamp.tick&&
        kcd2vr::grasp13::validFingerReference55(grasp,old.fingers55);
    if(nativeGrasp&&!finishingGraspR94&&!acceptedPendingD72&&!grasp.gripReady55&&g_graspSocket28&&(!originalBootstrapD71||originalFingerPairD71)){
        // Fixed native hand grip is captured once while the real pickup pose is
        // still alive. A later pocket/exit animation cannot drag this held item.
        auto grip55=RelativeFromAbsolute(body.allowed[side],currentModelD40);
        if(ValidPose6689(grip55)&&Len(FromT(grip55))<=.8f){
            kcd2vr::grasp13::SocketProof28 proof{};proof.binding=grasp;
            proof.self=reinterpret_cast<std::uintptr_t>(self);proof.attachment=reinterpret_cast<std::uintptr_t>(attachment);
            proof.sample=body.stamp.sample;proof.tick=body.stamp.tick;proof.joint=joint;
            std::memcpy(proof.relative,&socket.relative,sizeof(proof.relative));
            std::memcpy(&proof.grip55,&grip55,sizeof(grip55));proof.gripReady55=true;
            // Both immutable references came from this SAME original-joint
            // transaction, even if native evaluation reused its XR sample.
            if(old.fingers55.sample==proof.sample&&old.fingers55.tick==proof.tick&&
               kcd2vr::grasp13::validFingerReference55(grasp,old.fingers55))proof.fingers55=old.fingers55;
            g_graspSocket28(proof);
        }
    }
    // A same-callback current-original proof may own presentation only while
    // the controller still calls the native attachment socketOnly. It does NOT
    // promote HELD, inventory, physics, grip state or the pickup action.
    const bool pendingPresentationD71=nativeGrasp&&(grasp.socketOnly28||acceptedPendingD72)&&currentOriginalD71;
    if(nativeGrasp&&(grasp.socketOnly28||acceptedPendingD72)&&!pendingPresentationD71){
        // This exact pending item has no controller placement capability. Its
        // socket has now been correlated by BuildToolPoses against ORIGINAL
        // joints, and rechecked above against the current native relative pose.
        // The controller must revalidate the POD before promoting real HELD.
        return refuse("GRASP_SOCKET_CORRELATED_AWAIT_CONTROLLER");
    }
    ModelWorld6692 current{};float natural[8]{};
    if(!PhysAttachmentWorld164(attachment,body.stamp.actor,current)||!ReadAttachmentQs6689(attachment,natural))return refuse("ATTACHMENT_WORLD_UNAVAILABLE");
    auto target=PhysCurrentWorld164(PhysPose6698(currentModelD40),current);
    // D40: the first correlated native grip and the canonical grip use the
    // same relation. The old bootstrap-only clearance moved the item outward
    // by3cm and removed that displacement on capture, creating a visible step.
    // Keep native fingers and item together from the first verified placement.
    if(!ph::finite(target))return refuse("CONTROLLER_TARGET_NONFINITE");
    float wanted[8]{float(target.q.x),float(target.q.y),float(target.q.z),float(target.q.w),
        float(target.p.x),float(target.p.y),float(target.p.z),natural[7]};
    const bool presentationOnlyR94=pendingPresentationD71||finishingGraspR94;
    std::uint32_t finalHeld=0;
    kcd2vr::grasp13::Binding finalGrasp{};
    kcd2vr::alchemy_tool18::Binding finalAlchemy18{};kcd2vr::smith_tool36::Binding finalSmith36{};
    const bool stillOwned=finishingGraspR94?(CurrentFinishingVisualR94(finishingR94)&&FinishingNativeHandR94(finishingR94.binding)):
        nativeGrasp?(ReadGraspPresentationD72(finalGrasp)&&
        SameGraspPresentationR93R3(grasp,finalGrasp)&&((finalGrasp.attached&&finalGrasp.place)||
            (pendingPresentationD71&&!finalGrasp.attached&&(finalGrasp.socketOnly28||
                (acceptedPendingD72&&finalGrasp.acceptedAttachTickD72==grasp.acceptedAttachTickD72))))):
        nativeSmith36?(request==kcd2vr::workstation178r20c1::smithHoldEpoch36.load()&&
         ReadSmithTool36(attachment,bound,body.stamp.actor,finalSmith36)&&kcd2vr::smith_tool36::same(smith36,finalSmith36)):
        (request==kcd2vr::workstation178r20c1::experimentEpoch36.load()&&
         (nativeAlchemy18?(ReadAlchemyTool18(attachment,bound,body.stamp.actor,finalAlchemy18)&&kcd2vr::alchemy_tool18::same(alchemy18,finalAlchemy18)):
          (NativeHeldItemId6694(side,finalHeld)&&finalHeld==bound)));
    if((finishingGraspR94?!CurrentFinishingVisualR94(finishingR94):nativeSmith36?SmithAutoHoldReason36(now)!=nullptr:!ToolExperiment59c10(now))||
       ((nativeSmith36||nativeAlchemy18)&&socket.modeEpoch67!=kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire))||!stillOwned||
       !PhysEntityIdentity135(socket.entity,bound))return refuse("FINAL_OWNER_OR_IDENTITY_CHANGED");
    if(freeAlchemyPresentationD39){
        const auto finalNowD39=std::uint64_t(GetTickCount64());int finalSideR93=-1;
        if(!kcd2vr::minigame_hands45::freeTrackedMode(kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Alchemy))||
           !kcd2vr::workstation178r20c1::fullVrAlchemy63(finalNowD39)||
           !kcd2vr::subsystems::alchemy::bridge67::presentationOwnsCurrentR95(finalAlchemy18,body.stamp.epoch,[]() noexcept {return std::uint64_t(GetTickCount64());},nullptr,&finalSideR93)||finalSideR93!=side)
            return refuse("FINAL_ALCHEMY_PRESENTATION_OWNER_CHANGED");
    }
    if(ordinaryGrasp&&!finishingGraspR94&&!g_gameplay_allowed.load(std::memory_order_acquire)){
        const auto finalNow55=std::uint64_t(GetTickCount64());
        if(!acquisitionVisual55||!body.stamp.tick||finalNow55<body.stamp.tick||finalNow55-body.stamp.tick>80||
           !GraspAcquisitionVisual55(finalGrasp,finalNow55))return refuse("FINAL_PICKING_VISUAL_OWNER_CHANGED");
    }
    // C37R1 (ChatGPT): recheck after the native reads - entering gameplay or a new weapon classification never gives two writers.
    {bool weaponNow=false;for(auto& weapon:g_phys_relevant_id101)if(weapon.load(std::memory_order_relaxed)==bound)weaponNow=true;
     if(weaponNow&&(!nativeSmith36||g_gameplay_allowed.load(std::memory_order_acquire)))return refuse("WEAPON_OWNER_REGAINED_DURING_TOOL_READ");}
    if(kcd2vr::static_grab_r89::ownsEntityR94(bound))return refuse("RIGID_BODY_HOLD_REGAINED_DURING_TOOL_READ");
    if((originalBootstrapD71||pendingPresentationD71||finishingGraspR94)&&!OriginalToolSocketCurrentD71(body,socket))
        return refuse("D71_ORIGINAL_SOCKET_OR_PUBLICATION_CHANGED");
    bool child=false;
    if(!PlaceBothConsumers6689(self,socket.entity,wanted,child)){
        // The entity write may have succeeded before its child update failed.
        // Once this exact ordinary pickup owns the callback, a second native
        // writer must not put it back on the animation socket in the same frame.
        // No success receipt is issued; the next callback revalidates everything.
        (void)refuse("PLACEMENT_FAILED");
        return nativeGrasp;
    }
    if(nativeAlchemy18&&alchemy18.type==3&&side==0){
        kcd2vr::subsystems::alchemy::bridge67::PestlePresentationD47 p{};
        p.binding=finalAlchemy18;p.tick=body.stamp.tick;p.poseEpoch=body.stamp.epoch;
        p.sourceEpoch=body.doorTrackingEpoch66;p.modeEpoch=socket.modeEpoch67;p.parked=old.parkedD47;
        (void)kcd2vr::subsystems::alchemy::bridge67::pestlePresentationD47.publish(p);
    }
    TraceToolItem28(body,side,self,attachment,current,wanted,finishingGraspR94?nullptr:nativeGrasp?&finalGrasp:nullptr);
    if(nativeGrasp&&!presentationOnlyR94){
        // This exact ordinary item has actually moved to the controller-owned
        // wrist. Retire its earlier animation lease, not the native operation.
        // A newer accepted action (even at the same millisecond) stays current.
        auto retired=g_native_wrist_retired107[side].load(std::memory_order_acquire);
        while(retired<wristSerial107&&!g_native_wrist_retired107[side].compare_exchange_weak(
            retired,wristSerial107,std::memory_order_acq_rel,std::memory_order_acquire)){}
    }
    if(nativeSmith36&&side==0&&smith36.tool==int(kcd2vr::smith_tool36::Tool::Hammer)&&
       kcd2vr::combat_rules178r10::f5Rung.load(std::memory_order_relaxed)==1&&
       diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)&&
       kcd2vr::subsystems::smithing::contact68::exhaustedSession.load(std::memory_order_acquire)!=
         diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load(std::memory_order_acquire)){
        bool checked68=false;
        if(PlacementReadback6689(self,socket.entity,bound,wanted,checked68)){
            kcd2vr::subsystems::smithing::contact68::HammerFrame h{};
            h.binding=finalSmith36;h.tick=body.stamp.tick;h.sample=body.stamp.sample;
            h.poseEpoch=body.stamp.epoch;h.sourceEpoch=body.doorTrackingEpoch66;
            h.modeEpoch=socket.modeEpoch67;h.placed=true;
            std::memcpy(h.world,wanted,sizeof(h.world));(void)g_smith_placed68.publish(h);
        }
    }
    // Only an ACTUAL attached and controller-placed resource carafe can request
    // a child-animation hold. Native alchemy actions themselves are not IActions:
    // their +58 is the owner pointer. The separate verified child is at +78.
    if(nativeAlchemy18&&!kcd2vr::workstation178r20c1::fullVrAlchemy63(now)&&alchemy18.side==1&&alchemy18.type>=13&&alchemy18.type<=16&&
       alchemy18.actionClass==WhBase()+0x3A5C670){
        kcd2vr::alchemy_stage19c4::Proof proof{};std::uintptr_t owner=0;int status=-1;
        if(Read(reinterpret_cast<void*>(alchemy18.action+0x78),proof.child)&&proof.child&&
           PhysVft6698(reinterpret_cast<void*>(proof.child),0x3A47948)&&
           Read(reinterpret_cast<void*>(proof.child+0x268),owner)&&owner==alchemy18.instance&&
           Read(reinterpret_cast<void*>(proof.child+0x28),status)&&status==2&&
           Read(reinterpret_cast<void*>(proof.child+0x54),proof.token)){
            proof.binding=alchemy18;proof.request=request;proof.epoch=body.stamp.epoch;
            proof.sample=body.stamp.sample;proof.tick=body.stamp.tick;
            (void)kcd2vr::alchemy_stage19c4::attached.publish(proof);
        }
    }
    // Native stage still owns fingers and detachment. This short refresh is not
    // tracking grace: every write above independently requires fresh tracking.
    if(!presentationOnlyR94)g_native_use_until107[side].store(now+80,std::memory_order_release);
    if(nativeGrasp&&!presentationOnlyR94&&g_graspPlaced13){
        bool checked=false;
        if(PlacementReadback6689(self,socket.entity,bound,wanted,checked)){ // checked only denotes an optional animated child
            kcd2vr::grasp13::Placement p{};p.binding=finalGrasp;p.self=reinterpret_cast<std::uintptr_t>(self);
            p.attachment=reinterpret_cast<std::uintptr_t>(attachment);p.sample=body.stamp.sample;p.tick=body.stamp.tick;
            std::memcpy(&p.world,wanted,sizeof(p.world));g_graspPlaced13(p);
        }
    }
    if(ToolReceipt59c10(now,self,bound,side,finishingGraspR94?"R94_CLEANUP_DONE_PRESENTATION_ONLY":acceptedPendingD72?"D72_ACCEPTED_ATTACH_FIRST_CALLBACK_PRESENTATION_ONLY":pendingPresentationD71?"D71_PENDING_SOCKET_PRESENTATION_ONLY":
        originalBootstrapD71?"D71_CURRENT_ORIGINAL_FIRST_PLACEMENT":"CONTROLLER_HOLD",body.stamp.sample,joint)){
        bool checked=false;const bool readback=PlacementReadback6689(self,socket.entity,bound,wanted,checked);
        char line[420]{};std::snprintf(line,sizeof(line),
            "C10 F5_TOOL_HOLD side=%d target=%u sample=%llu nativeHand=VERIFIED socket=%d pose=%s readback=%d readbackChecked=%d release=NATIVE_ATTACHMENT constraints=NONE alchemyEntity18=%d alchemyType18=%d smithTool36=%s smithPhase36=%d smithKey36=%016llX grip=%s",
            side,bound,(unsigned long long)body.stamp.sample,joint,old.parkedD47?"PARKED_NATIVE_PESTLE":nativeSmith36?"CONTROLLER_SMITH_AUTO_HOLD":"CONTROLLER",readback,checked,nativeAlchemy18,nativeAlchemy18?alchemy18.type:-1,
            nativeSmith36?kcd2vr::smith_tool36::name(smith36.tool):"NONE",nativeSmith36?smith36.phase:-1,(unsigned long long)smith36.key,
            nativeSmith36?"NOT_REQUIRED":nativeGrasp?"NATIVE_GRASP":"REQUIRED");diagnostic_trace::Write(line);
        if(nativeAlchemy18){
            // D53: the separate mode3 pose stage may sweep qualified native
            // local bounds. Placement alone does not prove a contact query or
            // solid mesh/tool-to-tool contact. Keep this receipt at F5 cadence.
            char contact[512]{};std::snprintf(contact,sizeof(contact),
                "C52 F5_ALCHEMY_CONTACT side=%d entity=%u type=%d instance=%p lifetime=%llu attachment=%p sample=%llu owner=CONTROLLER_ATTACHMENT contactSelectedD55=%d contactPath=OPTIONAL_MODE3_BOUNDS_SWEEP meshContact=UNPROVEN toolToTool=UNIMPLEMENTED release=NATIVE_ATTACHMENT",
                side,alchemy18.entity,alchemy18.type,reinterpret_cast<void*>(alchemy18.instance),(unsigned long long)alchemy18.lifetime,
                reinterpret_cast<void*>(alchemy18.attachment),(unsigned long long)body.stamp.sample,
                kcd2vr::alchemy_tool_contact_d55::enabled.load(std::memory_order_relaxed)?1:0);
            diagnostic_trace::Write(contact);
        }
    }
#ifdef KCD2VR_CRAFT36B
    if(nativeGrasp&&!finishingGraspR94)GrabSocketReceipt43(now,self,bound,side,joint,finalGrasp,socket,body,currentModelD40,target,wanted);
#endif
    return true;
}
static void BuildToolPoses59c10(CommittedFrame6695& packet,const QuatT28* original,std::size_t count,
                              const kcd2vr::grasp13::FingerReference55* fingers55=nullptr) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    const bool experiment59c10=ToolExperiment59c10(now);
    const bool smithGate36=SmithAutoHoldReason36(now)==nullptr; // C37
    if(!experiment59c10&&!smithGate36)return;
    const auto request=kcd2vr::workstation178r20c1::experimentEpoch36.load();
    kcd2vr::grasp13::Binding grasp{};const bool haveGrasp=g_graspBinding13&&g_graspBinding13(grasp)&&((grasp.attached&&grasp.place)||grasp.socketOnly28);
    for(int side=0;side<2;++side){ToolSocket59c10 socket{};
        if(!g_tool_socket59c10[side].read(socket))continue;
        // C37: a smithing socket is current only for the same exact live binding under the smith gate.
        kcd2vr::smith_tool36::Binding liveSmith36{};
        const bool smithCurrent36=bool(socket.smith36)&&smithGate36&&!socket.nativeGrasp13&&
            socket.request==kcd2vr::workstation178r20c1::smithHoldEpoch36.load()&&
            socket.modeEpoch67==kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire)&&
            ReadSmithTool36(socket.attachment,socket.id,packet.stamp.actor,liveSmith36)&&kcd2vr::smith_tool36::sameGripIdentity67(socket.smith36,liveSmith36);
        if(socket.smith36?!smithCurrent36:!experiment59c10)continue;
        // D72 retained: detached forge blank does not revoke the tracked tongs.
        if(smithCurrent36&&socket.gripReady67&&
           !kcd2vr::smith_tool36::sameCanonicalGripD56(socket.smith36,liveSmith36))socket.gripReady67=false;
        if(smithCurrent36&&kcd2vr::subsystems::smithing::bridge67::fixtureOwnedH21(liveSmith36)&&
           !kcd2vr::subsystems::smithing::bridge67::fixtureJoined(packet.smithFixture[side],liveSmith36,
               packet.stamp.epoch,socket.modeEpoch67,packet.stamp.sample,now))continue;
        // Guided and free lower-mode tools share packet.allowed. Keep the exact
        // canonical socket alive even at full guidance, so releasing guidance
        // never restarts from a stale native attachment or waits to reacquire.
        kcd2vr::alchemy_tool18::Binding liveAlchemy18{};
        const bool alchemyCurrent18=!socket.alchemy18||(
            socket.modeEpoch67==kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire)&&
            ReadAlchemyTool18(socket.attachment,socket.id,packet.stamp.actor,liveAlchemy18)&&
            kcd2vr::alchemy_tool18::same(socket.alchemy18,liveAlchemy18));
        int presentationSideR93=liveAlchemy18.side;
        const bool ownerCurrent=smithCurrent36||(socket.nativeGrasp13?(haveGrasp&&grasp.side==side&&grasp.entity==socket.id&&
            grasp.token==socket.request&&grasp.character==packet.stamp.actor&&grasp.epoch==packet.stamp.epoch):
            (socket.request==request&&alchemyCurrent18&&
             (kcd2vr::workstation178r20c1::fullVrAlchemy63(now)?kcd2vr::subsystems::alchemy::bridge67::presentationOwnsCurrentR95(liveAlchemy18,packet.stamp.epoch,[]() noexcept {return std::uint64_t(GetTickCount64());},nullptr,&presentationSideR93):
              (kcd2vr::combat_rules178r10::f5Rung.load()==2&&kcd2vr::workstation178r20c1::experimentArmed36.load()&&kcd2vr::workstation178r20c1::ownsWrist(now)))));
        if(!ownerCurrent||(socket.alchemy18&&presentationSideR93!=side)||!packet.valid[side]||socket.side!=side||
           socket.epoch!=packet.stamp.epoch||
           reinterpret_cast<std::uintptr_t>(socket.character)!=packet.stamp.actor||
           !socket.tick||now<socket.tick||now-socket.tick>80||socket.joint<0||std::size_t(socket.joint)>=count)continue;
        const auto tool=AbsoluteFromRelative(original[socket.joint],socket.relative);
        const int nativeSideR93=socket.alchemy18?socket.alchemy18.side:side;
        auto grip=RelativeFromAbsolute(packet.native[nativeSideR93],tool);
        const bool gesture67=(socket.alchemy18&&kcd2vr::workstation178r20c1::fullVrAlchemy63(now))||
            (socket.smith36&&kcd2vr::workstation178r20c1::ownsActivityWrist45(now));
        if(gesture67&&socket.gripReady67)grip=socket.grip67;
        if(socket.nativeGrasp13&&grasp.gripReady55){
            if(!kcd2vr::grasp13::valid(grasp.grip55))continue;
            std::memcpy(&grip,&grasp.grip55,sizeof(grip));
        }
        if(!ValidPose6689(tool)||!ValidPose6689(grip)||Len(FromT(grip))>.8f)continue;
        auto& out=packet.tools59c10[side];out.socket=socket;
        if(smithCurrent36)out.socket.smith36=liveSmith36; // current native phase; geometry remains the same joined tool
        out.fingers55={};
        if(socket.nativeGrasp13&&fingers55&&fingers55[side].sample==packet.stamp.sample&&
           fingers55[side].tick==packet.stamp.tick&&kcd2vr::grasp13::validFingerReference55(grasp,fingers55[side]))
            out.fingers55=fingers55[side];
        if(gesture67){
            out.socket.grip67=grip;
            // The direct workpiece attaches during sword_in while its native
            // slot is still rotating into the palm. Keep that ORIGINAL-joint
            // relationship provisional until the native heating/anvil stage.
            // Placement stays live; this qualifies only the persistent cache.
            // Once learned, the same exact tool keeps its grip through strokes.
            bool alchemyGripReadyD44=true;
            if(socket.alchemy18&&socket.alchemy18.type==3&&!socket.gripReady67){
                kcd2vr::alchemy_semi46::WorkWitness workD44{};
                alchemyGripReadyD44=kcd2vr::alchemy_semi46::work.read(workD44)&&
                    kcd2vr::alchemy_tool18::pestleGripReadyD44(socket.alchemy18,workD44,now);
            }
            if(socket.alchemy18&&socket.alchemy18.type==19&&!socket.gripReady67&&
               kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Alchemy)==kcd2vr::minigame_hands45::SixDofVrik){
                // D53: OpenBook attaches before the native reading grip settles.
                // Keep the real ORIGINAL-joint relation provisional throughout
                // entry; only this exact book's native read-enable generation
                // may freeze its persistent grip. No guessed palm clearance or
                // artificial page/read enable is introduced here.
                kcd2vr::activity_input163::AlchemyBook45 bookD53{};
                alchemyGripReadyD44=kcd2vr::activity_input163::AlchemyBookOpen45(now,bookD53)&&
                    bookD53.instance==socket.alchemy18.instance&&bookD53.lifetime==socket.alchemy18.lifetime&&
                    bookD53.entity==socket.alchemy18.entity;
            }
            out.socket.gripReady67=socket.gripReady67||(smithCurrent36?
                kcd2vr::smith_tool36::gripCanSettleD56(liveSmith36):alchemyGripReadyD44);
        }
        out.model=AbsoluteFromRelative(packet.allowed[side],grip);out.valid=ValidPose6689(out.model);out.parkedD47=false;
        // Keep the exact canonical socket while released, never a parked
        // world-space pestle. Mode3 retains tracked presentation until native
        // detach; mode2 keeps its native right-release presentation fallback.
        if(kcd2vr::diagnostics::hands28::active(now)){out.nativeObserved28=tool;out.nativeObservedKnown28=true;}
    }
}

// Only a successfully committed/restored paired pose may nominate a guided
// controller-to-native-grip gap. Current attachment membership is re-read here;
// an earlier selection, proxy or F10 placement cannot earn this witness.
static void PublishGuidedTools55(const CommittedFrame6695& packet,std::uint64_t sourceEpoch) noexcept {
    namespace guided=kcd2vr::guided_crafting55;
    const auto now=std::uint64_t(GetTickCount64());
    for(int side=0;side<2;++side){guided::Dock d{};
        if(guided::active(now)&&packet.valid[side]&&packet.propRequestedValid5916c1[side]&&
           packet.stamp.actor&&packet.stamp.epoch&&packet.stamp.sample&&sourceEpoch&&
           packet.stamp.tick&&now>=packet.stamp.tick&&now-packet.stamp.tick<=80){
            NativeActivityHandWitness46 w{};
            if(g_native_activity_hand46[side].read(w)&&w.epoch==packet.stamp.epoch&&w.tick&&now>=w.tick&&now-w.tick<=80){
                const bool alchemy=bool(w.alchemy);
                NativeActivityHandWitness46 again{};kcd2vr::smith_tool36::Binding currentSmith{};
                // A Dock describes native membership/proximity, not wrist ownership.
                if(NativeActivityHandOwned46(alchemy,packet.stamp.actor,packet.stamp.epoch,side,now,&currentSmith,false)&&
                   g_native_activity_hand46[side].read(again)&&again.epoch==w.epoch&&again.tick==w.tick&&
                   (alchemy?kcd2vr::alchemy_tool18::same(w.alchemy,again.alchemy):kcd2vr::smith_tool36::same(w.smith,again.smith))){
                    d.instance=alchemy?w.alchemy.instance:w.smith.instance;
                    d.lifetime=alchemy?w.alchemy.lifetime:w.smith.lifetime;
                    d.character=packet.stamp.actor;d.attachment=alchemy?w.alchemy.attachment:w.smith.attachment;
                    d.object=alchemy?w.alchemy.object:w.smith.object;d.entity=alchemy?w.alchemy.entity:w.smith.entity;
                    d.type=alchemy?w.alchemy.type:w.smith.tool;d.kind=alchemy?1:2;d.phase=alchemy?-1:currentSmith.phase;
                    d.side=side;d.tick=packet.stamp.tick;d.sequence=packet.stamp.sample;
                    d.poseEpoch=packet.stamp.epoch;d.sourceEpoch=sourceEpoch;
                    const auto request=FromT(packet.propRequested5916c1[side]),native=FromT(packet.native[side]);
                    const float x=request.x-native.x,y=request.y-native.y,z=request.z-native.z;
                    d.gap=std::sqrt(x*x+y*y+z*z)*packet.world[7];
                }
            }
        }
        (void)guided::dock[side].publish(d);
    }
}
