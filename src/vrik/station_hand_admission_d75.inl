        // D81 final FREE-station presentation after self/world/handover stages.
        // Keep the same20mm allowance continuously instead of switching between
        // a fully corrected wrist and raw tracking at its threshold. A saturated
        // visible wrist remains explicitly separate from measured clearance:
        // ContactCommit must not move the next sweep's origin inside the surface.
        if(stationContactD75&&freeContact164R1[side]&&!nativePassengerHand46[side]&&
           stagedHandoverD47[side]<=.001f){
            V3 visibleD81{};bool limitedD81=false;
            if(kcd2vr::station_hand_contact_d75::presentationD81(rawGoalsH50[side],goals[side],
                    stagedContact[side].stationWorldScaleD75,visibleD81,limitedD81)){
                goals[side]=visibleD81;stagedContact[side].stationPresentationLimitedD81=limitedD81;
            }else{
                goals[side]=rawGoalsH50[side];ResetHandGeometryD56(stagedContact[side]);
                s.allowedReady[side]=false;s.handBlocked[side]=s.armBlocked[side]=s.armSaturated[side]=false;
            }
        }
