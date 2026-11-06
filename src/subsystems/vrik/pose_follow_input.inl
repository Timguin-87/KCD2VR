    // V66.128 (a): with body follow ON the pelvis subtree (legs, hip helpers, spine) is
    // written. Their locals stay native, so only the moved pelvis/legs differ.
    // Mode 3 is a station-only presentation owner. The completed C51 frame
    // already joins Henry, native activity instance/lifetime, mode epoch and
    // fresh original tracking; a saved setting alone never moves the body.
    // Frame ownership survives the optional root-step fallback. Revalidate the same
    // station instance/context immediately before native writes and publication.
    tx.stationLeaseH41=full&&tx.frame51.valid&&tx.frame51.workstationKindH51&&
        int(kcd2vr::minigame_hands45::modeFor(tx.frame51.workstationKindH51))==kcd2vr::station_body_h41::kMode&&
        !ts.head_held&&!ts.right_held&&!ts.left_held&&s.legsReady128;
    const bool station6DofH41=tx.stationLeaseH41&&
        // R9C: a visit whose 6DOF station step keeps getting refused runs as the FREE body (hands + frame stay VRIK)
        !(kcd2vr::station_body_h41::fallbackEnabled.load(std::memory_order_relaxed)&&
          kcd2vr::station_body_h41::fallbackFor(kcd2vr::station_body_h41::fallback,tx.frame51.workstationInstanceH51,tx.frame51.workstationLifetimeH51));
    tx.stationBodyH41=s.stationBodyH41;
    if(!station6DofH41)tx.stationBodyH41={};
    const V3 stationHeadH41=Sub(headTarget,headAnchor123); // before eye-pivot arc compensation
    // The same existing body follow policy applies after mounted admission;
    // native mounting itself never reaches this tracked pose transaction.
    const bool follow128=full&&!tx.diceBodyD81&&!animation_filter::NativeKickActive175()&&
        (g_body_follow128.load(std::memory_order_relaxed)||station6DofH41)&&s.legsReady128;
    // C54: actual downward HMD motion needs pelvis/leg participation even
    // when optional horizontal roomscale follow is OFF. Otherwise the fixed
    // spine reaches its lower limit and the body/head separate on real crouch.
    // This is explicit tracked displacement, never inferred native crouch.
    const bool physicalCrouch54=tx.frame51.valid&&!follow128&&headTarget.z<headAnchor123.z-1e-6f;
    const bool frameFollow51=tx.frame51.valid&&(framePelvis54||physicalCrouch54||pc::length(tx.frame51.bodyOffset)>1e-6)&&s.legsReady128;
    // J27: Free/6DOF alchemy+smithing put the hips behind the head (station_ws_j27.h). same pelvis subtree as follow.
    const bool stationTorsoJ27=full&&tx.frame51.valid&&tx.frame51.workstationKindH51&&s.legsReady128&&
        kcd2vr::station_ws_j27::torsoEnabled.load(std::memory_order_relaxed);
    if(follow128||frameFollow51||stationTorsoJ27)for(std::uint32_t j=0;j<count;++j)if(s.pelvisTree128[j])tx.selected[j]=true;
    V3 follow128Offset{};float follow128Reach[2]{};
