// Generated actual native leaf and consumer prefix; ONLY native primitive leaves have explicit host providers.
static bool ActualEquipmentCommandH36(const PhysEquipment6698& e,const ph::Command& c,bool* enteredD48=nullptr) noexcept {
    if(enteredD48)*enteredD48=false;
    // T2 explicitly drawn stock is not a normal equipped loadout slot. Its
    // observed hand/attachment lease permits only the existing per-WUID native
    // stow request; acceptance remains the subsequent real empty-hand witness.
    if(c.kind==ph::CommandKind::Holster&&c.side==0&&c.set<0&&c.bodyEpoch==e.epoch&&c.epoch==e.request){
        for(int n=0;n<e.count;++n)if(e.items[n].profile.key==c.key&&e.items[n].profile.generation==c.generation&&
            PhysSupplyHeldT2(e,e.items[n],0)&&retail_build112::Ready(WhBase())){
            if(!PhysEquipmentEntries62())return false;
            __try{if(enteredD48)*enteredD48=true;return HostStowH36(e.manager,c.key);}
            __except(EXCEPTION_EXECUTE_HANDLER){return false;}
        }
        return false;
    }
    if(c.set<0||c.set>3||!PhysVft6698(e.manager,0x3EC9958))return false;
    bool exact=false;for(int i=0;i<e.count;++i)if(e.items[i].profile.key==c.key&&e.items[i].profile.generation==c.generation)exact=true;
    if(!exact||c.bodyEpoch!=e.epoch||c.epoch!=e.request)return false;
    if(!PhysEquipmentSlot126(e,c)){
        diagnostic_trace::Write("V66.126 EQUIPMENT_REJECT reason=LIVE_LOADOUT_SLOT keyHandMapping=SEPARATE");return false;
    }
    const bool comparison=FullWeaponMode120(g_weapon_control_mode.load());
    if(comparison){
        if(!PhysEquipmentEntries62())return false;
    }
    __try {
        if(comparison){
            // Per-item native requests. Never sheath an independently gripped
            // offhand item, or fabricate success from a wrapper's return value.
            if(c.kind==ph::CommandKind::Holster){
                if(e.handKey[c.side]!=c.key)return false;
                if(!PhysLeftEquipmentInvokeH36(e,c,[&]{if(enteredD48)*enteredD48=true;return HostStowH36(e.manager,c.key);})){
                    diagnostic_trace::Write("V66.116 HOLSTER nativeReturned=0 attachmentChange=NOT_ACKNOWLEDGED");return false;
                }
            }else if(c.kind==ph::CommandKind::Draw){
                if(e.handKey[c.side])return false;
                if(c.set!=3&&e.selected!=c.set){
                    if(e.handKey[0]||e.handKey[1])return false;
                    // A normal selector return closes its synchronous work;
                    // only exact readback permits this SAME command's draw.
                    if(enteredD48)*enteredD48=true;
                    const bool selected=PhysSelectEquipmentInvokeD48(e,c,[&]{
                        HostSelectH36(e.manager,c.set);});
                    if(enteredD48)*enteredD48=false;if(!selected)return false;
                }
                if(!PhysLeftEquipmentInvokeH36(e,c,[&]{if(enteredD48)*enteredD48=true;
                    auto originalD77=[&](){HostDrawH36(e.manager,c.key,2);return true;};
                    return PhysObserveDrawInvokeD77(e,c,originalD77);}))return false;
            }else return false;
            return true; // request returned; only subsequent hand keys acknowledge it
        }
        if(c.set==3)return false;
        if(c.kind==ph::CommandKind::Draw){if(e.handKey[0]||e.handKey[1])return false;
            if(enteredD48)*enteredD48=true;
            const bool selected=PhysSelectEquipmentInvokeD48(e,c,[&]{
                HostSelectH36(e.manager,c.set);});
            if(enteredD48)*enteredD48=false;if(!selected)return false;
            if(!PhysLeftEquipmentBeginH36(e,c)||!PhysLeftEquipmentEnterH36(e,c))return false;
            if(enteredD48)*enteredD48=true;auto originalD77=[&](){HostLegacyDrawH36(e.manager);return true;};(void)PhysObserveDrawInvokeD77(e,c,originalD77);
        }else{if(e.selected!=c.set||e.handKey[c.side]!=c.key||!PhysLeftEquipmentBeginH36(e,c)||!PhysLeftEquipmentEnterH36(e,c))return false;
            if(enteredD48)*enteredD48=true;HostLegacyStowH36(e.manager); }
        return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){PhysFault6698("V66.98 PHYSICAL FAULT draw/holster; no repeated request");return false;}
}

