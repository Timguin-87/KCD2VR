        // D39: the full sampled hand shape owns hand-vs-hand contact. The old
        // DS1 wrist-sphere pass ran again AFTER world sweeps, could undo their
        // checked endpoint, and ignored collision/native-use admission. Keep a
        // coarse fallback only when the sampled shapes are unavailable, here
        // BEFORE the existing world sweeps. D56 retains this current sampled
        // self geometry through occupancy UNKNOWN only while no other hand
        // presentation owner exists; item/world authority still needs EMPTY.
        tx.handPairD39={};
        bool pairFreeD39=!directHandsR135&&ph::handSeparationDs1.load(std::memory_order_relaxed)&&
            valid[0]&&valid[1]&&selfPresentationD56[0]&&selfPresentationD56[1]&&
            ts.head_valid&&!ts.head_held&&!ts.right_held&&!ts.left_held&&
            ts.source_epoch==kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire)&&
            !readingVisual32&&!tx.reloadWorkingHandXR34&&!tx.bowContext169r2&&
            !tx.acquisitionRequired57&&!s.pendingUse55.serial&&
            s.handoverW178r14[0]<=.001f&&s.handoverW178r14[1]<=.001f;
        for(int side=0;side<2&&pairFreeD39;++side)
            pairFreeD39=!tx.doorSerial66[side]&&!nativePassengerHand46[side]&&
                !(g_wristOwnedByController59c12&&g_wristOwnedByController59c12(side));
        if(pairFreeD39){
            namespace hp=kcd2vr::hand_pair_contact_d39;
            bool havePairShapeD39=geometry164[0]&&geometry164[1]&&
                hp::validShape(handShape59c7f[0])&&hp::validShape(handShape59c7f[1]);
            bool coarseFallbackD39=false;
            if(!havePairShapeD39&&!perfContactModeDs2&&
               !QualifiedCombatFistH34(ts,tx.epoch,0,contactNow164R1)&&
               !QualifiedCombatFistH34(ts,tx.epoch,1,contactNow164R1)){
                ModelWorld6692 pairWorldD39{};
                if(ReadModelWorld6692(character,pairWorldD39)&&std::isfinite(pairWorldD39.scale)&&pairWorldD39.scale>.1f){
                    // The legacy 9 cm wrist separation includes the kernel's
                    // 2 mm skin. Normal geometry never receives this second
                    // sphere constraint or a second correction.
                    const double radius=.045/double(pairWorldD39.scale)-.001;
                    for(auto& shape:handShape59c7f){shape={};shape.valid=true;shape.count=1;shape.part[0]={{},{},radius};}
                    havePairShapeD39=hp::validShape(handShape59c7f[0]);coarseFallbackD39=true;
                }
            }
            if(havePairShapeD39){
                hp::Input pairInputD39{};
                pairInputD39.key={reinterpret_cast<std::uintptr_t>(s.character),reinterpret_cast<std::uintptr_t>(s.owner),
                    tx.epoch,ts.source_epoch,tx.policyGeneration57,contactControl,tx.control51,tx.nativeHeldAdmissionT4,
                    unsigned(preciseContactModeR24)|(unsigned(perfContactModeDs2)<<1)|(unsigned(tx.fingerTrialH7[0])<<2)|
                    (unsigned(tx.fingerTrialH7[1])<<3)|(unsigned(coarseFallbackD39)<<4)};
                pairInputD39.sample=ts.sequence;pairInputD39.tick=ts.tick;pairInputD39.timeNs=ts.timeNs;
                pairInputD39.now=contactNow164R1;pairInputD39.basis=PQ(frame);pairInputD39.eligible=true;
                const auto stagedPairD39=hp::stage(s.handPairD39,pairInputD39,handShape59c7f[0],handShape59c7f[1],PV(goals[0]),PV(goals[1]));
                tx.handPairD39=stagedPairD39.state;
                const auto& contact=stagedPairD39.contact;
                if(contact.valid&&contact.contact)for(int side=0;side<2;++side){
                    goals[side]=Add(goals[side],VP(contact.correction[side]));
                    selfMask164[side]|=2;selfCorrection164[side]+=pc::length(contact.correction[side]);
                }
            }
        }
