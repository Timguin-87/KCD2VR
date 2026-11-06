// Ordinary hand-gesture door setting, serviced on Henry's verified controller
// thread. Native accepted AnimDoor use is the authority; proximity never arms.
namespace door4 = kcd2vr::door19c4;
static kcd2vr::core::SnapshotMailbox<door4::Request> g_door_request19c4;
static std::atomic<std::uint64_t> g_door_serial19c4{0};
static door4::Owner g_door_owner19c4;
static std::atomic<bool> g_door_stop19c4{true},g_door_active19c4{false};
static std::atomic<std::uint64_t> g_door_live_serial66{0};
static int DriveDoor66(const door4::Request&,const kcd2vr::door_hold65::Hand&,float,bool,float&) noexcept;
static void RetireDoor66(const door4::Request&) noexcept;
static void DetachDoor66(const door4::Request&) noexcept;
// The script APIs are proven, but Henry's controller callback does not own the
// other character's animation jobs/FIFO. Never mistake a mod lock for that
// native scheduler boundary. Existing cleanup remains available below.
static constexpr bool kDoorJobOwnershipProven19c4=false;
static std::atomic<std::uint64_t> g_door_refused_serial19c4{0};
static bool DoorPins19c4() noexcept {
    const auto b=WhBase();if(!retail_build112::Ready(b))return false;
    static std::atomic<std::uintptr_t> checked{0};static std::atomic<bool> ready{false};
    if(checked.load(std::memory_order_acquire)==b)return ready.load(std::memory_order_relaxed);
    bool ok=true;
    for(const auto& p:kcd2vr::native_door_pins19c4::all){for(std::size_t i=0;i<p.size;++i){unsigned char v=0;
        if(!Read(reinterpret_cast<void*>(b+p.rva+i),v)||v!=p.bytes[i]){ok=false;break;}}if(!ok)break;}
    std::uintptr_t v=0;
    ok=ok&&Read(reinterpret_cast<void*>(b+kCSkeletonAnimVftRva+0xB8),v)&&v==b+0xAA8448&&
        Read(reinterpret_cast<void*>(b+kCSkeletonAnimVftRva+0xE8),v)&&v==b+0xAB1124&&
        Read(reinterpret_cast<void*>(b+kCCharInstanceVftRva+0x28),v)&&v==b+0x1A7FB10;
    ready.store(ok,std::memory_order_relaxed);checked.store(b,std::memory_order_release);return ok;
}
// Host fixtures substitute native leaf calls only; pins and ownership validation
// still execute from the production IO below.
#ifdef KCD2VR_DOOR_HOST_TEST
std::uintptr_t HostDoorNative19c4(std::uintptr_t rva) noexcept;
#endif
template<class Fn> static Fn DoorNative19c4(std::uintptr_t rva) noexcept {
#ifdef KCD2VR_DOOR_HOST_TEST
    return reinterpret_cast<Fn>(HostDoorNative19c4(rva));
#else
    return reinterpret_cast<Fn>(WhBase()+rva);
#endif
}
static void DoorArmReceipt35(std::uint32_t target,int side,const char* reason,const interaction_alignment::HandCandidate* candidate,
                             std::uint64_t now,bool stopped,bool armed,unsigned rung) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return;
    static std::atomic_flag busy35=ATOMIC_FLAG_INIT;
    if(busy35.test_and_set(std::memory_order_acquire))return;
    struct Leave35 {std::atomic_flag& flag;~Leave35(){flag.clear(std::memory_order_release);}} leave35{busy35};
    static std::uint64_t session35=0;static unsigned count35=0;
    const auto current35=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
    if(current35!=session35){session35=current35;count35=0;}if(count35>=32)return;++count35;
    const auto age35=candidate&&candidate->tick&&now>=candidate->tick?now-candidate->tick:~std::uint64_t(0);
    char line35[512]{};std::snprintf(line35,sizeof(line35),
        "C35 F5_DOOR_ADMISSION target=%u hand=%d reason=%s candidateObserved=%d candidateId=%u class=%s candidateOwner=%d object=%d ageMs=%llu distance=%.4f stopped=%d experimentArmed=%d rung=%u receipt=%u/32 nativeAction=ACCEPTED_ONLY controllerDoor=NOT_ADMITTED",
        target,side,reason,candidate?1:0,candidate?candidate->entity:0,candidate&&candidate->cls[0]?candidate->cls:"-",
        candidate?candidate->owner:-1,candidate?candidate->object:0,(unsigned long long)age35,candidate?candidate->distance:0.f,
        stopped,armed,rung,count35);diagnostic_trace::Write(line35);
}
struct DoorRequestWitness38 {
    interaction_alignment::HandCandidate candidate{};
    std::uint64_t epoch=0;
    std::uintptr_t player=0;
    kcd2vr::door_use46::Intent input46{};
    bool inputPresent46=false;
    kcd2vr::door_use46::AcceptedUse62 acceptedUse62{};
    kcd2vr::door_hold65::Hand hand65{};bool handSource65=false;
    // Diagnostic snapshots only, copied at the same native entry. No native
    // read/call and no permission/selection fallback is added by these fields.
    kcd2vr::door_use46::Input receiptInput63{};
    kcd2vr::door_use46::PhysicalUse62 receiptPhysical63{};
    interaction_alignment::HandCandidate receiptCandidate63{};
    bool receipt63=false,receiptHeld63=false,receiptPhysicalCurrent63=false;
    std::uint64_t diagnosticActionSeqH4=0; // copied call ID only; never grants ownership
};
// Called at the native hook ENTRY, before any synchronous nested work. Only a
// copied, still-live input for this exact target/hand can get comfortable reach.
static void CaptureDoorRequest46(DoorRequestWitness38& witness,std::uint32_t target,int side,std::uint64_t now) noexcept {
    // Capture the exact producing family BEFORE the native accepted action
    // executes. A new Mod frame/global experiment after that call cannot
    // relabel an old physical Use as a fresh source.
    kcd2vr::door_hold65::Frame entry65{};kcd2vr::door_use46::Input input65{};
    if(side>=0&&side<2&&kcd2vr::door_hold65::frame.read(entry65)&&
       kcd2vr::door_use46::currentInput62.read(input65)&&kcd2vr::door_use46::inputCurrent62(input65,now)){
        const auto& h=entry65.hand[side];
        if(kcd2vr::door_hold65::current(h,now)&&h.side==side&&h.player==witness.player&&h.poseEpoch==witness.epoch&&
           h.grip>=.30f&&h.press&&h.press==input65.press&&h.side==input65.side&&h.player==input65.player&&
           h.poseEpoch==input65.poseEpoch&&h.trackingEpoch==input65.trackingEpoch&&h.sourceGeneration==input65.sourceGeneration&&
           h.experimentEpoch==input65.experimentEpoch64&&h.experimentEpoch==kcd2vr::workstation178r20c1::experimentEpoch36.load()&&
           h.settingsEpoch==kcd2vr::door_use46::settingsEpoch.load()&&interaction_alignment::DoorAcquisitionCurrent65(h.sourceGeneration)){
            witness.hand65=h;witness.handSource65=true;
        }
    }
    if(kcd2vr::diagnostics::door_receipt63::active(now)){
        witness.receipt63=true;
        (void)kcd2vr::door_use46::currentInput62.read(witness.receiptInput63);
        (void)kcd2vr::door_use46::physicalUse62.read(witness.receiptPhysical63);
        (void)interaction_alignment::LastHandCandidate(witness.receiptCandidate63);
        witness.receiptHeld63=kcd2vr::door_use46::inputCurrent62(witness.receiptInput63,now);
        witness.receiptPhysicalCurrent63=kcd2vr::door_use46::pulseCurrent62(witness.receiptPhysical63,witness.receiptInput63,now);
    }
    kcd2vr::door_use46::Intent i{};
    if(kcd2vr::door_use46::read(i,now)&&i.side==side&&i.player==witness.player&&i.poseEpoch==witness.epoch){
        witness.inputPresent46=true;
        if(i.target==target&&kcd2vr::door_use46::fresh(i.selectedTick,now,250)&&
           interaction_alignment::DoorAcquisitionCurrent65(i.sourceGeneration))witness.input46=i;
    }
    if(!witness.input46.token){
        const auto& c=witness.candidate;
        kcd2vr::door_use46::Selection exact{c.object&&!std::strcmp(c.cls,"AnimDoor"),c.entity,c.owner,c.tick,c.sourceTick,
            c.sourceEpoch,c.sourceGeneration,c.sourcePlayer,{c.x,c.y,c.z},c.distance};
        kcd2vr::door_hold65::Frame hands65{};kcd2vr::door_surface65::Witness surface65{};
        if(!kcd2vr::door_hold65::frame.read(hands65)||side<0||side>1||
           !kcd2vr::door_surface65::read(target,hands65.hand[side],now,surface65))return;
        exact.point=surface65.point;exact.distance=surface65.distance;
        if(c.entity==target&&c.owner==side&&c.sourcePlayer==witness.player&&c.sourceEpoch==witness.epoch&&
           (interaction_alignment::DoorAcquisitionCurrent65(c.sourceGeneration)||interaction_alignment::DoorSourceCurrent38(c.sourceGeneration))&&
           interaction_alignment::DoorAcquisitionCurrent65(hands65.hand[side].sourceGeneration)){
            // The retained native projection is eligibility only. Normalize its
            // input identity from the independently proven current SAME door
            // surface; a withdrawn generic ray is not the Door grasp source.
            exact.sourceGeneration=surface65.source.sourceGeneration;
            exact.poseEpoch=surface65.source.poseEpoch;exact.poseTick=surface65.source.poseTick;exact.player=surface65.source.player;
            (void)kcd2vr::door_use46::captureAccepted62(exact,now,witness.acceptedUse62);
        }
    }
}
static bool ReadDoorInput46(const door4::Request& r,kcd2vr::door_use46::Intent& out,std::uint64_t now) noexcept {
    kcd2vr::door_use46::Intent expected{};expected.token=r.inputToken46;expected.target=r.id;expected.side=r.side;
    expected.player=r.player;expected.poseEpoch=r.epoch;expected.sourceGeneration=r.sourceGeneration;expected.trackingEpoch=r.trackingEpoch46;
    expected.settingsEpoch=r.inputSettingsEpoch46;
    return r.inputToken46&&kcd2vr::door_use46::current(expected,now,&out);
}
static void ArmDoor19c4(std::uint32_t target,std::uintptr_t action,const char* name,int side,std::uint64_t now,
                       const DoorRequestWitness38* requestWitness38=nullptr) noexcept {
    const bool stopped35=g_door_stop19c4.load(std::memory_order_acquire);
    const bool armed35=kcd2vr::workstation178r20c1::experimentArmed36.load();
    const unsigned rung35=kcd2vr::combat_rules178r10::f5Rung.load();
    const bool gate35=kcd2vr::door_use46::enabled.load(std::memory_order_acquire)&&!stopped35&&side>=0&&side<2&&target&&action;
    // OFF retains its cheap native no-op. Capture may describe an accepted door
    // which never had a fresh hand context, without granting it one.
    if(!gate35&&!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return;
    char actionName[16]{};bool ordinary=false;
    if(name){for(unsigned i=0;i<sizeof(actionName)-1;++i){if(!Read(name+i,actionName[i]))break;if(!actionName[i]){
        for(const char* known:{"door_l_b_o","door_l_b_c","door_l_f_o","door_l_f_c","door_r_b_o","door_r_b_c","door_r_f_o","door_r_f_c"})
            if(!std::strcmp(actionName,known)){ordinary=true;break;}break;}}}
    if(!ordinary)return;
    if(requestWitness38&&requestWitness38->receipt63){
        namespace dr63=kcd2vr::diagnostics::door_receipt63;
        static thread_local dr63::Gate receiptGate63;
        if(dr63::allow(receiptGate63,now)){
            const auto& w=*requestWitness38;const auto& i=w.receiptInput63;const auto& p=w.receiptPhysical63;
            const auto& c=w.receiptCandidate63;
            const bool exact=w.candidate.entity==target&&w.candidate.owner==side;
            const bool held=w.receiptHeld63&&i.side==side&&i.player==w.player&&i.poseEpoch==w.epoch;
            char line63[640]{};std::snprintf(line63,sizeof(line63),
                "C63 F5_DOOR_JOIN target=%u hand=%d reason=%s sourceHeld=%d sourceHand=%d sourcePress=%llu physicalUse=%llu physicalCurrent=%d palmCandidate=%u palmHand=%d exactDoorWitness=%d explicitToken=%llu nativeAction=ACCEPTED diagnosticOnly=1",
                target,side,dr63::join(held,c.entity&&c.entity!=target,exact,p.serial!=0,w.receiptPhysicalCurrent63),
                i.held?1:0,i.side,(unsigned long long)i.press,(unsigned long long)p.serial,w.receiptPhysicalCurrent63?1:0,
                c.entity,c.owner,exact?1:0,(unsigned long long)w.input46.token);
            diagnostic_trace::Write(line63);
        }
    }
    auto refuse35=[&](const char* why,const interaction_alignment::HandCandidate* c=nullptr){DoorArmReceipt35(target,side,why,c,now,stopped35,armed35,rung35);
        {namespace h4=diagnostics::interaction_cycle_h4;h4::Event e{};e.stage=h4::Stage::DoorAdmission;e.tick=now;e.target=target;e.side=side;e.action=action;e.code=0;
         if(requestWitness38){const auto& w=*requestWitness38;e.actionSeq=w.diagnosticActionSeqH4;e.player=w.player;e.poseEpoch=w.epoch;e.sourceTick=w.receiptInput63.poseTick;e.press=w.receiptInput63.press;e.physicalUse=w.receiptPhysical63.serial;e.token=w.input46.token;e.generation=w.receiptInput63.sourceGeneration;e.trackingEpoch=w.receiptInput63.trackingEpoch;}
         h4::copy(e.reason,sizeof(e.reason),why);h4::copy(e.label,sizeof(e.label),actionName);h4::observe(e,now);}
        if(std::strcmp(why,"DOOR_SETTING_OR_REQUEST_GATE")){char visible[140]{};std::snprintf(visible,sizeof(visible),"Door blocked: %s",why);frik_pose_probe::PublishVisibleFailure(visible);}}; // R9: the legacy route is off by design (CL-D), the hand door says its own why. DS-F12: Miki's one visible line
    if(!gate35){refuse35(side<0||side>1?"NO_FRESH_HAND_CONTEXT":"DOOR_SETTING_OR_REQUEST_GATE");return;}
    if(!DoorPins19c4()){refuse35("NATIVE_PINS_REFUSED");return;}
    interaction_alignment::HandCandidate candidate{};
    const bool recovered62=requestWitness38&&requestWitness38->acceptedUse62.valid&&!requestWitness38->input46.token;
    const bool semi46=requestWitness38&&(requestWitness38->input46.token||recovered62);
    // Every production hook call carries this entry witness. Manual/native
    // Use can complete normally, but cannot borrow clock ownership from a
    // canceled squeeze merely because a nearby candidate remains fresh.
    if(requestWitness38&&!semi46){
        refuse35(requestWitness38->inputPresent46?"SEMI_INPUT_TARGET_CHANGED_OR_EXPIRED":"NO_EXPLICIT_SEMI_INPUT");return;}
    kcd2vr::door_use46::Intent latest46{};
    if(semi46){
        kcd2vr::door_use46::Intent source62=requestWitness38->input46;
        if(recovered62&&!kcd2vr::door_use46::previewAccepted62(requestWitness38->acceptedUse62,now,source62)){
            refuse35("PHYSICAL_USE_SOURCE_REVOKED_BEFORE_ACCEPTANCE");return;}
        const auto& i=source62;
        if(i.target!=target||i.side!=side||!kcd2vr::door_use46::fresh(i.selectedTick,now,250)||
           (!recovered62&&!kcd2vr::door_use46::current(i,now,&latest46))||
           !interaction_alignment::DoorAcquisitionCurrent65(i.sourceGeneration)){
            refuse35("SEMI_INPUT_REVOKED_OR_EXPIRED");return;}
        if(recovered62)latest46=i;
        // The point remains a native selection point, never a claimed handle.
        // Its original timestamp is retained; the newer input pose is separate.
        candidate.tick=i.selectedTick;candidate.entity=i.target;candidate.owner=i.side;candidate.object=true;
        candidate.x=i.point.x;candidate.y=i.point.y;candidate.z=i.point.z;candidate.distance=i.distance;
        candidate.sourceGeneration=i.sourceGeneration;candidate.sourceEpoch=i.poseEpoch;candidate.sourcePlayer=i.player;
        candidate.sourceTick=i.poseTick;std::strcpy(candidate.cls,"AnimDoor");
    }else{
        // Legacy manual Use still needs a fresh local geometry witness.
        const bool haveWitness38=requestWitness38?(candidate=requestWitness38->candidate,candidate.tick!=0):
            interaction_alignment::ExactDoorCandidate38(target,side,now,candidate);
        if(!haveWitness38){
            interaction_alignment::HandCandidate latest{};
            const bool observed=interaction_alignment::LastHandCandidate(latest);
            refuse35("NO_EXACT_DOOR_HAND_WITNESS",observed?&latest:nullptr);return;}
        if(!candidate.tick||now<candidate.tick||now-candidate.tick>80){refuse35("CANDIDATE_STALE",&candidate);return;}
    }
    if(candidate.entity!=target||!candidate.object||candidate.owner!=side||std::strcmp(candidate.cls,"AnimDoor")){
        refuse35("CANDIDATE_TARGET_CLASS_OR_OWNER",&candidate);return;}
    // A native head/view offer or old projection centre cannot borrow a hand
    // lease. Require the independently queried SAME door surface within20cm.
    kcd2vr::door_hold65::Frame hands65{};kcd2vr::door_surface65::Witness surface65{};
    if(!kcd2vr::door_hold65::frame.read(hands65)||
       !kcd2vr::door_surface65::read(target,hands65.hand[side],now,surface65)){
        refuse35("NO_CURRENT_SAME_DOOR_20CM_SURFACE",&candidate);return;}
    const auto& hand65=hands65.hand[side];
    if(!requestWitness38||!requestWitness38->handSource65||
       !kcd2vr::door_hold65::current(requestWitness38->hand65,now)||
       !kcd2vr::door_hold65::sameSource(requestWitness38->hand65,hand65)||
       !hand65.press||hand65.press!=requestWitness38->hand65.press||
       hand65.experimentEpoch!=kcd2vr::workstation178r20c1::experimentEpoch36.load()||
       hand65.settingsEpoch!=kcd2vr::door_use46::settingsEpoch.load()||
       hand65.trackingEpoch!=kcd2vr::tracking59c6::epoch.load()){
        refuse35("ACCEPTED_ENTRY_SOURCE_RETIRED",&candidate);return;}
    candidate.x=surface65.point.x;candidate.y=surface65.point.y;candidate.z=surface65.point.z;
    candidate.distance=surface65.distance;
    if(!interaction_alignment::DoorAcquisitionCurrent65(candidate.sourceGeneration)){refuse35("DOOR_SOURCE_REVOKED",&candidate);return;}
    if(!candidate.sourceTick||now<candidate.sourceTick||now-candidate.sourceTick>80){refuse35("SOURCE_POSE_STALE",&candidate);return;}
    const float admissionReach46=.20f; // C65: grasp, not a two-metre reach-assisted Use
    if(!std::isfinite(candidate.distance)||candidate.distance<0||candidate.distance>admissionReach46){refuse35("HAND_TOO_FAR_OR_UNKNOWN",&candidate);return;}
    door4::Request r{};r.tick=now;r.id=target;r.side=side;r.action=action;r.sourceGeneration=candidate.sourceGeneration;
    r.epoch=requestWitness38?requestWitness38->epoch:g_pose_epoch6689.load();
    r.player=requestWitness38?requestWitness38->player:reinterpret_cast<std::uintptr_t>(g_last_player_entity.load());
    if(!candidate.sourceEpoch||!candidate.sourcePlayer||candidate.sourceEpoch!=r.epoch||candidate.sourcePlayer!=r.player){
        refuse35("SOURCE_POSE_EPOCH_OR_PLAYER_CHANGED",&candidate);return;}
    if(r.epoch!=g_pose_epoch6689.load()||r.player!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())){
        refuse35("REQUEST_PLAYER_OR_EPOCH_CHANGED",&candidate);return;}
    r.entity=reinterpret_cast<std::uintptr_t>(EntityById6694(target));
    r.surface66={surface65.point.x,surface65.point.y,surface65.point.z};
    r.nativeSide66=(!std::strcmp(actionName,"door_l_f_o")||!std::strcmp(actionName,"door_l_f_c")||
                    !std::strcmp(actionName,"door_r_b_c"))?1:0;
    // C65 reach is measured from the committed anatomical PALM. The legacy
    // hand-world getter returns the wrist and would reject a valid20cm grasp
    // merely because the wrist is several centimetres farther from the door.
    r.start=hand65.point;
    if(!r.player||!r.entity){refuse35("CURRENT_ENTITY_OR_HAND_UNAVAILABLE",&candidate);return;}
    const float dx38=r.start.x-candidate.x,dy38=r.start.y-candidate.y,dz38=r.start.z-candidate.z;
    const float distanceSq38=dx38*dx38+dy38*dy38+dz38*dz38;
    if(!std::isfinite(distanceSq38)||distanceSq38>admissionReach46*admissionReach46){refuse35("CURRENT_HAND_LEFT_DOOR_POINT",&candidate);return;}
    if(r.epoch!=g_pose_epoch6689.load()||r.player!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())){
        refuse35("REQUEST_PLAYER_OR_EPOCH_CHANGED",&candidate);return;}
    void* anim=nullptr;void* character=CharacterForEntity6689(reinterpret_cast<void*>(r.entity));
    if(!ValidateCharInstance(character,anim)){refuse35("DOOR_CHARACTER_UNQUALIFIED",&candidate);return;}
    if(!interaction_alignment::DoorAcquisitionCurrent65(r.sourceGeneration)){refuse35("DOOR_SOURCE_REVOKED",&candidate);return;}
    if(semi46){
        kcd2vr::door_use46::Intent source=requestWitness38->input46;
        const float liveGrip62=side?g_left_grip.load():g_right_grip.load();
        if(recovered62&&(!g_gameplay_allowed.load()||g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||
           animation_filter::nativeBodyOwned112.load()||!TrackingHeadUsable111(now)||!TrackingHandUsable111(side,now)||
           ExactHandItemState(side)!=0||!std::isfinite(liveGrip62)||liveGrip62<.30f)){
            refuse35("PHYSICAL_USE_CONTEXT_OR_HELD_HAND_REVOKED",&candidate);return;}
        if(recovered62&&!kcd2vr::door_use46::publishAccepted62(requestWitness38->acceptedUse62,now,source)){
            refuse35("PHYSICAL_USE_ACCEPTED_HANDOFF_REVOKED_OR_ALREADY_CLAIMED",&candidate);return;}
        if(recovered62)latest46=source;
        if(!kcd2vr::door_use46::consume(source,now)){
            refuse35("SEMI_INPUT_ALREADY_CONSUMED_OR_REVOKED",&candidate);return;}
        r.inputToken46=latest46.token;r.trackingEpoch46=latest46.trackingEpoch;r.inputSettingsEpoch46=latest46.settingsEpoch;r.start=latest46.motion;
        r.inputExperimentEpoch65=requestWitness38->hand65.experimentEpoch;
        r.inputPress65=latest46.press;
    }
    r.mode69=int(kcd2vr::combat_rules178r10::f5Rung.load(std::memory_order_acquire));
    if(!kcd2vr::door_use46::inputMode(unsigned(r.mode69))){refuse35("DOOR_MODE_CHANGED",&candidate);return;}
    r.serial=g_door_serial19c4.fetch_add(1)+1;
    if(g_door_request19c4.publish(r)){
        {namespace h4=diagnostics::interaction_cycle_h4;auto e=h4::doorEvent(r,h4::Stage::DoorAdmission,now,"ACCEPTED_REQUEST_MAILBOX_PUBLISHED_NOT_COMPLETION",0,true);e.actionSeq=requestWitness38?requestWitness38->diagnosticActionSeqH4:0;h4::copy(e.label,sizeof(e.label),actionName);h4::observe(e,now);}
        g_door_live_serial66.store(r.serial,std::memory_order_release);
        kcd2vr::door_start19c4::arm(reinterpret_cast<std::uintptr_t>(anim),r.serial,now);
        if(recovered62&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){
            char m[260]{};std::snprintf(m,sizeof(m),"C62 F5_DOOR_HANDOFF serial=%llu target=%u hand=%d sourcePress=%llu physicalUse=%llu nativeAction=ACCEPTED workerHold=WAIT_LUA_PAUSE_ACK extraUse=NONE",
                (unsigned long long)r.serial,r.id,r.side,(unsigned long long)latest46.press,(unsigned long long)latest46.physicalUseSerial62);
            diagnostic_trace::Write(m);
        }
    }
    else refuse35("REQUEST_MAILBOX_BUSY",&candidate);
}
struct DoorIO19c4 {
    bool allowed(const door4::Request& r) noexcept {
        kcd2vr::door_use46::Intent input46{};
        const auto now=std::uint64_t(GetTickCount64());const float grip=r.side?g_left_grip.load():g_right_grip.load();
        return kcd2vr::door_use46::enabled.load(std::memory_order_acquire)&&(!r.inputToken46||ReadDoorInput46(r,input46,now))&&!g_door_stop19c4.load(std::memory_order_acquire)&&r.side>=0&&r.side<2&&g_gameplay_allowed.load()&&
            !diagnostics::lab::NativeReference()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&
            !animation_filter::nativeBodyOwned112.load()&&interaction_alignment::DoorSourceCurrent38(r.sourceGeneration)&&r.epoch==g_pose_epoch6689.load()&&
            r.player==reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())&&std::isfinite(grip)&&grip>=.25f&&
            TrackingHeadUsable111(now)&&TrackingHandUsable111(r.side,now)&&ExactHandItemState(r.side)==0;
    }
    bool hand(const door4::Request& r,door4::Point& p) noexcept {
        if(r.inputToken46){kcd2vr::door_use46::Intent i{};if(!ReadDoorInput46(r,i,GetTickCount64()))return false;p=i.motion;return true;}
        return ReadInteractionHandWorld123R1(r.side,p.x,p.y,p.z);
    }
    bool layer(const door4::Binding& b,float& rate) noexcept {
        if(!DoorPins19c4()||!b.entity||!PhysEntityIdentity135(reinterpret_cast<void*>(b.entity),b.id)||
           EntityById6694(b.id)!=reinterpret_cast<void*>(b.entity)||!PhysVft6698(reinterpret_cast<void*>(b.character),kCCharInstanceVftRva)||
           CharacterForEntity6689(reinterpret_cast<void*>(b.entity))!=reinterpret_cast<void*>(b.character)||
           b.anim!=b.character+0x110||!PhysVft6698(reinterpret_cast<void*>(b.anim),kCSkeletonAnimVftRva))return false;
        return Read(reinterpret_cast<void*>(b.anim+0x50),rate)&&std::isfinite(rate);
    }
    // 1 restored, 0 proven foreign/replaced, -1 still-owned or unreadable:
    // retain a cleanup-only lease on failure instead of stranding layer rate0.
    int restore(const door4::Binding& b) noexcept {
        if(!DoorPins19c4()||!b.entity||!b.character||!b.anim)return -1;
        if(EntityById6694(b.id)!=reinterpret_cast<void*>(b.entity)||b.anim!=b.character+0x110)return 0;
        std::uintptr_t vf=0,fn=0;std::uint32_t id=0;
        if(!Read(reinterpret_cast<void*>(b.entity),vf))return -1;
        if(vf!=WhBase()+kCEntityVftRva)return 0;
        if(!Read(reinterpret_cast<void*>(b.entity+0x0C),id))return -1;
        if(id!=b.id)return 0;
        if(!Read(reinterpret_cast<void*>(vf+0x328),fn))return -1;
        if(fn!=WhBase()+kCEntityGetCharacterRva)return 0;
        if(!PhysEntityIdentity135(reinterpret_cast<void*>(b.entity),b.id))return -1;
        if(CharacterForEntity6689(reinterpret_cast<void*>(b.entity))!=reinterpret_cast<void*>(b.character))return 0;
        if(!Read(reinterpret_cast<void*>(b.character),vf))return -1;
        if(vf!=WhBase()+kCCharInstanceVftRva)return 0;
        if(!Read(reinterpret_cast<void*>(b.anim),vf))return -1;
        if(vf!=WhBase()+kCSkeletonAnimVftRva)return 0;
        float rate=-1;if(!Read(reinterpret_cast<void*>(b.anim+0x50),rate)||!std::isfinite(rate))return -1;
        if(rate!=0.f)return 0;
        return speed(b,b.savedRate)?1:-1;
    }
    bool snapshot(const door4::Binding& b,float& t,float& rate) noexcept {
        if(!layer(b,rate))return false;
        std::uintptr_t entries=0;unsigned count=0;int animation=-1;
        if(!Read(reinterpret_cast<void*>(b.anim+0x48),entries)||entries!=b.entries||!entries||
           !Read(reinterpret_cast<void*>(entries-4),count)||count!=1||
           !Read(reinterpret_cast<void*>(entries+0x10),animation)||animation!=b.animation||
           !Read(reinterpret_cast<void*>(b.anim+0x50),rate)||!std::isfinite(rate))return false;
        using Time=float(__fastcall*)(void*,int,int);
        __try{t=DoorNative19c4<Time>(0xA98B30)(reinterpret_cast<void*>(b.anim),0,0);}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
        return std::isfinite(t)&&t>=0&&t<=1.01f;
    }
    bool acquire(const door4::Request& r,door4::Binding& b) noexcept {
        if(!kDoorJobOwnershipProven19c4){
            auto previous=g_door_refused_serial19c4.load(std::memory_order_relaxed);
            if(previous!=r.serial&&g_door_refused_serial19c4.compare_exchange_strong(previous,r.serial,std::memory_order_relaxed))
                receipt(r,"DOOR_ANIMATION_JOB_BOUNDARY_UNPROVEN",0.f,false);
            return false;
        }
        b.id=r.id;b.entity=r.entity;
        void* entity=EntityById6694(r.id);if(entity!=reinterpret_cast<void*>(r.entity)||!PhysVft6698(entity,kCEntityVftRva))return false;
        std::uintptr_t vf=0,fn=0;if(!Read(entity,vf)||!Read(reinterpret_cast<void*>(vf+0x328),fn)||fn!=WhBase()+kCEntityGetCharacterRva)return false;
        void* character=CharacterForEntity6689(entity);void* anim=nullptr;if(!ValidateCharInstance(character,anim))return false;
        b.character=reinterpret_cast<std::uintptr_t>(character);b.anim=reinterpret_cast<std::uintptr_t>(anim);
        if(!Read(reinterpret_cast<void*>(b.anim+0x48),b.entries)||!b.entries||!Read(reinterpret_cast<void*>(b.entries+0x10),b.animation))return false;
        return kcd2vr::door_start19c4::matches(b.anim,r.serial,GetTickCount64(),b.animation)&&snapshot(b,b.initialTime,b.savedRate);
    }
    bool current(const door4::Binding& b,float& t,float& rate) noexcept {return snapshot(b,t,rate);}
    bool speed(const door4::Binding& b,float value) noexcept {
        if(value==0.f){
            // Claim before the final stop check. An uninstall before this claim
            // prevents the pause; an uninstall after it must retain cleanup.
            g_door_active19c4.store(true,std::memory_order_release);
            if(g_door_stop19c4.load(std::memory_order_acquire))return false;
        }
        using Speed=void(__fastcall*)(void*,int,float);
        __try{DoorNative19c4<Speed>(0xAA8448)(reinterpret_cast<void*>(b.anim),0,value);}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
        float readback=-1;return Read(reinterpret_cast<void*>(b.anim+0x50),readback)&&readback==value;
    }
    bool time(const door4::Binding& b,float value) noexcept {
        using Time=void(__fastcall*)(void*,int,float);
        __try{DoorNative19c4<Time>(0xAB1124)(reinterpret_cast<void*>(b.anim),0,value);}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
        float readback=0,rate=0;return snapshot(b,readback,rate)&&std::fabs(readback-value)<.02f;
    }
    void receipt(const door4::Request& r,const char* reason,float t,bool restored) noexcept {
        if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64()))return;
        {namespace h4=diagnostics::interaction_cycle_h4;const auto now=std::uint64_t(GetTickCount64());h4::observe(h4::doorEvent(r,h4::Stage::DoorProgress,now,reason,t,restored),now);}
        char line[340]{};std::snprintf(line,sizeof(line),"C19C4 F5_DOOR serial=%llu target=%u hand=%d stage=%s time=%.3f nativeWriteReadback=%d release=NATIVE_FINISH hingeConstraint=NONE",
            (unsigned long long)r.serial,r.id,r.side,reason,double(t),restored);diagnostic_trace::Write(line);
    }
};

