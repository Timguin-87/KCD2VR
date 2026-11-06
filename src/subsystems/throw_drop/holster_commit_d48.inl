// Included after the physical command ring. The native consumer publishes only
// exact requests it discarded BEFORE native entry. The serialized pose owner
// applies those receipts; native code never writes pose/gesture state directly.
static bool PhysHolsterCaptureD48(std::uint64_t now) noexcept;
static void PhysHolsterEnqueuedD48(const ph::Input&,const PhysEquipment6698&,
    std::array<ph::HolsterState,8>&,const ph::Command&) noexcept;
static kcd2vr::core::SnapshotMailbox<ph::Command> g_holster_rejected_d48[2];

static bool PhysEquipmentIntentD48(const ph::Command& command) noexcept {
    return command.kind==ph::CommandKind::Draw||command.kind==ph::CommandKind::Holster||
        command.kind==ph::CommandKind::DropT1;
}
static bool PhysCurrentDrawIntentD48(const ph::Input& input,const ph::Command& command) noexcept {
    if(command.kind!=ph::CommandKind::Draw)return true;
    return command.side>=0&&command.side<2&&input.allowed&&input.tracked[command.side]&&
        input.intentTracked[command.side]&&std::isfinite(input.grip[command.side])&&input.grip[command.side]>=.25;
}
static void PhysRejectUnenteredEquipmentD48(const ph::Command& command) noexcept {
    if(!PhysEquipmentIntentD48(command)||command.side<0||command.side>1)return;
    const bool published=g_holster_rejected_d48[command.side].publish(command);
    const auto now=std::uint64_t(GetTickCount64());
    if(PhysHolsterCaptureD48(now)){
        char line[256]{};std::snprintf(line,sizeof(line),
            "D48 HOLSTER_NOT_ENTERED side=%d kind=%u key=%016llX generation=%llu sample=%llu tick=%llu receipt=%d nativeDebt=UNCHANGED",
            command.side,unsigned(command.kind),(unsigned long long)command.key,
            (unsigned long long)command.generation,(unsigned long long)command.sample,
            (unsigned long long)command.tick,published?1:0);diagnostic_trace::Write(line);
    }
}
static bool PhysRejectHolsterLocalD48(const ph::Command& command,const ph::Input& input,
    const PhysEquipment6698& equipment,std::array<ph::HolsterState,8>& holsters) noexcept {
    if(!PhysEquipmentIntentD48(command)||command.side<0||command.side>1||
       command.bodyEpoch!=input.bodyEpoch||command.epoch!=input.epoch||
       equipment.count<0||equipment.count>int(equipment.items.size()))return false;
    for(int index=0;index<equipment.count;++index){const auto& item=equipment.items[index];
        if(item.profile.key!=command.key||item.profile.generation!=command.generation)continue;
        const int hand=command.kind==ph::CommandKind::Draw?ph::holster_hand126(item.profile):command.side;
        if(hand<0||hand>1)return false;
        auto& state=holsters[index];
        const bool changed=equipment114::RejectUnenteredD48(state.comparison114,command.key,
            command.generation,command.epoch,command.tick,command.kind==ph::CommandKind::Draw,input.grip[hand]);
        if(changed&&!state.comparison114.latched)state.acquisitionCg1={};
        // Retained comparison mode has a separate request clock. It still gets
        // an exact refusal, without altering a different/newer pending request.
        if(state.pending&&state.key==command.key&&state.epoch==command.epoch&&state.last==command.tick&&
           state.expectedDrawn==(command.kind==ph::CommandKind::Draw)){
            state.pending=false;state.latched=false;state.rearm=!(input.grip[hand]<.25);
            state.wasGrip=input.grip[hand]>.55;return true;
        }
        return changed;
    }
    return false;
}
static void PhysApplyHolsterRejectionsD48(const ph::Input& input,const PhysEquipment6698& equipment,
    std::array<ph::HolsterState,8>& holsters) noexcept {
    for(int side=0;side<2;++side){ph::Command command{};
        if(g_holster_rejected_d48[side].read(command))
            (void)PhysRejectHolsterLocalD48(command,input,equipment,holsters);
    }
}
static void PhysTraceAcquisitionD48(const ph::Input& input,const ph::Output& output,
    const std::array<ph::Profile,2>& profiles) noexcept {
    if(!PhysHolsterCaptureD48(input.tick))return;
    namespace cap=kcd2vr::diagnostics::capture168;
    const auto domain=cap::active(cap::Domain::Interaction,input.tick)?cap::Domain::Interaction:cap::Domain::Bug;
    const auto session=(cap::gate.session[unsigned(domain)].load(std::memory_order_relaxed)<<1)|
        (domain==cap::Domain::Interaction?1u:0u);
    static std::uint64_t lastSession=0,due[2]{},keys[2]{};static unsigned lines=0;
    if(session!=lastSession){lastSession=session;lines=0;due[0]=due[1]=keys[0]=keys[1]=0;}
    for(int side=0;side<2&&lines<48;++side){const auto& profile=profiles[side];
        if(!profile.key||(!output.initialAcquireSweepD44[side]&&!output.acquisitionPresentationD48[side]))continue;
        if(keys[side]==profile.key&&input.tick<due[side])continue;
        keys[side]=profile.key;due[side]=input.tick+250;++lines;
        const auto displayed=(output.displayLatched[side]||output.displayRejoinR13[side])?
            output.displayHands[side]:output.hands[side];
        char line[512]{};std::snprintf(line,sizeof(line),
            "D48 HOLSTER_PRESENTATION side=%d key=%016llX generation=%llu sample=%llu initialSweep=%d displayOnly=%d controllerWristMm=%.4f checkedGripMm=%.4f known=%d limited=%d plane=%d fraction=%.6f contactAuthority=%d queries=%u reason=%u budget=%u/48 targetPoseD70=%d",
            side,(unsigned long long)profile.key,(unsigned long long)profile.generation,(unsigned long long)input.sample,
            output.initialAcquireSweepD44[side],output.acquisitionPresentationD48[side],
            pm::length(displayed.p-input.hands[side].p)*1000.,
            pm::length(ph::compose(output.weapons[side],profile.grip[side]).p-input.hands[side].p)*1000.,
            output.contactKnown[side],output.sweepDeferred115[side],output.contact119[side].plane,
            output.contact119[side].fraction,ph::attack_authority55c1(input,output,side),
            output.contact119[side].coverage169r2.completedRays,unsigned(output.contact119[side].coverage169r2.reason),lines,output.contact119[side].targetPlacementD70);
        diagnostic_trace::Write(line);
    }
}
