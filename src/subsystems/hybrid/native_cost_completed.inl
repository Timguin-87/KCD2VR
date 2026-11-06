    if(costBound54){kcd2vr::motion59c4::Key credit54{};
        const auto& key54=costAction54.key;
        const bool owner54=kcd2vr::solver_rpg_h29::costOwned(costAction54,
            frik_pose_probe::GeometryIntentOwnerH30,frik_pose_probe::GeometrySolverCostOwnerValidH28);
        if(!result||!owner54)se54::ledger.nativeUncertain54(key54,GetTickCount64());
        const bool complete54=se54::actionRegistry54.dispatchFinish(costAction54,result&&owner54,GetTickCount64(),credit54)&&
            se54::ledger.nativeComplete54(credit54,GetTickCount64());
        if(Active()){char line54[300]{};std::snprintf(line54,sizeof(line54),
            "C54 NATIVE_ACTION_DISPATCH gesture=%llu action=0x%llX generation=%llu accepted=%d ownerCurrent=%d costComplete=%d ancestry=OWNED_RELEASE_EXACT_ACTION writes=NONE",
            (unsigned long long)costAction54.key.gesture,(unsigned long long)costAction54.action,
            (unsigned long long)costAction54.generation,int(result),int(owner54),int(complete54));diagnostic_trace::Write(line54);}}
