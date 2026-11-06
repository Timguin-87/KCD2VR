    // A bow may constrain its grip/string through the physical kernel below.
    // Its incoming controller intent uses exactly the same unscaled tracked
    // endpoint as bare hands/weapons, never a separate shoulder-ratio retarget.
    V3 bowGoals169r2[2]{requestedGoalsRawH50[0],requestedGoalsRawH50[1]}; // H50: bow admission/geometry keeps actual raw controller targets
    if(manualPose169r2){goals[0]=bowGoals169r2[0];goals[1]=bowGoals169r2[1];}
    auto stagedArcher169r2=s.archer169r2;
    const bool stringLatched169r2=manualPose169r2&&ph::manual_bow_geometry146(bowProfile169r2)&&sameBow169r2&&priorBow169r2.bowHeld&&!priorBow169r2.carry146;
    double archerPull169r2=0,archerWeight169r2=0;
    if(stringLatched169r2){
        const ph::Pose candidate=ph::compose({PQ(DesiredHand6689(s,1,frame,ts.lq)),PV(bowGoals169r2[1])},ph::inverse(bowProfile169r2.grip[1]));
        const auto offset=pc::rotate(candidate.q,priorBow169r2.bowAcquireOffset121);
        const auto v=PV(bowGoals169r2[0])-ph::point(candidate,priorBow169r2.nock)-offset;
        archerPull169r2=pc::length(v);
        archerWeight169r2=pc::smooth_range103(.015,.16,archerPull169r2);
    }
    tx.bowContext169r2=manualPose169r2&&kcd2vr::diagnostics::bow169r2::recording(ts.tick);
    if(tx.bowContext169r2){auto& r=tx.bowEvidence169r2;r.tick=ts.tick;r.sample=ts.sequence;r.epoch=tx.epoch;r.request=preparedRequest169r2;
        r.key=bowProfile169r2.key;r.generation=bowProfile169r2.generation;r.actor=reinterpret_cast<std::uintptr_t>(character);
        r.fresh=!ts.head_held&&!ts.right_held&&!ts.left_held;
        r.target=PV(bowGoals169r2[0]);r.pull=archerPull169r2;r.latched=stringLatched169r2;r.carry=sameBow169r2&&priorBow169r2.carry146;}

