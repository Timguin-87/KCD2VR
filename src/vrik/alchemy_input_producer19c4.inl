// Actual workstation input producer. No engine reads on the Mod input thread.
std::array<alchemy_input19c4::Observation,2> nativeAlchemyInput19c4{};
interaction_alignment::HandCandidate alchemyCandidate19c4{};
const bool alchemyExperiment19c4=kind==Kind::Alchemy&&fullVrAlchemy63(wsNow)&&
    !kcd2vr::subsystems::alchemy::bridge67::enabled.load(std::memory_order_acquire); // G1 owns its own coherent Grip producer.
const auto alchemyLife19c4=witness.lifetime175.load(std::memory_order_acquire);
const int alchemyHand19c4=alchemyExperiment19c4?interaction_alignment::LockedPalmOwner19c4(wsNow):-1;
alchemy_input19c4::Observation selectedAlchemyInput19c4{};
const bool selectedAlchemyKnown19c4=alchemyExperiment19c4&&alchemy_input19c4::lookupSelected(instance,alchemyLife19c4,wsNow,selectedAlchemyInput19c4);
if(selectedAlchemyKnown19c4&&alchemyHand19c4>=0&&alchemyHand19c4<2&&workstationC1.held[unsigned(alchemyHand19c4)]&&
   hands[unsigned(alchemyHand19c4)].valid&&hands[unsigned(alchemyHand19c4)].tick&&wsNow>=hands[unsigned(alchemyHand19c4)].tick&&
   wsNow-hands[unsigned(alchemyHand19c4)].tick<=80){
    nativeAlchemyInput19c4[unsigned(alchemyHand19c4)]=selectedAlchemyInput19c4;
}
static alchemy_input19c4::Gesture alchemyGesture19c4;
const bool alchemyUse19c4=alchemyGesture19c4.update(alchemyExperiment19c4,instance,alchemyLife19c4,
    experimentEpoch36.load(),workstationC1.held,nativeAlchemyInput19c4,wsNow);
workstationC1.nativeUseOverride19c4=alchemyExperiment19c4;
workstationC1.nativeUse19c4=alchemyUse19c4;
if(alchemyExperiment19c4)result.use=alchemyUse19c4;
alchemy_stage19c4::Control stageControl19c4{};
stageControl19c4.instance=instance;stageControl19c4.lifetime=alchemyLife19c4;
stageControl19c4.request=experimentEpoch36.load();stageControl19c4.enabled=alchemyExperiment19c4;
for(unsigned side19c4=0;side19c4<2;++side19c4){stageControl19c4.held[side19c4]=workstationC1.held[side19c4];
    stageControl19c4.tick[side19c4]=hands[side19c4].valid?hands[side19c4].tick:0;}
(void)alchemy_stage19c4::control.publish(stageControl19c4);
if(alchemyExperiment19c4&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,wsNow)){
    (void)interaction_alignment::LastHandCandidate(alchemyCandidate19c4); // bounded recorder only, never final selection authority
    static diagnostics::interaction_receipt59c10::Gate inputReceipt19c4;
    const auto session19c4=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
    const int routes19c4=int(alchemyGesture19c4.route[0])|(int(alchemyGesture19c4.route[1])<<3)|(alchemyUse19c4?64:0);
    // C38 idle hover is not an input change. Keep the actual selected target in
    // each receipt, but do not spend the finite input budget on every unheld
    // candidate transition before the first real grab/pour attempt.
    const auto inputTarget38=alchemyHand19c4>=0?selectedAlchemyInput19c4.entity:0;
    if(inputReceipt19c4.observe(session19c4,wsNow,{instance,inputTarget38,int(alchemyLife19c4),routes19c4},100,256)){
        char line19c4[512]{};std::snprintf(line19c4,sizeof(line19c4),
            "C19C4 F5_ALCHEMY_INPUT instance=%llX lifetime=%llu target=%u side=%d offered=%u offeredSide=%d nativeSelected=%d knownR=%d knownL=%d routeR=%u routeL=%u use=%d routes=LEGACY0_HOLD1_RELEASE2_CANCELLED3 outcome=INPUT_ONLY nativeStage=UNCHANGED",
            (unsigned long long)instance,(unsigned long long)alchemyLife19c4,selectedAlchemyInput19c4.entity,alchemyHand19c4,alchemyCandidate19c4.entity,alchemyCandidate19c4.owner,selectedAlchemyKnown19c4,
            nativeAlchemyInput19c4[0].entity!=0,nativeAlchemyInput19c4[1].entity!=0,unsigned(alchemyGesture19c4.route[0]),unsigned(alchemyGesture19c4.route[1]),alchemyUse19c4);
        diagnostic_trace::Write(line19c4);
    }
}