// R4 controller transport: animation writes execute solely in the target's
// native Lua Server.OnUpdate, never through DoorIO's refused cross-character
// setters above. The exact accepted native request remains the starting owner.
namespace doorScript4 = kcd2vr::door_script19c4r4;
static doorScript4::MotionOwner66 g_door_script_owner19c4r4;
static doorScript4::Owner g_door_legacy_owner69;
struct DoorScriptIO19c4r4 {
    const door4::Request* selected69=nullptr;
    bool control65(const door4::Request& r,kcd2vr::door_hold65::Hand& out) noexcept {
        out={};const auto now=std::uint64_t(GetTickCount64());
        kcd2vr::door_hold65::Frame frame{};
        if(r.side<0||r.side>1||!kcd2vr::door_hold65::frame.read(frame))return false;
        const auto h=frame.hand[r.side];
        if(r.mode69!=int(kcd2vr::combat_rules178r10::f5Rung.load(std::memory_order_acquire))||!kcd2vr::door_use46::inputMode(unsigned(r.mode69))||
           !kcd2vr::door_signal19c4r4::Ready()||!kcd2vr::door_use46::enabled.load(std::memory_order_acquire)||
           g_door_stop19c4.load(std::memory_order_acquire)||!g_gameplay_allowed.load()||diagnostics::lab::NativeReference()||
           g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||animation_filter::nativeBodyOwned112.load()||
           !kcd2vr::door_hold65::current(h,now)||h.side!=r.side||h.player!=r.player||h.poseEpoch!=r.epoch||
           h.trackingEpoch!=r.trackingEpoch46||h.settingsEpoch!=r.inputSettingsEpoch46||
           h.experimentEpoch!=r.inputExperimentEpoch65||r.inputExperimentEpoch65!=kcd2vr::workstation178r20c1::experimentEpoch36.load()||
           r.inputSettingsEpoch46!=kcd2vr::door_use46::settingsEpoch.load()||r.trackingEpoch46!=kcd2vr::tracking59c6::epoch.load()||
           !interaction_alignment::DoorAcquisitionCurrent65(r.sourceGeneration)||
           h.sourceGeneration!=r.sourceGeneration||r.epoch!=g_pose_epoch6689.load()||
           r.player!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())||
           !TrackingHeadUsable111(now)||!TrackingHandUsable111(r.side,now)||ExactHandItemState(r.side)!=0||
           (PhysicalHandReservations114()&(1u<<r.side))||
           EntityById6694(r.id)!=reinterpret_cast<void*>(r.entity)||!PhysEntityIdentity135(reinterpret_cast<void*>(r.entity),r.id))return false;
        out=h;return true;
    }
    bool allowed(const door4::Request& r) noexcept {
        kcd2vr::door_hold65::Hand h{};return r.id<=16777215u&&control65(r,h)&&
            (r.mode69!=3||(h.grip>=.30f&&h.press==r.inputPress65));
    }
    bool hand(const door4::Request& r,door4::Point& p) noexcept {DoorIO19c4 policy{};return policy.hand(r,p);}
    bool publish(const doorScript4::Command& c) noexcept {
        if(!selected69)return false;
        const auto& r=*selected69;
        // Recenter/tracking/UI may change during the hand snapshot. Revalidate
        // immediately before publishing authority; cleanup never requires it.
        if(c.phase!=0&&(c.request!=r.serial||c.target!=r.id||!allowed(r)))return false;
        auto scoped69=c;scoped69.mode69=kcd2vr::door_use46::motionMode(r.mode69);
        return kcd2vr::door_signal19c4r4::Publish(scoped69,r.entity);
    }
    bool ack(doorScript4::Ack& a) noexcept {const bool result=kcd2vr::door_signal19c4r4::ReadAck(a);
        if(result&&selected69){namespace h4=diagnostics::interaction_cycle_h4;const auto now=std::uint64_t(GetTickCount64());auto e=h4::doorEvent(*selected69,h4::Stage::DoorAck,now,"COPIED_NATIVE_LUA_ACK_NOT_AUTO_ADOPTED",a.fraction,false);e.sourceTick=a.tick;e.code=a.state;e.call=a.source;e.otherTarget=a.request;h4::observe(e,now);}return result;}
    int drive66(const door4::Request& r,const kcd2vr::door_hold65::Hand& h,float time,bool bind,float& out) noexcept {const int result=DriveDoor66(r,h,time,bind,out);
        {namespace h4=diagnostics::interaction_cycle_h4;const auto now=std::uint64_t(GetTickCount64());auto e=h4::doorEvent(r,h4::Stage::DoorProgress,now,result>0?"RESIDENT_CURVE_PROJECTED_REQUEST_NOT_PANEL_READBACK":result<0?"RESIDENT_CURVE_OR_CONTROLLER_PROOF_REFUSED":"RESIDENT_CURVE_BIND_PENDING",result>0?out:time,result>0);e.sourceTick=h.poseTick;e.otherPress=r.inputPress65;e.press=h.press;e.code=result;e.value2=time;h4::observe(e,now);}return result;}
    void retire66(const door4::Request& r) noexcept {RetireDoor66(r);}
    void detach66(const door4::Request& r) noexcept {DetachDoor66(r);}
    void receipt(const door4::Request& r,const char* reason,float time,bool held) noexcept {
        if(r.mode69==3&&!held)RetireDoor66(r);
        if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64()))return;
        {namespace h4=diagnostics::interaction_cycle_h4;const auto now=std::uint64_t(GetTickCount64());h4::observe(h4::doorEvent(r,h4::Stage::DoorProgress,now,reason,time,held),now);}
        char line[360]{};std::snprintf(line,sizeof(line),
            "G2 F5_DOOR_SCRIPT serial=%llu target=%u hand=%d stage=%s fraction=%.4f scriptHeldAck=%d setterOwner=ANIMDOOR_SERVER_ONUPDATE mode=%d completion=NATIVE",
            (unsigned long long)r.serial,r.id,r.side,reason,double(time),held?1:0,r.mode69);diagnostic_trace::Write(line);
    }
};
// Unlike EntityById6694, this adapter preserves UNKNOWN versus native null.
// +4B4F98 owns its registry lock and validates the full salted entity ID.
static bool DoorLookupEntry19c4r4() noexcept {
    const auto b=WhBase();if(!retail_build112::Ready(b))return false;
    const auto& expected=kcd2vr::native_door_pins19c4::entityLookup;
    for(std::size_t i=0;i<sizeof(expected);++i){unsigned char byte=0;
        if(!Read(reinterpret_cast<void*>(b+0x4B4F98ull+i),byte)||byte!=expected[i])return false;}
    return true;
}
static bool DoorRegistryLookup19c4r4(std::uint32_t id,std::uintptr_t& out) noexcept {
    out=0;if(!id||!DoorPins19c4()||!DoorLookupEntry19c4r4())return false;
    const auto b=WhBase();void* system=nullptr;std::uintptr_t vf=0,fn=0;
    if(!Read(reinterpret_cast<void*>(b+0x492D8A0ull),system)||!system||
       !Read(system,vf)||vf!=b+0x4077048ull||
       !Read(reinterpret_cast<void*>(vf+0x70),fn)||fn!=b+0x4B4F98ull)return false;
    void* entity=nullptr;using Lookup=void*(__fastcall*)(void*,std::uint32_t);
    __try{entity=DoorNative19c4<Lookup>(0x4B4F98)(system,id);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    void* again=nullptr;std::uintptr_t vfAgain=0,fnAgain=0;
    if(!Read(reinterpret_cast<void*>(b+0x492D8A0ull),again)||again!=system||
       !Read(system,vfAgain)||vfAgain!=vf||!Read(reinterpret_cast<void*>(vf+0x70),fnAgain)||fnAgain!=fn||!DoorLookupEntry19c4r4())return false;
    // Native lookup validated the full ID under its registry lock. Keep the
    // returned pointer opaque: do not dereference after that lock is released.
    out=reinterpret_cast<std::uintptr_t>(entity);return true;
}
static void TickDoorScript19c4r4() noexcept {
    // Cleanup survives local release, F5 OFF and replacement requests. Never
    // use Owner.request here: it can already name a different accepted door.
    doorScript4::CleanupIdentity cleanup{};std::uintptr_t current=0;
    if(kcd2vr::door_signal19c4r4::ReadCleanupIdentity(cleanup)&&
       DoorRegistryLookup19c4r4(cleanup.target,current)&&current!=cleanup.entity)
        (void)kcd2vr::door_signal19c4r4::RetireKnownAbsent(cleanup,current);
    door4::Request request{};(void)g_door_request19c4.read(request);
    // Both owners run cleanup, but exactly one can receive a new request.
    // Changing F5 revokes source identity; it never transfers a paused claim.
    const auto now69=std::uint64_t(GetTickCount64());
    DoorScriptIO19c4r4 spatial69{&g_door_script_owner19c4r4.request};
    DoorScriptIO19c4r4 legacy69{&g_door_legacy_owner69.request};
    const door4::Request none69{};
    g_door_script_owner19c4r4.sequence=std::max(g_door_script_owner19c4r4.sequence,g_door_legacy_owner69.sequence);
    g_door_script_owner19c4r4.tick(spatial69,kcd2vr::door_use46::spatialMode(unsigned(request.mode69))?request:none69,now69);
    g_door_legacy_owner69.sequence=std::max(g_door_script_owner19c4r4.sequence,g_door_legacy_owner69.sequence);
    g_door_legacy_owner69.tick(legacy69,request.mode69==3?request:none69,now69);
    // Native bridge separately retains any CLAIM until the matching Lua ACK0;
    // a published release is not itself proof that the native rate resumed.
    g_door_active19c4.store(g_door_script_owner19c4r4.active||g_door_script_owner19c4r4.pending||
        g_door_legacy_owner69.active||g_door_legacy_owner69.pending,std::memory_order_release);
}
#ifndef KCD2VR_DOOR_HOST_TEST
#include "door_curve_native66.inl"
#include "door_hand_native66.inl"
#endif
