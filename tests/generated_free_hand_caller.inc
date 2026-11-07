// Auto-extracted current caller; host fixtures provide explicit service leaves.

// Free-hand caller source SHA-256: 43449c37df7422578467ca09d9607bbac32a98a382b0d14dd9443e7c3d4283ea
#define WORLD_ADAPTER_FREE_HAND_CALLER_H24 \
    if(collisionMode&&!perfContactModeDs2){ \
        HandContactPending6695 pendingContactH25[2]{}; \
        kcd2vr::passive169r1::Sample contactSourceH25[2]{}; \
        V3 contactPalmH25[2][7]{};FingerContactGeometry103 contactGeometryH25[2]{}; \
        const std::uint64_t completedContactH24[2]{ \
            stagedContact[0].lastCompleteTickH18<=ts.tick?stagedContact[0].lastCompleteTickH18:0, \
            stagedContact[1].lastCompleteTickH18<=ts.tick?stagedContact[1].lastCompleteTickH18:0}; \
        const int firstContactSideH24=completedContactH24[1]<completedContactH24[0]?1:0; \
        for(int orderH24=0;orderH24<2;++orderH24){ \
            const int side=(firstContactSideH24+orderH24)&1;if(!valid[side])continue; \
            if(!freeContact164R1[side]){ \
                if(contactYield164R1[side]==16u&&!otherOwnerH50[side])ResetHandGeometryD56(stagedContact[side]); \
                else ResetHandContact6695(stagedContact[side]); \
                s.allowedReady[side]=false; \
                s.handBlocked[side]=s.armBlocked[side]=s.armSaturated[side]=false; \
                continue; \
            } \
            const V3 requested=goals[side];const int hand=side?rig.hand_l:rig.hand_r; \
            auto& palm=contactPalmH25[side];auto& fingerGeometry=contactGeometryH25[side]; \
            const auto orientation=DesiredHand6689(s,side,frame,visQH50[side]); \
            const bool havePalm=ContactPalmGeometryD79(s,side,orientation, \
                side?ts.left_grip:ts.right_grip,side?ts.left_trigger:ts.right_trigger,heldFingerKey[side], \
                palm,fingerGeometry,preciseContactModeR24,balancedContactD79, \
                tx.fingerTrialH7[side]?tx.fingerOpenBiasH7[side]:0.f,tx.fingerTrialH7[side]); \
            fingerGeometry.springSourceH31={reinterpret_cast<std::uintptr_t>(character),tx.epoch,tx.epoch, \
                contactControl,ts.sequence,ts.tick,tx.fingerSpringH31}; \
            auto& handRay169r1=contactSourceH25[side];handRay169r1.source=kcd2vr::passive169r1::Source::HandWorld; \
            handRay169r1.side=side;handRay169r1.actor=reinterpret_cast<std::uintptr_t>(character);handRay169r1.key=0x48414E4400000000ull+side; \
            handRay169r1.epoch=tx.epoch;handRay169r1.request=g_contact_control_epoch6695.load();handRay169r1.sequence=ts.sequence;handRay169r1.tick=ts.tick; \
            kcd2vr::diagnostics::passive169r1::RayScope rayScope169r1(handRay169r1); \
            if(havePalm&&ContactHand6695(stagedContact[side],character,tx.epoch,FromT(s.original[hand]),requested,goals[side],palm,preciseContactModeR24?&fingerGeometry:nullptr,&pendingContactH25[side],stationContactD75,balancedContactD79)){ \
                s.allowedGoal[side]=goals[side];s.allowedReady[side]=true;s.handBlocked[side]=stagedContact[side].blocked; \
            }else{s.allowedReady[side]=false;s.handBlocked[side]=false;goals[side]=requested;} \
        } \
        for(int orderH25=0;orderH25<2;++orderH25){ \
            const int side=(firstContactSideH24+orderH25)&1; \
            if(!pendingContactH25[side].ready)continue; \
            kcd2vr::diagnostics::passive169r1::RayScope rayScopeH25(contactSourceH25[side]); \
            if(CompleteHandContact6695(stagedContact[side],pendingContactH25[side],goals[side])){ \
                s.allowedGoal[side]=goals[side];s.allowedReady[side]=true;s.handBlocked[side]=stagedContact[side].blocked; \
            }else{s.allowedReady[side]=false;s.handBlocked[side]=false;goals[side]=pendingContactH25[side].requested;} \
        } \
        CollisionStatus6692(ts.tick,"WEAPON_FIRST_SHARED_BUDGET"); \
    }
