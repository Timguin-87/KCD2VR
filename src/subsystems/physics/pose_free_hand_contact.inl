    // V168 F10 collision rung: retained bounded self-contact approximation.
    // World sweeps run afterwards, so a torso correction cannot skip a wall test.
    // Held weapons, authored interactions and native protected actions keep ownership.
    unsigned selfMask164[2]{};double selfCorrection164[2]{};bool selfResolved164[2]{true,true};
    const bool selfContact167=collisionMode; // V168: explicit F10 collision rung, independent of diagnostic F5
    // D56: UNKNOWN equipment is not proof of a new visible-hand owner. Keep
    // the current torso proxy for a hand with no other presentation claim.
    // World sweeps, finger contact, and interaction authority still require
    // the positively EMPTY freeContact164R1 owner below. No old plane is reused.
    bool selfPresentationD56[2]{};
    for(int side=0;side<2;++side)selfPresentationD56[side]=freeContact164R1[side]||
        (valid[side]&&contactYield164R1[side]==16u&&!otherOwnerH50[side]&&!doorPose66[side]&&!doorPoseH40[side]);
    if(collisionMode&&selfContact167&&!directHandsR135&&!nativeFingerCapture){
        const V3 original164[2]{goals[0],goals[1]};
        V3 palm164[2][6]{};bool geometry164[2]{};kcd2vr::hand_shape59c7f::Shape handShape59c7f[2]{};
        for(int side=0;side<2;++side)if(selfPresentationD56[side]){FingerContactGeometry103 geometry{};
            const auto orientation=DesiredHand6689(s,side,frame,visQH50[side]);
            geometry164[side]=preciseContactModeR24?
                PalmGeometry103(s,side,orientation,side?ts.left_grip:ts.right_grip,side?ts.left_trigger:ts.right_trigger,0,palm164[side],geometry,tx.fingerTrialH7[side]?tx.fingerOpenBiasH7[side]:0.f,tx.fingerTrialH7[side]):
                SimplePalmGeometryR24(s,side,orientation,palm164[side],&handShape59c7f[side]);
            if(geometry164[side]&&preciseContactModeR24)handShape59c7f[side]=kcd2vr::hand_shape59c7f::make(geometry.curve,geometry.input,PQ(geometry.orientation));}
        const double radius164=pc::torso_radius103(PV(gestureShouldersR38[0]),PV(gestureShouldersR38[1]));
        for(int side=0;side<2;++side)if(geometry164[side]){
            pc::Vec3 offsets[6]{};for(int i=0;i<6;++i)offsets[i]=PV(palm164[side][i]);
            kcd2vr::body_contact164::Capsule obstacles[2]{{PV(FromT(s.work[rig.pelvis])),PV(FromT(s.work[rig.spine3])),radius164},{}};
            unsigned n=1;
            // C44: the actual sampled finger/palm shapes below own hand-vs-hand contact.
            const auto r=kcd2vr::body_contact164::project(PV(original164[side]),offsets,preciseContactModeR24?6:5,obstacles,n,{side?-1.0:1.0,0,0});
            selfMask164[side]=r.contacts;selfCorrection164[side]=r.correction;selfResolved164[side]=r.valid;
            if(r.valid)goals[side]=VP(r.wrist);
        }
#include "pose_hand_pair_d39.inl" // M2_OWNED_SOURCE
    }
    if(physicalMode&&!nativeFingerCapture)(void)StageShieldPat54(tx);
    // C54: own sword vs exact empty hand. Physical weapon intent/trajectory are
    // untouched; the hand proceeds through the existing world and fixed-arm solve.
    if(collisionMode&&!directHandsR135&&!nativeFingerCapture&&tx.physicalActive){
        kcd2vr::grasp13::Acquisition54 acquisition{};
        const bool acquiring=g_graspAcquisition54&&g_graspAcquisition54(acquisition)&&
            kcd2vr::grasp13::freshAcquisition54(acquisition,contactNow164R1)&&
            acquisition.character==reinterpret_cast<std::uintptr_t>(s.character)&&acquisition.epoch==tx.epoch;
        for(int side=0;side<2;++side)if(freeContact164R1[side]&&s.handoverW178r14[side]<=.001f&&
            !(acquiring&&acquisition.side==side)){
            V3 palm[6]{};FingerContactGeometry103 geometry{};
            const auto orientation=DesiredHand6689(s,side,frame,visQH50[side]);
            kcd2vr::hand_shape59c7f::Shape shape{};
            if(preciseContactModeR24){
                if(!PalmGeometry103(s,side,orientation,side?ts.left_grip:ts.right_grip,side?ts.left_trigger:ts.right_trigger,0,palm,geometry,tx.fingerTrialH7[side]?tx.fingerOpenBiasH7[side]:0.f,tx.fingerTrialH7[side]))continue;
                shape=kcd2vr::hand_shape59c7f::make(geometry.curve,geometry.input,PQ(geometry.orientation));
            }else if(!SimplePalmGeometryR24(s,side,orientation,palm,&shape))continue;
            (void)StageOwnBladeContact54(tx,side,freeContact164R1[side],shape,orientation,goals[side]);
        }
    }
    // V66.124: the held weapon owns its hands' collision constraint. It must get
    // first use of the existing shared 18-ray/1ms budget; two finger passes used
    // to exhaust that budget before the first blade ray. Constrain free hands
    // AFTER reach retargeting too, so profile mapping cannot undo a wall result.
    // All public hand modes query current world geometry. Balanced uses seven
    // stable features without collision-driven curls or optional recovery rays.
    if(collisionMode&&!directHandsR135&&!perfContactModeDs2){
        HandContactPending6695 pendingContactH25[2]{};
        kcd2vr::passive169r1::Sample contactSourceH25[2]{};
        V3 contactPalmH25[2][7]{};FingerContactGeometry103 contactGeometryH25[2]{};
        // A constrained shared budget must not give the same hand every full
        // pass. Prefer the older completed query. Treat future stamps as never
        // completed after a clock reset. Equal stamps retain right-first.
        const std::uint64_t completedContactH24[2]{
            stagedContact[0].lastCompleteTickH18<=ts.tick?stagedContact[0].lastCompleteTickH18:0,
            stagedContact[1].lastCompleteTickH18<=ts.tick?stagedContact[1].lastCompleteTickH18:0};
        const int firstContactSideH24=completedContactH24[1]<completedContactH24[0]?1:0;
        // D81: complete both hands' mandatory coverage before any optional
        // lost-feature recovery. The older completed hand still goes first.
        // Recovery keeps already measured current planes if the remaining
        // shared budget expires; it cannot starve the opposite mandatory set.
        for(int orderH24=0;orderH24<2;++orderH24){
            const int side=(firstContactSideH24+orderH24)&1;if(!valid[side])continue;
            if(!freeContact164R1[side]){
                if(contactYield164R1[side]==16u&&!otherOwnerH50[side])ResetHandGeometryD56(stagedContact[side]);
                else ResetHandContact6695(stagedContact[side]);
                s.allowedReady[side]=false;
                s.handBlocked[side]=s.armBlocked[side]=s.armSaturated[side]=false;
                continue;
            }
            const V3 requested=goals[side];const int hand=side?rig.hand_l:rig.hand_r;
            auto& palm=contactPalmH25[side];auto& fingerGeometry=contactGeometryH25[side];
            const auto orientation=DesiredHand6689(s,side,frame,visQH50[side]);
            const bool havePalm=ContactPalmGeometryD79(s,side,orientation,
                side?ts.left_grip:ts.right_grip,side?ts.left_trigger:ts.right_trigger,heldFingerKey[side],
                palm,fingerGeometry,preciseContactModeR24,balancedContactD79,
                tx.fingerTrialH7[side]?tx.fingerOpenBiasH7[side]:0.f,tx.fingerTrialH7[side]);
            fingerGeometry.springSourceH31={reinterpret_cast<std::uintptr_t>(character),tx.epoch,tx.epoch,
                contactControl,ts.sequence,ts.tick,tx.fingerSpringH31};
            auto& handRay169r1=contactSourceH25[side];handRay169r1.source=kcd2vr::passive169r1::Source::HandWorld;
            handRay169r1.side=side;handRay169r1.actor=reinterpret_cast<std::uintptr_t>(character);handRay169r1.key=0x48414E4400000000ull+side;
            handRay169r1.epoch=tx.epoch;handRay169r1.request=g_contact_control_epoch6695.load();handRay169r1.sequence=ts.sequence;handRay169r1.tick=ts.tick;
            kcd2vr::diagnostics::passive169r1::RayScope rayScope169r1(handRay169r1);
            if(havePalm&&ContactHand6695(stagedContact[side],character,tx.epoch,FromT(s.original[hand]),requested,goals[side],palm,preciseContactModeR24?&fingerGeometry:nullptr,&pendingContactH25[side],stationContactD75,balancedContactD79)){
                s.allowedGoal[side]=goals[side];s.allowedReady[side]=true;s.handBlocked[side]=stagedContact[side].blocked;
            }else{s.allowedReady[side]=false;s.handBlocked[side]=false;goals[side]=requested;}
        }
        // Each ready record has all of its current mandatory sweeps. No rays
        // are retried and no credits or query-work time are replenished here.
        for(int orderH25=0;orderH25<2;++orderH25){
            const int side=(firstContactSideH24+orderH25)&1;
            if(!pendingContactH25[side].ready)continue;
            kcd2vr::diagnostics::passive169r1::RayScope rayScopeH25(contactSourceH25[side]);
            if(CompleteHandContact6695(stagedContact[side],pendingContactH25[side],goals[side])){
                s.allowedGoal[side]=goals[side];s.allowedReady[side]=true;s.handBlocked[side]=stagedContact[side].blocked;
            }else{s.allowedReady[side]=false;s.handBlocked[side]=false;goals[side]=pendingContactH25[side].requested;}
        }
        CollisionStatus6692(ts.tick,"WEAPON_FIRST_SHARED_BUDGET");
    }
