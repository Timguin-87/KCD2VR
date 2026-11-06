// H40 runs BEFORE the old native Use/hold FSM. A known non-door keeps its
// existing route. A fresh otherwise eligible unknown Grip gets one bounded
// nearby-door query; it never depends on a prior successful native Use.
namespace doorH40=kcd2vr::hand_only_door_h40;
const auto handNowH40=std::uint64_t(GetTickCount64()); // actual consumption clock after blocking RunFrame
pi.now_ms=handNowH40;
doorH40::Admission oldUseAdmissionH40[2];
std::uint64_t fallbackPressH40[2]{};
doorH40::enabled.store(true,std::memory_order_release); // CL-D: sole hand-door owner, independent of F5.
for(int side=0;side<2;++side)if(xr_manager_){
    const auto& c=side?xr_manager_->LeftController():xr_manager_->RightController();
    const bool known= candidate59c7f.entity&&candidate59c7f.owner==side&&doorH40::Fresh(candidate59c7f.tick,handNowH40);
    const bool knownDoor=known&&std::strcmp(candidate59c7f.cls,"AnimDoor")==0;
    // R96: logs KCD2VR2.log (10 of 30 H40 rows) and KCD2VR.log (6 of 14) show
    // pressL=OTHER_TARGET: a native non-door offer under the LEFT hand refused
    // the squeeze before the nearby-door query was even made (right: 4 rows).
    // The other offer now keeps the Grip only when it is really the hand's
    // object: within the 25 cm hand-pickup reach AND no door candidate for this
    // same hand that is at least as close. Otherwise the door query runs; with
    // no door found the same Grip still returns to native Use (fallback below).
    interaction_alignment::HandCandidate sideDoorR96{};
    const bool sideDoorKnownR96=interaction_alignment::NativeDirectDoorCandidate54r1(side,handNowH40,sideDoorR96)&&
        sideDoorR96.entity&&std::isfinite(sideDoorR96.distance)&&sideDoorR96.distance<=.5f;
    const bool otherInReachR96=std::isfinite(candidate59c7f.distance)&&candidate59c7f.distance<=.25f;
    const bool knownOther=known&&candidate59c7f.cls[0]&&!knownDoor&&otherInReachR96&&
        !(sideDoorKnownR96&&sideDoorR96.distance<=candidate59c7f.distance+.05f);
    const bool eligible=pi.enabled&&!pi.menu&&pi.hand[side].tracked&&pi.hand[side].empty&&pi.hand[side].neutral&&
        c.active&&std::isfinite(c.grip)&&std::isfinite(c.trigger)&&c.trigger<.10f&&
        !diagnostics::lab::NativeReference()&&!alchemyPointer59c2;
    if(eligible&&knownDoor&&candidate59c7f.distance<=.5f)doorH40::ObserveNear(handNowH40);
    // Native non-door offers retain ordinary Use. Known doors and nearest-door
    // acquisition share the hand owner; locked refusal returns the same Grip
    // to native Use without claiming a successful opening.
    (void)doorH40::CandidatePress(side,knownDoor?candidate59c7f.entity:0,handNowH40,eligible&&!knownOther,c.grip,c.trigger,
        eligible&&knownOther?kcd2vr::door_press_why_r9::OtherTarget:kcd2vr::door_press_why_r9::HandBusy); // R9: why code
    // Native-use admission remains held through the actual producer/transport.
    // It excludes the script callback between its final check and original Use.
    if(doorH40::Pending(side)||!oldUseAdmissionH40[side].take(1u<<side))pi.hand[side].tracked=false;
    else if(eligible&&c.grip>=.30f){
        doorH40::Frame current{};doorH40::Candidate pending{};
        if(doorH40::completed.read(current)){
            if(doorH40::TakeFallback(side,current,handNowH40,pending))doorH40::nativeFallback[side]={pending,true,false};
            auto&handoff=doorH40::nativeFallback[side];
            if(handoff.active&&doorH40::Identity(handoff.original.original,current)&&doorH40::Current(current,side,handNowH40)&&
               doorH40::Fresh(handoff.original.tick,handNowH40,300)){
                fallbackPressH40[side]=handoff.original.tick;
                if(!handoff.armed){f5Physical.hand[side].down=false;f5Physical.hand[side].rearm=true;heldUse114.neutral[side]=true;handoff.armed=true;}
            }else handoff={};
        }
    }else doorH40::nativeFallback[side]={};
}