static void ActualCompletionDrainH36(const PhysFrame6698& frame,const PhysEquipment6698& e,std::uint64_t now){
 std::lock_guard<std::recursive_mutex> lock(g_phys_native_mutex6698);auto& n=g_phys_native6698;const auto nativeFrame=++n.dispatchFrame;
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
        if(c.kind==ph::CommandKind::DropT1){
            // The original command stays queued through exact collider
            // retirement; the existing80/100ms history/command limits still
            // apply. Never obtain a fresh release from a late stale velocity.
            static heldT4::Owner retiringOwnerH21{};static std::uint64_t releaseSampleH21=0,releaseEpochH21=0;
            const auto testEpochH21=kcd2vr::collab_h21::throwEpoch.load(std::memory_order_acquire);
            if(!c.throwEpochH21||c.throwEpochH21!=testEpochH21||c.physicsEpochH21!=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire)||!kcd2vr::weapon_release_t1::TestEnabledH21()){
                g_phys_commands6698.pop(c);g_weapon_release_pending_t1[c.side].store(0,std::memory_order_release);continue;
            }
            if(c.side==0&&kcd2vr::weapon_release_t1::TestEnabledH21()&&NativeHeldBusyT4(0)){
                heldT4::Owner ownerH21{};
                if(heldT4::ReadRetainedOwner(ownerH21)&&ownerH21.key==c.key&&ownerH21.generation==c.generation&&
                   ownerH21.request==c.epoch&&ownerH21.epoch==c.bodyEpoch&&ownerH21.side==0){
                    if(releaseSampleH21!=c.sample||releaseEpochH21!=testEpochH21){
                        retiringOwnerH21=ownerH21;releaseSampleH21=c.sample;releaseEpochH21=testEpochH21;NativeHeldInvalidateT4();
                    }
                    break;
                }
            }
            bool retiredReleaseH21=false;
            if(c.side==0&&releaseSampleH21==c.sample&&releaseEpochH21==testEpochH21){
                heldT4::Achieved retiredH21{};
                retiredReleaseH21=heldT4::Read(retiredH21)&&retiredH21.owner==retiringOwnerH21&&retiredH21.colliderRetired;
            }
            g_phys_commands6698.pop(c);PhysCancelNative6698();
            if(c.side==0&&(NativeHeldBusyT4(0)||(g_native_held_release_inhibit_t4.load(std::memory_order_acquire)&&!retiredReleaseH21))){
                g_weapon_release_pending_t1[0].store(0,std::memory_order_release);continue;}
            namespace w=kcd2vr::weapon_release_t1;w::Request release{};w::Sample sample{};
            const int index=e.heldIndex[c.side];
            if(w::TestEnabledH21()&&w::Mode()==1&&index>=0&&index<e.count&&e.items[index].dropUnitT1&&e.handKey[c.side]==c.key&&
               e.items[index].profile.generation==c.generation&&frame.input.tracked[c.side]&&
               frame.input.intentTracked[c.side]&&std::isfinite(frame.input.grip[c.side])&&frame.input.grip[c.side]<.25f&&
               g_weapon_release_sample_t1[c.side].read(sample)){
                release.manager=reinterpret_cast<std::uintptr_t>(e.manager);release.player=reinterpret_cast<std::uintptr_t>(e.player);
                release.character=frame.correlation126[c.side].stamp.actor;release.key=c.key;release.generation=c.generation;
                release.bodyEpoch=c.bodyEpoch;release.requestEpoch=c.epoch;release.sample=c.sample;release.tick=c.tick;
                release.throwEpochH21=c.throwEpochH21;
                release.physicsEpochH21=c.physicsEpochH21;
                release.side=c.side;release.classId=e.items[index].profile.classId;release.entity=e.items[index].profile.entity;release.attachment=sample.attachment;
                if(w::matches(sample,release,now)){
                    sample.motion.velocity(now,true,release.linear,release.angular);release.motion=sample.motion.pair;
                    release.lastAllowed=sample.motion.newer.world;release.motionTick=sample.motion.newer.tick;
                    (void)kcd2vr::action_time_owner59c11::DropWeaponT1(release);
                }else if(PhysHolsterCaptureD48(now))diagnostic_trace::Write("T1 WEAPON_RELEASE REFUSED reason=STALE_OR_CHANGED_ALLOWED_OBJECT_HISTORY sheathFallback=NONE");
            }else if(PhysHolsterCaptureD48(now))diagnostic_trace::Write("T1 WEAPON_RELEASE REFUSED reason=CURRENT_PRIMARY_RELEASE_OR_HISTORY_UNAVAILABLE sheathFallback=NONE");
            g_weapon_release_pending_t1[c.side].store(0,std::memory_order_release);break;
        }
        if(c.kind==ph::CommandKind::Draw||c.kind==ph::CommandKind::Holster){
            g_phys_commands6698.pop(c);PhysCancelNative6698();bool enteredD48=false;
            const bool returned=ActualEquipmentCommandH36(e,c,&enteredD48);
            // D63: an entered native DRAW from a positively non-held item owns
            // extraction motion. A failed/unentered request and an already-held
            // weapon cannot disarm combat. The producer additionally requires a
            // current same-key held profile and a newer XR source before this
            // ticket is admitted; native return alone is not that ACK.
            if(c.kind==ph::CommandKind::Draw&&enteredD48&&e.handKnown[c.side]&&e.handKey[c.side]==0)
                kcd2vr::draw_signal_d63::publish({0,c.key,c.generation,c.bodyEpoch,c.epoch,c.sample,c.tick,c.side,c.drawSourceD63});
            if(!returned&&!enteredD48&&PhysCancelUnenteredEquipmentH36(e,c))PhysRejectUnenteredEquipmentD48(c);
            // Refresh only the native facts changed by this draw. The native
            // return is not a held-item ACK and never grants a physical pose.
            if(returned&&c.kind==ph::CommandKind::Draw){
                const auto publishDrawFacts=[&](){
                    const auto currentOwner=[&](){return g_weapon_control_mode.load()>=4&&
                        !animation_filter::nativeBodyOwned112.load()&&!NativePassengerCurrentA34(GetTickCount64())&&
                        !g_phys_fault6698.load()&&g_gameplay_allowed.load()&&!diagnostics::lab::NativeReference()&&
                        e.manager==g_henry_item_attach_manager.load()&&e.request==g_weapon_request_epoch6694.load()&&
                        e.epoch==g_pose_epoch6689.load()&&TrackingHeadUsable111(GetTickCount64());};
                    if(!currentOwner())return;
                    const auto membership=g_left_native_membership_h36.load(std::memory_order_acquire);
                    const auto refreshedAt=std::uint64_t(GetTickCount64());PhysEquipment6698 refreshed{};
                    if(!PhysRefreshEquipment103(refreshed,refreshedAt)||!refreshed.valid||
                       refreshed.manager!=e.manager||refreshed.player!=e.player||
                       refreshed.request!=e.request||refreshed.epoch!=e.epoch||!currentOwner()||
                       membership!=g_left_native_membership_h36.load(std::memory_order_acquire))return;
                    refreshed.leftNativeMembershipH36=membership;
                    g_phys_equipment6698.publish(refreshed);
                    for(int i=0;i<8;++i)g_phys_relevant_id101[i].store(i<refreshed.count?refreshed.items[i].profile.entity:0,std::memory_order_relaxed);
                };
                publishDrawFacts();
            }
            if(PhysHolsterCaptureD48(now)){char msg[256]{};std::snprintf(msg,sizeof(msg),"V66.121 EQUIPMENT_REQUEST kind=%s side=%d set=%d key=%016llX returned=%d acknowledged=NOT_YET",
                c.kind==ph::CommandKind::Draw?"DRAW":"HOLSTER",c.side,c.set,(unsigned long long)c.key,returned);diagnostic_trace::Write(msg);}break;}

 return; // remaining attack-only consumer is outside this equipment fixture
 }
}
