    {const auto ret54=KCD2VR_COMBO_CALLER();
     // Application ONLY. Preserve all candidate evaluation and foreign/paired
     // native owners. GeometryNativeAnswer validates exact ordinary action,
     // current row/type/phase, current target, item and release ancestry.
     if(g_base&&ret54==g_base+0x173BF58ull&&applyNative&&(attackType==0||attackType==1)&&
        Synchronized59c10r9()&&!kcd2vr::motion59c4::selected.load(std::memory_order_acquire)&&
        !geom27::InjectedContact40(rpg,request)){
        kcd2vr::answer59c4r2::Lineage lineage{};
        const auto actor=Rd<const void*>(rpg,8,nullptr),target=Rd<const void*>(request,0x10,nullptr);
        if(frik_pose_probe::GeometryNativeAnswer19c4r2(actor,target,attackType,lineage)){
            const auto& t=lineage.release;
            const kcd2vr::motion59c4::Key key{t.gesture59c3,t.key,t.generation,t.epoch,t.request,t.recovery59c3,t.player};
            kcd2vr::motion59c4::Attempt attempt{};
            const auto claim=kcd2vr::motion59c4::hybridAttempts54.application54(key,GetTickCount64(),attempt);
            if(claim==kcd2vr::motion59c4::Claim::Duplicate)return false;
            if(claim==kcd2vr::motion59c4::Claim::Ready)nativeApplication54.hold(kcd2vr::motion59c4::hybridAttempts54,key);
        }
     }}
