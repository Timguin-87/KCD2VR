// Generated from the actual native consumer prefix; no native leaf substituted.
static int ActualEquipmentPrefixH36(const PhysFrame6698& frame,const PhysEquipment6698& e,std::uint64_t now) {
    std::lock_guard<std::recursive_mutex> lock(g_phys_native_mutex6698);
    auto& n=g_phys_native6698;const auto nativeFrame=++n.dispatchFrame;
    ph::Command c{};int budget=8;
    while(budget--&&g_phys_commands6698.peek(c)&&c.sample<=frame.input.sample){
        // A primary release cancels this item's older attack/string commands.
        // In particular a simultaneous string release must not fire a bow first.
        if(c.side>=0&&c.side<2&&c.kind!=ph::CommandKind::DropT1&&
           g_weapon_release_pending_t1[c.side].load(std::memory_order_acquire)==c.key){
            TraceHitCommandC54R3(c,"COMMAND_DISCARD","ITEM_DROP_OWNS_THIS_KEY",now);
            g_phys_commands6698.pop(c);continue;
        }
        // Let the original native frame observe prepare/begin before its release.
        // Do not drain both edges into one pre-frame callback, even after a delayed pose.
        if(PhysDeferRelease103(n,c,nativeFrame)&&!(kcd2vr::motion59c4::selected.load()&&!c.kick59&&
            (c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust||c.kind==ph::CommandKind::Pommel))){
            TraceHitCommandC54R3(c,"COMMAND_WAIT","PREPARE_AND_RELEASE_REQUIRE_DISTINCT_NATIVE_FRAMES",now);break;}
        if(g_phys_fault6698.load())break;
        // D82_FINAL_CONTACT_QUEUE_BEGIN: c is a local copy; the ring keeps the
        // original occurrence. Ordinary command expiry below is unchanged.
        const auto finalContactD82=c.epoch==e.request&&c.bodyEpoch==e.epoch&&c.side==0?
            NativeFinalContactCommandD82(c,frame,now):kcd2vr::final_combo_d72::QueueD82::Ordinary;
        if(finalContactD82==kcd2vr::final_combo_d72::QueueD82::Wait){
            TraceHitCommandC54R3(c,"COMMAND_WAIT","D82_FINAL_LIVE_SLOT_AWAITING_OWN_CONTACT",now);break;
        }
        if(finalContactD82==kcd2vr::final_combo_d72::QueueD82::Refused){
            TraceHitCommandC54R3(c,"COMMAND_DISCARD","D82_FINAL_CONTACT_OWNER_SLOT_OR_ONCE_ONLY",now);
            if(n.attackPrepared&&n.natural117&&n.prepareGestureC10R18==c.gesture59c3&&
               n.attackKey==c.key&&n.attackGeneration117==c.generation&&
               (c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust))PhysCancelAttack117();
            g_phys_commands6698.pop(c);continue;
        }
        // D82_FINAL_CONTACT_QUEUE_END
        if(c.epoch!=e.request||c.bodyEpoch!=e.epoch||now<c.tick||now-c.tick>100||c.side<0||c.side>1){
            TraceHitCommandC54R3(c,"COMMAND_DISCARD",c.epoch!=e.request?"REQUEST_CHANGED":c.bodyEpoch!=e.epoch?"POSE_EPOCH_CHANGED":now<c.tick?"FUTURE_COMMAND":now-c.tick>100?"COMMAND_EXPIRED_100MS":"INVALID_SIDE",now);
            if(n.attackPrepared&&n.natural117&&c.key==n.attackKey&&c.generation==n.attackGeneration117&&
               (c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust||c.kind==ph::CommandKind::Pommel))PhysCancelAttack117();
            if(PhysEquipmentIntentD48(c)&&PhysCancelUnenteredEquipmentH36(e,c))PhysRejectUnenteredEquipmentD48(c);
            g_phys_commands6698.pop(c);if(c.kind==ph::CommandKind::DropT1&&c.side>=0&&c.side<2)
                g_weapon_release_pending_t1[c.side].store(0,std::memory_order_release);continue;
        }
        // D48_DRAW_INTENT_CONSUMER_BEGIN
        // Release ends a queued, never-entered Draw. An entered H36 obligation
        // remains independent and continues its original completion observation.
        if(!PhysCurrentDrawIntentD48(frame.input,c)){
            if(PhysCancelUnenteredEquipmentH36(e,c))PhysRejectUnenteredEquipmentD48(c);
            g_phys_commands6698.pop(c);continue;
        }
        // D48_DRAW_INTENT_CONSUMER_END
        // T24: native-empty preview still owns this same hand.
        // Recheck queued equipment at its existing native consumer too.
        if((c.kind==ph::CommandKind::Holster||c.kind==ph::CommandKind::Draw)&&
           HandClaimBlockedT24(c.side,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),e.epoch,e.request)){PhysRejectUnenteredEquipmentD48(c);g_phys_commands6698.pop(c);continue;}
        if(c.side==0&&(c.kind==ph::CommandKind::Holster||c.kind==ph::CommandKind::Draw)&&
           (NativeHeldBusyT4(0)||g_native_held_release_inhibit_t4.load(std::memory_order_acquire))){PhysRejectUnenteredEquipmentD48(c);g_phys_commands6698.pop(c);continue;}
        // Draw/Holster reaches this consumer only after an actual ring enqueue.
        // Its producer already arbitrated Quick Talk before creating the request.
        // A later pointing pose cannot steal that admitted occurrence while the
        // native hand is still empty. Current item/epoch/deadline/native gates remain.
        kcd2vr::hand_only_door_h40::Admission doorDrawAdmissionH40;
        // CG1_DRAW_CONSUMER_BEGIN: Draw and its HS2 prerequisite stows share
        // one immutable both-hand occurrence, independent of current draw mode.
        const bool acquisitionCg1=c.kind==ph::CommandKind::Draw||
            (c.kind==ph::CommandKind::Holster&&c.corpseClaimCg1.mask);
        if(acquisitionCg1){
            if(c.corpseClaimCg1.mask!=3u){PhysRejectUnenteredEquipmentD48(c);g_phys_commands6698.pop(c);continue;}
            const unsigned doorPolicyCg1=c.kind==ph::CommandKind::Draw?(1u<<c.side):0u;
            if(kcd2vr::hand_only_door_h40::Reserved()&doorPolicyCg1)break;
            if(!doorDrawAdmissionH40.take(3u,true))break;
            if(kcd2vr::hand_only_door_h40::Reserved()&doorPolicyCg1)break;
            if(!kcd2vr::corpse_grip_cg1::claims.current(c.corpseClaimCg1)){
                PhysRejectUnenteredEquipmentD48(c);g_phys_commands6698.pop(c);continue; // permanently obsolete; no re-stamp/retry
            }
        }
        // CG1_DRAW_CONSUMER_END
        const auto equipmentFenceH36=PhysLeftEquipmentH36(frame,e,c);
        if(equipmentFenceH36==HolsterNativeH36::Wait)break;
        if(equipmentFenceH36==HolsterNativeH36::Discard){if(PhysCancelUnenteredEquipmentH36(e,c))PhysRejectUnenteredEquipmentD48(c);g_phys_commands6698.pop(c);continue;}
    return 1; // next production instruction is native_equipment_commands.inl
    }
    return 0;
}
