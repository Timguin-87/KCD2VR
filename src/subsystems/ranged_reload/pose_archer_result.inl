    if(tx.bowContext169r2){auto& r=tx.bowEvidence169r2;
        r.geometryReady=true;r.allowed=PV(goals[0]);r.final=PV(FromT(s.work[rig.hand_r]));r.shoulder=PV(FromT(s.work[rig.upper_r]));r.elbow=PV(FromT(s.work[rig.fore_r]));
        r.wristError=pc::length(r.final-r.allowed);r.reachError=std::max(0.0,pc::length(r.target-r.shoulder)-s.upper[0]-s.fore[0]);
        const double radius=std::clamp(double(Len(Sub(FromT(s.work[rig.upper_r]),FromT(s.work[rig.upper_l]))))*.32,.08,.16);
        r.armGap=kcd2vr::archer169r2::arm_proxy_gap(r.shoulder,r.elbow,r.final,PV(FromT(s.work[rig.pelvis])),PV(FromT(s.work[rig.spine3])),radius);
        r.owned=tx.physicalActive&&tx.physicalOutput.owned[0];r.carry=tx.physicalActive&&tx.physicalState.weapon[1].carry146;
        r.latched=tx.physicalActive&&tx.physicalOutput.bowHeld&&!r.carry;
    }
